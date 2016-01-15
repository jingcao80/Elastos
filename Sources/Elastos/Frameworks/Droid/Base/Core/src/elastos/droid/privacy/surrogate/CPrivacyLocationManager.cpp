
#include "elastos/droid/ext/frameworkdef.h"
#include "CPrivacyLocationManager.h"
#include "CPrivacySettingsManager.h"
#include "elastos/droid/location/CLocation.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/Binder.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Location::CLocation;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::ServiceManager;


namespace Elastos {
namespace Droid {
namespace Privacy {
namespace Surrogate {

const String CPrivacyLocationManager::TAG("PrivacyLocationManager");
const Int32 CPrivacyLocationManager::CUSTOM_LOCATION_UPDATE_COUNT = 5;

ECode CPrivacyLocationManager::constructor(
    /* [in] */ IILocationManager* service,
    /* [in] */ IContext* context)
{
    LocationManager::Init(context, service);
    mContext = context;
    AutoPtr<IIPrivacySettingsManager> mgr = (IIPrivacySettingsManager*)ServiceManager::GetService(String("privacy")).Get();
    return CPrivacySettingsManager::New(context, mgr, (IPrivacySettingsManager**)&mPrivacySetMan);
}

ECode CPrivacyLocationManager::GetAllProviders(
    /* [out] */ IObjectContainer** allProvoider)
{
    VALIDATE_NOT_NULL(allProvoider)
    return LocationManager::GetAllProviders(allProvoider);
}

ECode CPrivacyLocationManager::GetProviders(
    /* [in] */ Boolean enabledOnly,
    /* [out] */ IObjectContainer** providers)
{
    VALIDATE_NOT_NULL(providers)
    return LocationManager::GetProviders(enabledOnly, providers);
}

ECode CPrivacyLocationManager::GetProvider(
    /* [in] */ const String& name,
    /* [out] */ ILocationProvider** provider)
{
    VALIDATE_NOT_NULL(provider)

    if (name.IsNull()) {
        return LocationManager::GetProvider(name, provider);
    }

    AutoPtr<IPrivacySettings> pSet;
    String packageName;
    mContext->GetPackageName(&packageName);
    mPrivacySetMan->GetSettings(packageName, Binder::GetCallingUid(), (IPrivacySettings**)&pSet);
    AutoPtr<ILocationProvider> output;

    if (pSet != NULL) {
        if (name.Equals(ILocationManager::GPS_PROVIDER)) {
            switch (GetLocationSetting(0, pSet)) {
                case IPrivacySettings::REAL:
                case IPrivacySettings::CUSTOM:
                case IPrivacySettings::RANDOM:
                    LocationManager::GetProvider(name, (ILocationProvider**)&output);
                    break;
                case IPrivacySettings::EMPTY:
                    break;
            }
        } else if (name.Equals(ILocationManager::NETWORK_PROVIDER)) {
            switch (GetLocationSetting(1, pSet)) {
                case IPrivacySettings::REAL:
                case IPrivacySettings::CUSTOM:
                case IPrivacySettings::RANDOM:
                    LocationManager::GetProvider(name, (ILocationProvider**)&output);
                    break;
                case IPrivacySettings::EMPTY:
                    break;
            }
        } else if (name.Equals(ILocationManager::PASSIVE_PROVIDER)) { // could get location from any of above
            if (GetLocationSetting(0, pSet) == IPrivacySettings::REAL ||
                    GetLocationSetting(1, pSet) == IPrivacySettings::REAL) {
                LocationManager::GetProvider(name, (ILocationProvider**)&output);
            }
        }
    } else {
        LocationManager::GetProvider(name, (ILocationProvider**)&output);
    }

    *provider = output;
    REFCOUNT_ADD(*provider)
    return NOERROR;
}

ECode CPrivacyLocationManager::GetProviders(
    /* [in] */ ICriteria* criteria,
    /* [in] */ Boolean enabledOnly,
    /* [out] */ IObjectContainer** providers)
{
    VALIDATE_NOT_NULL(providers)
    return LocationManager::GetProviders(criteria, enabledOnly, providers);
}

ECode CPrivacyLocationManager::GetBestProvider(
    /* [in] */ ICriteria* criteria,
    /* [in] */ Boolean enabledOnly,
    /* [out] */ String* provider)
{
    VALIDATE_NOT_NULL(provider)
    return LocationManager::GetBestProvider(criteria, enabledOnly, provider);
}

ECode CPrivacyLocationManager::RequestLocationUpdates(
    /* [in] */ const String& provider,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ ILocationListener* listener)
{
    if (provider.IsNull() || listener == NULL) {
        return LocationManager::RequestLocationUpdates(provider, minTime, minDistance, listener);
    }

    Boolean result = FALSE;
    RequestLocationUpdates(provider, listener, NULL, &result);
    if (result) {
        return NOERROR;
    }
    return LocationManager::RequestLocationUpdates(provider, minTime, minDistance, listener);
}

ECode CPrivacyLocationManager::RequestLocationUpdates(
    /* [in] */ const String& provider,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
//    if (provider.IsNull() || listener == null) {
//        super.requestLocationUpdates(provider, minTime, minDistance, listener, looper);
//        return;
//    }
//    if (requestLocationUpdates(provider, listener, null)) return;
//    super.requestLocationUpdates(provider, minTime, minDistance, listener, looper);
    return E_NOT_IMPLEMENTED;
}

ECode CPrivacyLocationManager::RequestLocationUpdates(
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ ICriteria* criteria,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
//    if (criteria.IsNull() || listener == null) {
//        super.requestLocationUpdates(minTime, minDistance, criteria, listener, looper);
//        return;
//    }
//    if (requestLocationUpdates(criteria, listener, null)) return;
//    super.requestLocationUpdates(minTime, minDistance, criteria, listener, looper);
    return E_NOT_IMPLEMENTED;
}

ECode CPrivacyLocationManager::RequestLocationUpdates(
    /* [in] */ const String& provider,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ IPendingIntent* intent)
{
    if (provider.IsNull() || intent == NULL) {
        return LocationManager::RequestLocationUpdates(provider, minTime, minDistance, intent);
    }

    Boolean result = FALSE;
    RequestLocationUpdates(provider, NULL, intent, &result);

    if (result) {
        return NOERROR;
    }

    return LocationManager::RequestLocationUpdates(provider, minTime, minDistance, intent);
}

ECode CPrivacyLocationManager::RequestLocationUpdates(
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ ICriteria* criteria,
    /* [in] */ IPendingIntent* intent)
{
    if (criteria == NULL || intent == NULL) {
        return LocationManager::RequestLocationUpdates(minTime, minDistance, criteria, intent);
    }

    Boolean result = FALSE;
    RequestLocationUpdates(criteria, NULL, intent, &result);

    if (result) {
        return NOERROR;
    }

    return LocationManager::RequestLocationUpdates(minTime, minDistance, criteria, intent);
}

ECode CPrivacyLocationManager::RequestSingleUpdate(
    /* [in] */ const String& provider,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
//    if (provider.IsNull() || listener == null) {
//        super.requestSingleUpdate(provider, listener, looper);
//        return;
//    }
//    if (requestLocationUpdates(provider, listener, null)) return;
//    super.requestSingleUpdate(provider, listener, looper);
    return E_NOT_IMPLEMENTED;
}

ECode CPrivacyLocationManager::RequestSingleUpdate(
    /* [in] */ ICriteria* criteria,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
//    if (criteria == null || listener == null) {
//        super.requestSingleUpdate(criteria, listener, looper);
//        return;
//    }
//    if (requestLocationUpdates(criteria, listener, null)) return;
//    super.requestSingleUpdate(criteria, listener, looper);
    return E_NOT_IMPLEMENTED;
}

ECode CPrivacyLocationManager::RequestSingleUpdate(
    /* [in] */ const String& provider,
    /* [in] */ IPendingIntent* intent)
{
    if (provider.IsNull() || intent == NULL) {
        return LocationManager::RequestSingleUpdate(provider, intent);
    }

    Boolean result = FALSE;
    RequestLocationUpdates(provider, NULL, intent, &result);

    if (result) {
        return NOERROR;
    }

    return LocationManager::RequestSingleUpdate(provider, intent);
}

ECode CPrivacyLocationManager::RequestSingleUpdate(
    /* [in] */ ICriteria* criteria,
    /* [in] */ IPendingIntent* intent)
{
    if (criteria == NULL || intent == NULL) {
        return LocationManager::RequestSingleUpdate(criteria, intent);
    }

    Boolean result = FALSE;
    RequestLocationUpdates(criteria, NULL, intent, &result);

    if (result) {
        return NOERROR;
    }

    return LocationManager::RequestSingleUpdate(criteria, intent);
}

ECode CPrivacyLocationManager::RequestLocationUpdates(
    /* [in] */ ILocationRequest* request,
    /* [in] */ IPendingIntent* intent)
{
    return LocationManager::RequestLocationUpdates(request, intent);
}

ECode CPrivacyLocationManager::RemoveUpdates(
    /* [in] */ ILocationListener* listener)
{
    return LocationManager::RemoveUpdates(listener);
}

ECode CPrivacyLocationManager::RemoveUpdates(
    /* [in] */ IPendingIntent* intent)
{
    return LocationManager::RemoveUpdates(intent);
}

ECode CPrivacyLocationManager::AddProximityAlert(
    /* [in] */ Double latitude,
    /* [in] */ Double longitude,
    /* [in] */ Float radius,
    /* [in] */ Int64 expiration,
    /* [in] */ IPendingIntent* intent)
{
    return LocationManager::AddProximityAlert(latitude, longitude, radius, expiration, intent);
}

ECode CPrivacyLocationManager::AddGeofence(
    /* [in] */ ILocationRequest* request,
    /* [in] */ IGeofence* fence,
    /* [in] */ IPendingIntent* intent)
{
    return LocationManager::AddGeofence(request, fence, intent);
}

ECode CPrivacyLocationManager::RemoveProximityAlert(
    /* [in] */ IPendingIntent* intent)
{
    return LocationManager::RemoveProximityAlert(intent);
}

ECode CPrivacyLocationManager::RemoveGeofence(
    /* [in] */ IGeofence* fence,
    /* [in] */ IPendingIntent* intent)
{
    return LocationManager::RemoveGeofence(fence, intent);
}

ECode CPrivacyLocationManager::RemoveAllGeofences(
    /* [in] */ IPendingIntent* intent)
{
    return LocationManager::RemoveAllGeofences(intent);
}

ECode CPrivacyLocationManager::IsProviderEnabled(
    /* [in] */ const String& provider,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (provider.IsNull()) {
        return LocationManager::IsProviderEnabled(provider, result);
    }

    String packageName;
    AutoPtr<IPrivacySettings> pSet;
    Boolean output = FALSE;
    mContext->GetPackageName(&packageName);
    mPrivacySetMan->GetSettings(packageName, Binder::GetCallingUid(), (IPrivacySettings**)&pSet);

    if (pSet != NULL) {
        if (provider.Equals(ILocationManager::GPS_PROVIDER)) {
            switch (GetLocationSetting(0, pSet)) {
                case IPrivacySettings::REAL:
                    LocationManager::IsProviderEnabled(provider, &output);
                    break;
                case IPrivacySettings::EMPTY:
                    break;
                case IPrivacySettings::CUSTOM:
                case IPrivacySettings::RANDOM:
                    output = TRUE;
                    break;
            }
        } else if (provider.Equals(ILocationManager::NETWORK_PROVIDER)) {
            switch (GetLocationSetting(1, pSet)) {
                case IPrivacySettings::REAL:
                    LocationManager::IsProviderEnabled(provider, &output);
                    break;
                case IPrivacySettings::EMPTY:
                    break;
                case IPrivacySettings::CUSTOM:
                case IPrivacySettings::RANDOM:
                    output = TRUE;
                    break;
            }
        } else if (provider.Equals(ILocationManager::PASSIVE_PROVIDER)) { // could get location from any of above
            if (GetLocationSetting(0, pSet) == IPrivacySettings::REAL ||
                GetLocationSetting(1, pSet) == IPrivacySettings::REAL) {
                LocationManager::IsProviderEnabled(provider, &output);
            } else {
                output = FALSE;
            }
        }
    } else { // if querying unknown provider
        LocationManager::IsProviderEnabled(provider, &output);
    }

    *result = output;
    return NOERROR;
}

ECode CPrivacyLocationManager::GetLastLocation(
    /* [out] */ ILocation** location)
{
    VALIDATE_NOT_NULL(location);
    return LocationManager::GetLastLocation(location);
}

ECode CPrivacyLocationManager::GetLastKnownLocation(
    /* [in] */ const String& provider,
    /* [out] */ ILocation** location)
{
    VALIDATE_NOT_NULL(location)

    if (provider.IsNull()) {
        return LocationManager::GetLastKnownLocation(provider, location);
    }

    String packageName;
    mContext->GetPackageName(&packageName);
    Int32 uid = Binder::GetCallingUid();
    AutoPtr<IPrivacySettings> pSet;
    mPrivacySetMan->GetSettings(packageName, uid, (IPrivacySettings**)&pSet);
    AutoPtr<ILocation> output;

    if (pSet != NULL) {
        String latStr;
        String lngStr;

        if (provider.Equals(ILocationManager::GPS_PROVIDER)) {
            switch (GetLocationSetting(0, pSet)) {
                case IPrivacySettings::REAL:
                    LocationManager::GetLastKnownLocation(provider, (ILocation**)&output);
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_GPS, GetLocationNotificatonString(output), pSet.Get());
                    break;

                case IPrivacySettings::EMPTY:
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_LOCATION_GPS, String(NULL), pSet.Get());
                    break;

                case IPrivacySettings::CUSTOM:
                    CLocation::New(provider, (ILocation**)&output);
                    pSet->GetLocationGpsLat(&latStr);
                    pSet->GetLocationGpsLon(&lngStr);
                    output->SetLatitude(StringUtils::ParseDouble(latStr));
                    output->SetLongitude(StringUtils::ParseDouble(lngStr));

                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::CUSTOM, IPrivacySettings::DATA_LOCATION_GPS, GetLocationNotificatonString(output), pSet.Get());
                    break;

                case IPrivacySettings::RANDOM:
                    CLocation::New(provider, (ILocation**)&output);
                    pSet->GetLocationGpsLat(&latStr);
                    pSet->GetLocationGpsLon(&lngStr);
                    output->SetLatitude(StringUtils::ParseDouble(latStr));
                    output->SetLongitude(StringUtils::ParseDouble(lngStr));

                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::RANDOM, IPrivacySettings::DATA_LOCATION_GPS, GetLocationNotificatonString(output), pSet.Get());
                    break;
            }
        } else if (provider.Equals(ILocationManager::NETWORK_PROVIDER)) {
            switch (GetLocationSetting(1, pSet)) {
                case IPrivacySettings::REAL:
                    LocationManager::GetLastKnownLocation(provider, (ILocation**)&output);
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_NETWORK, GetLocationNotificatonString(output), pSet.Get());
                    break;

                case IPrivacySettings::EMPTY:
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_LOCATION_NETWORK, String(NULL), pSet.Get());
                    break;

