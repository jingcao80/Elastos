#ifndef __ELASTOS_DROID_INTERNAL_OS_PROCESSCPUTRACKER_H__
#define __ELASTOS_DROID_INTERNAL_OS_PROCESSCPUTRACKER_H__

#include <elastos/droid/ext/frameworkext.h>
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Utility::Etl::List;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class ECO_PUBLIC ProcessCpuTracker
    : public Object
    , public IProcessCpuTracker
{
public:
    class Stats
        : public Object
        , public IProcessCpuTrackerStats
    {
    public:
        Stats(
            /* [in] */ Int32 pid,
            /* [in] */ Int32 parentPid,
            /* [in] */ Boolean includeThreads);

        CAR_INTERFACE_DECL()

        CARAPI GetPid(
            /* [out] */ Int32* pid);

        CARAPI GetUid(
            /* [out] */ Int32* uid);

        CARAPI GetBatteryStats(
            /* [out] */ IBatteryStatsImplUidProc** batteryStats);

        CARAPI SetBatteryStats(
            /* [in] */ IBatteryStatsImplUidProc* batteryStats);

        CARAPI GetInteresting(
            /* [out] */ Boolean* interesting);

        CARAPI SetInteresting(
            /* [in] */ Boolean interesting);

        CARAPI GetBaseName(
            /* [out] */ String* name);

        CARAPI SetBaseName(
            /* [in] */ const String& baseName);

        CARAPI GetName(
            /* [out] */ String* name);

        CARAPI SetName(
            /* [in] */ const String& name);

        CARAPI GetNameWidth(
            /* [out] */ Int32* width);

        CARAPI SetNameWidth(
            /* [in] */ Int32 nameWidth);

        CARAPI GetVsize(
            /* [out] */ Int64* vsize);

        CARAPI SetVsize(
            /* [in] */ Int64 vsize);

        CARAPI GetBaseUptime(
            /* [out] */ Int64* time);

        CARAPI SetBaseUptime(
            /* [in] */ Int64 baseUptime);

        CARAPI GetRelUptime(
            /* [out] */ Int64* time);

        CARAPI SetRelUptime(
            /* [in] */ Int64 relUptime);

        CARAPI GetBaseUtime(
            /* [out] */ Int64* time);

        CARAPI SetBaseUtime(
            /* [in] */ Int64 baseUtime);

        CARAPI GetBaseStime(
            /* [out] */ Int64* time);

        CARAPI SetBaseStime(
            /* [in] */ Int64 baseStime);

        CARAPI GetRelUtime(
            /* [out] */ Int32* time);

        CARAPI SetRelUtime(
            /* [in] */ Int32 relUtime);

        CARAPI GetRelStime(
            /* [out] */ Int32* time);

        CARAPI SetRelStime(
            /* [in] */ Int32 relStime);

        CARAPI GetBaseMinfaults(
            /* [out] */ Int64* faults);

        CARAPI SetBaseMinfaults(
            /* [in] */ Int64 baseMinfaults);

        CARAPI GetBaseMajfaults(
            /* [out] */ Int64* faults);

        CARAPI SetBaseMajfaults(
            /* [in] */ Int64 baseMajfaults);

        CARAPI GetRelMinfaults(
            /* [out] */ Int32* faults);

        CARAPI SetRelMinfaults(
            /* [in] */ Int32 relMinfaults);

        CARAPI GetRelMajfaults(
            /* [out] */ Int32* faults);

        CARAPI SetRelMajfaults(
            /* [in] */ Int32 relMajfaults);

        CARAPI GetActive(
            /* [out] */ Boolean* active);

        CARAPI SetActive(
            /* [in] */ Boolean active);

        CARAPI GetWorking(
            /* [out] */ Boolean* working);

        CARAPI SetWorking(
            /* [in] */ Boolean working);

        CARAPI GetAdded(
            /* [out] */ Boolean* added);

        CARAPI SetAdded(
            /* [in] */ Boolean added);

        CARAPI GetRemoved(
            /* [out] */ Boolean* removed);

        CARAPI SetRemoved(
            /* [in] */ Boolean removed);

    public:
        Int32 mPid;
        Int32 mUid;
        String mStatFile;
        String mCmdlineFile;
        String mThreadsDir;
        AutoPtr< List< AutoPtr<Stats> > > mThreadStats;
        AutoPtr< List< AutoPtr<Stats> > > mWorkingThreads;

        AutoPtr<IBatteryStatsImplUidProc> mBatteryStats;

        Boolean mInteresting;

        String mBaseName;
        String mName;
        Int32 mNameWidth;

        // vsize capture when process first detected; can be used to
        // filter out kernel processes.
        Int64 mVsize;

        Int64 mBase_uptime;
        Int64 mRel_uptime;

        Int64 mBase_utime;
        Int64 mBase_stime;
        Int32 mRel_utime;
        Int32 mRel_stime;

        Int64 mBase_minfaults;
        Int64 mBase_majfaults;
        Int32 mRel_minfaults;
        Int32 mRel_majfaults;

        Boolean mActive;
        Boolean mWorking;
        Boolean mAdded;
        Boolean mRemoved;
    };

public:
    CAR_INTERFACE_DECL()

    ProcessCpuTracker();

    CARAPI constructor(
        /* [in] */ Boolean includeThreads);

    CARAPI OnLoadChanged(
        /* [in] */ Float load1,
        /* [in] */ Float load5,
        /* [in] */ Float load15);

    CARAPI OnMeasureProcessName(
        /* [in] */ const String& name,
        /* [out] */ Int32* result);

    CARAPI Init();

    CARAPI Update();

    CARAPI GetCpuTimeForPid(
        /* [in] */ Int32 pid,
        /* [out] */ Int64* times);

    /**
     * Returns the delta time (in clock ticks, or 1/100 sec) spent at each CPU
     * speed, since the last call to this method. If this is the first call, it
     * will return 1 for each value.
     */
    CARAPI GetLastCpuSpeedTimes(
        /* [out, callee] */ ArrayOf<Int64>** times);

    CARAPI GetLastUserTime(
        /* [out] */ Int32* time);

    CARAPI GetLastSystemTime(
        /* [out] */ Int32* time);

    CARAPI GetLastIoWaitTime(
        /* [out] */ Int32* time);

    CARAPI GetLastIrqTime(
        /* [out] */ Int32* time);

    CARAPI GetLastSoftIrqTime(
        /* [out] */ Int32* time);

    CARAPI GetLastIdleTime(
        /* [out] */ Int32* time);

    CARAPI GetTotalCpuPercent(
        /* [out] */ Float* percent);

    CARAPI CountStats(
        /* [out] */ Int32* size);

    CARAPI GetStats(
        /* [in] */ Int32 index,
        /* [out] */ IProcessCpuTrackerStats** stats);

    CARAPI CountWorkingStats(
        /* [out] */ Int32* size);

    CARAPI GetWorkingStats(
        /* [in] */ Int32 index,
        /* [out] */ IProcessCpuTrackerStats** stats);

    CARAPI PrintCurrentLoad(
        /* [out] */ String* str);

    CARAPI PrintCurrentState(
        /* [in] */ Int64 now,
        /* [out] */ String* str);

private:
    ECO_LOCAL static AutoPtr<ArrayOf<Int32> > InitPROCESS_STATS_FORMAT();
    ECO_LOCAL static AutoPtr<ArrayOf<Int32> > InitPROCESS_FULL_STATS_FORMAT();
    ECO_LOCAL static AutoPtr<ArrayOf<Int32> > InitSYSTEM_CPU_FORMAT();
    ECO_LOCAL static AutoPtr<ArrayOf<Int32> > InitLOAD_AVERAGE_FORMAT();

    CARAPI_(AutoPtr< ArrayOf<Int32> >) CollectStats(
        /* [in] */ const String& statsFile,
        /* [in] */ Int32 parentPid,
        /* [in] */ Boolean first,
        /* [in] */ ArrayOf<Int32>* curPids,
        /* [in] */ List< AutoPtr<Stats> >& allProcs);

    CARAPI_(AutoPtr<ArrayOf<Int64> >) GetCpuSpeedTimes(
        /* [in] */ ArrayOf<Int64>* out);

    CARAPI_(void) BuildWorkingProcs();

    CARAPI_(void) PrintRatio(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int64 numerator,
    /* [in] */ Int64 denominator);

    CARAPI_(void) PrintProcessCPU(
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
    /* [in] */ Int32 majFaults);

    CARAPI_(String) ReadFile(
    /* [in] */ const String& file,
    /* [in] */ Char32 endChar);

    CARAPI_(void) GetName(
        /* [in] */ Stats* st,
        /* [in] */ const String& cmdlineFile);

protected:
    static const Int32 PROCESS_STAT_MINOR_FAULTS = 0;
    static const Int32 PROCESS_STAT_MAJOR_FAULTS = 1;
    static const Int32 PROCESS_STAT_UTIME = 2;
    static const Int32 PROCESS_STAT_STIME = 3;

    static const Int32 PROCESS_FULL_STAT_MINOR_FAULTS = 1;
    static const Int32 PROCESS_FULL_STAT_MAJOR_FAULTS = 2;
    static const Int32 PROCESS_FULL_STAT_UTIME = 3;
    static const Int32 PROCESS_FULL_STAT_STIME = 4;
    static const Int32 PROCESS_FULL_STAT_VSIZE = 5;

private:
    ECO_LOCAL static const String TAG;
    ECO_LOCAL static const Boolean DEBUG;
    ECO_LOCAL static const Boolean localLOGV;

    ECO_LOCAL static const AutoPtr< ArrayOf<Int32> > PROCESS_STATS_FORMAT;

    /** Stores user time and system time in 100ths of a second. */
    AutoPtr< ArrayOf<Int64> > mProcessStatsData;
    /** Stores user time and system time in 100ths of a second.  Used for
     * public API to retrieve CPU use for a process.  Must lock while in use. */
    AutoPtr< ArrayOf<Int64> > mSinglePidStatsData;
    Object mSinglePidStatsDataLock;

    ECO_LOCAL static const AutoPtr< ArrayOf<Int32> > PROCESS_FULL_STATS_FORMAT;

    AutoPtr< ArrayOf<String> > mProcessFullStatsStringData;
    AutoPtr< ArrayOf<Int64> > mProcessFullStatsData;

    ECO_LOCAL static const AutoPtr< ArrayOf<Int32> >SYSTEM_CPU_FORMAT;

    AutoPtr< ArrayOf<Int64> > mSystemCpuData;

    ECO_LOCAL static const AutoPtr< ArrayOf<Int32> > LOAD_AVERAGE_FORMAT;

    AutoPtr< ArrayOf<Float> > mLoadAverageData;

    Boolean mIncludeThreads;

    Float mLoad1;
    Float mLoad5;
    Float mLoad15;

    Int64 mCurrentSampleTime;
    Int64 mLastSampleTime;

    Int64 mCurrentSampleRealTime;
    Int64 mLastSampleRealTime;

    Int64 mBaseUserTime;
    Int64 mBaseSystemTime;
    Int64 mBaseIoWaitTime;
    Int64 mBaseIrqTime;
    Int64 mBaseSoftIrqTime;
    Int64 mBaseIdleTime;
    Int32 mRelUserTime;
    Int32 mRelSystemTime;
    Int32 mRelIoWaitTime;
    Int32 mRelIrqTime;
    Int32 mRelSoftIrqTime;
    Int32 mRelIdleTime;

    AutoPtr< ArrayOf<Int32> > mCurPids;
    AutoPtr< ArrayOf<Int32> > mCurThreadPids;

    List< AutoPtr<Stats> > mProcStats;
    List< AutoPtr<Stats> > mWorkingProcs;
    Boolean mWorkingProcsSorted;

    Boolean mFirst;

    AutoPtr< ArrayOf<Byte> > mBuffer;

    /**
     * The time in microseconds that the CPU has been running at each speed.
     */
    AutoPtr< ArrayOf<Int64> > mCpuSpeedTimes;

    /**
     * The relative time in microseconds that the CPU has been running at each speed.
     */
    AutoPtr< ArrayOf<Int64> > mRelCpuSpeedTimes;

    /**
     * The different speeds that the CPU can be running at.
     */
    AutoPtr< ArrayOf<Int64> > mCpuSpeeds;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_PROCESSCPUTRACKER_H__
