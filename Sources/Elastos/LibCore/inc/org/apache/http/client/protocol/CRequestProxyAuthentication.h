
#ifndef __ORG_APACHE_HTTP_CLIENT_PROTOCOL_CREQUESTPROXYAUTHENTICATION_H_
#define __ORG_APACHE_HTTP_CLIENT_PROTOCOL_CREQUESTPROXYAUTHENTICATION_H_

#include "_Org_Apache_Http_Client_Protocol_CRequestProxyAuthentication.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpRequest;
using Org::Apache::Http::IHttpRequestInterceptor;
using Org::Apache::Http::Protocol::IHttpContext;

namespace Org {
namespace Apache {
namespace Http {
namespace Client {
namespace Protocol {

/**
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 673450 $
 *
 * @since 4.0
 */
CarClass(CRequestProxyAuthentication)
    , public Object
    , public IHttpRequestInterceptor
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI Process(
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpContext* contexT);

    CARAPI constructor();
};

} // namespace Protocol
} // namespace Client
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_CLIENT_PROTOCOL_CREQUESTPROXYAUTHENTICATION_H_
