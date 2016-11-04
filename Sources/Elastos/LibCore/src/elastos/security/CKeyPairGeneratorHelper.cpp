
#include "CKeyPairGeneratorHelper.h"
#include "KeyPairGenerator.h"

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(CKeyPairGeneratorHelper, Singleton, IKeyPairGeneratorHelper)

CAR_SINGLETON_IMPL(CKeyPairGeneratorHelper)

ECode CKeyPairGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IKeyPairGenerator** instance)
{
    return KeyPairGenerator::GetInstance(algorithm, instance);
}

ECode CKeyPairGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IKeyPairGenerator** instance)
{
    return KeyPairGenerator::GetInstance(algorithm, provider, instance);
}

ECode CKeyPairGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ IKeyPairGenerator** instance)
{
    return KeyPairGenerator::GetInstance(algorithm, provider, instance);
}

} // namespace Security
} // namespace Elastos