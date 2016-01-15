#include "elastos/droid/app/backup/CWallpaperBackupHelperHelper.h"
#include "elastos/droid/app/backup/CWallpaperBackupHelper.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CAR_INTERFACE_IMPL(CWallpaperBackupHelperHelper, Singleton, IWallpaperBackupHelperHelper)

CAR_SINGLETON_IMPL(CWallpaperBackupHelperHelper)

ECode CWallpaperBackupHelperHelper::GetWALLPAPER_IMAGE(
    /* [out] */ String* image)
{
    VALIDATE_NOT_NULL(image);
    *image = CWallpaperBackupHelper::GetWALLPAPER_IMAGE();
    return NOERROR;
}

ECode CWallpaperBackupHelperHelper::GetWALLPAPER_INFO(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    *info = CWallpaperBackupHelper::GetWALLPAPER_INFO();
    return NOERROR;
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
