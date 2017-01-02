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

#include "CForkJoinPool.h"
#include "ForkJoinWorkerThread.h"
#include "LockSupport.h"
#include "ForkJoinTask.h"
#include "TimeUnit.h"
#include "Arrays.h"
#include "CReentrantLock.h"
#include "CArrayList.h"
#include "Collections.h"
#include "AutoLock.h"
#include "CSystem.h"
#include "StringUtils.h"
#include "CountedCompleter.h"
#include "CThreadLocalRandom.h"
#include "CLibcore.h"
#include <elastos/droid/system/OsConstants.h>
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::ThreadState;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Collections;
using Elastos::Utility::Concurrent::ForkJoinWorkerThread;
using Elastos::Utility::Concurrent::ForkJoinTask;
using Elastos::Utility::Concurrent::TimeUnit;
using Elastos::Utility::Concurrent::AdaptedCallable;
using Elastos::Utility::Concurrent::AdaptedRunnable;
using Elastos::Utility::Concurrent::Locks::LockSupport;
using Elastos::Utility::Concurrent::Locks::CReentrantLock;
using Elastos::Utility::Concurrent::Locks::ILock;
using Elastos::Droid::System::OsConstants;
using Libcore::IO::IOs;
using Libcore::IO::ILibcore;
using Libcore::IO::CLibcore;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CForkJoinPool::DefaultForkJoinWorkerThreadFactory::
//====================================================================
CAR_INTERFACE_IMPL(CForkJoinPool::DefaultForkJoinWorkerThreadFactory, Object, IForkJoinPoolForkJoinWorkerThreadFactory)

AutoPtr<IForkJoinWorkerThread> CForkJoinPool::DefaultForkJoinWorkerThreadFactory::NewThread(
    /* [in] */ IForkJoinPool* pool)
{
    AutoPtr<IForkJoinWorkerThread> p = new ForkJoinWorkerThread(pool);
   return p;
}

//====================================================================
// CForkJoinPool::EmptyTask::
//====================================================================

CForkJoinPool::EmptyTask::EmptyTask()
{
    mStatus = ForkJoinTask::NORMAL;
} // force done

ECode CForkJoinPool::EmptyTask::SetRawResult(
    /* [in] */ IInterface* value)
{
    return NOERROR;
}

ECode CForkJoinPool::EmptyTask::Exec(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = TRUE;
    return NOERROR;
}

ECode CForkJoinPool::EmptyTask::GetRawResult(
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)

    return NOERROR;
}

//====================================================================
// CForkJoinPool::WorkQueue::
//====================================================================

Int32 CForkJoinPool::WorkQueue::INITIAL_QUEUE_CAPACITY = 1 << 13;

Int32 CForkJoinPool::WorkQueue::MAXIMUM_QUEUE_CAPACITY = 1 << 26; // 64M

CForkJoinPool::WorkQueue::WorkQueue(
    /* [in] */ IForkJoinPool* pool,
    /* [in] */ IForkJoinWorkerThread* owner,
    /* [in] */ Int32 mode,
    /* [in] */ Int32 seed)
{
    mPool = pool;
    mOwner = owner;
    mMode = (Int16)mode;
    mHint = seed; // store initial seed for runWorker
    // Place indices in the center of array (that is not yet allocated)
    mBase = mTop = INITIAL_QUEUE_CAPACITY >> 1;
}

Int32 CForkJoinPool::WorkQueue::QueueSize()
{
    Int32 n = mBase - mTop;       // non-owner callers must read base first
    return (n >= 0) ? 0 : -n; // ignore transient negative
}

Boolean CForkJoinPool::WorkQueue::IsEmpty()
{
    assert(0 && "TODO");
    return FALSE;
    // ForkJoinTask<?>[] a; int m, s;
    // int n = base - (s = top);
    // return (n >= 0 ||
    //         (n == -1 &&
    //          ((a = array) == null ||
    //           (m = a.length - 1) < 0 ||
    //           U.getObject
    //           (a, (long)((m & (s - 1)) << ASHIFT) + ABASE) == null)));
}

void CForkJoinPool::WorkQueue::Push(
    /* [in] */ IForkJoinTask* task)
{
    assert(0 && "TODO");
    // ForkJoinTask<?>[] a; ForkJoinPool p;
    // int s = top, n;
    // if ((a = array) != null) {    // ignore if queue removed
    //     int m = a.length - 1;
    //     U.putOrderedObject(a, ((m & s) << ASHIFT) + ABASE, task);
    //     if ((n = (top = s + 1) - base) <= 2)
    //         (p = pool).signalWork(p.workQueues, this);
    //     else if (n >= m)
    //         growArray();
    // }
}

AutoPtr<ArrayOf<IForkJoinTask*> > CForkJoinPool::WorkQueue::GrowArray()
{
    assert(0 && "TODO");
    return NULL;
    // ForkJoinTask<?>[] oldA = array;
    // int size = oldA != null ? oldA.length << 1 : INITIAL_QUEUE_CAPACITY;
    // if (size > MAXIMUM_QUEUE_CAPACITY)
    //     throw new RejectedExecutionException("Queue capacity exceeded");
    // int oldMask, t, b;
    // ForkJoinTask<?>[] a = array = new ForkJoinTask<?>[size];
    // if (oldA != null && (oldMask = oldA.length - 1) >= 0 &&
    //     (t = top) - (b = base) > 0) {
    //     int mask = size - 1;
    //     do {
    //         ForkJoinTask<?> x;
    //         int oldj = ((b & oldMask) << ASHIFT) + ABASE;
    //         int j    = ((b &    mask) << ASHIFT) + ABASE;
    //         x = (ForkJoinTask<?>)U.getObjectVolatile(oldA, oldj);
    //         if (x != null &&
    //             U.compareAndSwapObject(oldA, oldj, x, null))
    //             U.putObjectVolatile(a, j, x);
    //     } while (++b != t);
    // }
    // return a;
}

AutoPtr<IForkJoinTask> CForkJoinPool::WorkQueue::Pop()
{
    assert(0 && "TODO");
    // ForkJoinTask<?>[] a; ForkJoinTask<?> t; int m;
    // if ((a = array) != null && (m = a.length - 1) >= 0) {
    //     for (int s; (s = top - 1) - base >= 0;) {
    //         long j = ((m & s) << ASHIFT) + ABASE;
    //         if ((t = (ForkJoinTask<?>)U.getObject(a, j)) == null)
    //             break;
    //         if (U.compareAndSwapObject(a, j, t, null)) {
    //             top = s;
    //             return t;
    //         }
    //     }
    // }
    return NULL;
}

AutoPtr<IForkJoinTask> CForkJoinPool::WorkQueue::PollAt(
    /* [in] */ Int32 b)
{
    assert(0 && "TODO");
    // ForkJoinTask<?> t; ForkJoinTask<?>[] a;
    // if ((a = array) != null) {
    //     int j = (((a.length - 1) & b) << ASHIFT) + ABASE;
    //     if ((t = (ForkJoinTask<?>)U.getObjectVolatile(a, j)) != null &&
    //         base == b && U.compareAndSwapObject(a, j, t, null)) {
    //         U.putOrderedInt(this, QBASE, b + 1);
    //         return t;
    //     }
    // }
    return NULL;
}

AutoPtr<IForkJoinTask> CForkJoinPool::WorkQueue::Poll()
{
    assert(0 && "TODO");
    // ForkJoinTask<?>[] a; int b; ForkJoinTask<?> t;
    // while ((b = base) - top < 0 && (a = array) != null) {
    //     int j = (((a.length - 1) & b) << ASHIFT) + ABASE;
    //     t = (ForkJoinTask<?>)U.getObjectVolatile(a, j);
    //     if (t != null) {
    //         if (U.compareAndSwapObject(a, j, t, null)) {
    //             U.putOrderedInt(this, QBASE, b + 1);
    //             return t;
    //         }
    //     }
    //     else if (base == b) {
    //         if (b + 1 == top)
    //             break;
    //         Thread.yield(); // wait for lagging update (very rare)
    //     }
    // }
    return NULL;
}

AutoPtr<IForkJoinTask> CForkJoinPool::WorkQueue::NextLocalTask()
{
    AutoPtr<IForkJoinTask> res = mMode == 0 ? Pop() : Poll();
    return res;
}

AutoPtr<IForkJoinTask> CForkJoinPool::WorkQueue::Peek()
{
    assert(0 && "TODO");
    return NULL;
    // ForkJoinTask<?>[] a = array; int m;
    // if (a == null || (m = a.length - 1) < 0)
    //     return null;
    // int i = mode == 0 ? top - 1 : base;
    // int j = ((i & m) << ASHIFT) + ABASE;
    // return (ForkJoinTask<?>)U.getObjectVolatile(a, j);
}

Boolean CForkJoinPool::WorkQueue::TryUnpush(
    /* [in] */ IForkJoinTask* t)
{
    assert(0 && "TODO");
    // ForkJoinTask<?>[] a; int s;
    // if ((a = array) != null && (s = top) != base &&
    //     U.compareAndSwapObject
    //     (a, (((a.length - 1) & --s) << ASHIFT) + ABASE, t, null)) {
    //     top = s;
    //     return true;
    // }
    return FALSE;
}

void CForkJoinPool::WorkQueue::CancelAll()
{
    ForkJoinTask::CancelIgnoringExceptions((ForkJoinTask*)mCurrentJoin.Get());
    ForkJoinTask::CancelIgnoringExceptions((ForkJoinTask*)mCurrentSteal.Get());
    for (AutoPtr<IForkJoinTask> t; (t = Poll()) != NULL; )
        ForkJoinTask::CancelIgnoringExceptions((ForkJoinTask*)t.Get());
}

void CForkJoinPool::WorkQueue::PollAndExecAll()
{
    for (AutoPtr<IForkJoinTask> t; (t = Poll()) != NULL;) {
        Int32 res;
        t->DoExec(&res);
    }
}

