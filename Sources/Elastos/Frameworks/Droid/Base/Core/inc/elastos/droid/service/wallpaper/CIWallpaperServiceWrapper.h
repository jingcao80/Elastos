
#ifndef __ELASTOS_DROID_SERVICE_WALLPAPER_CIWALLPAPERSERVICEWRAPPER_H__
#define __ELASTOS_DROID_SERVICE_WALLPAPER_CIWALLPAPERSERVICEWRAPPER_H__

#include "_Elastos_Droid_Service_Wallpaper_CIWallpaperServiceWrapper.h"
#include "elastos/droid/service/wallpaper/WallpaperService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Wallpaper {

CarClass(CIWallpaperServiceWrapper)
    , public WallpaperService::IWallpaperServiceWrapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Wallpaper
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_WALLPAPER_CIWALLPAPERSERVICEWRAPPER_H__