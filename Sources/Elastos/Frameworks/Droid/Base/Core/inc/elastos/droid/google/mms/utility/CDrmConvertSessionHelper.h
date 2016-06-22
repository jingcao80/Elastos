#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CDRMCONVERTSESSIONHELPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CDRMCONVERTSESSIONHELPER_H__

#include "_Elastos_Droid_Google_Mms_Utility_CDrmConvertSessionHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

CarClass(CDrmConvertSessionHelper)
    , public Singleton
    , public IDrmConvertSessionHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI Open(
        /* [in] */ IContext* context,
        /* [in] */ const String& mimeType,
        /* [out] */ IDrmConvertSession** result);

};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CDRMCONVERTSESSIONHELPER_H__
