//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_BASESTATUSBAR_H__
#define  __ELASTOS_DROID_SYSTEMUI_STATUSBAR_BASESTATUSBAR_H__

#include <elastos/droid/ext/frameworkext.h>
#include "_Elastos.Droid.Core.h"
#include "elastos/droid/systemui/SystemUI.h"
#include "elastos/droid/systemui/statusbar/NotificationData.h"
#include <elastos/droid/animation/AnimatorListenerAdapter.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/service/notification/NotificationListenerService.h>
#include <elastos/droid/widget/RemoteViews.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Animation::AnimatorListenerAdapter;
using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::SystemUI::Keyguard::IKeyguardHostViewOnDismissAction;
using Elastos::Droid::Internal::StatusBar::IIStatusBarService;
using Elastos::Droid::Internal::StatusBar::IStatusBarIcon;
using Elastos::Droid::Internal::Utility::INotificationColorUtil;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::IViewOnLongClickListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Widget::RemoteViews;
using Elastos::Droid::Widget::IRemoteViewsOnClickHandler;
using Elastos::Droid::Service::Dreams::IIDreamManager;
using Elastos::Droid::Service::Notification::INotificationListenerService;
using Elastos::Droid::Service::Notification::IStatusBarNotification;
using Elastos::Droid::Service::Notification::NotificationListenerService;
using Elastos::Droid::SystemUI::ISearchPanelView;
using Elastos::Droid::SystemUI::StatusBar::Phone::INavigationBarView;
using Elastos::Droid::SystemUI::StatusBar::Phone::IStatusBarKeyguardViewManager;
using Elastos::Droid::SystemUI::StatusBar::Policy::IHeadsUpNotificationView;
using Elastos::Droid::SystemUI::StatusBar::Stack::INotificationStackScrollLayout;
using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {

class CSettingsObserver;
class CLockscreenSettingsObserver;
class CBaseBroadcastReceiver;
class DelegateViewHelper;
class CNotificationListenerService;

class BaseStatusBar
    : public SystemUI
    , public IBaseStatusBar
    , public ICommandQueueCallbacks
    , public IRecentsComponentCallbacks
    , public IExpansionLogger
    , public INotificationEnvironment
{
public:
    class TouchOutsideListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        TouchOutsideListener(
            /* [in] */ Int32 msg,
            /* [in] */ IStatusBarPanel* panel,
            /* [in] */ BaseStatusBar* host);

        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* ev,
            /* [out] */ Boolean* result);

    private:
        Int32 mMsg;
        AutoPtr<IStatusBarPanel> mPanel;
        BaseStatusBar* mHost;
    };

