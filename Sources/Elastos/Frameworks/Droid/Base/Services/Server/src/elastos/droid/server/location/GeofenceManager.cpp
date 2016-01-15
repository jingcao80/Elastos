
#include "location/GeofenceManager.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Location::CLocationRequest;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::EIID_ILocationManager;
using Elastos::Droid::Location::CGeofence;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::EIID_IPowerManager;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandler;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String GeofenceManager::TAG("GeofenceManager");
const Boolean GeofenceManager::D = FALSE;
const Int32 GeofenceManager::MSG_UPDATE_FENCES = 1;
const Int32 GeofenceManager::MAX_SPEED_M_S = 100;  // 360 km/hr (high speed train)
const Int64 GeofenceManager::MAX_AGE_NANOS = 5L * 60L * 300000000000L; // five minutes
const Int64 GeofenceManager::MIN_INTERVAL_MS = 1 * 60 * 1000; // one minute
const Int64 GeofenceManager::MAX_INTERVAL_MS = 2 * 60 * 60 * 1000; // two hours

ECode GeofenceManager::GeofenceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case GeofenceManager::MSG_UPDATE_FENCES:
        mHost->UpdateFences();
        break;
    }

    return NOERROR;
}

PInterface GeofenceManager::Probe(
    /* [in] */ REIID riid)
{
    if ( riid == EIID_IInterface) {
        return (IInterface*)(ILocationListener *)this;
    } else if ( riid == EIID_ILocationListener ) {
        return (ILocationListener *)this;
    } else if ( riid == EIID_IPendingIntentOnFinished ) {
        return (IPendingIntentOnFinished *)this;
    }
    return NULL;
}
UInt32 GeofenceManager::AddRef()
{
    return ElRefBase::AddRef();
}
UInt32 GeofenceManager::Release()
{
    return ElRefBase::Release();
}
ECode GeofenceManager::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == (IInterface*)(ILocationListener *)this) {
        *iid = EIID_ILocationListener ;
    } else if (object == (IInterface*)(IPendingIntentOnFinished *)this) {
        *iid = EIID_IPendingIntentOnFinished ;
    } else {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

GeofenceManager::GeofenceManager(
    /* [in] */ IContext* context,
    /* [in] */ LocationBlacklist* blacklist)
{
    mContext = context;
    AutoPtr<IInterface> serviceTemp;
    mContext->GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&serviceTemp);
    mLocationManager = (ILocationManager*)(serviceTemp->Probe(EIID_ILocationManager));
    serviceTemp = NULL;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&serviceTemp);
    AutoPtr<IPowerManager> powerManager = (IPowerManager*)(serviceTemp->Probe(EIID_IPowerManager));
    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mWakeLock);
    mHandler = new GeofenceHandler(this);
    mBlacklist = blacklist;
}

void GeofenceManager::AddFence(
    /* [in] */ ILocationRequest* request,
    /* [in] */ IGeofence* geofence,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ Int32 uid,
    /* [in] */ const String& packageName)
{
//  if (D) {
//        Slog.d(TAG, "addFence: request=" + request + ", geofence=" + geofence
//                + ", intent=" + intent + ", uid=" + uid + ", packageName=" + packageName);
//    }

    assert(request != NULL);
    assert(geofence != NULL);
    assert(intent != NULL);

    Int64 expireAt;
    request->GetExpireAt(&expireAt);
    AutoPtr<GeofenceState> state = new GeofenceState(geofence,
            expireAt, packageName, intent);
    //synchronized(mLock)
    {
        AutoLock lock(mLock);
        // first make sure it doesn't already exist
        //for (Int32 i = mFences.size() - 1; i >= 0; i--) {
        List<AutoPtr<GeofenceState> >::ReverseIterator rit = mFences.RBegin();
        Boolean bFlag1, bFlag2;
        while (rit != mFences.REnd()) {
            AutoPtr<GeofenceState> w = *rit;
            geofence->Equals(w->mFence, &bFlag1);
            intent->Equals(w->mIntent, &bFlag2);
            if (bFlag1 && bFlag2) {
                // already exists, remove the old one
            } else {
                rit++;
            }

        }
        mFences.PushBack(state);
        ScheduleUpdateFencesLocked();
    }
}

void GeofenceManager::RemoveFence(
    /* [in] */ IGeofence* fence,
    /* [in] */ IPendingIntent* intent)
{/*
    if (D) {
        Slog.d(TAG, "removeFence: fence=" + fence + ", intent=" + intent);
    }
*/
    //synchronized(mLock)
    PFL_EX("DEBUG")
    {
        AutoLock lock(mLock);
        Boolean bFlag;
        List<AutoPtr<GeofenceState> >::Iterator iter = mFences.Begin();
        while (iter != mFences.End()) {
            AutoPtr<GeofenceState> state = *iter;
            state->mIntent->Equals(intent, &bFlag);
            if (bFlag) {

                if (fence == NULL) {
                    // always remove
                    iter = mFences.Erase(iter);
                    continue;
                } else {
                    // just remove matching fences
                    fence->Equals(state->mFence, &bFlag);
                    if (bFlag) {
                        iter = mFences.Erase(iter);
                        continue;
                    }
                }
            }

            ++iter;
        }
        ScheduleUpdateFencesLocked();
    }
}

