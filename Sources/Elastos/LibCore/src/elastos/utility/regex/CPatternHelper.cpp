
#include "CPatternHelper.h"
#include "Pattern.h"

namespace Elastos {
namespace Utility {
namespace Regex {

CAR_INTERFACE_IMPL(CPatternHelper, Singleton, IPatternHelper)

CAR_SINGLETON_IMPL(CPatternHelper)

ECode CPatternHelper::Compile(
    /* [in] */ const String& regularExpression,
    /* [in] */ Int32 flags,
    /* [out] */ IPattern** obj)
{
    VALIDATE_NOT_NULL(obj);
    return Pattern::Compile(regularExpression, flags, obj);
}

ECode CPatternHelper::Compile(
    /* [in] */ const String& pattern,
    /* [in] */ IPattern** obj)
{
    return Pattern::Compile(pattern, obj);
}

ECode CPatternHelper::Matches(
    /* [in] */ const String& regularExpression,
    /* [in] */ const String& input,
    /* [out] */ Boolean* isMatch)
{

    VALIDATE_NOT_NULL(isMatch);
    *isMatch = Pattern::Matches(regularExpression, input);
    return NOERROR;
}

ECode CPatternHelper::Matches(
    /* [in] */ const String& regularExpression,
    /* [in] */ ICharSequence* input,
    /* [out] */ Boolean* isMatch)
{

    VALIDATE_NOT_NULL(isMatch);
    *isMatch = Pattern::Matches(regularExpression, input);
    return NOERROR;
}

ECode CPatternHelper::Quote(
    /* [in] */ const String& string,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Pattern::Quote(string);
    return NOERROR;
}

} // namespace Regex
} // namespace Utility
} // namespace Elastos
