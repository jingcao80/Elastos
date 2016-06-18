
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Telephony.h"
#include "elastos/droid/internal/os/BatteryStatsImpl.h"
#include "elastos/droid/internal/os/BatteryStatsHelper.h"
#include "elastos/droid/internal/os/CBackgroundThreadHelper.h"
#include "elastos/droid/internal/net/CNetworkStatsFactory.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/CJournaledFile.h"
#include "elastos/droid/net/CConnectivityManagerHelper.h"
#include "elastos/droid/net/CNetworkStats.h"
#include "elastos/droid/net/CNetworkStatsEntry.h"
#include "elastos/droid/net/CNetworkStatsHelper.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/TimeUtils.h"
#include "elastos/droid/utility/CAtomicFile.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/view/Display.h"
#include "elastos/droid/wifi/CWifiManagerHelper.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/AutoLock.h>

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Internal::Net::CNetworkStatsFactory;
using Elastos::Droid::Internal::Os::IBackgroundThreadHelper;
using Elastos::Droid::Internal::Os::CBackgroundThreadHelper;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::CJournaledFile;
using Elastos::Droid::Net::IConnectivityManagerHelper;
using Elastos::Droid::Net::CConnectivityManagerHelper;
using Elastos::Droid::Net::CNetworkStatsEntry;
using Elastos::Droid::Net::CNetworkStats;
using Elastos::Droid::Net::CNetworkStatsHelper;
using Elastos::Droid::Net::INetworkStatsHelper;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::EIID_IBatteryStatsCounter;
using Elastos::Droid::Os::EIID_IBatteryStatsInt64Counter;
using Elastos::Droid::Os::EIID_IBatteryStatsTimer;
using Elastos::Droid::Os::EIID_IBatteryStatsUidWakelock;
using Elastos::Droid::Os::EIID_IBatteryStatsUidSensor;
using Elastos::Droid::Os::EIID_IBatteryStatsUidPkgServ;
using Elastos::Droid::Os::EIID_IBatteryStatsUidPkg;
using Elastos::Droid::Os::EIID_IBatteryStats;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ISignalStrength;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Utility::ISparseInt32Array;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::View::Display;
using Elastos::Droid::Wifi::CWifiManagerHelper;
using Elastos::Droid::Wifi::IWifiManagerHelper;
using Elastos::Core::AutoLock;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IStringWriter;
using Elastos::IO::CStringWriter;
using Elastos::IO::CPrintWriter;
using Elastos::IO::IFlushable;
using Elastos::IO::IOutputStream;
using Elastos::IO::IInputStream;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IIterator;
using Elastos::Utility::IMapEntry;
using Elastos::Utility::ICollection;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

//==============================================================================
// BatteryStatsImpl::MyHandler
//==============================================================================

ECode BatteryStatsImpl::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);

    AutoPtr<IBatteryCallback> cb = mHost->mCallback;
    switch(what) {
        case BatteryStatsImpl::MSG_UPDATE_WAKELOCKS:
            if (cb != NULL) {
                cb->BatteryNeedsCpuUpdate();
            }
            break;
        case BatteryStatsImpl::MSG_REPORT_POWER_CHANGE:
        {
            Int32 arg1;
            msg->GetArg1(&arg1);
            if (cb != NULL) {
                cb->BatteryPowerChanged(arg1 != 0);
            }
            break;
        }
    }

    return NOERROR;
}


//==============================================================================
// BatteryStatsImpl::TimeBase
//==============================================================================

BatteryStatsImpl::TimeBase::TimeBase()
    : mUptime(0)
    , mRealtime(0)
    , mRunning(FALSE)
    , mPastUptime(0)
    , mUptimeStart(0)
    , mPastRealtime(0)
    , mRealtimeStart(0)
    , mUnpluggedUptime(0)
    , mUnpluggedRealtime(0)
{}

void BatteryStatsImpl::TimeBase::Dump(
    /* [in] */ IPrintWriter* pw,
    /* [in] */ const String& prefix)
{}

void BatteryStatsImpl::TimeBase::Add(
    /* [in] */ ITimeBaseObs* observer)
{
    mObservers.PushBack(observer);
}

void BatteryStatsImpl::TimeBase::Remove(
    /* [in] */ ITimeBaseObs* observer)
{
    // if (!mObservers.remove(observer)) {
    //     Slog.wtf(TAG, "Removed unknown observer: " + observer);
    // }
    mObservers.Remove(observer);
}

void BatteryStatsImpl::TimeBase::Init(
    /* [in] */ Int64 uptime,
    /* [in] */ Int64 realtime)
{
    mRealtime = 0;
    mUptime = 0;
    mPastUptime = 0;
    mPastRealtime = 0;
    mUptimeStart = uptime;
    mRealtimeStart = realtime;
    mUnpluggedUptime = GetUptime(mUptimeStart);
    mUnpluggedRealtime = GetRealtime(mRealtimeStart);
}

void BatteryStatsImpl::TimeBase::Reset(
    /* [in] */ Int64 uptime,
    /* [in] */ Int64 realtime)
{
    if (!mRunning) {
        mPastUptime = 0;
        mPastRealtime = 0;
    }
    else {
        mUptimeStart = uptime;
        mRealtimeStart = realtime;
        mUnpluggedUptime = GetUptime(uptime);
        mUnpluggedRealtime = GetRealtime(realtime);
    }
}

Int64 BatteryStatsImpl::TimeBase::ComputeUptime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which)
{
    switch (which) {
        case STATS_SINCE_CHARGED:
            return mUptime + GetUptime(curTime);
        case STATS_CURRENT:
            return GetUptime(curTime);
        case STATS_SINCE_UNPLUGGED:
            return GetUptime(curTime) - mUnpluggedUptime;
    }
    return 0;
}

Int64 BatteryStatsImpl::TimeBase::ComputeRealtime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which)
{
    switch (which) {
        case STATS_SINCE_CHARGED:
            return mRealtime + GetRealtime(curTime);
        case STATS_CURRENT:
            return GetRealtime(curTime);
        case STATS_SINCE_UNPLUGGED:
            return GetRealtime(curTime) - mUnpluggedRealtime;
    }
    return 0;
}

Int64 BatteryStatsImpl::TimeBase::GetUptime(
    /* [in] */ Int64 curTime)
{
    Int64 time = mPastUptime;
    if (mRunning) {
        time += curTime - mUptimeStart;
    }
    return time;
}

Int64 BatteryStatsImpl::TimeBase::GetRealtime(
    /* [in] */ Int64 curTime)
{
    Int64 time = mPastRealtime;
    if (mRunning) {
        time += curTime - mRealtimeStart;
    }
    return time;
}

Int64 BatteryStatsImpl::TimeBase::GetUptimeStart()
{
    return mUptimeStart;
}

Int64 BatteryStatsImpl::TimeBase::GetRealtimeStart()
{
    return mRealtimeStart;
}

Boolean BatteryStatsImpl::TimeBase::IsRunning()
{
    return mRunning;
}

Boolean BatteryStatsImpl::TimeBase::SetRunning(
    /* [in] */ Boolean running,
    /* [in] */ Int64 uptime,
    /* [in] */ Int64 realtime)
{
    if (mRunning != running) {
        mRunning = running;
        if (running) {
            mUptimeStart = uptime;
            mRealtimeStart = realtime;
            Int64 batteryUptime = mUnpluggedUptime = GetUptime(uptime);
            Int64 batteryRealtime = mUnpluggedRealtime = GetRealtime(realtime);

            List<AutoPtr<ITimeBaseObs> >::ReverseIterator rit = mObservers.RBegin();
            for (; rit != mObservers.REnd(); ++rit) {
                (*rit)->OnTimeStarted(realtime, batteryUptime, batteryRealtime);
            }
        }
        else {
            mPastUptime += uptime - mUptimeStart;
            mPastRealtime += realtime - mRealtimeStart;

            Int64 batteryUptime = GetUptime(uptime);
            Int64 batteryRealtime = GetRealtime(realtime);

            List<AutoPtr<ITimeBaseObs> >::ReverseIterator rit = mObservers.RBegin();
            for (; rit != mObservers.REnd(); ++rit) {
                (*rit)->OnTimeStopped(realtime, batteryUptime, batteryRealtime);
            }
        }
        return TRUE;
    }
    return FALSE;
}

void BatteryStatsImpl::TimeBase::ReadSummaryFromParcel(
    /* [in] */ IParcel* in)
{
    in->ReadInt64(&mUptime);
    in->ReadInt64(&mRealtime);
}

void BatteryStatsImpl::TimeBase::WriteSummaryToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 uptime,
    /* [in] */ Int64 realtime)
{
    out->WriteInt64(ComputeUptime(uptime, STATS_SINCE_CHARGED));
    out->WriteInt64(ComputeRealtime(realtime, STATS_SINCE_CHARGED));
}

void BatteryStatsImpl::TimeBase::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    mRunning = FALSE;
    in->ReadInt64(&mUptime);
    in->ReadInt64(&mPastUptime);
    in->ReadInt64(&mUptimeStart);
    in->ReadInt64(&mRealtime);
    in->ReadInt64(&mPastRealtime);
    in->ReadInt64(&mRealtimeStart);
    in->ReadInt64(&mUnpluggedUptime);
    in->ReadInt64(&mUnpluggedRealtime);
}

void BatteryStatsImpl::TimeBase::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 uptime,
    /* [in] */ Int64 realtime)
{
    Int64 runningUptime = GetUptime(uptime);
    Int64 runningRealtime = GetRealtime(realtime);
    out->WriteInt64(mUptime);
    out->WriteInt64(runningUptime);
    out->WriteInt64(mUptimeStart);
    out->WriteInt64(mRealtime);
    out->WriteInt64(runningRealtime);
    out->WriteInt64(mRealtimeStart);
    out->WriteInt64(mUnpluggedUptime);
    out->WriteInt64(mUnpluggedRealtime);
}


//==============================================================================
// BatteryStatsImpl::Counter
//==============================================================================

BatteryStatsImpl::Counter::Counter(
    /* [in] */ TimeBase* timeBase,
    /* [in] */ IParcel* in)
    : mLoadedCount(0)
    , mLastCount(0)
    , mUnpluggedCount(0)
    , mPluggedCount(0)
{
    CAtomicInteger32::New((IAtomicInteger32**)&mCount);
    mTimeBase = timeBase;
    in->ReadInt32(&mPluggedCount);
    mCount->Set(mPluggedCount);
    in->ReadInt32(&mLoadedCount);
    mLastCount = 0;
    in->ReadInt32(&mUnpluggedCount);
    timeBase->Add(this);
}

BatteryStatsImpl::Counter::Counter(
    /* [in] */ TimeBase* timeBase)
    : mLoadedCount(0)
    , mLastCount(0)
    , mUnpluggedCount(0)
    , mPluggedCount(0)
{
    CAtomicInteger32::New((IAtomicInteger32**)&mCount);
    mTimeBase = timeBase;
    timeBase->Add(this);
}

CAR_INTERFACE_IMPL_2(BatteryStatsImpl::Counter, Object, IBatteryStatsCounter, ITimeBaseObs)

void BatteryStatsImpl::Counter::WriteToParcel(
    /* [in] */ IParcel* out)
{
    Int32 value;
    mCount->Get(&value);
    out->WriteInt32(value);
    out->WriteInt32(mLoadedCount);
    out->WriteInt32(mUnpluggedCount);
}

ECode BatteryStatsImpl::Counter::OnTimeStarted(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    mUnpluggedCount = mPluggedCount;
    mCount->Set(mPluggedCount);
    return NOERROR;
}

ECode BatteryStatsImpl::Counter::OnTimeStopped(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    mCount->Get(&mPluggedCount);
    return NOERROR;
}

void BatteryStatsImpl::Counter::WriteCounterToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Counter* counter)
{
    if (counter == NULL) {
        out->WriteInt32(0); // indicates null
        return;
    }
    out->WriteInt32(1); // indicates non-null

    counter->WriteToParcel(out);
}

ECode BatteryStatsImpl::Counter::GetCountLocked(
    /* [in] */ Int32 which,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    Int32 val;
    mCount->Get(&val);
    if (which == STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedCount;
    }
    else if (which != STATS_SINCE_CHARGED) {
        val -= mLoadedCount;
    }
    *count = val;
    return NOERROR;
}

ECode BatteryStatsImpl::Counter::LogState(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    StringBuilder sb(prefix);
    sb += "mCount=";
    Int32 value;
    mCount->Get(&value);
    sb += value;
    sb += " mLoadedCount=";
    sb += mLoadedCount;
    sb += " mLastCount=";
    sb += mLastCount;
    sb += " mUnpluggedCount=";
    sb += mUnpluggedCount;
    sb += " mPluggedCount=";
    sb += mPluggedCount;
    pw->Println(sb.ToString());
    return NOERROR;
}

void BatteryStatsImpl::Counter::StepAtomic()
{
    Int32 result;
    mCount->IncrementAndGet(&result);
}

void BatteryStatsImpl::Counter::Reset(
    /* [in] */ Boolean detachIfReset)
{
    mCount->Set(0);
    mLoadedCount = mLastCount = mPluggedCount = mUnpluggedCount = 0;
    if (detachIfReset) {
        Detach();
    }
}

void BatteryStatsImpl::Counter::Detach()
{
    mTimeBase->Remove(this);
}

void BatteryStatsImpl::Counter::WriteSummaryFromParcelLocked(
    /* [in] */ IParcel* out)
{
    Int32 count;
    mCount->Get(&count);
    out->WriteInt32(count);
}

void BatteryStatsImpl::Counter::ReadSummaryFromParcelLocked(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mLoadedCount);
    mCount->Set(mLoadedCount);
    mLastCount = 0;
    mUnpluggedCount = mPluggedCount = mLoadedCount;
}


//==============================================================================
// BatteryStatsImpl::SamplingCounter
//==============================================================================

BatteryStatsImpl::SamplingCounter::SamplingCounter(
    /* [in] */ TimeBase* timeBase,
    /* [in] */ IParcel* in)
    : Counter(timeBase, in)
{}

BatteryStatsImpl::SamplingCounter::SamplingCounter(
    /* [in] */ TimeBase* timeBase)
    : Counter(timeBase)
{}

void BatteryStatsImpl::SamplingCounter::AddCountAtomic(
    /* [in] */ Int64 count)
{
    Int32 result;
    mCount->AddAndGet((Int32)count, &result);
}


//==============================================================================
// BatteryStatsImpl::Int64SamplingCounter
//==============================================================================

BatteryStatsImpl::Int64SamplingCounter::Int64SamplingCounter(
    /* [in] */ TimeBase* timeBase,
    /* [in] */ IParcel* in)
    : mTimeBase(timeBase)
    , mCount(0)
    , mLoadedCount(0)
    , mLastCount(0)
    , mUnpluggedCount(0)
    , mPluggedCount(0)
{
    in->ReadInt64(&mPluggedCount);
    mCount = mPluggedCount;
    in->ReadInt64(&mLoadedCount);
    in->ReadInt64(&mUnpluggedCount);
    timeBase->Add(this);
}

BatteryStatsImpl::Int64SamplingCounter::Int64SamplingCounter(
    /* [in] */ TimeBase* timeBase)
    : mTimeBase(timeBase)
    , mCount(0)
    , mLoadedCount(0)
    , mLastCount(0)
    , mUnpluggedCount(0)
    , mPluggedCount(0)
{
    timeBase->Add(this);
}

CAR_INTERFACE_IMPL_2(BatteryStatsImpl::Int64SamplingCounter, Object, IBatteryStatsInt64Counter, ITimeBaseObs)

void BatteryStatsImpl::Int64SamplingCounter::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteInt64(mCount);
    out->WriteInt64(mLoadedCount);
    out->WriteInt64(mUnpluggedCount);
}

ECode BatteryStatsImpl::Int64SamplingCounter::OnTimeStarted(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    mUnpluggedCount = mPluggedCount;
    mCount = mPluggedCount;
    return NOERROR;
}

ECode BatteryStatsImpl::Int64SamplingCounter::OnTimeStopped(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    mPluggedCount = mCount;
    return NOERROR;
}

ECode BatteryStatsImpl::Int64SamplingCounter::GetCountLocked(
    /* [in] */ Int32 which,
    /* [out] */ Int64* count)
{
    Int64 val = mCount;
    if (which == STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedCount;
    }
    else if (which != STATS_SINCE_CHARGED) {
        val -= mLoadedCount;
    }

    return val;
}

ECode BatteryStatsImpl::Int64SamplingCounter::LogState(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    StringBuilder sb(prefix);
    sb += "mCount=";
    sb += mCount;
    sb += " mLoadedCount=";
    sb += mLoadedCount;
    sb += " mLastCount=";
    sb += mLastCount;
    sb += " mUnpluggedCount=";
    sb += mUnpluggedCount;
    sb += " mPluggedCount=";
    sb += mPluggedCount;
    pw->Println(sb.ToString());
    return NOERROR;
}

void BatteryStatsImpl::Int64SamplingCounter::AddCountLocked(
    /* [in] */ Int64 count)
{
    mCount += count;
}

void BatteryStatsImpl::Int64SamplingCounter::Reset(
    /* [in] */ Boolean detachIfReset)
{
    mCount = 0;
    mLoadedCount = mLastCount = mPluggedCount = mUnpluggedCount = 0;
    if (detachIfReset) {
        Detach();
    }
}

void BatteryStatsImpl::Int64SamplingCounter::Detach()
{
    mTimeBase->Remove(this);
}

void BatteryStatsImpl::Int64SamplingCounter::WriteSummaryFromParcelLocked(
    /* [in] */ IParcel* out)
{
    out->WriteInt64(mCount);
}

void BatteryStatsImpl::Int64SamplingCounter::ReadSummaryFromParcelLocked(
    /* [in] */ IParcel* in)
{
    in->ReadInt64(&mLoadedCount);
    mCount = mLoadedCount;
    mLastCount = 0;
    mUnpluggedCount = mPluggedCount = mLoadedCount;
}


//==============================================================================
// BatteryStatsImpl::Timer
//==============================================================================

BatteryStatsImpl::Timer::Timer(
    /* [in] */ Int32 type,
    /* [in] */ TimeBase* timeBase,
    /* [in] */ IParcel* in)
    : mType(type)
    , mTimeBase(timeBase)
    , mCount(0)
    , mLoadedCount(0)
    , mLastCount(0)
    , mUnpluggedCount(0)
    , mTotalTime(0)
    , mLoadedTime(0)
    , mLastTime(0)
    , mUnpluggedTime(0)
{
    in->ReadInt32(&mCount);
    in->ReadInt32(&mLoadedCount);
    in->ReadInt32(&mUnpluggedCount);
    in->ReadInt64(&mTotalTime);
    in->ReadInt64(&mLoadedTime);
    in->ReadInt64(&mUnpluggedTime);
    timeBase->Add(this);
    if (DEBUG) Logger::I(TAG, "**** READ TIMER #%d: mTotalTim=%d", mType, mTotalTime);
}

BatteryStatsImpl::Timer::Timer(
    /* [in] */ Int32 type,
    /* [in] */ TimeBase* timeBase)
    : mType(type)
    , mTimeBase(timeBase)
    , mCount(0)
    , mLoadedCount(0)
    , mLastCount(0)
    , mUnpluggedCount(0)
    , mTotalTime(0)
    , mLoadedTime(0)
    , mLastTime(0)
    , mUnpluggedTime(0)
{
    timeBase->Add(this);
}

CAR_INTERFACE_IMPL_2(BatteryStatsImpl::Timer, Object, IBatteryStatsTimer, ITimeBaseObs)

Boolean BatteryStatsImpl::Timer::Reset(
    /* [in] */ Boolean detachIfReset)
{
    mTotalTime = mLoadedTime = mLastTime = 0;
    mCount = mLoadedCount = mLastCount = 0;
    if (detachIfReset) {
        Detach();
    }
    return TRUE;
}

void BatteryStatsImpl::Timer::Detach()
{
    mTimeBase->Remove(this);
}

void BatteryStatsImpl::Timer::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 elapsedRealtimeUs)
{
    if (DEBUG) Logger::I(TAG, "**** WRITING TIMER #%d: mTotalTime=%lld", mType,
            ComputeRunTimeLocked(mTimeBase->GetRealtime(elapsedRealtimeUs)));
    out->WriteInt32(mCount);
    out->WriteInt32(mLoadedCount);
    out->WriteInt32(mUnpluggedCount);
    out->WriteInt64(ComputeRunTimeLocked(mTimeBase->GetRealtime(elapsedRealtimeUs)));
    out->WriteInt64(mLoadedTime);
    out->WriteInt64(mUnpluggedTime);
}

ECode BatteryStatsImpl::Timer::OnTimeStarted(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    if (DEBUG && mType < 0) {
        Logger::V(TAG, "unplug #%d: realtime=%lld old mUnpluggedTime=%lld old mUnpluggedCount=%lld",
                mType, baseRealtime, mUnpluggedTime, mUnpluggedCount);
    }
    mUnpluggedTime = ComputeRunTimeLocked(baseRealtime);
    mUnpluggedCount = mCount;
    if (DEBUG && mType < 0) {
        Logger::V(TAG, "unplug #%d: new mUnpluggedTime=%lld new mUnpluggedCount=%d",
                mType, mUnpluggedTime, mUnpluggedCount);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Timer::OnTimeStopped(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    if (DEBUG && mType < 0) {
        Logger::V(TAG, "plug #%d: realtime=%lld old mTotalTime=%lld",
                mType, baseRealtime, mTotalTime);
    }
    mTotalTime = ComputeRunTimeLocked(baseRealtime);
    mCount = ComputeCurrentCountLocked();
    if (DEBUG && mType < 0) {
        Logger::V(TAG, "plug #%d: new mTotalTime=%lld", mType, mTotalTime);
    }
    return NOERROR;
}

void BatteryStatsImpl::Timer::WriteTimerToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Timer* timer,
    /* [in] */ Int64 elapsedRealtimeUs)
{
    if (timer == NULL) {
        out->WriteInt32(0); // indicates null
        return;
    }
    out->WriteInt32(1); // indicates non-null

    timer->WriteToParcel(out, elapsedRealtimeUs);
}

ECode BatteryStatsImpl::Timer::GetTotalTimeLocked(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    Int64 val = ComputeRunTimeLocked(mTimeBase->GetRealtime(elapsedRealtimeUs));
    if (which == STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedTime;
    }
    else if (which != STATS_SINCE_CHARGED) {
        val -= mLoadedTime;
    }

    *time = val;
    return NOERROR;
}

ECode BatteryStatsImpl::Timer::GetCountLocked(
    /* [in] */ Int32 which,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    Int32 val = ComputeCurrentCountLocked();
    if (which == STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedCount;
    }
    else if (which != STATS_SINCE_CHARGED) {
        val -= mLoadedCount;
    }

    *count = val;
    return NOERROR;
}

ECode BatteryStatsImpl::Timer::LogState(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    StringBuilder sb(prefix);
    sb += " mCount=";
    sb += mCount;
    sb += " mLoadedCount=";
    sb += mLoadedCount;
    sb += " mLastCount=";
    sb += mLastCount;
    sb += " mUnpluggedCount=";
    sb += mUnpluggedCount;
    pw->Println(sb.ToString());

    StringBuilder sb1(prefix);
    sb1 += "mTotalTime=";
    sb1 += mTotalTime;
    sb1 += " mLoadedTime=";
    sb1 += mLoadedTime;
    pw->Println(sb1.ToString());

    StringBuilder sb2(prefix);
    sb2 += "mLastTime=";
    sb2 += mLastTime;
    sb2 += " mUnpluggedTime=";
    sb2 += mUnpluggedTime;
    pw->Println(sb2.ToString());
    return NOERROR;
}

void BatteryStatsImpl::Timer::WriteSummaryFromParcelLocked(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 elapsedRealtimeUs)
{
    Int64 runTime = ComputeRunTimeLocked(mTimeBase->GetRealtime(elapsedRealtimeUs));
    // Divide by 1000 for backwards compatibility
    out->WriteInt64(runTime);
    out->WriteInt32(mCount);
}

void BatteryStatsImpl::Timer::ReadSummaryFromParcelLocked(
    /* [in] */ IParcel* in)
{
    // Multiply by 1000 for backwards compatibility
    Int64 value;
    in->ReadInt64(&value);
    mTotalTime = mLoadedTime = value;
    mLastTime = 0;
    mUnpluggedTime = mTotalTime;
    Int32 value1;
    in->ReadInt32(&value1);
    mCount = mLoadedCount = value1;
    mLastCount = 0;
    mUnpluggedCount = mCount;
}


//==============================================================================
// BatteryStatsImpl::SamplingTimer
//==============================================================================

BatteryStatsImpl::SamplingTimer::SamplingTimer(
    /* [in] */ TimeBase* timeBase,
    /* [in] */ IParcel* in)
    : Timer(0, timeBase, in)
    , mCurrentReportedCount(0)
    , mUnpluggedReportedCount(0)
    , mCurrentReportedTotalTime(0)
    , mUnpluggedReportedTotalTime(0)
    , mTimeBaseRunning(FALSE)
    , mTrackingReportedValues(FALSE)
    , mUpdateVersion(0)
{
    in->ReadInt32(&mCurrentReportedCount);
    in->ReadInt32(&mUnpluggedReportedCount);
    in->ReadInt64(&mCurrentReportedTotalTime);
    in->ReadInt64(&mUnpluggedReportedTotalTime);
    Int32 value;
    in->ReadInt32(&value);
    mTrackingReportedValues = value == 1;
    mTimeBaseRunning = timeBase->IsRunning();
}

BatteryStatsImpl::SamplingTimer::SamplingTimer(
    /* [in] */ TimeBase* timeBase,
    /* [in] */ Boolean trackReportedValues)
    : Timer(0, timeBase)
    , mCurrentReportedCount(0)
    , mUnpluggedReportedCount(0)
    , mCurrentReportedTotalTime(0)
    , mUnpluggedReportedTotalTime(0)
    , mTimeBaseRunning(FALSE)
    , mTrackingReportedValues(trackReportedValues)
    , mUpdateVersion(0)
{
    mTimeBaseRunning = timeBase->IsRunning();
}

void BatteryStatsImpl::SamplingTimer::SetStale()
{
    mTrackingReportedValues = FALSE;
    mUnpluggedReportedTotalTime = 0;
    mUnpluggedReportedCount = 0;
}

void BatteryStatsImpl::SamplingTimer::SetUpdateVersion(
    /* [in] */ Int32 version)
{
    mUpdateVersion = version;
}

Int32 BatteryStatsImpl::SamplingTimer::GetUpdateVersion()
{
    return mUpdateVersion;
}

void BatteryStatsImpl::SamplingTimer::UpdateCurrentReportedCount(
    /* [in] */ Int32 count)
{
    if (mTimeBaseRunning && mUnpluggedReportedCount == 0) {
        // Updating the reported value for the first time.
        mUnpluggedReportedCount = count;
        // If we are receiving an update update mTrackingReportedValues;
        mTrackingReportedValues = TRUE;
    }
    mCurrentReportedCount = count;
}

void BatteryStatsImpl::SamplingTimer::AddCurrentReportedCount(
    /* [in] */ Int32 delta)
{
    UpdateCurrentReportedCount(mCurrentReportedCount + delta);
}

void BatteryStatsImpl::SamplingTimer::UpdateCurrentReportedTotalTime(
    /* [in] */ Int64 totalTime)
{
    if (mTimeBaseRunning && mUnpluggedReportedTotalTime == 0) {
        // Updating the reported value for the first time.
        mUnpluggedReportedTotalTime = totalTime;
        // If we are receiving an update update mTrackingReportedValues;
        mTrackingReportedValues = TRUE;
    }
    mCurrentReportedTotalTime = totalTime;
}

void BatteryStatsImpl::SamplingTimer::AddCurrentReportedTotalTime(
    /* [in] */ Int64 delta)
{
    UpdateCurrentReportedTotalTime(mCurrentReportedTotalTime + delta);
}

ECode BatteryStatsImpl::SamplingTimer::OnTimeStarted(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    Timer::OnTimeStarted(elapsedRealtime, baseUptime, baseRealtime);
    if (mTrackingReportedValues) {
        mUnpluggedReportedTotalTime = mCurrentReportedTotalTime;
        mUnpluggedReportedCount = mCurrentReportedCount;
    }
    mTimeBaseRunning = TRUE;
    return NOERROR;
}

ECode BatteryStatsImpl::SamplingTimer::OnTimeStopped(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    Timer::OnTimeStopped(elapsedRealtime, baseUptime, baseRealtime);
    mTimeBaseRunning = FALSE;
    return NOERROR;
}

ECode BatteryStatsImpl::SamplingTimer::LogState(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    Timer::LogState(pw, prefix);
    StringBuilder sb(prefix);
    sb += "mCurrentReportedCount=";
    sb += mCurrentReportedCount;
    sb += " mUnpluggedReportedCount=";
    sb += mUnpluggedReportedCount;
    sb += " mCurrentReportedTotalTime=";
    sb += mCurrentReportedTotalTime;
    sb += " mUnpluggedReportedTotalTime=";
    sb += mUnpluggedReportedTotalTime;
    pw->Println(sb.ToString());
    return NOERROR;
}

Int64 BatteryStatsImpl::SamplingTimer::ComputeRunTimeLocked(
    /* [in] */ Int64 curBatteryRealtime)
{
    return mTotalTime + (mTimeBaseRunning && mTrackingReportedValues
            ? mCurrentReportedTotalTime - mUnpluggedReportedTotalTime : 0);
}

Int32 BatteryStatsImpl::SamplingTimer::ComputeCurrentCountLocked()
{
    return mCount + (mTimeBaseRunning && mTrackingReportedValues
            ? mCurrentReportedCount - mUnpluggedReportedCount : 0);
}

void BatteryStatsImpl::SamplingTimer::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 elapsedRealtimeUs)
{
    Timer::WriteToParcel(out, elapsedRealtimeUs);
    out->WriteInt32(mCurrentReportedCount);
    out->WriteInt32(mUnpluggedReportedCount);
    out->WriteInt64(mCurrentReportedTotalTime);
    out->WriteInt64(mUnpluggedReportedTotalTime);
    out->WriteInt32(mTrackingReportedValues ? 1 : 0);
}

Boolean BatteryStatsImpl::SamplingTimer::Reset(
    /* [in] */ Boolean detachIfReset)
{
    Timer::Reset(detachIfReset);
    SetStale();
    return TRUE;
}

void BatteryStatsImpl::SamplingTimer::WriteSummaryFromParcelLocked(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 batteryRealtime)
{
    Timer::WriteSummaryFromParcelLocked(out, batteryRealtime);
    out->WriteInt64(mCurrentReportedTotalTime);
    out->WriteInt32(mCurrentReportedCount);
    out->WriteInt32(mTrackingReportedValues ? 1 : 0);
}

void BatteryStatsImpl::SamplingTimer::ReadSummaryFromParcelLocked(
    /* [in] */ IParcel* in)
{
    Timer::ReadSummaryFromParcelLocked(in);
    Int64 value;
    in->ReadInt64(&value);
    mUnpluggedReportedTotalTime = mCurrentReportedTotalTime = value;
    Int32 value1;
    in->ReadInt32(&value1);
    mUnpluggedReportedCount = mCurrentReportedCount = value1;
    in->ReadInt32(&value1);
    mTrackingReportedValues = value1 == 1;
}


//==============================================================================
// BatteryStatsImpl::BatchTimer
//==============================================================================

BatteryStatsImpl::BatchTimer::BatchTimer(
    /* [in] */ BatteryStatsImpl::Uid* uid,
    /* [in] */ Int32 type,
    /* [in] */ TimeBase* timeBase,
    /* [in] */ IParcel* in)
    : Timer(type, timeBase, in)
    , mUid(uid)
    , mLastAddedTime(0)
    , mLastAddedDuration(0)
    , mInDischarge(FALSE)
{
    in->ReadInt64(&mLastAddedTime);
    in->ReadInt64(&mLastAddedDuration);
    mInDischarge = timeBase->IsRunning();
}

BatteryStatsImpl::BatchTimer::BatchTimer(
    /* [in] */ Uid* uid,
    /* [in] */ Int32 type,
    /* [in] */ TimeBase* timeBase)
    : Timer(type, timeBase)
    , mUid(uid)
    , mLastAddedTime(0)
    , mLastAddedDuration(0)
    , mInDischarge(FALSE)
{
    mInDischarge = timeBase->IsRunning();
}

void BatteryStatsImpl::BatchTimer::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 elapsedRealtimeUs)
{
    Timer::WriteToParcel(out, elapsedRealtimeUs);
    out->WriteInt64(mLastAddedTime);
    out->WriteInt64(mLastAddedDuration);
}

ECode BatteryStatsImpl::BatchTimer::OnTimeStopped(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    RecomputeLastDuration(SystemClock::GetElapsedRealtime() * 1000, FALSE);
    mInDischarge = FALSE;
    return Timer::OnTimeStopped(elapsedRealtime, baseUptime, baseRealtime);
}

ECode BatteryStatsImpl::BatchTimer::OnTimeStarted(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    RecomputeLastDuration(elapsedRealtime, FALSE);
    mInDischarge = TRUE;
    // If we are still within the last added duration, then re-added whatever remains.
    if (mLastAddedTime == elapsedRealtime) {
        mTotalTime += mLastAddedDuration;
    }
    return Timer::OnTimeStarted(elapsedRealtime, baseUptime, baseRealtime);
}

ECode BatteryStatsImpl::BatchTimer::LogState(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    Timer::LogState(pw, prefix);
    StringBuilder sb(prefix);
    sb += "mLastAddedTime=";
    sb += mLastAddedTime;
    sb += " mLastAddedDuration=";
    sb += mLastAddedDuration;
    pw->Println(sb.ToString());
    return NOERROR;
}

Int64 BatteryStatsImpl::BatchTimer::ComputeOverage(
    /* [in] */ Int64 curTime)
{
    if (mLastAddedTime > 0) {
        return mLastTime + mLastAddedDuration - curTime;
    }
    return 0;
}

void BatteryStatsImpl::BatchTimer::RecomputeLastDuration(
    /* [in] */ Int64 curTime,
    /* [in] */ Boolean abort)
{
    Int64 overage = ComputeOverage(curTime);
    if (overage > 0) {
        // Aborting before the duration ran out -- roll back the remaining
        // duration.  Only do this if currently discharging; otherwise we didn't
        // actually add the time.
        if (mInDischarge) {
            mTotalTime -= overage;
        }
        if (abort) {
            mLastAddedTime = 0;
        }
        else {
            mLastAddedTime = curTime;
            mLastAddedDuration -= overage;
        }
    }
}

void BatteryStatsImpl::BatchTimer::AddDuration(
    /* [in] */ BatteryStatsImpl* stats,
    /* [in] */ Int64 durationMillis)
{
    Int64 now = SystemClock::GetElapsedRealtime() * 1000;
    RecomputeLastDuration(now, TRUE);
    mLastAddedTime = now;
    mLastAddedDuration = durationMillis * 1000;
    if (mInDischarge) {
        mTotalTime += mLastAddedDuration;
        mCount++;
    }
}

void BatteryStatsImpl::BatchTimer::AbortLastDuration(
    /* [in] */ BatteryStatsImpl* stats)
{
    Int64 now = SystemClock::GetElapsedRealtime() * 1000;
    RecomputeLastDuration(now, TRUE);
}

Int32 BatteryStatsImpl::BatchTimer::ComputeCurrentCountLocked()
{
    return mCount;
}

Int64 BatteryStatsImpl::BatchTimer::ComputeRunTimeLocked(
    /* [in] */ Int64 curBatteryRealtime)
{
    Int64 overage = ComputeOverage(SystemClock::GetElapsedRealtime() * 1000);
    if (overage > 0) {
        return mTotalTime = overage;
    }
    return mTotalTime;
}

Boolean BatteryStatsImpl::BatchTimer::Reset(
    /* [in] */ Boolean detachIfReset)
{
    Int64 now = SystemClock::GetElapsedRealtime() * 1000;
    RecomputeLastDuration(now, TRUE);
    Boolean stillActive = mLastAddedTime == now;
    Timer::Reset(!stillActive && detachIfReset);
    return !stillActive;
}


//==============================================================================
// BatteryStatsImpl::StopwatchTimer
//==============================================================================

BatteryStatsImpl::StopwatchTimer::StopwatchTimer(
    /* [in] */ Uid* uid,
    /* [in] */ Int32 type,
    /* [in] */ List<AutoPtr<StopwatchTimer> >* timerPool,
    /* [in] */ TimeBase* timeBase,
    /* [in] */ IParcel* in)
    : Timer(type, timeBase, in)
    , mUid(uid)
    , mTimerPool(timerPool)
    , mNesting(0)
    , mAcquireTime(0)
    , mTimeout(0)
    , mInList(FALSE)
{
    in->ReadInt64(&mUpdateTime);
}

BatteryStatsImpl::StopwatchTimer::StopwatchTimer(
    /* [in] */ Uid* uid,
    /* [in] */ Int32 type,
    /* [in] */ List<AutoPtr<StopwatchTimer> >* timerPool,
    /* [in] */ TimeBase* timeBase)
    : Timer(type, timeBase)
    , mUid(uid)
    , mTimerPool(timerPool)
    , mNesting(0)
    , mUpdateTime(0)
    , mAcquireTime(0)
    , mTimeout(0)
    , mInList(FALSE)
{
}

void BatteryStatsImpl::StopwatchTimer::SetTimeout(
    /* [in] */ Int64 timeout)
{
    mTimeout = timeout;
}

void BatteryStatsImpl::StopwatchTimer::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 elapsedRealtimeUs)
{
    Timer::WriteToParcel(out, elapsedRealtimeUs);
    out->WriteInt64(mUpdateTime);
}

ECode BatteryStatsImpl::StopwatchTimer::OnTimeStopped(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    if (mNesting > 0) {
        if (DEBUG && mType < 0) {
            Logger::V(TAG, "old mUpdateTime=%d", mUpdateTime);
        }
        Timer::OnTimeStopped(elapsedRealtime, baseUptime, baseRealtime);
        mUpdateTime = baseRealtime;
        if (DEBUG && mType < 0) {
            Logger::V(TAG, "new mUpdateTime=%d", mUpdateTime);
        }
    }
    return NOERROR;
}

ECode BatteryStatsImpl::StopwatchTimer::LogState(
    /* [in] */ IPrinter* pw,
    /* [in] */ const String& prefix)
{
    Timer::LogState(pw, prefix);
    StringBuilder sb(prefix);
    sb += "mNesting=";
    sb += mNesting;
    sb += "mUpdateTime=";
    sb += mUpdateTime;
    sb += " mAcquireTime=";
    sb += mAcquireTime;
    pw->Println(sb.ToString());
    return NOERROR;
}

void BatteryStatsImpl::StopwatchTimer::StartRunningLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (mNesting++ == 0) {
        Int64 batteryRealtime = mTimeBase->GetRealtime(elapsedRealtimeMs * 1000);
        mUpdateTime = batteryRealtime;
        if (mTimerPool != NULL) {
            // Accumulate time to all currently active timers before adding
            // this new one to the pool.
            RefreshTimersLocked(batteryRealtime, mTimerPool, NULL);
            // Add this timer to the active pool
            mTimerPool->PushBack(this);
        }
        // Increment the count
        mCount++;
        mAcquireTime = mTotalTime;
        if (DEBUG && mType < 0) {
            StringBuilder sb("start #");
            sb += mType;
            sb += ": mUpdateTime=";
            sb += mUpdateTime;
            sb += " mTotalTime=";
            sb += mTotalTime;
            sb += " mCount=";
            sb += mCount;
            sb += " mAcquireTime=";
            sb += mAcquireTime;
            Logger::V(TAG, sb.ToString());
        }
    }
}

Boolean BatteryStatsImpl::StopwatchTimer::IsRunningLocked()
{
    return mNesting > 0;
}

Int64 BatteryStatsImpl::StopwatchTimer::CheckpointRunningLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (mNesting > 0) {
        // We are running...
        Int64 batteryRealtime = mTimeBase->GetRealtime(elapsedRealtimeMs * 1000);
        if (mTimerPool != NULL) {
            return RefreshTimersLocked(batteryRealtime, mTimerPool, this);
        }
        Int64 heldTime = batteryRealtime - mUpdateTime;
        mUpdateTime = batteryRealtime;
        mTotalTime += heldTime;
        return heldTime;
    }
    return 0;
}

void BatteryStatsImpl::StopwatchTimer::StopRunningLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    // Ignore attempt to stop a timer that isn't running
    if (mNesting == 0) {
        return;
    }
    if (--mNesting == 0) {
        Int64 batteryRealtime = mTimeBase->GetRealtime(elapsedRealtimeMs * 1000);
        if (mTimerPool != NULL) {
            // Accumulate time to all active counters, scaled by the total
            // active in the pool, before taking this one out of the pool.
            RefreshTimersLocked(batteryRealtime, mTimerPool, NULL);
            // Remove this timer from the active pool
            mTimerPool->Remove(this);
        }
        else {
            mNesting = 1;
            mTotalTime = ComputeRunTimeLocked(batteryRealtime);
            mNesting = 0;
        }

        if (DEBUG && mType < 0) {
            StringBuilder sb("stop #");
            sb += mType;
            sb += ": mUpdateTime=";
            sb += mUpdateTime;
            sb += " mTotalTime=";
            sb += mTotalTime;
            sb += " mCount=";
            sb += mCount;
            sb += " mAcquireTime=";
            sb += mAcquireTime;
            Logger::V(TAG, sb.ToString());
        }

        if (mTotalTime == mAcquireTime) {
            // If there was no change in the time, then discard this
            // count.  A somewhat cheezy strategy, but hey.
            mCount--;
        }
    }
}

void BatteryStatsImpl::StopwatchTimer::StopAllRunningLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (mNesting > 0) {
        mNesting = 1;
        StopRunningLocked(elapsedRealtimeMs);
    }
}

Int64 BatteryStatsImpl::StopwatchTimer::RefreshTimersLocked(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ List<AutoPtr<StopwatchTimer> >* pool,
    /* [in] */ StopwatchTimer* self)
{
    Int64 selfTime = 0;
    List<AutoPtr<StopwatchTimer> >::ReverseIterator rit = pool->RBegin();
    for (; rit != pool->REnd(); ++rit) {
        AutoPtr<StopwatchTimer> t = *rit;
        Int64 heldTime = batteryRealtime - t->mUpdateTime;
        if (heldTime > 0) {
            Int64 myTime = heldTime / pool->GetSize();
            if (t.Get() == self) {
                selfTime = myTime;
            }
            t->mTotalTime += myTime;
        }
        t->mUpdateTime = batteryRealtime;
    }
    return selfTime;
}

Int64 BatteryStatsImpl::StopwatchTimer::ComputeRunTimeLocked(
    /* [in] */ Int64 curBatteryRealtime)
{
    if (mTimeout > 0 && curBatteryRealtime > mUpdateTime + mTimeout) {
        curBatteryRealtime = mUpdateTime + mTimeout;
    }
    return mTotalTime + (mNesting > 0
            ? (curBatteryRealtime - mUpdateTime)
                    / (mTimerPool != NULL ? mTimerPool->GetSize() : 1)
            : 0);
}

Int32 BatteryStatsImpl::StopwatchTimer::ComputeCurrentCountLocked()
{
    return mCount;
}

Boolean BatteryStatsImpl::StopwatchTimer::Reset(
    /* [in] */ Boolean detachIfReset)
{
    Boolean canDetach = mNesting <= 0;
    Timer::Reset(canDetach && detachIfReset);
    if (mNesting > 0) {
        mUpdateTime = mTimeBase->GetRealtime(SystemClock::GetElapsedRealtime() * 1000);
    }
    mAcquireTime = mTotalTime;
    return canDetach;
}

void BatteryStatsImpl::StopwatchTimer::Detach()
{
    Timer::Detach();
    if (mTimerPool != NULL) {
        List<AutoPtr<StopwatchTimer> >::Iterator it = mTimerPool->Begin();
        for (; it != mTimerPool->End(); ++it) {
            if ((*it).Get() == this) {
                mTimerPool->Erase(it);
                break;
            }
        }
    }
}

void BatteryStatsImpl::StopwatchTimer::ReadSummaryFromParcelLocked(
    /* [in] */ IParcel* in)
{
    Timer::ReadSummaryFromParcelLocked(in);
    mNesting = 0;
}


//==============================================================================
// BatteryStatsImpl::Uid::Wakelock
//==============================================================================

CAR_INTERFACE_IMPL(BatteryStatsImpl::Uid::Wakelock, Object, IBatteryStatsUidWakelock)

AutoPtr<BatteryStatsImpl::StopwatchTimer> BatteryStatsImpl::Uid::Wakelock::ReadTimerFromParcel(
    /* [in] */ Int32 type,
    /* [in] */ List<AutoPtr<StopwatchTimer> >* pool,
    /* [in] */ TimeBase* timeBase,
    /* [in] */ IParcel* in)
{
    Int32 value;
    if (in->ReadInt32(&value), value == 0) {
        return NULL;
    }

    return new StopwatchTimer(mHost, type, pool, timeBase, in);
}

Boolean BatteryStatsImpl::Uid::Wakelock::Reset()
{
    Boolean wlactive = FALSE;
    if (mTimerFull != NULL) {
        wlactive |= !mTimerFull->Reset(FALSE);
    }
    if (mTimerPartial != NULL) {
        wlactive |= !mTimerPartial->Reset(FALSE);
    }
    if (mTimerWindow != NULL) {
        wlactive |= !mTimerWindow->Reset(FALSE);
    }
    if (!wlactive) {
        if (mTimerFull != NULL) {
            mTimerFull->Detach();
            mTimerFull = NULL;
        }
        if (mTimerPartial != NULL) {
            mTimerPartial->Detach();
            mTimerPartial = NULL;
        }
        if (mTimerWindow != NULL) {
            mTimerWindow->Detach();
            mTimerWindow = NULL;
        }
    }
    return !wlactive;
}

void BatteryStatsImpl::Uid::Wakelock::ReadFromParcelLocked(
    /* [in] */ TimeBase* timeBase,
    /* [in] */ TimeBase* screenOffTimeBase,
    /* [in] */ IParcel* in)
{
    mTimerPartial = ReadTimerFromParcel(WAKE_TYPE_PARTIAL,
            &mHost->mHost->mPartialTimers, screenOffTimeBase, in);
    mTimerFull = ReadTimerFromParcel(WAKE_TYPE_FULL,
            &mHost->mHost->mFullTimers, timeBase, in);
    mTimerWindow = ReadTimerFromParcel(WAKE_TYPE_WINDOW,
            &mHost->mHost->mWindowTimers, timeBase, in);
}

void BatteryStatsImpl::Uid::Wakelock::WriteToParcelLocked(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 elapsedRealtimeUs)
{
    Timer::WriteTimerToParcel(out, mTimerPartial, elapsedRealtimeUs);
    Timer::WriteTimerToParcel(out, mTimerFull, elapsedRealtimeUs);
    Timer::WriteTimerToParcel(out, mTimerWindow, elapsedRealtimeUs);
}

ECode BatteryStatsImpl::Uid::Wakelock::GetWakeTime(
    /* [in] */ Int32 type,
    /* [out] */ IBatteryStatsTimer** timer)
{
    VALIDATE_NOT_NULL(timer)
    *timer = NULL;
    switch (type) {
        case WAKE_TYPE_FULL: *timer = mTimerFull;break;
        case WAKE_TYPE_PARTIAL: *timer = mTimerPartial;break;
        case WAKE_TYPE_WINDOW: *timer = mTimerWindow;break;
        default:
            Logger::E(TAG, "line:%d, func:%s,type:%d\n", __LINE__, __func__, type);
            // throw new IllegalArgumentException("type = " + type);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    REFCOUNT_ADD(*timer);
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Wakelock::GetStopwatchTimer(
    /* [in] */ Int32 type,
    /* [out] */ StopwatchTimer** timer)
{
    VALIDATE_NOT_NULL(timer)
    *timer = NULL;
    AutoPtr<StopwatchTimer> t;
    switch (type) {
        case IBatteryStats::WAKE_TYPE_PARTIAL:
            t = mTimerPartial;
            if (t == NULL) {
                t = new StopwatchTimer(mHost, IBatteryStats::WAKE_TYPE_PARTIAL,
                        &mHost->mHost->mPartialTimers, mHost->mHost->mOnBatteryScreenOffTimeBase);
                mTimerPartial = t;
            }
            break;
        case IBatteryStats::WAKE_TYPE_FULL:
            t = mTimerFull;
            if (t == NULL) {
                t = new StopwatchTimer(mHost, IBatteryStats::WAKE_TYPE_FULL,
                        &mHost->mHost->mFullTimers, mHost->mHost->mOnBatteryTimeBase);
                mTimerFull = t;
            }
            break;
        case IBatteryStats::WAKE_TYPE_WINDOW:
            t = mTimerWindow;
            if (t == NULL) {
                t = new StopwatchTimer(mHost, IBatteryStats::WAKE_TYPE_WINDOW,
                        &mHost->mHost->mWindowTimers, mHost->mHost->mOnBatteryTimeBase);
                mTimerWindow = t;
            }
            break;
        default:
            Logger::E(TAG, "line:%d, func:%s,type:%d\n", __LINE__, __func__, type);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("type=" + type);
    }
    *timer = t;
    REFCOUNT_ADD(*timer);
    return NOERROR;
}


//==============================================================================
// BatteryStatsImpl::Uid::Sensor
//==============================================================================

CAR_INTERFACE_IMPL(BatteryStatsImpl::Uid::Sensor, Object, IBatteryStatsUidSensor)

AutoPtr<BatteryStatsImpl::StopwatchTimer> BatteryStatsImpl::Uid::Sensor::ReadTimerFromParcel(
    /* [in] */ TimeBase* timeBase,
    /* [in] */ IParcel* in)
{
    Int32 value;
    if (in->ReadInt32(&value), value == 0) {
        return NULL;
    }

    AutoPtr< List<AutoPtr<StopwatchTimer> > > pool;
    HashMap<Int32, AutoPtr< List<AutoPtr<StopwatchTimer> > > >::Iterator it = mHost->mHost->mSensorTimers.Find(mHandle);
    if (it != mHost->mHost->mSensorTimers.End()) {
        pool = it->mSecond;
    }
    if (pool == NULL) {
        pool = new List<AutoPtr<StopwatchTimer> >(32);
        mHost->mHost->mSensorTimers[mHandle] = pool;
    }
    return new StopwatchTimer(mHost, 0, pool, timeBase, in);
}

Boolean BatteryStatsImpl::Uid::Sensor::Reset()
{
    if (mTimer->Reset(TRUE)) {
        mTimer = NULL;
        return TRUE;
    }
    return FALSE;
}

void BatteryStatsImpl::Uid::Sensor::ReadFromParcelLocked(
    /* [in] */ TimeBase* timeBase,
    /* [in] */ IParcel* in)
{
    mTimer = ReadTimerFromParcel(timeBase, in);
}

void BatteryStatsImpl::Uid::Sensor::WriteToParcelLocked(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 elapsedRealtimeUs)
{
    Timer::WriteTimerToParcel(out, mTimer, elapsedRealtimeUs);
}

ECode BatteryStatsImpl::Uid::Sensor::GetHandle(
    /* [out] */ Int32* handle)
{
    VALIDATE_NOT_NULL(handle)
    *handle = mHandle;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Sensor::GetSensorTime(
    /* [out] */ IBatteryStatsTimer** timer)
{
    VALIDATE_NOT_NULL(timer)
    *timer = (IBatteryStatsTimer*)mTimer;
    REFCOUNT_ADD(*timer)
    return NOERROR;
}


//==============================================================================
// BatteryStatsImpl::Uid::Proc
//==============================================================================

BatteryStatsImpl::Uid::Proc::Proc(
    /* [in] */ const String& name,
    /* [in] */ BatteryStatsImpl* host)
    : mName(name)
    , mActive(TRUE)
    , mUserTime(0)
    , mSystemTime(0)
    , mStarts(0)
    , mForegroundTime(0)
    , mLoadedUserTime(0)
    , mLoadedSystemTime(0)
    , mLoadedStarts(0)
    , mLoadedForegroundTime(0)
    , mLastUserTime(0)
    , mLastSystemTime(0)
    , mLastStarts(0)
    , mLastForegroundTime(0)
    , mUnpluggedUserTime(0)
    , mUnpluggedSystemTime(0)
    , mUnpluggedForegroundTime(0)
    , mUnpluggedStarts(0)
    , mProcessState(PROCESS_STATE_NONE)
    , mHost(host)
{
    mHost->mOnBatteryTimeBase->Add(this);
    Int32 steps;
    mHost->GetCpuSpeedSteps(&steps);
    mSpeedBins = ArrayOf<SamplingCounter*>::Alloc(steps);
}

CAR_INTERFACE_IMPL_2(BatteryStatsImpl::Uid::Proc, Object, IBatteryStatsImplUidProc, ITimeBaseObs)

ECode BatteryStatsImpl::Uid::Proc::OnTimeStarted(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    mUnpluggedUserTime = mUserTime;
    mUnpluggedSystemTime = mSystemTime;
    mUnpluggedForegroundTime = mForegroundTime;
    mUnpluggedStarts = mStarts;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Proc::OnTimeStopped(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    return NOERROR;
}

void BatteryStatsImpl::Uid::Proc::Reset()
{
    mUserTime = mSystemTime = mForegroundTime = 0;
    mStarts = 0;
    mLoadedUserTime = mLoadedSystemTime = mLoadedForegroundTime = 0;
    mLoadedStarts = 0;
    mLastUserTime = mLastSystemTime = mLastForegroundTime = 0;
    mLastStarts = 0;
    mUnpluggedUserTime = mUnpluggedSystemTime = mUnpluggedForegroundTime = 0;
    mUnpluggedStarts = 0;
    for (Int32 i = 0; i < mSpeedBins->GetLength(); i++) {
        AutoPtr<SamplingCounter> c = (*mSpeedBins)[i];
        if (c != NULL) {
            c->Reset(FALSE);
        }
    }
    mExcessivePower = NULL;
}

void BatteryStatsImpl::Uid::Proc::Detach()
{
    mActive = FALSE;
    mHost->mOnBatteryTimeBase->Remove(this);
    for (Int32 i = 0; i < mSpeedBins->GetLength(); i++) {
        AutoPtr<SamplingCounter> c = (*mSpeedBins)[i];
        if (c != NULL) {
            mHost->mOnBatteryTimeBase->Remove((ITimeBaseObs*)c);
            mSpeedBins->Set(i, NULL);
        }
    }
}

Int32 BatteryStatsImpl::Uid::Proc::CountExcessivePowers()
{
    return mExcessivePower != NULL ? mExcessivePower->GetSize() : 0;
}

ECode BatteryStatsImpl::Uid::Proc::GetExcessivePower(
    /* [in] */ Int32 i,
    /* [out] */ IBatteryStatsUidProcExcessivePower** excessivePower)
{
    VALIDATE_NOT_NULL(excessivePower)
    *excessivePower = NULL;
    if (mExcessivePower != NULL) {
        *excessivePower = (IBatteryStatsUidProcExcessivePower*)(*mExcessivePower)[i];
        REFCOUNT_ADD(*excessivePower)
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Proc::AddExcessiveWake(
    /* [in] */ Int64 overTime,
    /* [in] */ Int64 usedTime)
{
    if (mExcessivePower == NULL) {
        mExcessivePower = new List<AutoPtr<ExcessivePower> >();
    }
    AutoPtr<ExcessivePower> ew = new ExcessivePower();
    ew->mType = ExcessivePower::TYPE_WAKE;
    ew->mOverTime = overTime;
    ew->mUsedTime = usedTime;
    mExcessivePower->PushBack(ew);
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Proc::AddExcessiveCpu(
    /* [in] */ Int64 overTime,
    /* [in] */ Int64 usedTime)
{
    if (mExcessivePower == NULL) {
        mExcessivePower = new List<AutoPtr<ExcessivePower> >();
    }
    AutoPtr<ExcessivePower> ew = new ExcessivePower();
    ew->mType = ExcessivePower::TYPE_CPU;
    ew->mOverTime = overTime;
    ew->mUsedTime = usedTime;
    mExcessivePower->PushBack(ew);
    return NOERROR;
}

void BatteryStatsImpl::Uid::Proc::WriteExcessivePowerToParcelLocked(
    /* [in] */ IParcel* out)
{
    if (mExcessivePower == NULL) {
        out->WriteInt32(0);
        return;
    }

    Int32 N = mExcessivePower->GetSize();
    out->WriteInt32(N);
    List<AutoPtr<ExcessivePower> >::Iterator it = mExcessivePower->Begin();
    for (; it != mExcessivePower->End(); ++it) {
        AutoPtr<ExcessivePower> ew = *it;
        out->WriteInt32(ew->mType);
        out->WriteInt64(ew->mOverTime);
        out->WriteInt64(ew->mUsedTime);
    }
}

Boolean BatteryStatsImpl::Uid::Proc::ReadExcessivePowerFromParcelLocked(
    /* [in] */ IParcel* in)
{
    Int32 N;
    in->ReadInt32(&N);
    if (N == 0) {
        mExcessivePower = NULL;
        return TRUE;
    }

    if (N > 10000) {
        Slogger::W(TAG, "File corrupt: too many excessive power entries %d", N);
        return FALSE;
    }

    mExcessivePower = new List<AutoPtr<ExcessivePower> >(N);
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ExcessivePower> ew = new ExcessivePower();
        in->ReadInt32(&ew->mType);
        in->ReadInt64(&ew->mOverTime);
        in->ReadInt64(&ew->mUsedTime);
        mExcessivePower->PushBack(ew);
    }
    return TRUE;
}

void BatteryStatsImpl::Uid::Proc::WriteToParcelLocked(
    /* [in] */ IParcel* out)
{
    out->WriteInt64(mUserTime);
    out->WriteInt64(mSystemTime);
    out->WriteInt64(mForegroundTime);
    out->WriteInt32(mStarts);
    out->WriteInt64(mLoadedUserTime);
    out->WriteInt64(mLoadedSystemTime);
    out->WriteInt64(mLoadedForegroundTime);
    out->WriteInt32(mLoadedStarts);
    out->WriteInt64(mUnpluggedUserTime);
    out->WriteInt64(mUnpluggedSystemTime);
    out->WriteInt64(mUnpluggedForegroundTime);
    out->WriteInt32(mUnpluggedStarts);

    out->WriteInt32(mSpeedBins->GetLength());
    for (Int32 i = 0; i < mSpeedBins->GetLength(); i++) {
        AutoPtr<SamplingCounter> c = (*mSpeedBins)[i];
        if (c != NULL) {
            out->WriteInt32(1);
            c->WriteToParcel(out);
        }
        else {
            out->WriteInt32(0);
        }
    }

    WriteExcessivePowerToParcelLocked(out);
}

void BatteryStatsImpl::Uid::Proc::ReadFromParcelLocked(
    /* [in] */ IParcel* in)
{
    in->ReadInt64(&mUserTime);
    in->ReadInt64(&mSystemTime);
    in->ReadInt64(&mForegroundTime);
    in->ReadInt32(&mStarts);
    in->ReadInt64(&mLoadedUserTime);
    in->ReadInt64(&mLoadedSystemTime);
    in->ReadInt64(&mLoadedForegroundTime);
    in->ReadInt32(&mLoadedStarts);
    mLastUserTime = 0;
    mLastSystemTime = 0;
    mLastForegroundTime = 0;
    mLastStarts = 0;
    in->ReadInt64(&mUnpluggedUserTime);
    in->ReadInt64(&mUnpluggedSystemTime);
    in->ReadInt64(&mUnpluggedForegroundTime);
    in->ReadInt32(&mUnpluggedStarts);

    Int32 bins;
    in->ReadInt32(&bins);
    Int32 steps;
    mHost->GetCpuSpeedSteps(&steps);
    mSpeedBins = ArrayOf<SamplingCounter*>::Alloc(bins >= steps ? bins : steps);
    for (Int32 i = 0; i < bins; i++) {
        Int32 value;
        if (in->ReadInt32(&value), value != 0) {
            AutoPtr<SamplingCounter> counter = new SamplingCounter(
                    mHost->mOnBatteryTimeBase, in);
            mSpeedBins->Set(i, counter);
        }
    }

    ReadExcessivePowerFromParcelLocked(in);
}

ECode BatteryStatsImpl::Uid::Proc::GetBatteryStats(
    /* [out] */ IBatteryStatsImpl** batteryStats)
{
    VALIDATE_NOT_NULL(batteryStats)
    *batteryStats = (IBatteryStatsImpl*)mHost;
    REFCOUNT_ADD(*batteryStats)
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Proc::AddCpuTimeLocked(
    /* [in] */ Int32 utime,
    /* [in] */ Int32 stime)
{
    mUserTime += utime;
    mSystemTime += stime;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Proc::AddForegroundTimeLocked(
    /* [in] */ Int64 ttime)
{
    mForegroundTime += ttime;
    return NOERROR;
}

void BatteryStatsImpl::Uid::Proc::IncStartsLocked()
{
    mStarts++;
}

ECode BatteryStatsImpl::Uid::Proc::IsActive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mActive;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Proc::GetUserTime(
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    Int64 val = mUserTime;
    if (which == IBatteryStats::STATS_CURRENT) {
        val -= mLoadedUserTime;
    }
    else if (which == IBatteryStats::STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedUserTime;
    }
    *time = val;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Proc::GetSystemTime(
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    Int64 val = mSystemTime;
    if (which == IBatteryStats::STATS_CURRENT) {
        val -= mLoadedSystemTime;
    }
    else if (which == IBatteryStats::STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedSystemTime;
    }
    *time = val;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Proc::GetForegroundTime(
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    Int64 val = mForegroundTime;
    if (which == IBatteryStats::STATS_CURRENT) {
        val -= mLoadedForegroundTime;
    }
    else if (which == IBatteryStats::STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedForegroundTime;
    }
    *time = val;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Proc::GetStarts(
    /* [in] */ Int32 which,
    /* [out] */ Int32* starts)
{
    VALIDATE_NOT_NULL(starts)
    Int32 val = mStarts;
    if (which == IBatteryStats::STATS_CURRENT) {
        val -= mLoadedStarts;
    }
    else if (which == IBatteryStats::STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedStarts;
    }
    *starts = val;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Proc::AddSpeedStepTimes(
    /* [in] */ ArrayOf<Int64>* values)
{
    for (Int32 i = 0; i < mSpeedBins->GetLength() && i < values->GetLength(); i++) {
        Int64 amt = (*values)[i];
        if (amt != 0) {
            AutoPtr<SamplingCounter> c = (*mSpeedBins)[i];
            if (c == NULL) {
                c = new SamplingCounter(mHost->mOnBatteryTimeBase);
                mSpeedBins->Set(i, c);
            }
            c->AddCountAtomic((*values)[i]);
        }
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Proc::GetTimeAtCpuSpeedStep(
    /* [in] */ Int32 speedStep,
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = 0;
    if (speedStep < mSpeedBins->GetLength()) {
        AutoPtr<SamplingCounter> c = (*mSpeedBins)[speedStep];
        Int32 count;
        *time = c != NULL ? (c->GetCountLocked(which, &count), count) : 0;
    }
    return NOERROR;
}


//==============================================================================
// BatteryStatsImpl::Uid::Pkg::Serv
//==============================================================================

BatteryStatsImpl::Uid::Pkg::Serv::Serv(
    /* [in] */ BatteryStatsImpl* host)
    : mStartTime(0)
    , mRunningSince(0)
    , mRunning(FALSE)
    , mStarts(0)
    , mLaunchedTime(0)
    , mLaunchedSince(0)
    , mLaunched(FALSE)
    , mLaunches(0)
    , mLoadedStartTime(0)
    , mLoadedStarts(0)
    , mLoadedLaunches(0)
    , mLastStartTime(0)
    , mLastStarts(0)
    , mLastLaunches(0)
    , mUnpluggedStartTime(0)
    , mUnpluggedStarts(0)
    , mUnpluggedLaunches(0)
    , mHost(host)
{
   mHost->mOnBatteryTimeBase->Add(this);
}

CAR_INTERFACE_IMPL_3(BatteryStatsImpl::Uid::Pkg::Serv, Object, IBatteryStatsUidPkgServ, IBatteryStatsImplUidPkgServ, ITimeBaseObs)

ECode BatteryStatsImpl::Uid::Pkg::Serv::OnTimeStarted(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    mUnpluggedStartTime = GetStartTimeToNowLocked(baseUptime);
    mUnpluggedStarts = mStarts;
    mUnpluggedLaunches = mLaunches;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Pkg::Serv::OnTimeStopped(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    return NOERROR;
}

void BatteryStatsImpl::Uid::Pkg::Serv::Detach()
{
    mHost->mOnBatteryTimeBase->Remove(this);
}

void BatteryStatsImpl::Uid::Pkg::Serv::ReadFromParcelLocked(
    /* [in] */ IParcel* in)
{
    in->ReadInt64(&mStartTime);
    in->ReadInt64(&mRunningSince);
    in->ReadBoolean(&mRunning);
    in->ReadInt32(&mStarts);
    in->ReadInt64(&mLaunchedTime);
    in->ReadInt64(&mLaunchedSince);
    in->ReadBoolean(&mLaunched);
    in->ReadInt32(&mLaunches);
    in->ReadInt64(&mLoadedStartTime);
    in->ReadInt32(&mLoadedStarts);
    in->ReadInt32(&mLoadedLaunches);
    mLastStartTime = 0;
    mLastStarts = 0;
    mLastLaunches = 0;
    in->ReadInt64(&mUnpluggedStartTime);
    in->ReadInt32(&mUnpluggedStarts);
    in->ReadInt32(&mUnpluggedLaunches);
}

void BatteryStatsImpl::Uid::Pkg::Serv::WriteToParcelLocked(
    /* [in] */ IParcel* out)
{
    out->WriteInt64(mStartTime);
    out->WriteInt64(mRunningSince);
    out->WriteBoolean(mRunning);
    out->WriteInt32(mStarts);
    out->WriteInt64(mLaunchedTime);
    out->WriteInt64(mLaunchedSince);
    out->WriteBoolean(mLaunched);
    out->WriteInt32(mLaunches);
    out->WriteInt64(mLoadedStartTime);
    out->WriteInt32(mLoadedStarts);
    out->WriteInt32(mLoadedLaunches);
    out->WriteInt64(mUnpluggedStartTime);
    out->WriteInt32(mUnpluggedStarts);
    out->WriteInt32(mUnpluggedLaunches);
}

Int64 BatteryStatsImpl::Uid::Pkg::Serv::GetLaunchTimeToNowLocked(
    /* [in] */ Int64 batteryUptime)
{
    if (!mLaunched) return mLaunchedTime;
    return mLaunchedTime + batteryUptime - mLaunchedSince;
}

Int64 BatteryStatsImpl::Uid::Pkg::Serv::GetStartTimeToNowLocked(
    /* [in] */ Int64 batteryUptime)
{
    if (!mRunning) return mStartTime;
    return mStartTime + batteryUptime - mRunningSince;
}

ECode BatteryStatsImpl::Uid::Pkg::Serv::StartLaunchedLocked()
{
    if (!mLaunched) {
        mLaunches++;
        mLaunchedSince = mHost->GetBatteryUptimeLocked();
        mLaunched = TRUE;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Pkg::Serv::StopLaunchedLocked()
{
    if (mLaunched) {
        Int64 time = mHost->GetBatteryUptimeLocked() - mLaunchedSince;
        if (time > 0) {
            mLaunchedTime += time;
        }
        else {
            mLaunches--;
        }
        mLaunched = FALSE;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Pkg::Serv::StartRunningLocked()
{
    if (!mRunning) {
        mStarts++;
        mRunningSince = mHost->GetBatteryUptimeLocked();
        mRunning = TRUE;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Pkg::Serv::StopRunningLocked()
{
    if (mRunning) {
        Int64 time = mHost->GetBatteryUptimeLocked() - mRunningSince;
        if (time > 0) {
            mStartTime += time;
        }
        else {
            mStarts--;
        }
        mRunning = FALSE;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Pkg::Serv::GetBatteryStats(
    /* [out] */ IBatteryStatsImpl** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = (IBatteryStatsImpl*)mHost;
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Pkg::Serv::GetLaunches(
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 val = mLaunches;
    if (which == IBatteryStats::STATS_CURRENT) {
        val -= mLoadedLaunches;
    }
    else if (which == IBatteryStats::STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedLaunches;
    }
    *result = val;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Pkg::Serv::GetStartTime(
    /* [in] */ Int64 now,
    /* [in] */ Int32 which,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    Int64 val = GetStartTimeToNowLocked(now);
    if (which == IBatteryStats::STATS_CURRENT) {
        val -= mLoadedStartTime;
    }
    else if (which == IBatteryStats::STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedStartTime;
    }
    *value = val;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Pkg::Serv::GetStarts(
    /* [in] */ Int32 which,
    /* [out] */ Int32* starts)
{
    VALIDATE_NOT_NULL(starts)
    Int32 val = mStarts;
    if (which == IBatteryStats::STATS_CURRENT) {
        val -= mLoadedStarts;
    }
    else if (which == IBatteryStats::STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedStarts;
    }

    *starts = val;
    return NOERROR;
}


//==============================================================================
// BatteryStatsImpl::Uid::Pkg
//==============================================================================

BatteryStatsImpl::Uid::Pkg::Pkg(
    /* [in] */ BatteryStatsImpl* host)
    : mWakeups(0)
    , mLoadedWakeups(0)
    , mLastWakeups(0)
    , mUnpluggedWakeups(0)
    , mHost(host)
{
    CHashMap::New((IHashMap**)&mServiceStats);
    mHost->mOnBatteryScreenOffTimeBase->Add(this);
}

CAR_INTERFACE_IMPL_3(BatteryStatsImpl::Uid::Pkg, Object, IBatteryStatsUidPkg, IBatteryStatsImplUidPkg, ITimeBaseObs)

ECode BatteryStatsImpl::Uid::Pkg::OnTimeStarted(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    mUnpluggedWakeups = mWakeups;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Pkg::OnTimeStopped(
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 baseUptime,
    /* [in] */ Int64 baseRealtime)
{
    return NOERROR;
}

void BatteryStatsImpl::Uid::Pkg::Detach()
{
    mHost->mOnBatteryScreenOffTimeBase->Remove(this);
}

void BatteryStatsImpl::Uid::Pkg::ReadFromParcelLocked(
    /* [in] */ IParcel* in)
{
    in->ReadInt32(&mWakeups);
    in->ReadInt32(&mLoadedWakeups);
    mLastWakeups = 0;
    in->ReadInt32(&mUnpluggedWakeups);

    Int32 numServs;
    in->ReadInt32(&numServs);
    mServiceStats->Clear();
    for (Int32 m = 0; m < numServs; m++) {
        String serviceName;
        in->ReadString(&serviceName);
        AutoPtr<ICharSequence> cs;
        CString::New(serviceName, (ICharSequence**)&cs);
        AutoPtr<Serv> serv = new Serv(mHost);
        mServiceStats->Put(cs, (IObject*)serv.Get());

        serv->ReadFromParcelLocked(in);
    }
}

void BatteryStatsImpl::Uid::Pkg::WriteToParcelLocked(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(mWakeups);
    out->WriteInt32(mLoadedWakeups);
    out->WriteInt32(mUnpluggedWakeups);

    Int32 size;
    mServiceStats->GetSize(&size);
    out->WriteInt32(size);
    AutoPtr<ISet> set;
    mServiceStats->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<IMapEntry> servEntry = IMapEntry::Probe(next);
        AutoPtr<IInterface> ki;
        servEntry->GetKey((IInterface**)&ki);
        String key;
        ICharSequence::Probe(ki)->ToString(&key);
        out->WriteString(key);
        AutoPtr<IInterface> value;
        servEntry->GetValue((IInterface**)&value);
        AutoPtr<Serv> serv = (Serv*)(IObject*)value.Get();
        serv->WriteToParcelLocked(out);
    }
}

ECode BatteryStatsImpl::Uid::Pkg::GetServiceStats(
    /* [out] */ IMap** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = IMap::Probe(mServiceStats);
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Pkg::GetWakeups(
    /* [in] */ Int32 which,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    Int32 val = mWakeups;
    if (which == IBatteryStats::STATS_CURRENT) {
        val -= mLoadedWakeups;
    }
    else if (which == IBatteryStats::STATS_SINCE_UNPLUGGED) {
        val -= mUnpluggedWakeups;
    }

    *value = val;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::Pkg::GetBatteryStats(
    /* [out] */ IBatteryStatsImpl** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = (IBatteryStatsImpl*)mHost;
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

void BatteryStatsImpl::Uid::Pkg::IncWakeupsLocked()
{
    mWakeups++;
}

AutoPtr<BatteryStatsImpl::Uid::Pkg::Serv> BatteryStatsImpl::Uid::Pkg::NewServiceStatsLocked()
{
    return new Serv(mHost);
}


//==============================================================================
// BatteryStatsImpl::Uid::WakelockStats
//==============================================================================

AutoPtr<BatteryStatsImpl::Uid::Wakelock> BatteryStatsImpl::Uid::WakelockStats::InstantiateObject()
{
    return new Wakelock(mHost);
}


//==============================================================================
// BatteryStatsImpl::Uid::SyncStats
//==============================================================================

AutoPtr<BatteryStatsImpl::StopwatchTimer> BatteryStatsImpl::Uid::SyncStats::InstantiateObject()
{
    return new StopwatchTimer(mHost, IBatteryStats::SYNC, NULL, mHost->mHost->mOnBatteryTimeBase);
}


//==============================================================================
// BatteryStatsImpl::Uid::JobStats
//==============================================================================

AutoPtr<BatteryStatsImpl::StopwatchTimer> BatteryStatsImpl::Uid::JobStats::InstantiateObject()
{
    return new StopwatchTimer(mHost, IBatteryStats::JOB, NULL, mHost->mHost->mOnBatteryTimeBase);
}


//==============================================================================
// BatteryStatsImpl::Uid
//==============================================================================

const Int32 BatteryStatsImpl::Uid::NO_BATCHED_SCAN_STARTED;
const Int32 BatteryStatsImpl::Uid::PROCESS_STATE_NONE;

BatteryStatsImpl::Uid::Uid(
    /* [in] */ Int32 uid,
    /* [in] */ BatteryStatsImpl* host)
    : mUid(uid)
    , mWifiRunning(FALSE)
    , mFullWifiLockOut(FALSE)
    , mWifiScanStarted(FALSE)
    , mWifiBatchedScanBinStarted(NO_BATCHED_SCAN_STARTED)
    , mWifiMulticastEnabled(FALSE)
    , mProcessState(PROCESS_STATE_NONE)
    , mHost(host)
{
    mWakelockStats = new WakelockStats(this);
    mSyncStats = new SyncStats(this);
    mJobStats = new JobStats(this);

    CSparseArray::New((ISparseArray**)&mSensorStats);
    CArrayMap::New((IArrayMap**)&mProcessStats);
    CArrayMap::New((IArrayMap**)&mPackageStats);
    CSparseArray::New((ISparseArray**)&mPids);

    mWifiRunningTimer = new StopwatchTimer(this, IBatteryStats::WIFI_RUNNING,
            &mHost->mWifiRunningTimers, mHost->mOnBatteryTimeBase);
    mFullWifiLockTimer = new StopwatchTimer(this, IBatteryStats::FULL_WIFI_LOCK,
            &mHost->mFullWifiLockTimers, mHost->mOnBatteryTimeBase);
    mWifiScanTimer = new StopwatchTimer(this, IBatteryStats::WIFI_SCAN,
            &mHost->mWifiScanTimers, mHost->mOnBatteryTimeBase);
    mWifiBatchedScanTimer = ArrayOf<StopwatchTimer*>::Alloc(IBatteryStatsUid::NUM_WIFI_BATCHED_SCAN_BINS);
    mWifiMulticastTimer = new StopwatchTimer(this, IBatteryStats::WIFI_MULTICAST_ENABLED,
            &mHost->mWifiMulticastTimers, mHost->mOnBatteryTimeBase);
    mProcessStateTimer = ArrayOf<StopwatchTimer*>::Alloc(IBatteryStatsUid::NUM_PROCESS_STATE);
}

ECode BatteryStatsImpl::Uid::GetWakelockStats(
    /* [out] */ IMap** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = IMap::Probe(mWakelockStats->GetMap());
    REFCOUNT_ADD(*stats);
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::GetSyncStats(
    /* [out] */ IMap** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = IMap::Probe(mSyncStats->GetMap());
    REFCOUNT_ADD(*stats);
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::GetJobStats(
    /* [out] */ IMap** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = IMap::Probe(mJobStats->GetMap());
    REFCOUNT_ADD(*stats);
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::GetSensorStats(
    /* [out] */ ISparseArray** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = mSensorStats;
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::GetProcessStats(
    /* [out] */ IMap** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = IMap::Probe(mProcessStats);
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::GetPackageStats(
    /* [out] */ IMap** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = IMap::Probe(mPackageStats);
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid = mUid;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteWifiRunningLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (!mWifiRunning) {
        mWifiRunning = TRUE;
        if (mWifiRunningTimer == NULL) {
            mWifiRunningTimer = new StopwatchTimer(this, IBatteryStats::WIFI_RUNNING,
                    &mHost->mWifiRunningTimers, mHost->mOnBatteryTimeBase);
        }
        mWifiRunningTimer->StartRunningLocked(elapsedRealtimeMs);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteWifiStoppedLocked(
    /* [in] */ Int64 elapsedRealtime)
{
    if (mWifiRunning) {
        mWifiRunning = FALSE;
        mWifiRunningTimer->StopRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteFullWifiLockAcquiredLocked(
    /* [in] */ Int64 elapsedRealtime)
{
    if (!mFullWifiLockOut) {
        mFullWifiLockOut = TRUE;
        if (mFullWifiLockTimer == NULL) {
            mFullWifiLockTimer = new StopwatchTimer(this, IBatteryStats::FULL_WIFI_LOCK,
                    &mHost->mFullWifiLockTimers, mHost->mOnBatteryTimeBase);
        }
        mFullWifiLockTimer->StartRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteFullWifiLockReleasedLocked(
    /* [in] */ Int64 elapsedRealtime)
{
    if (mFullWifiLockOut) {
        mFullWifiLockOut = FALSE;
        mFullWifiLockTimer->StopRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteWifiScanStartedLocked(
    /* [in] */ Int64 elapsedRealtime)
{
    if (!mWifiScanStarted) {
        mWifiScanStarted = TRUE;
        if (mWifiScanTimer == NULL) {
            mWifiScanTimer = new StopwatchTimer(this, IBatteryStats::WIFI_SCAN,
                    &mHost->mWifiScanTimers, mHost->mOnBatteryTimeBase);
        }
        mWifiScanTimer->StartRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteWifiScanStoppedLocked(
    /* [in] */ Int64 elapsedRealtime)
{
    if (mWifiScanStarted) {
        mWifiScanStarted = FALSE;
        mWifiScanTimer->StopRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteWifiBatchedScanStartedLocked(
    /* [in] */ Int32 csph,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    Int32 bin = 0;
    while (csph > 8 && bin < NUM_WIFI_BATCHED_SCAN_BINS) {
        csph = csph >> 3;
        bin++;
    }

    if (mWifiBatchedScanBinStarted == bin) return NOERROR;

    if (mWifiBatchedScanBinStarted != NO_BATCHED_SCAN_STARTED) {
        (*mWifiBatchedScanTimer)[mWifiBatchedScanBinStarted]->StopRunningLocked(elapsedRealtimeMs);
    }
    mWifiBatchedScanBinStarted = bin;
    if ((*mWifiBatchedScanTimer)[bin] == NULL) {
        MakeWifiBatchedScanBin(bin, NULL);
    }
    (*mWifiBatchedScanTimer)[bin]->StartRunningLocked(elapsedRealtimeMs);
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteWifiBatchedScanStoppedLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (mWifiBatchedScanBinStarted != NO_BATCHED_SCAN_STARTED) {
        (*mWifiBatchedScanTimer)[mWifiBatchedScanBinStarted]->StopRunningLocked(elapsedRealtimeMs);
        mWifiBatchedScanBinStarted = NO_BATCHED_SCAN_STARTED;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteWifiMulticastEnabledLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (!mWifiMulticastEnabled) {
        mWifiMulticastEnabled = TRUE;
        if (mWifiMulticastTimer == NULL) {
            mWifiMulticastTimer = new StopwatchTimer(this, WIFI_MULTICAST_ENABLED,
                    &mHost->mWifiMulticastTimers, mHost->mOnBatteryTimeBase);
        }
        mWifiMulticastTimer->StartRunningLocked(elapsedRealtimeMs);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteWifiMulticastDisabledLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (mWifiMulticastEnabled) {
        mWifiMulticastEnabled = FALSE;
        mWifiMulticastTimer->StopRunningLocked(elapsedRealtimeMs);
    }
    return NOERROR;
}

AutoPtr<BatteryStatsImpl::StopwatchTimer> BatteryStatsImpl::Uid::CreateAudioTurnedOnTimerLocked()
{
    if (mAudioTurnedOnTimer == NULL) {
        mAudioTurnedOnTimer = new StopwatchTimer(this, IBatteryStats::AUDIO_TURNED_ON,
                &mHost->mAudioTurnedOnTimers, mHost->mOnBatteryTimeBase);
    }
    return mAudioTurnedOnTimer;
}

void BatteryStatsImpl::Uid::NoteAudioTurnedOnLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    CreateAudioTurnedOnTimerLocked()->StartRunningLocked(elapsedRealtimeMs);
}

void BatteryStatsImpl::Uid::NoteAudioTurnedOffLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (mAudioTurnedOnTimer != NULL) {
        mAudioTurnedOnTimer->StopRunningLocked(elapsedRealtimeMs);
    }
}

void BatteryStatsImpl::Uid::NoteResetAudioLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (mAudioTurnedOnTimer != NULL) {
        mAudioTurnedOnTimer->StopAllRunningLocked(elapsedRealtimeMs);
    }
}

AutoPtr<BatteryStatsImpl::StopwatchTimer> BatteryStatsImpl::Uid::CreateVideoTurnedOnTimerLocked()
{
    if (mVideoTurnedOnTimer == NULL) {
        mVideoTurnedOnTimer = new StopwatchTimer(this, IBatteryStats::VIDEO_TURNED_ON,
                &mHost->mVideoTurnedOnTimers, mHost->mOnBatteryTimeBase);
    }
    return mVideoTurnedOnTimer;
}

void BatteryStatsImpl::Uid::NoteVideoTurnedOnLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    CreateVideoTurnedOnTimerLocked()->StartRunningLocked(elapsedRealtimeMs);
}

void BatteryStatsImpl::Uid::NoteVideoTurnedOffLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (mVideoTurnedOnTimer != NULL) {
        mVideoTurnedOnTimer->StopRunningLocked(elapsedRealtimeMs);
    }
}

void BatteryStatsImpl::Uid::NoteResetVideoLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (mVideoTurnedOnTimer != NULL) {
        mVideoTurnedOnTimer->StopAllRunningLocked(elapsedRealtimeMs);
    }
}

AutoPtr<BatteryStatsImpl::StopwatchTimer> BatteryStatsImpl::Uid::CreateForegroundActivityTimerLocked()
{
    if (mForegroundActivityTimer == NULL) {
        mForegroundActivityTimer = new StopwatchTimer(
                this, IBatteryStats::FOREGROUND_ACTIVITY, NULL, mHost->mOnBatteryTimeBase);
    }
    return mForegroundActivityTimer;
}

ECode BatteryStatsImpl::Uid::NoteActivityResumedLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    // We always start, since we want multiple foreground PIDs to nest
    CreateForegroundActivityTimerLocked()->StartRunningLocked(elapsedRealtimeMs);
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteActivityPausedLocked(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (mForegroundActivityTimer != NULL) {
        mForegroundActivityTimer->StopRunningLocked(elapsedRealtimeMs);
    }
    return NOERROR;
}

void BatteryStatsImpl::Uid::UpdateUidProcessStateLocked(
    /* [in] */ Int32 state,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    if (mProcessState == state) return;

    if (mProcessState != PROCESS_STATE_NONE) {
        (*mProcessStateTimer)[mProcessState]->StopRunningLocked(elapsedRealtimeMs);
    }
    mProcessState = state;
    if (state != PROCESS_STATE_NONE) {
        if ((*mProcessStateTimer)[state] == NULL) {
            MakeProcessState(state, NULL);
        }
        (*mProcessStateTimer)[state]->StartRunningLocked(elapsedRealtimeMs);
    }
}

AutoPtr<BatteryStatsImpl::BatchTimer> BatteryStatsImpl::Uid::CreateVibratorOnTimerLocked()
{
    if (mVibratorOnTimer == NULL) {
        mVibratorOnTimer = new BatchTimer(this, IBatteryStats::VIBRATOR_ON, mHost->mOnBatteryTimeBase);
    }
    return mVibratorOnTimer;
}

void BatteryStatsImpl::Uid::NoteVibratorOnLocked(
    /* [in] */ Int64 durationMillis)
{
    CreateVibratorOnTimerLocked()->AddDuration(mHost, durationMillis);
}

void BatteryStatsImpl::Uid::NoteVibratorOffLocked()
{
    if (mVibratorOnTimer != NULL) {
        mVibratorOnTimer->AbortLastDuration(mHost);
    }
}

ECode BatteryStatsImpl::Uid::GetWifiRunningTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = 0;
    if (mWifiRunningTimer == NULL) {
        return NOERROR;
    }
    return mWifiRunningTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, time);
}

ECode BatteryStatsImpl::Uid::GetFullWifiLockTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = 0;
    if (mFullWifiLockTimer == NULL) {
        return NOERROR;
    }
    return mFullWifiLockTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, time);
}

ECode BatteryStatsImpl::Uid::GetWifiScanTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = 0;
    if (mWifiScanTimer == NULL) {
        return NOERROR;
    }
    return mWifiScanTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, time);
}

ECode BatteryStatsImpl::Uid::GetWifiBatchedScanTime(
    /* [in] */ Int32 csphBin,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = 0;
    if (csphBin < 0 || csphBin >= IBatteryStatsUid::NUM_WIFI_BATCHED_SCAN_BINS) return NOERROR;
    if ((*mWifiBatchedScanTimer)[csphBin] == NULL) {
        return NOERROR;
    }
    return (*mWifiBatchedScanTimer)[csphBin]->GetTotalTimeLocked(elapsedRealtimeUs, which, time);
}

ECode BatteryStatsImpl::Uid::GetWifiMulticastTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = 0;
    if (mWifiMulticastTimer == NULL) {
        return NOERROR;
    }
    return mWifiMulticastTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, time);
}

ECode BatteryStatsImpl::Uid::GetAudioTurnedOnTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = 0;
    if (mAudioTurnedOnTimer == NULL) {
        return NOERROR;
    }
    return mAudioTurnedOnTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, time);
}

ECode BatteryStatsImpl::Uid::GetVideoTurnedOnTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    *time = 0;
    if (mVideoTurnedOnTimer == NULL) {
        return NOERROR;
    }
    return mVideoTurnedOnTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, time);
}

ECode BatteryStatsImpl::Uid::GetForegroundActivityTimer(
    /* [out] */ IBatteryStatsTimer** timer)
{
    VALIDATE_NOT_NULL(timer)
    *timer = (IBatteryStatsTimer*)mForegroundActivityTimer.Get();
    REFCOUNT_ADD(*timer)
    return NOERROR;
}

void BatteryStatsImpl::Uid::MakeProcessState(
    /* [in] */ Int32 i,
    /* [in] */ IParcel* in)
{
    if (i < 0 || i >= IBatteryStatsUid::NUM_PROCESS_STATE) return;

    if (in == NULL) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(this, PROCESS_STATE, NULL,
                mHost->mOnBatteryTimeBase);
        mProcessStateTimer->Set(i, timer);
    }
    else {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(this, PROCESS_STATE, NULL,
                mHost->mOnBatteryTimeBase, in);
    }
}

ECode BatteryStatsImpl::Uid::GetProcessStateTime(
    /* [in] */ Int32 state,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (state < 0 || state >= IBatteryStatsUid::NUM_PROCESS_STATE) return NOERROR;
    if ((*mProcessStateTimer)[state] == NULL) {
        return NOERROR;
    }
    return (*mProcessStateTimer)[state]->GetTotalTimeLocked(elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::Uid::GetVibratorOnTimer(
    /* [out] */ IBatteryStatsTimer** timer)
{
    VALIDATE_NOT_NULL(timer)
    *timer = mVibratorOnTimer;
    REFCOUNT_ADD(*timer)
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::NoteUserActivityLocked(
    /* [in] */ Int32 type)
{
    if (mUserActivityCounters == NULL) {
        InitUserActivityLocked();
    }
    if (type >= 0 && type < NUM_USER_ACTIVITY_TYPES) {
        (*mUserActivityCounters)[type]->StepAtomic();
    }
    else {
        Logger::W(TAG, "Unknown user activity type %d was specified.", type);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::HasUserActivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mUserActivityCounters != NULL;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::GetUserActivityCount(
    /* [in] */ Int32 type,
    /* [in] */ Int32 which,
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;
    if (mUserActivityCounters == NULL) {
        return NOERROR;
    }
    return (*mUserActivityCounters)[type]->GetCountLocked(which, count);
}

void BatteryStatsImpl::Uid::MakeWifiBatchedScanBin(
    /* [in] */ Int32 i,
    /* [in] */ IParcel* in)
{
    if (i < 0 || i >= IBatteryStatsUid::NUM_WIFI_BATCHED_SCAN_BINS) return;

    AutoPtr<List<AutoPtr<StopwatchTimer> > > collected;
    HashMap<Int32, AutoPtr<List<AutoPtr<StopwatchTimer> > > >::Iterator it = mHost->mWifiBatchedScanTimers.Find(i);
    if (it != mHost->mWifiBatchedScanTimers.End()) {
        collected = it->mSecond;
    }
    if (collected == NULL) {
        collected = new List<AutoPtr<StopwatchTimer> >();
        mHost->mWifiBatchedScanTimers[i] = collected;
    }
    if (in == NULL) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(this, IBatteryStats::WIFI_BATCHED_SCAN, collected,
                mHost->mOnBatteryTimeBase);
        mWifiBatchedScanTimer->Set(i, timer);
    }
    else {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(this, IBatteryStats::WIFI_BATCHED_SCAN, collected,
                mHost->mOnBatteryTimeBase, in);
        mWifiBatchedScanTimer->Set(i, timer);
    }
}

void BatteryStatsImpl::Uid::InitUserActivityLocked()
{
    mUserActivityCounters = ArrayOf<Counter*>::Alloc(NUM_USER_ACTIVITY_TYPES);
    for (Int32 i = 0; i < NUM_USER_ACTIVITY_TYPES; i++) {
        AutoPtr<Counter> counter = new Counter(mHost->mOnBatteryTimeBase);
        mUserActivityCounters->Set(i, counter);
    }
}

void BatteryStatsImpl::Uid::NoteNetworkActivityLocked(
    /* [in] */ Int32 type,
    /* [in] */ Int64 deltaBytes,
    /* [in] */ Int64 deltaPackets)
{
    if (mNetworkByteActivityCounters == NULL) {
        InitNetworkActivityLocked();
    }
    if (type >= 0 && type < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES) {
        (*mNetworkByteActivityCounters)[type]->AddCountLocked(deltaBytes);
        (*mNetworkPacketActivityCounters)[type]->AddCountLocked(deltaPackets);
    }
    else {
        Slogger::W(TAG, "Unknown network activity type %d was specified.", type);
    }
}

void BatteryStatsImpl::Uid::NoteMobileRadioActiveTimeLocked(
    /* [in] */ Int64 batteryUptime)
{
    if (mNetworkByteActivityCounters == NULL) {
        InitNetworkActivityLocked();
    }
    mMobileRadioActiveTime->AddCountLocked(batteryUptime);
    mMobileRadioActiveCount->AddCountLocked(1);
}

ECode BatteryStatsImpl::Uid::HasNetworkActivity(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNetworkByteActivityCounters != NULL;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::GetNetworkActivityBytes(
    /* [in] */ Int32 type,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mNetworkByteActivityCounters != NULL && type >= 0
            && type < mNetworkByteActivityCounters->GetLength()) {
        (*mNetworkByteActivityCounters)[type]->GetCountLocked(which, result);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::GetNetworkActivityPackets(
    /* [in] */ Int32 type,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (mNetworkPacketActivityCounters != NULL && type >= 0
            && type < mNetworkPacketActivityCounters->GetLength()) {
        (*mNetworkPacketActivityCounters)[type]->GetCountLocked(which, result);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::GetMobileRadioActiveTime(
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 count;
    *result = mMobileRadioActiveTime != NULL
            ? (mMobileRadioActiveTime->GetCountLocked(which, &count), count) : 0;
    return NOERROR;
}

ECode BatteryStatsImpl::Uid::GetMobileRadioActiveCount(
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 count;
    *result = mMobileRadioActiveCount != NULL
            ? (mMobileRadioActiveCount->GetCountLocked(which, &count), (Int32)count) : 0;
    return NOERROR;
}

void BatteryStatsImpl::Uid::InitNetworkActivityLocked()
{
    mNetworkByteActivityCounters = ArrayOf<Int64SamplingCounter*>::Alloc(IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES);
    mNetworkPacketActivityCounters = ArrayOf<Int64SamplingCounter*>::Alloc(IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES);
    for (Int32 i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
        AutoPtr<Int64SamplingCounter> c1 = new Int64SamplingCounter(mHost->mOnBatteryTimeBase);
        mNetworkByteActivityCounters->Set(i, c1);
        AutoPtr<Int64SamplingCounter> c2 = new Int64SamplingCounter(mHost->mOnBatteryTimeBase);
        mNetworkByteActivityCounters->Set(i, c2);
    }
    mMobileRadioActiveTime = new Int64SamplingCounter(mHost->mOnBatteryTimeBase);
    mMobileRadioActiveCount = new Int64SamplingCounter(mHost->mOnBatteryTimeBase);
}

Boolean BatteryStatsImpl::Uid::Reset()
{
    Boolean active = FALSE;

    if (mWifiRunningTimer != NULL) {
        active |= !mWifiRunningTimer->Reset(FALSE);
        active |= mWifiRunning;
    }
    if (mFullWifiLockTimer != NULL) {
        active |= !mFullWifiLockTimer->Reset(FALSE);
        active |= mFullWifiLockOut;
    }
    if (mWifiScanTimer != NULL) {
        active |= !mWifiScanTimer->Reset(FALSE);
        active |= mWifiScanStarted;
    }
    if (mWifiBatchedScanTimer != NULL) {
        for (Int32 i = 0; i < IBatteryStatsUid::NUM_WIFI_BATCHED_SCAN_BINS; i++) {
            if ((*mWifiBatchedScanTimer)[i] != NULL) {
                active |= !(*mWifiBatchedScanTimer)[i]->Reset(FALSE);
            }
        }
        active |= (mWifiBatchedScanBinStarted != NO_BATCHED_SCAN_STARTED);
    }
    if (mWifiMulticastTimer != NULL) {
        active |= !mWifiMulticastTimer->Reset(FALSE);
        active |= mWifiMulticastEnabled;
    }
    if (mAudioTurnedOnTimer != NULL) {
        active |= !mAudioTurnedOnTimer->Reset(FALSE);
    }
    if (mVideoTurnedOnTimer != NULL) {
        active |= !mVideoTurnedOnTimer->Reset(FALSE);
    }
    if (mForegroundActivityTimer != NULL) {
        active |= !mForegroundActivityTimer->Reset(FALSE);
    }
    if (mProcessStateTimer != NULL) {
        for (Int32 i = 0; i < IBatteryStatsUid::NUM_PROCESS_STATE; i++) {
            if ((*mProcessStateTimer)[i] != NULL) {
                active |= !(*mProcessStateTimer)[i]->Reset(FALSE);
            }
        }
        active |= (mProcessState != PROCESS_STATE_NONE);
    }
    if (mVibratorOnTimer != NULL) {
        if (mVibratorOnTimer->Reset(FALSE)) {
            mVibratorOnTimer->Detach();
            mVibratorOnTimer = NULL;
        }
        else {
            active = TRUE;
        }
    }

    if (mUserActivityCounters != NULL) {
        for (Int32 i = 0; i < IBatteryStatsUid::NUM_USER_ACTIVITY_TYPES; i++) {
            (*mUserActivityCounters)[i]->Reset(FALSE);
        }
    }

    if (mNetworkByteActivityCounters != NULL) {
        for (Int32 i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
            (*mNetworkByteActivityCounters)[i]->Reset(FALSE);
            (*mNetworkPacketActivityCounters)[i]->Reset(FALSE);
        }
        mMobileRadioActiveTime->Reset(FALSE);
        mMobileRadioActiveCount->Reset(FALSE);
    }

    AutoPtr<IArrayMap> wakeStats = mWakelockStats->GetMap();
    Int32 size;
    wakeStats->GetSize(&size);
    for (Int32 iw = size - 1; iw >= 0; iw--) {
        AutoPtr<IInterface> value;
        wakeStats->GetValueAt(iw, (IInterface**)&value);
        AutoPtr<Wakelock> wl = (Wakelock*)(IObject*)value.Get();
        if (wl->Reset()) {
            wakeStats->RemoveAt(iw);
        }
        else {
            active = TRUE;
        }
    }
    mWakelockStats->Cleanup();
    AutoPtr<IArrayMap> syncStats = mSyncStats->GetMap();
    syncStats->GetSize(&size);
    for (Int32 is= size - 1; is >= 0; is--) {
        AutoPtr<IInterface> value;
        syncStats->GetValueAt(is, (IInterface**)&value);
        AutoPtr<BatteryStatsImpl::StopwatchTimer> timer = (BatteryStatsImpl::StopwatchTimer*)(IObject*)value.Get();
        if (timer->Reset(FALSE)) {
            syncStats->RemoveAt(is);
            timer->Detach();
        }
        else {
            active = TRUE;
        }
    }
    mSyncStats->Cleanup();
    AutoPtr<IArrayMap> jobStats = mJobStats->GetMap();
    syncStats->GetSize(&size);
    for (Int32 ij= size - 1; ij >= 0; ij--) {
        AutoPtr<IInterface> value;
        jobStats->GetValueAt(ij, (IInterface**)&value);
        AutoPtr<BatteryStatsImpl::StopwatchTimer> timer = (BatteryStatsImpl::StopwatchTimer*)(IObject*)value.Get();
        if (timer->Reset(FALSE)) {
            jobStats->RemoveAt(ij);
            timer->Detach();
        }
        else {
            active = TRUE;
        }
    }
    mJobStats->Cleanup();
    mSensorStats->GetSize(&size);
    for (Int32 ise = size - 1; ise >= 0; ise--) {
        AutoPtr<IInterface> value;
        mSensorStats->ValueAt(ise, (IInterface**)&value);
        AutoPtr<Sensor> s = (Sensor*)(IObject*)value.Get();
        if (s->Reset()) {
            mSensorStats->RemoveAt(ise);
        }
        else {
            active = TRUE;
        }
    }
    mProcessStats->GetSize(&size);
    for (Int32 ip = size - 1; ip >= 0; ip--) {
        AutoPtr<IInterface> value;
        mProcessStats->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<Proc> proc = (Proc*)(IObject*)value.Get();
        if (proc->mProcessState == PROCESS_STATE_NONE) {
            proc->Detach();
            mProcessStats->RemoveAt(ip);
        }
        else {
            proc->Reset();
            active = TRUE;
        }
    }

    if (mPids->GetSize(&size), size > 0) {
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> value;
            mPids->ValueAt(i, (IInterface**)&value);
            AutoPtr<Pid> pid = (Pid*)(IObject*)value.Get();
            if (pid->mWakeNesting > 0) {
                active = TRUE;
            }
            else {
                mPids->RemoveAt(i);
            }
        }
    }

    if (mPackageStats->GetSize(&size), size > 0) {
        AutoPtr<ISet> set;
        mPackageStats->GetEntrySet((ISet**)&set);
        AutoPtr<IIterator> it;
        set->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            AutoPtr<IMapEntry> pkgEntry = IMapEntry::Probe(next);
            AutoPtr<IInterface> value;
            pkgEntry->GetValue((IInterface**)&value);
            AutoPtr<Pkg> p = (Pkg*)(IObject*)value.Get();
            p->Detach();
            Int32 size;
            p->mServiceStats->GetSize(&size);
            if (size > 0) {
                AutoPtr<ISet> set;
                p->mServiceStats->GetEntrySet((ISet**)&set);
                AutoPtr<IIterator> it;
                set->GetIterator((IIterator**)&it);
                Boolean hasNext;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> next;
                    it->GetNext((IInterface**)&next);
                    AutoPtr<IMapEntry> servEntry = IMapEntry::Probe(next);
                    AutoPtr<IInterface> value;
                    servEntry->GetValue((IInterface**)&value);
                    AutoPtr<Pkg::Serv> serv = (Pkg::Serv*)(IObject*)value.Get();
                    serv->Detach();
                }
            }
        }
        mPackageStats->Clear();
    }

    if (!active) {
        if (mWifiRunningTimer != NULL) {
            mWifiRunningTimer->Detach();
        }
        if (mFullWifiLockTimer != NULL) {
            mFullWifiLockTimer->Detach();
        }
        if (mWifiScanTimer != NULL) {
            mWifiScanTimer->Detach();
        }
        for (Int32 i = 0; i < IBatteryStatsUid::NUM_WIFI_BATCHED_SCAN_BINS; i++) {
            if ((*mWifiBatchedScanTimer)[i] != NULL) {
                (*mWifiBatchedScanTimer)[i]->Detach();
            }
        }
        if (mWifiMulticastTimer != NULL) {
            mWifiMulticastTimer->Detach();
        }
        if (mAudioTurnedOnTimer != NULL) {
            mAudioTurnedOnTimer->Detach();
            mAudioTurnedOnTimer = NULL;
        }
        if (mVideoTurnedOnTimer != NULL) {
            mVideoTurnedOnTimer->Detach();
            mVideoTurnedOnTimer = NULL;
        }
        if (mForegroundActivityTimer != NULL) {
            mForegroundActivityTimer->Detach();
            mForegroundActivityTimer = NULL;
        }
        if (mUserActivityCounters != NULL) {
            for (Int32 i = 0; i < NUM_USER_ACTIVITY_TYPES; i++) {
                (*mUserActivityCounters)[i]->Detach();
            }
        }
        if (mNetworkByteActivityCounters != NULL) {
            for (Int32 i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
                (*mNetworkByteActivityCounters)[i]->Detach();
                (*mNetworkPacketActivityCounters)[i]->Detach();
            }
        }
        mPids->Clear();
    }

    return !active;
}

void BatteryStatsImpl::Uid::WriteToParcelLocked(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 elapsedRealtimeUs)
{
    AutoPtr<IArrayMap> wakeStats = mWakelockStats->GetMap();
    Int32 NW;
    wakeStats->GetSize(&NW);
    out->WriteInt32(NW);
    for (Int32 iw = 0; iw < NW; iw++) {
        AutoPtr<IInterface> ki;
        wakeStats->GetKeyAt(iw, (IInterface**)&ki);
        String key;
        ICharSequence::Probe(ki)->ToString(&key);
        out->WriteString(key);
        AutoPtr<IInterface> value;
        wakeStats->GetValueAt(iw, (IInterface**)&value);
        AutoPtr<Wakelock> wakelock = (Wakelock*)(IObject*)value.Get();
        wakelock->WriteToParcelLocked(out, elapsedRealtimeUs);
    }

    AutoPtr<IArrayMap> syncStats = mSyncStats->GetMap();
    Int32 NS;
    syncStats->GetSize(&NS);
    out->WriteInt32(NS);
    for (Int32 is = 0; is < NS; is++) {
        AutoPtr<IInterface> ki;
        syncStats->GetKeyAt(is, (IInterface**)&ki);
        String key;
        ICharSequence::Probe(ki)->ToString(&key);
        out->WriteString(key);
        AutoPtr<IInterface> value;
        syncStats->GetValueAt(is, (IInterface**)&value);
        AutoPtr<BatteryStatsImpl::StopwatchTimer> timer = (BatteryStatsImpl::StopwatchTimer*)(IObject*)value.Get();
        BatteryStatsImpl::Timer::WriteTimerToParcel(out, timer, elapsedRealtimeUs);
    }

    AutoPtr<IArrayMap> jobStats = mJobStats->GetMap();
    Int32 NJ;
    jobStats->GetSize(&NJ);
    out->WriteInt32(NJ);
    for (Int32 ij = 0; ij < NJ; ij++) {
        AutoPtr<IInterface> ki;
        syncStats->GetKeyAt(ij, (IInterface**)&ki);
        String key;
        ICharSequence::Probe(ki)->ToString(&key);
        out->WriteString(key);
        AutoPtr<IInterface> value;
        syncStats->GetValueAt(ij, (IInterface**)&value);
        AutoPtr<BatteryStatsImpl::StopwatchTimer> timer = (BatteryStatsImpl::StopwatchTimer*)(IObject*)value.Get();
        BatteryStatsImpl::Timer::WriteTimerToParcel(out, timer, elapsedRealtimeUs);
    }

    Int32 NSE;
    mSensorStats->GetSize(&NSE);
    out->WriteInt32(NSE);
    for (Int32 ise = 0; ise < NSE; ise++) {
        Int32 key;
        mSensorStats->KeyAt(ise, &key);
        out->WriteInt32(key);
        AutoPtr<IInterface> value;
        mSensorStats->ValueAt(ise, (IInterface**)&value);
        AutoPtr<Sensor> sensor = (Sensor*)(IObject*)value.Get();
        sensor->WriteToParcelLocked(out, elapsedRealtimeUs);
    }

    Int32 NP;
    mProcessStats->GetSize(&NP);
    out->WriteInt32(NP);
    for (Int32 ip = 0; ip < NP; ip++) {
        AutoPtr<IInterface> ki;
        mProcessStats->GetKeyAt(ip, (IInterface**)&ki);
        String key;
        ICharSequence::Probe(ki)->ToString(&key);
        out->WriteString(key);
        AutoPtr<IInterface> value;
        mProcessStats->GetValueAt(ip, (IInterface**)&value);
        AutoPtr<Proc> proc = (Proc*)(IObject*)value.Get();
        proc->WriteToParcelLocked(out);
    }

    Int32 NPS;
    mPackageStats->GetSize(&NPS);
    out->WriteInt32(NPS);
    AutoPtr<ISet> set;
    mPackageStats->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<IMapEntry> pkgEntry = IMapEntry::Probe(pkgEntry);
        AutoPtr<IInterface> value;
        pkgEntry->GetValue((IInterface**)&value);
        AutoPtr<Pkg> pkg = (Pkg*)(IObject*)value.Get();
        pkg->WriteToParcelLocked(out);
    }

    if (mWifiRunningTimer != NULL) {
        out->WriteInt32(1);
        mWifiRunningTimer->WriteToParcel(out, elapsedRealtimeUs);
    }
    else {
        out->WriteInt32(0);
    }
    if (mFullWifiLockTimer != NULL) {
        out->WriteInt32(1);
        mFullWifiLockTimer->WriteToParcel(out, elapsedRealtimeUs);
    }
    else {
        out->WriteInt32(0);
    }
    if (mWifiScanTimer != NULL) {
        out->WriteInt32(1);
        mWifiScanTimer->WriteToParcel(out, elapsedRealtimeUs);
    }
    else {
        out->WriteInt32(0);
    }
    for (Int32 i = 0; i < IBatteryStatsUid::NUM_WIFI_BATCHED_SCAN_BINS; i++) {
        if ((*mWifiBatchedScanTimer)[i] != NULL) {
            out->WriteInt32(1);
            (*mWifiBatchedScanTimer)[i]->WriteToParcel(out, elapsedRealtimeUs);
        }
        else {
            out->WriteInt32(0);
        }
    }
    if (mWifiMulticastTimer != NULL) {
        out->WriteInt32(1);
        mWifiMulticastTimer->WriteToParcel(out, elapsedRealtimeUs);
    }
    else {
        out->WriteInt32(0);
    }
    if (mAudioTurnedOnTimer != NULL) {
        out->WriteInt32(1);
        mAudioTurnedOnTimer->WriteToParcel(out, elapsedRealtimeUs);
    }
    else {
        out->WriteInt32(0);
    }
    if (mVideoTurnedOnTimer != NULL) {
        out->WriteInt32(1);
        mVideoTurnedOnTimer->WriteToParcel(out, elapsedRealtimeUs);
    }
    else {
        out->WriteInt32(0);
    }
    if (mForegroundActivityTimer != NULL) {
        out->WriteInt32(1);
        mForegroundActivityTimer->WriteToParcel(out, elapsedRealtimeUs);
    }
    else {
        out->WriteInt32(0);
    }
    for (Int32 i = 0; i < IBatteryStatsUid::NUM_PROCESS_STATE; i++) {
        if ((*mProcessStateTimer)[i] != NULL) {
            out->WriteInt32(1);
            (*mProcessStateTimer)[i]->WriteToParcel(out, elapsedRealtimeUs);
        }
        else {
            out->WriteInt32(0);
        }
    }
    if (mVibratorOnTimer != NULL) {
        out->WriteInt32(1);
        mVibratorOnTimer->WriteToParcel(out, elapsedRealtimeUs);
    }
    else {
        out->WriteInt32(0);
    }
    if (mUserActivityCounters != NULL) {
        out->WriteInt32(1);
        for (Int32 i = 0; i < IBatteryStatsUid::NUM_USER_ACTIVITY_TYPES; i++) {
            (*mUserActivityCounters)[i]->WriteToParcel(out);
        }
    }
    else {
        out->WriteInt32(0);
    }
    if (mNetworkByteActivityCounters != NULL) {
        out->WriteInt32(1);
        for (Int32 i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
            (*mNetworkByteActivityCounters)[i]->WriteToParcel(out);
            (*mNetworkPacketActivityCounters)[i]->WriteToParcel(out);
        }
        mMobileRadioActiveTime->WriteToParcel(out);
        mMobileRadioActiveCount->WriteToParcel(out);
    }
    else {
        out->WriteInt32(0);
    }
}

void BatteryStatsImpl::Uid::ReadFromParcelLocked(
    /* [in] */ TimeBase* timeBase,
    /* [in] */ TimeBase* screenOffTimeBase,
    /* [in] */ IParcel* in)
{
    Int32 numWakelocks;
    in->ReadInt32(&numWakelocks);
    mWakelockStats->Clear();
    for (Int32 j = 0; j < numWakelocks; j++) {
        String wakelockName;
        in->ReadString(&wakelockName);
        AutoPtr<Wakelock> wakelock = new Wakelock(this);
        wakelock->ReadFromParcelLocked(timeBase, screenOffTimeBase, in);
        mWakelockStats->Add(wakelockName, wakelock);
    }

    Int32 numSyncs;
    in->ReadInt32(&numSyncs);
    mSyncStats->Clear();
    for (Int32 j = 0; j < numSyncs; j++) {
        String syncName;
        in->ReadString(&syncName);
        Int32 v;
        if (in->ReadInt32(&v), v != 0) {
            AutoPtr<BatteryStatsImpl::StopwatchTimer> stopTimer = new BatteryStatsImpl::StopwatchTimer(
                    this, IBatteryStats::SYNC, NULL, timeBase, in);
            mSyncStats->Add(syncName, stopTimer);
        }
    }

    Int32 numJobs;
    in->ReadInt32(&numJobs);
    mJobStats->Clear();
    for (Int32 j = 0; j < numJobs; j++) {
        String jobName;
        in->ReadString(&jobName);
        Int32 v;
        if (in->ReadInt32(&v), v != 0) {
            AutoPtr<BatteryStatsImpl::StopwatchTimer> stopTimer = new BatteryStatsImpl::StopwatchTimer(
                    this, IBatteryStats::JOB, NULL, timeBase, in);
            mJobStats->Add(jobName, stopTimer);
        }
    }

    Int32 numSensors;
    in->ReadInt32(&numSensors);
    mSensorStats->Clear();
    for (Int32 k = 0; k < numSensors; k++) {
        Int32 sensorNumber;
        in->ReadInt32(&sensorNumber);
        AutoPtr<Sensor> sensor = new Sensor(sensorNumber, this);
        sensor->ReadFromParcelLocked(mHost->mOnBatteryTimeBase, in);
        mSensorStats->Put(sensorNumber, (IObject*)sensor.Get());
    }

    Int32 numProcs;
    in->ReadInt32(&numProcs);
    mProcessStats->Clear();
    for (Int32 k = 0; k < numProcs; k++) {
        String processName;
        in->ReadString(&processName);
        AutoPtr<Proc> proc = new Proc(processName, mHost);
        proc->ReadFromParcelLocked(in);
        AutoPtr<ICharSequence> cs;
        CString::New(processName, (ICharSequence**)&cs);
        mProcessStats->Put(cs, (IObject*)proc.Get());
    }

    Int32 numPkgs;
    in->ReadInt32(&numPkgs);
    mPackageStats->Clear();
    for (Int32 l = 0; l < numPkgs; l++) {
        String packageName;
        in->ReadString(&packageName);
        AutoPtr<Pkg> pkg = new Pkg(mHost);
        pkg->ReadFromParcelLocked(in);
        AutoPtr<ICharSequence> cs;
        CString::New(packageName, (ICharSequence**)&cs);
        mPackageStats->Put(cs, (IObject*)pkg.Get());
    }

    mWifiRunning = FALSE;
    Int32 value;
    if (in->ReadInt32(&value), value != 0) {
        mWifiRunningTimer = new BatteryStatsImpl::StopwatchTimer(this, IBatteryStats::WIFI_RUNNING,
                &mHost->mWifiRunningTimers, mHost->mOnBatteryTimeBase, in);
    }
    else {
        mWifiRunningTimer = NULL;
    }
    mFullWifiLockOut = FALSE;
    if (in->ReadInt32(&value), value != 0) {
        mFullWifiLockTimer = new BatteryStatsImpl::StopwatchTimer(this, IBatteryStats::FULL_WIFI_LOCK,
                &mHost->mFullWifiLockTimers, mHost->mOnBatteryTimeBase, in);
    }
    else {
        mFullWifiLockTimer = NULL;
    }
    mWifiScanStarted = FALSE;
    if (in->ReadInt32(&value), value != 0) {
        mWifiScanTimer = new BatteryStatsImpl::StopwatchTimer(this, IBatteryStats::WIFI_SCAN,
                &mHost->mWifiScanTimers, mHost->mOnBatteryTimeBase, in);
    }
    else {
        mWifiScanTimer = NULL;
    }
    mWifiBatchedScanBinStarted = NO_BATCHED_SCAN_STARTED;
    for (Int32 i = 0; i < IBatteryStatsUid::NUM_WIFI_BATCHED_SCAN_BINS; i++) {
        Int32 v;
        if (in->ReadInt32(&v), v != 0) {
            MakeWifiBatchedScanBin(i, in);
        }
        else {
            mWifiBatchedScanTimer->Set(i, NULL);
        }
    }
    mWifiMulticastEnabled = FALSE;
    if (in->ReadInt32(&value), value != 0) {
        mWifiMulticastTimer = new BatteryStatsImpl::StopwatchTimer(this, IBatteryStats::WIFI_MULTICAST_ENABLED,
                &mHost->mWifiMulticastTimers, mHost->mOnBatteryTimeBase, in);
    }
    else {
        mWifiMulticastTimer = NULL;
    }
    if (in->ReadInt32(&value), value != 0) {
        mAudioTurnedOnTimer = new BatteryStatsImpl::StopwatchTimer(this, IBatteryStats::AUDIO_TURNED_ON,
                &mHost->mAudioTurnedOnTimers, mHost->mOnBatteryTimeBase, in);
    }
    else {
        mAudioTurnedOnTimer = NULL;
    }
    if (in->ReadInt32(&value), value != 0) {
        mVideoTurnedOnTimer = new BatteryStatsImpl::StopwatchTimer(this, IBatteryStats::VIDEO_TURNED_ON,
                &mHost->mAudioTurnedOnTimers, mHost->mOnBatteryTimeBase, in);
    }
    else {
        mVideoTurnedOnTimer = NULL;
    }
    if (in->ReadInt32(&value), value != 0) {
        mForegroundActivityTimer = new BatteryStatsImpl::StopwatchTimer(
                this, IBatteryStats::FOREGROUND_ACTIVITY, NULL, mHost->mOnBatteryTimeBase, in);
    }
    else {
        mForegroundActivityTimer = NULL;
    }
    mProcessState = PROCESS_STATE_NONE;
    for (Int32 i = 0; i < IBatteryStatsUid::NUM_PROCESS_STATE; i++) {
        Int32 v;
        if (in->ReadInt32(&v), v != 0) {
            MakeProcessState(i, in);
        }
        else {
            mProcessStateTimer->Set(i, NULL);
        }
    }
    if (in->ReadInt32(&value), value != 0) {
        mVibratorOnTimer = new BatteryStatsImpl::BatchTimer(this, IBatteryStats::VIBRATOR_ON, mHost->mOnBatteryTimeBase, in);
    }
    else {
        mVibratorOnTimer = NULL;
    }
    if (in->ReadInt32(&value), value != 0) {
        mUserActivityCounters = ArrayOf<BatteryStatsImpl::Counter*>::Alloc(
                IBatteryStatsUid::NUM_USER_ACTIVITY_TYPES);
        for (Int32 i = 0; i < IBatteryStatsUid::NUM_USER_ACTIVITY_TYPES; i++) {
            AutoPtr<BatteryStatsImpl::Counter> counter =
                    new BatteryStatsImpl::Counter(mHost->mOnBatteryTimeBase, in);
            mUserActivityCounters->Set(i, counter);
        }
    }
    else {
        mUserActivityCounters = NULL;
    }
    if (in->ReadInt32(&value), value != 0) {
        mNetworkByteActivityCounters = ArrayOf<BatteryStatsImpl::Int64SamplingCounter*>::Alloc(
                IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES);
        mNetworkPacketActivityCounters = ArrayOf<BatteryStatsImpl::Int64SamplingCounter*>::Alloc(
                IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES);
        for (Int32 i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
            AutoPtr<BatteryStatsImpl::Int64SamplingCounter> counter1 =
                    new BatteryStatsImpl::Int64SamplingCounter(mHost->mOnBatteryTimeBase, in);
            mNetworkByteActivityCounters->Set(i, counter1);
            AutoPtr<BatteryStatsImpl::Int64SamplingCounter> counter2 =
                    new BatteryStatsImpl::Int64SamplingCounter(mHost->mOnBatteryTimeBase, in);
            mNetworkPacketActivityCounters->Set(i, counter2);
        }
        mMobileRadioActiveTime = new BatteryStatsImpl::Int64SamplingCounter(mHost->mOnBatteryTimeBase, in);
        mMobileRadioActiveCount = new BatteryStatsImpl::Int64SamplingCounter(mHost->mOnBatteryTimeBase, in);
    }
    else {
        mNetworkByteActivityCounters = NULL;
        mNetworkPacketActivityCounters = NULL;
    }
}

AutoPtr<BatteryStatsImpl::Uid::Proc> BatteryStatsImpl::Uid::GetProcessStatsLocked(
    /* [in] */ const String& name)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    mProcessStats->Get(cs, (IInterface**)&value);
    AutoPtr<Proc> ps;
    if (value == NULL) {
        ps = new Proc(name, mHost);
        mProcessStats->Put(cs, (IObject*)ps);
    }
    else {
        ps = (Proc*)(IObject*)value.Get();
    }

    return ps;
}

void BatteryStatsImpl::Uid::UpdateProcessStateLocked(
    /* [in] */ const String& procName,
    /* [in] */ Int32 state,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    Int32 procState;
    if (state <= IActivityManager::PROCESS_STATE_IMPORTANT_FOREGROUND) {
        procState = IBatteryStatsUid::PROCESS_STATE_FOREGROUND;
    }
    else if (state <= IActivityManager::PROCESS_STATE_RECEIVER) {
        procState = IBatteryStatsUid::PROCESS_STATE_ACTIVE;
    }
    else {
        procState = IBatteryStatsUid::PROCESS_STATE_RUNNING;
    }
    UpdateRealProcessStateLocked(procName, procState, elapsedRealtimeMs);
}

void BatteryStatsImpl::Uid::UpdateRealProcessStateLocked(
    /* [in] */ const String& procName,
    /* [in] */ Int32 procState,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    AutoPtr<Proc> proc = GetProcessStatsLocked(procName);
    if (proc->mProcessState != procState) {
        Boolean changed;
        if (procState < proc->mProcessState) {
            // Has this process become more important?  If so,
            // we may need to change the uid if the currrent uid proc state
            // is not as important as what we are now setting.
            changed = mProcessState > procState;
        }
        else {
            // Has this process become less important?  If so,
            // we may need to change the uid if the current uid proc state
            // is the same importance as the old setting.
            changed = mProcessState == proc->mProcessState;
        }
        proc->mProcessState = procState;
        if (changed) {
            // uid's state may have changed; compute what the new state should be.
            Int32 uidProcState = PROCESS_STATE_NONE;
            Int32 size;
            mProcessStats->GetSize(&size);
            for (Int32 ip = size - 1; ip >= 0; ip--) {
                AutoPtr<IInterface> value;
                mProcessStats->GetValueAt(ip, (IInterface**)&value);
                proc = (Proc*)(IObject*)value.Get();
                if (proc->mProcessState < uidProcState) {
                    uidProcState = proc->mProcessState;
                }
            }
            UpdateUidProcessStateLocked(uidProcState, elapsedRealtimeMs);
        }
    }
}

ECode BatteryStatsImpl::Uid::GetPidStats(
    /* [out] */ ISparseArray** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = mPids;
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

AutoPtr<BatteryStatsImpl::Uid::Pid> BatteryStatsImpl::Uid::GetPidStatsLocked(
    /* [in] */ Int32 pid)
{
    AutoPtr<IInterface> value;
    mPids->Get(pid, (IInterface**)&value);
    AutoPtr<Pid> p;
    if (value == NULL) {
        p = new Pid();
        mPids->Put(pid, (IObject*)p);
    }
    else {
        p = (Pid*)(IObject*)value.Get();
    }
    return p;
}

AutoPtr<BatteryStatsImpl::Uid::Pkg> BatteryStatsImpl::Uid::GetPackageStatsLocked(
    /* [in] */ const String& name)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    mPackageStats->Get(cs, (IInterface**)&value);
    AutoPtr<Pkg> ps;
    if (value == NULL) {
        ps = new Pkg(mHost);
        mPackageStats->Put(cs, (IObject*)ps.Get());
    }
    else {
        ps = (Pkg*)(IObject*)value.Get();
    }

    return ps;
}

AutoPtr<BatteryStatsImpl::Uid::Pkg::Serv> BatteryStatsImpl::Uid::GetServiceStatsLocked(
    /* [in] */ const String& pkg,
    /* [in] */ const String& serv)
{
    AutoPtr<Pkg> ps = GetPackageStatsLocked(pkg);

    AutoPtr<ICharSequence> cs;
    CString::New(serv, (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    ps->mServiceStats->Get(cs, (IInterface**)&value);
    AutoPtr<Pkg::Serv> ss;
    if (value == NULL) {
        ss = ps->NewServiceStatsLocked();
        ps->mServiceStats->Put(cs, (IObject*)ss.Get());
    }
    else {
        ss = (Pkg::Serv*)(IObject*)value.Get();
    }

    return ss;
}

void BatteryStatsImpl::Uid::ReadSyncSummaryFromParcelLocked(
    /* [in] */ const String& name,
    /* [in] */ IParcel* in)
{
    AutoPtr<BatteryStatsImpl::StopwatchTimer> timer = mSyncStats->InstantiateObject();
    timer->ReadSummaryFromParcelLocked(in);
    mSyncStats->Add(name, timer);
}

void BatteryStatsImpl::Uid::ReadJobSummaryFromParcelLocked(
    /* [in] */ const String& name,
    /* [in] */ IParcel* in)
{
    AutoPtr<BatteryStatsImpl::StopwatchTimer> timer = mJobStats->InstantiateObject();
    timer->ReadSummaryFromParcelLocked(in);
    mJobStats->Add(name, timer);
}

void BatteryStatsImpl::Uid::ReadWakeSummaryFromParcelLocked(
    /* [in] */ const String& wlName,
    /* [in] */ IParcel* in)
{
    AutoPtr<Wakelock> wl = new Wakelock(this);
    mWakelockStats->Add(wlName, wl);
    Int32 value;
    AutoPtr<StopwatchTimer> timer;
    if (in->ReadInt32(&value), value != 0) {
        wl->GetStopwatchTimer(IBatteryStats::WAKE_TYPE_FULL, (StopwatchTimer**)&timer);
        if (timer != NULL) {
            timer->ReadSummaryFromParcelLocked(in);
        }
    }
    if (in->ReadInt32(&value), value != 0) {
        wl->GetStopwatchTimer(IBatteryStats::WAKE_TYPE_PARTIAL, (StopwatchTimer**)&timer);
        if (timer != NULL) {
            timer->ReadSummaryFromParcelLocked(in);
        }
    }
    if (in->ReadInt32(&value), value != 0) {
        wl->GetStopwatchTimer(IBatteryStats::WAKE_TYPE_WINDOW, (StopwatchTimer**)&timer);
        if (timer != NULL) {
            timer->ReadSummaryFromParcelLocked(in);
        }
    }
}

AutoPtr<BatteryStatsImpl::StopwatchTimer> BatteryStatsImpl::Uid::GetSensorTimerLocked(
    /* [in] */ Int32 sensor,
    /* [in] */ Boolean create)
{
    AutoPtr<IInterface> value;
    mSensorStats->Get(sensor, (IInterface**)&value);
    AutoPtr<Sensor> se;
    if (value == NULL) {
        if (!create) {
            return NULL;
        }
        se = new Sensor(sensor, this);
        mSensorStats->Put(sensor, (IObject*)se.Get());
    }
    else {
        se = (Sensor*)(IObject*)value.Get();
    }

    AutoPtr<BatteryStatsImpl::StopwatchTimer> t = se->mTimer;
    if (t != NULL) {
        return t;
    }
    AutoPtr< List<AutoPtr<BatteryStatsImpl::StopwatchTimer> > > timers;
    HashMap<Int32, AutoPtr< List<AutoPtr<BatteryStatsImpl::StopwatchTimer> > > >::Iterator it = mHost->mSensorTimers.Find(sensor);
    if (it != mHost->mSensorTimers.End()) {
        timers = it->mSecond;
    }
    if (timers == NULL) {
        timers = new List<AutoPtr<BatteryStatsImpl::StopwatchTimer> >();
        (mHost->mSensorTimers)[sensor] = timers;
    }
    t = new BatteryStatsImpl::StopwatchTimer(this, IBatteryStats::SENSOR, timers, mHost->mOnBatteryTimeBase);
    se->mTimer = t;
    return t;
}

void BatteryStatsImpl::Uid::NoteStartSyncLocked(
    /* [in] */ const String& name,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    AutoPtr<BatteryStatsImpl::StopwatchTimer> t = mSyncStats->StartObject(name);
    if (t != NULL) {
        t->StartRunningLocked(elapsedRealtimeMs);
    }
}

void BatteryStatsImpl::Uid::NoteStopSyncLocked(
    /* [in] */ const String& name,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    AutoPtr<BatteryStatsImpl::StopwatchTimer> t = mSyncStats->StopObject(name);
    if (t != NULL) {
        t->StopRunningLocked(elapsedRealtimeMs);
    }
}

void BatteryStatsImpl::Uid::NoteStartJobLocked(
    /* [in] */ const String& name,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    AutoPtr<BatteryStatsImpl::StopwatchTimer> t = mJobStats->StartObject(name);
    if (t != NULL) {
        t->StartRunningLocked(elapsedRealtimeMs);
    }
}

void BatteryStatsImpl::Uid::NoteStopJobLocked(
    /* [in] */ const String& name,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    AutoPtr<BatteryStatsImpl::StopwatchTimer> t = mJobStats->StopObject(name);
    if (t != NULL) {
        t->StopRunningLocked(elapsedRealtimeMs);
    }
}

void BatteryStatsImpl::Uid::NoteStartWakeLocked(
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    AutoPtr<Wakelock> wl = mWakelockStats->StartObject(name);
    if (wl != NULL) {
        AutoPtr<StopwatchTimer> timer;
        wl->GetStopwatchTimer(type, (StopwatchTimer**)&timer);
        timer->StartRunningLocked(elapsedRealtimeMs);
    }
    if (pid >= 0 && type == IBatteryStats::WAKE_TYPE_PARTIAL) {
        AutoPtr<Pid> p = GetPidStatsLocked(pid);
        if (p->mWakeNesting++ == 0) {
            p->mWakeStartMs = elapsedRealtimeMs;
        }
    }
}

void BatteryStatsImpl::Uid::NoteStopWakeLocked(
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    AutoPtr<Wakelock> wl = mWakelockStats->StopObject(name);
    if (wl != NULL) {
        AutoPtr<StopwatchTimer> timer;
        wl->GetStopwatchTimer(type, (StopwatchTimer**)&timer);
        timer->StopRunningLocked(elapsedRealtimeMs);
    }
    if (pid >= 0 && type == IBatteryStats::WAKE_TYPE_PARTIAL) {
        AutoPtr<IInterface> value;
        mPids->Get(pid, (IInterface**)&value);
        AutoPtr<Pid> p = (Pid*)(IObject*)value.Get();
        if (p != NULL && p->mWakeNesting > 0) {
            if (p->mWakeNesting-- == 1) {
                p->mWakeSumMs += elapsedRealtimeMs - p->mWakeStartMs;
                p->mWakeStartMs = 0;
            }
        }
    }
}

void BatteryStatsImpl::Uid::ReportExcessiveWakeLocked(
    /* [in] */ const String& proc,
    /* [in] */ Int64 overTime,
    /* [in] */ Int64 usedTime)
{
    AutoPtr<Proc> p = GetProcessStatsLocked(proc);
    if (p != NULL) {
        p->AddExcessiveWake(overTime, usedTime);
    }
}

void BatteryStatsImpl::Uid::ReportExcessiveCpuLocked(
    /* [in] */ const String& proc,
    /* [in] */ Int64 overTime,
    /* [in] */ Int64 usedTime)
{
    AutoPtr<Proc> p = GetProcessStatsLocked(proc);
    if (p != NULL) {
        p->AddExcessiveCpu(overTime, usedTime);
    }
}

void BatteryStatsImpl::Uid::NoteStartSensor(
    /* [in] */ Int32 sensor,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    AutoPtr<StopwatchTimer> t = GetSensorTimerLocked(sensor, TRUE);
    if (t != NULL) {
        t->StartRunningLocked(elapsedRealtimeMs);
    }
}

void BatteryStatsImpl::Uid::NoteStopSensor(
    /* [in] */ Int32 sensor,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    // Don't create a timer if one doesn't already exist
    AutoPtr<StopwatchTimer> t = GetSensorTimerLocked(sensor, FALSE);
    if (t != NULL) {
        t->StopRunningLocked(elapsedRealtimeMs);
    }
}

void BatteryStatsImpl::Uid::NoteStartGps(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    AutoPtr<StopwatchTimer> t = GetSensorTimerLocked(Sensor::GPS, TRUE);
    if (t != NULL) {
        t->StartRunningLocked(elapsedRealtimeMs);
    }
}

void BatteryStatsImpl::Uid::NoteStopGps(
    /* [in] */ Int64 elapsedRealtimeMs)
{
    AutoPtr<StopwatchTimer> t = GetSensorTimerLocked(Sensor::GPS, FALSE);
    if (t != NULL) {
        t->StopRunningLocked(elapsedRealtimeMs);
    }
}

AutoPtr<BatteryStatsImpl> BatteryStatsImpl::Uid::GetBatteryStats()
{
    return mHost;
}


//==============================================================================
// BatteryStatsImpl::SetOnBatteryRunnable
//==============================================================================

ECode BatteryStatsImpl::SetOnBatteryRunnable::Run()
{
    {    AutoLock syncLock(mHost->mCheckinFileLock);
        AutoPtr<IFileOutputStream> stream;
        // try {
        ECode ec = mHost->mCheckinFile->StartWrite((IFileOutputStream**)&stream);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::W("BatteryStats", "Error writing checkin battery statistics");
            mHost->mCheckinFile->FailWrite(stream);
            // mParcel->Recycle();
        }
        AutoPtr<ArrayOf<Byte> > bytes;
        mParcel->Marshall((ArrayOf<Byte>**)&bytes);
        ec = IOutputStream::Probe(stream)->Write(bytes);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::W("BatteryStats", "Error writing checkin battery statistics");
            mHost->mCheckinFile->FailWrite(stream);
            // mParcel->Recycle();
        }
        IFlushable::Probe(stream)->Flush();
        FileUtils::Sync(stream);
        ICloseable::Probe(stream)->Close();
        mHost->mCheckinFile->FinishWrite(stream);
        // } catch (IOException e) {
        //     Slog.w("BatteryStats",
        //             "Error writing checkin battery statistics", e);
        //     mCheckinFile.failWrite(stream);
        // } finally {
        //     parcel.recycle();
        // }
    }
    return NOERROR;
}


//==============================================================================
// BatteryStatsImpl::BatteryStatsWriteRunnable
//==============================================================================

ECode BatteryStatsImpl::BatteryStatsWriteRunnable::Run()
{
    mHost->CommitPendingDataToDisk();
    return NOERROR;
}


//==============================================================================
// BatteryStatsImpl
//==============================================================================

const String BatteryStatsImpl::TAG("BatteryStatsImpl");
const Boolean BatteryStatsImpl::DEBUG = FALSE;
const Boolean BatteryStatsImpl::DEBUG_HISTORY = FALSE;
const Boolean BatteryStatsImpl::USE_OLD_HISTORY = FALSE;
const Int32 BatteryStatsImpl::MAGIC = 0xBA757475; // 'BATSTATS'
const Int32 BatteryStatsImpl::VERSION = 114 + (USE_OLD_HISTORY ? 1000 : 0);
const Int32 BatteryStatsImpl::MAX_HISTORY_ITEMS;
const Int32 BatteryStatsImpl::MAX_MAX_HISTORY_ITEMS;
const Int32 BatteryStatsImpl::MAX_WAKELOCKS_PER_UID;

Int32 BatteryStatsImpl::sNumSpeedSteps = 0;
Int32 BatteryStatsImpl::sKernelWakelockUpdateVersion = 0;

const Int32 BatteryStatsImpl::MSG_UPDATE_WAKELOCKS;
const Int32 BatteryStatsImpl::MSG_REPORT_POWER_CHANGE;
const Int64 BatteryStatsImpl::DELAY_UPDATE_WAKELOCKS;
const Int32 BatteryStatsImpl::MAX_HISTORY_BUFFER;
const Int32 BatteryStatsImpl::MAX_MAX_HISTORY_BUFFER;
const Int32 BatteryStatsImpl::MAX_LEVEL_STEPS;
const Int32 BatteryStatsImpl::DELTA_TIME_MASK;
const Int32 BatteryStatsImpl::DELTA_TIME_LONG;
const Int32 BatteryStatsImpl::DELTA_TIME_INT;
const Int32 BatteryStatsImpl::DELTA_TIME_ABS;
const Int32 BatteryStatsImpl::DELTA_BATTERY_LEVEL_FLAG;
const Int32 BatteryStatsImpl::DELTA_STATE_FLAG;
const Int32 BatteryStatsImpl::DELTA_STATE2_FLAG;
const Int32 BatteryStatsImpl::DELTA_WAKELOCK_FLAG;
const Int32 BatteryStatsImpl::DELTA_EVENT_FLAG;
const Int32 BatteryStatsImpl::DELTA_STATE_MASK;
const Int32 BatteryStatsImpl::STATE_BATTERY_STATUS_MASK;
const Int32 BatteryStatsImpl::STATE_BATTERY_STATUS_SHIFT;
const Int32 BatteryStatsImpl::STATE_BATTERY_HEALTH_MASK;
const Int32 BatteryStatsImpl::STATE_BATTERY_HEALTH_SHIFT;
const Int32 BatteryStatsImpl::STATE_BATTERY_PLUG_MASK;
const Int32 BatteryStatsImpl::STATE_BATTERY_PLUG_SHIFT;

static AutoPtr< ArrayOf<Int32> > InitProcWakelocksFormat()
{
    AutoPtr< ArrayOf<Int32> > formats = ArrayOf<Int32>::Alloc(6);
    (*formats)[0] = Process::PROC_TAB_TERM | Process::PROC_OUT_STRING;                // 0: name
    (*formats)[1] = Process::PROC_TAB_TERM | Process::PROC_OUT_LONG;                  // 1: count
    (*formats)[2] = Process::PROC_TAB_TERM;
    (*formats)[3] = Process::PROC_TAB_TERM;
    (*formats)[4] = Process::PROC_TAB_TERM;
    (*formats)[5] = Process::PROC_TAB_TERM | Process::PROC_OUT_LONG;                  // 5: totalTime
    return formats;
}
const AutoPtr< ArrayOf<Int32> > BatteryStatsImpl::PROC_WAKELOCKS_FORMAT = InitProcWakelocksFormat();

static AutoPtr< ArrayOf<Int32> > InitWakeupSourcesFormat()
{
    AutoPtr< ArrayOf<Int32> > formats = ArrayOf<Int32>::Alloc(7);
    (*formats)[0] = Process::PROC_TAB_TERM | Process::PROC_OUT_STRING | Process::PROC_QUOTES;   // 0: name
    (*formats)[1] = Process::PROC_TAB_TERM | Process::PROC_COMBINE | Process::PROC_OUT_LONG;   // 1: count
    (*formats)[2] = Process::PROC_TAB_TERM | Process::PROC_COMBINE;
    (*formats)[3] = Process::PROC_TAB_TERM | Process::PROC_COMBINE;
    (*formats)[4] = Process::PROC_TAB_TERM | Process::PROC_COMBINE;
    (*formats)[5] = Process::PROC_TAB_TERM | Process::PROC_COMBINE;
    (*formats)[6] = Process::PROC_TAB_TERM | Process::PROC_COMBINE | Process::PROC_OUT_LONG;    // 6: totalTime
    return formats;
}
const AutoPtr< ArrayOf<Int32> > BatteryStatsImpl::WAKEUP_SOURCES_FORMAT = InitWakeupSourcesFormat();
const Int32 BatteryStatsImpl::BATTERY_PLUGGED_NONE;
const Int32 BatteryStatsImpl::NET_UPDATE_MOBILE;
const Int32 BatteryStatsImpl::NET_UPDATE_WIFI;
const Int32 BatteryStatsImpl::NET_UPDATE_ALL;

CAR_INTERFACE_IMPL_2(BatteryStatsImpl, BatteryStats, IBatteryStatsImpl, IParcelable)

BatteryStatsImpl::BatteryStatsImpl()
    : mDistributeWakelockCpu(FALSE)
    , mShuttingDown(FALSE)
    , mHistoryBaseTime(0)
    , mHaveBatteryLevel(FALSE)
    , mRecordingHistory(FALSE)
    , mNumHistoryItems(0)
    , mReadHistoryChars(0)
    , mNextHistoryTagIdx(0)
    , mNumHistoryTagChars(0)
    , mHistoryBufferLastPos(-1)
    , mHistoryOverflow(FALSE)
    , mLastHistoryElapsedRealtime(0)
    , mTrackRunningHistoryElapsedRealtime(0)
    , mTrackRunningHistoryUptime(0)
    , mReadOverflow(FALSE)
    , mIteratingHistory(FALSE)
    , mStartCount(0)
    , mStartClockTime(0)
    , mLastRecordedClockTime(0)
    , mLastRecordedClockRealtime(0)
    , mUptime(0)
    , mUptimeStart(0)
    , mRealtime(0)
    , mRealtimeStart(0)
    , mWakeLockNesting(0)
    , mWakeLockImportant(FALSE)
    , mRecordAllHistory(FALSE)
    , mNoAutoReset(FALSE)
    , mScreenState(IDisplay::STATE_UNKNOWN)
    , mScreenBrightnessBin(-1)
    , mInteractive(FALSE)
    , mLowPowerModeEnabled(FALSE)
    , mPhoneOn(FALSE)
    , mAudioOnNesting(0)
    , mVideoOnNesting(0)
    , mFlashlightOn(FALSE)
    , mPhoneSignalStrengthBin(-1)
    , mPhoneSignalStrengthBinRaw(-1)
    , mPhoneDataConnectionType(-1)
    , mWifiOn(FALSE)
    , mGlobalWifiRunning(FALSE)
    , mWifiState(-1)
    , mWifiSupplState(-1)
    , mWifiSignalStrengthBin(-1)
    , mBluetoothOn(FALSE)
    , mBluetoothState(-1)
    , mMobileRadioPowerState(IDataConnectionRealTimeInfo::DC_POWER_STATE_LOW)
    , mMobileRadioActiveStartTime(0)
    , mOnBattery(FALSE)
    , mOnBatteryInternal(FALSE)
    , mDischargeStartLevel(0)
    , mDischargeUnplugLevel(0)
    , mDischargePlugLevel(0)
    , mDischargeCurrentLevel(0)
    , mCurrentBatteryLevel(0)
    , mLowDischargeAmountSinceCharge(0)
    , mHighDischargeAmountSinceCharge(0)
    , mDischargeScreenOnUnplugLevel(0)
    , mDischargeScreenOffUnplugLevel(0)
    , mDischargeAmountScreenOn(0)
    , mDischargeAmountScreenOnSinceCharge(0)
    , mDischargeAmountScreenOff(0)
    , mDischargeAmountScreenOffSinceCharge(0)
    , mInitStepMode(0)
    , mCurStepMode(0)
    , mModStepMode(0)
    , mLastDischargeStepLevel(0)
    , mLastDischargeStepTime(0)
    , mMinDischargeStepLevel(0)
    , mNumDischargeStepDurations(0)
    , mLastChargeStepLevel(0)
    , mLastChargeStepTime(0)
    , mMaxChargeStepLevel(0)
    , mNumChargeStepDurations(0)
    , mLastWriteTime(0)
    , mBluetoothPingCount(0)
    , mBluetoothPingStart(-1)
    , mPhoneServiceState(-1)
    , mPhoneServiceStateRaw(-1)
    , mPhoneSimStateRaw(-1)
    , mLastWakeupUptimeMs(0)
    , mChangedStates(0)
    , mChangedStates2(0)
    , mInitialAcquireWakeUid(-1)
    , mSensorNesting(0)
    , mGpsNesting(0)
    , mWifiFullLockNesting(0)
{
    CSparseArray::New((ISparseArray**)&mUidStats);

    mOnBatteryTimeBase = new TimeBase();
    mOnBatteryScreenOffTimeBase = new TimeBase();
    mActiveEvents = new HistoryEventTracker();
    CParcel::New((IParcel**)&mHistoryBuffer);
    mHistoryLastWritten = new HistoryItem();
    mHistoryLastLastWritten = new HistoryItem();
    mHistoryReadTmp = new HistoryItem();
    mHistoryAddTmp = new HistoryItem();
    mHistoryCur = new HistoryItem();

    mScreenBrightnessTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_SCREEN_BRIGHTNESS_BINS);
    mPhoneSignalStrengthsTimer = ArrayOf<StopwatchTimer*>::Alloc(ISignalStrength::NUM_SIGNAL_STRENGTH_BINS);
    mPhoneDataConnectionsTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_DATA_CONNECTION_TYPES);
    mNetworkByteActivityCounters = ArrayOf<Int64SamplingCounter*>::Alloc(NUM_NETWORK_ACTIVITY_TYPES);
    mNetworkPacketActivityCounters = ArrayOf<Int64SamplingCounter*>::Alloc(NUM_NETWORK_ACTIVITY_TYPES);

    mWifiStateTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_WIFI_STATES);
    mWifiSupplStateTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_WIFI_SUPPL_STATES);
    mWifiSignalStrengthsTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_WIFI_SIGNAL_STRENGTH_BINS);

    mBluetoothStateTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_BLUETOOTH_STATES);

    mDischargeStepDurations = ArrayOf<Int64>::Alloc(MAX_LEVEL_STEPS);
    mChargeStepDurations = ArrayOf<Int64>::Alloc(MAX_LEVEL_STEPS);

    mProcWakelocksName = ArrayOf<String>::Alloc(3);
    mProcWakelocksData = ArrayOf<Int64>::Alloc(3);

    CNetworkStatsFactory::New((INetworkStatsFactory**)&mNetworkStatsFactory);
    CNetworkStats::New(SystemClock::GetElapsedRealtime(), 50, (INetworkStats**)&mCurMobileSnapshot);
    CNetworkStats::New(SystemClock::GetElapsedRealtime(), 50, (INetworkStats**)&mLastMobileSnapshot);
    CNetworkStats::New(SystemClock::GetElapsedRealtime(), 50, (INetworkStats**)&mCurWifiSnapshot);
    CNetworkStats::New(SystemClock::GetElapsedRealtime(), 50, (INetworkStats**)&mLastWifiSnapshot);
    CNetworkStatsEntry::New((INetworkStatsEntry**)&mTmpNetworkStatsEntry);

    mMobileIfaces = ArrayOf<String>::Alloc(0);
    mWifiIfaces = ArrayOf<String>::Alloc(0);

    CReentrantLock::New((IReentrantLock**)&mWriteLock);

    CHashMap::New((IHashMap**)&mKernelWakelockStats);
    CHashMap::New((IHashMap**)&mWakeupReasonStats);
}

ECode BatteryStatsImpl::constructor()
{
    mFile = NULL;
    mCheckinFile = NULL;
    mHandler = NULL;
    ClearHistoryLocked();
    return NOERROR;
}

ECode BatteryStatsImpl::constructor(
    /* [in] */ IFile* systemDir,
    /* [in] */ IHandler* handler)
{
    if (systemDir != NULL) {
        AutoPtr<IFile> f1, f2;
        CFile::New(systemDir, String("batterystats.bin"), (IFile**)&f1);
        CFile::New(systemDir, String("batterystats.bin.tmp"), (IFile**)&f2);
        CJournaledFile::New(f1, f2, (IJournaledFile**)&mFile);
    }
    else {
        mFile = NULL;
    }
    AutoPtr<IFile> f;
    CFile::New(systemDir, String("batterystats-checkin.bin"), (IFile**)&f);
    CAtomicFile::New(f, (IAtomicFile**)&mCheckinFile);
    AutoPtr<ILooper> l;
    handler->GetLooper((ILooper**)&l);
    mHandler = new MyHandler(l, this);
    mStartCount++;
    mScreenOnTimer = new StopwatchTimer(NULL, -1, NULL, mOnBatteryTimeBase);
    for (Int32 i = 0; i < IBatteryStats::NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -100 - i, NULL, mOnBatteryTimeBase);
        mScreenBrightnessTimer->Set(i, timer);
    }
    mInteractiveTimer = new StopwatchTimer(NULL, -9, NULL, mOnBatteryTimeBase);
    mLowPowerModeEnabledTimer = new StopwatchTimer(NULL, -2, NULL, mOnBatteryTimeBase);
    mPhoneOnTimer = new StopwatchTimer(NULL, -3, NULL, mOnBatteryTimeBase);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -200 - i, NULL,
                mOnBatteryTimeBase);
        mPhoneSignalStrengthsTimer->Set(i, timer);
    }
    mPhoneSignalScanningTimer = new StopwatchTimer(NULL, -200 + 1, NULL, mOnBatteryTimeBase);
    for (Int32 i = 0; i < IBatteryStats::NUM_DATA_CONNECTION_TYPES; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -300 - i, NULL,
                mOnBatteryTimeBase);
        mPhoneDataConnectionsTimer->Set(i, timer);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
        AutoPtr<Int64SamplingCounter> c1 = new Int64SamplingCounter(mOnBatteryTimeBase);
        mNetworkByteActivityCounters->Set(i, c1);
        AutoPtr<Int64SamplingCounter> c2 = new Int64SamplingCounter(mOnBatteryTimeBase);
        mNetworkPacketActivityCounters->Set(i, c2);
    }
    mMobileRadioActiveTimer = new StopwatchTimer(NULL, -400, NULL, mOnBatteryTimeBase);
    mMobileRadioActivePerAppTimer = new StopwatchTimer(NULL, -401, NULL, mOnBatteryTimeBase);
    mMobileRadioActiveAdjustedTime = new Int64SamplingCounter(mOnBatteryTimeBase);
    mMobileRadioActiveUnknownTime = new Int64SamplingCounter(mOnBatteryTimeBase);
    mMobileRadioActiveUnknownCount = new Int64SamplingCounter(mOnBatteryTimeBase);
    mWifiOnTimer = new StopwatchTimer(NULL, -4, NULL, mOnBatteryTimeBase);
    mGlobalWifiRunningTimer = new StopwatchTimer(NULL, -5, NULL, mOnBatteryTimeBase);
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_STATES; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -600 - i, NULL,
                mOnBatteryTimeBase);
        mWifiStateTimer->Set(i, timer);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SUPPL_STATES; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -700 - i, NULL,
                mOnBatteryTimeBase);
        mWifiSupplStateTimer->Set(i, timer);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -800 - i, NULL,
                mOnBatteryTimeBase);
        mWifiSignalStrengthsTimer->Set(i, timer);
    }
    mBluetoothOnTimer = new StopwatchTimer(NULL, -6, NULL, mOnBatteryTimeBase);
    for (Int32 i = 0; i < IBatteryStats::NUM_BLUETOOTH_STATES; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -500 - i, NULL,
                mOnBatteryTimeBase);
        mBluetoothStateTimer->Set(i, timer);
    }
    mAudioOnTimer = new StopwatchTimer(NULL, -7, NULL, mOnBatteryTimeBase);
    mVideoOnTimer = new StopwatchTimer(NULL, -8, NULL, mOnBatteryTimeBase);
    mFlashlightOnTimer = new StopwatchTimer(NULL, -9, NULL, mOnBatteryTimeBase);
    mOnBattery = mOnBatteryInternal = FALSE;
    Int64 uptime = SystemClock::GetUptimeMillis() * 1000;
    Int64 realtime = SystemClock::GetElapsedRealtime() * 1000;
    InitTimes(uptime, realtime);
    mStartPlatformVersion = mEndPlatformVersion = Build::ID;
    mDischargeStartLevel = 0;
    mDischargeUnplugLevel = 0;
    mDischargePlugLevel = -1;
    mDischargeCurrentLevel = 0;
    mCurrentBatteryLevel = 0;
    InitDischarge();
    ClearHistoryLocked();
    return NOERROR;
}

void BatteryStatsImpl::HandleUpdateWakelocks()
{
    if (mCallback != NULL) {
        mCallback->BatteryNeedsCpuUpdate();
    }
}

void BatteryStatsImpl::HandleReportPowerChange(
    /* [in] */ Boolean onBattery)
{
    if (mCallback != NULL) {
        mCallback->BatteryPowerChanged(onBattery);
    }
}

ECode BatteryStatsImpl::GetKernelWakelockStats(
    /* [out] */ IMap** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = IMap::Probe(mKernelWakelockStats);
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode BatteryStatsImpl::GetWakeupReasonStats(
    /* [out] */ IMap** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = IMap::Probe(mWakeupReasonStats);
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

AutoPtr<BatteryStatsImpl::SamplingTimer> BatteryStatsImpl::GetWakeupReasonTimerLocked(
    /* [in] */ const String& name)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    mKernelWakelockStats->Get(cs, (IInterface**)&value);
    AutoPtr<SamplingTimer> kwlt;
    if (value == NULL) {
        kwlt = new SamplingTimer(mOnBatteryScreenOffTimeBase, TRUE /* track reported values */);
        mKernelWakelockStats->Put(cs, (IObject*)kwlt);
    }
    else {
        kwlt = (SamplingTimer*)(IObject*)value.Get();
    }
    return kwlt;
}

HashMap<String, AutoPtr<BatteryStatsImpl::KernelWakelockStats> >* BatteryStatsImpl::ReadKernelWakelockStats()
{
    AutoPtr<IFileInputStream> is;
    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(8192);
    Int32 len;
    Boolean wakeup_sources = FALSE;

    // try {
    //     try {
    if (FAILED(CFileInputStream::New(String("/proc/wakelocks"), (IFileInputStream**)&is))) {
        is = NULL;
        if (FAILED(CFileInputStream::New(String("/d/wakeup_sources"), (IFileInputStream**)&is))) {
            return NULL;
        }
        wakeup_sources = TRUE;
    }
    // } catch (java.io.FileNotFoundException e) {
    //     try {
    //         is = new FileInputStream("/d/wakeup_sources");
    //         wakeup_sources = true;
    //     } catch (java.io.FileNotFoundException e2) {
    //         return null;
    //     }
    // }

    IInputStream::Probe(is)->Read(buffer, &len);
    ICloseable::Probe(is)->Close();
    // } catch (java.io.IOException e) {
    //     return null;
    // }

    if (len > 0) {
        Int32 i;
        for (i = 0; i < len; i++) {
            if ((*buffer)[i] == '\0') {
                len = i;
                break;
            }
        }
    }

    return ParseProcWakelocks(buffer, len, wakeup_sources);
}

HashMap<String, AutoPtr<BatteryStatsImpl::KernelWakelockStats> >* BatteryStatsImpl::ParseProcWakelocks(
    /* [in] */ ArrayOf<Byte>* wlBuffer,
    /* [in] */ Int32 len,
    /* [in] */ Boolean wakeup_sources)
{
    String name;
    Int32 count;
    Int64 totalTime;
    Int32 startIndex;
    Int32 endIndex;
    Int32 numUpdatedWlNames = 0;

    // Advance past the first line.
    Int32 i;
    for (i = 0; i < len && (*wlBuffer)[i] != '\n' && (*wlBuffer)[i] != '\0'; i++);
    startIndex = endIndex = i + 1;

    AutoLock lock(this);
    HashMap<String, AutoPtr<KernelWakelockStats> >& m = mProcWakelockFileStats;

    sKernelWakelockUpdateVersion++;
    while (endIndex < len) {
        for (endIndex = startIndex;
                endIndex < len && (*wlBuffer)[endIndex] != '\n' && (*wlBuffer)[endIndex] != '\0';
                endIndex++);
        endIndex++; // endIndex is an exclusive upper bound.
        // Don't go over the end of the buffer, Process.parseProcLine might
        // write to wlBuffer[endIndex]
        if (endIndex >= (len - 1) ) {
            return &m;
        }

        AutoPtr< ArrayOf<String> > nameStringArray = mProcWakelocksName;
        AutoPtr< ArrayOf<Int64> > wlData = mProcWakelocksData;
        // Stomp out any bad characters since this is from a circular buffer
        // A corruption is seen sometimes that results in the vm crashing
        // This should prevent crashes and the line will probably fail to parse
        for (Int32 j = startIndex; j < endIndex; j++) {
            if (((*wlBuffer)[j] & 0x80) != 0) (*wlBuffer)[j] = (Byte) '?';
        }
        Boolean parsed;
        Process::ParseProcLine(wlBuffer, startIndex, endIndex,
                wakeup_sources ? *WAKEUP_SOURCES_FORMAT : *PROC_WAKELOCKS_FORMAT,
                nameStringArray, wlData, NULL, &parsed);

        name = (*nameStringArray)[0];
        count = (Int32)(*wlData)[1];

        if (wakeup_sources) {
            // convert milliseconds to microseconds
            totalTime = (*wlData)[2] * 1000;
        }
        else {
            // convert nanoseconds to microseconds with rounding.
            totalTime = ((*wlData)[2] + 500) / 1000;
        }

        if (parsed && name.GetLength() > 0) {
            HashMap<String, AutoPtr<KernelWakelockStats> >::Iterator it = m.Begin();
            for (; it != m.End(); ++it) {
                if (it->mFirst == name) {
                    AutoPtr<KernelWakelockStats> kwlStats = it->mSecond;
                    if (kwlStats->mVersion == sKernelWakelockUpdateVersion) {
                        kwlStats->mCount += count;
                        kwlStats->mTotalTime += totalTime;
                    }
                    else {
                        kwlStats->mCount = count;
                        kwlStats->mTotalTime = totalTime;
                        kwlStats->mVersion = sKernelWakelockUpdateVersion;
                        numUpdatedWlNames++;
                    }
                    break;
                }
            }
            if (it == m.End()) {
                m[name] = new KernelWakelockStats(count, totalTime,
                        sKernelWakelockUpdateVersion);
                numUpdatedWlNames++;
            }
        }
        startIndex = endIndex;
    }

    if ((Int32)m.GetSize() != numUpdatedWlNames) {
        // Don't report old data.
        HashMap<String, AutoPtr<KernelWakelockStats> >::Iterator it = m.Begin();
        while (it != m.End()) {
            if (it->mSecond->mVersion != sKernelWakelockUpdateVersion) {
                m.Erase(it++);
                continue;
            }
            ++it;
        }
    }
    return &m;
}

AutoPtr<BatteryStatsImpl::SamplingTimer> BatteryStatsImpl::GetKernelWakelockTimerLocked(
    /* [in] */ const String& name)
{
    AutoPtr<ICharSequence> cs;
    CString::New(name, (ICharSequence**)&cs);
    AutoPtr<IInterface> value;
    mKernelWakelockStats->Get(cs, (IInterface**)&value);
    AutoPtr<SamplingTimer> kwlt;
    if (value == NULL) {
        kwlt = new SamplingTimer(mOnBatteryScreenOffTimeBase, TRUE /* track reported values */);
        mKernelWakelockStats->Put(cs, (IObject*)kwlt);
    }
    else {
        kwlt = (SamplingTimer*)(IObject*)value.Get();
    }
    return kwlt;
}

Int32 BatteryStatsImpl::GetCurrentBluetoothPingCount()
{
    if (mBtHeadset != NULL) {
        AutoPtr<IList> deviceList;
        mBtHeadset->GetConnectedDevices((IList**)&deviceList);
        Int32 size;
        if (deviceList->GetSize(&size), size > 0) {
            AutoPtr<IInterface> value;
            deviceList->Get(0, (IInterface**)&value);
            Int32 result;
            mBtHeadset->GetBatteryUsageHint(IBluetoothDevice::Probe(value), &result);
            return result;
        }
    }
    return -1;
}

ECode BatteryStatsImpl::GetBluetoothPingCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;
    if (mBluetoothPingStart == -1) {
        *count = mBluetoothPingCount;
    }
    else if (mBtHeadset != NULL) {
        *count = GetCurrentBluetoothPingCount() - mBluetoothPingStart;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::SetBtHeadset(
    /* [in] */ IBluetoothHeadset* headset)
{
    Boolean isOnBattery;
    if (headset != NULL && mBtHeadset == NULL && (IsOnBattery(&isOnBattery), isOnBattery) && mBluetoothPingStart == -1) {
        mBluetoothPingStart = GetCurrentBluetoothPingCount();
    }
    mBtHeadset = headset;
    return NOERROR;
}

Int32 BatteryStatsImpl::WriteHistoryTag(
    /* [in] */ HistoryTag* tag)
{
    AutoPtr<IInteger32> idxObj;
    HashMap<AutoPtr<HistoryTag>, AutoPtr<IInteger32> >::Iterator it = mHistoryTagPool.Find(tag);
    if (it != mHistoryTagPool.End()) {
        idxObj = it->mSecond;
    }
    Int32 idx;
    if (idxObj != NULL) {
        idxObj->GetValue(&idx);
    }
    else {
        idx = mNextHistoryTagIdx;
        AutoPtr<HistoryTag> key = new HistoryTag();
        key->SetTo(tag);
        tag->mPoolIdx = idx;
        AutoPtr<IInteger32> integer;
        CInteger32::New(idx, (IInteger32**)&integer);
        mHistoryTagPool[key] = integer;
        mNextHistoryTagIdx++;
        mNumHistoryTagChars += key->mString.GetLength() + 1;
    }
    return idx;
}

void BatteryStatsImpl::ReadHistoryTag(
    /* [in] */ Int32 index,
    /* [in] */ HistoryTag* tag)
{
    tag->mString = (*mReadHistoryStrings)[index];
    tag->mUid = (*mReadHistoryUids)[index];
    tag->mPoolIdx = index;
}

void BatteryStatsImpl::WriteHistoryDelta(
    /* [in] */ IParcel* dest,
    /* [in] */ HistoryItem* cur,
    /* [in] */ HistoryItem* last)
{
    if (last == NULL || cur->mCmd != HistoryItem::CMD_UPDATE) {
        dest->WriteInt32(DELTA_TIME_ABS);
        cur->WriteToParcel(dest);
        return;
    }

    Int64 deltaTime = cur->mTime - last->mTime;
    Int32 lastBatteryLevelInt = BuildBatteryLevelInt(last);
    Int32 lastStateInt = BuildStateInt(last);

    Int32 deltaTimeToken;
    if (deltaTime < 0 || deltaTime > Elastos::Core::Math::INT32_MAX_VALUE) {
        deltaTimeToken = DELTA_TIME_LONG;
    }
    else if (deltaTime >= DELTA_TIME_ABS) {
        deltaTimeToken = DELTA_TIME_INT;
    }
    else {
        deltaTimeToken = (Int32)deltaTime;
    }
    Int32 firstToken = deltaTimeToken | (cur->mStates & DELTA_STATE_MASK);
    Int32 batteryLevelInt = BuildBatteryLevelInt(cur);
    Boolean batteryLevelIntChanged = batteryLevelInt != lastBatteryLevelInt;
    if (batteryLevelIntChanged) {
        firstToken |= DELTA_BATTERY_LEVEL_FLAG;
    }
    Int32 stateInt = BuildStateInt(cur);
    Boolean stateIntChanged = stateInt != lastStateInt;
    if (stateIntChanged) {
        firstToken |= DELTA_STATE_FLAG;
    }
    Boolean state2IntChanged = cur->mStates2 != last->mStates2;
    if (state2IntChanged) {
        firstToken |= DELTA_STATE2_FLAG;
    }
    if (cur->mWakelockTag != NULL || cur->mWakeReasonTag != NULL) {
        firstToken |= DELTA_WAKELOCK_FLAG;
    }
    if (cur->mEventCode != HistoryItem::EVENT_NONE) {
        firstToken |= DELTA_EVENT_FLAG;
    }
    dest->WriteInt32(firstToken);
    if (DEBUG)
        Slogger::I(TAG, "WRITE DELTA: firstToken=0x%s deltaTime=%d",
                StringUtils::ToHexString(firstToken).string(), deltaTime);

    if (deltaTimeToken >= DELTA_TIME_INT) {
        if (deltaTimeToken == DELTA_TIME_INT) {
            if (DEBUG) Slogger::I(TAG, "WRITE DELTA: int deltaTime=%d", (Int32)deltaTime);
            dest->WriteInt32((Int32)deltaTime);
        }
        else {
            if (DEBUG) Slogger::I(TAG, "WRITE DELTA: long deltaTime=%d", deltaTime);
            dest->WriteInt64(deltaTime);
        }
    }
    if (batteryLevelIntChanged) {
        dest->WriteInt32(batteryLevelInt);
        if (DEBUG)
            Slogger::I(TAG, "WRITE DELTA: batteryToken=0x%s batteryLevel=%d batteryTemp=%d batteryVolt=%d",
                    StringUtils::ToHexString(batteryLevelInt).string(), cur->mBatteryLevel, cur->mBatteryTemperature,
                    (Int32)cur->mBatteryVoltage);
    }
    if (stateIntChanged) {
        dest->WriteInt32(stateInt);
        if (DEBUG)
            Slogger::I(TAG, "WRITE DELTA: stateToken=0x%s batteryStatus=%d batteryHealth=%d batteryPlugType=%d states=0x%s",
                    StringUtils::ToHexString(stateInt).string(), cur->mBatteryStatus, cur->mBatteryHealth,
                    (Int32)cur->mBatteryPlugType, StringUtils::ToHexString(cur->mStates).string());
    }
    if (state2IntChanged) {
        dest->WriteInt32(cur->mStates2);
        if (DEBUG)
            Slogger::I(TAG, "WRITE DELTA: states2=0x%s", StringUtils::ToHexString(cur->mStates2).string());
    }
    if (cur->mWakelockTag != NULL || cur->mWakeReasonTag != NULL) {
        Int32 wakeLockIndex;
        Int32 wakeReasonIndex;
        if (cur->mWakelockTag != NULL) {
            wakeLockIndex = WriteHistoryTag(cur->mWakelockTag);
            if (DEBUG)
                Slogger::I(TAG, "WRITE DELTA: wakelockTag=#%d %d:%s",
                        cur->mWakelockTag->mPoolIdx, cur->mWakelockTag->mUid, cur->mWakelockTag->mString.string());
        }
        else {
            wakeLockIndex = 0xffff;
        }
        if (cur->mWakeReasonTag != NULL) {
            wakeReasonIndex = WriteHistoryTag(cur->mWakeReasonTag);
            if (DEBUG)
                Slogger::I(TAG, "WRITE DELTA: wakeReasonTag=#%d %d:%s",
                        cur->mWakeReasonTag->mPoolIdx, cur->mWakeReasonTag->mUid, cur->mWakeReasonTag->mString.string());
        }
        else {
            wakeReasonIndex = 0xffff;
        }
        dest->WriteInt32((wakeReasonIndex << 16) | wakeLockIndex);
    }
    if (cur->mEventCode != HistoryItem::EVENT_NONE) {
        Int32 index = WriteHistoryTag(cur->mEventTag);
        Int32 codeAndIndex = (cur->mEventCode & 0xffff) | (index << 16);
        dest->WriteInt32(codeAndIndex);
        if (DEBUG)
            Slogger::I(TAG, "WRITE DELTA: event=%d tag=#%d %d:%s",
                    cur->mEventCode, cur->mEventTag->mPoolIdx, cur->mEventTag->mUid, cur->mEventTag->mString.string());
    }
}

Int32 BatteryStatsImpl::BuildBatteryLevelInt(
    /* [in] */ HistoryItem* h)
{
    return ((((Int32)h->mBatteryLevel) << 25) & 0xfe000000)
            | ((((Int32)h->mBatteryTemperature) << 14) & 0x01ffc000)
            | (((Int32)h->mBatteryVoltage) & 0x00003fff);
}

Int32 BatteryStatsImpl::BuildStateInt(
    /* [in] */ HistoryItem* h)
{
    Int32 plugType = 0;
    if ((h->mBatteryPlugType & IBatteryManager::BATTERY_PLUGGED_AC) != 0) {
        plugType = 1;
    }
    else if ((h->mBatteryPlugType & IBatteryManager::BATTERY_PLUGGED_USB) != 0) {
        plugType = 2;
    }
    else if ((h->mBatteryPlugType & IBatteryManager::BATTERY_PLUGGED_WIRELESS) != 0) {
        plugType = 3;
    }
    return ((h->mBatteryStatus & STATE_BATTERY_STATUS_MASK) << STATE_BATTERY_STATUS_SHIFT)
            | ((h->mBatteryHealth & STATE_BATTERY_HEALTH_MASK) << STATE_BATTERY_HEALTH_SHIFT)
            | ((plugType & STATE_BATTERY_PLUG_MASK) << STATE_BATTERY_PLUG_SHIFT)
            | (h->mStates & (~DELTA_STATE_MASK));
}

void BatteryStatsImpl::ReadHistoryDelta(
    /* [in] */ IParcel* src,
    /* [in] */ HistoryItem* cur)
{
    Int32 firstToken;
    src->ReadInt32(&firstToken);
    Int32 deltaTimeToken = firstToken & DELTA_TIME_MASK;
    cur->mCmd = HistoryItem::CMD_UPDATE;
    cur->mNumReadInts = 1;
    if (DEBUG)
        Slogger::I(TAG, "READ DELTA: firstToken=0x%s deltaTimeToken=%d",
                StringUtils::ToHexString(firstToken).string(), deltaTimeToken);

    if (deltaTimeToken < DELTA_TIME_ABS) {
        cur->mTime += deltaTimeToken;
    }
    else if (deltaTimeToken == DELTA_TIME_ABS) {
        src->ReadInt64(&cur->mTime);
        cur->mNumReadInts += 2;
        if (DEBUG)
            Slogger::I(TAG, "READ DELTA: ABS time=%d", cur->mTime);
        cur->ReadFromParcel(src);
        return;
    }
    else if (deltaTimeToken == DELTA_TIME_INT) {
        Int32 delta;
        src->ReadInt32(&delta);
        cur->mTime += delta;
        cur->mNumReadInts += 1;
        if (DEBUG) Slogger::I(TAG, "READ DELTA: time delta=%d new time=%d", delta, cur->mTime);
    }
    else {
        Int64 delta;
        src->ReadInt64(&delta);
        if (DEBUG) Slogger::I(TAG, "READ DELTA: time delta=%d new time=%d", delta, cur->mTime);
        cur->mTime += delta;
        cur->mNumReadInts += 2;
    }

    if ((firstToken & DELTA_BATTERY_LEVEL_FLAG) != 0) {
        Int32 batteryLevelInt;
        src->ReadInt32(&batteryLevelInt);
        cur->mBatteryLevel = (Byte)((batteryLevelInt >> 25) & 0x7f);
        cur->mBatteryTemperature = (Char32)((batteryLevelInt << 7) >> 21);
        cur->mBatteryVoltage = (Char32)(batteryLevelInt & 0x3fff);
        cur->mNumReadInts += 1;
        if (DEBUG)
            Slogger::I(TAG, "READ DELTA: batteryToken=0x%s batteryLevel=%d batteryTemp=%d batteryVolt=%d",
                    StringUtils::ToHexString(batteryLevelInt).string(), cur->mBatteryLevel, cur->mBatteryTemperature,
                    (Int32)cur->mBatteryVoltage);
    }

    if ((firstToken & DELTA_STATE_FLAG) != 0) {
        Int32 stateInt;
        src->ReadInt32(&stateInt);
        cur->mStates = (firstToken & DELTA_STATE_MASK) | (stateInt & (~DELTA_STATE_MASK));
        cur->mBatteryStatus = (Byte)((stateInt >> STATE_BATTERY_STATUS_SHIFT)
                & STATE_BATTERY_STATUS_MASK);
        cur->mBatteryHealth = (Byte)((stateInt >> STATE_BATTERY_HEALTH_SHIFT)
                & STATE_BATTERY_HEALTH_MASK);
        cur->mBatteryPlugType = (Byte)((stateInt >> STATE_BATTERY_PLUG_SHIFT)
                & STATE_BATTERY_PLUG_MASK);
        switch (cur->mBatteryPlugType) {
            case 1:
                cur->mBatteryPlugType = IBatteryManager::BATTERY_PLUGGED_AC;
                break;
            case 2:
                cur->mBatteryPlugType = IBatteryManager::BATTERY_PLUGGED_USB;
                break;
            case 3:
                cur->mBatteryPlugType = IBatteryManager::BATTERY_PLUGGED_WIRELESS;
                break;
        }
        cur->mNumReadInts += 1;
        if (DEBUG)
            Slogger::I(TAG, "READ DELTA: stateToken=0x%s batteryStatus=%d batteryHealth=%d batteryPlugType=%d states=0x%s",
                    StringUtils::ToHexString(stateInt).string(), cur->mBatteryStatus, cur->mBatteryHealth,
                    cur->mBatteryPlugType, StringUtils::ToHexString(cur->mStates).string());
    }
    else {
        cur->mStates = (firstToken & DELTA_STATE_MASK) | (cur->mStates & (~DELTA_STATE_MASK));
    }

    if ((firstToken & DELTA_STATE2_FLAG) != 0) {
        src->ReadInt32(&cur->mStates2);
        if (DEBUG) Slogger::I(TAG, "READ DELTA: states2=0x%s", StringUtils::ToHexString(cur->mStates2).string());
    }

    if ((firstToken & DELTA_WAKELOCK_FLAG) != 0) {
        Int32 indexes;
        src->ReadInt32(&indexes);
        Int32 wakeLockIndex = indexes & 0xffff;
        Int32 wakeReasonIndex = (indexes >> 16) & 0xffff;
        if (wakeLockIndex != 0xffff) {
            cur->mWakelockTag = cur->mLocalWakelockTag;
            ReadHistoryTag(wakeLockIndex, cur->mWakelockTag);
            if (DEBUG)
                Slogger::I(TAG, "READ DELTA: wakelockTag=#%d %d:%s",
                        cur->mWakelockTag->mPoolIdx, cur->mWakelockTag->mUid, cur->mWakelockTag->mString.string());
        }
        else {
            cur->mWakelockTag = NULL;
        }
        if (wakeReasonIndex != 0xffff) {
            cur->mWakeReasonTag = cur->mLocalWakeReasonTag;
            ReadHistoryTag(wakeReasonIndex, cur->mWakeReasonTag);
            if (DEBUG)
                Slogger::I(TAG, "READ DELTA: wakeReasonTag=#%d %d:%s",
                        cur->mWakeReasonTag->mPoolIdx, cur->mWakeReasonTag->mUid, cur->mWakeReasonTag->mString.string());
        }
        else {
            cur->mWakeReasonTag = NULL;
        }
        cur->mNumReadInts += 1;
    }
    else {
        cur->mWakelockTag = NULL;
        cur->mWakeReasonTag = NULL;
    }

    if ((firstToken & DELTA_EVENT_FLAG) != 0) {
        cur->mEventTag = cur->mLocalEventTag;
        Int32 codeAndIndex;
        src->ReadInt32(&codeAndIndex);
        cur->mEventCode = (codeAndIndex & 0xffff);
        Int32 index = ((codeAndIndex >> 16) & 0xffff);
        ReadHistoryTag(index, cur->mEventTag);
        cur->mNumReadInts += 1;
        if (DEBUG)
            Slogger::I(TAG, "READ DELTA: event=%d tag=#%d %d:%s",
                cur->mEventCode, cur->mEventTag->mPoolIdx, cur->mEventTag->mUid, cur->mEventTag->mString.string());
    }
    else {
        cur->mEventCode = HistoryItem::EVENT_NONE;
    }
}

ECode BatteryStatsImpl::CommitCurrentHistoryBatchLocked()
{
    mHistoryLastWritten->mCmd = HistoryItem::CMD_NULL;
    return NOERROR;
}

void BatteryStatsImpl::AddHistoryBufferLocked(
    /* [in] */ Int64 elapsedRealtimeMs,
    /* [in] */ Int64 uptimeMs,
    /* [in] */ HistoryItem* cur)
{
    if (!mHaveBatteryLevel || !mRecordingHistory) {
        return;
    }

    Int64 timeDiff = (mHistoryBaseTime + elapsedRealtimeMs) - mHistoryLastWritten->mTime;
    Int32 diffStates = mHistoryLastWritten->mStates ^ cur->mStates;
    Int32 diffStates2 = mHistoryLastWritten->mStates2 ^ cur->mStates2;
    Int32 lastDiffStates = mHistoryLastWritten->mStates ^ mHistoryLastLastWritten->mStates;
    Int32 lastDiffStates2 = mHistoryLastWritten->mStates2 ^ mHistoryLastLastWritten->mStates2;
    if (DEBUG)
        Slogger::I(TAG, "ADD: tdelta=%d diff=%s lastDiff=%s diff2=%s lastDiff2=%s",
                timeDiff, StringUtils::ToHexString(diffStates).string(), StringUtils::ToHexString(lastDiffStates).string(),
                StringUtils::ToHexString(diffStates2).string(), StringUtils::ToHexString(lastDiffStates2).string());
    if (mHistoryBufferLastPos >= 0 && mHistoryLastWritten->mCmd == HistoryItem::CMD_UPDATE
            && timeDiff < 1000 && (diffStates & lastDiffStates) == 0
            && (diffStates2 & lastDiffStates2) == 0
            && (mHistoryLastWritten->mWakelockTag == NULL || cur->mWakelockTag == NULL)
            && (mHistoryLastWritten->mWakeReasonTag == NULL || cur->mWakeReasonTag == NULL)
            && (mHistoryLastWritten->mEventCode == HistoryItem::EVENT_NONE
                    || cur->mEventCode == HistoryItem::EVENT_NONE)
            && mHistoryLastWritten->mBatteryLevel == cur->mBatteryLevel
            && mHistoryLastWritten->mBatteryStatus == cur->mBatteryStatus
            && mHistoryLastWritten->mBatteryHealth == cur->mBatteryHealth
            && mHistoryLastWritten->mBatteryPlugType == cur->mBatteryPlugType
            && mHistoryLastWritten->mBatteryTemperature == cur->mBatteryTemperature
            && mHistoryLastWritten->mBatteryVoltage == cur->mBatteryVoltage) {
        // We can merge this new change in with the last one.  Merging is
        // allowed as long as only the states have changed, and within those states
        // as long as no bit has changed both between now and the last entry, as
        // well as the last entry and the one before it (so we capture any toggles).
        if (DEBUG) Slogger::I(TAG, "ADD: rewinding back to %d", mHistoryBufferLastPos);
        // mHistoryBuffer->SetDataSize(mHistoryBufferLastPos);
        mHistoryBuffer->SetDataPosition(mHistoryBufferLastPos);
        mHistoryBufferLastPos = -1;
        elapsedRealtimeMs = mHistoryLastWritten->mTime - mHistoryBaseTime;
        // If the last written history had a wakelock tag, we need to retain it.
        // Note that the condition above made sure that we aren't in a case where
        // both it and the current history item have a wakelock tag.
        if (mHistoryLastWritten->mWakelockTag != NULL) {
            cur->mWakelockTag = cur->mLocalWakelockTag;
            cur->mWakelockTag->SetTo(mHistoryLastWritten->mWakelockTag);
        }
        // If the last written history had a wake reason tag, we need to retain it.
        // Note that the condition above made sure that we aren't in a case where
        // both it and the current history item have a wakelock tag.
        if (mHistoryLastWritten->mWakeReasonTag != NULL) {
            cur->mWakeReasonTag = cur->mLocalWakeReasonTag;
            cur->mWakeReasonTag->SetTo(mHistoryLastWritten->mWakeReasonTag);
        }
        // If the last written history had an event, we need to retain it.
        // Note that the condition above made sure that we aren't in a case where
        // both it and the current history item have an event.
        if (mHistoryLastWritten->mEventCode != HistoryItem::EVENT_NONE) {
            cur->mEventCode = mHistoryLastWritten->mEventCode;
            cur->mEventTag = cur->mLocalEventTag;
            cur->mEventTag->SetTo(mHistoryLastWritten->mEventTag);
        }
        mHistoryLastWritten->SetTo(mHistoryLastLastWritten);
    }

    Int32 dataSize;
    mHistoryBuffer->GetElementSize(&dataSize);
    if (dataSize >= MAX_HISTORY_BUFFER) {
        if (!mHistoryOverflow) {
            mHistoryOverflow = TRUE;
            AddHistoryBufferLocked(elapsedRealtimeMs, uptimeMs, HistoryItem::CMD_UPDATE, cur);
            AddHistoryBufferLocked(elapsedRealtimeMs, uptimeMs, HistoryItem::CMD_OVERFLOW, cur);
            return;
        }

        // Once we've reached the maximum number of items, we only
        // record changes to the battery level and the most interesting states.
        // Once we've reached the maximum maximum number of items, we only
        // record changes to the battery level.
        if (mHistoryLastWritten->mBatteryLevel == cur->mBatteryLevel &&
                (dataSize >= MAX_MAX_HISTORY_BUFFER
                        || ((mHistoryLastWritten->mStates ^ cur->mStates)
                                & HistoryItem::MOST_INTERESTING_STATES) == 0
                        || ((mHistoryLastWritten->mStates2 ^ cur->mStates2)
                                & HistoryItem::MOST_INTERESTING_STATES2) == 0)) {
            return;
        }

        AddHistoryBufferLocked(elapsedRealtimeMs, uptimeMs, HistoryItem::CMD_UPDATE, cur);
        return;
    }

    if (dataSize == 0) {
        // The history is currently empty; we need it to start with a time stamp.
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        sys->GetCurrentTimeMillis(&cur->mCurrentTime);
        mLastRecordedClockTime = cur->mCurrentTime;
        mLastRecordedClockRealtime = elapsedRealtimeMs;
        AddHistoryBufferLocked(elapsedRealtimeMs, uptimeMs, HistoryItem::CMD_RESET, cur);
    }
    AddHistoryBufferLocked(elapsedRealtimeMs, uptimeMs, HistoryItem::CMD_UPDATE, cur);
}

ECode BatteryStatsImpl::AddHistoryBufferLocked(
    /* [in] */ Int64 elapsedRealtimeMs,
    /* [in] */ Int64 uptimeMs,
    /* [in] */ Byte cmd,
    /* [in] */ HistoryItem* cur)
{
    if (mIteratingHistory) {
        Slogger::E(TAG, "Can't do this while iterating history!");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mHistoryBuffer->GetDataPosition(&mHistoryBufferLastPos);
    mHistoryLastLastWritten->SetTo(mHistoryLastWritten);
    mHistoryLastWritten->SetTo(mHistoryBaseTime + elapsedRealtimeMs, cmd, cur);
    WriteHistoryDelta(mHistoryBuffer, mHistoryLastWritten, mHistoryLastLastWritten);
    mLastHistoryElapsedRealtime = elapsedRealtimeMs;
    cur->mWakelockTag = NULL;
    cur->mWakeReasonTag = NULL;
    cur->mEventCode = HistoryItem::EVENT_NONE;
    cur->mEventTag = NULL;
    if (DEBUG_HISTORY) {
        Int32 size;
        mHistoryBuffer->GetElementSize(&size);
        Slogger::I(TAG, "Writing history buffer: was %d now %d size is now %d",
                mHistoryBufferLastPos, mHistoryBufferLastPos, size);
    }
    return NOERROR;
}

void BatteryStatsImpl::AddHistoryRecordLocked(
    /* [in] */ Int64 elapsedRealtimeMs,
    /* [in] */ Int64 uptimeMs)
{
    if (mTrackRunningHistoryElapsedRealtime != 0) {
        Int64 diffElapsed = elapsedRealtimeMs - mTrackRunningHistoryElapsedRealtime;
        Int64 diffUptime = uptimeMs - mTrackRunningHistoryUptime;
        if (diffUptime < (diffElapsed-20)) {
            Int64 wakeElapsedTime = elapsedRealtimeMs - (diffElapsed - diffUptime);
            mHistoryAddTmp->SetTo(mHistoryLastWritten);
            mHistoryAddTmp->mWakelockTag = NULL;
            mHistoryAddTmp->mWakeReasonTag = NULL;
            mHistoryAddTmp->mEventCode = HistoryItem::EVENT_NONE;
            mHistoryAddTmp->mStates &= ~HistoryItem::STATE_CPU_RUNNING_FLAG;
            AddHistoryRecordInnerLocked(wakeElapsedTime, uptimeMs, mHistoryAddTmp);
        }
    }
    mHistoryCur->mStates |= HistoryItem::STATE_CPU_RUNNING_FLAG;
    mTrackRunningHistoryElapsedRealtime = elapsedRealtimeMs;
    mTrackRunningHistoryUptime = uptimeMs;
    AddHistoryRecordInnerLocked(elapsedRealtimeMs, uptimeMs, mHistoryCur);
}

void BatteryStatsImpl::AddHistoryRecordInnerLocked(
    /* [in] */ Int64 elapsedRealtimeMs,
    /* [in] */ Int64 uptimeMs,
    /* [in] */ HistoryItem* cur)
{
    AddHistoryBufferLocked(elapsedRealtimeMs, uptimeMs, cur);

    if (!USE_OLD_HISTORY) {
        return;
    }

    if (!mHaveBatteryLevel || !mRecordingHistory) {
        return;
    }

    // If the current time is basically the same as the last time,
    // and no states have since the last recorded entry changed and
    // are now resetting back to their original value, then just collapse
    // into one record.
    if (mHistoryEnd != NULL && mHistoryEnd->mCmd == HistoryItem::CMD_UPDATE
            && (mHistoryBaseTime + elapsedRealtimeMs) < (mHistoryEnd->mTime + 1000)
            && ((mHistoryEnd->mStates ^ cur->mStates) & mChangedStates) == 0
            && ((mHistoryEnd->mStates2 ^ cur->mStates2) & mChangedStates2) == 0) {
        // If the current is the same as the one before, then we no
        // longer need the entry.
        if (mHistoryLastEnd != NULL && mHistoryLastEnd->mCmd == HistoryItem::CMD_UPDATE
                && (mHistoryBaseTime + elapsedRealtimeMs) < (mHistoryEnd->mTime + 500)
                && mHistoryLastEnd->SameNonEvent(cur)) {
            mHistoryLastEnd->mNext = NULL;
            mHistoryEnd->mNext = mHistoryCache;
            mHistoryCache = mHistoryEnd;
            mHistoryEnd = mHistoryLastEnd;
            mHistoryLastEnd = NULL;
        }
        else {
            mChangedStates |= mHistoryEnd->mStates ^ cur->mStates;
            mChangedStates2 |= mHistoryEnd->mStates ^ cur->mStates2;
            mHistoryEnd->SetTo(mHistoryEnd->mTime, HistoryItem::CMD_UPDATE, cur);
        }
        return;
    }

    mChangedStates = 0;
    mChangedStates2 = 0;

    if (mNumHistoryItems == MAX_HISTORY_ITEMS
            || mNumHistoryItems == MAX_MAX_HISTORY_ITEMS) {
        AddHistoryRecordLocked(elapsedRealtimeMs, HistoryItem::CMD_OVERFLOW);
    }

    if (mNumHistoryItems >= MAX_HISTORY_ITEMS) {
        // Once we've reached the maximum number of items, we only
        // record changes to the battery level and the most interesting states.
        // Once we've reached the maximum maximum number of items, we only
        // record changes to the battery level.
        if (mHistoryEnd != NULL && mHistoryEnd->mBatteryLevel
                == cur->mBatteryLevel &&
                (mNumHistoryItems >= MAX_MAX_HISTORY_ITEMS
                        || ((mHistoryEnd->mStates ^ cur->mStates)
                                & HistoryItem::MOST_INTERESTING_STATES) == 0)) {
            return;
        }
    }

    AddHistoryRecordLocked(elapsedRealtimeMs, HistoryItem::CMD_UPDATE);
}

void BatteryStatsImpl::AddHistoryEventLocked(
    /* [in] */ Int64 elapsedRealtimeMs,
    /* [in] */ Int64 uptimeMs,
    /* [in] */ Int32 code,
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    mHistoryCur->mEventCode = code;
    mHistoryCur->mEventTag = mHistoryCur->mLocalEventTag;
    mHistoryCur->mEventTag->mString = name;
    mHistoryCur->mEventTag->mUid = uid;
    AddHistoryRecordLocked(elapsedRealtimeMs, uptimeMs);
}

void BatteryStatsImpl::AddHistoryRecordLocked(
    /* [in] */ Int64 elapsedRealtimeMs,
    /* [in] */ Int64 uptimeMs,
    /* [in] */ Byte cmd,
    /* [in] */ HistoryItem* cur)
{
    AutoPtr<HistoryItem> rec = mHistoryCache;
    if (rec != NULL) {
        mHistoryCache = rec->mNext;
    }
    else {
        rec = new HistoryItem();
    }
    rec->SetTo(mHistoryBaseTime + elapsedRealtimeMs, cmd, cur);

    AddHistoryRecordLocked(rec);
}

void BatteryStatsImpl::AddHistoryRecordLocked(
    /* [in] */ HistoryItem* rec)
{
    mNumHistoryItems++;
    rec->mNext = NULL;
    mHistoryLastEnd = mHistoryEnd;
    if (mHistoryEnd != NULL) {
        mHistoryEnd->mNext = rec;
        mHistoryEnd = rec;
    }
    else {
        mHistory = mHistoryEnd = rec;
    }
}

void BatteryStatsImpl::ClearHistoryLocked()
{
    if (DEBUG_HISTORY) Logger::I(TAG, "********** CLEARING HISTORY!");
    if (USE_OLD_HISTORY) {
        if (mHistory != NULL) {
            mHistoryEnd->mNext = mHistoryCache;
            mHistoryCache = mHistory;
            mHistory = mHistoryLastEnd = mHistoryEnd = NULL;
        }
        mNumHistoryItems = 0;
    }

    mHistoryBaseTime = 0;
    mLastHistoryElapsedRealtime = 0;
    mTrackRunningHistoryElapsedRealtime = 0;
    mTrackRunningHistoryUptime = 0;

    // mHistoryBuffer->SetDataSize(0);
    mHistoryBuffer->SetDataPosition(0);
    // mHistoryBuffer->SetDataCapacity(MAX_HISTORY_BUFFER / 2);
    mHistoryLastLastWritten->Clear();
    mHistoryLastWritten->Clear();
    mHistoryTagPool.Clear();
    mNextHistoryTagIdx = 0;
    mNumHistoryTagChars = 0;
    mHistoryBufferLastPos = -1;
    mHistoryOverflow = FALSE;
    mLastRecordedClockTime = 0;
    mLastRecordedClockRealtime = 0;
}

void BatteryStatsImpl::UpdateTimeBasesLocked(
    /* [in] */ Boolean unplugged,
    /* [in] */ Boolean screenOff,
    /* [in] */ Int64 uptime,
    /* [in] */ Int64 realtime)
{
    if (mOnBatteryTimeBase->SetRunning(unplugged, uptime, realtime)) {
        if (unplugged) {
            // Track bt headset ping count
            mBluetoothPingStart = GetCurrentBluetoothPingCount();
            mBluetoothPingCount = 0;
        }
        else {
            // Track bt headset ping count
            GetBluetoothPingCount(&mBluetoothPingCount);
            mBluetoothPingStart = -1;
        }
    }

    Boolean unpluggedScreenOff = unplugged && screenOff;
    if (unpluggedScreenOff != mOnBatteryScreenOffTimeBase->IsRunning()) {
        UpdateKernelWakelocksLocked();
        RequestWakelockCpuUpdate();
        if (!unpluggedScreenOff) {
            // We are switching to no longer tracking wake locks, but we want
            // the next CPU update we receive to take them in to account.
            mDistributeWakelockCpu = TRUE;
        }
        mOnBatteryScreenOffTimeBase->SetRunning(unpluggedScreenOff, uptime, realtime);
    }
}

ECode BatteryStatsImpl::AddIsolatedUidLocked(
    /* [in] */ Int32 isolatedUid,
    /* [in] */ Int32 appUid)
{
    mIsolatedUids[isolatedUid] = appUid;
    return NOERROR;
}

ECode BatteryStatsImpl::RemoveIsolatedUidLocked(
    /* [in] */ Int32 isolatedUid,
    /* [in] */ Int32 appUid)
{
    Int32 curUid = -1;
    HashMap<Int32, Int32>::Iterator it = mIsolatedUids.Find(isolatedUid);
    if (it != mIsolatedUids.End()) {
        curUid = it->mSecond;
    }
    if (curUid == appUid) {
        mIsolatedUids.Erase(it);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::MapUid(
    /* [in] */ Int32 uid,
    /* [out] */ Int32* newUid)
{
    VALIDATE_NOT_NULL(newUid)
    Int32 isolated = -1;
    HashMap<Int32, Int32>::Iterator it = mIsolatedUids.Find(uid);
    if (it != mIsolatedUids.End()) {
        isolated = it->mSecond;
    }
    *newUid = isolated > 0 ? isolated : uid;
    return NOERROR;
}

ECode BatteryStatsImpl::NoteEventLocked(
    /* [in] */ Int32 code,
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    if (!mActiveEvents->UpdateState(code, name, uid, 0)) {
        return NOERROR;
    }
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    AddHistoryEventLocked(elapsedRealtime, uptime, code, name, uid);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteCurrentTimeChangedLocked()
{
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 currentTime;
    sys->GetCurrentTimeMillis(&currentTime);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (IsStartClockTimeValid()) {
        // Has the time changed sufficiently that it is really worth recording?
        if (mLastRecordedClockTime != 0) {
            Int64 expectedClockTime = mLastRecordedClockTime
                    + (elapsedRealtime - mLastRecordedClockRealtime);
            if (currentTime >= (expectedClockTime - 500)
                    && currentTime <= (expectedClockTime + 500)) {
                // Not sufficiently changed, skip!
                return NOERROR;
            }
        }
    }
    RecordCurrentTimeChangeLocked(currentTime, elapsedRealtime, uptime);
    if (IsStartClockTimeValid()) {
        mStartClockTime = currentTime;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteProcessStartLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Boolean isOnBattery;
    if (IsOnBattery(&isOnBattery), isOnBattery) {
        AutoPtr<Uid> u = GetUidStatsLocked(uid);
        u->GetProcessStatsLocked(name)->IncStartsLocked();
    }
    if (!mActiveEvents->UpdateState(HistoryItem::EVENT_PROC_START, name, uid, 0)) {
        return NOERROR;
    }
    if (!mRecordAllHistory) {
        return NOERROR;
    }
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    AddHistoryEventLocked(elapsedRealtime, uptime, HistoryItem::EVENT_PROC_START, name, uid);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteProcessStateLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 state)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    GetUidStatsLocked(uid)->UpdateProcessStateLocked(name, state, elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteProcessFinishLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    if (!mActiveEvents->UpdateState(HistoryItem::EVENT_PROC_FINISH, name, uid, 0)) {
        return NOERROR;
    }
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    GetUidStatsLocked(uid)->UpdateProcessStateLocked(name, Uid::PROCESS_STATE_NONE,
            elapsedRealtime);
    if (!mRecordAllHistory) {
        return NOERROR;
    }
    AddHistoryEventLocked(elapsedRealtime, uptime, HistoryItem::EVENT_PROC_FINISH, name, uid);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteSyncStartLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    GetUidStatsLocked(uid)->NoteStartSyncLocked(name, elapsedRealtime);
    if (!mActiveEvents->UpdateState(HistoryItem::EVENT_SYNC_START, name, uid, 0)) {
        return NOERROR;
    }
    AddHistoryEventLocked(elapsedRealtime, uptime, HistoryItem::EVENT_SYNC_START, name, uid);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteSyncFinishLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    GetUidStatsLocked(uid)->NoteStopSyncLocked(name, elapsedRealtime);
    if (!mActiveEvents->UpdateState(HistoryItem::EVENT_SYNC_FINISH, name, uid, 0)) {
        return NOERROR;
    }
    AddHistoryEventLocked(elapsedRealtime, uptime, HistoryItem::EVENT_SYNC_FINISH, name, uid);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteJobStartLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    GetUidStatsLocked(uid)->NoteStartJobLocked(name, elapsedRealtime);
    if (!mActiveEvents->UpdateState(HistoryItem::EVENT_JOB_START, name, uid, 0)) {
        return NOERROR;
    }
    AddHistoryEventLocked(elapsedRealtime, uptime, HistoryItem::EVENT_JOB_START, name, uid);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteJobFinishLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    GetUidStatsLocked(uid)->NoteStopJobLocked(name, elapsedRealtime);
    if (!mActiveEvents->UpdateState(HistoryItem::EVENT_JOB_FINISH, name, uid, 0)) {
        return NOERROR;
    }
    AddHistoryEventLocked(elapsedRealtime, uptime, HistoryItem::EVENT_JOB_FINISH, name, uid);
    return NOERROR;
}

void BatteryStatsImpl::RequestWakelockCpuUpdate()
{
    Boolean hasMessages;
    if (mHandler->HasMessages(MSG_UPDATE_WAKELOCKS, &hasMessages), !hasMessages) {
        AutoPtr<IMessage> m;
        mHandler->ObtainMessage(MSG_UPDATE_WAKELOCKS, (IMessage**)&m);
        Boolean result;
        mHandler->SendMessageDelayed(m, DELAY_UPDATE_WAKELOCKS, &result);
    }
}

ECode BatteryStatsImpl::SetRecordAllHistoryLocked(
    /* [in] */ Boolean enabled)
{
    mRecordAllHistory = enabled;
    if (!enabled) {
        // Clear out any existing state.
        mActiveEvents->RemoveEvents(HistoryItem::EVENT_WAKE_LOCK);
        // Record the currently running processes as stopping, now that we are no
        // longer tracking them.
        AutoPtr<IHashMap> active = mActiveEvents->GetStateForEvent(HistoryItem::EVENT_PROC);
        if (active != NULL) {
            Int64 mSecRealtime = SystemClock::GetElapsedRealtime();
            Int64 mSecUptime = SystemClock::GetUptimeMillis();
            AutoPtr<ISet> set;
            active->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> item;
                it->GetNext((IInterface**)&item);
                AutoPtr<IMapEntry> ent = IMapEntry::Probe(item);
                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                AutoPtr<ISparseInt32Array> uids = ISparseInt32Array::Probe(value);
                Int32 size;
                uids->GetSize(&size);
                for (Int32 j = 0; j < size; j++) {
                    AutoPtr<IInterface> ki;
                    ent->GetKey((IInterface**)&ki);
                    String key;
                    ICharSequence::Probe(ki)->ToString(&key);
                    Int32 uidKey;
                    uids->KeyAt(j, &uidKey);
                    AddHistoryEventLocked(mSecRealtime, mSecUptime,
                            HistoryItem::EVENT_PROC_FINISH, key, uidKey);
                }
            }
        }
    }
    else {
        // Record the currently running processes as starting, now that we are tracking them.
        AutoPtr<IHashMap> active = mActiveEvents->GetStateForEvent(HistoryItem::EVENT_PROC);
        if (active != NULL) {
            Int64 mSecRealtime = SystemClock::GetElapsedRealtime();
            Int64 mSecUptime = SystemClock::GetUptimeMillis();
            AutoPtr<ISet> set;
            active->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> it;
            set->GetIterator((IIterator**)&it);
            Boolean hasNext;
            while (it->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> item;
                it->GetNext((IInterface**)&item);
                AutoPtr<IMapEntry> ent = IMapEntry::Probe(item);
                AutoPtr<IInterface> value;
                ent->GetValue((IInterface**)&value);
                AutoPtr<ISparseInt32Array> uids = ISparseInt32Array::Probe(value);
                Int32 size;
                uids->GetSize(&size);
                for (Int32 j = 0; j < size; j++) {
                    AutoPtr<IInterface> ki;
                    ent->GetKey((IInterface**)&ki);
                    String key;
                    ICharSequence::Probe(ki)->ToString(&key);
                    Int32 uidKey;
                    uids->KeyAt(j, &uidKey);
                    AddHistoryEventLocked(mSecRealtime, mSecUptime,
                            HistoryItem::EVENT_PROC_START, key, uidKey);
                }
            }
        }
    }
    return NOERROR;
}

ECode BatteryStatsImpl::SetNoAutoReset(
    /* [in] */ Boolean enabled)
{
    mNoAutoReset = enabled;
    return NOERROR;
}

ECode BatteryStatsImpl::NoteStartWakeLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ const String& _historyName,
    /* [in] */ Int32 type,
    /* [in] */ Boolean unimportantForLogging,
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 uptime)
{
    String historyName = _historyName;
    MapUid(uid, &uid);
    if (type == WAKE_TYPE_PARTIAL) {
        // Only care about partial wake locks, since full wake locks
        // will be canceled when the user puts the screen to sleep.
        AggregateLastWakeupUptimeLocked(uptime);
        if (historyName.IsNull()) {
            historyName = name;
        }
        if (mRecordAllHistory) {
            if (mActiveEvents->UpdateState(HistoryItem::EVENT_WAKE_LOCK_START, historyName,
                    uid, 0)) {
                AddHistoryEventLocked(elapsedRealtime, uptime,
                        HistoryItem::EVENT_WAKE_LOCK_START, historyName, uid);
            }
        }
        if (mWakeLockNesting == 0) {
            mHistoryCur->mStates |= HistoryItem::STATE_WAKE_LOCK_FLAG;
            if (DEBUG_HISTORY) {
                Logger::V(TAG, "Start wake lock to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
            }
            mHistoryCur->mWakelockTag = mHistoryCur->mLocalWakelockTag;
            mHistoryCur->mWakelockTag->mString = mInitialAcquireWakeName = historyName;
            mHistoryCur->mWakelockTag->mUid = mInitialAcquireWakeUid = uid;
            mWakeLockImportant = !unimportantForLogging;
            AddHistoryRecordLocked(elapsedRealtime, uptime);
        }
        else if (!mWakeLockImportant && !unimportantForLogging
                && mHistoryLastWritten->mCmd == HistoryItem::CMD_UPDATE) {
            if (mHistoryLastWritten->mWakelockTag != NULL) {
                // We'll try to update the last tag.
                mHistoryLastWritten->mWakelockTag = NULL;
                mHistoryCur->mWakelockTag = mHistoryCur->mLocalWakelockTag;
                mHistoryCur->mWakelockTag->mString = mInitialAcquireWakeName = historyName;
                mHistoryCur->mWakelockTag->mUid = mInitialAcquireWakeUid = uid;
                AddHistoryRecordLocked(elapsedRealtime, uptime);
            }
            mWakeLockImportant = true;
        }
        mWakeLockNesting++;
    }
    if (uid >= 0) {
        //if (uid == 0) {
        //    Slog.wtf(TAG, "Acquiring wake lock from root: " + name);
        //}
        RequestWakelockCpuUpdate();
        GetUidStatsLocked(uid)->NoteStartWakeLocked(pid, name, type, elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteStopWakeLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ const String& _historyName,
    /* [in] */ Int32 type,
    /* [in] */ Int64 elapsedRealtime,
    /* [in] */ Int64 uptime)
{
    String historyName = _historyName;
    MapUid(uid, &uid);
    if (type == WAKE_TYPE_PARTIAL) {
        mWakeLockNesting--;
        if (mRecordAllHistory) {
            if (historyName.IsNull()) {
                historyName = name;
            }
            if (mActiveEvents->UpdateState(HistoryItem::EVENT_WAKE_LOCK_FINISH, historyName,
                    uid, 0)) {
                AddHistoryEventLocked(elapsedRealtime, uptime,
                        HistoryItem::EVENT_WAKE_LOCK_FINISH, historyName, uid);
            }
        }
        if (mWakeLockNesting == 0) {
            mHistoryCur->mStates &= ~HistoryItem::STATE_WAKE_LOCK_FLAG;
            if (DEBUG_HISTORY) {
                Logger::V(TAG, "Stop wake lock to: ", StringUtils::ToHexString(mHistoryCur->mStates).string());
            }
            mInitialAcquireWakeName = String(NULL);
            mInitialAcquireWakeUid = -1;
            AddHistoryRecordLocked(elapsedRealtime, uptime);
        }
    }
    if (uid >= 0) {
        RequestWakelockCpuUpdate();
        GetUidStatsLocked(uid)->NoteStopWakeLocked(pid, name, type, elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteChangeWakelockFromSourceLocked(
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
    /* [in] */ Boolean newUnimportantForLogging)
{
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    // For correct semantics, we start the need worksources first, so that we won't
    // make inappropriate history items as if all wake locks went away and new ones
    // appeared.  This is okay because tracking of wake locks allows nesting.
    Int32 NN;
    newWs->GetSize(&NN);
    for (Int32 i = 0; i < NN; i++) {
        Int32 value;
        newWs->Get(i, &value);
        NoteStartWakeLocked(value, newPid, newName, newHistoryName, newType,
                newUnimportantForLogging, elapsedRealtime, uptime);
    }
    Int32 NO;
    ws->GetSize(&NO);
    for (Int32 i = 0; i < NO; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteStopWakeLocked(value, pid, name, historyName, type, elapsedRealtime, uptime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteStartWakeFromSourceLocked(
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ const String& historyName,
    /* [in] */ Int32 type,
    /* [in] */ Boolean unimportantForLogging)
{
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteStartWakeLocked(value, pid, name, historyName, type, unimportantForLogging,
                elapsedRealtime, uptime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteStopWakeFromSourceLocked(
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ const String& historyName,
    /* [in] */ Int32 type)
{
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteStopWakeLocked(value, pid, name, historyName, type, elapsedRealtime, uptime);
    }
    return NOERROR;
}

void BatteryStatsImpl::AggregateLastWakeupUptimeLocked(
    /* [in] */ Int64 uptimeMs)
{
    if (mLastWakeupReason != NULL) {
        Int64 deltaUptime = uptimeMs - mLastWakeupUptimeMs;
        AutoPtr<SamplingTimer> timer = GetWakeupReasonTimerLocked(mLastWakeupReason);
        timer->AddCurrentReportedCount(1);
        timer->AddCurrentReportedTotalTime(deltaUptime * 1000); // time is in microseconds
        mLastWakeupReason = String(NULL);
    }
}

ECode BatteryStatsImpl::NoteWakeupReasonLocked(
    /* [in] */ const String& reason)
{
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (DEBUG_HISTORY)
        Slogger::V(TAG, "Wakeup reason \"%s\": %s", reason.string(), StringUtils::ToHexString(mHistoryCur->mStates).string());
    AggregateLastWakeupUptimeLocked(uptime);
    mHistoryCur->mWakeReasonTag = mHistoryCur->mLocalWakeReasonTag;
    mHistoryCur->mWakeReasonTag->mString = reason;
    mHistoryCur->mWakeReasonTag->mUid = 0;
    mLastWakeupReason = reason;
    mLastWakeupUptimeMs = uptime;
    AddHistoryRecordLocked(elapsedRealtime, uptime);
    return NOERROR;
}

ECode BatteryStatsImpl::StartAddingCpuLocked(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    mHandler->RemoveMessages(MSG_UPDATE_WAKELOCKS);

    if (mPartialTimers.IsEmpty()) {
        mLastPartialTimers.Clear();
        mDistributeWakelockCpu = FALSE;
        return NOERROR;
    }

    if (!mOnBatteryScreenOffTimeBase->IsRunning() && !mDistributeWakelockCpu) {
        return NOERROR;
    }

    mDistributeWakelockCpu = FALSE;

    // How many timers should consume CPU?  Only want to include ones
    // that have already been in the list.
    List<AutoPtr<StopwatchTimer> >::Iterator it = mPartialTimers.Begin();
    for (; it != mPartialTimers.End(); ++it) {
        AutoPtr<StopwatchTimer> st = *it;
        if (st->mInList) {
            AutoPtr<Uid> uid = st->mUid;
            // We don't include the system UID, because it so often
            // holds wake locks at one request or another of an app.
            if (uid != NULL && uid->mUid != IProcess::SYSTEM_UID) {
                *result = 50;
                return NOERROR;
            }
        }
    }

    return NOERROR;
}

ECode BatteryStatsImpl::FinishAddingCpuLocked(
    /* [in] */ Int32 perc,
    /* [in] */ Int32 utime,
    /* [in] */ Int32 stime,
    /* [in] */ ArrayOf<Int64>* cpuSpeedTimes)
{
    if (perc != 0) {
        Int32 num = 0;
        List<AutoPtr<StopwatchTimer> >::Iterator it = mPartialTimers.Begin();
        for (; it != mPartialTimers.End(); ++it) {
            AutoPtr<StopwatchTimer> st = *it;
            if (st->mInList) {
                AutoPtr<Uid> uid = st->mUid;
                // We don't include the system UID, because it so often
                // holds wake locks at one request or another of an app.
                if (uid != NULL && uid->mUid != IProcess::SYSTEM_UID) {
                    num++;
                }
            }
        }
        if (num != 0) {
            it = mPartialTimers.Begin();
            for (; it != mPartialTimers.End(); ++it) {
                AutoPtr<StopwatchTimer> st = *it;
                if (st->mInList) {
                    AutoPtr<Uid> uid = st->mUid;
                    if (uid != NULL && uid->mUid != IProcess::SYSTEM_UID) {
                        Int32 myUTime = utime/num;
                        Int32 mySTime = stime/num;
                        utime -= myUTime;
                        stime -= mySTime;
                        num--;
                        AutoPtr<Uid::Proc> proc = uid->GetProcessStatsLocked(String("*wakelock*"));
                        proc->AddCpuTimeLocked(myUTime, mySTime);
                        proc->AddSpeedStepTimes(cpuSpeedTimes);
                    }
                }
            }
        }

        // Just in case, collect any lost CPU time.
        if (utime != 0 || stime != 0) {
            AutoPtr<Uid> uid = GetUidStatsLocked(IProcess::SYSTEM_UID);
            if (uid != NULL) {
                AutoPtr<Uid::Proc> proc = uid->GetProcessStatsLocked(String("*lost*"));
                proc->AddCpuTimeLocked(utime, stime);
                proc->AddSpeedStepTimes(cpuSpeedTimes);
            }
        }
    }

    Int32 N = mPartialTimers.GetSize();
    Int32 NL = mLastPartialTimers.GetSize();
    Boolean diff = N != NL;
    List<AutoPtr<StopwatchTimer> >::Iterator it = mPartialTimers.Begin();
    List<AutoPtr<StopwatchTimer> >::Iterator lastIt = mLastPartialTimers.Begin();
    for (; lastIt != mLastPartialTimers.End() && !diff; ++lastIt, ++it) {
        diff |= *it != *lastIt;
    }
    if (!diff) {
        it = mPartialTimers.Begin();
        lastIt = mLastPartialTimers.Begin();
        for (; lastIt != mLastPartialTimers.End(); ++lastIt, ++it) {
            (*it)->mInList = TRUE;
        }
        return NOERROR;
    }

    lastIt = mLastPartialTimers.Begin();
    for (; lastIt != mLastPartialTimers.End(); ++lastIt) {
        (*lastIt)->mInList = FALSE;
    }
    mLastPartialTimers.Clear();
    it = mPartialTimers.Begin();
    for (; it != mPartialTimers.End(); ++it) {
        AutoPtr<StopwatchTimer> st = *it;
        st->mInList = TRUE;
        mLastPartialTimers.PushBack(st);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteProcessDiedLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    MapUid(uid, &uid);
    AutoPtr<Uid> u;
    AutoPtr<IInterface> value;
    mUidStats->Get(uid, (IInterface**)&value);
    if (value != NULL) {
        u = (Uid*)(IObject*)value.Get();
        u->mPids->Remove(pid);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::GetProcessWakeTime(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ Int64 realtime,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    MapUid(uid, &uid);
    AutoPtr<Uid> u;
    AutoPtr<IInterface> value;
    mUidStats->Get(uid, (IInterface**)&value);
    if (value != NULL) {
        u = (Uid*)(IObject*)value.Get();
        AutoPtr<Uid::Pid> p;
        AutoPtr<IInterface> temp;
        u->mPids->Get(pid, (IInterface**)&temp);
        if (temp != NULL) {
            p = (Uid::Pid*)(IObject*)temp.Get();
            *result = p->mWakeSumMs + (p->mWakeNesting > 0 ? (realtime - p->mWakeStartMs) : 0);
        }
    }
    return NOERROR;
}

ECode BatteryStatsImpl::ReportExcessiveWakeLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& proc,
    /* [in] */ Int64 overTime,
    /* [in] */ Int64 usedTime)
{
    MapUid(uid, &uid);
    AutoPtr<Uid> u;
    AutoPtr<IInterface> value;
    mUidStats->Get(uid, (IInterface**)&value);
    if (value != NULL) {
        u = (Uid*)(IObject*)value.Get();
        u->ReportExcessiveWakeLocked(proc, overTime, usedTime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::ReportExcessiveCpuLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& proc,
    /* [in] */ Int64 overTime,
    /* [in] */ Int64 usedTime)
{
    MapUid(uid, &uid);
    AutoPtr<Uid> u;
    AutoPtr<IInterface> value;
    mUidStats->Get(uid, (IInterface**)&value);
    if (value != NULL) {
        u = (Uid*)(IObject*)value.Get();
        u->ReportExcessiveCpuLocked(proc, overTime, usedTime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteStartSensorLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 sensor)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();

    if (mSensorNesting == 0) {
        mHistoryCur->mStates |= HistoryItem::STATE_SENSOR_ON_FLAG;
        if (DEBUG_HISTORY) {
            Logger::V(TAG, "Start sensor to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        }
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    mSensorNesting++;
    GetUidStatsLocked(uid)->NoteStartSensor(sensor, elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteStopSensorLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 sensor)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    mSensorNesting--;
    if (mSensorNesting == 0) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_SENSOR_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Stop sensor to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    GetUidStatsLocked(uid)->NoteStopSensor(sensor, elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteStartGpsLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (mGpsNesting == 0) {
        mHistoryCur->mStates |= HistoryItem::STATE_GPS_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Start GPS to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    mGpsNesting++;
    GetUidStatsLocked(uid)->NoteStartGps(elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteStopGpsLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    mGpsNesting--;
    if (mGpsNesting == 0) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_GPS_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Stop GPS to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    GetUidStatsLocked(uid)->NoteStopGps(elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteScreenStateLocked(
    /* [in] */ Int32 state)
{
    if (mScreenState != state) {
        Int32 oldState = mScreenState;
        mScreenState = state;
        if (DEBUG) Slogger::V(TAG, "Screen state: oldState=%s, newState=%s",
                Display::StateToString(oldState).string(), Display::StateToString(state).string());

        if (state != IDisplay::STATE_UNKNOWN) {
            Int32 stepState = state - 1;
            if (stepState < 4) {
                mModStepMode |= (mCurStepMode & STEP_LEVEL_MODE_SCREEN_STATE) ^ stepState;
                mCurStepMode = (mCurStepMode & ~STEP_LEVEL_MODE_SCREEN_STATE) | stepState;
            }
            else {
                Slogger::V(TAG, "Unexpected screen state: %d", state);
            }
        }

        if (state == IDisplay::STATE_ON) {
            // Screen turning on.
            Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
            Int64 uptime = SystemClock::GetUptimeMillis();
            mHistoryCur->mStates |= HistoryItem::STATE_SCREEN_ON_FLAG;
            if (DEBUG_HISTORY) Slogger::V(TAG, "Screen on to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
            AddHistoryRecordLocked(elapsedRealtime, uptime);
            mScreenOnTimer->StartRunningLocked(elapsedRealtime);
            if (mScreenBrightnessBin >= 0) {
                (*mScreenBrightnessTimer)[mScreenBrightnessBin]->StartRunningLocked(elapsedRealtime);
            }

            UpdateTimeBasesLocked(mOnBatteryTimeBase->IsRunning(), FALSE,
                    SystemClock::GetUptimeMillis() * 1000, elapsedRealtime * 1000);

            // Fake a wake lock, so we consider the device waked as long
            // as the screen is on.
            NoteStartWakeLocked(-1, -1, String("screen"), String(NULL), WAKE_TYPE_PARTIAL, FALSE,
                    elapsedRealtime, uptime);

            // Update discharge amounts.
            if (mOnBatteryInternal) {
                UpdateDischargeScreenLevelsLocked(FALSE, TRUE);
            }
        }
        else if (oldState == IDisplay::STATE_ON) {
            // Screen turning off or dozing.
            Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
            Int64 uptime = SystemClock::GetUptimeMillis();
            mHistoryCur->mStates &= ~HistoryItem::STATE_SCREEN_ON_FLAG;
            if (DEBUG_HISTORY) Slogger::V(TAG, "Screen off to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
            AddHistoryRecordLocked(elapsedRealtime, uptime);
            mScreenOnTimer->StopRunningLocked(elapsedRealtime);
            if (mScreenBrightnessBin >= 0) {
                (*mScreenBrightnessTimer)[mScreenBrightnessBin]->StopRunningLocked(elapsedRealtime);
            }

            NoteStopWakeLocked(-1, -1, String("screen"), String("screen"), WAKE_TYPE_PARTIAL,
                    elapsedRealtime, uptime);

            UpdateTimeBasesLocked(mOnBatteryTimeBase->IsRunning(), TRUE,
                    SystemClock::GetUptimeMillis() * 1000, elapsedRealtime * 1000);

            // Update discharge amounts.
            if (mOnBatteryInternal) {
                UpdateDischargeScreenLevelsLocked(TRUE, FALSE);
            }
        }
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteScreenBrightnessLocked(
    /* [in] */ Int32 brightness)
{
    // Bin the brightness.
    Int32 bin = brightness / (256 / NUM_SCREEN_BRIGHTNESS_BINS);
    if (bin < 0) bin = 0;
    else if (bin >= NUM_SCREEN_BRIGHTNESS_BINS) bin = NUM_SCREEN_BRIGHTNESS_BINS - 1;
    if (mScreenBrightnessBin != bin) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mHistoryCur->mStates = (mHistoryCur->mStates & ~HistoryItem::STATE_BRIGHTNESS_MASK)
                | (bin << HistoryItem::STATE_BRIGHTNESS_SHIFT);
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Screen brightness %d to: %s", bin, StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        if (mScreenState == IDisplay::STATE_ON) {
            if (mScreenBrightnessBin >= 0) {
                (*mScreenBrightnessTimer)[mScreenBrightnessBin]->StopRunningLocked(elapsedRealtime);
            }
            (*mScreenBrightnessTimer)[bin]->StartRunningLocked(elapsedRealtime);
        }
        mScreenBrightnessBin = bin;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteUserActivityLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 event)
{
    if (mOnBatteryInternal) {
        MapUid(uid, &uid);
        GetUidStatsLocked(uid)->NoteUserActivityLocked(event);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteInteractiveLocked(
    /* [in] */ Boolean interactive)
{
    if (mInteractive != interactive) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        mInteractive = interactive;
        if (DEBUG) Slogger::V(TAG, "Interactive: %d", interactive);
        if (interactive) {
            mInteractiveTimer->StartRunningLocked(elapsedRealtime);
        }
        else {
            mInteractiveTimer->StopRunningLocked(elapsedRealtime);
        }
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteMobileRadioPowerState(
    /* [in] */ Int32 powerState,
    /* [in] */ Int64 timestampNs)
{
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (mMobileRadioPowerState != powerState) {
        Int64 realElapsedRealtimeMs;
        Boolean active =
                powerState == IDataConnectionRealTimeInfo::DC_POWER_STATE_MEDIUM
                        || powerState == IDataConnectionRealTimeInfo::DC_POWER_STATE_HIGH;
        if (active) {
            mMobileRadioActiveStartTime = realElapsedRealtimeMs = elapsedRealtime;
            mHistoryCur->mStates |= HistoryItem::STATE_MOBILE_RADIO_ACTIVE_FLAG;
        }
        else {
            realElapsedRealtimeMs = timestampNs / (1000*1000);
            Int64 lastUpdateTimeMs = mMobileRadioActiveStartTime;
            if (realElapsedRealtimeMs < lastUpdateTimeMs) {
                Slogger::W(TAG, "Data connection inactive timestamp %d is before start time %d",
                        realElapsedRealtimeMs, lastUpdateTimeMs);
                realElapsedRealtimeMs = elapsedRealtime;
            }
            else if (realElapsedRealtimeMs < elapsedRealtime) {
                mMobileRadioActiveAdjustedTime->AddCountLocked(elapsedRealtime
                        - realElapsedRealtimeMs);
            }
            mHistoryCur->mStates &= ~HistoryItem::STATE_MOBILE_RADIO_ACTIVE_FLAG;
        }
        if (DEBUG_HISTORY) Slogger::V(TAG, "Mobile network active %d to: %s", active,
                StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mMobileRadioPowerState = powerState;
        if (active) {
            mMobileRadioActiveTimer->StartRunningLocked(elapsedRealtime);
            mMobileRadioActivePerAppTimer->StartRunningLocked(elapsedRealtime);
        }
        else {
            mMobileRadioActiveTimer->StopRunningLocked(realElapsedRealtimeMs);
            UpdateNetworkActivityLocked(NET_UPDATE_MOBILE, realElapsedRealtimeMs);
            mMobileRadioActivePerAppTimer->StopRunningLocked(realElapsedRealtimeMs);
        }
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteLowPowerMode(
    /* [in] */ Boolean enabled)
{
    if (mLowPowerModeEnabled != enabled) {
        Int32 stepState = enabled ? STEP_LEVEL_MODE_POWER_SAVE : 0;
        mModStepMode |= (mCurStepMode & STEP_LEVEL_MODE_POWER_SAVE) ^ stepState;
        mCurStepMode = (mCurStepMode & ~STEP_LEVEL_MODE_POWER_SAVE) | stepState;
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mLowPowerModeEnabled = enabled;
        if (enabled) {
            mHistoryCur->mStates2 |= HistoryItem::STATE2_LOW_POWER_FLAG;
            if (DEBUG_HISTORY) Slogger::V(TAG, "Low power mode enabled to: %s",
                    StringUtils::ToHexString(mHistoryCur->mStates2).string());
            mLowPowerModeEnabledTimer->StartRunningLocked(elapsedRealtime);
        }
        else {
            mHistoryCur->mStates2 &= ~HistoryItem::STATE2_LOW_POWER_FLAG;
            if (DEBUG_HISTORY) Slogger::V(TAG, "Low power mode disabled to: %s",
                    StringUtils::ToHexString(mHistoryCur->mStates2).string());
            mLowPowerModeEnabledTimer->StopRunningLocked(elapsedRealtime);
        }
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NotePhoneOnLocked()
{
    if (!mPhoneOn) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mHistoryCur->mStates |= HistoryItem::STATE_PHONE_IN_CALL_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Phone on to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mPhoneOn = TRUE;
        mPhoneOnTimer->StartRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NotePhoneOffLocked()
{
    if (mPhoneOn) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mHistoryCur->mStates &= ~HistoryItem::STATE_PHONE_IN_CALL_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Phone off to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mPhoneOn = FALSE;
        mPhoneOnTimer->StopRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

void BatteryStatsImpl::StopAllPhoneSignalStrengthTimersLocked(
        /* [in] */ Int32 except)
{
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        if (i == except) {
            continue;
        }
        while ((*mPhoneSignalStrengthsTimer)[i]->IsRunningLocked()) {
            (*mPhoneSignalStrengthsTimer)[i]->StopRunningLocked(elapsedRealtime);
        }
    }
}

Int32 BatteryStatsImpl::FixPhoneServiceState(
    /* [in] */ Int32 state,
    /* [in] */ Int32 signalBin)
{
    if (mPhoneSimStateRaw == ITelephonyManager::SIM_STATE_ABSENT) {
        // In this case we will always be STATE_OUT_OF_SERVICE, so need
        // to infer that we are scanning from other data.
        if (state == IServiceState::STATE_OUT_OF_SERVICE
                && signalBin > ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN) {
            state = IServiceState::STATE_IN_SERVICE;
        }
    }

    return state;
}

void BatteryStatsImpl::UpdateAllPhoneStateLocked(
    /* [in] */ Int32 state,
    /* [in] */ Int32 simState,
    /* [in] */ Int32 strengthBin)
{
    Boolean scanning = FALSE;
    Boolean newHistory = FALSE;

    mPhoneServiceStateRaw = state;
    mPhoneSimStateRaw = simState;
    mPhoneSignalStrengthBinRaw = strengthBin;

    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();

    if (simState == ITelephonyManager::SIM_STATE_ABSENT) {
        // In this case we will always be STATE_OUT_OF_SERVICE, so need
        // to infer that we are scanning from other data.
        if (state == IServiceState::STATE_OUT_OF_SERVICE
                && strengthBin > ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN) {
            state = IServiceState::STATE_IN_SERVICE;
        }
    }

    // If the phone is powered off, stop all timers.
    if (state == IServiceState::STATE_POWER_OFF) {
        strengthBin = -1;

    // If we are in service, make sure the correct signal string timer is running.
    }
    else if (state == IServiceState::STATE_IN_SERVICE) {
        // Bin will be changed below.

    // If we're out of service, we are in the lowest signal strength
    // bin and have the scanning bit set.
    }
    else if (state == IServiceState::STATE_OUT_OF_SERVICE) {
        scanning = TRUE;
        strengthBin = ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN;
        if (!mPhoneSignalScanningTimer->IsRunningLocked()) {
            mHistoryCur->mStates |= HistoryItem::STATE_PHONE_SCANNING_FLAG;
            newHistory = TRUE;
            if (DEBUG_HISTORY)
                Logger::V(TAG, "Phone started scanning to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
            mPhoneSignalScanningTimer->StartRunningLocked(elapsedRealtime);
        }
    }

    if (!scanning) {
        // If we are no longer scanning, then stop the scanning timer.
        if (mPhoneSignalScanningTimer->IsRunningLocked()) {
            mHistoryCur->mStates &= ~HistoryItem::STATE_PHONE_SCANNING_FLAG;
            if (DEBUG_HISTORY)
                Logger::V(TAG, "Phone stopped scanning to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
            newHistory = TRUE;
            mPhoneSignalScanningTimer->StopRunningLocked(elapsedRealtime);
        }
    }

    if (mPhoneServiceState != state) {
        mHistoryCur->mStates = (mHistoryCur->mStates & ~HistoryItem::STATE_PHONE_STATE_MASK)
                | (state << HistoryItem::STATE_PHONE_STATE_SHIFT);
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Phone state %d to: %s", state, StringUtils::ToHexString(mHistoryCur->mStates).string());
        newHistory = TRUE;
        mPhoneServiceState = state;
    }

    if (mPhoneSignalStrengthBin != strengthBin) {
        if (mPhoneSignalStrengthBin >= 0) {
            (*mPhoneSignalStrengthsTimer)[mPhoneSignalStrengthBin]->StopRunningLocked(elapsedRealtime);
        }
        if (strengthBin >= 0) {
            if (!(*mPhoneSignalStrengthsTimer)[strengthBin]->IsRunningLocked()) {
                (*mPhoneSignalStrengthsTimer)[strengthBin]->StartRunningLocked(elapsedRealtime);
            }
            mHistoryCur->mStates = (mHistoryCur->mStates & ~HistoryItem::STATE_PHONE_SIGNAL_STRENGTH_MASK)
                    | (strengthBin << HistoryItem::STATE_PHONE_SIGNAL_STRENGTH_SHIFT);
            if (DEBUG_HISTORY)
                Logger::V(TAG, "Signal strength %d to: %s", strengthBin, StringUtils::ToHexString(mHistoryCur->mStates).string());
            newHistory = TRUE;
        }
        else {
            StopAllPhoneSignalStrengthTimersLocked(-1);
        }
        mPhoneSignalStrengthBin = strengthBin;
    }

    if (newHistory) {
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
}

ECode BatteryStatsImpl::NotePhoneStateLocked(
    /* [in] */ Int32 state,
    /* [in] */ Int32 simState)
{
    UpdateAllPhoneStateLocked(state, simState, mPhoneSignalStrengthBinRaw);
    return NOERROR;
}

ECode BatteryStatsImpl::NotePhoneSignalStrengthLocked(
    /* [in] */ ISignalStrength* signalStrength)
{
    // Bin the strength.
    Int32 bin;
    signalStrength->GetLevel(&bin);
    UpdateAllPhoneStateLocked(mPhoneServiceStateRaw, mPhoneSimStateRaw, bin);
    return NOERROR;
}

ECode BatteryStatsImpl::NotePhoneDataConnectionStateLocked(
    /* [in] */ Int32 dataType,
    /* [in] */ Boolean hasData)
{
    Int32 bin = DATA_CONNECTION_NONE;
    if (hasData) {
        switch (dataType) {
            case ITelephonyManager::NETWORK_TYPE_EDGE:
                bin = DATA_CONNECTION_EDGE;
                break;
            case ITelephonyManager::NETWORK_TYPE_GPRS:
                bin = DATA_CONNECTION_GPRS;
                break;
            case ITelephonyManager::NETWORK_TYPE_UMTS:
                bin = DATA_CONNECTION_UMTS;
                break;
            case ITelephonyManager::NETWORK_TYPE_CDMA:
                bin = DATA_CONNECTION_CDMA;
                break;
            case ITelephonyManager::NETWORK_TYPE_EVDO_0:
                bin = DATA_CONNECTION_EVDO_0;
                break;
            case ITelephonyManager::NETWORK_TYPE_EVDO_A:
                bin = DATA_CONNECTION_EVDO_A;
                break;
            case ITelephonyManager::NETWORK_TYPE_1xRTT:
                bin = DATA_CONNECTION_1xRTT;
                break;
            case ITelephonyManager::NETWORK_TYPE_HSDPA:
                bin = DATA_CONNECTION_HSDPA;
                break;
            case ITelephonyManager::NETWORK_TYPE_HSUPA:
                bin = DATA_CONNECTION_HSUPA;
                break;
            case ITelephonyManager::NETWORK_TYPE_HSPA:
                bin = DATA_CONNECTION_HSPA;
                break;
            case ITelephonyManager::NETWORK_TYPE_IDEN:
                bin = DATA_CONNECTION_IDEN;
                break;
            case ITelephonyManager::NETWORK_TYPE_EVDO_B:
                bin = DATA_CONNECTION_EVDO_B;
                break;
            case ITelephonyManager::NETWORK_TYPE_LTE:
                bin = DATA_CONNECTION_LTE;
                break;
            case ITelephonyManager::NETWORK_TYPE_EHRPD:
                bin = DATA_CONNECTION_EHRPD;
                break;
            case ITelephonyManager::NETWORK_TYPE_HSPAP:
                bin = DATA_CONNECTION_HSPAP;
                break;
            default:
                bin = DATA_CONNECTION_OTHER;
                break;
        }
    }
    if (DEBUG) Logger::I(TAG, "Phone Data Connection -> %d = %d", dataType, hasData);
    if (mPhoneDataConnectionType != bin) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mHistoryCur->mStates = (mHistoryCur->mStates & ~HistoryItem::STATE_DATA_CONNECTION_MASK)
                | (bin << HistoryItem::STATE_DATA_CONNECTION_SHIFT);
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Data connection %d to: %s", bin, StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        if (mPhoneDataConnectionType >= 0) {
            (*mPhoneDataConnectionsTimer)[mPhoneDataConnectionType]->StopRunningLocked(elapsedRealtime);
        }
        mPhoneDataConnectionType = bin;
        (*mPhoneDataConnectionsTimer)[bin]->StartRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiOnLocked()
{
    if (!mWifiOn) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mHistoryCur->mStates2 |= HistoryItem::STATE2_WIFI_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI on to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mWifiOn = TRUE;
        mWifiOnTimer->StartRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiOffLocked()
{
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (mWifiOn) {
        mHistoryCur->mStates2 &= ~HistoryItem::STATE2_WIFI_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI off to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mWifiOn = FALSE;
        mWifiOnTimer->StopRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteAudioOnLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (mAudioOnNesting == 0) {
        mHistoryCur->mStates |= HistoryItem::STATE_AUDIO_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Audio on to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mAudioOnTimer->StartRunningLocked(elapsedRealtime);
    }
    mAudioOnNesting++;
    GetUidStatsLocked(uid)->NoteAudioTurnedOnLocked(elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteAudioOffLocked(
    /* [in] */ Int32 uid)
{
    if (mAudioOnNesting == 0) {
        return NOERROR;
    }
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (--mAudioOnNesting == 0) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_AUDIO_ON_FLAG;
        if (DEBUG_HISTORY)
            Slogger::V(TAG, "Audio off to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mAudioOnTimer->StopRunningLocked(elapsedRealtime);
    }
    GetUidStatsLocked(uid)->NoteAudioTurnedOffLocked(elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteVideoOnLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (mVideoOnNesting == 0) {
        mHistoryCur->mStates2 |= HistoryItem::STATE2_VIDEO_ON_FLAG;
        if (DEBUG_HISTORY)
            Slogger::V(TAG, "Video on to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mVideoOnTimer->StartRunningLocked(elapsedRealtime);
    }
    mVideoOnNesting++;
    GetUidStatsLocked(uid)->NoteVideoTurnedOnLocked(elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteVideoOffLocked(
    /* [in] */ Int32 uid)
{
    if (mVideoOnNesting == 0) {
        return NOERROR;
    }
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (--mVideoOnNesting == 0) {
        mHistoryCur->mStates2 &= ~HistoryItem::STATE2_VIDEO_ON_FLAG;
        if (DEBUG_HISTORY)
            Slogger::V(TAG, "Video off to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mVideoOnTimer->StopRunningLocked(elapsedRealtime);
    }
    GetUidStatsLocked(uid)->NoteVideoTurnedOffLocked(elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteResetAudioLocked()
{
    if (mAudioOnNesting > 0) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mAudioOnNesting = 0;
        mHistoryCur->mStates &= ~HistoryItem::STATE_AUDIO_ON_FLAG;
        if (DEBUG_HISTORY)
            Slogger::V(TAG, "Audio off to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mAudioOnTimer->StopAllRunningLocked(elapsedRealtime);
        Int32 size;
        mUidStats->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> value;
            mUidStats->ValueAt(i, (IInterface**)&value);
            AutoPtr<Uid> uid = (Uid*)(IObject*)value.Get();
            uid->NoteResetAudioLocked(elapsedRealtime);
        }
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteResetVideoLocked()
{
    if (mVideoOnNesting > 0) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mAudioOnNesting = 0;
        mHistoryCur->mStates2 &= ~HistoryItem::STATE2_VIDEO_ON_FLAG;
        if (DEBUG_HISTORY)
            Slogger::V(TAG, "Video off to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mVideoOnTimer->StopAllRunningLocked(elapsedRealtime);
        Int32 size;
        mUidStats->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> value;
            mUidStats->ValueAt(i, (IInterface**)&value);
            AutoPtr<Uid> uid = (Uid*)(IObject*)value.Get();
            uid->NoteResetAudioLocked(elapsedRealtime);
        }
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteActivityResumedLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    GetUidStatsLocked(uid)->NoteActivityResumedLocked(SystemClock::GetElapsedRealtime());
    return NOERROR;
}

ECode BatteryStatsImpl::NoteActivityPausedLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    GetUidStatsLocked(uid)->NoteActivityPausedLocked(SystemClock::GetElapsedRealtime());
    return NOERROR;
}

ECode BatteryStatsImpl::NoteVibratorOnLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int64 durationMillis)
{
    MapUid(uid, &uid);
    GetUidStatsLocked(uid)->NoteVibratorOnLocked(durationMillis);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteVibratorOffLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    GetUidStatsLocked(uid)->NoteVibratorOffLocked();
    return NOERROR;
}

ECode BatteryStatsImpl::NoteFlashlightOnLocked()
{
    if (!mFlashlightOn) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mHistoryCur->mStates2 |= HistoryItem::STATE2_FLASHLIGHT_FLAG;
        if (DEBUG_HISTORY)
            Slogger::V(TAG, "Flashlight on to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mFlashlightOn = TRUE;
        mFlashlightOnTimer->StartRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteFlashlightOffLocked()
{
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (mFlashlightOn) {
        mHistoryCur->mStates2 &= ~HistoryItem::STATE2_FLASHLIGHT_FLAG;
        if (DEBUG_HISTORY)
            Slogger::V(TAG, "Flashlight off to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mFlashlightOn = FALSE;
        mFlashlightOnTimer->StopRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiRunningLocked(
    /* [in] */ IWorkSource* ws)
{
    if (!mGlobalWifiRunning) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mHistoryCur->mStates2 |= HistoryItem::STATE2_WIFI_RUNNING_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI running to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mGlobalWifiRunning = TRUE;
        mGlobalWifiRunningTimer->StartRunningLocked(elapsedRealtime);
        Int32 N;
        ws->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            Int32 value;
            ws->Get(i, &value);
            Int32 uid;
            MapUid(value, &uid);
            GetUidStatsLocked(uid)->NoteWifiRunningLocked(elapsedRealtime);
        }
    }
    else {
        Logger::W(TAG, "noteWifiRunningLocked -- called while WIFI running");
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiRunningChangedLocked(
    /* [in] */ IWorkSource* oldWs,
    /* [in] */ IWorkSource* newWs)
{
    if (mGlobalWifiRunning) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int32 N;
        oldWs->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            Int32 value;
            oldWs->Get(i, &value);
            Int32 uid;
            MapUid(value, &uid);
            GetUidStatsLocked(uid)->NoteWifiStoppedLocked(elapsedRealtime);
        }
        newWs->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            Int32 value;
            newWs->Get(i, &value);
            Int32 uid;
            MapUid(value, &uid);
            GetUidStatsLocked(uid)->NoteWifiRunningLocked(elapsedRealtime);
        }
    }
    else {
        Logger::W(TAG, "noteWifiRunningChangedLocked -- called while WIFI not running");
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiStoppedLocked(
    /* [in] */ IWorkSource* ws)
{
    if (mGlobalWifiRunning) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mHistoryCur->mStates2 &= ~HistoryItem::STATE2_WIFI_RUNNING_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI stopped to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mGlobalWifiRunning = FALSE;
        mGlobalWifiRunningTimer->StopRunningLocked(elapsedRealtime);
        Int32 N;
        ws->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            Int32 value;
            ws->Get(i, &value);
            Int32 uid;
            MapUid(value, &uid);
            GetUidStatsLocked(uid)->NoteWifiStoppedLocked(elapsedRealtime);
        }
    }
    else {
        Logger::W(TAG, "noteWifiStoppedLocked -- called while WIFI not running");
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiStateLocked(
    /* [in] */ Int32 wifiState,
    /* [in] */ const String& accessPoint)
{
    if (DEBUG) Logger::I(TAG, "WiFi state -> %d", wifiState);
    if (mWifiState != wifiState) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        if (mWifiState >= 0) {
            (*mWifiStateTimer)[mWifiState]->StopRunningLocked(elapsedRealtime);
        }
        mWifiState = wifiState;
        (*mWifiStateTimer)[wifiState]->StartRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiSupplicantStateChangedLocked(
    /* [in] */ Int32 supplState,
    /* [in] */ Boolean failedAuth)
{
    if (DEBUG) Logger::I(TAG, "WiFi suppl state -> %d", supplState);
    if (mWifiSupplState != supplState) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        if (mWifiSupplState >= 0) {
            (*mWifiSupplStateTimer)[mWifiSupplState]->StopRunningLocked(elapsedRealtime);
        }
        mWifiSupplState = supplState;
        (*mWifiSupplStateTimer)[supplState]->StartRunningLocked(elapsedRealtime);
        mHistoryCur->mStates2 =
                (mHistoryCur->mStates2 & ~HistoryItem::STATE2_WIFI_SUPPL_STATE_MASK)
                | (supplState << HistoryItem::STATE2_WIFI_SUPPL_STATE_SHIFT);
        if (DEBUG_HISTORY)
            Slogger::V(TAG, "Wifi suppl state %d to: %s",
                    supplState, StringUtils::ToHexString(mHistoryCur->mStates2).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    return NOERROR;
}

void BatteryStatsImpl::StopAllWifiSignalStrengthTimersLocked(
    /* [in] */ Int32 except)
{
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    for (Int32 i = 0; i < NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        if (i == except) {
            continue;
        }
        while ((*mWifiSignalStrengthsTimer)[i]->IsRunningLocked()) {
            (*mWifiSignalStrengthsTimer)[i]->StopRunningLocked(elapsedRealtime);
        }
    }
}

ECode BatteryStatsImpl::NoteWifiRssiChangedLocked(
    /* [in] */ Int32 newRssi)
{
    AutoPtr<IWifiManagerHelper> helper;
    CWifiManagerHelper::AcquireSingleton((IWifiManagerHelper**)&helper);
    Int32 strengthBin;
    helper->CalculateSignalLevel(newRssi, NUM_WIFI_SIGNAL_STRENGTH_BINS, &strengthBin);
    if (DEBUG) Logger::I(TAG, "WiFi rssi -> %d bin=%d", newRssi, strengthBin);
    if (mWifiSignalStrengthBin != strengthBin) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        if (mWifiSignalStrengthBin >= 0) {
            (*mWifiSignalStrengthsTimer)[mWifiSignalStrengthBin]->StopRunningLocked(
                    elapsedRealtime);
        }
        if (strengthBin >= 0) {
            if (!(*mWifiSignalStrengthsTimer)[strengthBin]->IsRunningLocked()) {
                (*mWifiSignalStrengthsTimer)[strengthBin]->StartRunningLocked(elapsedRealtime);
            }
            mHistoryCur->mStates2 =
                    (mHistoryCur->mStates2 & ~HistoryItem::STATE2_WIFI_SIGNAL_STRENGTH_MASK)
                    | (strengthBin << HistoryItem::STATE2_WIFI_SIGNAL_STRENGTH_SHIFT);
            if (DEBUG_HISTORY)
                Slogger::V(TAG, "Wifi signal strength %d to: %s",
                        strengthBin, StringUtils::ToHexString(mHistoryCur->mStates2).string());
            AddHistoryRecordLocked(elapsedRealtime, uptime);
        }
        else {
            StopAllWifiSignalStrengthTimersLocked(-1);
        }
        mWifiSignalStrengthBin = strengthBin;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteBluetoothOnLocked()
{
    if (!mBluetoothOn) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mHistoryCur->mStates |= HistoryItem::STATE_BLUETOOTH_ON_FLAG;
        if (DEBUG_HISTORY)
            Slogger::V(TAG, "Bluetooth on to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mBluetoothOn = TRUE;
        mBluetoothOnTimer->StartRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteBluetoothOffLocked()
{
    if (mBluetoothOn) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        mHistoryCur->mStates &= ~HistoryItem::STATE_BLUETOOTH_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Bluetooth off to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
        mBluetoothOn = FALSE;
        mBluetoothOnTimer->StopRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteBluetoothStateLocked(
    /* [in] */ Int32 bluetoothState)
{
    if (DEBUG) Logger::I(TAG, "Bluetooth state -> %d", bluetoothState);
    if (mBluetoothState != bluetoothState) {
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        if (mBluetoothState >= 0) {
            (*mBluetoothStateTimer)[mBluetoothState]->StopRunningLocked(elapsedRealtime);
        }
        mBluetoothState = bluetoothState;
        (*mBluetoothStateTimer)[bluetoothState]->StartRunningLocked(elapsedRealtime);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteFullWifiLockAcquiredLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (mWifiFullLockNesting == 0) {
        mHistoryCur->mStates |= HistoryItem::STATE_WIFI_FULL_LOCK_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI full lock on to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    mWifiFullLockNesting++;
    GetUidStatsLocked(uid)->NoteFullWifiLockAcquiredLocked(elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteFullWifiLockReleasedLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    mWifiFullLockNesting--;
    if (mWifiFullLockNesting == 0) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_WIFI_FULL_LOCK_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI full lock off to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    GetUidStatsLocked(uid)->NoteFullWifiLockReleasedLocked(elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiScanStartedLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (mWifiScanNesting == 0) {
        mHistoryCur->mStates |= HistoryItem::STATE_WIFI_SCAN_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI full lock on to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    mWifiScanNesting++;
    GetUidStatsLocked(uid)->NoteWifiScanStartedLocked(elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiScanStoppedLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    mWifiScanNesting--;
    if (mWifiScanNesting == 0) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_WIFI_SCAN_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI full lock on to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    GetUidStatsLocked(uid)->NoteWifiScanStoppedLocked(elapsedRealtime);
    return NOERROR;
}

void BatteryStatsImpl::NoteWifiBatchedScanStartedLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 csph)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    GetUidStatsLocked(uid)->NoteWifiBatchedScanStartedLocked(csph, elapsedRealtime);
}

void BatteryStatsImpl::NoteWifiBatchedScanStoppedLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    GetUidStatsLocked(uid)->NoteWifiBatchedScanStoppedLocked(elapsedRealtime);
}

ECode BatteryStatsImpl::NoteWifiMulticastEnabledLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    if (mWifiMulticastNesting == 0) {
        mHistoryCur->mStates |= HistoryItem::STATE_WIFI_MULTICAST_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI multicast on to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    mWifiMulticastNesting++;
    GetUidStatsLocked(uid)->NoteWifiMulticastEnabledLocked(elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiMulticastDisabledLocked(
    /* [in] */ Int32 uid)
{
    MapUid(uid, &uid);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int64 uptime = SystemClock::GetUptimeMillis();
    mWifiMulticastNesting--;
    if (mWifiMulticastNesting == 0) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_WIFI_MULTICAST_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI multicast off to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(elapsedRealtime, uptime);
    }
    GetUidStatsLocked(uid)->NoteWifiMulticastDisabledLocked(elapsedRealtime);
    return NOERROR;
}

ECode BatteryStatsImpl::NoteFullWifiLockAcquiredFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteFullWifiLockAcquiredLocked(value);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteFullWifiLockReleasedFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteFullWifiLockReleasedLocked(value);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiScanStartedFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteWifiScanStartedLocked(value);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiScanStoppedFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteWifiScanStoppedLocked(value);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiBatchedScanStartedFromSourceLocked(
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 csph)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteWifiBatchedScanStartedLocked(value, csph);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiBatchedScanStoppedFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteWifiBatchedScanStoppedLocked(value);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiMulticastEnabledFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteWifiMulticastEnabledLocked(value);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteWifiMulticastDisabledFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteWifiMulticastDisabledLocked(value);
    }
    return NOERROR;
}

AutoPtr<ArrayOf<String> > BatteryStatsImpl::IncludeInStringArray(
    /* [in] */ ArrayOf<String>* array,
    /* [in] */ const String& _str)
{
    String str = _str;
    if (ArrayUtils::IndexOf(array, str) >= 0) {
        return array;
    }
    AutoPtr<ArrayOf<String> > newArray = ArrayOf<String>::Alloc(array->GetLength() + 1);
    newArray->Copy(array);
    (*newArray)[array->GetLength()] = str;
    return newArray;
}

AutoPtr<ArrayOf<String> > BatteryStatsImpl::ExcludeFromStringArray(
    /* [in] */ ArrayOf<String>* array,
    /* [in] */ const String& str)
{
    Int32 index = ArrayUtils::IndexOf(array, str);
    if (index >= 0) {
        AutoPtr<ArrayOf<String> > newArray = ArrayOf<String>::Alloc(array->GetLength() - 1);
        if (index > 0) {
            newArray->Copy(array);
        }
        if (index < array->GetLength() - 1) {
            newArray->Copy(index, array, index + 1, array->GetLength() - index - 1);
        }
        return newArray;
    }
    return array;
}

ECode BatteryStatsImpl::NoteNetworkInterfaceTypeLocked(
    /* [in] */ const String& iface,
    /* [in] */ Int32 networkType)
{
    if (TextUtils::IsEmpty(iface)) return NOERROR;
    AutoPtr<IConnectivityManagerHelper> helper;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&helper);
    Boolean result;
    if (helper->IsNetworkTypeMobile(networkType, &result), result) {
        mMobileIfaces = IncludeInStringArray(mMobileIfaces, iface);
        if (DEBUG) Slogger::D(TAG, "Note mobile iface %s: %p", iface.string(), mMobileIfaces.Get());
    }
    else {
        mMobileIfaces = ExcludeFromStringArray(mMobileIfaces, iface);
        if (DEBUG) Slogger::D(TAG, "Note non-mobile iface %s: %p", iface.string(), mMobileIfaces.Get());
    }
    if (helper->IsNetworkTypeWifi(networkType, &result), result) {
        mWifiIfaces = IncludeInStringArray(mWifiIfaces, iface);
        if (DEBUG) Slogger::D(TAG, "Note wifi iface %s: %p", iface.string(), mWifiIfaces.Get());
    }
    else {
        mWifiIfaces = ExcludeFromStringArray(mWifiIfaces, iface);
        if (DEBUG) Slogger::D(TAG, "Note non-wifi iface %s: %p", iface.string(), mWifiIfaces.Get());
    }
    return NOERROR;
}

ECode BatteryStatsImpl::NoteNetworkStatsEnabledLocked()
{
    // During device boot, qtaguid isn't enabled until after the inital
    // loading of battery stats. Now that they're enabled, take our initial
    // snapshot for future delta calculation.
    UpdateNetworkActivityLocked(NET_UPDATE_ALL, SystemClock::GetElapsedRealtime());
    return NOERROR;
}

ECode BatteryStatsImpl::GetScreenOnTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mScreenOnTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetScreenOnCount(
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mScreenOnTimer->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetScreenBrightnessTime(
    /* [in] */ Int32 brightnessBin,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mScreenBrightnessTimer)[brightnessBin]->GetTotalTimeLocked(
            elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetInteractiveTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mInteractiveTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetLowPowerModeEnabledTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mLowPowerModeEnabledTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetLowPowerModeEnabledCount(
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mLowPowerModeEnabledTimer->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetPhoneOnTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneOnTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetPhoneOnCount(
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneOnTimer->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetPhoneSignalStrengthTime(
    /* [in] */ Int32 strengthBin,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mPhoneSignalStrengthsTimer)[strengthBin]->GetTotalTimeLocked(elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetPhoneSignalScanningTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mPhoneSignalScanningTimer->GetTotalTimeLocked(
            elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetPhoneSignalStrengthCount(
    /* [in] */ Int32 strengthBin,
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mPhoneSignalStrengthsTimer)[strengthBin]->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetPhoneDataConnectionTime(
    /* [in] */ Int32 dataType,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mPhoneDataConnectionsTimer)[dataType]->GetTotalTimeLocked(
            elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetPhoneDataConnectionCount(
    /* [in] */ Int32 dataType,
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mPhoneDataConnectionsTimer)[dataType]->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetMobileRadioActiveTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mMobileRadioActiveTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetMobileRadioActiveCount(
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mMobileRadioActiveTimer->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetMobileRadioActiveAdjustedTime(
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mMobileRadioActiveAdjustedTime->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetMobileRadioActiveUnknownTime(
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mMobileRadioActiveUnknownTime->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetMobileRadioActiveUnknownCount(
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 r;
    mMobileRadioActiveUnknownCount->GetCountLocked(which, &r);
    *result = (Int32)r;
    return NOERROR;
}

ECode BatteryStatsImpl::GetWifiOnTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mWifiOnTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetGlobalWifiRunningTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mGlobalWifiRunningTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetWifiStateTime(
    /* [in] */ Int32 wifiState,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mWifiStateTimer)[wifiState]->GetTotalTimeLocked(
            elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetWifiStateCount(
    /* [in] */ Int32 wifiState,
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mWifiStateTimer)[wifiState]->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetWifiSupplStateTime(
    /* [in] */ Int32 state,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mWifiSupplStateTimer)[state]->GetTotalTimeLocked(
            elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetWifiSupplStateCount(
    /* [in] */ Int32 state,
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mWifiSupplStateTimer)[state]->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetWifiSignalStrengthTime(
    /* [in] */ Int32 strengthBin,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mWifiSignalStrengthsTimer)[strengthBin]->GetTotalTimeLocked(
            elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetWifiSignalStrengthCount(
    /* [in] */ Int32 strengthBin,
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mWifiSignalStrengthsTimer)[strengthBin]->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetBluetoothOnTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mBluetoothOnTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetBluetoothStateTime(
    /* [in] */ Int32 bluetoothState,
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mBluetoothStateTimer)[bluetoothState]->GetTotalTimeLocked(
            elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetBluetoothStateCount(
    /* [in] */ Int32 bluetoothState,
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return (*mBluetoothStateTimer)[bluetoothState]->GetCountLocked(which, result);
}

ECode BatteryStatsImpl::GetFlashlightOnTime(
    /* [in] */ Int64 elapsedRealtimeUs,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mFlashlightOnTimer->GetTotalTimeLocked(elapsedRealtimeUs, which, result);
}

ECode BatteryStatsImpl::GetFlashlightOnCount(
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 r;
    mFlashlightOnTimer->GetCountLocked(which, &r);
    *result = (Int64)r;
    return NOERROR;
}

ECode BatteryStatsImpl::GetNetworkActivityBytes(
    /* [in] */ Int32 type,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (type >= 0 && type < mNetworkByteActivityCounters->GetLength()) {
        (*mNetworkByteActivityCounters)[type]->GetCountLocked(which, result);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::GetNetworkActivityPackets(
    /* [in] */ Int32 type,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;
    if (type >= 0 && type < mNetworkPacketActivityCounters->GetLength()) {
        (*mNetworkByteActivityCounters)[type]->GetCountLocked(which, result);
    }
    return NOERROR;
}

Boolean BatteryStatsImpl::IsStartClockTimeValid()
{
    Int64 value = (Int64)365 * 24 * 60 * 60 * 1000;
    return mStartClockTime > value;
}

ECode BatteryStatsImpl::GetStartClockTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    if (!IsStartClockTimeValid()) {
        // If the last clock time we got was very small, then we hadn't had a real
        // time yet, so try to get it again.
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        sys->GetCurrentTimeMillis(&mStartClockTime);
        if (IsStartClockTimeValid()) {
            RecordCurrentTimeChangeLocked(mStartClockTime, SystemClock::GetElapsedRealtime(),
                    SystemClock::GetUptimeMillis());
        }
    }
    *result = mStartClockTime;
    return NOERROR;
}

ECode BatteryStatsImpl::GetStartPlatformVersion(
    /* [out] */ String* version)
{
    VALIDATE_NOT_NULL(version)
    *version = mStartPlatformVersion;
    return NOERROR;
}

ECode BatteryStatsImpl::GetEndPlatformVersion(
    /* [out] */ String* version)
{
    VALIDATE_NOT_NULL(version)
    *version = mEndPlatformVersion;
    return NOERROR;
}

ECode BatteryStatsImpl::GetParcelVersion(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = VERSION;
    return NOERROR;
}

ECode BatteryStatsImpl::GetIsOnBattery(
    /* [out] */ Boolean* isOnBattery)
{
    VALIDATE_NOT_NULL(isOnBattery)
    *isOnBattery = mOnBattery;
    return NOERROR;
}

ECode BatteryStatsImpl::GetUidStats(
    /* [out] */ ISparseArray** stats)
{
    VALIDATE_NOT_NULL(stats)
    *stats = mUidStats;
    REFCOUNT_ADD(*stats)
    return NOERROR;
}

ECode BatteryStatsImpl::SetCallback(
    /* [in] */ IBatteryCallback* cb)
{
    mCallback = cb;
    return NOERROR;
}

ECode BatteryStatsImpl::SetNumSpeedSteps(
    /* [in] */ Int32 steps)
{
    if (sNumSpeedSteps == 0) sNumSpeedSteps = steps;
    return NOERROR;
}

ECode BatteryStatsImpl::SetRadioScanningTimeout(
    /* [in] */ Int64 timeout)
{
    if (mPhoneSignalScanningTimer != NULL) {
        mPhoneSignalScanningTimer->SetTimeout(timeout);
    }
    return NOERROR;
}

ECode BatteryStatsImpl::StartIteratingOldHistoryLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    if (DEBUG_HISTORY) {
        Int32 size, pos;
        mHistoryBuffer->GetElementSize(&size);
        mHistoryBuffer->GetDataPosition(&pos);
        Slogger::I(TAG, "ITERATING: buff size=%d pos=%d", size, pos);
    }
    if ((mHistoryIterator = mHistory) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    mHistoryBuffer->SetDataPosition(0);
    mHistoryReadTmp->Clear();
    mReadOverflow = FALSE;
    mIteratingHistory = TRUE;
    *result = TRUE;
    return NOERROR;
}

ECode BatteryStatsImpl::GetNextOldHistoryLocked(
    /* [in] */ IBatteryStatsHistoryItem* out,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int32 pos, size;
    mHistoryBuffer->GetDataPosition(&pos);
    mHistoryBuffer->GetElementSize(&size);
    Boolean end = pos >= size;
    if (!end) {
        ReadHistoryDelta(mHistoryBuffer, mHistoryReadTmp);
        mReadOverflow |= mHistoryReadTmp->mCmd == IBatteryStatsHistoryItem::CMD_OVERFLOW;
    }
    AutoPtr<HistoryItem> cur = mHistoryIterator;
    if (cur == NULL) {
        if (!mReadOverflow && !end) {
            Slogger::W(TAG, "Old history ends before new history!");
        }
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<HistoryItem> hi = (HistoryItem*)out;
    hi->SetTo(cur);
    mHistoryIterator = cur->mNext;
    if (!mReadOverflow) {
        if (end) {
            Slogger::W(TAG, "New history ends before old history!");
        }
        else if (!hi->Same(mHistoryReadTmp)) {
            // TODO
            // PrintWriter pw = new FastPrintWriter(new LogWriter(android.util.Log.WARN, TAG));
            // pw.println("Histories differ!");
            // pw.println("Old history:");
            // (new HistoryPrinter()).printNextItem(pw, out, 0, false, true);
            // pw.println("New history:");
            // (new HistoryPrinter()).printNextItem(pw, mHistoryReadTmp, 0, false,
            //         true);
            // pw.flush();
        }
    }
    *result = TRUE;
    return NOERROR;
}

ECode BatteryStatsImpl::GetHistoryTotalSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = MAX_HISTORY_BUFFER;
    return NOERROR;
}

ECode BatteryStatsImpl::GetHistoryUsedSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    mHistoryBuffer->GetElementSize(size);
    return NOERROR;
}

ECode BatteryStatsImpl::FinishIteratingOldHistoryLocked()
{
    mIteratingHistory = FALSE;
    Int32 size;
    mHistoryBuffer->GetElementSize(&size);
    mHistoryBuffer->SetDataPosition(size);
    return NOERROR;
}

ECode BatteryStatsImpl::StartIteratingHistoryLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 size;
    mHistoryBuffer->GetElementSize(&size);
    if (DEBUG_HISTORY) {
        Int32 pos;
        mHistoryBuffer->GetDataPosition(&pos);
        Slogger::I(TAG, "ITERATING: buff size=%d pos=%d", size, pos);
    }
    if (size <= 0) {
        *result = FALSE;
        return NOERROR;
    }
    mHistoryBuffer->SetDataPosition(0);
    mReadOverflow = FALSE;
    mIteratingHistory = TRUE;
    mReadHistoryStrings = ArrayOf<String>::Alloc(mHistoryTagPool.GetSize());
    mReadHistoryUids = ArrayOf<Int32>::Alloc(mHistoryTagPool.GetSize());
    mReadHistoryChars = 0;
    HashMap<AutoPtr<HistoryTag>, AutoPtr<IInteger32> >::Iterator it = mHistoryTagPool.Begin();
    for (; it != mHistoryTagPool.End(); ++it) {
        AutoPtr<HistoryTag> tag = it->mFirst;
        Int32 idx;
        it->mSecond->GetValue(&idx);
        (*mReadHistoryStrings)[idx] = tag->mString;
        (*mReadHistoryUids)[idx] = tag->mUid;
        mReadHistoryChars += tag->mString.GetLength() + 1;
    }
    *result = TRUE;
    return NOERROR;
}

ECode BatteryStatsImpl::GetHistoryStringPoolSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mReadHistoryStrings->GetLength();
    return NOERROR;
}

ECode BatteryStatsImpl::GetHistoryStringPoolBytes(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // Each entry is a fixed 12 bytes: 4 for index, 4 for uid, 4 for string size
    // Each string character is 2 bytes.
    *result = (mReadHistoryStrings->GetLength() * 12) + (mReadHistoryChars * 2);
    return NOERROR;
}

ECode BatteryStatsImpl::GetHistoryTagPoolString(
    /* [in] */ Int32 index,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = (*mReadHistoryStrings)[index];
    return NOERROR;
}

ECode BatteryStatsImpl::GetHistoryTagPoolUid(
    /* [in] */ Int32 index,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (*mReadHistoryUids)[index];
    return NOERROR;
}

ECode BatteryStatsImpl::GetNextHistoryLocked(
    /* [in] */ IBatteryStatsHistoryItem* _out,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<HistoryItem> out = (HistoryItem*)_out;

    Int32 pos;
    mHistoryBuffer->GetDataPosition(&pos);
    if (pos == 0) {
        out->Clear();
    }
    Int32 size;
    mHistoryBuffer->GetElementSize(&size);
    Boolean end = pos >= size;
    if (end) {
        *result = FALSE;
        return NOERROR;
    }

    Int64 lastRealtime = out->mTime;
    Int64 lastWalltime = out->mCurrentTime;
    ReadHistoryDelta(mHistoryBuffer, out);
    if (out->mCmd != IBatteryStatsHistoryItem::CMD_CURRENT_TIME
            && out->mCmd != IBatteryStatsHistoryItem::CMD_RESET && lastWalltime != 0) {
        out->mCurrentTime = lastWalltime + (out->mTime - lastRealtime);
    }
    *result = TRUE;
    return NOERROR;
}

ECode BatteryStatsImpl::FinishIteratingHistoryLocked()
{
    mIteratingHistory = FALSE;
    Int32 size;
    mHistoryBuffer->GetElementSize(&size);
    mHistoryBuffer->SetDataPosition(size);
    mReadHistoryStrings = NULL;
    return NOERROR;
}

ECode BatteryStatsImpl::GetHistoryBaseTime(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mHistoryBaseTime;
    return NOERROR;
}

ECode BatteryStatsImpl::GetStartCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count)
    *count = mStartCount;
    return NOERROR;
}

ECode BatteryStatsImpl::IsOnBattery(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOnBattery;
    return NOERROR;
}

ECode BatteryStatsImpl::IsScreenOn(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mScreenState == IDisplay::STATE_ON;
    return NOERROR;
}

void BatteryStatsImpl::InitTimes(
    /* [in] */ Int64 uptime,
    /* [in] */ Int64 realtime)
{
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetCurrentTimeMillis(&mStartClockTime);
    mOnBatteryTimeBase->Init(uptime, realtime);
    mOnBatteryScreenOffTimeBase->Init(uptime, realtime);
    mRealtime = 0;
    mUptime = 0;
    mRealtimeStart = realtime;
    mUptimeStart = uptime;
}

void BatteryStatsImpl::InitDischarge()
{
    mLowDischargeAmountSinceCharge = 0;
    mHighDischargeAmountSinceCharge = 0;
    mDischargeAmountScreenOn = 0;
    mDischargeAmountScreenOnSinceCharge = 0;
    mDischargeAmountScreenOff = 0;
    mDischargeAmountScreenOffSinceCharge = 0;
    mLastDischargeStepTime = -1;
    mNumDischargeStepDurations = 0;
    mLastChargeStepTime = -1;
    mNumChargeStepDurations = 0;
}

ECode BatteryStatsImpl::ResetAllStatsCmdLocked()
{
    ResetAllStatsLocked();
    Int64 mSecUptime = SystemClock::GetUptimeMillis();
    Int64 uptime = mSecUptime * 1000;
    Int64 mSecRealtime = SystemClock::GetElapsedRealtime();
    Int64 realtime = mSecRealtime * 1000;
    mDischargeStartLevel = mHistoryCur->mBatteryLevel;
    PullPendingStateUpdatesLocked();
    AddHistoryRecordLocked(mSecRealtime, mSecUptime);
    mDischargeCurrentLevel = mDischargeUnplugLevel = mDischargePlugLevel
            = mCurrentBatteryLevel = mHistoryCur->mBatteryLevel;
    mOnBatteryTimeBase->Reset(uptime, realtime);
    mOnBatteryScreenOffTimeBase->Reset(uptime, realtime);
    if ((mHistoryCur->mStates & IBatteryStatsHistoryItem::STATE_BATTERY_PLUGGED_FLAG) == 0) {
        if (mScreenState == IDisplay::STATE_ON) {
            mDischargeScreenOnUnplugLevel = mHistoryCur->mBatteryLevel;
            mDischargeScreenOffUnplugLevel = 0;
        }
        else {
            mDischargeScreenOnUnplugLevel = 0;
            mDischargeScreenOffUnplugLevel = mHistoryCur->mBatteryLevel;
        }
        mDischargeAmountScreenOn = 0;
        mDischargeAmountScreenOff = 0;
    }
    InitActiveHistoryEventsLocked(mSecRealtime, mSecUptime);
    return NOERROR;
}

void BatteryStatsImpl::ResetAllStatsLocked()
{
    mStartCount = 0;
    InitTimes(SystemClock::GetUptimeMillis() * 1000, SystemClock::GetElapsedRealtime() * 1000);
    mScreenOnTimer->Reset(FALSE);
    for (Int32 i = 0; i < IBatteryStats::NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        (*mScreenBrightnessTimer)[i]->Reset(FALSE);
    }
    mInteractiveTimer->Reset(FALSE);
    mLowPowerModeEnabledTimer->Reset(FALSE);
    mPhoneOnTimer->Reset(FALSE);
    mAudioOnTimer->Reset(FALSE);
    mVideoOnTimer->Reset(FALSE);
    mFlashlightOnTimer->Reset(FALSE);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        (*mPhoneSignalStrengthsTimer)[i]->Reset(FALSE);
    }
    mPhoneSignalScanningTimer->Reset(FALSE);
    for (Int32 i = 0; i < IBatteryStats::NUM_DATA_CONNECTION_TYPES; i++) {
        (*mPhoneDataConnectionsTimer)[i]->Reset(FALSE);
    }
    for (int i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
        (*mNetworkByteActivityCounters)[i]->Reset(FALSE);
        (*mNetworkPacketActivityCounters)[i]->Reset(FALSE);
    }
    mMobileRadioActiveTimer->Reset(FALSE);
    mMobileRadioActivePerAppTimer->Reset(FALSE);
    mMobileRadioActiveAdjustedTime->Reset(FALSE);
    mMobileRadioActiveUnknownTime->Reset(FALSE);
    mMobileRadioActiveUnknownCount->Reset(FALSE);
    mWifiOnTimer->Reset(FALSE);
    mGlobalWifiRunningTimer->Reset(FALSE);
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_STATES; i++) {
        (*mWifiStateTimer)[i]->Reset(FALSE);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SUPPL_STATES; i++) {
        (*mWifiSupplStateTimer)[i]->Reset(FALSE);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        (*mWifiSignalStrengthsTimer)[i]->Reset(FALSE);
    }
    mBluetoothOnTimer->Reset(FALSE);
    for (Int32 i = 0; i < IBatteryStats::NUM_BLUETOOTH_STATES; i++) {
        (*mBluetoothStateTimer)[i]->Reset(FALSE);
    }

    Int32 size;
    mUidStats->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> value;
        mUidStats->ValueAt(i, (IInterface**)&value);
        AutoPtr<Uid> uid = (Uid*)(IObject*)value.Get();
        if (uid->Reset()) {
            Int32 key;
            mUidStats->KeyAt(i, &key);
            mUidStats->Remove(key);
            i--;
            size--;
        }
    }
    if (mKernelWakelockStats->GetSize(&size), size > 0) {
        AutoPtr<ICollection> col;
        mKernelWakelockStats->GetValues((ICollection**)&col);
        AutoPtr<IIterator> it;
        col->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> value;
            it->GetNext((IInterface**)&value);
            AutoPtr<SamplingTimer> timer = (SamplingTimer*)(IObject*)value.Get();
            mOnBatteryScreenOffTimeBase->Remove((ITimeBaseObs*)timer);
        }
        mKernelWakelockStats->Clear();
    }

    if (mWakeupReasonStats->GetSize(&size), size > 0) {
        AutoPtr<ICollection> col;
        mWakeupReasonStats->GetValues((ICollection**)&col);
        AutoPtr<IIterator> it;
        col->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> value;
            it->GetNext((IInterface**)&value);
            AutoPtr<SamplingTimer> timer = (SamplingTimer*)(IObject*)value.Get();
            mOnBatteryTimeBase->Remove((ITimeBaseObs*)timer);
        }
        mWakeupReasonStats->Clear();
    }

    InitDischarge();

    ClearHistoryLocked();
}

void BatteryStatsImpl::InitActiveHistoryEventsLocked(
    /* [in] */ Int64 elapsedRealtimeMs,
    /* [in] */ Int64 uptimeMs)
{
    for (Int32 i = 0; i < IBatteryStatsHistoryItem::EVENT_COUNT; i++) {
        if (!mRecordAllHistory && i == IBatteryStatsHistoryItem::EVENT_PROC) {
            // Not recording process starts/stops.
            continue;
        }
        AutoPtr<IHashMap> active = mActiveEvents->GetStateForEvent(i);
        if (active == NULL) {
            continue;
        }
        AutoPtr<ISet> set;
        active->GetEntrySet((ISet**)&set);
        AutoPtr<IIterator> it;
        set->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            AutoPtr<IMapEntry> ent = IMapEntry::Probe(next);
            AutoPtr<IInterface> value;
            ent->GetValue((IInterface**)&value);
            AutoPtr<ISparseInt32Array> uids = ISparseInt32Array::Probe(value);
            Int32 size;
            uids->GetSize(&size);
            for (Int32 j = 0; j < size; j++) {
                AutoPtr<IInterface> ki;
                ent->GetKey((IInterface**)&ki);
                String key;
                ICharSequence::Probe(ki)->ToString(&key);
                Int32 uidKey;
                uids->KeyAt(j, &uidKey);
                AddHistoryEventLocked(elapsedRealtimeMs, uptimeMs, i, key, uidKey);
            }
        }
    }
}

void BatteryStatsImpl::UpdateDischargeScreenLevelsLocked(
    /* [in] */ Boolean oldScreenOn,
    /* [in] */ Boolean newScreenOn)
{
    if (oldScreenOn) {
        Int32 diff = mDischargeScreenOnUnplugLevel - mDischargeCurrentLevel;
        if (diff > 0) {
            mDischargeAmountScreenOn += diff;
            mDischargeAmountScreenOnSinceCharge += diff;
        }
    }
    else {
        Int32 diff = mDischargeScreenOffUnplugLevel - mDischargeCurrentLevel;
        if (diff > 0) {
            mDischargeAmountScreenOff += diff;
            mDischargeAmountScreenOffSinceCharge += diff;
        }
    }
    if (newScreenOn) {
        mDischargeScreenOnUnplugLevel = mDischargeCurrentLevel;
        mDischargeScreenOffUnplugLevel = 0;
    }
    else {
        mDischargeScreenOnUnplugLevel = 0;
        mDischargeScreenOffUnplugLevel = mDischargeCurrentLevel;
    }
}

void BatteryStatsImpl::PullPendingStateUpdatesLocked()
{
    UpdateKernelWakelocksLocked();
    UpdateNetworkActivityLocked(NET_UPDATE_ALL, SystemClock::GetElapsedRealtime());
    if (mOnBatteryInternal) {
        Boolean screenOn = mScreenState == IDisplay::STATE_ON;
        UpdateDischargeScreenLevelsLocked(screenOn, screenOn);
    }
}

void BatteryStatsImpl::SetOnBatteryLocked(
    /* [in] */ Int64 mSecRealtime,
    /* [in] */ Int64 mSecUptime,
    /* [in] */ Boolean onBattery,
    /* [in] */ Int32 oldStatus,
    /* [in] */ Int32 level)
{
    Boolean doWrite = FALSE;
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MSG_REPORT_POWER_CHANGE, (IMessage**)&m);
    m->SetArg1(onBattery ? 1 : 0);
    Boolean result;
    mHandler->SendMessage(m, &result);

    Int64 uptime = mSecUptime * 1000;
    Int64 realtime = mSecRealtime * 1000;
    Boolean screenOn = mScreenState == IDisplay::STATE_ON;
    if (onBattery) {
        // We will reset our status if we are unplugging after the
        // battery was last full, or the level is at 100, or
        // we have gone through a significant charge (from a very low
        // level to a now very high level).
        Boolean reset = FALSE;
        Int32 high, eleSize;
        if (!mNoAutoReset && (oldStatus == IBatteryManager::BATTERY_STATUS_FULL
            || level >= 90
            || (mDischargeCurrentLevel < 20 && level >= 80)
            || ((GetHighDischargeAmountSinceCharge(&high), high >= 200)
                && (mHistoryBuffer->GetElementSize(&eleSize), eleSize >= MAX_HISTORY_BUFFER)))) {
            Int32 low;
            GetLowDischargeAmountSinceCharge(&low);
            Slogger::I(TAG, "Resetting battery stats: level=%d status=%d dischargeLevel=%d lowAmount=%d highAmount=%d",
                level, oldStatus, mDischargeCurrentLevel, low, high);
            // Before we write, collect a snapshot of the final aggregated
            // stats to be reported in the next checkin.  Only do this if we have
            // a sufficient amount of data to make it interesting.
            if (low >= 20) {
                AutoPtr<IParcel> parcel;// = Parcel.obtain();
                CParcel::New((IParcel**)&parcel);
                WriteSummaryToParcel(parcel, TRUE);
                AutoPtr<IBackgroundThreadHelper> helper;
                CBackgroundThreadHelper::AcquireSingleton((IBackgroundThreadHelper**)&helper);
                AutoPtr<IHandler> h;
                helper->GetHandler((IHandler**)&h);
                AutoPtr<IRunnable> runnable = new SetOnBatteryRunnable(parcel, this);

                Boolean result;
                h->Post(runnable, &result);
            }
            doWrite = TRUE;
            ResetAllStatsLocked();
            mDischargeStartLevel = level;
            reset = TRUE;
            mNumDischargeStepDurations = 0;
        }
        mOnBattery = mOnBatteryInternal = onBattery;
        mLastDischargeStepLevel = level;
        mMinDischargeStepLevel = level;
        mLastDischargeStepTime = -1;
        mInitStepMode = mCurStepMode;
        mModStepMode = 0;
        PullPendingStateUpdatesLocked();
        mHistoryCur->mBatteryLevel = (Byte)level;
        mHistoryCur->mStates &= ~IBatteryStatsHistoryItem::STATE_BATTERY_PLUGGED_FLAG;
        if (DEBUG_HISTORY)
            Slogger::V(TAG, "Battery unplugged to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        if (reset) {
            mRecordingHistory = TRUE;
            StartRecordingHistory(mSecRealtime, mSecUptime, reset);
        }
        AddHistoryRecordLocked(mSecRealtime, mSecUptime);
        mDischargeCurrentLevel = mDischargeUnplugLevel = level;
        if (screenOn) {
            mDischargeScreenOnUnplugLevel = level;
            mDischargeScreenOffUnplugLevel = 0;
        }
        else {
            mDischargeScreenOnUnplugLevel = 0;
            mDischargeScreenOffUnplugLevel = level;
        }
        mDischargeAmountScreenOn = 0;
        mDischargeAmountScreenOff = 0;
        UpdateTimeBasesLocked(TRUE, !screenOn, uptime, realtime);
    }
    else {
        mOnBattery = mOnBatteryInternal = onBattery;
        PullPendingStateUpdatesLocked();
        mHistoryCur->mBatteryLevel = (byte)level;
        mHistoryCur->mStates |= IBatteryStatsHistoryItem::STATE_BATTERY_PLUGGED_FLAG;
        if (DEBUG_HISTORY)
            Slogger::V(TAG, "Battery plugged to: %s", StringUtils::ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(mSecRealtime, mSecUptime);
        mDischargeCurrentLevel = mDischargePlugLevel = level;
        if (level < mDischargeUnplugLevel) {
            mLowDischargeAmountSinceCharge += mDischargeUnplugLevel - level - 1;
            mHighDischargeAmountSinceCharge += mDischargeUnplugLevel - level;
        }
        UpdateDischargeScreenLevelsLocked(screenOn, screenOn);
        UpdateTimeBasesLocked(FALSE, !screenOn, uptime, realtime);
        mNumChargeStepDurations = 0;
        mLastChargeStepLevel = level;
        mMaxChargeStepLevel = level;
        mLastChargeStepTime = -1;
        mInitStepMode = mCurStepMode;
        mModStepMode = 0;
    }
    if (doWrite || (mLastWriteTime + (60 * 1000)) < mSecRealtime) {
        if (mFile != NULL) {
            WriteAsyncLocked();
        }
    }
}

void BatteryStatsImpl::StartRecordingHistory(
    /* [in] */ Int64 elapsedRealtimeMs,
    /* [in] */ Int64 uptimeMs,
    /* [in] */ Boolean reset)
{
    mRecordingHistory = TRUE;
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetCurrentTimeMillis(&mHistoryCur->mCurrentTime);
    mLastRecordedClockTime = mHistoryCur->mCurrentTime;
    mLastRecordedClockRealtime = elapsedRealtimeMs;
    AddHistoryBufferLocked(elapsedRealtimeMs, uptimeMs,
            reset ? IBatteryStatsHistoryItem::CMD_RESET : IBatteryStatsHistoryItem::CMD_CURRENT_TIME,
            mHistoryCur);
    mHistoryCur->mCurrentTime = 0;
    if (reset) {
        InitActiveHistoryEventsLocked(elapsedRealtimeMs, uptimeMs);
    }
}

void BatteryStatsImpl::RecordCurrentTimeChangeLocked(
    /* [in] */ Int64 currentTime,
    /* [in] */ Int64 elapsedRealtimeMs,
    /* [in] */ Int64 uptimeMs)
{
    if (mRecordingHistory) {
        mHistoryCur->mCurrentTime = currentTime;
        mLastRecordedClockTime = currentTime;
        mLastRecordedClockRealtime = elapsedRealtimeMs;
        AddHistoryBufferLocked(elapsedRealtimeMs, uptimeMs, IBatteryStatsHistoryItem::CMD_CURRENT_TIME,
                mHistoryCur);
        mHistoryCur->mCurrentTime = 0;
    }
}

Int32 BatteryStatsImpl::AddLevelSteps(
    /* [in] */ ArrayOf<Int64>* steps,
    /* [in] */ Int32 stepCount,
    /* [in] */ Int64 lastStepTime,
    /* [in] */ Int32 numStepLevels,
    /* [in] */ Int64 modeBits,
    /* [in] */ Int64 elapsedRealtime)
{
    if (lastStepTime >= 0 && numStepLevels > 0) {
        Int64 duration = elapsedRealtime - lastStepTime;
        for (Int32 i = 0; i < numStepLevels; i++) {
            steps->Copy(1, steps, 0, steps->GetLength() - 1);
            Int64 thisDuration = duration / (numStepLevels-i);
            duration -= thisDuration;
            if (thisDuration > IBatteryStats::STEP_LEVEL_TIME_MASK) {
                thisDuration = IBatteryStats::STEP_LEVEL_TIME_MASK;
            }
            (*steps)[0] = thisDuration | modeBits;
        }
        stepCount += numStepLevels;
        if (stepCount > steps->GetLength()) {
            stepCount = steps->GetLength();
        }
    }
    return stepCount;
}

ECode BatteryStatsImpl::SetBatteryState(
    /* [in] */ Int32 status,
    /* [in] */ Int32 health,
    /* [in] */ Int32 plugType,
    /* [in] */ Int32 level,
    /* [in] */ Int32 temp,
    /* [in] */ Int32 volt)
{
    AutoLock lock(this);
    Boolean onBattery = plugType == BATTERY_PLUGGED_NONE;
    Int64 uptime = SystemClock::GetUptimeMillis();
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
    Int32 oldStatus = mHistoryCur->mBatteryStatus;
    if (!mHaveBatteryLevel) {
        mHaveBatteryLevel = TRUE;
        // We start out assuming that the device is plugged in (not
        // on battery).  If our first report is now that we are indeed
        // plugged in, then twiddle our state to correctly reflect that
        // since we won't be going through the full setOnBattery().
        if (onBattery == mOnBattery) {
            if (onBattery) {
                mHistoryCur->mStates &= ~HistoryItem::STATE_BATTERY_PLUGGED_FLAG;
            }
            else {
                mHistoryCur->mStates |= HistoryItem::STATE_BATTERY_PLUGGED_FLAG;
            }
        }
        oldStatus = status;
    }
    if (onBattery) {
        mDischargeCurrentLevel = level;
        if (!mRecordingHistory) {
            mRecordingHistory = TRUE;
            StartRecordingHistory(elapsedRealtime, uptime, TRUE);
        }
    }
    else if (level < 96) {
        if (!mRecordingHistory) {
            mRecordingHistory = TRUE;
            StartRecordingHistory(elapsedRealtime, uptime, TRUE);
        }
    }
    mCurrentBatteryLevel = level;
    if (mDischargePlugLevel < 0) {
        mDischargePlugLevel = level;
    }
    if (onBattery != mOnBattery) {
        mHistoryCur->mBatteryLevel = (Byte)level;
        mHistoryCur->mBatteryStatus = (Byte)status;
        mHistoryCur->mBatteryHealth = (Byte)health;
        mHistoryCur->mBatteryPlugType = (Byte)plugType;
        mHistoryCur->mBatteryTemperature = (Int16)temp;
        mHistoryCur->mBatteryVoltage = (Char32)volt;
        SetOnBatteryLocked(elapsedRealtime, uptime, onBattery, oldStatus, level);
    }
    else {
        Boolean changed = FALSE;
        if (mHistoryCur->mBatteryLevel != level) {
            mHistoryCur->mBatteryLevel = (Byte)level;
            changed = TRUE;
        }
        if (mHistoryCur->mBatteryStatus != status) {
            mHistoryCur->mBatteryStatus = (Byte)status;
            changed = TRUE;
        }
        if (mHistoryCur->mBatteryHealth != health) {
            mHistoryCur->mBatteryHealth = (Byte)health;
            changed = TRUE;
        }
        if (mHistoryCur->mBatteryPlugType != plugType) {
            mHistoryCur->mBatteryPlugType = (Byte)plugType;
            changed = TRUE;
        }
        if (temp >= (mHistoryCur->mBatteryTemperature + 10)
                || temp <= (mHistoryCur->mBatteryTemperature - 10)) {
            mHistoryCur->mBatteryTemperature = (Int16)temp;
            changed = TRUE;
        }
        if (volt > (Int32)(mHistoryCur->mBatteryVoltage + 20)
                || volt < (Int32)(mHistoryCur->mBatteryVoltage - 20)) {
            mHistoryCur->mBatteryVoltage = (Char32)volt;
            changed = TRUE;
        }
        if (changed) {
            AddHistoryRecordLocked(elapsedRealtime, uptime);
        }
        Int64 modeBits = (((Int64)mInitStepMode) << STEP_LEVEL_INITIAL_MODE_SHIFT)
                | (((Int64)mModStepMode) << STEP_LEVEL_MODIFIED_MODE_SHIFT)
                | (((Int64)(level & 0xff)) << STEP_LEVEL_LEVEL_SHIFT);
        if (onBattery) {
            if (mLastDischargeStepLevel != level && mMinDischargeStepLevel > level) {
                mNumDischargeStepDurations = AddLevelSteps(mDischargeStepDurations,
                        mNumDischargeStepDurations, mLastDischargeStepTime,
                        mLastDischargeStepLevel - level, modeBits, elapsedRealtime);
                mLastDischargeStepLevel = level;
                mMinDischargeStepLevel = level;
                mLastDischargeStepTime = elapsedRealtime;
                mInitStepMode = mCurStepMode;
                mModStepMode = 0;
            }
        }
        else {
            if (mLastChargeStepLevel != level && mMaxChargeStepLevel < level) {
                mNumChargeStepDurations = AddLevelSteps(mChargeStepDurations,
                        mNumChargeStepDurations, mLastChargeStepTime,
                        level - mLastChargeStepLevel, modeBits, elapsedRealtime);
                mLastChargeStepLevel = level;
                mMaxChargeStepLevel = level;
                mLastChargeStepTime = elapsedRealtime;
                mInitStepMode = mCurStepMode;
                mModStepMode = 0;
            }
        }
    }
    if (!onBattery && status == IBatteryManager::BATTERY_STATUS_FULL) {
        // We don't record history while we are plugged in and fully charged.
        // The next time we are unplugged, history will be cleared.
        mRecordingHistory = DEBUG;
    }
    return NOERROR;
}

void BatteryStatsImpl::UpdateKernelWakelocksLocked()
{
    HashMap<String, AutoPtr<KernelWakelockStats> >* m = ReadKernelWakelockStats();

    if (m == NULL) {
        // Not crashing might make board bringup easier.
        Slogger::W(TAG, "Couldn't get kernel wake lock stats");
        return;
    }

    HashMap<String, AutoPtr<KernelWakelockStats> >::Iterator it = m->Begin();
    for (; it != m->End(); ++it) {
        String name = it->mFirst;
        AutoPtr<KernelWakelockStats> kws = it->mSecond;

        AutoPtr<ICharSequence> cs;
        CString::New(name, (ICharSequence**)&cs);
        AutoPtr<IInterface> value;
        mKernelWakelockStats->Get(cs, (IInterface**)&value);
        AutoPtr<SamplingTimer> kwlt;
        if (value == NULL) {
            kwlt = new SamplingTimer(mOnBatteryScreenOffTimeBase,
                    TRUE /* track reported val */);
            mKernelWakelockStats->Put(cs, (IObject*)kwlt);
        }
        else {
            kwlt = (SamplingTimer*)(IObject*)value.Get();
        }
        kwlt->UpdateCurrentReportedCount(kws->mCount);
        kwlt->UpdateCurrentReportedTotalTime(kws->mTotalTime);
        kwlt->SetUpdateVersion(sKernelWakelockUpdateVersion);
    }

    Int32 size;
    if (mKernelWakelockStats->GetSize(&size), (Int32)m->GetSize() != size) {
        // Set timers to stale if they didn't appear in /proc/wakelocks this time.
        AutoPtr<ISet> set;
        mKernelWakelockStats->GetEntrySet((ISet**)&set);
        AutoPtr<IIterator> it;
        set->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            AutoPtr<IMapEntry> ent = IMapEntry::Probe(next);
            AutoPtr<IInterface> value;
            ent->GetValue((IInterface**)&value);
            AutoPtr<SamplingTimer> st = (SamplingTimer*)(IObject*)value.Get();
            if (st->GetUpdateVersion() != sKernelWakelockUpdateVersion) {
                st->SetStale();
            }
        }
    }
}

void BatteryStatsImpl::UpdateNetworkActivityLocked(
    /* [in] */ Int32 which,
    /* [in] */ Int64 elapsedRealtimeMs)
{
    AutoPtr<ISystemProperties> props;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&props);
    Boolean value;
    if (props->GetBoolean(String("net.qtaguid_enabled")/*INetworkManagementSocketTagger::PROP_QTAGUID_ENABLED*/,
            FALSE, &value), !value) return;

    if ((which & NET_UPDATE_MOBILE) != 0 && mMobileIfaces->GetLength() > 0) {
        AutoPtr<INetworkStats> snapshot;
        AutoPtr<INetworkStats> last = mCurMobileSnapshot;
        // try {
        ECode ec = mNetworkStatsFactory->ReadNetworkStatsDetail(INetworkStats::UID_ALL,
                    mMobileIfaces, INetworkStats::TAG_NONE, mLastMobileSnapshot, (INetworkStats**)&snapshot);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Failed to read mobile network stats");
            return;
        }
        // } catch (IOException e) {
        //     Log.wtf(TAG, "Failed to read mobile network stats", e);
        //     return;
        // }

        mCurMobileSnapshot = snapshot;
        mLastMobileSnapshot = last;

        if (mOnBatteryInternal) {
            AutoPtr<INetworkStatsHelper> helper;
            CNetworkStatsHelper::AcquireSingleton((INetworkStatsHelper**)&helper);
            AutoPtr<INetworkStats> delta;
            helper->Subtract(snapshot, last, NULL, NULL, mTmpNetworkStats, (INetworkStats**)&delta);
            mTmpNetworkStats = delta;

            Int64 radioTime = mMobileRadioActivePerAppTimer->CheckpointRunningLocked(
                    elapsedRealtimeMs);
            Int64 totalPackets;
            delta->GetTotalPackets(&totalPackets);

            Int32 size;
            delta->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<INetworkStatsEntry> entry;
                delta->GetValues(i, mTmpNetworkStatsEntry, (INetworkStatsEntry**)&entry);

                Int64 rxBytes, txBytes;
                entry->GetRxBytes(&rxBytes);
                entry->GetTxBytes(&txBytes);
                if (rxBytes == 0 || txBytes == 0) continue;

                Int32 uid;
                entry->GetUid(&uid);
                Int32 newUid;
                MapUid(uid, &newUid);
                AutoPtr<Uid> u = GetUidStatsLocked(newUid);
                Int64 rxPackets, txPackets;
                entry->GetRxPackets(&rxPackets);
                entry->GetTxPackets(&txPackets);
                u->NoteNetworkActivityLocked(IBatteryStats::NETWORK_MOBILE_RX_DATA, rxBytes, rxPackets);
                u->NoteNetworkActivityLocked(IBatteryStats::NETWORK_MOBILE_TX_DATA, txBytes, txPackets);

                if (radioTime > 0) {
                    // Distribute total radio active time in to this app.
                    Int64 appPackets = rxPackets + txPackets;
                    Int64 appRadioTime = (radioTime * appPackets) / totalPackets;
                    u->NoteMobileRadioActiveTimeLocked(appRadioTime);
                    // Remove this app from the totals, so that we don't lose any time
                    // due to rounding.
                    radioTime -= appRadioTime;
                    totalPackets -= appPackets;
                }

                (*mNetworkByteActivityCounters)[IBatteryStats::NETWORK_MOBILE_RX_DATA]->AddCountLocked(
                        rxBytes);
                (*mNetworkByteActivityCounters)[IBatteryStats::NETWORK_MOBILE_TX_DATA]->AddCountLocked(
                        txBytes);
                (*mNetworkPacketActivityCounters)[IBatteryStats::NETWORK_MOBILE_RX_DATA]->AddCountLocked(
                        rxPackets);
                (*mNetworkPacketActivityCounters)[IBatteryStats::NETWORK_MOBILE_TX_DATA]->AddCountLocked(
                        txPackets);
            }

            if (radioTime > 0) {
                // Whoops, there is some radio time we can't blame on an app!
                mMobileRadioActiveUnknownTime->AddCountLocked(radioTime);
                mMobileRadioActiveUnknownCount->AddCountLocked(1);
            }
        }
    }

    if ((which & NET_UPDATE_WIFI) != 0 && mWifiIfaces->GetLength() > 0) {
        AutoPtr<INetworkStats> snapshot;
        AutoPtr<INetworkStats> last = mCurWifiSnapshot;
        // try {
        ECode ec = mNetworkStatsFactory->ReadNetworkStatsDetail(INetworkStats::UID_ALL,
                mWifiIfaces, INetworkStats::TAG_NONE, mLastWifiSnapshot, (INetworkStats**)&snapshot);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Logger::E(TAG, "Failed to read wifi network stats");
            return;
        }
        // } catch (IOException e) {
        //     Log.wtf(TAG, "Failed to read wifi network stats", e);
        //     return;
        // }

        mCurWifiSnapshot = snapshot;
        mLastWifiSnapshot = last;

        if (mOnBatteryInternal) {
            AutoPtr<INetworkStats> delta;
            AutoPtr<INetworkStatsHelper> helper;
            CNetworkStatsHelper::AcquireSingleton((INetworkStatsHelper**)&helper);
            helper->Subtract(snapshot, last, NULL, NULL, mTmpNetworkStats, (INetworkStats**)&delta);
            mTmpNetworkStats = delta;

            Int32 size;
            delta->GetSize(&size);
            for (Int32 i = 0; i < size; i++) {
                AutoPtr<INetworkStatsEntry> entry;
                delta->GetValues(i, mTmpNetworkStatsEntry, (INetworkStatsEntry**)&entry);

                if (DEBUG) {
                    AutoPtr<INetworkStatsEntry> cur;
                    snapshot->GetValues(i, NULL, (INetworkStatsEntry**)&cur);
                    Int32 uid;
                    cur->GetUid(&uid);
                    Int64 entryRxBytes, entryTxBytes, curRxBytes, curTxBytes;
                    entry->GetRxBytes(&entryRxBytes);
                    entry->GetTxBytes(&entryTxBytes);
                    cur->GetRxBytes(&curRxBytes);
                    cur->GetTxBytes(&curRxBytes);
                    Slogger::D(TAG, "Wifi uid %d: delta rx=%d tx=%d, cur rx=%d tx=%d", uid,
                            entryRxBytes, entryTxBytes, curRxBytes, curTxBytes);
                }

                Int64 rxBytes, txBytes;
                entry->GetRxBytes(&rxBytes);
                entry->GetTxBytes(&txBytes);
                if (rxBytes == 0 || txBytes == 0) continue;

                Int32 uid;
                entry->GetUid(&uid);
                MapUid(uid, &uid);
                AutoPtr<Uid> u = GetUidStatsLocked(uid);
                Int64 rxPackets, txPackets;
                entry->GetRxPackets(&rxPackets);
                entry->GetTxPackets(&txPackets);
                u->NoteNetworkActivityLocked(IBatteryStats::NETWORK_WIFI_RX_DATA, rxBytes,
                        rxPackets);
                u->NoteNetworkActivityLocked(IBatteryStats::NETWORK_WIFI_TX_DATA, txBytes,
                        txPackets);

                (*mNetworkByteActivityCounters)[IBatteryStats::NETWORK_WIFI_RX_DATA]->AddCountLocked(rxBytes);
                (*mNetworkByteActivityCounters)[IBatteryStats::NETWORK_WIFI_TX_DATA]->AddCountLocked(txBytes);
                (*mNetworkPacketActivityCounters)[IBatteryStats::NETWORK_WIFI_RX_DATA]->AddCountLocked(rxPackets);
                (*mNetworkPacketActivityCounters)[IBatteryStats::NETWORK_WIFI_TX_DATA]->AddCountLocked(txPackets);
            }
        }
    }
}

ECode BatteryStatsImpl::GetAwakeTimeBattery(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return ComputeBatteryUptime(GetBatteryUptimeLocked(), IBatteryStats::STATS_CURRENT, result);
}

ECode BatteryStatsImpl::GetAwakeTimePlugged(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 time;
    GetAwakeTimeBattery(&time);
    *result = (SystemClock::GetUptimeMillis() * 1000) - time;
    return NOERROR;
}

ECode BatteryStatsImpl::ComputeUptime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    switch (which) {
        case STATS_SINCE_CHARGED: *value = mUptime + (curTime - mUptimeStart);break;
        case STATS_CURRENT: *value = curTime - mUptimeStart;break;
        case STATS_SINCE_UNPLUGGED: *value = curTime - mOnBatteryTimeBase->GetUptimeStart();break;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::ComputeRealtime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = 0;
    switch (which) {
        case STATS_SINCE_CHARGED: *value = mRealtime + (curTime-mRealtimeStart);break;
        case STATS_CURRENT: *value = (curTime - mRealtimeStart);break;
        case STATS_SINCE_UNPLUGGED: *value = (curTime - mOnBatteryTimeBase->GetRealtimeStart());break;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::ComputeBatteryUptime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mOnBatteryTimeBase->ComputeUptime(curTime, which);
    return NOERROR;
}

ECode BatteryStatsImpl::ComputeBatteryRealtime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which,
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mOnBatteryTimeBase->ComputeRealtime(curTime, which);
    return NOERROR;
}

ECode BatteryStatsImpl::ComputeBatteryScreenOffUptime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOnBatteryScreenOffTimeBase->ComputeUptime(curTime, which);
    return NOERROR;
}

ECode BatteryStatsImpl::ComputeBatteryScreenOffRealtime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOnBatteryScreenOffTimeBase->ComputeRealtime(curTime, which);
    return NOERROR;
}

Int64 BatteryStatsImpl::ComputeTimePerLevel(
    /* [in] */ ArrayOf<Int64>* steps,
    /* [in] */ Int32 numSteps)
{
    // For now we'll do a simple average across all steps.
    if (numSteps <= 0) {
        return -1;
    }
    Int64 total = 0;
    for (Int32 i = 0; i < numSteps; i++) {
        total += (*steps)[i] & IBatteryStats::STEP_LEVEL_TIME_MASK;
    }
    return total / numSteps;
    /*
    long[] buckets = new long[numSteps];
    int numBuckets = 0;
    int numToAverage = 4;
    int i = 0;
    while (i < numSteps) {
        long totalTime = 0;
        int num = 0;
        for (int j=0; j<numToAverage && (i+j)<numSteps; j++) {
            totalTime += steps[i+j] & STEP_LEVEL_TIME_MASK;
            num++;
        }
        buckets[numBuckets] = totalTime / num;
        numBuckets++;
        numToAverage *= 2;
        i += num;
    }
    if (numBuckets < 1) {
        return -1;
    }
    long averageTime = buckets[numBuckets-1];
    for (i=numBuckets-2; i>=0; i--) {
        averageTime = (averageTime + buckets[i]) / 2;
    }
    return averageTime;
    */
}

ECode BatteryStatsImpl::ComputeBatteryTimeRemaining(
    /* [in] */ Int64 curTime,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;
    if (!mOnBattery) {
        return NOERROR;
    }
    /* Simple implementation just looks at the average discharge per level across the
       entire sample period.
    int discharge = (getLowDischargeAmountSinceCharge()+getHighDischargeAmountSinceCharge())/2;
    if (discharge < 2) {
        return -1;
    }
    long duration = computeBatteryRealtime(curTime, STATS_SINCE_CHARGED);
    if (duration < 1000*1000) {
        return -1;
    }
    long usPerLevel = duration/discharge;
    return usPerLevel * mCurrentBatteryLevel;
    */
    if (mNumDischargeStepDurations < 1) {
        return NOERROR;
    }
    Int64 msPerLevel = ComputeTimePerLevel(mDischargeStepDurations, mNumDischargeStepDurations);
    if (msPerLevel <= 0) {
        return NOERROR;
    }
    *result = (msPerLevel * mCurrentBatteryLevel) * 1000;
    return NOERROR;
}

ECode BatteryStatsImpl::GetNumDischargeStepDurations(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNumDischargeStepDurations;
    return NOERROR;
}

ECode BatteryStatsImpl::GetDischargeStepDurationsArray(
    /* [out, callee] */ ArrayOf<Int64>** array)
{
    VALIDATE_NOT_NULL(array)
    *array = mDischargeStepDurations;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

ECode BatteryStatsImpl::ComputeChargeTimeRemaining(
    /* [in] */ Int64 curTime,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;

    if (mOnBattery) {
        // Not yet working.
        return NOERROR;
    }
    /* Broken
    int curLevel = mCurrentBatteryLevel;
    int plugLevel = mDischargePlugLevel;
    if (plugLevel < 0 || curLevel < (plugLevel+1)) {
        return -1;
    }
    long duration = computeBatteryRealtime(curTime, STATS_SINCE_UNPLUGGED);
    if (duration < 1000*1000) {
        return -1;
    }
    long usPerLevel = duration/(curLevel-plugLevel);
    return usPerLevel * (100-curLevel);
    */
    if (mNumChargeStepDurations < 1) {
        return NOERROR;
    }
    Int64 msPerLevel = ComputeTimePerLevel(mChargeStepDurations, mNumChargeStepDurations);
    if (msPerLevel <= 0) {
        return NOERROR;
    }
    *result = (msPerLevel * (100 - mCurrentBatteryLevel)) * 1000;
    return NOERROR;
}

ECode BatteryStatsImpl::GetNumChargeStepDurations(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mNumChargeStepDurations;
    return NOERROR;
}

ECode BatteryStatsImpl::GetChargeStepDurationsArray(
    /* [out, callee] */ ArrayOf<Int64>** array)
{
    VALIDATE_NOT_NULL(array)
    *array = mChargeStepDurations;
    REFCOUNT_ADD(*array)
    return NOERROR;
}

Int64 BatteryStatsImpl::GetBatteryUptimeLocked()
{
    return mOnBatteryTimeBase->GetUptime(SystemClock::GetUptimeMillis() * 1000);
}

ECode BatteryStatsImpl::GetBatteryUptime(
    /* [in] */ Int64 curTime,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOnBatteryTimeBase->GetUptime(curTime);
    return NOERROR;
}

ECode BatteryStatsImpl::GetBatteryRealtime(
    /* [in] */ Int64 curTime,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mOnBatteryTimeBase->GetRealtime(curTime);
    return NOERROR;
}

ECode BatteryStatsImpl::GetDischargeStartLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level)
    AutoLock lock(this);
    *level = GetDischargeStartLevelLocked();
    return NOERROR;
}

Int32 BatteryStatsImpl::GetDischargeStartLevelLocked()
{
    return mDischargeUnplugLevel;
}

ECode BatteryStatsImpl::GetDischargeCurrentLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level)
    AutoLock lock(this);
    *level = GetDischargeCurrentLevelLocked();
    return NOERROR;
}

Int32 BatteryStatsImpl::GetDischargeCurrentLevelLocked()
{
    return mDischargeCurrentLevel;
}

ECode BatteryStatsImpl::GetLowDischargeAmountSinceCharge(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(this);
    Int32 val = mLowDischargeAmountSinceCharge;
    if (mOnBattery && mDischargeCurrentLevel < mDischargeUnplugLevel) {
        val += mDischargeUnplugLevel - mDischargeCurrentLevel-1;
    }
    *result = val;
    return NOERROR;
}

ECode BatteryStatsImpl::GetHighDischargeAmountSinceCharge(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(this);
    Int32 val = mHighDischargeAmountSinceCharge;
    if (mOnBattery && mDischargeCurrentLevel < mDischargeUnplugLevel) {
        val += mDischargeUnplugLevel - mDischargeCurrentLevel;
    }
    *result = val;
    return NOERROR;
}

ECode BatteryStatsImpl::GetDischargeAmount(
    /* [in] */ Int32 which,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 dischargeAmount;
    if (which == IBatteryStats::STATS_SINCE_CHARGED) {
        GetHighDischargeAmountSinceCharge(&dischargeAmount);
    }
    else {
        Int32 startLevel, curLevel;
        GetDischargeStartLevel(&startLevel);
        GetDischargeCurrentLevel(&curLevel);
        dischargeAmount = startLevel - curLevel;
    }
    if (dischargeAmount < 0) {
        dischargeAmount = 0;
    }
    *result = dischargeAmount;
    return NOERROR;
}

ECode BatteryStatsImpl::GetDischargeAmountScreenOn(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(this);
    int val = mDischargeAmountScreenOn;
    if (mOnBattery && mScreenState == IDisplay::STATE_ON
            && mDischargeCurrentLevel < mDischargeScreenOnUnplugLevel) {
        val += mDischargeScreenOnUnplugLevel-mDischargeCurrentLevel;
    }
    *result = val;
    return NOERROR;
}

ECode BatteryStatsImpl::GetDischargeAmountScreenOnSinceCharge(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(this);
    Int32 val = mDischargeAmountScreenOnSinceCharge;
    if (mOnBattery && mScreenState == IDisplay::STATE_ON
            && mDischargeCurrentLevel < mDischargeScreenOnUnplugLevel) {
        val += mDischargeScreenOnUnplugLevel - mDischargeCurrentLevel;
    }
    *result = val;
    return NOERROR;
}

ECode BatteryStatsImpl::GetDischargeAmountScreenOff(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(this);
    Int32 val = mDischargeAmountScreenOff;
    if (mOnBattery && mScreenState != IDisplay::STATE_ON
            && mDischargeCurrentLevel < mDischargeScreenOffUnplugLevel) {
        val += mDischargeScreenOffUnplugLevel - mDischargeCurrentLevel;
    }
    *result = val;
    return NOERROR;
}

ECode BatteryStatsImpl::GetDischargeAmountScreenOffSinceCharge(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    AutoLock lock(this);
    Int32 val = mDischargeAmountScreenOffSinceCharge;
    if (mOnBattery && mScreenState != IDisplay::STATE_ON
            && mDischargeCurrentLevel < mDischargeScreenOffUnplugLevel) {
        val += mDischargeScreenOffUnplugLevel - mDischargeCurrentLevel;
    }
    *result = val;
    return NOERROR;
}

ECode BatteryStatsImpl::GetCpuSpeedSteps(
    /* [out] */ Int32* steps)
{
    VALIDATE_NOT_NULL(steps)
    *steps = sNumSpeedSteps;
    return NOERROR;
}

AutoPtr<BatteryStatsImpl::Uid> BatteryStatsImpl::GetUidStatsLocked(
    /* [in] */ Int32 uid)
{
    AutoPtr<IInterface> value;
    mUidStats->Get(uid, (IInterface**)&value);
    AutoPtr<Uid> u;
    if (value == NULL) {
        u = new Uid(uid, this);
        mUidStats->Put(uid, (IObject*)u);
    }
    else {
        u = (Uid*)(IObject*)value.Get();
    }
    return u;
}

ECode BatteryStatsImpl::RemoveUidStatsLocked(
    /* [in] */ Int32 uid)
{
    mUidStats->Remove(uid);
    return NOERROR;
}

ECode BatteryStatsImpl::GetProcessStatsLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& name,
    /* [out] */ IBatteryStatsImplUidProc** proc)
{
    VALIDATE_NOT_NULL(proc)
    MapUid(uid, &uid);
    AutoPtr<Uid> u = GetUidStatsLocked(uid);
    AutoPtr<IBatteryStatsImplUidProc> bsiup = (IBatteryStatsImplUidProc*)u->GetProcessStatsLocked(name);
    *proc = bsiup;
    REFCOUNT_ADD(*proc)
    return NOERROR;
}

ECode BatteryStatsImpl::GetPackageStatsLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& pkg,
    /* [out] */ IBatteryStatsImplUidPkg** pkgStats)
{
    VALIDATE_NOT_NULL(pkgStats)
    MapUid(uid, &uid);
    AutoPtr<Uid> u = GetUidStatsLocked(uid);
    AutoPtr<IBatteryStatsImplUidPkg> bsiup = (IBatteryStatsImplUidPkg*)u->GetPackageStatsLocked(pkg);
    *pkgStats = bsiup;
    REFCOUNT_ADD(*pkgStats)
    return NOERROR;
}

ECode BatteryStatsImpl::GetServiceStatsLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& pkg,
    /* [in] */ const String& name,
    /* [out] */ IBatteryStatsImplUidPkgServ** serv)
{
    VALIDATE_NOT_NULL(serv)
    MapUid(uid, &uid);
    AutoPtr<Uid> u = GetUidStatsLocked(uid);
    AutoPtr<IBatteryStatsImplUidPkgServ> bsiups = (IBatteryStatsImplUidPkgServ*)u->GetServiceStatsLocked(pkg, name);
    *serv = bsiups;
    REFCOUNT_ADD(*serv)
    return NOERROR;
}

void BatteryStatsImpl::DistributeWorkLocked(
    /* [in] */ Int32 which)
{
    // Aggregate all CPU time associated with WIFI.
    AutoPtr<Uid> wifiUid;
    AutoPtr<IInterface> value;
    mUidStats->Get(IProcess::WIFI_UID, (IInterface**)&value);
    if (value != NULL) {
        wifiUid = (Uid*)(IObject*)value.Get();
    }
    if (wifiUid != NULL) {
        Int64 uSecTime;
        ComputeBatteryRealtime(SystemClock::GetElapsedRealtime() * 1000, which, &uSecTime);
        Int32 size;
        wifiUid->mProcessStats->GetSize(&size);
        for (Int32 ip = size - 1; ip >= 0; ip--) {
            AutoPtr<IInterface> value;
            wifiUid->mProcessStats->GetValueAt(ip, (IInterface**)&value);
            AutoPtr<Uid::Proc> proc = (Uid::Proc*)(IObject*)value.Get();
            Int64 totalRunningTime;
            GetGlobalWifiRunningTime(uSecTime, which, &totalRunningTime);
            Int32 statsSize;
            mUidStats->GetSize(&statsSize);
            for (Int32 i = 0; i < statsSize; i++) {
                AutoPtr<IInterface> temp;
                mUidStats->ValueAt(i, (IInterface**)&temp);
                AutoPtr<Uid> uid = (Uid*)(IObject*)temp.Get();
                if (uid->mUid != IProcess::WIFI_UID) {
                    Int64 uidRunningTime;
                    uid->GetWifiRunningTime(uSecTime, which, &uidRunningTime);
                    if (uidRunningTime > 0) {
                        AutoPtr<Uid::Proc> uidProc = uid->GetProcessStatsLocked(String("*wifi*"));
                        Int64 time;
                        proc->GetUserTime(which, &time);
                        time = (time * uidRunningTime) / totalRunningTime;
                        uidProc->mUserTime += time;
                        proc->mUserTime -= time;
                        proc->GetSystemTime(which, &time);
                        time = (time * uidRunningTime) / totalRunningTime;
                        uidProc->mSystemTime += time;
                        proc->mSystemTime -= time;
                        proc->GetForegroundTime(which, &time);
                        time = (time * uidRunningTime) / totalRunningTime;
                        uidProc->mForegroundTime += time;
                        proc->mForegroundTime -= time;
                        for (Int32 sb=0; sb < proc->mSpeedBins->GetLength(); sb++) {
                            AutoPtr<SamplingCounter> sc = (*proc->mSpeedBins)[sb];
                            if (sc != NULL) {
                                Int32 c;
                                sc->GetCountLocked(which, &c);
                                time = (Int32)c;
                                time = (time * uidRunningTime) / totalRunningTime;
                                AutoPtr<SamplingCounter> uidSc = (*uidProc->mSpeedBins)[sb];
                                if (uidSc == NULL) {
                                    uidSc = new SamplingCounter(mOnBatteryTimeBase);
                                    uidProc->mSpeedBins->Set(sb, uidSc);
                                }
                                Int32 result;
                                uidSc->mCount->AddAndGet((Int32)time, &result);
                                sc->mCount->AddAndGet((Int32)-time, &result);
                            }
                        }
                        totalRunningTime -= uidRunningTime;
                    }
                }
            }
        }
    }
}

ECode BatteryStatsImpl::ShutdownLocked()
{
    WriteSyncLocked();
    mShuttingDown = TRUE;
    return NOERROR;
}

ECode BatteryStatsImpl::WriteAsyncLocked()
{
    WriteLocked(FALSE);
    return NOERROR;
}

ECode BatteryStatsImpl::WriteSyncLocked()
{
    WriteLocked(TRUE);
    return NOERROR;
}

void BatteryStatsImpl::WriteLocked(
    /* [in] */ Boolean sync)
{
    if (mFile == NULL) {
        Logger::W("BatteryStats", "writeLocked: no file associated with this instance");
        return;
    }

    if (mShuttingDown) {
        return;
    }

    AutoPtr<IParcel> out;
    CParcel::New((IParcel**)&out);
    WriteSummaryToParcel(out, TRUE);
    mLastWriteTime = SystemClock::GetElapsedRealtime();

    // if (mPendingWrite != NULL) {
    //     mPendingWrite->Recycle();
    // }
    mPendingWrite = out;

    if (sync) {
        CommitPendingDataToDisk();
    }
    else {
        AutoPtr<IBackgroundThreadHelper> helper;
        CBackgroundThreadHelper::AcquireSingleton((IBackgroundThreadHelper**)&helper);
        AutoPtr<IHandler> h;
        helper->GetHandler((IHandler**)&h);
        AutoPtr<IRunnable> run = (IRunnable*)new BatteryStatsWriteRunnable(this);
        Boolean result;
        h->Post(run, &result);
    }
}

void BatteryStatsImpl::CommitPendingDataToDisk()
{
    AutoPtr<IParcel> next;
    {
        AutoLock lock(this);
        next = mPendingWrite;
        mPendingWrite = NULL;
        if (next == NULL) {
            return;
        }

        ILock::Probe(mWriteLock)->Lock();
    }
    // try {
    AutoPtr<IFile> f;
    mFile->ChooseForWrite((IFile**)&f);
    AutoPtr<IFileOutputStream> stream;
    CFileOutputStream::New(f, (IFileOutputStream**)&stream);
    AutoPtr< ArrayOf<Byte> > bytes;
    next->Marshall((ArrayOf<Byte>**)&bytes);
    IOutputStream::Probe(stream)->Write(bytes);
    IFlushable::Probe(stream)->Flush();
    FileUtils::Sync(stream);
    ICloseable::Probe(stream)->Close();
    mFile->Commit();
    // } catch (IOException e) {
    //     Logger::W("BatteryStats", "Error writing battery statistics", e);
    //     mFile.failWrite(stream);
    // } finally {
    //     next.recycle();
    //     mWriteLock.unlock();
    // }
    // next.recycle();
    ILock::Probe(mWriteLock)->UnLock();
}

ECode BatteryStatsImpl::ReadLocked()
{
    if (mFile == NULL) {
        Slogger::W("BatteryStats", "readLocked: no file associated with this instance");
        return NOERROR;
    }

    mUidStats->Clear();

    // try {
    AutoPtr<IFile> file;
    mFile->ChooseForRead((IFile**)&file);
    Boolean exists;
    if (file->Exists(&exists), !exists) {
        return NOERROR;
    }
    AutoPtr<IFileInputStream> stream;
    CFileInputStream::New(file, (IFileInputStream**)&stream);

    AutoPtr<ArrayOf<byte> > raw = BatteryStatsHelper::ReadFully(stream);
    AutoPtr<IParcel> in;// = Parcel.obtain();
    CParcel::New((IParcel**)&in);
    in->Unmarshall(raw, 0, raw->GetLength());
    in->SetDataPosition(0);
    ICloseable::Probe(stream)->Close();

    ReadSummaryFromParcel(in);
    // } catch(Exception e) {
    //     Slog.e("BatteryStats", "Error reading battery statistics", e);
    // }

    mEndPlatformVersion = Build::ID;

    Int32 pos;
    if (mHistoryBuffer->GetDataPosition(&pos), pos > 0) {
        mRecordingHistory = TRUE;
        Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();
        Int64 uptime = SystemClock::GetUptimeMillis();
        if (USE_OLD_HISTORY) {
            AddHistoryRecordLocked(elapsedRealtime, uptime, IBatteryStatsHistoryItem::CMD_START, mHistoryCur);
        }
        AddHistoryBufferLocked(elapsedRealtime, uptime, IBatteryStatsHistoryItem::CMD_START, mHistoryCur);
        StartRecordingHistory(elapsedRealtime, uptime, FALSE);
    }
    return NOERROR;
}

void BatteryStatsImpl::ReadHistory(
    /* [in] */ IParcel* in,
    /* [in] */ Boolean andOldHistory)
{
    Int64 historyBaseTime;
    in->ReadInt64(&historyBaseTime);

    // mHistoryBuffer.setDataSize(0);
    mHistoryBuffer->SetDataPosition(0);

    mHistoryTagPool.Clear();
    mNextHistoryTagIdx = 0;
    mNumHistoryTagChars = 0;

    Int32 numTags;
    in->ReadInt32(&numTags);
    for (Int32 i = 0; i < numTags; i++) {
        Int32 idx;
        in->ReadInt32(&idx);
        String str;
        in->ReadString(&str);
        Int32 uid;
        in->ReadInt32(&uid);
        AutoPtr<HistoryTag> tag = new HistoryTag();
        tag->mString = str;
        tag->mUid = uid;
        tag->mPoolIdx = idx;
        AutoPtr<IInteger32> integer;
        CInteger32::New(idx, (IInteger32**)&integer);
        mHistoryTagPool[tag] = integer;
        if (idx >= mNextHistoryTagIdx) {
            mNextHistoryTagIdx = idx + 1;
        }
        mNumHistoryTagChars += tag->mString.GetLength() + 1;
    }

    Int32 bufSize;
    in->ReadInt32(&bufSize);
    Int32 curPos;
    in->GetDataPosition(&curPos);
    if (bufSize >= (MAX_MAX_HISTORY_BUFFER * 3)) {
        Logger::W(TAG, "File corrupt: history data buffer too large %d", bufSize);
    }
    else if ((bufSize &~3) != bufSize) {
        Logger::W(TAG, "File corrupt: history data buffer not aligned %d", bufSize);
    }
    else {
        if (DEBUG_HISTORY)
            Logger::I(TAG, "***************** READING NEW HISTORY: %d bytes at ", bufSize, curPos);
        mHistoryBuffer->AppendFrom(in, curPos, bufSize);
        in->SetDataPosition(curPos + bufSize);
    }

    if (andOldHistory) {
        ReadOldHistory(in);
    }

    if (DEBUG_HISTORY) {
        StringBuilder sb(128);
        sb += "****************** OLD mHistoryBaseTime: ";
        TimeUtils::FormatDuration(mHistoryBaseTime, sb);
        Logger::I(TAG, sb.ToString());
    }
    mHistoryBaseTime = historyBaseTime;
    if (DEBUG_HISTORY) {
        StringBuilder sb(128);
        sb += "****************** NEW mHistoryBaseTime: ";
        TimeUtils::FormatDuration(mHistoryBaseTime, sb);
        Logger::I(TAG, sb.ToString());
    }

    // We are just arbitrarily going to insert 1 minute from the sample of
    // the last run until samples in this run.
    if (mHistoryBaseTime > 0) {
        Int64 oldnow = SystemClock::GetElapsedRealtime();
        mHistoryBaseTime = mHistoryBaseTime - oldnow + 1;
        if (DEBUG_HISTORY) {
            StringBuilder sb(128);
            sb += "****************** ADJUSTED mHistoryBaseTime: ";
            TimeUtils::FormatDuration(mHistoryBaseTime, sb);
            Logger::I(TAG, sb.ToString());
        }
    }
}

void BatteryStatsImpl::ReadOldHistory(
    /* [in] */ IParcel* in)
{
    if (!USE_OLD_HISTORY) {
        return;
    }
    mHistory = mHistoryEnd = mHistoryCache = NULL;
    Int32 pos, size;
    in->GetDataPosition(&pos);
    in->GetElementSize(&size);
    Int64 time;
    while (size - pos > 0 && (in->ReadInt64(&time), time >= 0)) {
        AutoPtr<HistoryItem> rec = new HistoryItem(time, in);
        AddHistoryRecordLocked(rec);
    }
}

void BatteryStatsImpl::WriteHistory(
    /* [in] */ IParcel* out,
    /* [in] */ Boolean inclData,
    /* [in] */ Boolean andOldHistory)
{
    if (DEBUG_HISTORY) {
        StringBuilder sb(128);
        sb += "****************** WRITING mHistoryBaseTime: ";
        TimeUtils::FormatDuration(mHistoryBaseTime, sb);
        sb += " mLastHistoryElapsedRealtime: ";
        TimeUtils::FormatDuration(mLastHistoryElapsedRealtime, sb);
        Logger::I(TAG, sb.ToString());
    }
    out->WriteInt64(mHistoryBaseTime + mLastHistoryElapsedRealtime);
    if (!inclData) {
        out->WriteInt32(0);
        out->WriteInt32(0);
        return;
    }
    out->WriteInt32(mHistoryTagPool.GetSize());
    HashMap<AutoPtr<HistoryTag>, AutoPtr<IInteger32> >::Iterator it = mHistoryTagPool.Begin();
    for (; it != mHistoryTagPool.End(); ++it) {
        AutoPtr<HistoryTag> tag = it->mFirst;
        Int32 value;
        it->mSecond->GetValue(&value);
        out->WriteInt32(value);
        out->WriteString(tag->mString);
        out->WriteInt32(tag->mUid);
    }
    Int32 size;
    mHistoryBuffer->GetElementSize(&size);
    out->WriteInt32(size);
    if (DEBUG_HISTORY) {
        Int32 pos;
        out->GetDataPosition(&pos);
        Logger::I(TAG, "***************** WRITING HISTORY: %d bytes at %d", size, pos);
    }
    out->AppendFrom(mHistoryBuffer, 0, size);

    if (andOldHistory) {
        WriteOldHistory(out);
    }
}

void BatteryStatsImpl::WriteOldHistory(
    /* [in] */ IParcel* out)
{
    if (!USE_OLD_HISTORY) {
        return;
    }
    AutoPtr<HistoryItem> rec = mHistory;
    while (rec != NULL) {
        if (rec->mTime >= 0) rec->WriteToParcel(out);
        rec = rec->mNext;
    }
    out->WriteInt64(-1);
}

ECode BatteryStatsImpl::ReadSummaryFromParcel(
    /* [in] */ IParcel* in)
{
    Int32 version;
    in->ReadInt32(&version);
    if (version != VERSION) {
        Slogger::W(TAG, "readFromParcel: version got %d, expected %d; erasing old stats", version, VERSION);
        return NOERROR;
    }

    ReadHistory(in, TRUE);

    in->ReadInt32(&mStartCount);
    in->ReadInt64(&mUptime);
    in->ReadInt64(&mRealtime);
    in->ReadInt64(&mStartClockTime);
    in->ReadString(&mStartPlatformVersion);
    in->ReadString(&mEndPlatformVersion);
    mOnBatteryTimeBase->ReadSummaryFromParcel(in);
    mOnBatteryScreenOffTimeBase->ReadSummaryFromParcel(in);
    in->ReadInt32(&mDischargeUnplugLevel);
    in->ReadInt32(&mDischargePlugLevel);
    in->ReadInt32(&mDischargeCurrentLevel);
    in->ReadInt32(&mCurrentBatteryLevel);
    in->ReadInt32(&mLowDischargeAmountSinceCharge);
    in->ReadInt32(&mHighDischargeAmountSinceCharge);
    in->ReadInt32(&mDischargeAmountScreenOnSinceCharge);
    in->ReadInt32(&mDischargeAmountScreenOffSinceCharge);
    in->ReadInt32(&mNumDischargeStepDurations);
    mDischargeStepDurations = NULL;
    in->ReadArrayOf((Handle32*)&mDischargeStepDurations);
    in->ReadInt32(&mNumChargeStepDurations);
    mChargeStepDurations = NULL;
    in->ReadArrayOf((Handle32*)&mChargeStepDurations);

    mStartCount++;

    mScreenState = IDisplay::STATE_UNKNOWN;
    mScreenOnTimer->ReadSummaryFromParcelLocked(in);
    for (Int32 i = 0; i < IBatteryStats::NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        (*mScreenBrightnessTimer)[i]->ReadSummaryFromParcelLocked(in);
    }
    mInteractive = FALSE;
    mInteractiveTimer->ReadSummaryFromParcelLocked(in);
    mPhoneOn = FALSE;
    mLowPowerModeEnabledTimer->ReadSummaryFromParcelLocked(in);
    mPhoneOnTimer->ReadSummaryFromParcelLocked(in);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        (*mPhoneSignalStrengthsTimer)[i]->ReadSummaryFromParcelLocked(in);
    }
    mPhoneSignalScanningTimer->ReadSummaryFromParcelLocked(in);
    for (Int32 i = 0; i < IBatteryStats::NUM_DATA_CONNECTION_TYPES; i++) {
        (*mPhoneDataConnectionsTimer)[i]->ReadSummaryFromParcelLocked(in);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
        (*mNetworkByteActivityCounters)[i]->ReadSummaryFromParcelLocked(in);
        (*mNetworkPacketActivityCounters)[i]->ReadSummaryFromParcelLocked(in);
    }
    mMobileRadioPowerState = IDataConnectionRealTimeInfo::DC_POWER_STATE_LOW;
    mMobileRadioActiveTimer->ReadSummaryFromParcelLocked(in);
    mMobileRadioActivePerAppTimer->ReadSummaryFromParcelLocked(in);
    mMobileRadioActiveAdjustedTime->ReadSummaryFromParcelLocked(in);
    mMobileRadioActiveUnknownTime->ReadSummaryFromParcelLocked(in);
    mMobileRadioActiveUnknownCount->ReadSummaryFromParcelLocked(in);
    mWifiOn = FALSE;
    mWifiOnTimer->ReadSummaryFromParcelLocked(in);
    mGlobalWifiRunning = FALSE;
    mGlobalWifiRunningTimer->ReadSummaryFromParcelLocked(in);
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_STATES; i++) {
        (*mWifiStateTimer)[i]->ReadSummaryFromParcelLocked(in);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SUPPL_STATES; i++) {
        (*mWifiSupplStateTimer)[i]->ReadSummaryFromParcelLocked(in);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        (*mWifiSignalStrengthsTimer)[i]->ReadSummaryFromParcelLocked(in);
    }
    mBluetoothOn = FALSE;
    mBluetoothOnTimer->ReadSummaryFromParcelLocked(in);
    for (Int32 i = 0; i < IBatteryStats::NUM_BLUETOOTH_STATES; i++) {
        (*mBluetoothStateTimer)[i]->ReadSummaryFromParcelLocked(in);
    }
    mFlashlightOn = FALSE;
    mFlashlightOnTimer->ReadSummaryFromParcelLocked(in);

    Int32 NKW;
    in->ReadInt32(&NKW);
    if (NKW > 10000) {
        Slogger::W(TAG, "File corrupt: too many kernel wake locks %d", NKW);
        return NOERROR;
    }
    for (Int32 ikw = 0; ikw < NKW; ikw++) {
        Int32 value;
        if (in->ReadInt32(&value), value != 0) {
            String kwltName;
            in->ReadString(&kwltName);
            GetKernelWakelockTimerLocked(kwltName)->ReadSummaryFromParcelLocked(in);
        }
    }

    Int32 NWR;
    in->ReadInt32(&NWR);
    if (NWR > 10000) {
        Slogger::W(TAG, "File corrupt: too many wakeup reasons %d", NWR);
        return NOERROR;
    }
    for (Int32 iwr = 0; iwr < NWR; iwr++) {
        Int32 value;
        if (in->ReadInt32(&value), value != 0) {
            String reasonName;
            in->ReadString(&reasonName);
            GetWakeupReasonTimerLocked(reasonName)->ReadSummaryFromParcelLocked(in);
        }
    }

    in->ReadInt32(&sNumSpeedSteps);
    if (sNumSpeedSteps < 0 || sNumSpeedSteps > 100) {
        Slogger::E(TAG, "Bad speed steps in data: %d", sNumSpeedSteps);
        return E_BAD_PARCELABLE_EXCEPTION;
    }

    Int32 NU;
    in->ReadInt32(&NU);
    if (NU > 10000) {
        Slogger::W(TAG, "File corrupt: too many uids %d", NU);
        return NOERROR;
    }
    for (Int32 iu = 0; iu < NU; iu++) {
        Int32 uid;
        in->ReadInt32(&uid);
        AutoPtr<Uid> u = new Uid(uid, this);
        mUidStats->Put(uid, (IObject*)u.Get());

        u->mWifiRunning = FALSE;
        Int32 value;
        if (in->ReadInt32(&value), value != 0) {
            u->mWifiRunningTimer->ReadSummaryFromParcelLocked(in);
        }
        u->mFullWifiLockOut = FALSE;
        if (in->ReadInt32(&value), value != 0) {
            u->mFullWifiLockTimer->ReadSummaryFromParcelLocked(in);
        }
        u->mWifiScanStarted = FALSE;
        if (in->ReadInt32(&value), value != 0) {
            u->mWifiScanTimer->ReadSummaryFromParcelLocked(in);
        }
        u->mWifiBatchedScanBinStarted = Uid::NO_BATCHED_SCAN_STARTED;
        for (Int32 i = 0; i < Uid::NUM_WIFI_BATCHED_SCAN_BINS; i++) {
            if (in->ReadInt32(&value), value != 0) {
                u->MakeWifiBatchedScanBin(i, NULL);
                (*u->mWifiBatchedScanTimer)[i]->ReadSummaryFromParcelLocked(in);
            }
        }
        u->mWifiMulticastEnabled = FALSE;
        if (in->ReadInt32(&value), value != 0) {
            u->mWifiMulticastTimer->ReadSummaryFromParcelLocked(in);
        }
        if (in->ReadInt32(&value), value != 0) {
            u->CreateAudioTurnedOnTimerLocked()->ReadSummaryFromParcelLocked(in);
        }
        if (in->ReadInt32(&value), value != 0) {
            u->CreateVideoTurnedOnTimerLocked()->ReadSummaryFromParcelLocked(in);
        }
        if (in->ReadInt32(&value), value != 0) {
            u->CreateForegroundActivityTimerLocked()->ReadSummaryFromParcelLocked(in);
        }
        u->mProcessState = Uid::PROCESS_STATE_NONE;
        for (Int32 i = 0; i < Uid::NUM_PROCESS_STATE; i++) {
            if (in->ReadInt32(&value), value != 0) {
                u->MakeProcessState(i, NULL);
                (*u->mProcessStateTimer)[i]->ReadSummaryFromParcelLocked(in);
            }
        }
        if (in->ReadInt32(&value), value != 0) {
            u->CreateVibratorOnTimerLocked()->ReadSummaryFromParcelLocked(in);
        }

        if (in->ReadInt32(&value), value != 0) {
            if (u->mUserActivityCounters == NULL) {
                u->InitUserActivityLocked();
            }
            for (Int32 i = 0; i < Uid::NUM_USER_ACTIVITY_TYPES; i++) {
                (*u->mUserActivityCounters)[i]->ReadSummaryFromParcelLocked(in);
            }
        }

        if (in->ReadInt32(&value), value != 0) {
            if (u->mNetworkByteActivityCounters == NULL) {
                u->InitNetworkActivityLocked();
            }
            for (Int32 i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
                (*u->mNetworkByteActivityCounters)[i]->ReadSummaryFromParcelLocked(in);
                (*u->mNetworkPacketActivityCounters)[i]->ReadSummaryFromParcelLocked(in);
            }
            u->mMobileRadioActiveTime->ReadSummaryFromParcelLocked(in);
            u->mMobileRadioActiveCount->ReadSummaryFromParcelLocked(in);
        }

        Int32 NW;
        in->ReadInt32(&NW);
        if (NW > 100) {
            Slogger::W(TAG, "File corrupt: too many wake locks %d", NW);
            return NOERROR;
        }
        for (Int32 iw = 0; iw < NW; iw++) {
            String wlName;
            in->ReadString(&wlName);
            u->ReadWakeSummaryFromParcelLocked(wlName, in);
        }

        Int32 NS;
        in->ReadInt32(&NS);
        if (NS > 100) {
            Slogger::W(TAG, "File corrupt: too many syncs %d", NS);
            return NOERROR;
        }
        for (Int32 is = 0; is < NS; is++) {
            String name;
            in->ReadString(&name);
            u->ReadSyncSummaryFromParcelLocked(name, in);
        }

        Int32 NJ;
        in->ReadInt32(&NJ);
        if (NJ > 100) {
            Slogger::W(TAG, "File corrupt: too many job timers %d", NJ);
            return NOERROR;
        }
        for (Int32 ij = 0; ij < NJ; ij++) {
            String name;
            in->ReadString(&name);
            u->ReadJobSummaryFromParcelLocked(name, in);
        }

        Int32 NP;
        in->ReadInt32(&NP);
        if (NP > 1000) {
            Slogger::W(TAG, "File corrupt: too many sensors %d", NP);
            return NOERROR;
        }
        for (Int32 is = 0; is < NP; is++) {
            Int32 seNumber;
            in->ReadInt32(&seNumber);
            if (in->ReadInt32(&value), value != 0) {
                u->GetSensorTimerLocked(seNumber, TRUE)->ReadSummaryFromParcelLocked(in);
            }
        }

        in->ReadInt32(&NP);
        if (NP > 1000) {
            Slogger::W(TAG, "File corrupt: too many processes %d", NP);
            return NOERROR;
        }
        for (Int32 ip = 0; ip < NP; ip++) {
            String procName;
            in->ReadString(&procName);
            AutoPtr<Uid::Proc> p = u->GetProcessStatsLocked(procName);
            Int64 v;
            in->ReadInt64(&v);
            p->mUserTime = p->mLoadedUserTime = v;
            in->ReadInt64(&v);
            p->mSystemTime = p->mLoadedSystemTime = v;
            in->ReadInt64(&v);
            p->mForegroundTime = p->mLoadedForegroundTime = v;
            in->ReadInt32(&value);
            p->mStarts = p->mLoadedStarts = value;
            Int32 NSB;
            in->ReadInt32(&NSB);
            if (NSB > 100) {
                Slogger::W(TAG, "File corrupt: too many speed bins %d", NSB);
                return NOERROR;
            }
            p->mSpeedBins = ArrayOf<SamplingCounter*>::Alloc(NSB);
            for (Int32 i = 0; i < NSB; i++) {
                if (in->ReadInt32(&value), value != 0) {
                    AutoPtr<SamplingCounter> c = new SamplingCounter(mOnBatteryTimeBase);
                    p->mSpeedBins->Set(i, c);
                    (*p->mSpeedBins)[i]->ReadSummaryFromParcelLocked(in);
                }
            }
            if (!p->ReadExcessivePowerFromParcelLocked(in)) {
                return NOERROR;
            }
        }

        in->ReadInt32(&NP);
        if (NP > 10000) {
            Slogger::W(TAG, "File corrupt: too many packages %d", NP);
            return NOERROR;
        }
        for (Int32 ip = 0; ip < NP; ip++) {
            String pkgName;
            in->ReadString(&pkgName);
            AutoPtr<Uid::Pkg> p = u->GetPackageStatsLocked(pkgName);
            Int32 v;
            in->ReadInt32(&v);
            p->mWakeups = p->mLoadedWakeups = v;
            in->ReadInt32(&NS);
            if (NS > 1000) {
                Slogger::W(TAG, "File corrupt: too many services %d", NS);
                return NOERROR;
            }
            for (Int32 is = 0; is < NS; is++) {
                String servName;
                in->ReadString(&servName);
                AutoPtr<Uid::Pkg::Serv> s = u->GetServiceStatsLocked(pkgName, servName);
                Int64 startTime;
                in->ReadInt64(&startTime);
                s->mStartTime = s->mLoadedStartTime = startTime;
                in->ReadInt32(&v);
                s->mStarts = s->mLoadedStarts = v;
                in->ReadInt32(&v);
                s->mLaunches = s->mLoadedLaunches = v;
            }
        }
    }
    return NOERROR;
}

void BatteryStatsImpl::WriteSummaryToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Boolean inclHistory)
{
    PullPendingStateUpdatesLocked();

    // Pull the clock time.  This may update the time and make a new history entry
    // if we had originally pulled a time before the RTC was set.
    Int64 startClockTime;
    GetStartClockTime(&startClockTime);

    Int64 NOW_SYS = SystemClock::GetUptimeMillis() * 1000;
    Int64 NOWREAL_SYS = SystemClock::GetElapsedRealtime() * 1000;

    out->WriteInt32(VERSION);

    WriteHistory(out, inclHistory, TRUE);

    out->WriteInt32(mStartCount);
    Int64 uptime;
    ComputeUptime(NOW_SYS, IBatteryStats::STATS_SINCE_CHARGED, &uptime);
    out->WriteInt64(uptime);
    Int64 realtime;
    ComputeRealtime(NOWREAL_SYS, IBatteryStats::STATS_SINCE_CHARGED, &realtime);
    out->WriteInt64(realtime);
    out->WriteInt64(startClockTime);
    out->WriteString(mStartPlatformVersion);
    out->WriteString(mEndPlatformVersion);
    mOnBatteryTimeBase->WriteSummaryToParcel(out, NOW_SYS, NOWREAL_SYS);
    mOnBatteryScreenOffTimeBase->WriteSummaryToParcel(out, NOW_SYS, NOWREAL_SYS);
    out->WriteInt32(mDischargeUnplugLevel);
    out->WriteInt32(mDischargePlugLevel);
    out->WriteInt32(mDischargeCurrentLevel);
    out->WriteInt32(mCurrentBatteryLevel);
    Int32 low;
    GetLowDischargeAmountSinceCharge(&low);
    out->WriteInt32(low);
    Int32 high;
    GetHighDischargeAmountSinceCharge(&high);
    out->WriteInt32(high);
    Int32 on;
    GetDischargeAmountScreenOnSinceCharge(&on);
    out->WriteInt32(on);
    Int32 off;
    GetDischargeAmountScreenOffSinceCharge(&off);
    out->WriteInt32(off);
    out->WriteInt32(mNumDischargeStepDurations);
    out->WriteArrayOf((Handle32)mDischargeStepDurations.Get());
    out->WriteInt32(mNumChargeStepDurations);
    out->WriteArrayOf((Handle32)mChargeStepDurations.Get());

    mScreenOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    for (Int32 i = 0; i < NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        (*mScreenBrightnessTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    }
    mInteractiveTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    mLowPowerModeEnabledTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    mPhoneOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        (*mPhoneSignalStrengthsTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    }
    mPhoneSignalScanningTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    for (Int32 i = 0; i < NUM_DATA_CONNECTION_TYPES; i++) {
        (*mPhoneDataConnectionsTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    }
    for (Int32 i = 0; i < NUM_NETWORK_ACTIVITY_TYPES; i++) {
        (*mNetworkByteActivityCounters)[i]->WriteSummaryFromParcelLocked(out);
        (*mNetworkPacketActivityCounters)[i]->WriteSummaryFromParcelLocked(out);
    }
    mMobileRadioActiveTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    mMobileRadioActivePerAppTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    mMobileRadioActiveAdjustedTime->WriteSummaryFromParcelLocked(out);
    mMobileRadioActiveUnknownTime->WriteSummaryFromParcelLocked(out);
    mMobileRadioActiveUnknownCount->WriteSummaryFromParcelLocked(out);
    mWifiOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    mGlobalWifiRunningTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_STATES; i++) {
        (*mWifiStateTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SUPPL_STATES; i++) {
        (*mWifiSupplStateTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        (*mWifiSignalStrengthsTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    }
    mBluetoothOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    for (Int32 i = 0; i < IBatteryStats::NUM_BLUETOOTH_STATES; i++) {
        (*mBluetoothStateTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
    }
    mFlashlightOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);

    Int32 size;
    mKernelWakelockStats->GetSize(&size);
    out->WriteInt32(size);
    AutoPtr<ISet> set;
    mKernelWakelockStats->GetEntrySet((ISet**)&set);
    AutoPtr<IIterator> it;
    set->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        it->GetNext((IInterface**)&next);
        AutoPtr<IMapEntry> ent = IMapEntry::Probe(next);
        AutoPtr<IInterface> value;
        ent->GetValue((IInterface**)&value);
        if (value != NULL) {
            AutoPtr<SamplingTimer> kwlt = (SamplingTimer*)(IObject*)value.Get();
            out->WriteInt32(1);
            AutoPtr<IInterface> ki;
            ent->GetKey((IInterface**)&ki);
            String key;
            ICharSequence::Probe(ki)->ToString(&key);
            out->WriteString(key);
            kwlt->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }
        else {
            out->WriteInt32(0);
        }
    }

    mWakeupReasonStats->GetSize(&size);
    out->WriteInt32(size);
    AutoPtr<ISet> statsSet;
    mWakeupReasonStats->GetEntrySet((ISet**)&statsSet);
    AutoPtr<IIterator> statsIt;
    statsSet->GetIterator((IIterator**)&statsIt);
    while (statsIt->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> next;
        statsIt->GetNext((IInterface**)&next);
        AutoPtr<IMapEntry> ent = IMapEntry::Probe(next);
        AutoPtr<IInterface> value;
        ent->GetValue((IInterface**)&value);
        if (value == NULL) {
            AutoPtr<SamplingTimer> timer = (SamplingTimer*)(IObject*)value.Get();
            out->WriteInt32(1);
            AutoPtr<IInterface> ki;
            ent->GetKey((IInterface**)&ki);
            String key;
            ICharSequence::Probe(ki)->ToString(&key);
            out->WriteString(key);
            timer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }
        else {
            out->WriteInt32(0);
        }
    }

    out->WriteInt32(sNumSpeedSteps);
    Int32 NU;
    mUidStats->GetSize(&NU);
    out->WriteInt32(NU);
    for (Int32 iu = 0; iu < NU; iu++) {
        Int32 key;
        mUidStats->KeyAt(iu, &key);
        out->WriteInt32(key);
        AutoPtr<IInterface> value;
        mUidStats->ValueAt(iu, (IInterface**)&value);
        AutoPtr<Uid> u = (Uid*)(IObject*)value.Get();

        if (u->mWifiRunningTimer != NULL) {
            out->WriteInt32(1);
            u->mWifiRunningTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }
        else {
            out->WriteInt32(0);
        }
        if (u->mFullWifiLockTimer != NULL) {
            out->WriteInt32(1);
            u->mFullWifiLockTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }
        else {
            out->WriteInt32(0);
        }
        if (u->mWifiScanTimer != NULL) {
            out->WriteInt32(1);
            u->mWifiScanTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }
        else {
            out->WriteInt32(0);
        }
        for (Int32 i = 0; i < Uid::NUM_WIFI_BATCHED_SCAN_BINS; i++) {
            if ((*u->mWifiBatchedScanTimer)[i] != NULL) {
                out->WriteInt32(1);
                (*u->mWifiBatchedScanTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
            }
            else {
                out->WriteInt32(0);
            }
        }
        if (u->mWifiMulticastTimer != NULL) {
            out->WriteInt32(1);
            u->mWifiMulticastTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }
        else {
            out->WriteInt32(0);
        }
        if (u->mAudioTurnedOnTimer != NULL) {
            out->WriteInt32(1);
            u->mAudioTurnedOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }
        else {
            out->WriteInt32(0);
        }
        if (u->mVideoTurnedOnTimer != NULL) {
            out->WriteInt32(1);
            u->mVideoTurnedOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }
        else {
            out->WriteInt32(0);
        }
        if (u->mForegroundActivityTimer != NULL) {
            out->WriteInt32(1);
            u->mForegroundActivityTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }
        else {
            out->WriteInt32(0);
        }
        for (Int32 i = 0; i < Uid::NUM_PROCESS_STATE; i++) {
            if ((*u->mProcessStateTimer)[i] != NULL) {
                out->WriteInt32(1);
                (*u->mProcessStateTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
            }
            else {
                out->WriteInt32(0);
            }
        }
        if (u->mVibratorOnTimer != NULL) {
            out->WriteInt32(1);
            u->mVibratorOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }
        else {
            out->WriteInt32(0);
        }

        if (u->mUserActivityCounters == NULL) {
            out->WriteInt32(0);
        }
        else {
            out->WriteInt32(1);
            for (Int32 i = 0; i < Uid::NUM_USER_ACTIVITY_TYPES; i++) {
                (*u->mUserActivityCounters)[i]->WriteSummaryFromParcelLocked(out);
            }
        }

        if (u->mNetworkByteActivityCounters == NULL) {
            out->WriteInt32(0);
        }
        else {
            out->WriteInt32(1);
            for (Int32 i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
                (*u->mNetworkByteActivityCounters)[i]->WriteSummaryFromParcelLocked(out);
                (*u->mNetworkPacketActivityCounters)[i]->WriteSummaryFromParcelLocked(out);
            }
            u->mMobileRadioActiveTime->WriteSummaryFromParcelLocked(out);
            u->mMobileRadioActiveCount->WriteSummaryFromParcelLocked(out);
        }

        AutoPtr<IArrayMap> wakeStats = u->mWakelockStats->GetMap();
        Int32 NW;
        wakeStats->GetSize(&NW);
        out->WriteInt32(NW);
        for (Int32 iw = 0; iw < NW; iw++) {
            AutoPtr<IInterface> ki;
            wakeStats->GetKeyAt(iw, (IInterface**)&ki);
            String key;
            ICharSequence::Probe(ki)->ToString(&key);
            out->WriteString(key);
            AutoPtr<IInterface> vi;
            wakeStats->GetValueAt(iw, (IInterface**)&vi);
            AutoPtr<Uid::Wakelock> wl = (Uid::Wakelock*)(IObject*)vi.Get();
            if (wl->mTimerFull != NULL) {
                out->WriteInt32(1);
                wl->mTimerFull->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
            }
            else {
                out->WriteInt32(0);
            }
            if (wl->mTimerPartial != NULL) {
                out->WriteInt32(1);
                wl->mTimerPartial->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
            }
            else {
                out->WriteInt32(0);
            }
            if (wl->mTimerWindow != NULL) {
                out->WriteInt32(1);
                wl->mTimerWindow->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
            }
            else {
                out->WriteInt32(0);
            }
        }

        AutoPtr<IArrayMap> syncStats = u->mSyncStats->GetMap();
        Int32 NS;
        syncStats->GetSize(&NS);
        out->WriteInt32(NS);
        for (Int32 is = 0; is < NS; is++) {
            AutoPtr<IInterface> ki;
            syncStats->GetKeyAt(is, (IInterface**)&ki);
            String key;
            ICharSequence::Probe(ki)->ToString(&key);
            out->WriteString(key);
            AutoPtr<IInterface> vi;
            syncStats->GetValueAt(is, (IInterface**)&vi);
            AutoPtr<StopwatchTimer> timer = (StopwatchTimer*)(IObject*)vi.Get();
            timer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }

        AutoPtr<IArrayMap> jobStats = u->mJobStats->GetMap();
        Int32 NJ;
        jobStats->GetSize(&NJ);
        out->WriteInt32(NJ);
        for (Int32 ij = 0; ij < NJ; ij++) {
            AutoPtr<IInterface> ki;
            jobStats->GetKeyAt(ij, (IInterface**)&ki);
            String key;
            ICharSequence::Probe(ki)->ToString(&key);
            out->WriteString(key);
            AutoPtr<IInterface> vi;
            jobStats->GetValueAt(ij, (IInterface**)&vi);
            AutoPtr<StopwatchTimer> timer = (StopwatchTimer*)(IObject*)vi.Get();
            timer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
        }

        Int32 NSE;
        u->mSensorStats->GetSize(&NSE);
        out->WriteInt32(NSE);
        for (Int32 ise = 0; ise < NSE; ise++) {
            Int32 key;
            u->mSensorStats->KeyAt(ise, &key);
            out->WriteInt32(key);
            AutoPtr<IInterface> value;
            u->mSensorStats->ValueAt(ise, (IInterface**)&value);
            AutoPtr<Uid::Sensor> se = (Uid::Sensor*)(IObject*)value.Get();
            if (se->mTimer != NULL) {
                out->WriteInt32(1);
                se->mTimer->WriteSummaryFromParcelLocked(out, NOWREAL_SYS);
            }
            else {
                out->WriteInt32(0);
            }
        }

        Int32 NP;
        u->mProcessStats->GetSize(&NP);
        out->WriteInt32(NP);
        for (Int32 ip = 0; ip < NP; ip++) {
            AutoPtr<IInterface> ki;
            u->mProcessStats->GetKeyAt(ip, (IInterface**)&ki);
            String key;
            ICharSequence::Probe(ki)->ToString(&key);
            out->WriteString(key);
            AutoPtr<IInterface> vi;
            u->mProcessStats->GetValueAt(ip, (IInterface**)&vi);
            AutoPtr<Uid::Proc> ps = (Uid::Proc*)(IObject*)vi.Get();
            out->WriteInt64(ps->mUserTime);
            out->WriteInt64(ps->mSystemTime);
            out->WriteInt64(ps->mForegroundTime);
            out->WriteInt32(ps->mStarts);
            Int32 N = ps->mSpeedBins->GetLength();
            out->WriteInt32(N);
            for (Int32 i = 0; i < N; i++) {
                if ((*ps->mSpeedBins)[i] != NULL) {
                    out->WriteInt32(1);
                    (*ps->mSpeedBins)[i]->WriteSummaryFromParcelLocked(out);
                }
                else {
                    out->WriteInt32(0);
                }
            }
            ps->WriteExcessivePowerToParcelLocked(out);
        }

        u->mPackageStats->GetSize(&NP);
        out->WriteInt32(NP);
        if (NP > 0) {
            AutoPtr<ISet> set;
            u->mPackageStats->GetEntrySet((ISet**)&set);
            AutoPtr<IIterator> itr;
            set->GetIterator((IIterator**)&itr);
            Boolean hasNext;
            while (itr->HasNext(&hasNext), hasNext) {
                AutoPtr<IInterface> next;
                itr->GetNext((IInterface**)&next);
                AutoPtr<IMapEntry> ent = IMapEntry::Probe(next);
                AutoPtr<IInterface> ki;
                ent->GetKey((IInterface**)&ki);
                String key;
                ICharSequence::Probe(ki)->ToString(&key);
                out->WriteString(key);
                AutoPtr<IInterface> vi;
                ent->GetValue((IInterface**)&vi);
                AutoPtr<Uid::Pkg> ps = (Uid::Pkg*)(IObject*)vi.Get();
                out->WriteInt32(ps->mWakeups);
                ps->mServiceStats->GetSize(&NS);
                out->WriteInt32(NS);
                if (NS > 0) {
                    AutoPtr<ISet> ssSet;
                    ps->mServiceStats->GetEntrySet((ISet**)&ssSet);
                    AutoPtr<IIterator> iterator;
                    ssSet->GetIterator((IIterator**)&iterator);
                    Boolean hasNext;
                    while (iterator->HasNext(&hasNext), hasNext) {
                        AutoPtr<IInterface> sentI;
                        iterator->GetNext((IInterface**)&sentI);
                        AutoPtr<IMapEntry> sent = IMapEntry::Probe(sentI);
                        AutoPtr<IInterface> ski;
                        sent->GetKey((IInterface**)&ski);
                        String sKey;
                        ICharSequence::Probe(ski)->ToString(&sKey);
                        out->WriteString(sKey);
                        AutoPtr<IInterface> svi;
                        sent->GetValue((IInterface**)&svi);
                        AutoPtr<Uid::Pkg::Serv> ss = (Uid::Pkg::Serv*)(IObject*)svi.Get();
                        Int64 time = ss->GetStartTimeToNowLocked(
                                mOnBatteryTimeBase->GetUptime(NOW_SYS));
                        out->WriteInt64(time);
                        out->WriteInt32(ss->mStarts);
                        out->WriteInt32(ss->mLaunches);
                    }
                }
            }
        }
    }
}

ECode BatteryStatsImpl::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    return ReadFromParcelLocked(in);
}

ECode BatteryStatsImpl::ReadFromParcelLocked(
    /* [in] */ IParcel* in)
{
    Int32 magic;
    in->ReadInt32(&magic);
    if (magic != MAGIC) {
        Slogger::E(TAG, "Bad magic number: #%s", StringUtils::ToHexString(magic).string());
        return E_PARCEL_FORMAT_EXCEPTION;
    }

    ReadHistory(in, FALSE);

    in->ReadInt32(&mStartCount);
    in->ReadInt64(&mStartClockTime);
    in->ReadString(&mStartPlatformVersion);
    in->ReadString(&mEndPlatformVersion);
    in->ReadInt64(&mUptime);
    in->ReadInt64(&mUptimeStart);
    in->ReadInt64(&mRealtime);
    in->ReadInt64(&mRealtimeStart);
    in->ReadBoolean(&mOnBattery);
    mOnBatteryInternal = FALSE; // we are no longer really running.
    mOnBatteryTimeBase->ReadFromParcel(in);
    mOnBatteryScreenOffTimeBase->ReadFromParcel(in);

    mScreenState = IDisplay::STATE_UNKNOWN;
    mScreenOnTimer = new StopwatchTimer(NULL, -1, NULL, mOnBatteryTimeBase, in);
    for (Int32 i = 0; i < IBatteryStats::NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -100 - i, NULL, mOnBatteryTimeBase, in);
        mScreenBrightnessTimer->Set(i, timer);
    }
    mInteractive = FALSE;
    mInteractiveTimer = new StopwatchTimer(NULL, -9, NULL, mOnBatteryTimeBase, in);
    mPhoneOn = FALSE;
    mLowPowerModeEnabledTimer = new StopwatchTimer(NULL, -2, NULL, mOnBatteryTimeBase, in);
    mPhoneOnTimer = new StopwatchTimer(NULL, -3, NULL, mOnBatteryTimeBase, in);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -200 - i, NULL, mOnBatteryTimeBase, in);
        mPhoneSignalStrengthsTimer->Set(i, timer);
    }
    mPhoneSignalScanningTimer = new StopwatchTimer(NULL, -200 + 1, NULL, mOnBatteryTimeBase, in);
    for (Int32 i = 0; i < IBatteryStats::NUM_DATA_CONNECTION_TYPES; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -300 - i, NULL, mOnBatteryTimeBase, in);
        mPhoneDataConnectionsTimer->Set(i, timer);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
        AutoPtr<Int64SamplingCounter> counter1 = new Int64SamplingCounter(mOnBatteryTimeBase, in);
        mNetworkByteActivityCounters->Set(i, counter1);
        AutoPtr<Int64SamplingCounter> counter2 = new Int64SamplingCounter(mOnBatteryTimeBase, in);
        mNetworkPacketActivityCounters->Set(i, counter2);
    }
    mMobileRadioPowerState = IDataConnectionRealTimeInfo::DC_POWER_STATE_LOW;
    mMobileRadioActiveTimer = new StopwatchTimer(NULL, -400, NULL, mOnBatteryTimeBase, in);
    mMobileRadioActivePerAppTimer = new StopwatchTimer(NULL, -401, NULL, mOnBatteryTimeBase, in);
    mMobileRadioActiveAdjustedTime = new Int64SamplingCounter(mOnBatteryTimeBase, in);
    mMobileRadioActiveUnknownTime = new Int64SamplingCounter(mOnBatteryTimeBase, in);
    mMobileRadioActiveUnknownCount = new Int64SamplingCounter(mOnBatteryTimeBase, in);
    mWifiOn = FALSE;
    mWifiOnTimer = new StopwatchTimer(NULL, -4, NULL, mOnBatteryTimeBase, in);
    mGlobalWifiRunning = FALSE;
    mGlobalWifiRunningTimer = new StopwatchTimer(NULL, -5, NULL, mOnBatteryTimeBase, in);
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_STATES; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -600 - i, NULL, mOnBatteryTimeBase, in);
        mWifiStateTimer->Set(i, timer);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SUPPL_STATES; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -700 - i, NULL, mOnBatteryTimeBase, in);
        mWifiSupplStateTimer->Set(i, timer);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -800 - i, NULL, mOnBatteryTimeBase, in);
        mWifiSignalStrengthsTimer->Set(i, timer);
    }
    mBluetoothOn = FALSE;
    mBluetoothOnTimer = new StopwatchTimer(NULL, -6, NULL, mOnBatteryTimeBase, in);
    for (Int32 i = 0; i < IBatteryStats::NUM_BLUETOOTH_STATES; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -500 - i, NULL, mOnBatteryTimeBase, in);
        mBluetoothStateTimer->Set(i, timer);
    }
    mAudioOnNesting = 0;
    mAudioOnTimer = new StopwatchTimer(NULL, -7, NULL, mOnBatteryTimeBase);
    mVideoOnNesting = 0;
    mVideoOnTimer = new StopwatchTimer(NULL, -8, NULL, mOnBatteryTimeBase);
    mFlashlightOn = FALSE;
    mFlashlightOnTimer = new StopwatchTimer(NULL, -9, NULL, mOnBatteryTimeBase, in);
    in->ReadInt32(&mDischargeUnplugLevel);
    in->ReadInt32(&mDischargePlugLevel);
    in->ReadInt32(&mDischargeCurrentLevel);
    in->ReadInt32(&mCurrentBatteryLevel);
    in->ReadInt32(&mLowDischargeAmountSinceCharge);
    in->ReadInt32(&mHighDischargeAmountSinceCharge);
    in->ReadInt32(&mDischargeAmountScreenOn);
    in->ReadInt32(&mDischargeAmountScreenOnSinceCharge);
    in->ReadInt32(&mDischargeAmountScreenOff);
    in->ReadInt32(&mDischargeAmountScreenOffSinceCharge);
    in->ReadInt32(&mNumDischargeStepDurations);
    mDischargeStepDurations = NULL;
    in->ReadArrayOf((Handle32*)&mDischargeStepDurations);
    in->ReadInt32(&mNumChargeStepDurations);
    mChargeStepDurations = NULL;
    in->ReadArrayOf((Handle32*)&mChargeStepDurations);
    in->ReadInt64(&mLastWriteTime);

    in->ReadInt32(&mBluetoothPingCount);
    mBluetoothPingStart = -1;

    mKernelWakelockStats->Clear();
    Int32 NKW;
    in->ReadInt32(&NKW);
    for (Int32 ikw = 0; ikw < NKW; ikw++) {
        Int32 value;
        if (in->ReadInt32(&value), value != 0) {
            String wakelockName;
            in->ReadString(&wakelockName);
            AutoPtr<SamplingTimer> kwlt = new SamplingTimer(mOnBatteryScreenOffTimeBase, in);
            AutoPtr<ICharSequence> cs;
            CString::New(wakelockName, (ICharSequence**)&cs);
            mKernelWakelockStats->Put(cs, (IObject*)kwlt);
        }
    }

    mWakeupReasonStats->Clear();
    Int32 NWR;
    in->ReadInt32(&NWR);
    for (Int32 iwr = 0; iwr < NWR; iwr++) {
        Int32 value;
        if (in->ReadInt32(&value), value != 0) {
            String reasonName;
            in->ReadString(&reasonName);
            AutoPtr<SamplingTimer> timer = new SamplingTimer(mOnBatteryTimeBase, in);
            AutoPtr<ICharSequence> cs;
            CString::New(reasonName, (ICharSequence**)&cs);
            mWakeupReasonStats->Put(cs, (IObject*)timer);
        }
    }

    mPartialTimers.Clear();
    mFullTimers.Clear();
    mWindowTimers.Clear();
    mWifiRunningTimers.Clear();
    mFullWifiLockTimers.Clear();
    mWifiScanTimers.Clear();
    mWifiBatchedScanTimers.Clear();
    mWifiMulticastTimers.Clear();
    mAudioTurnedOnTimers.Clear();
    mVideoTurnedOnTimers.Clear();

    in->ReadInt32(&sNumSpeedSteps);

    Int32 numUids;
    in->ReadInt32(&numUids);
    mUidStats->Clear();
    for (Int32 i = 0; i < numUids; i++) {
        Int32 uid;
        in->ReadInt32(&uid);
        AutoPtr<Uid> u = new Uid(uid, this);
        u->ReadFromParcelLocked(mOnBatteryTimeBase, mOnBatteryScreenOffTimeBase, in);
        mUidStats->Append(uid, (IObject*)u.Get());
    }
    return NOERROR;
}

ECode BatteryStatsImpl::WriteToParcel(
    /* [in] */ IParcel* out)
{
    WriteToParcelLocked(out, TRUE);
    return NOERROR;
}

ECode BatteryStatsImpl::WriteToParcelWithoutUids(
    /* [in] */ IParcel* out)
{
    WriteToParcelLocked(out, FALSE);
    return NOERROR;
}

void BatteryStatsImpl::WriteToParcelLocked(
    /* [in] */ IParcel* out,
    /* [in] */ Boolean inclUids)
{
    // Need to update with current kernel wake lock counts.
    PullPendingStateUpdatesLocked();

    // Pull the clock time.  This may update the time and make a new history entry
    // if we had originally pulled a time before the RTC was set.
    Int64 startClockTime;
    GetStartClockTime(&startClockTime);

    Int64 uSecUptime = SystemClock::GetUptimeMillis() * 1000;
    Int64 uSecRealtime = SystemClock::GetElapsedRealtime() * 1000;
    // Int64 batteryRealtime = mOnBatteryTimeBase->GetRealtime(uSecRealtime);
    // Int64 batteryScreenOffRealtime = mOnBatteryScreenOffTimeBase->GetRealtime(uSecRealtime);

    out->WriteInt32(MAGIC);

    WriteHistory(out, TRUE, FALSE);

    out->WriteInt32(mStartCount);
    out->WriteInt64(startClockTime);
    out->WriteString(mStartPlatformVersion);
    out->WriteString(mEndPlatformVersion);
    out->WriteInt64(mUptime);
    out->WriteInt64(mUptimeStart);
    out->WriteInt64(mRealtime);
    out->WriteInt64(mRealtimeStart);
    out->WriteBoolean(mOnBattery);
    mOnBatteryTimeBase->WriteToParcel(out, uSecUptime, uSecRealtime);
    mOnBatteryScreenOffTimeBase->WriteToParcel(out, uSecUptime, uSecRealtime);

    mScreenOnTimer->WriteToParcel(out, uSecRealtime);
    for (Int32 i = 0; i < IBatteryStats::NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        (*mScreenBrightnessTimer)[i]->WriteToParcel(out, uSecRealtime);
    }
    mInteractiveTimer->WriteToParcel(out, uSecRealtime);
    mLowPowerModeEnabledTimer->WriteToParcel(out, uSecRealtime);
    mPhoneOnTimer->WriteToParcel(out, uSecRealtime);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        (*mPhoneSignalStrengthsTimer)[i]->WriteToParcel(out, uSecRealtime);
    }
    mPhoneSignalScanningTimer->WriteToParcel(out, uSecRealtime);
    for (Int32 i = 0; i < IBatteryStats::NUM_DATA_CONNECTION_TYPES; i++) {
        (*mPhoneDataConnectionsTimer)[i]->WriteToParcel(out, uSecRealtime);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_NETWORK_ACTIVITY_TYPES; i++) {
        (*mNetworkByteActivityCounters)[i]->WriteToParcel(out);
        (*mNetworkPacketActivityCounters)[i]->WriteToParcel(out);
    }
    mMobileRadioActiveTimer->WriteToParcel(out, uSecRealtime);
    mMobileRadioActivePerAppTimer->WriteToParcel(out, uSecRealtime);
    mMobileRadioActiveAdjustedTime->WriteToParcel(out);
    mMobileRadioActiveUnknownTime->WriteToParcel(out);
    mMobileRadioActiveUnknownCount->WriteToParcel(out);
    mWifiOnTimer->WriteToParcel(out, uSecRealtime);
    mGlobalWifiRunningTimer->WriteToParcel(out, uSecRealtime);
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_STATES; i++) {
        (*mWifiStateTimer)[i]->WriteToParcel(out, uSecRealtime);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SUPPL_STATES; i++) {
        (*mWifiSupplStateTimer)[i]->WriteToParcel(out, uSecRealtime);
    }
    for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
        (*mWifiSignalStrengthsTimer)[i]->WriteToParcel(out, uSecRealtime);
    }
    mBluetoothOnTimer->WriteToParcel(out, uSecRealtime);
    for (Int32 i = 0; i < IBatteryStats:: NUM_BLUETOOTH_STATES; i++) {
        (*mBluetoothStateTimer)[i]->WriteToParcel(out, uSecRealtime);
    }
    mFlashlightOnTimer->WriteToParcel(out, uSecRealtime);
    out->WriteInt32(mDischargeUnplugLevel);
    out->WriteInt32(mDischargePlugLevel);
    out->WriteInt32(mDischargeCurrentLevel);
    out->WriteInt32(mCurrentBatteryLevel);
    out->WriteInt32(mLowDischargeAmountSinceCharge);
    out->WriteInt32(mHighDischargeAmountSinceCharge);
    out->WriteInt32(mDischargeAmountScreenOn);
    out->WriteInt32(mDischargeAmountScreenOnSinceCharge);
    out->WriteInt32(mDischargeAmountScreenOff);
    out->WriteInt32(mDischargeAmountScreenOffSinceCharge);
    out->WriteInt32(mNumDischargeStepDurations);
    out->WriteArrayOf((Handle32)mDischargeStepDurations.Get());
    out->WriteInt32(mNumChargeStepDurations);
    out->WriteArrayOf((Handle32)mChargeStepDurations.Get());
    out->WriteInt64(mLastWriteTime);

    Int32 count;
    GetBluetoothPingCount(&count);
    out->WriteInt32(count);

    if (inclUids) {
        Int32 size;
        mKernelWakelockStats->GetSize(&size);
        out->WriteInt32(size);
        AutoPtr<ISet> set;
        mKernelWakelockStats->GetEntrySet((ISet**)&set);
        AutoPtr<IIterator> it;
        set->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            it->GetNext((IInterface**)&next);
            AutoPtr<IMapEntry> ent = IMapEntry::Probe(next);
            AutoPtr<IInterface> value;
            ent->GetValue((IInterface**)&value);
            if (value != NULL) {
                AutoPtr<SamplingTimer> kwlt = (SamplingTimer*)(IObject*)value.Get();
                out->WriteInt32(1);
                AutoPtr<IInterface> ki;
                ent->GetKey((IInterface**)&ki);
                String key;
                ICharSequence::Probe(ki)->ToString(&key);
                out->WriteString(key);
                kwlt->WriteToParcel(out, uSecRealtime);
            }
            else {
                out->WriteInt32(0);
            }
        }

        mWakeupReasonStats->GetSize(&size);
        out->WriteInt32(size);
        AutoPtr<ISet> statsSet;
        mWakeupReasonStats->GetEntrySet((ISet**)&statsSet);
        AutoPtr<IIterator> statsIt;
        statsSet->GetIterator((IIterator**)&statsIt);
        while (statsIt->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> next;
            statsIt->GetNext((IInterface**)&next);
            AutoPtr<IMapEntry> ent = IMapEntry::Probe(next);
            AutoPtr<IInterface> value;
            ent->GetValue((IInterface**)&value);
            if (value != NULL) {
                AutoPtr<SamplingTimer> timer = (SamplingTimer*)(IObject*)value.Get();
                out->WriteInt32(1);
                AutoPtr<IInterface> ki;
                ent->GetKey((IInterface**)&ki);
                String key;
                ICharSequence::Probe(ki)->ToString(&key);
                out->WriteString(key);
                timer->WriteToParcel(out, uSecRealtime);
            }
            else {
                out->WriteInt32(0);
            }
        }
    }
    else {
        out->WriteInt32(0);
    }

    out->WriteInt32(sNumSpeedSteps);

    if (inclUids) {
        Int32 size;
        mUidStats->GetSize(&size);
        out->WriteInt32(size);
        for (Int32 i = 0; i < size; i++) {
            Int32 key;
            mUidStats->KeyAt(i, &key);
            out->WriteInt32(key);
            AutoPtr<IInterface> value;
            mUidStats->ValueAt(i, (IInterface**)&value);
            AutoPtr<Uid> uid = (Uid*)(IObject*)value.Get();

            uid->WriteToParcelLocked(out, uSecRealtime);
        }
    }
    else {
        out->WriteInt32(0);
    }
}

void BatteryStatsImpl::PrepareForDumpLocked()
{
    // Need to retrieve current kernel wake lock stats before printing.
    PullPendingStateUpdatesLocked();

    // Pull the clock time.  This may update the time and make a new history entry
    // if we had originally pulled a time before the RTC was set.
    Int64 time;
    GetStartClockTime(&time);
}

void BatteryStatsImpl::DumpLocked(
    /* [in] */ IContext* context,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ Int32 flags,
    /* [in] */ Int32 reqUid,
    /* [in] */ Int64 histStart)
{
    if (DEBUG) {
        pw->Println(String("mOnBatteryTimeBase:"));
        mOnBatteryTimeBase->Dump(pw, String("  "));
        pw->Println(String("mOnBatteryScreenOffTimeBase:"));
        mOnBatteryScreenOffTimeBase->Dump(pw, String("  "));
        AutoPtr<IPrinter> pr;
        assert(0);
        // CPrintWriterPrinter::New(pw, (IPrinter**)&pr);
        pr->Println(String("*** Screen timer:"));
        mScreenOnTimer->LogState(pr, String("  "));
        for (Int32 i = 0; i < NUM_SCREEN_BRIGHTNESS_BINS; i++) {
            StringBuilder sb("*** Screen brightness #");
            sb += i;
            sb += ":";
            pr->Println(sb.ToString());
            (*mScreenBrightnessTimer)[i]->LogState(pr, String("  "));
        }
        pr->Println(String("*** Interactive timer:"));
        mInteractiveTimer->LogState(pr, String("  "));
        pr->Println(String("*** Low power mode timer:"));
        mLowPowerModeEnabledTimer->LogState(pr, String("  "));
        pr->Println(String("*** Phone timer:"));
        mPhoneOnTimer->LogState(pr, String("  "));
        for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
            StringBuilder sb("*** Phone signal strength #");
            sb += i;
            sb += ":";
            pr->Println(sb.ToString());
            (*mPhoneSignalStrengthsTimer)[i]->LogState(pr, String("  "));
        }
        pr->Println(String("*** Signal scanning :"));
        mPhoneSignalScanningTimer->LogState(pr, String("  "));
        for (Int32 i = 0; i < NUM_DATA_CONNECTION_TYPES; i++) {
            StringBuilder sb("*** Data connection type #");
            sb += i;
            sb += ":";
            pr->Println(sb.ToString());
            (*mPhoneDataConnectionsTimer)[i]->LogState(pr, String("  "));
        }
        pr->Println(String("*** mMobileRadioPowerState=") + StringUtils::ToString(mMobileRadioPowerState));
        pr->Println(String("*** Mobile network active timer:"));
        mMobileRadioActiveTimer->LogState(pr, String("  "));
        pr->Println(String("*** Mobile network active adjusted timer:"));
        mMobileRadioActiveAdjustedTime->LogState(pr, String("  "));
        pr->Println(String("*** Wifi timer:"));
        mWifiOnTimer->LogState(pr, String("  "));
        pr->Println(String("*** WifiRunning timer:"));
        mGlobalWifiRunningTimer->LogState(pr, String("  "));
        for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_STATES; i++) {
            pr->Println(String("*** Wifi state #") + StringUtils::ToString(i) + ":");
            (*mWifiStateTimer)[i]->LogState(pr, String("  "));
        }
        for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SUPPL_STATES; i++) {
            pr->Println(String("*** Wifi suppl state #") + StringUtils::ToString(i) + ":");
            (*mWifiSupplStateTimer)[i]->LogState(pr, String("  "));
        }
        for (Int32 i = 0; i < IBatteryStats::NUM_WIFI_SIGNAL_STRENGTH_BINS; i++) {
            pr->Println(String("*** Wifi signal strength #") + StringUtils::ToString(i) + ":");
            (*mWifiSignalStrengthsTimer)[i]->LogState(pr, String("  "));
        }
        pr->Println(String("*** Bluetooth timer:"));
        mBluetoothOnTimer->LogState(pr, String("  "));
        for (Int32 i = 0; i < IBatteryStats:: NUM_BLUETOOTH_STATES; i++) {
            pr->Println(String("*** Bluetooth active type #") + StringUtils::ToString(i) + ":");
            (*mBluetoothStateTimer)[i]->LogState(pr, String("  "));
        }
        pr->Println(String("*** Flashlight timer:"));
        mFlashlightOnTimer->LogState(pr, String("  "));
    }
    BatteryStats::DumpLocked(context, pw, flags, reqUid, histStart);
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
