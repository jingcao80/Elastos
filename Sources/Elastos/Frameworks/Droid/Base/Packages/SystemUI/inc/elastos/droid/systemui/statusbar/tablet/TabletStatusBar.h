#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_TABLETSTATUSBAR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_TABLETSTATUSBAR_H__

#include "elastos/droid/systemui/statusbar/BaseStatusBar.h"
#include "elastos/droid/systemui/statusbar/DoNotDisturb.h"
#include "elastos/droid/systemui/statusbar/tablet/TabletTicker.h"
#include "elastos/droid/systemui/statusbar/policy/NetworkController.h"
#include "elastos/droid/view/VelocityTracker.h"

using Elastos::Droid::Os::Storage::IStorageManager;
using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::VelocityTracker;
using Elastos::Droid::SystemUI::StatusBar::BaseStatusBar;
using Elastos::Droid::SystemUI::StatusBar::DoNotDisturb;
using Elastos::Droid::SystemUI::StatusBar::Policy::ICompatModeButton;
using Elastos::Droid::SystemUI::StatusBar::Policy::NetworkController;
using Elastos::Droid::Animation::ITransitionListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Tablet {

extern "C" const InterfaceID EIID_TabletStatusBar;

class TabletTicker;

class TabletStatusBar : public BaseStatusBar
{
public:
    class ShowSearchPanelRunnable
        : public Runnable
    {
    public:
        ShowSearchPanelRunnable(
            /* [in] */ TabletStatusBar* bar);

        virtual ECode Run();
    private:
        TabletStatusBar* mHost;
    };

    class HomeSearchActionListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()
        HomeSearchActionListener(
            /* [in] */ TabletStatusBar* bar);

        virtual ECode OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        TabletStatusBar* mHost;
    };

    class ScreenshotTimeoutRunnable
        : public Runnable
    {
    public:
        ScreenshotTimeoutRunnable(
            /* [in] */ TabletStatusBar* bar);

        virtual ECode Run();
    private:
        TabletStatusBar* mHost;
    };

    class NotificationTriggerTouchListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        class HiliteOnRRunnable
            : public Runnable
        {
        public:
            HiliteOnRRunnable(
                /* [in] */ TabletStatusBar* host);

            virtual ECode Run();
        private:
            TabletStatusBar* mHost;
        };
    public:
        CAR_INTERFACE_DECL()

        NotificationTriggerTouchListener(
            /* [in] */ TabletStatusBar* bar);

        void Hilite(
            /* [in] */ Boolean on);

        virtual ECode OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        TabletStatusBar* mHost;
        AutoPtr<VelocityTracker> mVT;
        Float mInitialTouchX;
        Float mInitialTouchY;
        Int32 mTouchSlop;

        AutoPtr<IRunnable> mHiliteOnR;
    };

    class TableStatusBarBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
         TableStatusBarBroadcastReceiver(
            /* [in] */ TabletStatusBar* bar);

    protected:
        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("TableStatusBar::TableStatusBarBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        TabletStatusBar* mHost;
    };

    class TouchOutsideListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        TouchOutsideListener(
            /* [in] */ Int32 msg,
            /* [in] */ IStatusBarPanel* panel,
            /* [in] */ TabletStatusBar* bar);

        virtual CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        Int32 mMsg;
        AutoPtr<IStatusBarPanel> mPanel;
        TabletStatusBar* mHost;
    };

    class ShadowTouchListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        ShadowTouchListener(
            /* [in] */ TabletStatusBar* bar);

        virtual ECode OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        TabletStatusBar* mHost;
    };

    class CompatibilityHelpDialogClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        CompatibilityHelpDialogClickListener(
            /* [in] */ TabletStatusBar* bar);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        TabletStatusBar* mHost;
    };

    class NotificationDNDClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        NotificationDNDClickListener(
            /* [in] */ TabletStatusBar* bar);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        TabletStatusBar* mHost;
    };

    class TabletStatusBarClickListener
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        TabletStatusBarClickListener(
            /* [in] */ TabletStatusBar* bar);

        virtual CARAPI OnClick(
            /* [in] */ IView* v);
    private:
        TabletStatusBar* mHost;
    };

    class TakeScreenshotServiceConnection
        : public ElRefBase
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL()

        TakeScreenshotServiceConnection(
            /* [in] */ TabletStatusBar* bar);

        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ IBinder* service);

        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);
    private:
        TabletStatusBar* mHost;
    };

    class MyTransitionListener
        : public ITransitionListener
        , ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        MyTransitionListener(
            /* [in] */ IViewGroup* v);

        CARAPI StartTransition(
            /* [in] */ ILayoutTransition* transition,
            /* [in] */ IViewGroup* container,
            /* [in] */ IView* view,
            /* [in] */ Int32 transitionType);

        CARAPI EndTransition(
            /* [in] */ ILayoutTransition* transition,
            /* [in] */ IViewGroup* container,
            /* [in] */ IView* view,
            /* [in] */ Int32 transitionType);
    private:
        AutoPtr<IViewGroup> mBar;
    };

