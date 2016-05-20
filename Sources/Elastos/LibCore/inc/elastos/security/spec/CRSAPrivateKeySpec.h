
#ifndef __ELASTOS_SECURITY_SPEC_CRSAPRIVATEKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CRSAPRIVATEKEYSPEC_H__

#include "_Elastos_Security_Spec_CRSAPrivateKeySpec.h"
#include "RSAPrivateKeySpec.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CRSAPrivateKeySpec), public RSAPrivateKeySpec
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */IBigInteger *modulus,
        /* [in] */ IBigInteger *privateExponent);
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CRSAPRIVATEKEYSPEC_H__
