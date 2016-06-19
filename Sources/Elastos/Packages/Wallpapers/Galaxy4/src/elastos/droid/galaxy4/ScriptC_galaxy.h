
#ifndef __ELASTOS_DROID_GALAXY4_SCRIPTC_GALAXY_H__
#define __ELASTOS_DROID_GALAXY4_SCRIPTC_GALAXY_H__

#include "Elastos.Droid.Content.h"
#include "Mesh.h"
#include "ScriptC.h"

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class ScriptC_galaxy : public ScriptC
{
public:
    ScriptC_galaxy() {}

    CARAPI constructor(
        /* [in] */ RenderScript* rs,
        /* [in] */ IResources* resources,
        /* [in] */ Int32 id);

    CARAPI Set_spaceCloudsMesh(
        /* [in] */ Mesh* v);

private:
    const static Int32 mExportVarIdx_spaceCloudsMesh = 4;
    AutoPtr<Mesh> mExportVar_spaceCloudsMesh;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_SCRIPTC_GALAXY_H__