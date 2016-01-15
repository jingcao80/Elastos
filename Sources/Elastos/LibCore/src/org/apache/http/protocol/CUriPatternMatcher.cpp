
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/protocol/CUriPatternMatcher.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_OBJECT_IMPL(CUriPatternMatcher)

ECode CUriPatternMatcher::constructor()
{
    return Init();
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
