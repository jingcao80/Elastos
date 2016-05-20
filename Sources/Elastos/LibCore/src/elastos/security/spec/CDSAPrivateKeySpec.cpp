
#include "CDSAPrivateKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CDSAPrivateKeySpec)
CAR_INTERFACE_IMPL_2(CDSAPrivateKeySpec, Object, IDSAPrivateKeySpec, IKeySpec)
ECode CDSAPrivateKeySpec::GetG(
    /* [out] */ IBigInteger **g)
{
    VALIDATE_NOT_NULL(g)
    *g = mG;
    REFCOUNT_ADD(*g)
    return NOERROR;
}

ECode CDSAPrivateKeySpec::GetP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CDSAPrivateKeySpec::GetQ(
    /* [out] */ IBigInteger **q)
{
    VALIDATE_NOT_NULL(q)
    *q = mQ;
    REFCOUNT_ADD(*q)
    return NOERROR;
}

ECode CDSAPrivateKeySpec::GetX(
    /* [out] */ IBigInteger **x)
{
    VALIDATE_NOT_NULL(x)
    *x = mX;
    REFCOUNT_ADD(*x)
    return NOERROR;
}

ECode CDSAPrivateKeySpec::constructor(
    /* [in] */ IBigInteger *x,
    /* [in] */ IBigInteger *p,
    /* [in] */ IBigInteger *q,
    /* [in] */ IBigInteger *g)
{
    mX = x;
    mP = p;
    mQ = q;
    mG = g;
    return NOERROR;
}

}
}
}

