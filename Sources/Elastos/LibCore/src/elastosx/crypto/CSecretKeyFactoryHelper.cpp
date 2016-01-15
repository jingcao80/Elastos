#include "CSecretKeyFactoryHelper.h"
#include "CSecretKeyFactory.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CSecretKeyFactoryHelper, Singleton, ISecretKeyFactoryHelper)

CAR_SINGLETON_IMPL(CSecretKeyFactoryHelper)

ECode CSecretKeyFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ISecretKeyFactory ** SecretKeyFactory)
{
    VALIDATE_NOT_NULL(SecretKeyFactory);
    return CSecretKeyFactory::GetInstance(algorithm, SecretKeyFactory);
}

ECode CSecretKeyFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ISecretKeyFactory ** SecretKeyFactory)
{
    VALIDATE_NOT_NULL(SecretKeyFactory);
    return CSecretKeyFactory::GetInstance(algorithm, provider, SecretKeyFactory);
}

ECode CSecretKeyFactoryHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ ISecretKeyFactory ** SecretKeyFactory)
{
    VALIDATE_NOT_NULL(SecretKeyFactory);
    return CSecretKeyFactory::GetInstance(algorithm, provider, SecretKeyFactory);
}

} // Crypto
} // Elastosx