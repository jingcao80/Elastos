#ifndef __ELASTOS_DROID_SERVER_WALLPAPER_CWALLPAPERMANAGERSERVICE_H__
#define __ELASTOS_DROID_SERVER_WALLPAPER_CWALLPAPERMANAGERSERVICE_H__

#include "_Elastos_Droid_Server_Wallpaper_CWallpaperManagerService.h"
#include "elastos/droid/server/wallpaper/WallpaperManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wallpaper {

CarClass(CWallpaperManagerService)
    , public WallpaperManagerService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Wallpaper
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WALLPAPER_CWALLPAPERMANAGERSERVICE_H__
