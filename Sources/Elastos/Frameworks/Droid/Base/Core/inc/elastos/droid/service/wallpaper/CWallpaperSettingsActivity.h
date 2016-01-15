#ifndef __ELASTOS_DROID_SERVICE_WALLPAPER_CWALLPAPERSETTINGSACTIVITY_H__
#define __ELASTOS_DROID_SERVICE_WALLPAPER_CWALLPAPERSETTINGSACTIVITY_H__

#include "_Elastos_Droid_Service_Wallpaper_CWallpaperSettingsActivity.h"
#include "elastos/droid/service/wallpaper/WallpaperSettingsActivity.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Wallpaper {

CarClass(CWallpaperSettingsActivity)
    , public WallpaperSettingsActivity
{
public:
    CAR_OBJECT_DECL()

};

} // namespace Wallpaper
} // namespace Service
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVICE_WALLPAPER_CWALLPAPERSETTINGSACTIVITY_H__