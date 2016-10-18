
#include "CAlgorithmParametersHelper.h"
#include "AlgorithmParameters.h"

namespace Elastos {
namespace Security {

CAR_SINGLETON_IMPL(CAlgorithmParametersHelper);
CAR_INTERFACE_IMPL(CAlgorithmParametersHelper, Singleton, IAlgorithmParametersHelper)

ECode CAlgorithmParametersHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IAlgorithmParameters **result)
{
    return AlgorithmParameters::GetInstance(algorithm, result);
}

ECode CAlgorithmParametersHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IAlgorithmParameters **result)
{
    return AlgorithmParameters::GetInstance(algorithm, provider, result);
}

ECode CAlgorithmParametersHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider *provider,
    /* [out] */ IAlgorithmParameters **result)
{
    return AlgorithmParameters::GetInstance(algorithm, provider, result);
}

} // namespace Security
} // namespace Elastos