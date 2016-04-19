
#include "CExchanger.h"
#include "LockSupport.h"
#include "Thread.h"
#include "Math.h"
#include "CAtomicInteger32.h"
#include "AutoLock.h"

using Elastos::Core::Thread;
using Elastos::Core::Math;
using Elastos::Utility::Concurrent::Locks::LockSupport;
using Elastos::Utility::Concurrent::Atomic::EIID_IAtomicReference;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;

namespace Elastos {
namespace Utility {
namespace Concurrent {

//====================================================================
// CExchanger::
//====================================================================
CAR_INTERFACE_IMPL(CExchanger, Object, IExchanger)

CAR_OBJECT_IMPL(CExchanger);

const Int32 CExchanger::sNCPU = 4;//= Runtime.getRuntime().availableProcessors();
const Int32 CExchanger::sASHIFT = 7;
const Int32 CExchanger::sMMASK = 0xff;
const Int32 CExchanger::sSEQ = sMMASK + 1;
const Int32 CExchanger::sFULL = (sNCPU >= (sMMASK << 1)) ? sMMASK : sNCPU >> 1;
const Int32 CExchanger::sSPINS = 1 << 10;

AutoPtr<IInterface> CExchanger::sNULL_ITEM = (IInterface*)(IObject*)new CDummyObject();
AutoPtr<IInterface> CExchanger::sTIMED_OUT = (IInterface*)(IObject*)new CDummyObject();

AutoPtr<IInterface> CExchanger::ArenaExchange(
    /* [in] */ IInterface* item,
    /* [in] */ Boolean timed,
    /* [in] */ Int64 ns)
{
    assert(0 && "TODO");
    return NULL;
    // Node[] a = arena;
    // Node p = participant.get();
    // for (int i = p.index;;) {                      // access slot at i
    //     int b, m, c; long j;                       // j is raw array offset
    //     Node q = (Node)U.getObjectVolatile(a, j = (i << ASHIFT) + ABASE);
    //     if (q != null && U.compareAndSwapObject(a, j, q, null)) {
    //         Object v = q.item;                     // release
    //         q.match = item;
    //         Thread w = q.parked;
    //         if (w != null)
    //             U.unpark(w);
    //         return v;
    //     }
    //     else if (i <= (m = (b = bound) & MMASK) && q == null) {
    //         p.item = item;                         // offer
    //         if (U.compareAndSwapObject(a, j, null, p)) {
    //             long end = (timed && m == 0) ? System.nanoTime() + ns : 0L;
    //             Thread t = Thread.currentThread(); // wait
    //             for (int h = p.hash, spins = SPINS;;) {
    //                 Object v = p.match;
    //                 if (v != null) {
    //                     U.putOrderedObject(p, MATCH, null);
    //                     p.item = null;             // clear for next use
    //                     p.hash = h;
    //                     return v;
    //                 }
    //                 else if (spins > 0) {
    //                     h ^= h << 1; h ^= h >>> 3; h ^= h << 10; // xorshift
    //                     if (h == 0)                // initialize hash
    //                         h = SPINS | (int)t.getId();
    //                     else if (h < 0 &&          // approx 50% true
    //                              (--spins & ((SPINS >>> 1) - 1)) == 0)
    //                         Thread.yield();        // two yields per wait
    //                 }
    //                 else if (U.getObjectVolatile(a, j) != p)
    //                     spins = SPINS;       // releaser hasn't set match yet
    //                 else if (!t.isInterrupted() && m == 0 &&
    //                          (!timed ||
    //                           (ns = end - System.nanoTime()) > 0L)) {
    //                     U.putObject(t, BLOCKER, this); // emulate LockSupport
    //                     p.parked = t;              // minimize window
    //                     if (U.getObjectVolatile(a, j) == p)
    //                         U.park(false, ns);
    //                     p.parked = null;
    //                     U.putObject(t, BLOCKER, null);
    //                 }
    //                 else if (U.getObjectVolatile(a, j) == p &&
    //                          U.compareAndSwapObject(a, j, p, null)) {
    //                     if (m != 0)                // try to shrink
    //                         U.compareAndSwapInt(this, BOUND, b, b + SEQ - 1);
    //                     p.item = null;
    //                     p.hash = h;
    //                     i = p.index >>>= 1;        // descend
    //                     if (Thread.interrupted())
    //                         return null;
    //                     if (timed && m == 0 && ns <= 0L)
    //                         return TIMED_OUT;
    //                     break;                     // expired; restart
    //                 }
    //             }
    //         }
    //         else
    //             p.item = null;                     // clear offer
    //     }
    //     else {
    //         if (p.bound != b) {                    // stale; reset
    //             p.bound = b;
    //             p.collides = 0;
    //             i = (i != m || m == 0) ? m : m - 1;
    //         }
    //         else if ((c = p.collides) < m || m == FULL ||
    //                  !U.compareAndSwapInt(this, BOUND, b, b + SEQ + 1)) {
    //             p.collides = c + 1;
    //             i = (i == 0) ? m : i - 1;          // cyclically traverse
    //         }
    //         else
    //             i = m + 1;                         // grow
    //         p.index = i;
    //     }
    // }
}

AutoPtr<IInterface> CExchanger::SlotExchange(
    /* [in] */ IInterface* item,
    /* [in] */ Boolean timed,
    /* [in] */ Int64 ns)
{
    assert(0 && "TODO");
    return NULL;
    // Node p = participant.get();
    // Thread t = Thread.currentThread();
    // if (t.isInterrupted()) // preserve interrupt status so caller can recheck
    //     return null;

    // for (Node q;;) {
    //     if ((q = slot) != null) {
    //         if (U.compareAndSwapObject(this, SLOT, q, null)) {
    //             Object v = q.item;
    //             q.match = item;
    //             Thread w = q.parked;
    //             if (w != null)
    //                 U.unpark(w);
    //             return v;
    //         }
    //         // create arena on contention, but continue until slot null
    //         if (NCPU > 1 && bound == 0 &&
    //             U.compareAndSwapInt(this, BOUND, 0, SEQ))
    //             arena = new Node[(FULL + 2) << ASHIFT];
    //     }
    //     else if (arena != null)
    //         return null; // caller must reroute to arenaExchange
    //     else {
    //         p.item = item;
    //         if (U.compareAndSwapObject(this, SLOT, null, p))
    //             break;
    //         p.item = null;
    //     }
    // }

    // // await release
    // int h = p.hash;
    // long end = timed ? System.nanoTime() + ns : 0L;
    // int spins = (NCPU > 1) ? SPINS : 1;
    // Object v;
    // while ((v = p.match) == null) {
    //     if (spins > 0) {
    //         h ^= h << 1; h ^= h >>> 3; h ^= h << 10;
    //         if (h == 0)
    //             h = SPINS | (int)t.getId();
    //         else if (h < 0 && (--spins & ((SPINS >>> 1) - 1)) == 0)
    //             Thread.yield();
    //     }
    //     else if (slot != p)
    //         spins = SPINS;
    //     else if (!t.isInterrupted() && arena == null &&
    //              (!timed || (ns = end - System.nanoTime()) > 0L)) {
    //         U.putObject(t, BLOCKER, this);
    //         p.parked = t;
    //         if (slot == p)
    //             U.park(false, ns);
    //         p.parked = null;
    //         U.putObject(t, BLOCKER, null);
    //     }
    //     else if (U.compareAndSwapObject(this, SLOT, p, null)) {
    //         v = timed && ns <= 0L && !t.isInterrupted() ? TIMED_OUT : null;
    //         break;
    //     }
    // }
    // U.putOrderedObject(p, MATCH, null);
    // p.item = null;
    // p.hash = h;
    // return v;
}

ECode CExchanger::constructor()
{
    mParticipant = new Participant();
    return NOERROR;
}

ECode CExchanger::Exchange(
    /* [in] */ IInterface* x,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface)
    *outface = NULL;

