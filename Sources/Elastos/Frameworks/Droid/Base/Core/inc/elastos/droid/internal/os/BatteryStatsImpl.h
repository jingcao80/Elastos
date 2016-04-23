
#ifndef __ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSIMPL_H__
#define __ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSIMPL_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/BatteryStats.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/HashSet.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Net::INetworkStats;
using Elastos::Droid::Net::INetworkStatsEntry;
using Elastos::Droid::Internal::Net::INetworkStatsFactory;
using Elastos::Droid::Internal::Utility::IJournaledFile;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Utility::IPrinter;
using Elastos::Core::IRunnable;
using Elastos::Core::IInteger32;
using Elastos::IO::IFileInputStream;
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

        // CARAPI_(void) dump(PrintWriter pw, String prefix);

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

    class SamplingTimer : CARAPI_(AutoPtr<T>)imer
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
    class BatchTimer : CARAPI_(AutoPtr<T>)imer
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
    class StopwatchTimer : CARAPI_(AutoPtr<T>)imer
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

        HashMap<String, AutoPtr<T> > GetMap();

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
        HashMap<String, AutoPtr<T> > mMap;
        AutoPtr<T> mCurOverflow;
        AutoPtr<HashMap<String, Int32/* MutableInt*/> > mActiveOverflow;

    private:
        static const String OVERFLOW_NAME;
        BatteryStatsImpl* mHost;
    };

    /**
     * The statistics associated with a particular uid.
     */
    class Uid : public BatteryStatsUid
    {
    public:
        /**
         * The statistics associated with a particular wake lock.
         */
        class Wakelock : public BatteryStatsWakelock
        {
        public:
            Wakelock(
                /* [in] */ Uid* host)
                : mHost(host)
            {}

            CARAPI GetWakeTime(
                /* [in] */ Int32 type,
                /* [out] */ BatteryStatsTimer** timer);

        private:
            /**
             * Reads a possibly null Timer from a Parcel.  The timer is associated with the
             * proper timer pool from the given BatteryStatsImpl object.
             *
             * @param in the Parcel to be read from.
             * return a new Timer, or null.
             */
            CARAPI_(AutoPtr<StopwatchTimer>) ReadTimerFromParcel(
                /* [in] */ Int32 type,
                /* [in] */ List<AutoPtr<StopwatchTimer> >* pool,
                /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
                /* [in] */ IParcel* in);

            CARAPI_(Boolean) Reset();

            CARAPI_(void) ReadFromParcelLocked(
                /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
                /* [in] */ IParcel* in);

            CARAPI_(void) WriteToParcelLocked(
                /* [in] */ IParcel* out,
                /* [in] */ Int64 batteryRealtime);

        private:
            /**
             * How long (in ms) this uid has been keeping the device partially awake.
             */
            AutoPtr<StopwatchTimer> mTimerPartial;

            /**
             * How long (in ms) this uid has been keeping the device fully awake.
             */
            AutoPtr<StopwatchTimer> mTimerFull;

            /**
             * How long (in ms) this uid has had a window keeping the device awake.
             */
            AutoPtr<StopwatchTimer> mTimerWindow;

            Uid* mHost;

            friend class Uid;
            friend class BatteryStatsImpl;
        };

        class Sensor : public BatteryStatsSensor
        {
        public:
            Sensor(
                /* [in] */ Int32 handle,
                /* [in] */ Uid* host)
                : mHandle(handle)
                , mHost(host)
            {}

            CARAPI_(Int32) GetHandle();

            CARAPI_(AutoPtr<BatteryStatsTimer>) GetSensorTime();

        private:
            CARAPI_(AutoPtr<StopwatchTimer>) ReadTimerFromParcel(
                /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
                /* [in] */ IParcel* in);

            CARAPI_(Boolean) Reset();

            CARAPI_(void) ReadFromParcelLocked(
                /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
                /* [in] */ IParcel* in);

            CARAPI_(void) WriteToParcelLocked(
                /* [in] */ IParcel* out,
                /* [in] */ Int64 batteryRealtime);

        private:
            Int32 mHandle;
            AutoPtr<StopwatchTimer> mTimer;
            Uid* mHost;

            friend class Uid;
            friend class BatteryStatsImpl;
        };

        class Proc
            : public BatteryStatsProc
            , public IUnpluggable
        {
        public:
            CAR_INTERFACE_DECL()

            Proc(
                /* [in] */ Uid* host);

            virtual ~Proc();

            CARAPI_(void) Unplug(
                /* [in] */ Int64 batteryUptime,
                /* [in] */ Int64 batteryRealtime);

            CARAPI_(void) Plug(
                /* [in] */ Int64 batteryUptime,
                /* [in] */ Int64 batteryRealtime);

            CARAPI_(void) Detach();

            CARAPI_(Int32) CountExcessivePowers();

            CARAPI_(AutoPtr<ExcessivePower>) GetExcessivePower(
                /* [in] */ Int32 i);

            CARAPI_(void) AddExcessiveWake(
                /* [in] */ Int64 overTime,
                /* [in] */ Int64 usedTime);

            CARAPI_(void) AddExcessiveCpu(
                /* [in] */ Int64 overTime,
                /* [in] */ Int64 usedTime);

            CARAPI_(AutoPtr<BatteryStatsImpl>) GetBatteryStats();

            CARAPI_(void) AddCpuTimeLocked(
                /* [in] */ Int32 utime,
                /* [in] */ Int32 stime);

            CARAPI_(void) AddForegroundTimeLocked(
                /* [in] */ Int64 ttime);

            CARAPI_(void) IncStartsLocked();

            CARAPI_(Int64) GetUserTime(
                /* [in] */ Int32 which);

            CARAPI_(Int64) GetSystemTime(
                /* [in] */ Int32 which);

            CARAPI_(Int64) GetForegroundTime(
                /* [in] */ Int32 which);

            CARAPI_(Int32) getStarts(
                /* [in] */ Int32 which);

            /* Called by ActivityManagerService when CPU times are updated. */
            CARAPI_(void) AddSpeedStepTimes(
                /* [in] */ ArrayOf<Int64>* values);

            CARAPI_(Int64) GetTimeAtCpuSpeedStep(
                /* [in] */ Int32 speedStep,
                /* [in] */ Int32 which);

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
             * The number of times the process has started before unplugged.
             */
            Int32 mUnpluggedStarts;

            /**
             * The amount of foreground time since unplugged.
             */
            Int64 mUnpluggedForegroundTime;

            AutoPtr< ArrayOf<SamplingCounter*> > mSpeedBins;

            List<AutoPtr<ExcessivePower> > mExcessivePower;

            Uid* mHost;

            friend class Uid;
            friend class BatteryStatsImpl;
        };

        class Pkg
            : public BatteryStatsPkg
            , public IUnpluggable
        {
        public:
            class Serv
                : public BatteryStatsServ
                , public IUnpluggable
            {
            public:
                Serv(
                    /* [in] */ Pkg* host);

                CAR_INTERFACE_DECL()

                CARAPI_(void) Unplug(
                    /* [in] */ Int64 batteryUptime,
                    /* [in] */ Int64 batteryRealtime);

                CARAPI_(void) Plug(
                    /* [in] */ Int64 batteryUptime,
                    /* [in] */ Int64 batteryRealtime);

                CARAPI_(void) StartLaunchedLocked();

                CARAPI_(void) StopLaunchedLocked();

                CARAPI_(void) StartRunningLocked();

                CARAPI_(void) StopRunningLocked();

                CARAPI_(AutoPtr<BatteryStatsImpl>) GetBatteryStats();

                CARAPI_(Int32) GetLaunches(
                    /* [in] */ Int32 which);

                CARAPI_(Int64) GetStartTime(
                    /* [in] */ Int64 batteryUptime,
                    /* [in] */ Int32 which);

                CARAPI_(Int32) GetStarts(
                    /* [in] */ Int32 which);

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

                Pkg* mHost;

                friend class Pkg;
                friend class Uid;
                friend class BatteryStatsImpl;
            };

        public:
            Pkg(
                /* [in] */ Uid* host);

            CAR_INTERFACE_DECL()

            CARAPI_(void) Unplug(
                /* [in] */ Int64 batteryUptime,
                /* [in] */ Int64 batteryRealtime);

            CARAPI_(void) Plug(
                /* [in] */ Int64 batteryUptime,
                /* [in] */ Int64 batteryRealtime);

            AutoPtr< HashMap<String, AutoPtr<BatteryStatsServ> > > GetServiceStats();

            CARAPI_(Int32) GetWakeups(
                /* [in] */ Int32 which);

            CARAPI_(AutoPtr<BatteryStatsImpl>) GetBatteryStats();

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
            HashMap<String, AutoPtr<Serv> > mServiceStats;

            Uid* mHost;

            friend class Uid;
            friend class BatteryStatsImpl;
        };

    public:
        Uid(
            /* [in] */ Int32 uid,
            /* [in] */ BatteryStatsImpl* host);

        AutoPtr< HashMap<String, AutoPtr<BatteryStatsImpl::Uid::BatteryStatsWakelock> > > GetWakelockStats();

        AutoPtr< HashMap<Int32, AutoPtr<BatteryStatsSensor> > > GetSensorStats();

        HashMap<Int32, AutoPtr<Pid> >& GetPidStats();

        AutoPtr< HashMap<String, AutoPtr<BatteryStatsProc> > > GetProcessStats();

        AutoPtr< HashMap<String, AutoPtr<BatteryStatsPkg> > > GetPackageStats();

        CARAPI_(Int32) GetUid();

        CARAPI_(Int64) GetTcpBytesReceived(
            /* [in] */ Int32 which);

        CARAPI_(Int64) ComputeCurrentTcpBytesReceived();

        CARAPI_(Int64) GetTcpBytesSent(
            /* [in] */ Int32 which);

        CARAPI_(void) NoteWifiRunningLocked();

        CARAPI_(void) NoteWifiStoppedLocked();

        CARAPI_(void) NoteFullWifiLockAcquiredLocked();

        CARAPI_(void) NoteFullWifiLockReleasedLocked();

        CARAPI_(void) NoteWifiScanStartedLocked();

        CARAPI_(void) NoteWifiScanStoppedLocked();

        CARAPI_(void) NoteWifiMulticastEnabledLocked();

        CARAPI_(void) NoteWifiMulticastDisabledLocked();

        CARAPI_(void) NoteAudioTurnedOnLocked();

        CARAPI_(void) NoteAudioTurnedOffLocked();

        CARAPI_(void) NoteVideoTurnedOnLocked();

        CARAPI_(void) NoteVideoTurnedOffLocked();

        CARAPI_(Int64) GetWifiRunningTime(
            /* [in] */ Int64 batteryRealtime,
            /* [in] */ Int32 which);

        CARAPI_(Int64) GetFullWifiLockTime(
            /* [in] */ Int64 batteryRealtime,
            /* [in] */ Int32 which);

        CARAPI_(Int64) GetWifiScanTime(
            /* [in] */ Int64 batteryRealtime,
            /* [in] */ Int32 which);

        CARAPI_(Int64) GetWifiMulticastTime(
            /* [in] */ Int64 batteryRealtime,
            /* [in] */ Int32 which);

        CARAPI_(Int64) GetAudioTurnedOnTime(
            /* [in] */ Int64 batteryRealtime,
            /* [in] */ Int32 which);

        CARAPI_(Int64) GetVideoTurnedOnTime(
            /* [in] */ Int64 batteryRealtime,
            /* [in] */ Int32 which);

        CARAPI_(void) NoteUserActivityLocked(
            /* [in] */ Int32 type);

        CARAPI_(Boolean) HasUserActivity();

        CARAPI_(Int32) GetUserActivityCount(
            /* [in] */ Int32 type,
            /* [in] */ Int32 which);

        CARAPI_(Int64) ComputeCurrentTcpBytesSent();

        /**
         * Retrieve the statistics object for a particular process, creating
         * if needed.
         */
        CARAPI_(AutoPtr<Proc>) GetProcessStatsLocked(
            /* [in] */ const String& name);

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

        CARAPI GetWakeTimerLocked(
            /* [in] */ const String& name,
            /* [in] */ Int32 type,
            /* [out] */ StopwatchTimer** timer);

        CARAPI_(AutoPtr<StopwatchTimer>) GetSensorTimerLocked(
            /* [in] */ Int32 sensor,
            /* [in] */ Boolean create);

        CARAPI_(void) NoteStartWakeLocked(
            /* [in] */ Int32 pid,
            /* [in] */ const String& name,
            /* [in] */ Int32 type,
            /* [in] */ Int64 elapsedRealtime);

        CARAPI_(void) NoteStopWakeLocked(
            /* [in] */ Int32 pid,
            /* [in] */ const String& name,
            /* [in] */ Int32 type);

        CARAPI_(void) ReportExcessiveWakeLocked(
            /* [in] */ const String& proc,
            /* [in] */ Int64 overTime,
            /* [in] */ Int64 usedTime);

        CARAPI_(void) ReportExcessiveCpuLocked(
            /* [in] */ const String& proc,
            /* [in] */ Int64 overTime,
            /* [in] */ Int64 usedTime);

        CARAPI_(void) NoteStartSensor(
            /* [in] */ Int32 sensor);

        CARAPI_(void) NoteStopSensor(
            /* [in] */ Int32 sensor);

        CARAPI_(void) NoteStartGps();

        CARAPI_(void) NoteStopGps();

        CARAPI_(AutoPtr<BatteryStatsImpl>) GetBatteryStats();

    private:
        CARAPI_(void) InitUserActivityLocked();

        /**
         * Clear all stats for this uid.  Returns true if the uid is completely
         * inactive so can be dropped.
         */
        CARAPI_(Boolean) Reset();

        CARAPI_(void) WriteToParcelLocked(
            /* [in] */ IParcel* out,
            /* [in] */ Int64 batteryRealtime);

        CARAPI_(void) ReadFromParcelLocked(
            /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
            /* [in] */ IParcel* in);

    private:
        Int32 mUid;
        Int64 mLoadedTcpBytesReceived;
        Int64 mLoadedTcpBytesSent;
        Int64 mCurrentTcpBytesReceived;
        Int64 mCurrentTcpBytesSent;
        Int64 mTcpBytesReceivedAtLastUnplug;
        Int64 mTcpBytesSentAtLastUnplug;

        // These are not saved/restored when parcelling, since we want
        // to return from the parcel with a snapshot of the state.
        Int64 mStartedTcpBytesReceived;
        Int64 mStartedTcpBytesSent;

        Boolean mWifiRunning;
        AutoPtr<StopwatchTimer> mWifiRunningTimer;

        Boolean mFullWifiLockOut;
        AutoPtr<StopwatchTimer> mFullWifiLockTimer;

        Boolean mWifiScanStarted;
        AutoPtr<StopwatchTimer> mWifiScanTimer;

        Boolean mWifiMulticastEnabled;
        AutoPtr<StopwatchTimer> mWifiMulticastTimer;

        Boolean mAudioTurnedOn;
        AutoPtr<StopwatchTimer> mAudioTurnedOnTimer;

        Boolean mVideoTurnedOn;
        AutoPtr<StopwatchTimer> mVideoTurnedOnTimer;

        AutoPtr< ArrayOf<Counter*> > mUserActivityCounters;

        /**
         * The statistics we have collected for this uid's wake locks.
         */
        HashMap<String, AutoPtr<Wakelock> > mWakelockStats;

        /**
         * The statistics we have collected for this uid's sensor activations.
         */
        HashMap<Int32, AutoPtr<Sensor> > mSensorStats;

        /**
         * The statistics we have collected for this uid's processes.
         */
        HashMap<String, AutoPtr<Proc> > mProcessStats;

        /**
         * The statistics we have collected for this uid's processes.
         */
        HashMap<String, AutoPtr<Pkg> > mPackageStats;

        /**
         * The transient wake stats we have collected for this uid's pids.
         */
        HashMap<Int32, AutoPtr<Pid> > mPids;

        BatteryStatsImpl* mHost;

        friend class BatteryStatsImpl;
    };

    class BatteryStatsWriteRunnable
        : public ElRefBase
        , public IRunnable
    {
    public:
        BatteryStatsWriteRunnable(
            /* [in] */ BatteryStatsImpl* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

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

public:
    BatteryStatsImpl();

    BatteryStatsImpl(
        /* [in] */ const String& filename);

    BatteryStatsImpl(
    /* [in] */ IParcel* p);

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI_(void) Lock();

    CARAPI_(void) Unlock();

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
        /* [in] */ Int32 uid);

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

    CARAPI_(Int64) GetScreenOnTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which);

    CARAPI_(Int32) GetScreenOnCount(
        /* [in] */ Int32 which);

    CARAPI_(Int64) GetScreenOnTime(
        /* [in] */ Int64 batteryRealtime,
        /* [in] */ Int32 which);

    CARAPI_(Int64) GetScreenBrightnessTime(
        /* [in] */ Int32 brightnessBin,
        /* [in] */ Int64 batteryRealtime,
        /* [in] */ Int32 which);

    CARAPI_(Int32) GetInputEventCount(
        /* [in] */ Int32 which);

    CARAPI_(Int64) GetPhoneOnTime(
        /* [in] */ Int64 batteryRealtime,
        /* [in] */ Int32 which);

    CARAPI_(Int64) GetPhoneSignalStrengthTime(
        /* [in] */ Int32 strengthBin,
        /* [in] */ Int64 batteryRealtime,
        /* [in] */ Int32 which);

    CARAPI_(Int64) GetPhoneSignalScanningTime(
        /* [in] */ Int64 batteryRealtime,
        /* [in] */ Int32 which);

    CARAPI_(Int32) GetPhoneSignalStrengthCount(
        /* [in] */ Int32 strengthBin,
        /* [in] */ Int32 which);

    CARAPI_(Int64) GetPhoneDataConnectionTime(
        /* [in] */ Int32 dataType,
        /* [in] */ Int64 batteryRealtime,
        /* [in] */ Int32 which);

    CARAPI_(Int32) GetPhoneDataConnectionCount(
        /* [in] */ Int32 dataType,
        /* [in] */ Int32 which);

    CARAPI_(Int64) GetWifiOnTime(
        /* [in] */ Int64 batteryRealtime,
        /* [in] */ Int32 which);

    CARAPI_(Int64) GetGlobalWifiRunningTime(
        /* [in] */ Int64 batteryRealtime,
        /* [in] */ Int32 which);

    CARAPI_(Int64) GetBluetoothOnTime(
        /* [in] */ Int64 batteryRealtime,
        /* [in] */ Int32 which);

    CARAPI_(Boolean) GetIsOnBattery();

    AutoPtr< HashMap<Int32, AutoPtr<BatteryStatsUid> > > GetUidStats();

    CARAPI_(void) SetCallback(
        /* [in] */ BatteryCallback* cb);

    CARAPI_(void) SetNumSpeedSteps(
        /* [in] */ Int32 steps);

    CARAPI_(void) SetRadioScanningTimeout(
        /* [in] */ Int64 timeout);

    CARAPI_(Boolean) StartIteratingOldHistoryLocked();

    CARAPI_(Boolean) GetNextOldHistoryLocked(
        /* [in] */ HistoryItem* out);

    CARAPI_(void) FinishIteratingOldHistoryLocked();

    CARAPI_(Boolean) StartIteratingHistoryLocked();

    CARAPI_(Boolean) GetNextHistoryLocked(
        /* [in] */ HistoryItem* out);

    CARAPI_(void) FinishIteratingHistoryLocked();

    CARAPI_(Int64) GetHistoryBaseTime();

    CARAPI_(Int32) GetStartCount();

    CARAPI_(Boolean) IsOnBattery();

    CARAPI_(Boolean) IsScreenOn();

    CARAPI_(void) ResetAllStatsLocked();

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

    CARAPI_(Int64) ComputeUptime(
        /* [in] */ Int64 curTime,
        /* [in] */ Int32 which);

    CARAPI_(Int64) ComputeRealtime(
        /* [in] */ Int64 curTime,
        /* [in] */ Int32 which);

    CARAPI_(Int64) ComputeBatteryUptime(
        /* [in] */ Int64 curTime,
        /* [in] */ Int32 which);

    CARAPI_(Int64) ComputeBatteryRealtime(
        /* [in] */ Int64 curTime,
        /* [in] */ Int32 which);

    CARAPI_(Int64) GetBatteryUptime(
        /* [in] */ Int64 curTime);

    CARAPI_(Int64) GetBatteryRealtime(
        /* [in] */ Int64 curTime);

    /** Only STATS_UNPLUGGED works properly */
    CARAPI_(Int64) GetMobileTcpBytesSent(
        /* [in] */ Int32 which);

    /** Only STATS_UNPLUGGED works properly */
    CARAPI_(Int64) GetMobileTcpBytesReceived(
        /* [in] */ Int32 which);

    /** Only STATS_UNPLUGGED works properly */
    CARAPI_(Int64) GetTotalTcpBytesSent(
        /* [in] */ Int32 which);

    /** Only STATS_UNPLUGGED works properly */
    CARAPI_(Int64) GetTotalTcpBytesReceived(
        /* [in] */ Int32 which);

    CARAPI_(Int32) GetDischargeStartLevel();

    CARAPI_(Int32) GetDischargeStartLevelLocked();

    CARAPI_(Int32) GetDischargeCurrentLevel();

    CARAPI_(Int32) GetDischargeCurrentLevelLocked();

    CARAPI_(Int32) GetLowDischargeAmountSinceCharge();

    CARAPI_(Int32) GetHighDischargeAmountSinceCharge();

    CARAPI_(Int32) GetDischargeAmountScreenOn();

    CARAPI_(Int32) GetDischargeAmountScreenOnSinceCharge();

    CARAPI_(Int32) GetDischargeAmountScreenOff();

    CARAPI_(Int32) GetDischargeAmountScreenOffSinceCharge();

    CARAPI_(Int32) GetCpuSpeedSteps();

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
     * Retrieve the statistics object for a particular process, given
     * the name of the process.
     * @param name process name
     * @return the statistics object for the process
     */
    CARAPI_(AutoPtr<Uid::Proc>) GetProcessStatsLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 pid);

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

    CARAPI_(void) InitTimes();

    CARAPI_(void) InitDischarge();

    CARAPI_(void) UpdateDischargeScreenLevelsLocked(
        /* [in] */ Boolean oldScreenOn,
        /* [in] */ Boolean newScreenOn);

    CARAPI_(void) SetOnBattery(
        /* [in] */ Boolean onBattery,
        /* [in] */ Int32 oldStatus,
        /* [in] */ Int32 level);

    CARAPI_(void) SetOnBatteryLocked(
        /* [in] */ Boolean onBattery,
        /* [in] */ Int32 oldStatus,
        /* [in] */ Int32 level);

    CARAPI_(Int64) GetBatteryUptimeLocked(
        /* [in] */ Int64 curTime);

    CARAPI_(Int64) GetBatteryUptimeLocked();

    CARAPI_(Int64) GetBatteryRealtimeLocked(
        /* [in] */ Int64 curTime);

    CARAPI_(Int64) GetTcpBytes(
        /* [in] */ Int64 current,
        /* [in] */ ArrayOf<Int64>* dataBytes,
        /* [in] */ Int32 which);

    CARAPI_(void) WriteLocked(
        /* [in] */ Boolean sync);

    static CARAPI ReadFully(
        /* [in] */ IFileInputStream* stream,
        /* [out] */ ArrayOf<Byte>** bytes);

    CARAPI_(void) ReadHistory(
        /* [in] */ IParcel* in,
        /* [in] */ Boolean andOldHistory);

    CARAPI_(void) ReadOldHistory(
        /* [in] */ IParcel* in);

    CARAPI_(void) WriteHistory(
        /* [in] */ IParcel* out,
        /* [in] */ Boolean andOldHistory);

    CARAPI_(void) WriteOldHistory(
        /* [in] */ IParcel* out);

    CARAPI_(void) ReadSummaryFromParcel(
        /* [in] */ IParcel* in);

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

    AutoPtr<IJournaledFile> mFile;

    AutoPtr<IBatteryCallback> mCallback;

    /**
     * Mapping isolated uids to the actual owning app uid.
     */
    HashMap<Int32, Int32> mIsolatedUids;

    /**
     * The statistics we have collected organized by uids.
     */
    HashMap<Int32, AutoPtr<Uid> > mUidStats;

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

    Int32 mMobileRadioPowerState = DataConnectionRealTimeInfo.DC_POWER_STATE_LOW;
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

    Object mLock;

    friend class OverflowArrayMap;
};


