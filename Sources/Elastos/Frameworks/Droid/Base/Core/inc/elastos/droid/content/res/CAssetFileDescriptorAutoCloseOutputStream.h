
#ifndef __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTORAUTOCLOSEOUTPUTSTREAM_H__
#define __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTORAUTOCLOSEOUTPUTSTREAM_H__

#include "_Elastos_Droid_Content_Res_CAssetFileDescriptorAutoCloseOutputStream.h"
#include "elastos/droid/os/ParcelFileDescriptor.h"

using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Os::ParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 * An OutputStream you can create on a ParcelFileDescriptor, which will
 * take care of calling {@link ParcelFileDescriptor#close
 * ParcelFileDescritor.close()} for you when the stream is closed.
 */
CarClass(CAssetFileDescriptorAutoCloseOutputStream)
    , public ParcelFileDescriptor::AutoCloseOutputStream
{
public:
    TO_STRING_IMPL("CAssetFileDescriptorAutoCloseOutputStream");

    CAR_OBJECT_DECL()

    CAssetFileDescriptorAutoCloseOutputStream();

    CARAPI constructor(
        /* [in] */ IAssetFileDescriptor* fd);

    CARAPI Write(
        /* [in] */ Int32 oneByte);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer);

    CARAPI Write(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count);

private:
    Int64 mRemaining;
};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTORAUTOCLOSEOUTPUTSTREAM_H__
