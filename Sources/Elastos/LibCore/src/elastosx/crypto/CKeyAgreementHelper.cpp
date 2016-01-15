#include "CKeyAgreementHelper.h"
#include "CKeyAgreement.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CKeyAgreementHelper, Singleton, IKeyAgreementHelper)

CAR_SINGLETON_IMPL(CKeyAgreementHelper)

ECode CKeyAgreementHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyAgreement ** keyAgreement)
{
	VALIDATE_NOT_NULL(keyAgreement);
	return CKeyAgreement::GetInstance(algorithm, keyAgreement);
}

ECode CKeyAgreementHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyAgreement ** keyAgreement)
{
	VALIDATE_NOT_NULL(keyAgreement);
	return CKeyAgreement::GetInstance(algorithm, provider, keyAgreement);
}

ECode CKeyAgreementHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IKeyAgreement ** keyAgreement)
{
	VALIDATE_NOT_NULL(keyAgreement);
	return CKeyAgreement::GetInstance(algorithm, provider, keyAgreement);
}

} // Crypto
} // Elastosx