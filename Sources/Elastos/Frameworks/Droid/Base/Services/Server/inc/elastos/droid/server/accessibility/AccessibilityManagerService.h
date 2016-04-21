
#ifndef __ELASTOS_DROID_SERVER_ACCESSIBILITY_ACCESSIBILITYMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_ACCESSIBILITY_ACCESSIBILITYMANAGERSERVICE_H__

#include <Elastos.Droid.AccessibilityService.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Text.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/server/accessibility/AccessibilityInputFilter.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/internal/content/PackageMonitor.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/utility/Pools.h"

using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::AccessibilityService::IIAccessibilityServiceClient;
using Elastos::Droid::AccessibilityService::IIAccessibilityServiceConnection;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Graphics::IRegion;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Internal::Content::PackageMonitor;
using Elastos::Droid::Internal::Widget::ILockPatternUtils;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Text::ISimpleStringSplitter;
using Elastos::Droid::Utility::Pools;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityWindowInfo;
using Elastos::Droid::View::Accessibility::IIAccessibilityManager;
using Elastos::Droid::View::Accessibility::IIAccessibilityManagerClient;
using Elastos::Droid::View::Accessibility::IIAccessibilityInteractionConnection;
using Elastos::Droid::View::Accessibility::IIAccessibilityInteractionConnectionCallback;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionClient;
using Elastos::Droid::View::IWindowsForAccessibilityCallback;
using Elastos::Droid::View::IInputEventConsistencyVerifier;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::IIWindow;
using Elastos::Droid::View::IWindowInfo;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMagnificationSpec;
using Elastos::Droid::View::IWindowManagerInternal;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::ICopyOnWriteArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

class PendingEvent
    : public Object
{
public:
    PendingEvent();

    ~PendingEvent();

    CARAPI_(void) Clear();

public:
    AutoPtr<PendingEvent> mNext;

    AutoPtr<IKeyEvent> mEvent;
    Int32 mPolicyFlags;
    Int32 mSequence;
    Boolean mHandled;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Server::Accessibility::PendingEvent, IInterface);

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

/**
 * This class is instantiated by the system as a system level service and can be
 * accessed only by the system. The task of this service is to be a centralized
 * event dispatch for {@link AccessibilityEvent}s generated across all processes
 * on the device. Events are dispatched to {@link AccessibilityService}s.
 */

