#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Telephony.h"
#include "Elastos.CoreLibrary.Libcore.h"
#include "Elastos.CoreLibrary.Net.h"
#include "elastos/droid/hardware/location/GeofenceHardwareImpl.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/server/location/CNetInitiatedListener.h"
#include "elastos/droid/server/location/CGpsGeofenceHardwareService.h"
#include "elastos/droid/server/location/CGpsStatusProviderService.h"
#include "elastos/droid/server/location/GpsLocationProvider.h"
#include "elastos/droid/server/location/GpsXtraDownloader.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <hardware/hardware.h>
#include <hardware/gps.h>
#include <hardware_legacy/power.h>
#include <utils/AndroidThreads.h>
#include <utils/misc.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <linux/in.h>
#include <linux/in6.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::CAppOpsManagerHelper;
using Elastos::Droid::App::IAppOpsManagerHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Hardware::Location::GeofenceHardwareImpl;
using Elastos::Droid::Hardware::Location::IGeofenceHardware;
using Elastos::Droid::Internal::Location::CGpsNiNotification;
using Elastos::Droid::Internal::Location::IGpsNiNotification;
using Elastos::Droid::Internal::Location::CGpsNetInitiatedHandler;
using Elastos::Droid::Internal::Location::CProviderProperties;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Location::CLocation;
using Elastos::Droid::Location::CLocationRequestHelper;
using Elastos::Droid::Location::ICriteria;
using Elastos::Droid::Location::EIID_IINetInitiatedListener;
using Elastos::Droid::Location::EIID_IIGpsGeofenceHardware;
using Elastos::Droid::Location::EIID_IIGpsStatusProvider;
using Elastos::Droid::Location::EIID_ILocationListener;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::ILocationProvider;
using Elastos::Droid::Location::ILocationRequest;
using Elastos::Droid::Location::ILocationRequestHelper;
using Elastos::Droid::Location::CGpsClock;
using Elastos::Droid::Location::IGpsClock;
using Elastos::Droid::Location::CGpsMeasurement;
using Elastos::Droid::Location::IGpsMeasurement;
using Elastos::Droid::Location::CGpsMeasurementsEvent;
using Elastos::Droid::Location::IGpsMeasurementsEvent;
using Elastos::Droid::Location::CGpsNavigationMessage;
using Elastos::Droid::Location::IGpsNavigationMessage;
using Elastos::Droid::Location::CGpsNavigationMessageEvent;
using Elastos::Droid::Location::IGpsNavigationMessageEvent;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CWorkSource;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::IBatteryStats;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Server::Location::CNetInitiatedListener;
using Elastos::Droid::Server::Location::CGpsGeofenceHardwareService;
using Elastos::Droid::Server::Location::CGpsStatusProviderService;
using Elastos::Droid::Server::Location::GpsXtraDownloader;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::Gsm::IGsmCellLocation;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CNtpTrustedTimeHelper;
using Elastos::Droid::Utility::INtpTrustedTimeHelper;
using Elastos::Droid::Utility::ITrustedTime;
using Elastos::Droid::R;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::IAppendable;
using Elastos::Core::ISystem;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CStringReader;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::ICloseable;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IReader;
using Elastos::IO::IOutputStream;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInetAddressHelper;
using Elastos::Utility::CDate;
using Elastos::Utility::CProperties;
using Elastos::Utility::IDate;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::IHashTable;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Logging::Logger;
using Libcore::IO::CIoUtils;
using Libcore::IO::IIoUtils;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

static AutoPtr<IProviderProperties> InitPROPERTIES()
{
    AutoPtr<IProviderProperties> pp;
    CProviderProperties::New(TRUE, TRUE, FALSE, FALSE, TRUE, TRUE, TRUE,
        ICriteria::Criteria_POWER_HIGH, ICriteria::Criteria_ACCURACY_FINE, (IProviderProperties**)&pp);
    return pp;
}

//===========================================
//GpsLocationProvider::MyRunnable
//===========================================

GpsLocationProvider::MyRunnable::MyRunnable(
    /* [in] */ GpsLocationProvider* host)
    : mHost(host)
{}

ECode GpsLocationProvider::MyRunnable::Run()
{
    AutoPtr<IInterface> lmObj;
    mHost->mContext->GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&lmObj);
    AutoPtr<ILocationManager> locManager = ILocationManager::Probe(lmObj);
    const Int64 minTime = 0;
    const Float minDistance = 0;
    const Boolean oneShot = FALSE;
    AutoPtr<ILocationRequestHelper> lrh;
    CLocationRequestHelper::AcquireSingleton((ILocationRequestHelper**)&lrh);
    AutoPtr<ILocationRequest> request;
    lrh->CreateFromDeprecatedProvider(ILocationManager::PASSIVE_PROVIDER,
        minTime, minDistance, oneShot, (ILocationRequest**)&request);
    // Don't keep track of this request since it's done on behalf of other clients
    // (which are kept track of separately).
    request->SetHideFromAppOps(TRUE);
    AutoPtr<ILooper> looper;
    mHost->mHandler->GetLooper((ILooper**)&looper);
    locManager->RequestLocationUpdates(request, new NetworkLocationListener(mHost), looper);
    return NOERROR;
}

//===========================================
//GpsLocationProvider::MyRunnable2
//===========================================

GpsLocationProvider::MyRunnable2::MyRunnable2(
    /* [in] */ GpsLocationProvider* host)
    : mHost(host)
{}

ECode GpsLocationProvider::MyRunnable2::Run()
{
    Int64 delay;

    // force refresh NTP cache when outdated
    Int64 cacheAge;
    ITrustedTime::Probe(mHost->mNtpTime)->GetCacheAge(&cacheAge);
    if (cacheAge >= NTP_INTERVAL) {
        Boolean isRefreshed;
        ITrustedTime::Probe(mHost->mNtpTime)->ForceRefresh(&isRefreshed);
    }

    // only update when NTP time is fresh
    if (cacheAge < NTP_INTERVAL) {
        Int64 time;
        mHost->mNtpTime->GetCachedNtpTime(&time);
        Int64 timeReference;
        mHost->mNtpTime->GetCachedNtpTimeReference(&timeReference);
        Int64 certainty;
        ITrustedTime::Probe(mHost->mNtpTime)->GetCacheCertainty(&certainty);
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 now;
        sys->GetCurrentTimeMillis(&now);
        AutoPtr<IDate> date;
        CDate::New(time, (IDate**)&date);
        String dateStr;
        IObject::Probe(date)->ToString(&dateStr);
        Logger::D(TAG, "NTP server returned: %lld (%s) reference: %lld certainty: %lld system time offset: %lld",
            time, dateStr.string(), timeReference, certainty, (time - now));
        mHost->Native_inject_time(time, timeReference, (Int32)certainty);
        delay = NTP_INTERVAL;
    }
    else {
        if (DEBUG) Logger::D(TAG, "requestTime failed");
        delay = RETRY_INTERVAL;
    }

    mHost->SendMessage(INJECT_NTP_TIME_FINISHED, 0, NULL);

    if (mHost->mPeriodicTimeInjection) {
        // send delayed message for next NTP injection
        // since this is delayed and not urgent we do not hold a wake lock here
        Boolean res;
        mHost->mHandler->SendEmptyMessageDelayed(INJECT_NTP_TIME, delay, &res);
    }

    // release wake lock held by task
    mHost->mWakeLock->ReleaseLock();
    return NOERROR;
}

//===========================================
//GpsLocationProvider::MyRunnable3
//===========================================

GpsLocationProvider::MyRunnable3::MyRunnable3(
    /* [in] */ GpsLocationProvider* host)
    : mHost(host)
{}

ECode GpsLocationProvider::MyRunnable3::Run()
{
    AutoPtr<GpsXtraDownloader> xtraDownloader = new GpsXtraDownloader(mHost->mContext, mHost->mProperties);
    AutoPtr<ArrayOf<Byte> > data = xtraDownloader->DownloadXtraData();
    if (data != NULL) {
        if (DEBUG) {
            Logger::D(TAG, "calling Native_inject_xtra_data");
        }
        mHost->Native_inject_xtra_data(data, data->GetLength());
    }

    mHost->SendMessage(DOWNLOAD_XTRA_DATA_FINISHED, 0, NULL);

    if (data == NULL) {
        // try again later
        // since this is delayed and not urgent we do not hold a wake lock here
        Boolean res;
        mHost->mHandler->SendEmptyMessageDelayed(DOWNLOAD_XTRA_DATA, RETRY_INTERVAL, &res);
    }

    // release wake lock held by task
    mHost->mWakeLock->ReleaseLock();
    return NOERROR;
}

//===========================================
//GpsLocationProvider::GpsRequest
//===========================================

GpsLocationProvider::GpsRequest::GpsRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* source)
    : mRequest(request)
    , mSource(source)
{
}

//===========================================
//GpsLocationProvider::MyListenerHelper
//===========================================

ECode GpsLocationProvider::MyListenerHelper::IsSupported(
    /* [out] */ Boolean* isSupported)
{
    VALIDATE_NOT_NULL(isSupported)
    *isSupported = GpsLocationProvider::IsSupported();
    return NOERROR;
}

ECode GpsLocationProvider::MyListenerHelper::RegisterWithService(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

ECode GpsLocationProvider::MyListenerHelper::UnregisterFromService()
{
    return NOERROR;
}

//===========================================
//GpsLocationProvider::GpsStatusProviderService
//===========================================

CAR_INTERFACE_IMPL_2(GpsLocationProvider::GpsStatusProviderService, Object, IIGpsStatusProvider, IBinder)

ECode GpsLocationProvider::GpsStatusProviderService::constructor(
    /* [in] */ ILocationProviderInterface* host)
{
    mHost = (GpsLocationProvider*)host;
    return NOERROR;
}

ECode GpsLocationProvider::GpsStatusProviderService::AddGpsStatusListener(
    /* [in] */ IIGpsStatusListener* listener)
{
    Boolean res;
    return mHost->mListenerHelper->AddListener(listener, &res);
}

ECode GpsLocationProvider::GpsStatusProviderService::RemoveGpsStatusListener(
    /* [in] */ IIGpsStatusListener* listener)
{
    Boolean res;
    return mHost->mListenerHelper->RemoveListener(listener, &res);
}

//===========================================
//GpsLocationProvider::MyGpsMeasurementsProvider
//===========================================

GpsLocationProvider::MyGpsMeasurementsProvider::MyGpsMeasurementsProvider(
    /* [in] */ GpsLocationProvider* host)
    : mHost(host)
{}

ECode GpsLocationProvider::MyGpsMeasurementsProvider::IsSupported(
    /* [out] */ Boolean* isSupported)
{
    VALIDATE_NOT_NULL(isSupported)
    *isSupported = GpsLocationProvider::Native_is_measurement_supported();
    return NOERROR;
}

ECode GpsLocationProvider::MyGpsMeasurementsProvider::RegisterWithService(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->Native_start_measurement_collection();
    return NOERROR;
}

ECode GpsLocationProvider::MyGpsMeasurementsProvider::UnregisterFromService()
{
    mHost->Native_stop_measurement_collection();
    return NOERROR;
}

//===========================================
//GpsLocationProvider::MyGpsNavigationMessageProvider
//===========================================
GpsLocationProvider::MyGpsNavigationMessageProvider::MyGpsNavigationMessageProvider(
    /* [in] */ GpsLocationProvider* host)
    : mHost(host)
{
}

ECode GpsLocationProvider::MyGpsNavigationMessageProvider::IsSupported(
    /* [out] */ Boolean* isSupported)
{
    VALIDATE_NOT_NULL(isSupported)
    *isSupported = GpsLocationProvider::Native_is_navigation_message_supported();
    return NOERROR;
}

ECode GpsLocationProvider::MyGpsNavigationMessageProvider::RegisterWithService(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->Native_start_navigation_message_collection();
    return NOERROR;
}

ECode GpsLocationProvider::MyGpsNavigationMessageProvider::UnregisterFromService()
{
    mHost->Native_stop_navigation_message_collection();
    return NOERROR;
}

//===========================================
//GpsLocationProvider::MyBroadcastReceiver
//===========================================

GpsLocationProvider::MyBroadcastReceiver::MyBroadcastReceiver(
    /* [in] */ GpsLocationProvider* host)
    : mHost(host)
{}

ECode GpsLocationProvider::MyBroadcastReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);

    if (DEBUG) Logger::D(TAG, "receive broadcast intent, action: %s", action.string());
    if (action.Equals(GpsLocationProvider::ALARM_WAKEUP)) {
        mHost->StartNavigating(FALSE);
    }
    else if (action.Equals(GpsLocationProvider::ALARM_TIMEOUT)) {
        mHost->Hibernate();
    }
#if 0 //TODO IIntents
    else if (action.Equals(IIntents::DATA_SMS_RECEIVED_ACTION)) {
        mHost->CheckSmsSuplInit(intent);
    }
    else if (action.Equals(IIntents::WAP_PUSH_RECEIVED_ACTION)) {
        mHost->CheckWapSuplInit(intent);
    }
#endif
    else if (action.Equals(IConnectivityManager::CONNECTIVITY_ACTION)) {
        Int32 networkState;
        Boolean b1;
        if (intent->GetBooleanExtra(IConnectivityManager::EXTRA_NO_CONNECTIVITY, FALSE, &b1), b1) {
            networkState = ILocationProvider::TEMPORARILY_UNAVAILABLE;
        }
        else {
            networkState = ILocationProvider::AVAILABLE;
        }

        // retrieve NetworkInfo result for this UID
        AutoPtr<IParcelable> p1;
        intent->GetParcelableExtra(IConnectivityManager::EXTRA_NETWORK_INFO,(IParcelable**)&p1);
        AutoPtr<INetworkInfo> info = INetworkInfo::Probe(p1);
        AutoPtr<IInterface> serviceTemp;
        mHost->mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&serviceTemp);
        AutoPtr<IConnectivityManager> connManager = IConnectivityManager::Probe(serviceTemp);
        Int32 type;
        info->GetType(&type);
        info = NULL;
        connManager->GetNetworkInfo(type, (INetworkInfo**)&info);
        mHost->UpdateNetworkState(networkState, info);
    }
    else if (IPowerManager::ACTION_POWER_SAVE_MODE_CHANGED.Equals(action)
        || IIntent::ACTION_SCREEN_OFF.Equals(action)
        || IIntent::ACTION_SCREEN_ON.Equals(action)) {
        mHost->UpdateLowPowerMode();
    }
    else if (action.Equals(GpsLocationProvider::SIM_STATE_CHANGED)
        || action.Equals(ITelephonyIntents::ACTION_SUBINFO_CONTENT_CHANGE)
        || action.Equals(ITelephonyIntents::ACTION_SUBINFO_RECORD_UPDATED)) {
        Logger::D(TAG, "received SIM realted action: %s", action.string());
        AutoPtr<IInterface> serviceTemp;
        mHost->mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&serviceTemp);
        AutoPtr<ITelephonyManager> phone = ITelephonyManager::Probe(serviceTemp);
        String mccMnc;
        phone->GetSimOperator(&mccMnc);
        if (!TextUtils::IsEmpty(mccMnc)) {
            Logger::D(TAG, "SIM MCC/MNC is available: %s", mccMnc.string());
            {    AutoLock syncLock(this);
                mHost->ReloadGpsProperties(context, mHost->mProperties);
                mHost->mNIHandler->SetSuplEsEnabled(mHost->mSuplEsEnabled);
            }
        }
        else {
            Logger::D(TAG, "SIM MCC/MNC is still not available");
        }
    }
    return NOERROR;
}

//===========================================
//GpsLocationProvider::GpsGeofenceHardwareService
//===========================================

CAR_INTERFACE_IMPL_2(GpsLocationProvider::GpsGeofenceHardwareService, Object, IIGpsGeofenceHardware, IBinder)

ECode GpsLocationProvider::GpsGeofenceHardwareService::constructor(
    /* [in] */ ILocationProviderInterface* host)
{
    mHost = (GpsLocationProvider*)host;
    return NOERROR;
}

ECode GpsLocationProvider::GpsGeofenceHardwareService::IsHardwareGeofenceSupported(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->Native_is_geofence_supported();
    return NOERROR;
}

ECode GpsLocationProvider::GpsGeofenceHardwareService::AddCircularHardwareGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Double radius,
    /* [in] */ Int32 lastTransition,
    /* [in] */ Int32 monitorTransitions,
    /* [in] */ Int32 notificationResponsiveness,
    /* [in] */ Int32 unknownTimer,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->Native_add_geofence(geofenceId, latitude, longitude, radius,
            lastTransition, monitorTransitions, notificationResponsiveness, unknownTimer);
    return NOERROR;
}

ECode GpsLocationProvider::GpsGeofenceHardwareService::RemoveHardwareGeofence(
    /* [in] */ Int32 geofenceId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->Native_remove_geofence(geofenceId);
    return NOERROR;
}

ECode GpsLocationProvider::GpsGeofenceHardwareService::PauseHardwareGeofence(
    /* [in] */ Int32 geofenceId,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->Native_pause_geofence(geofenceId);
    return NOERROR;
}

ECode GpsLocationProvider::GpsGeofenceHardwareService::ResumeHardwareGeofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 monitorTransition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHost->Native_resume_geofence(geofenceId, monitorTransition);
    return NOERROR;
}

//===========================================
//GpsLocationProvider::NetInitiatedListener
//===========================================

CAR_INTERFACE_IMPL_2(GpsLocationProvider::NetInitiatedListener, Object, IINetInitiatedListener, IBinder)

