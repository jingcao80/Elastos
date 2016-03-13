
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Hardware.h>
#include <Elastos.Droid.Provider.h>
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/server/accessibility/AccessibilityManagerService.h"
#include "elastos/droid/server/accessibility/DisplayAdjustmentUtils.h"
#include "elastos/droid/server/LocalServices.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::AccessibilityService::EIID_IIAccessibilityServiceConnection;
using Elastos::Droid::AccessibilityService::CAccessibilityServiceInfo;
using Elastos::Droid::AccessibilityService::IAccessibilityService;
using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfoHelper;
using Elastos::Droid::AccessibilityService::CAccessibilityServiceInfoHelper;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::EIID_IServiceConnection;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CRegion;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IInputManagerHelper;
using Elastos::Droid::Hardware::Input::CInputManagerHelper;
using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Internal::StatusBar::IIStatusBarService;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::CRemoteCallbackList;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Server::LocalServices;
using Elastos::Droid::Text::CSimpleStringSplitter;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::EIID_IIAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::IAccessibilityWindowInfoHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityWindowInfoHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityInteractionClientHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityInteractionClientHelper;
using Elastos::Droid::View::EIID_IWindowsForAccessibilityCallback;
using Elastos::Droid::View::EIID_IWindowManagerInternal;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::IWindowManagerPolicy;
using Elastos::Droid::View::IWindowManagerLayoutParams;
using Elastos::Droid::View::IInputDevice;
using Elastos::Core::CoreUtils;
using Elastos::Core::IAppendable;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

//===============================================================================
//                  PendingEvent
//===============================================================================

PendingEvent::PendingEvent()
    : mPolicyFlags(0)
    , mSequence(0)
    , mHandled(FALSE)
{}

PendingEvent::~PendingEvent()
{}

void PendingEvent::Clear()
{
    if (mEvent != NULL) {
        IAccessibilityRecord::Probe(mEvent)->Recycle();
        mEvent = NULL;
    }
    mNext = NULL;
    mPolicyFlags = 0;
    mSequence = 0;
    mHandled = FALSE;
}

const String AccessibilityManagerService::TAG("AccessibilityManagerService");
const Boolean AccessibilityManagerService::DEBUG = FALSE;
const Int32 AccessibilityManagerService::WAIT_FOR_USER_STATE_FULLY_INITIALIZED_MILLIS = 3000;
const Int32 AccessibilityManagerService::WAIT_WINDOWS_TIMEOUT_MILLIS = 5000;
const String AccessibilityManagerService::FUNCTION_REGISTER_UI_TEST_AUTOMATION_SERVICE("registerUiTestAutomationService");
const String AccessibilityManagerService::TEMPORARY_ENABLE_ACCESSIBILITY_UNTIL_KEYGUARD_REMOVED("temporaryEnableAccessibilityStateUntilKeyguardRemoved");
const String AccessibilityManagerService::GET_WINDOW_TOKEN("getWindowToken");

static AutoPtr<IComponentName> InitsFakeAccessibilityServiceComponentName()
{
    AutoPtr<IComponentName> name;
    CComponentName::New(String("foo.bar"), String("FakeService"), (IComponentName**)&name);
    return name;
}

AutoPtr<IComponentName> AccessibilityManagerService::sFakeAccessibilityServiceComponentName =
        InitsFakeAccessibilityServiceComponentName();
const String AccessibilityManagerService::FUNCTION_DUMP("dump");
const Char32 AccessibilityManagerService::COMPONENT_NAME_SEPARATOR(':');
const Int32 AccessibilityManagerService::OWN_PROCESS_ID = Process::MyPid();

const Int32 AccessibilityManagerService::MAX_POOL_SIZE = 10;
const Int32 AccessibilityManagerService::WINDOW_ID_UNKNOWN = -1;

Int32 AccessibilityManagerService::sIdCounter = 0;
Int32 AccessibilityManagerService::sNextWindowId = 0;


const Int32 AccessibilityManagerService::SecurityPolicy::INVALID_WINDOW_ID = -1;

const Int32 AccessibilityManagerService::SecurityPolicy::RETRIEVAL_ALLOWING_EVENT_TYPES =
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
        | IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED
        | IAccessibilityEvent::TYPE_VIEW_TEXT_TRAVERSED_AT_MOVEMENT_GRANULARITY;


const Int32 AccessibilityManagerService::InvocationHandler::MSG_ON_GESTURE;
const Int32 AccessibilityManagerService::InvocationHandler::MSG_ON_KEY_EVENT;
const Int32 AccessibilityManagerService::InvocationHandler::MSG_CLEAR_ACCESSIBILITY_CACHE;
const Int32 AccessibilityManagerService::InvocationHandler::MSG_ON_KEY_EVENT_TIMEOUT;

const Int64 AccessibilityManagerService::KeyEventDispatcher::ON_KEY_EVENT_TIMEOUT_MILLIS = 500;

const Int32 AccessibilityManagerService::MainHandler::MSG_SEND_ACCESSIBILITY_EVENT_TO_INPUT_FILTER;
const Int32 AccessibilityManagerService::MainHandler::MSG_SEND_STATE_TO_CLIENTS;
const Int32 AccessibilityManagerService::MainHandler::MSG_SEND_CLEARED_STATE_TO_CLIENTS_FOR_USER;
const Int32 AccessibilityManagerService::MainHandler::MSG_ANNOUNCE_NEW_USER_IF_NEEDED;
const Int32 AccessibilityManagerService::MainHandler::MSG_UPDATE_INPUT_FILTER;
const Int32 AccessibilityManagerService::MainHandler::MSG_SHOW_ENABLED_TOUCH_EXPLORATION_DIALOG;
const Int32 AccessibilityManagerService::MainHandler::MSG_SEND_KEY_EVENT_TO_INPUT_FILTER;
const Int32 AccessibilityManagerService::MainHandler::MSG_CLEAR_ACCESSIBILITY_FOCUS;

//===============================================================================
//                  AccessibilityManagerService::Service
//===============================================================================

CAR_INTERFACE_IMPL_4(AccessibilityManagerService::Service, Object, IIAccessibilityServiceConnection,
        IBinder, IServiceConnection, IProxyDeathRecipient);

AccessibilityManagerService::Service::Service()
    : mUserId(0)
    , mId(0)
    , mEventTypes(0)
    , mFeedbackType(0)
    , mIsDefault(FALSE)
    , mRequestTouchExplorationMode(FALSE)
    , mRequestEnhancedWebAccessibility(FALSE)
    , mRequestFilterKeyEvents(FALSE)
    , mRetrieveInteractiveWindows(FALSE)
    , mFetchFlags(0)
    , mNotificationTimeout(0)
    , mIsAutomation(FALSE)
    , mWasConnectedAndDied(FALSE)
{
}

AccessibilityManagerService::Service::~Service()
{}

ECode AccessibilityManagerService::Service::constructor(
    /* [in] */ Int32 userId,
    /* [in] */ IComponentName* componentName,
    /* [in] */ IAccessibilityServiceInfo* accessibilityServiceInfo,
    /* [in] */ IIAccessibilityManager* host)
{
    mHost = (AccessibilityManagerService*)host;

    CSparseArray::New((ISparseArray**)&mPendingEvents);
    mKeyEventDispatcher = new KeyEventDispatcher(this, mHost);
    AutoPtr<ILooper> looper;
    mHost->mMainHandler->GetLooper((ILooper**)&looper);
    mEventDispatchHandler = new EventDispatchHandler(looper, this);
    mInvocationHandler = new InvocationHandler(looper, this);
    /*  */

    mUserId = userId;
    accessibilityServiceInfo->GetResolveInfo((IResolveInfo**)&mResolveInfo);
    mId = sIdCounter++;
    mComponentName = componentName;
    mAccessibilityServiceInfo = accessibilityServiceInfo;
    IObject::Probe(sFakeAccessibilityServiceComponentName)->Equals(componentName,
            &mIsAutomation);
    if (!mIsAutomation) {
        ASSERT_SUCCEEDED(CIntent::New((IIntent**)&mIntent));
        mIntent->SetComponent(mComponentName);
        mIntent->PutExtra(IIntent::EXTRA_CLIENT_LABEL,
                R::string::accessibility_binding_label);
        AutoPtr<IIntent> i;
        CIntent::New(ISettings::ACTION_ACCESSIBILITY_SETTINGS, (IIntent**)&i);
        AutoPtr<IPendingIntentHelper> helper;
        ASSERT_SUCCEEDED(CPendingIntentHelper::AcquireSingleton(
                (IPendingIntentHelper**)&helper));
        AutoPtr<IPendingIntent> pendingI;
        helper->GetActivity(mHost->mContext, 0, i, 0, (IPendingIntent**)&pendingI);
        ASSERT_SUCCEEDED(mIntent->PutExtra(IIntent::EXTRA_CLIENT_INTENT,
                IParcelable::Probe(pendingI)));
    }
    SetDynamicallyConfigurableProperties(accessibilityServiceInfo);

    return NOERROR;
}

void AccessibilityManagerService::Service::SetDynamicallyConfigurableProperties(
    /* [in] */ IAccessibilityServiceInfo* info)
{
    info->GetEventTypes(&mEventTypes);
    info->GetFeedbackType(&mFeedbackType);
    AutoPtr< ArrayOf<String> > packageNames;
    info->GetPackageNames((ArrayOf<String>**)&packageNames);
    if (packageNames != NULL) {
        AutoPtr<IList> list;
        Arrays::AsList(packageNames, (IList**)&list);
        mPackageNames->AddAll(ICollection::Probe(list));
    }
    info->GetNotificationTimeout(&mNotificationTimeout);
    Int32 flags;
    info->GetFlags(&flags);
    mIsDefault = (flags & IAccessibilityServiceInfo::DEFAULT) != 0;

    AutoPtr<IResolveInfo> resolveInfo;
    ASSERT_SUCCEEDED(info->GetResolveInfo((IResolveInfo**)&resolveInfo));
    AutoPtr<IServiceInfo> serviceInfo;
    ASSERT_SUCCEEDED(resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo));
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 version;
    appInfo->GetTargetSdkVersion(&version);

    if (mIsAutomation || version >= Build::VERSION_CODES::JELLY_BEAN) {
        if ((flags & IAccessibilityServiceInfo::FLAG_INCLUDE_NOT_IMPORTANT_VIEWS) != 0) {
            mFetchFlags |= IAccessibilityNodeInfo::FLAG_INCLUDE_NOT_IMPORTANT_VIEWS;
        }
        else {
            mFetchFlags &= ~IAccessibilityNodeInfo::FLAG_INCLUDE_NOT_IMPORTANT_VIEWS;
        }
    }

    if ((flags & IAccessibilityServiceInfo::FLAG_REPORT_VIEW_IDS) != 0) {
        mFetchFlags |= IAccessibilityNodeInfo::FLAG_REPORT_VIEW_IDS;
    }
    else {
        mFetchFlags &= ~IAccessibilityNodeInfo::FLAG_REPORT_VIEW_IDS;
    }

    mRequestTouchExplorationMode = (flags
            & IAccessibilityServiceInfo::FLAG_REQUEST_TOUCH_EXPLORATION_MODE) != 0;
    mRequestEnhancedWebAccessibility = (flags
            & IAccessibilityServiceInfo::FLAG_REQUEST_ENHANCED_WEB_ACCESSIBILITY) != 0;
    mRequestFilterKeyEvents = (flags
            & IAccessibilityServiceInfo::FLAG_REQUEST_FILTER_KEY_EVENTS) != 0;
    mRetrieveInteractiveWindows = (flags
            & IAccessibilityServiceInfo::FLAG_RETRIEVE_INTERACTIVE_WINDOWS) != 0;
}

Boolean AccessibilityManagerService::Service::BindLocked()
{
    AutoPtr<UserState> userState = mHost->GetUserStateLocked(mUserId);
    if (!mIsAutomation) {
        AutoPtr<IUserHandle> handle;
        CUserHandle::New(mUserId, (IUserHandle**)&handle);
        Boolean res;
        if (mService == NULL &&
                (mHost->mContext->BindServiceAsUser(
                mIntent, (IServiceConnection*)this, IContext::BIND_AUTO_CREATE,
                handle, &res), res)) {
            userState->mBindingServices->Add(mComponentName);
        }
    }
    else {
        userState->mBindingServices->Add(mComponentName);
        mService = IBinder::Probe(userState->mUiAutomationServiceClient);
        AutoPtr<IRunnable> runnable = new BindLockedRunnable(this);
        Boolean res;
        mHost->mMainHandler->Post(runnable, &res);
        userState->mUiAutomationService = this;
    }

    return FALSE;
}

Boolean AccessibilityManagerService::Service::UnbindLocked()
{
    if (mService == NULL) {
        return FALSE;
    }
    AutoPtr<UserState> userState = mHost->GetUserStateLocked(mUserId);
    mKeyEventDispatcher->Flush();

    if (!mIsAutomation) {
        mHost->mContext->UnbindService(IServiceConnection::Probe(this));
    }
    else {
        userState->DestroyUiAutomationService();
    }
    mHost->RemoveServiceLocked(this, userState);
    ResetLocked();
    return TRUE;
}

Boolean AccessibilityManagerService::Service::CanReceiveEventsLocked()
{
    return (mEventTypes != 0 && mFeedbackType != 0 && mService != NULL);
}

ECode AccessibilityManagerService::Service::SetOnKeyEventResult(
    /* [in] */ Boolean handled,
    /* [in] */ Int32 sequence)
{
    mKeyEventDispatcher->SetOnKeyEventResult(handled, sequence);
    return NOERROR;
}

ECode AccessibilityManagerService::Service::GetServiceInfo(
    /* [out] */ IAccessibilityServiceInfo** serviceInfo)
{
    VALIDATE_NOT_NULL(serviceInfo);

    synchronized(mHost->mLock) {
        *serviceInfo = mAccessibilityServiceInfo;
        REFCOUNT_ADD(*serviceInfo);
        return NOERROR;
    }
    return NOERROR;
}

Boolean AccessibilityManagerService::Service::CanRetrieveInteractiveWindowsLocked()
{
    return mHost->mSecurityPolicy->CanRetrieveWindowContentLocked(this)
            && mRetrieveInteractiveWindows;
}

ECode AccessibilityManagerService::Service::SetServiceInfo(
    /* [in] */ IAccessibilityServiceInfo* info)
{
    Int32 identity = Binder::ClearCallingIdentity();
    // try {
    synchronized(mHost->mLock) {
        // If the XML manifest had data to configure the service its info
        // should be already set. In such a case update only the dynamically
        // configurable properties.
        AutoPtr<IAccessibilityServiceInfo> oldInfo = mAccessibilityServiceInfo;
        if (oldInfo != NULL) {
            oldInfo->UpdateDynamicallyConfigurableProperties(info);
            SetDynamicallyConfigurableProperties(oldInfo);
        }
        else {
            SetDynamicallyConfigurableProperties(info);
        }
        AutoPtr<UserState> userState = mHost->GetUserStateLocked(mUserId);
        mHost->OnUserStateChangedLocked(userState);
    }
    Binder::RestoreCallingIdentity(identity);
    // } finally {
    //     Binder.restoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode AccessibilityManagerService::Service::OnServiceConnected(
    /* [in] */ IComponentName* componentName,
    /* [in] */ IBinder* service)
{
    synchronized(mHost->mLock) {
        mService = service;
        mServiceInterface = IIAccessibilityServiceClient::Probe(service);
        AutoPtr<UserState> userState = mHost->GetUserStateLocked(mUserId);
        mHost->AddServiceLocked(this, userState);
        Boolean res;
        if ((userState->mBindingServices->Contains(mComponentName, &res), res)
                || mWasConnectedAndDied) {
            userState->mBindingServices->Remove(mComponentName);
            mWasConnectedAndDied = FALSE;
            // try {
            ECode ec = mServiceInterface->SetConnection(this, mId);
            mHost->OnUserStateChangedLocked(userState);
            if (FAILED(ec)) {
                Slogger::W("AccessibilityManagerService::Service",
                        "Error while setting connection for service: %p 0x%08x",
                        service, ec);
                ProxyDied();
                return ec;
            }
            // } catch (RemoteException re) {
            //     Slog.w(TAG, "Error while setting connection for service: "
            //             + service, re);
            //     binderDied();
            // }
        }
        else {
            ProxyDied();
        }
    }
    return NOERROR;
}

ECode AccessibilityManagerService::Service::GetWindows(
    /* [out] */ IList** windows)
{
    VALIDATE_NOT_NULL(windows);
    *windows = NULL;

    mHost->EnsureWindowsAvailableTimed();
    synchronized(mHost->mLock) {
        // We treat calls from a profile as if made by its perent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mHost->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                IUserHandle::USER_CURRENT, &resolvedUserId);
        if (resolvedUserId != mHost->mCurrentUserId) {
            return NOERROR;
        }
        Boolean permissionGranted =
                mHost->mSecurityPolicy->CanRetrieveWindowsLocked(this);
        if (!permissionGranted) {
            return NOERROR;
        }
        AutoPtr<IList> _windows;
        CArrayList::New((IArrayList**)&_windows);
        Int32 windowCount;
        mHost->mSecurityPolicy->mWindows->GetSize(&windowCount);
        for (Int32 i = 0; i < windowCount; i++) {
            AutoPtr<IInterface> obj;
            mHost->mSecurityPolicy->mWindows->Get(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityWindowInfo> window = IAccessibilityWindowInfo::Probe(obj);
            AutoPtr<IAccessibilityWindowInfoHelper> helper;
            CAccessibilityWindowInfoHelper::AcquireSingleton(
                    (IAccessibilityWindowInfoHelper**)&helper);
            AutoPtr<IAccessibilityWindowInfo> windowClone;
            helper->Obtain(window, (IAccessibilityWindowInfo**)&windowClone);
            windowClone->SetConnectionId(mId);
            _windows->Add(windowClone);
        }
        *windows = _windows;
        REFCOUNT_ADD(*windows);
        return NOERROR;
    }
    return NOERROR;
}

ECode AccessibilityManagerService::Service::GetWindow(
    /* [in] */ Int32 windowId,
    /* [out] */ IAccessibilityWindowInfo** window)
{
    VALIDATE_NOT_NULL(window);
    *window = NULL;

    mHost->EnsureWindowsAvailableTimed();
    synchronized(mHost->mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mHost->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                IUserHandle::USER_CURRENT, &resolvedUserId);
        if (resolvedUserId != mHost->mCurrentUserId) {
            return NOERROR;
        }
        Boolean permissionGranted =
                mHost->mSecurityPolicy->CanRetrieveWindowsLocked(this);
        if (!permissionGranted) {
            return NOERROR;
        }
        AutoPtr<IAccessibilityWindowInfo> _window =
                mHost->mSecurityPolicy->FindWindowById(windowId);
        if (_window != NULL) {
            AutoPtr<IAccessibilityWindowInfoHelper> helper;
            CAccessibilityWindowInfoHelper::AcquireSingleton(
                    (IAccessibilityWindowInfoHelper**)&helper);
            AutoPtr<IAccessibilityWindowInfo> windowClone;
            helper->Obtain(_window, (IAccessibilityWindowInfo**)&windowClone);
            windowClone->SetConnectionId(mId);
            *window = windowClone;
            REFCOUNT_ADD(*window);
            return NOERROR;
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode AccessibilityManagerService::Service::FindAccessibilityNodeInfosByViewId(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ const String& viewIdResName,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 resolvedWindowId;
    AutoPtr<IIAccessibilityInteractionConnection> connection;
    AutoPtr<IRegion> partialInteractiveRegion = mHost->mTempRegion;
    synchronized(mHost->mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mHost->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                IUserHandle::USER_CURRENT, &resolvedUserId);
        if (resolvedUserId != mHost->mCurrentUserId) {
            *result = FALSE;
            return NOERROR;
        }
        resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
        Boolean permissionGranted =
                mHost->mSecurityPolicy->CanGetAccessibilityNodeInfoLocked(this,
                    resolvedWindowId);
        if (!permissionGranted) {
            *result = FALSE;
            return NOERROR;
        }
        else {
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *result = FALSE;
                return NOERROR;
            }
        }
        if (!mHost->mSecurityPolicy->ComputePartialInteractiveRegionForWindowLocked(
                resolvedWindowId, partialInteractiveRegion)) {
            partialInteractiveRegion = NULL;
        }
    }

    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    AutoPtr<IMagnificationSpec> spec = mHost->GetCompatibleMagnificationSpecLocked(
        resolvedWindowId);
    // try {
    ECode ec = connection->FindAccessibilityNodeInfosByViewId(
            accessibilityNodeId, viewIdResName, partialInteractiveRegion,
            interactionId, callback, mFetchFlags, interrogatingPid,
            interrogatingTid, spec);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("AccessibilityManagerService::Service", "Error findAccessibilityNodeInfoByViewId().");
        }
        Binder::RestoreCallingIdentity(identityToken);
        *result = FALSE;
        return ec;
    }

    *result = TRUE;
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("AccessibilityManagerService::Service", "Error findAccessibilityNodeInfoByViewId().");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
}

ECode AccessibilityManagerService::Service::FindAccessibilityNodeInfosByText(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ const String& text,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 resolvedWindowId;
    AutoPtr<IIAccessibilityInteractionConnection> connection;
    AutoPtr<IRegion> partialInteractiveRegion = mHost->mTempRegion;
    synchronized(mHost->mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mHost->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                IUserHandle::USER_CURRENT, &resolvedUserId);
        if (resolvedUserId != mHost->mCurrentUserId) {
            *result = FALSE;
            return NOERROR;
        }
        resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
        Boolean permissionGranted =
                mHost->mSecurityPolicy->CanGetAccessibilityNodeInfoLocked(this,
                    resolvedWindowId);
        if (!permissionGranted) {
            *result = FALSE;
            return NOERROR;
        }
        else {
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *result = FALSE;
                return NOERROR;
            }
        }
        if (!mHost->mSecurityPolicy->ComputePartialInteractiveRegionForWindowLocked(
                resolvedWindowId, partialInteractiveRegion)) {
            partialInteractiveRegion = NULL;
        }
    }

    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    AutoPtr<IMagnificationSpec> spec = mHost->GetCompatibleMagnificationSpecLocked(
            resolvedWindowId);
    // try {
    ECode ec = connection->FindAccessibilityNodeInfosByText(accessibilityNodeId,
            text, partialInteractiveRegion, interactionId, callback, mFetchFlags,
            interrogatingPid, interrogatingTid, spec);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("AccessibilityManagerService::Service", "Error calling findAccessibilityNodeInfosByText()");
        }
        Binder::RestoreCallingIdentity(identityToken);
        *result = FALSE;
        return ec;
    }
    *result = TRUE;
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("AccessibilityManagerService::Service", "Error calling findAccessibilityNodeInfosByText()");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
}

