
#ifndef __ORG_APACHE_HTTP_PARAMS_HTTPPROTOCOLPARAMS_H_
#define __ORG_APACHE_HTTP_PARAMS_HTTPPROTOCOLPARAMS_H_

#include "Elastos.CoreLibrary.h"
#include <elastos/coredef.h>

using Org::Apache::Http::IProtocolVersion;

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

class HttpProtocolParams
{
public:
    /**
     * Returns the charset to be used for writing HTTP headers.
     * @return The charset
     */
    static CARAPI GetHttpElementCharset(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* charset);

    /**
     * Sets the charset to be used for writing HTTP headers.
     * @param charset The charset
     */
    static CARAPI SetHttpElementCharset(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& charset);

    /**
     * Returns the default charset to be used for writing content body,
     * when no charset explicitly specified.
     * @return The charset
     */
    static CARAPI GetContentCharset(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* charset);

    /**
     * Sets the default charset to be used for writing content body,
     * when no charset explicitly specified.
     * @param charset The charset
     */
    static CARAPI SetContentCharset(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& charset);

    /**
     * Returns {@link ProtocolVersion protocol version} to be used per default.
     *
     * @return {@link ProtocolVersion protocol version}
     */
    static CARAPI GetVersion(
        /* [in] */ IHttpParams* params,
        /* [out] */ IProtocolVersion** ver);

    /**
     * Assigns the {@link ProtocolVersion protocol version} to be used by the
     * HTTP methods that this collection of parameters applies to.
     *
     * @param version the {@link ProtocolVersion protocol version}
     */
    static CARAPI SetVersion(
        /* [in] */ IHttpParams* params,
        /* [in] */ IProtocolVersion* version);

    static CARAPI GetUserAgent(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* agent);

    static CARAPI SetUserAgent(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& useragent);

    static CARAPI UseExpectContinue(
        /* [in] */ IHttpParams* params,
        /* [out] */ Boolean* result);

    static CARAPI SetUseExpectContinue(
        /* [in] */ IHttpParams* params,
        /* [in] */ Boolean b);

private:
    HttpProtocolParams() {}
};

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PARAMS_HTTPPROTOCOLPARAMS_H_
