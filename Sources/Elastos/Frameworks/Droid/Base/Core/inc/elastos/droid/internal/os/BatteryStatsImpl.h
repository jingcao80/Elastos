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

#ifndef __ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSIMPL_H__
#define __ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSIMPL_H__

#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Net.h"
#include <Elastos.CoreLibrary.Utility.Concurrent.h>
#include "elastos/droid/os/BatteryStats.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/utility/CArrayMap.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Runnable.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Utility::IAtomicFile;
using Elastos::Droid::Bluetooth::IBluetoothHeadset;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IWorkSource;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::BatteryStats;
using Elastos::Droid::Os::IBatteryStatsCounter;
using Elastos::Droid::Os::IBatteryStatsInt64Counter;
using Elastos::Droid::Os::IBatteryStatsHistoryItem;
using Elastos::Droid::Os::IBatteryStatsUidWakelock;
using Elastos::Droid::Os::IBatteryStatsUidSensor;
using Elastos::Droid::Os::IBatteryStatsUidPkgServ;
using Elastos::Droid::Os::IBatteryStatsTimer;
using Elastos::Droid::Os::IBatteryStatsUidPkg;
using Elastos::Droid::Os::IBatteryStatsUidProc;
using Elastos::Droid::Os::IBatteryStatsUidProcExcessivePower;
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
using Elastos::IO::IFile;
using Elastos::Utility::IMap;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
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
        TO_STRING_IMPL("BatteryStatsImpl::TimeBase")

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
        TO_STRING_IMPL("BatteryStatsImpl::Counter")

        CAR_INTERFACE_DECL()

        Counter(
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        Counter(
            /* [in] */ TimeBase* timeBase);

        virtual ~Counter() {}

        virtual CARAPI_(void) WriteToParcel(
            /* [in] */ IParcel* out);

        virtual CARAPI OnTimeStarted(
            /* [in] */ Int64 elapsedRealtime,
            /* [in] */ Int64 baseUptime,
            /* [in] */ Int64 baseRealtime);

        virtual CARAPI OnTimeStopped(
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

        virtual CARAPI LogState(
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
        TO_STRING_IMPL("BatteryStatsImpl::SamplingCounter")

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
        TO_STRING_IMPL("BatteryStatsImpl::Int64SamplingCounter")

        CAR_INTERFACE_DECL()

        Int64SamplingCounter(
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        Int64SamplingCounter(
            /* [in] */ TimeBase* timeBase);

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
        TO_STRING_IMPL("BatteryStatsImpl::Timer")

        CAR_INTERFACE_DECL()

        Timer();

        /**
         * Constructs from a parcel.
         * @param type
         * @param timeBase
         * @param in
         */
        CARAPI constructor(
            /* [in] */ Int32 type,
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        CARAPI constructor(
            /* [in] */ Int32 type,
            /* [in] */ TimeBase* timeBase);

        virtual ~Timer() {}

        virtual CARAPI_(void) WriteToParcel(
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
            /* [out] */ Int32* count);

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

        friend class BatteryStatsImpl;
    };

    class SamplingTimer : public Timer
    {
    public:
        TO_STRING_IMPL("BatteryStatsImpl::SamplingTimer")

        SamplingTimer();

        CARAPI constructor(
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        CARAPI constructor(
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
    class BatchTimer : public Timer
    {
    public:
        TO_STRING_IMPL("BatteryStatsImpl::BatchTimer")

        BatchTimer();

        CARAPI constructor(
            /* [in] */ Uid* uid,
            /* [in] */ Int32 type,
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        CARAPI constructor(
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

        friend class Uid;
    };

    /**
     * State for keeping track of timing information.
     */
    class StopwatchTimer : public Timer
    {
    public:
        TO_STRING_IMPL("BatteryStatsImpl::StopwatchTimer")

        StopwatchTimer();

        CARAPI constructor(
            /* [in] */ Uid* uid,
            /* [in] */ Int32 type,
            /* [in] */ List<AutoPtr<StopwatchTimer> >* timerPool,
            /* [in] */ TimeBase* timeBase,
            /* [in] */ IParcel* in);

        CARAPI constructor(
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
        Uid* mUid;
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
        TO_STRING_IMPL("BatteryStatsImpl::OverflowArrayMap")

        OverflowArrayMap();

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
            TO_STRING_IMPL("BatteryStatsImpl::Uid::Wakelock")

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
                /* [out] */ StopwatchTimer** timer);

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
                /* [in] */ TimeBase* timeBase,
                /* [in] */ IParcel* in);

            CARAPI_(Boolean) Reset();

            CARAPI_(void) ReadFromParcelLocked(
                /* [in] */ TimeBase* timeBase,
                /* [in] */ TimeBase* screenOffTimeBase,
                /* [in] */ IParcel* in);

            CARAPI_(void) WriteToParcelLocked(
                /* [in] */ IParcel* out,
                /* [in] */ Int64 elapsedRealtimeUs);

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

        class Sensor
            : public Object
            , public IBatteryStatsUidSensor
        {
        public:
            TO_STRING_IMPL("BatteryStatsImpl::Uid::Sensor")

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
            CARAPI_(AutoPtr<StopwatchTimer>) ReadTimerFromParcel(
                /* [in] */ TimeBase* timeBase,
                /* [in] */ IParcel* in);

            CARAPI_(Boolean) Reset();

            CARAPI_(void) ReadFromParcelLocked(
                /* [in] */ TimeBase* timeBase,
                /* [in] */ IParcel* in);

            CARAPI_(void) WriteToParcelLocked(
                /* [in] */ IParcel* out,
                /* [in] */ Int64 elapsedRealtimeUs);

        private:
            Int32 mHandle;
            AutoPtr<StopwatchTimer> mTimer;
            Uid* mHost;

            friend class Uid;
            friend class BatteryStatsImpl;
        };

        class Proc
            : public Object
            , public IBatteryStatsImplUidProc
            , public IBatteryStatsUidProc
            , public ITimeBaseObs
        {
        public:
            TO_STRING_IMPL("BatteryStatsImpl::Uid::Proc")

            CAR_INTERFACE_DECL()

            Proc(
                /* [in] */ const String& name,
                /* [in] */ BatteryStatsImpl* host);

            CARAPI OnTimeStarted(
                /* [in] */ Int64 elapsedRealtime,
                /* [in] */ Int64 baseUptime,
                /* [in] */ Int64 baseRealtime);

            CARAPI OnTimeStopped(
                /* [in] */ Int64 elapsedRealtime,
                /* [in] */ Int64 baseUptime,
                /* [in] */ Int64 baseRealtime);

            CARAPI CountExcessivePowers(
                /* [out] */ Int32* powers);

            CARAPI GetExcessivePower(
                /* [in] */ Int32 i,
                /* [out] */ IBatteryStatsUidProcExcessivePower** excessivePower);

            CARAPI AddExcessiveWake(
                /* [in] */ Int64 overTime,
                /* [in] */ Int64 usedTime);

            CARAPI AddExcessiveCpu(
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
            CARAPI_(void) Reset();

            CARAPI_(void) Detach();

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

            AutoPtr< ArrayOf<SamplingCounter*> > mSpeedBins;

            AutoPtr<List<AutoPtr<ExcessivePower> > > mExcessivePower;

            BatteryStatsImpl* mHost;

            friend class Uid;
            friend class BatteryStatsImpl;
        };

        class Pkg
            : public Object
            , public IBatteryStatsUidPkg
            , public IBatteryStatsImplUidPkg
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
                TO_STRING_IMPL("BatteryStatsImpl::Uid::Pkg::Serv")

                CAR_INTERFACE_DECL()

                Serv();

                CARAPI constructor(
                    /* [in] */ BatteryStatsImpl* host);

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

                friend class Pkg;
                friend class Uid;
                friend class BatteryStatsImpl;
            };

        public:
            TO_STRING_IMPL("BatteryStatsImpl::Uid::Pkg")

            CAR_INTERFACE_DECL()

            Pkg(
                /* [in] */ BatteryStatsImpl* host);

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

            CARAPI IncWakeupsLocked();

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

            friend class Uid;
            friend class BatteryStatsImpl;
        };

    private:
        class WakelockStats : public OverflowArrayMap<Wakelock>
        {
        public:
            TO_STRING_IMPL("BatteryStatsImpl::Uid::WakelockStats")

            WakelockStats(
                /* [in] */ Uid* uid)
                : mHost(uid)
            {}

            CARAPI_(AutoPtr<Wakelock>) InstantiateObject();

        private:
            Uid* mHost;
        };

        class SyncStats : public OverflowArrayMap<StopwatchTimer>
        {
        public:
            TO_STRING_IMPL("BatteryStatsImpl::Uid::SyncStats")

            SyncStats(
                /* [in] */ Uid* uid)
                : mHost(uid)
            {}

            CARAPI_(AutoPtr<StopwatchTimer>) InstantiateObject();

        private:
            Uid* mHost;
        };

        class JobStats : public OverflowArrayMap<StopwatchTimer>
        {
        public:
            TO_STRING_IMPL("BatteryStatsImpl::Uid::JobStats")

            JobStats(
                /* [in] */ Uid* uid)
                : mHost(uid)
            {}

            CARAPI_(AutoPtr<StopwatchTimer>) InstantiateObject();

        private:
            Uid* mHost;
        };

    public:
        TO_STRING_IMPL("BatteryStatsImpl::Uid")

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

        CARAPI GetPidStats(
            /* [out] */ ISparseArray** stats);

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

        CARAPI_(AutoPtr<StopwatchTimer>) GetSensorTimerLocked(
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
        AutoPtr<StopwatchTimer> mWifiRunningTimer;

        Boolean mFullWifiLockOut;
        AutoPtr<StopwatchTimer> mFullWifiLockTimer;

        Boolean mWifiScanStarted;
        AutoPtr<StopwatchTimer> mWifiScanTimer;

        Int32 mWifiBatchedScanBinStarted;
        AutoPtr<ArrayOf<StopwatchTimer*> > mWifiBatchedScanTimer;

        Boolean mWifiMulticastEnabled;
        AutoPtr<StopwatchTimer> mWifiMulticastTimer;

        AutoPtr<StopwatchTimer> mAudioTurnedOnTimer;
        AutoPtr<StopwatchTimer> mVideoTurnedOnTimer;

        AutoPtr<StopwatchTimer> mForegroundActivityTimer;

        Int32 mProcessState;
        AutoPtr<ArrayOf<StopwatchTimer*> > mProcessStateTimer;

        AutoPtr<BatchTimer> mVibratorOnTimer;

        AutoPtr< ArrayOf<Counter*> > mUserActivityCounters;

        AutoPtr<ArrayOf<Int64SamplingCounter*> > mNetworkByteActivityCounters;
        AutoPtr<ArrayOf<Int64SamplingCounter*> > mNetworkPacketActivityCounters;
        AutoPtr<Int64SamplingCounter> mMobileRadioActiveTime;
        AutoPtr<Int64SamplingCounter> mMobileRadioActiveCount;

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
        AutoPtr<JobStats> mJobStats;

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
        TO_STRING_IMPL("BatteryStatsImpl::BatteryStatsWriteRunnable")

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
        TO_STRING_IMPL("BatteryStatsImpl::KernelWakelockStats")

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
        TO_STRING_IMPL("BatteryStatsImpl::SetOnBatteryRunnable")

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
    TO_STRING_IMPL("BatteryStatsImpl")

    CAR_INTERFACE_DECL()

    BatteryStatsImpl();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IFile* systemDir,
        /* [in] */ IHandler* handler);

    CARAPI GetKernelWakelockStats(
        /* [out] */ IMap** stats);

    CARAPI GetWakeupReasonStats(
        /* [out] */ IMap** stats);

    /*
     * Get the wakeup reason counter, and create a new one if one
     * doesn't already exist.
     */
    CARAPI_(AutoPtr<SamplingTimer>) GetWakeupReasonTimerLocked(
        /* [in] */ const String& name);

    /*
     * Get the KernelWakelockTimer associated with name, and create a new one if one
     * doesn't already exist.
     */
    CARAPI_(AutoPtr<SamplingTimer>) GetKernelWakelockTimerLocked(
        /* [in] */ const String& name);

    CARAPI GetBluetoothPingCount(
        /* [out] */ Int32* count);

    CARAPI SetBtHeadset(
        /* [in] */ IBluetoothHeadset* headset);

    CARAPI_(void) WriteHistoryDelta(
        /* [in] */ IParcel* dest,
        /* [in] */ HistoryItem* cur,
        /* [in] */ HistoryItem* last);

    CARAPI_(void) ReadHistoryDelta(
        /* [in] */ IParcel* src,
        /* [in] */ HistoryItem* cur);

    CARAPI CommitCurrentHistoryBatchLocked();

    CARAPI_(void) UpdateTimeBasesLocked(
        /* [in] */ Boolean unplugged,
        /* [in] */ Boolean screenOff,
        /* [in] */ Int64 uptime,
        /* [in] */ Int64 realtime);

    CARAPI AddIsolatedUidLocked(
        /* [in] */ Int32 isolatedUid,
        /* [in] */ Int32 appUid);

    CARAPI RemoveIsolatedUidLocked(
        /* [in] */ Int32 isolatedUid,
        /* [in] */ Int32 appUid);

    CARAPI MapUid(
        /* [in] */ Int32 uid,
        /* [out] */ Int32* newUid);

    CARAPI NoteEventLocked(
        /* [in] */ Int32 code,
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI NoteCurrentTimeChangedLocked();

    CARAPI NoteProcessStartLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI NoteProcessStateLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid,
        /* [in] */ Int32 state);

    CARAPI NoteProcessFinishLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI NoteSyncStartLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI NoteSyncFinishLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI NoteJobStartLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI NoteJobFinishLocked(
        /* [in] */ const String& name,
        /* [in] */ Int32 uid);

    CARAPI SetRecordAllHistoryLocked(
        /* [in] */ Boolean enabled);

    CARAPI SetNoAutoReset(
        /* [in] */ Boolean enabled);

    CARAPI NoteStartWakeLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ const String& historyName,
        /* [in] */ Int32 type,
        /* [in] */ Boolean unimportantForLogging,
        /* [in] */ Int64 elapsedRealtime,
        /* [in] */ Int64 uptime);

    CARAPI NoteStopWakeLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ const String& historyName,
        /* [in] */ Int32 type,
        /* [in] */ Int64 elapsedRealtime,
        /* [in] */ Int64 uptime);

    CARAPI NoteChangeWakelockFromSourceLocked(
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

    CARAPI NoteStartWakeFromSourceLocked(
        /* [in] */ IWorkSource* ws,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ const String& historyName,
        /* [in] */ Int32 type,
        /* [in] */ Boolean unimportantForLogging);

    CARAPI NoteStopWakeFromSourceLocked(
        /* [in] */ IWorkSource* ws,
        /* [in] */ Int32 pid,
        /* [in] */ const String& name,
        /* [in] */ const String& historyName,
        /* [in] */ Int32 type);

    CARAPI NoteWakeupReasonLocked(
        /* [in] */ const String& reason);

    CARAPI StartAddingCpuLocked(
        /* [out] */ Int32* result);

    CARAPI FinishAddingCpuLocked(
        /* [in] */ Int32 perc,
        /* [in] */ Int32 utime,
        /* [in] */ Int32 stime,
        /* [in] */ ArrayOf<Int64>* cpuSpeedTimes);

    CARAPI NoteProcessDiedLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid);

    CARAPI GetProcessWakeTime(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 pid,
        /* [in] */ Int64 realtime,
        /* [out] */ Int64* result);

    CARAPI ReportExcessiveWakeLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& proc,
        /* [in] */ Int64 overTime,
        /* [in] */ Int64 usedTime);

    CARAPI ReportExcessiveCpuLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& proc,
        /* [in] */ Int64 overTime,
        /* [in] */ Int64 usedTime);

    CARAPI NoteStartSensorLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 sensor);

    CARAPI NoteStopSensorLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 sensor);

    CARAPI NoteStartGpsLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteStopGpsLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteScreenStateLocked(
        /* [in] */ Int32 state);

    CARAPI NoteScreenBrightnessLocked(
        /* [in] */ Int32 brightness);

    CARAPI NoteUserActivityLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 event);

    CARAPI NoteInteractiveLocked(
        /* [in] */ Boolean interactive);

    CARAPI NoteMobileRadioPowerState(
        /* [in] */ Int32 powerState,
        /* [in] */ Int64 timestampNs);

    CARAPI NoteLowPowerMode(
        /* [in] */ Boolean enabled);

    CARAPI NotePhoneOnLocked();

    CARAPI NotePhoneOffLocked();

    /**
     * Telephony stack updates the phone state.
     * @param state phone state from ServiceState.getState()
     */
    CARAPI NotePhoneStateLocked(
        /* [in] */ Int32 state,
        /* [in] */ Int32 simState);

    CARAPI NotePhoneSignalStrengthLocked(
        /* [in] */ ISignalStrength* signalStrength);

    CARAPI NotePhoneDataConnectionStateLocked(
        /* [in] */ Int32 dataType,
        /* [in] */ Boolean hasData);

    CARAPI NoteWifiOnLocked();

    CARAPI NoteWifiOffLocked();

    CARAPI NoteAudioOnLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteAudioOffLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteVideoOnLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteVideoOffLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteResetAudioLocked();

    CARAPI NoteResetVideoLocked();

    CARAPI NoteActivityResumedLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteActivityPausedLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteVibratorOnLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int64 durationMillis);

    CARAPI NoteVibratorOffLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteFlashlightOnLocked();

    CARAPI NoteFlashlightOffLocked();

    CARAPI NoteWifiRunningLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiRunningChangedLocked(
        /* [in] */ IWorkSource* oldWs,
        /* [in] */ IWorkSource* newWs);

    CARAPI NoteWifiStoppedLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiStateLocked(
        /* [in] */ Int32 wifiState,
        /* [in] */ const String& accessPoint);

    CARAPI NoteWifiSupplicantStateChangedLocked(
        /* [in] */ Int32 supplState,
        /* [in] */ Boolean failedAuth);

    CARAPI NoteWifiRssiChangedLocked(
        /* [in] */ Int32 newRssi);

    CARAPI NoteBluetoothOnLocked();

    CARAPI NoteBluetoothOffLocked();

    CARAPI NoteBluetoothStateLocked(
        /* [in] */ Int32 bluetoothState);

    CARAPI NoteFullWifiLockAcquiredLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteFullWifiLockReleasedLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteWifiScanStartedLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteWifiScanStoppedLocked(
        /* [in] */ Int32 uid);

    CARAPI_(void) NoteWifiBatchedScanStartedLocked(
        /* [in] */ Int32 uid,
        /* [in] */ Int32 csph);

    CARAPI_(void) NoteWifiBatchedScanStoppedLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteWifiMulticastEnabledLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteWifiMulticastDisabledLocked(
        /* [in] */ Int32 uid);

    CARAPI NoteFullWifiLockAcquiredFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteFullWifiLockReleasedFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiScanStartedFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiScanStoppedFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiBatchedScanStartedFromSourceLocked(
        /* [in] */ IWorkSource* ws,
        /* [in] */ Int32 csph);

    CARAPI NoteWifiBatchedScanStoppedFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiMulticastEnabledFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteWifiMulticastDisabledFromSourceLocked(
        /* [in] */ IWorkSource* ws);

    CARAPI NoteNetworkInterfaceTypeLocked(
        /* [in] */ const String& iface,
        /* [in] */ Int32 networkType);

    CARAPI NoteNetworkStatsEnabledLocked();

    // @Override
    CARAPI GetScreenOnTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetScreenOnCount(
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetScreenBrightnessTime(
        /* [in] */ Int32 brightnessBin,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetInteractiveTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetLowPowerModeEnabledTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetLowPowerModeEnabledCount(
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetPhoneOnTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetPhoneOnCount(
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetPhoneSignalStrengthTime(
        /* [in] */ Int32 strengthBin,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetPhoneSignalScanningTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetPhoneSignalStrengthCount(
        /* [in] */ Int32 strengthBin,
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetPhoneDataConnectionTime(
        /* [in] */ Int32 dataType,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetPhoneDataConnectionCount(
        /* [in] */ Int32 dataType,
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetMobileRadioActiveTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetMobileRadioActiveCount(
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetMobileRadioActiveAdjustedTime(
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetMobileRadioActiveUnknownTime(
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetMobileRadioActiveUnknownCount(
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetWifiOnTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetGlobalWifiRunningTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetWifiStateTime(
        /* [in] */ Int32 wifiState,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetWifiStateCount(
        /* [in] */ Int32 wifiState,
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetWifiSupplStateTime(
        /* [in] */ Int32 state,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetWifiSupplStateCount(
        /* [in] */ Int32 state,
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetWifiSignalStrengthTime(
        /* [in] */ Int32 strengthBin,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetWifiSignalStrengthCount(
        /* [in] */ Int32 strengthBin,
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetBluetoothOnTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetBluetoothStateTime(
        /* [in] */ Int32 bluetoothState,
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetBluetoothStateCount(
        /* [in] */ Int32 bluetoothState,
        /* [in] */ Int32 which,
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetFlashlightOnTime(
        /* [in] */ Int64 elapsedRealtimeUs,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetFlashlightOnCount(
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetNetworkActivityBytes(
        /* [in] */ Int32 type,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetNetworkActivityPackets(
        /* [in] */ Int32 type,
        /* [in] */ Int32 which,
        /* [out] */ Int64* result);

    CARAPI_(Boolean) IsStartClockTimeValid();

    // @Override
    CARAPI GetStartClockTime(
        /* [out] */ Int64* result);

    // @Override
    CARAPI GetStartPlatformVersion(
        /* [out] */ String* version);

    // @Override
    CARAPI GetEndPlatformVersion(
        /* [out] */ String* version);

    // @Override
    CARAPI GetParcelVersion(
        /* [out] */ Int32* result);

    // @Override
    CARAPI GetUidStats(
        /* [out] */ ISparseArray** stats);

    CARAPI SetCallback(
        /* [in] */ IBatteryCallback* cb);

    CARAPI SetNumSpeedSteps(
        /* [in] */ Int32 steps);

    CARAPI SetRadioScanningTimeout(
        /* [in] */ Int64 timeout);

    CARAPI StartIteratingOldHistoryLocked(
        /* [out] */ Boolean* result);

    CARAPI GetNextOldHistoryLocked(
        /* [in] */ IBatteryStatsHistoryItem* out,
        /* [out] */ Boolean* result);

    CARAPI FinishIteratingOldHistoryLocked();

    CARAPI GetHistoryTotalSize(
        /* [out] */ Int32* size);

    CARAPI GetHistoryUsedSize(
        /* [out] */ Int32* size);

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

    CARAPI IsOnBattery(
        /* [out] */ Boolean* result);

    CARAPI IsScreenOn(
        /* [out] */ Boolean* result);

    CARAPI ResetAllStatsCmdLocked();

    CARAPI_(void) PullPendingStateUpdatesLocked();

    CARAPI SetBatteryState(
        /* [in] */ Int32 status,
        /* [in] */ Int32 health,
        /* [in] */ Int32 plugType,
        /* [in] */ Int32 level,
        /* [in] */ Int32 temp,
        /* [in] */ Int32 volt);

    CARAPI_(void) UpdateKernelWakelocksLocked();

    CARAPI GetAwakeTimeBattery(
        /* [out] */ Int64* result);

    CARAPI GetAwakeTimePlugged(
        /* [out] */ Int64* result);

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

    CARAPI GetNumDischargeStepDurations(
        /* [out] */ Int32* result);

    CARAPI GetDischargeStepDurationsArray(
        /* [out, callee] */ ArrayOf<Int64>** array);

    CARAPI ComputeChargeTimeRemaining(
        /* [in] */ Int64 curTime,
        /* [out] */ Int64* result);

    CARAPI GetNumChargeStepDurations(
        /* [out] */ Int32* result);

    CARAPI GetChargeStepDurationsArray(
        /* [out, callee] */ ArrayOf<Int64>** array);

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

    CARAPI GetDischargeAmountScreenOn(
        /* [out] */ Int32* result);

    CARAPI GetDischargeAmountScreenOnSinceCharge(
        /* [out] */ Int32* result);

    CARAPI GetDischargeAmountScreenOff(
        /* [out] */ Int32* result);

    CARAPI GetDischargeAmountScreenOffSinceCharge(
        /* [out] */ Int32* result);

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
    CARAPI RemoveUidStatsLocked(
        /* [in] */ Int32 uid);

    /**
     * Retrieve the statistics object for a particular process, creating
     * if needed.
     */
    CARAPI GetProcessStatsLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& name,
        /* [out] */ IBatteryStatsImplUidProc** proc);

    /**
     * Retrieve the statistics object for a particular process, creating
     * if needed.
     */
    CARAPI GetPackageStatsLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& pkg,
        /* [out] */ IBatteryStatsImplUidPkg** pkgStats);

    /**
     * Retrieve the statistics object for a particular service, creating
     * if needed.
     */
    CARAPI GetServiceStatsLocked(
        /* [in] */ Int32 uid,
        /* [in] */ const String& pkg,
        /* [in] */ const String& name,
        /* [out] */ IBatteryStatsImplUidPkgServ** serv);

    // /**
    //  * Massage data to distribute any reasonable work down to more specific
    //  * owners.  Must only be called on a dead BatteryStats object!
    //  */
    CARAPI_(void) DistributeWorkLocked(
        /* [in] */ Int32 which);

    CARAPI ShutdownLocked();

    CARAPI WriteAsyncLocked();

    CARAPI WriteSyncLocked();

    CARAPI_(void) CommitPendingDataToDisk();

    CARAPI ReadLocked();

    CARAPI ReadSummaryFromParcel(
        /* [in] */ IParcel* in);

    /**
     * Writes a summary of the statistics to a Parcel, in a format suitable to be written to
     * disk.  This format does not allow a lossless round-trip.
     *
     * @param out the Parcel to be written to.
     */
    CARAPI_(void) WriteSummaryToParcel(
        /* [in] */ IParcel* out,
        /* [in] */ Boolean inclHistory);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* in);

    CARAPI ReadFromParcelLocked(
        /* [in] */ IParcel* in);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* out);

    CARAPI WriteToParcelWithoutUids(
        /* [in] */ IParcel* out);

    CARAPI_(void) PrepareForDumpLocked();

    using BatteryStats::DumpLocked;

    CARAPI_(void) DumpLocked(
        /* [in] */ IContext* context,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ Int32 flags,
        /* [in] */ Int32 reqUid,
        /* [in] */ Int64 histStart);

private:
    CARAPI Init();

    CARAPI_(void) HandleUpdateWakelocks();

    CARAPI_(void) HandleReportPowerChange(
        /* [in] */ Boolean onBattery);

    HashMap<String, AutoPtr<KernelWakelockStats> >* ReadKernelWakelockStats();

    HashMap<String, AutoPtr<KernelWakelockStats> >* ParseProcWakelocks(
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

    CARAPI GetIsOnBattery(
        /* [out] */ Boolean* isOnBattery);

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
    static const Boolean DEBUG;
    static const Boolean DEBUG_HISTORY;
    static const Boolean USE_OLD_HISTORY;   // for debugging.

    // TODO: remove "tcp" from network methods, since we measure total stats.

    // In-memory Parcel magic number, used to detect attempts to unmarshall bad data
    static const Int32 MAGIC;

    // Current on-disk Parcel version
    static const Int32 VERSION;

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
    AutoPtr<List<AutoPtr<StopwatchTimer> > > mPartialTimers;
    AutoPtr<List<AutoPtr<StopwatchTimer> > > mFullTimers;
    AutoPtr<List<AutoPtr<StopwatchTimer> > > mWindowTimers;
    HashMap<Int32, AutoPtr< List<AutoPtr<StopwatchTimer> > > > mSensorTimers;
    AutoPtr<List<AutoPtr<StopwatchTimer> > > mWifiRunningTimers;
    AutoPtr<List<AutoPtr<StopwatchTimer> > > mFullWifiLockTimers;
    AutoPtr<List<AutoPtr<StopwatchTimer> > > mWifiMulticastTimers;
    AutoPtr<List<AutoPtr<StopwatchTimer> > > mWifiScanTimers;
    HashMap<Int32, AutoPtr<List<AutoPtr<StopwatchTimer> > > > mWifiBatchedScanTimers;
    AutoPtr<List<AutoPtr<StopwatchTimer> > > mAudioTurnedOnTimers;
    AutoPtr<List<AutoPtr<StopwatchTimer> > > mVideoTurnedOnTimers;

    // Last partial timers we use for distributing CPU usage.
    AutoPtr<List<AutoPtr<StopwatchTimer> > > mLastPartialTimers;

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
    AutoPtr<IHashMap> mKernelWakelockStats;

    String mLastWakeupReason;
    Int64 mLastWakeupUptimeMs;
    AutoPtr<IHashMap> mWakeupReasonStats;

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

    Int32 mSensorNesting;

    Int32 mGpsNesting;

    Int32 mWifiFullLockNesting;

    Int32 mWifiScanNesting;

    Int32 mWifiMulticastNesting;

    AutoPtr<IParcel> mPendingWrite;
    AutoPtr<IReentrantLock> mWriteLock;

    AutoPtr<INetworkStats> mNetworkSummaryCache;
    AutoPtr<INetworkStats> mNetworkDetailCache;
};


//==============================================================================
// BatteryStatsImpl::OverflowArrayMap
//==============================================================================

template<typename T>
const String BatteryStatsImpl::OverflowArrayMap<T>::OVERFLOW_NAME("*overflow*");

template<typename T>
BatteryStatsImpl::OverflowArrayMap<T>::OverflowArrayMap()
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
        if (mActiveOverflow->IsEmpty()) {
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
            AutoPtr<IInterface> value;
            mMap->Get(cs, (IInterface**)&value);
            AutoPtr<T> obj = (T*)(IObject*)value.Get();
            String str;
            obj->ToString(&str);
            Slogger::W(TAG, "Cleaning up with no active overflow, but have overflow entry %s", str.string());
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
            Slogger::W(TAG, "Cleaning up with active overflow, but no overflow entry: cur=%s,  map=",
                    TO_CSTR(mCurOverflow));
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

template <>
struct Conversion<Elastos::Droid::Internal::Os::BatteryStatsImpl::SamplingCounter*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

template <>
struct Conversion<Elastos::Droid::Internal::Os::BatteryStatsImpl::StopwatchTimer*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

template <>
struct Conversion<Elastos::Droid::Internal::Os::BatteryStatsImpl::Int64SamplingCounter*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

template <>
struct Conversion<Elastos::Droid::Internal::Os::BatteryStatsImpl::Counter*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif //__ELASTOS_DROID_INTERNAL_OS_BATTERYSTATSIMPL_H__
