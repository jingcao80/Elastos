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

#include "elastos/droid/systemui/CImageWallpaper.h"
#include "R.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.RenderScript.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::EIID_IService;
using Elastos::Droid::App::IActivityManagerHelper;
using Elastos::Droid::App::CActivityManagerHelper;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::EIID_IContextWrapper;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::RegionOp_DIFFERENCE;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::View::IWindowManager;
using Elastos::Droid::Opengl::IEGL14;
using Elastos::Droid::Opengl::IGLES20;
using Elastos::Droid::Opengl::CGLES20;
using Elastos::Droid::Opengl::IGLUtils;
using Elastos::Droid::Opengl::CGLUtils;
using Elastos::Droid::Opengl::Gles::CEGL10Helper;
using Elastos::Droid::Opengl::Gles::CEGLContextHelper;
using Elastos::Droid::RenderScript::IMatrix4f;
using Elastos::Droid::RenderScript::CMatrix4f;
using Elastosx::Microedition::Khronos::Egl::IEGL;
using Elastosx::Microedition::Khronos::Egl::IEGL10Helper;
using Elastosx::Microedition::Khronos::Egl::IEGLContextHelper;
using Elastos::Core::CString;
using Elastos::IO::ByteOrder;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::IByteBufferHelper;
using Elastos::IO::CByteBufferHelper;
using Elastos::IO::IByteOrderHelper;
using Elastos::IO::CByteOrderHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

static const String TAG("CImageWallpaper");
const String CImageWallpaper::GL_LOG_TAG("CImageWallpaperGL");
const Boolean CImageWallpaper::DEBUG = FALSE;
const String CImageWallpaper::PROPERTY_KERNEL_QEMU("ro.kernel.qemu");
const Boolean CImageWallpaper::FIXED_SIZED_SURFACE = TRUE;
const Boolean CImageWallpaper::USE_OPENGL = TRUE;

CAR_OBJECT_IMPL(CImageWallpaper)

CAR_INTERFACE_IMPL(CImageWallpaper, WallpaperService, IImageWallpaper)

CImageWallpaper::CImageWallpaper()
    : mIsHwAccelerated(FALSE)
{
}

ECode CImageWallpaper::OnCreate()
{
    WallpaperService::OnCreate();

    AutoPtr<IInterface> obj;
    GetSystemService(WALLPAPER_SERVICE, (IInterface**)&obj);
    mWallpaperManager = IWallpaperManager::Probe(obj);

    //noinspection PointlessBooleanExpression,ConstantConditions
    if (FIXED_SIZED_SURFACE && USE_OPENGL) {
        if (!IsEmulator()) {
            AutoPtr<IActivityManagerHelper> helper;
            CActivityManagerHelper::AcquireSingleton((IActivityManagerHelper**)&helper);
            helper->IsHighEndGfx(&mIsHwAccelerated);
        }
    }

    return NOERROR;
}

ECode CImageWallpaper::OnTrimMemory(
    /* [in] */ Int32 level)
{
    if (mEngine != NULL) {
        mEngine->TrimMemory(level);
    }
    return NOERROR;
}

Boolean CImageWallpaper::IsEmulator()
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String str;
    sysProp->Get(PROPERTY_KERNEL_QEMU, String("0"), &str);
    return str.Equals("1");
}

ECode CImageWallpaper::OnCreateEngine(
    /* [out] */ IWallpaperServiceEngine** engine)
{
    VALIDATE_NOT_NULL(engine);
    mEngine = new DrawableEngine(this);
    *engine = mEngine;
    REFCOUNT_ADD(*engine);
    return NOERROR;
}

CImageWallpaper::DrawableEngine::WallpaperObserver::WallpaperObserver(
    /* [in] */ DrawableEngine* owner)
    : mOwner(owner)
{}

ECode CImageWallpaper::DrawableEngine::WallpaperObserver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    if (DEBUG) {
        Logger::D(TAG, "onReceive");
    }

    mOwner->mLastSurfaceWidth = mOwner->mLastSurfaceHeight = -1;
    mOwner->mBackground = NULL;
    mOwner->mBackgroundWidth = -1;
    mOwner->mBackgroundHeight = -1;
    mOwner->mRedrawNeeded = TRUE;
    mOwner->DrawFrame();

    return NOERROR;
}

const String CImageWallpaper::DrawableEngine::sSimpleVS
    = String("attribute vec4 position;\n"
            "attribute vec2 texCoords;\n"
            "varying vec2 outTexCoords;\n"
            "uniform mat4 projection;\n"
            "\nvoid main(void) {\n"
            "    outTexCoords = texCoords;\n"
            "    gl_Position = projection * position;\n"
            "}\n\n");
const String CImageWallpaper::DrawableEngine::sSimpleFS
    = String("precision mediump float;\n\n"
            "varying vec2 outTexCoords;\n"
            "uniform sampler2D texture;\n"
            "\nvoid main(void) {\n"
            "    gl_FragColor = texture2D(texture, outTexCoords);\n"
            "}\n\n");