void CForkJoinPool::WorkQueue::RunTask(
    /* [in] */ IForkJoinTask* task)
{
    assert(0 && "TODO");
    // if ((currentSteal = task) != null) {
    //     task.doExec();
    //     ForkJoinTask<?>[] a = array;
    //     int md = mode;
    //     ++nsteals;
    //     currentSteal = null;
    //     if (md != 0)
    //         pollAndExecAll();
    //     else if (a != null) {
    //         int s, m = a.length - 1;
    //         while ((s = top - 1) - base >= 0) {
    //             long i = ((m & s) << ASHIFT) + ABASE;
    //             ForkJoinTask<?> t = (ForkJoinTask<?>)U.getObject(a, i);
    //             if (t == null)
    //                 break;
    //             if (U.compareAndSwapObject(a, i, t, null)) {
    //                 top = s;
    //                 t.doExec();
    //             }
    //         }
    //     }
    // }
}

Boolean CForkJoinPool::WorkQueue::TryRemoveAndExec(
    /* [in] */ IForkJoinTask* task)
{
    assert(0 && "TODO");
    return FALSE;
    // boolean stat;
    // ForkJoinTask<?>[] a; int m, s, b, n;
    // if (task != null && (a = array) != null && (m = a.length - 1) >= 0 &&
    //     (n = (s = top) - (b = base)) > 0) {
    //     boolean removed = false, empty = true;
    //     stat = true;
    //     for (ForkJoinTask<?> t;;) {           // traverse from s to b
    //         long j = ((--s & m) << ASHIFT) + ABASE;
    //         t = (ForkJoinTask<?>)U.getObject(a, j);
    //         if (t == null)                    // inconsistent length
    //             break;
    //         else if (t == task) {
    //             if (s + 1 == top) {           // pop
    //                 if (!U.compareAndSwapObject(a, j, task, null))
    //                     break;
    //                 top = s;
    //                 removed = true;
    //             }
    //             else if (base == b)           // replace with proxy
    //                 removed = U.compareAndSwapObject(a, j, task,
    //                                                  new EmptyTask());
    //             break;
    //         }
    //         else if (t.status >= 0)
    //             empty = false;
    //         else if (s + 1 == top) {          // pop and throw away
    //             if (U.compareAndSwapObject(a, j, t, null))
    //                 top = s;
    //             break;
    //         }
    //         if (--n == 0) {
    //             if (!empty && base == b)
    //                 stat = false;
    //             break;
    //         }
    //     }
    //     if (removed)
    //         task.doExec();
    // }
    // else
    //     stat = false;
    // return stat;
}

Boolean CForkJoinPool::WorkQueue::PollAndExecCC(
    /* [in] */ ICountedCompleter* root)
{
    assert(0 && "TODO");
    // ForkJoinTask<?>[] a; int b; Object o; CountedCompleter<?> t, r;
    // if ((b = base) - top < 0 && (a = array) != null) {
    //     long j = (((a.length - 1) & b) << ASHIFT) + ABASE;
    //     if ((o = U.getObjectVolatile(a, j)) == null)
    //         return true; // retry
    //     if (o instanceof CountedCompleter) {
    //         for (t = (CountedCompleter<?>)o, r = t;;) {
    //             if (r == root) {
    //                 if (base == b &&
    //                     U.compareAndSwapObject(a, j, t, null)) {
    //                     U.putOrderedInt(this, QBASE, b + 1);
    //                     t.doExec();
    //                 }
    //                 return true;
    //             }
    //             else if ((r = r.completer) == null)
    //                 break; // not part of root computation
    //         }
    //     }
    // }
    return FALSE;
}

Boolean CForkJoinPool::WorkQueue::ExternalPopAndExecCC(
    /* [in] */ ICountedCompleter* root)
{
    assert(0 && "TODO");
    // ForkJoinTask<?>[] a; int s; Object o; CountedCompleter<?> t, r;
    // if (base - (s = top) < 0 && (a = array) != null) {
    //     long j = (((a.length - 1) & (s - 1)) << ASHIFT) + ABASE;
    //     if ((o = U.getObject(a, j)) instanceof CountedCompleter) {
    //         for (t = (CountedCompleter<?>)o, r = t;;) {
    //             if (r == root) {
    //                 if (U.compareAndSwapInt(this, QLOCK, 0, 1)) {
    //                     if (top == s && array == a &&
    //                         U.compareAndSwapObject(a, j, t, null)) {
    //                         top = s - 1;
    //                         qlock = 0;
    //                         t.doExec();
    //                     }
    //                     else
    //                         qlock = 0;
    //                 }
    //                 return true;
    //             }
    //             else if ((r = r.completer) == null)
    //                 break;
    //         }
    //     }
    // }
    return FALSE;
}

Boolean CForkJoinPool::WorkQueue::InternalPopAndExecCC(
    /* [in] */ ICountedCompleter* root)
{
    assert(0 && "TODO");
    // ForkJoinTask<?>[] a; int s; Object o; CountedCompleter<?> t, r;
    // if (base - (s = top) < 0 && (a = array) != null) {
    //     long j = (((a.length - 1) & (s - 1)) << ASHIFT) + ABASE;
    //     if ((o = U.getObject(a, j)) instanceof CountedCompleter) {
    //         for (t = (CountedCompleter<?>)o, r = t;;) {
    //             if (r == root) {
    //                 if (U.compareAndSwapObject(a, j, t, null)) {
    //                     top = s - 1;
    //                     t.doExec();
    //                 }
    //                 return true;
    //             }
    //             else if ((r = r.completer) == null)
    //                 break;
    //         }
    //     }
    // }
    return FALSE;
}

Boolean CForkJoinPool::WorkQueue::IsApparentlyUnblocked()
{
    AutoPtr<IThread> wt; ThreadState s;
    return (mEventCount >= 0 &&
            (wt = IThread::Probe(mOwner)) != NULL &&
            (wt->GetState(&s), s) != Elastos::Core::ThreadState_BLOCKED &&
            s != Elastos::Core::ThreadState_WAITING &&
            s != Elastos::Core::ThreadState_TIMED_WAITING);
}

//====================================================================
// CForkJoinPool::
//====================================================================
CAR_INTERFACE_IMPL(CForkJoinPool, AbstractExecutorService, IForkJoinPool)

CAR_OBJECT_IMPL(CForkJoinPool)

AutoPtr<IForkJoinPoolForkJoinWorkerThreadFactory> CForkJoinPool::mDefaultForkJoinWorkerThreadFactory;

//AutoPtr<IThreadLocal> CForkJoinPool::mSubmitters;

//AutoPtr<IRuntimePermission> CForkJoinPool::mModifyThreadPermission;

AutoPtr<IForkJoinPool> CForkJoinPool::mCommon;

Int32 CForkJoinPool::mCommonParallelism;

Int32 CForkJoinPool::mPoolNumberSequence;

Int64 CForkJoinPool::IDLE_TIMEOUT      = 2000L * 1000L * 1000L; // 2sec

Int64 CForkJoinPool::FAST_IDLE_TIMEOUT =  200L * 1000L * 1000L;

Int64 CForkJoinPool::TIMEOUT_SLOP = 2000000L;

Int32 CForkJoinPool::MAX_HELP = 64;

Int32 CForkJoinPool::SEED_INCREMENT = 0x61c88647;

// bit positions/shifts for fields
Int32 CForkJoinPool::AC_SHIFT   = 48;
Int32 CForkJoinPool::TC_SHIFT   = 32;
Int32 CForkJoinPool::ST_SHIFT   = 31;
Int32 CForkJoinPool::EC_SHIFT   = 16;

// bounds
Int32 CForkJoinPool::SMASK      = 0xffff;  // short bits
Int32 CForkJoinPool::MAX_CAP    = 0x7fff;  // max #workers - 1
Int32 CForkJoinPool::EVENMASK   = 0xfffe;  // even short bits
Int32 CForkJoinPool::SQMASK     = 0x007e;  // max 64 (even) slots
Int32 CForkJoinPool::SHORT_SIGN = 1 << 15;
Int32 CForkJoinPool::INT_SIGN   = 1 << 31;

// masks
Int64 CForkJoinPool::STOP_BIT   = 0x0001L << ST_SHIFT;
Int64 CForkJoinPool::AC_MASK    = ((Int64)SMASK) << AC_SHIFT;
Int64 CForkJoinPool::TC_MASK    = ((Int64)SMASK) << TC_SHIFT;

// units for incrementing and decrementing
Int64 CForkJoinPool::TC_UNIT    = 1L << TC_SHIFT;
Int64 CForkJoinPool::AC_UNIT    = 1L << AC_SHIFT;

// masks and units for dealing with u = (int)(ctl >>> 32)
Int32 CForkJoinPool::UAC_SHIFT  = AC_SHIFT - 32;
Int32 CForkJoinPool::UTC_SHIFT  = TC_SHIFT - 32;
Int32 CForkJoinPool::UAC_MASK   = SMASK << UAC_SHIFT;
Int32 CForkJoinPool::UTC_MASK   = SMASK << UTC_SHIFT;
Int32 CForkJoinPool::UAC_UNIT   = 1 << UAC_SHIFT;
Int32 CForkJoinPool::UTC_UNIT   = 1 << UTC_SHIFT;

// masks and units for dealing with e = (int)ctl
Int32 CForkJoinPool::E_MASK     = 0x7fffffff; // no STOP_BIT
Int32 CForkJoinPool::E_SEQ    = 1 << EC_SHIFT;

Int32 CForkJoinPool::SHUTDOWN    = 1 << 31;
Int32 CForkJoinPool::PL_LOCK     = 2;
Int32 CForkJoinPool::PL_SIGNAL   = 1;
Int32 CForkJoinPool::PL_SPINS    = 1 << 8;

Int32 CForkJoinPool::LIFO_QUEUE          =  0;
Int32 CForkJoinPool::FIFO_QUEUE          =  1;
Int32 CForkJoinPool::SHARED_QUEUE        = -1;

//synchronized
Int32 CForkJoinPool::NextPoolId()
{
    return ++mPoolNumberSequence;
}

