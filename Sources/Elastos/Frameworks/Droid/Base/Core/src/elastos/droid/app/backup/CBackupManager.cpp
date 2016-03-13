#include "elastos/droid/app/backup/CBackupManager.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/app/backup/CRestoreSession.h"

using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::App::Backup::CRestoreSession;

namespace Elastos {
namespace Droid {
namespace App {
namespace Backup {

const String CBackupManager::TAG("CBackupManager");
AutoPtr<IIBackupManager> CBackupManager::sService;

CAR_INTERFACE_IMPL(CBackupManager, Object, IBackupManager)

CAR_OBJECT_IMPL(CBackupManager)

CBackupManager::CBackupManager()
{
}

CBackupManager::~CBackupManager()
{
}

ECode CBackupManager::constructor(
    /* [in] */ IContext *context)
{
    mContext = context;
    return NOERROR;
}

/**
 * Notifies the Android backup system that your application wishes to back up
 * new changes to its data.  A backup operation using your application's
 * {@link android.app.backup.BackupAgent} subclass will be scheduled when you
 * call this method.
 */
ECode CBackupManager::DataChanged()
{
    CheckServiceBinder();
    if (sService != NULL) {
        assert(mContext != NULL);
        String name;
        mContext->GetPackageName(&name);
        // try {
        ECode ec = sService->DataChanged(name);
        if (FAILED(ec)) {
            Logger::D(TAG, "dataChanged() couldn't connect");
        }
        // } catch (RemoteException e) {
            // Log.d(TAG, "dataChanged() couldn't connect");
        // }
    }
    return NOERROR;
}

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
ECode CBackupManager::DataChanged(
    /* [in] */ const String &packageName)
{
    CheckServiceBinder();
    if (sService != NULL) {
        // try {
        ECode ec = sService->DataChanged(packageName);
        if (FAILED(ec)) {
            Logger::D(TAG, "dataChanged(pkg) couldn't connect");
        }
        // } catch (RemoteException e) {
            // Log.d(TAG, "dataChanged(pkg) couldn't connect");
        // }
    }
    return NOERROR;
}

/**
 * Restore the calling application from backup.  The data will be restored from the
 * current backup dataset if the application has stored data there, or from
 * the dataset used during the last full device setup operation if the current
 * backup dataset has no matching data.  If no backup data exists for this application
 * in either source, a nonzero value will be returned.
 *
 * <p>If this method returns zero (meaning success), the OS will attempt to retrieve
 * a backed-up dataset from the remote transport, instantiate the application's
 * backup agent, and pass the dataset to the agent's
 * {@link android.app.backup.BackupAgent#onRestore(BackupDataInput, int, android.os.ParcelFileDescriptor) onRestore()}
 * method.
 *
 * @param observer The {@link RestoreObserver} to receive callbacks during the restore
 * operation. This must not be NULL.
 *
 * @return Zero on success; nonzero on error.
 */
ECode CBackupManager::RequestRestore(
    /* [in] */ IRestoreObserver *observer,
    /* [out] */ Int32 *result)
{
    VALIDATE_NOT_NULL(result);
    CheckServiceBinder();
    if (sService != NULL) {
        AutoPtr<IRestoreSession> session;
        //try {
            AutoPtr<IIRestoreSession> binder;
            String name;
            mContext->GetPackageName(&name);
            ECode ec = sService->BeginRestoreSession(name,
                    String(NULL), (IIRestoreSession**)&binder);
            if (FAILED(ec)) {
                Logger::W(TAG, "restoreSelf() unable to contact service");
                return ec;
            }

            if (binder != NULL) {
                //session = new RestoreSession(mContext, binder);
                CRestoreSession::New(mContext, binder, (IRestoreSession**)&session);
                ec = session->RestorePackage(name, observer, result);
                if (FAILED(ec)) {
                    if (session != NULL) {
                        session->EndRestoreSession();
                    }
                    Logger::W(TAG, "restoreSelf() unable to contact service");
                    return ec;
                }
            }
        //} catch (RemoteException e) {
        //    Log.w(TAG, "restoreSelf() unable to contact service");
        //} finally {
            if (session != NULL) {
                session->EndRestoreSession();
            }
        //}
    }
    return NOERROR;
}

/**
 * Begin the process of restoring data from backup.  See the
 * {@link android.app.backup.RestoreSession} class for documentation on that process.
 * @hide
 */
ECode CBackupManager::BeginRestoreSession(
    /* [out] */ IRestoreSession **session)
{
    VALIDATE_NOT_NULL(session);
    *session = NULL;

    CheckServiceBinder();
    if (sService != NULL) {
        //try {
        // All packages, current transport
        AutoPtr<IIRestoreSession> binder;
        ECode ec = sService->BeginRestoreSession(String(NULL),
                String(NULL), (IIRestoreSession**)&binder);
        if (FAILED(ec)) {
            Logger::W(TAG, "beginRestoreSession() couldn't connect");
            return ec;
        }
        if (binder != NULL) {
            return CRestoreSession::New(mContext, binder, session);
        }
        //} catch (RemoteException e) {
        //    Log.w(TAG, "beginRestoreSession() couldn't connect");
        //}
    }
    return NOERROR;
}

void CBackupManager::CheckServiceBinder()
{
    if (sService == NULL) {
        AutoPtr<IInterface> service = ServiceManager::GetService(IContext::BACKUP_SERVICE);
        sService = IIBackupManager::Probe(service);
        if (sService == NULL) {
            Logger::E(TAG, "BackupManagerService start unfinished...");
        }
    }
}

}
}
}
}
