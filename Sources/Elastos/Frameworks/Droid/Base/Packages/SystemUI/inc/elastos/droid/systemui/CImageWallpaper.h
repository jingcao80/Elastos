
#ifndef  __ELASTOS_DROID_SYSTEMUI_CIMAGEWALLPAPER_H__
#define  __ELASTOS_DROID_SYSTEMUI_CIMAGEWALLPAPER_H__

#include "_Elastos_Droid_SystemUI_CImageWallpaper.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Opengl.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/droid/service/wallpaper/WallpaperService.h>

using Elastos::Droid::App::IWallpaperManager;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Service::Wallpaper::IWallpaperServiceEngine;
using Elastos::Droid::Service::Wallpaper::WallpaperService;
using Elastosx::Microedition::Khronos::Egl::IEGLDisplay;
using Elastosx::Microedition::Khronos::Egl::IEGLConfig;
using Elastosx::Microedition::Khronos::Egl::IEGLContext;
using Elastosx::Microedition::Khronos::Egl::IEGLSurface;
using Elastosx::Microedition::Khronos::Egl::IEGL10;
using Elastos::IO::IFloatBuffer;

namespace Elastos {
namespace Droid {
namespace SystemUI {

CarClass(CImageWallpaper)
    , public WallpaperService
    , public IImageWallpaper
{
private:
    class DrawableEngine : public Engine
    {
    private:
        class WallpaperObserver : public BroadcastReceiver
        {
        public:
            WallpaperObserver(
                /* [in] */ DrawableEngine* owner);

            CARAPI OnReceive(
                /* [in] */ IContext* context,
                /* [in] */ IIntent* intent);

            TO_STRING_IMPL("CImageWallpaper::DrawableEngine::WallpaperObserver")
        private:
            DrawableEngine* mOwner;
        };

    public:
        DrawableEngine(
            /* [in] */ CImageWallpaper* owner);

        CARAPI_(void) TrimMemory(
            /* [in] */ Int32 level);

        //@Override
        CARAPI OnCreate(
            /* [in] */ ISurfaceHolder* surfaceHolder);

        //@Override
        CARAPI OnDestroy();

        //@Override
        CARAPI OnVisibilityChanged(
            /* [in] */ Boolean visible);

        //@Override
        CARAPI OnTouchEvent(
            /* [in] */ IMotionEvent* event);

        //@Override
        CARAPI OnOffsetsChanged(
            /* [in] */ Float xOffset,
            /* [in] */ Float yOffset,
            /* [in] */ Float xOffsetStep,
            /* [in] */ Float yOffsetStep,
            /* [in] */ Int32 xPixels,
            /* [in] */ Int32 yPixels);

        //@Override
        CARAPI OnSurfaceChanged(
            /* [in] */ ISurfaceHolder* holder,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        //@Override
        CARAPI OnSurfaceDestroyed(
            /* [in] */ ISurfaceHolder* holder);

        //@Override
        CARAPI OnSurfaceCreated(
            /* [in] */ ISurfaceHolder* holder);

        //@Override
        CARAPI OnSurfaceRedrawNeeded(
            /* [in] */ ISurfaceHolder* holder);

    private:
        CARAPI_(void) UpdateSurfaceSize(
            /* [in] */ ISurfaceHolder* surfaceHolder);

        CARAPI_(AutoPtr<IPoint>) GetDefaultDisplaySize();

        CARAPI_(void) DrawFrame();

        CARAPI_(void) UpdateWallpaperLocked();

        CARAPI_(void) DrawWallpaperWithCanvas(
            /* [in] */ ISurfaceHolder* sh,
            /* [in] */ Int32 w,
            /* [in] */ Int32 h,
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

        CARAPI_(Boolean) DrawWallpaperWithOpenGL(
            /* [in] */ ISurfaceHolder* sh,
            /* [in] */ Int32 w,
            /* [in] */ Int32 h,
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

        AutoPtr<IFloatBuffer> CreateMesh(
            /* [in] */ Int32 left,
            /* [in] */ Int32 top,
            /* [in] */ Int32 right,
            /* [in] */ Int32 bottom);

        Int32 LoadTexture(
            /* [in] */ IBitmap* bitmap);

        Int32 BuildProgram(
            /* [in] */ const String& vertex,
            /* [in] */ const String& fragment);

        Int32 BuildShader(
            /* [in] */ const String& source,
            /* [in] */ Int32 type);

        void CheckEglError();

        void CheckGlError();

        void FinishGL();

        Boolean InitGL(
            /* [in] */ ISurfaceHolder* surfaceHolder);

        AutoPtr<IEGLContext> CreateContext(
            /* [in] */ IEGL10* egl,
            /* [in] */ IEGLDisplay* eglDisplay,
            /* [in] */ IEGLConfig* eglConfig);

        AutoPtr<IEGLConfig> ChooseEglConfig();

        AutoPtr<ArrayOf<Int32> > GetConfig();

    private:
        static const Int32 EGL_CONTEXT_CLIENT_VERSION;
        static const Int32 EGL_OPENGL_ES2_BIT;

        static const Int32 FLOAT_SIZE_BYTES;
        static const Int32 TRIANGLE_VERTICES_DATA_STRIDE_BYTES;
        static const Int32 TRIANGLE_VERTICES_DATA_POS_OFFSET;
        static const Int32 TRIANGLE_VERTICES_DATA_UV_OFFSET;

    private:
        // TODO: Not currently used, keeping around until we know we don't need it
        //@SuppressWarnings({"UnusedDeclaration"})
        AutoPtr<WallpaperObserver> mReceiver;

        AutoPtr<IBitmap> mBackground;
        Int32 mBackgroundWidth;
        Int32 mBackgroundHeight;
        Int32 mLastSurfaceWidth;
        Int32 mLastSurfaceHeight;
        Int32 mLastRotation;
        Float mXOffset;
        Float mYOffset;
        Float mScale;

        Boolean mVisible;
        Boolean mRedrawNeeded;
        Boolean mOffsetsChanged;
        Int32 mLastXTranslation;
        Int32 mLastYTranslation;

        AutoPtr<IEGL10> mEgl;
        AutoPtr<IEGLDisplay> mEglDisplay;
        AutoPtr<IEGLConfig> mEglConfig;
        AutoPtr<IEGLContext> mEglContext;
        AutoPtr<IEGLSurface> mEglSurface;

        static const String sSimpleVS;
        static const String sSimpleFS;

        CImageWallpaper* mOwner;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CImageWallpaper();

    CARAPI OnCreate();

    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    virtual CARAPI OnCreateEngine(
        /* [out] */ IWallpaperServiceEngine** engine);

    static Boolean IsEmulator();

private:
    static const String GL_LOG_TAG;
    static const Boolean DEBUG;
    static const String PROPERTY_KERNEL_QEMU;

    static const Boolean FIXED_SIZED_SURFACE;
    static const Boolean USE_OPENGL;

private:
    AutoPtr<IWallpaperManager> mWallpaperManager;

    AutoPtr<DrawableEngine> mEngine;

    Boolean mIsHwAccelerated;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_CIMAGEWALLPAPER_H__
