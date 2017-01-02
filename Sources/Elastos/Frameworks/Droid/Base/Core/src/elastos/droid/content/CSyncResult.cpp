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

#include "elastos/droid/content/CSyncResult.h"
#include "elastos/droid/content/CSyncStats.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringBuffer.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Content {

static AutoPtr<ISyncResult> InitALREADY_IN_PROGRESS()
{
    AutoPtr<CSyncResult> cr;
    CSyncResult::NewByFriend(TRUE, (CSyncResult**)&cr);
    return (ISyncResult*)cr.Get();
}

const AutoPtr<ISyncResult> CSyncResult::ALREADY_IN_PROGRESS = InitALREADY_IN_PROGRESS();

CAR_INTERFACE_IMPL_2(CSyncResult, Object, ISyncResult, IParcelable)

CAR_OBJECT_IMPL(CSyncResult)

CSyncResult::CSyncResult()
    : mSyncAlreadyInProgress(FALSE)
    , mTooManyDeletions(FALSE)
    , mTooManyRetries(FALSE)
    , mDatabaseError(FALSE)
    , mFullSyncRequested(FALSE)
    , mPartialSyncUnavailable(FALSE)
    , mMoreRecordsToGet(FALSE)
    , mDelayUntil(0)
{}

CSyncResult::~CSyncResult()
{}

ECode CSyncResult::GetSyncAlreadyInProgress(
    /* [out] */ Boolean* syncAlreadyInProgress)
{
    VALIDATE_NOT_NULL(syncAlreadyInProgress)
    *syncAlreadyInProgress = mSyncAlreadyInProgress;
    return NOERROR;
}

ECode CSyncResult::GetTooManyDeletions(
    /* [out] */ Boolean* tooManyDeletions)
{
    VALIDATE_NOT_NULL(tooManyDeletions)
    *tooManyDeletions = mTooManyDeletions;
    return NOERROR;
}

ECode CSyncResult::SetTooManyDeletions(
    /* [in] */ Boolean tooManyDeletions)
{
    mTooManyDeletions = tooManyDeletions;
    return NOERROR;
}

ECode CSyncResult::GetTooManyRetries(
    /* [out] */ Boolean* tooManyRetries)
{
    VALIDATE_NOT_NULL(tooManyRetries)
    *tooManyRetries = mTooManyRetries;
    return NOERROR;
}

ECode CSyncResult::SetTooManyRetries(
    /* [in] */ Boolean tooManyRetries)
{
    mTooManyRetries = tooManyRetries;
    return NOERROR;
}

ECode CSyncResult::GetDatabaseError(
    /* [out] */ Boolean* databaseError)
{
    VALIDATE_NOT_NULL(databaseError)
    *databaseError = mDatabaseError;
    return NOERROR;
}

ECode CSyncResult::SetDatabaseError(
    /* [in] */ Boolean databaseError)
{
    mDatabaseError = databaseError;
    return NOERROR;
}

ECode CSyncResult::GetFullSyncRequested(
    /* [out] */ Boolean* fullSyncRequested)
{
    VALIDATE_NOT_NULL(fullSyncRequested)
    *fullSyncRequested = mFullSyncRequested;
    return NOERROR;
}

ECode CSyncResult::SetFullSyncRequested(
    /* [in] */ Boolean fullSyncRequested)
{
    mFullSyncRequested = fullSyncRequested;
    return NOERROR;
}

ECode CSyncResult::GetPartialSyncUnavailable(
    /* [out] */ Boolean* partialSyncUnavailable)
{
    VALIDATE_NOT_NULL(partialSyncUnavailable)
    *partialSyncUnavailable = mPartialSyncUnavailable;
    return NOERROR;
}

ECode CSyncResult::SetPartialSyncUnavailable(
    /* [in] */ Boolean partialSyncUnavailable)
{
    mPartialSyncUnavailable = partialSyncUnavailable;
    return NOERROR;
}

ECode CSyncResult::GetMoreRecordsToGet(
    /* [out] */ Boolean* moreRecordsToGet)
{
    VALIDATE_NOT_NULL(moreRecordsToGet)
    *moreRecordsToGet = mMoreRecordsToGet;
    return NOERROR;
}

ECode CSyncResult::SetMoreRecordsToGet(
    /* [in] */ Boolean moreRecordsToGet)
{
    mMoreRecordsToGet = moreRecordsToGet;
    return NOERROR;
}