protected:
    class ActivatableNotificationViewListener
        : public Object
        , public IActivatableNotificationViewOnActivatedListener
    {
    public:
        CAR_INTERFACE_DECL()

        ActivatableNotificationViewListener(
            /* [in] */ BaseStatusBar* host);

        // @Override
        CARAPI OnActivated(
            /* [in] */ IActivatableNotificationView* view);

        // @Override
        CARAPI OnActivationReset(
            /* [in] */ IActivatableNotificationView* view);

    private:
        BaseStatusBar* mHost;
    };

    class RecentsPreloadOnTouchListener
        : public Object
        , public IViewOnTouchListener
    {
    public:
        CAR_INTERFACE_DECL()

        RecentsPreloadOnTouchListener(
            /* [in] */ BaseStatusBar* host);

        // additional optimization when we have software system buttons - start loading the recent
        // tasks on touch down
        // @Override
        CARAPI OnTouch(
            /* [in] */ IView* v,
            /* [in] */ IMotionEvent* event,
            /* [out] */ Boolean* result);

    private:
        BaseStatusBar* mHost;
    };

    class H : public Handler
    {
    public:
        TO_STRING_IMPL("BaseStatusBar::H")

        H(
            /* [in] */ BaseStatusBar* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* m);

    private:
        BaseStatusBar* mHost;
    };

    class NotificationClicker;
    class KeyguardHostViewOnDismissAction;
    class KeyguardHostViewOnDismissActionThread
        : public Thread
    {
    public:
        KeyguardHostViewOnDismissActionThread(
            /* [in] */ BaseStatusBar* host,
            /* [in] */ KeyguardHostViewOnDismissAction* action);

        CARAPI Run();

    private:
        BaseStatusBar* mHost;
        KeyguardHostViewOnDismissAction* mAction;
    };

    class KeyguardHostViewOnDismissAction
        : public Object
        , public IKeyguardHostViewOnDismissAction
    {
    public:
        TO_STRING_IMPL("BaseStatusBar::KeyguardHostViewOnDismissAction")

        CAR_INTERFACE_DECL()

        KeyguardHostViewOnDismissAction(
            /* [in] */ BaseStatusBar* host,
            /* [in] */ NotificationClicker* clicker,
            /* [in] */ Boolean keyguardShowing,
            /* [in] */ Boolean afterKeyguardGone);
        /**
         * @return true if the dismiss should be deferred
         */
        CARAPI OnDismiss(
            /* [out] */ Boolean* result);

    private:
        friend class KeyguardHostViewOnDismissActionThread;
        BaseStatusBar* mHost;
        NotificationClicker* mClicker;
        Boolean mKeyguardShowing;
        Boolean mAfterKeyguardGone;
    };

    class NotificationClicker
        : public Object
        , public IViewOnClickListener
        , public INotificationClicker
    {
    public:
        TO_STRING_IMPL("BaseStatusBar::NotificationClicker")

        CAR_INTERFACE_DECL()

        NotificationClicker(
            /* [in] */ IPendingIntent* intent,
            /* [in] */ const String& notificationKey,
            /* [in] */ Boolean forHun,
            /* [in] */ BaseStatusBar* host);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        friend class KeyguardHostViewOnDismissAction;
        friend class KeyguardHostViewOnDismissActionThread;
        AutoPtr<IPendingIntent> mIntent;
        String mNotificationKey;
        Boolean mIsHeadsUp;
        BaseStatusBar* mHost;
    };