                case IPrivacySettings::CUSTOM:
                    CLocation::New(provider, (ILocation**)&output);
                    pSet->GetLocationNetworkLat(&latStr);
                    pSet->GetLocationNetworkLon(&lngStr);
                    output->SetLatitude(StringUtils::ParseDouble(latStr));
                    output->SetLongitude(StringUtils::ParseDouble(lngStr));

                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::CUSTOM, IPrivacySettings::DATA_LOCATION_NETWORK, GetLocationNotificatonString(output), pSet.Get());
                    break;
                case IPrivacySettings::RANDOM:
                    CLocation::New(provider, (ILocation**)&output);
                    pSet->GetLocationNetworkLat(&latStr);
                    pSet->GetLocationNetworkLon(&lngStr);
                    output->SetLatitude(StringUtils::ParseDouble(latStr));
                    output->SetLongitude(StringUtils::ParseDouble(lngStr));

                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::RANDOM, IPrivacySettings::DATA_LOCATION_NETWORK, GetLocationNotificatonString(output), pSet.Get());
                    break;
            }
        } else if (provider.Equals(ILocationManager::PASSIVE_PROVIDER) &&
                GetLocationSetting(0, pSet) == IPrivacySettings::REAL &&
                GetLocationSetting(1, pSet) == IPrivacySettings::REAL) {
            // only output real location if both gps and network are allowed
            LocationManager::GetLastKnownLocation(provider, (ILocation**)&output);
            mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_GPS, GetLocationNotificatonString(output), pSet.Get());
        }
    } else {
        LocationManager::GetLastKnownLocation(provider, (ILocation**)&output);

        if (provider.Equals(ILocationManager::NETWORK_PROVIDER)) {
            mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_NETWORK, GetLocationNotificatonString(output), pSet.Get());
        } else { // including GPS and passive providers
            mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_GPS, GetLocationNotificatonString(output), pSet.Get());
        }
    }

    *location = output;
    REFCOUNT_ADD(*location)
    return NOERROR;
}

