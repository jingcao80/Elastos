//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
#include "elastos/droid/graphics/GraphicsNative.h"

#include <ui/Region.h>
#include <ui/Rect.h>

#include <gui/GLConsumer.h>
#include <gui/Surface.h>

#include <SkBitmap.h>
#include <SkCanvas.h>
#include <SkImage.h>
#include <android/native_window.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CSurfaceTexture;
using Elastos::Droid::Graphics::GraphicsNative;
using Elastos::Droid::Graphics::EIID_IOnFrameAvailableListener;
using Elastos::Droid::Utility::IDisplayMetrics;

using Elastos::Utility::Logging::Logger;

using namespace android;

namespace Elastos {
namespace Droid {
namespace View {

static const String TAG("TextureView");

// ----------------------------------------------------------------------------
// Native layer
// ----------------------------------------------------------------------------

// FIXME: consider exporting this to share (e.g. android_view_Surface.cpp)
static inline SkImageInfo convertPixelFormat(const ANativeWindow_Buffer& buffer) {
    SkImageInfo info;
    info.fWidth = buffer.width;
    info.fHeight = buffer.height;
    switch (buffer.format) {
        case WINDOW_FORMAT_RGBA_8888:
            info.fColorType = kN32_SkColorType;
            info.fAlphaType = kPremul_SkAlphaType;
            break;
        case WINDOW_FORMAT_RGBX_8888:
            info.fColorType = kN32_SkColorType;
            info.fAlphaType = kOpaque_SkAlphaType;
        case WINDOW_FORMAT_RGB_565:
            info.fColorType = kRGB_565_SkColorType;
            info.fAlphaType = kOpaque_SkAlphaType;
        default:
            info.fColorType = kUnknown_SkColorType;
            info.fAlphaType = kIgnore_SkAlphaType;
            break;
    }
    return info;
}

/**
 * This is a private API, and this implementation is also provided in the NDK.
 * However, the NDK links against android_runtime, which means that using the
 * NDK implementation would create a circular dependency between the libraries.
 */
static int32_t native_window_lock(ANativeWindow* window, ANativeWindow_Buffer* outBuffer,
        Rect* inOutDirtyBounds) {
    return window->perform(window, NATIVE_WINDOW_LOCK, outBuffer, inOutDirtyBounds);
}

static int32_t native_window_unlockAndPost(ANativeWindow* window) {
    return window->perform(window, NATIVE_WINDOW_UNLOCK_AND_POST);
}

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
    Logger::I(TAG, " >> OnFrameAvailableListener::OnFrameAvailable");
    mHost->UpdateLayer();
    mHost->Invalidate();
    return NOERROR;
}

//========================================================================================
//              TextureView::
//========================================================================================
//const String TextureView::TAG("TextureView");

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

    mUpdateListener = new OnFrameAvailableListener(this);
}

ECode TextureView::IsOpaque(
    /* [out] */ Boolean* opaque)
{
    VALIDATE_NOT_NULL(opaque);
    *opaque = mOpaque;
    return NOERROR;
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
    if (IsHardwareAccelerated(&bIsAcc), !bIsAcc) {
        Logger::W(TAG,
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
        if (shouldRelease) mSurface->ReleaseResources();
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
    Logger::I(TAG, " >> Draw");
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

ECode TextureView::OnSizeChanged(
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
    return NOERROR;
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

        mAttachInfo->mHardwareRenderer->CreateTextureLayer((IHardwareLayer**)&mLayer);
        if (!mUpdateSurface) {
            // Create a new SurfaceTexture for the layer.
            mSurface = NULL;
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
    AutoLock lock(mLock);
    mUpdateLayer = TRUE;
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

    Logger::I(TAG, " >> ApplyUpdate: (%d, %d), opaque: %d, result: %d", w, h, mOpaque, bRes);
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
    if (width > 0 && height > 0 & (IsAvailable(&bIsAvail), bIsAvail)) {
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        AutoPtr<IDisplayMetrics> dm;
        res->GetDisplayMetrics((IDisplayMetrics**)&dm);
        AutoPtr<IBitmap> bmp;
        CBitmap::CreateBitmap(dm, width, height, BitmapConfig_ARGB_8888, (IBitmap**)&bmp);
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
        mSurface->ReleaseResources();
    }
    mSurface = surfaceTexture;
    mUpdateSurface = TRUE;

    // If the view is visible, update the listener in the new surface to use
    // the existing listener in the view.
    if (((mViewFlags & VISIBILITY_MASK) == VISIBLE)) {
        mSurface->SetOnFrameAvailableListener(mUpdateListener);
    }
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
    CSurfaceTexture* surfaceImpl = (CSurfaceTexture*)surface;
    sp<IGraphicBufferProducer> producer((IGraphicBufferProducer*)surfaceImpl->mProducer);;
    sp<ANativeWindow> window = new android::Surface(producer, true);

    window->incStrong((void*)&TextureView::NativeCreateNativeWindow);
    mNativeWindow = (Int64)(window.get());
}

void TextureView::NativeDestroyNativeWindow()
{
    ANativeWindow* nativeWindow = (ANativeWindow*)mNativeWindow;

    if (nativeWindow) {
        sp<ANativeWindow> window(nativeWindow);
            window->decStrong((void*)&TextureView::NativeCreateNativeWindow);
        mNativeWindow = 0;
    }
}

Boolean TextureView::NativeLockCanvas(
    /* [in] */ Int64 nativeWindow,
    /* [in] */ ICanvas* canvas,
    /* [in] */ IRect* dirtyRect)
{
    if (!nativeWindow) {
        return FALSE;
    }

    ANativeWindow_Buffer buffer;

    Rect rect;
    if (dirtyRect) {
        Int32 l, t, r, b;
        dirtyRect->GetLeft(&l);
        dirtyRect->GetTop(&t);
        dirtyRect->GetRight(&r);
        dirtyRect->GetBottom(&b);
        rect.left = l;
        rect.top = t;
        rect.right = r;
        rect.bottom = b;
    } else {
        rect.set(Rect(0x3FFF, 0x3FFF));
    }

    sp<ANativeWindow> window((ANativeWindow*) nativeWindow);
    int32_t status = native_window_lock(window.get(), &buffer, &rect);
    if (status) return FALSE;

    ssize_t bytesCount = buffer.stride * bytesPerPixel(buffer.format);

    SkBitmap bitmap;
    bitmap.setInfo(convertPixelFormat(buffer), bytesCount);

    if (buffer.width > 0 && buffer.height > 0) {
        bitmap.setPixels(buffer.bits);
    } else {
        bitmap.setPixels(NULL);
    }

    canvas->SetSurfaceFormat(buffer.format);
    canvas->SetNativeBitmap(reinterpret_cast<Int64>(&bitmap));

    SkRect clipRect;
    clipRect.set(rect.left, rect.top, rect.right, rect.bottom);
    SkCanvas* nativeCanvas = GraphicsNative::GetNativeCanvas(canvas);
    nativeCanvas->clipRect(clipRect);

    if (dirtyRect) {
        dirtyRect->Set(int(rect.left), int(rect.top), int(rect.right), int(rect.bottom));
    }

    return TRUE;
}

void TextureView::NativeUnlockCanvasAndPost(
    /* [in] */ Int64 nativeWindow,
    /* [in] */ ICanvas* canvas)
{
    canvas->SetNativeBitmap(0LL);

    if (nativeWindow) {
        sp<ANativeWindow> window((ANativeWindow*) nativeWindow);
        native_window_unlockAndPost(window.get());
    }
}

} // namespace View
} // namespace Droid
} // namespace Elastos