const Int32 CImageWallpaper::DrawableEngine::EGL_CONTEXT_CLIENT_VERSION = 0x3098;
const Int32 CImageWallpaper::DrawableEngine::EGL_OPENGL_ES2_BIT = 4;
const Int32 CImageWallpaper::DrawableEngine::FLOAT_SIZE_BYTES = 4;
const Int32 CImageWallpaper::DrawableEngine::TRIANGLE_VERTICES_DATA_STRIDE_BYTES = 5 * FLOAT_SIZE_BYTES;
const Int32 CImageWallpaper::DrawableEngine::TRIANGLE_VERTICES_DATA_POS_OFFSET = 0;
const Int32 CImageWallpaper::DrawableEngine::TRIANGLE_VERTICES_DATA_UV_OFFSET = 3;

CImageWallpaper::DrawableEngine::DrawableEngine(
    /* [in] */ CImageWallpaper* owner)
    : Engine(owner)
    , mBackgroundWidth(-1)
    , mBackgroundHeight(-1)
    , mLastSurfaceWidth(-1)
    , mLastSurfaceHeight(-1)
    , mLastRotation(-1)
    , mXOffset(0.5f)
    , mYOffset(0.5f)
    , mScale(1.f)
    , mVisible(TRUE)
    , mRedrawNeeded(FALSE)
    , mOffsetsChanged(FALSE)
    , mLastXTranslation(0)
    , mLastYTranslation(0)
    , mOwner(owner)
{
    SetFixedSizeAllowed(TRUE);
}

void CImageWallpaper::DrawableEngine::TrimMemory(
    /* [in] */ Int32 level)
{
    if (level >= IComponentCallbacks2::TRIM_MEMORY_RUNNING_LOW &&
            mBackground != NULL && mOwner->mIsHwAccelerated) {
        if (DEBUG) {
            Logger::D(TAG, "trimMemory");
        }
        mBackground->Recycle();
        mBackground = NULL;
        mBackgroundWidth = -1;
        mBackgroundHeight = -1;
        mOwner->mWallpaperManager->ForgetLoadedWallpaper();
    }
}

ECode CImageWallpaper::DrawableEngine::OnCreate(
    /* [in] */ ISurfaceHolder* surfaceHolder)
{
    if (DEBUG) {
        Logger::D(TAG, "onCreate");
    }

    Engine::OnCreate(surfaceHolder);

    // TODO: Don't need this currently because the wallpaper service
    // will restart the image wallpaper whenever the image changes.
    //IntentFilter filter = new IntentFilter(Intent.ACTION_WALLPAPER_CHANGED);
    //mReceiver = new WallpaperObserver();
    //registerReceiver(mReceiver, filter, NULL, mHandler);

    UpdateSurfaceSize(surfaceHolder);

    SetOffsetNotificationsEnabled(FALSE);
    return NOERROR;
}

ECode CImageWallpaper::DrawableEngine::OnDestroy()
{
    Engine::OnDestroy();
    if (mReceiver != NULL) {
        mOwner->UnregisterReceiver(mReceiver);
    }
    mBackground = NULL;
    return NOERROR;
}

void CImageWallpaper::DrawableEngine::UpdateSurfaceSize(
    /* [in] */ ISurfaceHolder* surfaceHolder)
{
    AutoPtr<IPoint> p = GetDefaultDisplaySize();

    // Load background image dimensions, if we haven't saved them yet
    if (mBackgroundWidth <= 0 || mBackgroundHeight <= 0) {
        // Need to load the image to get dimensions
        mOwner->mWallpaperManager->ForgetLoadedWallpaper();
        UpdateWallpaperLocked();
        if (mBackgroundWidth <= 0 || mBackgroundHeight <= 0) {
            // Default to the display size if we can't find the dimensions
            p->Get(&mBackgroundWidth, &mBackgroundHeight);
        }
    }

    // Force the wallpaper to cover the screen in both dimensions
    Int32 x = 0, y = 0;
    p->Get(&x, &y);
    Int32 surfaceWidth = Elastos::Core::Math::Max(x, mBackgroundWidth);
    Int32 surfaceHeight = Elastos::Core::Math::Max(y, mBackgroundHeight);

    // If the surface dimensions haven't changed, then just return
    AutoPtr<IRect> frame;
    surfaceHolder->GetSurfaceFrame((IRect**)&frame);
    if (frame != NULL) {
        Int32 dw = 0;
        frame->GetWidth(&dw);
        Int32 dh = 0;
        frame->GetHeight(&dh);
        if (surfaceWidth == dw && surfaceHeight == dh) {
            return;
        }
    }

    if (FIXED_SIZED_SURFACE) {
        // Used a fixed size surface, because we are special.  We can do
        // this because we know the current design of window animations doesn't
        // cause this to break.
        surfaceHolder->SetFixedSize(surfaceWidth, surfaceHeight);
    }
    else {
        surfaceHolder->SetSizeFromLayout();
    }
}

ECode CImageWallpaper::DrawableEngine::OnVisibilityChanged(
    /* [in] */ Boolean visible)
{
    if (DEBUG) {
        Logger::D(TAG, "onVisibilityChanged: mVisible, visible=%d, %d", mVisible, visible);
    }

    if (mVisible != visible) {
        if (DEBUG) {
            Logger::D(TAG, "Visibility changed to visible=%d", visible);
        }
        mVisible = visible;
        DrawFrame();
    }
    return NOERROR;
}

ECode CImageWallpaper::DrawableEngine::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    return Engine::OnTouchEvent(event);
}

ECode CImageWallpaper::DrawableEngine::OnOffsetsChanged(
    /* [in] */ Float xOffset,
    /* [in] */ Float yOffset,
    /* [in] */ Float xOffsetStep,
    /* [in] */ Float yOffsetStep,
    /* [in] */ Int32 xPixels,
    /* [in] */ Int32 yPixels)
{
    if (DEBUG) {
        Logger::D(TAG, "onOffsetsChanged: xOffset=%f , yOffset=%f,"
            " xOffsetStep=%f, yOffsetStep=%f, xPixels=%d, yPixels=%d",
            xOffset, yOffset, xOffsetStep, yOffsetStep, xPixels, yPixels);
    }

    if (mXOffset != xOffset || mYOffset != yOffset) {
        if (DEBUG) {
            Logger::D(TAG, "Offsets changed to (%f,%f).", xOffset, yOffset);
        }
        mXOffset = xOffset;
        mYOffset = yOffset;
        mOffsetsChanged = TRUE;
    }
    DrawFrame();
    return NOERROR;
}

ECode CImageWallpaper::DrawableEngine::OnSurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (DEBUG) {
        Logger::D(TAG, "onSurfaceChanged: width=%d, height=%d", width, height);
    }

    Engine::OnSurfaceChanged(holder, format, width, height);

    DrawFrame();
    return NOERROR;
}

ECode CImageWallpaper::DrawableEngine::OnSurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    Engine::OnSurfaceDestroyed(holder);
    mLastSurfaceWidth = mLastSurfaceHeight = -1;
    return NOERROR;
}

ECode CImageWallpaper::DrawableEngine::OnSurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    Engine::OnSurfaceCreated(holder);
    mLastSurfaceWidth = mLastSurfaceHeight = -1;
    return NOERROR;
}

ECode CImageWallpaper::DrawableEngine::OnSurfaceRedrawNeeded(
    /* [in] */ ISurfaceHolder* holder)
{
    if (DEBUG) {
        Logger::D(TAG, "onSurfaceRedrawNeeded");
    }
    Engine::OnSurfaceRedrawNeeded(holder);

    DrawFrame();
    return NOERROR;
}

AutoPtr<IPoint> CImageWallpaper::DrawableEngine::GetDefaultDisplaySize()
{
    AutoPtr<IPoint> p;
    CPoint::New((IPoint**)&p);
    AutoPtr<IContext> c;
    mOwner->GetApplicationContext((IContext**)&c);
    AutoPtr<IInterface> obj;
    c->GetSystemService(IContext::WINDOW_SERVICE, (IInterface**)&obj);
    AutoPtr<IWindowManager> wm = IWindowManager::Probe(obj);
    AutoPtr<IDisplay> d;
    wm->GetDefaultDisplay((IDisplay**)&d);
    d->GetRealSize(p);
    return p;
}

