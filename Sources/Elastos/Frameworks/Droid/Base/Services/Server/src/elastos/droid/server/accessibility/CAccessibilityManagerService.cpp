
#include "accessibility/CAccessibilityManagerService.h"
#include "accessibility/CAccessibilityServiceConnection.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/utility/etl/Algorithm.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CObjectContainer;
using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::CSimpleStringSplitter;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IWindowInfo;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::EIID_IIAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionClient;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionClientHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityInteractionClientHelper;
using Elastos::Droid::AccessibilityService::CAccessibilityServiceInfo;
using Elastos::Droid::AccessibilityService::IAccessibilityService;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;


namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

static Int32 InitProcessId()
{
    return Process::MyPid();
}

const String CAccessibilityManagerService::TAG("CAccessibilityManagerService");
const Boolean CAccessibilityManagerService::DEBUG = FALSE;
const Int32 CAccessibilityManagerService::WAIT_FOR_USER_STATE_FULLY_INITIALIZED_MILLIS = 3000;
const String CAccessibilityManagerService::FUNCTION_REGISTER_UI_TEST_AUTOMATION_SERVICE("registerUiTestAutomationService");
const String CAccessibilityManagerService::TEMPORARY_ENABLE_ACCESSIBILITY_UNTIL_KEYGUARD_REMOVED("temporaryEnableAccessibilityStateUntilKeyguardRemoved");
const Char32 CAccessibilityManagerService::COMPONENT_NAME_SEPARATOR(':');
const Int32 CAccessibilityManagerService::OWN_PROCESS_ID = InitProcessId();

Int32 CAccessibilityManagerService::sIdCounter = 0;
Int32 CAccessibilityManagerService::sNextWindowId = 0;

const Int32 CAccessibilityManagerService::MainHandler::MSG_SEND_ACCESSIBILITY_EVENT_TO_INPUT_FILTER = 1;
const Int32 CAccessibilityManagerService::MainHandler::MSG_SEND_STATE_TO_CLIENTS = 2;
const Int32 CAccessibilityManagerService::MainHandler::MSG_SEND_CLEARED_STATE_TO_CLIENTS_FOR_USER = 3;
const Int32 CAccessibilityManagerService::MainHandler::MSG_SEND_RECREATE_INTERNAL_STATE = 4;
const Int32 CAccessibilityManagerService::MainHandler::MSG_UPDATE_ACTIVE_WINDOW = 5;
const Int32 CAccessibilityManagerService::MainHandler::MSG_ANNOUNCE_NEW_USER_IF_NEEDED = 6;

CAccessibilityManagerService::UserState::UserState(
    /* [in] */ Int32 userId)
    : mUserId(userId)
    , mHandledFeedbackTypes(0)
{
    CRemoteCallbackList::New((IRemoteCallbackList**)&mClients);;
}

//======================================================================================
// CAccessibilityManagerService::MainHandler
//======================================================================================
ECode CAccessibilityManagerService::MainHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);

    switch(what) {
        case MSG_SEND_ACCESSIBILITY_EVENT_TO_INPUT_FILTER: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            IAccessibilityEvent* event = IAccessibilityEvent::Probe(obj);
            mHost->HandleSendAccessibilityEventToInputFilter(event);
            break;
        }
        case MSG_SEND_STATE_TO_CLIENTS: {
            mHost->HandleSendStateToClients(arg1, arg2);
            break;
        }
        case MSG_SEND_CLEARED_STATE_TO_CLIENTS_FOR_USER: {
            mHost->HandleSendClearedStateToClientsForUser(arg1);
            break;
        }
        case MSG_SEND_RECREATE_INTERNAL_STATE: {
            mHost->HandleSendRecreateInternalState(arg1);
            break;
        }
        case MSG_UPDATE_ACTIVE_WINDOW: {
            mHost->HandleUpdateActiveWindow(arg1, arg2);
            break;
        }
        case MSG_ANNOUNCE_NEW_USER_IF_NEEDED: {
            mHost->HandleAnnounceNewUserIfNeeded();
            break;
        }
    }

    return NOERROR;
}

//======================================================================================
// CAccessibilityManagerService::AccessibilityConnectionWrapper
//======================================================================================
CAR_INTERFACE_IMPL(CAccessibilityManagerService::AccessibilityConnectionWrapper, IProxyDeathRecipient)

CAccessibilityManagerService::AccessibilityConnectionWrapper::AccessibilityConnectionWrapper(
    /* [in] */ Int32 windowId,
    /* [in] */ IAccessibilityInteractionConnection* connection,
    /* [in] */ Int32 userId,
    /* [in] */ CAccessibilityManagerService* host)
    : mWindowId(windowId)
    , mUserId(userId)
    , mConnection(connection)
    , mHost(host)
{}

ECode CAccessibilityManagerService::AccessibilityConnectionWrapper::LinkToDeath()
{
    AutoPtr<IProxy> proxy = (IProxy*)mConnection->Probe(EIID_IProxy);
    if (proxy != NULL) {
        proxy->LinkToDeath(this, 0);
    }
    else {
        Slogger::W(CAccessibilityManagerService::TAG, "connection does not implement IProxy.");
    }
    return NOERROR;
}

ECode CAccessibilityManagerService::AccessibilityConnectionWrapper::UnlinkToDeath()
{
    AutoPtr<IProxy> proxy = (IProxy*)mConnection->Probe(EIID_IProxy);
    if (proxy != NULL) {
        Boolean result;
        proxy->UnlinkToDeath(this, 0, &result);
    }
    else {
        Slogger::W(CAccessibilityManagerService::TAG, "connection does not implement IProxy.");
    }
    return NOERROR;
}

ECode CAccessibilityManagerService::AccessibilityConnectionWrapper::ProxyDied()
{
    UnlinkToDeath();
    AutoLock lock(mHost->mLock);
    mHost->RemoveAccessibilityInteractionConnectionLocked(mWindowId, mUserId);

    return NOERROR;
}

//======================================================================================
// CAccessibilityManagerService::SecurityPolicy
//======================================================================================
const Int32 CAccessibilityManagerService::SecurityPolicy::VALID_ACTIONS =
        IAccessibilityNodeInfo::ACTION_CLICK
        | IAccessibilityNodeInfo::ACTION_LONG_CLICK
        | IAccessibilityNodeInfo::ACTION_FOCUS
        | IAccessibilityNodeInfo::ACTION_CLEAR_FOCUS
        | IAccessibilityNodeInfo::ACTION_SELECT
        | IAccessibilityNodeInfo::ACTION_CLEAR_SELECTION
        | IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS
        | IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS
        | IAccessibilityNodeInfo::ACTION_NEXT_AT_MOVEMENT_GRANULARITY
        | IAccessibilityNodeInfo::ACTION_PREVIOUS_AT_MOVEMENT_GRANULARITY
        | IAccessibilityNodeInfo::ACTION_NEXT_HTML_ELEMENT
        | IAccessibilityNodeInfo::ACTION_PREVIOUS_HTML_ELEMENT
        | IAccessibilityNodeInfo::ACTION_SCROLL_FORWARD
        | IAccessibilityNodeInfo::ACTION_SCROLL_BACKWARD;

const Int32 CAccessibilityManagerService::SecurityPolicy::RETRIEVAL_ALLOWING_EVENT_TYPES =
        IAccessibilityEvent::TYPE_VIEW_CLICKED
        | IAccessibilityEvent::TYPE_VIEW_FOCUSED
        | IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER
        | IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT
        | IAccessibilityEvent::TYPE_VIEW_LONG_CLICKED
        | IAccessibilityEvent::TYPE_VIEW_TEXT_CHANGED
        | IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED
        | IAccessibilityEvent::TYPE_VIEW_SELECTED
        | IAccessibilityEvent::TYPE_WINDOW_CONTENT_CHANGED
        | IAccessibilityEvent::TYPE_VIEW_TEXT_SELECTION_CHANGED
        | IAccessibilityEvent::TYPE_VIEW_SCROLLED
        | IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED
        | IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED;

CAccessibilityManagerService::SecurityPolicy::SecurityPolicy(
    /* [in] */ CAccessibilityManagerService* host)
    : mActiveWindowId(0)
    , mTouchInteractionInProgress(FALSE)
    , mHost(host)
{}

Boolean CAccessibilityManagerService::SecurityPolicy::CanDispatchAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    Int32 eventType;
    event->GetEventType(&eventType);
    Int32 wid;
    switch (eventType) {
        // All events that are for changes in a global window
        // state should *always* be dispatched.
        case IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED:
        case IAccessibilityEvent::TYPE_NOTIFICATION_STATE_CHANGED:
        // All events generated by the user touching the
        // screen should *always* be dispatched.
        case IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_START:
        case IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_END:
        case IAccessibilityEvent::TYPE_GESTURE_DETECTION_START:
        case IAccessibilityEvent::TYPE_GESTURE_DETECTION_END:
        case IAccessibilityEvent::TYPE_TOUCH_INTERACTION_START:
        case IAccessibilityEvent::TYPE_TOUCH_INTERACTION_END:
        // These will change the active window, so dispatch.
        case IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER:
        case IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT:
            return true;
        // All events for changes in window content should be
        // dispatched *only* if this window is the active one.
        default:
            event->GetWindowId(&wid);
            return wid == mActiveWindowId;
    }
}

void CAccessibilityManagerService::SecurityPolicy::UpdateEventSourceLocked(
    /* [in] */ IAccessibilityEvent* event)
{
    Int32 type;
    event->GetEventType(&type);
    if ((type & RETRIEVAL_ALLOWING_EVENT_TYPES) == 0) {
        event->SetSource(NULL);
    }
}

void CAccessibilityManagerService::SecurityPolicy::UpdateActiveWindow(
    /* [in] */ Int32 windowId,
    /* [in] */ Int32 eventType)
{
    // The active window is either the window that has input focus or
    // the window that the user is currently touching. If the user is
    // touching a window that does not have input focus as soon as the
    // the user stops touching that window the focused window becomes
    // the active one.
    switch (eventType) {
        case IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED: {
            if (GetFocusedWindowId() == windowId) {
                mActiveWindowId = windowId;
            }
        } break;
        case IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER: {
            // Do not allow delayed hover events to confuse us
            // which the active window is.
            if (mTouchInteractionInProgress) {
                mActiveWindowId = windowId;
            }
        } break;
    }
}

void CAccessibilityManagerService::SecurityPolicy::OnTouchInteractionStart()
{
    mTouchInteractionInProgress = TRUE;
}

