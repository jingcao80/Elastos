
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CRESQUESTDATE_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CRESQUESTDATE_H_

#include "_Org_Apache_Http_Protocol_CRequestDate.h"
#include "org/apache/http/protocol/RequestDate.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * A Request interceptor that adds a Date header.
 * For use on the server side.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 555989 $
 *
 * @since 4.0
 */
CarClass(CRequestDate) , public RequestDate
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CRESQUESTDATE_H_
