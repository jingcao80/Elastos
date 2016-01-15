
#include <elastos/droid/server/content/SyncQueue.h>
#include <elastos/droid/server/content/SyncOperation.h>
#include <elastos/droid/server/content/SyncManager.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/content/pm/RegisteredServicesCache.h>
#include <elastos/droid/text/format/DateUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Content::ISyncAdapterType;
using Elastos::Droid::Content::ISyncAdapterTypeHelper;
using Elastos::Droid::Content::CSyncAdapterTypeHelper;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Pm::IRegisteredServicesCacheServiceInfo;
using Elastos::Droid::Content::Pm::IRegisteredServicesCache;
using Elastos::Droid::Text::Format::DateUtils;
using Elastos::Core::IComparable;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {

const String SyncQueue::TAG("SyncManager");

SyncQueue::SyncQueue(
    /* [in] */ IPackageManager* packageManager,
    /* [in] */ SyncStorageEngine* syncStorageEngine,
    /* [in] */ ISyncAdaptersCache* syncAdapters)
{
    mOperationsMap = new HashMap<String, AutoPtr<SyncOperation> >();
    mPackageManager = packageManager;
    mSyncStorageEngine = syncStorageEngine;
    mSyncAdapters = syncAdapters;
}

void SyncQueue::AddPendingOperations(
    /* [in] */ Int32 userId)
{
    AutoPtr<List<AutoPtr<PendingOperation> > > list = mSyncStorageEngine->GetPendingOperations();
    List<AutoPtr<PendingOperation> >::Iterator it;
    PendingOperation* op;
    for (it = list->Begin(); it != list->End(); ++it) {
        op = *it;
        AutoPtr<EndPoint> info = op->mTarget;
        if (info->mUserId != userId) continue;

        AutoPtr<Pair<Int64, Int64> > backoff = mSyncStorageEngine->GetBackoff(info);
        AutoPtr<SyncOperation> operationToAdd;
        if (info->mTarget_provider) {
            AutoPtr<ISyncAdapterTypeHelper> typeHelper;
            CSyncAdapterTypeHelper::AcquireSingleton((ISyncAdapterTypeHelper**)&typeHelper);
            String accountType;
            info->mAccount->GetType(&accountType);
            AutoPtr<ISyncAdapterType> adapterType;
            typeHelper->NewKey(info->mProvider, accountType, (ISyncAdapterType**)&adapterType);

            AutoPtr<IRegisteredServicesCacheServiceInfo> syncAdapterInfo;
            IRegisteredServicesCache::Probe(mSyncAdapters)->GetServiceInfo(TO_IINTERFACE(adapterType), info->mUserId,
                (IRegisteredServicesCacheServiceInfo**)&syncAdapterInfo);
            if (syncAdapterInfo.Get() == NULL) {
                // if (Log.isLoggable(TAG, Log.VERBOSE)) {
                //     Log.v(TAG, "Missing sync adapter info for authority " + op.target);
                // }
                continue;
            }

            AutoPtr<IInterface> typeObj;
            syncAdapterInfo->GetType((IInterface**)&typeObj);
            ISyncAdapterType* type = ISyncAdapterType::Probe(typeObj);
            Boolean bval;
            type->AllowParallelSyncs(&bval);
            operationToAdd = new SyncOperation(
                info->mAccount, info->mUserId, op->mReason, op->mSyncSource, info->mProvider,
                op->mExtras,
                op->mExpedited ? -1 : 0 /* delay */,
                0 /* flex */,
                backoff != NULL ? backoff->mFirst : 0,
                mSyncStorageEngine->GetDelayUntilTime(info),
                bval);
            operationToAdd->mPendingOperation = op;
            Add(operationToAdd, op);
        }
        else if (info->mTarget_service) {
            AutoPtr<IServiceInfo> si;
            ECode ec = mPackageManager->GetServiceInfo(info->mService, 0, (IServiceInfo**)&si);
            if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
                // if (Log.isLoggable(TAG, Log.VERBOSE)) {
                //     Log.w(TAG, "Missing sync service for authority " + op.target);
                // }
                continue;
            }
            operationToAdd = new SyncOperation(
                    info->mService, info->mUserId, op->mReason, op->mSyncSource,
                    op->mExtras,
                    op->mExpedited ? -1 : 0 /* delay */,
                    0 /* flex */,
                    backoff != NULL ? backoff->mFirst : 0,
                    mSyncStorageEngine->GetDelayUntilTime(info));
            operationToAdd->mPendingOperation = op;
            Add(operationToAdd, op);
        }
    }
}

