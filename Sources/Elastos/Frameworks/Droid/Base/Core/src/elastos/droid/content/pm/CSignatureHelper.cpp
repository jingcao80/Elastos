
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/pm/CSignatureHelper.h"
#include "elastos/droid/content/pm/CSignature.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL(CSignatureHelper, Singleton, ISignatureHelper)

CAR_SINGLETON_IMPL(CSignatureHelper)

ECode CSignatureHelper::AreExactMatch(
    /* [in] */ ArrayOf<ISignature*>* a,
    /* [in] */ ArrayOf<ISignature*>* b,
    /* [out] */ Boolean* match)
{
    VALIDATE_NOT_NULL(match)
    *match = CSignature::AreExactMatch(a, b);
    return NOERROR;
}

ECode CSignatureHelper::AreEffectiveMatch(
    /* [in] */ ArrayOf<ISignature*>* a,
    /* [in] */ ArrayOf<ISignature*>* b,
    /* [out] */ Boolean* match)
{
    VALIDATE_NOT_NULL(match)
    *match = CSignature::AreEffectiveMatch(a, b);
    return NOERROR;
}

ECode CSignatureHelper::Bounce(
    /* [in] */ ICertificateFactory* cf,
    /* [in] */ ISignature* s,
    /* [out] */ ISignature** sig)
{
    VALIDATE_NOT_NULL(sig)
    return CSignature::Bounce(cf, s, sig);
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

