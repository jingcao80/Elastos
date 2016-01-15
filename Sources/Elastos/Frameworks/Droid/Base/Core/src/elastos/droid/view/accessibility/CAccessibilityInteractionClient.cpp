
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "Elastos.Droid.AccessibilityService.h"
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/accessibility/CAccessibilityInteractionClient.h"
#include "elastos/droid/view/accessibility/CAccessibilityCache.h"
#include "elastos/droid/view/accessibility/CAccessibilityNodeInfo.h"
#include "elastos/droid/graphics/CPoint.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::CInt64SparseArray;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Core::Thread;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::IQueue;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

const String CAccessibilityInteractionClient::TAG("CAccessibilityInteractionClient");
const Boolean CAccessibilityInteractionClient::DEBUG = FALSE;
const Boolean CAccessibilityInteractionClient::CHECK_INTEGRITY = TRUE;
const Int64 CAccessibilityInteractionClient::TIMEOUT_INTERACTION_MILLIS = 5000;
Object CAccessibilityInteractionClient::sStaticLock;

static AutoPtr<IInt64SparseArray> InitsClients()
{
    AutoPtr<CInt64SparseArray> array;
    CInt64SparseArray::NewByFriend((CInt64SparseArray**)&array);
    return (IInt64SparseArray*)array.Get();
}
const AutoPtr<IInt64SparseArray> CAccessibilityInteractionClient::sClients = InitsClients();

static AutoPtr<ISparseArray> InitsConnectionCache()
{
    AutoPtr<CSparseArray> array;
    CSparseArray::NewByFriend((CSparseArray**)&array);
    return (ISparseArray*)array.Get();
}
const AutoPtr<ISparseArray> CAccessibilityInteractionClient::sConnectionCache = InitsConnectionCache();

static AutoPtr<IAccessibilityCache> InitsAccessibilityCache()
{
    AutoPtr<CAccessibilityCache> cache;
    CAccessibilityCache::NewByFriend((CAccessibilityCache**)&cache);
    return (IAccessibilityCache*)cache.Get();
}
const AutoPtr<IAccessibilityCache> CAccessibilityInteractionClient::sAccessibilityCache = InitsAccessibilityCache();

CAR_INTERFACE_IMPL_3(CAccessibilityInteractionClient, Object, IAccessibilityInteractionClient, IIAccessibilityInteractionConnectionCallback, IBinder)

CAR_OBJECT_IMPL(CAccessibilityInteractionClient)

CAccessibilityInteractionClient::CAccessibilityInteractionClient()
    : mInteractionId(0)
    , mPerformAccessibilityActionResult(FALSE)
{
    ASSERT_SUCCEEDED(CAtomicInteger32::New((IAtomicInteger32**)&mInteractionIdCounter));
    CAccessibilityNodeInfo::New((IAccessibilityNodeInfo**)&mFindAccessibilityNodeInfoResult);
    CArrayList::New((IList**)&mFindAccessibilityNodeInfosResult);
    CPoint::New((IPoint**)&mComputeClickPointResult);
    CMessage::New((IMessage**)&mSameThreadMessage);
}

CAccessibilityInteractionClient::~CAccessibilityInteractionClient()
{
}

ECode CAccessibilityInteractionClient::constructor()
{
    return NOERROR;
}

ECode CAccessibilityInteractionClient::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = "CAccessibilityInteractionClient";
    return NOERROR;
}

AutoPtr<IAccessibilityInteractionClient> CAccessibilityInteractionClient::GetInstance()
{
    Int64 threadId;
    Thread::GetCurrentThread()->GetId(&threadId);
    return GetInstanceForThread(threadId);
}

AutoPtr<IAccessibilityInteractionClient> CAccessibilityInteractionClient::GetInstanceForThread(
    /* [in] */ Int64 threadId)
{
    AutoPtr<IAccessibilityInteractionClient> client;
    synchronized(sStaticLock) {
        AutoPtr<IInterface> obj;
        sClients->Get(threadId, (IInterface**)&obj);
        client = IAccessibilityInteractionClient::Probe(obj);
        if (client == NULL) {
            AutoPtr<CAccessibilityInteractionClient> _client;
            CAccessibilityInteractionClient::NewByFriend((CAccessibilityInteractionClient**)&_client);
            client = (IAccessibilityInteractionClient*)_client.Get();
            sClients->Put(threadId, client);
        }
    }
    return client;
}