ECode AccessibilityManagerService::Service::FindAccessibilityNodeInfoByAccessibilityId(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 resolvedWindowId;
    AutoPtr<IIAccessibilityInteractionConnection> connection;
    AutoPtr<IRegion> partialInteractiveRegion = mHost->mTempRegion;
    synchronized(mHost->mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mHost->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                IUserHandle::USER_CURRENT, &resolvedUserId);
        if (resolvedUserId != mHost->mCurrentUserId) {
            *result = FALSE;
            return NOERROR;
        }
        resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
        Boolean permissionGranted =
            mHost->mSecurityPolicy->CanGetAccessibilityNodeInfoLocked(this,
                resolvedWindowId);
        if (!permissionGranted) {
            *result = FALSE;
            return NOERROR;
        }
        else {
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *result = FALSE;
                return NOERROR;
            }
        }
        if (!mHost->mSecurityPolicy->ComputePartialInteractiveRegionForWindowLocked(
                resolvedWindowId, partialInteractiveRegion)) {
            partialInteractiveRegion = NULL;
        }
    }
    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    AutoPtr<IMagnificationSpec> spec = mHost->GetCompatibleMagnificationSpecLocked(
            resolvedWindowId);
    // try {
    ECode ec = connection->FindAccessibilityNodeInfoByAccessibilityId(
            accessibilityNodeId, partialInteractiveRegion, interactionId, callback,
            mFetchFlags | flags, interrogatingPid, interrogatingTid, spec);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("AccessibilityManagerService::Service", "Error calling findAccessibilityNodeInfoByAccessibilityId()");
        }
        Binder::RestoreCallingIdentity(identityToken);
        *result = FALSE;
        return ec;
    }
    *result = TRUE;
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("AccessibilityManagerService::Service", "Error calling findAccessibilityNodeInfoByAccessibilityId()");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
}

ECode AccessibilityManagerService::Service::FindFocus(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 focusType,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 resolvedWindowId;
    AutoPtr<IIAccessibilityInteractionConnection> connection;
    AutoPtr<IRegion> partialInteractiveRegion = mHost->mTempRegion;
    synchronized(mHost->mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mHost->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                IUserHandle::USER_CURRENT, &resolvedUserId);
        if (resolvedUserId != mHost->mCurrentUserId) {
            *result = FALSE;
            return NOERROR;
        }
        resolvedWindowId = ResolveAccessibilityWindowIdForFindFocusLocked(
                accessibilityWindowId, focusType);
        Boolean permissionGranted =
            mHost->mSecurityPolicy->CanGetAccessibilityNodeInfoLocked(this,
                resolvedWindowId);
        if (!permissionGranted) {
            *result = FALSE;
            return NOERROR;
        }
        else {
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *result = FALSE;
                return NOERROR;
            }
        }
        if (!mHost->mSecurityPolicy->ComputePartialInteractiveRegionForWindowLocked(
                resolvedWindowId, partialInteractiveRegion)) {
            partialInteractiveRegion = NULL;
        }
    }

    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    AutoPtr<IMagnificationSpec> spec = mHost->GetCompatibleMagnificationSpecLocked(
            resolvedWindowId);
    // try {
    ECode ec = connection->FindFocus(accessibilityNodeId, focusType,
            partialInteractiveRegion, interactionId, callback,
            mFetchFlags, interrogatingPid, interrogatingTid, spec);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("AccessibilityManagerService::Service", "Error calling findFocus()");
        }
        Binder::RestoreCallingIdentity(identityToken);
        *result = FALSE;
        return ec;
    }
    *result = TRUE;
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("AccessibilityManagerService::Service", "Error calling findAccessibilityFocus()");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
}

ECode AccessibilityManagerService::Service::FocusSearch(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 resolvedWindowId;
    AutoPtr<IIAccessibilityInteractionConnection> connection;
    AutoPtr<IRegion> partialInteractiveRegion = mHost->mTempRegion;
    synchronized(mHost->mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mHost->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                IUserHandle::USER_CURRENT, &resolvedUserId);
        if (resolvedUserId != mHost->mCurrentUserId) {
            *result = -1;
            return NOERROR;
        }
        resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
        Boolean permissionGranted =
                mHost->mSecurityPolicy->CanGetAccessibilityNodeInfoLocked(this,
                    resolvedWindowId);
        if (!permissionGranted) {
            *result = FALSE;
            return NOERROR;
        }
        else {
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *result = FALSE;
                return NOERROR;
            }
        }
        if (!mHost->mSecurityPolicy->ComputePartialInteractiveRegionForWindowLocked(
                resolvedWindowId, partialInteractiveRegion)) {
            partialInteractiveRegion = NULL;
        }
    }

    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    AutoPtr<IMagnificationSpec> spec = mHost->GetCompatibleMagnificationSpecLocked(resolvedWindowId);
    // try {
    ECode ec = connection->FocusSearch(accessibilityNodeId, direction,
            partialInteractiveRegion, interactionId, callback,
            mFetchFlags, interrogatingPid, interrogatingTid, spec);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("AccessibilityManagerService::Service", "Error calling accessibilityFocusSearch()");
        }
        Binder::RestoreCallingIdentity(identityToken);
        *result = FALSE;
        return ec;
    }
    *result = TRUE;
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("AccessibilityManagerService::Service", "Error calling accessibilityFocusSearch()");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
}

ECode AccessibilityManagerService::Service::PerformAccessibilityAction(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 resolvedWindowId;
    AutoPtr<IIAccessibilityInteractionConnection> connection;
    synchronized(mHost->mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mHost->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                IUserHandle::USER_CURRENT, &resolvedUserId);
        if (resolvedUserId != mHost->mCurrentUserId) {
            *result = FALSE;
            return NOERROR;
        }

        resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
        Boolean permissionGranted = mHost->mSecurityPolicy->CanGetAccessibilityNodeInfoLocked(
                this, resolvedWindowId);
        if (!permissionGranted) {
            *result = FALSE;
            return NOERROR;
        }
        else {
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *result = FALSE;
                return NOERROR;
            }
        }
    }

    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = connection->PerformAccessibilityAction(
            accessibilityNodeId, action, arguments, interactionId,
            callback, mFetchFlags, interrogatingPid, interrogatingTid);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("AccessibilityManagerService::Service", "Error calling performAccessibilityAction()");
        }
    }
    Binder::RestoreCallingIdentity(identityToken);
    *result = TRUE;
    return ec;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("AccessibilityManagerService::Service", "Error calling performAccessibilityAction()");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
}

ECode AccessibilityManagerService::Service::PerformGlobalAction(
    /* [in] */ Int32 action,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    synchronized(mHost->mLock) {
        Int32 resolvedUserId;
        mHost->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                IUserHandle::USER_CURRENT, &resolvedUserId);
        if (resolvedUserId != mHost->mCurrentUserId) {
            return NOERROR;
        }
    }

    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    switch (action) {
        case IAccessibilityService::GLOBAL_ACTION_BACK:
            SendDownAndUpKeyEvents(IKeyEvent::KEYCODE_BACK);
            *result = TRUE;
            break;
        case IAccessibilityService::GLOBAL_ACTION_HOME:
            SendDownAndUpKeyEvents(IKeyEvent::KEYCODE_HOME);
            *result = TRUE;
            break;
        case IAccessibilityService::GLOBAL_ACTION_RECENTS:
            OpenRecents();
            *result = TRUE;
            break;
        case IAccessibilityService::GLOBAL_ACTION_NOTIFICATIONS:
            ExpandNotifications();
            *result = TRUE;
            break;
        case IAccessibilityService::GLOBAL_ACTION_QUICK_SETTINGS:
            ExpandQuickSettings();
            *result = TRUE;
            break;
        case IAccessibilityService::GLOBAL_ACTION_POWER_DIALOG:
            ShowGlobalActions();
            *result = TRUE;
            break;
    }

    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
    // } finally {
    //     Binder.restoreCallingIdentity(identity);
    // }
}

ECode AccessibilityManagerService::Service::ComputeClickPointInScreen(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 interactionId,
    /* [in] */ IIAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 resolvedWindowId;
    AutoPtr<IIAccessibilityInteractionConnection> connection;
    AutoPtr<IRegion> partialInteractiveRegion = mHost->mTempRegion;
    synchronized(mHost->mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mHost->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                        IUserHandle::USER_CURRENT, &resolvedUserId);
        if (resolvedUserId != mHost->mCurrentUserId) {
            *result = FALSE;
            return NOERROR;
        }

        resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
        Boolean permissionGranted =
                mHost->mSecurityPolicy->CanRetrieveWindowContentLocked(this);
        if (!permissionGranted) {
            *result = FALSE;
            return NOERROR;
        }
        else {
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *result = FALSE;
                return NOERROR;
            }
        }
        if (!mHost->mSecurityPolicy->ComputePartialInteractiveRegionForWindowLocked(
                resolvedWindowId, partialInteractiveRegion)) {
            partialInteractiveRegion = NULL;
        }
    }

    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    AutoPtr<IMagnificationSpec> spec = mHost->GetCompatibleMagnificationSpecLocked(
            resolvedWindowId);
    // try {
    ECode ec = connection->ComputeClickPointInScreen(accessibilityNodeId,
            partialInteractiveRegion, interactionId, callback,
            interrogatingPid, interrogatingTid, spec);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("AccessibilityManagerService::Service", "Error computeClickPointInScreen().");
        }
        *result = FALSE;
        Binder::RestoreCallingIdentity(identityToken);
        return ec;
    }
    *result = TRUE;
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slog.e(TAG, "Error computeClickPointInScreen().");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
}

ECode AccessibilityManagerService::Service::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    mHost->mSecurityPolicy->EnforceCallingPermission(
            Elastos::Droid::Manifest::permission::DUMP, FUNCTION_DUMP);
    synchronized(mHost->mLock) {
        AutoPtr<IPackageManager> pk;
        mHost->mContext->GetPackageManager((IPackageManager**)&pk);
        AutoPtr<IResolveInfo> info;
        mAccessibilityServiceInfo->GetResolveInfo((IResolveInfo**)&info);
        AutoPtr<ICharSequence> seq;
        info->LoadLabel(pk, (ICharSequence**)&seq);
        String name;
        seq->ToString(&name);
        AutoPtr<IAppendable> apFw = IAppendable::Probe(pw);
        apFw->Append(CoreUtils::Convert(String("Service[label=") + name));

        AutoPtr<IAccessibilityServiceInfoHelper> helper;
        CAccessibilityServiceInfoHelper::AcquireSingleton(
                (IAccessibilityServiceInfoHelper**)&helper);
        String str;
        helper->FeedbackTypeToString(mFeedbackType, &str);
        apFw->Append(CoreUtils::Convert(String(", feedbackType") + str));

        Int32 capabilities;
        mAccessibilityServiceInfo->GetCapabilities(&capabilities);
        apFw->Append(CoreUtils::Convert(String(", capabilities=") +
                StringUtils::ToString(capabilities)));

        AutoPtr<IAccessibilityEventHelper> eventHelper;
        CAccessibilityEventHelper::AcquireSingleton(
                (IAccessibilityEventHelper**)&eventHelper);
        String result;
        eventHelper->EventTypeToString(mEventTypes, &result);
        apFw->Append(CoreUtils::Convert(String(", eventTypes=") + result));

        apFw->Append(CoreUtils::Convert(String(", notificationTimeout=") +
                StringUtils::ToString(mNotificationTimeout)));
        apFw->Append(CoreUtils::Convert(String("]")));
    }
    return NOERROR;
}

ECode AccessibilityManagerService::Service::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    /* do nothing - #binderDied takes care */
    return NOERROR;
}

ECode AccessibilityManagerService::Service::LinkToOwnDeathLocked()
{
    IProxy* proxy = (IProxy*)mService->Probe(EIID_IProxy);
    return proxy->LinkToDeath((IProxyDeathRecipient*)this, 0);
}

ECode AccessibilityManagerService::Service::UnlinkToOwnDeathLocked()
{
    IProxy* proxy = (IProxy*)mService->Probe(EIID_IProxy);
    Boolean result;
    return proxy->UnlinkToDeath((IProxyDeathRecipient*)this, 0, &result);
}

ECode AccessibilityManagerService::Service::ResetLocked()
{
    // try {
    // Clear the proxy in the other process so this
    // IIAccessibilityServiceConnection can be garbage collected.
    mServiceInterface->SetConnection(NULL, mId);
    // } catch (RemoteException re) {
    //     /* ignore */
    // }
    mService = NULL;
    mServiceInterface = NULL;
    return NOERROR;
}

Boolean AccessibilityManagerService::Service::IsConnectedLocked()
{
    return (mService != NULL);
}

ECode AccessibilityManagerService::Service::ProxyDied()
{
    synchronized(mHost->mLock) {
        // It is possible that this service's package was force stopped during
        // whose handling the death recipient is unlinked and still get a call
        // on binderDied since the call was made before we unlink but was
        // waiting on the lock we held during the force stop handling.
        if (!IsConnectedLocked()) {
            return NOERROR;
        }
        mWasConnectedAndDied = true;
        mKeyEventDispatcher->Flush();
        AutoPtr<UserState> userState = mHost->GetUserStateLocked(mUserId);
        // The death recipient is unregistered in removeServiceLocked
        mHost->RemoveServiceLocked(this, userState);
        ResetLocked();
        if (mIsAutomation) {
            // We no longer have an automation service, so restore
            // the state based on values in the settings database.
            userState->mInstalledServices->Remove(mAccessibilityServiceInfo);
            userState->mEnabledServices->Remove(mComponentName);
            userState->DestroyUiAutomationService();
            if (mHost->ReadConfigurationForUserStateLocked(userState)) {
                mHost->OnUserStateChangedLocked(userState);
            }
        }
        return NOERROR;
    }
    return NOERROR;
}

void AccessibilityManagerService::Service::NotifyAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    synchronized(mHost->mLock){
        Int32 eventType;
        event->GetEventType(&eventType);
        // Make a copy since during dispatch it is possible the event to
        // be modified to remove its source if the receiving service does
        // not have permission to access the window content.
        AutoPtr<IAccessibilityEventHelper> helper;
        ASSERT_SUCCEEDED(CAccessibilityEventHelper::AcquireSingleton(
                (IAccessibilityEventHelper**)&helper));
        AutoPtr<IAccessibilityEvent> newEvent;
        helper->Obtain(event, (IAccessibilityEvent**)&newEvent);
        AutoPtr<IInterface> obj;
        mPendingEvents->Get(eventType, (IInterface**)&obj);
        AutoPtr<IAccessibilityEvent> oldEvent = IAccessibilityEvent::Probe(obj);
        mPendingEvents->Put(eventType, newEvent);

        Int32 what = eventType;
        if (oldEvent != NULL) {
            mEventDispatchHandler->RemoveMessages(what);
            IAccessibilityRecord::Probe(oldEvent)->Recycle();
        }

        AutoPtr<IMessage> message;
        mEventDispatchHandler->ObtainMessage(what, (IMessage**)&message);
        Boolean result;
        mEventDispatchHandler->SendMessageDelayed(message, mNotificationTimeout, &result);
    }
}

void AccessibilityManagerService::Service::NotifyAccessibilityEventInternal(
    /* [in] */ Int32 eventType)
{
    AutoPtr<IIAccessibilityServiceClient> listener;
    AutoPtr<IAccessibilityEvent> event;

    synchronized(mHost->mLock) {
        listener = mServiceInterface;

        // If the service died/was disabled while the message for dispatching
        // the accessibility event was propagating the listener may be null.
        if (listener == NULL) {
            return;
        }

        AutoPtr<IInterface> obj;
        mPendingEvents->Get(eventType, (IInterface**)&obj);
        event = IAccessibilityEvent::Probe(obj);

        // Check for null here because there is a concurrent scenario in which this
        // happens: 1) A binder thread calls notifyAccessibilityServiceDelayedLocked
        // which posts a message for dispatching an event. 2) The message is pulled
        // from the queue by the handler on the service thread and the latter is
        // just about to acquire the lock and call this method. 3) Now another binder
        // thread acquires the lock calling notifyAccessibilityServiceDelayedLocked
        // so the service thread waits for the lock; 4) The binder thread replaces
        // the event with a more recent one (assume the same event type) and posts a
        // dispatch request releasing the lock. 5) Now the main thread is unblocked and
        // dispatches the event which is removed from the pending ones. 6) And ... now
        // the service thread handles the last message posted by the last binder call
        // but the event is already dispatched and hence looking it up in the pending
        // ones yields null. This check is much simpler that keeping count for each
        // event type of each service to catch such a scenario since only one message
        // is processed at a time.
        if (event == NULL) {
            return;
        }

        mPendingEvents->Remove(eventType);
        if (mHost->mSecurityPolicy->CanRetrieveWindowContentLocked(this)) {
            IAccessibilityRecord::Probe(event)->SetConnectionId(mId);
        }
        else {
            IAccessibilityRecord::Probe(event)->SetSource(NULL);
        }
        IAccessibilityRecord::Probe(event)->SetSealed(TRUE);
    }

    // try {
    if (FAILED(listener->OnAccessibilityEvent(event))) {
        Slogger::E("AccessibilityManagerService::Service",
                "Error during sending %p to %p", event.Get(), listener.Get());
    }
    IAccessibilityRecord::Probe(event)->Recycle();
    // if (DEBUG) {
    //     Slog.i("AccessibilityManagerService::Service", "Event " + event + " sent to " + listener);
    // }
    // } catch (RemoteException re) {
    //     Slogger::E("AccessibilityManagerService::Service", "Error during sending " + event + " to " + listener, re);
    // } finally {
    //     event.recycle();
    // }
}

void AccessibilityManagerService::Service::NotifyGesture(
    /* [in] */ Int32 gestureId)
{
    AutoPtr<IMessage> message;
    mInvocationHandler->ObtainMessage(InvocationHandler::MSG_ON_GESTURE,
            gestureId, 0, (IMessage**)&message);
    message->SendToTarget();
}

void AccessibilityManagerService::Service::NotifyKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<IMessage> message;
    mInvocationHandler->ObtainMessage(InvocationHandler::MSG_ON_KEY_EVENT,
            policyFlags, 0, event, (IMessage**)&message);
    message->SendToTarget();
}

void AccessibilityManagerService::Service::NotifyClearAccessibilityNodeInfoCache()
{
    Boolean res;
    mInvocationHandler->SendEmptyMessage(
            InvocationHandler::MSG_CLEAR_ACCESSIBILITY_CACHE, &res);
}

void AccessibilityManagerService::Service::NotifyGestureInternal(
    /* [in] */ Int32 gestureId)
{
    AutoPtr<IIAccessibilityServiceClient> listener;
    synchronized(mHost->mLock) {
        listener = mServiceInterface;
    }

    if (listener != NULL) {
        // try {
        ECode ec =listener->OnGesture(gestureId);
        if (FAILED(ec)) {
            Slogger::E("AccessibilityManagerService::Service",
                    "Error during sending gesture %d to %p 0x%08x", gestureId,
                    mService.Get(), ec);
        }
        // } catch (RemoteException re) {
        //     Slogger::E("AccessibilityManagerService::Service", "Error during sending gesture " + gestureId
        //             + " to " + mService, re);
        // }
    }
}

void AccessibilityManagerService::Service::NotifyKeyEventInternal(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    mKeyEventDispatcher->NotifyKeyEvent(event, policyFlags);
}

void AccessibilityManagerService::Service::NotifyClearAccessibilityCacheInternal()
{
    AutoPtr<IIAccessibilityServiceClient> listener;
    synchronized(mHost->mLock) {
        listener = mServiceInterface;
    }
    if (listener != NULL) {
        // try {
        ECode ec =listener->ClearAccessibilityCache();
        if (FAILED(ec)) {
            Slogger::E("AccessibilityManagerService::Service",
                    "Error during requesting accessibility info cache to be cleared. 0x%08x", ec);
        }
        // } catch (RemoteException re) {
        //     Slog.e(TAG, "Error during requesting accessibility info cache"
        //             + " to be cleared.", re);
        // }
    }
}

void AccessibilityManagerService::Service::SendDownAndUpKeyEvents(
    /* [in] */ Int32 keyCode)
{
    Int64 token = Binder::ClearCallingIdentity();
    String nullStr;

    // Inject down.
    Int64 downTime = SystemClock::GetUptimeMillis();
    AutoPtr<IKeyEventHelper> helper;
    ASSERT_SUCCEEDED(CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper));
    AutoPtr<IKeyEvent> down;
    helper->Obtain(downTime, downTime, IKeyEvent::ACTION_DOWN, keyCode, 0, 0,
            IKeyCharacterMap::VIRTUAL_KEYBOARD, 0, IKeyEvent::FLAG_FROM_SYSTEM,
            IInputDevice::SOURCE_KEYBOARD, nullStr, (IKeyEvent**)&down);

    AutoPtr<IInputManagerHelper> inputHelper;
    ASSERT_SUCCEEDED(CInputManagerHelper::AcquireSingleton(
            (IInputManagerHelper**)&inputHelper));
    AutoPtr<IInputManager> inputManager;
    ASSERT_SUCCEEDED(inputHelper->GetInstance((IInputManager**)&inputManager));
    Boolean result;
    inputManager->InjectInputEvent(IInputEvent::Probe(down),
            IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC, &result);
    IInputEvent::Probe(down)->Recycle();

    // Inject up.
    Int64 upTime = SystemClock::GetUptimeMillis();
    AutoPtr<IKeyEvent> up;
    helper->Obtain(downTime, upTime, IKeyEvent::ACTION_UP, keyCode, 0, 0,
            IKeyCharacterMap::VIRTUAL_KEYBOARD, 0, IKeyEvent::FLAG_FROM_SYSTEM,
            IInputDevice::SOURCE_KEYBOARD, nullStr, (IKeyEvent**)&up);
    inputManager->InjectInputEvent(IInputEvent::Probe(up),
            IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC, &result);
    IInputEvent::Probe(up)->Recycle();

    Binder::RestoreCallingIdentity(token);
}

void AccessibilityManagerService::Service::ExpandNotifications()
{
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<IInterface> obj;
    ASSERT_SUCCEEDED(mHost->mContext->GetSystemService(
            IContext::STATUS_BAR_SERVICE, (IInterface**)&obj));
    AutoPtr<IStatusBarManager> statusBarManager = IStatusBarManager::Probe(obj);
    statusBarManager->ExpandNotificationsPanel();

    Binder::RestoreCallingIdentity(token);
}

