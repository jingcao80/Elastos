//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_SERVER_CACTIVERESTORESESSION_H__
#define __ELASTOS_DROID_SERVER_CACTIVERESTORESESSION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CActiveRestoreSession.h"
#include "CBackupManagerService.h"

using Elastos::Droid::Server::CBackupManagerService;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CActiveRestoreSession)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActiveRestoreSession() {}
    ~CActiveRestoreSession() {}

    CARAPI constructor(
        /* [in] */ const String &packageName,
        /* [in] */ const String &transport,
        /* [in] */ Handle32 host);

    /**
     * Ask the current transport what the available restore sets are.
     *
     * @param observer This binder points to an object whose onRestoreSetsAvailable()
     *   method will be called to supply the results of the transport's lookup.
     * @return Zero on success; nonzero on error.  The observer will only receive a
     *   result callback if this method returned zero.
     */
    CARAPI GetAvailableRestoreSets(
        /* [in] */ IIRestoreObserver* observer,
        /* [out] */ Int32* result);

    /**
     * Restore the given set onto the device, replacing the current data of any app
     * contained in the restore set with the data previously backed up.
     *
     * <p>Callers must hold the android.permission.BACKUP permission to use this method.
     *
     * @return Zero on success; nonzero on error.  The observer will only receive
     *   progress callbacks if this method returned zero.
     * @param token The token from getAvailableRestoreSets() corresponding to
     *   the restore set that should be used.
     *
     * @sa getAvailableRestoreSets()
     * @param observer If non-null, this binder points to an object that will receive
     *   progress callbacks during the restore operation.
     */
    CARAPI RestoreAll(
        /* [in] */ Int64 token,
        /* [in] */ IIRestoreObserver* observer,
        /* [out] */ Int32* value);

    /**
     * Restore select packages from the given set onto the device, replacing the
     * current data of any app contained in the set with the data previously
     * backed up.
     *
     * <p>Callers must hold the android.permission.BACKUP permission to use this method.
     *
     * @return Zero on success, nonzero on error. The observer will only receive
     *   progress callbacks if this method returned zero.
     * @param token The token from getAvailableRestoreSets() corresponding to
     *   the restore set that should be used.
     *
     * @sa getAvailableRestoreSets()
     * @param observer If non-null, this binder points to an object that will receive
     *   progress callbacks during the restore operation.
     * @param packages The set of packages for which to attempt a restore.  Regardless of
     *   the contents of the actual back-end dataset named by {@code token}, only
     *   applications mentioned in this list will have their data restored.
     */
    CARAPI RestoreSome(
        /* [in] */ Int64 token,
        /* [in] */ IIRestoreObserver* observer,
        /* [in] */ const ArrayOf<String>& packages,
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
        /* [in] */ const String &packageName,
        /* [in] */ IIRestoreObserver* observer,
        /* [out] */ Int32* value);

    /**
     * End this restore session.  After this method is called, the IRestoreSession binder
     * is no longer valid.
     *
     * <p><b>Note:</b> The caller <i>must</i> invoke this method to end the restore session,
     *   even if getAvailableRestoreSets or performRestore failed.
     *
     * @sa getAvailableRestoreSets
     * @sa performRestore
     */
    CARAPI EndRestoreSession();

public:

    // Posted to the handler to tear down a restore session in a cleanly synchronized way
    class EndRestoreRunnable
        : public Runnable
    {
    public:
        EndRestoreRunnable(
            /* [in] */ CBackupManagerService* manager,
            /* [in] */ CActiveRestoreSession* session);

        CARAPI Run();
    private:
        AutoPtr<CBackupManagerService> mBackupManager;
        AutoPtr<CActiveRestoreSession> mSession;
        Object mSessionLock;
    };

private:
    static const String TAG;
    static const Boolean DEBUG;

    String mPackageName;
    AutoPtr<IIBackupTransport> mRestoreTransport;
    AutoPtr<ArrayOf<IRestoreSet*> > mRestoreSets;
    Boolean mEnded;
    CBackupManagerService* mHost;
    Object mActivieRSLock;
    friend class CBackupManagerService;
};

} //namespace Server
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CACTIVERESTORESESSION_H__
