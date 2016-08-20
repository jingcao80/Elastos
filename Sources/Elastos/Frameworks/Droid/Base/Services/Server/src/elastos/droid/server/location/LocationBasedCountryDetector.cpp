#include "elastos/droid/server/location/LocationBasedCountryDetector.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Location::CCountry;
using Elastos::Droid::Location::CGeocoder;
using Elastos::Droid::Location::EIID_ILocationListener;
using Elastos::Droid::Location::IAddress;
using Elastos::Droid::Location::IGeocoder;
using Elastos::Core::AutoLock;
using Elastos::Core::CThread;
using Elastos::Core::ICharSequence;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CTimer;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::ITimerTask;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//=============================
//LocationBasedCountryDetector::MyTimerTask
//=============================

LocationBasedCountryDetector::MyTimerTask::MyTimerTask(
    /* [in] */ LocationBasedCountryDetector* host)
    : mHost(host)
{}

ECode LocationBasedCountryDetector::MyTimerTask::Run()
{
    mHost->mTimer = NULL;
    mHost->Stop();
    // Looks like no provider could provide the location, let's try the last
    // known location.
    AutoPtr<ILocation> location = mHost->GetLastKnownLocation();
    mHost->QueryCountryCode(location);
    return NOERROR;
}

//=============================
//LocationBasedCountryDetector::MyLocationListener
//=============================

CAR_INTERFACE_IMPL(LocationBasedCountryDetector::MyLocationListener, Object, ILocationListener)

LocationBasedCountryDetector::MyLocationListener::MyLocationListener(
    /* [in] */ LocationBasedCountryDetector* host)
    : mHost(host)
{}

ECode LocationBasedCountryDetector::MyLocationListener::OnLocationChanged(
    /* [in] */ ILocation* location)
{
    if (location != NULL) {
        mHost->Stop();
        mHost->QueryCountryCode(location);
    }
    return NOERROR;
}

ECode LocationBasedCountryDetector::MyLocationListener::OnStatusChanged(
    /* [in] */ const String& provider,
    /* [in] */ Int32 status,
    /* [in] */ IBundle* extras)
{
    return NOERROR;
}

ECode LocationBasedCountryDetector::MyLocationListener::OnProviderEnabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

ECode LocationBasedCountryDetector::MyLocationListener::OnProviderDisabled(
    /* [in] */ const String& provider)
{
    return NOERROR;
}

//=============================
//LocationBasedCountryDetector::MyRunnable
//=============================

LocationBasedCountryDetector::MyRunnable::MyRunnable(
    /* [in] */ LocationBasedCountryDetector* host,
    /* [in] */ ILocation* location)
    : mHost(host)
    , mLocation(location)
{
}

ECode LocationBasedCountryDetector::MyRunnable::Run()
{
    // prevent mHost be released when calling host->NotifyListener;
    AutoPtr<LocationBasedCountryDetector> host = mHost;

    String countryIso;
    if (mLocation != NULL) {
        countryIso = host->GetCountryFromLocation(mLocation);
    }
    if (countryIso.IsNull()) {
        AutoPtr<ICountry> country;
        CCountry::New(countryIso, ICountry::COUNTRY_SOURCE_LOCATION, (ICountry**)&country);
        host->mDetectedCountry = country;
    }
    else {
        host->mDetectedCountry = NULL;
    }
    host->NotifyListener(host->mDetectedCountry);

    host->mQueryThread = NULL;
    return NOERROR;
}

//=============================
//LocationBasedCountryDetector
//=============================

const String LocationBasedCountryDetector::TAG("LocationBasedCountryDetector");
const Int64 LocationBasedCountryDetector::QUERY_LOCATION_TIMEOUT = 1000 * 60 * 5; // 5 mins

LocationBasedCountryDetector::LocationBasedCountryDetector(
    /* [in] */ IContext* ctx)
    : CountryDetectorBase(ctx)
{
    AutoPtr<IInterface> svTemp;
    ctx->GetSystemService(IContext::LOCATION_SERVICE, (IInterface**)&svTemp);
    mLocationManager = ILocationManager::Probe(svTemp);
}

String LocationBasedCountryDetector::GetCountryFromLocation(
    /* [in] */ ILocation* location)
{
    String country;
    AutoPtr<IGeocoder> geoCoder;
    CGeocoder::New(mContext, (IGeocoder**)&geoCoder);
    Double latitude;
    location->GetLatitude(&latitude);
    Double longitude;
    location->GetLongitude(&longitude);
    AutoPtr<IList> addresses;
    ECode ec = geoCoder->GetFromLocation(latitude, longitude, 1, (IList**)&addresses);
    if (FAILED(ec)) {
        Slogger::W(TAG, "Exception occurs when getting country from location");
        return String(NULL);
    }
    Int32 size;
    if (addresses != NULL && (addresses->GetSize(&size), size > 0)) {
        AutoPtr<IInterface> obj;
        addresses->Get(0, (IInterface**)&obj);
        IAddress::Probe(obj)->GetCountryCode(&country);
    }
    return country;
}

