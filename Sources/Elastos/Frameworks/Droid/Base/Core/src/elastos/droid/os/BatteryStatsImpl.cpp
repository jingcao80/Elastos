
#include "elastos/droid/os/BatteryStatsImpl.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/FileUtils.h"
#include "elastos/droid/utility/TimeUtils.h"
#include "elastos/droid/os/Handler.h"
#ifdef DROID_CORE
#include "elastos/droid/utility/CAtomicFile.h"
#include "elastos/droid/net/CNetworkStatsFactory.h"
#include "elastos/droid/net/CConnectivityManagerHelper.h"
#include "elastos/droid/net/CNetworkStats.h"
#include "elastos/droid/os/CSystemProperties.h"
#endif
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>


using Elastos::Core::ICharSequence;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::ICloseable;
using Elastos::IO::IStringWriter;
using Elastos::IO::CStringWriter;
using Elastos::IO::CPrintWriter;
using Elastos::IO::IFlushable;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Bluetooth::IBluetoothDevice;
using Elastos::Droid::Net::IConnectivityManagerHelper;
using Elastos::Droid::Net::CConnectivityManagerHelper;
using Elastos::Droid::Net::INetworkStatsEntry;
using Elastos::Droid::Os::FileUtils;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Utility::CAtomicFile;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::Internal::Net::CNetworkStatsFactory;
using Elastos::Droid::Net::CNetworkStats;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Text::Format::IDateUtils;

namespace Elastos {
namespace Droid {
namespace Os {

const Int32 BatteryStatsImpl::MSG_UPDATE_WAKELOCKS = 1;
const Int32 BatteryStatsImpl::MSG_REPORT_POWER_CHANGE = 2;

//==============================================================================
// BatteryStatsImpl::MyHandler
//==============================================================================

ECode BatteryStatsImpl::MyHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);

    BatteryCallback* cb = mHost->mCallback;
    switch(what) {
        case BatteryStatsImpl::MSG_UPDATE_WAKELOCKS:
            if (cb != NULL) {
                cb->BatteryNeedsCpuUpdate();
            }
            break;
        case BatteryStatsImpl::MSG_REPORT_POWER_CHANGE:
            if (cb != NULL) {
                cb->BatteryPowerChanged(arg1 != 0);
            }
            break;
    }

    return NOERROR;
}

//==============================================================================
// Counter
//==============================================================================
BatteryStatsImpl::Counter::Counter(
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
    /* [in] */ IParcel* in)
{
    CAtomicInteger32::New((IAtomicInteger32**)&mCount);
    mUnpluggables = unpluggables;
    in->ReadInt32(&mPluggedCount);
    mCount->Set(mPluggedCount);
    in->ReadInt32(&mLoadedCount);
    mLastCount = 0;
    in->ReadInt32(&mUnpluggedCount);
    unpluggables->PushBack(this);
}

BatteryStatsImpl::Counter::Counter(
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables)
    : mUnpluggables(unpluggables)
    , mLoadedCount(0)
    , mLastCount(0)
    , mUnpluggedCount(0)
    , mPluggedCount(0)
{
    CAtomicInteger32::New((IAtomicInteger32**)&mCount);
    unpluggables->PushBack(this);
}

BatteryStatsImpl::Counter::~Counter()
{
    if (mUnpluggables != NULL) {
        mUnpluggables->Clear();
        mUnpluggables = NULL;
    }
}

PInterface BatteryStatsImpl::Counter::Probe(
    /* [in]  */ REIID riid)
{
    return NULL;
}

UInt32 BatteryStatsImpl::Counter::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BatteryStatsImpl::Counter::Release()
{
    return ElRefBase::Release();
}

ECode BatteryStatsImpl::Counter::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return  NOERROR;
}

void BatteryStatsImpl::Counter::WriteToParcel(
    /* [in] */ IParcel* out)
{
    Int32 value;
    mCount->Get(&value);
    out->WriteInt32(value);
    out->WriteInt32(mLoadedCount);
    out->WriteInt32(mUnpluggedCount);
}

void BatteryStatsImpl::Counter::Unplug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    mUnpluggedCount = mPluggedCount;
    mCount->Set(mPluggedCount);
}

void BatteryStatsImpl::Counter::Plug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    mCount->Get(&mPluggedCount);
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

Int32 BatteryStatsImpl::Counter::GetCountLocked(
    /* [in] */ Int32 which)
{
    Int32 val;
    if (which == STATS_LAST) {
        val = mLastCount;
    }
    else {
        mCount->Get(&val);
        if (which == STATS_SINCE_UNPLUGGED) {
            val -= mUnpluggedCount;
        }
        else if (which != STATS_SINCE_CHARGED) {
            val -= mLoadedCount;
        }
    }

    return val;
}

void BatteryStatsImpl::Counter::LogState(
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
    List<AutoPtr<IUnpluggable> >::Iterator it = mUnpluggables->Begin();
    for (; it != mUnpluggables->End(); ++it) {
        if ((*it).Get() == (IUnpluggable*)this) {
            mUnpluggables->Erase(it);
            break;
        }
    }
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
// SamplingCounter
//==============================================================================
BatteryStatsImpl::SamplingCounter::SamplingCounter(
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
    /* [in] */ IParcel* in)
    : Counter(unpluggables, in)
{}

BatteryStatsImpl::SamplingCounter::SamplingCounter(
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables)
    : Counter(unpluggables)
{}

void BatteryStatsImpl::SamplingCounter::AddCountAtomic(
    /* [in] */ Int64 count)
{
    Int32 result;
    mCount->AddAndGet((Int32)count, &result);
}


//==============================================================================
// Timer
//==============================================================================
BatteryStatsImpl::Timer::Timer(
    /* [in] */ Int32 type,
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
    /* [in] */ IParcel* in)
{
    mType = type;
    mUnpluggables = unpluggables;

    in->ReadInt32(&mCount);
    in->ReadInt32(&mLoadedCount);
    mLastCount = 0;
    in->ReadInt32(&mUnpluggedCount);
    in->ReadInt64(&mTotalTime);
    in->ReadInt64(&mLoadedTime);
    mLastTime = 0;
    in->ReadInt64(&mUnpluggedTime);
    unpluggables->PushBack(this);
}

BatteryStatsImpl::Timer::Timer(
    /* [in] */ Int32 type,
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables)
    : mType(type)
    , mUnpluggables(unpluggables)
    , mCount(0)
    , mLoadedCount(0)
    , mLastCount(0)
    , mUnpluggedCount(0)
    , mTotalTime(0)
    , mLoadedTime(0)
    , mLastTime(0)
    , mUnpluggedTime(0)
{
    unpluggables->PushBack(this);
}

BatteryStatsImpl::Timer::~Timer()
{
    if (mUnpluggables != NULL) {
        mUnpluggables->Clear();
        mUnpluggables = NULL;
    }
}

PInterface BatteryStatsImpl::Timer::Probe(
    /* [in]  */ REIID riid)
{
    return NULL;
}

UInt32 BatteryStatsImpl::Timer::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BatteryStatsImpl::Timer::Release()
{
    return ElRefBase::Release();
}

ECode BatteryStatsImpl::Timer::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return  NOERROR;
}

Boolean BatteryStatsImpl::Timer::Reset(
    /* [in] */ BatteryStatsImpl* stats,
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
    List<AutoPtr<IUnpluggable> >::Iterator it = mUnpluggables->Begin();
    for (; it != mUnpluggables->End(); ++it) {
        if ((*it).Get() == (IUnpluggable*)this) {
            mUnpluggables->Erase(it);
            break;
        }
    }
}

void BatteryStatsImpl::Timer::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 batteryRealtime)
{
    out->WriteInt32(mCount);
    out->WriteInt32(mLoadedCount);
    out->WriteInt32(mUnpluggedCount);
    out->WriteInt64(ComputeRunTimeLocked(batteryRealtime));
    out->WriteInt64(mLoadedTime);
    out->WriteInt64(mUnpluggedTime);
}

void BatteryStatsImpl::Timer::Unplug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    if (DEBUG && mType < 0) {
        StringBuilder sb("unplug #");
        sb += mType;
        sb += ": realtime=";
        sb += batteryRealtime;
        sb += " old mUnpluggedTime=";
        sb += mUnpluggedTime;
        sb += " old mUnpluggedCount=";
        sb += mUnpluggedCount;
        Logger::V(TAG, sb.ToString());
    }
    mUnpluggedTime = ComputeRunTimeLocked(batteryRealtime);
    mUnpluggedCount = mCount;
    if (DEBUG && mType < 0) {
        StringBuilder sb("unplug #");
        sb += mType;
        sb += " new mUnpluggedTime=";
        sb += mUnpluggedTime;
        sb += " new mUnpluggedCount=";
        sb += mUnpluggedCount;
        Logger::V(TAG, sb.ToString());
    }
}

void BatteryStatsImpl::Timer::Plug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    if (DEBUG && mType < 0) {
        StringBuilder sb("plug #");
        sb += mType;
        sb += ": realtime=";
        sb += batteryRealtime;
        sb += " old mTotalTime=";
        sb += mTotalTime;
        Logger::V(TAG, sb.ToString());
    }
    mTotalTime = ComputeRunTimeLocked(batteryRealtime);
    mCount = ComputeCurrentCountLocked();
    if (DEBUG && mType < 0) {
        Logger::V(TAG, "plug #%d: new mTotalTime=%d", mType, mTotalTime);
    }
}

void BatteryStatsImpl::Timer::WriteTimerToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Timer* timer,
    /* [in] */ Int64 batteryRealtime)
{
    if (timer == NULL) {
        out->WriteInt32(0); // indicates null
        return;
    }
    out->WriteInt32(1); // indicates non-null

    timer->WriteToParcel(out, batteryRealtime);
}

Int64 BatteryStatsImpl::Timer::GetTotalTimeLocked(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    Int64 val;
    if (which == STATS_LAST) {
        val = mLastTime;
    }
    else {
        val = ComputeRunTimeLocked(batteryRealtime);
        if (which == STATS_SINCE_UNPLUGGED) {
            val -= mUnpluggedTime;
        }
        else if (which != STATS_SINCE_CHARGED) {
            val -= mLoadedTime;
        }
    }

    return val;
}

Int32 BatteryStatsImpl::Timer::GetCountLocked(
    /* [in] */ Int32 which)
{
    Int32 val;
    if (which == STATS_LAST) {
        val = mLastCount;
    }
    else {
        val = ComputeCurrentCountLocked();
        if (which == STATS_SINCE_UNPLUGGED) {
            val -= mUnpluggedCount;
        }
        else if (which != STATS_SINCE_CHARGED) {
            val -= mLoadedCount;
        }
    }

    return val;
}

void BatteryStatsImpl::Timer::LogState(
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
}

void BatteryStatsImpl::Timer::WriteSummaryFromParcelLocked(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 batteryRealtime)
{
    Int64 runTime = ComputeRunTimeLocked(batteryRealtime);
    // Divide by 1000 for backwards compatibility
    out->WriteInt64((runTime + 500) / 1000);
    out->WriteInt32(mCount);
}

void BatteryStatsImpl::Timer::ReadSummaryFromParcelLocked(
    /* [in] */ IParcel* in)
{
    // Multiply by 1000 for backwards compatibility
    Int64 value;
    in->ReadInt64(&value);
    mTotalTime = mLoadedTime = value * 1000;
    mLastTime = 0;
    mUnpluggedTime = mTotalTime;
    Int32 value1;
    in->ReadInt32(&value1);
    mCount = mLoadedCount = value1;
    mLastCount = 0;
    mUnpluggedCount = mCount;
}


//==============================================================================
// SamplingTimer
//==============================================================================
BatteryStatsImpl::SamplingTimer::SamplingTimer(
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
    /* [in] */ Boolean inDischarge,
    /* [in] */ IParcel* in)
    : Timer(0, unpluggables, in)
    , mUpdateVersion(0)
{
    in->ReadInt32(&mCurrentReportedCount);
    in->ReadInt32(&mUnpluggedReportedCount);
    in->ReadInt64(&mCurrentReportedTotalTime);
    in->ReadInt64(&mUnpluggedReportedTotalTime);
    Int32 value;
    in->ReadInt32(&value);
    mTrackingReportedValues = value == 1;
    mInDischarge = inDischarge;
}

BatteryStatsImpl::SamplingTimer::SamplingTimer(
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
    /* [in] */ Boolean inDischarge,
    /* [in] */ Boolean trackReportedValues)
    : Timer(0, unpluggables)
    , mCurrentReportedCount(0)
    , mUnpluggedReportedCount(0)
    , mCurrentReportedTotalTime(0)
    , mUnpluggedReportedTotalTime(0)
    , mInDischarge(inDischarge)
    , mTrackingReportedValues(trackReportedValues)
    , mUpdateVersion(0)
{
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
    if (mInDischarge && mUnpluggedReportedCount == 0) {
        // Updating the reported value for the first time.
        mUnpluggedReportedCount = count;
        // If we are receiving an update update mTrackingReportedValues;
        mTrackingReportedValues = TRUE;
    }
    mCurrentReportedCount = count;
}

void BatteryStatsImpl::SamplingTimer::UpdateCurrentReportedTotalTime(
    /* [in] */ Int64 totalTime)
{
    if (mInDischarge && mUnpluggedReportedTotalTime == 0) {
        // Updating the reported value for the first time.
        mUnpluggedReportedTotalTime = totalTime;
        // If we are receiving an update update mTrackingReportedValues;
        mTrackingReportedValues = TRUE;
    }
    mCurrentReportedTotalTime = totalTime;
}

void BatteryStatsImpl::SamplingTimer::Unplug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    Timer::Unplug(batteryUptime, batteryRealtime);
    if (mTrackingReportedValues) {
        mUnpluggedReportedTotalTime = mCurrentReportedTotalTime;
        mUnpluggedReportedCount = mCurrentReportedCount;
    }
    mInDischarge = TRUE;
}

void BatteryStatsImpl::SamplingTimer::Plug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    Timer::Plug(batteryUptime, batteryRealtime);
    mInDischarge = FALSE;
}

void BatteryStatsImpl::SamplingTimer::LogState(
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
}

Int64 BatteryStatsImpl::SamplingTimer::ComputeRunTimeLocked(
    /* [in] */ Int64 curBatteryRealtime)
{
    return mTotalTime + (mInDischarge && mTrackingReportedValues
            ? mCurrentReportedTotalTime - mUnpluggedReportedTotalTime : 0);
}

Int32 BatteryStatsImpl::SamplingTimer::ComputeCurrentCountLocked()
{
    return mCount + (mInDischarge && mTrackingReportedValues
            ? mCurrentReportedCount - mUnpluggedReportedCount : 0);
}

void BatteryStatsImpl::SamplingTimer::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 batteryRealtime)
{
    Timer::WriteToParcel(out, batteryRealtime);
    out->WriteInt32(mCurrentReportedCount);
    out->WriteInt32(mUnpluggedReportedCount);
    out->WriteInt64(mCurrentReportedTotalTime);
    out->WriteInt64(mUnpluggedReportedTotalTime);
    out->WriteInt32(mTrackingReportedValues ? 1 : 0);
}

Boolean BatteryStatsImpl::SamplingTimer::Reset(
    /* [in] */ BatteryStatsImpl* stats,
    /* [in] */ Boolean detachIfReset)
{
    Timer::Reset(stats, detachIfReset);
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
// StopwatchTimer
//==============================================================================
BatteryStatsImpl::StopwatchTimer::StopwatchTimer(
    /* [in] */ Uid* uid,
    /* [in] */ Int32 type,
    /* [in] */ List<AutoPtr<StopwatchTimer> >* timerPool,
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
    /* [in] */ IParcel* in)
    : Timer(type, unpluggables, in)
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
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables)
    : Timer(type, unpluggables)
    , mUid(uid)
    , mTimerPool(timerPool)
    , mNesting(0)
    , mUpdateTime(0)
    , mAcquireTime(0)
    , mTimeout(0)
    , mInList(FALSE)
{}

BatteryStatsImpl::StopwatchTimer::~StopwatchTimer()
{
    if (mTimerPool != NULL) {
        mTimerPool->Clear();
        mTimerPool = NULL;
    }
}

void BatteryStatsImpl::StopwatchTimer::SetTimeout(
    /* [in] */ Int64 timeout)
{
    mTimeout = timeout;
}

void BatteryStatsImpl::StopwatchTimer::WriteToParcel(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 batteryRealtime)
{
    Timer::WriteToParcel(out, batteryRealtime);
    out->WriteInt64(mUpdateTime);
}

void BatteryStatsImpl::StopwatchTimer::Plug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    if (mNesting > 0) {
        if (DEBUG && mType < 0) {
            Logger::V(TAG, "old mUpdateTime=%d", mUpdateTime);
        }
        Timer::Plug(batteryUptime, batteryRealtime);
        mUpdateTime = batteryRealtime;
        if (DEBUG && mType < 0) {
            Logger::V(TAG, "new mUpdateTime=%d", mUpdateTime);
        }
    }
}

void BatteryStatsImpl::StopwatchTimer::LogState(
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
}

