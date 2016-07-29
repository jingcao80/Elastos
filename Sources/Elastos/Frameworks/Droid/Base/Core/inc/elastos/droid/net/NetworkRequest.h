
#ifndef __ELASTOS_DROID_NET_NETWORKREQUEST_H__
#define __ELASTOS_DROID_NET_NETWORKREQUEST_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Defines a request for a network, made through {@link NetworkRequest.Builder} and used
 * to request a network via {@link ConnectivityManager#requestNetwork} or listen for changes
 * via {@link ConnectivityManager#registerNetworkCallback}.
 */
class NetworkRequest
    : public Object
    , public IParcelable
    , public INetworkRequest
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();
    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ INetworkCapabilities* nc,
        /* [in] */ Int32 legacyType,
        /* [in] */ Int32 rId);

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ INetworkRequest* that);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    /**
     * The {@link NetworkCapabilities} that define this request.
     * @hide
     */
    CARAPI GetNetworkCapabilities(
        /* [out] */ INetworkCapabilities** result);

    /**
     * Identifies the request.  NetworkRequests should only be constructed by
     * the Framework and given out to applications as tokens to be used to identify
     * the request.
     * @hide
     */
    CARAPI GetRequestId(
        /* [out] */ Int32* result);

    /**
     * Set for legacy requests and the default.  Set to TYPE_NONE for none.
     * Causes CONNECTIVITY_ACTION broadcasts to be sent.
     * @hide
     */
    CARAPI GetLegacyType(
        /* [out] */ Int32* result);

public:
    /**
     * Identifies the request.  NetworkRequests should only be constructed by
     * the Framework and given out to applications as tokens to be used to identify
     * the request.
     * @hide
     */
    Int32 mRequestId;

private:
    /**
     * The {@link NetworkCapabilities} that define this request.
     * @hide
     */
    AutoPtr<INetworkCapabilities> mNetworkCapabilities;

    /**
     * Set for legacy requests and the default.  Set to TYPE_NONE for none.
     * Causes CONNECTIVITY_ACTION broadcasts to be sent.
     * @hide
     */
    Int32 mLegacyType;
};


/**
 * Builder used to create {@link NetworkRequest} objects.  Specify the Network features
 * needed in terms of {@link NetworkCapabilities} features
 */
class NetworkRequestBuilder
    : public Object
    , public INetworkRequestBuilder
{
public:
    CAR_INTERFACE_DECL()

    NetworkRequestBuilder();

    /**
     * Default constructor for Builder.
     */
    CARAPI constructor();

    /**
     * Build {@link NetworkRequest} give the current set of capabilities.
     */
    CARAPI Build(
        /* [out] */ INetworkRequest** result);

    /**
     * Add the given capability requirement to this builder.  These represent
     * the requested network's required capabilities.  Note that when searching
     * for a network to satisfy a request, all capabilities requested must be
     * satisfied.  See {@link NetworkCapabilities} for {@code NET_CAPABILITIY_*}
     * definitions.
     *
     * @param capability The {@code NetworkCapabilities.NET_CAPABILITY_*} to add.
     * @return The builder to facilitate chaining
     *         {@code builder.addCapability(...).addCapability();}.
     */
    CARAPI AddCapability(
        /* [in] */ Int32 capability);

    /**
     * Removes (if found) the given capability from this builder instance.
     *
     * @param capability The {@code NetworkCapabilities.NET_CAPABILITY_*} to remove.
     * @return The builder to facilitate chaining.
     */
    CARAPI RemoveCapability(
        /* [in] */ Int32 capability);

    /**
     * Adds the given transport requirement to this builder.  These represent
     * the set of allowed transports for the request.  Only networks using one
     * of these transports will satisfy the request.  If no particular transports
     * are required, none should be specified here.  See {@link NetworkCapabilities}
     * for {@code TRANSPORT_*} definitions.
     *
     * @param transportType The {@code NetworkCapabilities.TRANSPORT_*} to add.
     * @return The builder to facilitate chaining.
     */
    CARAPI AddTransportType(
        /* [in] */ Int32 transportType);

    /**
     * Removes (if found) the given transport from this builder instance.
     *
     * @param transportType The {@code NetworkCapabilities.TRANSPORT_*} to remove.
     * @return The builder to facilitate chaining.
     */
    CARAPI RemoveTransportType(
        /* [in] */ Int32 transportType);

    /**
     * @hide
     */
    CARAPI SetLinkUpstreamBandwidthKbps(
        /* [in] */ Int32 upKbps);

    /**
     * @hide
     */
    CARAPI SetLinkDownstreamBandwidthKbps(
        /* [in] */ Int32 downKbps);

    /**
     * Sets the optional bearer specific network specifier.
     * This has no meaning if a single transport is also not specified, so calling
     * this without a single transport set will generate an exception, as will
     * subsequently adding or removing transports after this is set.
     * </p>
     * The interpretation of this {@code String} is bearer specific and bearers that use
     * it should document their particulars.  For example, Bluetooth may use some sort of
     * device id while WiFi could used ssid and/or bssid.  Cellular may use carrier spn.
     *
     * @param networkSpecifier An {@code String} of opaque format used to specify the bearer
     *                         specific network specifier where the bearer has a choice of
     *                         networks.
     */
    CARAPI SetNetworkSpecifier(
        /* [in] */ const String& networkSpecifier);

private:
    CARAPI_(AutoPtr<INetworkCapabilities>) CreateNetworkCapabilities();
    /* const */ AutoPtr<INetworkCapabilities> mNetworkCapabilities;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKREQUEST_H__
