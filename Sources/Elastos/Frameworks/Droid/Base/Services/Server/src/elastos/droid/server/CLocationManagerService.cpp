
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/server/CLocationManagerService.h"
#include "elastos/droid/server/location/GpsLocationProvider.h"
#include "elastos/droid/server/location/PassiveProvider.h"
#include "elastos/droid/server/location/FusedProxy.h"
#include "elastos/droid/server/location/FlpHardwareProvider.h"
#include "elastos/droid/server/location/GeofenceProxy.h"
#include "elastos/droid/server/location/ActivityRecognitionProxy.h"
#include "elastos/droid/server/location/MockProvider.h"
//#include "elastos/droid/server/location/GeoFencerProxy.h"
//#include "elastos/droid/server/location/LocationProviderInterface.h"
//#include "elastos/droid/internal/location/CProviderRequest.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Looper.h"
//#include "elastos/droid/os/CWorkSource.h"
//#include "elastos/droid/provider/CSettingsSecure.h"
//#include "ServiceWatcher.h"
#include "elastos/droid/hardware/location/ActivityRecognitionHardware.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::EIID_IAppOpsManagerOnOpChangedInternalListener;
using Elastos::Droid::App::EIID_IPendingIntentOnFinished;
using Elastos::Droid::App::IAppOpsManagerOnOpChangedListener;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IUserInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Hardware::Location::IActivityRecognitionHardware;
using Elastos::Droid::Hardware::Location::ActivityRecognitionHardware;
//using Elastos::Droid::Internal::Location::CProviderRequest;
using Elastos::Droid::Internal::Os::IBackgroundThreadHelper;
using Elastos::Droid::Internal::Os::CBackgroundThreadHelper;
using Elastos::Droid::Internal::Os::IBackgroundThread;
using Elastos::Droid::Internal::Location::CProviderProperties;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Location::CLocationRequest;
using Elastos::Droid::Location::ILocationProviderHelper;
using Elastos::Droid::Location::CLocationProviderHelper;
using Elastos::Droid::Location::CLocation;
using Elastos::Droid::Location::EIID_IILocationManager;
using Elastos::Droid::Location::IGeoFenceParams;
using Elastos::Droid::Location::CGeoFenceParams;
using Elastos::Droid::Location::ILocationProvider;
//using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Server::Location::LocationProviderProxy;
using Elastos::Droid::Server::Location::GpsLocationProvider;
using Elastos::Droid::Server::Location::FusedProxy;
using Elastos::Droid::Server::Location::IFlpHardwareProvider;
using Elastos::Droid::Server::Location::FlpHardwareProvider;
using Elastos::Droid::Server::Location::GeofenceProxy;
using Elastos::Droid::Server::Location::ActivityRecognitionProxy;
using Elastos::Droid::Server::Location::MockProvider;
//using Elastos::Droid::Server::Location::GeoFencerProxy;
using Elastos::Droid::R;
using Elastos::Core::Math;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringUtils;
using Elastos::Core::CString;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Core::CoreUtils;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CLocationManagerService::TAG("LocationManagerService");
const Boolean CLocationManagerService::D = Logger::IsLoggable(TAG, Logger::___DEBUG);

const String CLocationManagerService::WAKELOCK_KEY = TAG;

const Int32 CLocationManagerService::RESOLUTION_LEVEL_NONE = 0;
const Int32 CLocationManagerService::RESOLUTION_LEVEL_COARSE = 1;
const Int32 CLocationManagerService::RESOLUTION_LEVEL_FINE = 2;

const String CLocationManagerService::ACCESS_MOCK_LOCATION =
        Manifest::permission::ACCESS_MOCK_LOCATION;
const String CLocationManagerService::ACCESS_LOCATION_EXTRA_COMMANDS =
        Manifest::permission::ACCESS_LOCATION_EXTRA_COMMANDS;
const String CLocationManagerService::INSTALL_LOCATION_PROVIDER =
        Manifest::permission::INSTALL_LOCATION_PROVIDER;

const String CLocationManagerService::NETWORK_LOCATION_SERVICE_ACTION("elastos.location.service.v2.NetworkLocationProvider");
const String CLocationManagerService::FUSED_LOCATION_SERVICE_ACTION("elastos.location.service.FusedLocationProvider");

const Int32 CLocationManagerService::MSG_LOCATION_CHANGED = 1;

const Int64 CLocationManagerService::NANOS_PER_MILLI = 1000000L;

const Int64 CLocationManagerService::HIGH_POWER_INTERVAL_MS = 5 * 60 * 1000;

const Int32 CLocationManagerService::MAX_PROVIDER_SCHEDULING_JITTER_MS = 100;

static AutoPtr<ILocationRequest> InitDefaultLocationRequest() {
    AutoPtr<ILocationRequest> locationRequest;
    CLocationRequest::New((ILocationRequest**)&locationRequest);
    return locationRequest;
}

const AutoPtr<ILocationRequest> CLocationManagerService::DEFAULT_LOCATION_REQUEST =
        InitDefaultLocationRequest();

//=================================================================
// CLocationManagerService::AppOpsManagerOnOpChangedInternalListener
//=================================================================

CAR_INTERFACE_IMPL(CLocationManagerService::AppOpsManagerOnOpChangedInternalListener, Object,
                    IAppOpsManagerOnOpChangedInternalListener)

CLocationManagerService::AppOpsManagerOnOpChangedInternalListener::AppOpsManagerOnOpChangedInternalListener(
    /* [in] */ CLocationManagerService* host)
    : mHost(host)
{}

ECode CLocationManagerService::AppOpsManagerOnOpChangedInternalListener::OnOpChanged(
    /* [in] */ Int32 op,
    /* [in] */ const String& packageName)
{
    {    AutoLock syncLock(mHost->mLock);
        AutoPtr<ICollection> cl;
        mHost->mReceivers->GetValues((ICollection**)&cl);
        AutoPtr<IIterator> it;
        cl->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<Receiver> receiver = (Receiver*)IObject::Probe(p);
            receiver->UpdateMonitoring(TRUE);
        }
        mHost->ApplyAllProviderRequirementsLocked();
    }
    return NOERROR;
}

ECode CLocationManagerService::AppOpsManagerOnOpChangedInternalListener::OnOpChanged(
    /* [in] */ const String& op,
    /* [in] */ const String& packageName)
{
    return NOERROR;
}

//=================================================================
// CLocationManagerService::MyContentObserver
//=================================================================

CLocationManagerService::MyContentObserver::MyContentObserver(
    /* [in] */ CLocationManagerService* host)
    : mHost(host)
{}

ECode CLocationManagerService::MyContentObserver::constructor(
    /* [in] */ IHandler* handler)
{
    ContentObserver::constructor(handler);
    return NOERROR;
}

ECode CLocationManagerService::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    {    AutoLock syncLock(mLock);
        mHost->UpdateProvidersLocked();
    }
    return NOERROR;
}

//=================================================================
// CLocationManagerService::MyBroadcastReceiver
//=================================================================

CLocationManagerService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CLocationManagerService* host)
    : mHost(host)
{}

ECode CLocationManagerService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        Int32 i = 0;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &i);
        mHost->SwitchUser(i);
    }
    else if (IIntent::ACTION_MANAGED_PROFILE_ADDED.Equals(action)
            || IIntent::ACTION_MANAGED_PROFILE_REMOVED.Equals(action)) {
        mHost->UpdateUserProfiles(mHost->mCurrentUserId);
    }
    return NOERROR;
}

//=================================================================
// CLocationManagerService::LocationWorkerHandler
//=================================================================
CLocationManagerService::LocationWorkerHandler::LocationWorkerHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ CLocationManagerService* host)
    : Handler(looper, NULL, TRUE)
    , mHost(host)
{}

ECode CLocationManagerService::LocationWorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case CLocationManagerService::MSG_LOCATION_CHANGED: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            ILocation* location = ILocation::Probe(obj);
            Int32 arg1;
            msg->GetArg1(&arg1);
            mHost->HandleLocationChanged(location, arg1 == 1);
            break;
        }
    }
    return NOERROR;
}

//=================================================================
// CLocationManagerService::LocationPackageMonitor
//=================================================================

CLocationManagerService::LocationPackageMonitor::LocationPackageMonitor(
    /* [in] */ CLocationManagerService* host) : mHost(host)
{}

ECode CLocationManagerService::LocationPackageMonitor::OnPackageDisappeared(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 reason)
{
    // remove all receivers associated with this package name
    {    AutoLock syncLock(mLock);
        AutoPtr<IArrayList> deadReceivers;

        AutoPtr<ICollection> cl;
        mHost->mReceivers->GetValues((ICollection**)&cl);
        AutoPtr<IIterator> it;
        cl->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<Receiver> receiver = (Receiver*)IObject::Probe(p);
            if (receiver->mPackageName.Equals(packageName)) {
                if (deadReceivers == NULL) {
                    CArrayList::New((IArrayList**)&deadReceivers);
                }
                deadReceivers->Add(p);
            }
        }

        // perform removal outside of mReceivers loop
        if (deadReceivers != NULL) {
            AutoPtr<IIterator> itR;
            deadReceivers->GetIterator((IIterator**)&itR);
            bHasNxt = FALSE;
            while ((itR->HasNext(&bHasNxt), bHasNxt)) {
                AutoPtr<IInterface> p;
                itR->GetNext((IInterface**)&p);
                AutoPtr<Receiver> receiver = (Receiver*)IObject::Probe(p);
                mHost->RemoveUpdatesLocked(receiver);
            }
        }
    }
    return NOERROR;
}

//=================================================================
// CLocationManagerService::UpdateRecord
//=================================================================

CLocationManagerService::UpdateRecord::UpdateRecord(
    /* [in] */ const String& provider,
    /* [in] */ ILocationRequest* request,
    /* [in] */ Receiver* receiver,
    /* [in] */ CLocationManagerService* host)
    : mProvider(provider)
    , mRequest(request)
    , mReceiver(receiver)
    , mLastStatusBroadcast(0)
    , mHost(host)
{
    AutoPtr<IInterface> _records;
    mHost->mRecordsByProvider->Get(CoreUtils::Convert(provider), (IInterface**)&_records);
    AutoPtr<IArrayList> records = IArrayList::Probe(_records);
    if (records == NULL) {
        CArrayList::New((IArrayList**)&records);
        mHost->mRecordsByProvider->Put(CoreUtils::Convert(provider), records);
    }
    Boolean bContain = FALSE;
    records->Contains((IInterface*)(IObject*)this, &bContain);
    if (!bContain) {
        records->Add((IInterface*)(IObject*)this);
    }

    // Update statistics for historical location requests by package/provider
    Int64 interval = 0;
    request->GetInterval(&interval);
    mHost->mRequestStatistics->StartRequesting(
            mReceiver->mPackageName, provider, interval);
}

void CLocationManagerService::UpdateRecord::DisposeLocked(
    /* [in] */ Boolean removeReceiver)
{
    mHost->mRequestStatistics->StopRequesting(mReceiver->mPackageName, mProvider);

    // remove from mRecordsByProvider
    AutoPtr<IInterface> _globalRecords;
    mHost->mRecordsByProvider->Get(CoreUtils::Convert(mProvider), (IInterface**)&_globalRecords);
    AutoPtr<IArrayList> globalRecords = IArrayList::Probe(_globalRecords);
    if (globalRecords != NULL) {
        AutoPtr<UpdateRecord> tmp(this);
        globalRecords->Remove((IInterface*)(IObject*)tmp);
    }

    if (!removeReceiver) return;  // the caller will handle the rest

    // remove from Receiver#mUpdateRecords
    AutoPtr<IHashMap> receiverRecords = mReceiver->mUpdateRecords;
    if (receiverRecords != NULL) {
        receiverRecords->Remove(CoreUtils::Convert(mProvider));

        // and also remove the Receiver if it has no more update records
        Int32 size = 0;
        receiverRecords->GetSize(&size);
        if (removeReceiver && size == 0) {
            mHost->RemoveUpdatesLocked(mReceiver);
        }
    }
}

ECode CLocationManagerService::UpdateRecord::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder s;
    s.Append(String("UpdateRecord["));
    s.Append(mProvider);
    s.Append(' ');
    s.Append(mReceiver->mPackageName);
    s.Append('(');
    s.Append(mReceiver->mUid);
    s.Append(')');
    s.Append(' ');
    s.Append((IInterface*)(mRequest.Get()));
    s.Append(']');
    return s.ToString(result);
}

//=================================================================
// CLocationManagerService::Receiver
//=================================================================
CAR_INTERFACE_IMPL_2(CLocationManagerService::Receiver, Object, IProxyDeathRecipient, IPendingIntentOnFinished)

CLocationManagerService::Receiver::Receiver(
    /* [in] */ IILocationListener* listener,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ Boolean hideFromAppOps,
    /* [in] */ CLocationManagerService* host)
    : mHost(host)
    , mUid(uid)
    , mPid(pid)
    , mPackageName(packageName)
    , mListener(listener)
    , mPendingIntent(intent)
    , mPendingBroadcasts(0)
{
    if (listener != NULL) {
        mKey = listener;
    }
    else {
        mKey = intent;
    }
    mAllowedResolutionLevel = mHost->GetAllowedResolutionLevel(pid, uid);

    AutoPtr<IWorkSource> _workSource = workSource;
    Int32 size = 0;
    if (_workSource != NULL && (_workSource->GetSize(&size), size) <= 0) {
        _workSource = NULL;
    }
    mWorkSource = _workSource;
    mHideFromAppOps = hideFromAppOps;

    UpdateMonitoring(TRUE);

    // construct/configure wakelock
    mHost->mPowerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, WAKELOCK_KEY, (IPowerManagerWakeLock**)&mWakeLock);
    if (_workSource == NULL) {
        CWorkSource::New(mUid, mPackageName, (IWorkSource**)&_workSource);
    }
    mWakeLock->SetWorkSource(_workSource);
}

ECode CLocationManagerService::Receiver::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder s;
    s.Append("Reciever[");
    // s.Append(Integer.toHexString(System.identityHashCode(this)));
    if (mListener != NULL) {
        s.Append(" listener");
    }
    else {
        s.Append(" intent");
    }
    AutoPtr<ISet> st;
    mUpdateRecords->GetKeySet((ISet**)&st);
    AutoPtr<IIterator> it;
    st->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> _p;
        it->GetNext((IInterface**)&_p);
        // AutoPtr<ICharSequence> pStr = ICharSequence::Probe(_p);
        // String p;
        // pStr->ToString(&p);
        AutoPtr<IInterface> val;
        mUpdateRecords->Get(_p, (IInterface**)&val);
        AutoPtr<ICharSequence> pStr = ICharSequence::Probe(val);
        String str;
        pStr->ToString(&str);
        s.Append(" ");
        s.Append(str);
    }
    s.Append("]");
    return s.ToString(result);
}

