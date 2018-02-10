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

#include "CActiveRestoreSession.h"
#include "elastos/droid/Manifest.h"
#include "elastos/droid/os/Binder.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String CActiveRestoreSession::TAG("CActiveRestoreSession");
const Boolean CActiveRestoreSession::DEBUG = FALSE;

ECode CActiveRestoreSession::constructor(
    /* [in] */ const String &packageName,
    /* [in] */ const String &transport,
    /* [in] */ HANDLE host)
{
    mPackageName = packageName;
    mHost = (CBackupManagerService*)host;
    mRestoreTransport = mHost->GetTransport(transport);
    mEnded = FALSE;
    return NOERROR;
}

ECode CActiveRestoreSession::GetAvailableRestoreSets(
    /* [in] */ IIRestoreObserver* observer,
    /* [out] */ Int32* result)
{
    {
        AutoLock lock(mActivieRSLock);
        VALIDATE_NOT_NULL(result);

        *result = 0;
        mHost->mContext->EnforceCallingOrSelfPermission(
                Elastos::Droid::Manifest::permission::BACKUP,
                String("getAvailableRestoreSets"));
        if (observer == NULL) {
            //throw new IllegalArgumentException("Observer must not be NULL");
            return E_INVALID_ARGUMENT;
        }

        if (mEnded) {
            // throw new IllegalStateException("Restore session already ended");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        Int64 oldId = Binder::ClearCallingIdentity();
        //try {
        if (mRestoreTransport == NULL) {
            Slogger::W(TAG, "Null transport getting restore sets");
            *result = -1;
            Binder::RestoreCallingIdentity(oldId);
            return NOERROR;
        }

        // spin off the transport request to our service thread
        mHost->mWakelock->AcquireLock();

        AutoPtr<CBackupManagerService::RestoreGetSetsParams> data = new CBackupManagerService::RestoreGetSetsParams(mRestoreTransport, this, observer);

        AutoPtr<IMessage> msg;
        ECode ec = NOERROR;
        Boolean bTemp;
        FAIL_GOTO((ec = mHost->mBackupHandler->ObtainMessage(CBackupManagerService::MSG_RUN_GET_RESTORE_SETS, (IMessage**)&msg)), _Exit_);
        FAIL_GOTO((ec = msg->SetObj(data)), _Exit_);
        FAIL_GOTO((ec = mHost->mBackupHandler->SendMessage(msg, &bTemp)), _Exit_);

        // } catch (Exception e) {
        //     Slog.e(TAG, "Error in getAvailableRestoreSets", e);
        //     return -1;
        // } finally {
_Exit_:
        Binder::RestoreCallingIdentity(oldId);
        if (FAILED(ec)) {
            Slogger::E(TAG, "Error in getAvailableRestoreSets");
            *result = -1;
            return ec;
        }
        // }
    }

    return NOERROR;
}

ECode CActiveRestoreSession::RestoreAll(
    /* [in] */ Int64 token,
    /* [in] */ IIRestoreObserver* observer,
    /* [out] */ Int32* value)
{
    AutoLock lock(mActivieRSLock);
    VALIDATE_NOT_NULL(value);
    *value = 0;
    mHost->mContext->EnforceCallingOrSelfPermission(
            Elastos::Droid::Manifest::permission::BACKUP,
            String("performRestore"));

    if (DEBUG) Slogger::D(TAG, "restoreAll token= %d, observer", token);

    if (mEnded) {
        // throw new IllegalStateException("Restore session already ended");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mRestoreTransport == NULL || mRestoreSets == NULL) {
        Slogger::E(TAG, "Ignoring restoreAll() with no restore set");
        *value = -1;
        return NOERROR;
    }

    if (!mPackageName.IsNull()) {
        Slogger::E(TAG, "Ignoring restoreAll() on single-package session");
        *value = -1;
        return NOERROR;
    }

    {
        AutoLock lock(mHost->mQueueLock);
        if (mRestoreSets != NULL) {
            Int64 tempToken = 0;
            for (Int32 i = 0; i < mRestoreSets->GetLength(); i++) {
                AutoPtr<IRestoreSet> set = (*mRestoreSets)[i];
                assert(set != NULL);
                set->GetToken(&tempToken);
                if (tempToken == token) {
                    Int64 oldId = Binder::ClearCallingIdentity();

                    //TODO
                    mHost->mWakelock->AcquireLock();

                    AutoPtr<CBackupManagerService::RestoreParams> data = new CBackupManagerService::RestoreParams(mRestoreTransport, observer, token, TRUE);
                    AutoPtr<IMessage> msg;
                    mHost->mBackupHandler->ObtainMessage(CBackupManagerService::MSG_RUN_GET_RESTORE_SETS, (IMessage**)&msg);
                    msg->SetObj(data);
                    Boolean result;
                    mHost->mBackupHandler->SendMessage(msg, &result);

                    Binder::RestoreCallingIdentity(oldId);
                    return NOERROR;
                }
            }
        }
    }

    Slogger::W(TAG, "Restore token %d not found", token);
    *value = -1;
    return NOERROR;
}

ECode CActiveRestoreSession::RestoreSome(
    /* [in] */ Int64 token,
    /* [in] */ IIRestoreObserver* observer,
    /* [in] */ const ArrayOf<String>& packages,
    /* [out] */ Int32* success)
{
    VALIDATE_NOT_NULL(success);
    mHost->mContext->EnforceCallingOrSelfPermission(Elastos::Droid::Manifest::permission::BACKUP,
            String("performRestore"));

    if (DEBUG) {
        AutoPtr<StringBuilder> b = new StringBuilder(128);

        b->AppendCStr("restoreSome token=");
        b->AppendString(StringUtils::Int64ToHexString(token));
        b->AppendCStr(" observer=");
        b->AppendCStr("observer");
        b->AppendCStr(" packages=");
        if (packages.GetPayload() == NULL) {
            b->AppendCStr("null");
        } else {
            b->AppendChar('{');
            Boolean first = TRUE;
            for (int i = 0; i < packages.GetLength(); ++i) {
                if (!first) {
                    b->AppendCStr(", ");
                }
                else {
                    first = FALSE;
                }
                b->AppendString(packages[i]);
            }
            b->AppendChar('}');
        }
        Slogger::D(TAG, "%s", b->ToString().string());
    }

    if (mEnded) {
        // throw new IllegalStateException("Restore session already ended");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (mRestoreTransport == NULL || mRestoreSets == NULL) {
        Slogger::E(TAG, "Ignoring restoreAll() with no restore set");
        *success = -1;
        return NOERROR;
    }

    if (mPackageName != NULL) {
        Slogger::E(TAG, "Ignoring restoreAll() on single-package session");
        *success = -1;
        return NOERROR;
    }

    {
        AutoLock lock(mHost->mQueueLock);
        Int64 tempToken;
        for (Int32 i = 0; i < mRestoreSets->GetLength(); ++i) {
            (*mRestoreSets)[i]->GetToken(&tempToken);
            if (token == tempToken) {
                Int64 oldId = Binder::ClearCallingIdentity();
                mHost->mWakelock->AcquireLock();
                AutoPtr<IMessage> msg;
                AutoPtr<CBackupManagerService::RestoreParams> data = new CBackupManagerService::RestoreParams(mRestoreTransport, observer, token, (ArrayOf<String>*)&packages,  TRUE);
                mHost->mBackupHandler->ObtainMessage(CBackupManagerService::MSG_RUN_RESTORE, (IMessage**)&msg);
                msg->SetObj(data);
                Boolean result;
                mHost->mBackupHandler->SendMessage(msg, &result);
                Binder::RestoreCallingIdentity(oldId);
                return 0;
            }
        }
    }

    Slogger::W(TAG, "Restore token %s not found", StringUtils::Int64ToHexString(token).string());
    *success = -1;
    return NOERROR;
}

ECode CActiveRestoreSession::RestorePackage(
    /* [in] */ const String &packageName,
    /* [in] */ IIRestoreObserver* observer,
    /* [out] */ Int32* value)
{
    {
        AutoLock lock(mActivieRSLock);
        VALIDATE_NOT_NULL(value);
        *value = 0;

        if (DEBUG) Slogger::V(TAG, "restorePackage pkg= %s", packageName.string());

        if (mEnded) {
            // throw new IllegalStateException("Restore session already ended");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        if (!mPackageName.IsNull()) {
            if (! mPackageName.Equals(packageName)) {
                Slogger::E(TAG, "Ignoring attempt to restore pkg= %s on session for package %s", packageName.string(), mPackageName.string());
                *value = -1;
                return NOERROR;
            }
        }

        AutoPtr<IPackageInfo> app;
        //try {
        ECode ec = mHost->mPackageManager->GetPackageInfo(packageName, 0, (IPackageInfo**)&app);
        if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
            Slogger::W(TAG, "Asked to restore nonexistent pkg %s", packageName.string());
            *value = -1;
            return ec;
        }
        // } catch (NameNotFoundException nnf) {
        //     Slog.w(TAG, "Asked to restore nonexistent pkg " + packageName);
        //     *error = -1;
        //     return NOERROR;
        // }

        // If the caller is not privileged and is not coming from the target
        // app's uid, throw a permission exception back to the caller.
        Int32 perm = 0;
        FAIL_RETURN(mHost->mContext->CheckPermission(Elastos::Droid::Manifest::permission::BACKUP,
                Binder::GetCallingPid(), Binder::GetCallingUid(), &perm));

        AutoPtr<IApplicationInfo> info;
        app->GetApplicationInfo((IApplicationInfo**)&info);
        assert(info != NULL);
        Int32 uid = 0;
        info->GetUid(&uid);

        if ((perm == IPackageManager::PERMISSION_DENIED) &&
                (uid != Binder::GetCallingUid())) {
            Slogger::W(TAG, "restorePackage: bad packageName= %s or calling uid=%d", packageName.string(), Binder::GetCallingUid());
            //throw new SecurityException("No permission to restore other packages");
            return E_SECURITY_EXCEPTION;
        }

        String backupAgentName;
        info->GetBackupAgentName(&backupAgentName);
        // If the package has no backup agent, we obviously cannot proceed
        if (backupAgentName.IsNull()) {
            Slogger::W(TAG, "Asked to restore package %s with no agent", packageName.string());

            *value = -1;
            return NOERROR;
        }

        // So far so good; we're allowed to try to restore this package.  Now
        // check whether there is data for it in the current dataset, falling back
        // to the ancestral dataset if not.
        Int64 token = mHost->GetAvailableRestoreToken(packageName);

        // If we didn't come up with a place to look -- no ancestral dataset and
        // the app has never been backed up from this device -- there's nothing
        // to do but return failure.
        if (token == 0) {
            if (DEBUG) Slogger::W(TAG, "No data available for this package; not restoring");
            *value = -1;
            return NOERROR;
        }

        // Ready to go:  enqueue the restore request and claim success
        Int64 oldId = Binder::ClearCallingIdentity();

        mHost->mWakelock->AcquireLock();

        AutoPtr<CBackupManagerService::RestoreParams> data = new CBackupManagerService::RestoreParams(mRestoreTransport, observer, token, app, 0, FALSE);
        AutoPtr<IMessage> msg;
        mHost->mBackupHandler->ObtainMessage(CBackupManagerService::MSG_RUN_RESTORE, (IMessage**)&msg);
        msg->SetObj(data);
        Boolean result;
        mHost->mBackupHandler->SendMessage(msg, &result);

        Binder::RestoreCallingIdentity(oldId);
    }
    *value = 0;
    return NOERROR;
}