void AccessibilityManagerService::Service::ExpandQuickSettings()
{
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<IInterface> obj;
    ASSERT_SUCCEEDED(mHost->mContext->GetSystemService(
            IContext::STATUS_BAR_SERVICE, (IInterface**)&obj));
    AutoPtr<IStatusBarManager> statusBarManager = IStatusBarManager::Probe(obj);
    statusBarManager->ExpandSettingsPanel();

    Binder::RestoreCallingIdentity(token);
}

void AccessibilityManagerService::Service::OpenRecents()
{
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<IIStatusBarService> statusBarService =
            (IIStatusBarService*)ServiceManager::GetService(String("statusbar")).Get();
    // try {
    if (FAILED(statusBarService->ToggleRecentApps())) {
        Slogger::E("AccessibilityManagerService::Service", "Error toggling recent apps.");
    }
    // } catch (RemoteException e) {
    //     Slogger::E("AccessibilityManagerService::Service", "Error toggling recent apps.");
    // }

    Binder::RestoreCallingIdentity(token);
}

void AccessibilityManagerService::Service::ShowGlobalActions()
{
    mHost->mWindowManagerService->ShowGlobalActions();
}

AutoPtr<IIAccessibilityInteractionConnection> AccessibilityManagerService::Service::GetConnectionLocked(
    /* [in] */ Int32 windowId)
{
    if (DEBUG) {
        Slogger::I("AccessibilityManagerService::Service",
                "Trying to get interaction connection to windowId: %d", windowId);
    }

    AutoPtr<IInterface> obj;
    mHost->mGlobalInteractionConnections->Get(windowId, (IInterface**)&obj);
    AutoPtr<AccessibilityManagerService::AccessibilityConnectionWrapper> wrapper =
            (AccessibilityConnectionWrapper*) IProxyDeathRecipient::Probe(obj);

    if (wrapper == NULL) {
        AutoPtr<AccessibilityManagerService::UserState> state = mHost->GetCurrentUserStateLocked();
        obj = NULL;
        state->mInteractionConnections->Get(windowId, (IInterface**)&obj);
        wrapper = (AccessibilityConnectionWrapper*) IProxyDeathRecipient::Probe(obj);
    }
    if (wrapper != NULL && wrapper->mConnection != NULL) {
        return wrapper->mConnection;
    }
    if (DEBUG) {
        Slogger::E("AccessibilityManagerService::Service",
                "No interaction connection to window: %d", windowId);
    }
    return NULL;
}

Int32 AccessibilityManagerService::Service::ResolveAccessibilityWindowIdLocked(
    /* [in] */ Int32 accessibilityWindowId)
{
    if (accessibilityWindowId == IAccessibilityNodeInfo::ACTIVE_WINDOW_ID) {
        return mHost->mSecurityPolicy->GetActiveWindowId();
    }
    return accessibilityWindowId;
}

Int32 AccessibilityManagerService::Service::ResolveAccessibilityWindowIdForFindFocusLocked(
    /* [in] */ Int32 windowId,
    /* [in] */ Int32 focusType)
{
    if (windowId == IAccessibilityNodeInfo::ACTIVE_WINDOW_ID) {
        return mHost->mSecurityPolicy->mActiveWindowId;
    }
    if (windowId == IAccessibilityNodeInfo::ANY_WINDOW_ID) {
        if (focusType == IAccessibilityNodeInfo::FOCUS_INPUT) {
            return mHost->mSecurityPolicy->mFocusedWindowId;
        }
        else if (focusType == IAccessibilityNodeInfo::FOCUS_ACCESSIBILITY) {
            return mHost->mSecurityPolicy->mAccessibilityFocusedWindowId;
        }
    }
    return windowId;
}

//===============================================================================
//                  AccessibilityManagerService::WindowsForAccessibilityCallback
//===============================================================================

CAR_INTERFACE_IMPL(AccessibilityManagerService::WindowsForAccessibilityCallback,
        Object, IWindowsForAccessibilityCallback);

AccessibilityManagerService::WindowsForAccessibilityCallback::WindowsForAccessibilityCallback(
    /* [in] */ AccessibilityManagerService* host)
    : mHost(host)
{}

AccessibilityManagerService::WindowsForAccessibilityCallback::~WindowsForAccessibilityCallback()
{}

ECode AccessibilityManagerService::WindowsForAccessibilityCallback::OnWindowsForAccessibilityChanged(
    /* [in] */ IList* windows)
{
    synchronized(mHost->mLock) {
        // Populate the windows to report.
        AutoPtr<IList> reportedWindows;
        CArrayList::New((IList**)&reportedWindows);
        Int32 receivedWindowCount;
        windows->GetSize(&receivedWindowCount);
        for (Int32 i = 0; i < receivedWindowCount; i++) {
            AutoPtr<IInterface> obj;
            windows->Get(i, (IInterface**)&obj);
            AutoPtr<IWindowInfo> receivedWindow = IWindowInfo::Probe(obj);
            AutoPtr<IAccessibilityWindowInfo> reportedWindow =
                    PopulateReportedWindow(receivedWindow);
            if (reportedWindow != NULL) {
                reportedWindows->Add(reportedWindow);
            }
        }

        if (DEBUG) {
            Slogger::I("AccessibilityManagerService::WindowsForAccessibilityCallback",
                    "Windows changed: %p", reportedWindows.Get());
        }

        // Let the policy update the focused and active windows.
        mHost->mSecurityPolicy->UpdateWindowsLocked(reportedWindows);

        // Someone may be waiting for the windows - advertise it.
        mHost->mLock.NotifyAll();
        return NOERROR;
    }
    return NOERROR;
}

AutoPtr<IAccessibilityWindowInfo> AccessibilityManagerService::WindowsForAccessibilityCallback::PopulateReportedWindow(
    /* [in] */ IWindowInfo* window)
{
    AutoPtr<IBinder> token;
    window->GetToken((IBinder**)&token);
    Int32 windowId = mHost->FindWindowIdLocked(token);
    if (windowId < 0) {
        return NULL;
    }

    AutoPtr<IAccessibilityWindowInfoHelper> helper;
    CAccessibilityWindowInfoHelper::AcquireSingleton((IAccessibilityWindowInfoHelper**)&helper);
    AutoPtr<IAccessibilityWindowInfo> reportedWindow;
    helper->Obtain((IAccessibilityWindowInfo**)&reportedWindow);

    reportedWindow->SetId(windowId);
    Int32 type;
    window->GetType(&type);
    reportedWindow->SetType(GetTypeForWindowManagerWindowType(type));
    Int32 layer;
    window->GetLayer(&layer);
    reportedWindow->SetLayer(layer);
    Boolean focused;
    window->GetFocused(&focused);
    reportedWindow->SetFocused(focused);
    AutoPtr<IRect> boundsInScreen;
    window->GetBoundsInScreen((IRect**)&boundsInScreen);
    reportedWindow->SetBoundsInScreen(boundsInScreen);

    AutoPtr<IBinder> parentToken;
    window->GetParentToken((IBinder**)&parentToken);
    Int32 parentId = mHost->FindWindowIdLocked(parentToken);
    if (parentId >= 0) {
        reportedWindow->SetParentId(parentId);
    }

    AutoPtr<IList> childTokens;
    window->GetChildTokens((IList**)&childTokens);
    if (childTokens != NULL) {
        Int32 childCount;
        childTokens->GetSize(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IInterface> obj;
            childTokens->Get(i, (IInterface**)&obj);
            AutoPtr<IBinder> childToken = IBinder::Probe(obj);
            Int32 childId = mHost->FindWindowIdLocked(childToken);
            if (childId >= 0) {
                reportedWindow->AddChild(childId);
            }
        }
    }

    return reportedWindow;
}

Int32 AccessibilityManagerService::WindowsForAccessibilityCallback::GetTypeForWindowManagerWindowType(
    /* [in] */ Int32 windowType)
{
    switch (windowType) {
        case IWindowManagerLayoutParams::TYPE_APPLICATION:
        case IWindowManagerLayoutParams::TYPE_APPLICATION_MEDIA:
        case IWindowManagerLayoutParams::TYPE_APPLICATION_PANEL:
        case IWindowManagerLayoutParams::TYPE_APPLICATION_STARTING:
        case IWindowManagerLayoutParams::TYPE_APPLICATION_SUB_PANEL:
        case IWindowManagerLayoutParams::TYPE_BASE_APPLICATION:
        case IWindowManagerLayoutParams::TYPE_PHONE:
        case IWindowManagerLayoutParams::TYPE_PRIORITY_PHONE:
        case IWindowManagerLayoutParams::TYPE_TOAST:
        case IWindowManagerLayoutParams::TYPE_APPLICATION_ATTACHED_DIALOG: {
            return IAccessibilityWindowInfo::TYPE_APPLICATION;
        }

        case IWindowManagerLayoutParams::TYPE_INPUT_METHOD:
        case IWindowManagerLayoutParams::TYPE_INPUT_METHOD_DIALOG: {
            return IAccessibilityWindowInfo::TYPE_INPUT_METHOD;
        }

        case IWindowManagerLayoutParams::TYPE_KEYGUARD_DIALOG:
        case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR:
        case IWindowManagerLayoutParams::TYPE_NAVIGATION_BAR_PANEL:
        case IWindowManagerLayoutParams::TYPE_SEARCH_BAR:
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR:
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR_PANEL:
        case IWindowManagerLayoutParams::TYPE_STATUS_BAR_SUB_PANEL:
        case IWindowManagerLayoutParams::TYPE_RECENTS_OVERLAY:
        case IWindowManagerLayoutParams::TYPE_VOLUME_OVERLAY:
        case IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT:
        case IWindowManagerLayoutParams::TYPE_SYSTEM_DIALOG:
        case IWindowManagerLayoutParams::TYPE_SYSTEM_ERROR:
        case IWindowManagerLayoutParams::TYPE_SYSTEM_OVERLAY: {
            return IAccessibilityWindowInfo::TYPE_SYSTEM;
        }

        default: {
            return -1;
        }
    }
}

//===============================================================================
//                  AccessibilityManagerService::SecurityPolicy
//===============================================================================

AccessibilityManagerService::SecurityPolicy::SecurityPolicy(
    /* [in] */ AccessibilityManagerService* host)
    : mActiveWindowId(INVALID_WINDOW_ID)
    , mFocusedWindowId(INVALID_WINDOW_ID)
    , mAccessibilityFocusedWindowId(INVALID_WINDOW_ID)
    , mAccessibilityFocusNodeId(IAccessibilityNodeInfo::UNDEFINED_ITEM_ID)
    , mTouchInteractionInProgress(FALSE)
    , mHost(host)
{}

AccessibilityManagerService::SecurityPolicy::~SecurityPolicy()
{}

Boolean AccessibilityManagerService::SecurityPolicy::CanDispatchAccessibilityEventLocked(
    /* [in] */ IAccessibilityEvent* event)
{
    Int32 eventType;
    event->GetEventType(&eventType);
    Int32 wid;
    switch (eventType) {
        // All events that are for changes in a global window
        // state should *always* be dispatched.
        case IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED: {
            if (mHost->mWindowsForAccessibilityCallback != NULL) {
                // OK, this is fun. Sometimes the focused window is notified
                // it has focus before being shown. Historically this event
                // means that the window is focused and can be introspected.
                // But we still have not gotten the window state from the
                // window manager, so delay the notification until then.
                IAccessibilityRecord::Probe(event)->GetWindowId(&wid);
                AutoPtr<IAccessibilityWindowInfo> window = FindWindowById(wid);
                if (window == NULL) {
                    AutoPtr<IAccessibilityEventHelper> helper;
                    CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
                    helper->Obtain(event, (IAccessibilityEvent**)&mShowingFocusedWindowEvent);
                    return FALSE;
                }
            }
        }
        // $fall-through$
        case IAccessibilityEvent::TYPE_NOTIFICATION_STATE_CHANGED:
        case IAccessibilityEvent::TYPE_ANNOUNCEMENT:
        // All events generated by the user touching the
        // screen should *always* be dispatched.
        case IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_START:
        case IAccessibilityEvent::TYPE_TOUCH_EXPLORATION_GESTURE_END:
        case IAccessibilityEvent::TYPE_GESTURE_DETECTION_START:
        case IAccessibilityEvent::TYPE_GESTURE_DETECTION_END:
        case IAccessibilityEvent::TYPE_TOUCH_INTERACTION_START:
        case IAccessibilityEvent::TYPE_TOUCH_INTERACTION_END:
        case IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER:
        case IAccessibilityEvent::TYPE_VIEW_HOVER_EXIT:
        // Also windows changing should always be anounced.
        case IAccessibilityEvent::TYPE_WINDOWS_CHANGED:
            return TRUE;
        // All events for changes in window content should be
        // dispatched *only* if this window is one of the windows
        // the accessibility layer reports which are windows
        // that a sighted user can touch.
        default: {
            IAccessibilityRecord::Probe(event)->GetWindowId(&wid);
            return IsRetrievalAllowingWindow(wid);
        }
    }
}

void AccessibilityManagerService::SecurityPolicy::ClearWindowsLocked()
{
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    AutoPtr<IList> windows;
    coll->GetEmptyList((IList**)&windows);
    Int32 activeWindowId = mActiveWindowId;
    UpdateWindowsLocked(windows);
    mActiveWindowId = activeWindowId;
    mWindows = NULL;
}