void CAccessibilityManagerService::SecurityPolicy::OnTouchInteractionEnd()
{
    mTouchInteractionInProgress = FALSE;
    // We want to set the active window to be current immediately
    // after the user has stopped touching the screen since if the
    // user types with the IME he should get a feedback for the
    // letter typed in the text view which is in the input focused
    // window. Note that we always deliver hover accessibility events
    // (they are a result of user touching the screen) so change of
    // the active window before all hover accessibility events from
    // the touched window are delivered is fine.
    mActiveWindowId = GetFocusedWindowId();
}

Int32 CAccessibilityManagerService::SecurityPolicy::GetRetrievalAllowingWindowLocked()
{
    return mActiveWindowId;
}

Boolean CAccessibilityManagerService::SecurityPolicy::CanGetAccessibilityNodeInfoLocked(
    /* [in] */ IIAccessibilityServiceConnection* service,
    /* [in] */ Int32 windowId)
{
    return CanRetrieveWindowContent(service) && IsRetrievalAllowingWindow(windowId);
}

Boolean CAccessibilityManagerService::SecurityPolicy::CanPerformActionLocked(
    /* [in] */ IIAccessibilityServiceConnection* service,
    /* [in] */ Int32 windowId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    return CanRetrieveWindowContent(service)
            && IsRetrievalAllowingWindow(windowId)
            && IsActionPermitted(action);
}

Boolean CAccessibilityManagerService::SecurityPolicy::CanRetrieveWindowContent(
    /* [in] */ IIAccessibilityServiceConnection* service)
{
    CAccessibilityServiceConnection* serviceObj;
    return serviceObj->mCanRetrieveScreenContent;
}

ECode CAccessibilityManagerService::SecurityPolicy::EnforceCanRetrieveWindowContent(
    /* [in] */ IIAccessibilityServiceConnection* service)
{
    // This happens due to incorrect registration so make it apparent.
    if (!CanRetrieveWindowContent(service)) {
        CAccessibilityServiceConnection* serviceObj;
        String info;
        serviceObj->mComponentName->ToString(&info);
        Slogger::E(CAccessibilityManagerService::TAG, "Accessibility serivce %s" \
            " does not declare android:canRetrieveWindowContent.", info.string());
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode CAccessibilityManagerService::SecurityPolicy::ResolveCallingUserIdEnforcingPermissionsLocked(
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 callingUid = Binder::GetCallingUid();
    if (callingUid == IProcess::SYSTEM_UID
            || callingUid == IProcess::SHELL_UID) {
        *result = mHost->mCurrentUserId;
        return NOERROR;
    }
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 callingUserId;
    helper->GetUserId(callingUid, &callingUserId);
    if (callingUserId == userId) {
        *result = userId;
        return NOERROR;
    }
    if (!HasPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS)
            && !HasPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL)) {
        Slogger::E(CAccessibilityManagerService::TAG, "Call from user %d as user %d" \
            " without permission INTERACT_ACROSS_USERS or INTERACT_ACROSS_USERS_FULL not allowed.",
            callingUserId, userId);
        *result = -1;
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("Call from user " + callingUserId + " as user "
        //         + userId + " without permission INTERACT_ACROSS_USERS or "
        //         + "INTERACT_ACROSS_USERS_FULL not allowed.");
    }
    if (userId == IUserHandle::USER_CURRENT
            || userId == IUserHandle::USER_CURRENT_OR_SELF) {
        *result = mHost->mCurrentUserId;
        return NOERROR;
    }

    *result = -1;
    Slogger::E(CAccessibilityManagerService::TAG, "Calling user can be changed to" \
        " only IUserHandle::USER_CURRENT or IUserHandle::USER_CURRENT_OR_SELF.");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // throw new IllegalArgumentException("Calling user can be changed to only "
    //         + "IUserHandle::USER_CURRENT or IUserHandle::USER_CURRENT_OR_SELF.");
}

Boolean CAccessibilityManagerService::SecurityPolicy::IsCallerInteractingAcrossUsers(
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    return (Binder::GetCallingPid() == Process::MyPid()
            || callingUid == IProcess::SHELL_UID
            || userId == IUserHandle::USER_CURRENT
            || userId == IUserHandle::USER_CURRENT_OR_SELF);
}

Boolean CAccessibilityManagerService::SecurityPolicy::IsRetrievalAllowingWindow(
    /* [in] */ Int32 windowId)
{
    return (mActiveWindowId == windowId);
}

Boolean CAccessibilityManagerService::SecurityPolicy::IsActionPermitted(
    /* [in] */ Int32 action)
{
    return (VALID_ACTIONS & action) != 0;
}

ECode CAccessibilityManagerService::SecurityPolicy::EnforceCallingPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& function)
{
    if (CAccessibilityManagerService::OWN_PROCESS_ID == Binder::GetCallingPid()) {
        return NOERROR;
    }

    if (!HasPermission(permission)) {
        Slogger::E(CAccessibilityManagerService::TAG, "You do not have %s required to call %s",
                (const char*)permission, (const char*)function);
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("You do not have " + permission%s
        //         + " required to call " + function);
    }
    return NOERROR;
}

Boolean CAccessibilityManagerService::SecurityPolicy::HasPermission(
    /* [in] */ const String& permission)
{
    if (permission.IsNull()) return FALSE;
    Int32 result;
    mHost->mContext->CheckCallingPermission(permission, &result);
    return result == IPackageManager::PERMISSION_GRANTED;
}

Int32 CAccessibilityManagerService::SecurityPolicy::GetFocusedWindowId()
{
    // try {
    // We call this only on window focus change or after touch
    // exploration gesture end and the shown windows are not that
    // many, so the linear look up is just fine.
    AutoPtr<IBinder> token;
    ASSERT_SUCCEEDED(mHost->mWindowManagerService->GetFocusedWindowToken((IBinder**)&token));
    if (token != NULL) {
        AutoLock lock(mHost->mLock);
        Int32 windowId = GetFocusedWindowIdLocked(token, mHost->mGlobalWindowTokens);
        if (windowId < 0) {
            windowId = GetFocusedWindowIdLocked(token,
                    mHost->GetCurrentUserStateLocked()->mWindowTokens);
        }
        return windowId;
    }
    // } catch (RemoteException re) {
    //     /* ignore */
    // }
    return -1;
}

Int32 CAccessibilityManagerService::SecurityPolicy::GetFocusedWindowIdLocked(
    /* [in] */ IBinder* token,
    /* [in] */ const HashMap<Int32, AutoPtr<IBinder> >& windows)
{
    HashMap<Int32, AutoPtr<IBinder> >::ConstIterator it = windows.Begin();
    for (; it != windows.End(); ++it) {
        if (it->mSecond.Get() == token) {
            return it->mFirst;
        }
    }
    return -1;
}

//======================================================================================
// CAccessibilityManagerService::TempUserStateChangeMemento
//======================================================================================
CAccessibilityManagerService::TempUserStateChangeMemento::TempUserStateChangeMemento()
    : mUserId(IUserHandle::USER_NULL)
    , mIsAccessibilityEnabled(FALSE)
    , mIsTouchExplorationEnabled(FALSE)
    , mIsDisplayMagnificationEnabled(FALSE)
{}

void CAccessibilityManagerService::TempUserStateChangeMemento::Initialize(
    /* [in] */ Int32 userId,
    /* [in] */ UserState* userState)
{
    mUserId = userId;
    mIsAccessibilityEnabled = userState->mIsAccessibilityEnabled;
    mIsTouchExplorationEnabled = userState->mIsTouchExplorationEnabled;
    mIsDisplayMagnificationEnabled = userState->mIsDisplayMagnificationEnabled;
    mEnabledServices.Clear();
    HashSet< AutoPtr<IComponentName> >::Iterator it = userState->mEnabledServices.Begin();
    for (; it != userState->mEnabledServices.End(); ++it) {
        mEnabledServices.Insert(*it);
    }
    mTouchExplorationGrantedServices.Clear();
    it = userState->mTouchExplorationGrantedServices.Begin();
    for (; it != userState->mTouchExplorationGrantedServices.End(); ++it) {
        mTouchExplorationGrantedServices.Insert(*it);
    }
}

void CAccessibilityManagerService::TempUserStateChangeMemento::ApplyTo(
    /* [in] */ UserState* userState)
{
    userState->mIsAccessibilityEnabled = mIsAccessibilityEnabled;
    userState->mIsTouchExplorationEnabled = mIsTouchExplorationEnabled;
    userState->mIsDisplayMagnificationEnabled = mIsDisplayMagnificationEnabled;
    userState->mEnabledServices.Clear();
    HashSet< AutoPtr<IComponentName> >::Iterator it = mEnabledServices.Begin();
    for (; it != mEnabledServices.End(); ++it) {
        userState->mEnabledServices.Insert(*it);
    }
    userState->mTouchExplorationGrantedServices.Clear();
    it = mTouchExplorationGrantedServices.Begin();
    for (; it != mTouchExplorationGrantedServices.End(); ++it) {
        userState->mTouchExplorationGrantedServices.Insert(*it);
    }
}

void CAccessibilityManagerService::TempUserStateChangeMemento::Clear()
{
    mUserId = IUserHandle::USER_NULL;
    mIsAccessibilityEnabled = FALSE;
    mIsTouchExplorationEnabled = FALSE;
    mIsDisplayMagnificationEnabled = FALSE;
    mEnabledServices.Clear();
    mTouchExplorationGrantedServices.Clear();
}

//======================================================================================
// CAccessibilityManagerService::AccessibilityContentObserver
//======================================================================================
CAccessibilityManagerService::AccessibilityContentObserver::AccessibilityContentObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ CAccessibilityManagerService* host)
    : mHost(host)
{
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetUriFor(ISettingsSecure::ACCESSIBILITY_ENABLED,
        (IUri**)&mAccessibilityEnabledUri);

    settingsSecure->GetUriFor(ISettingsSecure::TOUCH_EXPLORATION_ENABLED,
        (IUri**)&mTouchExplorationEnabledUri);

    settingsSecure->GetUriFor(ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED,
        (IUri**)&mDisplayMagnificationEnabledUri);

    settingsSecure->GetUriFor(ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES,
        (IUri**)&mEnabledAccessibilityServicesUri);

    settingsSecure->GetUriFor(ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
        (IUri**)&mTouchExplorationGrantedAccessibilityServicesUri);
}

void CAccessibilityManagerService::AccessibilityContentObserver::Register(
    /* [in] */ IContentResolver* contentResolver)
{
    contentResolver->RegisterContentObserver(mAccessibilityEnabledUri,
        FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
    contentResolver->RegisterContentObserver(mTouchExplorationEnabledUri,
        FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
    contentResolver->RegisterContentObserver(mDisplayMagnificationEnabledUri,
        FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
    contentResolver->RegisterContentObserver(mEnabledAccessibilityServicesUri,
        FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
    contentResolver->RegisterContentObserver(
        mTouchExplorationGrantedAccessibilityServicesUri,
        FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
}

ECode CAccessibilityManagerService::AccessibilityContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    Boolean bval;
    if (mAccessibilityEnabledUri->Equals(uri ? uri->Probe(EIID_IInterface) : NULL, &bval), bval) {
        AutoLock lock(mHost->mLock);
        // We will update when the automation service dies.
        if (mHost->mUiAutomationService == NULL) {
            AutoPtr<UserState> userState = mHost->GetCurrentUserStateLocked();
            mHost->HandleAccessibilityEnabledSettingChangedLocked(userState);
            mHost->PerformServiceManagementLocked(userState);
            mHost->UpdateInputFilterLocked(userState);
            mHost->ScheduleSendStateToClientsLocked(userState);
        }
    }
    else if (mTouchExplorationEnabledUri->Equals(uri ? uri->Probe(EIID_IInterface) : NULL, &bval), bval) {
        AutoLock lock(mHost->mLock);
        // We will update when the automation service dies.
        if (mHost->mUiAutomationService == NULL) {
            AutoPtr<UserState> userState = mHost->GetCurrentUserStateLocked();
            mHost->HandleTouchExplorationEnabledSettingChangedLocked(userState);
            mHost->UpdateInputFilterLocked(userState);
            mHost->ScheduleSendStateToClientsLocked(userState);
        }
    }
    else if (mDisplayMagnificationEnabledUri->Equals(uri ? uri->Probe(EIID_IInterface) : NULL, &bval), bval) {
        AutoLock lock(mHost->mLock);
        // We will update when the automation service dies.
        if (mHost->mUiAutomationService == NULL) {
            AutoPtr<UserState> userState = mHost->GetCurrentUserStateLocked();
            mHost->HandleDisplayMagnificationEnabledSettingChangedLocked(userState);
            mHost->UpdateInputFilterLocked(userState);
            mHost->ScheduleSendStateToClientsLocked(userState);
        }
    }
    else if (mEnabledAccessibilityServicesUri->Equals(uri ? uri->Probe(EIID_IInterface) : NULL, &bval), bval) {
        AutoLock lock(mHost->mLock);
        // We will update when the automation service dies.
        if (mHost->mUiAutomationService == NULL) {
            AutoPtr<UserState> userState = mHost->GetCurrentUserStateLocked();
            mHost->PopulateEnabledAccessibilityServicesLocked(userState);
            mHost->ManageServicesLocked(userState);
        }
    }
    else if (mTouchExplorationGrantedAccessibilityServicesUri->Equals(uri ? uri->Probe(EIID_IInterface) : NULL, &bval), bval) {
        AutoLock lock(mHost->mLock);
        // We will update when the automation service dies.
        if (mHost->mUiAutomationService == NULL) {
            AutoPtr<UserState> userState = mHost->GetCurrentUserStateLocked();
            mHost->PopulateTouchExplorationGrantedAccessibilityServicesLocked(userState);
            mHost->HandleTouchExplorationGrantedAccessibilityServicesChangedLocked(userState);
        }
    }
    return NOERROR;
}

//======================================================================================
// CAccessibilityManagerService::UserChangedBroadcastReceiver
//======================================================================================
CAccessibilityManagerService::UserChangedBroadcastReceiver::UserChangedBroadcastReceiver(
    /* [in] */ CAccessibilityManagerService* host)
    : mHost(host)
{}

ECode CAccessibilityManagerService::UserChangedBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action.Equals(IIntent::ACTION_USER_SWITCHED)) {
        Int32 value;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &value);
        mHost->SwitchUser(value);
    }
    else if (action.Equals(IIntent::ACTION_USER_REMOVED)) {
        Int32 value;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &value);
        mHost->RemoveUser(value);
    }
    else if (action.Equals(IIntent::ACTION_USER_PRESENT)) {
        mHost->RestoreStateFromMementoIfNeeded();
    }

    return NOERROR;
}

//======================================================================================
// CAccessibilityManagerService::MyPackageMonitor
//======================================================================================
CAccessibilityManagerService::MyPackageMonitor::MyPackageMonitor(
    /* [in] */ CAccessibilityManagerService* host)
    : mHost(host)
{}

ECode CAccessibilityManagerService::MyPackageMonitor::OnSomePackagesChanged()
{
    AutoLock lock(mHost->mLock);

    Int32 userId;
    GetChangingUserId(&userId);
    if (userId != mHost->mCurrentUserId) {
        return NOERROR;
    }

    // We will update when the automation service dies.
    if (mHost->mUiAutomationService == NULL) {
        AutoPtr<UserState> userState = mHost->GetCurrentUserStateLocked();
        mHost->PopulateInstalledAccessibilityServiceLocked(userState);
        mHost->ManageServicesLocked(userState);
    }

    return NOERROR;
}

ECode CAccessibilityManagerService::MyPackageMonitor::OnPackageRemoved(
    /* [in] */ const String packageName,
    /* [in] */ Int32 uid)
{
    AutoLock lock(mHost->mLock);
    Int32 userId;
    GetChangingUserId(&userId);
    if (userId != mHost->mCurrentUserId) {
        return NOERROR;
    }

    AutoPtr<UserState> state = mHost->GetUserStateLocked(userId);
    String compPkg;
    HashSet<AutoPtr<IComponentName> >::Iterator it = state->mEnabledServices.Begin();
    for (; it != state->mEnabledServices.End();) {
        AutoPtr<IComponentName> comp = *it;
        comp->GetPackageName(&compPkg);
        if (compPkg.Equals(packageName)) {
            state->mEnabledServices.Erase(it++);

            // Update the enabled services setting.
            mHost->PersistComponentNamesToSettingLocked(
                ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES,
                &state->mEnabledServices, userId);
            // Update the touch exploration granted services setting.
            state->mTouchExplorationGrantedServices.Erase(comp);
            mHost->PersistComponentNamesToSettingLocked(
                ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
                &state->mEnabledServices, userId);
            return NOERROR;
        }
        else {
            ++it;
        }
    }

    return NOERROR;
}

ECode CAccessibilityManagerService::MyPackageMonitor::OnHandleForceStop(
    /* [in] */ IIntent* intent,
    /* [in] */ ArrayOf<String>* packages,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean doit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoLock lock(mHost->mLock);

    Int32 userId;
    GetChangingUserId(&userId);
    if (userId != mHost->mCurrentUserId) {
        return NOERROR;
    }

    AutoPtr<UserState> state = mHost->GetUserStateLocked(userId);
    String compPkg;
    HashSet<AutoPtr<IComponentName> >::Iterator it = state->mEnabledServices.Begin();
    for (; it != state->mEnabledServices.End();) {
        AutoPtr<IComponentName> comp = *it;
        comp->GetPackageName(&compPkg);
        for (Int32 i = 0; i < packages->GetLength(); ++i) {
            if (compPkg.Equals((*packages)[i])) {
                if (!doit) {
                    *result = TRUE;
                    return NOERROR;
                }

                state->mEnabledServices.Erase(it++);
                mHost->PersistComponentNamesToSettingLocked(
                    ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES,
                    &state->mEnabledServices, userId);
            }
            else {
                ++it;
            }
        }
    }

    return NOERROR;
}

//======================================================================================
// CAccessibilityManagerService
//======================================================================================
CAccessibilityManagerService::CAccessibilityManagerService()
    : mHasInputFilter(FALSE)
    , mCurrentUserId(IUserHandle::USER_OWNER)
{
    CSimpleStringSplitter::New(COMPONENT_NAME_SEPARATOR, (ISimpleStringSplitter**)&mStringColonSplitter);

    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempRect));
    ASSERT_SUCCEEDED(CPoint::New((IPoint**)&mTempPoint));

    CRemoteCallbackList::New((IRemoteCallbackList**)&mGlobalClients);;
    mTempStateChangeForCurrentUserMemento = new TempUserStateChangeMemento();
}

ECode CAccessibilityManagerService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    mContext->GetPackageManager((IPackageManager**)&mPackageManager);
    AutoPtr<IServiceManager> sm;
    FAIL_RETURN(CServiceManager::AcquireSingleton((IServiceManager**)&sm));
    FAIL_RETURN(sm->GetService(IContext::WINDOW_SERVICE, (IInterface**)&mWindowManagerService));
    mSecurityPolicy = new SecurityPolicy(this);
    AutoPtr<ILooper> looper;
    FAIL_RETURN(mContext->GetMainLooper((ILooper**)&looper));
    mMainHandler = new MainHandler(looper, this);
    //TODO: (multi-display) We need to support multiple displays.
    AutoPtr<IInterface> obj;
    FAIL_RETURN(mContext->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&obj));
    AutoPtr<IDisplayManager> displayManager = IDisplayManager::Probe(obj);
    FAIL_RETURN(displayManager->GetDisplay(IDisplay::DEFAULT_DISPLAY, (IDisplay**)&mDefaultDisplay));
    RegisterBroadcastReceivers();
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<AccessibilityContentObserver> observer = new AccessibilityContentObserver(mMainHandler, this);
    observer->Register(cr);
    return NOERROR;
}

AutoPtr<CAccessibilityManagerService::UserState> CAccessibilityManagerService::GetCurrentUserStateLocked()
{
    return GetUserStateLocked(mCurrentUserId);
}

AutoPtr<CAccessibilityManagerService::UserState> CAccessibilityManagerService::GetUserStateLocked(
    /* [in] */ Int32 userId)
{
    AutoPtr<UserState> state;
    HashMap<Int32, AutoPtr<UserState> >::Iterator it = mUserStates.Find(userId);
    if (it != mUserStates.End()) {
        state = it->mSecond;
    }
    if (state == NULL) {
        state = new UserState(userId);
        mUserStates[userId] = state;
    }
    return state;
}

void CAccessibilityManagerService::RegisterBroadcastReceivers()
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> all;
    helper->GetALL((IUserHandle**)&all);

    AutoPtr<IPackageMonitor> monitor =  new MyPackageMonitor(this);
    // package changes
    monitor->Register(mContext, NULL, all, TRUE);

    // user change and unlock
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_USER_SWITCHED);
    intentFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    intentFilter->AddAction(IIntent::ACTION_USER_PRESENT);

    String nullStr;
    AutoPtr<IBroadcastReceiver> receiver = new UserChangedBroadcastReceiver(this);
    AutoPtr<IIntent> sticky;
    mContext->RegisterReceiverAsUser(receiver, all, intentFilter, nullStr, NULL, (IIntent**)&sticky);
}