class AccessibilityManagerService
    : public Object
    , public IIAccessibilityManager
    , public IBinder
{
    class InvocationHandler;
    class KeyEventDispatcher;
    class EventDispatchHandler;

public:
    /**
     * This class represents an accessibility service. It stores all per service
     * data required for the service management, provides API for starting/stopping the
     * service and is responsible for adding/removing the service in the data structures
     * for service management. The class also exposes configuration interface that is
     * passed to the service it represents as soon it is bound. It also serves as the
     * connection for the service.
     */
    class Service
        : public Object
        , public IIAccessibilityServiceConnection
        , public IBinder
        , public IServiceConnection
        , public IProxyDeathRecipient
    {
        friend class EventDispatchHandler;
        friend class InvocationHandler;
    public:
        CAR_INTERFACE_DECL();

        Service();

        ~Service();

        CARAPI constructor(
            /* [in] */ Int32 userId,
            /* [in] */ IComponentName* componentName,
            /* [in] */ IAccessibilityServiceInfo* accessibilityServiceInfo,
            /* [in] */ IIAccessibilityManager* host);

        CARAPI_(void) SetDynamicallyConfigurableProperties(
            /* [in] */ IAccessibilityServiceInfo* info);

        /**
         * Binds to the accessibility service.
         *
         * @return True if binding is successful.
         */
        CARAPI_(Boolean) BindLocked();

        /**
         * Unbinds form the accessibility service and removes it from the data
         * structures for service management.
         *
         * @return True if unbinding is successful.
         */
        CARAPI_(Boolean) UnbindLocked();

        CARAPI_(Boolean) CanReceiveEventsLocked();

        // @Override
        CARAPI SetOnKeyEventResult(
            /* [in] */ Boolean handled,
            /* [in] */ Int32 sequence);

        CARAPI GetServiceInfo(
            /* [out] */ IAccessibilityServiceInfo** serviceInfo);

        CARAPI_(Boolean) CanRetrieveInteractiveWindowsLocked();

        CARAPI SetServiceInfo(
            /* [in] */ IAccessibilityServiceInfo* info);

        // @Override
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* componentName,
            /* [in] */ IBinder* service);

        // @Override
        CARAPI GetWindows(
            /* [out] */ IList** windows);

        // @Override
        CARAPI GetWindow(
            /* [in] */ Int32 windowId,
            /* [out] */ IAccessibilityWindowInfo** window);

        // @Override
        CARAPI FindAccessibilityNodeInfosByViewId(
            /* [in] */ Int32 accessibilityWindowId,
            /* [in] */ Int64 accessibilityNodeId,
            /* [in] */ const String& viewIdResName,
            /* [in] */ Int32 interactionId,
            /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
            /* [in] */ Int64 threadId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI FindAccessibilityNodeInfosByText(
            /* [in] */ Int32 accessibilityWindowId,
            /* [in] */ Int64 accessibilityNodeId,
            /* [in] */ const String& text,
            /* [in] */ Int32 interactionId,
            /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
            /* [in] */ Int64 threadId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI FindAccessibilityNodeInfoByAccessibilityId(
            /* [in] */ Int32 accessibilityWindowId,
            /* [in] */ Int64 accessibilityNodeId,
            /* [in] */ Int32 interactionId,
            /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
            /* [in] */ Int32 flags,
            /* [in] */ Int64 threadId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI FindFocus(
            /* [in] */ Int32 accessibilityWindowId,
            /* [in] */ Int64 accessibilityNodeId,
            /* [in] */ Int32 focusType,
            /* [in] */ Int32 interactionId,
            /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
            /* [in] */ Int64 threadId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI FocusSearch(
            /* [in] */ Int32 accessibilityWindowId,
            /* [in] */ Int64 accessibilityNodeId,
            /* [in] */ Int32 direction,
            /* [in] */ Int32 interactionId,
            /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
            /* [in] */ Int64 threadId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI PerformAccessibilityAction(
            /* [in] */ Int32 accessibilityWindowId,
            /* [in] */ Int64 accessibilityNodeId,
            /* [in] */ Int32 action,
            /* [in] */ IBundle* arguments,
            /* [in] */ Int32 interactionId,
            /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
            /* [in] */ Int64 threadId,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI PerformGlobalAction(
            /* [in] */ Int32 action,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ComputeClickPointInScreen(
            /* [in] */ Int32 accessibilityWindowId,
            /* [in] */ Int64 accessibilityNodeId,
            /* [in] */ Int32 interactionId,
            /* [in] */ IIAccessibilityInteractionConnectionCallback* cb,
            /* [in] */ Int64 interrogatingTid,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI Dump(
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ IPrintWriter* pw,
            /* [in] */ ArrayOf<String>* args);

        // @Override
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);

        CARAPI LinkToOwnDeathLocked();

        CARAPI UnlinkToOwnDeathLocked();

        CARAPI ResetLocked();

        CARAPI_(Boolean) IsConnectedLocked();

        CARAPI ProxyDied();

        /**
         * Performs a notification for an {@link AccessibilityEvent}.
         *
         * @param event The event.
         */
        CARAPI_(void) NotifyAccessibilityEvent(
            /* [in] */ IAccessibilityEvent* event);

        CARAPI_(void) NotifyGesture(
            /* [in] */ Int32 gestureId);

        CARAPI_(void) NotifyKeyEvent(
            /* [in] */ IKeyEvent* event,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) NotifyClearAccessibilityNodeInfoCache();

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "AccessibilityManagerService::Service:: ";
            info->AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        /**
         * Notifies an accessibility service client for a scheduled event given the event type.
         *
         * @param eventType The type of the event to dispatch.
         */
        CARAPI_(void) NotifyAccessibilityEventInternal(
            /* [in] */ Int32 eventType);

        CARAPI_(void) NotifyGestureInternal(
            /* [in] */ Int32 gestureId);

        CARAPI_(void) NotifyKeyEventInternal(
            /* [in] */ IKeyEvent* event,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) NotifyClearAccessibilityCacheInternal();

        CARAPI_(void) SendDownAndUpKeyEvents(
            /* [in] */ Int32 keyCode);

        CARAPI_(void) ExpandNotifications();

        CARAPI_(void) ExpandQuickSettings();

        CARAPI_(void) OpenRecents();

        CARAPI_(void) ShowGlobalActions();

        CARAPI_(AutoPtr<IIAccessibilityInteractionConnection>) GetConnectionLocked(
            /* [in] */ Int32 windowId);

        CARAPI_(Int32) ResolveAccessibilityWindowIdLocked(
            /* [in] */ Int32 accessibilityWindowId);

        CARAPI_(Int32) ResolveAccessibilityWindowIdForFindFocusLocked(
            /* [in] */ Int32 windowId,
            /* [in] */ Int32 focusType);

    public:
        Int32 mUserId;

        Int32 mId;

        AutoPtr<IAccessibilityServiceInfo> mAccessibilityServiceInfo;

        AutoPtr<IBinder> mService;

        AutoPtr<IIAccessibilityServiceClient> mServiceInterface;

        Int32 mEventTypes;

        Int32 mFeedbackType;

        /*Set<String> mPackageNames = new HashSet<>();*/
        AutoPtr<ISet> mPackageNames;

        Boolean mIsDefault;

        Boolean mRequestTouchExplorationMode;

        Boolean mRequestEnhancedWebAccessibility;

        Boolean mRequestFilterKeyEvents;

        Boolean mRetrieveInteractiveWindows;

        Int32 mFetchFlags;

        Int64 mNotificationTimeout;

        AutoPtr<IComponentName> mComponentName;

        AutoPtr<IIntent> mIntent;

        Boolean mIsAutomation;

        AutoPtr<IResolveInfo> mResolveInfo;

        // the events pending events to be dispatched to this service
        /* final SparseArray<AccessibilityEvent> mPendingEvents =
            new SparseArray<>(); */
        AutoPtr<ISparseArray> mPendingEvents;

        AutoPtr<KeyEventDispatcher> mKeyEventDispatcher;

        Boolean mWasConnectedAndDied;

        // Handler only for dispatching accessibility events since we use event
        // types as message types allowing us to remove messages per event type.
        AutoPtr<IHandler> mEventDispatchHandler;

        // Handler for scheduling method invocations on the main thread.
        AutoPtr<InvocationHandler> mInvocationHandler;

    private:
        // static const Boolean DEBUG;

        // // We pick the MSB to avoid collision since accessibility event types are
        // // used as message types allowing us to remove messages per event type.
        // static const Int32 MSG_ON_GESTURE;

        AccessibilityManagerService* mHost;
    };

    class WindowsForAccessibilityCallback
        : public Object
        , public IWindowsForAccessibilityCallback
    {
    public:
        CAR_INTERFACE_DECL();

        WindowsForAccessibilityCallback(
            /* [in] */ AccessibilityManagerService* host);

        ~WindowsForAccessibilityCallback();

        // @Override
        CARAPI OnWindowsForAccessibilityChanged(
            /* [in] */ IList* windows);

    private:
        CARAPI_(AutoPtr<IAccessibilityWindowInfo>) PopulateReportedWindow(
            /* [in] */ IWindowInfo* window);

        CARAPI_(Int32) GetTypeForWindowManagerWindowType(
            /* [in] */ Int32 windowType);

    private:
        AccessibilityManagerService* mHost;
    };

    class SecurityPolicy
        : public Object
    {
        friend class AccessibilityManagerService;
    public:
        SecurityPolicy(
            /* [in] */ AccessibilityManagerService* host);

        ~SecurityPolicy();

        CARAPI_(void) ClearWindowsLocked();

        CARAPI_(void) UpdateWindowsLocked(
            /* [in] */ IList* windows);

        CARAPI_(Boolean) ComputePartialInteractiveRegionForWindowLocked(
            /* [in] */ Int32 windowId,
            /* [in] */ IRegion* outRegion);

        CARAPI_(void) UpdateEventSourceLocked(
            /* [in] */ IAccessibilityEvent* event);

        CARAPI_(void) UpdateActiveAndAccessibilityFocusedWindowLocked(
            /* [in] */ Int32 windowId,
            /* [in] */ Int64 nodeId,
            /* [in] */ Int32 eventType);

        CARAPI_(void) OnTouchInteractionStart();

        CARAPI_(void) OnTouchInteractionEnd();

        CARAPI_(Int32) GetActiveWindowId();

        CARAPI_(Boolean) CanGetAccessibilityNodeInfoLocked(
            /* [in] */ Service* service,
            /* [in] */ Int32 windowId);

        CARAPI_(Boolean) CanRetrieveWindowsLocked(
            /* [in] */ Service* service);

        CARAPI_(Boolean) CanRetrieveWindowContentLocked(
            /* [in] */ Service* service);

        CARAPI ResolveCallingUserIdEnforcingPermissionsLocked(
            /* [in] */ Int32 userId,
            /* [out] */ Int32* result);

        CARAPI_(Boolean) IsCallerInteractingAcrossUsers(
            /* [in] */ Int32 userId);

    private:
        CARAPI_(Boolean) CanDispatchAccessibilityEventLocked(
            /* [in] */ IAccessibilityEvent* event);

        CARAPI_(void) SetActiveWindowLocked(
            /* [in] */ Int32 windowId);

        CARAPI_(void) SetAccessibilityFocusedWindowLocked(
            /* [in] */ Int32 windowId);

        CARAPI_(void) NotifyWindowsChanged();

        CARAPI_(Int32) ResolveProfileParentLocked(
            /* [in] */ Int32 userId);

        CARAPI_(Boolean) IsRetrievalAllowingWindow(
            /* [in] */ Int32 windowId);

        CARAPI_(AutoPtr<IAccessibilityWindowInfo>) FindWindowById(
            /* [in] */ Int32 windowId);

        CARAPI EnforceCallingPermission(
            /* [in] */ const String& permission,
            /* [in] */ const String& function);

        CARAPI_(Boolean) HasPermission(
            /* [in] */ const String& permission);

        CARAPI_(Int32) GetFocusedWindowId();

    public:
        static const Int32 INVALID_WINDOW_ID;
        static const Int32 RETRIEVAL_ALLOWING_EVENT_TYPES;

        /*List<AccessibilityWindowInfo> mWindows;*/
        AutoPtr<IList> mWindows;

        Int32 mActiveWindowId; // = INVALID_WINDOW_ID;
        Int32 mFocusedWindowId;// = INVALID_WINDOW_ID;
        Int32 mAccessibilityFocusedWindowId ;//= INVALID_WINDOW_ID;
        Int64 mAccessibilityFocusNodeId;// = IAccessibilityNodeInfo::UNDEFINED_ITEM_ID;

        AutoPtr<IAccessibilityEvent> mShowingFocusedWindowEvent;

        Boolean mTouchInteractionInProgress;

        AccessibilityManagerService* mHost;
    };

private:
    /* an inside class in class Service */
    class InvocationHandler
        : public Handler
    {
        friend class KeyEventDispatcher;
    public:
        InvocationHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ Service* host);

        ~InvocationHandler();

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* messages);

    public:
        static const Int32 MSG_ON_GESTURE = 1;
        static const Int32 MSG_ON_KEY_EVENT = 2;
        static const Int32 MSG_CLEAR_ACCESSIBILITY_CACHE = 3;
        static const Int32 MSG_ON_KEY_EVENT_TIMEOUT = 4;

    private:
        Service* mHost;
    };

    /* an inside class in class Service */
    class KeyEventDispatcher
        : public Object
    {
    public:
        KeyEventDispatcher(
            /* [in] */ Service* owner,
            /* [in] */ AccessibilityManagerService* host);

        ~KeyEventDispatcher();

        CARAPI NotifyKeyEvent(
            /* [in] */ IKeyEvent* event,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) SetOnKeyEventResult(
            /* [in] */ Boolean handled,
            /* [in] */ Int32 sequence);

        CARAPI_(void) Flush();

    private:
        CARAPI_(AutoPtr<PendingEvent>) AddPendingEventLocked(
            /* [in] */ IKeyEvent* event,
            /* [in] */ Int32 policyFlags);

        CARAPI_(AutoPtr<PendingEvent>) RemovePendingEventLocked(
            /* [in] */ Int32 sequence);

        CARAPI_(void) FinishPendingEventLocked(
            /* [in] */ PendingEvent* pendingEvent);

        CARAPI_(void) SendKeyEventToInputFilter(
            /* [in] */ IKeyEvent* event,
            /* [in] */ Int32 policyFlags);

        CARAPI_(void) CancelAllPendingEventsLocked();

    private:
        static const Int64 ON_KEY_EVENT_TIMEOUT_MILLIS;

        AutoPtr<PendingEvent> mPendingEvents;

        AutoPtr<IInputEventConsistencyVerifier> mSentEventsVerifier;

        Service* mOwner;
        AccessibilityManagerService* mHost;
    };

    /* used to define mHandler in class Service */
    class EventDispatchHandler
        : public Handler
    {
    public:
        EventDispatchHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ Service* host);

        ~EventDispatchHandler();

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        Service* mHost;
    };

    /* used to function BindLocked() in class Service*/
    class BindLockedRunnable
        : public Runnable
    {
    public:
        BindLockedRunnable(
            /* [in] */ Service* host);

        ~BindLockedRunnable();

        // @Override
        CARAPI Run();

    private:
        Service* mHost;
    };

    class AccessibilityConnectionWrapper
        : public Object
        , public IProxyDeathRecipient
    {
        friend class Service;
    public:
        CAR_INTERFACE_DECL();

        AccessibilityConnectionWrapper(
            /* [in] */ Int32 windowId,
            /* [in] */ IIAccessibilityInteractionConnection* connection,
            /* [in] */ Int32 userId,
            /* [in] */ AccessibilityManagerService* host);

        ~AccessibilityConnectionWrapper();

        CARAPI LinkToDeath();

        CARAPI UnlinkToDeath();

        CARAPI ProxyDied();

    private:
        friend class CAccessibilityServiceConnection;
        Int32 mWindowId;
        Int32 mUserId;
        AutoPtr<IIAccessibilityInteractionConnection> mConnection;
        AutoPtr<AccessibilityManagerService> mHost;
    };

    class MainHandler
        : public Handler
    {
    public:
        MainHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ AccessibilityManagerService* service);

        ~MainHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI_(void) AnnounceNewUserIfNeeded();

        CARAPI_(void) SendStateToClientsForUser(
            /* [in] */ Int32 clientState,
            /* [in] */ Int32 userId);

        CARAPI_(void) SendStateToClients(
            /* [in] */ Int32 clientState,
            /* [in] */ IRemoteCallbackList* clients);

    public:
        static const Int32 MSG_SEND_ACCESSIBILITY_EVENT_TO_INPUT_FILTER = 1;
        static const Int32 MSG_SEND_STATE_TO_CLIENTS = 2;
        static const Int32 MSG_SEND_CLEARED_STATE_TO_CLIENTS_FOR_USER = 3;
        static const Int32 MSG_ANNOUNCE_NEW_USER_IF_NEEDED = 5;
        static const Int32 MSG_UPDATE_INPUT_FILTER = 6;
        static const Int32 MSG_SHOW_ENABLED_TOUCH_EXPLORATION_DIALOG = 7;
        static const Int32 MSG_SEND_KEY_EVENT_TO_INPUT_FILTER = 8;
        static const Int32 MSG_CLEAR_ACCESSIBILITY_FOCUS = 9;

    private:
        AccessibilityManagerService* mHost;
    };


    class InteractionBridge
        : public Object
    {
    public:
        InteractionBridge(
            /* [in] */ AccessibilityManagerService* host);

        ~InteractionBridge();

        CARAPI_(void) ClearAccessibilityFocusNotLocked(
            /* [in] */ Int32 windowId);

        CARAPI_(Boolean) GetAccessibilityFocusClickPointInScreenNotLocked(
            /* [in] */ IPoint* outPoint);

    private:
        CARAPI_(AutoPtr<IAccessibilityNodeInfo>) GetAccessibilityFocusNotLocked();

        CARAPI_(AutoPtr<IAccessibilityNodeInfo>) GetAccessibilityFocusNotLocked(
            /* [in] */ Int32 windowId);

    private:
        AutoPtr<IDisplay> mDefaultDisplay;
        Int32 mConnectionId;
        AutoPtr<IAccessibilityInteractionClient> mClient;

        AccessibilityManagerService* mHost;
    };

    class UserState
        : public Object
    {
        friend class AccessibilityManagerService;
        friend class UserStateDeathRecipient;
    public:
        UserState(
            /* [in] */ Int32 userId,
            /* [in] */ AccessibilityManagerService* host);

        ~UserState();

        CARAPI_(Int32) GetClientState();

        CARAPI_(void) OnSwitchToAnotherUser();

        CARAPI_(void) DestroyUiAutomationService();

    public:
        Int32 mUserId;

        // Non-transient state.

        AutoPtr<IRemoteCallbackList> mClients;//= new RemoteCallbackList<IIAccessibilityManagerClient>();

        /*public final SparseArray<AccessibilityConnectionWrapper> mInteractionConnections =
                new SparseArray<>();*/
        AutoPtr<ISparseArray> mInteractionConnections;

        /*public final SparseArray<IBinder> mWindowTokens = new SparseArray<>();*/
        AutoPtr<ISparseArray> mWindowTokens;

        // Transient state.

        /*public final CopyOnWriteArrayList<Service> mBoundServices =
                new CopyOnWriteArrayList<>();*/
        AutoPtr<ICopyOnWriteArrayList> mBoundServices;

        /*public final Map<ComponentName, Service> mComponentNameToServiceMap =
                new HashMap<>();*/
        AutoPtr<IMap> mComponentNameToServiceMap;

        /*public final List<AccessibilityServiceInfo> mInstalledServices =
                new ArrayList<>();*/
        AutoPtr<IList> mInstalledServices;

        /*public final Set<ComponentName> mBindingServices = new HashSet<>();*/
        AutoPtr<ISet> mBindingServices;

        /*public final Set<ComponentName> mEnabledServices = new HashSet<>();*/
        AutoPtr<ISet> mEnabledServices;

        /*public final Set<ComponentName> mTouchExplorationGrantedServices =
                new HashSet<>();*/
        AutoPtr<ISet> mTouchExplorationGrantedServices;

        Int32 mHandledFeedbackTypes;

        Int32 mLastSentClientState;

        Boolean mIsAccessibilityEnabled;
        Boolean mIsTouchExplorationEnabled;
        Boolean mIsTextHighContrastEnabled;
        Boolean mIsEnhancedWebAccessibilityEnabled;
        Boolean mIsDisplayMagnificationEnabled;
        Boolean mIsFilterKeyEventsEnabled;
        Boolean mHasDisplayColorAdjustment;
        Boolean mAccessibilityFocusOnlyInActiveWindow;

    private:
        AutoPtr<Service> mUiAutomationService;
        AutoPtr<IIAccessibilityServiceClient> mUiAutomationServiceClient;

        AutoPtr<IBinder> mUiAutomationServiceOwner;
        AutoPtr<IProxyDeathRecipient> mUiAutomationSerivceOnwerDeathRecipient;

        AccessibilityManagerService* mHost;
    };

    /* used to define mUiAutomationSerivceOnwerDeathRecipient in class UserState */
    class UserStateDeathRecipient
        : public Object
        , public IProxyDeathRecipient
    {
    public:
        CAR_INTERFACE_DECL();

        UserStateDeathRecipient(
            /* [in] */ UserState* host);

        ~UserStateDeathRecipient();

        // @Override
        CARAPI ProxyDied();

    private:
        UserState* mHost;
    };

    class AccessibilityContentObserver
        : public ContentObserver
    {
    public:
        AccessibilityContentObserver(
            /* [in] */ AccessibilityManagerService* host);

        ~AccessibilityContentObserver();

        CARAPI constructor(
            /* [in] */ IHandler* handler);

        CARAPI_(void) Register(
            /* [in] */ IContentResolver* contentResolver);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        AutoPtr<IUri> mAccessibilityEnabledUri;

        AutoPtr<IUri> mTouchExplorationEnabledUri;

        AutoPtr<IUri> mDisplayMagnificationEnabledUri;

        AutoPtr<IUri> mEnabledAccessibilityServicesUri;

        AutoPtr<IUri> mTouchExplorationGrantedAccessibilityServicesUri;

        AutoPtr<IUri> mEnhancedWebAccessibilityUri;

        AutoPtr<IUri> mDisplayInversionEnabledUri;

        AutoPtr<IUri> mDisplayDaltonizerEnabledUri;

        AutoPtr<IUri> mDisplayDaltonizerUri;

        AutoPtr<IUri> mHighTextContrastUri;

        AccessibilityManagerService* mHost;
    };

    /* used in function registerBroadcastReceivers() */
    class MyPackageMonitor
        : public PackageMonitor
    {
    public:
        MyPackageMonitor(
            /* [in] */ AccessibilityManagerService* host);

        ~MyPackageMonitor();

        // @Override
        CARAPI OnSomePackagesChanged();

        // @Override
        CARAPI OnPackageRemoved(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        // @Override
        CARAPI OnHandleForceStop(
            /* [in] */ IIntent* intent,
            /* [in] */ ArrayOf<String>* packages,
            /* [in] */ Int32 uid,
            /* [in] */ Boolean doit,
            /* [out] */ Boolean* result);

    private:
        AccessibilityManagerService* mHost;
    };

    /* used in function registerBroadcastReceivers() */
    class UserChangedBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        UserChangedBroadcastReceiver(
            /* [in] */ AccessibilityManagerService* host);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = "AccessibilityManagerService::UserChangedBroadcastReceiver:: ";
            info->AppendFormat("%p", this);
            return NOERROR;
        }

    private:
        AutoPtr<AccessibilityManagerService> mHost;
    };

    /*used in function ShowEnableTouchExplorationDialog()*/
    class OnDialogOKListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnDialogOKListener(
            /* [in] */ UserState* state,
            /* [in] */ Service* service,
            /* [in] */ AccessibilityManagerService* host);

        ~OnDialogOKListener();

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        AutoPtr<UserState> mState;
        AutoPtr<Service> mService;
        AccessibilityManagerService* mHost;
    };

    /*used in function ShowEnableTouchExplorationDialog()*/
    class OnDialogCancelListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnDialogCancelListener();

        ~OnDialogCancelListener();

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);
    };

