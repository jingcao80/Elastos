
#ifndef __ELASTOS_DROID_SERVER_LOCATION_LOCATIONBASEDCOUNTRYDETECTOR_H__
#define __ELASTOS_DROID_SERVER_LOCATION_LOCATIONBASEDCOUNTRYDETECTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/server/location/CountryDetectorBase.h"
#include <elastos/core/Object.h>
#include <elastos/utility/TimerTask.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationListener;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Core::Runnable;
using Elastos::Droid::Server::Location::CountryDetectorBase;
using Elastos::Core::IThread;
using Elastos::Utility::IList;
using Elastos::Utility::ITimer;
using Elastos::Utility::TimerTask;

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
class LocationBasedCountryDetector
    : public CountryDetectorBase
{
private:
    class MyTimerTask
        : public TimerTask
    {
    public:
        MyTimerTask(
            /* [in] */ LocationBasedCountryDetector* host);

        CARAPI Run();

    private:
        AutoPtr<LocationBasedCountryDetector> mHost;    // hold host
    };

    class MyLocationListener
        : public Object
        , public ILocationListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyLocationListener(
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

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ LocationBasedCountryDetector* host,
            /* [in] */ ILocation* location);

        CARAPI Run();

    private:
        AutoPtr<LocationBasedCountryDetector> mHost;    // hold host
        AutoPtr<ILocation> mLocation;
    };

public:
    LocationBasedCountryDetector(
        /* [in] */ IContext* ctx);

    ~LocationBasedCountryDetector();

    TO_STRING_IMPL("LocationBasedCountryDetector")

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
    CARAPI Stop();

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

    CARAPI_(AutoPtr<IList>) GetEnabledProviders();

private:
    /**
     * Start a new thread to query the country from Geocoder.
     */
    //synchronized
    CARAPI_(void) QueryCountryCode(
        /* [in] */ ILocation* location);

    CARAPI TimerTaskRun();

    CARAPI QueryCountryCodeRun(
        /* [in] */ const String& countryIso);

    CARAPI OnLocationChanged(
        /* [in] */ ILocation* location);

protected:
    /**
     * Used for canceling location query
     */
    AutoPtr<ITimer> mTimer;

    /**
     * The thread to query the country from the GeoCoder.
     */
    AutoPtr<IThread> mQueryThread;
    AutoPtr<IList> mLocationListeners;

private:
    const static Int64 QUERY_LOCATION_TIMEOUT;
    AutoPtr<ILocationManager> mLocationManager;
    AutoPtr<IList> mEnabledProviders;
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_LOCATIONBASEDCOUNTRYDETECTOR_H__
