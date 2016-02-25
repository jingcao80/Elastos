#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NAVIGATIONBARVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NAVIGATIONBARVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/widget/LinearLayout.h"
#include "elastos/droid/systemui/statusbar/DelegateViewHelper.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"

using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::StatusBar::IIStatusBarService;
using Elastos::Droid::SystemUI::StatusBar::IBaseStatusBar;
using Elastos::Droid::SystemUI::StatusBar::Policy::IDeadZone;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class NavigationBarView : public LinearLayout
{
public:
    class MyViewOnClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ NavigationBarView* host);

        CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        NavigationBarView* mHost;
    };

    class MyViewOnTouchListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyViewOnTouchListener(
            /* [in] */ NavigationBarView* host);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        NavigationBarView* mHost;
    };

    class ScreenshotTimeoutRunnable
        : public Runnable
    {
    public:
        ScreenshotTimeoutRunnable(
            /* [in] */ NavigationBarView* host);

        virtual CARAPI Run();
    private:
        NavigationBarView* mHost;
    };

    class LowLightsAnimationListener
        : public AnimatorListenerAdapter
    {
    public:
        LowLightsAnimationListener(
            /* [in] */ IView* lights);

        virtual CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);
    private:
        IView* mLight;
    };

public:

    NavigationBarView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetDelegateView(
        /* [in] */ IView* view);

    CARAPI SetBar(
        /* [in] */ IBaseStatusBar* phoneStatusBar);

    // @Override
    virtual Boolean OnTouchEvent(
        /* [in] */ IMotionEvent* event);

    // @Override
    virtual Boolean OnInterceptTouchEvent(
            /* [in] */ IMotionEvent* event);

    AutoPtr<IView> GetRecentsButton();

    AutoPtr<IView> GetMenuButton();

    AutoPtr<IView> GetBackButton();

    AutoPtr<IView> GetHomeButton();

    AutoPtr<IView> GetScreenShotButton();

    AutoPtr<IView> GetVolumeDownButton();

    AutoPtr<IView> GetVolumeUpButton();

    AutoPtr<IView> GetSearchLight();

    CARAPI NotifyScreenOn(
        /* [in] */ Boolean screenOn);

    CARAPI SetNavigationIconHints(
        /* [in] */ Int32 hints);

    CARAPI SetNavigationIconHints(
        /* [in] */ Int32 hints,
        /* [in] */ Boolean force);

    CARAPI SetDisabledFlags(
        /* [in] */ Int32 disabledFlags);

    CARAPI SetDisabledFlags(
        /* [in] */ Int32 disabledFlags,
        /* [in] */ Boolean force);

    CARAPI SetSlippery(
        /* [in] */ Boolean newSlippery);

    CARAPI SetMenuVisibility(
        /* [in] */ Boolean show);

    CARAPI SetMenuVisibility(
        /* [in] */ Boolean show,
        /* [in] */ Boolean force);

    CARAPI SetLowProfile(
        /* [in] */ Boolean lightsOut);

    CARAPI SetLowProfile(
        /* [in] */ Boolean lightsOut,
        /* [in] */ Boolean animate,
        /* [in] */ Boolean force);

    CARAPI SetHidden(
        /* [in] */ Boolean hide);

    CARAPI OnFinishInflate();

    CARAPI Reorient();

protected:
    NavigationBarView();

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(void) OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32  b);

    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

private:
    // Assume this is called from the Handler thread.
    CARAPI_(void) TakeScreenshot();

    CARAPI_(String) GetResourceName(
        /* [in] */ Int32 resId);

    CARAPI_(void) PostCheckForInvalidLayout(
        /* [in] */ const String& how);

    static CARAPI_(String) VisibilityToString(
        /* [in] */ Int32 vis);

protected:
    AutoPtr<IIStatusBarService> mBarService;

private:
    static String TAG;

    static const Boolean NAVBAR_ALWAYS_AT_RIGHT = TRUE;

    // slippery nav bar when everything is disabled, e.g. during setup
    static const Boolean SLIPPERY_WHEN_DISABLED = TRUE;

    static const Boolean ANIMATE_HIDE_TRANSITION = FALSE; // turned off because it introduces unsightly delay when videos goes to full screen

    AutoPtr<IDisplay> mDisplay;

    AutoPtr<IView> mCurrentView;

    AutoPtr<ArrayOf<IView*> > mRotatedViews; // new View[4];

    Int32 mBarSize;
    Boolean mVertical;
    Boolean mScreenOn;

    Boolean mHidden;
    Boolean mLowProfile;
    Boolean mShowMenu;
    Int32 mDisabledFlags;
    Int32 mNavigationIconHints;

    AutoPtr<IDrawable> mBackIcon;
    AutoPtr<IDrawable> mBackLandIcon;
    AutoPtr<IDrawable> mBackAltIcon;
    AutoPtr<IDrawable> mBackAltLandIcon;

    AutoPtr<DelegateViewHelper> mDelegateHelper;
    AutoPtr<IDeadZone> mDeadZone;

    // workaround for LayoutTransitions leaving the nav buttons in a weird state (bug 5549288)
    static const Boolean WORKAROUND_INVALID_LAYOUT = TRUE;
    static const Int32 MSG_CHECK_INVALID_LAYOUT = 8686;

    Boolean mSoftVolumeKey;// = Boolean.parseBoolean(SystemProperties.get("ro.statusbar.volume", "false"));
    Boolean mSoftScreenShotKey;// = Boolean.parseBoolean(SystemProperties.get("ro.statusbar.screenshot", "false"));

    AutoPtr<IHandler> mHander;

    Object mScreenshotLock;
    AutoPtr<IServiceConnection> mScreenshotConnection;

    AutoPtr<IViewOnClickListener> mOnClickListener;
    AutoPtr<IViewOnTouchListener> mLightsOutListener;
    AutoPtr<IRunnable> mScreenshotTimeout;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_NAVIGATIONBARVIEW_H__
