
#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICEREQUESTHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICEREQUESTHELPER_H__

#include "_Elastos_Droid_Wifi_P2p_Nsd_CWifiP2pServiceRequestHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CarClass(CWifiP2pServiceRequestHelper)
    , public Singleton
    , public IWifiP2pServiceRequestHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewInstance(
        /* [in] */ Int32 protocolType,
        /* [in] */ const String& queryData,
        /* [out] */ IWifiP2pServiceRequest** instance);

    CARAPI NewInstance(
        /* [in] */ Int32 protocolType,
        /* [out] */ IWifiP2pServiceRequest** instance);
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICEREQUESTHELPER_H__
