
#ifndef __ELASTOS_DROID_APP_BACKUP_BACKUPAGENTHELPER_H__
#define  __ELASTOS_DROID_APP_BACKUP_BACKUPAGENTHELPER_H__

#include "elastos/droid/app/backup/BackupAgent.h"

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {
/**
 * A convenient {@link BackupAgent} wrapper class that automatically manages
 * heterogeneous data sets within the backup data, each identified by a unique
 * key prefix.  When processing a backup or restore operation, the BackupAgentHelper
 * dispatches to one or more installed {@link BackupHelper} objects, each
 * of which is responsible for a defined subset of the data being processed.
 * <p>
 * An application will typically extend this class in its own
 * backup agent. Then, within the agent's {@link BackupAgent#onCreate() onCreate()}
 * method, it will call {@link #addHelper(String, BackupHelper) addHelper()} one or more times to
 * install the handlers for each kind of data it wishes to manage within its backups.
 * <p>
 * The Android framework currently provides two predefined {@link BackupHelper} classes:</p>
 * <ul><li>{@link FileBackupHelper} - Manages the backup and restore of entire files
 * within an application's data directory hierarchy.</li>
 * <li>{@link SharedPreferencesBackupHelper} - Manages the backup and restore of an
 * application's {@link android.content.SharedPreferences} data.</li></ul>
 * <p>
 * An application can also implement its own helper classes to work within the
 * {@link BackupAgentHelper} framework.  See the {@link BackupHelper} interface
 * documentation for details.
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about using BackupAgentHelper, read the
 * <a href="{@docRoot}guide/topics/data/backup.html">Data Backup</a> developer guide.</p>
 * </div>
 *
 * @see BackupHelper
 * @see FileBackupHelper
 * @see SharedPreferencesBackupHelper
 */
extern "C" const InterfaceID EIID_BackupAgentHelper;
class BackupAgentHelper
    : public BackupAgent
    , public IBackupAgentHelper
{
public:
    BackupAgentHelper();

    ~BackupAgentHelper();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    /**
     * Run the backup process on each of the configured handlers.
     */
    //@Override
    CARAPI OnBackup(
        /* [in] */ IParcelFileDescriptor* oldState,
        /* [in] */ IBackupDataOutput* data,
        /* [in] */ IParcelFileDescriptor* newState);
    /**
     * Run the restore process on each of the configured handlers.
     */
    //@Override
    CARAPI OnRestore(
        /* [in] */ IBackupDataInput* data,
        /* [in] */ Int32 appVersionCode,
        /* [in] */ IParcelFileDescriptor* newState);

    /** @hide */
    CARAPI GetDispatcher(
        /* [out] */ IBackupHelperDispatcher** dispatcher);

    /**
     * Add a helper for a given data subset to the agent's configuration.  Each helper
     * must have a prefix string that is unique within this backup agent's set of
     * helpers.
     *
     * @param keyPrefix A string used to disambiguate the various helpers within this agent
     * @param helper A backup/restore helper object to be invoked during backup and restore
     *    operations.
     */
    CARAPI AddHelper(
        /* [in] */ const String& keyPrefix,
        /* [in] */ IBackupHelper* helper);

private:
    static CString TAG;

    AutoPtr<IBackupHelperDispatcher> mDispatcher;
};

}
}
}
}
#endif  //__ELASTOS_DROID_APP_BACKUP_BACKUPAGENTHELPER_H__
