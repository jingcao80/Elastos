
#ifndef __ELASTOS_DROID_NET_CNETWORKPOLICYMANAGER_H__
#define __ELASTOS_DROID_NET_CNETWORKPOLICYMANAGER_H__

#include "_Elastos_Droid_Net_CNetworkPolicyManager.h"
#include "elastos/droid/net/NetworkPolicyManager.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Manager for creating and modifying network policy rules.
 *
 * {@hide}
 */
CarClass(CNetworkPolicyManager)
    , public NetworkPolicyManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKPOLICYMANAGER_H__
