
#ifndef __ELASTOS_DROID_SERVICE_WALLPAPER_CWALLPAPERSERVICEENGINE_H__
#define __ELASTOS_DROID_SERVICE_WALLPAPER_CWALLPAPERSERVICEENGINE_H__

#include "_Elastos_Droid_Service_Wallpaper_CWallpaperServiceEngine.h"
#include "elastos/droid/service/wallpaper/WallpaperService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Wallpaper {

CarClass(CWallpaperServiceEngine)
    , public WallpaperService::Engine
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Wallpaper
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_WALLPAPER_CWALLPAPERSERVICEENGINE_H__
