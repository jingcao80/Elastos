
#ifndef __ELASTOS_SECURITY_SPEC_CRSAPUBLICKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CRSAPUBLICKEYSPEC_H__

#include "_Elastos_Security_Spec_CRSAPublicKeySpec.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CRSAPublicKeySpec)
    , public Object
    , public IRSAPublicKeySpec
    , public IKeySpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetModulus(
        /* [out] */ IBigInteger **modulus);

    CARAPI GetPublicExponent(
        /* [out] */ IBigInteger **exponent);

    CARAPI constructor(
        /* [in] */ IBigInteger *modulus,
        /* [in] */ IBigInteger *publicExponent);

private:
    // Modulus
    AutoPtr<IBigInteger> mModulus;
    // Public Exponent
    AutoPtr<IBigInteger> mPublicExponent;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CRSAPUBLICKEYSPEC_H__
