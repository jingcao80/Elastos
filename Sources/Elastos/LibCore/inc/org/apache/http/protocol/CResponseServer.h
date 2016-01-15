
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CRESPONSESERVER_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CRESPONSESERVER_H_

#include "_Org_Apache_Http_Protocol_CResponseServer.h"
#include "org/apache/http/protocol/ResponseServer.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * A response interceptor that adds a Server header.
 * For use on the server side.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 576073 $
 *
 * @since 4.0
 */
CarClass(CResponseServer) , public ResponseServer
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CRESPONSESERVER_H_
