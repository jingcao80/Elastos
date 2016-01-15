
#ifndef __ELASTOS_DROID_NET_CNETWORKSTATE_H__
#define __ELASTOS_DROID_NET_CNETWORKSTATE_H__

#include "_Elastos_Droid_Net_CNetworkState.h"
#include "elastos/droid/net/NetworkState.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Snapshot of network state.
 *
 * @hide
 */
CarClass(CNetworkState)
    , public NetworkState
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKSTATE_H__
