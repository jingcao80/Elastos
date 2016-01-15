
#ifndef __ELASTOS_DROID_NET_CLINKPROPERTIES_H__
#define __ELASTOS_DROID_NET_CLINKPROPERTIES_H__

#include "_Elastos_Droid_Net_CLinkProperties.h"
#include "LinkProperties.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Describes the properties of a network link.
 *
 * A link represents a connection to a network.
 * It may have multiple addresses and multiple gateways,
 * multiple dns servers but only one http proxy and one
 * network interface.
 *
 * Note that this is just a holder of data.  Modifying it
 * does not affect live networks.
 *
 */
CarClass(CLinkProperties)
    , public LinkProperties
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CLINKPROPERTIES_H__
