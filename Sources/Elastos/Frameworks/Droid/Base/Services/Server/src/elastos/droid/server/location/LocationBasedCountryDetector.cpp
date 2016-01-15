
#include "location/LocationBasedCountryDetector.h"

using Elastos::Core::EIID_ICharSequence;
using Elastos::Droid::Location::CGeocoder;
using Elastos::Droid::Location::IGeocoder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String LocationBasedCountryDetector::TAG("LocationBasedCountryDetector");
const Int64 LocationBasedCountryDetector::QUERY_LOCATION_TIMEOUT = 1000 * 60 * 5; // 5 mins

CAR_INTERFACE_IMPL(LocationBasedCountryDetector::QueryRunnable, IRunnable)
CAR_INTERFACE_IMPL(LocationBasedCountryDetector::LocationListener, ILocationListener)

LocationBasedCountryDetector::DetectCountryTimerTask::DetectCountryTimerTask(
    /* [in] */ LocationBasedCountryDetector* host) : mHost(host)
{}

ECode LocationBasedCountryDetector::DetectCountryTimerTask::Run()
{
    mHost->Stop();
    AutoPtr<ILocation> location = mHost->GetLastKnownLocation();
    mHost->QueryCountryCode(location);
    mHost->mTimer = NULL;
    return NOERROR;
}

LocationBasedCountryDetector::LocationListener::LocationListener(
    /* [in] */ LocationBasedCountryDetector* host)
                : mHost(host)
{}

ECode LocationBasedCountryDetector::LocationListener::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    if (location != NULL) {
        mHost->Stop();
        mHost->QueryCountryCode(location);
    }
    return NOERROR;
}

ECode LocationBasedCountryDetector::LocationListener::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}

ECode LocationBasedCountryDetector::LocationListener::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

ECode LocationBasedCountryDetector::LocationListener::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

LocationBasedCountryDetector::QueryRunnable::QueryRunnable(
    /* [in] */ LocationBasedCountryDetector* host,
    /* [in] */ ILocation* location)
                            : mHost(host)
                            , mLocation(location)
{}

ECode LocationBasedCountryDetector::QueryRunnable::Run()
{
    String countryIso;
    if (mLocation != NULL) {
        countryIso = mHost->GetCountryFromLocation(mLocation);
    }
    if (countryIso.IsNullOrEmpty()) {
        CCountry::New(countryIso, ICountry::COUNTRY_SOURCE_LOCATION, (ICountry**)(&(mHost->mDetectedCountry)));
    } else {
        mHost->mDetectedCountry = NULL;
    }
    mHost->NotifyListener(mHost->mDetectedCountry);

    mHost->mQueryThread = NULL;
    return NOERROR;
}

LocationBasedCountryDetector::LocationBasedCountryDetector(
    /* [in] */ IContext* ctx) : CountryDetectorBase(ctx)
{
    assert(ctx != NULL);
    AutoPtr<IInterface> svTemp;
    ctx->GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&svTemp);
    mLocationManager = (ILocationManager*)svTemp->Probe(EIID_ILocationManager);
}


/**
 * Start detecting the country.
 * <p>
 * Queries the location from all location providers, then starts a thread to query the
 * country from GeoCoder.
 */
