#include "NetworkTimeUpdateService.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::EIID_IContentObserver;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Internal::Telephony::ITelephonyIntents;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Utility::INtpTrustedTime;
using Elastos::Droid::Utility::INtpTrustedTimeHelper;
using Elastos::Droid::Utility::CNtpTrustedTimeHelper;
using Elastos::Core::StringBuilder;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::Math;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String NetworkTimeUpdateService::TAG("NetworkTimeUpdateService");
const Boolean NetworkTimeUpdateService::DBG = FALSE;

const Int32 NetworkTimeUpdateService::EVENT_AUTO_TIME_CHANGED;
const Int32 NetworkTimeUpdateService::EVENT_POLL_NETWORK_TIME;
const Int32 NetworkTimeUpdateService::EVENT_NETWORK_CONNECTED;
const Int64 NetworkTimeUpdateService::POLLING_INTERVAL_MS = 24L * 60 * 60 * 1000; // 24 hrs
const Int64 NetworkTimeUpdateService::POLLING_INTERVAL_SHORTER_MS = 60 * 1000L; // 60 seconds
const Int32 NetworkTimeUpdateService::TRY_AGAIN_TIMES_MAX;
const Int32 NetworkTimeUpdateService::TIME_ERROR_THRESHOLD_MS;

const String NetworkTimeUpdateService::ACTION_POLL("com.android.server.NetworkTimeUpdateService.action.POLL");

Int32 NetworkTimeUpdateService::POLL_REQUEST = 0;
const Int64 NetworkTimeUpdateService::NOT_SET = -1;

NetworkTimeUpdateService::MyHandler::MyHandler(
    /* [in] */ ILooper* l,
    /* [in] */ NetworkTimeUpdateService* host)
    : HandlerBase(l)
    , mHost(host)
{}

ECode NetworkTimeUpdateService::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case NetworkTimeUpdateService::EVENT_AUTO_TIME_CHANGED:
        case NetworkTimeUpdateService::EVENT_POLL_NETWORK_TIME:
        case NetworkTimeUpdateService::EVENT_NETWORK_CONNECTED:
            mHost->OnPollNetworkTime(what);
            break;
    }
    return NOERROR;
}

NetworkTimeUpdateService::NetworkTimeUpdateService(
    /* [in] */ IContext *context)
    : mContext(context)
    , mNitzTimeSetTime(NOT_SET)
    , mNitzZoneSetTime(NOT_SET)
    , mLastNtpFetchTime(NOT_SET)
    , mTryAgainCounter(0)
{
    AutoPtr<INtpTrustedTimeHelper> helper;
    CNtpTrustedTimeHelper::AcquireSingleton((INtpTrustedTimeHelper**)&helper);
    helper->GetInstance(context, (INtpTrustedTime**)&mTime);

    mContext->GetSystemService(IContext::ALARM_SERVICE, (IInterface**)&mAlarmManager);

    AutoPtr<IIntent> pollIntent;
    CIntent::New(ACTION_POLL, NULL, (IIntent**)&pollIntent);

    AutoPtr<IPendingIntentHelper> piHelper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&piHelper);
    piHelper->GetBroadcast(mContext, POLL_REQUEST, pollIntent, 0, (IPendingIntent**)&mPendingPollIntent);
}

ECode NetworkTimeUpdateService::SendMessage(
    /* [in] */ Int32 event)
{
    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(event, (IMessage**)&msg);
    Boolean result;
    return mHandler->SendMessage(msg, &result);
}

ECode NetworkTimeUpdateService::SystemReady()
{
    RegisterForTelephonyIntents();
    RegisterForAlarms();
    RegisterForConnectivityIntents();

    CHandlerThread::New(TAG, (IHandlerThread**)&mThread);
    mThread->Start();
    AutoPtr<ILooper> looper;
    mThread->GetLooper((ILooper**)&looper);
    mHandler = new MyHandler(looper, this);

    // Check the network time on the new thread
    SendMessage(EVENT_POLL_NETWORK_TIME);

    mSettingsObserver = new SettingsObserver(mHandler, EVENT_AUTO_TIME_CHANGED, this);
    mSettingsObserver->Observe(mContext);

    return NOERROR;
}