Boolean LocationBasedCountryDetector::IsAcceptableProvider(
    /* [in] */ const String& provider)
{
    // We don't want to actively initiate a location fix here (with gps or network providers).
    //return LocationManager.PASSIVE_PROVIDER.equals(provider);
    return provider.Equals(ILocationManager::PASSIVE_PROVIDER);
}

void LocationBasedCountryDetector::RegisterListener(
    /* [in] */ const String& provider,
    /* [in] */ ILocationListener* listener)
{
    mLocationManager->RequestLocationUpdates(provider, 0, 0, listener);
}

void LocationBasedCountryDetector::UnregisterListener(
    /* [in] */ ILocationListener* listener)
{
    mLocationManager->RemoveUpdates(listener);
}

AutoPtr<ILocation> LocationBasedCountryDetector::GetLastKnownLocation()
{
    AutoPtr<IList> providers;
    mLocationManager->GetAllProviders((IList**)&providers);
    AutoPtr<ILocation> bestLocation;
    AutoPtr<IIterator> iterator;
    ICollection::Probe(providers)->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while(iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        String provider;
        ICharSequence::Probe(obj)->ToString(&provider);
        AutoPtr<ILocation> lastKnownLocation;
        mLocationManager->GetLastKnownLocation(provider, (ILocation**)&lastKnownLocation);
        if (lastKnownLocation != NULL) {
            if (bestLocation == NULL) bestLocation = lastKnownLocation;
            else {
                Int64 elapsedRealtimeNanos1, elapsedRealtimeNanos2;
                bestLocation->GetElapsedRealtimeNanos(&elapsedRealtimeNanos1);
                lastKnownLocation->GetElapsedRealtimeNanos(&elapsedRealtimeNanos2);
                if (elapsedRealtimeNanos1 < elapsedRealtimeNanos2) {
                    bestLocation = lastKnownLocation;
                }
            }
        }
    }
    return bestLocation;
}

Int64 LocationBasedCountryDetector::GetQueryLocationTimeout()
{
    return QUERY_LOCATION_TIMEOUT;
}

AutoPtr<IList> LocationBasedCountryDetector::GetEnabledProviders()
{
    if (mEnabledProviders == NULL) {
        mLocationManager->GetProviders(TRUE, (IList**)&mEnabledProviders);
    }
    return mEnabledProviders;
}

ECode LocationBasedCountryDetector::DetectCountry(
    /* [out] */ ICountry** country)
{
    VALIDATE_NOT_NULL(country)
    {
        AutoLock syncLock(this);
        if (mLocationListeners  != NULL) {
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        // Request the location from all enabled providers.
        AutoPtr<IList> enabledProviders = GetEnabledProviders();
        Int32 totalProviders;
        enabledProviders->GetSize(&totalProviders);
        if (totalProviders > 0) {
            CArrayList::New(totalProviders, (IList**)&mLocationListeners);
            for (Int32 i = 0; i < totalProviders; i++) {
                AutoPtr<IInterface> obj;
                enabledProviders->Get(i, (IInterface**)&obj);
                String provider;
                ICharSequence::Probe(obj)->ToString(&provider);
                if (IsAcceptableProvider(provider)) {
                    AutoPtr<MyLocationListener> listener = new MyLocationListener(this);
                    mLocationListeners->Add((ILocationListener*)listener);
                    RegisterListener(provider, listener);
                }
            }

            CTimer::New((ITimer**)&mTimer);

            mTimer->Schedule((ITimerTask*)(new MyTimerTask(this)), GetQueryLocationTimeout());
        }
        else {
            // There is no provider enabled.
            AutoPtr<ILocation> location = GetLastKnownLocation();
            QueryCountryCode(location);
        }
        *country = mDetectedCountry;
        REFCOUNT_ADD(*country)
    }
    return NOERROR;
}

ECode LocationBasedCountryDetector::Stop()
{
    {
        AutoLock syncLock(this);
        if (mLocationListeners != NULL) {
            Int32 size;
            mLocationListeners->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<IInterface> obj;
                mLocationListeners->Get(i, (IInterface**)&obj);
                UnregisterListener(ILocationListener::Probe(obj));
            }
            mLocationListeners = NULL;
        }
        if (mTimer != NULL) {
            mTimer->Cancel();
            mTimer = NULL;
        }
    }
    return NOERROR;
}

void LocationBasedCountryDetector::QueryCountryCode(
    /* [in] */ ILocation* location)
{
    {
        AutoLock syncLock(this);
        if (location == NULL) {
            NotifyListener(NULL);
            return;
        }
        if (mQueryThread != NULL) return;
        AutoPtr<IRunnable> r = new MyRunnable(this, location);
        CThread::New(r, (IThread**)&mQueryThread);
        mQueryThread->Start();
    }
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
