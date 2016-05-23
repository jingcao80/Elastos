
#include "elastos/droid/server/media/CMediaRouterService.h"
#include "elastos/droid/server/Watchdog.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Media::IMediaRouter;
using Elastos::Droid::Media::CMediaRouterClientState;
using Elastos::Droid::Media::CMediaRouterClientStateRouteInfo;
using Elastos::Droid::Media::IAudioSystem;
using Elastos::Droid::Media::IMediaRouterRouteInfo;
using Elastos::Droid::Media::EIID_IIMediaRouterService;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Server::Watchdog;
using Elastos::Droid::Server::EIID_IWatchdogMonitor;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Media {

//==============================================================================
//                  CMediaRouterService::ClientRecord
//==============================================================================

CAR_INTERFACE_IMPL(CMediaRouterService::ClientRecord, Object, IProxyDeathRecipient)

void CMediaRouterService::ClientRecord::Dispose()
{
    AutoPtr<IBinder> binder = IBinder::Probe(mClient);
    if (binder != NULL) {
        AutoPtr<IProxy> proxy = (IProxy*)binder->Probe(EIID_IProxy);
        Boolean result;
        proxy->UnlinkToDeath(this, 0, &result);
    }
}

ECode CMediaRouterService::ClientRecord::ProxyDied()
{
    mHost->ClientDied(this);
    return NOERROR;
}

AutoPtr<IMediaRouterClientState> CMediaRouterService::ClientRecord::GetState()
{
    return mTrusted ? mUserRecord->mTrustedState : mUserRecord->mUntrustedState;
}

ECode CMediaRouterService::ClientRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("Client ") + mPackageName + " (pid " + StringUtils::ToString(mPid) + ")";
    return NOERROR;
}


//==============================================================================
//                  CMediaRouterService::UserRecord
//==============================================================================

CMediaRouterService::UserRecord::UserRecord(
    /* [in] */ Int32 userId,
    /* [in] */ CMediaRouterService* host)
    : mUserId(userId)
{
    mHandler = new UserHandler(host, this);
}

ECode CMediaRouterService::UserRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("User ") + StringUtils::ToString(mUserId);
    return NOERROR;
}


//==============================================================================
//                  CMediaRouterService::UserHandler
//==============================================================================

const Int32 CMediaRouterService::UserHandler::MSG_START;
const Int32 CMediaRouterService::UserHandler::MSG_STOP;
const Int32 CMediaRouterService::UserHandler::MSG_UPDATE_DISCOVERY_REQUEST;
const Int32 CMediaRouterService::UserHandler::MSG_SELECT_ROUTE;
const Int32 CMediaRouterService::UserHandler::MSG_UNSELECT_ROUTE;
const Int32 CMediaRouterService::UserHandler::MSG_REQUEST_SET_VOLUME;
const Int32 CMediaRouterService::UserHandler::MSG_REQUEST_UPDATE_VOLUME;
const Int32 CMediaRouterService::UserHandler::MSG_UPDATE_CLIENT_STATE;
const Int32 CMediaRouterService::UserHandler::MSG_CONNECTION_TIMED_OUT;
const Int32 CMediaRouterService::UserHandler::TIMEOUT_REASON_NOT_AVAILABLE;
const Int32 CMediaRouterService::UserHandler::TIMEOUT_REASON_CONNECTION_LOST;
const Int32 CMediaRouterService::UserHandler::TIMEOUT_REASON_WAITING_FOR_CONNECTING;
const Int32 CMediaRouterService::UserHandler::TIMEOUT_REASON_WAITING_FOR_CONNECTED;
const Int32 CMediaRouterService::UserHandler::PHASE_NOT_AVAILABLE;
const Int32 CMediaRouterService::UserHandler::PHASE_NOT_CONNECTED;
const Int32 CMediaRouterService::UserHandler::PHASE_CONNECTING;
const Int32 CMediaRouterService::UserHandler::PHASE_CONNECTED;

CMediaRouterService::UserHandler::UserHandler(
    /* [in] */ CMediaRouterService* service,
    /* [in] */ UserRecord* userRecord)
    : mService(service)
    , mUserRecord(userRecord)
    , mRunning(FALSE)
    , mDiscoveryMode(IRemoteDisplayState::DISCOVERY_MODE_NONE)
    , mConnectionPhase(PHASE_NOT_AVAILABLE)
    , mConnectionTimeoutReason(0)
    , mConnectionTimeoutStartTime(0)
    , mClientStateUpdateScheduled(FALSE)
{
    mWatcher = new RemoteDisplayProviderWatcher(service->mContext, (RemoteDisplayProviderWatcher::ICallback*)this,
            this, mUserRecord->mUserId);
}

PInterface CMediaRouterService::UserHandler::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IRemoteDisplayProviderWatcherCallback) {
        return (RemoteDisplayProviderWatcher::ICallback*)this;
    }
    else if (riid == EIID_IRemoteDisplayProviderProxyCallback) {
        return (RemoteDisplayProviderProxy::ICallback*)this;
    }
    else return Handler::Probe(riid);
}

UInt32 CMediaRouterService::UserHandler::AddRef()
{
    return Handler::AddRef();
}

UInt32 CMediaRouterService::UserHandler::Release()
{
    return Handler::Release();
}

ECode CMediaRouterService::UserHandler::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid)
    if (object == (RemoteDisplayProviderWatcher::ICallback*)this) {
        *iid = EIID_IRemoteDisplayProviderWatcherCallback;
        return NOERROR;
    }
    else if (object == (RemoteDisplayProviderProxy::ICallback*)this) {
        *iid = EIID_IRemoteDisplayProviderProxyCallback;
        return NOERROR;
    }
    else return Handler::GetInterfaceID(object, iid);
}

ECode CMediaRouterService::UserHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_START: {
            Start();
            break;
        }
        case MSG_STOP: {
            Stop();
            break;
        }
        case MSG_UPDATE_DISCOVERY_REQUEST: {
            UpdateDiscoveryRequest();
            break;
        }
        case MSG_SELECT_ROUTE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
            String str;
            cs->ToString(&str);
            SelectRoute(str);
            break;
        }
        case MSG_UNSELECT_ROUTE: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
            String str;
            cs->ToString(&str);
            UnselectRoute(str);
            break;
        }
        case MSG_REQUEST_SET_VOLUME: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
            String str;
            cs->ToString(&str);
            Int32 arg1;
            msg->GetArg1(&arg1);
            RequestSetVolume(str, arg1);
            break;
        }
        case MSG_REQUEST_UPDATE_VOLUME: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
            String str;
            cs->ToString(&str);
            Int32 arg1;
            msg->GetArg1(&arg1);
            RequestUpdateVolume(str, arg1);
            break;
        }
        case MSG_UPDATE_CLIENT_STATE: {
            UpdateClientState();
            break;
        }
        case MSG_CONNECTION_TIMED_OUT: {
            ConnectionTimedOut();
            break;
        }
    }
    return NOERROR;
}

void CMediaRouterService::UserHandler::Start()
{
    if (!mRunning) {
        mRunning = TRUE;
        mWatcher->Start(); // also starts all providers
    }
}

void CMediaRouterService::UserHandler::Stop()
{
    if (mRunning) {
        mRunning = FALSE;
        UnselectGloballySelectedRoute();
        mWatcher->Stop(); // also stops all providers
    }
}

