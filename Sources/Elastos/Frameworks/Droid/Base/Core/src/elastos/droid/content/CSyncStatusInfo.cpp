
#include "elastos/droid/content/CSyncStatusInfo.h"
#include "elastos/droid/content/ContentResolver.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {

const String CSyncStatusInfo::TAG("Sync");
const Int32 CSyncStatusInfo::VERSION;

CAR_INTERFACE_IMPL_2(CSyncStatusInfo, Object, ISyncStatusInfo, IParcelable)

CAR_OBJECT_IMPL(CSyncStatusInfo)

CSyncStatusInfo::CSyncStatusInfo()
    : mAuthorityId(0)
    , mTotalElapsedTime(0)
    , mNumSyncs(0)
    , mNumSourcePoll(0)
    , mNumSourceServer(0)
    , mNumSourceLocal(0)
    , mNumSourceUser(0)
    , mNumSourcePeriodic(0)
    , mLastSuccessTime(0)
    , mLastSuccessSource(0)
    , mLastFailureTime(0)
    , mLastFailureSource(0)
    , mInitialFailureTime(0)
    , mPending(FALSE)
    , mInitialize(FALSE)
{}

CSyncStatusInfo::~CSyncStatusInfo()
{}


ECode CSyncStatusInfo::constructor()
{
    return NOERROR;
}

ECode CSyncStatusInfo::constructor(
    /* [in] */ Int32 authorityId)
{
    mAuthorityId = authorityId;
    return NOERROR;
}

ECode CSyncStatusInfo::constructor(
    /* [in] */ ISyncStatusInfo* other)
{
    VALIDATE_NOT_NULL(other)
    CSyncStatusInfo* ssi = (CSyncStatusInfo*)other;

    mAuthorityId = ssi->mAuthorityId;
    mTotalElapsedTime = ssi->mTotalElapsedTime;
    mNumSyncs = ssi->mNumSyncs;
    mNumSourcePoll = ssi->mNumSourcePoll;
    mNumSourceServer = ssi->mNumSourceServer;
    mNumSourceLocal = ssi->mNumSourceLocal;
    mNumSourceUser = ssi->mNumSourceUser;
    mNumSourcePeriodic = ssi->mNumSourcePeriodic;
    mLastSuccessTime = ssi->mLastSuccessTime;
    mLastSuccessSource = ssi->mLastSuccessSource;
    mLastFailureTime = ssi->mLastFailureTime;
    mLastFailureSource = ssi->mLastFailureSource;
    mLastFailureMesg = ssi->mLastFailureMesg;
    mInitialFailureTime = ssi->mInitialFailureTime;
    mPending = ssi->mPending;
    mInitialize = ssi->mInitialize;
    if (ssi->mPeriodicSyncTimes != NULL) {
        mPeriodicSyncTimes = ssi->mPeriodicSyncTimes->Clone();
    }
    return NOERROR;
}

ECode CSyncStatusInfo::GetAuthorityId(
    /* [out] */ Int32* authorityId)
{
    VALIDATE_NOT_NULL(authorityId)
    *authorityId = mAuthorityId;
    return NOERROR;
}

ECode CSyncStatusInfo::GetTotalElapsedTime(
    /* [out] */ Int64* totalElapsedTime)
{
    VALIDATE_NOT_NULL(totalElapsedTime)
    *totalElapsedTime = mTotalElapsedTime;
    return NOERROR;
}

ECode CSyncStatusInfo::SetTotalElapsedTime(
    /* [in] */ Int64 totalElapsedTime)
{
    mTotalElapsedTime = totalElapsedTime;
    return NOERROR;
}

ECode CSyncStatusInfo::GetNumSyncs(
    /* [out] */ Int32* numSyncs)
{
    VALIDATE_NOT_NULL(numSyncs)
    *numSyncs = mNumSyncs;
    return NOERROR;
}

ECode CSyncStatusInfo::SetNumSyncs(
    /* [in] */ Int32 numSyncs)
{
    mNumSyncs = numSyncs;
    return NOERROR;
}

