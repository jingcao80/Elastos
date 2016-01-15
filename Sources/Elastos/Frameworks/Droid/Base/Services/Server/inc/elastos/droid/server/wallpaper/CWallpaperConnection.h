#ifndef __ELASTOS_DROID_SERVER_WALLPAPER_CWALLPAPERCONNECTION_H__
#define __ELASTOS_DROID_SERVER_WALLPAPER_CWALLPAPERCONNECTION_H__

#include "_Elastos_Droid_Server_Wallpaper_CWallpaperConnection.h"
#include "elastos/droid/server/wallpaper/WallpaperManagerService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wallpaper {

CarClass(CWallpaperConnection)
    , public WallpaperManagerService::WallpaperConnection
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Wallpaper
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_WALLPAPER_CWALLPAPERCONNECTION_H__
