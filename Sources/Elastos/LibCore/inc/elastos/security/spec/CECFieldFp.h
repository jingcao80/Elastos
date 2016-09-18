
#ifndef __ELASTOS_SECURITY_SPEC_CECFIELDFP_H__
#define __ELASTOS_SECURITY_SPEC_CECFIELDFP_H__

#include "_Elastos_Security_Spec_CECFieldFp.h"
#include "core/Object.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CECFieldFp)
    , public Object
    , public IECFieldFp
    , public IECField
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetFieldSize(
        /* [out] */ Int32 *size);

    CARAPI GetP(
        /* [out] */ IBigInteger **p);

    CARAPI Equals(
        /* [in] */ IInterface *obj,
        /* [out] */ Boolean *result);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI constructor(
        /* [in] */ IBigInteger *p);

private:
    // Prime
    AutoPtr<IBigInteger> mP;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CECFIELDFP_H__
