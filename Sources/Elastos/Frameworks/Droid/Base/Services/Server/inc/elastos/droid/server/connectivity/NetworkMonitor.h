
#ifndef __ELASTOS_DROID_SERVER_CONNECTIVITY_NETWORK_MONITOR_H__
#define __ELASTOS_DROID_SERVER_CONNECTIVITY_NETWORK_MONITOR_H__

#include <elastos/droid/internal/utility/StateMachine.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include "_Elastos.Droid.Server.h"

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetwork;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Telephony::ITelephonyManager;

using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Internal::Utility::StateMachine;
using Elastos::Droid::Server::Connectivity::INetworkAgentInfo;

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

class NetworkAgentInfo;

/**
 * {@hide}
 */
class NetworkMonitor
    : public StateMachine
{
private:

    class DefaultState : public State
    {
    public:
        DefaultState(
            /* [in] */ NetworkMonitor* host);

        //@Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();

    private:
        NetworkMonitor* mHost;
    };

    class OfflineState : public State {
    public:
        OfflineState(
            /* [in] */ NetworkMonitor* host);

        //@Override
        CARAPI Enter();

        //@Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();
    private:
        NetworkMonitor* mHost;
    };

    class ValidatedState : public State {
    public:
        ValidatedState(
            /* [in] */ NetworkMonitor* host);

        //@Override
        CARAPI Enter();

        //@Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        CARAPI_(String) GetName();
    private:
        NetworkMonitor* mHost;
    };

    class EvaluatingState : public State {
    public:
        EvaluatingState(
            /* [in] */ NetworkMonitor* host);

        //@Override
        CARAPI Enter();

        //@Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI Exit();

        CARAPI_(String) GetName();

    private:
        NetworkMonitor* mHost;
        Int32 mRetries;
    };

    // BroadcastReceiver that waits for a particular Intent and then posts a message.
    class CustomIntentReceiver
        : public BroadcastReceiver
    {
    public:
        TO_STRING_IMPL("NetworkMonitor::CustomIntentReceiver")

        CARAPI constructor(
            /* [in] */ const String& action,
            /* [in] */ Int32 token,
            /* [in] */ Int32 message,
            /* [in] */ NetworkMonitor* host);

        CARAPI GetPendingIntent(
            /* [out] */ IPendingIntent** pi);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);
    private:
        NetworkMonitor* mHost;
        AutoPtr<IMessage> mMessage;
        String mAction;
    };

    class UserPromptedState : public State
    {
    public:
        UserPromptedState(
            /* [in] */ NetworkMonitor* host);

        //@Override
        CARAPI Enter();

        //@Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI Exit();

        CARAPI_(String) GetName();

    private:

        // Intent broadcast when user selects sign-in notification.
        static const String ACTION_SIGN_IN_REQUESTED;// = "android.net.netmon.sign_in_requested";

        AutoPtr<CustomIntentReceiver>  mUserRespondedBroadcastReceiver;

        NetworkMonitor* mHost;
    };

        class CaptivePortalLoggedInBroadcastReceiver
            : public BroadcastReceiver
        {
        public:
            TO_STRING_IMPL("NetworkMonitor::CaptivePortalLoggedInBroadcastReceiver")

            CaptivePortalLoggedInBroadcastReceiver(
                /* [in] */ Int32 token,
                /* [in] */ NetworkMonitor* host);

            //@Override
            CARAPI OnReceive(
                /* [in] */ IContext* context,
                /* [in] */ IIntent* intent);
        private:
            Int32 mToken;
            NetworkMonitor* mHost;
        };

    class CaptivePortalState : public State
    {

    public:
        CaptivePortalState(
            /* [in] */ NetworkMonitor* host);

        //@Override
        CARAPI Enter();

        //@Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI Exit();

        CARAPI_(String) GetName();
    private:
        NetworkMonitor* mHost;
    };

    class LingeringState : public State {
    public:
        LingeringState(
            /* [in] */ NetworkMonitor* host);

        //@Override
        CARAPI Enter();

        //@Override
        CARAPI ProcessMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI Exit();

        CARAPI_(String) GetName();
    private:
        NetworkMonitor* mHost;

    private:
        static const String ACTION_LINGER_EXPIRED;

        AutoPtr<CustomIntentReceiver> mBroadcastReceiver;
        AutoPtr<IPendingIntent> mIntent;
    };

public:

    NetworkMonitor(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* handler,
        /* [in] */ NetworkAgentInfo* networkAgentInfo);

