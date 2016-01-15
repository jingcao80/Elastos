
#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEINFOHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEINFOHELPER_H__

#include "_Elastos_Droid_Wifi_P2p_Nsd_CWifiP2pDnsSdServiceInfoHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CarClass(CWifiP2pDnsSdServiceInfoHelper)
    , public Singleton
    , public IWifiP2pDnsSdServiceInfoHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI NewInstance(
        /* [in] */ const String& instanceName,
        /* [in] */ const String& serviceType,
        /* [in] */ IMap* txtMap,
        /* [out] */ IWifiP2pDnsSdServiceInfo** instance);

    CARAPI CreateRequest(
        /* [in] */ const String& dnsName,
        /* [in] */ const Int32 dnsType,
        /* [in] */ const Int32 versionNumber,
        /* [out] */ String* request);
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEINFOHELPER_H__