private:

    class NotificationGutsOnDismissAction;
    class NotificationGutsRunnable
        : public Runnable
    {
    public:
        NotificationGutsRunnable(
            /* [in] */ BaseStatusBar* host,
            /* [in] */ NotificationGutsOnDismissAction* mAction);

        CARAPI Run();

    private:
        BaseStatusBar* mHost;
        AutoPtr<NotificationGutsOnDismissAction> mAction;
    };

    class NotificationGutsOnDismissAction
        : public Object
        , public IKeyguardHostViewOnDismissAction
    {
    public:
        TO_STRING_IMPL("BaseStatusBar::NotificationGutsOnDismissAction")

        CAR_INTERFACE_DECL()

        NotificationGutsOnDismissAction(
            /* [in] */ BaseStatusBar* host,
            /* [in] */ Boolean keyguardShowing,
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 appUid);

        /**
         * @return true if the dismiss should be deferred
         */
        CARAPI OnDismiss(
            /* [out] */ Boolean* result);

    private:
        friend class NotificationGutsRunnable;
        BaseStatusBar* mHost;
        Boolean mKeyguardShowing;
        AutoPtr<IIntent> mIntent;
        Int32 mAppUid;
    };

    class NotificationRemoteViewsOnClickHandler;
    class NotificationRemoteViewsOnDismissAction
        : public Object
        , public IKeyguardHostViewOnDismissAction
    {
    public:
        TO_STRING_IMPL("BaseStatusBar::NotificationRemoteViewsOnDismissAction")

        CAR_INTERFACE_DECL()

        NotificationRemoteViewsOnDismissAction(
            /* [in] */ BaseStatusBar* host,
            /* [in] */ NotificationRemoteViewsOnClickHandler* clickHandler,
            /* [in] */ Boolean keyguardShowing,
            /* [in] */ Boolean afterKeyguardGone,
            /* [in] */ IView* view,
            /* [in] */ IPendingIntent* pendingIntent,
            /* [in] */ IIntent* fillInIntent);

        /**
         * @return true if the dismiss should be deferred
         */
        CARAPI OnDismiss(
            /* [out] */ Boolean* result);

    private:
        BaseStatusBar* mHost;
        NotificationRemoteViewsOnClickHandler* mClickHandler;
        Boolean mKeyguardShowing;
        Boolean mAfterKeyguardGone;
        AutoPtr<IView> mView;
        AutoPtr<IPendingIntent> mPendingIntent;
        AutoPtr<IIntent> mFillInIntent;
    };

    class NotificationRemoteViewsOnClickHandler
        : public RemoteViews::RemoteViewsOnClickHandler
    {
    public:
        NotificationRemoteViewsOnClickHandler(
            /* [in] */ BaseStatusBar* host);

        // @Override
        CARAPI OnClickHandler(
            /* [in] */ IView* view,
            /* [in] */ IPendingIntent* pendingIntent,
            /* [in] */ IIntent* fillInIntent,
            /* [out] */ Boolean* result);

    private:
        CARAPI_(Boolean) SuperOnClickHandler(
            /* [in] */ IView* view,
            /* [in] */ IPendingIntent* pendingIntent,
            /* [in] */ IIntent* fillInIntent);

    private:
        friend class NotificationRemoteViewsOnDismissAction;
        BaseStatusBar* mHost;
    };

    class RecentsComponentCallbacks
        : public Object
        , public IRecentsComponentCallbacks
    {
    public:
        CAR_INTERFACE_DECL()

        RecentsComponentCallbacks(
            /* [in] */ BaseStatusBar* host);

        CARAPI OnVisibilityChanged(
            /* [in] */ Boolean visible);

    private:
        BaseStatusBar* mHost;
    };

    class ViewOnClickListener1
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ViewOnClickListener1(
            /* [in] */ BaseStatusBar* host,
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ Int32 userid);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        BaseStatusBar* mHost;
        String mPkg;
        String mTag;
        Int32 mId;
        Int32 mUserId;
    };

    class ViewOnClickListener2
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ViewOnClickListener2(
            /* [in] */ BaseStatusBar* host,
            /* [in] */ const String& pkg,
            /* [in] */ Int32 appUidF);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        BaseStatusBar* mHost;
        String mPkg;
        Int32 mAppUidF;
    };

    class ViewOnClickListener3
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        ViewOnClickListener3(
            /* [in] */ BaseStatusBar* host,
            /* [in] */ IIntent* appSettingsLaunchIntent,
            /* [in] */ IStatusBarNotification* sbn,
            /* [in] */ Int32 appUidF);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        BaseStatusBar* mHost;
        AutoPtr<IIntent> mIntent;
        AutoPtr<IStatusBarNotification> mSbn;
        Int32 mAppUidF;
    };

    class BaseLongPressListener
        : public Object
        , public ISwipeHelperLongPressListener
    {
    public:
        CAR_INTERFACE_DECL()

        BaseLongPressListener(
            /* [in] */ BaseStatusBar* host);

        // @Override
        CARAPI OnLongPress(
            /* [in] */ IView* v,
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [out] */ Boolean* result);

    private:
        BaseStatusBar* mHost;
    };

    class BaseAnimatorListenerAdapter: public AnimatorListenerAdapter
    {
    public:
        BaseAnimatorListenerAdapter(
            /* [in] */ IView* v);

        // @Override
        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<IView> mView;
    };

