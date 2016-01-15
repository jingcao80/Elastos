
#ifndef __ELASTOS_DROID_OS_CPOWERMANAGERHELPER_H__
#define __ELASTOS_DROID_OS_CPOWERMANAGERHELPER_H__

#include "_Elastos_Droid_Os_CPowerManagerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CPowerManagerHelper)
    , public Singleton
    , public IPowerManagerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Returns true if the twilight service should be used to adjust screen brightness
     * policy.  This setting is experimental and disabled by default.
     * @hide
     */
    CARAPI UseTwilightAdjustmentFeature(
        /* [out] */ Boolean* result);

    CARAPI ValidateWakeLockParameters(
        /* [in] */ Int32 levelAndFlags,
        /* [in] */ const String& tag);
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OS_CPOWERMANAGERHELPER_H__
