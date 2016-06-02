#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/os/SystemClock.h"
// #include "elastos/droid/server/LocationManagerService.h"
#include "elastos/droid/server/location/GeofenceState.h"
#include "elastos/droid/server/location/GeofenceManager.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::EIID_IPendingIntentOnFinished;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Location::CLocationRequest;
using Elastos::Droid::Location::EIID_ILocationListener;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::SystemClock;
// using Elastos::Droid::Server::LocationManagerService;
using Elastos::Droid::Server::Location::GeofenceState;
using Elastos::Droid::Manifest;
using Elastos::Core::AutoLock;
using Elastos::Core::CDouble;
using Elastos::Core::CString;
using Elastos::Core::IAppendable;
using Elastos::Core::ICharSequence;
using Elastos::Core::IComparable;
using Elastos::Core::IDouble;
using Elastos::Core::Math;
using Elastos::Utility::IIterable;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//==========================
//GeofenceManager::GeofenceHandler
//==========================

GeofenceManager::GeofenceHandler::GeofenceHandler(
    /* [in] */ GeofenceManager* host)
    : mHost(host)
{}

ECode GeofenceManager::GeofenceHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    if (what == MSG_UPDATE_FENCES) {
        mHost->UpdateFences();
    }
    return NOERROR;
}

//==========================
//GeofenceManager
//==========================

const String GeofenceManager::TAG("GeofenceManager");
const Boolean GeofenceManager::D = FALSE;//LocationManagerService.D
const Int32 GeofenceManager::MSG_UPDATE_FENCES = 1;
const Int32 GeofenceManager::MAX_SPEED_M_S = 100;
const Int64 GeofenceManager::MAX_AGE_NANOS = 5L * 60L * 300000000000L;
const Int64 GeofenceManager::MIN_INTERVAL_MS = 1 * 60 * 1000;
const Int64 GeofenceManager::MAX_INTERVAL_MS = 2 * 60 * 60 * 1000;

CAR_INTERFACE_IMPL_2(GeofenceManager, Object, ILocationListener, IPendingIntentOnFinished)

GeofenceManager::GeofenceManager(
    /* [in] */ IContext* context,
    /* [in] */ LocationBlacklist* blacklist)
    : mReceivingLocationUpdates(FALSE)
    , mLocationUpdateInterval(0L)
    , mPendingUpdate(FALSE)
{
    mContext = context;
    AutoPtr<IInterface> serviceTemp;
    mContext->GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&serviceTemp);
    mLocationManager = ILocationManager::Probe(serviceTemp);
    serviceTemp = NULL;
    mContext->GetSystemService(IContext::APP_OPS_SERVICE, (IInterface**)&serviceTemp);
    mAppOps = IAppOpsManager::Probe(serviceTemp);
    serviceTemp = NULL;
    mContext->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&serviceTemp);
    AutoPtr<IPowerManager> powerManager = IPowerManager::Probe(serviceTemp);
    powerManager->NewWakeLock(IPowerManager::PARTIAL_WAKE_LOCK, TAG, (IPowerManagerWakeLock**)&mWakeLock);
    mHandler = new GeofenceHandler(this);
    mBlacklist = blacklist;
}

