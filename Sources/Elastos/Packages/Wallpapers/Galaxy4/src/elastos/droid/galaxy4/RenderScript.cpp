
#include "Elastos.Droid.Utility.h"
#include "RenderScript.h"
#include <elastos/core/AutoLock.h>
#include <rs/rs.h>

using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::ISurface;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

ECode RenderScript::constructor(
    /* [in] */ IContext* ctx,
    /* [in] */ SurfaceConfig* sc)
{
    // super(ctx);
    mSurfaceConfig = new SurfaceConfig(sc);

    AutoPtr<IApplicationInfo> appInfo;
    ctx->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 sdkVersion;
    appInfo->GetTargetSdkVersion(&sdkVersion);

    mWidth = 0;
    mHeight = 0;
    mDev = nDeviceCreate();
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 dpi;
    dm->GetDensityDpi(&dpi);
    mContext = nContextCreateGL(mDev, 0, sdkVersion,
            mSurfaceConfig->mColorMin, mSurfaceConfig->mColorPref,
            mSurfaceConfig->mAlphaMin, mSurfaceConfig->mAlphaPref,
            mSurfaceConfig->mDepthMin, mSurfaceConfig->mDepthPref,
            mSurfaceConfig->mStencilMin, mSurfaceConfig->mStencilPref,
            mSurfaceConfig->mSamplesMin, mSurfaceConfig->mSamplesPref,
            mSurfaceConfig->mSamplesQ, dpi);
    if (mContext == 0) {
        // throw new RSDriverException("Failed to create RS context.");
        return E_RUNTIME_EXCEPTION;
    }
    // mMessageThread = new MessageThread(this);
    // mMessageThread.start();

    return NOERROR;
}

ECode RenderScript::SetSurface(
    /* [in] */ ISurfaceHolder* sur,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    // validate();
    AutoPtr<ISurface> s;
    if (sur != NULL) {
        sur->GetSurface((ISurface**)&s);
    }
    mWidth = w;
    mHeight = h;
    nContextSetSurface(w, h, s);
    return NOERROR;
}

ECode RenderScript::Destroy()
{
    return NOERROR;
}

Int64 RenderScript::nDeviceCreate()
{
    return (Int64)(uintptr_t)rsDeviceCreate();
}

Int64 RenderScript::nContextCreateGL(
    /* [in] */ Int64 dev,
    /* [in] */ Int32 ver,
    /* [in] */ Int32 sdkVer,
    /* [in] */ Int32 colorMin,
    /* [in] */ Int32 colorPref,
    /* [in] */ Int32 alphaMin,
    /* [in] */ Int32 alphaPref,
    /* [in] */ Int32 depthMin,
    /* [in] */ Int32 depthPref,
    /* [in] */ Int32 stencilMin,
    /* [in] */ Int32 stencilPref,
    /* [in] */ Int32 samplesMin,
    /* [in] */ Int32 samplesPref,
    /* [in] */ Float samplesQ,
    /* [in] */ Int32 dpi)
{
    AutoLock lock(this);

    RsSurfaceConfig sc;
    sc.alphaMin = alphaMin;
    sc.alphaPref = alphaPref;
    sc.colorMin = colorMin;
    sc.colorPref = colorPref;
    sc.depthMin = depthMin;
    sc.depthPref = depthPref;
    sc.samplesMin = samplesMin;
    sc.samplesPref = samplesPref;
    sc.samplesQ = samplesQ;

    return (Int64)(uintptr_t)rsContextCreateGL((RsDevice)dev, ver, sdkVer, sc, dpi);
}

void RenderScript::nContextSetSurface(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ ISurface* sur)
{
    AutoLock lock(this);

    ANativeWindow* window = NULL;
    if (wnd != NULL) {
        Surface* sur = ((Surface*)sur);
        sp<ANativeWindow> nWindow = sur->GetSurface();
        window = android_view_Surface_getNativeWindow(_env, wnd).get();
    }

    rsContextSetSurface((RsContext)mContext, width, height, window);
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
