
#include "elastos/droid/app/CWallpaperManagerHelper.h"
#include "elastos/droid/app/CWallpaperManager.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CWallpaperManagerHelper, Object, IWallpaperManagerHelper)

CAR_SINGLETON_IMPL(CWallpaperManagerHelper)

ECode CWallpaperManagerHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ IWallpaperManager** manager)
{
    return CWallpaperManager::GetInstance(context, manager);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
