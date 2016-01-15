#include "elastos/droid/wifi/p2p/nsd/CWifiP2pDnsSdServiceResponseHelper.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pDnsSdServiceResponse.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

ECode CWifiP2pDnsSdServiceResponseHelper::NewInstance(
    /* [in] */ Int32 status,
    /* [in] */ Int32 transId,
    /* [in] */ IWifiP2pDevice* dev,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IWifiP2pDnsSdServiceResponse** instance)
{
    return CWifiP2pDnsSdServiceResponse::NewInstance(status, transId, dev, data, instance);
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