ECode CAccessibilityManagerService::AddClient(
    /* [in] */ IIAccessibilityManagerClient* client,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mLock);
    Int32 resolvedUserId;
    mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(userId, &resolvedUserId);
    // If the client is from a process that runs across users such as
    // the system UI or the system we add it to the global state that
    // is shared across users.
    AutoPtr<UserState> userState = GetUserStateLocked(resolvedUserId);
    if (mSecurityPolicy->IsCallerInteractingAcrossUsers(userId)) {
        Boolean res;
        mGlobalClients->Register((IInterface*)client, &res);
        if (DEBUG) {
            Slogger::I(TAG, "Added global client for pid: %d", Binder::GetCallingPid());
        }
        *result = GetClientState(userState);
    }
    else {
        Boolean res;
        userState->mClients->Register((IInterface*)client, &res);
        // If this client is not for the current user we do not
        // return a state since it is not for the foreground user.
        // We will send the state to the client on a user switch.
        if (DEBUG) {
            Slogger::I(TAG, "Added user client for pid: %d  and userId:",
                Binder::GetCallingPid(), mCurrentUserId);
        }
        *result = (resolvedUserId == mCurrentUserId) ? GetClientState(userState) : 0;
    }

    return NOERROR;
}

ECode CAccessibilityManagerService::SendAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mLock);
    Int32 resolvedUserId;
    mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(userId, &resolvedUserId);
    // This method does nothing for a background user.
    if (resolvedUserId != mCurrentUserId) {
        *result = TRUE;
        return NOERROR; // yes, recycle the event
    }

    if (mSecurityPolicy->CanDispatchAccessibilityEvent(event)) {
        mSecurityPolicy->UpdateEventSourceLocked(event);
        Int32 windowId, eventType;
        event->GetWindowId(&windowId);
        event->GetEventType(&eventType);

        AutoPtr<IMessage> msg;
        mMainHandler->ObtainMessage(MainHandler::MSG_UPDATE_ACTIVE_WINDOW,
            windowId, eventType, (IMessage**)&msg);
        Boolean result;
        mMainHandler->SendMessage(msg, &result);

        NotifyAccessibilityServicesDelayedLocked(event, FALSE);
        NotifyAccessibilityServicesDelayedLocked(event, TRUE);
    }

    if (mHasInputFilter && mInputFilter != NULL) {
        AutoPtr<IAccessibilityEventHelper> helper;
        CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
        AutoPtr<IAccessibilityEvent> outEvent;
        helper->Obtain(event, (IAccessibilityEvent**)&outEvent);

        AutoPtr<IMessage> msg;
        mMainHandler->ObtainMessage(MainHandler::MSG_SEND_ACCESSIBILITY_EVENT_TO_INPUT_FILTER,
            outEvent, (IMessage**)&msg);
        Boolean result;
        mMainHandler->SendMessage(msg, &result);
    }

    event->Recycle();
    GetUserStateLocked(resolvedUserId)->mHandledFeedbackTypes = 0;

    *result = (OWN_PROCESS_ID != Binder::GetCallingPid());
    return NOERROR;
}

