
#ifndef __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORAUTOCLOSEINPUTSTREAM_H__
#define __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORAUTOCLOSEINPUTSTREAM_H__

#include "_Elastos_Droid_Os_CParcelFileDescriptorAutoCloseInputStream.h"
#include "elastos/droid/os/ParcelFileDescriptor.h"

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * An InputStream you can create on a ParcelFileDescriptor, which will
 * take care of calling {@link ParcelFileDescriptor#close
 * ParcelFileDescriptor.close()} for you when the stream is closed.
 */
CarClass(CParcelFileDescriptorAutoCloseInputStream)
    , public ParcelFileDescriptor::AutoCloseInputStream
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORAUTOCLOSEINPUTSTREAM_H__
