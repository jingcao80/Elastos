#ifndef __ELASTOS_DROID_BLUETOOTH_BLUETOOTHTETHERINGDATATRACLER_H__
#define __ELASTOS_DROID_BLUETOOTH_BLUETOOTHTETHERINGDATATRACLER_H__

#include "elastos/droid/net/NetworkStateTracker.h"
#include "BluetoothPan.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Core::IThread;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Net::INetworkStateTracker;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::ILinkProperties;
using Elastos::Droid::Net::ILinkCapabilities;

namespace Elastos {
namespace Droid {
namespace Bluetooth {

class BluetoothTetheringDataTracker
    : public ElRefBase
    , public INetworkStateTracker
{
private:
    class ProfileServiceListener
        : public ElRefBase
        , public IBluetoothProfileServiceListener
    {
    public:
        ProfileServiceListener(
            /* [in] */ BluetoothTetheringDataTracker* host);

        CAR_INTERFACE_DECL()

        CARAPI OnServiceConnected(
            /* [in] */ Int32 profile,
            /* [in] */ IBluetoothProfile* proxy);

        CARAPI OnServiceDisconnected(
            /* [in] */ Int32 profile);

    private:
        BluetoothTetheringDataTracker* mHost;
    };

    class DhcpRunnable : public Runnable
    {
    public:
        DhcpRunnable(
            /* [in] */ BluetoothTetheringDataTracker* host);

        CARAPI Run();

    private:
        BluetoothTetheringDataTracker* mHost;
    };

private:
    BluetoothTetheringDataTracker();

public:
    CAR_INTERFACE_DECL()

    static CARAPI_(AutoPtr<BluetoothTetheringDataTracker>) GetInstance();

    CARAPI SetTeardownRequested(
        /* [in] */ Boolean isRequested);

    CARAPI IsTeardownRequested(
        /* [out] */ Boolean* isRequested);

    /**
     * Begin monitoring connectivity
     */
    CARAPI StartMonitoring(
        /* [in] */ IContext* context,
        /* [in] */ IHandler* target);

    /**
     * Disable connectivity to a network
     * TODO: do away with return value after making MobileDataStateTracker async
     */
    CARAPI Teardown(
        /* [out] */ Boolean* result);

    CARAPI CaptivePortalCheckComplete();

    /**
     * Re-enable connectivity to a network after a {@link #teardown()}.
     */
    CARAPI Reconnect(
        /* [out] */ Boolean* result);

    /**
     * Turn the wireless radio off for a network.
     * @param turnOn {@code true} to turn the radio on, {@code false}
     */
    CARAPI SetRadio(
        /* [in] */ Boolean turnOn,
        /* [out] */ Boolean* result);

    /**
     * @return true - If are we currently tethered with another device.
     */
    /*synchronized*/
    CARAPI IsAvailable(
        /* [out] */ Boolean* isAvailable);

    /**
     * Tells the underlying networking system that the caller wants to
     * begin using the named feature. The interpretation of {@code feature}
     * is completely up to each networking implementation.
     * @param feature the name of the feature to be used
     * @param callingPid the process ID of the process that is issuing this request
     * @param callingUid the user ID of the process that is issuing this request
     * @return an integer value representing the outcome of the request.
     * The interpretation of this value is specific to each networking
     * implementation+feature combination, except that the value {@code -1}
     * always indicates failure.
     * TODO: needs to go away
     */
    CARAPI_(Int32) StartUsingNetworkFeature(
        /* [in] */ const String& feature,
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid);

    /**
     * Tells the underlying networking system that the caller is finished
     * using the named feature. The interpretation of {@code feature}
     * is completely up to each networking implementation.
     * @param feature the name of the feature that is no longer needed.
     * @param callingPid the process ID of the process that is issuing this request
     * @param callingUid the user ID of the process that is issuing this request
     * @return an integer value representing the outcome of the request.
     * The interpretation of this value is specific to each networking
     * implementation+feature combination, except that the value {@code -1}
     * always indicates failure.
     * TODO: needs to go away
     */
    CARAPI_(Int32) StopUsingNetworkFeature(
        /* [in] */ const String& feature,
        /* [in] */ Int32 callingPid,
        /* [in] */ Int32 callingUid);

    CARAPI SetUserDataEnable(
        /* [in] */ Boolean enabled);

    CARAPI SetPolicyDataEnable(
        /* [in] */ Boolean enabled);

    /**
     * Check if private DNS route is set for the network
     */
    CARAPI IsPrivateDnsRouteSet(
        /* [out] */ Boolean* isSet);

    /**
     * Set a flag indicating private DNS route is set
     */
    CARAPI SetPrivateDnsRoute(
        /* [in] */ Boolean enabled);

    /**
     * Fetch NetworkInfo for the network
     */
    /*Synchronized*/
    CARAPI GetNetworkInfo(
        /* [out] */ INetworkInfo** info);

    /**
     * Fetch LinkProperties for the network
     */
    /*Synchronized*/
    CARAPI GetLinkProperties(
        /* [out] */ ILinkProperties** linkProperties);

   /**
     * A capability is an Integer/String pair, the capabilities
     * are defined in the class LinkSocket#Key.
     *
     * @return a copy of this connections capabilities, may be empty but never null.
     */
    CARAPI GetLinkCapabilities(
        /* [out] */ ILinkCapabilities** linkCapabilities);

    /**
     * Fetch default gateway address for the network
     */
    CARAPI_(Int32) GetDefaultGatewayAddr();

    /**
     * Check if default route is set
     */
    CARAPI IsDefaultRouteSet(
        /* [out] */ Boolean* isSet);

    /**
     * Set a flag indicating default route is set for the network
     */
    CARAPI SetDefaultRoute(
        /* [in] */ Boolean enabled);

    /**
     * Return the system properties name associated with the tcp buffer sizes
     * for this network.
     */
    CARAPI GetTcpBufferSizesPropName(
        /* [out] */ String* propName);

    // synchronized
    CARAPI_(void) StartReverseTether(
        /* [in] */ const String& iface);

    // synchronized
    CARAPI_(void) StopReverseTether();

    CARAPI SetDependencyMet(
        /* [in] */ Boolean met);

private:
    static CARAPI_(Int16) CountPrefixLength(
        /* [in] */ ArrayOf<Byte>* mask);

    CARAPI_(Boolean) ReadLinkProperty(
        /* [in] */ const String& iface);

private:
    static const String NETWORKTYPE;
    static const String TAG;
    static const Boolean DBG;
    static const Boolean VDBG;
    static AutoPtr<BluetoothTetheringDataTracker> sInstance;
    static String sIface;

    AutoPtr<IAtomicBoolean> mTeardownRequested;
    AutoPtr<IAtomicBoolean> mPrivateDnsRouteSet;
    AutoPtr<IAtomicInteger32> mDefaultGatewayAddr;
    AutoPtr<IAtomicBoolean> mDefaultRouteSet;

    AutoPtr<ILinkProperties> mLinkProperties;
    AutoPtr<ILinkCapabilities> mLinkCapabilities;
    AutoPtr<INetworkInfo> mNetworkInfo;

    AutoPtr<BluetoothPan> mBluetoothPan;
    AutoPtr<IThread> mDhcpThread;

    /* For sending events to connectivity service handler */
    AutoPtr<IHandler> mCsHandler;
    AutoPtr<IContext> mContext;

    AutoPtr<IBluetoothProfileServiceListener> mProfileServiceListener;

    Object mLock;
};

} // Bluetooth
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_BLUETOOTHTETHERINGDATATRACLER_H__
