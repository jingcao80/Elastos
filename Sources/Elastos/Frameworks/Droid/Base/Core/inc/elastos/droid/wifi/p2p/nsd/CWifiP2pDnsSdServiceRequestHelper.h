
#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEREQUESTHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEREQUESTHELPER_H__

#include "_Elastos_Droid_Wifi_P2p_Nsd_CWifiP2pDnsSdServiceRequestHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CarClass(CWifiP2pDnsSdServiceRequestHelper)
    , public Singleton
    , public IWifiP2pDnsSdServiceRequestHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewInstance(
        /* [out] */ IWifiP2pDnsSdServiceRequest** instance);

    CARAPI NewInstance(
        /* [in] */ const String& serviceType,
        /* [out] */ IWifiP2pDnsSdServiceRequest** instance);

    CARAPI NewInstance(
        /* [in] */ const String& instanceName,
        /* [in] */ const String& serviceType,
        /* [out] */ IWifiP2pDnsSdServiceRequest** instance);
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEREQUESTHELPER_H__
