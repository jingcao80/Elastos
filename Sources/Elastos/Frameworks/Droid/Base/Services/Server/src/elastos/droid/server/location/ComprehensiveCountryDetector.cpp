
#include "location/ComprehensiveCountryDetector.h"
#include "location/LocationBasedCountryDetector.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/TimerTask.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Utility::ITimerTask;
using Elastos::Utility::CTimer;
using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Location::ICountry;
using Elastos::Droid::Location::CCountry;
using Elastos::Droid::Location::IGeocoder;
using Elastos::Droid::Location::CGeocoder;
using Elastos::Droid::Location::IGeocoderHelper;
using Elastos::Droid::Location::CGeocoderHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Utility::ILocale;
using Elastos::Utility::CLocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Content::IContentResolver;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

const String ComprehensiveCountryDetector::TAG("CountryDetector");
/* package */ const Boolean ComprehensiveCountryDetector::DEBUG = FALSE;
const Int32 ComprehensiveCountryDetector::MAX_LENGTH_DEBUG_LOGS = 20;
const Int64 ComprehensiveCountryDetector::LOCATION_REFRESH_INTERVAL = 1000LL * 60LL * 60LL * 24LL; // 1 day


ComprehensiveCountryDetector::MyTimerTask::MyTimerTask(
    /* [in] */ ComprehensiveCountryDetector* host)
    : mHost(host)
{}

ECode ComprehensiveCountryDetector::MyTimerTask::Run()
{
    mHost->mLocationRefreshTimer = NULL;
    mHost->DetectCountry(FALSE, TRUE);
    return NOERROR;
}

ComprehensiveCountryDetector::MyRunnable::MyRunnable(
    /* [in] */ ComprehensiveCountryDetector* host,
    /* [in] */ ICountry* country,
    /* [in] */ ICountry* detectedCountry,
    /* [in] */ Boolean notifyChange,
    /* [in] */ Boolean startLocationBasedDetection)
    : mHost(host)
    , mCountry(country)
    , mDetectedCountry(detectedCountry)
    , mNotifyChange(notifyChange)
    , mStartLocationBasedDetection(startLocationBasedDetection)
{
}

ECode ComprehensiveCountryDetector::MyRunnable::Run()
{
    mHost->RunAfterDetection(mCountry, mDetectedCountry, mNotifyChange, mStartLocationBasedDetection);
    return NOERROR;
}

ComprehensiveCountryDetector::ComprehensiveCountryDetector(
    /* [in] */ IContext* context)
    : CountryDetectorBase(context)
    , mStopped(FALSE)
{
//     context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&mTelephonyManager);
}

//@Override
AutoPtr<ICountry> ComprehensiveCountryDetector::DetectCountry()
{
    return DetectCountry(FALSE, !mStopped);
}

//@Override
void ComprehensiveCountryDetector::Stop()
{
    // Note: this method in this subclass called only by tests.
//    Slog.i(TAG, "Stop the detector.");
    CancelLocationRefresh();
    RemovePhoneStateListener();
    StopLocationBasedDetector();
    mListener = NULL;
    mStopped = TRUE;
}

//@Override
void ComprehensiveCountryDetector::SetCountryListener(
    /* [in] */ ICountryListener* listener)
{
    AutoPtr<ICountryListener> prevListener = mListener;
    mListener = listener;
    if (mListener == NULL) {
        // Stop listening all services
        RemovePhoneStateListener();
        StopLocationBasedDetector();
        CancelLocationRefresh();
        mStopTime = SystemClock::GetElapsedRealtime();
        mTotalTime += mStopTime;
    } else if (prevListener == NULL) {
        AddPhoneStateListener();
        DetectCountry(FALSE, TRUE);
        mStartTime = SystemClock::GetElapsedRealtime();
        mStopTime = 0;
        mCountServiceStateChanges = 0;
    }
}

