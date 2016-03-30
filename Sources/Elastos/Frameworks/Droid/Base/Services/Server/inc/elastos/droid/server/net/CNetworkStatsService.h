
#ifndef __ELASTOS_DROID_SERVER_NET_CNETWORKSTATSSERVICE_H__
#define __ELASTOS_DROID_SERVER_NET_CNETWORKSTATSSERVICE_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "_Elastos_Droid_Server_Net_CNetworkStatsService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/server/net/NetworkStatsCollection.h"
#include "elastos/droid/server/net/NetworkStatsRecorder.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Telephony.h>
#include <Elastos.Droid.Utility.h>
#include <elastos/droid/server/net/BaseNetworkObserver.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::IIAlarmManager;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::CNetworkStats;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IIConnectivityManager;
using Elastos::Droid::Net::IINetworkManagementEventObserver;
using Elastos::Droid::Net::IINetworkStatsService;
using Elastos::Droid::Net::IINetworkStatsSession;
using Elastos::Droid::Net::INetworkIdentity;
using Elastos::Droid::Net::INetworkStats;
using Elastos::Droid::Net::INetworkStatsHistory;
using Elastos::Droid::Net::INetworkStatsNonMonotonicObserver;
using Elastos::Droid::Net::INetworkTemplate;
using Elastos::Droid::Net::INetworkTemplateHelper;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManager;
using Elastos::Droid::Os::IPowerManagerWakeLock;
// using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::ITrustedTime;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