ECode CSyncStatusInfo::GetNumSourcePoll(
    /* [out] */ Int32* numSourcePoll)
{
    VALIDATE_NOT_NULL(numSourcePoll)
    *numSourcePoll = mNumSourcePoll;
    return NOERROR;
}

ECode CSyncStatusInfo::SetNumSourcePoll(
    /* [in] */ Int32 numSourcePoll)
{
    mNumSourcePoll = numSourcePoll;
    return NOERROR;
}

ECode CSyncStatusInfo::GetNumSourceServer(
    /* [out] */ Int32* numSourceServer)
{
    VALIDATE_NOT_NULL(numSourceServer)
    *numSourceServer = mNumSourceServer;
    return NOERROR;
}

ECode CSyncStatusInfo::SetNumSourceServer(
    /* [in] */ Int32 numSourceServer)
{
    mNumSourceServer = numSourceServer;
    return NOERROR;
}

ECode CSyncStatusInfo::GetNumSourceLocal(
    /* [out] */ Int32* numSourceLocal)
{
    VALIDATE_NOT_NULL(numSourceLocal)
    *numSourceLocal = mNumSourceLocal;
    return NOERROR;
}

ECode CSyncStatusInfo::SetNumSourceLocal(
    /* [in] */ Int32 numSourceLocal)
{
    mNumSourceLocal = numSourceLocal;
    return NOERROR;
}

ECode CSyncStatusInfo::GetNumSourceUser(
    /* [out] */ Int32* numSourceUser)
{
    VALIDATE_NOT_NULL(numSourceUser)
    *numSourceUser = mNumSourceUser;
    return NOERROR;
}

ECode CSyncStatusInfo::SetNumSourceUser(
    /* [in] */ Int32 numSourceUser)
{
    mNumSourceUser = numSourceUser;
    return NOERROR;
}

ECode CSyncStatusInfo::GetNumSourcePeriodic(
    /* [out] */ Int32* numSourcePeriodic)
{
    VALIDATE_NOT_NULL(numSourcePeriodic)
    *numSourcePeriodic = mNumSourcePeriodic;
    return NOERROR;
}

ECode CSyncStatusInfo::SetNumSourcePeriodic(
    /* [in] */ Int32 numSourcePeriodic)
{
    mNumSourcePeriodic = numSourcePeriodic;
    return NOERROR;
}

ECode CSyncStatusInfo::GetLastSuccessTime(
    /* [out] */ Int64* lastSuccessTime)
{
    VALIDATE_NOT_NULL(lastSuccessTime)
    *lastSuccessTime = mLastSuccessTime;
    return NOERROR;
}

ECode CSyncStatusInfo::SetLastSuccessTime(
    /* [in] */ Int64 lastSuccessTime)
{
    mLastSuccessTime = lastSuccessTime;
    return NOERROR;
}

ECode CSyncStatusInfo::GetLastSuccessSource(
    /* [out] */ Int32* lastSuccessSource)
{
    VALIDATE_NOT_NULL(lastSuccessSource)
    *lastSuccessSource = mLastSuccessSource;
    return NOERROR;
}

ECode CSyncStatusInfo::SetLastSuccessSource(
    /* [in] */ Int32 lastSuccessSource)
{
    mLastSuccessSource = lastSuccessSource;
    return NOERROR;
}

ECode CSyncStatusInfo::GetLastFailureTime(
    /* [out] */ Int64* lastFailureTime)
{
    VALIDATE_NOT_NULL(lastFailureTime)
    *lastFailureTime = mLastFailureTime;
    return NOERROR;
}

ECode CSyncStatusInfo::SetLastFailureTime(
    /* [in] */ Int64 lastFailureTime)
{
    mLastFailureTime = lastFailureTime;
    return NOERROR;
}

ECode CSyncStatusInfo::GetLastFailureSource(
    /* [out] */ Int32* lastFailureSource)
{
    VALIDATE_NOT_NULL(lastFailureSource)
    *lastFailureSource = mLastFailureSource;
    return NOERROR;
}