void CLocationManagerService::Receiver::UpdateMonitoring(
    /* [in] */ Boolean allow)
{
    if (mHideFromAppOps) {
        return;
    }

    Boolean requestingLocation = FALSE;
    Boolean requestingHighPowerLocation = FALSE;
    if (allow) {
        // See if receiver has any enabled update records.  Also note if any update records
        // are high power (has a high power provider with an interval under a threshold).
        AutoPtr<ICollection> cl;
        mUpdateRecords->GetValues((ICollection**)&cl);
        AutoPtr<IIterator> it;
        cl->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<UpdateRecord> updateRecord = (UpdateRecord*)IObject::Probe(p);
            if (mHost->IsAllowedByCurrentUserSettingsLocked(updateRecord->mProvider)) {
                requestingLocation = TRUE;
                assert(0 && "TODO");
                // LocationProviderInterface locationProvider
                //         = mProvidersByName.get(updateRecord->mProvider);
                // AutoPtr<IProviderProperties> properties = locationProvider != NULL
                //         ? locationProvider->GetProperties() : NULL;
                // if (properties != NULL
                //         && properties->mPowerRequirement == ICriteria::POWER_HIGH
                //         && updateRecord->mRequest->GetInterval() < HIGH_POWER_INTERVAL_MS) {
                //     requestingHighPowerLocation = TRUE;
                //     break;
                // }
            }
        }
    }

    // First update monitoring of any location request (including high power).
    mOpMonitoring = UpdateMonitoring(
            requestingLocation,
            mOpMonitoring,
            IAppOpsManager::OP_MONITOR_LOCATION);

    // Now update monitoring of high power requests only.
    Boolean wasHighPowerMonitoring = mOpHighPowerMonitoring;
    mOpHighPowerMonitoring = UpdateMonitoring(
            requestingHighPowerLocation,
            mOpHighPowerMonitoring,
            IAppOpsManager::OP_MONITOR_HIGH_POWER_LOCATION);
    if (mOpHighPowerMonitoring != wasHighPowerMonitoring) {
        // Send an intent to notify that a high power request has been added/removed.
        AutoPtr<IIntent> intent;
        CIntent::New(ILocationManager::HIGH_POWER_REQUEST_CHANGE_ACTION, (IIntent**)&intent);
        AutoPtr<IUserHandleHelper> hlp;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&hlp);
        AutoPtr<IUserHandle> h;
        hlp->GetALL((IUserHandle**)&h);
        mHost->mContext->SendBroadcastAsUser(intent, h);
    }
}

Boolean CLocationManagerService::Receiver::UpdateMonitoring(
    /* [in] */ Boolean allowMonitoring,
    /* [in] */ Boolean currentlyMonitoring,
    /* [in] */ Int32 op)
{
    if (!currentlyMonitoring) {
        if (allowMonitoring) {
            Int32 mode = 0;
            mHost->mAppOps->StartOpNoThrow(op, mUid, mPackageName, &mode);
            return mode == IAppOpsManager::MODE_ALLOWED;
        }
    }
    else {
        Int32 mode = 0;
        mHost->mAppOps->CheckOpNoThrow(op, mUid, mPackageName, &mode);
        if (allowMonitoring && mode == IAppOpsManager::MODE_ASK) {
            return TRUE;
        }
        if (!allowMonitoring || mode != IAppOpsManager::MODE_ALLOWED) {
            mHost->mAppOps->FinishOp(op, mUid, mPackageName);
            return FALSE;
        }
    }

    return currentlyMonitoring;
}

Boolean CLocationManagerService::Receiver::IsListener()
{
    return mListener != NULL;
}

Boolean CLocationManagerService::Receiver::IsPendingIntent()
{
    return mPendingIntent != NULL;
}

ECode CLocationManagerService::Receiver::GetListener(
    /* [out] */ IILocationListener** l)
{
    VALIDATE_NOT_NULL(l);
    *l = NULL;

    if (mListener != NULL) {
        *l = mListener;
        REFCOUNT_ADD(*l);
        return NOERROR;
    }
    return E_ILLEGAL_STATE_EXCEPTION;
}


Boolean CLocationManagerService::Receiver::CallStatusChangedLocked(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    if (mListener != NULL) {
        // try {
        AutoLock lock(mLock);

        // synchronize to ensure incrementPendingBroadcastsLocked()
        // is called before decrementPendingBroadcasts()
        if (FAILED(mListener->OnStatusChanged(provider, status, extras))) {
            return FALSE;
        }
        // call this after broadcasting so we do not increment
        // if we throw an exeption.
        IncrementPendingBroadcastsLocked();
        // } catch (RemoteException e) {
        //     return false;
        // }
    }
    else {
        AutoPtr<IIntent> statusChanged;
        CIntent::New((IIntent**)&statusChanged);
        AutoPtr<IBundle> bundle;
        CBundle::New(extras, (IBundle**)&bundle);
        statusChanged->PutExtras(bundle);
        statusChanged->PutExtra(ILocationManager::KEY_STATUS_CHANGED, status);
//        try {
        AutoLock lock(mLock);
        // synchronize to ensure incrementPendingBroadcastsLocked()
        // is called before decrementPendingBroadcasts()
        if (FAILED(mPendingIntent->Send(mHost->mContext, 0, statusChanged,
                this, mHost->mLocationHandler, mHost->GetResolutionPermission(mAllowedResolutionLevel)))) {
            return FALSE;
        }
        // call this after broadcasting so we do not increment
        // if we throw an exeption.
        IncrementPendingBroadcastsLocked();
//        } catch (PendingIntent.CanceledException e) {
//            return false;
//        }
    }
    return TRUE;
}

Boolean CLocationManagerService::Receiver::CallLocationChangedLocked(
    /* [in] */ ILocation* location)
{
    AutoPtr<ILocation> l;
    CLocation::New(location, (ILocation**)&l);
    if (mListener != NULL) {
        // try {
        AutoLock lock(mLock);
        // synchronize to ensure incrementPendingBroadcastsLocked()
        // is called before decrementPendingBroadcasts()
        if (FAILED(mListener->OnLocationChanged(l))) {
            return FALSE;
        }
        // call this after broadcasting so we do not increment
        // if we throw an exeption.
        IncrementPendingBroadcastsLocked();
        // } catch (RemoteException e) {
        //     return false;
        // }
    }
    else {
        AutoPtr<IIntent> locationChanged;
        CIntent::New((IIntent**)&locationChanged);
        locationChanged->PutExtra(ILocationManager::KEY_LOCATION_CHANGED,
                IParcelable::Probe(l));
        // try {
        AutoLock lock(mLock);
        // synchronize to ensure incrementPendingBroadcastsLocked()
        // is called before decrementPendingBroadcasts()
        if (FAILED(mPendingIntent->Send(mHost->mContext, 0, locationChanged,
                this, mHost->mLocationHandler, mHost->GetResolutionPermission(mAllowedResolutionLevel)))) {
            return FALSE;
        }
        // call this after broadcasting so we do not increment
        // if we throw an exeption.
        IncrementPendingBroadcastsLocked();
        // } catch (PendingIntent.CanceledException e) {
        //     return false;
        // }
    }
    return TRUE;
}

Boolean CLocationManagerService::Receiver::CallProviderEnabledLocked(
    /* [in] */ const String& provider,
    /* [in] */ Boolean enabled)
{
    // First update AppOp monitoring.
    // An app may get/lose location access as providers are enabled/disabled.
    UpdateMonitoring(TRUE);

    if (mListener != NULL) {
        // try {
        AutoLock lock(mLock);
        // synchronize to ensure incrementPendingBroadcastsLocked()
        // is called before decrementPendingBroadcasts()
        ECode ec = NOERROR;
        if (enabled) {
            ec = mListener->OnProviderEnabled(provider);
        }
        else {
            ec = mListener->OnProviderDisabled(provider);
        }
        if (FAILED(ec)) {
            return FALSE;
        }

        // call this after broadcasting so we do not increment
        // if we throw an exeption.
        IncrementPendingBroadcastsLocked();
        // } catch (RemoteException e) {
        //     return false;
        // }
    }
    else {
        AutoPtr<IIntent> providerIntent;
        CIntent::New((IIntent**)&providerIntent);
        providerIntent->PutBooleanExtra(ILocationManager::KEY_PROVIDER_ENABLED,
                enabled);
        // try {
        AutoLock lock(mLock);
        // synchronize to ensure incrementPendingBroadcastsLocked()
        // is called before decrementPendingBroadcasts()
        if (FAILED(mPendingIntent->Send(mHost->mContext, 0, providerIntent,
                this, mHost->mLocationHandler, mHost->GetResolutionPermission(mAllowedResolutionLevel)))) {
            return FALSE;
        }
        // call this after broadcasting so we do not increment
        // if we throw an exeption.
        IncrementPendingBroadcastsLocked();
        // } catch (PendingIntent.CanceledException e) {
        //     return false;
        // }
    }
    return TRUE;
}

ECode CLocationManagerService::Receiver::ProxyDied()
{
    {
        AutoLock lock(mHost->mLock);
        mHost->RemoveUpdatesLocked(this);
    }

    AutoLock lock(mLock);
    ClearPendingBroadcastsLocked();
    return NOERROR;
}

ECode CLocationManagerService::Receiver::OnSendFinished(
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras)
{
    AutoLock lock(mLock);
    DecrementPendingBroadcastsLocked();
    return NOERROR;
}

void CLocationManagerService::Receiver::IncrementPendingBroadcastsLocked()
{
    if (mPendingBroadcasts++ == 0) {
        mWakeLock->AcquireLock();
    }
}

void CLocationManagerService::Receiver::DecrementPendingBroadcastsLocked()
{
    if (--mPendingBroadcasts == 0) {
        Boolean b = FALSE;
        mWakeLock->IsHeld(&b);
        if (b) {
            mWakeLock->Release();
        }
    }
}

void CLocationManagerService::Receiver::ClearPendingBroadcastsLocked()
{
    if (mPendingBroadcasts > 0) {
        mPendingBroadcasts = 0;
        Boolean b = FALSE;
        mWakeLock->IsHeld(&b);
        if (b) {
            mWakeLock->Release();
        }
    }
}

//=================================================================
// CLocationManagerService::
//=================================================================
CAR_OBJECT_IMPL(CLocationManagerService)

CAR_INTERFACE_IMPL(CLocationManagerService, Object, IILocationManager)

CLocationManagerService::CLocationManagerService()
    : mLocationFudger(NULL)
    , mGeofenceManager(NULL)
    , mGeocodeProvider(NULL)
    , mBlacklist(NULL)
    , mCurrentUserId(IUserHandle::USER_OWNER)
{
    // mPackageMonitor = new LocationPackageMonitor(this);
    CHashSet::New((ISet**)&mEnabledProviders);
    CHashSet::New((ISet**)&mDisabledProviders);
    CHashMap::New((IHashMap**)&mMockProviders);
    CHashMap::New((IHashMap**)&mReceivers);
    CArrayList::New((IArrayList**)&mProviders);
    CHashMap::New((IHashMap**)&mRealProviders);
    CHashMap::New((IHashMap**)&mProvidersByName);
    CHashMap::New((IHashMap**)&mRecordsByProvider);

    mRequestStatistics = new LocationRequestStatistics();

    CHashMap::New((IHashMap**)&mLastLocation);
    CHashMap::New((IHashMap**)&mLastLocationCoarseInterval);
    CArrayList::New((IArrayList**)&mProxyProviders);

    mCurrentUserId = IUserHandle::USER_OWNER;
    mCurrentUserProfiles = ArrayOf<Int32>::Alloc(1);
    (*mCurrentUserProfiles)[0] = IUserHandle::USER_OWNER;
}

ECode CLocationManagerService::constructor(
    /* [in] */ IContext* ctx)
{
    mContext = ctx;
    AutoPtr<IInterface> p;
    mContext->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&p);
    mAppOps = IAppOpsManager::Probe(p);
    mGeoFencerEnabled = FALSE;
    if (D) Logger::D(TAG, "Constructed");

    // most startup is deferred until systemReady()
    return NOERROR;
}

CLocationManagerService::~CLocationManagerService()
{
    mRecordsByProvider->Clear();
}

ECode CLocationManagerService::SystemRunning()
{
    {
        AutoLock lock(mLock);
        if (D) Logger::D(TAG, "systemReady()");

        // fetch package manager
        mContext->GetPackageManager((IPackageManager**)&mPackageManager);

        // fetch power manager
        AutoPtr<IInterface> pPM;
        mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&pPM);
        mPowerManager = IPowerManager::Probe(pPM);

        // prepare worker thread
        AutoPtr<IBackgroundThreadHelper> hlp;
        CBackgroundThreadHelper::AcquireSingleton((IBackgroundThreadHelper**)&hlp);
        AutoPtr<IBackgroundThread> bt;
        hlp->GetInstance((IBackgroundThread**)&bt);
        AutoPtr<ILooper> lp;
        IHandlerThread::Probe(bt)->GetLooper((ILooper**)&lp);
        mLocationHandler = new LocationWorkerHandler(lp, this);

        // prepare mLocationHandler's dependents
        mLocationFudger = new LocationFudger(mContext, mLocationHandler);
        mBlacklist = new LocationBlacklist(mContext, mLocationHandler);
        mBlacklist->Init();
        mGeofenceManager = new GeofenceManager(mContext, mBlacklist);

        // Monitor for app ops mode changes.
        AutoPtr<AppOpsManagerOnOpChangedInternalListener> callback
                = new AppOpsManagerOnOpChangedInternalListener(this);
        mAppOps->StartWatchingMode(IAppOpsManager::OP_COARSE_LOCATION, String(NULL), IAppOpsManagerOnOpChangedListener::Probe(callback));

        AutoPtr<IInterface> ss;
        mContext->GetSystemService(IContext::USER_SERVICE, (IInterface**)&ss);
        mUserManager = IUserManager::Probe(ss);
        UpdateUserProfiles(mCurrentUserId);

        // prepare providers
        LoadProvidersLocked();
        UpdateProvidersLocked();
    }

    // listen for settings changes
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> sSecure;
    //CSettingsSecure::AcquireSingleton((ISettingsSecure**)&sSecure);
    AutoPtr<IUri> uri;
    sSecure->GetUriFor(ISettingsSecure::LOCATION_PROVIDERS_ALLOWED, (IUri**)&uri);
    AutoPtr<MyContentObserver> co = new MyContentObserver(this);
    co->constructor(mLocationHandler);
    cr->RegisterContentObserver(
            uri, TRUE,
            co, IUserHandle::USER_ALL);
    AutoPtr<ILooper> lp;
    mLocationHandler->GetLooper((ILooper**)&lp);
    // mPackageMonitor->Register(mContext, lp, TRUE);

    // listen for user change
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_USER_SWITCHED);
    intentFilter->AddAction(IIntent::ACTION_MANAGED_PROFILE_ADDED);
    intentFilter->AddAction(IIntent::ACTION_MANAGED_PROFILE_REMOVED);

    AutoPtr<IUserHandleHelper> hlp;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&hlp);
    AutoPtr<IUserHandle> h;
    hlp->GetALL((IUserHandle**)&h);
    AutoPtr<IIntent> res;
    mContext->RegisterReceiverAsUser(new MyBroadcastReceiver(this),
        h, intentFilter, String(NULL), mLocationHandler, (IIntent**)&res);
    return NOERROR;
}

void CLocationManagerService::UpdateUserProfiles(
    /* [in] */ Int32 currentUserId)
{
    AutoPtr<IList> profiles;
    mUserManager->GetProfiles(currentUserId, (IList**)&profiles);
    {    AutoLock syncLock(mLock);
        Int32 size = 0;
        profiles->GetSize(&size);
        mCurrentUserProfiles = ArrayOf<Int32>::Alloc(size);
        for (Int32 i = 0; i < mCurrentUserProfiles->GetLength(); i++) {
            AutoPtr<IInterface> p;
            profiles->Get(i, (IInterface**)&p);
            AutoPtr<IUserInfo> _p = IUserInfo::Probe(p);
            _p->GetId(&((*mCurrentUserProfiles)[i]));
        }
    }
}

Boolean CLocationManagerService::IsCurrentProfile(
    /* [in] */ Int32 userId)
{
    {    AutoLock syncLock(mLock);
        for (Int32 i = 0; i < mCurrentUserProfiles->GetLength(); i++) {
            if ((*mCurrentUserProfiles)[i] == userId) {
                return TRUE;
            }
        }
        return FALSE;
    }
    return FALSE;
}

