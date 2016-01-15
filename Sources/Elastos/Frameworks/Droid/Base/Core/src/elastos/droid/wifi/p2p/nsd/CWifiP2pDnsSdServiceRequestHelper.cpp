
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pDnsSdServiceRequestHelper.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pDnsSdServiceRequest.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL(CWifiP2pDnsSdServiceRequestHelper, Singleton, IWifiP2pDnsSdServiceRequestHelper)

CAR_SINGLETON_IMPL(CWifiP2pDnsSdServiceRequestHelper)

ECode CWifiP2pDnsSdServiceRequestHelper::NewInstance(
    /* [out] */ IWifiP2pDnsSdServiceRequest** instance)
{
    return CWifiP2pDnsSdServiceRequest::NewInstance(instance);
}

ECode CWifiP2pDnsSdServiceRequestHelper::NewInstance(
    /* [in] */ const String& serviceType,
    /* [out] */ IWifiP2pDnsSdServiceRequest** instance)
{
    return CWifiP2pDnsSdServiceRequest::NewInstance(serviceType, instance);
}

ECode CWifiP2pDnsSdServiceRequestHelper::NewInstance(
    /* [in] */ const String& instanceName,
    /* [in] */ const String& serviceType,
    /* [out] */ IWifiP2pDnsSdServiceRequest** instance)
{
    return CWifiP2pDnsSdServiceRequest::NewInstance(
        instanceName, serviceType, instance);
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
