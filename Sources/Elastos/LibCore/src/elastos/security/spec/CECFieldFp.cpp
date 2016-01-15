
#include "CECFieldFp.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CECFieldFp);
CAR_INTERFACE_IMPL_2(CECFieldFp, Object, IECFieldFp, IECField);
ECode CECFieldFp::GetFieldSize(
    /* [out] */ Int32 *size)
{
    return mP->BitLength(size);
}

ECode CECFieldFp::GetP(
    /* [out] */ IBigInteger **p)
{
    VALIDATE_NOT_NULL(p)
    *p = mP;
    REFCOUNT_ADD(*p)
    return NOERROR;
}

ECode CECFieldFp::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    // object equals itself
    if (THIS_PROBE(IInterface) == obj) {
        *result = TRUE;
    }
    if (IECFieldFp::Probe(obj)) {
        return mP->Equals(((CECFieldFp*)(IECFieldFp*)obj)->mP.Get(), result);
    }
    *result = FALSE;
    return NOERROR;
}

ECode CECFieldFp::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    return mP->GetHashCode(hashCode);
}

ECode CECFieldFp::constructor(
    /* [in] */ IBigInteger *p)
{
    mP = p;

    if (mP == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 num;
    mP->GetSignum(&num);
    if (num != 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;

}

}
}
}

