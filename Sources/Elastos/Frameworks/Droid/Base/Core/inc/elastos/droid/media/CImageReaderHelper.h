
#ifndef __ELASTOS_DROID_MEDIA_CIMAGEREADER_HELPER_H__
#define __ELASTOS_DROID_MEDIA_CIMAGEREADER_HELPER_H__

#include "_Elastos_Droid_Media_CImageReaderHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CImageReaderHelper)
    , public Singleton
    , public IImageReaderHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewInstance(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height,
        /* [in] */ Int32 format,
        /* [in] */ Int32 maxImages,
        /* [out] */ IImageReader** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CIMAGEREADER_HELPER_H__
