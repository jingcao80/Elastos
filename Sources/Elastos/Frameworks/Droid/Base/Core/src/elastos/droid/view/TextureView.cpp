
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/TextureView.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/graphics/CMatrix.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CSurfaceTexture.h"

#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::Graphics::EIID_IOnFrameAvailableListener;
using Elastos::Droid::Utility::IDisplayMetrics;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

//========================================================================================
//              TextureView::OnFrameAvailableListener::
//========================================================================================
CAR_INTERFACE_IMPL(TextureView::OnFrameAvailableListener, Object, IOnFrameAvailableListener)

TextureView::OnFrameAvailableListener::OnFrameAvailableListener(
    /* [in] */ TextureView* h) : mHost(h)
{
}

ECode TextureView::OnFrameAvailableListener::OnFrameAvailable(
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    mHost->UpdateLayer();
    mHost->Invalidate();
    return NOERROR;
}

//========================================================================================
//              TextureView::
//========================================================================================
//const String TextureView::LOG_TAG("TextureView");

CAR_INTERFACE_IMPL(TextureView, View, ITextureView)

TextureView::TextureView()
    : mHadSurface(FALSE)
    , mOpaque(TRUE)
    , mMatrixChanged(FALSE)
    , mUpdateLayer(FALSE)
    , mUpdateSurface(FALSE)
    , mSaveCount(0)
    , mNativeWindow(0)
{
}

ECode TextureView::constructor(
    /* [in] */ IContext* context)
{
    FAIL_RETURN(View::constructor(context));
    Init();
    return NOERROR;
}

ECode TextureView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(View::constructor(context, attrs));
    Init();
    return NOERROR;
}

ECode TextureView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    ECode ec = View::constructor(context, attrs, defStyleAttr);
    Init();
    return ec;
}

ECode TextureView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    ECode ec = View::constructor(context, attrs, defStyleAttr, defStyleRes);
    Init();
    return ec;
}

void TextureView::Init()
{
    CMatrix::New((IMatrix**)&mMatrix);
    CPaint::New((IPaint**)&mLayerPaint);

//    mLock = ArrayOf<IInterface*>::Alloc(1);

//    mNativeWindowLock = ArrayOf<IInterface*>::Alloc(1);

    mUpdateListener = new OnFrameAvailableListener(this);
}

Boolean TextureView::IsOpaque()
{
    return mOpaque;
}

ECode TextureView::SetOpaque(
    /* [in] */ Boolean opaque)
{
    if (opaque != mOpaque) {
        mOpaque = opaque;
        if (mLayer != NULL) {
            UpdateLayerAndInvalidate();
        }
    }
    return NOERROR;
}

ECode TextureView::OnAttachedToWindow()
{
    View::OnAttachedToWindow();

    Boolean bIsAcc = FALSE;
    if (!(IsHardwareAccelerated(&bIsAcc), bIsAcc)) {
        Logger::W(LOG_TAG,
            "A TextureView or a subclass can only be used with hardware acceleration enabled.");
    }

    if (mHadSurface) {
        Invalidate(TRUE);
        mHadSurface = FALSE;
    }
    return NOERROR;
}

ECode TextureView::OnDetachedFromWindowInternal()
{
    DestroySurface();
    return View::OnDetachedFromWindowInternal();
}

void TextureView::DestroySurface()
{
    if (mLayer != NULL) {
        mLayer->DetachSurfaceTexture();

        Boolean shouldRelease = TRUE;
        if (mListener != NULL) {
            mListener->OnSurfaceTextureDestroyed(mSurface, &shouldRelease);
        }

        {
            AutoLock lock(mNativeWindowLock);
            NativeDestroyNativeWindow();
        }

        mLayer->Destory();
        if (shouldRelease) mSurface->Release();
        mSurface = NULL;
        mLayer = NULL;

        mHadSurface = TRUE;
    }
}

ECode TextureView::SetLayerType(
    /* [in] */ Int32 layerType,
    /* [in] */ IPaint* paint)
{
    if (paint != mLayerPaint) {
        if (paint == NULL) {
            CPaint::New((IPaint**)&mLayerPaint);
        }
        else {
            mLayerPaint = paint;
        }
        Invalidate();
    }
    return NOERROR;
}

ECode TextureView::SetLayerPaint(
    /* [in] */ IPaint* paint)
{
    SetLayerType(/* ignored */ 0, paint);
    return NOERROR;
}

ECode TextureView::GetLayerType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type)

    *type = LAYER_TYPE_HARDWARE;
    return NOERROR;
}

Boolean TextureView::HasStaticLayer()
{
    return TRUE;
}

ECode TextureView::BuildLayer()
{
    return NOERROR;
}

ECode TextureView::Draw(
    /* [in] */ ICanvas* canvas)
{
    // NOTE: Maintain this carefully (see View.java)
    mPrivateFlags = (mPrivateFlags & ~PFLAG_DIRTY_MASK) | PFLAG_DRAWN;

    ApplyUpdate();
    ApplyTransformMatrix();
    return NOERROR;
}

void TextureView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
}

void TextureView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    View::OnSizeChanged(w, h, oldw, oldh);
    if (mSurface != NULL) {
        Int32 w = 0, h = 0;
        GetWidth(&w);
        GetHeight(&h);
        mSurface->SetDefaultBufferSize(w, h);
        UpdateLayer();
        if (mListener != NULL) {
            mListener->OnSurfaceTextureSizeChanged(mSurface, w, h);
        }
    }
}

void TextureView::DestroyHardwareResources()
{
    View::DestroyHardwareResources();
    DestroySurface();
    InvalidateParentCaches();
    Invalidate(TRUE);
}

AutoPtr<IHardwareLayer> TextureView::GetHardwareLayer()
{
    // NOTE: Maintain these two lines very carefully (see View.java)
    mPrivateFlags |= PFLAG_DRAWN | PFLAG_DRAWING_CACHE_VALID;
    mPrivateFlags &= ~PFLAG_DIRTY_MASK;

    if (mLayer == NULL) {
        if (mAttachInfo == NULL || mAttachInfo->mHardwareRenderer == NULL) {
            return NULL;
        }

    assert(0 && "TODO");
    //    mLayer = mAttachInfo->mHardwareRenderer->CreateTextureLayer();
        if (!mUpdateSurface) {
            // Create a new SurfaceTexture for the layer.
            CSurfaceTexture::New(FALSE, (ISurfaceTexture**)&mSurface);
            mLayer->SetSurfaceTexture(mSurface);
        }
        Int32 w = 0, h = 0;
        GetWidth(&w);
        GetHeight(&h);
        mSurface->SetDefaultBufferSize(w, h);
        NativeCreateNativeWindow(mSurface);

        mSurface->SetOnFrameAvailableListener(mUpdateListener, mAttachInfo->mHandler);

        if (mListener != NULL && !mUpdateSurface) {
            mListener->OnSurfaceTextureAvailable(mSurface, w, h);
        }
        mLayer->SetLayerPaint(mLayerPaint);
    }

    if (mUpdateSurface) {
        // Someone has requested that we use a specific SurfaceTexture, so
        // tell mLayer about it and set the SurfaceTexture to use the
        // current view size.
        mUpdateSurface = FALSE;

        // Since we are updating the layer, force an update to ensure its
        // parameters are correct (width, height, transform, etc.)
        UpdateLayer();
        mMatrixChanged = TRUE;

        mLayer->SetSurfaceTexture(mSurface);
        Int32 w = 0, h = 0;
        GetWidth(&w);
        GetHeight(&h);
        mSurface->SetDefaultBufferSize(w, h);
    }

    ApplyUpdate();
    ApplyTransformMatrix();

    return mLayer;
}

ECode TextureView::OnVisibilityChanged(
    /* [in] */ IView* changedView,
    /* [in] */ Int32 visibility)
{
    View::OnVisibilityChanged(changedView, visibility);

    if (mSurface != NULL) {
        // When the view becomes invisible, stop updating it, it's a waste of CPU
        // To cancel updates, the easiest thing to do is simply to remove the
        // updates listener
        if (visibility == VISIBLE) {
            if (mLayer != NULL) {
                mSurface->SetOnFrameAvailableListener(mUpdateListener, mAttachInfo->mHandler);
            }
            UpdateLayerAndInvalidate();
        }
        else {
            mSurface->SetOnFrameAvailableListener(NULL);
        }
    }
    return NOERROR;
}

void TextureView::UpdateLayer()
{
    {
        AutoLock lock(mLock);
        mUpdateLayer = TRUE;
    }
}

void TextureView::UpdateLayerAndInvalidate()
{
    {
        AutoLock lock(mLock);
        mUpdateLayer = TRUE;
    }
    Invalidate();
}

void TextureView::ApplyUpdate()
{
    if (mLayer == NULL) {
        return;
    }

    {
        AutoLock lock(mLock);
        if (mUpdateLayer) {
            mUpdateLayer = FALSE;
        }
        else {
            return;
        }
    }

    Int32 w = 0, h = 0;
    GetWidth(&w);
    GetHeight(&h);
    Boolean bRes = FALSE;
    mLayer->Prepare(w, h, mOpaque, &bRes);
    mLayer->UpdateSurfaceTexture();

    if (mListener != NULL) {
        mListener->OnSurfaceTextureUpdated(mSurface);
    }
}

ECode TextureView::SetTransform(
    /* [in] */ IMatrix* transform)
{
    mMatrix->Set(transform);
    mMatrixChanged = TRUE;
    InvalidateParentIfNeeded();
    return NOERROR;
}