ECode CSyncResult::GetDelayUntil(
    /* [out] */ Int64* delayUntil)
{
    VALIDATE_NOT_NULL(delayUntil)
    *delayUntil = mDelayUntil;
    return NOERROR;
}

ECode CSyncResult::SetDelayUntil(
    /* [in] */ Int64 delayUntil)
{
    mDelayUntil = delayUntil;
    return NOERROR;
}

ECode CSyncResult::GetStats(
    /* [out] */ ISyncStats** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = mStats;
    REFCOUNT_ADD(*stats);
    return NOERROR;
}

ECode CSyncResult::HasHardError(
    /* [out] */ Boolean* hasError)
{
    VALIDATE_NOT_NULL(hasError)
    Int64 numParseExceptions = 0;
    Int64 numConflictDetectedExceptions = 0;
    Int64 numAuthExceptions = 0;
    FAIL_RETURN(mStats->GetNumParseExceptions(&numParseExceptions))
    FAIL_RETURN(mStats->GetNumConflictDetectedExceptions(&numConflictDetectedExceptions))
    FAIL_RETURN(mStats->GetNumAuthExceptions(&numAuthExceptions))
    *hasError = numParseExceptions > 0
            || numConflictDetectedExceptions > 0
            || numAuthExceptions > 0
            || mTooManyDeletions
            || mTooManyRetries
            || mDatabaseError;

    return NOERROR;
}

ECode CSyncResult::HasSoftError(
    /* [out] */ Boolean* hasError)
{
    VALIDATE_NOT_NULL(hasError)
    Int64 numIoExceptions = 0;
    FAIL_RETURN(mStats->GetNumIoExceptions(&numIoExceptions))
    *hasError = mSyncAlreadyInProgress || numIoExceptions > 0;
    return NOERROR;
}

ECode CSyncResult::HasError(
    /* [out] */ Boolean* hasError)
{
    VALIDATE_NOT_NULL(hasError)
    Boolean ret = FALSE;
    *hasError = (HasSoftError(&ret), ret) || (HasHardError(&ret), ret);
    return NOERROR;
}

ECode CSyncResult::MadeSomeProgress(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 numDeletes = 0;
    Int64 numInserts = 0;
    Int64 numUpdates = 0;
    FAIL_RETURN(mStats->GetNumDeletes(&numDeletes))
    FAIL_RETURN(mStats->GetNumInserts(&numInserts))
    FAIL_RETURN(mStats->GetNumUpdates(&numUpdates))
    *result = ((numDeletes > 0) && !mTooManyDeletions)
            || numInserts > 0
            || numUpdates > 0;

    return NOERROR;
}

ECode CSyncResult::Clear()
{
    if (mSyncAlreadyInProgress) {
        //throw new UnsupportedOperationException(
        //        "you are not allowed to clear the ALREADY_IN_PROGRESS SyncStats");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    mTooManyDeletions = FALSE;
    mTooManyRetries = FALSE;
    mDatabaseError = FALSE;
    mFullSyncRequested = FALSE;
    mPartialSyncUnavailable = FALSE;
    mMoreRecordsToGet = FALSE;
    mDelayUntil = 0;
    FAIL_RETURN(mStats->Clear())
    return NOERROR;
}

ECode CSyncResult::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb;
    sb.Append("SyncResult:");

    if (mSyncAlreadyInProgress) {
        sb.Append(" syncAlreadyInProgress: ");
        sb.Append(mSyncAlreadyInProgress);
    }
    if (mTooManyDeletions) {
        sb.Append(" tooManyDeletions: ");
        sb.Append(mTooManyDeletions);
    }
    if (mTooManyRetries) {
        sb.Append(" tooManyRetries: ");
        sb.Append(mTooManyRetries);
    }
    if (mDatabaseError) {
        sb.Append(" databaseError: ");
        sb.Append(mDatabaseError);
    }
    if (mFullSyncRequested) {
        sb.Append(" fullSyncRequested: ");
        sb.Append(mFullSyncRequested);
    }
    if (mPartialSyncUnavailable) {
        sb.Append(" partialSyncUnavailable: ");
        sb.Append(mPartialSyncUnavailable);
    }
    if (mMoreRecordsToGet) {
        sb.Append(" moreRecordsToGet: ");
        sb.Append(mMoreRecordsToGet);
    }
    if (mDelayUntil > 0) {
        sb.Append(" delayUntil: ");
        sb.Append(mDelayUntil);
    }

    sb.Append(Object::ToString(mStats));
    return sb.ToString(str);
}

