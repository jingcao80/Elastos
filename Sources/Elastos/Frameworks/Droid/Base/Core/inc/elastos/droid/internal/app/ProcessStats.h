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

#ifndef __ELASTOS_DROID_INTERNAL_APP_PROCESSSTATS_H__
#define __ELASTOS_DROID_INTERNAL_APP_PROCESSSTATS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Utility::IArrayMap;
using Elastos::Core::IInteger32;
using Elastos::Core::IComparator;
using Elastos::IO::IInputStream;
using Elastos::IO::IPrintWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class ProcessStats
    : public Object
    , public IProcessStats
    , public IParcelable
{
public:
    class DurationsTable
        : public Object
        , public IDurationsTable
    {
    public:
        DurationsTable();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IProcessStats* stats,
            /* [in] */ const String& name);

        CARAPI_(void) CopyDurationsTo(
            /* [in] */ DurationsTable* other);

        CARAPI_(void) AddDurations(
            /* [in] */ DurationsTable* other);

        CARAPI_(void) ResetDurationsSafely();

        CARAPI_(void) WriteDurationsToParcel(
            /* [in] */ IParcel* out);

        CARAPI_(Boolean) ReadDurationsFromParcel(
            /* [in] */ IParcel* in);

        CARAPI_(void) AddDuration(
            /* [in] */Int32 state,
            /* [in] */ Int64 dur);

        virtual CARAPI_(Int64) GetDuration(
            /* [in] */Int32 state,
            /* [in] */ Int64 now);

        CARAPI GetStats(
            /* [out] */ IProcessStats** stats);

        CARAPI GetName(
            /* [out] */ String* name);

        CARAPI GetDurationsTable(
            /* [out, callee] */ ArrayOf<Int32>** durationsTable);

        CARAPI SetDurationsTable(
            /* [in] */ ArrayOf<Int32>* durationsTable);

        CARAPI GetDurationsTableSize(
            /* [out] */ Int32* durationsTableSize);

        CARAPI SetDurationsTableSize(
            /* [in] */ Int32 durationsTableSize);

    public:
        ProcessStats* mStats;
        String mName;
        AutoPtr<ArrayOf<Int32> > mDurationsTable;
        Int32 mDurationsTableSize;
    };

    class ProcessState
        : public DurationsTable
        , public IProcessState
    {
    public:
        ProcessState();

        CAR_INTERFACE_DECL()

        /**
         * Create a new top-level process state, for the initial case where there is only
         * a single package running in a process.  The initial state is not running.
         */
        CARAPI constructor(
            /* [in] */ IProcessStats* processStats,
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 vers,
            /* [in] */ const String& name);

        /**
         * Create a new per-package process state for an existing top-level process
         * state.  The current running state of the top-level process is also copied,
         * marked as started running at 'now'.
         */
        CARAPI constructor(
            /* [in] */ IProcessState* _commonProcess,
            /* [in] */ const String& pkg,
            /* [in] */ Int32 uid,
            /* [in] */ Int32 vers,
            /* [in] */ const String& name,
            /* [in] */ Int64 now);

        AutoPtr<ProcessState> Clone(
            /* [in] */ const String& pkg,
            /* [in] */ Int64 now);

        CARAPI_(void) Add(
             /* [in] */ ProcessState* other);

        CARAPI_(void) ResetSafely(
            /* [in] */ Int64 now);

        CARAPI_(void) MakeDead();

        CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 now);

        CARAPI_(Boolean) ReadFromParcel(
            /* [in] */ IParcel* in,
            /* [in] */ Boolean fully);

        CARAPI MakeActive();

        CARAPI MakeInactive();

        CARAPI_(Boolean) IsInUse();

        CARAPI IsInUse(
            /* [out] */ Boolean* result);

        /**
         * Update the current state of the given list of processes.
         *
         * @param state Current ActivityManager.PROCESS_STATE_*
         * @param memFactor Current mem factor constant.
         * @param now Current time.
         * @param pkgList Processes to update.
         */
        CARAPI SetState(
            /* [in] */ Int32 state,
            /* [in] */ Int32 memFactor,
            /* [in] */ Int64 now,
            /* [in] */ IArrayMap* pkgList);

        CARAPI_(void) SetState(
            /* [in] */ Int32 state,
            /* [in] */ Int64 now);

        CARAPI_(void) CommitStateTime(
            /* [in] */ Int64 now);

        CARAPI_(void) IncActiveServices(
            /* [in] */ const String& serviceName);

        CARAPI_(void) DecActiveServices(
            /* [in] */ const String& serviceName);

        CARAPI_(void) IncStartedServices(
            /* [in] */ Int32 memFactor,
            /* [in] */ Int64 now,
            /* [in] */ const String& serviceName);

        CARAPI_(void) DecStartedServices(
            /* [in] */ Int32 memFactor,
            /* [in] */ Int64 now,
            /* [in] */ const String& serviceName);

        CARAPI AddPss(
            /* [in] */ Int64 pss,
            /* [in] */ Int64 uss,
            /* [in] */ Boolean always,
            /* [in] */ IArrayMap* pkgList);

        CARAPI_(void) AddPss(
            /* [in] */ Int32 state,
            /* [in] */ Int32 inCount,
            /* [in] */ Int64 minPss,
            /* [in] */ Int64 avgPss,
            /* [in] */ Int64 maxPss,
            /* [in] */ Int64 minUss,
            /* [in] */ Int64 avgUss,
            /* [in] */ Int64 maxUss);

        CARAPI ReportExcessiveWake(
            /* [in] */ IArrayMap* pkgList);

        CARAPI ReportExcessiveCpu(
            /* [in] */ IArrayMap* pkgList);

        CARAPI ReportCachedKill(
            /* [in] */ IArrayMap* pkgList,
            /* [in] */ Int64 pss);

        CARAPI PullFixedProc(
            /* [in] */ const String& pkgName,
            /* [out] */ ProcessState** retProc);

        CARAPI_(Int64) GetDuration(
            /* [in] */ Int32 state,
            /* [in] */ Int64 now);

        CARAPI_(Int64) GetPssSampleCount(
            /* [in] */ Int32 state);

        CARAPI_(Int64) GetPssMinimum(
            /* [in] */ Int32 state);

        CARAPI_(Int64) GetPssAverage(
            /* [in] */ Int32 state);

        CARAPI_(Int64) GetPssMaximum(
            /* [in] */ Int32 state);

        CARAPI_(Int64) GetPssUssMinimum(
            /* [in] */ Int32 state);

        CARAPI_(Int64) GetPssUssAverage(
            /* [in] */ Int32 state);

        CARAPI_(Int64) GetPssUssMaximum(
            /* [in] */ Int32 state);

        CARAPI_(String) ToString();

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI GetCommonProcess(
            /* [out] */ IProcessState** commonProcess);

        CARAPI SetCommonProcess(
            /* [in] */ IProcessState* commonProcess);

        CARAPI GetPackage(
            /* [out] */ String* package);

        CARAPI GetUid(
            /* [out] */ Int32* uid);

        CARAPI GetVersion(
            /* [out] */ Int32* version);

        CARAPI GetTmpTotalTime(
            /* [out] */ Int64* tmpTotalTime);

        CARAPI SetTmpTotalTime(
            /* [in] */ Int64 tmpTotalTime);

    private:
        CARAPI_(void) EnsureNotDead();

        CARAPI_(void) AddCachedKill(
            /* [in] */ Int32 num,
            /* [in] */ Int64 minPss,
            /* [in] */ Int64 avgPss,
            /* [in] */ Int64 maxPss);

        CARAPI PullFixedProc(
            /* [in] */ IArrayMap* pkgList,
            /* [in] */ Int32 index,
            /* [out] */ ProcessState** retProc);

    public:
        ProcessState* mCommonProcess; // TODO:???
        String mPackage;
        Int32 mUid;
        Int32 mVersion;
        Int64 mTmpTotalTime;

    public: // is not public in java
        //final AutoPtr<ArrayOf<Int64> > mDurations = ArrayOf<Int64>::Alloc(STATE_COUNT * ADJ_COUN);
        Int32 mCurState;
        Int64 mStartTime;

        Int32 mLastPssState;
        Int64 mLastPssTime;
        AutoPtr<ArrayOf<Int32> > mPssTable;
        Int32 mPssTableSize;

        Boolean mActive;
        Int32 mNumActiveServices;
        Int32 mNumStartedServices;

        Int32 mNumExcessiveWake;
        Int32 mNumExcessiveCpu;

        Int32 mNumCachedKill;
        Int64 mMinCachedKillPss;
        Int64 mAvgCachedKillPss;
        Int64 mMaxCachedKillPss;

        Boolean mMultiPackage;
        Boolean mDead;

        Int32 mTmpNumInUse;
        ProcessState* mTmpFoundSubProc; // TODO:???
    };

    class ProcessStateHolder
        : public Object
        , public IProcessStateHolder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ Int32 appVersion);

        CARAPI GetAppVersion(
            /* [out] */ Int32* appVersion);

        CARAPI GetState(
            /* [out] */ IProcessState** state);

        CARAPI SetState(
            /* [in] */ IProcessState* state);

    public:
        Int32 mAppVersion;
        AutoPtr<ProcessState> mState;
    };

    class ServiceState
        : public DurationsTable
        , public IServiceState
    {
    public:
        ServiceState();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IProcessStats* processStats,
            /* [in] */ const String& pkg,
            /* [in] */ const String& name,
            /* [in] */ const String& processName,
            /* [in] */ IProcessState* proc);

        CARAPI ApplyNewOwner(
            /* [in] */ IInterface* _newOwner);

        CARAPI ClearCurrentOwner(
            /* [in] */ IInterface* _owner,
            /* [in] */ Boolean silently);

        CARAPI_(Boolean) IsInUse();

        CARAPI IsInUse(
            /* [out] */ Boolean* result);

        CARAPI IsRestarting(
            /* [out] */ Boolean* result);

        CARAPI_(void) Add(
            /* [in] */ ServiceState* other);

        CARAPI_(void) ResetSafely(
            /* [in] */ Int64 now);

        CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 now);

        CARAPI_(Boolean) ReadFromParcel(
            /* [in] */ IParcel* in);

        CARAPI_(void) CommitStateTime(
            /* [in] */ Int64 now);

        CARAPI SetStarted(
            /* [in] */ Boolean started,
            /* [in] */ Int32 memFactor,
            /* [in] */ Int64 now);

        CARAPI SetRestarting(
            /* [in] */ Boolean restarting,
            /* [in] */ Int32 memFactor,
            /* [in] */ Int64 now);

        CARAPI UpdateStartedState(
            /* [in] */ Int32 memFactor,
            /* [in] */ Int64 now);

        CARAPI SetBound(
            /* [in] */ Boolean bound,
            /* [in] */ Int32 memFactor,
            /* [in] */ Int64 now);

        CARAPI SetExecuting(
            /* [in] */ Boolean executing,
            /* [in] */ Int32 memFactor,
            /* [in] */ Int64 now);

        using DurationsTable::GetDuration;

        CARAPI_(Int64) GetDuration(
            /* [in] */ Int32 opType,
            /* [in] */ Int32 curState,
            /* [in] */ Int64 startTime,
            /* [in] */ Int32 memFactor,
            /* [in] */ Int64 now);

        CARAPI_(String) ToString();

        CARAPI ToString(
            /* [out] */ String* str);

        CARAPI GetPackage(
            /* [out] */ String* package);

        CARAPI GetProcessName(
            /* [out] */ String* processName);

        CARAPI GetRunState(
            /* [out] */ Int32* runState);

        CARAPI SetRunState(
            /* [in] */ Int32 runState);

        CARAPI GetStartedState(
            /* [out] */ Int32* startedState);

        CARAPI SetStartedState(
            /* [in] */ Int32 startedState);

        CARAPI GetBoundState(
            /* [out] */ Int32* boundState);

        CARAPI SetBoundState(
            /* [in] */ Int32 boundState);

        CARAPI GetExecState(
            /* [out] */ Int32* execState);

        CARAPI SetExecState(
            /* [in] */ Int32 execState);

    private:
        CARAPI_(void) UpdateRunning(
            /* [in] */ Int32 memFactor,
            /* [in] */ Int64 now);

    public:
        String mPackage;
        String mProcessName;
        Int32 mRunState;
        Int32 mStartedState;
        Int32 mBoundState;
        Int32 mExecState;

    public: // is not public in java
        AutoPtr<ProcessState> mProc;

        AutoPtr<IInterface> mOwner;

        static const Int32 SERVICE_COUNT = 4;

        Int32 mRunCount;
        Int64 mRunStartTime;

        Boolean mStarted;
        Boolean mRestarting;
        Int32 mStartedCount;
        Int64 mStartedStartTime;

        Int32 mBoundCount;
        Int64 mBoundStartTime;

        Int32 mExecCount;
        Int64 mExecStartTime;
    };

    class PackageState
        : public Object
        , public IPackageState
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ const String& packageName,
            /* [in] */ Int32 uid);

        CARAPI GetProcesses(
            /* [out] */ IArrayMap** processes);

        CARAPI GetServices(
            /* [out] */ IArrayMap** services);

        CARAPI GetPackageName(
            /* [out] */ String* packageName);

        CARAPI GetUid(
            /* [out] */ Int32* uid);

    public:
        AutoPtr<IArrayMap> mProcesses;
        AutoPtr<IArrayMap> mServices;
        String mPackageName;
        Int32 mUid;
    };

    class ProcessDataCollection
        : public Object
        , public IProcessDataCollection
    {
    public:
        ProcessDataCollection();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ArrayOf<Int32>* screenStates,
            /* [in] */ ArrayOf<Int32>* memStates,
            /* [in] */ ArrayOf<Int32>* procStates);

        CARAPI_(void) Print(
            /* [in] */ IPrintWriter* pw,
            /* [in] */ Int64 overallTime,
            /* [in] */ Boolean full);

        CARAPI GetTotalTime(
            /* [out] */ Int64* totalTime);

        CARAPI SetTotalTime(
            /* [in] */ Int64 totalTime);

        CARAPI GetNumPss(
            /* [out] */ Int64* numPss);

        CARAPI SetNumPss(
            /* [in] */ Int64 numPss);

        CARAPI GetMinPss(
            /* [out] */ Int64* minPss);

        CARAPI SetMinPss(
            /* [in] */ Int64 minPss);

        CARAPI GetAvgPss(
            /* [out] */ Int64* avgPss);

        CARAPI SetAvgPss(
            /* [in] */ Int64 avgPss);

        CARAPI GetMaxPss(
            /* [out] */ Int64* maxPss);

        CARAPI SetMaxPss(
            /* [in] */ Int64 maxPss);

        CARAPI GetMinUss(
            /* [out] */ Int64* minUss);

        CARAPI SetMinUss(
            /* [in] */ Int64 minUss);

        CARAPI GetAvgUss(
            /* [out] */ Int64* avgUss);

        CARAPI SetAvgUss(
            /* [in] */ Int64 avgUss);

        CARAPI GetMaxUss(
            /* [out] */ Int64* maxUss);

        CARAPI SetMaxUss(
            /* [in] */ Int64 maxUss);

    public:
        Int64 mTotalTime;
        Int64 mNumPss;
        Int64 mMinPss;
        Int64 mAvgPss;
        Int64 mMaxPss;
        Int64 mMinUss;
        Int64 mAvgUss;
        Int64 mMaxUss;

    public: // is not public in java
        AutoPtr<ArrayOf<Int32> > mScreenStates;
        AutoPtr<ArrayOf<Int32> > mMemStates;
        AutoPtr<ArrayOf<Int32> > mProcStates;
    };

    class TotalMemoryUseCollection
        : public Object
        , public ITotalMemoryUseCollection
    {
    public:
        TotalMemoryUseCollection();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ ArrayOf<Int32>* screenStates,
            /* [in] */ ArrayOf<Int32>* memStates);

        CARAPI GetTotalTime(
            /* [out] */ Int64* totalTime);

        CARAPI SetTotalTime(
            /* [in] */ Int64 totalTime);

        CARAPI GetProcessStatePss(
            /* [out, callee] */ ArrayOf<Int64>** processStatePss);

        CARAPI SetProcessStatePss(
            /* [in] */ ArrayOf<Int64>* processStatePss);

        CARAPI GetProcessStateWeight(
            /* [out, callee] */ ArrayOf<Double>** processStateWeight);

        CARAPI SetProcessStateWeight(
            /* [in] */ ArrayOf<Double>* processStateWeight);

        CARAPI GetProcessStateTime(
            /* [out, callee] */ ArrayOf<Int64>** processStateTime);

        CARAPI SetProcessStateTime(
            /* [in] */ ArrayOf<Int64>* processStateTime);

        CARAPI GetProcessStateSamples(
            /* [out, callee] */ ArrayOf<Int32>** processStateSamples);

        CARAPI SetProcessStateSamples(
            /* [in] */ ArrayOf<Int32>* processStateSamples);

        CARAPI GetSysMemUsage(
            /* [out, callee] */ ArrayOf<Int64>** sysMemUsage);

        CARAPI SetSysMemUsage(
            /* [in] */ ArrayOf<Int64>* sysMemUsage);

        CARAPI GetSysMemCachedWeight(
            /* [out] */ Double* sysMemCachedWeight);

        CARAPI SetSysMemCachedWeight(
            /* [in] */ Double sysMemCachedWeight);

        CARAPI GetSysMemFreeWeight(
            /* [out] */ Double* sysMemFreeWeight);

        CARAPI SetSysMemFreeWeight(
            /* [in] */ Double sysMemFreeWeight);

        CARAPI GetSysMemZRamWeight(
            /* [out] */ Double* sysMemZRamWeight);

        CARAPI SetSysMemZRamWeight(
            /* [in] */ Double sysMemZRamWeight);

        CARAPI GetSysMemKernelWeight(
            /* [out] */ Double* sysMemKernelWeight);

        CARAPI SetSysMemKernelWeight(
            /* [in] */ Double sysMemKernelWeight);

        CARAPI GetSysMemNativeWeight(
            /* [out] */ Double* sysMemNativeWeight);

        CARAPI SetSysMemNativeWeight(
            /* [in] */ Double sysMemNativeWeight);

        CARAPI GetSysMemSamples(
            /* [out] */ Int32* sysMemSamples);

        CARAPI SetSysMemSamples(
            /* [in] */ Int32 sysMemSamples);

    public:
        Int64 mTotalTime;
        AutoPtr<ArrayOf<Int64> > mProcessStatePss;
        AutoPtr<ArrayOf<Double> > mProcessStateWeight;
        AutoPtr<ArrayOf<Int64> > mProcessStateTime;
        AutoPtr<ArrayOf<Int32> > mProcessStateSamples;
        AutoPtr<ArrayOf<Int64> > mSysMemUsage;
        Double mSysMemCachedWeight;
        Double mSysMemFreeWeight;
        Double mSysMemZRamWeight;
        Double mSysMemKernelWeight;
        Double mSysMemNativeWeight;
        Int32 mSysMemSamples;

    public: // is not public in java
        AutoPtr<ArrayOf<Int32> > mScreenStates;
        AutoPtr<ArrayOf<Int32> > mMemStates;
    };

