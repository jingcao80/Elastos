#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONPANEL_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONPANEL_H__

#include "elastos/droid/widget/RelativeLayout.h"
#include "elastos/droid/animation/AnimatorListenerAdapter.h"


using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IOnPreDrawListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::Widget::RelativeLayout;
using Elastos::Droid::SystemUI::IExpandHelper;
using Elastos::Droid::SystemUI::StatusBar::Policy::INotificationRowLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

using Elastos::Droid::View::Animation::IInterpolator;

class NotificationPanel : public RelativeLayout
{
private:

    class ClearButtonClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ClearButtonClickListener(
            /* [in] */ NotificationPanel* host);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        NotificationPanel* mHost;
    };

    /**
     * This is used only when we've created a hardware layer and are waiting until it's
     * been created in order to start the appearing animation.
     */
    class PreDrawListener
        : public ElRefBase
        , public IOnPreDrawListener
    {
    public:
        CAR_INTERFACE_DECL()

        PreDrawListener(
            /* [in] */ NotificationPanel* host);

        virtual CARAPI OnPreDraw(
            /* [out] */ Boolean* result);
    private:
        NotificationPanel* mHost;
    };

    class Choreographer
        : public ElRefBase
        , public IAnimatorListener
    {
    public:
        CAR_INTERFACE_DECL()

        Choreographer(
            /* [in] */ NotificationPanel* host);

        void CreateAnimation(
            /* [in] */ Boolean appearing);

        void StartAnimation(
            /* [in] */ Boolean appearing);

        CARAPI OnAnimationCancel(
            /* [out] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [out] */ IAnimator* animation);

        CARAPI OnAnimationRepeat(
            /* [out] */ IAnimator* animation);

        CARAPI OnAnimationStart(
            /* [out] */ IAnimator* animation);

    private:
        NotificationPanel* mHost;
        Boolean mVisible;
        Int32 mPanelHeight;
        AutoPtr<IAnimatorSet> mContentAnim;

        // should group this into a multi-property animation
        const static Int32 OPEN_DURATION;// = 250;
        const static Int32 CLOSE_DURATION;// = 250;

        // the panel will start to appear this many px from the end
        const static Int32 HYPERSPACE_OFFRAMP;// = 200;
    };

    class MyListener
        : public AnimatorListenerAdapter
    {
    public:
        MyListener(
            /* [in] */ IView* show,
            /* [in] */ IView* hide,
            /* [in] */ NotificationPanel* host);

        virtual CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);
    private:
        AutoPtr<IView> mShow;
        AutoPtr<IView> mHide;
        NotificationPanel* mHost;
    };
public:
    NotificationPanel();

    NotificationPanel(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    NotificationPanel(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    CARAPI SetBar(
        /* [in] */ ITabletStatusBar* b);

    CARAPI_(AutoPtr<IView>) GetClearButton();

    CARAPI Show(
        /* [in] */ Boolean show,
        /* [in] */ Boolean animate);

    Boolean IsShowing();

    CARAPI SetNotificationCount(
        /* [in] */ Int32 n);

    CARAPI SetContentFrameVisible(
        /* [in] */ Boolean showing,
        /* [in] */ Boolean animate);

    CARAPI SwapPanels();

    CARAPI UpdateClearButton();

    CARAPI SetClearable(
        /* [in] */ Boolean clearable);

    CARAPI UpdatePanelModeButtons();

    CARAPI SetSettingsEnabled(
        /* [in] */ Boolean settingsEnabled);

    virtual CARAPI OnFinishInflate();

    virtual CARAPI OnAttachedToWindow();

    virtual void OnVisibilityChanged(
        /* [in] */ IView* v,
        /* [in] */ Int32 vis);

    virtual Boolean DispatchHoverEvent(
        /* [in] */ IMotionEvent* event);

    Boolean DispatchKeyEvent(
        /* [in] */ IKeyEvent* event);

    CARAPI OnClick(
        /* [in] */ IView* v);

    Boolean IsInContentArea(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    virtual Boolean OnInterceptTouchEvent(
        /* [in] */ IMotionEvent* ev);

    virtual Boolean OnTouchEvent(
        /* [in] */ IMotionEvent* ev);

protected:
    void RemoveSettingsView();

    // NB: it will be invisible until you show it
    void AddSettingsView();

protected:
    const static Int32 PANEL_FADE_DURATION;// = 150;

    Boolean mShowing;
    Boolean mHasClearableNotifications;// = false;
    Int32 mNotificationCount;// = 0;
    AutoPtr<INotificationPanelTitle> mTitleArea;
    AutoPtr<IView> mSettingsButton;
    AutoPtr<IView> mNotificationButton;
    AutoPtr<IView> mNotificationScroller;
    AutoPtr<IViewGroup> mContentFrame;
    AutoPtr<IRect> mContentArea;// = new Rect();
    AutoPtr<IView> mSettingsView;
    AutoPtr<IViewGroup> mContentParent;
    AutoPtr<ITabletStatusBar> mBar;
    AutoPtr<IView> mClearButton;
    static AutoPtr<IInterpolator> sAccelerateInterpolator;// = new AccelerateInterpolator();
    static AutoPtr<IInterpolator> sDecelerateInterpolator;// = new DecelerateInterpolator();

    // amount to slide mContentParent down by when mContentFrame is missing
    Float mContentFrameMissingTranslation;

    AutoPtr<Choreographer> mChoreo;

    AutoPtr<IViewOnClickListener> mClearButtonListener;
    AutoPtr<IOnPreDrawListener> mPreDrawListener;
private:
    static const String TAG;
    static const Boolean DEBUG;

    AutoPtr<IExpandHelper> mExpandHelper;
    AutoPtr<INotificationRowLayout> mLatestItems;
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_NOTIFICATIONPANEL_H__
