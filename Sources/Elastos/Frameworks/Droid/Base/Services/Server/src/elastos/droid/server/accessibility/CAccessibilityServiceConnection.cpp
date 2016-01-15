
#include "accessibility/CAccessibilityServiceConnection.h"
#include "accessibility/CAccessibilityManagerService.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IKeyEventHelper;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::AccessibilityService::IAccessibilityService;
using Elastos::Droid::AccessibilityService::EIID_IAccessibilityServiceConnection;
using Elastos::Droid::Hardware::Input::IInputManager;
// using Elastos::Droid::Hardware::Input::IInputManagerHelper;
// using Elastos::Droid::Hardware::Input::CInputManagerHelper;
using Elastos::Droid::App::IStatusBarManager;
using Elastos::Droid::StatusBar::IIStatusBarService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Accessibility {

const Boolean CAccessibilityServiceConnection::DEBUG = TRUE;

const Int32 CAccessibilityServiceConnection::MSG_ON_GESTURE = 0x80000000;

//=========================================================================
// CAccessibilityServiceConnection::MyHandler
//=========================================================================
ECode CAccessibilityServiceConnection::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch(what) {
        case CAccessibilityServiceConnection::MSG_ON_GESTURE: {
            Int32 gestureId;
            msg->GetArg1(&gestureId);
            mHost->NotifyGestureInternal(gestureId);
            break;
        }
        default: {
            Int32 eventType = what;
            mHost->NotifyAccessibilityEventInternal(eventType);
            break;
        }
    }

    return NOERROR;
}

//=========================================================================
// CAccessibilityServiceConnection::MyHandler
//=========================================================================
CAccessibilityServiceConnection::CAccessibilityServiceConnection()
    : mUserId(0)
    , mId(0)
    , mEventTypes(0)
    , mFeedbackType(0)
    , mIsDefault(FALSE)
    , mRequestTouchExplorationMode(FALSE)
    , mIncludeNotImportantViews(FALSE)
    , mNotificationTimeout(0)
    , mCanRetrieveScreenContent(FALSE)
    , mIsAutomation(FALSE)
{
    CRect::New((IRect**)&mTempBounds);
}

ECode CAccessibilityServiceConnection::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ Int32 userId,
    /* [in] */ IComponentName* componentName,
    /* [in] */ IAccessibilityServiceInfo* accessibilityServiceInfo,
    /* [in] */ Boolean isAutomation,
    /* [in] */ IIAccessibilityManager* host)
{
    mHandler = new MyHandler(looper, this);
    mAccessibilityManager = host;
    CAccessibilityManagerService* service = (CAccessibilityManagerService*)host;
    mUserId = userId;
    accessibilityServiceInfo->GetResolveInfo((IResolveInfo**)&mResolveInfo);
    mId = CAccessibilityManagerService::sIdCounter++;
    mComponentName = componentName;
    mAccessibilityServiceInfo = accessibilityServiceInfo;
    mIsAutomation = isAutomation;
    if (!isAutomation) {
        accessibilityServiceInfo->GetCanRetrieveWindowContent(&mCanRetrieveScreenContent);
        Int32 flags;
        accessibilityServiceInfo->GetFlags(&flags);
        mRequestTouchExplorationMode =
            (flags & IAccessibilityServiceInfo::FLAG_REQUEST_TOUCH_EXPLORATION_MODE) != 0;
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
        helper->GetActivity(service->mContext, 0, i, 0, (IPendingIntent**)&pendingI);
        ASSERT_SUCCEEDED(mIntent->PutExtra(IIntent::EXTRA_CLIENT_INTENT,
            IParcelable::Probe(pendingI)));
    }
    else {
        mCanRetrieveScreenContent = TRUE;
    }
    SetDynamicallyConfigurableProperties(accessibilityServiceInfo);

    return NOERROR;
}

