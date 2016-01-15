
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CREQUESTTARGETHOST_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CREQUESTTARGETHOST_H_

#include "_Org_Apache_Http_Protocol_CRequestTargetHost.h"
#include "org/apache/http/protocol/RequestTargetHost.h"

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
CarClass(CRequestTargetHost) , public RequestTargetHost
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CREQUESTTARGETHOST_H_