ECode CPrivacyLocationManager::AddTestProvider(
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
    return LocationManager::AddTestProvider(name, requiresNetwork, requiresSatellite, requiresCell, hasMonetaryCost, supportsAltitude, supportsSpeed, supportsBearing, powerRequirement, accuracy);
}

ECode CPrivacyLocationManager::RemoveTestProvider(
    /* [in] */ const String& provider)
{
    return LocationManager::RemoveTestProvider(provider);
}

ECode CPrivacyLocationManager::SetTestProviderLocation(
    /* [in] */ const String& provider,
    /* [in] */ ILocation* loc)
{
    return LocationManager::SetTestProviderLocation(provider, loc);
}

ECode CPrivacyLocationManager::ClearTestProviderLocation(
    /* [in] */ const String& provider)
{
    return LocationManager::ClearTestProviderLocation(provider);
}

ECode CPrivacyLocationManager::SetTestProviderEnabled(
    /* [in] */ const String& provider,
    /* [in] */ Boolean enabled)
{
    return LocationManager::SetTestProviderEnabled(provider, enabled);
}

ECode CPrivacyLocationManager::ClearTestProviderEnabled(
    /* [in] */ const String& provider)
{
    return LocationManager::ClearTestProviderEnabled(provider);
}

ECode CPrivacyLocationManager::SetTestProviderStatus(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ Elastos::Droid::Os::IBundle* extras,
    /* [in] */ Int64 updateTime)
{
    return LocationManager::SetTestProviderStatus(provider, status, extras, updateTime);
}