void NetworkTimeUpdateService::RegisterForTelephonyIntents()
{
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(ITelephonyIntents::ACTION_NETWORK_SET_TIME);
    intentFilter->AddAction(ITelephonyIntents::ACTION_NETWORK_SET_TIMEZONE);
    AutoPtr<IIntent> intent;
    mNitzReceiver = new NitzReceiver(this);
    mContext->RegisterReceiver((IBroadcastReceiver*)mNitzReceiver, intentFilter, (IIntent**)&intent);
}

void NetworkTimeUpdateService::OnPollNetworkTime(
    /* [in] */ Int32 event)
{
    using Elastos::Core::Math;

    // If Automatic time is not set, don't bother.
    if (!IsAutomaticTimeRequested()) return;

    Int64 refTime = SystemClock::GetElapsedRealtime();
    // If NITZ time was received less than POLLING_INTERVAL_MS time ago,
    // no need to sync to NTP.
    if (mNitzTimeSetTime != NOT_SET && refTime - mNitzTimeSetTime < POLLING_INTERVAL_MS) {
        ResetAlarm(POLLING_INTERVAL_MS);
        return;
    }


    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 currentTime;
    system->GetCurrentTimeMillis(&currentTime);
    if (DBG) {
        StringBuilder builder("System time = ");
        builder += currentTime;
        Logger::D(TAG, builder.ToString());
    }

    // Get the NTP time
    if (mLastNtpFetchTime == NOT_SET || refTime >= mLastNtpFetchTime + POLLING_INTERVAL_MS
            || event == EVENT_AUTO_TIME_CHANGED) {
        if (DBG)
            Logger::D(TAG, "Before Ntp fetch");

        // force refresh NTP cache when outdated
        Int64 cacheAge = 0;
        mTime->GetCacheAge(&cacheAge);
        if (cacheAge >= POLLING_INTERVAL_MS) {
           Boolean isForceRefreshed;
           mTime->ForceRefresh(&isForceRefreshed);
        }

        // only update when NTP time is fresh
        mTime->GetCacheAge(&cacheAge);
        if (cacheAge < POLLING_INTERVAL_MS) {
            Int64 ntp = 0;
            mTime->GetCurrentTimeMillis(&ntp);
            mTryAgainCounter = 0;
            // If the clock is more than N seconds off or this is the first time it's been
            // fetched since boot, set the current time.
            if (Math::Abs(ntp - currentTime) > TIME_ERROR_THRESHOLD_MS
                    || mLastNtpFetchTime == NOT_SET) {
                // Set the system time
                if (DBG && mLastNtpFetchTime == NOT_SET
                        && Math::Abs(ntp - currentTime) <= TIME_ERROR_THRESHOLD_MS) {
                    StringBuilder builder("For initial setup, rtc = ");
                    builder += currentTime;
                    Logger::D(TAG, builder.ToString());
                }
                if (DBG) {
                    StringBuilder builder("Ntp time to be set = ");
                    builder += ntp;
                    Logger::D(TAG, builder.ToString());
                }
                // Make sure we don't overflow, since it's going to be converted to an int
                Int32 Integer_MAX_VALUE = 0x7FFFFFFF;
                if (ntp / 1000 < Integer_MAX_VALUE) {
                    SystemClock::SetCurrentTimeMillis(ntp);
                }
            } else {
                if (DBG) {
                    StringBuilder builder("Ntp time is close enough = ");
                    builder += ntp;
                    Logger::D(TAG, builder.ToString());
                }
            }
            mLastNtpFetchTime = SystemClock::GetElapsedRealtime();
        } else {
            // Try again shortly
            mTryAgainCounter++;
            if (mTryAgainCounter <= TRY_AGAIN_TIMES_MAX) {
                ResetAlarm(POLLING_INTERVAL_SHORTER_MS);
            } else {
                // Try much later
                mTryAgainCounter = 0;
                ResetAlarm(POLLING_INTERVAL_MS);
            }
            return;
        }
    }
    ResetAlarm(POLLING_INTERVAL_MS);
}

void NetworkTimeUpdateService::RegisterForAlarms()
{
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(ACTION_POLL);
    mAlarmReceiver = new MyReceiver(this);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver(mAlarmReceiver, intentFilter, (IIntent**)&intent);
}