public: // not public in java
    class PssAggr : public Object
    {
    public:
        PssAggr();

        CARAPI_(void) Add(
            /* [in] */ Int64 newPss,
            /* [in] */ Int64 newSamples);

    public:
        Int64 mPss;
        Int64 mSamples;
    };

private:
    class ProcessStateComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */ IInterface* _lhs,
            /* [in] */ IInterface* _rhs,
            /* [out] */ Int32* result);
    };

public:
    CAR_INTERFACE_DECL()

    ProcessStats();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Boolean running);

    CARAPI Add(
        /* [in] */ IProcessStats* _other);

    CARAPI AddSysMemUsage(
        /* [in] */ Int64 cachedMem,
        /* [in] */ Int64 freeMem,
        /* [in] */ Int64 zramMem,
        /* [in] */ Int64 kernelMem,
        /* [in] */ Int64 nativeMem);

    CARAPI_(void) AddSysMemUsage(
        /* [in] */ Int32 state,
        /* [in] */ const ArrayOf<Int64>& data,
        /* [in] */ Int32 dataOff);

    CARAPI ComputeTotalMemoryUse(
        /* [in] */ ITotalMemoryUseCollection* _data,
        /* [in] */ Int64 now);

    CARAPI_(Int64) GetSysMemUsageValue(
        /* [in] */ Int32 state,
        /* [in] */ Int32 index);

    CARAPI_(void) DumpSysMemUsageCategory(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ const String& label,
        /* [in] */ Int32 bucket,
        /* [in] */ Int32 index);

    CARAPI_(void) DumpSysMemUsage(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ const ArrayOf<Int32>& screenStates,
        /* [in] */ const ArrayOf<Int32>& memStates);

    CARAPI Reset();

    CARAPI ResetSafely();

    CARAPI EvaluateSystemProperties(
        /* [in] */ Boolean update,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    /** @hide */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* out,
        /* [in] */ Int64 now);

    CARAPI Read(
        /* [in] */ IInputStream* stream);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI_(Int32) AddInt64Data(
        /* [in] */ Int32 index,
        /* [in] */ Int32 type,
        /* [in] */ Int32 num);

    CARAPI_(Int32) AllocInt64Data(
        /* [in] */ Int32 num);

    CARAPI_(Boolean) ValidateInt64Offset(
        /* [in] */ Int32 off);

    CARAPI_(void) SetInt64(
        /* [in] */ Int32 off,
        /* [in] */ Int32 index,
        /* [in] */ Int64 value);

    CARAPI_(Int64) GetInt64(
        /* [in] */ Int32 off,
        /* [in] */ Int32 index);

    CARAPI GetPackageStateLocked(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 vers,
        /* [out] */ IPackageState** pkgState);

    CARAPI GetProcessStateLocked(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 vers,
        /* [in] */ const String& processName,
        /* [out] */ IProcessState** procState);

    CARAPI GetServiceStateLocked(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 vers,
        /* [in] */ const String& processName,
        /* [in] */ const String& className,
        /* [out] */ IServiceState** svcState);

    CARAPI DumpLocked(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& reqPackage,
        /* [in] */ Int64 now,
        /* [in] */ Boolean dumpSummary,
        /* [in] */ Boolean dumpAll,
        /* [in] */ Boolean activeOnly);

    CARAPI_(void) DumpServiceStats(
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
        /* [in] */ Boolean dumpAll);

    CARAPI DumpSummaryLocked(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& reqPackage,
        /* [in] */ Int64 now,
        /* [in] */ Boolean activeOnly);

    CARAPI_(Int64) PrintMemoryCategory(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ const String& label,
        /* [in] */ Double memWeight,
        /* [in] */ Int64 totalTime,
        /* [in] */ Int64 curTotalMem,
        /* [in] */ Int32 samples);

    CARAPI_(void) DumpTotalsLocked(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int64 now);

    CARAPI_(void) DumpFilteredSummaryLocked(
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
        /* [in] */ Boolean activeOnly);

    CARAPI CollectProcessesLocked(
        /* [in] */ ArrayOf<Int32>* screenStates,
        /* [in] */ ArrayOf<Int32>* memStates,
        /* [in] */ ArrayOf<Int32>* procStates,
        /* [in] */ ArrayOf<Int32>* sortProcStates,
        /* [in] */ Int64 now,
        /* [in] */ const String& reqPackage,
        /* [in] */ Boolean activeOnly,
        /* [out] */ IArrayList** list);

    CARAPI_(String) CollapseString(
        /* [in] */ const String& pkgName,
        /* [in] */ const String& itemName);

    CARAPI DumpCheckinLocked(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& reqPackage);

    CARAPI GetReadError(
        /* [out] */ String* readError);

    CARAPI SetReadError(
        /* [in] */ const String& readError);

    CARAPI GetTimePeriodStartClockStr(
        /* [out] */ String* timePeriodStartClockStr);

    CARAPI SetTimePeriodStartClockStr(
        /* [in] */ const String& timePeriodStartClockStr);

    CARAPI GetFlags(
        /* [out] */ Int32* flags);

    CARAPI SetFlags(
        /* [in] */ Int32 flags);

    CARAPI GetPackages(
        /* [out] */ IProcessMap** packages);

    CARAPI GetProcesses(
        /* [out] */ IProcessMap** processes);

    CARAPI GetMemFactorDurations(
        /* [out, callee] */ ArrayOf<Int64>** memFactorDurations);

    CARAPI GetMemFactor(
        /* [out] */ Int32* memFactor);

    CARAPI SetMemFactor(
        /* [in] */ Int32 memFactor);

    CARAPI GetStartTime(
        /* [out] */ Int64* startTime);

    CARAPI SetStartTime(
        /* [in] */ Int64 startTime);

    CARAPI GetSysMemUsageTable(
        /* [out, callee] */ ArrayOf<Int32>** sysMemUsageTable);

    CARAPI SetSysMemUsageTable(
        /* [in] */ ArrayOf<Int32>* sysMemUsageTable);

    CARAPI GetSysMemUsageTableSize(
        /* [out] */ Int32* sysMemUsageTableSize);

    CARAPI SetSysMemUsageTableSize(
        /* [in] */ Int32 sysMemUsageTableSize);

    CARAPI GetSysMemUsageArgs(
        /* [out, callee] */ ArrayOf<Int64>** sysMemUsageArgs);

    CARAPI GetTimePeriodStartClock(
        /* [out] */ Int64* timePeriodStartClock);

    CARAPI SetTimePeriodStartClock(
        /* [in] */ Int64 timePeriodStartClock);

    CARAPI GetTimePeriodStartRealtime(
        /* [out] */ Int64* timePeriodStartRealtime);

    CARAPI SetTimePeriodStartRealtime(
        /* [in] */ Int64 timePeriodStartRealtime);

    CARAPI GetTimePeriodEndRealtime(
        /* [out] */ Int64* timePeriodEndRealtime);

    CARAPI SetTimePeriodEndRealtime(
        /* [in] */ Int64 timePeriodEndRealtime);

    CARAPI GetTimePeriodStartUptime(
        /* [out] */ Int64* timePeriodStartUptime);

    CARAPI SetTimePeriodStartUptime(
        /* [in] */ Int64 timePeriodStartUptime);

    CARAPI GetTimePeriodEndUptime(
        /* [out] */ Int64* timePeriodEndUptime);

    CARAPI SetTimePeriodEndUptime(
        /* [in] */ Int64 timePeriodEndUptime);

public:
    static CARAPI_(void) AddSysMemUsage(
        /* [in] */ ArrayOf<Int64>& dstData,
        /* [in] */ Int32 dstOff,
        /* [in] */ const ArrayOf<Int64>& addData,
        /* [in] */ Int32 addOff);

    static CARAPI_(void) PrintScreenLabelCsv(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 offset);

    static CARAPI_(void) PrintMemLabelCsv(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 offset);

    static CARAPI_(Int64) DumpSingleTime(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ const ArrayOf<Int64>& durations,
        /* [in] */ Int32 curState,
        /* [in] */ Int64 curStartTime,
        /* [in] */ Int64 now);

    static CARAPI_(void) DumpAdjTimesCheckin(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& sep,
        /* [in] */ const ArrayOf<Int64>& durations,
        /* [in] */ Int32 curState,
        /* [in] */ Int64 curStartTime,
        /* [in] */ Int64 now);

    static CARAPI_(void) DumpServiceTimeCheckin(
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
        /* [in] */ Int64 now);

    static CARAPI ComputeProcessData(
        /* [in] */ IProcessState* _proc,
        /* [in] */ IProcessDataCollection* _data,
        /* [in] */ Int64 now);

    static CARAPI_(Int64) ComputeProcessTimeLocked(
        /* [in] */ ProcessState* proc,
        /* [in] */ const ArrayOf<Int32>& screenStates,
        /* [in] */ const ArrayOf<Int32>& memStates,
        /* [in] */ const ArrayOf<Int32>& procStates,
        /* [in] */ Int64 now);

    static CARAPI_(void) DumpProcessState(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ ProcessState* proc,
        /* [in] */ const ArrayOf<Int32>& screenStates,
        /* [in] */ const ArrayOf<Int32>& memStates,
        /* [in] */ const ArrayOf<Int32>& procStates,
        /* [in] */ Int64 now);

    static CARAPI_(void) DumpProcessPss(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ ProcessState* proc,
        /* [in] */ const ArrayOf<Int32>& screenStates,
        /* [in] */ const ArrayOf<Int32>& memStates,
        /* [in] */ const ArrayOf<Int32>& procStates);

    static CARAPI_(void) DumpStateHeadersCsv(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& sep,
        /* [in] */ ArrayOf<Int32>* screenStates,
        /* [in] */ ArrayOf<Int32>* memStates,
        /* [in] */ ArrayOf<Int32>* procStates);

    static CARAPI_(void) DumpProcessStateCsv(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ProcessState* proc,
        /* [in] */ Boolean sepScreenStates,
        /* [in] */ ArrayOf<Int32>* screenStates,
        /* [in] */ Boolean sepMemStates,
        /* [in] */ ArrayOf<Int32>* memStates,
        /* [in] */ Boolean sepProcStates,
        /* [in] */ ArrayOf<Int32>* procStates,
        /* [in] */ Int64 now);

    static CARAPI_(void) DumpProcessList(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ IArrayList* procs,
        /* [in] */ const ArrayOf<Int32>& screenStates,
        /* [in] */ const ArrayOf<Int32>& memStates,
        /* [in] */ const ArrayOf<Int32>& procStates,
        /* [in] */ Int64 now);

    static CARAPI_(void) DumpProcessSummaryDetails(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IProcessState* proc,
        /* [in] */ const String& prefix,
        /* [in] */ const String& label,
        /* [in] */ ArrayOf<Int32>* screenStates,
        /* [in] */ ArrayOf<Int32>* memStates,
        /* [in] */ ArrayOf<Int32>* procStates,
        /* [in] */ Int64 now,
        /* [in] */ Int64 totalTime,
        /* [in] */ Boolean full);

    static CARAPI_(void) DumpProcessSummaryLocked(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ IArrayList* procs,
        /* [in] */ ArrayOf<Int32>* screenStates,
        /* [in] */ ArrayOf<Int32>* memStates,
        /* [in] */ ArrayOf<Int32>* procStates,
        /* [in] */ Boolean inclUidVers,
        /* [in] */ Int64 now,
        /* [in] */ Int64 totalTime);

    static CARAPI_(void) PrintPercent(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Double fraction);

    static CARAPI_(void) PrintSizeValue(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int64 number);

    static CARAPI DumpProcessListCsv(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ IArrayList* procs,
        /* [in] */ Boolean sepScreenStates,
        /* [in] */ ArrayOf<Int32>* screenStates,
        /* [in] */ Boolean sepMemStates,
        /* [in] */ ArrayOf<Int32>* memStates,
        /* [in] */ Boolean sepProcStates,
        /* [in] */ ArrayOf<Int32>* procStates,
        /* [in] */ Int64 now);

    static CARAPI_(Int32) PrintArrayEntry(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const ArrayOf<String>& array,
        /* [in] */ Int32 value,
        /* [in] */ Int32 mod);

    static CARAPI_(void) PrintProcStateTag(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 state);

    static CARAPI_(void) PrintAdjTag(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 state);

    static CARAPI_(void) PrintProcStateTagAndValue(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 state,
        /* [in] */ Int64 value);

    static CARAPI_(void) PrintAdjTagAndValue(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 state,
        /* [in] */ Int64 value);

    static CARAPI_(void) DumpAllProcessStateCheckin(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ProcessState* proc,
        /* [in] */ Int64 now);

    static CARAPI_(void) DumpAllProcessPssCheckin(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ProcessState* proc);

    static CARAPI ReadFully(
        /* [in] */ IInputStream* stream,
        /* [in] */ ArrayOf<Int32>* outLen,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    static CARAPI_(String) PrintInt64Offset(
        /* [in] */ Int32 off);

    static CARAPI_(Int32) BinarySearch(
        /* [in] */ const ArrayOf<Int32>& array,
        /* [in] */ Int32 size,
        /* [in] */ Int32 value);

    static CARAPI_(Int64) DumpSingleServiceTime(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ IServiceState* service,
        /* [in] */ Int32 serviceType,
        /* [in] */ Int32 curState,
        /* [in] */ Int64 curStartTime,
        /* [in] */ Int64 now);

private:
    CARAPI_(void) ResetCommon();

    CARAPI_(void) BuildTimePeriodStartClockStr();

    CARAPI_(AutoPtr<ArrayOf<Int32> >) ReadTableFromParcel(
        /* [in] */ IParcel* in,
        /* [in] */ const String& name,
        /* [in] */ const String& what);

    CARAPI_(void) WriteCompactedInt64Array(
        /* [in] */ IParcel* out,
        /* [in] */ const ArrayOf<Int64>& array,
        /* [in] */ Int32 num);

    CARAPI ReadCompactedInt64Array(
        /* [in] */ IParcel* in,
        /* [in] */ Int32 version,
        /* [in] */ ArrayOf<Int64>* array,
        /* [in] */ Int32 num);

    CARAPI_(void) WriteCommonString(
        /* [in] */ IParcel* out,
        /* [in] */ const String& name);

    CARAPI_(String) ReadCommonString(
        /* [in] */ IParcel* in,
        /* [in] */ Int32 version);

    CARAPI_(Boolean) ReadCheckedInt32(
        /* [in] */ IParcel* in,
        /* [in] */ Int32 val,
        /* [in] */ const String& what);

    CARAPI_(void) DumpProcessInternalLocked(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ const String& prefix,
        /* [in] */ ProcessState* proc,
        /* [in] */ Boolean dumpAll);

private:
    static CARAPI_(AutoPtr<ArrayOf<Int32> >) InitALL_MEM_ADJ();

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) InitALL_SCREEN_ADJ();

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) InitNON_CACHED_PROC_STATES();

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) InitBACKGROUND_PROC_STATES();

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) InitPROCESS_STATE_TO_STATE();

    static CARAPI_(AutoPtr<ArrayOf<Int32> >) InitALL_PROC_STATES();

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitSTATE_NAMES();

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitADJ_SCREEN_NAMES_CSV();

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitADJ_MEM_NAMES_CSV();

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitSTATE_NAMES_CSV();

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitADJ_SCREEN_TAGS();

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitADJ_MEM_TAGS();

    static CARAPI_(AutoPtr<ArrayOf<String> >) InitSTATE_TAGS();

    static CARAPI_(void) PrintScreenLabel(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 offset);

    static CARAPI_(void) PrintMemLabel(
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 offset,
        /* [in] */ Char32 sep);

public:
    static const AutoPtr<ArrayOf<Int32> > ALL_MEM_ADJ;
    static const AutoPtr<ArrayOf<Int32> > ALL_SCREEN_ADJ;
    static const AutoPtr<ArrayOf<Int32> > NON_CACHED_PROC_STATES;
    static const AutoPtr<ArrayOf<Int32> > BACKGROUND_PROC_STATES;
    static const AutoPtr<ArrayOf<Int32> > ALL_PROC_STATES;
    static const AutoPtr<ArrayOf<String> > ADJ_SCREEN_NAMES_CSV;
    static const AutoPtr<ArrayOf<String> > ADJ_MEM_NAMES_CSV;
    static const AutoPtr<ArrayOf<String> > STATE_NAMES_CSV;

public: // not public in java
    static const String TAG;
    static const Boolean DEBUG = FALSE;
    static const Boolean DEBUG_PARCEL = FALSE;

    // How often the service commits its data, giving the minimum batching
    // that is done.
    static Int64 COMMIT_PERIOD;  // Commit current stats every 3 hours

    // Minimum uptime period before committing.  If the COMMIT_PERIOD has elapsed but
    // the total uptime has not exceeded this amount, then the commit will be held until
    // it is reached.
    static Int64 COMMIT_UPTIME_PERIOD;  // Must have at least 1 hour elapsed

    // Map from process states to the states we track.
    static const AutoPtr<ArrayOf<Int32> > PROCESS_STATE_TO_STATE ;
    static const AutoPtr<ArrayOf<String> > STATE_NAMES;
    static const AutoPtr<ArrayOf<String> > ADJ_SCREEN_TAGS;
    static const AutoPtr<ArrayOf<String> > ADJ_MEM_TAGS;
    static const AutoPtr<ArrayOf<String> > STATE_TAGS;
    static const String CSV_SEP ;

    // Where the "type"/"state" part of the data appears in an offset integer.
    static Int32 OFFSET_TYPE_SHIFT;
    static Int32 OFFSET_TYPE_MASK;
    // Where the "which array" part of the data appears in an offset integer.
    static Int32 OFFSET_ARRAY_SHIFT;
    static Int32 OFFSET_ARRAY_MASK;
    // Where the "index into array" part of the data appears in an offset integer.
    static Int32 OFFSET_INDEX_SHIFT;
    static Int32 OFFSET_INDEX_MASK;

    static const Int32 LONGS_SIZE = 4096;

    static const AutoPtr<ArrayOf<Int32> > BAD_TABLE;

