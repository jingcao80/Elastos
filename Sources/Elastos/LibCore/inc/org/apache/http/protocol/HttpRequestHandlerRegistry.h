
#ifndef __ORG_APACHE_HTTP_PROTOCOL_HTTPREQUESTHANDLERREGISTRY_H_
#define __ORG_APACHE_HTTP_PROTOCOL_HTTPREQUESTHANDLERREGISTRY_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Elastos::Utility::IMap;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * Maintains a map of HTTP request handlers keyed by a request URI pattern.
 * {@link HttpRequestHandler} instances can be looked up by request URI
 * using the {@link HttpRequestHandlerResolver} interface.<br/>
 * Patterns may have three formats:
 * <ul>
 *   <li><code>*</code></li>
 *   <li><code>*&lt;uri&gt;</code></li>
 *   <li><code>&lt;uri&gt;*</code></li>
 * </ul>
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 630662 $
 */
class HttpRequestHandlerRegistry
    : public Object
    , public IHttpRequestHandlerRegistry
    , public IHttpRequestHandlerResolver
{
public:
    CAR_INTERFACE_DECL()

    virtual ~HttpRequestHandlerRegistry() {}

    CARAPI Register(
        /* [in] */ const String& pattern,
        /* [in] */ IHttpRequestHandler* handler);

    CARAPI Unregister(
        /* [in] */ const String& pattern);

    CARAPI SetHandlers(
        /* [in] */ IMap* map);

    CARAPI Lookup(
        /* [in] */ const String& requestURI,
        /* [out] */ IHttpRequestHandler** handler);

protected:
    CARAPI Init();

    CARAPI MatchUriRequestPattern(
        /* [in] */ const String& pattern,
        /* [in] */ const String& requestUri,
        /* [out] */ Boolean* isMatch);

private:
    AutoPtr<IUriPatternMatcher> mMatcher;
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_HTTPREQUESTHANDLERREGISTRY_H_
