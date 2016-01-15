
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pDnsSdServiceInfoHelper.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pDnsSdServiceInfo.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL(CWifiP2pDnsSdServiceInfoHelper, Singleton, IWifiP2pDnsSdServiceInfoHelper)

CAR_SINGLETON_IMPL(CWifiP2pDnsSdServiceInfoHelper)

ECode CWifiP2pDnsSdServiceInfoHelper::NewInstance(
    /* [in] */ const String& instanceName,
    /* [in] */ const String& serviceType,
    /* [in] */ IMap* txtMap,
    /* [out] */ IWifiP2pDnsSdServiceInfo** instance)
{
    return CWifiP2pDnsSdServiceInfo::NewInstance(
        instanceName, serviceType, txtMap, instance);
}

ECode CWifiP2pDnsSdServiceInfoHelper::CreateRequest(
    /* [in] */ const String& dnsName,
    /* [in] */ const Int32 dnsType,
    /* [in] */ const Int32 versionNumber,
    /* [out] */ String* request)
{
    return CWifiP2pDnsSdServiceInfo::CreateRequest(
        dnsName, dnsType, versionNumber, request);
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
