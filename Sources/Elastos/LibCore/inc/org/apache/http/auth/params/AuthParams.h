
#ifndef __ORG_APACHE_HTTP_AUTH_PARAMS_AUTHPARAMS_H__
#define __ORG_APACHE_HTTP_AUTH_PARAMS_AUTHPARAMS_H__

#include "Elastos.CoreLibrary.Apache.h"
#include <elastos/coredef.h>

using Org::Apache::Http::Params::IHttpParams;

namespace Org {
namespace Apache {
namespace Http {
namespace Auth {
namespace Params {

/**
 * This class implements an adaptor around the {@link HttpParams} interface
 * to simplify manipulation of the HTTP authentication specific parameters.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 618365 $
 *
 * @since 4.0
 *
 * @see AuthPNames
 */
class AuthParams
{
private:
    AuthParams();

public:
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
    static CARAPI GetCredentialCharset(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* charset);

    /**
     * Sets the charset to be used when encoding
     * {@link org.apache.http.auth.Credentials}.
     *
     * @param charset The charset
     */
    static CARAPI SetCredentialCharset(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& charset);
};

} // namespace Params
} // namespace Auth
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_AUTH_PARAMS_AUTHPARAMS_H__