void BatteryStatsImpl::StopwatchTimer::StartRunningLocked(
    /* [in] */ BatteryStatsImpl* stats)
{
    if (mNesting++ == 0) {
        mUpdateTime = stats->GetBatteryRealtimeLocked(SystemClock::GetElapsedRealtime() * 1000);
        if (mTimerPool != NULL) {
            // Accumulate time to all currently active timers before adding
            // this new one to the pool.
            RefreshTimersLocked(stats, mTimerPool);
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

void BatteryStatsImpl::StopwatchTimer::StopRunningLocked(
    /* [in] */ BatteryStatsImpl* stats)
{
    // Ignore attempt to stop a timer that isn't running
    if (mNesting == 0) {
        return;
    }
    if (--mNesting == 0) {
        if (mTimerPool != NULL) {
            // Accumulate time to all active counters, scaled by the total
            // active in the pool, before taking this one out of the pool.
            RefreshTimersLocked(stats, mTimerPool);
            // Remove this timer from the active pool
            List<AutoPtr<StopwatchTimer> >::Iterator it = mTimerPool->Begin();
            for (; it != mTimerPool->End(); ++it) {
                if ((*it).Get() == this) {
                    mTimerPool->Erase(it);
                    break;
                }
            }
        }
        else {
            Int64 realtime = SystemClock::GetElapsedRealtime() * 1000;
            Int64 batteryRealtime = stats->GetBatteryRealtimeLocked(realtime);
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

void BatteryStatsImpl::StopwatchTimer::RefreshTimersLocked(
    /* [in] */ BatteryStatsImpl* stats,
    /* [in] */ List<AutoPtr<StopwatchTimer> >* pool)
{
    Int64 realtime = SystemClock::GetElapsedRealtime() * 1000;
    Int64 batteryRealtime = stats->GetBatteryRealtimeLocked(realtime);
    List<AutoPtr<StopwatchTimer> >::Iterator it = pool->Begin();
    Int32 N = pool->GetSize();
    for (; it != pool->End(); ++it) {
        AutoPtr<StopwatchTimer> t = *it;
        Int64 heldTime = batteryRealtime - t->mUpdateTime;
        if (heldTime > 0) {
            t->mTotalTime += heldTime / N;
        }
        t->mUpdateTime = batteryRealtime;
    }
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
    /* [in] */ BatteryStatsImpl* stats,
    /* [in] */ Boolean detachIfReset)
{
    Boolean canDetach = mNesting <= 0;
    Timer::Reset(stats, canDetach && detachIfReset);
    if (mNesting > 0) {
        mUpdateTime = stats->GetBatteryRealtimeLocked(
                SystemClock::GetElapsedRealtime() * 1000);
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
// Uid::Wakelock
//==============================================================================
AutoPtr<BatteryStatsImpl::StopwatchTimer> BatteryStatsImpl::Uid::Wakelock::ReadTimerFromParcel(
    /* [in] */ Int32 type,
    /* [in] */ List<AutoPtr<StopwatchTimer> >* pool,
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
    /* [in] */ IParcel* in)
{
    Int32 value;
    if (in->ReadInt32(&value), value == 0) {
        return NULL;
    }

    return new BatteryStatsImpl::StopwatchTimer(mHost, type, pool, unpluggables, in);
}

Boolean BatteryStatsImpl::Uid::Wakelock::Reset()
{
    Boolean wlactive = FALSE;
    if (mTimerFull != NULL) {
        wlactive |= !mTimerFull->Reset(mHost->mHost, FALSE);
    }
    if (mTimerPartial != NULL) {
        // wlactive |= !mTimerPartial.reset(mHost->mHost, FALSE);
    }
    if (mTimerWindow != NULL) {
        // wlactive |= !mTimerWindow.reset(mHost->mHost, FALSE);
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
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
    /* [in] */ IParcel* in)
{
    mTimerPartial = ReadTimerFromParcel(WAKE_TYPE_PARTIAL,
            &mHost->mHost->mPartialTimers, unpluggables, in);
    mTimerFull = ReadTimerFromParcel(WAKE_TYPE_FULL,
            &mHost->mHost->mFullTimers, unpluggables, in);
    mTimerWindow = ReadTimerFromParcel(WAKE_TYPE_WINDOW,
            &mHost->mHost->mWindowTimers, unpluggables, in);
}

void BatteryStatsImpl::Uid::Wakelock::WriteToParcelLocked(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 batteryRealtime)
{
    BatteryStatsImpl::Timer::WriteTimerToParcel(out, mTimerPartial, batteryRealtime);
    BatteryStatsImpl::Timer::WriteTimerToParcel(out, mTimerFull, batteryRealtime);
    BatteryStatsImpl::Timer::WriteTimerToParcel(out, mTimerWindow, batteryRealtime);
}

ECode BatteryStatsImpl::Uid::Wakelock::GetWakeTime(
    /* [in] */ Int32 type,
    /* [out] */ BatteryStatsTimer** timer)
{
    VALIDATE_NOT_NULL(timer);
    switch (type) {
        case WAKE_TYPE_FULL: *timer = mTimerFull;
        case WAKE_TYPE_PARTIAL: *timer = mTimerPartial;
        case WAKE_TYPE_WINDOW: *timer = mTimerWindow;
        default:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("type = " + type);
    }
    REFCOUNT_ADD(*timer);
    return NOERROR;
}


//==============================================================================
// Uid::Sensor
//==============================================================================
AutoPtr<BatteryStatsImpl::StopwatchTimer> BatteryStatsImpl::Uid::Sensor::ReadTimerFromParcel(
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
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
    return new BatteryStatsImpl::StopwatchTimer(mHost, 0, pool, unpluggables, in);
}

Boolean BatteryStatsImpl::Uid::Sensor::Reset()
{
    if (mTimer->Reset(mHost->mHost, TRUE)) {
        mTimer = NULL;
        return TRUE;
    }
    return FALSE;
}

void BatteryStatsImpl::Uid::Sensor::ReadFromParcelLocked(
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
    /* [in] */ IParcel* in)
{
    mTimer = ReadTimerFromParcel(unpluggables, in);
}

void BatteryStatsImpl::Uid::Sensor::WriteToParcelLocked(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 batteryRealtime)
{
    BatteryStatsImpl::Timer::WriteTimerToParcel(out, mTimer, batteryRealtime);
}

AutoPtr<BatteryStatsImpl::BatteryStatsTimer> BatteryStatsImpl::Uid::Sensor::GetSensorTime()
{
    return mTimer;
}

Int32 BatteryStatsImpl::Uid::Sensor::GetHandle()
{
    return mHandle;
}


//==============================================================================
// Uid::Proc
//==============================================================================
BatteryStatsImpl::Uid::Proc::Proc(
    /* [in] */ Uid* host)
    : mUserTime(0)
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
    , mUnpluggedStarts(0)
    , mUnpluggedForegroundTime(0)
    , mHost(host)
{
    mHost->mHost->mUnpluggables.PushBack(this);
    mSpeedBins = ArrayOf<SamplingCounter*>::Alloc(mHost->mHost->GetCpuSpeedSteps());
}

BatteryStatsImpl::Uid::Proc::~Proc()
{
    mExcessivePower.Clear();
}

PInterface BatteryStatsImpl::Uid::Proc::Probe(
    /* [in]  */ REIID riid)
{
    return NULL;
}

UInt32 BatteryStatsImpl::Uid::Proc::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BatteryStatsImpl::Uid::Proc::Release()
{
    return ElRefBase::Release();
}

ECode BatteryStatsImpl::Uid::Proc::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return  NOERROR;
}

void BatteryStatsImpl::Uid::Proc::Unplug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    mUnpluggedUserTime = mUserTime;
    mUnpluggedSystemTime = mSystemTime;
    mUnpluggedStarts = mStarts;
    mUnpluggedForegroundTime = mForegroundTime;
}

void BatteryStatsImpl::Uid::Proc::Plug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{}

void BatteryStatsImpl::Uid::Proc::Detach()
{
    List<AutoPtr<IUnpluggable> >::Iterator it = mHost->mHost->mUnpluggables.Begin();
    for (; it != mHost->mHost->mUnpluggables.End(); ++it) {
        if ((*it).Get() == (IUnpluggable*)this) {
            mHost->mHost->mUnpluggables.Erase(it);
            break;
        }
    }
    for (Int32 i = 0; i < mSpeedBins->GetLength(); i++) {
        AutoPtr<SamplingCounter> c = (*mSpeedBins)[i];
        if (c != NULL) {
            it = mHost->mHost->mUnpluggables.Begin();
            for (; it != mHost->mHost->mUnpluggables.End(); ++it) {
                if ((*it).Get() == (IUnpluggable*)c) {
                    mHost->mHost->mUnpluggables.Erase(it);
                    break;
                }
            }
            mSpeedBins->Set(i, NULL);
        }
    }
}

Int32 BatteryStatsImpl::Uid::Proc::CountExcessivePowers()
{
    return mExcessivePower.GetSize();
}

AutoPtr<BatteryStatsImpl::Uid::Proc::ExcessivePower> BatteryStatsImpl::Uid::Proc::GetExcessivePower(
    /* [in] */ Int32 i)
{
    if (mExcessivePower.Begin() != mExcessivePower.End()) {
        return mExcessivePower[i];
    }
    return NULL;
}

void BatteryStatsImpl::Uid::Proc::AddExcessiveWake(
    /* [in] */ Int64 overTime,
    /* [in] */ Int64 usedTime)
{
    AutoPtr<ExcessivePower> ew = new ExcessivePower();
    ew->mType = ExcessivePower::TYPE_WAKE;
    ew->mOverTime = overTime;
    ew->mUsedTime = usedTime;
    mExcessivePower.PushBack(ew);
}

void BatteryStatsImpl::Uid::Proc::AddExcessiveCpu(
    /* [in] */ Int64 overTime,
    /* [in] */ Int64 usedTime)
{
    AutoPtr<ExcessivePower> ew = new ExcessivePower();
    ew->mType = ExcessivePower::TYPE_CPU;
    ew->mOverTime = overTime;
    ew->mUsedTime = usedTime;
    mExcessivePower.PushBack(ew);
}

void BatteryStatsImpl::Uid::Proc::WriteExcessivePowerToParcelLocked(
    /* [in] */ IParcel* out)
{
    if (mExcessivePower.Begin() == mExcessivePower.End()) {
        out->WriteInt32(0);
        return;
    }

    Int32 N = mExcessivePower.GetSize();
    out->WriteInt32(N);
    List<AutoPtr<ExcessivePower> >::Iterator it = mExcessivePower.Begin();
    for (; it != mExcessivePower.End(); ++it) {
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
        mExcessivePower.Clear();
        return TRUE;
    }

    if (N > 10000) {
        Logger::W(TAG, "File corrupt: too many excessive power entries %d", N);
        return FALSE;
    }

    mExcessivePower.Clear();
    for (Int32 i = 0; i < N; i++) {
        AutoPtr<ExcessivePower> ew = new ExcessivePower();
        in->ReadInt32(&ew->mType);
        in->ReadInt64(&ew->mOverTime);
        in->ReadInt64(&ew->mUsedTime);
        mExcessivePower.PushBack(ew);
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
    Int32 steps = mHost->mHost->GetCpuSpeedSteps();
    mSpeedBins = ArrayOf<SamplingCounter*>::Alloc(bins >= steps ? bins : steps);
    for (Int32 i = 0; i < bins; i++) {
        Int32 value;
        if (in->ReadInt32(&value), value != 0) {
            AutoPtr<SamplingCounter> counter = new SamplingCounter(&mHost->mHost->mUnpluggables, in);
            mSpeedBins->Set(i, counter);
        }
    }

    ReadExcessivePowerFromParcelLocked(in);
}

AutoPtr<BatteryStatsImpl> BatteryStatsImpl::Uid::Proc::GetBatteryStats()
{
    return mHost->mHost;
}

void BatteryStatsImpl::Uid::Proc::AddCpuTimeLocked(
    /* [in] */ Int32 utime,
    /* [in] */ Int32 stime)
{
    mUserTime += utime;
    mSystemTime += stime;
}

void BatteryStatsImpl::Uid::Proc::AddForegroundTimeLocked(
    /* [in] */ Int64 ttime)
{
    mForegroundTime += ttime;
}

void BatteryStatsImpl::Uid::Proc::IncStartsLocked()
{
    mStarts++;
}

Int64 BatteryStatsImpl::Uid::Proc::GetUserTime(
    /* [in] */ Int32 which)
{
    Int64 val;
    if (which == STATS_LAST) {
        val = mLastUserTime;
    }
    else {
        val = mUserTime;
        if (which == STATS_CURRENT) {
            val -= mLoadedUserTime;
        }
        else if (which == STATS_SINCE_UNPLUGGED) {
            val -= mUnpluggedUserTime;
        }
    }
    return val;
}

Int64 BatteryStatsImpl::Uid::Proc::GetSystemTime(
    /* [in] */ Int32 which)
{
    Int64 val;
    if (which == STATS_LAST) {
        val = mLastSystemTime;
    }
    else {
        val = mSystemTime;
        if (which == STATS_CURRENT) {
            val -= mLoadedSystemTime;
        }
        else if (which == STATS_SINCE_UNPLUGGED) {
            val -= mUnpluggedSystemTime;
        }
    }
    return val;
}

Int64 BatteryStatsImpl::Uid::Proc::GetForegroundTime(
    /* [in] */ Int32 which)
{
    Int64 val;
    if (which == STATS_LAST) {
        val = mLastForegroundTime;
    }
    else {
        val = mForegroundTime;
        if (which == STATS_CURRENT) {
            val -= mLoadedForegroundTime;
        }
        else if (which == STATS_SINCE_UNPLUGGED) {
            val -= mUnpluggedForegroundTime;
        }
    }
    return val;
}

Int32 BatteryStatsImpl::Uid::Proc::getStarts(
    /* [in] */ Int32 which)
{
    Int32 val;
    if (which == STATS_LAST) {
        val = mLastStarts;
    }
    else {
        val = mStarts;
        if (which == STATS_CURRENT) {
            val -= mLoadedStarts;
        }
        else if (which == STATS_SINCE_UNPLUGGED) {
            val -= mUnpluggedStarts;
        }
    }
    return val;
}

void BatteryStatsImpl::Uid::Proc::AddSpeedStepTimes(
    /* [in] */ ArrayOf<Int64>* values)
{
    for (Int32 i = 0; i < mSpeedBins->GetLength() && i < values->GetLength(); i++) {
        Int64 amt = (*values)[i];
        if (amt != 0) {
            AutoPtr<SamplingCounter> c = (*mSpeedBins)[i];
            if (c == NULL) {
                AutoPtr<SamplingCounter> counter = new SamplingCounter(&mHost->mHost->mUnpluggables);
                mSpeedBins->Set(i, counter);
            }
            c->AddCountAtomic((*values)[i]);
        }
    }
}

Int64 BatteryStatsImpl::Uid::Proc::GetTimeAtCpuSpeedStep(
    /* [in] */ Int32 speedStep,
    /* [in] */ Int32 which)
{
    if (speedStep < mSpeedBins->GetLength()) {
        AutoPtr<SamplingCounter> c = (*mSpeedBins)[speedStep];
        return c != NULL ? c->GetCountLocked(which) : 0;
    }
    else {
        return 0;
    }
}


//==============================================================================
// Uid::Pkg::Serv
//==============================================================================
BatteryStatsImpl::Uid::Pkg::Serv::Serv(
    /* [in] */ Pkg* host)
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
   mHost->mHost->mHost->mUnpluggables.PushBack(this);
}

PInterface BatteryStatsImpl::Uid::Pkg::Serv::Probe(
    /* [in]  */ REIID riid)
{
    return NULL;
}

UInt32 BatteryStatsImpl::Uid::Pkg::Serv::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BatteryStatsImpl::Uid::Pkg::Serv::Release()
{
    return ElRefBase::Release();
}

ECode BatteryStatsImpl::Uid::Pkg::Serv::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return  NOERROR;
}

void BatteryStatsImpl::Uid::Pkg::Serv::Unplug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    mUnpluggedStartTime = GetStartTimeToNowLocked(batteryUptime);
    mUnpluggedStarts = mStarts;
    mUnpluggedLaunches = mLaunches;
}

void BatteryStatsImpl::Uid::Pkg::Serv::Plug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{}

void BatteryStatsImpl::Uid::Pkg::Serv::Detach()
{
    List<AutoPtr<IUnpluggable> >::Iterator it = mHost->mHost->mHost->mUnpluggables.Begin();
    for (; it != mHost->mHost->mHost->mUnpluggables.End(); ++it) {
        if ((*it).Get() == (IUnpluggable*)this) {
            mHost->mHost->mHost->mUnpluggables.Erase(it);
            break;
        }
    }
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

void BatteryStatsImpl::Uid::Pkg::Serv::StartLaunchedLocked()
{
    if (!mLaunched) {
        mLaunches++;
        mLaunchedSince = mHost->mHost->mHost->GetBatteryUptimeLocked();
        mLaunched = TRUE;
    }
}

void BatteryStatsImpl::Uid::Pkg::Serv::StopLaunchedLocked()
{
    if (mLaunched) {
        Int64 time = mHost->mHost->mHost->GetBatteryUptimeLocked() - mLaunchedSince;
        if (time > 0) {
            mLaunchedTime += time;
        }
        else {
            mLaunches--;
        }
        mLaunched = FALSE;
    }
}

void BatteryStatsImpl::Uid::Pkg::Serv::StartRunningLocked()
{
    if (!mRunning) {
        mStarts++;
        mRunningSince = mHost->mHost->mHost->GetBatteryUptimeLocked();
        mRunning = TRUE;
    }
}

void BatteryStatsImpl::Uid::Pkg::Serv::StopRunningLocked()
{
    if (mRunning) {
        Int64 time = mHost->mHost->mHost->GetBatteryUptimeLocked() - mRunningSince;
        if (time > 0) {
            mStartTime += time;
        }
        else {
            mStarts--;
        }
        mRunning = FALSE;
    }
}

AutoPtr<BatteryStatsImpl> BatteryStatsImpl::Uid::Pkg::Serv::GetBatteryStats()
{
    return mHost->mHost->mHost;
}

Int32 BatteryStatsImpl::Uid::Pkg::Serv::GetLaunches(
    /* [in] */ Int32 which)
{
    Int32 val;

    if (which == STATS_LAST) {
        val = mLastLaunches;
    }
    else {
        val = mLaunches;
        if (which == STATS_CURRENT) {
            val -= mLoadedLaunches;
        }
        else if (which == STATS_SINCE_UNPLUGGED) {
            val -= mUnpluggedLaunches;
        }
    }

    return val;
}

Int64 BatteryStatsImpl::Uid::Pkg::Serv::GetStartTime(
    /* [in] */ Int64 now,
    /* [in] */ Int32 which)
{
    Int64 val;
    if (which == STATS_LAST) {
        val = mLastStartTime;
    }
    else {
        val = GetStartTimeToNowLocked(now);
        if (which == STATS_CURRENT) {
            val -= mLoadedStartTime;
        }
        else if (which == STATS_SINCE_UNPLUGGED) {
            val -= mUnpluggedStartTime;
        }
    }

    return val;
}

Int32 BatteryStatsImpl::Uid::Pkg::Serv::GetStarts(
    /* [in] */ Int32 which)
{
    Int32 val;
    if (which == STATS_LAST) {
        val = mLastStarts;
    }
    else {
        val = mStarts;
        if (which == STATS_CURRENT) {
            val -= mLoadedStarts;
        }
        else if (which == STATS_SINCE_UNPLUGGED) {
            val -= mUnpluggedStarts;
        }
    }

    return val;
}


//==============================================================================
// Uid::Pkg
//==============================================================================
BatteryStatsImpl::Uid::Pkg::Pkg(
    /* [in] */ Uid* host)
    : mWakeups(0)
    , mLoadedWakeups(0)
    , mLastWakeups(0)
    , mUnpluggedWakeups(0)
    , mHost(host)
{
    mHost->mHost->mUnpluggables.PushBack(this);
}

PInterface BatteryStatsImpl::Uid::Pkg::Probe(
    /* [in]  */ REIID riid)
{
    return NULL;
}

UInt32 BatteryStatsImpl::Uid::Pkg::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 BatteryStatsImpl::Uid::Pkg::Release()
{
    return ElRefBase::Release();
}

ECode BatteryStatsImpl::Uid::Pkg::GetInterfaceID(
    /* [in] */ IInterface *pObject,
    /* [out] */ InterfaceID *pIID)
{
    return  NOERROR;
}

void BatteryStatsImpl::Uid::Pkg::Unplug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    mUnpluggedWakeups = mWakeups;
}

void BatteryStatsImpl::Uid::Pkg::Plug(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{}

void BatteryStatsImpl::Uid::Pkg::Detach()
{
    List<AutoPtr<IUnpluggable> >::Iterator it = mHost->mHost->mUnpluggables.Begin();
    for (; it != mHost->mHost->mUnpluggables.End(); ++it) {
        if ((*it).Get() == (IUnpluggable*)this) {
            mHost->mHost->mUnpluggables.Erase(it);
            break;
        }
    }
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
    mServiceStats.Clear();
    for (Int32 m = 0; m < numServs; m++) {
        String serviceName;
        in->ReadString(&serviceName);
        AutoPtr<Serv> serv = new Serv(this);
        mServiceStats[serviceName] = serv;

        serv->ReadFromParcelLocked(in);
    }
}

void BatteryStatsImpl::Uid::Pkg::WriteToParcelLocked(
    /* [in] */ IParcel* out)
{
    out->WriteInt32(mWakeups);
    out->WriteInt32(mLoadedWakeups);
    out->WriteInt32(mUnpluggedWakeups);

    out->WriteInt32(mServiceStats.GetSize());
    HashMap<String, AutoPtr<Serv> >::Iterator it = mServiceStats.Begin();
    for (; it != mServiceStats.End(); ++it) {
        out->WriteString(it->mFirst);
        AutoPtr<Serv> serv = it->mSecond;

        serv->WriteToParcelLocked(out);
    }
}

AutoPtr< HashMap<String, AutoPtr<BatteryStatsImpl::Uid::Pkg::BatteryStatsServ> > > BatteryStatsImpl::Uid::Pkg::GetServiceStats()
{
    AutoPtr< HashMap<String, AutoPtr<BatteryStatsServ> > > temp
            = new HashMap<String, AutoPtr<BatteryStatsServ> >();
    HashMap<String, AutoPtr<Serv> >::Iterator it = mServiceStats.Begin();
    for (; it != mServiceStats.End(); ++it) {
        (*temp)[it->mFirst] = (BatteryStatsServ*)it->mSecond;
    }
    return temp;
}

Int32 BatteryStatsImpl::Uid::Pkg::GetWakeups(
    /* [in] */ Int32 which)
{
    Int32 val;
    if (which == STATS_LAST) {
        val = mLastWakeups;
    }
    else {
        val = mWakeups;
        if (which == STATS_CURRENT) {
            val -= mLoadedWakeups;
        }
        else if (which == STATS_SINCE_UNPLUGGED) {
            val -= mUnpluggedWakeups;
        }
    }

    return val;
}

AutoPtr<BatteryStatsImpl> BatteryStatsImpl::Uid::Pkg::GetBatteryStats()
{
    return mHost->mHost;
}

void BatteryStatsImpl::Uid::Pkg::IncWakeupsLocked()
{
    mWakeups++;
}

AutoPtr<BatteryStatsImpl::Uid::Pkg::Serv> BatteryStatsImpl::Uid::Pkg::NewServiceStatsLocked()
{
    return new Serv(this);
}


//==============================================================================
// Uid
//==============================================================================
BatteryStatsImpl::Uid::Uid(
    /* [in] */ Int32 uid,
    /* [in] */ BatteryStatsImpl* host)
    : mUid(uid)
    , mLoadedTcpBytesReceived(0)
    , mLoadedTcpBytesSent(0)
    , mCurrentTcpBytesReceived(0)
    , mCurrentTcpBytesSent(0)
    , mTcpBytesReceivedAtLastUnplug(0)
    , mTcpBytesSentAtLastUnplug(0)
    , mStartedTcpBytesReceived(0)
    , mStartedTcpBytesSent(0)
    , mWifiRunning(FALSE)
    , mFullWifiLockOut(FALSE)
    , mWifiScanStarted(FALSE)
    , mWifiMulticastEnabled(FALSE)
    , mAudioTurnedOn(FALSE)
    , mVideoTurnedOn(FALSE)
    , mHost(host)
{
    mWifiRunningTimer = new StopwatchTimer(this, WIFI_RUNNING,
            &mHost->mWifiRunningTimers, &mHost->mUnpluggables);
    mFullWifiLockTimer = new StopwatchTimer(this, FULL_WIFI_LOCK,
            &mHost->mFullWifiLockTimers, &mHost->mUnpluggables);
    mWifiScanTimer = new StopwatchTimer(this, WIFI_SCAN,
            &mHost->mWifiScanTimers, &mHost->mUnpluggables);
    mWifiMulticastTimer = new StopwatchTimer(this, WIFI_MULTICAST_ENABLED,
            &mHost->mWifiMulticastTimers, &mHost->mUnpluggables);
    mAudioTurnedOnTimer = new StopwatchTimer(this, AUDIO_TURNED_ON,
            NULL, &mHost->mUnpluggables);
    mVideoTurnedOnTimer = new StopwatchTimer(this, VIDEO_TURNED_ON,
            NULL, &mHost->mUnpluggables);
}

AutoPtr< HashMap<String, AutoPtr<BatteryStatsImpl::Uid::BatteryStatsWakelock> > > BatteryStatsImpl::Uid::GetWakelockStats()
{
    AutoPtr< HashMap<String, AutoPtr<BatteryStatsWakelock> > > temp = new HashMap<String, AutoPtr<BatteryStatsWakelock> >();
    HashMap<String, AutoPtr<Wakelock> >::Iterator it = mWakelockStats.Begin();
    for (; it != mWakelockStats.End(); ++it) {
        (*temp)[it->mFirst] = it->mSecond;
    }
    return temp;
}

AutoPtr< HashMap<Int32, AutoPtr<BatteryStatsImpl::Uid::BatteryStatsSensor> > > BatteryStatsImpl::Uid::GetSensorStats()
{
    AutoPtr< HashMap<Int32, AutoPtr<BatteryStatsSensor> > > temp
            = new HashMap<Int32, AutoPtr<BatteryStatsSensor> >();
    HashMap<Int32, AutoPtr<Sensor> >::Iterator it = mSensorStats.Begin();
    for (; it != mSensorStats.End(); ++it) {
        (*temp)[it->mFirst] = it->mSecond;
    }
    return temp;
}

AutoPtr< HashMap<String, AutoPtr<BatteryStatsImpl::Uid::BatteryStatsProc> > > BatteryStatsImpl::Uid::GetProcessStats()
{
    AutoPtr< HashMap<String, AutoPtr<BatteryStatsProc> > > temp
            = new HashMap<String, AutoPtr<BatteryStatsProc> >();
    HashMap<String, AutoPtr<Proc> >::Iterator it = mProcessStats.Begin();
    for (; it != mProcessStats.End(); ++it) {
        (*temp)[it->mFirst] = it->mSecond;
    }
    return temp;
}

AutoPtr< HashMap<String, AutoPtr<BatteryStatsImpl::Uid::BatteryStatsPkg> > > BatteryStatsImpl::Uid::GetPackageStats()
{
    AutoPtr< HashMap<String, AutoPtr<BatteryStatsPkg> > > temp
            = new HashMap<String, AutoPtr<BatteryStatsPkg> >();
    HashMap<String, AutoPtr<Pkg> >::Iterator it = mPackageStats.Begin();
    for (; it != mPackageStats.End(); ++it) {
        (*temp)[it->mFirst] = it->mSecond;
    }
    return temp;
}

Int32 BatteryStatsImpl::Uid::GetUid()
{
    return mUid;
}

Int64 BatteryStatsImpl::Uid::GetTcpBytesReceived(
    /* [in] */ Int32 which)
{
    if (which == STATS_LAST) {
        return mLoadedTcpBytesReceived;
    }
    else {
        Int64 current = ComputeCurrentTcpBytesReceived();
        if (which == STATS_SINCE_UNPLUGGED) {
            current -= mTcpBytesReceivedAtLastUnplug;
        }
        else if (which == STATS_SINCE_CHARGED) {
            current += mLoadedTcpBytesReceived;
        }
        return current;
    }
}

Int64 BatteryStatsImpl::Uid::ComputeCurrentTcpBytesReceived()
{
    AutoPtr<INetworkStats> ns = mHost->GetNetworkStatsDetailGroupedByUid();
    AutoPtr<INetworkStatsEntry> entry;
    AutoPtr<IObjectContainer> container;
    CObjectContainer::New((IObjectContainer**)&container);
    AutoPtr<IInteger32> integer;
    CInteger32::New(mUid, (IInteger32**)&integer);
    container->Add((IInterface*)integer);
    ns->GetTotal(NULL, container, (INetworkStatsEntry**)&entry);
    Int64 uidRxBytes;
    entry->GetRxBytes(&uidRxBytes);
    return mCurrentTcpBytesReceived + (mStartedTcpBytesReceived >= 0
            ? (uidRxBytes - mStartedTcpBytesReceived) : 0);
}

Int64 BatteryStatsImpl::Uid::GetTcpBytesSent(
    /* [in] */ Int32 which)
{
    if (which == STATS_LAST) {
        return mLoadedTcpBytesSent;
    }
    else {
        Int64 current = ComputeCurrentTcpBytesSent();
        if (which == STATS_SINCE_UNPLUGGED) {
            current -= mTcpBytesSentAtLastUnplug;
        }
        else if (which == STATS_SINCE_CHARGED) {
            current += mLoadedTcpBytesSent;
        }
        return current;
    }
}

void BatteryStatsImpl::Uid::NoteWifiRunningLocked()
{
    if (!mWifiRunning) {
        mWifiRunning = TRUE;
        if (mWifiRunningTimer == NULL) {
            mWifiRunningTimer = new StopwatchTimer(this, WIFI_RUNNING,
                    &mHost->mWifiRunningTimers, &mHost->mUnpluggables);
        }
        mWifiRunningTimer->StartRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteWifiStoppedLocked()
{
    if (mWifiRunning) {
        mWifiRunning = FALSE;
        mWifiRunningTimer->StopRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteFullWifiLockAcquiredLocked()
{
    if (!mFullWifiLockOut) {
        mFullWifiLockOut = TRUE;
        if (mFullWifiLockTimer == NULL) {
            mFullWifiLockTimer = new StopwatchTimer(this, FULL_WIFI_LOCK,
                    &mHost->mFullWifiLockTimers, &mHost->mUnpluggables);
        }
        mFullWifiLockTimer->StartRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteFullWifiLockReleasedLocked()
{
    if (mFullWifiLockOut) {
        mFullWifiLockOut = FALSE;
        mFullWifiLockTimer->StopRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteWifiScanStartedLocked()
{
    if (!mWifiScanStarted) {
        mWifiScanStarted = TRUE;
        if (mWifiScanTimer == NULL) {
            mWifiScanTimer = new StopwatchTimer(this, WIFI_SCAN,
                    &mHost->mWifiScanTimers, &mHost->mUnpluggables);
        }
        mWifiScanTimer->StartRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteWifiScanStoppedLocked()
{
    if (mWifiScanStarted) {
        mWifiScanStarted = FALSE;
        mWifiScanTimer->StopRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteWifiMulticastEnabledLocked()
{
    if (!mWifiMulticastEnabled) {
        mWifiMulticastEnabled = TRUE;
        if (mWifiMulticastTimer == NULL) {
            mWifiMulticastTimer = new StopwatchTimer(this, WIFI_MULTICAST_ENABLED,
                    &mHost->mWifiMulticastTimers, &mHost->mUnpluggables);
        }
        mWifiMulticastTimer->StartRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteWifiMulticastDisabledLocked()
{
    if (mWifiMulticastEnabled) {
        mWifiMulticastEnabled = FALSE;
        mWifiMulticastTimer->StopRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteAudioTurnedOnLocked()
{
    if (!mAudioTurnedOn) {
        mAudioTurnedOn = TRUE;
        if (mAudioTurnedOnTimer == NULL) {
            mAudioTurnedOnTimer = new StopwatchTimer(this, AUDIO_TURNED_ON,
                    NULL, &mHost->mUnpluggables);
        }
        mAudioTurnedOnTimer->StartRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteAudioTurnedOffLocked()
{
    if (mAudioTurnedOn) {
        mAudioTurnedOn = FALSE;
        mAudioTurnedOnTimer->StopRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteVideoTurnedOnLocked()
{
    if (!mVideoTurnedOn) {
        mVideoTurnedOn = TRUE;
        if (mVideoTurnedOnTimer == NULL) {
            mVideoTurnedOnTimer = new StopwatchTimer(this, VIDEO_TURNED_ON,
                    NULL, &mHost->mUnpluggables);
        }
        mVideoTurnedOnTimer->StartRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteVideoTurnedOffLocked()
{
    if (mVideoTurnedOn) {
        mVideoTurnedOn = FALSE;
        mVideoTurnedOnTimer->StopRunningLocked(mHost);
    }
}

Int64 BatteryStatsImpl::Uid::GetWifiRunningTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    if (mWifiRunningTimer == NULL) {
        return 0;
    }
    return mWifiRunningTimer->GetTotalTimeLocked(batteryRealtime, which);
}

Int64 BatteryStatsImpl::Uid::GetFullWifiLockTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    if (mFullWifiLockTimer == NULL) {
        return 0;
    }
    return mFullWifiLockTimer->GetTotalTimeLocked(batteryRealtime, which);
}

Int64 BatteryStatsImpl::Uid::GetWifiScanTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    if (mWifiScanTimer == NULL) {
        return 0;
    }
    return mWifiScanTimer->GetTotalTimeLocked(batteryRealtime, which);
}

Int64 BatteryStatsImpl::Uid::GetWifiMulticastTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    if (mWifiMulticastTimer == NULL) {
        return 0;
    }
    return mWifiMulticastTimer->GetTotalTimeLocked(batteryRealtime,
                                                  which);
}

Int64 BatteryStatsImpl::Uid::GetAudioTurnedOnTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    if (mAudioTurnedOnTimer == NULL) {
        return 0;
    }
    return mAudioTurnedOnTimer->GetTotalTimeLocked(batteryRealtime, which);
}

Int64 BatteryStatsImpl::Uid::GetVideoTurnedOnTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    if (mVideoTurnedOnTimer == NULL) {
        return 0;
    }
    return mVideoTurnedOnTimer->GetTotalTimeLocked(batteryRealtime, which);
}

void BatteryStatsImpl::Uid::NoteUserActivityLocked(
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
}

Boolean BatteryStatsImpl::Uid::HasUserActivity()
{
    return mUserActivityCounters != NULL;
}

Int32 BatteryStatsImpl::Uid::GetUserActivityCount(
    /* [in] */ Int32 type,
    /* [in] */ Int32 which)
{
    if (mUserActivityCounters == NULL) {
        return 0;
    }
    return (*mUserActivityCounters)[type]->GetCountLocked(which);
}

void BatteryStatsImpl::Uid::InitUserActivityLocked()
{
    mUserActivityCounters = ArrayOf<Counter*>::Alloc(NUM_USER_ACTIVITY_TYPES);
    for (Int32 i = 0; i < NUM_USER_ACTIVITY_TYPES; i++) {
        AutoPtr<Counter> counter = new Counter(&mHost->mUnpluggables);
        mUserActivityCounters->Set(i, counter);
    }
}

Int64 BatteryStatsImpl::Uid::ComputeCurrentTcpBytesSent()
{
    AutoPtr<INetworkStats> ns = mHost->GetNetworkStatsDetailGroupedByUid();
    AutoPtr<INetworkStatsEntry> entry;
    AutoPtr<IObjectContainer> container;
    CObjectContainer::New((IObjectContainer**)&container);
    AutoPtr<IInteger32> integer;
    CInteger32::New(mUid, (IInteger32**)&integer);
    container->Add((IInterface*)integer);
    ns->GetTotal(NULL, container, (INetworkStatsEntry**)&entry);
    Int64 uidTxBytes;
    entry->GetTxBytes(&uidTxBytes);
    return mCurrentTcpBytesSent + (mStartedTcpBytesSent >= 0
            ? (uidTxBytes - mStartedTcpBytesSent) : 0);
}

Boolean BatteryStatsImpl::Uid::Reset()
{
    Boolean active = FALSE;

    if (mWifiRunningTimer != NULL) {
        active |= !mWifiRunningTimer->Reset(mHost, FALSE);
        active |= mWifiRunning;
    }
    if (mFullWifiLockTimer != NULL) {
        active |= !mFullWifiLockTimer->Reset(mHost, FALSE);
        active |= mFullWifiLockOut;
    }
    if (mWifiScanTimer != NULL) {
        active |= !mWifiScanTimer->Reset(mHost, FALSE);
        active |= mWifiScanStarted;
    }
    if (mWifiMulticastTimer != NULL) {
        active |= !mWifiMulticastTimer->Reset(mHost, FALSE);
        active |= mWifiMulticastEnabled;
    }
    if (mAudioTurnedOnTimer != NULL) {
        active |= !mAudioTurnedOnTimer->Reset(mHost, FALSE);
        active |= mAudioTurnedOn;
    }
    if (mVideoTurnedOnTimer != NULL) {
        active |= !mVideoTurnedOnTimer->Reset(mHost, FALSE);
        active |= mVideoTurnedOn;
    }

    mLoadedTcpBytesReceived = mLoadedTcpBytesSent = 0;
    mCurrentTcpBytesReceived = mCurrentTcpBytesSent = 0;

    if (mUserActivityCounters != NULL) {
        for (Int32 i = 0; i < NUM_USER_ACTIVITY_TYPES; i++) {
            (*mUserActivityCounters)[i]->Reset(FALSE);
        }
    }

    if (mWakelockStats.Begin() != mWakelockStats.End()) {
        HashMap<String, AutoPtr<Wakelock> >::Iterator it = mWakelockStats.Begin();
        for (; it != mWakelockStats.End();) {
            AutoPtr<Wakelock> wl = it->mSecond;
            if (wl->Reset()) {
                mWakelockStats.Erase(it++);
            }
            else {
                active = TRUE;
                ++it;
            }
        }
    }
    if (mSensorStats.Begin() != mSensorStats.End()) {
        HashMap<Int32, AutoPtr<Sensor> >::Iterator it = mSensorStats.Begin();
        for (; it != mSensorStats.End();) {
            AutoPtr<Sensor> s = it->mSecond;
            if (s->Reset()) {
                mSensorStats.Erase(it++);
            }
            else {
                active = TRUE;
                ++it;
            }
        }
    }
    if (mProcessStats.Begin() != mProcessStats.End()) {
        HashMap<String, AutoPtr<Proc> >::Iterator it = mProcessStats.Begin();
        for (; it != mProcessStats.End();) {
            it->mSecond->Detach();
        }
        mProcessStats.Clear();
    }
    if (mPids.Begin() != mPids.End()) {
        HashMap<Int32, AutoPtr<Pid> >::Iterator it = mPids.Begin();
        for (; !active && it != mPids.End(); ++it) {
            AutoPtr<Pid> pid = it->mSecond;
            if (pid->mWakeStart != 0) {
                active = TRUE;
            }
        }
    }
    if (mPackageStats.Begin() != mPackageStats.End()) {
        HashMap<String, AutoPtr<Pkg> >::Iterator it = mPackageStats.Begin();
        for (; it != mPackageStats.End();) {
            AutoPtr<Pkg> p = it->mSecond;
            p->Detach();
            if (p->mServiceStats.Begin() != p->mServiceStats.End()) {
                HashMap<String, AutoPtr<Pkg::Serv> >::Iterator servIt = p->mServiceStats.Begin();
                for (; servIt != p->mServiceStats.End(); ++servIt) {
                    servIt->mSecond->Detach();
                }
            }
        }
        mPackageStats.Clear();
    }

    mPids.Clear();

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
        if (mWifiMulticastTimer != NULL) {
            mWifiMulticastTimer->Detach();
        }
        if (mAudioTurnedOnTimer != NULL) {
            mAudioTurnedOnTimer->Detach();
        }
        if (mVideoTurnedOnTimer != NULL) {
            mVideoTurnedOnTimer->Detach();
        }
        if (mUserActivityCounters != NULL) {
            for (Int32 i = 0; i < NUM_USER_ACTIVITY_TYPES; i++) {
                (*mUserActivityCounters)[i]->Detach();
            }
        }
    }

    return !active;
}

void BatteryStatsImpl::Uid::WriteToParcelLocked(
    /* [in] */ IParcel* out,
    /* [in] */ Int64 batteryRealtime)
{
    out->WriteInt32(mWakelockStats.GetSize());
    HashMap<String, AutoPtr<Wakelock> >::Iterator it = mWakelockStats.Begin();
    for (; it != mWakelockStats.End(); ++it) {
        out->WriteString(it->mFirst);
        it->mSecond->WriteToParcelLocked(out, batteryRealtime);
    }

    out->WriteInt32(mSensorStats.GetSize());
    HashMap<Int32, AutoPtr<Sensor> >::Iterator sensorIt = mSensorStats.Begin();
    for (; sensorIt != mSensorStats.End(); ++sensorIt) {
        out->WriteInt32(sensorIt->mFirst);
        sensorIt->mSecond->WriteToParcelLocked(out, batteryRealtime);
    }

    out->WriteInt32(mProcessStats.GetSize());
    HashMap<String, AutoPtr<Proc> >::Iterator processIt = mProcessStats.Begin();
    for (; processIt != mProcessStats.End(); ++processIt) {
        out->WriteString(processIt->mFirst);
        processIt->mSecond->WriteToParcelLocked(out);
    }

    out->WriteInt32(mPackageStats.GetSize());
    HashMap<String, AutoPtr<Pkg> >::Iterator pkgIt = mPackageStats.Begin();
    for (; pkgIt != mPackageStats.End(); ++pkgIt) {
        out->WriteString(pkgIt->mFirst);
        pkgIt->mSecond->WriteToParcelLocked(out);
    }

    out->WriteInt64(mLoadedTcpBytesReceived);
    out->WriteInt64(mLoadedTcpBytesSent);
    out->WriteInt64(ComputeCurrentTcpBytesReceived());
    out->WriteInt64(ComputeCurrentTcpBytesSent());
    out->WriteInt64(mTcpBytesReceivedAtLastUnplug);
    out->WriteInt64(mTcpBytesSentAtLastUnplug);
    if (mWifiRunningTimer != NULL) {
        out->WriteInt32(1);
        mWifiRunningTimer->WriteToParcel(out, batteryRealtime);
    }
    else {
        out->WriteInt32(0);
    }
    if (mFullWifiLockTimer != NULL) {
        out->WriteInt32(1);
        mFullWifiLockTimer->WriteToParcel(out, batteryRealtime);
    }
    else {
        out->WriteInt32(0);
    }
    if (mWifiScanTimer != NULL) {
        out->WriteInt32(1);
        mWifiScanTimer->WriteToParcel(out, batteryRealtime);
    }
    else {
        out->WriteInt32(0);
    }
    if (mWifiMulticastTimer != NULL) {
        out->WriteInt32(1);
        mWifiMulticastTimer->WriteToParcel(out, batteryRealtime);
    }
    else {
        out->WriteInt32(0);
    }
    if (mAudioTurnedOnTimer != NULL) {
        out->WriteInt32(1);
        mAudioTurnedOnTimer->WriteToParcel(out, batteryRealtime);
    }
    else {
        out->WriteInt32(0);
    }
    if (mVideoTurnedOnTimer != NULL) {
        out->WriteInt32(1);
        mVideoTurnedOnTimer->WriteToParcel(out, batteryRealtime);
    }
    else {
        out->WriteInt32(0);
    }
    if (mUserActivityCounters != NULL) {
        out->WriteInt32(1);
        for (Int32 i = 0; i < NUM_USER_ACTIVITY_TYPES; i++) {
            (*mUserActivityCounters)[i]->WriteToParcel(out);
        }
    }
    else {
        out->WriteInt32(0);
    }
}

void BatteryStatsImpl::Uid::ReadFromParcelLocked(
    /* [in] */ List<AutoPtr<IUnpluggable> >* unpluggables,
    /* [in] */ IParcel* in)
{
    Int32 numWakelocks;
    in->ReadInt32(&numWakelocks);
    mWakelockStats.Clear();
    for (Int32 j = 0; j < numWakelocks; j++) {
        String wakelockName;
        in->ReadString(&wakelockName);
        AutoPtr<Wakelock> wakelock = new Wakelock(this);
        wakelock->ReadFromParcelLocked(unpluggables, in);
        // We will just drop some random set of wakelocks if
        // the previous run of the system was an older version
        // that didn't impose a limit.
        mWakelockStats[wakelockName] = wakelock;
    }

    Int32 numSensors;
    in->ReadInt32(&numSensors);
    mSensorStats.Clear();
    for (Int32 k = 0; k < numSensors; k++) {
        Int32 sensorNumber;
        in->ReadInt32(&sensorNumber);
        AutoPtr<Sensor> sensor = new Sensor(sensorNumber, this);
        sensor->ReadFromParcelLocked(&mHost->mUnpluggables, in);
        mSensorStats[sensorNumber] = sensor;
    }

    Int32 numProcs;
    in->ReadInt32(&numProcs);
    mProcessStats.Clear();
    for (Int32 k = 0; k < numProcs; k++) {
        String processName;
        in->ReadString(&processName);
        AutoPtr<Proc> proc = new Proc(this);
        proc->ReadFromParcelLocked(in);
        mProcessStats[processName] = proc;
    }

    Int32 numPkgs;
    in->ReadInt32(&numPkgs);
    mPackageStats.Clear();
    for (Int32 l = 0; l < numPkgs; l++) {
        String packageName;
        in->ReadString(&packageName);
        AutoPtr<Pkg> pkg = new Pkg(this);
        pkg->ReadFromParcelLocked(in);
        mPackageStats[packageName] = pkg;
    }

    in->ReadInt64(&mLoadedTcpBytesReceived);
    in->ReadInt64(&mLoadedTcpBytesSent);
    in->ReadInt64(&mCurrentTcpBytesReceived);
    in->ReadInt64(&mCurrentTcpBytesSent);
    in->ReadInt64(&mTcpBytesReceivedAtLastUnplug);
    in->ReadInt64(&mTcpBytesSentAtLastUnplug);
    mWifiRunning = FALSE;
    Int32 value;
    if (in->ReadInt32(&value), value != 0) {
        mWifiRunningTimer = new StopwatchTimer(this, WIFI_RUNNING,
                &mHost->mWifiRunningTimers, &mHost->mUnpluggables, in);
    }
    else {
        mWifiRunningTimer = NULL;
    }
    mFullWifiLockOut = FALSE;
    if (in->ReadInt32(&value), value != 0) {
        mFullWifiLockTimer = new StopwatchTimer(this, FULL_WIFI_LOCK,
                &mHost->mFullWifiLockTimers, &mHost->mUnpluggables, in);
    }
    else {
        mFullWifiLockTimer = NULL;
    }
    mWifiScanStarted = FALSE;
    if (in->ReadInt32(&value), value != 0) {
        mWifiScanTimer = new StopwatchTimer(this, WIFI_SCAN,
                &mHost->mWifiScanTimers, &mHost->mUnpluggables, in);
    }
    else {
        mWifiScanTimer = NULL;
    }
    mWifiMulticastEnabled = FALSE;
    if (in->ReadInt32(&value), value != 0) {
        mWifiMulticastTimer = new StopwatchTimer(this, WIFI_MULTICAST_ENABLED,
                &mHost->mWifiMulticastTimers, &mHost->mUnpluggables, in);
    }
    else {
        mWifiMulticastTimer = NULL;
    }
    mAudioTurnedOn = FALSE;
    if (in->ReadInt32(&value), value != 0) {
        mAudioTurnedOnTimer = new StopwatchTimer(this, AUDIO_TURNED_ON,
                NULL, &mHost->mUnpluggables, in);
    }
    else {
        mAudioTurnedOnTimer = NULL;
    }
    mVideoTurnedOn = FALSE;
    if (in->ReadInt32(&value), value != 0) {
        mVideoTurnedOnTimer = new StopwatchTimer(this, VIDEO_TURNED_ON,
                NULL, &mHost->mUnpluggables, in);
    }
    else {
        mVideoTurnedOnTimer = NULL;
    }
    if (in->ReadInt32(&value), value != 0) {
        mUserActivityCounters = ArrayOf<Counter*>::Alloc(NUM_USER_ACTIVITY_TYPES);
        for (Int32 i = 0; i < NUM_USER_ACTIVITY_TYPES; i++) {
            AutoPtr<Counter> counter = new Counter(&mHost->mUnpluggables, in);
            mUserActivityCounters->Set(i, counter);
        }
    }
    else {
        mUserActivityCounters = NULL;
    }
}

AutoPtr<BatteryStatsImpl::Uid::Proc> BatteryStatsImpl::Uid::GetProcessStatsLocked(
    /* [in] */ const String& name)
{
    AutoPtr<Proc> ps;
    HashMap<String, AutoPtr<Proc> >::Iterator it = mProcessStats.Find(name);
    if (it != mProcessStats.End()) {
        ps = it->mSecond;
    }
    if (ps == NULL) {
        ps = new Proc(this);
        mProcessStats[name] = ps;
    }

    return ps;
}

HashMap<Int32, AutoPtr<BatteryStatsImpl::Uid::Pid> >& BatteryStatsImpl::Uid::GetPidStats()
{
    return mPids;
}

AutoPtr<BatteryStatsImpl::Uid::Pid> BatteryStatsImpl::Uid::GetPidStatsLocked(
    /* [in] */ Int32 pid)
{
    AutoPtr<Pid> p;
    HashMap<Int32, AutoPtr<Pid> >::Iterator it = mPids.Find(pid);
    if (it != mPids.End()) {
        p = it->mSecond;
    }
    if (p == NULL) {
        p = new Pid();
        mPids[pid] = p;
    }
    return p;
}

AutoPtr<BatteryStatsImpl::Uid::Pkg> BatteryStatsImpl::Uid::GetPackageStatsLocked(
    /* [in] */ const String& name)
{
    AutoPtr<Pkg> ps;
    HashMap<String, AutoPtr<Pkg> >::Iterator it = mPackageStats.Find(name);
    if (it != mPackageStats.End()) {
        ps = it->mSecond;
    }
    if (ps == NULL) {
        ps = new Pkg(this);
        mPackageStats[name] = ps;
    }

    return ps;
}

AutoPtr<BatteryStatsImpl::Uid::Pkg::Serv> BatteryStatsImpl::Uid::GetServiceStatsLocked(
    /* [in] */ const String& pkg,
    /* [in] */ const String& serv)
{
    AutoPtr<Pkg> ps = GetPackageStatsLocked(pkg);
    AutoPtr<Pkg::Serv> ss;
    HashMap<String, AutoPtr<Pkg::Serv> >::Iterator it = ps->mServiceStats.Find(serv);
    if (it != ps->mServiceStats.End()) {
        ss = it->mSecond;
    }
    if (ss == NULL) {
        ss = ps->NewServiceStatsLocked();
        ps->mServiceStats[serv] = ss;
    }

    return ss;
}

ECode BatteryStatsImpl::Uid::GetWakeTimerLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 type,
    /* [out] */ StopwatchTimer** timer)
{
    VALIDATE_NOT_NULL(timer)

    String keyName = name;
    AutoPtr<Wakelock> wl;
    HashMap<String, AutoPtr<Wakelock> >::Iterator it = mWakelockStats.Find(keyName);
    if (it != mWakelockStats.End()) {
        wl = it->mSecond;
    }
    if (wl == NULL) {
        Int32 N = mWakelockStats.GetSize();
        if (N > MAX_WAKELOCKS_PER_UID && (mUid != IProcess::SYSTEM_UID
                || N > MAX_WAKELOCKS_PER_UID_IN_SYSTEM)) {
            keyName = BATCHED_WAKELOCK_NAME;
            it = mWakelockStats.Find(keyName);
            if (it != mWakelockStats.End()) {
                wl = it->mSecond;
            }
        }
        if (wl == NULL) {
            wl = new Wakelock(this);
            mWakelockStats[keyName] = wl;
        }
    }
    AutoPtr<StopwatchTimer> t;
    switch (type) {
        case WAKE_TYPE_PARTIAL:
            t = wl->mTimerPartial;
            if (t == NULL) {
                t = new StopwatchTimer(this, WAKE_TYPE_PARTIAL,
                        &mHost->mPartialTimers, &mHost->mUnpluggables);
                wl->mTimerPartial = t;
            }
            *timer = t;
            REFCOUNT_ADD(*timer)
            return NOERROR;
        case WAKE_TYPE_FULL:
            t = wl->mTimerFull;
            if (t == NULL) {
                t = new StopwatchTimer(this, WAKE_TYPE_FULL,
                        &mHost->mFullTimers, &mHost->mUnpluggables);
                wl->mTimerFull = t;
            }
            *timer = t;
            REFCOUNT_ADD(*timer)
            return NOERROR;
        case WAKE_TYPE_WINDOW:
            t = wl->mTimerWindow;
            if (t == NULL) {
                t = new StopwatchTimer(this, WAKE_TYPE_WINDOW,
                        &mHost->mWindowTimers, &mHost->mUnpluggables);
                wl->mTimerWindow = t;
            }
            *timer = t;
            REFCOUNT_ADD(*timer)
            return NOERROR;
        default:
            Logger::E(TAG, "type=%d", type);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            // throw new IllegalArgumentException("type=" + type);
    }
}

AutoPtr<BatteryStatsImpl::StopwatchTimer> BatteryStatsImpl::Uid::GetSensorTimerLocked(
    /* [in] */ Int32 sensor,
    /* [in] */ Boolean create)
{
    AutoPtr<Sensor> se;
    HashMap<Int32, AutoPtr<Sensor> >::Iterator it = mSensorStats.Find(sensor);
    if (it != mSensorStats.End()) {
        se = it->mSecond;
    }
    if (se == NULL) {
        if (!create) {
            return NULL;
        }
        se = new Sensor(sensor, this);
        mSensorStats[sensor] = se;
    }
    AutoPtr<StopwatchTimer> t = se->mTimer;
    if (t != NULL) {
        return t;
    }
    AutoPtr< List<AutoPtr<StopwatchTimer> > > timers;
    HashMap<Int32, AutoPtr< List<AutoPtr<StopwatchTimer> > > >::Iterator timersIt = mHost->mSensorTimers.Find(sensor);
    if (timersIt != mHost->mSensorTimers.End()) {
        timers = timersIt->mSecond;
    }
    if (timers == NULL) {
        timers = new List<AutoPtr<StopwatchTimer> >(32);
        mHost->mSensorTimers[sensor] = timers;
    }
    t = new StopwatchTimer(this, BatteryStats::SENSOR, timers, &mHost->mUnpluggables);
    se->mTimer = t;
    return t;
}

void BatteryStatsImpl::Uid::NoteStartWakeLocked(
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    AutoPtr<StopwatchTimer> t;
    GetWakeTimerLocked(name, type, (StopwatchTimer**)&t);
    if (t != NULL) {
        t->StartRunningLocked(mHost);
    }
    if (pid >= 0 && type == WAKE_TYPE_PARTIAL) {
        AutoPtr<Pid> p = GetPidStatsLocked(pid);
        if (p->mWakeStart == 0) {
            p->mWakeStart = SystemClock::GetElapsedRealtime();
        }
    }
}

void BatteryStatsImpl::Uid::NoteStopWakeLocked(
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    AutoPtr<StopwatchTimer> t;
    GetWakeTimerLocked(name, type, (StopwatchTimer**)&t);
    if (t != NULL) {
        t->StopRunningLocked(mHost);
    }
    if (pid >= 0 && type == WAKE_TYPE_PARTIAL) {
        AutoPtr<Pid> p;
        HashMap<Int32, AutoPtr<Pid> >::Iterator it = mPids.Find(pid);
        if (it != mPids.End()) {
            p = it->mSecond;
        }
        if (p != NULL && p->mWakeStart != 0) {
            p->mWakeSum += SystemClock::GetElapsedRealtime() - p->mWakeStart;
            p->mWakeStart = 0;
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
    /* [in] */ Int32 sensor)
{
    AutoPtr<StopwatchTimer> t = GetSensorTimerLocked(sensor, TRUE);
    if (t != NULL) {
        t->StartRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteStopSensor(
    /* [in] */ Int32 sensor)
{
    // Don't create a timer if one doesn't already exist
    AutoPtr<StopwatchTimer> t = GetSensorTimerLocked(sensor, FALSE);
    if (t != NULL) {
        t->StopRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteStartGps()
{
    AutoPtr<StopwatchTimer> t = GetSensorTimerLocked(Sensor::GPS, TRUE);
    if (t != NULL) {
        t->StartRunningLocked(mHost);
    }
}

void BatteryStatsImpl::Uid::NoteStopGps()
{
    AutoPtr<StopwatchTimer> t = GetSensorTimerLocked(Sensor::GPS, FALSE);
    if (t != NULL) {
        t->StopRunningLocked(mHost);
    }
}

AutoPtr<BatteryStatsImpl> BatteryStatsImpl::Uid::GetBatteryStats()
{
    return mHost;
}


//==============================================================================
// BatteryStatsImpl
//==============================================================================

const String BatteryStatsImpl::TAG("BatteryStatsImpl");
const Boolean BatteryStatsImpl::DEBUG;
const Boolean BatteryStatsImpl::DEBUG_HISTORY;
const Boolean BatteryStatsImpl::USE_OLD_HISTORY;
const Int32 BatteryStatsImpl::MAGIC;
const Int32 BatteryStatsImpl::VERSION;
const Int32 BatteryStatsImpl::MAX_HISTORY_ITEMS;
const Int32 BatteryStatsImpl::MAX_MAX_HISTORY_ITEMS;
const Int32 BatteryStatsImpl::MAX_WAKELOCKS_PER_UID;
const Int32 BatteryStatsImpl::MAX_WAKELOCKS_PER_UID_IN_SYSTEM;
const String BatteryStatsImpl::BATCHED_WAKELOCK_NAME("*overflow*");
const Int64 BatteryStatsImpl::DELAY_UPDATE_WAKELOCKS;
const Int32 BatteryStatsImpl::MAX_HISTORY_BUFFER;
const Int32 BatteryStatsImpl::MAX_MAX_HISTORY_BUFFER;

Int32 BatteryStatsImpl::sNumSpeedSteps = 0;
Int32 BatteryStatsImpl::sKernelWakelockUpdateVersion = 0;

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
    (*formats)[0] = Process::PROC_TAB_TERM | Process::PROC_OUT_STRING;                         // 0: name
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

CAR_INTERFACE_IMPL_2(BatteryStatsImpl, IBatteryStats, IParcelable)

BatteryStatsImpl::BatteryStatsImpl()
    : mShuttingDown(FALSE)
    , mHistoryBaseTime(0)
    , mHaveBatteryLevel(FALSE)
    , mRecordingHistory(TRUE)
    , mNumHistoryItems(0)
    , mHistoryBufferLastPos(-1)
    , mHistoryOverflow(FALSE)
    , mLastHistoryTime(0)
    , mReadOverflow(FALSE)
    , mIteratingHistory(FALSE)
    , mStartCount(0)
    , mBatteryUptime(0)
    , mBatteryLastUptime(0)
    , mBatteryRealtime(0)
    , mBatteryLastRealtime(0)
    , mUptime(0)
    , mUptimeStart(0)
    , mLastUptime(0)
    , mRealtime(0)
    , mRealtimeStart(0)
    , mLastRealtime(0)
    , mScreenOn(FALSE)
    , mScreenBrightnessBin(-1)
    , mPhoneOn(FALSE)
    , mAudioOn(FALSE)
    , mVideoOn(FALSE)
    , mPhoneSignalStrengthBin(-1)
    , mPhoneSignalStrengthBinRaw(-1)
    , mPhoneDataConnectionType(-1)
    , mWifiOn(FALSE)
    , mWifiOnUid(-1)
    , mGlobalWifiRunning(FALSE)
    , mBluetoothOn(FALSE)
    , mOnBattery(FALSE)
    , mOnBatteryInternal(FALSE)
    , mTrackBatteryPastUptime(0)
    , mTrackBatteryUptimeStart(0)
    , mTrackBatteryPastRealtime(0)
    , mTrackBatteryRealtimeStart(0)
    , mUnpluggedBatteryUptime(0)
    , mUnpluggedBatteryRealtime(0)
    , mDischargeStartLevel(0)
    , mDischargeUnplugLevel(0)
    , mDischargeCurrentLevel(0)
    , mLowDischargeAmountSinceCharge(0)
    , mHighDischargeAmountSinceCharge(0)
    , mDischargeScreenOnUnplugLevel(0)
    , mDischargeScreenOffUnplugLevel(0)
    , mDischargeAmountScreenOn(0)
    , mDischargeAmountScreenOnSinceCharge(0)
    , mDischargeAmountScreenOff(0)
    , mDischargeAmountScreenOffSinceCharge(0)
    , mLastWriteTime(0)
    , mRadioDataUptime(0)
    , mRadioDataStart(0)
    , mBluetoothPingCount(0)
    , mBluetoothPingStart(-1)
    , mPhoneServiceState(-1)
    , mPhoneServiceStateRaw(-1)
    , mPhoneSimStateRaw(-1)
    , mChangedBufferStates(0)
    , mChangedStates(0)
    , mWakeLockNesting(0)
    , mSensorNesting(0)
    , mGpsNesting(0)
    , mWifiFullLockNesting(0)
{
    CParcel::New((IParcel**)&mHistoryBuffer);
    mHistoryLastWritten = new HistoryItem();
    mHistoryLastLastWritten = new HistoryItem();
    mHistoryReadTmp = new HistoryItem();
    mHistoryCur = new HistoryItem();

    mScreenBrightnessTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_SCREEN_BRIGHTNESS_BINS);
    mPhoneSignalStrengthsTimer = ArrayOf<StopwatchTimer*>::Alloc(5/*ISignalStrength::NUM_SIGNAL_STRENGTH_BINS*/);
    mPhoneDataConnectionsTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_DATA_CONNECTION_TYPES);

    mMobileDataTx = ArrayOf<Int64>::Alloc(4);
    mMobileDataRx = ArrayOf<Int64>::Alloc(4);
    mTotalDataTx = ArrayOf<Int64>::Alloc(4);
    mTotalDataRx = ArrayOf<Int64>::Alloc(4);

    mProcWakelocksName = ArrayOf<String>::Alloc(3);
    mProcWakelocksData = ArrayOf<Int64>::Alloc(3);

    CNetworkStatsFactory::New((INetworkStatsFactory**)&mNetworkStatsFactory);

    CReentrantLock::New((IReentrantLock**)&mWriteLock);
}

BatteryStatsImpl::BatteryStatsImpl(
    /* [in] */ const String& filename)
    : mShuttingDown(FALSE)
    , mHistoryBaseTime(0)
    , mHaveBatteryLevel(FALSE)
    , mRecordingHistory(TRUE)
    , mNumHistoryItems(0)
    , mHistoryBufferLastPos(-1)
    , mHistoryOverflow(FALSE)
    , mLastHistoryTime(0)
    , mReadOverflow(FALSE)
    , mIteratingHistory(FALSE)
    , mStartCount(0)
    , mBatteryUptime(0)
    , mBatteryLastUptime(0)
    , mBatteryRealtime(0)
    , mBatteryLastRealtime(0)
    , mUptime(0)
    , mUptimeStart(0)
    , mLastUptime(0)
    , mRealtime(0)
    , mRealtimeStart(0)
    , mLastRealtime(0)
    , mScreenOn(FALSE)
    , mScreenBrightnessBin(-1)
    , mPhoneOn(FALSE)
    , mAudioOn(FALSE)
    , mVideoOn(FALSE)
    , mPhoneSignalStrengthBin(-1)
    , mPhoneSignalStrengthBinRaw(-1)
    , mPhoneDataConnectionType(-1)
    , mWifiOn(FALSE)
    , mWifiOnUid(-1)
    , mGlobalWifiRunning(FALSE)
    , mBluetoothOn(FALSE)
    , mOnBattery(FALSE)
    , mOnBatteryInternal(FALSE)
    , mTrackBatteryPastUptime(0)
    , mTrackBatteryUptimeStart(0)
    , mTrackBatteryPastRealtime(0)
    , mTrackBatteryRealtimeStart(0)
    , mUnpluggedBatteryUptime(0)
    , mUnpluggedBatteryRealtime(0)
    , mDischargeStartLevel(0)
    , mDischargeUnplugLevel(0)
    , mDischargeCurrentLevel(0)
    , mLowDischargeAmountSinceCharge(0)
    , mHighDischargeAmountSinceCharge(0)
    , mDischargeScreenOnUnplugLevel(0)
    , mDischargeScreenOffUnplugLevel(0)
    , mDischargeAmountScreenOn(0)
    , mDischargeAmountScreenOnSinceCharge(0)
    , mDischargeAmountScreenOff(0)
    , mDischargeAmountScreenOffSinceCharge(0)
    , mLastWriteTime(0)
    , mRadioDataUptime(0)
    , mRadioDataStart(0)
    , mBluetoothPingCount(0)
    , mBluetoothPingStart(-1)
    , mPhoneServiceState(-1)
    , mPhoneServiceStateRaw(-1)
    , mPhoneSimStateRaw(-1)
    , mChangedBufferStates(0)
    , mChangedStates(0)
    , mWakeLockNesting(0)
    , mSensorNesting(0)
    , mGpsNesting(0)
    , mWifiFullLockNesting(0)
{
    CParcel::New((IParcel**)&mHistoryBuffer);
    mHistoryLastWritten = new HistoryItem();
    mHistoryLastLastWritten = new HistoryItem();
    mHistoryReadTmp = new HistoryItem();
    mHistoryCur = new HistoryItem();

    mScreenBrightnessTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_SCREEN_BRIGHTNESS_BINS);
    mPhoneSignalStrengthsTimer = ArrayOf<StopwatchTimer*>::Alloc(5/*ISignalStrength::NUM_SIGNAL_STRENGTH_BINS*/);
    mPhoneDataConnectionsTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_DATA_CONNECTION_TYPES);

    mMobileDataTx = ArrayOf<Int64>::Alloc(4);
    mMobileDataRx = ArrayOf<Int64>::Alloc(4);
    mTotalDataTx = ArrayOf<Int64>::Alloc(4);
    mTotalDataRx = ArrayOf<Int64>::Alloc(4);

    mProcWakelocksName = ArrayOf<String>::Alloc(3);
    mProcWakelocksData = ArrayOf<Int64>::Alloc(3);

    CNetworkStatsFactory::New((INetworkStatsFactory**)&mNetworkStatsFactory);

    CReentrantLock::New((IReentrantLock**)&mWriteLock);

    AutoPtr<IFile> f;
    CFile::New(filename, (IFile**)&f);
    CAtomicFile::New(f, (IAtomicFile**)&mFile);
    mHandler = new MyHandler(this);
    mStartCount++;
    mScreenOnTimer = new StopwatchTimer(NULL, -1, NULL, &mUnpluggables);
    for (Int32 i = 0; i < NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -100-i, NULL, &mUnpluggables);
        mScreenBrightnessTimer->Set(i, timer);
    }
    mInputEventCounter = new Counter(&mUnpluggables);
    mPhoneOnTimer = new StopwatchTimer(NULL, -2, NULL, &mUnpluggables);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -200-i, NULL, &mUnpluggables);
        mPhoneSignalStrengthsTimer->Set(i, timer);
    }
    mPhoneSignalScanningTimer = new StopwatchTimer(NULL, -200+1, NULL, &mUnpluggables);
    for (Int32 i = 0; i < NUM_DATA_CONNECTION_TYPES; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -300-i, NULL, &mUnpluggables);
        mPhoneDataConnectionsTimer->Set(i, timer);
    }
    mWifiOnTimer = new StopwatchTimer(NULL, -3, NULL, &mUnpluggables);
    mGlobalWifiRunningTimer = new StopwatchTimer(NULL, -4, NULL, &mUnpluggables);
    mBluetoothOnTimer = new StopwatchTimer(NULL, -5, NULL, &mUnpluggables);
    mAudioOnTimer = new StopwatchTimer(NULL, -6, NULL, &mUnpluggables);
    mVideoOnTimer = new StopwatchTimer(NULL, -7, NULL, &mUnpluggables);
    InitTimes();
    mUptimeStart = mTrackBatteryUptimeStart = SystemClock::GetUptimeMillis() * 1000;
    mRealtimeStart = mTrackBatteryRealtimeStart = SystemClock::GetElapsedRealtime() * 1000;
    mUnpluggedBatteryUptime = GetBatteryUptimeLocked(mUptimeStart);
    mUnpluggedBatteryRealtime = GetBatteryRealtimeLocked(mRealtimeStart);
    InitDischarge();
    ClearHistoryLocked();
}

BatteryStatsImpl::BatteryStatsImpl(
    /* [in] */ IParcel* p)
    : mShuttingDown(FALSE)
    , mHistoryBaseTime(0)
    , mHaveBatteryLevel(FALSE)
    , mRecordingHistory(TRUE)
    , mNumHistoryItems(0)
    , mHistoryBufferLastPos(-1)
    , mHistoryOverflow(FALSE)
    , mLastHistoryTime(0)
    , mReadOverflow(FALSE)
    , mIteratingHistory(FALSE)
    , mStartCount(0)
    , mBatteryUptime(0)
    , mBatteryLastUptime(0)
    , mBatteryRealtime(0)
    , mBatteryLastRealtime(0)
    , mUptime(0)
    , mUptimeStart(0)
    , mLastUptime(0)
    , mRealtime(0)
    , mRealtimeStart(0)
    , mLastRealtime(0)
    , mScreenOn(FALSE)
    , mScreenBrightnessBin(-1)
    , mPhoneOn(FALSE)
    , mAudioOn(FALSE)
    , mVideoOn(FALSE)
    , mPhoneSignalStrengthBin(-1)
    , mPhoneSignalStrengthBinRaw(-1)
    , mPhoneDataConnectionType(-1)
    , mWifiOn(FALSE)
    , mWifiOnUid(-1)
    , mGlobalWifiRunning(FALSE)
    , mBluetoothOn(FALSE)
    , mOnBattery(FALSE)
    , mOnBatteryInternal(FALSE)
    , mTrackBatteryPastUptime(0)
    , mTrackBatteryUptimeStart(0)
    , mTrackBatteryPastRealtime(0)
    , mTrackBatteryRealtimeStart(0)
    , mUnpluggedBatteryUptime(0)
    , mUnpluggedBatteryRealtime(0)
    , mDischargeStartLevel(0)
    , mDischargeUnplugLevel(0)
    , mDischargeCurrentLevel(0)
    , mLowDischargeAmountSinceCharge(0)
    , mHighDischargeAmountSinceCharge(0)
    , mDischargeScreenOnUnplugLevel(0)
    , mDischargeScreenOffUnplugLevel(0)
    , mDischargeAmountScreenOn(0)
    , mDischargeAmountScreenOnSinceCharge(0)
    , mDischargeAmountScreenOff(0)
    , mDischargeAmountScreenOffSinceCharge(0)
    , mLastWriteTime(0)
    , mRadioDataUptime(0)
    , mRadioDataStart(0)
    , mBluetoothPingCount(0)
    , mBluetoothPingStart(-1)
    , mPhoneServiceState(-1)
    , mPhoneServiceStateRaw(-1)
    , mPhoneSimStateRaw(-1)
    , mChangedBufferStates(0)
    , mChangedStates(0)
    , mWakeLockNesting(0)
    , mSensorNesting(0)
    , mGpsNesting(0)
    , mWifiFullLockNesting(0)
{
    mHandler = new MyHandler(this);

    CParcel::New((IParcel**)&mHistoryBuffer);
    mHistoryLastWritten = new HistoryItem();
    mHistoryLastLastWritten = new HistoryItem();
    mHistoryReadTmp = new HistoryItem();
    mHistoryCur = new HistoryItem();

    mScreenBrightnessTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_SCREEN_BRIGHTNESS_BINS);
    mPhoneSignalStrengthsTimer = ArrayOf<StopwatchTimer*>::Alloc(5/*ISignalStrength::NUM_SIGNAL_STRENGTH_BINS*/);
    mPhoneDataConnectionsTimer = ArrayOf<StopwatchTimer*>::Alloc(NUM_DATA_CONNECTION_TYPES);

    mMobileDataTx = ArrayOf<Int64>::Alloc(4);
    mMobileDataRx = ArrayOf<Int64>::Alloc(4);
    mTotalDataTx = ArrayOf<Int64>::Alloc(4);
    mTotalDataRx = ArrayOf<Int64>::Alloc(4);

    mProcWakelocksName = ArrayOf<String>::Alloc(3);
    mProcWakelocksData = ArrayOf<Int64>::Alloc(3);

    CNetworkStatsFactory::New((INetworkStatsFactory**)&mNetworkStatsFactory);

    CReentrantLock::New((IReentrantLock**)&mWriteLock);

    ClearHistoryLocked();
    ReadFromParcel(p);
}

void BatteryStatsImpl::Lock()
{
    mLock.Lock();
}

void BatteryStatsImpl::Unlock()
{
    mLock.Unlock();
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

AutoPtr< HashMap<String, AutoPtr<BatteryStatsImpl::BatteryStatsTimer> > > BatteryStatsImpl::GetKernelWakelockStats()
{
    AutoPtr< HashMap<String, AutoPtr<BatteryStatsTimer> > > temp = new HashMap<String, AutoPtr<BatteryStatsTimer> >();
    HashMap<String, AutoPtr<SamplingTimer> >::Iterator it = mKernelWakelockStats.Begin();
    for (; it != mKernelWakelockStats.End(); ++it) {
        (*temp)[it->mFirst] = (BatteryStatsTimer*)it->mSecond;
    }
    return temp;
}

AutoPtr< HashMap<String, AutoPtr<Elastos::Droid::Os::BatteryStatsImpl::KernelWakelockStats> > > BatteryStatsImpl::ReadKernelWakelockStats()
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

    is->ReadBytes(buffer, &len);
    is->Close();
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

AutoPtr< HashMap<String, AutoPtr<Elastos::Droid::Os::BatteryStatsImpl::KernelWakelockStats> > > BatteryStatsImpl::ParseProcWakelocks(
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

    AutoLock lock(mLock);
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
    AutoPtr<SamplingTimer> kwlt;
    HashMap<String, AutoPtr<SamplingTimer> >::Iterator it = mKernelWakelockStats.Find(name);
    if (it != mKernelWakelockStats.End()) {
        kwlt = it->mSecond;
    }
    if (kwlt == NULL) {
        kwlt = new SamplingTimer(&mUnpluggables, mOnBatteryInternal, TRUE /* track reported values */);
        mKernelWakelockStats[name] = kwlt;
    }
    return kwlt;
}

void BatteryStatsImpl::DoDataPlug(
    /* [in] */ ArrayOf<Int64>* dataTransfer,
    /* [in] */ Int64 currentBytes)
{
    (*dataTransfer)[STATS_LAST] = (*dataTransfer)[STATS_SINCE_UNPLUGGED];
    (*dataTransfer)[STATS_SINCE_UNPLUGGED] = -1;
}

void BatteryStatsImpl::DoDataUnplug(
    /* [in] */ ArrayOf<Int64>* dataTransfer,
    /* [in] */ Int64 currentBytes)
{
    (*dataTransfer)[STATS_SINCE_UNPLUGGED] = currentBytes;
}

Int64 BatteryStatsImpl::GetCurrentRadioDataUptime()
{
    // try {
    AutoPtr<IFile> awakeTimeFile;
    if (FAILED(CFile::New(String("/sys/devices/virtual/net/rmnet0/awake_time_ms"), (IFile**)&awakeTimeFile))) {
        return 0;
    }
    Boolean isExists;
    if (awakeTimeFile->Exists(&isExists), !isExists) return 0;
    AutoPtr<IFileReader> fr;
    if (FAILED(CFileReader::New(awakeTimeFile, (IFileReader**)&fr))) {
        return 0;
    }
    AutoPtr<IBufferedReader> br;
    if (FAILED(CBufferedReader::New(fr, (IBufferedReader**)&br))) {
        return 0;
    }
    String line;
    if (FAILED(br->ReadLine(&line))) {
        return 0;
    }
    ICloseable::Probe(br)->Close();
    return StringUtils::ParseInt64(line) * 1000;
    // } catch (NumberFormatException nfe) {
    //     // Nothing
    // } catch (IOException ioe) {
    //     // Nothing
    // }
    // return 0;
}

Int64 BatteryStatsImpl::GetRadioDataUptimeMs()
{
    return GetRadioDataUptime() / 1000;
}

Int64 BatteryStatsImpl::GetRadioDataUptime()
{
    if (mRadioDataStart == -1) {
        return mRadioDataUptime;
    }
    else {
        return GetCurrentRadioDataUptime() - mRadioDataStart;
    }
}

Int32 BatteryStatsImpl::GetCurrentBluetoothPingCount()
{
    if (mBtHeadset != NULL) {
        AutoPtr< ArrayOf<IBluetoothDevice*> > deviceList;
        mBtHeadset->GetConnectedDevices((ArrayOf<IBluetoothDevice*>**)&deviceList);
        if (deviceList->GetLength() > 0) {
            Int32 result;
            mBtHeadset->GetBatteryUsageHint((*deviceList)[0], &result);
            return result;
        }
    }
    return -1;
}

Int32 BatteryStatsImpl::GetBluetoothPingCount()
{
    if (mBluetoothPingStart == -1) {
        return mBluetoothPingCount;
    }
    else if (mBtHeadset != NULL) {
        return GetCurrentBluetoothPingCount() - mBluetoothPingStart;
    }
    return 0;
}

void BatteryStatsImpl::SetBtHeadset(
    /* [in] */ IBluetoothHeadset* headset)
{
    if (headset != NULL && mBtHeadset == NULL && IsOnBattery() && mBluetoothPingStart == -1) {
        mBluetoothPingStart = GetCurrentBluetoothPingCount();
    }
    mBtHeadset = headset;
}

void BatteryStatsImpl::AddHistoryBufferLocked(
    /* [in] */ Int64 curTime)
{
    if (!mHaveBatteryLevel || !mRecordingHistory) {
        return;
    }

    Int64 timeDiff = (mHistoryBaseTime+curTime) - mHistoryLastWritten->mTime;
    if (mHistoryBufferLastPos >= 0 && mHistoryLastWritten->mCmd == HistoryItem::CMD_UPDATE
            && timeDiff < 2000
            && ((mHistoryLastWritten->mStates ^ mHistoryCur->mStates)&mChangedBufferStates) == 0) {
        // If the current is the same as the one before, then we no
        // longer need the entry.
        // mHistoryBuffer->SetDataSize(mHistoryBufferLastPos);
        mHistoryBuffer->SetDataPosition(mHistoryBufferLastPos);
        mHistoryBufferLastPos = -1;
        if (mHistoryLastLastWritten->mCmd == HistoryItem::CMD_UPDATE
                && timeDiff < 500 && mHistoryLastLastWritten->Same(mHistoryCur)) {
            // If this results in us returning to the state written
            // prior to the last one, then we can just delete the last
            // written one and drop the new one.  Nothing more to do.
            mHistoryLastWritten->SetTo(mHistoryLastLastWritten);
            mHistoryLastLastWritten->mCmd = HistoryItem::CMD_NULL;
            return;
        }
        mChangedBufferStates |= mHistoryLastWritten->mStates ^ mHistoryCur->mStates;
        curTime = mHistoryLastWritten->mTime - mHistoryBaseTime;
        mHistoryLastWritten->SetTo(mHistoryLastLastWritten);
    }
    else {
        mChangedBufferStates = 0;
    }

    Int32 dataSize;
    mHistoryBuffer->GetElementSize(&dataSize);
    if (dataSize >= MAX_HISTORY_BUFFER) {
        if (!mHistoryOverflow) {
            mHistoryOverflow = TRUE;
            AddHistoryBufferLocked(curTime, HistoryItem::CMD_OVERFLOW);
        }

        // Once we've reached the maximum number of items, we only
        // record changes to the battery level and the most interesting states.
        // Once we've reached the maximum maximum number of items, we only
        // record changes to the battery level.
        if (mHistoryLastWritten->mBatteryLevel == mHistoryCur->mBatteryLevel &&
                (dataSize >= MAX_MAX_HISTORY_BUFFER
                        || ((mHistoryLastWritten->mStates ^ mHistoryCur->mStates)
                                & HistoryItem::MOST_INTERESTING_STATES) == 0)) {
            return;
        }
    }

    AddHistoryBufferLocked(curTime, HistoryItem::CMD_UPDATE);
}

void BatteryStatsImpl::AddHistoryBufferLocked(
    /* [in] */ Int64 curTime,
    /* [in] */ Byte cmd)
{
    Int32 origPos = 0;
    if (mIteratingHistory) {
        mHistoryBuffer->GetDataPosition(&origPos);
        Int32 size;
        mHistoryBuffer->GetElementSize(&size);
        mHistoryBuffer->SetDataPosition(size);
    }
    mHistoryBuffer->GetDataPosition(&mHistoryBufferLastPos);
    mHistoryLastLastWritten->SetTo(mHistoryLastWritten);
    mHistoryLastWritten->SetTo(mHistoryBaseTime + curTime, cmd, mHistoryCur);
    mHistoryLastWritten->WriteDelta(mHistoryBuffer, mHistoryLastLastWritten);
    mLastHistoryTime = curTime;
    if (DEBUG_HISTORY) {
        StringBuilder sb("Writing history buffer: was ");
        sb += mHistoryBufferLastPos;
        sb += " now ";
        Int32 position;
        mHistoryBuffer->GetDataPosition(&position);
        sb += position;
        sb += " size is now ";
        Int32 size;
        mHistoryBuffer->GetElementSize(&size);
        sb += size;
        Logger::I(TAG, sb.ToString());
    }
    if (mIteratingHistory) {
        mHistoryBuffer->SetDataPosition(origPos);
    }
}

void BatteryStatsImpl::AddHistoryRecordLocked(
    /* [in] */ Int64 curTime)
{
    AddHistoryBufferLocked(curTime);

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
            && (mHistoryBaseTime+curTime) < (mHistoryEnd->mTime + 2000)
            && ((mHistoryEnd->mStates ^ mHistoryCur->mStates) & mChangedStates) == 0) {
        // If the current is the same as the one before, then we no
        // longer need the entry.
        if (mHistoryLastEnd != NULL && mHistoryLastEnd->mCmd == HistoryItem::CMD_UPDATE
                && (mHistoryBaseTime + curTime) < (mHistoryEnd->mTime + 500)
                && mHistoryLastEnd->Same(mHistoryCur)) {
            mHistoryLastEnd->mNext = NULL;
            mHistoryEnd->mNext = mHistoryCache;
            mHistoryCache = mHistoryEnd;
            mHistoryEnd = mHistoryLastEnd;
            mHistoryLastEnd = NULL;
        }
        else {
            mChangedStates |= mHistoryEnd->mStates ^ mHistoryCur->mStates;
            mHistoryEnd->SetTo(mHistoryEnd->mTime, HistoryItem::CMD_UPDATE, mHistoryCur);
        }
        return;
    }

    mChangedStates = 0;

    if (mNumHistoryItems == MAX_HISTORY_ITEMS
            || mNumHistoryItems == MAX_MAX_HISTORY_ITEMS) {
        AddHistoryRecordLocked(curTime, HistoryItem::CMD_OVERFLOW);
    }

    if (mNumHistoryItems >= MAX_HISTORY_ITEMS) {
        // Once we've reached the maximum number of items, we only
        // record changes to the battery level and the most interesting states.
        // Once we've reached the maximum maximum number of items, we only
        // record changes to the battery level.
        if (mHistoryEnd != NULL && mHistoryEnd->mBatteryLevel
                == mHistoryCur->mBatteryLevel &&
                (mNumHistoryItems >= MAX_MAX_HISTORY_ITEMS
                        || ((mHistoryEnd->mStates ^ mHistoryCur->mStates)
                                & HistoryItem::MOST_INTERESTING_STATES) == 0)) {
            return;
        }
    }

    AddHistoryRecordLocked(curTime, HistoryItem::CMD_UPDATE);
}

void BatteryStatsImpl::AddHistoryRecordLocked(
    /* [in] */ Int64 curTime,
    /* [in] */ Byte cmd)
{
    AutoPtr<HistoryItem> rec = mHistoryCache;
    if (rec != NULL) {
        mHistoryCache = rec->mNext;
    }
    else {
        rec = new HistoryItem();
    }
    rec->SetTo(mHistoryBaseTime + curTime, cmd, mHistoryCur);

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
    mLastHistoryTime = 0;

    // mHistoryBuffer->SetDataSize(0);
    mHistoryBuffer->SetDataPosition(0);
    // mHistoryBuffer->SetDataCapacity(MAX_HISTORY_BUFFER / 2);
    mHistoryLastLastWritten->mCmd = HistoryItem::CMD_NULL;
    mHistoryLastWritten->mCmd = HistoryItem::CMD_NULL;
    mHistoryBufferLastPos = -1;
    mHistoryOverflow = FALSE;
}

void BatteryStatsImpl::DoUnplugLocked(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    AutoPtr<INetworkStatsEntry> entry;
    // Track UID data usage
    AutoPtr<INetworkStats> uidStats = GetNetworkStatsDetailGroupedByUid();
    Int32 size;
    uidStats->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<INetworkStatsEntry> temp;
        uidStats->GetValues(i, entry, (INetworkStatsEntry**)&temp);
        entry = temp;

        AutoPtr<Uid> u;
        Int32 uid;
        entry->GetUid(&uid);
        HashMap<Int32, AutoPtr<Uid> >::Iterator it = mUidStats.Find(uid);
        if (it == mUidStats.End()) continue;
        u = it->mSecond;

        entry->GetRxBytes(&u->mStartedTcpBytesReceived);
        entry->GetTxBytes(&u->mStartedTcpBytesSent);
        u->mTcpBytesReceivedAtLastUnplug = u->mCurrentTcpBytesReceived;
        u->mTcpBytesSentAtLastUnplug = u->mCurrentTcpBytesSent;
    }

    List<AutoPtr<IUnpluggable> >::ReverseIterator rit = mUnpluggables.RBegin();
    for (; rit != mUnpluggables.REnd(); ++rit) {
        (*rit)->Unplug(batteryUptime, batteryRealtime);
    }

    // Track both mobile and total overall data
    AutoPtr<INetworkStats> ifaceStats = GetNetworkStatsSummary();
    AutoPtr<IObjectContainer> container;
    CObjectContainer::New((IObjectContainer**)&container);
    HashSet<String>::Iterator it = mMobileIfaces.Begin();
    for (; it != mMobileIfaces.End(); ++it) {
        AutoPtr<ICharSequence> cs;
        CString::New(*it, (ICharSequence**)&cs);
        container->Add((IInterface*)cs);
    }

    AutoPtr<INetworkStatsEntry> temp;
    ifaceStats->GetTotal(entry, container, (INetworkStatsEntry**)&temp);
    entry = temp;
    Int64 rxBytes, txBytes;
    entry->GetRxBytes(&rxBytes);
    entry->GetTxBytes(&txBytes);
    DoDataUnplug(mMobileDataRx, rxBytes);
    DoDataUnplug(mMobileDataTx, txBytes);

    temp = NULL;
    ifaceStats->GetTotal(entry, (INetworkStatsEntry**)&temp);
    entry = temp;
    entry->GetRxBytes(&rxBytes);
    entry->GetTxBytes(&txBytes);
    DoDataUnplug(mTotalDataRx, rxBytes);
    DoDataUnplug(mTotalDataTx, txBytes);

    // Track radio awake time
    mRadioDataStart = GetCurrentRadioDataUptime();
    mRadioDataUptime = 0;

    // Track bt headset ping count
    mBluetoothPingStart = GetCurrentBluetoothPingCount();
    mBluetoothPingCount = 0;
}

void BatteryStatsImpl::DoPlugLocked(
    /* [in] */ Int64 batteryUptime,
    /* [in] */ Int64 batteryRealtime)
{
    for (Int32 iu = mUidStats.GetSize() - 1; iu >= 0; iu--) {
        AutoPtr<Uid> u = mUidStats[iu];
        if (u->mStartedTcpBytesReceived >= 0) {
            u->mCurrentTcpBytesReceived = u->ComputeCurrentTcpBytesReceived();
            u->mStartedTcpBytesReceived = -1;
        }
        if (u->mStartedTcpBytesSent >= 0) {
            u->mCurrentTcpBytesSent = u->ComputeCurrentTcpBytesSent();
            u->mStartedTcpBytesSent = -1;
        }
    }

    List<AutoPtr<IUnpluggable> >::ReverseIterator rit = mUnpluggables.RBegin();
    for (; rit != mUnpluggables.REnd(); ++rit) {
        (*rit)->Plug(batteryUptime, batteryRealtime);
    }

    // Track both mobile and total overall data
    AutoPtr<INetworkStats> ifaceStats = GetNetworkStatsSummary();
    AutoPtr<IObjectContainer> container;
    CObjectContainer::New((IObjectContainer**)&container);
    HashSet<String>::Iterator it = mMobileIfaces.Begin();
    for (; it != mMobileIfaces.End(); ++it) {
        AutoPtr<ICharSequence> cs;
        CString::New(*it, (ICharSequence**)&cs);
        container->Add((IInterface*)cs);
    }

    AutoPtr<INetworkStatsEntry> entry;
    ifaceStats->GetTotal(NULL, container, (INetworkStatsEntry**)&entry);
    Int64 rxBytes, txBytes;
    entry->GetRxBytes(&rxBytes);
    entry->GetTxBytes(&txBytes);
    DoDataPlug(mMobileDataRx, rxBytes);
    DoDataPlug(mMobileDataTx, txBytes);

    AutoPtr<INetworkStatsEntry> temp;
    ifaceStats->GetTotal(entry, (INetworkStatsEntry**)&temp);
    entry = temp;
    entry->GetRxBytes(&rxBytes);
    entry->GetTxBytes(&txBytes);
    DoDataPlug(mTotalDataRx, rxBytes);
    DoDataPlug(mTotalDataTx, txBytes);

    // Track radio awake time
    mRadioDataUptime = GetRadioDataUptime();
    mRadioDataStart = -1;

    // Track bt headset ping count
    mBluetoothPingCount = GetBluetoothPingCount();
    mBluetoothPingStart = -1;
}

void BatteryStatsImpl::NoteStartWakeLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    if (type == WAKE_TYPE_PARTIAL) {
        // Only care about partial wake locks, since full wake locks
        // will be canceled when the user puts the screen to sleep.
        if (mWakeLockNesting == 0) {
            mHistoryCur->mStates |= HistoryItem::STATE_WAKE_LOCK_FLAG;
            if (DEBUG_HISTORY) {
                Logger::V(TAG, "Start wake lock to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
            }
            AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        }
        mWakeLockNesting++;
    }
    if (uid >= 0) {
        Boolean bval;
        if (mHandler->HasMessages(MSG_UPDATE_WAKELOCKS, &bval), !bval) {
            mHandler->SendEmptyMessageDelayed(MSG_UPDATE_WAKELOCKS,
                DELAY_UPDATE_WAKELOCKS, &bval);
        }

        GetUidStatsLocked(uid)->NoteStartWakeLocked(pid, name, type);
    }
}

void BatteryStatsImpl::NoteStopWakeLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    if (type == WAKE_TYPE_PARTIAL) {
        mWakeLockNesting--;
        if (mWakeLockNesting == 0) {
            mHistoryCur->mStates &= ~HistoryItem::STATE_WAKE_LOCK_FLAG;
            if (DEBUG_HISTORY) {
                Logger::V(TAG, "Stop wake lock to: ", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
            }
            AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        }
    }
    if (uid >= 0) {
        Boolean bval;
        if (mHandler->HasMessages(MSG_UPDATE_WAKELOCKS, &bval), !bval) {
            mHandler->SendEmptyMessageDelayed(DELAY_UPDATE_WAKELOCKS,
                DELAY_UPDATE_WAKELOCKS, &bval);
        }
        GetUidStatsLocked(uid)->NoteStopWakeLocked(pid, name, type);
    }
}

void BatteryStatsImpl::NoteStartWakeFromSourceLocked(
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteStartWakeLocked(value, pid, name, type);
    }
}

void BatteryStatsImpl::NoteStopWakeFromSourceLocked(
    /* [in] */ IWorkSource* ws,
    /* [in] */ Int32 pid,
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteStopWakeLocked(value, pid, name, type);
    }
}

Int32 BatteryStatsImpl::StartAddingCpuLocked()
{
    mHandler->RemoveMessages(MSG_UPDATE_WAKELOCKS);

    if (mScreenOn) {
        return 0;
    }

    if (mPartialTimers.Begin() == mPartialTimers.End()) {
        mLastPartialTimers.Clear();
        return 0;
    }

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
                return 50;
            }
        }
    }

    return 0;
}

void BatteryStatsImpl::FinishAddingCpuLocked(
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
        return;
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
}

void BatteryStatsImpl::NoteProcessDiedLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    AutoPtr<Uid> u;
    HashMap<Int32, AutoPtr<Uid> >::Iterator it = mUidStats.Find(uid);
    if (it != mUidStats.End()) {
        u = it->mSecond;
    }
    if (u != NULL) {
        u->mPids.Erase(pid);
    }
}

Int64 BatteryStatsImpl::GetProcessWakeTime(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid,
    /* [in] */ Int64 realtime)
{
    AutoPtr<Uid> u;
    HashMap<Int32, AutoPtr<Uid> >::Iterator it = mUidStats.Find(uid);
    if (it != mUidStats.End()) {
        u = it->mSecond;
    }
    if (u != NULL) {
        AutoPtr<Uid::Pid> p;
        HashMap<Int32, AutoPtr<Uid::Pid> >::Iterator pit = u->mPids.Find(pid);
        if (pit != u->mPids.End()) {
            p = pit->mSecond;
        }
        if (p != NULL) {
            return p->mWakeSum + (p->mWakeStart != 0 ? (realtime - p->mWakeStart) : 0);
        }
    }
    return 0;
}

void BatteryStatsImpl::ReportExcessiveWakeLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& proc,
    /* [in] */ Int64 overTime,
    /* [in] */ Int64 usedTime)
{
    AutoPtr<Uid> u;
    HashMap<Int32, AutoPtr<Uid> >::Iterator it = mUidStats.Find(uid);
    if (it != mUidStats.End()) {
        u = it->mSecond;
    }
    if (u != NULL) {
        u->ReportExcessiveWakeLocked(proc, overTime, usedTime);
    }
}

void BatteryStatsImpl::ReportExcessiveCpuLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& proc,
    /* [in] */ Int64 overTime,
    /* [in] */ Int64 usedTime)
{
    AutoPtr<Uid> u;
    HashMap<Int32, AutoPtr<Uid> >::Iterator it = mUidStats.Find(uid);
    if (it != mUidStats.End()) {
        u = it->mSecond;
    }
    if (u != NULL) {
        u->ReportExcessiveCpuLocked(proc, overTime, usedTime);
    }
}

void BatteryStatsImpl::NoteStartSensorLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 sensor)
{
    if (mSensorNesting == 0) {
        mHistoryCur->mStates |= HistoryItem::STATE_SENSOR_ON_FLAG;
        if (DEBUG_HISTORY) {
            Logger::V(TAG, "Start sensor to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        }
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
    }
    mSensorNesting++;
    GetUidStatsLocked(uid)->NoteStartSensor(sensor);
}

void BatteryStatsImpl::NoteStopSensorLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 sensor)
{
    mSensorNesting--;
    if (mSensorNesting == 0) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_SENSOR_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Stop sensor to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
    }
    GetUidStatsLocked(uid)->NoteStopSensor(sensor);
}

void BatteryStatsImpl::NoteStartGpsLocked(
    /* [in] */ Int32 uid)
{
    if (mGpsNesting == 0) {
        mHistoryCur->mStates |= HistoryItem::STATE_GPS_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Start GPS to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
    }
    mGpsNesting++;
    GetUidStatsLocked(uid)->NoteStartGps();
}

void BatteryStatsImpl::NoteStopGpsLocked(
    /* [in] */ Int32 uid)
{
    mGpsNesting--;
    if (mGpsNesting == 0) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_GPS_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Stop GPS to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
    }
    GetUidStatsLocked(uid)->NoteStopGps();
}

void BatteryStatsImpl::NoteScreenOnLocked()
{
    if (!mScreenOn) {
        mHistoryCur->mStates |= HistoryItem::STATE_SCREEN_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Screen on to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mScreenOn = TRUE;
        mScreenOnTimer->StartRunningLocked(this);
        if (mScreenBrightnessBin >= 0) {
            (*mScreenBrightnessTimer)[mScreenBrightnessBin]->StartRunningLocked(this);
        }

        // Fake a wake lock, so we consider the device waked as long
        // as the screen is on.
        NoteStartWakeLocked(-1, -1, String("dummy"), WAKE_TYPE_PARTIAL);

        // Update discharge amounts.
        if (mOnBatteryInternal) {
            UpdateDischargeScreenLevelsLocked(FALSE, TRUE);
        }
    }
}

void BatteryStatsImpl::NoteScreenOffLocked()
{
    if (mScreenOn) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_SCREEN_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Screen off to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mScreenOn = FALSE;
        mScreenOnTimer->StopRunningLocked(this);
        if (mScreenBrightnessBin >= 0) {
            (*mScreenBrightnessTimer)[mScreenBrightnessBin]->StopRunningLocked(this);
        }

        NoteStopWakeLocked(-1, -1, String("dummy"), WAKE_TYPE_PARTIAL);

        // Update discharge amounts.
        if (mOnBatteryInternal) {
            UpdateDischargeScreenLevelsLocked(TRUE, FALSE);
        }
    }
}

void BatteryStatsImpl::NoteScreenBrightnessLocked(
    /* [in] */ Int32 brightness)
{
    // Bin the brightness.
    Int32 bin = brightness / (256 / NUM_SCREEN_BRIGHTNESS_BINS);
    if (bin < 0) bin = 0;
    else if (bin >= NUM_SCREEN_BRIGHTNESS_BINS) bin = NUM_SCREEN_BRIGHTNESS_BINS - 1;
    if (mScreenBrightnessBin != bin) {
        mHistoryCur->mStates = (mHistoryCur->mStates & ~HistoryItem::STATE_BRIGHTNESS_MASK)
                | (bin << HistoryItem::STATE_BRIGHTNESS_SHIFT);
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Screen brightness %d to: %s", bin, StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        if (mScreenOn) {
            if (mScreenBrightnessBin >= 0) {
                (*mScreenBrightnessTimer)[mScreenBrightnessBin]->StopRunningLocked(this);
            }
            (*mScreenBrightnessTimer)[bin]->StartRunningLocked(this);
        }
        mScreenBrightnessBin = bin;
    }
}

void BatteryStatsImpl::NoteInputEventAtomic()
{
    mInputEventCounter->StepAtomic();
}

void BatteryStatsImpl::NoteUserActivityLocked(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 event)
{
    GetUidStatsLocked(uid)->NoteUserActivityLocked(event);
}

void BatteryStatsImpl::NotePhoneOnLocked()
{
    if (!mPhoneOn) {
        mHistoryCur->mStates |= HistoryItem::STATE_PHONE_IN_CALL_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Phone on to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mPhoneOn = TRUE;
        mPhoneOnTimer->StartRunningLocked(this);
    }
}

void BatteryStatsImpl::NotePhoneOffLocked()
{
    if (mPhoneOn) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_PHONE_IN_CALL_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Phone off to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mPhoneOn = FALSE;
        mPhoneOnTimer->StopRunningLocked(this);
    }
}

void BatteryStatsImpl::StopAllSignalStrengthTimersLocked(
        /* [in] */ Int32 except)
{
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        if (i == except) {
            continue;
        }
        while ((*mPhoneSignalStrengthsTimer)[i]->IsRunningLocked()) {
            (*mPhoneSignalStrengthsTimer)[i]->StopRunningLocked(this);
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
    /* [in] */ Int32 bin)
{
    Boolean scanning = FALSE;
    Boolean newHistory = FALSE;

    mPhoneServiceStateRaw = state;
    mPhoneSimStateRaw = simState;
    mPhoneSignalStrengthBinRaw = bin;

    if (simState == ITelephonyManager::SIM_STATE_ABSENT) {
        // In this case we will always be STATE_OUT_OF_SERVICE, so need
        // to infer that we are scanning from other data.
        if (state == IServiceState::STATE_OUT_OF_SERVICE
                && bin > ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN) {
            state = IServiceState::STATE_IN_SERVICE;
        }
    }

    // If the phone is powered off, stop all timers.
    if (state == IServiceState::STATE_POWER_OFF) {
        bin = -1;

    // If we are in service, make sure the correct signal string timer is running.
    }
    else if (state == IServiceState::STATE_IN_SERVICE) {
        // Bin will be changed below.

    // If we're out of service, we are in the lowest signal strength
    // bin and have the scanning bit set.
    }
    else if (state == IServiceState::STATE_OUT_OF_SERVICE) {
        scanning = TRUE;
        bin = ISignalStrength::SIGNAL_STRENGTH_NONE_OR_UNKNOWN;
        if (!mPhoneSignalScanningTimer->IsRunningLocked()) {
            mHistoryCur->mStates |= HistoryItem::STATE_PHONE_SCANNING_FLAG;
            newHistory = TRUE;
            if (DEBUG_HISTORY)
                Logger::V(TAG, "Phone started scanning to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
            mPhoneSignalScanningTimer->StartRunningLocked(this);
        }
    }

    if (!scanning) {
        // If we are no longer scanning, then stop the scanning timer.
        if (mPhoneSignalScanningTimer->IsRunningLocked()) {
            mHistoryCur->mStates &= ~HistoryItem::STATE_PHONE_SCANNING_FLAG;
            if (DEBUG_HISTORY)
                Logger::V(TAG, "Phone stopped scanning to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
            newHistory = TRUE;
            mPhoneSignalScanningTimer->StopRunningLocked(this);
        }
    }

    if (mPhoneServiceState != state) {
        mHistoryCur->mStates = (mHistoryCur->mStates & ~HistoryItem::STATE_PHONE_STATE_MASK)
                | (state << HistoryItem::STATE_PHONE_STATE_SHIFT);
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Phone state %d to: %s", state, StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        newHistory = TRUE;
        mPhoneServiceState = state;
    }

    if (mPhoneSignalStrengthBin != bin) {
        if (mPhoneSignalStrengthBin >= 0) {
            (*mPhoneSignalStrengthsTimer)[mPhoneSignalStrengthBin]->StopRunningLocked(this);
        }
        if (bin >= 0) {
            if (!(*mPhoneSignalStrengthsTimer)[bin]->IsRunningLocked()) {
                (*mPhoneSignalStrengthsTimer)[bin]->StartRunningLocked(this);
            }
            mHistoryCur->mStates = (mHistoryCur->mStates & ~HistoryItem::STATE_SIGNAL_STRENGTH_MASK)
                    | (bin << HistoryItem::STATE_SIGNAL_STRENGTH_SHIFT);
            if (DEBUG_HISTORY)
                Logger::V(TAG, "Signal strength %d to: %s", bin, StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
            newHistory = TRUE;
        }
        else {
            StopAllSignalStrengthTimersLocked(-1);
        }
        mPhoneSignalStrengthBin = bin;
    }

    if (newHistory) {
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
    }
}

void BatteryStatsImpl::NotePhoneStateLocked(
    /* [in] */ Int32 state,
    /* [in] */ Int32 simState)
{
    UpdateAllPhoneStateLocked(state, simState, mPhoneSignalStrengthBinRaw);
}

void BatteryStatsImpl::NotePhoneSignalStrengthLocked(
    /* [in] */ ISignalStrength* signalStrength)
{
    // Bin the strength.
    Int32 bin;
    signalStrength->GetLevel(&bin);
    UpdateAllPhoneStateLocked(mPhoneServiceStateRaw, mPhoneSimStateRaw, bin);
}

void BatteryStatsImpl::NotePhoneDataConnectionStateLocked(
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
            default:
                bin = DATA_CONNECTION_OTHER;
                break;
        }
    }
    if (DEBUG) Logger::I(TAG, "Phone Data Connection -> %d = %d", dataType, hasData);
    if (mPhoneDataConnectionType != bin) {
        mHistoryCur->mStates = (mHistoryCur->mStates & ~HistoryItem::STATE_DATA_CONNECTION_MASK)
                | (bin << HistoryItem::STATE_DATA_CONNECTION_SHIFT);
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Data connection %d to: %s", bin, StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        if (mPhoneDataConnectionType >= 0) {
            (*mPhoneDataConnectionsTimer)[mPhoneDataConnectionType]->StopRunningLocked(this);
        }
        mPhoneDataConnectionType = bin;
        (*mPhoneDataConnectionsTimer)[bin]->StartRunningLocked(this);
    }
}

void BatteryStatsImpl::NoteWifiOnLocked()
{
    if (!mWifiOn) {
        mHistoryCur->mStates |= HistoryItem::STATE_WIFI_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI on to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mWifiOn = TRUE;
        mWifiOnTimer->StartRunningLocked(this);
    }
}

void BatteryStatsImpl::NoteWifiOffLocked()
{
    if (mWifiOn) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_WIFI_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI off to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mWifiOn = FALSE;
        mWifiOnTimer->StopRunningLocked(this);
    }
    if (mWifiOnUid >= 0) {
        GetUidStatsLocked(mWifiOnUid)->NoteWifiStoppedLocked();
        mWifiOnUid = -1;
    }
}

void BatteryStatsImpl::NoteAudioOnLocked(
    /* [in] */ Int32 uid)
{
    if (!mAudioOn) {
        mHistoryCur->mStates |= HistoryItem::STATE_AUDIO_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Audio on to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mAudioOn = TRUE;
        mAudioOnTimer->StartRunningLocked(this);
    }
    GetUidStatsLocked(uid)->NoteAudioTurnedOnLocked();
}

void BatteryStatsImpl::NoteAudioOffLocked(
    /* [in] */ Int32 uid)
{
    if (mAudioOn) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_AUDIO_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Audio off to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mAudioOn = FALSE;
        mAudioOnTimer->StopRunningLocked(this);
    }
    GetUidStatsLocked(uid)->NoteAudioTurnedOffLocked();
}

void BatteryStatsImpl::NoteVideoOnLocked(
    /* [in] */ Int32 uid)
{
    if (!mVideoOn) {
        mHistoryCur->mStates |= HistoryItem::STATE_VIDEO_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Video on to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mVideoOn = TRUE;
        mVideoOnTimer->StartRunningLocked(this);
    }
    GetUidStatsLocked(uid)->NoteVideoTurnedOnLocked();
}

void BatteryStatsImpl::NoteVideoOffLocked(
    /* [in] */ Int32 uid)
{
    if (mVideoOn) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_VIDEO_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Video off to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mVideoOn = FALSE;
        mVideoOnTimer->StopRunningLocked(this);
    }
    GetUidStatsLocked(uid)->NoteVideoTurnedOffLocked();
}

void BatteryStatsImpl::NoteWifiRunningLocked(
    /* [in] */ IWorkSource* ws)
{
    if (!mGlobalWifiRunning) {
        mHistoryCur->mStates |= HistoryItem::STATE_WIFI_RUNNING_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI running to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mGlobalWifiRunning = TRUE;
        mGlobalWifiRunningTimer->StartRunningLocked(this);
        Int32 N;
        ws->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            Int32 value;
            ws->Get(i, &value);
            GetUidStatsLocked(value)->NoteWifiRunningLocked();
        }
    }
    else {
        Logger::W(TAG, "noteWifiRunningLocked -- called while WIFI running");
    }
}

