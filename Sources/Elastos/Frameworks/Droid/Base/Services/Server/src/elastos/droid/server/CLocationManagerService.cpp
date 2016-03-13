
#include "CLocationManagerService.h"
//#include "location/GpsLocationProvider.h"
#include "location/PassiveProvider.h"
#include "location/LocationProviderInterface.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Looper.h"
#include "ServiceWatcher.h"
#include "elastos/droid/R.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::Math;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Droid::R;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Location::CLocationRequest;
using Elastos::Droid::Location::CProviderRequest;
using Elastos::Droid::Location::ILocationProviderHelper;
using Elastos::Droid::Location::CLocationProviderHelper;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::CParcelableObjectContainer;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const Boolean CLocationManagerService::D = FALSE;
const String CLocationManagerService::TAG("LocationManagerService");
const String CLocationManagerService::WAKELOCK_KEY = TAG;
const String CLocationManagerService::THREAD_NAME = TAG;
const Int32 CLocationManagerService::RESOLUTION_LEVEL_NONE = 0;
const Int32 CLocationManagerService::RESOLUTION_LEVEL_COARSE = 1;
const Int32 CLocationManagerService::RESOLUTION_LEVEL_FINE = 2;
// const String CLocationManagerService::ACCESS_MOCK_LOCATION =
//         String("elastos.Manifest.permission.ACCESS_MOCK_LOCATION");
// const String CLocationManagerService::ACCESS_LOCATION_EXTRA_COMMANDS =
//         String("elastos.Manifest.permission.ACCESS_LOCATION_EXTRA_COMMANDS");
// const String CLocationManagerService::INSTALL_LOCATION_PROVIDER =
//         String("elastos.Manifest.permission.INSTALL_LOCATION_PROVIDER");
// const String CLocationManagerService::NETWORK_LOCATION_SERVICE_ACTION =
//         String("elastos.location.service.v2.NetworkLocationProvider");
// const String CLocationManagerService::FUSED_LOCATION_SERVICE_ACTION =
//         String("elastos.location.service.FusedLocationProvider");
const String CLocationManagerService::ACCESS_MOCK_LOCATION =
        String("android.permission.ACCESS_MOCK_LOCATION");
const String CLocationManagerService::ACCESS_LOCATION_EXTRA_COMMANDS =
        String("android.permission.ACCESS_LOCATION_EXTRA_COMMANDS");
const String CLocationManagerService::INSTALL_LOCATION_PROVIDER =
        String("android.permission.INSTALL_LOCATION_PROVIDER");
const String CLocationManagerService::NETWORK_LOCATION_SERVICE_ACTION =
        String("com.android.location.service.v2.NetworkLocationProvider");
const String CLocationManagerService::FUSED_LOCATION_SERVICE_ACTION =
        String("com.android.location.service.FusedLocationProvider");
const Int32 CLocationManagerService::MSG_LOCATION_CHANGED = 1;
const Int32 CLocationManagerService::MAX_PROVIDER_SCHEDULING_JITTER_MS = 100;

static AutoPtr<ILocationRequest> InitDefaultLocationRequest() {
    AutoPtr<ILocationRequest> locationRequest;
    CLocationRequest::New((ILocationRequest**)&locationRequest);
    return locationRequest;
}

const AutoPtr<ILocationRequest> CLocationManagerService::DEFAULT_LOCATION_REQUEST =
        InitDefaultLocationRequest();


/*******************************************************************************************/
/*                                                                                         */
/********        CLocationManagerService::LocationWorkerHandler Start         *************/
/*                                                                                         */
/*******************************************************************************************/
ECode CLocationManagerService::LocationWorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
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

/*******************************************************************************************/
/*                                                                                         */
/********        CLocationManagerService::LocationPackageMonitor Start         *************/
/*                                                                                         */
/*******************************************************************************************/

CLocationManagerService::LocationPackageMonitor::LocationPackageMonitor(
    /* [in] */ CLocationManagerService* host) : mHost(host)
{}

ECode CLocationManagerService::LocationPackageMonitor::OnPackageDisappeared(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 reason)
{
    AutoLock lock(mLock);
    AutoPtr< List<AutoPtr<Receiver> > > deadReceivers;
    InterfaceReceiverIterator it = mHost->mReceivers.Begin();
    for(; it != mHost->mReceivers.End(); ++it) {
        AutoPtr<Receiver> receiver = it->mSecond;
        if(receiver->mPackageName == packageName) {
            if (deadReceivers == NULL) {
                deadReceivers = new List<AutoPtr<Receiver> >();
            }
            deadReceivers->PushBack(receiver);
        }
    }

    // perform removal outside of mReceivers loop
    if (deadReceivers != NULL) {
        List<AutoPtr<Receiver> >::Iterator listIt = deadReceivers->Begin();
        AutoPtr<Receiver> receiver = *listIt;
        for(; listIt != deadReceivers->End(); ++listIt) {
            mHost->RemoveUpdatesLocked(receiver);
        }
    }

    return NOERROR;
}

/*******************************************************************************************/
/*                                                                                         */
/********             CLocationManagerService::UpdateRecord Start              *************/
/*                                                                                         */
/*******************************************************************************************/

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
    AutoPtr< UpdateRecordList > records;
    HashMap<String, AutoPtr< UpdateRecordList > >::Iterator it =
            mHost->mRecordsByProvider.Find(provider);
    if (it != mHost->mRecordsByProvider.End()) {
        records = it->mSecond;
    }
    if (records == NULL) {
        records = new List<AutoPtr<UpdateRecord> >();
        mHost->mRecordsByProvider[provider] = records;
    }
    List<AutoPtr<UpdateRecord> >::Iterator recordIt;
    for (recordIt = records->Begin(); recordIt != records->End(); ++recordIt) {
        if ((*recordIt).Get() == this) break;
    }
    if (recordIt == records->End()) {
        records->PushBack(this);
    }
}

void CLocationManagerService::UpdateRecord::DisposeLocked(
    /* [in] */ Boolean removeReceiver)
{
    AutoPtr< UpdateRecordList > globalRecords;
    HashMap<String, AutoPtr< UpdateRecordList > >::Iterator it =
            mHost->mRecordsByProvider.Find(mProvider);
    if (it != mHost->mRecordsByProvider.End()) {
        globalRecords = it->mSecond;
    }
    if (globalRecords != NULL) {
        AutoPtr<UpdateRecord> temp(this);
        globalRecords->Remove(temp);
    }

    if (!removeReceiver) return;  // the caller will handle the rest

    // remove from Receiver#mUpdateRecords
    HashMap<String, AutoPtr<UpdateRecord> > receiverRecords = mReceiver->mUpdateRecords;
    if (receiverRecords.Begin() != receiverRecords.End()) {
        receiverRecords.Erase(mProvider);

        // and also remove the Receiver if it has no more update records
        if (removeReceiver && receiverRecords.Begin() == receiverRecords.End()) {
            mHost->RemoveUpdatesLocked(mReceiver);
        }
    }
}

String CLocationManagerService::UpdateRecord::ToString()
{
    StringBuilder s;
    s.AppendString(String("UpdateRecord["));
    s.AppendString(mProvider);
    s.AppendChar(' ');
    s.AppendString(mReceiver->mPackageName);
    s.AppendChar('(');
    s.AppendInt32(mReceiver->mUid);
    s.AppendChar(')');
    s.AppendChar(' ');
    s.AppendObject((IInterface*)(mRequest.Get()));
    s.AppendChar(']');
    String result;
    s.ToString(&result);
    return result;
}

/*******************************************************************************************/
/*                                                                                         */
/********        CLocationManagerService::LocationContentObserver Start        *************/
/*                                                                                         */
/*******************************************************************************************/

CLocationManagerService::LocationContentObserver::LocationContentObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ CLocationManagerService* host)
    : ContentObserver(handler)
    , mHost(host)
{}

PInterface CLocationManagerService::LocationContentObserver::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IContentObserver*)this;
    }
    else if (riid == EIID_IContentObserver) {
        return (IContentObserver*)this;
    }

    return NULL;
}