ECode GpsLocationProvider::NetInitiatedListener::constructor(
    /* [in] */ ILocationProviderInterface* host)
{
    mHost = (GpsLocationProvider*)host;
    return NOERROR;
}

ECode GpsLocationProvider::NetInitiatedListener::SendNiResponse(
    /* [in] */ Int32 notificationId,
    /* [in] */ Int32 userResponse,
    /* [out] */ Boolean* result)
{
    // TODO Add Permission check
    VALIDATE_NOT_NULL(result)

    if (DEBUG) Logger::D(TAG, "sendNiResponse, notifId: %d, response: %d", notificationId, userResponse);
    mHost->Native_send_ni_response(notificationId, userResponse);
    *result = TRUE;
    return NOERROR;
}

ECode GpsLocationProvider::NetInitiatedListener::ToString(
    /* [out] */ String* s)
{
    return NOERROR;
}

//===========================================
//GpsLocationProvider::ProviderHandler
//===========================================

GpsLocationProvider::ProviderHandler::ProviderHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ GpsLocationProvider* host)
    : Handler(looper, NULL, TRUE /*async*/)
    , mHost(host)
{
}

ECode GpsLocationProvider::ProviderHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 message;
    msg->GetWhat(&message);
    switch (message) {
        case ENABLE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (arg1 == 1) {
                mHost->HandleEnable();
            }
            else {
                mHost->HandleDisable();
            }
            break;
        }
        case SET_REQUEST:{
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<GpsRequest> gpsRequest = (GpsRequest*)(IObject::Probe(obj));
            mHost->HandleSetRequest(gpsRequest->mRequest, gpsRequest->mSource);
            break;
        }
        case UPDATE_NETWORK_STATE: {
            Int32 arg1;
            msg->GetArg1(&arg1);
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->HandleUpdateNetworkState(arg1, INetworkInfo::Probe(obj));
            break;
        }
        case INJECT_NTP_TIME:
            mHost->HandleInjectNtpTime();
            break;
        case DOWNLOAD_XTRA_DATA:
            if (mHost->mSupportsXtra) {
                mHost->HandleDownloadXtraData();
            }
            break;
        case INJECT_NTP_TIME_FINISHED:
            mHost->mInjectNtpTimePending = STATE_IDLE;
            break;
        case DOWNLOAD_XTRA_DATA_FINISHED:
            mHost->mDownloadXtraDataPending = STATE_IDLE;
            break;
        case UPDATE_LOCATION: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            mHost->HandleUpdateLocation(ILocation::Probe(obj));
            break;
        }
    }
    Int32 arg2;
    msg->GetArg2(&arg2);
    if (arg2 == 1) {
        // wakelock was taken for this message, release it
        mHost->mWakeLock->ReleaseLock();
    }
    return NOERROR;
}

//===========================================
//GpsLocationProvider::DefaultApnObserver
//===========================================

GpsLocationProvider::DefaultApnObserver::DefaultApnObserver(
    /* [in] */ GpsLocationProvider* host)
    : mHost(host)
{
    ContentObserver::constructor(host->mHandler);
}

ECode GpsLocationProvider::DefaultApnObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    mHost->mDefaultApn = mHost->GetDefaultApn();
    if (DEBUG) Logger::D(TAG, "Observer mDefaultApn=%s", mHost->mDefaultApn.string());
    return NOERROR;
}

//===========================================
//GpsLocationProvider::NetworkLocationListener
//===========================================

CAR_INTERFACE_IMPL(GpsLocationProvider::NetworkLocationListener, Object, ILocationListener)

GpsLocationProvider::NetworkLocationListener::NetworkLocationListener(
    /* [in] */ GpsLocationProvider* host)
    : mHost(host)
{}

ECode GpsLocationProvider::NetworkLocationListener::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    // this callback happens on mHandler looper
    String provider;
    location->GetProvider(&provider);
    if (ILocationManager::NETWORK_PROVIDER.Equals(provider)) {
        mHost->HandleUpdateLocation(location);
    }
    return NOERROR;
}

ECode GpsLocationProvider::NetworkLocationListener::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}

ECode GpsLocationProvider::NetworkLocationListener::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

ECode GpsLocationProvider::NetworkLocationListener::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

//===========================================
//GpsLocationProvider
//===========================================

const String GpsLocationProvider::TAG("GpsLocationProvider");
const Boolean GpsLocationProvider::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
const Boolean GpsLocationProvider::VERBOSE = Logger::IsLoggable(TAG, Logger::VERBOSE);
const AutoPtr<IProviderProperties> GpsLocationProvider::PROPERTIES = InitPROPERTIES();

// // these need to match GpsPositionMode enum in gps.h
// const Int32 GpsLocationProvider::GPS_POSITION_MODE_STANDALONE;
// const Int32 GpsLocationProvider::GPS_POSITION_MODE_MS_BASED;
// const Int32 GpsLocationProvider::GPS_POSITION_MODE_MS_ASSISTED;

// // these need to match GpsPositionRecurrence enum in gps.h
// const Int32 GpsLocationProvider::GPS_POSITION_RECURRENCE_PERIODIC;
// const Int32 GpsLocationProvider::GPS_POSITION_RECURRENCE_SINGLE;

// // these need to match GpsStatusValue defines in gps.h
// const Int32 GpsLocationProvider::GPS_STATUS_NONE;
// const Int32 GpsLocationProvider::GPS_STATUS_SESSION_BEGIN;
// const Int32 GpsLocationProvider::GPS_STATUS_SESSION_END;
// const Int32 GpsLocationProvider::GPS_STATUS_ENGINE_ON;
// const Int32 GpsLocationProvider::GPS_STATUS_ENGINE_OFF;

// // these need to match GpsApgsStatusValue defines in gps.h
// /** AGPS status event values. */
// const Int32 GpsLocationProvider::GPS_REQUEST_AGPS_DATA_CONN;
// const Int32 GpsLocationProvider::GPS_RELEASE_AGPS_DATA_CONN;
// const Int32 GpsLocationProvider::GPS_AGPS_DATA_CONNECTED;
// const Int32 GpsLocationProvider::GPS_AGPS_DATA_CONN_DONE;
// const Int32 GpsLocationProvider::GPS_AGPS_DATA_CONN_FAILED;

// these need to match GpsLocationFlags enum in gps.h
const Int32 GpsLocationProvider::LOCATION_INVALID;
const Int32 GpsLocationProvider::LOCATION_HAS_LAT_LONG;
const Int32 GpsLocationProvider::LOCATION_HAS_ALTITUDE;
const Int32 GpsLocationProvider::LOCATION_HAS_SPEED;
const Int32 GpsLocationProvider::LOCATION_HAS_BEARING;
const Int32 GpsLocationProvider::LOCATION_HAS_ACCURACY;

// // IMPORTANT - the GPS_DELETE_* symbols here must match constants in gps.h
// const Int32 GpsLocationProvider::GPS_DELETE_EPHEMERIS;
// const Int32 GpsLocationProvider::GPS_DELETE_ALMANAC;
// const Int32 GpsLocationProvider::GPS_DELETE_POSITION;
// const Int32 GpsLocationProvider::GPS_DELETE_TIME;
// const Int32 GpsLocationProvider::GPS_DELETE_IONO;
// const Int32 GpsLocationProvider::GPS_DELETE_UTC;
// const Int32 GpsLocationProvider::GPS_DELETE_HEALTH;
// const Int32 GpsLocationProvider::GPS_DELETE_SVDIR;
// const Int32 GpsLocationProvider::GPS_DELETE_SVSTEER;
// const Int32 GpsLocationProvider::GPS_DELETE_SADATA;
// const Int32 GpsLocationProvider::GPS_DELETE_RTI;
// const Int32 GpsLocationProvider::GPS_DELETE_CELLDB_INFO;
// const Int32 GpsLocationProvider::GPS_DELETE_ALMANAC_CORR;
// const Int32 GpsLocationProvider::GPS_DELETE_FREQ_BIAS_EST;
// const Int32 GpsLocationProvider::GLO_DELETE_EPHEMERIS;
// const Int32 GpsLocationProvider::GLO_DELETE_ALMANAC;
// const Int32 GpsLocationProvider::GLO_DELETE_SVDIR;
// const Int32 GpsLocationProvider::GLO_DELETE_SVSTEER;
// const Int32 GpsLocationProvider::GLO_DELETE_ALMANAC_CORR;
// const Int32 GpsLocationProvider::GPS_DELETE_TIME_GPS;
// const Int32 GpsLocationProvider::GLO_DELETE_TIME;
// const Int32 GpsLocationProvider::BDS_DELETE_SVDIR;
// const Int32 GpsLocationProvider::BDS_DELETE_SVSTEER;
// const Int32 GpsLocationProvider::BDS_DELETE_TIME;
// const Int32 GpsLocationProvider::BDS_DELETE_ALMANAC_CORR;
// const Int32 GpsLocationProvider::BDS_DELETE_EPHEMERIS;
// const Int32 GpsLocationProvider::BDS_DELETE_ALMANAC;
// const Int32 GpsLocationProvider::GPS_DELETE_ALL;

// // The GPS_CAPABILITY_* flags must match the values in gps.h
// const Int32 GpsLocationProvider::GPS_CAPABILITY_SCHEDULING;
// const Int32 GpsLocationProvider::GPS_CAPABILITY_MSB;
// const Int32 GpsLocationProvider::GPS_CAPABILITY_MSA;
// const Int32 GpsLocationProvider::GPS_CAPABILITY_SINGLE_SHOT;
// const Int32 GpsLocationProvider::GPS_CAPABILITY_ON_DEMAND_TIME;

// The AGPS SUPL mode
const Int32 GpsLocationProvider::AGPS_SUPL_MODE_MSA;
const Int32 GpsLocationProvider::AGPS_SUPL_MODE_MSB;

// // these need to match AGpsType enum in gps.h
// const Int32 GpsLocationProvider::AGPS_TYPE_SUPL;
// const Int32 GpsLocationProvider::AGPS_TYPE_C2K;

// these must match the definitions in gps.h
const Int32 GpsLocationProvider::APN_INVALID;
const Int32 GpsLocationProvider::APN_IPV4;
const Int32 GpsLocationProvider::APN_IPV6;
const Int32 GpsLocationProvider::APN_IPV4V6;

// for mAGpsDataConnectionState
const Int32 GpsLocationProvider::AGPS_DATA_CONNECTION_CLOSED;
const Int32 GpsLocationProvider::AGPS_DATA_CONNECTION_OPENING;
const Int32 GpsLocationProvider::AGPS_DATA_CONNECTION_OPEN;

// Handler messages
const Int32 GpsLocationProvider::CHECK_LOCATION;
const Int32 GpsLocationProvider::ENABLE;
const Int32 GpsLocationProvider::SET_REQUEST;
const Int32 GpsLocationProvider::UPDATE_NETWORK_STATE;
const Int32 GpsLocationProvider::INJECT_NTP_TIME;
const Int32 GpsLocationProvider::DOWNLOAD_XTRA_DATA;
const Int32 GpsLocationProvider::UPDATE_LOCATION;
const Int32 GpsLocationProvider::ADD_LISTENER;
const Int32 GpsLocationProvider::REMOVE_LISTENER;
const Int32 GpsLocationProvider::INJECT_NTP_TIME_FINISHED;
const Int32 GpsLocationProvider::DOWNLOAD_XTRA_DATA_FINISHED;

// Request setid
// const Int32 GpsLocationProvider::AGPS_RIL_REQUEST_SETID_IMSI;
// const Int32 GpsLocationProvider::AGPS_RIL_REQUEST_SETID_MSISDN;

// Request ref location
// const Int32 GpsLocationProvider::AGPS_RIL_REQUEST_REFLOC_CELLID;
// const Int32 GpsLocationProvider::AGPS_RIL_REQUEST_REFLOC_MAC;

// // ref. location info
// const Int32 GpsLocationProvider::AGPS_REF_LOCATION_TYPE_GSM_CELLID;
// const Int32 GpsLocationProvider::AGPS_REF_LOCATION_TYPE_UMTS_CELLID;
// const Int32 GpsLocationProvider::AGPS_REG_LOCATION_TYPE_MAC       ;

// // set id info
// const Int32 GpsLocationProvider::AGPS_SETID_TYPE_NONE;
// const Int32 GpsLocationProvider::AGPS_SETID_TYPE_IMSI;
// const Int32 GpsLocationProvider::AGPS_SETID_TYPE_MSISDN;

const String GpsLocationProvider::PROPERTIES_FILE_PREFIX("/etc/gps");
const String GpsLocationProvider::PROPERTIES_FILE_SUFFIX(".conf");
const String GpsLocationProvider::DEFAULT_PROPERTIES_FILE = PROPERTIES_FILE_PREFIX + PROPERTIES_FILE_SUFFIX;

// const Int32 GpsLocationProvider::GPS_GEOFENCE_UNAVAILABLE;
// const Int32 GpsLocationProvider::GPS_GEOFENCE_AVAILABLE;

// // GPS Geofence errors. Should match gps.h constants.
// const Int32 GpsLocationProvider::GPS_GEOFENCE_OPERATION_SUCCESS;
// const Int32 GpsLocationProvider::GPS_GEOFENCE_ERROR_TOO_MANY_GEOFENCES;
// const Int32 GpsLocationProvider::GPS_GEOFENCE_ERROR_ID_EXISTS;
// const Int32 GpsLocationProvider::GPS_GEOFENCE_ERROR_ID_UNKNOWN;
// const Int32 GpsLocationProvider::GPS_GEOFENCE_ERROR_INVALID_TRANSITION;
// const Int32 GpsLocationProvider::GPS_GEOFENCE_ERROR_GENERIC;

// TCP/IP constants.
// Valid TCP/UDP port range is (0, 65535].
const Int32 GpsLocationProvider::TCP_MIN_PORT;
const Int32 GpsLocationProvider::TCP_MAX_PORT;

// Value of batterySaverGpsMode such that GPS isn't affected by battery saver mode.
const Int32 GpsLocationProvider::BATTERY_SAVER_MODE_NO_CHANGE;
// Value of batterySaverGpsMode such that GPS is disabled when battery saver mode
// is enabled and the screen is off.
const Int32 GpsLocationProvider::BATTERY_SAVER_MODE_DISABLED_WHEN_SCREEN_OFF;
// Secure setting for GPS behavior when battery saver mode is on.
const String GpsLocationProvider::BATTERY_SAVER_GPS_MODE("batterySaverGpsMode");
const String GpsLocationProvider::WAKELOCK_KEY("GpsLocationProvider");

// Alarms
const String GpsLocationProvider::ALARM_WAKEUP("com.android.internal.location.ALARM_WAKEUP");
const String GpsLocationProvider::ALARM_TIMEOUT("com.android.internal.location.ALARM_TIMEOUT");

// SIM/Carrier info.
const String GpsLocationProvider::SIM_STATE_CHANGED("android.intent.action.SIM_STATE_CHANGED");

const Int32 GpsLocationProvider::MAX_SVS;
const Int32 GpsLocationProvider::EPHEMERIS_MASK;
const Int32 GpsLocationProvider::ALMANAC_MASK;
const Int32 GpsLocationProvider::USED_FOR_FIX_MASK;

Boolean GpsLocationProvider::class_init_Native_value = GpsLocationProvider::Native_Class_init();

CAR_INTERFACE_IMPL(GpsLocationProvider, Object, ILocationProviderInterface)

