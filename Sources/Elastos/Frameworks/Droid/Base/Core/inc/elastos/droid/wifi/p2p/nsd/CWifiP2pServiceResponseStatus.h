
#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICERESPONSESTATUS_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICERESPONSESTATUS_H__

#include "_Elastos_Droid_Wifi_P2p_Nsd_CWifiP2pServiceResponseStatus.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CarClass(CWifiP2pServiceResponseStatus)
    , public Object
    , public IWifiP2pServiceResponseStatus
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    constructor();

    CARAPI ToString(
        /* [in] */ Int32 status,
        /* [out] */ String* pString);
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICERESPONSESTATUS_H__