void AccessibilityManagerService::SecurityPolicy::UpdateWindowsLocked(
    /* [in] */ IList* windows)
{
    if (mWindows == NULL) {
        CArrayList::New((IList**)&mWindows);
    }

    Int32 oldWindowCount;
    mWindows->GetSize(&oldWindowCount);
    for (Int32 i = oldWindowCount - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mWindows->Remove(i, (IInterface**)&obj);
        IAccessibilityWindowInfo::Probe(obj)->Recycle();
    }

    mFocusedWindowId = INVALID_WINDOW_ID;
    if (!mTouchInteractionInProgress) {
        mActiveWindowId = INVALID_WINDOW_ID;
    }

    // If the active window goes away while the user is touch exploring we
    // reset the active window id and wait for the next hover event from
    // under the user's finger to determine which one is the new one. It
    // is possible that the finger is not moving and the input system
    // filters out such events.
    Boolean activeWindowGone = TRUE;

    Int32 windowCount;
    windows->GetSize(&windowCount);
    if (windowCount > 0) {
        for (Int32 i = 0; i < windowCount; i++) {
            AutoPtr<IInterface> obj;
            windows->Get(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityWindowInfo> window = IAccessibilityWindowInfo::Probe(obj);
            Int32 windowId;
            window->GetId(&windowId);
            Boolean res;
            if (window->IsFocused(&res), res) {
                mFocusedWindowId = windowId;
                if (!mTouchInteractionInProgress) {
                    mActiveWindowId = windowId;
                    window->SetActive(TRUE);
                }
                else if (windowId == mActiveWindowId) {
                    activeWindowGone = FALSE;
                }
            }
            mWindows->Add(window);
        }

        if (mTouchInteractionInProgress && activeWindowGone) {
            mActiveWindowId = mFocusedWindowId;
        }

        // Focused window may change the active one, so set the
        // active window once we decided which it is.
        for (Int32 i = 0; i < windowCount; i++) {
            AutoPtr<IInterface> obj;
            mWindows->Get(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityWindowInfo> window = IAccessibilityWindowInfo::Probe(obj);
            Int32 id;
            if ((window->GetId(&id), id) == mActiveWindowId) {
                window->SetActive(TRUE);
            }
            if ((window->GetId(&id), id) == mAccessibilityFocusedWindowId) {
                window->SetAccessibilityFocused(TRUE);
            }
        }
    }

    NotifyWindowsChanged();

    // If we are delaying a window state change event as the window
    // source was showing when it was fired, now is the time to send.
    if (mShowingFocusedWindowEvent != NULL) {
        Int32 windowId;
        IAccessibilityRecord::Probe(mShowingFocusedWindowEvent)->GetWindowId(&windowId);
        AutoPtr<IAccessibilityWindowInfo> window = FindWindowById(windowId);
        if (window != NULL) {
            // Sending does the recycle.
            Boolean res;
            mHost->SendAccessibilityEvent(mShowingFocusedWindowEvent,
                    mHost->mCurrentUserId, &res);
        }
        mShowingFocusedWindowEvent = NULL;
    }
}

Boolean AccessibilityManagerService::SecurityPolicy::ComputePartialInteractiveRegionForWindowLocked(
    /* [in] */ Int32 windowId,
    /* [in] */ IRegion* outRegion)
{
    if (mWindows == NULL) {
        return FALSE;
    }

    // Windows are ordered in z order so start from the botton and find
    // the window of interest. After that all windows that cover it should
    // be subtracted from the resulting region. Note that for accessibility
    // we are returning only interactive windows.
    AutoPtr<IRegion> windowInteractiveRegion;
    Boolean windowInteractiveRegionChanged = FALSE;

    Int32 windowCount;
    mWindows->GetSize(&windowCount);
    for (Int32 i = windowCount - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        mWindows->Get(i, (IInterface**)&obj);
        AutoPtr<IAccessibilityWindowInfo> currentWindow =
                IAccessibilityWindowInfo::Probe(obj);
        if (windowInteractiveRegion == NULL) {
            Int32 id;
            currentWindow->GetId(&id);
            if (id == windowId) {
                AutoPtr<IRect> currentWindowBounds = mHost->mTempRect;
                currentWindow->GetBoundsInScreen(currentWindowBounds);
                Boolean res;
                outRegion->Set(currentWindowBounds, &res);
                windowInteractiveRegion = outRegion;
                continue;
            }
        }
        else {
            AutoPtr<IRect> currentWindowBounds = mHost->mTempRect;
            currentWindow->GetBoundsInScreen(currentWindowBounds);
            Boolean res;
            if (windowInteractiveRegion->Op(currentWindowBounds,
                    Elastos::Droid::Graphics::RegionOp_DIFFERENCE, &res), res) {
                windowInteractiveRegionChanged = TRUE;
            }
        }
    }

    return windowInteractiveRegionChanged;
}

void AccessibilityManagerService::SecurityPolicy::UpdateEventSourceLocked(
    /* [in] */ IAccessibilityEvent* event)
{
    Int32 type;
    event->GetEventType(&type);
    if ((type & RETRIEVAL_ALLOWING_EVENT_TYPES) == 0) {
        IAccessibilityRecord::Probe(event)->SetSource(NULL);
    }
}

void AccessibilityManagerService::SecurityPolicy::UpdateActiveAndAccessibilityFocusedWindowLocked(
    /* [in] */ Int32 windowId,
    /* [in] */ Int64 nodeId,
    /* [in] */ Int32 eventType)
{
    // The active window is either the window that has input focus or
    // the window that the user is currently touching. If the user is
    // touching a window that does not have input focus as soon as the
    // the user stops touching that window the focused window becomes
    // the active one. Here we detect the touched window and make it
    // active. In updateWindowsLocked() we update the focused window
    // and if the user is not touching the screen, we make the focused
    // window the active one.
    switch (eventType) {
        case IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED: {
            // If no service has the capability to introspect screen,
            // we do not register callback in the window manager for
            // window changes, so we have to ask the window manager
            // what the focused window is to update the active one.
            // The active window also determined events from which
            // windows are delivered.
            synchronized(mHost->mLock) {
                if (mHost->mWindowsForAccessibilityCallback == NULL) {
                    mFocusedWindowId = GetFocusedWindowId();
                    if (windowId == mFocusedWindowId) {
                        mActiveWindowId = windowId;
                    }
                }
            }
            break;
        }

        case IAccessibilityEvent::TYPE_VIEW_HOVER_ENTER: {
            // Do not allow delayed hover events to confuse us
            // which the active window is.
            synchronized(mHost->mLock) {
                if (mTouchInteractionInProgress && mActiveWindowId != windowId) {
                    SetActiveWindowLocked(windowId);
                }
            }
            break;
        }

        case IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED: {
            synchronized(mHost->mLock) {
                if (mAccessibilityFocusedWindowId != windowId) {
                    AutoPtr<IMessage> message;
                    mHost->mMainHandler->ObtainMessage(
                            MainHandler::MSG_CLEAR_ACCESSIBILITY_FOCUS,
                            mAccessibilityFocusedWindowId, 0,
                            (IMessage**)&message);
                    message->SendToTarget();
                    mHost->mSecurityPolicy->SetAccessibilityFocusedWindowLocked(windowId);
                    mAccessibilityFocusNodeId = nodeId;
                }
            }
            break;
        }

        case IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUS_CLEARED: {
            synchronized(mHost->mLock) {
                if (mAccessibilityFocusNodeId == nodeId) {
                    mAccessibilityFocusNodeId = IAccessibilityNodeInfo::UNDEFINED_ITEM_ID;
                }
                if (mAccessibilityFocusNodeId == IAccessibilityNodeInfo::UNDEFINED_ITEM_ID
                        && mAccessibilityFocusedWindowId == windowId) {
                    mAccessibilityFocusedWindowId = INVALID_WINDOW_ID;
                }
            }
            break;
        }
    }
}

void AccessibilityManagerService::SecurityPolicy::OnTouchInteractionStart()
{
    synchronized(mHost->mLock) {
        mTouchInteractionInProgress = TRUE;
    }
}

void AccessibilityManagerService::SecurityPolicy::OnTouchInteractionEnd()
{
    synchronized(mHost->mLock) {
        mTouchInteractionInProgress = FALSE;
        // We want to set the active window to be current immediately
        // after the user has stopped touching the screen since if the
        // user types with the IME he should get a feedback for the
        // letter typed in the text view which is in the input focused
        // window. Note that we always deliver hover accessibility events
        // (they are a result of user touching the screen) so change of
        // the active window before all hover accessibility events from
        // the touched window are delivered is fine.
        Int32 oldActiveWindow = mHost->mSecurityPolicy->mActiveWindowId;
        SetActiveWindowLocked(mFocusedWindowId);

        // If there is no service that can operate with active windows
        // we keep accessibility focus behavior to constrain it only in
        // the active window. Look at updateAccessibilityFocusBehaviorLocked
        // for details.
        if (oldActiveWindow != mHost->mSecurityPolicy->mActiveWindowId
                && mAccessibilityFocusedWindowId == oldActiveWindow
                && mHost->GetCurrentUserStateLocked()->mAccessibilityFocusOnlyInActiveWindow) {
            AutoPtr<IMessage> message;
            mHost->mMainHandler->ObtainMessage(
                    MainHandler::MSG_CLEAR_ACCESSIBILITY_FOCUS,
                    oldActiveWindow, 0, (IMessage**)&message);
            message->SendToTarget();
        }
    }
}

Int32 AccessibilityManagerService::SecurityPolicy::GetActiveWindowId()
{
    if (mActiveWindowId == INVALID_WINDOW_ID && !mTouchInteractionInProgress) {
        mActiveWindowId = GetFocusedWindowId();
    }
    return mActiveWindowId;
}

void AccessibilityManagerService::SecurityPolicy::SetActiveWindowLocked(
    /* [in] */ Int32 windowId)
{
    if (mActiveWindowId != windowId) {
        mActiveWindowId = windowId;
        if (mWindows != NULL) {
            Int32 windowCount;
            mWindows->GetSize(&windowCount);
            for (Int32 i = 0; i < windowCount; i++) {
                AutoPtr<IInterface> obj;
                mWindows->Get(i, (IInterface**)&obj);
                AutoPtr<IAccessibilityWindowInfo> window =
                        IAccessibilityWindowInfo::Probe(obj);
                Int32 id;
                window->GetId(&id);
                window->SetActive(id == windowId);
            }
        }
        NotifyWindowsChanged();
    }
}

void AccessibilityManagerService::SecurityPolicy::SetAccessibilityFocusedWindowLocked(
    /* [in] */ Int32 windowId)
{
    if (mAccessibilityFocusedWindowId != windowId) {
        mAccessibilityFocusedWindowId = windowId;
        if (mWindows != NULL) {
            Int32 windowCount;
            mWindows->GetSize(&windowCount);
            for (Int32 i = 0; i < windowCount; i++) {
                AutoPtr<IInterface> obj;
                mWindows->Get(i, (IInterface**)&obj);
                AutoPtr<IAccessibilityWindowInfo> window =
                        IAccessibilityWindowInfo::Probe(obj);
                Int32 id;
                window->GetId(&id);
                window->SetAccessibilityFocused(id == windowId);
            }
        }

        NotifyWindowsChanged();
    }
}

void AccessibilityManagerService::SecurityPolicy::NotifyWindowsChanged()
{
    if (mHost->mWindowsForAccessibilityCallback == NULL) {
        return;
    }
    const Int64 identity = Binder::ClearCallingIdentity();
    // try {
    // Let the client know the windows changed.
    AutoPtr<IAccessibilityEventHelper> helper;
    CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
    AutoPtr<IAccessibilityEvent> event;
    helper->Obtain(
            IAccessibilityEvent::TYPE_WINDOWS_CHANGED, (IAccessibilityEvent**)&event);
    event->SetEventTime(SystemClock::GetUptimeMillis());
    Boolean res;
    mHost->SendAccessibilityEvent(event, mHost->mCurrentUserId, &res);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
}

Boolean AccessibilityManagerService::SecurityPolicy::CanGetAccessibilityNodeInfoLocked(
    /* [in] */ Service* service,
    /* [in] */ Int32 windowId)
{
    return CanRetrieveWindowContentLocked(service) &&
            IsRetrievalAllowingWindow(windowId);
}

Boolean AccessibilityManagerService::SecurityPolicy::CanRetrieveWindowsLocked(
    /* [in] */ Service* service)
{
    return CanRetrieveWindowContentLocked(service) &&
            service->mRetrieveInteractiveWindows;
}

Boolean AccessibilityManagerService::SecurityPolicy::CanRetrieveWindowContentLocked(
    /* [in] */ Service* service)
{
    Int32 capability;
    service->mAccessibilityServiceInfo->GetCapabilities(&capability);
    return (capability
            & IAccessibilityServiceInfo::CAPABILITY_CAN_RETRIEVE_WINDOW_CONTENT) != 0;
}

Int32 AccessibilityManagerService::SecurityPolicy::ResolveProfileParentLocked(
    /* [in] */ Int32 userId)
{
    if (userId != mHost->mCurrentUserId) {
        const Int64 identity = Binder::ClearCallingIdentity();
        // try {
        AutoPtr<IUserInfo> parent;
        mHost->mUserManager->GetProfileParent(userId, (IUserInfo**)&parent);
        if (parent != NULL) {
            AutoPtr<IUserHandle> handle;
            parent->GetUserHandle((IUserHandle**)&handle);
            Int32 id;
            handle->GetIdentifier(&id);
            Binder::RestoreCallingIdentity(identity);
            return id;
        }
        // } finally {
        Binder::RestoreCallingIdentity(identity);
        // }
    }
    return userId;
}

ECode AccessibilityManagerService::SecurityPolicy::ResolveCallingUserIdEnforcingPermissionsLocked(
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 callingUid = Binder::GetCallingUid();
    if (callingUid == 0
            || callingUid == IProcess::SYSTEM_UID
            || callingUid == IProcess::SHELL_UID) {
        if (userId == IUserHandle::USER_CURRENT
                || userId == IUserHandle::USER_CURRENT_OR_SELF) {
            *result = mHost->mCurrentUserId;
            return NOERROR;
        }
    }

    Int32 callingUserId = UserHandle::GetUserId(callingUid);

    if (callingUserId == userId) {
        *result = ResolveProfileParentLocked(userId);
        return NOERROR;
    }
    Int32 callingUserParentId = ResolveProfileParentLocked(callingUserId);
    if (callingUserParentId == mHost->mCurrentUserId &&
            (userId == IUserHandle::USER_CURRENT
                    || userId == IUserHandle::USER_CURRENT_OR_SELF)) {
        *result = mHost->mCurrentUserId;
        return NOERROR;
    }

    if (!HasPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS)
            && !HasPermission(Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL)) {
        Slogger::E("AccessibilityManagerService::SecurityPolicy", "Call from user %d as user %d" \
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
    Slogger::E("AccessibilityManagerService::SecurityPolicy", "Calling user can be changed to" \
            " only IUserHandle::USER_CURRENT or IUserHandle::USER_CURRENT_OR_SELF.");
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
    // throw new IllegalArgumentException("Calling user can be changed to only "
    //         + "IUserHandle::USER_CURRENT or IUserHandle::USER_CURRENT_OR_SELF.");
}

Boolean AccessibilityManagerService::SecurityPolicy::IsCallerInteractingAcrossUsers(
    /* [in] */ Int32 userId)
{
    Int32 callingUid = Binder::GetCallingUid();
    return (Binder::GetCallingPid() == Process::MyPid()
            || callingUid == IProcess::SHELL_UID
            || userId == IUserHandle::USER_CURRENT
            || userId == IUserHandle::USER_CURRENT_OR_SELF);
}

Boolean AccessibilityManagerService::SecurityPolicy::IsRetrievalAllowingWindow(
    /* [in] */ Int32 windowId)
{
    // The system gets to interact with any window it wants.
    if (Binder::GetCallingUid() == IProcess::SYSTEM_UID) {
        return TRUE;
    }
    if (windowId == mActiveWindowId) {
        return TRUE;
    }
    return FindWindowById(windowId) != NULL;
}

AutoPtr<IAccessibilityWindowInfo> AccessibilityManagerService::SecurityPolicy::FindWindowById(
    /* [in] */ Int32 windowId)
{
    if (mWindows != NULL) {
        Int32 windowCount;
        mWindows->GetSize(&windowCount);
        for (Int32 i = 0; i < windowCount; i++) {
            AutoPtr<IInterface> obj;
            mWindows->Get(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityWindowInfo> window =
                    IAccessibilityWindowInfo::Probe(obj);
            Int32 id;
            window->GetId(&id);
            if (id == windowId) {
                return window;
            }
        }
    }
    return NULL;
}

ECode AccessibilityManagerService::SecurityPolicy::EnforceCallingPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& function)
{
    if (AccessibilityManagerService::OWN_PROCESS_ID == Binder::GetCallingPid()) {
        return NOERROR;
    }

    if (!HasPermission(permission)) {
        Slogger::E("AccessibilityManagerService::SecurityPolicy",
                "You do not have %s required to call %s from pid=%d, uid=%d",
                permission.string(), function.string(), Binder::GetCallingPid(),
                Binder::GetCallingUid());
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("You do not have " + permission%s
        //         + " required to call " + function + " from pid="
        //         + Binder.getCallingPid() + ", uid=" + Binder.getCallingUid());
    }
    return NOERROR;
}

Boolean AccessibilityManagerService::SecurityPolicy::HasPermission(
    /* [in] */ const String& permission)
{
    if (permission.IsNull()) return FALSE;
    Int32 result;
    mHost->mContext->CheckCallingPermission(permission, &result);
    return result == IPackageManager::PERMISSION_GRANTED;
}

Int32 AccessibilityManagerService::SecurityPolicy::GetFocusedWindowId()
{
    Int32 retval = 0;
    AutoPtr<IBinder> token;
    ASSERT_SUCCEEDED(mHost->mWindowManagerService->GetFocusedWindowToken((IBinder**)&token));
    synchronized(mHost->mLock) {
        retval = mHost->FindWindowIdLocked(token);
    }
    return retval;
}

//===============================================================================
//                  AccessibilityManagerService::InvocationHandler
//===============================================================================

AccessibilityManagerService::InvocationHandler::InvocationHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ Service* host)
    : Handler(looper, NULL, TRUE)
    , mHost(host)
{}

AccessibilityManagerService::InvocationHandler::~InvocationHandler()
{}

ECode AccessibilityManagerService::InvocationHandler::HandleMessage(
    /* [in] */ IMessage* message)
{
    Int32 type;
    message->GetWhat(&type);
    switch (type) {
        case MSG_ON_GESTURE: {
            Int32 gestureId;
            message->GetArg1(&gestureId);
            mHost->NotifyGestureInternal(gestureId);
            break;
        }

        case MSG_ON_KEY_EVENT: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<IKeyEvent> event = IKeyEvent::Probe(obj);
            Int32 policyFlags;
            message->GetArg1(&policyFlags);
            mHost->NotifyKeyEventInternal(event, policyFlags);
            break;
        }

        case MSG_CLEAR_ACCESSIBILITY_CACHE: {
            mHost->NotifyClearAccessibilityCacheInternal();
            break;
        }

        case MSG_ON_KEY_EVENT_TIMEOUT: {
            AutoPtr<IInterface> obj;
            message->GetObj((IInterface**)&obj);
            AutoPtr<PendingEvent> eventState = (PendingEvent*)IObject::Probe(obj);
            mHost->SetOnKeyEventResult(FALSE, eventState->mSequence);
            break;
        }

        default: {
            Slogger::E("AccessibilityManagerService::InvocationHandler", "Unknown message: %d", type);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("Unknown message: " + type);
        }
    }
    return NOERROR;
}

//===============================================================================
//                  AccessibilityManagerService::KeyEventDispatcher
//===============================================================================

AccessibilityManagerService::KeyEventDispatcher::KeyEventDispatcher(
    /* [in] */ Service* owner,
    /* [in] */ AccessibilityManagerService* host)
    : mOwner(owner)
    , mHost(host)
{
    assert(0 && "TODO");
    // mSentEventsVerifier =
    //         InputEventConsistencyVerifier.isInstrumentationEnabled()
    //                 ? new InputEventConsistencyVerifier(
    //                         this, 0, KeyEventDispatcher.class.getSimpleName()) : null;
}

AccessibilityManagerService::KeyEventDispatcher::~KeyEventDispatcher()
{}

ECode AccessibilityManagerService::KeyEventDispatcher::NotifyKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    AutoPtr<PendingEvent> pendingEvent;

    synchronized(mHost->mLock) {
        pendingEvent = AddPendingEventLocked(event, policyFlags);
    }

    AutoPtr<IMessage> message;
    mOwner->mInvocationHandler->ObtainMessage(
            InvocationHandler::MSG_ON_KEY_EVENT_TIMEOUT, (IObject*)pendingEvent,
            (IMessage**)&message);
    Boolean res;
    mOwner->mInvocationHandler->SendMessageDelayed(message,
            ON_KEY_EVENT_TIMEOUT_MILLIS, &res);

    // try {
    // Accessibility services are exclusively not in the system
    // process, therefore no need to clone the motion event to
    // prevent tampering. It will be cloned in the IPC call.
    ECode ec = mOwner->mServiceInterface->OnKeyEvent(pendingEvent->mEvent,
            pendingEvent->mSequence);
    if (FAILED(ec)) {
        SetOnKeyEventResult(FALSE, pendingEvent->mSequence);
    }
    // } catch (RemoteException re) {
    //     setOnKeyEventResult(false, pendingEvent.sequence);
    // }
    return ec;
}

void AccessibilityManagerService::KeyEventDispatcher::SetOnKeyEventResult(
    /* [in] */ Boolean handled,
    /* [in] */ Int32 sequence)
{
    synchronized(mHost->mLock) {
        AutoPtr<PendingEvent> pendingEvent = RemovePendingEventLocked(sequence);
        if (pendingEvent != NULL) {
            mOwner->mInvocationHandler->RemoveMessages(
                    InvocationHandler::MSG_ON_KEY_EVENT_TIMEOUT,
                    (IObject*)pendingEvent);
            pendingEvent->mHandled = handled;
            FinishPendingEventLocked(pendingEvent);
        }
    }
}

void AccessibilityManagerService::KeyEventDispatcher::Flush()
{
    synchronized(mHost->mLock) {
        CancelAllPendingEventsLocked();
        if (mSentEventsVerifier != NULL) {
            mSentEventsVerifier->Reset();
        }
    }
}

AutoPtr<PendingEvent> AccessibilityManagerService::KeyEventDispatcher::AddPendingEventLocked(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    Int32 sequence;
    IInputEvent::Probe(event)->GetSequenceNumber(&sequence);
    AutoPtr<PendingEvent> pendingEvent =
            mHost->ObtainPendingEventLocked(event, policyFlags, sequence);
    pendingEvent->mNext = mPendingEvents;
    mPendingEvents = pendingEvent;
    return pendingEvent;
}

AutoPtr<PendingEvent> AccessibilityManagerService::KeyEventDispatcher::RemovePendingEventLocked(
    /* [in] */ Int32 sequence)
{
    AutoPtr<PendingEvent> previous;
    AutoPtr<PendingEvent> current = mPendingEvents;

    while (current != NULL) {
        if (current->mSequence == sequence) {
            if (previous != NULL) {
                previous->mNext = current->mNext;
            }
            else {
                mPendingEvents = current->mNext;
            }
            current->mNext = NULL;
            return current;
        }
        previous = current;
        current = current->mNext;
    }
    return NULL;
}

void AccessibilityManagerService::KeyEventDispatcher::FinishPendingEventLocked(
    /* [in] */ PendingEvent* pendingEvent)
{
    if (!pendingEvent->mHandled) {
        SendKeyEventToInputFilter(pendingEvent->mEvent, pendingEvent->mPolicyFlags);
    }
    // Nullify the event since we do not want it to be
    // recycled yet. It will be sent to the input filter.
    pendingEvent->mEvent = NULL;
    mHost->RecyclePendingEventLocked(pendingEvent);
}

void AccessibilityManagerService::KeyEventDispatcher::SendKeyEventToInputFilter(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    if (DEBUG) {
        Slogger::I("AccessibilityManagerService::KeyEventDispatcher",
                "Injecting event: %p", event);
    }
    if (mSentEventsVerifier != NULL) {
        mSentEventsVerifier->OnKeyEvent(event, 0);
    }
    policyFlags |= IWindowManagerPolicy::FLAG_PASS_TO_USER;
    AutoPtr<IMessage> message;
    mHost->mMainHandler->ObtainMessage(MainHandler::MSG_SEND_KEY_EVENT_TO_INPUT_FILTER,
            policyFlags, 0, event, (IMessage**)&message);
    message->SendToTarget();
}

void AccessibilityManagerService::KeyEventDispatcher::CancelAllPendingEventsLocked()
{
    while (mPendingEvents != NULL) {
        AutoPtr<PendingEvent> pendingEvent = RemovePendingEventLocked(mPendingEvents->mSequence);
        pendingEvent->mHandled = FALSE;
        mOwner->mInvocationHandler->RemoveMessages(
                InvocationHandler::MSG_ON_KEY_EVENT_TIMEOUT, (IObject*)pendingEvent);
        FinishPendingEventLocked(pendingEvent);
    }
}

//===============================================================================
//                  AccessibilityManagerService::EventDispatchHandler
//===============================================================================

AccessibilityManagerService::EventDispatchHandler::EventDispatchHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ Service* host)
    : Handler(looper)
    , mHost(host)
{}

AccessibilityManagerService::EventDispatchHandler::~EventDispatchHandler()
{}

ECode AccessibilityManagerService::EventDispatchHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 eventType;
    msg->GetWhat(&eventType);

    mHost->NotifyAccessibilityEventInternal(eventType);

    return NOERROR;
}

//===============================================================================
//                  AccessibilityManagerService::BindLockedRunnable
//===============================================================================

AccessibilityManagerService::BindLockedRunnable::BindLockedRunnable(
    /* [in] */ Service* host)
    : mHost(host)
{}

AccessibilityManagerService::BindLockedRunnable::~BindLockedRunnable()
{}

ECode AccessibilityManagerService::BindLockedRunnable::Run()
{
    // Simulate asynchronous connection since in onServiceConnected
    // we may modify the state data in case of an error but bind is
    // called while iterating over the data and bad things can happen.
    return mHost->OnServiceConnected(mHost->mComponentName, mHost->mService);
}

//===============================================================================
//                  AccessibilityManagerService::AccessibilityConnectionWrapper
//===============================================================================

CAR_INTERFACE_IMPL(AccessibilityManagerService::AccessibilityConnectionWrapper, Object, IProxyDeathRecipient);

AccessibilityManagerService::AccessibilityConnectionWrapper::AccessibilityConnectionWrapper(
    /* [in] */ Int32 windowId,
    /* [in] */ IIAccessibilityInteractionConnection* connection,
    /* [in] */ Int32 userId,
    /* [in] */ AccessibilityManagerService* host)
    : mWindowId(windowId)
    , mUserId(userId)
    , mConnection(connection)
    , mHost(host)
{}

AccessibilityManagerService::AccessibilityConnectionWrapper::~AccessibilityConnectionWrapper()
{}

ECode AccessibilityManagerService::AccessibilityConnectionWrapper::LinkToDeath()
{
    AutoPtr<IProxy> proxy = (IProxy*)mConnection->Probe(EIID_IProxy);
    if (proxy != NULL) {
        proxy->LinkToDeath(this, 0);
    }
    else {
        Slogger::W("AccessibilityManagerService::AccessibilityConnectionWrapper",
                "connection does not implement IProxy.");
    }
    return NOERROR;
}

ECode AccessibilityManagerService::AccessibilityConnectionWrapper::UnlinkToDeath()
{
    AutoPtr<IProxy> proxy = (IProxy*)mConnection->Probe(EIID_IProxy);
    if (proxy != NULL) {
        Boolean result;
        proxy->UnlinkToDeath(this, 0, &result);
    }
    else {
        Slogger::W("AccessibilityManagerService::AccessibilityConnectionWrapper",
                "connection does not implement IProxy.");
    }
    return NOERROR;
}

ECode AccessibilityManagerService::AccessibilityConnectionWrapper::ProxyDied()
{
    UnlinkToDeath();
    synchronized(mHost->mLock) {
        mHost->RemoveAccessibilityInteractionConnectionLocked(mWindowId, mUserId);
    }

    return NOERROR;
}

//===============================================================================
//                  AccessibilityManagerService::MainHandler
//===============================================================================

AccessibilityManagerService::MainHandler::MainHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ AccessibilityManagerService* service)
    : Handler(looper)
    , mHost(service)
{}

AccessibilityManagerService::MainHandler::~MainHandler()
{}

ECode AccessibilityManagerService::MainHandler::HandleMessage(
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
            AutoPtr<IAccessibilityEvent> event = IAccessibilityEvent::Probe(obj);
            synchronized(mHost->mLock) {
                if (mHost->mHasInputFilter && mHost->mInputFilter != NULL) {
                    mHost->mInputFilter->NotifyAccessibilityEvent(event);
                }
            }
            IAccessibilityRecord::Probe(event)->Recycle();
            break;
        }

        case MSG_SEND_KEY_EVENT_TO_INPUT_FILTER: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<IKeyEvent> event = IKeyEvent::Probe(obj);
            Int32 policyFlags = arg1;
            synchronized(mHost->mLock) {
                if (mHost->mHasInputFilter && mHost->mInputFilter != NULL) {
                    mHost->mInputFilter->SendInputEvent(IInputEvent::Probe(event),
                            policyFlags);
                }
            }
            IAccessibilityRecord::Probe(event)->Recycle();
            break;
        }

        case MSG_SEND_STATE_TO_CLIENTS: {
            Int32 clientState =arg1;
            Int32 userId = arg2;
            SendStateToClients(clientState, mHost->mGlobalClients);
            SendStateToClientsForUser(clientState, userId);
            break;
        }

        case MSG_SEND_CLEARED_STATE_TO_CLIENTS_FOR_USER: {
            Int32 userId = arg1;
            SendStateToClientsForUser(0, userId);
            break;
        }

        case MSG_ANNOUNCE_NEW_USER_IF_NEEDED: {
            AnnounceNewUserIfNeeded();
            break;
        }

        case MSG_UPDATE_INPUT_FILTER: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<UserState> userState = (UserState*)IObject::Probe(obj);
            mHost->UpdateInputFilter(userState);
            break;
        }

        case MSG_SHOW_ENABLED_TOUCH_EXPLORATION_DIALOG: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);
            mHost->ShowEnableTouchExplorationDialog(service);
            break;
        }

        case MSG_CLEAR_ACCESSIBILITY_FOCUS: {
            Int32 windowId = arg1;
            AutoPtr<InteractionBridge> bridge;
            synchronized(mHost->mLock) {
                bridge = mHost->GetInteractionBridgeLocked();
            }
            bridge->ClearAccessibilityFocusNotLocked(windowId);
            break;
        }
    }

    return NOERROR;
}

void AccessibilityManagerService::MainHandler::AnnounceNewUserIfNeeded()
{
    synchronized(mHost->mLock) {
        AutoPtr<UserState> userState = mHost->GetCurrentUserStateLocked();
        if (userState->mIsAccessibilityEnabled) {
            AutoPtr<IInterface> obj;
            ASSERT_SUCCEEDED(mHost->mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj));
            AutoPtr<IUserManager> userManager = IUserManager::Probe(obj);
            AutoPtr<IUserInfo> userInfo;
            userManager->GetUserInfo(mHost->mCurrentUserId, (IUserInfo**)&userInfo);
            String name;
            userInfo->GetName(&name);
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(name));
            String message;
            mHost->mContext->GetString(R::string::user_switched, args, &message);
            AutoPtr<IAccessibilityEventHelper> helper;
            CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
            AutoPtr<IAccessibilityEvent> event;
            helper->Obtain(IAccessibilityEvent::TYPE_ANNOUNCEMENT, (IAccessibilityEvent**)&event);
            AutoPtr<IList> texts;
            IAccessibilityRecord::Probe(event)->GetText((IList**)&texts);
            texts->Add(CoreUtils::Convert(message));
            Boolean result;
            mHost->SendAccessibilityEvent(event, mHost->mCurrentUserId, &result);
        }
    }
}

