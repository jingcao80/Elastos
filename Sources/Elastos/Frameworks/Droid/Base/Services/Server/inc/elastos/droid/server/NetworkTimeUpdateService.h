#ifndef __ELASTOS_DROID_SERVER_NETWORKTIMEUPDATESERVICE_H__
#define __ELASTOS_DROID_SERVER_NETWORKTIMEUPDATESERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Utility::ITrustedTime;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Database::ContentObserver;

namespace Elastos {
namespace Droid {
namespace Server {

class NetworkTimeUpdateService : public ElRefBase
{
private:
    class NitzReceiver : public BroadcastReceiver
    {
    public:
        NitzReceiver(
            /* [in] */ NetworkTimeUpdateService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("NetworkTimeUpdateService::NitzReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        NetworkTimeUpdateService* mOwner;
    };

    /** Receiver for ConnectivityManager events */
    class ConnectivityReceiver : public BroadcastReceiver
    {
    public:
        ConnectivityReceiver(
            /* [in] */ NetworkTimeUpdateService* owner);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("NetworkTimeUpdateService::ConnectivityReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        NetworkTimeUpdateService* mOwner;
    };

    /** Observer to watch for changes to the AUTO_TIME setting */
    class SettingsObserver : public ContentObserver
    {
    public:
        SettingsObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ Int32 msg,
            /* [in] */ NetworkTimeUpdateService* owner);

        CARAPI Observe(
            /* [in] */ IContext* context);

        //@Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        Int32 mMsg;
        AutoPtr<IHandler> mHandler;
        NetworkTimeUpdateService* mOwner;
    };

    class MyReceiver : public BroadcastReceiver
    {
    public:
        MyReceiver(
            /* [in] */ NetworkTimeUpdateService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("NetworkTimeUpdateService::MyReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        NetworkTimeUpdateService* mOwner;
    };

    /** Handler to do the network accesses on */
    class MyHandler
        : public HandlerBase
    {
    public:
        MyHandler(
            /* [in] */ ILooper* l,
            /* [in] */ NetworkTimeUpdateService* host);

        virtual CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        NetworkTimeUpdateService* mHost;
    };

public:
    NetworkTimeUpdateService(
        /* [in] */ IContext* context);

    /** Initialize the receivers and initiate the first NTP request */
    CARAPI SystemReady();

private:
    CARAPI_(void) RegisterForTelephonyIntents();

    CARAPI_(void) RegisterForAlarms();

    CARAPI_(void) RegisterForConnectivityIntents();

    CARAPI_(void) OnPollNetworkTime(
        /* [in] */ Int32 event);

    /**
     * Cancel old alarm and starts a new one for the specified interval.
     *
     * @param interval when to trigger the alarm, starting from now.
     */
    CARAPI_(void) ResetAlarm(
        /* [in] */ Int64 interval);

    /**
     * Checks if the user prefers to automatically set the time.
     */
    CARAPI_(Boolean) IsAutomaticTimeRequested();

    CARAPI SendMessage(
        /* [in] */ Int32 event);
private:
    friend class MyHandler;
    friend class ConnectivityReceiver;

    static const String TAG;
    static const Boolean DBG;

    static const Int32 EVENT_AUTO_TIME_CHANGED = 1;
    static const Int32 EVENT_POLL_NETWORK_TIME = 2;
    static const Int32 EVENT_NETWORK_CONNECTED = 3;

    /** Normal polling frequency */
    static const Int64 POLLING_INTERVAL_MS; // 24 hrs
    /** Try-again polling interval, in case the network request failed */
    static const Int64 POLLING_INTERVAL_SHORTER_MS; // 60 seconds
    /** Number of times to try again */
    static const Int32 TRY_AGAIN_TIMES_MAX = 3;
    /** If the time difference is greater than this threshold, then update the time. */
    static const Int32 TIME_ERROR_THRESHOLD_MS = 5 * 1000;

    static const String ACTION_POLL;
    static Int32 POLL_REQUEST;

    static const Int64 NOT_SET;
    Int64 mNitzTimeSetTime;
    // TODO: Have a way to look up the timezone we are in
    Int64 mNitzZoneSetTime;

    AutoPtr<IContext> mContext;
    AutoPtr<ITrustedTime> mTime;

    // NTP lookup is done on this thread and handler
    AutoPtr<MyHandler> mHandler;
    AutoPtr<IHandlerThread> mThread;
    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IPendingIntent> mPendingPollIntent;
    AutoPtr<SettingsObserver> mSettingsObserver;
    // The last time that we successfully fetched the NTP time.
    Int64 mLastNtpFetchTime;
    // Keeps track of how many quick attempts were made to fetch NTP time.
    // During bootup, the network may not have been up yet, or it's taking time for the
    // connection to happen.
    Int32 mTryAgainCounter;

    AutoPtr<MyReceiver> mAlarmReceiver;
    AutoPtr<NitzReceiver> mNitzReceiver;

    AutoPtr<ConnectivityReceiver> mConnectivityReceiver;
};

}//namespace Server
}//namespace Droid
}//namespace Elastos

#endif //__ELASTOS_DROID_SERVER_NETWORKTIMEUPDATESERVICE_H__