void CImageWallpaper::DrawableEngine::DrawFrame()
{
    AutoPtr<IInterface> obj;
    ASSERT_SUCCEEDED(mOwner->GetSystemService(
        IContext::WINDOW_SERVICE, (IInterface**)&obj));
    AutoPtr<IWindowManager> wm = IWindowManager::Probe(obj);
    AutoPtr<IDisplay> display;
    ASSERT_SUCCEEDED(wm->GetDefaultDisplay((IDisplay**)&display));

    Int32 newRotation;
    display->GetRotation(&newRotation);

    // Sometimes a wallpaper is not large enough to cover the screen in one dimension.
    // Call updateSurfaceSize -- it will only actually do the update if the dimensions
    // should change
    AutoPtr<ISurfaceHolder> sh;
    GetSurfaceHolder((ISurfaceHolder**)&sh);
    if (newRotation != mLastRotation) {
        // Update surface size (if necessary)
        UpdateSurfaceSize(sh);
    }

    AutoPtr<IRect> frame;
    sh->GetSurfaceFrame((IRect**)&frame);
    Int32 dw, dh;
    frame->GetWidth(&dw);
    frame->GetHeight(&dh);

    Boolean surfaceDimensionsChanged = dw != mLastSurfaceWidth || dh != mLastSurfaceHeight;

    Boolean redrawNeeded = surfaceDimensionsChanged || newRotation != mLastRotation;
    if (!redrawNeeded && !mOffsetsChanged) {
        if (DEBUG) {
            Logger::D(TAG, "Suppressed drawFrame since redraw is not needed "
                "and offsets have not changed.");
        }
        return;
    }
    mLastRotation = newRotation;

    // Load bitmap if it is not yet loaded or if it was loaded at a different size
    if (mBackground == NULL || surfaceDimensionsChanged) {
        if (DEBUG) {
            Int32 w = 0, h = 0;
            if (mBackground != NULL) {
                mBackground->GetWidth(&w);
                mBackground->GetHeight(&h);
            }
            Logger::D(TAG, "Reloading bitmap: mBackground %p, bgw = %d,"
                " bgh = %d, dw = %d, dh = %d", mBackground.Get(), w, h, dw, dh);
        }
        mOwner->mWallpaperManager->ForgetLoadedWallpaper();
        UpdateWallpaperLocked();
        if (mBackground == NULL) {
            if (DEBUG) {
                Logger::D(TAG, "Unable to load bitmap");
            }
            return;
        }
        if (DEBUG) {
            Int32 w = 0, h = 0;
            mBackground->GetWidth(&w);
            mBackground->GetHeight(&h);
            if (dw != w || dh != h) {
                Logger::D(TAG, "Surface != bitmap dimensions: surface w/h,"
                    " bitmap w/h: %d, %d, %d, %d", dw, dh, w, h);
            }
        }
    }

    Int32 w = 0, h = 0;
    mBackground->GetWidth(&w);
    mBackground->GetHeight(&h);
    // Center the scaled image
    mScale = Elastos::Core::Math::Max(1.f, Elastos::Core::Math::Max(dw / (Float) w,
            dh / (Float) h));
    const Int32 availw = dw - (Int32) (w * mScale);
    const Int32 availh = dh - (Int32) (h * mScale);
    Int32 xPixels = availw / 2;
    Int32 yPixels = availh / 2;

    // Adjust the image for xOffset/yOffset values. If window manager is handling offsets,
    // mXOffset and mYOffset are set to 0.5f by default and therefore xPixels and yPixels
    // will remain unchanged
    const Int32 availwUnscaled = dw - w;
    const Int32 availhUnscaled = dh - h;
    if (availwUnscaled < 0) xPixels += (Int32)(availwUnscaled * (mXOffset - .5f) + .5f);
    if (availhUnscaled < 0) yPixels += (Int32)(availhUnscaled * (mYOffset - .5f) + .5f);

    mOffsetsChanged = FALSE;
    mRedrawNeeded = FALSE;
    if (surfaceDimensionsChanged) {
        mLastSurfaceWidth = dw;
        mLastSurfaceHeight = dh;
    }
    if (!redrawNeeded && xPixels == mLastXTranslation && yPixels == mLastYTranslation) {
        if (DEBUG) {
            Logger::D(TAG, "Suppressed drawFrame since the image has not "
                "actually moved an integral number of pixels.");
        }
        return;
    }
    mLastXTranslation = xPixels;
    mLastYTranslation = yPixels;

    if (DEBUG) {
        Logger::D(TAG, "Redrawing wallpaper");
    }

    if (mOwner->mIsHwAccelerated) {
        if (!DrawWallpaperWithOpenGL(sh, availw, availh, xPixels, yPixels)) {
            DrawWallpaperWithCanvas(sh, availw, availh, xPixels, yPixels);
        }
    }
    else {
        DrawWallpaperWithCanvas(sh, availw, availh, xPixels, yPixels);
        if (FIXED_SIZED_SURFACE) {
            // If the surface is fixed-size, we should only need to
            // draw it once and then we'll let the window manager
            // position it appropriately.  As such, we no longer needed
            // the loaded bitmap.  Yay!
            // hw-accelerated path retains bitmap for faster rotation
            mBackground = NULL;
            mOwner->mWallpaperManager->ForgetLoadedWallpaper();
        }
    }

}

void CImageWallpaper::DrawableEngine::UpdateWallpaperLocked()
{
    mBackground = NULL;
    mBackgroundWidth = -1;
    mBackgroundHeight = -1;
    ECode ec = mOwner->mWallpaperManager->GetBitmap((IBitmap**)&mBackground);
    mBackground->GetWidth(&mBackgroundWidth);
    mBackground->GetHeight(&mBackgroundHeight);

    if (FAILED(ec)) {
        mBackground = NULL;
        mBackgroundWidth = -1;
        mBackgroundHeight = -1;
        // Note that if we do fail at this, and the default wallpaper can't
        // be loaded, we will go into a cycle.  Don't do a build where the
        // default wallpaper can't be loaded.
        Logger::W(TAG, "Unable to load wallpaper!");
        //try {
        if (FAILED(mOwner->mWallpaperManager->Clear())) {
            // now we're really screwed.
            Logger::W(TAG, "Unable reset to default wallpaper!");
        }
    }
}

void CImageWallpaper::DrawableEngine::DrawWallpaperWithCanvas(
    /* [in] */ ISurfaceHolder* sh,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top)
{
    AutoPtr<ICanvas> c;
    sh->LockCanvas((ICanvas**)&c);
    if (c != NULL) {
        do
        {
            if (DEBUG) {
                Logger::D(TAG, "Redrawing: left=%d, top=%d", left, top);
            }

            Int32 value = 0;
            const Float right = left + (mBackground->GetWidth(&value), value) * mScale;
            const Float bottom = top + (mBackground->GetHeight(&value), value) * mScale;
            if (w < 0 || h < 0) {
                Int32 count;
                if (FAILED(c->Save(ICanvas::CLIP_SAVE_FLAG, &count)))
                    break;
                mBackground->GetWidth(&w);
                mBackground->GetHeight(&h);
                Boolean res;
                if (FAILED(c->ClipRect(left, top, right, bottom, RegionOp_DIFFERENCE, &res)))
                    break;
                if (FAILED(c->DrawColor(0xff000000)))
                    break;
                if (FAILED(c->Restore()))
                    break;
            }
            if (mBackground != NULL) {
                AutoPtr<IRectF> dest;
                CRectF::New(left, top, right, bottom, (IRectF**)&dest);
                // add a filter bitmap?
                if (FAILED(c->DrawBitmap(mBackground, NULL, dest, NULL)))
                    break;
            }
        } while (0);
        sh->UnlockCanvasAndPost(c);
    }
}

