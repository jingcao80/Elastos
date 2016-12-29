
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/internal/os/ProcessCpuTracker.h"
#include "elastos/droid/internal/utility/CFastPrintWriter.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/CStrictMode.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Internal::Utility::CFastPrintWriter;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::CStrictMode;
using Elastos::Droid::Os::IStrictModeThreadPolicy;
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IComparator;
using Elastos::IO::IStringWriter;
using Elastos::IO::CStringWriter;
using Elastos::IO::CPrintWriter;
using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IFlushable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

//========================================================================
// ProcessCpuTracker::Stats
//========================================================================

CAR_INTERFACE_IMPL(ProcessCpuTracker::Stats, Object, IProcessCpuTrackerStats)

ProcessCpuTracker::Stats::Stats(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 parentPid,
    /* [in] */ Boolean includeThreads)
    : mPid(pid)
    , mUid(0)
    , mInteresting(TRUE)
    , mNameWidth(0)
    , mVsize(0)
    , mBase_uptime(0)
    , mRel_uptime(0)
    , mBase_utime(0)
    , mBase_stime(0)
    , mRel_utime(0)
    , mRel_stime(0)
    , mBase_minfaults(0)
    , mBase_majfaults(0)
    , mRel_minfaults(0)
    , mRel_majfaults(0)
    , mActive(FALSE)
    , mWorking(FALSE)
    , mAdded(FALSE)
    , mRemoved(FALSE)
{
    if (parentPid < 0) {
        AutoPtr<IFile> procDir, sFile, cFile, tDir;
        CFile::New(String("/proc"), StringUtils::ToString(mPid), (IFile**)&procDir);
        CFile::New(procDir, String("/proc"), (IFile**)&sFile);
        sFile->ToString(&mStatFile);
        CFile::New(procDir, String("cmdline"), (IFile**)&cFile);
        cFile->ToString(&mCmdlineFile);
        CFile::New(procDir, String("task"), (IFile**)&tDir);
        tDir->ToString(&mThreadsDir);
        if (includeThreads) {
            CArrayList::New((IArrayList**)&mThreadStats);
            CArrayList::New((IArrayList**)&mWorkingThreads);
        }
    }
    else {
        AutoPtr<IFile> procDir, tempDir, tDir, sFile;
        CFile::New(String("/proc"), StringUtils::ToString(parentPid), (IFile**)&procDir);
        CFile::New(procDir, String("task"), (IFile**)&tempDir);
        CFile::New(tempDir, StringUtils::ToString(mPid), (IFile**)&tDir);
        CFile::New(tDir, String("stat"), (IFile**)&sFile);
        sFile->ToString(&mStatFile);
        mCmdlineFile = NULL;
        mThreadsDir = NULL;
    }
    mUid = FileUtils::GetUid(mStatFile);
}