void CMediaRouterService::UserHandler::UpdateDiscoveryRequest()
{
    Int32 routeTypes = 0;
    Boolean activeScan = FALSE;
    {
        AutoLock lock(mService->mLock);
        List<AutoPtr<ClientRecord> >::Iterator it = mUserRecord->mClientRecords.Begin();
        for (; it != mUserRecord->mClientRecords.End(); ++it) {
            AutoPtr<ClientRecord> clientRecord = *it;
            routeTypes |= clientRecord->mRouteTypes;
            activeScan |= clientRecord->mActiveScan;
        }
    }

    Int32 newDiscoveryMode;
    if ((routeTypes & IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY) != 0) {
        if (activeScan) {
            newDiscoveryMode = IRemoteDisplayState::DISCOVERY_MODE_ACTIVE;
        }
        else {
            newDiscoveryMode = IRemoteDisplayState::DISCOVERY_MODE_PASSIVE;
        }
    }
    else {
        newDiscoveryMode = IRemoteDisplayState::DISCOVERY_MODE_NONE;
    }

    if (mDiscoveryMode != newDiscoveryMode) {
        mDiscoveryMode = newDiscoveryMode;
        List<AutoPtr<ProviderRecord> >::Iterator it = mProviderRecords.Begin();
        for (; it != mProviderRecords.End(); ++it) {
            (*it)->GetProvider()->SetDiscoveryMode(mDiscoveryMode);
        }
    }
}

void CMediaRouterService::UserHandler::SelectRoute(
    /* [in] */ const String& routeId)
{
    if (!routeId.IsNull()
            && (mGloballySelectedRouteRecord == NULL
                    || !routeId.Equals(mGloballySelectedRouteRecord->GetUniqueId()))) {
        AutoPtr<RouteRecord> routeRecord = FindRouteRecord(routeId);
        if (routeRecord != NULL) {
            UnselectGloballySelectedRoute();

            Slogger::I(CMediaRouterService::TAG, "Selected global route:%p", routeRecord.Get());
            mGloballySelectedRouteRecord = routeRecord;
            CheckGloballySelectedRouteState();
            routeRecord->GetProvider()->SetSelectedDisplay(routeRecord->GetDescriptorId());

            ScheduleUpdateClientState();
        }
    }
}

void CMediaRouterService::UserHandler::UnselectRoute(
    /* [in] */ const String& routeId)
{
    if (!routeId.IsNull()
            && mGloballySelectedRouteRecord != NULL
            && routeId.Equals(mGloballySelectedRouteRecord->GetUniqueId())) {
        UnselectGloballySelectedRoute();
    }
}

void CMediaRouterService::UserHandler::UnselectGloballySelectedRoute()
{
    if (mGloballySelectedRouteRecord != NULL) {
        Slogger::I(TAG, "Unselected global route:%p", mGloballySelectedRouteRecord.Get());
        mGloballySelectedRouteRecord->GetProvider()->SetSelectedDisplay(String(NULL));
        mGloballySelectedRouteRecord = NULL;
        CheckGloballySelectedRouteState();

        ScheduleUpdateClientState();
    }
}

void CMediaRouterService::UserHandler::RequestSetVolume(
    /* [in] */ const String& routeId,
    /* [in] */ Int32 volume)
{
    if (mGloballySelectedRouteRecord != NULL
            && routeId.Equals(mGloballySelectedRouteRecord->GetUniqueId())) {
        mGloballySelectedRouteRecord->GetProvider()->SetDisplayVolume(volume);
    }
}

void CMediaRouterService::UserHandler::RequestUpdateVolume(
    /* [in] */ const String& routeId,
    /* [in] */ Int32 direction)
{
    if (mGloballySelectedRouteRecord != NULL
            && routeId.Equals(mGloballySelectedRouteRecord->GetUniqueId())) {
        mGloballySelectedRouteRecord->GetProvider()->AdjustDisplayVolume(direction);
    }
}

ECode CMediaRouterService::UserHandler::AddProvider(
    /* [in] */ RemoteDisplayProviderProxy* provider)
{
    provider->SetCallback((RemoteDisplayProviderProxy::ICallback*)this);
    provider->SetDiscoveryMode(mDiscoveryMode);
    provider->SetSelectedDisplay(String(NULL)); // just to be safe

    AutoPtr<ProviderRecord> providerRecord = new ProviderRecord(provider);
    mProviderRecords.PushBack(providerRecord);
    AutoPtr<IRemoteDisplayState> rds = provider->GetDisplayState();
    providerRecord->UpdateDescriptor(rds);

    ScheduleUpdateClientState();
    return NOERROR;
}

ECode CMediaRouterService::UserHandler::RemoveProvider(
    /* [in] */ RemoteDisplayProviderProxy* provider)
{
    List<AutoPtr<ProviderRecord> >::Iterator it = FindProviderRecord(provider);
    if (it != mProviderRecords.End()) {
        AutoPtr<ProviderRecord> providerRecord = *it;
        mProviderRecords.Erase(it);
        providerRecord->UpdateDescriptor(NULL); // mark routes invalid
        provider->SetCallback(NULL);
        provider->SetDiscoveryMode(IRemoteDisplayState::DISCOVERY_MODE_NONE);

        CheckGloballySelectedRouteState();
        ScheduleUpdateClientState();
    }
    return NOERROR;
}

ECode CMediaRouterService::UserHandler::OnDisplayStateChanged(
    /* [in] */ RemoteDisplayProviderProxy* provider,
    /* [in] */ IRemoteDisplayState* state)
{
    UpdateProvider(provider, state);
    return NOERROR;
}

void CMediaRouterService::UserHandler::UpdateProvider(
    /* [in] */ RemoteDisplayProviderProxy* provider,
    /* [in] */ IRemoteDisplayState* state)
{
    List<AutoPtr<ProviderRecord> >::Iterator it = FindProviderRecord(provider);
    if (it != mProviderRecords.End()) {
        AutoPtr<ProviderRecord> providerRecord = *it;
        if (providerRecord->UpdateDescriptor(state)) {
            CheckGloballySelectedRouteState();
            ScheduleUpdateClientState();
        }
    }
}

void CMediaRouterService::UserHandler::CheckGloballySelectedRouteState()
{
    // Unschedule timeouts when the route is unselected.
    if (mGloballySelectedRouteRecord == NULL) {
        mConnectionPhase = PHASE_NOT_AVAILABLE;
        UpdateConnectionTimeout(0);
        return;
    }

    // Ensure that the route is still present and enabled.
    if (!mGloballySelectedRouteRecord->IsValid()
            || !mGloballySelectedRouteRecord->IsEnabled()) {
        UpdateConnectionTimeout(TIMEOUT_REASON_NOT_AVAILABLE);
        return;
    }

    // Make sure we haven't lost our connection.
    Int32 oldPhase = mConnectionPhase;
    mConnectionPhase = GetConnectionPhase(mGloballySelectedRouteRecord->GetStatus());
    if (oldPhase >= PHASE_CONNECTING && mConnectionPhase < PHASE_CONNECTING) {
        UpdateConnectionTimeout(TIMEOUT_REASON_CONNECTION_LOST);
        return;
    }

    // Check the route status.
    switch (mConnectionPhase) {
        case PHASE_CONNECTED:
            if (oldPhase != PHASE_CONNECTED) {
                Slogger::I(TAG, "Connected to global route: %p", mGloballySelectedRouteRecord.Get());
            }
            UpdateConnectionTimeout(0);
            break;
        case PHASE_CONNECTING:
            if (oldPhase != PHASE_CONNECTING) {
                Slogger::I(TAG, "Connecting to global route: %p", mGloballySelectedRouteRecord.Get());
            }
            UpdateConnectionTimeout(TIMEOUT_REASON_WAITING_FOR_CONNECTED);
            break;
        case PHASE_NOT_CONNECTED:
            UpdateConnectionTimeout(TIMEOUT_REASON_WAITING_FOR_CONNECTING);
            break;
        case PHASE_NOT_AVAILABLE:
        default:
            UpdateConnectionTimeout(TIMEOUT_REASON_NOT_AVAILABLE);
            break;
    }
}

void CMediaRouterService::UserHandler::UpdateConnectionTimeout(
    /* [in] */ Int32 reason)
{
    if (reason != mConnectionTimeoutReason) {
        if (mConnectionTimeoutReason != 0) {
            RemoveMessages(MSG_CONNECTION_TIMED_OUT);
        }
        mConnectionTimeoutReason = reason;
        mConnectionTimeoutStartTime = SystemClock::GetUptimeMillis();
        switch (reason) {
            case TIMEOUT_REASON_NOT_AVAILABLE:
            case TIMEOUT_REASON_CONNECTION_LOST: {
                // Route became unavailable or connection lost.
                // Unselect it immediately.
                Boolean result;
                SendEmptyMessage(MSG_CONNECTION_TIMED_OUT, &result);
                break;
            }
            case TIMEOUT_REASON_WAITING_FOR_CONNECTING: {
                // Waiting for route to start connecting.
                Boolean result;
                SendEmptyMessageDelayed(MSG_CONNECTION_TIMED_OUT, CONNECTING_TIMEOUT, &result);
                break;
            }
            case TIMEOUT_REASON_WAITING_FOR_CONNECTED: {
                // Waiting for route to complete connection.
                Boolean result;
                SendEmptyMessageDelayed(MSG_CONNECTION_TIMED_OUT, CONNECTED_TIMEOUT, &result);
                break;
            }
        }
    }
}

void CMediaRouterService::UserHandler::ConnectionTimedOut()
{
    if (mConnectionTimeoutReason == 0 || mGloballySelectedRouteRecord == NULL) {
        // Shouldn't get here.  There must be a bug somewhere.
        Slogger::I(TAG, "Handled connection timeout for no reason.");
        return;
    }

    switch (mConnectionTimeoutReason) {
        case TIMEOUT_REASON_NOT_AVAILABLE:
            Slogger::I(TAG, "Global route no longer available: %p", mGloballySelectedRouteRecord.Get());
            break;
        case TIMEOUT_REASON_CONNECTION_LOST:
            Slogger::I(TAG, "Global route connection lost: %p", mGloballySelectedRouteRecord.Get());
            break;
        case TIMEOUT_REASON_WAITING_FOR_CONNECTING:
            Slogger::I(TAG, "Global route timed out while waiting for connection attempt to begin after %d ms: %p",
                    SystemClock::GetUptimeMillis() - mConnectionTimeoutStartTime, mGloballySelectedRouteRecord.Get());
            break;
        case TIMEOUT_REASON_WAITING_FOR_CONNECTED:
            Slogger::I(TAG, "Global route timed out while connecting after %d ms: %p",
                    SystemClock::GetUptimeMillis() - mConnectionTimeoutStartTime, mGloballySelectedRouteRecord.Get());
            break;
    }
    mConnectionTimeoutReason = 0;

    UnselectGloballySelectedRoute();
}

void CMediaRouterService::UserHandler::ScheduleUpdateClientState()
{
    if (!mClientStateUpdateScheduled) {
        mClientStateUpdateScheduled = TRUE;
        Boolean result;
        SendEmptyMessage(MSG_UPDATE_CLIENT_STATE, &result);
    }
}

void CMediaRouterService::UserHandler::UpdateClientState()
{
    mClientStateUpdateScheduled = FALSE;

    String globallySelectedRouteId = mGloballySelectedRouteRecord != NULL ?
            mGloballySelectedRouteRecord->GetUniqueId() : String(NULL);

    // Build a new client state for trusted clients.
    AutoPtr<IMediaRouterClientState> trustedState;
    CMediaRouterClientState::New((IMediaRouterClientState**)&trustedState);
    trustedState->SetGloballySelectedRouteId(globallySelectedRouteId);
    List<AutoPtr<ProviderRecord> >::Iterator it = mProviderRecords.Begin();
    for (; it != mProviderRecords.End(); ++it) {
        (*it)->AppendClientState(trustedState);
    }

    // Build a new client state for untrusted clients that can only see
    // the currently selected route.
    AutoPtr<IMediaRouterClientState> untrustedState;
    CMediaRouterClientState::New((IMediaRouterClientState**)&untrustedState);
    untrustedState->SetGloballySelectedRouteId(globallySelectedRouteId);
    if (!globallySelectedRouteId.IsNull()) {
        AutoPtr<IArrayList> routes;
        untrustedState->GetRoutes((IArrayList**)&routes);
        AutoPtr<IMediaRouterClientStateRouteInfo> routeInfo;
        trustedState->GetRoute(globallySelectedRouteId, (IMediaRouterClientStateRouteInfo**)&routeInfo);
        routes->Add(routeInfo);
    }

    // try {
    {
        AutoLock lock(mService->mLock);
        // Update the UserRecord.
        mUserRecord->mTrustedState = trustedState;
        mUserRecord->mUntrustedState = untrustedState;

        // Collect all clients.
        List<AutoPtr<ClientRecord> >::Iterator it = mUserRecord->mClientRecords.Begin();
        for (; it != mUserRecord->mClientRecords.End(); ++it) {
            mTempClients.PushBack((*it)->mClient);
        }
    }

    // Notify all clients (outside of the lock).
    List<AutoPtr<IIMediaRouterClient> >::Iterator clientIt = mTempClients.Begin();
    for (; clientIt != mTempClients.End(); ++clientIt) {
        // try {
        (*clientIt)->OnStateChanged();
        // } catch (RemoteException ex) {
        //     // ignore errors, client probably died
        // }
    }
    // } finally {
    //     // Clear the list in preparation for the next time.
    //     mTempClients.clear();
    // }
    mTempClients.Clear();
}

List<AutoPtr<CMediaRouterService::UserHandler::ProviderRecord> >::Iterator CMediaRouterService::UserHandler::FindProviderRecord(
    /* [in] */ RemoteDisplayProviderProxy* provider)
{
    List<AutoPtr<ProviderRecord> >::Iterator it = mProviderRecords.Begin();
    for (; it != mProviderRecords.End(); ++it) {
        AutoPtr<ProviderRecord> record = *it;
        if (record->GetProvider().Get() == provider) {
            return it;
        }
    }
    return mProviderRecords.End();
}

AutoPtr<CMediaRouterService::UserHandler::RouteRecord> CMediaRouterService::UserHandler::FindRouteRecord(
    /* [in] */ const String& uniqueId)
{
    List<AutoPtr<ProviderRecord> >::Iterator it = mProviderRecords.Begin();
    for (; it != mProviderRecords.End(); ++it) {
        AutoPtr<RouteRecord> record = (*it)->FindRouteByUniqueId(uniqueId);
        if (record != NULL) {
            return record;
        }
    }
    return NULL;
}

Int32 CMediaRouterService::UserHandler::GetConnectionPhase(
    /* [in] */ Int32 status)
{
    switch (status) {
        case IMediaRouterRouteInfo::STATUS_NONE:
        case IMediaRouterRouteInfo::STATUS_CONNECTED:
            return PHASE_CONNECTED;
        case IMediaRouterRouteInfo::STATUS_CONNECTING:
            return PHASE_CONNECTING;
        case IMediaRouterRouteInfo::STATUS_SCANNING:
        case IMediaRouterRouteInfo::STATUS_AVAILABLE:
            return PHASE_NOT_CONNECTED;
        case IMediaRouterRouteInfo::STATUS_NOT_AVAILABLE:
        case IMediaRouterRouteInfo::STATUS_IN_USE:
        default:
            return PHASE_NOT_AVAILABLE;
    }
}


//==============================================================================
//                  CMediaRouterService::UserHandler::ProviderRecord
//==============================================================================

CMediaRouterService::UserHandler::ProviderRecord::ProviderRecord(
    /* [in] */ RemoteDisplayProviderProxy* provider)
{
    mProvider = provider;
    mUniquePrefix = provider->GetFlattenedComponentName() + ":";
    CArrayList::New((IArrayList**)&mRoutes);
}

AutoPtr<RemoteDisplayProviderProxy> CMediaRouterService::UserHandler::ProviderRecord::GetProvider()
{
    return mProvider;
}

String CMediaRouterService::UserHandler::ProviderRecord::GetUniquePrefix()
{
    return mUniquePrefix;
}

Boolean CMediaRouterService::UserHandler::ProviderRecord::UpdateDescriptor(
    /* [in] */ IRemoteDisplayState* descriptor)
{
    Boolean changed = FALSE;
    if (mDescriptor.Get() != descriptor) {
        mDescriptor = descriptor;

        // Update all existing routes and reorder them to match
        // the order of their descriptors.
        Int32 targetIndex = 0;
        if (descriptor != NULL) {
            Boolean isValid;
            if (descriptor->IsValid(&isValid), isValid) {
                AutoPtr<IArrayList> routeDescriptors;
                descriptor->GetDisplays((IArrayList**)&routeDescriptors);
                AutoPtr<IIterator> iterator;
                routeDescriptors->GetIterator((IIterator**)&iterator);
                Boolean hasNext;
                while (iterator->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> next;
                    iterator->GetNext((IInterface**)&next);
                    AutoPtr<IRemoteDisplayStateRemoteDisplayInfo> routeDescriptor
                            = IRemoteDisplayStateRemoteDisplayInfo::Probe(next);
                    String descriptorId;
                    routeDescriptor->GetId(&descriptorId);
                    Int32 sourceIndex = FindRouteByDescriptorId(descriptorId);
                    if (sourceIndex < 0) {
                        // Add the route to the provider.
                        String uniqueId = AssignRouteUniqueId(descriptorId);
                        AutoPtr<RouteRecord> route =
                                new RouteRecord(this, descriptorId, uniqueId);
                        mRoutes->Add(targetIndex++, (IObject*)route.Get());
                        route->UpdateDescriptor(routeDescriptor);
                        changed = TRUE;
                    }
                    else if (sourceIndex < targetIndex) {
                        // Ignore route with duplicate id.
                        Slogger::W(TAG, "Ignoring route descriptor with duplicate id: %p", routeDescriptor.Get());
                    }
                    else {
                        // Reorder existing route within the list.
                        AutoPtr<IInterface> value;
                        mRoutes->Get(sourceIndex, (IInterface**)&value);
                        AutoPtr<RouteRecord> route = (RouteRecord*)(IObject*)value.Get();
                        AutoPtr<ICollections> cols;
                        CCollections::AcquireSingleton((ICollections**)&cols);
                        cols->Swap(IList::Probe(mRoutes), sourceIndex, targetIndex++);
                        changed |= route->UpdateDescriptor(routeDescriptor);
                    }
                }
            }
            else {
                Slogger::W(TAG, "Ignoring invalid descriptor from media route provider: %s",
                        mProvider->GetFlattenedComponentName().string());
            }
        }

        // Dispose all remaining routes that do not have matching descriptors.
        Int32 size;
        mRoutes->GetSize(&size);
        for (Int32 i = size - 1; i >= targetIndex; i--) {
            AutoPtr<IInterface> value;
            mRoutes->Remove(i, (IInterface**)&value);
            AutoPtr<RouteRecord> route = (RouteRecord*)(IObject*)value.Get();
            route->UpdateDescriptor(NULL); // mark route invalid
            changed = TRUE;
        }
    }
    return changed;
}

void CMediaRouterService::UserHandler::ProviderRecord::AppendClientState(
    /* [in] */ IMediaRouterClientState* state)
{
    Int32 routeCount;
    mRoutes->GetSize(&routeCount);
    for (Int32 i = 0; i < routeCount; i++) {
        AutoPtr<IArrayList> routes;
        state->GetRoutes((IArrayList**)&routes);
        AutoPtr<IInterface> value;
        mRoutes->Get(i, (IInterface**)&value);
        AutoPtr<RouteRecord> route = (RouteRecord*)(IObject*)value.Get();
        routes->Add(route->GetInfo());
    }
}

AutoPtr<CMediaRouterService::UserHandler::RouteRecord> CMediaRouterService::UserHandler::ProviderRecord::FindRouteByUniqueId(
    /* [in] */ const String& uniqueId)
{
    Int32 routeCount;
    mRoutes->GetSize(&routeCount);
    for (Int32 i = 0; i < routeCount; i++) {
        AutoPtr<IInterface> value;
        mRoutes->Get(i, (IInterface**)&value);
        AutoPtr<RouteRecord> route = (RouteRecord*)(IObject*)value.Get();
        if (route->GetUniqueId().Equals(uniqueId)) {
            return route;
        }
    }
    return NULL;
}

Int32 CMediaRouterService::UserHandler::ProviderRecord::FindRouteByDescriptorId(
    /* [in] */ const String& descriptorId)
{
    Int32 routeCount;
    mRoutes->GetSize(&routeCount);
    for (Int32 i = 0; i < routeCount; i++) {
        AutoPtr<IInterface> value;
        mRoutes->Get(i, (IInterface**)&value);
        AutoPtr<RouteRecord> route = (RouteRecord*)(IObject*)value.Get();
        if (route->GetDescriptorId().Equals(descriptorId)) {
            return i;
        }
    }
    return -1;
}

ECode CMediaRouterService::UserHandler::ProviderRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("Provider ") + mProvider->GetFlattenedComponentName();
    return NOERROR;
}

String CMediaRouterService::UserHandler::ProviderRecord::AssignRouteUniqueId(
    /* [in] */ const String& descriptorId)
{
    return mUniquePrefix + descriptorId;
}


//==============================================================================
//                  CMediaRouterService::UserHandler::RouteRecord
//==============================================================================