Boolean CImageWallpaper::DrawableEngine::DrawWallpaperWithOpenGL(
    /* [in] */ ISurfaceHolder* sh,
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top)
{
    Logger::I(TAG, " >> TODO DrawWallpaperWithOpenGL");
    return FALSE;

    if (!InitGL(sh)) {
        Logger::E(GL_LOG_TAG, "Failed to init opengl");
        return FALSE;
    }

    Int32 tw, th;
    mBackground->GetWidth(&tw);
    mBackground->GetHeight(&th);
    Float right = left + tw * mScale;
    Float bottom = top + th * mScale;

    AutoPtr<IRect> frame;
    sh->GetSurfaceFrame((IRect**)&frame);
    AutoPtr<IMatrix4f> ortho;
    CMatrix4f::New((IMatrix4f**)&ortho);
    frame->GetWidth(&tw); frame->GetHeight(&th);
    ortho->LoadOrtho(0.0f, tw, th, 0.0f, -1.0f, 1.0f);

    AutoPtr<IFloatBuffer> triangleVertices = CreateMesh(left, top, right, bottom);

    Int32 texture = LoadTexture(mBackground);
    Int32 program = BuildProgram(sSimpleVS, sSimpleFS);

    AutoPtr<IGLES20> gles20;
    CGLES20::AcquireSingleton((IGLES20**)&gles20);
    Int32 attribPosition, attribTexCoords, uniformTexture, uniformProjection;
    gles20->GlGetAttribLocation(program, String("position"), &attribPosition);
    gles20->GlGetAttribLocation(program, String("texCoords"), &attribTexCoords);
    gles20->GlGetUniformLocation(program, String("texture"), &uniformTexture);
    gles20->GlGetUniformLocation(program, String("projection"), &uniformProjection);

    CheckGlError();

    gles20->GlViewport(0, 0, tw, th);
    gles20->GlBindTexture(IGLES20::_GL_TEXTURE_2D, texture);

    gles20->GlUseProgram(program);
    gles20->GlEnableVertexAttribArray(attribPosition);
    gles20->GlEnableVertexAttribArray(attribTexCoords);
    gles20->GlUniform1i(uniformTexture, 0);
    AutoPtr<ArrayOf<Float> > oa;
    ortho->GetArray((ArrayOf<Float>**)&oa);
    gles20->GlUniformMatrix4fv(uniformProjection, 1, FALSE, oa, 0);

    CheckGlError();

    if (w > 0 || h > 0) {
        gles20->GlClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        gles20->GlClear(IGLES20::_GL_COLOR_BUFFER_BIT);
    }

    // drawQuad
    IBuffer* buf = IBuffer::Probe(triangleVertices);
    buf->SetPosition(TRIANGLE_VERTICES_DATA_POS_OFFSET);
    gles20->GlVertexAttribPointer(attribPosition, 3, IGLES20::_GL_FLOAT, FALSE,
            TRIANGLE_VERTICES_DATA_STRIDE_BYTES, buf);

    buf->SetPosition(TRIANGLE_VERTICES_DATA_UV_OFFSET);
    gles20->GlVertexAttribPointer(attribTexCoords, 3, IGLES20::_GL_FLOAT, FALSE,
            TRIANGLE_VERTICES_DATA_STRIDE_BYTES, buf);

    gles20->GlDrawArrays(IGLES20::_GL_TRIANGLE_STRIP, 0, 4);

    Boolean status;
    mEgl->EglSwapBuffers(mEglDisplay, mEglSurface, &status);
    CheckEglError();

    FinishGL();
    return status;
}

AutoPtr<IFloatBuffer> CImageWallpaper::DrawableEngine::CreateMesh(
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    AutoPtr<ArrayOf<Float> > verticesData = ArrayOf<Float>::Alloc(20);
    // X, Y, Z, U, V
    Int32 offset = 0;
    verticesData->Set(offset + 0, left);
    verticesData->Set(offset + 1, bottom);
    verticesData->Set(offset + 2, 0.0f);
    verticesData->Set(offset + 3, 0.0f);
    verticesData->Set(offset + 4, 1.0f);

    offset = 5;
    verticesData->Set(offset + 0, right);
    verticesData->Set(offset + 1, bottom);
    verticesData->Set(offset + 2, 0.0f);
    verticesData->Set(offset + 3, 1.0f);
    verticesData->Set(offset + 4, 1.0f);

    offset = 10;
    verticesData->Set(offset + 0, left);
    verticesData->Set(offset + 1, top);
    verticesData->Set(offset + 2, 0.0f);
    verticesData->Set(offset + 3, 0.0f);
    verticesData->Set(offset + 4, 0.0f);

    offset = 15;
    verticesData->Set(offset + 0, right);
    verticesData->Set(offset + 1, top);
    verticesData->Set(offset + 2, 0.0f);
    verticesData->Set(offset + 3, 1.0f);
    verticesData->Set(offset + 4, 0.0f);

    Int32 bytes = verticesData->GetLength() * FLOAT_SIZE_BYTES;

    AutoPtr<IByteBufferHelper> bbh;
    CByteBufferHelper::AcquireSingleton((IByteBufferHelper**)&bbh);
    AutoPtr<IByteOrderHelper> boh;
    CByteOrderHelper::AcquireSingleton((IByteOrderHelper**)&boh);
    ByteOrder bo;
    boh->GetNativeOrder(&bo);

    AutoPtr<IByteBuffer> bb;
    bbh->AllocateDirect(bytes, (IByteBuffer**)&bb);
    bb->SetOrder(bo);
    AutoPtr<IFloatBuffer> triangleVertices;
    bb->AsFloatBuffer((IFloatBuffer**)&triangleVertices);
    triangleVertices->Put(*verticesData);
    IBuffer::Probe(triangleVertices)->SetPosition(0);
    return triangleVertices;
}

