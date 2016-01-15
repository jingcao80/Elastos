
#ifndef __ORG_APACHE_HTTP_PROTOCOL_CHTTPSERVICE_H_
#define __ORG_APACHE_HTTP_PROTOCOL_CHTTPSERVICE_H_

#include "_Org_Apache_Http_Protocol_CHttpService.h"
#include "org/apache/http/protocol/HttpService.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

/**
 * Minimalistic server-side implementation of an HTTP processor.
 *
 * @author <a href="mailto:oleg at ural.ru">Oleg Kalnichevski</a>
 *
 * @version $Revision: 610763 $
 */
CarClass(CHttpService) , public HttpService
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IHttpProcessor* processor,
        /* [in] */ IConnectionReuseStrategy* connStrategy,
        /* [in] */ IHttpResponseFactory* responseFactory);
};

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_PROTOCOL_CHTTPSERVICE_H_
