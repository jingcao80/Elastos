#ifndef __ELASTOS_DROID_GOOGLE_MMS_CCONTENTTYPEHELPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_CCONTENTTYPEHELPER_H__

#include "_Elastos_Droid_Google_Mms_CContentTypeHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {

CarClass(CContentTypeHelper)
    , public Singleton
    , public IContentTypeHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI IsSupportedType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsSupportedImageType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsSupportedAudioType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsSupportedVideoType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsTextType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsImageType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsAudioType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsVideoType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsDrmType(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);

    CARAPI IsUnspecified(
        /* [in] */ const String& contentType,
        /* [out] */ Boolean* result);
};

} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_CCONTENTTYPEHELPER_H__
