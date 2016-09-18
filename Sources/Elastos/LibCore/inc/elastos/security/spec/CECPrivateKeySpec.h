
#ifndef __ELASTOS_SECURITY_SPEC_CECPRIVATEKEYSPEC_H__
#define __ELASTOS_SECURITY_SPEC_CECPRIVATEKEYSPEC_H__

#include "_Elastos_Security_Spec_CECPrivateKeySpec.h"
#include "core/Object.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CECPrivateKeySpec)
    , public Object
    , public IECPrivateKeySpec
    , public IKeySpec
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetParams(
        /* [out] */ IECParameterSpec **params);

    CARAPI GetS(
        /* [out] */ IBigInteger **s);

    CARAPI constructor(
        /* [in] */ IBigInteger *s,
        /* [in] */ IECParameterSpec *params);

private:
    // Private value associated with this key
    AutoPtr<IBigInteger> mS;
    // Elliptic Curve domain parameters associated with this key
    AutoPtr<IECParameterSpec> mParams;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CECPRIVATEKEYSPEC_H__
