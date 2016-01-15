
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pUpnpServiceResponseHelper.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pUpnpServiceResponse.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

ECode CWifiP2pUpnpServiceResponseHelper::NewInstance(
    /* [in] */ Int32 status,
    /* [in] */ Int32 transId,
    /* [in] */ IWifiP2pDevice* dev,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IWifiP2pUpnpServiceResponse** instance)
{
    return CWifiP2pUpnpServiceResponse::NewInstance(
        status, transId, dev, data, instance);
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