void ComprehensiveCountryDetector::RunAfterDetection(
    /* [in] */ ICountry* country,
    /* [in] */ ICountry* detectedCountry,
    /* [in] */ const Boolean notifyChange,
    /* [in] */ const Boolean startLocationBasedDetection)
{
    assert(country != NULL);
    assert(detectedCountry != NULL);

    if (notifyChange) {
        NotifyIfCountryChanged(country, detectedCountry);
    }
/*    if (DEBUG) {
        Slog.d(TAG, "startLocationBasedDetection=" + startLocationBasedDetection
                + " detectCountry=" + (detectedCountry == null ? null :
                    "(source: " + detectedCountry.getSource()
                    + ", countryISO: " + detectedCountry.getCountryIso() + ")")
                + " isAirplaneModeOff()=" + isAirplaneModeOff()
                + " mListener=" + mListener
                + " isGeoCoderImplemnted()=" + isGeoCoderImplemented());
    }*/

    Int32 dcSource = 0;
    detectedCountry->GetSource(&dcSource);

    if (startLocationBasedDetection && (detectedCountry == NULL
            || dcSource > ICountry::COUNTRY_SOURCE_LOCATION)
            && IsAirplaneModeOff() && mListener != NULL && IsGeoCoderImplemented()) {
//        if (DEBUG) Slog.d(TAG, "run startLocationBasedDetector()");
        // Start finding location when the source is less reliable than the
        // location and the airplane mode is off (as geocoder will not
        // work).
        // TODO : Shall we give up starting the detector within a
        // period of time?
        StartLocationBasedDetector(mLocationBasedCountryDetectionListener);
    }

    if (detectedCountry == NULL
            || dcSource >= ICountry::COUNTRY_SOURCE_LOCATION) {
        // Schedule the location refresh if the country source is
        // not more reliable than the location or no country is
        // found.
        // TODO: Listen to the preference change of GPS, Wifi etc,
        // and start detecting the country.
        ScheduleLocationRefresh();
    } else {
        // Cancel the location refresh once the current source is
        // more reliable than the location.
        CancelLocationRefresh();
        StopLocationBasedDetector();
    }
}

//@Override
String ComprehensiveCountryDetector::ToString()
{
    Int64 currentTime = SystemClock::GetElapsedRealtime();
    Int64 currentSessionLength = 0;
    StringBuilder sb;
    sb += "ComprehensiveCountryDetector{";
    // The detector hasn't stopped yet --> still running
    if (mStopTime == 0) {
        currentSessionLength = currentTime - mStartTime;
        sb += "timeRunning=";
        sb += currentSessionLength;
        sb += ", ";
    } else {
        // Otherwise, it has already stopped, so take the last session
        sb += "lastRunTimeLength=";
        sb += mStopTime - mStartTime;
        sb += ", ";
    }
    sb += "totalCountServiceStateChanges=";
    sb += mTotalCountServiceStateChanges;
    sb += ", ";
    sb += "currentCountServiceStateChanges=";
    sb += + mCountServiceStateChanges;
    sb += ", ";
    sb += "totalTime=";
    sb += mTotalTime + currentSessionLength;
    sb += ", ";
    sb += "currentTime=";
    sb += currentTime;
    sb += ", ";
    sb += "countries=";

    List<AutoPtr<ICountry> >::Iterator it = mDebugLogs.Begin();
    for (; it != mDebugLogs.End(); it ++) {
        AutoPtr<ICountry> country = *it;
        String str;
        country->ToString(&str);
        sb += str;
    }
    // for (Country country : mDebugLogs) {
    //     sb.append("\n   " + country.toString());
    // }
    // sb.append("}");
    return sb.ToString();
}

/**
 * @return the country from the mobile network.
 */
AutoPtr<ICountry> ComprehensiveCountryDetector::GetNetworkBasedCountry()
{
    AutoPtr<ICharSequence> cs;
    if (IsNetworkCountryCodeAvailable()) {
//        mTelephonyManager->GetNetworkCountryIso(&cs);
        if (!TextUtils::IsEmpty(cs)) {
            AutoPtr<ICountry> country;
            String countryIso;
            cs->ToString(&countryIso);
            CCountry::New(countryIso, ICountry::COUNTRY_SOURCE_NETWORK, (ICountry**)&country);
            return country;
        }
    }

    return NULL;
}

/**
 * @return the cached location based country.
 */
AutoPtr<ICountry> ComprehensiveCountryDetector::GetLastKnownLocationBasedCountry()
{
    return mCountryFromLocation;
}

/**
 * @return the country from SIM card
 */
AutoPtr<ICountry> ComprehensiveCountryDetector::GetSimBasedCountry()
{
    String countryIso;
    AutoPtr<ICharSequence> cs;
//    mTelephonyManager->GetSimCountryIso(&cs);
    if (!TextUtils::IsEmpty(cs)) {
        AutoPtr<ICountry> country;
        cs->ToString(&countryIso);
        CCountry::New(countryIso, ICountry::COUNTRY_SOURCE_SIM, (ICountry**)&country);
    }

    return NULL;
}

/**
 * @return the country from the system's locale.
 */
AutoPtr<ICountry> ComprehensiveCountryDetector::GetLocaleCountry()
{
    AutoPtr<ILocaleHelper> localeHelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper);

    AutoPtr<ILocale> defaultLocale;
    localeHelper->GetDefault((ILocale**)&defaultLocale);
    if (defaultLocale != NULL) {
        String countryStr;
        AutoPtr<ICountry> countryRet;
        defaultLocale->GetCountry(&countryStr);
        CCountry::New(countryStr, ICountry::COUNTRY_SOURCE_LOCALE, (ICountry**)&countryRet);
        return countryRet;
    } else {
        return NULL;
    }
}

/**
 * Run the tasks in the service's thread.
 */
void ComprehensiveCountryDetector::RunAfterDetectionAsync(
    /* [in] */ ICountry* country,
    /* [in] */ ICountry* detectedCountry,
    /* [in] */ Boolean notifyChange,
    /* [in] */ Boolean startLocationBasedDetection)
{
    AutoPtr<IRunnable> runnable = new MyRunnable(this, country, detectedCountry, notifyChange, startLocationBasedDetection);
    Boolean rst;
    mHandler->Post(runnable, &rst);
}

AutoPtr<CountryDetectorBase> ComprehensiveCountryDetector::CreateLocationBasedCountryDetector()
{
    AutoPtr<CountryDetectorBase> base = new LocationBasedCountryDetector(mContext);
    return base;
}

Boolean ComprehensiveCountryDetector::IsAirplaneModeOff()
{
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    Int32 rst;
    settingsGlobal->GetInt32(cr, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &rst);

    return rst == 0;
}

/* synchronized */
void ComprehensiveCountryDetector::AddPhoneStateListener()
{
    AutoLock lock(mLock);

    /*
    if (mPhoneStateListener == null) {
        mPhoneStateListener = new PhoneStateListener() {
            @Override
            public void onServiceStateChanged(ServiceState serviceState) {
                mCountServiceStateChanges++;
                mTotalCountServiceStateChanges++;

                if (!isNetworkCountryCodeAvailable()) {
                    return;
                }
                if (DEBUG) Slog.d(TAG, "onServiceStateChanged: " + serviceState.getState());

                detectCountry(true, true);
            }
        };
        mTelephonyManager.listen(mPhoneStateListener, PhoneStateListener.LISTEN_SERVICE_STATE);
    }*/
}

/* synchronized */
void ComprehensiveCountryDetector::RemovePhoneStateListener()
{
    AutoLock lock(mLock);
#if 0
    if (mPhoneStateListener != NULL) {
        mTelephonyManager->Listen(mPhoneStateListener, IPhoneStateListener::LISTEN_NONE);
        mPhoneStateListener = NULL;
    }
#endif
}

Boolean ComprehensiveCountryDetector::IsGeoCoderImplemented()
{
    Boolean rst;
    AutoPtr<IGeocoderHelper> helper;
    CGeocoderHelper::AcquireSingleton((IGeocoderHelper**)&helper);
    helper->IsPresent(&rst);
    return rst;
}

/**
 * Get the country from different sources in order of the reliability.
 */
AutoPtr<ICountry> ComprehensiveCountryDetector::GetCountry()
{
    AutoPtr<ICountry> result;
    result = GetNetworkBasedCountry();
    if (result == NULL) {
        result = GetLastKnownLocationBasedCountry();
    }
    if (result == NULL) {
        result = GetSimBasedCountry();
    }
    if (result == NULL) {
        result = GetLocaleCountry();
    }
    AddToLogs(result);
    return result;
}

/**
 * Attempt to add this {@link Country} to the debug logs.
 */
void ComprehensiveCountryDetector::AddToLogs(
    /* [in] */ ICountry* country)
{
    if (country == NULL) {
        return;
    }
    // If the country (ISO and source) are the same as before, then there is no
    // need to add this country as another entry in the logs. Synchronize access to this
    // variable since multiple threads could be calling this method.
    {
        AutoLock lock(mObject);
        Boolean equal = FALSE;
        if (mLastCountryAddedToLogs != NULL) {
            AutoPtr<IObject> countryObj = IObject::Probe(mLastCountryAddedToLogs);
            if (countryObj != NULL)
                countryObj->Equals(country, &equal);
            if (equal) return;
        }
        mLastCountryAddedToLogs = country;
    }
    // Manually maintain a max limit for the list of logs
    if (mDebugLogs.GetSize() >= MAX_LENGTH_DEBUG_LOGS) {
        mDebugLogs.PopFront();
    }
    // if (DEBUG) {
    //     Slog.d(TAG, country.toString());
    // }
    mDebugLogs.PushBack(country);
}

