
#include "CSSLSocketFactoryHelper.h"
#include "SSLSocketFactory.h"

namespace Elastosx {
namespace Net {
namespace Ssl {

CAR_INTERFACE_IMPL(CSSLSocketFactoryHelper, Singleton, ISSLSocketFactoryHelper)

CAR_SINGLETON_IMPL(CSSLSocketFactoryHelper)

ECode CSSLSocketFactoryHelper::GetDefault(
    /* [out] */ ISocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)

    return SSLSocketFactory::GetDefault(factory);
}

} // namespace Ssl
} // namespace Net
} // namespace Elastosx