GpsLocationProvider::GpsLocationProvider(
    /* [in] */ IContext* context,
    /* [in] */ IILocationManager* ilocationManager,
    /* [in] */ ILooper* looper)
    : mLocationFlags(LOCATION_INVALID)
    , mStatus(ILocationProvider::TEMPORARILY_UNAVAILABLE)
    , mStatusUpdateTime(SystemClock::GetElapsedRealtime())
    , mEnabled(FALSE)
    , mNetworkAvailable(FALSE)
    , mInjectNtpTimePending(STATE_PENDING_NETWORK)
    , mDownloadXtraDataPending(STATE_PENDING_NETWORK)
    , mPeriodicTimeInjection(FALSE)
    , mNavigating(FALSE)
    , mEngineOn(FALSE)
    , mFixInterval(1000)
    , mStarted(FALSE)
    , mSingleShot(FALSE)
    , mEngineCapabilities(0)
    , mSupportsXtra(FALSE)
    , mFixRequestTime(0)
    , mTimeToFirstFix(0)
    , mLastFixTime(0L)
    , mPositionMode(0)
    , mDisableGps(FALSE)
    , mSuplServerPort(TCP_MIN_PORT)
    , mC2KServerPort(0)
    , mSuplEsEnabled(FALSE)
    , mContext(context)
    , mILocationManager(ilocationManager)
    , mApnIpType(0)
    , mAGpsDataConnectionState(0)
    , mSvCount(0)
{
    CLocation::New(ILocationManager::GPS_PROVIDER, (ILocation**)&mLocation);

    CBundle::New((IBundle**)&mLocationExtras);

    mListenerHelper = (GpsStatusListenerHelper*)(new MyListenerHelper());

    CWorkSource::New((IWorkSource**)&mClientSource);

    CGpsStatusProviderService::New(this, (IIGpsStatusProvider**)&mGpsStatusProvider);

    mGpsMeasurementsProvider = (GpsMeasurementsProvider*)(new MyGpsMeasurementsProvider(this));
    mGpsNavigationMessageProvider = (GpsNavigationMessageProvider*)(new MyGpsNavigationMessageProvider(this));
    mBroadcastReceiver = new MyBroadcastReceiver(this);

    CGpsGeofenceHardwareService::New(this, (IIGpsGeofenceHardware**)&mGpsGeofenceBinder);
    CNetInitiatedListener::New(this, (IINetInitiatedListener**)&mNetInitiatedListener);

    mSvs = ArrayOf<Int32>::Alloc(MAX_SVS);
    mSnrs = ArrayOf<Float>::Alloc(MAX_SVS);
    mSvElevations = ArrayOf<Float>::Alloc(MAX_SVS);
    mSvAzimuths = ArrayOf<Float>::Alloc(MAX_SVS);
    mSvMasks = ArrayOf<Int32>::Alloc(3);
    mNmeaBuffer = ArrayOf<Byte>::Alloc(120);
    mDefaultApnObserver = new DefaultApnObserver(this);

    AutoPtr<INtpTrustedTimeHelper> ntth;
    CNtpTrustedTimeHelper::AcquireSingleton((INtpTrustedTimeHelper**)&ntth);
    ntth->GetInstance(context, (INtpTrustedTime**)&mNtpTime);

    mLocation->SetExtras(mLocationExtras);

    // Create a wake lock
    AutoPtr<IInterface> pmObj;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&pmObj);
    mPowerManager = IPowerManager::Probe(pmObj);
    mPowerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, WAKELOCK_KEY, (IPowerManagerWakeLock**)&mWakeLock);

    mWakeLock->SetReferenceCounted(TRUE);

    AutoPtr<IInterface> amObj;
    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&amObj);
    mAlarmManager = IAlarmManager::Probe(amObj);

    AutoPtr<IIntent> i1, i2;
    CIntent::New(ALARM_WAKEUP, (IIntent**)&i1);
    CIntent::New(ALARM_TIMEOUT, (IIntent**)&i2);
    AutoPtr<IPendingIntentHelper> pih;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
    pih->GetBroadcast(mContext, 0, i1, 0, (IPendingIntent**)&mWakeupIntent);
    pih->GetBroadcast(mContext, 0, i2, 0, (IPendingIntent**)&mTimeoutIntent);

    AutoPtr<IInterface> cmObj;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&cmObj);
    mConnMgr = IConnectivityManager::Probe(cmObj);

    // App ops service to keep track of who is accessing the GPS
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> aosObj;
    sm->GetService(IContext::APP_OPS_SERVICE, (IInterface**)&aosObj);
    mAppOpsService = IIAppOpsService::Probe(aosObj);

    // Battery statistics service to be notified when GPS turns on or off
    AutoPtr<IInterface> bsObj;
    sm->GetService(IBatteryStats::SERVICE_NAME, (IInterface**)&bsObj);
    mBatteryStats = IIBatteryStats::Probe(bsObj);

    // Load GPS configuration.
    CProperties::New((IProperties**)&mProperties);
    ReloadGpsProperties(mContext, mProperties);

    // Create a GPS net-initiated handler.
    CGpsNetInitiatedHandler::New(context, mNetInitiatedListener, mSuplEsEnabled, (IGpsNetInitiatedHandler**)&mNIHandler);

    // construct handler, listen for events
    mHandler = new ProviderHandler(looper, this);
    ListenForBroadcasts();

    // also listen for PASSIVE_PROVIDER updates
    Boolean res;
    mHandler->Post((IRunnable*)(new MyRunnable(this)), &res);
}

ECode GpsLocationProvider::GetGpsStatusProvider(
    /* [out] */ IIGpsStatusProvider** gsp)
{
    VALIDATE_NOT_NULL(gsp)
    *gsp = mGpsStatusProvider.Get();
    REFCOUNT_ADD(*gsp)
    return NOERROR;
}

ECode GpsLocationProvider::GetGpsGeofenceProxy(
    /* [out] */ IIGpsGeofenceHardware** ggh)
{
    VALIDATE_NOT_NULL(ggh)
    *ggh = mGpsGeofenceBinder.Get();
    REFCOUNT_ADD(*ggh)
    return NOERROR;
}

ECode GpsLocationProvider::GetGpsMeasurementsProvider(
    /* [out] */ GpsMeasurementsProvider** gmp)
{
    VALIDATE_NOT_NULL(gmp)
    *gmp = mGpsMeasurementsProvider.Get();
    REFCOUNT_ADD(*gmp)
    return NOERROR;
}

ECode GpsLocationProvider::GetGpsNavigationMessageProvider(
    /* [out] */ GpsNavigationMessageProvider** gnmp)
{
    VALIDATE_NOT_NULL(gnmp)
    *gnmp = mGpsNavigationMessageProvider.Get();
    REFCOUNT_ADD(*gnmp)
    return NOERROR;
}

void GpsLocationProvider::CheckSmsSuplInit(
    /* [in] */ IIntent* intent)
{
#if 0 //TODO
    SmsMessage[] messages = Intents.getMessagesFromIntent(intent);
    for (int i=0; i <messages.length; i++) {
        byte[] supl_init = messages[i].getUserData();
        Native_agps_ni_message(supl_init,supl_init.length);
    }
#endif
}

void GpsLocationProvider::CheckWapSuplInit(
    /* [in] */ IIntent* intent)
{
    AutoPtr<ArrayOf<Byte> > supl_init;
    intent->GetByteArrayExtra(String("data"), (ArrayOf<Byte>**)&supl_init);
    Native_agps_ni_message(supl_init.Get(), supl_init->GetLength());
}

void GpsLocationProvider::UpdateLowPowerMode()
{
    Boolean disableGps;
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsSecure> ss;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&ss);
    Int32 v;
    ss->GetInt32(cr.Get(), BATTERY_SAVER_GPS_MODE, BATTERY_SAVER_MODE_DISABLED_WHEN_SCREEN_OFF, &v);
    switch (v) {
        case BATTERY_SAVER_MODE_DISABLED_WHEN_SCREEN_OFF:
            Boolean isPowerSaveMode, isInteractive;
            mPowerManager->IsPowerSaveMode(&isPowerSaveMode);
            mPowerManager->IsInteractive(&isInteractive);
            disableGps = isPowerSaveMode && !isInteractive;
            break;
        default:
            disableGps = FALSE;
    }
    if (disableGps != mDisableGps) {
        mDisableGps = disableGps;
        UpdateRequirements();
    }
}

Boolean GpsLocationProvider::IsSupported()
{
    return Native_is_supported();
}

void GpsLocationProvider::ReloadGpsProperties(
    /* [in] */ IContext* context,
    /* [in] */ IProperties* properties)
{
    Int32 size;
    IHashTable::Probe(properties)->GetSize(&size);
    Logger::D(TAG, "Reset GPS properties, previous size = %d", size);
    LoadPropertiesFromResource(context, properties);
    Boolean isPropertiesLoadedFromFile = FALSE;
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String gh;
    sp->Get(String("ro.hardware.gps"), &gh);
    const String gpsHardware = gh;
    if (!TextUtils::IsEmpty(gpsHardware)) {
        const String propFilename =
                PROPERTIES_FILE_PREFIX + "." + gpsHardware + PROPERTIES_FILE_SUFFIX;
        isPropertiesLoadedFromFile =
                LoadPropertiesFromFile(propFilename, properties);
    }
    if (!isPropertiesLoadedFromFile) {
        LoadPropertiesFromFile(DEFAULT_PROPERTIES_FILE, properties);
    }
    IHashTable::Probe(properties)->GetSize(&size);
    Logger::D(TAG, "GPS properties reloaded, size = %d", size);

    // TODO: we should get rid of C2K specific setting.
    String s1, s2;
    properties->GetProperty(String("SUPL_HOST"), &s1);
    properties->GetProperty(String("SUPL_PORT"), &s2);
    SetSuplHostPort(s1, s2);
    properties->GetProperty(String("C2K_HOST"), &mC2KServerHost);
    String portString;
    properties->GetProperty(String("C2K_PORT"), &portString);
    if (!mC2KServerHost.IsNull() && !portString.IsNull()) {
        ECode ec = mC2KServerPort = StringUtils::ParseInt32(portString);
        if (FAILED(ec)) {
            Logger::E(TAG, "unable to parse C2K_PORT: %s", portString.string());
            // return E_NUMBER_FORMAT_EXCEPTION;
        }
    }

    // Convert properties to string contents and send it to HAL.
    AutoPtr<IByteArrayOutputStream> baos;
    CByteArrayOutputStream::New(4096, (IByteArrayOutputStream**)&baos);
    ECode ec = properties->Store(IOutputStream::Probe(baos), String(NULL));
    if (FAILED(ec)) {
        Logger::W(TAG, "failed to dump properties contents");
        // return E_IO_EXCEPTION;
    }
    String str;
    baos->ToString(&str);
    Native_configuration_update(str);
    Logger::D(TAG, "final config = %s", str.string());

    // SUPL_ES configuration.
    String suplESProperty;
    mProperties->GetProperty(String("SUPL_ES"), &suplESProperty);
    if (!suplESProperty.IsNull()) {
        // try {
        mSuplEsEnabled = (StringUtils::ParseInt32(suplESProperty) == 1);
        // } catch (NumberFormatException e) {
        //     Log.e(TAG, "unable to parse SUPL_ES: " + suplESProperty);
        // }
    }
}

void GpsLocationProvider::LoadPropertiesFromResource(
    /* [in] */ IContext* context,
    /* [in] */ IProperties* properties)
{
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<ArrayOf<String> > configValues;
    res->GetStringArray(R::array::config_gpsParameters, (ArrayOf<String>**)&configValues);
    for (Int32 i = 0; i < configValues->GetLength(); i++) {
        String item = (*configValues)[i];
        Logger::D(TAG, "GpsParamsResource: %s", item.string());
        // We need to support "KEY =", but not "=VALUE".
        AutoPtr<ArrayOf<String> > split;
        StringUtils::Split(item, "=", (ArrayOf<String>**)&split);
        if (split->GetLength() == 2) {
            String s;
            properties->SetProperty((*split)[0].Trim().ToUpperCase(), (*split)[1], &s);
        }
        else {
            Logger::W(TAG, "malformed contents: %s", item.string());
        }
    }
}

Boolean GpsLocationProvider::LoadPropertiesFromFile(
    /* [in] */ const String& filename,
    /* [in] */ IProperties* properties)
{

    AutoPtr<IFile> file;
    CFile::New(filename, (IFile**)&file);
    AutoPtr<IFileInputStream> stream;
    CFileInputStream::New(file, (IFileInputStream**)&stream);
    ECode ec = properties->Load(IInputStream::Probe(stream));
    if (FAILED(ec)) {
        Logger::W(TAG, "Could not open GPS configuration file %s", filename.string());
        return FALSE;
    }
    AutoPtr<IIoUtils> iu;
    CIoUtils::AcquireSingleton((IIoUtils**)&iu);
    iu->CloseQuietly(ICloseable::Probe(stream));
    return TRUE;
}

void GpsLocationProvider::ListenForBroadcasts()
{
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    //TODO IIntents
    // intentFilter->AddAction(IIntents::DATA_SMS_RECEIVED_ACTION);
    intentFilter->AddDataScheme(String("sms"));
    intentFilter->AddDataAuthority(String("localhost"), String("7275"));
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mBroadcastReceiver, intentFilter, String(NULL), mHandler, (IIntent**)&intent);

    intentFilter = NULL;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    //TODO IIntents
    // intentFilter->AddAction(IIntents::WAP_PUSH_RECEIVED_ACTION);
    ECode ec = intentFilter->AddDataType(String("application/vnd.omaloc-supl-init"));
    if (FAILED(ec)) {
        Logger::W(TAG, "Malformed SUPL init mime type");
        // return E_INTENTFILTER_MALFORMED_MIME_TYPE_EXCEPTION;
    }
    intent = NULL;
    mContext->RegisterReceiver(mBroadcastReceiver, intentFilter, String(NULL), mHandler, (IIntent**)&intent);

    intentFilter = NULL;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(ALARM_WAKEUP);
    intentFilter->AddAction(ALARM_TIMEOUT);
    intentFilter->AddAction(IConnectivityManager::CONNECTIVITY_ACTION);
    intentFilter->AddAction(IPowerManager::ACTION_POWER_SAVE_MODE_CHANGED);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_OFF);
    intentFilter->AddAction(IIntent::ACTION_SCREEN_ON);
    intentFilter->AddAction(SIM_STATE_CHANGED);
    // TODO: remove the use TelephonyIntents. We are using it because SIM_STATE_CHANGED
    // is not reliable at the moment.
    intentFilter->AddAction(ITelephonyIntents::ACTION_SUBINFO_CONTENT_CHANGE);
    intentFilter->AddAction(ITelephonyIntents::ACTION_SUBINFO_RECORD_UPDATED);
    intent = NULL;
    mContext->RegisterReceiver(mBroadcastReceiver, intentFilter, String(NULL), mHandler, (IIntent**)&intent);
    AutoPtr<IUriHelper> uriHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&uriHelper);
    AutoPtr<IUri> uri;
    uriHelper->Parse(String("content://telephony/carriers/preferapn"), (IUri**)&uri);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    cr->RegisterContentObserver(uri, FALSE, mDefaultApnObserver);
}

ECode GpsLocationProvider::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = ILocationManager::GPS_PROVIDER;
    return NOERROR;
}

ECode GpsLocationProvider::GetProperties(
    /* [out] */ IProviderProperties** properties)
{
    // VALIDATE_NOT_NULL(properties)
    // *properties = PROPERTIES;
    // REFCOUNT_ADD(*properties)
    return NOERROR;
}

ECode GpsLocationProvider::UpdateNetworkState(
    /* [in] */ Int32 state,
    /* [in] */ INetworkInfo* info)
{
    SendMessage(UPDATE_NETWORK_STATE, state, info);
    return NOERROR;
}

void GpsLocationProvider::HandleUpdateNetworkState(
    /* [in] */ Int32 state,
    /* [in] */ INetworkInfo* info)
{
    mNetworkAvailable = (state == ILocationProvider::AVAILABLE);

    if (DEBUG) {
        String infoStr;
        IObject::Probe(info)->ToString(&infoStr);
        Logger::D(TAG, "updateNetworkState %s info: %s",
            (mNetworkAvailable ? "available" : "unavailable"), infoStr.string());
    }

    if (info != NULL) {
        AutoPtr<ISettingsGlobal> sg;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        Int32 v;
        sg->GetInt32(cr, ISettingsGlobal::MOBILE_DATA, 1, &v);
        Boolean dataEnabled = v == 1;
        Boolean isAvailable;
        info->IsAvailable(&isAvailable);
        Boolean networkAvailable = isAvailable && dataEnabled;
        if (mDefaultApn == NULL) {
            mDefaultApn = GetDefaultApn();
        }

        Boolean isConnected;
        info->IsConnected(&isConnected);
        Int32 type;
        info->GetType(&type);
        Boolean isRoaming;
        info->IsRoaming(&isRoaming);
        String extraInfo;
        info->GetExtraInfo(&extraInfo);
        Native_update_network_state(isConnected, type, isRoaming, networkAvailable,
            extraInfo, mDefaultApn);
    }
    Int32 type;
    info->GetType(&type);
    if (info != NULL && type == IConnectivityManager::TYPE_MOBILE_SUPL
        && mAGpsDataConnectionState == AGPS_DATA_CONNECTION_OPENING) {
        if (mNetworkAvailable) {
            String apnName;
            info->GetExtraInfo(&apnName);
            if (apnName.IsNull()) {
                /* Assign a dummy value in the case of C2K as otherwise we will have a runtime
                exception in the following call to Native_agps_data_conn_open*/
                apnName = "dummy-apn";
            }
            mAGpsApn = apnName;
            mApnIpType = GetApnIpType(apnName);
            SetRouting();
            if (DEBUG) {
                StringBuilder sb("Native_agps_data_conn_open: mAgpsApn=");
                //TODO
                // sb += mAGpsApn;
                sb += ", mApnIpType=";
                sb += mApnIpType;
                String message = sb.ToString();
                Logger::D(TAG, message.string());
            }
            Native_agps_data_conn_open(mAGpsApn, mApnIpType);
            mAGpsDataConnectionState = AGPS_DATA_CONNECTION_OPEN;
        }
        else {
            String s;
            IObject::Probe(info)->ToString(&s);
            Logger::E(TAG, "call Native_agps_data_conn_failed, info: %s", s.string());
            mAGpsApn = NULL;
            mApnIpType = APN_INVALID;
            mAGpsDataConnectionState = AGPS_DATA_CONNECTION_CLOSED;
            Native_agps_data_conn_failed();
        }
    }

    if (mNetworkAvailable) {
        if (mInjectNtpTimePending == STATE_PENDING_NETWORK) {
            SendMessage(INJECT_NTP_TIME, 0, NULL);
        }
        if (mDownloadXtraDataPending == STATE_PENDING_NETWORK) {
            SendMessage(DOWNLOAD_XTRA_DATA, 0, NULL);
        }
    }
}

void GpsLocationProvider::HandleInjectNtpTime()
{
    if (mInjectNtpTimePending == STATE_DOWNLOADING) {
        // already downloading data
        return;
    }
    if (!mNetworkAvailable) {
        // try again when network is up
        mInjectNtpTimePending = STATE_PENDING_NETWORK;
        return;
    }
    mInjectNtpTimePending = STATE_DOWNLOADING;

    // hold wake lock while task runs
    mWakeLock->AcquireLock();
    AsyncTask::THREAD_POOL_EXECUTOR->Execute((IRunnable*)(new MyRunnable2(this)));
}

void GpsLocationProvider::HandleDownloadXtraData()
{
    if (mDownloadXtraDataPending == STATE_DOWNLOADING) {
        // already downloading data
        return;
    }
    if (!mNetworkAvailable) {
        // try again when network is up
        mDownloadXtraDataPending = STATE_PENDING_NETWORK;
        return;
    }
    mDownloadXtraDataPending = STATE_DOWNLOADING;

    // hold wake lock while task runs
    mWakeLock->AcquireLock();
    AsyncTask::THREAD_POOL_EXECUTOR->Execute((IRunnable*)(new MyRunnable3(this)));
}

void GpsLocationProvider::HandleUpdateLocation(
    /* [in] */ ILocation* location)
{
    Boolean hasAccuracy;
    location->HasAccuracy(&hasAccuracy);
    if (hasAccuracy) {
        Double latitude, longitude;
        location->GetLatitude(&latitude);
        location->GetLongitude(&longitude);
        Float accuracy;
        location->GetAccuracy(&accuracy);
        Native_inject_location(latitude, longitude, accuracy);
    }
}

ECode GpsLocationProvider::Enable()
{
    {    AutoLock syncLock(this);
        if (mEnabled) return E_NULL_POINTER_EXCEPTION;
        mEnabled = TRUE;
    }

    SendMessage(ENABLE, 1, NULL);
    return NOERROR;
}

void GpsLocationProvider::SetSuplHostPort(
    /* [in] */ const String& hostString,
    /* [in] */ const String& portString)
{
    if (!hostString.IsNull()) {
        mSuplServerHost = hostString;
    }
    if (!portString.IsNull()) {
        // try {
        mSuplServerPort = StringUtils::ParseInt32(portString);
        // } catch (NumberFormatException e) {
            // Log.e(TAG, "unable to parse SUPL_PORT: " + portString);
        // }
    }
    if (!mSuplServerHost.IsNull()
        && mSuplServerPort > TCP_MIN_PORT
        && mSuplServerPort <= TCP_MAX_PORT) {
        Native_set_agps_server(AGPS_TYPE_SUPL, mSuplServerHost, mSuplServerPort);
    }
}

Int32 GpsLocationProvider::GetSuplMode(
    /* [in] */ IProperties* properties,
    /* [in] */ Boolean agpsEnabled,
    /* [in] */ Boolean singleShot)
{
    if (agpsEnabled) {
        String modeString;
        properties->GetProperty(String("SUPL_MODE"), &modeString);
        Int32 suplMode = 0;
        if (!TextUtils::IsEmpty(modeString)) {
            // try {
            suplMode = StringUtils::ParseInt32(modeString);
            // } catch (NumberFormatException e) {
            //     Log.e(TAG, "unable to parse SUPL_MODE: " + modeString);
            //     return GPS_POSITION_MODE_STANDALONE;
            // }
        }
        if (singleShot
            && HasCapability(GPS_CAPABILITY_MSA)
            && (suplMode & AGPS_SUPL_MODE_MSA) != 0) {
            return GPS_POSITION_MODE_MS_ASSISTED;
        }
        else if (HasCapability(GPS_CAPABILITY_MSB)
            && (suplMode & AGPS_SUPL_MODE_MSB) != 0) {
            return GPS_POSITION_MODE_MS_BASED;
        }
    }
    return GPS_POSITION_MODE_STANDALONE;
}

void GpsLocationProvider::HandleEnable()
{
    if (DEBUG) Logger::D(TAG, "handleEnable");

    Boolean enabled = Native_init();

    if (enabled) {
        mSupportsXtra = Native_supports_xtra();

        // TODO: remove the following Native calls if we can make sure they are redundant.
        if (!mSuplServerHost.IsNull()) {
            Native_set_agps_server(AGPS_TYPE_SUPL, mSuplServerHost, mSuplServerPort);
        }
        if (!mC2KServerHost.IsNull()) {
            Native_set_agps_server(AGPS_TYPE_C2K, mC2KServerHost, mC2KServerPort);
        }
    }
    else {
        {    AutoLock syncLock(this);
            mEnabled = FALSE;
        }
        Logger::W(TAG, "Failed to enable location provider");
    }
}

ECode GpsLocationProvider::Disable()
{
    {    AutoLock syncLock(this);
        if (!mEnabled) return E_NULL_POINTER_EXCEPTION;
        mEnabled = FALSE;
    }

    SendMessage(ENABLE, 0, NULL);
    return NOERROR;
}

void GpsLocationProvider::HandleDisable()
{
    if (DEBUG) Logger::D(TAG, "handleDisable");

    AutoPtr<IWorkSource> ws;
    CWorkSource::New((IWorkSource**)&ws);
    UpdateClientUids(ws);
    StopNavigating();
    mAlarmManager->Cancel(mWakeupIntent);
    mAlarmManager->Cancel(mTimeoutIntent);

    // do this before releasing wakelock
    Native_cleanup();
}

ECode GpsLocationProvider::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    {    AutoLock syncLock(this);
        *result = mEnabled;
    }
    return NOERROR;
}

ECode GpsLocationProvider::GetStatus(
    /* [in] */ IBundle* extras,
    /* [out] */ Int32* status)
{
    if (extras != NULL) {
        extras->PutInt32(String("satellites"), mSvCount);
    }
    *status = mStatus;
    return NOERROR;
}

void GpsLocationProvider::UpdateStatus(
    /* [in] */ Int32 status,
    /* [in] */ Int32 svCount)
{
    if (status != mStatus || svCount != mSvCount) {
        mStatus = status;
        mSvCount = svCount;
        mLocationExtras->PutInt32(String("satellites"), svCount);
        mStatusUpdateTime = SystemClock::GetElapsedRealtime();
    }
}

ECode GpsLocationProvider::GetStatusUpdateTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = mStatusUpdateTime;
    return NOERROR;
}

ECode GpsLocationProvider::SetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* source)
{
    SendMessage(SET_REQUEST, 0, (IObject*)(new GpsRequest(request, source)));
    return NOERROR;
}

void GpsLocationProvider::HandleSetRequest(
    /* [in] */ IProviderRequest* request,
    /* [in] */ IWorkSource* source)
{
    mProviderRequest = request;
    mWorkSource = source;
    UpdateRequirements();
}

void GpsLocationProvider::UpdateRequirements()
{
    if (mProviderRequest == NULL || mWorkSource == NULL) {
        return;
    }

    Boolean singleShot = FALSE;

    // see if the request is for a single update
    AutoPtr<IList> requests;
    mProviderRequest->GetLocationRequests((IList**)&requests);
    Int32 size;
    requests->GetSize(&size);
    if (requests != NULL && size > 0) {
        // if any request has zero or more than one updates
        // requested, then this is not single-shot mode
        singleShot = TRUE;

        AutoPtr<IIterator> iter;
        IIterable::Probe(requests)->GetIterator((IIterator**)&iter);
        Boolean hasNext;
        while(iter->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            iter->GetNext((IInterface**)&obj);
            AutoPtr<ILocationRequest> lr = ILocationRequest::Probe(obj);
            Int32 nu;
            lr->GetNumUpdates(&nu);
            if (nu != 1) {
                singleShot = FALSE;
            }
        }
    }
    String s;
    IObject::Probe(mProviderRequest)->ToString(&s);
    if (DEBUG) Logger::D(TAG, "setRequest %s", s.string());
    Boolean reportLocation;
    mProviderRequest->GetReportLocation(&reportLocation);
    if (reportLocation && !mDisableGps) {
        // update client uids
        UpdateClientUids(mWorkSource);
        Int64 interval;
        mProviderRequest->GetInterval(&interval);
        mFixInterval = (Int32)interval;

        // check for overflow
        if (mFixInterval != interval) {
            Logger::W(TAG, "interval overflow: %lld", interval);
            mFixInterval = Elastos::Core::Math::INT32_MAX_VALUE;
        }

        // apply request to GPS engine
        if (mStarted && HasCapability(GPS_CAPABILITY_SCHEDULING)) {
            // change period
            if (!Native_set_position_mode(mPositionMode, GPS_POSITION_RECURRENCE_PERIODIC,
                mFixInterval, 0, 0)) {
                Logger::E(TAG, "set_position_mode failed in setMinTime()");
            }
        }
        else if (!mStarted) {
            // start GPS
            StartNavigating(singleShot);
        }
    }
    else {
        AutoPtr<IWorkSource> ws;
        CWorkSource::New((IWorkSource**)&ws);
        UpdateClientUids(ws);

        StopNavigating();
        mAlarmManager->Cancel(mWakeupIntent);
        mAlarmManager->Cancel(mTimeoutIntent);
    }
}

void GpsLocationProvider::UpdateClientUids(
    /* [in] */ IWorkSource* source)
{
    // Update work source.
    AutoPtr<ArrayOf<IWorkSource*> > changes;
    mClientSource->SetReturningDiffs(source, (ArrayOf<IWorkSource*>**)&changes);
    if (changes == NULL) {
        return;
    }
    AutoPtr<IWorkSource> newWork = (*changes)[0];
    AutoPtr<IWorkSource> goneWork = (*changes)[1];

    AutoPtr<IAppOpsManagerHelper> appOpsManagerHelper;
    CAppOpsManagerHelper::AcquireSingleton((IAppOpsManagerHelper**)&appOpsManagerHelper);
    // Update sources that were not previously tracked.
    if (newWork != NULL) {
        Int32 lastuid = -1;
        Int32 val;
        newWork->GetSize(&val);
        for (Int32 i=0; i < val; i++) {
            Int32 uid;
            newWork->Get(i, &uid);
            AutoPtr<IBinder> binder;
            ECode ec = appOpsManagerHelper->GetToken(mAppOpsService, (IBinder**)&binder);
            if (FAILED(ec)) {
                Logger::W(TAG, "RemoteException%08x", ec);
                // return E_REMOTE_EXCEPTION;
            }
            String name;
            newWork->GetName(i, &name);
            Int32 ii;
            mAppOpsService->StartOperation(binder, IAppOpsManager::OP_GPS, uid, name, &ii);
            if (uid != lastuid) {
                lastuid = uid;
                mBatteryStats->NoteStartGps(uid);
            }
        }
    }

    // Update sources that are no longer tracked.
    if (goneWork != NULL) {
        Int32 lastuid = -1;
        Int32 v;
        goneWork->GetSize(&v);
        for (Int32 i=0; i< v; i++) {
            Int32 uid;
            goneWork->Get(i, &uid);
            AutoPtr<IBinder> binder;
            ECode ec = appOpsManagerHelper->GetToken(mAppOpsService, (IBinder**)&binder);
            if (FAILED(ec)) {
                Logger::W(TAG, "RemoteException%08x", ec);
                // return E_REMOTE_EXCEPTION;
            }
            String name;
            goneWork->GetName(i, &name);
            mAppOpsService->FinishOperation(binder, IAppOpsManager::OP_GPS, uid, name);
            if (uid != lastuid) {
                lastuid = uid;
                mBatteryStats->NoteStopGps(uid);
            }
        }
    }
}

ECode GpsLocationProvider::SendExtraCommand(
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 identity = Binder::ClearCallingIdentity();
    *result = FALSE;

    if (command.Equals("delete_aiding_data")) {
        *result = DeleteAidingData(extras);
    }
    else if (command.Equals("force_time_injection")) {
        SendMessage(INJECT_NTP_TIME, 0, NULL);
        *result = TRUE;
    }
    else if (command.Equals("force_xtra_injection")) {
        if (mSupportsXtra) {
            XtraDownloadRequest();
            *result = TRUE;
        }
    }
    else {
        Logger::W(TAG, "sendExtraCommand: unknown command %s", command.string());
    }

    Binder::RestoreCallingIdentity(identity);
    return NOERROR;
}

Boolean GpsLocationProvider::DeleteAidingData(
    /* [in] */ IBundle* extras)
{
    Int32 flags;

    if (extras == NULL) {
        flags = GPS_DELETE_ALL;
    }
    else {
        flags = 0;
        Boolean v;
        if (extras->GetBoolean(String("ephemeris"), &v),v) flags |= GPS_DELETE_EPHEMERIS;
        if (extras->GetBoolean(String("almanac"), &v),v) flags |= GPS_DELETE_ALMANAC;
        if (extras->GetBoolean(String("position"), &v),v) flags |= GPS_DELETE_POSITION;
        if (extras->GetBoolean(String("time"), &v),v) flags |= GPS_DELETE_TIME;
        if (extras->GetBoolean(String("iono"), &v),v) flags |= GPS_DELETE_IONO;
        if (extras->GetBoolean(String("utc"), &v),v) flags |= GPS_DELETE_UTC;
        if (extras->GetBoolean(String("health"), &v),v) flags |= GPS_DELETE_HEALTH;
        if (extras->GetBoolean(String("svdir"), &v),v) flags |= GPS_DELETE_SVDIR;
        if (extras->GetBoolean(String("svsteer"), &v),v) flags |= GPS_DELETE_SVSTEER;
        if (extras->GetBoolean(String("sadata"), &v),v) flags |= GPS_DELETE_SADATA;
        if (extras->GetBoolean(String("rti"), &v),v) flags |= GPS_DELETE_RTI;
        if (extras->GetBoolean(String("celldb-info"), &v),v) flags |= GPS_DELETE_CELLDB_INFO;
        if (extras->GetBoolean(String("all"), &v),v) flags |= GPS_DELETE_ALL;
        if (extras->GetBoolean(String("almanac-corr"), &v),v) flags |= GPS_DELETE_ALMANAC_CORR;
        if (extras->GetBoolean(String("freq-bias-est"), &v),v) flags |= GPS_DELETE_FREQ_BIAS_EST;
        if (extras->GetBoolean(String("ephemeris-GLO"), &v),v) flags |= GLO_DELETE_EPHEMERIS;
        if (extras->GetBoolean(String("almanac-GLO"), &v),v) flags |= GLO_DELETE_ALMANAC;
        if (extras->GetBoolean(String("svdir-GLO"), &v),v) flags |= GLO_DELETE_SVDIR;
        if (extras->GetBoolean(String("svsteer-GLO"), &v),v) flags |= GLO_DELETE_SVSTEER;
        if (extras->GetBoolean(String("almanac-corr-GLO"), &v),v) flags |= GLO_DELETE_ALMANAC_CORR;
        if (extras->GetBoolean(String("time-gps"), &v),v) flags |= GPS_DELETE_TIME_GPS;
        if (extras->GetBoolean(String("time-GLO"), &v),v) flags |= GLO_DELETE_TIME;
        if (extras->GetBoolean(String("ephemeris-BDS"), &v),v) flags |= BDS_DELETE_EPHEMERIS;
        if (extras->GetBoolean(String("almanac-BDS"), &v),v) flags |= BDS_DELETE_ALMANAC;
        if (extras->GetBoolean(String("svdir-BDS"), &v),v) flags |= BDS_DELETE_SVDIR;
        if (extras->GetBoolean(String("svsteer-BDS"), &v),v) flags |= BDS_DELETE_SVSTEER;
        if (extras->GetBoolean(String("almanac-corr-BDS"), &v),v) flags |= BDS_DELETE_ALMANAC_CORR;
        if (extras->GetBoolean(String("time-BDS"), &v),v) flags |= BDS_DELETE_TIME;
        if (extras->GetBoolean(String("all"), &v),v) flags |= GPS_DELETE_ALL;
    }

    if (flags != 0) {
        Native_delete_aiding_data(flags);
        return TRUE;
    }

    return FALSE;
}

void GpsLocationProvider::StartNavigating(
    /* [in] */ Boolean singleShot)
{
    if (!mStarted) {
        if (DEBUG) Logger::D(TAG, "startNavigating, singleShot is %s", singleShot ? "TRUE" : "FALSE");
        mTimeToFirstFix = 0;
        mLastFixTime = 0;
        mStarted = TRUE;
        mSingleShot = singleShot;
        mPositionMode = GPS_POSITION_MODE_STANDALONE;

        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);
        AutoPtr<ISettingsGlobal> sg;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
        Int32 i;
        sg->GetInt32(cr, ISettingsGlobal::ASSISTED_GPS_ENABLED, 1, &i);
        Boolean agpsEnabled = i != 0;
        mPositionMode = GetSuplMode(mProperties, agpsEnabled, singleShot);

        if (DEBUG) {
            String mode;

            switch(mPositionMode) {
                case GPS_POSITION_MODE_STANDALONE:
                    mode = "standalone";
                    break;
                case GPS_POSITION_MODE_MS_ASSISTED:
                    mode = "MS_ASSISTED";
                    break;
                case GPS_POSITION_MODE_MS_BASED:
                    mode = "MS_BASED";
                    break;
                default:
                    mode = "unknown";
                    break;
            }
            Logger::D(TAG, "setting position_mode to %s", mode.string());
        }

        Int32 interval = (HasCapability(GPS_CAPABILITY_SCHEDULING) ? mFixInterval : 1000);
        if (!Native_set_position_mode(mPositionMode, GPS_POSITION_RECURRENCE_PERIODIC,
            interval, 0, 0)) {
            mStarted = FALSE;
            Logger::E(TAG, "set_position_mode failed in startNavigating()");
            return;
        }
        if (!Native_start()) {
            mStarted = FALSE;
            Logger::E(TAG, "Native_start failed in startNavigating()");
            return;
        }

        // reset SV count to zero
        UpdateStatus(ILocationProvider::TEMPORARILY_UNAVAILABLE, 0);
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        sys->GetCurrentTimeMillis(&mFixRequestTime);
        if (!HasCapability(GPS_CAPABILITY_SCHEDULING)) {
            // set timer to give up if we do not receive a fix within NO_FIX_TIMEOUT
            // and our fix interval is not short
            if (mFixInterval >= NO_FIX_TIMEOUT) {
                mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP,
                        SystemClock::GetElapsedRealtime() + NO_FIX_TIMEOUT, mTimeoutIntent);
            }
        }
    }
}