CMediaRouterService::UserHandler::RouteRecord::RouteRecord(
    /* [in] */ ProviderRecord* providerRecord,
    /* [in] */ const String& descriptorId,
    /* [in] */ const String& uniqueId)
    : mProviderRecord(providerRecord)
    , mDescriptorId(descriptorId)
{
    CMediaRouterClientStateRouteInfo::New(uniqueId, (IMediaRouterClientStateRouteInfo**)&mMutableInfo);
}

AutoPtr<RemoteDisplayProviderProxy> CMediaRouterService::UserHandler::RouteRecord::GetProvider()
{
    return mProviderRecord->GetProvider();
}

AutoPtr<CMediaRouterService::UserHandler::ProviderRecord> CMediaRouterService::UserHandler::RouteRecord::GetProviderRecord()
{
    return mProviderRecord;
}

String CMediaRouterService::UserHandler::RouteRecord::GetDescriptorId()
{
    return mDescriptorId;
}

String CMediaRouterService::UserHandler::RouteRecord::GetUniqueId()
{
    String id;
    mMutableInfo->GetId(&id);
    return id;
}

AutoPtr<IMediaRouterClientStateRouteInfo> CMediaRouterService::UserHandler::RouteRecord::GetInfo()
{
    if (mImmutableInfo == NULL) {
        CMediaRouterClientStateRouteInfo::New(mMutableInfo, (IMediaRouterClientStateRouteInfo**)&mImmutableInfo);
    }
    return mImmutableInfo;
}

Boolean CMediaRouterService::UserHandler::RouteRecord::IsValid()
{
    return mDescriptor != NULL;
}

Boolean CMediaRouterService::UserHandler::RouteRecord::IsEnabled()
{
    Boolean enabled;
    mMutableInfo->GetEnabled(&enabled);
    return enabled;
}

Int32 CMediaRouterService::UserHandler::RouteRecord::GetStatus()
{
    Int32 statusCode;
    mMutableInfo->GetStatusCode(&statusCode);
    return statusCode;
}

Boolean CMediaRouterService::UserHandler::RouteRecord::UpdateDescriptor(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    Boolean changed = FALSE;
    if (mDescriptor.Get() != descriptor) {
        mDescriptor = descriptor;
        if (descriptor != NULL) {
            String name = ComputeName(descriptor);
            String infoName;
            mMutableInfo->GetName(&infoName);
            if (!infoName.Equals(name)) {
                mMutableInfo->SetName(name);
                changed = TRUE;
            }
            String description = ComputeDescription(descriptor);
            String infoDescription;
            mMutableInfo->GetDescription(&infoDescription);
            if (!infoDescription.Equals(description)) {
                mMutableInfo->SetDescription(description);
                changed = TRUE;
            }
            Int32 supportedTypes = ComputeSupportedTypes(descriptor);
            Int32 infoSupportedTypes;
            if (mMutableInfo->GetSupportedTypes(&infoSupportedTypes), infoSupportedTypes != supportedTypes) {
                mMutableInfo->SetSupportedTypes(supportedTypes);
                changed = TRUE;
            }
            Boolean enabled = ComputeEnabled(descriptor);
            Boolean infoEnabled;
            if (mMutableInfo->GetEnabled(&infoEnabled), infoEnabled != enabled) {
                mMutableInfo->SetEnabled(enabled);
                changed = TRUE;
            }
            Int32 statusCode = ComputeStatusCode(descriptor);
            Int32 infoStatusCode;
            if (mMutableInfo->GetStatusCode(&infoStatusCode), infoStatusCode != statusCode) {
                mMutableInfo->SetStatusCode(statusCode);
                changed = TRUE;
            }
            Int32 playbackType = ComputePlaybackType(descriptor);
            Int32 infoPlaybackType;
            if (mMutableInfo->GetPlaybackType(&infoPlaybackType), infoPlaybackType != playbackType) {
                mMutableInfo->SetPlaybackType(playbackType);
                changed = TRUE;
            }
            Int32 playbackStream = ComputePlaybackStream(descriptor);
            Int32 infoPlaybackStream;
            if (mMutableInfo->GetPlaybackStream(&infoPlaybackStream), infoPlaybackStream != playbackStream) {
                mMutableInfo->SetPlaybackStream(playbackStream);
                changed = TRUE;
            }
            Int32 volume = ComputeVolume(descriptor);
            Int32 infoVolume;
            if (mMutableInfo->GetVolume(&infoVolume), infoVolume != volume) {
                mMutableInfo->SetVolume(volume);
                changed = TRUE;
            }
            Int32 volumeMax = ComputeVolumeMax(descriptor);
            Int32 infoVolumeMax;
            if (mMutableInfo->GetVolumeMax(&infoVolumeMax), infoVolumeMax != volumeMax) {
                mMutableInfo->SetVolumeMax(volumeMax);
                changed = TRUE;
            }
            Int32 volumeHandling = ComputeVolumeHandling(descriptor);
            Int32 infoVolumeHandling;
            if (mMutableInfo->GetVolumeHandling(&infoVolumeHandling), infoVolumeHandling != volumeHandling) {
                mMutableInfo->SetVolumeHandling(volumeHandling);
                changed = TRUE;
            }
            Int32 presentationDisplayId = ComputePresentationDisplayId(descriptor);
            Int32 infoDisplayid;
            if (mMutableInfo->GetPresentationDisplayId(&infoDisplayid), infoDisplayid != presentationDisplayId) {
                mMutableInfo->SetPresentationDisplayId(presentationDisplayId);
                changed = TRUE;
            }
        }
    }
    if (changed) {
        mImmutableInfo = NULL;
    }
    return changed;
}

ECode CMediaRouterService::UserHandler::RouteRecord::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    String name;
    mMutableInfo->GetName(&name);
    String id;
    mMutableInfo->GetId(&id);
    *str = String("Route ") + name + " (" + id + ")";
    return NOERROR;
}

String CMediaRouterService::UserHandler::RouteRecord::ComputeName(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    // Note that isValid() already ensures the name is non-empty.
    String name;
    descriptor->GetName(&name);
    return name;
}

String CMediaRouterService::UserHandler::RouteRecord::ComputeDescription(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    String description;
    descriptor->GetDescription(&description);
    return TextUtils::IsEmpty(description) ? String(NULL) : description;
}

Int32 CMediaRouterService::UserHandler::RouteRecord::ComputeSupportedTypes(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    return IMediaRouter::ROUTE_TYPE_LIVE_AUDIO
            | IMediaRouter::ROUTE_TYPE_LIVE_VIDEO
            | IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY;
}

Boolean CMediaRouterService::UserHandler::RouteRecord::ComputeEnabled(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    Int32 status;
    descriptor->GetStatus(&status);
    switch (status) {
        case IRemoteDisplayStateRemoteDisplayInfo::STATUS_CONNECTED:
        case IRemoteDisplayStateRemoteDisplayInfo::STATUS_CONNECTING:
        case IRemoteDisplayStateRemoteDisplayInfo::STATUS_AVAILABLE:
            return TRUE;
        default:
            return FALSE;
    }
}

