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

#ifndef __ELASTOS_DROID_CONTENT_CSYNCRESULT_H__
#define __ELASTOS_DROID_CONTENT_CSYNCRESULT_H__

#include "_Elastos_Droid_Content_CSyncResult.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CSyncResult)
    , public Object
    , public ISyncResult
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSyncResult();

    ~CSyncResult();

    /**
     * Used to indicate that the SyncAdapter is already performing a sync operation, though
     * not necessarily for the requested account and authority and that it wasn't able to
     * process this request. The SyncManager will reschedule the request to run later.
     */
    CARAPI GetSyncAlreadyInProgress(
        /* [out] */ Boolean* SyncAlreadyInProgress);

    /**
     * Used to indicate that the SyncAdapter determined that it would need to issue
     * too many delete operations to the server in order to satisfy the request
     * (as defined by the SyncAdapter). The SyncManager will record
     * that the sync request failed and will cause a System Notification to be created
     * asking the user what they want to do about this. It will give the user a chance to
     * choose between (1) go ahead even with those deletes, (2) revert the deletes,
     * or (3) take no action. If the user decides (1) or (2) the SyncManager will issue another
     * sync request with either {@link ContentResolver#SYNC_EXTRAS_OVERRIDE_TOO_MANY_DELETIONS}
     * or {@link ContentResolver#SYNC_EXTRAS_DISCARD_LOCAL_DELETIONS} set in the extras.
     * It is then up to the SyncAdapter to decide how to honor that request.
     */
    CARAPI GetTooManyDeletions(
        /* [out] */ Boolean* TooManyDeletions);

    CARAPI SetTooManyDeletions(
        /* [in] */ Boolean tooManyDeletions);

    /**
     * Used to indicate that the SyncAdapter experienced a hard error due to trying the same
     * operation too many times (as defined by the SyncAdapter). The SyncManager will record
     * that the sync request failed and it will not reschedule the request.
     */
    CARAPI GetTooManyRetries(
        /* [out] */ Boolean* TooManyRetries);

    CARAPI SetTooManyRetries(
        /* [in] */ Boolean tooManyRetries);

    /**
     * Used to indicate that the SyncAdapter experienced a hard error due to an error it
     * received from interacting with the storage later. The SyncManager will record that
     * the sync request failed and it will not reschedule the request.
     */
    CARAPI GetDatabaseError(
        /* [out] */ Boolean* DatabaseError);

    CARAPI SetDatabaseError(
        /* [in] */ Boolean databaseError);

    /**
     * If set the SyncManager will request an immediate sync with the same Account and authority
     * (but empty extras Bundle) as was used in the sync request.
     */
    CARAPI GetFullSyncRequested(
        /* [out] */ Boolean* FullSyncRequested);

    CARAPI SetFullSyncRequested(
        /* [in] */ Boolean fullSyncRequested);

    /**
     * This field is ignored by the SyncManager.
     */
    CARAPI GetPartialSyncUnavailable(
        /* [out] */ Boolean* partialSyncUnavailable);

    CARAPI SetPartialSyncUnavailable(
        /* [in] */ Boolean partialSyncUnavailable);

    /**
     * This field is ignored by the SyncManager.
     */
    CARAPI GetMoreRecordsToGet(
        /* [out] */ Boolean* moreRecordsToGet);

    CARAPI SetMoreRecordsToGet(
        /* [in] */ Boolean moreRecordsToGet);

    /**
     * Used to indicate to the SyncManager that future sync requests that match the request's
     * Account and authority should be delayed at least this many seconds.
     */
    CARAPI GetDelayUntil(
        /* [out] */ Int64* delayUntil);

    CARAPI SetDelayUntil(
        /* [in] */ Int64 delayUntil);

    /**
     * Used to hold extras statistics about the sync operation. Some of these indicate that
     * the sync request resulted in a hard or soft error, others are for purely informational
     * purposes.
     */
    CARAPI GetStats(
        /* [out] */ ISyncStats** stats);

    /**
     * Convenience method for determining if the SyncResult indicates that a hard error
     * occurred. See {@link #SyncResult()} for an explanation of what the SyncManager does
     * when it sees a hard error.
     * <p>
     * A hard error is indicated when any of the following is true:
     * <ul>
     * <li> {@link SyncStats#numParseExceptions} > 0
     * <li> {@link SyncStats#numConflictDetectedExceptions} > 0
     * <li> {@link SyncStats#numAuthExceptions} > 0
     * <li> {@link #tooManyDeletions}
     * <li> {@link #tooManyRetries}
     * <li> {@link #databaseError}
     * @return true if a hard error is indicated
     */
    CARAPI HasHardError(
        /* [out] */ Boolean* hasError);

    /**
     * Convenience method for determining if the SyncResult indicates that a soft error
     * occurred. See {@link #SyncResult()} for an explanation of what the SyncManager does
     * when it sees a soft error.
     * <p>
     * A soft error is indicated when any of the following is true:
     * <ul>
     * <li> {@link SyncStats#numIoExceptions} > 0
     * <li> {@link #syncAlreadyInProgress}
     * </ul>
     * @return true if a hard error is indicated
     */
    CARAPI HasSoftError(
        /* [out] */ Boolean* hasError);

    /**
     * A convenience method for determining of the SyncResult indicates that an error occurred.
     * @return true if either a soft or hard error occurred
     */
    CARAPI HasError(
        /* [out] */ Boolean* hasError);

    CARAPI MadeSomeProgress(
        /* [out] */ Boolean* result);

    /**
     * Clears the SyncResult to a clean state. Throws an {@link UnsupportedOperationException}
     * if this is called when {@link #syncAlreadyInProgress} is set.
     */
    CARAPI Clear();

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Generates a debugging string indicating the status.
     * The string consist of a sequence of code letter followed by the count.
     * Code letters are f - fullSyncRequested, r - partialSyncUnavailable,
     * X - hardError, e - numParseExceptions, c - numConflictDetectedExceptions,
     * a - numAuthExceptions, D - tooManyDeletions, R - tooManyRetries,
     * b - databaseError, x - softError, l - syncAlreadyInProgress,
     * I - numIoExceptions
     * @return debugging string.
     */
    CARAPI ToDebugString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Create a "clean" SyncResult. If this is returned without any changes then the
     * SyncManager will consider the sync to have completed successfully. The various fields
     * can be set by the SyncAdapter in order to give the SyncManager more information as to
     * the disposition of the sync.
     * <p>
     * The errors are classified into two broad categories: hard errors and soft errors.
     * Soft errors are retried with exponential backoff. Hard errors are not retried (except
     * when the hard error is for a {@link ContentResolver#SYNC_EXTRAS_UPLOAD} request,
     * in which the request is retryed without the {@link ContentResolver#SYNC_EXTRAS_UPLOAD}
     * extra set). The SyncManager checks the type of error by calling
     * {@link SyncResult#hasHardError()} and  {@link SyncResult#hasSoftError()}. If both are
     * true then the SyncManager treats it as a hard error, not a soft error.
     */
    CARAPI constructor();

    /**
     * Internal helper for creating a clean SyncResult or one that indicated that
     * a sync is already in progress.
     * @param syncAlreadyInProgress if true then set the {@link #syncAlreadyInProgress} flag
     */
    CARAPI constructor(
        /* [in] */ Boolean syncAlreadyInProgress);

public:
    static const AutoPtr<ISyncResult> ALREADY_IN_PROGRESS;

private:
    /**
     * Used to indicate that the SyncAdapter is already performing a sync operation, though
     * not necessarily for the requested account and authority and that it wasn't able to
     * process this request. The SyncManager will reschedule the request to run later.
     */
    Boolean mSyncAlreadyInProgress;

    /**
     * Used to indicate that the SyncAdapter determined that it would need to issue
     * too many delete operations to the server in order to satisfy the request
     * (as defined by the SyncAdapter). The SyncManager will record
     * that the sync request failed and will cause a System Notification to be created
     * asking the user what they want to do about this. It will give the user a chance to
     * choose between (1) go ahead even with those deletes, (2) revert the deletes,
     * or (3) take no action. If the user decides (1) or (2) the SyncManager will issue another
     * sync request with either {@link ContentResolver#SYNC_EXTRAS_OVERRIDE_TOO_MANY_DELETIONS}
     * or {@link ContentResolver#SYNC_EXTRAS_DISCARD_LOCAL_DELETIONS} set in the extras.
     * It is then up to the SyncAdapter to decide how to honor that request.
     */
    Boolean mTooManyDeletions;

    /**
     * Used to indicate that the SyncAdapter experienced a hard error due to trying the same
     * operation too many times (as defined by the SyncAdapter). The SyncManager will record
     * that the sync request failed and it will not reschedule the request.
     */
    Boolean mTooManyRetries;

    /**
     * Used to indicate that the SyncAdapter experienced a hard error due to an error it
     * received from interacting with the storage later. The SyncManager will record that
     * the sync request failed and it will not reschedule the request.
     */
    Boolean mDatabaseError;

    /**
     * If set the SyncManager will request an immediate sync with the same Account and authority
     * (but empty extras Bundle) as was used in the sync request.
     */
    Boolean mFullSyncRequested;

    /**
     * This field is ignored by the SyncManager.
     */
    Boolean mPartialSyncUnavailable;

    /**
     * This field is ignored by the SyncManager.
     */
    Boolean mMoreRecordsToGet;

    /**
     * Used to indicate to the SyncManager that future sync requests that match the request's
     * Account and authority should be delayed at least this many seconds.
     */
    Int64 mDelayUntil;

    /**
     * Used to hold extras statistics about the sync operation. Some of these indicate that
     * the sync request resulted in a hard or soft error, others are for purely informational
     * purposes.
     */
    AutoPtr<ISyncStats> mStats;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSYNCRESULT_H__
