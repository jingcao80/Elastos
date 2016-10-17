
#include "CSignatureHelper.h"
#include "Signature.h"

namespace Elastos {
namespace Security {

CAR_SINGLETON_IMPL(CSignatureHelper);
CAR_INTERFACE_IMPL(CSignatureHelper, Singleton, ISignatureHelper)

ECode CSignatureHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ISignature **instance)
{
    return Signature::GetInstance(algorithm, instance);
}

ECode CSignatureHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ISignature **instance)
{
    return Signature::GetInstance(algorithm, provider, instance);
}

ECode CSignatureHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider *provider,
    /* [out] */ ISignature **instance)
{
    return Signature::GetInstance(algorithm, provider, instance);
}

} // namespace Security
} // namespace Elastos