Int32 CMediaRouterService::UserHandler::RouteRecord::ComputeStatusCode(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    Int32 status;
    descriptor->GetStatus(&status);
    switch (status) {
        case IRemoteDisplayStateRemoteDisplayInfo::STATUS_NOT_AVAILABLE:
            return IMediaRouterRouteInfo::STATUS_NOT_AVAILABLE;
        case IRemoteDisplayStateRemoteDisplayInfo::STATUS_AVAILABLE:
            return IMediaRouterRouteInfo::STATUS_AVAILABLE;
        case IRemoteDisplayStateRemoteDisplayInfo::STATUS_IN_USE:
            return IMediaRouterRouteInfo::STATUS_IN_USE;
        case IRemoteDisplayStateRemoteDisplayInfo::STATUS_CONNECTING:
            return IMediaRouterRouteInfo::STATUS_CONNECTING;
        case IRemoteDisplayStateRemoteDisplayInfo::STATUS_CONNECTED:
            return IMediaRouterRouteInfo::STATUS_CONNECTED;
        default:
            return IMediaRouterRouteInfo::STATUS_NONE;
    }
}

Int32 CMediaRouterService::UserHandler::RouteRecord::ComputePlaybackType(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    return IMediaRouterRouteInfo::PLAYBACK_TYPE_REMOTE;
}

Int32 CMediaRouterService::UserHandler::RouteRecord::ComputePlaybackStream(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    return IAudioSystem::STREAM_MUSIC;
}

Int32 CMediaRouterService::UserHandler::RouteRecord::ComputeVolume(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    Int32 volume;
    descriptor->GetVolume(&volume);
    Int32 volumeMax;
    descriptor->GetVolumeMax(&volumeMax);
    if (volume < 0) {
        return 0;
    }
    else if (volume > volumeMax) {
        return volumeMax;
    }
    return volume;
}

Int32 CMediaRouterService::UserHandler::RouteRecord::ComputeVolumeMax(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    Int32 volumeMax;
    descriptor->GetVolumeMax(&volumeMax);
    return volumeMax > 0 ? volumeMax : 0;
}

Int32 CMediaRouterService::UserHandler::RouteRecord::ComputeVolumeHandling(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    Int32 volumeHandling;
    descriptor->GetVolumeHandling(&volumeHandling);
    switch (volumeHandling) {
        case IRemoteDisplayStateRemoteDisplayInfo::PLAYBACK_VOLUME_VARIABLE:
            return IMediaRouterRouteInfo::PLAYBACK_VOLUME_VARIABLE;
        case IRemoteDisplayStateRemoteDisplayInfo::PLAYBACK_VOLUME_FIXED:
        default:
            return IMediaRouterRouteInfo::PLAYBACK_VOLUME_FIXED;
    }
}

Int32 CMediaRouterService::UserHandler::RouteRecord::ComputePresentationDisplayId(
    /* [in] */ IRemoteDisplayStateRemoteDisplayInfo* descriptor)
{
    // The MediaRouter class validates that the id corresponds to an extant
    // presentation display.  So all we do here is canonicalize the null case.
    Int32 displayId;
    descriptor->GetPresentationDisplayId(&displayId);
    return displayId < 0 ? -1 : displayId;
}


//==============================================================================
//                  CMediaRouterService::SystemRunningBroadReceiver
//==============================================================================

ECode CMediaRouterService::SystemRunningBroadReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    if (intent->GetAction(&action), action.Equals(IIntent::ACTION_USER_SWITCHED)) {
        mHost->SwitchUser();
    }
    return NOERROR;
}

ECode CMediaRouterService::SystemRunningBroadReceiver::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info)
    *info = String("CMediaRouterService::SystemRunningBroadReceiver:");
    (*info).AppendFormat("%p", this);
    return NOERROR;
}


//==============================================================================
//                  CMediaRouterService
//==============================================================================

const Int64 CMediaRouterService::CONNECTING_TIMEOUT;
const Int64 CMediaRouterService::CONNECTED_TIMEOUT;
const String CMediaRouterService::TAG("MediaRouterService");
const Boolean CMediaRouterService::DEBUG;

CAR_INTERFACE_IMPL_3(CMediaRouterService, Object, IIMediaRouterService, IWatchdogMonitor, IBinder)

CAR_OBJECT_IMPL(CMediaRouterService)

CMediaRouterService::CMediaRouterService()
    : mCurrentUserId(-1)
{
    CSparseArray::New((ISparseArray**)&mUserRecords);
}

ECode CMediaRouterService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    Watchdog::GetInstance()->AddMonitor(this);
    return NOERROR;
}

void CMediaRouterService::SystemRunning()
{
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_USER_SWITCHED, (IIntentFilter**)&filter);
    AutoPtr<IBroadcastReceiver> receiver = (IBroadcastReceiver*)new SystemRunningBroadReceiver(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(receiver, filter, (IIntent**)&intent);
    SwitchUser();
}

ECode CMediaRouterService::Monitor()
{
    {    AutoLock syncLock(mLock); /* check for deadlock */ }
    return NOERROR;
}

