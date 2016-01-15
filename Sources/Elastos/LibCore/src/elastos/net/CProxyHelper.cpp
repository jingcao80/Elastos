
#include <CProxyHelper.h>
#include <CProxy.h>

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CProxyHelper, Singleton, IProxyHelper)

CAR_SINGLETON_IMPL(CProxyHelper)

ECode CProxyHelper::GetNO_PROXY(
    /* [out] */ IProxy** proxy)
{
    return CProxy::GetNO_PROXY(proxy);
}

} // namespace Net
} // namespace Elastos
