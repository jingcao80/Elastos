
#ifndef __ORG_APACHE_HTTP_CLIENT_PARAMS_HTTPCLIENTPARAMS_H__
#define __ORG_APACHE_HTTP_CLIENT_PARAMS_HTTPCLIENTPARAMS_H__

#include "Elastos.CoreLibrary.Apache.h"

using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Params {

/**
 * An adaptor for accessing HTTP client parameters in {@link HttpParams}.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 659595 $
 *
 * @since 4.0
 */
class HttpClientParams
{
private:
    HttpClientParams();

public:
    static CARAPI IsRedirecting(
        /* [in] */ IHttpParams* params,
        /* [out] */ Boolean* isRedirecting);

    static CARAPI SetRedirecting(
        /* [in] */ IHttpParams* params,
        /* [in] */ Boolean value);

    static CARAPI IsAuthenticating(
        /* [in] */ IHttpParams* params,
        /* [out] */ Boolean* isAuthenticating);

    static CARAPI SetAuthenticating(
        /* [in] */ IHttpParams* params,
        /* [in] */ Boolean value);

    static CARAPI GetCookiePolicy(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* cookiePolicy);

    static CARAPI SetCookiePolicy(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& cookiePolicy);
};

} // namespace Params
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_PARAMS_HTTPCLIENTPARAMS_H__
