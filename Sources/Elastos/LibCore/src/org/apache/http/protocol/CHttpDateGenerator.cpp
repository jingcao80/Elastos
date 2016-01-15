
#include "Elastos.CoreLibrary.Text.h"
#include "org/apache/http/protocol/CHttpDateGenerator.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_OBJECT_IMPL(CHttpDateGenerator)

ECode CHttpDateGenerator::constructor()
{
    return Init();
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
