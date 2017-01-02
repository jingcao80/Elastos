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

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "CReentrantReadWriteLock.h"
#include "StringUtils.h"

using Elastos::Core::StringUtils;
using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {

//==========================================================
//       CReentrantReadWriteLock
//==========================================================
const Int64 CReentrantReadWriteLock::sSerialVersionUID = -6992448646407690164L;

CAR_INTERFACE_IMPL_3(CReentrantReadWriteLock, Object, IReentrantReadWriteLock, IReadWriteLock, ISerializable)

CAR_OBJECT_IMPL(CReentrantReadWriteLock)

ECode CReentrantReadWriteLock::constructor()
{
    return constructor(FALSE);
}

ECode CReentrantReadWriteLock::constructor(
    /* [in] */ Boolean fair)
{
    mSync = fair ? (Sync*) new CReentrantReadWriteLock::FairSync() : (Sync*) new CReentrantReadWriteLock::NonfairSync();
    mReaderLock = new CReadLock(this);
    mWriterLock = new CWriteLock(this);
    return NOERROR;
}

ECode CReentrantReadWriteLock::ReadLock(
    /* [out] */ ILock** outlock)
{
    VALIDATE_NOT_NULL(outlock)

    *outlock = (ILock*) mReaderLock->Probe(EIID_ILock);
    REFCOUNT_ADD(*outlock)
    return NOERROR;
}

ECode CReentrantReadWriteLock::WriteLock(
    /* [out] */ ILock** outlock)
{
    VALIDATE_NOT_NULL(outlock)

    *outlock = (ILock*) mWriterLock->Probe(EIID_ILock);
    REFCOUNT_ADD(*outlock)
    return NOERROR;
}

ECode CReentrantReadWriteLock::IsFair(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

//    *value = mSync->Probe(CLSID_FairSync) == NULL ? FALSE : TRUE;
    return NOERROR;
}

AutoPtr<IThread> CReentrantReadWriteLock::GetOwner()
{
    AutoPtr<IThread> res = mSync->GetOwner();
    return res;
}

ECode CReentrantReadWriteLock::GetReadLockCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mSync->GetReadLockCount();
    return NOERROR;
}

ECode CReentrantReadWriteLock::IsWriteLocked(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mSync->IsWriteLocked();
    return NOERROR;
}

ECode CReentrantReadWriteLock::IsWriteLockedByCurrentThread(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mSync->IsHeldExclusively();
    return NOERROR;
}

ECode CReentrantReadWriteLock::GetWriteHoldCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mSync->GetWriteHoldCount();
    return NOERROR;
}

ECode CReentrantReadWriteLock::GetReadHoldCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mSync->GetReadHoldCount();
    return NOERROR;
}

AutoPtr<ICollection> CReentrantReadWriteLock::GetQueuedWriterThreads()
{
    AutoPtr<ICollection> res;
    mSync->GetExclusiveQueuedThreads((ICollection**)&res);
    return res;
}

AutoPtr<ICollection> CReentrantReadWriteLock::GetQueuedReaderThreads()
{
    AutoPtr<ICollection> res;
    mSync->GetSharedQueuedThreads((ICollection**)&res);
    return res;
}

ECode CReentrantReadWriteLock::HasQueuedThreads(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    return mSync->HasQueuedThreads(value);
}

ECode CReentrantReadWriteLock::HasQueuedThread(
    /* [in] */ IThread* thread,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    return mSync->IsQueued(thread, value);
}

ECode CReentrantReadWriteLock::GetQueueLength(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    return mSync->GetQueueLength(value);
}

AutoPtr<ICollection> CReentrantReadWriteLock::GetQueuedThreads()
{
    AutoPtr<ICollection> res;
    mSync->GetQueuedThreads((ICollection**)&res);
    return res;
}

ECode CReentrantReadWriteLock::HasWaiters(
    /* [in] */ ICondition* condition,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    if (condition == NULL) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    assert(0 && "TODO");
    // if (!(condition instanceof AbstractQueuedSynchronizer.ConditionObject))
    //     throw new IllegalArgumentException("not owner");
    // return sync.hasWaiters((AbstractQueuedSynchronizer.ConditionObject)condition);
    return NOERROR;
}