Int32 CForkJoinPool::AcquirePlock()
{
    assert(0 && "TODO");
    return 0;
    // int spins = PL_SPINS, ps, nps;
    // for (;;) {
    //     if (((ps = plock) & PL_LOCK) == 0 &&
    //         U.compareAndSwapInt(this, PLOCK, ps, nps = ps + PL_LOCK))
    //         return nps;
    //     else if (spins >= 0) {
    //         if (ThreadLocalRandom.current().nextInt() >= 0)
    //             --spins;
    //     }
    //     else if (U.compareAndSwapInt(this, PLOCK, ps, ps | PL_SIGNAL)) {
    //         {    AutoLock syncLock(this);
    //             if ((plock & PL_SIGNAL) != 0) {
    //                 try {
    //                     wait();
    //                 } catch (InterruptedException ie) {
    //                     try {
    //                         Thread.currentThread().interrupt();
    //                     } catch (SecurityException ignore) {
    //                     }
    //                 }
    //             }
    //             else
    //                 notifyAll();
    //         }
    //     }
    // }
}

void CForkJoinPool::ReleasePlock(
    /* [in] */ Int32 ps)
{
    mPlock = ps;
    AutoLock syncLock(this);
    NotifyAll();
}

void CForkJoinPool::TryAddWorker()
{
    assert(0 && "TODO");
    // long c; int u, e;
    // while ((u = (int)((c = ctl) >>> 32)) < 0 &&
    //        (u & SHORT_SIGN) != 0 && (e = (int)c) >= 0) {
    //     long nc = ((long)(((u + UTC_UNIT) & UTC_MASK) |
    //                       ((u + UAC_UNIT) & UAC_MASK)) << 32) | (long)e;
    //     if (U.compareAndSwapLong(this, CTL, c, nc)) {
    //         ForkJoinWorkerThreadFactory fac;
    //         Throwable ex = null;
    //         ForkJoinWorkerThread wt = null;
    //         try {
    //             if ((fac = factory) != null &&
    //                 (wt = fac.newThread(this)) != null) {
    //                 wt.start();
    //                 break;
    //             }
    //         } catch (Throwable rex) {
    //             ex = rex;
    //         }
    //         deregisterWorker(wt, ex);
    //         break;
    //     }
    // }
}

AutoPtr<CForkJoinPool::WorkQueue> CForkJoinPool::RegisterWorker(
    /* [in] */ IForkJoinWorkerThread* wt)
{
    assert(0 && "TODO");
    return NULL;
    // IThreadUncaughtExceptionHandler handler; WorkQueue[] ws; int s, ps;
    // wt.setDaemon(true);
    // if ((handler = ueh) != null)
    //     wt.setUncaughtExceptionHandler(handler);
    // do {} while (!U.compareAndSwapInt(this, INDEXSEED, s = indexSeed,
    //                                   s += SEED_INCREMENT) ||
    //              s == 0); // skip 0
    // WorkQueue w = new WorkQueue(this, wt, mode, s);
    // if (((ps = plock) & PL_LOCK) != 0 ||
    //     !U.compareAndSwapInt(this, PLOCK, ps, ps += PL_LOCK))
    //     ps = acquirePlock();
    // int nps = (ps & SHUTDOWN) | ((ps + PL_LOCK) & ~SHUTDOWN);
    // try {
    //     if ((ws = workQueues) != null) {    // skip if shutting down
    //         int n = ws.length, m = n - 1;
    //         int r = (s << 1) | 1;           // use odd-numbered indices
    //         if (ws[r &= m] != null) {       // collision
    //             int probes = 0;             // step by approx half size
    //             int step = (n <= 4) ? 2 : ((n >>> 1) & EVENMASK) + 2;
    //             while (ws[r = (r + step) & m] != null) {
    //                 if (++probes >= n) {
    //                     workQueues = ws = Arrays.copyOf(ws, n <<= 1);
    //                     m = n - 1;
    //                     probes = 0;
    //                 }
    //             }
    //         }
    //         w.poolIndex = (short)r;
    //         w.eventCount = r; // volatile write orders
    //         ws[r] = w;
    //     }
    // } finally {
    //     if (!U.compareAndSwapInt(this, PLOCK, ps, nps))
    //         releasePlock(nps);
    // }
    // wt.setName(workerNamePrefix.concat(Integer.toString(w.poolIndex >>> 1)));
    // return w;
}

void CForkJoinPool::DeregisterWorker(
    /* [in] */ IForkJoinWorkerThread* w,
    /* [in] */ IThrowable* ex)
{
    assert(0 && "TODO");
    // WorkQueue w = null;
    // if (wt != null && (w = wt.workQueue) != null) {
    //     int ps; long sc;
    //     w.qlock = -1;                // ensure set
    //     do {} while (!U.compareAndSwapLong(this, STEALCOUNT,
    //                                        sc = stealCount,
    //                                        sc + w.nsteals));
    //     if (((ps = plock) & PL_LOCK) != 0 ||
    //         !U.compareAndSwapInt(this, PLOCK, ps, ps += PL_LOCK))
    //         ps = acquirePlock();
    //     int nps = (ps & SHUTDOWN) | ((ps + PL_LOCK) & ~SHUTDOWN);
    //     try {
    //         int idx = w.poolIndex;
    //         WorkQueue[] ws = workQueues;
    //         if (ws != null && idx >= 0 && idx < ws.length && ws[idx] == w)
    //             ws[idx] = null;
    //     } finally {
    //         if (!U.compareAndSwapInt(this, PLOCK, ps, nps))
    //             releasePlock(nps);
    //     }
    // }

    // long c;                          // adjust ctl counts
    // do {} while (!U.compareAndSwapLong
    //              (this, CTL, c = ctl, (((c - AC_UNIT) & AC_MASK) |
    //                                    ((c - TC_UNIT) & TC_MASK) |
    //                                    (c & ~(AC_MASK|TC_MASK)))));

    // if (!tryTerminate(false, false) && w != null && w.array != null) {
    //     w.cancelAll();               // cancel remaining tasks
    //     WorkQueue[] ws; WorkQueue v; Thread p; int u, i, e;
    //     while ((u = (int)((c = ctl) >>> 32)) < 0 && (e = (int)c) >= 0) {
    //         if (e > 0) {             // activate or create replacement
    //             if ((ws = workQueues) == null ||
    //                 (i = e & SMASK) >= ws.length ||
    //                 (v = ws[i]) == null)
    //                 break;
    //             long nc = (((long)(v.nextWait & E_MASK)) |
    //                        ((long)(u + UAC_UNIT) << 32));
    //             if (v.eventCount != (e | INT_SIGN))
    //                 break;
    //             if (U.compareAndSwapLong(this, CTL, c, nc)) {
    //                 v.eventCount = (e + E_SEQ) & E_MASK;
    //                 if ((p = v.parker) != null)
    //                     U.unpark(p);
    //                 break;
    //             }
    //         }
    //         else {
    //             if ((short)u < 0)
    //                 tryAddWorker();
    //             break;
    //         }
    //     }
    // }
    // if (ex == null)                     // help clean refs on way out
    //     ForkJoinTask.helpExpungeStaleExceptions();
    // else                                // rethrow
    //     ForkJoinTask.rethrow(ex);
}

void CForkJoinPool::ExternalPush(
    /* [in] */ IForkJoinTask* task)
{
    assert(0 && "TODO");
    // Submitter z = submitters.get();
    // WorkQueue q; int r, m, s, n, am; ForkJoinTask<?>[] a;
    // int ps = plock;
    // WorkQueue[] ws = workQueues;
    // if (z != null && ps > 0 && ws != null && (m = (ws.length - 1)) >= 0 &&
    //     (q = ws[m & (r = z.seed) & SQMASK]) != null && r != 0 &&
    //     U.compareAndSwapInt(q, QLOCK, 0, 1)) { // lock
    //     if ((a = q.array) != null &&
    //         (am = a.length - 1) > (n = (s = q.top) - q.base)) {
    //         int j = ((am & s) << ASHIFT) + ABASE;
    //         U.putOrderedObject(a, j, task);
    //         q.top = s + 1;                     // push on to deque
    //         q.qlock = 0;
    //         if (n <= 1)
    //             signalWork(ws, q);
    //         return;
    //     }
    //     q.qlock = 0;
    // }
    // fullExternalPush(task);
}

void CForkJoinPool::FullExternalPush(
    /* [in] */ IForkJoinTask* task)
{
    assert(0 && "TODO");
    // int r = 0; // random index seed
    // for (Submitter z = submitters.get();;) {
    //     WorkQueue[] ws; WorkQueue q; int ps, m, k;
    //     if (z == null) {
    //         if (U.compareAndSwapInt(this, INDEXSEED, r = indexSeed,
    //                                 r += SEED_INCREMENT) && r != 0)
    //             submitters.set(z = new Submitter(r));
    //     }
    //     else if (r == 0) {                  // move to a different index
    //         r = z.seed;
    //         r ^= r << 13;                   // same xorshift as WorkQueues
    //         r ^= r >>> 17;
    //         z.seed = r ^= (r << 5);
    //     }
    //     if ((ps = plock) < 0)
    //         throw new RejectedExecutionException();
    //     else if (ps == 0 || (ws = workQueues) == null ||
    //              (m = ws.length - 1) < 0) { // initialize workQueues
    //         int p = parallelism;            // find power of two table size
    //         int n = (p > 1) ? p - 1 : 1;    // ensure at least 2 slots
    //         n |= n >>> 1; n |= n >>> 2;  n |= n >>> 4;
    //         n |= n >>> 8; n |= n >>> 16; n = (n + 1) << 1;
    //         WorkQueue[] nws = ((ws = workQueues) == null || ws.length == 0 ?
    //                            new WorkQueue[n] : null);
    //         if (((ps = plock) & PL_LOCK) != 0 ||
    //             !U.compareAndSwapInt(this, PLOCK, ps, ps += PL_LOCK))
    //             ps = acquirePlock();
    //         if (((ws = workQueues) == null || ws.length == 0) && nws != null)
    //             workQueues = nws;
    //         int nps = (ps & SHUTDOWN) | ((ps + PL_LOCK) & ~SHUTDOWN);
    //         if (!U.compareAndSwapInt(this, PLOCK, ps, nps))
    //             releasePlock(nps);
    //     }
    //     else if ((q = ws[k = r & m & SQMASK]) != null) {
    //         if (q.qlock == 0 && U.compareAndSwapInt(q, QLOCK, 0, 1)) {
    //             ForkJoinTask<?>[] a = q.array;
    //             int s = q.top;
    //             boolean submitted = false;
    //             try {                      // locked version of push
    //                 if ((a != null && a.length > s + 1 - q.base) ||
    //                     (a = q.growArray()) != null) {   // must presize
    //                     int j = (((a.length - 1) & s) << ASHIFT) + ABASE;
    //                     U.putOrderedObject(a, j, task);
    //                     q.top = s + 1;
    //                     submitted = true;
    //                 }
    //             } finally {
    //                 q.qlock = 0;  // unlock
    //             }
    //             if (submitted) {
    //                 signalWork(ws, q);
    //                 return;
    //             }
    //         }
    //         r = 0; // move on failure
    //     }
    //     else if (((ps = plock) & PL_LOCK) == 0) { // create new queue
    //         q = new WorkQueue(this, null, SHARED_QUEUE, r);
    //         q.poolIndex = (short)k;
    //         if (((ps = plock) & PL_LOCK) != 0 ||
    //             !U.compareAndSwapInt(this, PLOCK, ps, ps += PL_LOCK))
    //             ps = acquirePlock();
    //         if ((ws = workQueues) != null && k < ws.length && ws[k] == null)
    //             ws[k] = q;
    //         int nps = (ps & SHUTDOWN) | ((ps + PL_LOCK) & ~SHUTDOWN);
    //         if (!U.compareAndSwapInt(this, PLOCK, ps, nps))
    //             releasePlock(nps);
    //     }
    //     else
    //         r = 0;
    // }
}

