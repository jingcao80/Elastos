
#ifndef __ELASTOS_SECURITY_SPEC_CECPOINT_H__
#define __ELASTOS_SECURITY_SPEC_CECPOINT_H__

#include "_Elastos_Security_Spec_CECPoint.h"
#include "core/Object.h"

using Elastos::Math::IBigInteger;

namespace Elastos {
namespace Security {
namespace Spec {

CarClass(CECPoint)
    , public Object
    , public IECPoint
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    static CARAPI_(const AutoPtr<IECPoint>) InitStatic();

    CARAPI GetAffineX(
        /* [out] */ IBigInteger **affineX);

    CARAPI GetAffineY(
        /* [out] */ IBigInteger **affineY);

    CARAPI Equals(
        /* [in] */ IInterface *other,
        /* [out] */ Boolean *result);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBigInteger *affineX,
        /* [in] */ IBigInteger *affineY);

    /**
     * The point on an Elliptic Curve at infinity.
    */
    static const AutoPtr<IECPoint> POINT_INFINITY;

private:
    // affine X coordinate of this point
    AutoPtr<IBigInteger> mAffineX;
    // affine Y coordinate of this point
    AutoPtr<IBigInteger> mAffineY;
};

}
}
}

#endif // __ELASTOS_SECURITY_SPEC_CECPOINT_H__
