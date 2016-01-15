
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CConnectivityManager.h"
#include "elastos/droid/net/CConnectivityManagerHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CConnectivityManagerHelper, Singleton, IConnectivityManagerHelper)

CAR_SINGLETON_IMPL(CConnectivityManagerHelper)

ECode CConnectivityManagerHelper::IsNetworkTypeValid(
    /* [in] */ Int32 networkType,
    /* [out] */ Boolean* result)
{
    return CConnectivityManager::IsNetworkTypeValid(networkType, result);
}

ECode CConnectivityManagerHelper::GetNetworkTypeName(
    /* [in] */ Int32 networkType,
    /* [out] */ String* result)
{
    return CConnectivityManager::GetNetworkTypeName(networkType, result);
}

ECode CConnectivityManagerHelper::IsNetworkTypeMobile(
    /* [in] */ Int32 networkType,
    /* [out] */ Boolean* result)
{
    return CConnectivityManager::IsNetworkTypeMobile(networkType, result);
}

ECode CConnectivityManagerHelper::IsNetworkTypeWifi(
        /* [in] */ Int32 networkType,
        /* [out] */ Boolean* result)
{
    return CConnectivityManager::IsNetworkTypeWifi(networkType, result);
}

ECode CConnectivityManagerHelper::MaybeMarkCapabilitiesRestricted(
    /* [in] */ INetworkCapabilities* nc)
{
    return CConnectivityManager::MaybeMarkCapabilitiesRestricted(nc);
}

ECode CConnectivityManagerHelper::From(
    /* [in] */ IContext* ctx,
    /* [out] */ IConnectivityManager** result)
{
    return CConnectivityManager::From(ctx, result);
}

ECode CConnectivityManagerHelper::EnforceTetherChangePermission(
    /* [in] */ IContext* context)
{
    return CConnectivityManager::EnforceTetherChangePermission(context);
}

ECode CConnectivityManagerHelper::SetProcessDefaultNetwork(
    /* [in] */ INetwork* network,
    /* [out] */ Boolean* result)
{
    return CConnectivityManager::SetProcessDefaultNetwork(network, result);
}

ECode CConnectivityManagerHelper::GetProcessDefaultNetwork(
        /* [out] */ INetwork** result)
{
    return CConnectivityManager::GetProcessDefaultNetwork(result);
}

ECode CConnectivityManagerHelper::SetProcessDefaultNetworkForHostResolution(
    /* [in] */ INetwork* network,
    /* [out] */ Boolean* result)
{
    return CConnectivityManager::SetProcessDefaultNetworkForHostResolution(network, result);
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
