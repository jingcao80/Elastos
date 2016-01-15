
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CREQUESTCONTENT_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CREQUESTCONTENT_H_

#include "_Org_Apache_Http_Protocol_CRequestContent.h"
#include "org/apache/http/protocol/RequestContent.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * A request interceptor that decides about the transport encoding.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 573864 $
 *
 * @since 4.0
 */
CarClass(CRequestContent) , public RequestContent
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CREQUESTCONTENT_H_