ECode CLocationManagerService::EnsureFallbackFusedProviderPresentLocked(
    /* [in] */ IArrayList* pkgs)
{
    AutoPtr<IPackageManager> pm;
    ASSERT_SUCCEEDED(mContext->GetPackageManager((IPackageManager**)&pm));
    String systemPackageName;
    mContext->GetPackageName(&systemPackageName);
    assert(0 && "TODO");
    AutoPtr<IArrayList> sigSets;// = ServiceWatcher::GetSignatureSets(mContext, pkgs);

    AutoPtr<IList> rInfos;
    AutoPtr<IIntent> intent;
    CIntent::New(String(FUSED_LOCATION_SERVICE_ACTION), (IIntent**)&intent);
    pm->QueryIntentServicesAsUser(intent, IPackageManager::GET_META_DATA,
            mCurrentUserId, (IList**)&rInfos);
    AutoPtr<IIterator> it;
    rInfos->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    while ((it->HasNext(&hasNext), hasNext)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IResolveInfo> rInfo = IResolveInfo::Probe(p);

        AutoPtr<IServiceInfo> serviceInfo;
        rInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        String packageName;
        IPackageItemInfo::Probe(serviceInfo)->GetPackageName(&packageName);

        // Check that the signature is in the list of supported sigs. If it's not in
        // this list the standard provider binding logic won't bind to it.
        // try {
        AutoPtr<IPackageInfo> pInfo;
        if (FAILED(pm->GetPackageInfo(packageName, IPackageManager::GET_SIGNATURES,
                (IPackageInfo**)&pInfo))) {
            Logger::E(TAG, "missing package: %s", packageName.string());
            continue;
        }

        // TODO: Ignore signature check
        // AutoPtr<ArrayOf<ISignature*> > signatures;
        // pInfo->GetSignatures((ArrayOf<ISignature*>**)&signatures);
        // if (!ServiceWatcher::IsSignatureMatch(signatures, sigSets)) {
        //     // Log.w(TAG, packageName + " resolves service " + FUSED_LOCATION_SERVICE_ACTION +
        //     //         ", but has wrong signature, ignoring");
        //     continue;
        // }

        // Get the version info
        AutoPtr<IBundle> metaData;
        IPackageItemInfo::Probe(serviceInfo)->GetMetaData((IBundle**)&metaData);
        if (metaData == NULL) {
            Logger::W(TAG, "Found fused provider without metadata: %s", packageName.string());
            continue;
        }

        Int32 version = 0;
        metaData->GetInt32(
                ServiceWatcher::EXTRA_SERVICE_VERSION, -1, &version);
        if (version == 0) {
            // This should be the fallback fused location provider.

            // Make sure it's in the system partition.
            AutoPtr<IApplicationInfo> appInfo;
            IComponentInfo::Probe(serviceInfo)->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 flags;
            appInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_SYSTEM) == 0) {
                if (D) Logger::D(TAG, "Fallback candidate not in /system: %s", packageName.string());
                continue;
            }

            // Check that the fallback is signed the same as the OS
            // as a proxy for coreApp="true"
            // TODO: Ignore signature check
            Int32 result;
            pm->CheckSignatures(systemPackageName, packageName, &result);
            if (result != IPackageManager::SIGNATURE_MATCH) {
                if (D) {
                    Logger::D(TAG, "Fallback candidate not signed the same as system: %s",
                        packageName.string());
                }
                continue;
            }

            // Found a valid fallback.
            if (D) Logger::D(TAG, "Found fallback provider: %s", packageName.string());
            return NOERROR;
        }
        else {
            if (D) Logger::D(TAG, "Fallback candidate not version 0: %s", packageName.string());
        }
    }

    return E_ILLEGAL_STATE_EXCEPTION;
    // throw new IllegalStateException("Unable to find a fused location provider that is in the "
    //         + "system partition with version 0 and signed with the platform certificate. "
    //         + "Such a package is needed to provide a default fused location provider in the "
    //         + "event that no other fused location provider has been installed or is currently "
    //         + "available. For example, coreOnly boot mode when decrypting the data "
    //         + "partition. The fallback must also be marked coreApp=\"true\" in the manifest");
}

void CLocationManagerService::LoadProvidersLocked()
{
    // create a passive location provider, which is always enabled
    AutoPtr<PassiveProvider> passiveProvider = new PassiveProvider(this);
    AddProviderLocked(passiveProvider);
    String name;
    passiveProvider->GetName(&name);
    mEnabledProviders->Add(CoreUtils::Convert(name));
    mPassiveProvider = passiveProvider;
    // Create a gps location provider
    AutoPtr<ILooper> lp;
    mLocationHandler->GetLooper((ILooper**)&lp);
    AutoPtr<GpsLocationProvider> gpsProvider = new GpsLocationProvider(mContext, this,
            lp);

    if (GpsLocationProvider::IsSupported()) {
        gpsProvider->GetGpsStatusProvider((IIGpsStatusProvider**)&mGpsStatusProvider);
        gpsProvider->GetNetInitiatedListener((IINetInitiatedListener**)&mNetInitiatedListener);
        AddProviderLocked(gpsProvider);
        mRealProviders->Put(CoreUtils::Convert(ILocationManager::GPS_PROVIDER), ILocationProviderInterface::Probe(gpsProvider));
    }
    gpsProvider->GetGpsMeasurementsProvider((GpsMeasurementsProvider**)&mGpsMeasurementsProvider);
    gpsProvider->GetGpsNavigationMessageProvider((GpsNavigationMessageProvider**)&mGpsNavigationMessageProvider);

    /*
    Load package name(s) containing location provider support.
    These packages can contain services implementing location providers:
    Geocoder Provider, Network Location Provider, and
    Fused Location Provider. They will each be searched for
    service components implementing these providers.
    The location framework also has support for installation
    of new location providers at run-time. The new package does not
    have to be explicitly listed here, however it must have a signature
    that matches the signature of at least one package on this list.
    */
    AutoPtr<IResources> resources;
    mContext->GetResources((IResources**)&resources);
    AutoPtr<IArrayList> providerPackageNames;
    CArrayList::New((IArrayList**)&providerPackageNames);
    AutoPtr<ArrayOf<String> > pkgs;
    resources->GetStringArray(
            R::array::config_locationProviderPackageNames, (ArrayOf<String>**)&pkgs);
    if (D) {
        Logger::D(TAG, "certificates for location providers pulled from: %s",
            Arrays::ToString(pkgs).string());
    }
    if (pkgs != NULL) {
        AutoPtr<IList> res;
        Arrays::AsList(pkgs, (IList**)&res);
        providerPackageNames->AddAll(ICollection::Probe(res));
    }

    EnsureFallbackFusedProviderPresentLocked(providerPackageNames);

    // bind to network provider
    AutoPtr<LocationProviderProxy> networkProvider = LocationProviderProxy::CreateAndBind(
            mContext,
            ILocationManager::NETWORK_PROVIDER,
            String(NETWORK_LOCATION_SERVICE_ACTION),
            R::bool_::config_enableNetworkLocationOverlay,
            R::string::config_networkLocationProviderPackageName,
            R::array::config_locationProviderPackageNames,
            mLocationHandler);
    if (networkProvider != NULL) {
        ILocationProviderInterface* lpi = (ILocationProviderInterface*)networkProvider.Get();
        mRealProviders->Put(CoreUtils::Convert(ILocationManager::NETWORK_PROVIDER), lpi);
        mProxyProviders->Add(lpi);
        AddProviderLocked(lpi);
    }
    else {
        Slogger::W(TAG, "no network location provider found");
    }

    // bind to fused provider
    AutoPtr<LocationProviderProxy> fusedLocationProvider
            = LocationProviderProxy::CreateAndBind(
                    mContext,
                    ILocationManager::FUSED_PROVIDER,
                    String(FUSED_LOCATION_SERVICE_ACTION),
                    R::bool_::config_enableFusedLocationOverlay,
                    R::string::config_fusedLocationProviderPackageName,
                    R::array::config_locationProviderPackageNames,
                    mLocationHandler);
    if (fusedLocationProvider != NULL) {
        ILocationProviderInterface* lpi = (ILocationProviderInterface*)fusedLocationProvider.Get();
        AddProviderLocked(fusedLocationProvider);
        mProxyProviders->Add(lpi);
        String providerName;
        fusedLocationProvider->GetName(&providerName);
        mEnabledProviders->Add(CoreUtils::Convert(providerName));
        mRealProviders->Put(CoreUtils::Convert(ILocationManager::FUSED_PROVIDER), lpi);
    }
    else {
        Slogger::E(TAG, "no fused location provider found Location service needs a fused location provider");
    }

    // bind to geocoder provider
    mGeocodeProvider = GeocoderProxy::CreateAndBind(mContext,
            R::bool_::config_enableGeocoderOverlay,
            R::string::config_geocoderProviderPackageName,
            R::array::config_locationProviderPackageNames,
            mLocationHandler);
    if (mGeocodeProvider == NULL) {
        Slogger::E(TAG,  "no geocoder provider found");
    }

    resources->GetString(
            R::string::config_geofenceServicesProvider, &mGeoFencerPackageName);
    if (!mGeoFencerPackageName.IsNull()) {
        AutoPtr<IIntent> intent;
        CIntent::New(mGeoFencerPackageName, (IIntent**)&intent);
        AutoPtr<IResolveInfo> ri;
        mPackageManager->ResolveService(intent, 0, (IResolveInfo**)&ri);
        if (ri != NULL) {
            //mGeoFencer = GeoFencerProxy::GetGeoFencerProxy(mContext, mGeoFencerPackageName);
            mGeoFencerEnabled = TRUE;
        }
    }
    else {
        mGeoFencer = NULL;
        mGeoFencerEnabled = FALSE;
    }

    resources->GetString(
        R::string::config_comboNetworkLocationProvider, &mComboNlpPackageName);
    if (!mComboNlpPackageName.IsNull()) {
        mComboNlpReadyMarker = mComboNlpPackageName + ".nlp:ready";
        mComboNlpScreenMarker = mComboNlpPackageName + ".nlp:screen";
    }

    // bind to fused hardware provider if supported
    // in devices without support, requesting an instance of FlpHardwareProvider will raise an
    // exception, so make sure we only do that when supported
    AutoPtr<IFlpHardwareProvider> flpHardwareProvider;
    if (FlpHardwareProvider::IsSupported()) {
        flpHardwareProvider = FlpHardwareProvider::GetInstance(mContext);
        AutoPtr<IIFusedLocationHardware> hd;
        flpHardwareProvider->GetLocationHardware((IIFusedLocationHardware**)&hd);
        AutoPtr<FusedProxy> fusedProxy = FusedProxy::CreateAndBind(
                mContext,
                mLocationHandler,
                hd,
                R::bool_::config_enableHardwareFlpOverlay,
                R::string::config_hardwareFlpPackageName,
                R::array::config_locationProviderPackageNames);
        if (fusedProxy == NULL) {
            Slogger::E(TAG, "Unable to bind FusedProxy.");
        }
    }
    else {
        flpHardwareProvider = NULL;
        Slogger::E(TAG, "FLP HAL not supported");
    }

    // bind to geofence provider
    AutoPtr<IIFusedGeofenceHardware> hd;
    if (flpHardwareProvider != NULL) {
        flpHardwareProvider->GetGeofenceHardware((IIFusedGeofenceHardware**)&hd);
    }
    AutoPtr<IIGpsGeofenceHardware> gf;
    gpsProvider->GetGpsGeofenceProxy((IIGpsGeofenceHardware**)&gf);
    AutoPtr<GeofenceProxy> provider = GeofenceProxy::CreateAndBind(
            mContext,
            R::bool_::config_enableGeofenceOverlay,
            R::string::config_geofenceProviderPackageName,
            R::array::config_locationProviderPackageNames,
            mLocationHandler,
            gf,
            hd);
    if (provider == NULL) {
        Slogger::E(TAG,  "Unable to bind FLP Geofence proxy.");
    }

    // bind to the hardware activity recognition if supported
    Boolean bSup = FALSE;
    if ((ActivityRecognitionHardware::IsSupported(&bSup), bSup)) {
        AutoPtr<IActivityRecognitionHardware> arh;
        ActivityRecognitionHardware::GetInstance(mContext, (IActivityRecognitionHardware**)&arh);
        AutoPtr<ActivityRecognitionProxy> proxy = ActivityRecognitionProxy::CreateAndBind(
                mContext,
                mLocationHandler,
                arh,
                R::bool_::config_enableActivityRecognitionHardwareOverlay,
                R::string::config_activityRecognitionHardwarePackageName,
                R::array::config_locationProviderPackageNames);

        if (proxy == NULL) {
            Slogger::E(TAG, "Unable to bind ActivityRecognitionProxy.");
        }
    }
    else {
        Slogger::E(TAG, "Hardware Activity-Recognition not supported.");
    }

    AutoPtr<ArrayOf<String> > testProviderStrings;
    resources->GetStringArray(
            R::array::config_testLocationProviders, (ArrayOf<String>**)&testProviderStrings);
    for (Int32 i = 0;i < testProviderStrings->GetLength();i++) {
        String testProviderString = (*testProviderStrings)[i];
        AutoPtr<ArrayOf<String> > fragments;
        StringUtils::Split(testProviderString, String(","), (ArrayOf<String>**)&fragments);
        String name = (*fragments)[0].Trim();
        AutoPtr<IInterface> pro;
        mProvidersByName->Get(CoreUtils::Convert(name), (IInterface**)&pro);
        if (pro != NULL) {
            // throw new IllegalArgumentException("Provider \"" + name + "\" already exists");
            return;
        }
        AutoPtr<IProviderProperties> properties;
        CProviderProperties::New(
                StringUtils::ParseBoolean((*fragments)[1]) /* requiresNetwork */,
                StringUtils::ParseBoolean((*fragments)[2]) /* requiresSatellite */,
                StringUtils::ParseBoolean((*fragments)[3]) /* requiresCell */,
                StringUtils::ParseBoolean((*fragments)[4]) /* hasMonetaryCost */,
                StringUtils::ParseBoolean((*fragments)[5]) /* supportsAltitude */,
                StringUtils::ParseBoolean((*fragments)[6]) /* supportsSpeed */,
                StringUtils::ParseBoolean((*fragments)[7]) /* supportsBearing */,
                StringUtils::ParseInt32((*fragments)[8]) /* powerRequirement */,
                StringUtils::ParseInt32((*fragments)[9]) /* accuracy */,
                (IProviderProperties**)&properties);
        AddTestProviderLocked(name, properties);
    }
}

void CLocationManagerService::SwitchUser(
    /* [in] */ Int32 userId)
{
    if (mCurrentUserId == userId) {
        return;
    }
    mBlacklist->SwitchUser(userId);
    mLocationHandler->RemoveMessages(MSG_LOCATION_CHANGED);
    AutoLock lock(mLock);
    mLastLocation->Clear();
    mLastLocationCoarseInterval->Clear();
    AutoPtr<IIterator> it;
    mProviders->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> _p;
        it->GetNext((IInterface**)&_p);
        AutoPtr<ILocationProviderInterface> p = ILocationProviderInterface::Probe(_p);
        String name;
        p->GetName(&name);
        UpdateProviderListenersLocked(name, FALSE);
    }
    mCurrentUserId = userId;
    UpdateUserProfiles(userId);
    UpdateProvidersLocked();
}

ECode CLocationManagerService::LocationCallbackFinished(
    /* [in] */ IILocationListener* listener)
{
    //Do not use getReceiverLocked here as that will add the ILocationListener to
    //the receiver list if it is not found.  If it is not found then the
    //LocationListener was removed when it had a pending broadcast and should
    //not be added back.
    AutoLock lock(mLock);
    AutoPtr<IBinder> binder;// = listener->AsBinder();
    AutoPtr<IInterface> p;
    mReceivers->Get(binder, (IInterface**)&p);
    AutoPtr<Receiver> receiver = (Receiver*)IProxyDeathRecipient::Probe(p);
    if (receiver != NULL) {
        AutoLock lock(receiver->mLock);
            // so wakelock calls will succeed
        Int64 identity = Binder::ClearCallingIdentity();
        receiver->DecrementPendingBroadcastsLocked();
        Binder::RestoreCallingIdentity(identity);
    }
    return NOERROR;
}

void CLocationManagerService::AddProviderLocked(
    /* [in] */ ILocationProviderInterface* provider)
{
    mProviders->Add(provider);
    String name;
    provider->GetName(&name);
    mProvidersByName->Put(CoreUtils::Convert(name), provider);
}

void CLocationManagerService::RemoveProviderLocked(
    /* [in] */ ILocationProviderInterface* provider)
{
    provider->Disable();
    mProviders->Remove(provider);
    String name;
    provider->GetName(&name);
    mProvidersByName->Remove(CoreUtils::Convert(name));
}

