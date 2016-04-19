
#include "org/apache/http/conn/ssl/CSSLSocketFactoryHelper.h"
#include "org/apache/http/conn/ssl/CSSLSocketFactory.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Conn {
namespace SSL {

CAR_INTERFACE_IMPL(CSSLSocketFactoryHelper, Singleton, ISSLSocketFactoryHelper)

CAR_SINGLETON_IMPL(CSSLSocketFactoryHelper)

ECode CSSLSocketFactoryHelper::GetAllowAllHostnameVerifier(
    /* [out] */ IX509HostnameVerifier** verifier)
{
    VALIDATE_NOT_NULL(verifier)
    *verifier = CSSLSocketFactory::ALLOW_ALL_HOSTNAME_VERIFIER;
    REFCOUNT_ADD(*verifier)
    return NOERROR;
}

ECode CSSLSocketFactoryHelper::GetBrowserCompatibleHostnameVerifier(
    /* [out] */ IX509HostnameVerifier** verifier)
{
    VALIDATE_NOT_NULL(verifier)
    *verifier = CSSLSocketFactory::BROWSER_COMPATIBLE_HOSTNAME_VERIFIER;
    REFCOUNT_ADD(*verifier)
    return NOERROR;
}

ECode CSSLSocketFactoryHelper::GetStrictHostnameVerifier(
    /* [out] */ IX509HostnameVerifier** verifier)
{
    VALIDATE_NOT_NULL(verifier)
    *verifier = CSSLSocketFactory::STRICT_HOSTNAME_VERIFIER;
    REFCOUNT_ADD(*verifier)
    return NOERROR;
}

ECode CSSLSocketFactoryHelper::GetSocketFactory(
    /* [out] */ ISSLSocketFactory** factory)
{
    VALIDATE_NOT_NULL(factory)
    AutoPtr<ISSLSocketFactory> temp = CSSLSocketFactory::GetSocketFactory();
    *factory = temp;
    REFCOUNT_ADD(*factory)
    return NOERROR;
}

} // namespace SSL
} // namespace Conn
} // namespace Http
} // namespace Apache
} // namespace Org