CarClass(CNetworkStatsService)
    , public Object
    , public IBinder
    , public IINetworkStatsService
{
public:
    class MyHandlerCallback
        : public Object
        , public IHandlerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyHandlerCallback(
            /* [in] */ CNetworkStatsService* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    private:
        CNetworkStatsService* mHost;
    };

    class NetworkStatsSettingsConfig
        : public Object
        , public INetworkStatsSettingsConfig
    {
    public:
        CAR_INTERFACE_DECL()

        NetworkStatsSettingsConfig(
            /* [in] */ Int64 bucketDuration,
            /* [in] */ Int64 rotateAgeMillis,
            /* [in] */ Int64 deleteAgeMillis)
            : mBucketDuration(bucketDuration)
            , mRotateAgeMillis(rotateAgeMillis)
            , mDeleteAgeMillis(deleteAgeMillis)
        {}

        CARAPI GetBucketDuration(
            /* [out] */ Int64* result);

        CARAPI GetRotateAgeMillis(
            /* [out] */ Int64* result);

        CARAPI GetDeleteAgeMillis(
            /* [out] */ Int64* result);

    private:
        Int64 mBucketDuration;
        Int64 mRotateAgeMillis;
        Int64 mDeleteAgeMillis;
    };

private:
    class ConnReceiver
        : public BroadcastReceiver
    {
    public:
        ConnReceiver(
            /* [in] */ CNetworkStatsService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CNetworkStatsService::ConnReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CNetworkStatsService* mOwner;
    };

    class TetherReceiver
        : public BroadcastReceiver
    {
    public:
        TetherReceiver(
            /* [in] */ CNetworkStatsService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CNetworkStatsService::TetherReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CNetworkStatsService* mOwner;
    };

    class PollReceiver
        : public BroadcastReceiver
    {
    public:
        PollReceiver(
            /* [in] */ CNetworkStatsService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CNetworkStatsService::PollReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CNetworkStatsService* mOwner;
    };

    class RemovedReceiver
        : public BroadcastReceiver
    {
    public:
        RemovedReceiver(
            /* [in] */ CNetworkStatsService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CNetworkStatsService::RemovedReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CNetworkStatsService* mOwner;
    };

    class UserReceiver
        : public BroadcastReceiver
    {
    public:
        UserReceiver(
            /* [in] */ CNetworkStatsService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CNetworkStatsService::UserReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CNetworkStatsService* mOwner;
    };

    class ShutdownReceiver
        : public BroadcastReceiver
    {
    public:
        ShutdownReceiver(
            /* [in] */ CNetworkStatsService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CNetworkStatsService::ShutdownReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CNetworkStatsService* mOwner;
    };

    /**
     * Receiver that watches for {@link TelephonyManager} changes, such as
     * transitioning between network types.
     */

    // class PhoneListener// : public PhoneStateListener
    // {
    // public:
    //     PhoneListener(
    //         /* [in] */ CNetworkStatsService* host);
    //
    //     CARAPI_(void) OnDataConnectionStateChanged(
    //         /* [in] */ Int32 state,
    //         /* [in] */ Int32 networkType);
    // private:
    //     CNetworkStatsService* mHost;
    // };

    class DropBoxNonMonotonicObserver
        : public Object
        , public INetworkStatsNonMonotonicObserver
    {
    public:
        CAR_INTERFACE_DECL()

        DropBoxNonMonotonicObserver(
            /* [in] */ CNetworkStatsService* owner);

        CARAPI FoundNonMonotonic(
            /* [in] */ INetworkStats* left,
            /* [in] */ Int32 leftIndex,
            /* [in] */ INetworkStats* right,
            /* [in] */ Int32 rightIndex,
            /* [in] */ IInterface* cookie);

    private:
        CNetworkStatsService* mOwner;
    };

    /**
     * Default external settings that read from
     * {@link android.provider.Settings.Global}.
     */
    class DefaultNetworkStatsSettings
        : public Object
        , public INetworkStatsSettings
    {
    public:
        CAR_INTERFACE_DECL()

        DefaultNetworkStatsSettings(
            /* [in] */ IContext* context);

        CARAPI GetPollInterval(
            /* [out] */ Int64* result);

        CARAPI GetTimeCacheMaxAge(
            /* [out] */ Int64* result);

        CARAPI GetSampleEnabled(
            /* [out] */ Boolean* result);

        CARAPI GetReportXtOverDev(
            /* [out] */ Boolean* result);

        CARAPI GetDevConfig(
            /* [out] */ INetworkStatsSettingsConfig** result);

        CARAPI GetXtConfig(
            /* [out] */ INetworkStatsSettingsConfig** result);

        CARAPI GetUidConfig(
            /* [out] */ INetworkStatsSettingsConfig** result);

        CARAPI GetUidTagConfig(
            /* [out] */ INetworkStatsSettingsConfig** result);

        CARAPI GetGlobalAlertBytes(
            /* [in] */ Int64 def,
            /* [out] */ Int64* result);

        CARAPI GetDevPersistBytes(
            /* [in] */ Int64 def,
            /* [out] */ Int64* result);

        CARAPI GetXtPersistBytes(
            /* [in] */ Int64 def,
            /* [out] */ Int64* result);

        CARAPI GetUidPersistBytes(
            /* [in] */ Int64 def,
            /* [out] */ Int64* result);

        CARAPI GetUidTagPersistBytes(
            /* [in] */ Int64 def,
            /* [out] */ Int64* result);

    private:
        CARAPI_(Int64) GetGlobalInt64(
            /* [in] */ const String& name,
            /* [in] */ Int64 def);

        CARAPI_(Boolean) GetGlobalBoolean(
            /* [in] */ const String& name,
            /* [in] */ Boolean def);

    private:
        AutoPtr<IContentResolver> mResolver;
    };

    class InnerSub_INetworkStatsSession
        : public Object
        , public IINetworkStatsSession
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_INetworkStatsSession(
            /* [in] */ CNetworkStatsService* host);

        // @Override
        CARAPI GetSummaryForNetwork(
            /* [in] */ INetworkTemplate* templ,
            /* [in] */ Int64 start,
            /* [in] */ Int64 end,
            /* [out] */ INetworkStats** result);

        // @Override
        CARAPI GetHistoryForNetwork(
            /* [in] */ INetworkTemplate* templ,
            /* [in] */ Int32 fields,
            /* [out] */ INetworkStatsHistory** result);

        // @Override
        CARAPI GetSummaryForAllUid(
            /* [in] */ INetworkTemplate* templ,
            /* [in] */ Int64 start,
            /* [in] */ Int64 end,
            /* [in] */ Boolean includeTags,
            /* [out] */ INetworkStats** result);

        // @Override
        CARAPI GetHistoryForUid(
            /* [in] */ INetworkTemplate* templ,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 set,
            /* [in] */ Int32 tag,
            /* [in] */ Int32 fields,
            /* [out] */ INetworkStatsHistory** result);

        // @Override
        CARAPI Close();

    private:
        CARAPI_(AutoPtr<NetworkStatsCollection>) GetUidComplete();

        CARAPI_(AutoPtr<NetworkStatsCollection>) GetUidTagComplete();

    private:
        CNetworkStatsService* mHost;
        AutoPtr<NetworkStatsCollection> mUidComplete;
        AutoPtr<NetworkStatsCollection> mUidTagComplete;
    };

    class InnerSub_BaseNetworkObserver
        : public BaseNetworkObserver
    {
    public:
        InnerSub_BaseNetworkObserver(
            /* [in] */ CNetworkStatsService* host);

        // @Override
        CARAPI LimitReached(
            /* [in] */ const String& limitName,
            /* [in] */ const String& iface);

    private:
        CNetworkStatsService* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNetworkStatsService();

    CARAPI BindConnectivityManager(
        /* [in] */ IIConnectivityManager* connManager);

    CARAPI_(void) SystemReady();

    //@Override
    CARAPI OpenSession(
        /* [out] */ IINetworkStatsSession** comple);

    //@Override
    CARAPI GetNetworkTotalBytes(
        /* [in] */ INetworkTemplate* templ,
        /* [in] */ Int64 start,
        /* [in] */ Int64 end,
        /* [out] */ Int64* totalBytes);

    //@Override
    CARAPI GetDataLayerSnapshotForUid(
        /* [in] */ Int32 uid,
        /* [out] */ INetworkStats** datalayerOut);

    //@Override
    CARAPI GetMobileIfaces(
        /* [out, callee] */ ArrayOf<String>** mobileIfaces);

    //@Override
    CARAPI IncrementOperationCount(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 tag,
        /* [in] */ Int32 operationCount);

    //@Override
    CARAPI SetUidForeground(
        /* [in] */ Int32 uid,
        /* [in] */ Boolean uidForeground);

    //@Override
    CARAPI ForceUpdate();

    //@Override
    CARAPI AdvisePersistThreshold(
        /* [in] */ Int64 thresholdBytes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IINetworkManagementService* networkManager,
        /* [in] */ IIAlarmManager* alarmManager);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IINetworkManagementService* networkManager,
        /* [in] */ IIAlarmManager* alarmManager,
        /* [in] */ ITrustedTime* time,
        /* [in] */ IFile* systemDir,
        /* [in] */ INetworkStatsSettings* settings);

    CARAPI ToString(
        /* [out] */ String* info)
    {
        VALIDATE_NOT_NULL(info)
        return Object::ToString(info);
    }
protected:
    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ const ArrayOf<String>& args);

private:
    static CARAPI_(AutoPtr<IFile>) GetDefaultSystemDir();

    CARAPI_(AutoPtr<NetworkStatsRecorder>) BuildRecorder(
        /* [in] */ const String& prefix,
        /* [in] */ NetworkStatsSettingsConfig* config,
        /* [in] */ Boolean includeTags);

    CARAPI_(void) ShutdownLocked();

    CARAPI MaybeUpgradeLegacyStatsLocked();

    /**
     * Clear any existing {@link #ACTION_NETWORK_STATS_POLL} alarms, and
     * reschedule based on current {@link NetworkStatsSettings#getPollInterval()}.
     */
    CARAPI_(void) RegisterPollAlarmLocked();

    /**
     * Register for a global alert that is delivered through
     * {@link INetworkManagementEventObserver} once a threshold amount of data
     * has been transferred.
     */
    CARAPI_(void) RegisterGlobalAlert();

    /**
     * Return network summary, splicing between {@link #mDevStatsCached}
     * and {@link #mXtStatsCached} when appropriate.
     */
    CARAPI_(AutoPtr<INetworkStats>) InternalGetSummaryForNetwork(
        /* [in] */ INetworkTemplate* templ,
        /* [in] */ Int64 start,
        /* [in] */ Int64 end);

    /**
     * Return network history, splicing between {@link #mDevStatsCached}
     * and {@link #mXtStatsCached} when appropriate.
     */
    CARAPI_(AutoPtr<INetworkStatsHistory>) InternalGetHistoryForNetwork(
        /* [in] */ INetworkTemplate* templ,
        /* [in] */ Int32 fields);

    /**
     * Update {@link NetworkStatsRecorder} and {@link #mGlobalAlertBytes} to
     * reflect current {@link #mPersistThreshold} value. Always defers to
     * {@link Global} values when defined.
     */
    CARAPI_(void) UpdatePersistThresholds();

    CARAPI_(void) UpdateIfaces();

    /**
    * Inspect all current {@link NetworkState} to derive mapping from {@code
    * iface} to {@link NetworkStatsHistory}. When multiple {@link NetworkInfo}
    * are active on a single {@code iface}, they are combined under a single
    * {@link NetworkIdentitySet}.
    */
    CARAPI_(void) UpdateIfacesLocked();

    /**
     * Bootstrap initial stats snapshot, usually during {@link #systemReady()}
     * so we have baseline values without double-counting.
     */
    CARAPI_(void) BootstrapStatsLocked();

    CARAPI_(void) PerformPoll(
        /* [in] */ Int32 flags);

    CARAPI_(void) PerformPollLocked(
        /* [in] */ Int32 flags);

    /**
     * Sample recent statistics summary into {@link EventLog}.
     */
    CARAPI_(void) PerformSampleLocked();

    /**
    * Clean up {@link #mUidRecorder} after UID is removed.
    */
    CARAPI_(void) RemoveUidsLocked(
        /* [in] */ ArrayOf<Int32>* uids);

    /**
     * Clean up {@link #mUidRecorder} after user is removed.
     */
    CARAPI_(void) RemoveUserLocked(
        /* [in] */ Int32 userId);

    /**
     * Return snapshot of current UID statistics, including any
     * {@link TrafficStats#UID_TETHERING} and {@link #mUidOperations} values.
     */
    CARAPI GetNetworkStatsUidDetail(
        /* [out] */ INetworkStats** stats);

    /**
     * Return snapshot of current tethering statistics. Will return empty
     * {@link NetworkStats} if any problems are encountered.
     */
    CARAPI GetNetworkStatsTethering(
        /* [out] */ INetworkStats** stats);


    CARAPI AssertBandwidthControlEnabled();

    CARAPI_(Boolean) IsBandwidthControlEnabled();

    CARAPI_(void) HandleMsgPerformPoll(
        /* [in] */ Int32 flags);

    CARAPI_(void) HandleMsgUpdateIfaces();

    CARAPI_(void) HandleMsgRegisterGlobalAlert();

    static CARAPI_(AutoPtr<NetworkIdentitySet>) FindOrCreateNetworkIdentitySet(
        /* [in] */ IArrayMap* map,
        /* [in] */ IInterface* key);

public:
    // @VisibleForTesting
    static const String ACTION_NETWORK_STATS_POLL;
    static const String ACTION_NETWORK_STATS_UPDATED;

private:
    static const String TAG;
    static const Boolean LOGV = FALSE;

    static const Int32 MSG_PERFORM_POLL;// = 1;
    static const Int32 MSG_UPDATE_IFACES;// = 2;
    static const Int32 MSG_REGISTER_GLOBAL_ALERT;// = 3;

    /** Flags to control detail level of poll event. */
    static const Int32 FLAG_PERSIST_NETWORK = 0x1;
    static const Int32 FLAG_PERSIST_UID = 0x2;
    static const Int32 FLAG_PERSIST_ALL = FLAG_PERSIST_NETWORK | FLAG_PERSIST_UID;
    static const Int32 FLAG_PERSIST_FORCE = 0x100;

    static const String TAG_NETSTATS_ERROR;

    static const String PREFIX_DEV;
    static const String PREFIX_XT;
    static const String PREFIX_UID;
    static const String PREFIX_UID_TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<IINetworkManagementService> mNetworkManager;
    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<ITrustedTime> mTime;
    AutoPtr<ITelephonyManager> mTeleManager;
    AutoPtr<INetworkStatsSettings> mSettings;
    AutoPtr<IFile> mSystemDir;
    AutoPtr<IFile> mBaseDir;
    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    AutoPtr<IIConnectivityManager> mConnManager;
    AutoPtr<IPendingIntent> mPollIntent;

    AutoPtr<IObject> mStatsLock;
    /** Set of currently active ifaces. */
    AutoPtr<IArrayMap> mActiveIfaces;
    AutoPtr<IArrayMap> mActiveUidIfaces;
    /** Current default active iface. */
    String mActiveIface;
    /** Set of any ifaces associated with mobile networks since boot. */
    AutoPtr<ArrayOf<String> > mMobileIfaces;

    AutoPtr<DropBoxNonMonotonicObserver> mNonMonotonicObserver;

    AutoPtr<NetworkStatsRecorder> mDevRecorder;
    AutoPtr<NetworkStatsRecorder> mXtRecorder;
    AutoPtr<NetworkStatsRecorder> mUidRecorder;
    AutoPtr<NetworkStatsRecorder> mUidTagRecorder;

    /** Cached {@link #mDevRecorder} stats. */
    AutoPtr<NetworkStatsCollection> mDevStatsCached;
    /** Cached {@link #mXtRecorder} stats. */
    AutoPtr<NetworkStatsCollection> mXtStatsCached;

    /** Current counter sets for each UID. */
    HashMap<Int32, Int32> mActiveUidCounterSet;

    /** Data layer operation counters for splicing into other structures. */
    AutoPtr<INetworkStats> mUidOperations;// = new NetworkStats(0L, 10);

    AutoPtr<IHandler> mHandler;
    AutoPtr<IHandlerCallback> mHandlerCallback;

    Boolean mSystemReady;
    Int64 mPersistThreshold;
    Int64 mGlobalAlertBytes;

    AutoPtr<BroadcastReceiver> mConnReceiver;
    AutoPtr<BroadcastReceiver> mTetherReceiver;
    AutoPtr<BroadcastReceiver> mPollReceiver;
    AutoPtr<BroadcastReceiver> mRemovedReceiver;
    AutoPtr<BroadcastReceiver> mUserReceiver;
    AutoPtr<BroadcastReceiver> mShutdownReceiver;

    AutoPtr<IINetworkManagementEventObserver> mAlertObserver;

    Int32 mLastPhoneState;
    Int32 mLastPhoneNetworkType;

    //AutoPtr<PhoneListener> mPhoneListener;

};

} // namespace Net
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NET_CNETWORKSTATSSERVICE_H__
