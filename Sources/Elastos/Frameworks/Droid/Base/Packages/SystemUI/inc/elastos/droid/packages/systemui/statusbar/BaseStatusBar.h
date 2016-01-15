#ifndef __ELASTOS_DROID_SYSTEMUI_STATUSBAR_BASESTATUSBAR_H__
#define __ELASTOS_DROID_SYSTEMUI_STATUSBAR_BASESTATUSBAR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/systemui/SystemUI.h"
#include "elastos/droid/systemui/statusbar/NotificationData.h"
#include "elastos/droid/widget/RemoteViews.h"


using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::App::IActivityOptionsOnAnimationStartedListener;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Widget::IPopupMenu;
using Elastos::Droid::Widget::IPopupMenuOnMenuItemClickListener;
using Elastos::Droid::Widget::RemoteViewsOnClickHandler;
using Elastos::Droid::Widget::IRemoteViewsOnClickHandler;
using Elastos::Droid::SystemUI::ISearchPanelView;
using Elastos::Droid::StatusBar::IIStatusBarService;
using Elastos::Droid::StatusBar::IStatusBarIcon;
using Elastos::Droid::StatusBar::IStatusBarNotification;
using Elastos::Droid::SystemUI::StatusBar::Policy::INotificationRowLayout;
using Elastos::Droid::SystemUI::StatusBar::Tablet::IStatusBarPanel;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class BaseStatusBar : public Elastos::Droid::SystemUI::SystemUI
{
public:
    class MyHandler : public HandlerBase
    {
    public:
        MyHandler(
            /* [in] */ BaseStatusBar* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        BaseStatusBar* mHost;
    };

private:
    class ProvisioningObserver
        : public ContentObserver
    {
    public:
        ProvisioningObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ BaseStatusBar* bar);

        virtual CARAPI OnChange(
            /* [in] */ Boolean selfChange);
    private:
        BaseStatusBar* mHost;
    };

    class StatusBarRemoteViewsOnClickHandler
        : public RemoteViewsOnClickHandler
    {
    public:
        StatusBarRemoteViewsOnClickHandler(
            /* [in] */ BaseStatusBar* bar);

        virtual CARAPI OnClickHandler(
         /* [in] */ IView* view,
         /* [in] */ IPendingIntent* pendingIntent,
         /* [in] */ IIntent* fillInIntent,
         /* [out] */ Boolean* result);

    private:
        BaseStatusBar* mHost;
    };

    class NotificationLongClicker
        : public ElRefBase
        , public IViewOnLongClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        NotificationLongClicker(
            /* [in] */ BaseStatusBar* bar);

        virtual CARAPI OnLongClick(
            /* [in] */ IView* v,
            /* [out] */ Boolean* result);
    private:
        BaseStatusBar* mHost;
    };

    class RecentsPreloadOnTouchListener
        : public ElRefBase
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        RecentsPreloadOnTouchListener(
            /* [in] */ BaseStatusBar* bar);

        virtual CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        BaseStatusBar* mHost;
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
            /* [in] */ BaseStatusBar* bar);

        virtual CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);
    private:
        Int32 mMsg;
        AutoPtr<IStatusBarPanel> mPanel;
        BaseStatusBar* mHost;
    };

    class NotificationClicker
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        NotificationClicker(
            /* [in] */ IPendingIntent* intent,
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ BaseStatusBar* bar);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        AutoPtr<IPendingIntent> mIntent;
        String mPkg;
        String mTag;
        Int32 mId;
        BaseStatusBar* mHost;
    };

    class VetoButtonClicker
        : public ElRefBase
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        VetoButtonClicker(
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ BaseStatusBar* bar);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        String mPkg;
        String mTag;
        Int32 mId;
        BaseStatusBar* mHost;
    };

    class NotificationBlamePopupOnMenuItemClickListener
        : public ElRefBase
        , public IPopupMenuOnMenuItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        NotificationBlamePopupOnMenuItemClickListener(
            /* [in] */ BaseStatusBar* bar,
            /* [in] */ const String& packageName);

        CARAPI OnMenuItemClick(
            /* [in] */ IMenuItem* menu,
            /* [out] */ Boolean* result);

    private:
        BaseStatusBar* mHost;
        String mPackageName;
    };

    class StatusBarBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        StatusBarBroadcastReceiver(
            /* [in] */ BaseStatusBar* bar);

    protected:
        virtual CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("BaseStatusBar::StatusBarBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        BaseStatusBar* mHost;
    };

    class ActivityOptionsOnAnimationStartedListener
        : public ElRefBase
        , public IActivityOptionsOnAnimationStartedListener
    {
    public:
        CAR_INTERFACE_DECL()

        ActivityOptionsOnAnimationStartedListener(
            /* [in] */ BaseStatusBar* bar);

        CARAPI OnAnimationStarted();
    private:
        BaseStatusBar* mHost;
    };

