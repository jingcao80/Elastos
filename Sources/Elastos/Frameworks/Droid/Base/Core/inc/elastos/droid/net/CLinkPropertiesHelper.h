
#ifndef __ELASTOS_DROID_NET_CLINKPROPERTIESHELPER_H__
#define __ELASTOS_DROID_NET_CLINKPROPERTIESHELPER_H__

#include "_Elastos_Droid_Net_CLinkPropertiesHelper.h"
#include <elastos/core/Singleton.h>

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
CarClass(CLinkPropertiesHelper)
    , public Singleton
    , public ILinkPropertiesHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Check the valid MTU range based on IPv4 or IPv6.
     * @hide
     */
    CARAPI IsValidMtu(
        /* [in] */ Int32 mtu,
        /* [in] */ Boolean ipv6,
        /* [out] */ Boolean* result);
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif //  __ELASTOS_DROID_NET_CLINKPROPERTIESHELPER_H__