ECode TextureView::GetTransform(
    /* [in] */ IMatrix* transform,
    /* [out] */ IMatrix** result)
{
    VALIDATE_NOT_NULL(result)

    AutoPtr<IMatrix> tf = transform;
    if (tf == NULL) {
        CMatrix::New((IMatrix**)&tf);
    }

    tf->Set(mMatrix);

    *result = tf.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void TextureView::ApplyTransformMatrix()
{
    if (mMatrixChanged && mLayer != NULL) {
        mLayer->SetTransform(mMatrix);
        mMatrixChanged = FALSE;
    }
}

ECode TextureView::GetBitmap(
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result)

    Int32 w = 0, h = 0;
    GetWidth(&w);
    GetHeight(&h);
    return GetBitmap(w, h, result);
}

ECode TextureView::GetBitmap(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result)

    Boolean bIsAvail = FALSE;
    if ((IsAvailable(&bIsAvail), bIsAvail) && width > 0 && height > 0) {
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        AutoPtr<IDisplayMetrics> dm;
        res->GetDisplayMetrics((IDisplayMetrics**)&dm);
        AutoPtr<IBitmap> bmp;
        CBitmap::CreateBitmap(dm,
                width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bmp);
        return GetBitmap(bmp, result);
    }
    *result = NULL;
    return NOERROR;
}

ECode TextureView::GetBitmap(
    /* [in] */ IBitmap* bitmap,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result)

    Boolean bIsAvail = FALSE;
    if (bitmap != NULL && (IsAvailable(&bIsAvail), bIsAvail)) {
        ApplyUpdate();
        ApplyTransformMatrix();

        // This case can happen if the app invokes setSurfaceTexture() before
        // we are able to create the hardware layer. We can safely initialize
        // the layer here thanks to the validate() call at the beginning of
        // this method
        if (mLayer == NULL && mUpdateSurface) {
            GetHardwareLayer();
        }

        if (mLayer != NULL) {
            Boolean res = FALSE;
            mLayer->CopyInto(bitmap, &res);
        }
    }
    *result = bitmap;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TextureView::IsAvailable(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSurface != NULL;
    return NOERROR;
}

ECode TextureView::LockCanvas(
    /* [out] */ ICanvas** canvas)
{
    VALIDATE_NOT_NULL(canvas)

    return LockCanvas(NULL, canvas);
}

ECode TextureView::LockCanvas(
    /* [in] */ IRect* dirty,
    /* [out] */ ICanvas** canvas)
{
    VALIDATE_NOT_NULL(canvas)

    Boolean bIsAvail = FALSE;
    if (!(IsAvailable(&bIsAvail), bIsAvail)) {
        *canvas = NULL;
        return NOERROR;
    }

    if (mCanvas == NULL) {
        CCanvas::New((ICanvas**)&mCanvas);
    }

    {
        AutoLock lock(mNativeWindowLock);
        if (!NativeLockCanvas(mNativeWindow, mCanvas, dirty)) {
            *canvas = NULL;
            return NOERROR;
        }
    }
    mCanvas->Save(&mSaveCount);

    *canvas = mCanvas;
    REFCOUNT_ADD(*canvas)
    return NOERROR;
}

ECode TextureView::UnlockCanvasAndPost(
    /* [in] */ ICanvas* canvas)
{
    if (mCanvas != NULL && canvas == mCanvas.Get()) {
        canvas->RestoreToCount(mSaveCount);
        mSaveCount = 0;

        {
            AutoLock lock(mNativeWindowLock);
            NativeUnlockCanvasAndPost(mNativeWindow, mCanvas);
        }
    }
    return NOERROR;
}

ECode TextureView::GetSurfaceTexture(
    /* [out] */ ISurfaceTexture** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mSurface;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TextureView::SetSurfaceTexture(
    /* [in] */ ISurfaceTexture* surfaceTexture)
{
    if (surfaceTexture == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mSurface != NULL) {
        mSurface->ReleaseBuffers();
    }
    mSurface = surfaceTexture;
    mUpdateSurface = TRUE;
    InvalidateParentIfNeeded();
    return NOERROR;
}

ECode TextureView::GetSurfaceTextureListener(
    /* [out] */ ISurfaceTextureListener** result)
{
    VALIDATE_NOT_NULL(result)

    *result = mListener;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode TextureView::SetSurfaceTextureListener(
    /* [in] */ ISurfaceTextureListener* listener)
{
    mListener = listener;
    return NOERROR;
}

void TextureView::NativeCreateNativeWindow(
    /* [in] */ ISurfaceTexture* surface)
{
    assert(0 && "TODO");
}

void TextureView::NativeDestroyNativeWindow()
{
    assert(0 && "TODO");
}

Boolean TextureView::NativeLockCanvas(
    /* [in] */ Int64 nativeWindow,
    /* [in] */ ICanvas* canvas,
    /* [in] */ IRect* dirty)
{
    assert(0 && "TODO");
    return FALSE;
}

void TextureView::NativeUnlockCanvasAndPost(
    /* [in] */ Int64 nativeWindow,
    /* [in] */ ICanvas* canvas)
{
    assert(0 && "TODO");
}

} // namespace View
} // namespace Droid
} // namespace Elastos
