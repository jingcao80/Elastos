
#ifndef __ELASTOS_DROID_NET_CIPCONFIGURATION_H__
#define __ELASTOS_DROID_NET_CIPCONFIGURATION_H__

#include "_Elastos_Droid_Net_CIpConfiguration.h"
#include "elastos/droid/net/IpConfiguration.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A class representing a configured network.
 * @hide
 */
CarClass(CIpConfiguration)
    , public IpConfiguration
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CIPCONFIGURATION_H__
