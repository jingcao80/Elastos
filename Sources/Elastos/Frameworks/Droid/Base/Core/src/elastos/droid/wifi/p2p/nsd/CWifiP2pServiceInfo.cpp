
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceInfo.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_OBJECT_IMPL(CWifiP2pServiceInfo)

ECode CWifiP2pServiceInfo::GetSupplicantQueryList(
    /* [out, callee] */ ArrayOf<String>** list)
{
    return WifiP2pServiceInfo::GetSupplicantQueryList(list);
}

ECode CWifiP2pServiceInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    return WifiP2pServiceInfo::Equals(obj, isEqual);
}

ECode CWifiP2pServiceInfo::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return WifiP2pServiceInfo::GetHashCode(hashCode);
}

ECode CWifiP2pServiceInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return WifiP2pServiceInfo::ReadFromParcel(source);
}

ECode CWifiP2pServiceInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return WifiP2pServiceInfo::WriteToParcel(dest);
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
