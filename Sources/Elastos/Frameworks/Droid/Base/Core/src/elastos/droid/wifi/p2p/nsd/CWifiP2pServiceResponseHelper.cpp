#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceResponseHelper.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceResponse.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL(CWifiP2pServiceResponseHelper, Singleton, IWifiP2pServiceResponseHelper)

CAR_SINGLETON_IMPL(CWifiP2pServiceResponseHelper)

ECode CWifiP2pServiceResponseHelper::NewInstance(
    /* [in] */ const String& supplicantEvent,
    /* [out] */ IList** list)
{
    assert(0);
    // TODO
    // return CWifiP2pServiceResponse::NewInstance(supplicantEvent, list);
    return E_NOT_IMPLEMENTED;
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
