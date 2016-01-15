
#include "CImageWallpaper.h"
#include "R.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CStringWrapper;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::App::EIID_IService;
using Elastos::Droid::Content::EIID_IContext;
using Elastos::Droid::Content::EIID_IContextWrapper;
using Elastos::Droid::Content::EIID_IComponentCallbacks2;
using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Graphics::RegionOp_DIFFERENCE;
using Elastos::Droid::Graphics::IPixelFormat;


namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace SystemUI {

const String CImageWallpaper::TAG("SystemUI");
const String CImageWallpaper::GL_LOG_TAG("ImageWallpaperGL");
const Boolean CImageWallpaper::DEBUG = FALSE;
const String CImageWallpaper::PROPERTY_KERNEL_QEMU("ro.kernel.qemu");

const Boolean CImageWallpaper::FIXED_SIZED_SURFACE;
const Boolean CImageWallpaper::USE_OPENGL;


CImageWallpaper::CImageWallpaper()
    : mIsHwAccelerated(FALSE)
{
}

PInterface CImageWallpaper::Probe(
    /* [in] */ REIID riid)
{
    return _CImageWallpaper::Probe(riid);
}

UInt32 CImageWallpaper::AddRef()
{
    return _CImageWallpaper::AddRef();
}

UInt32 CImageWallpaper::Release()
{
    return _CImageWallpaper::Release();
}

ECode CImageWallpaper::OnCreate()
{
    WallpaperService::OnCreate();

    GetSystemService(WALLPAPER_SERVICE, (IInterface**)&mWallpaperManager);

    //noinspection PointlessBooleanExpression,ConstantConditions
    // if (FIXED_SIZED_SURFACE && USE_OPENGL) {
    //     if (!isEmulator()) {
    //         mIsHwAccelerated = ActivityManager.isHighEndGfx();
    //     }
    // }

    return NOERROR;
}

ECode CImageWallpaper::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    return WallpaperService::OnBind(intent, binder);
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

AutoPtr<WallpaperService::Engine> CImageWallpaper::OnCreateEngine()
{
    mEngine = new DrawableEngine(this);
    return mEngine;
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

    AutoLock lock(mOwner->mLock);
    mOwner->mLastSurfaceWidth = mOwner->mLastSurfaceHeight = -1;
    mOwner->mBackground = NULL;
    mOwner->mRedrawNeeded = TRUE;
    mOwner->DrawFrameLocked();

    return NOERROR;
}

const Int32 CImageWallpaper::DrawableEngine::EGL_CONTEXT_CLIENT_VERSION;
const Int32 CImageWallpaper::DrawableEngine::EGL_OPENGL_ES2_BIT;
const Int32 CImageWallpaper::DrawableEngine::FLOAT_SIZE_BYTES;
const Int32 CImageWallpaper::DrawableEngine::TRIANGLE_VERTICES_DATA_STRIDE_BYTES;
const Int32 CImageWallpaper::DrawableEngine::TRIANGLE_VERTICES_DATA_POS_OFFSET;
const Int32 CImageWallpaper::DrawableEngine::TRIANGLE_VERTICES_DATA_UV_OFFSET;

CImageWallpaper::DrawableEngine::DrawableEngine(
    /* [in] */ CImageWallpaper* owner)
    : Engine(owner)
    , mLastSurfaceWidth(-1)
    , mLastSurfaceHeight(-1)
    , mLastRotation(-1)
    , mXOffset(0.0)
    , mYOffset(0.0)
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
        mOwner->mWallpaperManager->ForgetLoadedWallpaper();
    }
}

//@Override
void CImageWallpaper::DrawableEngine::OnCreate(
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
}

//@Override
void CImageWallpaper::DrawableEngine::OnDestroy()
{
    Engine::OnDestroy();
    if (mReceiver != NULL) {
        mOwner->UnregisterReceiver(mReceiver);
    }
}

//@Override
void CImageWallpaper::DrawableEngine::OnDesiredSizeChanged(
    /* [in] */ Int32 desiredWidth,
    /* [in] */ Int32 desiredHeight)
{
    Engine::OnDesiredSizeChanged(desiredWidth, desiredHeight);
    AutoPtr<ISurfaceHolder> surfaceHolder = GetSurfaceHolder();
    if (surfaceHolder != NULL) {
        UpdateSurfaceSize(surfaceHolder);
    }
}

void CImageWallpaper::DrawableEngine::UpdateSurfaceSize(
    /* [in] */ ISurfaceHolder* surfaceHolder)
{
    if (FIXED_SIZED_SURFACE) {
        // Used a fixed size surface, because we are special.  We can do
        // this because we know the current design of window animations doesn't
        // cause this to break.
        surfaceHolder->SetFixedSize(GetDesiredMinimumWidth(), GetDesiredMinimumHeight());
    }
    else {
        surfaceHolder->SetSizeFromLayout();
    }
}