const String BatteryStatsImpl::OverflowArrayMap<T>::OVERFLOW_NAME("*overflow*");

template<typename T>
BatteryStatsImpl::OverflowArrayMap<T>::OverflowArrayMap(
    /* [in] */ BatteryStatsImpl* host)
    : mHost(host)
{}

template<typename T>
HashMap<String, AutoPtr<T> > BatteryStatsImpl::OverflowArrayMap<T>::GetMap()
{
    return mMap;
}

template<typename T>
void BatteryStatsImpl::OverflowArrayMap<T>::Clear()
{
    mMap.Clear();
    mCurOverflow = NULL;
    mActiveOverflow = NULL;
}

template<typename T>
void BatteryStatsImpl::OverflowArrayMap<T>::Add(
    /* [in] */ const String& name,
    /* [in] */ T* obj)
{
    mMap[name] = obj;
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
        HashMap<String, AutoPtr<T> >::Iterator it = mMap.Find(OVERFLOW_NAME);
        if (it != mMap.End()) {
            Slogger::W(TAG, "Cleaning up with no active overflow, but have overflow entry %s", TO_CSTR(it->mSecond));
            mMap.Erase(it);
        }
        mCurOverflow = NULL;
    }
    else {
        // There is currently active overflow, so we should still have an overflow entry.
        HashMap<String, AutoPtr<T> >::Iterator it = mMap.Find(OVERFLOW_NAME);
        if (mCurOverflow == NULL || it == mMap.End()) {
            Slogger::W(TAG, "Cleaning up with active overflow, but no overflow entry: cur=%s,  map=%s",
                    TO_CSTR(mCurOverflow), TO_CSTR(it->mSecond));
        }
    }
}

