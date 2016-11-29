
#ifndef __ELASTOS_DROID_SERVER_LOCATION_COMPREHENSIVECOUNTRYDETECTOR_H__
#define __ELASTOS_DROID_SERVER_LOCATION_COMPREHENSIVECOUNTRYDETECTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/server/location/CountryDetectorBase.h"
#include "elastos/droid/telephony/PhoneStateListener.h"
#include "elastos/utility/TimerTask.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Telephony.h"

using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::ICountryListener;
using Elastos::Core::Runnable;
using Elastos::Droid::Telephony::PhoneStateListener;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Utility::Concurrent::IConcurrentLinkedQueue;
using Elastos::Utility::ITimer;
using Elastos::Utility::TimerTask;

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
class ComprehensiveCountryDetector
    : public CountryDetectorBase
{
private:
    class MyCountryListener
        : public Object
        , public ICountryListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyCountryListener(
            /* [in] */ ComprehensiveCountryDetector* host);

        //@Override
        CARAPI OnCountryDetected(
            /* [in] */ ICountry* country);

    private:
        ComprehensiveCountryDetector* mHost;
    };

    class MyRunnable
        : public Runnable
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

    class MyTimeTask
        : public TimerTask
    {
    public:
        MyTimeTask(
            /* [in] */ ComprehensiveCountryDetector* host);

        CARAPI Run();

    private:
        ComprehensiveCountryDetector* mHost;
    };

    class MyPhoneStateListener
        : public PhoneStateListener
    {
    public:
        MyPhoneStateListener(
            /* [in] */ ComprehensiveCountryDetector* host);

        CARAPI constructor();

        //@Override
        CARAPI OnServiceStateChanged(
            /* [in] */ Elastos::Droid::Telephony::IServiceState* serviceState);

    private:
        ComprehensiveCountryDetector* mHost;
    };


public:
    ComprehensiveCountryDetector(
        /* [in] */ IContext* context);

    // @Override
    CARAPI DetectCountry(
        /* [out] */ ICountry** country);

    // @Override
    CARAPI Stop();

    // @Override
    CARAPI SetCountryListener(
        /* [in] */ ICountryListener* listener);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

protected:
    /**
     * @return the country from the mobile network.
     */
    CARAPI GetNetworkBasedCountry(
        /* [out] */ ICountry** country);

    /**
     * @return the cached location based country.
     */
    CARAPI GetLastKnownLocationBasedCountry(
        /* [out] */ ICountry** country);

    /**
     * @return the country from SIM card
     */
    CARAPI GetSimBasedCountry(
        /* [out] */ ICountry** country);

    /**
     * @return the country from the system's locale.
     */
    CARAPI GetLocaleCountry(
        /* [out] */ ICountry** country);

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
     * Run the tasks in the service's thread.
     */
    CARAPI RunAfterDetectionAsync(
        /* [in] */ ICountry* country,
        /* [in] */ ICountry* detectedCountry,
        /* [in] */ Boolean notifyChange,
        /* [in] */ Boolean startLocationBasedDetection);

    CARAPI RunAfterDetection(
        /* [in] */ ICountry* country,
        /* [in] */ ICountry* detectedCountry,
        /* [in] */ Boolean notifyChange,
        /* [in] */ Boolean startLocationBasedDetection);

    CARAPI CreateLocationBasedCountryDetector(
        /* [out] */ CountryDetectorBase** db);

    CARAPI IsAirplaneModeOff(
        /* [out] */ Boolean* isAirplaneModeOff);

    CARAPI RemovePhoneStateListener();

    CARAPI IsGeoCoderImplemented(
        /* [out] */ Boolean* isGeoCoderImplemented);

private:
    /**
     * Get the country from different sources in order of the reliability.
     */
    CARAPI_(AutoPtr<ICountry>) GetCountry();

    /**
     * Attempt to add this {@link Country} to the debug logs.
     */
    CARAPI AddToLogs(
        /* [in] */ ICountry* country);

    CARAPI_(Boolean) IsNetworkCountryCodeAvailable();

    /**
     * Find the country from LocationProvider.
     */
    CARAPI StartLocationBasedDetector(
        /* [in] */ ICountryListener* listener);

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
    CARAPI ScheduleLocationRefresh();

    /**
     * Cancel the scheduled refresh task if it exists
     */
    CARAPI_(void) CancelLocationRefresh();

    CARAPI_(void) AddPhoneStateListener();

protected:
    AutoPtr<CountryDetectorBase> mLocationBasedCountryDetector;
    AutoPtr<ITimer> mLocationRefreshTimer;

private:
    const static String TAG;
    /* package */ const static Boolean DEBUG;

    /**
     * Max length of logs to maintain for debugging.
     */
    const static Int32 MAX_LENGTH_DEBUG_LOGS;

    /**
     * The refresh interval when the location based country was used
     */
    const static Int64 LOCATION_REFRESH_INTERVAL; // 1 day

    AutoPtr<ICountry> mCountry;
    AutoPtr<ITelephonyManager> mTelephonyManager;
    AutoPtr<ICountry> mCountryFromLocation;
    Boolean mStopped;

    AutoPtr<IPhoneStateListener> mPhoneStateListener;

    /**
     * List of the most recent country state changes for debugging. This should have
     * a max length of MAX_LENGTH_LOGS.
     */
     AutoPtr<IConcurrentLinkedQueue> mDebugLogs;

    /**
     * Most recent {@link Country} result that was added to the debug logs {@link #mDebugLogs}.
     * We keep track of this value to help prevent adding many of the same {@link Country} objects
     * to the logs.
     */
    AutoPtr<ICountry> mLastCountryAddedToLogs;

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
};

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_LOCATION_COMPREHENSIVECOUNTRYDETECTOR_H__

