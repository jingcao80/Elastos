
#ifndef __ELASTOS_DROID_SERVER_NET_CNETWORKPOLICYMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_NET_CNETWORKPOLICYMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_Net_CNetworkPolicyManagerService.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.Droid.App.h>
#include <elastos/droid/server/net/BaseNetworkObserver.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Core::IArrayOf;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IINotificationManager;
using Elastos::Droid::App::IIProcessObserver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IIConnectivityManager;
using Elastos::Droid::Net::IINetworkManagementEventObserver;
using Elastos::Droid::Net::IINetworkPolicyListener;
using Elastos::Droid::Net::IINetworkPolicyManager;
using Elastos::Droid::Net::IINetworkStatsService;
using Elastos::Droid::Net::INetworkIdentity;
using Elastos::Droid::Net::INetworkPolicy;
using Elastos::Droid::Net::INetworkQuotaInfo;
using Elastos::Droid::Net::INetworkState;
using Elastos::Droid::Net::INetworkTemplate;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IHandlerCallback;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::IIdleHandler;
using Elastos::Droid::Os::ILowPowerModeListener;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IPowerManagerInternal;
using Elastos::Droid::Os::IRemoteCallbackList;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::Utility::ITrustedTime;
using Elastos::IO::IFile;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::HashSet;
using Org::Xmlpull::V1::IXmlPullParser;
using Org::Xmlpull::V1::IXmlSerializer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

class CNetworkPolicyManagerServiceAlertObserber;

CarClass(CNetworkPolicyManagerService)
    , public Object
    , public IBinder
    , public IINetworkPolicyManager
{
    friend class CProcessObserver;

public:
    class MyHandlerCallback
        : public Object
        , public IHandlerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyHandlerCallback(
            /* [in] */ CNetworkPolicyManagerService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg,
            /* [out] */ Boolean* result);

    private:
        CNetworkPolicyManagerService* mHost;
    };

