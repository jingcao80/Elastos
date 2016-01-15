#include "CDHGenParameterSpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CDHGenParameterSpec)
CAR_INTERFACE_IMPL_2(CDHGenParameterSpec, Object, IDHGenParameterSpec, IAlgorithmParameterSpec)

CDHGenParameterSpec::CDHGenParameterSpec()
    : mPrimeSize(0)
    , mExponentSize(0)
{
}

ECode CDHGenParameterSpec::constructor(
    /* [in] */ Int32 primeSize,
    /* [in] */ Int32 exponentSize)
{
    mPrimeSize = primeSize;
    mExponentSize = exponentSize;
    return NOERROR;
}

ECode CDHGenParameterSpec::GetPrimeSize(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPrimeSize;
    return NOERROR;
}

ECode CDHGenParameterSpec::GetExponentSize(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExponentSize;
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx