
#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYMANAGERSERVICE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Server_Accessibility_CAccessibilityManagerService.h"
#include "accessibility/CAccessibilityInputFilter.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/content/PackageMonitor.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/HandlerBase.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Text::ISimpleStringSplitter;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IIWindow;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IIAccessibilityManagerClient;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionConnection;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionConnectionCallback;
using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::AccessibilityService::IIAccessibilityServiceClient;
using Elastos::Droid::AccessibilityService::IIAccessibilityServiceConnection;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::App::IAlertDialog;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

CarClass(CAccessibilityManagerService)
{
private:
    class MainHandler : public HandlerBase
    {
    public:
        MainHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ CAccessibilityManagerService* service)
            : HandlerBase(looper)
            , mHost(service)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        static const Int32 MSG_SEND_ACCESSIBILITY_EVENT_TO_INPUT_FILTER;
        static const Int32 MSG_SEND_STATE_TO_CLIENTS;
        static const Int32 MSG_SEND_CLEARED_STATE_TO_CLIENTS_FOR_USER;
        static const Int32 MSG_SEND_RECREATE_INTERNAL_STATE;
        static const Int32 MSG_UPDATE_ACTIVE_WINDOW;
        static const Int32 MSG_ANNOUNCE_NEW_USER_IF_NEEDED;

    private:
        CAccessibilityManagerService* mHost;
    };

    class AccessibilityConnectionWrapper
        : public ElRefBase
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL()

        AccessibilityConnectionWrapper(
            /* [in] */ Int32 windowId,
            /* [in] */ IAccessibilityInteractionConnection* connection,
            /* [in] */ Int32 userId,
            /* [in] */ CAccessibilityManagerService* host);

        CARAPI LinkToDeath();

        CARAPI UnlinkToDeath();

        CARAPI ProxyDied();

    private:
        friend class CAccessibilityServiceConnection;
        Int32 mWindowId;
        Int32 mUserId;
        AutoPtr<IAccessibilityInteractionConnection> mConnection;
        AutoPtr<CAccessibilityManagerService> mHost;
    };

    class UserState : public ElRefBase
    {
    public:
        UserState(
            /* [in] */ Int32 userId);

    public:
        Int32 mUserId;

        List<AutoPtr<IIAccessibilityServiceConnection> > mServices;

        AutoPtr<IRemoteCallbackList> mClients;//= new RemoteCallbackList<IIAccessibilityManagerClient>();

        HashMap<AutoPtr<IComponentName>, AutoPtr<IIAccessibilityServiceConnection> >
                mComponentNameToServiceMap;

        List<AutoPtr<IAccessibilityServiceInfo> > mInstalledServices;

        HashSet<AutoPtr<IComponentName> > mEnabledServices;

        HashSet<AutoPtr<IComponentName> > mTouchExplorationGrantedServices;

        HashMap<Int32, AutoPtr<AccessibilityConnectionWrapper> > mInteractionConnections;

        HashMap<Int32, AutoPtr<IBinder> > mWindowTokens;

        Int32 mHandledFeedbackTypes;

        Boolean mIsAccessibilityEnabled;
        Boolean mIsTouchExplorationEnabled;
        Boolean mIsDisplayMagnificationEnabled;
    };

    class TempUserStateChangeMemento : public ElRefBase
    {
    public:
        TempUserStateChangeMemento();

        CARAPI_(void) Initialize(
            /* [in] */ Int32 userId,
            /* [in] */ UserState* userState);

        CARAPI_(void) ApplyTo(
            /* [in] */ UserState* userState);

        CARAPI_(void) Clear();

    public:
        Int32 mUserId;
        Boolean mIsAccessibilityEnabled;
        Boolean mIsTouchExplorationEnabled;
        Boolean mIsDisplayMagnificationEnabled;
        HashSet<AutoPtr<IComponentName> > mEnabledServices;
        HashSet<AutoPtr<IComponentName> > mTouchExplorationGrantedServices;
    };

    class AccessibilityContentObserver
        : public ContentObserver
    {
    public:
        AccessibilityContentObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ CAccessibilityManagerService* host);

        CARAPI_(void) Register(
            /* [in] */ IContentResolver* contentResolver);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        AutoPtr<IUri> mAccessibilityEnabledUri;

        AutoPtr<IUri> mTouchExplorationEnabledUri;

        AutoPtr<IUri> mDisplayMagnificationEnabledUri;

        AutoPtr<IUri> mEnabledAccessibilityServicesUri;

        AutoPtr<IUri> mTouchExplorationGrantedAccessibilityServicesUri;

        AutoPtr<CAccessibilityManagerService> mHost;
    };

    class UserChangedBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        UserChangedBroadcastReceiver(
            /* [in] */ CAccessibilityManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CAccessibilityManagerService::UserChangedBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        AutoPtr<CAccessibilityManagerService> mHost;
    };

    class MyPackageMonitor
        : public PackageMonitor
    {
    public:
        MyPackageMonitor(
            /* [in] */ CAccessibilityManagerService* host);

        CARAPI OnSomePackagesChanged();

        CARAPI OnPackageRemoved(
            /* [in] */ const String packageName,
            /* [in] */ Int32 uid);

        CARAPI OnHandleForceStop(
            /* [in] */ IIntent* intent,
            /* [in] */ ArrayOf<String>* packages,
            /* [in] */ Int32 uid,
            /* [in] */ Boolean doit,
            /* [out] */ Boolean* result);

    private:
        AutoPtr<CAccessibilityManagerService> mHost;
    };

    class OnDialogOKListener
        : public ElRefBase
        , public IDialogInterfaceOnClickListener
    {
    public:
        OnDialogOKListener(
            /* [in] */ UserState* state,
            /* [in] */ IIAccessibilityServiceConnection* service,
            /* [in] */ CAccessibilityManagerService* host);

        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        AutoPtr<UserState> mState;
        AutoPtr<IIAccessibilityServiceConnection> mService;
        AutoPtr<CAccessibilityManagerService> mHost;
    };

    class OnDialogCancelListener
        : public ElRefBase
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);
    };

