
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CHTTPDATEGENERATOR_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CHTTPDATEGENERATOR_H_

#include "_Org_Apache_Http_Protocol_CHttpDateGenerator.h"
#include "org/apache/http/protocol/HttpDateGenerator.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * Generates a date in the format required by the HTTP protocol.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 548066 $
 *
 * @since 4.0
 */
CarClass(CHttpDateGenerator) , public HttpDateGenerator
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CHTTPDATEGENERATOR_H_
