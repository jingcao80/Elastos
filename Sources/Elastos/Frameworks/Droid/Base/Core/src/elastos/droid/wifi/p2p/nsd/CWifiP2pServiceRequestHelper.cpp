
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceRequestHelper.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceRequest.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL(CWifiP2pServiceRequestHelper, Singleton, IWifiP2pServiceRequestHelper)

CAR_SINGLETON_IMPL(CWifiP2pServiceRequestHelper)

ECode CWifiP2pServiceRequestHelper::NewInstance(
    /* [in] */ Int32 protocolType,
    /* [in] */ const String& queryData,
    /* [out] */ IWifiP2pServiceRequest** instance)
{
    return CWifiP2pServiceRequest::NewInstance(protocolType, queryData, instance);
}

ECode CWifiP2pServiceRequestHelper::NewInstance(
    /* [in] */ Int32 protocolType,
    /* [out] */ IWifiP2pServiceRequest** instance)
{
    return CWifiP2pServiceRequest::NewInstance(protocolType, String(NULL), instance);
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