void CForkJoinPool::IncrementActiveCount()
{
    assert(0 && "TODO");
    // long c;
    // do {} while (!U.compareAndSwapLong
    //              (this, CTL, c = ctl, ((c & ~AC_MASK) |
    //                                    ((c & AC_MASK) + AC_UNIT))));
}

void CForkJoinPool::SignalWork(
    /* [in] */ ArrayOf<WorkQueue*>* ws,
    /* [in] */ WorkQueue* q)
{
    assert(0 && "TODO");
    // for (;;) {
    //     long c; int e, u, i; WorkQueue w; Thread p;
    //     if ((u = (int)((c = ctl) >>> 32)) >= 0)
    //         break;
    //     if ((e = (int)c) <= 0) {
    //         if ((short)u < 0)
    //             tryAddWorker();
    //         break;
    //     }
    //     if (ws == null || ws.length <= (i = e & SMASK) ||
    //         (w = ws[i]) == null)
    //         break;
    //     long nc = (((long)(w.nextWait & E_MASK)) |
    //                ((long)(u + UAC_UNIT)) << 32);
    //     int ne = (e + E_SEQ) & E_MASK;
    //     if (w.eventCount == (e | INT_SIGN) &&
    //         U.compareAndSwapLong(this, CTL, c, nc)) {
    //         w.eventCount = ne;
    //         if ((p = w.parker) != null)
    //             U.unpark(p);
    //         break;
    //     }
    //     if (q != null && q.base >= q.top)
    //         break;
    // }
}

void CForkJoinPool::RunWorker(
    /* [in] */ WorkQueue* w)
{
    w->GrowArray(); // allocate queue
    for (Int32 r = w->mHint; Scan(w, r) == 0; ) {
        r ^= r << 13; r ^= r >> 17; r ^= r << 5; // xorshift
    }
}

Int32 CForkJoinPool::Scan(
    /* [in] */ WorkQueue* w,
    /* [in] */ Int32 r)
{
    assert(0 && "TODO");
    // WorkQueue[] ws; int m;
    // long c = ctl;                            // for consistency check
    // if ((ws = workQueues) != null && (m = ws.length - 1) >= 0 && w != null) {
    //     for (int j = m + m + 1, ec = w.eventCount;;) {
    //         WorkQueue q; int b, e; ForkJoinTask<?>[] a; ForkJoinTask<?> t;
    //         if ((q = ws[(r - j) & m]) != null &&
    //             (b = q.base) - q.top < 0 && (a = q.array) != null) {
    //             long i = (((a.length - 1) & b) << ASHIFT) + ABASE;
    //             if ((t = ((ForkJoinTask<?>)
    //                       U.getObjectVolatile(a, i))) != null) {
    //                 if (ec < 0)
    //                     helpRelease(c, ws, w, q, b);
    //                 else if (q.base == b &&
    //                          U.compareAndSwapObject(a, i, t, null)) {
    //                     U.putOrderedInt(q, QBASE, b + 1);
    //                     if ((b + 1) - q.top < 0)
    //                         signalWork(ws, q);
    //                     w.runTask(t);
    //                 }
    //             }
    //             break;
    //         }
    //         else if (--j < 0) {
    //             if ((ec | (e = (int)c)) < 0) // inactive or terminating
    //                 return awaitWork(w, c, ec);
    //             else if (ctl == c) {         // try to inactivate and enqueue
    //                 long nc = (long)ec | ((c - AC_UNIT) & (AC_MASK|TC_MASK));
    //                 w.nextWait = e;
    //                 w.eventCount = ec | INT_SIGN;
    //                 if (!U.compareAndSwapLong(this, CTL, c, nc))
    //                     w.eventCount = ec;   // back out
    //             }
    //             break;
    //         }
    //     }
    // }
    return 0;
}

Int32 CForkJoinPool::AwaitWork(
    /* [in] */ WorkQueue* w,
    /* [in] */ Int64 c,
    /* [in] */ Int32 ec)
{
    assert(0 && "TODO");
    return 0;
    // int stat, ns; long parkTime, deadline;
    // if ((stat = w.qlock) >= 0 && w.eventCount == ec && ctl == c &&
    //     !Thread.interrupted()) {
    //     int e = (int)c;
    //     int u = (int)(c >>> 32);
    //     int d = (u >> UAC_SHIFT) + parallelism; // active count

    //     if (e < 0 || (d <= 0 && tryTerminate(false, false)))
    //         stat = w.qlock = -1;          // pool is terminating
    //     else if ((ns = w.nsteals) != 0) { // collect steals and retry
    //         long sc;
    //         w.nsteals = 0;
    //         do {} while (!U.compareAndSwapLong(this, STEALCOUNT,
    //                                            sc = stealCount, sc + ns));
    //     }
    //     else {
    //         long pc = ((d > 0 || ec != (e | INT_SIGN)) ? 0L :
    //                    ((long)(w.nextWait & E_MASK)) | // ctl to restore
    //                    ((long)(u + UAC_UNIT)) << 32);
    //         if (pc != 0L) {               // timed wait if last waiter
    //             int dc = -(short)(c >>> TC_SHIFT);
    //             parkTime = (dc < 0 ? FAST_IDLE_TIMEOUT:
    //                         (dc + 1) * IDLE_TIMEOUT);
    //             deadline = System.nanoTime() + parkTime - TIMEOUT_SLOP;
    //         }
    //         else
    //             parkTime = deadline = 0L;
    //         if (w.eventCount == ec && ctl == c) {
    //             Thread wt = Thread.currentThread();
    //             U.putObject(wt, PARKBLOCKER, this);
    //             w.parker = wt;            // emulate LockSupport.park
    //             if (w.eventCount == ec && ctl == c)
    //                 U.park(false, parkTime);  // must recheck before park
    //             w.parker = null;
    //             U.putObject(wt, PARKBLOCKER, null);
    //             if (parkTime != 0L && ctl == c &&
    //                 deadline - System.nanoTime() <= 0L &&
    //                 U.compareAndSwapLong(this, CTL, c, pc))
    //                 stat = w.qlock = -1;  // shrink pool
    //         }
    //     }
    // }
    // return stat;
}

void CForkJoinPool::HelpRelease(
    /* [in] */ Int64 c,
    /* [in] */ ArrayOf<WorkQueue*>* ws,
    /* [in] */ WorkQueue* w,
    /* [in] */ WorkQueue* q,
    /* [in] */ Int32 b)
{
    assert(0 && "TODO");
    // WorkQueue v; int e, i; Thread p;
    // if (w != null && w.eventCount < 0 && (e = (int)c) > 0 &&
    //     ws != null && ws.length > (i = e & SMASK) &&
    //     (v = ws[i]) != null && ctl == c) {
    //     long nc = (((long)(v.nextWait & E_MASK)) |
    //                ((long)((int)(c >>> 32) + UAC_UNIT)) << 32);
    //     int ne = (e + E_SEQ) & E_MASK;
    //     if (q != null && q.base == b && w.eventCount < 0 &&
    //         v.eventCount == (e | INT_SIGN) &&
    //         U.compareAndSwapLong(this, CTL, c, nc)) {
    //         v.eventCount = ne;
    //         if ((p = v.parker) != null)
    //             U.unpark(p);
    //     }
    // }
}

