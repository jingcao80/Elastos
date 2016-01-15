
#ifndef __ELASTOS_SECURITY_SPEC_RSAPRIVATEKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_RSAPRIVATEKEYSPEC_H__

#include "Elastos.CoreLibrary_server.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

class RSAPrivateKeySpec
    : public Object
    , public IRSAPrivateKeySpec
    , public IKeySpec
{
public:
    CAR_INTERFACE_DECL();

    CARAPI GetModulus(
        /* [out] */ IBigInteger **modulus);

    CARAPI GetPrivateExponent(
        /* [out] */ IBigInteger **exponent);

    CARAPI Init(
        /* [in] */IBigInteger *modulus,
        /* [in] */ IBigInteger *privateExponent);

private:
    // Modulus
    AutoPtr<IBigInteger> mModulus;
    // Private Exponent
    AutoPtr<IBigInteger> mPrivateExponent;
};

}
}
}

#endif // __CRSAPRIVATEKEYSPEC_H__