ECode CPrivacyLocationManager::ClearTestProviderStatus(
    /* [in] */ const String& provider)
{
    return LocationManager::ClearTestProviderStatus(provider);
}

ECode CPrivacyLocationManager::AddGpsStatusListener(
    /* [in] */ IGpsStatusListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return LocationManager::AddGpsStatusListener(listener, result);
}

ECode CPrivacyLocationManager::RemoveGpsStatusListener(
    /* [in] */ IGpsStatusListener* listener)
{
    return LocationManager::RemoveGpsStatusListener(listener);
}

ECode CPrivacyLocationManager::AddNmeaListener(
    /* [in] */ IGpsStatusNmeaListener* listener,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    // only blocks if access is not allowed
    // custom and random values not implemented due to Decimal Degrees->NMEA conversion complexity
    String packageName;
    mContext->GetPackageName(&packageName);

    Int32 uid = Binder::GetCallingUid();

    AutoPtr<IPrivacySettings> pSet;
    mPrivacySetMan->GetSettings(packageName, uid, (IPrivacySettings**)&pSet);

    Byte gpsSetting = 0;
    pSet->GetLocationGpsSetting(&gpsSetting);

    if (pSet != NULL && gpsSetting != IPrivacySettings::REAL) {
        mPrivacySetMan->Notification(packageName, uid, (Byte)IPrivacySettings::EMPTY, IPrivacySettings::DATA_LOCATION_GPS, String(NULL), pSet);
        *result = FALSE;
        return NOERROR;
    } else {
        mPrivacySetMan->Notification(packageName, uid, (Byte)IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_GPS, String(NULL), pSet);
    }

//    Log.d(TAG, "addNmeaListener - " + context.getPackageName() + " (" + Binder.getCallingUid() + ") output: [real value]");
    return LocationManager::AddNmeaListener(listener, result);
}