public:
    CAR_INTERFACE_DECL();

    AccessibilityManagerService();

    ~AccessibilityManagerService();

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
        /* [out] */ IList** services);

    CARAPI GetEnabledAccessibilityServiceList(
        /* [in] */ Int32 feedbackType,
        /* [in] */ Int32 userId,
        /* [out] */ IList** services);

    CARAPI Interrupt(
        /* [in] */ Int32 userId);

    CARAPI AddAccessibilityInteractionConnection(
        /* [in] */ IIWindow* windowToken,
        /* [in] */ IIAccessibilityInteractionConnection* connection,
        /* [in] */ Int32 userId,
        /* [out] */ Int32* result);

    CARAPI RemoveAccessibilityInteractionConnection(
        /* [in] */ IIWindow* windowToken);

    CARAPI RegisterUiTestAutomationService(
        /* [in] */ IBinder* owner,
        /* [in] */ IIAccessibilityServiceClient* serviceClient,
        /* [in] */ IAccessibilityServiceInfo* accessibilityServiceInfo);

    CARAPI UnregisterUiTestAutomationService(
        /* [in] */ IIAccessibilityServiceClient* client);

    CARAPI TemporaryEnableAccessibilityStateUntilKeyguardRemoved(
        /* [in] */ IComponentName* service,
        /* [in] */ Boolean touchExplorationEnabled);

    CARAPI GetWindowToken(
        /* [in] */ Int32 windowId,
        /* [out] */ IBinder** binder);

    CARAPI_(Boolean) OnGesture(
        /* [in] */ Int32 gestureId);

    CARAPI_(Boolean) NotifyKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags);

    /**
     * Gets a point within the accessibility focused node where we can send down
     * and up events to perform a click.
     *
     * @param outPoint The click point to populate.
     * @return Whether accessibility a click point was found and set.
     */
    // TODO: (multi-display) Make sure this works for multiple displays.
    CARAPI_(Boolean) GetAccessibilityFocusClickPointInScreen(
        /* [in] */ IPoint* outPoint);

    /**
     * Gets the bounds of the active window.
     *
     * @param outBounds The output to which to write the bounds.
     */
    CARAPI_(Boolean) GetActiveWindowBounds(
        /* [in] */ IRect* outBounds);

    CARAPI_(Boolean) AccessibilityFocusOnlyInActiveWindow();

    CARAPI_(Int32) GetActiveWindowId();

    CARAPI_(void) OnTouchInteractionStart();

    CARAPI_(void) OnTouchInteractionEnd();

    CARAPI_(void) OnMagnificationStateChanged();

    // @Override
    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info);
        *info = "AccessibilityManagerService:: ";
        info->AppendFormat("%p", this);
        return NOERROR;
    }

