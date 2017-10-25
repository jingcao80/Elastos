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

#ifndef __ELASTOS_DROID_SERVER_LOCATION_GEOFENCEMANAGER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GEOFENCEMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/server/location/LocationBlacklist.h"

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentOnFinished;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Location::IGeofence;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationListener;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::ILocationRequest;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

class GeofenceManager
    : public Object
    , public ILocationListener
    , public IPendingIntentOnFinished
{
public:
    class GeofenceHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("GeofenceManager::GeofenceHandler")

        GeofenceHandler(
            /* [in] */ GeofenceManager* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        GeofenceManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    GeofenceManager(
        /* [in] */ IContext* context,
        /* [in] */ LocationBlacklist* blacklist);

    CARAPI AddFence(
        /* [in] */ ILocationRequest* request,
        /* [in] */ IGeofence* geofence,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 allowedResolutionLevel,
        /* [in] */ const String& packageName);

    CARAPI RemoveFence(
        /* [in] */ IGeofence* fence,
        /* [in] */ IPendingIntent* intent);

    CARAPI RemoveFence(
        /* [in] */ const String& packageName);

    // Runs on the handler (which was passed into LocationManager.requestLocationUpdates())
    //@Override
    CARAPI OnLocationChanged(
        /* [in] */ ILocation* location);

    //@Override
    CARAPI OnStatusChanged(
        /* [in] */ const String& provider,
        /* [in] */ Int32 status,
        /* [in] */ IBundle* extras);

    //@Override
    CARAPI OnProviderEnabled(
        /* [in] */ const String& provider);

    //@Override
    CARAPI OnProviderDisabled(
        /* [in] */ const String& provider);

    //@Override
    CARAPI OnSendFinished(
        /* [in] */ IPendingIntent* pendingIntent,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 resultCode,
        /* [in] */ const String& resultData,
        /* [in] */ IBundle* resultExtras);

    CARAPI Dump(
        /* [in] */ IPrintWriter* pw);

private:
    CARAPI_(void) RemoveExpiredFencesLocked();
    CARAPI_(void) ScheduleUpdateFencesLocked();

    /**
     * Returns the location received most recently from {@link #onLocationChanged(Location)},
     * or consult {@link LocationManager#getLastLocation()} if none has arrived. Does not return
     * either if the location would be too stale to be useful.
     *
     * @return a fresh, valid Location, or null if none is available
     */
    CARAPI_(AutoPtr<ILocation>) GetFreshLocationLocked();

    /**
     * The geofence update loop. This function removes expired fences, then tests the most
     * recently-received {@link Location} against each registered {@link GeofenceState}, sending
     * {@link Intent}s for geofences that have been tripped. It also adjusts the active location
     * update request with {@link LocationManager} as appropriate for any active geofences.
     */
    // Runs on the handler.
    CARAPI UpdateFences();

    CARAPI_(void) SendIntentEnter(
        /* [in] */ IPendingIntent* pendingIntent);

    CARAPI_(void) SendIntentExit(
        /* [in] */ IPendingIntent* pendingIntent);

    CARAPI SendIntent(
        /* [in] */ IPendingIntent* pendingIntent,
        /* [in] */ IIntent* intent);

private:
    static const String TAG;
    static const Boolean D;

    static const Int32 MSG_UPDATE_FENCES;

    /**
     * Assume a maximum land speed, as a heuristic to throttle location updates.
     * (Air travel should result in an airplane mode toggle which will
     * force a new location update anyway).
     */
    static const Int32 MAX_SPEED_M_S;  // 360 km/hr (high speed train)

    /**
     * Maximum age after which a location is no longer considered fresh enough to use.
     */
    static const Int64 MAX_AGE_NANOS; // five minutes

    /**
     * Most frequent update interval allowed.
     */
    static const Int64 MIN_INTERVAL_MS; // one minute

    /**
     * Least frequent update interval allowed.
     */
    static const Int64 MAX_INTERVAL_MS; // two hours

    AutoPtr<IContext> mContext;
    AutoPtr<ILocationManager> mLocationManager;
    AutoPtr<IAppOpsManager> mAppOps;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<GeofenceHandler> mHandler;
    AutoPtr<LocationBlacklist> mBlacklist;

    // access to members below is synchronized on mLock
    /**
     * A list containing all registered geofences.
     */
     AutoPtr<IList> mFences;

    /**
     * This is set true when we have an active request for {@link Location} updates via
     * {@link LocationManager#requestLocationUpdates(LocationRequest, LocationListener,
     * android.os.Looper).
     */
    Boolean mReceivingLocationUpdates;

    /**
     * The update interval component of the current active {@link Location} update request.
     */
    Int64 mLocationUpdateInterval;

    /**
     * The {@link Location} most recently received via {@link #onLocationChanged(Location)}.
     */
    AutoPtr<ILocation> mLastLocationUpdate;

    /**
     * This is set true when a {@link Location} is received via
     * {@link #onLocationChanged(Location)} or {@link #scheduleUpdateFencesLocked()}, and cleared
     * when that Location has been processed via {@link #updateFences()}
     */
    Boolean mPendingUpdate;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_GEOFENCEMANAGER_H__
