
#ifndef __ELASTOS_DROID_NET_CPROXYDATATRACKER_H__
#define __ELASTOS_DROID_NET_CPROXYDATATRACKER_H__

#include "_Elastos_Droid_Net_CProxyDataTracker.h"
#include "elastos/droid/net/ProxyDataTracker.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * A data tracker responsible for bringing up and tearing down the system proxy server.
 *
 * {@hide}
 */
CarClass(CProxyDataTracker)
    , public ProxyDataTracker
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_CPROXYDATATRACKER_H__
