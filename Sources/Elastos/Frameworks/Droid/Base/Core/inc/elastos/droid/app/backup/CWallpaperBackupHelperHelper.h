#ifndef __ELASTOS_DROID_APP_BACKUP_CWALLPAPERBACKUPHELPERHELPER_H__
#define __ELASTOS_DROID_APP_BACKUP_CWALLPAPERBACKUPHELPERHELPER_H__

#include "_Elastos_Droid_App_Backup_CWallpaperBackupHelperHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CWallpaperBackupHelperHelper)
    , public Singleton
    , public IWallpaperBackupHelperHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetWALLPAPER_IMAGE(
        /* [out] */ String* image);

    CARAPI GetWALLPAPER_INFO(
        /* [out] */ String* info);
};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_APP_BACKUP_CWALLPAPERBACKUPHELPERHELPER_H__