void CAccessibilityServiceConnection::SetDynamicallyConfigurableProperties(
    /* [in] */ IAccessibilityServiceInfo* info)
{
    info->GetEventTypes(&mEventTypes);
    info->GetFeedbackType(&mFeedbackType);
    AutoPtr< ArrayOf<String> > packageNames;
    info->GetPackageNames((ArrayOf<String>**)&packageNames);
    if (packageNames != NULL) {
        for (Int32 i = 0; i < packageNames->GetLength(); ++i) {
            mPackageNames.Insert((*packageNames)[i]);
        }
    }
    info->GetNotificationTimeout(&mNotificationTimeout);
    Int32 flags;
    info->GetFlags(&flags);
    mIsDefault = (flags & IAccessibilityServiceInfo::DEFAULT) != 0;

    if (mIsAutomation) {
        mIncludeNotImportantViews =
            (flags & IAccessibilityServiceInfo::FLAG_INCLUDE_NOT_IMPORTANT_VIEWS) != 0;
    }
    else {
        AutoPtr<IResolveInfo> resolveInfo;
        ASSERT_SUCCEEDED(info->GetResolveInfo((IResolveInfo**)&resolveInfo));
        AutoPtr<IServiceInfo> serviceInfo;
        ASSERT_SUCCEEDED(resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo));
        AutoPtr<IApplicationInfo> appInfo;
        serviceInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 version;
        appInfo->GetTargetSdkVersion(&version);
        if (version >= Build::VERSION_CODES::JELLY_BEAN) {
            mIncludeNotImportantViews =
                (flags & IAccessibilityServiceInfo::FLAG_INCLUDE_NOT_IMPORTANT_VIEWS) != 0;
        }
    }

    mRequestTouchExplorationMode = (flags
            & IAccessibilityServiceInfo::FLAG_REQUEST_TOUCH_EXPLORATION_MODE) != 0;

    // If this service is up and running we may have to enable touch
    // exploration, otherwise this will happen when the service connects.
    CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
    AutoLock lock(service->mLock);
    if (CanReceiveEvents()) {
        if (mRequestTouchExplorationMode) {
            service->TryEnableTouchExplorationLocked(this);
        }
        else {
            service->TryDisableTouchExplorationLocked(this);
        }
    }
}

Boolean CAccessibilityServiceConnection::Bind()
{
    if (!mIsAutomation && mService == NULL) {
        CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
        Boolean result;
        service->mContext->BindService(mIntent, IServiceConnection::Probe(this),
            IContext::BIND_AUTO_CREATE, mUserId, &result);
        return result;
    }
    return FALSE;
}

Boolean CAccessibilityServiceConnection::Unbind()
{
    if (mService != NULL) {
        CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
        {
            AutoLock lock(service->mLock);
            service->TryRemoveServiceLocked(THIS_PROBE(IIAccessibilityServiceConnection));
        }
        if (!mIsAutomation) {
            service->mContext->UnbindService(IServiceConnection::Probe(this));
        }
        return TRUE;
    }
    return FALSE;
}

Boolean CAccessibilityServiceConnection::CanReceiveEvents()
{
    return (mEventTypes != 0 && mFeedbackType != 0 && mService != NULL);
}

ECode CAccessibilityServiceConnection::GetServiceInfo(
    /* [out] */ IAccessibilityServiceInfo** serviceInfo)
{
    VALIDATE_NOT_NULL(serviceInfo);
    CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
    AutoLock lock(service->mLock);
    *serviceInfo = mAccessibilityServiceInfo;
    REFCOUNT_ADD(*serviceInfo);
    return NOERROR;
}

