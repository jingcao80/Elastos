
#ifndef __ELASTOS_DROID_NET_CNETWORKPOLICY_H__
#define __ELASTOS_DROID_NET_CNETWORKPOLICY_H__

#include "_Elastos_Droid_Net_CNetworkPolicy.h"
#include "elastos/droid/net/NetworkPolicy.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Policy for networks matching a {@link NetworkTemplate}, including usage cycle
 * and limits to be enforced.
 *
 * @hide
 */
CarClass(CNetworkPolicy)
    , public NetworkPolicy
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKPOLICY_H__
