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

#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CEXCHANGER_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CEXCHANGER_H__

#include "_Elastos_Utility_Concurrent_CExchanger.h"
#include "AtomicReference.h"
#include "Object.h"

using Elastos::Core::IThread;
using Elastos::Utility::Concurrent::Atomic::IAtomicReference;
using Elastos::Utility::Concurrent::Atomic::AtomicReference;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CExchanger)
    , public Object
    , public IExchanger
{
public:
    class CDummyObject
        : public Object
    {
    };

    /**
     * Nodes hold partially exchanged data, plus other per-thread
     * bookkeeping.
     */
    class Node
        : public Object
    {
    public:
        Int32 mIndex;              // Arena index
        Int32 mBound;              // Last recorded value of Exchanger.bound
        Int32 mCollides;           // Number of CAS failures at current bound
        Int32 mHash;               // Pseudo-random for spins
        AutoPtr<IInterface> mItem;            // This thread's current item
        volatile AutoPtr<IInterface> mMatch;  // Item provided by releasing thread
        volatile AutoPtr<IThread> mParked; // Set to this thread when parked, else null

        // Padding to ameliorate unfortunate memory placements
        AutoPtr<IInterface> mP0, mP1, mP2, mP3, mP4, mP5, mP6, mP7, mP8, mP9, mPa, mPb, mPc, mPd, mPe, mPf;
        AutoPtr<IInterface> mQ0, mQ1, mQ2, mQ3, mQ4, mQ5, mQ6, mQ7, mQ8, mQ9, mQa, mQb, mQc, mQd, mQe, mQf;
    };

    /** The corresponding thread local class */
    class Participant
        : public Object
//        : public ThreadLocal
    {
    public:
        AutoPtr<Node> InitialValue()
        {
            return new Node();
        }
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Creates a new Exchanger.
     */
    CARAPI constructor();

    CARAPI Exchange(
        /* [in] */ IInterface* x,
        /* [out] */ IInterface** outface);

    CARAPI Exchange(
        /* [in] */ IInterface* x,
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** outface);

private:
    /**
     * Exchange function when arenas enabled. See above for explanation.
     *
     * @param item the (non-null) item to exchange
     * @param timed true if the wait is timed
     * @param ns if timed, the maximum wait time, else 0L
     * @return the other thread's item; or null if interrupted; or
     * TIMED_OUT if timed and timed out
     */
    CARAPI_(AutoPtr<IInterface>) ArenaExchange(
        /* [in] */ IInterface* item,
        /* [in] */ Boolean timed,
        /* [in] */ Int64 ns);

    /**
     * Exchange function used until arenas enabled. See above for explanation.
     *
     * @param item the item to exchange
     * @param timed true if the wait is timed
     * @param ns if timed, the maximum wait time, else 0L
     * @return the other thread's item; or null if either the arena
     * was enabled or the thread was interrupted before completion; or
     * TIMED_OUT if timed and timed out
     */
    CARAPI_(AutoPtr<IInterface>) SlotExchange(
        /* [in] */ IInterface* item,
        /* [in] */ Boolean timed,
        /* [in] */ Int64 ns);

private:
    /**
     * The byte distance (as a shift value) between any two used slots
     * in the arena.  1 << ASHIFT should be at least cacheline size.
     */
    static const Int32 sASHIFT;

    /**
     * The maximum supported arena index. The maximum allocatable
     * arena size is MMASK + 1. Must be a power of two minus one, less
     * than (1<<(31-ASHIFT)). The cap of 255 (0xff) more than suffices
     * for the expected scaling limits of the main algorithms.
     */
    static const Int32 sMMASK;

    /**
     * Unit for sequence/version bits of bound field. Each successful
     * change to the bound also adds SEQ.
     */
    static const Int32 sSEQ;

    /** The number of CPUs, for sizing and spin control */
    static const Int32 sNCPU;

    /**
     * Sentinel value returned by internal exchange methods upon
     * timeout, to avoid need for separate timed versions of these
     * methods.
     */
    static AutoPtr<IInterface> sTIMED_OUT;

    /**
     * The value of "max" that will hold all threads without
     * contention.  When this value is less than CAPACITY, some
     * otherwise wasted expansion can be avoided.
     */
    static const Int32 sFULL;

    /**
     * The number of times to spin (doing nothing except polling a
     * memory location) before blocking or giving up while waiting to
     * be fulfilled.  Should be zero on uniprocessors.  On
     * multiprocessors, this value should be large enough so that two
     * threads exchanging items as fast as possible block only when
     * one of them is stalled (due to GC or preemption), but not much
     * longer, to avoid wasting CPU resources.  Seen differently, this
     * value is a little over half the number of cycles of an average
     * context switch time on most systems.  The value here is
     * approximately the average of those across a range of tested
     * systems.
     */
    static const Int32 sSPINS;

    /**
     * Value representing null arguments/returns from public
     * methods.  This disambiguates from internal requirement that
     * holes start out as null to mean they are not yet set.
     */
    static AutoPtr<IInterface> sNULL_ITEM;

    /**
     * Per-thread state
     */
    AutoPtr<Participant> mParticipant;

    /**
     * Slot used until contention detected.
     */
    volatile AutoPtr<Node> mSlot;

    /**
     * The index of the largest valid arena position, OR'ed with SEQ
     * number in high bits, incremented on each update.  The initial
     * update from 0 to SEQ is used to ensure that the arena array is
     * constructed only once.
     */
    volatile Int32 mBound;

    /**
     * Elimination array; null until enabled (within slotExchange).
     * Element accesses use emulation of volatile gets and CAS.
     */
    /* volatile */ AutoPtr<ArrayOf<Node*> > mArena;

    // // Unsafe mechanics
    // private static final sun.misc.Unsafe U;
    // private static final long BOUND;
    // private static final long SLOT;
    // private static final long MATCH;
    // private static final long BLOCKER;
    // private static final int ABASE;
    // static {
    //     int s;
    //     try {
    //         U = sun.misc.Unsafe.getUnsafe();
    //         Class<?> ek = Exchanger.class;
    //         Class<?> nk = Node.class;
    //         Class<?> ak = Node[].class;
    //         Class<?> tk = Thread.class;
    //         BOUND = U.objectFieldOffset
    //             (ek.getDeclaredField("bound"));
    //         SLOT = U.objectFieldOffset
    //             (ek.getDeclaredField("slot"));
    //         MATCH = U.objectFieldOffset
    //             (nk.getDeclaredField("match"));
    //         BLOCKER = U.objectFieldOffset
    //             (tk.getDeclaredField("parkBlocker"));
    //         s = U.arrayIndexScale(ak);
    //         // ABASE absorbs padding in front of element 0
    //         ABASE = U.arrayBaseOffset(ak) + (1 << ASHIFT);

    //     } catch (Exception e) {
    //         throw new Error(e);
    //     }
    //     if ((s & (s-1)) != 0 || s > (1 << ASHIFT))
    //         throw new Error("Unsupported array scale");
    // }
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Utility::Concurrent::CExchanger::Node, IInterface)

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CEXCHANGER_H__