void GeofenceManager::RemoveFence(
    /* [in] */ const String& packageName)
{/*
    if (D) {
        Slog.d(TAG, "removeFence: packageName=" + packageName);
    }
*/
    //synchronized(mLock)
    {
        AutoLock lock(mLock);

        List<AutoPtr<GeofenceState> >::Iterator iter = mFences.Begin();
        while (iter != mFences.End()) {
            AutoPtr<GeofenceState> state = *iter;
            if (state->mPackageName.Equals(packageName)) {
                iter = mFences.Erase(iter);
                continue;
            }

            ++iter;
        }

        ScheduleUpdateFencesLocked();
    }
}

// Runs on the handler (which was passed into LocationManager.requestLocationUpdates())
//@Override
ECode GeofenceManager::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    //synchronized(mLock)
    {
        AutoLock lock(mLock);
        if (mReceivingLocationUpdates) {
            mLastLocationUpdate = location;
        }

        // Update the fences immediately before returning in
        // case the caller is holding a wakelock.
        if (mPendingUpdate) {
            mHandler->RemoveMessages(MSG_UPDATE_FENCES);
        }
        else {
            mPendingUpdate = TRUE;
        }
    }
    UpdateFences();

    return NOERROR;
}

//@Override
ECode GeofenceManager::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}

//@Override
ECode GeofenceManager::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

//@Override
ECode GeofenceManager::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

//@Override
ECode GeofenceManager::OnSendFinished(
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras)
{
    mWakeLock->Release();
    return NOERROR;
}

void GeofenceManager::Dump(
    /* [in] */ IPrintWriter* pw)
{/*
    pw.println("  Geofences:");

    for (GeofenceState state : mFences) {
        pw.append("    ");
        pw.append(state.mPackageName);
        pw.append(" ");
        pw.append(state.mFence.toString());
        pw.append("\n");
    }*/
}

void GeofenceManager::RemoveExpiredFencesLocked()
{
    Int64 time = SystemClock::GetElapsedRealtime();

    List<AutoPtr<GeofenceState> >::Iterator iter = mFences.Begin();
    while (iter != mFences.End()) {
        AutoPtr<GeofenceState> state = *iter;
        if (state->mExpireAt < time) {
            iter = mFences.Erase(iter);
            continue;
        }

        ++iter;
    }
}

void GeofenceManager::ScheduleUpdateFencesLocked()
{
    if (!mPendingUpdate) {
        mPendingUpdate = TRUE;
        Boolean result;
        mHandler->SendEmptyMessage(MSG_UPDATE_FENCES, &result);
    }
}

/**
 * Returns the location received most recently from {@link #onLocationChanged(Location)},
 * or consult {@link LocationManager#getLastLocation()} if none has arrived. Does not return
 * either if the location would be too stale to be useful.
 *
 * @return a fresh, valid Location, or null if none is available
 */
AutoPtr<ILocation> GeofenceManager::GetFreshLocationLocked()
{
    // Prefer mLastLocationUpdate to LocationManager.getLastLocation().
    AutoPtr<ILocation> location = mReceivingLocationUpdates ? mLastLocationUpdate : NULL;
    if (location == NULL && !mFences.IsEmpty()) {
        mLocationManager->GetLastLocation((ILocation**)&location);
    }

    // Early out for null location.
    if (location == NULL) {
        return NULL;
    }

    // Early out for stale location.
    Int64 now = SystemClock::GetElapsedRealtimeNanos();
    Int64 time = 0;
    location->GetElapsedRealtimeNanos(&time);
    if (now - time > MAX_AGE_NANOS) {
        return NULL;
    }

    // Made it this far? Return our fresh, valid location.
    return location;
}

/**
 * The geofence update loop. This function removes expired fences, then tests the most
 * recently-received {@link Location} against each registered {@link GeofenceState}, sending
 * {@link Intent}s for geofences that have been tripped. It also adjusts the active location
 * update request with {@link LocationManager} as appropriate for any active geofences.
 */
