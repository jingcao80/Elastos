
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.AccessibilityService.h"
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CAccessibilityManager.h"
#include "elastos/droid/view/accessibility/CAccessibilityManagerClient.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::AccessibilityService::IAccessibilityServiceInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IProcess;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Concurrent::CCopyOnWriteArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const Boolean CAccessibilityManager::DEBUG = FALSE;
const String CAccessibilityManager::localLOG_TAG("AccessibilityManager");

AutoPtr<IAccessibilityManager> CAccessibilityManager::sInstance = NULL;
Object CAccessibilityManager::sInstanceSync;

const Int32 CAccessibilityManager::MyHandler::MSG_NOTIFY_ACCESSIBILITY_STATE_CHANGED = 1;
const Int32 CAccessibilityManager::MyHandler::MSG_NOTIFY_EXPLORATION_STATE_CHANGED = 2;
const Int32 CAccessibilityManager::MyHandler::MSG_NOTIFY_HIGH_TEXT_CONTRAST_STATE_CHANGED = 3;
const Int32 CAccessibilityManager::MyHandler::MSG_SET_STATE = 4;

CAccessibilityManager::MyHandler::MyHandler(
    /* [in] */ CAccessibilityManager* host,
    /* [in] */ ILooper* looper)
    : Handler::Handler(looper, NULL, FALSE)
    , mHost(host)
{}

CAccessibilityManager::MyHandler::~MyHandler()
{}

ECode CAccessibilityManager::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_NOTIFY_ACCESSIBILITY_STATE_CHANGED:
            mHost->HandleNotifyAccessibilityStateChanged();
            break;
        case MSG_NOTIFY_EXPLORATION_STATE_CHANGED:
            mHost->HandleNotifyTouchExplorationStateChanged();
            break;
        case MSG_NOTIFY_HIGH_TEXT_CONTRAST_STATE_CHANGED:
            mHost->HandleNotifyHighTextContrastStateChanged();
            break;
        case MSG_SET_STATE: {
            // See comment at mClient
            Int32 state;
            msg->GetArg1(&state);
            Object& lock = mHost->mLock;
            synchronized(lock) {
                mHost->SetStateLocked(state);
            }
        } break;
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CAccessibilityManager, Object, IAccessibilityManager)

CAR_OBJECT_IMPL(CAccessibilityManager)

CAccessibilityManager::CAccessibilityManager()
    : mUserId(0)
    , mIsEnabled(FALSE)
    , mIsTouchExplorationEnabled(FALSE)
    , mIsHighTextContrastEnabled(FALSE)
{
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&mAccessibilityStateChangeListeners);
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&mTouchExplorationStateChangeListeners);
    CCopyOnWriteArrayList::New((ICopyOnWriteArrayList**)&mHighTextContrastStateChangeListeners);
    CAccessibilityManagerClient::New((IAccessibilityManager*)this, (IIAccessibilityManagerClient**)&mClient);
}

CAccessibilityManager::~CAccessibilityManager()
{}

ECode CAccessibilityManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IIAccessibilityManager* service,
    /* [in] */ Int32 userId)
{
    assert(context != NULL && service != NULL);
    AutoPtr<ILooper> looper;
    context->GetMainLooper((ILooper**)&looper);

    mHandler = new MyHandler(this, looper);
    mHandler->constructor();
    mService = service;
    mUserId = userId;
    synchronized(mLock) {
        TryConnectToServiceLocked();
    }
    return NOERROR;
}

ECode CAccessibilityManager::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ IAccessibilityManager** manager)
{
    VALIDATE_NOT_NULL(manager);
    *manager = NULL;

    synchronized(sInstanceSync) {
        if (sInstance == NULL) {
            Int32 userId;
            Int32 result1, result2;
            if (Binder::GetCallingUid() == IProcess::SYSTEM_UID
                    || context->CheckCallingOrSelfPermission(
                            Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS, &result1), result1
                                    == IPackageManager::PERMISSION_GRANTED
                    || context->CheckCallingOrSelfPermission(
                            Elastos::Droid::Manifest::permission::INTERACT_ACROSS_USERS_FULL, &result2), result2
                                    == IPackageManager::PERMISSION_GRANTED) {
                userId = IUserHandle::USER_CURRENT;
            }
            else {
                userId = UserHandle::GetMyUserId();
            }
            AutoPtr<IInterface> iBinder = ServiceManager::GetService(IContext::ACCESSIBILITY_SERVICE);
            AutoPtr<IIAccessibilityManager> service = IIAccessibilityManager::Probe(iBinder);
            AutoPtr<CAccessibilityManager> caManager;
            CAccessibilityManager::NewByFriend(context, service, userId, (CAccessibilityManager**)&caManager);
            sInstance = (IAccessibilityManager*)caManager.Get();
        }
    }
    *manager = sInstance;
    REFCOUNT_ADD(*manager);
    return NOERROR;
}

