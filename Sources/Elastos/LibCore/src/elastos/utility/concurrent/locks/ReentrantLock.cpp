
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "ReentrantLock.h"
#include "Thread.h"
//#include <elastos/utility/logging/Slogger.h>
//using Elastos::Utility::Logging::Slogger;

using Elastos::Core::Thread;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {

//===============================================================================
// ReentrantLock::Sync
//===============================================================================

Boolean ReentrantLock::Sync::NonfairTryAcquire(
    /* [in] */ Int32 acquires)
{
    AutoPtr<IThread> current = Thread::GetCurrentThread();
    Int32 c = GetState();
    if (c == 0) {
        if (CompareAndSetState(0, acquires)) {
            SetExclusiveOwnerThread(current);
            return TRUE;
        }
    }
    else if (current == GetExclusiveOwnerThread()) {
        Int32 nextc = c + acquires;
        if (nextc < 0) { // overflow
            // throw new Error("Maximum lock count exceeded");
            return FALSE;
        }

        SetState(nextc);
        return TRUE;
    }
    return FALSE;
}

Boolean ReentrantLock::Sync::TryRelease(
    /* [in] */ Int32 releases)
{
    Int32 c = GetState() - releases;
    if (Thread::GetCurrentThread() != GetExclusiveOwnerThread()) {
        // throw new IllegalMonitorStateException();
        return FALSE;
    }

    Boolean isFree = FALSE;
    if (c == 0) {
        isFree = TRUE;
        SetExclusiveOwnerThread(NULL);
    }
    SetState(c);
    return isFree;
}

Boolean ReentrantLock::Sync::IsHeldExclusively()
{
    // While we must in general read state before owner,
    // we don't need to do so to check if current thread is owner
    // return getExclusiveOwnerThread() == Thread.currentThread();
    return FALSE;
}

AutoPtr<AbstractQueuedSynchronizer::ConditionObject> ReentrantLock::Sync::NewCondition()
{
    AutoPtr<AbstractQueuedSynchronizer::ConditionObject> obj =
        new AbstractQueuedSynchronizer::ConditionObject(this);
    return obj;
}

// Methods relayed from outer class
AutoPtr<IThread> ReentrantLock::Sync::GetOwner()
{
    return GetState() == 0 ? NULL : GetExclusiveOwnerThread();
}

Int32 ReentrantLock::Sync::GetHoldCount()
{
    return IsHeldExclusively() ? GetState() : 0;
}

Boolean ReentrantLock::Sync::IsLocked()
{
    return GetState() != 0;
}

void ReentrantLock::Sync::ReadObject(
    /* [in] */ IObjectInputStream* s)
{
    s->DefaultReadObject();
    SetState(0); // reset to unlocked state
}

//===============================================================================
// ReentrantLock::NonfairSync
//===============================================================================
ECode ReentrantLock::NonfairSync::Lock()
{
    if (CompareAndSetState(0, 1)) {
        AutoPtr<IThread> current = Thread::GetCurrentThread();
        SetExclusiveOwnerThread(current);
    }
    else {
        TryAcquire(1);
    }
    return NOERROR;
}

Boolean ReentrantLock::NonfairSync::TryAcquire(
    /* [in] */ Int32 acquires)
{
    return NonfairTryAcquire(acquires);
}

//===============================================================================
// ReentrantLock::FairSync
//===============================================================================
ECode ReentrantLock::FairSync::Lock()
{
    TryAcquire(1);
    return NOERROR;
}

Boolean ReentrantLock::FairSync::TryAcquire(
    /* [in] */ Int32 acquires)
{
    AutoPtr<IThread> current = Thread::GetCurrentThread();
    Int32 c = GetState();
    if (c == 0) {
        Boolean b = FALSE;
        if (!(HasQueuedPredecessors(&b), b) &&
            CompareAndSetState(0, acquires)) {
            SetExclusiveOwnerThread(current);
            return TRUE;
        }
    }
    else if (current == GetExclusiveOwnerThread()) {
        Int32 nextc = c + acquires;
        if (nextc < 0) {
            // throw new Error("Maximum lock count exceeded");
            return FALSE;
        }
        SetState(nextc);
        return TRUE;
    }
    return FALSE;
}

//===============================================================================
// ReentrantLock
//===============================================================================
CAR_INTERFACE_IMPL_3(ReentrantLock, Object, IReentrantLock, ISerializable, ILock)

ECode ReentrantLock::constructor()
{
    mSync = new NonfairSync();
    return NOERROR;
}

ECode ReentrantLock::constructor(
    /* [in] */ Boolean fair)
{
    if (fair) {
        mSync = new FairSync();
    }
    else {
        mSync = new NonfairSync();
    }
    return NOERROR;
}

ECode ReentrantLock::Lock()
{
    mSync->Lock();
    return NOERROR;
}

ECode ReentrantLock::LockInterruptibly()
{
    mSync->AcquireInterruptibly(1);
    return NOERROR;
}


ECode ReentrantLock::TryLock(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSync->NonfairTryAcquire(1);
    return NOERROR;
}

ECode ReentrantLock::TryLock(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(unit);
    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    return mSync->TryAcquireNanos(1, nanos, result);
}

ECode ReentrantLock::UnLock()
{
    mSync->TryRelease(1);
    return NOERROR;
}

ECode ReentrantLock::NewCondition(
    /* [out] */ ICondition** condition)
{
    VALIDATE_NOT_NULL(condition);
    AutoPtr<ICondition> c = mSync->NewCondition();
    *condition = c;
    REFCOUNT_ADD(*condition);
    return NOERROR;
}

ECode ReentrantLock::GetHoldCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
     *count = mSync->GetHoldCount();
    return NOERROR;
}

