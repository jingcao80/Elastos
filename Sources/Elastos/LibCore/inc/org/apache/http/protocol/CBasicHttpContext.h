
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CBASICHTTPCONTEXT_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CBASICHTTPCONTEXT_H_

#include "_Org_Apache_Http_Protocol_CBasicHttpContext.h"
#include "org/apache/http/protocol/BasicHttpContext.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * Default implementation of the {@link HttpContext HttpContext}.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 654882 $
 *
 * @since 4.0
 */
CarClass(CBasicHttpContext) , public BasicHttpContext
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IHttpContext* parentContext);
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CBASICHTTPCONTEXT_H_