private:
    CARAPI_(AutoPtr<UserState>) GetCurrentUserStateLocked();

    CARAPI_(AutoPtr<UserState>) GetUserStateLocked(
        /* [in] */ Int32 userId);

    CARAPI_(void) RegisterBroadcastReceivers();

    CARAPI_(Int32) RemoveAccessibilityInteractionConnectionInternalLocked(
        /* [in] */ IBinder* windowToken,
        /* [in] */ ISparseArray* windowTokens/*SparseArray<IBinder>*/,
        /* [in] */ ISparseArray* interactionConnections/*SparseArray<AccessibilityConnectionWrapper>*/);

    CARAPI_(void) SwitchUser(
        /* [in] */ Int32 userId);

    CARAPI_(void) RemoveUser(
        /* [in] */ Int32 userId);

    CARAPI_(AutoPtr<InteractionBridge>) GetInteractionBridgeLocked();

    CARAPI_(Boolean) NotifyGestureLocked(
        /* [in] */ Int32 gestureId,
        /* [in] */ Boolean isDefault);

    CARAPI_(Boolean) NotifyKeyEventLocked(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags,
        /* [in] */ Boolean isDefault);

    CARAPI_(void) NotifyClearAccessibilityCacheLocked();

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

    CARAPI_(Boolean) ReadInstalledAccessibilityServiceLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) ReadEnabledAccessibilityServicesLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) ReadTouchExplorationGrantedAccessibilityServicesLocked(
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

    CARAPI_(void) AddServiceLocked(
        /* [in] */ Service* service,
        /* [in] */ UserState* userState);

    /**
     * Removes a service.
     *
     * @param service The service.
     */
    CARAPI_(void) RemoveServiceLocked(
        /* [in] */ Service* service,
        /* [in] */ UserState* userState);

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
    CARAPI_(Boolean) CanDispatchEventToServiceLocked(
        /* [in] */ Service* service,
        /* [in] */ IAccessibilityEvent* event,
        /* [in] */ Int32 handledFeedbackTypes);

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
    CARAPI_(void) ReadComponentNamesFromSettingLocked(
        /* [in] */ const String& settingName,
        /* [in] */ Int32 userId,
        /* [in] */ ISet* outComponentNames);

    /**
     * Persists the component names in the specified setting in a
     * colon separated fashion.
     *
     * @param settingName The setting name.
     * @param componentNames The component names.
     */
    CARAPI_(void) PersistComponentNamesToSettingLocked(
        /* [in] */ const String& settingName,
        /* [in] */ ISet* componentNames,
        /* [in] */ Int32 userId);

    CARAPI_(void) ManageServicesLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) ScheduleUpdateClientsIfNeededLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) ScheduleUpdateInputFilter(
        /* [in] */ UserState* userState);

    CARAPI_(void) UpdateInputFilter(
        /* [in] */ UserState* userState);

    CARAPI_(void) ShowEnableTouchExplorationDialog(
        /* [in] */ Service* service);

    CARAPI_(void) OnUserStateChangedLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) UpdateAccessibilityFocusBehaviorLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) UpdateWindowsForAccessibilityCallbackLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) UpdateLegacyCapabilitiesLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) UpdateFilterKeyEventsLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) UpdateServicesLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) ReadConfigurationForUserStateLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) ReadAccessibilityEnabledSettingLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) ReadTouchExplorationEnabledSettingLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) ReadDisplayMagnificationEnabledSettingLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) ReadEnhancedWebAccessibilityEnabledChangedLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) ReadDisplayColorAdjustmentSettingsLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) ReadHighTextContrastEnabledSettingLocked(
        /* [in] */ UserState* userState);

    CARAPI_(void) UpdateTouchExplorationLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) CanRequestAndRequestsTouchExplorationLocked(
        /* [in] */ Service* service);

    CARAPI_(void) UpdateEnhancedWebAccessibilityLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) CanRequestAndRequestsEnhancedWebAccessibilityLocked(
        /* [in] */ Service* service);

    CARAPI_(void) UpdateDisplayColorAdjustmentSettingsLocked(
        /* [in] */ UserState* userState);

    CARAPI_(Boolean) HasRunningServicesLocked(
        /* [in] */ UserState* userState);

    CARAPI_(AutoPtr<IMagnificationSpec>) GetCompatibleMagnificationSpecLocked(
        /* [in] */ Int32 windowId);

    CARAPI_(AutoPtr<PendingEvent>) ObtainPendingEventLocked(
        /* [in] */ IKeyEvent* event,
        /* [in] */ Int32 policyFlags,
        /* [in] */ Int32 sequence);

    CARAPI_(void) RecyclePendingEventLocked(
        /* [in] */ PendingEvent* pendingEvent);

    CARAPI_(Int32) FindWindowIdLocked(
        /* [in] */ IBinder* token);

    CARAPI_(void) EnsureWindowsAvailableTimed();