ECode CSyncResult::ToDebugString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    Boolean ret = FALSE;
    Int64 numParseExceptions = 0;
    Int64 numConflictDetectedExceptions = 0;
    Int64 numAuthExceptions = 0;
    Int64 numIoExceptions = 0;
    StringBuffer sb;
    FAIL_RETURN(mStats->GetNumParseExceptions(&numParseExceptions))
    FAIL_RETURN(mStats->GetNumConflictDetectedExceptions(&numConflictDetectedExceptions))
    FAIL_RETURN(mStats->GetNumAuthExceptions(&numAuthExceptions))
    FAIL_RETURN(mStats->GetNumIoExceptions(&numIoExceptions))

    if (mFullSyncRequested) {
        sb.Append("f1");
    }
    if (mPartialSyncUnavailable) {
        sb.Append("r1");
    }
    if ((HasHardError(&ret), ret)) {
        sb.Append("X1");
    }
    if (numParseExceptions > 0) {
        sb.Append("e");
        sb.Append(numParseExceptions);
    }
    if (numConflictDetectedExceptions > 0) {
        sb.Append("c");
        sb.Append(numConflictDetectedExceptions);
    }
    if (numAuthExceptions > 0) {
        sb.Append("a");
        sb.Append(numAuthExceptions);
    }
    if (mTooManyDeletions) {
        sb.Append("D1");
    }
    if (mTooManyRetries) {
        sb.Append("R1");
    }
    if (mDatabaseError) {
        sb.Append("b1");
    }
    if ((HasSoftError(&ret), ret)) {
        sb.Append("x1");
    }
    if (mSyncAlreadyInProgress) {
        sb.Append("l1");
    }
    if (numIoExceptions > 0) {
        sb.Append("I");
        sb.Append(numIoExceptions);
    }

    return sb.ToString(str);
}

ECode CSyncResult::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 value = 0;
    mSyncAlreadyInProgress = (source->ReadInt32(&value), value) != 0;
    mTooManyDeletions = (source->ReadInt32(&value), value) != 0;
    mTooManyRetries = (source->ReadInt32(&value), value) != 0;
    mDatabaseError = (source->ReadInt32(&value), value) != 0;
    mFullSyncRequested = (source->ReadInt32(&value), value) != 0;
    mPartialSyncUnavailable = (source->ReadInt32(&value), value) != 0;
    mMoreRecordsToGet = (source->ReadInt32(&value), value) != 0;
    source->ReadInt64(&mDelayUntil);
    AutoPtr<IInterface> stats;
    source->ReadInterfacePtr((Handle32*)(IInterface**)&stats);
    mStats = ISyncStats::Probe(stats);
    return NOERROR;
}

ECode CSyncResult::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mSyncAlreadyInProgress ? 1 : 0);
    dest->WriteInt32(mTooManyDeletions ? 1 : 0);
    dest->WriteInt32(mTooManyRetries ? 1 : 0);
    dest->WriteInt32(mDatabaseError ? 1 : 0);
    dest->WriteInt32(mFullSyncRequested ? 1 : 0);
    dest->WriteInt32(mPartialSyncUnavailable ? 1 : 0);
    dest->WriteInt32(mMoreRecordsToGet ? 1 : 0);
    dest->WriteInt64(mDelayUntil);
    dest->WriteInterfacePtr(mStats);
    return NOERROR;
}

ECode CSyncResult::constructor()
{
    return constructor(FALSE);
}

ECode CSyncResult::constructor(
    /* [in] */ Boolean syncAlreadyInProgress)
{
    mSyncAlreadyInProgress = syncAlreadyInProgress;
    mTooManyDeletions = FALSE;
    mTooManyRetries = FALSE;
    mFullSyncRequested = FALSE;
    mPartialSyncUnavailable = FALSE;
    mMoreRecordsToGet = FALSE;
    mDelayUntil = 0;
    FAIL_RETURN(CSyncStats::New((ISyncStats**)&mStats))
    return NOERROR;
}

}
}
}

