#include "elastos/droid/server/location/ComprehensiveCountryDetector.h"
#include "elastos/droid/server/location/LocationBasedCountryDetector.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Location::CCountry;
using Elastos::Droid::Location::CGeocoderHelper;
using Elastos::Droid::Location::EIID_ICountryListener;
using Elastos::Droid::Location::IGeocoderHelper;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Concurrent::CConcurrentLinkedQueue;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::CTimer;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::IQueue;
using Elastos::Utility::ITimer;
using Elastos::Utility::ITimerTask;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Location {

//=====================================
// ComprehensiveCountryDetector::MyCountryListener
//=====================================

CAR_INTERFACE_IMPL(ComprehensiveCountryDetector::MyCountryListener, Object, ICountryListener)

ComprehensiveCountryDetector::MyCountryListener::MyCountryListener(
    /* [in] */ ComprehensiveCountryDetector* host)
    : mHost(host)
{}

ECode ComprehensiveCountryDetector::MyCountryListener::OnCountryDetected(
    /* [in] */ ICountry* country)
{
    if (ComprehensiveCountryDetector::DEBUG) {
        Slogger::D(ComprehensiveCountryDetector::TAG, "Country detected via LocationBasedCountryDetector");
    }
    mHost->mCountryFromLocation = country;
    // Don't start the LocationBasedCountryDetector.
    mHost->DetectCountry(TRUE, FALSE);
    mHost->StopLocationBasedDetector();
    return NOERROR;
}

//=====================================
// ComprehensiveCountryDetector
//=====================================

const String ComprehensiveCountryDetector::TAG("ComprehensiveCountryDetector");
const Boolean ComprehensiveCountryDetector::DEBUG = TRUE;
const Int32 ComprehensiveCountryDetector::MAX_LENGTH_DEBUG_LOGS = 20;
const Int64 ComprehensiveCountryDetector::LOCATION_REFRESH_INTERVAL = 1000 * 60 * 60 * 24; // 1 day

ComprehensiveCountryDetector::ComprehensiveCountryDetector(
    /* [in] */ IContext* context)
    : CountryDetectorBase(context)
    , mStopped(FALSE)
    , mStartTime(0L)
    , mStopTime(0L)
    , mTotalTime(0L)
    , mCountServiceStateChanges(0)
    , mTotalCountServiceStateChanges(0)
{
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::TELEPHONY_SERVICE, (IInterface**)&obj);
    mTelephonyManager = ITelephonyManager::Probe(obj);
    AutoPtr<MyCountryListener> cl = new MyCountryListener(this);
    mLocationBasedCountryDetectionListener = (ICountryListener*)cl.Get();
    CConcurrentLinkedQueue::New((IConcurrentLinkedQueue**)&mDebugLogs);
}

ECode ComprehensiveCountryDetector::DetectCountry(
    /* [out] */ ICountry** country)
{
    VALIDATE_NOT_NULL(country)
    // Don't start the LocationBasedCountryDetector if we have been stopped.
    AutoPtr<ICountry> temp = DetectCountry(FALSE, !mStopped);
    *country = temp;
    REFCOUNT_ADD(*country)
    return NOERROR;
}

ECode ComprehensiveCountryDetector::Stop()
{
    // Note: this method in this subclass called only by tests.
    Slogger::I(TAG, "Stop the detector.");
    CancelLocationRefresh();
    RemovePhoneStateListener();
    StopLocationBasedDetector();
    mListener = NULL;
    mStopped = TRUE;
    return NOERROR;
}

AutoPtr<ICountry> ComprehensiveCountryDetector::GetCountry()
{
    AutoPtr<ICountry> result;
    GetNetworkBasedCountry((ICountry**)&result);
    if (result == NULL) {
        GetLastKnownLocationBasedCountry((ICountry**)&result);
    }
    if (result == NULL) {
        GetSimBasedCountry((ICountry**)&result);
    }
    if (result == NULL) {
        GetLocaleCountry((ICountry**)&result);
    }
    AddToLogs(result);
    return result;
}

