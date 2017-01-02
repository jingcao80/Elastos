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

#include "TimerTask.h"
#include "AutoLock.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(TimerTask, Runnable, ITimerTask)

TimerTask::TimerTask()
    : mCancelled(FALSE)
    , mWhen(0)
    , mPeriod(0)
    , mFixedRate(FALSE)
    , mScheduledTime(0)
{}


TimerTask::~TimerTask()
{
}

ECode TimerTask::GetWhen(
    /* [out] */ Int64* when)
{
    VALIDATE_NOT_NULL(when)
    AutoLock lock(mLock);
    *when = mWhen;
    return NOERROR;
}

ECode TimerTask::SetScheduledTime(
    /* [in] */ Int64 time)
{
    AutoLock lock(mLock);
    mScheduledTime = time;
    return NOERROR;
}

ECode TimerTask::IsScheduled(
    /* [out] */ Boolean* scheduled)
{
    VALIDATE_NOT_NULL(scheduled)
    AutoLock lock(mLock);
    *scheduled = mWhen > 0 || mScheduledTime > 0;
    return NOERROR;
}

ECode TimerTask::Cancel(
    /* [out] */ Boolean* cancelled)
{
    VALIDATE_NOT_NULL(cancelled)
    AutoLock lock(mLock);
    Boolean willRun = !mCancelled && mWhen > 0;
    mCancelled = TRUE;
    *cancelled = willRun;
    return NOERROR;
}

ECode TimerTask::ScheduledExecutionTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)
    AutoLock lock(mLock);
    *time = mScheduledTime;
    return NOERROR;
}

ECode TimerTask::SetWhen(
    /* [in] */ Int64 when)
{
    AutoLock lock(mLock);
    mWhen = when;
    return NOERROR;
}

ECode TimerTask::IsCancelled(
    /* [out] */ Boolean* cancelled)
{
    VALIDATE_NOT_NULL(cancelled)
    AutoLock lock(mLock);
    *cancelled = mCancelled;
    return NOERROR;
}

ECode TimerTask::GetPeriod(
    /* [out] */ Int64* period)
{
    VALIDATE_NOT_NULL(period)
    AutoLock lock(mLock);
    *period = mPeriod;
    return NOERROR;
}

ECode TimerTask::SetPeriod(
    /* [in] */ Int64 period)
{
    AutoLock lock(mLock);
    mPeriod = period;
    return NOERROR;
}

ECode TimerTask::IsFixedRate(
    /* [out] */ Boolean* fixed)
{
    VALIDATE_NOT_NULL(fixed);
    AutoLock lock(mLock);
    *fixed = mFixedRate;
    return NOERROR;
}

ECode TimerTask::SetFixedRate(
    /* [in] */ Boolean fixed)
{
    AutoLock lock(mLock);
    mFixedRate = fixed;
    return NOERROR;
}

ECode TimerTask::Lock()
{
    mLock.Lock();
    return NOERROR;
}

ECode TimerTask::Unlock()
{
    mLock.Unlock();
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
