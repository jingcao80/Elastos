
#ifndef __ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_CREENTRANTREADWRITELOCK_H__
#define __ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_CREENTRANTREADWRITELOCK_H__

#include "_Elastos_Utility_Concurrent_Locks_CReentrantReadWriteLock.h"
#include "AbstractQueuedSynchronizer.h"
#include "Thread.h"

using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::IO::IObjectInputStream;
using Elastos::Utility::ICollection;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Utility {
namespace Concurrent {
namespace Locks {

CarClass(CReentrantReadWriteLock)
    , public Object
    , public IReentrantReadWriteLock
    , public IReadWriteLock
    , public ISerializable
{
public:
    enum CLSID {
    CLSID_Sync,
    CLSID_NonfairSync,
    CLSID_FairSync,
    };

    /**
     * Synchronization implementation for ReentrantReadWriteLock.
     * Subclassed into fair and nonfair versions.
     */
    class Sync
        : public AbstractQueuedSynchronizer
    {
    public:
        /** Returns the number of shared holds represented in count  */
        static CARAPI_(Int32) SharedCount(
            /* [in] */ Int32 c);

        /** Returns the number of exclusive holds represented in count  */
        static CARAPI_(Int32) ExclusiveCount(
            /* [in] */ Int32 c);

        /**
         * A counter for per-thread read hold counts.
         * Maintained as a ThreadLocal; cached in cachedHoldCounter
         */
        class HoldCounter : public Object
        {
        public:
            HoldCounter(){
                mCount = 0;
                Thread::GetCurrentThread()->GetId(&mTid);
            }

            Int32 mCount;
            // Use id, not reference, to avoid garbage retention
            Int64 mTid;
        };

        /**
         * ThreadLocal subclass. Easiest to explicitly define for sake
         * of deserialization mechanics.
         */
        class ThreadLocalHoldCounter : public Object
        { // extends ThreadLocal<HoldCounter> {
        public:
            ThreadLocalHoldCounter() {}

            AutoPtr<HoldCounter> InitialValue() {
                AutoPtr<HoldCounter> res = new HoldCounter();
                return res;
            }
        };

        Sync();

        /*
         * Acquires and releases use the same code for fair and
         * nonfair locks, but differ in whether/how they allow barging
         * when queues are non-empty.
         */

        /**
         * Returns true if the current thread, when trying to acquire
         * the read lock, and otherwise eligible to do so, should block
         * because of policy for overtaking other waiting threads.
         */
        virtual CARAPI_(Boolean) ReaderShouldBlock() = 0;

        /**
         * Returns true if the current thread, when trying to acquire
         * the write lock, and otherwise eligible to do so, should block
         * because of policy for overtaking other waiting threads.
         */
        virtual CARAPI_(Boolean) WriterShouldBlock() = 0;

        /**
         * Performs tryLock for write, enabling barging in both modes.
         * This is identical in effect to tryAcquire except for lack
         * of calls to writerShouldBlock.
         */
        CARAPI_(Boolean) TryWriteLock();

        /**
         * Performs tryLock for read, enabling barging in both modes.
         * This is identical in effect to tryAcquireShared except for
         * lack of calls to readerShouldBlock.
         */
        CARAPI_(Boolean) TryReadLock();

        // Methods relayed to outer class

        CARAPI_(AutoPtr<ICondition>) NewCondition();

        CARAPI_(AutoPtr<IThread>) GetOwner();

        CARAPI_(Int32) GetReadLockCount();

        CARAPI_(Boolean) IsWriteLocked();

        CARAPI_(Int32) GetWriteHoldCount();

        CARAPI_(Int32) GetReadHoldCount();

        CARAPI_(Int32) GetCount();

        CARAPI_(Boolean) IsHeldExclusively();

    protected:
        /*
         * Note that tryRelease and tryAcquire can be called by
         * Conditions. So it is possible that their arguments contain
         * both read and write holds that are all released during a
         * condition wait and re-established in tryAcquire.
         */

        CARAPI_(Boolean) TryRelease(
            /* [in] */ Int32 releases);

        CARAPI_(Boolean) TryAcquire(
            /* [in] */ Int32 acquires);

        CARAPI_(Boolean) TryReleaseShared(
            /* [in] */ Int32 unused);

        CARAPI_(Int32) TryAcquireShared(
            /* [in] */ Int32 unused);

        /**
         * Full version of acquire for reads, that handles CAS misses
         * and reentrant reads not dealt with in tryAcquireShared.
         */
        CARAPI_(Int32) FullTryAcquireShared(
            /* [in] */ IThread* current);

    private:
        /**
         * Reconstitutes this lock instance from a stream (that is,
         * deserializes it).
         *
         * @param s the stream
         */
        CARAPI_(void) ReadObject(
            /* [in] */ IObjectInputStream* s);

    public:
        /*
         * Read vs write count extraction constants and functions.
         * Lock state is logically divided into two unsigned shorts:
         * The lower one representing the exclusive (writer) lock hold count,
         * and the upper the shared (reader) hold count.
         */

        static const Int32 SHARED_SHIFT; //   = 16;
        static const Int32 SHARED_UNIT; //    = (1 << SHARED_SHIFT);
        static const Int32 MAX_COUNT; //      = (1 << SHARED_SHIFT) - 1;
        static const Int32 EXCLUSIVE_MASK; // = (1 << SHARED_SHIFT) - 1;

    private:
        static const Int64 sSerialVersionUID; // = 6317671515068378041L;

        /**
         * The number of reentrant read locks held by current thread.
         * Initialized only in constructor and readObject.
         * Removed whenever a thread's read hold count drops to 0.
         */
        /* transient */ AutoPtr<ThreadLocalHoldCounter> mReadHolds;

        /**
         * The hold count of the last thread to successfully acquire
         * readLock. This saves ThreadLocal lookup in the common case
         * where the next thread to release is the last one to
         * acquire. This is non-volatile since it is just used
         * as a heuristic, and would be great for threads to cache.
         *
         * <p>Can outlive the Thread for which it is caching the read
         * hold count, but avoids garbage retention by not retaining a
         * reference to the Thread.
         *
         * <p>Accessed via a benign data race; relies on the memory
         * model's final field and out-of-thin-air guarantees.
         */
        /* transient */ AutoPtr<HoldCounter> mCachedHoldCounter;

        /**
         * firstReader is the first thread to have acquired the read lock.
         * firstReaderHoldCount is firstReader's hold count.
         *
         * <p>More precisely, firstReader is the unique thread that last
         * changed the shared count from 0 to 1, and has not released the
         * read lock since then; null if there is no such thread.
         *
         * <p>Cannot cause garbage retention unless the thread terminated
         * without relinquishing its read locks, since tryReleaseShared
         * sets it to null.
         *
         * <p>Accessed via a benign data race; relies on the memory
         * model's out-of-thin-air guarantees for references.
         *
         * <p>This allows tracking of read holds for uncontended read
         * locks to be very cheap.
         */
        /* transient */ AutoPtr<IThread> mFirstReader;
        /* transient */ Int32 mFirstReaderHoldCount;
    };


    /**
     * Nonfair version of Sync
     */
    class NonfairSync : public Sync
    {
    public:
        NonfairSync(){}

        CARAPI_(Boolean) WriterShouldBlock() {
            return FALSE; // writers can always barge
        }

        CARAPI_(Boolean) ReaderShouldBlock() {
            /* As a heuristic to avoid indefinite writer starvation,
             * block if the thread that momentarily appears to be head
             * of queue, if one exists, is a waiting writer.  This is
             * only a probabilistic effect since a new reader will not
             * block if there is a waiting writer behind other enabled
             * readers that have not yet drained from the queue.
             */
            assert(0 && "TODO");
            // return ApparentlyFirstQueuedIsExclusive();
            return FALSE;
        }

    private:
        static const Int64 sSerialVersionUID = -8159625535654395037L;
    };

    /**
     * Fair version of Sync
     */
    class FairSync : public Sync
    {
    public:
        FairSync(){}

        CARAPI_(Boolean) WriterShouldBlock() {
            assert(0 && "TODO");
            // return HasQueuedPredecessors();
            return FALSE;
        }

        CARAPI_(Boolean) ReaderShouldBlock() {
            assert(0 && "TODO");
            // return HasQueuedPredecessors();
            return FALSE;
        }

    private:
        static const Int64 sSerialVersionUID = -2274990926593161451L;
    };

    /**
     * The lock returned by method {@link ReentrantReadWriteLock#readLock}.
     */
    class CReadLock
        : public Object
        , public ILock
        , public ISerializable
    {
    public:
        CAR_INTERFACE_DECL();
        /**
         * Constructor for use by subclasses
         *
         * @param lock the outer lock object
         * @throws NullPointerException if the lock is null
         */
        CReadLock(
            /* [in] */ CReentrantReadWriteLock* lock);

        /**
         * Acquires the read lock.
         *
         * <p>Acquires the read lock if the write lock is not held by
         * another thread and returns immediately.
         *
         * <p>If the write lock is held by another thread then
         * the current thread becomes disabled for thread scheduling
         * purposes and lies dormant until the read lock has been acquired.
         */
        CARAPI Lock();

        /**
         * Acquires the read lock unless the current thread is
         * {@linkplain Thread#interrupt interrupted}.
         *
         * <p>Acquires the read lock if the write lock is not held
         * by another thread and returns immediately.
         *
         * <p>If the write lock is held by another thread then the
         * current thread becomes disabled for thread scheduling
         * purposes and lies dormant until one of two things happens:
         *
         * <ul>
         *
         * <li>The read lock is acquired by the current thread; or
         *
         * <li>Some other thread {@linkplain Thread#interrupt interrupts}
         * the current thread.
         *
         * </ul>
         *
         * <p>If the current thread:
         *
         * <ul>
         *
         * <li>has its interrupted status set on entry to this method; or
         *
         * <li>is {@linkplain Thread#interrupt interrupted} while
         * acquiring the read lock,
         *
         * </ul>
         *
         * then {@link InterruptedException} is thrown and the current
         * thread's interrupted status is cleared.
         *
         * <p>In this implementation, as this method is an explicit
         * interruption point, preference is given to responding to
         * the interrupt over normal or reentrant acquisition of the
         * lock.
         *
         * @throws InterruptedException if the current thread is interrupted
         */
        CARAPI LockInterruptibly();

        /**
         * Acquires the read lock only if the write lock is not held by
         * another thread at the time of invocation.
         *
         * <p>Acquires the read lock if the write lock is not held by
         * another thread and returns immediately with the value
         * {@code true}. Even when this lock has been set to use a
         * fair ordering policy, a call to {@code tryLock()}
         * <em>will</em> immediately acquire the read lock if it is
         * available, whether or not other threads are currently
         * waiting for the read lock.  This &quot;barging&quot; behavior
         * can be useful in certain circumstances, even though it
         * breaks fairness. If you want to honor the fairness setting
         * for this lock, then use {@link #tryLock(long, TimeUnit)
         * tryLock(0, TimeUnit.SECONDS) } which is almost equivalent
         * (it also detects interruption).
         *
         * <p>If the write lock is held by another thread then
         * this method will return immediately with the value
         * {@code false}.
         *
         * @return {@code true} if the read lock was acquired
         */
        CARAPI TryLock(
            /* [out] */ Boolean* value);

        /**
         * Acquires the read lock if the write lock is not held by
         * another thread within the given waiting time and the
         * current thread has not been {@linkplain Thread#interrupt
         * interrupted}.
         *
         * <p>Acquires the read lock if the write lock is not held by
         * another thread and returns immediately with the value
         * {@code true}. If this lock has been set to use a fair
         * ordering policy then an available lock <em>will not</em> be
         * acquired if any other threads are waiting for the
         * lock. This is in contrast to the {@link #tryLock()}
         * method. If you want a timed {@code tryLock} that does
         * permit barging on a fair lock then combine the timed and
         * un-timed forms together:
         *
         *  <pre> {@code
         * if (lock.tryLock() ||
         *     lock.tryLock(timeout, unit)) {
         *   ...
         * }}</pre>
         *
         * <p>If the write lock is held by another thread then the
         * current thread becomes disabled for thread scheduling
         * purposes and lies dormant until one of three things happens:
         *
         * <ul>
         *
         * <li>The read lock is acquired by the current thread; or
         *
         * <li>Some other thread {@linkplain Thread#interrupt interrupts}
         * the current thread; or
         *
         * <li>The specified waiting time elapses.
         *
         * </ul>
         *
         * <p>If the read lock is acquired then the value {@code true} is
         * returned.
         *
         * <p>If the current thread:
         *
         * <ul>
         *
         * <li>has its interrupted status set on entry to this method; or
         *
         * <li>is {@linkplain Thread#interrupt interrupted} while
         * acquiring the read lock,
         *
         * </ul> then {@link InterruptedException} is thrown and the
         * current thread's interrupted status is cleared.
         *
         * <p>If the specified waiting time elapses then the value
         * {@code false} is returned.  If the time is less than or
         * equal to zero, the method will not wait at all.
         *
         * <p>In this implementation, as this method is an explicit
         * interruption point, preference is given to responding to
         * the interrupt over normal or reentrant acquisition of the
         * lock, and over reporting the elapse of the waiting time.
         *
         * @param timeout the time to wait for the read lock
         * @param unit the time unit of the timeout argument
         * @return {@code true} if the read lock was acquired
         * @throws InterruptedException if the current thread is interrupted
         * @throws NullPointerException if the time unit is null
         *
         */
        CARAPI TryLock(
          /* [in] */ Int64 time,
          /* [in] */ ITimeUnit* unit,
          /* [out] */ Boolean* value);

        /**
         * Attempts to release this lock.
         *
         * <p> If the number of readers is now zero then the lock
         * is made available for write lock attempts.
         */
        CARAPI UnLock();

        /**
         * Throws {@code UnsupportedOperationException} because
         * {@code ReadLocks} do not support conditions.
         *
         * @throws UnsupportedOperationException always
         */
        CARAPI NewCondition(
            /* [out] */ ICondition** cond);

        /**
         * Returns a string identifying this lock, as well as its lock state.
         * The state, in brackets, includes the String {@code "Read locks ="}
         * followed by the number of held read locks.
         *
         * @return a string identifying this lock, as well as its lock state
         */
        CARAPI ToString(
            /* [out] */ String* str);

    private:
        static const Int64 sSerialVersionUID = -5992448646407690164L;
        AutoPtr<Sync> mSync;
    };

    /**
     * The lock returned by method {@link ReentrantReadWriteLock#writeLock}.
     */
    class CWriteLock
        : public Object
        , public ILock
        , public ISerializable
    {
    public:
        CAR_INTERFACE_DECL();
        /**
         * Constructor for use by subclasses
         *
         * @param lock the outer lock object
         * @throws NullPointerException if the lock is null
         */
        CWriteLock(
            /* [in] */ CReentrantReadWriteLock* lock);

        /**
         * Acquires the write lock.
         *
         * <p>Acquires the write lock if neither the read nor write lock
         * are held by another thread
         * and returns immediately, setting the write lock hold count to
         * one.
         *
         * <p>If the current thread already holds the write lock then the
         * hold count is incremented by one and the method returns
         * immediately.
         *
         * <p>If the lock is held by another thread then the current
         * thread becomes disabled for thread scheduling purposes and
         * lies dormant until the write lock has been acquired, at which
         * time the write lock hold count is set to one.
         */
        CARAPI Lock();

        /**
         * Acquires the write lock unless the current thread is
         * {@linkplain Thread#interrupt interrupted}.
         *
         * <p>Acquires the write lock if neither the read nor write lock
         * are held by another thread
         * and returns immediately, setting the write lock hold count to
         * one.
         *
         * <p>If the current thread already holds this lock then the
         * hold count is incremented by one and the method returns
         * immediately.
         *
         * <p>If the lock is held by another thread then the current
         * thread becomes disabled for thread scheduling purposes and
         * lies dormant until one of two things happens:
         *
         * <ul>
         *
         * <li>The write lock is acquired by the current thread; or
         *
         * <li>Some other thread {@linkplain Thread#interrupt interrupts}
         * the current thread.
         *
         * </ul>
         *
         * <p>If the write lock is acquired by the current thread then the
         * lock hold count is set to one.
         *
         * <p>If the current thread:
         *
         * <ul>
         *
         * <li>has its interrupted status set on entry to this method;
         * or
         *
         * <li>is {@linkplain Thread#interrupt interrupted} while
         * acquiring the write lock,
         *
         * </ul>
         *
         * then {@link InterruptedException} is thrown and the current
         * thread's interrupted status is cleared.
         *
         * <p>In this implementation, as this method is an explicit
         * interruption point, preference is given to responding to
         * the interrupt over normal or reentrant acquisition of the
         * lock.
         *
         * @throws InterruptedException if the current thread is interrupted
         */
        CARAPI LockInterruptibly();

        /**
         * Acquires the write lock only if it is not held by another thread
         * at the time of invocation.
         *
         * <p>Acquires the write lock if neither the read nor write lock
         * are held by another thread
         * and returns immediately with the value {@code true},
         * setting the write lock hold count to one. Even when this lock has
         * been set to use a fair ordering policy, a call to
         * {@code tryLock()} <em>will</em> immediately acquire the
         * lock if it is available, whether or not other threads are
         * currently waiting for the write lock.  This &quot;barging&quot;
         * behavior can be useful in certain circumstances, even
         * though it breaks fairness. If you want to honor the
         * fairness setting for this lock, then use {@link
         * #tryLock(long, TimeUnit) tryLock(0, TimeUnit.SECONDS) }
         * which is almost equivalent (it also detects interruption).
         *
         * <p> If the current thread already holds this lock then the
         * hold count is incremented by one and the method returns
         * {@code true}.
         *
         * <p>If the lock is held by another thread then this method
         * will return immediately with the value {@code false}.
         *
         * @return {@code true} if the lock was free and was acquired
         * by the current thread, or the write lock was already held
         * by the current thread; and {@code false} otherwise.
         */
        CARAPI TryLock(
            /* [out] */ Boolean* value);

        /**
         * Acquires the write lock if it is not held by another thread
         * within the given waiting time and the current thread has
         * not been {@linkplain Thread#interrupt interrupted}.
         *
         * <p>Acquires the write lock if neither the read nor write lock
         * are held by another thread
         * and returns immediately with the value {@code true},
         * setting the write lock hold count to one. If this lock has been
         * set to use a fair ordering policy then an available lock
         * <em>will not</em> be acquired if any other threads are
         * waiting for the write lock. This is in contrast to the {@link
         * #tryLock()} method. If you want a timed {@code tryLock}
         * that does permit barging on a fair lock then combine the
         * timed and un-timed forms together:
         *
         *  <pre> {@code
         * if (lock.tryLock() ||
         *     lock.tryLock(timeout, unit)) {
         *   ...
         * }}</pre>
         *
         * <p>If the current thread already holds this lock then the
         * hold count is incremented by one and the method returns
         * {@code true}.
         *
         * <p>If the lock is held by another thread then the current
         * thread becomes disabled for thread scheduling purposes and
         * lies dormant until one of three things happens:
         *
         * <ul>
         *
         * <li>The write lock is acquired by the current thread; or
         *
         * <li>Some other thread {@linkplain Thread#interrupt interrupts}
         * the current thread; or
         *
         * <li>The specified waiting time elapses
         *
         * </ul>
         *
         * <p>If the write lock is acquired then the value {@code true} is
         * returned and the write lock hold count is set to one.
         *
         * <p>If the current thread:
         *
         * <ul>
         *
         * <li>has its interrupted status set on entry to this method;
         * or
         *
         * <li>is {@linkplain Thread#interrupt interrupted} while
         * acquiring the write lock,
         *
         * </ul>
         *
         * then {@link InterruptedException} is thrown and the current
         * thread's interrupted status is cleared.
         *
         * <p>If the specified waiting time elapses then the value
         * {@code false} is returned.  If the time is less than or
         * equal to zero, the method will not wait at all.
         *
         * <p>In this implementation, as this method is an explicit
         * interruption point, preference is given to responding to
         * the interrupt over normal or reentrant acquisition of the
         * lock, and over reporting the elapse of the waiting time.
         *
         * @param timeout the time to wait for the write lock
         * @param unit the time unit of the timeout argument
         *
         * @return {@code true} if the lock was free and was acquired
         * by the current thread, or the write lock was already held by the
         * current thread; and {@code false} if the waiting time
         * elapsed before the lock could be acquired.
         *
         * @throws InterruptedException if the current thread is interrupted
         * @throws NullPointerException if the time unit is null
         *
         */
        CARAPI TryLock(
            /* [in] */ Int64 timeout,
            /* [in] */ ITimeUnit* unit,
            /* [out] */ Boolean* value);

        /**
         * Attempts to release this lock.
         *
         * <p>If the current thread is the holder of this lock then
         * the hold count is decremented. If the hold count is now
         * zero then the lock is released.  If the current thread is
         * not the holder of this lock then {@link
         * IllegalMonitorStateException} is thrown.
         *
         * @throws IllegalMonitorStateException if the current thread does not
         * hold this lock.
         */
        CARAPI UnLock();

        /**
         * Returns a {@link Condition} instance for use with this
         * {@link Lock} instance.
         * <p>The returned {@link Condition} instance supports the same
         * usages as do the {@link Object} monitor methods ({@link
         * Object#wait() wait}, {@link Object#notify notify}, and {@link
         * Object#notifyAll notifyAll}) when used with the built-in
         * monitor lock.
         *
         * <ul>
         *
         * <li>If this write lock is not held when any {@link
         * Condition} method is called then an {@link
         * IllegalMonitorStateException} is thrown.  (Read locks are
         * held independently of write locks, so are not checked or
         * affected. However it is essentially always an error to
         * invoke a condition waiting method when the current thread
         * has also acquired read locks, since other threads that
         * could unblock it will not be able to acquire the write
         * lock.)
         *
         * <li>When the condition {@linkplain Condition#await() waiting}
         * methods are called the write lock is released and, before
         * they return, the write lock is reacquired and the lock hold
         * count restored to what it was when the method was called.
         *
         * <li>If a thread is {@linkplain Thread#interrupt interrupted} while
         * waiting then the wait will terminate, an {@link
         * InterruptedException} will be thrown, and the thread's
         * interrupted status will be cleared.
         *
         * <li> Waiting threads are signalled in FIFO order.
         *
         * <li>The ordering of lock reacquisition for threads returning
         * from waiting methods is the same as for threads initially
         * acquiring the lock, which is in the default case not specified,
         * but for <em>fair</em> locks favors those threads that have been
         * waiting the longest.
         *
         * </ul>
         *
         * @return the Condition object
         */
        CARAPI NewCondition(
            /* [out] */ ICondition** con);

        /**
         * Returns a string identifying this lock, as well as its lock
         * state.  The state, in brackets includes either the String
         * {@code "UnLocked"} or the String {@code "Locked by"}
         * followed by the {@linkplain Thread#getName name} of the owning thread.
         *
         * @return a string identifying this lock, as well as its lock state
         */
        CARAPI ToString(
            /* [out] */ String* str);

        /**
         * Queries if this write lock is held by the current thread.
         * Identical in effect to {@link
         * ReentrantReadWriteLock#isWriteLockedByCurrentThread}.
         *
         * @return {@code true} if the current thread holds this lock and
         *         {@code false} otherwise
         * @since 1.6
         */
        CARAPI IsHeldByCurrentThread(
            /* [out] */ Boolean* value);

        /**
         * Queries the number of holds on this write lock by the current
         * thread.  A thread has a hold on a lock for each lock action
         * that is not matched by an unlock action.  Identical in effect
         * to {@link ReentrantReadWriteLock#getWriteHoldCount}.
         *
         * @return the number of holds on this lock by the current thread,
         *         or zero if this lock is not held by the current thread
         * @since 1.6
         */
        CARAPI GetHoldCount(
            /* [out] */ Int32* value);

    private:
        static const Int64 sSerialVersionUID = -4992448646407690164L;
        AutoPtr<Sync> mSync;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()
    /**
     * Creates a new {@code ReentrantReadWriteLock} with
     * default (nonfair) ordering properties.
     */
    CARAPI constructor();

    /**
     * Creates a new {@code ReentrantReadWriteLock} with
     * the given fairness policy.
     *
     * @param fair {@code true} if this lock should use a fair ordering policy
     */
    CARAPI constructor(
        /* [in] */ Boolean fair);

    /**
     * Returns the lock used for reading.
     *
     * @return the lock used for reading.
     */
    CARAPI ReadLock(
        /* [out] */ ILock** outlock);

    /**
     * Returns the lock used for writing.
     *
     * @return the lock used for writing.
     */
    CARAPI WriteLock(
        /* [out] */ ILock** outlock);

    /**
     * Returns {@code true} if this lock has fairness set true.
     *
     * @return {@code true} if this lock has fairness set true
     */
    CARAPI IsFair(
        /* [out] */ Boolean* value);

    /**
     * Queries the number of read locks held for this lock. This
     * method is designed for use in monitoring system state, not for
     * synchronization control.
     * @return the number of read locks held.
     */
    CARAPI GetReadLockCount(
        /* [out] */ Int32* value);

    /**
     * Queries if the write lock is held by any thread. This method is
     * designed for use in monitoring system state, not for
     * synchronization control.
     *
     * @return {@code true} if any thread holds the write lock and
     *         {@code false} otherwise
     */
    CARAPI IsWriteLocked(
        /* [out] */ Boolean* value);

    /**
     * Queries if the write lock is held by the current thread.
     *
     * @return {@code true} if the current thread holds the write lock and
     *         {@code false} otherwise
     */
    CARAPI IsWriteLockedByCurrentThread(
        /* [out] */ Boolean* value);

    /**
     * Queries the number of reentrant write holds on this lock by the
     * current thread.  A writer thread has a hold on a lock for
     * each lock action that is not matched by an unlock action.
     *
     * @return the number of holds on the write lock by the current thread,
     *         or zero if the write lock is not held by the current thread
     */
    CARAPI GetWriteHoldCount(
        /* [out] */ Int32* value);

    /**
     * Queries the number of reentrant read holds on this lock by the
     * current thread.  A reader thread has a hold on a lock for
     * each lock action that is not matched by an unlock action.
     *
     * @return the number of holds on the read lock by the current thread,
     *         or zero if the read lock is not held by the current thread
     * @since 1.6
     */
    CARAPI GetReadHoldCount(
        /* [out] */ Int32* value);

    /**
     * Queries whether any threads are waiting to acquire the read or
     * write lock. Note that because cancellations may occur at any
     * time, a {@code true} return does not guarantee that any other
     * thread will ever acquire a lock.  This method is designed
     * primarily for use in monitoring of the system state.
     *
     * @return {@code true} if there may be other threads waiting to
     *         acquire the lock
     */
    CARAPI HasQueuedThreads(
        /* [out] */ Boolean* value);

    /**
     * Queries whether the given thread is waiting to acquire either
     * the read or write lock. Note that because cancellations may
     * occur at any time, a {@code true} return does not guarantee
     * that this thread will ever acquire a lock.  This method is
     * designed primarily for use in monitoring of the system state.
     *
     * @param thread the thread
     * @return {@code true} if the given thread is queued waiting for this lock
     * @throws NullPointerException if the thread is null
     */
    CARAPI HasQueuedThread(
        /* [in] */ IThread* thread,
        /* [out] */ Boolean* value);

    /**
     * Returns an estimate of the number of threads waiting to acquire
     * either the read or write lock.  The value is only an estimate
     * because the number of threads may change dynamically while this
     * method traverses internal data structures.  This method is
     * designed for use in monitoring of the system state, not for
     * synchronization control.
     *
     * @return the estimated number of threads waiting for this lock
     */
    CARAPI GetQueueLength(
        /* [out] */ Int32* value);

    /**
     * Queries whether any threads are waiting on the given condition
     * associated with the write lock. Note that because timeouts and
     * interrupts may occur at any time, a {@code true} return does
     * not guarantee that a future {@code signal} will awaken any
     * threads.  This method is designed primarily for use in
     * monitoring of the system state.
     *
     * @param condition the condition
     * @return {@code true} if there are any waiting threads
     * @throws IllegalMonitorStateException if this lock is not held
     * @throws IllegalArgumentException if the given condition is
     *         not associated with this lock
     * @throws NullPointerException if the condition is null
     */
    CARAPI HasWaiters(
        /* [in] */ ICondition* condition,
        /* [out] */ Boolean* value);

    /**
     * Returns an estimate of the number of threads waiting on the
     * given condition associated with the write lock. Note that because
     * timeouts and interrupts may occur at any time, the estimate
     * serves only as an upper bound on the actual number of waiters.
     * This method is designed for use in monitoring of the system
     * state, not for synchronization control.
     *
     * @param condition the condition
     * @return the estimated number of waiting threads
     * @throws IllegalMonitorStateException if this lock is not held
     * @throws IllegalArgumentException if the given condition is
     *         not associated with this lock
     * @throws NullPointerException if the condition is null
     */
    CARAPI GetWaitQueueLength(
        /* [in] */ ICondition* condition,
        /* [out] */ Int32* value);

    /**
     * Returns a string identifying this lock, as well as its lock state.
     * The state, in brackets, includes the String {@code "Write locks ="}
     * followed by the number of reentrantly held write locks, and the
     * String {@code "Read locks ="} followed by the number of held
     * read locks.
     *
     * @return a string identifying this lock, as well as its lock state
     */
    CARAPI ToString(
        /* [out] */ String* str);

protected:
    CARAPI_(AutoPtr<IThread>) GetOwner();

    CARAPI_(AutoPtr<ICollection>) GetQueuedWriterThreads();

    CARAPI_(AutoPtr<ICollection>) GetQueuedReaderThreads();

    CARAPI_(AutoPtr<ICollection>) GetQueuedThreads();

    CARAPI_(AutoPtr<ICollection>) GetWaitingThreads(
        /* [in] */ ICondition* condition);

private:
    static const Int64 sSerialVersionUID; // = -6992448646407690164L;

    /** Inner class providing readlock */
    AutoPtr<CReadLock> mReaderLock;
    /** Inner class providing writelock */
    AutoPtr<CWriteLock> mWriterLock;

    /** Performs all synchronization mechanics */
    AutoPtr<Sync> mSync;
};

} // namespace Locks
} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_LOCKS_ELASTOS_UTILITY_CREENTRANTREADWRITELOCK_H__
