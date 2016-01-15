#include "CKeyGeneratorHelper.h"
#include "CKeyGenerator.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CKeyGeneratorHelper, Singleton, IKeyGeneratorHelper)

CAR_SINGLETON_IMPL(CKeyGeneratorHelper)

ECode CKeyGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyGenerator ** ka)
{
    VALIDATE_NOT_NULL(ka);
    return CKeyGenerator::GetInstance(algorithm, ka);
}

ECode CKeyGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyGenerator ** ka)
{
    VALIDATE_NOT_NULL(ka);
    return CKeyGenerator::GetInstance(algorithm, provider, ka);
}

ECode CKeyGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IKeyGenerator ** ka)
{
    VALIDATE_NOT_NULL(ka);
    return CKeyGenerator::GetInstance(algorithm, provider, ka);
}

} // Crypto
} // Elastosx