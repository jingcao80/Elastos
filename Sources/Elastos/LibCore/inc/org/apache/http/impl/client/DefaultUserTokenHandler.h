
#ifndef __ORG_APACHE_HTTP_IMPL_CLIENT_DefaultUserTokenHandler_H__
#define __ORG_APACHE_HTTP_IMPL_CLIENT_DefaultUserTokenHandler_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Security::IPrincipal;
using Org::Apache::Http::Auth::IAuthState;
using Org::Apache::Http::Client::IUserTokenHandler;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Client {

class DefaultUserTokenHandler
    : public Object
    , public IUserTokenHandler
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetUserToken(
        /* [in] */ IHttpContext* context,
        /* [out] */ IInterface** token);

private:
    static CARAPI GetAuthPrincipal(
        /* [in] */ IAuthState* authState,
        /* [out] */ IPrincipal** principal);
};

} // namespace Client
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CLIENT_DefaultUserTokenHandler_H__