ECode CAccessibilityServiceConnection::SetServiceInfo(
    /* [in] */ IAccessibilityServiceInfo* info)
{
    Int32 identity = Binder::ClearCallingIdentity();
    // try {
    CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
    AutoLock lock(service->mLock);
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
    Binder::RestoreCallingIdentity(identity);
    // } finally {
    //     Binder.restoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode CAccessibilityServiceConnection::OnServiceConnected(
    /* [in] */ IComponentName* name,
    /* [in] */ IBinder* binder)
{
    mService = binder;
    mServiceInterface = IIAccessibilityServiceClient::Probe(binder);
    // try {
    ECode ec = mServiceInterface->SetConnection(this, mId);
    if (FAILED(ec)) {
        Slogger::W("CAccessibilityServiceConnection",
            "Error while setting Controller for service: %p 0x%08x", binder, ec);
        return ec;
    }

    CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
    AutoLock lock(service->mLock);
    service->TryAddServiceLocked(this, mUserId);
    return NOERROR;
    // } catch (RemoteException re) {
    //     Slog.w("CAccessibilityServiceConnection", "Error while setting Controller for service: " + service, re);
    // }
}

ECode CAccessibilityServiceConnection::FindAccessibilityNodeInfoByViewId(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 viewId,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale);

    Int32 resolvedWindowId;
    AutoPtr<IAccessibilityInteractionConnection> connection;
    {
        CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
        AutoLock lock(service->mLock);
        AutoPtr<IUserHandleHelper> helper;
        ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper));
        Int32 id, resolvedUserId;
        helper->GetCallingUserId(&id);
        service->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(id, &resolvedUserId);
        if (resolvedUserId != service->mCurrentUserId) {
            *scale = -1;
            return NOERROR;
        }
        service->mSecurityPolicy->EnforceCanRetrieveWindowContent(this);
        Boolean permissionGranted = service->mSecurityPolicy->CanRetrieveWindowContent(this);
        if (!permissionGranted) {
            *scale = 0;
            return NOERROR;
        }
        else {
            resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *scale = 0;
                return NOERROR;
            }
        }
    }

    Int32 flags = (mIncludeNotImportantViews) ? IAccessibilityNodeInfo::INCLUDE_NOT_IMPORTANT_VIEWS : 0;
    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = connection->FindAccessibilityNodeInfoByViewId(
        accessibilityNodeId, viewId,
        interactionId, callback, flags, interrogatingPid, interrogatingTid);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("CAccessibilityServiceConnection", "Error findAccessibilityNodeInfoByViewId().");
        }
        Binder::RestoreCallingIdentity(identityToken);
        *scale = 0;
        return ec;
    }

    *scale = GetCompatibilityScale(resolvedWindowId);
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("CAccessibilityServiceConnection", "Error findAccessibilityNodeInfoByViewId().");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
    // return 0;
}

ECode CAccessibilityServiceConnection::FindAccessibilityNodeInfosByText(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ const String& text,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale);

    Int32 resolvedWindowId;
    AutoPtr<IAccessibilityInteractionConnection> connection;
    {
        CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
        AutoLock lock(service->mLock);
        AutoPtr<IUserHandleHelper> helper;
        ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper));
        Int32 id, resolvedUserId;
        helper->GetCallingUserId(&id);
        service->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(id, &resolvedUserId);
        if (resolvedUserId != service->mCurrentUserId) {
            *scale = -1;
            return NOERROR;
        }
        service->mSecurityPolicy->EnforceCanRetrieveWindowContent(this);
        resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
        Boolean permissionGranted =
            service->mSecurityPolicy->CanGetAccessibilityNodeInfoLocked(this, resolvedWindowId);
        if (!permissionGranted) {
            *scale = 0;
            return NOERROR;
        }
        else {
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *scale = 0;
                return NOERROR;
            }
        }
    }
    Int32 flags = (mIncludeNotImportantViews) ?
        IAccessibilityNodeInfo::INCLUDE_NOT_IMPORTANT_VIEWS : 0;
    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = connection->FindAccessibilityNodeInfosByText(accessibilityNodeId, text,
        interactionId, callback, flags, interrogatingPid, interrogatingTid);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("CAccessibilityServiceConnection", "Error calling findAccessibilityNodeInfosByText()");
        }
        Binder::RestoreCallingIdentity(identityToken);
        *scale = 0;
        return ec;
    }
    *scale = GetCompatibilityScale(resolvedWindowId);
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("CAccessibilityServiceConnection", "Error calling findAccessibilityNodeInfosByText()");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
    // return 0;
}

