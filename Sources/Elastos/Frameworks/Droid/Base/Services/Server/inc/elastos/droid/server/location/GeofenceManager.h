
#ifndef __ELASTOS_DROID_SERVER_LOCATION_GEOFENCEMANAGER_H__
#define __ELASTOS_DROID_SERVER_LOCATION_GEOFENCEMANAGER_H__

#include "location/LocationBlacklist.h"
#include "location/GeofenceState.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentOnFinished;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Location::IGeofence;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationListener;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::ILocationRequest;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::IO::IPrintWriter;
using Elastos::Droid::Location::EIID_ILocationListener;
using Elastos::Droid::App::EIID_IPendingIntentOnFinished;
using Elastos::Droid::App::EIID_IPendingIntentOnFinished;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

class GeofenceManager
    : public ElRefBase
    , public ILocationListener
    , public IPendingIntentOnFinished
{
public:
    class GeofenceHandler : public HandlerBase
    {
    public:
        GeofenceHandler(
            /* [in] */ GeofenceManager* host)
            : mHost(host)
        {}

        HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        GeofenceManager* mHost;
    };

public:

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    GeofenceManager(
        /* [in] */ IContext* context,
        /* [in] */ LocationBlacklist* blacklist);

    CARAPI_(void) AddFence(
        /* [in] */ ILocationRequest* request,
        /* [in] */ IGeofence* geofence,
        /* [in] */ IPendingIntent* intent,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

    CARAPI_(void) RemoveFence(
        /* [in] */ IGeofence* fence,
        /* [in] */ IPendingIntent* intent);

    CARAPI_(void) RemoveFence(
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

    CARAPI_(void) Dump(
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

    CARAPI_(void) SendIntent(
        /* [in] */ IPendingIntent* pendingIntent,
        /* [in] */ IIntent* intent);

private:
    static const String TAG;// = "GeofenceManager";
    static const Boolean D;// = ILocationManagerService::D;

    static const Int32 MSG_UPDATE_FENCES;// = 1;

    /**
     * Assume a maximum land speed, as a heuristic to throttle location updates.
     * (Air travel should result in an airplane mode toggle which will
     * force a new location update anyway).
     */
    static const Int32 MAX_SPEED_M_S;// = 100;  // 360 km/hr (high speed train)

    /**
     * Maximum age after which a location is no longer considered fresh enough to use.
     */
    static const Int64 MAX_AGE_NANOS;// = 5 * 60 * 1000000000L; // five minutes

    /**
     * Most frequent update interval allowed.
     */
    static const Int64 MIN_INTERVAL_MS;// = 1 * 60 * 1000; // one minute

    /**
     * Least frequent update interval allowed.
     */
    static const Int64 MAX_INTERVAL_MS;// = 2 * 60 * 60 * 1000; // two hours

    AutoPtr<IContext> mContext;
    AutoPtr<ILocationManager> mLocationManager;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<GeofenceHandler> mHandler;
    AutoPtr<LocationBlacklist> mBlacklist;

    Object mLock;

    // access to members below is synchronized on mLock
    /**
     * A list containing all registered geofences.
     */
    List<AutoPtr<GeofenceState> > mFences;;

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
