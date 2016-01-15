
#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PUPNPSERVICEINFOHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PUPNPSERVICEINFOHELPER_H__

#include "_Elastos_Droid_Wifi_P2p_Nsd_CWifiP2pUpnpServiceInfoHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CarClass(CWifiP2pUpnpServiceInfoHelper)
    , public Singleton
    , public IWifiP2pUpnpServiceInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewInstance(
        /* [in] */ const String& uuid,
        /* [in] */ const String& device,
        /* [in] */ IList* services,
        /* [out] */ IWifiP2pUpnpServiceInfo** instance);
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PUPNPSERVICEINFOHELPER_H__