ECode CActiveRestoreSession::EndRestoreSession()
{
    if (DEBUG) Slogger::D(TAG, "endRestoreSession");

    if (mEnded) {
        // throw new IllegalStateException("Restore session already ended");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IRunnable> runnable = new EndRestoreRunnable(mHost, this);
    Boolean temp;
    mHost->mBackupHandler->Post(runnable, &temp);

    return NOERROR;
}

//============================================================================
// CActiveRestoreSession::EndRestoreRunnable
//============================================================================
// Posted to the handler to tear down a restore session in a cleanly synchronized way
CActiveRestoreSession::EndRestoreRunnable::EndRestoreRunnable(
    /* [in] */ CBackupManagerService* manager,
    /* [in] */ CActiveRestoreSession* session)
    : mBackupManager(manager)
    , mSession(session)
{}

ECode CActiveRestoreSession::EndRestoreRunnable::Run()
{
    // clean up the session's bookkeeping
    {
        AutoLock lock(mSessionLock);
        // TRY {
        ECode ec = NOERROR;
        if (mSession->mRestoreTransport != NULL) {
            ec = mSession->mRestoreTransport->FinishRestore();
        }
        // } catch (Exception e) {
            // Slog.e(TAG, "Error in finishRestore", e);
        // } finally {
        mSession->mRestoreTransport = NULL;
        mSession->mEnded = TRUE;
        if (FAILED(ec)) {
            Slogger::E(TAG, "Error in finishRestore");
        }
        // }
    }

    // clean up the BackupManagerService side of the bookkeeping
    // and cancel any pending timeout message
    mBackupManager->ClearRestoreSession(mSession);
    return NOERROR;
}

} //namespace Server
} //namespace Droid
} //namespace Elastos
