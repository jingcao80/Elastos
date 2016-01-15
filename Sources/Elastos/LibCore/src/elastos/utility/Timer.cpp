
#include "Timer.h"
#include "CSystem.h"
#include "StringBuilder.h"
#include "AutoLock.h"

using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Utility {

//==========================================================
// Timer::TimerImpl::TimerHeap
//==========================================================
Timer::TimerImpl::TimerHeap::TimerHeap()
    : DEFAULT_HEAP_SIZE(256)
    , mTimers(ArrayOf<ITimerTask*>::Alloc(DEFAULT_HEAP_SIZE))
    , mSize(0)
    , mDeletedCancelledNumber(0)
{}

Timer::TimerImpl::TimerHeap::~TimerHeap()
{}

AutoPtr<ITimerTask> Timer::TimerImpl::TimerHeap::Minimum()
{
    return (*mTimers)[0];
}

Boolean Timer::TimerImpl::TimerHeap::IsEmpty()
{
    return mSize == 0;
}

void Timer::TimerImpl::TimerHeap::Insert(
    /* [in] */ ITimerTask* task)
{
    if (mTimers->GetLength() == mSize) {
        AutoPtr< ArrayOf<ITimerTask*> > appendedTimers = ArrayOf<ITimerTask*>::Alloc(mSize * 2);
        appendedTimers->Copy(mTimers);
        mTimers = appendedTimers;
    }
    mTimers->Set(mSize++, task);
    UpHeap();
}

void Timer::TimerImpl::TimerHeap::Delete(
    /* [in] */ Int32 pos)
{
    if (pos >= 0 && pos < mSize) {
        mTimers->Set(pos, (*mTimers)[--mSize]);
        mTimers->Set(mSize, NULL);
        DownHeap(pos);
    }
}

void Timer::TimerImpl::TimerHeap::UpHeap()
{
    Int32 current = mSize - 1;
    Int32 parent = (current - 1) / 2;

    Int64 curWhen, parWhen;
    while ((*mTimers)[current]->GetWhen(&curWhen),
            (*mTimers)[parent]->GetWhen(&parWhen),
            curWhen < parWhen) {
        // swap the two
        AutoPtr<ITimerTask> tmp = (*mTimers)[current];
        mTimers->Set(current, (*mTimers)[parent]);
        mTimers->Set(parent, tmp);

        // update pos and current
        current = parent;
        parent = (current - 1) / 2;
    }
}

void Timer::TimerImpl::TimerHeap::DownHeap(
    /* [in] */ Int32 pos)
{
    Int32 current = pos;
    Int32 child = 2 * current + 1;

    Int64 when1, when2;
    while (child < mSize && mSize > 0) {
        // compare the children if they exist
        if (child + 1 < mSize
            && ((*mTimers)[child + 1]->GetWhen(&when1),
                (*mTimers)[child]->GetWhen(&when2),
                when1 < when2)) {
            child++;
        }

        // compare selected child with parent
        if ((*mTimers)[current]->GetWhen(&when1),
            (*mTimers)[child]->GetWhen(&when2),
            when1 < when2) {
            break;
        }

        // swap the two
        AutoPtr<ITimerTask> tmp = (*mTimers)[current];
        mTimers->Set(current, (*mTimers)[child]);
        mTimers->Set(child, tmp);

        // update pos and current
        current = child;
        child = 2 * current + 1;
    }
}

void Timer::TimerImpl::TimerHeap::Reset()
{
    mTimers = ArrayOf<ITimerTask*>::Alloc(DEFAULT_HEAP_SIZE);;
    mSize = 0;
}

void Timer::TimerImpl::TimerHeap::AdjustMinimum()
{
    DownHeap(0);
}

void Timer::TimerImpl::TimerHeap::DeleteIfCancelled()
{
    for (Int32 i = 0; i < mSize; i++) {
        Boolean cancelled;
        if ((*mTimers)[i]->IsCancelled(&cancelled), cancelled) {
            mDeletedCancelledNumber++;
            Delete(i);
            // re-try this point
            i--;
        }
    }
}

ECode Timer::TimerImpl::TimerHeap::GetTask(
    /* [in] */ ITimerTask* task)
{
    for (Int32 i = 0; i < mTimers->GetLength(); i++) {
        if ((*mTimers)[i] == task) {
            return i;
        }
    }
    return -1;
}


//==========================================================
// Timer::TimerImpl
//==========================================================
Timer::TimerImpl::TimerImpl(
    /* [in] */ const String& name,
    /* [in] */ Boolean isDaemon)
    : mCancelled(FALSE)
    , mFinished(FALSE)
    , mTasks(new TimerHeap())
{
    Thread::constructor();
    Thread::SetName(name);
    Thread::SetDaemon(isDaemon);
}

Timer::TimerImpl::~TimerImpl()
{}

ECode Timer::TimerImpl::Run()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    while (TRUE) {
        AutoPtr<ITimerTask> task;
        synchronized (this) {
            // need to check cancelled inside the synchronized block
            if (mCancelled) {
                return NOERROR;
            }
            if (mTasks->IsEmpty()) {
                if (mFinished) {
                    return NOERROR;
                }
                // no tasks scheduled -- sleep until any task appear
                // try {
                Wait();
                // } catch (InterruptedException ignored) {
                // }
                continue;
            }

            Int64 currentTime;
            system->GetCurrentTimeMillis(&currentTime);

            task = mTasks->Minimum();
            Int64 timeToSleep;

            task->Lock();
            {
                Boolean cancelled;
                if (task->IsCancelled(&cancelled), cancelled) {
                    mTasks->Delete(0);
                    task->Unlock();
                    continue;
                }

                // check the time to sleep for the first task scheduled
                Int64 when;
                task->GetWhen(&when);
                timeToSleep = when - currentTime;
            }
            task->Unlock();

            if (timeToSleep > 0) {
                // sleep!
                // try {
                Wait(timeToSleep);
                // } catch (InterruptedException ignored) {
                // }
                continue;
            }

            // no sleep is necessary before launching the task

            task->Lock();
            {
                Int32 pos = 0;
                Int64 when1, when2;
                mTasks->Minimum()->GetWhen(&when1);
                task->GetWhen(&when2);
                if (when1 != when2) {
                    pos = mTasks->GetTask(task);
                }
                Boolean cancelled;
                task->IsCancelled(&cancelled);
                if (cancelled) {
                    mTasks->Delete(mTasks->GetTask(task));
                    task->Unlock();
                    continue;
                }

                // set time to schedule
                task->SetScheduledTime(when2);

                // remove task from queue
                mTasks->Delete(pos);

                // set when the next task should be launched
                Int64 period;
                if (task->GetPeriod(&period), period >= 0) {
                    // this is a repeating task,
                    Boolean fixed;
                    if (task->IsFixedRate(&fixed), fixed) {
                        // task is scheduled at fixed rate
                        task->SetWhen(when2 + period);
                    }
                    else {
                        // task is scheduled at fixed delay
                        Int64 millis;
                        system->GetCurrentTimeMillis(&millis);
                        task->SetWhen(millis + period);
                    }

                    // insert this task into queue
                    InsertTask(task);
                }
                else {
                    task->SetWhen(0);
                }
            }
            task->Unlock();
        }

        Boolean taskCompletedNormally = FALSE;
        if (SUCCEEDED(IRunnable::Probe(task)->Run())) {
            taskCompletedNormally = TRUE;
        }
        if (!taskCompletedNormally) {
            synchronized(this) {
                mCancelled = TRUE;
            }
        }
    }
    return NOERROR;
}

