
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/app/ProcessStats.h"
#include "elastos/droid/internal/app/CProcessDataCollection.h"
#include "elastos/droid/internal/app/CProcessMap.h"
#include "elastos/droid/internal/app/CTotalMemoryUseCollection.h"
#include "elastos/droid/internal/utility/GrowingArrayUtils.h"
#include "elastos/droid/os/CParcel.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/format/DateFormat.h"
#include "elastos/droid/utility/CArrayMap.h"
#include "elastos/droid/utility/CArraySet.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/utility/TimeUtils.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>
#include <libcore/utility/EmptyArray.h>

using Elastos::Droid::Internal::Utility::GrowingArrayUtils;
using Elastos::Droid::Os::CParcel;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Text::Format::DateFormat;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::Logging::Slogger;
using Libcore::Utility::EmptyArray;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

ProcessStats::PssAggr::PssAggr()
    : mPss(0)
    , mSamples(0)
{}

void ProcessStats::PssAggr::Add(
    /* [in] */ Int64 newPss,
    /* [in] */ Int64 newSamples)
{
    mPss = (Int64)((mPss * (Double)mSamples) + (newPss * (Double)newSamples))
            / (mSamples+newSamples);
    mSamples += newSamples;
}

CAR_INTERFACE_IMPL(ProcessStats::ProcessStateComparator, Object, IComparator)