Boolean CLocationManagerService::IsAllowedByCurrentUserSettingsLocked(
    /* [in] */ const String& provider)
{
    Boolean bContain = FALSE;
    if ((mEnabledProviders->Contains(CoreUtils::Convert(provider), &bContain), bContain)) {
        return TRUE;
    }
    if ((mDisabledProviders->Contains(CoreUtils::Convert(provider), &bContain), bContain)) {
        return FALSE;
    }
    // Use system settings
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    Boolean rst;
    AutoPtr<ISettingsSecure> settingsSecure;
    //CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->IsLocationProviderEnabled(resolver, provider, &rst);
    return rst;
}

Boolean CLocationManagerService::IsAllowedByUserSettingsLocked(
    /* [in] */ String provider,
    /* [in] */ Int32 uid)
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userHandleId = 0;
    helper->GetUserId(uid, &userHandleId);
    if (!IsCurrentProfile(userHandleId) && !IsUidALocationProvider(uid)) {
        return FALSE;
    }
    return IsAllowedByCurrentUserSettingsLocked(provider);
}

String CLocationManagerService::GetResolutionPermission(
    /* [in] */ Int32 resolutionLevel)
{
    switch (resolutionLevel) {
        case RESOLUTION_LEVEL_FINE:
            return Manifest::permission::ACCESS_FINE_LOCATION;
        case RESOLUTION_LEVEL_COARSE:
            return Manifest::permission::ACCESS_COARSE_LOCATION;
        default:
            return String(NULL);
    }
}

Int32 CLocationManagerService::GetAllowedResolutionLevel(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid)
{
    Int32 result;
    FAIL_RETURN(mContext->CheckPermission(Manifest::permission::ACCESS_FINE_LOCATION,
            pid, uid, &result));
    if (result == IPackageManager::PERMISSION_GRANTED) {
        return RESOLUTION_LEVEL_FINE;
    }
    else {
        FAIL_RETURN(mContext->CheckPermission(Manifest::permission::ACCESS_COARSE_LOCATION,
                pid, uid, &result));
        if (result == IPackageManager::PERMISSION_GRANTED) {
            return RESOLUTION_LEVEL_COARSE;
        }
        else {
            return RESOLUTION_LEVEL_NONE;
        }
    }
}

Int32 CLocationManagerService::GetCallerAllowedResolutionLevel()
{
    return GetAllowedResolutionLevel(Binder::GetCallingPid(), Binder::GetCallingUid());
}

ECode CLocationManagerService::CheckResolutionLevelIsSufficientForGeofenceUse(
    /* [in] */ Int32 allowedResolutionLevel)
{
    if (allowedResolutionLevel < RESOLUTION_LEVEL_FINE) {
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("Geofence usage requires ACCESS_FINE_LOCATION permission");
    }
    return NOERROR;
}

Int32 CLocationManagerService::GetMinimumResolutionLevelForProviderUse(
    /* [in] */ const String& provider)
{
    if (ILocationManager::GPS_PROVIDER.Equals(provider) ||
            ILocationManager::PASSIVE_PROVIDER.Equals(provider)) {
        // gps and passive providers require FINE permission
        return RESOLUTION_LEVEL_FINE;
    }
    else if (ILocationManager::NETWORK_PROVIDER.Equals(provider) ||
            ILocationManager::FUSED_PROVIDER.Equals(provider)) {
        // network and fused providers are ok with COARSE or FINE
        return RESOLUTION_LEVEL_COARSE;
    }
    else {
        // mock providers
        AutoPtr<IInterface> p;
        mMockProviders->Get(CoreUtils::Convert(provider), (IInterface**)&p);
        AutoPtr<ILocationProviderInterface> lp = ILocationProviderInterface::Probe(p);
        if (lp != NULL) {
            AutoPtr<IProviderProperties> properties;
            lp->GetProperties((IProviderProperties**)&properties);
            if (properties != NULL) {
                Boolean requestSatellite;
                properties->GetRequiresSatellite(&requestSatellite);
                if (requestSatellite) {
                    // provider requiring satellites require FINE permission
                    return RESOLUTION_LEVEL_FINE;
                }
                else {
                    Boolean requiresNetwork, requiresCell;
                    properties->GetRequiresNetwork(&requiresNetwork);
                    properties->GetRequiresCell(&requiresCell);
                    if (requiresNetwork || requiresCell) {
                        // provider requiring network and or cell require COARSE or FINE
                        return RESOLUTION_LEVEL_COARSE;
                    }
                }
            }
        }
    }
    return RESOLUTION_LEVEL_FINE; // if in doubt, require FINE
}

ECode CLocationManagerService::CheckResolutionLevelIsSufficientForProviderUse(
    /* [in] */ Int32 allowedResolutionLevel,
    /* [in] */ const String& providerName)
{
    Int32 requiredResolutionLevel = GetMinimumResolutionLevelForProviderUse(providerName);
    if (allowedResolutionLevel < requiredResolutionLevel) {
        switch (requiredResolutionLevel) {
            case RESOLUTION_LEVEL_FINE:
                return E_SECURITY_EXCEPTION;
                // throw new SecurityException("\"" + providerName + "\" location provider " +
                //         "requires ACCESS_FINE_LOCATION permission.");
            case RESOLUTION_LEVEL_COARSE:
                return E_SECURITY_EXCEPTION;
                // throw new SecurityException("\"" + providerName + "\" location provider " +
                //         "requires ACCESS_COARSE_LOCATION or ACCESS_FINE_LOCATION permission.");
            default:
                return E_SECURITY_EXCEPTION;
                // throw new SecurityException("Insufficient permission for \"" + providerName +
                //         "\" location provider.");
        }
    }
    return NOERROR;
}

void CLocationManagerService::CheckDeviceStatsAllowed()
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::UPDATE_DEVICE_STATS, String(NULL));
}

void CLocationManagerService::CheckUpdateAppOpsAllowed()
{
    mContext->EnforceCallingOrSelfPermission(
            Manifest::permission::UPDATE_APP_OPS_STATS, String(NULL));
}

ECode CLocationManagerService::ResolutionLevelToOp(
    /* [in] */ Int32 allowedResolutionLevel,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    if (allowedResolutionLevel != RESOLUTION_LEVEL_NONE) {
        if (allowedResolutionLevel == RESOLUTION_LEVEL_COARSE) {
            *result = IAppOpsManager::OP_COARSE_LOCATION;
            return NOERROR;
        }
        else {
            *result = IAppOpsManager::OP_FINE_LOCATION;
            return NOERROR;
        }
    }
    *result = -1;
    return NOERROR;
}

Boolean CLocationManagerService::ReportLocationAccessNoThrow(
    /* [in] */ Int32 uid,
    /* [in] */ String packageName,
    /* [in] */ Int32 allowedResolutionLevel)
{
    Int32 op = 0;
    ResolutionLevelToOp(allowedResolutionLevel, &op);
    if (op >= 0) {
        Int32 mode = 0;
        mAppOps->NoteOpNoThrow(op, uid, packageName, &mode);
        if (mode != IAppOpsManager::MODE_ALLOWED) {
            return FALSE;
        }
    }
    return TRUE;
}

Boolean CLocationManagerService::CheckLocationAccess(
    /* [in] */ Int32 uid,
    /* [in] */ String packageName,
    /* [in] */ Int32 allowedResolutionLevel)
{
    Int32 op = 0;
    ResolutionLevelToOp(allowedResolutionLevel, &op);
    if (op >= 0) {
        Int32 mode = 0;
        mAppOps->CheckOp(op, uid, packageName, &mode);
        if (mode != IAppOpsManager::MODE_ALLOWED && mode != IAppOpsManager::MODE_ASK) {
            return FALSE;
        }
    }
    return TRUE;
}

ECode CLocationManagerService::GetAllProviders(
    /* [out] */ IList** allProviders)
{
    VALIDATE_NOT_NULL(allProviders);

    AutoPtr<IArrayList> out;
    {
        AutoLock lock(mLock);
        Int32 size = 0;
        mProviders->GetSize(&size);
        CArrayList::New(size, (IArrayList**)&out);
        AutoPtr<IIterator> it;
        mProviders->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<ILocationProviderInterface> provider = ILocationProviderInterface::Probe(p);
            String name;
            provider->GetName(&name);
            if (ILocationManager::FUSED_PROVIDER.Equals(name)) {
                continue;
            }
            AutoPtr<ICharSequence> wrapper;
            CString::New(name, (ICharSequence**)&wrapper);
            out->Add(wrapper.Get());
        }
    }

    if (D) Logger::D(TAG, "getAllProviders()=%p", out.Get());
    *allProviders = IList::Probe(out);
    REFCOUNT_ADD(*allProviders)
    return NOERROR;
}

ECode CLocationManagerService::GetProviders(
    /* [in] */ ICriteria* criteria,
    /* [in] */ Boolean enabledOnly,
    /* [out] */ IList** providers)
{
    VALIDATE_NOT_NULL(providers);

    Int32 allowedResolutionLevel = GetCallerAllowedResolutionLevel();
    AutoPtr<IArrayList> out;
    Int32 uid = Binder::GetCallingUid();
    Int64 identity = Binder::ClearCallingIdentity();

    AutoLock lock(mLock);

    Int32 size = 0;
    mProviders->GetSize(&size);
    CArrayList::New(size, (IArrayList**)&out);
    AutoPtr<IIterator> it;
    mProviders->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<ILocationProviderInterface> provider = ILocationProviderInterface::Probe(p);
        String name;
        provider->GetName(&name);
        if (ILocationManager::FUSED_PROVIDER.Equals(name)) {
            continue;
        }
        if (allowedResolutionLevel >= GetMinimumResolutionLevelForProviderUse(name)) {
            if (enabledOnly && !IsAllowedByUserSettingsLocked(name, uid)) {
                continue;
            }

            if (criteria != NULL) {
                AutoPtr<ILocationProviderHelper> lpHelper;
                CLocationProviderHelper::AcquireSingleton((ILocationProviderHelper**)&lpHelper);
                AutoPtr<IProviderProperties> properties;
                provider->GetProperties((IProviderProperties**)&properties);
                Boolean result = FALSE;
                lpHelper->PropertiesMeetCriteria(name, properties, criteria, &result);
                if (!result) {
                    continue;
                }
            }
            AutoPtr<ICharSequence> wrapper;
            CString::New(name, (ICharSequence**)&wrapper);
            out->Add(wrapper);
        }
    }
    Binder::RestoreCallingIdentity(identity);

    if (D) Logger::D(TAG, "getProviders()=%p", out.Get());
    *providers = IList::Probe(out);
    REFCOUNT_ADD(*providers)
    return NOERROR;
}

ECode CLocationManagerService::GetBestProvider(
    /* [in] */ ICriteria* criteria,
    /* [in] */ Boolean enabledOnly,
    /* [out] */ String* provider)
{
    VALIDATE_NOT_NULL(provider);
    String result(NULL);

    AutoPtr<IList> providers;
    GetProviders(criteria, enabledOnly, (IList**)&providers);
    Boolean bEmp = FALSE;
    if (!(providers->IsEmpty(&bEmp), bEmp)) {
        result = PickBest(providers);
        if (D) Logger::D(TAG, "getBestProvider(%p, %b)=%s", criteria, enabledOnly, result.string());
        *provider = result;
        return NOERROR;
    }

    providers = NULL;
    GetProviders(NULL, enabledOnly, (IList**)&providers);
    if (!(providers->IsEmpty(&bEmp), bEmp)) {
        result = PickBest(providers);
        if (D) Logger::D(TAG, "getBestProvider(%p, %b)=%s", criteria, enabledOnly, result.string());
        *provider = result;
        return NOERROR;
    }

    if (D) Logger::D(TAG, "getBestProvider(%p, %b)=%s", criteria, enabledOnly, result.string());
    *provider = String(NULL);
    return NOERROR;
}

String CLocationManagerService::PickBest(
    /* [in] */ IList* providers)
{
    Boolean bContain = FALSE;
    if ((providers->Contains(CoreUtils::Convert(ILocationManager::GPS_PROVIDER), &bContain), bContain)) {
        return ILocationManager::GPS_PROVIDER;
    }
    else if ((providers->Contains(CoreUtils::Convert(ILocationManager::NETWORK_PROVIDER), &bContain), bContain)) {
        return ILocationManager::NETWORK_PROVIDER;
    }
    else {
        AutoPtr<IInterface> p;
        providers->Get(0, (IInterface**)&p);
        AutoPtr<ICharSequence> pStr = ICharSequence::Probe(p);
        String str;
        pStr->ToString(&str);
        return str;
    }
}

ECode CLocationManagerService::ProviderMeetsCriteria(
    /* [in] */ const String& provider,
    /* [in] */ ICriteria* criteria,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInterface> _p;
    mProvidersByName->Get(CoreUtils::Convert(provider), (IInterface**)&_p);
    AutoPtr<ILocationProviderInterface> p = ILocationProviderInterface::Probe(_p);
    if (p == NULL) {
        // throw new IllegalArgumentException("provider=" + provider);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<ILocationProviderHelper> helper;
    CLocationProviderHelper::AcquireSingleton((ILocationProviderHelper**)&helper);
    String name;
    p->GetName(&name);
    AutoPtr<IProviderProperties> properties;
    p->GetProperties((IProviderProperties**)&properties);
    helper->PropertiesMeetCriteria(name, properties, criteria, result);
    if (D) Logger::D(TAG, "providerMeetsCriteria(%s, %p)=%b", provider.string(), criteria, result);
    return NOERROR;
}

void CLocationManagerService::UpdateProvidersLocked()
{
    Boolean changesMade = FALSE;

    AutoPtr<IIterator> it;
    mProviders->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> _p;
        it->GetNext((IInterface**)&_p);
        AutoPtr<ILocationProviderInterface> p = ILocationProviderInterface::Probe(_p);
        Boolean isEnabled = FALSE;
        p->IsEnabled(&isEnabled);
        String name;
        p->GetName(&name);
        Boolean shouldBeEnabled = IsAllowedByCurrentUserSettingsLocked(name);
        if (isEnabled && !shouldBeEnabled) {
            UpdateProviderListenersLocked(name, FALSE);
            // If any provider has been disabled, clear all last locations for all providers.
            // This is to be on the safe side in case a provider has location derived from
            // this disabled provider.
            mLastLocation->Clear();
            mLastLocationCoarseInterval->Clear();
            changesMade = TRUE;
        }
        else if (!isEnabled && shouldBeEnabled) {
            UpdateProviderListenersLocked(name, TRUE);
            changesMade = TRUE;
        }
    }
    if (changesMade) {
        AutoPtr<IIntent> intent;
        CIntent::New(ILocationManager::PROVIDERS_CHANGED_ACTION, (IIntent**)&intent);
        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        AutoPtr<IUserHandle> all;
        helper->GetALL((IUserHandle**)&all);
        mContext->SendBroadcastAsUser(intent, all);
        AutoPtr<IIntent> intentMode;
        CIntent::New(ILocationManager::MODE_CHANGED_ACTION, (IIntent**)&intentMode);
        mContext->SendBroadcastAsUser(intentMode, all);
    }
}

