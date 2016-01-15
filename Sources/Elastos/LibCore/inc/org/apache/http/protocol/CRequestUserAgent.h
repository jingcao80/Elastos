
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CREQUESTUSERAGENT_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CREQUESTUSERAGENT_H_

#include "_Org_Apache_Http_Protocol_CRequestUserAgent.h"
#include "org/apache/http/protocol/RequestUserAgent.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * A request interceptor that adds a User-Agent header.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 496070 $
 *
 * @since 4.0
 */
CarClass(CRequestUserAgent) , public RequestUserAgent
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CREQUESTUSERAGENT_H_
