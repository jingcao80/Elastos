
#include "CInetSocketAddressHelper.h"
#include "CInetSocketAddress.h"

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CInetSocketAddressHelper, Singleton, IInetSocketAddressHelper)

CAR_SINGLETON_IMPL(CInetSocketAddressHelper)

ECode CInetSocketAddressHelper::CreateUnresolved(
    /* [in] */ const String& host,
    /* [in] */ Int32 port,
    /* [out] */ IInetSocketAddress** addr)
{
    return CInetSocketAddress::CreateUnresolved(host, port, addr);
}

} // namespace Net
} // namespace Elastos
