
#include "GalaxyRS.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

ECode GalaxyRS::Init(
    /* [in] */ Int32 dpi,
    /* [in] */ RenderScript* rs,
    /* [in] */ IResources* res,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (!mInited) {
        mDensityDPI = dpi;

        mRS = rs;
        mRes = res;

        mWidth = width;
        mHeight = height;

    //     mOptionsARGB.inScaled = false;
    //     mOptionsARGB.inPreferredConfig = Bitmap.Config.ARGB_8888;

    //     mSpaceClouds = new ScriptField_Particle(mRS, SPACE_CLOUDSTAR_COUNT);
        AutoPtr<Mesh::AllocationBuilder> smb = new Mesh::AllocationBuilder(mRS);
    //     smb.addVertexAllocation(mSpaceClouds.getAllocation());
    //     smb.addIndexSetType(Mesh.Primitive.POINT);
    //     mSpaceCloudsMesh = smb.create();

    //     mBgStars = new ScriptField_Particle(mRS, BG_STAR_COUNT);
    //     Mesh.AllocationBuilder smb2 = new Mesh.AllocationBuilder(mRS);
    //     smb2.addVertexAllocation(mBgStars.getAllocation());
    //     smb2.addIndexSetType(Mesh.Primitive.POINT);
    //     mBgStarsMesh = smb2.create();

    //     mStaticStars = new ScriptField_Particle(mRS, STATIC_STAR_COUNT);
    //     Mesh.AllocationBuilder smb3 = new Mesh.AllocationBuilder(mRS);
    //     smb3.addVertexAllocation(mStaticStars.getAllocation());
    //     smb3.addIndexSetType(Mesh.Primitive.POINT);
    //     mStaticStarsMesh = smb3.create();

        mScript = new ScriptC_galaxy();
        FAIL_RETURN(mScript->constructor(mRS, mRes, R::raw::galaxy))
        mScript->Set_spaceCloudsMesh(mSpaceCloudsMesh);
    //     mScript.bind_spaceClouds(mSpaceClouds);
    //     mScript.set_bgStarsMesh(mBgStarsMesh);
    //     mScript.bind_bgStars(mBgStars);
    //     mScript.set_staticStarsMesh(mStaticStarsMesh);
    //     mScript.bind_staticStars(mStaticStars);

    //     mPvConsts = new ScriptField_VpConsts(mRS, 1);

    //     createProgramVertex();
    //     createProgramRaster();
    //     createProgramFragmentStore();
    //     createProgramFragment();

    //     loadTextures();

    //     mScript.set_densityDPI(mDensityDPI);
    //     mRS.bindRootScript(mScript);
    //     mScript.invoke_positionParticles();
    //     mInited = true;
    }
    return NOERROR;
}

ECode GalaxyRS::CreateProgramVertex()
{
    return NOERROR;
}

ECode GalaxyRS::Start()
{
    mRS->BindRootScript(mScript);
    return NOERROR;
}

ECode GalaxyRS::Stop()
{
    mRS->BindRootScript(NULL);
    return NOERROR;
}

ECode GalaxyRS::Resize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