void AccessibilityManagerService::MainHandler::SendStateToClientsForUser(
    /* [in] */ Int32 clientState,
    /* [in] */ Int32 userId)
{
    AutoPtr<UserState> userState;
    synchronized(mHost->mLock) {
        userState = mHost->GetUserStateLocked(userId);
    }

    SendStateToClients(clientState, userState->mClients);
}

void AccessibilityManagerService::MainHandler::SendStateToClients(
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

//===============================================================================
//                  AccessibilityManagerService::InteractionBridge
//===============================================================================

AccessibilityManagerService::InteractionBridge::InteractionBridge(
    /* [in] */ AccessibilityManagerService* host)
    : mConnectionId(0)
    , mHost(host)
{
    AutoPtr<IAccessibilityServiceInfo> info;
    CAccessibilityServiceInfo::New((IAccessibilityServiceInfo**)&info);
    info->SetCapabilities(IAccessibilityServiceInfo::CAPABILITY_CAN_RETRIEVE_WINDOW_CONTENT);
    Int32 flags;
    info->GetFlags(&flags);
    flags |= IAccessibilityServiceInfo::FLAG_RETRIEVE_INTERACTIVE_WINDOWS;
    flags |= IAccessibilityServiceInfo::FLAG_INCLUDE_NOT_IMPORTANT_VIEWS;
    info->SetFlags(flags);
    AutoPtr<Service> service = new Service();
    service->constructor(IUserHandle::USER_NULL,
            sFakeAccessibilityServiceComponentName, info, (IIAccessibilityManager*)mHost);

    mConnectionId = service->mId;

    AutoPtr<IAccessibilityInteractionClientHelper> helper;
    CAccessibilityInteractionClientHelper::AcquireSingleton((IAccessibilityInteractionClientHelper**)&helper);
    helper->GetInstance((IAccessibilityInteractionClient**)&mClient);
    mClient->AddConnection(mConnectionId, service);

    //TODO: (multi-display) We need to support multiple displays.
    AutoPtr<IInterface> obj;
    mHost->mContext->GetSystemService(IContext::DISPLAY_SERVICE, (IInterface**)&obj);
    AutoPtr<IDisplayManager> displayManager = IDisplayManager::Probe(obj);
    displayManager->GetDisplay(IDisplay::DEFAULT_DISPLAY, (IDisplay**)&mDefaultDisplay);
}

AccessibilityManagerService::InteractionBridge::~InteractionBridge()
{}

void AccessibilityManagerService::InteractionBridge::ClearAccessibilityFocusNotLocked(
    /* [in] */ Int32 windowId)
{
    AutoPtr<IAccessibilityNodeInfo> focus = GetAccessibilityFocusNotLocked(windowId);
    if (focus != NULL) {
        Boolean res;
        focus->PerformAction(
                IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS,
                &res);
    }
}

Boolean AccessibilityManagerService::InteractionBridge::GetAccessibilityFocusClickPointInScreenNotLocked(
    /* [in] */ IPoint* outPoint)
{
    Boolean res;
    AutoPtr<IAccessibilityNodeInfo> focus = GetAccessibilityFocusNotLocked();
    if (focus == NULL) {
        res = FALSE;
    }

    synchronized(mHost->mLock) {
        Int32 wId;
        focus->GetWindowId(&wId);
        Int64 nId;
        focus->GetSourceNodeId(&nId);
        AutoPtr<IPoint> point;
        mClient->ComputeClickPointInScreen(mConnectionId, wId, nId, (IPoint**)&point);

        if (point == NULL) {
            res = FALSE;
        }

        AutoPtr<IMagnificationSpec> spec = mHost->GetCompatibleMagnificationSpecLocked(wId);
        Boolean result;
        if (spec != NULL && (spec->IsNop(&result), !result)) {
            Float offsetX, offsetY;
            spec->GetOffsetX(&offsetX);
            spec->GetOffsetY(&offsetY);
            point->Offset((Int32) -offsetX, (Int32) -offsetY);
            Float scale;
            spec->GetScale(&scale);
            Int32 x, y;
            point->Get(&x, &y);
            point->Set((Int32) (x * (1 / scale)), (Int32) (y * (1 / scale)));
        }

        // Make sure the point is within the window.
        AutoPtr<IRect> windowBounds = mHost->mTempRect;
        mHost->GetActiveWindowBounds(windowBounds);
        Int32 pointX, pointY;
        point->Get(&pointX, &pointY);
        if (windowBounds->Contains(pointX, pointY, &result), !result) {
            res = FALSE;
        }

        // Make sure the point is within the screen.
        AutoPtr<IPoint> screenSize = mHost->mTempPoint;
        mDefaultDisplay->GetRealSize(screenSize);
        Int32 screenSizeX, screenSizeY;
        screenSize->Get(&screenSizeX, &screenSizeY);
        if (pointX < 0 || pointX > screenSizeX
                || pointY < 0 || pointY > screenSizeY) {
            res = FALSE;
        }

        outPoint->Set(pointX, pointY);
        res = TRUE;
    }
    return res;
}

AutoPtr<IAccessibilityNodeInfo> AccessibilityManagerService::InteractionBridge::GetAccessibilityFocusNotLocked()
{
    Int32 focusedWindowId;
    synchronized(mHost->mLock) {
        focusedWindowId = mHost->mSecurityPolicy->mAccessibilityFocusedWindowId;
        if (focusedWindowId == SecurityPolicy::INVALID_WINDOW_ID) {
            return NULL;
        }
    }
    return GetAccessibilityFocusNotLocked(focusedWindowId);
}

AutoPtr<IAccessibilityNodeInfo> AccessibilityManagerService::InteractionBridge::GetAccessibilityFocusNotLocked(
    /* [in] */ Int32 windowId)
{
    AutoPtr<IAccessibilityNodeInfo> info;

    mClient->FindFocus(mConnectionId,
            windowId, IAccessibilityNodeInfo::ROOT_NODE_ID,
            IAccessibilityNodeInfo::FOCUS_ACCESSIBILITY,
            (IAccessibilityNodeInfo**)&info);
    return info;
}

//===============================================================================
//                  AccessibilityManagerService::UserState
//===============================================================================

AccessibilityManagerService::UserState::UserState(
    /* [in] */ Int32 userId,
    /* [in] */ AccessibilityManagerService* host)
    : mUserId(userId)
    , mHandledFeedbackTypes(0)
    , mLastSentClientState(-1)
    , mIsAccessibilityEnabled(FALSE)
    , mIsTouchExplorationEnabled(FALSE)
    , mIsTextHighContrastEnabled(FALSE)
    , mIsEnhancedWebAccessibilityEnabled(FALSE)
    , mIsDisplayMagnificationEnabled(FALSE)
    , mIsFilterKeyEventsEnabled(FALSE)
    , mHasDisplayColorAdjustment(FALSE)
    , mAccessibilityFocusOnlyInActiveWindow(FALSE)
    , mHost(host)
{
    CRemoteCallbackList::New((IRemoteCallbackList**)&mClients);

    CSparseArray::New((ISparseArray**)&mInteractionConnections);
    CSparseArray::New((ISparseArray**)&mWindowTokens);

    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&mBoundServices);

    CHashMap::New((IMap**)&mComponentNameToServiceMap);

    CArrayList::New((IList**)&mInstalledServices);

    CHashSet::New((ISet**)&mBindingServices);
    CHashSet::New((ISet**)&mEnabledServices);
    CHashSet::New((ISet**)&mTouchExplorationGrantedServices);

    mUiAutomationSerivceOnwerDeathRecipient = new UserStateDeathRecipient(this);
}

AccessibilityManagerService::UserState::~UserState()
{}

Int32 AccessibilityManagerService::UserState::GetClientState()
{
    Int32 clientState = 0;
    if (mIsAccessibilityEnabled) {
        clientState |= IAccessibilityManager::STATE_FLAG_ACCESSIBILITY_ENABLED;
    }
    // Touch exploration relies on enabled accessibility.
    if (mIsAccessibilityEnabled && mIsTouchExplorationEnabled) {
        clientState |= IAccessibilityManager::STATE_FLAG_TOUCH_EXPLORATION_ENABLED;
    }
    if (mIsTextHighContrastEnabled) {
        clientState |= IAccessibilityManager::STATE_FLAG_HIGH_TEXT_CONTRAST_ENABLED;
    }
    return clientState;
}

void AccessibilityManagerService::UserState::OnSwitchToAnotherUser()
{
    // Clear UI test automation state.
    if (mUiAutomationService != NULL) {
        mUiAutomationService->ProxyDied();
    }

    // Unbind all services.
    mHost->UnbindAllServicesLocked(this);

    // Clear service management state.
    IList::Probe(mBoundServices)->Clear();
    mBindingServices->Clear();

    // Clear event management state.
    mHandledFeedbackTypes = 0;
    mLastSentClientState = -1;

    // Clear state persisted in settings.
    mEnabledServices->Clear();
    mTouchExplorationGrantedServices->Clear();
    mIsAccessibilityEnabled = FALSE;
    mIsTouchExplorationEnabled = FALSE;
    mIsEnhancedWebAccessibilityEnabled = FALSE;
    mIsDisplayMagnificationEnabled = FALSE;
}

void AccessibilityManagerService::UserState::DestroyUiAutomationService()
{
    mUiAutomationService = NULL;
    mUiAutomationServiceClient = NULL;
    if (mUiAutomationServiceOwner != NULL) {
        IProxy* proxy = (IProxy*)mUiAutomationServiceOwner->Probe(EIID_IProxy);
        Boolean res;
        proxy->UnlinkToDeath(
                mUiAutomationSerivceOnwerDeathRecipient, 0, &res);
        mUiAutomationServiceOwner = NULL;
    }
}

//===============================================================================
//                  AccessibilityManagerService::UserStateDeathRecipient
//===============================================================================

CAR_INTERFACE_IMPL(AccessibilityManagerService::UserStateDeathRecipient, Object, IProxyDeathRecipient);

AccessibilityManagerService::UserStateDeathRecipient::UserStateDeathRecipient(
    /* [in] */ UserState* host)
    : mHost(host)
{}

AccessibilityManagerService::UserStateDeathRecipient::~UserStateDeathRecipient()
{}

ECode AccessibilityManagerService::UserStateDeathRecipient::ProxyDied()
{
    IProxy* proxy = (IProxy*)mHost->mUiAutomationServiceOwner->Probe(EIID_IProxy);
    Boolean res;
    proxy->UnlinkToDeath(
            mHost->mUiAutomationSerivceOnwerDeathRecipient, 0, &res);
    mHost->mUiAutomationServiceOwner = NULL;
    if (mHost->mUiAutomationService != NULL) {
        mHost->mUiAutomationService->ProxyDied();
    }
    return NOERROR;
}

//===============================================================================
//                  AccessibilityManagerService::AccessibilityContentObserver
//===============================================================================

AccessibilityManagerService::AccessibilityContentObserver::AccessibilityContentObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ AccessibilityManagerService* host)
    : mHost(host)
{
    ContentObserver::constructor(handler);

    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);

    settingsSecure->GetUriFor(ISettingsSecure::ACCESSIBILITY_ENABLED,
            (IUri**)&mAccessibilityEnabledUri);

    settingsSecure->GetUriFor(ISettingsSecure::TOUCH_EXPLORATION_ENABLED,
            (IUri**)&mTouchExplorationEnabledUri);

    settingsSecure->GetUriFor(
            ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED,
            (IUri**)&mDisplayMagnificationEnabledUri);

    settingsSecure->GetUriFor(ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES,
            (IUri**)&mEnabledAccessibilityServicesUri);

    settingsSecure->GetUriFor(
            ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
            (IUri**)&mTouchExplorationGrantedAccessibilityServicesUri);

    settingsSecure->GetUriFor(ISettingsSecure::ACCESSIBILITY_SCRIPT_INJECTION,
            (IUri**)&mEnhancedWebAccessibilityUri);

    settingsSecure->GetUriFor(
            ISettingsSecure::ACCESSIBILITY_DISPLAY_INVERSION_ENABLED,
            (IUri**)&mDisplayInversionEnabledUri);

    settingsSecure->GetUriFor(
            ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED,
            (IUri**)&mDisplayDaltonizerEnabledUri);

    settingsSecure->GetUriFor(ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER,
            (IUri**)&mDisplayDaltonizerUri);

    settingsSecure->GetUriFor(
            ISettingsSecure::ACCESSIBILITY_HIGH_TEXT_CONTRAST_ENABLED,
            (IUri**)&mHighTextContrastUri);
}

AccessibilityManagerService::AccessibilityContentObserver::~AccessibilityContentObserver()
{}

void AccessibilityManagerService::AccessibilityContentObserver::Register(
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
    contentResolver->RegisterContentObserver(mEnhancedWebAccessibilityUri,
            FALSE, (IContentObserver*)this, IUserHandle::USER_ALL);
    contentResolver->RegisterContentObserver(
            mDisplayInversionEnabledUri, FALSE, (IContentObserver*)this,
            IUserHandle::USER_ALL);
    contentResolver->RegisterContentObserver(
            mDisplayDaltonizerEnabledUri, FALSE, (IContentObserver*)this,
            IUserHandle::USER_ALL);
    contentResolver->RegisterContentObserver(
            mDisplayDaltonizerUri, FALSE, (IContentObserver*)this,
            IUserHandle::USER_ALL);
    contentResolver->RegisterContentObserver(
            mHighTextContrastUri, FALSE, (IContentObserver*)this,
            IUserHandle::USER_ALL);
}

ECode AccessibilityManagerService::AccessibilityContentObserver::OnChange(
    /* [in] */ Boolean selfChange,
    /* [in] */ IUri* uri)
{
    synchronized(mHost->mLock) {
        // Profiles share the accessibility state of the parent. Therefore,
        // we are checking for changes only the parent settings.
        AutoPtr<UserState> userState = mHost->GetCurrentUserStateLocked();

        // We will update when the automation service dies.
        if (userState->mUiAutomationService != NULL) {
            return NOERROR;
        }

        Boolean res, res1, res2;
        if (IObject::Probe(mAccessibilityEnabledUri)->Equals(uri, &res), res) {
            if (mHost->ReadAccessibilityEnabledSettingLocked(userState)) {
                mHost->OnUserStateChangedLocked(userState);
            }
        }
        else if (IObject::Probe(mTouchExplorationEnabledUri)->Equals(uri,
                &res), res) {
            if (mHost->ReadTouchExplorationEnabledSettingLocked(userState)) {
                mHost->OnUserStateChangedLocked(userState);
            }
        }
        else if (IObject::Probe(mDisplayMagnificationEnabledUri)->Equals(uri,
                &res), res) {
            if (mHost->ReadDisplayMagnificationEnabledSettingLocked(userState)) {
                mHost->OnUserStateChangedLocked(userState);
            }
        }
        else if (IObject::Probe(mEnabledAccessibilityServicesUri)->Equals(uri,
                &res), res) {
            if (mHost->ReadEnabledAccessibilityServicesLocked(userState)) {
                mHost->OnUserStateChangedLocked(userState);
            }
        }
        else if (IObject::Probe(mTouchExplorationGrantedAccessibilityServicesUri)->Equals(
                uri, &res), res) {
            if (mHost->ReadTouchExplorationGrantedAccessibilityServicesLocked(userState)) {
                mHost->OnUserStateChangedLocked(userState);
            }
        }
        else if (IObject::Probe(mEnhancedWebAccessibilityUri)->Equals(uri,
                &res), res) {
            if (mHost->ReadEnhancedWebAccessibilityEnabledChangedLocked(userState)) {
                mHost->OnUserStateChangedLocked(userState);
            }
        }
        else if ((IObject::Probe(mDisplayInversionEnabledUri)->Equals(uri, &res), res)
                || (IObject::Probe(mDisplayDaltonizerEnabledUri)->Equals(uri, &res1), res1)
                || (IObject::Probe(mDisplayDaltonizerUri)->Equals(uri, &res2), res2)) {
            if (mHost->ReadDisplayColorAdjustmentSettingsLocked(userState)) {
                mHost->UpdateDisplayColorAdjustmentSettingsLocked(userState);
            }
        }
        else if (IObject::Probe(mHighTextContrastUri)->Equals(uri, &res), res) {
            if (mHost->ReadHighTextContrastEnabledSettingLocked(userState)) {
                mHost->OnUserStateChangedLocked(userState);
            }
        }
        return NOERROR;
    }

    return NOERROR;
}

//===============================================================================
//                  AccessibilityManagerService::MyPackageMonitor
//===============================================================================

AccessibilityManagerService::MyPackageMonitor::MyPackageMonitor(
    /* [in] */ AccessibilityManagerService* host)
    : mHost(host)
{}

AccessibilityManagerService::MyPackageMonitor::~MyPackageMonitor()
{}

ECode AccessibilityManagerService::MyPackageMonitor::OnSomePackagesChanged()
{
    synchronized(mHost->mLock) {
        // Only the profile parent can install accessibility services.
        // Therefore we ignore packages from linked profiles.
        Int32 userId;
        GetChangingUserId(&userId);
        if (userId != mHost->mCurrentUserId) {
            return NOERROR;
        }

        // We will update when the automation service dies.
        AutoPtr<UserState> userState = mHost->GetCurrentUserStateLocked();
        // We have to reload the installed services since some services may
        // have different attributes, resolve info (does not support equals),
        // etc. Remove them then to force reload. Do it even if automation is
        // running since when it goes away, we will have to reload as well.
        userState->mInstalledServices->Clear();
        if (userState->mUiAutomationService == NULL) {
            if (mHost->ReadConfigurationForUserStateLocked(userState)) {
                mHost->OnUserStateChangedLocked(userState);
            }
        }
        return NOERROR;
    }

    return NOERROR;
}

ECode AccessibilityManagerService::MyPackageMonitor::OnPackageRemoved(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    synchronized(mHost->mLock) {
        Int32 userId;
        GetChangingUserId(&userId);
        // Only the profile parent can install accessibility services.
        // Therefore we ignore packages from linked profiles.
        if (userId != mHost->mCurrentUserId) {
            return NOERROR;
        }

        AutoPtr<UserState> userState = mHost->GetUserStateLocked(userId);
        AutoPtr<IIterator> it;
        userState->mEnabledServices->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IComponentName> comp = IComponentName::Probe(obj);
            String compPkg;
            comp->GetPackageName(&compPkg);
            if (compPkg.Equals(packageName)) {
                it->Remove();

                // Update the enabled services setting.
                mHost->PersistComponentNamesToSettingLocked(
                        ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES,
                        userState->mEnabledServices, userId);
                // Update the touch exploration granted services setting.
                userState->mTouchExplorationGrantedServices->Remove(comp);
                mHost->PersistComponentNamesToSettingLocked(
                        ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
                        userState->mTouchExplorationGrantedServices, userId);
                // We will update when the automation service dies.
                if (userState->mUiAutomationService == NULL) {
                    mHost->OnUserStateChangedLocked(userState);
                }
                return NOERROR;
            }

        }
        return NOERROR;
    }
    return NOERROR;
}

ECode AccessibilityManagerService::MyPackageMonitor::OnHandleForceStop(
    /* [in] */ IIntent* intent,
    /* [in] */ ArrayOf<String>* packages,
    /* [in] */ Int32 uid,
    /* [in] */ Boolean doit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    synchronized(mHost->mLock) {
        Int32 userId;
        GetChangingUserId(&userId);
        // Only the profile parent can install accessibility services.
        // Therefore we ignore packages from linked profiles.
        if (userId != mHost->mCurrentUserId) {
            return NOERROR;
        }

        AutoPtr<UserState> userState = mHost->GetUserStateLocked(userId);
        AutoPtr<IIterator> it;
        userState->mEnabledServices->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            AutoPtr<IComponentName> comp = IComponentName::Probe(obj);
            String compPkg;
            comp->GetPackageName(&compPkg);
            for (Int32 i = 0; i < packages->GetLength(); ++i) {
                if (compPkg.Equals((*packages)[i])) {
                    if (!doit) {
                        *result = TRUE;
                        return NOERROR;
                    }
                    it->Remove();
                    mHost->PersistComponentNamesToSettingLocked(
                            ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES,
                            userState->mEnabledServices, userId);
                    // We will update when the automation service dies.
                    if (userState->mUiAutomationService == NULL) {
                        mHost->OnUserStateChangedLocked(userState);
                    }
                }
            }
        }
        return NOERROR;
    }
    return NOERROR;
}

//===============================================================================
//                  AccessibilityManagerService::UserChangedBroadcastReceiver
//===============================================================================

AccessibilityManagerService::UserChangedBroadcastReceiver::UserChangedBroadcastReceiver(
    /* [in] */ AccessibilityManagerService* host)
    : mHost(host)
{}

ECode AccessibilityManagerService::UserChangedBroadcastReceiver::OnReceive(
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
        // We will update when the automation service dies.
        AutoPtr<UserState> userState = mHost->GetCurrentUserStateLocked();
        if (userState->mUiAutomationService == NULL) {
            if (mHost->ReadConfigurationForUserStateLocked(userState)) {
                mHost->OnUserStateChangedLocked(userState);
            }
        }
    }

    return NOERROR;
}

//===============================================================================
//                  AccessibilityManagerService::OnDialogOKListener
//===============================================================================

CAR_INTERFACE_IMPL(AccessibilityManagerService::OnDialogOKListener, Object, IDialogInterfaceOnClickListener);

AccessibilityManagerService::OnDialogOKListener::OnDialogOKListener(
    /* [in] */ UserState* state,
    /* [in] */ Service* service,
    /* [in] */ AccessibilityManagerService* host)
    : mState(state)
    , mService(service)
    , mHost(host)
{}

AccessibilityManagerService::OnDialogOKListener::~OnDialogOKListener()
{}

