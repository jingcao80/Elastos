
#ifndef __ORG_APACHE_HTTP_PROTOCOL_REQUESTTARGETHOST_H_
#define __ORG_APACHE_HTTP_PROTOCOL_REQUESTTARGETHOST_H_

#include "Elastos.CoreLibrary.Apache.h"
#include "elastos/core/Object.h"

using Org::Apache::Http::IHttpRequestInterceptor;
using Org::Apache::Http::IHttpRequest;

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * A request interceptor that sets the Host header for HTTP/1.1 requests.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 573864 $
 *
 * @since 4.0
 */
class RequestTargetHost
    : public Object
    , public IHttpRequestInterceptor
{
public:
    CAR_INTERFACE_DECL()

    virtual ~RequestTargetHost() {}

    CARAPI Process(
        /* [in] */ IHttpRequest* request,
        /* [in] */ IHttpContext* context);
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_REQUESTTARGETHOST_H_