ECode CAccessibilityManagerService::GetInstalledAccessibilityServiceList(
    /* [in] */ Int32 userId,
    /* [out] */ IObjectContainer** result)
{
    VALIDATE_NOT_NULL(result);

    AutoLock lock(mLock);
    Int32 resolvedUserId;
    mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(userId, &resolvedUserId);
    AutoPtr<IObjectContainer> services;
    CObjectContainer::New((IObjectContainer**)&services);
    AutoPtr<UserState> userState = GetUserStateLocked(resolvedUserId);
    List<AutoPtr<IAccessibilityServiceInfo> >::Iterator it = userState->mInstalledServices.Begin();
    for (; it != userState->mInstalledServices.End(); ++it) {
        services->Add((*it).Get());
    }

    *result = services;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAccessibilityManagerService::GetEnabledAccessibilityServiceList(
    /* [in] */ Int32 feedbackType,
    /* [in] */ Int32 userId,
    /* [out] */ IObjectContainer** _services)
{
    VALIDATE_NOT_NULL(_services);

    AutoPtr<IObjectContainer> result;

    AutoLock lock(mLock);
    CObjectContainer::New((IObjectContainer**)&result);
    mEnabledServicesForFeedbackTempList.Clear();

    Int32 resolvedUserId;
    mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(userId, &resolvedUserId);
    AutoPtr<UserState> userState = GetUserStateLocked(resolvedUserId);
    List<AutoPtr<IIAccessibilityServiceConnection> >& services = userState->mServices;
    Int32 notz, feedbackTypeBit;
    CAccessibilityServiceConnection* service;
    while (feedbackType != 0) {
        notz = Elastos::Core::Math::NumberOfTrailingZeros(feedbackType);
        feedbackTypeBit = (1 << notz);
        feedbackType &= ~feedbackTypeBit;
        List<AutoPtr<IIAccessibilityServiceConnection> >::Iterator it = services.Begin();
        for (; it != services.End(); ++it) {
            service = (CAccessibilityServiceConnection*)(*it).Get();
            if ((service->mFeedbackType & feedbackTypeBit) != 0) {
                result->Add(service->mAccessibilityServiceInfo.Get());
                mEnabledServicesForFeedbackTempList.PushBack(service->mAccessibilityServiceInfo);
            }
        }
    }

    *_services = result;
    REFCOUNT_ADD(*_services);
    return NOERROR;
}

ECode CAccessibilityManagerService::Interrupt(
    /* [in] */ Int32 userId)
{
    // CopyOnWriteArrayList<Service> services;
    AutoPtr<List<AutoPtr<IIAccessibilityServiceConnection> > > services;
    {
        AutoLock lock(mLock);
        Int32 resolvedUserId;
        mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(userId, &resolvedUserId);
        // This method does nothing for a background user.
        if (resolvedUserId != mCurrentUserId) {
            return NOERROR;
        }
        AutoPtr<UserState> userState = GetUserStateLocked(resolvedUserId);
        services = new List<AutoPtr<IIAccessibilityServiceConnection> >(userState->mServices);
    }

    CAccessibilityServiceConnection* service;
    ECode ec;
    List<AutoPtr<IIAccessibilityServiceConnection> >::Iterator it = services->Begin();
    for (; it != services->End(); ++it) {
        service = (CAccessibilityServiceConnection*)(*it).Get();
        // try {
        ec = service->mServiceInterface->OnInterrupt();
        if (FAILED(ec)) {
            Slogger::E(TAG, "Error during sending interrupt request to %p 0x%08x",
                service->mService.Get(), ec);
        }
        // } catch (RemoteException re) {
        //     Slogger::E(TAG, "Error during sending interrupt request to "
        //         + service.mService, re);
        // }
    }

    return NOERROR;
}

ECode CAccessibilityManagerService::AddAccessibilityInteractionConnection(
    /* [in] */ IIWindow* windowToken,
    /* [in] */ IAccessibilityInteractionConnection* connection,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    assert(connection != NULL);

    AutoLock lock(mLock);
    Int32 resolvedUserId;
    mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(userId, &resolvedUserId);
    Int32 windowId = sNextWindowId++;
    // If the window is from a process that runs across users such as
    // the system UI or the system we add it to the global state that
    // is shared across users.
    if (mSecurityPolicy->IsCallerInteractingAcrossUsers(userId)) {
        AutoPtr<AccessibilityConnectionWrapper> wrapper = new AccessibilityConnectionWrapper(
            windowId, connection, IUserHandle::USER_ALL, this);
        wrapper->LinkToDeath();
        mGlobalInteractionConnections[windowId] = wrapper;
        mGlobalWindowTokens[windowId] = IBinder::Probe(windowToken);
        if (DEBUG) {
            Slogger::I(TAG, "Added global connection for pid: %d with windowId: ",
                Binder::GetCallingPid(), windowId);
        }
    }
    else {
        AutoPtr<AccessibilityConnectionWrapper> wrapper = new AccessibilityConnectionWrapper(
            windowId, connection, resolvedUserId, this);
        wrapper->LinkToDeath();
        AutoPtr<UserState> userState = GetUserStateLocked(resolvedUserId);
        userState->mInteractionConnections[windowId] = wrapper;
        userState->mWindowTokens[windowId] = IBinder::Probe(windowToken);
        if (DEBUG) {
            Slogger::I(TAG, "Added user connection for pid:%d with windowId: %d and userId:%d",
                Binder::GetCallingPid(), windowId, mCurrentUserId);
        }
    }
    if (DEBUG) {
        Slogger::I(TAG, "Adding interaction connection to windowId: %d", windowId);
    }
    *result = windowId;
    return NOERROR;
}

ECode CAccessibilityManagerService::RemoveAccessibilityInteractionConnection(
    /* [in] */ IIWindow* window)
{
    AutoLock lock(mLock);
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 id, result;
    helper->GetCallingUserId(&id);
    mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(id, &result);
    AutoPtr<IBinder> token = IBinder::Probe(window);
    Int32 removedWindowId = RemoveAccessibilityInteractionConnectionInternalLocked(
        token, mGlobalWindowTokens, mGlobalInteractionConnections);
    if (removedWindowId >= 0) {
        if (DEBUG) {
            Slogger::I(TAG, "Removed global connection for pid: %d with windowId: %d",
                Binder::GetCallingPid(), removedWindowId);
        }
        return NOERROR;
    }

    HashMap<Int32, AutoPtr<UserState> >::Iterator it = mUserStates.Begin();
    for (; it != mUserStates.End(); ++it) {
        AutoPtr<UserState> userState = it->mSecond;
        Int32 removedWindowIdForUser = RemoveAccessibilityInteractionConnectionInternalLocked(
            token, userState->mWindowTokens, userState->mInteractionConnections);
        if (removedWindowIdForUser >= 0) {
            if (DEBUG) {
                Slogger::I(TAG, "Removed user connection for pid: %d  with windowId: %d and userId: %d",
                    Binder::GetCallingPid(), removedWindowIdForUser, it->mFirst);
            }
            return NOERROR;
        }
    }
    return NOERROR;
}

Int32 CAccessibilityManagerService::RemoveAccessibilityInteractionConnectionInternalLocked(
    /* [in] */ IBinder* windowToken,
    /* [in] */ HashMap<Int32, AutoPtr<IBinder> > windowTokens,
    /* [in] */ HashMap<Int32, AutoPtr<AccessibilityConnectionWrapper> > interactionConnections)
{
    HashMap<Int32, AutoPtr<IBinder> >::Iterator it = windowTokens.Begin();
    for (; it != windowTokens.End();) {
        if (it->mSecond.Get() == windowToken) {
            Int32 windowId = it->mFirst;
            windowTokens.Erase(it++);

            HashMap<Int32, AutoPtr<AccessibilityConnectionWrapper> >::Iterator wrapperIt =
                interactionConnections.Find(windowId);
            if (wrapperIt != interactionConnections.End()) {
                AutoPtr<AccessibilityConnectionWrapper> wrapper = wrapperIt->mSecond;
                wrapper->UnlinkToDeath();
                interactionConnections.Erase(wrapperIt);
            }
            return windowId;
        }
        else {
            ++it;
        }
    }
    return -1;
}

ECode CAccessibilityManagerService::RegisterUiTestAutomationService(
    /* [in] */ IIAccessibilityServiceClient* serviceClient,
    /* [in] */ IAccessibilityServiceInfo* accessibilityServiceInfo)
{
    FAIL_RETURN(mSecurityPolicy->EnforceCallingPermission(
        Elastos::Droid::Manifest::permission::RETRIEVE_WINDOW_CONTENT,
        FUNCTION_REGISTER_UI_TEST_AUTOMATION_SERVICE));
    AutoPtr<IComponentName> componentName;
    CComponentName::New(String("foo.bar"), String("AutomationAccessibilityService"),
         (IComponentName**)&componentName);

    AutoLock lock(mLock);
    // If an automation services is connected to the system all services are stopped
    // so the automation one is the only one running. Settings are not changed so when
    // the automation service goes away the state is restored from the settings.
    AutoPtr<UserState> userState = GetCurrentUserStateLocked();
    UnbindAllServicesLocked(userState);

    // If necessary enable accessibility and announce that.
    if (!userState->mIsAccessibilityEnabled) {
        userState->mIsAccessibilityEnabled = TRUE;
    }
    // No touch exploration.
    userState->mIsTouchExplorationEnabled = FALSE;

    // Hook the automation service up.
    AutoPtr<ILooper> looper;
    mMainHandler->GetLooper((ILooper**)&looper);
    mUiAutomationService = NULL;
    CAccessibilityServiceConnection::New(looper, mCurrentUserId, componentName,
        accessibilityServiceInfo, TRUE, THIS_PROBE(IIAccessibilityManager),
        (IIAccessibilityServiceConnection**)&mUiAutomationService);
    IServiceConnection* sc = IServiceConnection::Probe(mUiAutomationService);
    sc->OnServiceConnected(componentName, IBinder::Probe(serviceClient));

    UpdateInputFilterLocked(userState);
    ScheduleSendStateToClientsLocked(userState);
    return NOERROR;
}

ECode CAccessibilityManagerService::TemporaryEnableAccessibilityStateUntilKeyguardRemoved(
    /* [in] */ IComponentName* service,
    /* [in] */ Boolean touchExplorationEnabled)
{
    FAIL_RETURN(mSecurityPolicy->EnforceCallingPermission(
        Elastos::Droid::Manifest::permission::TEMPORARY_ENABLE_ACCESSIBILITY,
        TEMPORARY_ENABLE_ACCESSIBILITY_UNTIL_KEYGUARD_REMOVED));
    // try {
    Boolean isSecure;
    FAIL_RETURN(mWindowManagerService->IsKeyguardLocked(&isSecure));
    if (!isSecure) {
        return NOERROR;
    }
    // } catch (RemoteException re) {
    //     return;
    // }
    AutoLock lock(mLock);
    AutoPtr<UserState> userState = GetCurrentUserStateLocked();
    // Stash the old state so we can restore it when the keyguard is gone.
    mTempStateChangeForCurrentUserMemento->Initialize(mCurrentUserId, GetCurrentUserStateLocked());
    // Set the temporary state.
    userState->mIsAccessibilityEnabled = TRUE;
    userState->mIsTouchExplorationEnabled= touchExplorationEnabled;
    userState->mIsDisplayMagnificationEnabled = FALSE;
    userState->mEnabledServices.Clear();
    userState->mEnabledServices.Insert(service);
    userState->mTouchExplorationGrantedServices.Clear();
    userState->mTouchExplorationGrantedServices.Insert(service);
    // Update the internal state.
    PerformServiceManagementLocked(userState);
    UpdateInputFilterLocked(userState);
    ScheduleSendStateToClientsLocked(userState);
    return NOERROR;
}

ECode CAccessibilityManagerService::UnregisterUiTestAutomationService(
    /* [in] */ IIAccessibilityServiceClient* serviceClient)
{
    AutoLock lock(mLock);
    CAccessibilityServiceConnection* sc = (CAccessibilityServiceConnection*)mUiAutomationService.Get();
    // Automation service is not bound, so pretend it died to perform clean up.
    if (sc != NULL && sc->mServiceInterface != NULL
        && serviceClient != NULL
        && IBinder::Probe(sc->mServiceInterface.Get()) == IBinder::Probe(serviceClient))
    {
        sc->ProxyDied();
    }
    return NOERROR;
}

Boolean CAccessibilityManagerService::OnGesture(
    /* [in] */ Int32 gestureId)
{
    AutoLock lock(mLock);
    Boolean handled = NotifyGestureLocked(gestureId, FALSE);
    if (!handled) {
        handled = NotifyGestureLocked(gestureId, TRUE);
    }
    return handled;
}

Boolean CAccessibilityManagerService::GetAccessibilityFocusBoundsInActiveWindow(
    /* [in] */ IRect* outBounds)
{
    // Instead of keeping track of accessibility focus events per
    // window to be able to find the focus in the active window,
    // we take a stateless approach and look it up. This is fine
    // since we do this only when the user clicks/long presses.
    AutoPtr<IIAccessibilityServiceConnection> service = GetQueryBridge();
    CAccessibilityServiceConnection* sc = (CAccessibilityServiceConnection*)service.Get();
    Int32 connectionId = sc->mId;
    AutoPtr<IAccessibilityInteractionClient> client;
    AutoPtr<IAccessibilityInteractionClientHelper> helper;
    CAccessibilityInteractionClientHelper::AcquireSingleton(
        (IAccessibilityInteractionClientHelper**)&helper);
    ASSERT_SUCCEEDED(helper->GetInstance((IAccessibilityInteractionClient**)&client));
    client->AddConnection(connectionId, (IIAccessibilityServiceConnection*)service);

    // try {
    AutoPtr<IAccessibilityNodeInfo> root;
    if (FAILED(client->GetRootInActiveWindow(connectionId, (IAccessibilityNodeInfo**)&root))) {
        client->RemoveConnection(connectionId);
        return FALSE;
    }

    AutoPtr<IAccessibilityNodeInfo> focus;
    if (FAILED(root->FindFocus(IAccessibilityNodeInfo::FOCUS_ACCESSIBILITY,
        (IAccessibilityNodeInfo**)&focus))) {
        client->RemoveConnection(connectionId);
        return FALSE;
    }

    focus->GetBoundsInScreen(outBounds);
    // Clip to the window rectangle.
    AutoPtr<IRect> windowBounds = mTempRect;
    GetActiveWindowBounds(windowBounds);
    Boolean result;
    outBounds->Intersect(windowBounds, &result);
    // Clip to the screen rectangle.
    mDefaultDisplay->GetRealSize(mTempPoint);
    Int32 x, y;
    mTempPoint->GetX(&x);
    mTempPoint->GetY(&y);
    outBounds->Intersect(0,  0,  x, y, &result);

    client->RemoveConnection(connectionId);
    return TRUE;
    // } finally {
    //     client.removeConnection(connectionId);
    // }
}

Boolean CAccessibilityManagerService::GetActiveWindowBounds(
    /* [in] */ IRect* outBounds)
{
    AutoPtr<IBinder> token;
    {
        AutoLock lock(mLock);
        Int32 windowId = mSecurityPolicy->mActiveWindowId;
        HashMap<Int32, AutoPtr<IBinder> >::Iterator it = mGlobalWindowTokens.Find(windowId);
        if (it != mGlobalWindowTokens.End()) {
            token = it->mSecond;
        }
        if (token == NULL) {
            AutoPtr<UserState> userState = GetCurrentUserStateLocked();
            HashMap<Int32, AutoPtr<IBinder> >::Iterator tokenIt =
                    userState->mWindowTokens.Find(windowId);
            if (tokenIt != userState->mWindowTokens.End()) {
                token = tokenIt->mSecond;
            }
        }
    }
    AutoPtr<IWindowInfo> info;
    // try {
    if (FAILED(mWindowManagerService->GetWindowInfo(token, (IWindowInfo**)&info))) {
        if (info != NULL) info->Recycle();
        return FALSE;
    }

    if (info != NULL) {
        AutoPtr<IRect> frame;
        info->GetFrame((IRect**)&frame);
        outBounds->Set(frame);
        info->Recycle();
        return TRUE;
    }

    if (info != NULL) info->Recycle();
    return FALSE;
}

Int32 CAccessibilityManagerService::GetActiveWindowId()
{
    return mSecurityPolicy->mActiveWindowId;
}

void CAccessibilityManagerService::OnTouchInteractionStart()
{
    return mSecurityPolicy->OnTouchInteractionStart();
}

void CAccessibilityManagerService::OnTouchInteractionEnd()
{
    return mSecurityPolicy->OnTouchInteractionEnd();
}

void CAccessibilityManagerService::SwitchUser(
    /* [in] */ Int32 userId)
{
    AutoLock lock(mLock);
    // The user switched so we do not need to restore the current user
    // state since we will fully rebuild it when he becomes current again.
    mTempStateChangeForCurrentUserMemento.Clear();

    // Disconnect from services for the old user.
    AutoPtr<UserState> oldUserState = GetUserStateLocked(mCurrentUserId);
    UnbindAllServicesLocked(oldUserState);

    // Disable the local managers for the old user.
    Int32 count;
    if (oldUserState->mClients->GetRegisteredCallbackCount(&count), count > 0) {
        AutoPtr<IMessage> msg;
        mMainHandler->ObtainMessage(MainHandler::MSG_SEND_CLEARED_STATE_TO_CLIENTS_FOR_USER,
            oldUserState->mUserId, 0, (IMessage**)&msg);
        Boolean result;
        mMainHandler->SendMessage(msg, &result);
    }

    // Announce user changes only if more that one exist.
    Int32 size = 0;

    AutoPtr<IUserManager> userManager;
    ASSERT_SUCCEEDED(mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&userManager));
    if (userManager != NULL) {
        AutoPtr<IObjectContainer> users;
        ASSERT_SUCCEEDED(userManager->GetUsers((IObjectContainer**)&users));
        users->GetObjectCount(&size);
    }

    Boolean announceNewUser = size > 1;

    // The user changed.
    mCurrentUserId = userId;

    // Recreate the internal state for the new user.
    AutoPtr<IMessage> msg;
    mMainHandler->ObtainMessage(MainHandler::MSG_SEND_RECREATE_INTERNAL_STATE,
        mCurrentUserId, 0, (IMessage**)&msg);
    Boolean result;
    mMainHandler->SendMessage(msg, &result);

    if (announceNewUser) {
        // Schedule announcement of the current user if needed.
        Boolean result;
        mMainHandler->SendEmptyMessageDelayed(
            MainHandler::MSG_ANNOUNCE_NEW_USER_IF_NEEDED,
            WAIT_FOR_USER_STATE_FULLY_INITIALIZED_MILLIS, &result);
    }
}