ECode GeofenceManager::AddFence(
    /* [in] */ ILocationRequest* request,
    /* [in] */ IGeofence* geofence,
    /* [in] */ IPendingIntent* intent,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 allowedResolutionLevel,
    /* [in] */ const String& packageName)
{
    if (D) {
        String r,g,i;
        IObject::Probe(request)->ToString(&r);
        IObject::Probe(geofence)->ToString(&g);
        IObject::Probe(intent)->ToString(&i);
        Slogger::D(TAG,
            "addFence: request=%s, geofence=%s, intent=%s, uid=%d, packageName=%s",
            r.string(), g.string(), i.string(), uid,packageName.string());
    }

    Int64 expireAt;
    request->GetExpireAt(&expireAt);
    AutoPtr<GeofenceState> state = new GeofenceState(geofence, expireAt,
        allowedResolutionLevel, uid, packageName, intent);
    {    AutoLock syncLock(this);
        Int32 size;
        mFences->GetSize(&size);
        // first make sure it doesn't already exist
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> obj;
            mFences->Get(i, (IInterface**)&obj);
            AutoPtr<GeofenceState> w = (GeofenceState*)(IObject::Probe(obj));
            Boolean b1, b2;
            IObject::Probe(geofence)->Equals(w->mFence, &b1);
            IObject::Probe(intent)->Equals(w->mIntent, &b2);
            if (b1 && b2) {
                // already exists, remove the old one
                mFences->Remove(i);
                break;
            }
        }
        //TODO
        // mFences->Add(IObject::Probe(state));
        ScheduleUpdateFencesLocked();
    }
    return NOERROR;
}

ECode GeofenceManager::RemoveFence(
    /* [in] */ IGeofence* fence,
    /* [in] */ IPendingIntent* intent)
{
    if (D) {
        String f, i;
        IObject::Probe(fence)->ToString(&f);
        IObject::Probe(intent)->ToString(&i);
        Slogger::D(TAG, "removeFence: fence=%s, intent=%s", f.string(), i.string());
    }
    {    AutoLock syncLock(this);
        AutoPtr<IIterator> iter;
        IIterable::Probe(mFences)->GetIterator((IIterator**)&iter);
        Boolean hasNext;
        while (iter->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            iter->GetNext((IInterface**)&obj);
            AutoPtr<GeofenceState> state = (GeofenceState*)(IObject::Probe(obj));
            Boolean isEquals;
            IObject::Probe(state->mIntent)->Equals(intent, &isEquals);
            if (isEquals) {
                if (fence == NULL) {
                    // always remove
                    iter->Remove();
                }
                else {
                    // just remove matching fences
                    IObject::Probe(fence)->Equals(state->mFence, &isEquals);
                    if (isEquals) {
                        iter->Remove();
                    }
                }
            }
        }
        ScheduleUpdateFencesLocked();
    }
    return NOERROR;
}

ECode GeofenceManager::RemoveFence(
    /* [in] */ const String& packageName)
{
    if (D) {
        Slogger::D(TAG, "removeFence: packageName=%s", packageName.string());
    }
    {    AutoLock syncLock(this);
        AutoPtr<IIterator> iter;
        IIterable::Probe(mFences)->GetIterator((IIterator**)&iter);
        Boolean hasNext;
        while (iter->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            iter->GetNext((IInterface**)&obj);
            AutoPtr<GeofenceState> state = (GeofenceState*)(IObject::Probe(obj));
            if (state->mPackageName.Equals(packageName)) {
                iter->Remove();
            }
        }
        ScheduleUpdateFencesLocked();
    }
    return NOERROR;
}

