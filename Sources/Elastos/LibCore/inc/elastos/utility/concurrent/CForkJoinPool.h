
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CFORKJOINPOOL_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CFORKJOINPOOL_H__

#include "_Elastos_Utility_Concurrent_CForkJoinPool.h"
#include "AbstractExecutorService.h"
#include "ForkJoinTask.h"

using Elastos::Core::IThread;
using Elastos::Core::IThreadUncaughtExceptionHandler;
using Elastos::Core::IThrowable;
using Elastos::Utility::IRandom;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Locks::IReentrantLock;
using Elastos::Utility::Concurrent::Locks::ICondition;

namespace Elastos {
namespace Utility {
namespace Concurrent {

CarClass(CForkJoinPool)
    , public AbstractExecutorService
    , public IForkJoinPool
{
public:
    // Nested classes
    /**
     * Default ForkJoinWorkerThreadFactory implementation; creates a
     * new ForkJoinWorkerThread.
     */
    class DefaultForkJoinWorkerThreadFactory
        : public Object
        , public IForkJoinPoolForkJoinWorkerThreadFactory
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI_(AutoPtr<IForkJoinWorkerThread>) NewThread(
            /* [in] */ IForkJoinPool* pool);
    };

    /**
     * Class for artificial tasks that are used to replace the target
     * of local joins if they are removed from an interior queue slot
     * in WorkQueue.tryRemoveAndExec. We don't need the proxy to
     * actually do anything beyond having a unique identity.
     */
    class EmptyTask
        : public ForkJoinTask
    {
    public:
        EmptyTask();

        CARAPI SetRawResult(
            /* [in] */ IInterface* value);

        CARAPI Exec(
            /* [out] */ Boolean* res);

        CARAPI GetRawResult(
            /* [out] */ IInterface** outface);

    private:
//        static long serialVersionUID = -7721805057305804111L;
    };

