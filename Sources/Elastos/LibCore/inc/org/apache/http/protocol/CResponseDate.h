
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CRESPONSEDATE_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CRESPONSEDATE_H_

#include "_Org_Apache_Http_Protocol_CResponseDate.h"
#include "org/apache/http/protocol/ResponseDate.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * A response interceptor that adds a Date header.
 * For use on the server side.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 555989 $
 *
 * @since 4.0
 */
CarClass(CResponseDate) , public ResponseDate
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CRESPONSEDATE_H_