void NetworkTimeUpdateService::RegisterForConnectivityIntents()
{
    AutoPtr<IIntentFilter> intentFilter;
    CIntentFilter::New((IIntentFilter**)&intentFilter);
    intentFilter->AddAction(IConnectivityManager::CONNECTIVITY_ACTION);
    AutoPtr<IIntent> intent;
    mContext->RegisterReceiver((IBroadcastReceiver*)mConnectivityReceiver, intentFilter, (IIntent**)&intent);
}

void NetworkTimeUpdateService::ResetAlarm(
    /* [in] */ Int64 interval)
{
    mAlarmManager->Cancel(mPendingPollIntent);
    Int64 now = SystemClock::GetElapsedRealtime();
    Int64 next = now + interval;
    mAlarmManager->Set(IAlarmManager::ELAPSED_REALTIME, next, mPendingPollIntent);
}

Boolean NetworkTimeUpdateService::IsAutomaticTimeRequested()
{
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    Int32 value = 0;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt32(resolver, ISettingsGlobal::AUTO_TIME, 0, &value);
    return value != 0;
}


//====================================================================
// NetworkTimeUpdateService::NitzReceiver
//====================================================================
NetworkTimeUpdateService::NitzReceiver::NitzReceiver(
    /* [in] */ NetworkTimeUpdateService* owner)
    : mOwner(owner)
{
}

ECode NetworkTimeUpdateService::NitzReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (ITelephonyIntents::ACTION_NETWORK_SET_TIME.Equals(action)) {
        mOwner->mNitzTimeSetTime = SystemClock::GetElapsedRealtime();
    } else if (ITelephonyIntents::ACTION_NETWORK_SET_TIMEZONE.Equals(action)) {
        mOwner->mNitzZoneSetTime = SystemClock::GetElapsedRealtime();
    }
    return NOERROR;
}


//====================================================================
// NetworkTimeUpdateService::ConnectivityReceiver
//====================================================================
NetworkTimeUpdateService::ConnectivityReceiver::ConnectivityReceiver(
    /* [in] */ NetworkTimeUpdateService* owner)
    : mOwner(owner)
{
}

ECode NetworkTimeUpdateService::ConnectivityReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IConnectivityManager::CONNECTIVITY_ACTION.Equals(action)) {
        // There is connectivity
        AutoPtr<IConnectivityManager> connManager;
        context->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&connManager);
        AutoPtr<INetworkInfo> netInfo;
        connManager->GetActiveNetworkInfo((INetworkInfo**)&netInfo);
        if (netInfo != NULL) {
            // Verify that it's a WIFI connection
            Elastos::Droid::Net::NetworkInfoState state;
            netInfo->GetState(&state);
            Int32 type = 0;
            netInfo->GetType(&type);
            if (state == Elastos::Droid::Net::NetworkInfoState_CONNECTED &&
                    (type == IConnectivityManager::TYPE_WIFI ||
                        type == IConnectivityManager::TYPE_ETHERNET) ) {
                mOwner->SendMessage(EVENT_NETWORK_CONNECTED);
                //mHandler.obtainMessage(EVENT_NETWORK_CONNECTED).sendToTarget();
            }
        }
    }
    return NOERROR;
}


//====================================================================
// NetworkTimeUpdateService::SettingsObserver
//====================================================================
NetworkTimeUpdateService::SettingsObserver::SettingsObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ Int32 msg,
    /* [in] */ NetworkTimeUpdateService* owner)
{
    mHandler = handler;
    mMsg = msg;
    mOwner = owner;
    //ContentObserver(handler);
}

ECode NetworkTimeUpdateService::SettingsObserver::Observe(
    /* [in] */ IContext* context)
{
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<IUri> uri;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetUriFor(String("auto_time"), (IUri**)&uri);
    resolver->RegisterContentObserver((IUri*)uri, FALSE, (IContentObserver*)this, 0);
    return NOERROR;
}

ECode NetworkTimeUpdateService::SettingsObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    Boolean result;
    return mHandler->SendEmptyMessage(mMsg, &result);
}

//====================================================================
// NetworkTimeUpdateService::MyReceiver
//====================================================================
NetworkTimeUpdateService::MyReceiver::MyReceiver(
    /* [in] */ NetworkTimeUpdateService* owner)
    : mOwner(owner)
{
}

ECode NetworkTimeUpdateService::MyReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    return mOwner->SendMessage(EVENT_POLL_NETWORK_TIME);
}

}//namespace Server
}//namespace Droid
}//namespace Elastos