    /**
     * Queues supporting work-stealing as well as external task
     * submission. See above for main rationale and algorithms.
     * Implementation relies heavily on "Unsafe" intrinsics
     * and selective use of "volatile":
     *
     * Field "base" is the index (mod array.length) of the least valid
     * queue slot, which is always the next position to steal (poll)
     * from if nonempty. Reads and writes require volatile orderings
     * but not CAS, because updates are only performed after slot
     * CASes.
     *
     * Field "top" is the index (mod array.length) of the next queue
     * slot to push to or pop from. It is written only by owner thread
     * for push, or under lock for external/shared push, and accessed
     * by other threads only after reading (volatile) base.  Both top
     * and base are allowed to wrap around on overflow, but (top -
     * base) (or more commonly -(base - top) to force volatile read of
     * base before top) still estimates size. The lock ("qlock") is
     * forced to -1 on termination, causing all further lock attempts
     * to fail. (Note: we don't need CAS for termination state because
     * upon pool shutdown, all shared-queues will stop being used
     * anyway.)  Nearly all lock bodies are set up so that exceptions
     * within lock bodies are "impossible" (modulo JVM errors that
     * would cause failure anyway.)
     *
     * The array slots are read and written using the emulation of
     * volatiles/atomics provided by Unsafe. Insertions must in
     * general use putOrderedObject as a form of releasing store to
     * ensure that all writes to the task object are ordered before
     * its publication in the queue.  All removals entail a CAS to
     * null.  The array is always a power of two. To ensure safety of
     * Unsafe array operations, all accesses perform explicit null
     * checks and implicit bounds checks via power-of-two masking.
     *
     * In addition to basic queuing support, this class contains
     * fields described elsewhere to control execution. It turns out
     * to work better memory-layout-wise to include them in this class
     * rather than a separate class.
     *
     * Performance on most platforms is very sensitive to placement of
     * instances of both WorkQueues and their arrays -- we absolutely
     * do not want multiple WorkQueue instances or multiple queue
     * arrays sharing cache lines. (It would be best for queue objects
     * and their arrays to share, but there is nothing available to
     * help arrange that). The @Contended annotation alerts JVMs to
     * try to keep instances apart.
     */
    class WorkQueue
        : public Object
    {
    public:
        WorkQueue(
            /* [in] */ IForkJoinPool* pool,
            /* [in] */ IForkJoinWorkerThread* owner,
            /* [in] */ Int32 mode,
            /* [in] */ Int32 seed);

        /**
         * Returns the approximate number of tasks in the queue.
         */
        CARAPI_(Int32) QueueSize();

        /**
         * Provides a more accurate estimate of whether this queue has
         * any tasks than does queueSize, by checking whether a
         * near-empty queue has at least one unclaimed task.
         */
        CARAPI_(Boolean) IsEmpty();

        /**
         * Pushes a task. Call only by owner in unshared queues.  (The
         * shared-queue version is embedded in method externalPush.)
         *
         * @param task the task. Caller must ensure non-null.
         * @throws RejectedExecutionException if array cannot be resized
         */
        CARAPI_(void) Push(
            /* [in] */ IForkJoinTask* task);

        /**
         * Initializes or doubles the capacity of array. Call either
         * by owner or with lock held -- it is OK for base, but not
         * top, to move while resizings are in progress.
         */
        CARAPI_(AutoPtr<ArrayOf<IForkJoinTask*> >) GrowArray();

        /**
         * Takes next task, if one exists, in LIFO order.  Call only
         * by owner in unshared queues.
         */
        CARAPI_(AutoPtr<IForkJoinTask>) Pop();

        /**
         * Takes a task in FIFO order if b is base of queue and a task
         * can be claimed without contention. Specialized versions
         * appear in ForkJoinPool methods scan and tryHelpStealer.
         */
        CARAPI_(AutoPtr<IForkJoinTask>) PollAt(
            /* [in] */ Int32 b);

        /**
         * Takes next task, if one exists, in FIFO order.
         */
        CARAPI_(AutoPtr<IForkJoinTask>) Poll();

        /**
         * Takes next task, if one exists, in order specified by mode.
         */
        CARAPI_(AutoPtr<IForkJoinTask>) NextLocalTask();

        /**
         * Returns next task, if one exists, in order specified by mode.
         */
        CARAPI_(AutoPtr<IForkJoinTask>) Peek();

        /**
         * Pops the given task only if it is at the current top.
         * (A shared version is available only via FJP.tryExternalUnpush)
         */
        CARAPI_(Boolean) TryUnpush(
            /* [in] */ IForkJoinTask* t);

        /**
         * Removes and cancels all known tasks, ignoring any exceptions.
         */
        CARAPI_(void) CancelAll();

        // Specialized execution methods

        /**
         * Polls and runs tasks until empty.
         */
        CARAPI_(void) PollAndExecAll();

        /**
         * Executes a top-level task and any local tasks remaining
         * after execution.
         */
        CARAPI_(void) RunTask(
            /* [in] */ IForkJoinTask* task);

        /**
         * If present, removes from queue and executes the given task,
         * or any other cancelled task. Returns (true) on any CAS
         * or consistency check failure so caller can retry.
         *
         * @return false if no progress can be made, else true
         */
        CARAPI_(Boolean) TryRemoveAndExec(
            /* [in] */ IForkJoinTask* task);

        /**
         * Tries to poll for and execute the given task or any other
         * task in its CountedCompleter computation.
         */
        CARAPI_(Boolean) PollAndExecCC(
            /* [in] */ ICountedCompleter* root);

        /**
         * Tries to pop and execute the given task or any other task
         * in its CountedCompleter computation.
         */
        CARAPI_(Boolean) ExternalPopAndExecCC(
            /* [in] */ ICountedCompleter* root);

        /**
         * Internal version
         */
        CARAPI_(Boolean) InternalPopAndExecCC(
            /* [in] */ ICountedCompleter* root);

        /**
         * Returns true if owned and not known to be blocked.
         */
        CARAPI_(Boolean) IsApparentlyUnblocked();

    public:
        /**
         * Capacity of work-stealing queue array upon initialization.
         * Must be a power of two; at least 4, but should be larger to
         * reduce or eliminate cacheline sharing among queues.
         * Currently, it is much larger, as a partial workaround for
         * the fact that JVMs often place arrays in locations that
         * share GC bookkeeping (especially cardmarks) such that
         * per-write accesses encounter serious memory contention.
         */
        static Int32 INITIAL_QUEUE_CAPACITY;

        /**
         * Maximum size for queue arrays. Must be a power of two less
         * than or equal to 1 << (31 - width of array entry) to ensure
         * lack of wraparound of index calculations, but defined to a
         * value a bit less than this to help users trap runaway
         * programs before saturating systems.
         */
        static Int32 MAXIMUM_QUEUE_CAPACITY;

        // Heuristic padding to ameliorate unfortunate memory placements
        Int64 mPad00, mPad01, mPad02, mPad03, mPad04, mPad05, mPad06;

        Int32 mEventCount;   // encoded inactivation count; < 0 if inactive
        Int32 mNextWait;              // encoded record of next event waiter
        Int32 mNsteals;               // number of steals
        Int32 mHint;                  // steal index hint
        Int16 mPoolIndex;           // index of this queue in pool
        Int16 mMode;          // 0: lifo, > 0: fifo, < 0: shared
        Int32 mQlock;        // 1: locked, -1: terminate; else 0
        Int32 mBase;         // index of next slot for poll
        Int32 mTop;                   // index of next slot for push
        AutoPtr<ArrayOf<IForkJoinTask*> > mArray;   // the elements (initially unallocated)
        AutoPtr<IForkJoinPool> mPool;   // the containing pool (may be null)
        AutoPtr<IForkJoinWorkerThread> mOwner; // owning thread or null if shared
        AutoPtr<IThread> mParker;    // == owner during call to park; else null
        AutoPtr<IForkJoinTask> mCurrentJoin;  // task being joined in awaitJoin
        AutoPtr<IForkJoinTask> mCurrentSteal; // current non-local task being executed

        AutoPtr<IInterface> mPad10, mPad11, mPad12, mPad13, mPad14, mPad15, mPad16, mPad17;
        AutoPtr<IInterface> mPad18, mPad19, mPad1a, mPad1b, mPad1c, mPad1d;

        // // Unsafe mechanics
        // private static final sun.misc.Unsafe U;
        // private static final Int64 QBASE;
        // private static final Int64 QLOCK;
        // private static final Int32 ABASE;
        // private static final Int32 ASHIFT;
        // static {
        //     try {
        //         U = sun.misc.Unsafe.getUnsafe();
        //         Class<?> k = WorkQueue.class;
        //         Class<?> ak = ForkJoinTask[].class;
        //         QBASE = U.objectFieldOffset
        //             (k.getDeclaredField("base"));
        //         QLOCK = U.objectFieldOffset
        //             (k.getDeclaredField("qlock"));
        //         ABASE = U.arrayBaseOffset(ak);
        //         Int32 scale = U.arrayIndexScale(ak);
        //         if ((scale & (scale - 1)) != 0)
        //             throw new Error("data type scale not a power of two");
        //         ASHIFT = 31 - Integer.numberOfLeadingZeros(scale);
        //     } catch (Exception e) {
        //         throw new Error(e);
        //     }
        // }
    };