ECode CAccessibilityServiceConnection::FindAccessibilityNodeInfoByAccessibilityId(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int32 flags,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale);

    Int32 resolvedWindowId;
    AutoPtr<IAccessibilityInteractionConnection> connection;
    {
        CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
        AutoLock lock(service->mLock);
        AutoPtr<IUserHandleHelper> helper;
        ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper));
        Int32 id, resolvedUserId;
        helper->GetCallingUserId(&id);
        service->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(id, &resolvedUserId);
        if (resolvedUserId != service->mCurrentUserId) {
            *scale = -1;
            return NOERROR;
        }
        service->mSecurityPolicy->EnforceCanRetrieveWindowContent(this);
        resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
        Boolean permissionGranted =
            service->mSecurityPolicy->CanGetAccessibilityNodeInfoLocked(this, resolvedWindowId);
        if (!permissionGranted) {
            *scale = 0;
            return NOERROR;
        }
        else {
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *scale = 0;
                return NOERROR;
            }
        }
    }
    Int32 allFlags = flags | ((mIncludeNotImportantViews) ?
            IAccessibilityNodeInfo::INCLUDE_NOT_IMPORTANT_VIEWS : 0);
    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = connection->FindAccessibilityNodeInfoByAccessibilityId(accessibilityNodeId,
        interactionId, callback, allFlags, interrogatingPid, interrogatingTid);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("CAccessibilityServiceConnection", "Error calling findAccessibilityNodeInfoByAccessibilityId()");
        }
        Binder::RestoreCallingIdentity(identityToken);
        *scale = 0;
        return ec;
    }
    *scale = GetCompatibilityScale(resolvedWindowId);
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("CAccessibilityServiceConnection", "Error calling findAccessibilityNodeInfoByAccessibilityId()");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
    // return 0;
}

ECode CAccessibilityServiceConnection::FindFocus(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 focusType,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale);

    Int32 resolvedWindowId;
    AutoPtr<IAccessibilityInteractionConnection> connection;
    {
        CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
        AutoLock lock(service->mLock);
        AutoPtr<IUserHandleHelper> helper;
        ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper));
        Int32 id, resolvedUserId;
        helper->GetCallingUserId(&id);
        service->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(id, &resolvedUserId);
        if (resolvedUserId != service->mCurrentUserId) {
            *scale = -1;
            return NOERROR;
        }
        service->mSecurityPolicy->EnforceCanRetrieveWindowContent(this);
        resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
        Boolean permissionGranted =
            service->mSecurityPolicy->CanGetAccessibilityNodeInfoLocked(this, resolvedWindowId);
        if (!permissionGranted) {
            *scale = 0;
            return NOERROR;
        }
        else {
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *scale = 0;
                return NOERROR;
            }
        }
    }
    Int32 flags = (mIncludeNotImportantViews) ?
        IAccessibilityNodeInfo::INCLUDE_NOT_IMPORTANT_VIEWS : 0;
    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = connection->FindFocus(
        accessibilityNodeId, focusType, interactionId, callback,
        flags, interrogatingPid, interrogatingTid);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("CAccessibilityServiceConnection", "Error calling findAccessibilityFocus()");
        }
        Binder::RestoreCallingIdentity(identityToken);
        *scale = 0;
        return ec;
    }
    *scale = GetCompatibilityScale(resolvedWindowId);
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("CAccessibilityServiceConnection", "Error calling findAccessibilityFocus()");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
    // return 0;
}

