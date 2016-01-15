
#ifndef __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTORAUTOCLOSEINPUTSTREAM_H__
#define __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTORAUTOCLOSEINPUTSTREAM_H__

#include "_Elastos_Droid_Content_Res_CAssetFileDescriptorAutoCloseInputStream.h"
#include "elastos/droid/os/ParcelFileDescriptor.h"

using Elastos::IO::IFileDescriptor;
using Elastos::Droid::Os::ParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 * An InputStream you can create on a ParcelFileDescriptor, which will
 * take care of calling {@link ParcelFileDescriptor#close
 * ParcelFileDescritor.close()} for you when the stream is closed.
 */
CarClass(CAssetFileDescriptorAutoCloseInputStream)
    , public ParcelFileDescriptor::AutoCloseInputStream
{
public:
    CAR_OBJECT_DECL()

    CAssetFileDescriptorAutoCloseInputStream();

    CARAPI constructor(
        /* [in] */ IAssetFileDescriptor* fd);

    CARAPI Available(
        /* [out] */ Int32* able);

    CARAPI Read(
        /* [out] */ Int32* result);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ Int32* result);

    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [out] */ Int32* result);

    CARAPI Skip(
        /* [in] */ Int64 count,
        /* [out] */ Int64* result);

    CARAPI Mark(
        /* [in] */ Int32 readlimit);

    CARAPI IsMarkSupported(
        /* [out] */ Boolean* result);

    CARAPI Reset();

private:
    Int64 mRemaining;
};

}
}
}
}

#endif // __ELASTOS_DROID_CONTENT_RES_CASSETFILEDESCRIPTORAUTOCLOSEINPUTSTREAM_H__
