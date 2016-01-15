
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pUpnpServiceInfoHelper.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pUpnpServiceInfo.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL(CWifiP2pUpnpServiceInfoHelper, Singleton, IWifiP2pUpnpServiceInfoHelper)

CAR_SINGLETON_IMPL(CWifiP2pUpnpServiceInfoHelper)

ECode CWifiP2pUpnpServiceInfoHelper::NewInstance(
    /* [in] */ const String& uuid,
    /* [in] */ const String& device,
    /* [in] */ IList* services,
    /* [out] */ IWifiP2pUpnpServiceInfo** instance)
{
    assert(0);
    // TODO
    // return CWifiP2pUpnpServiceInfo::NewInstance(uuid, device, services, instance);
    return E_NOT_IMPLEMENTED;
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