void CLocationManagerService::UpdateProviderListenersLocked(
    /* [in] */ const String& provider,
    /* [in] */ Boolean enabled)
{
    Int32 listeners = 0;

    AutoPtr<IInterface> _p;
    mProvidersByName->Get(CoreUtils::Convert(provider), (IInterface**)&_p);
    AutoPtr<ILocationProviderInterface> p = ILocationProviderInterface::Probe(_p);
    if (p == NULL) {
        return;
    }

    AutoPtr<IArrayList> deadReceivers;

    AutoPtr<IInterface> _records;
    mRecordsByProvider->Get(CoreUtils::Convert(provider), (IInterface**)&_records);
    AutoPtr<IArrayList> records = IArrayList::Probe(_records);

    if (records != NULL) {
        Int32 N = 0;
        records->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> p;
            records->Get(i, (IInterface**)&p);
            AutoPtr<UpdateRecord> record = (UpdateRecord*)IObject::Probe(p);
            AutoPtr<IUserHandleHelper> helper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
            Int32 userHandleId = 0;
            helper->GetUserId(record->mReceiver->mUid, &userHandleId);
            if (IsCurrentProfile(userHandleId)) {
                // Sends a notification message to the receiver
                if (!record->mReceiver->CallProviderEnabledLocked(provider, enabled)) {
                    if (deadReceivers == NULL) {
                        CArrayList::New((IArrayList**)&deadReceivers);
                    }
                    deadReceivers->Add((IInterface*)(IObject*)(record->mReceiver));
                }
                listeners++;
            }
        }
    }

    if (deadReceivers != NULL) {
        Int32 size = 0;
        deadReceivers->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> p;
            deadReceivers->Get(i, (IInterface**)&p);
            AutoPtr<Receiver> _p = (Receiver*)IObject::Probe(p);
            RemoveUpdatesLocked(_p);
        }
    }

    if (enabled) {
        p->Enable();
        if (listeners > 0) {
            ApplyRequirementsLocked(provider);
        }
    }
    else {
        p->Disable();
    }
}

void CLocationManagerService::ApplyRequirementsLocked(
    /* [in] */ const String& provider)
{
    AutoPtr<IInterface> _p;
    mProvidersByName->Get(CoreUtils::Convert(provider), (IInterface**)&_p);
    AutoPtr<ILocationProviderInterface> p = ILocationProviderInterface::Probe(_p);
    if (p == NULL) return;

    AutoPtr<IInterface> _records;
    mRecordsByProvider->Get(CoreUtils::Convert(provider), (IInterface**)&_records);
    AutoPtr<IArrayList> records = IArrayList::Probe(_records);
    AutoPtr<IWorkSource> worksource;
    CWorkSource::New((IWorkSource**)&worksource);
    AutoPtr<IProviderRequest> providerRequest;
    assert(0 && "TODO");
    // CProviderRequest::New((IProviderRequest**)&providerRequest);

    if (records != NULL) {
        AutoPtr<IIterator> it;
        records->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<UpdateRecord> record = (UpdateRecord*)IObject::Probe(p);
            AutoPtr<IUserHandleHelper> helper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
            Int32 userHandleId = 0;
            helper->GetUserId(record->mReceiver->mUid, &userHandleId);
            if (IsCurrentProfile(userHandleId)) {
                if (CheckLocationAccess(record->mReceiver->mUid, record->mReceiver->mPackageName,
                        record->mReceiver->mAllowedResolutionLevel)) {
                    AutoPtr<ILocationRequest> locationRequest = record->mRequest;
                    // providerRequest->SetLocationRequest(locationRequest);
                    Int64 locationInterval = 0, providerInterval = 0;
                    locationRequest->GetInterval(&locationInterval);
                    providerRequest->GetInterval(&providerInterval);
                    if (locationInterval < providerInterval) {
                        providerRequest->SetReportLocation(TRUE);
                        providerRequest->SetInterval(locationInterval);
                    }
                }
            }
        }

        Boolean value = FALSE;
        providerRequest->GetReportLocation(&value);
        if (value) {
            // calculate who to blame for power
            // This is somewhat arbitrary. We pick a threshold interval
            // that is slightly higher that the minimum interval, and
            // spread the blame across all applications with a request
            // under that threshold.
            Int64 interval = 0;
            providerRequest->GetInterval(&interval);
            Int64 thresholdInterval = (interval + 1000) * 3 / 2;
            it = NULL;
            records->GetIterator((IIterator**)&it);
            bHasNxt = FALSE;
            while ((it->HasNext(&bHasNxt), bHasNxt)) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<UpdateRecord> record = (UpdateRecord*)IObject::Probe(p);
                AutoPtr<IUserHandleHelper> helper;
                CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
                Int32 userHandleId;
                helper->GetUserId(record->mReceiver->mUid, &userHandleId);
                if (IsCurrentProfile(userHandleId)) {
                    AutoPtr<ILocationRequest> locationRequest = record->mRequest;
                    Int64 locationInterval;
                    locationRequest->GetInterval(&locationInterval);
                    if (locationInterval <= thresholdInterval) {
                        if (record->mReceiver->mWorkSource != NULL) {
                            Int32 size = 0;
                            record->mReceiver->mWorkSource->GetSize(&size);
                            String name;
                            record->mReceiver->mWorkSource->GetName(0, &name);
                            if ( size > 0 && !name.IsNull()) {
                                // Assign blame to another work source.
                                // Can only assign blame if the WorkSource contains names.
                                Boolean addRst;
                                worksource->Add(record->mReceiver->mWorkSource, &addRst);
                            }
                        }
                        else {
                            // Assign blame to caller.
                            Boolean addRst;
                            worksource->Add(
                                    record->mReceiver->mUid,
                                    record->mReceiver->mPackageName, &addRst);
                        }
                    }
                }
            }
        }
    }

    // if (D) Log.d(TAG, "provider request: " + provider + " " + providerRequest);
    p->SetRequest(providerRequest, worksource);
}

AutoPtr<CLocationManagerService::Receiver> CLocationManagerService::GetReceiverLocked(
    /* [in] */ IILocationListener* listener,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ Boolean hideFromAppOps)
{
    assert(0 && "TODO");
    AutoPtr<IBinder> binder;// = listener.asBinder();
    AutoPtr<IInterface> _receiver;
    mReceivers->Get(binder, (IInterface**)&_receiver);
    AutoPtr<Receiver> receiver = (Receiver*)IProxyDeathRecipient::Probe(_receiver);

    if (receiver == NULL) {
        receiver = new Receiver(listener, NULL, pid, uid, packageName, workSource, hideFromAppOps, this);
        mReceivers->Put(binder, (IInterface*)(IObject*)receiver);

        AutoPtr<IILocationListener> l;
        receiver->GetListener((IILocationListener**)&l);
        AutoPtr<IProxy> link = (IProxy*)(l->Probe(EIID_IProxy));
        if (link == NULL) {
            return NULL;
        }
        if (FAILED(link->LinkToDeath(receiver, 0))) {
            return NULL;
        }
    }
    return receiver;
}

AutoPtr<CLocationManagerService::Receiver> CLocationManagerService::GetReceiverLocked(
    /* [in] */ IPendingIntent* intent,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ Boolean hideFromAppOps)
{
    AutoPtr<IInterface> _receiver;
    mReceivers->Get(intent, (IInterface**)&_receiver);
    AutoPtr<Receiver> receiver = (Receiver*)IProxyDeathRecipient::Probe(_receiver);

    if (receiver == NULL) {
        receiver = new Receiver(NULL, intent, pid, uid, packageName, workSource, hideFromAppOps, this);
        mReceivers->Put(intent, (IInterface*)(IObject*)receiver);
    }
    return receiver;
}

AutoPtr<ILocationRequest> CLocationManagerService::CreateSanitizedRequest(
    /* [in] */ ILocationRequest* request,
    /* [in] */ Int32 resolutionLevel)
{
    AutoPtr<ILocationRequest> sanitizedRequest;
    CLocationRequest::New(request, (ILocationRequest**)&sanitizedRequest);
    Int64 interval = 0;
    if (resolutionLevel < RESOLUTION_LEVEL_FINE) {
        Int32 quality = 0;
        sanitizedRequest->GetQuality(&quality);
        switch (quality) {
            case ILocationRequest::ACCURACY_FINE:
                sanitizedRequest->SetQuality(ILocationRequest::ACCURACY_BLOCK);
                break;
            case ILocationRequest::POWER_HIGH:
                sanitizedRequest->SetQuality(ILocationRequest::POWER_LOW);
                break;
        }
        // throttle
        sanitizedRequest->GetInterval(&interval);
        if (interval < LocationFudger::FASTEST_INTERVAL_MS) {
            sanitizedRequest->SetInterval(LocationFudger::FASTEST_INTERVAL_MS);
        }
        sanitizedRequest->GetFastestInterval(&interval);
        if (interval < LocationFudger::FASTEST_INTERVAL_MS) {
            sanitizedRequest->SetFastestInterval(LocationFudger::FASTEST_INTERVAL_MS);
        }
    }
    // make getFastestInterval() the minimum of interval and fastest interval
    Int64 fastestInterval = 0;
    sanitizedRequest->GetFastestInterval(&fastestInterval);
    sanitizedRequest->GetInterval(&interval);
    if (fastestInterval > interval) {
        request->GetInterval(&interval);
        request->SetFastestInterval(interval);
    }
    return sanitizedRequest;
}

ECode CLocationManagerService::CheckPackageName(
    /* [in] */ const String& packageName)
{
    if (packageName.IsNull()) {
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("invalid package name: " + packageName);
    }
    Int32 uid = Binder::GetCallingUid();
    AutoPtr<ArrayOf<String> > packages;
    mPackageManager->GetPackagesForUid(uid, (ArrayOf<String>**)&packages);
    if (packages == NULL) {
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("invalid UID " + uid);
    }
    for (Int32 i = 0; i < packages->GetLength(); ++i) {
        if (packageName.Equals((*packages)[i])) return NOERROR;
    }
    return E_SECURITY_EXCEPTION;
    // throw new SecurityException("invalid package name: " + packageName);
}

ECode CLocationManagerService::CheckPendingIntent(
    /* [in] */ IPendingIntent* intent)
{
    if (intent == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("invalid pending intent: " + intent);
    }
    return NOERROR;
}

ECode CLocationManagerService::CheckListenerOrIntentLocked(
    /* [in] */ IILocationListener* listener,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [in] */ IWorkSource* workSource,
    /* [in] */ Boolean hideFromAppOps,
    /* [out] */ Receiver** receiver)
{
    VALIDATE_NOT_NULL(receiver);
    *receiver = NULL;

    if (intent == NULL && listener == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("need either listener or intent");
    }
    else if (intent != NULL && listener != NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("cannot register both listener and intent");
    }
    else if (intent != NULL) {
        FAIL_RETURN(CheckPendingIntent(intent));
        AutoPtr<Receiver> r = GetReceiverLocked(intent, pid, uid, packageName, workSource, hideFromAppOps);
        *receiver = r;
        REFCOUNT_ADD(*receiver);
        return NOERROR;
    }
    else {
        AutoPtr<Receiver> r = GetReceiverLocked(listener, pid, uid, packageName, workSource, hideFromAppOps);
        *receiver = r;
        REFCOUNT_ADD(*receiver);
        return NOERROR;
    }
}

ECode CLocationManagerService::RequestLocationUpdates(
    /* [in] */ ILocationRequest* request,
    /* [in] */ IILocationListener* listener,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ const String& packageName)
{
    if (request == NULL) request = DEFAULT_LOCATION_REQUEST;
    FAIL_RETURN(CheckPackageName(packageName));
    Int32 allowedResolutionLevel = GetCallerAllowedResolutionLevel();
    String provider;
    request->GetProvider(&provider);
    FAIL_RETURN(CheckResolutionLevelIsSufficientForProviderUse(
            allowedResolutionLevel, provider));
    AutoPtr<IWorkSource> workSource;
    request->GetWorkSource((IWorkSource**)&workSource);
    Int32 size = 0;
    if (workSource != NULL &&
        (workSource->GetSize(&size), size) > 0) {
        CheckDeviceStatsAllowed();
    }
    Boolean hideFromAppOps = FALSE;
    request->GetHideFromAppOps(&hideFromAppOps);
    if (hideFromAppOps) {
        CheckUpdateAppOpsAllowed();
    }
    AutoPtr<ILocationRequest> sanitizedRequest
            = CreateSanitizedRequest(request, allowedResolutionLevel);

    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();
    // providers may use public location API's, need to clear identity
    Int64 identity = Binder::ClearCallingIdentity();

    // We don't check for MODE_IGNORED here; we will do that when we go to deliver
    // a location.
    ECode ec = 0;
    CheckLocationAccess(uid, packageName, allowedResolutionLevel);
    {
        AutoLock lock(mLock);
        AutoPtr<Receiver> receiver;
        CheckListenerOrIntentLocked(listener, intent, pid, uid,
                            packageName, workSource, hideFromAppOps, (Receiver**)&receiver);
        if (receiver != NULL) {
            ec = RequestLocationUpdatesLocked(sanitizedRequest, receiver, pid, uid,
                    packageName);
        }
    }
    Binder::RestoreCallingIdentity(identity);
    return ec;
}

ECode CLocationManagerService::RequestLocationUpdatesLocked(
    /* [in] */ ILocationRequest* request,
    /* [in] */ Receiver* receiver,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    // Figure out the provider. Either its explicitly request (legacy use cases), or
    // use the fused provider
    if (request == NULL) request = DEFAULT_LOCATION_REQUEST;
    String name;
    request->GetProvider(&name);
    if (name.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("provider name must not be null");
    }

    if (D) {
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int32 hashCode = 0;
        sys->IdentityHashCode((IInterface*)(IObject*)receiver, &hashCode);
        Logger::D(TAG, "request %s %s %p from %s(%d)", StringUtils::ToHexString(hashCode).string(),
                name.string(), request, packageName.string(), uid);
    }
    AutoPtr<IInterface> _provider;
    mProvidersByName->Get(CoreUtils::Convert(name), (IInterface**)&_provider);
    AutoPtr<ILocationProviderInterface> provider = ILocationProviderInterface::Probe(_provider);
    if (provider == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("provider doesn't exist: " + name);
    }

    AutoPtr<UpdateRecord> record = new UpdateRecord(name, request, receiver, this);
    AutoPtr<IInterface> _oldRecord;
    receiver->mUpdateRecords->Put(CoreUtils::Convert(name), (IInterface*)(IObject*)(record.Get()), (IInterface**)&_oldRecord);
    AutoPtr<UpdateRecord> oldRecord = (UpdateRecord*)IObject::Probe(_oldRecord);
    if (oldRecord != NULL) {
        oldRecord->DisposeLocked(FALSE);
    }

    Boolean isProviderEnabled = IsAllowedByUserSettingsLocked(name, uid);
    if (isProviderEnabled) {
        ApplyRequirementsLocked(name);
    }
    else {
        // Notify the listener that updates are currently disabled
        receiver->CallProviderEnabledLocked(name, FALSE);
    }
    // Update the monitoring here just in case multiple location requests were added to the
    // same receiver (this request may be high power and the initial might not have been).
    receiver->UpdateMonitoring(TRUE);
    return NOERROR;
}

