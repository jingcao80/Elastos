
#ifndef __ELASTOS_SECURITY_SPEC_CDSAPRIVATEKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CDSAPRIVATEKEYSPEC_H__

#include "_Elastos_Security_Spec_CDSAPrivateKeySpec.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CDSAPrivateKeySpec)
    , public Object
    , public IDSAPrivateKeySpec
    , public IKeySpec
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI GetG(
        /* [out] */ IBigInteger **g);

    CARAPI GetP(
        /* [out] */ IBigInteger **p);

    CARAPI GetQ(
        /* [out] */ IBigInteger **q);

    CARAPI GetX(
        /* [out] */ IBigInteger **x);

    CARAPI constructor(
        /* [in] */ IBigInteger *x,
        /* [in] */ IBigInteger *p,
        /* [in] */ IBigInteger *q,
        /* [in] */ IBigInteger *g);

private:
    // Private key
    AutoPtr<IBigInteger> mX;
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

#endif // __ELASTOS_SECURITY_SPEC_CDSAPRIVATEKEYSPEC_H__