ECode ReentrantLock::IsHeldByCurrentThread(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSync->IsHeldExclusively();
    return NOERROR;
}

ECode ReentrantLock::IsLocked(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSync->IsLocked();
    return NOERROR;
}

ECode ReentrantLock::IsFair(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
//    *result = mSync->Probe(EIID_FairSync) != NULL;
    return NOERROR;
}

AutoPtr<IThread> ReentrantLock::GetOwner()
{
    return mSync->GetOwner();
}

ECode ReentrantLock::HasQueuedThreads(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSync->HasQueuedThreads(result);
}

ECode ReentrantLock::HasQueuedThread(
    /* [in] */ IThread* thread,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return mSync->IsQueued(thread, result);
}

ECode ReentrantLock::GetQueueLength(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    return mSync->GetQueueLength(result);
}

AutoPtr<ICollection> ReentrantLock::GetQueuedThreads()
{
    AutoPtr<ICollection> res;
    mSync->GetQueuedThreads((ICollection**)&res);
    return res;
}

ECode ReentrantLock::HasWaiters(
    /* [in] */ ICondition* condition,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (condition == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    IConditionObject* co = IConditionObject::Probe(condition);
    if (co == NULL) {
        //throw new IllegalArgumentException("not owner");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AbstractQueuedSynchronizer::ConditionObject* obj = (AbstractQueuedSynchronizer::ConditionObject*)co;
    return mSync->HasWaiters(obj, result);
}

ECode ReentrantLock::GetWaitQueueLength(
    /* [in] */ ICondition* condition,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;
    if (condition == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    IConditionObject* co = IConditionObject::Probe(condition);
    if (co == NULL) {
        //throw new IllegalArgumentException("not owner");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AbstractQueuedSynchronizer::ConditionObject* obj = (AbstractQueuedSynchronizer::ConditionObject*)co;
    return mSync->GetWaitQueueLength(obj, result);
}

ECode ReentrantLock::GetWaitingThreads(
    /* [in] */ ICondition* condition,
    /* [out] */ ICollection** threads)
{
    VALIDATE_NOT_NULL(threads);
    if (condition == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    IConditionObject* co = IConditionObject::Probe(condition);
    if (co == NULL) {
        //throw new IllegalArgumentException("not owner");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AbstractQueuedSynchronizer::ConditionObject* obj = (AbstractQueuedSynchronizer::ConditionObject*)co;
    return mSync->GetWaitingThreads(obj, threads);
}

ECode ReentrantLock::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);

    AutoPtr<IThread> o = mSync->GetOwner();
    if (o == NULL) {
        *info = String("[Unlocked]");
    }
    else {
        String name;
        o->GetName(&name);
        String result;
        result.AppendFormat("[Locked by thread %s]", name.string());
        *info = result;
    }
    return NOERROR;
}

} // namespace Locks
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
