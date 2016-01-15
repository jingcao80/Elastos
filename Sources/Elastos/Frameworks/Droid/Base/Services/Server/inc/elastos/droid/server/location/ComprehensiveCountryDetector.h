
#ifndef __ELASTOS_DROID_SERVER_LOCATION_COMPREHENSIVECOUNTRYDETECTOR_H__
#define __ELASTOS_DROID_SERVER_LOCATION_COMPREHENSIVECOUNTRYDETECTOR_H__

#ifdef DROID_CORE
#include "Elastos.Droid.Core_server.h"
#elif defined(DROID_SERVER)
#include "Elastos.Droid.Core.h"
#endif
#include "location/CountryDetectorBase.h"
#include <elastos/TimerTask.h>
#include "elastos/droid/os/Runnable.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::TimerTask;
using Elastos::Utility::ITimer;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::ICountryListener;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

/**
 * This class is used to detect the country where the user is. The sources of
 * country are queried in order of reliability, like
 * <ul>
 * <li>Mobile network</li>
 * <li>Location</li>
 * <li>SIM's country</li>
 * <li>Phone's locale</li>
 * </ul>
 * <p>
 * Call the {@link #detectCountry()} to get the available country immediately.
 * <p>
 * To be notified of the future country change, using the
 * {@link #setCountryListener(CountryListener)}
 * <p>
 * Using the {@link #stop()} to stop listening to the country change.
 * <p>
 * The country information will be refreshed every
 * {@link #LOCATION_REFRESH_INTERVAL} once the location based country is used.
 *
 * @hide
 */
class ComprehensiveCountryDetector : public CountryDetectorBase
{

private:
    const static String TAG;// = "CountryDetector";

    class MyTimerTask : public TimerTask
    {
    public:
        MyTimerTask(
            /* [in] */ ComprehensiveCountryDetector* host);

        CARAPI Run();

    private:
        ComprehensiveCountryDetector* mHost;
    };

    class MyRunnable : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ ComprehensiveCountryDetector* host,
            /* [in] */ ICountry* country,
            /* [in] */ ICountry* detectedCountry,
            /* [in] */ Boolean notifyChange,
            /* [in] */ Boolean startLocationBasedDetection);

        CARAPI Run();

    private:
        ComprehensiveCountryDetector* mHost;
        AutoPtr<ICountry> mCountry;
        AutoPtr<ICountry> mDetectedCountry;
        Boolean mNotifyChange;
        Boolean mStartLocationBasedDetection;
    };

public:
    /* package */ static const Boolean DEBUG;// = FALSE;

public:
    ComprehensiveCountryDetector(
        /* [in] */ IContext* context);

    //@Override
    CARAPI_(AutoPtr<ICountry>) DetectCountry();

    //@Override
    CARAPI_(void) Stop();

    //@Override
    CARAPI_(void) SetCountryListener(
        /* [in] */ ICountryListener* listener);

    CARAPI_(void) RunAfterDetection(
        /* [in] */ ICountry* country,
        /* [in] */ ICountry* detectedCountry,
        /* [in] */ const Boolean notifyChange,
        /* [in] */ const Boolean startLocationBasedDetection);

    //@Override
    CARAPI_(String) ToString();

protected:
    /**
     * @return the country from the mobile network.
     */
    CARAPI_(AutoPtr<ICountry>) GetNetworkBasedCountry();

    /**
     * @return the cached location based country.
     */
    CARAPI_(AutoPtr<ICountry>) GetLastKnownLocationBasedCountry();

    /**
     * @return the country from SIM card
     */
    CARAPI_(AutoPtr<ICountry>) GetSimBasedCountry();

    /**
     * @return the country from the system's locale.
     */
    CARAPI_(AutoPtr<ICountry>) GetLocaleCountry();

    /**
     * Run the tasks in the service's thread.
     */
    CARAPI_(void) RunAfterDetectionAsync(
        /* [in] */ ICountry* country,
        /* [in] */ ICountry* detectedCountry,
        /* [in] */ Boolean notifyChange,
        /* [in] */ Boolean startLocationBasedDetection);

    CARAPI_(AutoPtr<CountryDetectorBase>) CreateLocationBasedCountryDetector();

    CARAPI_(Boolean) IsAirplaneModeOff();

    /* synchronized */
    CARAPI_(void) AddPhoneStateListener();

    /* synchronized */
    CARAPI_(void) RemovePhoneStateListener();

    CARAPI_(Boolean) IsGeoCoderImplemented();