//@Override
void CImageWallpaper::DrawableEngine::OnVisibilityChanged(
    /* [in] */ Boolean visible)
{
    if (DEBUG) {
        Logger::D(TAG, "onVisibilityChanged: mVisible, visible=%d, %d", mVisible, visible);
    }

    AutoLock lock(&mLock);
    if (mVisible != visible) {
        if (DEBUG) {
            Logger::D(TAG, "Visibility changed to visible=%d", visible);
        }
        mVisible = visible;
        DrawFrameLocked();
    }
}

//@Override
void CImageWallpaper::DrawableEngine::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    Engine::OnTouchEvent(event);
}

//@Override
void CImageWallpaper::DrawableEngine::OnOffsetsChanged(
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

    AutoLock lock(&mLock);
    if (mXOffset != xOffset || mYOffset != yOffset) {
        if (DEBUG) {
            Logger::D(TAG, "Offsets changed to (%f,%f).", xOffset, yOffset);
        }
        mXOffset = xOffset;
        mYOffset = yOffset;
        mOffsetsChanged = TRUE;
    }
    DrawFrameLocked();
}

//@Override
void CImageWallpaper::DrawableEngine::OnSurfaceChanged(
    /* [in] */ ISurfaceHolder* holder,
    /* [in] */ Int32 format,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (DEBUG) {
        Logger::D(TAG, "onSurfaceChanged: width=%d, height=%d", width, height);
    }

    Engine::OnSurfaceChanged(holder, format, width, height);

    AutoLock lock(&mLock);
    DrawFrameLocked();
}

//@Override
void CImageWallpaper::DrawableEngine::OnSurfaceDestroyed(
    /* [in] */ ISurfaceHolder* holder)
{
    Engine::OnSurfaceDestroyed(holder);
    mLastSurfaceWidth = mLastSurfaceHeight = -1;
}

//@Override
void CImageWallpaper::DrawableEngine::OnSurfaceCreated(
    /* [in] */ ISurfaceHolder* holder)
{
    Engine::OnSurfaceCreated(holder);
    mLastSurfaceWidth = mLastSurfaceHeight = -1;
}

//@Override
void CImageWallpaper::DrawableEngine::OnSurfaceRedrawNeeded(
    /* [in] */ ISurfaceHolder* holder)
{
    if (DEBUG) {
        Logger::D(TAG, "onSurfaceRedrawNeeded");
    }
    Engine::OnSurfaceRedrawNeeded(holder);

    AutoLock lock(&mLock);
    DrawFrameLocked();
}

