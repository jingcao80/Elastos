
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTRACKERBASE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTRACKERBASE_H__

#include "_Elastos.Droid.Internal.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include <elastos/core/Object.h>
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/database/ContentObserver.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/internal/utility/State.h>
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/provider/Settings.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Internal::Telephony::IDctConstants;
using Elastos::Droid::Internal::Telephony::IPhoneBase;
using Elastos::Droid::Internal::Telephony::Uicc::IIccRecords;
using Elastos::Droid::Internal::Telephony::Uicc::IUiccController;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::State;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::INetworkCapabilities;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Provider::Settings;
using Elastos::Core::IRunnable;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::IConcurrentHashMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicReference;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IPriorityQueue;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

/**
 * {@hide}
 */
class DcTrackerBase
    : public Handler
    , public IDcTrackerBase
{
public:
    /**
     * Maintain the sum of transmit and receive packets.
     *
     * The packet counts are initialized and reset to -1 and
     * remain -1 until they can be updated.
     */
    class TxRxSum
        : public Object
        , public IDcTrackerBaseTxRxSum
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ Int64 txPkts,
            /* [in] */ Int64 rxPkts);

        CARAPI constructor(
            /* [in] */ IDcTrackerBaseTxRxSum* sum);

        CARAPI Reset();

        // @Override
        CARAPI ToString(
            /* [out] */ String* result);

        CARAPI UpdateTxRxSum();

        CARAPI GetTxPkts(
            /* [out] */ Int64* result);

        CARAPI SetTxPkts(
            /* [in] */ Int64 txPkts);

        CARAPI GetRxPkts(
            /* [out] */ Int64* result);

        CARAPI SetRxPkts(
            /* [in] */ Int64 rxPkts);

    private:
        Int64 mTxPkts;

        Int64 mRxPkts;
    };

    // Recovery action taken in case of data stall
    class RecoveryAction
        : public Object
    {
    private:
        static CARAPI IsAggressiveRecovery(
            /* [in] */ Int32 value,
            /* [out] */ Boolean* result);
    };

private:
    class SubBroadcastReceiver
        : public BroadcastReceiver
    {
    public:
        SubBroadcastReceiver(
            /* [in] */ DcTrackerBase* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        DcTrackerBase* mHost;
    };

    class SubRunnable
        : public Object
        , public IRunnable
    {
    public:
        CAR_INTERFACE_DECL()

        SubRunnable(
            /* [in] */ DcTrackerBase* host);

        // @Override
        CARAPI Run();

    private:
        DcTrackerBase* mHost;
    };

    class DataRoamingSettingObserver
        : public ContentObserver
    {
    public:
        DataRoamingSettingObserver(
            /* [in] */ DcTrackerBase* host);

        CARAPI constructor(
            /* [in] */ IHandler* handler,
            /* [in] */ IContext* context);

        CARAPI Register();

        CARAPI Unregister();

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        DcTrackerBase* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    DcTrackerBase();

    CARAPI GetInitialMaxRetry(
        /* [out] */ Int32* result);

    CARAPI OnActionIntentReconnectAlarm(
        /* [in] */ IIntent* intent);

    CARAPI OnActionIntentRestartTrySetupAlarm(
        /* [in] */ IIntent* intent);

    CARAPI OnActionIntentDataStallAlarm(
        /* [in] */ IIntent* intent);

    /**
     * Default constructor
     */
    CARAPI constructor(
        /* [in] */ IPhoneBase* phone);

    CARAPI Dispose();

    CARAPI GetActivity(
        /* [out] */ DctConstantsActivity* result);

    CARAPI SetActivity(
        /* [in] */ DctConstantsActivity activity);

    CARAPI IsApnTypeActive(
        /* [in] */ const String& type,
        /* [out] */ Boolean* result);

    CARAPI FetchDunApn(
        /* [out] */ IApnSetting** result);

    CARAPI GetActiveApnTypes(
        /* [out, callee] */ ArrayOf<String>** result);

    /** TODO: See if we can remove */
    CARAPI GetActiveApnString(
        /* [in] */ const String& apnType,
        /* [out] */ String* result);

    /**
     * Modify {@link android.provider.Settings.Global#DATA_ROAMING} value.
     */
    CARAPI SetDataOnRoamingEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Return current {@link android.provider.Settings.Global#DATA_ROAMING} value.
     */
    CARAPI GetDataOnRoamingEnabled(
        /* [out] */ Boolean* result);

    /**
     * Modify {@link android.provider.Settings.Global#MOBILE_DATA} value.
     */
    CARAPI SetDataEnabled(
        /* [in] */ Boolean enable);

    /**
     * Return current {@link android.provider.Settings.Global#MOBILE_DATA} value.
     */
    CARAPI GetDataEnabled(
        /* [out] */ Boolean* result);

    // abstract methods
    CARAPI RestartRadio() = 0;

    CARAPI Log(
        /* [in] */ const String& s) = 0;

    CARAPI Loge(
        /* [in] */ const String& s) = 0;

    CARAPI IsDataAllowed(
        /* [out] */ Boolean* result) = 0;

    CARAPI IsApnTypeAvailable(
        /* [in] */ const String& type,
        /* [out] */ Boolean* result) = 0;

    CARAPI GetState(
        /* [in] */ const String& apnType,
        /* [out] */ DctConstantsState* result) = 0;

    CARAPI IsProvisioningApn(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result) = 0;

    virtual CARAPI SetState(
        /* [in] */ DctConstantsState s) = 0;

    virtual CARAPI GotoIdleAndNotifyDataConnection(
        /* [in] */ const String& reason) = 0;

    virtual CARAPI OnTrySetupData(
        /* [in] */ const String& reason,
        /* [out] */ Boolean* result) = 0;

    virtual CARAPI OnRoamingOff() = 0;

    virtual CARAPI OnRoamingOn() = 0;

    virtual CARAPI OnRadioAvailable() = 0;

    virtual CARAPI OnRadioOffOrNotAvailable() = 0;

    virtual CARAPI OnDataSetupComplete(
        /* [in] */ IAsyncResult* ar) = 0;

    virtual CARAPI OnDataSetupCompleteError(
        /* [in] */ IAsyncResult* ar) = 0;

    virtual CARAPI OnDisconnectDone(
        /* [in] */ Int32 connId,
        /* [in] */ IAsyncResult* ar) = 0;

    virtual CARAPI OnDisconnectDcRetrying(
        /* [in] */ Int32 connId,
        /* [in] */ IAsyncResult* ar) = 0;

    virtual CARAPI OnVoiceCallStarted() = 0;

    virtual CARAPI OnVoiceCallEnded() = 0;

    virtual CARAPI OnCleanUpConnection(
        /* [in] */ Boolean tearDown,
        /* [in] */ Int32 apnId,
        /* [in] */ const String& reason) = 0;

    virtual CARAPI OnCleanUpAllConnections(
        /* [in] */ const String& cause) = 0;

    virtual CARAPI IsDataPossible(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result) = 0;

    virtual CARAPI IsOnDemandDataPossible(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result) = 0;

    virtual CARAPI OnUpdateIcc(
        /* [out] */ Boolean* result) = 0;

    virtual CARAPI CompleteConnection(
        /* [in] */ IApnContext* apnContext) = 0;

    virtual CARAPI SetDataAllowed(
        /* [in] */ Boolean enable,
        /* [in] */ IMessage* response) = 0;

    virtual CARAPI GetPcscfAddress(
        /* [in] */ const String& apnType,
        /* [out, callee] */ ArrayOf<String>** result) = 0;

    virtual CARAPI SetImsRegistrationState(
        /* [in] */ Boolean registered) = 0;

    virtual CARAPI MvnoMatches(
        /* [in] */ IIccRecords* r,
        /* [in] */ const String& mvno_type,
        /* [in] */ const String& mvno_match_data,
        /* [out] */ Boolean* result) = 0;

    virtual CARAPI IsPermanentFail(
        /* [in] */ IDcFailCause* dcFailCause,
        /* [out] */ Boolean* result) = 0;

    // @Override
    CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    /**
     * Report on whether data connectivity is enabled
     *
     * @return {@code false} if data connectivity has been explicitly disabled,
     *         {@code true} otherwise.
     */
    CARAPI GetAnyDataEnabled(
        /* [out] */ Boolean* result);

    CARAPI IsEmergency(
        /* [out] */ Boolean* result);

    CARAPI ApnTypeToId(
        /* [in] */ const String& type,
        /* [out] */ Int32* result);

    CARAPI ApnIdToType(
        /* [in] */ Int32 id,
        /* [out] */ String* result);

    CARAPI GetLinkProperties(
        /* [in] */ const String& apnType,
        /* [out] */ ILinkProperties** result);

    CARAPI GetNetworkCapabilities(
        /* [in] */ const String& apnType,
        /* [out] */ INetworkCapabilities** result);

    // tell all active apns of the current condition
    CARAPI NotifyDataConnection(
        /* [in] */ const String& reason);

    // disabled apn's still need avail/unavail notificiations - send them out
    CARAPI NotifyOffApnsOfAvailability(
        /* [in] */ const String& reason);

    CARAPI IsApnTypeEnabled(
        /* [in] */ const String& apnType,
        /* [out] */ Boolean* result);

    CARAPI IsApnIdEnabled(
        /* [in] */ Int32 id,
        /* [out] */ Boolean* result);

    CARAPI SetEnabled(
        /* [in] */ Int32 id,
        /* [in] */ Boolean enable);

    CARAPI OnEnableApn(
        /* [in] */ Int32 apnId,
        /* [in] */ Int32 enabled);

    /**
     * Called when we switch APNs.
     *
     * mRequestedApnType is set prior to call
     * To be overridden.
     */
    CARAPI OnEnableNewApn();

    /**
     * Called when EVENT_RESET_DONE is received so goto
     * IDLE state and send notifications to those interested.
     *
     * TODO - currently unused.  Needs to be hooked into DataConnection cleanup
     * TODO - needs to pass some notion of which connection is reset..
     */
    CARAPI OnResetDone(
        /* [in] */ IAsyncResult* ar);

    /**
     * Prevent mobile data connections from being established, or once again
     * allow mobile data connections. If the state toggles, then either tear
     * down or set up data, as appropriate to match the new state.
     *
     * @param enable indicates whether to enable ({@code true}) or disable (
     *            {@code false}) data
     * @return {@code true} if the operation succeeded
     */
    CARAPI SetInternalDataEnabled(
        /* [in] */ Boolean enable,
        /* [out] */ Boolean* result);

    CARAPI OnSetInternalDataEnabled(
        /* [in] */ Boolean enabled);

    CARAPI CleanUpAllConnections(
        /* [in] */ const String& cause);

    CARAPI IsDisconnected(
        /* [out] */ Boolean* result);

    CARAPI OnSetUserDataEnabled(
        /* [in] */ Boolean enabled);

    CARAPI OnSetDependencyMet(
        /* [in] */ const String& apnType,
        /* [in] */ Boolean met);

    CARAPI OnSetPolicyDataEnabled(
        /* [in] */ Boolean enabled);

    CARAPI GetReryConfig(
        /* [in] */ Boolean forDefault,
        /* [out] */ String* result);

    CARAPI ResetPollStats();

    CARAPI GetOverallState(
        /* [out] */ DctConstantsState* result);

    CARAPI StartNetStatPoll();

    CARAPI StopNetStatPoll();

    CARAPI SendStartNetStatPoll(
        /* [in] */ DctConstantsActivity activity);

    CARAPI HandleStartNetStatPoll(
        /* [in] */ DctConstantsActivity activity);

    CARAPI SendStopNetStatPoll(
        /* [in] */ DctConstantsActivity activity);

    CARAPI HandleStopNetStatPoll(
        /* [in] */ DctConstantsActivity activity);

    CARAPI UpdateDataActivity();

    CARAPI GetRecoveryAction(
        /* [out] */ Int32* result);

    CARAPI PutRecoveryAction(
        /* [in] */ Int32 action);

    CARAPI IsConnected(
        /* [out] */ Boolean* result);

    CARAPI DoRecovery();

    CARAPI OnDataStallAlarm(
        /* [in] */ Int32 tag);

    CARAPI StartDataStallAlarm(
        /* [in] */ Boolean suspectedStall);

    CARAPI StopDataStallAlarm();

    CARAPI RestartDataStallAlarm();

    CARAPI SetInitialAttachApn();

    CARAPI SetDataProfilesAsNeeded();

    CARAPI OnActionIntentProvisioningApnAlarm(
        /* [in] */ IIntent* intent);

    CARAPI StartProvisioningApnAlarm();

    CARAPI StopProvisioningApnAlarm();

    CARAPI SendCleanUpConnection(
        /* [in] */ Boolean tearDown,
        /* [in] */ IApnContext* apnContext);

    CARAPI SendRestartRadio();

    CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);
private:
    // current condition
    CARAPI NotifyApnIdUpToCurrent(
        /* [in] */ const String& reason,
        /* [in] */ Int32 apnId);

    // since we normally don't send info to a disconnected APN, we need to do this specially
    CARAPI NotifyApnIdDisconnected(
        /* [in] */ const String& reason,
        /* [in] */ Int32 apnId);

    CARAPI UpdateDataStallInfo();

public:
    static const Boolean DBG;

    static const Boolean VDBG;

    // STOPSHIP if true
    static const Boolean VDBG_STALL;

    // STOPSHIP if true
    static const Boolean RADIO_TESTS;

    static Boolean mIsCleanupRequired;

    /**
     * Constants for the data connection activity:
     * physical link down/up
     */
    static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_INACTIVE;

    static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_DOWN;

    static const Int32 DATA_CONNECTION_ACTIVE_PH_LINK_UP;

    /** Delay between APN attempts.
    Note the property override mechanism is there just for testing purpose only. */
    static const Int32 APN_DELAY_DEFAULT_MILLIS;

    /** Delay between APN attempts when in fail fast mode */
    static const Int32 APN_FAIL_FAST_DELAY_DEFAULT_MILLIS;

    AutoPtr<IAlarmManager> mAlarmManager;

    AutoPtr<IObject> mDataEnabledLock;

    // for example during emergency calls
    Boolean mInternalDataEnabled;

    // persisted
    Boolean mUserDataEnabled;

    // TODO: move away from static state once 5587429 is fixed.
    static Boolean sPolicyDataEnabled;

    /* Currently requested APN type (TODO: This should probably be a parameter not a member) */
    String mRequestedApnType;

    /** Retry configuration: A doubling of retry times from 5secs to 30minutes */
    static const String DEFAULT_DATA_RETRY_CONFIG;

    /** Retry configuration for secondary networks: 4 tries in 20 sec */
    static const String SECONDARY_DATA_RETRY_CONFIG;

    /** Slow poll when attempting connection recovery. */
    static const Int32 POLL_NETSTAT_SLOW_MILLIS;

    /** Default max failure count before attempting to network re-registration. */
    static const Int32 DEFAULT_MAX_PDP_RESET_FAIL;

    /**
     * After detecting a potential connection problem, this is the max number
     * of subsequent polls before attempting recovery.
     */
    static const Int32 NO_RECV_POLL_LIMIT;

    // 1 sec. default polling interval when screen is on.
    static const Int32 POLL_NETSTAT_MILLIS;

    // 10 min. default polling interval when screen is off.
    static const Int32 POLL_NETSTAT_SCREEN_OFF_MILLIS;

    // 2 min for round trip time
    static const Int32 POLL_LONGEST_RTT;

    // Default sent packets without ack which triggers initial recovery steps
    static const Int32 NUMBER_SENT_PACKETS_OF_HANG;

    // how long to wait before switching back to default APN
    static const Int32 RESTORE_DEFAULT_APN_DELAY;

    // system property that can override the above value
    static const String APN_RESTORE_DELAY_PROP_NAME;

    // represents an invalid IP address
    static const String NULL_IP;

    // Default for the data stall alarm while non-aggressive stall detection
    static const Int32 DATA_STALL_ALARM_NON_AGGRESSIVE_DELAY_IN_MS_DEFAULT;

    // Default for the data stall alarm for aggressive stall detection
    static const Int32 DATA_STALL_ALARM_AGGRESSIVE_DELAY_IN_MS_DEFAULT;

    // If attempt is less than this value we're doing first level recovery
    static const Int32 DATA_STALL_NO_RECV_POLL_LIMIT;

    // Tag for tracking stale alarms
    static const String DATA_STALL_ALARM_TAG_EXTRA;

    static const Boolean DATA_STALL_SUSPECTED;

    static const Boolean DATA_STALL_NOT_SUSPECTED;

    String mRADIO_RESET_PROPERTY;

    static const String INTENT_RECONNECT_ALARM;

    static const String INTENT_RECONNECT_ALARM_EXTRA_TYPE;

    static const String INTENT_RECONNECT_ALARM_EXTRA_REASON;

    static const String INTENT_RESTART_TRYSETUP_ALARM;

    static const String INTENT_RESTART_TRYSETUP_ALARM_EXTRA_TYPE;

    static const String INTENT_DATA_STALL_ALARM;

    static const String DEFALUT_DATA_ON_BOOT_PROP;

    AutoPtr<IDcTesterFailBringUpAll> mDcTesterFailBringUpAll;

    AutoPtr<IDcController> mDcc;

    // member variables
    AutoPtr<IPhoneBase> mPhone;

    AutoPtr<IUiccController> mUiccController;

    AutoPtr<IAtomicReference> mIccRecords;

    DctConstantsActivity mActivity;

    DctConstantsState mState;

    AutoPtr<IHandler> mDataConnectionTracker;

    Int64 mTxPkts;

    Int64 mRxPkts;

    Int32 mNetStatPollPeriod;

    Boolean mNetStatPollEnabled;

    AutoPtr<IDcTrackerBaseTxRxSum> mDataStallTxRxSum;

    // Used to track stale data stall alarms.
    Int32 mDataStallAlarmTag;

    // The current data stall alarm intent
    AutoPtr<IPendingIntent> mDataStallAlarmIntent;

    // Number of packets sent since the last received packet
    Int64 mSentSinceLastRecv;

    // Controls when a simple recovery attempt it to be tried
    Int32 mNoRecvPollCount;

    // Refrence counter for enabling fail fast
    static Int32 sEnableFailFastRefCounter;

    // True if data stall detection is enabled
    Boolean mDataStallDetectionEnabled;

    Boolean mFailFast;

    // True when in voice call
    Boolean mInVoiceCall;

    // wifi connection status will be updated by sticky intent
    Boolean mIsWifiConnected;

    /** Intent sent when the reconnect alarm fires. */
    AutoPtr<IPendingIntent> mReconnectIntent;

    /** CID of active data connection */
    Int32 mCidActive;

    // When false we will not auto attach and manually attaching is required.
    Boolean mAutoAttachOnCreationConfig;

    Boolean mAutoAttachOnCreation;

    // State of screen
    // (TODO: Reconsider tying directly to screen, maybe this is
    //        really a lower power mode")
    Boolean mIsScreenOn;

    /** Allows the generation of unique Id's for DataConnection objects */
    AutoPtr<IAtomicInteger32> mUniqueIdGenerator;

    /** The data connections. */
    AutoPtr<IHashMap> mDataConnections;

    /** The data connection async channels */
    AutoPtr<IHashMap> mDataConnectionAcHashMap;

    /** Convert an ApnType string to Id (TODO: Use "enumeration" instead of String for ApnType) */
    AutoPtr<IHashMap> mApnToDataConnectionId;

    /** Phone.APN_TYPE_* ===> ApnContext */
    AutoPtr<IConcurrentHashMap> mApnContexts;

    /** kept in sync with mApnContexts
     * Higher numbers are higher priority and sorted so highest priority is first */
    AutoPtr<IPriorityQueue> mPrioritySortedApnContexts;

    /* Currently active APN */
    AutoPtr<IApnSetting> mActiveApn;

    /** allApns holds all apns */
    AutoPtr<IArrayList> mAllApnSettings;

    /** preferred apn */
    AutoPtr<IApnSetting> mPreferredApn;

    /** Is packet service restricted by network */
    Boolean mIsPsRestricted;

    /** emergency apn Setting*/
    AutoPtr<IApnSetting> mEmergencyApn;

    /* Once disposed dont handle any messages */
    Boolean mIsDisposed;

    AutoPtr<IContentResolver> mResolver;

    /* Set to true with CMD_ENABLE_MOBILE_PROVISIONING */
    Boolean mIsProvisioning;

    /* The Url passed as object parameter in CMD_ENABLE_MOBILE_PROVISIONING */
    String mProvisioningUrl;

    /* Intent for the provisioning apn alarm */
    static const String INTENT_PROVISIONING_APN_ALARM;

    /* Tag for tracking stale alarms */
    static const String PROVISIONING_APN_ALARM_TAG_EXTRA;

    /* Debug property for overriding the PROVISIONING_APN_ALARM_DELAY_IN_MS */
    static const String DEBUG_PROV_APN_ALARM;

    /* Default for the provisioning apn alarm timeout */
    static const Int32 PROVISIONING_APN_ALARM_DELAY_IN_MS_DEFAULT;

    /* The provision apn alarm intent used to disable the provisioning apn */
    AutoPtr<IPendingIntent> mProvisioningApnAlarmIntent;

    /* Used to track stale provisioning apn alarms */
    Int32 mProvisioningApnAlarmTag;

    AutoPtr<IAsyncChannel> mReplyAc;

    AutoPtr<IBroadcastReceiver> mIntentReceiver;

    AutoPtr<IConnectivityManager> mCm;

private:
    AutoPtr<ArrayOf<Boolean> > mDataEnabled;

    Int32 mEnabledCount;

    AutoPtr<DataRoamingSettingObserver> mDataRoamingSettingObserver;

    AutoPtr<IRunnable> mPollNetStat;

    /**
     * The Initial MaxRetry sent to a DataConnection as a parameter
     * to DataConnectionAc.bringUp. This value can be defined at compile
     * time using the SystemProperty Settings.Global.DCT_INITIAL_MAX_RETRY
     * and at runtime using gservices to change Settings.Global.DCT_INITIAL_MAX_RETRY.
     */
    static const Int32 DEFAULT_MDC_INITIAL_RETRY;
};

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_DATACONNECTION_DCTRACKERBASE_H__