private:
    /**
     * Do a URL fetch on a known server to see if we get the data we expect.
     * Returns HTTP response code.
     */
    Int32 IsCaptivePortal();

    /**
     * @param responseReceived - whether or not we received a valid HTTP response to our request.
     * If false, IsCaptivePortal and responseTimestampMs are ignored
     * TODO: This should be moved to the transports.  The latency could be passed to the transports
     * along with the captive portal result.  Currently the TYPE_MOBILE broadcasts appear unused so
     * perhaps this could just be added to the WiFi transport only.
     */
    void SendNetworkConditionsBroadcast(
        /* [in] */ Boolean responseReceived,
        /* [in] */ Boolean isCaptivePortal,
        /* [in] */ Int64 requestTimestampMs,
         /* [in] */ Int64 responseTimestampMs);

public:
    static const String ACTION_NETWORK_CONDITIONS_MEASURED;// = "android.net.conn.NETWORK_CONDITIONS_MEASURED";
    static const String EXTRA_CONNECTIVITY_TYPE;// = "extra_connectivity_type";
    static const String EXTRA_NETWORK_TYPE;// = "extra_network_type";
    static const String EXTRA_RESPONSE_RECEIVED;// = "extra_response_received";
    static const String EXTRA_IS_CAPTIVE_PORTAL;// = "extra_is_captive_portal";
    static const String EXTRA_CELL_ID;// = "extra_cellid";
    static const String EXTRA_SSID;// = "extra_ssid";
    static const String EXTRA_BSSID;// = "extra_bssid";
    /** real time since boot */
    static const String EXTRA_REQUEST_TIMESTAMP_MS;// = "extra_request_timestamp_ms";
    static const String EXTRA_RESPONSE_TIMESTAMP_MS;// = "extra_response_timestamp_ms";

    // After a network has been tested this result can be sent with EVENT_NETWORK_TESTED.
    // The network should be used as a default internet connection.  It was found to be:
    // 1. a functioning network providing internet access, or
    // 2. a captive portal and the user decided to use it as is.
    static const Int32 NETWORK_TEST_RESULT_VALID;// = 0;
    // After a network has been tested this result can be sent with EVENT_NETWORK_TESTED.
    // The network should not be used as a default internet connection.  It was found to be:
    // 1. a captive portal and the user is prompted to sign-in, or
    // 2. a captive portal and the user did not want to use it, or
    // 3. a broken network (e.g. DNS failed, connect failed, HTTP request failed).
    static const Int32 NETWORK_TEST_RESULT_INVALID;// = 1;

    static const Int32 BASE = 0x00082000;// = IProtocol::BASE_NETWORK_MONITOR;
    /**
     * Inform NetworkMonitor that their network is connected.
     * Initiates Network Validation.
     */
    static const Int32 CMD_NETWORK_CONNECTED = 0x00082000 + 1;// = BASE + 1;

    /**
     * Inform ConnectivityService that the network has been tested.
     * obj = NetworkAgentInfo
     * arg1 = One of the NETWORK_TESTED_RESULT_* constants.
     */
    static const Int32 EVENT_NETWORK_TESTED = 0x00082000 + 2;// = BASE + 2;

    /**
     * Inform NetworkMonitor to linger a network.  The Monitor should
     * start a timer and/or start watching for zero live connections while
     * moving towards LINGER_COMPLETE.  After the Linger period expires
     * (or other events mark the end of the linger state) the LINGER_COMPLETE
     * event should be sent and the network will be shut down.  If a
     * CMD_NETWORK_CONNECTED happens before the LINGER completes
     * it indicates further desire to keep the network alive and so
     * the LINGER is aborted.
     */
    static const Int32 CMD_NETWORK_LINGER = 0x00082000 + 3;// = BASE + 3;

    /**
     * Message to self indicating linger delay has expired.
     * arg1 = Token to ignore old messages.
     */
    static const Int32 CMD_LINGER_EXPIRED = 0x00082000 + 4;// = BASE + 4;

    /**
     * Inform ConnectivityService that the network LINGER period has
     * expired.
     * obj = NetworkAgentInfo
     */
    static const Int32 EVENT_NETWORK_LINGER_COMPLETE = 0x00082000 + 5;// = BASE + 5;

    /**
     * Message to self indicating it's time to evaluate a network's connectivity.
     * arg1 = Token to ignore old messages.
     */
    static const Int32 CMD_REEVALUATE = 0x00082000 + 6;// = BASE + 6;

    /**
     * Inform NetworkMonitor that the network has disconnected.
     */
    static const Int32 CMD_NETWORK_DISCONNECTED = 0x00082000 + 7;// = BASE + 7;

    /**
     * Force evaluation even if it has succeeded in the past.
     * arg1 = UID responsible for requesting this reeval.  Will be billed for data.
     */
    static const Int32 CMD_FORCE_REEVALUATION = 0x00082000 + 8;// = BASE + 8;

    /**
     * Message to self indicating captive portal login is complete.
     * arg1 = Token to ignore old messages.
     * arg2 = 1 if we should use this network, 0 otherwise.
     */
    static const Int32 CMD_CAPTIVE_PORTAL_LOGGED_IN = 0x00082000 + 9;// = BASE + 9;

    /**
     * Message to self indicating user desires to log into captive portal.
     * arg1 = Token to ignore old messages.
     */
    static const Int32 CMD_USER_WANTS_SIGN_IN = 0x00082000 + 10;// = BASE + 10;

    /**
     * Request ConnectivityService display provisioning notification.
     * arg1    = Whether to make the notification visible.
     * arg2    = NetID.
     * obj     = Intent to be launched when notification selected by user, null if !arg1.
     */
    static const Int32 EVENT_PROVISIONING_NOTIFICATION = 0x00082000 + 11;// = BASE + 11;

    /**
     * Message to self indicating sign-in app bypassed captive portal.
     */
    static const Int32 EVENT_APP_BYPASSED_CAPTIVE_PORTAL = 0x00082000 + 12;// = BASE + 12;

    /**
     * Message to self indicating no sign-in app responded.
     */
    static const Int32 EVENT_NO_APP_RESPONSE = 0x00082000 + 13;// = BASE + 13;

    /**
     * Message to self indicating sign-in app indicates sign-in is not possible.
     */
    static const Int32 EVENT_APP_INDICATES_SIGN_IN_IMPOSSIBLE = 0x00082000 + 14;// = BASE + 14;

    Boolean mSystemReady;
