
#ifndef __ELASTOS_DROID_NET_NSD_CNSDSERVICEINFO_H__
#define __ELASTOS_DROID_NET_NSD_CNSDSERVICEINFO_H__

#include "_Elastos_Droid_Net_Nsd_CNsdServiceInfo.h"
#include "elastos/droid/net/nsd/NsdServiceInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Nsd {

/**
 * A class representing service information for network service discovery
 * {@see NsdManager}
 */
CarClass(CNsdServiceInfo)
    , public NsdServiceInfo
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Nsd
} // namespace Net
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_NSD_CNSDSERVICEINFO_H__
