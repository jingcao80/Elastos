
#include "elastos/net/CInet6AddressHelper.h"
#include "elastos/net/Inet6Address.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CInet6AddressHelper, Singleton, IInet6AddressHelper)

CAR_SINGLETON_IMPL(CInet6AddressHelper)

ECode CInet6AddressHelper::GetByAddress(
    /* [in] */ const String& host,
    /* [in] */ ArrayOf<Byte>* addr,
    /* [in] */ Int32 scope_id,
    /* [out] */ IInet6Address** address)
{
    return Inet6Address::GetByAddress(host, addr, scope_id, address);
}

ECode CInet6AddressHelper::GetByAddress(
    /* [in] */ const String& host,
    /* [in] */ ArrayOf<Byte>* addr,
    /* [in] */ INetworkInterface* nif,
    /* [out] */ IInet6Address** address)
{
    return Inet6Address::GetByAddress(host, addr, nif, address);
}

ECode CInet6AddressHelper::GetANY(
    /* [out] */ IInetAddress** result)
{
    return Inet6Address::GetANY(result);
}

ECode CInet6AddressHelper::GetLOOPBACK(
    /* [out] */ IInetAddress** result)
{
    return Inet6Address::GetLOOPBACK(result);
}

} // namespace Net
} // namespace Elastos
