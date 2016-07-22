
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CNAVIGATIONBARVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CNAVIGATIONBARVIEW_H__

#include "_Elastos_Droid_SystemUI_StatusBar_Phone_CNavigationBarView.h"
#include "elastos/droid/systemui/statusbar/DelegateViewHelper.h"
#include "elastos/droid/systemui/statusbar/phone/NavigationBarTransitions.h"
#include "Elastos.Droid.Animation.h"
#include "Elastos.Droid.View.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/widget/LinearLayout.h>

using Elastos::Droid::Animation::ITransitionListener;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::SystemUI::StatusBar::Policy::IDeadZone;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::LinearLayout;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

CarClass(CNavigationBarView)
    , public LinearLayout
    , public INavigationBarView
{
private:
    class NavTransitionListener
        : public Object
        , public ITransitionListener
    {
    public:
        CAR_INTERFACE_DECL()

        NavTransitionListener(
            /* [in] */ CNavigationBarView* host);

        // @Override
        CARAPI StartTransition(
            /* [in] */ ILayoutTransition* transition,
            /* [in] */ IViewGroup* container,
            /* [in] */ IView* view,
            /* [in] */ Int32 transitionType);

        // @Override
        CARAPI EndTransition(
            /* [in] */ ILayoutTransition* transition,
            /* [in] */ IViewGroup* container,
            /* [in] */ IView* view,
            /* [in] */ Int32 transitionType);

        CARAPI OnBackAltCleared();

    private:
        CNavigationBarView* mHost;
        Boolean mBackTransitioning;
        Boolean mHomeAppearing;
        Int64 mStartDelay;
        Int64 mDuration;
        AutoPtr<ITimeInterpolator> mInterpolator;
    };

    class H: public Handler
    {
    public:
        TO_STRING_IMPL("CNavigationBarView::H")

        H(
            /* [in] */ CNavigationBarView* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* m);

    private:
        CNavigationBarView* mHost;
    };

    class ImeSwitcherClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ImeSwitcherClickListener(
            /* [in] */ CNavigationBarView* host);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        CNavigationBarView* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNavigationBarView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetBarTransitions(
        /* [out] */ IBarTransitions** result);

    CARAPI SetDelegateView(
        /* [in] */ IView* view);

    CARAPI SetBar(
        /* [in] */ IBaseStatusBar* phoneStatusBar);

    CARAPI SetOnVerticalChangedListener(
        /* [in] */ IOnVerticalChangedListener* onVerticalChangedListener);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI GetCurrentView(
        /* [out] */ IView** view);

    CARAPI GetRecentsButton(
        /* [out] */ IView** view);

    CARAPI GetMenuButton(
        /* [out] */ IView** view);

    CARAPI GetBackButton(
        /* [out] */ IView** view);

    CARAPI GetHomeButton(
        /* [out] */ IView** view);

    CARAPI GetImeSwitchButton(
        /* [out] */ IView** view);

    // @Override
    CARAPI SetLayoutDirection(
        /* [in] */ Int32 layoutDirection);

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

    // @Override
    CARAPI OnFinishInflate();

    CARAPI IsVertical(
        /* [out] */ Boolean* result);

    CARAPI Reorient();

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

protected:
    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    // @Override
    CARAPI_(void) OnSizeChanged(
        /* [in] */ Int32 w,
        /* [in] */ Int32 h,
        /* [in] */ Int32 oldw,
        /* [in] */ Int32 oldh);

    // @Override
    CARAPI_(void) OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

private:
    CARAPI_(void) InitDownStates(
        /* [in] */ IMotionEvent* ev);

    CARAPI_(void) GetIcons(
        /* [in] */ IResources* res);

    CARAPI_(void) SetVisibleOrGone(
        /* [in] */ IView* view,
        /* [in] */ Boolean visible);

    CARAPI_(void) UpdateTaskSwitchHelper();

    CARAPI_(void) NotifyVerticalChangedListener(
        /* [in] */ Boolean newVertical);

    /**
     * In landscape, the LinearLayout is not auto mirrored since it is vertical. Therefore we
     * have to do it manually
     */
    CARAPI_(void) UpdateRTLOrder();
    /**
     * Swaps the children order of a LinearLayout if it's orientation is Vertical
     *
     * @param group The LinearLayout to swap the children from.
     */
    CARAPI_(void) SwapChildrenOrderIfVertical(
        /* [in] */ IView* group);

    /*
    @Override
    protected void onLayout (Boolean changed, Int32 left, Int32 top, Int32 right, Int32 bottom) {
        if (DEBUG) Log.d(TAG, String.format(
                    "onLayout: %s (%d,%d,%d,%d)",
                    changed?"changed":"notchanged", left, top, right, bottom));
        super.onLayout(changed, left, top, right, bottom);
    }

    // uncomment this for extra defensiveness in WORKAROUND_INVALID_LAYOUT situations: if all else
    // fails, any touch on the display will fix the layout.
    @Override
    public Boolean onInterceptTouchEvent(MotionEvent ev) {
        if (DEBUG) Log.d(TAG, "onInterceptTouchEvent: " + ev.toString());
        if (ev.getAction() == MotionEvent.ACTION_DOWN) {
            postCheckForInvalidLayout("touch");
        }
        return super.onInterceptTouchEvent(ev);
    }
    */

    CARAPI_(String) GetResourceName(
        /* [in] */ Int32 resId);

    CARAPI_(void) PostCheckForInvalidLayout(
        /* [in] */ const String& how);

    static CARAPI_(String) VisibilityToString(
        /* [in] */ Int32 vis);

    static CARAPI_(void) DumpButton(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& caption,
        /* [in] */ IView* button);

private:
    static Boolean DEBUG;
    static String TAG;

    // slippery nav bar when everything is disabled, e.g. during setup
    static Boolean SLIPPERY_WHEN_DISABLED;

    AutoPtr<IDisplay> mDisplay;
    AutoPtr<IView> mCurrentView;
    AutoPtr<ArrayOf<IView*> > mRotatedViews;

    Int32 mBarSize;
    Boolean mVertical;
    Boolean mScreenOn;

    Boolean mShowMenu;
    Int32 mDisabledFlags;
    Int32 mNavigationIconHints;

    AutoPtr<IDrawable> mBackIcon;
    AutoPtr<IDrawable> mBackLandIcon;
    AutoPtr<IDrawable> mBackAltIcon;
    AutoPtr<IDrawable> mBackAltLandIcon;
    AutoPtr<IDrawable> mRecentIcon;
    AutoPtr<IDrawable> mRecentLandIcon;

    AutoPtr<INavigationBarViewTaskSwitchHelper> mTaskSwitchHelper;
    AutoPtr<DelegateViewHelper> mDelegateHelper;
    AutoPtr<IDeadZone> mDeadZone;
    AutoPtr<NavigationBarTransitions> mBarTransitions;

    // workaround for LayoutTransitions leaving the nav buttons in a weird state (bug 5549288)
    static const Boolean WORKAROUND_INVALID_LAYOUT;
    static const Int32 MSG_CHECK_INVALID_LAYOUT;

    // performs manual animation in sync with layout transitions
    AutoPtr<NavTransitionListener> mTransitionListener;

    AutoPtr<IOnVerticalChangedListener> mOnVerticalChangedListener;
    Boolean mIsLayoutRtl;
    Boolean mDelegateIntercepted;

    AutoPtr<IViewOnClickListener> mImeSwitcherClickListener;;

    AutoPtr<H> mHandler;
};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_CNAVIGATIONBARVIEW_H__