Int32 CForkJoinPool::TryHelpStealer(
    /* [in] */ WorkQueue* joiner,
    /* [in] */ IForkJoinTask* task)
{
    assert(0 && "TODO");
    return 0;
    // int stat = 0, steps = 0;                    // bound to avoid cycles
    // if (task != null && joiner != null &&
    //     joiner.base - joiner.top >= 0) {        // hoist checks
    //     restart: for (;;) {
    //         ForkJoinTask<?> subtask = task;     // current target
    //         for (WorkQueue j = joiner, v;;) {   // v is stealer of subtask
    //             WorkQueue[] ws; int m, s, h;
    //             if ((s = task.status) < 0) {
    //                 stat = s;
    //                 break restart;
    //             }
    //             if ((ws = workQueues) == null || (m = ws.length - 1) <= 0)
    //                 break restart;              // shutting down
    //             if ((v = ws[h = (j.hint | 1) & m]) == null ||
    //                 v.currentSteal != subtask) {
    //                 for (int origin = h;;) {    // find stealer
    //                     if (((h = (h + 2) & m) & 15) == 1 &&
    //                         (subtask.status < 0 || j.currentJoin != subtask))
    //                         continue restart;   // occasional staleness check
    //                     if ((v = ws[h]) != null &&
    //                         v.currentSteal == subtask) {
    //                         j.hint = h;        // save hint
    //                         break;
    //                     }
    //                     if (h == origin)
    //                         break restart;      // cannot find stealer
    //                 }
    //             }
    //             for (;;) { // help stealer or descend to its stealer
    //                 ForkJoinTask[] a; int b;
    //                 if (subtask.status < 0)     // surround probes with
    //                     continue restart;       //   consistency checks
    //                 if ((b = v.base) - v.top < 0 && (a = v.array) != null) {
    //                     int i = (((a.length - 1) & b) << ASHIFT) + ABASE;
    //                     ForkJoinTask<?> t =
    //                         (ForkJoinTask<?>)U.getObjectVolatile(a, i);
    //                     if (subtask.status < 0 || j.currentJoin != subtask ||
    //                         v.currentSteal != subtask)
    //                         continue restart;   // stale
    //                     stat = 1;               // apparent progress
    //                     if (v.base == b) {
    //                         if (t == null)
    //                             break restart;
    //                         if (U.compareAndSwapObject(a, i, t, null)) {
    //                             U.putOrderedInt(v, QBASE, b + 1);
    //                             ForkJoinTask<?> ps = joiner.currentSteal;
    //                             int jt = joiner.top;
    //                             do {
    //                                 joiner.currentSteal = t;
    //                                 t.doExec(); // clear local tasks too
    //                             } while (task.status >= 0 &&
    //                                      joiner.top != jt &&
    //                                      (t = joiner.pop()) != null);
    //                             joiner.currentSteal = ps;
    //                             break restart;
    //                         }
    //                     }
    //                 }
    //                 else {                      // empty -- try to descend
    //                     ForkJoinTask<?> next = v.currentJoin;
    //                     if (subtask.status < 0 || j.currentJoin != subtask ||
    //                         v.currentSteal != subtask)
    //                         continue restart;   // stale
    //                     else if (next == null || ++steps == MAX_HELP)
    //                         break restart;      // dead-end or maybe cyclic
    //                     else {
    //                         subtask = next;
    //                         j = v;
    //                         break;
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
    // return stat;
}

Int32 CForkJoinPool::HelpComplete(
    /* [in] */ WorkQueue* joiner,
    /* [in] */ ICountedCompleter* task)
{
    AutoPtr<ArrayOf<WorkQueue*> > ws; Int32 m;
    Int32 s = 0;
    if ((ws = mWorkQueues) != NULL && (m = ws->GetLength() - 1) >= 0 &&
        joiner != NULL && task != NULL) {
        Int32 j = joiner->mPoolIndex;
        Int32 scans = m + m + 1;
        Int64 c = 0LL;              // for stability check
        for (Int32 k = scans; ; j += 2) {
            AutoPtr<CountedCompleter> ct = (CountedCompleter*)task;
            AutoPtr<WorkQueue> q;
            if ((s = ct->mStatus) < 0)
                break;
            else if (joiner->InternalPopAndExecCC(task))
                k = scans;
            else if ((s = ct->mStatus) < 0)
                break;
            else if ((q = (*ws)[j & m]) != NULL && q->PollAndExecCC(task))
                k = scans;
            else if (--k < 0) {
                if (c == mCtl) {
                    break;
                }
                c = mCtl;
                k = scans;
            }
        }
    }
    return s;
}

Boolean CForkJoinPool::TryCompensate(
    /* [in] */ Int64 c)
{
    assert(0 && "TODO");
    // WorkQueue[] ws = workQueues;
    // int pc = parallelism, e = (int)c, m, tc;
    // if (ws != null && (m = ws.length - 1) >= 0 && e >= 0 && ctl == c) {
    //     WorkQueue w = ws[e & m];
    //     if (e != 0 && w != null) {
    //         Thread p;
    //         long nc = ((long)(w.nextWait & E_MASK) |
    //                    (c & (AC_MASK|TC_MASK)));
    //         int ne = (e + E_SEQ) & E_MASK;
    //         if (w.eventCount == (e | INT_SIGN) &&
    //             U.compareAndSwapLong(this, CTL, c, nc)) {
    //             w.eventCount = ne;
    //             if ((p = w.parker) != null)
    //                 U.unpark(p);
    //             return true;   // replace with idle worker
    //         }
    //     }
    //     else if ((tc = (short)(c >>> TC_SHIFT)) >= 0 &&
    //              (int)(c >> AC_SHIFT) + pc > 1) {
    //         long nc = ((c - AC_UNIT) & AC_MASK) | (c & ~AC_MASK);
    //         if (U.compareAndSwapLong(this, CTL, c, nc))
    //             return true;   // no compensation
    //     }
    //     else if (tc + pc < MAX_CAP) {
    //         long nc = ((c + TC_UNIT) & TC_MASK) | (c & ~TC_MASK);
    //         if (U.compareAndSwapLong(this, CTL, c, nc)) {
    //             ForkJoinWorkerThreadFactory fac;
    //             Throwable ex = null;
    //             ForkJoinWorkerThread wt = null;
    //             try {
    //                 if ((fac = factory) != null &&
    //                     (wt = fac.newThread(this)) != null) {
    //                     wt.start();
    //                     return true;
    //                 }
    //             } catch (Throwable rex) {
    //                 ex = rex;
    //             }
    //             deregisterWorker(wt, ex); // clean up and return false
    //         }
    //     }
    // }
    return FALSE;
}

Int32 CForkJoinPool::AwaitJoin(
    /* [in] */ WorkQueue* joiner,
    /* [in] */ IForkJoinTask* task)
{
    assert(0 && "TODO");
    int s = 0;
    // if (task != null && (s = task.status) >= 0 && joiner != null) {
    //     ForkJoinTask<?> prevJoin = joiner.currentJoin;
    //     joiner.currentJoin = task;
    //     do {} while (joiner.tryRemoveAndExec(task) && // process local tasks
    //                  (s = task.status) >= 0);
    //     if (s >= 0 && (task instanceof CountedCompleter))
    //         s = helpComplete(joiner, (CountedCompleter<?>)task);
    //     long cc = 0;        // for stability checks
    //     while (s >= 0 && (s = task.status) >= 0) {
    //         if ((s = tryHelpStealer(joiner, task)) == 0 &&
    //             (s = task.status) >= 0) {
    //             if (!tryCompensate(cc))
    //                 cc = ctl;
    //             else {
    //                 if (task.trySetSignal() && (s = task.status) >= 0) {
    //                     {    AutoLock syncLock(task);
    //                         if (task.status >= 0) {
    //                             try {                // see ForkJoinTask
    //                                 task.wait();     //  for explanation
    //                             } catch (InterruptedException ie) {
    //                             }
    //                         }
    //                         else
    //                             task.notifyAll();
    //                     }
    //                 }
    //                 long c; // reactivate
    //                 do {} while (!U.compareAndSwapLong
    //                              (this, CTL, c = ctl,
    //                               ((c & ~AC_MASK) |
    //                                ((c & AC_MASK) + AC_UNIT))));
    //             }
    //         }
    //     }
    //     joiner.currentJoin = prevJoin;
    // }
    return s;
}

void CForkJoinPool::HelpJoinOnce(
    /* [in] */ WorkQueue* joiner,
    /* [in] */ ForkJoinTask* task)
{
    Int32 s = 0;
    if (joiner != NULL && task != NULL && (s = task->mStatus) >= 0) {
        AutoPtr<IForkJoinTask> prevJoin = joiner->mCurrentJoin;
        joiner->mCurrentJoin = task;
        do {} while (joiner->TryRemoveAndExec(task) && // process local tasks
                     (s = task->mStatus) >= 0);
        if (s >= 0) {
            if (task->Probe(EIID_ICountedCompleter) != NULL)
                HelpComplete(joiner, ICountedCompleter::Probe(task));
            do {} while (task->mStatus >= 0 &&
                         TryHelpStealer(joiner, task) > 0);
        }
        joiner->mCurrentJoin = prevJoin;
    }
}

AutoPtr<CForkJoinPool::WorkQueue> CForkJoinPool::FindNonEmptyStealQueue()
{
    Int32 r = 0;
    IRandom::Probe(CThreadLocalRandom::GetCurrent())->NextInt32(&r);
    for (;;) {
        Int32 ps = mPlock, m; AutoPtr<ArrayOf<WorkQueue*> > ws; AutoPtr<WorkQueue> q;
        if ((ws = mWorkQueues) != NULL && (m = ws->GetLength() - 1) >= 0) {
            for (Int32 j = (m + 1) << 2; j >= 0; --j) {
                if ((q = (*ws)[(((r - j) << 1) | 1) & m]) != NULL &&
                    q->mBase - q->mTop < 0)
                    return q;
            }
        }
        if (mPlock == ps)
            return NULL;
    }
}

void CForkJoinPool::HelpQuiescePool(
    /* [in] */ WorkQueue* w)
{
    assert(0 && "TODO");
    // ForkJoinTask<?> ps = w.currentSteal;
    // for (boolean active = true;;) {
    //     long c; WorkQueue q; ForkJoinTask<?> t; int b;
    //     while ((t = w.nextLocalTask()) != null)
    //         t.doExec();
    //     if ((q = findNonEmptyStealQueue()) != null) {
    //         if (!active) {      // re-establish active count
    //             active = true;
    //             do {} while (!U.compareAndSwapLong
    //                          (this, CTL, c = ctl,
    //                           ((c & ~AC_MASK) |
    //                            ((c & AC_MASK) + AC_UNIT))));
    //         }
    //         if ((b = q.base) - q.top < 0 && (t = q.pollAt(b)) != null) {
    //             (w.currentSteal = t).doExec();
    //             w.currentSteal = ps;
    //         }
    //     }
    //     else if (active) {       // decrement active count without queuing
    //         long nc = ((c = ctl) & ~AC_MASK) | ((c & AC_MASK) - AC_UNIT);
    //         if ((int)(nc >> AC_SHIFT) + parallelism == 0)
    //             break;          // bypass decrement-then-increment
    //         if (U.compareAndSwapLong(this, CTL, c, nc))
    //             active = false;
    //     }
    //     else if ((int)((c = ctl) >> AC_SHIFT) + parallelism <= 0 &&
    //              U.compareAndSwapLong
    //              (this, CTL, c, ((c & ~AC_MASK) |
    //                              ((c & AC_MASK) + AC_UNIT))))
    //         break;
    // }
}

