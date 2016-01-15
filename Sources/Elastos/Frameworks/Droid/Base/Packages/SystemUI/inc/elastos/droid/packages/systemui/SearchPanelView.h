#ifndef __ELASTOS_DROID_SYSTEMUI_SEARCHPANELVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_SEARCHPANELVIEW_H__

#include "elastos/droid/widget/FrameLayout.h"

using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::SystemUI::StatusBar::IBaseStatusBar;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class SearchPanelView : public FrameLayout
{
//     class GlowPadTriggerListener implements GlowPadView.OnTriggerListener {
//     boolean mWaitingForLaunch;

//     public void onGrabbed(View v, int handle) {
//     }

//     public void onReleased(View v, int handle) {
//     }

//     public void onGrabbedStateChange(View v, int handle) {
//         if (!mWaitingForLaunch && OnTriggerListener.NO_HANDLE == handle) {
//             mBar.hideSearchPanel();
//         }
//     }

//     public void onTrigger(View v, final int target) {
//         final int resId = mGlowPadView.getResourceIdForTarget(target);
//         switch (resId) {
//             case com.android.internal.R.drawable.ic_action_assist_generic:
//                 mWaitingForLaunch = true;
//                 startAssistActivity();
//                 vibrate();
//                 break;
//         }
//     }

//     public void onFinishFinalAnimation() {
//     }
// }
// final GlowPadTriggerListener mGlowPadViewListener = new GlowPadTriggerListener();

    // private final OnPreDrawListener mPreDrawListener = new ViewTreeObserver.OnPreDrawListener() {
    //     public boolean onPreDraw() {
    //         getViewTreeObserver().removeOnPreDrawListener(this);
    //         mGlowPadView.resumeAnimations();
    //         return false;
    //     }
    // };

public:
    SearchPanelView();

    SearchPanelView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    SearchPanelView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI OnAnimationStarted();

    CARAPI_(Boolean) IsInContentArea(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI Show(
        /* [in] */ Boolean show,
        /* [in] */ Boolean animate);

    CARAPI Hide(
        /* [in] */ Boolean animate);

    CARAPI_(Boolean) IsShowing();

    CARAPI SetBar(
        /* [in] */ IBaseStatusBar* bar);

    CARAPI SetStatusBarView(
        /* [in] */ IView* statusBarView);

    CARAPI_(Boolean) IsAssistantAvailable();

    virtual Boolean DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

protected:
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI InitImpl(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    virtual ECode OnFinishInflate();

    virtual void OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

private:
    void StartAssistActivity();

    void MaybeSwapSearchIcon();

    Boolean PointInside(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IView* v);

    void Vibrate();

    AutoPtr<ILayoutTransition> CreateLayoutTransitioner();

private:
    static const Int32 SEARCH_PANEL_HOLD_DURATION;// = 0;
    static const String TAG;// = "SearchPanelView";
    static const Boolean DEBUG;// = TabletStatusBar.DEBUG || PhoneStatusBar.DEBUG || false;
    static const String ASSIST_ICON_METADATA_NAME;// = "com.android.systemui.action_assist_icon";

    AutoPtr<IContext> mContext;
    AutoPtr<IBaseStatusBar> mBar;
    // AutoPtr<IStatusBarTouchProxy> mStatusBarTouchProxy;

    Boolean mShowing;
    AutoPtr<IView> mSearchTargetsContainer;
    // AutoPtr<IGlowPadView> mGlowPadView;
    AutoPtr<IIWindowManager> mWm;
};


}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SEARCHPANELVIEW_H__
