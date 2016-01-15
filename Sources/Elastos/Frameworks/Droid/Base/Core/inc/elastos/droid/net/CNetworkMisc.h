
#ifndef __ELASTOS_DROID_NET_CNETWORKMISC_H__
#define __ELASTOS_DROID_NET_CNETWORKMISC_H__

#include "_Elastos_Droid_Net_CNetworkMisc.h"
#include "elastos/droid/net/NetworkMisc.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A grab-bag of information (metadata, policies, properties, etc) about a {@link Network}.
 *
 * @hide
 */
CarClass(CNetworkMisc)
    , public NetworkMisc
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKMISC_H__