// Runs on the handler.
ECode GeofenceManager::UpdateFences()
{
    List<AutoPtr<IPendingIntent> > enterIntents;
    List<AutoPtr<IPendingIntent> > exitIntents;

    {
        AutoLock lock(mLock);
        mPendingUpdate = FALSE;

        // Remove expired fences.
        RemoveExpiredFencesLocked();

        // Get a location to work with, either received via onLocationChanged() or
        // via LocationManager.getLastLocation().
        AutoPtr<ILocation> location = GetFreshLocationLocked();

        // Update all fences.
        // Keep track of the distance to the nearest fence.
        Double minFenceDistance = Elastos::Core::Math::DOUBLE_MAX_VALUE;
        Boolean needUpdates = FALSE;
        Boolean bFlag = FALSE;
        List<AutoPtr<GeofenceState> >::Iterator iter = mFences.Begin();
        for (AutoPtr<GeofenceState> state = *iter; iter != mFences.End(); state = *(++iter)) {
            bFlag = mBlacklist->IsBlacklisted(state->mPackageName);
            if (bFlag) {
/*                if (D) {
                    Slog.d(TAG, "skipping geofence processing for blacklisted app: "
                            + state.mPackageName);
                }*/
                continue;
            }

            needUpdates = TRUE;
            if (location != NULL) {
                Int32 event = state->ProcessLocation(location);
                if ((event & GeofenceState::FLAG_ENTER) != 0) {
                    enterIntents.PushBack(state->mIntent);
                }
                if ((event & GeofenceState::FLAG_EXIT) != 0) {
                    exitIntents.PushBack(state->mIntent);
                }

                // FIXME: Ideally this code should take into account the accuracy of the
                // location fix that was used to calculate the distance in the first place.
                Double fenceDistance = state->GetDistanceToBoundary(); // MAX_VALUE if unknown
                if (fenceDistance < minFenceDistance) {
                    minFenceDistance = fenceDistance;
                }
            }
        }

        // Request or cancel location updates if needed.
        if (needUpdates) {
            // Request location updates.
            // Compute a location update interval based on the distance to the nearest fence.
            Int64 intervalMs;
            if (location != NULL && Elastos::Core::Math::Compare(minFenceDistance, Elastos::Core::Math::DOUBLE_MAX_VALUE) != 0) {
                intervalMs = (Int64)Elastos::Core::Math::Min(MAX_INTERVAL_MS, Elastos::Core::Math::Max(MIN_INTERVAL_MS,
                        (Int64)(minFenceDistance * 1000 / MAX_SPEED_M_S)));
            } else {
                intervalMs = MIN_INTERVAL_MS;
            }
            if (!mReceivingLocationUpdates || mLocationUpdateInterval != intervalMs) {
                mReceivingLocationUpdates = TRUE;
                mLocationUpdateInterval = intervalMs;
                mLastLocationUpdate = location;

                AutoPtr<ILocationRequest> request;
                CLocationRequest::New((ILocationRequest**)&request);

                request->SetInterval(intervalMs);
                request->SetFastestInterval(0);
                AutoPtr<ILooper> looper;
                mHandler->GetLooper((ILooper**)&looper);
                PFL_EX("DEBUG")
//                mLocationManager->RequestLocationUpdates(request, this, looper);
            }
        } else {
            // Cancel location updates.
            if (mReceivingLocationUpdates) {
                mReceivingLocationUpdates = FALSE;
                mLocationUpdateInterval = 0;
                mLastLocationUpdate = NULL;

                mLocationManager->RemoveUpdates(this);
            }
        }
/*
        if (D) {
            Slog.d(TAG, "updateFences: location=" + location
                    + ", mFences.size()=" + mFences.size()
                    + ", mReceivingLocationUpdates=" + mReceivingLocationUpdates
                    + ", mLocationUpdateInterval=" + mLocationUpdateInterval
                    + ", mLastLocationUpdate=" + mLastLocationUpdate);
        }*/
    }

    // release lock before sending intents
    List<AutoPtr<IPendingIntent> >::Iterator iter = exitIntents.Begin();
    for (AutoPtr<IPendingIntent> intent = *iter; iter != exitIntents.End(); intent = *(++iter)) {
        SendIntentExit(intent);
    }

    iter = enterIntents.Begin();
    for (AutoPtr<IPendingIntent> intent = *iter; iter != enterIntents.End(); intent = *(++iter)) {
        SendIntentEnter(intent);
    }
    return NOERROR;
}

void GeofenceManager::SendIntentEnter(
    /* [in] */ IPendingIntent* pendingIntent)
{
//  if (D) {
//        Slog.d(TAG, "sendIntentEnter: pendingIntent=" + pendingIntent);
//    }

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutBooleanExtra(ILocationManager::KEY_PROXIMITY_ENTERING, TRUE);
    SendIntent(pendingIntent, intent);
}

void GeofenceManager::SendIntentExit(
    /* [in] */ IPendingIntent* pendingIntent)
{
//  if (D) {
//        Slog.d(TAG, "sendIntentExit: pendingIntent=" + pendingIntent);
//    }

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutBooleanExtra(ILocationManager::KEY_PROXIMITY_ENTERING, FALSE);
    SendIntent(pendingIntent, intent);
}

void GeofenceManager::SendIntent(
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* intent)
{
    mWakeLock->AcquireLock();
    ECode ec = pendingIntent->Send(mContext, 0, intent, this, NULL,
            Elastos::Droid::Manifest::permission::ACCESS_FINE_LOCATION);
    if(FAILED(ec))
    {
        RemoveFence(NULL, pendingIntent);
        mWakeLock->Release();
    }
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
