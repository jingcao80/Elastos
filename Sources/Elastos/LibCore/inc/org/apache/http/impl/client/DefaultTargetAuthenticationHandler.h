
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTTARGETAUTHENTICATIONHANDLER_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTTARGETAUTHENTICATIONHANDLER_H__

#include "org/apache/http/impl/client/AbstractAuthenticationHandler.h"

using Elastos::Utility::IMap;
using Org::Apache::Http::IHttpResponse;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

/**
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 */
class DefaultTargetAuthenticationHandler : public AbstractAuthenticationHandler
{
public:
    DefaultTargetAuthenticationHandler();

    CARAPI IsAuthenticationRequested(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context,
        /* [out] */ Boolean* result);

    CARAPI GetChallenges(
        /* [in] */ IHttpResponse* response,
        /* [in] */ IHttpContext* context,
        /* [out] */ IMap** callenges);
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_DEFAULTTARGETAUTHENTICATIONHANDLER_H__
