
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_UNLOCKMETHODCACHE_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_UNLOCKMETHODCACHE_H__

#include "_SystemUI.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * Caches whether the current unlock method is insecure, taking trust into account. This information
 * might be a little bit out of date and should not be used for actual security decisions; it should
 * be only used for visual indications.
 */
class UnlockMethodCache: public Object
{
public:
    static CARAPI_(AutoPtr<UnlockMethodCache>) GetInstance(
        /* [in] */ IContext* context);

    /**
     * @return whether the current security method is secure, i. e. the bouncer will be shown
     */
    CARAPI_(Boolean) IsMethodInsecure();

    CARAPI_(void) AddListener(
        /* [in] */ IOnUnlockMethodChangedListener* listener);

    CARAPI_(void) RemoveListener(
        /* [in] */ IOnUnlockMethodChangedListener* listener);

    // private final KeyguardUpdateMonitorCallback mCallback = new KeyguardUpdateMonitorCallback() {
    //     @Override
    //     public void onUserSwitchComplete(int userId) {
    //         updateMethodSecure(FALSE /* updateAlways */);
    //     }

    //     @Override
    //     public void onTrustChanged(int userId) {
    //         updateMethodSecure(FALSE /* updateAlways */);
    //     }

    //     @Override
    //     public void onTrustManagedChanged(int userId) {
    //         updateMethodSecure(FALSE /* updateAlways */);
    //     }

    //     @Override
    //     public void onScreenTurnedOn() {
    //         updateMethodSecure(FALSE /* updateAlways */);
    //     }

    //     @Override
    //     public void onFingerprintRecognized(int userId) {
    //         updateMethodSecure(FALSE /* updateAlways */);
    //     }

    //     @Override
    //     public void onFaceUnlockStateChanged(Boolean running, int userId) {
    //         updateMethodSecure(FALSE /* updateAlways */);
    //     }
    // };

    CARAPI_(Boolean) IsTrustManaged();

    CARAPI_(Boolean) IsFaceUnlockRunning();

private:
    UnlockMethodCache(
        /* [in] */ IContext* ctx);

    CARAPI_(void) UpdateMethodSecure(
        /* [in] */ Boolean updateAlways);

    CARAPI_(void) NotifyListeners(
        /* [in] */ Boolean secure);

private:
    static AutoPtr<UnlockMethodCache> sInstance;

    AutoPtr<ILockPatternUtils> mLockPatternUtils;
    // AutoPtr<IKeyguardUpdateMonitor> mKeyguardUpdateMonitor;
    AutoPtr<IArrayList> mListeners;  /*<OnUnlockMethodChangedListener*/
    Boolean mMethodInsecure;
    Boolean mTrustManaged;
    Boolean mFaceUnlockRunning;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_UNLOCKMETHODCACHE_H__
