
#include "elastos/droid/server/pm/dex/MemMap.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

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

//AutoPtr<MemMap> MemMap::MapFileAtAddress(
//    /* [in] */ Byte* expected_ptr,
//    /* [in] */ size_t byte_count,
//    /* [in] */ int prot,
//    /* [in] */ int flags,
//    /* [in] */ int fd,
//    /* [in] */ off_t start,
//    /* [in] */ Boolean reuse,
//    /* [in] */ const char* filename,
//    /* [out] */ String* error_msg)
//{
//    assert(prot != 0);
//    assert((flags & (MAP_SHARED | MAP_PRIVATE)) != 0);
//
//    // Note that we do not allow MAP_FIXED unless reuse == true, i.e we
//    // expect his mapping to be contained within an existing map.
//    if (reuse) {
//        // reuse means it is okay that it overlaps an existing page mapping.
//        // Only use this if you actually made the page reservation yourself.
//        assert(expected_ptr != NULL);
//
//#if !defined(__APPLE__)  // TODO: Reanable after b/16861075 BacktraceMap issue is addressed.
//        uintptr_t expected = reinterpret_cast<uintptr_t>(expected_ptr);
//        uintptr_t limit = expected + byte_count;
//        DCHECK(ContainedWithinExistingMap(expected, limit, error_msg));
//#endif
//        flags |= MAP_FIXED;
//    }
//    else {
//        assert((flags & MAP_FIXED) == 0);
//        // Don't bother checking for an overlapping region here. We'll
//        // check this if required after the fact inside CheckMapRequest.
//    }
//
//  if (byte_count == 0) {
//    return new MemMap(filename, nullptr, 0, nullptr, 0, prot, false);
//  }
//  // Adjust 'offset' to be page-aligned as required by mmap.
//  int page_offset = start % kPageSize;
//  off_t page_aligned_offset = start - page_offset;
//  // Adjust 'byte_count' to be page-aligned as we will map this anyway.
//  size_t page_aligned_byte_count = RoundUp(byte_count + page_offset, kPageSize);
//  // The 'expected_ptr' is modified (if specified, ie non-null) to be page aligned to the file but
//  // not necessarily to virtual memory. mmap will page align 'expected' for us.
//  byte* page_aligned_expected = (expected_ptr == nullptr) ? nullptr : (expected_ptr - page_offset);
//
//  byte* actual = reinterpret_cast<byte*>(mmap(page_aligned_expected,
//                                              page_aligned_byte_count,
//                                              prot,
//                                              flags,
//                                              fd,
//                                              page_aligned_offset));
//  if (actual == MAP_FAILED) {
//    auto saved_errno = errno;
//
//    std::string maps;
//    ReadFileToString("/proc/self/maps", &maps);
//
//    *error_msg = StringPrintf("mmap(%p, %zd, 0x%x, 0x%x, %d, %" PRId64
//                              ") of file '%s' failed: %s\n%s",
//                              page_aligned_expected, page_aligned_byte_count, prot, flags, fd,
//                              static_cast<int64_t>(page_aligned_offset), filename,
//                              strerror(saved_errno), maps.c_str());
//    return nullptr;
//  }
//  std::ostringstream check_map_request_error_msg;
//  if (!CheckMapRequest(expected_ptr, actual, page_aligned_byte_count, error_msg)) {
//    return nullptr;
//  }
//  return new MemMap(filename, actual + page_offset, byte_count, actual, page_aligned_byte_count,
//                    prot, reuse);
//}

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos
