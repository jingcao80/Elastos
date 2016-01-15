
#include "CRSAPublicKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CRSAPublicKeySpec);
CAR_INTERFACE_IMPL_2(CRSAPublicKeySpec, Object, IRSAPublicKeySpec, IKeySpec);
ECode CRSAPublicKeySpec::GetModulus(
    /* [out] */ IBigInteger **modulus)
{
    VALIDATE_NOT_NULL(modulus)
    *modulus = mModulus;
    REFCOUNT_ADD(*modulus)
    return NOERROR;
}

ECode CRSAPublicKeySpec::GetPublicExponent(
    /* [out] */ IBigInteger **exponent)
{
    VALIDATE_NOT_NULL(exponent)
    *exponent = mPublicExponent;
    REFCOUNT_ADD(*exponent)
    return NOERROR;
}

ECode CRSAPublicKeySpec::constructor(
    /* [in] */ IBigInteger *modulus,
    /* [in] */ IBigInteger *publicExponent)
{
    mModulus = modulus;
    mPublicExponent = publicExponent;
    return NOERROR;
}

}
}
}