ECode CAccessibilityServiceConnection::FocusSearch(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 direction,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale);

    Int32 resolvedWindowId;
    AutoPtr<IAccessibilityInteractionConnection> connection;
    {
        CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
        AutoLock lock(service->mLock);
        AutoPtr<IUserHandleHelper> helper;
        ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper));
        Int32 id, resolvedUserId;
        helper->GetCallingUserId(&id);
        service->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(id, &resolvedUserId);
        if (resolvedUserId != service->mCurrentUserId) {
            *scale = -1;
            return NOERROR;
        }
        service->mSecurityPolicy->EnforceCanRetrieveWindowContent(this);
        resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
        Boolean permissionGranted =
            service->mSecurityPolicy->CanGetAccessibilityNodeInfoLocked(this, resolvedWindowId);
        if (!permissionGranted) {
            *scale = 0;
            return NOERROR;
        }
        else {
            connection = GetConnectionLocked(resolvedWindowId);
            if (connection == NULL) {
                *scale = 0;
                return NOERROR;
            }
        }
    }
    Int32 flags = (mIncludeNotImportantViews) ?
        IAccessibilityNodeInfo::INCLUDE_NOT_IMPORTANT_VIEWS : 0;
    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = connection->FocusSearch(
        accessibilityNodeId, direction, interactionId, callback,
        flags, interrogatingPid, interrogatingTid);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("CAccessibilityServiceConnection", "Error calling accessibilityFocusSearch()");
        }
        Binder::RestoreCallingIdentity(identityToken);
        *scale = 0;
        return ec;
    }
    *scale = GetCompatibilityScale(resolvedWindowId);
    Binder::RestoreCallingIdentity(identityToken);
    return NOERROR;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("CAccessibilityServiceConnection", "Error calling accessibilityFocusSearch()");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
    // return 0;
}

ECode CAccessibilityServiceConnection::PerformAccessibilityAction(
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [in] */ Int32 interactionId,
    /* [in] */ IAccessibilityInteractionConnectionCallback* callback,
    /* [in] */ Int64 interrogatingTid,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 resolvedWindowId;
    AutoPtr<IAccessibilityInteractionConnection> connection;
    {
        CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
        AutoLock lock(service->mLock);
        AutoPtr<IUserHandleHelper> helper;
        ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper));
        Int32 id, resolvedUserId;
        helper->GetCallingUserId(&id);
        service->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(id, &resolvedUserId);
        if (resolvedUserId != service->mCurrentUserId) {
            *result = FALSE;
            return NOERROR;
        }

        service->mSecurityPolicy->EnforceCanRetrieveWindowContent(this);
        resolvedWindowId = ResolveAccessibilityWindowIdLocked(accessibilityWindowId);
        Boolean permissionGranted = service->mSecurityPolicy->CanPerformActionLocked(
            THIS_PROBE(IIAccessibilityServiceConnection), resolvedWindowId, action, arguments);
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

    Int32 flags = (mIncludeNotImportantViews) ?
        IAccessibilityNodeInfo::INCLUDE_NOT_IMPORTANT_VIEWS : 0;
    Int32 interrogatingPid = Binder::GetCallingPid();
    Int64 identityToken = Binder::ClearCallingIdentity();
    // try {
    ECode ec = connection->PerformAccessibilityAction(
        accessibilityNodeId, action, arguments,
        interactionId, callback, flags, interrogatingPid, interrogatingTid);
    if (FAILED(ec)) {
        if (DEBUG) {
            Slogger::E("CAccessibilityServiceConnection", "Error calling performAccessibilityAction()");
        }
    }
    Binder::RestoreCallingIdentity(identityToken);
    *result = TRUE;
    return ec;
    // } catch (RemoteException re) {
    //     if (DEBUG) {
    //         Slogger::E("CAccessibilityServiceConnection", "Error calling performAccessibilityAction()");
    //     }
    // } finally {
    //     Binder.restoreCallingIdentity(identityToken);
    // }
    // return true;
}