private:
    class LocalHandler
        : public BaseStatusBar::MyHandler
    {
    public:
        LocalHandler(
            /* [in] */ TabletStatusBar* host)
            : BaseStatusBar::MyHandler(host)
            , mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        TabletStatusBar* mHost;
    };

public:
    TabletStatusBar();

    CARAPI_(Int32) GetStatusBarHeight();

    CARAPI OnBarHeightChanged(
        /* [in] */ Int32 height);

    CARAPI ShowClock(
        /* [in] */ Boolean show);

    // called by TabletTicker when it's done with all queued ticks
    CARAPI DoneTicking();

    CARAPI SetLightsOn(
        /* [in] */ Boolean on);

    CARAPI OnClickRecentButton();

    CARAPI OnClickInputMethodSwitchButton();

    CARAPI OnClickCompatModeButton();

    CARAPI ResetNotificationPeekFadeTimer();

    CARAPI ClearAll();

    AutoPtr<IContext> GetContext();

    virtual CARAPI Start();

    AutoPtr<IView> GetStatusBarView();

    virtual CARAPI ShowSearchPanel();

    virtual CARAPI HideSearchPanel();

    virtual CARAPI UpdateSearchPanel();

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

    virtual CARAPI Disable(
        /* [in] */ Int32 state);

    virtual CARAPI AnimateExpandNotificationsPanel();

    CARAPI AnimateCollapsePanels();

    virtual CARAPI AnimateCollapsePanels(
        /* [in] */ Int32 flags);

    virtual CARAPI AnimateExpandSettingsPanel();

    virtual CARAPI SetNavigationIconHints(
        /* [in] */ Int32 hints);

    virtual CARAPI SetSystemUiVisibility(
        /* [in] */ Int32 vis,
        /* [in] */ Int32 mask);

    CARAPI TopAppWindowChanged(
        /* [in] */ Boolean showMenu);

    virtual CARAPI SetImeWindowStatus(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 vis,
        /* [in] */ Int32 backDisposition);

    virtual CARAPI SetHardKeyboardStatus(
        /* [in] */ Boolean available,
        /* [in] */ Boolean enabled);

    CARAPI OnHardKeyboardEnabledChange(
        /* [in] */ Boolean enabled);

    CARAPI_(void) OnStatusBarViewInterceptTouchEvent();
