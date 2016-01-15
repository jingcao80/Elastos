
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CUrlQuerySanitizer.h"
#include "elastos/droid/net/CUrlQuerySanitizerHelper.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(CUrlQuerySanitizerHelper, Singleton, IUrlQuerySanitizerHelper)

CAR_SINGLETON_IMPL(CUrlQuerySanitizerHelper)

ECode CUrlQuerySanitizerHelper::GetAllIllegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAllIllegal(result);
}

ECode CUrlQuerySanitizerHelper::GetAllButNulLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAllButNulLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetAllButWhitespaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAllButWhitespaceLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetUrlLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetUrlLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetUrlAndSpaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetUrlAndSpaceLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetAmpLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAmpLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetAmpAndSpaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAmpAndSpaceLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetSpaceLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetSpaceLegal(result);
}

ECode CUrlQuerySanitizerHelper::GetAllButNulAndAngleBracketsLegal(
    /* [out] */ IUrlQuerySanitizerValueSanitizer** result)
{
    return CUrlQuerySanitizer::GetAllButNulAndAngleBracketsLegal(result);
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