template<typename T>
AutoPtr<T> BatteryStatsImpl::OverflowArrayMap<T>::StartObject(
    /* [in] */ const String& name)
{
    AutoPtr<T> obj;
    HashMap<String, AutoPtr<T> >::Iterator it = mMap.Find(name);
    if (it != mMap.End()) {
        obj = it->mSecond;
    }
    if (obj != NULL) {
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
                mMap[OVERFLOW_NAME] = obj;
            }
            overflowIt->mSecond++;
            return obj;
        }
    }

    // No object exists for given name nor in the overflow; we need to make
    // a new one.
    Int32 N = mMap.GetSize();
    if (N >= BatteryStatsImpl::MAX_WAKELOCKS_PER_UID) {
        // Went over the limit on number of objects to track; this one goes
        // in to the overflow.
        obj = mCurOverflow;
        if (obj == NULL) {
            // Need to start overflow now...
            obj = mCurOverflow = InstantiateObject();
            mMap[OVERFLOW_NAME] = obj;
        }
        if (mActiveOverflow == NULL) {
            mActiveOverflow = new HashMap<String, Int32>();
        }
        (*mActiveOverflow)[name] = 1;
        return obj;
    }

    // Normal case where we just need to make a new object.
    obj = InstantiateObject();
    mMap[name] = obj;
    return obj;
}

template<typename T>
AutoPtr<T> BatteryStatsImpl::OverflowArrayMap<T>::StopObject(
    /* [in] */ const String& name)
{
    AutoPtr<T> obj;
    HashMap<String, AutoPtr<T> >::Iterator it = mMap.Find(name);
    if (it != mMap.End()) {
        obj = it->mSecond;
    }
    if (obj != NULL) {
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
    Slogger::W(TAG, "Unable to find object for %s mapsize=%d activeoverflow=%p curoverflow=%s",
            name.string(), mMap.GetSize(), mActiveOverflow.Get(), TO_CSTR(mCurOverflow));
    return NULL;
}


} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSIMPL_H__
