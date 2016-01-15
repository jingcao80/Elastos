
#ifndef __ELASTOS_DROID_NET_NETWORKCAPABILITIES_H__
#define __ELASTOS_DROID_NET_NETWORKCAPABILITIES_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * This class represents the capabilities of a network.  This is used both to specify
 * needs to {@link ConnectivityManager} and when inspecting a network.
 *
 * Note that this replaces the old {@link ConnectivityManager#TYPE_MOBILE} method
 * of network selection.  Rather than indicate a need for Wi-Fi because an application
 * needs high bandwidth and risk obselence when a new, fast network appears (like LTE),
 * the application should specify it needs high bandwidth.  Similarly if an application
 * needs an unmetered network for a bulk transfer it can specify that rather than assuming
 * all cellular based connections are metered and all Wi-Fi based connections are not.
 */
class NetworkCapabilities
    : public Object
    , public IParcelable
    , public INetworkCapabilities
{
public:
    CAR_INTERFACE_DECL()

    NetworkCapabilities();

    /**
     * @hide
     */
    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ INetworkCapabilities* nc);

    /**
     * Adds the given capability to this {@code NetworkCapability} instance.
     * Multiple capabilities may be applied sequentially.  Note that when searching
     * for a network to satisfy a request, all capabilities requested must be satisfied.
     *
     * @param capability the {@code NetworkCapabilities.NET_CAPABILITY_*} to be added.
     * @return This NetworkCapability to facilitate chaining.
     * @hide
     */
    CARAPI AddCapability(
        /* [in] */ Int32 capability);

    /**
     * Removes (if found) the given capability from this {@code NetworkCapability} instance.
     *
     * @param capability the {@code NetworkCapabilities.NET_CAPABILTIY_*} to be removed.
     * @return This NetworkCapability to facilitate chaining.
     * @hide
     */
    CARAPI RemoveCapability(
        /* [in] */ Int32 capability);

    /**
     * Gets all the capabilities set on this {@code NetworkCapability} instance.
     *
     * @return an array of {@code NetworkCapabilities.NET_CAPABILITY_*} values
     *         for this instance.
     * @hide
     */
    CARAPI GetCapabilities(
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Tests for the presence of a capabilitity on this instance.
     *
     * @param capability the {@code NetworkCapabilities.NET_CAPABILITY_*} to be tested for.
     * @return {@code true} if set on this instance.
     */
    CARAPI HasCapability(
        /* [in] */ Int32 capability,
        /* [out] */ Boolean* result);

    /**
     * Adds the given transport type to this {@code NetworkCapability} instance.
     * Multiple transports may be applied sequentially.  Note that when searching
     * for a network to satisfy a request, any listed in the request will satisfy the request.
     * For example {@code TRANSPORT_WIFI} and {@code TRANSPORT_ETHERNET} added to a
     * {@code NetworkCapabilities} would cause either a Wi-Fi network or an Ethernet network
     * to be selected.  This is logically different than
     * {@code NetworkCapabilities.NET_CAPABILITY_*} listed above.
     *
     * @param transportType the {@code NetworkCapabilities.TRANSPORT_*} to be added.
     * @return This NetworkCapability to facilitate chaining.
     * @hide
     */
    CARAPI AddTransportType(
        /* [in] */ Int32 transportType);

    /**
     * Removes (if found) the given transport from this {@code NetworkCapability} instance.
     *
     * @param transportType the {@code NetworkCapabilities.TRANSPORT_*} to be removed.
     * @return This NetworkCapability to facilitate chaining.
     * @hide
     */
    CARAPI RemoveTransportType(
        /* [in] */ Int32 transportType);

    /**
     * Gets all the transports set on this {@code NetworkCapability} instance.
     *
     * @return an array of {@code NetworkCapabilities.TRANSPORT_*} values
     *         for this instance.
     * @hide
     */
    CARAPI GetTransportTypes(
        /* [out, callee] */ ArrayOf<Int32>** result);

    /**
     * Tests for the presence of a transport on this instance.
     *
     * @param transportType the {@code NetworkCapabilities.TRANSPORT_*} to be tested for.
     * @return {@code true} if set on this instance.
     */
    CARAPI HasTransport(
        /* [in] */ Int32 transportType,
        /* [out] */ Boolean* result);

    /**
     * Sets the upstream bandwidth for this network in Kbps.  This always only refers to
     * the estimated first hop transport bandwidth.
     * <p>
     * Note that when used to request a network, this specifies the minimum acceptable.
     * When received as the state of an existing network this specifies the typical
     * first hop bandwidth expected.  This is never measured, but rather is inferred
     * from technology type and other link parameters.  It could be used to differentiate
     * between very slow 1xRTT cellular links and other faster networks or even between
     * 802.11b vs 802.11AC wifi technologies.  It should not be used to differentiate between
     * fast backhauls and slow backhauls.
     *
     * @param upKbps the estimated first hop upstream (device to network) bandwidth.
     * @hide
     */
    CARAPI SetLinkUpstreamBandwidthKbps(
        /* [in] */ Int32 upKbps);

    /**
     * Retrieves the upstream bandwidth for this network in Kbps.  This always only refers to
     * the estimated first hop transport bandwidth.
     *
     * @return The estimated first hop upstream (device to network) bandwidth.
     */
    CARAPI GetLinkUpstreamBandwidthKbps(
        /* [out] */ Int32* result);

    /**
     * Sets the downstream bandwidth for this network in Kbps.  This always only refers to
     * the estimated first hop transport bandwidth.
     * <p>
     * Note that when used to request a network, this specifies the minimum acceptable.
     * When received as the state of an existing network this specifies the typical
     * first hop bandwidth expected.  This is never measured, but rather is inferred
     * from technology type and other link parameters.  It could be used to differentiate
     * between very slow 1xRTT cellular links and other faster networks or even between
     * 802.11b vs 802.11AC wifi technologies.  It should not be used to differentiate between
     * fast backhauls and slow backhauls.
     *
     * @param downKbps the estimated first hop downstream (network to device) bandwidth.
     * @hide
     */
    CARAPI SetLinkDownstreamBandwidthKbps(
        /* [in] */ Int32 downKbps);

    /**
     * Retrieves the downstream bandwidth for this network in Kbps.  This always only refers to
     * the estimated first hop transport bandwidth.
     *
     * @return The estimated first hop downstream (network to device) bandwidth.
     */
    CARAPI GetLinkDownstreamBandwidthKbps(
        /* [out] */ Int32* result);

    /**
     * Sets the optional bearer specific network specifier.
     * This has no meaning if a single transport is also not specified, so calling
     * this without a single transport set will generate an exception, as will
     * subsequently adding or removing transports after this is set.
     * </p>
     * The interpretation of this {@code String} is bearer specific and bearers that use
     * it should document their particulars.  For example, Bluetooth may use some sort of
     * device id while WiFi could used SSID and/or BSSID.  Cellular may use carrier SPN (name)
     * or Subscription ID.
     *
     * @param networkSpecifier An {@code String} of opaque format used to specify the bearer
     *                         specific network specifier where the bearer has a choice of
     *                         networks.
     * @hide
     */
    CARAPI SetNetworkSpecifier(
        /* [in] */ const String& networkSpecifier);

    /**
     * Gets the optional bearer specific network specifier.
     *
     * @return The optional {@code String} specifying the bearer specific network specifier.
     *         See {@link #setNetworkSpecifier}.
     * @hide
     */
    CARAPI GetNetworkSpecifier(
        /* [out] */ String* result);

    /**
     * Combine a set of Capabilities to this one.  Useful for coming up with the complete set
     * {@hide}
     */
    CARAPI CombineCapabilities(
        /* [in] */ INetworkCapabilities* nc);

    /**
     * Check if our requirements are satisfied by the given Capabilities.
     * {@hide}
     */
    CARAPI SatisfiedByNetworkCapabilities(
        /* [in] */ INetworkCapabilities* nc,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* result);

private:
    CARAPI_(AutoPtr<ArrayOf<Int32> >) EnumerateBits(
        /* [in] */ Int64 val);

    CARAPI CombineNetCapabilities(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI_(Boolean) SatisfiedByNetCapabilities(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI_(Boolean) EqualsNetCapabilities(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI CombineTransportTypes(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI_(Boolean) SatisfiedByTransportTypes(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI_(Boolean) EqualsTransportTypes(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI CombineLinkBandwidths(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI_(Boolean) SatisfiedByLinkBandwidths(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI_(Boolean) EqualsLinkBandwidths(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI CombineSpecifiers(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI_(Boolean) SatisfiedBySpecifier(
        /* [in] */ INetworkCapabilities* nc);

    CARAPI_(Boolean) EqualsSpecifier(
        /* [in] */ INetworkCapabilities* nc);

private:
    static const String TAG;

    static const Boolean DBG;

    static const Int32 MIN_NET_CAPABILITY;

    static const Int32 MAX_NET_CAPABILITY;

    /**
     * Representing the transport type.  Apps should generally not care about transport.  A
     * request for a fast internet connection could be satisfied by a number of different
     * transports.  If any are specified here it will be satisfied a Network that matches
     * any of them.  If a caller doesn't care about the transport it should not specify any.
     */
    Int64 mTransportTypes;

    static const Int32 MIN_TRANSPORT;

    static const Int32 MAX_TRANSPORT;

    /**
     * Passive link bandwidth.  This is a rough guide of the expected peak bandwidth
     * for the first hop on the given transport.  It is not measured, but may take into account
     * link parameters (Radio technology, allocated channels, etc).
     */
    Int32 mLinkUpBandwidthKbps;

    Int32 mLinkDownBandwidthKbps;

    String mNetworkSpecifier;

    /**
     * Represents the network's capabilities.  If any are specified they will be satisfied
     * by any Network that matches all of them.
     */
    Int64 mNetworkCapabilities;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKCAPABILITIES_H__
