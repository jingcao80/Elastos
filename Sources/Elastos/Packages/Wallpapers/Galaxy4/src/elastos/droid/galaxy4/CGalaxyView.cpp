
#include "Elastos.Droid.Content.h"
#include "CGalaxyView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::EIID_ISurfaceHolderCallback;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

//====================================================================
// CGalaxyView::SHCallback
//====================================================================
CAR_INTERFACE_IMPL(CGalaxyView::SHCallback, Object, ISurfaceHolderCallback)

CGalaxyView::SHCallback::SHCallback(
    /* [in] */ CGalaxyView* host)
    : mHost(host)
{}

ECode CGalaxyView::SHCallback::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    return mHost->SurfaceCreated(holder);
}

ECode CGalaxyView::SHCallback::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    return mHost->SurfaceChanged(holder, format, width, height);
}

ECode CGalaxyView::SHCallback::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    return mHost->SurfaceDestroyed(holder);
}


//===================================================================
// CGalaxyView
//===================================================================
ECode CGalaxyView::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(SurfaceView::constructor(context));
    Init();
    SetFocusable(TRUE);
    SetFocusableInTouchMode(TRUE);
    return NOERROR;
}

ECode CGalaxyView::SurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    {
        AutoLock lock(this);
        if (mRS != NULL) {
            mRS->SetSurface(holder, width, height);
        }
    }
    if (mRS == NULL) {
        AutoPtr<RenderScript::SurfaceConfig> sc = new RenderScript::SurfaceConfig();
        FAIL_RETURN(CreateRenderScript(sc, (RenderScript**)&mRS))
        mRS->SetSurface(holder, width, height);


    }
    else {

    }
    // if (mRS == null) {
    //     RenderScriptGL.SurfaceConfig sc = new RenderScriptGL.SurfaceConfig();
    //     mRS = createRenderScriptGL(sc);
    //     mRS.setSurface(holder, w, h);

    //     DisplayMetrics metrics = new DisplayMetrics();
    //     ((WindowManager) getContext()
    //             .getSystemService(Service.WINDOW_SERVICE))
    //             .getDefaultDisplay().getMetrics(metrics);

    //     mRender = new GalaxyRS();
    //     mRender.init(metrics.densityDpi, mRS, getResources(), w, h);
    // } else {
    //     mRender.createProgramVertex();
    // }
    return NOERROR;
}

ECode CGalaxyView::OnDetachedFromWindow()
{
    if (mRS != NULL) {
        mRS->SetSurface(NULL, 0, 0);
        mRS = NULL;
        DestroyRenderScript();
    }
    return NOERROR;
}

void CGalaxyView::Init()
{
    // Install a SurfaceHolder.Callback so we get notified when the
    // underlying surface is created and destroyed
    AutoPtr<ISurfaceHolder> holder;
    GetHolder((ISurfaceHolder**)&holder);
    mCallback = new SHCallback(this);
    holder->AddCallback(mCallback);
}

ECode CGalaxyView::SurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    mSurfaceHolder = holder;
    return NOERROR;
}

ECode CGalaxyView::SurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    {
        AutoLock lock(this);
        // Surface will be destroyed when we return
        if (mRS != NULL) {
            mRS->SetSurface(NULL, 0, 0);
        }
    }
    return NOERROR;
}

ECode CGalaxyView::CreateRenderScript(
    /* [in] */ RenderScript::SurfaceConfig* sc,
    /* [out] */ RenderScript** result)
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<RenderScript> rs = new RenderScript();
    FAIL_RETURN(rs->constructor(ctx, sc));
    SetRenderScript(rs);
    *result = rs;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CGalaxyView::DestroyRenderScript()
{
    {
        AutoLock lock(this);
        mRS->Destroy();
        mRS = NULL;
    }
    return NOERROR;
}

ECode CGalaxyView::SetRenderScript(
    /* [in] */ RenderScript* rs)
{
    mRS = rs;
    return NOERROR;
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
