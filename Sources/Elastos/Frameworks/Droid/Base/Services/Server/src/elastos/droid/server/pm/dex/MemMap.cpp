
#include "elastos/droid/server/pm/dex/MemMap.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

extern Boolean ReadFileToString(const String& file_name, String* result);

static const int kPageSize = 4096;

// CheckMapRequest to validate a non-MAP_FAILED mmap result based on
// the expected value, calling munmap if validation fails, giving the
// reason in error_msg.
//
// If the expected_ptr is nullptr, nothing is checked beyond the fact
// that the actual_ptr is not MAP_FAILED. However, if expected_ptr is
// non-null, we check that pointer is the actual_ptr == expected_ptr,
// and if not, report in error_msg what the conflict mapping was if
// found, or a generic error in other cases.
static Boolean CheckMapRequest(Byte* expected_ptr, void* actual_ptr, size_t byte_count,
        String* error_msg)
{
    // Handled first by caller for more specific error messages.
    assert(actual_ptr != MAP_FAILED);

    if (expected_ptr == NULL) {
        return TRUE;
    }

    uintptr_t actual = reinterpret_cast<uintptr_t>(actual_ptr);
    uintptr_t expected = reinterpret_cast<uintptr_t>(expected_ptr);
//    uintptr_t limit = expected + byte_count;

    if (expected_ptr == actual_ptr) {
        return TRUE;
    }

    // We asked for an address but didn't get what we wanted, all paths below here should fail.
    int result = munmap(actual_ptr, byte_count);
    if (result == -1) {
        Slogger::D("MemMap", "munmap(%p, %zd) failed", actual_ptr, byte_count);
    }

    // We call this here so that we can try and generate a full error
    // message with the overlapping mapping. There's no guarantee that
    // that there will be an overlap though, since
    // - The kernel is not *required* to honour expected_ptr unless MAP_FIXED is
    //   true, even if there is no overlap
    // - There might have been an overlap at the point of mmap, but the
    //   overlapping region has since been unmapped.
//    std::string error_detail;
//    CheckNonOverlapping(expected, limit, &error_detail);

    String msg("");
    msg.AppendFormat("MemMap", "Failed to mmap at expected address, mapped at "
                        "0x%08p instead of 0x%08p",
                        actual, expected);
//    if (!error_detail.empty()) {
//        os << " : " << error_detail;
//    }

    *error_msg = msg;
    return FALSE;
}

AutoPtr<MemMap> MemMap::MapFile(
    /* [in] */ size_t byte_count,
    /* [in] */ int prot,
    /* [in] */ int flags,
    /* [in] */ int fd,
    /* [in] */ off_t start,
    /* [in] */ const char* filename,
    /* [out] */ String* error_msg)
{
    return MapFileAtAddress(NULL, byte_count, prot, flags, fd, start, FALSE, filename, error_msg);
}

Byte* MemMap::Begin()
{
    return mBegin;
}

size_t MemMap::Size()
{
    return mSize;
}

MemMap::MemMap(
    /* [in] */ const String& name,
    /* [in] */ Byte* begin,
    /* [in] */ size_t size,
    /* [in] */ void* base_begin,
    /* [in] */ size_t base_size,
    /* [in] */ int prot,
    /* [in] */ Boolean reuse)
    : mName(name)
    , mBegin(begin)
    , mSize(size)
    , mBaseBegin(base_begin)
    , mBaseSize(base_size)
    , mProt(prot)
    , mReuse(reuse)
{
    if (mSize == 0) {
        assert(mBegin == NULL);
        assert(mBaseBegin == NULL);
        assert(mBaseSize == 0U);
    }
    else {
        assert(mBegin != NULL);
        assert(mBaseBegin != NULL);
        assert(mBaseSize != 0U);

        // // Add it to maps_.
        // MutexLock mu(Thread::Current(), *Locks::mem_maps_lock_);
        // DCHECK(maps_ != nullptr);
        // maps_->insert(std::make_pair(base_begin_, this));
    }
};

AutoPtr<MemMap> MemMap::MapFileAtAddress(
    /* [in] */ Byte* expected_ptr,
    /* [in] */ size_t byte_count,
    /* [in] */ int prot,
    /* [in] */ int flags,
    /* [in] */ int fd,
    /* [in] */ off_t start,
    /* [in] */ Boolean reuse,
    /* [in] */ const char* filename,
    /* [out] */ String* error_msg)
{
    assert(prot != 0);
    assert((flags & (MAP_SHARED | MAP_PRIVATE)) != 0);

    // Note that we do not allow MAP_FIXED unless reuse == true, i.e we
    // expect his mapping to be contained within an existing map.
    if (reuse) {
        // reuse means it is okay that it overlaps an existing page mapping.
        // Only use this if you actually made the page reservation yourself.
        assert(expected_ptr != NULL);

        // TODO:
        assert(0);
// #if !defined(__APPLE__)  // TODO: Reanable after b/16861075 BacktraceMap issue is addressed.
//         uintptr_t expected = reinterpret_cast<uintptr_t>(expected_ptr);
//         uintptr_t limit = expected + byte_count;
//         DCHECK(ContainedWithinExistingMap(expected, limit, error_msg));
// #endif
        flags |= MAP_FIXED;
    }
    else {
        assert((flags & MAP_FIXED) == 0);
        // Don't bother checking for an overlapping region here. We'll
        // check this if required after the fact inside CheckMapRequest.
    }

    if (byte_count == 0) {
        return new MemMap(String(filename), NULL, 0, NULL, 0, prot, FALSE);
    }
    // Adjust 'offset' to be page-aligned as required by mmap.
    int page_offset = start % kPageSize;
    off_t page_aligned_offset = start - page_offset;
    // Adjust 'byte_count' to be page-aligned as we will map this anyway.
    size_t page_aligned_byte_count = RoundUp(byte_count + page_offset, kPageSize);
    // The 'expected_ptr' is modified (if specified, ie non-null) to be page aligned to the file but
    // not necessarily to virtual memory. mmap will page align 'expected' for us.
    byte* page_aligned_expected = (expected_ptr == NULL) ? NULL : (expected_ptr - page_offset);

    byte* actual = reinterpret_cast<byte*>(mmap(page_aligned_expected,
                                                page_aligned_byte_count,
                                                prot,
                                                flags,
                                                fd,
                                                page_aligned_offset));
    if (actual == MAP_FAILED) {
        int saved_errno = errno;

        String maps;
        ReadFileToString(String("/proc/self/maps"), &maps);

        String msg("");
        msg.AppendFormat("mmap(%p, %zd, 0x%x, 0x%x, %d, %lld) of file '%s' failed: %s\n%s",
                                page_aligned_expected, page_aligned_byte_count, prot, flags, fd,
                                static_cast<int64_t>(page_aligned_offset), filename,
                                strerror(saved_errno), maps.string());
        *error_msg = msg;
        return NULL;
    }
    if (!CheckMapRequest(expected_ptr, actual, page_aligned_byte_count, error_msg)) {
        return NULL;
    }
    return new MemMap(String(filename), actual + page_offset, byte_count, actual, page_aligned_byte_count,
                    prot, reuse);
}

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
