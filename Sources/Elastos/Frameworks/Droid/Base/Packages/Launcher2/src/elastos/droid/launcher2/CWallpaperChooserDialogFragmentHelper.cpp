
#include "elastos/droid/launcher2/WallpaperChooserDialogFragment.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(CWallpaperChooserDialogFragmentHelper, Singleton, IWallpaperChooserDialogFragmentHelper)

CAR_SINGLETON_IMPL(CWallpaperChooserDialogFragmentHelper)

ECode CWallpaperChooserDialogFragmentHelper::NewInstance(
    /* [out] */ IWallpaperChooserDialogFragment** fragment);
{
    VALIDATE_NOT_NULL(fragment);

    return WallpaperChooserDialogFragment::NewInstance(fragment);
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos