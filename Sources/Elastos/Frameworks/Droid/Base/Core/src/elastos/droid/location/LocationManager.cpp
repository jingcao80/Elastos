
#include "Elastos.Droid.App.h"
#include "elastos/droid/location/LocationManager.h"
#include "elastos/droid/location/CLocation.h"
// #include "elastos/droid/location/CProviderProperties.h"
#include "elastos/droid/location/CLocationManagerGpsStatusListenerTransport.h"
#include "elastos/droid/location/CLocationManagerListenerTransport.h"
#include "elastos/droid/location/CLocationProvider.h"
#include "elastos/droid/location/CLocationRequest.h"
#include "elastos/droid/location/CGpsMeasurementListenerTransport.h"
#include "elastos/droid/location/CGpsNavigationMessageListenerTransport.h"
#include "elastos/droid/location/CGpsStatus.h"
#include "elastos/droid/location/Geofence.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CMessage.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Core::AutoLock;
using Elastos::Core::CString;
using Elastos::Core::IString;
using Elastos::Core::StringUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Location {

//============================================
//LocationManager::ListenerTransport::ListenerTransportHandler
//============================================

LocationManager::ListenerTransport::ListenerTransportHandler::ListenerTransportHandler(
    /* [in] */ ListenerTransport* listener)
    : mLTHost(listener)
{
}

ECode LocationManager::ListenerTransport::ListenerTransportHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    return mLTHost->_handleMessage(msg);
}

//============================================
//LocationManager::ListenerTransport
//============================================

const Int32 LocationManager::ListenerTransport::TYPE_LOCATION_CHANGED;
const Int32 LocationManager::ListenerTransport::TYPE_STATUS_CHANGED;
const Int32 LocationManager::ListenerTransport::TYPE_PROVIDER_ENABLED;
const Int32 LocationManager::ListenerTransport::TYPE_PROVIDER_DISABLED;

CAR_INTERFACE_IMPL_2(LocationManager::ListenerTransport, Object, IILocationListener, IBinder)

LocationManager::ListenerTransport::ListenerTransport()
{
}

ECode LocationManager::ListenerTransport::constructor(
    /* [in] */ ILocationManager* host,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
    mListener = listener;
    mLMHost = (LocationManager*)host;
    AutoPtr<ListenerTransportHandler> h;
    if (looper == NULL) {
        h = new ListenerTransportHandler(this);
        h->constructor();
    }
    else {
        h = new ListenerTransportHandler(this);
        h->constructor(looper);
    }
    mListenerHandler = IHandler::Probe(h);
    return NOERROR;
}

ECode LocationManager::ListenerTransport::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    AutoPtr<IMessage> msg = CMessage::Obtain();
    msg->SetWhat(TYPE_LOCATION_CHANGED);
    msg->SetObj(location);
    Boolean result = FALSE;
    mListenerHandler->SendMessage(msg, &result);
    return NOERROR;
}

ECode LocationManager::ListenerTransport::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    AutoPtr<IMessage> msg = CMessage::Obtain();
    msg->SetWhat(TYPE_STATUS_CHANGED);
    AutoPtr<IBundle> b;
    CBundle::New((IBundle**)&b);
    b->PutString(String("provider"), provider);
    b->PutInt32(String("status"), status);
    if (extras != NULL) {
        b->PutBundle(String("extras"), extras);
    }
    msg->SetObj(b);
    Boolean result = FALSE;
    mListenerHandler->SendMessage(msg, &result);
    return NOERROR;
}

ECode LocationManager::ListenerTransport::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    AutoPtr<IMessage> msg = CMessage::Obtain();
    msg->SetWhat(TYPE_PROVIDER_ENABLED);
    AutoPtr<IString> istr;
    CString::New(provider, (IString**)&istr);
    msg->SetObj(istr.Get());
    Boolean result = FALSE;
    mListenerHandler->SendMessage(msg, &result);
    return NOERROR;
}

LocationManager::ListenerTransport::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    AutoPtr<IMessage> msg = CMessage::Obtain();
    msg->SetWhat(TYPE_PROVIDER_DISABLED);
    AutoPtr<IString> istr;
    CString::New(provider, (IString**)&istr);
    msg->SetObj(istr.Get());
    Boolean result = FALSE;
    mListenerHandler->SendMessage(msg, &result);
    return NOERROR;
}

