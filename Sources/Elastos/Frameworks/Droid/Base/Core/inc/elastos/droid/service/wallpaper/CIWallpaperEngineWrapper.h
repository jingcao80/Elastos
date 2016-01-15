
#ifndef __ELASTOS_DROID_SERVICE_WALLPAPER_CIWALLPAPERENGINEWRAPPER_H__
#define __ELASTOS_DROID_SERVICE_WALLPAPER_CIWALLPAPERENGINEWRAPPER_H__

#include "_Elastos_Droid_Service_Wallpaper_CIWallpaperEngineWrapper.h"
#include "elastos/droid/service/wallpaper/WallpaperService.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Wallpaper {

CarClass(CIWallpaperEngineWrapper)
    , public WallpaperService::IWallpaperEngineWrapper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Wallpaper
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_WALLPAPER_CIWALLPAPERENGINEWRAPPER_H__