void CAccessibilityManagerService::RemoveUser(
    /* [in] */ Int32 userId)
{
    AutoLock lock(mLock);
    mUserStates.Erase(userId);
}

void CAccessibilityManagerService::RestoreStateFromMementoIfNeeded()
{
    AutoLock lock(mLock);
    if (mTempStateChangeForCurrentUserMemento->mUserId != IUserHandle::USER_NULL) {
        AutoPtr<UserState> userState = GetCurrentUserStateLocked();
        // Restore the state from the memento.
        mTempStateChangeForCurrentUserMemento->ApplyTo(userState);
        mTempStateChangeForCurrentUserMemento->Clear();
        // Update the internal state.
        PerformServiceManagementLocked(userState);
        UpdateInputFilterLocked(userState);
        ScheduleSendStateToClientsLocked(userState);
    }
}

AutoPtr<IIAccessibilityServiceConnection> CAccessibilityManagerService::GetQueryBridge()
{
    if (mQueryBridge == NULL) {
        AutoPtr<ILooper> looper;
        mMainHandler->GetLooper((ILooper**)&looper);
        AutoPtr<IAccessibilityServiceInfo> info;
        ASSERT_SUCCEEDED(CAccessibilityServiceInfo::New((IAccessibilityServiceInfo**)&info));
        ASSERT_SUCCEEDED(CAccessibilityServiceConnection::New(
            looper, IUserHandle::USER_NULL, NULL, info, TRUE,
            THIS_PROBE(IIAccessibilityManager),
            (IIAccessibilityServiceConnection**)&mQueryBridge));
    }
    return mQueryBridge;
}

Boolean CAccessibilityManagerService::NotifyGestureLocked(
    /* [in] */ Int32 gestureId,
    /* [in] */ Boolean isDefault)
{
    // TODO: Now we are giving the gestures to the last enabled
    //       service that can handle them which is the last one
    //       in our list since we write the last enabled as the
    //       last record in the enabled services setting. Ideally,
    //       the user should make the call which service handles
    //       gestures. However, only one service should handle
    //       gestures to avoid user frustration when different
    //       behavior is observed from different combinations of
    //       enabled accessibility services.
    AutoPtr<UserState> state = GetCurrentUserStateLocked();
    CAccessibilityServiceConnection* service;
    List<AutoPtr<IIAccessibilityServiceConnection> >::ReverseIterator rit = state->mServices.RBegin();
    for (; rit != state->mServices.REnd(); ++rit) {
        service = (CAccessibilityServiceConnection*)(*rit).Get();
        if (service->mRequestTouchExplorationMode && service->mIsDefault == isDefault) {
            service->NotifyGesture(gestureId);
            return TRUE;
        }
    }
    return FALSE;
}

void CAccessibilityManagerService::RemoveAccessibilityInteractionConnectionLocked(
    /* [in] */ Int32 windowId,
    /* [in] */ Int32 userId)
{
    if (userId == IUserHandle::USER_ALL) {
        mGlobalWindowTokens.Erase(windowId);
        mGlobalInteractionConnections.Erase(windowId);
    }
    else {
        AutoPtr<UserState> userState = GetCurrentUserStateLocked();
        userState->mWindowTokens.Erase(windowId);
        userState->mInteractionConnections.Erase(windowId);
    }
    if (DEBUG) {
        Slogger::I(TAG, "Removing interaction connection to windowId: %d", windowId);
    }
}

void CAccessibilityManagerService::PopulateInstalledAccessibilityServiceLocked(
    /* [in] */ CAccessibilityManagerService::UserState* userState)
{
    userState->mInstalledServices.Clear();

    AutoPtr<IObjectContainer> installedServices;
    AutoPtr<IIntent> intent;
    CIntent::New(IAccessibilityService::SERVICE_INTERFACE, (IIntent**)&intent);
    ASSERT_SUCCEEDED(mPackageManager->QueryIntentServicesAsUser(intent,
        IPackageManager::GET_SERVICES | IPackageManager::GET_META_DATA,
        mCurrentUserId, (IObjectContainer**)&installedServices));
    AutoPtr<IObjectEnumerator> enumerator;
    installedServices->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext = FALSE;
    while(enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IResolveInfo> resolveInfo;
        enumerator->Current((IInterface**)&resolveInfo);
        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        String permission;
        serviceInfo->GetPermission(&permission);
        if (!Elastos::Droid::Manifest::permission::BIND_ACCESSIBILITY_SERVICE.Equals(permission)) {
            // Slogger::W(TAG, "Skipping accessibilty service " + new ComponentName(
            //         serviceInfo.packageName, serviceInfo.name).flattenToShortString()
            //         + ": it does not require the permission "
            //         + Elastos::Droid::Manifest::permission::BIND_ACCESSIBILITY_SERVICE);
            continue;
        }
        AutoPtr<IAccessibilityServiceInfo> accessibilityServiceInfo;
        // try {
        ECode ec = CAccessibilityServiceInfo::New(resolveInfo, mContext,
            (IAccessibilityServiceInfo**)&accessibilityServiceInfo);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Error while initializing AccessibilityServiceInfo 0x%08x", ec);
            return;
        }
        userState->mInstalledServices.PushBack(accessibilityServiceInfo);
        // } catch (XmlPullParserException xppe) {
        //     Slogger::E(TAG, "Error while initializing AccessibilityServiceInfo", xppe);
        // } catch (IOException ioe) {
        //     Slogger::E(TAG, "Error while initializing AccessibilityServiceInfo", ioe);
        // }
    }
}

void CAccessibilityManagerService::PopulateEnabledAccessibilityServicesLocked(
    /* [in] */ UserState* userState)
{
    PopulateComponentNamesFromSettingLocked(
        ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES,
        userState->mUserId,
        &userState->mEnabledServices);
}

void CAccessibilityManagerService::PopulateTouchExplorationGrantedAccessibilityServicesLocked(
    /* [in] */ UserState* userState)
{
    PopulateComponentNamesFromSettingLocked(
        ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
        userState->mUserId,
        &userState->mTouchExplorationGrantedServices);
}

void CAccessibilityManagerService::NotifyAccessibilityServicesDelayedLocked(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Boolean isDefault)
{
    // try {
    CAccessibilityServiceConnection* service;
    AutoPtr<UserState> state = GetCurrentUserStateLocked();
    List<AutoPtr<IIAccessibilityServiceConnection> >::Iterator it = state->mServices.Begin();
    for (; it != state->mServices.End(); ++it) {
        service = (CAccessibilityServiceConnection*)(*it).Get();
        if (service->mIsDefault == isDefault) {
            if (CanDispathEventLocked(service, event, state->mHandledFeedbackTypes)) {
                state->mHandledFeedbackTypes |= service->mFeedbackType;
                service->NotifyAccessibilityEvent(event);
            }
        }
    }
    // } catch (IndexOutOfBoundsException oobe) {
    //     // An out of bounds exception can happen if services are going away
    //     // as the for loop is running. If that happens, just bail because
    //     // there are no more services to notify.
    //     return;
    // }
}

void CAccessibilityManagerService::TryAddServiceLocked(
    /* [in] */ IIAccessibilityServiceConnection* service,
    /* [in] */ Int32 userId)
{
    // try {
    AutoPtr<UserState> userState = GetUserStateLocked(userId);
    List<AutoPtr<IIAccessibilityServiceConnection> >::Iterator it = userState->mServices.Begin();
    for (; it != userState->mServices.End(); ++it) {
        if ((*it).Get() == service) {
            return;
        }
    }

    CAccessibilityServiceConnection* sc = (CAccessibilityServiceConnection*)service;
    // IProxyDeathRecipient::Probe(service)->LinkToOwnDeath();
    userState->mServices.PushBack(service);
    userState->mComponentNameToServiceMap[sc->mComponentName] = service;
    UpdateInputFilterLocked(userState);
    TryEnableTouchExplorationLocked(service);
    // } catch (RemoteException e) {
    //     /* do nothing */
    // }
}

Boolean CAccessibilityManagerService::TryRemoveServiceLocked(
    /* [in] */ IIAccessibilityServiceConnection* service)
{
    CAccessibilityServiceConnection* sc = (CAccessibilityServiceConnection*)service;
    AutoPtr<UserState> userState = GetUserStateLocked(sc->mUserId);
    AutoPtr<IIAccessibilityServiceConnection> as = service;
    List<AutoPtr<IIAccessibilityServiceConnection> >::Iterator it;
    it = Find(userState->mServices.Begin(), userState->mServices.End(), as);

    Boolean removed  = FALSE;
    if (it != userState->mServices.End()) {
        userState->mServices.Erase(it);
        removed = TRUE;
    }

    if (!removed) {
        return FALSE;
    }

    userState->mComponentNameToServiceMap.Erase(sc->mComponentName);
    sc->UnlinkToOwnDeath();
    sc->Dispose();
    UpdateInputFilterLocked(userState);
    TryDisableTouchExplorationLocked(service);
    return removed;
}

Boolean CAccessibilityManagerService::CanDispathEventLocked(
    /* [in] */ IIAccessibilityServiceConnection* service,
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 handledFeedbackTypes)
{
    CAccessibilityServiceConnection* sc = (CAccessibilityServiceConnection*)service;
    if (!sc->CanReceiveEvents()) {
        return FALSE;
    }

    Boolean result;
    if ((event->IsImportantForAccessibility(&result), !result)
        && !sc->mIncludeNotImportantViews) {
        return FALSE;
    }

    Int32 eventType;
    event->GetEventType(&eventType);
    if ((sc->mEventTypes & eventType) != eventType) {
        return FALSE;
    }

    AutoPtr<ICharSequence> packageName;
    event->GetPackageName((ICharSequence**)&packageName);
    String strName;
    packageName->ToString(&strName);

    HashSet<String>::Iterator it = Find(sc->mPackageNames.Begin(), sc->mPackageNames.End(), strName);
    if (sc->mPackageNames.IsEmpty() || it != sc->mPackageNames.End()) {
        Int32 feedbackType = sc->mFeedbackType;
        if ((handledFeedbackTypes & feedbackType) != feedbackType
            || feedbackType == IAccessibilityServiceInfo::FEEDBACK_GENERIC) {
            return TRUE;
        }
    }

    return FALSE;
}