ECode ProcessCpuTracker::Stats::GetPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);
    *pid = mPid;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);
    *uid = mUid;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetBatteryStats(
    /* [out] */ IBatteryStatsImplUidProc** batteryStats)
{
    VALIDATE_NOT_NULL(batteryStats)
    *batteryStats = mBatteryStats;
    REFCOUNT_ADD(*batteryStats);
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetBatteryStats(
    /* [in] */ IBatteryStatsImplUidProc* batteryStats)
{
    mBatteryStats = batteryStats;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetInteresting(
    /* [out] */ Boolean* interesting)
{
    VALIDATE_NOT_NULL(interesting);
    *interesting = mInteresting;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetInteresting(
    /* [in] */ Boolean interesting)
{
    mInteresting = interesting;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetBaseName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mBaseName;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetBaseName(
    /* [in] */ const String& baseName)
{
    mBaseName = baseName;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetNameWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mNameWidth;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetNameWidth(
    /* [in] */ Int32 nameWidth)
{
    mNameWidth = nameWidth;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetVsize(
    /* [out] */ Int64* vsize)
{
    VALIDATE_NOT_NULL(vsize);
    *vsize = mVsize;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetVsize(
    /* [in] */ Int64 vsize)
{
    mVsize = vsize;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetBaseUptime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mBase_uptime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetBaseUptime(
    /* [in] */ Int64 baseUptime)
{
    mBase_uptime = baseUptime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetRelUptime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mRel_uptime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetRelUptime(
    /* [in] */ Int64 relUptime)
{
    mRel_uptime = relUptime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetBaseUtime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mBase_utime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetBaseUtime(
    /* [in] */ Int64 baseUtime)
{
    mBase_utime = baseUtime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetBaseStime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mBase_stime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetBaseStime(
    /* [in] */ Int64 baseStime)
{
    mBase_stime = baseStime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetRelUtime(
    /* [out] */ Int32* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mRel_utime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetRelUtime(
    /* [in] */ Int32 relUtime)
{
    mRel_utime = relUtime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetRelStime(
    /* [out] */ Int32* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mRel_stime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetRelStime(
    /* [in] */ Int32 relStime)
{
    mRel_stime = relStime;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetBaseMinfaults(
    /* [out] */ Int64* faults)
{
    VALIDATE_NOT_NULL(faults);
    *faults = mBase_minfaults;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetBaseMinfaults(
    /* [in] */ Int64 baseMinfaults)
{
    mBase_minfaults = baseMinfaults;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetBaseMajfaults(
    /* [out] */ Int64* faults)
{
    VALIDATE_NOT_NULL(faults);
    *faults = mBase_majfaults;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetBaseMajfaults(
    /* [in] */ Int64 baseMajfaults)
{
    mBase_majfaults = baseMajfaults;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetRelMinfaults(
    /* [out] */ Int32* faults)
{
    VALIDATE_NOT_NULL(faults);
    *faults = mRel_minfaults;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetRelMinfaults(
    /* [in] */ Int32 relMinfaults)
{
    mRel_minfaults = relMinfaults;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetRelMajfaults(
    /* [out] */ Int32* faults)
{
    VALIDATE_NOT_NULL(faults);
    *faults = mRel_majfaults;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetRelMajfaults(
    /* [in] */ Int32 relMajfaults)
{
    mRel_majfaults = relMajfaults;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetActive(
    /* [out] */ Boolean* active)
{
    VALIDATE_NOT_NULL(active);
    *active = mActive;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetActive(
    /* [in] */ Boolean active)
{
    mActive = active;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetWorking(
    /* [out] */ Boolean* working)
{
    VALIDATE_NOT_NULL(working);
    *working = mWorking;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetWorking(
    /* [in] */ Boolean working)
{
    mWorking = working;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetAdded(
    /* [out] */ Boolean* added)
{
    VALIDATE_NOT_NULL(added);
    *added = mAdded;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetAdded(
    /* [in] */ Boolean added)
{
    mAdded = added;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::GetRemoved(
    /* [out] */ Boolean* removed)
{
    VALIDATE_NOT_NULL(removed);
    *removed = mRemoved;
    return NOERROR;
}

ECode ProcessCpuTracker::Stats::SetRemoved(
    /* [in] */ Boolean removed)
{
    mRemoved = removed;
    return NOERROR;
}


//========================================================================
// ProcessCpuTracker::Comparator
//========================================================================
CAR_INTERFACE_IMPL(ProcessCpuTracker::Comparator, Object, IComparator)

ECode ProcessCpuTracker::Comparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Stats* sta = (Stats*)IProcessCpuTrackerStats::Probe(lhs);
    Stats* stb = (Stats*)IProcessCpuTrackerStats::Probe(rhs);
    Int32 ta = sta->mRel_utime + sta->mRel_stime;
    Int32 tb = stb->mRel_utime + stb->mRel_stime;
    if (ta != tb) {
        *result = ta > tb ? -1 : 1;
        return NOERROR;
    }
    if (sta->mAdded != stb->mAdded) {
        *result = sta->mAdded ? -1 : 1;
        return NOERROR;
    }
    if (sta->mRemoved != stb->mRemoved) {
        *result = sta->mAdded ? -1 : 1;
        return NOERROR;
    }
    *result = 0;
    return NOERROR;
}


//========================================================================
// ProcessCpuTracker
//========================================================================
const String ProcessCpuTracker::TAG("ProcessStats");
const Boolean ProcessCpuTracker::DEBUG = FALSE;
const Boolean ProcessCpuTracker::localLOGV = DEBUG || FALSE;

AutoPtr<ArrayOf<Int32> > ProcessCpuTracker::InitPROCESS_STATS_FORMAT()
{
    AutoPtr<ArrayOf<Int32> > int32Array = ArrayOf<Int32>::Alloc(15);

    (*int32Array)[0]  = Process::PROC_SPACE_TERM;
    (*int32Array)[1]  = Process::PROC_SPACE_TERM | Process::PROC_PARENS;
    (*int32Array)[2]  = Process::PROC_SPACE_TERM;
    (*int32Array)[3]  = Process::PROC_SPACE_TERM;
    (*int32Array)[4]  = Process::PROC_SPACE_TERM;
    (*int32Array)[5]  = Process::PROC_SPACE_TERM;
    (*int32Array)[6]  = Process::PROC_SPACE_TERM;
    (*int32Array)[7]  = Process::PROC_SPACE_TERM;
    (*int32Array)[8]  = Process::PROC_SPACE_TERM;
    (*int32Array)[9]  = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;        // 9: minor faults
    (*int32Array)[10] = Process::PROC_SPACE_TERM;
    (*int32Array)[11] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;        // 11: major faults
    (*int32Array)[12] = Process::PROC_SPACE_TERM;
    (*int32Array)[13] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;        // 13: utime
    (*int32Array)[14] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;        // 14: stime

    return int32Array;
}

AutoPtr<ArrayOf<Int32> > ProcessCpuTracker::InitPROCESS_FULL_STATS_FORMAT()
{
    AutoPtr<ArrayOf<Int32> > int32Array = ArrayOf<Int32>::Alloc(23);

    (*int32Array)[0]  = Process::PROC_SPACE_TERM;
    (*int32Array)[1]  = Process::PROC_SPACE_TERM | Process::PROC_PARENS | Process::PROC_OUT_STRING;    // 1: name
    (*int32Array)[2]  = Process::PROC_SPACE_TERM;
    (*int32Array)[3]  = Process::PROC_SPACE_TERM;
    (*int32Array)[4]  = Process::PROC_SPACE_TERM;
    (*int32Array)[5]  = Process::PROC_SPACE_TERM;
    (*int32Array)[6]  = Process::PROC_SPACE_TERM;
    (*int32Array)[7]  = Process::PROC_SPACE_TERM;
    (*int32Array)[8]  = Process::PROC_SPACE_TERM;
    (*int32Array)[9]  = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 9: minor faults
    (*int32Array)[10] = Process::PROC_SPACE_TERM;
    (*int32Array)[11] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 11: major faults
    (*int32Array)[12] = Process::PROC_SPACE_TERM;
    (*int32Array)[13] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 13: utime
    (*int32Array)[14] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 14: stime
    (*int32Array)[15] = Process::PROC_SPACE_TERM;
    (*int32Array)[16] = Process::PROC_SPACE_TERM;
    (*int32Array)[17] = Process::PROC_SPACE_TERM;
    (*int32Array)[18] = Process::PROC_SPACE_TERM;
    (*int32Array)[19] = Process::PROC_SPACE_TERM;
    (*int32Array)[20] = Process::PROC_SPACE_TERM;
    (*int32Array)[21] = Process::PROC_SPACE_TERM;
    (*int32Array)[22] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 21: vsize

    return int32Array;
}

AutoPtr<ArrayOf<Int32> > ProcessCpuTracker::InitSYSTEM_CPU_FORMAT()
{
    AutoPtr<ArrayOf<Int32> > int32Array = ArrayOf<Int32>::Alloc(8);

    (*int32Array)[0] = Process::PROC_SPACE_TERM | Process::PROC_COMBINE;
    (*int32Array)[1] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 1: user time
    (*int32Array)[2] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 2: nice time
    (*int32Array)[3] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 3: sys time
    (*int32Array)[4] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 4: idle time
    (*int32Array)[5] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 5: iowait time
    (*int32Array)[6] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 6: irq time
    (*int32Array)[7] = Process::PROC_SPACE_TERM | Process::PROC_OUT_LONG;                  // 7: softirq time

    return int32Array;
}

AutoPtr<ArrayOf<Int32> > ProcessCpuTracker::InitLOAD_AVERAGE_FORMAT()
{
    AutoPtr<ArrayOf<Int32> > int32Array = ArrayOf<Int32>::Alloc(3);

    (*int32Array)[0] = Process::PROC_SPACE_TERM | Process::PROC_OUT_FLOAT;                 // 0: 1 min
    (*int32Array)[1] = Process::PROC_SPACE_TERM | Process::PROC_OUT_FLOAT;                 // 1: 5 mins
    (*int32Array)[2] = Process::PROC_SPACE_TERM | Process::PROC_OUT_FLOAT;                 // 2: 15 mins

    return int32Array;
}

const AutoPtr<ArrayOf<Int32> > ProcessCpuTracker::PROCESS_STATS_FORMAT = InitPROCESS_STATS_FORMAT();
const AutoPtr<ArrayOf<Int32> > ProcessCpuTracker::PROCESS_FULL_STATS_FORMAT = InitPROCESS_FULL_STATS_FORMAT();
const AutoPtr<ArrayOf<Int32> > ProcessCpuTracker::SYSTEM_CPU_FORMAT = InitSYSTEM_CPU_FORMAT();
const AutoPtr<ArrayOf<Int32> > ProcessCpuTracker::LOAD_AVERAGE_FORMAT = InitLOAD_AVERAGE_FORMAT();

CAR_INTERFACE_IMPL(ProcessCpuTracker, Object, IProcessCpuTracker)

ProcessCpuTracker::ProcessCpuTracker()
    : mIncludeThreads(FALSE)
    , mLoad1(0)
    , mLoad5(0)
    , mLoad15(0)
    , mCurrentSampleTime(0)
    , mLastSampleTime(0)
    , mCurrentSampleRealTime(0)
    , mLastSampleRealTime(0)
    , mBaseUserTime(0)
    , mBaseSystemTime(0)
    , mBaseIoWaitTime(0)
    , mBaseIrqTime(0)
    , mBaseSoftIrqTime(0)
    , mBaseIdleTime(0)
    , mRelUserTime(0)
    , mRelSystemTime(0)
    , mRelIoWaitTime(0)
    , mRelIrqTime(0)
    , mRelSoftIrqTime(0)
    , mRelIdleTime(0)
    , mWorkingProcsSorted(FALSE)
    , mFirst(TRUE)
{
    CArrayList::New((IArrayList**)&mProcStats);
    CArrayList::New((IArrayList**)&mWorkingProcs);
    sLoadComparator = new Comparator();
}

ECode ProcessCpuTracker::constructor(
    /* [in] */ Boolean includeThreads)
{
    mProcessStatsData = ArrayOf<Int64>::Alloc(4);
    mSinglePidStatsData = ArrayOf<Int64>::Alloc(4);
    mProcessFullStatsStringData = ArrayOf<String>::Alloc(6);
    mProcessFullStatsData = ArrayOf<Int64>::Alloc(6);
    mSystemCpuData = ArrayOf<Int64>::Alloc(7);
    mLoadAverageData = ArrayOf<Float>::Alloc(3);
    mBuffer = ArrayOf<Byte>::Alloc(4096);
    mIncludeThreads = includeThreads;
    return NOERROR;
}

ECode ProcessCpuTracker::OnLoadChanged(
    /* [in] */ Float load1,
    /* [in] */ Float load5,
    /* [in] */ Float load15)
{
    return NOERROR;
}

ECode ProcessCpuTracker::OnMeasureProcessName(
    /* [in] */ const String& name,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    return NOERROR;
}

ECode ProcessCpuTracker::Init()
{
    //if (DEBUG) Slog.v(TAG, "Init: " + this);
    mFirst = TRUE;
    Update();
    return NOERROR;
}

ECode ProcessCpuTracker::Update()
{
    //if (DEBUG) Slog.v(TAG, "Update: " + this);
    mLastSampleTime = mCurrentSampleTime;
    mCurrentSampleTime = SystemClock::GetUptimeMillis();
    mLastSampleRealTime = mCurrentSampleRealTime;
    mCurrentSampleRealTime = SystemClock::GetElapsedRealtime();

    AutoPtr<ArrayOf<Int64> > sysCpu = mSystemCpuData;
    Boolean result;
    FAIL_RETURN(Process::ReadProcFile(String("/proc/stat"), *SYSTEM_CPU_FORMAT, NULL, sysCpu, NULL, &result));
    if (result) {
        // Total user time is user + nice time.
        const Int64 usertime = (*sysCpu)[0] + (*sysCpu)[1];
        // Total system time is simply system time.
        const Int64 systemtime = (*sysCpu)[2];
        // Total idle time is simply idle time.
        const Int64 idletime = (*sysCpu)[3];
        // Total irq time is iowait + irq + softirq time.
        const Int64 iowaittime = (*sysCpu)[4];
        const Int64 irqtime = (*sysCpu)[5];
        const Int64 softirqtime = (*sysCpu)[6];

        mRelUserTime = (Int32)(usertime - mBaseUserTime);
        mRelSystemTime = (Int32)(systemtime - mBaseSystemTime);
        mRelIoWaitTime = (Int32)(iowaittime - mBaseIoWaitTime);
        mRelIrqTime = (Int32)(irqtime - mBaseIrqTime);
        mRelSoftIrqTime = (Int32)(softirqtime - mBaseSoftIrqTime);
        mRelIdleTime = (Int32)(idletime - mBaseIdleTime);

        // if (DEBUG) {
        //     Slog.i("Load", "Total U:" + sysCpu[0] + " N:" + sysCpu[1]
        //           + " S:" + sysCpu[2] + " I:" + sysCpu[3]
        //           + " W:" + sysCpu[4] + " Q:" + sysCpu[5]
        //           + " O:" + sysCpu[6]);
        //     Slog.i("Load", "Rel U:" + mRelUserTime + " S:" + mRelSystemTime
        //           + " I:" + mRelIdleTime + " Q:" + mRelIrqTime);
        // }

        mBaseUserTime = usertime;
        mBaseSystemTime = systemtime;
        mBaseIoWaitTime = iowaittime;
        mBaseIrqTime = irqtime;
        mBaseSoftIrqTime = softirqtime;
        mBaseIdleTime = idletime;
    }

    mCurPids = CollectStats(String("/proc"), -1, mFirst, mCurPids, mProcStats);

    AutoPtr<ArrayOf<Float> > loadAverages = mLoadAverageData;
    FAIL_RETURN(Process::ReadProcFile(String("/proc/loadavg"), *LOAD_AVERAGE_FORMAT, NULL, NULL, loadAverages, &result));
    if (result) {
        Float load1 = (*loadAverages)[0];
        Float load5 = (*loadAverages)[1];
        Float load15 = (*loadAverages)[2];
        if (load1 != mLoad1 || load5 != mLoad5 || load15 != mLoad15) {
            mLoad1 = load1;
            mLoad5 = load5;
            mLoad15 = load15;
            OnLoadChanged(load1, load5, load15);
        }
    }

    // if (DEBUG) Slog.i(TAG, "*** TIME TO COLLECT STATS: "
    //         + (SystemClock.uptimeMillis()-mCurrentSampleTime));

    mWorkingProcsSorted = FALSE;
    mFirst = FALSE;
    return NOERROR;
}

AutoPtr<ArrayOf<Int32> > ProcessCpuTracker::CollectStats(
    /* [in] */ const String& statsFile,
    /* [in] */ Int32 parentPid,
    /* [in] */ Boolean first,
    /* [in] */ ArrayOf<Int32>* curPids,
    /* [in] */ IArrayList* allProcs)
{
    AutoPtr<ArrayOf<Int32> > pids;
    Process::GetPids(statsFile, curPids, (ArrayOf<Int32>**)&pids);
    Int32 NP = (pids == NULL) ? 0 : pids->GetLength();
    Int32 NS;
    allProcs->GetSize(&NS);
    Int32 curStatsIndex = 0;
    for (Int32 i = 0; i < NP; i++) {
        Int32 pid = (*pids)[i];
        if (pid < 0) {
            NP = pid;
            break;
        }
        AutoPtr<Stats> st;
        if (curStatsIndex < NS) {
            AutoPtr<IInterface> obj;
            allProcs->Get(curStatsIndex, (IInterface**)&obj);
            st = (Stats*)IProcessCpuTrackerStats::Probe(obj);
        }

        if (st != NULL && st->mPid == pid) {
            // Update an existing process...
            st->mAdded = FALSE;
            st->mWorking = FALSE;
            curStatsIndex++;
            if (DEBUG) {
                Slogger::V(TAG, "Existing %s pid %d : %s",
                        parentPid < 0 ? "process" : "thread", pid, TO_CSTR(st));
            }

            if (st->mInteresting) {
                const Int64 uptime = SystemClock::GetUptimeMillis();

                AutoPtr< ArrayOf<Int64> > procStats = mProcessStatsData;
                Boolean result;
                ECode ec = Process::ReadProcFile(st->mStatFile, *PROCESS_STATS_FORMAT, NULL, procStats, NULL, &result);
                if (FAILED(ec) || !result) {
                    continue;
                }

                const Int64 minfaults = (*procStats)[PROCESS_STAT_MINOR_FAULTS];
                const Int64 majfaults = (*procStats)[PROCESS_STAT_MAJOR_FAULTS];
                const Int64 utime = (*procStats)[PROCESS_STAT_UTIME];
                const Int64 stime = (*procStats)[PROCESS_STAT_STIME];

                if (utime == st->mBase_utime && stime == st->mBase_stime) {
                    st->mRel_utime = 0;
                    st->mRel_stime = 0;
                    st->mRel_minfaults = 0;
                    st->mRel_majfaults = 0;
                    if (st->mActive) {
                        st->mActive = FALSE;
                    }
                    continue;
                }

                if (!st->mActive) {
                    st->mActive = TRUE;
                }

                if (parentPid < 0) {
                    GetName(st, st->mCmdlineFile);
                    if (st->mThreadStats != NULL) {
                        mCurThreadPids = CollectStats(st->mThreadsDir, pid, FALSE, mCurThreadPids, st->mThreadStats);
                    }
                }

                if (DEBUG) {
                    Slogger::V("Load", "Stats changed %s pid=%d utime=%lld - %lld stime=%lld - %lld minfaults=%lld - %lld majfaults=%lld - %lld",
                        st->mName.string(), st->mPid,
                        uptime, st->mBase_utime,
                        stime, st->mBase_stime,
                        minfaults, st->mBase_minfaults,
                        majfaults, st->mBase_majfaults);
                }

                st->mRel_uptime = uptime - st->mBase_uptime;
                st->mBase_uptime = uptime;
                st->mRel_utime = (Int32)(utime - st->mBase_utime);
                st->mRel_stime = (Int32)(stime - st->mBase_stime);
                st->mBase_utime = utime;
                st->mBase_stime = stime;
                st->mRel_minfaults = (Int32)(minfaults - st->mBase_minfaults);
                st->mRel_majfaults = (Int32)(majfaults - st->mBase_majfaults);
                st->mBase_minfaults = minfaults;
                st->mBase_majfaults = majfaults;
                st->mWorking = TRUE;
            }

            continue;
        }

        if (st == NULL || st->mPid > pid) {
            // We have a new process!
            st = new Stats(pid, parentPid, mIncludeThreads);
            allProcs->Add(curStatsIndex, (IProcessCpuTrackerStats*)st);
            curStatsIndex++;
            NS++;
            if (DEBUG) {
                Slogger::V(TAG, "New %s pid %d : %s",
                    parentPid < 0 ? "process" : "thread", pid, TO_CSTR(st));
            }

            AutoPtr<ArrayOf<String> > procStatsString = mProcessFullStatsStringData;
            AutoPtr<ArrayOf<Int64> > procStats = mProcessFullStatsData;
            st->mBase_uptime = SystemClock::GetUptimeMillis();
            Boolean result;
            ECode ec = Process::ReadProcFile(st->mStatFile, *PROCESS_FULL_STATS_FORMAT, procStatsString,
                    procStats, NULL, &result);
            if (SUCCEEDED(ec) && result) {
                // This is a possible way to filter out processes that
                // are actually kernel threads...  do we want to?  Some
                // of them do use CPU, but there can be a *lot* that are
                // not doing anything.
                st->mVsize = (*procStats)[PROCESS_FULL_STAT_VSIZE];
                if (TRUE || (*procStats)[PROCESS_FULL_STAT_VSIZE] != 0) {
                    st->mInteresting = true;
                    st->mBaseName = (*procStatsString)[0];
                    st->mBase_minfaults = (*procStats)[PROCESS_FULL_STAT_MINOR_FAULTS];
                    st->mBase_majfaults = (*procStats)[PROCESS_FULL_STAT_MAJOR_FAULTS];
                    st->mBase_utime = (*procStats)[PROCESS_FULL_STAT_UTIME];
                    st->mBase_stime = (*procStats)[PROCESS_FULL_STAT_STIME];
                }
                else {
                    st->mBaseName = (*procStatsString)[0];
                    Slogger::I(TAG, "Skipping kernel process pid %d name %s",
                        pid, st->mBaseName.string());
                }
            }
            else {
                //Slog.w(TAG, "Skipping unknown process pid " + pid);
                st->mBaseName = "<unknown>";
                st->mBase_utime = st->mBase_stime = 0;
                st->mBase_minfaults = st->mBase_majfaults = 0;
            }

            if (parentPid < 0) {
                GetName(st, st->mCmdlineFile);
                if (st->mThreadStats != NULL) {
                    mCurThreadPids = CollectStats(st->mThreadsDir, pid, TRUE,
                            mCurThreadPids, st->mThreadStats);
                }
            }
            else if (st->mInteresting) {
                st->mName = st->mBaseName;
                OnMeasureProcessName(st->mName, &st->mNameWidth);
            }

            if (DEBUG) {
                Slogger::V("Load", "Stats added %s pid=%d utime=%lld stime=%lld minfaults=%lld majfaults=%lld",
                    st->mName.string(), st->mPid, st->mBase_utime,
                    st->mBase_stime, st->mBase_minfaults, st->mBase_majfaults);
            }

            st->mRel_utime = 0;
            st->mRel_stime = 0;
            st->mRel_minfaults = 0;
            st->mRel_majfaults = 0;
            st->mAdded = TRUE;
            if (!first && st->mInteresting) {
                st->mWorking = TRUE;
            }
            continue;
        }

        // This process has gone away!
        st->mRel_utime = 0;
        st->mRel_stime = 0;
        st->mRel_minfaults = 0;
        st->mRel_majfaults = 0;
        st->mRemoved = TRUE;
        st->mWorking = TRUE;
        allProcs->Remove(curStatsIndex);
        NS--;
        if (DEBUG) {
            Slogger::V(TAG, "Removed %s pid %d : %p",
                parentPid < 0 ? "process" : "thread", pid, st.Get());
        }
        // Decrement the loop counter so that we process the current pid
        // again the next time through the loop.
        i--;
        continue;
    }

    while (curStatsIndex < NS) {
        // This process has gone away!
        AutoPtr<IInterface> obj;
        allProcs->Get(curStatsIndex, (IInterface**)&obj);
        Stats* st = (Stats*)IProcessCpuTrackerStats::Probe(obj);
        st->mRel_utime = 0;
        st->mRel_stime = 0;
        st->mRel_minfaults = 0;
        st->mRel_majfaults = 0;
        st->mRemoved = true;
        st->mWorking = true;
        allProcs->Remove(curStatsIndex);
        NS--;
        if (localLOGV) {
            Slogger::V(TAG, "Removed pid %d : %s", st->mPid, TO_CSTR(st));
        }
    }

    return pids;
}

ECode ProcessCpuTracker::GetCpuTimeForPid(
    /* [in] */ Int32 pid,
    /* [out] */ Int64* _time)
{
    VALIDATE_NOT_NULL(_time);

    AutoLock lock(mSinglePidStatsDataLock);
    StringBuilder sb;
    sb += "/proc/";
    sb += pid;
    sb += "/stat";
    const String statFile = sb.ToString();
    AutoPtr<ArrayOf<Int64> > statsData = mSinglePidStatsData;
    Boolean result;
    FAIL_RETURN(Process::ReadProcFile(statFile, *PROCESS_STATS_FORMAT, NULL, statsData, NULL ,&result));
    if (result) {
        Int64 time = (*statsData)[PROCESS_STAT_UTIME]
                + (*statsData)[PROCESS_STAT_STIME];
        *_time = time;
        return NOERROR;
    }
    *_time =  0;
    return NOERROR;
}

/**
 * Returns the delta time (in clock ticks, or 1/100 sec) spent at each CPU
 * speed, since the last call to this method. If this is the first call, it
 * will return 1 for each value.
 */
ECode ProcessCpuTracker::GetLastCpuSpeedTimes(
    /* [out, callee] */ ArrayOf<Int64>** times)
{
    VALIDATE_NOT_NULL(times);

    if (mCpuSpeedTimes == NULL) {
        mCpuSpeedTimes = GetCpuSpeedTimes(NULL);
        mRelCpuSpeedTimes = ArrayOf<Int64>::Alloc(mCpuSpeedTimes->GetLength());
        for (Int32 i = 0; i < mCpuSpeedTimes->GetLength(); i++) {
            (*mRelCpuSpeedTimes)[i] = 1; // Initialize
        }
    }
    else {
        GetCpuSpeedTimes(mRelCpuSpeedTimes);
        for (Int32 i = 0; i < mCpuSpeedTimes->GetLength(); i++) {
            Int64 temp = (*mRelCpuSpeedTimes)[i];
            (*mRelCpuSpeedTimes)[i] -= (*mCpuSpeedTimes)[i];
            (*mCpuSpeedTimes)[i] = temp;
        }
    }
    *times = mRelCpuSpeedTimes;
    REFCOUNT_ADD(*times);
    return NOERROR;
}

AutoPtr<ArrayOf<Int64> > ProcessCpuTracker::GetCpuSpeedTimes(
    /* [in] */ ArrayOf<Int64>* out)
{
    AutoPtr<ArrayOf<Int64> > tempTimes = out;
    AutoPtr<ArrayOf<Int64> > tempSpeeds = mCpuSpeeds;
    const Int32 MAX_SPEEDS = 60;
    if (out == NULL) {
        tempTimes = ArrayOf<Int64>::Alloc(MAX_SPEEDS);// Hopefully no more than that
        tempSpeeds = ArrayOf<Int64>::Alloc(MAX_SPEEDS);
    }
    Int32 speed = 0;
    String file = ReadFile(String("/sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state"), '\0');
    // Note: file may be null on kernels without cpufreq (i.e. the emulator's)
    if (!file.IsNull()) {
        file = file.Replace('\n', ' ');
        AutoPtr< ArrayOf<String> > st;
        StringUtils::Split(file, String(" "), (ArrayOf<String>**)&st);
        if (st && st->GetLength() > 1) {
            for (Int32 i = 0; i < st->GetLength(); ++i) {
                String token = (*st)[i];
                //skip the iteration if the index is out of bounds
                if(speed >= tempSpeeds->GetLength()) {
                    break;
                }
                (*tempSpeeds)[speed] = StringUtils::ParseInt64(token);
                token = (*st)[++i];
                (*tempTimes)[speed] = StringUtils::ParseInt64(token);
                speed++;
                if (speed == MAX_SPEEDS) break; // No more
                if (localLOGV && out == NULL) {
                    Slogger::V(TAG, "First time : Speed/Time = %lld\t%lld",
                        (*tempSpeeds)[speed - 1], (*tempTimes)[speed - 1]);
                }
            }
        }
    }
    else {
        if(DEBUG) Slogger::I(TAG, "time_in_state file does not exist");
    }

    if (out == NULL) {
        out = ArrayOf<Int64>::Alloc(speed);
        mCpuSpeeds = ArrayOf<Int64>::Alloc(speed);
        mCpuSpeeds->Copy(tempSpeeds, 0, speed);
        out->Copy(tempTimes, 0, speed);
    }
    return out;
}

ECode ProcessCpuTracker::GetLastUserTime(
    /* [out] */ Int32* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mRelUserTime;
    return NOERROR;
}

ECode ProcessCpuTracker::GetLastSystemTime(
    /* [out] */ Int32* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mRelSystemTime;
    return NOERROR;
}

ECode ProcessCpuTracker::GetLastIoWaitTime(
    /* [out] */ Int32* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mRelIoWaitTime;
    return NOERROR;
}

ECode ProcessCpuTracker::GetLastIrqTime(
    /* [out] */ Int32* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mRelIrqTime;
    return NOERROR;
}

ECode ProcessCpuTracker::GetLastSoftIrqTime(
    /* [out] */ Int32* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mRelSoftIrqTime;
    return NOERROR;
}

ECode ProcessCpuTracker::GetLastIdleTime(
    /* [out] */ Int32* time)
{
    VALIDATE_NOT_NULL(time);
    *time = mRelIdleTime;
    return NOERROR;
}

ECode ProcessCpuTracker::GetTotalCpuPercent(
    /* [out] */ Float* percent)
{
    VALIDATE_NOT_NULL(percent);
    Int32 denom = mRelUserTime + mRelSystemTime + mRelIrqTime+mRelIdleTime;
    if (denom <= 0) {
        *percent = 0;
        return NOERROR;
    }
    *percent = ((Float)(mRelUserTime+mRelSystemTime + mRelIrqTime) * 100) / denom;
    return NOERROR;
}

void ProcessCpuTracker::BuildWorkingProcs()
{
    if (!mWorkingProcsSorted) {
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        mWorkingProcs->Clear();
        Int32 N;
        mProcStats->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IInterface> obj;
            mProcStats->Get(i, (IInterface**)&obj);
            Stats* stats = (Stats*)IProcessCpuTrackerStats::Probe(obj);
            if (stats->mWorking) {
                mWorkingProcs->Add(obj);
                Int32 M;
                if (stats->mThreadStats != NULL && (stats->mThreadStats->GetSize(&M), M > 1)) {
                    stats->mWorkingThreads->Clear();
                    for (Int32 j = 0; j < M; j++) {
                        AutoPtr<IInterface> tobj;
                        stats->mThreadStats->Get(j, (IInterface**)&tobj);
                        Stats* tstats = (Stats*)IProcessCpuTrackerStats::Probe(tobj);
                        if (tstats->mWorking) {
                            stats->mWorkingThreads->Add(tobj);
                        }
                    }
                    collections->Sort(IList::Probe(stats->mWorkingThreads), sLoadComparator);
                }
            }
        }
        collections->Sort(IList::Probe(mWorkingProcs), sLoadComparator);
        mWorkingProcsSorted = TRUE;
    }
}

ECode ProcessCpuTracker::CountStats(
    /* [out] */ Int32* stats)
{
    VALIDATE_NOT_NULL(stats);
    mProcStats->GetSize(stats);
    return NOERROR;
}

ECode ProcessCpuTracker::GetStats(
    /* [in] */ Int32 index,
    /* [out] */ IProcessCpuTrackerStats** stats)
{
    VALIDATE_NOT_NULL(stats);
    AutoPtr<IInterface> obj;
    mProcStats->Get(index, (IInterface**)&obj);
    *stats = IProcessCpuTrackerStats::Probe(obj);
    REFCOUNT_ADD(*stats);
    return NOERROR;
}

ECode ProcessCpuTracker::CountWorkingStats(
    /* [out] */ Int32* stats)
{
    VALIDATE_NOT_NULL(stats);
    BuildWorkingProcs();
    mWorkingProcs->GetSize(stats);
    return NOERROR;
}

ECode ProcessCpuTracker::GetWorkingStats(
    /* [in] */ Int32 index,
    /* [out] */ IProcessCpuTrackerStats** stats)
{
    VALIDATE_NOT_NULL(stats);
    AutoPtr<IInterface> obj;
    mWorkingProcs->Get(index, (IInterface**)&obj);
    *stats = IProcessCpuTrackerStats::Probe(obj);
    REFCOUNT_ADD(*stats);
    return NOERROR;
}

ECode ProcessCpuTracker::PrintCurrentLoad(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<IWriter> sw;
    CStringWriter::New((IWriter**)&sw);
    AutoPtr<IPrintWriter> pw;
    CFastPrintWriter::New(sw, FALSE, 128, (IPrintWriter**)&pw);
    pw->Print(String("Load: "));
    pw->Print(mLoad1);
    pw->Print(String(" / "));
    pw->Print(mLoad5);
    pw->Print(String(" / "));
    pw->Print(mLoad15);
    IStringWriter::Probe(sw)->ToString(str);
    IFlushable::Probe(pw)->Flush();
    return NOERROR;
}

ECode ProcessCpuTracker::PrintCurrentState(
    /* [in] */ Int64 now,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    BuildWorkingProcs();

    AutoPtr<IWriter> sw;
    CStringWriter::New((IWriter**)&sw);
    AutoPtr<IPrintWriter> pw;
    CFastPrintWriter::New(sw, FALSE, 1024, (IPrintWriter**)&pw);

    pw->Print(String("CPU usage from "));
    if (now > mLastSampleTime) {
        pw->Print(now-mLastSampleTime);
        pw->Print(String("ms to "));
        pw->Print(now-mCurrentSampleTime);
        pw->Print(String("ms ago"));
    }
    else {
        pw->Print(mLastSampleTime-now);
        pw->Print(String("ms to "));
        pw->Print(mCurrentSampleTime-now);
        pw->Print(String("ms later"));
    }

    Int64 sampleTime = mCurrentSampleTime - mLastSampleTime;
    Int64 sampleRealTime = mCurrentSampleRealTime - mLastSampleRealTime;
    Int64 percAwake = sampleRealTime > 0 ? ((sampleTime * 100) / sampleRealTime) : 0;
    if (percAwake != 100) {
        pw->Print(String(" with "));
        pw->Print(percAwake);
        pw->Print(String("% awake"));
    }
    pw->Println(String(":"));

    const Int32 totalTime = mRelUserTime + mRelSystemTime + mRelIoWaitTime
            + mRelIrqTime + mRelSoftIrqTime + mRelIdleTime;

    // if (DEBUG) Slog.i(TAG, "totalTime " + totalTime + " over sample time "
    //        + (mCurrentSampleTime-mLastSampleTime));

    Int32 N;
    mWorkingProcs->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<IInterface> obj;
        mWorkingProcs->Get(i, (IInterface**)&obj);
        Stats* st = (Stats*)IProcessCpuTrackerStats::Probe(obj);
        PrintProcessCPU(pw, st->mAdded ? String(" +") : (st->mRemoved ? String(" -"): String("  ")),
                st->mPid, st->mName, (Int32)(st->mRel_uptime + 5) / 10,
                st->mRel_utime, st->mRel_stime, 0, 0, 0, st->mRel_minfaults, st->mRel_majfaults);
        if (!st->mRemoved && st->mWorkingThreads != NULL) {
            Int32 M;
            st->mWorkingThreads->GetSize(&M);
            for (Int32 j = 0; j < M; j++) {
                AutoPtr<IInterface> tobj;
                st->mWorkingThreads->Get(j, (IInterface**)&tobj);
                Stats* tst = (Stats*)IProcessCpuTrackerStats::Probe(tobj);
                PrintProcessCPU(pw,
                        tst->mAdded ? String("   +") : (tst->mRemoved ? String("   -"): String("    ")),
                        tst->mPid, tst->mName, (Int32)(st->mRel_uptime + 5) / 10,
                        tst->mRel_utime, tst->mRel_stime, 0, 0, 0, 0, 0);
            }
        }
    }

    PrintProcessCPU(pw, String(""), -1, String("TOTAL"), totalTime, mRelUserTime, mRelSystemTime,
           mRelIoWaitTime, mRelIrqTime, mRelSoftIrqTime, 0, 0);

    IFlushable::Probe(pw)->Flush();
    IStringWriter::Probe(sw)->ToString(str);
    return NOERROR;
}

void ProcessCpuTracker::PrintRatio(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int64 numerator,
    /* [in] */ Int64 denominator)
{
    Int64 thousands = (numerator * 1000) / denominator;
    Int64 hundreds = thousands / 10;
    pw->Print(hundreds);
    if (hundreds < 10) {
        Int64 remainder = thousands - (hundreds * 10);
        if (remainder != 0) {
            pw->PrintChar('.');
            pw->Print(remainder);
        }
    }
}

void ProcessCpuTracker::PrintProcessCPU(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix,
    /* [in] */ Int32 pid,
    /* [in] */ const String& label,
    /* [in] */ Int32 totalTime,
    /* [in] */ Int32 user,
    /* [in] */ Int32 system,
    /* [in] */ Int32 iowait,
    /* [in] */ Int32 irq,
    /* [in] */ Int32 softIrq,
    /* [in] */ Int32 minFaults,
    /* [in] */ Int32 majFaults)
{
    pw->Print(prefix);
    if (totalTime == 0) totalTime = 1;
    PrintRatio(pw, user + system + iowait + irq + softIrq, totalTime);
    pw->Print(String("% "));
    if (pid >= 0) {
        pw->Print(pid);
        pw->Print(String("/"));
    }
    pw->Print(label);
    pw->Print(String(": "));
    PrintRatio(pw, user, totalTime);
    pw->Print(String("% user + "));
    PrintRatio(pw, system, totalTime);
    pw->Print(String("% kernel"));
    if (iowait > 0) {
        pw->Print(String(" + "));
        PrintRatio(pw, iowait, totalTime);
        pw->Print(String("% iowait"));
    }
    if (irq > 0) {
        pw->Print(String(" + "));
        PrintRatio(pw, irq, totalTime);
        pw->Print(String("% irq"));
    }
    if (softIrq > 0) {
        pw->Print(String(" + "));
        PrintRatio(pw, softIrq, totalTime);
        pw->Print(String("% softirq"));
    }
    if (minFaults > 0 || majFaults > 0) {
        pw->Print(String(" / faults:"));
        if (minFaults > 0) {
            pw->Print(String(" "));
            pw->Print(minFaults);
            pw->Print(String(" minor"));
        }
        if (majFaults > 0) {
            pw->Print(String(" "));
            pw->Print(majFaults);
            pw->Print(String(" major"));
        }
    }
    pw->Println();
}

String ProcessCpuTracker::ReadFile(
    /* [in] */ const String& file,
    /* [in] */ Char32 endChar)
{
    // Permit disk reads here, as /proc/meminfo isn't really "on
    // disk" and should be fast.  TODO: make BlockGuard ignore
    // /proc/ and /sys/ files perhaps?
    AutoPtr<IStrictMode> strictMode;
    CStrictMode::AcquireSingleton((IStrictMode**)&strictMode);
    AutoPtr<IStrictModeThreadPolicy> savedPolicy;
    strictMode->AllowThreadDiskReads((IStrictModeThreadPolicy**)&savedPolicy);

    AutoPtr<IFileInputStream> is;
    Int32 len;
    String result;
    //try {
    ECode ec = CFileInputStream::New(file, (IFileInputStream**)&is);
    FAIL_GOTO(ec, _EXIT_)
    IInputStream::Probe(is)->Read(mBuffer, &len);

    if (len > 0) {
        Int32 i;
        AutoPtr< ArrayOf<Char32> > intBuffer = ArrayOf<Char32>::Alloc(len);
        for (i = 0; i < len; i++) {
            (*intBuffer)[i] = (Char32)(*mBuffer)[i];
            if ((*mBuffer)[i] == endChar) {
                break;
            }
        }
        result = String(*intBuffer, 0, i);
    }
    //} catch (java.io.FileNotFoundException e) {
    //} catch (java.io.IOException e) {
    //} finally {
_EXIT_:
    if (is != NULL) {
        ICloseable::Probe(is)->Close();
        is = NULL;
    }
    strictMode->SetThreadPolicy(savedPolicy);
    //}
    return result;
}

void ProcessCpuTracker::GetName(
    /* [in] */ Stats* st,
    /* [in] */ const String& cmdlineFile)
{
    String newName = st->mName;
    if (st->mName.IsNull() || st->mName.Equals("app_process")
            || st->mName.Equals("<pre-initialized>")) {
        String cmdName = ReadFile(cmdlineFile, '\0');
        if (!cmdName.IsNull() && cmdName.GetLength() > 1) {
            newName = cmdName;
            Int32 i = newName.LastIndexOf("/");
            if (i > 0 && i < newName.GetLength() - 1) {
                newName = newName.Substring(i + 1);
            }
        }
        if (newName.IsNull()) {
            newName = st->mBaseName;
        }
    }
    if (st->mName.IsNull() || !newName.Equals(st->mName)) {
        st->mName = newName;
        OnMeasureProcessName(st->mName, &st->mNameWidth);
    }
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
