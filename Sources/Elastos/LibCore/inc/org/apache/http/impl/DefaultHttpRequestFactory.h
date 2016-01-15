
#ifndef __ORG_APACHE_HTTP_IMPL_DEFAULTHTTPREQUESTFACTORY_H__
#define __ORG_APACHE_HTTP_IMPL_DEFAULTHTTPREQUESTFACTORY_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpRequestFactory;
using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IRequestLine;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {

/**
 * Abstract client-side HTTP connection capable of transmitting and receiving data
 * using arbitrary {@link SessionInputBuffer} and {@link SessionOutputBuffer}
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 627457 $
 *
 * @since 4.0
 */
class DefaultHttpRequestFactory
    : public Object
    , public IHttpRequestFactory
{
public:
    DefaultHttpRequestFactory();

    CAR_INTERFACE_DECL()

    CARAPI NewHttpRequest(
        /* [in] */ IRequestLine* requestline,
        /* [out] */ IHttpRequest** request);

    CARAPI NewHttpRequest(
        /* [in] */ const String& method,
        /* [in] */ const String& uri,
        /* [out] */ IHttpRequest** request);

private:
    static CARAPI_(Boolean) IsOneOf(
        /* [in] */ ArrayOf<String>* methods,
        /* [in] */ const String& method);

private:
    static const AutoPtr< ArrayOf<String> > RFC2616_COMMON_METHODS;

    static const AutoPtr< ArrayOf<String> > RFC2616_ENTITY_ENC_METHODS;

    static const AutoPtr< ArrayOf<String> > RFC2616_SPECIAL_METHODS;
};

} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_DEFAULTHTTPREQUESTFACTORY_H__
