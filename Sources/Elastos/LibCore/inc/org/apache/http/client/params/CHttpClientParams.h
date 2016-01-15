
#ifndef __ORG_APACHE_HTTP_CLIENT_PARAMS_CHTTPCLIENTPARAMS_H_
#define __ORG_APACHE_HTTP_CLIENT_PARAMS_CHTTPCLIENTPARAMS_H_

#include "_Org_Apache_Http_Client_Params_CHttpClientParams.h"
#include "elastos/core/Singleton.h"

using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Params {

CarClass(CHttpClientParams)
    , public Object
    , public IHttpClientParams
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI IsRedirecting(
        /* [in] */ IHttpParams* params,
        /* [out] */ Boolean* isRedirecting);

    CARAPI SetRedirecting(
        /* [in] */ IHttpParams* params,
        /* [in] */ Boolean value);

    CARAPI IsAuthenticating(
        /* [in] */ IHttpParams* params,
        /* [out] */ Boolean* isAuthenticating);

    CARAPI SetAuthenticating(
        /* [in] */ IHttpParams* params,
        /* [in] */ Boolean value);

    CARAPI GetCookiePolicy(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* cookiePolicy);

    CARAPI SetCookiePolicy(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& cookiePolicy);

};

} // namespace Params
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_PARAMS_CHTTPCLIENTPARAMS_H_
