
#include <CNetworkInterfaceHelper.h>
#include "NetworkInterface.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CNetworkInterfaceHelper, Singleton, INetworkInterfaceHelper)

CAR_SINGLETON_IMPL(CNetworkInterfaceHelper)

ECode CNetworkInterfaceHelper::GetByName(
   /* [in] */ const String& interfaceName,
   /* [out] */ INetworkInterface** networkInterface)
{
    return NetworkInterface::GetByName(interfaceName, networkInterface);
}

ECode CNetworkInterfaceHelper::GetByInetAddress(
   /* [in] */ IInetAddress* address,
   /* [out] */ INetworkInterface** networkInterface)
{
    return NetworkInterface::GetByInetAddress(address, networkInterface);
}

ECode CNetworkInterfaceHelper::GetByIndex(
   /* [in] */ Int32 index,
   /* [out] */ INetworkInterface** networkInterface)
{
    return NetworkInterface::GetByIndex(index, networkInterface);
}

ECode CNetworkInterfaceHelper::GetNetworkInterfaces(
   /* [out] */ IEnumeration** networkInterfaceList)
{
    return NetworkInterface::GetNetworkInterfaces(networkInterfaceList);
}

} // namespace Net
} // namespace Elastos