void CAccessibilityManagerService::ManageServicesLocked(
    /* [in] */ UserState* userState)
{
    Int32 enabledInstalledServicesCount = UpdateServicesStateLocked(userState);
    // No enabled installed services => disable accessibility to avoid
    // sending accessibility events with no recipient across processes.
    if (userState->mIsAccessibilityEnabled && enabledInstalledServicesCount == 0) {
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        Boolean result;
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        settingsSecure->PutInt32ForUser(cr, ISettingsSecure::ACCESSIBILITY_ENABLED,
            0, userState->mUserId, &result);
    }
}

void CAccessibilityManagerService::UnbindAllServicesLocked(
    /* [in] */ UserState* userState)
{
    CAccessibilityServiceConnection* sc;
    List<AutoPtr<IIAccessibilityServiceConnection> > services(userState->mServices);
    List<AutoPtr<IIAccessibilityServiceConnection> >::Iterator it = services.Begin();
    for (; it != services.End(); ++it) {
        sc = (CAccessibilityServiceConnection*)(*it).Get();
        sc->Unbind();
    }
}

void CAccessibilityManagerService::PopulateComponentNamesFromSettingLocked(
    /* [in] */ const String& settingName,
    /* [in] */ Int32 userId,
    /* [in] */ HashSet<AutoPtr<IComponentName> >* outComponentNames)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String settingValue;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetStringForUser(resolver, settingName, userId, &settingValue);
    outComponentNames->Clear();
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    if (!settingValue.IsNull()) {
        AutoPtr<ISimpleStringSplitter> splitter = mStringColonSplitter;
        splitter->SetString(settingValue);
        Boolean hasNext;
        String str;
        while (splitter->HasNext(&hasNext), hasNext) {
            splitter->NextString(&str);
            if (str.IsNullOrEmpty()) {
                continue;
            }

            AutoPtr<IComponentName> enabledService;
            helper->UnflattenFromString(str, (IComponentName**)&enabledService);
            if (enabledService != NULL) {
                outComponentNames->Insert(enabledService);
            }
        }
    }
}

void CAccessibilityManagerService::PersistComponentNamesToSettingLocked(
    /* [in] */ const String& settingName,
    /* [in] */ HashSet<AutoPtr<IComponentName> >* componentNames,
    /* [in] */ Int32 userId)
{
    StringBuilder builder;
    String s;
    HashSet<AutoPtr<IComponentName> >::Iterator it = componentNames->Begin();
    for (; it != componentNames->End(); ++it) {
        AutoPtr<IComponentName> componentName;
        if (builder.GetByteCount() > 0) {
            builder.AppendChar(COMPONENT_NAME_SEPARATOR);
        }

        componentName->FlattenToShortString(&s);
        builder.AppendString(s);
    }
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Boolean result;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->PutStringForUser(resolver,
        settingName, builder.ToString(), userId, &result);
}

Int32 CAccessibilityManagerService::UpdateServicesStateLocked(
    /* [in] */ UserState* userState)
{
    HashMap<AutoPtr<IComponentName>, AutoPtr<IIAccessibilityServiceConnection> >
            componentNameToServiceMap = userState->mComponentNameToServiceMap;
    Boolean isEnabled = userState->mIsAccessibilityEnabled;

    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    AutoPtr<ILooper> looper;
    mMainHandler->GetLooper((ILooper**)&looper);

    String id;
    Int32 enabledInstalledServices = 0;
    CAccessibilityServiceConnection* sc;
    List<AutoPtr<IAccessibilityServiceInfo> >::Iterator it = userState->mInstalledServices.Begin();
    for (; it != userState->mInstalledServices.End(); ++it) {
        AutoPtr<IAccessibilityServiceInfo> installedService = *it;
        installedService->GetId(&id);
        AutoPtr<IComponentName> componentName;
        helper->UnflattenFromString(id, (IComponentName**)&componentName);
        HashMap<AutoPtr<IComponentName>, AutoPtr<IIAccessibilityServiceConnection> >::Iterator
            it = componentNameToServiceMap.Find(componentName);
        AutoPtr<IIAccessibilityServiceConnection> service;
        if (it != componentNameToServiceMap.End()) {
            service = it->mSecond;
        }

        if (isEnabled) {
            if (userState->mEnabledServices.Find(componentName)
                    != userState->mEnabledServices.End()) {
                if (service == NULL) {
                    CAccessibilityServiceConnection::New(looper, userState->mUserId,
                        componentName, installedService, FALSE, THIS_PROBE(IIAccessibilityManager),
                        (IIAccessibilityServiceConnection**)&service);
                }
                sc = (CAccessibilityServiceConnection*)service.Get();
                sc->Bind();
                enabledInstalledServices++;
            }
            else {
                if (service != NULL) {
                    sc = (CAccessibilityServiceConnection*)service.Get();
                    sc->Unbind();
                }
            }
        }
        else {
            if (service != NULL) {
                sc = (CAccessibilityServiceConnection*)service.Get();
                sc->Unbind();
            }
        }
    }

    return enabledInstalledServices;
}

void CAccessibilityManagerService::ScheduleSendStateToClientsLocked(
    /* [in] */ UserState* userState)
{
    Int32 count1, count2;
    mGlobalClients->GetRegisteredCallbackCount(&count1);
    if (count1 > 0
            || (userState->mClients->GetRegisteredCallbackCount(&count2), count2 > 0)) {
        Int32 clientState = GetClientState(userState);
        AutoPtr<IMessage> msg;
        mMainHandler->ObtainMessage(MainHandler::MSG_SEND_STATE_TO_CLIENTS,
            clientState, userState->mUserId, (IMessage**)&msg);
        Boolean result;
        mMainHandler->SendMessage(msg, &result);
    }
}

void CAccessibilityManagerService::UpdateInputFilterLocked(
    /* [in] */ UserState* userState)
{
    Boolean setInputFilter = FALSE;
    AutoPtr<CAccessibilityInputFilter> inputFilter;

    {
        AutoLock lock(mLock);
        if ((userState->mIsAccessibilityEnabled && userState->mIsTouchExplorationEnabled)
                || userState->mIsDisplayMagnificationEnabled) {
            if (!mHasInputFilter) {
                mHasInputFilter = TRUE;
                if (mInputFilter == NULL) {
                    CAccessibilityInputFilter::NewByFriend(mContext,
                        THIS_PROBE(IIAccessibilityManager),
                        (CAccessibilityInputFilter**)&mInputFilter);
                }
                inputFilter = mInputFilter;
                setInputFilter = TRUE;
            }
            Int32 flags = 0;
            if (userState->mIsDisplayMagnificationEnabled) {
                flags |= CAccessibilityInputFilter::FLAG_FEATURE_SCREEN_MAGNIFIER;
            }
            if (userState->mIsTouchExplorationEnabled) {
                flags |= CAccessibilityInputFilter::FLAG_FEATURE_TOUCH_EXPLORATION;
            }
            mInputFilter->SetEnabledFeatures(flags);
        }
        else {
            if (mHasInputFilter) {
                mHasInputFilter = FALSE;
                mInputFilter->SetEnabledFeatures(0);
                inputFilter = NULL;
                setInputFilter = TRUE;
            }
        }
    }

    if (setInputFilter) {
        // try {
        mWindowManagerService->SetInputFilter(inputFilter);
        // } catch (RemoteException re) {
        //     /* ignore */
        // }
    }
}

//=======================================================================
// CAccessibilityManagerService::OnDialogOKListener
//=======================================================================
CAR_INTERFACE_IMPL(CAccessibilityManagerService::OnDialogOKListener, IDialogInterfaceOnClickListener);

CAccessibilityManagerService::OnDialogOKListener::OnDialogOKListener(
    /* [in] */ UserState* state,
    /* [in] */ IIAccessibilityServiceConnection* service,
    /* [in] */ CAccessibilityManagerService* host)
    : mState(state)
    , mService(service)
    , mHost(host)
{}

ECode CAccessibilityManagerService::OnDialogOKListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    CAccessibilityServiceConnection* sc = (CAccessibilityServiceConnection*)mService.Get();
    // The user allowed the service to toggle touch exploration.
    mState->mTouchExplorationGrantedServices.Insert(sc->mComponentName);
    mHost->PersistComponentNamesToSettingLocked(
        ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
        &mState->mTouchExplorationGrantedServices, mState->mUserId);
    // Enable touch exploration.
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    Boolean result;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    return settingsSecure->PutInt32ForUser(resolver,
        ISettingsSecure::TOUCH_EXPLORATION_ENABLED, 1, sc->mUserId, &result);
}

//=======================================================================
// CAccessibilityManagerService::OnDialogCancelListener
//=======================================================================
CAR_INTERFACE_IMPL(CAccessibilityManagerService::OnDialogCancelListener, IDialogInterfaceOnClickListener);

ECode CAccessibilityManagerService::OnDialogCancelListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return dialog->Dismiss();
}

void CAccessibilityManagerService::ShowEnableTouchExplorationDialog(
    /* [in] */ IIAccessibilityServiceConnection* service)
{
    CAccessibilityServiceConnection* sc = (CAccessibilityServiceConnection*)service;
    AutoPtr<IPackageManager> packageManager;
    mContext->GetPackageManager((IPackageManager**)&packageManager);
    AutoPtr<ICharSequence> csq;
    sc->mResolveInfo->LoadLabel(packageManager, (ICharSequence**)&csq);
    String label;
    csq->ToString(&label);
    AutoLock lock(mLock);
    AutoPtr<UserState> state = GetCurrentUserStateLocked();
    if (state->mIsTouchExplorationEnabled) {
        return;
    }
    Boolean isShowing;
    if (mEnableTouchExplorationDialog != NULL
        && (mEnableTouchExplorationDialog->IsShowing(&isShowing), isShowing)) {
        return;
    }
    AutoPtr<IAlertDialogBuilder> dialogBuilder;
    CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&dialogBuilder);
    dialogBuilder->SetIcon(R::drawable::ic_dialog_alert);
    AutoPtr<IDialogInterfaceOnClickListener> listener_ok = new OnDialogOKListener(state, service, this);
    dialogBuilder->SetPositiveButton(R::string::ok, listener_ok);
    AutoPtr<IDialogInterfaceOnClickListener> listener_cancel = new OnDialogCancelListener();
    dialogBuilder->SetNegativeButton(R::string::cancel, listener_cancel);
    dialogBuilder->SetTitle(R::string::enable_explore_by_touch_warning_title);
    AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, (IInterface*)csq.Get());
    String str;
    mContext->GetString(R::string::enable_explore_by_touch_warning_message, args, &str);
    AutoPtr<ICharSequence> seq;
    CString::New(str, (ICharSequence**)&seq);
    dialogBuilder->SetMessage(seq);
    mEnableTouchExplorationDialog = NULL;
    dialogBuilder->Create((IAlertDialog**)&mEnableTouchExplorationDialog);

    AutoPtr<IWindow> win;
    mEnableTouchExplorationDialog->GetWindow((IWindow**)&win);
    win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
    AutoPtr<IWindowManagerLayoutParams> attrs;
    win->GetAttributes((IWindowManagerLayoutParams**)&attrs);
    Int32 privateFlags;
    attrs->GetPrivateFlags(&privateFlags);
    privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS;
    attrs->SetPrivateFlags(privateFlags);
    mEnableTouchExplorationDialog->SetCanceledOnTouchOutside(TRUE);
    mEnableTouchExplorationDialog->Show();
}