void Timer::TimerImpl::InsertTask(
    /* [in] */ ITimerTask* newTask)
{
    // callers are synchronized
    mTasks->Insert(newTask);
    Notify();
}

void Timer::TimerImpl::Cancel()
{
    synchronized(this) {
        mCancelled = TRUE;
        mTasks->Reset();
        Notify();
    }
}

Int32 Timer::TimerImpl::Purge()
{
    if (mTasks->IsEmpty()) {
        return 0;
    }
    // callers are synchronized
    mTasks->mDeletedCancelledNumber = 0;
    mTasks->DeleteIfCancelled();
    return mTasks->mDeletedCancelledNumber;
}

//==========================================================
// Timer::FinalizerHelper
//==========================================================
Timer::FinalizerHelper::FinalizerHelper(
    /* [in] */ TimerImpl* impl)
    : mImpl(impl)
{}

Timer::FinalizerHelper::~FinalizerHelper()
{
    AutoLock lock(mImpl);
    mImpl->mFinished = TRUE;
    mImpl->Notify();
}

//==========================================================
// Timer::Timer
//==========================================================

Int64 Timer::sTimerId = 0;
Object Timer::sTimerIdLock;

CAR_INTERFACE_IMPL(Timer, Object, ITimer)

Int64 Timer::NextId()
{
    AutoLock lock(sTimerIdLock);
    return sTimerId++;
}

Timer::Timer()
{}

Timer::Timer(
    /* [in] */ const String& name,
    /* [in] */ Boolean isDaemon)
{
    ASSERT_SUCCEEDED(constructor(name, isDaemon));
}

