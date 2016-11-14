#ifndef __ELASTOS_DROID_SERVER_CSTATUSBARMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_CSTATUSBARMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_StatusBar_CStatusBarManagerService.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/Runnable.h>
#include "elastos/droid/content/BroadcastReceiver.h"
// #include "elastos/droid/server/wm/CWindowManagerService.h"
#include <Elastos.Droid.Internal.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::Internal::StatusBar::IIStatusBar;
using Elastos::Droid::Internal::StatusBar::IStatusBarIconList;
using Elastos::Droid::Internal::StatusBar::IIStatusBarService;
// using Elastos::Droid::Server::Wm::CWindowManagerService;
using Elastos::Droid::Server::Notification::INotificationDelegate;
using Elastos::Droid::Server::StatusBar::IStatusBarManagerInternal;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {
namespace StatusBar {

CarClass(CStatusBarManagerService)
    , public Object
    , public IIStatusBarService
    , public IBinder
{
private:

    /**
     * Private API used by NotificationManagerService.
     */
    class MyStatusBarManagerInternal
        : public Object
        , public IStatusBarManagerInternal
    {
    public:
        CAR_INTERFACE_DECL()

        MyStatusBarManagerInternal(
            /* [in] */ CStatusBarManagerService* host);

        CARAPI ToString(
            /* [out] */ String* str);

        //@Override
        CARAPI SetNotificationDelegate(
            /* [in] */ INotificationDelegate* delegate);

        //@Override
        CARAPI BuzzBeepBlinked();

        //@Override
        CARAPI NotificationLightPulse(
            /* [in] */ Int32 argb,
            /* [in] */ Int32 onMillis,
            /* [in] */ Int32 offMillis);

        //@Override
        CARAPI NotificationLightOff();

    private:
        Boolean mNotificationLightOn;
        CStatusBarManagerService* mHost;
    };

    class DisableRecord
        : public Object
        , public IProxyDeathRecipient
    {
        friend class CStatusBarManagerService;

    public:
        DisableRecord(
            /* [in] */ CStatusBarManagerService* host);

        CAR_INTERFACE_DECL()

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

    class SetImeWindowStatusRunnable
        : public Runnable
    {
    public:
        SetImeWindowStatusRunnable(
            /* [in] */ IBinder* token,
            /* [in] */ Int32 vis,
            /* [in] */ Int32 backDisposition,
            /* [in] */ Boolean showImeSwitcher,
            /* [in] */ CStatusBarManagerService* host);

        CARAPI Run();

    private:
        AutoPtr<IBinder> mToken;
        Int32 mVisible;
        Int32 mBackDisposition;
        Boolean mShowImeSwitcher;
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

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CStatusBarManagerService();

    /**
     * Construct the service, add the status bar view to the window manager
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIWindowManager* wm);

    //@Override
    CARAPI ToString(
        /* [out] */ String* result);

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
        /* [in] */ Int32 backDisposition,
        /* [in] */ Boolean showImeSwitcher);

    CARAPI SetSystemUiVisibility(
        /* [in] */ Int32 vis,
        /* [in] */ Int32 mask);

    //@Override
    CARAPI ToggleRecentApps();

    //@Override
    CARAPI PreloadRecentApps();

    //@Override
    CARAPI CancelPreloadRecentApps();

    //@Override
    CARAPI ShowRecentApps(
        /* [in] */ Boolean triggeredFromAltTab);

    //@Override
    CARAPI HideRecentApps(
        /* [in] */ Boolean triggeredFromAltTab,
        /* [in] */ Boolean triggeredFromHomeKey);

    //@Override
    CARAPI SetCurrentUser(
        /* [in] */ Int32 newUserId);

    //@Override
    CARAPI SetWindowState(
        /* [in] */ Int32 window,
        /* [in] */ Int32 state);

    // ================================================================================
    // Callbacks from the status bar service.
    // ================================================================================
    CARAPI RegisterStatusBar(
        /* [in] */ IIStatusBar* cb,
        /* [in] */ IStatusBarIconList* inIconList,
        /* [in] */ ArrayOf<Int32>* inSwitches,
        /* [in] */ IList* inBinders,
        /* [out] */ IStatusBarIconList** outIconList,
        /* [out, callee] */ ArrayOf<Int32>** outSwitches,
        /* [out] */ IList** outBinders);/*List<IBinder*>*/

    /**
     * The status bar service should call this each time the user brings the panel from
     * invisible to visible in order to clear the notification light.
     */
    CARAPI OnPanelRevealed();

    CARAPI OnPanelHidden();

    CARAPI OnNotificationClick(
        /* [in] */ const String& key);

    CARAPI OnNotificationError(
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 initialPid,
        /* [in] */ const String& message,
        /* [in] */ Int32 userId);

    CARAPI OnNotificationClear(
        /* [in] */ const String& pkg,
        /* [in] */ const String& tag,
        /* [in] */ Int32 id,
        /* [in] */ Int32 userId);

    //@Override
    CARAPI OnNotificationVisibilityChanged(
        /* [in] */ ArrayOf<String>* newlyVisibleKeys,
        /* [in] */ ArrayOf<String>* noLongerVisibleKeys);

    //@Override
    CARAPI OnNotificationExpansionChanged(
        /* [in] */ const String& key,
        /* [in] */ Boolean userAction,
        /* [in] */ Boolean expanded);

    CARAPI OnClearAllNotifications(
        /* [in] */ Int32 userId);

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
    // AutoPtr<CWindowManagerService> mWindowManager; // TODO: weak ref?
    AutoPtr<IHandler> mHandler;
    AutoPtr<INotificationDelegate> mNotificationDelegate;
    AutoPtr<IIStatusBar> mBar;
    AutoPtr<IStatusBarIconList> mIcons;
    Object mIconsLock;

    // for disabling the status bar
    List<AutoPtr<DisableRecord> > mDisableRecords;
    AutoPtr<IBinder> mSysUiVisToken;
    Int32 mDisabled;

    // encompasses lights-out mode and other flags defined on View
    Int32 mSystemUiVisibility;
    Boolean mMenuVisible;
    Int32 mImeWindowVis;
    Int32 mImeBackDisposition;
    Boolean mShowImeSwitcher;
    AutoPtr<IBinder> mImeToken;
    Int32 mCurrentUserId;

    AutoPtr<IStatusBarManagerInternal> mInternalService;
};

}//namespace StatusBar
}//namespace Server
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CSTATUSBARMANAGERSERVICE_H__