ECode AccessibilityManagerService::OnDialogOKListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    // The user allowed the service to toggle touch exploration.
    mState->mTouchExplorationGrantedServices->Add(mService->mComponentName);
    mHost->PersistComponentNamesToSettingLocked(
            ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
            mState->mTouchExplorationGrantedServices, mState->mUserId);
    // Enable touch exploration.
    AutoPtr<UserState> userState = mHost->GetUserStateLocked(mService->mUserId);
    userState->mIsTouchExplorationEnabled = TRUE;
    AutoPtr<IContentResolver> resolver;
    mHost->mContext->GetContentResolver((IContentResolver**)&resolver);
    Boolean result;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->PutInt32ForUser(resolver,
        ISettingsSecure::TOUCH_EXPLORATION_ENABLED, 1, mService->mUserId, &result);

    mHost->OnUserStateChangedLocked(userState);
    return NOERROR;
}

//===============================================================================
//                  AccessibilityManagerService::OnDialogCancelListener
//===============================================================================

CAR_INTERFACE_IMPL(AccessibilityManagerService::OnDialogCancelListener, Object, IDialogInterfaceOnClickListener);

AccessibilityManagerService::OnDialogCancelListener::OnDialogCancelListener()
{}

AccessibilityManagerService::OnDialogCancelListener::~OnDialogCancelListener()
{}

ECode AccessibilityManagerService::OnDialogCancelListener::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    return dialog->Dismiss();
}

//===============================================================================
//                  AccessibilityManagerService
//===============================================================================

CAR_INTERFACE_IMPL_2(AccessibilityManagerService, Object, IIAccessibilityManager, IBinder);

AccessibilityManagerService::AccessibilityManagerService()
    : mHasInputFilter(FALSE)
    , mCurrentUserId(IUserHandle::USER_OWNER)
    , mInitialized(FALSE)
{
}

AccessibilityManagerService::~AccessibilityManagerService()
{}

ECode AccessibilityManagerService::constructor(
    /* [in] */ IContext* context)
{
    mPendingEventPool = new Pools::SimplePool<PendingEvent>(MAX_POOL_SIZE);

    CSimpleStringSplitter::New(COMPONENT_NAME_SEPARATOR, (ISimpleStringSplitter**)&mStringColonSplitter);

    CArrayList::New((IList**)&mEnabledServicesForFeedbackTempList);

    ASSERT_SUCCEEDED(CRegion::New((IRegion**)&mTempRegion));
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mTempRect));
    ASSERT_SUCCEEDED(CPoint::New((IPoint**)&mTempPoint));

    CHashSet::New((ISet**)&mTempComponentNameSet);
    CArrayList::New((IList**)&mTempAccessibilityServiceInfoList);

    CRemoteCallbackList::New((IRemoteCallbackList**)&mGlobalClients);

    CSparseArray::New((ISparseArray**)&mGlobalInteractionConnections);
    CSparseArray::New((ISparseArray**)&mGlobalWindowTokens);
    CSparseArray::New((ISparseArray**)&mUserStates);
    /*  */

    mContext = context;
    mContext->GetPackageManager((IPackageManager**)&mPackageManager);
    AutoPtr<IInterface> obj = LocalServices::GetService(EIID_IWindowManagerInternal);
    mWindowManagerService = IWindowManagerInternal::Probe(obj);

    obj = NULL;
    FAIL_RETURN(context->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj));
    mUserManager = IUserManager::Probe(obj);

    mSecurityPolicy = new SecurityPolicy(this);
    AutoPtr<ILooper> looper;
    FAIL_RETURN(mContext->GetMainLooper((ILooper**)&looper));
    mMainHandler = new MainHandler(looper, this);

    CLockPatternUtils::New(context, (ILockPatternUtils**)&mLockPatternUtils);

    RegisterBroadcastReceivers();
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<AccessibilityContentObserver> observer = new AccessibilityContentObserver(mMainHandler, this);
    observer->Register(cr);
    return NOERROR;
}

AutoPtr<AccessibilityManagerService::UserState> AccessibilityManagerService::GetCurrentUserStateLocked()
{
    return GetUserStateLocked(mCurrentUserId);
}

AutoPtr<AccessibilityManagerService::UserState> AccessibilityManagerService::GetUserStateLocked(
    /* [in] */ Int32 userId)
{
    AutoPtr<IInterface> obj;
    mUserStates->Get(userId, (IInterface**)&obj);
    AutoPtr<UserState> state = (UserState*)IObject::Probe(obj);
    if (state == NULL) {
        state = new UserState(userId, this);
        mUserStates->Put(userId, (IObject*)state);
    }
    return state;
}

void AccessibilityManagerService::RegisterBroadcastReceivers()
{
    AutoPtr<IPackageMonitor> monitor = (IPackageMonitor*)new MyPackageMonitor(this);

    // package changes
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> all;
    helper->GetALL((IUserHandle**)&all);
    monitor->Register(mContext, NULL, all, TRUE);

    // user change and unlock
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_USER_SWITCHED);
    intentFilter->AddAction(IIntent::ACTION_USER_REMOVED);
    intentFilter->AddAction(IIntent::ACTION_USER_PRESENT);

    AutoPtr<IBroadcastReceiver> receiver = (IBroadcastReceiver*)new UserChangedBroadcastReceiver(this);
    AutoPtr<IIntent> sticky;
    mContext->RegisterReceiverAsUser(receiver, all, intentFilter, String(NULL),
            NULL, (IIntent**)&sticky);
}

ECode AccessibilityManagerService::AddClient(
    /* [in] */ IIAccessibilityManagerClient* client,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                userId, &resolvedUserId);
        // If the client is from a process that runs across users such as
        // the system UI or the system we add it to the global state that
        // is shared across users.
        AutoPtr<UserState> userState = GetUserStateLocked(resolvedUserId);
        if (mSecurityPolicy->IsCallerInteractingAcrossUsers(userId)) {
            Boolean res;
            mGlobalClients->Register((IInterface*)client, &res);
            if (DEBUG) {
                Slogger::I(TAG, "Added global client for pid: %d",
                        Binder::GetCallingPid());
            }
            *result = userState->GetClientState();
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
            *result = (resolvedUserId == mCurrentUserId) ? userState->GetClientState() : 0;
        }
        return NOERROR;
    }
    return NOERROR;
}

ECode AccessibilityManagerService::SendAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 userId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution..
        Int32 resolvedUserId;
        mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                userId, &resolvedUserId);
        // This method does nothing for a background user.
        if (resolvedUserId != mCurrentUserId) {
            *result = TRUE;
            return NOERROR; // yes, recycle the event
        }

        if (mSecurityPolicy->CanDispatchAccessibilityEventLocked(event)) {
            Int32 windowId, eventType;
            IAccessibilityRecord::Probe(event)->GetWindowId(&windowId);
            event->GetEventType(&eventType);
            Int64 nId;
            IAccessibilityRecord::Probe(event)->GetSourceNodeId(&nId);
            mSecurityPolicy->UpdateActiveAndAccessibilityFocusedWindowLocked(
                    windowId, nId, eventType);
            mSecurityPolicy->UpdateEventSourceLocked(event);

            NotifyAccessibilityServicesDelayedLocked(event, FALSE);
            NotifyAccessibilityServicesDelayedLocked(event, TRUE);
        }

        if (mHasInputFilter && mInputFilter != NULL) {
            AutoPtr<IAccessibilityEventHelper> helper;
            CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
            AutoPtr<IAccessibilityEvent> outEvent;
            helper->Obtain(event, (IAccessibilityEvent**)&outEvent);

            AutoPtr<IMessage> msg;
            mMainHandler->ObtainMessage(
                    MainHandler::MSG_SEND_ACCESSIBILITY_EVENT_TO_INPUT_FILTER,
                    outEvent, (IMessage**)&msg);
            Boolean result;
            mMainHandler->SendMessage(msg, &result);
        }

        IAccessibilityRecord::Probe(event)->Recycle();
        GetUserStateLocked(resolvedUserId)->mHandledFeedbackTypes = 0;

        *result = (OWN_PROCESS_ID != Binder::GetCallingPid());
        return NOERROR;
    }
    return NOERROR;
}

ECode AccessibilityManagerService::GetInstalledAccessibilityServiceList(
    /* [in] */ Int32 userId,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);

    synchronized(mLock){
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                userId, &resolvedUserId);
        // The automation service is a fake one and should not be reported
        // to clients as being installed - it really is not.
        AutoPtr<UserState> userState = GetUserStateLocked(resolvedUserId);
        if (userState->mUiAutomationService != NULL) {
            AutoPtr<IList> installedServices;
            CArrayList::New((IList**)&installedServices);
            installedServices->AddAll(ICollection::Probe(userState->mInstalledServices));
            installedServices->Remove(userState->mUiAutomationService->mAccessibilityServiceInfo);
            *result = installedServices;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
        *result = userState->mInstalledServices;
        REFCOUNT_ADD(*result);
        return NOERROR;
    }
    return NOERROR;
}

ECode AccessibilityManagerService::GetEnabledAccessibilityServiceList(
    /* [in] */ Int32 feedbackType,
    /* [in] */ Int32 userId,
    /* [out] */ IList** _services)
{
    VALIDATE_NOT_NULL(_services);

    AutoPtr<IList> result;

    synchronized(mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                userId, &resolvedUserId);

        // The automation service is a fake one and should not be reported
        // to clients as being enabled. The automation service is always the
        // only active one, if it exists.
        AutoPtr<UserState> userState = GetUserStateLocked(resolvedUserId);
        if (userState->mUiAutomationService != NULL) {
            AutoPtr<ICollections> coll;
            CCollections::AcquireSingleton((ICollections**)&coll);
            return coll->GetEmptyList(_services);
        }

        result = mEnabledServicesForFeedbackTempList;
        result->Clear();
        AutoPtr<IList> services = IList::Probe(userState->mBoundServices);
        while (feedbackType != 0) {
            using Elastos::Core::Math;
            Int32 feedbackTypeBit = (1 << Math::NumberOfTrailingZeros(feedbackType));
            feedbackType &= ~feedbackTypeBit;
            Int32 serviceCount;
            services->GetSize(&serviceCount);
            for (Int32 i = 0; i < serviceCount; i++) {
                AutoPtr<IInterface> obj;
                services->Get(i, (IInterface**)&obj);
                AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);
                if ((service->mFeedbackType & feedbackTypeBit) != 0) {
                    result->Add(service->mAccessibilityServiceInfo);
                }
            }
        }
    }
    *_services = result;
    REFCOUNT_ADD(*_services);
    return NOERROR;
}

ECode AccessibilityManagerService::Interrupt(
    /* [in] */ Int32 userId)
{
    // CopyOnWriteArrayList<Service> services;
    AutoPtr<ICopyOnWriteArrayList> services;
    synchronized(mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                userId, &resolvedUserId);
        // This method does nothing for a background user.
        if (resolvedUserId != mCurrentUserId) {
            return NOERROR;
        }
        AutoPtr<UserState> userState = GetUserStateLocked(resolvedUserId);
        services = userState->mBoundServices;
    }

    Int32 count;
    IList::Probe(services)->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        IList::Probe(services)->Get(i, (IInterface**)&obj);
        AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);
        // try {
        ECode ec = service->mServiceInterface->OnInterrupt();
        if (FAILED(ec)) {
            Slogger::E(TAG, "Error during sending interrupt request to %p 0x%08x",
                    service->mService.Get(), ec);
        }
        // } catch (RemoteException re) {
        //     Slog.e(TAG, "Error during sending interrupt request to "
        //         + service.mService, re);
        // }
    }

    return NOERROR;
}

ECode AccessibilityManagerService::AddAccessibilityInteractionConnection(
    /* [in] */ IIWindow* windowToken,
    /* [in] */ IIAccessibilityInteractionConnection* connection,
    /* [in] */ Int32 userId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    assert(connection != NULL);

    synchronized(mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                userId, &resolvedUserId);
        Int32 windowId = sNextWindowId++;
        // If the window is from a process that runs across users such as
        // the system UI or the system we add it to the global state that
        // is shared across users.
        if (mSecurityPolicy->IsCallerInteractingAcrossUsers(userId)) {
            AutoPtr<AccessibilityConnectionWrapper> wrapper = new AccessibilityConnectionWrapper(
                    windowId, connection, IUserHandle::USER_ALL, this);
            wrapper->LinkToDeath();
            mGlobalInteractionConnections->Put(windowId, (IProxyDeathRecipient*)wrapper);
            mGlobalWindowTokens->Put(windowId, IBinder::Probe(windowToken));
            if (DEBUG) {
                Slogger::I(TAG, "Added global connection for pid: %d with windowId:%d and  token: %p",
                    Binder::GetCallingPid(), windowId, IBinder::Probe(windowToken));
            }
        }
        else {
            AutoPtr<AccessibilityConnectionWrapper> wrapper = new AccessibilityConnectionWrapper(
                windowId, connection, resolvedUserId, this);
            wrapper->LinkToDeath();
            AutoPtr<UserState> userState = GetUserStateLocked(resolvedUserId);
            userState->mInteractionConnections->Put(windowId, (IProxyDeathRecipient*)wrapper);
            userState->mWindowTokens->Put(windowId, IBinder::Probe(windowToken));
            if (DEBUG) {
                Slogger::I(TAG, "Added user connection for pid:%d with windowId: %d and userId:%d and  token: %p",
                    Binder::GetCallingPid(), windowId, mCurrentUserId, IBinder::Probe(windowToken));
            }
        }
        *result = windowId;
        return NOERROR;
    }
    return NOERROR;
}

ECode AccessibilityManagerService::RemoveAccessibilityInteractionConnection(
    /* [in] */ IIWindow* window)
{
    synchronized(mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 result;
        mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                UserHandle::GetCallingUserId(), &result);
        AutoPtr<IBinder> token = IBinder::Probe(window);
        Int32 removedWindowId = RemoveAccessibilityInteractionConnectionInternalLocked(
                token, mGlobalWindowTokens, mGlobalInteractionConnections);
        if (removedWindowId >= 0) {
            if (DEBUG) {
                Slogger::I(TAG, "Removed global connection for pid: %d with windowId: %d  and token: ",
                    Binder::GetCallingPid(), removedWindowId, token.Get());
            }
            return NOERROR;
        }

        Int32 userCount;
        mUserStates->GetSize(&userCount);
        for (Int32 i = 0; i < userCount; i++) {
            AutoPtr<IInterface> obj;
            mUserStates->ValueAt(i, (IInterface**)&obj);
            AutoPtr<UserState> userState = (UserState*)IObject::Probe(obj);
            Int32 removedWindowIdForUser =
                    RemoveAccessibilityInteractionConnectionInternalLocked(
                        token, userState->mWindowTokens, userState->mInteractionConnections);
            if (removedWindowIdForUser >= 0) {
                if (DEBUG) {
                    Int32 key;
                    mUserStates->KeyAt(i, &key);
                    Slogger::I(TAG, "Removed user connection for pid: %d  with windowId: %d and userId: %d and token: ",
                        Binder::GetCallingPid(), removedWindowIdForUser, key, token.Get());
                }
                return NOERROR;
            }
        }
    }
    return NOERROR;
}

Int32 AccessibilityManagerService::RemoveAccessibilityInteractionConnectionInternalLocked(
    /* [in] */ IBinder* windowToken,
    /* [in] */ ISparseArray* windowTokens,
    /* [in] */ ISparseArray* interactionConnections)
{
    Int32 count;
    windowTokens->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        windowTokens->ValueAt(i, (IInterface**)&obj);
        if (TO_IINTERFACE(obj) == TO_IINTERFACE(windowToken)) {
            Int32 windowId;
            windowTokens->KeyAt(i, &windowId);
            windowTokens->RemoveAt(i);
            AutoPtr<IInterface> other;
            interactionConnections->Get(windowId, (IInterface**)&other);
            AutoPtr<AccessibilityConnectionWrapper> wrapper = (AccessibilityConnectionWrapper*)IProxyDeathRecipient::Probe(other);
            wrapper->UnlinkToDeath();
            interactionConnections->Remove(windowId);
            return windowId;
        }
    }
    return -1;
}

ECode AccessibilityManagerService::RegisterUiTestAutomationService(
    /* [in] */ IBinder* owner,
    /* [in] */ IIAccessibilityServiceClient* serviceClient,
    /* [in] */ IAccessibilityServiceInfo* accessibilityServiceInfo)
{
    FAIL_RETURN(mSecurityPolicy->EnforceCallingPermission(
            Elastos::Droid::Manifest::permission::RETRIEVE_WINDOW_CONTENT,
            FUNCTION_REGISTER_UI_TEST_AUTOMATION_SERVICE));

    accessibilityServiceInfo->SetComponentName(sFakeAccessibilityServiceComponentName);

    synchronized(mLock) {
        AutoPtr<UserState> userState = GetCurrentUserStateLocked();

        if (userState->mUiAutomationService != NULL) {
            Slogger::E(TAG, "UiAutomationService %p already registered!", serviceClient);
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("UiAutomationService " + serviceClient
            //         + "already registered!");
        }

        // try {
        IProxy* proxy = (IProxy*)owner->Probe(EIID_IProxy);
        ECode ec = proxy->LinkToDeath(userState->mUiAutomationSerivceOnwerDeathRecipient, 0);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Couldn't register for the death of a 0X%08x UiTestAutomationService!", ec);
            return ec;
        }
        // } catch (RemoteException re) {
        //     Slog.e(TAG, "Couldn't register for the death of a"
        //             + " UiTestAutomationService!", re);
        //     return;
        // }

        userState->mUiAutomationServiceOwner = owner;
        userState->mUiAutomationServiceClient = serviceClient;

        // Set the temporary state.
        userState->mIsAccessibilityEnabled = TRUE;
        userState->mIsTouchExplorationEnabled = FALSE;
        userState->mIsEnhancedWebAccessibilityEnabled = FALSE;
        userState->mIsDisplayMagnificationEnabled = FALSE;
        userState->mInstalledServices->Add(accessibilityServiceInfo);
        userState->mEnabledServices->Clear();
        userState->mEnabledServices->Add(sFakeAccessibilityServiceComponentName);
        userState->mTouchExplorationGrantedServices->Add(sFakeAccessibilityServiceComponentName);

        // Use the new state instead of settings.
        OnUserStateChangedLocked(userState);
    }

    return NOERROR;
}

ECode AccessibilityManagerService::UnregisterUiTestAutomationService(
    /* [in] */ IIAccessibilityServiceClient* serviceClient)
{
    synchronized(mLock) {
        AutoPtr<UserState> userState = GetCurrentUserStateLocked();
        // Automation service is not bound, so pretend it died to perform clean up.
        if (userState->mUiAutomationService != NULL
                && serviceClient != NULL
                && userState->mUiAutomationService->mServiceInterface != NULL
                && IBinder::Probe(userState->mUiAutomationService->mServiceInterface)
                == IBinder::Probe(serviceClient)) {
            userState->mUiAutomationService->ProxyDied();
        }
        else {
            Slogger::E(TAG, "UiAutomationService %p not registered!", serviceClient);
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("UiAutomationService " + serviceClient
            //         + " not registered!");
        }
    }
    return NOERROR;
}

ECode AccessibilityManagerService::TemporaryEnableAccessibilityStateUntilKeyguardRemoved(
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

    synchronized(mLock) {
        // Set the temporary state.
        AutoPtr<UserState> userState = GetCurrentUserStateLocked();

        // This is a nop if UI automation is enabled.
        if (userState->mUiAutomationService != NULL) {
            return NOERROR;
        }

        userState->mIsAccessibilityEnabled = TRUE;
        userState->mIsTouchExplorationEnabled= touchExplorationEnabled;
        userState->mIsEnhancedWebAccessibilityEnabled = FALSE;
        userState->mIsDisplayMagnificationEnabled = FALSE;
        userState->mEnabledServices->Clear();
        userState->mEnabledServices->Add(service);
        userState->mBindingServices->Clear();
        userState->mTouchExplorationGrantedServices->Clear();
        userState->mTouchExplorationGrantedServices->Add(service);

        // User the current state instead settings.
        OnUserStateChangedLocked(userState);
    }
    return NOERROR;
}

ECode AccessibilityManagerService::GetWindowToken(
    /* [in] */ Int32 windowId,
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder);
    *binder = NULL;

    mSecurityPolicy->EnforceCallingPermission(
            Elastos::Droid::Manifest::permission::RETRIEVE_WINDOW_TOKEN,
            GET_WINDOW_TOKEN);
    synchronized(mLock) {
        // We treat calls from a profile as if made by its parent as profiles
        // share the accessibility state of the parent. The call below
        // performs the current profile parent resolution.
        Int32 resolvedUserId;
        mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(
                UserHandle::GetCallingUserId(), &resolvedUserId);
        if (resolvedUserId != mCurrentUserId) {
            return NOERROR;
        }
        if (mSecurityPolicy->FindWindowById(windowId) == NULL) {
            return NOERROR;
        }
        AutoPtr<IInterface> obj;
        mGlobalWindowTokens->Get(windowId, (IInterface**)&obj);
        AutoPtr<IBinder> token = IBinder::Probe(obj);
        if (token != NULL) {
            *binder = token;
            REFCOUNT_ADD(*binder);
            return NOERROR;
        }

        obj = NULL;
        GetCurrentUserStateLocked()->mWindowTokens->Get(windowId, (IInterface**)&obj);
        AutoPtr<IBinder> _token = IBinder::Probe(obj);
        *binder = _token;
        REFCOUNT_ADD(*binder);
        return NOERROR;
    }
    return NOERROR;
}

Boolean AccessibilityManagerService::OnGesture(
    /* [in] */ Int32 gestureId)
{
    Boolean res;
    synchronized(mLock) {
        Boolean handled = NotifyGestureLocked(gestureId, FALSE);
        if (!handled) {
            handled = NotifyGestureLocked(gestureId, TRUE);
        }
        res = handled;
    }
    return res;
}

Boolean AccessibilityManagerService::NotifyKeyEvent(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags)
{
    Boolean res;
    synchronized(mLock) {
        AutoPtr<IKeyEventHelper> helper;
        CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);
        AutoPtr<IKeyEvent> localClone;
        helper->Obtain(event, (IKeyEvent**)&localClone);
        Boolean handled = NotifyKeyEventLocked(localClone, policyFlags, FALSE);
        if (!handled) {
            handled = NotifyKeyEventLocked(localClone, policyFlags, TRUE);
        }
        res = handled;
    }
    return res;
}

Boolean AccessibilityManagerService::GetAccessibilityFocusClickPointInScreen(
    /* [in] */ IPoint* outPoint)
{
    return GetInteractionBridgeLocked()->GetAccessibilityFocusClickPointInScreenNotLocked(outPoint);
}