Timer::Timer(
    /* [in] */ const String& name)
{
    ASSERT_SUCCEEDED(constructor(name, FALSE));
}

Timer::Timer(
    /* [in] */Boolean isDaemon)
{
    ASSERT_SUCCEEDED(constructor(isDaemon));
}

Timer::~Timer()
{}

ECode Timer::constructor(
    /* [in] */ const String& name,
    /* [in] */ Boolean isDaemon)
{
    if (name.IsNull()){
        // throw new NullPointerException("name is null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mImpl = new TimerImpl(name, isDaemon);
    mImpl->Start();
    mFinalizer = new FinalizerHelper(mImpl);
    return NOERROR;
}

ECode Timer::constructor(
    /* [in] */ const String& name)
{
    return constructor(name, FALSE);
}

ECode Timer::constructor(
    /* [in] */Boolean isDaemon)
{
    StringBuilder sb;
    sb += "Timer-";
    sb += Timer::NextId();
    return constructor(sb.ToString(), isDaemon);
}

ECode Timer::constructor()
{
    return constructor(FALSE);
}

ECode Timer::Cancel()
{
    mImpl->Cancel();
    return NOERROR;
}

ECode Timer::Purge(
    /* [out] */ Int32* number)
{
    AutoLock lock(mImpl);
    *number = mImpl->Purge();
    return NOERROR;
}

ECode Timer::Schedule(
    /* [in] */ ITimerTask* task,
    /* [in] */ IDate* when)
{
    Int64 time;
    when->GetTime(&time);
    if (time < 0) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 delay;
    system->GetCurrentTimeMillis(&delay);
    delay = time - delay;
    return ScheduleImpl(task, delay < 0 ? 0 : delay, -1, FALSE);
}

ECode Timer::Schedule(
    /* [in] */ ITimerTask* task,
    /* [in] */ Int64 delay)
{
    if (delay < 0) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return ScheduleImpl(task, delay, -1, FALSE);
}

ECode Timer::Schedule(
    /* [in] */ ITimerTask* task,
    /* [in] */ Int64 delay,
    /* [in] */ Int64 period)
{
    if (delay < 0 || period <= 0) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return ScheduleImpl(task, delay, period, FALSE);
}

ECode Timer::Schedule(
    /* [in] */ ITimerTask* task,
    /* [in] */ IDate* when,
    /* [in] */ Int64 period)
{
    Int64 time;
    when->GetTime(&time);
    if (period <= 0 || time < 0) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 delay;
    system->GetCurrentTimeMillis(&delay);
    delay = time - delay;
    return ScheduleImpl(task, delay < 0 ? 0 : delay, period, FALSE);
}

ECode Timer::ScheduleAtFixedRate(
    /* [in] */ ITimerTask* task,
    /* [in] */ Int64 delay,
    /* [in] */ Int64 period)
{
    if (delay < 0 || period <= 0) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return ScheduleImpl(task, delay, period, TRUE);
}

ECode Timer::ScheduleAtFixedRate(
    /* [in] */ ITimerTask* task,
    /* [in] */ IDate* when,
    /* [in] */ Int64 period)
{
    Int64 time;
    when->GetTime(&time);
    if (period <= 0 || time < 0) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    Int64 delay;
    system->GetCurrentTimeMillis(&delay);
    delay = time - delay;
    return ScheduleImpl(task, delay, period, TRUE);
}

ECode Timer::ScheduleImpl(
    /* [in] */ ITimerTask* task,
    /* [in] */ Int64 delay,
    /* [in] */ Int64 period,
    /* [in] */ Boolean fixed)
{
    AutoLock lock(mImpl);

    if (mImpl->mCancelled) {
        // throw new IllegalStateException("Timer was canceled");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);

    Int64 when;
    system->GetCurrentTimeMillis(&when);
    when = delay + when;

    if (when < 0) {
        // throw new IllegalArgumentException("Illegal delay to start the TimerTask: " + when);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    task->Lock();
    {
        Boolean scheduled;
        if (task->IsScheduled(&scheduled), scheduled) {
            // throw new IllegalStateException("TimerTask is scheduled already");
            task->Unlock();
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        Boolean cancelled;
        if (task->IsCancelled(&cancelled), cancelled) {
            // throw new IllegalStateException("TimerTask is canceled");
            task->Unlock();
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        task->SetWhen(when);
        task->SetPeriod(period);
        task->SetFixedRate(fixed);
    }
    task->Unlock();

    // insert the newTask into queue
    mImpl->InsertTask(task);
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
