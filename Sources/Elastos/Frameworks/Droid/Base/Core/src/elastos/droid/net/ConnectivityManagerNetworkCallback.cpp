
#include "elastos/droid/net/ConnectivityManagerNetworkCallback.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(ConnectivityManagerNetworkCallback, Object, IConnectivityManagerNetworkCallback)
ECode ConnectivityManagerNetworkCallback::constructor()
{
    return NOERROR;
}

ECode ConnectivityManagerNetworkCallback::OnPreCheck(
    /* [in] */ INetwork* network)
{
    return NOERROR;
}


ECode ConnectivityManagerNetworkCallback::OnAvailable(
            /* [in] */ INetwork* network)
{
    return NOERROR;
}

ECode ConnectivityManagerNetworkCallback::OnLosing(
    /* [in] */ INetwork* network,
    /* [in] */ Int32 maxMsToLive)
{
    return NOERROR;
}

ECode ConnectivityManagerNetworkCallback::OnLost(
    /* [in] */ INetwork* network)
{
    return NOERROR;
}

ECode ConnectivityManagerNetworkCallback::OnUnavailable()
{
    return NOERROR;
}

ECode ConnectivityManagerNetworkCallback::OnCapabilitiesChanged(
    /* [in] */ INetwork* network,
    /* [in] */ INetworkCapabilities* networkCapabilities)
{
    return NOERROR;
}

ECode ConnectivityManagerNetworkCallback::OnLinkPropertiesChanged(
    /* [in] */ INetwork* network,
    /* [in] */ ILinkProperties* linkProperties)
{
    return NOERROR;
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