void BatteryStatsImpl::NoteWifiRunningChangedLocked(
    /* [in] */ IWorkSource* oldWs,
    /* [in] */ IWorkSource* newWs)
{
    if (mGlobalWifiRunning) {
        Int32 N;
        oldWs->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            Int32 value;
            oldWs->Get(i, &value);
            GetUidStatsLocked(value)->NoteWifiStoppedLocked();
        }
        newWs->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            Int32 value;
            newWs->Get(i, &value);
            GetUidStatsLocked(value)->NoteWifiRunningLocked();
        }
    }
    else {
        Logger::W(TAG, "noteWifiRunningChangedLocked -- called while WIFI not running");
    }
}

void BatteryStatsImpl::NoteWifiStoppedLocked(
    /* [in] */ IWorkSource* ws)
{
    if (mGlobalWifiRunning) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_WIFI_RUNNING_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI stopped to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mGlobalWifiRunning = FALSE;
        mGlobalWifiRunningTimer->StopRunningLocked(this);
        Int32 N;
        ws->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            Int32 value;
            ws->Get(i, &value);
            GetUidStatsLocked(value)->NoteWifiStoppedLocked();
        }
    }
    else {
        Logger::W(TAG, "noteWifiStoppedLocked -- called while WIFI not running");
    }
}

void BatteryStatsImpl::NoteBluetoothOnLocked()
{
    if (!mBluetoothOn) {
        mHistoryCur->mStates |= HistoryItem::STATE_BLUETOOTH_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Bluetooth on to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mBluetoothOn = TRUE;
        mBluetoothOnTimer->StartRunningLocked(this);
    }
}

void BatteryStatsImpl::NoteBluetoothOffLocked()
{
    if (mBluetoothOn) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_BLUETOOTH_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Bluetooth off to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        mBluetoothOn = FALSE;
        mBluetoothOnTimer->StopRunningLocked(this);
    }
}

void BatteryStatsImpl::NoteFullWifiLockAcquiredLocked(
    /* [in] */ Int32 uid)
{
    if (mWifiFullLockNesting == 0) {
        mHistoryCur->mStates |= HistoryItem::STATE_WIFI_FULL_LOCK_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI full lock on to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
    }
    mWifiFullLockNesting++;
    GetUidStatsLocked(uid)->NoteFullWifiLockAcquiredLocked();
}

void BatteryStatsImpl::NoteFullWifiLockReleasedLocked(
    /* [in] */ Int32 uid)
{
    mWifiFullLockNesting--;
    if (mWifiFullLockNesting == 0) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_WIFI_FULL_LOCK_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI full lock off to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
    }
    GetUidStatsLocked(uid)->NoteFullWifiLockReleasedLocked();
}

