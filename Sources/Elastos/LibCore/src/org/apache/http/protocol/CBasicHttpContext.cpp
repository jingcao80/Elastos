
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/protocol/CBasicHttpContext.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_OBJECT_IMPL(CBasicHttpContext)

ECode CBasicHttpContext::constructor()
{
    return Init();
}

ECode CBasicHttpContext::constructor(
    /* [in] */ IHttpContext* parentContext)
{
    return Init(parentContext);
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
