#include "elastos/droid/app/backup/CBackupManagerHelper.h"
#include "elastos/droid/app/backup/CBackupManager.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

CAR_INTERFACE_IMPL(CBackupManagerHelper, Singleton, IBackupManagerHelper)

CAR_SINGLETON_IMPL(CBackupManagerHelper)

ECode CBackupManagerHelper::DataChanged(
    /* [in] */ const String& packageName)
{
    return CBackupManager::DataChanged(packageName);
}

} // namespace Backup
} // namespace App
} // namespace Droid
} // namespace Elastos