void BatteryStatsImpl::NoteWifiScanStartedLocked(
    /* [in] */ Int32 uid)
{
    if (mWifiScanNesting == 0) {
        mHistoryCur->mStates |= HistoryItem::STATE_WIFI_SCAN_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI full lock on to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
    }
    mWifiScanNesting++;
    GetUidStatsLocked(uid)->NoteWifiScanStartedLocked();
}

void BatteryStatsImpl::NoteWifiScanStoppedLocked(
    /* [in] */ Int32 uid)
{
    mWifiScanNesting--;
    if (mWifiScanNesting == 0) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_WIFI_SCAN_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI full lock on to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
    }
    GetUidStatsLocked(uid)->NoteWifiScanStoppedLocked();
}

void BatteryStatsImpl::NoteWifiMulticastEnabledLocked(
    /* [in] */ Int32 uid)
{
    if (mWifiMulticastNesting == 0) {
        mHistoryCur->mStates |= HistoryItem::STATE_WIFI_MULTICAST_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI multicast on to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
    }
    mWifiMulticastNesting++;
    GetUidStatsLocked(uid)->NoteWifiMulticastEnabledLocked();
}

void BatteryStatsImpl::NoteWifiMulticastDisabledLocked(
    /* [in] */ Int32 uid)
{
    mWifiMulticastNesting--;
    if (mWifiMulticastNesting == 0) {
        mHistoryCur->mStates &= ~HistoryItem::STATE_WIFI_MULTICAST_ON_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "WIFI multicast off to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
    }
    GetUidStatsLocked(uid)->NoteWifiMulticastDisabledLocked();
}