Boolean SyncQueue::Add(
    /* [in] */ SyncOperation* operation)
{
    return Add(operation, NULL /* this is not coming from the database */);
}

Boolean SyncQueue::Add(
    /* [in] */ SyncOperation* operation,
    /* [in] */ PendingOperation* pop)
{
    // If an operation with the same key exists and this one should run sooner/overlaps,
    // replace the run Int32erval of the existing operation with this new one.
    // Complications: what if the existing operation is expedited but the new operation has an
    // earlier run time? Will not be a problem for periodic syncs (no expedited flag), and for
    // one-off syncs we only change it if the new sync is sooner.
    String operationKey = operation->mKey;
    HashMap<String, AutoPtr<SyncOperation> >::Iterator it = mOperationsMap->Find(operationKey);
    AutoPtr<SyncOperation> existingOperation;
    if (it != mOperationsMap->End()) {
        existingOperation = it->mSecond;
    }

    if (existingOperation != NULL) {
        Boolean changed = FALSE;
        Int32 ival;
        IComparable::Probe(operation)->CompareTo(TO_IINTERFACE(existingOperation), &ival);
        if (ival <= 0 ) {
            Int64 newRunTime =
                Elastos::Core::Math::Min(existingOperation->mLatestRunTime, operation->mLatestRunTime);
            // Take smaller runtime.
            existingOperation->mLatestRunTime = newRunTime;
            // Take newer flextime.
            existingOperation->mFlexTime = operation->mFlexTime;
            changed = TRUE;
        }
        return changed;
    }

    operation->mPendingOperation = pop;
    // Don't update the PendingOp if one already exists. This really is just a placeholder,
    // no actual scheduling info is placed here.
    if (operation->mPendingOperation == NULL) {
        pop = mSyncStorageEngine->InsertIntoPending(operation);
        if (pop == NULL) {
            Logger::E(TAG, "error adding pending sync operation ");
            // return E_ILLEGAL_STATE_EXCEPTION;
            return FALSE;
        }
        operation->mPendingOperation = pop;
    }

    (*mOperationsMap)[operationKey] = operation;
    return TRUE;
}

void SyncQueue::RemoveUserLocked(
    /* [in] */ Int32 userId)
{
    List<AutoPtr<SyncOperation> > opsToRemove;
    HashMap<String, AutoPtr<SyncOperation> >::Iterator it;
    for (it = mOperationsMap->Begin(); it != mOperationsMap->End(); ++it) {
        AutoPtr<SyncOperation> op = it->mSecond;
        if (op->mTarget->mUserId == userId) {
            opsToRemove.PushBack(op);
        }
    }

    List<AutoPtr<SyncOperation> >::Iterator lit;
    for (lit = opsToRemove.Begin(); lit != opsToRemove.End(); ++lit) {
        Remove(*lit);
    }
}

void SyncQueue::Remove(
    /* [in] */ SyncOperation* operation)
{
    // Boolean isLoggable = Log.isLoggable(TAG, Log.VERBOSE);
    HashMap<String, AutoPtr<SyncOperation> >::Iterator it;
    it = mOperationsMap->Find(operation->mKey);
    AutoPtr<SyncOperation> operationToRemove;
    if (it != mOperationsMap->End()) {
        operationToRemove = it->mSecond;
        mOperationsMap->Erase(it);
    }
    // if (isLoggable) {
    //     Log.v(TAG, "Attempting to remove: " + operation.key);
    // }
    if (operationToRemove == NULL) {
        // if (isLoggable) {
        //     Log.v(TAG, "Could not find: " + operation.key);
        // }
        return;
    }
    if (!mSyncStorageEngine->DeleteFromPending(operationToRemove->mPendingOperation)) {
        Logger::E(TAG, "unable to find pending row for %s",
            Object::ToString(operationToRemove).string());
    }
}

void SyncQueue::ClearBackoffs()
{
    HashMap<String, AutoPtr<SyncOperation> >::Iterator it;
    for (it = mOperationsMap->Begin(); it != mOperationsMap->End(); ++it) {
        AutoPtr<SyncOperation> op = it->mSecond;
        op->mBackoff = 0;
        op->UpdateEffectiveRunTime();
    }
}

void SyncQueue::OnBackoffChanged(
    /* [in] */ EndPoint* target,
    /* [in] */ Int64 backoff)
{
    // For each op that matches the target of the changed op, update its
    // backoff and effectiveStartTime
    HashMap<String, AutoPtr<SyncOperation> >::Iterator it;
    for (it = mOperationsMap->Begin(); it != mOperationsMap->End(); ++it) {
        AutoPtr<SyncOperation> op = it->mSecond;
        if (op->mTarget->MatchesSpec(target)) {
            op->mBackoff = backoff;
            op->UpdateEffectiveRunTime();
        }
    }
}

void SyncQueue::OnDelayUntilTimeChanged(
    /* [in] */ EndPoint* target,
    /* [in] */ Int64 delayUntil)
{
    // for each op that matches the target info of the provided op, change the delay time.
    HashMap<String, AutoPtr<SyncOperation> >::Iterator it;
    for (it = mOperationsMap->Begin(); it != mOperationsMap->End(); ++it) {
        AutoPtr<SyncOperation> op = it->mSecond;
        if (op->mTarget->MatchesSpec(target)) {
            op->mDelayUntil = delayUntil;
            op->UpdateEffectiveRunTime();
        }
    }
}

void SyncQueue::Remove(
    /* [in] */ EndPoint* info,
    /* [in] */ IBundle* extras)
{
    HashMap<String, AutoPtr<SyncOperation> >::Iterator it;
    for (it = mOperationsMap->Begin(); it != mOperationsMap->End(); ) {
        AutoPtr<SyncOperation> syncOperation = it->mSecond;
        AutoPtr<EndPoint> opInfo = syncOperation->mTarget;
        if (!opInfo->MatchesSpec(info)) {
            ++it;
            continue;
        }
        if (extras != NULL
                && !SyncManager::SyncExtrasEquals(
                    syncOperation->mExtras,
                    extras,
                    FALSE /* no config flags*/)) {
            ++it;
            continue;
        }
        mOperationsMap->Erase(it++);

        if (!mSyncStorageEngine->DeleteFromPending(syncOperation->mPendingOperation)) {
            Logger::E(TAG, "unable to find pending row for %s",
                Object::ToString(syncOperation).string());
        }
    }
}

AutoPtr< HashMap<String, AutoPtr<SyncOperation> > > SyncQueue::GetOperations()
{
    return mOperationsMap;
}

void SyncQueue::Dump(
    /* [in] */ StringBuilder* sb)
{
    Int64 now = SystemClock::GetElapsedRealtime();
    sb->Append("SyncQueue: ");
    sb->Append((Int32)mOperationsMap->GetSize());
    sb->Append(" operation(s)\n");
    HashMap<String, AutoPtr<SyncOperation> >::Iterator it = mOperationsMap->Begin();
    for (; it != mOperationsMap->End(); ++it) {
        SyncOperation* operation = it->mSecond;
        sb->Append("  ");
        if (operation->mEffectiveRunTime <= now) {
            sb->Append("READY");
        }
        else {
            sb->Append(DateUtils::FormatElapsedTime((operation->mEffectiveRunTime - now) / 1000));
        }
        sb->Append(" - ");
        sb->Append(operation->Dump(mPackageManager, FALSE));
        sb->Append("\n");
    }
}


} // namespace Content
} // namespace Server
} // namespace Droid
} // namespace Elastos
