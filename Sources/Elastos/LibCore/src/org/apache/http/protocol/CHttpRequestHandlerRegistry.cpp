
#include "org/apache/http/protocol/CHttpRequestHandlerRegistry.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_OBJECT_IMPL(CHttpRequestHandlerRegistry)

ECode CHttpRequestHandlerRegistry::constructor()
{
    return Init();
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