private:
    static const Boolean DBG;
    static const String TAG;
    static const String DEFAULT_SERVER;
    static const Int32 SOCKET_TIMEOUT_MS;

    static const String PERMISSION_ACCESS_NETWORK_CONDITIONS;

    // Keep these in sync with CaptivePortalLoginActivity.java.
    // Intent broadcast from CaptivePortalLogin indicating sign-in is complete.
    // Extras:
    //     EXTRA_TEXT       = netId
    //     LOGGED_IN_RESULT = "1" if we should use network, "0" if not.
    static const String ACTION_CAPTIVE_PORTAL_LOGGED_IN;
    static const String LOGGED_IN_RESULT;

    static const String LINGER_DELAY_PROPERTY;
    // Default to 30s linger time-out.
    static const Int32 DEFAULT_LINGER_DELAY_MS;
    Int32 mLingerDelayMs;
    Int32 mLingerToken;

    // Negative values disable reevaluation.
    static const String REEVALUATE_DELAY_PROPERTY;
    // Default to 5s reevaluation delay.
    static const Int32 DEFAULT_REEVALUATE_DELAY_MS;
    static const Int32 MAX_RETRIES;
    Int32 mReevaluateDelayMs;
    Int32 mReevaluateToken;
    static const Int32 INVALID_UID;
    Int32 mUidResponsibleForReeval;

    Int32 mCaptivePortalLoggedInToken;
    Int32 mUserPromptedToken;

    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mConnectivityServiceHandler;
    NetworkAgentInfo* mNetworkAgentInfo;
    AutoPtr<ITelephonyManager> mTelephonyManager;
    AutoPtr<IWifiManager> mWifiManager;
    AutoPtr<IAlarmManager> mAlarmManager;

    String mServer;
    Boolean mIsCaptivePortalCheckEnabled;

    // Set if the user explicitly selected "Do not use this network" in captive portal sign-in app.
    Boolean mUserDoesNotWant;

    AutoPtr<State> mDefaultState;
    AutoPtr<State> mOfflineState;
    AutoPtr<State> mValidatedState;
    AutoPtr<State> mEvaluatingState;
    AutoPtr<State> mUserPromptedState;
    AutoPtr<State> mCaptivePortalState;
    AutoPtr<State> mLingeringState;

    AutoPtr<CaptivePortalLoggedInBroadcastReceiver>  mCaptivePortalLoggedInBroadcastReceiver;
};

} // Connectivity
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CONNECTIVITY_NETWORK_MONITOR_H__
