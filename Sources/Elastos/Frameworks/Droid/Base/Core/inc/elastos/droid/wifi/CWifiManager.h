
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFIMANAGER_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFIMANAGER_H__

#include "_Elastos_Droid_Wifi_CWifiManager.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/internal/utility/AsyncChannel.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Internal::Utility::AsyncChannel;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Net::IDhcpInfo;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Net::IInetAddress;
using Elastos::Utility::Concurrent::ICountDownLatch;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiManager)
    , public Object
    , public IWifiManager
{
public:
    /**
     * Allows an application to keep the Wi-Fi radio awake.
     * Normally the Wi-Fi radio may turn off when the user has not used the device in a while.
     * Acquiring a WifiLock will keep the radio on until the lock is released.  Multiple
     * applications may hold WifiLocks, and the radio will only be allowed to turn off when no
     * WifiLocks are held in any application.
     * <p>
     * Before using a WifiLock, consider carefully if your application requires Wi-Fi access, or
     * could function over a mobile network, if available.  A program that needs to download large
     * files should hold a WifiLock to ensure that the download will complete, but a program whose
     * network usage is occasional or low-bandwidth should not hold a WifiLock to avoid adversely
     * affecting battery life.
     * <p>
     * Note that WifiLocks cannot override the user-level "Wi-Fi Enabled" setting, nor Airplane
     * Mode.  They simply keep the radio from turning off when Wi-Fi is already on but the device
     * is idle.
     * <p>
     * Any application using a WifiLock must request the {@code android.permission.WAKE_LOCK}
     * permission in an {@code &lt;uses-permission&gt;} element of the application's manifest.
     */
    class WifiLock
        : public Object
        , public IWifiManagerWifiLock
    {
    public:
        CAR_INTERFACE_DECL()

        WifiLock(
            /* [in] */ Int32 lockType,
            /* [in] */ const String& tag,
            /* [in] */ CWifiManager* owner);

        ~WifiLock();

        /**
         * Locks the Wi-Fi radio on until {@link #release} is called.
         *
         * If this WifiLock is reference-counted, each call to {@code acquire} will increment the
         * reference count, and the radio will remain locked as long as the reference count is
         * above zero.
         *
         * If this WifiLock is not reference-counted, the first call to {@code acquire} will lock
         * the radio, but subsequent calls will be ignored.  Only one call to {@link #release}
         * will be required, regardless of the number of times that {@code acquire} is called.
         */
        CARAPI AcquireLock();

        /**
         * Unlocks the Wi-Fi radio, allowing it to turn off when the device is idle.
         *
         * If this WifiLock is reference-counted, each call to {@code release} will decrement the
         * reference count, and the radio will be unlocked only when the reference count reaches
         * zero.  If the reference count goes below zero (that is, if {@code release} is called
         * a greater number of times than {@link #acquire}), an exception is thrown.
         *
         * If this WifiLock is not reference-counted, the first call to {@code release} (after
         * the radio was locked using {@link #acquire}) will unlock the radio, and subsequent
         * calls will be ignored.
         */
        CARAPI ReleaseLock();

        /**
         * Controls whether this is a reference-counted or non-reference-counted WifiLock.
         *
         * Reference-counted WifiLocks keep track of the number of calls to {@link #acquire} and
         * {@link #release}, and only allow the radio to sleep when every call to {@link #acquire}
         * has been balanced with a call to {@link #release}.  Non-reference-counted WifiLocks
         * lock the radio whenever {@link #acquire} is called and it is unlocked, and unlock the
         * radio whenever {@link #release} is called and it is locked.
         *
         * @param refCounted TRUE if this WifiLock should keep a reference count
         */
        CARAPI SetReferenceCounted(
            /* [in] */ Boolean refCounted);

        /**
         * Checks whether this WifiLock is currently held.
         *
         * @return TRUE if this WifiLock is held, FALSE otherwise
         */
        CARAPI IsHeld(
            /* [out] */ Boolean* held);

        CARAPI SetWorkSource(
            /* [in] */ IWorkSource* ws);

        // public String toString() {
        //     String s1, s2, s3;
        //     {    AutoLock syncLock(mBinder);
        //         s1 = Integer.toHexString(System.identityHashCode(this));
        //         s2 = mHeld ? "held; " : "";
        //         if (mRefCounted) {
        //             s3 = "refcounted: refcount = " + mRefCount;
        //         } else {
        //             s3 = "not refcounted";
        //         }
        //         return "WifiLock{ " + s1 + "; " + s2 + s3 + " }";
        //     }
        // }

    private:
        String mTag;
        AutoPtr<IBinder> mBinder;
        Object mBinderLock;
        Int32 mRefCount;
        Int32 mLockType;
        Boolean mRefCounted;
        Boolean mHeld;
        AutoPtr<IWorkSource> mWorkSource;
        CWifiManager* mOwner;
    };

    /**
     * Allows an application to receive Wifi Multicast packets.
     * Normally the Wifi stack filters out packets not explicitly
     * addressed to this device.  Acquring a MulticastLock will
     * cause the stack to receive packets addressed to multicast
     * addresses.  Processing these extra packets can cause a noticable
     * battery drain and should be disabled when not needed.
     */
    class MulticastLock
        : public Object
        , public IWifiManagerMulticastLock
    {
    public:
        CAR_INTERFACE_DECL()

        MulticastLock(
            /* [in] */ const String& tag,
            /* [in] */ CWifiManager* owner);

        ~MulticastLock();

        /**
         * Locks Wifi Multicast on until {@link #release} is called.
         *
         * If this MulticastLock is reference-counted each call to
         * {@code acquire} will increment the reference count, and the
         * wifi interface will receive multicast packets as long as the
         * reference count is above zero.
         *
         * If this MulticastLock is not reference-counted, the first call to
         * {@code acquire} will turn on the multicast packets, but subsequent
         * calls will be ignored.  Only one call to {@link #release} will
         * be required, regardless of the number of times that {@code acquire}
         * is called.
         *
         * Note that other applications may also lock Wifi Multicast on.
         * Only they can relinquish their lock.
         *
         * Also note that applications cannot leave Multicast locked on.
         * When an app exits or crashes, any Multicast locks will be released.
         */
        CARAPI AcquireLock();

        /**
         * Unlocks Wifi Multicast, restoring the filter of packets
         * not addressed specifically to this device and saving power.
         *
         * If this MulticastLock is reference-counted, each call to
         * {@code release} will decrement the reference count, and the
         * multicast packets will only stop being received when the reference
         * count reaches zero.  If the reference count goes below zero (that
         * is, if {@code release} is called a greater number of times than
         * {@link #acquire}), an exception is thrown.
         *
         * If this MulticastLock is not reference-counted, the first call to
         * {@code release} (after the radio was multicast locked using
         * {@link #acquire}) will unlock the multicast, and subsequent calls
         * will be ignored.
         *
         * Note that if any other Wifi Multicast Locks are still outstanding
         * this {@code release} call will not have an immediate effect.  Only
         * when all applications have released all their Multicast Locks will
         * the Multicast filter be turned back on.
         *
         * Also note that when an app exits or crashes all of its Multicast
         * Locks will be automatically released.
         */
        CARAPI ReleaseLock();

        /**
         * Controls whether this is a reference-counted or non-reference-
         * counted MulticastLock.
         *
         * Reference-counted MulticastLocks keep track of the number of calls
         * to {@link #acquire} and {@link #release}, and only stop the
         * reception of multicast packets when every call to {@link #acquire}
         * has been balanced with a call to {@link #release}.  Non-reference-
         * counted MulticastLocks allow the reception of multicast packets
         * whenever {@link #acquire} is called and stop accepting multicast
         * packets whenever {@link #release} is called.
         *
         * @param refCounted TRUE if this MulticastLock should keep a reference
         * count
         */
        CARAPI SetReferenceCounted(
            /* [in] */ Boolean refCounted);

        /**
         * Checks whether this MulticastLock is currently held.
         *
         * @return TRUE if this MulticastLock is held, FALSE otherwise
         */
        CARAPI IsHeld(
            /* [in] */ Boolean* held);

        // public String toString() {
        //     String s1, s2, s3;
        //     {    AutoLock syncLock(mBinder);
        //         s1 = Integer.toHexString(System.identityHashCode(this));
        //         s2 = mHeld ? "held; " : "";
        //         if (mRefCounted) {
        //             s3 = "refcounted: refcount = " + mRefCount;
        //         } else {
        //             s3 = "not refcounted";
        //         }
        //         return "MulticastLock{ " + s1 + "; " + s2 + s3 + " }";
        //     }
        // }

    private:
        String mTag;
        AutoPtr<IBinder> mBinder;
        Object mBinderLock;
        Int32 mRefCount;
        Boolean mRefCounted;
        Boolean mHeld;
        CWifiManager* mOwner;
    };

private:
    class ServiceHandler
        : public Handler
    {
    public:
        ServiceHandler(
            /* [in] */ ILooper* looper)
            : Handler(looper)
        {}

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* message);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiManager();

    ~CWifiManager();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IIWifiManager* service);

    CARAPI GetConfiguredNetworks(
        /* [out] */ IList** networks);

    CARAPI AddNetwork(
        /* [in] */ IWifiConfiguration* config,
        /* [out] */ Int32* id);

    CARAPI UpdateNetwork(
        /* [in] */ IWifiConfiguration* config,
        /* [out] */ Int32* networkId);

    CARAPI RemoveNetwork(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* succeeded);

    CARAPI EnableNetwork(
        /* [in] */ Int32 netId,
        /* [in] */ Boolean disableOthers,
        /* [out] */ Boolean* succeeded);

    CARAPI DisableNetwork(
        /* [in] */ Int32 netId,
        /* [out] */ Boolean* succeeded);

    CARAPI Disconnect(
        /*[out] */ Boolean* succeeded);

    CARAPI Reconnect(
        /* [out] */ Boolean* succeeded);

    CARAPI Reassociate(
        /* [out] */ Boolean* succeeded);

    CARAPI PingSupplicant(
        /* [out] */ Boolean* succeeded);

    CARAPI StartScan(
        /* [out] */ Boolean* succeeded);

    CARAPI GetConnectionInfo(
        /* [out] */ IWifiInfo** info);

    CARAPI GetScanResults(
        /* [out] */ IList** results);

    CARAPI SaveConfiguration(
        /* [out] */ Boolean* succeeded);

    /**
     * Set the country code.
     * @param countryCode country code in ISO 3166 format.
     * @param persist {@code TRUE} if this needs to be remembered
     *
     * @hide
     */
    CARAPI SetCountryCode(
        /* [in] */ const String& country,
        /* [in] */ Boolean persist);

    CARAPI SetFrequencyBand(
        /* [in] */ Int32 band,
        /* [in] */ Boolean persist);

    CARAPI GetFrequencyBand(
        /* [out] */ Int32*state);

    /**
     * Check if the chipset supports dual frequency band (2.4 GHz and 5 GHz)
     * @return {@code TRUE} if supported, {@code FALSE} otherwise.
     * @hide
     */
    CARAPI IsDualBandSupported(
        /* [out] */ Boolean* supported);

    /**
     * Check if the chipset supports IBSS (Adhoc) mode
     * @return {@code true} if supported, {@code false} otherwise.
     * @hide
     */
    CARAPI IsIbssSupported(
        /* [out] */ Boolean* supported);

    CARAPI GetDhcpInfo(
        /* [out] */ IDhcpInfo** info);

    CARAPI SetWifiEnabled(
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* succeeded);

    CARAPI GetWifiState(
        /* [out] */ Int32* state);

    CARAPI IsWifiEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Return TX packet counter, for CTS test of WiFi watchdog.
     * @param listener is the interface to receive result
     *
     * @hide for CTS test only
     */
    CARAPI GetTxPacketCount(
        /* [in] */ IWifiManagerTxPacketCountListener* listener);

    /**
     * Calculates the level of the signal. This should be used any time a signal
     * is being shown.
     *
     * @param rssi The power of the signal measured in RSSI.
     * @param numLevels The number of levels to consider in the calculated
     *            level.
     * @return A level of the signal, given in the range of 0 to numLevels-1
     *         (both inclusive).
     */
    static CARAPI_(Int32) CalculateSignalLevel(
        /* [in] */ Int32 rssi,
        /* [in] */ Int32 numLevels);

    /**
     * Compares two signal strengths.
     *
     * @param rssiA The power of the first signal measured in RSSI.
     * @param rssiB The power of the second signal measured in RSSI.
     * @return Returns <0 if the first signal is weaker than the second signal,
     *         0 if the two signals have the same strength, and >0 if the first
     *         signal is stronger than the second signal.
     */
    static CARAPI_(Int32) CompareSignalLevel(
        /* [in] */ Int32 rssiA,
        /* [in] */ Int32 rssiB);

    CARAPI SetWifiApEnabled(
        /* [in] */ IWifiConfiguration* wifiConfig,
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* succeeded);

    /**
     * Gets the Wi-Fi enabled state.
     * @return One of {@link #WIFI_AP_STATE_DISABLED},
     *         {@link #WIFI_AP_STATE_DISABLING}, {@link #WIFI_AP_STATE_ENABLED},
     *         {@link #WIFI_AP_STATE_ENABLING}, {@link #WIFI_AP_STATE_FAILED}
     * @see #isWifiApEnabled()
     *
     * @hide Dont open yet
     */
    CARAPI GetWifiApState(
        /* [out] */ Int32* state);

    /**
     * Return whether Wi-Fi AP is enabled or disabled.
     * @return {@code TRUE} if Wi-Fi AP is enabled
     * @see #getWifiApState()
     *
     * @hide Dont open yet
     */
    CARAPI IsWifiApEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Gets the Wi-Fi AP Configuration.
     * @return AP details in WifiConfiguration
     *
     * @hide Dont open yet
     */
    CARAPI GetWifiApConfiguration(
        /* [out] */ IWifiConfiguration** config);

    /**
     * Sets the Wi-Fi AP Configuration.
     * @return {@code TRUE} if the operation succeeded, {@code FALSE} otherwise
     *
     * @hide Dont open yet
     */
    CARAPI SetWifiApConfiguration(
        /* [in] */ IWifiConfiguration* wifiConfig,
        /* [out] */ Boolean* succeeded);

   /**
     * Start the driver and connect to network.
     *
     * This function will over-ride WifiLock and device idle status. For example,
     * even if the device is idle or there is only a scan-only lock held,
     * a start wifi would mean that wifi connection is kept active until
     * a stopWifi() is sent.
     *
     * This API is used by WifiStateTracker
     *
     * @return {@code TRUE} if the operation succeeds else {@code FALSE}
     * @hide
     */
    CARAPI StartWifi(
        /* [out] */ Boolean* succeeded);

    /**
     * Disconnect from a network (if any) and stop the driver.
     *
     * This function will over-ride WifiLock and device idle status. Wi-Fi
     * stays inactive until a startWifi() is issued.
     *
     * This API is used by WifiStateTracker
     *
     * @return {@code TRUE} if the operation succeeds else {@code FALSE}
     * @hide
     */
    CARAPI StopWifi(
        /* [out] */ Boolean* succeeded);

    /**
     * Add a bssid to the supplicant blacklist
     *
     * This API is used by WifiWatchdogService
     *
     * @return {@code TRUE} if the operation succeeds else {@code FALSE}
     * @hide
     */
    CARAPI AddToBlacklist(
        /* [in] */ const String& bssid,
        /* [out] */ Boolean* succeeded);

    /**
     * Clear the supplicant blacklist
     *
     * This API is used by WifiWatchdogService
     *
     * @return {@code TRUE} if the operation succeeds else {@code FALSE}
     * @hide
     */
    CARAPI ClearBlacklist(
        /* [out] */ Boolean* succeeded);

    CARAPI Connect(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ IWifiManagerActionListener* listener);

    CARAPI Connect(
        /* [in] */ Int32 networkId,
        /* [in] */ IWifiManagerActionListener* listener);

    CARAPI Save(
        /* [in] */ IWifiConfiguration* config,
        /* [in] */ IWifiManagerActionListener* listener);

    CARAPI Forget(
        /* [in] */ Int32 netId,
        /* [in] */ IWifiManagerActionListener* listener);

    CARAPI Disable(
        /* [in] */ Int32 netId,
        /* [in] */ IWifiManagerActionListener* listener);

    CARAPI StartWps(
        /* [in] */ IWpsInfo* config,
        /* [in] */ IWifiManagerWpsCallback* listener);

    CARAPI CancelWps(
        /* [in] */ IWifiManagerWpsCallback* listener);

    /**
     * Get a reference to WifiService handler. This is used by a client to establish
     * an AsyncChannel communication with WifiService
     *
     * @return Messenger pointing to the WifiService handler
     * @hide
     */
    CARAPI GetWifiServiceMessenger(
        /* [out] */ IMessenger** msg);

    CARAPI GetConfigFile(
        /* [out] */ String* cfgFile);

    CARAPI CreateWifiLock(
        /* [in] */ Int32 lockType,
        /* [in] */ const String& tag,
        /* [out] */ IWifiManagerWifiLock** lock);

    CARAPI CreateWifiLock(
        /* [in] */ const String& tag,
        /* [out] */ IWifiManagerWifiLock** lock);

    CARAPI CreateMulticastLock(
        /* [in] */ const String& tag,
        /* [out] */ IWifiManagerMulticastLock** lock);

    CARAPI IsMulticastEnabled(
        /* [out] */ Boolean* enabled);

    CARAPI InitializeMulticastFiltering(
        /* [out] */ Boolean* succeeded);

    CARAPI GetPrivilegedConfiguredNetworks(
        /* [out] */ IList** result);

    CARAPI GetConnectionStatistics(
        /* [out] */ IWifiConnectionStatistics** result);

    CARAPI GetChannelList(
        /* [out] */ IList** result);

    CARAPI Is5GHzBandSupported(
        /* [out] */ Boolean* result);

    CARAPI IsPasspointSupported(
        /* [out] */ Boolean* result);

    CARAPI IsP2pSupported(
        /* [out] */ Boolean* result);

    CARAPI IsPortableHotspotSupported(
        /* [out] */ Boolean* result);

    CARAPI IsWifiScannerSupported(
        /* [out] */ Boolean* result);

    CARAPI IsNanSupported(
        /* [out] */ Boolean* result);

    CARAPI IsDeviceToDeviceRttSupported(
        /* [out] */ Boolean* result);

    CARAPI IsDeviceToApRttSupported(
        /* [out] */ Boolean* result);

    CARAPI IsPreferredNetworkOffloadSupported(
        /* [out] */ Boolean* result);

    CARAPI IsAdditionalStaSupported(
        /* [out] */ Boolean* result);

    CARAPI IsTdlsSupported(
        /* [out] */ Boolean* result);

    CARAPI IsOffChannelTdlsSupported(
        /* [out] */ Boolean* result);

    CARAPI IsEnhancedPowerReportingSupported(
        /* [out] */ Boolean* result);

    CARAPI GetControllerActivityEnergyInfo(
        /* [in] */ Int32 updateType,
        /* [out] */ IWifiActivityEnergyInfo** result);

    CARAPI StartScan(
        /* [in] */ IWorkSource* workSource,
        /* [out] */ Boolean* result);

    CARAPI StartCustomizedScan(
        /* [in] */ IScanSettings* requested,
        /* [out] */ Boolean* result);

    CARAPI StartCustomizedScan(
        /* [in] */ IScanSettings* requested,
        /* [in] */ IWorkSource* workSource,
        /* [out] */ Boolean* result);

    CARAPI RequestBatchedScan(
        /* [in] */ IBatchedScanSettings* requested,
        /* [out] */ Boolean* result);

    CARAPI RequestBatchedScan(
        /* [in] */ IBatchedScanSettings* requested,
        /* [in] */ IWorkSource* workSource,
        /* [out] */ Boolean* result);

    CARAPI IsBatchedScanSupported(
        /* [out] */ Boolean* result);

    CARAPI StopBatchedScan(
        /* [in] */ IBatchedScanSettings* requested);

    CARAPI GetBatchedScanResults(
        /* [out] */ IList** result);

    CARAPI PollBatchedScan();

    CARAPI GetWpsNfcConfigurationToken(
        /* [in] */ Int32 netId,
        /* [out] */ String* result);

    CARAPI IsScanAlwaysAvailable(
        /* [out] */ Boolean* result);

    CARAPI SetTdlsEnabled(
        /* [in] */ IInetAddress* remoteIPAddress,
        /* [in] */ Boolean bEnable);

    CARAPI SetTdlsEnabledWithMacAddress(
        /* [in] */ const String& remoteMacAddress,
        /* [in] */ Boolean bEnable);

    CARAPI EnableVerboseLogging(
        /* [in] */ Int32 verbose);

    CARAPI GetVerboseLoggingLevel(
        /* [out] */ Int32* result);

    CARAPI EnableAggressiveHandover(
        /* [in] */ Int32 enabled);

    CARAPI GetAggressiveHandover(
        /* [out] */ Int32* result);

    CARAPI SetAllowScansWithTraffic(
        /* [in] */ Int32 enabled);

    CARAPI GetAllowScansWithTraffic(
        /* [out] */ Int32* result);