ECode CAccessibilityInteractionClient::SetSameThreadMessage(
    /* [in] */ IMessage* message)
{
    synchronized(mInstanceLock) {
        mSameThreadMessage = message;
        mInstanceLock.NotifyAll();
    }
    return NOERROR;
}

ECode CAccessibilityInteractionClient::GetRootInActiveWindow(
    /* [in] */ Int32 connectionId,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return FindAccessibilityNodeInfoByAccessibilityId(connectionId,
            IAccessibilityNodeInfo::ACTIVE_WINDOW_ID, IAccessibilityNodeInfo::ROOT_NODE_ID,
            FALSE, IAccessibilityNodeInfo::FLAG_PREFETCH_DESCENDANTS, info);
}

ECode CAccessibilityInteractionClient::GetWindow(
    /* [in] */ Int32 connectionId,
    /* [in] */ Int32 accessibilityWindowId,
    /* [out] */ IAccessibilityWindowInfo** info)
{
    VALIDATE_NOT_NULL(info)
    *info = NULL;
    // try {
    AutoPtr<IIAccessibilityServiceConnection> connection;
    GetConnection(connectionId, (IIAccessibilityServiceConnection**)&connection);
    if (connection != NULL) {
        AutoPtr<IAccessibilityWindowInfo> window;
        sAccessibilityCache->GetWindow(accessibilityWindowId, (IAccessibilityWindowInfo**)&window);
        if (window != NULL) {
            if (DEBUG) {
                Slogger::I(LOG_TAG, "Window cache hit");
            }
            *info = window;
            REFCOUNT_ADD(*info);
            return NOERROR;
        }
        if (DEBUG) {
            Slogger::I(LOG_TAG, "Window cache miss");
        }
        connection->GetWindow(accessibilityWindowId, (IAccessibilityWindowInfo**)&window);
        if (window != NULL) {
            sAccessibilityCache->AddWindow(window);
            *info = window;
            REFCOUNT_ADD(*info);
            return NOERROR;
        }
    }
    else {
        if (DEBUG) {
            Slogger::W(LOG_TAG, "No connection for connection id: %d", connectionId);
        }
    }
    // } catch (RemoteException re) {
    //     Log.e(LOG_TAG, "Error while calling remote getWindow", re);
    // }
    return NOERROR;
}

ECode CAccessibilityInteractionClient::GetWindows(
    /* [in] */ Int32 connectionId,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;
    // try {
    AutoPtr<IIAccessibilityServiceConnection> connection;
    GetConnection(connectionId, (IIAccessibilityServiceConnection**)&connection);
    if (connection != NULL) {
        AutoPtr<IList> windows;
        sAccessibilityCache->GetWindows((IList**)&windows);
        if (windows != NULL) {
            if (DEBUG) {
                Slogger::I(LOG_TAG, "Windows cache hit");
            }
            *list = windows;
            REFCOUNT_ADD(*list);
            return NOERROR;
        }
        if (DEBUG) {
            Slogger::I(LOG_TAG, "Windows cache miss");
        }
        connection->GetWindows((IList**)&windows);
        if (windows != NULL) {
            Int32 windowCount;
            windows->GetSize(&windowCount);
            for (Int32 i = 0; i < windowCount; i++) {
                AutoPtr<IInterface> obj;
                windows->Get(i, (IInterface**)&obj);
                AutoPtr<IAccessibilityWindowInfo> window = IAccessibilityWindowInfo::Probe(obj);
                sAccessibilityCache->AddWindow(window);
            }
            *list = windows;
            REFCOUNT_ADD(*list);
            return NOERROR;
        }
    }
    else {
        if (DEBUG) {
            Slogger::W(LOG_TAG, "No connection for connection id: %d", connectionId);
        }
    }
    // } catch (RemoteException re) {
    //     Log.e(LOG_TAG, "Error while calling remote getWindows", re);
    // }
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    return collections->GetEmptyList(list);
}