public:
    BaseStatusBar();

    virtual ~BaseStatusBar();

    virtual CARAPI_(PInterface) Probe(
            /* [in] */ REIID riid) = 0;

    Boolean NotificationIsForCurrentUser(
        /* [in] */ IStatusBarNotification* n);

    AutoPtr<IViewOnClickListener> MakeClicker(
        /* [in] */ IPendingIntent* intent,
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id);

    virtual CARAPI Start();

protected:
    // UI-specific methods

    /**
     * Create all windows necessary for the status bar (including navigation, overlay panels, etc)
     * and add them to the window manager.
     */
    virtual CARAPI CreateAndAddWindows() = 0;

    AutoPtr<IIStatusBarService> GetStatusBarService();

    CARAPI_(Boolean) IsDeviceProvisioned();

    virtual CARAPI UserSwitched(
        /* [in] */ Int32 newUserId);

    virtual CARAPI DismissPopups();

    virtual CARAPI DismissIntruder();

    virtual CARAPI ToggleRecentApps();

    virtual CARAPI PreloadRecentApps();

    virtual CARAPI CancelPreloadRecentApps();

    virtual CARAPI ShowSearchPanel();

    virtual CARAPI HideSearchPanel();

    virtual CARAPI UpdateSearchPanel();

    virtual CARAPI UpdateNotification(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* notification);

    CARAPI_(Boolean) InKeyguardRestrictedInputMode();

public:
    // CommandQueueCallbacks interfaces
    //
    virtual CARAPI AddIcon(
        /* [in] */ const String& slot,
        /* [in] */ Int32 index,
        /* [in] */ Int32 viewIndex,
        /* [in] */ IStatusBarIcon* icon) = 0;

    virtual CARAPI UpdateIcon(
        /* [in] */ const String& slot,
        /* [in] */ Int32 index,
        /* [in] */ Int32 viewIndex,
        /* [in] */ IStatusBarIcon* old,
        /* [in] */ IStatusBarIcon* icon) = 0;

    virtual CARAPI RemoveIcon(
        /* [in] */ const String& slot,
        /* [in] */ Int32 index,
        /* [in] */ Int32 viewIndex) = 0;

    virtual CARAPI AddNotification(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* notification) = 0;

    virtual CARAPI RemoveNotification(
        /* [in] */ IBinder* key) = 0;

    virtual CARAPI Disable(
    /* [in] */ Int32 state) = 0;

    virtual CARAPI AnimateExpandNotificationsPanel() = 0;

    virtual CARAPI AnimateCollapsePanels(
        /* [in] */ Int32 flags) = 0;

    virtual CARAPI AnimateExpandSettingsPanel() = 0;

    virtual CARAPI SetSystemUiVisibility(
        /* [in] */ Int32 vis,
        /* [in] */ Int32 mask) = 0;

    virtual CARAPI TopAppWindowChanged(
        /* [in] */ Boolean visible) = 0;

    virtual CARAPI SetImeWindowStatus(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 vis,
        /* [in] */ Int32 backDisposition) = 0;

    virtual CARAPI SetHardKeyboardStatus(
        /* [in] */ Boolean available,
        /* [in] */ Boolean enabled) = 0;

    virtual CARAPI SetNavigationIconHints(
        /* [in] */ Int32 hints) = 0;

