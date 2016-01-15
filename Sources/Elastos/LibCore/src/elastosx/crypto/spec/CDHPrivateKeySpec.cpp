#include "CDHPrivateKeySpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(CDHPrivateKeySpec)
CAR_INTERFACE_IMPL_2(CDHPrivateKeySpec, Object, IDHPrivateKeySpec, IKeySpec)

CDHPrivateKeySpec::CDHPrivateKeySpec()
{
}

ECode CDHPrivateKeySpec::constructor(
    /* [in] */ IBigInteger * x,
    /* [in] */ IBigInteger * p,
    /* [in] */ IBigInteger * g)
{
    mX = x;
    mP = p;
    mG = g;
    return NOERROR;
}

ECode CDHPrivateKeySpec::GetX(
    /* [out] */ IBigInteger ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mX;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDHPrivateKeySpec::GetP(
    /* [out] */ IBigInteger ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mP;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDHPrivateKeySpec::GetG(
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