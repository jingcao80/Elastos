
#include "elastos/droid/systemui/CImageWallpaper.h"
#include "R.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::App::EIID_IService;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::EIID_IContextWrapper;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Graphics::CPoint;
using Elastos::Droid::Graphics::CRectF;
using Elastos::Droid::Graphics::IRectF;
using Elastos::Droid::Graphics::RegionOp_DIFFERENCE;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::View::IWindowManager;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {

const String CImageWallpaper::TAG("SystemUI");
const String CImageWallpaper::GL_LOG_TAG("ImageWallpaperGL");
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
    // if (FIXED_SIZED_SURFACE && USE_OPENGL) {
    //     if (!isEmulator()) {
    //         mIsHwAccelerated = ActivityManager.isHighEndGfx();
    //     }
    // }

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

// private static boolean isEmulator() {
//     return "1".equals(SystemProperties.get(PROPERTY_KERNEL_QEMU, "0"));
// }

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
        assert(0 && "TODO");
        // if (!drawWallpaperWithOpenGL(sh, availw, availh, xPixels, yPixels)) {
        //     drawWallpaperWithCanvas(sh, availw, availh, xPixels, yPixels);
        // }
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

// private boolean drawWallpaperWithOpenGL(SurfaceHolder sh, int w, int h, int left, int top) {
//     if (!initGL(sh)) return false;

//     final float right = left + mBackground.getWidth() * mScale;
//     final float bottom = top + mBackground.getHeight() * mScale;

//     final Rect frame = sh.getSurfaceFrame();
//     final Matrix4f ortho = new Matrix4f();
//     ortho.loadOrtho(0.0f, frame.width(), frame.height(), 0.0f, -1.0f, 1.0f);

//     final FloatBuffer triangleVertices = createMesh(left, top, right, bottom);

//     final int texture = loadTexture(mBackground);
//     final int program = buildProgram(sSimpleVS, sSimpleFS);

//     final int attribPosition = glGetAttribLocation(program, "position");
//     final int attribTexCoords = glGetAttribLocation(program, "texCoords");
//     final int uniformTexture = glGetUniformLocation(program, "texture");
//     final int uniformProjection = glGetUniformLocation(program, "projection");

//     checkGlError();

//     glViewport(0, 0, frame.width(), frame.height());
//     glBindTexture(GL_TEXTURE_2D, texture);

//     glUseProgram(program);
//     glEnableVertexAttribArray(attribPosition);
//     glEnableVertexAttribArray(attribTexCoords);
//     glUniform1i(uniformTexture, 0);
//     glUniformMatrix4fv(uniformProjection, 1, false, ortho.getArray(), 0);

//     checkGlError();

//     if (w > 0 || h > 0) {
//         glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//         glClear(GL_COLOR_BUFFER_BIT);
//     }

//     // drawQuad
//     triangleVertices.position(TRIANGLE_VERTICES_DATA_POS_OFFSET);
//     glVertexAttribPointer(attribPosition, 3, GL_FLOAT, false,
//             TRIANGLE_VERTICES_DATA_STRIDE_BYTES, triangleVertices);

//     triangleVertices.position(TRIANGLE_VERTICES_DATA_UV_OFFSET);
//     glVertexAttribPointer(attribTexCoords, 3, GL_FLOAT, false,
//             TRIANGLE_VERTICES_DATA_STRIDE_BYTES, triangleVertices);

//     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//     boolean status = mEgl.eglSwapBuffers(mEglDisplay, mEglSurface);
//     checkEglError();

//     finishGL();

//     return status;
// }

// private FloatBuffer createMesh(Int32 left, Int32 top, Float right, Float bottom) {
//     final Float[] verticesData = {
//             // X, Y, Z, U, V
//              left,  bottom, 0.0f, 0.0f, 1.0f,
//              right, bottom, 0.0f, 1.0f, 1.0f,
//              left,  top,    0.0f, 0.0f, 0.0f,
//              right, top,    0.0f, 1.0f, 0.0f,
//     };

//     final Int32 bytes = verticesData.length * FLOAT_SIZE_BYTES;
//     final FloatBuffer triangleVertices = ByteBuffer.allocateDirect(bytes).order(
//             ByteOrder.nativeOrder()).asFloatBuffer();
//     triangleVertices.put(verticesData).position(0);
//     return triangleVertices;
// }

// private Int32 loadTexture(Bitmap bitmap) {
//     Int32[] textures = new Int32[1];

//     glActiveTexture(GL_TEXTURE0);
//     glGenTextures(1, textures, 0);
//     checkGlError();

//     Int32 texture = textures[0];
//     glBindTexture(GL_TEXTURE_2D, texture);
//     checkGlError();

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

//     GLUtils.texImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bitmap, GL_UNSIGNED_BYTE, 0);
//     checkGlError();

//     return texture;
// }

// private Int32 buildProgram(String vertex, String fragment) {
//     Int32 vertexShader = buildShader(vertex, GL_VERTEX_SHADER);
//     if (vertexShader == 0) return 0;

//     Int32 fragmentShader = buildShader(fragment, GL_FRAGMENT_SHADER);
//     if (fragmentShader == 0) return 0;

//     Int32 program = glCreateProgram();
//     glAttachShader(program, vertexShader);
//     checkGlError();

//     glAttachShader(program, fragmentShader);
//     checkGlError();

//     glLinkProgram(program);
//     checkGlError();

//     Int32[] status = new Int32[1];
//     glGetProgramiv(program, GL_LINK_STATUS, status, 0);
//     if (status[0] != GL_TRUE) {
//         String error = glGetProgramInfoLog(program);
//         Logger::D(GL_LOG_TAG, "Error while linking program:\n" + error);
//         glDeleteShader(vertexShader);
//         glDeleteShader(fragmentShader);
//         glDeleteProgram(program);
//         return 0;
//     }

//     return program;
// }

// private Int32 buildShader(String source, Int32 type) {
//     Int32 shader = glCreateShader(type);

//     glShaderSource(shader, source);
//     checkGlError();

//     glCompileShader(shader);
//     checkGlError();

//     Int32[] status = new Int32[1];
//     glGetShaderiv(shader, GL_COMPILE_STATUS, status, 0);
//     if (status[0] != GL_TRUE) {
//         String error = glGetShaderInfoLog(shader);
//         Logger::D(GL_LOG_TAG, "Error while compiling shader:\n" + error);
//         glDeleteShader(shader);
//         return 0;
//     }

//     return shader;
// }

// private void checkEglError() {
//     Int32 error = mEgl.eglGetError();
//     if (error != EGL_SUCCESS) {
//         Logger::W(GL_LOG_TAG, "EGL error = " + GLUtils.getEGLErrorString(error));
//     }
// }

// private void checkGlError() {
//     Int32 error = glGetError();
//     if (error != GL_NO_ERROR) {
//         Logger::W(GL_LOG_TAG, "GL error = 0x" + Integer.toHexString(error), new Throwable());
//     }
// }

// private void finishGL() {
//     mEgl.eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
//     mEgl.eglDestroySurface(mEglDisplay, mEglSurface);
//     mEgl.eglDestroyContext(mEglDisplay, mEglContext);
//     mEgl.eglTerminate(mEglDisplay);
// }

// private boolean initGL(SurfaceHolder surfaceHolder) {
//     mEgl = (EGL10) EGLContext.getEGL();

//     mEglDisplay = mEgl.eglGetDisplay(EGL_DEFAULT_DISPLAY);
//     if (mEglDisplay == EGL_NO_DISPLAY) {
//         throw new RuntimeException("eglGetDisplay failed " +
//                 GLUtils.getEGLErrorString(mEgl.eglGetError()));
//     }

//     int[] version = new int[2];
//     if (!mEgl.eglInitialize(mEglDisplay, version)) {
//         throw new RuntimeException("eglInitialize failed " +
//                 GLUtils.getEGLErrorString(mEgl.eglGetError()));
//     }

//     mEglConfig = chooseEglConfig();
//     if (mEglConfig == null) {
//         throw new RuntimeException("eglConfig not initialized");
//     }

//     mEglContext = createContext(mEgl, mEglDisplay, mEglConfig);
//     if (mEglContext == EGL_NO_CONTEXT) {
//         throw new RuntimeException("createContext failed " +
//                 GLUtils.getEGLErrorString(mEgl.eglGetError()));
//     }

//     int attribs[] = {
//         EGL_WIDTH, 1,
//         EGL_HEIGHT, 1,
//         EGL_NONE
//     };
//     EGLSurface tmpSurface = mEgl.eglCreatePbufferSurface(mEglDisplay, mEglConfig, attribs);
//     mEgl.eglMakeCurrent(mEglDisplay, tmpSurface, tmpSurface, mEglContext);

//     int[] maxSize = new int[1];
//     Rect frame = surfaceHolder.getSurfaceFrame();
//     glGetIntegerv(GL_MAX_TEXTURE_SIZE, maxSize, 0);

//     mEgl.eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
//     mEgl.eglDestroySurface(mEglDisplay, tmpSurface);

//     if(frame.width() > maxSize[0] || frame.height() > maxSize[0]) {
//         mEgl.eglDestroyContext(mEglDisplay, mEglContext);
//         mEgl.eglTerminate(mEglDisplay);
//         Log.e(GL_LOG_TAG, "requested  texture size " +
//             frame.width() + "x" + frame.height() + " exceeds the support maximum of " +
//             maxSize[0] + "x" + maxSize[0]);
//         return false;
//     }

//     mEglSurface = mEgl.eglCreateWindowSurface(mEglDisplay, mEglConfig, surfaceHolder, null);
//     if (mEglSurface == null || mEglSurface == EGL_NO_SURFACE) {
//         int error = mEgl.eglGetError();
//         if (error == EGL_BAD_NATIVE_WINDOW || error == EGL_BAD_ALLOC) {
//             Log.e(GL_LOG_TAG, "createWindowSurface returned " +
//                                  GLUtils.getEGLErrorString(error) + ".");
//             return false;
//         }
//         throw new RuntimeException("createWindowSurface failed " +
//                 GLUtils.getEGLErrorString(error));
//     }

//     if (!mEgl.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
//         throw new RuntimeException("eglMakeCurrent failed " +
//                 GLUtils.getEGLErrorString(mEgl.eglGetError()));
//     }

//     return true;
// }

// EGLContext createContext(EGL10 egl, EGLDisplay eglDisplay, EGLConfig eglConfig) {
//     Int32[] attrib_list = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
//     return egl.eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, attrib_list);
// }

// private EGLConfig chooseEglConfig() {
//     Int32[] configsCount = new Int32[1];
//     EGLConfig[] configs = new EGLConfig[1];
//     Int32[] configSpec = getConfig();
//     if (!mEgl.eglChooseConfig(mEglDisplay, configSpec, configs, 1, configsCount)) {
//         throw new IllegalArgumentException("eglChooseConfig failed " +
//                 GLUtils.getEGLErrorString(mEgl.eglGetError()));
//     } else if (configsCount[0] > 0) {
//         return configs[0];
//     }
//     return NULL;
// }

// private Int32[] getConfig() {
//     return new Int32[] {
//             EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
//             EGL_RED_SIZE, 8,
//             EGL_GREEN_SIZE, 8,
//             EGL_BLUE_SIZE, 8,
//             EGL_ALPHA_SIZE, 0,
//             EGL_DEPTH_SIZE, 0,
//             EGL_STENCIL_SIZE, 0,
//             EGL_CONFIG_CAVEAT, EGL_NONE,
//             EGL_NONE
//     };
// }

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