public:
    class SecurityPolicy : public ElRefBase
    {
    public:
        SecurityPolicy(
            /* [in] */ CAccessibilityManagerService* host);

        CARAPI_(void) UpdateEventSourceLocked(
            /* [in] */ IAccessibilityEvent* event);

        CARAPI_(void) UpdateActiveWindow(
            /* [in] */ Int32 windowId,
            /* [in] */ Int32 eventType);

        CARAPI_(void) OnTouchInteractionStart();

        CARAPI_(void) OnTouchInteractionEnd();

        CARAPI_(Int32) GetRetrievalAllowingWindowLocked();

        CARAPI_(Boolean) CanGetAccessibilityNodeInfoLocked(
            /* [in] */ IIAccessibilityServiceConnection* service,
            /* [in] */ Int32 windowId);

        CARAPI_(Boolean) CanPerformActionLocked(
            /* [in] */ IIAccessibilityServiceConnection* service,
            /* [in] */ Int32 windowId,
            /* [in] */ Int32 action,
            /* [in] */ IBundle* arguments);

        CARAPI_(Boolean) CanRetrieveWindowContent(
            /* [in] */ IIAccessibilityServiceConnection* service);

        CARAPI EnforceCanRetrieveWindowContent(
            /* [in] */ IIAccessibilityServiceConnection* service);

        CARAPI ResolveCallingUserIdEnforcingPermissionsLocked(
            /* [in] */ Int32 userId,
            /* [out] */ Int32* result);

        CARAPI_(Boolean) IsCallerInteractingAcrossUsers(
            /* [in] */ Int32 userId);

    private:
        CARAPI_(Boolean) CanDispatchAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event);

        CARAPI_(Boolean) IsRetrievalAllowingWindow(
            /* [in] */ Int32 windowId);

        CARAPI_(Boolean) IsActionPermitted(
            /* [in] */ Int32 action);

        CARAPI EnforceCallingPermission(
            /* [in] */ const String& permission,
            /* [in] */ const String& function);

        CARAPI_(Boolean) HasPermission(
            /* [in] */ const String& permission);

        CARAPI_(Int32) GetFocusedWindowId();

        CARAPI_(Int32) GetFocusedWindowIdLocked(
            /* [in] */ IBinder* token,
            /* [in] */ const HashMap<Int32, AutoPtr<IBinder> >& windows);

    private:
        friend class CAccessibilityServiceConnection;
        friend class CAccessibilityManagerService;

        static const Int32 VALID_ACTIONS;
        static const Int32 RETRIEVAL_ALLOWING_EVENT_TYPES;

        Int32 mActiveWindowId;
        Boolean mTouchInteractionInProgress;
        AutoPtr<CAccessibilityManagerService> mHost;
    };

