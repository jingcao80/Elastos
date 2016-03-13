
#ifndef  __ELASTOS_DROID_LAUNCHER2_CWALLPAPERCHOOSERDIALOGFRAGMENT_H__
#define  __ELASTOS_DROID_LAUNCHER2_CWALLPAPERCHOOSERDIALOGFRAGMENT_H__

#include "_Elastos_Droid_Launcher2_CWallpaperChooserDialogFragment.h"
#include "elastos/droid/launcher2/WallpaperChooserDialogFragment.h"

using Elastos::Droid::Launcher2::WallpaperChooserDialogFragment;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CWallpaperChooserDialogFragment)
    , public WallpaperChooserDialogFragment
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CWALLPAPERCHOOSERDIALOGFRAGMENT_H__
