
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
    // {
    //     return MapFileAtAddress(NULL, byte_count, prot, flags, fd, start, FALSE, filename, error_msg);
    // }

    // Map part of a file, taking care of non-page aligned offsets.  The
    // "start" offset is absolute, not relative. This version allows
    // requesting a specific address for the base of the
    // mapping. "reuse" allows us to create a view into an existing
    // mapping where we do not take ownership of the memory.
    //
    // On success, returns returns a MemMap instance.  On failure, returns a
    // nullptr;
    static CARAPI_(AutoPtr<MemMap>) MapFileAtAddress(
        /* [in] */ byte* addr,
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
};

} // namespace Dex
} // namespace Pm
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_PM_DEX_MEMMAP_H__