void BatteryStatsImpl::NoteFullWifiLockAcquiredFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteFullWifiLockAcquiredLocked(value);
    }
}

void BatteryStatsImpl::NoteFullWifiLockReleasedFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteFullWifiLockReleasedLocked(value);
    }
}

void BatteryStatsImpl::NoteWifiScanStartedFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteWifiScanStartedLocked(value);
    }
}

void BatteryStatsImpl::NoteWifiScanStoppedFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteWifiScanStoppedLocked(value);
    }
}

void BatteryStatsImpl::NoteWifiMulticastEnabledFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteWifiMulticastEnabledLocked(value);
    }
}

void BatteryStatsImpl::NoteWifiMulticastDisabledFromSourceLocked(
    /* [in] */ IWorkSource* ws)
{
    Int32 N;
    ws->GetSize(&N);
    for (Int32 i = 0; i < N; i++) {
        Int32 value;
        ws->Get(i, &value);
        NoteWifiMulticastDisabledLocked(value);
    }
}

void BatteryStatsImpl::NoteNetworkInterfaceTypeLocked(
    /* [in] */ const String& iface,
    /* [in] */ Int32 networkType)
{
    AutoPtr<IConnectivityManagerHelper> helper;
    CConnectivityManagerHelper::AcquireSingleton((IConnectivityManagerHelper**)&helper);
    Boolean result;
    if (helper->IsNetworkTypeMobile(networkType, &result), result) {
        mMobileIfaces.Insert(iface);
    }
    else {
        mMobileIfaces.Erase(iface);
    }
}

Int64 BatteryStatsImpl::GetScreenOnTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    return mScreenOnTimer->GetTotalTimeLocked(batteryRealtime, which);
}

Int64 BatteryStatsImpl::GetScreenBrightnessTime(
    /* [in] */ Int32 brightnessBin,
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    return (*mScreenBrightnessTimer)[brightnessBin]->GetTotalTimeLocked(
            batteryRealtime, which);
}

Int32 BatteryStatsImpl::GetInputEventCount(
    /* [in] */ Int32 which)
{
    return mInputEventCounter->GetCountLocked(which);
}

Int64 BatteryStatsImpl::GetPhoneOnTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    return mPhoneOnTimer->GetTotalTimeLocked(batteryRealtime, which);
}

Int64 BatteryStatsImpl::GetPhoneSignalStrengthTime(
    /* [in] */ Int32 strengthBin,
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    return (*mPhoneSignalStrengthsTimer)[strengthBin]->GetTotalTimeLocked(
            batteryRealtime, which);
}

Int64 BatteryStatsImpl::GetPhoneSignalScanningTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    return mPhoneSignalScanningTimer->GetTotalTimeLocked(
            batteryRealtime, which);
}

Int32 BatteryStatsImpl::GetPhoneSignalStrengthCount(
    /* [in] */ Int32 strengthBin,
    /* [in] */ Int32 which)
{
    return (*mPhoneSignalStrengthsTimer)[strengthBin]->GetCountLocked(which);
}

Int64 BatteryStatsImpl::GetPhoneDataConnectionTime(
    /* [in] */ Int32 dataType,
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    return (*mPhoneDataConnectionsTimer)[dataType]->GetTotalTimeLocked(
            batteryRealtime, which);
}

Int32 BatteryStatsImpl::GetPhoneDataConnectionCount(
    /* [in] */ Int32 dataType,
    /* [in] */ Int32 which)
{
    return (*mPhoneDataConnectionsTimer)[dataType]->GetCountLocked(which);
}

Int64 BatteryStatsImpl::GetWifiOnTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    return mWifiOnTimer->GetTotalTimeLocked(batteryRealtime, which);
}

Int64 BatteryStatsImpl::GetGlobalWifiRunningTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    return mGlobalWifiRunningTimer->GetTotalTimeLocked(batteryRealtime, which);
}

Int64 BatteryStatsImpl::GetBluetoothOnTime(
    /* [in] */ Int64 batteryRealtime,
    /* [in] */ Int32 which)
{
    return mBluetoothOnTimer->GetTotalTimeLocked(batteryRealtime, which);
}

Boolean BatteryStatsImpl::GetIsOnBattery()
{
    return mOnBattery;
}

AutoPtr< HashMap<Int32, AutoPtr<BatteryStatsImpl::BatteryStatsUid> > > BatteryStatsImpl::GetUidStats()
{
    HashMap<Int32, AutoPtr<Uid> >::Iterator it = mUidStats.Begin();
    AutoPtr< HashMap<Int32, AutoPtr<BatteryStatsUid> > > temp = new HashMap<Int32, AutoPtr<BatteryStatsUid> >();
    for (; it != mUidStats.End(); ++it) {
        (*temp)[it->mFirst] = (BatteryStatsUid*)it->mSecond;
    }
    return temp;
}

void BatteryStatsImpl::SetCallback(
    /* [in] */ BatteryCallback* cb)
{
    mCallback = cb;
}

void BatteryStatsImpl::SetNumSpeedSteps(
    /* [in] */ Int32 steps)
{
    if (sNumSpeedSteps == 0) sNumSpeedSteps = steps;
}

void BatteryStatsImpl::SetRadioScanningTimeout(
    /* [in] */ Int64 timeout)
{
    if (mPhoneSignalScanningTimer != NULL) {
        mPhoneSignalScanningTimer->SetTimeout(timeout);
    }
}

Boolean BatteryStatsImpl::StartIteratingOldHistoryLocked()
{
    if (DEBUG_HISTORY) {
        Int32 size, pos;
        mHistoryBuffer->GetElementSize(&size);
        mHistoryBuffer->GetDataPosition(&pos);
        Logger::I(TAG, "ITERATING: buff size=%d pos=%d", size, pos);
    }
    mHistoryBuffer->SetDataPosition(0);
    mHistoryReadTmp.Clear();
    mReadOverflow = FALSE;
    mIteratingHistory = TRUE;
    return (mHistoryIterator = mHistory) != NULL;
}

Boolean BatteryStatsImpl::GetNextOldHistoryLocked(
    /* [in] */ HistoryItem* out)
{
    Int32 pos, size;
    mHistoryBuffer->GetDataPosition(&pos);
    mHistoryBuffer->GetElementSize(&size);
    Boolean end = pos >= size;
    if (!end) {
        mHistoryReadTmp->ReadDelta(mHistoryBuffer);
        mReadOverflow |= mHistoryReadTmp->mCmd == HistoryItem::CMD_OVERFLOW;
    }
    AutoPtr<HistoryItem> cur = mHistoryIterator;
    if (cur == NULL) {
        if (!mReadOverflow && !end) {
            Logger::W(TAG, "Old history ends before new history!");
        }
        return FALSE;
    }
    out->SetTo(cur);
    mHistoryIterator = cur->mNext;
    if (!mReadOverflow) {
        if (end) {
            Logger::W(TAG, "New history ends before old history!");
        }
        else if (!out->Same(mHistoryReadTmp)) {
            Int64 now = GetHistoryBaseTime() + SystemClock::GetElapsedRealtime();
            AutoPtr<IStringWriter> sw;
            CStringWriter::New((IStringWriter**)&sw);
            AutoPtr<IPrintWriter> pw;
            CPrintWriter::New(sw, (IPrintWriter**)&pw);
            pw->PrintString(String("Histories differ!"));
            pw->PrintString(String("Old history:"));
            AutoPtr<HistoryPrinter> hp = new HistoryPrinter();
            hp->PrintNextItem(pw, out, now);
            pw->PrintString(String("New history:"));
            AutoPtr<HistoryPrinter> hp1 = new HistoryPrinter();
            hp1->PrintNextItem(pw, mHistoryReadTmp, now);
        }
    }
    return TRUE;
}

void BatteryStatsImpl::FinishIteratingOldHistoryLocked()
{
    mIteratingHistory = FALSE;
    Int32 size;
    mHistoryBuffer->GetElementSize(&size);
    mHistoryBuffer->SetDataPosition(size);
}

Boolean BatteryStatsImpl::StartIteratingHistoryLocked()
{
    Int32 size;
    mHistoryBuffer->GetElementSize(&size);
    if (DEBUG_HISTORY) {
        Int32 pos;
        mHistoryBuffer->GetDataPosition(&pos);
        Logger::I(TAG, "ITERATING: buff size=%d pos=%d", size, pos);
    }
    mHistoryBuffer->SetDataPosition(0);
    mReadOverflow = FALSE;
    mIteratingHistory = TRUE;
    return size > 0;
}

Boolean BatteryStatsImpl::GetNextHistoryLocked(
    /* [in] */ HistoryItem* out)
{
    Int32 pos;
    mHistoryBuffer->GetDataPosition(&pos);
    if (pos == 0) {
        out->Clear();
    }
    Int32 size;
    mHistoryBuffer->GetElementSize(&size);
    Boolean end = pos >= size;
    if (end) {
        return FALSE;
    }

    out->ReadDelta(mHistoryBuffer);
    return TRUE;
}

void BatteryStatsImpl::FinishIteratingHistoryLocked()
{
    mIteratingHistory = FALSE;
    Int32 size;
    mHistoryBuffer->GetElementSize(&size);
    mHistoryBuffer->SetDataPosition(size);
}

Int64 BatteryStatsImpl::GetHistoryBaseTime()
{
    return mHistoryBaseTime;
}

Int32 BatteryStatsImpl::GetStartCount()
{
    return mStartCount;
}

Boolean BatteryStatsImpl::IsOnBattery()
{
    return mOnBattery;
}

Boolean BatteryStatsImpl::IsScreenOn()
{
    return mScreenOn;
}

void BatteryStatsImpl::InitTimes()
{
    mBatteryRealtime = mTrackBatteryPastUptime = 0;
    mBatteryUptime = mTrackBatteryPastRealtime = 0;
    mUptimeStart = mTrackBatteryUptimeStart = SystemClock::GetUptimeMillis() * 1000;
    mRealtimeStart = mTrackBatteryRealtimeStart = SystemClock::GetElapsedRealtime() * 1000;
    mUnpluggedBatteryUptime = GetBatteryUptimeLocked(mUptimeStart);
    mUnpluggedBatteryRealtime = GetBatteryRealtimeLocked(mRealtimeStart);
}

void BatteryStatsImpl::InitDischarge()
{
    mLowDischargeAmountSinceCharge = 0;
    mHighDischargeAmountSinceCharge = 0;
    mDischargeAmountScreenOn = 0;
    mDischargeAmountScreenOnSinceCharge = 0;
    mDischargeAmountScreenOff = 0;
    mDischargeAmountScreenOffSinceCharge = 0;
}

void BatteryStatsImpl::ResetAllStatsLocked()
{
    mStartCount = 0;
    InitTimes();
    mScreenOnTimer->Reset(this, FALSE);
    for (Int32 i = 0; i < NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        (*mScreenBrightnessTimer)[i]->Reset(this, FALSE);
    }
    mInputEventCounter->Reset(FALSE);
    mPhoneOnTimer->Reset(this, FALSE);
    mAudioOnTimer->Reset(this, FALSE);
    mVideoOnTimer->Reset(this, FALSE);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        (*mPhoneSignalStrengthsTimer)[i]->Reset(this, FALSE);
    }
    mPhoneSignalScanningTimer->Reset(this, FALSE);
    for (Int32 i = 0; i < NUM_DATA_CONNECTION_TYPES; i++) {
        (*mPhoneDataConnectionsTimer)[i]->Reset(this, FALSE);
    }
    mWifiOnTimer->Reset(this, FALSE);
    mGlobalWifiRunningTimer->Reset(this, FALSE);
    mBluetoothOnTimer->Reset(this, FALSE);

    HashMap<Int32, AutoPtr<Uid> >::Iterator it = mUidStats.Begin();
    for (; it != mUidStats.End(); ++it) {
        if (it->mSecond->Reset()) {
            mUidStats.Erase(it++);
        }
        else ++it;
    }

    if (mKernelWakelockStats.Begin() != mKernelWakelockStats.End()) {
        HashMap<String, AutoPtr<SamplingTimer> >::Iterator timerIt = mKernelWakelockStats.Begin();
        for (; timerIt != mKernelWakelockStats.End(); ++timerIt) {
            AutoPtr<SamplingTimer> timer = timerIt->mSecond;
            List<AutoPtr<IUnpluggable> >::Iterator unpluggableIt = mUnpluggables.Begin();
            for (; unpluggableIt != mUnpluggables.End(); ++it) {
                if ((*unpluggableIt).Get() == (IUnpluggable*)timer) {
                    mUnpluggables.Erase(unpluggableIt);
                    break;
                }
            }
        }
        mKernelWakelockStats.Clear();
    }

    InitDischarge();

    ClearHistoryLocked();
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

void BatteryStatsImpl::SetOnBattery(
    /* [in] */ Boolean onBattery,
    /* [in] */ Int32 oldStatus,
    /* [in] */ Int32 level)
{
    AutoLock lock(mLock);
    SetOnBatteryLocked(onBattery, oldStatus, level);
}

void BatteryStatsImpl::SetOnBatteryLocked(
    /* [in] */ Boolean onBattery,
    /* [in] */ Int32 oldStatus,
    /* [in] */ Int32 level)
{
    Boolean doWrite = FALSE;

    AutoPtr<IMessage> msg;
    mHandler->ObtainMessage(MSG_REPORT_POWER_CHANGE,
        onBattery ? 1 : 0, 0, (IMessage**)&msg);
    Boolean result;
    mHandler->SendMessage(msg, &result);
    mOnBattery = mOnBatteryInternal = onBattery;

    Int64 uptime = SystemClock::GetUptimeMillis() * 1000;
    Int64 mSecRealtime = SystemClock::GetElapsedRealtime();
    Int64 realtime = mSecRealtime * 1000;
    if (onBattery) {
        // We will reset our status if we are unplugging after the
        // battery was last full, or the level is at 100, or
        // we have gone through a significant charge (from a very low
        // level to a now very high level).
        if (oldStatus == IBatteryManager::BATTERY_STATUS_FULL
                || level >= 90
                || (mDischargeCurrentLevel < 20 && level >= 80)) {
            doWrite = TRUE;
            ResetAllStatsLocked();
            mDischargeStartLevel = level;
        }
        UpdateKernelWakelocksLocked();
        mHistoryCur->mBatteryLevel = (Byte)level;
        mHistoryCur->mStates &= ~HistoryItem::STATE_BATTERY_PLUGGED_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Battery unplugged to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(mSecRealtime);
        mTrackBatteryUptimeStart = uptime;
        mTrackBatteryRealtimeStart = realtime;
        mUnpluggedBatteryUptime = GetBatteryUptimeLocked(uptime);
        mUnpluggedBatteryRealtime = GetBatteryRealtimeLocked(realtime);
        mDischargeCurrentLevel = mDischargeUnplugLevel = level;
        if (mScreenOn) {
            mDischargeScreenOnUnplugLevel = level;
            mDischargeScreenOffUnplugLevel = 0;
        }
        else {
            mDischargeScreenOnUnplugLevel = 0;
            mDischargeScreenOffUnplugLevel = level;
        }
        mDischargeAmountScreenOn = 0;
        mDischargeAmountScreenOff = 0;
        DoUnplugLocked(mUnpluggedBatteryUptime, mUnpluggedBatteryRealtime);
    }
    else {
        UpdateKernelWakelocksLocked();
        mHistoryCur->mBatteryLevel = (Byte)level;
        mHistoryCur->mStates |= HistoryItem::STATE_BATTERY_PLUGGED_FLAG;
        if (DEBUG_HISTORY)
            Logger::V(TAG, "Battery plugged to: %s", StringUtils::Int32ToHexString(mHistoryCur->mStates).string());
        AddHistoryRecordLocked(mSecRealtime);
        mTrackBatteryPastUptime += uptime - mTrackBatteryUptimeStart;
        mTrackBatteryPastRealtime += realtime - mTrackBatteryRealtimeStart;
        mDischargeCurrentLevel = level;
        if (level < mDischargeUnplugLevel) {
            mLowDischargeAmountSinceCharge += mDischargeUnplugLevel - level - 1;
            mHighDischargeAmountSinceCharge += mDischargeUnplugLevel - level;
        }
        UpdateDischargeScreenLevelsLocked(mScreenOn, mScreenOn);
        DoPlugLocked(GetBatteryUptimeLocked(uptime), GetBatteryRealtimeLocked(realtime));
    }
    if (doWrite || (mLastWriteTime + (60 * 1000)) < mSecRealtime) {
        if (mFile != NULL) {
            WriteAsyncLocked();
        }
    }
}

void BatteryStatsImpl::SetBatteryState(
    /* [in] */ Int32 status,
    /* [in] */ Int32 health,
    /* [in] */ Int32 plugType,
    /* [in] */ Int32 level,
    /* [in] */ Int32 temp,
    /* [in] */ Int32 volt)
{
    AutoLock lock(mLock);
    Boolean onBattery = plugType == BATTERY_PLUGGED_NONE;
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
        mRecordingHistory = TRUE;
    }
    if (onBattery != mOnBattery) {
        mHistoryCur->mBatteryLevel = (Byte)level;
        mHistoryCur->mBatteryStatus = (Byte)status;
        mHistoryCur->mBatteryHealth = (Byte)health;
        mHistoryCur->mBatteryPlugType = (Byte)plugType;
        mHistoryCur->mBatteryTemperature = (Char32)temp;
        mHistoryCur->mBatteryVoltage = (Char32)volt;
        SetOnBatteryLocked(onBattery, oldStatus, level);
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
            mHistoryCur->mBatteryTemperature = (Char32)temp;
            changed = TRUE;
        }
        if (volt > (mHistoryCur->mBatteryVoltage + 20)
                || volt < (mHistoryCur->mBatteryVoltage - 20)) {
            mHistoryCur->mBatteryVoltage = (Char32)volt;
            changed = TRUE;
        }
        if (changed) {
            AddHistoryRecordLocked(SystemClock::GetElapsedRealtime());
        }
    }
    if (!onBattery && status == IBatteryManager::BATTERY_STATUS_FULL) {
        // We don't record history while we are plugged in and fully charged.
        // The next time we are unplugged, history will be cleared.
        mRecordingHistory = FALSE;
    }
}

void BatteryStatsImpl::UpdateKernelWakelocksLocked()
{
    AutoPtr< HashMap<String, AutoPtr<KernelWakelockStats> > > m = ReadKernelWakelockStats();

    if (m == NULL) {
        // Not crashing might make board bringup easier.
        Logger::W(TAG, "Couldn't get kernel wake lock stats");
        return;
    }

    HashMap<String, AutoPtr<KernelWakelockStats> >::Iterator it = m->Begin();
    for (; it != m->End(); ++it) {
        String name = it->mFirst;
        AutoPtr<KernelWakelockStats> kws = it->mSecond;

        AutoPtr<SamplingTimer> kwlt;
        HashMap<String, AutoPtr<SamplingTimer> >::Iterator timerIt = mKernelWakelockStats.Find(name);
        if (timerIt != mKernelWakelockStats.End()) {
            kwlt = timerIt->mSecond;
        }
        if (kwlt == NULL) {
            kwlt = new SamplingTimer(&mUnpluggables, mOnBatteryInternal,
                    TRUE /* track reported values */);
            mKernelWakelockStats[name] = kwlt;
        }
        kwlt->UpdateCurrentReportedCount(kws->mCount);
        kwlt->UpdateCurrentReportedTotalTime(kws->mTotalTime);
        kwlt->SetUpdateVersion(sKernelWakelockUpdateVersion);
    }

    if (m->GetSize() != mKernelWakelockStats.GetSize()) {
        // Set timers to stale if they didn't appear in /proc/wakelocks this time.
        HashMap<String, AutoPtr<SamplingTimer> >::Iterator timerIt = mKernelWakelockStats.Begin();
        for (; timerIt!= mKernelWakelockStats.End(); ++timerIt) {
            AutoPtr<SamplingTimer> st = timerIt->mSecond;
            if (st->GetUpdateVersion() != sKernelWakelockUpdateVersion) {
                st->SetStale();
            }
        }
    }
}

Int64 BatteryStatsImpl::GetAwakeTimeBattery()
{
    return ComputeBatteryUptime(GetBatteryUptimeLocked(), STATS_CURRENT);
}

Int64 BatteryStatsImpl::GetAwakeTimePlugged()
{
    return (SystemClock::GetUptimeMillis() * 1000) - GetAwakeTimeBattery();
}

