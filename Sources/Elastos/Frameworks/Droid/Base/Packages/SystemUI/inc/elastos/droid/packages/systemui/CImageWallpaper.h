
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_CIMAGEWALLPAPER_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_CIMAGEWALLPAPER_H__

#include "_Elastos_Droid_Packages_SystemUI_CImageWallpaper.h"
#include "Elastos.Droid.App.h"
#include <elastos/droid/service/wallpaper/WallpaperService.h>

using Elastos::Droid::App::IWallpaperManager;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Service::Wallpaper::IWallpaperServiceEngine;
using Elastos::Droid::Service::Wallpaper::WallpaperService;

namespace Elastos {
namespace Droid {
namespace Packages {
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

            CARAPI ToString(
                /* [out] */ String* info)
            {
                VALIDATE_NOT_NULL(info);
                *info = String("CImageWallpaper::DrawableEngine::WallpaperObserver");
                (*info).AppendFormat("%p", this);
                return NOERROR;
            }
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

        // private EGL10 mEgl;
        // private EGLDisplay mEglDisplay;
        // private EGLConfig mEglConfig;
        // private EGLContext mEglContext;
        // private EGLSurface mEglSurface;

        // private static final String sSimpleVS =
        //         "attribute vec4 position;\n" +
        //         "attribute vec2 texCoords;\n" +
        //         "varying vec2 outTexCoords;\n" +
        //         "uniform mat4 projection;\n" +
        //         "\nvoid main(void) {\n" +
        //         "    outTexCoords = texCoords;\n" +
        //         "    gl_Position = projection * position;\n" +
        //         "}\n\n";
        // private static final String sSimpleFS =
        //         "precision mediump Float;\n\n" +
        //         "varying vec2 outTexCoords;\n" +
        //         "uniform sampler2D texture;\n" +
        //         "\nvoid main(void) {\n" +
        //         "    gl_FragColor = texture2D(texture, outTexCoords);\n" +
        //         "}\n\n";

        CImageWallpaper* mOwner;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CImageWallpaper();

    CARAPI OnCreate();

    CARAPI OnTrimMemory(
        /* [in] */ Int32 level);

    virtual CARAPI OnCreateEngine(
        /* [out] */ IWallpaperServiceEngine** engine);

private:
    static const String TAG;
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
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_CIMAGEWALLPAPER_H__
