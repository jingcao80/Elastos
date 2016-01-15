
#ifndef __ELASTOS_DROID_NET_CVPNSERVICEBUILDER_H__
#define __ELASTOS_DROID_NET_CVPNSERVICEBUILDER_H__

#include "_Elastos_Droid_Net_CVpnServiceBuilder.h"
#include "elastos/droid/net/VpnService.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Helper class to create a VPN interface. This class should be always
 * used within the scope of the outer {@link VpnService}.
 *
 * @see VpnService
 */
CarClass(CVpnServiceBuilder)
    , public VpnServiceBuilder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CVPNSERVICEBUILDER_H__