ECode ComprehensiveCountryDetector::AddToLogs(
    /* [in] */ ICountry* country)
{
    if (country == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    // If the country (ISO and source) are the same as before, then there is no
    // need to add this country as another entry in the logs. Synchronize access to this
    // variable since multiple threads could be calling this method.
    {
        AutoLock syncLock(this);
        if (mLastCountryAddedToLogs != NULL && Object::Equals(mLastCountryAddedToLogs, country)) {
            return E_NULL_POINTER_EXCEPTION;
        }
        mLastCountryAddedToLogs = country;
    }
    // Manually maintain a max limit for the list of logs
    Int32 size;
    ICollection::Probe(mDebugLogs)->GetSize(&size);
    if (size >= MAX_LENGTH_DEBUG_LOGS) {
        AutoPtr<IInterface> obj;
        IQueue::Probe(mDebugLogs)->Poll((IInterface**)&obj);
    }
    if (DEBUG) {
        Slogger::D(TAG, TO_CSTR(country));
    }
    ICollection::Probe(mDebugLogs)->Add(country);
    return NOERROR;
}

Boolean ComprehensiveCountryDetector::IsNetworkCountryCodeAvailable()
{
    // On CDMA TelephonyManager.getNetworkCountryIso() just returns SIM country.  We don't want
    // to prioritize it over location based country, so ignore it.
    Int32 phoneType;
    mTelephonyManager->GetPhoneType(&phoneType);
    if (DEBUG) Slogger::V(TAG, "    phonetype=%d", phoneType);
    return phoneType == ITelephonyManager::PHONE_TYPE_GSM;
}

ECode ComprehensiveCountryDetector::GetNetworkBasedCountry(
    /* [out] */ ICountry** country)
{
    VALIDATE_NOT_NULL(country)
    *country = NULL;

    // String countryIso;
    if (IsNetworkCountryCodeAvailable()) {
        String countryIso;
        mTelephonyManager->GetNetworkCountryIso(&countryIso);
        if (!TextUtils::IsEmpty(countryIso)) {
            CCountry::New(countryIso, ICountry::COUNTRY_SOURCE_NETWORK, country);
            return NOERROR;
        }
    }
    return NOERROR;
}

ECode ComprehensiveCountryDetector::GetLastKnownLocationBasedCountry(
    /* [out] */ ICountry** country)
{
    VALIDATE_NOT_NULL(country)
    *country = mCountryFromLocation;
    REFCOUNT_ADD(*country)
    return NOERROR;
}

ECode ComprehensiveCountryDetector::GetSimBasedCountry(
    /* [out] */ ICountry** country)
{
    VALIDATE_NOT_NULL(country)
    *country = NULL;
    String countryIso;
    mTelephonyManager->GetSimCountryIso(&countryIso);
    if (!TextUtils::IsEmpty(countryIso)) {
        return CCountry::New(countryIso, ICountry::COUNTRY_SOURCE_SIM, country);
    }
    return NOERROR;
}

ECode ComprehensiveCountryDetector::GetLocaleCountry(
    /* [out] */ ICountry** country)
{
    AutoPtr<ILocaleHelper> lh;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lh);
    AutoPtr<ILocale> defaultLocale;
    lh->GetDefault((ILocale**)&defaultLocale);
    if (defaultLocale != NULL) {
        String countrystr;
        defaultLocale->GetCountry(&countrystr);
        CCountry::New(countrystr, ICountry::COUNTRY_SOURCE_LOCALE, country);
        return NOERROR;
    }
    else {
        *country = NULL;
        return NOERROR;
    }
}

