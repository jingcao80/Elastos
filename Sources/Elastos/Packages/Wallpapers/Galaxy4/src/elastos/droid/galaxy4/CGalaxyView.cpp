
#include "CGalaxyView.h"

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
        AutoPtr<>
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
    // if (mRS != null) {
    //     mRS.setSurface(null, 0, 0);
    //     mRS = null;
    //     destroyRenderScriptGL();
    // }
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
    synchronized (this) {
        // Surface will be destroyed when we return
        if (mRS != null) {
            mRS.setSurface(null, 0, 0);
        }
    }
}

ECode CGalaxyView::RsSetSurface(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h)
{
    validate();
    Surface s = null;
    if (sur != null) {
        s = sur.getSurface();
    }
    mWidth = w;
    mHeight = h;
    nContextSetSurface(w, h, s);
}

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos
