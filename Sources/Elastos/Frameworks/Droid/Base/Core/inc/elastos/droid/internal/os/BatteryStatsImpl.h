
#ifndef __ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSIMPL_H__
#define __ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/BatteryStats.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Net::INetworkStats;
using Elastos::Droid::Net::INetworkStatsEntry;
using Elastos::Droid::Internal::Net::INetworkStatsFactory;
using Elastos::Droid::Internal::Utility::IJournaledFile;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Utility::IPrinter;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Core::IInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::IO::IFileInputStream;
using Elastos::Utility::IMap;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

/**
 * All information we are collecting about things that can happen that impact
 * battery life.  All times are represented in microseconds except where indicated
 * otherwise.
 */
class BatteryStatsImpl
    : public BatteryStats
    , public IBatteryStatsImpl
    , public IParcelable
{
public:
    class MyHandler : public Handler
    {
    public:
        TO_STRING_IMPL("BatteryStatsImpl::MyHandler")

        MyHandler(
            /* [in] */ ILooper* looper,
            /* [in] */ BatteryStatsImpl* host)
            : Handler(looper, NULL, TRUE)
            , mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        BatteryStatsImpl* mHost;
    };

    class TimeBase : public Object
    {
    public:
        TimeBase();

        CARAPI_(void) Dump(
            /* [in] */ IPrintWriter* pw,
            /* [in] */ const String& prefix);

        CARAPI_(void) Add(
            /* [in] */ ITimeBaseObs* observer);

        CARAPI_(void) Remove(
            /* [in] */ ITimeBaseObs* observer);

        CARAPI_(void) Init(
            /* [in] */ Int64 uptime,
            /* [in] */ Int64 realtime);

        CARAPI_(void) Reset(
            /* [in] */ Int64 uptime,
            /* [in] */ Int64 realtime);

        CARAPI_(Int64) ComputeUptime(
            /* [in] */ Int64 curTime,
            /* [in] */ Int32 which);

        CARAPI_(Int64) ComputeRealtime(
            /* [in] */ Int64 curTime,
            /* [in] */ Int32 which);

        CARAPI_(Int64) GetUptime(
            /* [in] */ Int64 curTime);

        CARAPI_(Int64) GetRealtime(
            /* [in] */ Int64 curTime);

        CARAPI_(Int64) GetUptimeStart();

        CARAPI_(Int64) GetRealtimeStart();

        CARAPI_(Boolean) IsRunning();

        CARAPI_(Boolean) SetRunning(
            /* [in] */ Boolean running,
            /* [in] */ Int64 uptime,
            /* [in] */ Int64 realtime);

        CARAPI_(void) ReadSummaryFromParcel(
            /* [in] */ IParcel* in);

        CARAPI_(void) WriteSummaryToParcel(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 uptime,
            /* [in] */ Int64 realtime);

        CARAPI_(void) ReadFromParcel(
            /* [in] */ IParcel* in);

        CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 uptime,
            /* [in] */ Int64 realtime);

    private:
        List<AutoPtr<ITimeBaseObs> > mObservers;

        Int64 mUptime;
        Int64 mRealtime;

        Boolean mRunning;

        Int64 mPastUptime;
        Int64 mUptimeStart;
        Int64 mPastRealtime;
        Int64 mRealtimeStart;
        Int64 mUnpluggedUptime;
        Int64 mUnpluggedRealtime;
    };

    /**
     * State for keeping track of counting information.
     */
    class Counter
        : public Object
        , public IBatteryStatsCounter
        , public ITimeBaseObs
    {
    public:
        Counter(
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        Counter(
            /* [in] */ TimeBase* timeBase);

        virtual ~Counter() {}

        CAR_INTERFACE_DECL()

        CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* out);

        CARAPI OnTimeStarted(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        CARAPI OnTimeStopped(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        /**
         * Writes a possibly null Counter to a Parcel.
         *
         * @param out the Parcel to be written to.
         * @param counter a Counter, or null.
         */
        static CARAPI_(void) WriteCounterToParcel(
            /* [in] */ IParcel* out,
            /* [in] */ Counter* counter);

        CARAPI GetCountLocked(
            /* [in] */ Int32 which,
            /* [out] */ Int32* count);

        CARAPI LogState(
            /* [in] */ IPrinter* pw,
            /* [in] */ const String& prefix);

    private:
        CARAPI_(void) StepAtomic();

        CARAPI_(void) Reset(
            /* [in] */ Boolean detachIfReset);

        CARAPI_(void) Detach();

        CARAPI_(void) WriteSummaryFromParcelLocked(
            /* [in] */ IParcel* out);

        CARAPI_(void) ReadSummaryFromParcelLocked(
            /* [in] */ IParcel* in);

    protected:
        AutoPtr<IAtomicInteger32> mCount;
        AutoPtr<TimeBase> mTimeBase;
        Int32 mLoadedCount;
        Int32 mLastCount;
        Int32 mUnpluggedCount;
        Int32 mPluggedCount;

        friend class BatteryStatsImpl;
    };

    class SamplingCounter : public Counter
    {
    public:
        SamplingCounter(
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        SamplingCounter(
            /* [in] */ TimeBase* timeBase);

        CARAPI_(void) AddCountAtomic(
            /* [in] */ Int64 count);
    };

    class Int64SamplingCounter
        : public Object
        , public IBatteryStatsInt64Counter
        , public ITimeBaseObs
    {
    public:
        Int64SamplingCounter(
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        Int64SamplingCounter(
            /* [in] */ TimeBase* timeBase);

        CAR_INTERFACE_DECL()

        CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* out);

        // @Override
        CARAPI OnTimeStarted(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        // @Override
        CARAPI OnTimeStopped(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        CARAPI GetCountLocked(
            /* [in] */ Int32 which,
            /* [out] */ Int64* count);

        // @Override
        CARAPI LogState(
            /* [in] */ IPrinter* pw,
            /* [in] */ const String& prefix);

        CARAPI_(void) AddCountLocked(
            /* [in] */ Int64 count);

        /**
         * Clear state of this counter.
         */
        CARAPI_(void) Reset(
            /* [in] */ Boolean detachIfReset);

        CARAPI_(void) Detach();

        CARAPI_(void) WriteSummaryFromParcelLocked(
            /* [in] */ IParcel* out);

        CARAPI_(void) ReadSummaryFromParcelLocked(
            /* [in] */ IParcel* in);

    public:
        AutoPtr<TimeBase> mTimeBase;
        Int64 mCount;
        Int64 mLoadedCount;
        Int64 mLastCount;
        Int64 mUnpluggedCount;
        Int64 mPluggedCount;
    };

    /**
     * State for keeping track of timing information.
     */
    class Timer
        : public Object
        , public IBatteryStatsTimer
        , public ITimeBaseObs
    {
    public:
        /**
         * Constructs from a parcel.
         * @param type
         * @param timeBase
         * @param in
         */
        Timer(
            /* [in] */ Int32 type,
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        Timer(
            /* [in] */ Int32 type,
            /* [in] */ TimeBase* timeBase);

        virtual ~Timer() {}

        CAR_INTERFACE_DECL()

        CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 elapsedRealtimeUs);

        // @Override
        CARAPI OnTimeStarted(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        // @Override
        CARAPI OnTimeStopped(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        /**
         * Writes a possibly null Timer to a Parcel.
         *
         * @param out the Parcel to be written to.
         * @param timer a Timer, or null.
         */
        static CARAPI_(void) WriteTimerToParcel(
            /* [in] */ IParcel* out,
            /* [in] */ Timer* timer,
            /* [in] */ Int64 elapsedRealtimeUs);

        CARAPI GetTotalTimeLocked(
            /* [in] */ Int64 elapsedRealtimeUs,
            /* [in] */ Int32 which,
            /* [out] */ Int64* time);

        CARAPI GetCountLocked(
            /* [in] */ Int32 which,
            /* [out] */ Int64* count);

        CARAPI LogState(
            /* [in] */ IPrinter* pw,
            /* [in] */ const String& prefix);

    protected:
        virtual CARAPI_(Int64) ComputeRunTimeLocked(
            /* [in] */ Int64 curBatteryRealtime) = 0;

        virtual CARAPI_(Int32) ComputeCurrentCountLocked() = 0;

        /**
         * Clear state of this timer.  Returns true if the timer is inactive
         * so can be completely dropped.
         */
        CARAPI_(Boolean) Reset(
            /* [in] */ Boolean detachIfReset);

        CARAPI_(void) Detach();

        CARAPI_(void) WriteSummaryFromParcelLocked(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 elapsedRealtimeUs);

        CARAPI_(void) ReadSummaryFromParcelLocked(
            /* [in] */ IParcel* in);

    protected:
        Int32 mType;
        AutoPtr<TimeBase> mTimeBase;

        Int32 mCount;
        Int32 mLoadedCount;
        Int32 mLastCount;
        Int32 mUnpluggedCount;

        // Times are in microseconds for better accuracy when dividing by the
        // lock count, and are in "battery realtime" units.

        /**
         * The total time we have accumulated since the start of the original
         * boot, to the last time something interesting happened in the
         * current run.
         */
        Int64 mTotalTime;

        /**
         * The total time we loaded for the previous runs.  Subtract this from
         * mTotalTime to find the time for the current run of the system.
         */
        Int64 mLoadedTime;

        /**
         * The run time of the last run of the system, as loaded from the
         * saved data.
         */
        Int64 mLastTime;

        /**
         * The value of mTotalTime when unplug() was last called.  Subtract
         * this from mTotalTime to find the time since the last unplug from
         * power.
         */
        Int64 mUnpluggedTime;
    };

    class SamplingTimer : Timer
    {
    public:
        SamplingTimer(
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        SamplingTimer(
            /* [in] */ TimeBase* timeBase,
            /* [in] */ Boolean trackReportedValues);

        CARAPI_(void) SetStale();

        CARAPI_(void) SetUpdateVersion(
            /* [in] */ Int32 version);

        CARAPI_(Int32) GetUpdateVersion();

        CARAPI_(void) UpdateCurrentReportedCount(
            /* [in] */ Int32 count);

        CARAPI_(void) AddCurrentReportedCount(
            /* [in] */ Int32 delta);

        CARAPI_(void) UpdateCurrentReportedTotalTime(
            /* [in] */ Int64 totalTime);

        CARAPI_(void) AddCurrentReportedTotalTime(
            /* [in] */ Int64 delta);

        CARAPI OnTimeStarted(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        CARAPI OnTimeStopped(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        CARAPI LogState(
            /* [in] */ IPrinter* pw,
            /* [in] */ const String& prefix);

        CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 elapsedRealtimeUs);

    protected:
        CARAPI_(Int64) ComputeRunTimeLocked(
            /* [in] */ Int64 curBatteryRealtime);

        CARAPI_(Int32) ComputeCurrentCountLocked();

    private:
        CARAPI_(Boolean) Reset(
            /* [in] */ Boolean detachIfReset);

        CARAPI_(void) WriteSummaryFromParcelLocked(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 batteryRealtime);

        CARAPI_(void) ReadSummaryFromParcelLocked(
            /* [in] */ IParcel* in);

    private:
        /**
         * The most recent reported count from /proc/wakelocks.
         */
        Int32 mCurrentReportedCount;

        /**
         * The reported count from /proc/wakelocks when unplug() was last
         * called.
         */
        Int32 mUnpluggedReportedCount;

        /**
         * The most recent reported total_time from /proc/wakelocks.
         */
        Int64 mCurrentReportedTotalTime;


        /**
         * The reported total_time from /proc/wakelocks when unplug() was last
         * called.
         */
        Int64 mUnpluggedReportedTotalTime;

        /**
         * Whether we are currently in a discharge cycle.
         */
        Boolean mTimeBaseRunning;

        /**
         * Whether we are currently recording reported values.
         */
        Boolean mTrackingReportedValues;

        /*
         * A sequence counter, incremented once for each update of the stats.
         */
        Int32 mUpdateVersion;

        friend class BatteryStatsImpl;
    };

    class Uid;

    /**
     * A timer that increments in batches.  It does not run for durations, but just jumps
     * for a pre-determined amount.
     */
    class BatchTimer : Timer
    {
    public:
        BatchTimer(
            /* [in] */ Uid* uid,
            /* [in] */ Int32 type,
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        BatchTimer(
            /* [in] */ Uid* uid,
            /* [in] */ Int32 type,
            /* [in] */ TimeBase* timeBase);

        // @Override
        CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 elapsedRealtimeUs);

        CARAPI OnTimeStopped(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        CARAPI OnTimeStarted(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        CARAPI LogState(
            /* [in] */ IPrinter* pw,
            /* [in] */ const String& prefix);

        CARAPI_(void) AddDuration(
            /* [in] */ BatteryStatsImpl* stats,
            /* [in] */ Int64 durationMillis);

        CARAPI_(void) AbortLastDuration(
            /* [in] */ BatteryStatsImpl* stats);

    protected:
        CARAPI_(Int32) ComputeCurrentCountLocked();

        CARAPI_(Int64) ComputeRunTimeLocked(
            /* [in] */ Int64 curBatteryRealtime);

        // @Override
        CARAPI_(Boolean) Reset(
            /* [in] */ Boolean detachIfReset);

    private:
        CARAPI_(Int64) ComputeOverage(
            /* [in] */ Int64 curTime);

        CARAPI_(void) RecomputeLastDuration(
            /* [in] */ Int64 curTime,
            /* [in] */ Boolean abort);

    public:
        AutoPtr<Uid> mUid;

        /**
         * The last time at which we updated the timer.  This is in elapsed realtime microseconds.
         */
        Int64 mLastAddedTime;

        /**
         * The last duration that we added to the timer.  This is in microseconds.
         */
        Int64 mLastAddedDuration;

        /**
         * Whether we are currently in a discharge cycle.
         */
        Boolean mInDischarge;
    };

    /**
     * State for keeping track of timing information.
     */
    class StopwatchTimer : Timer
    {
    public:
        StopwatchTimer(
            /* [in] */ Uid* uid,
            /* [in] */ Int32 type,
            /* [in] */ List<AutoPtr<StopwatchTimer> >* timerPool,
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        StopwatchTimer(
            /* [in] */ Uid* uid,
            /* [in] */ Int32 type,
            /* [in] */ List<AutoPtr<StopwatchTimer> >* timerPool,
            /* [in] */ TimeBase* timeBase);

        CARAPI_(void) SetTimeout(
            /* [in] */ Int64 timeout);

        CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 elapsedRealtimeUs);

        CARAPI OnTimeStopped(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        CARAPI LogState(
            /* [in] */ IPrinter* pw,
            /* [in] */ const String& prefix);

    protected:
        CARAPI_(void) StartRunningLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(Boolean) IsRunningLocked();

        CARAPI_(Int64) CheckpointRunningLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) StopRunningLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) StopAllRunningLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(Int64) ComputeRunTimeLocked(
            /* [in] */ Int64 curBatteryRealtime);

        CARAPI_(Int32) ComputeCurrentCountLocked();

        CARAPI_(Boolean) Reset(
            /* [in] */ Boolean detachIfReset);

        CARAPI_(void) Detach();

        CARAPI_(void) ReadSummaryFromParcelLocked(
            /* [in] */ IParcel* in);

    private:
        // Update the total time for all other running Timers with the same type as this Timer
        // due to a change in timer count
        static CARAPI_(Int64) RefreshTimersLocked(
            /* [in] */ Int64 batteryRealtime,
            /* [in] */ List<AutoPtr<StopwatchTimer> >* pool,
            /* [in] */ StopwatchTimer* self);

    private:
        AutoPtr<Uid> mUid;
        AutoPtr< List<AutoPtr<StopwatchTimer> > > mTimerPool;

        Int32 mNesting;

        /**
         * The last time at which we updated the timer.  If mNesting is > 0,
         * subtract this from the current battery time to find the amount of
         * time we have been running since we last computed an update.
         */
        Int64 mUpdateTime;

        /**
         * The total time at which the timer was acquired, to determine if it
         * was actually held for an interesting duration.
         */
        Int64 mAcquireTime;

        Int64 mTimeout;

        /**
         * For partial wake locks, keep track of whether we are in the list
         * to consume CPU cycles.
         */
        Boolean mInList;

        friend class BatteryStatsImpl;
    };

    template<typename T>
    class OverflowArrayMap : public Object
    {
    public:
        OverflowArrayMap(
            /* [in] */ BatteryStatsImpl* host);

        CARAPI_(AutoPtr<IArrayMap>) GetMap();

        CARAPI_(void) Clear();

        CARAPI_(void) Add(
            /* [in] */ const String& name,
            /* [in] */ T* obj);

        CARAPI_(void) Cleanup();

        CARAPI_(AutoPtr<T>) StartObject(
            /* [in] */ const String& name);

        CARAPI_(AutoPtr<T>) StopObject(
            /* [in] */ const String& name);

        virtual CARAPI_(AutoPtr<T>) InstantiateObject() = 0;

    protected:
        AutoPtr<IArrayMap> mMap;
        AutoPtr<T> mCurOverflow;
        AutoPtr<HashMap<String, Int32/* MutableInt*/> > mActiveOverflow;

    private:
        static const String OVERFLOW_NAME;
        BatteryStatsImpl* mHost;
    };

    /**
     * The statistics associated with a particular uid.
     */
    class Uid
        : public BatteryStatsUid
    {
    public:
        /**
         * The statistics associated with a particular wake lock.
         */
        class Wakelock
            : public Object
            , public IBatteryStatsUidWakelock
        {
        public:
            Wakelock(
                /* [in] */ Uid* host)
                : mHost(host)
            {}

            CAR_INTERFACE_DECL()

            CARAPI GetWakeTime(
                /* [in] */ Int32 type,
                /* [out] */ IBatteryStatsTimer** timer);

            CARAPI GetStopwatchTimer(
                /* [in] */ Int32 type,
                /* [out] */ BatteryStatsImpl::StopwatchTimer** timer);

        private:
            /**
             * Reads a possibly null Timer from a Parcel.  The timer is associated with the
             * proper timer pool from the given BatteryStatsImpl object.
             *
             * @param in the Parcel to be read from.
             * return a new Timer, or null.
             */
            CARAPI_(AutoPtr<BatteryStatsImpl::StopwatchTimer>) ReadTimerFromParcel(
                /* [in] */ Int32 type,
                /* [in] */ List<AutoPtr<BatteryStatsImpl::StopwatchTimer> >* pool,
                /* [in] */ BatteryStatsImpl::TimeBase* timeBase,
                /* [in] */ IParcel* in);

            CARAPI_(Boolean) Reset();

            CARAPI_(void) ReadFromParcelLocked(
                /* [in] */ BatteryStatsImpl::TimeBase* timeBase,
                /* [in] */ BatteryStatsImpl::TimeBase* screenOffTimeBase,
                /* [in] */ IParcel* in);

            CARAPI_(void) WriteToParcelLocked(
                /* [in] */ IParcel* out,
                /* [in] */ Int64 elapsedRealtimeUs);

        private:
            /**
             * How long (in ms) this uid has been keeping the device partially awake.
             */
            AutoPtr<BatteryStatsImpl::StopwatchTimer> mTimerPartial;

            /**
             * How long (in ms) this uid has been keeping the device fully awake.
             */
            AutoPtr<BatteryStatsImpl::StopwatchTimer> mTimerFull;

            /**
             * How long (in ms) this uid has had a window keeping the device awake.
             */
            AutoPtr<BatteryStatsImpl::StopwatchTimer> mTimerWindow;

            Uid* mHost;
        };

        class Sensor
            : public Object
            , public IBatteryStatsUidSensor
        {
        public:
            Sensor(
                /* [in] */ Int32 handle,
                /* [in] */ Uid* host)
                : mHandle(handle)
                , mHost(host)
            {}

            CAR_INTERFACE_DECL()

            CARAPI GetHandle(
                /* [out] */ Int32* handle);

            CARAPI GetSensorTime(
                /* [out] */ IBatteryStatsTimer** time);

        private:
            CARAPI_(AutoPtr<BatteryStatsImpl::StopwatchTimer>) ReadTimerFromParcel(
                /* [in] */ BatteryStatsImpl::TimeBase* timeBase,
                /* [in] */ IParcel* in);

            CARAPI_(Boolean) Reset();

            CARAPI_(void) ReadFromParcelLocked(
                /* [in] */ BatteryStatsImpl::TimeBase* timeBase,
                /* [in] */ IParcel* in);

            CARAPI_(void) WriteToParcelLocked(
                /* [in] */ IParcel* out,
                /* [in] */ Int64 elapsedRealtimeUs);

        private:
            Int32 mHandle;
            AutoPtr<BatteryStatsImpl::StopwatchTimer> mTimer;
            Uid* mHost;
        };

        class Proc
            : public BatteryStats::Uid::Proc
            , public IBatteryStatsImplUidProc
            , public ITimeBaseObs
        {
        public:
            Proc(
                /* [in] */ const String& name,
                /* [in] */ BatteryStatsImpl* host);

            CAR_INTERFACE_DECL()

            CARAPI OnTimeStarted(
                /* [in] */ Int64 elapsedRealtime,
                /* [in] */ Int64 baseUptime,
                /* [in] */ Int64 baseRealtime);

            CARAPI OnTimeStopped(
                /* [in] */ Int64 elapsedRealtime,
                /* [in] */ Int64 baseUptime,
                /* [in] */ Int64 baseRealtime);

            CARAPI_(void) Detach();

            CARAPI_(Int32) CountExcessivePowers();

            CARAPI GetExcessivePower(
                /* [in] */ Int32 i,
                /* [out] */ IBatteryStatsUidProcExcessivePower* excessivePower);

            CARAPI_(void) AddExcessiveWake(
                /* [in] */ Int64 overTime,
                /* [in] */ Int64 usedTime);

            CARAPI_(void) AddExcessiveCpu(
                /* [in] */ Int64 overTime,
                /* [in] */ Int64 usedTime);

            CARAPI GetBatteryStats(
                /* [out] */ IBatteryStatsImpl** batteryStats);

            CARAPI AddCpuTimeLocked(
                /* [in] */ Int32 utime,
                /* [in] */ Int32 stime);

            CARAPI AddForegroundTimeLocked(
                /* [in] */ Int64 ttime);

            CARAPI_(void) IncStartsLocked();

            CARAPI IsActive(
                /* [out] */ Boolean* result);

            CARAPI GetUserTime(
                /* [in] */ Int32 which,
                /* [out] */ Int64* time);

            CARAPI GetSystemTime(
                /* [in] */ Int32 which,
                /* [out] */ Int64* time);

            CARAPI GetForegroundTime(
                /* [in] */ Int32 which,
                /* [out] */ Int64* time);

            CARAPI GetStarts(
                /* [in] */ Int32 which,
                /* [out] */ Int32* starts);

            /* Called by ActivityManagerService when CPU times are updated. */
            CARAPI AddSpeedStepTimes(
                /* [in] */ ArrayOf<Int64>* values);

            CARAPI GetTimeAtCpuSpeedStep(
                /* [in] */ Int32 speedStep,
                /* [in] */ Int32 which,
                /* [out] */ Int64* time);

        private:
            CARAPI_(void) WriteExcessivePowerToParcelLocked(
                /* [in] */ IParcel* out);

            CARAPI_(Boolean) ReadExcessivePowerFromParcelLocked(
                /* [in] */ IParcel* in);

            CARAPI_(void) WriteToParcelLocked(
                /* [in] */ IParcel* out);

            CARAPI_(void) ReadFromParcelLocked(
                /* [in] */ IParcel* in);

        private:
            /**
             * The name of this process.
             */
            String mName;

            /**
             * Remains true until removed from the stats.
             */
            Boolean mActive;

            /*
             * Total time (in 1/100 sec) spent executing in user code.
             */
            Int64 mUserTime;

            /**
             * Total time (in 1/100 sec) spent executing in kernel code.
             */
            Int64 mSystemTime;

            /**
             * Number of times the process has been started.
             */
            Int32 mStarts;

            /**
             * Amount of time the process was running in the foreground.
             */
            Int64 mForegroundTime;

            /**
             * The amount of user time loaded from a previous save.
             */
            Int64 mLoadedUserTime;

            /**
             * The amount of system time loaded from a previous save.
             */
            Int64 mLoadedSystemTime;

            /**
             * The number of times the process has started from a previous save.
             */
            Int32 mLoadedStarts;

            /**
             * The amount of foreground time loaded from a previous save.
             */
            Int64 mLoadedForegroundTime;

            /**
             * The amount of user time loaded from the previous run.
             */
            Int64 mLastUserTime;

            /**
             * The amount of system time loaded from the previous run.
             */
            Int64 mLastSystemTime;

            /**
             * The number of times the process has started from the previous run.
             */
            Int32 mLastStarts;

            /**
             * The amount of foreground time loaded from the previous run
             */
            Int64 mLastForegroundTime;

            /**
             * The amount of user time when last unplugged.
             */
            Int64 mUnpluggedUserTime;

            /**
             * The amount of system time when last unplugged.
             */
            Int64 mUnpluggedSystemTime;

            /**
             * The amount of foreground time since unplugged.
             */
            Int64 mUnpluggedForegroundTime;

            /**
             * The number of times the process has started before unplugged.
             */
            Int32 mUnpluggedStarts;

            /**
             * Current process state.
             */
            Int32 mProcessState;

            AutoPtr< ArrayOf<BatteryStatsImpl::SamplingCounter*> > mSpeedBins;

            AutoPtr<List<AutoPtr<ExcessivePower> > > mExcessivePower;

            BatteryStatsImpl* mHost;
        };

        class Pkg
            : public Object
            , public IBatteryStatsUidPkg
            , public IIBatteryStatsImplUidPkg
            , public ITimeBaseObs
        {
        public:
            class Serv
                : public Object
                , public IBatteryStatsUidPkgServ
                , public IBatteryStatsImplUidPkgServ
                , public ITimeBaseObs
            {
            public:
                Serv(
                    /* [in] */ BatteryStatsImpl* host);

                CAR_INTERFACE_DECL()

                CARAPI OnTimeStarted(
                    /* [in] */ Int64 elapsedRealtime,
                    /* [in] */ Int64 baseUptime,
                    /* [in] */ Int64 baseRealtime);

                CARAPI OnTimeStopped(
                    /* [in] */ Int64 elapsedRealtime,
                    /* [in] */ Int64 baseUptime,
                    /* [in] */ Int64 baseRealtime);

                CARAPI StartLaunchedLocked();

                CARAPI StopLaunchedLocked();

                CARAPI StartRunningLocked();

                CARAPI StopRunningLocked();

                CARAPI GetBatteryStats(
                    /* [out] */ IBatteryStatsImpl** stats);

                CARAPI GetLaunches(
                    /* [in] */ Int32 which,
                    /* [out] */ Int32* result);

                CARAPI GetStartTime(
                    /* [in] */ Int64 batteryUptime,
                    /* [in] */ Int32 which,
                    /* [out] */ Int64* value);

                CARAPI GetStarts(
                    /* [in] */ Int32 which,
                    /* [out] */ Int32* starts);

            private:
                CARAPI_(void) Detach();

                CARAPI_(void) ReadFromParcelLocked(
                    /* [in] */ IParcel* in);

                CARAPI_(void) WriteToParcelLocked(
                    /* [in] */ IParcel* out);

                CARAPI_(Int64) GetLaunchTimeToNowLocked(
                    /* [in] */ Int64 batteryUptime);

                CARAPI_(Int64) GetStartTimeToNowLocked(
                    /* [in] */ Int64 batteryUptime);

            private:
                /**
                 * Total time (ms in battery uptime) the service has been left started.
                 */
                Int64 mStartTime;

                /**
                 * If service has been started and not yet stopped, this is
                 * when it was started.
                 */
                Int64 mRunningSince;

                /**
                 * True if we are currently running.
                 */
                Boolean mRunning;

                /*
                 * Total number of times startService() has been called.
                 */
                Int32 mStarts;

                /**
                 * Total time (ms in battery uptime) the service has been left launched.
                 */
                Int64 mLaunchedTime;

                /**
                 * If service has been launched and not yet exited, this is
                 * when it was launched (ms in battery uptime).
                 */
                Int64 mLaunchedSince;

                /**
                 * True if we are currently launched.
                 */
                Boolean mLaunched;

                /**
                 * Total number times the service has been launched.
                 */
                Int32 mLaunches;

                /*
                 * The amount of time spent started loaded from a previous save
                 * (ms in battery uptime).
                 */
                Int64 mLoadedStartTime;

                /**
                 * The number of starts loaded from a previous save.
                 */
                Int32 mLoadedStarts;

                /**
                 * The number of launches loaded from a previous save.
                 */
                Int32 mLoadedLaunches;

                /**
                 * The amount of time spent started as of the last run (ms
                 * in battery uptime).
                 */
                Int64 mLastStartTime;

                /**
                 * The number of starts as of the last run.
                 */
                Int32 mLastStarts;

                /**
                 * The number of launches as of the last run.
                 */
                Int32 mLastLaunches;

                /**
                 * The amount of time spent started when last unplugged (ms
                 * in battery uptime).
                 */
                Int64 mUnpluggedStartTime;

                /**
                 * The number of starts when last unplugged.
                 */
                Int32 mUnpluggedStarts;

                /**
                 * The number of launches when last unplugged.
                 */
                Int32 mUnpluggedLaunches;

                BatteryStatsImpl* mHost;
            };

        public:
            Pkg(
                /* [in] */ BatteryStatsImpl* host);

            CAR_INTERFACE_DECL()

            CARAPI OnTimeStarted(
                /* [in] */ Int64 elapsedRealtime,
                /* [in] */ Int64 baseUptime,
                /* [in] */ Int64 baseRealtime);

            CARAPI OnTimeStopped(
                /* [in] */ Int64 elapsedRealtime,
                /* [in] */ Int64 baseUptime,
                /* [in] */ Int64 baseRealtime);

            CARAPI GetServiceStats(
                /* [out] */ IMap** stats);

            CARAPI GetWakeups(
                /* [in] */ Int32 which,
                /* [out] */ Int32* value);

            CARAPI GetBatteryStats(
                /* [out] */ IBatteryStatsImpl** batteryStats);

            CARAPI_(void) IncWakeupsLocked();

        private:
            CARAPI_(void) Detach();

            CARAPI_(void) ReadFromParcelLocked(
                /* [in] */ IParcel* in);

            CARAPI_(void) WriteToParcelLocked(
                /* [in] */ IParcel* out);

            CARAPI_(AutoPtr<Serv>) NewServiceStatsLocked();

        private:
            /**
             * Number of times this package has done something that could wake up the
             * device from sleep.
             */
            Int32 mWakeups;

            /**
             * Number of things that could wake up the device loaded from a
             * previous save.
             */
            Int32 mLoadedWakeups;

            /**
             * Number of things that could wake up the device as of the
             * last run.
             */
            Int32 mLastWakeups;

            /**
             * Number of things that could wake up the device as of the
             * last run.
             */
            Int32 mUnpluggedWakeups;

            /**
             * The statics we have collected for this package's services.
             */
            AutoPtr<IHashMap> mServiceStats;

            BatteryStatsImpl* mHost;
        };

    private:
        class WakelockStats : public OverflowArrayMap<Wakelock>
        {
        public:
            WakelockStats(
                /* [in] */ Uid* host)
                : mHost(host)
            {}

            CARAPI_(AutoPtr<Wakelock>) InstantiateObject();

        private:
            Uid* mHost;
        };

        class SyncStats : public OverflowArrayMap<StopwatchTimer>
        {
        public:
            SyncStats(
                /* [in] */ Uid* host)
                : mHost(host)
            {}

            CARAPI_(AutoPtr<StopwatchTimer>) InstantiateObject();

        private:
            Uid* mHost;
        };

        class JobStats : public OverflowArrayMap<StopwatchTimer>
        {
        public:
            SyncStats(
                /* [in] */ Uid* host)
                : mHost(host)
            {}

            CARAPI_(AutoPtr<StopwatchTimer>) InstantiateObject();

        private:
            Uid* mHost;
        };

    public:
        Uid(
            /* [in] */ Int32 uid,
            /* [in] */ BatteryStatsImpl* host);

        CARAPI GetWakelockStats(
            /* [out] */ IMap** stats);

        CARAPI GetSyncStats(
            /* [out] */ IMap** stats);

        CARAPI GetJobStats(
            /* [out] */ IMap** stats);

        CARAPI GetSensorStats(
            /* [out] */ ISparseArray** stats);

        CARAPI GetProcessStats(
            /* [out] */ IMap** stats);

        CARAPI GetPackageStats(
            /* [out] */ IMap** stats);

        CARAPI GetUid(
            /* [out] */ Int32* result);

        CARAPI NoteWifiRunningLocked(
            /* [in] */ Int64 elapsedRealtime);

        CARAPI NoteWifiStoppedLocked(
            /* [in] */ Int64 elapsedRealtime);

        CARAPI NoteFullWifiLockAcquiredLocked(
            /* [in] */ Int64 elapsedRealtime);

        CARAPI NoteFullWifiLockReleasedLocked(
            /* [in] */ Int64 elapsedRealtime);

        CARAPI NoteWifiScanStartedLocked(
            /* [in] */ Int64 elapsedRealtime);

        CARAPI NoteWifiScanStoppedLocked(
            /* [in] */ Int64 elapsedRealtime);

        CARAPI NoteWifiBatchedScanStartedLocked(
            /* [in] */ Int32 csph,
            /* [in] */ Int64 elapsedRealtime);

        CARAPI NoteWifiBatchedScanStoppedLocked(
            /* [in] */ Int64 elapsedRealtime);

        CARAPI NoteWifiMulticastEnabledLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI NoteWifiMulticastDisabledLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(AutoPtr<StopwatchTimer>) CreateAudioTurnedOnTimerLocked();

        CARAPI_(void) NoteAudioTurnedOnLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) NoteAudioTurnedOffLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) NoteResetAudioLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(AutoPtr<StopwatchTimer>) CreateVideoTurnedOnTimerLocked();

        CARAPI_(void) NoteVideoTurnedOnLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) NoteVideoTurnedOffLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) NoteResetVideoLocked(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(AutoPtr<StopwatchTimer>) CreateForegroundActivityTimerLocked();

        CARAPI NoteActivityResumedLocked(
            /* [in] */ Int64 elapsedRealtime);

        CARAPI NoteActivityPausedLocked(
            /* [in] */ Int64 elapsedRealtime);

        CARAPI_(void) UpdateUidProcessStateLocked(
            /* [in] */ Int32 state,
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(AutoPtr<BatchTimer>) CreateVibratorOnTimerLocked();

        CARAPI_(void) NoteVibratorOnLocked(
            /* [in] */ Int64 durationMillis);

        CARAPI_(void) NoteVibratorOffLocked();

        CARAPI GetWifiRunningTime(
            /* [in] */ Int64 elapsedRealtimeMs,
            /* [in] */ Int32 which,
            /* [out] */ Int64* time);

        CARAPI GetFullWifiLockTime(
            /* [in] */ Int64 elapsedRealtimeMs,
            /* [in] */ Int32 which,
            /* [out] */ Int64* time);

        CARAPI GetWifiScanTime(
            /* [in] */ Int64 elapsedRealtimeMs,
            /* [in] */ Int32 which,
            /* [out] */ Int64* time);

        CARAPI GetWifiBatchedScanTime(
            /* [in] */ Int32 csphBin,
            /* [in] */ Int64 elapsedRealtimeUs,
            /* [in] */ Int32 which,
            /* [out] */ Int64* time);

        CARAPI GetWifiMulticastTime(
            /* [in] */ Int64 elapsedRealtimeUs,
            /* [in] */ Int32 which,
            /* [out] */ Int64* time);

        CARAPI GetAudioTurnedOnTime(
            /* [in] */ Int64 elapsedRealtimeUs,
            /* [in] */ Int32 which,
            /* [out] */ Int64* time);

        CARAPI GetVideoTurnedOnTime(
            /* [in] */ Int64 elapsedRealtimeUs,
            /* [in] */ Int32 which,
            /* [out] */ Int64* time);

        CARAPI GetForegroundActivityTimer(
            /* [out] */ IBatteryStatsTimer** timer);

        CARAPI_(void) MakeProcessState(
            /* [in] */ Int32 i,
            /* [in] */ IParcel* in);

        CARAPI GetProcessStateTime(
            /* [in] */ Int32 state,
            /* [in] */ Int64 elapsedRealtimeUs,
            /* [in] */ Int32 which,
            /* [out] */ Int64* result);

        CARAPI GetVibratorOnTimer(
            /* [out] */ IBatteryStatsTimer** timer);

        CARAPI NoteUserActivityLocked(
            /* [in] */ Int32 type);

        CARAPI HasUserActivity(
            /* [out] */ Boolean* result);

        CARAPI GetUserActivityCount(
            /* [in] */ Int32 type,
            /* [in] */ Int32 which,
            /* [out] */ Int32* count);

        CARAPI HasNetworkActivity(
            /* [out] */ Boolean* result);

        CARAPI GetNetworkActivityBytes(
            /* [in] */ Int32 type,
            /* [in] */ Int32 which,
            /* [out] */ Int64* result);

        CARAPI GetNetworkActivityPackets(
            /* [in] */ Int32 type,
            /* [in] */ Int32 which,
            /* [out] */ Int64* result);

        CARAPI GetMobileRadioActiveTime(
            /* [in] */ Int32 which,
            /* [out] */ Int64* result);

        CARAPI GetMobileRadioActiveCount(
            /* [in] */ Int32 which,
            /* [out] */ Int32* result);

        /**
         * Retrieve the statistics object for a particular process, creating
         * if needed.
         */
        CARAPI_(AutoPtr<Proc>) GetProcessStatsLocked(
            /* [in] */ const String& name);

        CARAPI_(void) UpdateProcessStateLocked(
            /* [in] */ const String& procName,
            /* [in] */ Int32 state,
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) UpdateRealProcessStateLocked(
            /* [in] */ const String& procName,
            /* [in] */ Int32 procState,
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(AutoPtr<ISparseArray>) GetPidStats();

        CARAPI_(AutoPtr<Pid>) GetPidStatsLocked(
            /* [in] */ Int32 pid);

        /**
         * Retrieve the statistics object for a particular service, creating
         * if needed.
         */
        CARAPI_(AutoPtr<Pkg>) GetPackageStatsLocked(
            /* [in] */ const String& name);

        /**
         * Retrieve the statistics object for a particular service, creating
         * if needed.
         */
        CARAPI_(AutoPtr<Pkg::Serv>) GetServiceStatsLocked(
            /* [in] */ const String& pkg,
            /* [in] */ const String& serv);

        CARAPI_(void) ReadSyncSummaryFromParcelLocked(
            /* [in] */ const String& name,
            /* [in] */ IParcel* in);

        CARAPI_(void) ReadJobSummaryFromParcelLocked(
            /* [in] */ const String& name,
            /* [in] */ IParcel* in);

        CARAPI_(void) ReadWakeSummaryFromParcelLocked(
            /* [in] */ const String& wlName,
            /* [in] */ IParcel* in);

        CARAPI_(AutoPtr<BatteryStatsImpl::StopwatchTimer>) GetSensorTimerLocked(
            /* [in] */ Int32 sensor,
            /* [in] */ Boolean create);

        CARAPI_(void) NoteStartSyncLocked(
            /* [in] */ const String& name,
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) NoteStopSyncLocked(
            /* [in] */ const String& name,
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) NoteStartJobLocked(
            /* [in] */ const String& name,
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) NoteStopJobLocked(
            /* [in] */ const String& name,
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) NoteStartWakeLocked(
            /* [in] */ Int32 pid,
            /* [in] */ const String& name,
            /* [in] */ Int32 type,
            /* [in] */ Int64 elapsedRealtime);

        CARAPI_(void) NoteStopWakeLocked(
            /* [in] */ Int32 pid,
            /* [in] */ const String& name,
            /* [in] */ Int32 type,
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) ReportExcessiveWakeLocked(
            /* [in] */ const String& proc,
            /* [in] */ Int64 overTime,
            /* [in] */ Int64 usedTime);

        CARAPI_(void) ReportExcessiveCpuLocked(
            /* [in] */ const String& proc,
            /* [in] */ Int64 overTime,
            /* [in] */ Int64 usedTime);

        CARAPI_(void) NoteStartSensor(
            /* [in] */ Int32 sensor,
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) NoteStopSensor(
            /* [in] */ Int32 sensor,
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) NoteStartGps(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(void) NoteStopGps(
            /* [in] */ Int64 elapsedRealtimeMs);

        CARAPI_(AutoPtr<BatteryStatsImpl>) GetBatteryStats();

    private:
        CARAPI_(void) MakeWifiBatchedScanBin(
            /* [in] */ Int32 i,
            /* [in] */ IParcel* in);

        CARAPI_(void) InitUserActivityLocked();

        CARAPI_(void) NoteNetworkActivityLocked(
            /* [in] */ Int32 type,
            /* [in] */ Int64 deltaBytes,
            /* [in] */ Int64 deltaPackets);

        CARAPI_(void) NoteMobileRadioActiveTimeLocked(
            /* [in] */ Int64 batteryUptime);

        CARAPI_(void) InitNetworkActivityLocked();

        /**
         * Clear all stats for this uid.  Returns true if the uid is completely
         * inactive so can be dropped.
         */
        CARAPI_(Boolean) Reset();

        CARAPI_(void) WriteToParcelLocked(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 elapsedRealtimeUs);

        CARAPI_(void) ReadFromParcelLocked(
            /* [in] */ TimeBase* timeBase,
            /* [in] */ TimeBase* screenOffTimeBase,
            /* [in] */ IParcel* in);

    public:
        static const Int32 NO_BATCHED_SCAN_STARTED = -1;
        static const Int32 PROCESS_STATE_NONE = IBatteryStatsUid::NUM_PROCESS_STATE;

    private:
        Int32 mUid;

        Boolean mWifiRunning;
        AutoPtr<BatteryStatsImpl::StopwatchTimer> mWifiRunningTimer;

        Boolean mFullWifiLockOut;
        AutoPtr<BatteryStatsImpl::StopwatchTimer> mFullWifiLockTimer;

        Boolean mWifiScanStarted;
        AutoPtr<BatteryStatsImpl::StopwatchTimer> mWifiScanTimer;

        Int32 mWifiBatchedScanBinStarted;
        AutoPtr<ArrayOf<BatteryStatsImpl::StopwatchTimer*> > mWifiBatchedScanTimer;

        Boolean mWifiMulticastEnabled;
        AutoPtr<BatteryStatsImpl::StopwatchTimer> mWifiMulticastTimer;

        AutoPtr<BatteryStatsImpl::StopwatchTimer> mAudioTurnedOnTimer;
        AutoPtr<BatteryStatsImpl::StopwatchTimer> mVideoTurnedOnTimer;

        AutoPtr<BatteryStatsImpl::StopwatchTimer> mForegroundActivityTimer;

        Int32 mProcessState;
        AutoPtr<ArrayOf<SBatteryStatsImpl::topwatchTimer*> > mProcessStateTimer;

        AutoPtr<BatteryStatsImpl::BatchTimer> mVibratorOnTimer;

        AutoPtr< ArrayOf<BatteryStatsImpl::Counter*> > mUserActivityCounters;

        AutoPtr<ArrayOf<BatteryStatsImpl::Int64SamplingCounter*> > mNetworkByteActivityCounters;
        AutoPtr<ArrayOf<BatteryStatsImpl::Int64SamplingCounter*> > mNetworkPacketActivityCounters;
        AutoPtr<BatteryStatsImpl::Int64SamplingCounter> mMobileRadioActiveTime;
        AutoPtr<BatteryStatsImpl::Int64SamplingCounter> mMobileRadioActiveCount;

        /**
         * The statistics we have collected for this uid's wake locks.
         */
        AutoPtr<WakelockStats> mWakelockStats;

        /**
         * The statistics we have collected for this uid's syncs.
         */
        AutoPtr<SyncStats> mSyncStats;

        /**
         * The statistics we have collected for this uid's jobs.
         */
        AutoPtr<JobStats> mJobStats

        /**
         * The statistics we have collected for this uid's sensor activations.
         */
        AutoPtr<ISparseArray> mSensorStats;

        /**
         * The statistics we have collected for this uid's processes.
         */
        AutoPtr<IArrayMap> mProcessStats;

        /**
         * The statistics we have collected for this uid's processes.
         */
        AutoPtr<IArrayMap> mPackageStats;

        /**
         * The transient wake stats we have collected for this uid's pids.
         */
        AutoPtr<ISparseArray> mPids;

        BatteryStatsImpl* mHost;

        friend class BatteryStatsImpl;
    };

    class BatteryStatsWriteRunnable : public Runnable
    {
    public:
        BatteryStatsWriteRunnable(
            /* [in] */ BatteryStatsImpl* host)
            : mHost(host)
        {}

        CARAPI Run();

    private:
        BatteryStatsImpl* mHost;
    };

private:
    class KernelWakelockStats : public Object
    {
    public:
        KernelWakelockStats(
            /* [in] */ Int32 count,
            /* [in] */ Int64 totalTime,
            /* [in] */ Int32 version)
            : mCount(count)
            , mTotalTime(totalTime)
            , mVersion(version)
        {}

    public:
        Int32 mCount;
        Int64 mTotalTime;
        Int32 mVersion;
    };

    class SetOnBatteryRunnable : public Runnable
    {
    public:
        SetOnBatteryRunnable(
            /* [in] */ IParcel* in,
            /* [in] */ BatteryStatsImpl* host)
            : mParcel(in)
            , mHost(host)
        {}

        CARAPI Run();

    private:
        AutoPtr<IParcel> mParcel;
        BatteryStatsImpl* mHost;
    };

public:
    BatteryStatsImpl();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IFile* systemDir,
        /* [in] */ IHandler* handler);

    CARAPI constructor(
        /* [in] */ IParcel* p);

    HashMap<String, AutoPtr<BatteryStatsTimer> >& GetKernelWakelockStats();

    HashMap<String, AutoPtr<SamplingTimer> >& GetWakeupReasonStats();

    /*
     * Get the KernelWakelockTimer associated with name, and create a new one if one
     * doesn't already exist.
     */
    CARAPI_(AutoPtr<SamplingTimer>) GetKernelWakelockTimerLocked(
        /* [in] */ const String& name);

    CARAPI_(Int32) GetBluetoothPingCount();

    CARAPI_(void) SetBtHeadset(
        /* [in] */ IBluetoothHeadset* headset);

    CARAPI_(void) WriteHistoryDelta(
        /* [in] */ IParcel* dest,
        /* [in] */ HistoryItem* cur,
        /* [in] */ HistoryItem* last);

    CARAPI_(void) ReadHistoryDelta(
        /* [in] */ IParcel* src,
        /* [in] */ HistoryItem* cur);

    CARAPI_(void) CommitCurrentHistoryBatchLocked();

    CARAPI_(void) UpdateTimeBasesLocked(
        /* [in] */ Boolean unplugged,
        /* [in] */ Boolean screenOff,
        /* [in] */ Int64 uptime,
        /* [in] */ Int64 realtime);

    CARAPI_(void) AddIsolatedUidLocked(
        /* [in] */ Int32 isolatedUid,
        /* [in] */ Int32 appUid);

    CARAPI_(void) RemoveIsolatedUidLocked(
        /* [in] */ Int32 isolatedUid,
        /* [in] */ Int32 appUid);

    CARAPI_(Int32) MapUid(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteEventLocked(
        /* [in] */ Int32 code,
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteCurrentTimeChangedLocked();

    CARAPI_(void) NoteProcessStartLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteProcessStateLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 state);

    CARAPI_(void) NoteProcessFinishLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteSyncStartLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteSyncFinishLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteJobStartLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteJobFinishLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI_(void) SetRecordAllHistoryLocked(
        /* [in] */ Boolean enabled);

    CARAPI_(void) SetNoAutoReset(
        /* [in] */ Boolean enabled);

    CARAPI_(void) NoteStartWakeLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ const String& historyName,
        /* [in] */ Int32 type,
        /* [in] */ Boolean unimportantForLogging,
        /* [in] */ Int64 elapsedRealtime,
        /* [in] */ Int64 uptime);

    CARAPI_(void) NoteStopWakeLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ const String& historyName,
        /* [in] */ Int32 type,
        /* [in] */ Int64 elapsedRealtime,
        /* [in] */ Int64 uptime);

    CARAPI_(void) NoteChangeWakelockFromSourceLocked(
        /* [in] */ IWorkSource* ws,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ const String& historyName,
        /* [in] */ Int32 type,
        /* [in] */ IWorkSource* newWs,
        /* [in] */ Int32 newPid,
        /* [in] */ const String& newName,
        /* [in] */ const String& newHistoryName,
        /* [in] */ Int32 newType,
        /* [in] */ Boolean newUnimportantForLogging);

    CARAPI_(void) NoteStartWakeFromSourceLocked(
        /* [in] */ IWorkSource* ws,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ Int32 type);

    CARAPI_(void) NoteStopWakeFromSourceLocked(
        /* [in] */ IWorkSource* ws,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ const String& historyName,
        /* [in] */ Int32 type);

    CARAPI_(void) NoteWakeupReasonLocked(
        /* [in] */ const String& reason);

    CARAPI_(Int32) StartAddingCpuLocked();

    CARAPI_(void) FinishAddingCpuLocked(
        /* [in] */ Int32 perc,
        /* [in] */ Int32 utime,
        /* [in] */ Int32 stime,
        /* [in] */ ArrayOf<Int64>* cpuSpeedTimes);

    CARAPI_(void) NoteProcessDiedLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid);

    CARAPI_(Int64) GetProcessWakeTime(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid,
        /* [in] */ Int64 realtime);

    CARAPI_(void) ReportExcessiveWakeLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& proc,
        /* [in] */ Int64 overTime,
        /* [in] */ Int64 usedTime);

    CARAPI_(void) ReportExcessiveCpuLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& proc,
        /* [in] */ Int64 overTime,
        /* [in] */ Int64 usedTime);

    CARAPI_(void) NoteStartSensorLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 sensor);

    CARAPI_(void) NoteStopSensorLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 sensor);

    CARAPI_(void) NoteStartGpsLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteStopGpsLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteScreenStateLocked(
        /* [in] */ Int32 state);

    CARAPI_(void) NoteScreenBrightnessLocked(
        /* [in] */ Int32 brightness);

    CARAPI_(void) NoteUserActivityLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 event);

    CARAPI_(void) NoteInteractiveLocked(
        /* [in] */ Boolean interactive);

    CARAPI_(void) NoteMobileRadioPowerState(
        /* [in] */ Int32 powerState,
        /* [in] */ Int64 timestampNs);

    CARAPI_(void) NoteLowPowerMode(
        /* [in] */ Boolean enabled);

    CARAPI_(void) NotePhoneOnLocked();

    CARAPI_(void) NotePhoneOffLocked();

    /**
     * Telephony stack updates the phone state.
     * @param state phone state from ServiceState.getState()
     */
    CARAPI_(void) NotePhoneStateLocked(
        /* [in] */ Int32 state,
        /* [in] */ Int32 simState);

    CARAPI_(void) NotePhoneSignalStrengthLocked(
        /* [in] */ ISignalStrength* signalStrength);

    CARAPI_(void) NotePhoneDataConnectionStateLocked(
        /* [in] */ Int32 dataType,
        /* [in] */ Boolean hasData);

    CARAPI_(void) NoteWifiOnLocked();

    CARAPI_(void) NoteWifiOffLocked();

    CARAPI_(void) NoteAudioOnLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteAudioOffLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteVideoOnLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteVideoOffLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteResetAudioLocked();

    CARAPI_(void) NoteResetVideoLocked();

    CARAPI_(void) NoteActivityResumedLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteActivityPausedLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteVibratorOnLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int64 durationMillis);

    CARAPI_(void) NoteVibratorOffLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteFlashlightOnLocked();

    CARAPI_(void) NoteFlashlightOffLocked();

    CARAPI_(void) NoteWifiRunningLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI_(void) NoteWifiRunningChangedLocked(
        /* [in] */ IWorkSource* oldWs,
        /* [in] */ IWorkSource* newWs);

    CARAPI_(void) NoteWifiStoppedLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI_(void) NoteWifiStateLocked(
        /* [in] */ Int32 wifiState,
        /* [in] */ const String& accessPoint);

    CARAPI_(void) NoteWifiSupplicantStateChangedLocked(
        /* [in] */ Int32 supplState,
        /* [in] */ Boolean failedAuth);

    CARAPI_(void) NoteWifiRssiChangedLocked(
        /* [in] */ Int32 newRssi);

    CARAPI_(void) NoteBluetoothOnLocked();

    CARAPI_(void) NoteBluetoothOffLocked();

    CARAPI_(void) NoteBluetoothStateLocked(
        /* [in] */ Int32 bluetoothState);

    CARAPI_(void) NoteFullWifiLockAcquiredLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteFullWifiLockReleasedLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteWifiScanStartedLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteWifiScanStoppedLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteWifiBatchedScanStartedLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 csph);

    CARAPI_(void) NoteWifiBatchedScanStoppedLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteWifiMulticastEnabledLocked(
        /* [in] */ Int64 elapsedRealtimeMs);

    CARAPI_(void) NoteWifiMulticastDisabledLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteFullWifiLockAcquiredFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI_(void) NoteFullWifiLockReleasedFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI_(void) NoteWifiScanStartedFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI_(void) NoteWifiScanStoppedFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI_(void) NoteWifiBatchedScanStartedFromSourceLocked(
        /* [in] */ IWorkSource* ws,
        /* [in] */ Int32 csph);

    CARAPI_(void) NoteWifiBatchedScanStoppedFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI_(void) NoteWifiMulticastEnabledFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI_(void) NoteWifiMulticastDisabledFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI_(void) NoteNetworkInterfaceTypeLocked(
        /* [in] */ const String& iface,
        /* [in] */ Int32 networkType);

    CARAPI_(void) NoteNetworkStatsEnabledLocked();

    // @Override
    CARAPI_(Int64) GetScreenOnTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int32) GetScreenOnCount(
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetScreenOnTime(
        /* [in] */ Int64 batteryRealtime,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetScreenBrightnessTime(
        /* [in] */ Int32 brightnessBin,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetInteractiveTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetLowPowerModeEnabledTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int32) GetLowPowerModeEnabledCount(
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetPhoneOnTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int32) GetPhoneOnCount(
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetPhoneSignalStrengthTime(
        /* [in] */ Int32 strengthBin,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetPhoneSignalScanningTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int32) GetPhoneSignalStrengthCount(
        /* [in] */ Int32 strengthBin,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetPhoneDataConnectionTime(
        /* [in] */ Int32 dataType,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int32) GetPhoneDataConnectionCount(
        /* [in] */ Int32 dataType,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetMobileRadioActiveTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int32) GetMobileRadioActiveCount(
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetMobileRadioActiveAdjustedTime(
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetMobileRadioActiveUnknownTime(
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int32) GetMobileRadioActiveUnknownCount(
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetWifiOnTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetGlobalWifiRunningTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetWifiStateTime(
        /* [in] */ Int32 wifiState,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int32) GetWifiStateCount(
        /* [in] */ Int32 wifiState,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetWifiSupplStateTime(
        /* [in] */ Int32 state,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int32) GetWifiSupplStateCount(
        /* [in] */ Int32 state,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetWifiSignalStrengthTime(
        /* [in] */ Int32 strengthBin,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int32) GetWifiSignalStrengthCount(
        /* [in] */ Int32 strengthBin,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetBluetoothOnTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetBluetoothStateTime(
        /* [in] */ Int32 bluetoothState,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int32) GetBluetoothStateCount(
        /* [in] */ Int32 bluetoothState,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetFlashlightOnTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetFlashlightOnCount(
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetNetworkActivityBytes(
        /* [in] */ Int32 type,
        /* [in] */ Int32 which);

    // @Override
    CARAPI_(Int64) GetNetworkActivityPackets(
        /* [in] */ Int32 type,
        /* [in] */ Int32 which);

    CARAPI_(Boolean) IsStartClockTimeValid();

    // @Override
    CARAPI_(Int64) GetStartClockTime();

    // @Override
    CARAPI_(String) GetStartPlatformVersion();

    // @Override
    CARAPI_(String) GetEndPlatformVersion();

    // @Override
    CARAPI_(Int32) GetParcelVersion();

    // @Override
    CARAPI_(AutoPtr<ISparseArray>) GetUidStats();

    CARAPI_(void) SetCallback(
        /* [in] */ BatteryCallback* cb);

    CARAPI_(void) SetNumSpeedSteps(
        /* [in] */ Int32 steps);

    CARAPI_(void) SetRadioScanningTimeout(
        /* [in] */ Int64 timeout);

    CARAPI StartIteratingOldHistoryLocked(
        /* [out] */ Boolean* result);

    CARAPI GetNextOldHistoryLocked(
        /* [in] */ IBatteryStatsHistoryItem* out,
        /* [out] */ Boolean* result);

    CARAPI FinishIteratingOldHistoryLocked();

    CARAPI_(Int32) GetHistoryTotalSize();

    CARAPI_(Int32) GetHistoryUsedSize();

    CARAPI StartIteratingHistoryLocked(
        /* [out] */ Boolean* result);

    CARAPI GetHistoryStringPoolSize(
        /* [out] */ Int32* result);

    CARAPI GetHistoryStringPoolBytes(
        /* [out] */ Int32* result);

    CARAPI GetHistoryTagPoolString(
        /* [in] */ Int32 index,
        /* [out] */ String* str);

    CARAPI GetHistoryTagPoolUid(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);

    CARAPI GetNextHistoryLocked(
        /* [in] */ IBatteryStatsHistoryItem* out,
        /* [out] */ Boolean* result);

    CARAPI FinishIteratingHistoryLocked();

    CARAPI GetHistoryBaseTime(
        /* [out] */ Int64* result);

    CARAPI GetStartCount(
        /* [out] */ Int32* count);

    CARAPI_(Boolean) IsOnBattery();

    CARAPI_(Boolean) IsScreenOn();

    CARAPI_(void) ResetAllStatsCmdLocked();

    CARAPI_(void) PullPendingStateUpdatesLocked();

    CARAPI_(void) SetBatteryState(
        /* [in] */ Int32 status,
        /* [in] */ Int32 health,
        /* [in] */ Int32 plugType,
        /* [in] */ Int32 level,
        /* [in] */ Int32 temp,
        /* [in] */ Int32 volt);

    CARAPI_(void) UpdateKernelWakelocksLocked();

    CARAPI_(Int64) GetAwakeTimeBattery();

    CARAPI_(Int64) GetAwakeTimePlugged();

    CARAPI ComputeUptime(
        /* [in] */ Int64 curTime,
        /* [in] */ Int32 which,
        /* [out] */ Int64* value);

    CARAPI ComputeRealtime(
        /* [in] */ Int64 curTime,
        /* [in] */ Int32 which,
        /* [out] */ Int64* value);

    CARAPI ComputeBatteryUptime(
        /* [in] */ Int64 curTime,
        /* [in] */ Int32 which,
        /* [out] */ Int64* value);

    CARAPI ComputeBatteryRealtime(
        /* [in] */ Int64 curTime,
        /* [in] */ Int32 which,
        /* [out] */ Int64* value);

    CARAPI ComputeBatteryScreenOffUptime(
        /* [in] */ Int64 curTime,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    CARAPI ComputeBatteryScreenOffRealtime(
        /* [in] */ Int64 curTime,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    CARAPI ComputeBatteryTimeRemaining(
        /* [in] */ Int64 curTime,
        /* [out] */ Int64* result);

    CARAPI_(Int32) GetNumDischargeStepDurations();

    CARAPI_(AutoPtr<ArrayOf<Int64> >) GetDischargeStepDurationsArray();

    CARAPI ComputeChargeTimeRemaining(
        /* [in] */ Int64 curTime,
        /* [out] */ Int64* result);

    CARAPI_(Int32) GetNumChargeStepDurations();

    CARAPI_(AutoPtr<ArrayOf<Int64> >) GetChargeStepDurationsArray();

    CARAPI GetBatteryUptime(
        /* [in] */ Int64 curTime,
        /* [out] */ Int64* result);

    CARAPI GetBatteryRealtime(
        /* [in] */ Int64 curTime,
        /* [out] */ Int64* result);

    CARAPI GetDischargeStartLevel(
        /* [out] */ Int32* level);

    CARAPI_(Int32) GetDischargeStartLevelLocked();

    CARAPI GetDischargeCurrentLevel(
        /* [out] */ Int32* level);

    CARAPI_(Int32) GetDischargeCurrentLevelLocked();

    CARAPI GetLowDischargeAmountSinceCharge(
        /* [out] */ Int32* result);

    CARAPI GetHighDischargeAmountSinceCharge(
        /* [out] */ Int32* result);

    CARAPI GetDischargeAmount(
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    CARAPI_(Int32) GetDischargeAmountScreenOn();

    CARAPI_(Int32) GetDischargeAmountScreenOnSinceCharge();

    CARAPI_(Int32) GetDischargeAmountScreenOff();

    CARAPI_(Int32) GetDischargeAmountScreenOffSinceCharge();

    CARAPI GetCpuSpeedSteps(
        /* [out] */ Int32* steps);

    /**
     * Retrieve the statistics object for a particular uid, creating if needed.
     */
    CARAPI_(AutoPtr<Uid>) GetUidStatsLocked(
        /* [in] */ Int32 uid);

    /**
     * Remove the statistics object for a particular uid.
     */
    CARAPI_(void) RemoveUidStatsLocked(
        /* [in] */ Int32 uid);

    /**
     * Retrieve the statistics object for a particular process, creating
     * if needed.
     */
    CARAPI_(AutoPtr<Uid::Proc>) GetProcessStatsLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& name);

    /**
     * Retrieve the statistics object for a particular process, creating
     * if needed.
     */
    CARAPI_(AutoPtr<Uid::Pkg>) GetPackageStatsLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& pkg);

    /**
     * Retrieve the statistics object for a particular service, creating
     * if needed.
     */
    CARAPI_(AutoPtr<Uid::Pkg::Serv>) GetServiceStatsLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& pkg,
        /* [in] */ const String& name);

    // /**
    //  * Massage data to distribute any reasonable work down to more specific
    //  * owners.  Must only be called on a dead BatteryStats object!
    //  */
    CARAPI_(void) DistributeWorkLocked(
        /* [in] */ Int32 which);

    CARAPI_(void) ShutdownLocked();

    CARAPI_(void) WriteAsyncLocked();

    CARAPI_(void) WriteSyncLocked();

    CARAPI_(void) CommitPendingDataToDisk();

    CARAPI_(void) ReadLocked();

    CARAPI ReadSummaryFromParcel(
        /* [in] */ IParcel* in);

    /**
     * Writes a summary of the statistics to a Parcel, in a format suitable to be written to
     * disk.  This format does not allow a lossless round-trip.
     *
     * @param out the Parcel to be written to.
     */
    CARAPI_(void) WriteSummaryToParcel(
        /* [in] */ IParcel* out);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI ReadFromParcelLocked(
        /* [in] */ IParcel* in);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI_(void) WriteToParcelWithoutUids(
        /* [in] */ IParcel* out);

    CARAPI_(void) PrepareForDumpLocked();

    CARAPI_(void) DumpLocked(
        /* [in] */ IPrintWriter* pw);

private:
    CARAPI_(void) HandleUpdateWakelocks();

    CARAPI_(void) HandleReportPowerChange(
        /* [in] */ Boolean onBattery);

    HashMap<String, AutoPtr<KernelWakelockStats> >& ReadKernelWakelockStats();

    HashMap<String, AutoPtr<KernelWakelockStats> >& ParseProcWakelocks(
        /* [in] */ ArrayOf<Byte>* wlBuffer,
        /* [in] */ Int32 len,
        /* [in] */ Boolean wakeup_sources);

    CARAPI_(Int32) GetCurrentBluetoothPingCount();

    CARAPI_(Int32) WriteHistoryTag(
        /* [in] */ HistoryTag* tag);

    CARAPI_(void) ReadHistoryTag(
        /* [in] */ Int32 index,
        /* [in] */ HistoryTag* tag);

    CARAPI_(Int32) BuildBatteryLevelInt(
        /* [in] */ HistoryItem* h);

    CARAPI_(Int32) BuildStateInt(
        /* [in] */ HistoryItem* h);

    CARAPI_(void) AddHistoryBufferLocked(
        /* [in] */ Int64 elapsedRealtimeMs,
        /* [in] */ Int64 uptimeMs,
        /* [in] */ HistoryItem* cur);

    CARAPI AddHistoryBufferLocked(
        /* [in] */ Int64 elapsedRealtimeMs,
        /* [in] */ Int64 uptimeMs,
        /* [in] */ Byte cmd,
        /* [in] */ HistoryItem* cur);

    CARAPI_(void) AddHistoryRecordLocked(
        /* [in] */ Int64 elapsedRealtimeMs,
        /* [in] */ Int64 uptimeMs);

    CARAPI_(void) AddHistoryRecordInnerLocked(
        /* [in] */ Int64 curTime,
        /* [in] */ Int64 uptimeMs,
        /* [in] */ HistoryItem* cur);

    CARAPI_(void) AddHistoryEventLocked(
        /* [in] */ Int64 elapsedRealtimeMs,
        /* [in] */ Int64 uptimeMs,
        /* [in] */ Int32 code,
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI_(void) AddHistoryRecordLocked(
        /* [in] */ Int64 elapsedRealtimeMs,
        /* [in] */ Int64 uptimeMs,
        /* [in] */ Byte cmd,
        /* [in] */ HistoryItem* cur);

    CARAPI_(void) AddHistoryRecordLocked(
        /* [in] */ HistoryItem* rec);

    CARAPI_(void) ClearHistoryLocked();

    CARAPI_(void) RequestWakelockCpuUpdate();

    CARAPI_(void) AggregateLastWakeupUptimeLocked(
        /* [in] */ Int64 uptimeMs);

    CARAPI_(void) StopAllPhoneSignalStrengthTimersLocked(
        /* [in] */ Int32 except);

    CARAPI_(Int32) FixPhoneServiceState(
        /* [in] */ Int32 state,
        /* [in] */ Int32 signalBin);

    CARAPI_(void) UpdateAllPhoneStateLocked(
        /* [in] */ Int32 state,
        /* [in] */ Int32 simState,
        /* [in] */ Int32 strengthBin);

    CARAPI_(void) StopAllWifiSignalStrengthTimersLocked(
        /* [in] */ Int32 except);

    static CARAPI_(AutoPtr<ArrayOf<String> >) IncludeInStringArray(
        /* [in] */ ArrayOf<String>* array,
        /* [in] */ const String& str);

    static CARAPI_(AutoPtr<ArrayOf<String> >) ExcludeFromStringArray(
        /* [in] */ ArrayOf<String>* array,
        /* [in] */ const String& str);

    CARAPI_(Boolean) GetIsOnBattery();

    CARAPI_(void) InitTimes(
        /* [in] */ Int64 uptime,
        /* [in] */ Int64 realtime);

    CARAPI_(void) InitDischarge();

    CARAPI_(void) ResetAllStatsLocked();

    CARAPI_(void) InitActiveHistoryEventsLocked(
        /* [in] */ Int64 elapsedRealtimeMs,
        /* [in] */ Int64 uptimeMs);

    CARAPI_(void) UpdateDischargeScreenLevelsLocked(
        /* [in] */ Boolean oldScreenOn,
        /* [in] */ Boolean newScreenOn);

    CARAPI_(void) SetOnBattery(
        /* [in] */ Boolean onBattery,
        /* [in] */ Int32 oldStatus,
        /* [in] */ Int32 level);

    CARAPI_(void) SetOnBatteryLocked(
        /* [in] */ Int64 mSecRealtime,
        /* [in] */ Int64 mSecUptime,
        /* [in] */ Boolean onBattery,
        /* [in] */ Int32 oldStatus,
        /* [in] */ Int32 level);

    CARAPI_(void) StartRecordingHistory(
        /* [in] */ Int64 elapsedRealtimeMs,
        /* [in] */ Int64 uptimeMs,
        /* [in] */ Boolean reset);

    CARAPI_(void) RecordCurrentTimeChangeLocked(
        /* [in] */ Int64 currentTime,
        /* [in] */ Int64 elapsedRealtimeMs,
        /* [in] */ Int64 uptimeMs);

    static CARAPI_(Int32) AddLevelSteps(
        /* [in] */ ArrayOf<Int64>* steps,
        /* [in] */ Int32 stepCount,
        /* [in] */ Int64 lastStepTime,
        /* [in] */ Int32 numStepLevels,
        /* [in] */ Int64 modeBits,
        /* [in] */ Int64 elapsedRealtime);

    CARAPI_(void) UpdateNetworkActivityLocked(
        /* [in] */ Int32 which,
        /* [in] */ Int64 elapsedRealtimeMs);

    CARAPI_(Int64) ComputeTimePerLevel(
        /* [in] */ ArrayOf<Int64>* steps,
        /* [in] */ Int32 numSteps);

    CARAPI_(Int64) GetBatteryUptimeLocked();

    CARAPI_(Int64) GetTcpBytes(
        /* [in] */ Int64 current,
        /* [in] */ ArrayOf<Int64>* dataBytes,
        /* [in] */ Int32 which);

    CARAPI_(void) WriteLocked(
        /* [in] */ Boolean sync);

    CARAPI_(void) ReadHistory(
        /* [in] */ IParcel* in,
        /* [in] */ Boolean andOldHistory);

    CARAPI_(void) ReadOldHistory(
        /* [in] */ IParcel* in);

    CARAPI_(void) WriteHistory(
        /* [in] */ IParcel* out,
        /* [in] */ Boolean inclData,
        /* [in] */ Boolean andOldHistory);

    CARAPI_(void) WriteOldHistory(
        /* [in] */ IParcel* out);

    CARAPI_(void) WriteToParcelLocked(
        /* [in] */ IParcel* out,
        /* [in] */ Boolean inclUids);

    CARAPI_(AutoPtr<INetworkStats>) GetNetworkStatsSummary();

    CARAPI_(AutoPtr<INetworkStats>) GetNetworkStatsDetailGroupedByUid();

public:
    static const Int32 MSG_UPDATE_WAKELOCKS = 1;
    static const Int32 MSG_REPORT_POWER_CHANGE = 2;

    static const Int64 DELAY_UPDATE_WAKELOCKS = 5 * 1000;

    static const Int32 MAX_HISTORY_BUFFER = 256*1024; // 256KB
    static const Int32 MAX_MAX_HISTORY_BUFFER = 320*1024; // 320KB

    static const Int32 MAX_LEVEL_STEPS = 200;

    // Part of initial delta int that specifies the time delta.
    static const Int32 DELTA_TIME_MASK = 0x7ffff;
    static const Int32 DELTA_TIME_LONG = 0x7ffff;   // The delta is a following long
    static const Int32 DELTA_TIME_INT = 0x7fffe;    // The delta is a following int
    static const Int32 DELTA_TIME_ABS = 0x7fffd;    // Following is an entire abs update.
    // Flag in delta int: a new battery level int follows.
    static const Int32 DELTA_BATTERY_LEVEL_FLAG   = 0x00080000;
    // Flag in delta int: a new full state and battery status int follows.
    static const Int32 DELTA_STATE_FLAG           = 0x00100000;
    // Flag in delta int: a new full state2 int follows.
    static const Int32 DELTA_STATE2_FLAG          = 0x00200000;
    // Flag in delta int: contains a wakelock or wakeReason tag.
    static const Int32 DELTA_WAKELOCK_FLAG        = 0x00400000;
    // Flag in delta int: contains an event description.
    static const Int32 DELTA_EVENT_FLAG           = 0x00800000;
    // These upper bits are the frequently changing state bits.
    static const Int32 DELTA_STATE_MASK           = 0xff000000;

    // These are the pieces of battery state that are packed in to the upper bits of
    // the state int that have been packed in to the first delta int.  They must fit
    // in DELTA_STATE_MASK.
    static const Int32 STATE_BATTERY_STATUS_MASK  = 0x00000007;
    static const Int32 STATE_BATTERY_STATUS_SHIFT = 29;
    static const Int32 STATE_BATTERY_HEALTH_MASK  = 0x00000007;
    static const Int32 STATE_BATTERY_HEALTH_SHIFT = 26;
    static const Int32 STATE_BATTERY_PLUG_MASK    = 0x00000003;
    static const Int32 STATE_BATTERY_PLUG_SHIFT   = 24;

    AutoPtr<IAtomicFile> mCheckinFile;
    Object mCheckinFileLock;
    AutoPtr<IHandler> mHandler;

private:
    static const String TAG;
    static const Boolean DEBUG = FALSE;
    static const Boolean DEBUG_HISTORY = FALSE;
    static const Boolean USE_OLD_HISTORY = FALSE;   // for debugging.

    // TODO: remove "tcp" from network methods, since we measure total stats.

    // In-memory Parcel magic number, used to detect attempts to unmarshall bad data
    static const Int32 MAGIC = 0xBA757475; // 'BATSTATS'

    // Current on-disk Parcel version
    static const Int32 VERSION = 114 + (USE_OLD_HISTORY ? 1000 : 0);

    // Maximum number of items we will record in the history.
    static const Int32 MAX_HISTORY_ITEMS = 2000;

    // No, really, THIS is the maximum number of items we will record in the history.
    static const Int32 MAX_MAX_HISTORY_ITEMS = 3000;

    // The maximum number of names wakelocks we will keep track of
    // per uid; once the limit is reached, we batch the remaining wakelocks
    // in to one common name.
    static const Int32 MAX_WAKELOCKS_PER_UID = 100;

    static Int32 sNumSpeedSteps;

    static Int32 sKernelWakelockUpdateVersion;

    static const AutoPtr< ArrayOf<Int32> > PROC_WAKELOCKS_FORMAT;

    static const AutoPtr< ArrayOf<Int32> > WAKEUP_SOURCES_FORMAT;

    // This should probably be exposed in the API, though it's not critical
    static const Int32 BATTERY_PLUGGED_NONE = 0;

    static const Int32 NET_UPDATE_MOBILE = 1 << 0;
    static const Int32 NET_UPDATE_WIFI = 1 << 1;
    static const Int32 NET_UPDATE_ALL = 0xffff;

    AutoPtr<IJournaledFile> mFile;

    AutoPtr<IBatteryCallback> mCallback;

    /**
     * Mapping isolated uids to the actual owning app uid.
     */
    HashMap<Int32, Int32> mIsolatedUids;

    /**
     * The statistics we have collected organized by uids.
     */
    AutoPtr<ISparseArray> mUidStats;

    // A set of pools of currently active timers.  When a timer is queried, we will divide the
    // elapsed time by the number of active timers to arrive at that timer's share of the time.
    // In order to do this, we must refresh each timer whenever the number of active timers
    // changes.
    List<AutoPtr<StopwatchTimer> > mPartialTimers;
    List<AutoPtr<StopwatchTimer> > mFullTimers;
    List<AutoPtr<StopwatchTimer> > mWindowTimers;
    HashMap<Int32, AutoPtr< List<AutoPtr<StopwatchTimer> > > > mSensorTimers;
    List<AutoPtr<StopwatchTimer> > mWifiRunningTimers;
    List<AutoPtr<StopwatchTimer> > mFullWifiLockTimers;
    List<AutoPtr<StopwatchTimer> > mWifiMulticastTimers;
    List<AutoPtr<StopwatchTimer> > mWifiScanTimers;
    HashMap<Int32, AutoPtr<List<AutoPtr<StopwatchTimer> > > > mWifiBatchedScanTimers;
    List<AutoPtr<StopwatchTimer> > mAudioTurnedOnTimers;
    List<AutoPtr<StopwatchTimer> > mVideoTurnedOnTimers;

    // Last partial timers we use for distributing CPU usage.
    List<AutoPtr<StopwatchTimer> > mLastPartialTimers;

    // These are the objects that will want to do something when the device
    // is unplugged from power.
    AutoPtr<TimeBase> mOnBatteryTimeBase;
    // These are the objects that will want to do something when the device
    // is unplugged from power *and* the screen is off.
    AutoPtr<TimeBase> mOnBatteryScreenOffTimeBase;

    // Set to true when we want to distribute CPU across wakelocks for the next
    // CPU update, even if we aren't currently running wake locks.
    Boolean mDistributeWakelockCpu;

    Boolean mShuttingDown;

    AutoPtr<HistoryEventTracker> mActiveEvents;

    Int64 mHistoryBaseTime;
    Boolean mHaveBatteryLevel;
    Boolean mRecordingHistory;
    Int32 mNumHistoryItems;

    AutoPtr<IParcel> mHistoryBuffer;
    AutoPtr<HistoryItem> mHistoryLastWritten;
    AutoPtr<HistoryItem> mHistoryLastLastWritten;
    AutoPtr<HistoryItem> mHistoryReadTmp;
    AutoPtr<HistoryItem> mHistoryAddTmp;
    HashMap<AutoPtr<HistoryTag>, AutoPtr<IInteger32> > mHistoryTagPool;
    AutoPtr<ArrayOf<String> > mReadHistoryStrings;
    AutoPtr<ArrayOf<Int32> > mReadHistoryUids;
    Int32 mReadHistoryChars;
    Int32 mNextHistoryTagIdx;
    Int32 mNumHistoryTagChars;
    Int32 mHistoryBufferLastPos;
    Boolean mHistoryOverflow;
    Int64 mLastHistoryElapsedRealtime;
    Int64 mTrackRunningHistoryElapsedRealtime;
    Int64 mTrackRunningHistoryUptime;

    AutoPtr<HistoryItem> mHistoryCur;

    AutoPtr<HistoryItem> mHistory;
    AutoPtr<HistoryItem> mHistoryEnd;
    AutoPtr<HistoryItem> mHistoryLastEnd;
    AutoPtr<HistoryItem> mHistoryCache;

    AutoPtr<HistoryItem> mHistoryIterator;
    Boolean mReadOverflow;
    Boolean mIteratingHistory;

    Int32 mStartCount;

    Int64 mStartClockTime;
    String mStartPlatformVersion;
    String mEndPlatformVersion;

    Int64 mLastRecordedClockTime;
    Int64 mLastRecordedClockRealtime;

    Int64 mUptime;
    Int64 mUptimeStart;
    Int64 mRealtime;
    Int64 mRealtimeStart;

    Int32 mWakeLockNesting;
    Boolean mWakeLockImportant;
    Boolean mRecordAllHistory;
    Boolean mNoAutoReset;

    Int32 mScreenState;

    AutoPtr<StopwatchTimer> mScreenOnTimer;

    Int32 mScreenBrightnessBin;
    AutoPtr< ArrayOf<StopwatchTimer*> > mScreenBrightnessTimer;

    Boolean mInteractive;
    AutoPtr<StopwatchTimer> mInteractiveTimer;

    Boolean mLowPowerModeEnabled;
    AutoPtr<StopwatchTimer> mLowPowerModeEnabledTimer;

    Boolean mPhoneOn;
    AutoPtr<StopwatchTimer> mPhoneOnTimer;

    Int32 mAudioOnNesting;
    AutoPtr<StopwatchTimer> mAudioOnTimer;

    Int32 mVideoOnNesting;
    AutoPtr<StopwatchTimer> mVideoOnTimer;

    Boolean mFlashlightOn;
    AutoPtr<StopwatchTimer> mFlashlightOnTimer;

    Int32 mPhoneSignalStrengthBin;
    Int32 mPhoneSignalStrengthBinRaw;
    AutoPtr< ArrayOf<StopwatchTimer*> > mPhoneSignalStrengthsTimer;

    AutoPtr<StopwatchTimer> mPhoneSignalScanningTimer;

    Int32 mPhoneDataConnectionType;
    AutoPtr< ArrayOf<StopwatchTimer*> > mPhoneDataConnectionsTimer;

    AutoPtr< ArrayOf<Int64SamplingCounter*> > mNetworkByteActivityCounters;
    AutoPtr< ArrayOf<Int64SamplingCounter*> > mNetworkPacketActivityCounters;

    Boolean mWifiOn;
    AutoPtr<StopwatchTimer> mWifiOnTimer;

    Boolean mGlobalWifiRunning;
    AutoPtr<StopwatchTimer> mGlobalWifiRunningTimer;

    Int32 mWifiState;
    AutoPtr< ArrayOf<StopwatchTimer*> > mWifiStateTimer;

    Int32 mWifiSupplState;
    AutoPtr< ArrayOf<StopwatchTimer*> > mWifiSupplStateTimer;

    Int32 mWifiSignalStrengthBin;
    AutoPtr< ArrayOf<StopwatchTimer*> > mWifiSignalStrengthsTimer;

    Boolean mBluetoothOn;
    AutoPtr<StopwatchTimer> mBluetoothOnTimer;

    Int32 mBluetoothState;
    AutoPtr< ArrayOf<StopwatchTimer*> > mBluetoothStateTimer;

    Int32 mMobileRadioPowerState;
    Int64 mMobileRadioActiveStartTime;
    AutoPtr<StopwatchTimer> mMobileRadioActiveTimer;
    AutoPtr<StopwatchTimer> mMobileRadioActivePerAppTimer;
    AutoPtr<Int64SamplingCounter> mMobileRadioActiveAdjustedTime;
    AutoPtr<Int64SamplingCounter> mMobileRadioActiveUnknownTime;
    AutoPtr<Int64SamplingCounter> mMobileRadioActiveUnknownCount;

    /** Bluetooth headset object */
    AutoPtr<IBluetoothHeadset> mBtHeadset;

     /**
     * These provide time bases that discount the time the device is plugged
     * in to power.
     */
    Boolean mOnBattery;
    Boolean mOnBatteryInternal;

    /*
     * These keep track of battery levels (1-100) at the last plug event and the last unplug event.
     */
    Int32 mDischargeStartLevel;
    Int32 mDischargeUnplugLevel;
    Int32 mDischargePlugLevel;
    Int32 mDischargeCurrentLevel;
    Int32 mCurrentBatteryLevel;
    Int32 mLowDischargeAmountSinceCharge;
    Int32 mHighDischargeAmountSinceCharge;
    Int32 mDischargeScreenOnUnplugLevel;
    Int32 mDischargeScreenOffUnplugLevel;
    Int32 mDischargeAmountScreenOn;
    Int32 mDischargeAmountScreenOnSinceCharge;
    Int32 mDischargeAmountScreenOff;
    Int32 mDischargeAmountScreenOffSinceCharge;

    Int32 mInitStepMode;
    Int32 mCurStepMode;
    Int32 mModStepMode;

    Int32 mLastDischargeStepLevel;
    Int64 mLastDischargeStepTime;
    Int32 mMinDischargeStepLevel;
    Int32 mNumDischargeStepDurations;
    AutoPtr<ArrayOf<Int64> > mDischargeStepDurations;

    Int32 mLastChargeStepLevel;
    Int64 mLastChargeStepTime;
    Int32 mMaxChargeStepLevel;
    Int32 mNumChargeStepDurations;
    AutoPtr<ArrayOf<Int64> > mChargeStepDurations;

    Int64 mLastWriteTime;

    Int32 mBluetoothPingCount;
    Int32 mBluetoothPingStart;

    Int32 mPhoneServiceState;
    Int32 mPhoneServiceStateRaw;
    Int32 mPhoneSimStateRaw;

    /*
     * Holds a SamplingTimer associated with each kernel wakelock name being tracked.
     */
    HashMap<String, AutoPtr<SamplingTimer> > mKernelWakelockStats;

    String mLastWakeupReason;
    Int64 mLastWakeupUptimeMs;
    HashMap<String, AutoPtr<SamplingTimer> > mWakeupReasonStats;

    AutoPtr< ArrayOf<String> > mProcWakelocksName;
    AutoPtr< ArrayOf<Int64> > mProcWakelocksData;

    /*
     * Used as a buffer for reading in data from /proc/wakelocks before it is processed and added
     * to mKernelWakelockStats.
     */
    HashMap<String, AutoPtr<KernelWakelockStats> > mProcWakelockFileStats;

    AutoPtr<INetworkStatsFactory> mNetworkStatsFactory;
    AutoPtr<INetworkStats> mCurMobileSnapshot;
    AutoPtr<INetworkStats> mLastMobileSnapshot;
    AutoPtr<INetworkStats> mCurWifiSnapshot;
    AutoPtr<INetworkStats> mLastWifiSnapshot;
    AutoPtr<INetworkStats> mTmpNetworkStats;
    AutoPtr<INetworkStatsEntry> mTmpNetworkStatsEntry;

    // @GuardedBy("this")
    AutoPtr<ArrayOf<String> > mMobileIfaces;
    // @GuardedBy("this")
    AutoPtr<ArrayOf<String> > mWifiIfaces;

    Int32 mChangedStates;
    Int32 mChangedStates2;

    String mInitialAcquireWakeName;
    Int32 mInitialAcquireWakeUid;

    Int32 mWakeLockNesting;

    Int32 mSensorNesting;

    Int32 mGpsNesting;

    Int32 mWifiFullLockNesting;

    Int32 mWifiScanNesting;

    Int32 mWifiMulticastNesting;

    AutoPtr<IParcel> mPendingWrite;
    AutoPtr<IReentrantLock> mWriteLock;

    AutoPtr<INetworkStats> mNetworkSummaryCache;
    AutoPtr<INetworkStats> mNetworkDetailCache;

    friend class OverflowArrayMap;
};


//==============================================================================
// BatteryStatsImpl::OverflowArrayMap
//==============================================================================

const String BatteryStatsImpl::OverflowArrayMap<T>::OVERFLOW_NAME("*overflow*");

template<typename T>
BatteryStatsImpl::OverflowArrayMap<T>::OverflowArrayMap(
    /* [in] */ BatteryStatsImpl* host)
    : mHost(host)
{
    CArrayMap::New((IArrayMap**)&mMap);
}

template<typename T>
AutoPtr<IArrayMap> BatteryStatsImpl::OverflowArrayMap<T>::GetMap()
{
    return mMap;
}

template<typename T>
void BatteryStatsImpl::OverflowArrayMap<T>::Clear()
{
    mMap->Clear();
    mCurOverflow = NULL;
    mActiveOverflow = NULL;
}

template<typename T>
void BatteryStatsImpl::OverflowArrayMap<T>::Add(
    /* [in] */ const String& name,
    /* [in] */ T* obj)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    mMap->Put(cs, (IObject*)obj);
    if (OVERFLOW_NAME.Equals(name)) {
        mCurOverflow = obj;
    }
}

template<typename T>
void BatteryStatsImpl::OverflowArrayMap<T>::Cleanup()
{
    if (mActiveOverflow != NULL) {
        if (mActiveOverflow->Begin() == mActiveOverflow->End()) {
            mActiveOverflow = NULL;
        }
    }
    if (mActiveOverflow == NULL) {
        // There is no currently active overflow, so we should no longer have
        // an overflow entry.
        AutoPtr<ICharSequence> cs;
        CString::New(OVERFLOW_NAME, (ICharSequence**)&cs);
        Boolean containsKey;
        if (mMap->ContainsKey(cs, &containsKey), containsKey) {
            Slogger::W(TAG, "Cleaning up with no active overflow, but have overflow entry %s", TO_CSTR(it->mSecond));
            mMap->Remove(cs);
        }
        mCurOverflow = NULL;
    }
    else {
        // There is currently active overflow, so we should still have an overflow entry.
        AutoPtr<ICharSequence> cs;
        CString::New(OVERFLOW_NAME, (ICharSequence**)&cs);
        Boolean containsKey;
        if (mCurOverflow == NULL || (mMap->ContainsKey(cs, &containsKey), !containsKey)) {
            Slogger::W(TAG, "Cleaning up with active overflow, but no overflow entry: cur=%s,  map=%s",
                    TO_CSTR(mCurOverflow), TO_CSTR(it->mSecond));
        }
    }
}

template<typename T>
AutoPtr<T> BatteryStatsImpl::OverflowArrayMap<T>::StartObject(
    /* [in] */ const String& name)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<T> obj;
    AutoPtr<IInterface> value;
    mMap->Get(cs, (IInterface**)&value);
    if (value != NULL) {
        obj = (T*)(IObject*)value.Get();
        return obj;
    }

    // No object exists for the given name, but do we currently have it
    // running as part of the overflow?
    if (mActiveOverflow != NULL) {
        HashMap<String, Int32>::Iterator overflowIt = mActiveOverflow->Find(name);
        if (overflowIt != mActiveOverflow->End()) {
            // We are already actively counting this name in the overflow object.
            obj = mCurOverflow;
            if (obj == NULL) {
                // Shouldn't be here, but we'll try to recover.
                Slogger::W(TAG, "Have active overflow %s but null overflow", name.string());
                obj = mCurOverflow = InstantiateObject();
                AutoPtr<ICharSequence> cs1;
                CString::New(OVERFLOW_NAME, (ICharSequence**)&cs1);
                mMap->Put(cs1, (IObject*)obj.Get());
            }
            overflowIt->mSecond++;
            return obj;
        }
    }

    // No object exists for given name nor in the overflow; we need to make
    // a new one.
    Int32 N;
    mMap->GetSize(&N);
    if (N >= BatteryStatsImpl::MAX_WAKELOCKS_PER_UID) {
        // Went over the limit on number of objects to track; this one goes
        // in to the overflow.
        obj = mCurOverflow;
        if (obj == NULL) {
            // Need to start overflow now...
            obj = mCurOverflow = InstantiateObject();
            AutoPtr<ICharSequence> cs1;
            CString::New(OVERFLOW_NAME, (ICharSequence**)&cs1);
            mMap->Put(cs1, (IObject*)obj.Get());
        }
        if (mActiveOverflow == NULL) {
            mActiveOverflow = new HashMap<String, Int32>();
        }
        (*mActiveOverflow)[name] = 1;
        return obj;
    }

    // Normal case where we just need to make a new object.
    obj = InstantiateObject();
    mMap->Put(cs, (IObject*)obj.Get());
    return obj;
}

template<typename T>
AutoPtr<T> BatteryStatsImpl::OverflowArrayMap<T>::StopObject(
    /* [in] */ const String& name)
{
    AutoPtr<T> obj;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<T> obj;
    AutoPtr<IInterface> value;
    mMap->Get(cs, (IInterface**)&value);
    if (value != NULL) {
        obj = (T*)(IObject*)value.Get();
        return obj;
    }

    // No object exists for the given name, but do we currently have it
    // running as part of the overflow?
    if (mActiveOverflow != NULL) {
        HashMap<String, Int32>::Iterator overflowIt = mActiveOverflow->Find(name);
        if (overflowIt != mActiveOverflow->End()) {
            // We are already actively counting this name in the overflow object.
            obj = mCurOverflow;
            if (obj != NULL) {
                overflowIt->mSecond--;
                if (overflowIt->mSecond <= 0) {
                    mActiveOverflow->Erase(overflowIt);
                }
                return obj;
            }
        }
    }

    // Huh, they are stopping an active operation but we can't find one!
    // That's not good.
    Int32 size;
    mMap->GetSize(&size);
    Slogger::W(TAG, "Unable to find object for %s mapsize=%d activeoverflow=%p curoverflow=%s",
            name.string(), size, mActiveOverflow.Get(), TO_CSTR(mCurOverflow));
    return NULL;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSIMPL_H__
