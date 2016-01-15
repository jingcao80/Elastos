#ifndef __ELASTOS_DROID_APP_BACKUP_CBACKUPMANAGERHELPER_H__
#define __ELASTOS_DROID_APP_BACKUP_CBACKUPMANAGERHELPER_H__

#include "_Elastos_Droid_App_Backup_CBackupManagerHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CarClass(CBackupManagerHelper)
    , public Singleton
    , public IBackupManagerHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Convenience method for callers who need to indicate that some other package
     * needs a backup pass.  This can be useful in the case of groups of packages
     * that share a uid.
     * <p>
     * This method requires that the application hold the "android.permission.BACKUP"
     * permission if the package named in the argument does not run under the same uid
     * as the caller.
     *
     * @param packageName The package name identifying the application to back up.
     */
    CARAPI DataChanged(
        /* [in] */ const String& packageName);

};

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_BACKUP_CBACKUPMANAGERHELPER_H__