Boolean ComprehensiveCountryDetector::IsNetworkCountryCodeAvailable()
{
    // On CDMA TelephonyManager.getNetworkCountryIso() just returns SIM country.  We don't want
    // to prioritize it over location based country, so ignore it.
    Int32 phoneType = 0;
//    mTelephonyManager->GetPhoneType(&phoneType);
//    if (DEBUG) Slog.v(TAG, "    phonetype=" + phoneType);
//    return phoneType == ITelephonyManager::PHONE_TYPE_GSM;

    return FALSE;//temp
}

/**
 * @param notifyChange indicates whether the listener should be notified the change of the
 * country
 * @param startLocationBasedDetection indicates whether the LocationBasedCountryDetector could
 * be started if the current country source is less reliable than the location.
 * @return the current available UserCountry
 */
AutoPtr<ICountry> ComprehensiveCountryDetector::DetectCountry(
    /* [in] */ Boolean notifyChange,
    /* [in] */ Boolean startLocationBasedDetection)
{
    AutoPtr<ICountry> country = GetCountry();
    AutoPtr<ICountry> countryTemp;

    if (mCountry != NULL) {
        CCountry::New(mCountry, (ICountry**)&countryTemp);
    } else {
        countryTemp = mCountry;
    }

    RunAfterDetectionAsync(countryTemp, country,
            notifyChange, startLocationBasedDetection);
    mCountry = country;
    return mCountry;
}

/**
 * Find the country from LocationProvider.
 */
/* synchronized */
void ComprehensiveCountryDetector::StartLocationBasedDetector(
    /* [in] */ ICountryListener* listener)
{
    AutoLock lock(mLock);

    if (mLocationBasedCountryDetector != NULL) {
        return;
    }
//    if (DEBUG) {
//        Slog.d(TAG, "starts LocationBasedDetector to detect Country code via Location info "
//                + "(e.g. GPS)");
//    }
    mLocationBasedCountryDetector = CreateLocationBasedCountryDetector();
    mLocationBasedCountryDetector->SetCountryListener(listener);
    AutoPtr<ICountry> country;
    mLocationBasedCountryDetector->DetectCountry((ICountry**)&country);
}

/* synchronized */
void ComprehensiveCountryDetector::StopLocationBasedDetector()
{
    AutoLock lock(mLock);

//    if (DEBUG) {
//        Slog.d(TAG, "tries to stop LocationBasedDetector "
//                + "(current detector: " + mLocationBasedCountryDetector + ")");
//    }
    if (mLocationBasedCountryDetector != NULL) {
        mLocationBasedCountryDetector->Stop();
        mLocationBasedCountryDetector = NULL;
    }
}

/**
 * Notify the country change.
 */
void ComprehensiveCountryDetector::NotifyIfCountryChanged(
    /* [in] */ ICountry* country,
    /* [in] */ ICountry* detectedCountry)
{
    Boolean tmp = FALSE;
    if (country == NULL) {
        tmp = TRUE;
    } else {
        Boolean equal;
        AutoPtr<IObject> cObj = IObject::Probe(country);
        if (cObj->Equals(detectedCountry, &equal), !equal) {
            tmp = TRUE;
        }
    }
    if (detectedCountry != NULL && mListener != NULL && tmp) {
//        if (DEBUG) {
//            Slog.d(TAG, "" + country + " --> " + detectedCountry);
//        }
        NotifyListener(detectedCountry);
    }
}

/**
 * Schedule the next location refresh. We will do nothing if the scheduled task exists.
 */
/* synchronized */
void ComprehensiveCountryDetector::ScheduleLocationRefresh()
{
    AutoLock lock(mLock);
    if (mLocationRefreshTimer != NULL) return;
    // if (DEBUG) {
    //     Slog.d(TAG, "start periodic location refresh timer. Interval: "
    //             + LOCATION_REFRESH_INTERVAL);
    // }
    CTimer::New((ITimer**)&mLocationRefreshTimer);
    AutoPtr<ITimerTask> timerTask = new MyTimerTask(this);

    mLocationRefreshTimer->Schedule(timerTask, LOCATION_REFRESH_INTERVAL);
}

/**
 * Cancel the scheduled refresh task if it exists
 */
/* synchronized */
void ComprehensiveCountryDetector::CancelLocationRefresh()
{
    AutoLock lock(mLock);
    if (mLocationRefreshTimer != NULL) {
        mLocationRefreshTimer->Cancel();
        mLocationRefreshTimer = NULL;
    }
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
