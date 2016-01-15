
#ifndef __ELASTOS_DROID_SERVER_LOCATION_LOCATIONBASEDCOUNTRYDETECTOR_H__
#define __ELASTOS_DROID_SERVER_LOCATION_LOCATIONBASEDCOUNTRYDETECTOR_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif
#include "elastos/droid/ext/frameworkext.h"
#include "location/CountryDetectorBase.h"
#include <elastos/TimerTask.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Utility::Etl::List;
using Elastos::Core::ICharSequence;
using Elastos::Utility::TimerTask;
using Elastos::Utility::ITimerTask;
using Elastos::Utility::EIID_ITimerTask;
using Elastos::Core::IRunnable;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::ITimer;
using Elastos::Utility::CTimer;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Location::CCountry;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationListener;
using Elastos::Droid::Location::EIID_ILocationListener;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Location::EIID_ILocationManager;
using Elastos::Droid::Location::IAddress;
using Elastos::Droid::Location::EIID_IAddress;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * This class detects which country the user currently is in through the enabled
 * location providers and the GeoCoder
 * <p>
 * Use {@link #detectCountry} to start querying. If the location can not be
 * resolved within the given time, the last known location will be used to get
 * the user country through the GeoCoder. The IllegalStateException will be
 * thrown if there is a ongoing query.
 * <p>
 * The current query can be stopped by {@link #stop()}
 *
 * @hide
 */
class LocationBasedCountryDetector : public CountryDetectorBase
{
private:
    const static String TAG;// = "LocationBasedCountryDetector";

    class DetectCountryTimerTask : public TimerTask
    {
    public:
        DetectCountryTimerTask(
            /* [in] */ LocationBasedCountryDetector* host);

        CARAPI Run();

    private:
        LocationBasedCountryDetector* mHost;
    };

    class LocationListener
            : public ILocationListener
            , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        LocationListener(
            /* [in] */ LocationBasedCountryDetector* host);

        CARAPI OnLocationChanged(
            /* [in] */ ILocation* location);

        CARAPI OnStatusChanged(
            /* [in] */ const String& provider,
            /* [in] */ Int32 status,
            /* [in] */ IBundle* extras);

        CARAPI OnProviderEnabled(
            /* [in] */ const String& provider);

        CARAPI OnProviderDisabled(
            /* [in] */ const String& provider);
    private:
        LocationBasedCountryDetector* mHost;

    };

    class QueryRunnable
            : public IRunnable
            , public ElRefBase
    {
    public:
        CAR_INTERFACE_DECL()

        QueryRunnable(
            /* [in] */ LocationBasedCountryDetector* host,
            /* [in] */ ILocation* location);

        CARAPI Run();
    private:
        LocationBasedCountryDetector* mHost;
        ILocation* mLocation;
    };
public:
    LocationBasedCountryDetector(
        /* [in] */ IContext* ctx);

    /**
     * Start detecting the country.
     * <p>
     * Queries the location from all location providers, then starts a thread to query the
     * country from GeoCoder.
     */
    //@Override
    //synchronized
    CARAPI DetectCountry(
        /* [out] */ ICountry** country);

    /**
     * Stop the current query without notifying the listener.
     */
    //@Override
    //synchronized
    CARAPI_(void) Stop();

protected:

    /**
     * @return the ISO 3166-1 two letters country code from the location
     */
    CARAPI_(String) GetCountryFromLocation(
        /* [in] */ ILocation* location);

    CARAPI_(Boolean) IsAcceptableProvider(
        /* [in] */ const String& provider);

    /**
     * Register a listener with a provider name
     */
    CARAPI_(void) RegisterListener(
        /* [in] */ const String& provider,
        /* [in] */ ILocationListener* listener);

    /**
     * Unregister an already registered listener
     */
    CARAPI_(void) UnregisterListener(
        /* [in] */ ILocationListener* listener);

    /**
     * @return the last known location from all providers
     */
    CARAPI_(AutoPtr<ILocation>) GetLastKnownLocation();

    /**
     * @return the timeout for querying the location.
     */
    CARAPI_(Int64) GetQueryLocationTimeout();

    CARAPI_(AutoPtr< List<String> >) GetEnabledProviders();


protected:
    /**
     * Used for canceling location query
     */
    AutoPtr<ITimer> mTimer;

    /**
     * The thread to query the country from the GeoCoder.
     */
    AutoPtr<IThread> mQueryThread;
    AutoPtr<List<AutoPtr<LocationListener> > > mLocationListeners;

private:

    /**
     * Start a new thread to query the country from Geocoder.
     */
    //synchronized
    CARAPI_(void) QueryCountryCode(
        /* [in] */ ILocation* location);

private:
    const static Int64 QUERY_LOCATION_TIMEOUT;// = 1000 * 60 * 5; // 5 mins

    AutoPtr<ILocationManager> mLocationManager;
    AutoPtr<List<String> > mEnabledProviders;

    Object mLock;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_LOCATIONBASEDCOUNTRYDETECTOR_H__
