#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CDOWNLOADDRMHELPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CDOWNLOADDRMHELPER_H__

#include "_Elastos_Droid_Google_Mms_Utility_CDownloadDrmHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

CarClass(CDownloadDrmHelper)
    , public Singleton
    , public IDownloadDrmHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI IsDrmMimeType(
        /* [in] */ IContext* context,
        /* [in] */ const String& mimetype,
        /* [out] */ Boolean* result);

    CARAPI IsDrmConvertNeeded(
        /* [in] */ const String& mimetype,
        /* [out] */ Boolean* result);

    CARAPI ModifyDrmFwLockFileExtension(
        /* [in] */ const String& filename,
        /* [out] */ String* result);

    CARAPI GetOriginalMimeType(
        /* [in] */ IContext* context,
        /* [in] */ const String& path,
        /* [in] */ const String& containingMime,
        /* [out] */ String* result);
};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CDOWNLOADDRMHELPER_H__
