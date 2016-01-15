
#ifndef __ELASTOS_DROID_APP_CBACKUPAGENTBACKUPSERVICEBINDER_H__
#define __ELASTOS_DROID_APP_CBACKUPAGENTBACKUPSERVICEBINDER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_App_CBackupAgentBackupServiceBinder.h"
#include "backup/BackupAgent.h"

using Elastos::Droid::App::Backup::BackupAgent;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::Os::IParcelFileDescriptor;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CBackupAgentBackupServiceBinder)
{
public:
    CARAPI constructor(
        /* [in] */ Handle32 host);

    CARAPI DoBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ IParcelFileDescriptor* newState,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoRestore(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int32 appVersionCode,
        /* [in] */ IParcelFileDescriptor* newState,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoFullBackup(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

    CARAPI DoRestoreFile(
        /* [in] */ IParcelFileDescriptor* data,
        /* [in] */ Int64 size,
        /* [in] */ Int32 type,
        /* [in] */ const String& domainName,
        /* [in] */ const String& path,
        /* [in] */ Int64 mode,
        /* [in] */ Int64 mtime,
        /* [in] */ Int32 token,
        /* [in] */ IIBackupManager* callbackBinder);

private:
    static const String TAG;
    static const Boolean DEBUG;
    BackupAgent* mHost;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_APP_CBACKUPAGENTBACKUPSERVICEBINDER_H__