Int32 CImageWallpaper::DrawableEngine::LoadTexture(
    /* [in] */ IBitmap* bitmap)
{
    AutoPtr<ArrayOf<Int32> > textures = ArrayOf<Int32>::Alloc(1);

    AutoPtr<IGLES20> gles20;
    CGLES20::AcquireSingleton((IGLES20**)&gles20);

    gles20->GlActiveTexture(IGLES20::_GL_TEXTURE0);
    gles20->GlGenTextures(1, textures, 0);
    CheckGlError();

    Int32 texture = (*textures)[0];
    gles20->GlBindTexture(IGLES20::_GL_TEXTURE_2D, texture);
    CheckGlError();

    gles20->GlTexParameteri(
        IGLES20::_GL_TEXTURE_2D, IGLES20::_GL_TEXTURE_MIN_FILTER, IGLES20::_GL_LINEAR);
    gles20->GlTexParameteri(
        IGLES20::_GL_TEXTURE_2D, IGLES20::_GL_TEXTURE_MAG_FILTER, IGLES20::_GL_LINEAR);

    gles20->GlTexParameteri(
        IGLES20::_GL_TEXTURE_2D, IGLES20::_GL_TEXTURE_WRAP_S, IGLES20::_GL_CLAMP_TO_EDGE);
    gles20->GlTexParameteri(
        IGLES20::_GL_TEXTURE_2D, IGLES20::_GL_TEXTURE_WRAP_T, IGLES20::_GL_CLAMP_TO_EDGE);

    AutoPtr<IGLUtils> glutils;
    CGLUtils::AcquireSingleton((IGLUtils**)&glutils);
    glutils->TexImage2D(
        IGLES20::_GL_TEXTURE_2D, 0, IGLES20::_GL_RGBA, bitmap, IGLES20::_GL_UNSIGNED_BYTE, 0);
    CheckGlError();

    return texture;
}

Int32 CImageWallpaper::DrawableEngine::BuildProgram(
    /* [in] */ const String& vertex,
    /* [in] */ const String& fragment)
{
    Int32 vertexShader = BuildShader(vertex, IGLES20::_GL_VERTEX_SHADER);
    if (vertexShader == 0) return 0;

    Int32 fragmentShader = BuildShader(fragment, IGLES20::_GL_FRAGMENT_SHADER);
    if (fragmentShader == 0) return 0;

    AutoPtr<IGLES20> gles20;
    CGLES20::AcquireSingleton((IGLES20**)&gles20);

    Int32 program = 0;
    gles20->GlCreateProgram(&program);
    gles20->GlAttachShader(program, vertexShader);
    CheckGlError();

    gles20->GlAttachShader(program, fragmentShader);
    CheckGlError();

    gles20->GlLinkProgram(program);
    CheckGlError();

    AutoPtr<ArrayOf<Int32> > status = ArrayOf<Int32>::Alloc(1);
    gles20->GlGetProgramiv(program, IGLES20::_GL_LINK_STATUS, status, 0);
    if ((*status)[0] != IGLES20::_GL_TRUE) {
        String error;
        gles20->GlGetProgramInfoLog(program, &error);
        Logger::D(GL_LOG_TAG, "Error while linking program:%s\n", error.string());
        gles20->GlDeleteShader(vertexShader);
        gles20->GlDeleteShader(fragmentShader);
        gles20->GlDeleteProgram(program);
        return 0;
    }

    return program;
}

Int32 CImageWallpaper::DrawableEngine::BuildShader(
    /* [in] */ const String& source,
    /* [in] */ Int32 type)
{
    AutoPtr<IGLES20> gles20;
    CGLES20::AcquireSingleton((IGLES20**)&gles20);

    Int32 shader = 0;
    gles20->GlCreateShader(type, &shader);

    gles20->GlShaderSource(shader, source);
    CheckGlError();

    gles20->GlCompileShader(shader);
    CheckGlError();

    AutoPtr<ArrayOf<Int32> > status = ArrayOf<Int32>::Alloc(1);
    gles20->GlGetShaderiv(shader, IGLES20::_GL_COMPILE_STATUS, status, 0);
    if ((*status)[0] != IGLES20::_GL_TRUE) {
        String error;
        gles20->GlGetShaderInfoLog(shader, &error);
        Logger::D(GL_LOG_TAG, "Error while compiling shader:%s\n", error.string());
        gles20->GlDeleteShader(shader);
        return 0;
    }

    return shader;
}

void CImageWallpaper::DrawableEngine::CheckEglError()
{
    Int32 error;
    mEgl->EglGetError(&error);
    if (error != IEGL14::EGL_SUCCESS) {
        AutoPtr<IGLUtils> glutils;
        CGLUtils::AcquireSingleton((IGLUtils**)&glutils);
        String str;
        glutils->GetEGLErrorString(error, &str);
        Logger::W(GL_LOG_TAG, "EGL error = %s", str.string());
    }
}