AutoPtr<IForkJoinTask> CForkJoinPool::NextTaskFor(
    /* [in] */ WorkQueue* w)
{
    for (AutoPtr<IForkJoinTask> t;;) {
        AutoPtr<WorkQueue> q; Int32 b;
        if ((t = w->NextLocalTask()) != NULL)
            return t;
        if ((q = FindNonEmptyStealQueue()) == NULL)
            return NULL;
        if ((b = q->mBase) - q->mTop < 0 && (t = q->PollAt(b)) != NULL)
            return t;
    }
}

Int32 CForkJoinPool::GetSurplusQueuedTaskCount()
{
    AutoPtr<IThread> t; AutoPtr<ForkJoinWorkerThread> wt;
    AutoPtr<IForkJoinPool> pool; AutoPtr<WorkQueue> q;
    if ((t = Thread::GetCurrentThread())->Probe(EIID_IForkJoinWorkerThread) != NULL) {
        wt = (ForkJoinWorkerThread*)IForkJoinWorkerThread::Probe(t);
        pool = wt->mPool;
        AutoPtr<CForkJoinPool> cpool = (CForkJoinPool*)pool.Get();
        Int32 p = cpool->mParallelism;
        Int32 n = (q = wt->mWorkQueue)->mTop - q->mBase;
        Int32 a = (Int32)(cpool->mCtl >> AC_SHIFT) + p;
        return n - (a > (p >>= 1) ? 0 :
                    a > (p >>= 1) ? 1 :
                    a > (p >>= 1) ? 2 :
                    a > (p >>= 1) ? 4 :
                    8);
    }
    return 0;
}

Boolean CForkJoinPool::TryTerminate(
    /* [in] */ Boolean now,
    /* [in] */ Boolean enable)
{
    assert(0 && "TODO");
    return FALSE;
    // int ps;
    // if (this == common)                        // cannot shut down
    //     return false;
    // if ((ps = plock) >= 0) {                   // enable by setting plock
    //     if (!enable)
    //         return false;
    //     if ((ps & PL_LOCK) != 0 ||
    //         !U.compareAndSwapInt(this, PLOCK, ps, ps += PL_LOCK))
    //         ps = acquirePlock();
    //     int nps = ((ps + PL_LOCK) & ~SHUTDOWN) | SHUTDOWN;
    //     if (!U.compareAndSwapInt(this, PLOCK, ps, nps))
    //         releasePlock(nps);
    // }
    // for (long c;;) {
    //     if (((c = ctl) & STOP_BIT) != 0) {     // already terminating
    //         if ((short)(c >>> TC_SHIFT) + parallelism <= 0) {
    //             {    AutoLock syncLock(this);
    //                 notifyAll();               // signal when 0 workers
    //             }
    //         }
    //         return true;
    //     }
    //     if (!now) {                            // check if idle & no tasks
    //         WorkQueue[] ws; WorkQueue w;
    //         if ((int)(c >> AC_SHIFT) + parallelism > 0)
    //             return false;
    //         if ((ws = workQueues) != null) {
    //             for (int i = 0; i < ws.length; ++i) {
    //                 if ((w = ws[i]) != null &&
    //                     (!w.isEmpty() ||
    //                      ((i & 1) != 0 && w.eventCount >= 0))) {
    //                     signalWork(ws, w);
    //                     return false;
    //                 }
    //             }
    //         }
    //     }
    //     if (U.compareAndSwapLong(this, CTL, c, c | STOP_BIT)) {
    //         for (int pass = 0; pass < 3; ++pass) {
    //             WorkQueue[] ws; WorkQueue w; Thread wt;
    //             if ((ws = workQueues) != null) {
    //                 int n = ws.length;
    //                 for (int i = 0; i < n; ++i) {
    //                     if ((w = ws[i]) != null) {
    //                         w.qlock = -1;
    //                         if (pass > 0) {
    //                             w.cancelAll();
    //                             if (pass > 1 && (wt = w.owner) != null) {
    //                                 if (!wt.isInterrupted()) {
    //                                     try {
    //                                         wt.interrupt();
    //                                     } catch (Throwable ignore) {
    //                                     }
    //                                 }
    //                                 U.unpark(wt);
    //                             }
    //                         }
    //                     }
    //                 }
    //                 // Wake up workers parked on event queue
    //                 int i, e; long cc; Thread p;
    //                 while ((e = (int)(cc = ctl) & E_MASK) != 0 &&
    //                        (i = e & SMASK) < n && i >= 0 &&
    //                        (w = ws[i]) != null) {
    //                     long nc = ((long)(w.nextWait & E_MASK) |
    //                                ((cc + AC_UNIT) & AC_MASK) |
    //                                (cc & (TC_MASK|STOP_BIT)));
    //                     if (w.eventCount == (e | INT_SIGN) &&
    //                         U.compareAndSwapLong(this, CTL, cc, nc)) {
    //                         w.eventCount = (e + E_SEQ) & E_MASK;
    //                         w.qlock = -1;
    //                         if ((p = w.parker) != null)
    //                             U.unpark(p);
    //                     }
    //                 }
    //             }
    //         }
    //     }
    // }
}

AutoPtr<CForkJoinPool::WorkQueue> CForkJoinPool::CommonSubmitterQueue()
{
    AutoPtr<Submitter> z; AutoPtr<IForkJoinPool> p;
    AutoPtr<ArrayOf<WorkQueue*> > ws; Int32 m, r;
    //TODO:
    // return ((z = submitters->Get()) != NULL &&
    //         (p = mCommon) != NULL &&
    //         (ws = p->mWorkQueues) != NULL &&
    //         (m = ws->GetLength() - 1) >= 0) ?
    //     (*ws)[m & z->mSeed & SQMASK] : NULL;
    return NULL;
}

Boolean CForkJoinPool::TryExternalUnpush(
    /* [in] */ IForkJoinTask* task)
{
    assert(0 && "TODO");
    return FALSE;
    // WorkQueue joiner; ForkJoinTask<?>[] a; int m, s;
    // Submitter z = submitters.get();
    // WorkQueue[] ws = workQueues;
    // boolean popped = false;
    // if (z != null && ws != null && (m = ws.length - 1) >= 0 &&
    //     (joiner = ws[z.seed & m & SQMASK]) != null &&
    //     joiner.base != (s = joiner.top) &&
    //     (a = joiner.array) != null) {
    //     long j = (((a.length - 1) & (s - 1)) << ASHIFT) + ABASE;
    //     if (U.getObject(a, j) == task &&
    //         U.compareAndSwapInt(joiner, QLOCK, 0, 1)) {
    //         if (joiner.top == s && joiner.array == a &&
    //             U.compareAndSwapObject(a, j, task, null)) {
    //             joiner.top = s - 1;
    //             popped = true;
    //         }
    //         joiner.qlock = 0;
    //     }
    // }
    // return popped;
}

Int32 CForkJoinPool::ExternalHelpComplete(
    /* [in] */ ICountedCompleter* task)
{
    assert(0 && "TODO");
    // AutoPtr<WorkQueue> joiner; Int32 m, j;
    // AutoPtr<Submitter> z = mSubmitters->Get();
    // AutoPtr<ArrayOf<WorkQueue*> > ws = mWorkQueues;
    Int32 s = 0;
    // if (z != NULL && ws != NULL && (m = ws->GetLength() - 1) >= 0 &&
    //     (joiner = (*ws)[(j = z->mSeed) & m & SQMASK]) != NULL && task != NULL) {
    //     Int32 scans = m + m + 1;
    //     Int64 c = 0L;             // for stability check
    //     j |= 1;                  // poll odd queues
    //     for (Int32 k = scans; ; j += 2) {
    //         AutoPtr<WorkQueue> q;
    //         if ((s = task->mStatus) < 0)
    //             break;
    //         else if (joiner->ExternalPopAndExecCC(task))
    //             k = scans;
    //         else if ((s = task->mStatus) < 0)
    //             break;
    //         else if ((q = (*ws)[j & m]) != NULL && q->PollAndExecCC(task))
    //             k = scans;
    //         else if (--k < 0) {
    //             if (c == (c = ctl))
    //                 break;
    //             k = scans;
    //         }
    //     }
    // }
    return s;
}

void CForkJoinPool::CheckPermission()
{
    assert(0 && "TODO");
    // SecurityManager security = System.getSecurityManager();
    // if (security != NULL)
    //     security->CheckPermission(modifyThreadPermission);
}

ECode CForkJoinPool::constructor()
{
    AutoPtr<IOs> os;
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    libcore->GetOs((IOs**)&os);
    Int64 ival = 4;
    os->Sysconf(OsConstants::__SC_NPROCESSORS_CONF, &ival);
    Int32 numCpu = ival;
    return constructor(Elastos::Core::Math::Min(MAX_CAP, numCpu),
        mDefaultForkJoinWorkerThreadFactory, NULL, FALSE);
}

ECode CForkJoinPool::constructor(
    /* [in] */ const Int32& parallelism)
{
    return constructor(parallelism, mDefaultForkJoinWorkerThreadFactory, NULL, FALSE);
}

ECode CForkJoinPool::constructor(
    /* [in] */ const Int32& parallelism,
    /* [in] */ IForkJoinPoolForkJoinWorkerThreadFactory* factory,
    /* [in] */ IThreadUncaughtExceptionHandler* handler,
    /* [in] */ const Boolean& asyncMode)
{
    StringBuilder sb;
    sb += "ForkJoinPool-";
    sb += NextPoolId();
    sb += "-worker-";
    constructor(CheckParallelism(parallelism),
         CheckFactory(factory),
         handler,
         (asyncMode ? FIFO_QUEUE : LIFO_QUEUE),
         sb.ToString());
    CheckPermission();
    return NOERROR;
}

Int32 CForkJoinPool::CheckParallelism(
    /* [in] */ Int32 parallelism)
{
    if (parallelism <= 0 || parallelism > MAX_CAP)
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    return parallelism;
}