private:
    static const String TAG;
    static const Boolean DEBUG;

    // TODO: This is arbitrary. When there is time implement this by watching
    //       when that accessibility services are bound.
    static const Int32 WAIT_FOR_USER_STATE_FULLY_INITIALIZED_MILLIS;

    static const Int32 WAIT_WINDOWS_TIMEOUT_MILLIS;

    static const String FUNCTION_REGISTER_UI_TEST_AUTOMATION_SERVICE;

    static const String TEMPORARY_ENABLE_ACCESSIBILITY_UNTIL_KEYGUARD_REMOVED;

    static const String GET_WINDOW_TOKEN;

    static AutoPtr<IComponentName> sFakeAccessibilityServiceComponentName;//new ComponentName("foo.bar", "FakeService");

    static const String FUNCTION_DUMP;

    static const Char32 COMPONENT_NAME_SEPARATOR;

    static const Int32 OWN_PROCESS_ID;

    static const Int32 MAX_POOL_SIZE;

    static const Int32 WINDOW_ID_UNKNOWN;

    static Int32 sIdCounter;

    static Int32 sNextWindowId;

    AutoPtr<IContext> mContext;

    Object mLock;

    AutoPtr< Pools::Pool<PendingEvent> > mPendingEventPool;

    AutoPtr<ISimpleStringSplitter> mStringColonSplitter;

    /*private final List<AccessibilityServiceInfo> mEnabledServicesForFeedbackTempList =
            new ArrayList<>();*/
    AutoPtr<IList> mEnabledServicesForFeedbackTempList;

    AutoPtr<IRegion> mTempRegion;

    AutoPtr<IRect> mTempRect;

    AutoPtr<IPoint> mTempPoint;

    AutoPtr<IPackageManager> mPackageManager;

    AutoPtr<IWindowManagerInternal> mWindowManagerService;

    AutoPtr<SecurityPolicy> mSecurityPolicy;

    AutoPtr<IHandler> mMainHandler;

    AutoPtr<InteractionBridge> mInteractionBridge;

    AutoPtr<IAlertDialog> mEnableTouchExplorationDialog;

    AutoPtr<AccessibilityInputFilter> mInputFilter;

    Boolean mHasInputFilter;

    /*private final Set<ComponentName> mTempComponentNameSet = new HashSet<>();*/
    AutoPtr<ISet> mTempComponentNameSet;

    /*private final List<AccessibilityServiceInfo> mTempAccessibilityServiceInfoList =
            new ArrayList<>();*/
    AutoPtr<IList> mTempAccessibilityServiceInfoList;

    /*private final RemoteCallbackList<IAccessibilityManagerClient> mGlobalClients =
            new RemoteCallbackList<>();*/
    AutoPtr<IRemoteCallbackList> mGlobalClients;

    /*private final SparseArray<AccessibilityConnectionWrapper> mGlobalInteractionConnections =
            new SparseArray<>();*/
    AutoPtr<ISparseArray> mGlobalInteractionConnections;

    /*private final SparseArray<IBinder> mGlobalWindowTokens = new SparseArray<>();*/
    AutoPtr<ISparseArray> mGlobalWindowTokens;

    /*private final SparseArray<UserState> mUserStates = new SparseArray<>();*/
    AutoPtr<ISparseArray> mUserStates;

    AutoPtr<IUserManager> mUserManager;

    AutoPtr<ILockPatternUtils> mLockPatternUtils;

    Int32 mCurrentUserId;

    //TODO: Remove this hack
    Boolean mInitialized;

    AutoPtr<WindowsForAccessibilityCallback> mWindowsForAccessibilityCallback;
};

} // Accessibility
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_ACCESSIBILITY_ACCESSIBILITYMANAGERSERVICE_H__
