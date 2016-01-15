
#ifndef __ELASTOS_DROID_NET_CNETWORKIDENTITY_H__
#define __ELASTOS_DROID_NET_CNETWORKIDENTITY_H__

#include "_Elastos_Droid_Net_CNetworkIdentity.h"
#include "elastos/droid/net/NetworkIdentity.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Network definition that includes strong identity. Analogous to combining
 * {@link NetworkInfo} and an IMSI.
 *
 * @hide
 */
CarClass(CNetworkIdentity)
    , public NetworkIdentity
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKIDENTITY_H__