ECode CAccessibilityInteractionClient::FindAccessibilityNodeInfoByAccessibilityId(
    /* [in] */ Int32 connectionId,
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Boolean bypassCache,
    /* [in] */ Int32 prefetchFlags,
    /* [out] */ IAccessibilityNodeInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;

    if ((prefetchFlags & IAccessibilityNodeInfo::FLAG_PREFETCH_SIBLINGS) != 0
            && (prefetchFlags & IAccessibilityNodeInfo::FLAG_PREFETCH_PREDECESSORS) == 0) {
        Slogger::I(LOG_TAG, "FLAG_PREFETCH_SIBLINGS requires FLAG_PREFETCH_PREDECESSORS");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("FLAG_PREFETCH_SIBLINGS"
        //     + " requires FLAG_PREFETCH_PREDECESSORS");
    }

    // try {
    AutoPtr<IIAccessibilityServiceConnection> connection;
    GetConnection(connectionId, (IIAccessibilityServiceConnection**)&connection);
    if (connection != NULL) {
        if (!bypassCache) {
            AutoPtr<IAccessibilityNodeInfo> cachedInfo;
            sAccessibilityCache->GetNode(accessibilityWindowId, accessibilityNodeId, (IAccessibilityNodeInfo**)&cachedInfo);
            if (cachedInfo != NULL) {
                if (DEBUG) {
                    Slogger::I(LOG_TAG, "Node cache hit");
                }
                *info = cachedInfo;
                REFCOUNT_ADD(*info);
                return NOERROR;
            }
            if (DEBUG) {
                Slogger::I(LOG_TAG, "Node cache miss");
            }
        }

        Int32 interactionId;
        mInteractionIdCounter->GetAndIncrement(&interactionId);
        Int64 id;
        Thread::GetCurrentThread()->GetId(&id);
        Boolean success;
        ECode ec = connection->FindAccessibilityNodeInfoByAccessibilityId(
                accessibilityWindowId, accessibilityNodeId, interactionId,
                THIS_PROBE(IIAccessibilityInteractionConnectionCallback),
                prefetchFlags, id, &success);
        if (FAILED(ec)) {
            if (DEBUG) {
                Slogger::W(TAG, "Error while calling remote" \
                    " findAccessibilityNodeInfoByAccessibilityId, ec=%08x", ec);
            }
            return NOERROR;
        }

        // If the scale is zero the call has failed.
        if (success) {
            AutoPtr<IList> infos = GetFindAccessibilityNodeInfosResultAndClear(interactionId);
            FinalizeAndCacheAccessibilityNodeInfos(infos, connectionId);
            Boolean res;
            if (infos != NULL && (infos->IsEmpty(&res), !res)) {
                AutoPtr<IInterface> obj;
                infos->Get(0, (IInterface**)&obj);
                *info = IAccessibilityNodeInfo::Probe(obj);
                REFCOUNT_ADD(*info);
                return NOERROR;
            }
        }
    }
    else {
        if (DEBUG) {
            Slogger::W(TAG, "No connection for connection id: %d", connectionId);
        }
    }
    // } catch (RemoteException re) {
    //         Slogger::E(TAG, "Error while calling remote"
    //                 + " findAccessibilityNodeInfoByAccessibilityId", re);
    // }
    return NOERROR;
}

ECode CAccessibilityInteractionClient::FindAccessibilityNodeInfosByViewId(
    /* [in] */ Int32 connectionId,
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ const String& viewId,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;

    // try {
    AutoPtr<IIAccessibilityServiceConnection> connection;
    GetConnection(connectionId, (IIAccessibilityServiceConnection**)&connection);
    if (connection != NULL) {
        Int32 interactionId;
        mInteractionIdCounter->GetAndIncrement(&interactionId);
        Int64 id;
        Thread::GetCurrentThread()->GetId(&id);
        Boolean success;
        ECode ec = connection->FindAccessibilityNodeInfosByViewId(
                accessibilityWindowId, accessibilityNodeId, viewId, interactionId,
                THIS_PROBE(IIAccessibilityInteractionConnectionCallback), id, &success);
        if (FAILED(ec)) {
            if (DEBUG) {
                Slogger::W(TAG, "Error while calling remote" \
                    " findAccessibilityNodeInfoByViewIdInActiveWindow, ec=%d", ec);
            }
            return NOERROR;
        }

        if (success) {
            AutoPtr<IList> infos = GetFindAccessibilityNodeInfosResultAndClear(interactionId);
            if (infos != NULL) {
                FinalizeAndCacheAccessibilityNodeInfos(infos, connectionId);
                *list = infos;
                REFCOUNT_ADD(*list);
                return NOERROR;
            }
        }
    }
    else {
        if (DEBUG) {
            Slogger::W(TAG, "No connection for connection id: %d", connectionId);
        }
    }
    // } catch (RemoteException re) {
    //         Slogger::W(TAG, "Error while calling remote"
    //                 + " findAccessibilityNodeInfoByViewIdInActiveWindow", re);
    // }
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    return collections->GetEmptyList(list);
}

ECode CAccessibilityInteractionClient::FindAccessibilityNodeInfosByText(
    /* [in] */ Int32 connectionId,
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ const String& text,
    /* [out] */ IList** list)
{
    VALIDATE_NOT_NULL(list);
    *list = NULL;

    // try {
    AutoPtr<IIAccessibilityServiceConnection> connection;
    GetConnection(connectionId, (IIAccessibilityServiceConnection**)&connection);
    if (connection != NULL) {
        Int32 interactionId;
        mInteractionIdCounter->GetAndIncrement(&interactionId);
        Int64 id;
        Thread::GetCurrentThread()->GetId(&id);
        Boolean success;
        ECode ec = connection->FindAccessibilityNodeInfosByText(
                accessibilityWindowId, accessibilityNodeId, text, interactionId,
                THIS_PROBE(IIAccessibilityInteractionConnectionCallback), id, &success);
        if (FAILED(ec)) {
            if (DEBUG) {
                Slogger::W(TAG, "Error while calling remote" \
                    " findAccessibilityNodeInfosByViewText, ec=%08x", ec);
            }
            return ec;
        }

        if (success) {
            AutoPtr<IList> infos = GetFindAccessibilityNodeInfosResultAndClear(interactionId);
            if (infos != NULL) {
                FinalizeAndCacheAccessibilityNodeInfos(infos, connectionId);
                *list = infos;
                REFCOUNT_ADD(*list);
                return NOERROR;
            }
        }
    }
    else {
        if (DEBUG) {
            Slogger::W(TAG, "No connection for connection id: %d", connectionId);
        }
    }
    // } catch (RemoteException re) {
    //         Slogger::W(TAG, "Error while calling remote"
    //                 + " findAccessibilityNodeInfosByViewText", re);
    // }
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    return collections->GetEmptyList(list);
}

ECode CAccessibilityInteractionClient::FindFocus(
    /* [in] */ Int32 connectionId,
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 focusType,
    /* [out] */ IAccessibilityNodeInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    // try {
    AutoPtr<IIAccessibilityServiceConnection> connection;
    GetConnection(connectionId, (IIAccessibilityServiceConnection**)&connection);
    if (connection != NULL) {
        Int32 interactionId;
        mInteractionIdCounter->GetAndIncrement(&interactionId);
        Int64 id;
        Thread::GetCurrentThread()->GetId(&id);
        Boolean success;
        ECode ec = connection->FindFocus(accessibilityWindowId,
                accessibilityNodeId, focusType, interactionId,
                THIS_PROBE(IIAccessibilityInteractionConnectionCallback), id, &success);
        if (FAILED(ec)) {
            if (DEBUG) {
                Slogger::W(TAG, "Error while calling remote findAccessibilityFocus, ec=%08x", ec);
            }
            return ec;
        }

        if (success) {
            AutoPtr<IAccessibilityNodeInfo> info = GetFindAccessibilityNodeInfoResultAndClear(interactionId);
            FinalizeAndCacheAccessibilityNodeInfo(info, connectionId);
            *result = info;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    else {
        if (DEBUG) {
            Slogger::W(TAG, "No connection for connection id: %d", connectionId);
        }
    }
    // } catch (RemoteException re) {
    //         Slogger::W(TAG, "Error while calling remote findFocus", re);
    // }
    return NOERROR;
}

ECode CAccessibilityInteractionClient::FocusSearch(
    /* [in] */ Int32 connectionId,
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 direction,
    /* [out] */ IAccessibilityNodeInfo** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    AutoPtr<IIAccessibilityServiceConnection> connection;
    GetConnection(connectionId, (IIAccessibilityServiceConnection**)&connection);
    if (connection != NULL) {
        Int32 interactionId;
        mInteractionIdCounter->GetAndIncrement(&interactionId);
        Int64 id;
        Thread::GetCurrentThread()->GetId(&id);
        Boolean success;
        ECode ec = connection->FindFocus(accessibilityWindowId,
            accessibilityNodeId, direction, interactionId,
            THIS_PROBE(IIAccessibilityInteractionConnectionCallback), id, &success);
        if (FAILED(ec)) {
            if (DEBUG) {
                Slogger::W(TAG, "Error while calling remote accessibilityFocusSearch, ec=%08x", ec);
            }
            return ec;
        }

        if (success) {
            AutoPtr<IAccessibilityNodeInfo> info = GetFindAccessibilityNodeInfoResultAndClear(interactionId);
            FinalizeAndCacheAccessibilityNodeInfo(info, connectionId);
            *result = info;
            REFCOUNT_ADD(*result);
            return NOERROR;
        }
    }
    else {
        if (DEBUG) {
            Slogger::W(TAG, "No connection for connection id: %d", connectionId);
        }
    }
    // } catch (RemoteException re) {
    //         Slogger::W(TAG, "Error while calling remote accessibilityFocusSearch", re);
    // }
    return NOERROR;
}

ECode CAccessibilityInteractionClient::PerformAccessibilityAction(
    /* [in] */ Int32 connectionId,
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoPtr<IIAccessibilityServiceConnection> connection;
    GetConnection(connectionId, (IIAccessibilityServiceConnection**)&connection);
    if (connection != NULL) {
        Int32 interactionId;
        mInteractionIdCounter->GetAndIncrement(&interactionId);
        Int64 id;
        Thread::GetCurrentThread()->GetId(&id);
        Boolean success;
        ECode ec = connection->PerformAccessibilityAction(accessibilityWindowId,
            accessibilityNodeId, action, arguments, interactionId,
            THIS_PROBE(IIAccessibilityInteractionConnectionCallback), id, &success);
        if (FAILED(ec)) {
            if (DEBUG) {
                Slogger::W(TAG, "Error while calling remote performAccessibilityAction, ec =%08x", ec);
            }
            return ec;
        }
        if (success) {
            *result = GetPerformAccessibilityActionResultAndClear(interactionId);
            return NOERROR;
        }
    }
    else {
        if (DEBUG) {
            Slogger::W(TAG, "No connection for connection id: %d", connectionId);
        }
    }
    // } catch (RemoteException re) {
    //         Slogger::W(TAG, "Error while calling remote performAccessibilityAction", re);
    // }
    return NOERROR;
}

ECode CAccessibilityInteractionClient::ComputeClickPointInScreen(
    /* [in] */ Int32 connectionId,
    /* [in] */ Int32 accessibilityWindowId,
    /* [in] */ Int64 accessibilityNodeId,
    /* [out] */ IPoint** point)
{
    VALIDATE_NOT_NULL(point);
    *point = NULL;

    // try {
    AutoPtr<IIAccessibilityServiceConnection> connection;
    GetConnection(connectionId, (IIAccessibilityServiceConnection**)&connection);
    if (connection != NULL) {
        Int32 interactionId;
        mInteractionIdCounter->GetAndIncrement(&interactionId);
        Int64 id;
        Thread::GetCurrentThread()->GetId(&id);
        Boolean success;
        ECode ec = connection->ComputeClickPointInScreen(accessibilityWindowId,
            accessibilityNodeId, interactionId,
            THIS_PROBE(IIAccessibilityInteractionConnectionCallback), id, &success);
        if (FAILED(ec)) {
            if (DEBUG) {
                Slogger::W(TAG, "Error while calling remote ComputeClickPointInScreen, ec =%08x", ec);
            }
            return ec;
        }
        if (success) {
            AutoPtr<IPoint> _point = GetComputeClickPointInScreenResultAndClear(interactionId);
            *point = _point;
            REFCOUNT_ADD(*point);
            return NOERROR;
        }
    }
    else {
        if (DEBUG) {
            Slogger::W(LOG_TAG, "No connection for connection id: %d", connectionId);
        }
    }
    // } catch (RemoteException re) {
    //     Log.w(LOG_TAG, "Error while calling remote computeClickPointInScreen", re);
    // }
    return NOERROR;
}

ECode CAccessibilityInteractionClient::ClearCache()
{
    sAccessibilityCache->Clear();
    return NOERROR;
}

ECode CAccessibilityInteractionClient::OnAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    sAccessibilityCache->OnAccessibilityEvent(event);
    return NOERROR;
}

AutoPtr<IAccessibilityNodeInfo> CAccessibilityInteractionClient::GetFindAccessibilityNodeInfoResultAndClear(
    /* [in] */ Int32 interactionId)
{
    AutoPtr<IAccessibilityNodeInfo> result;
    synchronized(mInstanceLock) {
        Boolean success = WaitForResultTimedLocked(interactionId);
        result = success ? mFindAccessibilityNodeInfoResult : NULL;
        ClearResultLocked();
    }
    return result;
}

ECode CAccessibilityInteractionClient::SetFindAccessibilityNodeInfoResult(
    /* [in] */ IAccessibilityNodeInfo* info,
    /* [in] */ Int32 interactionId)
{
    synchronized(mInstanceLock) {
        if (interactionId > mInteractionId) {
            mFindAccessibilityNodeInfoResult = info;
            mInteractionId = interactionId;
        }
        mInstanceLock.NotifyAll();
    }
    return NOERROR;
}

AutoPtr<IList> CAccessibilityInteractionClient::GetFindAccessibilityNodeInfosResultAndClear(
    /* [in] */ Int32 interactionId)
{
    AutoPtr<IList> result;
    synchronized(mInstanceLock) {
        Boolean success = WaitForResultTimedLocked(interactionId);
        if (success) {
            result = mFindAccessibilityNodeInfosResult;
        }
        else {
            AutoPtr<ICollections> collections;
            CCollections::AcquireSingleton((ICollections**)&collections);
            collections->GetEmptyList((IList**)&result);
        }

        ClearResultLocked();
        if (Build::IS_DEBUGGABLE && CHECK_INTEGRITY) {
            CheckFindAccessibilityNodeInfoResultIntegrity(result);
        }
    }
    return result;
}

ECode CAccessibilityInteractionClient::SetFindAccessibilityNodeInfosResult(
    /* [in] */ IList* infos,
    /* [in] */ Int32 interactionId)
{
    synchronized(mInstanceLock) {
        if (interactionId > mInteractionId) {
            if (infos != NULL) {
                // If the call is not an IPC, i.e. it is made from the same process, we need to
                // instantiate new result list to avoid passing internal instances to clients.
                Boolean isIpcCall = (Binder::GetCallingPid() != Process::MyPid());
                if (!isIpcCall){
                    mFindAccessibilityNodeInfosResult = NULL;
                    CArrayList::New(ICollection::Probe(infos), (IList**)&mFindAccessibilityNodeInfosResult);
                }
                else {
                    mFindAccessibilityNodeInfosResult = infos;
                }
            }
            else {
                AutoPtr<ICollections> collections;
                CCollections::AcquireSingleton((ICollections**)&collections);
                mFindAccessibilityNodeInfosResult = NULL;
                collections->GetEmptyList((IList**)&mFindAccessibilityNodeInfosResult);
            }
            mInteractionId = interactionId;
        }
        mInstanceLock.NotifyAll();
    }
    return NOERROR;
}

Boolean CAccessibilityInteractionClient::GetPerformAccessibilityActionResultAndClear(
    /* [in] */ Int32 interactionId)
{
    Boolean result;
    synchronized(mInstanceLock) {
        Boolean success = WaitForResultTimedLocked(interactionId);
        result = success ? mPerformAccessibilityActionResult : FALSE;
        ClearResultLocked();
    }
    return result;
}

ECode CAccessibilityInteractionClient::SetPerformAccessibilityActionResult(
    /* [in] */ Boolean succeeded,
    /* [in] */ Int32 interactionId)
{
    synchronized(mInstanceLock) {
        if (interactionId > mInteractionId) {
            mPerformAccessibilityActionResult = succeeded;
            mInteractionId = interactionId;
        }
        mInstanceLock.NotifyAll();
    }
    return NOERROR;
}

AutoPtr<IPoint> CAccessibilityInteractionClient::GetComputeClickPointInScreenResultAndClear(
    /* [in] */ Int32 interactionId)
{
    AutoPtr<IPoint> result;
    synchronized(mInstanceLock) {
        Boolean success = WaitForResultTimedLocked(interactionId);
        result = success ? mComputeClickPointResult : NULL;
        ClearResultLocked();
    }
    return result;
}

ECode CAccessibilityInteractionClient::SetComputeClickPointInScreenActionResult(
    /* [in] */ IPoint* point,
    /* [in] */ Int32 interactionId)
{
    synchronized(mInstanceLock) {
        if (interactionId > mInteractionId) {
            mComputeClickPointResult = point;
            mInteractionId = interactionId;
        }
        mInstanceLock.NotifyAll();
    }
    return NOERROR;
}

void CAccessibilityInteractionClient::ClearResultLocked()
{
    mInteractionId = -1;
    mFindAccessibilityNodeInfoResult = NULL;
    mPerformAccessibilityActionResult = FALSE;
    mComputeClickPointResult = NULL;

    mFindAccessibilityNodeInfosResult->Clear();
}

Boolean CAccessibilityInteractionClient::WaitForResultTimedLocked(
    /* [in] */ Int32 interactionId)
{
    Int64 waitTimeMillis = TIMEOUT_INTERACTION_MILLIS;
    Int64 startTimeMillis = SystemClock::GetUptimeMillis();
    while (TRUE) {
        // try {
        AutoPtr<IMessage> sameProcessMessage = GetSameProcessMessageAndClear();
        if (sameProcessMessage != NULL) {
            AutoPtr<IHandler> handler;
            sameProcessMessage->GetTarget((IHandler**)&handler);
            handler->HandleMessage(sameProcessMessage);
        }

        if (mInteractionId == interactionId) {
            return TRUE;
        }
        if (mInteractionId > interactionId) {
            return FALSE;
        }
        Int64 elapsedTimeMillis = SystemClock::GetUptimeMillis() - startTimeMillis;
        waitTimeMillis = TIMEOUT_INTERACTION_MILLIS - elapsedTimeMillis;
        if (waitTimeMillis <= 0) {
            return FALSE;
        }
        mInstanceLock.Wait(waitTimeMillis);
        // } catch (InterruptedException ie) {
        //     /* ignore */
        // }
    }
}

void CAccessibilityInteractionClient::FinalizeAndCacheAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info,
    /* [in] */ Int32 connectionId)
{
    if (info != NULL) {
        info->SetConnectionId(connectionId);
        info->SetSealed(TRUE);
        sAccessibilityCache->Add(info);
    }
}

void CAccessibilityInteractionClient::FinalizeAndCacheAccessibilityNodeInfos(
    /* [in] */ IList* infos,
    /* [in] */ Int32 connectionId)
{
    if (infos != NULL) {
        Int32 infosCount;
        infos->GetSize(&infosCount);
        for (Int32 i = 0; i < infosCount; i++) {
            AutoPtr<IInterface> obj;
            infos->Get(i, (IInterface**)&obj);
            AutoPtr<IAccessibilityNodeInfo> info = IAccessibilityNodeInfo::Probe(obj);
            FinalizeAndCacheAccessibilityNodeInfo(info, connectionId);
        }
    }
}

AutoPtr<IMessage> CAccessibilityInteractionClient::GetSameProcessMessageAndClear()
{
    AutoPtr<IMessage> result;
    synchronized(mInstanceLock) {
        result = mSameThreadMessage;
        mSameThreadMessage = NULL;
    }
    return result;
}

ECode CAccessibilityInteractionClient::GetConnection(
    /* [in] */ Int32 connectionId,
    /* [out] */ IIAccessibilityServiceConnection** connection)
{
    VALIDATE_NOT_NULL(connection);
    *connection = NULL;

    synchronized(sConnectionCache) {
        AutoPtr<IInterface> obj;
        sConnectionCache->Get(connectionId, (IInterface**)&obj);
        *connection = IIAccessibilityServiceConnection::Probe(obj);
        REFCOUNT_ADD(*connection);
    }
    return NOERROR;
}

ECode CAccessibilityInteractionClient::AddConnection(
    /* [in] */ Int32 connectionId,
    /* [in] */ IIAccessibilityServiceConnection* connection)
{
    synchronized(sConnectionCache) {
        sConnectionCache->Put(connectionId, connection);
    }
    return NOERROR;
}

ECode CAccessibilityInteractionClient::RemoveConnection(
    /* [in] */ Int32 connectionId)
{
    synchronized(sConnectionCache) {
        sConnectionCache->Remove(connectionId);
    }
    return NOERROR;
}

void CAccessibilityInteractionClient::CheckFindAccessibilityNodeInfoResultIntegrity(
    /* [in] */ IList* infos)
{
    Int32 infoCount;
    infos->GetSize(&infoCount);
    if (infoCount == 0) {
        return;
    }
    // Find the root node.
    AutoPtr<IInterface> obj;
    infos->Get(0, (IInterface**)&obj);
    AutoPtr<IAccessibilityNodeInfo> root = IAccessibilityNodeInfo::Probe(obj);
    for (Int32 i = 1; i < infoCount; i++) {
        for (Int32 j = i; j < infoCount; j++) {
            obj = NULL;
            infos->Get(j, (IInterface**)&obj);
            AutoPtr<IAccessibilityNodeInfo> candidate = IAccessibilityNodeInfo::Probe(obj);
            Int64 id1, id2;
            if ((root->GetParentNodeId(&id1), id1) == (candidate->GetSourceNodeId(&id2), id2)) {
                root = candidate;
                break;
            }
        }
    }
    if (root == NULL) {
        Slogger::E(TAG, "No root.");
    }
    // Check for duplicates.
    AutoPtr<IHashSet> seen;
    CHashSet::New((IHashSet**)&seen);
    AutoPtr<IQueue> fringe;
    CLinkedList::New((IQueue**)&fringe);
    fringe->Add(root);
    Boolean res;
    while ((fringe->IsEmpty(&res), !res)) {
        AutoPtr<IInterface> obj;
        fringe->Poll((IInterface**)&obj);
        AutoPtr<IAccessibilityNodeInfo> current = IAccessibilityNodeInfo::Probe(obj);
        Boolean suc;
        if ((seen->Add(current, &suc), !suc)) {
            Slogger::E(TAG, "Duplicate node.");
            return;
        }
        Int32 childCount;
        current->GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            Int64 childId;
            current->GetChildId(i, &childId);
            for (Int32 j = 0; j < infoCount; j++) {
                obj = NULL;
                infos->Get(j, (IInterface**)&obj);
                AutoPtr<IAccessibilityNodeInfo> child = IAccessibilityNodeInfo::Probe(obj);
                Int64 nodeId;
                child->GetSourceNodeId(&nodeId);
                if (nodeId == childId) {
                    fringe->Add(child);
                }
            }
        }
    }

    Int32 size;
    seen->GetSize(&size);
    Int32 disconnectedCount = infoCount - size;
    if (disconnectedCount > 0) {
        Slogger::E(TAG, "%d Disconnected nodes.", disconnectedCount);
    }
}

} // Accessibility
} // View
} // Droid
} // Elastos