Boolean AccessibilityManagerService::GetActiveWindowBounds(
    /* [in] */ IRect* outBounds)
{
    // TODO: This should be refactored to work with accessibility
    // focus in multiple windows.
    AutoPtr<IBinder> token;
    synchronized(mLock) {
        Int32 windowId = mSecurityPolicy->mActiveWindowId;
        AutoPtr<IInterface> obj;
        mGlobalWindowTokens->Get(windowId, (IInterface**)&obj);
        token = IBinder::Probe(obj);
        if (token == NULL) {
            obj = NULL;
            GetCurrentUserStateLocked()->mWindowTokens->Get(windowId, (IInterface**)&obj);
            token = IBinder::Probe(obj);
        }
    }
    mWindowManagerService->GetWindowFrame(token, outBounds);
    Boolean res;
    if (outBounds->IsEmpty(&res), !res) {
        return TRUE;
    }
    return FALSE;
}

Boolean AccessibilityManagerService::AccessibilityFocusOnlyInActiveWindow()
{
    Boolean res;
    synchronized(mLock) {
        res = mWindowsForAccessibilityCallback == NULL;
    }
    return res;
}

Int32 AccessibilityManagerService::GetActiveWindowId()
{
    return mSecurityPolicy->GetActiveWindowId();
}

void AccessibilityManagerService::OnTouchInteractionStart()
{
    return mSecurityPolicy->OnTouchInteractionStart();
}

void AccessibilityManagerService::OnTouchInteractionEnd()
{
    return mSecurityPolicy->OnTouchInteractionEnd();
}

void AccessibilityManagerService::OnMagnificationStateChanged()
{
    NotifyClearAccessibilityCacheLocked();
}

void AccessibilityManagerService::SwitchUser(
    /* [in] */ Int32 userId)
{
    synchronized(mLock) {
        if (mCurrentUserId == userId && mInitialized) {
            return ;
        }

        // Disconnect from services for the old user.
        AutoPtr<UserState> oldUserState = GetCurrentUserStateLocked();
        oldUserState->OnSwitchToAnotherUser();

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
        AutoPtr<IInterface> obj;
        ASSERT_SUCCEEDED(mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj));
        AutoPtr<IUserManager> userManager = IUserManager::Probe(obj);
        Int32 size = 0;
        if (userManager != NULL) {
            AutoPtr<IList> users;
            ASSERT_SUCCEEDED(userManager->GetUsers((IList**)&users));
            users->GetSize(&size);
        }

        Boolean announceNewUser = size > 1;

        // The user changed.
        mCurrentUserId = userId;

        AutoPtr<UserState> userState = GetCurrentUserStateLocked();
        if (userState->mUiAutomationService != NULL) {
            // Switching users disables the UI automation service.
            userState->mUiAutomationService->ProxyDied();
        }

        ReadConfigurationForUserStateLocked(userState);
        // Even if reading did not yield change, we have to update
        // the state since the context in which the current user
        // state was used has changed since it was inactive.
        OnUserStateChangedLocked(userState);

        if (announceNewUser) {
            // Schedule announcement of the current user if needed.
            Boolean result;
            mMainHandler->SendEmptyMessageDelayed(
                    MainHandler::MSG_ANNOUNCE_NEW_USER_IF_NEEDED,
                    WAIT_FOR_USER_STATE_FULLY_INITIALIZED_MILLIS, &result);
        }
    }
}

void AccessibilityManagerService::RemoveUser(
    /* [in] */ Int32 userId)
{
    synchronized(mLock) {
        mUserStates->Remove(userId);
    }
}

AutoPtr<AccessibilityManagerService::InteractionBridge> AccessibilityManagerService::GetInteractionBridgeLocked()
{
    if (mInteractionBridge == NULL) {
        mInteractionBridge = new InteractionBridge(this);
    }
    return mInteractionBridge;
}

Boolean AccessibilityManagerService::NotifyGestureLocked(
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
    Int32 size;
    IList::Probe(state->mBoundServices)->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        IList::Probe(state->mBoundServices)->Get(i, (IInterface**)&obj);
        AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);
        if (service->mRequestTouchExplorationMode && service->mIsDefault == isDefault) {
            service->NotifyGesture(gestureId);
            return TRUE;
        }
    }
    return FALSE;
}

Boolean AccessibilityManagerService::NotifyKeyEventLocked(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags,
    /* [in] */ Boolean isDefault)
{
    // TODO: Now we are giving the key events to the last enabled
    //       service that can handle them Ideally, the user should
    //       make the call which service handles key events. However,
    //       only one service should handle key events to avoid user
    //       frustration when different behavior is observed from
    //       different combinations of enabled accessibility services.
    AutoPtr<UserState> state = GetCurrentUserStateLocked();
    Int32 size;
    IList::Probe(state->mBoundServices)->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        IList::Probe(state->mBoundServices)->Get(i, (IInterface**)&obj);
        AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);
        // Key events are handled only by services that declared
        // this capability and requested to filter key events.
        Int32 cap;
        if (!service->mRequestFilterKeyEvents ||
                ((service->mAccessibilityServiceInfo->GetCapabilities(&cap), cap)
                    & IAccessibilityServiceInfo::CAPABILITY_CAN_REQUEST_FILTER_KEY_EVENTS) == 0) {
            continue;
        }
        if (service->mIsDefault == isDefault) {
            service->NotifyKeyEvent(event, policyFlags);
            return TRUE;
        }
    }
    return FALSE;
}

void AccessibilityManagerService::NotifyClearAccessibilityCacheLocked()
{
    AutoPtr<UserState> state = GetCurrentUserStateLocked();
    Int32 size;
    IList::Probe(state->mBoundServices)->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> obj;
        IList::Probe(state->mBoundServices)->Get(i, (IInterface**)&obj);
        AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);
        service->NotifyClearAccessibilityNodeInfoCache();
    }
}

void AccessibilityManagerService::RemoveAccessibilityInteractionConnectionLocked(
    /* [in] */ Int32 windowId,
    /* [in] */ Int32 userId)
{
    if (userId == IUserHandle::USER_ALL) {
        mGlobalWindowTokens->Remove(windowId);
        mGlobalInteractionConnections->Remove(windowId);
    }
    else {
        AutoPtr<UserState> userState = GetCurrentUserStateLocked();
        userState->mWindowTokens->Remove(windowId);
        userState->mInteractionConnections->Remove(windowId);
    }
    if (DEBUG) {
        Slogger::I(TAG, "Removing interaction connection to windowId: %d", windowId);
    }
}

Boolean AccessibilityManagerService::ReadInstalledAccessibilityServiceLocked(
    /* [in] */ AccessibilityManagerService::UserState* userState)
{
    mTempAccessibilityServiceInfoList->Clear();

    AutoPtr<IList> installedServices;
    AutoPtr<IIntent> intent;
    CIntent::New(IAccessibilityService::SERVICE_INTERFACE, (IIntent**)&intent);
    ASSERT_SUCCEEDED(mPackageManager->QueryIntentServicesAsUser(intent,
            IPackageManager::GET_SERVICES | IPackageManager::GET_META_DATA,
            mCurrentUserId, (IList**)&installedServices));

    Int32 count;
    installedServices->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        installedServices->Get(i, (IInterface**)&obj);
        AutoPtr<IResolveInfo> resolveInfo = IResolveInfo::Probe(obj);
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
        }
        mTempAccessibilityServiceInfoList->Add(accessibilityServiceInfo);
        // } catch (XmlPullParserException | IOException xppe) {
        //     Slogger::E(TAG, "Error while initializing AccessibilityServiceInfo", xppe);
    }

    Boolean res;
    if (mTempAccessibilityServiceInfoList->Equals(userState->mInstalledServices, &res), !res) {
        userState->mInstalledServices->Clear();
        userState->mInstalledServices->AddAll(ICollection::Probe(mTempAccessibilityServiceInfoList));
        mTempAccessibilityServiceInfoList->Clear();
        return TRUE;
    }

    mTempAccessibilityServiceInfoList->Clear();
    return FALSE;
}

Boolean AccessibilityManagerService::ReadEnabledAccessibilityServicesLocked(
    /* [in] */ UserState* userState)
{
    mTempComponentNameSet->Clear();
    ReadComponentNamesFromSettingLocked(ISettingsSecure::ENABLED_ACCESSIBILITY_SERVICES,
            userState->mUserId, mTempComponentNameSet);
    Boolean res;
    if (mTempComponentNameSet->Equals(userState->mEnabledServices, &res), !res) {
        userState->mEnabledServices->Clear();
        userState->mEnabledServices->AddAll(ICollection::Probe(mTempComponentNameSet));
        mTempComponentNameSet->Clear();
        return TRUE;
    }
    mTempComponentNameSet->Clear();
    return FALSE;
}

Boolean AccessibilityManagerService::ReadTouchExplorationGrantedAccessibilityServicesLocked(
    /* [in] */ UserState* userState)
{
    mTempComponentNameSet->Clear();
    ReadComponentNamesFromSettingLocked(
            ISettingsSecure::TOUCH_EXPLORATION_GRANTED_ACCESSIBILITY_SERVICES,
            userState->mUserId, mTempComponentNameSet);
    Boolean res;
    if (mTempComponentNameSet->Equals(userState->mTouchExplorationGrantedServices, &res), !res) {
        userState->mTouchExplorationGrantedServices->Clear();
        userState->mTouchExplorationGrantedServices->AddAll(ICollection::Probe(mTempComponentNameSet));
        mTempComponentNameSet->Clear();
        return TRUE;
    }
    mTempComponentNameSet->Clear();
    return FALSE;
}

void AccessibilityManagerService::NotifyAccessibilityServicesDelayedLocked(
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Boolean isDefault)
{
    // try {
    AutoPtr<UserState> state = GetCurrentUserStateLocked();
    Int32 count;
    IList::Probe(state->mBoundServices)->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        IList::Probe(state->mBoundServices)->Get(i, (IInterface**)&obj);
        AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);

        if (service->mIsDefault == isDefault) {
            if (CanDispatchEventToServiceLocked(service, event, state->mHandledFeedbackTypes)) {
                state->mHandledFeedbackTypes |= service->mFeedbackType;
                service->NotifyAccessibilityEvent(event);
            }
        }
    }
    // } catch (IndexOutOfBoundsException oobe) {
    //     // An out of bounds exception can happen if services are going away
    //     // as the for loop is running. If that happens, just bail because
    //     // there are no more services to notify.
    // }
}

void AccessibilityManagerService::AddServiceLocked(
    /* [in] */ Service* service,
    /* [in] */ UserState* userState)
{
    // try {
    service->LinkToOwnDeathLocked();
    IList::Probe(userState->mBoundServices)->Add((IIAccessibilityServiceConnection*)service);
    userState->mComponentNameToServiceMap->Put(service->mComponentName,
            (IIAccessibilityServiceConnection*)service);
    // } catch (RemoteException e) {
    //     /* do nothing */
    // }
}

void AccessibilityManagerService::RemoveServiceLocked(
    /* [in] */ Service* service,
    /* [in] */ UserState* userState)
{
    IList::Probe(userState->mBoundServices)->Remove((IIAccessibilityServiceConnection*)service);
    userState->mComponentNameToServiceMap->Remove(service->mComponentName);
    service->UnlinkToOwnDeathLocked();
}

Boolean AccessibilityManagerService::CanDispatchEventToServiceLocked(
    /* [in] */ Service* service,
    /* [in] */ IAccessibilityEvent* event,
    /* [in] */ Int32 handledFeedbackTypes)
{
    if (!service->CanReceiveEventsLocked()) {
        return FALSE;
    }

    Int32 wId;
    IAccessibilityRecord::Probe(event)->GetWindowId(&wId);
    Boolean result;
    if (wId != WINDOW_ID_UNKNOWN
            && (IAccessibilityRecord::Probe(event)->IsImportantForAccessibility(&result), !result)
            && (service->mFetchFlags
                & IAccessibilityNodeInfo::FLAG_INCLUDE_NOT_IMPORTANT_VIEWS) == 0) {
        return FALSE;
    }

    Int32 eventType;
    event->GetEventType(&eventType);
    if ((service->mEventTypes & eventType) != eventType) {
        return FALSE;
    }

    AutoPtr<ISet> packageNames = service->mPackageNames;

    AutoPtr<ICharSequence> csq;
    event->GetPackageName((ICharSequence**)&csq);
    String packageName;
    if (csq != NULL) {
        csq->ToString(&packageName);
    }

    Boolean res;
    if ((packageNames->IsEmpty(&result), result) ||
            (packageNames->Contains(CoreUtils::Convert(packageName), &res), res)) {
        Int32 feedbackType = service->mFeedbackType;
        if ((handledFeedbackTypes & feedbackType) != feedbackType
            || feedbackType == IAccessibilityServiceInfo::FEEDBACK_GENERIC) {
            return TRUE;
        }
    }
    return FALSE;
}

void AccessibilityManagerService::UnbindAllServicesLocked(
    /* [in] */ UserState* userState)
{
    AutoPtr<IList> services = IList::Probe(userState->mBoundServices);
    Int32 count;
    services->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        services->Get(i, (IInterface**)&obj);
        AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);
        if (service->UnbindLocked()) {
            i--;
            count--;
        }
    }
}

void AccessibilityManagerService::ReadComponentNamesFromSettingLocked(
    /* [in] */ const String& settingName,
    /* [in] */ Int32 userId,
    /* [in] */ ISet* outComponentNames)
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
            splitter->GetNext(&str);
            if (str.IsNullOrEmpty()) {
                continue;
            }

            AutoPtr<IComponentName> enabledService;
            helper->UnflattenFromString(str, (IComponentName**)&enabledService);
            if (enabledService != NULL) {
                outComponentNames->Add(enabledService);
            }
        }
    }
}

void AccessibilityManagerService::PersistComponentNamesToSettingLocked(
    /* [in] */ const String& settingName,
    /* [in] */ ISet* componentNames,
    /* [in] */ Int32 userId)
{
    StringBuilder builder;
    AutoPtr<IIterator> it;
    componentNames->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IComponentName> componentName = IComponentName::Probe(obj);
        if (builder.GetByteCount() > 0) {
            builder.AppendChar(COMPONENT_NAME_SEPARATOR);
        }
        String s;
        componentName->FlattenToShortString(&s);
        builder.Append(s);
    }

    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    Boolean result;
    settingsSecure->PutStringForUser(resolver,
            settingName, builder.ToString(), userId, &result);
}

void AccessibilityManagerService::ManageServicesLocked(
    /* [in] */ UserState* userState)
{
    AutoPtr<IMap> componentNameToServiceMap = userState->mComponentNameToServiceMap;
    Boolean isEnabled = userState->mIsAccessibilityEnabled;

    Int32 count;
    userState->mInstalledServices->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        userState->mInstalledServices->Get(i, (IInterface**)&obj);
        AutoPtr<IAccessibilityServiceInfo> installedService =
                IAccessibilityServiceInfo::Probe(obj);

        String id;
        installedService->GetId(&id);

        AutoPtr<IComponentNameHelper> helper;
        CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
        AutoPtr<IComponentName> componentName;
        helper->UnflattenFromString(id, (IComponentName**)&componentName);

        obj = NULL;
        componentNameToServiceMap->Get(componentName, (IInterface**)&obj);
        AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);

        if (isEnabled) {
            // Wait for the binding if it is in process.
            Boolean result;
            if (userState->mBindingServices->Contains(componentName, &result), result) {
                continue;
            }
            if (userState->mEnabledServices->Contains(componentName, &result), result) {
                if (service == NULL) {
                    service = new Service();
                    service->constructor(userState->mUserId, componentName,
                            installedService, (IIAccessibilityManager*)this);
                }
                else if (IList::Probe(userState->mBoundServices)->Contains(
                        (IIAccessibilityServiceConnection*)service, &result), result) {
                    continue;
                }
                service->BindLocked();
            }
            else {
                if (service != NULL) {
                    service->UnbindLocked();
                }
            }
        }
        else {
            if (service != NULL) {
                service->UnbindLocked();
            }
            else {
                userState->mBindingServices->Remove(componentName);
            }
        }
    }

    // No enabled installed services => disable accessibility to avoid
    // sending accessibility events with no recipient across processes.
    Boolean res1, res2;
    if (isEnabled && (IList::Probe(userState->mBoundServices)->IsEmpty(&res1), res1)
            && (userState->mBindingServices->IsEmpty(&res2), res2)) {
        userState->mIsAccessibilityEnabled = FALSE;

        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        Boolean result;
        settingsSecure->PutStringForUser(resolver,
                ISettingsSecure::ACCESSIBILITY_ENABLED,
                StringUtils::ToString(0), userState->mUserId, &result);
    }
}

void AccessibilityManagerService::ScheduleUpdateClientsIfNeededLocked(
    /* [in] */ UserState* userState)
{
    Int32 clientState = userState->GetClientState();

    Int32 count1, count2;
    mGlobalClients->GetRegisteredCallbackCount(&count1);
    if (userState->mLastSentClientState != clientState
            && (count1 > 0 || (userState->mClients->GetRegisteredCallbackCount(
                &count2), count2 > 0))) {
        userState->mLastSentClientState = clientState;
        AutoPtr<IMessage> msg;
        mMainHandler->ObtainMessage(MainHandler::MSG_SEND_STATE_TO_CLIENTS,
                clientState, userState->mUserId, (IMessage**)&msg);
        msg->SendToTarget();
    }
}

void AccessibilityManagerService::ScheduleUpdateInputFilter(
    /* [in] */ UserState* userState)
{
    AutoPtr<IMessage> msg;
    mMainHandler->ObtainMessage(MainHandler::MSG_UPDATE_INPUT_FILTER,
            (IObject*)userState, (IMessage**)&msg);
    msg->SendToTarget();
}

void AccessibilityManagerService::UpdateInputFilter(
    /* [in] */ UserState* userState)
{
    Boolean setInputFilter = FALSE;
    AutoPtr<AccessibilityInputFilter> inputFilter;

    synchronized(mLock) {
        Int32 flags = 0;
        if (userState->mIsDisplayMagnificationEnabled) {
            flags |= AccessibilityInputFilter::FLAG_FEATURE_SCREEN_MAGNIFIER;
        }
        // Touch exploration without accessibility makes no sense.
        if (userState->mIsAccessibilityEnabled
                && userState->mIsTouchExplorationEnabled) {
            flags |= AccessibilityInputFilter::FLAG_FEATURE_TOUCH_EXPLORATION;
        }
        if (userState->mIsFilterKeyEventsEnabled) {
            flags |= AccessibilityInputFilter::FLAG_FEATURE_FILTER_KEY_EVENTS;
        }
        if (flags != 0) {
            if (!mHasInputFilter) {
                mHasInputFilter = TRUE;
                if (mInputFilter == NULL) {
                    mInputFilter = new AccessibilityInputFilter();
                    mInputFilter->constructor(mContext, (IIAccessibilityManager*)this);
                }
                inputFilter = mInputFilter;
                setInputFilter = TRUE;
            }
            mInputFilter->SetEnabledFeatures(flags);
        }
        else {
            if (mHasInputFilter) {
                mHasInputFilter = FALSE;
                mInputFilter->DisableFeatures();
                inputFilter = NULL;
                setInputFilter = TRUE;
            }
        }
    }

    if (setInputFilter) {
        mWindowManagerService->SetInputFilter(inputFilter);
    }
}

void AccessibilityManagerService::ShowEnableTouchExplorationDialog(
    /* [in] */ Service* service)
{
    synchronized(mLock) {
        AutoPtr<IPackageManager> packageManager;
        mContext->GetPackageManager((IPackageManager**)&packageManager);
        AutoPtr<ICharSequence> csq;
        service->mResolveInfo->LoadLabel(packageManager, (ICharSequence**)&csq);
        String label;
        csq->ToString(&label);
        AutoPtr<UserState> state = GetCurrentUserStateLocked();
        if (state->mIsTouchExplorationEnabled) {
            return;
        }
        Boolean isShowing;
        if (mEnableTouchExplorationDialog != NULL
            && (IDialog::Probe(mEnableTouchExplorationDialog)->IsShowing(&isShowing), isShowing)) {
            return;
        }
        AutoPtr<IAlertDialogBuilder> dialogBuilder;
        CAlertDialogBuilder::New(mContext, (IAlertDialogBuilder**)&dialogBuilder);
        dialogBuilder->SetIconAttribute(R::attr::alertDialogIcon);
        AutoPtr<IDialogInterfaceOnClickListener> listener_ok = new OnDialogOKListener(state, service, this);
        dialogBuilder->SetPositiveButton(R::string::ok, listener_ok);
        AutoPtr<IDialogInterfaceOnClickListener> listener_cancel = new OnDialogCancelListener();
        dialogBuilder->SetNegativeButton(R::string::cancel, listener_cancel);
        dialogBuilder->SetTitle(R::string::enable_explore_by_touch_warning_title);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, (IInterface*)csq.Get());
        String str;
        mContext->GetString(R::string::enable_explore_by_touch_warning_message, args, &str);
        dialogBuilder->SetMessage(CoreUtils::Convert(str));
        mEnableTouchExplorationDialog = NULL;
        dialogBuilder->Create((IAlertDialog**)&mEnableTouchExplorationDialog);

        AutoPtr<IWindow> win;
        IDialog::Probe(mEnableTouchExplorationDialog)->GetWindow((IWindow**)&win);
        win->SetType(IWindowManagerLayoutParams::TYPE_SYSTEM_ALERT);
        AutoPtr<IWindowManagerLayoutParams> attrs;
        win->GetAttributes((IWindowManagerLayoutParams**)&attrs);
        Int32 privateFlags;
        attrs->GetPrivateFlags(&privateFlags);
        privateFlags |= IWindowManagerLayoutParams::PRIVATE_FLAG_SHOW_FOR_ALL_USERS;
        attrs->SetPrivateFlags(privateFlags);
        IDialog::Probe(mEnableTouchExplorationDialog)->SetCanceledOnTouchOutside(TRUE);
        IDialog::Probe(mEnableTouchExplorationDialog)->Show();
    }
}

void AccessibilityManagerService::OnUserStateChangedLocked(
    /* [in] */ UserState* userState)
{
    // TODO: Remove this hack
    mInitialized = TRUE;
    UpdateLegacyCapabilitiesLocked(userState);
    UpdateServicesLocked(userState);
    UpdateWindowsForAccessibilityCallbackLocked(userState);
    UpdateAccessibilityFocusBehaviorLocked(userState);
    UpdateFilterKeyEventsLocked(userState);
    UpdateTouchExplorationLocked(userState);
    UpdateEnhancedWebAccessibilityLocked(userState);
    UpdateDisplayColorAdjustmentSettingsLocked(userState);
    ScheduleUpdateInputFilter(userState);
    ScheduleUpdateClientsIfNeededLocked(userState);
}