Int64 BatteryStatsImpl::ComputeUptime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which)
{
    switch (which) {
        case STATS_SINCE_CHARGED: return mUptime + (curTime-mUptimeStart);
        case STATS_LAST: return mLastUptime;
        case STATS_CURRENT: return (curTime - mUptimeStart);
        case STATS_SINCE_UNPLUGGED: return (curTime - mTrackBatteryUptimeStart);
    }
    return 0;
}

Int64 BatteryStatsImpl::ComputeRealtime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which)
{
    switch (which) {
        case STATS_SINCE_CHARGED: return mRealtime + (curTime-mRealtimeStart);
        case STATS_LAST: return mLastRealtime;
        case STATS_CURRENT: return (curTime - mRealtimeStart);
        case STATS_SINCE_UNPLUGGED: return (curTime - mTrackBatteryRealtimeStart);
    }
    return 0;
}

Int64 BatteryStatsImpl::ComputeBatteryUptime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which)
{
    switch (which) {
        case STATS_SINCE_CHARGED:
            return mBatteryUptime + GetBatteryUptime(curTime);
        case STATS_LAST:
            return mBatteryLastUptime;
        case STATS_CURRENT:
            return GetBatteryUptime(curTime);
        case STATS_SINCE_UNPLUGGED:
            return GetBatteryUptimeLocked(curTime) - mUnpluggedBatteryUptime;
    }
    return 0;
}

Int64 BatteryStatsImpl::ComputeBatteryRealtime(
    /* [in] */ Int64 curTime,
    /* [in] */ Int32 which)
{
    switch (which) {
        case STATS_SINCE_CHARGED:
            return mBatteryRealtime + GetBatteryRealtimeLocked(curTime);
        case STATS_LAST:
            return mBatteryLastRealtime;
        case STATS_CURRENT:
            return GetBatteryRealtimeLocked(curTime);
        case STATS_SINCE_UNPLUGGED:
            return GetBatteryRealtimeLocked(curTime) - mUnpluggedBatteryRealtime;
    }
    return 0;
}

Int64 BatteryStatsImpl::GetBatteryUptimeLocked(
        /* [in] */ Int64 curTime)
{
    Int64 time = mTrackBatteryPastUptime;
    if (mOnBatteryInternal) {
        time += curTime - mTrackBatteryUptimeStart;
    }
    return time;
}

Int64 BatteryStatsImpl::GetBatteryUptimeLocked()
{
    return GetBatteryUptime(SystemClock::GetUptimeMillis() * 1000);
}

Int64 BatteryStatsImpl::GetBatteryUptime(
    /* [in] */ Int64 curTime)
{
    Int64 time = mTrackBatteryPastUptime;
    if (mOnBatteryInternal) {
        time += curTime - mTrackBatteryUptimeStart;
    }
    return time;
}

Int64 BatteryStatsImpl::GetBatteryRealtimeLocked(
        /* [in] */ Int64 curTime)
{
    Int64 time = mTrackBatteryPastRealtime;
    if (mOnBatteryInternal) {
        time += curTime - mTrackBatteryRealtimeStart;
    }
    return time;
}

Int64 BatteryStatsImpl::GetBatteryRealtime(
        /* [in] */ Int64 curTime)
{
    return GetBatteryRealtimeLocked(curTime);
}

Int64 BatteryStatsImpl::GetTcpBytes(
    /* [in] */ Int64 current,
    /* [in] */ ArrayOf<Int64>* dataBytes,
    /* [in] */ Int32 which)
{
    if (which == STATS_LAST) {
        return (*dataBytes)[STATS_LAST];
    }
    else {
        if (which == STATS_SINCE_UNPLUGGED) {
            if ((*dataBytes)[STATS_SINCE_UNPLUGGED] < 0) {
                return (*dataBytes)[STATS_LAST];
            }
            else {
                return current - (*dataBytes)[STATS_SINCE_UNPLUGGED];
            }
        }
        else if (which == STATS_SINCE_CHARGED) {
            return (current - (*dataBytes)[STATS_CURRENT]) + (*dataBytes)[STATS_SINCE_CHARGED];
        }
        return current - (*dataBytes)[STATS_CURRENT];
    }
}

Int64 BatteryStatsImpl::GetMobileTcpBytesSent(
        /* [in] */ Int32 which)
{
    AutoPtr<INetworkStats> ns = GetNetworkStatsSummary();
    AutoPtr<INetworkStatsEntry> entry;
    AutoPtr<IObjectContainer> container;
    CObjectContainer::New((IObjectContainer**)&container);
    HashSet<String>::Iterator it = mMobileIfaces.Begin();
    for (; it != mMobileIfaces.End(); ++it) {
        AutoPtr<ICharSequence> cs;
        CString::New(*it, (ICharSequence**)&cs);
        container->Add(cs);
    }
    ns->GetTotal(NULL, container, (INetworkStatsEntry**)&entry);
    Int64 mobileTxBytes;
    entry->GetTxBytes(&mobileTxBytes);
    return GetTcpBytes(mobileTxBytes, mMobileDataTx, which);
}

Int64 BatteryStatsImpl::GetMobileTcpBytesReceived(
    /* [in] */ Int32 which)
{
    AutoPtr<INetworkStats> ns = GetNetworkStatsSummary();
    AutoPtr<INetworkStatsEntry> entry;
    AutoPtr<IObjectContainer> container;
    CObjectContainer::New((IObjectContainer**)&container);
    HashSet<String>::Iterator it = mMobileIfaces.Begin();
    for (; it != mMobileIfaces.End(); ++it) {
        AutoPtr<ICharSequence> cs;
        CString::New(*it, (ICharSequence**)&cs);
        container->Add(cs);
    }
    ns->GetTotal(NULL, container, (INetworkStatsEntry**)&entry);
    Int64 mobileRxBytes;
    entry->GetRxBytes(&mobileRxBytes);
    return GetTcpBytes(mobileRxBytes, mMobileDataRx, which);
}

Int64 BatteryStatsImpl::GetTotalTcpBytesSent(
    /* [in] */ Int32 which)
{
    AutoPtr<INetworkStats> ns = GetNetworkStatsSummary();
    AutoPtr<INetworkStatsEntry> entry;
    ns->GetTotal(NULL, (INetworkStatsEntry**)&entry);
    Int64 totalTxBytes;
    entry->GetTxBytes(&totalTxBytes);
    return GetTcpBytes(totalTxBytes, mTotalDataTx, which);
}

Int64 BatteryStatsImpl::GetTotalTcpBytesReceived(
    /* [in] */ Int32 which)
{
    AutoPtr<INetworkStats> ns = GetNetworkStatsSummary();
    AutoPtr<INetworkStatsEntry> entry;
    ns->GetTotal(NULL, (INetworkStatsEntry**)&entry);
    Int64 totalRxBytes;
    entry->GetRxBytes(&totalRxBytes);
    return GetTcpBytes(totalRxBytes, mTotalDataRx, which);
}

Int32 BatteryStatsImpl::GetDischargeStartLevel()
{
    AutoLock lock(mLock);
    return GetDischargeStartLevelLocked();
}

Int32 BatteryStatsImpl::GetDischargeStartLevelLocked()
{
    return mDischargeUnplugLevel;
}

Int32 BatteryStatsImpl::GetDischargeCurrentLevel()
{
    AutoLock lock(mLock);
    return GetDischargeCurrentLevelLocked();
}

Int32 BatteryStatsImpl::GetDischargeCurrentLevelLocked()
{
    return mDischargeCurrentLevel;
}

Int32 BatteryStatsImpl::GetLowDischargeAmountSinceCharge()
{
    AutoLock lock(mLock);
    Int32 val = mLowDischargeAmountSinceCharge;
    if (mOnBattery && mDischargeCurrentLevel < mDischargeUnplugLevel) {
        val += mDischargeUnplugLevel - mDischargeCurrentLevel-1;
    }
    return val;
}

Int32 BatteryStatsImpl::GetHighDischargeAmountSinceCharge()
{
    AutoLock lock(mLock);
    Int32 val = mHighDischargeAmountSinceCharge;
    if (mOnBattery && mDischargeCurrentLevel < mDischargeUnplugLevel) {
        val += mDischargeUnplugLevel - mDischargeCurrentLevel;
    }
    return val;
}

Int32 BatteryStatsImpl::GetDischargeAmountScreenOn()
{
    AutoLock lock(mLock);
    int val = mDischargeAmountScreenOn;
    if (mOnBattery && mScreenOn
            && mDischargeCurrentLevel < mDischargeScreenOnUnplugLevel) {
        val += mDischargeScreenOnUnplugLevel-mDischargeCurrentLevel;
    }
    return val;
}

Int32 BatteryStatsImpl::GetDischargeAmountScreenOnSinceCharge()
{
    AutoLock lock(mLock);
    Int32 val = mDischargeAmountScreenOnSinceCharge;
    if (mOnBattery && mScreenOn
            && mDischargeCurrentLevel < mDischargeScreenOnUnplugLevel) {
        val += mDischargeScreenOnUnplugLevel - mDischargeCurrentLevel;
    }
    return val;
}

Int32 BatteryStatsImpl::GetDischargeAmountScreenOff()
{
    AutoLock lock(mLock);
    Int32 val = mDischargeAmountScreenOff;
    if (mOnBattery && !mScreenOn
            && mDischargeCurrentLevel < mDischargeScreenOffUnplugLevel) {
        val += mDischargeScreenOffUnplugLevel - mDischargeCurrentLevel;
    }
    return val;
}

Int32 BatteryStatsImpl::GetDischargeAmountScreenOffSinceCharge()
{
    AutoLock lock(mLock);
    Int32 val = mDischargeAmountScreenOffSinceCharge;
    if (mOnBattery && !mScreenOn
            && mDischargeCurrentLevel < mDischargeScreenOffUnplugLevel) {
        val += mDischargeScreenOffUnplugLevel - mDischargeCurrentLevel;
    }
    return val;
}

Int32 BatteryStatsImpl::GetCpuSpeedSteps()
{
    return sNumSpeedSteps;
}

AutoPtr<BatteryStatsImpl::Uid> BatteryStatsImpl::GetUidStatsLocked(
    /* [in] */ Int32 uid)
{
    AutoPtr<Uid> u;
    HashMap<Int32, AutoPtr<Uid> >::Iterator it = mUidStats.Find(uid);
    if (it != mUidStats.End()) {
        u = it->mSecond;
    }
    if (u == NULL) {
        u = new Uid(uid, this);
        mUidStats[uid] = u;
    }
    return u;
}

void BatteryStatsImpl::RemoveUidStatsLocked(
    /* [in] */ Int32 uid)
{
    mUidStats.Erase(uid);
}

AutoPtr<BatteryStatsImpl::Uid::Proc> BatteryStatsImpl::GetProcessStatsLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& name)
{
    AutoPtr<Uid> u = GetUidStatsLocked(uid);
    return u->GetProcessStatsLocked(name);
}

AutoPtr<BatteryStatsImpl::Uid::Proc> BatteryStatsImpl::GetProcessStatsLocked(
    /* [in] */ const String& name,
    /* [in] */ Int32 pid)
{
    Int32 uid;
    HashMap<String, Int32>::Iterator it = mUidCache.Begin();
    for (; it != mUidCache.End(); ++it) {
        if (it->mFirst == name) {
            uid = it->mSecond;
            break;
        }
    }
    if (it == mUidCache.End()) {
        uid = Process::GetUidForPid(pid);
        mUidCache[name] = uid;
    }
    AutoPtr<Uid> u = GetUidStatsLocked(uid);
    return u->GetProcessStatsLocked(name);
}

AutoPtr<BatteryStatsImpl::Uid::Pkg> BatteryStatsImpl::GetPackageStatsLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& pkg)
{
    AutoPtr<Uid> u = GetUidStatsLocked(uid);
    return u->GetPackageStatsLocked(pkg);
}

AutoPtr<BatteryStatsImpl::Uid::Pkg::Serv> BatteryStatsImpl::GetServiceStatsLocked(
    /* [in] */ Int32 uid,
    /* [in] */ const String& pkg,
    /* [in] */ const String& name)
{
    AutoPtr<Uid> u = GetUidStatsLocked(uid);
    return u->GetServiceStatsLocked(pkg, name);
}

void BatteryStatsImpl::DistributeWorkLocked(
    /* [in] */ Int32 which)
{
    // Aggregate all CPU time associated with WIFI.
    AutoPtr<Uid> wifiUid;
    HashMap<Int32, AutoPtr<Uid> >::Iterator it = mUidStats.Find(IProcess::WIFI_UID);
    if (it != mUidStats.End()) {
        wifiUid = it->mSecond;
    }
    if (wifiUid != NULL) {
        Int64 uSecTime = ComputeBatteryRealtime(SystemClock::GetElapsedRealtime() * 1000, which);
        HashMap<String, AutoPtr<Uid::Proc> >::Iterator procIt = wifiUid->mProcessStats.Begin();
        for (; procIt != wifiUid->mProcessStats.End(); ++procIt) {
            AutoPtr<Uid::Proc> proc = procIt->mSecond;
            Int64 totalRunningTime = GetGlobalWifiRunningTime(uSecTime, which);
            for (Int32 i = 0; i < (Int32)mUidStats.GetSize(); i++) {
                AutoPtr<Uid> uid = mUidStats[i];
                if (uid->mUid != IProcess::WIFI_UID) {
                    Int64 uidRunningTime = uid->GetWifiRunningTime(uSecTime, which);
                    if (uidRunningTime > 0) {
                        AutoPtr<Uid::Proc> uidProc = uid->GetProcessStatsLocked(String("*wifi*"));
                        Int64 time = proc->GetUserTime(which);
                        time = (time * uidRunningTime) / totalRunningTime;
                        uidProc->mUserTime += time;
                        proc->mUserTime -= time;
                        time = proc->GetSystemTime(which);
                        time = (time * uidRunningTime) / totalRunningTime;
                        uidProc->mSystemTime += time;
                        proc->mSystemTime -= time;
                        time = proc->GetForegroundTime(which);
                        time = (time * uidRunningTime) / totalRunningTime;
                        uidProc->mForegroundTime += time;
                        proc->mForegroundTime -= time;
                        for (Int32 sb=0; sb < proc->mSpeedBins->GetLength(); sb++) {
                            AutoPtr<SamplingCounter> sc = (*proc->mSpeedBins)[sb];
                            if (sc != NULL) {
                                time = sc->GetCountLocked(which);
                                time = (time * uidRunningTime) / totalRunningTime;
                                AutoPtr<SamplingCounter> uidSc = (*uidProc->mSpeedBins)[sb];
                                if (uidSc == NULL) {
                                    uidSc = new SamplingCounter(&mUnpluggables);
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

void BatteryStatsImpl::ShutdownLocked()
{
    WriteSyncLocked();
    mShuttingDown = TRUE;
}

void BatteryStatsImpl::WriteAsyncLocked()
{
    WriteLocked(FALSE);
}

void BatteryStatsImpl::WriteSyncLocked()
{
    WriteLocked(TRUE);
}

CAR_INTERFACE_IMPL(BatteryStatsImpl::BatteryStatsWriteRunnable, IRunnable)

ECode BatteryStatsImpl::BatteryStatsWriteRunnable::Run()
{
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    mHost->CommitPendingDataToDisk();
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
    WriteSummaryToParcel(out);
    mLastWriteTime = SystemClock::GetElapsedRealtime();

    // if (mPendingWrite != NULL) {
    //     mPendingWrite->Recycle();
    // }
    mPendingWrite = out;

    if (sync) {
        CommitPendingDataToDisk();
    }
    else {
        AutoPtr<IRunnable> run = (IRunnable*)new BatteryStatsWriteRunnable(this);
        AutoPtr<IThread> thr;
        CThread::New(run, String("BatteryStats-Write"), (IThread**)&thr);
        thr->Start();
    }
}

void BatteryStatsImpl::CommitPendingDataToDisk()
{
    AutoPtr<IParcel> next;
    {
        AutoLock lock(mLock);
        next = mPendingWrite;
        mPendingWrite = NULL;
        if (next == NULL) {
            return;
        }

        mWriteLock->Lock();
    }
    AutoPtr<IFileOutputStream> stream;
    // try {
    mFile->StartWrite((IFileOutputStream**)&stream);
    AutoPtr< ArrayOf<Byte> > bytes;
    next->Marshall((ArrayOf<Byte>**)&bytes);
    stream->WriteBytes(*bytes);
    IFlushable::Probe(stream)->Flush();
    Boolean result;
    FileUtils::Sync(stream, &result);
    stream->Close();
    mFile->FinishWrite(stream);
    // } catch (IOException e) {
    //     Logger::W("BatteryStats", "Error writing battery statistics", e);
    //     mFile.failWrite(stream);
    // } finally {
    //     next.recycle();
    //     mWriteLock.unlock();
    // }
    // next.recycle();
    mWriteLock->UnLock();
}

ECode BatteryStatsImpl::ReadFully(
    /* [in] */ IFileInputStream* stream,
    /* [out] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)

    Int32 pos = 0;
    Int32 avail;
    stream->Available(&avail);
    AutoPtr< ArrayOf<Byte> > data = ArrayOf<Byte>::Alloc(avail);
    while (TRUE) {
        Int32 amt;
        stream->ReadBytes(data, pos, data->GetLength() - pos, &amt);
        //Log.i("foo", "Read " + amt + " bytes at " + pos
        //        + " of avail " + data.length);
        if (amt <= 0) {
            //Log.i("foo", "**** FINISHED READING: pos=" + pos
            //        + " len=" + data.length);
            *bytes = data;
            REFCOUNT_ADD(*bytes);
            return NOERROR;
        }
        pos += amt;
        stream->Available(&avail);
        if (avail > data->GetLength() - pos) {
            AutoPtr< ArrayOf<Byte> > newData = ArrayOf<Byte>::Alloc(pos + avail);
            newData->Copy(data->GetPayload(), pos);
            data = newData;
        }
    }
}

void BatteryStatsImpl::ReadLocked()
{
    if (mFile == NULL) {
        Logger::W("BatteryStats", "readLocked: no file associated with this instance");
        return;
    }

    mUidStats.Clear();

    // try {
    AutoPtr<IFileInputStream> stream;
    mFile->OpenRead((IFileInputStream**)&stream);

    AutoPtr< ArrayOf<Byte> > raw;
    ReadFully(stream, (ArrayOf<Byte>**)&raw);
    AutoPtr<IParcel> in;
    CParcel::New((IParcel**)&in);
    in->Unmarshall(raw, 0, raw->GetLength());
    in->SetDataPosition(0);
    stream->Close();

    ReadSummaryFromParcel(in);
    // } catch(java.io.IOException e) {
    //     Slog.e("BatteryStats", "Error reading battery statistics", e);
    // }

    Int64 now = SystemClock::GetElapsedRealtime();
    if (USE_OLD_HISTORY) {
        AddHistoryRecordLocked(now, HistoryItem::CMD_START);
    }
    AddHistoryBufferLocked(now, HistoryItem::CMD_START);
}

void BatteryStatsImpl::ReadHistory(
    /* [in] */ IParcel* in,
    /* [in] */ Boolean andOldHistory)
{
    Int64 historyBaseTime;
    in->ReadInt64(&historyBaseTime);

    // mHistoryBuffer.setDataSize(0);
    mHistoryBuffer->SetDataPosition(0);

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
        mHistoryBaseTime = (mHistoryBaseTime - SystemClock::GetElapsedRealtime()) + 60 * 1000;
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
    /* [in] */ Boolean andOldHistory)
{
    if (DEBUG_HISTORY) {
        StringBuilder sb(128);
        sb += "****************** WRITING mHistoryBaseTime: ";
        TimeUtils::FormatDuration(mHistoryBaseTime, sb);
        sb += " mLastHistoryTime: ";
        TimeUtils::FormatDuration(mLastHistoryTime, sb);
        Logger::I(TAG, sb.ToString());
    }
    out->WriteInt64(mHistoryBaseTime + mLastHistoryTime);
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

void BatteryStatsImpl::ReadSummaryFromParcel(
    /* [in] */ IParcel* in)
{
    Int32 version;
    in->ReadInt32(&version);
    if (version != VERSION) {
        Logger::W("BatteryStats", "readFromParcel: version got %d, expected %d; erasing old stats", version, VERSION);
        return;
    }

    ReadHistory(in, TRUE);

    in->ReadInt32(&mStartCount);
    in->ReadInt64(&mBatteryUptime);
    in->ReadInt64(&mBatteryRealtime);
    in->ReadInt64(&mUptime);
    in->ReadInt64(&mRealtime);
    in->ReadInt32(&mDischargeUnplugLevel);
    in->ReadInt32(&mDischargeCurrentLevel);
    in->ReadInt32(&mLowDischargeAmountSinceCharge);
    in->ReadInt32(&mHighDischargeAmountSinceCharge);
    in->ReadInt32(&mDischargeAmountScreenOnSinceCharge);
    in->ReadInt32(&mDischargeAmountScreenOffSinceCharge);

    mStartCount++;

    mScreenOn = FALSE;
    mScreenOnTimer->ReadSummaryFromParcelLocked(in);
    for (Int32 i = 0; i < NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        (*mScreenBrightnessTimer)[i]->ReadSummaryFromParcelLocked(in);
    }
    mInputEventCounter->ReadSummaryFromParcelLocked(in);
    mPhoneOn = FALSE;
    mPhoneOnTimer->ReadSummaryFromParcelLocked(in);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        (*mPhoneSignalStrengthsTimer)[i]->ReadSummaryFromParcelLocked(in);
    }
    mPhoneSignalScanningTimer->ReadSummaryFromParcelLocked(in);
    for (Int32 i = 0; i < NUM_DATA_CONNECTION_TYPES; i++) {
        (*mPhoneDataConnectionsTimer)[i]->ReadSummaryFromParcelLocked(in);
    }
    mWifiOn = FALSE;
    mWifiOnTimer->ReadSummaryFromParcelLocked(in);
    mGlobalWifiRunning = FALSE;
    mGlobalWifiRunningTimer->ReadSummaryFromParcelLocked(in);
    mBluetoothOn = FALSE;
    mBluetoothOnTimer->ReadSummaryFromParcelLocked(in);

    Int32 NKW;
    in->ReadInt32(&NKW);
    if (NKW > 10000) {
        Logger::W(TAG, "File corrupt: too many kernel wake locks %d", NKW);
        return;
    }
    for (Int32 ikw = 0; ikw < NKW; ikw++) {
        Int32 value;
        if (in->ReadInt32(&value), value != 0) {
            String kwltName;
            in->ReadString(&kwltName);
            GetKernelWakelockTimerLocked(kwltName)->ReadSummaryFromParcelLocked(in);
        }
    }

    in->ReadInt32(&sNumSpeedSteps);

    Int32 NU;
    in->ReadInt32(&NU);
    if (NU > 10000) {
        Logger::W(TAG, "File corrupt: too many uids %d", NU);
        return;
    }
    for (Int32 iu = 0; iu < NU; iu++) {
        Int32 uid;
        in->ReadInt32(&uid);
        AutoPtr<Uid> u = new Uid(uid, this);
        mUidStats[uid] = u;

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
        u->mWifiMulticastEnabled = FALSE;
        if (in->ReadInt32(&value), value != 0) {
            u->mWifiMulticastTimer->ReadSummaryFromParcelLocked(in);
        }
        u->mAudioTurnedOn = FALSE;
        if (in->ReadInt32(&value), value != 0) {
            u->mAudioTurnedOnTimer->ReadSummaryFromParcelLocked(in);
        }
        u->mVideoTurnedOn = FALSE;
        if (in->ReadInt32(&value), value != 0) {
            u->mVideoTurnedOnTimer->ReadSummaryFromParcelLocked(in);
        }

        if (in->ReadInt32(&value), value != 0) {
            if (u->mUserActivityCounters == NULL) {
                u->InitUserActivityLocked();
            }
            for (Int32 i = 0; i < Uid::NUM_USER_ACTIVITY_TYPES; i++) {
                (*u->mUserActivityCounters)[i]->ReadSummaryFromParcelLocked(in);
            }
        }

        Int32 NW;
        in->ReadInt32(&NW);
        if (NW > 100) {
            Logger::W(TAG, "File corrupt: too many wake locks %d", NW);
            return;
        }
        for (Int32 iw = 0; iw < NW; iw++) {
            String wlName;
            in->ReadString(&wlName);
            if (in->ReadInt32(&value), value != 0) {
                AutoPtr<StopwatchTimer> timer;
                u->GetWakeTimerLocked(wlName, WAKE_TYPE_FULL, (StopwatchTimer**)&timer);
                timer->ReadSummaryFromParcelLocked(in);
            }
            if (in->ReadInt32(&value), value != 0) {
                AutoPtr<StopwatchTimer> timer;
                u->GetWakeTimerLocked(wlName, WAKE_TYPE_PARTIAL, (StopwatchTimer**)&timer);
                timer->ReadSummaryFromParcelLocked(in);
            }
            if (in->ReadInt32(&value), value != 0) {
                AutoPtr<StopwatchTimer> timer;
                u->GetWakeTimerLocked(wlName, WAKE_TYPE_WINDOW, (StopwatchTimer**)&timer);
                timer->ReadSummaryFromParcelLocked(in);
            }
        }

        Int32 NP;
        in->ReadInt32(&NP);
        if (NP > 1000) {
            Logger::W(TAG, "File corrupt: too many sensors %d", NP);
            return;
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
            Logger::W(TAG, "File corrupt: too many processes %d", NP);
            return;
        }
        for (Int32 ip = 0; ip < NP; ip++) {
            String procName;
            in->ReadString(&procName);
            AutoPtr<Uid::Proc> p = u->GetProcessStatsLocked(procName);
            Int64 userTime;
            in->ReadInt64(&userTime);
            p->mUserTime = p->mLoadedUserTime = userTime;
            Int64 systemTime;
            in->ReadInt64(&systemTime);
            p->mSystemTime = p->mLoadedSystemTime = systemTime;
            Int32 starts;
            in->ReadInt32(&starts);
            p->mStarts = p->mLoadedStarts = starts;
            Int32 NSB;
            in->ReadInt32(&NSB);
            if (NSB > 100) {
                Logger::W(TAG, "File corrupt: too many speed bins %d", NSB);
                return;
            }
            p->mSpeedBins = ArrayOf<SamplingCounter*>::Alloc(NSB);
            for (Int32 i = 0; i < NSB; i++) {
                if (in->ReadInt32(&value), value != 0) {
                    AutoPtr<SamplingCounter> counter = new SamplingCounter(&mUnpluggables);
                    p->mSpeedBins->Set(i, counter);
                    (*p->mSpeedBins)[i]->ReadSummaryFromParcelLocked(in);
                }
            }
            if (!p->ReadExcessivePowerFromParcelLocked(in)) {
                return;
            }
        }

        in->ReadInt32(&NP);
        if (NP > 10000) {
            Logger::W(TAG, "File corrupt: too many packages %d", NP);
            return;
        }
        for (Int32 ip = 0; ip < NP; ip++) {
            String pkgName;
            in->ReadString(&pkgName);
            AutoPtr<Uid::Pkg> p = u->GetPackageStatsLocked(pkgName);
            Int32 wakeups;
            in->ReadInt32(&wakeups);
            p->mWakeups = p->mLoadedWakeups = wakeups;
            Int32 NS;
            in->ReadInt32(&NS);
            if (NS > 1000) {
                Logger::W(TAG, "File corrupt: too many services %d", NS);
                return;
            }
            for (Int32 is = 0; is < NS; is++) {
                String servName;
                in->ReadString(&servName);
                AutoPtr<Uid::Pkg::Serv> s = u->GetServiceStatsLocked(pkgName, servName);
                Int64 startTime;
                in->ReadInt64(&startTime);
                s->mStartTime = s->mLoadedStartTime = startTime;
                Int32 starts;
                in->ReadInt32(&starts);
                s->mStarts = s->mLoadedStarts = starts;
                Int32 launches;
                in->ReadInt32(&launches);
                s->mLaunches = s->mLoadedLaunches = launches;
            }
        }

        in->ReadInt64(&u->mLoadedTcpBytesReceived);
        in->ReadInt64(&u->mLoadedTcpBytesSent);
    }
}

void BatteryStatsImpl::WriteSummaryToParcel(
    /* [in] */ IParcel* out)
{
    // Need to update with current kernel wake lock counts.
    UpdateKernelWakelocksLocked();

    Int64 NOW_SYS = SystemClock::GetUptimeMillis() * 1000;
    Int64 NOWREAL_SYS = SystemClock::GetElapsedRealtime() * 1000;
    Int64 NOW = GetBatteryUptimeLocked(NOW_SYS);
    Int64 NOWREAL = GetBatteryRealtimeLocked(NOWREAL_SYS);

    out->WriteInt32(VERSION);

    WriteHistory(out, TRUE);

    out->WriteInt32(mStartCount);
    out->WriteInt64(ComputeBatteryUptime(NOW_SYS, STATS_SINCE_CHARGED));
    out->WriteInt64(ComputeBatteryRealtime(NOWREAL_SYS, STATS_SINCE_CHARGED));
    out->WriteInt64(ComputeUptime(NOW_SYS, STATS_SINCE_CHARGED));
    out->WriteInt64(ComputeRealtime(NOWREAL_SYS, STATS_SINCE_CHARGED));
    out->WriteInt32(mDischargeUnplugLevel);
    out->WriteInt32(mDischargeCurrentLevel);
    out->WriteInt32(GetLowDischargeAmountSinceCharge());
    out->WriteInt32(GetHighDischargeAmountSinceCharge());
    out->WriteInt32(GetDischargeAmountScreenOnSinceCharge());
    out->WriteInt32(GetDischargeAmountScreenOffSinceCharge());

    mScreenOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
    for (Int32 i = 0; i < NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        (*mScreenBrightnessTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL);
    }
    mInputEventCounter->WriteSummaryFromParcelLocked(out);
    mPhoneOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        (*mPhoneSignalStrengthsTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL);
    }
    mPhoneSignalScanningTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
    for (Int32 i = 0; i < NUM_DATA_CONNECTION_TYPES; i++) {
        (*mPhoneDataConnectionsTimer)[i]->WriteSummaryFromParcelLocked(out, NOWREAL);
    }
    mWifiOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
    mGlobalWifiRunningTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
    mBluetoothOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL);

    out->WriteInt32(mKernelWakelockStats.GetSize());
    HashMap<String, AutoPtr<SamplingTimer> >::Iterator it = mKernelWakelockStats.Begin();
    for (; it != mKernelWakelockStats.End(); ++it) {
        AutoPtr<Timer> kwlt = (Timer*)it->mSecond;
        if (kwlt != NULL) {
            out->WriteInt32(1);
            out->WriteString(it->mFirst);
            it->mSecond->WriteSummaryFromParcelLocked(out, NOWREAL);
        }
        else {
            out->WriteInt32(0);
        }
    }

    out->WriteInt32(sNumSpeedSteps);
    Int32 NU = mUidStats.GetSize();
    out->WriteInt32(NU);
    HashMap<Int32, AutoPtr<Uid> >::Iterator uidIt = mUidStats.Begin();
    for (; uidIt != mUidStats.End(); ++uidIt) {
        out->WriteInt32(uidIt->mFirst);
        AutoPtr<Uid> u = uidIt->mSecond;

        if (u->mWifiRunningTimer != NULL) {
            out->WriteInt32(1);
            u->mWifiRunningTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
        }
        else {
            out->WriteInt32(0);
        }
        if (u->mFullWifiLockTimer != NULL) {
            out->WriteInt32(1);
            u->mFullWifiLockTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
        }
        else {
            out->WriteInt32(0);
        }
        if (u->mWifiScanTimer != NULL) {
            out->WriteInt32(1);
            u->mWifiScanTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
        }
        else {
            out->WriteInt32(0);
        }
        if (u->mWifiMulticastTimer != NULL) {
            out->WriteInt32(1);
            u->mWifiMulticastTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
        }
        else {
            out->WriteInt32(0);
        }
        if (u->mAudioTurnedOnTimer != NULL) {
            out->WriteInt32(1);
            u->mAudioTurnedOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
        }
        else {
            out->WriteInt32(0);
        }
        if (u->mVideoTurnedOnTimer != NULL) {
            out->WriteInt32(1);
            u->mVideoTurnedOnTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
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

        Int32 NW = u->mWakelockStats.GetSize();
        out->WriteInt32(NW);
        if (NW > 0) {
            HashMap<String, AutoPtr<Uid::Wakelock> >::Iterator wakelockIt = u->mWakelockStats.Begin();
            for (; wakelockIt != u->mWakelockStats.End(); ++wakelockIt) {
                out->WriteString(wakelockIt->mFirst);
                AutoPtr<Uid::Wakelock> wl = wakelockIt->mSecond;
                if (wl->mTimerFull != NULL) {
                    out->WriteInt32(1);
                    wl->mTimerFull->WriteSummaryFromParcelLocked(out, NOWREAL);
                }
                else {
                    out->WriteInt32(0);
                }
                if (wl->mTimerPartial != NULL) {
                    out->WriteInt32(1);
                    wl->mTimerPartial->WriteSummaryFromParcelLocked(out, NOWREAL);
                }
                else {
                    out->WriteInt32(0);
                }
                if (wl->mTimerWindow != NULL) {
                    out->WriteInt32(1);
                    wl->mTimerWindow->WriteSummaryFromParcelLocked(out, NOWREAL);
                }
                else {
                    out->WriteInt32(0);
                }
            }
        }

        Int32 NSE = u->mSensorStats.GetSize();
        out->WriteInt32(NSE);
        if (NSE > 0) {
            HashMap<Int32, AutoPtr<Uid::Sensor> >::Iterator sensorIt =u->mSensorStats.Begin();
            for (; sensorIt != u->mSensorStats.End(); ++sensorIt) {
                out->WriteInt32(sensorIt->mFirst);
                AutoPtr<Uid::Sensor> se = sensorIt->mSecond;
                if (se->mTimer != NULL) {
                    out->WriteInt32(1);
                    se->mTimer->WriteSummaryFromParcelLocked(out, NOWREAL);
                }
                else {
                    out->WriteInt32(0);
                }
            }
        }

        Int32 NP = u->mProcessStats.GetSize();
        out->WriteInt32(NP);
        if (NP > 0) {
            HashMap<String, AutoPtr<Uid::Proc> >::Iterator procIt = u->mProcessStats.Begin();
            for (; procIt != u->mProcessStats.End(); ++procIt) {
                out->WriteString(procIt->mFirst);
                AutoPtr<Uid::Proc> ps = procIt->mSecond;
                out->WriteInt64(ps->mUserTime);
                out->WriteInt64(ps->mSystemTime);
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
        }

        NP = u->mPackageStats.GetSize();
        out->WriteInt32(NP);
        if (NP > 0) {
            HashMap<String, AutoPtr<Uid::Pkg> >::Iterator pkgIt = u->mPackageStats.Begin();
            for (; pkgIt != u->mPackageStats.End(); ++pkgIt) {
                out->WriteString(pkgIt->mFirst);
                AutoPtr<Uid::Pkg> ps = pkgIt->mSecond;
                out->WriteInt32(ps->mWakeups);
                Int32 NS = ps->mServiceStats.GetSize();
                out->WriteInt32(NS);
                if (NS > 0) {
                    HashMap<String, AutoPtr<Uid::Pkg::Serv> >::Iterator servIt = ps->mServiceStats.Begin();
                    for (; servIt != ps->mServiceStats.End(); ++servIt) {
                        out->WriteString(servIt->mFirst);
                        AutoPtr<Uid::Pkg::Serv> ss = servIt->mSecond;
                        Int64 time = ss->GetStartTimeToNowLocked(NOW);
                        out->WriteInt64(time);
                        out->WriteInt32(ss->mStarts);
                        out->WriteInt32(ss->mLaunches);
                    }
                }
            }
        }

        out->WriteInt64(u->GetTcpBytesReceived(STATS_SINCE_CHARGED));
        out->WriteInt64(u->GetTcpBytesSent(STATS_SINCE_CHARGED));
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
        Logger::E(TAG, "Bad magic number");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new ParcelFormatException("Bad magic number");
    }

    ReadHistory(in, FALSE);

    in->ReadInt32(&mStartCount);
    in->ReadInt64(&mBatteryUptime);
    mBatteryLastUptime = 0;
    in->ReadInt64(&mBatteryRealtime);
    mBatteryLastRealtime = 0;
    mScreenOn = FALSE;
    mScreenOnTimer = new StopwatchTimer(NULL, -1, NULL, &mUnpluggables, in);
    for (Int32 i = 0; i < NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -100 - i,
                NULL, &mUnpluggables, in);
        mScreenBrightnessTimer->Set(i, timer);
    }
    mInputEventCounter = new Counter(&mUnpluggables, in);
    mPhoneOn = FALSE;
    mPhoneOnTimer = new StopwatchTimer(NULL, -2, NULL, &mUnpluggables, in);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -200 - i,
                NULL, &mUnpluggables, in);
        mPhoneSignalStrengthsTimer->Set(i, timer);
    }
    mPhoneSignalScanningTimer = new StopwatchTimer(NULL, -200 + 1, NULL, &mUnpluggables, in);
    for (Int32 i = 0; i < NUM_DATA_CONNECTION_TYPES; i++) {
        AutoPtr<StopwatchTimer> timer = new StopwatchTimer(NULL, -300-i,
                NULL, &mUnpluggables, in);
        mPhoneDataConnectionsTimer->Set(i, timer);
    }
    mWifiOn = FALSE;
    mWifiOnTimer = new StopwatchTimer(NULL, -2, NULL, &mUnpluggables, in);
    mGlobalWifiRunning = FALSE;
    mGlobalWifiRunningTimer = new StopwatchTimer(NULL, -2, NULL, &mUnpluggables, in);
    mBluetoothOn = FALSE;
    mBluetoothOnTimer = new StopwatchTimer(NULL, -2, NULL, &mUnpluggables, in);
    in->ReadInt64(&mUptime);
    in->ReadInt64(&mUptimeStart);
    mLastUptime = 0;
    in->ReadInt64(&mRealtime);
    in->ReadInt64(&mRealtimeStart);
    mLastRealtime = 0;
    in->ReadBoolean(&mOnBattery);
    mOnBatteryInternal = FALSE; // we are no longer really running.
    in->ReadInt64(&mTrackBatteryPastUptime);
    in->ReadInt64(&mTrackBatteryUptimeStart);
    in->ReadInt64(&mTrackBatteryPastRealtime);
    in->ReadInt64(&mTrackBatteryRealtimeStart);
    in->ReadInt64(&mUnpluggedBatteryUptime);
    in->ReadInt64(&mUnpluggedBatteryRealtime);
    in->ReadInt32(&mDischargeUnplugLevel);
    in->ReadInt32(&mDischargeCurrentLevel);
    in->ReadInt32(&mLowDischargeAmountSinceCharge);
    in->ReadInt32(&mHighDischargeAmountSinceCharge);
    in->ReadInt32(&mDischargeAmountScreenOn);
    in->ReadInt32(&mDischargeAmountScreenOnSinceCharge);
    in->ReadInt32(&mDischargeAmountScreenOff);
    in->ReadInt32(&mDischargeAmountScreenOffSinceCharge);
    in->ReadInt64(&mLastWriteTime);

    in->ReadInt64(&(*mMobileDataRx)[STATS_LAST]);
    (*mMobileDataRx)[STATS_SINCE_UNPLUGGED] = -1;
    in->ReadInt64(&(*mMobileDataTx)[STATS_LAST]);
    (*mMobileDataTx)[STATS_SINCE_UNPLUGGED] = -1;
    in->ReadInt64(&(*mTotalDataRx)[STATS_LAST]);
    (*mTotalDataRx)[STATS_SINCE_UNPLUGGED] = -1;
    in->ReadInt64(&(*mTotalDataTx)[STATS_LAST]);
    (*mTotalDataTx)[STATS_SINCE_UNPLUGGED] = -1;

    in->ReadInt64(&mRadioDataUptime);
    mRadioDataStart = -1;

    in->ReadInt32(&mBluetoothPingCount);
    mBluetoothPingStart = -1;

    mKernelWakelockStats.Clear();
    Int32 NKW;
    in->ReadInt32(&NKW);
    for (Int32 ikw = 0; ikw < NKW; ikw++) {
        Int32 value;
        if (in->ReadInt32(&value), value != 0) {
            String wakelockName;
            in->ReadString(&wakelockName);
            in->ReadInt32(&value); // Extra 0/1 written by Timer.writeTimerToParcel
            AutoPtr<SamplingTimer> kwlt = new SamplingTimer(&mUnpluggables, mOnBattery, in);
            mKernelWakelockStats[wakelockName] = kwlt;
        }
    }

    mPartialTimers.Clear();
    mFullTimers.Clear();
    mWindowTimers.Clear();
    mWifiRunningTimers.Clear();
    mFullWifiLockTimers.Clear();
    mWifiScanTimers.Clear();
    mWifiMulticastTimers.Clear();

    in->ReadInt32(&sNumSpeedSteps);

    Int32 numUids;
    in->ReadInt32(&numUids);
    mUidStats.Clear();
    for (Int32 i = 0; i < numUids; i++) {
        Int32 uid;
        in->ReadInt32(&uid);
        AutoPtr<Uid> u = new Uid(uid, this);
        u->ReadFromParcelLocked(&mUnpluggables, in);
        mUidStats[uid] = u;
    }
    return NOERROR;
}

ECode BatteryStatsImpl::WriteToParcel(
    /* [in] */ IParcel* out)
{
    WriteToParcelLocked(out, TRUE);
    return NOERROR;
}

void BatteryStatsImpl::WriteToParcelWithoutUids(
    /* [in] */ IParcel* out)
{
    WriteToParcelLocked(out, FALSE);
}

void BatteryStatsImpl::WriteToParcelLocked(
    /* [in] */ IParcel* out,
    /* [in] */ Boolean inclUids)
{
    // Need to update with current kernel wake lock counts.
    UpdateKernelWakelocksLocked();

    Int64 uSecUptime = SystemClock::GetUptimeMillis() * 1000;
    Int64 uSecRealtime = SystemClock::GetElapsedRealtime() * 1000;
    Int64 batteryUptime = GetBatteryUptimeLocked(uSecUptime);
    Int64 batteryRealtime = GetBatteryRealtimeLocked(uSecRealtime);

    out->WriteInt32(MAGIC);

    WriteHistory(out, FALSE);

    out->WriteInt32(mStartCount);
    out->WriteInt64(mBatteryUptime);
    out->WriteInt64(mBatteryRealtime);
    mScreenOnTimer->WriteToParcel(out, batteryRealtime);
    for (Int32 i = 0; i < NUM_SCREEN_BRIGHTNESS_BINS; i++) {
        (*mScreenBrightnessTimer)[i]->WriteToParcel(out, batteryRealtime);
    }
    mInputEventCounter->WriteToParcel(out);
    mPhoneOnTimer->WriteToParcel(out, batteryRealtime);
    for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
        (*mPhoneSignalStrengthsTimer)[i]->WriteToParcel(out, batteryRealtime);
    }
    mPhoneSignalScanningTimer->WriteToParcel(out, batteryRealtime);
    for (Int32 i = 0; i < NUM_DATA_CONNECTION_TYPES; i++) {
        (*mPhoneDataConnectionsTimer)[i]->WriteToParcel(out, batteryRealtime);
    }
    mWifiOnTimer->WriteToParcel(out, batteryRealtime);
    mGlobalWifiRunningTimer->WriteToParcel(out, batteryRealtime);
    mBluetoothOnTimer->WriteToParcel(out, batteryRealtime);
    out->WriteInt64(mUptime);
    out->WriteInt64(mUptimeStart);
    out->WriteInt64(mRealtime);
    out->WriteInt64(mRealtimeStart);
    out->WriteBoolean(mOnBattery);
    out->WriteInt64(batteryUptime);
    out->WriteInt64(mTrackBatteryUptimeStart);
    out->WriteInt64(batteryRealtime);
    out->WriteInt64(mTrackBatteryRealtimeStart);
    out->WriteInt64(mUnpluggedBatteryUptime);
    out->WriteInt64(mUnpluggedBatteryRealtime);
    out->WriteInt32(mDischargeUnplugLevel);
    out->WriteInt32(mDischargeCurrentLevel);
    out->WriteInt32(mLowDischargeAmountSinceCharge);
    out->WriteInt32(mHighDischargeAmountSinceCharge);
    out->WriteInt32(mDischargeAmountScreenOn);
    out->WriteInt32(mDischargeAmountScreenOnSinceCharge);
    out->WriteInt32(mDischargeAmountScreenOff);
    out->WriteInt32(mDischargeAmountScreenOffSinceCharge);
    out->WriteInt64(mLastWriteTime);

    out->WriteInt64(GetMobileTcpBytesReceived(STATS_SINCE_UNPLUGGED));
    out->WriteInt64(GetMobileTcpBytesSent(STATS_SINCE_UNPLUGGED));
    out->WriteInt64(GetTotalTcpBytesReceived(STATS_SINCE_UNPLUGGED));
    out->WriteInt64(GetTotalTcpBytesSent(STATS_SINCE_UNPLUGGED));

    // Write radio uptime for data
    out->WriteInt64(GetRadioDataUptime());

    out->WriteInt32(GetBluetoothPingCount());

    if (inclUids) {
        out->WriteInt32(mKernelWakelockStats.GetSize());
        HashMap<String, AutoPtr<SamplingTimer> >::Iterator it = mKernelWakelockStats.Begin();
        for (; it != mKernelWakelockStats.End(); ++it) {
            AutoPtr<SamplingTimer> kwlt = it->mSecond;
            if (kwlt != NULL) {
                out->WriteInt32(1);
                out->WriteString(it->mFirst);
                Timer::WriteTimerToParcel(out, kwlt, batteryRealtime);
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
        Int32 size = mUidStats.GetSize();
        out->WriteInt32(size);
        HashMap<Int32, AutoPtr<Uid> >::Iterator it = mUidStats.Begin();
        for (; it != mUidStats.End(); ++it) {
            out->WriteInt32(it->mFirst);

            it->mSecond->WriteToParcelLocked(out, batteryRealtime);
        }
    }
    else {
        out->WriteInt32(0);
    }
}

void BatteryStatsImpl::PrepareForDumpLocked()
{
    // Need to retrieve current kernel wake lock stats before printing.
    UpdateKernelWakelocksLocked();
}

void BatteryStatsImpl::DumpLocked(
    /* [in] */ IPrintWriter* pw)
{
    if (DEBUG) {
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
        pr->Println(String("*** Input event counter:"));
        mInputEventCounter->LogState(pr, String("  "));
        pr->Println(String("*** Phone timer:"));
        mPhoneOnTimer->LogState(pr, String("  "));
        for (Int32 i = 0; i < ISignalStrength::NUM_SIGNAL_STRENGTH_BINS; i++) {
            StringBuilder sb("*** Signal strength #");
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
        pr->Println(String("*** Wifi timer:"));
        mWifiOnTimer->LogState(pr, String("  "));
        pr->Println(String("*** WifiRunning timer:"));
        mGlobalWifiRunningTimer->LogState(pr, String("  "));
        pr->Println(String("*** Bluetooth timer:"));
        mBluetoothOnTimer->LogState(pr, String("  "));
        pr->Println(String("*** Mobile ifaces:"));
        // pr->Println(mMobileIfaces.toString());
    }
    BatteryStats::DumpLocked(pw);
}

AutoPtr<INetworkStats> BatteryStatsImpl::GetNetworkStatsSummary()
{
    // NOTE: calls from BatteryStatsService already hold this lock
    AutoLock lock(mLock);
    Int64 age;
    if (mNetworkSummaryCache == NULL
            || (mNetworkSummaryCache->GetElapsedRealtimeAge(&age), age > IDateUtils::SECOND_IN_MILLIS)) {
        mNetworkSummaryCache = NULL;

        AutoPtr<ISystemProperties> prop;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&prop);
        Boolean value;
        if (prop->GetBoolean(String("net.qtaguid_enabled")/*PROP_QTAGUID_ENABLED*/, FALSE, &value), value) {
            // try {
            if (FAILED(mNetworkStatsFactory->ReadNetworkStatsSummaryDev((INetworkStats**)&mNetworkSummaryCache))) {
                Logger::E(TAG, "problem reading network stats");
            }
            // } catch (IllegalStateException e) {
            //     Log.wtf(TAG, "problem reading network stats", e);
            // }
        }

        if (mNetworkSummaryCache == NULL) {
            CNetworkStats::New(SystemClock::GetElapsedRealtime(), 0, (INetworkStats**)&mNetworkSummaryCache);
        }
    }
    return mNetworkSummaryCache;
}

AutoPtr<INetworkStats> BatteryStatsImpl::GetNetworkStatsDetailGroupedByUid()
{
    // NOTE: calls from BatteryStatsService already hold this lock
    AutoLock lock(mLock);
    Int64 age;
    if (mNetworkDetailCache == NULL
            || (mNetworkDetailCache->GetElapsedRealtimeAge(&age), age > IDateUtils::SECOND_IN_MILLIS)) {
        mNetworkDetailCache = NULL;

        AutoPtr<ISystemProperties> prop;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&prop);
        Boolean value;
        if (prop->GetBoolean(String("net.qtaguid_enabled")/*PROP_QTAGUID_ENABLED*/, FALSE, &value), value) {
            // try {
            AutoPtr<INetworkStats> detail;
            if (FAILED(mNetworkStatsFactory->ReadNetworkStatsDetail((INetworkStats**)&detail))) {
                Logger::E(TAG, "problem reading network stats");
            }
            else {
                detail->GroupedByUid((INetworkStats**)&mNetworkDetailCache);
            }
            // } catch (IllegalStateException e) {
            //     Log.wtf(TAG, "problem reading network stats", e);
            // }
        }

        if (mNetworkDetailCache == NULL) {
            CNetworkStats::New(SystemClock::GetElapsedRealtime(), 0, (INetworkStats**)&mNetworkDetailCache);
        }
    }
    return mNetworkDetailCache;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
