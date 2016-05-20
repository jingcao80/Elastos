
#ifndef __ELASTOS_SECURITY_SPEC_CRSAPRIVATECRTKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CRSAPRIVATECRTKEYSPEC_H__

#include "_Elastos_Security_Spec_CRSAPrivateCrtKeySpec.h"
#include "RSAPrivateKeySpec.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CRSAPrivateCrtKeySpec)
    , public RSAPrivateKeySpec
    , public IRSAPrivateCrtKeySpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCrtCoefficient(
        /* [out] */ IBigInteger **coefficient);

    CARAPI GetPrimeExponentP(
        /* [out] */ IBigInteger **p);

    CARAPI GetPrimeExponentQ(
        /* [out] */ IBigInteger **q);

    CARAPI GetPrimeP(
        /* [out] */ IBigInteger **p);

    CARAPI GetPrimeQ(
        /* [out] */ IBigInteger **q);

    CARAPI GetPublicExponent(
        /* [out] */ IBigInteger **pubExponent);

    CARAPI constructor(
        /* [in] */ IBigInteger *modulus,
        /* [in] */ IBigInteger *publicExponent,
        /* [in] */ IBigInteger *privateExponent,
        /* [in] */ IBigInteger *primeP,
        /* [in] */ IBigInteger *primeQ,
        /* [in] */ IBigInteger *primeExponentP,
        /* [in] */ IBigInteger *primeExponentQ,
        /* [in] */ IBigInteger *crtCoefficient);

private:
    // Public Exponent
    AutoPtr<IBigInteger> mPublicExponent;
    // Prime P
    AutoPtr<IBigInteger> mPrimeP;
    // Prime Q
    AutoPtr<IBigInteger> mPrimeQ;
    // Prime Exponent P
    AutoPtr<IBigInteger> mPrimeExponentP;
    // Prime Exponent Q
    AutoPtr<IBigInteger> mPrimeExponentQ;
    // CRT Coefficient
    AutoPtr<IBigInteger> mCrtCoefficient;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CRSAPRIVATECRTKEYSPEC_H__