ECode LocationManager::ListenerTransport::_handleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case TYPE_LOCATION_CHANGED: {
            AutoPtr<IInterface> obj;
            AutoPtr<ILocation> location;
            msg->GetObj((IInterface**)&obj);
            CLocation::New(ILocation::Probe(obj), (ILocation**)&location);
            mListener->OnLocationChanged(location.Get());
            break;
        }
        case TYPE_STATUS_CHANGED: {
            AutoPtr<IInterface> obj;
            String provider;
            AutoPtr<IBundle> extras, b;
            msg->GetObj((IInterface**)&obj);
            b = IBundle::Probe(obj);
            b->GetString(String("provider"), &provider);
            Int32 status;
            b->GetInt32(String("status"), &status);
            b->GetBundle(String("extras"), (IBundle**)&extras);
            mListener->OnStatusChanged(provider, status, extras.Get());
            break;
        }
        case TYPE_PROVIDER_ENABLED: {
            AutoPtr<IInterface> obj;
            String provider;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
            cs->ToString(&provider);
            mListener->OnProviderEnabled(provider);
            break;
        }

        case TYPE_PROVIDER_DISABLED: {
            AutoPtr<IInterface> obj;
            String provider;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ICharSequence> cs = ICharSequence::Probe(obj);
            cs->ToString(&provider);
            mListener->OnProviderDisabled(provider);
            break;
        }
    }
    ECode ec = mLMHost->mService->LocationCallbackFinished(this);
    if (FAILED(ec)) {
        Logger::E(TAG, "locationCallbackFinished: RemoteException");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode LocationManager::ListenerTransport::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

//============================================
//LocationManager::GpsStatusListenerTransport::Nmea
//============================================

LocationManager::GpsStatusListenerTransport::Nmea::Nmea(
    /* [in] */ Int64 timestamp,
    /* [in] */ const String& nmea)
    : mTimestamp(timestamp)
    , mNmea(nmea)
{
}

//============================================
//LocationManager::GpsStatusListenerTransport::GpsHandler
//============================================

LocationManager::GpsStatusListenerTransport::GpsHandler::GpsHandler(
    /* [in] */ GpsStatusListenerTransport* host)
    : mGLTHost(host)
{
}

ECode LocationManager::GpsStatusListenerTransport::GpsHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    if (what == GpsStatusListenerTransport::NMEA_RECEIVED) {
        // {    AutoLock syncLock(mGLTHost->mNmeaBuffer);
        {    AutoLock syncLock(mLock);
            Int32 length;
            mGLTHost->mNmeaBuffer->GetSize(&length);
            for (Int32 i = 0; i < length; i++) {
                AutoPtr<IInterface> iinterface;
                mGLTHost->mNmeaBuffer->Get(i, (IInterface**)&iinterface);
                assert(0);
                // AutoPtr<Nmea> nmea = (Nema*)iinterface;
                // mGLTHost->mNmeaListener->OnNmeaReceived(nmea->mTimestamp, nmea->mNmea);
            }
            mGLTHost->mNmeaBuffer->Clear();
        }
    }
    else {
        // synchronize on mGpsStatus to ensure the data is copied atomically.
        // {    AutoLock syncLock(mGLTHost->mLMHost->mGpsStatus);
        {    AutoLock syncLock(mLock);
            mGLTHost->mListener->OnGpsStatusChanged(what);
        }
    }
    return NOERROR;
}

//============================================
//LocationManager::GpsStatusListenerTransport
//============================================

CAR_INTERFACE_IMPL_2(LocationManager::GpsStatusListenerTransport, Object, IIGpsStatusListener, IBinder)

LocationManager::GpsStatusListenerTransport::GpsStatusListenerTransport()
{
}

ECode LocationManager::GpsStatusListenerTransport::constructor(
    /* [in] */ ILocationManager* host,
    /* [in] */ IGpsStatusListener* listener)
{
    mListener = listener;
    mNmeaListener = NULL;
    mLMHost = (LocationManager*)host;
    AutoPtr<GpsHandler> gh = new GpsHandler(this);
    gh->constructor();
    mGpsHandler = IHandler::Probe(gh);
    return NOERROR;
}

ECode LocationManager::GpsStatusListenerTransport::constructor(
    /* [in] */ ILocationManager* host,
    /* [in] */ IGpsStatusNmeaListener* listener)
{
    mNmeaListener = listener;
    mListener = NULL;
    mLMHost = (LocationManager*)host;
    CArrayList::New((IArrayList**)&mNmeaBuffer);
    AutoPtr<GpsHandler> gh = new GpsHandler(this);
    mGpsHandler = IHandler::Probe(gh);
    return NOERROR;
}

ECode LocationManager::GpsStatusListenerTransport::OnGpsStarted()
{
    if (mListener != NULL) {
        AutoPtr<IMessage> msg = CMessage::Obtain();
        msg->SetWhat(IGpsStatus::GpsStatus_GPS_EVENT_STARTED);
        Boolean result = FALSE;
        mGpsHandler->SendMessage(msg, &result);
    }
    return NOERROR;
}

ECode LocationManager::GpsStatusListenerTransport::OnGpsStopped()
{
    if (mListener != NULL) {
        AutoPtr<IMessage> msg = CMessage::Obtain();
        msg->SetWhat(IGpsStatus::GpsStatus_GPS_EVENT_STOPPED);
        Boolean result = FALSE;
        mGpsHandler->SendMessage(msg, &result);
    }
    return NOERROR;
}

ECode LocationManager::GpsStatusListenerTransport::OnFirstFix(
    /* [in] */ Int32 ttff)
{
    if (mListener != NULL) {
        mLMHost->mGpsStatus->SetTimeToFirstFix(ttff);
        AutoPtr<IMessage> msg = CMessage::Obtain();
        msg->SetWhat(IGpsStatus::GpsStatus_GPS_EVENT_FIRST_FIX);
        Boolean result = FALSE;
        mGpsHandler->SendMessage(msg, &result);
    }
    return NOERROR;
}

ECode LocationManager::GpsStatusListenerTransport::OnSvStatusChanged(
    /* [in] */ Int32 svCount,
    /* [in] */ ArrayOf<Int32>* prns,
    /* [in] */ ArrayOf<Float>* snrs,
    /* [in] */ ArrayOf<Float>* elevations,
    /* [in] */ ArrayOf<Float>* azimuths,
    /* [in] */ Int32 ephemerisMask,
    /* [in] */ Int32 almanacMask,
    /* [in] */ Int32 usedInFixMask)
{
    if (mListener != NULL) {
        mLMHost->mGpsStatus->SetStatus(svCount, prns, snrs, elevations, azimuths,
                ephemerisMask, almanacMask, usedInFixMask);
        AutoPtr<IMessage> msg = CMessage::Obtain();
        msg->SetWhat(IGpsStatus::GpsStatus_GPS_EVENT_SATELLITE_STATUS);
        // remove any SV status messages already in the queue
        mGpsHandler->RemoveMessages(IGpsStatus::GpsStatus_GPS_EVENT_SATELLITE_STATUS);
        Boolean result = FALSE;
        mGpsHandler->SendMessage(msg, &result);
    }
    return NOERROR;
}

ECode LocationManager::GpsStatusListenerTransport::OnNmeaReceived(
    /* [in] */ Int64 timestamp,
    /* [in] */ const String& nmea)
{
    if (mNmeaListener != NULL)
    {
        {    AutoLock syncLock(mNmeaBuffer);
            assert(0);
            // AutoPtr<Nmea> nmea = new Nmea(timestamp, nmea);
            // Boolean result = FALSE;
            // mNmeaBuffer->Add(IInterface::Probe(nmea), &result);
        }
        AutoPtr<IMessage> msg = CMessage::Obtain();
        msg->SetWhat(NMEA_RECEIVED);
        // remove any NMEA_RECEIVED messages already in the queue
        mGpsHandler->RemoveMessages(NMEA_RECEIVED);
        Boolean result = FALSE;
        mGpsHandler->SendMessage(msg, &result);
    }
    return NOERROR;
}

ECode LocationManager::GpsStatusListenerTransport::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

//============================================
//LocationManager
//============================================

const String LocationManager::TAG("LocationManager");

CAR_INTERFACE_IMPL(LocationManager, Object, ILocationManager)

LocationManager::LocationManager()
{
}

ECode LocationManager::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IILocationManager* service)
{
    mService = service;
    mContext = context;
    CGpsStatus::New((IGpsStatus**)&mGpsStatus);
    CGpsMeasurementListenerTransport::New(mContext, mService, (IGpsMeasurementListenerTransport**)&mGpsMeasurementListenerTransport);
    CGpsNavigationMessageListenerTransport::New(mContext, mService, (IGpsNavigationMessageListenerTransport**)&mGpsNavigationMessageListenerTransport);
    return NOERROR;
}

AutoPtr<ILocationProvider> LocationManager::CreateProvider(
    /* [in] */ const String& name,
    /* [in] */ IProviderProperties* properties)
{
    AutoPtr<ILocationProvider> tmp;
    CLocationProvider::New(name, properties, (ILocationProvider**)&tmp);
    return tmp;
}

ECode LocationManager::GetAllProviders(
    /* [out] */ IList** allProvders)
{
    VALIDATE_NOT_NULL(allProvders)
    *allProvders = NULL;
    ECode ec = mService->GetAllProviders(allProvders);
    if (FAILED(ec)) {
        Logger::E(TAG, "RemoteException");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode LocationManager::GetProviders(
    /* [in] */ Boolean enabledOnly,
    /* [out] */ IList** providers)
{
    VALIDATE_NOT_NULL(providers)
    *providers = NULL;
    ECode ec = mService->GetProviders(NULL, enabledOnly, providers);
    if (FAILED(ec)) {
        Logger::E(TAG, "RemoteException");
        return E_REMOTE_EXCEPTION;
    }
    return NOERROR;
}

ECode LocationManager::GetProvider(
    /* [in] */ const String& name,
    /* [out] */ ILocationProvider** provider)
{
    assert(provider != NULL);
    FAIL_RETURN(CheckProvider(name));

    AutoPtr<IProviderProperties> properties;
    ECode ec = mService->GetProviderProperties(name, (IProviderProperties**)&properties);
    if (FAILED(ec) || properties == NULL) {
        *provider = NULL;
        return NOERROR;
    }

    AutoPtr<ILocationProvider> temp = CreateProvider(name, properties);
    *provider = temp;
    REFCOUNT_ADD(*provider);

    return NOERROR;
}

ECode LocationManager::GetProviders(
    /* [in] */ ICriteria* criteria,
    /* [in] */ Boolean enabledOnly,
    /* [out] */ IList** providers)
{
    assert(providers != NULL);
    *providers = NULL;
    FAIL_RETURN(CheckCriteria(criteria));

    return mService->GetProviders(criteria, enabledOnly, providers);
}

ECode LocationManager::GetBestProvider(
    /* [in] */ ICriteria* criteria,
    /* [in] */ Boolean enabledOnly,
    /* [out] */ String* provider)
{
    assert(provider != NULL);
    *provider = "";
    FAIL_RETURN(CheckCriteria(criteria));

    return mService->GetBestProvider(criteria, enabledOnly, provider);
}

ECode LocationManager::RequestLocationUpdates(
    /* [in] */ const String& provider,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ ILocationListener* listener)
{
    FAIL_RETURN(CheckProvider(provider));
    FAIL_RETURN(CheckListener(listener));

    AutoPtr<ILocationRequest> request = CLocationRequest::CreateFromDeprecatedProvider(
            provider, minTime, minDistance, FALSE);
    RequestLocationUpdates(request, listener, NULL, NULL);

    return NOERROR;
}

ECode LocationManager::RequestLocationUpdates(
    /* [in] */ const String& provider,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
    FAIL_RETURN(CheckProvider(provider));
    FAIL_RETURN(CheckListener(listener));

    AutoPtr<ILocationRequest> request = CLocationRequest::CreateFromDeprecatedProvider(
            provider, minTime, minDistance, FALSE);
    RequestLocationUpdates(request, listener, looper, NULL);

    return NOERROR;
}

ECode LocationManager::RequestLocationUpdates(
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ ICriteria* criteria,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
    FAIL_RETURN(CheckCriteria(criteria));
    FAIL_RETURN(CheckListener(listener));

    AutoPtr<ILocationRequest> request = CLocationRequest::CreateFromDeprecatedCriteria(
            criteria, minTime, minDistance, FALSE);
    RequestLocationUpdates(request, listener, looper, NULL);

    return NOERROR;
}

ECode LocationManager::RequestLocationUpdates(
    /* [in] */ const String& provider,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ IPendingIntent* intent)
{
    FAIL_RETURN(CheckProvider(provider));
    FAIL_RETURN(CheckPendingIntent(intent));

    AutoPtr<ILocationRequest> request = CLocationRequest::CreateFromDeprecatedProvider(
            provider, minTime, minDistance, FALSE);
    RequestLocationUpdates(request, NULL, NULL, intent);

    return NOERROR;
}

ECode LocationManager::RequestLocationUpdates(
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ ICriteria* criteria,
    /* [in] */ IPendingIntent* intent)
{
    FAIL_RETURN(CheckCriteria(criteria));
    FAIL_RETURN(CheckPendingIntent(intent));

    AutoPtr<ILocationRequest> request = CLocationRequest::CreateFromDeprecatedCriteria(
            criteria, minTime, minDistance, FALSE);
    RequestLocationUpdates(request, NULL, NULL, intent);

    return NOERROR;
}

ECode LocationManager::RequestSingleUpdate(
    /* [in] */ const String& provider,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
    FAIL_RETURN(CheckProvider(provider));
    FAIL_RETURN(CheckListener(listener));

    AutoPtr<ILocationRequest> request = CLocationRequest::CreateFromDeprecatedProvider(
            provider, 0, 0, TRUE);
    RequestLocationUpdates(request, listener, looper, NULL);

    return NOERROR;
}

ECode LocationManager::RequestSingleUpdate(
    /* [in] */ ICriteria* criteria,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
    FAIL_RETURN(CheckCriteria(criteria));
    FAIL_RETURN(CheckListener(listener));

    AutoPtr<ILocationRequest> request = CLocationRequest::CreateFromDeprecatedCriteria(
            criteria, 0, 0, TRUE);
    RequestLocationUpdates(request, listener, looper, NULL);

    return NOERROR;
}

ECode LocationManager::RequestSingleUpdate(
    /* [in] */ const String& provider,
    /* [in] */ IPendingIntent* intent)
{
    FAIL_RETURN(CheckProvider(provider));
    FAIL_RETURN(CheckPendingIntent(intent));

    AutoPtr<ILocationRequest> request = CLocationRequest::CreateFromDeprecatedProvider(
            provider, 0, 0, TRUE);
    RequestLocationUpdates(request, NULL, NULL, intent);

    return NOERROR;
}

ECode LocationManager::RequestSingleUpdate(
    /* [in] */ ICriteria* criteria,
    /* [in] */ IPendingIntent* intent)
{
    FAIL_RETURN(CheckCriteria(criteria));
    FAIL_RETURN(CheckPendingIntent(intent));

    AutoPtr<ILocationRequest> request = CLocationRequest::CreateFromDeprecatedCriteria(
            criteria, 0, 0, TRUE);
    RequestLocationUpdates(request, NULL, NULL, intent);

    return NOERROR;
}

ECode LocationManager::RequestLocationUpdates(
    /* [in] */ ILocationRequest* request,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
    FAIL_RETURN(CheckListener(listener));
    RequestLocationUpdates(request, listener, looper, NULL);

    return NOERROR;
}

ECode LocationManager::RequestLocationUpdates(
    /* [in] */ ILocationRequest* request,
    /* [in] */ IPendingIntent* intent)
{
    FAIL_RETURN(CheckPendingIntent(intent));
    RequestLocationUpdates(request, NULL, NULL, intent);

    return NOERROR;
}

ECode LocationManager::RemoveUpdates(
    /* [in] */ ILocationListener* listener)
{
    FAIL_RETURN(CheckListener(listener));
    String packageName;
    mContext->GetPackageName(&packageName);
    AutoPtr<IILocationListener> transport;
    {    AutoLock syncLock(this);
        HashMap<AutoPtr<ILocationListener>, AutoPtr<IILocationListener> >::Iterator it
                = mListeners.Find(listener);
        if (it != mListeners.End()) {
            transport = it->mSecond;
            mListeners.Erase(it);
        }
    }

    if (transport == NULL) return NOERROR;

    return mService->RemoveUpdates(transport, NULL, packageName);
}

ECode LocationManager::RemoveUpdates(
    /* [in] */ IPendingIntent* intent)
{
    FAIL_RETURN(CheckPendingIntent(intent));
    String packageName;
    mContext->GetPackageName(&packageName);
    return mService->RemoveUpdates(NULL, intent, packageName);
}

ECode LocationManager::AddProximityAlert(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Float radius,
    /* [in] */ Int64 expiration,
    /* [in] */ IPendingIntent* intent)
{
    FAIL_RETURN(CheckPendingIntent(intent));
    if (expiration < 0) expiration = Elastos::Core::Math::INT64_MAX_VALUE;

    AutoPtr<IGeofence> fence;
    Geofence::CreateCircle(latitude, longitude, radius, (IGeofence**)&fence);
    AutoPtr<ILocationRequest> request;
    CLocationRequest::New((ILocationRequest**)&request);
    request->SetExpireIn(expiration);

    String packageName;
    mContext->GetPackageName(&packageName);
    return mService->RequestGeofence(request, fence, intent, packageName);
}

ECode LocationManager::AddGeofence(
    /* [in] */ ILocationRequest* request,
    /* [in] */ IGeofence* fence,
    /* [in] */ IPendingIntent* intent)
{
    FAIL_RETURN(CheckPendingIntent(intent));
    FAIL_RETURN(CheckGeofence(fence));

    String packageName;
    mContext->GetPackageName(&packageName);

    return mService->RequestGeofence(request, fence, intent, packageName);
}

ECode LocationManager::RemoveProximityAlert(
    /* [in] */ IPendingIntent* intent)
{
    FAIL_RETURN(CheckPendingIntent(intent));
    String packageName;
    mContext->GetPackageName(&packageName);

    return mService->RemoveGeofence(NULL, intent, packageName);
}

ECode LocationManager::RemoveGeofence(
    /* [in] */ IGeofence* fence,
    /* [in] */ IPendingIntent* intent)
{
    FAIL_RETURN(CheckPendingIntent(intent));
    FAIL_RETURN(CheckGeofence(fence));
    String packageName;
    mContext->GetPackageName(&packageName);

    return mService->RemoveGeofence(fence, intent, packageName);
}

ECode LocationManager::RemoveAllGeofences(
    /* [in] */ IPendingIntent* intent)
{
    FAIL_RETURN(CheckPendingIntent(intent));
    String packageName;
    mContext->GetPackageName(&packageName);

    return mService->RemoveGeofence(NULL, intent, packageName);
}

ECode LocationManager::IsProviderEnabled(
    /* [in] */ const String& provider,
    /* [out] */ Boolean* result)
{
    assert(result != NULL);
    *result = FALSE;
    FAIL_RETURN(CheckProvider(provider));
    return mService->IsProviderEnabled(provider, result);
}

ECode LocationManager::GetLastLocation(
    /* [out] */ ILocation** location)
{
    VALIDATE_NOT_NULL(location);
    *location = NULL;
    String packageName;
    mContext->GetPackageName(&packageName);

    return mService->GetLastLocation(NULL, packageName, location);
}

ECode LocationManager::GetLastKnownLocation(
    /* [in] */ const String& provider,
    /* [out] */ ILocation** location)
{
    VALIDATE_NOT_NULL(location);
    *location = NULL;
    FAIL_RETURN(CheckProvider(provider));
    String packageName;
    mContext->GetPackageName(&packageName);
    AutoPtr<ILocationRequest> request = CLocationRequest::CreateFromDeprecatedProvider(
            provider, 0, 0, TRUE);

    return mService->GetLastLocation(request, packageName, location);
}

ECode LocationManager::AddTestProvider(
    /* [in] */ const String& name,
    /* [in] */ Boolean requiresNetwork,
    /* [in] */ Boolean requiresSatellite,
    /* [in] */ Boolean requiresCell,
    /* [in] */ Boolean hasMonetaryCost,
    /* [in] */ Boolean supportsAltitude,
    /* [in] */ Boolean supportsSpeed,
    /* [in] */ Boolean supportsBearing,
    /* [in] */ Int32 powerRequirement,
    /* [in] */ Int32 accuracy)
{
#if 0 //CProviderProperties has not been implemented
    AutoPtr<IProviderProperties> properties;
    CProviderProperties::New(requiresNetwork,
            requiresSatellite, requiresCell, hasMonetaryCost, supportsAltitude, supportsSpeed,
            supportsBearing, powerRequirement, accuracy, (IProviderProperties**)&properties);
    Boolean match;
    StringUtils::Matches(name, ILocationProvider::BAD_CHARS_REGEX, &match);
    if(match)
    {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return mService->AddTestProvider(name, properties);
#endif
    return E_NOT_IMPLEMENTED;
}

ECode LocationManager::RemoveTestProvider(
    /* [in] */ const String& provider)
{
    return mService->RemoveTestProvider(provider);
}

ECode LocationManager::SetTestProviderLocation(
    /* [in] */ const String& provider,
    /* [in] */ ILocation* loc)
{
    VALIDATE_NOT_NULL(loc);

    Boolean result = FALSE;
    loc->IsComplete(&result);
    if (!result) {

        AutoPtr<IApplicationInfo> appInfo;
        mContext->GetApplicationInfo((IApplicationInfo**)&appInfo);
        Int32 targetSdkVersion;
        appInfo->GetTargetSdkVersion(&targetSdkVersion);
        if (targetSdkVersion <= Build::VERSION_CODES::JELLY_BEAN) {
            // just log on old platform (for backwards compatibility)
            loc->MakeComplete();
        }
        else {
            // really throw it!
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    return mService->SetTestProviderLocation(provider, loc);
}

ECode LocationManager::ClearTestProviderLocation(
    /* [in] */ const String& provider)
{
    return mService->ClearTestProviderLocation(provider);
}

ECode LocationManager::SetTestProviderEnabled(
    /* [in] */ const String& provider,
    /* [in] */ Boolean enabled)
{
    return mService->SetTestProviderEnabled(provider, enabled);
}

ECode LocationManager::ClearTestProviderEnabled(
    /* [in] */ const String& provider)
{
    return mService->ClearTestProviderEnabled(provider);
}

ECode LocationManager::SetTestProviderStatus(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras,
    /* [in] */ Int64 updateTime)
{
    return mService->SetTestProviderStatus(provider, status, extras, updateTime);
}

ECode LocationManager::ClearTestProviderStatus(
    /* [in] */ const String& provider)
{
    return mService->ClearTestProviderStatus(provider);
}

ECode LocationManager::AddGpsStatusListener(
    /* [in] */ IGpsStatusListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    HashMap<AutoPtr<IGpsStatusListener>, AutoPtr<GpsStatusListenerTransport> >::Iterator it =
                  mGpsStatusListeners.Find(listener);
    if (it != mGpsStatusListeners.End() && (it->mSecond != NULL)) {
        // listener is already registered
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<IIGpsStatusListener> transport;
    CLocationManagerGpsStatusListenerTransport::New(this, listener, (IIGpsStatusListener**)&transport);

    String packageName;
    mContext->GetPackageName(&packageName);
    ECode ec = mService->AddGpsStatusListener(IIGpsStatusListener::Probe(transport), packageName, result);
    if(FAILED(ec)) {
        return ec;
    }
    if (*result) {
        mGpsStatusListeners[listener] = (GpsStatusListenerTransport*)(transport.Get());
    }
    return NOERROR;
}

ECode LocationManager::RemoveGpsStatusListener(
    /* [in] */ IGpsStatusListener* listener)
{
    HashMap<AutoPtr<IGpsStatusListener>, AutoPtr<GpsStatusListenerTransport> >::Iterator it =
                  mGpsStatusListeners.Find(listener);
    AutoPtr<GpsStatusListenerTransport> transport;
    if (it != mGpsStatusListeners.End()) {
        transport = it->mSecond;
        mGpsStatusListeners.Erase(it);
    }

    if (transport != NULL) {
        mService->RemoveGpsStatusListener(IIGpsStatusListener::Probe(transport));
    }
    return NOERROR;
}

ECode LocationManager::AddNmeaListener(
    /* [in] */ IGpsStatusNmeaListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    HashMap<AutoPtr<IGpsStatusNmeaListener>, AutoPtr<GpsStatusListenerTransport> >::Iterator it
              = mNmeaListeners.Find(listener);
    if (it != mNmeaListeners.End() && (it->mSecond != NULL)) {
        // listener is already registered
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<IIGpsStatusListener> transport;
    CLocationManagerGpsStatusListenerTransport::New(this, listener, (IIGpsStatusListener**)&transport);

    String packageName;
    mContext->GetPackageName(&packageName);
    ECode ec = mService->AddGpsStatusListener(IIGpsStatusListener::Probe(transport), packageName, result);

    if(FAILED(ec)) {
        return NOERROR;
    }

    if (*result) {
        mNmeaListeners[listener] = (GpsStatusListenerTransport*)(transport.Get());
    }
    return NOERROR;
}

ECode LocationManager::RemoveNmeaListener(
    /* [in] */ IGpsStatusNmeaListener* listener)
{
    HashMap<AutoPtr<IGpsStatusNmeaListener>, AutoPtr<GpsStatusListenerTransport> >::Iterator it
              = mNmeaListeners.Find(listener);

    if (it != mNmeaListeners.End() && it->mSecond) {
        AutoPtr<GpsStatusListenerTransport> transport = it->mSecond;
        mNmeaListeners.Erase(it);
        mService->RemoveGpsStatusListener(IIGpsStatusListener::Probe(transport));
    }
    return NOERROR;
}

ECode LocationManager::AddGpsMeasurementListener(
    /* [in] */ IGpsMeasurementsEventListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ILocalListenerHelper> lh = ILocalListenerHelper::Probe(mGpsMeasurementListenerTransport);
    lh->Add(listener, result);
    return NOERROR;
}

ECode LocationManager::RemoveGpsMeasurementListener(
    /* [in] */ IGpsMeasurementsEventListener* listener)
{
    AutoPtr<ILocalListenerHelper> lh = ILocalListenerHelper::Probe(mGpsMeasurementListenerTransport);
    lh->Remove(listener);
    return NOERROR;
}

ECode LocationManager::AddGpsNavigationMessageListener(
    /* [in] */ IGpsNavigationMessageEventListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ILocalListenerHelper> lh = ILocalListenerHelper::Probe(mGpsNavigationMessageListenerTransport);
    lh->Add(listener, result);
    return NOERROR;
}

ECode LocationManager::RemoveGpsNavigationMessageListener(
    /* [in] */ IGpsNavigationMessageEventListener* listener)
{
    AutoPtr<ILocalListenerHelper> lh = ILocalListenerHelper::Probe(mGpsNavigationMessageListenerTransport);
    lh->Remove(listener);
    return NOERROR;
}

ECode LocationManager::GetGpsStatus(
    /* [in] */ IGpsStatus* inStatus,
    /* [out] */ IGpsStatus** outStatus)
{
    VALIDATE_NOT_NULL(outStatus);

    AutoPtr<IGpsStatus> status = inStatus;
    if (status == NULL) {
        FAIL_RETURN(CGpsStatus::New((IGpsStatus**)&status))
    }
    status->SetStatus(mGpsStatus);

    *outStatus = status;
    REFCOUNT_ADD(*outStatus);

    return NOERROR;
}

ECode LocationManager::SendExtraCommand(
    /* [in] */ const String& provider,
    /* [in] */ const String& command,
    /* [in] */ IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return mService->SendExtraCommand(provider, command, &extras, result);
}

ECode LocationManager::SendNiResponse(
    /* [in] */ Int32 notifId,
    /* [in] */ Int32 userResponse,
    /* [out] */ Boolean* result)
{
    *result = FALSE;
    return mService->SendNiResponse(notifId, userResponse, result);
}

AutoPtr<IILocationListener> LocationManager::WrapListener(
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
    if (listener == NULL) return NULL;
    {    AutoLock syncLock(this);
        AutoPtr<ILocationListener> tempKey = listener;
        HashMap<AutoPtr<ILocationListener>, AutoPtr<IILocationListener> >::Iterator it =
                mListeners.Find(tempKey);
        AutoPtr<IILocationListener> transport;
        if ((it == mListeners.End()) || (it->mSecond == NULL)) {
            AutoPtr<IILocationListener> lt;
            CLocationManagerListenerTransport::New(this, listener, looper, (IILocationListener**)&lt);
            transport = lt.Get();
        }
        mListeners[tempKey] = transport;
        return transport;
    }
    return NULL;
}

void LocationManager::RequestLocationUpdates(
    /* [in] */ ILocationRequest* request,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper,
    /* [in] */ IPendingIntent* intent)
{
    String packageName;
    mContext->GetPackageName(&packageName);

    // wrap the listener class
    AutoPtr<IILocationListener> transport = WrapListener(listener, looper);
    mService->RequestLocationUpdates(request, transport, intent, packageName);
}

ECode LocationManager::CheckProvider(
    /* [in] */ const String& provider)
{
    if(provider.IsNull())
    {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode LocationManager::CheckCriteria(
    /* [in] */ ICriteria* criteria)
{
    if(criteria == NULL)
    {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode LocationManager::CheckListener(
    /* [in] */ ILocationListener* listener)
{
    if(listener == NULL)
    {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode LocationManager::CheckPendingIntent(
    /* [in] */ IPendingIntent* intent)
{
    if(intent == NULL)
    {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Boolean result = FALSE;
    intent->IsTargetedToPackage(&result);
    if (!result) {
        AutoPtr<IApplicationInfo> aInfo;
        mContext->GetApplicationInfo((IApplicationInfo**)&aInfo);
        Int32 version;
        aInfo->GetTargetSdkVersion(&version);
        if(version > Build::VERSION_CODES::JELLY_BEAN)
        {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode LocationManager::CheckGeofence(
    /* [in] */ IGeofence* fence)
{
    if(fence == NULL)
    {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Location
} // namepsace Droid
} // namespace Elastos