Int32 CAccessibilityManagerService::GetClientState(
    /* [in] */ UserState* userState)
{
    Int32 clientState = 0;
    if (userState->mIsAccessibilityEnabled) {
        clientState |= IAccessibilityManager::STATE_FLAG_ACCESSIBILITY_ENABLED;
    }
    // Touch exploration relies on enabled accessibility.
    if (userState->mIsAccessibilityEnabled && userState->mIsTouchExplorationEnabled) {
        clientState |= IAccessibilityManager::STATE_FLAG_TOUCH_EXPLORATION_ENABLED;
    }
    return clientState;
}

void CAccessibilityManagerService::RecreateInternalStateLocked(
    /* [in] */ UserState* userState)
{
    PopulateInstalledAccessibilityServiceLocked(userState);
    PopulateEnabledAccessibilityServicesLocked(userState);
    PopulateTouchExplorationGrantedAccessibilityServicesLocked(userState);

    HandleTouchExplorationEnabledSettingChangedLocked(userState);
    HandleDisplayMagnificationEnabledSettingChangedLocked(userState);
    HandleAccessibilityEnabledSettingChangedLocked(userState);

    PerformServiceManagementLocked(userState);
    UpdateInputFilterLocked(userState);
    ScheduleSendStateToClientsLocked(userState);
}

void CAccessibilityManagerService::HandleAccessibilityEnabledSettingChangedLocked(
    /* [in] */ UserState* userState)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetInt32ForUser(resolver, ISettingsSecure::ACCESSIBILITY_ENABLED,
           0, userState->mUserId, &value);
    userState->mIsAccessibilityEnabled = (value == 1);
}

void CAccessibilityManagerService::PerformServiceManagementLocked(
    /* [in] */ UserState* userState)
{
    if (userState->mIsAccessibilityEnabled) {
        ManageServicesLocked(userState);
    }
    else {
        UnbindAllServicesLocked(userState);
    }
}

void CAccessibilityManagerService::HandleTouchExplorationEnabledSettingChangedLocked(
    /* [in] */ UserState* userState)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetInt32ForUser(resolver, ISettingsSecure::TOUCH_EXPLORATION_ENABLED,
        0, userState->mUserId, &value);
    userState->mIsTouchExplorationEnabled = value == 1;
}

void CAccessibilityManagerService::HandleDisplayMagnificationEnabledSettingChangedLocked(
    /* [in] */ UserState* userState)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetInt32ForUser(resolver,
        ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED,
        0, userState->mUserId, &value);
    userState->mIsDisplayMagnificationEnabled = value == 1;
}

void CAccessibilityManagerService::HandleTouchExplorationGrantedAccessibilityServicesChangedLocked(
    /* [in] */ UserState* userState)
{
    CAccessibilityServiceConnection* sc;
    List<AutoPtr<IIAccessibilityServiceConnection> >::Iterator it = userState->mServices.Begin();
    for (; it != userState->mServices.End(); ++it) {
        sc = (CAccessibilityServiceConnection*)(*it).Get();
        if (sc->mRequestTouchExplorationMode
            && (userState->mTouchExplorationGrantedServices.Find(sc->mComponentName)
                != userState->mTouchExplorationGrantedServices.End())) {
            TryEnableTouchExplorationLocked(*it);
            return;
        }
    }

    if (userState->mIsTouchExplorationEnabled) {
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        Boolean res;
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        settingsSecure->PutInt32ForUser(resolver,
            ISettingsSecure::TOUCH_EXPLORATION_ENABLED, 0, userState->mUserId, &res);
    }
}

void CAccessibilityManagerService::TryEnableTouchExplorationLocked(
    /* [in] */ IIAccessibilityServiceConnection* service)
{
    CAccessibilityServiceConnection* sc = (CAccessibilityServiceConnection*)service;
    AutoPtr<UserState> userState = GetUserStateLocked(sc->mUserId);
    if (!userState->mIsTouchExplorationEnabled && sc->mRequestTouchExplorationMode
            && sc->CanReceiveEvents()) {
        HashSet<AutoPtr<IComponentName> >::Iterator it;
        it = Find(userState->mTouchExplorationGrantedServices.Begin(),
            userState->mTouchExplorationGrantedServices.End(),
            sc->mComponentName);
        Boolean canToggleTouchExploration = (it != userState->mTouchExplorationGrantedServices.End());
        if (!sc->mIsAutomation && !canToggleTouchExploration) {
            ShowEnableTouchExplorationDialog(service);
        }
        else {
            AutoPtr<IContentResolver> resolver;
            mContext->GetContentResolver((IContentResolver**)&resolver);
            Boolean result;
            AutoPtr<ISettingsSecure> settingsSecure;
            CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
            settingsSecure->PutInt32ForUser(resolver,
                    ISettingsSecure::TOUCH_EXPLORATION_ENABLED, 1,
                    userState->mUserId, &result);
        }
    }
}

void CAccessibilityManagerService::TryDisableTouchExplorationLocked(
    /* [in] */ IIAccessibilityServiceConnection* service)
{
    CAccessibilityServiceConnection* sc = (CAccessibilityServiceConnection*)service;
    AutoPtr<UserState> userState = GetUserStateLocked(sc->mUserId);
    if (userState->mIsTouchExplorationEnabled) {
        CAccessibilityServiceConnection* other;
        List<AutoPtr<IIAccessibilityServiceConnection> >::Iterator it = userState->mServices.Begin();
        for (; it != userState->mServices.End(); ++it) {
            other = (CAccessibilityServiceConnection*)(*it).Get();
            if (other != sc && other->mRequestTouchExplorationMode) {
                return;
            }
        }

        Boolean result;
        AutoPtr<IContentResolver> resolver;
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        settingsSecure->PutInt32ForUser(resolver,
            ISettingsSecure::TOUCH_EXPLORATION_ENABLED, 0, userState->mUserId, &result);
    }
}

void CAccessibilityManagerService::HandleSendAccessibilityEventToInputFilter(
    /* [in] */ IAccessibilityEvent* event)
{
    {
        AutoLock lock(mLock);
        if (mHasInputFilter && mInputFilter != NULL) {
            mInputFilter->NotifyAccessibilityEvent(event);
        }
    }
    event->Recycle();
}

void CAccessibilityManagerService::HandleSendStateToClients(
    /* [in] */ Int32 clientState,
    /* [in] */ Int32 userId)
{
    SendStateToClients(clientState, mGlobalClients);
    SendStateToClientsForUser(clientState, userId);
}

void CAccessibilityManagerService::HandleSendClearedStateToClientsForUser(
    /* [in] */ Int32 userId)
{
    SendStateToClientsForUser(0, userId);
}

void CAccessibilityManagerService::HandleSendRecreateInternalState(
    /* [in] */ Int32 userId)
{
    AutoLock lock(mLock);
    AutoPtr<UserState> userState = GetUserStateLocked(userId);
    RecreateInternalStateLocked(userState);
}

void CAccessibilityManagerService::HandleUpdateActiveWindow(
    /* [in] */ Int32 windowId,
    /* [in] */ Int32 eventType)
{
    mSecurityPolicy->UpdateActiveWindow(windowId, eventType);
}

void CAccessibilityManagerService::HandleAnnounceNewUserIfNeeded()
{
    AnnounceNewUserIfNeeded();
}

void CAccessibilityManagerService::AnnounceNewUserIfNeeded()
{
    AutoLock lock(mLock);
    AutoPtr<UserState> userState = GetCurrentUserStateLocked();
    if (userState->mIsAccessibilityEnabled) {
        AutoPtr<IUserManager> userManager;
        ASSERT_SUCCEEDED(mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&userManager));
        AutoPtr<IUserInfo> userInfo;
        userManager->GetUserInfo(mCurrentUserId, (IUserInfo**)&userInfo);
        String name;
        userInfo->GetName(&name);
        AutoPtr<ICharSequence> csq;
        CString::New(name, (ICharSequence**)&csq);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, csq.Get());
        String message;
        mContext->GetString(R::string::user_switched, args, &message);
        AutoPtr<IAccessibilityEventHelper> helper;
        CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
        AutoPtr<IAccessibilityEvent> event;
        helper->Obtain(IAccessibilityEvent::TYPE_ANNOUNCEMENT, (IAccessibilityEvent**)&event);
        AutoPtr<IObjectContainer> texts;
        event->GetText((IObjectContainer**)&texts);
        AutoPtr<ICharSequence> messageCsq;
        CString::New(message, (ICharSequence**)&messageCsq);
        texts->Add(messageCsq);
        event->SetWindowId(mSecurityPolicy->GetRetrievalAllowingWindowLocked());
        Boolean result;
        SendAccessibilityEvent(event, mCurrentUserId, &result);
    }
}

void CAccessibilityManagerService::SendStateToClientsForUser(
    /* [in] */ Int32 clientState,
    /* [in] */ Int32 userId)
{
    AutoPtr<UserState> userState;
    {
        AutoLock lock(mLock);
        userState = GetUserStateLocked(userId);
    }
    SendStateToClients(clientState, userState->mClients);
}

void CAccessibilityManagerService::SendStateToClients(
    /* [in] */ Int32 clientState,
    /* [in] */ IRemoteCallbackList* clients)
{
    // try {
    Int32 userClientCount;
    clients->BeginBroadcast(&userClientCount);
    for (Int32 i = 0; i < userClientCount; i++) {
        AutoPtr<IIAccessibilityManagerClient> client;
        clients->GetBroadcastItem(i, (IInterface**)&client);
        // try {
        client->SetState(clientState);
        // } catch (RemoteException re) {
        //     /* ignore */
        // }
    }
    clients->FinishBroadcast();
    // } finally {
    //     clients.finishBroadcast();
    // }
}


} // Accessibility
} // Server
} // Droid
} // Elastos
