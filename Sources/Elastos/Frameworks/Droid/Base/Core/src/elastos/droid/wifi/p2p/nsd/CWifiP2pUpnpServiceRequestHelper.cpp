
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pUpnpServiceRequestHelper.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pUpnpServiceRequest.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL(CWifiP2pUpnpServiceRequestHelper, Singleton, IWifiP2pUpnpServiceRequestHelper)

CAR_SINGLETON_IMPL(CWifiP2pUpnpServiceRequestHelper)

ECode CWifiP2pUpnpServiceRequestHelper::NewInstance(
    /* [out] */ IWifiP2pUpnpServiceRequest** instance)
{
    return CWifiP2pUpnpServiceRequest::NewInstance(instance);
}

ECode CWifiP2pUpnpServiceRequestHelper::NewInstance(
    /* [in] */ const String& st,
    /* [out] */ IWifiP2pUpnpServiceRequest** instance)
{
    return CWifiP2pUpnpServiceRequest::NewInstance(st, instance);
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