ECode CAccessibilityServiceConnection::PerformGlobalAction(
    /* [in] */ Int32 action,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    {
        CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
        AutoLock lock(service->mLock);
        AutoPtr<IUserHandleHelper> helper;
        ASSERT_SUCCEEDED(CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper));
        Int32 id, resolvedUserId;
        helper->GetCallingUserId(&id);
        service->mSecurityPolicy->ResolveCallingUserIdEnforcingPermissionsLocked(id, &resolvedUserId);
        if (resolvedUserId != service->mCurrentUserId) {
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
    }

    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
    // } finally {
    //     Binder.restoreCallingIdentity(identity);
    // }
}

ECode CAccessibilityServiceConnection::OnServiceDisconnected(
    /* [in] */ IComponentName* name)
{
    /* do nothing - #binderDied takes care */
    return NOERROR;
}

ECode CAccessibilityServiceConnection::LinkToOwnDeath()
{
    IProxy* proxy = (IProxy*)mService->Probe(EIID_IProxy);
    return proxy->LinkToDeath(THIS_PROBE(IProxyDeathRecipient), 0);
}

ECode CAccessibilityServiceConnection::UnlinkToOwnDeath()
{
    IProxy* proxy = (IProxy*)mService->Probe(EIID_IProxy);
    Boolean result;
    return proxy->UnlinkToDeath(THIS_PROBE(IProxyDeathRecipient), 0, &result);
}

ECode CAccessibilityServiceConnection::Dispose()
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

ECode CAccessibilityServiceConnection::ProxyDied()
{
    CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
    AutoLock lock(service->mLock);
    // The death recipient is unregistered in tryRemoveServiceLocked
    service->TryRemoveServiceLocked(THIS_PROBE(IIAccessibilityServiceConnection));
    // We no longer have an automation service, so restore
    // the state based on values in the settings database.
    if (mIsAutomation) {
        service->mUiAutomationService = NULL;
        service->RecreateInternalStateLocked(service->GetUserStateLocked(mUserId));
    }
    return NOERROR;
}

void CAccessibilityServiceConnection::NotifyAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
    AutoLock lock(service->mLock);
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
    AutoPtr<IAccessibilityEvent> oldEvent;
    HashMap<Int32, AutoPtr<IAccessibilityEvent> >::Iterator it
            = mPendingEvents.Find(eventType);
    if (it != mPendingEvents.End()) {
        oldEvent = it->mSecond;
    }
    mPendingEvents[eventType] = newEvent;

    Int32 what = eventType;
    if (oldEvent != NULL) {
        mHandler->RemoveMessages(what);
        oldEvent->Recycle();
    }

    Boolean result;
    mHandler->SendEmptyMessageDelayed(what, mNotificationTimeout, &result);
}

void CAccessibilityServiceConnection::NotifyAccessibilityEventInternal(
    /* [in] */ Int32 eventType)
{
    AutoPtr<IIAccessibilityServiceClient> listener;
    AutoPtr<IAccessibilityEvent> event;

    {
        CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
        AutoLock lock(service->mLock);
        listener = mServiceInterface;

        // If the service died/was disabled while the message for dispatching
        // the accessibility event was propagating the listener may be null.
        if (listener == NULL) {
            return;
        }

        HashMap<Int32, AutoPtr<IAccessibilityEvent> >::Iterator it
            = mPendingEvents.Find(eventType);
        if (it != mPendingEvents.End()) {
            event = it->mSecond;
        }

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

        mPendingEvents.Erase(it);
        if (service->mSecurityPolicy->CanRetrieveWindowContent(THIS_PROBE(IIAccessibilityServiceConnection))) {
            event->SetConnectionId(mId);
        }
        else {
            event->SetSource(NULL);
        }
        event->SetSealed(TRUE);
    }

    // try {
    if (FAILED(listener->OnAccessibilityEvent(event))) {
        Slogger::E("CAccessibilityServiceConnection",
            "Error during sending %p to %p", event.Get(), listener.Get());
    }
    event->Recycle();
    // if (DEBUG) {
    //     Slog.i("CAccessibilityServiceConnection", "Event " + event + " sent to " + listener);
    // }
    // } catch (RemoteException re) {
    //     Slogger::E("CAccessibilityServiceConnection", "Error during sending " + event + " to " + listener, re);
    // } finally {
    //     event.recycle();
    // }
}

