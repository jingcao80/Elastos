
#include "elastos/droid/server/twilight/TwilightService.h"
#include "elastos/droid/server/twilight/TwilightState.h"

#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Text.h>
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Location::CLocation;
using Elastos::Droid::Location::ICriteria;
using Elastos::Droid::Location::CCriteria;
using Elastos::Droid::Location::EIID_ILocationListener;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Core::IRunnable;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Twilight {

const String TwilightService::TAG("TwilightService");
const Boolean TwilightService::DEBUG = FALSE;
const String TwilightService::ACTION_UPDATE_TWILIGHT_STATE("com.android.server.action.UPDATE_TWILIGHT_STATE");



//====================================================================
// TwilightService::TwilightListenerRecord
//====================================================================
TwilightService::TwilightListenerRecord::TwilightListenerRecord(
    /* [in] */ ITwilightListener* listener,
    /* [in] */ IHandler* handler)
    : mListener(listener)
    , mHandler(handler)
{
}

ECode TwilightService::TwilightListenerRecord::PostUpdate()
{
    Boolean result;
    return mHandler->Post(this, &result);
}

ECode TwilightService::TwilightListenerRecord::Run()
{
    return mListener->OnTwilightStateChanged();
}

//====================================================================
// TwilightService::MyTwilightManager
//====================================================================
CAR_INTERFACE_IMPL(TwilightService::MyTwilightManager, Object, ITwilightManager)

ECode TwilightService::MyTwilightManager::GetCurrentState(
    /* [out] */ ITwilightState** state)
{
    VALIDATE_NOT_NULL(state)
    AutoLock lock(mHost->mLock);
    *state = mHost->mTwilightState;
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode TwilightService::MyTwilightManager::RegisterListener(
    /* [in] */ ITwilightListener* listener,
    /* [in] */ IHandler* handler)
{
    AutoLock lock(mHost->mLock);
    AutoPtr<TwilightListenerRecord> record = new TwilightListenerRecord(listener, handler);
    mHost->mListeners.PushBack(record);

    if (mHost->mListeners.GetSize() == 1) {
        mHost->mLocationHandler->EnableLocationUpdates();
    }
    return NOERROR;
}

//====================================================================
// TwilightService::LocationHandler
//====================================================================
const Int32 TwilightService::LocationHandler::MSG_ENABLE_LOCATION_UPDATES = 1;
const Int32 TwilightService::LocationHandler::MSG_GET_NEW_LOCATION_UPDATE = 2;
const Int32 TwilightService::LocationHandler::MSG_PROCESS_NEW_LOCATION = 3;
const Int32 TwilightService::LocationHandler::MSG_DO_TWILIGHT_UPDATE = 4;
const Int64 TwilightService::LocationHandler::LOCATION_UPDATE_MS = IDateUtils::HOUR_IN_MILLIS;
const Int64 TwilightService::LocationHandler::MIN_LOCATION_UPDATE_MS = IDateUtils::MINUTE_IN_MILLIS;
const Float TwilightService::LocationHandler::LOCATION_UPDATE_DISTANCE_METER = 1000 * 20;
const Int64 TwilightService::LocationHandler::LOCATION_UPDATE_ENABLE_INTERVAL_MIN = 5000;
const Int64 TwilightService::LocationHandler::LOCATION_UPDATE_ENABLE_INTERVAL_MAX = IDateUtils::MINUTE_IN_MILLIS;
const Double TwilightService::LocationHandler::FACTOR_GMT_OFFSET_LONGITUDE = IDateUtils::DAY_IN_MILLIS;

TwilightService::LocationHandler::LocationHandler(
    /* [in] */ TwilightService* host)
    : mPassiveListenerEnabled(FALSE)
    , mNetworkListenerEnabled(FALSE)
    , mDidFirstInit(FALSE)
    , mLastNetworkRegisterTime(-MIN_LOCATION_UPDATE_MS)
    , mLastUpdateInterval(0LL)
    , mHost(host)
{
    mTwilightCalculator = new TwilightCalculator();
}

ECode TwilightService::LocationHandler::ProcessNewLocation(
    /* [in] */ ILocation* location)
{
    AutoPtr<IMessage> msg;
    ObtainMessage(MSG_PROCESS_NEW_LOCATION, (IMessage**)&msg);
    msg->SetObj(location ? location : NULL);
    Boolean result;
    return SendMessage(msg, &result);
}

ECode TwilightService::LocationHandler::EnableLocationUpdates()
{
    Boolean result;
    return SendEmptyMessage(MSG_ENABLE_LOCATION_UPDATES, &result);
}

ECode TwilightService::LocationHandler::RequestLocationUpdate()
{
    Boolean result;
    return SendEmptyMessage(MSG_GET_NEW_LOCATION_UPDATE, &result);
}

ECode TwilightService::LocationHandler::RequestTwilightUpdate()
{
    Boolean result;
    return SendEmptyMessage(MSG_DO_TWILIGHT_UPDATE, &result);
}

ECode TwilightService::LocationHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    switch (what) {
        case MSG_PROCESS_NEW_LOCATION: {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<ILocation> location = ILocation::Probe(obj);
            Boolean hasMoved = mHost->HasMoved(mLocation, location);
            Float accuracy, accuracy2;
            location->GetAccuracy(&accuracy);
            mLocation->GetAccuracy(&accuracy2);
            Boolean hasBetterAccuracy = (mLocation == NULL)
                    || (accuracy < accuracy2);
            if (DEBUG) {
                //Slog.d(TAG, "Processing new location: " + location
                //       + ", hasMoved=" + hasMoved
                //       + ", hasBetterAccuracy=" + hasBetterAccuracy);
            }
            if (hasMoved || hasBetterAccuracy) {
                SetLocation(location);
            }
            break;
        }

        case MSG_GET_NEW_LOCATION_UPDATE:
            if (!mNetworkListenerEnabled) {
                // Don't do anything -- we are still trying to get a
                // location.
                return NOERROR;
            }
            if ((mLastNetworkRegisterTime + MIN_LOCATION_UPDATE_MS) >=
                    SystemClock::GetElapsedRealtime()) {
                // Don't do anything -- it hasn't been long enough
                // since we last requested an update.
                return NOERROR;
            }

            // Unregister the current location monitor, so we can
            // register a new one for it to get an immediate update.
            mNetworkListenerEnabled = FALSE;
            if (mHost->mLocationManager) {
                mHost->mLocationManager->RemoveUpdates(mHost->mEmptyLocationListener.Get());
            }

            // Fall through to re-register listener.
        case MSG_ENABLE_LOCATION_UPDATES: {
            // enable network provider to receive at least location updates for a given
            // distance.
            Boolean networkLocationEnabled = FALSE;

            if (mHost->mLocationManager) {
                ECode ec = mHost->mLocationManager->IsProviderEnabled(
                    ILocationManager::NETWORK_PROVIDER, &networkLocationEnabled);
                if (FAILED(ec)) {
                    // we may get IllegalArgumentException if network location provider
                    // does not exist or is not yet installed.
                    networkLocationEnabled = FALSE;
                }
            }

            if (!mNetworkListenerEnabled && networkLocationEnabled) {
                mNetworkListenerEnabled = TRUE;
                mLastNetworkRegisterTime = SystemClock::GetElapsedRealtime();
                if (mHost->mLocationManager) {
                    mHost->mLocationManager->RequestLocationUpdates(ILocationManager::NETWORK_PROVIDER,
                        LOCATION_UPDATE_MS, 0, mHost->mEmptyLocationListener.Get());
                }

                if (!mDidFirstInit) {
                    mDidFirstInit = TRUE;
                    if (mLocation == NULL) {
                        RetrieveLocation();
                    }
                }
            }

            // enable passive provider to receive updates from location fixes (gps
            // and network).
            Boolean passiveLocationEnabled = FALSE;
            if (mHost->mLocationManager) {
                if (FAILED(mHost->mLocationManager->IsProviderEnabled(
                    ILocationManager::PASSIVE_PROVIDER, &passiveLocationEnabled))) {
                    // we may get IllegalArgumentException if passive location provider
                    // does not exist or is not yet installed.
                    passiveLocationEnabled = FALSE;
                }
            }

            if (!mPassiveListenerEnabled && passiveLocationEnabled) {
                mPassiveListenerEnabled = TRUE;
                if (mHost->mLocationManager) {
                    mHost->mLocationManager->RequestLocationUpdates(ILocationManager::PASSIVE_PROVIDER,
                        0, LOCATION_UPDATE_DISTANCE_METER , mHost->mLocationListener.Get());
                }
            }

            if (!(mNetworkListenerEnabled && mPassiveListenerEnabled)) {
                mLastUpdateInterval *= 1.5;
                if (mLastUpdateInterval == 0) {
                    mLastUpdateInterval = LOCATION_UPDATE_ENABLE_INTERVAL_MIN;
                }
                else if (mLastUpdateInterval > LOCATION_UPDATE_ENABLE_INTERVAL_MAX) {
                    mLastUpdateInterval = LOCATION_UPDATE_ENABLE_INTERVAL_MAX;
                }

                Boolean result;
                SendEmptyMessageDelayed(MSG_ENABLE_LOCATION_UPDATES,
                    mLastUpdateInterval, &result);
            }
            break;
        }

        case MSG_DO_TWILIGHT_UPDATE:
            UpdateTwilightState();
            break;
    }

    return NOERROR;
}

void TwilightService::LocationHandler::RetrieveLocation()
{
    AutoPtr<ILocation> location;
    AutoPtr<ICriteria> criteria;
    CCriteria::New((ICriteria**)&criteria);

    if (mHost->mLocationManager) {
        AutoPtr<IList> providers;
        mHost->mLocationManager->GetProviders(criteria, TRUE, (IList**)&providers);

        AutoPtr<IIterator> it;
        providers->GetIterator((IIterator**)&it);
        Boolean hasNext;

        while(it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> current;
            it->GetNext((IInterface**)&current);
            String currentStr = Object::ToString(current);

            AutoPtr<ILocation> lastKnownLocation;
            mHost->mLocationManager->GetLastKnownLocation(currentStr, (ILocation**)&lastKnownLocation);
            //pick the most recent location
            Int64 elapsedRealtimeNanos, lastElapsedRealtimeNanos;
            location->GetElapsedRealtimeNanos(&elapsedRealtimeNanos);
            lastKnownLocation->GetElapsedRealtimeNanos(&lastElapsedRealtimeNanos);
            if (location == NULL || (lastKnownLocation != NULL &&
                   elapsedRealtimeNanos < lastElapsedRealtimeNanos)) {
                location = lastKnownLocation;
            }
        }
    }


    // In the case there is no location available (e.g. GPS fix or network location
    // is not available yet), the longitude of the location is estimated using the timezone,
    // latitude and accuracy are set to get a good average.
    if (location == NULL) {
        AutoPtr<ITime> currentTime;
        CTime::New((ITime**)&currentTime);
        AutoPtr<ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
        Int64 now;
        system->GetCurrentTimeMillis(&now);
        currentTime->Set(now);
        Int64 gmtoff;
        currentTime->GetGmtoff(&gmtoff);
        Int32 isDst;
        currentTime->GetIsDst(&isDst);
        Double lngOffset = FACTOR_GMT_OFFSET_LONGITUDE * (gmtoff - (isDst > 0 ? 3600 : 0));
        CLocation::New(String("fake"), (ILocation**)&location);
        location->SetLongitude(lngOffset);
        location->SetLatitude(0);
        location->SetAccuracy(417000.0f);
        system->GetCurrentTimeMillis(&now);
        location->SetTime(now);
        location->SetElapsedRealtimeNanos(SystemClock::GetElapsedRealtimeNanos());

        if (DEBUG) {
            //Slog.d(TAG, "Estimated location from timezone: " + location);
        }
    }

    SetLocation(location);
}

void TwilightService::LocationHandler::SetLocation(
    /* [in] */ ILocation* location)
{
    mLocation = location;
    UpdateTwilightState();
}

void TwilightService::LocationHandler::UpdateTwilightState()
{
    if (mLocation == NULL) {
        mHost->SetTwilightState(NULL);
        return;
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);

    // calculate yesterday's twilight
    Double latitude;
    mLocation->GetLatitude(&latitude);
    Double longitude;
    mLocation->GetLongitude(&longitude);
    mTwilightCalculator->CalculateTwilight(now - IDateUtils::DAY_IN_MILLIS,
            latitude, longitude);
    Int64 yesterdaySunset = mTwilightCalculator->GetSunset();

    // calculate today's twilight
    mTwilightCalculator->CalculateTwilight(now,
            latitude, longitude);
    Boolean isNight = (mTwilightCalculator->GetState() == TwilightCalculator::NIGHT);
    Int64 todaySunrise = mTwilightCalculator->GetSunrise();
    Int64 todaySunset = mTwilightCalculator->GetSunset();

    // calculate tomorrow's twilight
    mTwilightCalculator->CalculateTwilight(now + IDateUtils::DAY_IN_MILLIS,
            latitude, longitude);
    Int64 tomorrowSunrise = mTwilightCalculator->GetSunrise();

    // set twilight state
    AutoPtr<ITwilightState> state = new TwilightState(isNight, yesterdaySunset,
            todaySunrise, todaySunset, tomorrowSunrise);
    if (DEBUG) {
        //Slog.d(TAG, "Updating twilight state: " + state);
    }
    mHost->SetTwilightState(state);

    // schedule next update
    Int64 nextUpdate = 0L;
    if (todaySunrise == -1 || todaySunset == -1) {
        // In the case the day or night never ends the update is scheduled 12 hours later.
        nextUpdate = now + 12 * IDateUtils::HOUR_IN_MILLIS;
    }
    else
    {
        // add some extra time to be on the safe side.
        nextUpdate += IDateUtils::MINUTE_IN_MILLIS;

        if (now > todaySunset) {
            nextUpdate += tomorrowSunrise;
        }
        else if (now > todaySunrise) {
            nextUpdate += todaySunset;
        }
        else {
            nextUpdate += todaySunrise;
        }
    }

    if (DEBUG) {
        Slogger::D(TAG, "Next update in %d ms", (nextUpdate - now));
    }

    AutoPtr<IIntent> updateIntent;
    CIntent::New(ACTION_UPDATE_TWILIGHT_STATE, (IIntent**)&updateIntent);
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    AutoPtr<IPendingIntent> pendingIntent;
    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    helper->GetBroadcast(context, 0, updateIntent, 0, (IPendingIntent**)&pendingIntent);
    mHost->mAlarmManager->Cancel(pendingIntent);
    mHost->mAlarmManager->SetExact(IAlarmManager::RTC, nextUpdate, pendingIntent);
}


//====================================================================
// TwilightService::UpdateLocationReceiver
//====================================================================
TwilightService::UpdateLocationReceiver::UpdateLocationReceiver(
    /* [in] */ TwilightService* owner)
    : mHost(owner)
{
}

ECode TwilightService::UpdateLocationReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    Boolean result;
    intent->GetBooleanExtra(String("state"), FALSE, &result);
    if (IIntent::ACTION_AIRPLANE_MODE_CHANGED.Equals(action) && !result) {
        // Airplane mode is now off!
        mHost->mLocationHandler->RequestLocationUpdate();
        return NOERROR;
    }

    // Time zone has changed or alarm expired.
    mHost->mLocationHandler->RequestTwilightUpdate();
    return NOERROR;
}


//====================================================================
// TwilightService::EmptyLocationListener
//====================================================================
CAR_INTERFACE_IMPL(TwilightService::EmptyLocationListener, Object, ILocationListener);

TwilightService::EmptyLocationListener::EmptyLocationListener()
{}


ECode TwilightService::EmptyLocationListener::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    return NOERROR;
}

ECode TwilightService::EmptyLocationListener::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

ECode TwilightService::EmptyLocationListener::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

ECode TwilightService::EmptyLocationListener::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}


//====================================================================
// TwilightService::MyLocationListener
//====================================================================
CAR_INTERFACE_IMPL(TwilightService::MyLocationListener, Object, ILocationListener);

TwilightService::MyLocationListener::MyLocationListener(
    /* [in] */ TwilightService* owner)
    : mHost(owner)
{
}

ECode TwilightService::MyLocationListener::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    mHost->mLocationHandler->ProcessNewLocation(location);
    return NOERROR;
}

ECode TwilightService::MyLocationListener::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    return E_NOT_IMPLEMENTED;
}

ECode TwilightService::MyLocationListener::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    return E_NOT_IMPLEMENTED;
}

ECode TwilightService::MyLocationListener::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    return E_NOT_IMPLEMENTED;
}


//====================================================================
// TwilightService
//====================================================================
TwilightService::TwilightService()
{
}

