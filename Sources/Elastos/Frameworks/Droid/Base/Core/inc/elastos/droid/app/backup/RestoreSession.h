
#ifndef __ELASTOS_DROID_APP_BACKUP_RESTORESESSION_H__
#define __ELASTOS_DROID_APP_BACKUP_RESTORESESSION_H__

#include "elastos/droid/ext/frameworkext.h"


namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

class RestoreSession:
    public ElRefBase
{
public:
    RestoreSession(
        /* [in] */ IContext* context,
        /* [in] */ IRestoreSession* binder);

    ~RestoreSession();

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    /**
     * Ask the current transport what the available restore sets are.
     *
     * @param observer a RestoreObserver object whose restoreSetsAvailable() method will
     *   be called on the application's main thread in order to supply the results of
     *   the restore set lookup by the backup transport.  This parameter must not be
     *   null.
     * @return Zero on success, nonzero on error.  The observer's restoreSetsAvailable()
     *   method will only be called if this method returned zero.
     */
    CARAPI GetAvailableRestoreSets(
        /* [in] */ IRestoreObserver* observer,
        /* [out] */ Int32* error);

    /**
     * Restore the given set onto the device, replacing the current data of any app
     * contained in the restore set with the data previously backed up.
     *
     * <p>Callers must hold the android.permission.BACKUP permission to use this method.
     *
     * @return Zero on success; nonzero on error.  The observer will only receive
     *   progress callbacks if this method returned zero.
     * @param token The token from {@link #getAvailableRestoreSets()} corresponding to
     *   the restore set that should be used.
     * @param observer If non-null, this binder points to an object that will receive
     *   progress callbacks during the restore operation.
     */
    CARAPI RestoreAll(
        /* [in] */ Int64 token,
        /* [in] */ IRestoreObserver* observer,
        /* [out] */ Int32* error);

    /**
     * Restore select packages from the given set onto the device, replacing the
     * current data of any app contained in the set with the data previously
     * backed up.
     *
     * <p>Callers must hold the android.permission.BACKUP permission to use this method.
     *
     * @return Zero on success, nonzero on error. The observer will only receive
     *   progress callbacks if this method returned zero.
     * @param token The token from {@link getAvailableRestoreSets()} corresponding to
     *   the restore set that should be used.
     * @param observer If non-null, this binder points to an object that will receive
     *   progress callbacks during the restore operation.
     * @param packages The set of packages for which to attempt a restore.  Regardless of
     *   the contents of the actual back-end dataset named by {@code token}, only
     *   applications mentioned in this list will have their data restored.
     */
    CARAPI RestoreSome(
        /* [in] */ Int64 token,
        /* [in] */ IRestoreObserver* observer,
        /* [in] */ ArrayOf<String> packages,
        /* [out] */ Int32* success);

    /**
     * Restore a single application from backup.  The data will be restored from the
     * current backup dataset if the given package has stored data there, or from
     * the dataset used during the last full device setup operation if the current
     * backup dataset has no matching data.  If no backup data exists for this package
     * in either source, a nonzero value will be returned.
     *
     * @return Zero on success; nonzero on error.  The observer will only receive
     *   progress callbacks if this method returned zero.
     * @param packageName The name of the package whose data to restore.  If this is
     *   not the name of the caller's own package, then the android.permission.BACKUP
     *   permission must be held.
     * @param observer If non-null, this binder points to an object that will receive
     *   progress callbacks during the restore operation.
     */
    CARAPI RestorePackage(
        /* [in] */ const String& package,
        /* [in] */ IRestoreObserver* observer,
        /* [out] */ Int32* error);

    /**
     * End this restore session.  After this method is called, the RestoreSession
     * object is no longer valid.
     *
     * <p><b>Note:</b> The caller <i>must</i> invoke this method to end the restore session,
     *   even if {@link #restorePackage(String, RestoreObserver)} failed.
     */
    CARAPI EndRestoreSession();

private:
    //static final String TAG = "RestoreSession";
    AutoPtr<IContext> mContext;
    AutoPtr<IRestoreSession> mBinder;
    AutoPtr<IRestoreObserver>  mObserver;
};

}
}
}
}
#endif  //__ELASTOS_DROID_APP_BACKUP_RESTORESESSION_H__