ECode CReentrantReadWriteLock::GetWaitQueueLength(
    /* [in] */ ICondition* condition,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    if (condition == NULL) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    assert(0 && "TODO");
    // if (!(condition instanceof AbstractQueuedSynchronizer.ConditionObject))
    //     throw new IllegalArgumentException("not owner");
    // return sync.getWaitQueueLength((AbstractQueuedSynchronizer.ConditionObject)condition);
    return NOERROR;
}

AutoPtr<ICollection> CReentrantReadWriteLock::GetWaitingThreads(
    /* [in] */ ICondition* condition)
{
    if (condition == NULL) {
        // throw new NullPointerException();
        return NULL;
    }

    assert(0 && "TODO");
    // if (!(condition instanceof AbstractQueuedSynchronizer.ConditionObject))
    //     throw new IllegalArgumentException("not owner");
    // return sync.getWaitingThreads((AbstractQueuedSynchronizer.ConditionObject)condition);
    return NULL;
}

ECode CReentrantReadWriteLock::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Int32 c = mSync->GetCount();
    Int32 w = Sync::ExclusiveCount(c);
    Int32 r = Sync::SharedCount(c);

    // return super.toString() + "[Write locks = " + w + ", Read locks = " + r + "]";
    *str = String("[Write locks = ") +
           StringUtils::ToString(w) +
           String(", Read locks = ") +
           StringUtils::ToString(r) +
           String("]");

    return NOERROR;
}


//==========================================================
//       CReentrantReadWriteLock::Sync
//==========================================================
const Int32 CReentrantReadWriteLock::Sync::SHARED_SHIFT   = 16;
const Int32 CReentrantReadWriteLock::Sync::SHARED_UNIT    = (1 << SHARED_SHIFT);
const Int32 CReentrantReadWriteLock::Sync::MAX_COUNT      = (1 << SHARED_SHIFT) - 1;
const Int32 CReentrantReadWriteLock::Sync::EXCLUSIVE_MASK = (1 << SHARED_SHIFT) - 1;

static const Int64 sSerialVersionUID = 6317671515068378041L;

Int32 CReentrantReadWriteLock::Sync::SharedCount(
    /* [in] */ Int32 c)
{
    return c >> SHARED_SHIFT;
}

Int32 CReentrantReadWriteLock::Sync::ExclusiveCount(
    /* [in] */ Int32 c)
{
    return c & EXCLUSIVE_MASK;
}

CReentrantReadWriteLock::Sync::Sync()
{
    mReadHolds = new ThreadLocalHoldCounter();
    SetState(GetState()); // ensures visibility of readHolds
}

Boolean CReentrantReadWriteLock::Sync::TryWriteLock()
{
    AutoPtr<IThread> current = Thread::GetCurrentThread();
    Int32 c = GetState();
    if (c != 0) {
        Int32 w = ExclusiveCount(c);
        if (w == 0 || current != GetExclusiveOwnerThread()) {
            return FALSE;
        }
        if (w == MAX_COUNT) {
            // throw new Error("Maximum lock count exceeded");
            return FALSE;
        }
    }
    if (!CompareAndSetState(c, c + 1)) {
        return FALSE;
    }
    SetExclusiveOwnerThread(current);
    return TRUE;
}

Boolean CReentrantReadWriteLock::Sync::TryReadLock()
{
    AutoPtr<IThread> current = Thread::GetCurrentThread();
    for (;;) {
        Int32 c = GetState();
        if (ExclusiveCount(c) != 0 &&
            GetExclusiveOwnerThread() != current) {
            return FALSE;
        }
        Int32 r = SharedCount(c);
        if (r == MAX_COUNT) {
            // throw new Error("Maximum lock count exceeded");
            return FALSE;
        }
        if (CompareAndSetState(c, c + SHARED_UNIT)) {
            if (r == 0) {
                mFirstReader = current;
                mFirstReaderHoldCount = 1;
            }
            else if (mFirstReader == current) {
                mFirstReaderHoldCount++;
            }
            else {
                AutoPtr<HoldCounter> rh = mCachedHoldCounter;
                Int64 idflag = 0;
                if (rh == NULL || rh->mTid != (current->GetId(&idflag), idflag)) {
                    mCachedHoldCounter = NULL;
                    rh = NULL;
                    // cachedHoldCounter = rh = readHolds.get();
                    assert(0 && "TODO");
                }
                else if (rh->mCount == 0) {
                    // readHolds.set(rh);
                    assert(0 && "TODO");
                }
                rh->mCount++;
            }
            return TRUE;
        }
    }
    return FALSE;
}

