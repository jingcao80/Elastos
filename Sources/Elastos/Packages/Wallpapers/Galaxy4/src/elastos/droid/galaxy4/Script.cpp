
#include "Script.h"
#include "RenderScript.h"

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

ECode Script::constructor(
    /* [in] */ Int64 id,
    /* [in] */ RenderScript* rs)
{
    return BaseObj::constructor(id, rs);
}

ECode Script::SetVar(
    /* [in] */ Int32 index,
    /* [in] */ BaseObj* o)
{
    FAIL_RETURN(mRS->Validate());
    FAIL_RETURN(mRS->ValidateObject(o));
    Int64 id;
    FAIL_RETURN(GetID(mRS, &id));
    Int64 val = 0;
    if (o != NULL) {
        FAIL_RETURN(o->GetID(mRS, &val));
    }
    mRS->nScriptSetVarObj(id, index, val);
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