ECode CLocationManagerService::RemoveUpdates(
    /* [in] */ IILocationListener* listener,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ const String& packageName)
{
    FAIL_RETURN(CheckPackageName(packageName));

    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();

    AutoLock lock(mLock);
    AutoPtr<IWorkSource> workSource;
    Boolean hideFromAppOps = FALSE;
    AutoPtr<Receiver> receiver;
    CheckListenerOrIntentLocked(listener, intent, pid, uid,
            packageName, workSource, hideFromAppOps, (Receiver**)&receiver);

    // providers may use public location API's, need to clear identity
    Int64 identity = Binder::ClearCallingIdentity();

    if (receiver != NULL) {
        RemoveUpdatesLocked(receiver);
    }

    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

void CLocationManagerService::RemoveUpdatesLocked(
    /* [in] */ Receiver* receiver)
{
    if (D) {
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int32 hashCode = 0;
        sys->IdentityHashCode((IInterface*)(IObject*)receiver, &hashCode);
        Logger::I(TAG, "remove %s", StringUtils::ToHexString(hashCode).string());
    }

    AutoPtr<IInterface> old;
    mReceivers->Remove(receiver->mKey, (IInterface**)&old);
    if (old != NULL && receiver->IsListener()) {
        // receiver.getListener().asBinder().unlinkToDeath(receiver, 0);
        AutoLock lock(receiver->mLock);
        receiver->ClearPendingBroadcastsLocked();
    }

    receiver->UpdateMonitoring(FALSE);

    // Record which providers were associated with this listener
    AutoPtr<IHashSet> providers;
    CHashSet::New((IHashSet**)&providers);
    AutoPtr<IHashMap> oldRecords = receiver->mUpdateRecords;
    if (oldRecords != NULL) {
        // Call dispose() on the obsolete update records.
        AutoPtr<ICollection> cl;
        oldRecords->GetValues((ICollection**)&cl);
        AutoPtr<IIterator> it;
        cl->GetIterator((IIterator**)&it);
        Boolean bHasNxt = FALSE;
        while ((it->HasNext(&bHasNxt), bHasNxt)) {
            AutoPtr<IInterface> p;
            it->GetNext((IInterface**)&p);
            AutoPtr<UpdateRecord> record = (UpdateRecord*)IObject::Probe(p);
            // Update statistics for historical location requests by package/provider
            record->DisposeLocked(FALSE);
        }
        // Accumulate providers
        AutoPtr<ISet> st;
        oldRecords->GetKeySet((ISet**)&st);
        providers->AddAll(ICollection::Probe(st));
    }

    // update provider
    AutoPtr<IIterator> itP;
    providers->GetIterator((IIterator**)&itP);
    Boolean b = FALSE;
    while ((itP->HasNext(&b), b)) {
        AutoPtr<IInterface> p;
        itP->GetNext((IInterface**)&p);
        AutoPtr<ICharSequence> _p = ICharSequence::Probe(p);
        String provider;
        _p->ToString(&provider);
        // If provider is already disabled, don't need to do anything
        if (!IsAllowedByCurrentUserSettingsLocked(provider)) {
            continue;
        }

        ApplyRequirementsLocked(provider);
    }
}

void CLocationManagerService::ApplyAllProviderRequirementsLocked()
{
    AutoPtr<IIterator> it;
    mProviders->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> _p;
        it->GetNext((IInterface**)&_p);
        AutoPtr<ILocationProviderInterface> p = ILocationProviderInterface::Probe(_p);
        // If provider is already disabled, don't need to do anything
        String name;
        p->GetName(&name);
        if (!IsAllowedByCurrentUserSettingsLocked(name)) {
            continue;
        }

        ApplyRequirementsLocked(name);
    }
}