ECode CAccessibilityManager::GetClient(
    /* [out] */ IIAccessibilityManagerClient** client)
{
    VALIDATE_NOT_NULL(client);
    *client = mClient;
    REFCOUNT_ADD(*client);
    return NOERROR;
}

ECode CAccessibilityManager::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized(mLock) {
        AutoPtr<IIAccessibilityManager> service = GetServiceLocked();
        if (service == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        *result = mIsEnabled;
    }
    return NOERROR;
}

ECode CAccessibilityManager::IsTouchExplorationEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized(mLock) {
        AutoPtr<IIAccessibilityManager> service = GetServiceLocked();
        if (service == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        *result = mIsTouchExplorationEnabled;
    }
    return NOERROR;
}

ECode CAccessibilityManager::IsHighTextContrastEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    synchronized(mLock) {
        AutoPtr<IIAccessibilityManager> service = GetServiceLocked();
        if (service == NULL) {
            *result = FALSE;
            return NOERROR;
        }
        *result = mIsHighTextContrastEnabled;
    }
    return NOERROR;
}

ECode CAccessibilityManager::SendAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    AutoPtr<IIAccessibilityManager> service;
    Int32 userId = 0;
    synchronized(mLock) {
        service = GetServiceLocked();
        if (service == NULL) {
            return NOERROR;
        }
        if (!mIsEnabled) {
            Logger::E(localLOG_TAG, "Accessibility off. Did you forget to check that?");
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("Accessibility off. Did you forget to check that?");
        }
        userId = mUserId;
    }

    Boolean doRecycle = FALSE;
    // try {
    event->SetEventTime(SystemClock::GetUptimeMillis());
    // it is possible that this manager is in the same process as the service but
    // client using it is called through Binder from another process. Example: MMS
    // app adds a SMS notification and the NotificationManagerService calls this method
    Int64 identityToken = Binder::ClearCallingIdentity();
    ECode ec = service->SendAccessibilityEvent(event, userId, &doRecycle);
    if (FAILED(ec)) {
        Logger::E(localLOG_TAG, "Error during sending %p 0x%08x", event, ec);
    }
    else {
        if (DEBUG) {
            Logger::I(localLOG_TAG, " %p sent", event);
        }
    }
    Binder::RestoreCallingIdentity(identityToken);

    if (doRecycle) {
        IAccessibilityRecord::Probe(event)->Recycle();
    }
    return NOERROR;
    // } catch (RemoteException re) {
    //     Log.e(localLOG_TAG, "Error during sending " + event + " ", re);
    // } finally {
    //     if (doRecycle) {
    //         event.recycle();
    //     }
    // }
}

ECode CAccessibilityManager::Interrupt()
{
    AutoPtr<IIAccessibilityManager> service;
    Int32 userId = 0;
    synchronized(mLock) {
        service = GetServiceLocked();
        if (service == NULL) {
            return NOERROR;
        }
        if (!mIsEnabled) {
            Logger::E(localLOG_TAG, "Accessibility off. Did you forget to check that?");
            return E_ILLEGAL_STATE_EXCEPTION;
            // throw new IllegalStateException("Accessibility off. Did you forget to check that?");
        }
        userId = mUserId;
    }

    // try {
    ECode ec = service->Interrupt(userId);
    if (FAILED(ec)) {
        Logger::E(localLOG_TAG, "Error while requesting interrupt from all services. 0x%08x", ec);
    }
    else {
        if (DEBUG) {
            Logger::I(localLOG_TAG, "Requested interrupt from all services");
        }
    }
    return NOERROR;
    // } catch (RemoteException re) {
    //     Log.e(localLOG_TAG, "Error while requesting interrupt from all services. ", re);
    // }
}

