
#ifndef __ELASTOS_DROID_NET_NETWORKKEY_H__
#define __ELASTOS_DROID_NET_NETWORKKEY_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Information which identifies a specific network.
 *
 * @hide
 */
// @SystemApi
// NOTE: Ideally, we would abstract away the details of what identifies a network of a specific
// type, so that all networks appear the same and can be scored without concern to the network type
// itself. However, because no such cross-type identifier currently exists in the Android framework,
// and because systems might obtain information about networks from sources other than Android
// devices, we need to provide identifying details about each specific network type (wifi, cell,
// etc.) so that clients can pull out these details depending on the type of network.
class NetworkKey
    : public Object
    , public IParcelable
    , public INetworkKey
{
public:
    CAR_INTERFACE_DECL()

    NetworkKey();

    /**
     * Construct a new {@link NetworkKey} for a Wi-Fi network.
     * @param wifiKey the {@link WifiKey} identifying this Wi-Fi network.
     */
    CARAPI constructor(
        /* [in] */ IWifiKey* wifiKey);

    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * The type of this network.
     * @see #TYPE_WIFI
     */
    CARAPI GetType(
        /* [out] */ Int32* result);

    /**
     * Information identifying a Wi-Fi network. Only set when {@link #type} equals
     * {@link #TYPE_WIFI}.
     */
    CARAPI GetWifiKey(
        /* [out] */ IWifiKey** result);

private:
    /**
     * The type of this network.
     * @see #TYPE_WIFI
     */
    Int32 mType;

    /**
     * Information identifying a Wi-Fi network. Only set when {@link #type} equals
     * {@link #TYPE_WIFI}.
     */
    AutoPtr<IWifiKey> mWifiKey;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_NETWORKKEY_H__
