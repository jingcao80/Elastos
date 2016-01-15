#include "CExemptionMechanismHelper.h"
#include "CExemptionMechanism.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CExemptionMechanismHelper, Singleton, IExemptionMechanismHelper)

CAR_SINGLETON_IMPL(CExemptionMechanismHelper)

ECode CExemptionMechanismHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IExemptionMechanism ** ExemptionMechanism)
{
	VALIDATE_NOT_NULL(ExemptionMechanism);
	return CExemptionMechanism::GetInstance(algorithm, ExemptionMechanism);
}

ECode CExemptionMechanismHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IExemptionMechanism ** ExemptionMechanism)
{
	VALIDATE_NOT_NULL(ExemptionMechanism);
	return CExemptionMechanism::GetInstance(algorithm, provider, ExemptionMechanism);
}

ECode CExemptionMechanismHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IExemptionMechanism ** ExemptionMechanism)
{
	VALIDATE_NOT_NULL(ExemptionMechanism);
	return CExemptionMechanism::GetInstance(algorithm, provider, ExemptionMechanism);
}

} // Crypto
} // Elastosx