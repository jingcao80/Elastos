
#ifndef __ELASTOS_SECURITY_SPEC_CECFIELDF2M_H__
#define __ELASTOS_SECURITY_SPEC_CECFIELDF2M_H__

#include "_Elastos_Security_Spec_CECFieldF2m.h"
#include "core/Object.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CECFieldF2m)
    , public Object
    , public IECFieldF2m
    , public IECField
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetFieldSize(
        /* [out] */ Int32 *size);

    CARAPI GetM(
        /* [out] */ Int32* m);

    CARAPI Equals(
        /* [in] */ IInterface *obj,
        /* [out] */ Boolean *result);

    CARAPI GetMidTermsOfReductionPolynomial(
        /* [out, callee] */ ArrayOf<Int32> **terms);

    CARAPI GetReductionPolynomial(
        /* [out] */ IBigInteger **rp);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI constructor(
        /* [in] */ Int32 m);

    CARAPI constructor(
        /* [in] */ Int32 m,
        /* [in] */ IBigInteger *rp);

    CARAPI constructor(
        /* [in] */ Int32 m,
        /* [in] */ ArrayOf<Int32> *ks);

private:
    // Mid terms array length for trinomial basis
    static const Int32 TPB_MID_LEN = 1;
    // Mid terms array length for pentanomial basis
    static const Int32 PPB_MID_LEN = 3;
    // All terms number for trinomial basis
    static const Int32 TPB_LEN = 3;
    // All terms number for pentanomial basis
    static const Int32 PPB_LEN = 5;
    // m value
    Int32 mM;
    // Reduction polynomial
    AutoPtr<IBigInteger> mRp;
    // Mid term(s) of reduction polynomial
    AutoPtr<ArrayOf<Int32> > mKs;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CECFIELDF2M_H__