ECode CPrivacyLocationManager::RemoveNmeaListener(
    /* [in] */ IGpsStatusNmeaListener* listener)
{
    return LocationManager::RemoveNmeaListener(listener);
}

ECode CPrivacyLocationManager::GetGpsStatus(
    /* [in] */ IGpsStatus* status,
    /* [out] */ IGpsStatus** outStatus)
{
    VALIDATE_NOT_NULL(outStatus)
    return LocationManager::GetGpsStatus(status, outStatus);
}

ECode CPrivacyLocationManager::SendExtraCommand(
    /* [in] */ const String& provider,
    /* [in] */ const String& command,
    /* [in] */ Elastos::Droid::Os::IBundle* extras,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return LocationManager::SendExtraCommand(provider, command, extras, result);
}

ECode CPrivacyLocationManager::SendNiResponse(
    /* [in] */ Int32 notifId,
    /* [in] */ Int32 userResponse,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return LocationManager::SendNiResponse(notifId, userResponse, result);
}

ECode CPrivacyLocationManager::RequestLocationUpdates(
    /* [in] */ const String& provider,
    /* [in] */ ILocationListener* listener,
    /* [in] */ IPendingIntent* intent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    /*synchronized(lock) */
    { // custom listener should only return a value after this method has returned
        AutoLock lock(mLock);

        String packageName;
        mContext->GetPackageName(&packageName);

        Int32 uid = Binder::GetCallingUid();
        AutoPtr<IPrivacySettings> pSet;
        mPrivacySetMan->GetSettings(packageName, uid, (IPrivacySettings**)&pSet);

        Boolean output = FALSE;

        if (pSet != NULL) {
            String lat;
            String lng;

            if (provider.Equals(ILocationManager::GPS_PROVIDER)) {
                switch (GetLocationSetting(0, pSet)) {
                    case IPrivacySettings::REAL:
                        mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_GPS, String(NULL), pSet.Get());
                        break;

                    case IPrivacySettings::EMPTY:
                        if (intent != NULL) {
                            intent->Cancel();
                        }
                        output = TRUE;
                        mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_LOCATION_GPS, String(NULL), pSet.Get());
                        break;

                    case IPrivacySettings::CUSTOM:
                    {
//                        try {
                            pSet->GetLocationGpsLat(&lat);
                            pSet->GetLocationGpsLon(&lng);

                            PrivacyLocationUpdater* updateThread = new PrivacyLocationUpdater(
                                provider, listener, intent,
                                StringUtils::ParseDouble(lat),
                                StringUtils::ParseDouble(lng));
                            updateThread->Start();
                            delete updateThread;

                            output = TRUE;
//                        } catch (NumberFormatException e) {
//                            Logger::E(TAG, "requestLocationUpdates: invalid coordinates");
//                            output = TRUE;
//                        }
                        mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::CUSTOM, IPrivacySettings::DATA_LOCATION_GPS,
                                String("Lat: ") + lat + " Lon: " + lng, pSet.Get());
                    }
                        break;

                    case IPrivacySettings::RANDOM:
                    {
//                        try {
                            pSet->GetLocationGpsLat(&lat);
                            pSet->GetLocationGpsLon(&lng);

                            PrivacyLocationUpdater* updateThread2 = new PrivacyLocationUpdater(
                                provider, listener, intent,
                                StringUtils::ParseDouble(lat),
                                StringUtils::ParseDouble(lng));
                            updateThread2->Start();
                            delete updateThread2;
                            output = TRUE;
//                        } catch (NumberFormatException e) {
//                            Logger::E(TAG, "requestLocationUpdates: invalid coordinates");
//                            output = true;
//                        }
                        mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::RANDOM, IPrivacySettings::DATA_LOCATION_GPS,
                                String("Lat: ") + lat + " Lon: " + lng, pSet.Get());
                    }
                        break;
                }
            } else if (provider.Equals(ILocationManager::NETWORK_PROVIDER)) {
                switch (GetLocationSetting(1, pSet)) {
                    case IPrivacySettings::REAL:
                        mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_NETWORK, String(NULL), pSet.Get());
                        break;
                    case IPrivacySettings::EMPTY:
                        if (intent != NULL) {
                            intent->Cancel();
                        }
                        output = true;
                        mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_LOCATION_NETWORK, String(NULL), pSet.Get());
                        break;
                    case IPrivacySettings::CUSTOM:
                    {
//                        try {
                            pSet->GetLocationNetworkLat(&lat);
                            pSet->GetLocationNetworkLon(&lng);

                            PrivacyLocationUpdater* updateThread = new PrivacyLocationUpdater(
                                provider, listener, intent,
                                StringUtils::ParseDouble(lat),
                                StringUtils::ParseDouble(lng));
                            updateThread->Start();
                            delete updateThread;
                            output = TRUE;
//                        } catch (NumberFormatException e) {
//                            Log.e(TAG, "requestLocationUpdates: invalid coordinates");
//                            output = true;
//                        }
                        mPrivacySetMan->Notification(
                            packageName, uid,
                            IPrivacySettings::CUSTOM,
                            IPrivacySettings::DATA_LOCATION_NETWORK,
                            String("Lat: ") + lat + " Lon: " + lng, pSet.Get());
                    }
                        break;
                    case IPrivacySettings::RANDOM:
                    {
//                        try {
                        pSet->GetLocationNetworkLat(&lat);
                        pSet->GetLocationNetworkLon(&lng);
                        PrivacyLocationUpdater* updateThread2 = new PrivacyLocationUpdater(
                            provider, listener, intent,
                            StringUtils::ParseDouble(lat),
                            StringUtils::ParseDouble(lng));
                        updateThread2->Start();
                        delete updateThread2;
                        output = TRUE;
//                        } catch (NumberFormatException e) {
//                            Log.e(TAG, "requestLocationUpdates: invalid coordinates");
//                            output = true;
//                        }
                        mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::RANDOM, IPrivacySettings::DATA_LOCATION_NETWORK,
                                String("Lat: ") + lat + " Lon: " + lng, pSet.Get());
                    }
                        break;
                }
            } else if (provider.Equals(ILocationManager::PASSIVE_PROVIDER)) { // could get location from any of above
                if (GetLocationSetting(0, pSet) == IPrivacySettings::REAL &&
                    GetLocationSetting(1, pSet) == IPrivacySettings::REAL) {
                    output = FALSE;
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_GPS, String(NULL), pSet.Get());
                } else {
                    output = TRUE;
                    mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::EMPTY, IPrivacySettings::DATA_LOCATION_GPS, String(NULL), pSet.Get());
                }
            }
        } else {
            if (provider.Equals(ILocationManager::NETWORK_PROVIDER)) {
                mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_NETWORK, String(NULL), pSet.Get());
            } else { // including GPS and passive providers
                mPrivacySetMan->Notification(packageName, uid, IPrivacySettings::REAL, IPrivacySettings::DATA_LOCATION_GPS, String(NULL), pSet.Get());
            }
        }

        *result = output;
    }

    return NOERROR;
}