    AutoPtr<IInterface> v;
    AutoPtr<IInterface> item = (x == NULL) ? sNULL_ITEM.Get() : x; // translate null args
    if ((mArena != NULL ||
         (v = SlotExchange(item, FALSE, 0L)) == NULL) &&
        ((Thread::Interrupted() || // disambiguates null return
          (v = ArenaExchange(item, FALSE, 0L)) == NULL)))
        return E_INTERRUPTED_EXCEPTION;
    if (!Object::Equals(v, sNULL_ITEM)) {
        *outface = v;
        REFCOUNT_ADD(*outface)
    }
    return NOERROR;
}

ECode CExchanger::Exchange(
    /* [in] */ IInterface* x,
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    AutoPtr<IInterface> v;
    AutoPtr<IInterface> item = (x == NULL) ? sNULL_ITEM.Get() : x;
    Int64 ns;
    unit->ToNanos(timeout, &ns);
    if ((mArena != NULL ||
         (v = SlotExchange(item, TRUE, ns)) == NULL) &&
        ((Thread::Interrupted() ||
          (v = ArenaExchange(item, TRUE, ns)) == NULL)))
        return E_INTERRUPTED_EXCEPTION;
    if (Object::Equals(v, sTIMED_OUT))
        return E_TIMEOUT_EXCEPTION;

    if (!Object::Equals(v, sNULL_ITEM)) {
        *outface = v;
        REFCOUNT_ADD(*outface)
    }
    return NOERROR;
}

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos
