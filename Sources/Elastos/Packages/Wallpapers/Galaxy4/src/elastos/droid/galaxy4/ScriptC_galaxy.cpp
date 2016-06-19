
#include "ScriptC_galaxy.h"

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

ECode ScriptC_galaxy::constructor(
    /* [in] */ RenderScript* rs,
    /* [in] */ IResources* resources,
    /* [in] */ Int32 id)
{
    return ScriptC::constructor(rs, resources, id);
}

ECode ScriptC_galaxy::Set_spaceCloudsMesh(
    /* [in] */ Mesh* v)
{
    FAIL_RETURN(SetVar(mExportVarIdx_spaceCloudsMesh, v));
    mExportVar_spaceCloudsMesh = v;
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
