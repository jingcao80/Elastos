
#ifndef __ELASTOS_DROID_GALAXY4_CGALAXY4WALLPAPER_H__
#define __ELASTOS_DROID_GALAXY4_CGALAXY4WALLPAPER_H__

#include "_Elastos_Droid_Galaxy4_CGalaxy4Wallpaper.h"
#include "RenderScript.h"
#include "GalaxyRS.h"
#include <elastos/droid/service/wallpaper/WallpaperService.h>

using Elastos::Droid::Service::Wallpaper::WallpaperService;
using Elastos::Droid::Service::Wallpaper::IWallpaperServiceEngine;
using Elastos::Droid::View::ISurfaceHolder;

namespace Elastos {
namespace Droid {
namespace Galaxy4 {

CarClass(CGalaxy4Wallpaper)
    , public WallpaperService
{
private:
    class RenderScriptEngine
        : public WallpaperService::Engine
    {
    public:
        RenderScriptEngine(
            /* [in] */ CGalaxy4Wallpaper* host)
            : mHost(host)
        {}

        // @Override
        CARAPI OnCreate(
            /* [in] */ ISurfaceHolder* surfaceHolder);

        // @Override
        CARAPI OnDestroy();

        CARAPI_(void) DestroyRenderer();

        // @Override
        CARAPI OnSurfaceCreated(
            /* [in] */ ISurfaceHolder* surfaceHolder);

        // @Override
        CARAPI OnSurfaceDestroyed(
            /* [in] */ ISurfaceHolder* surfaceHolder);

        // @Override
        CARAPI OnSurfaceChanged(
            /* [in] */ ISurfaceHolder* surfaceHolder,
            /* [in] */ Int32 format,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        // @Override
        CARAPI OnCommand(
            /* [in] */ const String& action,
            /* [in] */ Int32 x,
            /* [in] */ Int32 y,
            /* [in] */ Int32 z,
            /* [in] */ IBundle* extras,
            /* [in] */ Boolean resultRequested,
            /* [out] */ IBundle** result);

        // @Override
        CARAPI OnVisibilityChanged(
            /* [in] */ Boolean visible);

    private:
        AutoPtr<RenderScript> mRenderScript;
        AutoPtr<GalaxyRS> mWallpaperRS;
        Int32 mDensityDPI;

        CGalaxy4Wallpaper* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    // @Override
    CARAPI OnCreateEngine(
        /* [out] */ IWallpaperServiceEngine** engine);
};

} // namespace Galaxy4
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GALAXY4_CGALAXY4WALLPAPER_H__