//@Override
//synchronized
ECode LocationBasedCountryDetector::DetectCountry(
    /* [out] */ ICountry** country)
{
    VALIDATE_NOT_NULL(country);
    *country = NULL;

    AutoLock lock(mLock);
    if (mLocationListeners != NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    // Request the location from all enabled providers.
    AutoPtr< List<String> > enabledProviders = GetEnabledProviders();
    Int32 totalProviders = enabledProviders->GetSize();
    if (totalProviders > 0) {
        mLocationListeners = new List<AutoPtr<LocationListener> >(totalProviders);
        List<String>::Iterator it = enabledProviders->Begin();
        for (; it != enabledProviders->End(); it++) {
            String provider = *it;
            if (IsAcceptableProvider(provider)) {
                AutoPtr<LocationListener> listener = new LocationListener (this);
                mLocationListeners->PushBack(listener);
                RegisterListener(provider, listener);
            }
        }

        AutoPtr<DetectCountryTimerTask> timerTask = new DetectCountryTimerTask(this);
        mTimer = NULL;
        CTimer::New((ITimer**)&mTimer);
        mTimer->Schedule(timerTask, GetQueryLocationTimeout());
    } else {
        // There is no provider enabled.
        QueryCountryCode(GetLastKnownLocation());
    }

    *country = mDetectedCountry;
    REFCOUNT_ADD(*country);
    return NOERROR;
}

/**
 * Stop the current query without notifying the listener.
 */
//@Override
//synchronized
void LocationBasedCountryDetector::Stop()
{
    AutoLock lock(mLock);

    if (mLocationListeners != NULL) {
        List<AutoPtr<LocationListener> >::Iterator it = mLocationListeners->Begin();
        for (; it != mLocationListeners->End(); it++) {
            AutoPtr<LocationListener> listener = *it;
            UnregisterListener(listener.Get());
        }
        mLocationListeners = NULL;
    }

    if (mTimer != NULL) {
        mTimer->Cancel();
        mTimer = NULL;
    }
}

/**
 * @return the ISO 3166-1 two letters country code from the location
 */
String LocationBasedCountryDetector::GetCountryFromLocation(
    /* [in] */ ILocation* location)
{
    assert(location != NULL);

    String country;
    AutoPtr<IGeocoder> geoCoder;// = new Geocoder(mContext);
    CGeocoder::New(mContext, (IGeocoder**)&geoCoder);
    //try {
        AutoPtr<IObjectContainer> addresses;
        Double latitude, longitude;
        location->GetLatitude(&latitude);
        location->GetLongitude(&longitude);
        geoCoder->GetFromLocation(
                latitude, longitude, 1, (IObjectContainer**)&addresses);
        AutoPtr<IAddress> addr;
        if(addresses != NULL)
        {
            AutoPtr<IObjectEnumerator> em;
            addresses->GetObjectEnumerator((IObjectEnumerator**)&em);
            Boolean hasNext;
            if(em->MoveNext(&hasNext), hasNext)
            {
                AutoPtr<IInterface> addrTemp;
                em->Current((IInterface**)&addrTemp);
                addr = (IAddress*)addrTemp->Probe(EIID_IAddress);
            }
        }

        if(addr != NULL)
        {
            addr->GetCountryCode(&country);
        }
//        if (addresses != NULL && addresses.size() > 0) {
//            country = addresses.get(0).getCountryCode();
//        }
    //} catch (IOException e) {
    //    Slog.w(TAG, "Exception occurs when getting country from location");
    //}
    return country;
}

Boolean LocationBasedCountryDetector::IsAcceptableProvider(
    /* [in] */ const String& provider)
{
    // We don't want to actively initiate a location fix here (with gps or network providers).
    //return LocationManager.PASSIVE_PROVIDER.equals(provider);
    return provider.Equals(ILocationManager::PASSIVE_PROVIDER);
}

/**
 * Register a listener with a provider name
 */
void LocationBasedCountryDetector::RegisterListener(
    /* [in] */ const String& provider,
    /* [in] */ ILocationListener* listener)
{
    mLocationManager->RequestLocationUpdates(provider, 0, 0, listener);
}

/**
 * Unregister an already registered listener
 */
void LocationBasedCountryDetector::UnregisterListener(
    /* [in] */ ILocationListener* listener)
{
    mLocationManager->RemoveUpdates(listener);
}

/**
 * @return the last known location from all providers
 */
AutoPtr<ILocation> LocationBasedCountryDetector::GetLastKnownLocation()
{
/*  List<String> providers = mLocationManager->GetAllProviders();
    Location bestLocation = null;
    for (String provider : providers) {
        Location lastKnownLocation = mLocationManager.getLastKnownLocation(provider);
        if (lastKnownLocation != null) {
            if (bestLocation == null ||
                    bestLocation.getElapsedRealtimeNanos() <
                    lastKnownLocation.getElapsedRealtimeNanos()) {
                bestLocation = lastKnownLocation;
            }
        }
    }
    return bestLocation;
*/
    return NULL;//temp
}

/**
 * @return the timeout for querying the location.
 */
Int64 LocationBasedCountryDetector::GetQueryLocationTimeout()
{
    return QUERY_LOCATION_TIMEOUT;
}

AutoPtr< List<String> > LocationBasedCountryDetector::GetEnabledProviders()
{
    if (mEnabledProviders == NULL) {
        AutoPtr<IObjectContainer> providers;
        mLocationManager->GetProviders(TRUE, (IObjectContainer**)&providers);
        AutoPtr<IObjectEnumerator> em;
        providers->GetObjectEnumerator((IObjectEnumerator**)&em);
        Boolean next;
        if(em->MoveNext(&next), next) {
            AutoPtr<IInterface> temp;
            em->Current((IInterface**)&temp);
            ICharSequence* cs = (ICharSequence*)temp->Probe(EIID_ICharSequence);
            if(cs) {
                String str;
                cs->ToString(&str);
                if (!str.IsNullOrEmpty()) {
                    if (mEnabledProviders == NULL) {
                        mEnabledProviders = new List<String>();
                    }
                    mEnabledProviders->PushBack(str);
                }
            }

        }
    }

    return mEnabledProviders;
}

/**
 * Start a new thread to query the country from Geocoder.
 */
//synchronized
void LocationBasedCountryDetector::QueryCountryCode(
    /* [in] */ ILocation* location)
{
    AutoLock lock(mLock);
    if (location == NULL) {
            NotifyListener(NULL);
            return;
    }
    if (mQueryThread != NULL) return;
    AutoPtr<QueryRunnable> runnable = new QueryRunnable(this, location);
    mQueryThread = NULL;
    CThread::New(runnable, (IThread**)&mQueryThread);
    mQueryThread->Start();
/*
    if (location == null) {
            notifyListener(null);
            return;
        }
        if (mQueryThread != null) return;
        mQueryThread = new Thread(new Runnable() {
            @Override
            public void run() {

            }
        });
        mQueryThread.start();
    }
*/
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
