
#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PHONESTATUSBAR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PHONESTATUSBAR_H__

#include "elastos/droid/systemui/statusbar/BaseStatusBar.h"

using Elastos::Droid::SystemUI::StatusBar::BaseStatusBar;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

class PhoneStatusBar : public BaseStatusBar
{
private:
    class MakeIconsInvisibleAnimatorListener
        : public ElRefBase
        , public IAnimatorListener
    {
    public:
        CAR_INTERFACE_DECL();

        MakeIconsInvisibleAnimatorListener(
            /* [in] */ PhoneStatusBar* host);

        virtual CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);
    private:
        PhoneStatusBar* mHost;
    };

    class UserSetupObserver
        : public ContentObserver
    {
    public:
        UserSetupObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ PhoneStatusBar* host);

        virtual CARAPI OnChange(
            /* [in] */ Boolean selfChange);
    private:
        PhoneStatusBar* mHost;
    };

    class StatusBarWindowTouchListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        StatusBarWindowTouchListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        PhoneStatusBar* mHost;
    };

    class HeaderTouchListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        HeaderTouchListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        PhoneStatusBar* mHost;
    };

    class EmergencyCallLabelClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        EmergencyCallLabelClickListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        PhoneStatusBar* mHost;
    };

    class EmergencyCallLabelLayoutChangeListener
        : public ElRefBase
        , public IViewOnLayoutChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        EmergencyCallLabelLayoutChangeListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI onLayoutChange(
            /* [in] */ IView* v,
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom,
            /* [in] */ Int32 oldLeft,
            /* [in] */ Int32 oldTop,
            /* [in] */ Int32 oldRight,
            /* [in] */ Int32 oldBottom);
    private:
        PhoneStatusBar* mHost;
    };

    class PileSizeChangedListener
        : public ElRefBase
        , public IOnSizeChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        PileSizeChangedListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnSizeChanged(
            /* [in] */ IView* v,
            /* [in] */ Int32 w,
            /* [in] */ Int32 h,
            /* [in] */ Int32 oldw,
            /* [in] */ Int32 oldh);
    private:
        PhoneStatusBar* mHost;
    };

    class RecentsClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        RecentsClickListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        PhoneStatusBar* mHost;
    };

    class RotSettingClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        RotSettingClickListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        PhoneStatusBar* mHost;
    };

    class HomeSearchActionListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        HomeSearchActionListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        PhoneStatusBar* mHost;
    };

    class ViewOnFocusChangeListener
        : public ElRefBase
        , public IViewOnFocusChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        ViewOnFocusChangeListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnFocusChange(
            /* [in] */ IView* v,
            /* [in] */ Boolean hasFocus);
    private:
        PhoneStatusBar* mHost;
    };

    class ExpandedInvisibleRunnable
        : public Runnable
    {
    public:
        ExpandedInvisibleRunnable(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI Run();
    private:
        PhoneStatusBar* mHost;
    };

    class ClingRunnable
        : public Runnable
    {
    public:
        ClingRunnable(
            /* [in] */ IViewStub* viewStub,
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI Run();
    private:
        AutoPtr<IViewStub> mViewStub;
        PhoneStatusBar* mHost;
    };

    class ClingClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ClingClickListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        PhoneStatusBar* mHost;
    };

    class ClingTouchListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        ClingTouchListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        PhoneStatusBar* mHost;
    };

    class ClearButtonClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ClearButtonClickListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        PhoneStatusBar* mHost;
    };

    class SettingsButtonClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        SettingsButtonClickListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        PhoneStatusBar* mHost;
    };

    class ClockClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ClockClickListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        PhoneStatusBar* mHost;
    };

    class NotificationButtonListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        NotificationButtonListener(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        PhoneStatusBar* mHost;
    };

    class  PhoneStatusBarBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
         PhoneStatusBarBroadcastReceiver(
            /* [in] */ PhoneStatusBar* bar);

    protected:
        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("PhoneStatusBar::PhoneStatusBarBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        PhoneStatusBar* mHost;
    };

    class MyTicker
        : public Ticker
    {
    public:
         MyTicker(
            /* [in] */ IContext* context,
            /* [in] */ IView* sb,
            /* [in] */ PhoneStatusBar* bar);

        //@Override
        virtual CARAPI TickerStarting();

        //@Override
        virtual CARAPI TickerDone();

        //@Override
        virtual CARAPI TickerHalting();

    private:
        PhoneStatusBar* mHost;
    };

    class TickingDoneListener
        : public ElRefBase
        , public IAnimationAnimationListener
    {
    public:
        TickingDoneListener(
            /* [in] */ PhoneStatusBar* bar);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimation* animation);

        CARAPI OnAnimationRepeat(
            /* [in] */ IAnimation* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimation* animation);
    private:
        PhoneStatusBar* mHost;
    };

    class ShowSearchPanelRunnable
        : public Runnable
    {
    public:
        ShowSearchPanelRunnable(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI Run();
    private:
        PhoneStatusBar* mHost;
    };

    class ClearButtonClickRunnable
        : public Runnable
    {
    public:
        ClearButtonClickRunnable(
            /* [in] */ List<AutoPtr<IView> >& snapshot,
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI Run();
    private:
        List<AutoPtr<IView> > mSnapshot;
        PhoneStatusBar* mHost;
    };

    class CollapseCleanupRunnable
        : public Runnable
    {
    public:
        CollapseCleanupRunnable(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI Run();
    private:
        PhoneStatusBar* mHost;
    };

    class DismissRowAnimatedRunnable
        : public Runnable
    {
    public:
        DismissRowAnimatedRunnable(
            /* [in] */ IView* view,
            /* [in] */ Int32 velocity,
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI Run();
    private:
        AutoPtr<IView> mView;
        Int32 mVelocity;
        PhoneStatusBar* mHost;
    };


    class DelayCollapseAnimationRunnable
        : public Runnable
    {
    public:
        DelayCollapseAnimationRunnable(
            /* [in] */ PhoneStatusBar* bar);

        virtual CARAPI Run();
    private:
        PhoneStatusBar* mHost;
    };

public:
    //@Override
    virtual CARAPI Start();

    Int32 GetStatusBarHeight();

    // CommandQueueCallbacks interfaces
    //
    virtual CARAPI AddIcon(
        /* [in] */ const String& slot,
        /* [in] */ Int32 index,
        /* [in] */ Int32 viewIndex,
        /* [in] */ IStatusBarIcon* icon);
    virtual CARAPI UpdateIcon(
        /* [in] */ const String& slot,
        /* [in] */ Int32 index,
        /* [in] */ Int32 viewIndex,
        /* [in] */ IStatusBarIcon* old,
        /* [in] */ IStatusBarIcon* icon);
    virtual CARAPI RemoveIcon(
        /* [in] */ const String& slot,
        /* [in] */ Int32 index,
        /* [in] */ Int32 viewIndex);
    virtual CARAPI AddNotification(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* notification);
    virtual CARAPI RemoveNotification(
        /* [in] */ IBinder* key);

    /**
     * State is one or more of the DISABLE constants from StatusBarManager.
     */
    virtual CARAPI Disable(
        /* [in] */ Int32 state);

    virtual CARAPI AnimateExpandNotificationsPanel();

    CARAPI AnimateCollapsePanels();

    virtual CARAPI AnimateCollapsePanels(
        /* [in] */ Int32 flags);
    virtual CARAPI AnimateExpandSettingsPanel();
    virtual CARAPI SetSystemUiVisibility(Int32 vis, Int32 mask);
    virtual CARAPI TopAppWindowChanged(
        /* [in] */ Boolean visible);
    virtual CARAPI SetImeWindowStatus(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 vis,
        /* [in] */ Int32 backDisposition);
    virtual CARAPI SetHardKeyboardStatus(
        /* [in] */ Boolean available,
        /* [in] */ Boolean enabled);
    virtual CARAPI SetNavigationIconHints(
        /* [in] */ Int32 hints);

    CARAPI ShowClock(
        /* [in] */ Boolean show);

    AutoPtr<IHandler> GetHandler();

    AutoPtr<IViewPropertyAnimator> SetVisibilityWhenDone();

    AutoPtr<IAnimator> SetVisibilityWhenDone();

    AutoPtr<IAnimator> Interpolator(
        /* [in] */ ITimeInterpolator* ti,
        /* [in] */ IAnimator* a);

    AutoPtr<IAnimator> StartDelay(
        /* [in] */ Int32 d,
        /* [in] */ IAnimator* a);

    AutoPtr<IAnimator> Start(
        /* [in] */ IAnimator* a);

    CARAPI FlipToNotifications();

    CARAPI SwitchToSettings();

    CARAPI FlipToSettings();

    CARAPI FlipPanels();

    CARAPI AnimateCollapseQuickSettings();

    Boolean IsClinging();

    CARAPI HideCling();

    CARAPI ShowCling();

    Boolean InterceptTouchEvent(
        /* [in] */ IMotionEvent* event);

    // GestureRecorder getGestureRecorder();

    CARAPI SetLightsOn(
        /* [in] */ Boolean on);

    static String ViewInfo(
        /* [in] */ IView* v);

    //@Override
    virtual CARAPI CreateAndAddWindows();

    static Float Saturate(
        /* [in] */ Float a);

    //@Override
    virtual CARAPI UpdateExpandedViewPos(
        /* [in] */ Int32 thingy);

    CARAPI StartActivityDismissingKeyguard(
        /* [in] */ IIntent* intent,
        /* [in] */ Boolean onlyProvisioned);

    //@Override
    virtual CARAPI UserSwitched(
        /* [in] */ Int32 newUserId) ;

    CARAPI DismissIntruder();

protected:
    // ================================================================================
    // Constructing the view
    // ================================================================================
    AutoPtr<IPhoneStatusBarView> MakeStatusBarView();

    //@Override
    virtual AutoPtr<IView> GetStatusBarView();

    //@Override
    virtual AutoPtr<IWindowManagerLayoutParams> GetRecentsLayoutParams(
        /* [in] */ IViewGroupLayoutParams* layoutParams);

    //@Override
    virtual AutoPtr<IWindowManagerLayoutParams> GetSearchLayoutParams(
        /* [in] */ IViewGroupLayoutParams* layoutParams);

    //@Override
    virtual CARAPI ShowSearchPanel();

    //@Override
    virtual CARAPI HideSearchPanel();

    //@Override
    virtual CARAPI UpdateSearchPanel();

    Int32 GetStatusBarGravity();

    //@Override
    virtual CARAPI UpdateNotificationIcons();

    CARAPI_(void) UpdateCarrierLabelVisibility(Boolean force);

    //@Override
    virtual CARAPI SetAreThereNotifications();

    CARAPI_(void) MakeExpandedVisible(
        /* [in] */ Boolean revealAfterDraw);

    CARAPI_(void) MakeExpandedInvisibleSoon();

    CARAPI_(void) MakeExpandedInvisible();

    //@Override
    virtual CARAPI Tick(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* n,
        /* [in] */ Boolean firstTime);

    CARAPI_(void) SetNotificationIconVisibility(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 anim);

    CARAPI_(void) UpdateExpandedInvisiblePosition();

    //@Override
    virtual Int32 GetExpandedViewMaxHeight();

    // called by makeStatusbar and also by PhoneStatusBarView
    CARAPI_(void) UpdateDisplaySize();

    CARAPI_(void) UpdateResources();

    CARAPI_(void) LoadDimens();

    //
    // tracing
    //
    CARAPI_(void) PostStartTracing();
    CARAPI_(void) Vibrate();

    //@Override
    virtual CARAPI_(void) HaltTicker()

    //@Override
    virtual Boolean ShouldDisableNavbarGestures();

private:
    CARAPI_(void) SendRotBroadcast();

    CARAPI_(void) AwakenDreams();

    CARAPI_(void) PrepareNavigationBarView();

    // For small-screen devices (read: phones) that lack hardware navigation buttons
    CARAPI_(void) AddNavigationBar();

    CARAPI_(void) RepositionNavigationBar();

    CARAPI_(void) NotifyNavigationBarScreenOn(
        /* [in] */ Boolean screenOn);

    AutoPtr<IWindowManagerLayoutParams> GetNavigationBarLayoutParams();

    CARAPI_(void) AddIntruderView();

    CARAPI_(void) UpdateShowSearchHoldoff();

    CARAPI_(void) LoadNotificationShade();

    /**
     * Enables or disables layers on the children of the notifications pile.
     *
     * When layers are enabled, this method attempts to enable layers for the minimal
     * number of children. Only children visible when the notification area is fully
     * expanded will receive a layer. The technique used in this method might cause
     * more children than necessary to get a layer (at most one extra child with the
     * current UI.)
     *
     * @param layerType {@link View#LAYER_TYPE_NONE} or {@link View#LAYER_TYPE_HARDWARE}
     */
    CARAPI_(void) SetPileLayers(
        /* [in] */ Int32 layerType);

    CARAPI_(void) SetStatusBarLowProfile(
        /* [in] */ Boolean lightsOut);

    Boolean AreLightsOn();

    CARAPI_(void) NotifyUiVisibilityChanged();

    AutoPtr<IAnimation> LoadAnim(
        /* [in] */ Int32 id,
        /* [in] */ IAnimationAnimationListener* listener);

    CARAPI_(void) AddStatusBarWindow();

    CARAPI_(void) ResetUserSetupObserver();

    CARAPI_(void) SetIntruderAlertVisibility(
        /* [in] */ Boolean vis);

protected:
    // AutoPtr<IPhoneStatusBarPolicy> mIconPolicy;

    // // These are no longer handled by the policy, because we need custom strategies for them
    // AutoPtr<IBluetoothController> mBluetoothController;
    // AutoPtr<IBatteryController> mBatteryController;
    // AutoPtr<ILocationController> mLocationController;
    // AutoPtr<INetworkController> mNetworkController;

    Int32 mNaturalBarHeight;// = -1;
    Int32 mIconSize;// = -1;
    Int32 mIconHPadding;// = -1;
    Display mDisplay;
    AutoPtr<IPoint> mCurrentDisplaySize;//= new Point();

    AutoPtr<IIDreamManager> mDreamManager;

    AutoPtr<IStatusBarWindowView> mStatusBarWindow;
    AutoPtr<IPhoneStatusBarView> mStatusBarView;

    Int32 mPixelFormat;
    Object mQueueLock;
    //TODO Object mQueueLock = new Object();

    // viewgroup containing the normal contents of the statusbar
    AutoPtr<ILinearLayout> mStatusBarContents;

    // right-hand icons
    AutoPtr<ILinearLayout> mSystemIconArea;

    // left-hand icons
    AutoPtr<ILinearLayout> mStatusIcons;
    // the icons themselves
    AutoPtr<iIconMerger> mNotificationIcons;
    // [+>
    AutoPtr<IView> mMoreIcon;

    // expanded notifications
    AutoPtr<INotificationPanelView> mNotificationPanel; // the sliding/resizing panel within the notification window
    AutoPtr<IScrollView> mScrollView;
    AutoPtr<IView> mExpandedContents;
    Int32 mNotificationPanelGravity;
    Int32 mNotificationPanelMarginBottomPx;
    Int32 mNotificationPanelMarginPx;
    Float mNotificationPanelMinHeightFrac;
    Boolean mNotificationPanelIsFullScreenWidth;
    AutoPtr<ITextView> mNotificationPanelDebugText;

    // settings
    AutoPtr<IQuickSettings> mQS;
    Boolean mHasSettingsPanel;
    Boolean mHasFlipSettings;
    AutoPtr<iSettingsPanelView> mSettingsPanel;
    AutoPtr<IView> mFlipSettingsView;
    AutoPtr<IQuickSettingsContainerView> mSettingsContainer;
    Int32 mSettingsPanelGravity;

    // top bar
    AutoPtr<IView> mNotificationPanelHeader;
    AutoPtr<IView> mDateTimeView;
    AutoPtr<IView> mClearButton;
    AutoPtr<IImageView> mSettingsButton;
    AutoPtr<IImageView>mNotificationButton;

    // carrier/wifi label
    AutoPtr<ITextView> mCarrierLabel;
    Boolean mCarrierLabelVisible;// = false;
    Int32 mCarrierLabelHeight;
    AutoPtr<ITextView> mEmergencyCallLabel;
    Int32 mNotificationHeaderHeight;

    Boolean mShowCarrierInPanel;// = false;

    // position
    Int32 mPositionTmp[2];
    Boolean mExpandedVisible;

    // the date view
    AutoPtr<IDateView> mDateView;

    // the tracker view
    Int32 mTrackingPosition; // the position of the top of the tracking view.

    // Tracking finger for opening/closing.
    Int32 mEdgeBorder; // corresponds to R.dimen.status_bar_edge_ignore
    Boolean mTracking;
    AutoPtr<VelocityTracker> mVelocityTracker;

    Boolean mAnimating;
    Boolean mClosing; // only valid when mAnimating; indicates the initial acceleration
    Float mAnimY;
    Float mAnimVel;
    Float mAnimAccel;
    long mAnimLastTimeNanos;
    Boolean mAnimatingReveal;// = false;
    Int32 mViewDelta;
    Float mFlingVelocity;
    Int32 mFlingY;
    Int32 mAbsPos[2];
    AutoPtr<IRunnable> mPostCollapseCleanup;// = null;

    // for disabling the status bar
    Int32 mDisabled;// = 0;

    // tracking calls to View.setSystemUiVisibility()
    Int32 mSystemUiVisibility;//= View.SYSTEM_UI_FLAG_VISIBLE;

    AutoPtr<IDisplayMetrics> mDisplayMetrics;// = new DisplayMetrics();

    AutoPtr<IViewOnTouchListener> mHomeSearchActionListener;

    AutoPtr<IViewOnFocusChangeListener> mFocusChangeListener;

    AutoPtr<ITimeInterpolator> mAccelerateInterpolator;

    AutoPtr<ITimeInterpolator> mDecelerateInterpolator;

    static const Int32 FLIP_DURATION_OUT;// = 125;
    static const Int32 FLIP_DURATION_IN;// = 225;
    static const Int32 FLIP_DURATION;// = (FLIP_DURATION_IN + FLIP_DURATION_OUT);

    AutoPtr<IAnimator> mScrollViewAnim;
    AutoPtr<IAnimator> mFlipSettingsViewAnim;
    AutoPtr<IAnimator> mNotificationButtonAnim;
    AutoPtr<IAnimator> mSettingsButtonAnim;
    AutoPtr<IAnimator> mClearButtonAnim;

    AutoPtr<IAnimationAnimationListener> mTickingDoneListener;

    AutoPtr<IRunnable> mStartTracing;
    AutoPtr<IRunnable> mStopTracing;

private:
    static const Int32 MSG_OPEN_NOTIFICATION_PANEL;// = 1000;
    static const Int32 MSG_CLOSE_PANELS;// = 1001;
    static const Int32 MSG_OPEN_SETTINGS_PANEL;// = 1002;
    // 1020-1030 reserved for BaseStatusBar

    // will likely move to a resource or other tunable param at some point
    static const Int32 INTRUDER_ALERT_DECAY_MS;// = 0; // disabled, was 10000;

    static const Boolean CLOSE_PANEL_WHEN_EMPTIED;// = TRUE;

    static const Int32 NOTIFICATION_PRIORITY_MULTIPLIER;// = 10; // see NotificationManagerService
    static const Int32 HIDE_ICONS_BELOW_SCORE;// = Notification.PRIORITY_LOW * NOTIFICATION_PRIORITY_MULTIPLIER;

    // fling gesture tuning parameters, scaled to display density
    Float mSelfExpandVelocityPx; // classic value: 2000px/s
    Float mSelfCollapseVelocityPx; // classic value: 2000px/s (will be negated to collapse "up")
    Float mFlingExpandMinVelocityPx; // classic value: 200px/s
    Float mFlingCollapseMinVelocityPx; // classic value: 200px/s
    Float mCollapseMinDisplayFraction; // classic value: 0.08 (25px/min(320px,480px) on G1)
    Float mExpandMinDisplayFraction; // classic value: 0.5 (drag open halfway to expand)
    Float mFlingGestureMaxXVelocityPx; // classic value: 150px/s

    Float mExpandAccelPx; // classic value: 2000px/s/s
    Float mCollapseAccelPx; // classic value: 2000px/s/s (will be negated to collapse "up")

    Float mFlingGestureMaxOutputVelocityPx; // how fast can it really go? (should be a little
                                                    // faster than mSelfCollapseVelocityPx)

    // for immersive activities
    AutoPtr<IIntruderAlertView> mIntruderAlertView;

    // on-screen navigation buttons
    AutoPtr<INavigationBarView> mNavigationBarView;// = null;

    // ticker
    AutoPtr<ITicker> mTicker;
    AutoPtr<IView> mTickerView;
    Boolean mTicking;

    // help screen
    Boolean mClingShown;
    AutoPtr<IViewGroup> mCling;
    Boolean mSuppressStatusBarDrags; // while a cling is up, briefly deaden the bar to give things time to settle

    static const String setRotVal;// = "android.intent.action.RECORD_ROTATION_SETTINGS";
    static const String setRotShow;// = "android.intent.action.RECORD_ROTATION_SHOW";
    static const String setRotStr;// = "rot_setting";
    static Int32 irot;// = 0;
    AutoPtr<ArrayOf<IDrawable> > mRotsrc;//=new Drawable[4];

    // XXX: gesture research
    // final GestureRecorder mGestureRec = DEBUG_GESTURES
    //     ? new GestureRecorder("/sdcard/statusbar_gestures.dat")
    //     : null;

    Int32 mNavigationIconHints; = 0;
    AutoPtr<IAnimatorListener> mMakeIconsInvisible;

    // ensure quick settings is disabled until the current user makes it through the setup wizard
    Boolean mUserSetup;// = false;
    AutoPtr<IContentObserver> mUserSetupObserver;

    AutoPtr<IViewOnClickListener> mRecentsClickListener;
    AutoPtr<IViewOnClickListener> mRotSettingClickListener;

    Int32 mShowSearchHoldoff;// = 0;

    AutoPtr<IRunnable> mShowSearchPanel;

    AutoPtr<IViewOnClickListener> mClearButtonListener;
    AutoPtr<IViewOnClickListener> mSettingsButtonListener;
    AutoPtr<IViewOnClickListener> mClockClickListener;
    AutoPtr<IViewOnClickListener> mNotificationButtonListener;

    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;



};

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_PHONE_PHONESTATUSBAR_H__
