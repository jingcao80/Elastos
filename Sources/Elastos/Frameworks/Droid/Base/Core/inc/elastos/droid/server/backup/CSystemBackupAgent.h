#ifndef __ELASTOS_DROID_SERVER_BACKUP_CSYSTEMBACKUPAGENT_H__
#define __ELASTOS_DROID_SERVER_BACKUP_CSYSTEMBACKUPAGENT_H__

#include "_Elastos_Droid_Server_Backup_CSystemBackupAgent.h"
#include "elastos/droid/server/backup/SystemBackupAgent.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Backup {

CarClass(CSystemBackupAgent)
    , public SystemBackupAgent
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Backup
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_BACKUP_CSYSTEMBACKUPAGENT_H__
