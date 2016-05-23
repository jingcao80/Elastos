
#ifndef __ELASTOS_DROID_NET_NETWORKINFO_H__
#define __ELASTOS_DROID_NET_NETWORKINFO_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Describes the status of a network interface.
 * <p>Use {@link ConnectivityManager#getActiveNetworkInfo()} to get an instance that represents
 * the current network connection.
 */
class NetworkInfo
    : public Object
    , public IParcelable
    , public INetworkInfo
{
public:
    CAR_INTERFACE_DECL()

    NetworkInfo();

    CARAPI constructor();
    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ Int32 type,
        /* [in] */ Int32 subtype,
        /* [in] */ const String& typeName,
        /* [in] */ const String& subtypeName);

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ INetworkInfo* source);

    /**
     * Reports the type of network to which the
     * info in this {@code NetworkInfo} pertains.
     * @return one of {@link ConnectivityManager#TYPE_MOBILE}, {@link
     * ConnectivityManager#TYPE_WIFI}, {@link ConnectivityManager#TYPE_WIMAX}, {@link
     * ConnectivityManager#TYPE_ETHERNET},  {@link ConnectivityManager#TYPE_BLUETOOTH}, or other
     * types defined by {@link ConnectivityManager}
     */
    CARAPI GetType(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetType(
        /* [in] */ Int32 type);

    /**
     * Return a network-type-specific integer describing the subtype
     * of the network.
     * @return the network subtype
     */
    CARAPI GetSubtype(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetSubtype(
        /* [in] */ Int32 subtype,
        /* [in] */ const String& subtypeName);

    /**
     * Return a human-readable name describe the type of the network,
     * for example "WIFI" or "MOBILE".
     * @return the name of the network type
     */
    CARAPI GetTypeName(
        /* [out] */ String* result);

    /**
     * Return a human-readable name describing the subtype of the network.
     * @return the name of the network subtype
     */
    CARAPI GetSubtypeName(
        /* [out] */ String* result);

    /**
     * Indicates whether network connectivity exists or is in the process
     * of being established. This is good for applications that need to
     * do anything related to the network other than read or write data.
     * For the latter, call {@link #isConnected()} instead, which guarantees
     * that the network is fully usable.
     * @return {@code true} if network connectivity exists or is in the process
     * of being established, {@code false} otherwise.
     */
    CARAPI IsConnectedOrConnecting(
        /* [out] */ Boolean* result);

    /**
     * Indicates whether network connectivity exists and it is possible to establish
     * connections and pass data.
     * <p>Always call this before attempting to perform data transactions.
     * @return {@code true} if network connectivity exists, {@code false} otherwise.
     */
    CARAPI IsConnected(
        /* [out] */ Boolean* result);

    /**
     * Indicates whether network connectivity is possible. A network is unavailable
     * when a persistent or semi-persistent condition prevents the possibility
     * of connecting to that network. Examples include
     * <ul>
     * <li>The device is out of the coverage area for any network of this type.</li>
     * <li>The device is on a network other than the home network (i.e., roaming), and
     * data roaming has been disabled.</li>
     * <li>The device's radio is turned off, e.g., because airplane mode is enabled.</li>
     * </ul>
     * @return {@code true} if the network is available, {@code false} otherwise
     */
    CARAPI IsAvailable(
        /* [out] */ Boolean* result);

    /**
     * Sets if the network is available, ie, if the connectivity is possible.
     * @param isAvailable the new availability value.
     *
     * @hide
     */
    CARAPI SetIsAvailable(
        /* [in] */ Boolean isAvailable);

    /**
     * Indicates whether the current attempt to connect to the network
     * resulted from the ConnectivityManager trying to fail over to this
     * network following a disconnect from another network.
     * @return {@code true} if this is a failover attempt, {@code false}
     * otherwise.
     */
    CARAPI IsFailover(
        /* [out] */ Boolean* result);

    /**
     * Set the failover boolean.
     * @param isFailover {@code true} to mark the current connection attempt
     * as a failover.
     * @hide
     */
    CARAPI SetFailover(
        /* [in] */ Boolean isFailover);

    /**
     * Indicates whether the device is currently roaming on this network.
     * When {@code true}, it suggests that use of data on this network
     * may incur extra costs.
     * @return {@code true} if roaming is in effect, {@code false} otherwise.
     */
    CARAPI IsRoaming(
        /* [out] */ Boolean* result);

    /** {@hide} */
    // @VisibleForTesting
    CARAPI SetRoaming(
        /* [in] */ Boolean isRoaming);

    /** {@hide} */
    // @VisibleForTesting
    CARAPI IsConnectedToProvisioningNetwork(
        /* [out] */ Boolean* result);

    /** {@hide} */
    // @VisibleForTesting
    CARAPI SetIsConnectedToProvisioningNetwork(
        /* [in] */ Boolean val);

    /**
     * Reports the current coarse-grained state of the network.
     * @return the coarse-grained state
     */
    CARAPI GetState(
        /* [out] */ NetworkInfoState* result);

    /**
     * Reports the current fine-grained state of the network.
     * @return the fine-grained state
     */
    CARAPI GetDetailedState(
        /* [out] */ NetworkInfoDetailedState* result);

    /**
     * Sets the fine-grained state of the network.
     * @param detailedState the {@link DetailedState}.
     * @param reason a {@code String} indicating the reason for the state change,
     * if one was supplied. May be {@code null}.
     * @param extraInfo an optional {@code String} providing addditional network state
     * information passed up from the lower networking layers.
     * @hide
     */
    CARAPI SetDetailedState(
        /* [in] */ NetworkInfoDetailedState detailedState,
        /* [in] */ const String& reason,
        /* [in] */ const String& extraInfo);

    /**
     * Set the extraInfo field.
     * @param extraInfo an optional {@code String} providing addditional network state
     * information passed up from the lower networking layers.
     * @hide
     */
    CARAPI SetExtraInfo(
        /* [in] */ const String& extraInfo);

    /**
     * Report the reason an attempt to establish connectivity failed,
     * if one is available.
     * @return the reason for failure, or null if not available
     */
    CARAPI GetReason(
        /* [out] */ String* result);

    /**
     * Report the extra information about the network state, if any was
     * provided by the lower networking layers.
     * @return the extra information, or null if not available
     */
    CARAPI GetExtraInfo(
        /* [out] */ String* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    /**
     * Implement the Parcelable interface.
     * @hide
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:

    // for initialize STATE_MAP
    static CARAPI_(Int32) InternalInit();

    /**
     * This is the map described in the Javadoc comment above. The positions
     * of the elements of the array must correspond to the ordinal values
     * of <code>DetailedState</code>.
     */
    static /* const */ HashMap<NetworkInfoDetailedState, NetworkInfoState> mStateMap;

    static const Int32 mInitFlag;

    Int32 mNetworkType;

    Int32 mSubtype;

    String mTypeName;

    String mSubtypeName;

    NetworkInfoState mState;

    NetworkInfoDetailedState mDetailedState;

    String mReason;

    String mExtraInfo;

    Boolean mIsFailover;

    Boolean mIsRoaming;

    Boolean mIsConnectedToProvisioningNetwork;

    /**
     * Indicates whether network connectivity is possible:
     */
    Boolean mIsAvailable;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKINFO_H__
