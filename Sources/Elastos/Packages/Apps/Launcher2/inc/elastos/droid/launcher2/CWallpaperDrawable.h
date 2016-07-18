
#ifndef __ELASTOS_DROID_LAUNCHER2_WALLPAPERDRAWABLE_H__
#define __ELASTOS_DROID_LAUNCHER2_WALLPAPERDRAWABLE_H__

#include "_Elastos_Droid_Launcher2_CWallpaperDrawable.h"
#include "elastos/droid/launcher2/WallpaperChooserDialogFragment.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CWallpaperDrawable)
    , public WallpaperChooserDialogFragment::WallpaperDrawable
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_WALLPAPERDRAWABLE_H__