void CImageWallpaper::DrawableEngine::DrawFrameLocked()
{
    AutoPtr<ISurfaceHolder> sh = GetSurfaceHolder();
    AutoPtr<IRect> frame;
    sh->GetSurfaceFrame((IRect**)&frame);
    Int32 dw, dh;
    frame->GetWidth(&dw);
    frame->GetHeight(&dh);
    AutoPtr<IWindowManager> wm;
    ASSERT_SUCCEEDED(mOwner->GetSystemService(
        IContext::WINDOW_SERVICE, (IInterface**)&wm));
    AutoPtr<IDisplay> display;
    ASSERT_SUCCEEDED(wm->GetDefaultDisplay((IDisplay**)&display));

    Int32 newRotation;
    display->GetRotation(&newRotation);
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
    const Int32 availw = dw - w;
    const Int32 availh = dh - h;
    Int32 xPixels = availw < 0 ? (Int32)(availw * mXOffset + .5f) : (availw / 2);
    Int32 yPixels = availh < 0 ? (Int32)(availh * mYOffset + .5f) : (availh / 2);

    mOffsetsChanged = FALSE;
    mRedrawNeeded = FALSE;
    if (surfaceDimensionsChanged) {
        mLastSurfaceWidth = dw;
        mLastSurfaceHeight = dh;
    }
    mLastXTranslation = xPixels;
    mLastYTranslation = yPixels;
    if (!redrawNeeded && xPixels == mLastXTranslation && yPixels == mLastYTranslation) {
        if (DEBUG) {
            Logger::D(TAG, "Suppressed drawFrame since the image has not "
                "actually moved an integral number of pixels.");
        }
        return;
    }

    if (DEBUG) {
        Logger::D(TAG, "Redrawing wallpaper");
    }
    if (mOwner->mIsHwAccelerated) {
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
    //Throwable exception = NULL;
    ECode ec;
    do {
        ec = mOwner->mWallpaperManager->ForgetLoadedWallpaper(); // force reload
        if (FAILED(ec))
            break;
        mBackground = NULL;
        ec = mOwner->mWallpaperManager->GetBitmap((IBitmap**)&mBackground);
    } while (0);
    // } catch (RuntimeException e) {
    //     exception = e;
    // } catch (OutOfMemoryError e) {
    //     exception = e;
    // }

    if (FAILED(ec)) {
        mBackground = NULL;
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
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    AutoPtr<ICanvas> c;
    sh->LockCanvas((ICanvas**)&c);
    if (c != NULL) {
        do
        {
            if (DEBUG) {
                Logger::D(TAG, "Redrawing: x=%d, y=%d", x, y);
            }

            if (FAILED(c->Translate(x, y)))
                break;
            if (w < 0 || h < 0) {
                Int32 count;
                if (FAILED(c->Save(ICanvas::CLIP_SAVE_FLAG, &count)))
                    break;
                mBackground->GetWidth(&w);
                mBackground->GetHeight(&h);
                Boolean res;
                if (FAILED(c->ClipRect(0, 0, w, h, RegionOp_DIFFERENCE, &res)))
                    break;
                if (FAILED(c->DrawColor(0xff000000)))
                    break;
                if (FAILED(c->Restore()))
                    break;
            }
            if (mBackground != NULL) {
                if (FAILED(c->DrawBitmap(mBackground, 0, 0, NULL)))
                    break;
            }
        } while (0);
        sh->UnlockCanvasAndPost(c);
    }
}

// private Boolean drawWallpaperWithOpenGL(SurfaceHolder sh, Int32 w, Int32 h, Int32 left, Int32 top) {
//     if (!initGL(sh)) return FALSE;

//     final Float right = left + mBackground.getWidth();
//     final Float bottom = top + mBackground.getHeight();

//     final Rect frame = sh.getSurfaceFrame();
//     final Matrix4f ortho = new Matrix4f();
//     ortho.loadOrtho(0.0f, frame.width(), frame.height(), 0.0f, -1.0f, 1.0f);

//     final FloatBuffer triangleVertices = createMesh(left, top, right, bottom);

//     final Int32 texture = loadTexture(mBackground);
//     final Int32 program = buildProgram(sSimpleVS, sSimpleFS);

//     final Int32 attribPosition = glGetAttribLocation(program, "position");
//     final Int32 attribTexCoords = glGetAttribLocation(program, "texCoords");
//     final Int32 uniformTexture = glGetUniformLocation(program, "texture");
//     final Int32 uniformProjection = glGetUniformLocation(program, "projection");

//     checkGlError();

//     glViewport(0, 0, frame.width(), frame.height());
//     glBindTexture(GL_TEXTURE_2D, texture);

//     glUseProgram(program);
//     glEnableVertexAttribArray(attribPosition);
//     glEnableVertexAttribArray(attribTexCoords);
//     glUniform1i(uniformTexture, 0);
//     glUniformMatrix4fv(uniformProjection, 1, FALSE, ortho.getArray(), 0);

//     checkGlError();

//     if (w < 0 || h < 0) {
//         glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
//         glClear(GL_COLOR_BUFFER_BIT);
//     }

//     // drawQuad
//     triangleVertices.position(TRIANGLE_VERTICES_DATA_POS_OFFSET);
//     glVertexAttribPointer(attribPosition, 3, GL_FLOAT, FALSE,
//             TRIANGLE_VERTICES_DATA_STRIDE_BYTES, triangleVertices);

//     triangleVertices.position(TRIANGLE_VERTICES_DATA_UV_OFFSET);
//     glVertexAttribPointer(attribTexCoords, 3, GL_FLOAT, FALSE,
//             TRIANGLE_VERTICES_DATA_STRIDE_BYTES, triangleVertices);

//     glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

//     if (!mEgl.eglSwapBuffers(mEglDisplay, mEglSurface)) {
//         throw new RuntimeException("Cannot swap buffers");
//     }
//     checkEglError();

//     finishGL();

//     return TRUE;
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
// }

// private Boolean initGL(SurfaceHolder surfaceHolder) {
//     mEgl = (EGL10) EGLContext.getEGL();

//     mEglDisplay = mEgl.eglGetDisplay(EGL_DEFAULT_DISPLAY);
//     if (mEglDisplay == EGL_NO_DISPLAY) {
//         throw new RuntimeException("eglGetDisplay failed " +
//                 GLUtils.getEGLErrorString(mEgl.eglGetError()));
//     }

//     Int32[] version = new Int32[2];
//     if (!mEgl.eglInitialize(mEglDisplay, version)) {
//         throw new RuntimeException("eglInitialize failed " +
//                 GLUtils.getEGLErrorString(mEgl.eglGetError()));
//     }

//     mEglConfig = chooseEglConfig();
//     if (mEglConfig == NULL) {
//         throw new RuntimeException("eglConfig not initialized");
//     }

//     mEglContext = createContext(mEgl, mEglDisplay, mEglConfig);

//     mEglSurface = mEgl.eglCreateWindowSurface(mEglDisplay, mEglConfig, surfaceHolder, NULL);

//     if (mEglSurface == NULL || mEglSurface == EGL_NO_SURFACE) {
//         Int32 error = mEgl.eglGetError();
//         if (error == EGL_BAD_NATIVE_WINDOW) {
//             Logger::E(GL_LOG_TAG, "createWindowSurface returned EGL_BAD_NATIVE_WINDOW.");
//             return FALSE;
//         }
//         throw new RuntimeException("createWindowSurface failed " +
//                 GLUtils.getEGLErrorString(error));
//     }

//     if (!mEgl.eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext)) {
//         throw new RuntimeException("eglMakeCurrent failed " +
//                 GLUtils.getEGLErrorString(mEgl.eglGetError()));
//     }

//     mGL = mEglContext.getGL();

//     return TRUE;
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

ECode CImageWallpaper::GetAssets(
    /* [out] */ IAssetManager** assetManager)
{
    VALIDATE_NOT_NULL(assetManager);
    return WallpaperService::GetAssets(assetManager);
}

ECode CImageWallpaper::GetResources(
    /* [out] */ IResources** resources)
{
    VALIDATE_NOT_NULL(resources)
    return WallpaperService::GetResources(resources);
}

ECode CImageWallpaper::GetPackageManager(
    /* [out] */ IPackageManager** packageManager)
{
    VALIDATE_NOT_NULL(packageManager);
    return WallpaperService::GetPackageManager(packageManager);
}

ECode CImageWallpaper::GetContentResolver(
    /* [out] */ IContentResolver** resolver)
{
    VALIDATE_NOT_NULL(resolver);
    return WallpaperService::GetContentResolver(resolver);
}

ECode CImageWallpaper::GetApplicationContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);
    return WallpaperService::GetApplicationContext(ctx);
}