private:
    /**
     * Internal method for doing the RPC that creates a new network description
     * or updates an existing one.
     *
     * @param config The possibly sparse object containing the variables that
     *         are to set or updated in the network description.
     * @return the ID of the network on success, {@code -1} on failure.
     */
    CARAPI_(Int32) AddOrUpdateNetwork(
        /* [in] */ IWifiConfiguration* config);

    static CARAPI_(Int32) PutListener(
        /* [in] */ IInterface* listener);

    static CARAPI_(AutoPtr<IInterface>) RemoveListener(
        /* [in] */ Int32 key);

    CARAPI_(void) Init();

    CARAPI ValidateChannel();

    CARAPI_(Int32) GetSupportedFeatures();

    CARAPI_(Boolean) IsFeatureSupported(
        /* [in] */ Int32 feature);

private:
    static const String TAG;

    /** Anything worse than or equal to this will show 0 bars. */
    static const Int32 MIN_RSSI;

    /** Anything better than or equal to this will show the max bars. */
    static const Int32 MAX_RSSI;

    /* Maximum number of active locks we allow.
     * This limit was added to prevent apps from creating a ridiculous number
     * of locks and crashing the system by overflowing the global ref table.
     */
    static const Int32 MAX_ACTIVE_LOCKS;

    /* Number of currently active WifiLocks and MulticastLocks */
    Int32 mActiveLockCount;

    AutoPtr<IContext> mContext;
    AutoPtr<IIWifiManager> mService;

    static const Int32 INVALID_KEY;
    static Int32 sListenerKey;
    static AutoPtr<ISparseArray> sListenerMap;
    static Object sListenerMapLock;

    static AutoPtr<IAsyncChannel> sAsyncChannel;
    static AutoPtr<ICountDownLatch> sConnected;

    static Object sThreadRefLock;
    static Int32 sThreadRefCount;
    static AutoPtr<IHandlerThread> sHandlerThread;
    AutoPtr<IAppOpsManager> mAppOps;

    /* TODO: deprecate synchronous API and open up the following API */
    static const Int32 BASE;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_NET_WIFI_CWIFIMANAGER_H__