AutoPtr<IForkJoinPoolForkJoinWorkerThreadFactory> CForkJoinPool::CheckFactory(
    /* [in] */ IForkJoinPoolForkJoinWorkerThreadFactory* factory)
{
    if (factory == NULL)
        return NULL;
//        return E_NULL_POINTER_EXCEPTION;
    return factory;
}

ECode CForkJoinPool::constructor(
    /* [in] */ Int32 parallelism,
    /* [in] */ IForkJoinPoolForkJoinWorkerThreadFactory* factory,
    /* [in] */ IThreadUncaughtExceptionHandler* handler,
    /* [in] */ Int32 mode,
    /* [in] */ const String& workerNamePrefix)
{
    mWorkerNamePrefix = workerNamePrefix;
    mFactory = factory;
    mUeh = handler;
    mMode = (Int16)mode;
    mParallelism = (Int16)parallelism;
    Int64 np = (Int64)(-parallelism); // offset ctl counts
    mCtl = ((np << AC_SHIFT) & AC_MASK) | ((np << TC_SHIFT) & TC_MASK);
    return NOERROR;
}

AutoPtr<IForkJoinPool> CForkJoinPool::CommonPool()
{
    // assert common != null : "static init error";
    return mCommon;
}

ECode CForkJoinPool::Invoke(
    /* [in] */ IForkJoinTask* task,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    if (task == NULL)
        return E_NULL_POINTER_EXCEPTION;
    ExternalPush(task);
    return task->Join(outface);
}

ECode CForkJoinPool::Execute(
    /* [in] */ IForkJoinTask* task)
{
    if (task == NULL)
        return E_NULL_POINTER_EXCEPTION;
    ExternalPush(task);
    return NOERROR;
}

ECode CForkJoinPool::Execute(
    /* [in] */ IRunnable* task)
{
    if (task == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IForkJoinTask> job;
    if (task->Probe(EIID_IForkJoinTask) != NULL) // avoid re-wrap
        job = (IForkJoinTask*) task;
    else
        job = new RunnableExecuteAction(task);
    ExternalPush(job);
    return NOERROR;
}

ECode CForkJoinPool::Submit(
    /* [in] */ IForkJoinTask* task,
    /* [out] */ IForkJoinTask** outfork)
{
    VALIDATE_NOT_NULL(outfork);
    if (task == NULL)
        return E_NULL_POINTER_EXCEPTION;
    ExternalPush(task);
    *outfork = task;
    REFCOUNT_ADD(*outfork);
    return NOERROR;
}

ECode CForkJoinPool::Submit(
    /* [in] */ ICallable* task,
    /* [out] */ IFuture** outfork)
{
    VALIDATE_NOT_NULL(outfork);
    if (task == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IForkJoinTask> job = new AdaptedCallable(task);
    ExternalPush(job);
    *outfork = (IFuture*)job->Probe(EIID_IFuture);
    REFCOUNT_ADD(*outfork);
    return NOERROR;
}

ECode CForkJoinPool::Submit(
    /* [in] */ IRunnable* task,
    /* [in] */ IInterface* result,
    /* [out] */ IFuture** outfork)
{
    VALIDATE_NOT_NULL(outfork);
    if (task == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IForkJoinTask> job = new AdaptedRunnable(task, result);
    ExternalPush(job);
    *outfork = (IFuture*)job->Probe(EIID_IFuture);
    REFCOUNT_ADD(*outfork);
    return NOERROR;
}

ECode CForkJoinPool::Submit(
    /* [in] */ IRunnable* task,
    /* [out] */ IFuture** outfork)
{
    VALIDATE_NOT_NULL(outfork);
    if (task == NULL)
        return E_NULL_POINTER_EXCEPTION;
    AutoPtr<IForkJoinTask> job;
    if (task->Probe(EIID_IForkJoinTask) != NULL) // avoid re-wrap
        job = (IForkJoinTask*) task;
    else
        job = new AdaptedRunnableAction(task);
    ExternalPush(job);
    *outfork = (IFuture*)job->Probe(EIID_IFuture);
    REFCOUNT_ADD(*outfork);
    return NOERROR;
}

ECode CForkJoinPool::InvokeAll(
    /* [in] */ ICollection* tasks,
    /* [out] */ IList** futures)
{
    VALIDATE_NOT_NULL(futures);

    // In previous versions of this class, this method constructed
    // a task to run ForkJoinTask.invokeAll, but now external
    // invocation of multiple tasks is at least as efficient.
    Int32 size;
    tasks->GetSize(&size);
    AutoPtr<IArrayList> forkJoinTasks;
    CArrayList::New(size, (IArrayList**)&forkJoinTasks);
    Boolean done = FALSE;

    AutoPtr<IIterator> it;
    tasks->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<ICallable> task = ICallable::Probe(obj);
        AutoPtr<IForkJoinTask> f = new AdaptedCallable(task);
        (IList::Probe(forkJoinTasks))->Add(f);
        ExternalPush(f);
    }
    forkJoinTasks->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> p;
        forkJoinTasks->Get(i, (IInterface**)&p);
        IForkJoinTask::Probe(p)->QuietlyJoin();
    }
    done = TRUE;
    if (!done) {
        forkJoinTasks->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
            AutoPtr<IInterface> p;
            forkJoinTasks->Get(i, (IInterface**)&p);
            Boolean bCancl;
            IFuture::Probe(IForkJoinTask::Probe(p))->Cancel(FALSE, &bCancl);
        }
    }
    AutoPtr<IList> l = (IList*)(forkJoinTasks->Probe(EIID_IList));
    *futures = l;
    REFCOUNT_ADD(*futures);
    return NOERROR;
}

ECode CForkJoinPool::GetFactory(
    /* [out] */ IForkJoinPoolForkJoinWorkerThreadFactory** res)
{
    VALIDATE_NOT_NULL(res);
    *res = mFactory;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CForkJoinPool::GetUncaughtExceptionHandler(
    /* [out] */ IThreadUncaughtExceptionHandler** res)
{
    VALIDATE_NOT_NULL(res);
    *res = mUeh;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CForkJoinPool::GetParallelism(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Int32 par;
    *value = ((par = mParallelism) > 0) ? par : 1;
    return NOERROR;
}

Int32 CForkJoinPool::GetCommonPoolParallelism()
{
    return mCommonParallelism;
}

ECode CForkJoinPool::GetPoolSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mParallelism + (Int16)(mCtl >> TC_SHIFT);
    return NOERROR;
}

ECode CForkJoinPool::GetAsyncMode(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mMode == FIFO_QUEUE;
    return NOERROR;
}

ECode CForkJoinPool::GetRunningThreadCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Int32 rc = 0;
    AutoPtr<ArrayOf<WorkQueue*> > ws; AutoPtr<WorkQueue> w;
    if ((ws = mWorkQueues) != NULL) {
        for (Int32 i = 1; i < ws->GetLength(); i += 2) {
            if ((w = (*ws)[i]) != NULL && w->IsApparentlyUnblocked())
                ++rc;
        }
    }
    *value = rc;
    return NOERROR;
}

ECode CForkJoinPool::GetActiveThreadCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    Int32 r = mParallelism + (Int32)(mCtl >> AC_SHIFT);
    *value = (r <= 0) ? 0 : r; // suppress momentarily negative values
    return NOERROR;
}

ECode CForkJoinPool::IsQuiescent(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mParallelism + (Int32)(mCtl >> AC_SHIFT) <= 0;
    return NOERROR;
}

ECode CForkJoinPool::GetStealCount(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    Int64 count = mStealCount;
    AutoPtr<ArrayOf<WorkQueue*> > ws; AutoPtr<WorkQueue> w;
    if ((ws = mWorkQueues) != NULL) {
        for (Int32 i = 1; i < ws->GetLength(); i += 2) {
            if ((w = (*ws)[i]) != NULL)
                count += w->mNsteals;
        }
    }
    *value = count;
    return NOERROR;
}

ECode CForkJoinPool::GetQueuedTaskCount(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)

    Int64 count = 0;
    AutoPtr<ArrayOf<WorkQueue*> > ws; AutoPtr<WorkQueue> w;
    if ((ws = mWorkQueues) != NULL) {
        for (Int32 i = 1; i < ws->GetLength(); i += 2) {
            if ((w = (*ws)[i]) != NULL)
                count += w->QueueSize();
        }
    }
    *value = count;
    return NOERROR;
}

ECode CForkJoinPool::GetQueuedSubmissionCount(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    Int32 count = 0;
    AutoPtr<ArrayOf<WorkQueue*> > ws; AutoPtr<WorkQueue> w;
    if ((ws = mWorkQueues) != NULL) {
        for (Int32 i = 0; i < ws->GetLength(); i += 2) {
            if ((w = (*ws)[i]) != NULL)
                count += w->QueueSize();
        }
    }
    *value = count;
    return NOERROR;
}

ECode CForkJoinPool::HasQueuedSubmissions(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<ArrayOf<WorkQueue*> > ws; AutoPtr<WorkQueue> w;
    if ((ws = mWorkQueues) != NULL) {
        for (Int32 i = 0; i < ws->GetLength(); i += 2) {
            if ((w = (*ws)[i]) != NULL && !w->IsEmpty()) {
                *value = TRUE;
                return NOERROR;
            }
        }
    }
    *value = FALSE;
    return NOERROR;
}

AutoPtr<IForkJoinTask> CForkJoinPool::PollSubmission()
{
    AutoPtr<ArrayOf<WorkQueue*> > ws; AutoPtr<WorkQueue> w;
    AutoPtr<IForkJoinTask> t;
    if ((ws = mWorkQueues) != NULL) {
        for (Int32 i = 0; i < ws->GetLength(); i += 2) {
            if ((w = (*ws)[i]) != NULL && (t = w->Poll()) != NULL)
                return t;
        }
    }
    return NULL;
}

Int32 CForkJoinPool::DrainTasksTo(
    /* [in] */ ICollection* c)
{
    Int32 count = 0;
    AutoPtr<ArrayOf<WorkQueue*> > ws; AutoPtr<WorkQueue> w;
    AutoPtr<IForkJoinTask> t;
    if ((ws = mWorkQueues) != NULL) {
        for (Int32 i = 0; i < ws->GetLength(); ++i) {
            if ((w = (*ws)[i]) != NULL) {
                while ((t = w->Poll()) != NULL) {
                    c->Add(t);
                    ++count;
                }
            }
        }
    }
    return count;
}