    /**
     * Per-thread records for threads that submit to pools. Currently
     * holds only pseudo-random seed / index that is used to choose
     * submission queues in method externalPush. In the future, this may
     * also incorporate a means to implement different task rejection
     * and resubmission policies.
     *
     * Seeds for submitters and workers/workQueues work in basically
     * the same way but are initialized and updated using slightly
     * different mechanics. Both are initialized using the same
     * approach as in class ThreadLocal, where successive values are
     * unlikely to collide with previous values. Seeds are then
     * randomly modified upon collisions using xorshifts, which
     * requires a non-zero seed.
     */
    class Submitter
        : public Object
    {
    public:
        Submitter(
            /* [in] */ Int32 s) {
            mSeed = s;
        }

    public:
        Int32 mSeed;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    // Exported methods

    // Constructors

    /**
     * Creates a {@code ForkJoinPool} with parallelism equal to {@link
     * java.lang.Runtime#availableProcessors}, using the {@linkplain
     * #defaultForkJoinWorkerThreadFactory default thread factory},
     * no UncaughtExceptionHandler, and non-async LIFO processing mode.
     */
    CARAPI constructor();

    /**
     * Creates a {@code ForkJoinPool} with the indicated parallelism
     * level, the {@linkplain
     * #defaultForkJoinWorkerThreadFactory default thread factory},
     * no UncaughtExceptionHandler, and non-async LIFO processing mode.
     *
     * @param parallelism the parallelism level
     * @throws IllegalArgumentException if parallelism less than or
     *         equal to zero, or greater than implementation limit
     */
    CARAPI constructor(
        /* [in] */ const Int32& parallelism);

    /**
     * Creates a {@code ForkJoinPool} with the given parameters.
     *
     * @param parallelism the parallelism level. For default value,
     * use {@link java.lang.Runtime#availableProcessors}.
     * @param factory the factory for creating new threads. For default value,
     * use {@link #defaultForkJoinWorkerThreadFactory}.
     * @param handler the handler for internal worker threads that
     * terminate due to unrecoverable errors encountered while executing
     * tasks. For default value, use {@code null}.
     * @param asyncMode if true,
     * establishes local first-in-first-out scheduling mode for forked
     * tasks that are never joined. This mode may be more appropriate
     * than default locally stack-based mode in applications in which
     * worker threads only process event-style asynchronous tasks.
     * For default value, use {@code false}.
     * @throws IllegalArgumentException if parallelism less than or
     *         equal to zero, or greater than implementation limit
     * @throws NullPointerException if the factory is null
     */
    CARAPI constructor(
        /* [in] */ const Int32& parallelism,
        /* [in] */ IForkJoinPoolForkJoinWorkerThreadFactory* factory,
        /* [in] */ IThreadUncaughtExceptionHandler* handler,
        /* [in] */ const Boolean& asyncMode);

    CARAPI constructor(
        /* [in] */ Int32 parallelism,
        /* [in] */ IForkJoinPoolForkJoinWorkerThreadFactory* factory,
        /* [in] */ IThreadUncaughtExceptionHandler* handler,
        /* [in] */ Int32 mode,
        /* [in] */ String workerNamePrefix);

    // Execution methods

    CARAPI Invoke(
        /* [in] */ IForkJoinTask* task,
        /* [out] */ IInterface** outface);

    CARAPI Execute(
        /* [in] */ IForkJoinTask* task);

    // AbstractExecutorService methods

    CARAPI Execute(
        /* [in] */ IRunnable* command);

    CARAPI Submit(
        /* [in] */ IForkJoinTask* task,
        /* [out] */ IForkJoinTask** outfork);

    CARAPI Submit(
        /* [in] */ ICallable* task,
        /* [out] */ IFuture** outfork);

    CARAPI Submit(
        /* [in] */ IRunnable* task,
        /* [in] */ IInterface* result,
        /* [out] */ IFuture** outfork);

    CARAPI Submit(
        /* [in] */ IRunnable* task,
        /* [out] */ IFuture** outfork);

    /**
     * Returns the factory used for constructing new workers.
     *
     * @return the factory used for constructing new workers
     */
    CARAPI GetFactory(
        /* [out] */ IForkJoinPoolForkJoinWorkerThreadFactory** res);

    /**
     * Returns the handler for internal worker threads that terminate
     * due to unrecoverable errors encountered while executing tasks.
     *
     * @return the handler, or {@code null} if none
     */
    CARAPI GetUncaughtExceptionHandler(
        /* [out] */ IThreadUncaughtExceptionHandler** res);

    CARAPI GetParallelism(
        /* [out] */ Int32* value);

    CARAPI GetPoolSize(
        /* [out] */ Int32* value);

    CARAPI GetAsyncMode(
        /* [out] */ Boolean* value);

    CARAPI GetRunningThreadCount(
        /* [out] */ Int32* value);

    CARAPI GetActiveThreadCount(
        /* [out] */ Int32* value);

    CARAPI IsQuiescent(
        /* [out] */ Boolean* value);

    CARAPI GetStealCount(
        /* [out] */ Int64* value);

    CARAPI GetQueuedTaskCount(
        /* [out] */ Int64* value);

    CARAPI GetQueuedSubmissionCount(
        /* [out] */ Int32* value);

    CARAPI HasQueuedSubmissions(
        /* [out] */ Boolean* value);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI Shutdown();

    CARAPI ShutdownNow(
        /* [out] */ IList** tasks);

    CARAPI IsTerminated(
        /* [out] */ Boolean* result);

    CARAPI IsTerminating(
        /* [out] */ Boolean* value);

    CARAPI IsShutdown(
        /* [out] */ Boolean* result);

    CARAPI AwaitTermination(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* result);

    CARAPI InvokeAll(
        /* [in] */ ICollection* tasks,
        /* [out] */ IList** futures);

    CARAPI AwaitQuiescence(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ Boolean* result);

    /**
     * Returns the targeted parallelism level of the common pool.
     *
     * @return the targeted parallelism level of the common pool
     * @since 1.8
     * @hide
     */
    static CARAPI_(Int32) GetCommonPoolParallelism();

    /**
     * Removes all available unexecuted submitted and forked tasks
     * from scheduling queues and adds them to the given collection,
     * without altering their execution status. These may include
     * artificially generated or wrapped tasks. This method is
     * designed to be invoked only when the pool is known to be
     * quiescent. Invocations at other times may not remove all
     * tasks. A failure encountered while attempting to add elements
     * to collection {@code c} may result in elements being in
     * neither, either or both collections when the associated
     * exception is thrown.  The behavior of this operation is
     * undefined if the specified collection is modified while the
     * operation is in progress.
     *
     * @param c the collection to transfer elements into
     * @return the number of elements transferred
     */
    CARAPI_(Int32) DrainTasksTo(
        /* [in] */ ICollection* c);

    /**
     * Removes and returns the next unexecuted submission if one is
     * available.  This method may be useful in extensions to this
     * class that re-assign work in systems with multiple pools.
     *
     * @return the next submission, or {@code null} if none
     */
    CARAPI_(AutoPtr<IForkJoinTask>) PollSubmission();

    CARAPI_(AutoPtr<IRunnableFuture>) NewTaskFor(
        /* [in] */ IRunnable* runnable,
        /* [in] */ IInterface* value);

    CARAPI_(AutoPtr<IRunnableFuture>) NewTaskFor(
        /* [in] */ ICallable* callable);

    /**
     * Callback from ForkJoinWorkerThread constructor to
     * determine its poolIndex and record in workers array.
     *
     * @param w the worker
     * @return the worker's pool index
     */
    CARAPI_(AutoPtr<WorkQueue>) RegisterWorker(
        /* [in] */ IForkJoinWorkerThread* wt);

    /**
     * Final callback from terminating worker.  Removes record of
     * worker from array, and adjusts counts. If pool is shutting
     * down, tries to complete termination.
     *
     * @param w the worker
     */
    CARAPI_(void) DeregisterWorker(
        /* [in] */ IForkJoinWorkerThread* w,
        /* [in] */ IThrowable* ex);

    /**
     * Unless shutting down, adds the given task to a submission queue
     * at submitter's current queue index (modulo submission
     * range). Only the most common path is directly handled in this
     * method. All others are relayed to fullExternalPush.
     *
     * @param task the task. Caller must ensure non-null.
     */
    CARAPI_(void) ExternalPush(
        /* [in] */ IForkJoinTask* task);

    /**
     * Increments active count; mainly called upon return from blocking.
     */
    CARAPI_(void) IncrementActiveCount();

    /**
     * Wakes up or creates a worker.
     */
    CARAPI_(void) SignalWork(
        /* [in] */ ArrayOf<WorkQueue*>* ws,
        /* [in] */ WorkQueue* q);

    /**
     * Top-level runloop for workers, called by ForkJoinWorkerThread.run.
     */
    CARAPI_(void) RunWorker(
        /* [in] */ WorkQueue* w);

    /**
     * Blocks in accord with the given blocker.  If the current thread
     * is a {@link ForkJoinWorkerThread}, this method possibly
     * arranges for a spare thread to be activated if necessary to
     * ensure sufficient parallelism while the current thread is blocked.
     *
     * <p>If the caller is not a {@link ForkJoinTask}, this method is
     * behaviorally equivalent to
     *  <pre> {@code
     * while (!blocker.isReleasable())
     *   if (blocker.block())
     *     return;
     * }</pre>
     *
     * If the caller is a {@code ForkJoinTask}, then the pool may
     * first be expanded to ensure parallelism, and later adjusted.
     *
     * @param blocker the blocker
     * @throws InterruptedException if blocker.block did so
     */
    static CARAPI_(void) ManagedBlock(
        /* [in] */ IForkJoinPoolManagedBlocker* blocker);

    /**
     * Waits and/or attempts to assist performing tasks indefinitely
     * until the {@code commonPool()} {@link #isQuiescent}.
     */
    static CARAPI_(void) QuiesceCommonPool();

    /**
     * Tries to decrement active count (sometimes implicitly) and
     * possibly release or create a compensating worker in preparation
     * for blocking. Fails on contention or termination. Otherwise,
     * adds a new thread if no idle workers are available and pool
     * may become starved.
     *
     * @param c the assumed ctl value
     */
    CARAPI_(Boolean) TryCompensate(
        /* [in] */ Int64 c);

    /**
     * Helps and/or blocks until the given task is done.
     *
     * @param joiner the joining worker
     * @param task the task
     * @return task status on exit
     */
    CARAPI_(Int32) AwaitJoin(
        /* [in] */ WorkQueue* joiner,
        /* [in] */ IForkJoinTask* task);

    /**
     * Stripped-down variant of awaitJoin used by timed joins. Tries
     * to help join only while there is continuous progress. (Caller
     * will then enter a timed wait.)
     *
     * @param joiner the joining worker
     * @param task the task
     */
    CARAPI_(void) HelpJoinOnce(
        /* [in] */ WorkQueue* joiner,
        /* [in] */ ForkJoinTask* task);

    /**
     * Runs tasks until {@code isQuiescent()}. We piggyback on
     * active count ctl maintenance, but rather than blocking
     * when tasks cannot be found, we rescan until all others cannot
     * find tasks either.
     */
    CARAPI_(void) HelpQuiescePool(
        /* [in] */ WorkQueue* w);

    /**
     * Gets and removes a local or stolen task for the given worker.
     *
     * @return a task, if available
     */
    CARAPI_(AutoPtr<IForkJoinTask>) NextTaskFor(
        /* [in] */ WorkQueue* w);

    static CARAPI_(Int32) GetSurplusQueuedTaskCount();

    /**
     * Returns common pool queue for a thread that has submitted at
     * least one task.
     */
    static CARAPI_(AutoPtr<WorkQueue>) CommonSubmitterQueue();

    /**
     * Tries to pop the given task from submitter's queue in common pool.
     */
    CARAPI_(Boolean) TryExternalUnpush(
        /* [in] */ IForkJoinTask* task);

    CARAPI_(Int32) ExternalHelpComplete(
        /* [in] */ ICountedCompleter* task);

    /**
     * Returns the common pool instance. This pool is statically
     * constructed; its run state is unaffected by attempts to {@link
     * #shutdown} or {@link #shutdownNow}. However this pool and any
     * ongoing processing are automatically terminated upon program
     * {@link System#exit}.  Any program that relies on asynchronous
     * task processing to complete before program termination should
     * invoke {@code commonPool().}{@link #awaitQuiescence awaitQuiescence},
     * before exit.
     *
     * @return the common pool instance
     * @since 1.8
     * @hide
     */
    static CARAPI_(AutoPtr<IForkJoinPool>) CommonPool();

private:
    /**
     * If there is a security manager, makes sure caller has
     * permission to modify threads.
     */
    static CARAPI_(void) CheckPermission();

    /**
     * Returns the next sequence number. We don't expect this to
     * ever contend, so use simple builtin sync.
     */
    static CARAPI_(Int32) NextPoolId();

    /**
     * Acquires the plock lock to protect worker array and related
     * updates. This method is called only if an initial CAS on plock
     * fails. This acts as a spinlock for normal cases, but falls back
     * to builtin monitor to block when (rarely) needed. This would be
     * a terrible idea for a highly contended lock, but works fine as
     * a more conservative alternative to a pure spinlock.
     */
    CARAPI_(Int32) AcquirePlock();

    /**
     * Unlocks and signals any thread waiting for plock. Called only
     * when CAS of seq value for unlock fails.
     */
    CARAPI_(void) ReleasePlock(
        /* [in] */ Int32 ps);

    /**
     * Tries to create and start one worker if fewer than target
     * parallelism level exist. Adjusts counts etc on failure.
     */
    CARAPI_(void) TryAddWorker();

    /**
     * Full version of externalPush. This method is called, among
     * other times, upon the first submission of the first task to the
     * pool, so must perform secondary initialization.  It also
     * detects first submission by an external thread by looking up
     * its ThreadLocal, and creates a new shared queue if the one at
     * index if empty or contended. The plock lock body must be
     * exception-free (so no try/finally) so we optimistically
     * allocate new queues outside the lock and throw them away if
     * (very rarely) not needed.
     *
     * Secondary initialization occurs when plock is zero, to create
     * workQueue array and set plock to a valid value.  This lock body
     * must also be exception-free. Because the plock seq value can
     * eventually wrap around zero, this method harmlessly fails to
     * reinitialize if workQueues exists, while still advancing plock.
     */
    CARAPI_(void) FullExternalPush(
        /* [in] */ IForkJoinTask* task);

    /**
     * Scans for and, if found, runs one task, else possibly
     * inactivates the worker. This method operates on single reads of
     * volatile state and is designed to be re-invoked continuously,
     * in part because it returns upon detecting inconsistencies,
     * contention, or state changes that indicate possible success on
     * re-invocation.
     *
     * The scan searches for tasks across queues starting at a random
     * index, checking each at least twice.  The scan terminates upon
     * either finding a non-empty queue, or completing the sweep. If
     * the worker is not inactivated, it takes and runs a task from
     * this queue. Otherwise, if not activated, it tries to activate
     * itself or some other worker by signalling. On failure to find a
     * task, returns (for retry) if pool state may have changed during
     * an empty scan, or tries to inactivate if active, else possibly
     * blocks or terminates via method awaitWork.
     *
     * @param w the worker (via its WorkQueue)
     * @param r a random seed
     * @return worker qlock status if would have waited, else 0
     */
    CARAPI_(Int32) Scan(
        /* [in] */ WorkQueue* w,
        /* [in] */ Int32 r);

    /**
     * A continuation of scan(), possibly blocking or terminating
     * worker w. Returns without blocking if pool state has apparently
     * changed since last invocation.  Also, if inactivating w has
     * caused the pool to become quiescent, checks for pool
     * termination, and, so long as this is not the only worker, waits
     * for event for up to a given duration.  On timeout, if ctl has
     * not changed, terminates the worker, which will in turn wake up
     * another worker to possibly repeat this process.
     *
     * @param w the calling worker
     * @param c the ctl value on entry to scan
     * @param ec the worker's eventCount on entry to scan
     */
    CARAPI_(Int32) AwaitWork(
        /* [in] */ WorkQueue* w,
        /* [in] */ Int64 c,
        /* [in] */ Int32 ec);

    /**
     * Possibly releases (signals) a worker. Called only from scan()
     * when a worker with apparently inactive status finds a non-empty
     * queue. This requires revalidating all of the associated state
     * from caller.
     */
    CARAPI_(void) HelpRelease(
        /* [in] */ Int64 c,
        /* [in] */ ArrayOf<WorkQueue*>* ws,
        /* [in] */ WorkQueue* w,
        /* [in] */ WorkQueue* q,
        /* [in] */ Int32 b);

    /**
     * Tries to locate and execute tasks for a stealer of the given
     * task, or in turn one of its stealers, Traces currentSteal ->
     * currentJoin links looking for a thread working on a descendant
     * of the given task and with a non-empty queue to steal back and
     * execute tasks from. The first call to this method upon a
     * waiting join will often entail scanning/search, (which is OK
     * because the joiner has nothing better to do), but this method
     * leaves hints in workers to speed up subsequent calls. The
     * implementation is very branchy to cope with potential
     * inconsistencies or loops encountering chains that are stale,
     * unknown, or so long that they are likely cyclic.
     *
     * @param joiner the joining worker
     * @param task the task to join
     * @return 0 if no progress can be made, negative if task
     * known complete, else positive
     */
    CARAPI_(Int32) TryHelpStealer(
        /* [in] */ WorkQueue* joiner,
        /* [in] */ IForkJoinTask* task);

    /**
     * Analog of tryHelpStealer for CountedCompleters. Tries to steal
     * and run tasks within the target's computation.
     *
     * @param task the task to join
     */
    CARAPI_(Int32) HelpComplete(
        /* [in] */ WorkQueue* joiner,
        /* [in] */ ICountedCompleter* task);

    /**
     * Returns a (probably) non-empty steal queue, if one is found
     * during a scan, else null.  This method must be retried by
     * caller if, by the time it tries to use the queue, it is empty.
     */
    CARAPI_(AutoPtr<WorkQueue>) FindNonEmptyStealQueue();

    /**
     * Possibly initiates and/or completes termination.  The caller
     * triggering termination runs three passes through workQueues:
     * (0) Setting termination status, followed by wakeups of queued
     * workers; (1) cancelling all tasks; (2) interrupting lagging
     * threads (likely in external tasks, but possibly also blocked in
     * joins).  Each pass repeats previous steps because of potential
     * lagging thread creation.
     *
     * @param now if true, unconditionally terminate, else only
     * if no work and no active workers
     * @param enable if true, enable shutdown when next possible
     * @return true if now terminating or terminated
     */
    CARAPI_(Boolean) TryTerminate(
        /* [in] */ Boolean now,
        /* [in] */ Boolean enable);

    static CARAPI_(Int32) CheckParallelism(
        /* [in] */ Int32 parallelism);

    static CARAPI_(AutoPtr<IForkJoinPoolForkJoinWorkerThreadFactory>) CheckFactory(
        /* [in] */ IForkJoinPoolForkJoinWorkerThreadFactory* factory);

    /**
     * Creates a {@code ForkJoinPool} with the given parameters, without
     * any security checks or parameter validation.  Invoked directly by
     * makeCommonPool.
     */
    ForkJoinPool(
        /* [in] */ Int32 parallelism,
        /* [in] */ IForkJoinPoolForkJoinWorkerThreadFactory* factory,
        /* [in] */ IThreadUncaughtExceptionHandler* handler,
        /* [in] */ Int32 mode,
        /* [in] */ String workerNamePrefix);

    static CARAPI_(AutoPtr<IForkJoinPool>) MakeCommonPool();

public:
    /**
     * Per-thread submission bookkeeping. Shared across all pools
     * to reduce ThreadLocal pollution and because random motion
     * to avoid contention in one pool is likely to hold for others.
     * Lazily initialized on first submission (but null-checked
     * in other contexts to avoid unnecessary initialization).
     */
//    static AutoPtr<IThreadLocal> mSubmitters;

    /**
     * Creates a new ForkJoinWorkerThread. This factory is used unless
     * overridden in ForkJoinPool constructors.
     */
    static AutoPtr<IForkJoinPoolForkJoinWorkerThreadFactory>
        mDefaultForkJoinWorkerThreadFactory;

    /**
     * Permission required for callers of methods that may start or
     * kill threads.
     */
//    static AutoPtr<IRuntimePermission> mModifyThreadPermission;

    /**
     * Common (static) pool. Non-null for public use unless a static
     * construction exception, but internal usages null-check on use
     * to paranoically avoid potential initialization circularities
     * as well as to simplify generated code.
     */
    static AutoPtr<IForkJoinPool> mCommon;

    /**
     * Common pool parallelism. To allow simpler use and management
     * when common pool threads are disabled, we allow the underlying
     * common.parallelism field to be zero, but in that case still report
     * parallelism as 1 to reflect resulting caller-runs mechanics.
     */
    static Int32 mCommonParallelism;

    /*
     * Bits and masks for control variables
     *
     * Field ctl is a long packed with:
     * AC: Number of active running workers minus target parallelism (16 bits)
     * TC: Number of total workers minus target parallelism (16 bits)
     * ST: true if pool is terminating (1 bit)
     * EC: the wait count of top waiting thread (15 bits)
     * ID: poolIndex of top of Treiber stack of waiters (16 bits)
     *
     * When convenient, we can extract the upper 32 bits of counts and
     * the lower 32 bits of queue state, u = (Int32)(ctl >>> 32) and e =
     * (int)ctl.  The ec field is never accessed alone, but always
     * together with id and st. The offsets of counts by the target
     * parallelism and the positionings of fields makes it possible to
     * perform the most common checks via sign tests of fields: When
     * ac is negative, there are not enough active workers, when tc is
     * negative, there are not enough total workers, and when e is
     * negative, the pool is terminating.  To deal with these possibly
     * negative fields, we use casts in and out of "short" and/or
     * signed shifts to maintain signedness.
     *
     * When a thread is queued (inactivated), its eventCount field is
     * set negative, which is the only way to tell if a worker is
     * prevented from executing tasks, even though it must continue to
     * scan for them to avoid queuing races. Note however that
     * eventCount updates lag releases so usage requires care.
     *
     * Field plock is an int packed with:
     * SHUTDOWN: true if shutdown is enabled (1 bit)
     * SEQ:  a sequence lock, with PL_LOCK bit set if locked (30 bits)
     * SIGNAL: set when threads may be waiting on the lock (1 bit)
     *
     * The sequence number enables simple consistency checks:
     * Staleness of read-only operations on the workQueues array can
     * be checked by comparing plock before vs after the reads.
     */

    // bit positions/shifts for fields
    static Int32 AC_SHIFT;
    static Int32 TC_SHIFT;
    static Int32 ST_SHIFT;
    static Int32 EC_SHIFT;

    // bounds
    static Int32 SMASK;  // short bits
    static Int32 MAX_CAP;  // max #workers - 1
    static Int32 EVENMASK;  // even short bits
    static Int32 SQMASK;  // max 64 (even) slots
    static Int32 SHORT_SIGN;
    static Int32 INT_SIGN;

    // masks
    static Int64 STOP_BIT;
    static Int64 AC_MASK;
    static Int64 TC_MASK;

    // units for incrementing and decrementing
    static Int64 TC_UNIT;
    static Int64 AC_UNIT;

    // masks and units for dealing with u = (int)(ctl >>> 32)
    static Int32 UAC_SHIFT;
    static Int32 UTC_SHIFT;
    static Int32 UAC_MASK;
    static Int32 UTC_MASK;
    static Int32 UAC_UNIT;
    static Int32 UTC_UNIT;

    // masks and units for dealing with e = (int)ctl
    static Int32 E_MASK; // no STOP_BIT
    static Int32 E_SEQ;

    // access mode for WorkQueue
    static Int32 LIFO_QUEUE;
    static Int32 FIFO_QUEUE;
    static Int32 SHARED_QUEUE;

    // plock bits
    static Int32 SHUTDOWN;
    static Int32 PL_LOCK;
    static Int32 PL_SIGNAL;
    static Int32 PL_SPINS;

    // Heuristic padding to ameliorate unfortunate memory placements
    Int64 mPad00, mPad01, mPad02, mPad03, mPad04, mPad05, mPad06;

    // Instance fields
    Int64 mStealCount;                  // collects worker counts
    Int64 mCtl;                         // main pool control
    Int32 mPlock;                        // shutdown status and seqLock
    Int32 mIndexSeed;                    // worker/submitter index seed
    Int16 mParallelism;                   // parallelism level
    Int16 mMode;                          // LIFO/FIFO
    AutoPtr<ArrayOf<WorkQueue*> > mWorkQueues;                    // main registry
    AutoPtr<IForkJoinPoolForkJoinWorkerThreadFactory> mFactory;
    AutoPtr<IThreadUncaughtExceptionHandler> mUeh;        // per-worker UEH
    String mWorkerNamePrefix;             // to create worker name string

    AutoPtr<IInterface> mPad10, mPad11, mPad12, mPad13, mPad14, mPad15, mPad16, mPad17;
    AutoPtr<IInterface> mPad18, mPad19, mPad1a, mPad1b;

private:
    /**
     * Initial timeout value (in nanoseconds) for the thread
     * triggering quiescence to park waiting for new work. On timeout,
     * the thread will instead try to shrink the number of
     * workers. The value should be large enough to avoid overly
     * aggressive shrinkage during most transient stalls (long GCs
     * etc).
     */
    static Int64 IDLE_TIMEOUT;

    /**
     * Timeout value when there are more threads than parallelism level
     */
    static Int64 FAST_IDLE_TIMEOUT;

    /**
     * Tolerance for idle timeouts, to cope with timer undershoots
     */
    static Int64 TIMEOUT_SLOP;

    /**
     * The maximum stolen->joining link depth allowed in method
     * tryHelpStealer.  Must be a power of two.  Depths for legitimate
     * chains are unbounded, but we use a fixed constant to avoid
     * (otherwise unchecked) cycles and to bound staleness of
     * traversal parameters at the expense of sometimes blocking when
     * we could be helping.
     */
    static Int32 MAX_HELP;

    /**
     * Increment for seed generators. See class ThreadLocal for
     * explanation.
     */
    static Int32 SEED_INCREMENT;

    /**
     * Sequence number for creating workerNamePrefix.
     */
    static Int32 mPoolNumberSequence;

    // // Unsafe mechanics
    // private static final sun.misc.Unsafe U;
    // private static final long CTL;
    // private static final long PARKBLOCKER;
    // private static final int ABASE;
    // private static final int ASHIFT;
    // private static final long STEALCOUNT;
    // private static final long PLOCK;
    // private static final long INDEXSEED;
    // private static final long QBASE;
    // private static final long QLOCK;

    // static {
    //     // initialize field offsets for CAS etc
    //     try {
    //         U = sun.misc.Unsafe.getUnsafe();
    //         Class<?> k = ForkJoinPool.class;
    //         CTL = U.objectFieldOffset
    //             (k.getDeclaredField("ctl"));
    //         STEALCOUNT = U.objectFieldOffset
    //             (k.getDeclaredField("stealCount"));
    //         PLOCK = U.objectFieldOffset
    //             (k.getDeclaredField("plock"));
    //         INDEXSEED = U.objectFieldOffset
    //             (k.getDeclaredField("indexSeed"));
    //         Class<?> tk = Thread.class;
    //         PARKBLOCKER = U.objectFieldOffset
    //             (tk.getDeclaredField("parkBlocker"));
    //         Class<?> wk = WorkQueue.class;
    //         QBASE = U.objectFieldOffset
    //             (wk.getDeclaredField("base"));
    //         QLOCK = U.objectFieldOffset
    //             (wk.getDeclaredField("qlock"));
    //         Class<?> ak = ForkJoinTask[].class;
    //         ABASE = U.arrayBaseOffset(ak);
    //         int scale = U.arrayIndexScale(ak);
    //         if ((scale & (scale - 1)) != 0)
    //             throw new Error("data type scale not a power of two");
    //         ASHIFT = 31 - Integer.numberOfLeadingZeros(scale);
    //     } catch (Exception e) {
    //         throw new Error(e);
    //     }

    //     submitters = new ThreadLocal<Submitter>();
    //     defaultForkJoinWorkerThreadFactory =
    //         new DefaultForkJoinWorkerThreadFactory();
    //     modifyThreadPermission = new RuntimePermission("modifyThread");

    //     common = java.security.AccessController.doPrivileged
    //         (new java.security.PrivilegedAction<ForkJoinPool>() {
    //             public ForkJoinPool run() { return makeCommonPool(); }});
    //     int par = common.parallelism; // report 1 even if threads disabled
    //     commonParallelism = par > 0 ? par : 1;
    // }
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Utility::Concurrent::CForkJoinPool::WorkQueue, IInterface)

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_CFORKJOINPOOL_H__
