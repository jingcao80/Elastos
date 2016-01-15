
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CRESPONSECONNCONTROL_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CRESPONSECONNCONTROL_H_

#include "_Org_Apache_Http_Protocol_CResponseConnControl.h"
#include "org/apache/http/protocol/ResponseConnControl.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * A response interceptor that suggests connection keep-alive to the client.
 * For use on the server side.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 618017 $
 *
 * @since 4.0
 */
CarClass(CResponseConnControl) , public ResponseConnControl
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CRESPONSECONNCONTROL_H_