AutoPtr<ICondition> CReentrantReadWriteLock::Sync::NewCondition()
{
    AutoPtr<ICondition> res = (ICondition*) new ConditionObject(this);
    return res;
}

AutoPtr<IThread> CReentrantReadWriteLock::Sync::GetOwner()
{
    // Must read state before owner to ensure memory consistency
    return ((ExclusiveCount(GetState()) == 0) ? NULL : GetExclusiveOwnerThread());
}

Int32 CReentrantReadWriteLock::Sync::GetReadLockCount()
{
    return SharedCount(GetState());
}

Boolean CReentrantReadWriteLock::Sync::IsWriteLocked()
{
    return ExclusiveCount(GetState()) != 0;
}

Int32 CReentrantReadWriteLock::Sync::GetWriteHoldCount()
{
    return IsHeldExclusively() ? ExclusiveCount(GetState()) : 0;
}

Int32 CReentrantReadWriteLock::Sync::GetReadHoldCount()
{
    if (GetReadLockCount() == 0)
        return 0;

    AutoPtr<IThread> current = Thread::GetCurrentThread();
    if (mFirstReader == current)
        return mFirstReaderHoldCount;

    AutoPtr<HoldCounter> rh = mCachedHoldCounter;
    Int64 idflag = 0;
    if (rh != NULL && rh->mTid == (current->GetId(&idflag), idflag))
        return rh->mCount;

    assert(0 && "TODO");
    // int count = readHolds.get().count;
    // if (count == 0) readHolds.remove();
    // return count;
    return 0;
}

Int32 CReentrantReadWriteLock::Sync::GetCount()
{
    return GetState();
}

Boolean CReentrantReadWriteLock::Sync::TryRelease(
    /* [in] */ Int32 releases)
{
    if (!IsHeldExclusively()) {
        // throw new IllegalMonitorStateException();
        return FALSE;
    }
    Int32 nextc = GetState() - releases;
    Boolean free = ExclusiveCount(nextc) == 0;
    if (free) {
        SetExclusiveOwnerThread(NULL);
    }
    SetState(nextc);
    return free;
}

Boolean CReentrantReadWriteLock::Sync::TryAcquire(
    /* [in] */ Int32 acquires)
{
    /*
     * Walkthrough:
     * 1. If read count nonzero or write count nonzero
     *    and owner is a different thread, fail.
     * 2. If count would saturate, fail. (This can only
     *    happen if count is already nonzero.)
     * 3. Otherwise, this thread is eligible for lock if
     *    it is either a reentrant acquire or
     *    queue policy allows it. If so, update state
     *    and set owner.
     */
    AutoPtr<IThread> current = Thread::GetCurrentThread();
    Int32 c = GetState();
    Int32 w = ExclusiveCount(c);
    if (c != 0) {
        // (Note: if c != 0 and w == 0 then shared count != 0)
        if (w == 0 || current != GetExclusiveOwnerThread())
            return FALSE;
        if (w + ExclusiveCount(acquires) > MAX_COUNT) {
            // throw new Error("Maximum lock count exceeded");
            return FALSE;
        }
        // Reentrant acquire
        SetState(c + acquires);
        return TRUE;
    }
    if (WriterShouldBlock() ||
        !CompareAndSetState(c, c + acquires))
        return FALSE;
    SetExclusiveOwnerThread(current);
    return TRUE;
}

Boolean CReentrantReadWriteLock::Sync::TryReleaseShared(
    /* [in] */ Int32 unused)
{
    AutoPtr<IThread> current = Thread::GetCurrentThread();
    if (mFirstReader == current) {
        // assert firstReaderHoldCount > 0;
        if (mFirstReaderHoldCount == 1)
            mFirstReader = NULL;
        else
            mFirstReaderHoldCount--;
    }
    else {
        AutoPtr<HoldCounter> rh = mCachedHoldCounter;
        Int64 idflag = 0;
        if (rh == NULL || rh->mTid != (current->GetId(&idflag), idflag)) {
            // rh = readHolds.get();
            assert(0 && "TODO");
        }

        Int32 count = rh->mCount;
        if (count <= 1) {
            // readHolds.remove();
            assert(0 && "TODO");
            if (count <= 0){
                // throw unmatchedUnlockException();
                return FALSE;
            }
        }
        --rh->mCount;
    }
    for (;;) {
        Int32 c = GetState();
        Int32 nextc = c - SHARED_UNIT;
        if (CompareAndSetState(c, nextc))
            // Releasing the read lock has no effect on readers,
            // but it may allow waiting writers to proceed if
            // both read and write locks are now free.
            return nextc == 0;
    }
}

