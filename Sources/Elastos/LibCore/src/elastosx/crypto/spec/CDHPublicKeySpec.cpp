#include "CDHPublicKeySpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CDHPublicKeySpec)
CAR_INTERFACE_IMPL_2(CDHPublicKeySpec, Object, IDHPublicKeySpec, IKeySpec)

CDHPublicKeySpec::CDHPublicKeySpec()
{
}

ECode CDHPublicKeySpec::constructor(
    /* [in] */ IBigInteger * y,
    /* [in] */ IBigInteger * p,
    /* [in] */ IBigInteger * g)
{
    mY = y;
    mP = p;
    mG = g;
    return NOERROR;
}

ECode CDHPublicKeySpec::GetY(
    /* [out] */ IBigInteger ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mY;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDHPublicKeySpec::GetP(
    /* [out] */ IBigInteger ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mP;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDHPublicKeySpec::GetG(
    /* [out] */ IBigInteger ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mG;
    REFCOUNT_ADD(*result)
    return NOERROR;
}


} // Spec
} // Crypto
} // Elastosx