ECode CPrivacyLocationManager::RequestLocationUpdates(
    /* [in] */ ICriteria* criteria,
    /* [in] */ ILocationListener* listener,
    /* [in] */ IPendingIntent* intent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (criteria == NULL) {
        *result = FALSE;
        return NOERROR;
    } else {
        Int32 accuracy;
        Int32 bearingAccuracy;
        Int32 horizontalAccuracy;
        Int32 verticalAccuracy;
        Int32 speedAccuracy;

        criteria->GetAccuracy(&accuracy);
        criteria->GetBearingAccuracy(&bearingAccuracy);
        criteria->GetHorizontalAccuracy(&horizontalAccuracy);
        criteria->GetVerticalAccuracy(&verticalAccuracy);
        criteria->GetSpeedAccuracy(&speedAccuracy);

        if (accuracy == ICriteria::Criteria_ACCURACY_FINE ||
            bearingAccuracy == ICriteria::Criteria_ACCURACY_HIGH ||
            horizontalAccuracy == ICriteria::Criteria_ACCURACY_HIGH ||
            verticalAccuracy == ICriteria::Criteria_ACCURACY_HIGH ||
            speedAccuracy == ICriteria::Criteria_ACCURACY_HIGH) {
            return RequestLocationUpdates(ILocationManager::GPS_PROVIDER, listener, intent, result);
        } else { // treat all others as network providers
            return RequestLocationUpdates(ILocationManager::NETWORK_PROVIDER, listener, intent, result);
        }
    }

    return NOERROR;
}

