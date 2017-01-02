//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_PRIVACY_SURROGATE_CPRIVACYLOCATIONMANAGER_H__
#define __ELASTOS_DROID_PRIVACY_SURROGATE_CPRIVACYLOCATIONMANAGER_H__

#include "_Elastos_Droid_Privacy_Surrogate_CPrivacyLocationManager.h"
#include "elastos/droid/location/LocationManager.h"

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Location::ICriteria;
using Elastos::Droid::Location::IGeofence;
using Elastos::Droid::Location::IGpsStatus;
using Elastos::Droid::Location::IGpsStatusListener;
using Elastos::Droid::Location::IGpsStatusNmeaListener;
using Elastos::Droid::Location::IILocationManager;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationListener;
using Elastos::Droid::Location::ILocationProvider;
using Elastos::Droid::Location::ILocationRequest;
using Elastos::Droid::Location::LocationManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ILooper;

namespace Elastos {
namespace Droid {
namespace Privacy {
namespace Surrogate {

CarClass(CPrivacyLocationManager), public LocationManager
{
    friend class PrivacyLocationUpdater;

private:
    class PrivacyLocationUpdater
        : public ThreadBase
    {
    public:
        PrivacyLocationUpdater(
            /* [in] */ const String& provider,
            /* [in] */ ILocationListener* listener,
            /* [in] */ IPendingIntent* intent,
            /* [in] */ Double latitude,
            /* [in] */ Double longitude);

    public:
        virtual CARAPI Run();

    private:
        String mProvider;
        AutoPtr<ILocationListener> mListener;
        AutoPtr<IPendingIntent> mIntent;
        Double mLatitude;
        Double mLongitude;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IILocationManager* service,
        /* [in] */ IContext* context);

    CARAPI GetAllProviders(
        /* [out] */ IObjectContainer** allProvoider);

    CARAPI GetProviders(
        /* [in] */ Boolean enabledOnly,
        /* [out] */ IObjectContainer** providers);

    CARAPI GetProvider(
        /* [in] */ const String& name,
        /* [out] */ ILocationProvider** provider);

    CARAPI GetProviders(
        /* [in] */ ICriteria* criteria,
        /* [in] */ Boolean enabledOnly,
        /* [out] */ IObjectContainer** providers);

    CARAPI GetBestProvider(
        /* [in] */ ICriteria* criteria,
        /* [in] */ Boolean enabledOnly,
        /* [out] */ String* provider);

    CARAPI RequestLocationUpdates(
        /* [in] */ const String& provider,
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ ILocationListener* listener);

    CARAPI RequestLocationUpdates(
        /* [in] */ const String& provider,
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ IPendingIntent* intent);

    CARAPI RequestLocationUpdates(
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ ICriteria* criteria,
        /* [in] */ IPendingIntent* intent);

    CARAPI RequestSingleUpdate(
        /* [in] */ const String& provider,
        /* [in] */ ILocationListener* listener,
        /* [in] */ ILooper* looper);

    CARAPI RequestSingleUpdate(
        /* [in] */ ICriteria* criteria,
        /* [in] */ ILocationListener* listener,
        /* [in] */ ILooper* looper);

    CARAPI RequestLocationUpdates(
        /* [in] */ const String& provider,
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ ILocationListener* listener,
        /* [in] */ ILooper* looper);

    CARAPI RequestLocationUpdates(
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ ICriteria* criteria,
        /* [in] */ ILocationListener* listener,
        /* [in] */ ILooper* looper);

    CARAPI RequestLocationUpdates(
        /* [in] */ const String& provider,
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ ILocationListener* listener,
        /* [in] */ ILooper* looper);

    CARAPI RequestLocationUpdates(
        /* [in] */ Int64 minTime,
        /* [in] */ Float minDistance,
        /* [in] */ ICriteria* criteria,
        /* [in] */ ILocationListener* listener,
        /* [in] */ ILooper* looper);

    CARAPI RequestSingleUpdate(
        /* [in] */ ICriteria* criteria,
        /* [in] */ ILocationListener* listener,
        /* [in] */ ILooper* looper);


    CARAPI RequestSingleUpdate(
        /* [in] */ const String& provider,
        /* [in] */ IPendingIntent* intent);

    CARAPI RequestSingleUpdate(
        /* [in] */ ICriteria* criteria,
        /* [in] */ IPendingIntent* intent);

    CARAPI RequestLocationUpdates(
        /* [in] */ ILocationRequest* request,
        /* [in] */ ILocationListener* listener,
        /* [in] */ ILooper* looper);


    CARAPI RequestLocationUpdates(
        /* [in] */ ILocationRequest* request,
        /* [in] */ IPendingIntent* intent);

    CARAPI RemoveUpdates(
        /* [in] */ ILocationListener* listener);

    CARAPI RemoveUpdates(
        /* [in] */ IPendingIntent* intent);

    CARAPI AddProximityAlert(
        /* [in] */ Double latitude,
        /* [in] */ Double longitude,
        /* [in] */ Float radius,
        /* [in] */ Int64 expiration,
        /* [in] */ IPendingIntent* intent);

    CARAPI AddGeofence(
        /* [in] */ ILocationRequest* request,
        /* [in] */ IGeofence* fence,
        /* [in] */ IPendingIntent* intent);

    CARAPI RemoveProximityAlert(
        /* [in] */ IPendingIntent* intent);

    CARAPI RemoveGeofence(
        /* [in] */ IGeofence* fence,
        /* [in] */ IPendingIntent* intent);

    CARAPI RemoveAllGeofences(
        /* [in] */ IPendingIntent* intent);

    CARAPI IsProviderEnabled(
        /* [in] */ const String& provider,
        /* [out] */ Boolean* flag);

    CARAPI GetLastLocation(
        /* [out] */ ILocation** location);

    CARAPI GetLastKnownLocation(
        /* [in] */ const String& provider,
        /* [out] */ ILocation** location);

    CARAPI AddTestProvider(
        /* [in] */ const String& name,
        /* [in] */ Boolean requiresNetwork,
        /* [in] */ Boolean requiresSatellite,
        /* [in] */ Boolean requiresCell,
        /* [in] */ Boolean hasMonetaryCost,
        /* [in] */ Boolean supportsAltitude,
        /* [in] */ Boolean supportsSpeed,
        /* [in] */ Boolean supportsBearing,
        /* [in] */ Int32 powerRequirement,
        /* [in] */ Int32 accuracy);

    CARAPI RemoveTestProvider(
        /* [in] */ const String& provider);

    CARAPI SetTestProviderLocation(
        /* [in] */ const String& provider,
        /* [in] */ ILocation* loc);

    CARAPI ClearTestProviderLocation(
        /* [in] */ const String& provider);

    CARAPI SetTestProviderEnabled(
        /* [in] */ const String& provider,
        /* [in] */ Boolean enabled);

    CARAPI ClearTestProviderEnabled(
        /* [in] */ const String& provider);

    CARAPI SetTestProviderStatus(
        /* [in] */ const String& provider,
        /* [in] */ Int32 status,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 updateTime);

    CARAPI ClearTestProviderStatus(
        /* [in] */ const String& provider);

    CARAPI AddGpsStatusListener(
        /* [in] */ IGpsStatusListener* listener,
        /* [out] */ Boolean* result);

    CARAPI RemoveGpsStatusListener(
        /* [in] */ IGpsStatusListener* listener);

    CARAPI AddNmeaListener(
        /* [in] */ IGpsStatusNmeaListener* listener,
        /* [out] */ Boolean* result);

    CARAPI RemoveNmeaListener(
        /* [in] */ IGpsStatusNmeaListener* listener);

    CARAPI GetGpsStatus(
        /* [in] */ IGpsStatus* status,
        /* [out] */ IGpsStatus** outStatus);

    CARAPI SendExtraCommand(
        /* [in] */ const String& provider,
        /* [in] */ const String& command,
        /* [in] */ IBundle* extras,
        /* [out] */ Boolean* result);

    CARAPI SendNiResponse(
        /* [in] */ Int32 notifId,
        /* [in] */ Int32 userResponse,
        /* [out] */ Boolean* result);

private:
    CARAPI_(String) GetLocationNotificatonString(
        /* [in] */ ILocation* output);

    CARAPI_(Int16) GetLocationSetting(
        /* [in] */ Int32 type,
        /* [in] */ IPrivacySettings* setting);

    CARAPI RequestLocationUpdates(
        /* [in] */ const String& provider,
        /* [in] */ ILocationListener* listener,
        /* [in] */ IPendingIntent* intent,
        /* [out] */ Boolean* result);

    CARAPI RequestLocationUpdates(
        /* [in] */ ICriteria* criteria,
        /* [in] */ ILocationListener* listener,
        /* [in] */ IPendingIntent* intent,
        /* [out] */ Boolean* result);

private:
    const static String TAG;
    const static Int32 CUSTOM_LOCATION_UPDATE_COUNT;

private:
    AutoPtr<IContext> mContext;
    AutoPtr<IPrivacySettingsManager> mPrivacySetMan;
    Object mLock;

    //TODO
    // List< AutoPtr<IThread> > mThreads;
};

} // namespace Surrogate
} // namespace Privacy
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PRIVACY_SURROGATE_CPRIVACYLOCATIONMANAGER_H__
