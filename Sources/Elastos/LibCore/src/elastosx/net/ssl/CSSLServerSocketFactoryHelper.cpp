
#include "CSSLServerSocketFactoryHelper.h"
#include "SSLServerSocketFactory.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(CSSLServerSocketFactoryHelper, Singleton, ISSLServerSocketFactoryHelper)

CAR_SINGLETON_IMPL(CSSLServerSocketFactoryHelper)

ECode CSSLServerSocketFactoryHelper::GetDefault(
    /* [out] */ IServerSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return SSLServerSocketFactory::GetDefault(factory);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
