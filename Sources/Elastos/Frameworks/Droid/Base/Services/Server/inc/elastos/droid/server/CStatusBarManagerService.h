#ifndef __ELASTOS_DROID_SERVER_CSTATUSBARMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CSTATUSBARMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_CStatusBarManagerService.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include "wm/CWindowManagerService.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::StatusBar::IIStatusBar;
using Elastos::Droid::StatusBar::IStatusBarIconList;
using Elastos::Droid::StatusBar::IStatusBarNotification;
using Elastos::Droid::Server::Wm::CWindowManagerService;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CStatusBarManagerService)
    , public CWindowManagerService::IOnHardKeyboardStatusChangeListener
{
public:
    interface INotificationCallbacks : public IInterface
    {
        virtual CARAPI OnSetDisabled(
            /* [in] */ Int32 status) = 0;

        virtual CARAPI OnClearAll() = 0;

        virtual CARAPI OnNotificationClick(
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id) = 0;

        virtual CARAPI OnNotificationClear(
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id) = 0;

        virtual CARAPI OnPanelRevealed() = 0;

        virtual CARAPI OnNotificationError(
            /* [in] */ const String& pkg,
            /* [in] */ const String& tag,
            /* [in] */ Int32 id,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 initialPid,
            /* [in] */ const String& message) = 0;
    };

private:
    class DisableRecord
        : public ElRefBase
        , public IProxyDeathRecipient
    {
        friend class CStatusBarManagerService;

    public:
        DisableRecord(
            /* [in] */ CStatusBarManagerService* host);

        CAR_INTERFACE_DECL();

        CARAPI ProxyDied();

    protected:
        Int32 mUserId;
        String mPkg;
        Int32 mWhat;
        AutoPtr<IBinder> mToken;
        CStatusBarManagerService* mHost;
    };

    class DisableNotificationRunnable
        : public Runnable
    {
    public:
        DisableNotificationRunnable(
            /* [in] */ Int32 status,
            /* [in] */ CStatusBarManagerService* host);

        CARAPI Run();

    private:
        Int32 mStatus;
        CStatusBarManagerService* mHost;
    };

    class TopAppWindowChangedRunnable
        : public Runnable
    {
    public:
        TopAppWindowChangedRunnable(
            /* [in] */ Boolean menuVisible,
            /* [in] */ CStatusBarManagerService* host);

        CARAPI Run();

    private:
        Boolean mMenuVisible;
        CStatusBarManagerService* mHost;
    };

    class SetImeWindowStatusRunnable : public Runnable
    {
    public:
        SetImeWindowStatusRunnable(
            /* [in] */ IBinder* token,
            /* [in] */ Int32 vis,
            /* [in] */ Int32 backDisposition,
            /* [in] */ CStatusBarManagerService* host);

        CARAPI Run();

    private:
        AutoPtr<IBinder> mToken;
        Int32 mVisible;
        Int32 mBackDisposition;
        CStatusBarManagerService* mHost;
    };

    class UpdateUiVisibilityRunnable
        : public Runnable
    {
    public:
        UpdateUiVisibilityRunnable(
            /* [in] */ Int32 vis,
            /* [in] */ Int32 mask,
            /* [in] */ CStatusBarManagerService* host);

        CARAPI Run();

    private:
        Int32 mVis;
        Int32 mMask;
        CStatusBarManagerService* mHost;
    };

    class SetHardKeyboardEnabledRunnable
        : public Runnable
    {
    public:
        SetHardKeyboardEnabledRunnable(
            /* [in] */ Boolean enable,
            /* [in] */ CStatusBarManagerService* host);

        CARAPI Run();

    private:
        Boolean mEnabled;
        CStatusBarManagerService* mHost;
    };

    class HardKeyboardStatusChangeRunnable
        : public Runnable
    {
    public:
        HardKeyboardStatusChangeRunnable(
            /* [in] */ Boolean available,
            /* [in] */ Boolean enabled,
            /* [in] */ CStatusBarManagerService* host);

        CARAPI Run();

    private:
        Boolean mAvailable;
        Boolean mEnabled;
        CStatusBarManagerService* mHost;
    };

    class MBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        MBroadcastReceiver(
            /* [in] */ CStatusBarManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CStatusBarManagerService::MBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CStatusBarManagerService* mHost;
    };

public:
    CStatusBarManagerService();

    CAR_INTERFACE_DECL()

    //@Override
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Construct the service, add the status bar view to the window manager
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ Handle32 windowManager);

    CARAPI SetNotificationCallbacks(
        /* [in] */ INotificationCallbacks* listener);

    // ================================================================================
    // From IStatusBarService
    // ================================================================================
    CARAPI ExpandNotificationsPanel();

    CARAPI CollapsePanels();

    CARAPI ExpandSettingsPanel();

    CARAPI Disable(
        /* [in] */ Int32 what,
        /* [in] */ IBinder* token,
        /* [in] */ const String& pkg);

    CARAPI SetIcon(
        /* [in] */ const String& slot,
        /* [in] */ const String& iconPackage,
        /* [in] */ Int32 iconId,
        /* [in] */ Int32 iconLevel,
        /* [in] */ const String& contentDescription);

    CARAPI SetIconVisibility(
        /* [in] */ const String& slot,
        /* [in] */ Boolean visible);

    CARAPI RemoveIcon(
        /* [in] */ const String& slot);

    /**
     * Hide or show the on-screen Menu key. Only call this from the window manager, typically in
     * response to a window with FLAG_NEEDS_MENU_KEY set.
     */
    CARAPI TopAppWindowChanged(
        /* [in] */ Boolean menuVisible);

    CARAPI SetImeWindowStatus(
        /* [in] */ IBinder* token,
        /* [in] */ Int32 vis,
        /* [in] */ Int32 backDisposition);

    CARAPI SetSystemUiVisibility(
        /* [in] */ Int32 vis,
        /* [in] */ Int32 mask);

    CARAPI SetHardKeyboardEnabled(
        /* [in] */ Boolean enabled);

    //@Override
    CARAPI_(void) OnHardKeyboardStatusChange(
        /* [in] */ Boolean available,
        /* [in] */ Boolean enabled);

    //@Override
    CARAPI ToggleRecentApps();

    //@Override
    CARAPI PreloadRecentApps();

    //@Override
    CARAPI CancelPreloadRecentApps();

    //@Override
    CARAPI SetCurrentUser(
        /* [in] */ Int32 newUserId);

    // ================================================================================
    // Callbacks from the status bar service.
    // ================================================================================
    CARAPI RegisterStatusBar(
        /* [in] */ IIStatusBar* bar,
        /* [out] */ IStatusBarIconList** iconList,
        /* [out] */ IObjectContainer** notificationKeys,/*List<IBinder*>*/
        /* [out] */ IObjectContainer** notifications,/*List<IStatusBarNotification*>*/
        /* [out,callee] */ ArrayOf<Int32>** switches,
        /* [out] */ IObjectContainer** binders);/*List<IBinder*>*/

    /**
     * The status bar service should call this each time the user brings the panel from
     * invisible to visible in order to clear the notification light.
     */
    CARAPI OnPanelRevealed();

    CARAPI OnNotificationClick(
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id);

    CARAPI OnNotificationError(
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 initialPid,
        /* [in] */ const String& message);

    CARAPI OnNotificationClear(
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id);

    CARAPI OnClearAllNotifications();

    // ================================================================================
    // Callbacks for NotificationManagerService.
    // ================================================================================
    CARAPI AddNotification(
        /* [in] */ IStatusBarNotification* notification,
        /* [out] */ IBinder** binder);

    CARAPI UpdateNotification(
        /* [in] */ IBinder* key,
        /* [in] */ IStatusBarNotification* notification);

    CARAPI RemoveNotification(
        /* [in] */ IBinder* key);

