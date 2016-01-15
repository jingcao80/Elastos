
#include "CDSAPublicKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CDSAPublicKeySpec);
CAR_INTERFACE_IMPL_2(CDSAPublicKeySpec, Object, IDSAPublicKeySpec, IKeySpec);
ECode CDSAPublicKeySpec::GetG(
    /* [out] */ IBigInteger **g)
{
    VALIDATE_NOT_NULL(g)
    *g = mG;
    REFCOUNT_ADD(*g)
    return NOERROR;
}

ECode CDSAPublicKeySpec::GetP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CDSAPublicKeySpec::GetQ(
    /* [out] */ IBigInteger **q)
{
    VALIDATE_NOT_NULL(q)
    *q = mQ;
    REFCOUNT_ADD(*q)
    return NOERROR;
}

ECode CDSAPublicKeySpec::GetY(
    /* [out] */ IBigInteger **y)
{
    VALIDATE_NOT_NULL(y)
    *y = mY;
    REFCOUNT_ADD(*y)
    return NOERROR;
}

ECode CDSAPublicKeySpec::constructor(
    /* [in] */ IBigInteger *y,
    /* [in] */ IBigInteger *p,
    /* [in] */ IBigInteger *q,
    /* [in] */ IBigInteger *g)
{
    mY = y;
    mP = p;
    mQ = q;
    mG = g;
    return NOERROR;
}

}
}
}

