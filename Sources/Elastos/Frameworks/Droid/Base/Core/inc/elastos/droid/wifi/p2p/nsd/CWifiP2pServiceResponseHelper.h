
#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICERESPONSEHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICERESPONSEHELPER_H__

#include "_Elastos_Droid_Wifi_P2p_Nsd_CWifiP2pServiceResponseHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CarClass(CWifiP2pServiceResponseHelper)
    , public Singleton
    , public IWifiP2pServiceResponseHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewInstance(
        /* [in] */ const String& supplicantEvent,
        /* [out] */ IList** list);
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICERESPONSEHELPER_H__
