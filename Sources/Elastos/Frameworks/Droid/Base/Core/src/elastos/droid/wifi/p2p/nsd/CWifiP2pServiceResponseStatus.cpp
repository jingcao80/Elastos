
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceResponseStatus.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL(CWifiP2pServiceResponseStatus, Object, IWifiP2pServiceResponseStatus)

CAR_OBJECT_IMPL(CWifiP2pServiceResponseStatus)

ECode CWifiP2pServiceResponseStatus::constructor()
{
    return NOERROR;
}

ECode CWifiP2pServiceResponseStatus::ToString(
    /* [in] */ Int32 status,
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    switch(status) {
        case IWifiP2pServiceResponseStatus::SUCCESS:
            *string = String("SUCCESS");
            break;
        case IWifiP2pServiceResponseStatus::SERVICE_PROTOCOL_NOT_AVAILABLE:
            *string = String("SERVICE_PROTOCOL_NOT_AVAILABLE");
            break;
        case IWifiP2pServiceResponseStatus::REQUESTED_INFORMATION_NOT_AVAILABLE:
            *string = String("REQUESTED_INFORMATION_NOT_AVAILABLE");
            break;
        case IWifiP2pServiceResponseStatus::BAD_REQUEST:
            *string = String("BAD_REQUEST");
            break;
        default:
            *string = String("UNKNOWN");
            break;
    }
    return NOERROR;
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
