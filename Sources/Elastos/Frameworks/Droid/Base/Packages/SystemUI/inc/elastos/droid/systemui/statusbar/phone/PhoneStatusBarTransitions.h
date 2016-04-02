
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BOUNCELNTERPOLATOR_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BOUNCELNTERPOLATOR_H__

#include "_SystemUI.h"
#include "elastos/droid/systemui/statusbar/phone/BarTransitions.h"
#include "Elastos.Droid.Animation.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IObjectAnimator;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class PhoneStatusBarTransitions
    : public BarTransitions
    , public IPhoneStatusBarTransitions
{
public:
    CAR_INTERFACE_DECL();

    PhoneStatusBarTransitions(
        /* [in] */ IPhoneStatusBarView* view);

    CARAPI Init();

    CARAPI AnimateTransitionTo(
        /* [in] */ IView* v,
        /* [in] */ Float toAlpha,
        /* [out] */ IObjectAnimator** result);

protected:
    // @Override
    CARAPI_(void) OnTransition(
        /* [in] */ Int32 oldMode,
        /* [in] */ Int32 newMode,
        /* [in] */ Boolean animate);

private:
    CARAPI_(Float) GetNonBatteryClockAlphaFor(
        /* [in] */ Int32 mode);

    CARAPI_(Float) GetBatteryClockAlpha(
        /* [in] */ Int32 mode);

    CARAPI_(Boolean) IsOpaque(
        /* [in] */ Int32 mode);

    CARAPI_(void) ApplyMode(
        /* [in] */ Int32 mode,
        /* [in] */ Boolean animate);

private:
    static const Float ICON_ALPHA_WHEN_NOT_OPAQUE;
    static const Float ICON_ALPHA_WHEN_LIGHTS_OUT_BATTERY_CLOCK;
    static const Float ICON_ALPHA_WHEN_LIGHTS_OUT_NON_BATTERY_CLOCK;

    AutoPtr<IPhoneStatusBarView> mView;
    Float mIconAlphaWhenOpaque;

    AutoPtr<IView> mLeftSide;
    AutoPtr<IView> mStatusIcons;
    AutoPtr<IView> mSignalCluster;
    AutoPtr<IView> mBattery;
    AutoPtr<IView> mClock;
    AutoPtr<IAnimator> mCurrentAnimation;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_BOUNCELNTERPOLATOR_H__