void CImageWallpaper::DrawableEngine::CheckGlError()
{
    AutoPtr<IGLES20> gles20;
    CGLES20::AcquireSingleton((IGLES20**)&gles20);

    Int32 error;
    gles20->GlGetError(&error);
    if (error != IGLES20::_GL_NO_ERROR) {
        Logger::W(GL_LOG_TAG, "GL error = 0x%08x", error);
    }
}

void CImageWallpaper::DrawableEngine::FinishGL()
{
    AutoPtr<IEGL10Helper> helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
    AutoPtr<IEGLSurface> EGL_NO_SURFACE;
    helper->GetNoSurface((IEGLSurface**)&EGL_NO_SURFACE);
    AutoPtr<IEGLContext> EGL_NO_CONTEXT;
    helper->GetNoContext((IEGLContext**)&EGL_NO_CONTEXT);

    Boolean bval;
    mEgl->EglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT, &bval);
    mEgl->EglDestroySurface(mEglDisplay, mEglSurface, &bval);
    mEgl->EglDestroyContext(mEglDisplay, mEglContext, &bval);
    mEgl->EglTerminate(mEglDisplay, &bval);
}

Boolean CImageWallpaper::DrawableEngine::InitGL(
    /* [in] */ ISurfaceHolder* surfaceHolder)
{
    AutoPtr<IGLES20> gles20;
    CGLES20::AcquireSingleton((IGLES20**)&gles20);
    AutoPtr<IGLUtils> glutils;
    CGLUtils::AcquireSingleton((IGLUtils**)&glutils);
    Int32 error;
    String str;
    Boolean bval;

    AutoPtr<IEGLContextHelper> ch;
    CEGLContextHelper::AcquireSingleton((IEGLContextHelper**)&ch);
    AutoPtr<IEGL> egl;
    ch->GetEGL((IEGL**)&egl);
    mEgl = IEGL10::Probe(egl);
    assert(mEgl != NULL);

    AutoPtr<IEGL10Helper> helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
    AutoPtr<IEGLDisplay> EGL_NO_DISPLAY;
    helper->GetNoDisplay((IEGLDisplay**)&EGL_NO_DISPLAY);
    AutoPtr<IEGLSurface> EGL_NO_SURFACE;
    helper->GetNoSurface((IEGLSurface**)&EGL_NO_SURFACE);
    AutoPtr<IInterface> EGL_DEFAULT_DISPLAY;
    helper->GetDefaultDisplay((IInterface**)&EGL_DEFAULT_DISPLAY);
    AutoPtr<IEGLContext> EGL_NO_CONTEXT;
    helper->GetNoContext((IEGLContext**)&EGL_NO_CONTEXT);

    mEgl->EglGetDisplay(EGL_DEFAULT_DISPLAY, (IEGLDisplay**)&mEglDisplay);
    if (mEglDisplay == NULL || mEglDisplay == EGL_NO_DISPLAY) {
        mEgl->EglGetError(&error);
        glutils->GetEGLErrorString(error, &str);
        Logger::W(GL_LOG_TAG, "eglGetDisplay failed  %s", str.string());
        return FALSE;
    }

    AutoPtr<ArrayOf<Int32> > version = ArrayOf<Int32>::Alloc(2);
    mEgl->EglInitialize(mEglDisplay, version, &bval);
    if (!bval) {
        mEgl->EglGetError(&error);
        glutils->GetEGLErrorString(error, &str);
        Logger::W(GL_LOG_TAG, "eglInitialize failed  %s", str.string());
        return FALSE;
    }

    mEglConfig = ChooseEglConfig();
    if (mEglConfig == NULL) {
        Logger::W(GL_LOG_TAG, "eglConfig not initialized");
        return FALSE;
    }

    mEglContext = CreateContext(mEgl, mEglDisplay, mEglConfig);
    if (mEglContext == NULL || mEglContext == EGL_NO_CONTEXT) {
        mEgl->EglGetError(&error);
        glutils->GetEGLErrorString(error, &str);
        Logger::W(GL_LOG_TAG, "createContext failed  %s", str.string());
        return FALSE;
    }

    AutoPtr<ArrayOf<Int32> > attribs = ArrayOf<Int32>::Alloc(5);
    attribs->Set(0, IEGL14::EGL_WIDTH); attribs->Set(1, 1);
    attribs->Set(2, IEGL14::EGL_HEIGHT); attribs->Set(3, 1);
    attribs->Set(4, IEGL14::EGL_NONE);

    AutoPtr<IEGLSurface> tmpSurface;
    mEgl->EglCreatePbufferSurface(mEglDisplay, mEglConfig, attribs, (IEGLSurface**)&tmpSurface);
    mEgl->EglMakeCurrent(mEglDisplay, tmpSurface, tmpSurface, mEglContext, &bval);

    AutoPtr<ArrayOf<Int32> > maxSize = ArrayOf<Int32>::Alloc(1);
    AutoPtr<IRect> frame;
    surfaceHolder->GetSurfaceFrame((IRect**)&frame);
    gles20->GlGetIntegerv(IGLES20::_GL_MAX_TEXTURE_SIZE, maxSize, 0);

    mEgl->EglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT, &bval);
    mEgl->EglDestroySurface(mEglDisplay, tmpSurface, &bval);

    Int32 fw, fh;
    frame->GetWidth(&fw);
    frame->GetHeight(&fh);
    if (fw > (*maxSize)[0] || fh > (*maxSize)[0]) {
        mEgl->EglDestroyContext(mEglDisplay, mEglContext, &bval);
        mEgl->EglTerminate(mEglDisplay, &bval);
        Logger::E(GL_LOG_TAG, "requested  texture size %d x %d  exceeds the support maximum of %d x %d.",
            fw, fh, (*maxSize)[0], (*maxSize)[0]);
        return FALSE;
    }

    mEgl->EglCreateWindowSurface(mEglDisplay, mEglConfig, surfaceHolder, NULL, (IEGLSurface**)&mEglSurface);
    if (mEglSurface == NULL || mEglSurface == EGL_NO_SURFACE) {
        mEgl->EglGetError(&error);
        glutils->GetEGLErrorString(error, &str);
        if (error == IEGL14::EGL_BAD_NATIVE_WINDOW || error == IEGL14::EGL_BAD_ALLOC) {
            Logger::E(GL_LOG_TAG, "createWindowSurface returned %s.", str.string());
            return FALSE;
        }
        Logger::E(GL_LOG_TAG, "createWindowSurface failed %s.", str.string());
        return FALSE;
    }

    mEgl->EglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext, &bval);
    if (!bval) {
        mEgl->EglGetError(&error);
        glutils->GetEGLErrorString(error, &str);
        Logger::E(GL_LOG_TAG, "eglMakeCurrent failed %s.", str.string());
        return FALSE;
    }

    return TRUE;
}

