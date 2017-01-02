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

#include "CCyclicBarrier.h"
#include "Thread.h"

using Elastos::Core::Thread;
using Elastos::Utility::Concurrent::Locks::ILock;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//===============================================================================
// CCyclicBarrier::
//===============================================================================
CAR_INTERFACE_IMPL(CCyclicBarrier, Object, ICyclicBarrier)

CAR_OBJECT_IMPL(CCyclicBarrier)

void CCyclicBarrier::NextGeneration()
{
    // signal completion of last generation
    mTrip->SignalAll();
    // set up next generation
    mCount = mParties;
    mGeneration = new Generation();
}

void CCyclicBarrier::BreakBarrier()
{
    mGeneration->mBroken = TRUE;
    mCount = mParties;
    mTrip->SignalAll();
}

Int32 CCyclicBarrier::Dowait(
    /* [in] */ Boolean timed,
    /* [in] */ Int64 nanos)
{
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
//    try {
        AutoPtr<Generation> g = mGeneration;

        if (g->mBroken) {
            ILock::Probe(lock)->UnLock();
            return 0;
            // throw new BrokenBarrierException();
        }

        if (Thread::Interrupted()) {
            BreakBarrier();
            ILock::Probe(lock)->UnLock();
            return 0;
            // throw new InterruptedException();
        }

        Int32 index = --mCount;
        if (index == 0) {  // tripped
            Boolean ranAction = FALSE;
//            try {
                AutoPtr<IRunnable> command = mBarrierCommand;
                if (command != NULL)
                    command->Run();
                ranAction = TRUE;
                NextGeneration();
//            } finally {
                if (!ranAction)
                    BreakBarrier();
                ILock::Probe(lock)->UnLock();
                return 0;
//            }
        }

        // loop until tripped, broken, interrupted, or timed out
        for (;;) {
//            try {
                if (!timed)
                    mTrip->Await();
                else if (nanos > 0L)
                    mTrip->AwaitNanos(nanos, &nanos);
            // } catch (InterruptedException ie) {
            //     if (g == generation && ! g.broken) {
            //         breakBarrier();
            //         throw ie;
            //     } else {
            //         // We're about to finish waiting even if we had not
            //         // been interrupted, so this interrupt is deemed to
            //         // "belong" to subsequent execution.
            //         Thread.currentThread().interrupt();
            //     }
            // }

            if (g->mBroken) {
                ILock::Probe(lock)->UnLock();
                return 0;
                // throw new BrokenBarrierException();
            }

            if (g != mGeneration) {
                ILock::Probe(lock)->UnLock();
                return index;
            }

            if (timed && nanos <= 0L) {
                BreakBarrier();
                ILock::Probe(lock)->UnLock();
                return 0;
//                throw new TimeoutException();
            }
        }
//    } finally {
//        ILock::Probe(lock)->UnLock();
//    }
}

ECode CCyclicBarrier::constructor(
    /* [in] */ Int32 parties,
    /* [in] */ IRunnable* barrierAction)
{
    if (parties <= 0) return E_ILLEGAL_ARGUMENT_EXCEPTION;
    mParties = parties;
    mCount = parties;
    mBarrierCommand = barrierAction;
    return NOERROR;
}

ECode CCyclicBarrier::constructor(
    /* [in] */ Int32 parties)
{
    return constructor(parties, NULL);
}

ECode CCyclicBarrier::GetParties(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = mParties;
    return NOERROR;
}

ECode CCyclicBarrier::Await(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    *value = Dowait(FALSE, 0L);
    return NOERROR;
}

ECode CCyclicBarrier::Await(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    Int64 t;
    unit->ToNanos(timeout, &t);
    *value = Dowait(TRUE, t);
    return NOERROR;
}

ECode CCyclicBarrier::IsBroken(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    *value = mGeneration->mBroken;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CCyclicBarrier::Reset()
{
    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    BreakBarrier();   // break the current generation
    NextGeneration(); // start a new generation
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

ECode CCyclicBarrier::GetNumberWaiting(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)

    AutoPtr<IReentrantLock> lock = mLock;
    ILock::Probe(lock)->Lock();
    *value = mParties - mCount;
    ILock::Probe(lock)->UnLock();
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
