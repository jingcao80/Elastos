
#include "CAlgorithmParameterGeneratorHelper.h"
#include "CAlgorithmParameterGenerator.h"

namespace Elastos {
namespace Security {

CAR_SINGLETON_IMPL(CAlgorithmParameterGeneratorHelper);
CAR_INTERFACE_IMPL(CAlgorithmParameterGeneratorHelper, Singleton, IAlgorithmParameterGeneratorHelper);
ECode CAlgorithmParameterGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException*/
{
    return CAlgorithmParameterGenerator::GetInstance(algorithm, generator);
}

ECode CAlgorithmParameterGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException, NoSuchProviderException*/
{
    return CAlgorithmParameterGenerator::GetInstance(algorithm, provider, generator);
}

ECode CAlgorithmParameterGeneratorHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ IAlgorithmParameterGenerator** generator) /*throws NoSuchAlgorithmException*/
{
    return CAlgorithmParameterGenerator::GetInstance(algorithm, provider, generator);
}

} // namespace Security
} // namespace Elastos