ECode ProcessStats::ProcessStateComparator::Compare(
    /* [in] */ IInterface* _lhs,
    /* [in] */ IInterface* _rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    ProcessState* lhs = (ProcessState*)IProcessState::Probe(_lhs);
    ProcessState* rhs = (ProcessState*)IProcessState::Probe(_rhs);
    assert(lhs && rhs);
    if (lhs->mTmpTotalTime < rhs->mTmpTotalTime) {
        *result = -1;
        return NOERROR;
    }
    else if (lhs->mTmpTotalTime > rhs->mTmpTotalTime) {
        *result = 1;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ProcessStats::DurationsTable, Object, IDurationsTable)

ProcessStats::DurationsTable::DurationsTable()
    : mStats(NULL)
    , mDurationsTableSize(0)
{}

ECode ProcessStats::DurationsTable::constructor(
    /* [in] */ IProcessStats* stats,
    /* [in] */ const String& name)
{
    mStats = (ProcessStats*)stats;
    mName = name;
    return NOERROR;
}

void ProcessStats::DurationsTable::CopyDurationsTo(
    /* [in] */ DurationsTable* other)
{
    if (mDurationsTable != NULL) {
        mStats->mAddInt64Table = ArrayOf<Int32>::Alloc(mDurationsTable->GetLength());
        mStats->mAddInt64TableSize = 0;
        for (Int32 i = 0; i < mDurationsTableSize; i++) {
            Int32 origEnt = (*mDurationsTable)[i];
            Int32 type = (origEnt >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;
            Int32 newOff = mStats->AddInt64Data(i, type, 1);
            (*mStats->mAddInt64Table)[i] = newOff | type;
            mStats->SetInt64(newOff, 0, mStats->GetInt64(origEnt, 0));
        }
        other->mDurationsTable = mStats->mAddInt64Table;
        other->mDurationsTableSize = mStats->mAddInt64TableSize;
    }
    else {
        other->mDurationsTable = NULL;
        other->mDurationsTableSize = 0;
    }
}

void ProcessStats::DurationsTable::AddDurations(
    /* [in] */ DurationsTable* other)
{
    for (Int32 i = 0; i < other->mDurationsTableSize; i++) {
        Int32 ent = (*other->mDurationsTable)[i];
        Int32 state = (ent >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;
        if (DEBUG) Slogger::D(TAG, "Adding state %d duration %lld",
            state, other->mStats->GetInt64(ent, 0));
        AddDuration(state, other->mStats->GetInt64(ent, 0));
    }
}

void ProcessStats::DurationsTable::ResetDurationsSafely()
{
    mDurationsTable = NULL;
    mDurationsTableSize = 0;
}

void ProcessStats::DurationsTable::WriteDurationsToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(mDurationsTableSize);
    for (Int32 i = 0; i < mDurationsTableSize; i++) {
        if (DEBUG_PARCEL) Slogger::I(TAG, "Writing in %s dur #%d: %s",
            mName.string(), i, PrintInt64Offset((*mDurationsTable)[i]).string());
        out->WriteInt32((*mDurationsTable)[i]);
    }
}

Boolean ProcessStats::DurationsTable::ReadDurationsFromParcel(
    /* [in] */ IParcel* in)
{
    mDurationsTable = mStats->ReadTableFromParcel(in, mName, String("durations"));
    if (mDurationsTable == BAD_TABLE) {
        return FALSE;
    }
    mDurationsTableSize = mDurationsTable != NULL ? mDurationsTable->GetLength() : 0;
    return TRUE;
}

void ProcessStats::DurationsTable::AddDuration(
    /* [in] */Int32 state,
    /* [in] */ Int64 dur)
{
    Int32 idx = BinarySearch(*mDurationsTable, mDurationsTableSize, state);
    Int32 off;
    if (idx >= 0) {
        off = (*mDurationsTable)[idx];
    }
    else {
        mStats->mAddInt64Table = mDurationsTable;
        mStats->mAddInt64TableSize = mDurationsTableSize;
        off = mStats->AddInt64Data(~idx, state, 1);
        mDurationsTable = mStats->mAddInt64Table;
        mDurationsTableSize = mStats->mAddInt64TableSize;
    }

    Int32 size = mStats->mInt64s.GetSize();
    if (((off >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK) >= size) {
        return;
    }

    AutoPtr<ArrayOf<Int64> > longs = mStats->mInt64s[(off >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK];
    if (DEBUG) Slogger::D(TAG, "Duration of %s state %d inc by %lld from %lld",
        mName.string(), state, dur, (*longs)[(off >> OFFSET_INDEX_SHIFT) & OFFSET_INDEX_MASK]);
    (*longs)[(off >> OFFSET_INDEX_SHIFT) & OFFSET_INDEX_MASK] += dur;
}

Int64 ProcessStats::DurationsTable::GetDuration(
    /* [in] */Int32 state,
    /* [in] */ Int64 now)
{
    Int32 idx = BinarySearch(*mDurationsTable, mDurationsTableSize, state);
    return idx >= 0 ? mStats->GetInt64((*mDurationsTable)[idx], 0) : 0;
}

ECode ProcessStats::DurationsTable::GetStats(
    /* [out] */ IProcessStats** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = mStats;
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode ProcessStats::DurationsTable::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode ProcessStats::DurationsTable::GetDurationsTable(
    /* [out, callee] */ ArrayOf<Int32>** durationsTable)
{
    VALIDATE_NOT_NULL(durationsTable)
    *durationsTable = mDurationsTable;
    REFCOUNT_ADD(*durationsTable)
    return NOERROR;
}

ECode ProcessStats::DurationsTable::SetDurationsTable(
    /* [in] */ ArrayOf<Int32>* durationsTable)
{
    mDurationsTable = durationsTable;
    return NOERROR;
}

ECode ProcessStats::DurationsTable::GetDurationsTableSize(
    /* [out] */ Int32* durationsTableSize)
{
    VALIDATE_NOT_NULL(durationsTableSize)
    *durationsTableSize = mDurationsTableSize;
    return NOERROR;
}

ECode ProcessStats::DurationsTable::SetDurationsTableSize(
    /* [in] */ Int32 durationsTableSize)
{
    mDurationsTableSize = durationsTableSize;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ProcessStats::ProcessStateHolder, Object, IProcessStateHolder)

ECode ProcessStats::ProcessStateHolder::constructor(
    /* [in] */ Int32 appVersion)
{
    mAppVersion = appVersion;
    return NOERROR;
}

ECode ProcessStats::ProcessStateHolder::GetAppVersion(
    /* [out] */ Int32* appVersion)
{
    VALIDATE_NOT_NULL(appVersion)
    *appVersion = mAppVersion;
    return NOERROR;
}

ECode ProcessStats::ProcessStateHolder::GetState(
    /* [out] */ IProcessState** state)
{
    VALIDATE_NOT_NULL(state)
    *state = mState;
    REFCOUNT_ADD(*state)
    return NOERROR;
}

ECode ProcessStats::ProcessStateHolder::SetState(
    /* [in] */ IProcessState* state)
{
    mState = (ProcessState*)state;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ProcessStats::ProcessState, DurationsTable, IProcessState)

ProcessStats::ProcessState::ProcessState()
    : mCommonProcess(NULL)
    , mUid(0)
    , mVersion(0)
    , mTmpTotalTime(0)
    , mCurState(STATE_NOTHING)
    , mStartTime(0)
    , mLastPssState(STATE_NOTHING)
    , mLastPssTime(0)
    , mPssTableSize(0)
    , mActive(FALSE)
    , mNumActiveServices(0)
    , mNumStartedServices(0)
    , mNumExcessiveWake(0)
    , mNumExcessiveCpu(0)
    , mNumCachedKill(0)
    , mMinCachedKillPss(0)
    , mAvgCachedKillPss(0)
    , mMaxCachedKillPss(0)
    , mMultiPackage(FALSE)
    , mDead(FALSE)
    , mTmpNumInUse(0)
    , mTmpFoundSubProc(NULL)
{}

ECode ProcessStats::ProcessState::constructor(
    /* [in] */ IProcessStats* processStats,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 vers,
    /* [in] */ const String& name)
{
    DurationsTable::constructor(processStats, name);
    mCommonProcess = this;
    mPackage = pkg;
    mUid = uid;
    mVersion = vers;
    return NOERROR;
}

ECode ProcessStats::ProcessState::constructor(
    /* [in] */ IProcessState* _commonProcess,
    /* [in] */ const String& pkg,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 vers,
    /* [in] */ const String& name,
    /* [in] */ Int64 now)
{
    ProcessState* commonProcess = (ProcessState*)_commonProcess;
    DurationsTable::constructor(commonProcess->mStats, name);
    mCommonProcess = commonProcess;
    mPackage = pkg;
    mUid = uid;
    mVersion = vers;
    mCurState = commonProcess->mCurState;
    mStartTime = now;
    return NOERROR;
}

AutoPtr<ProcessStats::ProcessState> ProcessStats::ProcessState::Clone(
    /* [in] */ const String& pkg,
    /* [in] */ Int64 now)
{
    AutoPtr<ProcessState> pnew = new ProcessState();
    pnew->constructor(this, pkg, mUid, mVersion, mName, now);
    CopyDurationsTo(pnew);
    if (mPssTable != NULL) {
        mStats->mAddInt64Table = ArrayOf<Int32>::Alloc(mPssTable->GetLength());
        mStats->mAddInt64TableSize = 0;
        for (Int32 i = 0; i < mPssTableSize; i++) {
            Int32 origEnt = (*mPssTable)[i];
            Int32 type = (origEnt >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;
            Int32 newOff = mStats->AddInt64Data(i, type, PSS_COUNT);
            (*mStats->mAddInt64Table)[i] = newOff | type;
            for (Int32 j=0; j<PSS_COUNT; j++) {
                mStats->SetInt64(newOff, j, mStats->GetInt64(origEnt, j));
            }
        }
        pnew->mPssTable = mStats->mAddInt64Table;
        pnew->mPssTableSize = mStats->mAddInt64TableSize;
    }
    pnew->mNumExcessiveWake = mNumExcessiveWake;
    pnew->mNumExcessiveCpu = mNumExcessiveCpu;
    pnew->mNumCachedKill = mNumCachedKill;
    pnew->mMinCachedKillPss = mMinCachedKillPss;
    pnew->mAvgCachedKillPss = mAvgCachedKillPss;
    pnew->mMaxCachedKillPss = mMaxCachedKillPss;
    pnew->mActive = mActive;
    pnew->mNumActiveServices = mNumActiveServices;
    pnew->mNumStartedServices = mNumStartedServices;
    return pnew;
}

void ProcessStats::ProcessState::Add(
    /* [in] */ ProcessState* other)
{
    AddDurations(other);
    for (Int32 i = 0; i < other->mPssTableSize; i++) {
        Int32 ent = (*other->mPssTable)[i];
        Int32 state = (ent >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;
        AddPss(state, (Int32) other->mStats->GetInt64(ent, PSS_SAMPLE_COUNT),
                other->mStats->GetInt64(ent, PSS_MINIMUM),
                other->mStats->GetInt64(ent, PSS_AVERAGE),
                other->mStats->GetInt64(ent, PSS_MAXIMUM),
                other->mStats->GetInt64(ent, PSS_USS_MINIMUM),
                other->mStats->GetInt64(ent, PSS_USS_AVERAGE),
                other->mStats->GetInt64(ent, PSS_USS_MAXIMUM));
    }
    mNumExcessiveWake += other->mNumExcessiveWake;
    mNumExcessiveCpu += other->mNumExcessiveCpu;
    if (other->mNumCachedKill > 0) {
        AddCachedKill(other->mNumCachedKill, other->mMinCachedKillPss,
                other->mAvgCachedKillPss, other->mMaxCachedKillPss);
    }
}

void ProcessStats::ProcessState::ResetSafely(
    /* [in] */ Int64 now)
{
    ResetDurationsSafely();
    mStartTime = now;
    mLastPssState = STATE_NOTHING;
    mLastPssTime = 0;
    mPssTable = NULL;
    mPssTableSize = 0;
    mNumExcessiveWake = 0;
    mNumExcessiveCpu = 0;
    mNumCachedKill = 0;
    mMinCachedKillPss = mAvgCachedKillPss = mMaxCachedKillPss = 0;
}

void ProcessStats::ProcessState::MakeDead()
{
    mDead = TRUE;
}

void ProcessStats::ProcessState::EnsureNotDead()
{
    if (!mDead) {
        return;
    }
    Slogger::W/*WtfStack*/(TAG, "ProcessState dead: name=%s pkg=%s uid=%s common.name=%s",
        mName.string(), mPackage.string(), mUid, mCommonProcess->mName.string());
}

void ProcessStats::ProcessState::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 now)
{
    out->WriteBoolean(mMultiPackage);
    WriteDurationsToParcel(out);
    out->WriteInt32(mPssTableSize);
    for (Int32 i = 0; i < mPssTableSize; i++) {
        if (DEBUG_PARCEL) Slogger::I(TAG, "Writing in %s pss #%d: %s",
            mName.string(), i, PrintInt64Offset((*mPssTable)[i]).string());
        out->WriteInt32((*mPssTable)[i]);
    }
    out->WriteInt32(mNumExcessiveWake);
    out->WriteInt32(mNumExcessiveCpu);
    out->WriteInt32(mNumCachedKill);
    if (mNumCachedKill > 0) {
        out->WriteInt64(mMinCachedKillPss);
        out->WriteInt64(mAvgCachedKillPss);
        out->WriteInt64(mMaxCachedKillPss);
    }
}

Boolean ProcessStats::ProcessState::ReadFromParcel(
    /* [in] */ IParcel* in,
    /* [in] */ Boolean fully)
{
    Boolean multiPackage;
    in->ReadBoolean(&multiPackage);
    if (fully) {
        mMultiPackage = multiPackage;
    }
    if (DEBUG_PARCEL) Slogger::D(TAG, "Reading durations table...");
    if (!ReadDurationsFromParcel(in)) {
        return FALSE;
    }
    if (DEBUG_PARCEL) Slogger::D(TAG, "Reading pss table...");
    mPssTable = mStats->ReadTableFromParcel(in, mName, String("pss"));
    if (mPssTable == BAD_TABLE) {
        return FALSE;
    }
    mPssTableSize = mPssTable != NULL ? mPssTable->GetLength() : 0;
    in->ReadInt32(&mNumExcessiveWake);
    in->ReadInt32(&mNumExcessiveCpu);
    in->ReadInt32(&mNumCachedKill);
    if (mNumCachedKill > 0) {
        in->ReadInt64(&mMinCachedKillPss);
        in->ReadInt64(&mAvgCachedKillPss);
        in->ReadInt64(&mMaxCachedKillPss);
    }
    else {
        mMinCachedKillPss = mAvgCachedKillPss = mMaxCachedKillPss = 0;
    }
    return TRUE;
}

ECode ProcessStats::ProcessState::MakeActive()
{
    EnsureNotDead();
    mActive = TRUE;
    return NOERROR;
}

ECode ProcessStats::ProcessState::MakeInactive()
{
    mActive = FALSE;
    return NOERROR;
}

Boolean ProcessStats::ProcessState::IsInUse()
{
    return mActive || mNumActiveServices > 0 || mNumStartedServices > 0
            || mCurState != STATE_NOTHING;
}

ECode ProcessStats::ProcessState::IsInUse(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsInUse();
    return NOERROR;
}

ECode ProcessStats::ProcessState::SetState(
    /* [in] */ Int32 state,
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now,
    /* [in] */ IArrayMap* pkgList)
{
    if (state < 0) {
        state = mNumStartedServices > 0
                ? (STATE_SERVICE_RESTARTING+(memFactor*STATE_COUNT)) : STATE_NOTHING;
    }
    else {
        state = (*PROCESS_STATE_TO_STATE)[state] + (memFactor*STATE_COUNT);
    }

    // First update the common process.
    mCommonProcess->SetState(state, now);

    // If the common process is not multi-package, there is nothing else to do.
    if (!mCommonProcess->mMultiPackage) {
        return NOERROR;;
    }

    if (pkgList != NULL) {
        Int32 size;
        pkgList->GetSize(&size);
        for (Int32 ip = size-1; ip >= 0; ip--) {
            AutoPtr<ProcessState> proc;
            FAIL_RETURN(PullFixedProc(pkgList, ip, (ProcessState**)&proc));
            proc->SetState(state, now);
        }
    }
    return NOERROR;
}

void ProcessStats::ProcessState::SetState(
    /* [in] */ Int32 state,
    /* [in] */ Int64 now)
{
    EnsureNotDead();
    if (mCurState != state) {
        //Slogger::I(TAG, "Setting state in " + mName + "/" + mPackage + ": " + state);
        CommitStateTime(now);
        mCurState = state;
    }
}

void ProcessStats::ProcessState::CommitStateTime(
    /* [in] */ Int64 now)
{
    if (mCurState != STATE_NOTHING) {
        Int64 dur = now - mStartTime;
        if (dur > 0) {
            AddDuration(mCurState, dur);
        }
    }
    mStartTime = now;
}

void ProcessStats::ProcessState::IncActiveServices(
    /* [in] */ const String& serviceName)
{
    if (DEBUG && mName.IsEmpty()) {
        // RuntimeException here = new RuntimeException("here");
        // here.fillInStackTrace();
        Slogger::D(TAG, "IncActiveServices: %s service=%s to %d",
            ToString().string(), serviceName.string(), (mNumActiveServices+1));
    }
    if (mCommonProcess != this) {
        mCommonProcess->IncActiveServices(serviceName);
    }
    mNumActiveServices++;
}

void ProcessStats::ProcessState::DecActiveServices(
    /* [in] */ const String& serviceName)
{
    if (DEBUG && mName.IsEmpty()) {
        // RuntimeException here = new RuntimeException("here");
        // here.fillInStackTrace();
        Slogger::D(TAG, "DecActiveServices: %s service=%s to %d",
            ToString().string(), serviceName.string(), (mNumActiveServices-1));
    }
    if (mCommonProcess != this) {
        mCommonProcess->DecActiveServices(serviceName);
    }
    mNumActiveServices--;
    if (mNumActiveServices < 0) {
        Slogger::W/*WtfStack*/(TAG, "Proc active services underrun: pkg=%s uid=%d proc=%s service=%s",
            mPackage.string(), mUid, mName.string(), serviceName.string());
        mNumActiveServices = 0;
    }
}

void ProcessStats::ProcessState::IncStartedServices(
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now,
    /* [in] */ const String& serviceName)
{
    if (FALSE) {
        // RuntimeException here = new RuntimeException("here");
        // here.fillInStackTrace();
        Slogger::D(TAG, "IncStartedServices: %s service=%s to %d",
            ToString().string(), serviceName.string(), (mNumStartedServices+1));
    }
    if (mCommonProcess != this) {
        mCommonProcess->IncStartedServices(memFactor, now, serviceName);
    }
    mNumStartedServices++;
    if (mNumStartedServices == 1 && mCurState == STATE_NOTHING) {
        SetState(STATE_SERVICE_RESTARTING + (memFactor*STATE_COUNT), now);
    }
}

void ProcessStats::ProcessState::DecStartedServices(
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now,
    /* [in] */ const String& serviceName)
{
    if (FALSE) {
        // RuntimeException here = new RuntimeException("here");
        // here.fillInStackTrace();
        Slogger::D(TAG, "DecActiveServices: %s service=%s to %d",
            ToString().string(), serviceName.string(), (mNumStartedServices-1));
    }
    if (mCommonProcess != this) {
        mCommonProcess->DecStartedServices(memFactor, now, serviceName);
    }
    mNumStartedServices--;
    if (mNumStartedServices == 0 && (mCurState%STATE_COUNT) == STATE_SERVICE_RESTARTING) {
        SetState(STATE_NOTHING, now);
    }
    else if (mNumStartedServices < 0) {
        Slogger::W/*WtfStack*/(TAG, "Proc started services underrun: pkg=%s uid=%d name=%s",
                mPackage.string(), mUid, mName.string());
        mNumStartedServices = 0;
    }
}

ECode ProcessStats::ProcessState::AddPss(
    /* [in] */ Int64 pss,
    /* [in] */ Int64 uss,
    /* [in] */ Boolean always,
    /* [in] */ IArrayMap* pkgList)
{
    EnsureNotDead();
    if (!always) {
        if (mLastPssState == mCurState && SystemClock::GetUptimeMillis()
                < (mLastPssTime+(30*1000))) {
            return NOERROR;
        }
    }
    mLastPssState = mCurState;
    mLastPssTime = SystemClock::GetUptimeMillis();
    if (mCurState != STATE_NOTHING) {
        // First update the common process.
        mCommonProcess->AddPss(mCurState, 1, pss, pss, pss, uss, uss, uss);

        // If the common process is not multi-package, there is nothing else to do.
        if (!mCommonProcess->mMultiPackage) {
            return NOERROR;
        }

        if (pkgList != NULL) {
            Int32 size;
            pkgList->GetSize(&size);
            for (Int32 ip = size - 1; ip >= 0; ip--) {
                AutoPtr<ProcessState> proc;
                FAIL_RETURN(PullFixedProc(pkgList, ip, (ProcessState**)&proc));
                proc->AddPss(mCurState, 1,
                        pss, pss, pss, uss, uss, uss);
            }
        }
    }
    return NOERROR;
}

void ProcessStats::ProcessState::AddPss(
    /* [in] */ Int32 state,
    /* [in] */ Int32 inCount,
    /* [in] */ Int64 minPss,
    /* [in] */ Int64 avgPss,
    /* [in] */ Int64 maxPss,
    /* [in] */ Int64 minUss,
    /* [in] */ Int64 avgUss,
    /* [in] */ Int64 maxUss)
{
    Int32 idx = BinarySearch(*mPssTable, mPssTableSize, state);
    Int32 off;
    if (idx >= 0) {
        off = (*mPssTable)[idx];
    }
    else {
        mStats->mAddInt64Table = mPssTable;
        mStats->mAddInt64TableSize = mPssTableSize;
        off = mStats->AddInt64Data(~idx, state, PSS_COUNT);
        mPssTable = mStats->mAddInt64Table;
        mPssTableSize = mStats->mAddInt64TableSize;
    }

    Int32 size = mStats->mInt64s.GetSize();
    if (((off >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK) >= size) {
        return;
    }

    AutoPtr<ArrayOf<Int64> > longs = mStats->mInt64s[(off >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK];
    idx = (off >> OFFSET_INDEX_SHIFT) & OFFSET_INDEX_MASK;
    Int64 count = (*longs)[idx+PSS_SAMPLE_COUNT];
    if (count == 0) {
        (*longs)[idx+PSS_SAMPLE_COUNT] = inCount;
        (*longs)[idx+PSS_MINIMUM] = minPss;
        (*longs)[idx+PSS_AVERAGE] = avgPss;
        (*longs)[idx+PSS_MAXIMUM] = maxPss;
        (*longs)[idx+PSS_USS_MINIMUM] = minUss;
        (*longs)[idx+PSS_USS_AVERAGE] = avgUss;
        (*longs)[idx+PSS_USS_MAXIMUM] = maxUss;
    }
    else {
        (*longs)[idx+PSS_SAMPLE_COUNT] = count+inCount;
        if ((*longs)[idx+PSS_MINIMUM] > minPss) {
            (*longs)[idx+PSS_MINIMUM] = minPss;
        }
        (*longs)[idx+PSS_AVERAGE] = (Int64)(
                (((*longs)[idx+PSS_AVERAGE]*(Double)count)+(avgPss*(Double)inCount))
                        / (count+inCount) );
        if ((*longs)[idx+PSS_MAXIMUM] < maxPss) {
            (*longs)[idx+PSS_MAXIMUM] = maxPss;
        }
        if ((*longs)[idx+PSS_USS_MINIMUM] > minUss) {
            (*longs)[idx+PSS_USS_MINIMUM] = minUss;
        }
        (*longs)[idx+PSS_USS_AVERAGE] = (Int64)(
                (((*longs)[idx+PSS_USS_AVERAGE]*(Double)count)+(avgUss*(Double)inCount))
                        / (count+inCount) );
        if ((*longs)[idx+PSS_USS_MAXIMUM] < maxUss) {
            (*longs)[idx+PSS_USS_MAXIMUM] = maxUss;
        }
    }
}

ECode ProcessStats::ProcessState::ReportExcessiveWake(
    /* [in] */ IArrayMap* pkgList)
{
    EnsureNotDead();
    mCommonProcess->mNumExcessiveWake++;
    if (!mCommonProcess->mMultiPackage) {
        return NOERROR;;
    }

    Int32 size;
    pkgList->GetSize(&size);
    for (Int32 ip = size - 1; ip >= 0; ip--) {
        AutoPtr<ProcessState> proc;
        FAIL_RETURN(PullFixedProc(pkgList, ip, (ProcessState**)&proc));
        proc->mNumExcessiveWake++;
    }
    return NOERROR;
}

ECode ProcessStats::ProcessState::ReportExcessiveCpu(
    /* [in] */ IArrayMap* pkgList)
{
    EnsureNotDead();
    mCommonProcess->mNumExcessiveCpu++;
    if (!mCommonProcess->mMultiPackage) {
        return NOERROR;;
    }

    Int32 size;
    pkgList->GetSize(&size);
    for (Int32 ip = size - 1; ip >= 0; ip--) {
        AutoPtr<ProcessState> proc;
        FAIL_RETURN(PullFixedProc(pkgList, ip, (ProcessState**)&proc));
        proc->mNumExcessiveCpu++;
    }
    return NOERROR;
}

void ProcessStats::ProcessState::AddCachedKill(
    /* [in] */ Int32 num,
    /* [in] */ Int64 minPss,
    /* [in] */ Int64 avgPss,
    /* [in] */ Int64 maxPss)
{
    if (mNumCachedKill <= 0) {
        mNumCachedKill = num;
        mMinCachedKillPss = minPss;
        mAvgCachedKillPss = avgPss;
        mMaxCachedKillPss = maxPss;
    }
    else {
        if (minPss < mMinCachedKillPss) {
            mMinCachedKillPss = minPss;
        }
        if (maxPss > mMaxCachedKillPss) {
            mMaxCachedKillPss = maxPss;
        }
        mAvgCachedKillPss = (Int64)(((mAvgCachedKillPss*(Double)mNumCachedKill) + avgPss)
                / (mNumCachedKill+num));
        mNumCachedKill += num;
    }
}

ECode ProcessStats::ProcessState::ReportCachedKill(
    /* [in] */ IArrayMap* pkgList,
    /* [in] */ Int64 pss)
{
    EnsureNotDead();
    mCommonProcess->AddCachedKill(1, pss, pss, pss);
    if (!mCommonProcess->mMultiPackage) {
        return NOERROR;
    }

    Int32 size;
    pkgList->GetSize(&size);
    for (Int32 ip = size - 1; ip >= 0; ip--) {
        AutoPtr<ProcessState> proc;
        FAIL_RETURN(PullFixedProc(pkgList, ip, (ProcessState**)&proc));
        proc->AddCachedKill(1, pss, pss, pss);
    }
    return NOERROR;
}

ECode ProcessStats::ProcessState::PullFixedProc(
    /* [in] */ const String& pkgName,
    /* [out] */ ProcessState** retProc)
{
    VALIDATE_NOT_NULL(retProc)
    if (mMultiPackage) {
        // The array map is still pointing to a common process state
        // that is now shared across packages.  Update it to point to
        // the new per-package state.
        AutoPtr<IInterface> item;
        mStats->mPackages->Get(pkgName, mUid, (IInterface**)&item);
        AutoPtr<ISparseArray> vpkg = ISparseArray::Probe(item);
        if (vpkg == NULL) {
            Slogger::E(TAG, "Didn't find package %s / %d", pkgName.string(), mUid);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        item = NULL;
        vpkg->Get(mVersion, (IInterface**)&item);
        AutoPtr<PackageState> pkg = (PackageState*)IPackageState::Probe(item);
        if (pkg == NULL) {
            Slogger::E(TAG, "Didn't find package %s / %d vers %d",
                pkgName.string(), mUid, mVersion);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        item = NULL;
        pkg->mProcesses->Get(CoreUtils::Convert(mName), (IInterface**)&item);
        AutoPtr<ProcessState> proc = (ProcessState*)IProcessState::Probe(item);
        if (proc == NULL) {
            Slogger::E(TAG, "Didn't create per-package process %s in pkg %s / %d vers %d",
                mName.string(), pkgName.string(), mUid, mVersion);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        *retProc = proc;
        REFCOUNT_ADD(*retProc)
    }
    else {
        *retProc = this;
        REFCOUNT_ADD(*retProc)
    }

    return NOERROR;
}

ECode ProcessStats::ProcessState::PullFixedProc(
    /* [in] */ IArrayMap* pkgList,
    /* [in] */ Int32 index,
    /* [out] */ ProcessState** retProc)
{
    VALIDATE_NOT_NULL(retProc)
    AutoPtr<IInterface> value;
    pkgList->GetValueAt(index, (IInterface**)&value);
    ProcessStateHolder* holder = (ProcessStateHolder*)IProcessStateHolder::Probe(value);
    AutoPtr<ProcessState> proc = holder->mState;
    if (mDead && proc->mCommonProcess != proc) {
        // Somehow we are contining to use a process state that is dead, because
        // it was not being told it was active during the last commit.  We can recover
        // from this by generating a fresh new state, but this is bad because we
        // are losing whatever data we had in the old process state.
        Slogger::W/*wtf*/(TAG, "Pulling dead proc: name=%s pkg=%s uid=%d common.name=%s",
            mName.string(), mPackage.string(), mUid, mCommonProcess->mName.string());
        AutoPtr<IProcessState> temp;
        mStats->GetProcessStateLocked(proc->mPackage, proc->mUid, proc->mVersion,
                proc->mName, (IProcessState**)&temp);
        proc = (ProcessState*)temp.Get();
    }
    if (proc->mMultiPackage) {
        // The array map is still pointing to a common process state
        // that is now shared across packages.  Update it to point to
        // the new per-package state.
        AutoPtr<IInterface> key;
        pkgList->GetKeyAt(index, (IInterface**)&key);
        String pkgName;
        ICharSequence::Probe(key)->ToString(&pkgName);
        AutoPtr<IInterface> item;
        mStats->mPackages->Get(pkgName, proc->mUid, (IInterface**)&item);
        AutoPtr<ISparseArray> vpkg = ISparseArray::Probe(item);
        if (vpkg == NULL) {
            Slogger::E(TAG, "No existing package %s/%d for multi-proc %s",
                pkgName.string(), proc->mUid, proc->mName.string());
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        item = NULL;
        vpkg->Get(proc->mVersion, (IInterface**)&item);
        AutoPtr<PackageState> pkg = (PackageState*)IPackageState::Probe(item);
        if (pkg == NULL) {
            Slogger::E(TAG, "No existing package %s/%d for multi-proc %d version %d",
                pkgName.string(), proc->mUid, proc->mName.string(), proc->mVersion);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        item = NULL;
        pkg->mProcesses->Get(CoreUtils::Convert(proc->mName), (IInterface**)&item);
        proc = (ProcessState*)IProcessState::Probe(item);
        if (proc == NULL) {
            Slogger::E(TAG, "Didn't create per-package process %s in pkg %s/%d",
                proc->mName.string(), pkg->mPackageName.string(), pkg->mUid);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
        holder->mState = proc;
    }
    *retProc = proc;
    REFCOUNT_ADD(*retProc)
    return NOERROR;
}

Int64 ProcessStats::ProcessState::GetDuration(
    /* [in] */ Int32 state,
    /* [in] */ Int64 now)
{
    Int64 time = DurationsTable::GetDuration(state, now);
    if (mCurState == state) {
        time += now - mStartTime;
    }
    return time;
}

Int64 ProcessStats::ProcessState::GetPssSampleCount(
    /* [in] */ Int32 state)
{
    Int32 idx = BinarySearch(*mPssTable, mPssTableSize, state);
    return idx >= 0 ? mStats->GetInt64((*mPssTable)[idx], PSS_SAMPLE_COUNT) : 0;
}

Int64 ProcessStats::ProcessState::GetPssMinimum(
    /* [in] */ Int32 state)
{
    Int32 idx = BinarySearch(*mPssTable, mPssTableSize, state);
    return idx >= 0 ? mStats->GetInt64((*mPssTable)[idx], PSS_MINIMUM) : 0;
}

Int64 ProcessStats::ProcessState::GetPssAverage(
    /* [in] */ Int32 state)
{
    Int32 idx = BinarySearch(*mPssTable, mPssTableSize, state);
    return idx >= 0 ? mStats->GetInt64((*mPssTable)[idx], PSS_AVERAGE) : 0;
}

Int64 ProcessStats::ProcessState::GetPssMaximum(
    /* [in] */ Int32 state)
{
    Int32 idx = BinarySearch(*mPssTable, mPssTableSize, state);
    return idx >= 0 ? mStats->GetInt64((*mPssTable)[idx], PSS_MAXIMUM) : 0;
}

Int64 ProcessStats::ProcessState::GetPssUssMinimum(
    /* [in] */ Int32 state)
{
    Int32 idx = BinarySearch(*mPssTable, mPssTableSize, state);
    return idx >= 0 ? mStats->GetInt64((*mPssTable)[idx], PSS_USS_MINIMUM) : 0;
}

Int64 ProcessStats::ProcessState::GetPssUssAverage(
    /* [in] */ Int32 state)
{
    Int32 idx = BinarySearch(*mPssTable, mPssTableSize, state);
    return idx >= 0 ? mStats->GetInt64((*mPssTable)[idx], PSS_USS_AVERAGE) : 0;
}

Int64 ProcessStats::ProcessState::GetPssUssMaximum(
    /* [in] */ Int32 state)
{
    Int32 idx = BinarySearch(*mPssTable, mPssTableSize, state);
    return idx >= 0 ? mStats->GetInt64((*mPssTable)[idx], PSS_USS_MAXIMUM) : 0;
}

String ProcessStats::ProcessState::ToString()
{
    StringBuilder sb(128);
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    sb.Append("ProcessState{");
    Int32 hashCode;
    system->IdentityHashCode((IProcessState*)this, &hashCode);
    sb.Append(StringUtils::ToString(hashCode, 16));
    sb.Append(" ");
    sb.Append(mName);
    sb.Append("/");
    sb.Append(mUid);
    sb.Append(" pkg=");
    sb.Append(mPackage);
    if (mMultiPackage)
        sb.Append(" (multi)");
    if (mCommonProcess != this)
        sb.Append(" (sub)");
    sb.Append("}");
    return sb.ToString();
}

ECode ProcessStats::ProcessState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

ECode ProcessStats::ProcessState::GetCommonProcess(
    /* [out] */ IProcessState** commonProcess)
{
    VALIDATE_NOT_NULL(commonProcess)
    *commonProcess = mCommonProcess;
    REFCOUNT_ADD(*commonProcess)
    return NOERROR;
}

ECode ProcessStats::ProcessState::SetCommonProcess(
    /* [in] */ IProcessState* commonProcess)
{
    mCommonProcess = (ProcessState*)commonProcess;
    return NOERROR;
}

ECode ProcessStats::ProcessState::GetPackage(
    /* [out] */ String* package)
{
    VALIDATE_NOT_NULL(package)
    *package = mPackage;
    return NOERROR;
}

ECode ProcessStats::ProcessState::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid = mUid;
    return NOERROR;
}

ECode ProcessStats::ProcessState::GetVersion(
    /* [out] */ Int32* version)
{
    VALIDATE_NOT_NULL(version)
    *version = mVersion;
    return NOERROR;
}

ECode ProcessStats::ProcessState::GetTmpTotalTime(
    /* [out] */ Int64* tmpTotalTime)
{
    VALIDATE_NOT_NULL(tmpTotalTime)
    *tmpTotalTime = mTmpTotalTime;
    return NOERROR;
}

ECode ProcessStats::ProcessState::SetTmpTotalTime(
    /* [in] */ Int64 tmpTotalTime)
{
    mTmpTotalTime = tmpTotalTime;
    return NOERROR;
}

const Int32 ProcessStats::ServiceState::SERVICE_COUNT;

CAR_INTERFACE_IMPL(ProcessStats::ServiceState, DurationsTable, IServiceState)

ProcessStats::ServiceState::ServiceState()
    : mRunState(STATE_NOTHING)
    , mStartedState(STATE_NOTHING)
    , mBoundState(STATE_NOTHING)
    , mExecState(STATE_NOTHING)
    , mRunCount(0)
    , mRunStartTime(0)
    , mStarted(FALSE)
    , mRestarting(FALSE)
    , mStartedCount(0)
    , mStartedStartTime(0)
    , mBoundCount(0)
    , mBoundStartTime(0)
    , mExecCount(0)
    , mExecStartTime(0)
{}

ECode ProcessStats::ServiceState::constructor(
    /* [in] */ IProcessStats* processStats,
    /* [in] */ const String& pkg,
    /* [in] */ const String& name,
    /* [in] */ const String& processName,
    /* [in] */ IProcessState* proc)
{
    DurationsTable::constructor(processStats, name);
    mPackage = pkg;
    mProcessName = processName;
    mProc = (ProcessState*)proc;
    return NOERROR;
}

ECode ProcessStats::ServiceState::ApplyNewOwner(
    /* [in] */ IInterface* _newOwner)
{
    IInterface* newOwner = TO_IINTERFACE(_newOwner);
    if (mOwner.Get() != newOwner) {
        if (mOwner == NULL) {
            mOwner = newOwner;
            mProc->IncActiveServices(mName);
        }
        else {
            // There was already an old owner, Reset this object for its
            // new owner.
            mOwner = newOwner;
            if (mStarted || mBoundState != STATE_NOTHING || mExecState != STATE_NOTHING) {
                Int64 now = SystemClock::GetUptimeMillis();
                String newOwnerStr, ownerStr;
                IObject::Probe(newOwner)->ToString(&newOwnerStr);
                IObject::Probe(mOwner)->ToString(&ownerStr);
                if (mStarted) {
                    if (DEBUG) Slogger::D(TAG, "Service has new owner %s from %s while started:"
                        " pkg=%s service=%s proc=%s", newOwnerStr.string(), ownerStr.string(),
                        mPackage.string(), mName.string(), mProc->ToString().string());
                    SetStarted(FALSE, 0, now);
                }
                if (mBoundState != STATE_NOTHING) {
                    if (DEBUG) Slogger::D(TAG, "Service has new owner %s from %s while bound:"
                        " pkg=%s service=%s proc=%s", newOwnerStr.string(), ownerStr.string(),
                        mPackage.string(), mName.string(), mProc->ToString().string());
                    SetBound(FALSE, 0, now);
                }
                if (mExecState != STATE_NOTHING) {
                    if (DEBUG) Slogger::D(TAG, "Service has new owner %s from %s while executing:"
                        " pkg=%s service=%s proc=%s", newOwnerStr.string(), ownerStr.string(),
                        mPackage.string(), mName.string(), mProc->ToString().string());
                    SetExecuting(FALSE, 0, now);
                }
            }
        }
    }
    return NOERROR;
}

ECode ProcessStats::ServiceState::ClearCurrentOwner(
    /* [in] */ IInterface* _owner,
    /* [in] */ Boolean silently)
{
    IInterface* owner = TO_IINTERFACE(_owner);
    if (mOwner.Get() == owner) {
        mProc->DecActiveServices(mName);
        if (mStarted || mBoundState != STATE_NOTHING || mExecState != STATE_NOTHING) {
            Int64 now = SystemClock::GetUptimeMillis();
            String ownerStr;
            IObject::Probe(owner)->ToString(&ownerStr);
            if (mStarted) {
                if (!silently) {
                    Slogger::W/*WtfStack*/(TAG, "Service owner %s cleared while started:"
                        " pkg=%s service=%s proc=%s", ownerStr.string(), mPackage.string(),
                        mName.string(), mProc->ToString().string());
                }
                SetStarted(FALSE, 0, now);
            }
            if (mBoundState != STATE_NOTHING) {
                if (!silently) {
                    Slogger::W/*WtfStack*/(TAG, "Service owner %s cleared while bound:"
                        " pkg=%s service=%s proc=%s", ownerStr.string(), mPackage.string(),
                        mName.string(), mProc->ToString().string());
                }
                SetBound(FALSE, 0, now);
            }
            if (mExecState != STATE_NOTHING) {
                if (!silently) {
                    Slogger::W/*WtfStack*/(TAG, "Service owner %s cleared while exec:"
                        " pkg=%s service=%s proc=%s", ownerStr.string(), mPackage.string(),
                        mName.string(), mProc->ToString().string());
                }
                SetExecuting(FALSE, 0, now);
            }
        }
        mOwner = NULL;
    }
    return NOERROR;
}

Boolean ProcessStats::ServiceState::IsInUse()
{
    return mOwner != NULL || mRestarting;
}

ECode ProcessStats::ServiceState::IsInUse(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsInUse();
    return NOERROR;
}

ECode ProcessStats::ServiceState::IsRestarting(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mRestarting;
    return NOERROR;
}

void ProcessStats::ServiceState::Add(
    /* [in] */ ServiceState* other)
{
    AddDurations(other);
    mRunCount += other->mRunCount;
    mStartedCount += other->mStartedCount;
    mBoundCount += other->mBoundCount;
    mExecCount += other->mExecCount;
}

void ProcessStats::ServiceState::ResetSafely(
    /* [in] */ Int64 now)
{
    ResetDurationsSafely();
    mRunCount = mRunState != STATE_NOTHING ? 1 : 0;
    mStartedCount = mStartedState != STATE_NOTHING ? 1 : 0;
    mBoundCount = mBoundState != STATE_NOTHING ? 1 : 0;
    mExecCount = mExecState != STATE_NOTHING ? 1 : 0;
    mRunStartTime = mStartedStartTime = mBoundStartTime = mExecStartTime = now;
}

void ProcessStats::ServiceState::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 now)
{
    WriteDurationsToParcel(out);
    out->WriteInt32(mRunCount);
    out->WriteInt32(mStartedCount);
    out->WriteInt32(mBoundCount);
    out->WriteInt32(mExecCount);
}

Boolean ProcessStats::ServiceState::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    if (!ReadDurationsFromParcel(in)) {
        return FALSE;
    }
    in->ReadInt32(&mRunCount);
    in->ReadInt32(&mStartedCount);
    in->ReadInt32(&mBoundCount);
    in->ReadInt32(&mExecCount);
    return TRUE;
}

void ProcessStats::ServiceState::CommitStateTime(
    /* [in] */ Int64 now)
{
    if (mRunState != STATE_NOTHING) {
        AddDuration(SERVICE_RUN + (mRunState*SERVICE_COUNT), now - mRunStartTime);
        mRunStartTime = now;
    }
    if (mStartedState != STATE_NOTHING) {
        AddDuration(SERVICE_STARTED + (mStartedState*SERVICE_COUNT),
                now - mStartedStartTime);
        mStartedStartTime = now;
    }
    if (mBoundState != STATE_NOTHING) {
        AddDuration(SERVICE_BOUND + (mBoundState*SERVICE_COUNT), now - mBoundStartTime);
        mBoundStartTime = now;
    }
    if (mExecState != STATE_NOTHING) {
        AddDuration(SERVICE_EXEC + (mExecState*SERVICE_COUNT), now - mExecStartTime);
        mExecStartTime = now;
    }
}

void ProcessStats::ServiceState::UpdateRunning(
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now)
{
    Int32 state = (mStartedState != STATE_NOTHING || mBoundState != STATE_NOTHING
            || mExecState != STATE_NOTHING) ? memFactor : STATE_NOTHING;
    if (mRunState != state) {
        if (mRunState != STATE_NOTHING) {
            AddDuration(SERVICE_RUN + (mRunState*SERVICE_COUNT),
                    now - mRunStartTime);
        }
        else if (state != STATE_NOTHING) {
            mRunCount++;
        }
        mRunState = state;
        mRunStartTime = now;
    }
}

ECode ProcessStats::ServiceState::SetStarted(
    /* [in] */ Boolean started,
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now)
{
    if (mOwner == NULL) {
        Slogger::W/*Wtf*/(TAG, "Starting service %s without owner", ToString().string());
    }
    mStarted = started;
    return UpdateStartedState(memFactor, now);
}

ECode ProcessStats::ServiceState::SetRestarting(
    /* [in] */ Boolean restarting,
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now)
{
    mRestarting = restarting;
    return UpdateStartedState(memFactor, now);
}

ECode ProcessStats::ServiceState::UpdateStartedState(
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now)
{
    Boolean wasStarted = mStartedState != STATE_NOTHING;
    Boolean started = mStarted || mRestarting;
    Int32 state = started ? memFactor : STATE_NOTHING;
    if (mStartedState != state) {
        if (mStartedState != STATE_NOTHING) {
            AddDuration(SERVICE_STARTED + (mStartedState*SERVICE_COUNT),
                    now - mStartedStartTime);
        }
        else if (started) {
            mStartedCount++;
        }
        mStartedState = state;
        mStartedStartTime = now;
        AutoPtr<ProcessState> temp;
        FAIL_RETURN(mProc->PullFixedProc(mPackage, (ProcessState**)&temp));
        mProc = temp;
        if (wasStarted != started) {
            if (started) {
                mProc->IncStartedServices(memFactor, now, mName);
            }
            else {
                mProc->DecStartedServices(memFactor, now, mName);
            }
        }
        UpdateRunning(memFactor, now);
    }
    return NOERROR;
}

ECode ProcessStats::ServiceState::SetBound(
    /* [in] */ Boolean bound,
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now)
{
    if (mOwner == NULL) {
        Slogger::W/*Wtf*/(TAG, "Binding service %s without owner", ToString().string());
    }
    Int32 state = bound ? memFactor : STATE_NOTHING;
    if (mBoundState != state) {
        if (mBoundState != STATE_NOTHING) {
            AddDuration(SERVICE_BOUND + (mBoundState*SERVICE_COUNT),
                    now - mBoundStartTime);
        }
        else if (bound) {
            mBoundCount++;
        }
        mBoundState = state;
        mBoundStartTime = now;
        UpdateRunning(memFactor, now);
    }
    return NOERROR;
}

ECode ProcessStats::ServiceState::SetExecuting(
    /* [in] */ Boolean executing,
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now)
{
    if (mOwner == NULL) {
        Slogger::W/*Wtf*/(TAG, "Executing service %s without owner", ToString().string());
    }
    Int32 state = executing ? memFactor : STATE_NOTHING;
    if (mExecState != state) {
        if (mExecState != STATE_NOTHING) {
            AddDuration(SERVICE_EXEC + (mExecState*SERVICE_COUNT), now - mExecStartTime);
        }
        else if (executing) {
            mExecCount++;
        }
        mExecState = state;
        mExecStartTime = now;
        UpdateRunning(memFactor, now);
    }
    return NOERROR;
}

Int64 ProcessStats::ServiceState::GetDuration(
    /* [in] */ Int32 opType,
    /* [in] */ Int32 curState,
    /* [in] */ Int64 startTime,
    /* [in] */ Int32 memFactor,
    /* [in] */ Int64 now)
{
    Int32 state = opType + (memFactor*SERVICE_COUNT);
    Int64 time = DurationsTable::GetDuration(state, now);
    if (curState == memFactor) {
        time += now - startTime;
    }
    return time;
}

String ProcessStats::ServiceState::ToString()
{
    StringBuilder sb;
    sb.Append("ServiceState{");
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int32 hashCode;
    system->IdentityHashCode((IServiceState*)this, &hashCode);
    String strHashCode = StringUtils::ToString(hashCode, 16);
    sb.Append(strHashCode);
    sb.Append(" ");
    sb.Append(mName);
    sb.Append(" pkg");
    sb.Append(mPackage);
    sb.Append(" proc=");
    sb.Append(strHashCode);
    sb.Append("}");
    return sb.ToString();
}

ECode ProcessStats::ServiceState::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = ToString();
    return NOERROR;
}

ECode ProcessStats::ServiceState::GetPackage(
    /* [out] */ String* package)
{
    VALIDATE_NOT_NULL(package)
    *package = mPackage;
    return NOERROR;
}

ECode ProcessStats::ServiceState::GetProcessName(
    /* [out] */ String* processName)
{
    VALIDATE_NOT_NULL(processName)
    *processName = mProcessName;
    return NOERROR;
}

ECode ProcessStats::ServiceState::GetRunState(
    /* [out] */ Int32* runState)
{
    VALIDATE_NOT_NULL(runState)
    *runState = mRunState;
    return NOERROR;
}

ECode ProcessStats::ServiceState::SetRunState(
    /* [in] */ Int32 runState)
{
    mRunState = runState;
    return NOERROR;
}

ECode ProcessStats::ServiceState::GetStartedState(
    /* [out] */ Int32* startedState)
{
    VALIDATE_NOT_NULL(startedState)
    *startedState = mStartedState;
    return NOERROR;
}

ECode ProcessStats::ServiceState::SetStartedState(
    /* [in] */ Int32 startedState)
{
    mStartedState = startedState;
    return NOERROR;
}

ECode ProcessStats::ServiceState::GetBoundState(
    /* [out] */ Int32* boundState)
{
    VALIDATE_NOT_NULL(boundState)
    *boundState = mBoundState;
    return NOERROR;
}

ECode ProcessStats::ServiceState::SetBoundState(
    /* [in] */ Int32 boundState)
{
    mBoundState = boundState;
    return NOERROR;
}

ECode ProcessStats::ServiceState::GetExecState(
    /* [out] */ Int32* execState)
{
    VALIDATE_NOT_NULL(execState)
    *execState = mExecState;
    return NOERROR;
}

ECode ProcessStats::ServiceState::SetExecState(
    /* [in] */ Int32 execState)
{
    mExecState = execState;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ProcessStats::PackageState, Object, IPackageState)

ECode ProcessStats::PackageState::constructor(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid)
{
    CArrayMap::New((IArrayMap**)&mProcesses);
    CArrayMap::New((IArrayMap**)&mServices);
    mUid = uid;
    mPackageName = packageName;
    return NOERROR;
}

ECode ProcessStats::PackageState::GetProcesses(
    /* [out] */ IArrayMap** processes)
{
    VALIDATE_NOT_NULL(processes)
    *processes = mProcesses;
    REFCOUNT_ADD(*processes)
    return NOERROR;
}

ECode ProcessStats::PackageState::GetServices(
    /* [out] */ IArrayMap** services)
{
    VALIDATE_NOT_NULL(services)
    *services = mServices;
    REFCOUNT_ADD(*services)
    return NOERROR;
}

ECode ProcessStats::PackageState::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName)
    *packageName = mPackageName;
    return NOERROR;
}

ECode ProcessStats::PackageState::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid = mUid;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ProcessStats::ProcessDataCollection, Object, IProcessDataCollection)

ProcessStats::ProcessDataCollection::ProcessDataCollection()
    : mTotalTime(0)
    , mNumPss(0)
    , mMinPss(0)
    , mAvgPss(0)
    , mMaxPss(0)
    , mMinUss(0)
    , mAvgUss(0)
    , mMaxUss(0)
{}

ECode ProcessStats::ProcessDataCollection::constructor(
    /* [in] */ ArrayOf<Int32>* screenStates,
    /* [in] */ ArrayOf<Int32>* memStates,
    /* [in] */ ArrayOf<Int32>* procStates)
{
    mScreenStates = screenStates;
    mMemStates = memStates;
    mProcStates = procStates;
    return NOERROR;
}

void ProcessStats::ProcessDataCollection::Print(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int64 overallTime,
    /* [in] */ Boolean full)
{
    if (mTotalTime > overallTime) {
        pw->Print(String("*"));
    }
    PrintPercent(pw, (Double) mTotalTime / (Double) overallTime);
    if (mNumPss > 0) {
        pw->Print(String(" ("));
        PrintSizeValue(pw, mMinPss * 1024);
        pw->Print(String("-"));
        PrintSizeValue(pw, mAvgPss * 1024);
        pw->Print(String("-"));
        PrintSizeValue(pw, mMaxPss * 1024);
        pw->Print(String("/"));
        PrintSizeValue(pw, mMinUss * 1024);
        pw->Print(String("-"));
        PrintSizeValue(pw, mAvgUss * 1024);
        pw->Print(String("-"));
        PrintSizeValue(pw, mMaxUss * 1024);
        if (full) {
            pw->Print(String(" over "));
            pw->Print(mNumPss);
        }
        pw->Print(String(")"));
    }
}

ECode ProcessStats::ProcessDataCollection::GetTotalTime(
    /* [out] */ Int64* totalTime)
{
    VALIDATE_NOT_NULL(totalTime)
    *totalTime = mTotalTime;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::SetTotalTime(
    /* [in] */ Int64 totalTime)
{
    mTotalTime = totalTime;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::GetNumPss(
    /* [out] */ Int64* numPss)
{
    VALIDATE_NOT_NULL(numPss)
    *numPss = mNumPss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::SetNumPss(
    /* [in] */ Int64 numPss)
{
    mNumPss = numPss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::GetMinPss(
    /* [out] */ Int64* minPss)
{
    VALIDATE_NOT_NULL(minPss)
    *minPss = mMinPss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::SetMinPss(
    /* [in] */ Int64 minPss)
{
    mMinPss = minPss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::GetAvgPss(
    /* [out] */ Int64* avgPss)
{
    VALIDATE_NOT_NULL(avgPss)
    *avgPss = mAvgPss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::SetAvgPss(
    /* [in] */ Int64 avgPss)
{
    mAvgPss = avgPss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::GetMaxPss(
    /* [out] */ Int64* maxPss)
{
    VALIDATE_NOT_NULL(maxPss)
    *maxPss = mMaxPss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::SetMaxPss(
    /* [in] */ Int64 maxPss)
{
    mMaxPss = maxPss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::GetMinUss(
    /* [out] */ Int64* minUss)
{
    VALIDATE_NOT_NULL(minUss)
    *minUss = mMinUss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::SetMinUss(
    /* [in] */ Int64 minUss)
{
    mMinUss = minUss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::GetAvgUss(
    /* [out] */ Int64* avgUss)
{
    VALIDATE_NOT_NULL(avgUss)
    *avgUss = mAvgUss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::SetAvgUss(
    /* [in] */ Int64 avgUss)
{
    mAvgUss = avgUss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::GetMaxUss(
    /* [out] */ Int64* maxUss)
{
    VALIDATE_NOT_NULL(maxUss)
    *maxUss = mMaxUss;
    return NOERROR;
}

ECode ProcessStats::ProcessDataCollection::SetMaxUss(
    /* [in] */ Int64 maxUss)
{
    mMaxUss = maxUss;
    return NOERROR;
}

CAR_INTERFACE_IMPL(ProcessStats::TotalMemoryUseCollection, Object, ITotalMemoryUseCollection)

ProcessStats::TotalMemoryUseCollection::TotalMemoryUseCollection()
    : mTotalTime(0)
    , mSysMemCachedWeight(0)
    , mSysMemFreeWeight(0)
    , mSysMemZRamWeight(0)
    , mSysMemKernelWeight(0)
    , mSysMemNativeWeight(0)
    , mSysMemSamples(0)
{
    mProcessStatePss = ArrayOf<Int64>::Alloc(STATE_COUNT);
    mProcessStateWeight = ArrayOf<Double>::Alloc(STATE_COUNT);
    mProcessStateTime = ArrayOf<Int64>::Alloc(STATE_COUNT);
    mProcessStateSamples = ArrayOf<Int32>::Alloc(STATE_COUNT);
    mSysMemUsage = ArrayOf<Int64>::Alloc(SYS_MEM_USAGE_COUNT);
}

ECode ProcessStats::TotalMemoryUseCollection::constructor(
    /* [in] */ ArrayOf<Int32>* screenStates,
    /* [in] */ ArrayOf<Int32>* memStates)
{
    mScreenStates = screenStates;
    mMemStates = memStates;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetTotalTime(
    /* [out] */ Int64* totalTime)
{
    VALIDATE_NOT_NULL(totalTime)
    *totalTime = mTotalTime;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetTotalTime(
    /* [in] */ Int64 totalTime)
{
    mTotalTime = totalTime;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetProcessStatePss(
    /* [out, callee] */ ArrayOf<Int64>** processStatePss)
{
    VALIDATE_NOT_NULL(processStatePss)
    *processStatePss = mProcessStatePss;
    REFCOUNT_ADD(*processStatePss)
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetProcessStatePss(
    /* [in] */ ArrayOf<Int64>* processStatePss)
{
    mProcessStatePss = processStatePss;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetProcessStateWeight(
    /* [out, callee] */ ArrayOf<Double>** processStateWeight)
{
    VALIDATE_NOT_NULL(processStateWeight)
    *processStateWeight = mProcessStateWeight;
    REFCOUNT_ADD(*processStateWeight)
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetProcessStateWeight(
    /* [in] */ ArrayOf<Double>* processStateWeight)
{
    mProcessStateWeight = processStateWeight;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetProcessStateTime(
    /* [out, callee] */ ArrayOf<Int64>** processStateTime)
{
    VALIDATE_NOT_NULL(processStateTime)
    *processStateTime = mProcessStateTime;
    REFCOUNT_ADD(*processStateTime)
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetProcessStateTime(
    /* [in] */ ArrayOf<Int64>* processStateTime)
{
    mProcessStateTime = processStateTime;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetProcessStateSamples(
    /* [out, callee] */ ArrayOf<Int32>** processStateSamples)
{
    VALIDATE_NOT_NULL(processStateSamples)
    *processStateSamples = mProcessStateSamples;
    REFCOUNT_ADD(*processStateSamples)
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetProcessStateSamples(
    /* [in] */ ArrayOf<Int32>* processStateSamples)
{
    mProcessStateSamples = processStateSamples;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetSysMemUsage(
    /* [out, callee] */ ArrayOf<Int64>** sysMemUsage)
{
    VALIDATE_NOT_NULL(sysMemUsage)
    *sysMemUsage = mSysMemUsage;
    REFCOUNT_ADD(*sysMemUsage)
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetSysMemUsage(
    /* [in] */ ArrayOf<Int64>* sysMemUsage)
{
    mSysMemUsage = sysMemUsage;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetSysMemCachedWeight(
    /* [out] */ Double* sysMemCachedWeight)
{
    VALIDATE_NOT_NULL(sysMemCachedWeight)
    *sysMemCachedWeight = mSysMemCachedWeight;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetSysMemCachedWeight(
    /* [in] */ Double sysMemCachedWeight)
{
    mSysMemCachedWeight = sysMemCachedWeight;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetSysMemFreeWeight(
    /* [out] */ Double* sysMemFreeWeight)
{
    VALIDATE_NOT_NULL(sysMemFreeWeight)
    *sysMemFreeWeight = mSysMemFreeWeight;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetSysMemFreeWeight(
    /* [in] */ Double sysMemFreeWeight)
{
    mSysMemFreeWeight = sysMemFreeWeight;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetSysMemZRamWeight(
    /* [out] */ Double* sysMemZRamWeight)
{
    VALIDATE_NOT_NULL(sysMemZRamWeight)
    *sysMemZRamWeight = mSysMemZRamWeight;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetSysMemZRamWeight(
    /* [in] */ Double sysMemZRamWeight)
{
    mSysMemZRamWeight = sysMemZRamWeight;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetSysMemKernelWeight(
    /* [out] */ Double* sysMemKernelWeight)
{
    VALIDATE_NOT_NULL(sysMemKernelWeight)
    *sysMemKernelWeight = mSysMemKernelWeight;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetSysMemKernelWeight(
    /* [in] */ Double sysMemKernelWeight)
{
    mSysMemKernelWeight = sysMemKernelWeight;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetSysMemNativeWeight(
    /* [out] */ Double* sysMemNativeWeight)
{
    VALIDATE_NOT_NULL(sysMemNativeWeight)
    *sysMemNativeWeight = mSysMemNativeWeight;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetSysMemNativeWeight(
    /* [in] */ Double sysMemNativeWeight)
{
    mSysMemNativeWeight = sysMemNativeWeight;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::GetSysMemSamples(
    /* [out] */ Int32* sysMemSamples)
{
    VALIDATE_NOT_NULL(sysMemSamples)
    *sysMemSamples = mSysMemSamples;
    return NOERROR;
}

ECode ProcessStats::TotalMemoryUseCollection::SetSysMemSamples(
    /* [in] */ Int32 sysMemSamples)
{
    mSysMemSamples = sysMemSamples;
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > ProcessStats::InitALL_MEM_ADJ()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(4);
    (*array)[0] = ADJ_MEM_FACTOR_NORMAL;
    (*array)[1] = ADJ_MEM_FACTOR_MODERATE;
    (*array)[2] = ADJ_MEM_FACTOR_LOW;
    (*array)[3] = ADJ_MEM_FACTOR_CRITICAL;
    return array;
}

AutoPtr<ArrayOf<Int32> > ProcessStats::InitALL_SCREEN_ADJ()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(2);
    (*array)[0] = ADJ_SCREEN_OFF;
    (*array)[1] = ADJ_SCREEN_ON;
    return array;
}

AutoPtr<ArrayOf<Int32> > ProcessStats::InitNON_CACHED_PROC_STATES()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(9);
    (*array)[0] = STATE_PERSISTENT;
    (*array)[1] = STATE_TOP;
    (*array)[2] = STATE_IMPORTANT_FOREGROUND;
    (*array)[3] = STATE_IMPORTANT_BACKGROUND;
    (*array)[4] = STATE_BACKUP;
    (*array)[5] = STATE_HEAVY_WEIGHT;
    (*array)[6] = STATE_SERVICE;
    (*array)[7] = STATE_SERVICE_RESTARTING;
    (*array)[8] = STATE_RECEIVER;
    return array;
}

AutoPtr<ArrayOf<Int32> > ProcessStats::InitBACKGROUND_PROC_STATES()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(7);
    (*array)[0] = STATE_IMPORTANT_FOREGROUND;
    (*array)[1] = STATE_IMPORTANT_BACKGROUND;
    (*array)[2] = STATE_BACKUP;
    (*array)[3] = STATE_HEAVY_WEIGHT;
    (*array)[4] = STATE_SERVICE;
    (*array)[5] = STATE_SERVICE_RESTARTING;
    (*array)[6] = STATE_RECEIVER;
    return array;
}

AutoPtr<ArrayOf<Int32> > ProcessStats::InitPROCESS_STATE_TO_STATE()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(14);
    (*array)[0] = STATE_PERSISTENT;               // ActivityManager.PROCESS_STATE_PERSISTENT
    (*array)[1] = STATE_PERSISTENT;               // ActivityManager.PROCESS_STATE_PERSISTENT_UI
    (*array)[2] = STATE_TOP;                      // ActivityManager.PROCESS_STATE_TOP
    (*array)[3] = STATE_IMPORTANT_FOREGROUND;     // ActivityManager.PROCESS_STATE_IMPORTANT_FOREGROUND
    (*array)[4] = STATE_IMPORTANT_BACKGROUND;     // ActivityManager.PROCESS_STATE_IMPORTANT_BACKGROUND
    (*array)[5] = STATE_BACKUP;                   // ActivityManager.PROCESS_STATE_BACKUP
    (*array)[6] = STATE_HEAVY_WEIGHT;             // ActivityManager.PROCESS_STATE_HEAVY_WEIGHT
    (*array)[7] = STATE_SERVICE;                  // ActivityManager.PROCESS_STATE_SERVICE
    (*array)[8] = STATE_RECEIVER;                 // ActivityManager.PROCESS_STATE_RECEIVER
    (*array)[9] = STATE_HOME;                     // ActivityManager.PROCESS_STATE_HOME
    (*array)[10] = STATE_LAST_ACTIVITY;            // ActivityManager.PROCESS_STATE_LAST_ACTIVITY
    (*array)[11] = STATE_CACHED_ACTIVITY;          // ActivityManager.PROCESS_STATE_CACHED_ACTIVITY
    (*array)[12] = STATE_CACHED_ACTIVITY_CLIENT;   // ActivityManager.PROCESS_STATE_CACHED_ACTIVITY_CLIENT
    (*array)[13] = STATE_CACHED_EMPTY;             // ActivityManager.PROCESS_STATE_CACHED_EMPTY
    return array;
}

AutoPtr<ArrayOf<Int32> > ProcessStats::InitALL_PROC_STATES()
{
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(13);
    (*array)[0] = STATE_TOP;
    (*array)[1] = STATE_IMPORTANT_FOREGROUND;
    (*array)[2] = STATE_IMPORTANT_BACKGROUND;
    (*array)[3] = STATE_BACKUP;
    (*array)[4] = STATE_HEAVY_WEIGHT;
    (*array)[5] = STATE_SERVICE;
    (*array)[6] = STATE_SERVICE_RESTARTING;
    (*array)[7] = STATE_RECEIVER;
    (*array)[8] = STATE_HOME;
    (*array)[9] = STATE_LAST_ACTIVITY;
    (*array)[10] = STATE_CACHED_ACTIVITY;
    (*array)[11] = STATE_CACHED_ACTIVITY_CLIENT;
    (*array)[12] = STATE_CACHED_EMPTY;
    return array;
}

AutoPtr<ArrayOf<String> > ProcessStats::InitSTATE_NAMES()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(14);
    (*array)[0] = String("Persist");
    (*array)[1] = String("Top    ");
    (*array)[2] = String("ImpFg  ");
    (*array)[3] = String("ImpBg  ");
    (*array)[4] = String("Backup ");
    (*array)[5] = String("HeavyWt");
    (*array)[6] = String("Service");
    (*array)[7] = String("ServRst");
    (*array)[8] = String("Receivr");
    (*array)[9] = String("Home   ");
    (*array)[10] = String("LastAct");
    (*array)[11] = String("CchAct ");
    (*array)[12] = String("CchCAct");
    (*array)[13] = String("CchEmty");
    return array;
}

AutoPtr<ArrayOf<String> > ProcessStats::InitADJ_SCREEN_NAMES_CSV()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    (*array)[0] = String("off");
    (*array)[1] = String("on");
    return array;
}

AutoPtr<ArrayOf<String> > ProcessStats::InitADJ_MEM_NAMES_CSV()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(4);
    (*array)[0] = String("norm");
    (*array)[1] = String("mod");
    (*array)[2] = String("low");
    (*array)[3] = String("crit");
    return array;
}

AutoPtr<ArrayOf<String> > ProcessStats::InitSTATE_NAMES_CSV()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(14);
    (*array)[0] = String("pers");
    (*array)[1] = String("top");
    (*array)[2] = String("impfg");
    (*array)[3] = String("impbg");
    (*array)[4] = String("backup");
    (*array)[5] = String("heavy");
    (*array)[6] = String("service");
    (*array)[7] = String("service-rs");
    (*array)[8] = String("receiver");
    (*array)[9] = String("home");
    (*array)[10] = String("lastact");
    (*array)[11] = String("cch-activity");
    (*array)[12] = String("cch-aclient");
    (*array)[13] = String("cch-empty");
    return array;
}

AutoPtr<ArrayOf<String> > ProcessStats::InitADJ_SCREEN_TAGS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(2);
    (*array)[0] = String("0");
    (*array)[1] = String("1");
    return array;
}

AutoPtr<ArrayOf<String> > ProcessStats::InitADJ_MEM_TAGS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(4);
    (*array)[0] = String("n");
    (*array)[1] = String("m");
    (*array)[2] = String("l");
    (*array)[3] = String("c");
    return array;
}

AutoPtr<ArrayOf<String> > ProcessStats::InitSTATE_TAGS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(14);
    (*array)[0] = String("p");
    (*array)[1] = String("t");
    (*array)[2] = String("f");
    (*array)[3] = String("b");
    (*array)[4] = String("u");
    (*array)[5] = String("w");
    (*array)[6] = String("s");
    (*array)[7] = String("x");
    (*array)[8] = String("r");
    (*array)[9] = String("h");
    (*array)[10] = String("l");
    (*array)[11] = String("a");
    (*array)[12] = String("c");
    (*array)[13] = String("e");
    return array;
}

const String ProcessStats::TAG("ProcessStats");
const Boolean ProcessStats::DEBUG;
const Boolean ProcessStats::DEBUG_PARCEL;

Int64 ProcessStats::COMMIT_PERIOD = 3*60*60*1000;  // Commit current stats every 3 hours
Int64 ProcessStats::COMMIT_UPTIME_PERIOD = 60*60*1000;  // Must have at least 1 hour elapsed

const AutoPtr<ArrayOf<Int32> > ProcessStats::ALL_MEM_ADJ = ProcessStats::InitALL_MEM_ADJ();
const AutoPtr<ArrayOf<Int32> > ProcessStats::ALL_SCREEN_ADJ = ProcessStats::InitALL_SCREEN_ADJ();
const AutoPtr<ArrayOf<Int32> > ProcessStats::NON_CACHED_PROC_STATES = ProcessStats::InitNON_CACHED_PROC_STATES();
const AutoPtr<ArrayOf<Int32> > ProcessStats::BACKGROUND_PROC_STATES = ProcessStats::InitBACKGROUND_PROC_STATES();
const AutoPtr<ArrayOf<Int32> > ProcessStats::PROCESS_STATE_TO_STATE = ProcessStats::InitPROCESS_STATE_TO_STATE() ;
const AutoPtr<ArrayOf<Int32> > ProcessStats::ALL_PROC_STATES = ProcessStats::InitALL_PROC_STATES();

const AutoPtr<ArrayOf<String> > ProcessStats::STATE_NAMES = ProcessStats::InitSTATE_NAMES();
const AutoPtr<ArrayOf<String> > ProcessStats::ADJ_SCREEN_NAMES_CSV = ProcessStats::InitADJ_SCREEN_NAMES_CSV();
const AutoPtr<ArrayOf<String> > ProcessStats::ADJ_MEM_NAMES_CSV = ProcessStats::InitADJ_MEM_NAMES_CSV();
const AutoPtr<ArrayOf<String> > ProcessStats::STATE_NAMES_CSV = ProcessStats::InitSTATE_NAMES_CSV();
const AutoPtr<ArrayOf<String> > ProcessStats::ADJ_SCREEN_TAGS = ProcessStats::InitADJ_SCREEN_TAGS();
const AutoPtr<ArrayOf<String> > ProcessStats::ADJ_MEM_TAGS = ProcessStats::InitADJ_MEM_TAGS();
const AutoPtr<ArrayOf<String> > ProcessStats::STATE_TAGS = ProcessStats::InitSTATE_TAGS();

const String ProcessStats::CSV_SEP("\t");
const Int32 ProcessStats::PARCEL_VERSION;
const Int32 ProcessStats::MAGIC;

Int32 ProcessStats::OFFSET_TYPE_SHIFT = 0;
Int32 ProcessStats::OFFSET_TYPE_MASK = 0xff;
Int32 ProcessStats::OFFSET_ARRAY_SHIFT = 8;
Int32 ProcessStats::OFFSET_ARRAY_MASK = 0xff;
Int32 ProcessStats::OFFSET_INDEX_SHIFT = 16;
Int32 ProcessStats::OFFSET_INDEX_MASK = 0xffff;

const Int32 ProcessStats::LONGS_SIZE;
const AutoPtr<ArrayOf<Int32> > ProcessStats::BAD_TABLE = ArrayOf<Int32>::Alloc(0);

CAR_INTERFACE_IMPL_2(ProcessStats, Object, IProcessStats, IParcelable)

ProcessStats::ProcessStats()
    : mFlags(0)
    , mMemFactor(STATE_NOTHING)
    , mStartTime(0)
    , mSysMemUsageTableSize(0)
    , mTimePeriodStartClock(0)
    , mTimePeriodStartRealtime(0)
    , mTimePeriodEndRealtime(0)
    , mTimePeriodStartUptime(0)
    , mTimePeriodEndUptime(0)
    , mRunning(FALSE)
    , mNextInt64(0)
    , mAddInt64TableSize(0)
{
    CProcessMap::New((IProcessMap**)&mPackages);
    CProcessMap::New((IProcessMap**)&mProcesses);
    mMemFactorDurations = ArrayOf<Int64>::Alloc(ADJ_COUNT);
    mSysMemUsageArgs = ArrayOf<Int64>::Alloc(SYS_MEM_USAGE_COUNT);
}

ECode ProcessStats::constructor()
{
    return Reset();
}

ECode ProcessStats::constructor(
    /* [in] */ Boolean running)
{
    mRunning = running;
    return Reset();
}

ECode ProcessStats::Add(
    /* [in] */ IProcessStats* _other)
{
    ProcessStats* other = (ProcessStats*)_other;
    AutoPtr<IArrayMap> pkgMap;
    other->mPackages->GetMap((IArrayMap**)&pkgMap);
    Int32 size;
    pkgMap->GetSize(&size);
    for (Int32 ip = 0; ip < size; ip++) {
        AutoPtr<IInterface> key;
        pkgMap->GetKeyAt(ip, (IInterface**)&key);
        String pkgName;
        ICharSequence::Probe(key)->ToString(&pkgName);
        AutoPtr<IInterface> value;
        pkgMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 uSize;
        uids->GetSize(&uSize);
        for (Int32 iu = 0; iu < uSize; iu++) {
            Int32 uid;
            uids->KeyAt(iu, &uid);
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            AutoPtr<ISparseArray> versions = ISparseArray::Probe(uValue);
            Int32 vSize;
            versions->GetSize(&vSize);
            for (Int32 iv = 0; iv < vSize; iv++) {
                Int32 vers;
                versions->KeyAt(iv, &vers);
                AutoPtr<IInterface> vValue;
                versions->ValueAt(iv, (IInterface**)&vValue);
                PackageState* otherState = (PackageState*)IPackageState::Probe(vValue);
                Int32 NPROCS;
                otherState->mProcesses->GetSize(&NPROCS);
                Int32 NSRVS;
                otherState->mServices->GetSize(&NSRVS);
                for (Int32 iproc = 0; iproc < NPROCS; iproc++) {
                    AutoPtr<IInterface> procValue;
                    otherState->mProcesses->GetValueAt(iproc, (IInterface**)&procValue);
                    ProcessState* otherProc = (ProcessState*)IProcessState::Probe(procValue);
                    if (otherProc->mCommonProcess != otherProc) {
                        if (DEBUG) Slogger::D(TAG, "Adding pkg %s uid %d vers %d proc %s",
                            pkgName.string(), uid, vers, otherProc->mName.string());
                        AutoPtr<IProcessState> _thisProc;
                        GetProcessStateLocked(pkgName, uid, vers,
                            otherProc->mName, (IProcessState**)&_thisProc);
                        AutoPtr<ProcessState> thisProc = (ProcessState*)_thisProc.Get();
                        if (thisProc->mCommonProcess == thisProc) {
                            if (DEBUG) Slogger::D(TAG, "Existing process is single-package, splitting");
                            thisProc->mMultiPackage = TRUE;
                            Int64 now = SystemClock::GetUptimeMillis();
                            AutoPtr<IPackageState> pkgState;
                            GetPackageStateLocked(pkgName, uid, vers, (IPackageState**)&pkgState);
                            thisProc = thisProc->Clone(thisProc->mPackage, now);
                            ((PackageState*)pkgState.Get())->mProcesses->Put(
                                CoreUtils::Convert(thisProc->mName), (IPackageState*)thisProc.Get());
                        }
                        thisProc->Add(otherProc);
                    }
                }
                for (Int32 isvc = 0; isvc < NSRVS; isvc++) {
                    AutoPtr<IInterface> svcValue;
                    otherState->mServices->GetValueAt(isvc, (IInterface**)&svcValue);
                    ServiceState* otherSvc = (ServiceState*)IServiceState::Probe(svcValue);
                    if (DEBUG) Slogger::D(TAG, "Adding pkg %s uid %d service %s",
                        pkgName.string(), uid, otherSvc->mName.string());
                    AutoPtr<IServiceState> thisSvc;
                    GetServiceStateLocked(pkgName, uid, vers, otherSvc->mProcessName,
                        otherSvc->mName, (IServiceState**)&thisSvc);
                    ServiceState* ss = (ServiceState*)thisSvc.Get();
                    ss->Add(otherSvc);
                }
            }
        }
    }

    AutoPtr<IArrayMap> procMap;
    other->mProcesses->GetMap((IArrayMap**)&procMap);
    procMap->GetSize(&size);
    for (Int32 ip = 0; ip < size; ip++) {
        AutoPtr<IInterface> value;
        procMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 uSize;
        uids->GetSize(&uSize);
        for (Int32 iu = 0; iu < uSize; iu++) {
            Int32 uid;
            uids->KeyAt(iu, &uid);
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            ProcessState* otherProc = (ProcessState*)IProcessState::Probe(uValue);
            AutoPtr<IInterface> obj;
            mProcesses->Get(otherProc->mName, uid, (IInterface**)&obj);
            AutoPtr<ProcessState> thisProc = (ProcessState*)IProcessState::Probe(obj);
            if (DEBUG) Slogger::D(TAG, "Adding uid %d proc %s", uid, otherProc->mName.string());
            if (thisProc == NULL) {
                if (DEBUG) Slogger::D(TAG, "Creating new process!");
                thisProc = new ProcessState();
                thisProc->constructor(this, otherProc->mPackage, uid, otherProc->mVersion,
                        otherProc->mName);
                mProcesses->Put(otherProc->mName, uid, (IProcessState*)thisProc);
                AutoPtr<IPackageState> _thisState;
                GetPackageStateLocked(otherProc->mPackage, uid,
                    otherProc->mVersion, (IPackageState**)&_thisState);
                PackageState* thisState = (PackageState*)_thisState.Get();
                AutoPtr<ICharSequence> key = CoreUtils::Convert(otherProc->mName);
                Boolean res;
                thisState->mProcesses->ContainsKey(key, &res);
                if (!res) {
                    thisState->mProcesses->Put(key, (IProcessState*)thisProc);
                }
            }
            thisProc->Add(otherProc);
        }
    }

    for (Int32 i = 0; i < ADJ_COUNT; i++) {
        if (DEBUG) Slogger::D(TAG, "Total duration #%d inc by %lld from %lld",
                i, (*other->mMemFactorDurations)[i], (*mMemFactorDurations)[i]);
        (*mMemFactorDurations)[i] += (*other->mMemFactorDurations)[i];
    }

    for (Int32 i = 0; i < other->mSysMemUsageTableSize; i++) {
        Int32 ent = (*other->mSysMemUsageTable)[i];
        Int32 state = (ent >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;
        AutoPtr<ArrayOf<Int64> > longs = other->mInt64s[(ent >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK];
        AddSysMemUsage(state, *longs, ((ent >> OFFSET_INDEX_SHIFT) & OFFSET_INDEX_MASK));
    }

    if (other->mTimePeriodStartClock < mTimePeriodStartClock) {
        mTimePeriodStartClock = other->mTimePeriodStartClock;
        mTimePeriodStartClockStr = other->mTimePeriodStartClockStr;
    }
    mTimePeriodEndRealtime += other->mTimePeriodEndRealtime - other->mTimePeriodStartRealtime;
    mTimePeriodEndUptime += other->mTimePeriodEndUptime - other->mTimePeriodStartUptime;

    return NOERROR;
}

ECode ProcessStats::AddSysMemUsage(
    /* [in] */ Int64 cachedMem,
    /* [in] */ Int64 freeMem,
    /* [in] */ Int64 zramMem,
    /* [in] */ Int64 kernelMem,
    /* [in] */ Int64 nativeMem)
{
    if (mMemFactor != STATE_NOTHING) {
        Int32 state = mMemFactor * STATE_COUNT;
        (*mSysMemUsageArgs)[SYS_MEM_USAGE_SAMPLE_COUNT] = 1;
        for (Int32 i = 0; i < 3; i++) {
            (*mSysMemUsageArgs)[SYS_MEM_USAGE_CACHED_MINIMUM + i] = cachedMem;
            (*mSysMemUsageArgs)[SYS_MEM_USAGE_FREE_MINIMUM + i] = freeMem;
            (*mSysMemUsageArgs)[SYS_MEM_USAGE_ZRAM_MINIMUM + i] = zramMem;
            (*mSysMemUsageArgs)[SYS_MEM_USAGE_KERNEL_MINIMUM + i] = kernelMem;
            (*mSysMemUsageArgs)[SYS_MEM_USAGE_NATIVE_MINIMUM + i] = nativeMem;
        }
        AddSysMemUsage(state, *mSysMemUsageArgs, 0);
    }
    return NOERROR;
}

void ProcessStats::AddSysMemUsage(
    /* [in] */ Int32 state,
    /* [in] */ const ArrayOf<Int64>& data,
    /* [in] */ Int32 dataOff)
{
    Int32 idx = BinarySearch(*mSysMemUsageTable, mSysMemUsageTableSize, state);
    Int32 off;
    if (idx >= 0) {
        off = (*mSysMemUsageTable)[idx];
    }
    else {
        mAddInt64Table = mSysMemUsageTable;
        mAddInt64TableSize = mSysMemUsageTableSize;
        off = AddInt64Data(~idx, state, SYS_MEM_USAGE_COUNT);
        mSysMemUsageTable = mAddInt64Table;
        mSysMemUsageTableSize = mAddInt64TableSize;
    }
    AutoPtr<ArrayOf<Int64> > longs = mInt64s[(off >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK];
    idx = (off >> OFFSET_INDEX_SHIFT) & OFFSET_INDEX_MASK;
    AddSysMemUsage(*longs, idx, data, dataOff);
}

void ProcessStats::AddSysMemUsage(
    /* [in] */ ArrayOf<Int64>& dstData,
    /* [in] */ Int32 dstOff,
    /* [in] */ const ArrayOf<Int64>& addData,
    /* [in] */ Int32 addOff)
{
    Int64 dstCount = dstData[dstOff+SYS_MEM_USAGE_SAMPLE_COUNT];
    Int64 addCount = addData[addOff+SYS_MEM_USAGE_SAMPLE_COUNT];
    if (dstCount == 0) {
        dstData[dstOff+SYS_MEM_USAGE_SAMPLE_COUNT] = addCount;
        for (Int32 i = SYS_MEM_USAGE_CACHED_MINIMUM; i < SYS_MEM_USAGE_COUNT; i++) {
            dstData[dstOff+i] = addData[addOff+i];
        }
    }
    else if (addCount > 0) {
        dstData[dstOff+SYS_MEM_USAGE_SAMPLE_COUNT] = dstCount + addCount;
        for (Int32 i = SYS_MEM_USAGE_CACHED_MINIMUM; i < SYS_MEM_USAGE_COUNT; i+=3) {
            if (dstData[dstOff+i] > addData[addOff+i]) {
                dstData[dstOff+i] = addData[addOff+i];
            }
            dstData[dstOff+i+1] = (Int64)(
                ((dstData[dstOff+i+1]*(Double)dstCount)
                    + (addData[addOff+i+1]*(Double)addCount))
                    / (dstCount+addCount) );
            if (dstData[dstOff+i+2] < addData[addOff+i+2]) {
                dstData[dstOff+i+2] = addData[addOff+i+2];
            }
        }
    }
}

void ProcessStats::PrintScreenLabel(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 offset)
{
    switch (offset) {
        case ADJ_NOTHING:
            pw->Print(String("     "));
            break;
        case ADJ_SCREEN_OFF:
            pw->Print(String("SOff/"));
            break;
        case ADJ_SCREEN_ON:
            pw->Print(String("SOn /"));
            break;
        default:
            pw->Print(String("\?\?\?\?/"));
            break;
    }
}

void ProcessStats::PrintScreenLabelCsv(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 offset)
{
    switch (offset) {
        case ADJ_NOTHING:
            break;
        case ADJ_SCREEN_OFF:
            pw->Print((*ADJ_SCREEN_NAMES_CSV)[0]);
            break;
        case ADJ_SCREEN_ON:
            pw->Print((*ADJ_SCREEN_NAMES_CSV)[1]);
            break;
        default:
            pw->Print(String("???"));
            break;
    }
}

void ProcessStats::PrintMemLabel(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 offset,
    /* [in] */ Char32 sep)
{
    switch (offset) {
        case ADJ_NOTHING:
            pw->Print(String("    "));
            if (sep != 0) pw->PrintChar(' ');
            break;
        case ADJ_MEM_FACTOR_NORMAL:
            pw->Print(String("Norm"));
            if (sep != 0) pw->PrintChar(sep);
            break;
        case ADJ_MEM_FACTOR_MODERATE:
            pw->Print(String("Mod "));
            if (sep != 0) pw->PrintChar(sep);
            break;
        case ADJ_MEM_FACTOR_LOW:
            pw->Print(String("Low "));
            if (sep != 0) pw->PrintChar(sep);
            break;
        case ADJ_MEM_FACTOR_CRITICAL:
            pw->Print(String("Crit"));
            if (sep != 0) pw->PrintChar(sep);
            break;
        default:
            pw->Print(String("\?\?\?\?"));
            if (sep != 0) pw->PrintChar(sep);
            break;
    }
}

void ProcessStats::PrintMemLabelCsv(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 offset)
{
    if (offset >= ADJ_MEM_FACTOR_NORMAL) {
        if (offset <= ADJ_MEM_FACTOR_CRITICAL) {
            pw->Print((*ADJ_MEM_NAMES_CSV)[offset]);
        }
        else {
            pw->Print(String("\?\?\?"));
        }
    }
}

Int64 ProcessStats::DumpSingleTime(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ const ArrayOf<Int64>& durations,
    /* [in] */ Int32 curState,
    /* [in] */ Int64 curStartTime,
    /* [in] */ Int64 now)
{
    Int64 totalTime = 0;
    Int32 printedScreen = -1;
    for (Int32 iscreen = 0; iscreen < ADJ_COUNT; iscreen+=ADJ_SCREEN_MOD) {
        Int32 printedMem = -1;
        for (Int32 imem = 0; imem < ADJ_MEM_FACTOR_COUNT; imem++) {
            Int32 state = imem + iscreen;
            Int64 time = durations[state];
            String running("");
            if (curState == state) {
                time += now - curStartTime;
                if (pw != NULL) {
                    running = " (running)";
                }
            }
            if (time != 0) {
                if (pw != NULL) {
                    pw->Print(prefix);
                    PrintScreenLabel(pw, printedScreen != iscreen
                            ? iscreen : STATE_NOTHING);
                    printedScreen = iscreen;
                    PrintMemLabel(pw, printedMem != imem ? imem : STATE_NOTHING, (Char32)0);
                    printedMem = imem;
                    pw->Print(String(": "));
                    TimeUtils::FormatDuration(time, pw);
                    pw->Println(running);
                }
                totalTime += time;
            }
        }
    }
    if (totalTime != 0 && pw != NULL) {
        pw->Print(prefix);
        pw->Print(String("    TOTAL: "));
        TimeUtils::FormatDuration(totalTime, pw);
        pw->Println();
    }
    return totalTime;
}

void ProcessStats::DumpAdjTimesCheckin(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& sep,
    /* [in] */ const ArrayOf<Int64>& durations,
    /* [in] */ Int32 curState,
    /* [in] */ Int64 curStartTime,
    /* [in] */ Int64 now)
{
    for (Int32 iscreen = 0; iscreen < ADJ_COUNT; iscreen += ADJ_SCREEN_MOD) {
        for (Int32 imem = 0; imem < ADJ_MEM_FACTOR_COUNT; imem++) {
            Int32 state = imem+iscreen;
            Int64 time = durations[state];
            if (curState == state) {
                time += now - curStartTime;
            }
            if (time != 0) {
                PrintAdjTagAndValue(pw, state, time);
            }
        }
    }
}

void ProcessStats::DumpServiceTimeCheckin(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& label,
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 vers,
    /* [in] */ const String& serviceName,
    /* [in] */ ServiceState* svc,
    /* [in] */ Int32 serviceType,
    /* [in] */ Int32 opCount,
    /* [in] */ Int32 curState,
    /* [in] */ Int64 curStartTime,
    /* [in] */ Int64 now)
{
    if (opCount <= 0) {
        return;
    }
    pw->Print(label);
    pw->Print(String(","));
    pw->Print(packageName);
    pw->Print(String(","));
    pw->Print(uid);
    pw->Print(String(","));
    pw->Print(vers);
    pw->Print(String(","));
    pw->Print(serviceName);
    pw->Print(String(","));
    pw->Print(opCount);
    Boolean didCurState = FALSE;
    for (Int32 i = 0; i < svc->mDurationsTableSize; i++) {
        Int32 off = (*svc->mDurationsTable)[i];
        Int32 type = (off >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;
        Int32 memFactor = type / ServiceState::SERVICE_COUNT;
        type %= ServiceState::SERVICE_COUNT;
        if (type != serviceType) {
            continue;
        }
        Int64 time = svc->mStats->GetInt64(off, 0);
        if (curState == memFactor) {
            didCurState = TRUE;
            time += now - curStartTime;
        }
        PrintAdjTagAndValue(pw, memFactor, time);
    }
    if (!didCurState && curState != STATE_NOTHING) {
        PrintAdjTagAndValue(pw, curState, now - curStartTime);
    }
    pw->Println();
}

ECode ProcessStats::ComputeProcessData(
    /* [in] */ IProcessState* _proc,
    /* [in] */ IProcessDataCollection* _data,
    /* [in] */ Int64 now)
{
    ProcessState* proc = (ProcessState*)_proc;
    ProcessDataCollection* data = (ProcessDataCollection*)_data;
    data->mTotalTime = 0;
    data->mNumPss = data->mMinPss = data->mAvgPss = data->mMaxPss =
            data->mMinUss = data->mAvgUss = data->mMaxUss = 0;
    for (Int32 is = 0; is < data->mScreenStates->GetLength(); is++) {
        for (Int32 im = 0; im < data->mMemStates->GetLength(); im++) {
            for (Int32 ip = 0; ip < data->mProcStates->GetLength(); ip++) {
                Int32 bucket = (((*data->mScreenStates)[is] + (*data->mMemStates)[im]) * STATE_COUNT)
                        + (*data->mProcStates)[ip];
                data->mTotalTime += proc->GetDuration(bucket, now);
                Int64 samples = proc->GetPssSampleCount(bucket);
                if (samples > 0) {
                    Int64 minPss = proc->GetPssMinimum(bucket);
                    Int64 avgPss = proc->GetPssAverage(bucket);
                    Int64 maxPss = proc->GetPssMaximum(bucket);
                    Int64 minUss = proc->GetPssUssMinimum(bucket);
                    Int64 avgUss = proc->GetPssUssAverage(bucket);
                    Int64 maxUss = proc->GetPssUssMaximum(bucket);
                    if (data->mNumPss == 0) {
                        data->mMinPss = minPss;
                        data->mAvgPss = avgPss;
                        data->mMaxPss = maxPss;
                        data->mMinUss = minUss;
                        data->mAvgUss = avgUss;
                        data->mMaxUss = maxUss;
                    }
                    else {
                        if (minPss < data->mMinPss) {
                            data->mMinPss = minPss;
                        }
                        data->mAvgPss = (Int64)( ((data->mAvgPss*(Double)data->mNumPss)
                                + (avgPss*(Double)samples)) / (data->mNumPss+samples) );
                        if (maxPss > data->mMaxPss) {
                            data->mMaxPss = maxPss;
                        }
                        if (minUss < data->mMinUss) {
                            data->mMinUss = minUss;
                        }
                        data->mAvgUss = (Int64)( ((data->mAvgUss*(Double)data->mNumPss)
                                + (avgUss*(Double)samples)) / (data->mNumPss+samples) );
                        if (maxUss > data->mMaxUss) {
                            data->mMaxUss = maxUss;
                        }
                    }
                    data->mNumPss += samples;
                }
            }
        }
    }
    return NOERROR;
}

Int64 ProcessStats::ComputeProcessTimeLocked(
    /* [in] */ ProcessState* proc,
    /* [in] */ const ArrayOf<Int32>& screenStates,
    /* [in] */ const ArrayOf<Int32>& memStates,
    /* [in] */ const ArrayOf<Int32>& procStates,
    /* [in] */ Int64 now)
{
    Int64 totalTime = 0;
    /*
    for (Int32 i = 0; i < proc->mDurationsTableSize; i++) {
        Int32 val = (*proc->mDurationsTable)[i];
        totalTime += proc->mState.GetInt64(val, 0);
        if ((val&0xff) == proc->mCurState) {
            totalTime += now - proc->mStartTime;
        }
    }
    */
    for (Int32 is = 0; is < screenStates.GetLength(); is++) {
        for (Int32 im = 0; im < memStates.GetLength(); im++) {
            for (Int32 ip = 0; ip < procStates.GetLength(); ip++) {
                Int32 bucket = ((screenStates[is] + memStates[im]) * STATE_COUNT)
                        + procStates[ip];
                totalTime += proc->GetDuration(bucket, now);
            }
        }
    }
    proc->mTmpTotalTime = totalTime;
    return totalTime;
}

ECode ProcessStats::ComputeTotalMemoryUse(
    /* [in] */ ITotalMemoryUseCollection* _data,
    /* [in] */ Int64 now)
{
    TotalMemoryUseCollection* data = (TotalMemoryUseCollection*)_data;
    data->mTotalTime = 0;
    for (Int32 i = 0; i < STATE_COUNT; i++) {
        (*data->mProcessStateWeight)[i] = 0;
        (*data->mProcessStatePss)[i] = 0;
        (*data->mProcessStateTime)[i] = 0;
        (*data->mProcessStateSamples)[i] = 0;
    }
    for (Int32 i = 0; i < SYS_MEM_USAGE_COUNT; i++) {
        (*data->mSysMemUsage)[i] = 0;
    }
    data->mSysMemCachedWeight = 0;
    data->mSysMemFreeWeight = 0;
    data->mSysMemZRamWeight = 0;
    data->mSysMemKernelWeight = 0;
    data->mSysMemNativeWeight = 0;
    data->mSysMemSamples = 0;
    AutoPtr<ArrayOf<Int64> > totalMemUsage = ArrayOf<Int64>::Alloc(SYS_MEM_USAGE_COUNT);
    for (Int32 i = 0; i < mSysMemUsageTableSize; i++) {
        Int32 ent = (*mSysMemUsageTable)[i];
        AutoPtr<ArrayOf<Int64> > longs = mInt64s[(ent >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK];
        Int32 idx = (ent >> OFFSET_INDEX_SHIFT) & OFFSET_INDEX_MASK;
        AddSysMemUsage(*totalMemUsage, 0, *longs, idx);
    }
    for (Int32 is = 0; is < data->mScreenStates->GetLength(); is++) {
        for (Int32 im = 0; im < data->mMemStates->GetLength(); im++) {
            Int32 memBucket = (*data->mScreenStates)[is] + (*data->mMemStates)[im];
            Int32 stateBucket = memBucket * STATE_COUNT;
            Int64 memTime = (*mMemFactorDurations)[memBucket];
            if (mMemFactor == memBucket) {
                memTime += now - mStartTime;
            }
            data->mTotalTime += memTime;
            Int32 sysIdx = BinarySearch(*mSysMemUsageTable, mSysMemUsageTableSize, stateBucket);
            AutoPtr<ArrayOf<Int64> > longs = totalMemUsage;
            Int32 idx = 0;
            if (sysIdx >= 0) {
                Int32 ent = (*mSysMemUsageTable)[sysIdx];
                AutoPtr<ArrayOf<Int64> > tmpInt64s = mInt64s[(ent >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK];
                Int32 tmpIdx = (ent >> OFFSET_INDEX_SHIFT) & OFFSET_INDEX_MASK;
                if ((*tmpInt64s)[tmpIdx+SYS_MEM_USAGE_SAMPLE_COUNT] >= 3) {
                    AddSysMemUsage(*data->mSysMemUsage, 0, *longs, idx);
                    longs = tmpInt64s;
                    idx = tmpIdx;
                }
            }
            data->mSysMemCachedWeight += (*longs)[idx+SYS_MEM_USAGE_CACHED_AVERAGE]
                    * (Double)memTime;
            data->mSysMemFreeWeight += (*longs)[idx+SYS_MEM_USAGE_FREE_AVERAGE]
                    * (Double)memTime;
            data->mSysMemZRamWeight += (*longs)[idx+SYS_MEM_USAGE_ZRAM_AVERAGE]
                    * (Double)memTime;
            data->mSysMemKernelWeight += (*longs)[idx+SYS_MEM_USAGE_KERNEL_AVERAGE]
                    * (Double)memTime;
            data->mSysMemNativeWeight += (*longs)[idx+SYS_MEM_USAGE_NATIVE_AVERAGE]
                    * (Double)memTime;
            data->mSysMemSamples += (*longs)[idx+SYS_MEM_USAGE_SAMPLE_COUNT];
         }
    }

    AutoPtr<IArrayMap> procMap;
    mProcesses->GetMap((IArrayMap**)&procMap);
    Int32 size;
    procMap->GetSize(&size);
    for (Int32 iproc = 0; iproc < size; iproc++) {
        AutoPtr<IInterface> value;
        procMap->GetValueAt(iproc, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 uSize;
        uids->GetSize(&uSize);
        for (Int32 iu = 0; iu < uSize; iu++) {
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            ProcessState* proc = (ProcessState*)IProcessState::Probe(uValue);
            AutoPtr<PssAggr> fgPss = new PssAggr();
            AutoPtr<PssAggr> bgPss = new PssAggr();
            AutoPtr<PssAggr> cachedPss = new PssAggr();
            Boolean havePss = FALSE;
            for (Int32 i = 0; i < proc->mDurationsTableSize; i++) {
                Int32 off = (*proc->mDurationsTable)[i];
                Int32 type = (off >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;
                Int32 procState = type % STATE_COUNT;
                Int64 samples = proc->GetPssSampleCount(type);
                if (samples > 0) {
                    Int64 avg = proc->GetPssAverage(type);
                    havePss = TRUE;
                    if (procState <= STATE_IMPORTANT_FOREGROUND) {
                        fgPss->Add(avg, samples);
                    }
                    else if (procState <= STATE_RECEIVER) {
                        bgPss->Add(avg, samples);
                    }
                    else {
                        cachedPss->Add(avg, samples);
                    }
                }
            }
            if (!havePss) {
                continue;
            }
            Boolean fgHasBg = FALSE;
            Boolean fgHasCached = FALSE;
            Boolean bgHasCached = FALSE;
            if (fgPss->mSamples < 3 && bgPss->mSamples > 0) {
                fgHasBg = TRUE;
                fgPss->Add(bgPss->mPss, bgPss->mSamples);
            }
            if (fgPss->mSamples < 3 && cachedPss->mSamples > 0) {
                fgHasCached = TRUE;
                fgPss->Add(cachedPss->mPss, cachedPss->mSamples);
            }
            if (bgPss->mSamples < 3 && cachedPss->mSamples > 0) {
                bgHasCached = TRUE;
                bgPss->Add(cachedPss->mPss, cachedPss->mSamples);
            }
            if (bgPss->mSamples < 3 && !fgHasBg && fgPss->mSamples > 0) {
                bgPss->Add(fgPss->mPss, fgPss->mSamples);
            }
            if (cachedPss->mSamples < 3 && !bgHasCached && bgPss->mSamples > 0) {
                cachedPss->Add(bgPss->mPss, bgPss->mSamples);
            }
            if (cachedPss->mSamples < 3 && !fgHasCached && fgPss->mSamples > 0) {
                cachedPss->Add(fgPss->mPss, fgPss->mSamples);
            }
            for (Int32 i = 0; i < proc->mDurationsTableSize; i++) {
                Int32 off = (*proc->mDurationsTable)[i];
                Int32 type = (off >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;
                Int64 time = GetInt64(off, 0);
                if (proc->mCurState == type) {
                    time += now - proc->mStartTime;
                }
                Int32 procState = type % STATE_COUNT;
                (*data->mProcessStateTime)[procState] += time;
                Int64 samples = proc->GetPssSampleCount(type);
                Int64 avg;
                if (samples > 0) {
                    avg = proc->GetPssAverage(type);
                }
                else if (procState <= STATE_IMPORTANT_FOREGROUND) {
                    samples = fgPss->mSamples;
                    avg = fgPss->mPss;
                }
                else if (procState <= STATE_RECEIVER) {
                    samples = bgPss->mSamples;
                    avg = bgPss->mPss;
                }
                else {
                    samples = cachedPss->mSamples;
                    avg = cachedPss->mPss;
                }
                Double newAvg = ( ( (*data->mProcessStatePss)[procState]
                        * (Double)(*data->mProcessStateSamples)[procState])
                            + (avg*(Double)samples)
                        ) / ((*data->mProcessStateSamples)[procState]+samples);
                (*data->mProcessStatePss)[procState] = (Int64)newAvg;
                (*data->mProcessStateSamples)[procState] += samples;
                (*data->mProcessStateWeight)[procState] += avg * (Double)time;
            }
        }
    }
    return NOERROR;
}

void ProcessStats::DumpProcessState(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ ProcessState* proc,
    /* [in] */ const ArrayOf<Int32>& screenStates,
    /* [in] */ const ArrayOf<Int32>& memStates,
    /* [in] */ const ArrayOf<Int32>& procStates,
    /* [in] */ Int64 now)
{
    Int64 totalTime = 0;
    Int32 printedScreen = -1;
    for (Int32 is = 0; is < screenStates.GetLength(); is++) {
        Int32 printedMem = -1;
        for (Int32 im = 0; im < memStates.GetLength(); im++) {
            for (Int32 ip = 0; ip < procStates.GetLength(); ip++) {
                Int32 iscreen = screenStates[is];
                Int32 imem = memStates[im];
                Int32 bucket = ((iscreen + imem) * STATE_COUNT) + procStates[ip];
                Int64 time = proc->GetDuration(bucket, now);
                String running("");
                if (proc->mCurState == bucket) {
                    running = " (running)";
                }
                if (time != 0) {
                    pw->Print(prefix);
                    if (screenStates.GetLength() > 1) {
                        PrintScreenLabel(pw, printedScreen != iscreen
                                ? iscreen : STATE_NOTHING);
                        printedScreen = iscreen;
                    }
                    if (memStates.GetLength() > 1) {
                        PrintMemLabel(pw, printedMem != imem ? imem : STATE_NOTHING, '/');
                        printedMem = imem;
                    }
                    pw->Print((*STATE_NAMES)[procStates[ip]]);
                    pw->Print(String(": "));
                    TimeUtils::FormatDuration(time, pw);
                    pw->Println(running);
                    totalTime += time;
                }
            }
        }
    }
    if (totalTime != 0) {
        pw->Print(prefix);
        if (screenStates.GetLength() > 1) {
            PrintScreenLabel(pw, STATE_NOTHING);
        }
        if (memStates.GetLength() > 1) {
            PrintMemLabel(pw, STATE_NOTHING, '/');
        }
        pw->Print(String("TOTAL  : "));
        TimeUtils::FormatDuration(totalTime, pw);
        pw->Println();
    }
}

void ProcessStats::DumpProcessPss(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ ProcessState* proc,
    /* [in] */ const ArrayOf<Int32>& screenStates,
    /* [in] */ const ArrayOf<Int32>& memStates,
    /* [in] */ const ArrayOf<Int32>& procStates)
{
    Boolean printedHeader = FALSE;
    Int32 printedScreen = -1;
    for (Int32 is = 0; is < screenStates.GetLength(); is++) {
        Int32 printedMem = -1;
        for (Int32 im = 0; im < memStates.GetLength(); im++) {
            for (Int32 ip = 0; ip < procStates.GetLength(); ip++) {
                Int32 iscreen = screenStates[is];
                Int32 imem = memStates[im];
                Int32 bucket = ((iscreen + imem) * STATE_COUNT) + procStates[ip];
                Int64 count = proc->GetPssSampleCount(bucket);
                if (count > 0) {
                    if (!printedHeader) {
                        pw->Print(prefix);
                        pw->Print(String("PSS/USS ("));
                        pw->Print(proc->mPssTableSize);
                        pw->Println(String(" entries):"));
                        printedHeader = TRUE;
                    }
                    pw->Print(prefix);
                    pw->Print(String("  "));
                    if (screenStates.GetLength() > 1) {
                        PrintScreenLabel(pw, printedScreen != iscreen
                                ? iscreen : STATE_NOTHING);
                        printedScreen = iscreen;
                    }
                    if (memStates.GetLength() > 1) {
                        PrintMemLabel(pw, printedMem != imem ? imem : STATE_NOTHING, '/');
                        printedMem = imem;
                    }
                    pw->Print((*STATE_NAMES)[procStates[ip]]);
                    pw->Print(String(": "));
                    pw->Print(count);
                    pw->Print(String(" samples "));
                    PrintSizeValue(pw, proc->GetPssMinimum(bucket) * 1024);
                    pw->Print(String(" "));
                    PrintSizeValue(pw, proc->GetPssAverage(bucket) * 1024);
                    pw->Print(String(" "));
                    PrintSizeValue(pw, proc->GetPssMaximum(bucket) * 1024);
                    pw->Print(String(" / "));
                    PrintSizeValue(pw, proc->GetPssUssMinimum(bucket) * 1024);
                    pw->Print(String(" "));
                    PrintSizeValue(pw, proc->GetPssUssAverage(bucket) * 1024);
                    pw->Print(String(" "));
                    PrintSizeValue(pw, proc->GetPssUssMaximum(bucket) * 1024);
                    pw->Println();
                }
            }
        }
    }
    if (proc->mNumExcessiveWake != 0) {
        pw->Print(prefix);
        pw->Print(String("Killed for excessive wake locks: "));
        pw->Print(proc->mNumExcessiveWake);
        pw->Println(String(" times"));
    }
    if (proc->mNumExcessiveCpu != 0) {
        pw->Print(prefix);
        pw->Print(String("Killed for excessive CPU use: "));
        pw->Print(proc->mNumExcessiveCpu);
        pw->Println(String(" times"));
    }
    if (proc->mNumCachedKill != 0) {
        pw->Print(prefix);
        pw->Print(String("Killed from cached state: "));
        pw->Print(proc->mNumCachedKill);
        pw->Print(String(" times from pss "));
        PrintSizeValue(pw, proc->mMinCachedKillPss * 1024);
        pw->Print(String("-"));
        PrintSizeValue(pw, proc->mAvgCachedKillPss * 1024);
        pw->Print(String("-"));
        PrintSizeValue(pw, proc->mMaxCachedKillPss * 1024);
        pw->Println();
    }
}

Int64 ProcessStats::GetSysMemUsageValue(
    /* [in] */ Int32 state,
    /* [in] */ Int32 index)
{
    Int32 idx = BinarySearch(*mSysMemUsageTable, mSysMemUsageTableSize, state);
    return idx >= 0 ? GetInt64((*mSysMemUsageTable)[idx], index) : 0;
}

void ProcessStats::DumpSysMemUsageCategory(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ const String& label,
    /* [in] */ Int32 bucket,
    /* [in] */ Int32 index)
{
    pw->Print(prefix);
    pw->Print(label);
    pw->Print(String(": "));
    PrintSizeValue(pw, GetSysMemUsageValue(bucket, index) * 1024);
    pw->Print(String(" min, "));
    PrintSizeValue(pw, GetSysMemUsageValue(bucket, index + 1) * 1024);
    pw->Print(String(" avg, "));
    PrintSizeValue(pw, GetSysMemUsageValue(bucket, index+2) * 1024);
    pw->Println(String(" max"));
}

void ProcessStats::DumpSysMemUsage(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ const ArrayOf<Int32>& screenStates,
    /* [in] */ const ArrayOf<Int32>& memStates)
{
    Int32 printedScreen = -1;
    for (Int32 is = 0; is < screenStates.GetLength(); is++) {
        Int32 printedMem = -1;
        for (Int32 im = 0; im < memStates.GetLength(); im++) {
            Int32 iscreen = screenStates[is];
            Int32 imem = memStates[im];
            Int32 bucket = ((iscreen + imem) * STATE_COUNT);
            Int64 count = GetSysMemUsageValue(bucket, SYS_MEM_USAGE_SAMPLE_COUNT);
            if (count > 0) {
                pw->Print(prefix);
                if (screenStates.GetLength() > 1) {
                    PrintScreenLabel(pw, printedScreen != iscreen
                            ? iscreen : STATE_NOTHING);
                    printedScreen = iscreen;
                }
                if (memStates.GetLength() > 1) {
                    PrintMemLabel(pw, printedMem != imem ? imem : STATE_NOTHING, '\0');
                    printedMem = imem;
                }
                pw->Print(String(": "));
                pw->Print(count);
                pw->Println(String(" samples:"));
                DumpSysMemUsageCategory(pw, prefix, String("  Cached"), bucket,
                        SYS_MEM_USAGE_CACHED_MINIMUM);
                DumpSysMemUsageCategory(pw, prefix, String("  Free"), bucket,
                        SYS_MEM_USAGE_FREE_MINIMUM);
                DumpSysMemUsageCategory(pw, prefix, String("  ZRam"), bucket,
                        SYS_MEM_USAGE_ZRAM_MINIMUM);
                DumpSysMemUsageCategory(pw, prefix, String("  Kernel"), bucket,
                        SYS_MEM_USAGE_KERNEL_MINIMUM);
                DumpSysMemUsageCategory(pw, prefix, String("  Native"), bucket,
                        SYS_MEM_USAGE_NATIVE_MINIMUM);
            }
        }
    }
}

void ProcessStats::DumpStateHeadersCsv(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& sep,
    /* [in] */ ArrayOf<Int32>* screenStates,
    /* [in] */ ArrayOf<Int32>* memStates,
    /* [in] */ ArrayOf<Int32>* procStates)
{
    const Int32 NS = screenStates != NULL ? screenStates->GetLength() : 1;
    const Int32 NM = memStates != NULL ? memStates->GetLength() : 1;
    const Int32 NP = procStates != NULL ? procStates->GetLength() : 1;
    for (Int32 is = 0; is < NS; is++) {
        for (Int32 im = 0; im < NM; im++) {
            for (Int32 ip = 0; ip < NP; ip++) {
                pw->Print(sep);
                Boolean printed = FALSE;
                if (screenStates != NULL && screenStates->GetLength() > 1) {
                    PrintScreenLabelCsv(pw, (*screenStates)[is]);
                    printed = TRUE;
                }
                if (memStates != NULL && memStates->GetLength() > 1) {
                    if (printed) {
                        pw->Print(String("-"));
                    }
                    PrintMemLabelCsv(pw, (*memStates)[im]);
                    printed = TRUE;
                }
                if (procStates != NULL && procStates->GetLength() > 1) {
                    if (printed) {
                        pw->Print(String("-"));
                    }
                    pw->Print((*STATE_NAMES_CSV)[(*procStates)[ip]]);
                }
            }
        }
    }
}

void ProcessStats::DumpProcessStateCsv(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ProcessState* proc,
    /* [in] */ Boolean sepScreenStates,
    /* [in] */ ArrayOf<Int32>* screenStates,
    /* [in] */ Boolean sepMemStates,
    /* [in] */ ArrayOf<Int32>* memStates,
    /* [in] */ Boolean sepProcStates,
    /* [in] */ ArrayOf<Int32>* procStates,
    /* [in] */ Int64 now)
{
    const Int32 NSS = sepScreenStates ? screenStates->GetLength() : 1;
    const Int32 NMS = sepMemStates ? memStates->GetLength() : 1;
    const Int32 NPS = sepProcStates ? procStates->GetLength() : 1;
    for (Int32 iss = 0; iss < NSS; iss++) {
        for (Int32 ims = 0; ims < NMS; ims++) {
            for (Int32 ips = 0; ips < NPS; ips++) {
                const Int32 vsscreen = sepScreenStates ? (*screenStates)[iss] : 0;
                const Int32 vsmem = sepMemStates ? (*memStates)[ims] : 0;
                const Int32 vsproc = sepProcStates ? (*procStates)[ips] : 0;
                const Int32 NSA = sepScreenStates ? 1 : screenStates->GetLength();
                const Int32 NMA = sepMemStates ? 1 : memStates->GetLength();
                const Int32 NPA = sepProcStates ? 1 : procStates->GetLength();
                Int64 totalTime = 0;
                for (Int32 isa = 0; isa < NSA; isa++) {
                    for (Int32 ima = 0; ima < NMA; ima++) {
                        for (Int32 ipa = 0; ipa < NPA; ipa++) {
                            const Int32 vascreen = sepScreenStates ? 0 : (*screenStates)[isa];
                            const Int32 vamem = sepMemStates ? 0 : (*memStates)[ima];
                            const Int32 vaproc = sepProcStates ? 0 : (*procStates)[ipa];
                            const Int32 bucket = ((vsscreen + vascreen + vsmem + vamem)
                                    * STATE_COUNT) + vsproc + vaproc;
                            totalTime += proc->GetDuration(bucket, now);
                        }
                    }
                }
                pw->Print(CSV_SEP);
                pw->Print(totalTime);
            }
        }
    }
}

void ProcessStats::DumpProcessList(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ IArrayList* procs,
    /* [in] */ const ArrayOf<Int32>& screenStates,
    /* [in] */ const ArrayOf<Int32>& memStates,
    /* [in] */ const ArrayOf<Int32>& procStates,
    /* [in] */ Int64 now)
{
    String innerPrefix = prefix + "  ";
    Int32 size;
    procs->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> item;
        procs->Get(i, (IInterface**)&item);
        ProcessState* proc = (ProcessState*)IProcessState::Probe(item);
        pw->Print(prefix);
        pw->Print(proc->mName);
        pw->Print(String(" / "));
        UserHandle::FormatUid(pw, proc->mUid);
        pw->Print(String(" ("));
        pw->Print(proc->mDurationsTableSize);
        pw->Print(String(" entries)"));
        pw->Println(String(":"));
        DumpProcessState(pw, innerPrefix, proc, screenStates, memStates, procStates, now);
        if (proc->mPssTableSize > 0) {
            DumpProcessPss(pw, innerPrefix, proc, screenStates, memStates, procStates);
        }
    }
}

void ProcessStats::DumpProcessSummaryDetails(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IProcessState* proc,
    /* [in] */ const String& prefix,
    /* [in] */ const String& label,
    /* [in] */ ArrayOf<Int32>* screenStates,
    /* [in] */ ArrayOf<Int32>* memStates,
    /* [in] */ ArrayOf<Int32>* procStates,
    /* [in] */ Int64 now,
    /* [in] */ Int64 totalTime,
    /* [in] */ Boolean full)
{
    AutoPtr<CProcessDataCollection> totals;
    CProcessDataCollection::NewByFriend(screenStates, memStates, procStates,
        (CProcessDataCollection**)&totals);
    ComputeProcessData(proc, totals, now);
    if (totals->mTotalTime != 0 || totals->mNumPss != 0) {
        if (prefix != NULL) {
            pw->Print(prefix);
        }
        if (label != NULL) {
            pw->Print(label);
        }
        totals->Print(pw, totalTime, full);
        if (prefix != NULL) {
            pw->Println();
        }
    }
}

void ProcessStats::DumpProcessSummaryLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ IArrayList* procs,
    /* [in] */ ArrayOf<Int32>* screenStates,
    /* [in] */ ArrayOf<Int32>* memStates,
    /* [in] */ ArrayOf<Int32>* procStates,
    /* [in] */ Boolean inclUidVers,
    /* [in] */ Int64 now,
    /* [in] */ Int64 totalTime)
{
    Int32 size;
    procs->GetSize(&size);
    for (Int32 i = size - 1; i >= 0; i--) {
        AutoPtr<IInterface> item;
        procs->Get(i, (IInterface**)&item);
        ProcessState* proc = (ProcessState*)IProcessState::Probe(item);
        pw->Print(prefix);
        pw->Print(String("* "));
        pw->Print(proc->mName);
        pw->Print(String(" / "));
        UserHandle::FormatUid(pw, proc->mUid);
        pw->Print(String(" / v"));
        pw->Print(proc->mVersion);
        pw->Println(String(":"));
        DumpProcessSummaryDetails(pw, proc, prefix, String("         TOTAL: "),
            screenStates, memStates, procStates, now, totalTime, TRUE);
        AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(1);
        (*array)[0] = STATE_PERSISTENT;
        DumpProcessSummaryDetails(pw, proc, prefix, String("    Persistent: "),
            screenStates, memStates, array, now, totalTime, TRUE);
        (*array)[0] = STATE_TOP;
        DumpProcessSummaryDetails(pw, proc, prefix, String("           Top: "),
            screenStates, memStates, array, now, totalTime, TRUE);
        (*array)[0] = STATE_IMPORTANT_FOREGROUND;
        DumpProcessSummaryDetails(pw, proc, prefix, String("        Imp Fg: "),
            screenStates, memStates, array, now, totalTime, TRUE);
        (*array)[0] = STATE_IMPORTANT_BACKGROUND;
        DumpProcessSummaryDetails(pw, proc, prefix, String("        Imp Bg: "),
            screenStates, memStates, array, now, totalTime, TRUE);
        (*array)[0] = STATE_BACKUP;
        DumpProcessSummaryDetails(pw, proc, prefix, String("        Backup: "),
            screenStates, memStates, array, now, totalTime, TRUE);
        (*array)[0] = STATE_HEAVY_WEIGHT;
        DumpProcessSummaryDetails(pw, proc, prefix, String("     Heavy Wgt: "),
            screenStates, memStates, array, now, totalTime, TRUE);
        (*array)[0] = STATE_SERVICE;
        DumpProcessSummaryDetails(pw, proc, prefix, String("       Service: "),
            screenStates, memStates, array, now, totalTime, TRUE);
        (*array)[0] = STATE_SERVICE_RESTARTING;
        DumpProcessSummaryDetails(pw, proc, prefix, String("    Service Rs: "),
            screenStates, memStates, array, now, totalTime, TRUE);
        (*array)[0] = STATE_RECEIVER;
        DumpProcessSummaryDetails(pw, proc, prefix, String("      Receiver: "),
            screenStates, memStates, array, now, totalTime, TRUE);
        (*array)[0] = STATE_HOME;
        DumpProcessSummaryDetails(pw, proc, prefix, String("        (Home): "),
            screenStates, memStates, array, now, totalTime, TRUE);
        (*array)[0] = STATE_LAST_ACTIVITY;
        DumpProcessSummaryDetails(pw, proc, prefix, String("    (Last Act): "),
            screenStates, memStates, array, now, totalTime, TRUE);
        array = ArrayOf<Int32>::Alloc(3);
        (*array)[0] = STATE_CACHED_ACTIVITY;
        (*array)[1] = STATE_CACHED_ACTIVITY_CLIENT;
        (*array)[2] = STATE_CACHED_EMPTY;
        DumpProcessSummaryDetails(pw, proc, prefix, String("      (Cached): "),
            screenStates, memStates, array, now, totalTime, TRUE);
    }
}

void ProcessStats::PrintPercent(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Double fraction)
{
    fraction *= 100;
    String str;
    if (fraction < 1) {
        str.AppendFormat("%.2f", fraction);
        pw->Print(str);
    }
    else if (fraction < 10) {
        str.AppendFormat("%.1f", fraction);
        pw->Print(str);
    }
    else {
        str.AppendFormat("%.0f", fraction);
        pw->Print(str);
    }
    pw->Print(String("%"));
}

void ProcessStats::PrintSizeValue(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int64 number)
{
    Float result = number;
    String suffix("");
    if (result > 900) {
        suffix = "KB";
        result = result / 1024;
    }
    if (result > 900) {
        suffix = "MB";
        result = result / 1024;
    }
    if (result > 900) {
        suffix = "GB";
        result = result / 1024;
    }
    if (result > 900) {
        suffix = "TB";
        result = result / 1024;
    }
    if (result > 900) {
        suffix = "PB";
        result = result / 1024;
    }
    String value;
    if (result < 1) {
        value.AppendFormat("%.2f", result);
    }
    else if (result < 10) {
        value.AppendFormat("%.1f", result);
    }
    else if (result < 100) {
        value.AppendFormat("%.0f", result);
    }
    else {
        value.AppendFormat("%.0f", result);
    }
    pw->Print(value);
    pw->Print(suffix);
}

ECode ProcessStats::DumpProcessListCsv(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ IArrayList* procs,
    /* [in] */ Boolean sepScreenStates,
    /* [in] */ ArrayOf<Int32>* screenStates,
    /* [in] */ Boolean sepMemStates,
    /* [in] */ ArrayOf<Int32>* memStates,
    /* [in] */ Boolean sepProcStates,
    /* [in] */ ArrayOf<Int32>* procStates,
    /* [in] */ Int64 now)
{
    pw->Print(String("process"));
    pw->Print(CSV_SEP);
    pw->Print(String("uid"));
    pw->Print(CSV_SEP);
    pw->Print(String("vers"));
    DumpStateHeadersCsv(pw, CSV_SEP, sepScreenStates ? screenStates : NULL,
            sepMemStates ? memStates : NULL, sepProcStates ? procStates : NULL);
    pw->Println();
    Int32 size;
    procs->GetSize(&size);
    for (Int32 i = size-1; i >= 0; i--) {
        AutoPtr<IInterface> item;
        procs->Get(i, (IInterface**)&item);
        ProcessState* proc = (ProcessState*)IProcessState::Probe(item);
        pw->Print(proc->mName);
        pw->Print(CSV_SEP);
        UserHandle::FormatUid(pw, proc->mUid);
        pw->Print(CSV_SEP);
        pw->Print(proc->mVersion);
        DumpProcessStateCsv(pw, proc, sepScreenStates, screenStates,
                sepMemStates, memStates, sepProcStates, procStates, now);
        pw->Println();
    }
    return NOERROR;
}

Int32 ProcessStats::PrintArrayEntry(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const ArrayOf<String>& array,
    /* [in] */ Int32 value,
    /* [in] */ Int32 mod)
{
    Int32 index = value/mod;
    if (index >= 0 && index < array.GetLength()) {
        pw->Print(array[index]);
    }
    else {
        pw->PrintChar('?');
    }
    return value - index*mod;
}

void ProcessStats::PrintProcStateTag(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 state)
{
    state = PrintArrayEntry(pw, *ADJ_SCREEN_TAGS,  state, ADJ_SCREEN_MOD*STATE_COUNT);
    state = PrintArrayEntry(pw, *ADJ_MEM_TAGS,  state, STATE_COUNT);
    PrintArrayEntry(pw, *STATE_TAGS,  state, 1);
}

void ProcessStats::PrintAdjTag(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 state)
{
    state = PrintArrayEntry(pw, *ADJ_SCREEN_TAGS,  state, ADJ_SCREEN_MOD);
    PrintArrayEntry(pw, *ADJ_MEM_TAGS, state, 1);
}

void ProcessStats::PrintProcStateTagAndValue(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 state,
    /* [in] */ Int64 value)
{
    pw->PrintChar(',');
    PrintProcStateTag(pw, state);
    pw->PrintChar(':');
    pw->Print(value);
}

void ProcessStats::PrintAdjTagAndValue(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 state,
    /* [in] */ Int64 value)
{
    pw->PrintChar(',');
    PrintAdjTag(pw, state);
    pw->PrintChar(':');
    pw->Print(value);
}

void ProcessStats::DumpAllProcessStateCheckin(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ProcessState* proc,
    /* [in] */ Int64 now)
{
    Boolean didCurState = FALSE;
    for (Int32 i = 0; i < proc->mDurationsTableSize; i++) {
        Int32 off = (*proc->mDurationsTable)[i];
        Int32 type = (off >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;
        Int64 time = proc->mStats->GetInt64(off, 0);
        if (proc->mCurState == type) {
            didCurState = TRUE;
            time += now - proc->mStartTime;
        }
        PrintProcStateTagAndValue(pw, type, time);
    }
    if (!didCurState && proc->mCurState != STATE_NOTHING) {
        PrintProcStateTagAndValue(pw, proc->mCurState, now - proc->mStartTime);
    }
}

void ProcessStats::DumpAllProcessPssCheckin(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ProcessState* proc)
{
    for (Int32 i = 0; i < proc->mPssTableSize; i++) {
        Int32 off = (*proc->mPssTable)[i];
        Int32 type = (off >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;
        Int64 count = proc->mStats->GetInt64(off, PSS_SAMPLE_COUNT);
        Int64 min = proc->mStats->GetInt64(off, PSS_MINIMUM);
        Int64 avg = proc->mStats->GetInt64(off, PSS_AVERAGE);
        Int64 max = proc->mStats->GetInt64(off, PSS_MAXIMUM);
        Int64 umin = proc->mStats->GetInt64(off, PSS_USS_MINIMUM);
        Int64 uavg = proc->mStats->GetInt64(off, PSS_USS_AVERAGE);
        Int64 umax = proc->mStats->GetInt64(off, PSS_USS_MAXIMUM);
        pw->PrintChar(',');
        PrintProcStateTag(pw, type);
        pw->PrintChar(':');
        pw->Print(count);
        pw->PrintChar(':');
        pw->Print(min);
        pw->PrintChar(':');
        pw->Print(avg);
        pw->PrintChar(':');
        pw->Print(max);
        pw->PrintChar(':');
        pw->Print(umin);
        pw->PrintChar(':');
        pw->Print(uavg);
        pw->PrintChar(':');
        pw->Print(umax);
    }
}

ECode ProcessStats::Reset()
{
    if (DEBUG) Slogger::D(TAG, "Resetting state of %s", mTimePeriodStartClockStr.string());
    ResetCommon();
    AutoPtr<IArrayMap> map;
    mPackages->GetMap((IArrayMap**)&map);
    map->Clear();
    map = NULL;
    mProcesses->GetMap((IArrayMap**)&map);
    map->Clear();
    mMemFactor = STATE_NOTHING;
    mStartTime = 0;
    if (DEBUG) Slogger::D(TAG, "State Reset; now %s", mTimePeriodStartClockStr.string());
    return NOERROR;
}

ECode ProcessStats::ResetSafely()
{
    if (DEBUG) Slogger::D(TAG, "Safely Resetting state of %s", mTimePeriodStartClockStr.string());
    ResetCommon();

    // First initialize use count of all common processes.
    const Int64 now = SystemClock::GetUptimeMillis();
    AutoPtr<IArrayMap> procMap;
    mProcesses->GetMap((IArrayMap**)&procMap);
    Int32 size;
    procMap->GetSize(&size);
    for (Int32 ip = size - 1; ip >= 0; ip--) {
        AutoPtr<IInterface> value;
        procMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 uSize;
        uids->GetSize(&uSize);
        for (Int32 iu = uSize - 1; iu >= 0; iu--) {
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            ProcessState* proc = (ProcessState*)IProcessState::Probe(uValue);
            proc->mTmpNumInUse = 0;
       }
    }

    // Next Reset or prune all per-package processes, and for the ones that are Reset
    // track this back to the common processes.
    AutoPtr<IArrayMap> pkgMap;
    mPackages->GetMap((IArrayMap**)&pkgMap);
    pkgMap->GetSize(&size);
    for (Int32 ip = size - 1; ip >= 0; ip--) {
        AutoPtr<IInterface> value;
        pkgMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 uSize;
        uids->GetSize(&uSize);
        for (Int32 iu = uSize - 1; iu >= 0; iu--) {
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            AutoPtr<ISparseArray> vpkgs = ISparseArray::Probe(uValue);
            Int32 vSize;
            vpkgs->GetSize(&vSize);
            for (Int32 iv = vSize - 1; iv >= 0; iv--) {
                AutoPtr<IInterface> vValue;
                vpkgs->ValueAt(iv, (IInterface**)&vValue);
                PackageState* pkgState = (PackageState*)IPackageState::Probe(vValue);
                Int32 NPROCS;
                pkgState->mProcesses->GetSize(&NPROCS);
                Int32 NSRVS;
                pkgState->mServices->GetSize(&NSRVS);
                for (Int32 iproc = NPROCS - 1; iproc >= 0; iproc--) {
                    AutoPtr<IInterface> procValue;
                    pkgState->mProcesses->GetValueAt(iproc, (IInterface**)&procValue);
                    ProcessState* ps = (ProcessState*)IProcessState::Probe(procValue);
                    if (ps->IsInUse()) {
                        ps->ResetSafely(now);
                        ps->mCommonProcess->mTmpNumInUse++;
                        ps->mCommonProcess->mTmpFoundSubProc = ps;
                    }
                    else {
                        ps->MakeDead();
                        pkgState->mProcesses->RemoveAt(iproc);
                    }
                }

                for (Int32 isvc = NSRVS - 1; isvc >= 0; isvc--) {
                    AutoPtr<IInterface> svcValue;
                    pkgState->mServices->GetValueAt(isvc, (IInterface**)&svcValue);
                    ServiceState* ss = (ServiceState*)IServiceState::Probe(svcValue);
                    if (ss->IsInUse()) {
                        ss->ResetSafely(now);
                    }
                    else {
                        pkgState->mServices->RemoveAt(isvc);
                    }
                }
                pkgState->mProcesses->GetSize(&NPROCS);
                pkgState->mServices->GetSize(&NSRVS);
                if (NPROCS <= 0 && NSRVS <= 0) {
                    vpkgs->RemoveAt(iv);
                }
            }
            vpkgs->GetSize(&vSize);
            if (vSize <= 0) {
                uids->RemoveAt(iu);
            }
        }
        uids->GetSize(&uSize);
        if (uSize <= 0) {
            pkgMap->RemoveAt(ip);
        }
    }

    // Finally prune out any common processes that are no longer in use.
    procMap = NULL;
    mProcesses->GetMap((IArrayMap**)&procMap);
    procMap->GetSize(&size);
    for (Int32 ip = size - 1; ip >= 0; ip--) {
        AutoPtr<IInterface> value;
        procMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 uSize;
        uids->GetSize(&uSize);
        for (Int32 iu = uSize - 1; iu >= 0; iu--) {
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            AutoPtr<ProcessState> ps = (ProcessState*)IProcessState::Probe(uValue);
            if (ps->IsInUse() || ps->mTmpNumInUse > 0) {
                // If this is a process for multiple packages, we could at this point
                // be back down to one package.  In that case, we want to revert back
                // to a single shared ProcessState.  We can do this by converting the
                // current package-specific ProcessState up to the shared ProcessState,
                // throwing away the current one we have here (because nobody else is
                // using it).
                if (!ps->mActive && ps->mMultiPackage && ps->mTmpNumInUse == 1) {
                    // Here we go...
                    ps = ps->mTmpFoundSubProc;
                    ps->mCommonProcess = ps;
                    uids->SetValueAt(iu, (IProcessState*)ps);
                }
                else {
                    ps->ResetSafely(now);
                }
            }
            else {
                ps->MakeDead();
                uids->RemoveAt(iu);
            }
        }
        uids->GetSize(&uSize);
        if (uSize <= 0) {
            procMap->RemoveAt(ip);
        }
    }

    mStartTime = now;
    if (DEBUG) Slogger::D(TAG, "State Reset; now %s", mTimePeriodStartClockStr.string());
    return NOERROR;
}

void ProcessStats::ResetCommon()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mTimePeriodStartClock);
    BuildTimePeriodStartClockStr();
    mTimePeriodStartRealtime = mTimePeriodEndRealtime = SystemClock::GetElapsedRealtime();
    mTimePeriodStartUptime = mTimePeriodEndUptime = SystemClock::GetUptimeMillis();
    mInt64s.Clear();
    AutoPtr<ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(LONGS_SIZE);
    mInt64s.PushBack(array);
    mNextInt64 = 0;
    Arrays::Fill(mMemFactorDurations, 0LL);
    mSysMemUsageTable = NULL;
    mSysMemUsageTableSize = 0;
    mStartTime = 0;
    mReadError = NULL;
    mFlags = 0;
    Boolean res;
    EvaluateSystemProperties(TRUE, &res);
}

ECode ProcessStats::EvaluateSystemProperties(
    /* [in] */ Boolean update,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean changed = FALSE;
    String runtime;
    SystemProperties::Get(String("persist.sys.dalvik.vm.lib.2"),
        String(NULL)/*VMRuntime.getRuntime().vmLibrary()*/, &runtime);
    if (!runtime.Equals(mRuntime)) {
        changed = TRUE;
        if (update) {
            mRuntime = runtime;
        }
    }
    *result = changed;
    return NOERROR;
}

void ProcessStats::BuildTimePeriodStartClockStr()
{
    AutoPtr<ICharSequence> formatStr = CoreUtils::Convert(String("yyyy-MM-dd-HH-mm-ss"));
    AutoPtr<ICharSequence> csq = DateFormat::Format(formatStr, mTimePeriodStartClock);
    mTimePeriodStartClockStr = Object::ToString(csq);
    Slogger::I(TAG, " TimePeriodStartClockStr: [%s]", mTimePeriodStartClockStr.string());
}

AutoPtr<ArrayOf<Int32> > ProcessStats::ReadTableFromParcel(
    /* [in] */ IParcel* in,
    /* [in] */ const String& name,
    /* [in] */ const String& what)
{
    Int32 size;
    in->ReadInt32(&size);
    if (size < 0) {
        Slogger::W(TAG, "Ignoring existing stats; bad %s table size: %d", what.string(), size);
        return BAD_TABLE;
    }
    if (size == 0) {
        return NULL;
    }
    AutoPtr<ArrayOf<Int32> > table = ArrayOf<Int32>::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        in->ReadInt32(&(*table)[i]);
        if (DEBUG_PARCEL) Slogger::I(TAG, "Reading in %s table #%d: %s",
                name.string(), i, PrintInt64Offset((*table)[i]).string());
        if (!ValidateInt64Offset((*table)[i])) {
            Slogger::W(TAG, "Ignoring existing stats; bad %s table entry: ",
                what.string(), PrintInt64Offset((*table)[i]).string());
            return NULL;
        }
    }
    return table;
}

void ProcessStats::WriteCompactedInt64Array(
    /* [in] */ IParcel* out,
    /* [in] */ const ArrayOf<Int64>& array,
    /* [in] */ Int32 num)
{
    for (Int32 i = 0; i < num; i++) {
        Int64 val = array[i];
        if (val < 0) {
            Slogger::W(TAG, "Time val negative: %lld", val);
            val = 0;
        }
        if (val <= Elastos::Core::Math::INT32_MAX_VALUE) {
            out->WriteInt32((Int32)val);
        }
        else {
            Int32 top = ~((Int32)((val>>32)&0x7fffffff));
            Int32 bottom = (Int32)(val&0xfffffff);
            out->WriteInt32(top);
            out->WriteInt32(bottom);
        }
    }
}

ECode ProcessStats::ReadCompactedInt64Array(
    /* [in] */ IParcel* in,
    /* [in] */ Int32 version,
    /* [in] */ ArrayOf<Int64>* array,
    /* [in] */ Int32 num)
{
    if (version <= 10) {
        Int32 N;
        in->ReadInt32(&N);
        if (N == array->GetLength()) {
            for (Int32 i = 0; i < N; i++) {
                in->ReadInt64(&(*array)[i]);
            }
        }
        else {
            Slogger::E(TAG, "bad array lengths");
            return E_RUNTIME_EXCEPTION;
        }
        return NOERROR;
    }

    Int32 alen = array->GetLength();
    if (num > alen) {
        Slogger::E(TAG, "bad array lengths: got %d array is %d", num, alen);
        return E_RUNTIME_EXCEPTION;
    }
    Int32 i;
    for (i = 0; i < num; i++) {
        Int32 val;
        in->ReadInt32(&val);
        if (val >= 0) {
            (*array)[i] = val;
        }
        else {
            Int32 bottom;
            in->ReadInt32(&bottom);
            (*array)[i] = (((Int64)~val)<<32) | bottom;
        }
    }
    while (i < alen) {
        (*array)[i] = 0;
        i++;
    }
    return NOERROR;
}

void ProcessStats::WriteCommonString(
    /* [in] */ IParcel* out,
    /* [in] */ const String& name)
{
    AutoPtr<IInteger32> index = (*mCommonStringToIndex)[name];
    Int32 value;
    if (index != NULL) {
        index->GetValue(&value);
        out->WriteInt32(value);
        return;
    }
    index = CoreUtils::Convert((Int32)mCommonStringToIndex->GetSize());
    (*mCommonStringToIndex)[name] = index;
    index->GetValue(&value);
    out->WriteInt32(~value);
    out->WriteString(name);
}

String ProcessStats::ReadCommonString(
    /* [in] */ IParcel* in,
    /* [in] */ Int32 version)
{
    String name;
    if (version <= 9) {
        in->ReadString(&name);
        return name;
    }
    Int32 index;
    in->ReadInt32(&index);
    if (index >= 0) {
        return (*mIndexToCommonString)[index];
    }
    index = ~index;
    in->ReadString(&name);
    while ((Int32)mIndexToCommonString->GetSize() <= index) {
        mIndexToCommonString->PushBack(String(NULL));
    }
    (*mIndexToCommonString)[index] = name;
    return name;
}

ECode ProcessStats::WriteToParcel(
    /* [in] */ IParcel* out)
{
    return WriteToParcel(out, SystemClock::GetUptimeMillis());
}

ECode ProcessStats::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 now)
{
    out->WriteInt32(MAGIC);
    out->WriteInt32(PARCEL_VERSION);
    out->WriteInt32(STATE_COUNT);
    out->WriteInt32(ADJ_COUNT);
    out->WriteInt32(PSS_COUNT);
    out->WriteInt32(SYS_MEM_USAGE_COUNT);
    out->WriteInt32(LONGS_SIZE);

    mCommonStringToIndex = new HashMap<String, AutoPtr<IInteger32> >;

    // First commit all running times.
    AutoPtr<IArrayMap> procMap;
    mProcesses->GetMap((IArrayMap**)&procMap);
    Int32 NPROC;
    procMap->GetSize(&NPROC);
    for (Int32 ip = 0; ip < NPROC; ip--) {
        AutoPtr<IInterface> value;
        procMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 NUID;
        uids->GetSize(&NUID);
        for (Int32 iu = 0; iu < NUID; iu--) {
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            ProcessState* proc = (ProcessState*)IProcessState::Probe(uValue);
            proc->CommitStateTime(now);
       }
    }

    AutoPtr<IArrayMap> pkgMap;
    mPackages->GetMap((IArrayMap**)&pkgMap);
    Int32 NPKG;
    pkgMap->GetSize(&NPKG);
    for (Int32 ip = 0; ip < NPKG; ip++) {
        AutoPtr<IInterface> value;
        pkgMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 NUID;
        uids->GetSize(&NUID);
        for (Int32 iu = 0; iu < NUID; iu++) {
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            AutoPtr<ISparseArray> vpkgs = ISparseArray::Probe(uValue);
            Int32 NVERS;
            vpkgs->GetSize(&NVERS);
            for (Int32 iv = 0; iv < NVERS; iv++) {
                AutoPtr<IInterface> vValue;
                vpkgs->ValueAt(iv, (IInterface**)&vValue);
                PackageState* pkgState = (PackageState*)IPackageState::Probe(vValue);
                Int32 NPROCS;
                pkgState->mProcesses->GetSize(&NPROCS);
                for (Int32 iproc = 0; iproc < NPROCS; iproc++) {
                    AutoPtr<IInterface> procValue;
                    pkgState->mProcesses->GetValueAt(iproc, (IInterface**)&procValue);
                    ProcessState* proc = (ProcessState*)IProcessState::Probe(procValue);
                    if (proc->mCommonProcess != proc) {
                        proc->CommitStateTime(now);
                    }
                }

                Int32 NSRVS;
                pkgState->mServices->GetSize(&NSRVS);
                for (Int32 isvc = 0; isvc < NSRVS; isvc++) {
                    AutoPtr<IInterface> svcValue;
                    pkgState->mServices->GetValueAt(isvc, (IInterface**)&svcValue);
                    ServiceState* svc = (ServiceState*)IServiceState::Probe(svcValue);
                    svc->CommitStateTime(now);
                }
            }
        }
    }

    out->WriteInt64(mTimePeriodStartClock);
    out->WriteInt64(mTimePeriodStartRealtime);
    out->WriteInt64(mTimePeriodEndRealtime);
    out->WriteInt64(mTimePeriodStartUptime);
    out->WriteInt64(mTimePeriodEndUptime);
    out->WriteString(mRuntime);
    out->WriteInt32(mFlags);

    out->WriteInt32(mInt64s.GetSize());
    out->WriteInt32(mNextInt64);
    Int32 size = mInt64s.GetSize();
    for (Int32 i = 0; i < size -1; i++) {
        AutoPtr<ArrayOf<Int64> > array = mInt64s[i];
        WriteCompactedInt64Array(out, *array, array->GetLength());
    }
    AutoPtr<ArrayOf<Int64> > lastInt64s = mInt64s[mInt64s.GetSize() - 1];
    WriteCompactedInt64Array(out, *lastInt64s, mNextInt64);

    if (mMemFactor != STATE_NOTHING) {
        (*mMemFactorDurations)[mMemFactor] += now - mStartTime;
        mStartTime = now;
    }
    WriteCompactedInt64Array(out, *mMemFactorDurations, mMemFactorDurations->GetLength());

    out->WriteInt32(mSysMemUsageTableSize);
    for (Int32 i = 0; i < mSysMemUsageTableSize; i++) {
        if (DEBUG_PARCEL) Slogger::I(TAG, "Writing sys mem usage #%d: %s",
            i, PrintInt64Offset((*mSysMemUsageTable)[i]).string());
        out->WriteInt32((*mSysMemUsageTable)[i]);
    }

    out->WriteInt32(NPROC);
    for (Int32 ip = 0; ip < NPROC; ip++) {
        AutoPtr<IInterface> key;
        procMap->GetKeyAt(ip, (IInterface**)&key);
        String pkgName;
        ICharSequence::Probe(key)->ToString(&pkgName);
        WriteCommonString(out, pkgName);
        AutoPtr<IInterface> value;
        procMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 NUID;
        uids->GetSize(&NUID);
        for (Int32 iu = 0; iu < NUID; iu--) {
            Int32 uid;
            uids->KeyAt(iu, &uid);
            out->WriteInt32(uid);
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            ProcessState* proc = (ProcessState*)IProcessState::Probe(uValue);
            WriteCommonString(out, proc->mPackage);
            out->WriteInt32(proc->mVersion);
            proc->WriteToParcel(out, now);
        }
    }
    out->WriteInt32(NPKG);
    for (Int32 ip = 0; ip < NPKG; ip++) {
        AutoPtr<IInterface> key;
        pkgMap->GetKeyAt(ip, (IInterface**)&key);
        String pkgName;
        ICharSequence::Probe(key)->ToString(&pkgName);
        WriteCommonString(out, pkgName);
        AutoPtr<IInterface> value;
        pkgMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 NUID;
        uids->GetSize(&NUID);
        out->WriteInt32(NUID);
        for (Int32 iu = 0; iu < NUID; iu++) {
            Int32 uid;
            uids->KeyAt(iu, &uid);
            out->WriteInt32(uid);
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            AutoPtr<ISparseArray> vpkgs = ISparseArray::Probe(uValue);
            Int32 NVERS;
            vpkgs->GetSize(&NVERS);
            for (Int32 iv = 0; iv < NVERS; iv++) {
                Int32 vers;
                vpkgs->KeyAt(iv, &vers);
                out->WriteInt32(vers);
                AutoPtr<IInterface> vValue;
                vpkgs->ValueAt(iv, (IInterface**)&vValue);
                PackageState* pkgState = (PackageState*)IPackageState::Probe(vValue);
                Int32 NPROCS;
                pkgState->mProcesses->GetSize(&NPROCS);
                out->WriteInt32(NPROCS);
                for (Int32 iproc = 0; iproc < NPROCS; iproc++) {
                    AutoPtr<IInterface> procKey;
                    pkgState->mProcesses->GetKeyAt(iproc, (IInterface**)&procKey);
                    String procName;
                    ICharSequence::Probe(procKey)->ToString(&procName);
                    WriteCommonString(out, procName);

                    AutoPtr<IInterface> procValue;
                    pkgState->mProcesses->GetValueAt(iproc, (IInterface**)&procValue);
                    ProcessState* proc = (ProcessState*)IProcessState::Probe(procValue);
                    if (proc->mCommonProcess == proc) {
                        // This is the same as the common process we wrote above.
                        out->WriteInt32(0);
                    }
                    else {
                        // There is separate data for this package's process.
                        out->WriteInt32(1);
                        proc->WriteToParcel(out, now);
                    }
                }
                Int32 NSRVS;
                pkgState->mServices->GetSize(&NSRVS);
                out->WriteInt32(NSRVS);
                for (Int32 isvc = 0; isvc < NSRVS; isvc++) {
                    AutoPtr<IInterface> svcKey;
                    pkgState->mServices->GetKeyAt(isvc, (IInterface**)&svcKey);
                    String svcName;
                    ICharSequence::Probe(svcKey)->ToString(&svcName);
                    out->WriteString(svcName);
                    AutoPtr<IInterface> svcValue;
                    pkgState->mServices->GetValueAt(isvc, (IInterface**)&svcValue);
                    ServiceState* svc = (ServiceState*)IServiceState::Probe(svcValue);
                    WriteCommonString(out, svc->mProcessName);
                    svc->WriteToParcel(out, now);
                }
            }
        }
    }

    mCommonStringToIndex = NULL;
    return NOERROR;
}

Boolean ProcessStats::ReadCheckedInt32(
    /* [in] */ IParcel* in,
    /* [in] */ Int32 val,
    /* [in] */ const String& what)
{
    Int32 got;
    if ((in->ReadInt32(&got), got) != val) {
        mReadError = "";
        mReadError.AppendFormat("bad %s: %d", what.string(), got);
        return FALSE;
    }
    return TRUE;
}

ECode ProcessStats::ReadFully(
    /* [in] */ IInputStream* stream,
    /* [in] */ ArrayOf<Int32>* outLen,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    Int32 pos = 0;
    Int32 initialAvail;
    stream->Available(&initialAvail);
    AutoPtr<ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(initialAvail > 0 ? (initialAvail+1) : 16384);
    while (TRUE) {
        Int32 amt;
        FAIL_RETURN(stream->Read(data, pos, data->GetLength()-pos, &amt));
        if (DEBUG_PARCEL) Slogger::I("foo", "Read %d bytes at %d of avail %d",\
                amt, pos, data->GetLength());
        if (amt < 0) {
            if (DEBUG_PARCEL) Slogger::I("foo", "**** FINISHED READING: pos=%d len=%d",
             pos, data->GetLength());
            (*outLen)[0] = pos;
            *bytes = data;
            REFCOUNT_ADD(*bytes)
            return NOERROR;
        }
        pos += amt;
        if (pos >= data->GetLength()) {
            AutoPtr<ArrayOf<Byte> > newData = ArrayOf<Byte>::Alloc(pos + 16384);
            if (DEBUG_PARCEL) Slogger::I(TAG, "Copying %d bytes to new array len %d",
                    pos, newData->GetLength());
            newData->Copy(data, pos);
            data = newData;
        }
    }
    return NOERROR;
}

ECode ProcessStats::Read(
    /* [in] */ IInputStream* stream)
{
    AutoPtr<ArrayOf<Int32> > len = ArrayOf<Int32>::Alloc(1);
    AutoPtr<ArrayOf<Byte> > raw;
    if (FAILED(ReadFully(stream, len, (ArrayOf<Byte>**)&raw))){
        mReadError = "caught exception: ";
        return NOERROR;
    }
    AutoPtr<IParcel> in;
    CParcel::New((IParcel**)&in);
    in->Unmarshall(raw, 0, (*len)[0]);
    in->SetDataPosition(0);
    ICloseable::Probe(stream)->Close();

    return ReadFromParcel(in);
}

ECode ProcessStats::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    AutoPtr<IArrayMap> pkgMap, procMap;
    mPackages->GetMap((IArrayMap**)&pkgMap);
    mProcesses->GetMap((IArrayMap**)&procMap);
    Int32 pkgSize, procSize;
    pkgMap->GetSize(&pkgSize);
    procMap->GetSize(&procSize);
    Boolean hadData = pkgSize > 0 || procSize > 0;
    if (hadData) {
        ResetSafely();
    }

    if (!ReadCheckedInt32(in, MAGIC, String("magic number"))) {
        return NOERROR;;
    }
    Int32 version;
    in->ReadInt32(&version);
    if (version != PARCEL_VERSION) {
        mReadError = "";
        mReadError.AppendFormat("bad version: %d", version);
        return NOERROR;
    }
    if (!ReadCheckedInt32(in, STATE_COUNT, String("state count"))) {
        return NOERROR;
    }
    if (!ReadCheckedInt32(in, ADJ_COUNT, String("adj count"))) {
        return NOERROR;
    }
    if (!ReadCheckedInt32(in, PSS_COUNT, String("pss count"))) {
        return NOERROR;
    }
    if (!ReadCheckedInt32(in, SYS_MEM_USAGE_COUNT, String("sys mem usage count"))) {
        return NOERROR;
    }
    if (!ReadCheckedInt32(in, LONGS_SIZE, String("longs size"))) {
        return NOERROR;
    }

    mIndexToCommonString = new List<String>;

    in->ReadInt64(&mTimePeriodStartClock);
    BuildTimePeriodStartClockStr();
    in->ReadInt64(&mTimePeriodStartRealtime);
    in->ReadInt64(&mTimePeriodEndRealtime);
    in->ReadInt64(&mTimePeriodStartUptime);
    in->ReadInt64(&mTimePeriodEndUptime);
    in->ReadString(&mRuntime);
    in->ReadInt32(&mFlags);

    Int32 NLONGS;
    in->ReadInt32(&NLONGS);
    Int32 NEXTLONG;
    in->ReadInt32(&NEXTLONG);
    mInt64s.Clear();
    for (Int32 i = 0; i < (NLONGS-1); i++) {
        while (i >= (Int32)mInt64s.GetSize()) {
            AutoPtr<ArrayOf<Int64> > array = ArrayOf<Int64>::Alloc(LONGS_SIZE);
            mInt64s.PushBack(array);
        }
        FAIL_RETURN(ReadCompactedInt64Array(in, version, mInt64s[i], LONGS_SIZE));
    }
    AutoPtr<ArrayOf<Int64> > longs = ArrayOf<Int64>::Alloc(LONGS_SIZE);
    mNextInt64 = NEXTLONG;
    FAIL_RETURN(ReadCompactedInt64Array(in, version, longs, NEXTLONG));
    mInt64s.PushBack(longs);

    FAIL_RETURN(ReadCompactedInt64Array(in, version, mMemFactorDurations, mMemFactorDurations->GetLength()));

    mSysMemUsageTable = ReadTableFromParcel(in, TAG, String("sys mem usage"));
    if (mSysMemUsageTable == BAD_TABLE) {
        return NOERROR;
    }
    mSysMemUsageTableSize = mSysMemUsageTable != NULL ? (*mSysMemUsageTable).GetLength() : 0;

    Int32 NPROC;
    in->ReadInt32(&NPROC);
    if (NPROC < 0) {
        mReadError.AppendFormat("bad process count: %d", NPROC);
        return NOERROR;
    }
    while (NPROC > 0) {
        NPROC--;
        String procName = ReadCommonString(in, version);
        if (procName == NULL) {
            mReadError = "bad process name";
            return NOERROR;
        }
        Int32 NUID;
        in->ReadInt32(&NUID);
        if (NUID < 0) {
            mReadError.AppendFormat("bad uid count: %d", NUID);
            return NOERROR;
        }
        while (NUID > 0) {
            NUID--;
            Int32 uid;
            in->ReadInt32(&uid);
            if (uid < 0) {
                mReadError.AppendFormat("bad uid: %d", uid);
                return NOERROR;
            }
            String pkgName = ReadCommonString(in, version);
            if (pkgName == NULL) {
                mReadError = "bad process package name";
                return NOERROR;
            }
            Int32 vers;
            in->ReadInt32(&vers);
            AutoPtr<ProcessState> proc;
            if (hadData) {
                AutoPtr<IInterface> _proc;
                mProcesses->Get(procName, uid, (IInterface**)&_proc);
                proc = (ProcessState*)IProcessState::Probe(_proc);
            }

            if (proc != NULL) {
                if (!proc->ReadFromParcel(in, FALSE)) {
                    return NOERROR;
                }
            }
            else {
                proc = new ProcessState();
                proc->constructor(this, pkgName, uid, vers, procName);
                if (!proc->ReadFromParcel(in, TRUE)) {
                    return NOERROR;
                }
            }
            if (DEBUG_PARCEL) Slogger::D(TAG, "Adding process: %s %d %s",
                    procName.string(), uid, proc->ToString().string());
            mProcesses->Put(procName, uid, (IProcessState*)proc);
        }
    }

    if (DEBUG_PARCEL) {
        procMap->GetSize(&procSize);
        Slogger::D(TAG, "Read %d processes", procSize);
    }

    Int32 NPKG;
    in->ReadInt32(&NPKG);
    if (NPKG < 0) {
        mReadError.AppendFormat("bad package count: %d", NPKG);
        return NOERROR;
    }
    while (NPKG > 0) {
        NPKG--;
        String pkgName = ReadCommonString(in, version);
        if (pkgName == NULL) {
            mReadError = "bad package name";
            return NOERROR;
        }
        Int32 NUID;
        in->ReadInt32(&NUID);
        if (NUID < 0) {
            mReadError.AppendFormat("bad uid count: %d", NUID);
            return NOERROR;
        }
        while (NUID > 0) {
            NUID--;
            Int32 uid;
            in->ReadInt32(&uid);
            if (uid < 0) {
                mReadError.AppendFormat("bad uid: %d", uid);
                return NOERROR;
            }
            Int32 NVERS;
            in->ReadInt32(&NVERS);
            if (NVERS < 0) {
                mReadError.AppendFormat("bad versions count: %d", NVERS);
                return NOERROR;
            }
            while (NVERS > 0) {
                NVERS--;
                Int32 vers;
                in->ReadInt32(&vers);
                AutoPtr<PackageState> pkgState = new PackageState();
                pkgState->constructor(pkgName, uid);
                AutoPtr<IInterface> item;
                mPackages->Get(pkgName, uid, (IInterface**)&item);
                AutoPtr<ISparseArray> vpkg = ISparseArray::Probe(item);
                if (vpkg == NULL) {
                    CSparseArray::New((ISparseArray**)&vpkg);
                    mPackages->Put(pkgName, uid, vpkg);
                }
                vpkg->Put(vers, (IPackageState*)pkgState);
                Int32 NPROCS;
                in->ReadInt32(&NPROCS);
                if (NPROCS < 0) {
                    mReadError.AppendFormat("bad package process count: %d", NPROCS);
                    return NOERROR;
                }
                while (NPROCS > 0) {
                    NPROCS--;
                    String procName = ReadCommonString(in, version);
                    if (procName == NULL) {
                        mReadError = "bad package process name";
                        return NOERROR;
                    }
                    Int32 hasProc;
                    in->ReadInt32(&hasProc);
                    if (DEBUG_PARCEL) Slogger::D(TAG, "Reading package %s %d process %s hasProc=%d",
                        pkgName.string(), uid, procName.string(), hasProc);

                    AutoPtr<IInterface> _commonProc;
                    mProcesses->Get(procName, uid, (IInterface**)&_commonProc);
                    ProcessState* commonProc = (ProcessState*)IProcessState::Probe(_commonProc);
                    if (DEBUG_PARCEL) Slogger::D(TAG, "Got common proc %s %d: %s",
                        procName.string(), uid, commonProc == NULL ? "NULL"
                            : commonProc->ToString().string());
                    if (commonProc == NULL) {
                        mReadError = String("no common proc: ") + procName;
                        return NOERROR;
                    }
                    if (hasProc != 0) {
                        // The process for this package is unique to the package; we
                        // need to load it.  We don't need to do anything about it if
                        // it is not unique because if someone later looks for it
                        // they will find and use it from the global procs.
                        AutoPtr<ProcessState> proc;
                        if (hadData) {
                            AutoPtr<IInterface> _proc;
                            pkgState->mProcesses->Get(CoreUtils::Convert(procName), (IInterface**)&_proc);
                            proc = (ProcessState*)IProcessState::Probe(_proc);
                        }
                        if (proc != NULL) {
                            if (!proc->ReadFromParcel(in, FALSE)) {
                                return NOERROR;
                            }
                        }
                        else {
                            proc = new ProcessState();
                            proc->constructor(commonProc, pkgName, uid, vers, procName, 0);
                            if (!proc->ReadFromParcel(in, TRUE)) {
                                return NOERROR;
                            }
                        }
                        if (DEBUG_PARCEL) Slogger::D(TAG, "Adding package %s process: %s %d %s",
                            pkgName.string(), procName.string(), uid, proc->ToString().string());
                        pkgState->mProcesses->Put(CoreUtils::Convert(procName), (IProcessState*)proc);
                    }
                    else {
                        if (DEBUG_PARCEL) Slogger::D(TAG, "Adding package %s process: %s %d %s",
                            pkgName.string(), procName.string(), uid, commonProc->ToString().string());
                        pkgState->mProcesses->Put(CoreUtils::Convert(procName), (IProcessState*)commonProc);
                    }
                }
                Int32 NSRVS;
                in->ReadInt32(&NSRVS);
                if (NSRVS < 0) {
                    mReadError.AppendFormat("bad package service count: %d", NSRVS);
                    return NOERROR;
                }
                while (NSRVS > 0) {
                    NSRVS--;
                    String serviceName;
                    in->ReadString(&serviceName);
                    if (serviceName == NULL) {
                        mReadError = "bad package service name";
                        return NOERROR;
                    }
                    String processName = version > 9 ? ReadCommonString(in, version) : String(NULL);
                    AutoPtr<ServiceState> serv;
                    if (hadData) {
                        AutoPtr<IInterface> item;
                        pkgState->mServices->Get(CoreUtils::Convert(serviceName), (IInterface**)&item);
                        serv = (ServiceState*)IServiceState::Probe(item);
                    }
                    if (serv == NULL) {
                        serv = new ServiceState();
                        serv->constructor(this, pkgName, serviceName, processName, NULL);
                    }
                    if (!serv->ReadFromParcel(in)) {
                        return NOERROR;
                    }
                    if (DEBUG_PARCEL)
                        Slogger::D(TAG, "Adding package %s service: %s %d %s",
                            pkgName.string(), serviceName.string(), uid, serv->ToString().string());
                    pkgState->mServices->Put(CoreUtils::Convert(serviceName), (IServiceState*)serv);
                }
            }
        }
    }

    mIndexToCommonString = NULL;

    if (DEBUG_PARCEL) Slogger::D(TAG, "Successfully read procstats!");
    return NOERROR;
}

Int32 ProcessStats::AddInt64Data(
    /* [in] */ Int32 index,
    /* [in] */ Int32 type,
    /* [in] */ Int32 num)
{
    Int32 off = AllocInt64Data(num);
    mAddInt64Table = GrowingArrayUtils::Insert(
            mAddInt64Table != NULL ? mAddInt64Table.Get() : EmptyArray::INT32.Get(),
            mAddInt64TableSize, index, type | off);
    mAddInt64TableSize++;
    return off;
}

Int32 ProcessStats::AllocInt64Data(
    /* [in] */ Int32 num)
{
    Int32 whichInt64s = mInt64s.GetSize() - 1;
    AutoPtr<ArrayOf<Int64> > longs = mInt64s[whichInt64s];
    if (mNextInt64 + num > longs->GetLength()) {
        longs = ArrayOf<Int64>::Alloc(LONGS_SIZE);
        mInt64s.PushBack(longs);
        whichInt64s++;
        mNextInt64 = 0;
    }
    Int32 off = (whichInt64s << OFFSET_ARRAY_SHIFT) | (mNextInt64 << OFFSET_INDEX_SHIFT);
    mNextInt64 += num;
    return off;
}

Boolean ProcessStats::ValidateInt64Offset(
    /* [in] */ Int32 off)
{
    Int32 arr = (off >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK;
    if (arr >= (Int32)mInt64s.GetSize()) {
        return FALSE;
    }
    Int32 idx = (off >> OFFSET_INDEX_SHIFT) & OFFSET_INDEX_MASK;
    if (idx >= LONGS_SIZE) {
        return FALSE;
    }
    if (DEBUG_PARCEL) Slogger::D(TAG, "Validated Int64 %s: %d",
        PrintInt64Offset(off).string(), GetInt64(off, 0));
    return TRUE;
}

String ProcessStats::PrintInt64Offset(
    /* [in] */ Int32 off)
{
    StringBuilder sb(16);
    sb.Append("a");
    sb.Append((off >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK);
    sb.Append("i");
    sb.Append((off >> OFFSET_INDEX_SHIFT) & OFFSET_INDEX_MASK);
    sb.Append("t");
    sb.Append((off >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK);
    return sb.ToString();
}

void ProcessStats::SetInt64(
    /* [in] */ Int32 off,
    /* [in] */ Int32 index,
    /* [in] */ Int64 value)
{
    AutoPtr<ArrayOf<Int64> > longs = mInt64s[(off >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK];
    (*longs)[index + ((off >> OFFSET_INDEX_SHIFT) & OFFSET_INDEX_MASK)] = value;
}

Int64 ProcessStats::GetInt64(
    /* [in] */ Int32 off,
    /* [in] */ Int32 index)
{
    AutoPtr<ArrayOf<Int64> > longs = mInt64s[(off >> OFFSET_ARRAY_SHIFT) & OFFSET_ARRAY_MASK];
    return (*longs)[index + ((off >> OFFSET_INDEX_SHIFT) & OFFSET_INDEX_MASK)];
}

Int32 ProcessStats::BinarySearch(
    /* [in] */ const ArrayOf<Int32>& array,
    /* [in] */ Int32 size,
    /* [in] */ Int32 value)
{
    Int32 lo = 0;
    Int32 hi = size - 1;

    while (lo <= hi) {
        Int32 mid = (UInt32)(lo + hi) >> 1;
        Int32 midVal = (array[mid] >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;

        if (midVal < value) {
            lo = mid + 1;
        }
        else if (midVal > value) {
            hi = mid - 1;
        }
        else {
            return mid;  // value found
        }
    }
    return ~lo;  // value not present
}

ECode ProcessStats::GetPackageStateLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 vers,
    /* [out] */ IPackageState** pkgState)
{
    VALIDATE_NOT_NULL(pkgState)
    AutoPtr<IInterface> item;
    mPackages->Get(packageName, uid, (IInterface**)&item);
    AutoPtr<ISparseArray> vpkg = ISparseArray::Probe(item);
    if (vpkg == NULL) {
        CSparseArray::New((ISparseArray**)&vpkg);
        mPackages->Put(packageName, uid, vpkg);
    }
    AutoPtr<IInterface> vItem;
    vpkg->Get(vers, (IInterface**)&vItem);
    AutoPtr<IPackageState> as = IPackageState::Probe(vItem);
    if (as == NULL) {
        as = new PackageState();
        ((PackageState*)as.Get())->constructor(packageName, uid);
        vpkg->Put(vers, as);
    }
    *pkgState = as;
    REFCOUNT_ADD(*pkgState)
    return NOERROR;
}

ECode ProcessStats::GetProcessStateLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 vers,
    /* [in] */ const String& processName,
    /* [out] */ IProcessState** procState)
{
    VALIDATE_NOT_NULL(procState)
    AutoPtr<IPackageState> _pkgState;
    GetPackageStateLocked(packageName, uid, vers, (IPackageState**)&_pkgState);
    PackageState* pkgState = (PackageState*)_pkgState.Get();
    AutoPtr<IInterface> _ps;
    pkgState->mProcesses->Get(CoreUtils::Convert(processName), (IInterface**)&_ps);
    AutoPtr<ProcessState> ps = (ProcessState*)IProcessState::Probe(_ps);
    if (ps != NULL) {
        *procState = ps;
        REFCOUNT_ADD(*procState)
        return NOERROR;
    }
    AutoPtr<IInterface> _commonProc;
    mProcesses->Get(processName, uid, (IInterface**)&_commonProc);
    AutoPtr<ProcessState> commonProc = (ProcessState*)IProcessState::Probe(_commonProc);
    if (commonProc == NULL) {
        commonProc = new ProcessState();
        commonProc->constructor( this, packageName, uid, vers, processName);
        mProcesses->Put(processName, uid, (IProcessState*)commonProc);
        if (DEBUG) Slogger::D(TAG, "GETPROC created new common %s",
            commonProc->ToString().string());
    }
    if (!commonProc->mMultiPackage) {
        if (packageName.Equals(commonProc->mPackage) && vers == commonProc->mVersion) {
            // This common process is not in use by multiple packages, and
            // is for the calling package, so we can just use it directly.
            ps = commonProc;
            if (DEBUG) Slogger::D(TAG, "GETPROC also using for pkg %s",
                commonProc->ToString().string());
        }
        else {
            if (DEBUG) Slogger::D(TAG, "GETPROC need to split common proc!");
            // This common process has not been in use by multiple packages,
            // but it was created for a different package than the caller.
            // We need to convert it to a multi-package process.
            commonProc->mMultiPackage = TRUE;
            // To do this, we need to make two new process states, one a copy
            // of the current state for the process under the original package
            // name, and the second a free new process state for it as the
            // new package name.
            Int64 now = SystemClock::GetUptimeMillis();
            // First let's make a copy of the current process state and put
            // that under the now unique state for its original package name.
            AutoPtr<IPackageState> _commonPkgState;
            GetPackageStateLocked(commonProc->mPackage,uid, commonProc->mVersion,
                (IPackageState**)&_commonPkgState);
            PackageState* commonPkgState = (PackageState*)_commonPkgState.Get();
            if (commonPkgState != NULL) {
                AutoPtr<ProcessState> cloned = commonProc->Clone(commonProc->mPackage, now);
                if (DEBUG) Slogger::D(TAG, "GETPROC setting clone to pkg %s: %s",
                    commonProc->mPackage.string(), cloned->ToString().string());
                commonPkgState->mProcesses->Put(
                    CoreUtils::Convert(commonProc->mName), (IProcessState*)cloned);
                // If this has active services, we need to update their process pointer
                // to point to the new package-specific process state.
                Int32 size;
                commonPkgState->mServices->GetSize(&size);
                for (Int32 i = size - 1; i >= 0; i--) {
                    AutoPtr<IInterface> value;
                    commonPkgState->mServices->GetValueAt(i, (IInterface**)&value);
                    ServiceState* ss = (ServiceState*)IServiceState::Probe(value);
                    if (ss->mProc == commonProc) {
                        if (DEBUG) Slogger::D(TAG, "GETPROC switching service to cloned: %s",
                                ss->ToString().string());
                        ss->mProc = cloned;
                    }
                    else if (DEBUG) {
                        Slogger::D(TAG, "GETPROC leaving proc of %s", ss->ToString().string());
                    }
                }
            }
            else {
                Slogger::W(TAG, "Cloning proc state: no package state %s/%d for proc %s",
                    commonProc->mPackage.string(), uid, commonProc->mName.string());
            }
            // And now make a fresh new process state for the new package name.
            ps = new ProcessState();
            ps->constructor(commonProc, packageName, uid, vers, processName, now);
            if (DEBUG) Slogger::D(TAG, "GETPROC created new pkg %s", ps->ToString().string());
        }
    }
    else {
        // The common process is for multiple packages, we need to create a
        // separate object for the per-package data.
        ps = new ProcessState();
        ps->constructor(commonProc, packageName, uid, vers, processName,
                SystemClock::GetUptimeMillis());
        if (DEBUG) Slogger::D(TAG, "GETPROC created new pkg %s", ps->ToString().string());
    }
    pkgState->mProcesses->Put(CoreUtils::Convert(processName), (IProcessState*)ps);
    if (DEBUG) Slogger::D(TAG, "GETPROC adding new pkg %s", ps->ToString().string());
    *procState = ps;
    REFCOUNT_ADD(*procState)
    return NOERROR;
}

ECode ProcessStats::GetServiceStateLocked(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 vers,
    /* [in] */ const String& processName,
    /* [in] */ const String& className,
    /* [out] */ IServiceState** svcState)
{
    AutoPtr<IPackageState> _as;
    GetPackageStateLocked(packageName, uid, vers, (IPackageState**)&_as);
    PackageState* as = (PackageState*)_as.Get();
    AutoPtr<IInterface> _ss;
    as->mServices->Get(CoreUtils::Convert(className), (IInterface**)&_ss);
    AutoPtr<IServiceState> ss = IServiceState::Probe(_ss);
    if (ss != NULL) {
        if (DEBUG) Slogger::D(TAG, "GETSVC: returning existing %s",
                ((ServiceState*)ss.Get())->ToString().string());
    }
    else {
        AutoPtr<IProcessState> ps;
        if (processName != NULL)
            GetProcessStateLocked(packageName, uid, vers, processName, (IProcessState**)&ps);
        ss = new ServiceState();
        ((ServiceState*)ss.Get())->constructor(this, packageName, className, processName, ps);
        as->mServices->Put(CoreUtils::Convert(className), ss);
        if (DEBUG) Slogger::D(TAG, "GETSVC: creating %s in %s",
            ((ServiceState*)ss.Get())->ToString().string(),
            ((ProcessState*)ps.Get())->ToString().string());
    }
    *svcState = ss;
    REFCOUNT_ADD(*svcState)
    return NOERROR;
}

void ProcessStats::DumpProcessInternalLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ ProcessState* proc,
    /* [in] */ Boolean dumpAll)
{
    if (dumpAll) {
        pw->Print(prefix);
        pw->Print(String("myID="));
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        Int32 hashCode;
        system->IdentityHashCode((IProcessState*)proc, &hashCode);
        pw->Print(StringUtils::ToString(hashCode, 16));
        pw->Print(String(" mCommonProcess="));
        system->IdentityHashCode((IProcessState*)proc->mCommonProcess, &hashCode);
        pw->Print(StringUtils::ToString(hashCode, 16));
        pw->Print(String(" mPackage="));
        pw->Println(proc->mPackage);
        if (proc->mMultiPackage) {
            pw->Print(prefix);
            pw->Print(String("mMultiPackage="));
            pw->Println(proc->mMultiPackage);
        }
        if (proc != proc->mCommonProcess) {
            pw->Print(prefix);
            pw->Print(String("Common Proc: "));
            pw->Print(proc->mCommonProcess->mName);
            pw->Print(String("/"));
            pw->Print(proc->mCommonProcess->mUid);
            pw->Print(String(" pkg="));
            pw->Println(proc->mCommonProcess->mPackage);
        }
    }
    if (proc->mActive) {
        pw->Print(prefix);
        pw->Print(String("mActive="));
        pw->Println(proc->mActive);
    }
    if (proc->mDead) {
        pw->Print(prefix);
        pw->Print(String("mDead="));
        pw->Println(proc->mDead);
    }
    if (proc->mNumActiveServices != 0 || proc->mNumStartedServices != 0) {
        pw->Print(prefix);
        pw->Print(String("mNumActiveServices="));
        pw->Print(proc->mNumActiveServices);
        pw->Print(String(" mNumStartedServices="));
        pw->Println(proc->mNumStartedServices);
    }
}

ECode ProcessStats::DumpLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& reqPackage,
    /* [in] */ Int64 now,
    /* [in] */ Boolean dumpSummary,
    /* [in] */ Boolean dumpAll,
    /* [in] */ Boolean activeOnly)
{
    Int64 totalTime = DumpSingleTime(NULL, String(NULL), *mMemFactorDurations, mMemFactor,
            mStartTime, now);
    Boolean sepNeeded = FALSE;
    if (mSysMemUsageTable != NULL) {
        pw->Println(String("System memory usage:"));
        DumpSysMemUsage(pw, String("  "), *ALL_SCREEN_ADJ, *ALL_MEM_ADJ);
        sepNeeded = TRUE;
    }
    AutoPtr<IArrayMap> pkgMap;
    mPackages->GetMap((IArrayMap**)&pkgMap);
    Boolean printedHeader = FALSE;
    Int32 NPKG;
    pkgMap->GetSize(&NPKG);
    for (Int32 ip = 0; ip < NPKG; ip++) {
        AutoPtr<IInterface> key;
        pkgMap->GetKeyAt(ip, (IInterface**)&key);
        String pkgName;
        ICharSequence::Probe(key)->ToString(&pkgName);
        AutoPtr<IInterface> value;
        pkgMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 NUID;
        uids->GetSize(&NUID);
        for (Int32 iu = 0; iu < NUID; iu++) {
            Int32 uid;
            uids->KeyAt(iu, &uid);
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            AutoPtr<ISparseArray> vpkgs = ISparseArray::Probe(uValue);
            Int32 NVERS;
            vpkgs->GetSize(&NVERS);
            for (Int32 iv = 0; iv < NVERS; iv++) {
                Int32 vers;
                vpkgs->KeyAt(iv, &vers);
                AutoPtr<IInterface> vValue;
                vpkgs->ValueAt(iv, (IInterface**)&vValue);
                PackageState* pkgState = (PackageState*)IPackageState::Probe(vValue);
                Int32 NPROCS;
                pkgState->mProcesses->GetSize(&NPROCS);
                Int32 NSRVS;
                pkgState->mServices->GetSize(&NSRVS);
                Boolean pkgMatch = reqPackage == NULL || reqPackage.Equals(pkgName);
                if (!pkgMatch) {
                    Boolean procMatch = FALSE;
                    for (Int32 iproc = 0; iproc < NPROCS; iproc++) {
                        AutoPtr<IInterface> procValue;
                        pkgState->mProcesses->GetValueAt(iproc, (IInterface**)&procValue);
                        ProcessState* proc = (ProcessState*)IProcessState::Probe(procValue);
                        if (reqPackage.Equals(proc->mName)) {
                            procMatch = TRUE;
                            break;
                        }
                    }
                    if (!procMatch) {
                        continue;
                    }
                }
                if (NPROCS > 0 || NSRVS > 0) {
                    if (!printedHeader) {
                        if (sepNeeded)
                            pw->Println();
                        pw->Println(String("Per-Package Stats:"));
                        printedHeader = TRUE;
                        sepNeeded = TRUE;
                    }
                    pw->Print(String("  * "));
                    pw->Print(pkgName);
                    pw->Print(String(" / "));
                    UserHandle::FormatUid(pw, uid);
                    pw->Print(String(" / v"));
                    pw->Print(vers);
                    pw->Println(String(":"));
                }
                if (!dumpSummary || dumpAll) {
                    for (Int32 iproc = 0; iproc < NPROCS; iproc++) {
                        AutoPtr<IInterface> procValue;
                        pkgState->mProcesses->GetValueAt(iproc, (IInterface**)&procValue);
                        ProcessState* proc = (ProcessState*)IProcessState::Probe(procValue);
                        if (!pkgMatch && !reqPackage.Equals(proc->mName)) {
                            continue;
                        }
                        AutoPtr<IInterface> procKey;
                        pkgState->mProcesses->GetKeyAt(iproc, (IInterface**)&procKey);
                        String procName;
                        ICharSequence::Probe(procKey)->ToString(&procName);
                        if (activeOnly && !proc->IsInUse()) {
                            pw->Print(String("      (Not active: "));
                            pw->Print(procName);
                            pw->Println(String(")"));
                            continue;
                        }
                        pw->Print(String("      Process "));
                        pw->Print(procName);
                        if (proc->mCommonProcess->mMultiPackage) {
                            pw->Print(String(" (multi, "));
                        }
                        else {
                            pw->Print(String(" (unique, "));
                        }
                        pw->Print(proc->mDurationsTableSize);
                        pw->Print(String(" entries)"));
                        pw->Println(String(":"));
                        DumpProcessState(pw, String("        "), proc, *ALL_SCREEN_ADJ, *ALL_MEM_ADJ,
                                *ALL_PROC_STATES, now);
                        DumpProcessPss(pw, String("        "), proc, *ALL_SCREEN_ADJ, *ALL_MEM_ADJ,
                                *ALL_PROC_STATES);
                        DumpProcessInternalLocked(pw, String("        "), proc, dumpAll);
                    }
                }
                else {
                    AutoPtr<IArrayList> procs;
                    CArrayList::New((IArrayList**)&procs);
                    for (Int32 iproc = 0; iproc < NPROCS; iproc++) {
                        AutoPtr<IInterface> procValue;
                        pkgState->mProcesses->GetValueAt(iproc, (IInterface**)&procValue);
                        ProcessState* proc = (ProcessState*)IProcessState::Probe(procValue);
                        if (!pkgMatch && !reqPackage.Equals(proc->mName)) {
                            continue;
                        }
                        if (activeOnly && !proc->IsInUse()) {
                            continue;
                        }
                        procs->Add(procValue);
                    }
                    DumpProcessSummaryLocked(pw, String("      "), procs, ALL_SCREEN_ADJ, ALL_MEM_ADJ,
                        NON_CACHED_PROC_STATES, FALSE, now, totalTime);
                }

                for (Int32 isvc = 0; isvc < NSRVS; isvc++) {
                    AutoPtr<IInterface> svcValue;
                    pkgState->mServices->GetValueAt(isvc, (IInterface**)&svcValue);
                    ServiceState* svc = (ServiceState*)IServiceState::Probe(svcValue);
                    if (!pkgMatch && !reqPackage.Equals(svc->mProcessName)) {
                        continue;
                    }
                    AutoPtr<IInterface> svcKey;
                    pkgState->mServices->GetKeyAt(isvc, (IInterface**)&svcKey);
                    String svcName;
                    ICharSequence::Probe(svcKey)->ToString(&svcName);
                    if (activeOnly && !svc->IsInUse()) {
                        pw->Print(String("      (Not active: "));
                        pw->Print(svcName);
                        pw->Println(String(")"));
                        continue;
                    }
                    if (dumpAll) {
                        pw->Print(String("      Service "));
                    }
                    else {
                        pw->Print(String("      * "));
                    }
                    pw->Print(svcName);
                    pw->Println(String(":"));
                    pw->Print(String("        Process: "));
                    pw->Println(svc->mProcessName);
                    DumpServiceStats(pw, String("        "), String("          "), String("    "),
                            String("Running"), svc, svc->mRunCount, ServiceState::SERVICE_RUN,
                            svc->mRunState, svc->mRunStartTime, now, totalTime, !dumpSummary || dumpAll);
                    DumpServiceStats(pw, String("        "), String("          "), String("    "),
                            String("Started"), svc, svc->mStartedCount, ServiceState::SERVICE_STARTED,
                            svc->mStartedState, svc->mStartedStartTime, now, totalTime, !dumpSummary || dumpAll);
                    DumpServiceStats(pw, String("        "), String("          "), String("      "),
                            String("Bound"), svc, svc->mBoundCount, ServiceState::SERVICE_BOUND,
                            svc->mBoundState, svc->mBoundStartTime, now, totalTime, !dumpSummary || dumpAll);
                    DumpServiceStats(pw, String("        "), String("          "), String("  "),
                            String("Executing"), svc, svc->mExecCount, ServiceState::SERVICE_EXEC,
                            svc->mExecState, svc->mExecStartTime, now, totalTime, !dumpSummary || dumpAll);
                    if (dumpAll) {
                        if (svc->mOwner != NULL) {
                            pw->Print(String("        mOwner="));
                            pw->Println(svc->mOwner);
                        }
                        if (svc->mStarted || svc->mRestarting) {
                            pw->Print(String("        mStarted="));
                            pw->Print(svc->mStarted);
                            pw->Print(String(" mRestarting="));
                            pw->Println(svc->mRestarting);
                        }
                    }
                }
            }
        }
    }

    AutoPtr<IArrayMap> procMap;
    mProcesses->GetMap((IArrayMap**)&procMap);
    printedHeader = FALSE;
    Int32 numShownProcs = 0, numTotalProcs = 0;
    Int32 size;
    procMap->GetSize(&size);
    for (Int32 ip = 0; ip < size; ip++) {
        AutoPtr<IInterface> key;
        procMap->GetKeyAt(ip, (IInterface**)&key);
        String procName;
        ICharSequence::Probe(key)->ToString(&procName);
        AutoPtr<IInterface> value;
        procMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 uSize;
        uids->GetSize(&uSize);
        for (Int32 iu = 0; iu < uSize; iu++) {
            Int32 uid;
            uids->KeyAt(iu, &uid);
            numTotalProcs++;
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            ProcessState* proc = (ProcessState*)IProcessState::Probe(uValue);
            if (proc->mDurationsTableSize == 0 && proc->mCurState == STATE_NOTHING
                    && proc->mPssTableSize == 0) {
                continue;
            }
            if (!proc->mMultiPackage) {
                continue;
            }
            if (reqPackage != NULL && !reqPackage.Equals(procName)
                    && !reqPackage.Equals(proc->mPackage)) {
                continue;
            }
            numShownProcs++;
            if (sepNeeded) {
                pw->Println();
            }
            sepNeeded = TRUE;
            if (!printedHeader) {
                pw->Println(String("Multi-Package Common Processes:"));
                printedHeader = TRUE;
            }
            if (activeOnly && !proc->IsInUse()) {
                pw->Print(String("      (Not active: "));
                pw->Print(procName);
                pw->Println(String(")"));
                continue;
            }
            pw->Print(String("  * "));
            pw->Print(procName);
            pw->Print(String(" / "));
            UserHandle::FormatUid(pw, uid);
            pw->Print(String(" ("));
            pw->Print(proc->mDurationsTableSize);
            pw->Print(String(" entries)"));
            pw->Println(String(":"));
            DumpProcessState(pw, String("        "), proc, *ALL_SCREEN_ADJ, *ALL_MEM_ADJ,
                    *ALL_PROC_STATES, now);
            DumpProcessPss(pw, String("        "), proc, *ALL_SCREEN_ADJ, *ALL_MEM_ADJ,
                    *ALL_PROC_STATES);
            DumpProcessInternalLocked(pw, String("        "), proc, dumpAll);
        }
    }
    if (dumpAll) {
        pw->Println();
        pw->Print(String("  Total procs: "));
        pw->Print(numShownProcs);
        pw->Print(String(" shown of "));
        pw->Print(numTotalProcs);
        pw->Println(String(" total"));
    }

    if (sepNeeded) {
        pw->Println();
    }
    if (dumpSummary) {
        pw->Println(String("Summary:"));
        DumpSummaryLocked(pw, reqPackage, now, activeOnly);
    }
    else {
        DumpTotalsLocked(pw, now);
    }

    if (dumpAll) {
        pw->Println();
        pw->Println(String("Internal state:"));
        pw->Print(String("  Num Int64 arrays: "));
        pw->Println((Int32)mInt64s.GetSize());
        pw->Print(String("  Next Int64 entry: "));
        pw->Println(mNextInt64);
        pw->Print(String("  mRunning="));
        pw->Println(mRunning);
    }
    return NOERROR;
}

Int64 ProcessStats::DumpSingleServiceTime(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ IServiceState* _service,
    /* [in] */ Int32 serviceType,
    /* [in] */ Int32 curState,
    /* [in] */ Int64 curStartTime,
    /* [in] */ Int64 now)
{
    ServiceState* service = (ServiceState*)_service;
    Int64 totalTime = 0;
    Int32 printedScreen = -1;
    for (Int32 iscreen = 0; iscreen < ADJ_COUNT; iscreen += ADJ_SCREEN_MOD) {
        Int32 printedMem = -1;
        for (Int32 imem = 0; imem < ADJ_MEM_FACTOR_COUNT; imem++) {
            Int32 state = imem+iscreen;
            Int64 time = service->GetDuration(serviceType, curState, curStartTime,
                    state, now);
            String running("");
            if (curState == state && pw != NULL) {
                running = " (running)";
            }
            if (time != 0) {
                if (pw != NULL) {
                    pw->Print(prefix);
                    PrintScreenLabel(pw, printedScreen != iscreen
                            ? iscreen : STATE_NOTHING);
                    printedScreen = iscreen;
                    PrintMemLabel(pw, printedMem != imem ? imem : STATE_NOTHING, (Char32)0);
                    printedMem = imem;
                    pw->Print(String(": "));
                    TimeUtils::FormatDuration(time, pw);
                    pw->Println(running);
                }
                totalTime += time;
            }
        }
    }
    if (totalTime != 0 && pw != NULL) {
        pw->Print(prefix);
        pw->Print(String("    TOTAL: "));
        TimeUtils::FormatDuration(totalTime, pw);
        pw->Println();
    }
    return totalTime;
}

void ProcessStats::DumpServiceStats(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ const String& prefixInner,
    /* [in] */ const String& headerPrefix,
    /* [in] */ const String& header,
    /* [in] */ ServiceState* service,
    /* [in] */ Int32 count,
    /* [in] */ Int32 serviceType,
    /* [in] */ Int32 state,
    /* [in] */ Int64 startTime,
    /* [in] */ Int64 now,
    /* [in] */ Int64 totalTime,
    /* [in] */ Boolean dumpAll)
{
    if (count != 0) {
        if (dumpAll) {
            pw->Print(prefix);
            pw->Print(header);
            pw->Print(String(" op count "));
            pw->Print(count);
            pw->Println(String(":"));
            DumpSingleServiceTime(pw, prefixInner, service,
                serviceType, state, startTime, now);
        }
        else {
            Int64 myTime = DumpSingleServiceTime(NULL, String(NULL), service,
                serviceType, state, startTime, now);
            pw->Print(prefix);
            pw->Print(headerPrefix);
            pw->Print(header);
            pw->Print(String(" count "));
            pw->Print(count);
            pw->Print(String(" / time "));
            PrintPercent(pw, (Double)myTime/(Double)totalTime);
            pw->Println();
        }
    }
}

ECode ProcessStats::DumpSummaryLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& reqPackage,
    /* [in] */ Int64 now,
    /* [in] */ Boolean activeOnly)
{
    Int64 totalTime = DumpSingleTime(NULL, String(NULL), *mMemFactorDurations, mMemFactor,
            mStartTime, now);
    DumpFilteredSummaryLocked(pw, String(NULL), String("  "), ALL_SCREEN_ADJ, ALL_MEM_ADJ,
            ALL_PROC_STATES, NON_CACHED_PROC_STATES, now, totalTime, reqPackage, activeOnly);
    pw->Println();
    DumpTotalsLocked(pw, now);
    return NOERROR;
}

Int64 ProcessStats::PrintMemoryCategory(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ const String& label,
    /* [in] */ Double memWeight,
    /* [in] */ Int64 totalTime,
    /* [in] */ Int64 curTotalMem,
    /* [in] */ Int32 samples)
{
    if (memWeight != 0) {
        Int64 mem = (Int64)(memWeight * 1024 / totalTime);
        pw->Print(prefix);
        pw->Print(label);
        pw->Print(String(": "));
        PrintSizeValue(pw, mem);
        pw->Print(String(" ("));
        pw->Print(samples);
        pw->Print(String(" samples)"));
        pw->Println();
        return curTotalMem + mem;
    }
    return curTotalMem;
}

void ProcessStats::DumpTotalsLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int64 now)
{
    pw->Println(String("Run time Stats:"));
    DumpSingleTime(pw, String("  "), *mMemFactorDurations, mMemFactor, mStartTime, now);
    pw->Println();
    pw->Println(String("Memory usage:"));
    AutoPtr<CTotalMemoryUseCollection> totalMem;
    CTotalMemoryUseCollection::NewByFriend(ALL_SCREEN_ADJ, ALL_MEM_ADJ,
        (CTotalMemoryUseCollection**)&totalMem);
    ComputeTotalMemoryUse(totalMem, now);
    Int64 totalPss = 0;
    totalPss = PrintMemoryCategory(pw, String("  "), String("Kernel "), totalMem->mSysMemKernelWeight,
            totalMem->mTotalTime, totalPss, totalMem->mSysMemSamples);
    totalPss = PrintMemoryCategory(pw, String("  "), String("Native "), totalMem->mSysMemNativeWeight,
            totalMem->mTotalTime, totalPss, totalMem->mSysMemSamples);
    for (Int32 i = 0; i < STATE_COUNT; i++) {
        // Skip restarting service state -- that is not actually a running process.
        if (i != STATE_SERVICE_RESTARTING) {
            totalPss = PrintMemoryCategory(pw, String("  "), (*STATE_NAMES)[i],
                (*totalMem->mProcessStateWeight)[i], totalMem->mTotalTime, totalPss,
                (*totalMem->mProcessStateSamples)[i]);
        }
    }
    totalPss = PrintMemoryCategory(pw, String("  "), String("Cached "), totalMem->mSysMemCachedWeight,
            totalMem->mTotalTime, totalPss, totalMem->mSysMemSamples);
    totalPss = PrintMemoryCategory(pw, String("  "), String("Free   "), totalMem->mSysMemFreeWeight,
            totalMem->mTotalTime, totalPss, totalMem->mSysMemSamples);
    totalPss = PrintMemoryCategory(pw, String("  "), String("Z-Ram   "), totalMem->mSysMemZRamWeight,
            totalMem->mTotalTime, totalPss, totalMem->mSysMemSamples);
    pw->Print(String("  TOTAL  : "));
    PrintSizeValue(pw, totalPss);
    pw->Println();
    PrintMemoryCategory(pw, String("  "), (*STATE_NAMES)[STATE_SERVICE_RESTARTING],
        (*totalMem->mProcessStateWeight)[STATE_SERVICE_RESTARTING], totalMem->mTotalTime, totalPss,
        (*totalMem->mProcessStateSamples)[STATE_SERVICE_RESTARTING]);
    pw->Println();
    pw->Print(String("          Start time: "));
    pw->Print(DateFormat::Format(CoreUtils::Convert(String("yyyy-MM-dd HH:mm:ss")),
        mTimePeriodStartClock));
    pw->Println();
    pw->Print(String("  Total elapsed time: "));
    TimeUtils::FormatDuration(
            (mRunning ? SystemClock::GetElapsedRealtime() : mTimePeriodEndRealtime)
                - mTimePeriodStartRealtime, pw);
    Boolean partial = TRUE;
    if ((mFlags&FLAG_SHUTDOWN) != 0) {
        pw->Print(String(" (shutdown)"));
        partial = FALSE;
    }
    if ((mFlags&FLAG_SYSPROPS) != 0) {
        pw->Print(String(" (sysprops)"));
        partial = FALSE;
    }
    if ((mFlags&FLAG_COMPLETE) != 0) {
        pw->Print(String(" (complete)"));
        partial = FALSE;
    }
    if (partial) {
        pw->Print(String(" (partial)"));
    }
    pw->PrintChar(' ');
    pw->Print(mRuntime);
    pw->Println();
}

void ProcessStats::DumpFilteredSummaryLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& header,
    /* [in] */ const String& prefix,
    /* [in] */ ArrayOf<Int32>* screenStates,
    /* [in] */ ArrayOf<Int32>* memStates,
    /* [in] */ ArrayOf<Int32>* procStates,
    /* [in] */ ArrayOf<Int32>* sortProcStates,
    /* [in] */ Int64 now,
    /* [in] */ Int64 totalTime,
    /* [in] */ const String& reqPackage,
    /* [in] */ Boolean activeOnly)
{
    AutoPtr<IArrayList> procs;
    CollectProcessesLocked(screenStates, memStates, procStates,
        sortProcStates, now, reqPackage, activeOnly, (IArrayList**)&procs);
    Int32 size;
    procs->GetSize(&size);
    if (size > 0) {
        if (header != NULL) {
            pw->Println();
            pw->Println(header);
        }
        DumpProcessSummaryLocked(pw, prefix, procs, screenStates, memStates,
                sortProcStates, TRUE, now, totalTime);
    }
}

ECode ProcessStats::CollectProcessesLocked(
    /* [in] */ ArrayOf<Int32>* screenStates,
    /* [in] */ ArrayOf<Int32>* memStates,
    /* [in] */ ArrayOf<Int32>* procStates,
    /* [in] */ ArrayOf<Int32>* sortProcStates,
    /* [in] */ Int64 now,
    /* [in] */ const String& reqPackage,
    /* [in] */ Boolean activeOnly,
    /* [out] */ IArrayList** list)
{
    AutoPtr<ICollection> foundProcs;
    CArraySet::New((ICollection**)&foundProcs);
    AutoPtr<IArrayMap> pkgMap;
    mPackages->GetMap((IArrayMap**)&pkgMap);
    Int32 NPKG;
    pkgMap->GetSize(&NPKG);
    for (Int32 ip = 0; ip < NPKG; ip++) {
        AutoPtr<IInterface> key;
        pkgMap->GetKeyAt(ip, (IInterface**)&key);
        String pkgName;
        ICharSequence::Probe(key)->ToString(&pkgName);
        AutoPtr<IInterface> value;
        pkgMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 NUID;
        uids->GetSize(&NUID);
        for (Int32 iu = 0; iu < NUID; iu++) {
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            AutoPtr<ISparseArray> vpkgs = ISparseArray::Probe(uValue);
            Int32 NVERS;
            vpkgs->GetSize(&NVERS);
            for (Int32 iv = 0; iv < NVERS; iv++) {
                AutoPtr<IInterface> vValue;
                vpkgs->ValueAt(iv, (IInterface**)&vValue);
                PackageState* pkgState = (PackageState*)IPackageState::Probe(vValue);
                Int32 NPROCS;
                pkgState->mProcesses->GetSize(&NPROCS);
                Boolean pkgMatch = reqPackage == NULL || reqPackage.Equals(pkgName);
                for (Int32 iproc = 0; iproc < NPROCS; iproc++) {
                    AutoPtr<IInterface> procValue;
                    pkgState->mProcesses->GetValueAt(iproc, (IInterface**)&procValue);
                    ProcessState* proc = (ProcessState*)IProcessState::Probe(procValue);
                    if (!pkgMatch && !reqPackage.Equals(proc->mName)) {
                        continue;
                    }
                    if (activeOnly && !proc->IsInUse()) {
                        continue;
                    }
                    foundProcs->Add((IProcessState*)proc->mCommonProcess);
                }
            }
        }
    }

    Int32 size;
    foundProcs->GetSize(&size);
    AutoPtr<IArrayList> outProcs;
    CArrayList::New(size, (IArrayList**)&outProcs);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> item;
        IArraySet::Probe(foundProcs)->GetValueAt(i, (IInterface**)&item);
        AutoPtr<ProcessState> proc = (ProcessState*)IProcessState::Probe(item);
        if (ComputeProcessTimeLocked(proc, *screenStates, *memStates, *procStates, now) > 0) {
            outProcs->Add(item);
            if (procStates != sortProcStates) {
                ComputeProcessTimeLocked(proc, *screenStates, *memStates, *sortProcStates, now);
            }
        }
    }

    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    AutoPtr<ProcessStateComparator> comparator = new ProcessStateComparator();
    collections->Sort(IList::Probe(outProcs), comparator);
    *list = outProcs;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

String ProcessStats::CollapseString(
    /* [in] */ const String& pkgName,
    /* [in] */ const String& itemName)
{
    if (itemName.StartWith(pkgName)) {
        Int32 ITEMLEN = itemName.GetLength();
        Int32 PKGLEN = pkgName.GetLength();
        if (ITEMLEN == PKGLEN) {
            return String("");
        }
        else if (ITEMLEN >= PKGLEN) {
            if (itemName.GetChar(PKGLEN) == '.') {
                return itemName.Substring(PKGLEN);
            }
        }
    }
    return itemName;
}

ECode ProcessStats::DumpCheckinLocked(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& reqPackage)
{
    Int64 now = SystemClock::GetUptimeMillis();
    AutoPtr<IArrayMap> pkgMap;
    mPackages->GetMap((IArrayMap**)&pkgMap);
    pw->Println(String("vers,5"));
    pw->Print(String("period,"));
    pw->Print(mTimePeriodStartClockStr);
    pw->Print(String(","));
    pw->Print(mTimePeriodStartRealtime);
    pw->Print(String(","));
    pw->Print(mRunning ? SystemClock::GetElapsedRealtime() : mTimePeriodEndRealtime);
    Boolean partial = TRUE;
    if ((mFlags&FLAG_SHUTDOWN) != 0) {
        pw->Print(String(",shutdown"));
        partial = FALSE;
    }
    if ((mFlags&FLAG_SYSPROPS) != 0) {
        pw->Print(String(",sysprops"));
        partial = FALSE;
    }
    if ((mFlags&FLAG_COMPLETE) != 0) {
        pw->Print(String(",complete"));
        partial = FALSE;
    }
    if (partial) {
        pw->Print(String(",partial"));
    }
    pw->Println();
    pw->Print(String("config,"));
    pw->Println(mRuntime);

    Int32 NPKG;
    pkgMap->GetSize(&NPKG);
    for (Int32 ip = 0; ip < NPKG; ip++) {
        AutoPtr<IInterface> key;
        pkgMap->GetKeyAt(ip, (IInterface**)&key);
        String pkgName;
        ICharSequence::Probe(key)->ToString(&pkgName);
        if (reqPackage != NULL && !reqPackage.Equals(pkgName)) {
            continue;
        }
        AutoPtr<IInterface> value;
        pkgMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 NUID;
        uids->GetSize(&NUID);
        for (Int32 iu = 0; iu < NUID; iu++) {
            Int32 uid;
            uids->KeyAt(iu, &uid);
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            AutoPtr<ISparseArray> vpkgs = ISparseArray::Probe(uValue);
            Int32 NVERS;
            vpkgs->GetSize(&NVERS);
            for (Int32 iv = 0; iv < NVERS; iv++) {
                Int32 vers;
                vpkgs->KeyAt(iv, &vers);
                AutoPtr<IInterface> vValue;
                vpkgs->ValueAt(iv, (IInterface**)&vValue);
                PackageState* pkgState = (PackageState*)IPackageState::Probe(vValue);
                Int32 NPROCS;
                pkgState->mProcesses->GetSize(&NPROCS);
                Int32 NSRVS;
                pkgState->mServices->GetSize(&NSRVS);
                for (Int32 iproc = 0; iproc < NPROCS; iproc++) {
                    AutoPtr<IInterface> procValue;
                    pkgState->mProcesses->GetValueAt(iproc, (IInterface**)&procValue);
                    ProcessState* proc = (ProcessState*)IProcessState::Probe(procValue);
                    pw->Print(String("pkgproc,"));
                    pw->Print(pkgName);
                    pw->Print(String(","));
                    pw->Print(uid);
                    pw->Print(String(","));
                    pw->Print(vers);
                    pw->Print(String(","));
                    AutoPtr<IInterface> procKey;
                    pkgState->mProcesses->GetKeyAt(iproc, (IInterface**)&procKey);
                    String procName;
                    ICharSequence::Probe(procKey)->ToString(&procName);
                    pw->Print(CollapseString(pkgName, procName));
                    DumpAllProcessStateCheckin(pw, proc, now);
                    pw->Println();
                    if (proc->mPssTableSize > 0) {
                        pw->Print(String("pkgpss,"));
                        pw->Print(pkgName);
                        pw->Print(String(","));
                        pw->Print(uid);
                        pw->Print(String(","));
                        pw->Print(vers);
                        pw->Print(String(","));
                        pw->Print(CollapseString(pkgName, procName));
                        DumpAllProcessPssCheckin(pw, proc);
                        pw->Println();
                    }
                    if (proc->mNumExcessiveWake > 0 || proc->mNumExcessiveCpu > 0
                            || proc->mNumCachedKill > 0) {
                        pw->Print(String("pkgkills,"));
                        pw->Print(pkgName);
                        pw->Print(String(","));
                        pw->Print(uid);
                        pw->Print(String(","));
                        pw->Print(vers);
                        pw->Print(String(","));
                        pw->Print(CollapseString(pkgName, procName));
                        pw->Print(String(","));
                        pw->Print(proc->mNumExcessiveWake);
                        pw->Print(String(","));
                        pw->Print(proc->mNumExcessiveCpu);
                        pw->Print(String(","));
                        pw->Print(proc->mNumCachedKill);
                        pw->Print(String(","));
                        pw->Print(proc->mMinCachedKillPss);
                        pw->Print(String(":"));
                        pw->Print(proc->mAvgCachedKillPss);
                        pw->Print(String(":"));
                        pw->Print(proc->mMaxCachedKillPss);
                        pw->Println();
                    }
                }
                for (Int32 isvc = 0; isvc < NSRVS; isvc++) {
                    AutoPtr<IInterface> svcKey;
                    pkgState->mServices->GetKeyAt(isvc, (IInterface**)&svcKey);
                    String serviceName;
                    ICharSequence::Probe(svcKey)->ToString(&serviceName);
                    AutoPtr<IInterface> svcValue;
                    pkgState->mServices->GetValueAt(isvc, (IInterface**)&svcValue);
                    ServiceState* svc = (ServiceState*)IServiceState::Probe(svcValue);
                    DumpServiceTimeCheckin(pw, String("pkgsvc-run"), pkgName, uid, vers, serviceName,
                            svc, ServiceState::SERVICE_RUN, svc->mRunCount,
                            svc->mRunState, svc->mRunStartTime, now);
                    DumpServiceTimeCheckin(pw, String("pkgsvc-start"), pkgName, uid, vers, serviceName,
                            svc, ServiceState::SERVICE_STARTED, svc->mStartedCount,
                            svc->mStartedState, svc->mStartedStartTime, now);
                    DumpServiceTimeCheckin(pw, String("pkgsvc-bound"), pkgName, uid, vers, serviceName,
                            svc, ServiceState::SERVICE_BOUND, svc->mBoundCount,
                            svc->mBoundState, svc->mBoundStartTime, now);
                    DumpServiceTimeCheckin(pw, String("pkgsvc-exec"), pkgName, uid, vers, serviceName,
                            svc, ServiceState::SERVICE_EXEC, svc->mExecCount,
                            svc->mExecState, svc->mExecStartTime, now);
                }
            }
        }
    }

    AutoPtr<IArrayMap> procMap;
    mProcesses->GetMap((IArrayMap**)&procMap);
    Int32 size;
    procMap->GetSize(&size);
    for (Int32 ip = 0; ip < size; ip++) {
        AutoPtr<IInterface> key;
        procMap->GetKeyAt(ip, (IInterface**)&key);
        String procName;
        ICharSequence::Probe(key)->ToString(&procName);
        AutoPtr<IInterface> value;
        procMap->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<ISparseArray> uids = ISparseArray::Probe(value);
        Int32 uSize;
        uids->GetSize(&uSize);
        for (Int32 iu = 0; iu < uSize; iu++) {
            Int32 uid;
            uids->KeyAt(iu, &uid);
            AutoPtr<IInterface> uValue;
            uids->ValueAt(iu, (IInterface**)&uValue);
            ProcessState* procState = (ProcessState*)IProcessState::Probe(uValue);
            if (procState->mDurationsTableSize > 0) {
                pw->Print(String("proc,"));
                pw->Print(procName);
                pw->Print(String(","));
                pw->Print(uid);
                DumpAllProcessStateCheckin(pw, procState, now);
                pw->Println();
            }
            if (procState->mPssTableSize > 0) {
                pw->Print(String("pss,"));
                pw->Print(procName);
                pw->Print(String(","));
                pw->Print(uid);
                DumpAllProcessPssCheckin(pw, procState);
                pw->Println();
            }
            if (procState->mNumExcessiveWake > 0 || procState->mNumExcessiveCpu > 0
                    || procState->mNumCachedKill > 0) {
                pw->Print(String("kills,"));
                pw->Print(procName);
                pw->Print(String(","));
                pw->Print(uid);
                pw->Print(String(","));
                pw->Print(procState->mNumExcessiveWake);
                pw->Print(String(","));
                pw->Print(procState->mNumExcessiveCpu);
                pw->Print(String(","));
                pw->Print(procState->mNumCachedKill);
                pw->Print(String(","));
                pw->Print(procState->mMinCachedKillPss);
                pw->Print(String(":"));
                pw->Print(procState->mAvgCachedKillPss);
                pw->Print(String(":"));
                pw->Print(procState->mMaxCachedKillPss);
                pw->Println();
            }
        }
    }
    pw->Print(String("total"));
    DumpAdjTimesCheckin(pw, String(","), *mMemFactorDurations, mMemFactor,
            mStartTime, now);
    if (mSysMemUsageTable != NULL) {
        pw->Print(String("sysmemusage"));
        for (Int32 i = 0; i < mSysMemUsageTableSize; i++) {
            Int32 off = (*mSysMemUsageTable)[i];
            Int32 type = (off >> OFFSET_TYPE_SHIFT) & OFFSET_TYPE_MASK;
            pw->Print(String(","));
            PrintProcStateTag(pw, type);
            for (Int32 j = SYS_MEM_USAGE_SAMPLE_COUNT; j < SYS_MEM_USAGE_COUNT; j++) {
                if (j > SYS_MEM_USAGE_CACHED_MINIMUM) {
                    pw->Print(String(":"));
                }
                pw->Print(GetInt64(off, j));
            }
        }
    }
    pw->Println();
    AutoPtr<CTotalMemoryUseCollection> totalMem;
    CTotalMemoryUseCollection::NewByFriend(ALL_SCREEN_ADJ, ALL_MEM_ADJ,
        (CTotalMemoryUseCollection**)&totalMem);
    ComputeTotalMemoryUse(totalMem, now);
    pw->Print(String("weights,"));
    pw->Print(totalMem->mTotalTime);
    pw->Print(String(","));
    pw->Print(totalMem->mSysMemCachedWeight);
    pw->Print(String(":"));
    pw->Print(totalMem->mSysMemSamples);
    pw->Print(String(","));
    pw->Print(totalMem->mSysMemFreeWeight);
    pw->Print(String(":"));
    pw->Print(totalMem->mSysMemSamples);
    pw->Print(String(","));
    pw->Print(totalMem->mSysMemZRamWeight);
    pw->Print(String(":"));
    pw->Print(totalMem->mSysMemSamples);
    pw->Print(String(","));
    pw->Print(totalMem->mSysMemKernelWeight);
    pw->Print(String(":"));
    pw->Print(totalMem->mSysMemSamples);
    pw->Print(String(","));
    pw->Print(totalMem->mSysMemNativeWeight);
    pw->Print(String(":"));
    pw->Print(totalMem->mSysMemSamples);
    for (Int32 i = 0; i < STATE_COUNT; i++) {
        pw->Print(String(","));
        pw->Print((*totalMem->mProcessStateWeight)[i]);
        pw->Print(String(":"));
        pw->Print((*totalMem->mProcessStateSamples)[i]);
    }
    pw->Println();
    return NOERROR;
}

ECode ProcessStats::GetReadError(
    /* [out] */ String* readError)
{
    VALIDATE_NOT_NULL(readError)
    *readError = mReadError;
    return NOERROR;
}

ECode ProcessStats::SetReadError(
    /* [in] */ const String& readError)
{
    mReadError = readError;
    return NOERROR;
}

ECode ProcessStats::GetTimePeriodStartClockStr(
    /* [out] */ String* timePeriodStartClockStr)
{
    VALIDATE_NOT_NULL(timePeriodStartClockStr)
    *timePeriodStartClockStr = mTimePeriodStartClockStr;
    return NOERROR;
}

ECode ProcessStats::SetTimePeriodStartClockStr(
    /* [in] */ const String& timePeriodStartClockStr)
{
    mTimePeriodStartClockStr = timePeriodStartClockStr;
    return NOERROR;
}

ECode ProcessStats::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags)
    *flags = mFlags;
    return NOERROR;
}

ECode ProcessStats::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode ProcessStats::GetPackages(
    /* [out] */ IProcessMap** packages)
{
    VALIDATE_NOT_NULL(packages)
    *packages = mPackages;
    REFCOUNT_ADD(*packages)
    return NOERROR;
}

ECode ProcessStats::GetProcesses(
    /* [out] */ IProcessMap** processes)
{
    VALIDATE_NOT_NULL(processes)
    *processes = mProcesses;
    REFCOUNT_ADD(*processes)
    return NOERROR;
}

ECode ProcessStats::GetMemFactorDurations(
    /* [out, callee] */ ArrayOf<Int64>** memFactorDurations)
{
    VALIDATE_NOT_NULL(memFactorDurations)
    *memFactorDurations = mMemFactorDurations;
    REFCOUNT_ADD(*memFactorDurations)
    return NOERROR;
}

ECode ProcessStats::GetMemFactor(
    /* [out] */ Int32* memFactor)
{
    VALIDATE_NOT_NULL(memFactor)
    *memFactor = mMemFactor;
    return NOERROR;
}

ECode ProcessStats::SetMemFactor(
    /* [in] */ Int32 memFactor)
{
    mMemFactor = memFactor;
    return NOERROR;
}

ECode ProcessStats::GetStartTime(
    /* [out] */ Int64* startTime)
{
    VALIDATE_NOT_NULL(startTime)
    *startTime = mStartTime;
    return NOERROR;
}

ECode ProcessStats::SetStartTime(
    /* [in] */ Int64 startTime)
{
    mStartTime = startTime;
    return NOERROR;
}

ECode ProcessStats::GetSysMemUsageTable(
    /* [out, callee] */ ArrayOf<Int32>** sysMemUsageTable)
{
    VALIDATE_NOT_NULL(sysMemUsageTable)
    *sysMemUsageTable = mSysMemUsageTable;
    REFCOUNT_ADD(*sysMemUsageTable)
    return NOERROR;
}

ECode ProcessStats::SetSysMemUsageTable(
    /* [in] */ ArrayOf<Int32>* sysMemUsageTable)
{
    mSysMemUsageTable = sysMemUsageTable;
    return NOERROR;
}

ECode ProcessStats::GetSysMemUsageTableSize(
    /* [out] */ Int32* sysMemUsageTableSize)
{
    VALIDATE_NOT_NULL(sysMemUsageTableSize)
    *sysMemUsageTableSize = mSysMemUsageTableSize;
    return NOERROR;
}

ECode ProcessStats::SetSysMemUsageTableSize(
    /* [in] */ Int32 sysMemUsageTableSize)
{
    mSysMemUsageTableSize = sysMemUsageTableSize;
    return NOERROR;
}

ECode ProcessStats::GetSysMemUsageArgs(
    /* [out, callee] */ ArrayOf<Int64>** sysMemUsageArgs)
{
    VALIDATE_NOT_NULL(sysMemUsageArgs)
    *sysMemUsageArgs = mSysMemUsageArgs;
    REFCOUNT_ADD(*sysMemUsageArgs)
    return NOERROR;
}

ECode ProcessStats::GetTimePeriodStartClock(
    /* [out] */ Int64* timePeriodStartClock)
{
    VALIDATE_NOT_NULL(timePeriodStartClock)
    *timePeriodStartClock = mTimePeriodStartClock;
    return NOERROR;
}

ECode ProcessStats::SetTimePeriodStartClock(
    /* [in] */ Int64 timePeriodStartClock)
{
    mTimePeriodStartClock = timePeriodStartClock;
    return NOERROR;
}

ECode ProcessStats::GetTimePeriodStartRealtime(
    /* [out] */ Int64* timePeriodStartRealtime)
{
    VALIDATE_NOT_NULL(timePeriodStartRealtime)
    *timePeriodStartRealtime = mTimePeriodStartRealtime;
    return NOERROR;
}

ECode ProcessStats::SetTimePeriodStartRealtime(
    /* [in] */ Int64 timePeriodStartRealtime)
{
    mTimePeriodStartRealtime = timePeriodStartRealtime;
    return NOERROR;
}

ECode ProcessStats::GetTimePeriodEndRealtime(
    /* [out] */ Int64* timePeriodEndRealtime)
{
    VALIDATE_NOT_NULL(timePeriodEndRealtime)
    *timePeriodEndRealtime = mTimePeriodEndRealtime;
    return NOERROR;
}

ECode ProcessStats::SetTimePeriodEndRealtime(
    /* [in] */ Int64 timePeriodEndRealtime)
{
    mTimePeriodEndRealtime = timePeriodEndRealtime;
    return NOERROR;
}

ECode ProcessStats::GetTimePeriodStartUptime(
    /* [out] */ Int64* timePeriodStartUptime)
{
    VALIDATE_NOT_NULL(timePeriodStartUptime)
    *timePeriodStartUptime = mTimePeriodStartUptime;
    return NOERROR;
}

ECode ProcessStats::SetTimePeriodStartUptime(
    /* [in] */ Int64 timePeriodStartUptime)
{
    mTimePeriodStartUptime = timePeriodStartUptime;
    return NOERROR;
}

ECode ProcessStats::GetTimePeriodEndUptime(
    /* [out] */ Int64* timePeriodEndUptime)
{
    VALIDATE_NOT_NULL(timePeriodEndUptime)
    *timePeriodEndUptime = mTimePeriodEndUptime;
    return NOERROR;
}

ECode ProcessStats::SetTimePeriodEndUptime(
    /* [in] */ Int64 timePeriodEndUptime)
{
    mTimePeriodEndUptime = timePeriodEndUptime;
    return NOERROR;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
