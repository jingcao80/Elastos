
#ifndef __ELASTOS_SECURITY_SPEC_CRSAOTHERPRIMEINFO_H__
#define __ELASTOS_SECURITY_SPEC_CRSAOTHERPRIMEINFO_H__

#include "_Elastos_Security_Spec_CRSAOtherPrimeInfo.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CRSAOtherPrimeInfo)
    , public Object
    , public IRSAOtherPrimeInfo
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetCrtCoefficient(
        /* [out] */ IBigInteger **crtCoefficient);

    CARAPI GetPrime(
        /* [out] */ IBigInteger **prime);

    CARAPI GetExponent(
        /* [out] */ IBigInteger **exponent);

    CARAPI constructor(
        /* [in] */ IBigInteger *prime,
        /* [in] */ IBigInteger *primeExponent,
        /* [in] */ IBigInteger *crtCoefficient);

private:
    // Prime
    AutoPtr<IBigInteger> mPrime;
    // Prime Exponent
    AutoPtr<IBigInteger> mPrimeExponent;
    // CRT Coefficient
    AutoPtr<IBigInteger> mCrtCoefficient;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CRSAOTHERPRIMEINFO_H__
