
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CREQUESTCONNCONTROL_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CREQUESTCONNCONTROL_H_

#include "_Org_Apache_Http_Protocol_CRequestConnControl.h"
#include "org/apache/http/protocol/RequestConnControl.h"

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
CarClass(CRequestConnControl) , public RequestConnControl
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CREQUESTCONNCONTROL_H_