void CAccessibilityServiceConnection::NotifyGesture(
    /* [in] */ Int32 gestureId)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_ON_GESTURE, (IMessage**)&msg);
    msg->SetArg1(gestureId);
    Boolean result;
    mHandler->SendMessage(msg, &result);
}

void CAccessibilityServiceConnection::NotifyGestureInternal(
    /* [in] */ Int32 gestureId)
{
    AutoPtr<IIAccessibilityServiceClient> listener = mServiceInterface;
    if (listener != NULL) {
        // try {
        ECode ec =listener->OnGesture(gestureId);
        if (FAILED(ec)) {
            Slogger::E("CAccessibilityServiceConnection",
                    "Error during sending gesture %d to %p 0x%08x", gestureId, mService.Get(),
                    ec);
        }
        // } catch (RemoteException re) {
        //     Slogger::E("CAccessibilityServiceConnection", "Error during sending gesture " + gestureId
        //             + " to " + mService, re);
        // }
    }
}

void CAccessibilityServiceConnection::SendDownAndUpKeyEvents(
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

    assert(0);
    // AutoPtr<IInputManagerHelper> inputHelper;
    // ASSERT_SUCCEEDED(CInputManagerHelper::AcquireSingleton(
    //         (IInputManagerHelper**)&inputHelper));
    AutoPtr<IInputManager> inputManager;
    // ASSERT_SUCCEEDED(inputHelper->GetInstance((IInputManager**)&inputManager));
    Boolean result;
    inputManager->InjectInputEvent(down,
            IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC, &result);
    down->Recycle();

    // Inject up.
    Int64 upTime = SystemClock::GetUptimeMillis();
    AutoPtr<IKeyEvent> up;
    helper->Obtain(downTime, upTime, IKeyEvent::ACTION_UP, keyCode, 0, 0,
        IKeyCharacterMap::VIRTUAL_KEYBOARD, 0, IKeyEvent::FLAG_FROM_SYSTEM,
        IInputDevice::SOURCE_KEYBOARD, nullStr, (IKeyEvent**)&up);
    inputManager->InjectInputEvent(up,
        IInputManager::INJECT_INPUT_EVENT_MODE_ASYNC, &result);
    up->Recycle();

    Binder::RestoreCallingIdentity(token);
}

void CAccessibilityServiceConnection::ExpandNotifications()
{
    Int64 token = Binder::ClearCallingIdentity();

    CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
    AutoPtr<IInterface> obj;
    ASSERT_SUCCEEDED(service->mContext->GetSystemService(
        IContext::STATUS_BAR_SERVICE, (IInterface**)&obj));
    AutoPtr<IStatusBarManager> statusBarManager = IStatusBarManager::Probe(obj);
    statusBarManager->ExpandNotificationsPanel();

    Binder::RestoreCallingIdentity(token);
}

void CAccessibilityServiceConnection::ExpandQuickSettings()
{
    Int64 token = Binder::ClearCallingIdentity();

    CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
    AutoPtr<IInterface> obj;
    ASSERT_SUCCEEDED(service->mContext->GetSystemService(
        IContext::STATUS_BAR_SERVICE, (IInterface**)&obj));
    AutoPtr<IStatusBarManager> statusBarManager = IStatusBarManager::Probe(obj);
    statusBarManager->ExpandSettingsPanel();

    Binder::RestoreCallingIdentity(token);
}