public:
    CAR_INTERFACE_DECL()

    BaseStatusBar();

    CARAPI constructor();

    // @Override  // NotificationData.Environment
    CARAPI IsDeviceProvisioned(
        /* [out] */ Boolean* result);

    CARAPI Start();

    CARAPI UserSwitched(
        /* [in] */ Int32 newUserId);

    CARAPI IsHeadsUp(
        /* [in] */ const String& key,
        /* [out] */ Boolean* result);

    // @Override  // NotificationData.Environment
    CARAPI IsNotificationForCurrentProfiles(
        /* [in] */ IStatusBarNotification* n,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetCurrentMediaNotificationKey(
        /* [out] */ String* str);

    CARAPI IsMediaNotification(
        /* [in] */ INotificationDataEntry* entry,
        /* [out] */ Boolean* result);

    CARAPI DismissPopups();

    CARAPI OnHeadsUpDismissed();

    // @Override
    CARAPI ShowRecentApps(
        /* [in] */ Boolean triggeredFromAltTab);

    // @Override
    CARAPI HideRecentApps(
        /* [in] */ Boolean triggeredFromAltTab,
        /* [in] */ Boolean triggeredFromHomeKey);

    // @Override
    CARAPI ToggleRecentApps();

    // @Override
    CARAPI PreloadRecentApps();

    // @Override
    CARAPI CancelPreloadRecentApps();

    /** Jumps to the next affiliated task in the group. */
    CARAPI ShowNextAffiliatedTask();

    /** Jumps to the previous affiliated task in the group. */
    CARAPI ShowPreviousAffiliatedTask();

    // @Override
    CARAPI ShowSearchPanel();

    // @Override
    CARAPI HideSearchPanel();

    static CARAPI_(void) SendCloseSystemWindows(
        /* [in] */ IContext* context,
        /* [in] */ const String& reason);

    // @Override
    CARAPI OnVisibilityChanged(
        /* [in] */ Boolean visible);

    virtual CARAPI ResetHeadsUpDecayTimer() = 0;

    virtual CARAPI ScheduleHeadsUpOpen() = 0;

    virtual CARAPI ScheduleHeadsUpClose() = 0;

    virtual CARAPI ScheduleHeadsUpEscalation() = 0;

    virtual CARAPI AddNotification(
        /* [in] */ IStatusBarNotification* notification,
        /* [in] */ INotificationListenerServiceRankingMap* ranking) = 0;

    virtual CARAPI RemoveNotification(
        /* [in] */ const String& key,
        /* [in] */ INotificationListenerServiceRankingMap* ranking) = 0;

    /**
     * Save the current "public" (locked and secure) state of the lockscreen.
     */
    CARAPI SetLockscreenPublicMode(
        /* [in] */ Boolean publicMode);

    CARAPI IsLockscreenPublicMode(
        /* [out] */ Boolean* result);

    /**
     * Has the given user chosen to allow their private (full) notifications to be shown even
     * when the lockscreen is in "public" (secure & locked) mode?
     */
    CARAPI UserAllowsPrivateNotificationsInPublic(
        /* [in] */ Int32 userHandle,
        /* [out] */ Boolean* result);

    /**
     * Returns TRUE if we're on a secure lockscreen and the user wants to hide "sensitive"
     * notification data. If so, private notifications should show their (possibly
     * auto-generated) publicVersion, and secret notifications should be totally invisible.
     */
    // @Override  // NotificationData.Environment
    CARAPI ShouldHideSensitiveContents(
        /* [in] */ Int32 userid,
        /* [out] */ Boolean* result);

    CARAPI OnNotificationClear(
        /* [in] */ IStatusBarNotification* notification);

    CARAPI MakeClicker(
        /* [in] */ IPendingIntent* intent,
        /* [in] */ const String& notificationKey,
        /* [in] */ Boolean forHun,
        /* [out] */ INotificationClicker** clicker);

    using ICommandQueueCallbacks::AnimateCollapsePanels;

    CARAPI AnimateCollapsePanels(
        /* [in] */ Int32 flags,
        /* [in] */ Boolean force);

    CARAPI OverrideActivityPendingAppTransition(
        /* [in] */ Boolean keyguardShowing);

    /**
     * Cancel this notification and tell the StatusBarManagerService / NotificationManagerService
     * about the failure.
     *
     * WARNING: this will call back into us.  Don't hold any locks.
     */
    CARAPI_(void) HandleNotificationError(
        /* [in] */ IStatusBarNotification* n,
        /* [in] */ const String& message);

    CARAPI UpdateNotification(
        /* [in] */ IStatusBarNotification* notification,
        /* [in] */ INotificationListenerServiceRankingMap* ranking);

    CARAPI InKeyguardRestrictedInputMode(
        /* [out] */ Boolean* result);

    CARAPI SetInteracting(
        /* [in] */ Int32 barWindow,
        /* [in] */ Boolean interacting);

    CARAPI SetBouncerShowing(
        /* [in] */ Boolean bouncerShowing);

    /**
     * @return Whether the security bouncer from Keyguard is showing.
     */
    CARAPI IsBouncerShowing(
        /* [out] */ Boolean* result);

    CARAPI Destroy();

    // @Override
    CARAPI LogNotificationExpansion(
        /* [in] */ const String& key,
        /* [in] */ Boolean userAction,
        /* [in] */ Boolean expanded);

    /**
     * @return The number of notifications we show on Keyguard.
     */
    virtual CARAPI_(Int32) GetMaxKeyguardNotifications() = 0;

    // @Override
    virtual CARAPI OnActivated(
        /* [in] */ IActivatableNotificationView* view) = 0;

    // @Override
    virtual CARAPI OnActivationReset(
        /* [in] */ IActivatableNotificationView* view) = 0;

protected:
    // UI-specific methods
    /**
     * Create all windows necessary for the status bar (including navigation, overlay panels, etc)
     * and add them to the window manager.
     */
    virtual CARAPI_(void) CreateAndAddWindows() = 0;

    virtual CARAPI_(void) RefreshLayout(
        /* [in] */ Int32 layoutDirection) = 0;

    virtual CARAPI_(void) NotifyUserAboutHiddenNotifications();

    /**
     * Takes the necessary steps to prepare the status bar for starting an activity, then starts it.
     * @param action A dismiss action that is called if it's safe to start the activity.
     * @param afterKeyguardGone Whether the action should be executed after the Keyguard is gone.
     */
    virtual CARAPI_(void) DismissKeyguardThenExecute(
        /* [in] */ IKeyguardHostViewOnDismissAction* action,
        /* [in] */ Boolean afterKeyguardGone);

    // @Override
    virtual CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    virtual CARAPI_(AutoPtr<IView>) UpdateNotificationVetoButton(
        /* [in] */ IView* row,
        /* [in] */ IStatusBarNotification* n);

    virtual CARAPI_(void) ApplyColorsAndBackgrounds(
        /* [in] */ IStatusBarNotification* sbn,
        /* [in] */ INotificationDataEntry* entry);

    virtual CARAPI_(AutoPtr<ISwipeHelperLongPressListener>) GetNotificationLongClicker();

    virtual CARAPI_(AutoPtr<IWindowManagerLayoutParams>) GetSearchLayoutParams(
        /* [in] */ IViewGroupLayoutParams* layoutParams) = 0;

    virtual CARAPI_(void) UpdateSearchPanel();

    virtual CARAPI_(AutoPtr<H>) CreateHandler();

    virtual CARAPI GetStatusBarView(
        /* [out] */ IView** view) = 0;

    /** Proxy for RecentsComponent */
    virtual CARAPI_(void) ShowRecents(
        /* [in] */ Boolean triggeredFromAltTab);

    virtual CARAPI_(void) HideRecents(
        /* [in] */ Boolean triggeredFromAltTab,
        /* [in] */ Boolean triggeredFromHomeKey);

    virtual CARAPI_(void) ToggleRecents();

    virtual CARAPI_(void) PreloadRecents();

    virtual CARAPI_(void) CancelPreloadingRecents();

    virtual CARAPI_(void) ShowRecentsNextAffiliatedTask();

    virtual CARAPI_(void) ShowRecentsPreviousAffiliatedTask();

    virtual CARAPI_(void) WorkAroundBadLayerDrawableOpacity(
        /* [in] */ IView* v);

    virtual CARAPI_(Boolean) InflateViewsForHeadsUp(
        /* [in] */ INotificationDataEntry* entry,
        /* [in] */ IViewGroup* parent);

    /**
     * The LEDs are turned o)ff when the notification panel is shown, even just a little bit.
     * This was added last-minute and is inconsistent with the way the rest of the notifications
     * are handled, because the notification isn't really cancelled.  The lights are just
     * turned off.  If any other notifications happen, the lights will turn back on.  Steve says
     * this is what he wants. (see bug 1131461)
     */
    virtual CARAPI_(void) VisibilityChanged(
        /* [in] */ Boolean visible);

    virtual CARAPI_(AutoPtr<IStatusBarNotification>) RemoveNotificationViews(
        /* [in] */ const String& key,
        /* [in] */ INotificationListenerServiceRankingMap* ranking);

    virtual CARAPI_(AutoPtr<INotificationDataEntry>) CreateNotificationViews(
        /* [in] */ IStatusBarNotification* sbn);

    virtual CARAPI_(void) AddNotificationViews(
        /* [in] */ INotificationDataEntry* entry,
        /* [in] */ INotificationListenerServiceRankingMap* ranking);

    /**
     * Updates expanded, dimmed and locked states of notification rows.
     */
    virtual CARAPI_(void) UpdateRowStates();

    virtual CARAPI_(void) SetZenMode(
        /* [in] */ Int32 mode);

    // extended in PhoneStatusBar
    virtual CARAPI_(void) SetShowLockscreenNotifications(
        /* [in] */ Boolean show);

    virtual CARAPI_(void) HaltTicker() = 0;

    virtual CARAPI_(void) SetAreThereNotifications() = 0;

    virtual CARAPI_(void) UpdateNotifications() = 0;

    virtual CARAPI_(void) Tick(
        /* [in] */ IStatusBarNotification* n,
        /* [in] */ Boolean firstTime) = 0;

    virtual CARAPI_(void) UpdateExpandedViewPos(
        /* [in] */ Int32 expandedPosition) = 0;

    virtual CARAPI_(Boolean) ShouldDisableNavbarGestures() = 0;

    virtual CARAPI UpdateNotificationRanking(
        /* [in] */ INotificationListenerServiceRankingMap* ranking) = 0;

    virtual CARAPI_(void) NotifyHeadsUpScreenOn(
        /* [in] */ Boolean screenOn);

    virtual CARAPI_(Boolean) ShouldInterrupt(
        /* [in] */ IStatusBarNotification* sbn);

    /**
     * @return a PackageManger for userId or if userId is < 0 (USER_ALL etc) then
     *         return PackageManager for mContext
     */
    virtual CARAPI_(AutoPtr<IPackageManager>) GetPackageManagerForUser(
        /* [in] */ Int32 userId);

    virtual CARAPI_(String) GetClass() = 0;

private:
    CARAPI_(void) UpdateCurrentProfilesCache();

    // The gear button in the guts that links to the app's own notification settings
    CARAPI_(void) StartAppOwnNotificationSettingsActivity(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 notificationId,
        /* [in] */ const String& notificationTag,
        /* [in] */ Int32 appUid);

    // The (i) button in the guts that links to the system notification settings for that app
    CARAPI_(void) StartAppNotificationSettingsActivity(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 appUid);

    CARAPI_(void) StartNotificationGutsIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 appUid);

    CARAPI_(void) InflateGuts(
        /* [in] */ IExpandableNotificationRow* row);

    CARAPI_(Boolean) InflateViews(
        /* [in] */ INotificationDataEntry* entry,
        /* [in] */ IViewGroup* parent);

    CARAPI_(Boolean) InflateViews(
        /* [in] */ INotificationDataEntry* entry,
        /* [in] */ IViewGroup* parent,
        /* [in] */ Boolean isHeadsUp);

    CARAPI_(Boolean) ShouldShowOnKeyguard(
        /* [in] */ IStatusBarNotification* sbn);

    CARAPI_(void) UpdateLockscreenNotificationSetting();

    CARAPI_(void) UpdateNotificationViews(
        /* [in] */ INotificationDataEntry* entry,
        /* [in] */ IStatusBarNotification* notification);

    CARAPI_(void) UpdateHeadsUpViews(
        /* [in] */ INotificationDataEntry* entry,
        /* [in] */ IStatusBarNotification* notification);

    CARAPI_(void) UpdateNotificationViews(
        /* [in] */ INotificationDataEntry* entry,
        /* [in] */ IStatusBarNotification* notification,
        /* [in] */ Boolean isHeadsUp);

    CARAPI_(Boolean) AlertAgain(
        /* [in] */ INotificationDataEntry* oldEntry,
        /* [in] */ INotification* newNotification);

