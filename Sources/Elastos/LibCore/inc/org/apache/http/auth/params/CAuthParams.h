
#ifndef __ORG_APACHE_HTTP_AUTH_PARAMS_CAUTHPARAMS_H_
#define __ORG_APACHE_HTTP_AUTH_PARAMS_CAUTHPARAMS_H_

#include "_Org_Apache_Http_Auth_Params_CAuthParams.h"
#include "elastos/core/Singleton.h"

using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {
namespace Params {

CarClass(CAuthParams)
    , public Object
    , public IAuthParams
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Obtains the charset for encoding
     * {@link org.apache.http.auth.Credentials}.
     * If not configured,
     * {@link HTTP#DEFAULT_PROTOCOL_CHARSET HTTP.DEFAULT_PROTOCOL_CHARSET}
     * is used instead.
     *
     * @return The charset
     *
     * @see AuthPNames#CREDENTIAL_CHARSET
     */
    CARAPI GetCredentialCharset(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* charset);

    /**
     * Sets the charset to be used when encoding
     * {@link org.apache.http.auth.Credentials}.
     *
     * @param charset The charset
     */
    CARAPI SetCredentialCharset(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& charset);

};

} // namespace Params
} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_AUTH_PARAMS_CAUTHPARAMS_H_