protected:
    // @Override
    virtual CARAPI CreateAndAddWindows();

    void AddPanelWindows();

    virtual Int32 GetExpandedViewMaxHeight();

    virtual ECode OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    void LoadDimens();

    AutoPtr<IView> MakeStatusBarView();

    virtual AutoPtr<IWindowManagerLayoutParams> GetRecentsLayoutParams(
        /* [in] */ IViewGroupLayoutParams* layoutParams);

    virtual AutoPtr<IWindowManagerLayoutParams> GetSearchLayoutParams(
        /* [in] */ IViewGroupLayoutParams* layoutParams);

    Int32 GetStatusBarGravity();

    virtual CARAPI Tick(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* n,
        /* [in] */ Boolean firstTime);

    void NotifyUiVisibilityChanged();

    void ShowCompatibilityHelp();

    void HideCompatibilityHelp();

    virtual CARAPI SetAreThereNotifications();

    virtual CARAPI UpdateNotificationIcons();

    virtual CARAPI WorkAroundBadLayerDrawableOpacity(
        /* [in] */ IView* v);

    virtual Boolean IsTopNotification(
        /* [in] */ IViewGroup* parent,
        /* [in] */ NotificationDataEntry* entry);

    virtual CARAPI HaltTicker();

    virtual CARAPI UpdateExpandedViewPos(
        /* [in] */ Int32 expandedPosition);

    virtual Boolean ShouldDisableNavbarGestures();

private:
    void AddStatusBarWindow();

    Int32 GetNotificationPanelHeight();

    void SetNavigationVisibility(
        /* [in] */ Int32 visibility);

    Boolean HasTicker(
        /* [in] */ INotification* n);

    Boolean IsImmersive();

    // Assume this is called from the Handler thread.
    void TakeScreenshot();

    void ReloadAllNotificationIcons();

    void LoadNotificationPanel();

    ECode RemoveAndSendMessage(
        /* [in] */ Int32 msg);

public:
    static const Boolean DEBUG;// = false;
    static const Boolean DEBUG_COMPAT_HELP;// = false;
    static const String TAG;// = "TabletStatusBar";

    static const Int32 MSG_OPEN_NOTIFICATION_PANEL;// = 1000;
    static const Int32 MSG_CLOSE_NOTIFICATION_PANEL;// = 1001;
    static const Int32 MSG_OPEN_NOTIFICATION_PEEK;// = 1002;
    static const Int32 MSG_CLOSE_NOTIFICATION_PEEK;// = 1003;
    // 1020-1029 reserved for BaseStatusBar
    static const Int32 MSG_SHOW_CHROME;// = 1030;
    static const Int32 MSG_HIDE_CHROME;// = 1031;
    static const Int32 MSG_OPEN_INPUT_METHODS_PANEL;// = 1040;
    static const Int32 MSG_CLOSE_INPUT_METHODS_PANEL;// = 1041;
    static const Int32 MSG_OPEN_COMPAT_MODE_PANEL;// = 1050;
    static const Int32 MSG_CLOSE_COMPAT_MODE_PANEL;// = 1051;
    static const Int32 MSG_STOP_TICKER;// = 2000;

    // Notification "peeking" (flyover preview of individual notifications)
    const static Int32 NOTIFICATION_PEEK_HOLD_THRESH;// = 200; // ms
    const static Int32 NOTIFICATION_PEEK_FADE_DELAY;// = 3000; // ms