void GeofenceManager::RemoveExpiredFencesLocked()
{
    Int64 time = SystemClock::GetElapsedRealtime();
    AutoPtr<IIterator> iter;
    IIterable::Probe(mFences)->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<GeofenceState> state = (GeofenceState*)(IObject::Probe(obj));
        if (state->mExpireAt < time) {
            iter->Remove();
        }
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

AutoPtr<ILocation> GeofenceManager::GetFreshLocationLocked()
{
    // Prefer mLastLocationUpdate to LocationManager.getLastLocation().
    AutoPtr<ILocation> location = mReceivingLocationUpdates ? mLastLocationUpdate : NULL;
    Boolean isEmpty;
    mFences->IsEmpty(&isEmpty);
    if (location == NULL && !isEmpty) {
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

ECode GeofenceManager::UpdateFences()
{
    AutoPtr<IList> enterIntents;
    AutoPtr<IList> exitIntents;
    {    AutoLock syncLock(this);
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
        AutoPtr<IIterator> iter;
        IIterable::Probe(mFences)->GetIterator((IIterator**)&iter);
        Boolean hasNext;
        while(iter->HasNext(&hasNext), &hasNext) {
            AutoPtr<IInterface> obj;
            iter->GetNext((IInterface**)&obj);
            AutoPtr<GeofenceState> state = (GeofenceState*)(IObject::Probe(obj));
            Boolean isBlacklisted;
            if (mBlacklist->IsBlacklisted(state->mPackageName, &isBlacklisted), isBlacklisted) {
                if (D) {
                    Slogger::D(TAG, "skipping geofence processing for blacklisted app: %s"
                        , (state->mPackageName).string());
                }
                continue;
            }

            Int32 op = 0;
            // Int32 op = LocationManagerService::ResolutionLevelToOp(state->mAllowedResolutionLevel);
            if (op >= 0) {
                Int32 noteValue;
                mAppOps->NoteOpNoThrow(IAppOpsManager::OP_FINE_LOCATION, state->mUid,
                    state->mPackageName, &noteValue);
                if (noteValue != IAppOpsManager::MODE_ALLOWED) {
                    if (D) {
                        Slogger::D(TAG, "skipping geofence processing for no op app: %s"
                            , (state->mPackageName).string());
                    }
                    continue;
                }
            }

            needUpdates = TRUE;
            if (location != NULL) {
                Int32 event;
                state->ProcessLocation(location, &event);
                if ((event & GeofenceState::FLAG_ENTER) != 0) {
                    enterIntents->Add(state->mIntent);
                }
                if ((event & GeofenceState::FLAG_EXIT) != 0) {
                    exitIntents->Add(state->mIntent);
                }

                // FIXME: Ideally this code should take into account the accuracy of the
                // location fix that was used to calculate the distance in the first place.
                Double fenceDistance;
                state->GetDistanceToBoundary(&fenceDistance); // MAX_VALUE if unknown
                if (fenceDistance < minFenceDistance) {
                    minFenceDistance = fenceDistance;
                }
            }
        }

        // Request or cancel location updates if needed.
        if (needUpdates) {
            // Request location updates.
            // Compute a location update interval based on the distance to the nearest fence.
            AutoPtr<IDouble> d1, d2;
            CDouble::New(minFenceDistance, (IDouble**)&d1);
            CDouble::New(Elastos::Core::Math::DOUBLE_MAX_VALUE, (IDouble**)&d2);
            Int32 compareValue;
            IComparable::Probe(d1)->CompareTo(d2, &compareValue);
            Int64 intervalMs;
            if (location != NULL && compareValue != 0) {
                intervalMs = (Int64)Elastos::Core::Math::Min(MAX_INTERVAL_MS, Elastos::Core::Math::Max(MIN_INTERVAL_MS,
                    (Int64)(minFenceDistance * 1000 / MAX_SPEED_M_S)));
            }
            else {
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
                mLocationManager->RequestLocationUpdates(request.Get(), this, looper.Get());
            }
        }
        else {
            // Cancel location updates.
            if (mReceivingLocationUpdates) {
                mReceivingLocationUpdates = FALSE;
                mLocationUpdateInterval = 0;
                mLastLocationUpdate = NULL;

                mLocationManager->RemoveUpdates(this);
            }
        }

        if (D) {
            String locationStr,locationStr2;
            IObject::Probe(location)->ToString(&locationStr);
            IObject::Probe(mLastLocationUpdate)->ToString(&locationStr2);
            Int32 size;
            mFences->GetSize(&size);
            Slogger::D(TAG, "updateFences: location=%s, mFences.size()=%d, mReceivingLocationUpdates=%s, mLocationUpdateInterval=%ld, mLastLocationUpdate=",
                locationStr.string(), size, mReceivingLocationUpdates ? "TRUE" : "FALSE", mLocationUpdateInterval, locationStr2.string());
        }
    }

    // release lock before sending intents
    AutoPtr<IIterator> eiIter;
    IIterable::Probe(exitIntents)->GetIterator((IIterator**)&eiIter);
    Boolean hn;
    while(eiIter->HasNext(&hn), hn) {
        AutoPtr<IInterface> obj;
        eiIter->GetNext((IInterface**)&obj);
        AutoPtr<IPendingIntent> intent = IPendingIntent::Probe(obj);
        SendIntentExit(intent.Get());
    }
    eiIter = NULL;
    IIterable::Probe(enterIntents)->GetIterator((IIterator**)&eiIter);
    hn = FALSE;
    while(eiIter->HasNext(&hn), hn) {
        AutoPtr<IInterface> obj;
        eiIter->GetNext((IInterface**)&obj);
        AutoPtr<IPendingIntent> intent = IPendingIntent::Probe(obj);
        SendIntentEnter(intent.Get());
    }
    return NOERROR;
}

void GeofenceManager::SendIntentEnter(
    /* [in] */ IPendingIntent* pendingIntent)
{
    if (D) {
        String str;
        IObject::Probe(pendingIntent)->ToString(&str);
        Slogger::D(TAG, "sendIntentEnter: pendingIntent=%s", str.string());
    }

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutBooleanExtra(ILocationManager::KEY_PROXIMITY_ENTERING, TRUE);
    SendIntent(pendingIntent, intent);
}

void GeofenceManager::SendIntentExit(
    /* [in] */ IPendingIntent* pendingIntent)
{
    if (D) {
        String str;
        IObject::Probe(pendingIntent)->ToString(&str);
        Slogger::D(TAG, "sendIntentExit: pendingIntent=%s", str.string());
    }

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    intent->PutBooleanExtra(ILocationManager::KEY_PROXIMITY_ENTERING, FALSE);
    SendIntent(pendingIntent, intent);
}

ECode GeofenceManager::SendIntent(
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* intent)
{
    mWakeLock->AcquireLock();
    ECode ec = pendingIntent->Send(mContext, 0, intent, this, NULL,
            Elastos::Droid::Manifest::permission::ACCESS_FINE_LOCATION);
    if(FAILED(ec))
    {
        RemoveFence(NULL, pendingIntent);
        mWakeLock->ReleaseLock();
        return E_PENDING_INTENT_CANCELED_EXCEPTION;
    }
    return NOERROR;
}

ECode GeofenceManager::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    {    AutoLock syncLock(this);
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

ECode GeofenceManager::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}

ECode GeofenceManager::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

ECode GeofenceManager::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

ECode GeofenceManager::OnSendFinished(
    /* [in] */ IPendingIntent* pendingIntent,
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& resultData,
    /* [in] */ IBundle* resultExtras)
{
    return mWakeLock->ReleaseLock();
}

ECode GeofenceManager::Dump(
    /* [in] */ IPrintWriter* pw)
{
    pw->Println(String("  Geofences:"));
    AutoPtr<IIterator> iter;
    IIterable::Probe(mFences)->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while(iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iter->GetNext((IInterface**)&obj);
        AutoPtr<GeofenceState> state = (GeofenceState*)(IObject::Probe(obj));
        AutoPtr<ICharSequence> cs;
        CString::New(String("    "), (ICharSequence**)&cs);
        AutoPtr<IAppendable> appendable = IAppendable::Probe(pw);
        appendable->Append(cs.Get());
        cs = NULL;
        CString::New(state->mPackageName, (ICharSequence**)&cs);
        appendable->Append(cs.Get());
        // appendable->AppendChar("");
        cs = NULL;
        String fence;
        IObject::Probe(state->mFence)->ToString(&fence);
        CString::New(fence, (ICharSequence**)&cs);
        appendable->Append(cs.Get());
        // appendable->AppendChar("\n");
    }
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos