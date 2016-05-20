
#ifndef __ELASTOS_SECURITY_SPEC_CRSAKEYGENPARAMETERSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CRSAKEYGENPARAMETERSPEC_H__

#include "_Elastos_Security_Spec_CRSAKeyGenParameterSpec.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CRSAKeyGenParameterSpec)
    , public Object
    , public IRSAKeyGenParameterSpec
    , public IAlgorithmParameterSpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    static CARAPI_(const AutoPtr<IBigInteger>) InitStatic(
        /* [in] */ Int64 bi);

    CARAPI GetKeysize(
        /* [out] */ Int32 *keySize);

    CARAPI GetPublicExponent(
        /* [out] */ IBigInteger **pubExponent);

    CARAPI constructor(
        /* [in] */ Int32 keysize,
        /* [in] */ IBigInteger *publicExponent);

    /**
     * The value of the public exponent {@code F0} = 3.
     */
    static const AutoPtr<IBigInteger> F0;

    /**
     * The value of the public exponent {@code F4} = 65537.
     */
    static const AutoPtr<IBigInteger> F4;

private:
    // Key size
    Int32 mKeysize;
    // Public Exponent
    AutoPtr<IBigInteger> mPublicExponent;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CRSAKEYGENPARAMETERSPEC_H__