ECode CAccessibilityManager::GetAccessibilityServiceList(
    /* [out] */ IList** serviceList)
{
    VALIDATE_NOT_NULL(serviceList);

    AutoPtr<IList> infos;
    GetInstalledAccessibilityServiceList((IList**)&infos);
    AutoPtr<IList> services;
    CArrayList::New((IList**)&services);
    Int32 infoCount;
    infos->GetSize(&infoCount);
    for (Int32 i = 0; i < infoCount; i++) {
        AutoPtr<IInterface> obj;
        infos->Get(i, (IInterface**)&obj);
        AutoPtr<IAccessibilityServiceInfo> info = IAccessibilityServiceInfo::Probe(obj);
        AutoPtr<IResolveInfo> resolveInfo;
        info->GetResolveInfo((IResolveInfo**)&resolveInfo);
        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        services->Add(serviceInfo);
    }
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    return coll->UnmodifiableList(services, serviceList);
}

ECode CAccessibilityManager::GetInstalledAccessibilityServiceList(
    /* [out] */ IList** serviceList)
{
    VALIDATE_NOT_NULL(serviceList);

    AutoPtr<IIAccessibilityManager> service;
    Int32 userId = 0;
    synchronized(mLock) {
        service = GetServiceLocked();
        if (service == NULL) {
            AutoPtr<ICollections> coll;
            CCollections::AcquireSingleton((ICollections**)&coll);
            return coll->GetEmptyList(serviceList);
        }
        userId = mUserId;
    }

    AutoPtr<IList> services;
    // try {
    ECode ec = service->GetInstalledAccessibilityServiceList(userId, (IList**)&services);
    if (FAILED(ec)) {
        Logger::E(localLOG_TAG, "Error while obtaining the installed AccessibilityServices. 0x%08x", ec);
    }
    else {
        if (DEBUG) {
            Logger::I(localLOG_TAG, "Installed AccessibilityServices %p", services.Get());
        }
    }
    // } catch (RemoteException re) {
    //     Log.e(localLOG_TAG, "Error while obtaining the installed AccessibilityServices. ", re);
    // }
    if (services != NULL) {
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        return coll->UnmodifiableList(services, serviceList);
    }
    else {
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        return coll->GetEmptyList(serviceList);
    }
    return NOERROR;
}

ECode CAccessibilityManager::GetEnabledAccessibilityServiceList(
    /* [in] */ Int32 feedbackTypeFlags,
    /* [out] */ IList** serviceList)
{
    VALIDATE_NOT_NULL(serviceList);

    AutoPtr<IIAccessibilityManager> service;
    Int32 userId = 0;
    synchronized(mLock) {
        service = GetServiceLocked();
        if (service == NULL) {
            AutoPtr<ICollections> coll;
            CCollections::AcquireSingleton((ICollections**)&coll);
            return coll->GetEmptyList(serviceList);
        }
        userId = mUserId;
    }

    AutoPtr<IList> services;
    // try {
    ECode ec = service->GetEnabledAccessibilityServiceList(feedbackTypeFlags,
            userId, (IList**)&services);
    if (FAILED(ec)) {
        Logger::E(localLOG_TAG, "Error while obtaining the installed AccessibilityServices. 0x%08x", ec);
    }
    else {
        if (DEBUG) {
            Logger::I(localLOG_TAG, "Installed AccessibilityServices %p", services.Get());
        }
    }
    // } catch (RemoteException re) {
    //     Log.e(localLOG_TAG, "Error while obtaining the installed AccessibilityServices. ", re);
    // }
    if (services != NULL) {
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        return coll->UnmodifiableList(services, serviceList);
    }
    else {
        AutoPtr<ICollections> coll;
        CCollections::AcquireSingleton((ICollections**)&coll);
        return coll->GetEmptyList(serviceList);
    }
    return NOERROR;
}

ECode CAccessibilityManager::AddAccessibilityStateChangeListener(
    /* [in] */ IAccessibilityManagerAccessibilityStateChangeListener* listener,
    /* [out] */ Boolean* result)
{
    // Final CopyOnArrayList - no lock needed.
    VALIDATE_NOT_NULL(result);
    return ICollection::Probe(mAccessibilityStateChangeListeners)->Add(listener, result);
}