public:
    static const Boolean DEBUG;
    static const Boolean MULTIUSER_DEBUG;
    // Should match the value in PhoneWindowManager
    static const String SYSTEM_DIALOG_REASON_RECENT_APPS;

    static const Int32 EXPANDED_LEAVE_ALONE;
    static const Int32 EXPANDED_FULL_OPEN;

protected:
    static const Int32 MSG_SHOW_RECENT_APPS = 1019;
    static const Int32 MSG_HIDE_RECENT_APPS = 1020;
    static const Int32 MSG_TOGGLE_RECENTS_APPS = 1021;
    static const Int32 MSG_PRELOAD_RECENT_APPS = 1022;
    static const Int32 MSG_CANCEL_PRELOAD_RECENT_APPS = 1023;
    static const Int32 MSG_SHOW_NEXT_AFFILIATED_TASK = 1024;
    static const Int32 MSG_SHOW_PREV_AFFILIATED_TASK = 1025;
    static const Int32 MSG_CLOSE_SEARCH_PANEL = 1027;
    static const Int32 MSG_SHOW_HEADS_UP = 1028;
    static const Int32 MSG_HIDE_HEADS_UP = 1029;
    static const Int32 MSG_ESCALATE_HEADS_UP = 1030;
    static const Int32 MSG_DECAY_HEADS_UP = 1031;

    static const Boolean ENABLE_HEADS_UP;
    // scores above this threshold should be displayed in heads up mode.
    static const Int32 INTERRUPTION_THRESHOLD;
    static const String SETTING_HEADS_UP_TICKER;
    AutoPtr<ICommandQueue> mCommandQueue;
    AutoPtr<IIStatusBarService> mBarService;
    AutoPtr<H> mHandler;

    // all notifications
    AutoPtr<NotificationData> mNotificationData;
    AutoPtr<INotificationStackScrollLayout> mStackScroller;

    // for heads up notifications
    AutoPtr<IHeadsUpNotificationView> mHeadsUpNotificationView;
    Int32 mHeadsUpNotificationDecay;

    // used to notify status bar for suppressing notification LED
    Boolean mPanelSlightlyVisible;

    // Search panel
    AutoPtr<ISearchPanelView> mSearchPanelView;

    Int32 mCurrentUserId;
    AutoPtr<ISparseArray> mCurrentProfiles;  /*<UserInfo*/

    Int32 mLayoutDirection; // invalid
    AutoPtr<IAccessibilityManager> mAccessibilityManager;

    // on-screen navigation buttons
    AutoPtr<INavigationBarView> mNavigationBarView;
    AutoPtr<ILocale> mLocale;

    Boolean mUseHeadsUp;
    Boolean mHeadsUpTicker;
    Boolean mDisableNotificationAlerts;

    AutoPtr<IDevicePolicyManager> mDevicePolicyManager;
    AutoPtr<IIDreamManager> mDreamManager;
    AutoPtr<IPowerManager> mPowerManager;
    AutoPtr<IStatusBarKeyguardViewManager> mStatusBarKeyguardViewManager;
    Int32 mRowMinHeight;
    Int32 mRowMaxHeight;

    AutoPtr<IWindowManager> mWindowManager;
    AutoPtr<IIWindowManager> mWindowManagerService;
    AutoPtr<IDisplay> mDisplay;
    Int32 mZenMode;

    /**
     * The {@link StatusBarState} of the status bar.
     */
    Int32 mState;
    Boolean mBouncerShowing;
    Boolean mShowLockscreenNotifications;

    AutoPtr<INotificationOverflowContainer> mKeyguardIconOverflowContainer;
    AutoPtr<IDismissView> mDismissView;
    AutoPtr<IEmptyShadeView> mEmptyShadeView;

    AutoPtr<IContentObserver> mSettingsObserver;
    AutoPtr<IViewOnTouchListener> mRecentsPreloadOnTouchListener;