ECode CImageWallpaper::GetText(
    /* [in] */ Int32 resId,
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    return WallpaperService::GetText(resId, text);
}

ECode CImageWallpaper::GetString(
    /* [in] */ Int32 resId,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    return WallpaperService::GetString(resId, str);
}

ECode CImageWallpaper::SetTheme(
    /* [in] */ Int32 resid)
{
    return WallpaperService::SetTheme(resid);
}

ECode CImageWallpaper::GetTheme(
    /* [out] */ IResourcesTheme** theme)
{
    VALIDATE_NOT_NULL(theme);
    return WallpaperService::GetTheme(theme);
}

ECode CImageWallpaper::ObtainStyledAttributes(
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles);
    return WallpaperService::ObtainStyledAttributes(attrs, styles);
}

ECode CImageWallpaper::ObtainStyledAttributes(
    /* [in] */ Int32 resid,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles);
    return WallpaperService::ObtainStyledAttributes(resid, attrs, styles);
}

ECode CImageWallpaper::ObtainStyledAttributes(
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles);
    return WallpaperService::ObtainStyledAttributes(set, attrs, styles);
}

ECode CImageWallpaper::ObtainStyledAttributes(
    /* [in] */ IAttributeSet* set,
    /* [in] */ ArrayOf<Int32>* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes,
    /* [out] */ ITypedArray** styles)
{
    VALIDATE_NOT_NULL(styles);
    return WallpaperService::ObtainStyledAttributes(set, attrs, defStyleAttr, defStyleRes, styles);
}

ECode CImageWallpaper::GetClassLoader(
    /* [out] */ IClassLoader** loader)
{
    VALIDATE_NOT_NULL(loader);
    return WallpaperService::GetClassLoader(loader);
}

ECode CImageWallpaper::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    return WallpaperService::GetPackageName(packageName);
}

ECode CImageWallpaper::GetApplicationInfo(
    /* [out] */ IApplicationInfo** info)
{
    VALIDATE_NOT_NULL(info);
    return WallpaperService::GetApplicationInfo(info);
}

ECode CImageWallpaper::GetPackageResourcePath(
    /* [out] */ String* path)
{
    VALIDATE_NOT_NULL(path);
    return WallpaperService::GetPackageResourcePath(path);
}

ECode CImageWallpaper::GetExternalFilesDir(
    /* [in] */ const String& type,
    /* [out] */ IFile** filesDir)
{
    VALIDATE_NOT_NULL(filesDir);
    return WallpaperService::GetExternalFilesDir(type, filesDir);
}

ECode CImageWallpaper::GetCacheDir(
    /* [out] */ IFile** cacheDir)
{
    VALIDATE_NOT_NULL(cacheDir);
    return WallpaperService::GetCacheDir(cacheDir);
}

ECode CImageWallpaper::GetDir(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */ IFile** dir)
{
    VALIDATE_NOT_NULL(dir);
    return WallpaperService::GetDir(name, mode, dir);
}

ECode CImageWallpaper::StartActivity(
    /* [in] */ IIntent* intent)
{
    return WallpaperService::StartActivity(intent);
}

ECode CImageWallpaper::StartIntentSender(
    /* [in] */ IIntentSender* intent,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags)
{
    return WallpaperService::StartIntentSender(intent, fillInIntent,
        flagsMask, flagsValues, extraFlags);
}

ECode CImageWallpaper::SendBroadcast(
    /* [in] */ IIntent* intent)
{
    return WallpaperService::SendBroadcast(intent);
}

ECode CImageWallpaper::StartService(
    /* [in] */ IIntent* service,
    /* [out] */ IComponentName** name)
{
    VALIDATE_NOT_NULL(name);
    return WallpaperService::StartService(service, name);
}

ECode CImageWallpaper::StopService(
    /* [in] */ IIntent* service,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    return WallpaperService::StopService(service, succeeded);
}

