
#include "RSAPrivateKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_INTERFACE_IMPL_2(RSAPrivateKeySpec, Object, IRSAPrivateKeySpec, IKeySpec);
ECode RSAPrivateKeySpec::GetModulus(
    /* [out] */ IBigInteger **modulus)
{
    VALIDATE_NOT_NULL(modulus)
    *modulus = mModulus;
    REFCOUNT_ADD(*modulus)
    return NOERROR;
}

ECode RSAPrivateKeySpec::GetPrivateExponent(
    /* [out] */ IBigInteger **exponent)
{
    VALIDATE_NOT_NULL(exponent)
    *exponent = mPrivateExponent;
    REFCOUNT_ADD(*exponent)
    return NOERROR;
}

ECode RSAPrivateKeySpec::Init(
    /* [in] */ IBigInteger *modulus,
    /* [in] */ IBigInteger *privateExponent)
{
    mModulus = modulus;
    mPrivateExponent = privateExponent;
    return NOERROR;
}

}
}
}