UInt32 CLocationManagerService::LocationContentObserver::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CLocationManagerService::LocationContentObserver::Release()
{
    return ElRefBase::Release();
}

ECode CLocationManagerService::LocationContentObserver::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [in] */ InterfaceID* pIID)
{
    return E_NOT_IMPLEMENTED;
}

ECode CLocationManagerService::LocationContentObserver::OnChange(
    /* [in] */ Boolean selfUpdate)
{
    AutoLock lock(mHost->mLock);
    mHost->UpdateProvidersLocked();
    return NOERROR;
}

/*******************************************************************************************/
/*                                                                                         */
/*********              CLocationManagerService::Receiver Start               **************/
/*                                                                                         */
/*******************************************************************************************/

CLocationManagerService::Receiver::Receiver(
    /* [in] */ IILocationListener* listener,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
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
}

PInterface CLocationManagerService::Receiver::Probe(
    /* [in]  */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IProxyDeathRecipient*)this;
    } else if(riid == EIID_IProxyDeathRecipient) {
        return (IProxyDeathRecipient*)this;
    } else if (riid == EIID_IPendingIntentOnFinished) {
        return (IPendingIntentOnFinished*)this;
    }
    return NULL;
}

UInt32 CLocationManagerService::Receiver::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CLocationManagerService::Receiver::Release()
{
    return ElRefBase::Release();
}

ECode CLocationManagerService::Receiver::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [out] */ InterfaceID* pIID)
{
    return E_NOT_IMPLEMENTED;
}

String CLocationManagerService::Receiver::ToString()
{
    StringBuilder s;
    s.Append("Reciever[");
    // s.Append(Integer.toHexString(System.identityHashCode(this)));
    if (mListener != NULL) {
        s.Append(" listener");
    }
    else {
        s.Append(" intent");
    }
    HashMap<String, AutoPtr<UpdateRecord> >::Iterator it;
    for (it = mUpdateRecords.Begin(); it != mUpdateRecords.End(); ++it) {
        s.Append(" ");
        s.AppendString(it->mSecond->ToString());
    }
    s.Append("]");
    String result;
    s.ToString(&result);
    return result;
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
    if (mPendingBroadcasts > 0) {
        mHost->DecrementPendingBroadcasts();
        mPendingBroadcasts = 0;
    }
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
        mHost->IncrementPendingBroadcasts();
    }
}

void CLocationManagerService::Receiver::DecrementPendingBroadcastsLocked()
{
    if (--mPendingBroadcasts == 0) {
        mHost->DecrementPendingBroadcasts();
    }
}

/*******************************************************************************************/
/*                                                                                         */
/*********        CLocationManagerService::MyBroadcastReceiver::Start         **************/
/*                                                                                         */
/*******************************************************************************************/
CLocationManagerService::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ CLocationManagerService* host)
{
    mHost = host;
}

ECode CLocationManagerService::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_USER_SWITCHED.Equals(action)) {
        Int32 userId;
        intent->GetInt32Extra(IIntent::EXTRA_USER_HANDLE, 0, &userId);
        mHost->SwitchUser(userId);
    }
    return NOERROR;
}
/*******************************************************************************************/
/*                                                                                         */
/*********                   CLocationManagerService::Start                   **************/
/*                                                                                         */
/*******************************************************************************************/

CLocationManagerService::CLocationManagerService()
    : mLocationFudger(NULL)
    , mGeofenceManager(NULL)
    , mGeocodeProvider(NULL)
    , mBlacklist(NULL)
    , mPendingBroadcasts(0)
    , mCurrentUserId(IUserHandle::USER_OWNER)
{
    mPackageMonitor = new LocationPackageMonitor(this);
}

ECode CLocationManagerService::constructor(
    /* [in] */ IContext* ctx)
{
    mContext = ctx;
    // most startup is deferred until systemReady()
    return NOERROR;
}

CLocationManagerService::~CLocationManagerService()
{
    mRecordsByProvider.Clear();
}

ECode CLocationManagerService::SystemReady()
{
    AutoPtr<IThread> thread;
    ASSERT_SUCCEEDED(CThread::New(NULL, (IRunnable*)this, String("CLocationManagerService"),
            (IThread**)&thread));
    thread->Start();
    return NOERROR;
}

ECode CLocationManagerService::Run()
{
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    Looper::Prepare();
    mLocationHandler = new LocationWorkerHandler(this);
    Init();
    Looper::Loop();
    return NOERROR;
}

void CLocationManagerService::Init()
{
    // if (D) Log.d(TAG, "init()");

    AutoPtr<IInterface> obj;
    ASSERT_SUCCEEDED(mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&obj));
    AutoPtr<IPowerManager> pmc = IPowerManager::Probe(obj);
    if(mWakeLock) mWakeLock = NULL;
    pmc->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK,
            WAKELOCK_KEY, (IPowerManagerWakeLock**)&mWakeLock);
    if(mPackageManager) mPackageManager = NULL;
    mContext->GetPackageManager((IPackageManager**)&mPackageManager);

    mBlacklist = new LocationBlacklist(mContext, mLocationHandler);
    mBlacklist->Init();
    mLocationFudger = new LocationFudger(mContext, mLocationHandler);

    {
        AutoLock lock(mLock);
        LoadProvidersLocked();
    }

    mGeofenceManager = new GeofenceManager(mContext, mBlacklist);

    // listen for settings changes
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> settingsCursor;
    AutoPtr<LocationContentObserver> lcObserver = new LocationContentObserver(mLocationHandler, this);
    AutoPtr<IUri> uri;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetUriFor(ISettingsSecure::LOCATION_PROVIDERS_ALLOWED, (IUri**)&uri);
    resolver->RegisterContentObserver(uri, TRUE, lcObserver.Get(), IUserHandle::USER_ALL);
    AutoPtr<ILooper> myLooper = Looper::GetMyLooper();
    mPackageMonitor->Register(mContext, myLooper, TRUE);

    // listen for user change
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IIntent::ACTION_USER_SWITCHED);
    AutoPtr<MyBroadcastReceiver> mbr = new MyBroadcastReceiver(this);
    AutoPtr<IIntent> rst;
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> ALL;
    helper->GetALL((IUserHandle**)&ALL);

    mContext->RegisterReceiverAsUser(mbr.Get(), ALL, intentFilter, String(NULL), NULL, (IIntent**)&rst);

    UpdateProvidersLocked();
}

ECode CLocationManagerService::EnsureFallbackFusedProviderPresentLocked(
    /* [in] */ List<String>* pkgs)
{
    AutoPtr<IPackageManager> pm;
    ASSERT_SUCCEEDED(mContext->GetPackageManager((IPackageManager**)&pm));
    String systemPackageName;
    mContext->GetPackageName(&systemPackageName);
    // TODO: Ignore signature check
    // List<HashSet<AutoPtr<ISignature> > > sigSets =
    //         ServiceWatcher::GetSignatureSets(mContext, pkgs);

    AutoPtr<IObjectContainer> rInfos;
    AutoPtr<IIntent> intent;
    CIntent::New(String(FUSED_LOCATION_SERVICE_ACTION), (IIntent**)&intent);
    pm->QueryIntentServicesAsUser(intent, IPackageManager::GET_META_DATA,
            mCurrentUserId, (IObjectContainer**)&rInfos);
    AutoPtr<IObjectEnumerator> enumerator;
    rInfos->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext = FALSE;
    while (enumerator->MoveNext(&hasNext), hasNext) {
        AutoPtr<IResolveInfo> rInfo;
        enumerator->Current((IInterface**)&rInfo);

        AutoPtr<IServiceInfo> serviceInfo;
        rInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);
        String packageName;
        serviceInfo->GetPackageName(&packageName);

        // Check that the signature is in the list of supported sigs. If it's not in
        // this list the standard provider binding logic won't bind to it.
        // try {
        AutoPtr<IPackageInfo> pInfo;
        if (FAILED(pm->GetPackageInfo(packageName, IPackageManager::GET_SIGNATURES,
                (IPackageInfo**)&pInfo))) {
            // Log.e(TAG, "missing package: " + packageName);
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

        // } catch (NameNotFoundException e) {
        //     Log.e(TAG, "missing package: " + packageName);
        //     continue;
        // }

        // Get the version info
        AutoPtr<IBundle> metaData;
        serviceInfo->GetMetaData((IBundle**)&metaData);
        if (metaData == NULL) {
            // Log.w(TAG, "Found fused provider without metadata: " + packageName);
            continue;
        }

        Int32 version;
        metaData->GetInt32(
                ServiceWatcher::EXTRA_SERVICE_VERSION, -1, &version);
        if (version == 0) {
            // This should be the fallback fused location provider.

            // Make sure it's in the system partition.
            AutoPtr<IApplicationInfo> appInfo;
            serviceInfo->GetApplicationInfo((IApplicationInfo**)&appInfo);
            Int32 flags;
            appInfo->GetFlags(&flags);
            if ((flags & IApplicationInfo::FLAG_SYSTEM) == 0) {
                // if (D) Log.d(TAG, "Fallback candidate not in /system: " + packageName);
                continue;
            }

            // Check that the fallback is signed the same as the OS
            // as a proxy for coreApp="true"
            // TODO: Ignore signature check
            // Int32 result;
            // pm->CheckSignatures(systemPackageName, packageName, &result);
            // if (result != IPackageManager::SIGNATURE_MATCH) {
            //     // if (D) Log.d(TAG, "Fallback candidate not signed the same as system: "
            //     //         + packageName);
            //     continue;
            // }

            // Found a valid fallback.
            // if (D) Log.d(TAG, "Found fallback provider: " + packageName);
            return NOERROR;
        }
        else {
            // if (D) Log.d(TAG, "Fallback candidate not version 0: " + packageName);
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
    mEnabledProviders.Insert(name);
    mPassiveProvider = passiveProvider;

    PFL_EX("DEBUG")
//    if (GpsLocationProvider::IsSupported()) {
//        // Create a gps location provider
//
//        AutoPtr<GpsLocationProvider> gpsProvider;// = new GpsLocationProvider(mContext, this);
//        mGpsStatusProvider = gpsProvider->GetGpsStatusProvider();
//        mNetInitiatedListener = gpsProvider->GetNetInitiatedListener();
//        AddProviderLocked(gpsProvider);
//        mRealProviders[ILocationManager::GPS_PROVIDER] = gpsProvider;
//    }

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
    AutoPtr< List<String> > providerPackageNames = new List<String>();
    AutoPtr<ArrayOf<String> > pkgs;
    resources->GetStringArray(
            R::array::config_locationProviderPackageNames, (ArrayOf<String>**)&pkgs);
    // if (D) Log.d(TAG, "certificates for location providers pulled from: " +
    //         Arrays.toString(pkgs));
    if (pkgs != NULL) {
        for (Int32 i = 0; i< pkgs->GetLength(); ++i) {
            providerPackageNames->PushBack((*pkgs)[i]);
        }
    }

    EnsureFallbackFusedProviderPresentLocked(providerPackageNames);

    // bind to network provider
    AutoPtr<LocationProviderProxy> networkProvider = LocationProviderProxy::CreateAndBind(
            mContext,
            ILocationManager::NETWORK_PROVIDER,
            String(NETWORK_LOCATION_SERVICE_ACTION),
            providerPackageNames, mLocationHandler, mCurrentUserId);
    if (networkProvider != NULL) {
        mRealProviders[ILocationManager::NETWORK_PROVIDER] = networkProvider;
        mProxyProviders.PushBack(networkProvider);
        AddProviderLocked((ILocationProviderInterface*)networkProvider);
    }
    else {
        // Slog.w(TAG,  "no network location provider found");
        Slogger::W(TAG, "no network location provider found");
    }

    // bind to fused provider
    AutoPtr<LocationProviderProxy> fusedLocationProvider
            = LocationProviderProxy::CreateAndBind(
                    mContext,
                    ILocationManager::FUSED_PROVIDER,
                    String(FUSED_LOCATION_SERVICE_ACTION),
                    providerPackageNames, mLocationHandler, mCurrentUserId);
    if (fusedLocationProvider != NULL) {
        AddProviderLocked(fusedLocationProvider);
        mProxyProviders.PushBack(fusedLocationProvider);
        String providerName;
        fusedLocationProvider->GetName(&providerName);
        mEnabledProviders.Insert(providerName);
        mRealProviders[ILocationManager::FUSED_PROVIDER] = fusedLocationProvider;
    }
    else {
        Slogger::E(TAG, "no fused location provider found Location service needs a fused location provider");
    }

    // bind to geocoder provider
    mGeocodeProvider = GeocoderProxy::CreateAndBind(mContext, providerPackageNames,
            mCurrentUserId);
    if (mGeocodeProvider == NULL) {
        Slogger::E(TAG,  "no geocoder provider found");
    }
}

void CLocationManagerService::SwitchUser(
    /* [in] */ Int32 userId)
{
    mBlacklist->SwitchUser(userId);
    AutoLock lock(mLock);
    mLastLocation.Clear();
    List<AutoPtr<ILocationProviderInterface> >::Iterator it;
    for (it = mProviders.Begin(); it != mProviders.End(); ++it) {
        AutoPtr<ILocationProviderInterface> p = *it;
        String name;
        p->GetName(&name);
        UpdateProviderListenersLocked(name, FALSE, mCurrentUserId);
        p->SwitchUser(userId);
    }
    mCurrentUserId = userId;
    UpdateProvidersLocked();
}

ECode CLocationManagerService::LocationCallbackFinished(
    /* [in] */ IILocationListener* listener)
{
    //Do not use getReceiver here as that will add the ILocationListener to
    //the receiver list if it is not found.  If it is not found then the
    //LocationListener was removed when it had a pending broadcast and should
    //not be added back.
    AutoPtr<Receiver> receiver;
    AutoPtr<IInterface> temp = listener;
    InterfaceReceiverIterator it = mReceivers.Find(temp);
    if (it != mReceivers.End()) {
        receiver = it->mSecond;
    }
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
    mProviders.PushBack(provider);
    String name;
    provider->GetName(&name);
    mProvidersByName[name] = provider;
}

void CLocationManagerService::RemoveProviderLocked(
    /* [in] */ ILocationProviderInterface* provider)
{
    provider->Disable();
    List<AutoPtr<ILocationProviderInterface> >::Iterator it = mProviders.Begin();
    for(; it != mProviders.End(); ++it) {
        if (provider == *it) {
            break;
        }
    }
    if(it != mProviders.End())
    {
        mProviders.Erase(it);
    }
    String name;
    provider->GetName(&name);
    mProvidersByName.Erase(name);
}

Boolean CLocationManagerService::IsAllowedBySettingsLocked(
    /* [in] */ const String& provider,
    /* [in] */ Int32 userId)
{
    if (userId != mCurrentUserId) {
        return FALSE;
    }
    if (mEnabledProviders.Find(provider) != mEnabledProviders.End()) {
        return TRUE;
    }
    if (mDisabledProviders.Find(provider) != mDisabledProviders.End()) {
        return FALSE;
    }
    // Use system settings
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);

    Boolean rst;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->IsLocationProviderEnabled(resolver, provider, &rst);
    return rst;
}

String CLocationManagerService::GetResolutionPermission(
    /* [in] */ Int32 resolutionLevel)
{
    switch (resolutionLevel) {
        case RESOLUTION_LEVEL_FINE:
            return Elastos::Droid::Manifest::permission::ACCESS_FINE_LOCATION;
        case RESOLUTION_LEVEL_COARSE:
            return Elastos::Droid::Manifest::permission::ACCESS_COARSE_LOCATION;
        default:
            return String(NULL);
    }
}

Int32 CLocationManagerService::GetAllowedResolutionLevel(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid)
{
    Int32 result;
    FAIL_RETURN(mContext->CheckPermission(Elastos::Droid::Manifest::permission::ACCESS_FINE_LOCATION,
            pid, uid, &result));
    if (result == IPackageManager::PERMISSION_GRANTED) {
        return RESOLUTION_LEVEL_FINE;
    }
    else {
        FAIL_RETURN(mContext->CheckPermission(Elastos::Droid::Manifest::permission::ACCESS_COARSE_LOCATION,
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
        HashMap<String, AutoPtr<MockProvider> >::Iterator it
                = mMockProviders.Find(provider);
        AutoPtr<ILocationProviderInterface> lp;
        if (it != mMockProviders.End()) {
            lp = it->mSecond;
        }
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

ECode CLocationManagerService::GetAllProviders(
    /* [out] */ IObjectContainer** allProviders)
{
    VALIDATE_NOT_NULL(allProviders);

    AutoLock lock(mLock);

    CParcelableObjectContainer::New(allProviders);
    List<AutoPtr<ILocationProviderInterface> >::Iterator it;
    for (it = mProviders.Begin(); it != mProviders.End(); ++it) {
        String name;
        (*it)->GetName(&name);
        if (ILocationManager::FUSED_PROVIDER.Equals(name)) {
            continue;
        }
        AutoPtr<ICharSequence> wrapper;
        CString::New(name, (ICharSequence**)&wrapper);
        (*allProviders)->Add(wrapper.Get());
    }

    // if (D) Log.d(TAG, "getAllProviders()=" + out);
    return NOERROR;
}

ECode CLocationManagerService::GetProviders(
    /* [in] */ ICriteria* criteria,
    /* [in] */ Boolean enabledOnly,
    /* [out] */ IObjectContainer** providers)
{
    VALIDATE_NOT_NULL(providers);

    Int32 allowedResolutionLevel = GetCallerAllowedResolutionLevel();
    AutoPtr<IUserHandleHelper> h;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&h);
    Int32 callingUserId;
    h->GetCallingUserId(&callingUserId);
    Int64 identity = Binder::ClearCallingIdentity();

    // try {
    AutoLock lock(mLock);

    CParcelableObjectContainer::New(providers);
    List<AutoPtr<ILocationProviderInterface> >::Iterator it;
    for (it = mProviders.Begin(); it != mProviders.End(); ++it) {
        AutoPtr<ILocationProviderInterface> provider = *it;
        String name;
        provider->GetName(&name);
        if (ILocationManager::FUSED_PROVIDER.Equals(name)) {
            continue;
        }
        if (allowedResolutionLevel >= GetMinimumResolutionLevelForProviderUse(name)) {
            if (enabledOnly && !IsAllowedBySettingsLocked(name, callingUserId)) {
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
            (*providers)->Add(wrapper);
        }
    }
    // } finally {
    //     Binder.restoreCallingIdentity(identity);
    // }
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CLocationManagerService::GetBestProvider(
    /* [in] */ ICriteria* criteria,
    /* [in] */ Boolean enabledOnly,
    /* [out] */ String* provider)
{
    VALIDATE_NOT_NULL(provider);

    *provider = String(NULL);

    AutoPtr<IObjectContainer> providers;
    GetProviders(criteria, enabledOnly, (IObjectContainer**)&providers);
    AutoPtr<IObjectEnumerator> enumerator;
    providers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext = FALSE;
    enumerator->MoveNext(&hasNext);
    if (hasNext) {
        *provider = PickBest(providers);
        // if (D) Log.d(TAG, "getBestProvider(" + criteria + ", " + enabledOnly + ")=" + result);
        return NOERROR;
    }

    providers = NULL;
    GetProviders(NULL, enabledOnly, (IObjectContainer**)&providers);
    enumerator = NULL;
    providers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    enumerator->MoveNext(&hasNext);
    if (hasNext) {
        *provider = PickBest(providers);
        // if (D) Log.d(TAG, "getBestProvider(" + criteria + ", " + enabledOnly + ")=" + result);
        return NOERROR;
    }

    // if (D) Log.d(TAG, "getBestProvider(" + criteria + ", " + enabledOnly + ")=" + result);
    return NOERROR;
}

String CLocationManagerService::PickBest(
    /* [in] */ IObjectContainer* providers)
{
    AutoPtr<IObjectEnumerator> enumerator;
    providers->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
    Boolean hasNext = FALSE;
    AutoPtr<ICharSequence> cs;
    String s;
    while (enumerator->MoveNext(&hasNext), hasNext) {
        cs = NULL;
        enumerator->Current((IInterface**)&cs);
        cs->ToString(&s);
        if (s.Equals(ILocationManager::GPS_PROVIDER)) {
            return ILocationManager::GPS_PROVIDER;
        }
        else if (s.Equals(ILocationManager::NETWORK_PROVIDER)) {
            return ILocationManager::NETWORK_PROVIDER;
        }
    }
    enumerator->Reset();
    enumerator->MoveNext(&hasNext);
    cs = NULL;
    enumerator->Current((IInterface**)&cs);
    cs->ToString(&s);
    return s;
}

ECode CLocationManagerService::ProviderMeetsCriteria(
    /* [in] */ const String& provider,
    /* [in] */ ICriteria* criteria,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ILocationProviderInterface> p;
    HashMap<String, AutoPtr<ILocationProviderInterface> >::Iterator it
            = mProvidersByName.Find(provider);
    if (it != mProvidersByName.End()) {
        p = it->mSecond;
    }
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
    // if (D) Log.d(TAG, "providerMeetsCriteria(" + provider + ", " + criteria + ")=" + result);
    return NOERROR;
}

void CLocationManagerService::UpdateProvidersLocked()
{
    Boolean changesMade = FALSE;

    List<AutoPtr<ILocationProviderInterface> >::ReverseIterator rit;
    for (rit = mProviders.RBegin(); rit != mProviders.REnd(); ++rit) {
        AutoPtr<ILocationProviderInterface> p = *rit;
        Boolean isEnabled;
        p->IsEnabled(&isEnabled);
        String name;
        p->GetName(&name);
        Boolean shouldBeEnabled = IsAllowedBySettingsLocked(name, mCurrentUserId);
        if (isEnabled && !shouldBeEnabled) {
            UpdateProviderListenersLocked(name, FALSE, mCurrentUserId);
            changesMade = TRUE;
        }
        else if (!isEnabled && shouldBeEnabled) {
            UpdateProviderListenersLocked(name, TRUE, mCurrentUserId);
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
    }
}

void CLocationManagerService::UpdateProviderListenersLocked(
    /* [in] */ const String& provider,
    /* [in] */ Boolean enabled,
    /* [in] */ Int32 userId)
{
    Int32 listeners = 0;

    AutoPtr<ILocationProviderInterface> p;
    HashMap<String, AutoPtr<ILocationProviderInterface> >::Iterator it =
            mProvidersByName.Find(provider);
    if (it != mProvidersByName.End()) {
        p = it->mSecond;
    }
    if (p == NULL) {
        return;
    }

    AutoPtr< List<AutoPtr<Receiver> > > deadReceivers;

    AutoPtr< UpdateRecordList > records;
    HashMap<String, AutoPtr< UpdateRecordList > >::Iterator hmIt =
            mRecordsByProvider.Find(provider);
    if (hmIt != mRecordsByProvider.End()) {
        records = hmIt->mSecond;
    }
    if (records != NULL) {
        List<AutoPtr<UpdateRecord> >::Iterator lit;
        for (lit = records->Begin(); lit != records->End(); ++lit) {
            AutoPtr<UpdateRecord> record = *lit;
            AutoPtr<IUserHandleHelper> helper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
            Int32 userHandleId;
            helper->GetUserId(record->mReceiver->mUid, &userHandleId);
            if (userHandleId == userId) {
                // Sends a notification message to the receiver
                if (!record->mReceiver->CallProviderEnabledLocked(provider, enabled)) {
                    if (deadReceivers == NULL) {
                        deadReceivers = new List<AutoPtr<Receiver> >();
                    }
                    deadReceivers->PushBack(record->mReceiver);
                }
                listeners++;
            }
        }
    }

    if (deadReceivers != NULL) {
        List<AutoPtr<Receiver> >::ReverseIterator lrit;
        for (lrit = deadReceivers->RBegin(); lrit != deadReceivers->REnd(); ++lrit) {
            RemoveUpdatesLocked(*lrit);
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
    AutoPtr<ILocationProviderInterface> p;
    HashMap<String, AutoPtr<ILocationProviderInterface> >::Iterator it =
            mProvidersByName.Find(provider);
    if (it != mProvidersByName.End()) {
        p = it->mSecond;
    }
    if (p == NULL) return;

    AutoPtr< UpdateRecordList > records;
    HashMap<String, AutoPtr< UpdateRecordList > >::Iterator hmIt =
            mRecordsByProvider.Find(provider);
    if (hmIt != mRecordsByProvider.End()) {
        records = hmIt->mSecond;
    }
    AutoPtr<IWorkSource> worksource;
    CWorkSource::New((IWorkSource**)&worksource);
    AutoPtr<IProviderRequest> providerRequest;
    CProviderRequest::New((IProviderRequest**)&providerRequest);

    if (records != NULL) {
        List<AutoPtr<UpdateRecord> >::Iterator recordIt;
        for (recordIt = records->Begin(); recordIt != records->End(); ++recordIt) {
            AutoPtr<UpdateRecord> record = *recordIt;
            AutoPtr<IUserHandleHelper> helper;
            CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
            Int32 userHandleId;
            helper->GetUserId(record->mReceiver->mUid, &userHandleId);
            if (userHandleId == mCurrentUserId) {
                AutoPtr<ILocationRequest> locationRequest = record->mRequest;
                providerRequest->SetLocationRequest(locationRequest);
                Int64 locationInterval, providerInterval;
                locationRequest->GetInterval(&locationInterval);
                providerRequest->GetInterval(&providerInterval);
                if (locationInterval < providerInterval) {
                    providerRequest->SetReportLocation(TRUE);
                    providerRequest->SetInterval(locationInterval);
                }
            }
        }

        Boolean value;
        providerRequest->GetReportLocation(&value);
        if (value) {
            // calculate who to blame for power
            // This is somewhat arbitrary. We pick a threshold interval
            // that is slightly higher that the minimum interval, and
            // spread the blame across all applications with a request
            // under that threshold.
            Int64 interval;
            providerRequest->GetInterval(&interval);
            Int64 thresholdInterval = (interval + 1000) * 3 / 2;
            for (recordIt = records->Begin(); recordIt != records->End(); ++recordIt) {
                AutoPtr<UpdateRecord> record = *recordIt;
                AutoPtr<IUserHandleHelper> helper;
                CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
                Int32 userHandleId;
                helper->GetUserId(record->mReceiver->mUid, &userHandleId);
                if (userHandleId == mCurrentUserId) {
                    AutoPtr<ILocationRequest> locationRequest = record->mRequest;
                    Int64 locationInterval;
                    locationRequest->GetInterval(&locationInterval);
                    if (locationInterval <= thresholdInterval) {
                        Boolean addRst;
                        worksource->Add(record->mReceiver->mUid, &addRst);
                    }
                }
            }
        }
    }

    // if (D) Log.d(TAG, "provider request: " + provider + " " + providerRequest);
    p->SetRequest(providerRequest, worksource);
}

AutoPtr<CLocationManagerService::Receiver> CLocationManagerService::GetReceiver(
    /* [in] */ IILocationListener* listener,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    // IBinder binder = listener.asBinder();
    AutoPtr<Receiver> receiver;
    AutoPtr<IInterface> keyTemp = listener->Probe(EIID_IInterface);
    InterfaceReceiverIterator it = mReceivers.Find(keyTemp);
    if (it != mReceivers.End()) {
        receiver = it->mSecond;
    }
    if (receiver == NULL) {
        receiver = new Receiver(listener, NULL, pid, uid, packageName, this);
        mReceivers[keyTemp] = receiver;

        AutoPtr<IILocationListener> l;
        receiver->GetListener((IILocationListener**)&l);
        AutoPtr<IProxy> link = (IProxy*)(l->Probe(EIID_IProxy));
        if (link != NULL && FAILED(link->LinkToDeath(receiver, 0))) {
            return NULL;
        }
        // try {
        // receiver.getListener().asBinder().linkToDeath(receiver, 0);
        // } catch (RemoteException e) {
        //     Slog.e(TAG, "linkToDeath failed:", e);
        //     return null;
        // }
    }
    return receiver;
}

AutoPtr<CLocationManagerService::Receiver> CLocationManagerService::GetReceiver(
    /* [in] */ IPendingIntent* intent,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    AutoPtr<Receiver> receiver;
    AutoPtr<IInterface> keyTemp = intent->Probe(EIID_IInterface);
    HashMap<AutoPtr<IInterface>, AutoPtr<Receiver> >::Iterator it = mReceivers.Find(keyTemp);
    if (it != mReceivers.End()) {
        receiver = it->mSecond;
    }
    if (receiver == NULL) {
        receiver = new Receiver(NULL, intent, pid, uid, packageName, this);
        mReceivers[keyTemp] = receiver;
    }
    return receiver;
}

AutoPtr<ILocationRequest> CLocationManagerService::CreateSanitizedRequest(
    /* [in] */ ILocationRequest* request,
    /* [in] */ Int32 resolutionLevel)
{
    AutoPtr<ILocationRequest> sanitizedRequest;
    CLocationRequest::New(request, (ILocationRequest**)&sanitizedRequest);
    Int64 interval;
    if (resolutionLevel < RESOLUTION_LEVEL_FINE) {
        Int32 quality;
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
    Int64 fastestInterval;
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

ECode CLocationManagerService::CheckListenerOrIntent(
    /* [in] */ IILocationListener* listener,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName,
    /* [out] */ Receiver** receiver)
{
    VALIDATE_NOT_NULL(receiver);
    *receiver = NULL;

    if (intent == NULL && listener == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("need eiter listener or intent");
    }
    else if (intent != NULL && listener != NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("cannot register both listener and intent");
    }
    else if (intent != NULL) {
        FAIL_RETURN(CheckPendingIntent(intent));
        AutoPtr<Receiver> r = GetReceiver(intent, pid, uid, packageName);
        *receiver = r;
        REFCOUNT_ADD(*receiver);
        return NOERROR;
    }
    else {
        AutoPtr<Receiver> r = GetReceiver(listener, pid, uid, packageName);
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
    AutoPtr<ILocationRequest> sanitizedRequest
            = CreateSanitizedRequest(request, allowedResolutionLevel);

    Int32 pid = Binder::GetCallingPid();
    Int32 uid = Binder::GetCallingUid();
    AutoPtr<Receiver> receiver;
    FAIL_RETURN(CheckListenerOrIntent(listener, intent, pid, uid,
            packageName, (Receiver**)&receiver));

    // providers may use public location API's, need to clear identity
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    AutoLock lock(mLock);
    ECode ec = RequestLocationUpdatesLocked(sanitizedRequest, receiver, pid, uid,
            packageName);
        // }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
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
    AutoPtr<ILocationProviderInterface> provider;
    HashMap<String, AutoPtr<ILocationProviderInterface> >::Iterator it
            = mProvidersByName.Find(name);
    if (it != mProvidersByName.End()) {
        provider = it->mSecond;
    }
    if (provider == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("provider doesn't exisit: " + provider);
    }

    // if (D) Log.d(TAG, "request " + Integer.toHexString(System.identityHashCode(receiver))
    //         + " " + name + " " + request + " from " + packageName + "(" + uid + ")");

    AutoPtr<UpdateRecord> record = new UpdateRecord(name, request, receiver, this);
    AutoPtr<UpdateRecord> oldRecord = receiver->mUpdateRecords[name];
    receiver->mUpdateRecords[name] = record;
    if (oldRecord != NULL) {
        oldRecord->DisposeLocked(FALSE);
    }

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId;
    helper->GetUserId(uid, &userId);
    Boolean isProviderEnabled = IsAllowedBySettingsLocked(name, userId);
    if (isProviderEnabled) {
        ApplyRequirementsLocked(name);
    }
    else {
        // Notify the listener that updates are currently disabled
        receiver->CallProviderEnabledLocked(name, FALSE);
    }
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
    AutoPtr<Receiver> receiver;
    FAIL_RETURN(CheckListenerOrIntent(listener, intent, pid, uid,
            packageName, (Receiver**)&receiver));

    // providers may use public location API's, need to clear identity
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    AutoLock lock(mLock);
    RemoveUpdatesLocked(receiver);
        // }
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

void CLocationManagerService::RemoveUpdatesLocked(
    /* [in] */ Receiver* receiver)
{
    // if (D) Log.i(TAG, "remove " + Integer.toHexString(System.identityHashCode(receiver)));

    HashMap<AutoPtr<IInterface>, AutoPtr<Receiver> >::Iterator it
            = mReceivers.Find(receiver->mKey);
    if (it != mReceivers.End() && receiver->IsListener()) {
        mReceivers.Erase(it);
        // receiver.getListener().asBinder().unlinkToDeath(receiver, 0);
        AutoLock lock(receiver->mLock);
        if(receiver->mPendingBroadcasts > 0) {
            DecrementPendingBroadcasts();
            receiver->mPendingBroadcasts = 0;
        }
    }

    // Record which providers were associated with this listener
    HashSet<String> providers;
    HashMap<String, AutoPtr<UpdateRecord> >& oldRecords = receiver->mUpdateRecords;
    // Call dispose() on the obsolete update records.
    HashMap<String, AutoPtr<UpdateRecord> >::Iterator oldit;
    for (oldit = oldRecords.Begin(); oldit != oldRecords.End(); ++oldit) {
        oldit->mSecond->DisposeLocked(FALSE);
        // Accumulate providers
        providers.Insert(oldit->mFirst);
    }

    // update provider
    HashSet<String>::Iterator providersIt;
    for (providersIt = providers.Begin(); providersIt != providers.End(); ++providersIt) {
        // If provider is already disabled, don't need to do anything
        if (!IsAllowedBySettingsLocked(*providersIt, mCurrentUserId)) {
            continue;
        }

        ApplyRequirementsLocked(*providersIt);
    }
}

ECode CLocationManagerService::GetLastLocation(
    /* [in] */ ILocationRequest* request,
    /* [in] */ const String& packageName,
    /* [out] */ ILocation** location)
{
    VALIDATE_NOT_NULL(location);
    *location = NULL;

    // if (D) Log.d(TAG, "getLastLocation: " + request);
    if (request == NULL) request = DEFAULT_LOCATION_REQUEST;
    Int32 allowedResolutionLevel = GetCallerAllowedResolutionLevel();
    FAIL_RETURN(CheckPackageName(packageName));
    String name;
    request->GetProvider(&name);
    FAIL_RETURN(CheckResolutionLevelIsSufficientForProviderUse(allowedResolutionLevel,
            name));
    // no need to sanitize this request, as only the provider name is used

    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    if (mBlacklist->IsBlacklisted(packageName)) {
        // if (D) Log.d(TAG, "not returning last loc for blacklisted app: " +
        //         packageName);
        Binder::RestoreCallingIdentity(identity);
        return NOERROR;
    }

    AutoLock lock(mLock);
    // Figure out the provider. Either its explicitly request (deprecated API's),
    // or use the fused provider
    if (name.IsNull()) name = ILocationManager::FUSED_PROVIDER;
    AutoPtr<ILocationProviderInterface> provider;
    HashMap<String, AutoPtr<ILocationProviderInterface> >::Iterator it
            = mProvidersByName.Find(name);
    if (it != mProvidersByName.End()) {
        provider = it->mSecond;
    }
    if (provider == NULL) {
        Binder::RestoreCallingIdentity(identity);
        return NOERROR;
    }

    if (!IsAllowedBySettingsLocked(name, mCurrentUserId)) {
        Binder::RestoreCallingIdentity(identity);
        return NOERROR;
    }

    AutoPtr<ILocation> l;
    HashMap<String, AutoPtr<ILocation> >::Iterator locationIt
            = mLastLocation.Find(name);
    if (locationIt != mLastLocation.End()) {
        l = locationIt->mSecond;
    }
    if (l == NULL) {
        Binder::RestoreCallingIdentity(identity);
        return NOERROR;
    }
    if (allowedResolutionLevel < RESOLUTION_LEVEL_FINE) {
        AutoPtr<ILocation> noGPSLocation;
        l->GetExtraLocation(ILocation::EXTRA_NO_GPS_LOCATION, (ILocation**)&noGPSLocation);
        if (noGPSLocation != NULL) {
            AutoPtr<ILocation> temp = mLocationFudger->GetOrCreate(noGPSLocation);
            CLocation::New(temp, location);
            Binder::RestoreCallingIdentity(identity);
            return NOERROR;
        }
    }
    else {
        CLocation::New(l, location);
        Binder::RestoreCallingIdentity(identity);
        return NOERROR;
    }

    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
    // } finally {
    //     Binder.restoreCallingIdentity(identity);
    // }
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

    // if (D) Log.d(TAG, "requestGeofence: " + sanitizedRequest + " " + geofence + " " + intent);

    // geo-fence manager uses the public location API, need to clear identity
    Int32 uid = Binder::GetCallingUid();

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    Int32 userId;
    helper->GetUserId(uid, &userId);
    if (userId != IUserHandle::USER_OWNER) {
        // temporary measure until geofences work for secondary users
        // Log.w(TAG, "proximity alerts are currently available only to the primary user");
        return NOERROR;
    }
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    mGeofenceManager->AddFence(sanitizedRequest, geofence, intent, uid, packageName);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
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

    // if (D) Log.d(TAG, "removeGeofence: " + geofence + " " + intent);

    // geo-fence manager uses the public location API, need to clear identity
    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    mGeofenceManager->RemoveFence(geofence, intent);
    // } finally {
    Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
}

ECode CLocationManagerService::AddGpsStatusListener(
    /* [in] */ IIGpsStatusListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (mGpsStatusProvider == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    FAIL_RETURN(CheckResolutionLevelIsSufficientForProviderUse(
            GetCallerAllowedResolutionLevel(),
            ILocationManager::GPS_PROVIDER));

    // try {
    ECode ec = mGpsStatusProvider->AddGpsStatusListener(listener);
    if (FAILED(ec)) {
        *result = FALSE;
        return ec;
    }
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "mGpsStatusProvider.addGpsStatusListener failed", e);
    //     return false;
    // }
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
    Int32 perm;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(ACCESS_LOCATION_EXTRA_COMMANDS, &perm));
    if ((perm != IPackageManager::PERMISSION_GRANTED)) {
        return E_SECURITY_EXCEPTION;
        // throw new SecurityException("Requires ACCESS_LOCATION_EXTRA_COMMANDS permission");
    }

    AutoLock lock(mLock);
    AutoPtr<ILocationProviderInterface> p;
    HashMap<String, AutoPtr<ILocationProviderInterface> >::Iterator it =
            mProvidersByName.Find(provider);
    if (it != mProvidersByName.End()) {
        p = it->mSecond;
    }
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
    if (Binder::GetCallingUid() != Process::MyUid()) {
        // throw new SecurityException(
        //        "calling sendNiResponse from outside of the system is not allowed");
        return E_SECURITY_EXCEPTION;
    }
    // try {
    ECode ec = mNetInitiatedListener->SendNiResponse(notifId, userResponse, result);
    if (FAILED(ec)) {
        *result = FALSE;
    }
    return ec;
    // }
    // catch (RemoteException e)
    // {
    //     Slog.e(TAG, "RemoteException in LocationManagerService.sendNiResponse");
    //     return false;
    // }
}

ECode CLocationManagerService::GetProviderProperties(
    /* [in] */ const String& provider,
    /* [out] */ IProviderProperties** properties)
{
    VALIDATE_NOT_NULL(properties);
    *properties = NULL;

    AutoPtr<ILocationProviderInterface> p;
    HashMap<String, AutoPtr<ILocationProviderInterface> >::Iterator it
            = mProvidersByName.Find(provider);
    if (it == mProvidersByName.End()) {
        return NOERROR;
    }

    FAIL_RETURN(CheckResolutionLevelIsSufficientForProviderUse(
            GetCallerAllowedResolutionLevel(), provider));
    {
        AutoLock lock(mLock);
        it = mProvidersByName.Find(provider);
        p = it->mSecond;
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

    FAIL_RETURN(CheckResolutionLevelIsSufficientForProviderUse(
            GetCallerAllowedResolutionLevel(), provider));
    if (ILocationManager::FUSED_PROVIDER.Equals(provider)) {
        return NOERROR;
    }

    Int64 identity = Binder::ClearCallingIdentity();
    // try {
    AutoLock lock(mLock);
    AutoPtr<ILocationProviderInterface> p;
    HashMap<String, AutoPtr<ILocationProviderInterface> >::Iterator it
            = mProvidersByName.Find(provider);
    if (it == mProvidersByName.End()) {
        *isEnabled = FALSE;
        Binder::RestoreCallingIdentity(identity);
        return NOERROR;
    }

    *isEnabled = IsAllowedBySettingsLocked(provider, mCurrentUserId);
    // } finally {
        Binder::RestoreCallingIdentity(identity);
    // }
    return NOERROR;
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

    Int32 uid = Binder::GetCallingUid();

    if (mGeocodeProvider != NULL) {
        if (DoesPackageHaveUid(uid, mGeocodeProvider->GetConnectedPackageName()))
            return NOERROR;
    }
    List<AutoPtr<LocationProviderProxy> >::Iterator it = mProxyProviders.Begin();
    for (; it != mProxyProviders.End(); ++it) {
        if (DoesPackageHaveUid(uid, (*it)->GetConnectedPackageName()))
            return NOERROR;
    }
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
        // Log.w(TAG, "Dropping incomplete location: " + location);
        return NOERROR;
    }

    mLocationHandler->RemoveMessages(MSG_LOCATION_CHANGED, location);
    AutoPtr<IMessage> msg;
    mLocationHandler->ObtainMessage(MSG_LOCATION_CHANGED, location, (IMessage**)&msg);
    msg->SetArg1(passive ? 1 : 0);
    Boolean result;
    return mLocationHandler->SendMessageAtFrontOfQueue(msg, &result);
}

Boolean CLocationManagerService::DoesPackageHaveUid(
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
    if (packageName.IsNull()) {
        return FALSE;
    }
//    try {
        AutoPtr<IApplicationInfo> appInfo;
        ECode ec = mPackageManager->GetApplicationInfo(packageName, 0, (IApplicationInfo**)&appInfo);
        if(appInfo == NULL || FAILED(ec))
        {
            return FALSE;
        }
        Int32 infoUid;
        appInfo->GetUid(&infoUid);

        if (infoUid != uid) {
            return FALSE;
        }
//    } catch (NameNotFoundException e) {
//        return false;
//    }
    return TRUE;
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
    Int64 minTime;
    record->mRequest->GetFastestInterval(&minTime);
    Int64 nanos, lastNanos;
    loc->GetElapsedRealtimeNanos(&nanos);
    lastLoc->GetElapsedRealtimeNanos(&lastNanos);
    Int64 delta = (nanos - lastNanos) / 1000000L;
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
        loc->DistanceTo(lastLoc, (Float*)&dis);
        if (dis <= minDistance) {
            return FALSE;
        }
    }

    // Check whether sufficient number of udpates is left
    Int32 numUpdates;
    record->mRequest->GetNumUpdates(&numUpdates);
    if (numUpdates <= 0) {
        return FALSE;
    }

    // Check whether the expiry date has passed
    Int64 value;
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
    // if (D) Log.d(TAG, "incoming location: " + location);

    Int64 now = SystemClock::GetElapsedRealtime();
    String provider;
    if(passive) {
        provider = ILocationManager::PASSIVE_PROVIDER;
    }
    else {
        location->GetProvider(&provider);
    }

    // Skip if the provider is unknown.
    AutoPtr<ILocationProviderInterface> p;
    HashMap<String, AutoPtr<ILocationProviderInterface> >::Iterator providersIt =
            mProvidersByName.Find(provider);
    if (providersIt != mProvidersByName.End()) {
        p = providersIt->mSecond;
    }
    if (p == NULL) {
        return;
    }

    // Update last known location for locations
    AutoPtr<ILocation> noGPSLocation;
    location->GetExtraLocation(ILocation::EXTRA_NO_GPS_LOCATION, (ILocation**)&noGPSLocation);
    AutoPtr<ILocation> lastNoGPSLocation;
    AutoPtr<ILocation> lastLocation;
    HashMap<String, AutoPtr<ILocation> >::Iterator locIt = mLastLocation.Find(provider);
    if (locIt != mLastLocation.End()) {
        lastLocation = locIt->mSecond;
    }
    if (lastLocation == NULL) {
        CLocation::New(provider, (ILocation**)&lastLocation);
        mLastLocation[provider] = lastLocation;
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

    // Skip if there are no UpdateRecords for this provider.
    AutoPtr< UpdateRecordList > records;
    HashMap<String, AutoPtr< UpdateRecordList > >::Iterator it =
            mRecordsByProvider.Find(provider);
    if (it == mRecordsByProvider.End() || it->mSecond->Begin() == it->mSecond->End())
        return;

    records = it->mSecond;

    // Fetch coarse location
    AutoPtr<ILocation> coarseLocation = NULL;
    if (noGPSLocation != NULL && !(noGPSLocation == lastNoGPSLocation)) {
        coarseLocation = mLocationFudger->GetOrCreate(noGPSLocation);
    }

    // Fetch latest status update time
    Int64 newStatusUpdateTime;
    p->GetStatusUpdateTime(&newStatusUpdateTime);

    // Get latest status
    AutoPtr<IBundle> extras;
    CBundle::New((IBundle**)&extras);
    Int32 status;
    p->GetStatus(extras, &status);

    List<AutoPtr<Receiver> > deadReceivers;
    List<AutoPtr<UpdateRecord> > deadUpdateRecords;

    // Broadcast location or status to all listeners
    UpdateRecordListIterator recordIt;
    for (recordIt = records->Begin(); recordIt != records->End(); ++recordIt) {
        AutoPtr<UpdateRecord> r = *recordIt;
        AutoPtr<Receiver> receiver = r->mReceiver;
        Boolean receiverDead = FALSE;

        AutoPtr<IUserHandleHelper> helper;
        CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
        Int32 receiverUserId;
        helper->GetUserId(receiver->mUid, &receiverUserId);
        if (receiverUserId != mCurrentUserId) {
            // if (D) {
            //     Log.d(TAG, "skipping loc update for background user " + receiverUserId +
            //             " (current user: " + mCurrentUserId + ", app: " +
            //             receiver.mPackageName + ")");
            // }
            continue;
        }

        if (mBlacklist->IsBlacklisted(receiver->mPackageName)) {
            // if (D) Log.d(TAG, "skipping loc update for blacklisted app: " +
            //         receiver.mPackageName);
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
                    // Slog.w(TAG, "RemoteException calling onLocationChanged on " + receiver);
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
                // Slog.w(TAG, "RemoteException calling onStatusChanged on " + receiver);
            }
        }

        // track expired records
        Int32 numUpdates;
        r->mRequest->GetNumUpdates(&numUpdates);
        Int64 value;
        r->mRequest->GetExpireAt(&value);
        if (numUpdates <= 0 || value < now) {
            deadUpdateRecords.PushBack(r);
        }

        // track dead receivers
        if (receiverDead) {
            List<AutoPtr<Receiver> >::Iterator deadIt = Find(deadReceivers.Begin(), deadReceivers.End(), receiver);
            if (deadIt == deadReceivers.End()) {
                deadReceivers.PushBack(receiver);
            }
        }
    }

    // remove dead records and receivers outside the loop
    if (deadReceivers.IsEmpty() == FALSE) {
        List<AutoPtr<Receiver> >::Iterator deadIt = deadReceivers.Begin();
        for (; deadIt != deadReceivers.End(); ++deadIt) {
            RemoveUpdatesLocked(*deadIt);
        }
        deadReceivers.Clear();
    }

    if (deadUpdateRecords.IsEmpty() == FALSE) {
        UpdateRecordListIterator deadIt = deadUpdateRecords.Begin();
        for (; deadIt != deadUpdateRecords.End(); ++deadIt) {
            (*deadIt)->DisposeLocked(TRUE);
        }
        ApplyRequirementsLocked(provider);
        deadUpdateRecords.Clear();
    }
}

void CLocationManagerService::HandleLocationChanged(
    /* [in] */ ILocation* location,
    /* [in] */ Boolean passive)
{
    String provider;
    location->GetProvider(&provider);

    if (!passive) {
        // notify passive provider of the new location
        mPassiveProvider->UpdateLocation(location);
    }

    AutoLock lock(mLock);
    if (IsAllowedBySettingsLocked(provider, mCurrentUserId)) {
        HandleLocationChangedLocked(location, passive);
    }
}

void CLocationManagerService::IncrementPendingBroadcasts()
{
    // synchronized(mWakeLock) {
    AutoLock lock(mLockForWakeLock);
    if (mPendingBroadcasts++ == 0) {
        // try {
//        mWakeLock->Acquire();
        // log("Acquired wakelock");
        // } catch (Exception e) {
        //     // This is to catch a runtime exception thrown when we try to release an
        //     // already released lock.
        //     Slog.e(TAG, "exception in acquireWakeLock()", e);
        // }
    }
    // }
}

void CLocationManagerService::DecrementPendingBroadcasts()
{
    AutoLock lock(mLockForWakeLock);
    // synchronized(mWakeLock) {
    if (--mPendingBroadcasts == 0) {
        // try {
        // Release wake lock
        Boolean isHeld;
        mWakeLock->IsHeld(&isHeld);
        if (isHeld) {
            mWakeLock->Release();
            // log("Released wakelock");
        }
        else {
            // log("Can't release wakelock again!");
        }
        // } catch (Exception e) {
        //     // This is to catch a runtime exception thrown when we try to release an
        //     // already released lock.
        //     Slog.e(TAG, "exception in releaseWakeLock()", e);
        // }
    }
    // }
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
    /* [out] */ IObjectContainer** addrs,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(addrs);
    VALIDATE_NOT_NULL(result);

    if (mGeocodeProvider != NULL) {
        *result = mGeocodeProvider->GetFromLocation(latitude, longitude, maxResults,
                params, addrs);
        return NOERROR;
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
    /* [out] */ IObjectContainer** addrs,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(addrs);
    VALIDATE_NOT_NULL(result);

    if (mGeocodeProvider != NULL) {
        *result = mGeocodeProvider->GetFromLocationName(locationName, lowerLeftLatitude,
                lowerLeftLongitude, upperRightLatitude, upperRightLongitude,
                maxResults, params, addrs);
        return NOERROR;
    }
    *result = String(NULL);
    return NOERROR;
}

ECode CLocationManagerService::CheckMockPermissionsSafe()
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 mockLocation;
    AutoPtr<ISettingsSecure> settingsSecure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&settingsSecure);
    settingsSecure->GetInt32(cr, ISettingsSecure::ALLOW_MOCK_LOCATION, 0, &mockLocation);
    Boolean allowMocks = mockLocation == 1;
    // Boolean allowMocks = Settings.Secure.getInt(mContext.getContentResolver(),
    //         Settings.Secure.ALLOW_MOCK_LOCATION, 0) == 1;
     if (!allowMocks) {
//         throw new SecurityException("Requires ACCESS_MOCK_LOCATION secure setting");
         return E_SECURITY_EXCEPTION;
     }

    Int32 value;
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
    AutoLock lock(mLock);
    AutoPtr<MockProvider> provider = new MockProvider(name, this, properties);
    // remove the real provider if we are replacing GPS or network provider
    HashMap<String, AutoPtr<ILocationProviderInterface> >::Iterator it;
    if (ILocationManager::GPS_PROVIDER.Equals(name)
            || ILocationManager::NETWORK_PROVIDER.Equals(name)
            || ILocationManager::FUSED_PROVIDER.Equals(name)) {
        it = mProvidersByName.Find(name);
        if (it != mProvidersByName.End()) {
            AutoPtr<ILocationProviderInterface> p = it->mSecond;
            if(p != NULL)
                RemoveProviderLocked(p);
        }
    }
    it = mProvidersByName.Find(name);
    if (it != mProvidersByName.End()) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Provider \"" + name + "\" already exists");
    }
    AddProviderLocked(provider);
    mMockProviders[name] = provider;
    mLastLocation[name] = NULL;
    UpdateProvidersLocked();
    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

ECode CLocationManagerService::RemoveTestProvider(
    /* [in] */ const String& provider)
{
    FAIL_RETURN(CheckMockPermissionsSafe());
    AutoLock lock(mLock);
    AutoPtr<MockProvider> mockProvider;
    HashMap<String, AutoPtr<MockProvider> >::Iterator mpIt =
            mMockProviders.Find(provider);
    if(mpIt != mMockProviders.End())
    {
        mockProvider = mpIt->mSecond;
    }
    if (mockProvider == NULL) {
//        throw new IllegalArgumentException("Provider \"" + provider + "\" unknown");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int64 identity = Binder::ClearCallingIdentity();
    HashMap<String, AutoPtr<ILocationProviderInterface> >::Iterator lpiIt =
            mProvidersByName.Find(provider);

    RemoveProviderLocked(lpiIt->mSecond);
    mMockProviders.Erase(mpIt);

    // reinstate real provider if available
    AutoPtr<ILocationProviderInterface> realProvider;
    lpiIt = mRealProviders.Find(provider);
    if(lpiIt != mRealProviders.End())
    {
        realProvider = lpiIt->mSecond;
        if(realProvider != NULL)
        {
            AddProviderLocked(realProvider);
        }
    }

    mLastLocation[provider] = NULL;
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
    AutoPtr<MockProvider> mockProvider;
    HashMap<String, AutoPtr<MockProvider> >::Iterator mpIt =
            mMockProviders.Find(provider);
    if(mpIt != mMockProviders.End())
    {
        mockProvider = mpIt->mSecond;
    }
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
    AutoPtr<MockProvider> mockProvider;
    HashMap<String, AutoPtr<MockProvider> >::Iterator mpIt =
            mMockProviders.Find(provider);
    if(mpIt != mMockProviders.End())
    {
        mockProvider = mpIt->mSecond;
    }
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
    AutoPtr<MockProvider> mockProvider;
    HashMap<String, AutoPtr<MockProvider> >::Iterator mpIt =
            mMockProviders.Find(provider);
    if(mpIt != mMockProviders.End())
    {
        mockProvider = mpIt->mSecond;
    }
    if (mockProvider == NULL) {
//        throw new IllegalArgumentException("Provider \"" + provider + "\" unknown");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 identity = Binder::ClearCallingIdentity();
    if (enabled) {
        mockProvider->Enable();
        mEnabledProviders.Insert(provider);
        mDisabledProviders.Erase(provider);
    } else {
        mockProvider->Disable();
        mEnabledProviders.Erase(provider);
        mDisabledProviders.Insert(provider);
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
    AutoPtr<MockProvider> mockProvider;
    HashMap<String, AutoPtr<MockProvider> >::Iterator mpIt =
            mMockProviders.Find(provider);
    if(mpIt != mMockProviders.End())
    {
        mockProvider = mpIt->mSecond;
    }
    if (mockProvider == NULL) {
//        throw new IllegalArgumentException("Provider \"" + provider + "\" unknown");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // clear calling identity so INSTALL_LOCATION_PROVIDER permission is not required
    Int64 identity = Binder::ClearCallingIdentity();
    mEnabledProviders.Erase(provider);
    mDisabledProviders.Erase(provider);
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
    AutoPtr<MockProvider> mockProvider;
    HashMap<String, AutoPtr<MockProvider> >::Iterator mpIt =
            mMockProviders.Find(provider);
    if(mpIt != mMockProviders.End())
    {
        mockProvider = mpIt->mSecond;
    }
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
    AutoPtr<MockProvider> mockProvider;
    HashMap<String, AutoPtr<MockProvider> >::Iterator mpIt =
            mMockProviders.Find(provider);
    if(mpIt != mMockProviders.End())
    {
        mockProvider = mpIt->mSecond;
    }
    if (mockProvider == NULL) {
//        throw new IllegalArgumentException("Provider \"" + provider + "\" unknown");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mockProvider->ClearStatus();
    return NOERROR;
}

} // wm
} // Server
} // Droid
} // Elastos
