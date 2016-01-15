
#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "org/apache/http/protocol/CHttpDateGeneratorHelper.h"
#include "org/apache/http/protocol/HttpDateGenerator.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Protocol {

CAR_INTERFACE_IMPL(CHttpDateGeneratorHelper, Object, IHttpDateGeneratorHelper)

CAR_SINGLETON_IMPL(CHttpDateGeneratorHelper)

ECode CHttpDateGeneratorHelper::GetGMT(
    /* [out] */ ITimeZone** timeZone)
{
    VALIDATE_NOT_NULL(timeZone)
    *timeZone = HttpDateGenerator::GMT;
    REFCOUNT_ADD(*timeZone)
    return NOERROR;
}

} // namespace Protocol
} // namespace Http
} // namespace Apache
} // namespace Org