public:
    CAccessibilityManagerService();

    /**
     * Creates a new instance.
     *
     * @param context A {@link Context} instance.
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI AddClient(
        /* [in] */ IIAccessibilityManagerClient* client,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI SendAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Int32 userId,
        /* [out] */ Boolean* result);

    CARAPI GetInstalledAccessibilityServiceList(
        /* [in] */ Int32 userId,
        /* [out] */ IObjectContainer** services);

    CARAPI GetEnabledAccessibilityServiceList(
        /* [in] */ Int32 feedbackType,
        /* [in] */ Int32 userId,
        /* [out] */ IObjectContainer** services);

    CARAPI Interrupt(
        /* [in] */ Int32 userId);

    CARAPI AddAccessibilityInteractionConnection(
        /* [in] */ IIWindow* windowToken,
        /* [in] */ IAccessibilityInteractionConnection* connection,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI RemoveAccessibilityInteractionConnection(
        /* [in] */ IIWindow* windowToken);

    CARAPI RegisterUiTestAutomationService(
        /* [in] */ IIAccessibilityServiceClient* serviceClient,
        /* [in] */ IAccessibilityServiceInfo* accessibilityServiceInfo);

    CARAPI TemporaryEnableAccessibilityStateUntilKeyguardRemoved(
        /* [in] */ IComponentName* service,
        /* [in] */ Boolean touchExplorationEnabled);

    CARAPI UnregisterUiTestAutomationService(
        /* [in] */ IIAccessibilityServiceClient* client);

    CARAPI_(Boolean) OnGesture(
        /* [in] */ Int32 gestureId);

    /**
     * Gets the bounds of the accessibility focus in the active window.
     *
     * @param outBounds The output to which to write the focus bounds.
     * @return Whether accessibility focus was found and the bounds are populated.
     */
    // TODO: (multi-display) Make sure this works for multiple displays.
    CARAPI_(Boolean) GetAccessibilityFocusBoundsInActiveWindow(
        /* [in] */ IRect* outBounds);

    /**
     * Gets the bounds of the active window.
     *
     * @param outBounds The output to which to write the bounds.
     */
    CARAPI_(Boolean) GetActiveWindowBounds(
        /* [in] */ IRect* outBounds);

    CARAPI_(Int32) GetActiveWindowId();

    CARAPI_(void) OnTouchInteractionStart();

    CARAPI_(void) OnTouchInteractionEnd();

private:
    CARAPI_(AutoPtr<UserState>) GetCurrentUserStateLocked();

    CARAPI_(AutoPtr<UserState>) GetUserStateLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) RegisterBroadcastReceivers();

    CARAPI_(Int32) RemoveAccessibilityInteractionConnectionInternalLocked(
        /* [in] */ IBinder* windowToken,
        /* [in] */ HashMap<Int32, AutoPtr<IBinder> > windowTokens,
        /* [in] */ HashMap<Int32, AutoPtr<AccessibilityConnectionWrapper> > interactionConnections);

    CARAPI_(void) SwitchUser(
        /* [in] */ Int32 userId);

    CARAPI_(void) RemoveUser(
        /* [in] */ Int32 userId);

    CARAPI_(void) RestoreStateFromMementoIfNeeded();

    CARAPI_(AutoPtr<IIAccessibilityServiceConnection>) GetQueryBridge();

    CARAPI_(Boolean) NotifyGestureLocked(
        /* [in] */ Int32 gestureId,
        /* [in] */ Boolean isDefault);

    /**
     * Removes an AccessibilityInteractionConnection.
     *
     * @param windowId The id of the window to which the connection is targeted.
     * @param userId The id of the user owning the connection. UserHandle.USER_ALL
     *     if global.
     */
    CARAPI_(void) RemoveAccessibilityInteractionConnectionLocked(
        /* [in] */ Int32 windowId,
        /* [in] */ Int32 userId);

    CARAPI_(void) PopulateInstalledAccessibilityServiceLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) PopulateEnabledAccessibilityServicesLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) PopulateTouchExplorationGrantedAccessibilityServicesLocked(
        /* [in] */ UserState* userState);

    /**
     * Performs {@link AccessibilityService}s delayed notification. The delay is configurable
     * and denotes the period after the last event before notifying the service.
     *
     * @param event The event.
     * @param isDefault True to notify default listeners, not default services.
     */
    CARAPI_(void) NotifyAccessibilityServicesDelayedLocked(
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Boolean isDefault);

    /**
     * Adds a service for a user.
     *
     * @param service The service to add.
     * @param userId The user id.
     */
    CARAPI_(void) TryAddServiceLocked(
        /* [in] */ IIAccessibilityServiceConnection* service,
        /* [in] */ Int32 userId);

    /**
     * Removes a service.
     *
     * @param service The service.
     * @return True if the service was removed, false otherwise.
     */
    CARAPI_(Boolean) TryRemoveServiceLocked(
        /* [in] */ IIAccessibilityServiceConnection* service);

    /**
     * Determines if given event can be dispatched to a service based on the package of the
     * event source and already notified services for that event type. Specifically, a
     * service is notified if it is interested in events from the package and no other service
     * providing the same feedback type has been notified. Exception are services the
     * provide generic feedback (feedback type left as a safety net for unforeseen feedback
     * types) which are always notified.
     *
     * @param service The potential receiver.
     * @param event The event.
     * @param handledFeedbackTypes The feedback types for which services have been notified.
     * @return True if the listener should be notified, false otherwise.
     */
    CARAPI_(Boolean) CanDispathEventLocked(
        /* [in] */ IIAccessibilityServiceConnection* service,
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Int32 handledFeedbackTypes);

    /**
     * Manages services by starting enabled ones and stopping disabled ones.
     */
    CARAPI_(void) ManageServicesLocked(
        /* [in] */ UserState* userState);

    /**
     * Unbinds all bound services for a user.
     *
     * @param userState The user state.
     */
    CARAPI_(void) UnbindAllServicesLocked(
        /* [in] */ UserState* userState);

    /**
     * Populates a set with the {@link ComponentName}s stored in a colon
     * separated value setting for a given user.
     *
     * @param settingName The setting to parse.
     * @param userId The user id.
     * @param outComponentNames The output component names.
     */
    CARAPI_(void) PopulateComponentNamesFromSettingLocked(
        /* [in] */ const String& settingName,
        /* [in] */ Int32 userId,
        /* [in] */ HashSet<AutoPtr<IComponentName> >* outComponentNames);

    /**
     * Persists the component names in the specified setting in a
     * colon separated fashion.
     *
     * @param settingName The setting name.
     * @param componentNames The component names.
     */
    CARAPI_(void) PersistComponentNamesToSettingLocked(
        /* [in] */ const String& settingName,
        /* [in] */ HashSet<AutoPtr<IComponentName> >* componentNames,
        /* [in] */ Int32 userId);

    /**
     * Updates the state of each service by starting (or keeping running) enabled ones and
     * stopping the rest.
     *
     * @param userState The user state for which to do that.
     * @return The number of enabled installed services.
     */
    CARAPI_(Int32) UpdateServicesStateLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) ScheduleSendStateToClientsLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) UpdateInputFilterLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) ShowEnableTouchExplorationDialog(
        /* [in] */ IIAccessibilityServiceConnection* service);

    CARAPI_(Int32) GetClientState(
        /* [in] */ UserState* userState);

    CARAPI_(void) RecreateInternalStateLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) HandleAccessibilityEnabledSettingChangedLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) PerformServiceManagementLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) HandleTouchExplorationEnabledSettingChangedLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) HandleDisplayMagnificationEnabledSettingChangedLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) HandleTouchExplorationGrantedAccessibilityServicesChangedLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) TryEnableTouchExplorationLocked(
        /* [in] */ IIAccessibilityServiceConnection* service);

    CARAPI_(void) TryDisableTouchExplorationLocked(
        /* [in] */ IIAccessibilityServiceConnection* service);

    CARAPI_(void) HandleSendAccessibilityEventToInputFilter(
        /* [in] */ IAccessibilityEvent* event);

    CARAPI_(void) HandleSendStateToClients(
        /* [in] */ Int32 clientState,
        /* [in] */ Int32 userId);

    CARAPI_(void) HandleSendClearedStateToClientsForUser(
        /* [in] */ Int32 userId);

    CARAPI_(void) HandleSendRecreateInternalState(
        /* [in] */ Int32 userId);

    CARAPI_(void) HandleUpdateActiveWindow(
        /* [in] */ Int32 windowId,
        /* [in] */ Int32 eventType);

    CARAPI_(void) HandleAnnounceNewUserIfNeeded();

    CARAPI_(void) AnnounceNewUserIfNeeded();

    CARAPI_(void) SendStateToClientsForUser(
        /* [in] */ Int32 clientState,
        /* [in] */ Int32 userId);

    CARAPI_(void) SendStateToClients(
        /* [in] */ Int32 clientState,
        /* [in] */ IRemoteCallbackList* clients);