AutoPtr<IEGLContext> CImageWallpaper::DrawableEngine::CreateContext(
    /* [in] */ IEGL10* egl,
    /* [in] */ IEGLDisplay* eglDisplay,
    /* [in] */ IEGLConfig* eglConfig)
{
    AutoPtr<IEGL10Helper> helper;
    CEGL10Helper::AcquireSingleton((IEGL10Helper**)&helper);
    AutoPtr<IEGLContext> EGL_NO_CONTEXT;
    helper->GetNoContext((IEGLContext**)&EGL_NO_CONTEXT);

    AutoPtr<ArrayOf<Int32> > attrib_list = ArrayOf<Int32>::Alloc(3);
    attrib_list->Set(0, IEGL14::EGL_CONTEXT_CLIENT_VERSION);
    attrib_list->Set(1, 2);
    attrib_list->Set(2, IEGL14::EGL_NONE);
    AutoPtr<IEGLContext> ctx;
    ECode ec = egl->EglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, attrib_list, (IEGLContext**)&ctx);
    if (FAILED(ec)) {
        Logger::E(TAG, "EglCreateContext failed, ec=%08x", ec);
    }
    return ctx;
}

AutoPtr<IEGLConfig> CImageWallpaper::DrawableEngine::ChooseEglConfig()
{
    AutoPtr<ArrayOf<Int32> > configsCount = ArrayOf<Int32>::Alloc(1);
    AutoPtr<ArrayOf<IEGLConfig*> > configs = ArrayOf<IEGLConfig*>::Alloc(1);
    AutoPtr<ArrayOf<Int32> > configSpec = GetConfig();
    Boolean bval;
    mEgl->EglChooseConfig(mEglDisplay, configSpec, configs, 1, configsCount, &bval);
    if (!bval) {
        Int32 err;
        mEgl->EglGetError(&err);
        AutoPtr<IGLUtils> glutils;
        CGLUtils::AcquireSingleton((IGLUtils**)&glutils);
        String str;
        glutils->GetEGLErrorString(err, &str);

        Logger::E(TAG, "eglChooseConfig failed %s", str.string());
        return NULL;
    }
    else if ((*configsCount)[0] > 0) {
        return (*configs)[0];
    }
    Logger::I(TAG, " >>> No configs");
    return NULL;
}

AutoPtr<ArrayOf<Int32> > CImageWallpaper::DrawableEngine::GetConfig()
{
    AutoPtr<ArrayOf<Int32> > cfg = ArrayOf<Int32>::Alloc(17);
    cfg->Set(0, IEGL14::EGL_RENDERABLE_TYPE);   cfg->Set(1, IEGL14::EGL_OPENGL_ES2_BIT);
    cfg->Set(2, IEGL14::EGL_RED_SIZE);   cfg->Set(3, 8);
    cfg->Set(4, IEGL14::EGL_GREEN_SIZE);   cfg->Set(5, 8);
    cfg->Set(6, IEGL14::EGL_BLUE_SIZE);   cfg->Set(7, 8);
    cfg->Set(8, IEGL14::EGL_ALPHA_SIZE);   cfg->Set(9, 0);
    cfg->Set(10, IEGL14::EGL_DEPTH_SIZE);   cfg->Set(11, 0);
    cfg->Set(12, IEGL14::EGL_STENCIL_SIZE);   cfg->Set(13, 0);
    cfg->Set(14, IEGL14::EGL_CONFIG_CAVEAT);   cfg->Set(15, IEGL14::EGL_NONE);
    cfg->Set(16, IEGL14::EGL_NONE);
    return cfg;
}

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