void GpsLocationProvider::StopNavigating()
{
    if (DEBUG) Logger::D(TAG, "stopNavigating");
    if (mStarted) {
        mStarted = FALSE;
        mSingleShot = FALSE;
        Native_stop();
        mTimeToFirstFix = 0;
        mLastFixTime = 0;
        mLocationFlags = LOCATION_INVALID;

        // reset SV count to zero
        UpdateStatus(ILocationProvider::TEMPORARILY_UNAVAILABLE, 0);
    }
}

void GpsLocationProvider::Hibernate()
{
    // stop GPS until our next fix interval arrives
    StopNavigating();
    mAlarmManager->Cancel(mTimeoutIntent);
    mAlarmManager->Cancel(mWakeupIntent);
    Int64 now = SystemClock::GetElapsedRealtime();
    mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME_WAKEUP, now + mFixInterval, mWakeupIntent);
}

Boolean GpsLocationProvider::HasCapability(
    /* [in] */ Int32 capability)
{
    return ((mEngineCapabilities & capability) != 0);
}

void GpsLocationProvider::ReportLocation(
    /* [in] */ Int32 flags,
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Double altitude,
    /* [in] */ Float speed,
    /* [in] */ Float bearing,
    /* [in] */ Float accuracy,
    /* [in] */ Int64 timestamp)
{
    if (VERBOSE) Logger::V(TAG, "reportLocation lat: %lf long: %lf timestamp: %lld", latitude,longitude,timestamp);

    {    AutoLock syncLock(mLocation);
        mLocationFlags = flags;
        if ((flags & LOCATION_HAS_LAT_LONG) == LOCATION_HAS_LAT_LONG) {
            mLocation->SetLatitude(latitude);
            mLocation->SetLongitude(longitude);
            mLocation->SetTime(timestamp);
            // It would be nice to push the elapsed real-time timestamp
            // further down the stack, but this is still useful
            mLocation->SetElapsedRealtimeNanos(SystemClock::GetElapsedRealtimeNanos());
        }
        if ((flags & LOCATION_HAS_ALTITUDE) == LOCATION_HAS_ALTITUDE) {
            mLocation->SetAltitude(altitude);
        }
        else {
            mLocation->RemoveAltitude();
        }
        if ((flags & LOCATION_HAS_SPEED) == LOCATION_HAS_SPEED) {
            mLocation->SetSpeed(speed);
        }
        else {
            mLocation->RemoveSpeed();
        }
        if ((flags & LOCATION_HAS_BEARING) == LOCATION_HAS_BEARING) {
            mLocation->SetBearing(bearing);
        }
        else {
            mLocation->RemoveBearing();
        }
        if ((flags & LOCATION_HAS_ACCURACY) == LOCATION_HAS_ACCURACY) {
            mLocation->SetAccuracy(accuracy);
        }
        else {
            mLocation->RemoveAccuracy();
        }
        mLocation->SetExtras(mLocationExtras);

        ECode ec = mILocationManager->ReportLocation(mLocation, FALSE);
        if (FAILED(ec)) {
            Logger::E(TAG, "RemoteException calling reportLocation");
            // return E_REMOTE_EXCEPTION;
        }
    }
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetCurrentTimeMillis(&mLastFixTime);
    // report time to first fix
    if (mTimeToFirstFix == 0 && (flags & LOCATION_HAS_LAT_LONG) == LOCATION_HAS_LAT_LONG) {
        mTimeToFirstFix = (Int32)(mLastFixTime - mFixRequestTime);
        if (DEBUG) Logger::D(TAG, "TTFF: %d", mTimeToFirstFix);

        // notify status listeners
        mListenerHelper->OnFirstFix(mTimeToFirstFix);
    }

    if (mSingleShot) {
        StopNavigating();
    }

    if (mStarted && mStatus != ILocationProvider::AVAILABLE) {
        // we want to time out if we do not receive a fix
        // within the time out and we are requesting infrequent fixes
        if (!HasCapability(GPS_CAPABILITY_SCHEDULING) && mFixInterval < NO_FIX_TIMEOUT) {
            mAlarmManager->Cancel(mTimeoutIntent);
        }

        // send an intent to notify that the GPS is receiving fixes.
        AutoPtr<IIntent> intent;
        CIntent::New(ILocationManager::GPS_FIX_CHANGE_ACTION, (IIntent**)&intent);
        intent->PutBooleanExtra(ILocationManager::EXTRA_GPS_ENABLED, TRUE);
        mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
        UpdateStatus(ILocationProvider::AVAILABLE, mSvCount);
    }

   if (!HasCapability(GPS_CAPABILITY_SCHEDULING) && mStarted &&
           mFixInterval > GPS_POLLING_THRESHOLD_INTERVAL) {
        if (DEBUG) Logger::D(TAG, "got fix, hibernating");
        Hibernate();
    }
}

void GpsLocationProvider::ReportStatus(
    /* [in] */ Int32 status)
{
    if (DEBUG) Logger::V(TAG, "reportStatus status: %d", status);

    Boolean wasNavigating = mNavigating;
    switch (status) {
        case GPS_STATUS_SESSION_BEGIN:
            mNavigating = TRUE;
            mEngineOn = TRUE;
            break;
        case GPS_STATUS_SESSION_END:
            mNavigating = FALSE;
            break;
        case GPS_STATUS_ENGINE_ON:
            mEngineOn = TRUE;
            break;
        case GPS_STATUS_ENGINE_OFF:
            mEngineOn = FALSE;
            mNavigating = FALSE;
            break;
    }

    if (wasNavigating != mNavigating) {
        mListenerHelper->OnStatusChanged(mNavigating);

        // send an intent to notify that the GPS has been enabled or disabled
        AutoPtr<IIntent> intent;
        CIntent::New(ILocationManager::GPS_ENABLED_CHANGE_ACTION, (IIntent**)&intent);
        intent->PutExtra(ILocationManager::EXTRA_GPS_ENABLED, mNavigating);
        mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
    }
}

void GpsLocationProvider::ReportSvStatus()
{
    Int32 svCount = Native_read_sv_status(mSvs, mSnrs, mSvElevations, mSvAzimuths, mSvMasks);
    mListenerHelper->OnSvStatusChanged(
        svCount, mSvs, mSnrs, mSvElevations, mSvAzimuths, (*mSvMasks)[EPHEMERIS_MASK],
        (*mSvMasks)[ALMANAC_MASK], (*mSvMasks)[USED_FOR_FIX_MASK]);

    if (VERBOSE) {
        Logger::V(TAG, "SV count: %d ephemerisMask: %s almanacMask: ", svCount,
            StringUtils::ToHexString((*mSvMasks)[EPHEMERIS_MASK]).string(),
            StringUtils::ToHexString((*mSvMasks)[ALMANAC_MASK]).string());
        for (Int32 i = 0; i < svCount; i++) {
            Logger::V(TAG, "sv: %d snr: %f elev: %f azimuth: %f%s%s%s",
                (*mSvs)[i],(*mSnrs)[i]/10, (*mSvElevations)[i], (*mSvAzimuths)[i],
                (((*mSvMasks)[EPHEMERIS_MASK] & (1 << ((*mSvs)[i] - 1))) == 0 ? "  " : " E"),
                (((*mSvMasks)[ALMANAC_MASK] & (1 << ((*mSvs)[i] - 1))) == 0 ? "  " : " A"),
                (((*mSvMasks)[USED_FOR_FIX_MASK] & (1 << ((*mSvs)[i] - 1))) == 0 ? "" : "U"));
        }
    }

    // return number of sets used in fix instead of total
    //TODO Integer.bitCount
    // UpdateStatus(mStatus, Integer.bitCount(mSvMasks[USED_FOR_FIX_MASK]));

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 tm;
    sys->GetCurrentTimeMillis(&tm);
    if (mNavigating && mStatus == ILocationProvider::AVAILABLE && mLastFixTime > 0 &&
        tm - mLastFixTime > RECENT_FIX_TIMEOUT) {
        // send an intent to notify that the GPS is no longer receiving fixes.
        AutoPtr<IIntent> intent;
        CIntent::New(ILocationManager::GPS_FIX_CHANGE_ACTION, (IIntent**)&intent);
        intent->PutBooleanExtra(ILocationManager::EXTRA_GPS_ENABLED, FALSE);
        mContext->SendBroadcastAsUser(intent, UserHandle::ALL);
        UpdateStatus(ILocationProvider::TEMPORARILY_UNAVAILABLE, mSvCount);
    }
}

void GpsLocationProvider::ReportAGpsStatus(
    /* [in] */ Int32 type,
    /* [in] */ Int32 status,
    /* [in] */ ArrayOf<Byte>* ipaddr)
{
    switch (status) {
        case GPS_REQUEST_AGPS_DATA_CONN: {
            if (DEBUG) Logger::D(TAG, "GPS_REQUEST_AGPS_DATA_CONN");
            Logger::V(TAG, "Received SUPL IP addr[]: %g", ipaddr);
            // Set mAGpsDataConnectionState before calling startUsingNetworkFeature
            //  to avoid a race condition with handleUpdateNetworkState()
            mAGpsDataConnectionState = AGPS_DATA_CONNECTION_OPENING;
            Int32 result = 0;
            //TODO IPhone
            // mConnMgr->StartUsingNetworkFeature(
                // IConnectivityManager::TYPE_MOBILE, IPhone::FEATURE_ENABLE_SUPL, &result);
            if (ipaddr != NULL) {
                AutoPtr<IInetAddressHelper> iah;
                CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&iah);
                ECode ec = iah->GetByAddress(ipaddr, (IInetAddress**)&mAGpsDataConnectionIpAddr);
                if (FAILED(ec)) {
                    Logger::E(TAG, "Bad IP Address: %g%08x", ipaddr, ec);
                    mAGpsDataConnectionIpAddr = NULL;
                    // return E_UNKNOWN_HOST_EXCEPTION;
                }
                String str;
                IObject::Probe(mAGpsDataConnectionIpAddr)->ToString(&str);
                Logger::V(TAG, "IP address converted to: %s", str.string());
            }

            if (result == IPhoneConstants::APN_ALREADY_ACTIVE) {
                if (DEBUG) Logger::D(TAG, "PhoneConstants.APN_ALREADY_ACTIVE");
                if (!mAGpsApn.IsNull()) {
                    SetRouting();
                    Native_agps_data_conn_open(mAGpsApn, mApnIpType);
                    mAGpsDataConnectionState = AGPS_DATA_CONNECTION_OPEN;
                }
                else {
                    Logger::E(TAG, "mAGpsApn not set when receiving PhoneConstants.APN_ALREADY_ACTIVE");
                    mAGpsDataConnectionState = AGPS_DATA_CONNECTION_CLOSED;
                    Native_agps_data_conn_failed();
                }
            }
            else if (result == IPhoneConstants::APN_REQUEST_STARTED) {
                if (DEBUG) Logger::D(TAG, "PhoneConstants.APN_REQUEST_STARTED");
                // Nothing to do here
            }
            else {
                if (DEBUG) Logger::D(TAG, "startUsingNetworkFeature failed, value is %d", result);
                mAGpsDataConnectionState = AGPS_DATA_CONNECTION_CLOSED;
                Native_agps_data_conn_failed();
            }
            break;
        }
        case GPS_RELEASE_AGPS_DATA_CONN: {
            if (DEBUG) Logger::D(TAG, "GPS_RELEASE_AGPS_DATA_CONN");
            if (mAGpsDataConnectionState != AGPS_DATA_CONNECTION_CLOSED) {
                //TODO IPhone
                // Int32 i;
                // mConnMgr->StopUsingNetworkFeature(
                //         IConnectivityManager::TYPE_MOBILE, IPhone::FEATURE_ENABLE_SUPL, &i);
                Native_agps_data_conn_closed();
                mAGpsDataConnectionState = AGPS_DATA_CONNECTION_CLOSED;
                mAGpsDataConnectionIpAddr = NULL;
            }
            break;
        }
        case GPS_AGPS_DATA_CONNECTED:
            if (DEBUG) Logger::D(TAG, "GPS_AGPS_DATA_CONNECTED");
            break;
        case GPS_AGPS_DATA_CONN_DONE:
            if (DEBUG) Logger::D(TAG, "GPS_AGPS_DATA_CONN_DONE");
            break;
        case GPS_AGPS_DATA_CONN_FAILED:
            if (DEBUG) Logger::D(TAG, "GPS_AGPS_DATA_CONN_FAILED");
            break;
        default:
            Logger::D(TAG, "Received Unknown AGPS status: %d", status);
    }
}

void GpsLocationProvider::ReportNmea(
    /* [in] */ Int64 timestamp)
{
    Int32 length = Native_read_nmea(mNmeaBuffer, mNmeaBuffer->GetLength());
    String nmea((char const*)(mNmeaBuffer->GetPayload()), length);
    mListenerHelper->OnNmeaReceived(timestamp, nmea);
}

void GpsLocationProvider::ReportMeasurementData(
    /* [in] */ IGpsMeasurementsEvent* event)
{
    mGpsMeasurementsProvider->OnMeasurementsAvailable(event);
}

void GpsLocationProvider::ReportNavigationMessage(
    /* [in] */ IGpsNavigationMessageEvent* event)
{
    mGpsNavigationMessageProvider->OnNavigationMessageAvailable(event);
}

void GpsLocationProvider::SetEngineCapabilities(
    /* [in] */ Int32 capabilities)
{
    mEngineCapabilities = capabilities;

    if (!HasCapability(GPS_CAPABILITY_ON_DEMAND_TIME) && !mPeriodicTimeInjection) {
        mPeriodicTimeInjection = TRUE;
        RequestUtcTime();
    }
}

void GpsLocationProvider::XtraDownloadRequest()
{
    if (DEBUG) Logger::D(TAG, "xtraDownloadRequest");
    SendMessage(DOWNLOAD_XTRA_DATA, 0, NULL);
}

AutoPtr<ILocation> GpsLocationProvider::BuildLocation(
    /* [in] */ Int32 flags,
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Double altitude,
    /* [in] */ Float speed,
    /* [in] */ Float bearing,
    /* [in] */ Float accuracy,
    /* [in] */ Int64 timestamp)
{
    AutoPtr<ILocation> location;
    CLocation::New(ILocationManager::GPS_PROVIDER, (ILocation**)&location);
    if((flags & LOCATION_HAS_LAT_LONG) == LOCATION_HAS_LAT_LONG) {
        location->SetLatitude(latitude);
        location->SetLongitude(longitude);
        location->SetTime(timestamp);
        location->SetElapsedRealtimeNanos(SystemClock::GetElapsedRealtimeNanos());
    }
    if((flags & LOCATION_HAS_ALTITUDE) == LOCATION_HAS_ALTITUDE) {
        location->SetAltitude(altitude);
    }
    if((flags & LOCATION_HAS_SPEED) == LOCATION_HAS_SPEED) {
        location->SetSpeed(speed);
    }
    if((flags & LOCATION_HAS_BEARING) == LOCATION_HAS_BEARING) {
        location->SetBearing(bearing);
    }
    if((flags & LOCATION_HAS_ACCURACY) == LOCATION_HAS_ACCURACY) {
        location->SetAccuracy(accuracy);
    }
    return location;
}

Int64 GpsLocationProvider::GetGeofenceStatus(
    /* [in] */ Int32 status)
{
    switch(status) {
        case GPS_GEOFENCE_OPERATION_SUCCESS:
            return IGeofenceHardware::GEOFENCE_SUCCESS;
        case GPS_GEOFENCE_ERROR_GENERIC:
            return IGeofenceHardware::GEOFENCE_FAILURE;
        case GPS_GEOFENCE_ERROR_ID_EXISTS:
            return IGeofenceHardware::GEOFENCE_ERROR_ID_EXISTS;
        case GPS_GEOFENCE_ERROR_INVALID_TRANSITION:
            return IGeofenceHardware::GEOFENCE_ERROR_INVALID_TRANSITION;
        case GPS_GEOFENCE_ERROR_TOO_MANY_GEOFENCES:
            return IGeofenceHardware::GEOFENCE_ERROR_TOO_MANY_GEOFENCES;
        case GPS_GEOFENCE_ERROR_ID_UNKNOWN:
            return IGeofenceHardware::GEOFENCE_ERROR_ID_UNKNOWN;
        default:
            return -1;
    }
}

void GpsLocationProvider::ReportGeofenceTransition(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 flags,
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Double altitude,
    /* [in] */ Float speed,
    /* [in] */ Float bearing,
    /* [in] */ Float accuracy,
    /* [in] */ Int64 timestamp,
    /* [in] */ Int32 transition,
    /* [in] */ Int64 transitionTimestamp)
{
    if (mGeofenceHardwareImpl == NULL) {
        mGeofenceHardwareImpl = GeofenceHardwareImpl::GetInstance(mContext);
    }
    AutoPtr<ILocation> location = BuildLocation(
            flags,
            latitude,
            longitude,
            altitude,
            speed,
            bearing,
            accuracy,
            timestamp);
    Int32 GNSS = 1 << 0;
    mGeofenceHardwareImpl->ReportGeofenceTransition(
            geofenceId,
            location,
            transition,
            transitionTimestamp,
            IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE,
            /*FusedBatchOptions::SourceTechnologies::*/GNSS);
}

void GpsLocationProvider::ReportGeofenceStatus(
    /* [in] */ Int32 status,
    /* [in] */ Int32 flags,
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Double altitude,
    /* [in] */ Float speed,
    /* [in] */ Float bearing,
    /* [in] */ Float accuracy,
    /* [in] */ Int64 timestamp)
{
    if (mGeofenceHardwareImpl == NULL) {
        mGeofenceHardwareImpl = GeofenceHardwareImpl::GetInstance(mContext);
    }
    AutoPtr<ILocation> location = BuildLocation(
            flags,
            latitude,
            longitude,
            altitude,
            speed,
            bearing,
            accuracy,
            timestamp);
    Int32 monitorStatus = IGeofenceHardware::MONITOR_CURRENTLY_UNAVAILABLE;
    if(status == GPS_GEOFENCE_AVAILABLE) {
        monitorStatus = IGeofenceHardware::MONITOR_CURRENTLY_AVAILABLE;
    }

    Int32 GNSS = 1 << 0;
    mGeofenceHardwareImpl->ReportGeofenceMonitorStatus(
            IGeofenceHardware::MONITORING_TYPE_GPS_HARDWARE,
            monitorStatus,
            location,
            /*FusedBatchOptions::SourceTechnologies::*/GNSS);
}

void GpsLocationProvider::ReportGeofenceAddStatus(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    if (mGeofenceHardwareImpl == NULL) {
        mGeofenceHardwareImpl = GeofenceHardwareImpl::GetInstance(mContext);
    }
    mGeofenceHardwareImpl->ReportGeofenceAddStatus(geofenceId, GetGeofenceStatus(status));
}

void GpsLocationProvider::ReportGeofenceRemoveStatus(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    if (mGeofenceHardwareImpl == NULL) {
        mGeofenceHardwareImpl = GeofenceHardwareImpl::GetInstance(mContext);
    }
    mGeofenceHardwareImpl->ReportGeofenceRemoveStatus(geofenceId, GetGeofenceStatus(status));
}

void GpsLocationProvider::ReportGeofencePauseStatus(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    if (mGeofenceHardwareImpl == NULL) {
        mGeofenceHardwareImpl = GeofenceHardwareImpl::GetInstance(mContext);
    }
    mGeofenceHardwareImpl->ReportGeofencePauseStatus(geofenceId, GetGeofenceStatus(status));
}

void GpsLocationProvider::ReportGeofenceResumeStatus(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 status)
{
    if (mGeofenceHardwareImpl == NULL) {
        mGeofenceHardwareImpl = GeofenceHardwareImpl::GetInstance(mContext);
    }
    mGeofenceHardwareImpl->ReportGeofenceResumeStatus(geofenceId, GetGeofenceStatus(status));
}

ECode GpsLocationProvider::GetNetInitiatedListener(
    /* [out] */ IINetInitiatedListener** nil)
{
    VALIDATE_NOT_NULL(nil)
    *nil = mNetInitiatedListener;
    REFCOUNT_ADD(*nil)
    return NOERROR;
}

ECode GpsLocationProvider::ReportNiNotification(
    /* [in] */ Int32 notificationId,
    /* [in] */ Int32 niType,
    /* [in] */ Int32 notifyFlags,
    /* [in] */ Int32 timeout,
    /* [in] */ Int32 defaultResponse,
    /* [in] */ const String& requestorId,
    /* [in] */ const String& text,
    /* [in] */ Int32 requestorIdEncoding,
    /* [in] */ Int32 textEncoding,
    /* [in] */ const String& extras)
{
    Logger::I(TAG, "reportNiNotification: entered");
    Logger::I(TAG, "notificationId: %d, niType: %d, notifyFlags: %d, timeout: %d, defaultResponse: %d",
        notificationId, niType, notifyFlags, timeout, defaultResponse);

    Logger::I(TAG, "requestorId: %s, text: %s, requestorIdEncoding: %d, textEncoding: %d",
        requestorId.string(), text.string(), requestorIdEncoding, textEncoding);

    AutoPtr<IGpsNiNotification> notification;
    CGpsNiNotification::New((IGpsNiNotification**)&notification);
    notification->SetNotificationId(notificationId);
    notification->SetNiType(niType);
    notification->SetNeedNotify((notifyFlags & GPS_NI_NEED_NOTIFY) != 0);
    notification->SetNeedVerify((notifyFlags & GPS_NI_NEED_VERIFY) != 0);
    notification->SetPrivacyOverride((notifyFlags & GPS_NI_PRIVACY_OVERRIDE) != 0);
    notification->SetTimeOut(timeout);
    notification->SetDefaultResponse(defaultResponse);
    notification->SetRequestorId(requestorId);
    notification->SetText(text);
    notification->SetRequestorIdEncoding(requestorIdEncoding);
    notification->SetTextEncoding(textEncoding);

    // Process extras, assuming the format is
    // one of more lines of "key = value"
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);

    //TODO
    // if (extras.IsNull()) extras = "";
    AutoPtr<IProperties> extraProp;
    CProperties::New((IProperties**)&extraProp);

    AutoPtr<IReader> reader;
    CStringReader::New(extras, (IReader**)&reader);
    ECode ec = extraProp->Load(reader);
    if (FAILED(ec)) {
        Logger::E(TAG, "reportNiNotification cannot parse extras data: %s", extras.string());
        return E_IO_EXCEPTION;
    }

    AutoPtr<ISet> set;
    IHashTable::Probe(extraProp)->GetEntrySet((ISet**)&set);
    AutoPtr<ArrayOf<IInterface*> > array;
    set->ToArray((ArrayOf<IInterface*>**)&array);
    for (Int32 i = 0; i < array->GetLength(); i++) {
        AutoPtr<IMapEntry> ent = IMapEntry::Probe((*array)[i]);
        AutoPtr<IInterface> key, value;
        ent->GetKey((IInterface**)&key);
        ent->GetValue((IInterface**)&value);
        bundle->PutString(TO_STR(key), TO_STR(value));
    }

    notification->SetExtras(bundle);
    mNIHandler->HandleNiNotification(notification);

    return NOERROR;
}

void GpsLocationProvider::RequestSetID(
    /* [in] */ Int32 flags)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> phone = ITelephonyManager::Probe(obj);
    Int32 type = AGPS_SETID_TYPE_NONE;
    String data("");

    if ((flags & AGPS_RIL_REQUEST_SETID_IMSI) == AGPS_RIL_REQUEST_SETID_IMSI) {
        String data_temp;
        phone->GetSubscriberId(&data_temp);
        if (data_temp.IsNull()) {
            // This means the framework does not have the SIM card ready.
        }
        else {
            // This means the framework has the SIM card.
            data = data_temp;
            type = AGPS_SETID_TYPE_IMSI;
        }
    }
    else if ((flags & AGPS_RIL_REQUEST_SETID_MSISDN) == AGPS_RIL_REQUEST_SETID_MSISDN) {
        String data_temp;
        phone->GetLine1Number(&data_temp);
        if (data_temp.IsNull()) {
            // This means the framework does not have the SIM card ready.
        }
        else {
            // This means the framework has the SIM card.
            data = data_temp;
            type = AGPS_SETID_TYPE_MSISDN;
        }
    }
    Native_agps_set_id(type, data);
}

void GpsLocationProvider::RequestUtcTime()
{
    SendMessage(INJECT_NTP_TIME, 0, NULL);
}

void GpsLocationProvider::RequestRefLocation(
    /* [in] */ Int32 flags)
{
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    AutoPtr<ITelephonyManager> phone = ITelephonyManager::Probe(obj);
    Int32 t;
    phone->GetPhoneType(&t);
    const Int32 phoneType = t;
    if (phoneType == ITelephonyManager::PHONE_TYPE_GSM) {
        AutoPtr<ICellLocation> cell;
        phone->GetCellLocation((ICellLocation**)&cell);
        AutoPtr<IGsmCellLocation> gsm_cell = IGsmCellLocation::Probe(cell);
        String s;
        phone->GetNetworkOperator(&s);
        if ((gsm_cell != NULL) && (!s.IsNull()) && (s.GetLength() > 3)) {
            Int32 type;
            Int32 mcc = StringUtils::ParseInt32(s.Substring(0,3));
            Int32 mnc = StringUtils::ParseInt32(s.Substring(3));
            Int32 networkType;
            phone->GetNetworkType(&networkType);
            if (networkType == ITelephonyManager::NETWORK_TYPE_UMTS
                || networkType == ITelephonyManager::NETWORK_TYPE_HSDPA
                || networkType == ITelephonyManager::NETWORK_TYPE_HSUPA
                || networkType == ITelephonyManager::NETWORK_TYPE_HSPA
                || networkType == ITelephonyManager::NETWORK_TYPE_HSPAP) {
                type = AGPS_REF_LOCATION_TYPE_UMTS_CELLID;
            }
            else {
                type = AGPS_REF_LOCATION_TYPE_GSM_CELLID;
            }
            Int32 lac, cid;
            gsm_cell->GetLac(&lac);
            gsm_cell->GetCid(&cid);
            Native_agps_set_ref_location_cellid(type, mcc, mnc, lac, cid);
        }
        else {
            Logger::E(TAG,"Error getting cell location info.");
        }
    }
    else if (phoneType == ITelephonyManager::PHONE_TYPE_CDMA) {
        Logger::E(TAG, "CDMA not supported.");
    }
}

void GpsLocationProvider::SendMessage(
    /* [in] */ Int32 message,
    /* [in] */ Int32 arg,
    /* [in] */ IInterface* obj)
{
    // hold a wake lock until this message is delivered
    // note that this assumes the message will not be removed from the queue before
    // it is handled (otherwise the wake lock would be leaked).
    mWakeLock->AcquireLock();
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(message, arg, 1, obj, (IMessage**)&msg);
    msg->SendToTarget();
}

String GpsLocationProvider::GetDefaultApn()
{
    AutoPtr<IUriHelper> uh;
    CUriHelper::AcquireSingleton((IUriHelper**)&uh);
    AutoPtr<IUri> uri;
    uh->Parse(String("content://telephony/carriers/preferapn"), (IUri**)&uri);
    AutoPtr<ICursor> cursor;
    // try {
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ArrayOf<String> > proj = ArrayOf<String>::Alloc(1);
    (*proj)[0] = "apn";
    #if 0 //TODO Carriers.DEFAULT_SORT_ORDER
    cursor = mContext.getContentResolver().query(
            uri,
            new String[] { "apn" },
            null /* selection */,
            null /* selectionArgs */,
            Carriers.DEFAULT_SORT_ORDER);
    if (cursor != null && cursor.moveToFirst()) {
        return cursor.getString(0);
    }
    else {
        Log.e(TAG, "No APN found to select.");
    }
    // } catch (Exception e) {
        // Log.e(TAG, "Error encountered on selecting the APN.", e);
    // } finally {
        if (cursor != null) {
            cursor.close();
        }
    // }

    // return null;
    #endif
    return String("dummy-apn");
}

Int32 GpsLocationProvider::GetApnIpType(
    /* [in] */ const String& apn)
{
    if (apn.IsNull()) {
        return APN_INVALID;
    }

    // look for cached data to use
    if (apn.Equals(mAGpsApn) && mApnIpType != APN_INVALID) {
        return mApnIpType;
    }

    StringBuilder sb("current = 1 and apn = '");
    sb += apn;
    sb += "' and carrier_enabled = 1";
    String selection = sb.ToString();
    AutoPtr<ICursor> cursor;
    #if 0 //TODO Carriers.PROTOCOL Carriers.DEFAULT_SORT_ORDER
    // try {
    cursor = mContext.getContentResolver().query(
            Carriers.CONTENT_URI,
            new String[] { Carriers.PROTOCOL },
            selection,
            null,
            Carriers.DEFAULT_SORT_ORDER);

    if (null != cursor && cursor.moveToFirst()) {
        return translateToApnIpType(cursor.getString(0), apn);
    }
    else {
        Log.e(TAG, "No entry found in query for APN: " + apn);
    }
    // } catch (Exception e) {
    //     Log.e(TAG, "Error encountered on APN query for: " + apn, e);
    // } finally {
        if (cursor != null) {
            cursor.close();
        }
    // }
    #endif
    return APN_INVALID;
}

Int32 GpsLocationProvider::TranslateToApnIpType(
    /* [in] */ const String& ipProtocol,
    /* [in] */ const String& apn)
{
    if (ipProtocol.Equals("IP")) {
        return APN_IPV4;
    }
    if (ipProtocol.Equals("IPV6")) {
        return APN_IPV6;
    }
    if (ipProtocol.Equals("IPV4V6")) {
        return APN_IPV4V6;
    }

    // we hit the default case so the ipProtocol is not recognized
    StringBuilder sb("Unknown IP Protocol: ");
    sb += ipProtocol;
    sb += ", for APN: ";
    sb += apn;
    String message = sb.ToString();
    Logger::E(TAG, message.string());
    return APN_INVALID;
}

void GpsLocationProvider::SetRouting()
{
    if (mAGpsDataConnectionIpAddr == NULL) {
        return;
    }

    Boolean result;
    mConnMgr->RequestRouteToHostAddress(
        IConnectivityManager::TYPE_MOBILE_SUPL, mAGpsDataConnectionIpAddr, &result);
    String s;
    IObject::Probe(mAGpsDataConnectionIpAddr)->ToString(&s);
    if (!result) {
        Logger::E(TAG, "Error requesting route to host: %s", s.string());
    }
    else if (DEBUG) {
        Logger::D(TAG, "Successfully requested route to host: %s", s.string());
    }
}

ECode GpsLocationProvider::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<String>* args)
{
    StringBuilder s;
    s += "  mFixInterval=";
    s += mFixInterval;
    s += "\n";
    s += "  mDisableGps (battery saver mode)=";
    s += mDisableGps;
    s += "\n";
    s += "  mEngineCapabilities=0x";
    s += StringUtils::ToHexString(mEngineCapabilities);
    s += " (";
    if (HasCapability(GPS_CAPABILITY_SCHEDULING)) s += "SCHED ";
    if (HasCapability(GPS_CAPABILITY_MSB)) s += "MSB ";
    if (HasCapability(GPS_CAPABILITY_MSA)) s += "MSA ";
    if (HasCapability(GPS_CAPABILITY_SINGLE_SHOT)) s += "SINGLE_SHOT ";
    if (HasCapability(GPS_CAPABILITY_ON_DEMAND_TIME)) s += "ON_DEMAND_TIME ";
    s += ")\n";

    s += Native_get_internal_state();
    AutoPtr<ICharSequence> cs;
    CString::New(s.ToString(), (ICharSequence**)&cs);
    IAppendable::Probe(pw)->Append(cs);
    return NOERROR;
}

static AutoPtr<GpsLocationProvider> mCallbacksObj;
static const GpsInterface* sGpsInterface = NULL;
static const GpsXtraInterface* sGpsXtraInterface = NULL;
static const AGpsInterface* sAGpsInterface = NULL;
static const GpsNiInterface* sGpsNiInterface = NULL;
static const GpsDebugInterface* sGpsDebugInterface = NULL;
static const AGpsRilInterface* sAGpsRilInterface = NULL;
static const GpsGeofencingInterface* sGpsGeofencingInterface = NULL;
static const GpsMeasurementInterface* sGpsMeasurementInterface = NULL;
static const GpsNavigationMessageInterface* sGpsNavigationMessageInterface = NULL;
static const GnssConfigurationInterface* sGnssConfigurationInterface = NULL;

// temporary storage for GPS callbacks
static GpsSvStatus  sGpsSvStatus;
static const char* sNmeaString;
static int sNmeaStringLength;

#define WAKE_LOCK_NAME  "GPS"

static void location_callback(GpsLocation* location)
{
    mCallbacksObj->ReportLocation(location->flags,
            (Double)location->latitude, (Double)location->longitude,
            (Double)location->altitude,
            (Float)location->speed, (Float)location->bearing,
            (Float)location->accuracy, (Int64)location->timestamp);
}

static void status_callback(GpsStatus* status)
{
    mCallbacksObj->ReportStatus(status->status);
}

static void sv_status_callback(GpsSvStatus* sv_status)
{
    memcpy(&sGpsSvStatus, sv_status, sizeof(sGpsSvStatus));
    mCallbacksObj->ReportSvStatus();
}

static void nmea_callback(GpsUtcTime timestamp, const char* nmea, int length)
{
    // The Java code will call back to read these values
    // We do this to avoid creating unnecessary String objects
    sNmeaString = nmea;
    sNmeaStringLength = length;
    mCallbacksObj->ReportNmea(timestamp);
}

static void set_capabilities_callback(uint32_t capabilities)
{
    ALOGD("set_capabilities_callback: %du\n", capabilities);
    mCallbacksObj->SetEngineCapabilities(capabilities);
}

static void acquire_wakelock_callback()
{
    acquire_wake_lock(PARTIAL_WAKE_LOCK, WAKE_LOCK_NAME);
}

static void release_wakelock_callback()
{
    release_wake_lock(WAKE_LOCK_NAME);
}

static void request_utc_time_callback()
{
    mCallbacksObj->RequestUtcTime();
}

static pthread_t create_thread_callback(const char* name, void (*start)(void *), void* arg)
{
    // return (pthread_t)AndroidRuntime::createJavaThread(name, start, arg);
    android_thread_id_t threadId = 0;
    androidCreateRawThreadEtc((android_thread_func_t)start, arg, name,
        ANDROID_PRIORITY_DEFAULT, 0, &threadId);
    return (pthread_t)threadId;
}

GpsCallbacks sGpsCallbacks = {
    sizeof(GpsCallbacks),
    location_callback,
    status_callback,
    sv_status_callback,
    nmea_callback,
    set_capabilities_callback,
    acquire_wakelock_callback,
    release_wakelock_callback,
    create_thread_callback,
    request_utc_time_callback,
};

