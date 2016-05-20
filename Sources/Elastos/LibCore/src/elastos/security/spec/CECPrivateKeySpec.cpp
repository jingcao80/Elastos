
#include "CECPrivateKeySpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CECPrivateKeySpec)
CAR_INTERFACE_IMPL_2(CECPrivateKeySpec, Object, IECPrivateKeySpec, IKeySpec)
ECode CECPrivateKeySpec::GetParams(
    /* [out] */ IECParameterSpec **params)
{
    VALIDATE_NOT_NULL(params)
    *params = mParams;
    REFCOUNT_ADD(*params)
    return NOERROR;
}

ECode CECPrivateKeySpec::GetS(
    /* [out] */ IBigInteger **s)
{
    VALIDATE_NOT_NULL(s)
    *s = mS;
    REFCOUNT_ADD(*s)
    return NOERROR;
}

ECode CECPrivateKeySpec::constructor(
    /* [in] */ IBigInteger *s,
    /* [in] */ IECParameterSpec *params)
{
    mS = s;
    mParams = params;
    // throw NullPointerException if s or params is null
    if (mS == NULL || mParams == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

}
}
}