protected:
    // ================================================================================
    // Can be called from any thread
    // ================================================================================

    // lock on mDisableRecords
    CARAPI ManageDisableListLocked(
        /* [in] */ Int32 userId,
        /* [in] */ Int32 what,
        /* [in] */ IBinder* token,
        /* [in] */ const String& pkg);

    // lock on mDisableRecords
    CARAPI GatherDisableActionsLocked(
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

private:
    CARAPI DisableInternal(
        /* [in] */ Int32 userId,
        /* [in] */ Int32 what,
        /* [in] */ IBinder* token,
        /* [in] */ const String& pkg);

    CARAPI_(void) DisableLocked(
        /* [in] */ Int32 userId,
        /* [in] */ Int32 what,
        /* [in] */ IBinder* token,
        /* [in] */ const String& pkg);

    CARAPI_(void) UpdateUiVisibilityLocked(
        /* [in] */ Int32 vis,
        /* [in] */ Int32 mask);

    CARAPI EnforceStatusBar();

    CARAPI EnforceExpandStatusBar();

    CARAPI EnforceStatusBarService();

public:
    static const String TAG;
    static const Boolean SPEW;

    AutoPtr<IContext> mContext;
    AutoPtr<CWindowManagerService> mWindowManager;
    AutoPtr<IHandler> mHandler;
    AutoPtr<INotificationCallbacks> mNotificationCallbacks;
    AutoPtr<IIStatusBar> mBar;
    AutoPtr<IStatusBarIconList> mIcons;
    Object mIconsLock;

    HashMap<AutoPtr<IBinder>, AutoPtr<IStatusBarNotification> > mNotifications;
    Object mNotificationsLock;

    // for disabling the status bar
    List<AutoPtr<DisableRecord> > mDisableRecords;
    AutoPtr<IBinder> mSysUiVisToken;
    Int32 mDisabled;

    // encompasses lights-out mode and other flags defined on View
    Int32 mSystemUiVisibility;
    Boolean mMenuVisible;
    Int32 mImeWindowVis;
    Int32 mImeBackDisposition;
    AutoPtr<IBinder> mImeToken;
    Int32 mCurrentUserId;

private:
    AutoPtr<MBroadcastReceiver> mBroadcastReceiver;
};

}//namespace Server
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CSTATUSBARMANAGERSERVICE_H__
