
#ifndef __ELASTOS_DROID_NET_CNETWORKCONFIG_H__
#define __ELASTOS_DROID_NET_CNETWORKCONFIG_H__

#include "_Elastos_Droid_Net_CNetworkConfig.h"
#include "elastos/droid/net/NetworkConfig.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Describes the buildtime configuration of a network.
 * Holds settings read from resources.
 * @hide
 */
CarClass(CNetworkConfig)
    , public NetworkConfig
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKCONFIG_H__