ECode TwilightService::constructor(
    /* [in] */ IContext* context)
{
    return SystemService::constructor(context);
}

ECode TwilightService::OnStart()
{
    mUpdateLocationReceiver = new UpdateLocationReceiver(this);
    mEmptyLocationListener = new EmptyLocationListener();
    mLocationListener = new MyLocationListener(this);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&service);
    mAlarmManager = IAlarmManager::Probe(service);
    service = NULL;
    context->GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&service);
    mLocationManager = ILocationManager::Probe(service);
    mLocationHandler = new LocationHandler(this);
    mLocationHandler->constructor();

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED,(IIntentFilter**)&filter);
    filter->AddAction(IIntent::ACTION_TIME_CHANGED);
    filter->AddAction(IIntent::ACTION_TIMEZONE_CHANGED);
    filter->AddAction(ACTION_UPDATE_TWILIGHT_STATE);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(mUpdateLocationReceiver.Get(), filter, (IIntent**)&intent);

    mService = new MyTwilightManager(this);
    PublishLocalService(EIID_ITwilightManager, mService);
    return NOERROR;
}

void TwilightService::SetTwilightState(
    /* [in] */ ITwilightState* state)
{
    AutoLock lock(mLock);
    if (mTwilightState.Get() != state) {
        if (DEBUG) {
            Slogger::D(TAG, "Twilight state changed: %s", TO_CSTR(state));
        }

        mTwilightState = state;
        List<AutoPtr<TwilightListenerRecord> >::Iterator iter = mListeners.Begin();
        for (; iter != mListeners.End(); ++iter) {
            (*iter)->PostUpdate();
        }
    }
}

Boolean TwilightService::HasMoved(
    /* [in] */ ILocation* from,
    /* [in] */ ILocation* to)
{
    if (to == NULL) {
        return FALSE;
    }

    if (from == NULL) {
        return TRUE;
    }

    Int64 elapsedRealtimeNanos, elapsedRealtimeNanos2;
    to->GetElapsedRealtimeNanos(&elapsedRealtimeNanos);
    from->GetElapsedRealtimeNanos(&elapsedRealtimeNanos2);
    // if new location is older than the current one, the device hasn't moved.
    if (elapsedRealtimeNanos < elapsedRealtimeNanos2) {
        return FALSE;
    }

    // Get the distance between the two points.
    Float distance;
    from->DistanceTo(to, &distance);

    // Get the total accuracy radius for both locations.
    Float accuracy, accuracy2;
    from->GetAccuracy(&accuracy);
    to->GetAccuracy(&accuracy2);
    Float totalAccuracy = accuracy + accuracy2;

    // If the distance is greater than the combined accuracy of the two
    // points then they can't overlap and hence the user has moved.
    return distance >= totalAccuracy;
}

} // namespace Twilight
} // namespace Server
} // namespace Droid
} // namespace Elastos