private:
    // Current version of the parcel format.
    static const Int32 PARCEL_VERSION = 18;
    // In-memory Parcel magic number, used to detect attempts to unmarshall bad data
    static const Int32 MAGIC = 0x50535453;

public:
    String mReadError;
    String mTimePeriodStartClockStr;
    Int32 mFlags;

    AutoPtr<IProcessMap> mPackages;
    AutoPtr<IProcessMap> mProcesses;

    AutoPtr<ArrayOf<Int64> > mMemFactorDurations;
    Int32 mMemFactor;
    Int64 mStartTime;

    AutoPtr<ArrayOf<Int32> > mSysMemUsageTable;
    Int32 mSysMemUsageTableSize;
    AutoPtr<ArrayOf<Int64> > mSysMemUsageArgs;

    Int64 mTimePeriodStartClock;
    Int64 mTimePeriodStartRealtime;
    Int64 mTimePeriodEndRealtime;
    Int64 mTimePeriodStartUptime;
    Int64 mTimePeriodEndUptime;

public: // not public in java
    String mRuntime;
    Boolean mRunning;

    List<AutoPtr<ArrayOf<Int64> > > mInt64s;
    Int32 mNextInt64;

    AutoPtr<ArrayOf<Int32> > mAddInt64Table;
    Int32 mAddInt64TableSize;

    // For writing parcels.
    AutoPtr<HashMap<String, AutoPtr<IInteger32> > > mCommonStringToIndex;

    // For reading parcels.
    AutoPtr<List<String> > mIndexToCommonString;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_PROCESSSTATS_H__
