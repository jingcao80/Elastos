#ifndef __ELASTOS_DROID_SERVER_WIFI_WIFISERVICEIMPL_H__
#define __ELASTOS_DROID_SERVER_WIFI_WIFISERVICEIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/server/wifi/WifiMonitor.h"
#include "elastos/droid/server/wifi/WifiStateMachine.h"
#include "elastos/droid/server/wifi/WifiSettingsStore.h"
#include "elastos/droid/server/wifi/WifiNotificationController.h"
#include "elastos/droid/server/wifi/WifiTrafficPoller.h"
#include "elastos/droid/server/wifi/WifiWatchdogStateMachine.h"
#include "elastos/droid/server/wifi/WifiController.h"


// import static com.android.server.wifi.WifiController.CMD_AIRPLANE_TOGGLED;
// import static com.android.server.wifi.WifiController.CMD_BATTERY_CHANGED;
// import static com.android.server.wifi.WifiController.CMD_EMERGENCY_MODE_CHANGED;
// import static com.android.server.wifi.WifiController.CMD_LOCKS_CHANGED;
// import static com.android.server.wifi.WifiController.CMD_SCAN_ALWAYS_MODE_CHANGED;
// import static com.android.server.wifi.WifiController.CMD_SCREEN_OFF;
// import static com.android.server.wifi.WifiController.CMD_SCREEN_ON;
// import static com.android.server.wifi.WifiController.CMD_SET_AP;
// import static com.android.server.wifi.WifiController.CMD_USER_PRESENT;
// import static com.android.server.wifi.WifiController.CMD_WIFI_TOGGLED;

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Internal::App::IIBatteryStats;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Net::IDhcpInfo;
using Elastos::Droid::Wifi::IIWifiManager;
using Elastos::Droid::Wifi::IScanSettings;
using Elastos::Droid::Wifi::IBatchedScanSettings;
using Elastos::Droid::Wifi::IWifiConfiguration;
using Elastos::Droid::Wifi::IWifiActivityEnergyInfo;
using Elastos::Droid::Wifi::IWifiConnectionStatistics;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

/**
  * WifiService handles remote WiFi operation requests by implementing
  * the IWifiManager interface.
  *
  * @hide
  */
