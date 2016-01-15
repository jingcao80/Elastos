
#ifndef __ELASTOS_DROID_SERVER_CWIFISERVICE_H__
#define __ELASTOS_DROID_SERVER_CWIFISERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CWifiService.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/net/wifi/WifiStateMachine.h"
#include "elastos/droid/net/wifi/WifiWatchdogStateMachine.h"
#include "elastos/droid/os/HandlerBase.h"
#include "util/AsyncChannel.h"
#include "R.h"
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Droid::R;
using Elastos::Droid::App::IAlarmManager;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::IDhcpInfo;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::IWifiInfo;
using Elastos::Droid::Wifi::IScanResult;
using Elastos::Droid::Wifi::WifiStateMachine;
using Elastos::Droid::Wifi::WifiWatchdogStateMachine;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::HandlerBase;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::Utility::AsyncChannel;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CWifiService)
{
private:
    /**
     * Handles client connections
     */
    class AsyncServiceHandler
        : public HandlerBase
    {
    public:
        AsyncServiceHandler(
            /* [in] */ ILooper* l,
            /* [in] */ CWifiService* owner)
            : HandlerBase(l)
            , mOwner(owner)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CWifiService* mOwner;
    };

    /**
     * Handles interaction with WifiStateMachine
     */
    class WifiStateMachineHandler
        : public HandlerBase
    {
    public:
        WifiStateMachineHandler(
            /* [in] */ ILooper* l,
            /* [in] */ CWifiService* owner);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI Clear();
    private:
        AutoPtr<AsyncChannel> mWsmChannel;
        CWifiService* mOwner;
    };

    class DeathRecipient
        : public ElRefBase
        , public IProxyDeathRecipient
    {
    public:
        DeathRecipient(
            /* [in] */ Int32 mode,
            /* [in] */ const String& tag,
            /* [in] */ IBinder* binder,
            /* [in] */ IWorkSource* ws);

        CAR_INTERFACE_DECL()

        CARAPI_(void) UnlinkDeathRecipient();

    public:
        String mTag;
        Int32 mMode;
        AutoPtr<IBinder> mBinder;
        AutoPtr<IWorkSource> mWorkSource;
    };

    class WifiLock : public DeathRecipient
    {
    public:
        WifiLock(
            /* [in] */ Int32 lockMode,
            /* [in] */ const String& tag,
            /* [in] */ IBinder* binder,
            /* [in] */ IWorkSource* ws,
            /* [in] */ CWifiService* owner);

        CARAPI ProxyDied();

        CARAPI_(String) ToString();

    private:
        CWifiService* mOwner;
    };

    class LockList : public ElRefBase
    {
        friend class CWifiService;

    private:
        LockList(
            /* [in] */ CWifiService* owner)
            : mOwner(owner)
        {}

        CARAPI_(Boolean) HasLocks();

        CARAPI_(Int32) GetStrongestLockMode();

        CARAPI_(void) AddLock(
            /* [in] */ WifiLock* lock);

        CARAPI_(AutoPtr<WifiLock>) RemoveLock(
            /* [in] */ IBinder* binder);

        CARAPI_(List< AutoPtr<WifiLock> >::Iterator) FindLockByBinder(
            /* [in] */ IBinder* binder);

        CARAPI_(void) Dump(
            /* [in] */ IPrintWriter* pw);

    private:
        List< AutoPtr<WifiLock> > mList;
        Object mLock;
        CWifiService* mOwner;
    };

    class Multicaster : public DeathRecipient
    {
    public:
        Multicaster(
            /* [in] */ const String& tag,
            /* [in] */ IBinder* binder,
            /* [in] */ CWifiService* owner);

        CARAPI ProxyDied();

        CARAPI_(String) ToString();

        CARAPI_(Int32) GetUid() { return mMode; }

    private:
        CWifiService* mOwner;
    };

    class NotificationEnabledSettingObserver : public ContentObserver
    {
    public:
        NotificationEnabledSettingObserver(
            /* [in] */ IHandler* handler,
            /* [in] */ CWifiService* owner)
            : ContentObserver(handler)
            , mOwner(owner)
        {}

        CARAPI Register();

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        CARAPI_(Boolean) GetValue();
        CWifiService* mOwner;
    };

    class AirplaneModeChangedBroadcastReceiver : public BroadcastReceiver
    {
    public:
        AirplaneModeChangedBroadcastReceiver(
            /* [in] */ CWifiService* owner);

        //Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CWifiService::AirplaneModeChangedBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CWifiService* mWifiService;
    };

    class StateChangedBroadcastReceiver : public BroadcastReceiver
    {
    public:
        StateChangedBroadcastReceiver(
            /* [in] */ CWifiService* owner);

        //Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CWifiService::StateChangedBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    private:
        CWifiService* mWifiService;
    };

    class WifiServiceBroadcastReceiver : public BroadcastReceiver
    {
    public:
        WifiServiceBroadcastReceiver(
            /* [in] */ CWifiService* owner);

        //Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CWifiService::WifiServiceBroadcastReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }

        /**
         * Determines whether the Wi-Fi chipset should stay awake or be put to
         * sleep. Looks at the setting for the sleep policy and the current
         * conditions.
         *
         * @see #shouldDeviceStayAwake(int, int)
         */
        CARAPI_(Boolean) ShouldWifiStayAwake(
            /* [in] */ Int32 stayAwakeConditions,
            /* [in] */ Int32 pluggedType);
    private:
        /**
         * Determine whether the bit value corresponding to {@code pluggedType} is set in
         * the bit string {@code stayAwakeConditions}. Because a {@code pluggedType} value
         * of {@code 0} isn't really a plugged type, but rather an indication that the
         * device isn't plugged in at all, there is no bit value corresponding to a
         * {@code pluggedType} value of {@code 0}. That is why we shift by
         * {@code pluggedType - 1} instead of by {@code pluggedType}.
         * @param stayAwakeConditions a bit string specifying which "plugged types" should
         * keep the device (and hence Wi-Fi) awake.
         * @param pluggedType the type of plug (USB, AC, or none) for which the check is
         * being made
         * @return {@code true} if {@code pluggedType} indicates that the device is
         * supposed to stay awake, {@code false} otherwise.
         */
        CARAPI_(Boolean) ShouldDeviceStayAwake(
            /* [in] */ Int32 stayAwakeConditions,
            /* [in] */ Int32 pluggedType);
    private:
        CWifiService* mWifiService;
    };

public:
    CWifiService();

    ~CWifiService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Check if Wi-Fi needs to be enabled and start
     * if needed
     *
     * This function is used only at boot time
     */
    CARAPI_(void) CheckAndStartWifi();

    /**
     * see {@link android.net.wifi.WifiManager#pingSupplicant()}
     * @return {@code true} if the operation succeeds, {@code false} otherwise
     */
    CARAPI PingSupplicant(
        /* [out] */ Boolean* result);

    /**
     * see {@link android.net.wifi.WifiManager#startScan()}
     */
    CARAPI StartScan(
        /* [in] */ Boolean forceActive);

    /**
     * see {@link android.net.wifi.WifiManager#setWifiEnabled(boolean)}
     * @param enable {@code true} to enable, {@code false} to disable.
     * @return {@code true} if the enable/disable operation was
     *         started or is already in the queue.
     */
    /*synchronized*/
    CARAPI SetWifiEnabled(
        /* [in] */ Boolean enable,
        /* [out] */ Boolean* result);

    /**
     * see {@link WifiManager#getWifiState()}
     * @return One of {@link WifiManager#WIFI_STATE_DISABLED},
     *         {@link WifiManager#WIFI_STATE_DISABLING},
     *         {@link WifiManager#WIFI_STATE_ENABLED},
     *         {@link WifiManager#WIFI_STATE_ENABLING},
     *         {@link WifiManager#WIFI_STATE_UNKNOWN}
     */
    CARAPI GetWifiEnabledState(
        /* [our] */ Int32* result);

    /**
     * see {@link android.net.wifi.WifiManager#setWifiApEnabled(WifiConfiguration, boolean)}
     * @param wifiConfig SSID, security and channel details as
     *        part of WifiConfiguration
     * @param enabled true to enable and false to disable
     */
    CARAPI SetWifiApEnabled(
        /* [in] */ IWifiConfiguration* wifiConfig,
        /* [in] */ Boolean enabled);

    /**
     * see {@link WifiManager#getWifiApState()}
     * @return One of {@link WifiManager#WIFI_AP_STATE_DISABLED},
     *         {@link WifiManager#WIFI_AP_STATE_DISABLING},
     *         {@link WifiManager#WIFI_AP_STATE_ENABLED},
     *         {@link WifiManager#WIFI_AP_STATE_ENABLING},
     *         {@link WifiManager#WIFI_AP_STATE_FAILED}
     */
    CARAPI GetWifiApEnabledState(
        /* [out] */ Int32* result);

    /**
     * see {@link WifiManager#getWifiApConfiguration()}
     * @return soft access point configuration
     */
    CARAPI GetWifiApConfiguration(
        /* [out] */ IWifiConfiguration** result);

    /**
     * see {@link WifiManager#setWifiApConfiguration(WifiConfiguration)}
     * @param wifiConfig WifiConfiguration details for soft access point
     */
    CARAPI SetWifiApConfiguration(
        /* [in] */ IWifiConfiguration* wifiConfig);

    /**
     * see {@link android.net.wifi.WifiManager#disconnect()}
     */
    CARAPI Disconnect();

    /**
     * see {@link android.net.wifi.WifiManager#reconnect()}
     */
    CARAPI Reconnect();

    /**
     * see {@link android.net.wifi.WifiManager#reassociate()}
     */
    CARAPI Reassociate();

    /**
     * see {@link android.net.wifi.WifiManager#getConfiguredNetworks()}
     * @return the list of configured networks
     */
    CARAPI GetConfiguredNetworks(
        /* [out] */ IObjectContainer** result);

    /**
     * see {@link android.net.wifi.WifiManager#addOrUpdateNetwork(WifiConfiguration)}
     * @return the supplicant-assigned identifier for the new or updated
     * network if the operation succeeds, or {@code -1} if it fails
     */
    CARAPI AddOrUpdateNetwork(
        /* [in] */ IWifiConfiguration* config,
        /* [out] */ Int32* result);

     /**
     * See {@link android.net.wifi.WifiManager#removeNetwork(int)}
     * @param netId the integer that identifies the network configuration
     * to the supplicant
     * @return {@code true} if the operation succeeded
     */
    CARAPI RemoveNetwork(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /**
     * See {@link android.net.wifi.WifiManager#enableNetwork(int, boolean)}
     * @param netId the integer that identifies the network configuration
     * to the supplicant
     * @param disableOthers if true, disable all other networks.
     * @return {@code true} if the operation succeeded
     */
    CARAPI EnableNetwork(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers,
        /* [out] */ Boolean* result);

    /**
     * See {@link android.net.wifi.WifiManager#disableNetwork(int)}
     * @param netId the integer that identifies the network configuration
     * to the supplicant
     * @return {@code true} if the operation succeeded
     */
    CARAPI DisableNetwork(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /**
     * See {@link android.net.wifi.WifiManager#getConnectionInfo()}
     * @return the Wi-Fi information, contained in {@link WifiInfo}.
     */
    CARAPI GetConnectionInfo(
        /* [out] */ IWifiInfo** result);

    /**
     * Return the results of the most recent access point scan, in the form of
     * a list of {@link ScanResult} objects.
     * @return the list of results
     */
    CARAPI GetScanResults(
        /* [out] */ IObjectContainer** result);

    /**
     * Tell the supplicant to persist the current list of configured networks.
     * @return {@code true} if the operation succeeded
     *
     * TODO: deprecate this
     */
    CARAPI SaveConfiguration(
        /* [out] */ Boolean* result);

    /**
     * Set the country code
     * @param countryCode ISO 3166 country code.
     * @param persist {@code true} if the setting should be remembered.
     *
     * The persist behavior exists so that wifi can fall back to the last
     * persisted country code on a restart, when the locale information is
     * not available from telephony.
     */
    CARAPI SetCountryCode(
        /* [in] */ const String& countryCode,
        /* [in] */ Boolean persist);

    /**
     * Set the operational frequency band
     * @param band One of
     *     {@link WifiManager#WIFI_FREQUENCY_BAND_AUTO},
     *     {@link WifiManager#WIFI_FREQUENCY_BAND_5GHZ},
     *     {@link WifiManager#WIFI_FREQUENCY_BAND_2GHZ},
     * @param persist {@code true} if the setting should be remembered.
     *
     */
    CARAPI SetFrequencyBand(
        /* [in] */ Int32 band,
        /* [in] */ Boolean persist);

    /**
     * Get the operational frequency band
     */
    CARAPI GetFrequencyBand(
        /* [out] */ Int32* result);

    CARAPI IsDualBandSupported(
        /* [out] */ Boolean* result);

    /**
     * Return the DHCP-assigned addresses from the last successful DHCP request,
     * if any.
     * @return the DHCP information
     */
    CARAPI GetDhcpInfo(
        /* [out] */ IDhcpInfo** result);

    /**
     * see {@link android.net.wifi.WifiManager#startWifi}
     *
     */
    CARAPI StartWifi();

    CARAPI CaptivePortalCheckComplete();

    /**
     * see {@link android.net.wifi.WifiManager#stopWifi}
     *
     */
    CARAPI StopWifi();

    /**
     * see {@link android.net.wifi.WifiManager#addToBlacklist}
     *
     */
    CARAPI AddToBlacklist(
        /* [in] */ const String& bssid);

    /**
     * see {@link android.net.wifi.WifiManager#clearBlacklist}
     *
     */
    CARAPI ClearBlacklist();

    /**
     * Get a reference to handler. This is used by a client to establish
     * an AsyncChannel communication with WifiService
     */
    CARAPI GetWifiServiceMessenger(
        /* [out] */ IMessenger** result);

    /** Get a reference to WifiStateMachine handler for AsyncChannel communication */
    CARAPI GetWifiStateMachineMessenger(
        /* [out] */ IMessenger** result);

    /**
     * Get the IP and proxy configuration file
     */
    CARAPI GetConfigFile(
        /* [out] */ String* result);

    CARAPI AcquireWifiLock(
        /* [in] */ IBinder* binder,
        /* [in] */ Int32 lockMode,
        /* [in] */ const String& tag,
        /* [in] */ IWorkSource* ws,
        /* [out] */ Boolean* result);

    CARAPI UpdateWifiLockWorkSource(
        /* [in] */ IBinder* lock,
        /* [in] */ IWorkSource* ws);

    CARAPI ReleaseWifiLock(
        /* [in] */ IBinder* lock,
        /* [out] */ Boolean* result);

    CARAPI InitializeMulticastFiltering();

    CARAPI AcquireMulticastLock(
        /* [in] */ IBinder* binder,
        /* [in] */ const String& tag);

    CARAPI ReleaseMulticastLock();

    CARAPI IsMulticastEnabled(
        /* [out] */ Boolean* result);

protected:
    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    /** Tell battery stats about a new WIFI scan */
    CARAPI_(void) NoteScanStart();

    /** Tell battery stats that the current WIFI scan has completed */
    CARAPI_(void) NoteScanEnd();

    CARAPI_(Boolean) TestAndClearWifiSavedState();

    CARAPI_(Int32) GetPersistedWifiState();

    CARAPI_(Boolean) ShouldWifiBeEnabled();

    CARAPI_(void) HandleWifiToggled(
        /* [in] */ Boolean wifiEnabled);

    CARAPI_(void) HandleAirplaneModeToggled(
        /* [in] */ Boolean airplaneEnabled);

    CARAPI_(void) PersistWifiState(
        /* [in] */ Int32 state);

    CARAPI EnforceAccessPermission();

    CARAPI EnforceChangePermission();

    CARAPI EnforceMulticastChangePermission();

    CARAPI EnforceConnectivityInternalPermission();

    CARAPI_(void) SetDeviceIdleAndUpdateWifi(
        /* [in] */ Boolean deviceIdle);

    CARAPI_(void) ReportStartWorkSource();

    CARAPI_(void) UpdateWifiState();

    CARAPI_(void) RegisterForBroadcasts();

    CARAPI_(Boolean) IsAirplaneSensitive();

    CARAPI_(Boolean) IsAirplaneToggleable();

    /**
     * Returns true if Wi-Fi is sensitive to airplane mode, and airplane mode is
     * currently on.
     * @return {@code true} if airplane mode is on.
     */
    CARAPI_(Boolean) IsAirplaneModeOn();

    CARAPI EnforceWakeSourcePermission(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid);

    CARAPI_(void) NoteAcquireWifiLock(
        /* [in] */ WifiLock* wifiLock); // throws RemoteException;

    CARAPI_(void) NoteReleaseWifiLock(
        /* [in] */ WifiLock* wifiLock); // throws RemoteException;

    CARAPI_(Boolean) AcquireWifiLockLocked(
        /* [in] */ WifiLock* wifiLock);

    CARAPI_(Boolean) ReleaseWifiLockLocked(
        /* [in] */ IBinder* lock);

    CARAPI_(void) RemoveMulticasterLocked(
        /* [in] */ List< AutoPtr<Multicaster> >::Iterator it,
        /* [in] */ Int32 uid);

    /**
     * Evaluate if traffic stats polling is needed based on
     * connection and screen on status
     */
    CARAPI_(void) EvaluateTrafficStatsPolling();

    CARAPI_(void) NotifyOnDataActivity();

    CARAPI_(void) CheckAndSetNotification();

    /**
     * Clears variables related to tracking whether a notification has been
     * shown recently and clears the current notification.
     */
    CARAPI_(void) ResetNotification();

    /**
     * Display or don't display a notification that there are open Wi-Fi networks.
     * @param visible {@code true} if notification should be visible, {@code false} otherwise
     * @param numNetworks the number networks seen
     * @param force {@code true} to force notification to be shown/not-shown,
     * even if it is already shown/not-shown.
     * @param delay time in milliseconds after which the notification should be made
     * visible or invisible.
     */
    CARAPI_(void) SetNotificationVisible(
        /* [in] */ Boolean visible,
        /* [in] */ Int32 numNetworks,
        /* [in] */ Boolean force,
        /* [in] */ Int32 delay);

private:
    static const String TAG;
    static const Boolean DBG;

    AutoPtr<WifiStateMachine> mWifiStateMachine;

    AutoPtr<IContext> mContext;

    AutoPtr<IAlarmManager> mAlarmManager;
    AutoPtr<IPendingIntent> mIdleIntent;
    static const Int32 IDLE_REQUEST = 0;
    Boolean mScreenOff;
    Boolean mDeviceIdle;
    Boolean mEmergencyCallbackMode;
    Int32 mPluggedType;

    AutoPtr<LockList> mLocks;
    Object mLocksLock;
    // some wifi lock statistics
    Int32 mFullHighPerfLocksAcquired;
    Int32 mFullHighPerfLocksReleased;
    Int32 mFullLocksAcquired;
    Int32 mFullLocksReleased;
    Int32 mScanLocksAcquired;
    Int32 mScanLocksReleased;

    List< AutoPtr<Multicaster> > mMulticasters;
    Object mMulticastersLock;
    Int32 mMulticastEnabled;
    Int32 mMulticastDisabled;

    AutoPtr<IIBatteryStats> mBatteryStats;

    Boolean mEnableTrafficStatsPoll;
    Int32 mTrafficStatsPollToken;
    Int64 mTxPkts;
    Int64 mRxPkts;
    /* Tracks last reported data activity */
    Int32 mDataActivity;
    String mInterfaceName;

    /**
     * Interval in milliseconds between polling for traffic
     * statistics
     */
    static const Int32 POLL_TRAFFIC_STATS_INTERVAL_MSECS = 1000;

    /**
     * See {@link Settings.Global#WIFI_IDLE_MS}. This is the default value if a
     * Settings.Global value is not present. This timeout value is chosen as
     * the approximate point at which the battery drain caused by Wi-Fi
     * being enabled but not active exceeds the battery drain caused by
     * re-establishing a connection to the mobile data network.
     */
    static const Int64 DEFAULT_IDLE_MS = 15 * 60 * 1000; /* 15 minutes */

    static const String ACTION_DEVICE_IDLE;

    static const Int32 WIFI_DISABLED = 0;
    static const Int32 WIFI_ENABLED = 1;
    /* Wifi enabled while in airplane mode */
    static const Int32 WIFI_ENABLED_AIRPLANE_OVERRIDE = 2;
    /* Wifi disabled due to airplane mode on */
    static const Int32 WIFI_DISABLED_AIRPLANE_ON = 3;

    /* Persisted state that tracks the wifi & airplane interaction from settings */
    AutoPtr<IAtomicInteger32> mPersistWifiState;
    /* Tracks current airplane mode state */
    AutoPtr<IAtomicBoolean> mAirplaneModeOn;
    /* Tracks whether wifi is enabled from WifiStateMachine's perspective */
    Boolean mWifiEnabled;

    /* The work source (UID) that triggered the current WIFI scan, synchronized
     * on this */
    AutoPtr<IWorkSource> mScanWorkSource;

    Boolean mIsReceiverRegistered;


    AutoPtr<INetworkInfo> mNetworkInfo;

    // Variables relating to the 'available networks' notification
    /**
     * The icon to show in the 'available networks' notification. This will also
     * be the ID of the Notification given to the NotificationManager.
     */
    static const Int32 ICON_NETWORKS_AVAILABLE = R::drawable::stat_notify_wifi_in_range;
    /**
     * When a notification is shown, we wait this amount before possibly showing it again.
     */
    Int64 NOTIFICATION_REPEAT_DELAY_MS;
    /**
     * Whether the user has set the setting to show the 'available networks' notification.
     */
    Boolean mNotificationEnabled;
    /**
     * Observes the user setting to keep {@link #mNotificationEnabled} in sync.
     */
    AutoPtr<NotificationEnabledSettingObserver> mNotificationEnabledSettingObserver;
    /**
     * The {@link System#currentTimeMillis()} must be at least this value for us
     * to show the notification again.
     */
    Int64 mNotificationRepeatTime;
    /**
     * The Notification object given to the NotificationManager.
     */
    AutoPtr<INotification> mNotification;
    /**
     * Whether the notification is being shown, as set by us. That is, if the
     * user cancels the notification, we will not receive the callback so this
     * will still be true. We only guarantee if this is false, then the
     * notification is not showing.
     */
    Boolean mNotificationShown;
    /**
     * The number of continuous scans that must occur before consider the
     * supplicant in a scanning state. This allows supplicant to associate with
     * remembered networks that are in the scan results.
     */
    static const Int32 NUM_SCANS_BEFORE_ACTUALLY_SCANNING = 3;
    /**
     * The number of scans since the last network state change. When this
     * exceeds {@link #NUM_SCANS_BEFORE_ACTUALLY_SCANNING}, we consider the
     * supplicant to actually be scanning. When the network state changes to
     * something other than scanning, we reset this to 0.
     */
    Int32 mNumScansSinceNetworkStateChange;

    /**
     * Asynchronous channel to WifiStateMachine
     */
    AutoPtr<AsyncChannel> mWifiStateMachineChannel;

    /**
     * Clients receiving asynchronous messages
     */
    List< AutoPtr<AsyncChannel> > mClients;

    AutoPtr<IHandler> mAsyncServiceHandler;

    AutoPtr<IHandler> mWifiStateMachineHandler;

    /**
     * Temporary for computing UIDS that are responsible for starting WIFI.
     * Protected by mWifiStateTracker lock.
     */
    AutoPtr<IWorkSource> mTmpWorkSource;
    AutoPtr<WifiWatchdogStateMachine> mWifiWatchdogStateMachine;

    AutoPtr<IBroadcastReceiver> mReceiver;
};

} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CWIFISERVICE_H__
