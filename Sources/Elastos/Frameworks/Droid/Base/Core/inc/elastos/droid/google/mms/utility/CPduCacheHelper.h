#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CPDUCACHEHELPER_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CPDUCACHEHELPER_H__

#include "_Elastos_Droid_Google_Mms_Utility_CPduCacheHelper.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

CarClass(CPduCacheHelper)
    , public Singleton
    , public IPduCacheHelper
{
public:
    CAR_INTERFACE_DECL()
    CAR_SINGLETON_DECL()

    CARAPI GetInstance(
        /* [out] */ IPduCache** result);

};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_CPDUCACHEHELPER_H__