protected:
    // The height of the bar, as definied by the build.  It may be taller if we're plugged
    // into hdmi.
    Int32 mNaturalBarHeight;// = -1;
    Int32 mIconSize;// = -1;
    Int32 mIconHPadding;// = -1;
    Int32 mNavIconWidth;// = -1;
    Int32 mMenuNavIconWidth;// = -1;
    Int32 mMaxNotificationIcons;// = 5;

    AutoPtr<ITabletStatusBarView> mStatusBarView;
    AutoPtr<IView> mNotificationArea;
    AutoPtr<IView> mNotificationTrigger;
    AutoPtr<INotificationIconArea> mNotificationIconArea;
    AutoPtr<IViewGroup> mNavigationArea;

    Boolean mNotificationDNDMode;
    AutoPtr<NotificationDataEntry> mNotificationDNDDummyEntry;

    AutoPtr<IImageView> mBackButton;
    AutoPtr<IView> mHomeButton;
    AutoPtr<IView> mMenuButton;
    AutoPtr<IView> mRecentButton;
    AutoPtr<IView> mScreenShotButton;
    AutoPtr<IView> mVolumeDownButton;
    AutoPtr<IView> mVolumeUpButton;

    AutoPtr<IViewGroup> mFeedbackIconArea; // notification icons, IME icon, compat icon
    AutoPtr<IInputMethodButton> mInputMethodSwitchButton;
    AutoPtr<ICompatModeButton> mCompatModeButton;

    AutoPtr<INotificationPanel> mNotificationPanel;
    AutoPtr<IWindowManagerLayoutParams> mNotificationPanelParams;
    AutoPtr<INotificationPeekPanel> mNotificationPeekWindow;
    AutoPtr<IViewGroup> mNotificationPeekRow;
    Int32 mNotificationPeekIndex;
    AutoPtr<IBinder> mNotificationPeekKey;
    AutoPtr<ILayoutTransition> mNotificationPeekScrubLeft;
    AutoPtr<ILayoutTransition> mNotificationPeekScrubRight;

    Int32 mNotificationPeekTapDuration;
    Int32 mNotificationFlingVelocity;

    // BatteryController mBatteryController;
    // BluetoothController mBluetoothController;
    // LocationController mLocationController;
    AutoPtr<NetworkController> mNetworkController;
    AutoPtr<DoNotDisturb> mDoNotDisturb;

    // DisplayController mDisplayController;
    // SoundController mSoundController;

    AutoPtr<IViewGroup> mBarContents;

    // hide system chrome ("lights out") support
    AutoPtr<IView> mShadow;

    AutoPtr<INotificationIconAreaIconLayout> mIconLayout;

    AutoPtr<TabletTicker> mTicker;

    AutoPtr<IView> mFakeSpaceBar;
    AutoPtr<IKeyEvent> mSpaceBarKeyEvent;// = null;

    AutoPtr<IView> mCompatibilityHelpDialog;// = null;

    // for disabling the status bar
    Int32 mDisabled;// = 0;

    Object mScreenshotLock;
    AutoPtr<IServiceConnection> mScreenshotConnection;// = null;
    AutoPtr<IRunnable> mScreenshotTimeout;

private:
    Boolean mTakingScreenShot;

    // Fitts' Law assistance for LatinIME; see policy.EventHole
    static const Boolean FAKE_SPACE_BAR;// = true;

    static const Int32 NOTIFICATION_PRIORITY_MULTIPLIER;// = 10; // see NotificationManagerService
    static const Int32 HIDE_ICONS_BELOW_SCORE;// = Notification.PRIORITY_LOW * NOTIFICATION_PRIORITY_MULTIPLIER;

    Boolean mAltBackButtonEnabledForIme;

    AutoPtr<IInputMethodsPanel> mInputMethodsPanel;
    AutoPtr<ICompatModePanel> mCompatModePanel;

    Int32 mSystemUiVisibility;// = 0;

    Int32 mNavigationIconHints;// = 0;

    Int32 mShowSearchHoldoff;// = 0;

    AutoPtr<IStorageManager> mStorageManager;
    Boolean mShow;// = true;   //add by Derek for volume display,2012.11.26
    Boolean mHasVolume;// = false; // add by zengsc for volume display,2013.5.30

    // Boolean mHWNoBattery;// = Boolean.parseBoolean(SystemProperties.get("hw.nobattery", "false"));
    // Boolean mSoftVolumeKey;// = Boolean.parseBoolean(SystemProperties.get("ro.statusbar.volume", "false"));

    AutoPtr<IRunnable> mShowSearchPanel;

    AutoPtr<IViewOnTouchListener> mHomeSearchActionListener;

    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;

    AutoPtr<IViewOnClickListener> mOnClickListener;

    AutoPtr<IHandler> mHandler;
};

}// namespace Tablet
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_TABLET_TABLETSTATUSBAR_H__
