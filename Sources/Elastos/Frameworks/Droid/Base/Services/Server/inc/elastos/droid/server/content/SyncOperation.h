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

#ifndef __ELASTOS_DROID_SERVER_CONTENT_SYNC_OPERATION_H__
#define __ELASTOS_DROID_SERVER_CONTENT_SYNC_OPERATION_H__

#include <elastos/droid/ext/frameworkext.h>
#include "_Elastos.Droid.Server.h"
#include <elastos/droid/server/content/SyncStorageEngine.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Core::IComparable;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {

/**
 * Value type that represents a sync operation.
 * TODO: This is the class to flesh out with all the scheduling data - metered/unmetered,
 * transfer-size, etc.
 * {@hide}
 */
class SyncOperation
    : public Object
    , public ISyncOperation
    , public IComparable
{
public:
    CAR_INTERFACE_DECL()

    SyncOperation(
        /* [in] */ IAccount* account,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 reason,
        /* [in] */ Int32 source,
        /* [in] */ const String& provider,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 runTimeFromNow,
        /* [in] */ Int64 flexTime,
        /* [in] */ Int64 backoff,
        /* [in] */ Int64 delayUntil,
        /* [in] */ Boolean allowParallelSyncs);

    SyncOperation(
        /* [in] */ IComponentName* service,
        /* [in] */ Int32 userId,
        /* [in] */ Int32 reason,
        /* [in] */ Int32 source,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 runTimeFromNow,
        /* [in] */ Int64 flexTime,
        /* [in] */ Int64 backoff,
        /* [in] */ Int64 delayUntil);

    CARAPI Init(
        /* [in] */ EndPoint* info,
        /* [in] */ Int32 reason,
        /* [in] */ Int32 source,
        /* [in] */ IBundle* extras,
        /* [in] */ Int64 runTimeFromNow,
        /* [in] */ Int64 flexTime,
        /* [in] */ Int64 backoff,
        /* [in] */ Int64 delayUntil,
        /* [in] */ Boolean allowParallelSyncs);

    /** Used to reschedule a sync at a new point in time. */
    SyncOperation(
        /* [in] */ SyncOperation* other,
        /* [in] */ Int64 newRunTimeFromNow);

    Boolean MatchesAuthority(
        /* [in] */ SyncOperation* other);

    /**
     * Determine whether if this sync operation is running, the provided operation would conflict
     * with it.
     * Parallel syncs allow multiple accounts to be synced at the same time.
     */
    Boolean IsConflict(
        /* [in] */ SyncOperation* toRun);

    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

    String Dump(
        /* [in ]*/ IPackageManager* pm,
        /* [in] */ Boolean useOneLine);

    static String ReasonToString(
        /* [in] */ IPackageManager* pm,
        /* [in] */ Int32 reason);

    Boolean IsInitialization();

    Boolean IsExpedited();

    Boolean IgnoreBackoff();

    Boolean IsNotAllowedOnMetered();

    Boolean IsManual();

    Boolean IsIgnoreSettings();

    /** Changed in V3. */
    static String ToKey(
        /* [in] */ EndPoint* info,
        /* [in] */ IBundle* extras);

    String WakeLockName();

    /**
     * Update the effective run time of this Operation based on latestRunTime (specified at
     * creation time of sync), delayUntil (specified by SyncAdapter), or backoff (specified by
     * SyncManager on soft failures).
     */
    void UpdateEffectiveRunTime();

    /**
     * SyncOperations are sorted based on their earliest effective run time.
     * This comparator is used to sort the SyncOps at a given time when
     * deciding which to run, so earliest run time is the best criteria.
     */
    // @Override
    CARAPI CompareTo(
        /* [in] */ IInterface* o,
        /* [out] */ Int32* result);

    // TODO: Test this to make sure that casting to object doesn't lose the type info for EventLog.
    AutoPtr<ArrayOf<IInterface*> > ToEventLog(
        /* [in] */ Int32 event);

private:

    /**
     * Make sure the bundle attached to this SyncOperation doesn't have unnecessary
     * flags set.
     * @param bundle to clean.
     */
    void CleanBundle(
        /* [in] */ IBundle* bundle);

    void RemoveFalseExtra(
        /* [in] */ IBundle* bundle,
        /* [in] */ const String& extraName);

    static void ExtrasToStringBuilder(
        /* [in] */ IBundle* bundle,
        /* [in] */ StringBuilder& sb);

public:
    static const String TAG;

    static const Int32 REASON_BACKGROUND_DATA_SETTINGS_CHANGED;
    static const Int32 REASON_ACCOUNTS_UPDATED;
    static const Int32 REASON_SERVICE_CHANGED;
    static const Int32 REASON_PERIODIC;
    /** Sync started because it has just been set to isSyncable. */
    static const Int32 REASON_IS_SYNCABLE;
    /** Sync started because it has just been set to sync automatically. */
    static const Int32 REASON_SYNC_AUTO;
    /** Sync started because master sync automatically has been set to true. */
    static const Int32 REASON_MASTER_SYNC_AUTO;
    static const Int32 REASON_USER_START;

    static const Int32 SYNC_TARGET_UNKNOWN;
    static const Int32 SYNC_TARGET_ADAPTER;
    static const Int32 SYNC_TARGET_SERVICE;

    /** Identifying info for the target for this operation. */
    AutoPtr<EndPoint> mTarget;
    /** Why this sync was kicked off. {@link #REASON_NAMES} */
    Int32 mReason;
    /** Where this sync was initiated. */
    Int32 mSyncSource;
    Boolean mAllowParallelSyncs;
    String mKey;
    /** Internal boolean to avoid reading a bundle everytime we want to compare operations. */
    Boolean mExpedited;
    AutoPtr<IBundle> mExtras;
    /** Bare-bones version of this operation that is persisted across reboots. */
    AutoPtr<PendingOperation> mPendingOperation;
    /** Elapsed real time in millis at which to run this sync. */
    Int64 mLatestRunTime;
    /** Set by the SyncManager in order to delay retries. */
    Int64 mBackoff;
    /** Specified by the adapter to delay subsequent sync operations. */
    Int64 mDelayUntil;
    /**
     * Elapsed real time in millis when this sync will be run.
     * Depends on max(backoff, latestRunTime, and delayUntil).
     */
    Int64 mEffectiveRunTime;
    /** Amount of time before {@link #effectiveRunTime} from which this sync can run. */
    Int64 mFlexTime;

    /** Descriptive string key for this operation */
    String mWakeLockName;

private:
    static AutoPtr<ArrayOf<String> > REASON_NAMES;
};

} // namespace Content
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_SERVER_CONTENT_SYNC_OPERATION_H__