ECode CPrivacyLocationManager::RequestLocationUpdates(
    /* [in] */ const String& provider,
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
    return NOERROR;
}

ECode CPrivacyLocationManager::RequestLocationUpdates(
    /* [in] */ Int64 minTime,
    /* [in] */ Float minDistance,
    /* [in] */ ICriteria* criteria,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
    return NOERROR;
}

ECode CPrivacyLocationManager::RequestSingleUpdate(
    /* [in] */ ICriteria* criteria,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
    return NOERROR;
}

ECode CPrivacyLocationManager::RequestLocationUpdates(
    /* [in] */ ILocationRequest* request,
    /* [in] */ ILocationListener* listener,
    /* [in] */ ILooper* looper)
{
    return NOERROR;
}

String CPrivacyLocationManager::GetLocationNotificatonString(
    /* [in] */ ILocation* output)
{
    String notification(NULL);
    if(output != NULL){
        Double lat;
        Double lng;
        output->GetLatitude(&lat);
        output->GetLongitude(&lng);
        StringBuilder sb;
        sb += "Lat: ";
        sb += lat;
        sb += " Lon: ";
        sb += lng;
        notification = sb.ToString();
    }

    return notification;
}

/**
 * @param type: 0:GPS, 1:network
 */
Int16 CPrivacyLocationManager::GetLocationSetting(
    /* [in] */ Int32 type,
    /* [in] */ IPrivacySettings* setting)
{
    Byte netSetting = 0;

    if (type == 0) { // GPS
        setting->GetLocationGpsSetting(&netSetting);
    }
    else if (type == 1) {
        setting->GetLocationNetworkSetting(&netSetting);
    }

    return (Int16)netSetting;
}