void AccessibilityManagerService::UpdateAccessibilityFocusBehaviorLocked(
    /* [in] */ UserState* userState)
{
    // If there is no service that can operate with interactive windows
    // then we keep the old behavior where a window loses accessibility
    // focus if it is no longer active. This still changes the behavior
    // for services that do not operate with interactive windows and run
    // at the same time as the one(s) which does. In practice however,
    // there is only one service that uses accessibility focus and it
    // is typically the one that operates with interactive windows, So,
    // this is fine. Note that to allow a service to work across windows
    // we have to allow accessibility focus stay in any of them. Sigh...
    AutoPtr<IList> boundServices = IList::Probe(userState->mBoundServices);
    Int32 boundServiceCount;
    boundServices->GetSize(&boundServiceCount);
    for (Int32 i = 0; i < boundServiceCount; i++) {
        AutoPtr<IInterface> obj;
        boundServices->Get(i, (IInterface**)&obj);
        AutoPtr<Service> boundService = (Service*)IIAccessibilityServiceConnection::Probe(obj);
        if (boundService->CanRetrieveInteractiveWindowsLocked()) {
            userState->mAccessibilityFocusOnlyInActiveWindow = FALSE;
            return;
        }
    }
    userState->mAccessibilityFocusOnlyInActiveWindow = TRUE;
}

void AccessibilityManagerService::UpdateWindowsForAccessibilityCallbackLocked(
    /* [in] */ UserState* userState)
{
    if (userState->mIsAccessibilityEnabled) {
        // We observe windows for accessibility only if there is at least
        // one bound service that can retrieve window content that specified
        // it is interested in accessing such windows. For services that are
        // binding we do an update pass after each bind event, so we run this
        // code and register the callback if needed.
        Boolean boundServiceCanRetrieveInteractiveWindows = FALSE;

        AutoPtr<IList> boundServices = IList::Probe(userState->mBoundServices);
        Int32 boundServiceCount;
        boundServices->GetSize(&boundServiceCount);
        for (Int32 i = 0; i < boundServiceCount; i++) {
            AutoPtr<IInterface> obj;
            boundServices->Get(i, (IInterface**)&obj);
            AutoPtr<Service> boundService = (Service*)IIAccessibilityServiceConnection::Probe(obj);
            if (boundService->CanRetrieveInteractiveWindowsLocked()) {
                boundServiceCanRetrieveInteractiveWindows = TRUE;
                break;
            }
        }

        if (boundServiceCanRetrieveInteractiveWindows) {
            if (mWindowsForAccessibilityCallback == NULL) {
                mWindowsForAccessibilityCallback = new WindowsForAccessibilityCallback(this);
                mWindowManagerService->SetWindowsForAccessibilityCallback(
                        mWindowsForAccessibilityCallback);
            }
            return;
        }
    }

    if (mWindowsForAccessibilityCallback != NULL) {
        mWindowsForAccessibilityCallback = NULL;
        mWindowManagerService->SetWindowsForAccessibilityCallback(NULL);
        // Drop all windows we know about.
        mSecurityPolicy->ClearWindowsLocked();
    }
}

void AccessibilityManagerService::UpdateLegacyCapabilitiesLocked(
    /* [in] */ UserState* userState)
{
    // Up to JB-MR1 we had a white list with services that can enable touch
    // exploration. When a service is first started we show a dialog to the
    // use to get a permission to white list the service.
    Int32 installedServiceCount;
    userState->mInstalledServices->GetSize(&installedServiceCount);
    for (Int32 i = 0; i < installedServiceCount; i++) {
        AutoPtr<IInterface> obj;
        userState->mInstalledServices->Get(i, (IInterface**)&obj);
        AutoPtr<IAccessibilityServiceInfo> serviceInfo = IAccessibilityServiceInfo::Probe(obj);
        AutoPtr<IResolveInfo> resolveInfo;
        serviceInfo->GetResolveInfo((IResolveInfo**)&resolveInfo);

        AutoPtr<IServiceInfo> info;
        resolveInfo->GetServiceInfo((IServiceInfo**)&info);
        AutoPtr<IApplicationInfo> appInfo;
        IComponentInfo::Probe(info)->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 targetSdkVersion;
        appInfo->GetTargetSdkVersion(&targetSdkVersion);

        Int32 capabilities;
        if (((serviceInfo->GetCapabilities(&capabilities), capabilities)
                & IAccessibilityServiceInfo::CAPABILITY_CAN_REQUEST_TOUCH_EXPLORATION) == 0
                && targetSdkVersion <= Build::VERSION_CODES::JELLY_BEAN_MR1) {
            String packageName;
            IPackageItemInfo::Probe(info)->GetPackageName(&packageName);
            String name;
            IPackageItemInfo::Probe(info)->GetName(&name);
            AutoPtr<IComponentName> componentName;
            CComponentName::New(packageName, name, (IComponentName**)&componentName);
            Boolean res;
            if (userState->mTouchExplorationGrantedServices->Contains(componentName, &res), res) {
                serviceInfo->SetCapabilities(capabilities
                        | IAccessibilityServiceInfo::CAPABILITY_CAN_REQUEST_TOUCH_EXPLORATION);
            }
        }
    }
}

void AccessibilityManagerService::UpdateFilterKeyEventsLocked(
    /* [in] */ UserState* userState)
{
    Int32 serviceCount;
    IList::Probe(userState->mBoundServices)->GetSize(&serviceCount);
    for (Int32 i = 0; i < serviceCount; i++) {
        AutoPtr<IInterface> obj;
        IList::Probe(userState->mBoundServices)->Get(i, (IInterface**)&obj);
        AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);
        Int32 cap;
        if (service->mRequestFilterKeyEvents
                && ((service->mAccessibilityServiceInfo->GetCapabilities(&cap), cap)
                    & IAccessibilityServiceInfo::CAPABILITY_CAN_REQUEST_FILTER_KEY_EVENTS) != 0) {
            userState->mIsFilterKeyEventsEnabled = TRUE;
            return;
        }
    }
    userState->mIsFilterKeyEventsEnabled = FALSE;
}

void AccessibilityManagerService::UpdateServicesLocked(
    /* [in] */ UserState* userState)
{
    if (userState->mIsAccessibilityEnabled) {
        ManageServicesLocked(userState);
    }
    else {
        UnbindAllServicesLocked(userState);
    }
}

Boolean AccessibilityManagerService::ReadConfigurationForUserStateLocked(
    /* [in] */ UserState* userState)
{
    Boolean somthingChanged = ReadAccessibilityEnabledSettingLocked(userState);
    somthingChanged |= ReadInstalledAccessibilityServiceLocked(userState);
    somthingChanged |= ReadEnabledAccessibilityServicesLocked(userState);
    somthingChanged |= ReadTouchExplorationGrantedAccessibilityServicesLocked(userState);
    somthingChanged |= ReadTouchExplorationEnabledSettingLocked(userState);
    somthingChanged |= ReadHighTextContrastEnabledSettingLocked(userState);
    somthingChanged |= ReadEnhancedWebAccessibilityEnabledChangedLocked(userState);
    somthingChanged |= ReadDisplayMagnificationEnabledSettingLocked(userState);
    somthingChanged |= ReadDisplayColorAdjustmentSettingsLocked(userState);
    return somthingChanged;
}

Boolean AccessibilityManagerService::ReadAccessibilityEnabledSettingLocked(
    /* [in] */ UserState* userState)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    Int32 value;
    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::ACCESSIBILITY_ENABLED,
            0, userState->mUserId, &value);
    Boolean accessibilityEnabled = value == 1;

    if (accessibilityEnabled != userState->mIsAccessibilityEnabled) {
        userState->mIsAccessibilityEnabled = accessibilityEnabled;
        return TRUE;
    }
    return FALSE;
}

Boolean AccessibilityManagerService::ReadTouchExplorationEnabledSettingLocked(
    /* [in] */ UserState* userState)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    Int32 value;
    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::TOUCH_EXPLORATION_ENABLED,
            0, userState->mUserId, &value);
    Boolean touchExplorationEnabled = value == 1;

    if (touchExplorationEnabled != userState->mIsTouchExplorationEnabled) {
        userState->mIsTouchExplorationEnabled = touchExplorationEnabled;
        return TRUE;
    }
    return FALSE;
}

Boolean AccessibilityManagerService::ReadDisplayMagnificationEnabledSettingLocked(
    /* [in] */ UserState* userState)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    Int32 value;
    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED,
            0, userState->mUserId, &value);
    Boolean displayMagnificationEnabled = value == 1;

    if (displayMagnificationEnabled != userState->mIsDisplayMagnificationEnabled) {
        userState->mIsDisplayMagnificationEnabled = displayMagnificationEnabled;
        return TRUE;
    }
    return FALSE;
}

Boolean AccessibilityManagerService::ReadEnhancedWebAccessibilityEnabledChangedLocked(
    /* [in] */ UserState* userState)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    Int32 value;
    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::ACCESSIBILITY_SCRIPT_INJECTION,
            0, userState->mUserId, &value);
    Boolean enhancedWeAccessibilityEnabled = value == 1;

     if (enhancedWeAccessibilityEnabled != userState->mIsEnhancedWebAccessibilityEnabled) {
         userState->mIsEnhancedWebAccessibilityEnabled = enhancedWeAccessibilityEnabled;
         return TRUE;
     }
     return FALSE;
}

Boolean AccessibilityManagerService::ReadDisplayColorAdjustmentSettingsLocked(
    /* [in] */ UserState* userState)
{
    Boolean displayAdjustmentsEnabled = DisplayAdjustmentUtils::HasAdjustments(mContext,
            userState->mUserId);
    if (displayAdjustmentsEnabled != userState->mHasDisplayColorAdjustment) {
        userState->mHasDisplayColorAdjustment = displayAdjustmentsEnabled;
        return TRUE;
    }
    // If display adjustment is enabled, always assume there was a change in
    // the adjustment settings.
    return displayAdjustmentsEnabled;
}

Boolean AccessibilityManagerService::ReadHighTextContrastEnabledSettingLocked(
    /* [in] */ UserState* userState)
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    Int32 value;
    settingsSecure->GetInt32ForUser(resolver,
            ISettingsSecure::ACCESSIBILITY_HIGH_TEXT_CONTRAST_ENABLED,
            0, userState->mUserId, &value);
    Boolean highTextContrastEnabled = value == 1;

    if (highTextContrastEnabled != userState->mIsTextHighContrastEnabled) {
        userState->mIsTextHighContrastEnabled = highTextContrastEnabled;
        return TRUE;
    }
    return FALSE;
}

void AccessibilityManagerService::UpdateTouchExplorationLocked(
    /* [in] */ UserState* userState)
{
    Boolean enabled = FALSE;
    Int32 serviceCount;
    IList::Probe(userState->mBoundServices)->GetSize(&serviceCount);
    for (Int32 i = 0; i < serviceCount; i++) {
        AutoPtr<IInterface> obj;
        IList::Probe(userState->mBoundServices)->Get(i, (IInterface**)&obj);
        AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);
        if (CanRequestAndRequestsTouchExplorationLocked(service)) {
            enabled = TRUE;
            break;
        }
    }
    if (enabled != userState->mIsTouchExplorationEnabled) {
        userState->mIsTouchExplorationEnabled = enabled;
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        Boolean res;
        settingsSecure->PutInt32ForUser(resolver,
                ISettingsSecure::TOUCH_EXPLORATION_ENABLED, enabled ? 1 : 0,
                userState->mUserId, &res);
    }
}

Boolean AccessibilityManagerService::CanRequestAndRequestsTouchExplorationLocked(
    /* [in] */ Service* service)
{
    // Service not ready or cannot request the feature - well nothing to do.
    if (!service->CanReceiveEventsLocked() || !service->mRequestTouchExplorationMode) {
        return FALSE;
    }
    // UI test automation service can always enable it.
    if (service->mIsAutomation) {
        return TRUE;
    }

    AutoPtr<IServiceInfo> serviceInfo;
    service->mResolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
    AutoPtr<IApplicationInfo> appInfo;
    IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 targetSdkVersion;
    appInfo->GetTargetSdkVersion(&targetSdkVersion);

    if (targetSdkVersion <= Build::VERSION_CODES::JELLY_BEAN_MR1) {
        // Up to JB-MR1 we had a white list with services that can enable touch
        // exploration. When a service is first started we show a dialog to the
        // use to get a permission to white list the service.
        AutoPtr<UserState> userState = GetUserStateLocked(service->mUserId);
        Boolean res;
        if (userState->mTouchExplorationGrantedServices->Contains(service->mComponentName,
                &res), res) {
            return TRUE;
        }
        else if (mEnableTouchExplorationDialog == NULL
                || (IDialog::Probe(mEnableTouchExplorationDialog)->IsShowing(&res), !res)) {
            AutoPtr<IMessage> message;
            mMainHandler->ObtainMessage(
                    MainHandler::MSG_SHOW_ENABLED_TOUCH_EXPLORATION_DIALOG,
                    (IIAccessibilityServiceConnection*)service, (IMessage**)&message);
            message->SendToTarget();
        }
    }
    else {
        // Starting in JB-MR2 we request an accessibility service to declare
        // certain capabilities in its meta-data to allow it to enable the
        // corresponding features.
        Int32 cap;
        if (((service->mAccessibilityServiceInfo->GetCapabilities(&cap), cap)
                & IAccessibilityServiceInfo::CAPABILITY_CAN_REQUEST_TOUCH_EXPLORATION) != 0) {
            return TRUE;
        }
    }
    return FALSE;
}

void AccessibilityManagerService::UpdateEnhancedWebAccessibilityLocked(
    /* [in] */ UserState* userState)
{
    Boolean enabled = FALSE;
    Int32 serviceCount;
    IList::Probe(userState->mBoundServices)->GetSize(&serviceCount);
    for (Int32 i = 0; i < serviceCount; i++) {
        AutoPtr<IInterface> obj;
        IList::Probe(userState->mBoundServices)->Get(i, (IInterface**)&obj);
        AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);
        if (CanRequestAndRequestsEnhancedWebAccessibilityLocked(service)) {
            enabled = TRUE;
            break;
        }
    }
    if (enabled != userState->mIsEnhancedWebAccessibilityEnabled) {
        userState->mIsEnhancedWebAccessibilityEnabled = enabled;
        AutoPtr<IContentResolver> resolver;
        mContext->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSecure> settingsSecure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
        Boolean res;
        settingsSecure->PutInt32ForUser(resolver,
                ISettingsSecure::ACCESSIBILITY_SCRIPT_INJECTION, enabled ? 1 : 0,
                userState->mUserId, &res);
    }
}

Boolean AccessibilityManagerService::CanRequestAndRequestsEnhancedWebAccessibilityLocked(
    /* [in] */ Service* service)
{
    if (!service->CanReceiveEventsLocked() || !service->mRequestEnhancedWebAccessibility ) {
        return FALSE;
    }

    Int32 cap;
    if (service->mIsAutomation || ((service->mAccessibilityServiceInfo->GetCapabilities(&cap), cap)
           & IAccessibilityServiceInfo::CAPABILITY_CAN_REQUEST_ENHANCED_WEB_ACCESSIBILITY) != 0) {
        return TRUE;
    }
    return FALSE;
}

void AccessibilityManagerService::UpdateDisplayColorAdjustmentSettingsLocked(
    /* [in] */ UserState* userState)
{
    DisplayAdjustmentUtils::ApplyAdjustments(mContext, userState->mUserId);
}

Boolean AccessibilityManagerService::HasRunningServicesLocked(
    /* [in] */ UserState* userState)
{
    Boolean res1, res2;
    IList::Probe(userState->mBoundServices)->IsEmpty(&res1);
    userState->mBindingServices->IsEmpty(&res2);

    return !res1 || !res2;
}

AutoPtr<IMagnificationSpec> AccessibilityManagerService::GetCompatibleMagnificationSpecLocked(
    /* [in] */ Int32 windowId)
{
    AutoPtr<IInterface> obj;
    mGlobalWindowTokens->Get(windowId, (IInterface**)&obj);
    AutoPtr<IBinder> windowToken = IBinder::Probe(obj);
    if (windowToken == NULL) {
        AutoPtr<IInterface> other;
        GetCurrentUserStateLocked()->mWindowTokens->Get(windowId, (IInterface**)&other);
        windowToken = IBinder::Probe(other);
    }
    if (windowToken != NULL) {
        AutoPtr<IMagnificationSpec> spec;
        mWindowManagerService->GetCompatibleMagnificationSpecForWindow(
                windowToken, (IMagnificationSpec**)&spec);
        return spec;
    }
    return NULL;
}

ECode AccessibilityManagerService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    mSecurityPolicy->EnforceCallingPermission(
            Elastos::Droid::Manifest::permission::DUMP, FUNCTION_DUMP);
    synchronized(mLock) {
        pw->Println(String("ACCESSIBILITY MANAGER (dumpsys accessibility)"));
        pw->Println();
        Int32 userCount;
        mUserStates->GetSize(&userCount);
        AutoPtr<IAppendable> appPw = IAppendable::Probe(pw);
        for (Int32 i = 0; i < userCount; i++) {
            AutoPtr<IInterface> obj;
            mUserStates->ValueAt(i, (IInterface**)&obj);
            AutoPtr<UserState> userState = (UserState*)IObject::Probe(obj);
            appPw->Append(CoreUtils::Convert(
                    String("User state[attributes:{id=") +
                    StringUtils::ToString(userState->mUserId)));
            appPw->Append(CoreUtils::Convert(
                    String(", currentUser=") +
                    StringUtils::BooleanToString(userState->mUserId == mCurrentUserId)));
            appPw->Append(CoreUtils::Convert(
                    String(", accessibilityEnabled=") +
                    StringUtils::ToString(userState->mIsAccessibilityEnabled)));
            appPw->Append(CoreUtils::Convert(
                    String(", touchExplorationEnabled=") +
                    StringUtils::ToString(userState->mIsTouchExplorationEnabled)));
            appPw->Append(CoreUtils::Convert(
                    String(", displayMagnificationEnabled=") +
                    StringUtils::ToString(userState->mIsDisplayMagnificationEnabled)));
            if (userState->mUiAutomationService != NULL) {
                appPw->Append(CoreUtils::Convert(String(", ")));
                userState->mUiAutomationService->Dump(fd, pw, args);
                pw->Println();
            }
            appPw->Append(CoreUtils::Convert(String("}")));
            pw->Println();
            appPw->Append(CoreUtils::Convert(String("           services:{")));
            Int32 serviceCount;
            IList::Probe(userState->mBoundServices)->GetSize(&serviceCount);
            for (Int32 j = 0; j < serviceCount; j++) {
                if (j > 0) {
                    appPw->Append(CoreUtils::Convert(String(", ")));
                    pw->Println();
                    appPw->Append(CoreUtils::Convert(String("                     ")));
                }
                AutoPtr<IInterface> obj;
                IList::Probe(userState->mBoundServices)->Get(i, (IInterface**)&obj);
                AutoPtr<Service> service = (Service*)IIAccessibilityServiceConnection::Probe(obj);
                service->Dump(fd, pw, args);
            }
            pw->Println(String("}]"));
            pw->Println();
        }
        if (mSecurityPolicy->mWindows != NULL) {
            Int32 windowCount;
            mSecurityPolicy->mWindows->GetSize(&windowCount);
            for (Int32 j = 0; j < windowCount; j++) {
                if (j > 0) {
                    appPw->AppendChar(',');
                    pw->Println();
                }
                appPw->Append(CoreUtils::Convert(String("Window[")));
                AutoPtr<IInterface> obj;
                mSecurityPolicy->mWindows->Get(j, (IInterface**)&obj);
                AutoPtr<IAccessibilityWindowInfo> window = IAccessibilityWindowInfo::Probe(obj);
                String str;
                IObject::Probe(window)->ToString(&str);
                appPw->Append(CoreUtils::Convert(str));
                appPw->AppendChar(']');
            }
        }
    }
    return NOERROR;
}

AutoPtr<PendingEvent> AccessibilityManagerService::ObtainPendingEventLocked(
    /* [in] */ IKeyEvent* event,
    /* [in] */ Int32 policyFlags,
    /* [in] */ Int32 sequence)
{
    AutoPtr<PendingEvent> pendingEvent = mPendingEventPool->AcquireItem();
    if (pendingEvent == NULL) {
        pendingEvent = new PendingEvent();
    }
    pendingEvent->mEvent = event;
    pendingEvent->mPolicyFlags = policyFlags;
    pendingEvent->mSequence = sequence;
    return pendingEvent;
}

void AccessibilityManagerService::RecyclePendingEventLocked(
    /* [in] */ PendingEvent* pendingEvent)
{
    pendingEvent->Clear();
    mPendingEventPool->ReleaseItem(pendingEvent);
}

Int32 AccessibilityManagerService::FindWindowIdLocked(
    /* [in] */ IBinder* token)
{
    Int32 globalIndex;
    mGlobalWindowTokens->IndexOfValue(token, &globalIndex);
    if (globalIndex >= 0) {
        Int32 key;
        mGlobalWindowTokens->KeyAt(globalIndex, &key);
        return key;
    }
    AutoPtr<UserState> userState = GetCurrentUserStateLocked();
    Int32 userIndex;
    userState->mWindowTokens->IndexOfValue(token, &userIndex);
    if (userIndex >= 0) {
        Int32 key;
        userState->mWindowTokens->KeyAt(userIndex, &key);
        return key;
    }
    return -1;
}

void AccessibilityManagerService::EnsureWindowsAvailableTimed()
{
    synchronized(mLock) {
        if (mSecurityPolicy->mWindows != NULL) {
            return;
        }
        // If we have no registered callback, update the state we
        // we may have to register one but it didn't happen yet.
        if (mWindowsForAccessibilityCallback == NULL) {
            AutoPtr<UserState> userState = GetCurrentUserStateLocked();
            OnUserStateChangedLocked(userState);
        }
        // We have no windows but do not care about them, done.
        if (mWindowsForAccessibilityCallback == NULL) {
            return;
        }

        // Wait for the windows with a timeout.
        Int64 startMillis = SystemClock::GetUptimeMillis();
        while (mSecurityPolicy->mWindows == NULL) {
            Int64 elapsedMillis = SystemClock::GetUptimeMillis() - startMillis;
            Int64 remainMillis = WAIT_WINDOWS_TIMEOUT_MILLIS - elapsedMillis;
            if (remainMillis <= 0) {
                return;
            }
            // try {
            mLock.Wait(remainMillis);
            // } catch (InterruptedException ie) {
            //     /* ignore */
            // }
        }
    }
}

} // Accessibility
} // Server
} // Droid
} // Elastos