Int32 CReentrantReadWriteLock::Sync::TryAcquireShared(
    /* [in] */ Int32 unused)
{
    /*
     * Walkthrough:
     * 1. If write lock held by another thread, fail.
     * 2. Otherwise, this thread is eligible for
     *    lock wrt state, so ask if it should block
     *    because of queue policy. If not, try
     *    to grant by CASing state and updating count.
     *    Note that step does not check for reentrant
     *    acquires, which is postponed to full version
     *    to avoid having to check hold count in
     *    the more typical non-reentrant case.
     * 3. If step 2 fails either because thread
     *    apparently not eligible or CAS fails or count
     *    saturated, chain to version with full retry loop.
     */
    AutoPtr<IThread> current = Thread::GetCurrentThread();
    Int32 c = GetState();
    if (ExclusiveCount(c) != 0 &&
        GetExclusiveOwnerThread() != current)
        return -1;
    Int32 r = SharedCount(c);
    if (!ReaderShouldBlock() &&
        r < MAX_COUNT &&
        CompareAndSetState(c, c + SHARED_UNIT)) {
        if (r == 0) {
            mFirstReader = current;
            mFirstReaderHoldCount = 1;
        }
        else if (mFirstReader == current) {
            mFirstReaderHoldCount++;
        }
        else {
            AutoPtr<HoldCounter> rh = mCachedHoldCounter;
            Int64 idflag = 0;
            if (rh == NULL || rh->mTid != (current->GetId(&idflag), idflag)) {
                // cachedHoldCounter = rh = readHolds.get();
                assert(0 && "TODO");
            }
            else if (rh->mCount == 0) {
                // readHolds.set(rh);
                assert(0 && "TODO");
            }
            rh->mCount++;
        }
        return 1;
    }
    return FullTryAcquireShared(current);
}

Int32 CReentrantReadWriteLock::Sync::FullTryAcquireShared(
    /* [in] */ IThread* current)
{
    /*
     * This code is in part redundant with that in
     * tryAcquireShared but is simpler overall by not
     * complicating tryAcquireShared with interactions between
     * retries and lazily reading hold counts.
     */
    AutoPtr<HoldCounter> rh;
    for (;;) {
        Int32 c = GetState();
        if (ExclusiveCount(c) != 0) {
            if (GetExclusiveOwnerThread().Get() != current)
                return -1;
            // else we hold the exclusive lock; blocking here
            // would cause deadlock.
        }
        else if (ReaderShouldBlock()) {
            // Make sure we're not acquiring read lock reentrantly
            if (mFirstReader.Get() == current) {
                // assert firstReaderHoldCount > 0;
            }
            else {
                if (rh == NULL) {
                    rh = mCachedHoldCounter;
                    Int64 idflag = 0;
                    if (rh == NULL || rh->mTid != (current->GetId(&idflag), idflag)) {
                        assert(0 && "TODO");
                        // rh = readHolds.get();
                        // if (rh.count == 0)
                        //     readHolds.remove();
                    }
                }
                if (rh->mCount == 0)
                    return -1;
            }
        }
        if (SharedCount(c) == MAX_COUNT) {
            // throw new Error("Maximum lock count exceeded");
            return 0;
        }

        if (CompareAndSetState(c, c + SHARED_UNIT)) {
            if (SharedCount(c) == 0) {
                mFirstReader = current;
                mFirstReaderHoldCount = 1;
            }
            else if (mFirstReader.Get() == current) {
                mFirstReaderHoldCount++;
            }
            else {
                if (rh == NULL)
                    rh = mCachedHoldCounter;
                Int64 idflag = 0;
                if (rh == NULL || rh->mTid != (current->GetId(&idflag), idflag)) {
                    // rh = readHolds.get();
                    assert(0 && "TODO");
                }
                else if (rh->mCount == 0) {
                    // readHolds.set(rh);
                    assert(0 && "TODO");
                }
                rh->mCount++;
                mCachedHoldCounter = rh; // cache for release
            }
            return 1;
        }
    }
}

Boolean CReentrantReadWriteLock::Sync::IsHeldExclusively()
{
    // // While we must in general read state before owner,
    // // we don't need to do so to check if current thread is owner
    return GetExclusiveOwnerThread() == Thread::GetCurrentThread();
}

