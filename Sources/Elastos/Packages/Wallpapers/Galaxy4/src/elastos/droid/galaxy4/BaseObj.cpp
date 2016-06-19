
#include "BaseObj.h"
#include "RenderScript.h"

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

ECode BaseObj::constructor(
    /* [in] */ Int64 id,
    /* [in] */ RenderScript* rs)
{
    FAIL_RETURN(rs->Validate());
    mRS = rs;
    mID = id;
    mDestroyed = FALSE;
    return NOERROR;
}

ECode BaseObj::SetID(
    /* [in] */ Int64 id)
{
    if (mID != 0) {
        // throw new RSRuntimeException("Internal Error, reset of object ID.");
        return E_RUNTIME_EXCEPTION;
    }
    mID = id;
    return NOERROR;
}

ECode BaseObj::GetID(
    /* [in] */ RenderScript* rs,
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = -1;
    FAIL_RETURN(mRS->Validate());
    if (mDestroyed) {
        // throw new RSInvalidStateException("using a destroyed object.");
        return E_RUNTIME_EXCEPTION;
    }
    if (mID == 0) {
        // throw new RSRuntimeException("Internal error: Object id 0.");
        return E_RUNTIME_EXCEPTION;
    }
    if ((rs != NULL) && (rs != mRS)) {
        // throw new RSInvalidStateException("using object with mismatched context.");
        return E_RUNTIME_EXCEPTION;
    }
    *id = mID;
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
