
#ifndef __ELASTOS_DROID_SERVICE_WALLPAPER_CWALLPAPERSERVICEENGINEWINDOW_H__
#define __ELASTOS_DROID_SERVICE_WALLPAPER_CWALLPAPERSERVICEENGINEWINDOW_H__

#include "_Elastos_Droid_Service_Wallpaper_CWallpaperServiceEngineWindow.h"
#include "elastos/droid/service/wallpaper/WallpaperService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Wallpaper {

CarClass(CWallpaperServiceEngineWindow)
    , public WallpaperService::Engine::MWindow
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Wallpaper
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_WALLPAPER_CWALLPAPERSERVICEENGINEWINDOW_H__