protected:
    AutoPtr<CountryDetectorBase> mLocationBasedCountryDetector;
    AutoPtr<ITimer> mLocationRefreshTimer;

private:
    /**
     * Get the country from different sources in order of the reliability.
     */
    CARAPI_(AutoPtr<ICountry>) GetCountry();

    /**
     * Attempt to add this {@link Country} to the debug logs.
     */
    CARAPI_(void) AddToLogs(
        /* [in] */ ICountry* country);

    CARAPI_(Boolean) IsNetworkCountryCodeAvailable();

    /**
     * @param notifyChange indicates whether the listener should be notified the change of the
     * country
     * @param startLocationBasedDetection indicates whether the LocationBasedCountryDetector could
     * be started if the current country source is less reliable than the location.
     * @return the current available UserCountry
     */
    CARAPI_(AutoPtr<ICountry>) DetectCountry(
        /* [in] */ Boolean notifyChange,
        /* [in] */ Boolean startLocationBasedDetection);

    /**
     * Find the country from LocationProvider.
     */
    /* synchronized */
    CARAPI_(void) StartLocationBasedDetector(
        /* [in] */ ICountryListener* listener);

    /* synchronized */
    CARAPI_(void) StopLocationBasedDetector();

    /**
     * Notify the country change.
     */
    CARAPI_(void) NotifyIfCountryChanged(
        /* [in] */ ICountry* country,
        /* [in] */ ICountry* detectedCountry);

    /**
     * Schedule the next location refresh. We will do nothing if the scheduled task exists.
     */
    /* synchronized */
    CARAPI_(void) ScheduleLocationRefresh();

    /**
     * Cancel the scheduled refresh task if it exists
     */
    /* synchronized */
    CARAPI_(void) CancelLocationRefresh();

private:

    /**
     * Max length of logs to maintain for debugging.
     */
    static const Int32 MAX_LENGTH_DEBUG_LOGS;// = 20;

    /**
     * The refresh interval when the location based country was used
     */
    const static Int64 LOCATION_REFRESH_INTERVAL;// = 1000 * 60 * 60 * 24; // 1 day

    AutoPtr<ICountry> mCountry;
//    AutoPtr<ITelephonyManager> mTelephonyManager;
    AutoPtr<ICountry> mCountryFromLocation;
    Boolean mStopped;// = false;

//    AutoPtr<IPhoneStateListener> mPhoneStateListener;

    /**
     * List of the most recent country state changes for debugging. This should have
     * a max length of MAX_LENGTH_LOGS.
     */
//    private final ConcurrentLinkedQueue<Country> mDebugLogs = new ConcurrentLinkedQueue<Country>();
     List<AutoPtr<ICountry> > mDebugLogs;

    /**
     * Most recent {@link Country} result that was added to the debug logs {@link #mDebugLogs}.
     * We keep track of this value to help prevent adding many of the same {@link Country} objects
     * to the logs.
     */
    AutoPtr<ICountry> mLastCountryAddedToLogs;

    /**
     * Object used to synchronize access to {@link #mLastCountryAddedToLogs}. Be careful if
     * using it to synchronize anything else in this file.
     */
    //private final Object mObject = new Object();
    Object mObject;

    /**
     * Start time of the current session for which the detector has been active.
     */
    Int64 mStartTime;

    /**
     * Stop time of the most recent session for which the detector was active.
     */
    Int64 mStopTime;

    /**
     * The sum of all the time intervals in which the detector was active.
     */
    Int64 mTotalTime;

    /**
     * Number of {@link PhoneStateListener#onServiceStateChanged(ServiceState state)} events that
     * have occurred for the current session for which the detector has been active.
     */
    Int32 mCountServiceStateChanges;

    /**
     * Total number of {@link PhoneStateListener#onServiceStateChanged(ServiceState state)} events
     * that have occurred for all time intervals in which the detector has been active.
     */
    Int32 mTotalCountServiceStateChanges;

    /**
     * The listener for receiving the notification from LocationBasedCountryDetector.
     */
    AutoPtr<ICountryListener> mLocationBasedCountryDetectionListener;

    Object mLock;

};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_COMPREHENSIVECOUNTRYDETECTOR_H__