private:
    class ScreenReceiver
        : public BroadcastReceiver
    {
    public:
        ScreenReceiver(
            /* [in] */ CNetworkPolicyManagerService* owner);

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info);

    private:
        CNetworkPolicyManagerService* mOwner;
    };

    class PackageReceiver
        : public BroadcastReceiver
    {
    public:
        PackageReceiver(
            /* [in] */ CNetworkPolicyManagerService* owner) : mOwner(owner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CNetworkPolicyManagerService::PackageReceiver: ")

    private:
        CNetworkPolicyManagerService* mOwner;
    };

    class UidRemovedReceiver
        : public BroadcastReceiver
    {
    public:
        UidRemovedReceiver(
            /* [in] */ CNetworkPolicyManagerService* owner) : mOwner(owner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CNetworkPolicyManagerService::UidRemovedReceiver: ")

    private:
        CNetworkPolicyManagerService* mOwner;
    };

    class UserReceiver
        : public BroadcastReceiver
    {
    public:
        UserReceiver(
            /* [in] */ CNetworkPolicyManagerService* owner) : mOwner(owner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CNetworkPolicyManagerService::UserReceiver: ")

    private:
        CNetworkPolicyManagerService* mOwner;
    };

    /**
     * Receiver that watches for {@link INetworkStatsService} updates, which we
     * use to check against {@link NetworkPolicy#warningBytes}.
     */
    class StatsReceiver
        : public BroadcastReceiver
    {
    public:
        StatsReceiver(
            /* [in] */ CNetworkPolicyManagerService* owner) : mOwner(owner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CNetworkPolicyManagerService::StatsReceiver: ")
    private:
        CNetworkPolicyManagerService* mOwner;
    };

    /**
     * Receiver that watches for {@link Notification} control of
     * {@link #mRestrictBackground}.
     */
    class AllowReceiver
        : public BroadcastReceiver
    {
    public:
        AllowReceiver(
            /* [in] */ CNetworkPolicyManagerService* owner) : mOwner(owner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CNetworkPolicyManagerService::AllowReceiver: ")
    private:
        CNetworkPolicyManagerService* mOwner;
    };

    /**
     * Receiver that watches for {@link Notification} control of
     * {@link NetworkPolicy#lastWarningSnooze}.
     */
    class SnoozeWarningReceiver
        : public BroadcastReceiver
    {
    public:
        SnoozeWarningReceiver(
            /* [in] */ CNetworkPolicyManagerService* owner) : mOwner(owner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CNetworkPolicyManagerService::SnoozeWarningReceiver: ")
    private:
        CNetworkPolicyManagerService* mOwner;

    };

    /**
    * Receiver that watches for {@link WifiConfiguration} to be changed.
    */
    class WifiConfigReceiver
        : public BroadcastReceiver
    {
    public:
        WifiConfigReceiver(
            /* [in] */ CNetworkPolicyManagerService* owner) : mOwner(owner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CNetworkPolicyManagerService::WifiConfigReceiver: ")
    private:
        CNetworkPolicyManagerService* mOwner;
    };

    /**
    * Receiver that watches {@link WifiInfo} state changes to infer metered
    * state. Ignores hints when policy is user-defined.
    */
    class WifiStateReceiver
        : public BroadcastReceiver
    {
    public:
        WifiStateReceiver(
            /* [in] */ CNetworkPolicyManagerService* owner) : mOwner(owner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CNetworkPolicyManagerService::WifiStateReceiver: ")
    private:
        CNetworkPolicyManagerService* mOwner;
    };

    /**
     * Receiver that watches for {@link IConnectivityManager} to claim network
     * interfaces. Used to apply {@link NetworkPolicy} to matching networks.
     */
    class ConnReceiver
        : public BroadcastReceiver
    {
    public:
        ConnReceiver(
            /* [in] */ CNetworkPolicyManagerService* owner) : mOwner(owner)
        {}

        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CNetworkPolicyManagerService::ConnReceiver: ")
    private:
        CNetworkPolicyManagerService* mOwner;
    };

    class InnerSub_LowPowerModeListener
        : public Object
        , public ILowPowerModeListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_LowPowerModeListener(
            /* [in] */ CNetworkPolicyManagerService* host,
            /* [in] */ IObject* rulesLock,
            /* [in] */ Boolean& restrictPower);

        // @Override
        CARAPI OnLowPowerModeChanged(
            /* [in] */ Boolean enabled);

    private:
        CNetworkPolicyManagerService* mHost;
        AutoPtr<IObject> mRulesLock;
        Boolean& mRestrictPower;
    };

    class InnerSub_IProcessObserver
        : public Object
        , public IIProcessObserver
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        InnerSub_IProcessObserver(
            /* [in] */ CNetworkPolicyManagerService* host,
            /* [in] */ IObject* rulesLock,
            /* [in] */ ISparseArray* uidPidState);

        // @Override
        CARAPI OnForegroundActivitiesChanged(
            /* [in] */ Int32 pid,
            /* [in] */ Int32 uid,
            /* [in] */ Boolean foregroundActivities);

        // @Override
        CARAPI OnProcessStateChanged(
            /* [in] */ Int32 pid,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 procState);

        // @Override
        CARAPI OnProcessDied(
            /* [in] */ Int32 pid,
            /* [in] */ Int32 uid);

        // for IBinder
        CARAPI ToString(
            /* [out] */ String* info);

    private:
        CNetworkPolicyManagerService* mHost;
        AutoPtr<IObject> mRulesLock;
        AutoPtr<ISparseArray> mUidPidState;
    };

    class InnerSub_AlertObserver
        : public BaseNetworkObserver
    {
    public:
        InnerSub_AlertObserver(
            /* [in] */ CNetworkPolicyManagerService* host);

        // @Override
        CARAPI LimitReached(
            /* [in] */ const String& limitName,
            /* [in] */ const String& iface);

    private:
        CNetworkPolicyManagerService* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CNetworkPolicyManagerService();

    ~CNetworkPolicyManagerService();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIActivityManager* activityManager,
        /* [in] */ IIPowerManager* powerManager,
        /* [in] */ IINetworkStatsService* networkStats,
        /* [in] */ IINetworkManagementService* networkManagement);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIActivityManager* activityManager,
        /* [in] */ IIPowerManager* powerManager,
        /* [in] */ IINetworkStatsService* networkStats,
        /* [in] */ IINetworkManagementService* networkManagement,
        /* [in] */ ITrustedTime* time,
        /* [in] */ IFile* systemDir,
        /* [in] */ Boolean suppressDefaultPolicy);

    CARAPI BindConnectivityManager(
        /* [in] */ IIConnectivityManager* connManager);

    CARAPI BindNotificationManager(
        /* [in] */ IINotificationManager* notifManager);

    CARAPI SystemReady();

    CARAPI SetUidPolicy(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 policy);

    CARAPI GetUidPolicy(
        /* [in] */ Int32 uid,
        /* [out] */ Int32* policy);

    CARAPI GetUidsWithPolicy(
        /* [in] */ Int32 policy,
        /* [out, callee] */ ArrayOf<Int32>** uids);

    CARAPI RegisterListener(
        /* [in] */ IINetworkPolicyListener* listener);

    CARAPI UnregisterListener(
        /* [in] */ IINetworkPolicyListener* listener);

    CARAPI SetNetworkPolicies(
        /* [in] */ ArrayOf<INetworkPolicy*>* policies);

    CARAPI GetNetworkPolicies(
        /* [out, callee] */ ArrayOf<INetworkPolicy*>** policies);

    CARAPI SnoozeLimit(
        /* [in] */ INetworkTemplate* templ);

    CARAPI SetRestrictBackground(
        /* [in] */ Boolean restrictBackground);

    CARAPI GetRestrictBackground(
        /* [out] */ Boolean* RestrictBackground);

    CARAPI GetNetworkQuotaInfo(
        /* [in] */ INetworkState* state,
        /* [out] */ INetworkQuotaInfo** info);

    CARAPI IsNetworkMetered(
        /* [in] */ INetworkState* state,
        /* [out] */ Boolean* metered);

    CARAPI IsUidForeground(
        /* [in] */ Int32 uid,
        /* [out] */ Boolean* isUidForeground);

    CARAPI AddIdleHandler(
        /* [in] */ IIdleHandler* handler);

    // package
    /**
     * Check {@link NetworkPolicy} against current {@link INetworkStatsService}
     * to show visible notifications as needed.
     */
    CARAPI_(void) UpdateNotificationsLocked();

    // package
    /**
     * Proactively control network data connections when they exceed
     * {@link NetworkPolicy#limitBytes}.
     */
    CARAPI_(void) UpdateNetworkEnabledLocked();

    // package
    /**
     * Examine all connected {@link NetworkState}, looking for
     * {@link NetworkPolicy} that need to be enforced. When matches found, set
     * remaining quota based on usage cycle and historical stats.
     */
    CARAPI_(void) UpdateNetworkRulesLocked();

    // package
    CARAPI WritePolicyLocked();

    // @Override
    CARAPI AddUidPolicy(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 policy);

    // @Override
    CARAPI RemoveUidPolicy(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 policy);

    // @Override
    CARAPI GetPowerSaveAppIdWhitelist(
        /* [out, callee] */ ArrayOf<Int32>** result);

    // package
    /**
     * Remove any policies associated with given {@link UserHandle}, persisting
     * if any changes are made.
     */
    CARAPI_(void) RemovePoliciesForUserLocked(
        /* [in] */ Int32 userId);

    // package
    CARAPI_(void) AddNetworkPolicyLocked(
        /* [in] */ INetworkPolicy* policy);

    // package
    CARAPI PerformSnooze(
        /* [in] */ INetworkTemplate* templ,
        /* [in] */ Int32 type);

    // package
    CARAPI_(Boolean) IsUidForegroundLocked(
        /* [in] */ Int32 uid);

    // package
    /**
     * Process state of PID changed; recompute state at UID level. If
     * changed, will trigger {@link #updateRulesForUidLocked(int)}.
     */
    CARAPI_(void) ComputeUidStateLocked(
        /* [in] */ Int32 uid);

    // package
    /**
     * Update rules that might be changed by {@link #mRestrictBackground}
     * or {@link #mRestrictPower} value.
     */
    CARAPI_(void) UpdateRulesForScreenLocked();

    // package
    /**
     * Update rules that might be changed by {@link #mRestrictBackground}
     * or {@link #mRestrictPower} value.
     */
    CARAPI_(void) UpdateRulesForGlobalChangeLocked(
        /* [in] */ Boolean restrictedNetworksChanged);

    // package
    CARAPI_(void) UpdateRulesForUidLocked(
        /* [in] */ Int32 uid);

    // package
    /**
     * Try refreshing {@link #mTime} when stale.
     */
    CARAPI_(void) MaybeRefreshTrustedTime();

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
    static CARAPI_(AutoPtr<IFile>) GetSystemDir();

    /**
    * Test if given {@link NetworkTemplate} is relevant to user based on
    * current device state, such as when
    * {@link TelephonyManager#getSubscriberId()} matches. This is regardless of
    * data connection status.
    */
    CARAPI_(Boolean) IsTemplateRelevant(
        /* [in] */ INetworkTemplate* templ);

    /**
     * Notify that given {@link NetworkTemplate} is over
     * {@link NetworkPolicy#limitBytes}, potentially showing dialog to user.
     */
    CARAPI_(void) NotifyOverLimitLocked(
        /* [in] */ INetworkTemplate* templ);

    CARAPI_(void) NotifyUnderLimitLocked(
        /* [in] */ INetworkTemplate* templ);

    /**
     * Build unique tag that identifies an active {@link NetworkPolicy}
     * notification of a specific type, like {@link #TYPE_LIMIT}.
     */
    CARAPI_(String) BuildNotificationTag(
        /* [in] */ INetworkPolicy* policy,
        /* [in] */ Int32 type);

    /**
     * Show notification for combined {@link NetworkPolicy} and specific type,
     * like {@link #TYPE_LIMIT}. Okay to call multiple times.
     */
    CARAPI_(void) EnqueueNotification(
        /* [in] */ INetworkPolicy* policy,
        /* [in] */ Int32 type,
        /* [in] */ Int64 totalBytes);

    /**
     * Show ongoing notification to reflect that {@link #mRestrictBackground}
     * has been enabled.
     */
    CARAPI_(void) EnqueueRestrictedNotification(
        /* [in] */ const String& tag);

    CARAPI_(void) CancelNotification(
        /* [in] */ const String& tag);

    /**
     * Control {@link IConnectivityManager#setPolicyDataEnable(int, boolean)}
     * for the given {@link NetworkTemplate}.
     */
    CARAPI_(void) SetNetworkTemplateEnabled(
        /* [in] */ INetworkTemplate* templ,
        /* [in] */ Boolean enabled);

    /**
     * Once any {@link #mNetworkPolicy} are loaded from disk, ensure that we
     * have at least a default mobile policy defined.
     */
    CARAPI_(void) EnsureActiveMobilePolicyLocked();

    CARAPI ReadPolicyLocked();

    /**
     * Upgrade legacy background data flags, notifying listeners of one last
     * change to always-true.
     */
    CARAPI_(void) UpgradeLegacyBackgroundData();

    CARAPI_(void) SetUidPolicyUncheckedLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 policy,
        /* [in] */ Boolean persist);

    CARAPI_(AutoPtr<INetworkPolicy>) FindPolicyForNetworkLocked(
        /* [in] */ INetworkIdentity* ident);

    CARAPI_(AutoPtr<INetworkQuotaInfo>) GetNetworkQuotaInfoUnchecked(
        /* [in] */ INetworkState* state);

    CARAPI_(void) UpdateScreenOn();

    static CARAPI_(Boolean) IsUidValidForRules(
        /* [in] */ Int32 uid);

    CARAPI_(void) SetInterfaceQuota(
        /* [in] */ const String& iface,
        /* [in] */ Int64 quotaBytes);

    CARAPI_(void) RemoveInterfaceQuota(
        /* [in] */ const String& iface);

    CARAPI_(void) SetUidNetworkRules(
        /* [in] */ Int32 uid,
        /* [in] */ Boolean rejectOnQuotaInterfaces);

    /**
     * Control {@link IConnectivityManager#setPolicyDataEnable(int, boolean)}.
     */
    CARAPI_(void) SetPolicyDataEnable(
        /* [in] */ Int32 networkType,
        /* [in] */ Boolean enabled);

    CARAPI_(Int64) GetTotalBytes(
        /* [in] */ INetworkTemplate* templ,
        /* [in] */ Int64 start,
        /* [in] */ Int64 end);

    CARAPI_(Boolean) IsBandwidthControlEnabled();

    CARAPI_(Int64) CurrentTimeMillis();

    static CARAPI_(AutoPtr<IIntent>) BuildAllowBackgroundDataIntent();

    static CARAPI_(AutoPtr<IIntent>) BuildSnoozeWarningIntent(
        /* [in] */ INetworkTemplate* templ);

    static CARAPI_(AutoPtr<IIntent>) BuildNetworkOverLimitIntent(
        /* [in] */ INetworkTemplate* templ);

    static CARAPI_(AutoPtr<IIntent>) BuildViewDataUsageIntent(
        /* [in] */ INetworkTemplate* templ);

    static CARAPI_(void) CollectKeys(
        /* [in] */ ISparseInt32Array* source,
        /* [in] */ ISparseBooleanArray* target);

    static CARAPI_(void) DumpHashMap(
        /* [in] */ IPrintWriter* fout,
        /* [in] */ HashMap<Int32, Boolean>* value);

    CARAPI_(void) HandleMsgRulesChanged(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 uidRules);

    CARAPI_(void) HandleMsgMeteredIfacesChanged(
        /* [in] */ IArrayOf* meteredIfaces);

    CARAPI_(void) HandleMsgLimitReached(
        /* [in] */ const String& iface);

    CARAPI_(void) HandleMsgRestrictBackgroundChanged(
        /* [in] */ Boolean restrictBackground);

    CARAPI_(void) HandleMsgAdvisePersistThreshold(
        /* [in] */ Int64 lowestRule);

    CARAPI_(void) HandleMsgScreenOnChanged();

    static CARAPI_(void) DumpSparseInt32Array(
        /* [in] */ IPrintWriter* fout,
        /* [in] */ ISparseInt32Array* value);

public:
    // @VisibleForTesting
    static const String ACTION_NETWORK_STATS_POLL;
    static const String ACTION_NETWORK_STATS_UPDATED;

    static const Int32 TYPE_WARNING;
    static const Int32 TYPE_LIMIT;
    static const Int32 TYPE_LIMIT_SNOOZED;

    // package
    /* volatile */ Boolean mScreenOn;
    /* volatile */ Boolean mRestrictBackground;
    /* volatile */ Boolean mRestrictPower;

    // package
    /** Defined network policies. */
    /* const */ AutoPtr<IArrayMap> mNetworkPolicy;

    // package
    /** Defined UID policies. */
    /* const */ HashMap<Int32, Int32> mUidPolicy;

    // package
    /** Foreground at both UID and PID granularity. */
    /* const */ AutoPtr<ISparseArray> mUidPidState;

private:
    static const String TAG;
    static const Boolean LOGD;
    static const Boolean LOGV;

    static const Int32 VERSION_INIT;
    static const Int32 VERSION_ADDED_SNOOZE;
    static const Int32 VERSION_ADDED_RESTRICT_BACKGROUND;
    static const Int32 VERSION_ADDED_METERED;
    static const Int32 VERSION_SPLIT_SNOOZE;
    static const Int32 VERSION_ADDED_TIMEZONE;
    static const Int32 VERSION_ADDED_INFERRED;
    static const Int32 VERSION_SWITCH_APP_ID;
    static const Int32 VERSION_ADDED_NETWORK_ID;
    static const Int32 VERSION_SWITCH_UID;
    static const Int32 VERSION_LATEST;

    static const String TAG_POLICY_LIST;
    static const String TAG_NETWORK_POLICY;
    static const String TAG_UID_POLICY;
    static const String TAG_APP_POLICY;

    static const String ATTR_VERSION;
    static const String ATTR_RESTRICT_BACKGROUND;
    static const String ATTR_NETWORK_TEMPLATE;
    static const String ATTR_SUBSCRIBER_ID;
    static const String ATTR_NETWORK_ID;
    static const String ATTR_CYCLE_DAY;
    static const String ATTR_CYCLE_TIMEZONE;
    static const String ATTR_WARNING_BYTES;
    static const String ATTR_LIMIT_BYTES;
    static const String ATTR_LAST_SNOOZE;
    static const String ATTR_LAST_WARNING_SNOOZE;
    static const String ATTR_LAST_LIMIT_SNOOZE;
    static const String ATTR_METERED;
    static const String ATTR_INFERRED;
    static const String ATTR_UID;
    static const String ATTR_APP_ID;
    static const String ATTR_POLICY;

    static const String TAG_ALLOW_BACKGROUND;

    static const String ACTION_ALLOW_BACKGROUND;
    static const String ACTION_SNOOZE_WARNING;

    static const Int64 TIME_CACHE_MAX_AGE;

    static const Int32 MSG_RULES_CHANGED;// = 1;
    static const Int32 MSG_METERED_IFACES_CHANGED;// = 2;
    static const Int32 MSG_LIMIT_REACHED;// = 5;
    static const Int32 MSG_RESTRICT_BACKGROUND_CHANGED;// = 6;
    static const Int32 MSG_ADVISE_PERSIST_THRESHOLD;// = 7;
    static const Int32 MSG_SCREEN_ON_CHANGED;// = 8;

    AutoPtr<IContext> mContext;
    AutoPtr<IIActivityManager> mActivityManager;
    AutoPtr<IIPowerManager> mPowerManager;
    AutoPtr<IINetworkStatsService> mNetworkStats;
    AutoPtr<IINetworkManagementService> mNetworkManager;
    AutoPtr<ITrustedTime> mTime;

    AutoPtr<IIConnectivityManager> mConnManager;
    AutoPtr<IINotificationManager> mNotifManager;
    AutoPtr<IPowerManagerInternal> mPowerManagerInternal;

    AutoPtr<IObject> mRulesLock;

    Boolean mSuppressDefaultPolicy;

    /** Currently active network rules for ifaces. */
    /* const */ AutoPtr<IArrayMap> mNetworkRules;

    /** Currently derived rules for each UID. */
    /* const */ AutoPtr<ISparseInt32Array> mUidRules;

    /** UIDs that have been white-listed to always be able to have network access in
     * power save mode. */
    /* const */ AutoPtr<ISparseBooleanArray> mPowerSaveWhitelistAppIds;

    /** Set of ifaces that are metered. */
    AutoPtr<IArraySet> mMeteredIfaces;
    /** Set of over-limit templates that have been notified. */
    /* const */ AutoPtr<IArraySet> mOverLimitNotified;

    /** Set of currently active {@link Notification} tags. */
    /* const */ AutoPtr<IArraySet> mActiveNotifs;

    /** Foreground at both UID and PID granularity. */
    /* const */ AutoPtr<ISparseInt32Array> mUidState;

    /** The current maximum process state that we are considering to be foreground. */
    Int32 mCurForegroundState;

    AutoPtr<IRemoteCallbackList> mListeners;

    AutoPtr<IHandler> mHandler;

    AutoPtr<IHandlerCallback> mHandlerCallback;

    AutoPtr<IAtomicFile> mPolicyFile;

    AutoPtr<IIProcessObserver> mProcessObserver;
    AutoPtr<ScreenReceiver> mScreenReceiver;
    AutoPtr<PackageReceiver> mPackageReceiver;
    AutoPtr<UidRemovedReceiver> mUidRemovedReceiver;
    AutoPtr<UserReceiver> mUserReceiver;
    AutoPtr<StatsReceiver> mStatsReceiver;
    AutoPtr<AllowReceiver> mAllowReceiver;
    AutoPtr<SnoozeWarningReceiver> mSnoozeWarningReceiver;
    AutoPtr<WifiConfigReceiver> mWifiConfigReceiver;
    AutoPtr<WifiStateReceiver> mWifiStateReceiver;

    /**
     * Observer that watches for {@link INetworkManagementService} alerts.
     */
    AutoPtr<IINetworkManagementEventObserver> mAlertObserver;

    AutoPtr<ConnReceiver> mConnReceiver;

    friend class CNetworkPolicyManagerServiceAlertObserber;
};

} // namespace Net
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_NET_CNETWORKPOLICYMANAGERSERVICE_H__