ECode CMediaRouterService::RegisterClientAsUser(
    /* [in] */ IIMediaRouterClient* client,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId)
{
    if (client == NULL) {
        Slogger::E(TAG, "client must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int32 uid = Binder::GetCallingUid();
    if (!ValidatePackageName(uid, packageName)) {
        Slogger::E(TAG, "packageName must match the calling uid");
        return E_SECURITY_EXCEPTION;
    }

    Int32 pid = Binder::GetCallingPid();
    AutoPtr<IActivityManagerHelper> helper;
    CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
    Int32 resolvedUserId;
    helper->HandleIncomingUser(pid, uid, userId,
            FALSE /*allowAll*/, TRUE /*requireFull*/, String("registerClientAsUser"), packageName, &resolvedUserId);

    Int32 perm;
    mContext->CheckCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::CONFIGURE_WIFI_DISPLAY, &perm);
    Boolean trusted = perm == IPackageManager::PERMISSION_GRANTED;
    Int64 token = Binder::ClearCallingIdentity();
    // try {
    {    AutoLock syncLock(mLock);
        RegisterClientLocked(client, pid, packageName, resolvedUserId, trusted);
    }
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CMediaRouterService::UnregisterClient(
    /* [in] */ IIMediaRouterClient* client)
{
    if (client == NULL) {
        Slogger::E(TAG, "client must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 token = Binder::ClearCallingIdentity();
    // try {
    {    AutoLock syncLock(mLock);
        UnregisterClientLocked(client, FALSE);
    }
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CMediaRouterService::GetState(
    /* [in] */ IIMediaRouterClient* client,
    /* [out] */ IMediaRouterClientState** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (client == NULL) {
        Slogger::E(TAG, "client must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 token = Binder::ClearCallingIdentity();
    // try {
    {    AutoLock syncLock(mLock);
        AutoPtr<IMediaRouterClientState> temp = GetStateLocked(client);
        *result = temp;
        REFCOUNT_ADD(*result)
    }
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CMediaRouterService::SetDiscoveryRequest(
    /* [in] */ IIMediaRouterClient* client,
    /* [in] */ Int32 routeTypes,
    /* [in] */ Boolean activeScan)
{
    if (client == NULL) {
        Slogger::E(TAG, "client must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 token = Binder::ClearCallingIdentity();
    // try {
    {    AutoLock syncLock(mLock);
        SetDiscoveryRequestLocked(client, routeTypes, activeScan);
    }
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CMediaRouterService::SetSelectedRoute(
    /* [in] */ IIMediaRouterClient* client,
    /* [in] */ const String& routeId,
    /* [in] */ Boolean isExplicit)
{
    if (client == NULL) {
        Slogger::E(TAG, "client must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 token = Binder::ClearCallingIdentity();
    // try {
    {    AutoLock syncLock(mLock);
        SetSelectedRouteLocked(client, routeId, isExplicit);
    }
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CMediaRouterService::RequestSetVolume(
    /* [in] */ IIMediaRouterClient* client,
    /* [in] */ const String& routeId,
    /* [in] */ Int32 volume)
{
    if (client == NULL) {
        Slogger::E(TAG, "client must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (routeId.IsNull()) {
        Slogger::E(TAG, "routeId must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 token = Binder::ClearCallingIdentity();
    // try {
    {    AutoLock syncLock(mLock);
        RequestSetVolumeLocked(client, routeId, volume);
    }
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

ECode CMediaRouterService::RequestUpdateVolume(
    /* [in] */ IIMediaRouterClient* client,
    /* [in] */ const String& routeId,
    /* [in] */ Int32 direction)
{
    if (client == NULL) {
        Slogger::E(TAG, "client must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (routeId.IsNull()) {
        Slogger::E(TAG, "routeId must not be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 token = Binder::ClearCallingIdentity();
    // try {
    {    AutoLock syncLock(mLock);
        RequestUpdateVolumeLocked(client, routeId, direction);
    }
    // } finally {
    //     Binder.restoreCallingIdentity(token);
    // }
    Binder::RestoreCallingIdentity(token);
    return NOERROR;
}

void CMediaRouterService::SwitchUser()
{
    {    AutoLock syncLock(mLock);
        AutoPtr<IActivityManagerHelper> helper;
        CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
        Int32 userId;
        helper->GetCurrentUser(&userId);
        if (mCurrentUserId != userId) {
            Int32 oldUserId = mCurrentUserId;
            mCurrentUserId = userId; // do this first

            AutoPtr<IInterface> value;
            mUserRecords->Get(oldUserId, (IInterface**)&value);
            AutoPtr<UserRecord> oldUser = (UserRecord*)(IObject*)value.Get();
            if (oldUser != NULL) {
                Boolean result;
                oldUser->mHandler->SendEmptyMessage(UserHandler::MSG_STOP, &result);
                DisposeUserIfNeededLocked(oldUser); // since no longer current user
            }

            value = NULL;
            mUserRecords->Get(userId, (IInterface**)&value);
            AutoPtr<UserRecord> newUser = (UserRecord*)(IObject*)value.Get();
            if (newUser != NULL) {
                Boolean result;
                newUser->mHandler->SendEmptyMessage(UserHandler::MSG_START, &result);
            }
        }
    }
}

void CMediaRouterService::ClientDied(
    /* [in] */ ClientRecord* clientRecord)
{
    {    AutoLock syncLock(mLock);
        UnregisterClientLocked(clientRecord->mClient, TRUE);
    }
}

ECode CMediaRouterService::RegisterClientLocked(
    /* [in] */ IIMediaRouterClient* client,
    /* [in] */ Int32 pid,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 userId,
    /* [in] */ Boolean trusted)
{
    AutoPtr<IBinder> binder = IBinder::Probe(client);
    HashMap<AutoPtr<IBinder>, AutoPtr<ClientRecord> >::Iterator it = mAllClientRecords.Find(binder);
    AutoPtr<ClientRecord> clientRecord;
    if (it != mAllClientRecords.End()) {
        clientRecord = it->mSecond;
    }
    if (clientRecord == NULL) {
        Boolean newUser = FALSE;
        AutoPtr<IInterface> value;
        mUserRecords->Get(userId, (IInterface**)&value);
        AutoPtr<UserRecord> userRecord = (UserRecord*)(IObject*)value.Get();
        if (userRecord == NULL) {
            userRecord = new UserRecord(userId, this);
            newUser = TRUE;
        }
        clientRecord = new ClientRecord(userRecord, client, pid, packageName, trusted, this);
        // try {
        AutoPtr<IProxy> proxy = (IProxy*)binder->Probe(EIID_IProxy);
        if (proxy != NULL) {
            ECode ec = proxy->LinkToDeath((IProxyDeathRecipient*)clientRecord.Get(), 0);
            if (FAILED(ec)) {
                Slogger::E(TAG, "Media router client died prematurely.");
                return ec;
            }
        }
        // } catch (RemoteException ex) {
        //     throw new RuntimeException("Media router client died prematurely.", ex);
        // }

        if (newUser) {
            mUserRecords->Put(userId, (IObject*)userRecord.Get());
            InitializeUserLocked(userRecord);
        }

        userRecord->mClientRecords.PushBack(clientRecord);
        mAllClientRecords[binder] = clientRecord;
        InitializeClientLocked(clientRecord);
    }
    return NOERROR;
}

void CMediaRouterService::UnregisterClientLocked(
    /* [in] */ IIMediaRouterClient* client,
    /* [in] */ Boolean died)
{
    AutoPtr<IBinder> binder = IBinder::Probe(client);
    HashMap<AutoPtr<IBinder>, AutoPtr<ClientRecord> >::Iterator it = mAllClientRecords.Find(binder);
    AutoPtr<ClientRecord> clientRecord;
    if (it != mAllClientRecords.End()) {
        clientRecord = it->mSecond;
        mAllClientRecords.Erase(it);
    }
    if (clientRecord != NULL) {
        AutoPtr<UserRecord> userRecord = clientRecord->mUserRecord;
        userRecord->mClientRecords.Remove(clientRecord);
        DisposeClientLocked(clientRecord, died);
        DisposeUserIfNeededLocked(userRecord); // since client removed from user
    }
}

AutoPtr<IMediaRouterClientState> CMediaRouterService::GetStateLocked(
    /* [in] */ IIMediaRouterClient* client)
{
    AutoPtr<IBinder> binder = IBinder::Probe(client);
    HashMap<AutoPtr<IBinder>, AutoPtr<ClientRecord> >::Iterator it = mAllClientRecords.Find(binder);
    AutoPtr<ClientRecord> clientRecord;
    if (it != mAllClientRecords.End()) {
        clientRecord = it->mSecond;
    }
    if (clientRecord != NULL) {
        return clientRecord->GetState();
    }
    return NULL;
}

void CMediaRouterService::SetDiscoveryRequestLocked(
    /* [in] */ IIMediaRouterClient* client,
    /* [in] */ Int32 routeTypes,
    /* [in] */ Boolean activeScan)
{
    AutoPtr<IBinder> binder = IBinder::Probe(client);
    HashMap<AutoPtr<IBinder>, AutoPtr<ClientRecord> >::Iterator it = mAllClientRecords.Find(binder);
    AutoPtr<ClientRecord> clientRecord;
    if (it != mAllClientRecords.End()) {
        clientRecord = it->mSecond;
    }
    if (clientRecord != NULL) {
        // Only let the system discover remote display routes for now.
        if (!clientRecord->mTrusted) {
            routeTypes &= ~IMediaRouter::ROUTE_TYPE_REMOTE_DISPLAY;
        }

        if (clientRecord->mRouteTypes != routeTypes
                || clientRecord->mActiveScan != activeScan) {
            if (DEBUG) {
                Slogger::D(TAG, "%p: Set discovery request, routeTypes=0x%d, activeScan=%d",
                        clientRecord.Get(), routeTypes, activeScan);
            }
            clientRecord->mRouteTypes = routeTypes;
            clientRecord->mActiveScan = activeScan;
            Boolean result;
            clientRecord->mUserRecord->mHandler->SendEmptyMessage(
                    UserHandler::MSG_UPDATE_DISCOVERY_REQUEST, &result);
        }
    }
}

void CMediaRouterService::SetSelectedRouteLocked(
    /* [in] */ IIMediaRouterClient* client,
    /* [in] */ const String& routeId,
    /* [in] */ Boolean isExplicit)
{
    AutoPtr<IBinder> binder = IBinder::Probe(client);
    HashMap<AutoPtr<IBinder>, AutoPtr<ClientRecord> >::Iterator it = mAllClientRecords.Find(binder);
    AutoPtr<ClientRecord> clientRecord;
    if (it != mAllClientRecords.End()) {
        clientRecord = it->mSecond;
    }
    if (clientRecord != NULL) {
        String oldRouteId = clientRecord->mSelectedRouteId;
        if (!routeId.Equals(oldRouteId)) {
            if (DEBUG) {
                Slogger::D(TAG, "%p: Set selected route, routeId=%s, oldRouteId=%s, explicit=%d",
                    clientRecord.Get(), routeId.string(), oldRouteId.string(), isExplicit);
            }

            clientRecord->mSelectedRouteId = routeId;
            if (isExplicit) {
                // Any app can disconnect from the globally selected route.
                if (!oldRouteId.IsNull()) {
                    AutoPtr<ICharSequence> cs;
                    CString::New(oldRouteId, (ICharSequence**)&cs);
                    AutoPtr<IMessage> msg;
                    clientRecord->mUserRecord->mHandler->ObtainMessage(
                            UserHandler::MSG_UNSELECT_ROUTE, cs, (IMessage**)&msg);
                    msg->SendToTarget();
                }
                // Only let the system connect to new global routes for now.
                // A similar check exists in the display manager for wifi display.
                if (!routeId.IsNull() && clientRecord->mTrusted) {
                    AutoPtr<ICharSequence> cs;
                    CString::New(routeId, (ICharSequence**)&cs);
                    AutoPtr<IMessage> msg;
                    clientRecord->mUserRecord->mHandler->ObtainMessage(
                            UserHandler::MSG_SELECT_ROUTE, cs, (IMessage**)&msg);
                    msg->SendToTarget();
                }
            }
        }
    }
}

void CMediaRouterService::RequestSetVolumeLocked(
    /* [in] */ IIMediaRouterClient* client,
    /* [in] */ const String& routeId,
    /* [in] */ Int32 volume)
{
    AutoPtr<IBinder> binder = IBinder::Probe(client);
    HashMap<AutoPtr<IBinder>, AutoPtr<ClientRecord> >::Iterator it = mAllClientRecords.Find(binder);
    AutoPtr<ClientRecord> clientRecord;
    if (it != mAllClientRecords.End()) {
        clientRecord = it->mSecond;
    }
    if (clientRecord != NULL) {
        AutoPtr<ICharSequence> cs;
        CString::New(routeId, (ICharSequence**)&cs);
        AutoPtr<IMessage> msg;
        clientRecord->mUserRecord->mHandler->ObtainMessage(
                UserHandler::MSG_REQUEST_SET_VOLUME, volume, 0, cs, (IMessage**)&msg);
        msg->SendToTarget();
    }
}

void CMediaRouterService::RequestUpdateVolumeLocked(
    /* [in] */ IIMediaRouterClient* client,
    /* [in] */ const String& routeId,
    /* [in] */ Int32 direction)
{
    AutoPtr<IBinder> binder = IBinder::Probe(client);
    HashMap<AutoPtr<IBinder>, AutoPtr<ClientRecord> >::Iterator it = mAllClientRecords.Find(binder);
    AutoPtr<ClientRecord> clientRecord;
    if (it != mAllClientRecords.End()) {
        clientRecord = it->mSecond;
    }
    if (clientRecord != NULL) {
        AutoPtr<ICharSequence> cs;
        CString::New(routeId, (ICharSequence**)&cs);
        AutoPtr<IMessage> msg;
        clientRecord->mUserRecord->mHandler->ObtainMessage(
                UserHandler::MSG_REQUEST_UPDATE_VOLUME, direction, 0, cs, (IMessage**)&msg);
        msg->SendToTarget();
    }
}

void CMediaRouterService::InitializeUserLocked(
    /* [in] */ UserRecord* userRecord)
{
    if (DEBUG) {
        Slogger::D(TAG, "%p: Initialized", userRecord);
    }
    if (userRecord->mUserId == mCurrentUserId) {
        Boolean result;
        userRecord->mHandler->SendEmptyMessage(UserHandler::MSG_START, &result);
    }
}

void CMediaRouterService::DisposeUserIfNeededLocked(
    /* [in] */ UserRecord* userRecord)
{
    // If there are no records left and the user is no longer current then go ahead
    // and purge the user record and all of its associated state.  If the user is current
    // then leave it alone since we might be connected to a route or want to query
    // the same route information again soon.
    if (userRecord->mUserId != mCurrentUserId
            && userRecord->mClientRecords.IsEmpty()) {
        if (DEBUG) {
            Slogger::D(TAG, "%p: Disposed", userRecord);
        }
        mUserRecords->Remove(userRecord->mUserId);
        // Note: User already stopped (by switchUser) so no need to send stop message here.
    }
}

void CMediaRouterService::InitializeClientLocked(
    /* [in] */ ClientRecord* clientRecord)
{
    if (DEBUG) {
        Slogger::D(TAG, "%p: Registered", clientRecord);
    }
}

void CMediaRouterService::DisposeClientLocked(
    /* [in] */ ClientRecord* clientRecord,
    /* [in] */ Boolean died)
{
    if (DEBUG) {
        if (died) {
            Slogger::D(TAG, "%p: Died!", clientRecord);
        }
        else {
            Slogger::D(TAG, "%p: Unregistered", clientRecord);
        }
    }
    if (clientRecord->mRouteTypes != 0 || clientRecord->mActiveScan) {
        Boolean result;
        clientRecord->mUserRecord->mHandler->SendEmptyMessage(
                UserHandler::MSG_UPDATE_DISCOVERY_REQUEST, &result);
    }
    clientRecord->Dispose();
}

Boolean CMediaRouterService::ValidatePackageName(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    if (!packageName.IsNull()) {
        AutoPtr<IPackageManager> pm;
        mContext->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<ArrayOf<String> > packageNames;
        pm->GetPackagesForUid(uid, (ArrayOf<String>**)&packageNames);
        if (packageNames != NULL) {
            for (Int32 i = 0; i < packageNames->GetLength(); ++i) {
                String n = (*packageNames)[i];
                if (n.Equals(packageName)) {
                    return TRUE;
                }
            }
        }
    }
    return FALSE;
}

ECode CMediaRouterService::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    return Object::ToString(str);
}

} // namespace Media
} // namespace Server
} // namespace Droid
} // namespace Elastos