class WifiServiceImpl
    : public Object
    , public IIWifiManager
    , public IBinder
{
public:
    /**
      * enable TDLS for the local NIC to remote NIC
      * The APPs don't know the remote MAC address to identify NIC though,
      * so we need to do additional work to find it from remote IP address
      */
    class TdlsTaskParams
        : public Object
    {
    public:
        String remoteIpAddress;
        Boolean enable;
    };

    class TdlsTask
        : public AsyncTask//<TdlsTaskParams, Integer, Integer>
    {
    protected:
        // @Override
        //CARAPI_(AutoPtr<Integer>) DoInBackground(
        //    /* [in] */  TdlsTaskParams);
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

    };

    class DeathRecipient
        : public Object
        //TODO , public IBinder::DeathRecipient
    {
    public:
        DeathRecipient(
            /* [in] */ Int32 mode,
            /* [in] */ const String& tag,
            /* [in] */ IBinder* binder,
            /* [in] */ IWorkSource* ws);

        virtual CARAPI UnlinkDeathRecipient();

    public:
        String mTag;
        Int32 mMode;
        AutoPtr<IBinder> mBinder;
        AutoPtr<IWorkSource> mWorkSource;
    };

    class WifiLock
        : public DeathRecipient
    {
    public:
        WifiLock(
            /* [in] */ Int32 lockMode,
            /* [in] */ const String& tag,
            /* [in] */ IBinder* binder,
            /* [in] */ IWorkSource* ws);

        virtual CARAPI BinderDied();

        virtual CARAPI ToString(
            /* [out] */ String* result);
    };

    class LockList
        : public Object
    {
    public:
        // synchronized
        virtual CARAPI HasLocks(
            /* [out] */ Boolean* result);

        // synchronized
        virtual CARAPI GetStrongestLockMode(
            /* [out] */ Int32* result);

        // synchronized
        virtual CARAPI UpdateWorkSource(
            /* [in] */ IWorkSource* ws);

    private:
        LockList();

        CARAPI_(void) AddLock(
            /* [in] */ WifiLock* lock);

        CARAPI_(AutoPtr<WifiLock>) RemoveLock(
            /* [in] */ IBinder* binder);

        CARAPI_(Int32) FindLockByBinder(
            /* [in] */ IBinder* binder);

        CARAPI_(void) Dump(
            /* [in] */ IPrintWriter* pw);

    private:
        AutoPtr<IList> mList;//WifiLock
    };

private:
    /**
      * Handles client connections
      */
    class ClientHandler
        : public Handler
    {
    public:
        ClientHandler(
            /* [in] */ ILooper* looper);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) ReplyFailed(
            /* [in] */ IMessage* msg,
            /* [in] */ Int32 what,
            /* [in] */ Int32 why);
    };

    /**
      * Handles interaction with WifiStateMachine
      */
    class WifiStateMachineHandler
        : public Handler
    {
    public:
        WifiStateMachineHandler(
            /* [in] */ ILooper* looper);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AutoPtr<IAsyncChannel> mWsmChannel;
    };

    class InnerBroadcastReceiver1
        : public BroadcastReceiver
    {
    public:
        InnerBroadcastReceiver1(
            /* [in] */ WifiServiceImpl* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiServiceImpl* mOwner;
    };

    class BatchedScanRequest
        : public DeathRecipient
    {
    public:
        BatchedScanRequest(
            /* [in] */ IBatchedScanSettings* settings,
            /* [in] */ IBinder* binder,
            /* [in] */ IWorkSource* ws);

        virtual CARAPI BinderDied();

        virtual CARAPI ToString(
            /* [out] */ String* result);

        virtual CARAPI IsSameApp(
            /* [in] */ Int32 uid,
            /* [in] */ Int32 pid,
            /* [out] */ Boolean* result);

    public:
        AutoPtr<IBatchedScanSettings> settings;
        Int32 uid;
        Int32 pid;
        AutoPtr<IWorkSource> workSource;
    };

    class InnerBroadcastReceiver2
        : public BroadcastReceiver
    {
    public:
        InnerBroadcastReceiver2(
            /* [in] */ WifiServiceImpl* owner);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        WifiServiceImpl* mOwner;
    };

    class InnerContentObserver1
        : public ContentObserver
    {
    public:
        InnerContentObserver1(
            /* [in] */ WifiServiceImpl* owner);

        // @Override
        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        WifiServiceImpl* mOwner;
    };

    class Multicaster
        : public DeathRecipient
    {
    public:
        Multicaster(
            /* [in] */ const String& tag,
            /* [in] */ IBinder* binder);

        virtual CARAPI BinderDied();

        virtual CARAPI ToString(
            /* [out] */ String* result);

        virtual CARAPI GetUid(
            /* [out] */ Int32* result);
    };

public:
    WifiServiceImpl(
        /* [in] */ IContext* context);

    /**
      * Check if Wi-Fi needs to be enabled and start
      * if needed
      *
      * This function is used only at boot time
      */
    virtual CARAPI CheckAndStartWifi();

    /**
      * see {@link android.net.wifi.WifiManager#pingSupplicant()}
      * @return {@code true} if the operation succeeds, {@code false} otherwise
      */
    virtual CARAPI PingSupplicant(
        /* [out] */ Boolean* result);

    /**
      * see {@link android.net.wifi.WifiManager#getChannelList}
      */
    virtual CARAPI GetChannelList(
        /* [out] */ IList** result);//WifiChannel

    /**
      * see {@link android.net.wifi.WifiManager#startScan}
      * and {@link android.net.wifi.WifiManager#startCustomizedScan}
      *
      * @param settings If null, use default parameter, i.e. full scan.
      * @param workSource If null, all blame is given to the calling uid.
      */
    virtual CARAPI StartScan(
        /* [in] */ IScanSettings* settings,
        /* [in] */ IWorkSource* workSource);

    virtual CARAPI IsBatchedScanSupported(
        /* [out] */ Boolean* result);

    virtual CARAPI PollBatchedScan();

    virtual CARAPI GetWpsNfcConfigurationToken(
        /* [in] */ Int32 netId,
        /* [out] */ String* result);

    /**
      * see {@link android.net.wifi.WifiManager#requestBatchedScan()}
      */
    virtual CARAPI RequestBatchedScan(
        /* [in] */ IBatchedScanSettings* requested,
        /* [in] */ IBinder* binder,
        /* [in] */ IWorkSource* workSource,
        /* [out] */ Boolean* result);

    virtual CARAPI GetBatchedScanResults(
        /* [in] */ const String& callingPackage,
        /* [out] */ IList** result);//BatchedScanResult

    virtual CARAPI StopBatchedScan(
        /* [in] */ IBatchedScanSettings* settings);

    /**
      * see {@link android.net.wifi.WifiManager#setWifiEnabled(boolean)}
      * @param enable {@code true} to enable, {@code false} to disable.
      * @return {@code true} if the enable/disable operation was
      *         started or is already in the queue.
      */
    // synchronized
    virtual CARAPI SetWifiEnabled(
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
    virtual CARAPI GetWifiEnabledState(
        /* [out] */ Int32* result);

    /**
      * see {@link android.net.wifi.WifiManager#setWifiApEnabled(WifiConfiguration, boolean)}
      * @param wifiConfig SSID, security and channel details as
      *        part of WifiConfiguration
      * @param enabled true to enable and false to disable
      */
    virtual CARAPI SetWifiApEnabled(
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
    virtual CARAPI GetWifiApEnabledState(
        /* [out] */ Int32* result);

    /**
      * see {@link WifiManager#getWifiApConfiguration()}
      * @return soft access point configuration
      */
    virtual CARAPI GetWifiApConfiguration(
        /* [out] */ IWifiConfiguration** result);

    /**
      * see {@link WifiManager#setWifiApConfiguration(WifiConfiguration)}
      * @param wifiConfig WifiConfiguration details for soft access point
      */
    virtual CARAPI SetWifiApConfiguration(
        /* [in] */ IWifiConfiguration* wifiConfig);

    /**
      * @param enable {@code true} to enable, {@code false} to disable.
      * @return {@code true} if the enable/disable operation was
      *         started or is already in the queue.
      */
    virtual CARAPI IsScanAlwaysAvailable(
        /* [out] */ Boolean* result);

    /**
      * see {@link android.net.wifi.WifiManager#disconnect()}
      */
    virtual CARAPI Disconnect();

    /**
      * see {@link android.net.wifi.WifiManager#reconnect()}
      */
    virtual CARAPI Reconnect();

    /**
      * see {@link android.net.wifi.WifiManager#reassociate()}
      */
    virtual CARAPI Reassociate();

    /**
      * see {@link android.net.wifi.WifiManager#getSupportedFeatures}
      */
    virtual CARAPI GetSupportedFeatures(
        /* [out] */ Int32* result);

    /**
      * see {@link android.net.wifi.WifiAdapter#reportActivityInfo}
      */
    virtual CARAPI ReportActivityInfo(
        /* [out] */ IWifiActivityEnergyInfo** result);

    /**
      * see {@link android.net.wifi.WifiManager#getConfiguredNetworks()}
      * @return the list of configured networks
      */
    virtual CARAPI GetConfiguredNetworks(
        /* [out] */ IList** result);//WifiConfiguration

    /**
      * see {@link android.net.wifi.WifiManager#getPrivilegedConfiguredNetworks()}
      * @return the list of configured networks with real preSharedKey
      */
    virtual CARAPI GetPrivilegedConfiguredNetworks(
        /* [out] */ IList** result);//WifiConfiguration

    /**
      * see {@link android.net.wifi.WifiManager#addOrUpdateNetwork(WifiConfiguration)}
      * @return the supplicant-assigned identifier for the new or updated
      * network if the operation succeeds, or {@code -1} if it fails
      */
    virtual CARAPI AddOrUpdateNetwork(
        /* [in] */ IWifiConfiguration* config,
        /* [out] */ Int32* result);

    /**
      * See {@link android.net.wifi.WifiManager#removeNetwork(int)}
      * @param netId the integer that identifies the network configuration
      * to the supplicant
      * @return {@code true} if the operation succeeded
      */
    virtual CARAPI RemoveNetwork(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /**
      * See {@link android.net.wifi.WifiManager#enableNetwork(int, boolean)}
      * @param netId the integer that identifies the network configuration
      * to the supplicant
      * @param disableOthers if true, disable all other networks.
      * @return {@code true} if the operation succeeded
      */
    virtual CARAPI EnableNetwork(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers,
        /* [out] */ Boolean* result);

    /**
      * See {@link android.net.wifi.WifiManager#disableNetwork(int)}
      * @param netId the integer that identifies the network configuration
      * to the supplicant
      * @return {@code true} if the operation succeeded
      */
    virtual CARAPI DisableNetwork(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* result);

    /**
      * See {@link android.net.wifi.WifiManager#getConnectionInfo()}
      * @return the Wi-Fi information, contained in {@link WifiInfo}.
      */
    virtual CARAPI GetConnectionInfo(
        /* [out] */ IWifiInfo** result);

    /**
      * Return the results of the most recent access point scan, in the form of
      * a list of {@link ScanResult} objects.
      * @return the list of results
      */
    virtual CARAPI GetScanResults(
        /* [in] */ const String& callingPackage,
        /* [out] */ IList** result);//ScanResult

    /**
      * Tell the supplicant to persist the current list of configured networks.
      * @return {@code true} if the operation succeeded
      *
      * TODO: deprecate this
      */
    virtual CARAPI SaveConfiguration(
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
    virtual CARAPI SetCountryCode(
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
    virtual CARAPI SetFrequencyBand(
        /* [in] */ Int32 band,
        /* [in] */ Boolean persist);

    /**
      * Get the operational frequency band
      */
    virtual CARAPI GetFrequencyBand(
        /* [out] */ Int32* result);

    virtual CARAPI IsDualBandSupported(
        /* [out] */ Boolean* result);

    /**
      * Is Ad-Hoc (IBSS) mode supported by the driver?
      * Will only return correct results when we have reached WIFI_STATE_ENABLED
      * @return {@code true} if IBSS mode is supported, {@code false} if not
      */
    virtual CARAPI IsIbssSupported(
        /* [out] */ Boolean* result);

    /**
      * Return the DHCP-assigned addresses from the last successful DHCP request,
      * if any.
      * @return the DHCP information
      * @deprecated
      */
    virtual CARAPI GetDhcpInfo(
        /* [out] */ IDhcpInfo** result);

    /**
      * see {@link android.net.wifi.WifiManager#startWifi}
      *
      */
    virtual CARAPI StartWifi();

    /**
      * see {@link android.net.wifi.WifiManager#stopWifi}
      *
      */
    virtual CARAPI StopWifi();

    /**
      * see {@link android.net.wifi.WifiManager#addToBlacklist}
      *
      */
    virtual CARAPI AddToBlacklist(
        /* [in] */ const String& bssid);

    /**
      * see {@link android.net.wifi.WifiManager#clearBlacklist}
      *
      */
    virtual CARAPI ClearBlacklist();

    virtual CARAPI EnableTdls(
        /* [in] */ const String& remoteAddress,
        /* [in] */ Boolean enable);

    virtual CARAPI EnableTdlsWithMacAddress(
        /* [in] */ const String& remoteMacAddress,
        /* [in] */ Boolean enable);

    /**
      * Get a reference to handler. This is used by a client to establish
      * an AsyncChannel communication with WifiService
      */
    virtual CARAPI GetWifiServiceMessenger(
        /* [out] */ IMessenger** result);

    /**
      * Get the IP and proxy configuration file
      */
    virtual CARAPI GetConfigFile(
        /* [out] */ String* result);

    virtual CARAPI EnforceWakeSourcePermission(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid);

    virtual CARAPI AcquireWifiLock(
        /* [in] */ IBinder* binder,
        /* [in] */ Int32 lockMode,
        /* [in] */ const String& tag,
        /* [in] */ IWorkSource* ws,
        /* [out] */ Boolean* result);

    virtual CARAPI UpdateWifiLockWorkSource(
        /* [in] */ IBinder* lock,
        /* [in] */ IWorkSource* ws);

    virtual CARAPI ReleaseWifiLock(
        /* [in] */ IBinder* lock,
        /* [out] */ Boolean* result);

    virtual CARAPI InitializeMulticastFiltering();

    virtual CARAPI AcquireMulticastLock(
        /* [in] */ IBinder* binder,
        /* [in] */ const String& tag);

    virtual CARAPI ReleaseMulticastLock();

    virtual CARAPI IsMulticastEnabled(
        /* [out] */ Boolean* result);

    virtual CARAPI GetWifiMonitor(
        /* [out] */ WifiMonitor** result);

    virtual CARAPI EnableVerboseLogging(
        /* [in] */ Int32 verbose);

    virtual CARAPI GetVerboseLoggingLevel(
        /* [out] */ Int32* result);

    virtual CARAPI EnableAggressiveHandover(
        /* [in] */ Int32 enabled);

    virtual CARAPI GetAggressiveHandover(
        /* [out] */ Int32* result);

    virtual CARAPI SetAllowScansWithTraffic(
        /* [in] */ Int32 enabled);

    virtual CARAPI GetAllowScansWithTraffic(
        /* [out] */ Int32* result);

    /* Return the Wifi Connection statistics object */
    virtual CARAPI GetConnectionStatistics(
        /* [out] */ IWifiConnectionStatistics** result);

protected:
    // @Override
    CARAPI_(void) Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

private:
    CARAPI_(void) StopBatchedScan(
        /* [in] */ IBatchedScanSettings* settings,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid);

    CARAPI_(void) ResolveBatchedScannersLocked();

    CARAPI_(void) EnforceAccessPermission();

    CARAPI_(void) EnforceChangePermission();

    CARAPI_(void) EnforceReadCredentialPermission();

    CARAPI_(void) EnforceWorkSourcePermission();

    CARAPI_(void) EnforceMulticastChangePermission();

    CARAPI_(void) EnforceConnectivityInternalPermission();

    /**
      * Returns true if the calling user is the current one or a profile of the
      * current user..
      */
    CARAPI_(Boolean) IsCurrentProfile(
        /* [in] */ Int32 userId);

    /**
      * Returns true if uid is an application running under the owner or a profile of the owner.
      *
      * Note: Should not be called if identity is cleared.
      */
    CARAPI_(Boolean) IsOwner(
        /* [in] */ Int32 uid);

    /**
      * Observes settings changes to scan always mode.
      */
    CARAPI_(void) RegisterForScanModeChange();

    CARAPI_(void) RegisterForBroadcasts();

    CARAPI_(void) NoteAcquireWifiLock(
        /* [in] */ WifiLock* wifiLock);

    CARAPI_(void) NoteReleaseWifiLock(
        /* [in] */ WifiLock* wifiLock);

    CARAPI_(Boolean) AcquireWifiLockLocked(
        /* [in] */ WifiLock* wifiLock);

    CARAPI_(Boolean) ReleaseWifiLockLocked(
        /* [in] */ IBinder* lock);

    CARAPI_(void) RemoveMulticasterLocked(
        /* [in] */ Int32 i,
        /* [in] */ Int32 uid);

public:
    AutoPtr<WifiStateMachine> mWifiStateMachine;
    AutoPtr<LockList> mLocks;
    /* Tracks the persisted states for wi-fi & airplane mode */
    AutoPtr<WifiSettingsStore> mSettingsStore;
    Boolean mBatchedScanSupported;
    AutoPtr<WifiStateMachineHandler> mWifiStateMachineHandler;

private:
    static const String TAG;
    static const Boolean DBG;
    AutoPtr<IContext> mContext;
    // some wifi lock statistics
    Int32 mFullHighPerfLocksAcquired;
    Int32 mFullHighPerfLocksReleased;
    Int32 mFullLocksAcquired;
    Int32 mFullLocksReleased;
    Int32 mScanLocksAcquired;
    Int32 mScanLocksReleased;
    AutoPtr<IList> mMulticasters;//Multicaster
    Int32 mMulticastEnabled;
    Int32 mMulticastDisabled;
    AutoPtr<IIBatteryStats> mBatteryStats;
    AutoPtr<IAppOpsManager> mAppOps;
    String mInterfaceName;
    // Debug counter tracking scan requests sent by WifiManager
    Int32 scanRequestCounter;
    /* Tracks the open wi-fi network notification */
    AutoPtr<WifiNotificationController> mNotificationController;
    /* Polls traffic stats and notifies clients */
    AutoPtr<WifiTrafficPoller> mTrafficPoller;
    Boolean mIsControllerStarted;
    /**
      * Asynchronous channel to WifiStateMachine
      */
    AutoPtr<IAsyncChannel> mWifiStateMachineChannel;
    AutoPtr<ClientHandler> mClientHandler;
    AutoPtr<WifiWatchdogStateMachine> mWifiWatchdogStateMachine;
    AutoPtr<WifiController> mWifiController;
    AutoPtr<IList> mBatchedScanners;//BatchedScanRequest
    AutoPtr<IBroadcastReceiver> mReceiver;
};

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WIFI_WIFISERVICEIMPL_H__

