
#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDSTATUSBARVIEW_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDSTATUSBARVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CKeyguardStatusBarView.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/widget/RelativeLayout.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::SystemUI::StatusBar::Policy::IBatteryController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IBatteryStateChangeCallback;
using Elastos::Droid::SystemUI::StatusBar::Policy::IKeyguardUserSwitcher;
using Elastos::Droid::SystemUI::StatusBar::Policy::IUserInfoController;
using Elastos::Droid::SystemUI::StatusBar::Policy::IUserInfoControllerOnUserInfoChangedListener;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::View::Animation::IInterpolator;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::RelativeLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

/**
 * A view to show hints on Keyguard ("Swipe up to unlock", "Tap again to open").
 */
CarClass(CKeyguardStatusBarView)
    , public RelativeLayout
    , public IKeyguardStatusBarView
{
private:
    class BatteryStateChangeCallback
        : public Object
        , public IBatteryStateChangeCallback
    {
    public:
        CAR_INTERFACE_DECL()

        BatteryStateChangeCallback(
            /* [in] */ CKeyguardStatusBarView* host);

        // @Override
        CARAPI OnBatteryLevelChanged(
            /* [in] */ Int32 level,
            /* [in] */ Boolean pluggedIn,
            /* [in] */ Boolean charging);

        // @Override
        CARAPI OnPowerSaveChanged();
    private:
        CKeyguardStatusBarView* mHost;
    };

    class OnUserInfoChangedListener
        : public Object
        , public IUserInfoControllerOnUserInfoChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        OnUserInfoChangedListener(
            /* [in] */ CKeyguardStatusBarView* host);

        // @Override
        CARAPI OnUserInfoChanged(
            /* [in] */ const String& name,
            /* [in] */ IDrawable* picture);

    private:
        CKeyguardStatusBarView* mHost;
    };

    class OnPreDrawListener
        : public Object
        , public IOnPreDrawListener
    {
    private:
        class Runnable1: public Runnable
        {
        public:
            Runnable1(
                /* [in] */ CKeyguardStatusBarView* host);

            // @Override
            CARAPI Run();

        private:
            CKeyguardStatusBarView* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        OnPreDrawListener(
            /* [in] */ CKeyguardStatusBarView* host,
            /* [in] */ Boolean userSwitcherVisible,
            /* [in] */ Int32 systemIconsCurrentX);

        // @Override
        CARAPI OnPreDraw(
            /* [out] */ Boolean* result);

    private:
        CKeyguardStatusBarView* mHost;
        Boolean mUserSwitcherVisible;
        Int32 mSystemIconsCurrentX;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CKeyguardStatusBarView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetListening(
        /* [in] */ Boolean listening);

    CARAPI SetBatteryController(
        /* [in] */ IBatteryController* batteryController);

    CARAPI SetUserInfoController(
        /* [in] */ IUserInfoController* userInfoController);

    // @Override
    virtual CARAPI OnBatteryLevelChanged(
        /* [in] */ Int32 level,
        /* [in] */ Boolean pluggedIn,
        /* [in] */ Boolean charging);

    // @Override
    virtual CARAPI OnPowerSaveChanged();

    CARAPI SetKeyguardUserSwitcher(
        /* [in] */ IKeyguardUserSwitcher* keyguardUserSwitcher);

    CARAPI SetKeyguardUserSwitcherShowing(
        /* [in] */ Boolean showing,
        /* [in] */ Boolean animate);

    // @Override
    CARAPI SetVisibility(
        /* [in] */ Int32 visibility);

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

protected:
    // @Override
    CARAPI OnFinishInflate();

    // @Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    CARAPI_(void) LoadDimens();

    CARAPI_(void) UpdateVisibilities();

    CARAPI_(void) UpdateSystemIconsLayoutParams();

    CARAPI_(void) UpdateUserSwitcher();

    CARAPI_(void) AnimateNextLayoutChange();

private:
    Boolean mBatteryCharging;
    Boolean mKeyguardUserSwitcherShowing;
    Boolean mBatteryListening;

    AutoPtr<ITextView> mCarrierLabel;
    AutoPtr<IView> mSystemIconsSuperContainer;
    AutoPtr<IMultiUserSwitch> mMultiUserSwitch;
    AutoPtr<IImageView> mMultiUserAvatar;
    AutoPtr<ITextView> mBatteryLevel;

    AutoPtr<IBatteryController> mBatteryController;
    AutoPtr<IKeyguardUserSwitcher> mKeyguardUserSwitcher;

    Int32 mSystemIconsSwitcherHiddenExpandedMargin;
    AutoPtr<IInterpolator> mFastOutSlowInInterpolator;
    AutoPtr<BatteryStateChangeCallback> mBatteryStateChangeCallback;
};

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CKEYGUARDSTATUSBARVIEW_H__
