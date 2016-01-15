
#ifndef __ORG_APACHE_HTTP_PARAMS_CHTTPPROTOCOLPARAMS_H_
#define __ORG_APACHE_HTTP_PARAMS_CHTTPPROTOCOLPARAMS_H_

#include "_Org_Apache_Http_Params_CHttpProtocolParams.h"
#include "elastos/core/Singleton.h"

using Org::Apache::Http::IProtocolVersion;

namespace Org {
namespace Apache {
namespace Http {
namespace Params {

/**
 * This class implements an adaptor around the {@link HttpParams} interface
 * to simplify manipulation of the HTTP protocol specific parameters.
 * <br/>
 * Note that the <i>implements</i> relation to {@link CoreProtocolPNames}
 * is for compatibility with existing application code only. References to
 * the parameter names should use the interface, not this class.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 576089 $
 *
 * @since 4.0
 *
 * @see CoreProtocolPNames
 */
CarClass(CHttpProtocolParams)
    , public Singleton
    , public IHttpProtocolParams
    , public ICoreProtocolPNames
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Returns the charset to be used for writing HTTP headers.
     * @return The charset
     */
    CARAPI GetHttpElementCharset(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* charset);

    /**
     * Sets the charset to be used for writing HTTP headers.
     * @param charset The charset
     */
    CARAPI SetHttpElementCharset(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& charset);

    /**
     * Returns the default charset to be used for writing content body,
     * when no charset explicitly specified.
     * @return The charset
     */
    CARAPI GetContentCharset(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* charset);

    /**
     * Sets the default charset to be used for writing content body,
     * when no charset explicitly specified.
     * @param charset The charset
     */
    CARAPI SetContentCharset(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& charset);

    /**
     * Returns {@link ProtocolVersion protocol version} to be used per default.
     *
     * @return {@link ProtocolVersion protocol version}
     */
    CARAPI GetVersion(
        /* [in] */ IHttpParams* params,
        /* [out] */ IProtocolVersion** ver);

    /**
     * Assigns the {@link ProtocolVersion protocol version} to be used by the
     * HTTP methods that this collection of parameters applies to.
     *
     * @param version the {@link ProtocolVersion protocol version}
     */
    CARAPI SetVersion(
        /* [in] */ IHttpParams* params,
        /* [in] */ IProtocolVersion* version);

    CARAPI GetUserAgent(
        /* [in] */ IHttpParams* params,
        /* [out] */ String* agent);

    CARAPI SetUserAgent(
        /* [in] */ IHttpParams* params,
        /* [in] */ const String& useragent);

    CARAPI UseExpectContinue(
        /* [in] */ IHttpParams* params,
        /* [out] */ Boolean* result);

    CARAPI SetUseExpectContinue(
        /* [in] */ IHttpParams* params,
        /* [in] */ Boolean b);

};

} // namespace Params
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PARAMS_CHTTPPROTOCOLPARAMS_H_
