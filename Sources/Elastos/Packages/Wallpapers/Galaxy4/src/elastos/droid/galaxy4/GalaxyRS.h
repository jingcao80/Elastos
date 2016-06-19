
#ifndef __ELASTOS_DROID_GALAXY4_GALAXYRS_H__
#define __ELASTOS_DROID_GALAXY4_GALAXYRS_H__

#include "Elastos.Droid.Content.h"
#include "Mesh.h"
#include "RenderScript.h"
#include "ScriptC_galaxy.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

class GalaxyRS
    : public Object
{
public:
    CARAPI Init(
        /* [in] */ Int32 dpi,
        /* [in] */ RenderScript* rs,
        /* [in] */ IResources* res,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    CARAPI CreateProgramVertex();

    CARAPI Start();

    CARAPI Stop();

    CARAPI Resize(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

private:
    AutoPtr<IResources> mRes;

    AutoPtr<RenderScript> mRS;
    AutoPtr<ScriptC_galaxy> mScript;

    AutoPtr<Mesh> mSpaceCloudsMesh;

    Int32 mHeight;
    Int32 mWidth;
    Boolean mInited;
    Int32 mDensityDPI;
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_GALAXYRS_H__