ECode CLocationManagerService::GetLastLocation(
    /* [in] */ ILocationRequest* request,
    /* [in] */ const String& packageName,
    /* [out] */ ILocation** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    if (D) {
        Logger::D(TAG, "getLastLocation: %p", request);
    }
    if (request == NULL) request = DEFAULT_LOCATION_REQUEST;
    Int32 allowedResolutionLevel = GetCallerAllowedResolutionLevel();
    FAIL_RETURN(CheckPackageName(packageName));
    String name;
    request->GetProvider(&name);
    FAIL_RETURN(CheckResolutionLevelIsSufficientForProviderUse(allowedResolutionLevel,
            name));
    // no need to sanitize this request, as only the provider name is used

    Int32 uid = Binder::GetCallingUid();
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    Boolean b = FALSE;
    mBlacklist->IsBlacklisted(packageName, &b);
    if (b) {
        if (D) {
            Logger::D(TAG, "not returning last loc for blacklisted app: %s",
                (const char*)packageName);
        }
        *result = NULL;
        Binder::RestoreCallingIdentity(identity);
        return NOERROR;
    }

    if (!ReportLocationAccessNoThrow(uid, packageName, allowedResolutionLevel)) {
        if (D) {
            Logger::D(TAG, "not returning last loc for no op app: %s",
                (const char*)packageName);
        }
        *result = NULL;
        Binder::RestoreCallingIdentity(identity);
        return NOERROR;
    }

    AutoLock lock(mLock);
    // Figure out the provider. Either its explicitly request (deprecated API's),
    // or use the fused provider
    if (name.IsNull()) name = ILocationManager::FUSED_PROVIDER;
    AutoPtr<IInterface> _provider;
    mProvidersByName->Get(CoreUtils::Convert(name), (IInterface**)&_provider);
    AutoPtr<ILocationProviderInterface> provider = ILocationProviderInterface::Probe(_provider);
    if (provider == NULL) {
        Binder::RestoreCallingIdentity(identity);
        *result = NULL;
        return NOERROR;
    }

    if (!IsAllowedByUserSettingsLocked(name, uid)) {
        Binder::RestoreCallingIdentity(identity);
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<ILocation> location;
    if (allowedResolutionLevel < RESOLUTION_LEVEL_FINE) {
        // Make sure that an app with coarse permissions can't get frequent location
        // updates by calling LocationManager.getLastKnownLocation repeatedly.
        AutoPtr<IInterface> p;
        mLastLocationCoarseInterval->Get(CoreUtils::Convert(name), (IInterface**)&p);
        location = ILocation::Probe(p);
    }
    else {
        AutoPtr<IInterface> p;
        mLastLocation->Get(CoreUtils::Convert(name), (IInterface**)&p);
        location = ILocation::Probe(p);
    }
    if (location == NULL) {
        Binder::RestoreCallingIdentity(identity);
        *result = NULL;
        return NOERROR;
    }
    if (allowedResolutionLevel < RESOLUTION_LEVEL_FINE) {
        AutoPtr<ILocation> noGPSLocation;
        location->GetExtraLocation(ILocation::EXTRA_NO_GPS_LOCATION, (ILocation**)&noGPSLocation);
        if (noGPSLocation != NULL) {
            AutoPtr<ILocation> temp;
            mLocationFudger->GetOrCreate(noGPSLocation, (ILocation**)&temp);
            AutoPtr<ILocation> l;
            CLocation::New(temp, (ILocation**)&l);
            Binder::RestoreCallingIdentity(identity);
            *result = l;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
    }
    else {
        AutoPtr<ILocation> l;
        CLocation::New(location, (ILocation**)&l);
        Binder::RestoreCallingIdentity(identity);
        *result = l;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CLocationManagerService::RequestGeofence(
    /* [in] */ ILocationRequest* request,
    /* [in] */ IGeofence* geofence,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ const String& packageName)
{
    if (request == NULL) request = DEFAULT_LOCATION_REQUEST;
    Int32 allowedResolutionLevel = GetCallerAllowedResolutionLevel();
    FAIL_RETURN(CheckResolutionLevelIsSufficientForGeofenceUse(allowedResolutionLevel));
    FAIL_RETURN(CheckPendingIntent(intent));
    FAIL_RETURN(CheckPackageName(packageName));
    String name;
    request->GetProvider(&name);
    FAIL_RETURN(CheckResolutionLevelIsSufficientForProviderUse(allowedResolutionLevel,
            name));
    AutoPtr<ILocationRequest> sanitizedRequest = CreateSanitizedRequest(
            request, allowedResolutionLevel);

    if (D) Logger::D(TAG, "requestGeofence: %p %p %p", sanitizedRequest.Get(), geofence, intent);

    // geo-fence manager uses the public location API, need to clear identity
    Int32 uid = Binder::GetCallingUid();

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId = 0;
    helper->GetUserId(uid, &userId);
    if (userId != IUserHandle::USER_OWNER) {
        // temporary measure until geofences work for secondary users
        Logger::W(TAG, "proximity alerts are currently available only to the primary user");
        return NOERROR;
    }
    Int64 identity = Binder::ClearCallingIdentity();

    if (mGeoFencer != NULL && mGeoFencerEnabled) {
        Int64 expiration = 0, expi = 0;
        sanitizedRequest->GetExpireAt(&expi);
        if (expi == Elastos::Core::Math::INT64_MAX_VALUE) {
            expiration = -1; // -1 means forever
        }
        else {
            expiration = expi - SystemClock::GetElapsedRealtime();
        }
        Double lat = 0.0, tud = 0.0;
        Float rad = 0.0;
        geofence->GetLatitude(&lat);
        geofence->GetLongitude(&tud);
        geofence->GetRadius(&rad);
        AutoPtr<IGeoFenceParams> p;
        CGeoFenceParams::New(uid, lat,
                          tud, rad,
                          expiration, intent, packageName,
                          (IGeoFenceParams**)&p);
        mGeoFencer->Add(p);
    }
    else {
        mGeofenceManager->AddFence(sanitizedRequest, geofence, intent,
                                  allowedResolutionLevel, uid, packageName);
    }

    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CLocationManagerService::RemoveGeofence(
    /* [in] */ IGeofence* geofence,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ const String& packageName)
{
    FAIL_RETURN(CheckResolutionLevelIsSufficientForGeofenceUse(
            GetCallerAllowedResolutionLevel()));
    FAIL_RETURN(CheckPendingIntent(intent));
    FAIL_RETURN(CheckPackageName(packageName));

    if (D) Logger::D(TAG, "removeGeofence: %p %p", geofence, intent);

    // geo-fence manager uses the public location API, need to clear identity
    Int64 identity = Binder::ClearCallingIdentity();

    if (mGeoFencer != NULL && mGeoFencerEnabled) {
        mGeoFencer->Remove(intent);
    }
    else {
        mGeofenceManager->RemoveFence(geofence, intent);
    }

    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CLocationManagerService::AddGpsStatusListener(
    /* [in] */ IIGpsStatusListener* listener,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mGpsStatusProvider == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    Int32 allowedResolutionLevel = GetCallerAllowedResolutionLevel();
    FAIL_RETURN(CheckResolutionLevelIsSufficientForProviderUse(
            allowedResolutionLevel,
            ILocationManager::GPS_PROVIDER));

    Int32 uid = Binder::GetCallingUid();
    Int64 ident = Binder::ClearCallingIdentity();
    if (!CheckLocationAccess(uid, packageName, allowedResolutionLevel)) {
        *result = FALSE;
        return NOERROR;
    }
    Binder::RestoreCallingIdentity(ident);

    ECode ec = mGpsStatusProvider->AddGpsStatusListener(listener);
    if (FAILED(ec)) {
        Slogger::E(TAG, "mGpsStatusProvider.addGpsStatusListener failed%d", ec);
        *result = FALSE;
        return ec;
    }
    *result = TRUE;
    return NOERROR;
}

ECode CLocationManagerService::RemoveGpsStatusListener(
    /* [in] */ IIGpsStatusListener* listener)
{
    AutoLock lock(&mLock);
        // try {
    return mGpsStatusProvider->RemoveGpsStatusListener(listener);
        // } catch (Exception e) {
        //     Slog.e(TAG, "mGpsStatusProvider.removeGpsStatusListener failed", e);
        // }
}

ECode CLocationManagerService::AddGpsMeasurementsListener(
    /* [in] */ IIGpsMeasurementsListener* listener,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 allowedResolutionLevel = GetCallerAllowedResolutionLevel();
    CheckResolutionLevelIsSufficientForProviderUse(
            allowedResolutionLevel,
            ILocationManager::GPS_PROVIDER);

    Int32 uid = Binder::GetCallingUid();
    Int64 identity = Binder::ClearCallingIdentity();
    Boolean hasLocationAccess;

    hasLocationAccess = CheckLocationAccess(uid, packageName, allowedResolutionLevel);

    Binder::RestoreCallingIdentity(identity);

    if (!hasLocationAccess) {
        *result = FALSE;
        return NOERROR;
    }
    return mGpsMeasurementsProvider->AddListener(listener, result);
}

ECode CLocationManagerService::RemoveGpsMeasurementsListener(
    /* [in] */ IIGpsMeasurementsListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mGpsMeasurementsProvider->RemoveListener(listener, result);
}

ECode CLocationManagerService::AddGpsNavigationMessageListener(
    /* [in] */ IIGpsNavigationMessageListener* listener,
    /* [in] */ const String& packageName,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 allowedResolutionLevel = GetCallerAllowedResolutionLevel();
    CheckResolutionLevelIsSufficientForProviderUse(
            allowedResolutionLevel,
            ILocationManager::GPS_PROVIDER);

    Int32 uid = Binder::GetCallingUid();
    Int64 identity = Binder::ClearCallingIdentity();
    Boolean hasLocationAccess;

    hasLocationAccess = CheckLocationAccess(uid, packageName, allowedResolutionLevel);

    Binder::RestoreCallingIdentity(identity);

    if (!hasLocationAccess) {
        *result = FALSE;
        return NOERROR;
    }
    return mGpsNavigationMessageProvider->AddListener(listener, result);
}

ECode CLocationManagerService::RemoveGpsNavigationMessageListener(
    /* [in] */ IIGpsNavigationMessageListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mGpsNavigationMessageProvider->RemoveListener(listener, result);
}

ECode CLocationManagerService::SendExtraCommand(
    /* [in] */ const String& provider,
    /* [in] */ const String& command,
    /* [out] */ IBundle** extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(extras);


    if (provider.IsNull()) {
        // throw NullPointerException to remain compatible with previous implementation
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    FAIL_RETURN(CheckResolutionLevelIsSufficientForProviderUse(
            GetCallerAllowedResolutionLevel(), provider));

    // and check for ACCESS_LOCATION_EXTRA_COMMANDS
    Int32 perm = 0;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(ACCESS_LOCATION_EXTRA_COMMANDS, &perm));
    if ((perm != IPackageManager::PERMISSION_GRANTED)) {
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("Requires ACCESS_LOCATION_EXTRA_COMMANDS permission");
    }

    AutoLock lock(mLock);
    AutoPtr<IInterface> _p;
    mProvidersByName->Get(CoreUtils::Convert(provider), (IInterface**)&_p);
    AutoPtr<ILocationProviderInterface> p = ILocationProviderInterface::Probe(_p);
    if (p == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    p->SendExtraCommand(command, *extras, result);
    return NOERROR;
}

ECode CLocationManagerService::SendNiResponse(
    /* [in] */ Int32 notifId,
    /* [in] */ Int32 userResponse,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (Binder::GetCallingUid() != Process::MyUid()) {
        // throw new SecurityException(
        //        "calling sendNiResponse from outside of the system is not allowed");
        return E_SECURITY_EXCEPTION;
    }

    ECode ec = mNetInitiatedListener->SendNiResponse(notifId, userResponse, result);
    if (FAILED(ec)) {
        Slogger::E(TAG, "RemoteException in LocationManagerService.sendNiResponse");
        *result = FALSE;
    }
    return ec;
}

ECode CLocationManagerService::GetProviderProperties(
    /* [in] */ const String& provider,
    /* [out] */ IProviderProperties** properties)
{
    VALIDATE_NOT_NULL(properties);
    *properties = NULL;

    AutoPtr<IInterface> _p;
    mProvidersByName->Get(CoreUtils::Convert(provider), (IInterface**)&_p);
    AutoPtr<ILocationProviderInterface> p = ILocationProviderInterface::Probe(_p);
    if (p == NULL) {
        return NOERROR;
    }

    FAIL_RETURN(CheckResolutionLevelIsSufficientForProviderUse(
            GetCallerAllowedResolutionLevel(), provider));
    {
        AutoLock lock(mLock);
        _p = NULL;
        mProvidersByName->Get(CoreUtils::Convert(provider), (IInterface**)&_p);
        p = NULL;
        p = ILocationProviderInterface::Probe(_p);
    }

    if (p == NULL) {
        return NOERROR;
    }
    return p->GetProperties(properties);
}

ECode CLocationManagerService::IsProviderEnabled(
    /* [in] */ const String& provider,
    /* [out] */ Boolean* isEnabled)
{
    VALIDATE_NOT_NULL(isEnabled);
    *isEnabled = FALSE;

    // Fused provider is accessed indirectly via criteria rather than the provider-based APIs,
    // so we discourage its use
    if (ILocationManager::FUSED_PROVIDER.Equals(provider)) {
        return NOERROR;
    }

    Int32 uid = Binder::GetCallingUid();
    Int64 identity = Binder::ClearCallingIdentity();
    AutoLock lock(mLock);
    AutoPtr<IInterface> _p;
    mProvidersByName->Get(CoreUtils::Convert(provider), (IInterface**)&_p);
    AutoPtr<ILocationProviderInterface> p = ILocationProviderInterface::Probe(_p);
    if (p == NULL) {
        Binder::RestoreCallingIdentity(identity);
        return NOERROR;
    }

    *isEnabled = IsAllowedByUserSettingsLocked(provider, uid);
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

Boolean CLocationManagerService::IsUidALocationProvider(
    /* [in] */ Int32 uid)
{
    if (uid == IProcess::SYSTEM_UID) {
        return TRUE;
    }
    if (mGeocodeProvider != NULL) {
        String gName;
        mGeocodeProvider->GetConnectedPackageName(&gName);
        if (DoesUidHavePackage(uid, gName)) return TRUE;
    }
    AutoPtr<IIterator> it;
    mProxyProviders->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<LocationProviderProxy> proxy = (LocationProviderProxy*)ILocationProviderInterface::Probe(p);
        String pName;
        proxy->GetConnectedPackageName(&pName);
        if (DoesUidHavePackage(uid, pName)) return TRUE;
    }
    return FALSE;
}

ECode CLocationManagerService::CheckCallerIsProvider()
{
    Int32 perm;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(INSTALL_LOCATION_PROVIDER, &perm));
    if (perm == IPackageManager::PERMISSION_GRANTED) {
        return NOERROR;
    }

    // Previously we only used the INSTALL_LOCATION_PROVIDER
    // check. But that is system or signature
    // protection level which is not flexible enough for
    // providers installed oustide the system image. So
    // also allow providers with a UID matching the
    // currently bound package name

    if (IsUidALocationProvider(Binder::GetCallingUid())) {
        return NOERROR;
    }

    // throw new SecurityException("need INSTALL_LOCATION_PROVIDER permission, " +
    //             "or UID of a currently bound location provider");
    return E_SECURITY_EXCEPTION;
}

ECode CLocationManagerService::ReportLocation(
    /* [in] */ ILocation* location,
    /* [in] */ Boolean passive)
{
    FAIL_RETURN(CheckCallerIsProvider());

    Boolean isComplete;
    location->IsComplete(&isComplete);
    if (!isComplete) {
        Logger::W(TAG, "Dropping incomplete location: %p", location);
        return NOERROR;
    }

    mLocationHandler->RemoveMessages(MSG_LOCATION_CHANGED, location);
    AutoPtr<IMessage> msg;
    mLocationHandler->ObtainMessage(MSG_LOCATION_CHANGED, location, (IMessage**)&msg);
    msg->SetArg1(passive ? 1 : 0);
    Boolean result;
    return mLocationHandler->SendMessageAtFrontOfQueue(msg, &result);
}

Boolean CLocationManagerService::DoesUidHavePackage(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    if (packageName.IsNull()) {
        return FALSE;
    }

    AutoPtr<ArrayOf<String> > packageNames;
    mPackageManager->GetPackagesForUid(uid, (ArrayOf<String>**)&packageNames);
    if (packageNames == NULL) {
        return FALSE;
    }
    for (Int32 i = 0; i < packageNames->GetLength(); i++) {
        String name = (*packageNames)[i];
        if (packageName.Equals(name)) {
            return TRUE;
        }
    }
    return FALSE;
}

Boolean CLocationManagerService::ShouldBroadcastSafe(
    /* [in] */ ILocation* loc,
    /* [in] */ ILocation* lastLoc,
    /* [in] */ UpdateRecord* record,
    /* [in] */ Int64 now)
{
    // Always broadcast the first update
    if (lastLoc == NULL) {
        return TRUE;
    }

    // Check whether sufficient time has passed
    Int64 minTime = 0;
    record->mRequest->GetFastestInterval(&minTime);
    Int64 nanos = 0, lastNanos = 0;
    loc->GetElapsedRealtimeNanos(&nanos);
    lastLoc->GetElapsedRealtimeNanos(&lastNanos);
    Int64 delta = (nanos - lastNanos) / NANOS_PER_MILLI;
    if (delta < minTime - MAX_PROVIDER_SCHEDULING_JITTER_MS) {
        return FALSE;
    }

    // Check whether sufficient distance has been traveled
    Double minDistance;
    Float distanceTemp;
    record->mRequest->GetSmallestDisplacement(&distanceTemp);
    minDistance = distanceTemp;
    if (minDistance > 0.0) {
        Float dis;
        loc->DistanceTo(lastLoc, &dis);
        if (dis <= minDistance) {
            return FALSE;
        }
    }

    // Check whether sufficient number of udpates is left
    Int32 numUpdates = 0;
    record->mRequest->GetNumUpdates(&numUpdates);
    if (numUpdates <= 0) {
        return FALSE;
    }

    // Check whether the expiry date has passed
    Int64 value = 0;
    record->mRequest->GetExpireAt(&value);
    if (value < now) {
        return FALSE;
    }

    return TRUE;
}

void CLocationManagerService::HandleLocationChangedLocked(
    /* [in] */ ILocation* location,
    /* [in] */ Boolean passive)
{
    if (D) {
        Logger::D(TAG, "incoming location: %p", location);
    }

    Int64 now = SystemClock::GetElapsedRealtime();
    String provider;
    if(passive) {
        provider = ILocationManager::PASSIVE_PROVIDER;
    }
    else {
        location->GetProvider(&provider);
    }

    // Skip if the provider is unknown.
    AutoPtr<IInterface> _p;
    mProvidersByName->Get(CoreUtils::Convert(provider), (IInterface**)&_p);
    AutoPtr<ILocationProviderInterface> p = ILocationProviderInterface::Probe(_p);
    if (p == NULL) {
        return;
    }

    // Update last known location for locations
    AutoPtr<ILocation> noGPSLocation;
    location->GetExtraLocation(ILocation::EXTRA_NO_GPS_LOCATION, (ILocation**)&noGPSLocation);
    AutoPtr<ILocation> lastNoGPSLocation;
    AutoPtr<IInterface> _lastLocation;
    mLastLocation->Get(CoreUtils::Convert(provider), (IInterface**)&_lastLocation);
    AutoPtr<ILocation> lastLocation = ILocation::Probe(_lastLocation);
    if (lastLocation == NULL) {
        CLocation::New(provider, (ILocation**)&lastLocation);
        mLastLocation->Put(CoreUtils::Convert(provider), lastLocation);
    }
    else {
        lastLocation->GetExtraLocation(ILocation::EXTRA_NO_GPS_LOCATION,
                (ILocation**)&lastNoGPSLocation);
        if (noGPSLocation == NULL && lastNoGPSLocation != NULL) {
            // New location has no no-GPS location: adopt last no-GPS location. This is set
            // directly into location because we do not want to notify COARSE clients.
            location->SetExtraLocation(ILocation::EXTRA_NO_GPS_LOCATION, lastNoGPSLocation);
        }
    }
    lastLocation->Set(location);

    // Update last known coarse interval location if enough time has passed.
    AutoPtr<IInterface> pIn;
    mLastLocationCoarseInterval->Get(CoreUtils::Convert(provider), (IInterface**)&pIn);
    AutoPtr<ILocation> lastLocationCoarseInterval = ILocation::Probe(pIn);
    if (lastLocationCoarseInterval == NULL) {
        CLocation::New(location, (ILocation**)&lastLocationCoarseInterval);
        mLastLocationCoarseInterval->Put(CoreUtils::Convert(provider), lastLocationCoarseInterval);
    }
    Int64 lNanos = 0, iNano = 0;
    location->GetElapsedRealtimeNanos(&lNanos);
    lastLocationCoarseInterval->GetElapsedRealtimeNanos(&iNano);
    Int64 timeDiffNanos = lNanos - iNano;
    if (timeDiffNanos > LocationFudger::FASTEST_INTERVAL_MS * NANOS_PER_MILLI) {
        lastLocationCoarseInterval->Set(location);
    }
    // Don't ever return a coarse location that is more recent than the allowed update
    // interval (i.e. don't allow an app to keep registering and unregistering for
    // location updates to overcome the minimum interval).
    lastLocationCoarseInterval->GetExtraLocation(ILocation::EXTRA_NO_GPS_LOCATION, (ILocation**)&noGPSLocation);

    // Skip if there are no UpdateRecords for this provider.
    AutoPtr<IInterface> _records;
    mRecordsByProvider->Get(CoreUtils::Convert(provider), (IInterface**)&_records);
    AutoPtr<IArrayList> records = IArrayList::Probe(_records);
    Int32 size = 0;
    if (records == NULL || (records->GetSize(&size), size) == 0) return;

    // Fetch coarse location
    AutoPtr<ILocation> coarseLocation;
    if (noGPSLocation != NULL) {
        mLocationFudger->GetOrCreate(noGPSLocation, (ILocation**)&coarseLocation);
    }

    // Fetch latest status update time
    Int64 newStatusUpdateTime;
    p->GetStatusUpdateTime(&newStatusUpdateTime);

    // Get latest status
    AutoPtr<IBundle> extras;
    CBundle::New((IBundle**)&extras);
    Int32 status;
    p->GetStatus(extras, &status);

    AutoPtr<IArrayList> deadReceivers;
    AutoPtr<IArrayList> deadUpdateRecords;

    // Broadcast location or status to all listeners
    AutoPtr<IIterator> it;
    records->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> _r;
        it->GetNext((IInterface**)&_r);
        AutoPtr<UpdateRecord> r = (UpdateRecord*)IObject::Probe(_r);
        AutoPtr<Receiver> receiver = r->mReceiver;
        Boolean receiverDead = FALSE;

        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 receiverUserId;
        helper->GetUserId(receiver->mUid, &receiverUserId);
        if (!IsCurrentProfile(receiverUserId) &&
            !IsUidALocationProvider(receiver->mUid)) {
            if (D) {
                Logger::D(TAG, "skipping loc update for background user %d (current user: %d, app: %s)",
                    receiverUserId, mCurrentUserId, receiver->mPackageName.string());
            }
            continue;
        }

        Boolean bl = FALSE;
        mBlacklist->IsBlacklisted(receiver->mPackageName, &bl);
        if (bl) {
            if (D) {
                Logger::D(TAG, "skipping loc update for blacklisted app: %s",
                    receiver->mPackageName.string());
            }
            continue;
        }

        if (!ReportLocationAccessNoThrow(receiver->mUid, receiver->mPackageName,
                receiver->mAllowedResolutionLevel)) {
            if (D) {
                Logger::D(TAG, "skipping loc update for no op app: %s",
                    receiver->mPackageName.string());
            }
            continue;
        }

        AutoPtr<ILocation> notifyLocation;
        if (receiver->mAllowedResolutionLevel < RESOLUTION_LEVEL_FINE) {
            notifyLocation = coarseLocation;  // use coarse location
        }
        else {
            notifyLocation = lastLocation;  // use fine location
        }
        if (notifyLocation != NULL) {
            AutoPtr<ILocation> lastLoc = r->mLastFixBroadcast;
            if ((lastLoc == NULL) || ShouldBroadcastSafe(notifyLocation, lastLoc, r, now)) {
                if (lastLoc == NULL) {
                    CLocation::New(notifyLocation, (ILocation**)&lastLoc);
                    r->mLastFixBroadcast = lastLoc;
                }
                else {
                    lastLoc->Set(notifyLocation);
                }
                if (!receiver->CallLocationChangedLocked(notifyLocation)) {
                    Slogger::W(TAG, "RemoteException calling onLocationChanged on %p", receiver.Get());
                    receiverDead = TRUE;
                }
                r->mRequest->DecrementNumUpdates();
            }
        }

        Int64 prevStatusUpdateTime = r->mLastStatusBroadcast;
        if ((newStatusUpdateTime > prevStatusUpdateTime) &&
            (prevStatusUpdateTime != 0 || status != ILocationProvider::AVAILABLE)) {
            r->mLastStatusBroadcast = newStatusUpdateTime;
            if (!receiver->CallStatusChangedLocked(provider, status, extras)) {
                receiverDead = TRUE;
                Slogger::W(TAG, "RemoteException calling onStatusChanged on %p", receiver.Get());
            }
        }

        // track expired records
        Int32 numUpdates = 0;
        r->mRequest->GetNumUpdates(&numUpdates);
        Int64 value = 0;
        r->mRequest->GetExpireAt(&value);
        if (numUpdates <= 0 || value < now) {
            deadUpdateRecords->Add(_r);
        }

        // track dead receivers
        if (receiverDead) {
            if (deadReceivers == NULL) {
                CArrayList::New((IArrayList**)&deadReceivers);
            }
            Boolean bContain = FALSE;
            deadReceivers->Contains((IInterface*)(IObject*)receiver.Get(), &bContain);
            if (!bContain) {
                deadReceivers->Add((IInterface*)(IObject*)receiver.Get());
            }
        }
    }

    // remove dead records and receivers outside the loop
    if (deadReceivers != NULL) {
        AutoPtr<IIterator> recIt;
        deadReceivers->GetIterator((IIterator**)&recIt);
        Boolean b = FALSE;
        while ((recIt->HasNext(&b), b)) {
            AutoPtr<IInterface> p;
            recIt->GetNext((IInterface**)&p);
            AutoPtr<Receiver> receiver = (Receiver*)IProxyDeathRecipient::Probe(p);
            RemoveUpdatesLocked(receiver);
        }
    }

    if (deadUpdateRecords != NULL) {
        AutoPtr<IIterator> recIt;
        deadUpdateRecords->GetIterator((IIterator**)&recIt);
        Boolean b = FALSE;
        while ((recIt->HasNext(&b), b)) {
            AutoPtr<IInterface> p;
            recIt->GetNext((IInterface**)&p);
            AutoPtr<UpdateRecord> r = (UpdateRecord*)IObject::Probe(p);
            r->DisposeLocked(TRUE);
        }
        ApplyRequirementsLocked(provider);
    }
}

Boolean CLocationManagerService::IsMockProvider(
    /* [in] */ String provider)
{
    AutoLock lock(mLock);
    Boolean b = FALSE;
    mMockProviders->ContainsKey(CoreUtils::Convert(provider), &b);
    return b;
}

AutoPtr<ILocation> CLocationManagerService::ScreenLocationLocked(
    /* [in] */ ILocation* location,
    /* [in] */ String provider)
{
    if (IsMockProvider(ILocationManager::NETWORK_PROVIDER)) {
        return location;
    }
    AutoPtr<IInterface> p;
    mProvidersByName->Get(CoreUtils::Convert(ILocationManager::NETWORK_PROVIDER), (IInterface**)&p);
    AutoPtr<LocationProviderProxy> providerProxy = (LocationProviderProxy*)IObject::Probe(p);
    if (mComboNlpPackageName == NULL || providerProxy == NULL ||
        FALSE == provider.Equals(ILocationManager::NETWORK_PROVIDER) ||
        IsMockProvider(ILocationManager::NETWORK_PROVIDER)) {
        return location;
    }

    String connectedNlpPackage;
    providerProxy->GetConnectedPackageName(&connectedNlpPackage);
    if (connectedNlpPackage.IsNull() || !connectedNlpPackage.Equals(mComboNlpPackageName)) {
        return location;
    }

    AutoPtr<IBundle> extras;
    location->GetExtras((IBundle**)&extras);
    Boolean isBeingScreened = FALSE;
    if (extras == NULL) {
        CBundle::New((IBundle**)&extras);
    }

    Boolean bContain = FALSE;
    extras->ContainsKey(mComboNlpReadyMarker, &bContain);
    if (!bContain) {
        // see if Combo Nlp is a passive listener
        AutoPtr<IInterface> _records;
        mRecordsByProvider->Get(CoreUtils::Convert(ILocationManager::PASSIVE_PROVIDER), (IInterface**)&_records);
        AutoPtr<IArrayList> records = IArrayList::Probe(_records);
        if (records != NULL) {
            AutoPtr<IIterator> it;
            records->GetIterator((IIterator**)&it);
            Boolean bHasNxt = FALSE;
            while ((it->HasNext(&bHasNxt), bHasNxt)) {
                AutoPtr<IInterface> _r;
                it->GetNext((IInterface**)&_r);
                AutoPtr<UpdateRecord> r = (UpdateRecord*)IObject::Probe(_r);
                if (r->mReceiver->mPackageName.Equals(mComboNlpPackageName)) {
                    if (!isBeingScreened) {
                        isBeingScreened = TRUE;
                        extras->PutBoolean(mComboNlpScreenMarker, TRUE);
                    }
                    // send location to Combo Nlp for screening
                    if (!r->mReceiver->CallLocationChangedLocked(location)) {
                        Slogger::W(TAG, "RemoteException calling onLocationChanged on %p",
                               r->mReceiver.Get());
                    }
                    else {
                        if (D) {
                            Logger::D(TAG, "Sending location for screening");
                        }
                    }
                }
            }
        }
        if (isBeingScreened) {
            return NULL;
        }
        if (D) {
            Logger::D(TAG, "Not screening locations");
        }
    }
    else {
        if (D) {
            Logger::D(TAG, "This location is marked as ready for broadcast");
        }
        // clear the ready marker
        extras->Remove(mComboNlpReadyMarker);
    }

    return location;
}

void CLocationManagerService::HandleLocationChanged(
    /* [in] */ ILocation* location,
    /* [in] */ Boolean passive)
{
    // create a working copy of the incoming Location so that the service can modify it without
    // disturbing the caller's copy
    AutoPtr<ILocation> myLocation;
    CLocation::New(location, (ILocation**)&myLocation);
    String provider;
    myLocation->GetProvider(&provider);

    // set "isFromMockProvider" bit if location came from a mock provider. we do not clear this
    // bit if location did not come from a mock provider because passive/fused providers can
    // forward locations from mock providers, and should not grant them legitimacy in doing so.
    Boolean b = FALSE;
    myLocation->IsFromMockProvider(&b);
    if (!b && IsMockProvider(provider)) {
        myLocation->SetIsFromMockProvider(TRUE);
    }

    AutoLock lock(mLock);
    if (IsAllowedByCurrentUserSettingsLocked(provider)) {
        if (!passive) {
            location = ScreenLocationLocked(location, provider);
            if (location == NULL) {
                return;
            }
            // notify passive provider of the new location
            mPassiveProvider->UpdateLocation(myLocation);
        }
        HandleLocationChangedLocked(myLocation, passive);
    }
}

ECode CLocationManagerService::GeocoderIsPresent(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    PFL_EX("CLocationManagerService::GeocoderIsPresent")
    *result = mGeocodeProvider != NULL;
    return NOERROR;
}

ECode CLocationManagerService::GetFromLocation(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Int32 maxResults,
    /* [in] */ IGeocoderParams* params,
    /* [out] */ IList** addrs,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(addrs);
    VALIDATE_NOT_NULL(result);

    if (mGeocodeProvider != NULL) {
        return mGeocodeProvider->GetFromLocation(latitude, longitude, maxResults,
                params, addrs, result);
    }
    *result = String(NULL);
    return NOERROR;
}

ECode CLocationManagerService::GetFromLocationName(
    /* [in] */ const String& locationName,
    /* [in] */ Double lowerLeftLatitude,
    /* [in] */ Double lowerLeftLongitude,
    /* [in] */ Double upperRightLatitude,
    /* [in] */ Double upperRightLongitude,
    /* [in] */ Int32 maxResults,
    /* [in] */ IGeocoderParams* params,
    /* [out] */ IList** addrs,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(addrs);
    VALIDATE_NOT_NULL(result);

    if (mGeocodeProvider != NULL) {
        return mGeocodeProvider->GetFromLocationName(locationName, lowerLeftLatitude,
                lowerLeftLongitude, upperRightLatitude, upperRightLongitude,
                maxResults, params, addrs, result);
    }
    *result = String(NULL);
    return NOERROR;
}

ECode CLocationManagerService::CheckMockPermissionsSafe()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 mockLocation = 0;
    AutoPtr<ISettingsSecure> settingsSecure;
    //CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetInt32(cr, ISettingsSecure::ALLOW_MOCK_LOCATION, 0, &mockLocation);
    Boolean allowMocks = mockLocation == 1;
    if (!allowMocks) {
        //throw new SecurityException("Requires ACCESS_MOCK_LOCATION secure setting");
        return E_SECURITY_EXCEPTION;
    }

    Int32 value = 0;
    FAIL_RETURN(mContext->CheckCallingPermission(ACCESS_MOCK_LOCATION, &value));
    if (value != IPackageManager::PERMISSION_GRANTED) {
        // throw new SecurityException("Requires ACCESS_MOCK_LOCATION permission");
        return E_SECURITY_EXCEPTION;
    }
    return NOERROR;
}

ECode CLocationManagerService::AddTestProvider(
    /* [in] */ const String& name,
    /* [in] */ IProviderProperties* properties)
{
    FAIL_RETURN(CheckMockPermissionsSafe());

    if (ILocationManager::PASSIVE_PROVIDER.Equals(name)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Cannot mock the passive location provider");
    }

    Int64 identity = Binder::ClearCallingIdentity();
    {
        AutoLock lock(mLock);
        // remove the real provider if we are replacing GPS or network provider
        if (ILocationManager::GPS_PROVIDER.Equals(name)
                || ILocationManager::NETWORK_PROVIDER.Equals(name)
                || ILocationManager::FUSED_PROVIDER.Equals(name)) {
            AutoPtr<IInterface> _p;
            mProvidersByName->Get(CoreUtils::Convert(name), (IInterface**)&_p);
            AutoPtr<ILocationProviderInterface> p = ILocationProviderInterface::Probe(_p);
            if(p != NULL) {
                RemoveProviderLocked(p);
            }
        }
        mGeoFencerEnabled = FALSE;
        AddTestProviderLocked(name, properties);
        UpdateProvidersLocked();
    }
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

void CLocationManagerService::AddTestProviderLocked(
    /* [in] */ String name,
    /* [in] */ IProviderProperties* properties)
{
    AutoPtr<IInterface> p;
    mProvidersByName->Get(CoreUtils::Convert(name), (IInterface**)&p);
    if (p != NULL) {
        // throw new IllegalArgumentException("Provider \"" + name + "\" already exists");
        return;
    }
    AutoPtr<MockProvider> provider = new MockProvider(name, this, properties);
    AddProviderLocked(provider);
    mMockProviders->Put(CoreUtils::Convert(name), ILocationProviderInterface::Probe(provider));
    mLastLocation->Put(CoreUtils::Convert(name), NULL);
    mLastLocationCoarseInterval->Put(CoreUtils::Convert(name), NULL);
}

ECode CLocationManagerService::RemoveTestProvider(
    /* [in] */ const String& provider)
{
    FAIL_RETURN(CheckMockPermissionsSafe());
    AutoLock lock(mLock);

    // These methods can't be called after removing the test provider, so first make sure
    // we don't leave anything dangling.
    ClearTestProviderEnabled(provider);
    ClearTestProviderLocation(provider);
    ClearTestProviderStatus(provider);

    AutoPtr<IInterface> _mockProvider;
    mMockProviders->Remove(CoreUtils::Convert(provider), (IInterface**)&_mockProvider);
    AutoPtr<MockProvider> mockProvider = (MockProvider*)ILocationProviderInterface::Probe(_mockProvider);
    if (mockProvider == NULL) {
//        throw new IllegalArgumentException("Provider \"" + provider + "\" unknown");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int64 identity = Binder::ClearCallingIdentity();
    AutoPtr<IInterface> p;
    mProvidersByName->Get(CoreUtils::Convert(provider), (IInterface**)&p);
    RemoveProviderLocked(ILocationProviderInterface::Probe(p));

    if (mGeoFencer != NULL) {
        mGeoFencerEnabled = TRUE;
    }
    // reinstate real provider if available
    AutoPtr<IInterface> _realProvider;
    mRealProviders->Get(CoreUtils::Convert(provider), (IInterface**)&_realProvider);
    AutoPtr<ILocationProviderInterface> realProvider = ILocationProviderInterface::Probe(_realProvider);
    if (realProvider != NULL) {
        AddProviderLocked(realProvider);
    }
    mLastLocation->Put(CoreUtils::Convert(provider), NULL);
    mLastLocationCoarseInterval->Put(CoreUtils::Convert(provider), NULL);
    UpdateProvidersLocked();
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CLocationManagerService::SetTestProviderLocation(
    /* [in] */ const String& provider,
    /* [in] */ ILocation* loc)
{
    FAIL_RETURN(CheckMockPermissionsSafe());
    AutoLock lock(mLock);
    AutoPtr<IInterface> _mockProvider;
    mMockProviders->Get(CoreUtils::Convert(provider), (IInterface**)&_mockProvider);
    AutoPtr<MockProvider> mockProvider = (MockProvider*)ILocationProviderInterface::Probe(_mockProvider);
    if (mockProvider == NULL) {
//        throw new IllegalArgumentException("Provider \"" + provider + "\" unknown");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // clear calling identity so INSTALL_LOCATION_PROVIDER permission is not required
    Int64 identity = Binder::ClearCallingIdentity();
    mockProvider->SetLocation(loc);
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CLocationManagerService::ClearTestProviderLocation(
    /* [in] */ const String& provider)
{
    FAIL_RETURN(CheckMockPermissionsSafe());
    AutoLock lock(mLock);
    AutoPtr<IInterface> _mockProvider;
    mMockProviders->Get(CoreUtils::Convert(provider), (IInterface**)&_mockProvider);
    AutoPtr<MockProvider> mockProvider = (MockProvider*)ILocationProviderInterface::Probe(_mockProvider);
    if (mockProvider == NULL) {
//        throw new IllegalArgumentException("Provider \"" + provider + "\" unknown");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mockProvider->ClearLocation();
    return NOERROR;
}

ECode CLocationManagerService::SetTestProviderEnabled(
    /* [in] */ const String& provider,
    /* [in] */ Boolean enabled)
{
    FAIL_RETURN(CheckMockPermissionsSafe());
    AutoLock lock(mLock);
    AutoPtr<IInterface> _mockProvider;
    mMockProviders->Get(CoreUtils::Convert(provider), (IInterface**)&_mockProvider);
    AutoPtr<MockProvider> mockProvider = (MockProvider*)ILocationProviderInterface::Probe(_mockProvider);
    if (mockProvider == NULL) {
//        throw new IllegalArgumentException("Provider \"" + provider + "\" unknown");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 identity = Binder::ClearCallingIdentity();
    if (enabled) {
        mockProvider->Enable();
        mEnabledProviders->Add(CoreUtils::Convert(provider));
        mDisabledProviders->Remove(CoreUtils::Convert(provider));
    }
    else {
        mockProvider->Disable();
        mEnabledProviders->Remove(CoreUtils::Convert(provider));
        mDisabledProviders->Add(CoreUtils::Convert(provider));
    }
    UpdateProvidersLocked();
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CLocationManagerService::ClearTestProviderEnabled(
    /* [in] */ const String& provider)
{
    FAIL_RETURN(CheckMockPermissionsSafe());
    AutoLock lock(mLock);
    AutoPtr<IInterface> _mockProvider;
    mMockProviders->Get(CoreUtils::Convert(provider), (IInterface**)&_mockProvider);
    AutoPtr<MockProvider> mockProvider = (MockProvider*)ILocationProviderInterface::Probe(_mockProvider);
    if (mockProvider == NULL) {
//        throw new IllegalArgumentException("Provider \"" + provider + "\" unknown");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // clear calling identity so INSTALL_LOCATION_PROVIDER permission is not required
    Int64 identity = Binder::ClearCallingIdentity();
    mEnabledProviders->Remove(CoreUtils::Convert(provider));
    mDisabledProviders->Remove(CoreUtils::Convert(provider));
    UpdateProvidersLocked();
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CLocationManagerService::SetTestProviderStatus(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 updateTime)
{
    FAIL_RETURN(CheckMockPermissionsSafe());
    AutoLock lock(mLock);
    AutoPtr<IInterface> _mockProvider;
    mMockProviders->Get(CoreUtils::Convert(provider), (IInterface**)&_mockProvider);
    AutoPtr<MockProvider> mockProvider = (MockProvider*)ILocationProviderInterface::Probe(_mockProvider);
    if (mockProvider == NULL) {
//        throw new IllegalArgumentException("Provider \"" + provider + "\" unknown");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mockProvider->SetStatus(status, extras, updateTime);
    return NOERROR;
}

ECode CLocationManagerService::ClearTestProviderStatus(
    /* [in] */ const String& provider)
{
    FAIL_RETURN(CheckMockPermissionsSafe());
    AutoLock lock(mLock);
    AutoPtr<IInterface> _mockProvider;
    mMockProviders->Get(CoreUtils::Convert(provider), (IInterface**)&_mockProvider);
    AutoPtr<MockProvider> mockProvider = (MockProvider*)ILocationProviderInterface::Probe(_mockProvider);
    if (mockProvider == NULL) {
//        throw new IllegalArgumentException("Provider \"" + provider + "\" unknown");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mockProvider->ClearStatus();
    return NOERROR;
}

void CLocationManagerService::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    Int32 mode = 0;
    mContext->CheckCallingOrSelfPermission(Manifest::permission::DUMP, &mode);
    if (mode != IPackageManager::PERMISSION_GRANTED) {
        // pw->Println("Permission Denial: can't dump LocationManagerService from from pid=%d, uid=%d",
        //         Binder::GetCallingPid(),
        //         Binder::GetCallingUid());
        return;
    }

    // {    AutoLock syncLock(mLock);
    //     pw->Println("Current Location Manager state:");
    //     pw->Println("  Location Listeners:");
    //     for (Receiver receiver : mReceivers.values()) {
    //         pw->Println("    " + receiver);
    //     }
    //     pw.println("  Active Records by Provider:");
    //     for (Map.Entry<String, ArrayList<UpdateRecord>> entry : mRecordsByProvider.entrySet()) {
    //         pw.println("    " + entry.getKey() + ":");
    //         for (UpdateRecord record : entry.getValue()) {
    //             pw.println("      " + record);
    //         }
    //     }
    //     pw.println("  Historical Records by Provider:");
    //     for (Map.Entry<PackageProviderKey, PackageStatistics> entry
    //             : mRequestStatistics.statistics.entrySet()) {
    //         PackageProviderKey key = entry.getKey();
    //         PackageStatistics stats = entry.getValue();
    //         pw.println("    " + key.packageName + ": " + key.providerName + ": " + stats);
    //     }
    //     pw.println("  Last Known Locations:");
    //     for (Map.Entry<String, Location> entry : mLastLocation.entrySet()) {
    //         String provider = entry.getKey();
    //         Location location = entry.getValue();
    //         pw.println("    " + provider + ": " + location);
    //     }

    //     pw.println("  Last Known Locations Coarse Intervals:");
    //     for (Map.Entry<String, Location> entry : mLastLocationCoarseInterval.entrySet()) {
    //         String provider = entry.getKey();
    //         Location location = entry.getValue();
    //         pw.println("    " + provider + ": " + location);
    //     }

    //     mGeofenceManager.dump(pw);

    //     if (mEnabledProviders.size() > 0) {
    //         pw.println("  Enabled Providers:");
    //         for (String i : mEnabledProviders) {
    //             pw.println("    " + i);
    //         }

    //     }
    //     if (mDisabledProviders.size() > 0) {
    //         pw.println("  Disabled Providers:");
    //         for (String i : mDisabledProviders) {
    //             pw.println("    " + i);
    //         }
    //     }
    //     pw.append("  ");
    //     mBlacklist.dump(pw);
    //     if (mMockProviders.size() > 0) {
    //         pw.println("  Mock Providers:");
    //         for (Map.Entry<String, MockProvider> i : mMockProviders.entrySet()) {
    //             i.getValue().dump(pw, "      ");
    //         }
    //     }

    //     pw.append("  fudger: ");
    //     mLocationFudger.dump(fd, pw,  args);

    //     if (args.length > 0 && "short".equals(args[0])) {
    //         return;
    //     }
    //     for (LocationProviderInterface provider: mProviders) {
    //         pw.print(provider.getName() + " Internal State");
    //         if (provider instanceof LocationProviderProxy) {
    //             LocationProviderProxy proxy = (LocationProviderProxy) provider;
    //             pw.print(" (" + proxy.getConnectedPackageName() + ")");
    //         }
    //         pw.println(":");
    //         provider.dump(fd, pw, args);
    //     }
    // }
}

} // Server
} // Droid
} // Elastos
