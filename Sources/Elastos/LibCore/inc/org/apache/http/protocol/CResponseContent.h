
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CRESPONSECONTENT_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CRESPONSECONTENT_H_

#include "_Org_Apache_Http_Protocol_CResponseContent.h"
#include "org/apache/http/protocol/ResponseContent.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * A response interceptor that sets up entity-related headers.
 * For use on the server side.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 573864 $
 *
 * @since 4.0
 */
CarClass(CResponseContent) , public ResponseContent
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CRESPONSECONTENT_H_