protected:
    AutoPtr<IView> UpdateNotificationVetoButton(
        /* [in] */ IView* row,
        /* [in] */ IStatusBarNotification* n);

    CARAPI ApplyLegacyRowBackground(
        /* [in] */ IStatusBarNotification* sbn,
        /* [in] */ IView* content);

    AutoPtr<IViewOnLongClickListener> GetNotificationLongClicker();

    virtual AutoPtr<IWindowManagerLayoutParams> GetRecentsLayoutParams(
        /* [in] */ IViewGroupLayoutParams* layoutParams) = 0;

    virtual AutoPtr<IWindowManagerLayoutParams> GetSearchLayoutParams(
        /* [in] */ IViewGroupLayoutParams* layoutParams) = 0;

    static CARAPI SendCloseSystemWindows(
        /* [in] */ IContext* context,
        /* [in] */ const String& reason);

    virtual AutoPtr<IView> GetStatusBarView();

    virtual CARAPI ToggleRecentsActivity();

    virtual CARAPI PreloadRecentTasksList();

    virtual CARAPI CancelPreloadingRecentTasksList();

    virtual CARAPI WorkAroundBadLayerDrawableOpacity(
        /* [in] */ IView* v);

    virtual Boolean InflateViews(
        /* [in] */ NotificationDataEntry* entry,
        /* [in] */ IViewGroup* parent);

    /**
     * The LEDs are turned o)ff when the notification panel is shown, even just a little bit.
     * This was added last-minute and is inconsistent with the way the rest of the notifications
     * are handled, because the notification isn't really cancelled.  The lights are just
     * turned off.  If any other notifications happen, the lights will turn back on.  Steve says
     * this is what he wants. (see bug 1131461)
     */
    virtual CARAPI VisibilityChanged(
        /* [in] */ Boolean visible);

    /**
     * Cancel this notification and tell the StatusBarManagerService / NotificationManagerService
     * about the failure.
     *
     * WARNING: this will call back into us.  Don't hold any locks.
     */
    virtual CARAPI HandleNotificationError(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* n,
        /* [in] */ const String& message);

    AutoPtr<IStatusBarNotification> RemoveNotificationViews(
        /* [in] */ IBinder* key);

    AutoPtr<IStatusBarIconView> AddNotificationViews(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* notification);

    Boolean ExpandView(
        /* [in] */ NotificationDataEntry* entry,
        /* [in] */ Boolean expand);

    CARAPI UpdateExpansionStates();

    virtual CARAPI HaltTicker() = 0;
    virtual CARAPI SetAreThereNotifications() = 0;
    virtual CARAPI UpdateNotificationIcons() = 0;
    virtual CARAPI Tick(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* n,
        /* [in] */ Boolean firstTime) = 0;
    virtual CARAPI UpdateExpandedViewPos(
        /* [in] */ Int32 expandedPosition) = 0;
    virtual Int32 GetExpandedViewMaxHeight() = 0;
    virtual Boolean ShouldDisableNavbarGestures() = 0;

    virtual Boolean IsTopNotification(
        /* [in] */ IViewGroup* parent,
        /* [in] */ NotificationDataEntry* entry);

    // Q: What kinds of notifications should show during setup?
    // A: Almost none! Only things coming from the system (package is "android") that also
    // have special "kind" tags marking them as relevant for setup (see below).
    Boolean ShowNotificationEvenIfUnprovisioned(
        /* [in] */ IStatusBarNotification* sbn);

protected:
    CARAPI StartApplicationDetailsActivity(
        /* [in] */ const String& packageName);

    CARAPI HandleShowIntruder();

    CARAPI HandleHideIntruder();

public:
    static const String TAG;
    static const Boolean DEBUG;
    static const Boolean MULTIUSER_DEBUG;

protected:
    friend class ProvisioningObserver;

    static const Int32 MSG_TOGGLE_RECENTS_PANEL;
    static const Int32 MSG_CLOSE_RECENTS_PANEL;
    static const Int32 MSG_PRELOAD_RECENT_APPS;
    static const Int32 MSG_CANCEL_PRELOAD_RECENT_APPS;
    static const Int32 MSG_OPEN_SEARCH_PANEL;
    static const Int32 MSG_CLOSE_SEARCH_PANEL;
    static const Int32 MSG_SHOW_INTRUDER;
    static const Int32 MSG_HIDE_INTRUDER;

    static const Boolean ENABLE_INTRUDERS;

    AutoPtr<ICommandQueue> mCommandQueue;
    AutoPtr<IIStatusBarService> mBarService;
    AutoPtr<IHandler> mHander;

    // all notifications
    AutoPtr<NotificationData> mNotificationData;
    AutoPtr<INotificationRowLayout> mPile;

    AutoPtr<IStatusBarNotification> mCurrentlyIntrudingNotification;

    // used to notify status bar for suppressing notification LED
    Boolean mPanelSlightlyVisible;

    // Search panel
    AutoPtr<ISearchPanelView> mSearchPanelView;

    AutoPtr<IPopupMenu> mNotificationBlamePopup;

    Int32 mCurrentUserId;

    AutoPtr<IWindowManager> mWindowManager;
    AutoPtr<IIWindowManager> mWindowManagerService;
    AutoPtr<IDisplay> mDisplay;

    AutoPtr<IRemoteViewsOnClickHandler> mOnClickHandler;

    AutoPtr<IHandler> mHandler;

    AutoPtr<ContentObserver> mProvisioningObserver;
    AutoPtr<IViewOnTouchListener> mRecentsPreloadOnTouchListener;

private:
    Boolean mDeviceProvisioned;
};

}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_STATUSBAR_BASESTATUSBAR_H__