private:
    static const String TAG;
    static const Boolean DEBUG;

    // TODO: This is arbitrary. When there is time implement this by watching
    //       when that accessibility services are bound.
    static const Int32 WAIT_FOR_USER_STATE_FULLY_INITIALIZED_MILLIS;

    static const String FUNCTION_REGISTER_UI_TEST_AUTOMATION_SERVICE;

    static const String TEMPORARY_ENABLE_ACCESSIBILITY_UNTIL_KEYGUARD_REMOVED;

    static const Char32 COMPONENT_NAME_SEPARATOR;

    static const Int32 OWN_PROCESS_ID;

    static Int32 sIdCounter;

    static Int32 sNextWindowId;

    AutoPtr<IContext> mContext;

    Object mLock;

    AutoPtr<ISimpleStringSplitter> mStringColonSplitter;

    List<AutoPtr<IAccessibilityServiceInfo> > mEnabledServicesForFeedbackTempList;

    AutoPtr<IRect> mTempRect;

    AutoPtr<IPoint> mTempPoint;

    AutoPtr<IDisplay> mDefaultDisplay;

    AutoPtr<IPackageManager> mPackageManager;

    AutoPtr<IIWindowManager> mWindowManagerService;

    AutoPtr<SecurityPolicy> mSecurityPolicy;

    AutoPtr<IHandler> mMainHandler;

    AutoPtr<IIAccessibilityServiceConnection> mUiAutomationService;

    AutoPtr<IIAccessibilityServiceConnection> mQueryBridge;

    AutoPtr<IAlertDialog> mEnableTouchExplorationDialog;

    AutoPtr<CAccessibilityInputFilter> mInputFilter;

    Boolean mHasInputFilter;

    AutoPtr<IRemoteCallbackList> mGlobalClients;

    HashMap<Int32, AutoPtr<AccessibilityConnectionWrapper> > mGlobalInteractionConnections;

    HashMap<Int32, AutoPtr<IBinder> > mGlobalWindowTokens;

    HashMap<Int32, AutoPtr<UserState> > mUserStates;

    AutoPtr<TempUserStateChangeMemento> mTempStateChangeForCurrentUserMemento;

    Int32 mCurrentUserId;

    friend class SecurityPolicy;
    friend class AccessibilityContentObserver;
    friend class CAccessibilityServiceConnection;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_CACCESSIBILITYMANAGERSERVICE_H__