static void xtra_download_request_callback()
{
    mCallbacksObj->XtraDownloadRequest();
}

GpsXtraCallbacks sGpsXtraCallbacks = {
    xtra_download_request_callback,
    create_thread_callback,
};

static AutoPtr<ArrayOf<Byte> > convert_to_ipv4(uint32_t ip, bool net_order)
{
    if (INADDR_NONE == ip) {
        return NULL;
    }

    AutoPtr<ArrayOf<Byte> > byteArray = ArrayOf<Byte>::Alloc(4);
    if (byteArray == NULL) {
        ALOGE("Unable to allocate byte array for IPv4 address");
        return NULL;
    }

    Byte ipv4[4];
    if (net_order) {
        ALOGV("Converting IPv4 address(net_order) %x", ip);
        memcpy(ipv4, &ip, sizeof(ipv4));
    }
    else {
        ALOGV("Converting IPv4 address(host_order) %x", ip);
        //endianess transparent conversion from int to char[]
        ipv4[0] = (Byte) (ip & 0xFF);
        ipv4[1] = (Byte)((ip>>8) & 0xFF);
        ipv4[2] = (Byte)((ip>>16) & 0xFF);
        ipv4[3] = (Byte) (ip>>24);
    }

    byteArray->Copy(ipv4, 4);
    return byteArray;
}

static void agps_status_callback(AGpsStatus* agps_status)
{
    AutoPtr<ArrayOf<Byte> > byteArray;
    bool isSupported = false;

    size_t status_size = agps_status->size;
    if (status_size == sizeof(AGpsStatus_v3)) {
      ALOGV("AGpsStatus is V3: %d", status_size);
      switch (agps_status->addr.ss_family)
      {
      case AF_INET:
          {
            struct sockaddr_in *in = (struct sockaddr_in*)&(agps_status->addr);
            uint32_t *pAddr = (uint32_t*)&(in->sin_addr);
            byteArray = convert_to_ipv4(*pAddr, true /* net_order */);
            if (byteArray != NULL) {
                isSupported = true;
            }
            IF_ALOGD() {
                // log the IP for reference in case there is a bogus value pushed by HAL
                char str[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(in->sin_addr), str, INET_ADDRSTRLEN);
                ALOGD("AGPS IP is v4: %s", str);
            }
          }
          break;
      case AF_INET6:
          {
            struct sockaddr_in6 *in6 = (struct sockaddr_in6*)&(agps_status->addr);
            byteArray = ArrayOf<Byte>::Alloc(16);
            if (byteArray != NULL) {
                byteArray->Copy((const Byte*)&(in6->sin6_addr), 16);
                isSupported = true;
            }
            else {
                ALOGE("Unable to allocate byte array for IPv6 address.");
            }
            IF_ALOGD() {
                // log the IP for reference in case there is a bogus value pushed by HAL
                char str[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &(in6->sin6_addr), str, INET6_ADDRSTRLEN);
                ALOGD("AGPS IP is v6: %s", str);
            }
          }
          break;
      default:
          ALOGE("Invalid ss_family found: %d", agps_status->addr.ss_family);
          break;
      }
    }
    else if (status_size >= sizeof(AGpsStatus_v2)) {
      ALOGV("AGpsStatus is V2+: %d", status_size);
      // for back-compatibility reasons we check in v2 that the data structure size is greater or
      // equal to the declared size in gps.h
      uint32_t ipaddr = agps_status->ipaddr;
      ALOGV("AGPS IP is v4: %x", ipaddr);
      byteArray = convert_to_ipv4(ipaddr, false /* net_order */);
      if (ipaddr == INADDR_NONE || byteArray != NULL) {
          isSupported = true;
      }
    }
    else if (status_size >= sizeof(AGpsStatus_v1)) {
        ALOGV("AGpsStatus is V1+: %d", status_size);
        // because we have to check for >= with regards to v2, we also need to relax the check here
        // and only make sure that the size is at least what we expect
        isSupported = true;
    }
    else {
        ALOGE("Invalid size of AGpsStatus found: %d.", status_size);
    }

    if (isSupported) {
        Int32 byteArrayLength = byteArray != NULL ? byteArray->GetLength() : 0;
        ALOGV("Passing AGPS IP addr: size %d", byteArrayLength);
        mCallbacksObj->ReportAGpsStatus(agps_status->type,
                            agps_status->status, byteArray);

    }
    else {
        ALOGD("Skipping calling method_reportAGpsStatus.");
    }
}

AGpsCallbacks sAGpsCallbacks = {
    agps_status_callback,
    create_thread_callback,
};

static void gps_ni_notify_callback(GpsNiNotification *notification)
{
    ALOGD("gps_ni_notify_callback\n");
    String requestor_id(notification->requestor_id);
    String text(notification->text);
    String extras(notification->extras);

    if (requestor_id && text && extras) {
        mCallbacksObj->ReportNiNotification(
            notification->notification_id, notification->ni_type,
            notification->notify_flags, notification->timeout,
            notification->default_response, requestor_id, text,
            notification->requestor_id_encoding,
            notification->text_encoding, extras);
    }
    else {
        ALOGE("out of memory in gps_ni_notify_callback\n");
    }
}

GpsNiCallbacks sGpsNiCallbacks = {
    gps_ni_notify_callback,
    create_thread_callback,
};

static void agps_request_set_id(uint32_t flags)
{
    mCallbacksObj->RequestSetID(flags);
}

static void agps_request_ref_location(uint32_t flags)
{
    mCallbacksObj->RequestRefLocation(flags);
}

AGpsRilCallbacks sAGpsRilCallbacks = {
    agps_request_set_id,
    agps_request_ref_location,
    create_thread_callback,
};

static void gps_geofence_transition_callback(int32_t geofence_id,  GpsLocation* location,
        int32_t transition, GpsUtcTime timestamp)
{

    mCallbacksObj->ReportGeofenceTransition(geofence_id,
            location->flags, (Double)location->latitude, (Double)location->longitude,
            (Double)location->altitude,
            (Float)location->speed, (Float)location->bearing,
            (Float)location->accuracy, (Int64)location->timestamp,
            transition, timestamp);
};

static void gps_geofence_status_callback(int32_t status, GpsLocation* location)
{
    Int32 flags = 0;
    Double latitude = 0;
    Double longitude = 0;
    Double altitude = 0;
    Float speed = 0;
    Float bearing = 0;
    Float accuracy = 0;
    Int64 timestamp = 0;
    if (location != NULL) {
        flags = location->flags;
        latitude = location->latitude;
        longitude = location->longitude;
        altitude = location->altitude;
        speed = location->speed;
        bearing = location->bearing;
        accuracy = location->accuracy;
        timestamp = location->timestamp;
    }

    mCallbacksObj->ReportGeofenceStatus(status,
            flags, latitude, longitude, altitude, speed, bearing, accuracy, timestamp);
};

static void gps_geofence_add_callback(int32_t geofence_id, int32_t status)
{
    if (status != GPS_GEOFENCE_OPERATION_SUCCESS) {
        ALOGE("Error in geofence_add_callback: %d\n", status);
    }
    mCallbacksObj->ReportGeofenceAddStatus(geofence_id, status);
};

static void gps_geofence_remove_callback(int32_t geofence_id, int32_t status)
{
    if (status != GPS_GEOFENCE_OPERATION_SUCCESS) {
        ALOGE("Error in geofence_remove_callback: %d\n", status);
    }
    mCallbacksObj->ReportGeofenceRemoveStatus(geofence_id, status);
};

static void gps_geofence_resume_callback(int32_t geofence_id, int32_t status)
{
    if (status != GPS_GEOFENCE_OPERATION_SUCCESS) {
        ALOGE("Error in geofence_resume_callback: %d\n", status);
    }
    mCallbacksObj->ReportGeofenceResumeStatus(geofence_id, status);
};

static void gps_geofence_pause_callback(int32_t geofence_id, int32_t status)
{
    if (status != GPS_GEOFENCE_OPERATION_SUCCESS) {
        ALOGE("Error in geofence_pause_callback: %d\n", status);
    }
    mCallbacksObj->ReportGeofencePauseStatus(geofence_id, status);
};

GpsGeofenceCallbacks sGpsGeofenceCallbacks = {
    gps_geofence_transition_callback,
    gps_geofence_status_callback,
    gps_geofence_add_callback,
    gps_geofence_remove_callback,
    gps_geofence_pause_callback,
    gps_geofence_resume_callback,
    create_thread_callback,
};

Boolean GpsLocationProvider::Native_Class_init()
{
    int err;
    hw_module_t* module;

    err = hw_get_module(GPS_HARDWARE_MODULE_ID, (hw_module_t const**)&module);
    if (err == 0) {
        hw_device_t* device;
        err = module->methods->open(module, GPS_HARDWARE_MODULE_ID, &device);
        if (err == 0) {
            gps_device_t* gps_device = (gps_device_t *)device;
            sGpsInterface = gps_device->get_gps_interface(gps_device);
        }
    }
    if (sGpsInterface) {
        sGpsXtraInterface =
            (const GpsXtraInterface*)sGpsInterface->get_extension(GPS_XTRA_INTERFACE);
        sAGpsInterface =
            (const AGpsInterface*)sGpsInterface->get_extension(AGPS_INTERFACE);
        sGpsNiInterface =
            (const GpsNiInterface*)sGpsInterface->get_extension(GPS_NI_INTERFACE);
        sGpsDebugInterface =
            (const GpsDebugInterface*)sGpsInterface->get_extension(GPS_DEBUG_INTERFACE);
        sAGpsRilInterface =
            (const AGpsRilInterface*)sGpsInterface->get_extension(AGPS_RIL_INTERFACE);
        sGpsGeofencingInterface =
            (const GpsGeofencingInterface*)sGpsInterface->get_extension(GPS_GEOFENCING_INTERFACE);
        sGpsMeasurementInterface =
            (const GpsMeasurementInterface*)sGpsInterface->get_extension(GPS_MEASUREMENT_INTERFACE);
        sGpsNavigationMessageInterface =
            (const GpsNavigationMessageInterface*)sGpsInterface->get_extension(
                    GPS_NAVIGATION_MESSAGE_INTERFACE);
        sGnssConfigurationInterface =
            (const GnssConfigurationInterface*)sGpsInterface->get_extension(
                    GNSS_CONFIGURATION_INTERFACE);
    }
    return TRUE;
}

Boolean GpsLocationProvider::Native_is_supported()
{
    if (sGpsInterface != NULL) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

Boolean GpsLocationProvider::Native_init()
{
    // this must be set before calling into the HAL library
    if (!mCallbacksObj) {
        mCallbacksObj = this;
    }

    // fail if the main interface fails to initialize
    if (!sGpsInterface || sGpsInterface->init(&sGpsCallbacks) != 0)
        return FALSE;

    // if XTRA initialization fails we will disable it by sGpsXtraInterface to NULL,
    // but continue to allow the rest of the GPS interface to work.
    if (sGpsXtraInterface && sGpsXtraInterface->init(&sGpsXtraCallbacks) != 0)
        sGpsXtraInterface = NULL;
    if (sAGpsInterface)
        sAGpsInterface->init(&sAGpsCallbacks);
    if (sGpsNiInterface)
        sGpsNiInterface->init(&sGpsNiCallbacks);
    if (sAGpsRilInterface)
        sAGpsRilInterface->init(&sAGpsRilCallbacks);
    if (sGpsGeofencingInterface)
        sGpsGeofencingInterface->init(&sGpsGeofenceCallbacks);

    return TRUE;
}

void GpsLocationProvider::Native_cleanup()
{
    if (sGpsInterface)
        sGpsInterface->cleanup();
}

Boolean GpsLocationProvider::Native_set_position_mode(
    /* [in] */ Int32 mode,
    /* [in] */ Int32 recurrence,
    /* [in] */ Int32 min_interval,
    /* [in] */ Int32 preferred_accuracy,
    /* [in] */ Int32 preferred_time)
{
    if (sGpsInterface) {
        if (sGpsInterface->set_position_mode(mode, recurrence, min_interval, preferred_accuracy,
                preferred_time) == 0) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    else
        return FALSE;
}

Boolean GpsLocationProvider::Native_start()
{
    if (sGpsInterface) {
        if (sGpsInterface->start() == 0) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    else
        return FALSE;
}

Boolean GpsLocationProvider::Native_stop()
{
    if (sGpsInterface) {
        if (sGpsInterface->stop() == 0) {
            return TRUE;
        }
        else {
            return FALSE;
        }
    }
    else
        return FALSE;
}

void GpsLocationProvider::Native_delete_aiding_data(
    /* [in] */ Int32 flags)
{
    if (sGpsInterface)
        sGpsInterface->delete_aiding_data(flags);
}

Int32 GpsLocationProvider::Native_read_sv_status(
    /* [in] */ ArrayOf<Int32>* prns,
    /* [in] */ ArrayOf<Float>* snrs,
    /* [in] */ ArrayOf<Float>* elev,
    /* [in] */ ArrayOf<Float>* azim,
    /* [in] */ ArrayOf<Int32>* mask)
{
    // this should only be called from within a call to reportSvStatus

    int num_svs = sGpsSvStatus.num_svs;
    for (int i = 0; i < num_svs; i++) {
        (*prns)[i] = sGpsSvStatus.sv_list[i].prn;
        (*snrs)[i] = sGpsSvStatus.sv_list[i].snr;
        (*elev)[i] = sGpsSvStatus.sv_list[i].elevation;
        (*azim)[i] = sGpsSvStatus.sv_list[i].azimuth;
    }
    (*mask)[0] = sGpsSvStatus.ephemeris_mask;
    (*mask)[1] = sGpsSvStatus.almanac_mask;
    (*mask)[2] = sGpsSvStatus.used_in_fix_mask;

    return num_svs;
}

void GpsLocationProvider::Native_agps_set_ref_location_cellid(
    /* [in] */ Int32 type,
    /* [in] */ Int32 mcc,
    /* [in] */ Int32 mnc,
    /* [in] */ Int32 lac,
    /* [in] */ Int32 cid)
{
    AGpsRefLocation location;

    if (!sAGpsRilInterface) {
        ALOGE("no AGPS RIL interface in agps_set_reference_location_cellid");
        return;
    }

    switch(type) {
        case AGPS_REF_LOCATION_TYPE_GSM_CELLID:
        case AGPS_REF_LOCATION_TYPE_UMTS_CELLID:
            location.type = type;
            location.u.cellID.mcc = mcc;
            location.u.cellID.mnc = mnc;
            location.u.cellID.lac = lac;
            location.u.cellID.cid = cid;
            break;
        default:
            ALOGE("Neither a GSM nor a UMTS cellid (%s:%d).",__FUNCTION__,__LINE__);
            return;
            break;
    }
    sAGpsRilInterface->set_ref_location(&location, sizeof(location));
}

void GpsLocationProvider::Native_agps_ni_message(
    /* [in] */ ArrayOf<Byte>* msg,
    /* [in] */ Int32 size)
{
    size_t sz;

    if (!sAGpsRilInterface) {
        ALOGE("no AGPS RIL interface in send_ni_message");
        return;
    }
    if (size < 0)
        return;
    sz = (size_t)size;
    sAGpsRilInterface->ni_message((uint8_t *)msg->GetPayload(), sz);
}

void GpsLocationProvider::Native_agps_set_id(
    /* [in] */ Int32 type,
    /* [in] */ const String& setid)
{
    if (!sAGpsRilInterface) {
        ALOGE("no AGPS RIL interface in agps_set_id");
        return;
    }

    sAGpsRilInterface->set_set_id(type, setid.string());
}

Int32 GpsLocationProvider::Native_read_nmea(
    /* [in] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 bufferSize)
{
    // this should only be called from within a call to reportNmea
    int length = sNmeaStringLength;
    if (length > bufferSize)
        length = bufferSize;
    memcpy(buffer->GetPayload(), sNmeaString, length);
    return length;
}

void GpsLocationProvider::Native_inject_time(
    /* [in] */ Int64 time,
    /* [in] */ Int64 timeReference,
    /* [in] */ Int32 uncertainty)
{
    if (sGpsInterface)
        sGpsInterface->inject_time(time, timeReference, uncertainty);
}

void GpsLocationProvider::Native_inject_location(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Float accuracy)
{
    if (sGpsInterface)
        sGpsInterface->inject_location(latitude, longitude, accuracy);
}

Boolean GpsLocationProvider::Native_supports_xtra()
{
    if (sGpsXtraInterface != NULL) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

void GpsLocationProvider::Native_inject_xtra_data(
    /* [in] */ ArrayOf<Byte>* data,
    /* [in] */ Int32 length)
{
    if (!sGpsXtraInterface) {
        ALOGE("no XTRA interface in inject_xtra_data");
        return;
    }

    sGpsXtraInterface->inject_xtra_data((char *)data->GetPayload(), length);
}

// AGPS Support
void GpsLocationProvider::Native_agps_data_conn_open(
    /* [in] */ const String& apn,
    /* [in] */ Int32 apnIpType)
{
    if (!sAGpsInterface) {
        ALOGE("no AGPS interface in agps_data_conn_open");
        return;
    }
    if (apn == NULL) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", NULL);
        ALOGE("Native_agps_data_conn_open apn is NULL!");
        return;
    }

    const char *apnStr = apn.string();

    size_t interface_size = sAGpsInterface->size;
    if (interface_size == sizeof(AGpsInterface_v2)) {
        sAGpsInterface->data_conn_open_with_apn_ip_type(apnStr, apnIpType);
    }
    else if (interface_size == sizeof(AGpsInterface_v1)) {
        sAGpsInterface->data_conn_open(apnStr);
    }
    else {
        ALOGE("Invalid size of AGpsInterface found: %d.", interface_size);
    }
}

void GpsLocationProvider::Native_agps_data_conn_closed()
{
    if (!sAGpsInterface) {
        ALOGE("no AGPS interface in agps_data_conn_closed");
        return;
    }
    sAGpsInterface->data_conn_closed();
}

void GpsLocationProvider::Native_agps_data_conn_failed()
{
    if (!sAGpsInterface) {
        ALOGE("no AGPS interface in agps_data_conn_failed");
        return;
    }
    sAGpsInterface->data_conn_failed();
}

void GpsLocationProvider::Native_set_agps_server(
    /* [in] */ Int32 type,
    /* [in] */ const String& hostname,
    /* [in] */ Int32 port)
{
    if (!sAGpsInterface) {
        ALOGE("no AGPS interface in set_agps_server");
        return;
    }
    const char *c_hostname = hostname.string();
    sAGpsInterface->set_server(type, c_hostname, port);
}

void GpsLocationProvider::Native_send_ni_response(
    /* [in] */ Int32 notificationId,
    /* [in] */ Int32 userResponse)
{
    if (!sGpsNiInterface) {
        ALOGE("no NI interface in send_ni_response");
        return;
    }

    sGpsNiInterface->respond(notificationId, userResponse);
}

String GpsLocationProvider::Native_get_internal_state()
{
    String result ;
    if (sGpsDebugInterface) {
        const size_t maxLength = 2047;
        char buffer[maxLength+1];
        size_t length = sGpsDebugInterface->get_internal_state(buffer, maxLength);
        if (length > maxLength) length = maxLength;
        buffer[length] = 0;
        result = String(buffer);
    }
    return result;
}

void GpsLocationProvider::Native_update_network_state(
    /* [in] */ Boolean connected,
    /* [in] */ Int32 type,
    /* [in] */ Boolean roaming,
    /* [in] */ Boolean available,
    /* [in] */ const String& extraInfo,
    /* [in] */ const String& apn)
{
    if (sAGpsRilInterface && sAGpsRilInterface->update_network_state) {
        if (extraInfo) {
            const char *extraInfoStr =extraInfo.string();
            sAGpsRilInterface->update_network_state(connected, type, roaming, extraInfoStr);
        }
        else {
            sAGpsRilInterface->update_network_state(connected, type, roaming, NULL);
        }

        // update_network_availability callback was not included in original AGpsRilInterface
        if (sAGpsRilInterface->size >= sizeof(AGpsRilInterface)
                && sAGpsRilInterface->update_network_availability) {
            const char *c_apn = apn.string();
            sAGpsRilInterface->update_network_availability(available, c_apn);
        }
    }
}

Boolean GpsLocationProvider::Native_is_geofence_supported()
{
    if (sGpsGeofencingInterface != NULL) {
        return TRUE;
    }
    return FALSE;
}

Boolean GpsLocationProvider::Native_add_geofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Double radius,
    /* [in] */ Int32 lastTransition,
    /* [in] */ Int32 monitorTransitions,
    /* [in] */ Int32 notificationResponsivenes,
    /* [in] */ Int32 unknownTimer)
{
    if (sGpsGeofencingInterface != NULL) {
        sGpsGeofencingInterface->add_geofence_area(geofenceId, latitude, longitude,
                radius, lastTransition, monitorTransitions, notificationResponsivenes,
                unknownTimer);
        return TRUE;
    }
    else {
        ALOGE("Geofence interface not available");
    }
    return FALSE;
}

Boolean GpsLocationProvider::Native_remove_geofence(
    /* [in] */ Int32 geofenceId)
{
    if (sGpsGeofencingInterface != NULL) {
        sGpsGeofencingInterface->remove_geofence_area(geofenceId);
        return TRUE;
    }
    else {
        ALOGE("Geofence interface not available");
    }
    return FALSE;
}

Boolean GpsLocationProvider::Native_resume_geofence(
    /* [in] */ Int32 geofenceId,
    /* [in] */ Int32 transitions)
{
    if (sGpsGeofencingInterface != NULL) {
        sGpsGeofencingInterface->resume_geofence(geofenceId, transitions);
        return TRUE;
    }
    else {
        ALOGE("Geofence interface not available");
    }
    return FALSE;
}

Boolean GpsLocationProvider::Native_pause_geofence(
    /* [in] */ Int32 geofenceId)
{
    if (sGpsGeofencingInterface != NULL) {
        sGpsGeofencingInterface->pause_geofence(geofenceId);
        return TRUE;
    }
    else {
        ALOGE("Geofence interface not available");
    }
    return FALSE;
}

static AutoPtr<IGpsClock> translate_gps_clock(GpsClock* clock)
{
    AutoPtr<IGpsClock> gpsClockObject;
    CGpsClock::New((IGpsClock**)&gpsClockObject);
    GpsClockFlags flags = clock->flags;

    if (flags & GPS_CLOCK_HAS_LEAP_SECOND) {
        gpsClockObject->SetLeapSecond(clock->leap_second);
    }

    gpsClockObject->SetType(clock->type);
    gpsClockObject->SetTimeInNs(clock->time_ns);

    if (flags & GPS_CLOCK_HAS_TIME_UNCERTAINTY) {
        gpsClockObject->SetTimeUncertaintyInNs(clock->time_uncertainty_ns);
    }

    if (flags & GPS_CLOCK_HAS_FULL_BIAS) {
        gpsClockObject->SetFullBiasInNs(clock->full_bias_ns);
    }

    if (flags & GPS_CLOCK_HAS_BIAS) {
        gpsClockObject->SetBiasInNs(clock->bias_ns);
    }

    if (flags & GPS_CLOCK_HAS_BIAS_UNCERTAINTY) {
        gpsClockObject->SetBiasUncertaintyInNs(clock->bias_uncertainty_ns);
    }

    if (flags & GPS_CLOCK_HAS_DRIFT) {
        gpsClockObject->SetDriftInNsPerSec(clock->drift_nsps);
    }

    if (flags & GPS_CLOCK_HAS_DRIFT_UNCERTAINTY) {
        gpsClockObject->SetDriftUncertaintyInNsPerSec(clock->drift_uncertainty_nsps);
    }

    return gpsClockObject;
}

static AutoPtr<IGpsMeasurement> translate_gps_measurement(GpsMeasurement* measurement)
{
    AutoPtr<IGpsMeasurement> gpsMeasurementObject;
    CGpsMeasurement::New((IGpsMeasurement**)&gpsMeasurementObject);

    GpsMeasurementFlags flags = measurement->flags;

    gpsMeasurementObject->SetPrn(measurement->prn);
    gpsMeasurementObject->SetTimeOffsetInNs(measurement->time_offset_ns);
    gpsMeasurementObject->SetState(measurement->state);
    gpsMeasurementObject->SetReceivedGpsTowInNs(measurement->received_gps_tow_ns);
    gpsMeasurementObject->SetReceivedGpsTowUncertaintyInNs(measurement->received_gps_tow_uncertainty_ns);
    gpsMeasurementObject->SetCn0InDbHz(measurement->c_n0_dbhz);
    gpsMeasurementObject->SetPseudorangeRateInMetersPerSec(measurement->pseudorange_rate_mps);
    gpsMeasurementObject->SetPseudorangeRateUncertaintyInMetersPerSec(measurement->pseudorange_rate_uncertainty_mps);
    gpsMeasurementObject->SetAccumulatedDeltaRangeState(measurement->accumulated_delta_range_state);
    gpsMeasurementObject->SetAccumulatedDeltaRangeInMeters(measurement->accumulated_delta_range_m);
    gpsMeasurementObject->SetAccumulatedDeltaRangeUncertaintyInMeters(measurement->accumulated_delta_range_uncertainty_m);

    if (flags & GPS_MEASUREMENT_HAS_PSEUDORANGE) {
        gpsMeasurementObject->SetPseudorangeInMeters(measurement->pseudorange_m);
    }

    if (flags & GPS_MEASUREMENT_HAS_PSEUDORANGE_UNCERTAINTY) {
        gpsMeasurementObject->SetPseudorangeUncertaintyInMeters(measurement->pseudorange_uncertainty_m);
    }

    if (flags & GPS_MEASUREMENT_HAS_CODE_PHASE) {
        gpsMeasurementObject->SetCodePhaseInChips(measurement->code_phase_chips);
    }

    if (flags & GPS_MEASUREMENT_HAS_CODE_PHASE_UNCERTAINTY) {
        gpsMeasurementObject->SetCodePhaseUncertaintyInChips(measurement->code_phase_uncertainty_chips);
    }

    if (flags & GPS_MEASUREMENT_HAS_CARRIER_FREQUENCY) {
        gpsMeasurementObject->SetCarrierFrequencyInHz(measurement->carrier_frequency_hz);
    }

    if (flags & GPS_MEASUREMENT_HAS_CARRIER_CYCLES) {
        gpsMeasurementObject->SetCarrierCycles(measurement->carrier_cycles);
    }

    if (flags & GPS_MEASUREMENT_HAS_CARRIER_PHASE) {
        gpsMeasurementObject->SetCarrierPhase(measurement->carrier_phase);
    }

    if (flags & GPS_MEASUREMENT_HAS_CARRIER_PHASE_UNCERTAINTY) {
        gpsMeasurementObject->SetCarrierPhaseUncertainty(measurement->carrier_phase_uncertainty);
    }

    gpsMeasurementObject->SetLossOfLock(measurement->loss_of_lock);

    if (flags & GPS_MEASUREMENT_HAS_BIT_NUMBER) {
        gpsMeasurementObject->SetBitNumber(measurement->bit_number);
    }

    if (flags & GPS_MEASUREMENT_HAS_TIME_FROM_LAST_BIT) {
        gpsMeasurementObject->SetTimeFromLastBitInMs(measurement->time_from_last_bit_ms);
    }

    if (flags & GPS_MEASUREMENT_HAS_DOPPLER_SHIFT) {
        gpsMeasurementObject->SetDopplerShiftInHz(measurement->doppler_shift_hz);
    }

    if (flags & GPS_MEASUREMENT_HAS_DOPPLER_SHIFT_UNCERTAINTY) {
        gpsMeasurementObject->SetDopplerShiftUncertaintyInHz(measurement->doppler_shift_uncertainty_hz);
    }

    gpsMeasurementObject->SetMultipathIndicator(measurement->multipath_indicator);

    if (flags & GPS_MEASUREMENT_HAS_SNR) {
        gpsMeasurementObject->SetSnrInDb(measurement->snr_db);
    }

    if (flags & GPS_MEASUREMENT_HAS_ELEVATION) {
        gpsMeasurementObject->SetElevationInDeg(measurement->elevation_deg);
    }

    if (flags & GPS_MEASUREMENT_HAS_ELEVATION_UNCERTAINTY) {
        gpsMeasurementObject->SetElevationUncertaintyInDeg(measurement->elevation_uncertainty_deg);
    }

    if (flags & GPS_MEASUREMENT_HAS_AZIMUTH) {
        gpsMeasurementObject->SetAzimuthInDeg(measurement->azimuth_deg);
    }

    if (flags & GPS_MEASUREMENT_HAS_AZIMUTH_UNCERTAINTY) {
        gpsMeasurementObject->SetAzimuthUncertaintyInDeg(measurement->azimuth_uncertainty_deg);
    }

    gpsMeasurementObject->SetUsedInFix((flags & GPS_MEASUREMENT_HAS_USED_IN_FIX) && measurement->used_in_fix);

    return gpsMeasurementObject;
}

static AutoPtr<ArrayOf<IGpsMeasurement*> > translate_gps_measurements(GpsData* data)
{
    size_t measurementCount = data->measurement_count;
    if (measurementCount == 0) {
        return NULL;
    }

    AutoPtr<ArrayOf<IGpsMeasurement*> > gpsMeasurementArray = ArrayOf<IGpsMeasurement*>::Alloc(measurementCount);

    GpsMeasurement* gpsMeasurements = data->measurements;
    for (uint16_t i = 0; i < measurementCount; ++i) {
        AutoPtr<IGpsMeasurement> gpsMeasurement = translate_gps_measurement(&gpsMeasurements[i]);
        gpsMeasurementArray->Set(i, gpsMeasurement);
    }

    return gpsMeasurementArray;
}

static void measurement_callback(GpsData* data)
{
    if (data == NULL) {
        ALOGE("Invalid data provided to gps_measurement_callback");
        return;
    }

    if (data->size == sizeof(GpsData)) {
        AutoPtr<IGpsClock> gpsClock = translate_gps_clock(&data->clock);
        AutoPtr<ArrayOf<IGpsMeasurement*> > measurementArray = translate_gps_measurements(data);

        AutoPtr<IGpsMeasurementsEvent> gpsMeasurementsEvent;
        CGpsMeasurementsEvent::New(gpsClock, measurementArray, (IGpsMeasurementsEvent**)&gpsMeasurementsEvent);
        mCallbacksObj->ReportMeasurementData(gpsMeasurementsEvent);
    }
    else {
        ALOGE("Invalid GpsData size found in gps_measurement_callback, size=%d", data->size);
    }
}

GpsMeasurementCallbacks sGpsMeasurementCallbacks = {
    sizeof(GpsMeasurementCallbacks),
    measurement_callback,
};

Boolean GpsLocationProvider::Native_is_measurement_supported()
{
    if (sGpsMeasurementInterface != NULL) {
        return TRUE;
    }
    return FALSE;
}

Boolean GpsLocationProvider::Native_start_measurement_collection()
{
    if (sGpsMeasurementInterface == NULL) {
        ALOGE("Measurement interface is not available.");
        return FALSE;
    }

    int result = sGpsMeasurementInterface->init(&sGpsMeasurementCallbacks);
    if (result != GPS_GEOFENCE_OPERATION_SUCCESS) {
        ALOGE("An error has been found on GpsMeasurementInterface::init, status=%d", result);
        return FALSE;
    }

    return TRUE;
}

Boolean GpsLocationProvider::Native_stop_measurement_collection()
{
    if (sGpsMeasurementInterface == NULL) {
        ALOGE("Measurement interface not available");
        return FALSE;
    }

    sGpsMeasurementInterface->close();
    return TRUE;
}

static AutoPtr<IGpsNavigationMessage> translate_gps_navigation_message(GpsNavigationMessage* message)
{
    size_t dataLength = message->data_length;
    uint8_t* data = message->data;
    if (dataLength == 0 || data == NULL) {
        ALOGE("Invalid Navigation Message found: data=%p, length=%d", data, dataLength);
        return NULL;
    }

    AutoPtr<IGpsNavigationMessage> navigationMessageObject;
    CGpsNavigationMessage::New((IGpsNavigationMessage**)&navigationMessageObject);
    navigationMessageObject->SetType(message->type);
    navigationMessageObject->SetPrn(message->prn);
    navigationMessageObject->SetMessageId(message->message_id);
    navigationMessageObject->SetSubmessageId(message->submessage_id);

    AutoPtr<ArrayOf<Byte> > dataArray = ArrayOf<Byte>::Alloc((Byte*)data, dataLength);
    navigationMessageObject->SetData(dataArray);

    return navigationMessageObject;
}

static void navigation_message_callback(GpsNavigationMessage* message)
{
    if (message == NULL) {
        ALOGE("Invalid Navigation Message provided to callback");
        return;
    }

    if (message->size == sizeof(GpsNavigationMessage)) {
        AutoPtr<IGpsNavigationMessage> navigationMessage = translate_gps_navigation_message(message);

        AutoPtr<IGpsNavigationMessageEvent> navigationMessageEvent;
        CGpsNavigationMessageEvent::New(navigationMessage, (IGpsNavigationMessageEvent**)&navigationMessageEvent);
        mCallbacksObj->ReportNavigationMessage(navigationMessageEvent);
    }
    else {
        ALOGE("Invalid GpsNavigationMessage size found: %d", message->size);
    }
}

GpsNavigationMessageCallbacks sGpsNavigationMessageCallbacks = {
    sizeof(GpsNavigationMessageCallbacks),
    navigation_message_callback,
};

Boolean GpsLocationProvider::Native_is_navigation_message_supported()
{
    if (sGpsNavigationMessageInterface != NULL) {
        return TRUE;
    }
    return FALSE;
}

Boolean GpsLocationProvider::Native_start_navigation_message_collection()
{
    if (sGpsNavigationMessageInterface == NULL) {
        ALOGE("Navigation Message interface is not available.");
        return FALSE;
    }

    int result = sGpsNavigationMessageInterface->init(&sGpsNavigationMessageCallbacks);
    if (result != GPS_NAVIGATION_MESSAGE_OPERATION_SUCCESS) {
        ALOGE("An error has been found in %s: %d", __FUNCTION__, result);
        return FALSE;
    }

    return TRUE;
}

Boolean GpsLocationProvider::Native_stop_navigation_message_collection()
{
    if (sGpsNavigationMessageInterface == NULL) {
        ALOGE("Navigation Message interface is not available.");
        return FALSE;
    }

    sGpsNavigationMessageInterface->close();
    return TRUE;
}

void GpsLocationProvider::Native_configuration_update(
    /* [in] */ const String& configData)
{
    if (!sGnssConfigurationInterface) {
        ALOGE("no GPS configuration interface in configuraiton_update");
        return;
    }
    const char *data = configData.string();
    ALOGD("GPS configuration:\n %s", data);
    sGnssConfigurationInterface->configuration_update(
            data, configData.GetLength());
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