ECode CSyncStatusInfo::SetLastFailureSource(
    /* [in] */ Int32 lastFailureSource)
{
    mLastFailureSource = lastFailureSource;
    return NOERROR;
}

ECode CSyncStatusInfo::GetLastFailureMesg(
    /* [out] */ String* lastFailureMesg)
{
    VALIDATE_NOT_NULL(lastFailureMesg)
    *lastFailureMesg = mLastFailureMesg;
    return NOERROR;
}

ECode CSyncStatusInfo::SetLastFailureMesg(
    /* [in] */ const String& lastFailureMesg)
{
    mLastFailureMesg = lastFailureMesg;
    return NOERROR;
}

ECode CSyncStatusInfo::GetInitialFailureTime(
    /* [out] */ Int64* initialFailureTime)
{
    VALIDATE_NOT_NULL(initialFailureTime)
    *initialFailureTime = mInitialFailureTime;
    return NOERROR;
}

ECode CSyncStatusInfo::SetInitialFailureTime(
    /* [in] */ Int64 initialFailureTime)
{
    mInitialFailureTime = initialFailureTime;
    return NOERROR;
}

ECode CSyncStatusInfo::GetPending(
    /* [out] */ Boolean* pending)
{
    VALIDATE_NOT_NULL(pending)
    *pending = mPending;
    return NOERROR;
}

ECode CSyncStatusInfo::SetPending(
    /* [in] */ Boolean pending)
{
    mPending = pending;
    return NOERROR;
}

ECode CSyncStatusInfo::GetInitialize(
    /* [out] */ Boolean* initialize)
{
    VALIDATE_NOT_NULL(initialize)
    *initialize = mInitialize;
    return NOERROR;
}

ECode CSyncStatusInfo::SetInitialize(
    /* [in] */ Boolean initialize)
{
    mInitialize = initialize;
    return NOERROR;
}

ECode CSyncStatusInfo::GetLastFailureMesgAsInt(
    /* [in] */ Int32 def,
    /* [out] */ Int32* msg)
{
    VALIDATE_NOT_NULL(msg)

    Int32 i = ContentResolver::SyncErrorStringToInt(mLastFailureMesg);
    if (i > 0) {
        *msg = i;
    }
    else {
        Logger::D(TAG, "Unknown lastFailureMesg:%s", mLastFailureMesg.string());
        *msg = def;
    }
    return NOERROR;
}

ECode CSyncStatusInfo::SetPeriodicSyncTime(
    /* [in] */ Int32 index,
    /* [in] */ Int64 when)
{
    // The list is initialized lazily when scheduling occurs so we need to make sure
    // we initialize elements < index to zero (zero is ignore for scheduling purposes)
    FAIL_RETURN(EnsurePeriodicSyncTimeSize(index))
    (*mPeriodicSyncTimes)[index] = when;
    return NOERROR;
}

ECode CSyncStatusInfo::GetPeriodicSyncTime(
    /* [in] */ Int32 index,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = 0;

    if (NULL != mPeriodicSyncTimes && index < mPeriodicSyncTimes->GetLength()) {
        *time = (*mPeriodicSyncTimes)[index];
    }
    return NOERROR;
}

ECode CSyncStatusInfo::RemovePeriodicSyncTime(
    /* [in] */ Int32 index)
{
    if (mPeriodicSyncTimes != NULL && index < mPeriodicSyncTimes->GetLength()) {
        Int32 length = mPeriodicSyncTimes->GetLength();

        Int32 newSize = length - 1;
        if (newSize < 0) newSize = 0;

        AutoPtr<ArrayOf<Int64> > newPeriodicSyncTimes = ArrayOf<Int64>::Alloc(newSize);

        // Copy head partd data befor index
        newPeriodicSyncTimes->Copy(mPeriodicSyncTimes->GetPayload(), index);

        // Copy tail partd data after index
        for (Int32 i = index; i < newSize; i++) {
            (*newPeriodicSyncTimes)[i] = (*mPeriodicSyncTimes)[i + 1];
        }

        mPeriodicSyncTimes = newPeriodicSyncTimes;
    }

    return NOERROR;
}