CPrivacyLocationManager::PrivacyLocationUpdater::PrivacyLocationUpdater(
    /* [in] */ const String& provider,
    /* [in] */ ILocationListener* listener,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ Double latitude,
    /* [in] */ Double longitude)
    : mProvider(provider)
    , mListener(listener)
    , mIntent(intent)
    , mLatitude(latitude)
    , mLongitude(longitude)
{
    Thread::constructor(String("PrivacyLocationManager"));
}

ECode CPrivacyLocationManager::PrivacyLocationUpdater::Run()
{
    if (!mProvider.IsNull()) {
        AutoPtr<ILocation> location;
        CLocation::New(mProvider, (ILocation**)&location);

        location->SetLatitude(mLatitude);
        location->SetLongitude(mLongitude);

        for (Int32 i = 0; i < CUSTOM_LOCATION_UPDATE_COUNT; i++) {
            if (mListener != NULL) {
                mListener->OnLocationChanged(location);
            } else if (mIntent != NULL) {
                // no custom or random location implemented due to complexity
                mIntent->Cancel();
            }

//            try {
                Thread::Sleep((int)(Elastos::Core::Math::Random() * 1000));
//            } catch (InterruptedException e) {
//                e.printStackTrace();
//            }
        }
    }
    return NOERROR;
}

} // namespace Surrogate
} // namespace Privacy
} // namespace Droid
} // namespace Elastos

