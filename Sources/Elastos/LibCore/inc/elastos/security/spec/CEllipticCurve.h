
#ifndef __ELASTOS_SECURITY_SPEC_CELLIPTICCURVE_H__
#define __ELASTOS_SECURITY_SPEC_CELLIPTICCURVE_H__

#include "_Elastos_Security_Spec_CEllipticCurve.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CEllipticCurve)
    , public Object
    , public IEllipticCurve
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI GetA(
        /* [out] */ IBigInteger **a);

    CARAPI GetB(
        /* [out] */ IBigInteger **b);

    CARAPI GetField(
        /* [out] */ IECField **field);

    CARAPI GetSeed(
        /* [out, callee] */ ArrayOf<Byte> **seed);

    CARAPI Equals(
        /* [in] */ IInterface *other,
        /* [out] */ Boolean *result);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI constructor(
        /* [in] */ IECField *field,
        /* [in] */ IBigInteger *a,
        /* [in] */ IBigInteger *b,
        /* [in] */ ArrayOf<Byte> *seed);

    CARAPI constructor(
        /* [in] */ IECField *field,
        /* [in] */ IBigInteger *a,
        /* [in] */ IBigInteger *b);

private:
    // Underlying finite field
    const AutoPtr<IECField> mField;

    // The first coefficient of the equation defining this elliptic curve
    const AutoPtr<IBigInteger> mA;

    // The second coefficient of the equation defining this elliptic curve
    const AutoPtr<IBigInteger> mB;

    // Bytes used during this elliptic curve generation,
    // if it was generated randomly
    const AutoPtr<ArrayOf<Byte> > mSeed;

    // Hash code
    volatile Int32 mHash;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CELLIPTICCURVE_H__