private:
    static const Int32 HIDDEN_NOTIFICATION_ID;
    static const String BANNER_ACTION_CANCEL;
    static const String BANNER_ACTION_SETUP;

    // public mode, private notifications, etc
    Boolean mLockscreenPublicMode;
    AutoPtr<ISparseBooleanArray> mUsersAllowingPrivateNotifications;
    AutoPtr<INotificationColorUtil> mNotificationColorUtil;
    Float mFontScale;
    AutoPtr<IUserManager> mUserManager;
    Boolean mDeviceProvisioned;
    AutoPtr<IRecentsComponent> mRecents;
    // which notification is currently being longpress-examined by the user
    AutoPtr<INotificationGuts> mNotificationGutsExposed;
    AutoPtr<ITimeInterpolator> mLinearOutSlowIn;
    AutoPtr<ITimeInterpolator> mFastOutLinearIn;
    AutoPtr<IContentObserver> mLockscreenSettingsObserver;
    AutoPtr<IRemoteViewsOnClickHandler> mOnClickHandler;
    AutoPtr<IBroadcastReceiver> mBroadcastReceiver;
    AutoPtr<INotificationListenerService> mNotificationListener;

    AutoPtr<IContext> mContextForUser;

    friend class CSettingsObserver;
    friend class CLockscreenSettingsObserver;
    friend class CBaseBroadcastReceiver;
    friend class DelegateViewHelper;
    friend class CNotificationListenerService;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_STATUSBAR_BASESTATUSBAR_H__
