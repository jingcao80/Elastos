
#ifndef __ELASTOS_DROID_NET_CNETWORKREQUESTBUILDER_H__
#define __ELASTOS_DROID_NET_CNETWORKREQUESTBUILDER_H__

#include "_Elastos_Droid_Net_CNetworkRequestBuilder.h"
#include "elastos/droid/net/NetworkRequest.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * Builder used to create {@link NetworkRequest} objects.  Specify the Network features
 * needed in terms of {@link NetworkCapabilities} features
 */
// inner class of NetworkRequest
CarClass(CNetworkRequestBuilder)
    , public NetworkRequestBuilder
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CNETWORKREQUESTBUILDER_H__
