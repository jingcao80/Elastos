
#ifndef __ELASTOS_DROID_NET_CNETWORKKEY_H__
#define __ELASTOS_DROID_NET_CNETWORKKEY_H__

#include "_Elastos_Droid_Net_CNetworkKey.h"
#include "elastos/droid/net/NetworkKey.h"

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
CarClass(CNetworkKey)
    , public NetworkKey
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKKEY_H__
