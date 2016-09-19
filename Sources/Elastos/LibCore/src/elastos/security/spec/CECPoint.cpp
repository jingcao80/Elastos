
#include "CECPoint.h"

namespace Elastos {
namespace Security {
namespace Spec {

const AutoPtr<IECPoint> CECPoint::POINT_INFINITY = CECPoint::InitStatic();

CAR_OBJECT_IMPL(CECPoint)
CAR_INTERFACE_IMPL(CECPoint, Object, IECPoint)
const AutoPtr<IECPoint> CECPoint::InitStatic()
{
    AutoPtr<CECPoint> ecp;
    CECPoint::NewByFriend((CECPoint**)&ecp);
    return ecp;
}

ECode CECPoint::GetAffineX(
    /* [out] */ IBigInteger **affineX)
{
    VALIDATE_NOT_NULL(affineX)
    *affineX = mAffineX;
    REFCOUNT_ADD(*affineX)
    return NOERROR;
}

ECode CECPoint::GetAffineY(
    /* [out] */ IBigInteger **affineY)
{
    VALIDATE_NOT_NULL(affineY)
    *affineY = mAffineY;
    REFCOUNT_ADD(*affineY)
    return NOERROR;
}

ECode CECPoint::Equals(
    /* [in] */ IInterface *other,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    if (TO_IINTERFACE(this) == other) {
        *result = TRUE;
        return NOERROR;
    }
    if (IECPoint::Probe(other)) {
        if (mAffineX != NULL) {
            AutoPtr<IECPoint> otherPoint = IECPoint::Probe(other);
            // no need to check for null in this case
            Boolean equalx, equaly;
            mAffineX->Equals(((CECPoint*)otherPoint.Get())->mAffineX.Get(), &equalx);
            mAffineY->Equals(((CECPoint*)otherPoint.Get())->mAffineY.Get(), &equaly);
            *result = equalx && equaly;
            return NOERROR;
        }
        else {
            *result = other == (IInterface*)POINT_INFINITY.Get();
            return NOERROR;
        }
    }
    *result = FALSE;
    return NOERROR;
}

ECode CECPoint::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    if (mAffineX != NULL) {
        Int32 tmp;
        IObject::Probe(mAffineX)->GetHashCode(hashCode);
        *hashCode *= 31;
        IObject::Probe(mAffineY)->GetHashCode(&tmp);
        *hashCode += tmp;
        return NOERROR;
    }
    *hashCode = 11;
    return NOERROR;
}

ECode CECPoint::constructor()
{
    return NOERROR;
}

ECode CECPoint::constructor(
    /* [in] */ IBigInteger *affineX,
    /* [in] */ IBigInteger *affineY)
{
    mAffineX = affineX;
    if (mAffineX == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    mAffineY = affineY;
    if (mAffineY == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

}
}
}

