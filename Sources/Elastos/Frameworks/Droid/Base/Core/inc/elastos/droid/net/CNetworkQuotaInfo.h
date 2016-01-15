
#ifndef __ELASTOS_DROID_NET_CNETWORKQUOTAINFO_H__
#define __ELASTOS_DROID_NET_CNETWORKQUOTAINFO_H__

#include "_Elastos_Droid_Net_CNetworkQuotaInfo.h"
#include "elastos/droid/net/NetworkQuotaInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Information about quota status on a specific network.
 *
 * @hide
 */
CarClass(CNetworkQuotaInfo)
    , public NetworkQuotaInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKQUOTAINFO_H__