ECode CAccessibilityManager::RemoveAccessibilityStateChangeListener(
    /* [in] */ IAccessibilityManagerAccessibilityStateChangeListener* listener,
    /* [out] */ Boolean* result)
{
    // Final CopyOnArrayList - no lock needed.
    VALIDATE_NOT_NULL(result);
    return ICollection::Probe(mAccessibilityStateChangeListeners)->Remove(listener, result);
}

ECode CAccessibilityManager::AddTouchExplorationStateChangeListener(
    /* [in] */ IAccessibilityManagerTouchExplorationStateChangeListener* listener,
    /* [out] */ Boolean* result)
{
    // Final CopyOnArrayList - no lock needed.
    VALIDATE_NOT_NULL(result);
    return ICollection::Probe(mTouchExplorationStateChangeListeners)->Add(listener, result);
}

ECode CAccessibilityManager::RemoveTouchExplorationStateChangeListener(
    /* [in] */ IAccessibilityManagerTouchExplorationStateChangeListener* listener,
    /* [out] */ Boolean* result)
{
    // Final CopyOnArrayList - no lock needed.
    VALIDATE_NOT_NULL(result);
    return ICollection::Probe(mTouchExplorationStateChangeListeners)->Remove(listener, result);
}

ECode CAccessibilityManager::AddHighTextContrastStateChangeListener(
    /* [in] */ IAccessibilityManagerHighTextContrastChangeListener* listener,
    /* [out] */ Boolean* result)
{
    // Final CopyOnArrayList - no lock needed.
    VALIDATE_NOT_NULL(result);
    return ICollection::Probe(mHighTextContrastStateChangeListeners)->Add(listener, result);
}

ECode CAccessibilityManager::RemoveHighTextContrastStateChangeListener(
    /* [in] */ IAccessibilityManagerHighTextContrastChangeListener* listener,
    /* [out] */ Boolean* result)
{
    // Final CopyOnArrayList - no lock needed.
    VALIDATE_NOT_NULL(result);
    return ICollection::Probe(mHighTextContrastStateChangeListeners)->Remove(listener, result);
}

void CAccessibilityManager::SetStateLocked(
    /* [in] */ Int32 stateFlags)
{
    const Boolean enabled = (stateFlags & STATE_FLAG_ACCESSIBILITY_ENABLED) != 0;
    const Boolean touchExplorationEnabled =
            (stateFlags & STATE_FLAG_TOUCH_EXPLORATION_ENABLED) != 0;
    const Boolean highTextContrastEnabled =
            (stateFlags & STATE_FLAG_HIGH_TEXT_CONTRAST_ENABLED) != 0;

    const Boolean wasEnabled = mIsEnabled;
    const Boolean wasTouchExplorationEnabled = mIsTouchExplorationEnabled;
    const Boolean wasHighTextContrastEnabled = mIsHighTextContrastEnabled;

    // Ensure listeners get current state from isZzzEnabled() calls.
    mIsEnabled = enabled;
    mIsTouchExplorationEnabled = touchExplorationEnabled;
    mIsHighTextContrastEnabled = highTextContrastEnabled;

    Boolean res;
    if (wasEnabled != enabled) {
        mHandler->SendEmptyMessage(MyHandler::MSG_NOTIFY_ACCESSIBILITY_STATE_CHANGED, &res);
    }

    if (wasTouchExplorationEnabled != touchExplorationEnabled) {
        mHandler->SendEmptyMessage(MyHandler::MSG_NOTIFY_EXPLORATION_STATE_CHANGED, &res);
    }

    if (wasHighTextContrastEnabled != highTextContrastEnabled) {
        mHandler->SendEmptyMessage(MyHandler::MSG_NOTIFY_HIGH_TEXT_CONTRAST_STATE_CHANGED, &res);
    }
}

ECode CAccessibilityManager::AddAccessibilityInteractionConnection(
    /* [in] */ IIWindow* windowToken,
    /* [in] */ IIAccessibilityInteractionConnection* connection,
    /* [out] */ Int32* add)
{
    VALIDATE_NOT_NULL(add);

    AutoPtr<IIAccessibilityManager> service;
    Int32 userId = 0;
    synchronized(mLock) {
        service = GetServiceLocked();
        if (service == NULL) {
            *add = IView::NO_ID;
            return NOERROR;
        }
        userId = mUserId;
    }

    // try {
    ECode ec = service->AddAccessibilityInteractionConnection(windowToken, connection, userId, add);
    if (SUCCEEDED(ec)) return NOERROR;
    Logger::E(localLOG_TAG, "Error while adding an accessibility interaction connection. 0x%08x", ec);

    *add = IView::NO_ID;
    return NOERROR;
    // } catch (RemoteException re) {
    //     Log.e(localLOG_TAG, "Error while adding an accessibility interaction connection. ", re);
    // }
}