AutoPtr<ICountry> ComprehensiveCountryDetector::DetectCountry(
    /* [in] */ Boolean notifyChange,
    /* [in] */ Boolean startLocationBasedDetection)
{
    AutoPtr<ICountry> country = GetCountry();
    AutoPtr<ICountry> cy1;
    CCountry::New(mCountry, (ICountry**)&cy1);
    RunAfterDetectionAsync(
        mCountry != NULL ? cy1 : mCountry, country,
        notifyChange, startLocationBasedDetection);
    mCountry = country;
    return mCountry;
}

ECode ComprehensiveCountryDetector::RunAfterDetectionAsync(
    /* [in] */ ICountry* country,
    /* [in] */ ICountry* detectedCountry,
    /* [in] */ Boolean notifyChange,
    /* [in] */ Boolean startLocationBasedDetection)
{
    AutoPtr<MyRunnable> mr = new MyRunnable(
        this, country, detectedCountry, notifyChange, startLocationBasedDetection);
    Boolean result;
    return mHandler->Post((IRunnable*)mr.Get(), &result);
}

ECode ComprehensiveCountryDetector::SetCountryListener(
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
    }
    else if (prevListener == NULL) {
        AddPhoneStateListener();
        DetectCountry(FALSE, TRUE);
        mStartTime = SystemClock::GetElapsedRealtime();
        mStopTime = 0;
        mCountServiceStateChanges = 0;
    }
    return NOERROR;
}

ECode ComprehensiveCountryDetector::RunAfterDetection(
    /* [in] */ ICountry* country,
    /* [in] */ ICountry* detectedCountry,
    /* [in] */ Boolean notifyChange,
    /* [in] */ Boolean startLocationBasedDetection)
{
    if (notifyChange) {
        NotifyIfCountryChanged(country, detectedCountry);
    }

    Int32 source;
    detectedCountry->GetSource(&source);
    String ciso;
    detectedCountry->GetCountryIso(&ciso);
    Boolean isAirplaneModeOff;
    IsAirplaneModeOff(&isAirplaneModeOff);
    Boolean isGeoCoderImplemented;
    IsGeoCoderImplemented(&isGeoCoderImplemented);

    if (DEBUG) {
        StringBuilder sb("startLocationBasedDetection=");
        sb += startLocationBasedDetection;
        sb += " detectCountry=";
        if (detectedCountry == NULL) {
            sb += (ICountry*)NULL;
        }
        else {
            sb += "(source: ";
            sb += source;
            sb += ", countryISO: ";
            sb += ciso;
            sb += ")";
        }
        sb += " isAirplaneModeOff()=";
        sb += isAirplaneModeOff;
        sb += " mListener=";
        sb += mListener;
        sb += " isGeoCoderImplemented()=";
        sb += isGeoCoderImplemented;
        Slogger::D(TAG, sb.ToString().string());
    }

    if (startLocationBasedDetection && (detectedCountry == NULL
        || source > ICountry::COUNTRY_SOURCE_LOCATION)
        && isAirplaneModeOff && mListener != NULL && isGeoCoderImplemented) {
        if (DEBUG) Slogger::D(TAG, "run startLocationBasedDetector()");
        // Start finding location when the source is less reliable than the
        // location and the airplane mode is off (as geocoder will not
        // work).
        // TODO : Shall we give up starting the detector within a
        // period of time?
        StartLocationBasedDetector(mLocationBasedCountryDetectionListener);
    }
    if (detectedCountry == NULL
            || source >= ICountry::COUNTRY_SOURCE_LOCATION) {
        // Schedule the location refresh if the country source is
        // not more reliable than the location or no country is
        // found.
        // TODO: Listen to the preference change of GPS, Wifi etc,
        // and start detecting the country.
        ScheduleLocationRefresh();
    }
    else {
        // Cancel the location refresh once the current source is
        // more reliable than the location.
        CancelLocationRefresh();
        StopLocationBasedDetector();
    }
    return NOERROR;
}

ECode ComprehensiveCountryDetector::StartLocationBasedDetector(
    /* [in] */ ICountryListener* listener)
{
    AutoLock syncLock(this);
    if (mLocationBasedCountryDetector != NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (DEBUG) {
        Slogger::D(TAG, "starts LocationBasedDetector to detect Country code via Location info (e.g. GPS)");
    }
    CreateLocationBasedCountryDetector((CountryDetectorBase**)&mLocationBasedCountryDetector);
    mLocationBasedCountryDetector->SetCountryListener(listener);
    AutoPtr<ICountry> c;
    mLocationBasedCountryDetector->DetectCountry((ICountry**)&c);
    return NOERROR;
}

void ComprehensiveCountryDetector::StopLocationBasedDetector()
{
    if (DEBUG) {
        Slogger::D(TAG, "tries to stop LocationBasedDetector (current detector: %s)",
            TO_CSTR(mLocationBasedCountryDetector));
    }
    if (mLocationBasedCountryDetector != NULL) {
        mLocationBasedCountryDetector->Stop();
        mLocationBasedCountryDetector = NULL;
    }
}

ECode ComprehensiveCountryDetector::CreateLocationBasedCountryDetector(
    /* [out] */ CountryDetectorBase** db)
{
    VALIDATE_NOT_NULL(db)
    {
        AutoLock syncLock(this);
        *db = new LocationBasedCountryDetector(mContext);
        REFCOUNT_ADD(*db)
    }
    return NOERROR;
}

ECode ComprehensiveCountryDetector::IsAirplaneModeOff(
    /* [out] */ Boolean* isAirplaneModeOff)
{
    VALIDATE_NOT_NULL(isAirplaneModeOff)
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ISettingsGlobal> sg;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
    Int32 v;
    sg->GetInt32(resolver.Get(), ISettingsGlobal::AIRPLANE_MODE_ON, 0, &v);
    *isAirplaneModeOff = v == 0;
    return NOERROR;
}

void ComprehensiveCountryDetector::NotifyIfCountryChanged(
    /* [in] */ ICountry* country,
    /* [in] */ ICountry* detectedCountry)
{
    Boolean equals;
    IObject::Probe(country)->Equals(detectedCountry, &equals);
    if (detectedCountry != NULL && mListener != NULL && (country == NULL || !equals)) {
        if (DEBUG) {
            String str1, str2;
            IObject::Probe(country)->ToString(&str1);
            IObject::Probe(country)->ToString(&str2);
            Slogger::D(TAG, "%s --> %s", str1.string(), str2.string());
        }
        CountryDetectorBase::NotifyListener(detectedCountry);
    }
}

ECode ComprehensiveCountryDetector::ScheduleLocationRefresh()
{
    AutoLock syncLock(this);
    if (mLocationRefreshTimer != NULL) return E_NULL_POINTER_EXCEPTION;
    if (DEBUG) {
        Slogger::D(TAG, "start periodic location refresh timer. Interval: %lld", LOCATION_REFRESH_INTERVAL);
    }
    CTimer::New((ITimer**)&mLocationRefreshTimer);
    AutoPtr<MyTimeTask> tt = new MyTimeTask(this);
    mLocationRefreshTimer->Schedule((ITimerTask*)tt.Get(), LOCATION_REFRESH_INTERVAL);
    return NOERROR;
}

void ComprehensiveCountryDetector::CancelLocationRefresh()
{
    AutoLock syncLock(this);
    if (mLocationRefreshTimer != NULL) {
        mLocationRefreshTimer->Cancel();
        mLocationRefreshTimer = NULL;
    }
}

void ComprehensiveCountryDetector::AddPhoneStateListener()
{
    AutoLock syncLock(this);
    if (mPhoneStateListener == NULL) {
        AutoPtr<MyPhoneStateListener> psl = new MyPhoneStateListener(this);
        mPhoneStateListener = (IPhoneStateListener*)psl.Get();
        mTelephonyManager->Listen(mPhoneStateListener.Get(), IPhoneStateListener::LISTEN_SERVICE_STATE);
    }
}

ECode ComprehensiveCountryDetector::RemovePhoneStateListener()
{
    AutoLock syncLock(this);
    if (mPhoneStateListener != NULL) {
        mTelephonyManager->Listen(mPhoneStateListener.Get(), IPhoneStateListener::LISTEN_NONE);
        mPhoneStateListener = NULL;
    }
    return NOERROR;
}

ECode ComprehensiveCountryDetector::IsGeoCoderImplemented(
    /* [out] */ Boolean* isGeoCoderImplemented)
{
    VALIDATE_NOT_NULL(isGeoCoderImplemented)
    {    AutoLock syncLock(this);
        AutoPtr<IGeocoderHelper> gh;
        CGeocoderHelper::AcquireSingleton((IGeocoderHelper**)&gh);
        gh->IsPresent(isGeoCoderImplemented);
    }
    return NOERROR;
}

ECode ComprehensiveCountryDetector::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
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
    sb +=  ", ";

    sb += "currentCountServiceStateChanges=";
    sb += mCountServiceStateChanges;
    sb +=  ", ";

    sb += "totalTime=";
    sb += mTotalTime + currentSessionLength;
    sb +=  ", ";

    sb += "currentTime=";
    sb += currentTime;
    sb +=  ", ";

    sb += "countries=";

    AutoPtr<IIterator> iterator;
    ICollection::Probe(mDebugLogs)->GetIterator((IIterator**)&iterator);
    Boolean hasNext;
    while (iterator->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        iterator->GetNext((IInterface**)&obj);
        AutoPtr<ICountry> country = ICountry::Probe(obj);
        String str;
        IObject::Probe(country)->ToString(&str);
        sb += "\n   ";
        sb += str;
    }
    sb += "}";
    *result = sb.ToString();
    return NOERROR;
}

//=====================================
// ComprehensiveCountryDetector::MyRunnable
//=====================================

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
{}

ECode ComprehensiveCountryDetector::MyRunnable::Run()
{
    return mHost->RunAfterDetection(mCountry, mDetectedCountry, mNotifyChange, mStartLocationBasedDetection);
}

//=====================================
// ComprehensiveCountryDetector::MyTimeTask
//=====================================

ComprehensiveCountryDetector::MyTimeTask::MyTimeTask(
    /* [in] */ ComprehensiveCountryDetector* host)
    : mHost(host)
{}

ECode ComprehensiveCountryDetector::MyTimeTask::Run()
{
    if (DEBUG) {
        Slogger::D(ComprehensiveCountryDetector::TAG, "periodic location refresh event. Starts detecting Country code");
    }
    mHost->mLocationRefreshTimer = NULL;
    mHost->DetectCountry(FALSE, TRUE);
    return NOERROR;
}

//=====================================
// ComprehensiveCountryDetector::MyPhoneStateListener
//=====================================
ComprehensiveCountryDetector::MyPhoneStateListener::MyPhoneStateListener(
    /* [in] */ ComprehensiveCountryDetector* host)
    : mHost(host)
{}

ECode ComprehensiveCountryDetector::MyPhoneStateListener::OnServiceStateChanged(
    /* [in] */ Elastos::Droid::Telephony::IServiceState* serviceState)
{
    mHost->mCountServiceStateChanges++;
    mHost->mTotalCountServiceStateChanges++;

    if (!mHost->IsNetworkCountryCodeAvailable()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (ComprehensiveCountryDetector::DEBUG) {
        Int32 state;
        serviceState->GetState(&state);
        Slogger::D(ComprehensiveCountryDetector::TAG, "onServiceStateChanged: %d", state);
    }

    mHost->DetectCountry(TRUE, TRUE);
    return NOERROR;
}

} // namespace Location
} // namespace Server
} // namespace Droid
} // namespace Elastos