void CAccessibilityServiceConnection::OpenRecents()
{
    Int64 token = Binder::ClearCallingIdentity();

    AutoPtr<IIStatusBarService> statusBarService =
        (IIStatusBarService*)ServiceManager::GetService(String("statusbar")).Get();
    // try {
    if (FAILED(statusBarService->ToggleRecentApps())) {
        Slogger::E("CAccessibilityServiceConnection", "Error toggling recent apps.");
    }
    // } catch (RemoteException e) {
    //     Slogger::E("CAccessibilityServiceConnection", "Error toggling recent apps.");
    // }

    Binder::RestoreCallingIdentity(token);
}

AutoPtr<IAccessibilityInteractionConnection> CAccessibilityServiceConnection::GetConnectionLocked(
    /* [in] */ Int32 windowId)
{
    // if (DEBUG) {
    //     Slog.i("CAccessibilityServiceConnection", "Trying to get interaction connection to windowId: " + windowId);
    // }
    CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
    AutoPtr<CAccessibilityManagerService::AccessibilityConnectionWrapper> wrapper;
    HashMap<Int32, AutoPtr<CAccessibilityManagerService::AccessibilityConnectionWrapper> >::Iterator it
        = service->mGlobalInteractionConnections.Find(windowId);
    if (it != service->mGlobalInteractionConnections.End()) {
        wrapper = it->mSecond;
    }
    if (wrapper == NULL) {
        AutoPtr<CAccessibilityManagerService::UserState> state = service->GetCurrentUserStateLocked();
        it = state->mInteractionConnections.Find(windowId);
        if (it != state->mInteractionConnections.End()) {
            wrapper = it->mSecond;
        }
    }
    if (wrapper != NULL && wrapper->mConnection != NULL) {
        return wrapper->mConnection;
    }
    // if (DEBUG) {
    //     Slogger::E("CAccessibilityServiceConnection", "No interaction connection to window: " + windowId);
    // }
    return NULL;
}

Int32 CAccessibilityServiceConnection::ResolveAccessibilityWindowIdLocked(
    /* [in] */ Int32 accessibilityWindowId)
{
    if (accessibilityWindowId == IAccessibilityNodeInfo::ACTIVE_WINDOW_ID) {
        CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
        return service->mSecurityPolicy->mActiveWindowId;
    }
    return accessibilityWindowId;
}

Float CAccessibilityServiceConnection::GetCompatibilityScale(
    /* [in] */ Int32 windowId)
{
    // try {
    CAccessibilityManagerService* service = (CAccessibilityManagerService*)mAccessibilityManager.Get();
    AutoPtr<IBinder> windowToken;
    HashMap<Int32, AutoPtr<IBinder> >::Iterator it = service->mGlobalWindowTokens.Find(windowId);
    if (it != service->mGlobalWindowTokens.End()) {
        windowToken = it->mSecond;
    }
    if (windowToken != NULL) {
        Float scale;
        if (FAILED(service->mWindowManagerService->GetWindowCompatibilityScale(
            windowToken, &scale))) {
            scale = 1.0f;
        }
        return scale;
    }
    AutoPtr<CAccessibilityManagerService::UserState> state
        = service->GetCurrentUserStateLocked();
    it = state->mWindowTokens.Find(windowId);
    if (it != state->mWindowTokens.End()) {
        windowToken = it->mSecond;
    }
    if (windowToken != NULL) {
        Float scale;
        if (FAILED(service->mWindowManagerService->GetWindowCompatibilityScale(
                windowToken, &scale))) {
            scale = 1.0f;
        }
        return scale;
    }
    // } catch (RemoteException re) {
    //     /* ignore */
    // }
    return 1.0f;
}

void CAccessibilityServiceConnection::HandleOnGesture(
    /* [in] */ Int32 gestureId)
{
    NotifyGestureInternal(gestureId);
}

void CAccessibilityServiceConnection::HandleDefault(
    /* [in] */ Int32 eventType)
{
    NotifyAccessibilityEventInternal(eventType);
}

} // Accessibility
} // Server
} // Droid
} // Elastos