ECode CImageWallpaper::BindService(
    /* [in] */ IIntent* service,
    /* [in] */ IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    return WallpaperService::BindService(service, conn, flags, succeeded);
}

ECode CImageWallpaper::UnbindService(
    /* [in] */ IServiceConnection* conn)
{
    return WallpaperService::UnbindService(conn);
}

ECode CImageWallpaper::GetSystemService(
    /* [in] */ const String& name,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object);
    return WallpaperService::GetSystemService(name, object);
}

ECode CImageWallpaper::CreatePackageContext(
    /* [in] */ const String& capsuleName,
    /* [in] */ Int32 flags,
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);
    return WallpaperService::CreatePackageContext(capsuleName, flags, ctx);
}

ECode CImageWallpaper::CheckCallingPermission(
    /* [in] */ const String& permission,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    return WallpaperService::CheckCallingPermission(permission, value);
}

ECode CImageWallpaper::EnforceCallingOrSelfPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& message)
{
    return WallpaperService::EnforceCallingOrSelfPermission(permission, message);
}

ECode CImageWallpaper::RevokeUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags)
{
    return WallpaperService::RevokeUriPermission(uri, modeFlags);
}

ECode CImageWallpaper::CheckCallingOrSelfPermission(
    /* [in] */ const String& permission,
    /* [out] */ Int32* perm)
{
    VALIDATE_NOT_NULL(perm);
    return WallpaperService::CheckCallingOrSelfPermission(permission, perm);
}

ECode CImageWallpaper::CheckPermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [out] */ Int32 * result)
{
    return WallpaperService::CheckPermission(permission, pid, uid, result);
}

ECode CImageWallpaper::CheckUriPermission(
    /* [in] */ IUri * uri,
    /* [in] */ const String& readPermission,
    /* [in] */ const String& writePermission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32 * result)
{
    return WallpaperService::CheckUriPermission(
            uri,
            readPermission,
            writePermission,
            pid,
            uid,
            modeFlags,
            result);
}

ECode CImageWallpaper::CheckUriPermission(
    /* [in] */ IUri * uri,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32 * result)
{
    return WallpaperService::CheckUriPermission(
            uri,
            pid,
            uid,
            modeFlags,
            result);
}

ECode CImageWallpaper::GrantUriPermission(
    /* [in] */ const String& toCapsule,
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags)
{
    return WallpaperService::GrantUriPermission(toCapsule, uri, modeFlags);
}

ECode CImageWallpaper::GetBaseContext(
    /* [out] */ IContext** ctx)
{
    VALIDATE_NOT_NULL(ctx);
    return WallpaperService::GetBaseContext(ctx);
}

ECode CImageWallpaper::Attach(
    /* [in] */ IContext* ctx,
    /* [in] */ IActivityThread* apartment,
    /* [in] */ const String& className,
    /* [in] */ IBinder* token,
    /* [in] */ IApplication* application,
    /* [in] */ IIActivityManager* activityManager)
{
    return WallpaperService::Attach(ctx, apartment, className, token, application, activityManager);
}

ECode CImageWallpaper::OnStart(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 startId)
{
    return WallpaperService::OnStart(intent, startId);
}

ECode CImageWallpaper::OnStartCommand(
    /* [in] */ IIntent * intent,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 startId,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return WallpaperService::OnStartCommand(intent, flags, startId, result);
}

ECode CImageWallpaper::OnDestroy()
{
    return WallpaperService::OnDestroy();
}

ECode CImageWallpaper::OnUnbind(
    /* [in] */ IIntent* intent,
    /* [out] */ Boolean* succeeded)
{
    VALIDATE_NOT_NULL(succeeded);
    return WallpaperService::OnUnbind(intent, succeeded);
}

ECode CImageWallpaper::OnRebind(
    /* [in] */ IIntent* intent)
{
    return WallpaperService::OnRebind(intent);
}

ECode CImageWallpaper::GetClassName(
    /* [out] */ String* className)
{
    VALIDATE_NOT_NULL(className);
    return WallpaperService::GetClassName(className);
}


ECode CImageWallpaper::GetMainLooper(
    /* [out] */ ILooper** looper)
{
    return WallpaperService::GetMainLooper(looper);
}

ECode CImageWallpaper::RegisterComponentCallbacks(
    /* [in] */ IComponentCallbacks* componentCallback)
{
    return WallpaperService::RegisterComponentCallbacks(componentCallback);
}

ECode CImageWallpaper::UnregisterComponentCallbacks(
    /* [in] */ IComponentCallbacks* componentCallback)
{
    return WallpaperService::UnregisterComponentCallbacks(componentCallback);
}

ECode CImageWallpaper::GetString(
    /* [in] */ Int32 resId,
    /* [in] */ ArrayOf<IInterface*>* formatArgs,
    /* [out] */ String* str)
{
    return WallpaperService::GetString(resId, formatArgs, str);
}

ECode CImageWallpaper::GetThemeResId(
    /* [out] */ Int32* resId)
{
    return WallpaperService::GetThemeResId(resId);
}