ECode CAccessibilityManager::RemoveAccessibilityInteractionConnection(
    /* [in] */ IIWindow* windowToken)
{
    AutoPtr<IIAccessibilityManager> service;
    synchronized(mLock) {
        service = GetServiceLocked();
        if (service == NULL) {
            return NOERROR;
        }
    }

    // try {
    ECode ec = service->RemoveAccessibilityInteractionConnection(windowToken);
    if (FAILED(ec)) {
        Logger::E(localLOG_TAG, "Error while removing an accessibility interaction connection. 0x%08x", ec);
    }
    return NOERROR;
    // } catch (RemoteException re) {
    //     Log.e(localLOG_TAG, "Error while removing an accessibility interaction connection. ", re);
    // }
}

AutoPtr<IIAccessibilityManager> CAccessibilityManager::GetServiceLocked()
{
    if (mService == NULL) {
        TryConnectToServiceLocked();
    }
    return mService;
}

void CAccessibilityManager::TryConnectToServiceLocked()
{
    AutoPtr<IInterface> iBinder = ServiceManager::GetService(IContext::ACCESSIBILITY_SERVICE);
    if (iBinder == NULL) {
        return;
    }
    AutoPtr<IIAccessibilityManager> service =IIAccessibilityManager::Probe(iBinder);

    // try {
    Int32 stateFlags;
    ECode ec = service->AddClient(mClient, mUserId, &stateFlags);
    if (FAILED(ec)) {
        Logger::E(LOG_TAG, "AccessibilityManagerService is dead, 0x%08x", ec);
    }
    SetStateLocked(stateFlags);
    mService = service;
    // } catch (RemoteException re) {
    //     Log.e(LOG_TAG, "AccessibilityManagerService is dead", re);
    // }
}

void CAccessibilityManager::HandleNotifyAccessibilityStateChanged()
{
    Boolean isEnabled = FALSE;
    synchronized(mLock) {
        isEnabled = mIsEnabled;
    }
    Int32 listenerCount;
    IList::Probe(mAccessibilityStateChangeListeners)->GetSize(&listenerCount);
    for (Int32 i = 0; i < listenerCount; i++) {
        AutoPtr<IInterface> obj;
        IList::Probe(mAccessibilityStateChangeListeners)->Get(i, (IInterface**)&obj);
        IAccessibilityManagerAccessibilityStateChangeListener::Probe(obj)->OnAccessibilityStateChanged(isEnabled);
    }
}

void CAccessibilityManager::HandleNotifyTouchExplorationStateChanged()
{
    Boolean isTouchExplorationEnabled = FALSE;
    synchronized(mLock) {
        isTouchExplorationEnabled = mIsTouchExplorationEnabled;
    }
    Int32 listenerCount;
    IList::Probe(mTouchExplorationStateChangeListeners)->GetSize(&listenerCount);
    for (Int32 i = 0; i < listenerCount; i++) {
        AutoPtr<IInterface> obj;
        IList::Probe(mTouchExplorationStateChangeListeners)->Get(i, (IInterface**)&obj);
        IAccessibilityManagerTouchExplorationStateChangeListener::Probe(obj)->OnTouchExplorationStateChanged(isTouchExplorationEnabled);
    }
}

void CAccessibilityManager::HandleNotifyHighTextContrastStateChanged()
{
    Boolean isHighTextContrastEnabled = FALSE;
    synchronized(mLock) {
        isHighTextContrastEnabled = mIsHighTextContrastEnabled;
    }
    Int32 listenerCount;
    IList::Probe(mHighTextContrastStateChangeListeners)->GetSize(&listenerCount);
    for (Int32 i = 0; i < listenerCount; i++) {
        AutoPtr<IInterface> obj;
        IList::Probe(mHighTextContrastStateChangeListeners)->Get(i, (IInterface**)&obj);
        IAccessibilityManagerHighTextContrastChangeListener::Probe(obj)->OnHighTextContrastStateChanged(isHighTextContrastEnabled);
    }
}

} // Accessibility
} // View
} // Droid
} // Elastos