ECode CSyncStatusInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 version = 0;
    Int32 value = 0;
    source->ReadInt32(&version);
    if (version != VERSION && version != 1) {
        Logger::W("SyncStatusInfo", "Unknown version: %s", StringUtils::ToString(version).string());
    }

    source->ReadInt32(&mAuthorityId);
    source->ReadInt64(&mTotalElapsedTime);
    source->ReadInt32(&mNumSyncs);
    source->ReadInt32(&mNumSourcePoll);
    source->ReadInt32(&mNumSourceServer);
    source->ReadInt32(&mNumSourceLocal);
    source->ReadInt32(&mNumSourceUser);
    source->ReadInt64(&mLastSuccessTime);
    source->ReadInt32(&mLastSuccessSource);
    source->ReadInt64(&mLastFailureTime);
    source->ReadInt32(&mLastFailureSource);
    source->ReadString(&mLastFailureMesg);
    source->ReadInt64(&mInitialFailureTime);
    source->ReadInt32(&value);
    mPending = value != 0;
    source->ReadInt32(&value);
    mInitialize = value != 0;

    if (version == 1) {
        mPeriodicSyncTimes = NULL;
    }
    else {
        Int32 N = 0;
        source->ReadInt32(&N);
        if (N < 0) {
            mPeriodicSyncTimes = NULL;
        }
        else {
            mPeriodicSyncTimes = ArrayOf<Int64>::Alloc(N);
            Int64 tmp = 0;
            for (Int32 i = 0; i < N; i++) {
                source->ReadInt64(&tmp);
                (*mPeriodicSyncTimes)[i] = tmp;
            }
        }
    }

    return NOERROR;
}

ECode CSyncStatusInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(VERSION);
    dest->WriteInt32(mAuthorityId);
    dest->WriteInt64(mTotalElapsedTime);
    dest->WriteInt32(mNumSyncs);
    dest->WriteInt32(mNumSourcePoll);
    dest->WriteInt32(mNumSourceServer);
    dest->WriteInt32(mNumSourceLocal);
    dest->WriteInt32(mNumSourceUser);
    dest->WriteInt64(mLastSuccessTime);
    dest->WriteInt32(mLastSuccessSource);
    dest->WriteInt64(mLastFailureTime);
    dest->WriteInt32(mLastFailureSource);
    dest->WriteString(mLastFailureMesg);
    dest->WriteInt64(mInitialFailureTime);
    dest->WriteInt32(mPending ? 1 : 0);
    dest->WriteInt32(mInitialize ? 1 : 0);

    if (NULL != mPeriodicSyncTimes) {
        Int32 length = mPeriodicSyncTimes->GetLength();
        dest->WriteInt32(length);
        for (Int32 i = 0; i < length; i++) {
            dest->WriteInt64((*mPeriodicSyncTimes)[i]);
        }
    }
    else {
        dest->WriteInt32(-1);
    }

    return NOERROR;
}

ECode CSyncStatusInfo::EnsurePeriodicSyncTimeSize(
    /* [in] */ Int32 index)
{
    const Int32 requiredSize = index + 1;

    if (NULL == mPeriodicSyncTimes) {
        mPeriodicSyncTimes = ArrayOf<Int64>::Alloc(requiredSize);
        for (Int32 i = 0; i < requiredSize; i++) {
            (*mPeriodicSyncTimes)[i] = 0;
        }
    }
    else if (mPeriodicSyncTimes->GetLength() < requiredSize) {
        AutoPtr<ArrayOf<Int64> > newPeriodicSyncTimes = ArrayOf<Int64>::Alloc(requiredSize);
        newPeriodicSyncTimes->Copy(mPeriodicSyncTimes->GetPayload(), mPeriodicSyncTimes->GetLength());
        mPeriodicSyncTimes = newPeriodicSyncTimes;

        for (Int32 i = mPeriodicSyncTimes->GetLength(); i < requiredSize; i++) {
            (*mPeriodicSyncTimes)[i] = 0;
        }
    }

    return NOERROR;
}

}
}
}

