
#ifndef __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORAUTOCLOSEOUTPUTSTREAM_H__
#define __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORAUTOCLOSEOUTPUTSTREAM_H__

#include "_Elastos_Droid_Os_CParcelFileDescriptorAutoCloseOutputStream.h"
#include "elastos/droid/os/ParcelFileDescriptor.h"

using Elastos::IO::IFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * An OutputStream you can create on a ParcelFileDescriptor, which will
 * take care of calling {@link ParcelFileDescriptor#close
 * ParcelFileDescriptor.close()} for you when the stream is closed.
 */
CarClass(CParcelFileDescriptorAutoCloseOutputStream)
    , public ParcelFileDescriptor::AutoCloseOutputStream
{
public:
    CAR_OBJECT_DECL();

};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CPARCELFILEDESCRIPTORAUTOCLOSEOUTPUTSTREAM_H__