ECode CForkJoinPool::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    // Use a single pass through workQueues to collect counts
    Int64 qt = 0L, qs = 0L; Int32 rc = 0;
    Int64 st = mStealCount;
    Int64 c = mCtl;
    AutoPtr<ArrayOf<WorkQueue*> > ws; AutoPtr<WorkQueue> w;
    if ((ws = mWorkQueues) != NULL) {
        for (Int32 i = 0; i < ws->GetLength(); ++i) {
            if ((w = (*ws)[i]) != NULL) {
                Int32 size = w->QueueSize();
                if ((i & 1) == 0)
                    qs += size;
                else {
                    qt += size;
                    st += w->mNsteals;
                    if (w->IsApparentlyUnblocked())
                        ++rc;
                }
            }
        }
    }
    Int32 pc = mParallelism;
    Int32 tc = pc + (Int16)(c >> TC_SHIFT);
    Int32 ac = pc + (Int32)(c >> AC_SHIFT);
    if (ac < 0) // ignore transient negative
        ac = 0;
    String level;
    if ((c & STOP_BIT) != 0)
        level = (tc == 0) ? "Terminated" : "Terminating";
    else
        level = mPlock < 0 ? "Shutting down" : "Running";
    StringBuilder sb("[");
    sb += level;
    sb += ", parallelism = "; sb += pc;
    sb += ", size = "; sb += tc;
    sb += ", active = "; sb += ac;
    sb += ", running = "; sb += rc;
    sb += ", steals = "; sb += st;
    sb += ", tasks = "; sb += qt;
    sb += ", submissions = "; sb += qs;
    sb += "]";
    sb.ToString(str);
    return NOERROR;
}

ECode CForkJoinPool::Shutdown()
{
    CheckPermission();
    TryTerminate(FALSE, TRUE);
    return NOERROR;
}

ECode CForkJoinPool::ShutdownNow(
    /* [out] */ IList** tasks)
{
    VALIDATE_NOT_NULL(tasks);
    CheckPermission();
    TryTerminate(TRUE, TRUE);
    return Collections::GetEmptyList(tasks);
}

ECode CForkJoinPool::IsTerminated(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int64 c = mCtl;
    *result = ((c & STOP_BIT) != 0L &&
            (Int16)(c >> TC_SHIFT) + mParallelism <= 0);
    return NOERROR;
}

ECode CForkJoinPool::IsTerminating(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    Int64 c = mCtl;
    *value = ((c & STOP_BIT) != 0L &&
            (Int16)(c >> TC_SHIFT) + mParallelism > 0);
    return NOERROR;
}

ECode CForkJoinPool::IsShutdown(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPlock < 0;
    return NOERROR;
}

ECode CForkJoinPool::AwaitTermination(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (Thread::Interrupted())
        return E_INTERRUPTED_EXCEPTION;
    if (Object::Equals(TO_IINTERFACE(this), mCommon->Probe(EIID_IInterface))) {
        Boolean b = FALSE;
        AwaitQuiescence(timeout, unit, &b);
        *result = FALSE;
        return NOERROR;
    }
    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    Boolean b = FALSE;
    if ((IsTerminated(&b), b)) {
        *result = TRUE;
        return NOERROR;
    }
    if (nanos <= 0L) {
        *result = FALSE;
        return NOERROR;
    }
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 nas;
    system->GetNanoTime(&nas);
    Int64 deadline = nas + nanos;
    {    AutoLock syncLock(this);
        for (;;) {
            Boolean b2 = FALSE;
            if ((IsTerminated(&b2), b2)) {
                *result = TRUE;
                return NOERROR;
            }
            if (nanos <= 0L) {
                *result = FALSE;
                return NOERROR;
            }
            Int64 millis;
            TimeUnit::GetNANOSECONDS()->ToMillis(nanos, &millis);
            Wait(millis > 0L ? millis : 1L);
            Int64 nas2;
            system->GetNanoTime(&nas2);
            nanos = deadline - nas2;
        }
    }
    return NOERROR;
}

ECode CForkJoinPool::AwaitQuiescence(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    Int64 nanos;
    unit->ToNanos(timeout, &nanos);
    AutoPtr<ForkJoinWorkerThread> wt;
    AutoPtr<IThread> thread = Thread::GetCurrentThread();
    if (thread->Probe(EIID_IForkJoinWorkerThread) != NULL) {
        wt = (ForkJoinWorkerThread*)IForkJoinWorkerThread::Probe(thread);
        if (Object::Equals(wt->mPool->Probe(EIID_IInterface), TO_IINTERFACE(this))) {
            HelpQuiescePool(wt->mWorkQueue);
            *result = TRUE;
            return NOERROR;
        }
    }
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 startTime;
    system->GetNanoTime(&startTime);
    AutoPtr<ArrayOf<WorkQueue*> > ws;
    Int32 r = 0, m;
    Boolean found = TRUE;
    Boolean isQuies = FALSE;
    while (!(IsQuiescent(&isQuies), isQuies) && (ws = mWorkQueues) != NULL &&
           (m = ws->GetLength() - 1) >= 0) {
        if (!found) {
            Int64 nas;
            system->GetNanoTime(&nas);
            if ((nas - startTime) > nanos) {
                *result = FALSE;
                return NOERROR;
            }
            Thread::Yield(); // cannot block
        }
        found = FALSE;
        for (Int32 j = (m + 1) << 2; j >= 0; --j) {
            AutoPtr<IForkJoinTask> t; AutoPtr<WorkQueue> q; Int32 b;
            if ((q = (*ws)[r++ & m]) != NULL && (b = q->mBase) - q->mTop < 0) {
                found = TRUE;
                if ((t = q->PollAt(b)) != NULL) {
                    Int32 res;
                    t->DoExec(&res);
                }
                break;
            }
        }
    }
    *result = TRUE;
    return NOERROR;
}

void CForkJoinPool::QuiesceCommonPool()
{
    Boolean b;
    mCommon->AwaitQuiescence(Elastos::Core::Math::INT64_MAX_VALUE, TimeUnit::GetNANOSECONDS(), &b);
}

void CForkJoinPool::ManagedBlock(
    /* [in] */ IForkJoinPoolManagedBlocker* blocker)
{
    AutoPtr<IThread> t = Thread::GetCurrentThread();
    if (t->Probe(EIID_IForkJoinWorkerThread) != NULL) {
        AutoPtr<IForkJoinWorkerThread> w = (IForkJoinWorkerThread*) t->Probe(EIID_IForkJoinWorkerThread);
        AutoPtr<ForkJoinWorkerThread> cw = (ForkJoinWorkerThread*) w.Get();
        AutoPtr<CForkJoinPool> cp = (CForkJoinPool*)cw->mPool.Get();
        Boolean bBlocker = TRUE;
        while (!(blocker->IsReleasable(&bBlocker), bBlocker)) {
            if (cp->TryCompensate(cp->mCtl)) {
                Boolean a = FALSE, b = FALSE;
                do {} while (!(blocker->IsReleasable(&a), a) && !(blocker->Block(&b), b));
                cp->IncrementActiveCount();
                break;
            }
        }
    }
    else {
        Boolean a = FALSE, b = FALSE;
        do {} while (!(blocker->IsReleasable(&a), a) && !(blocker->Block(&b), b));
    }
}

AutoPtr<IRunnableFuture> CForkJoinPool::NewTaskFor(
    /* [in] */ IRunnable* runnable,
    /* [in] */ IInterface* value)
{
    AutoPtr<IForkJoinTask> p = new AdaptedRunnable(runnable, value);
    AutoPtr<IFuture> pf = (IFuture*)p->Probe(EIID_IFuture);
    return (IRunnableFuture*)pf.Get();
}

AutoPtr<IRunnableFuture> CForkJoinPool::NewTaskFor(
    /* [in] */ ICallable* callable)
{
    AutoPtr<IForkJoinTask> p = new AdaptedCallable(callable);
    AutoPtr<IFuture> pf = (IFuture*)p->Probe(EIID_IFuture);
    return (IRunnableFuture*)pf.Get();
}

AutoPtr<IForkJoinPool> CForkJoinPool::MakeCommonPool()
{
    Int32 parallelism = -1;
    AutoPtr<IForkJoinPoolForkJoinWorkerThreadFactory> factory
        = mDefaultForkJoinWorkerThreadFactory;
    AutoPtr<IThreadUncaughtExceptionHandler> handler = NULL;
    String pp;// = System.getProperty("java.util.concurrent.ForkJoinPool.common.parallelism");
    String fp;// = System.getProperty("java.util.concurrent.ForkJoinPool.common.threadFactory");
    String hp;// = System.getProperty("java.util.concurrent.ForkJoinPool.common.exceptionHandler");
    if (pp != NULL)
        parallelism = StringUtils::ParseInt32(pp);
    // if (fp != NULL)
    //     factory = ((IForkJoinPoolForkJoinWorkerThreadFactory*)ClassLoader.
    //                getSystemClassLoader().loadClass(fp).newInstance());
    // if (hp != NULL)
    //     handler = ((IThreadUncaughtExceptionHandler)ClassLoader.
    //                getSystemClassLoader().loadClass(hp).newInstance());

    AutoPtr<IOs> os;
    AutoPtr<ILibcore> libcore;
    CLibcore::AcquireSingleton((ILibcore**)&libcore);
    libcore->GetOs((IOs**)&os);
    Int64 ival = 4;
    os->Sysconf(OsConstants::__SC_NPROCESSORS_CONF, &ival);
    Int32 numCpu = ival;

    if (parallelism < 0 && // default 1 less than #cores
        (parallelism = numCpu - 1) < 0)
        parallelism = 0;
    if (parallelism > MAX_CAP)
        parallelism = MAX_CAP;
    AutoPtr<IForkJoinPool> res;
    CForkJoinPool::New(parallelism, factory, handler, LIFO_QUEUE,
                            String("ForkJoinPool.commonPool-worker-"), (IForkJoinPool**)&res);
    return res;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
