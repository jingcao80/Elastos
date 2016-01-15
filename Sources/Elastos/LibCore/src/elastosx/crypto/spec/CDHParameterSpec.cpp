#include "CDHParameterSpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CDHParameterSpec)
CAR_INTERFACE_IMPL_2(CDHParameterSpec, Object, IDHParameterSpec, IAlgorithmParameterSpec)

CDHParameterSpec::CDHParameterSpec()
    : mL(0)
{
}

ECode CDHParameterSpec::constructor(
    /* [in] */ IBigInteger * p,
    /* [in] */ IBigInteger * g)
{
    mP = p;
    mG = g;
    mL = 0;
    return NOERROR;
}

ECode CDHParameterSpec::constructor(
    /* [in] */ IBigInteger * p,
    /* [in] */ IBigInteger * g,
    /* [in] */ Int32 l)
{
    mP = p;
    mG = g;
    mL = l;
    return NOERROR;
}

ECode CDHParameterSpec::GetP(
    /* [out] */ IBigInteger ** bi)
{
    VALIDATE_NOT_NULL(bi)
    *bi = mP;
    REFCOUNT_ADD(*bi)
    return NOERROR;
}

ECode CDHParameterSpec::GetG(
    /* [out] */ IBigInteger ** bi)
{
    VALIDATE_NOT_NULL(bi)
    *bi = mG;
    REFCOUNT_ADD(*bi)
    return NOERROR;
}

ECode CDHParameterSpec::GetL(
    /* [out] */ Int32 * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mL;
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx