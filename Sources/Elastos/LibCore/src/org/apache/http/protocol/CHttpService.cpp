
#include "org/apache/http/protocol/CHttpService.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_OBJECT_IMPL(CHttpService)

ECode CHttpService::constructor(
    /* [in] */ IHttpProcessor* processor,
    /* [in] */ IConnectionReuseStrategy* connStrategy,
    /* [in] */ IHttpResponseFactory* responseFactory)
{
    return Init(processor, connStrategy, responseFactory);
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