ECode CImageWallpaper::GetPackageCodePath(
    /* [out] */ String* codePath)
{
    return WallpaperService::GetPackageCodePath(codePath);
}

ECode CImageWallpaper::GetSharedPrefsFile(
    /* [in] */ const String& name,
    /* [out] */ IFile** file)
{
    return WallpaperService::GetSharedPrefsFile(name, file);
}

ECode CImageWallpaper::GetSharedPreferences(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */ ISharedPreferences** prefs)
{
    return WallpaperService::GetSharedPreferences(name, mode, prefs);
}

ECode CImageWallpaper::OpenFileOutput(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [out] */IFileOutputStream** fileOutputStream)
{
    return WallpaperService::OpenFileOutput(name, mode, fileOutputStream);
}

ECode CImageWallpaper::DeleteFile(
    /* [in] */ const String& name,
    /* [out] */ Boolean* succeeded)
{
    return WallpaperService::DeleteFile(name, succeeded);
}

ECode CImageWallpaper::GetFileStreamPath(
    /* [in] */ const String& name,
    /* [out] */ IFile** file)
{
    return WallpaperService::GetFileStreamPath(name, file);
}

ECode CImageWallpaper::GetFilesDir(
    /* [out] */ IFile** filesDir)
{
    VALIDATE_NOT_NULL(filesDir);
    return WallpaperService::GetFilesDir(filesDir);
}

ECode CImageWallpaper::GetObbDir(
    /* [out] */ IFile** obbDir)
{
    return WallpaperService::GetObbDir(obbDir);
}

ECode CImageWallpaper::GetFileList(
    /* [out, callee] */ ArrayOf<String>** fileList)
{
    return WallpaperService::GetFileList(fileList);
}

ECode CImageWallpaper::OpenOrCreateDatabase(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [out] */ ISQLiteDatabase** sqliteDB)
{
    return WallpaperService::OpenOrCreateDatabase(name, mode, factory, sqliteDB);
}

ECode CImageWallpaper::OpenOrCreateDatabase(
    /* [in] */ const String& name,
    /* [in] */ Int32 mode,
    /* [in] */ ISQLiteDatabaseCursorFactory* factory,
    /* [in] */ IDatabaseErrorHandler* errorHandler,
    /* [out] */ ISQLiteDatabase** sqliteDB)
{
    return WallpaperService::OpenOrCreateDatabase(name, mode, factory, errorHandler, sqliteDB);
}

ECode CImageWallpaper::DeleteDatabase(
    /* [in] */ const String& name,
    /* [out] */ Boolean* succeeded)
{
    return WallpaperService::DeleteDatabase(name, succeeded);
}

ECode CImageWallpaper::GetDatabasePath(
    /* [in] */ const String& name,
    /* [out] */ IFile** path)
{
    return WallpaperService::GetDatabasePath(name, path);
}

ECode CImageWallpaper::GetDatabaseList(
    /* [out, callee] */ ArrayOf<String>** databaseList)
{
    return WallpaperService::GetDatabaseList(databaseList);
}

ECode CImageWallpaper::GetWallpaper(
    /* [out] */ IDrawable** drawable)
{
    return WallpaperService::GetWallpaper(drawable);
}

ECode CImageWallpaper::PeekWallpaper(
    /* [out] */ IDrawable** drawable)
{
    return WallpaperService::PeekWallpaper(drawable);
}

ECode CImageWallpaper::GetWallpaperDesiredMinimumWidth(
    /* [out] */ Int32* minWidth)
{
    return WallpaperService::GetWallpaperDesiredMinimumWidth(minWidth);
}

ECode CImageWallpaper::GetWallpaperDesiredMinimumHeight(
    /* [out] */ Int32* minHeight)
{
    return WallpaperService::GetWallpaperDesiredMinimumHeight(minHeight);
}

ECode CImageWallpaper::SetWallpaper(
    /* [in] */ IBitmap* bitmap)
{
    return WallpaperService::SetWallpaper(bitmap);
}

ECode CImageWallpaper::SetWallpaper(
    /* [in] */ IInputStream* data)
{
    return WallpaperService::SetWallpaper(data);
}

ECode CImageWallpaper::ClearWallpaper()
{
    return WallpaperService::ClearWallpaper();
}

ECode CImageWallpaper::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return WallpaperService::StartActivityAsUser(intent, user);
}

ECode CImageWallpaper::StartActivityAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* user)
{
    return WallpaperService::StartActivityAsUser(intent, options, user);
}

ECode CImageWallpaper::StartActivities(
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options)
{
    return WallpaperService::StartActivities(intents, options);
}

ECode CImageWallpaper::StartIntentSender(
    /* [in] */ IIntentSender* intent,
    /* [in] */ IIntent* fillInIntent,
    /* [in] */ Int32 flagsMask,
    /* [in] */ Int32 flagsValues,
    /* [in] */ Int32 extraFlags,
    /* [in] */ IBundle* options)
{
    return WallpaperService::StartIntentSender(intent, fillInIntent, flagsMask, flagsValues, extraFlags, options);
}

ECode CImageWallpaper::SendBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission)
{
    return WallpaperService::SendBroadcast(intent, receiverPermission);
}

ECode CImageWallpaper::SendOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission)
{
    return WallpaperService::SendOrderedBroadcast(intent, receiverPermission);
}

ECode CImageWallpaper::SendOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& receiverPermission,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return WallpaperService::SendOrderedBroadcast(intent, receiverPermission,
            resultReceiver, scheduler, initialCode, initialData, initialExtras);
}

ECode CImageWallpaper::SendBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return WallpaperService::SendBroadcastAsUser(intent, user);
}

ECode CImageWallpaper::SendBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& receiverPermission)
{
    return WallpaperService::SendBroadcastAsUser(intent, user, receiverPermission);
}

ECode CImageWallpaper::SendOrderedBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ const String& receiverPermission,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return WallpaperService::SendOrderedBroadcastAsUser(intent, user, receiverPermission,
            resultReceiver, scheduler, initialCode, initialData, initialExtras);
}

ECode CImageWallpaper::SendStickyBroadcast(
    /* [in] */ IIntent* intent)
{
    return WallpaperService::SendStickyBroadcast(intent);
}

ECode CImageWallpaper::SendStickyOrderedBroadcast(
    /* [in] */ IIntent* intent,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return WallpaperService::SendStickyOrderedBroadcast(intent, resultReceiver,
            scheduler, initialCode, initialData, initialExtras);
}

ECode CImageWallpaper::RemoveStickyBroadcast(
    /* [in] */ IIntent* intent)
{
    return WallpaperService::RemoveStickyBroadcast(intent);
}

ECode CImageWallpaper::SendStickyBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return WallpaperService::SendStickyBroadcastAsUser(intent, user);
}

ECode CImageWallpaper::SendStickyOrderedBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user,
    /* [in] */ IBroadcastReceiver* resultReceiver,
    /* [in] */ IHandler* scheduler,
    /* [in] */ Int32 initialCode,
    /* [in] */ const String& initialData,
    /* [in] */ IBundle* initialExtras)
{
    return WallpaperService::SendStickyOrderedBroadcastAsUser(intent, user, resultReceiver,
            scheduler, initialCode, initialData, initialExtras);
}

ECode CImageWallpaper::RemoveStickyBroadcastAsUser(
    /* [in] */ IIntent* intent,
    /* [in] */ IUserHandle* user)
{
    return WallpaperService::RemoveStickyBroadcastAsUser(intent, user);
}

ECode CImageWallpaper::StartInstrumentation(
    /* [in] */ IComponentName* className,
    /* [in] */ const String& profileFile,
    /* [in] */ IBundle* arguments,
    /* [out] */ Boolean* succeeded)
{
    assert(succeeded != NULL);
    return WallpaperService::StartInstrumentation(className, profileFile, arguments, succeeded);
}

ECode CImageWallpaper::EnforcePermission(
    /* [in] */ const String& permission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ const String& message)
{
    return WallpaperService::EnforcePermission(permission, pid, uid, message);
}

ECode CImageWallpaper::EnforceCallingPermission(
    /* [in] */ const String& permission,
    /* [in] */ const String& message)
{
    return WallpaperService::EnforceCallingPermission(permission, message);
}

ECode CImageWallpaper::IsRestricted(
    /* [out] */ Boolean* isRestricted)
{
    return WallpaperService::IsRestricted(isRestricted);
}

ECode CImageWallpaper::OpenFileInput(
    /* [in] */ const String& name,
    /* [out] */ IFileInputStream** fileInputStream)
{
    return WallpaperService::OpenFileInput(name, fileInputStream);
}

ECode CImageWallpaper::GetExternalCacheDir(
    /* [out] */ IFile** externalDir)
{
    return WallpaperService::GetExternalCacheDir(externalDir);
}

ECode CImageWallpaper::StartActivity(
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options)
{
    return WallpaperService::StartActivity(intent, options);
}

ECode CImageWallpaper::StartActivities(
    /* [in] */ ArrayOf<IIntent*>* intents)
{
    return WallpaperService::StartActivities(intents);
}

ECode CImageWallpaper::StartActivitiesAsUser(
    /* [in] */ ArrayOf<IIntent*>* intents,
    /* [in] */ IBundle* options,
    /* [in] */ IUserHandle* userHandle)
{
    return WallpaperService::StartActivitiesAsUser(intents, options, userHandle);
}

ECode CImageWallpaper::RegisterReceiverAsUser(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IUserHandle* user,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& broadcastPermission,
    /* [in] */ IHandler* scheduler,
    /* [out] */ IIntent** stickyIntent)
{
    return WallpaperService::RegisterReceiverAsUser(receiver, user, filter, broadcastPermission,
            scheduler, stickyIntent);
}

ECode CImageWallpaper::StartServiceAsUser(
    /* [in] */ IIntent* service,
    /* [in] */ IUserHandle* user,
    /* [out] */ IComponentName** name)
{
    return WallpaperService::StartServiceAsUser(service, user, name);
}

