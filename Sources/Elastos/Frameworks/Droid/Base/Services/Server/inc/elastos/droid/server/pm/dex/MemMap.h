
#ifndef __ELASTOS_DROID_SERVER_PM_DEX_MEMMAP_H__
#define __ELASTOS_DROID_SERVER_PM_DEX_MEMMAP_H__

#include <elastos/core/Object.h>
#include <sys/mman.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Pm {
namespace Dex {

class MemMap : public Object
{
public:
    // Map part of a file, taking care of non-page aligned offsets.  The
    // "start" offset is absolute, not relative.
    //
    // On success, returns returns a MemMap instance.  On failure, returns a NULL;
    static CARAPI_(AutoPtr<MemMap>) MapFile(
        /* [in] */ size_t byte_count,
        /* [in] */ int prot,
        /* [in] */ int flags,
        /* [in] */ int fd,
        /* [in] */ off_t start,
        /* [in] */ const char* filename,
        /* [out] */ String* error_msg);

    // Map part of a file, taking care of non-page aligned offsets.  The
    // "start" offset is absolute, not relative. This version allows
    // requesting a specific address for the base of the
    // mapping. "reuse" allows us to create a view into an existing
    // mapping where we do not take ownership of the memory.
    //
    // On success, returns returns a MemMap instance.  On failure, returns a
    // nullptr;
    static CARAPI_(AutoPtr<MemMap>) MapFileAtAddress(
        /* [in] */ Byte* addr,
        /* [in] */ size_t byte_count,
        /* [in] */ int prot,
        /* [in] */ int flags,
        /* [in] */ int fd,
        /* [in] */ off_t start,
        /* [in] */ Boolean reuse,
        /* [in] */ const char* filename,
        /* [out] */ String* error_msg);

    CARAPI_(Byte*) Begin();

    CARAPI_(size_t) Size();

private:
    MemMap(
        /* [in] */ const String& name,
        /* [in] */ Byte* begin,
        /* [in] */ size_t size,
        /* [in] */ void* base_begin,
        /* [in] */ size_t base_size,
        /* [in] */ int prot,
        /* [in] */ Boolean reuse);

private:
    const String mName;
    Byte* const mBegin;  // Start of data.
    size_t mSize;  // Length of data.

    void* const mBaseBegin;  // Page-aligned base address.
    size_t mBaseSize;  // Length of mapping. May be changed by RemapAtEnd (ie Zygote).
    int mProt;  // Protection of the map.

    // When reuse_ is true, this is just a view of an existing mapping
    // and we do not take ownership and are not responsible for
    // unmapping.
    const Boolean mReuse;
};

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_DEX_MEMMAP_H__
