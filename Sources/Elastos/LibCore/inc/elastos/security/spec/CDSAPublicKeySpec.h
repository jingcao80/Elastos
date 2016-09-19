
#ifndef __ELASTOS_SECURITY_SPEC_CDSAPUBLICKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CDSAPUBLICKEYSPEC_H__

#include "_Elastos_Security_Spec_CDSAPublicKeySpec.h"
#include "core/Object.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CDSAPublicKeySpec)
    , public Object
    , public IDSAPublicKeySpec
    , public IKeySpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetG(
        /* [out] */ IBigInteger **g);

    CARAPI GetP(
        /* [out] */ IBigInteger **p);

    CARAPI GetQ(
        /* [out] */ IBigInteger **q);

    CARAPI GetY(
        /* [out] */ IBigInteger **y);

    CARAPI constructor(
        /* [in] */ IBigInteger *y,
        /* [in] */ IBigInteger *p,
        /* [in] */ IBigInteger *q,
        /* [in] */ IBigInteger *g);

private:
    // Private key
    AutoPtr<IBigInteger> mY;
    // Prime
    AutoPtr<IBigInteger> mP;
    // Sub-prime
    AutoPtr<IBigInteger> mQ;
    // Base
    AutoPtr<IBigInteger> mG;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CDSAPUBLICKEYSPEC_H__