void CReentrantReadWriteLock::Sync::ReadObject(
    /* [in] */ IObjectInputStream* s)
{
    s->DefaultReadObject();
    mReadHolds = new ThreadLocalHoldCounter();
    SetState(0); // reset to unlocked state
}


//==========================================================
//       CReentrantReadWriteLock::CReadLock
//==========================================================
CAR_INTERFACE_IMPL_2(CReentrantReadWriteLock::CReadLock, Object, ILock, ISerializable)

CReentrantReadWriteLock::CReadLock::CReadLock(
    /* [in] */ CReentrantReadWriteLock* lock)
{
    mSync = lock->mSync;
}

ECode CReentrantReadWriteLock::CReadLock::Lock()
{
    return mSync->AcquireShared(1);
}

ECode CReentrantReadWriteLock::CReadLock::LockInterruptibly()
{
    return mSync->AcquireSharedInterruptibly(1);
}

ECode CReentrantReadWriteLock::CReadLock::TryLock(
    /* [out] */ Boolean* value)
{
    return mSync->TryReadLock();
}

ECode CReentrantReadWriteLock::CReadLock::TryLock(
  /* [in] */ Int64 time,
  /* [in] */ ITimeUnit* unit,
  /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    Int64 result = 0;
    unit->ToNanos(time, &result);
    return mSync->TryAcquireSharedNanos(1, result, value);
}

ECode CReentrantReadWriteLock::CReadLock::UnLock()
{
    Boolean b = FALSE;
    return mSync->ReleaseShared(1, &b);
}

ECode CReentrantReadWriteLock::CReadLock::NewCondition(
    /* [out] */ ICondition** cond)
{
    // throw new UnsupportedOperationException();
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CReentrantReadWriteLock::CReadLock::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    Int32 r = mSync->GetReadLockCount();
    // return super.toString() + "[Read locks = " + r + "]";
    *str = String("[Read locks = ") + StringUtils::ToString(r) + String("]");
    return NOERROR;
}


//==========================================================
//       CReentrantReadWriteLock::WriteLock
//==========================================================
CAR_INTERFACE_IMPL_2(CReentrantReadWriteLock::CWriteLock, Object, ILock, ISerializable)

CReentrantReadWriteLock::CWriteLock::CWriteLock(
    /* [in] */ CReentrantReadWriteLock* lock)
{
    mSync = lock->mSync;
}

ECode CReentrantReadWriteLock::CWriteLock::Lock()
{
    return mSync->AcquireIt(1);
}

ECode CReentrantReadWriteLock::CWriteLock::LockInterruptibly()
{
    return mSync->AcquireInterruptibly(1);
}

ECode CReentrantReadWriteLock::CWriteLock::TryLock(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mSync->TryWriteLock();
    return NOERROR;
}

ECode CReentrantReadWriteLock::CWriteLock::TryLock(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    Int64 result = 0;
    unit->ToNanos(timeout, &result);
    return mSync->TryAcquireNanos(1, result, value);
}

ECode CReentrantReadWriteLock::CWriteLock::UnLock()
{
    Boolean flag = FALSE;
    return mSync->ReleaseIt(1, &flag);
}

ECode CReentrantReadWriteLock::CWriteLock::NewCondition(
    /* [out] */ ICondition** con)
{
    VALIDATE_NOT_NULL(con)

    AutoPtr<ICondition> res = mSync->NewCondition();
    *con = res;
    REFCOUNT_ADD(*con)
    return NOERROR;
}

ECode CReentrantReadWriteLock::CWriteLock::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)

    AutoPtr<IThread> o = mSync->GetOwner();
    // return super.toString() + ((o == null) ?
    //                            "[Unlocked]" :
    //                            "[Locked by thread " + o.getName() + "]");
    if (o == NULL) {
        *str = "[Unlocked]";
    }
    else {
        String namestr;
        o->GetName(&namestr);
        *str = String("[Locked by thread ") + namestr + String("]");
    }
    return NOERROR;
}

ECode CReentrantReadWriteLock::CWriteLock::IsHeldByCurrentThread(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mSync->IsHeldExclusively();
    return NOERROR;
}

ECode CReentrantReadWriteLock::CWriteLock::GetHoldCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mSync->GetWriteHoldCount();
    return NOERROR;
}

} // namespace Locks
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
