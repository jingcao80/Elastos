
#include "CMatcherHelper.h"
#include "CMatcher.h"

namespace Elastos {
namespace Utility {
namespace Regex {

CAR_INTERFACE_IMPL(CMatcherHelper, Singleton, IMatcherHelper)

CAR_SINGLETON_IMPL(CMatcherHelper)

ECode CMatcherHelper::QuoteReplacement(
    /* [in] */ const String& s,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = CMatcher::QuoteReplacement(s);
    return NOERROR;
}

} // namespace Regex
} // namespace Utility
} // namespace Elastos