ECode CImageWallpaper::StopServiceAsUser(
    /* [in] */ IIntent* service,
    /* [in] */ IUserHandle* user,
    /* [out] */ Boolean* succeeded)
{
    return WallpaperService::StopServiceAsUser(service, user, succeeded);
}

ECode CImageWallpaper::BindService(
    /* [in] */ IIntent* service,
    /* [in] */ Elastos::Droid::Content::IServiceConnection* conn,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 userHandle,
    /* [out] */ Boolean* succeeded)
{
    return WallpaperService::BindService(service, conn, flags, userHandle, succeeded);
}

ECode CImageWallpaper::CheckCallingUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* result)
{
    return WallpaperService::CheckCallingUriPermission(uri, modeFlags, result);
}

ECode CImageWallpaper::CheckCallingOrSelfUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [out] */ Int32* result)
{
    return WallpaperService::CheckCallingOrSelfUriPermission(uri, modeFlags, result);
}

ECode CImageWallpaper::EnforceUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return WallpaperService::EnforceUriPermission(uri, pid, uid, modeFlags, message);
}

ECode CImageWallpaper::EnforceCallingUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return WallpaperService::EnforceCallingUriPermission(uri, modeFlags, message);
}

ECode CImageWallpaper::EnforceCallingOrSelfUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return WallpaperService::EnforceCallingOrSelfUriPermission(uri, modeFlags, message);
}

ECode CImageWallpaper::EnforceUriPermission(
    /* [in] */ IUri* uri,
    /* [in] */ const String& readPermission,
    /* [in] */ const String& writePermission,
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 modeFlags,
    /* [in] */ const String& message)
{
    return WallpaperService::EnforceUriPermission(uri, readPermission, writePermission,
            pid, uid, modeFlags, message);
}

ECode CImageWallpaper::CreatePackageContextAsUser(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 flags,
    /* [in] */ IUserHandle* user,
    /* [out] */ IContext** ctx)
{
    return WallpaperService::CreatePackageContextAsUser(packageName, flags, user, ctx);
}

ECode CImageWallpaper::CreateConfigurationContext(
    /* [in] */ IConfiguration* overrideConfiguration,
    /* [out] */ IContext** ctx)
{
    return WallpaperService::CreateConfigurationContext(overrideConfiguration, ctx);
}

ECode CImageWallpaper::CreateDisplayContext(
    /* [in] */ IDisplay* display,
    /* [out] */ IContext** ctx)
{
    return WallpaperService::CreateDisplayContext(display, ctx);
}

ECode CImageWallpaper::GetCompatibilityInfo(
    /* [in] */ Int32 displayId,
    /* [out] */ ICompatibilityInfoHolder** infoHolder)
{
    return WallpaperService::GetCompatibilityInfo(displayId, infoHolder);
}

ECode CImageWallpaper::GetApplication(
    /* [out] */ IApplication** application)
{
    return WallpaperService::GetApplication(application);
}

ECode CImageWallpaper::RegisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [out] */ IIntent** intent)
{
    return WallpaperService::RegisterReceiver(receiver,filter,intent);
}

ECode CImageWallpaper::RegisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver,
    /* [in] */ IIntentFilter* filter,
    /* [in] */ const String& broadcastPermission,
    /* [in] */ IHandler* scheduler,
    /* [out] */ IIntent** intent)
{
    return WallpaperService::RegisterReceiver(receiver,filter,
        broadcastPermission,scheduler,intent);
}

ECode CImageWallpaper::UnregisterReceiver(
    /* [in] */ IBroadcastReceiver* receiver)
{
    return WallpaperService::UnregisterReceiver(receiver);
}

ECode CImageWallpaper::OnTaskRemoved(
    /* [in] */ IIntent* rootIntent)
{
    return WallpaperService::OnTaskRemoved(rootIntent);
}

ECode CImageWallpaper::StopSelf()
{
    return WallpaperService::StopSelf();
}

ECode CImageWallpaper::StopSelf(
    /* [in] */ Int32 startId)
{
    return WallpaperService::StopSelf(startId);
}

ECode CImageWallpaper::StopSelfResult(
    /* [in] */ Int32 startId,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    return WallpaperService::StopSelfResult(startId, res);
}

ECode CImageWallpaper::SetForeground(
    /* [in] */ Boolean isForeground)
{
    return WallpaperService::SetForeground(isForeground);
}

ECode CImageWallpaper::StartForeground(
    /* [in] */ Int32 id,
    /* [in] */ INotification* notification)
{
    return WallpaperService::StartForeground(id, notification);
}

ECode CImageWallpaper::StopForeground(
    /* [in] */ Boolean removeNotification)
{
    return WallpaperService::StopForeground(removeNotification);
}

ECode CImageWallpaper::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    return WallpaperService::OnConfigurationChanged(newConfig);
}

ECode CImageWallpaper::OnLowMemory()
{
    return WallpaperService::OnLowMemory();
}


} // namespace SystemUI
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
