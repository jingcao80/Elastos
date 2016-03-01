
#include "coredef.h"
#include "NativeThread.h"
#include "Thread.h"
#include "ThreadGroup.h"
#include "CThreadGroup.h"
#include "Globals.h"
#include "CThread.h"
#include "Os.h"
#include <sys/resource.h>
#include <sys/time.h>
#include <cutils/sched_policy.h>
#include <cutils/atomic.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <utils/Log.h>

namespace Elastos {
namespace Core {

/*
 * Monitor shape field.  Used to distinguish immediate thin locks from
 * indirecting fat locks.
 */
#define LW_SHAPE_THIN 0
#define LW_SHAPE_FAT 1
#define LW_SHAPE_MASK 0x1
#define LW_SHAPE(x) ((x) & LW_SHAPE_MASK)

/*
 * Hash state field.  Used to signify that an object has had its
 * identity hash code exposed or relocated.
 */
#define LW_HASH_STATE_UNHASHED 0
#define LW_HASH_STATE_HASHED 1
#define LW_HASH_STATE_HASHED_AND_MOVED 3
#define LW_HASH_STATE_MASK 0x3
#define LW_HASH_STATE_SHIFT 1
#define LW_HASH_STATE(x) (((x) >> LW_HASH_STATE_SHIFT) & LW_HASH_STATE_MASK)

/*
 * Monitor accessor.  Extracts a monitor structure pointer from a fat
 * lock.  Performs no error checking.
 */
#define LW_MONITOR(x) \
  ((Monitor*)((x) & ~((LW_HASH_STATE_MASK << LW_HASH_STATE_SHIFT) | \
                      LW_SHAPE_MASK)))

/*
 * Lock owner field.  Contains the thread id of the thread currently
 * holding the lock.
 */
#define LW_LOCK_OWNER_MASK 0xffff
#define LW_LOCK_OWNER_SHIFT 3
#define LW_LOCK_OWNER(x) (((x) >> LW_LOCK_OWNER_SHIFT) & LW_LOCK_OWNER_MASK)

/*
 * Lock recursion count field.  Contains a count of the numer of times
 * a lock has been recursively acquired.
 */
#define LW_LOCK_COUNT_MASK 0x1fff
#define LW_LOCK_COUNT_SHIFT 19
#define LW_LOCK_COUNT(x) (((x) >> LW_LOCK_COUNT_SHIFT) & LW_LOCK_COUNT_MASK)

/*
 * Initialize a Lock to the proper starting value.
 * This is necessary for thin locking.
 */
#define NATIVE_LOCK_INITIAL_THIN_VALUE (0)

#define NATIVE_LOCK_INIT(lock) \
    do { *(lock) = NATIVE_LOCK_INITIAL_THIN_VALUE; } while (0)


static NativeThread* AllocThread(Int32 stackSize);
static void SetThreadSelf(NativeThread* thread);
static void AssignThreadId(NativeThread* thread);
static void* ThreadEntry(void* arg);
static void* InternalThreadStart(void* arg);
static void ThreadExitCheck(void* arg);
static Boolean PrepareThread(NativeThread* thread);
static Monitor* NativeCreateMonitor(NativeObject* obj);
static UInt64 NativeGetRelativeTimeNsec();
static UInt64 NativeGetRelativeTimeUsec();

/*
 * Initialize thread list and main thread's environment.  We need to set
 * up some basic stuff so that dvmThreadSelf() will work when we start
 * loading classes (e.g. to check for exceptions).
 */
Boolean NativeThreadStartup()
{
    NativeThread* thread = NULL;

    /* allocate a TLS slot */
    if (pthread_key_create(&gCore.mPthreadKeySelf, ThreadExitCheck) != 0) {
        //LOGE("ERROR: pthread_key_create failed\n");
        return FALSE;
    }

    /* test our pthread lib */
    if (pthread_getspecific(gCore.mPthreadKeySelf) != NULL) {
        //LOGW("WARNING: newly-created pthread TLS slot is not NULL\n");
        pthread_setspecific(gCore.mPthreadKeySelf, NULL);
    }

    /* prep thread-related locks and conditions */
    NativeInitMutex(&gCore.mThreadListLock);
    pthread_cond_init(&gCore.mThreadStartCond, NULL);
    //dvmInitMutex(&gDvm.vmExitLock);
    //pthread_cond_init(&gDvm.vmExitCond, NULL);
    NativeInitMutex(&gCore.mThreadSuspendLock);
    NativeInitMutex(&gCore.mThreadSuspendCountLock);
    pthread_cond_init(&gCore.mThreadSuspendCountCond, NULL);
// #ifdef WITH_DEADLOCK_PREDICTION
//     dvmInitMutex(&gDvm.deadlockHistoryLock);
// #endif
    NativeInitMutex(&gCore.mThreadCountLock);

    /*
     * Dedicated monitor for Thread.sleep().
     * TODO: change this to an Object* so we don't have to expose this
     * call, and we interact better with JDWP monitor calls.  Requires
     * deferring the object creation to much later (e.g. final "main"
     * thread prep) or until first use.
     */
    gCore.mThreadSleepMon = NativeCreateMonitor(NULL);

    // gDvm.threadIdMap = dvmAllocBitVector(kMaxThreadId, false);

    thread = AllocThread(gCore.mStackSize);
    if (thread == NULL) {
        return FALSE;
    }

    /* switch mode for when we run initializers */
    thread->mStatus = NTHREAD_RUNNING;

    /*
     * We need to assign the threadId early so we can lock/notify
     * object monitors.  We'll set the "threadObj" field later.
     */
    PrepareThread(thread);
    gCore.mThreadList = thread;

// #ifdef COUNT_PRECISE_METHODS
//     gDvm.preciseMethods = dvmPointerSetAlloc(200);
// #endif

    return TRUE;
}

/*
 * Grab the thread list global lock.
 *
 * This is held while "suspend all" is trying to make everybody stop.  If
 * the shutdown is in progress, and somebody tries to grab the lock, they'll
 * have to wait for the GC to finish.  Therefore it's important that the
 * thread not be in RUNNING mode.
 *
 * We don't have to check to see if we should be suspended once we have
 * the lock.  Nobody can suspend all threads without holding the thread list
 * lock while they do it, so by definition there isn't a GC in progress.
 *
 * This function deliberately avoids the use of dvmChangeStatus(),
 * which could grab threadSuspendCountLock.  To avoid deadlock, threads
 * are required to grab the thread list lock before the thread suspend
 * count lock.  (See comment in DvmGlobals.)
 *
 * TODO: consider checking for suspend after acquiring the lock, and
 * backing off if set.  As stated above, it can't happen during normal
 * execution, but it *can* happen during shutdown when daemon threads
 * are being suspended.
 */
void NativeLockThreadList(
    /* [in] */ NativeThread* self)
{
    NativeThreadStatus oldStatus;

    if (self == NULL) {
        /* try to get it from TLS */
        self = NativeThreadSelf();
    }

    if (self != NULL) {
        oldStatus = self->mStatus;
        self->mStatus = NTHREAD_VMWAIT;
    }
    else {
        /* happens during VM shutdown */
        //LOGW("NULL self in dvmLockThreadList\n");
        // oldStatus = -1;         // shut up gcc
    }

    NativeLockMutex(&gCore.mThreadListLock);

    if (self != NULL) {
        self->mStatus = oldStatus;
    }
}

/*
 * Release the thread list global lock.
 */
void NativeUnlockThreadList()
{
    NativeUnlockMutex(&gCore.mThreadListLock);
}

/*
 * Finish preparing the main thread, allocating some objects to represent
 * it.  As part of doing so, we finish initializing Thread and ThreadGroup.
 * This will execute some interpreted code (e.g. class initializers).
 */
Boolean NativePrepMainThread()
{
    NativeThread* thread;
    AutoPtr<IThreadGroup> groupObj;
    AutoPtr<CThread> threadObj;
    // Method* init;
    // JValue unused;

    //LOGV("+++ finishing prep on main VM thread\n");

    /* main thread is always first in list at this point */
    thread = gCore.mThreadList;
    // assert(thread->mThreadId == kMainThreadId);

    groupObj = NativeGetMainThreadGroup();
    if (groupObj == NULL) {
        return FALSE;
    }

    /*
     * Allocate and construct a Thread with the internal-creation
     * constructor.
     */
    ECode ec = CThread::NewByFriend(groupObj, String("main"), IThread::NORM_PRIORITY, FALSE, (CThread**)&threadObj);
    if (FAILED(ec)) {
    //     LOGE("exception thrown while constructing main thread object\n");
        return FALSE;
    }

    Thread* _t = (Thread*)threadObj.Get();
    assert(_t != NULL);
    _t->mNativeThread = thread;
    thread->mThreadObj = reinterpret_cast<Int32>(_t);
    REFCOUNT_ADD(threadObj);

    /*
     * Set the context class loader.  This invokes a ClassLoader method,
     * which could conceivably call Thread.currentThread(), so we want the
     * Thread to be fully configured before we do this.
     */
    // Object* systemLoader = dvmGetSystemClassLoader();
    // if (systemLoader == NULL) {
    //     LOGW("WARNING: system class loader is NULL (setting main ctxt)\n");
    //     /* keep going */
    // }
    // int ctxtClassLoaderOffset = dvmFindFieldOffset(gDvm.classJavaLangThread,
    //     "contextClassLoader", "Ljava/lang/ClassLoader;");
    // if (ctxtClassLoaderOffset < 0) {
    //     LOGE("Unable to find contextClassLoader field in Thread\n");
    //     return false;
    // }
    // dvmSetFieldObject(threadObj, ctxtClassLoaderOffset, systemLoader);

    /*
     * Finish our thread prep.
     */

    /* include self in non-daemon threads (mainly for AttachCurrentThread) */
    gCore.mNonDaemonThreadCount++;
    gCore.mThreadCount++;

    return TRUE;
}

/*
 * Alloc and initialize a Thread struct.
 *
 * Does not create any objects, just stuff on the system (malloc) heap.
 */
static NativeThread* AllocThread(
    /* [in] */ Int32 stackSize)
{
    NativeThread* thread;
    // u1* stackBottom;

    thread = (NativeThread*)calloc(1, sizeof(NativeThread));
    if (thread == NULL) {
        return NULL;
    }

//#if defined(WITH_SELF_VERIFICATION)
//    if (dvmSelfVerificationShadowSpaceAlloc(thread) == NULL)
//        return NULL;
//#endif

    assert(stackSize >= kMinStackSize && stackSize <=kMaxStackSize);

    thread->mStatus = NTHREAD_INITIALIZING;
    thread->mSuspendCount = 0;

#ifdef WITH_ALLOC_LIMITS
    thread->mAllocLimit = -1;
#endif

//    /*
//     * Allocate and initialize the interpreted code stack.  We essentially
//     * "lose" the alloc pointer, which points at the bottom of the stack,
//     * but we can get it back later because we know how big the stack is.
//     *
//     * The stack must be aligned on a 4-byte boundary.
//     */
//#ifdef MALLOC_INTERP_STACK
//    stackBottom = (u1*) malloc(interpStackSize);
//    if (stackBottom == NULL) {
//#if defined(WITH_SELF_VERIFICATION)
//        dvmSelfVerificationShadowSpaceFree(thread);
//#endif
//        free(thread);
//        return NULL;
//    }
//    memset(stackBottom, 0xc5, interpStackSize);     // stop valgrind complaints
//#else
//    stackBottom = mmap(NULL, interpStackSize, PROT_READ | PROT_WRITE,
//        MAP_PRIVATE | MAP_ANON, -1, 0);
//    if (stackBottom == MAP_FAILED) {
//#if defined(WITH_SELF_VERIFICATION)
//        dvmSelfVerificationShadowSpaceFree(thread);
//#endif
//        free(thread);
//        return NULL;
//    }
//#endif

//    assert(((u4)stackBottom & 0x03) == 0); // looks like our malloc ensures this
    thread->mStackSize = stackSize;
//    thread->interpStackStart = stackBottom + interpStackSize;
//    thread->interpStackEnd = stackBottom + STACK_OVERFLOW_RESERVE;
//
//    /* give the thread code a chance to set things up */
//    dvmInitInterpStack(thread, interpStackSize);

    return thread;
}

/*
 * Get a meaningful thread ID.  At present this only has meaning under Linux,
 * where getpid() and gettid() sometimes agree and sometimes don't depending
 * on your thread model (try "export LD_ASSUME_KERNEL=2.4.19").
 */
pid_t NativeGetSysThreadId()
{
#ifdef HAVE_GETTID
    return gettid();
#else
    return getpid();
#endif
}

/*
 * Finish initialization of a Thread struct.
 *
 * This must be called while executing in the new thread, but before the
 * thread is added to the thread list.
 *
 * NOTE: The threadListLock must be held by the caller (needed for
 * assignThreadId()).
 */
static Boolean PrepareThread(
    /* [in] */ NativeThread* thread)
{
    thread->mSystemTid = NativeGetSysThreadId();
    AssignThreadId(thread);
    thread->mHandle = pthread_self();

    //LOGI("SYSTEM TID IS %d (pid is %d)\n", (int) thread->systemTid,
    //    (int) getpid());
    /*
     * If we were called by dvmAttachCurrentThread, the self value is
     * already correctly established as "thread".
     */
    SetThreadSelf(thread);

    // LOGV("threadid=%d: interp stack at %p\n",
    //     thread->threadId, thread->interpStackStart - thread->interpStackSize);

    /*
     * Initialize invokeReq.
     */
//    DvmInitMutex(&thread->invokeReq.lock);
//    pthread_cond_init(&thread->invokeReq.cv, NULL);

//    /*
//     * Initialize our reference tracking tables.
//     *
//     * Most threads won't use jniMonitorRefTable, so we clear out the
//     * structure but don't call the init function (which allocs storage).
//     */
//#ifdef USE_INDIRECT_REF
//    if (!dvmInitIndirectRefTable(&thread->jniLocalRefTable,
//            kJniLocalRefMin, kJniLocalRefMax, kIndirectKindLocal))
//        return false;
//#else
//    /*
//     * The JNI local ref table *must* be fixed-size because we keep pointers
//     * into the table in our stack frames.
//     */
//    if (!dvmInitReferenceTable(&thread->jniLocalRefTable,
//            kJniLocalRefMax, kJniLocalRefMax))
//        return false;
//#endif
//    if (!dvmInitReferenceTable(&thread->internalLocalRefTable,
//            kInternalRefDefault, kInternalRefMax))
//        return false;

    // memset(&thread->jniMonitorRefTable, 0, sizeof(thread->jniMonitorRefTable));

    pthread_cond_init(&thread->mWaitCond, NULL);
    NativeInitMutex(&thread->mWaitMutex);

    return TRUE;
}

/*
 * Remove a thread from the internal list.
 * Clear out the links to make it obvious that the thread is
 * no longer on the list.  Caller must hold gDvm.threadListLock.
 */
static void UnlinkThread(
    /* [in] */ NativeThread* thread)
{
    // LOG_THREAD("threadid=%d: removing from list\n", thread->threadId);
    if (thread == gCore.mThreadList) {
        assert(thread->mPrev == NULL);
        gCore.mThreadList = thread->mNext;
    }
    else {
        assert(thread->mPrev != NULL);
        thread->mPrev->mNext = thread->mNext;
    }
    if (thread->mNext != NULL) {
        thread->mNext->mPrev = thread->mPrev;
    }
    thread->mPrev = thread->mNext = NULL;
}

/*
 * Free a Thread struct, and all the stuff allocated within.
 */
static void FreeThread(NativeThread* thread)
{
    if (thread == NULL) {
        return;
    }

    /* thread->threadId is zero at this point */
    // LOGVV("threadid=%d: freeing\n", thread->threadId);

//     if (thread->interpStackStart != NULL) {
//         u1* interpStackBottom;

//         interpStackBottom = thread->interpStackStart;
//         interpStackBottom -= thread->interpStackSize;
// #ifdef MALLOC_INTERP_STACK
//         free(interpStackBottom);
// #else
//         if (munmap(interpStackBottom, thread->interpStackSize) != 0)
//             LOGW("munmap(thread stack) failed\n");
// #endif
//     }

// #ifdef USE_INDIRECT_REF
//     dvmClearIndirectRefTable(&thread->jniLocalRefTable);
// #else
//     dvmClearReferenceTable(&thread->jniLocalRefTable);
// #endif
//     dvmClearReferenceTable(&thread->internalLocalRefTable);
//     if (&thread->jniMonitorRefTable.table != NULL)
//         dvmClearReferenceTable(&thread->jniMonitorRefTable);

// #if defined(WITH_SELF_VERIFICATION)
//     dvmSelfVerificationShadowSpaceFree(thread);
// #endif
    free(thread);
}


/*
 * Like pthread_self(), but on a Thread*.
 */
NativeThread* NativeThreadSelf()
{
    return (NativeThread*)pthread_getspecific(gCore.mPthreadKeySelf);
}

/*
 * Explore our sense of self.  Stuffs the thread pointer into TLS.
 */
static void SetThreadSelf(
    /* [in] */ NativeThread* thread)
{
    Int32 cc;

    cc = pthread_setspecific(gCore.mPthreadKeySelf, thread);
    if (cc != 0) {
        /*
         * Sometimes this fails under Bionic with EINVAL during shutdown.
         * This can happen if the timing is just right, e.g. a thread
         * fails to attach during shutdown, but the "fail" path calls
         * here to ensure we clean up after ourselves.
         */
        if (thread != NULL) {
            // LOGE("pthread_setspecific(%p) failed, err=%d\n", thread, cc);
            //dvmAbort();     /* the world is fundamentally hosed */
        }
    }
}

/*
 * This is associated with the pthreadKeySelf key.  It's called by the
 * pthread library when a thread is exiting and the "self" pointer in TLS
 * is non-NULL, meaning the VM hasn't had a chance to clean up.  In normal
 * operation this will not be called.
 *
 * This is mainly of use to ensure that we don't leak resources if, for
 * example, a thread attaches itself to us with AttachCurrentThread and
 * then exits without notifying the VM.
 *
 * We could do the detach here instead of aborting, but this will lead to
 * portability problems.  Other implementations do not do this check and
 * will simply be unaware that the thread has exited, leading to resource
 * leaks (and, if this is a non-daemon thread, an infinite hang when the
 * VM tries to shut down).
 *
 * Because some implementations may want to use the pthread destructor
 * to initiate the detach, and the ordering of destructors is not defined,
 * we want to iterate a couple of times to give those a chance to run.
 */
static void ThreadExitCheck(void* arg)
{
//    const Int32 kMaxCount = 2;
//
//    NativeThread* self = (NativeThread*)arg;
//    assert(self != NULL);
//
//    // LOGV("threadid=%d: threadExitCheck(%p) count=%d\n",
//    //     self->threadId, arg, self->threadExitCheckCount);
//
//    if (self->mStatus == NTHREAD_ZOMBIE) {
//        // LOGW("threadid=%d: Weird -- shouldn't be in threadExitCheck\n",
//        //     self->threadId);
//        return;
//    }
//
//    if (self->mThreadExitCheckCount < kMaxCount) {
//        /*
//         * Spin a couple of times to let other destructors fire.
//         */
//        // LOGD("threadid=%d: thread exiting, not yet detached (count=%d)\n",
//        //     self->threadId, self->threadExitCheckCount);
//        self->mThreadExitCheckCount++;
//        Int32 cc = pthread_setspecific(gDvm.mPthreadKeySelf, self);
//        if (cc != 0) {
//            // LOGE("threadid=%d: unable to re-add thread to TLS\n",
//            //     self->threadId);
//            //NativeAbort();
//        }
//    }
//    else {
//        // LOGE("threadid=%d: native thread exited without detaching\n",
//        //     self->threadId);
//        //NativeAbort();
//    }
}

/*
 * Assign the threadId.  This needs to be a small integer so that our
 * "thin" locks fit in a small number of bits.
 *
 * We reserve zero for use as an invalid ID.
 *
 * This must be called with threadListLock held.
 */
static void AssignThreadId(
    /* [in] */ NativeThread* thread)
{
    /*
     * Find a small unique integer.  threadIdMap is a vector of
     * kMaxThreadId bits;  dvmAllocBit() returns the index of a
     * bit, meaning that it will always be < kMaxThreadId.
     */
    // int num = dvmAllocBit(gDvm.threadIdMap);
    // if (num < 0) {
    //     LOGE("Ran out of thread IDs\n");
    //     dvmAbort();     // TODO: make this a non-fatal error result
    // }

    // thread->mThreadId = num + 1;
    thread->mThreadId = (UInt32)thread->mSystemTid;

    assert(thread->mThreadId != 0);
    // assert(thread->mThreadId != DVM_LOCK_INITIAL_THIN_VALUE);
}

/*
 * Give back the thread ID.
 */
static void ReleaseThreadId(
    /* [in] */ NativeThread* thread)
{
    assert(thread->mThreadId > 0);
    // dvmClearBit(gDvm.threadIdMap, thread->threadId - 1);
    thread->mThreadId = 0;
}

/*
 * Helper function to set the name of the current thread
 */
static void SetThreadName(
    /* [in] */ const char *threadName)
{
    if (threadName == NULL) {
        ALOGD("Thread name is null. Maybe you forgot to call Thread::constructor() explicitly.");
    }
    assert(threadName != NULL && "Thread name is null. Maybe you forgot to call Thread::constructor() explicitly.");

    Int32 hasAt = 0;
    Int32 hasDot = 0;
    const char *s = threadName;
    while (*s) {
        if (*s == '.') hasDot = 1;
        else if (*s == '@') hasAt = 1;
        s++;
    }
    Int32 len = s - threadName;
    if (len < 15 || hasAt || !hasDot) {
        s = threadName;
    }
    else {
        s = threadName + len - 15;
    }
#if defined(HAVE_ANDROID_PTHREAD_SETNAME_NP)
    /* pthread_setname_np fails rather than truncating long strings */
    char buf[16];       // MAX_TASK_COMM_LEN=16 is hard-coded into bionic
    strncpy(buf, s, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    Int32 err = pthread_setname_np(pthread_self(), buf);
    if (err != 0) {
        // LOGW("Unable to set the name of current thread to '%s': %s\n",
        //     buf, strerror(err));
    }
#elif defined(HAVE_PRCTL)
    prctl(PR_SET_NAME, (unsigned long) s, 0, 0, 0);
#else
    // LOGD("No way to set current thread's name (%s)\n", s);
#endif
}

/*
 * Create a thread as a result of java.lang.Thread.start().
 *
 * We do have to worry about some concurrency problems, e.g. programs
 * that try to call Thread.start() on the same object from multiple threads.
 * (This will fail for all but one, but we have to make sure that it succeeds
 * for exactly one.)
 *
 * Some of the complexity here arises from our desire to mimic the
 * Thread vs. VMThread class decomposition we inherited.  We've been given
 * a Thread, and now we need to create a VMThread and then populate both
 * objects.  We also need to create one of our internal Thread objects.
 *
 * Pass in a stack size of 0 to get the default.
 *
 * The "threadObj" reference must be pinned by the caller to prevent the GC
 * from moving it around (e.g. added to the tracked allocation list).
 */
Boolean NativeCreateThread(
    /* [in] */ Int32 threadObj,
    /* [in] */ Int32 reqStackSize)
{
    pthread_attr_t threadAttr;
    pthread_t threadHandle;
    NativeThread* self = NULL;
    NativeThread* newThread = NULL;
    Int32 stackSize;
    // NativeThreadStatus oldStatus;
    Int32 cc;

    assert(threadObj != 0);
    self = NativeThreadSelf();
    if (reqStackSize == 0) {
        stackSize = kDefaultStackSize;//gDvm.mStackSize;
    }
    else if (reqStackSize < kMinStackSize) {
        stackSize = kMinStackSize;
    }
    else if (reqStackSize > kMaxStackSize) {
        stackSize = kMaxStackSize;
    }
    else {
        stackSize = reqStackSize;
    }

    pthread_attr_init(&threadAttr);
    pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

    newThread = AllocThread(stackSize);
    if (newThread == NULL) {
        goto fail;
    }
    pthread_attr_setstacksize(&threadAttr, stackSize);
    newThread->mThreadObj = threadObj;

    assert(newThread->mStatus == NTHREAD_INITIALIZING);

    /*
     * We need to lock out other threads while we test and set the
     * "vmThread" field in java.lang.Thread, because we use that to determine
     * if this thread has been started before.  We use the thread list lock
     * because it's handy and we're going to need to grab it again soon
     * anyway.
     */
    NativeLockThreadList(self);

    // if (dvmGetFieldObject(threadObj, gDvm.offJavaLangThread_vmThread) != NULL) {
    //     dvmUnlockThreadList();
    //     dvmThrowException("Ljava/lang/IllegalThreadStateException;",
    //         "thread has already been started");
    //     goto fail;
    // }

    /*
     * There are actually three data structures: Thread (object), VMThread
     * (object), and Thread (C struct).  All of them point to at least one
     * other.
     *
     * As soon as "VMThread.vmData" is assigned, other threads can start
     * making calls into us (e.g. setPriority).
     */
    // dvmSetFieldInt(vmThreadObj, gDvm.offJavaLangVMThread_vmData, (u4)newThread);
    // dvmSetFieldObject(threadObj, gDvm.offJavaLangThread_vmThread, vmThreadObj);
    reinterpret_cast<Thread*>(threadObj)->mNativeThread = newThread;

    /*
     * Thread creation might take a while, so release the lock.
     */
    NativeUnlockThreadList();
    // oldStatus = NativeChangeStatus(self, NTHREAD_VMWAIT);
    cc = pthread_create(&threadHandle, &threadAttr, ThreadEntry, newThread);
    // oldStatus = NativeChangeStatus(self, oldStatus);
    if (cc != 0) {
        /*
         * Failure generally indicates that we have exceeded system
         * resource limits.  VirtualMachineError is probably too severe,
         * so use OutOfMemoryError.
         */
        // LOGE("Thread creation failed (err=%s)\n", strerror(errno));

        // dvmSetFieldObject(threadObj, gDvm.offJavaLangThread_vmThread, NULL);

        // dvmThrowException("Ljava/lang/OutOfMemoryError;",
        //     "thread creation failed");
        goto fail;
    }

    /*
     * We need to wait for the thread to start.  Otherwise, depending on
     * the whims of the OS scheduler, we could return and the code in our
     * thread could try to do operations on the new thread before it had
     * finished starting.
     *
     * The new thread will lock the thread list, change its state to
     * THREAD_STARTING, broadcast to gDvm.threadStartCond, and then sleep
     * on gDvm.threadStartCond (which uses the thread list lock).  This
     * thread (the parent) will either see that the thread is already ready
     * after we grab the thread list lock, or will be awakened from the
     * condition variable on the broadcast.
     *
     * We don't want to stall the rest of the VM while the new thread
     * starts, which can happen if the GC wakes up at the wrong moment.
     * So, we change our own status to VMWAIT, and self-suspend if
     * necessary after we finish adding the new thread.
     *
     *
     * We have to deal with an odd race with the GC/debugger suspension
     * mechanism when creating a new thread.  The information about whether
     * or not a thread should be suspended is contained entirely within
     * the Thread struct; this is usually cleaner to deal with than having
     * one or more globally-visible suspension flags.  The trouble is that
     * we could create the thread while the VM is trying to suspend all
     * threads.  The suspend-count won't be nonzero for the new thread,
     * so dvmChangeStatus(THREAD_RUNNING) won't cause a suspension.
     *
     * The easiest way to deal with this is to prevent the new thread from
     * running until the parent says it's okay.  This results in the
     * following (correct) sequence of events for a "badly timed" GC
     * (where '-' is us, 'o' is the child, and '+' is some other thread):
     *
     *  - call pthread_create()
     *  - lock thread list
     *  - put self into THREAD_VMWAIT so GC doesn't wait for us
     *  - sleep on condition var (mutex = thread list lock) until child starts
     *  + GC triggered by another thread
     *  + thread list locked; suspend counts updated; thread list unlocked
     *  + loop waiting for all runnable threads to suspend
     *  + success, start GC
     *  o child thread wakes, signals condition var to wake parent
     *  o child waits for parent ack on condition variable
     *  - we wake up, locking thread list
     *  - add child to thread list
     *  - unlock thread list
     *  - change our state back to THREAD_RUNNING; GC causes us to suspend
     *  + GC finishes; all threads in thread list are resumed
     *  - lock thread list
     *  - set child to THREAD_VMWAIT, and signal it to start
     *  - unlock thread list
     *  o child resumes
     *  o child changes state to THREAD_RUNNING
     *
     * The above shows the GC starting up during thread creation, but if
     * it starts anywhere after VMThread.create() is called it will
     * produce the same series of events.
     *
     * Once the child is in the thread list, it will be suspended and
     * resumed like any other thread.  In the above scenario the resume-all
     * code will try to resume the new thread, which was never actually
     * suspended, and try to decrement the child's thread suspend count to -1.
     * We can catch this in the resume-all code.
     *
     * Bouncing back and forth between threads like this adds a small amount
     * of scheduler overhead to thread startup.
     *
     * One alternative to having the child wait for the parent would be
     * to have the child inherit the parents' suspension count.  This
     * would work for a GC, since we can safely assume that the parent
     * thread didn't cause it, but we must only do so if the parent suspension
     * was caused by a suspend-all.  If the parent was being asked to
     * suspend singly by the debugger, the child should not inherit the value.
     *
     * We could also have a global "new thread suspend count" that gets
     * picked up by new threads before changing state to THREAD_RUNNING.
     * This would be protected by the thread list lock and set by a
     * suspend-all.
     */
    NativeLockThreadList(self);
    assert(self->mStatus == NTHREAD_RUNNING);
    self->mStatus = NTHREAD_VMWAIT;
    while (newThread->mStatus != NTHREAD_STARTING) {
        pthread_cond_wait(&gCore.mThreadStartCond, &gCore.mThreadListLock);
    }

    // LOG_THREAD("threadid=%d: adding to list\n", newThread->threadId);
    newThread->mNext = gCore.mThreadList->mNext;
    if (newThread->mNext != NULL) {
        newThread->mNext->mPrev = newThread;
    }
    newThread->mPrev = gCore.mThreadList;
    gCore.mThreadList->mNext = newThread;

    // if (!dvmGetFieldBoolean(threadObj, gDvm.offJavaLangThread_daemon)) {
    //     gDvm.nonDaemonThreadCount++;        // guarded by thread list lock
    // }

    NativeUnlockThreadList();

    /* change status back to RUNNING, self-suspending if necessary */
    NativeChangeStatus(self, NTHREAD_RUNNING);

    /*
     * Tell the new thread to start.
     *
     * We must hold the thread list lock before messing with another thread.
     * In the general case we would also need to verify that newThread was
     * still in the thread list, but in our case the thread has not started
     * executing user code and therefore has not had a chance to exit.
     *
     * We move it to VMWAIT, and it then shifts itself to RUNNING, which
     * comes with a suspend-pending check.
     */
    NativeLockThreadList(self);

    assert(newThread->mStatus == NTHREAD_STARTING);
    newThread->mStatus = NTHREAD_VMWAIT;
    pthread_cond_broadcast(&gCore.mThreadStartCond);

    NativeUnlockThreadList();

    // DvmReleaseTrackedAlloc(vmThreadObj, NULL);
    return TRUE;

fail:
    FreeThread(newThread);
    // dvmReleaseTrackedAlloc(vmThreadObj, NULL);
    return FALSE;
}

/*
 * pthread entry function for threads started from interpreted code.
 */
static void* ThreadEntry(void* arg)
{
    NativeThread* self = (NativeThread*)arg;
    Thread* threadObj = reinterpret_cast<Thread*>(self->mThreadObj);

    const char* threadName = threadObj->mName.string();
    SetThreadName(threadName);

    /*
     * Finish initializing the Thread struct.
     */
    NativeLockThreadList(self);
    PrepareThread(self);

    // LOG_THREAD("threadid=%d: created from interp\n", self->threadId);

    /*
     * Change our status and wake our parent, who will add us to the
     * thread list and advance our state to VMWAIT.
     */
    self->mStatus = NTHREAD_STARTING;
    pthread_cond_broadcast(&gCore.mThreadStartCond);

    /*
     * Wait until the parent says we can go.  Assuming there wasn't a
     * suspend pending, this will happen immediately.  When it completes,
     * we're full-fledged citizens of the VM.
     *
     * We have to use THREAD_VMWAIT here rather than THREAD_RUNNING
     * because the pthread_cond_wait below needs to reacquire a lock that
     * suspend-all is also interested in.  If we get unlucky, the parent could
     * change us to THREAD_RUNNING, then a GC could start before we get
     * signaled, and suspend-all will grab the thread list lock and then
     * wait for us to suspend.  We'll be in the tail end of pthread_cond_wait
     * trying to get the lock.
     */
    while (self->mStatus != NTHREAD_VMWAIT) {
        pthread_cond_wait(&gCore.mThreadStartCond, &gCore.mThreadListLock);
    }

    NativeUnlockThreadList();

    /*
     * Add a JNI context.
     */
    // self->jniEnv = dvmCreateJNIEnv(self);

    /*
     * Change our state so the GC will wait for us from now on.  If a GC is
     * in progress this call will suspend us.
     */
    NativeChangeStatus(self, NTHREAD_RUNNING);

    /*
     * Notify the debugger & DDM.  The debugger notification may cause
     * us to suspend ourselves (and others).
     */
    // if (gDvm.debuggerConnected) {
    //     dvmDbgPostThreadStart(self);
    // }

    /*
     * Set the system thread priority according to the Thread object's
     * priority level.  We don't usually need to do this, because both the
     * Thread object and system thread priorities inherit from parents.  The
     * tricky case is when somebody creates a Thread object, calls
     * setPriority(), and then starts the thread.  We could manage this with
     * a "needs priority update" flag to avoid the redundant call.
     */
    Int32 priority = threadObj->mPriority;
    NativeChangeThreadPriority(self, priority);

    /*
     * Execute the "run" method.
     *
     * At this point our stack is empty, so somebody who comes looking for
     * stack traces right now won't have much to look at.  This is normal.
     */
    // Method* run = self->threadObj->clazz->vtable[gDvm.voffJavaLangThread_run];
    // JValue unused;

    // LOGV("threadid=%d: calling run()\n", self->threadId);
    // assert(strcmp(run->name, "run") == 0);
    // dvmCallMethod(self, run, self->threadObj, &unused);
    /*ECode ec = */threadObj->Run();
    // LOGV("threadid=%d: exiting\n", self->threadId);

    /*
     * Remove the thread from various lists, report its death, and free
     * its resources.
     */
    NativeDetachCurrentThread();

    return NULL;
}

/*
 * Create an internal VM thread, for things like JDWP and finalizers.
 *
 * The easiest way to do this is create a new thread and then use the
 * JNI AttachCurrentThread implementation.
 *
 * This does not return until after the new thread has begun executing.
 */
Boolean NativeCreateInternalThread(
    /* [in] */ pthread_t* handle,
    /* [in] */ const char* name,
    /* [in] */ InternalThreadStartFunc func,
    /* [in] */ void* funcArg)
{
    InternalStartArgs* args;
    AutoPtr<IThreadGroup> systemGroup;
    pthread_attr_t threadAttr;
    volatile NativeThread* newThread = NULL;
    volatile Int32 createStatus = 0;

    systemGroup = NativeGetSystemThreadGroup();
    if (systemGroup == NULL) {
        return FALSE;
    }

    args = (InternalStartArgs*)malloc(sizeof(*args));
    args->mFunc = func;
    args->mFuncArg = funcArg;
    args->mName = strdup(name);     // storage will be owned by new thread
    args->mGroup = reinterpret_cast<Int32>(systemGroup.Get());
    args->mIsDaemon = TRUE;
    args->mThread = &newThread;
    args->mCreateStatus = &createStatus;

    pthread_attr_init(&threadAttr);
    //pthread_attr_setdetachstate(&threadAttr, PTHREAD_CREATE_DETACHED);

    if (pthread_create(handle, &threadAttr, InternalThreadStart,
            args) != 0) {
        // ALOGE("internal thread creation failed");
        free(args->mName);
        free(args);
        return FALSE;
    }

    /*
     * Wait for the child to start.  This gives us an opportunity to make
     * sure that the thread started correctly, and allows our caller to
     * assume that the thread has started running.
     *
     * Because we aren't holding a lock across the thread creation, it's
     * possible that the child will already have completed its
     * initialization.  Because the child only adjusts "createStatus" while
     * holding the thread list lock, the initial condition on the "while"
     * loop will correctly avoid the wait if this occurs.
     *
     * It's also possible that we'll have to wait for the thread to finish
     * being created, and as part of allocating a Thread object it might
     * need to initiate a GC.  We switch to VMWAIT while we pause.
     */
    NativeThread* self = NativeThreadSelf();
    NativeThreadStatus oldStatus = NativeChangeStatus(self, NTHREAD_VMWAIT);
    NativeLockThreadList(self);
    while (createStatus == 0) {
        pthread_cond_wait(&gCore.mThreadStartCond, &gCore.mThreadListLock);
    }

    if (newThread == NULL) {
        // ALOGW("internal thread create failed (createStatus=%d)", createStatus);
        assert(createStatus < 0);
        /* don't free pArgs -- if pthread_create succeeded, child owns it */
        NativeUnlockThreadList();
        NativeChangeStatus(self, oldStatus);
        return FALSE;
    }

    /* thread could be in any state now (except early init states) */
    //assert(newThread->status == THREAD_RUNNING);

    NativeUnlockThreadList();
    NativeChangeStatus(self, oldStatus);

    return TRUE;
}

/*
 * pthread entry function for internally-created threads.
 *
 * We are expected to free "arg" and its contents.  If we're a daemon
 * thread, and we get cancelled abruptly when the VM shuts down, the
 * storage won't be freed.  If this becomes a concern we can make a copy
 * on the stack.
 */
static void* InternalThreadStart(void* arg)
{
    InternalStartArgs* args = (InternalStartArgs*)arg;
    NativeAttachArgs nativeArgs;

    // jniArgs.version = JNI_VERSION_1_2;
    nativeArgs.mName = args->mName;
    nativeArgs.mGroup = args->mGroup;

    SetThreadName(args->mName);

    /* use local jniArgs as stack top */
    AutoPtr<IThread> thread;
    if (SUCCEEDED(NativeAttachCurrentThread(&nativeArgs, args->mIsDaemon, (IThread**)&thread))) {
        /*
         * Tell the parent of our success.
         *
         * threadListLock is the mutex for threadStartCond.
         */
        NativeLockThreadList(NativeThreadSelf());
        *args->mCreateStatus = 1;
        *args->mThread = NativeThreadSelf();
        pthread_cond_broadcast(&gCore.mThreadStartCond);
        NativeUnlockThreadList();

        // LOG_THREAD("threadid=%d: internal '%s'",
        //     dvmThreadSelf()->threadId, pArgs->name);

        /* execute */
        (*args->mFunc)(args->mFuncArg);

        /* detach ourselves */
        NativeDetachCurrentThread();
    }
    else {
        /*
         * Tell the parent of our failure.  We don't have a Thread struct,
         * so we can't be suspended, so we don't need to enter a critical
         * section.
         */
        NativeLockThreadList(NativeThreadSelf());
        *args->mCreateStatus = -1;
        assert(*args->mThread == NULL);
        pthread_cond_broadcast(&gCore.mThreadStartCond);
        NativeUnlockThreadList();

        assert(*args->mThread == NULL);
    }

    free(args->mName);
    free(args);
    return NULL;
}

/*
 * Attach the current thread to the VM.
 *
 * Used for internally-created threads and JNI's AttachCurrentThread.
 */
ECode NativeAttachCurrentThread(
    /* [in] */ const NativeAttachArgs* args,
    /* [in] */ Boolean isDaemon,
    /* [out] */ IThread** thread)
{
    NativeThread* self = NULL;
    AutoPtr<CThread> threadObj;
    // Object* vmThreadObj = NULL;
    String threadNameStr;
    // Method* init;
    Boolean ok;
    ECode ec = NOERROR;

    /* allocate thread struct, and establish a basic sense of self */
    self = AllocThread(gCore.mStackSize);
    if (self == NULL) {
        goto fail;
    }
    SetThreadSelf(self);

    /*
     * Finish our thread prep.  We need to do this before adding ourselves
     * to the thread list or invoking any interpreted code.  prepareThread()
     * requires that we hold the thread list lock.
     */
    NativeLockThreadList(self);
    ok = PrepareThread(self);
    NativeUnlockThreadList();
    if (!ok) {
        goto fail;
    }

    // self->jniEnv = dvmCreateJNIEnv(self);
    // if (self->jniEnv == NULL)
    //     goto fail;

    /*
     * Create a "fake" JNI frame at the top of the main thread interp stack.
     * It isn't really necessary for the internal threads, but it gives
     * the debugger something to show.  It is essential for the JNI-attached
     * threads.
     */
    // if (!createFakeRunFrame(self))
    //     goto fail;

    /*
     * The native side of the thread is ready; add it to the list.  Once
     * it's on the list the thread is visible to the JDWP code and the GC.
     */
    // LOG_THREAD("threadid=%d: adding to list (attached)\n", self->threadId);

    NativeLockThreadList(self);

    self->mNext = gCore.mThreadList->mNext;
    if (self->mNext != NULL) {
        self->mNext->mPrev = self;
    }
    self->mPrev = gCore.mThreadList;
    gCore.mThreadList->mNext = self;
    if (!isDaemon) {
        gCore.mNonDaemonThreadCount++;
    }

    NativeUnlockThreadList();

    /*
     * Switch state from initializing to running.
     *
     * It's possible that a GC began right before we added ourselves
     * to the thread list, and is still going.  That means our thread
     * suspend count won't reflect the fact that we should be suspended.
     * To deal with this, we transition to VMWAIT, pulse the heap lock,
     * and then advance to RUNNING.  That will ensure that we stall until
     * the GC completes.
     *
     * Once we're in RUNNING, we're like any other thread in the VM (except
     * for the lack of an initialized threadObj).  We're then free to
     * allocate and initialize objects.
     */
    assert(self->mStatus == NTHREAD_INITIALIZING);
    NativeChangeStatus(self, NTHREAD_VMWAIT);
    // dvmLockMutex(&gDvm.gcHeapLock);
    // dvmUnlockMutex(&gDvm.gcHeapLock);
    NativeChangeStatus(self, NTHREAD_RUNNING);

    /*
     * Create Thread and VMThread objects.
     */
    // vmThreadObj = dvmAllocObject(gDvm.classJavaLangVMThread, ALLOC_DEFAULT);

    /*
     * Create a string for the thread name.
     */
    if (!args->mName.IsNull()) {
        threadNameStr = args->mName;
        // if (threadNameStr == NULL) {
        //     assert(dvmCheckException(dvmThreadSelf()));
        //     goto fail_unlink;
        // }
    }

    // init = dvmFindDirectMethodByDescriptor(gDvm.classJavaLangThread, "<init>",
    //         "(Ljava/lang/ThreadGroup;Ljava/lang/String;IZ)V");
    // if (init == NULL) {
    //     assert(dvmCheckException(self));
    //     goto fail_unlink;
    // }

    /*
     * Now we're ready to run some interpreted code.
     *
     * We need to construct the Thread object and set the VMThread field.
     * Setting VMThread tells interpreted code that we're alive.
     *
     * Call the (group, name, priority, daemon) constructor on the Thread.
     * This sets the thread's name and adds it to the specified group, and
     * provides values for priority and daemon (which are normally inherited
     * from the current thread).
     */
    ec = CThread::NewByFriend(reinterpret_cast<ThreadGroup*>(args->mGroup),
            threadNameStr, os_getThreadPriorityFromSystem(), isDaemon, (CThread**)&threadObj);
    if (FAILED(ec)) {
    //     LOGE("exception thrown while constructing attached thread object\n");
        goto fail_unlink;
    }

    /*
     * This makes threadObj visible to the GC.  We still have it in the
     * tracked allocation table, so it can't move around on us.
     */
    self->mThreadObj = reinterpret_cast<Int32>((Thread*)threadObj.Get());

    /*
     * Set the VMThread field, which tells interpreted code that we're alive.
     *
     * The risk of a thread start collision here is very low; somebody
     * would have to be deliberately polling the ThreadGroup list and
     * trying to start threads against anything it sees, which would
     * generally cause problems for all thread creation.  However, for
     * correctness we test "vmThread" before setting it.
     *
     * TODO: this still has a race, it's just smaller.  Not sure this is
     * worth putting effort into fixing.  Need to hold a lock while
     * fiddling with the field, or maybe initialize the Thread object in a
     * way that ensures another thread can't call start() on it.
     */
//    if (dvmGetFieldObject(threadObj, gDvm.offJavaLangThread_vmThread) != NULL) {
//        LOGW("WOW: thread start hijack\n");
//        dvmThrowException("Ljava/lang/IllegalThreadStateException;",
//            "thread has already been started");
//        /* We don't want to free anything associated with the thread
//         * because someone is obviously interested in it.  Just let
//         * it go and hope it will clean itself up when its finished.
//         * This case should never happen anyway.
//         *
//         * Since we're letting it live, we need to finish setting it up.
//         * We just have to let the caller know that the intended operation
//         * has failed.
//         *
//         * [ This seems strange -- stepping on the vmThread object that's
//         * already present seems like a bad idea.  TODO: figure this out. ]
//         */
//        ret = false;
//    } else {
//        ret = true;
//    }
//    dvmSetFieldObject(threadObj, gDvm.offJavaLangThread_vmThread, vmThreadObj);
    ((Thread*)threadObj.Get())->mNativeThread = self;
//
//    /* we can now safely un-pin these */
//    dvmReleaseTrackedAlloc(threadObj, self);
//    dvmReleaseTrackedAlloc(vmThreadObj, self);
//    dvmReleaseTrackedAlloc((Object*)threadNameStr, self);

    // LOG_THREAD("threadid=%d: attached from native, name=%s\n",
    //     self->threadId, pArgs->name);

    /* tell the debugger & DDM */
    // if (gDvm.debuggerConnected)
    //     dvmDbgPostThreadStart(self);

    *thread = (IThread*)threadObj.Get();
    REFCOUNT_ADD(*thread);
    return NOERROR;

fail_unlink:
    NativeLockThreadList(self);
    UnlinkThread(self);
    if (!isDaemon) {
        gCore.mNonDaemonThreadCount--;
    }
    NativeUnlockThreadList();
    /* fall through to "fail" */
fail:
    // dvmReleaseTrackedAlloc(threadObj, self);
    // dvmReleaseTrackedAlloc(vmThreadObj, self);
    // dvmReleaseTrackedAlloc((Object*)threadNameStr, self);
    if (self != NULL) {
        // if (self->jniEnv != NULL) {
        //     dvmDestroyJNIEnv(self->jniEnv);
        //     self->jniEnv = NULL;
        // }
        FreeThread(self);
    }
    SetThreadSelf(NULL);
    *thread = NULL;
    return E_RUNTIME_EXCEPTION;
}

/*
 * Detach the thread from the various data structures, notify other threads
 * that are waiting to "join" it, and free up all heap-allocated storage.
 *
 * Used for all threads.
 *
 * When we get here the interpreted stack should be empty.  The JNI 1.6 spec
 * requires us to enforce this for the DetachCurrentThread call, probably
 * because it also says that DetachCurrentThread causes all monitors
 * associated with the thread to be released.  (Because the stack is empty,
 * we only have to worry about explicit JNI calls to MonitorEnter.)
 *
 * THOUGHT:
 * We might want to avoid freeing our internal Thread structure until the
 * associated Thread/VMThread objects get GCed.  Our Thread is impossible to
 * get to once the thread shuts down, but there is a small possibility of
 * an operation starting in another thread before this thread halts, and
 * finishing much later (perhaps the thread got stalled by a weird OS bug).
 * We don't want something like Thread.isInterrupted() crawling through
 * freed storage.  Can do with a Thread finalizer, or by creating a
 * dedicated ThreadObject class for java/lang/Thread and moving all of our
 * state into that.
 */
void NativeDetachCurrentThread()
{
    NativeThread* self = NativeThreadSelf();
    assert(self != NULL);
    Thread* thread = reinterpret_cast<Thread*>(self->mThreadObj);

    /*
     * Make sure we're not detaching a thread that's still running.  (This
     * could happen with an explicit JNI detach call.)
     *
     * A thread created by interpreted code will finish with a depth of
     * zero, while a JNI-attached thread will have the synthetic "stack
     * starter" native method at the top.
     */
    // int curDepth = dvmComputeExactFrameDepth(self->curFrame);
    // if (curDepth != 0) {
    //     bool topIsNative = false;

    //     if (curDepth == 1) {
    //         /* not expecting a lingering break frame; just look at curFrame */
    //         assert(!dvmIsBreakFrame(self->curFrame));
    //         StackSaveArea* ssa = SAVEAREA_FROM_FP(self->curFrame);
    //         if (dvmIsNativeMethod(ssa->method))
    //             topIsNative = true;
    //     }

    //     if (!topIsNative) {
    //         LOGE("ERROR: detaching thread with interp frames (count=%d)\n",
    //             curDepth);
    //         dvmDumpThread(self, false);
    //         dvmAbort();
    //     }
    // }

    AutoPtr<IThreadGroup> group = thread->mGroup;
    // group = dvmGetFieldObject(self->threadObj, gDvm.offJavaLangThread_group);
    // LOG_THREAD("threadid=%d: detach (group=%p)\n", self->threadId, group);

    /*
     * Release any held monitors.  Since there are no interpreted stack
     * frames, the only thing left are the monitors held by JNI MonitorEnter
     * calls.
     */
    // dvmReleaseJniMonitors(self);

    /*
     * Do some thread-exit uncaught exception processing if necessary.
     */
    // if (dvmCheckException(self))
    //     threadExitUncaughtException(self, group);

    /*
     * Remove the thread from the thread group.
     */
    AutoPtr<IThread> threadObj;
    if (group != NULL) {
        ThreadGroup* tg = (ThreadGroup*)group.Get();
        /*
         * Use threadObj to take the ref of self->mThreadObj;
         */
        threadObj = (IThread*)thread->Probe(EIID_IThread);
        tg->RemoveThread(threadObj);
    }

    /*
     * Clear the vmThread reference in the Thread object.  Interpreted code
     * will now see that this Thread is not running.  As this may be the
     * only reference to the VMThread object that the VM knows about, we
     * have to create an internal reference to it first.
     */
    // vmThread = dvmGetFieldObject(self->threadObj,
    //                 gDvm.offJavaLangThread_vmThread);
    // dvmAddTrackedAlloc(vmThread, self);
    // dvmSetFieldObject(self->threadObj, gDvm.offJavaLangThread_vmThread, NULL);
    thread->mNativeThread = NULL;

    /* clear out our struct Thread pointer, since it's going away */
    // dvmSetFieldObject(vmThread, gDvm.offJavaLangVMThread_vmData, NULL);

    /*
     * Tell the debugger & DDM.  This may cause the current thread or all
     * threads to suspend.
     *
     * The JDWP spec is somewhat vague about when this happens, other than
     * that it's issued by the dying thread, which may still appear in
     * an "all threads" listing.
     */
    // if (gDvm.debuggerConnected)
    //     dvmDbgPostThreadDeath(self);

    /*
     * Thread.join() is implemented as an Object.wait() on the VMThread
     * object.  Signal anyone who is waiting.
     */
    thread->Lock();
    thread->NotifyAll();
    thread->Unlock();

    // dvmReleaseTrackedAlloc(vmThread, self);
    // vmThread = NULL;

    /*
     * We're done manipulating objects, so it's okay if the GC runs in
     * parallel with us from here out.  It's important to do this if
     * profiling is enabled, since we can wait indefinitely.
     */
    android_atomic_release_store(NTHREAD_VMWAIT, (int32_t*)&self->mStatus);

    /*
     * If we're doing method trace profiling, we don't want threads to exit,
     * because if they do we'll end up reusing thread IDs.  This complicates
     * analysis and makes it impossible to have reasonable output in the
     * "threads" section of the "key" file.
     *
     * We need to do this after Thread.join() completes, or other threads
     * could get wedged.  Since self->threadObj is still valid, the Thread
     * object will not get GCed even though we're no longer in the ThreadGroup
     * list (which is important since the profiling thread needs to get
     * the thread's name).
     */
//    MethodTraceState* traceState = &gDvm.methodTrace;
//
//    dvmLockMutex(&traceState->startStopLock);
//    if (traceState->traceEnabled) {
//        LOGI("threadid=%d: waiting for method trace to finish\n",
//            self->threadId);
//        while (traceState->traceEnabled) {
//            dvmWaitCond(&traceState->threadExitCond,
//                        &traceState->startStopLock);
//        }
//    }
//    dvmUnlockMutex(&traceState->startStopLock);

    NativeLockThreadList(self);

    /*
     * Lose the JNI context.
     */
    // dvmDestroyJNIEnv(self->jniEnv);
    // self->jniEnv = NULL;

    self->mStatus = NTHREAD_ZOMBIE;

    /*
     * Remove ourselves from the internal thread list.
     */
    UnlinkThread(self);

    /*
     * If we're the last one standing, signal anybody waiting in
     * DestroyJavaVM that it's okay to exit.
     */
//    if (!dvmGetFieldBoolean(self->threadObj, gDvm.offJavaLangThread_daemon)) {
//        gDvm.nonDaemonThreadCount--;        // guarded by thread list lock
//
//        if (gDvm.nonDaemonThreadCount == 0) {
//            int cc;
//
//            LOGV("threadid=%d: last non-daemon thread\n", self->threadId);
//            //dvmDumpAllThreads(false);
//            // cond var guarded by threadListLock, which we already hold
//            cc = pthread_cond_signal(&gDvm.vmExitCond);
//            assert(cc == 0);
//        }
//    }

    // LOGV("threadid=%d: bye!\n", self->threadId);
    ReleaseThreadId(self);
    NativeUnlockThreadList();

    SetThreadSelf(NULL);

    FreeThread(self);
}

/*
 * Check to see if we need to suspend ourselves.  If so, go to sleep on
 * a condition variable.
 *
 * Returns "true" if we suspended ourselves.
 */
static Boolean FullSuspendCheck(
    /* [in] */ NativeThread* self)
{
    assert(self != NULL);
    assert(self->mSuspendCount >= 0);

    /*
     * Grab gDvm.threadSuspendCountLock.  This gives us exclusive write
     * access to self->suspendCount.
     */
//    lockThreadSuspendCount();   /* grab gDvm.threadSuspendCountLock */

    Boolean needSuspend = (self->mSuspendCount != 0);
    if (needSuspend) {
//        LOG_THREAD("threadid=%d: self-suspending\n", self->threadId);
//        ThreadStatus oldStatus = self->status;      /* should be RUNNING */
//        self->status = THREAD_SUSPENDED;
//
//        while (self->suspendCount != 0) {
//            /*
//             * Wait for wakeup signal, releasing lock.  The act of releasing
//             * and re-acquiring the lock provides the memory barriers we
//             * need for correct behavior on SMP.
//             */
//            dvmWaitCond(&gDvm.threadSuspendCountCond,
//                    &gDvm.threadSuspendCountLock);
//        }
//        assert(self->suspendCount == 0 && self->dbgSuspendCount == 0);
//        self->status = oldStatus;
//        LOG_THREAD("threadid=%d: self-reviving, status=%d\n",
//            self->threadId, self->status);
    }

//    unlockThreadSuspendCount();

    return needSuspend;
}

/*
 * Update our status.
 *
 * The "self" argument, which may be NULL, is accepted as an optimization.
 *
 * Returns the old status.
 */
NativeThreadStatus NativeChangeStatus(
    /* [in] */ NativeThread* self,
    /* [in] */ NativeThreadStatus newStatus)
{
    NativeThreadStatus oldStatus;

    if (self == NULL) {
        self = NativeThreadSelf();
    }

    // LOGVV("threadid=%d: (status %d -> %d)\n",
    //     self->mThreadId, self->mStatus, newStatus);

    oldStatus = self->mStatus;

    if (newStatus == NTHREAD_RUNNING) {
        /*
         * Change our status to THREAD_RUNNING.  The transition requires
         * that we check for pending suspension, because the VM considers
         * us to be "asleep" in all other states, and another thread could
         * be performing a GC now.
         *
         * The order of operations is very significant here.  One way to
         * do this wrong is:
         *
         *   GCing thread                   Our thread (in NATIVE)
         *   ------------                   ----------------------
         *                                  check suspend count (== 0)
         *   dvmSuspendAllThreads()
         *   grab suspend-count lock
         *   increment all suspend counts
         *   release suspend-count lock
         *   check thread state (== NATIVE)
         *   all are suspended, begin GC
         *                                  set state to RUNNING
         *                                  (continue executing)
         *
         * We can correct this by grabbing the suspend-count lock and
         * performing both of our operations (check suspend count, set
         * state) while holding it, now we need to grab a mutex on every
         * transition to RUNNING.
         *
         * What we do instead is change the order of operations so that
         * the transition to RUNNING happens first.  If we then detect
         * that the suspend count is nonzero, we switch to SUSPENDED.
         *
         * Appropriate compiler and memory barriers are required to ensure
         * that the operations are observed in the expected order.
         *
         * This does create a small window of opportunity where a GC in
         * progress could observe what appears to be a running thread (if
         * it happens to look between when we set to RUNNING and when we
         * switch to SUSPENDED).  At worst this only affects assertions
         * and thread logging.  (We could work around it with some sort
         * of intermediate "pre-running" state that is generally treated
         * as equivalent to running, but that doesn't seem worthwhile.)
         *
         * We can also solve this by combining the "status" and "suspend
         * count" fields into a single 32-bit value.  This trades the
         * store/load barrier on transition to RUNNING for an atomic RMW
         * op on all transitions and all suspend count updates (also, all
         * accesses to status or the thread count require bit-fiddling).
         * It also eliminates the brief transition through RUNNING when
         * the thread is supposed to be suspended.  This is possibly faster
         * on SMP and slightly more correct, but less convenient.
         */
        assert(oldStatus != NTHREAD_RUNNING);
        android_atomic_acquire_store(newStatus, (int32_t*)&self->mStatus);
        if (self->mSuspendCount != 0) {
            FullSuspendCheck(self);
        }
    }
    else {
        /*
         * Not changing to THREAD_RUNNING.  No additional work required.
         *
         * We use a releasing store to ensure that, if we were RUNNING,
         * any updates we previously made to objects on the managed heap
         * will be observed before the state change.
         */
        assert(newStatus != NTHREAD_SUSPENDED);
        android_atomic_release_store(newStatus, (int32_t*)&self->mStatus);
    }

    return oldStatus;
}

AutoPtr<IThreadGroup> NativeGetSystemThreadGroup()
{
    return CThreadGroup::GetSystemThreadGroup();
}

AutoPtr<IThreadGroup> NativeGetMainThreadGroup()
{
    return CThreadGroup::GetMainThreadGroup();
}

/*
 * Given a VMThread object, return the associated Thread*.
 *
 * NOTE: if the thread detaches, the struct Thread will disappear, and
 * we will be touching invalid data.  For safety, lock the thread list
 * before calling this.
 */
NativeThread* NativeGetThreadFromThreadObject(
    /* [in] */ Int32 threadObj)
{
    // Int32 vmData;

    // vmData = dvmGetFieldInt(vmThreadObj, gDvm.offJavaLangVMThread_vmData);
    assert(threadObj != 0);
    NativeThread* thread = reinterpret_cast<Thread*>(threadObj)->mNativeThread;
    if (FALSE) {
        thread = gCore.mThreadList;
        while (thread != NULL) {
            if (threadObj == thread->mThreadObj)
                break;

            thread = thread->mNext;
        }
        // if (thread == NULL) {
        //     //LOGW("WARNING: vmThreadObj=%p has thread=%p, not in thread list\n",
        //     //    vmThreadObj, (Thread*)vmData);
        //     vmData = 0;
        // }
    }

    return thread;
}

/*
 * Conversion map for "nice" values.
 *
 * We use Android thread priority constants to be consistent with the rest
 * of the system.  In some cases adjacent entries may overlap.
 */
static const Int32 kNiceValues[10] = {
    ELASTOS_PRIORITY_LOWEST,                /* 1 (MIN_PRIORITY) */
    ELASTOS_PRIORITY_BACKGROUND + 6,
    ELASTOS_PRIORITY_BACKGROUND + 3,
    ELASTOS_PRIORITY_BACKGROUND,
    ELASTOS_PRIORITY_NORMAL,                /* 5 (NORM_PRIORITY) */
    ELASTOS_PRIORITY_NORMAL - 2,
    ELASTOS_PRIORITY_NORMAL - 4,
    ELASTOS_PRIORITY_URGENT_DISPLAY + 3,
    ELASTOS_PRIORITY_URGENT_DISPLAY + 2,
    ELASTOS_PRIORITY_URGENT_DISPLAY         /* 10 (MAX_PRIORITY) */
};

/*
 * Change the priority of a system thread to match that of the Thread object.
 *
 * We map a priority value from 1-10 to Linux "nice" values, where lower
 * numbers indicate higher priority.
 */
void NativeChangeThreadPriority(
    /* [in] */ NativeThread* thread,
    /* [in] */ Int32 newPriority)
{
    pid_t pid = thread->mSystemTid;
    Int32 newNice;

    if (newPriority < 1 || newPriority > 10) {
        // LOGW("bad priority %d\n", newPriority);
        newPriority = 5;
    }
    newNice = kNiceValues[newPriority - 1];

    if (newNice >= ELASTOS_PRIORITY_BACKGROUND) {
        set_sched_policy(NativeGetSysThreadId(), SP_BACKGROUND);
    }
    else if (getpriority(PRIO_PROCESS, pid) >= ELASTOS_PRIORITY_BACKGROUND) {
        set_sched_policy(NativeGetSysThreadId(), SP_FOREGROUND);
    }

    if (setpriority(PRIO_PROCESS, pid, newNice) != 0) {
        //const char* str = thread->mThreadObj->mName.string();
        // LOGI("setPriority(%d) '%s' to prio=%d(n=%d) failed: %s\n",
        //     pid, str, newPriority, newNice, strerror(errno));
    }
    else {
        // LOGV("setPriority(%d) to prio=%d(n=%d)\n",
        //     pid, newPriority, newNice);
    }
}

Int32 NativeGetCount()
{
    NativeLockMutex(&gCore.mThreadCountLock);
    Int32 count = gCore.mThreadCount++;
    NativeUnlockMutex(&gCore.mThreadCountLock);
    return count;
}

//sync
/*
 * Create and initialize a monitor.
 */
static Monitor* NativeCreateMonitor(
    /* [in] */ NativeObject* obj)
{
    Monitor* mon;

    mon = (Monitor*)calloc(1, sizeof(Monitor));
    if (mon == NULL) {
        //LOGE("Unable to allocate monitor\n");
        //dvmAbort();
    }
    if (((UInt32)mon & 7) != 0) {
        //LOGE("Misaligned monitor: %p\n", mon);
        //dvmAbort();
    }
    //mon->obj = obj;
    NativeInitMutex(&mon->mLock);

    /* replace the head of the list with the new monitor */
    do {
        mon->mNext = gCore.mMonitorList;
    } while (android_atomic_release_cas((int32_t)mon->mNext, (int32_t)mon,
           (int32_t*)(void*)&gCore.mMonitorList) != 0);

    return mon;
}

/*
 * Lock a monitor.
 */
static void LockMonitor(
    /* [in] */ NativeThread* self,
    /* [in] */ Monitor* mon)
{
    NativeThreadStatus oldStatus;
    // UInt32 waitThreshold, samplePercent;
    // UInt64 waitStart, waitEnd, waitMs;

    if (mon->mOwner == self) {
        mon->mLockCount++;
        return;
    }
    if (NativeTryLockMutex(&mon->mLock) != 0) {
        oldStatus = NativeChangeStatus(self, NTHREAD_MONITOR);
        // waitThreshold = gCore.mLockProfThreshold;
        // if (waitThreshold) {
        //     waitStart = NativeGetRelativeTimeUsec();
        // }
        //const char* currentOwnerFileName = mon->mOwnerFileName;
        //UInt32 currentOwnerLineNumber = mon->mOwnerLineNumber;

        NativeLockMutex(&mon->mLock);
        // if (waitThreshold) {
        //     waitEnd = NativeGetRelativeTimeUsec();
        // }
        NativeChangeStatus(self, oldStatus);
        // if (waitThreshold) {
        //     waitMs = (waitEnd - waitStart) / 1000;
        //     if (waitMs >= waitThreshold) {
        //         samplePercent = 100;
        //     }
        //     else {
        //         samplePercent = 100 * waitMs / waitThreshold;
        //     }
        //     if (samplePercent != 0 && ((UInt32)rand() % 100 < samplePercent)) {
        //         logContentionEvent(self, waitMs, samplePercent,
        //                            currentOwnerFileName, currentOwnerLineNumber);
        //     }
        // }
    }
    mon->mOwner = self;
    assert(mon->mLockCount == 0);

    // When debugging, save the current monitor holder for future
    // acquisition failures to use in sampled logging.
    // if (gCore.mLockProfThreshold > 0) {
    //     const StackSaveArea *saveArea;
    //     const Method *meth;
    //     mon->mOwnerLineNumber = 0;
    //     if (self->curFrame == NULL) {
    //     mon->mOwnerFileName = String("no_frame");
    //     } else if ((saveArea = SAVEAREA_FROM_FP(self->curFrame)) == NULL) {
    //         mon->ownerFileName = "no_save_area";
    //     } else if ((meth = saveArea->method) == NULL) {
    //         mon->ownerFileName = "no_method";
    //     } else {
    //         u4 relativePc = saveArea->xtra.currentPc - saveArea->method->insns;
    //         mon->ownerFileName = (char*) dvmGetMethodSourceFile(meth);
    //         if (mon->ownerFileName == NULL) {
    //             mon->ownerFileName = "no_method_file";
    //         } else {
    //             mon->ownerLineNumber = dvmLineNumFromPC(meth, relativePc);
    //         }
    //     }
    // }
}

/*
 * Unlock a monitor.
 *
 * Returns true if the unlock succeeded.
 * If the unlock failed, an exception will be pending.
 */
static ECode UnlockMonitor(
    /* [in] */ NativeThread* self,
    /* [in] */ Monitor* mon)
{
    assert(self != NULL);
    assert(mon != NULL);
    if (mon->mOwner == self) {
        /*
         * We own the monitor, so nobody else can be in here.
         */
        if (mon->mLockCount == 0) {
            mon->mOwner = NULL;
            mon->mOwnerFileName = "unlocked";
            mon->mOwnerLineNumber = 0;
            NativeUnlockMutex(&mon->mLock);
        }
        else {
            mon->mLockCount--;
        }
    }
    else {
        /*
         * We don't own this, so we're not allowed to unlock it.
         * The JNI spec says that we should throw IllegalMonitorStateException
         * in this case.
         */
        //dvmThrowException("Ljava/lang/IllegalMonitorStateException;",
        //                  "unlock of unowned monitor");
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }
    return NOERROR;
}

/*
 * Checks the wait set for circular structure.  Returns 0 if the list
 * is not circular.  Otherwise, returns 1.  Used only by asserts.
 */
#ifndef NDEBUG
static Int32 WaitSetCheck(
    /* [in] */ Monitor* mon)
{
    NativeThread *fast, *slow;
    size_t n;

    assert(mon != NULL);
    fast = slow = mon->mWaitSet;
    n = 0;
    for (;;) {
        if (fast == NULL) return 0;
        if (fast->mWaitNext == NULL) return 0;
        if (fast == slow && n > 0) return 1;
        n += 2;
        fast = fast->mWaitNext->mWaitNext;
        slow = slow->mWaitNext;
    }
}
#endif

/*
 * Links a thread into a monitor's wait set.  The monitor lock must be
 * held by the caller of this routine.
 */
static void WaitSetAppend(
    /* [in] */ Monitor* mon,
    /* [in] */ NativeThread* thread)
{
    NativeThread* elt;

    assert(mon != NULL);
    assert(mon->mOwner == NativeThreadSelf());
    assert(thread != NULL);
    assert(thread->mWaitNext == NULL);
    assert(WaitSetCheck(mon) == 0);
    if (mon->mWaitSet == NULL) {
        mon->mWaitSet = thread;
        return;
    }
    elt = mon->mWaitSet;
    while (elt->mWaitNext != NULL) {
        elt = elt->mWaitNext;
    }
    elt->mWaitNext = thread;
}

/*
 * Unlinks a thread from a monitor's wait set.  The monitor lock must
 * be held by the caller of this routine.
 */
static void WaitSetRemove(
    /* [in] */ Monitor* mon,
    /* [in] */ NativeThread* thread)
{
    NativeThread *elt;

    assert(mon != NULL);
    assert(mon->mOwner == NativeThreadSelf());
    assert(thread != NULL);
    assert(WaitSetCheck(mon) == 0);
    if (mon->mWaitSet == NULL) {
        return;
    }
    if (mon->mWaitSet == thread) {
        mon->mWaitSet = thread->mWaitNext;
        thread->mWaitNext = NULL;
        return;
    }
    elt = mon->mWaitSet;
    while (elt->mWaitNext != NULL) {
        if (elt->mWaitNext == thread) {
            elt->mWaitNext = thread->mWaitNext;
            thread->mWaitNext = NULL;
            return;
        }
        elt = elt->mWaitNext;
    }
}

/*
 * Converts the given relative waiting time into an absolute time.
 */
void AbsoluteTime(
    /* [in] */ Int64 msec,
    /* [in] */ Int32 nsec,
    /* [in] */ struct timespec* ts)
{
    Int64 endSec;

#ifdef HAVE_TIMEDWAIT_MONOTONIC
    clock_gettime(CLOCK_MONOTONIC, ts);
#else
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        ts->tv_sec = tv.tv_sec;
        ts->tv_nsec = tv.tv_usec * 1000;
    }
#endif
    endSec = ts->tv_sec + msec / 1000;
    if (endSec >= 0x7fffffff) {
        //LOGV("NOTE: end time exceeds epoch\n");
        endSec = 0x7ffffffe;
    }
    ts->tv_sec = endSec;
    ts->tv_nsec = (ts->tv_nsec + (msec % 1000) * 1000000) + nsec;

    /* catch rollover */
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

/*
 * Wait on a monitor until timeout, interrupt, or notification.  Used for
 * Object.wait() and (somewhat indirectly) Thread.sleep() and Thread.join().
 *
 * If another thread calls Thread.interrupt(), we throw InterruptedException
 * and return immediately if one of the following are true:
 *  - blocked in wait(), wait(long), or wait(long, int) methods of Object
 *  - blocked in join(), join(long), or join(long, int) methods of Thread
 *  - blocked in sleep(long), or sleep(long, int) methods of Thread
 * Otherwise, we set the "interrupted" flag.
 *
 * Checks to make sure that "nsec" is in the range 0-999999
 * (i.e. fractions of a millisecond) and throws the appropriate
 * exception if it isn't.
 *
 * The spec allows "spurious wakeups", and recommends that all code using
 * Object.wait() do so in a loop.  This appears to derive from concerns
 * about pthread_cond_wait() on multiprocessor systems.  Some commentary
 * on the web casts doubt on whether these can/should occur.
 *
 * Since we're allowed to wake up "early", we clamp extremely long durations
 * to return at the end of the 32-bit time epoch.
 */
static ECode WaitMonitor(
    /* [in] */ NativeThread* self,
    /* [in] */ Monitor* mon,
    /* [in] */ Int64 msec,
    /* [in] */ Int32 nsec,
    /* [in] */ Boolean interruptShouldThrow)
{
    struct timespec ts;
    Boolean wasInterrupted = FALSE;
    Boolean timed;
    Int32 UNUSED(ret);
    String savedFileName;
    UInt32 savedLineNumber;

    assert(self != NULL);
    assert(mon != NULL);

    /* Make sure that we hold the lock. */
    if (mon->mOwner != self) {
        // dvmThrowException("Ljava/lang/IllegalMonitorStateException;",
        //     "object not locked by thread before wait()");
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }

    /*
     * Enforce the timeout range.
     */
    if (msec < 0 || nsec < 0 || nsec > 999999) {
        // dvmThrowException("Ljava/lang/IllegalArgumentException;",
        //     "timeout arguments out of range");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    /*
     * Compute absolute wakeup time, if necessary.
     */
    if (msec == 0 && nsec == 0) {
        timed = FALSE;
    }
    else {
        AbsoluteTime(msec, nsec, &ts);
        timed = TRUE;
    }

    /*
     * Add ourselves to the set of threads waiting on this monitor, and
     * release our hold.  We need to let it go even if we're a few levels
     * deep in a recursive lock, and we need to restore that later.
     *
     * We append to the wait set ahead of clearing the count and owner
     * fields so the subroutine can check that the calling thread owns
     * the monitor.  Aside from that, the order of member updates is
     * not order sensitive as we hold the pthread mutex.
     */
    WaitSetAppend(mon, self);
    Int32 prevLockCount = mon->mLockCount;
    mon->mLockCount = 0;
    mon->mOwner = NULL;
    savedFileName = mon->mOwnerFileName;
    mon->mOwnerFileName = NULL;
    savedLineNumber = mon->mOwnerLineNumber;
    mon->mOwnerLineNumber = 0;

    /*
     * Update thread status.  If the GC wakes up, it'll ignore us, knowing
     * that we won't touch any references in this state, and we'll check
     * our suspend mode before we transition out.
     */
    if (timed) {
        NativeChangeStatus(self, NTHREAD_TIMED_WAIT);
    }
    else {
        NativeChangeStatus(self, NTHREAD_WAIT);
    }

    NativeLockMutex(&self->mWaitMutex);

    /*
     * Set waitMonitor to the monitor object we will be waiting on.
     * When waitMonitor is non-NULL a notifying or interrupting thread
     * must signal the thread's waitCond to wake it up.
     */
    assert(self->mWaitMonitor == NULL);
    self->mWaitMonitor = mon;

    /*
     * Handle the case where the thread was interrupted before we called
     * wait().
     */
    if (self->mInterrupted) {
        wasInterrupted = TRUE;
        self->mWaitMonitor = NULL;
        NativeUnlockMutex(&self->mWaitMutex);
        goto done;
    }

    /*
     * Release the monitor lock and wait for a notification or
     * a timeout to occur.
     */
    NativeUnlockMutex(&mon->mLock);

    if (!timed) {
        ret = pthread_cond_wait(&self->mWaitCond, &self->mWaitMutex);
        assert(ret == 0);
    }
    else {
#ifdef HAVE_TIMEDWAIT_MONOTONIC
        ret = pthread_cond_timedwait_monotonic(&self->mWaitCond, &self->mWaitMutex, &ts);
#else
        ret = pthread_cond_timedwait(&self->mWaitCond, &self->mWaitMutex, &ts);
#endif
        assert(ret == 0 || ret == ETIMEDOUT);
    }
    if (self->mInterrupted) {
        wasInterrupted = TRUE;
    }

    self->mInterrupted = FALSE;
    self->mWaitMonitor = NULL;

    NativeUnlockMutex(&self->mWaitMutex);

    /* Reacquire the monitor lock. */
    LockMonitor(self, mon);

done:
    /*
     * We remove our thread from wait set after restoring the count
     * and owner fields so the subroutine can check that the calling
     * thread owns the monitor. Aside from that, the order of member
     * updates is not order sensitive as we hold the pthread mutex.
     */
    mon->mOwner = self;
    mon->mLockCount = prevLockCount;
    mon->mOwnerFileName = savedFileName;
    mon->mOwnerLineNumber = savedLineNumber;
    WaitSetRemove(mon, self);

    /* set self->status back to THREAD_RUNNING, and self-suspend if needed */
    NativeChangeStatus(self, NTHREAD_RUNNING);

    if (wasInterrupted) {
        /*
         * We were interrupted while waiting, or somebody interrupted an
         * un-interruptible thread earlier and we're bailing out immediately.
         *
         * The doc sayeth: "The interrupted status of the current thread is
         * cleared when this exception is thrown."
         */
        self->mInterrupted = FALSE;
        if (interruptShouldThrow) {
            //dvmThrowException("Ljava/lang/InterruptedException;", NULL);
            return E_INTERRUPTED_EXCEPTION;
        }
    }
    return NOERROR;
}

/*
 * Notify one thread waiting on this monitor.
 */
static ECode NotifyMonitor(
    /* [in] */ NativeThread* self,
    /* [in] */ Monitor* mon)
{
    NativeThread* thread;

    assert(self != NULL);
    assert(mon != NULL);

    /* Make sure that we hold the lock. */
    if (mon->mOwner != self) {
        // dvmThrowException("Ljava/lang/IllegalMonitorStateException;",
        //     "object not locked by thread before notify()");
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }
    /* Signal the first waiting thread in the wait set. */
    while (mon->mWaitSet != NULL) {
        thread = mon->mWaitSet;
        mon->mWaitSet = thread->mWaitNext;
        thread->mWaitNext = NULL;
        NativeLockMutex(&thread->mWaitMutex);
        /* Check to see if the thread is still waiting. */
        if (thread->mWaitMonitor != NULL) {
            pthread_cond_signal(&thread->mWaitCond);
            NativeUnlockMutex(&thread->mWaitMutex);
            return NOERROR;
        }
        NativeUnlockMutex(&thread->mWaitMutex);
    }
    return NOERROR;
}

/*
 * Notify all threads waiting on this monitor.
 */
static ECode NotifyAllMonitor(
    /* [in] */ NativeThread* self,
    /* [in] */ Monitor* mon)
{
    NativeThread* thread;

    assert(self != NULL);
    assert(mon != NULL);

    /* Make sure that we hold the lock. */
    if (mon->mOwner != self) {
        // dvmThrowException("Ljava/lang/IllegalMonitorStateException;",
        //     "object not locked by thread before notifyAll()");
        return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
    }
    /* Signal all threads in the wait set. */
    while (mon->mWaitSet != NULL) {
        thread = mon->mWaitSet;
        mon->mWaitSet = thread->mWaitNext;
        thread->mWaitNext = NULL;
        NativeLockMutex(&thread->mWaitMutex);
        /* Check to see if the thread is still waiting. */
        if (thread->mWaitMonitor != NULL) {
            pthread_cond_signal(&thread->mWaitCond);
        }
        NativeUnlockMutex(&thread->mWaitMutex);
    }
    return NOERROR;
}

/*
 * Changes the shape of a monitor from thin to fat, preserving the
 * internal lock state.  The calling thread must own the lock.
 */
static void InflateMonitor(
    /* [in] */ NativeThread *self,
    /* [in] */ NativeObject *obj)
{
    Monitor *mon;
    UInt32 thin;

    assert(self != NULL);
    assert(obj != NULL);
    assert(LW_SHAPE(obj->mLock) == LW_SHAPE_THIN);
    assert(LW_LOCK_OWNER(obj->mLock) == self->mThreadId);
    /* Allocate and acquire a new monitor. */
    mon = NativeCreateMonitor(obj);
    LockMonitor(self, mon);
    /* Propagate the lock state. */
    thin = obj->mLock;
    mon->mLockCount = LW_LOCK_COUNT(thin);
    thin &= LW_HASH_STATE_MASK << LW_HASH_STATE_SHIFT;
    thin |= (UInt32)mon | LW_SHAPE_FAT;
    /* Publish the updated lock word. */
    android_atomic_release_store(thin, (int32_t *)&obj->mLock);
}

NativeObject* NativeCreateObject()
{
    NativeObject* obj = (NativeObject*)calloc(1, sizeof(NativeObject));
    if (obj == NULL) {
        return NULL;
    }
    NATIVE_LOCK_INIT(&(obj)->mLock);
    return obj;
}

void NativeDestroyObject(
    /* [in] */ NativeObject* obj)
{
    free(obj);
}

/*
 * Implements monitorenter for "synchronized" stuff.
 *
 * This does not fail or throw an exception (unless deadlock prediction
 * is enabled and set to "err" mode).
 */
ECode NativeLockObject(
    /* [in] */ NativeObject *obj)
{
    volatile UInt32* thinp;
    NativeThreadStatus oldStatus;
    useconds_t sleepDelay;
    const useconds_t maxSleepDelay = 1 << 20;
    UInt32 thin, newThin, threadId;

    NativeThread* self = NativeThreadSelf();
    assert(self != NULL);
    assert(obj != NULL);
    threadId = self->mThreadId;
    thinp = &obj->mLock;
retry:
    thin = *thinp;
    if (LW_SHAPE(thin) == LW_SHAPE_THIN) {
        /*
         * The lock is a thin lock.  The owner field is used to
         * determine the acquire method, ordered by cost.
         */
        if (LW_LOCK_OWNER(thin) == threadId) {
            /*
             * The calling thread owns the lock.  Increment the
             * value of the recursion count field.
             */
            obj->mLock += 1 << LW_LOCK_COUNT_SHIFT;
            if (LW_LOCK_COUNT(obj->mLock) == LW_LOCK_COUNT_MASK) {
                /*
                 * The reacquisition limit has been reached.  Inflate
                 * the lock so the next acquire will not overflow the
                 * recursion count field.
                 */
                InflateMonitor(self, obj);
            }
        }
        else if (LW_LOCK_OWNER(thin) == 0) {
            /*
             * The lock is unowned.  Install the thread id of the
             * calling thread into the owner field.  This is the
             * common case.  In performance critical code the JIT
             * will have tried this before calling out to the VM.
             */
            newThin = thin | (threadId << LW_LOCK_OWNER_SHIFT);
            if (android_atomic_acquire_cas(thin, newThin,
                    (int32_t*)thinp) != 0) {
                /*
                 * The acquire failed.  Try again.
                 */
                goto retry;
            }
        }
        else {
            // LOG_THIN("(%d) spin on lock %p: %#x (%#x) %#x",
            //          threadId, &obj->lock, 0, *thinp, thin);
            /*
             * The lock is owned by another thread.  Notify the VM
             * that we are about to wait.
             */
            oldStatus = NativeChangeStatus(self, NTHREAD_MONITOR);
            /*
             * Spin until the thin lock is released or inflated.
             */
            sleepDelay = 0;
            for (;;) {
                thin = *thinp;
                /*
                 * Check the shape of the lock word.  Another thread
                 * may have inflated the lock while we were waiting.
                 */
                if (LW_SHAPE(thin) == LW_SHAPE_THIN) {
                    if (LW_LOCK_OWNER(thin) == 0) {
                        /*
                         * The lock has been released.  Install the
                         * thread id of the calling thread into the
                         * owner field.
                         */
                        newThin = thin | (threadId << LW_LOCK_OWNER_SHIFT);
                        if (android_atomic_acquire_cas(thin, newThin,
                                (int32_t *)thinp) == 0) {
                            /*
                             * The acquire succeed.  Break out of the
                             * loop and proceed to inflate the lock.
                             */
                            break;
                        }
                    }
                    else {
                        /*
                         * The lock has not been released.  Yield so
                         * the owning thread can run.
                         */
                        if (sleepDelay == 0) {
                            sched_yield();
                            sleepDelay = 1000;
                        }
                        else {
                            usleep(sleepDelay);
                            if (sleepDelay < maxSleepDelay / 2) {
                                sleepDelay *= 2;
                            }
                        }
                    }
                }
                else {
                    /*
                     * The thin lock was inflated by another thread.
                     * Let the VM know we are no longer waiting and
                     * try again.
                     */
                    // LOG_THIN("(%d) lock %p surprise-fattened",
                    //          threadId, &obj->lock);
                    NativeChangeStatus(self, oldStatus);
                    goto retry;
                }
            }
            // LOG_THIN("(%d) spin on lock done %p: %#x (%#x) %#x",
            //          threadId, &obj->lock, 0, *thinp, thin);
            /*
             * We have acquired the thin lock.  Let the VM know that
             * we are no longer waiting.
             */
            NativeChangeStatus(self, oldStatus);
            /*
             * Fatten the lock.
             */
            InflateMonitor(self, obj);
            // LOG_THIN("(%d) lock %p fattened", threadId, &obj->lock);
        }
    }
    else {
        /*
         * The lock is a fat lock.
         */
        assert(LW_MONITOR(obj->mLock) != NULL);
        LockMonitor(self, LW_MONITOR(obj->mLock));
    }
//#ifdef WITH_DEADLOCK_PREDICTION
//    /*
//     * See if we were allowed to grab the lock at this time.  We do it
//     * *after* acquiring the lock, rather than before, so that we can
//     * freely update the Monitor struct.  This seems counter-intuitive,
//     * but our goal is deadlock *prediction* not deadlock *prevention*.
//     * (If we actually deadlock, the situation is easy to diagnose from
//     * a thread dump, so there's no point making a special effort to do
//     * the checks before the lock is held.)
//     *
//     * This needs to happen before we add the object to the thread's
//     * monitor list, so we can tell the difference between first-lock and
//     * re-lock.
//     *
//     * It's also important that we do this while in THREAD_RUNNING, so
//     * that we don't interfere with cleanup operations in the GC.
//     */
//    if (gDvm.deadlockPredictMode != kDPOff) {
//        if (self->status != NTHREAD_RUNNING) {
//            LOGE("Bad thread status (%d) in DP\n", self->status);
//            dvmDumpThread(self, false);
//            dvmAbort();
//        }
//        assert(!dvmCheckException(self));
//        updateDeadlockPrediction(self, obj);
//        if (dvmCheckException(self)) {
//            /*
//             * If we're throwing an exception here, we need to free the
//             * lock.  We add the object to the thread's monitor list so the
//             * "unlock" code can remove it.
//             */
//            dvmAddToMonitorList(self, obj, false);
//            dvmUnlockObject(self, obj);
//            LOGV("--- unlocked, pending is '%s'\n",
//                dvmGetException(self)->clazz->descriptor);
//        }
//    }
//
//    /*
//     * Add the locked object, and the current stack trace, to the list
//     * held by the Thread object.  If deadlock prediction isn't on,
//     * don't capture the stack trace.
//     */
//    dvmAddToMonitorList(self, obj, gDvm.deadlockPredictMode != kDPOff);
//#elif defined(WITH_MONITOR_TRACKING)
//    /*
//     * Add the locked object to the list held by the Thread object.
//     */
//    dvmAddToMonitorList(self, obj, false);
//#endif
    return NOERROR;
}

/*
 * Implements monitorexit for "synchronized" stuff.
 *
 * On failure, throws an exception and returns "false".
 */
ECode NativeUnlockObject(
    /* [in] */ NativeObject *obj)
{
    UInt32 thin;

    NativeThread* self = NativeThreadSelf();
    assert(self != NULL);
    assert(self->mStatus == NTHREAD_RUNNING);
    assert(obj != NULL);
    /*
     * Cache the lock word as its value can change while we are
     * examining its state.
     */
    thin = obj->mLock;
    if (LW_SHAPE(thin) == LW_SHAPE_THIN) {
        /*
         * The lock is thin.  We must ensure that the lock is owned
         * by the given thread before unlocking it.
         */
        if (LW_LOCK_OWNER(thin) == self->mThreadId) {
            /*
             * We are the lock owner.  It is safe to update the lock
             * without CAS as lock ownership guards the lock itself.
             */
            if (LW_LOCK_COUNT(thin) == 0) {
                /*
                 * The lock was not recursively acquired, the common
                 * case.  Unlock by clearing all bits except for the
                 * hash state.
                 */
                obj->mLock &= (LW_HASH_STATE_MASK << LW_HASH_STATE_SHIFT);
            }
            else {
                /*
                 * The object was recursively acquired.  Decrement the
                 * lock recursion count field.
                 */
                obj->mLock -= 1 << LW_LOCK_COUNT_SHIFT;
            }
        }
        else {
            /*
             * We do not own the lock.  The JVM spec requires that we
             * throw an exception in this case.
             */
            // dvmThrowException("Ljava/lang/IllegalMonitorStateException;",
            //                   "unlock of unowned monitor");
            return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
        }
    }
    else {
        /*
         * The lock is fat.  We must check to see if unlockMonitor has
         * raised any exceptions before continuing.
         */
        assert(LW_MONITOR(obj->mLock) != NULL);
        ECode ec = UnlockMonitor(self, LW_MONITOR(obj->mLock));
        if (FAILED(ec)) {
            /*
             * An exception has been raised.  Do not fall through.
             */
            return ec;
        }
    }

//#ifdef WITH_MONITOR_TRACKING
//    /*
//     * Remove the object from the Thread's list.
//     */
//    dvmRemoveFromMonitorList(self, obj);
//#endif

    return NOERROR;
}

/*
 * Object.wait().  Also called for class init.
 */
ECode NativeObjectWait(
    /* [in] */ NativeObject* obj,
    /* [in] */ Int64 msec,
    /* [in] */ Int32 nsec,
    /* [in] */ Boolean interruptShouldThrow)
{
    NativeThread* self = NativeThreadSelf();
    assert(self != NULL);

    Monitor* mon;
    UInt32 thin = obj->mLock;

    /* If the lock is still thin, we need to fatten it.
     */
    if (LW_SHAPE(thin) == LW_SHAPE_THIN) {
        /* Make sure that 'self' holds the lock.
         */
        if (LW_LOCK_OWNER(thin) != self->mThreadId) {
            // dvmThrowException("Ljava/lang/IllegalMonitorStateException;",
            //     "object not locked by thread before wait()");
            return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
        }

        /* This thread holds the lock.  We need to fatten the lock
         * so 'self' can block on it.  Don't update the object lock
         * field yet, because 'self' needs to acquire the lock before
         * any other thread gets a chance.
         */
        InflateMonitor(self, obj);
        // LOG_THIN("(%d) lock %p fattened by wait() to count %d",
        //          self->threadId, &obj->lock, mon->lockCount);
    }
    mon = LW_MONITOR(obj->mLock);
    return WaitMonitor(self, mon, msec, nsec, interruptShouldThrow);
}

/*
 * Object.notify().
 */
ELAPI NativeObjectNotify(
    /* [in] */ NativeObject *obj)
{
    NativeThread* self = NativeThreadSelf();
    assert(self != NULL);

    UInt32 thin = obj->mLock;

    /* If the lock is still thin, there aren't any waiters;
     * waiting on an object forces lock fattening.
     */
    if (LW_SHAPE(thin) == LW_SHAPE_THIN) {
        /* Make sure that 'self' holds the lock.
         */
        if (LW_LOCK_OWNER(thin) != self->mThreadId) {
            // dvmThrowException("Ljava/lang/IllegalMonitorStateException;",
            //     "object not locked by thread before notify()");
            return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
        }

        /* no-op;  there are no waiters to notify.
         */
        return NOERROR;
    } else {
        /* It's a fat lock.
         */
        return NotifyMonitor(self, LW_MONITOR(thin));
    }
}

/*
 * Object.notifyAll().
 */
ELAPI NativeObjectNotifyAll(
    /* [in] */ NativeObject *obj)
{
    NativeThread* self = NativeThreadSelf();
    assert(self != NULL);

    UInt32 thin = obj->mLock;

    /* If the lock is still thin, there aren't any waiters;
     * waiting on an object forces lock fattening.
     */
    if (LW_SHAPE(thin) == LW_SHAPE_THIN) {
        /* Make sure that 'self' holds the lock.
         */
        if (LW_LOCK_OWNER(thin) != self->mThreadId) {
            // dvmThrowException("Ljava/lang/IllegalMonitorStateException;",
            //     "object not locked by thread before notifyAll()");
            return E_ILLEGAL_MONITOR_STATE_EXCEPTION;
        }

        /* no-op;  there are no waiters to notify.
         */
        return NOERROR;
    } else {
        /* It's a fat lock.
         */
        return NotifyAllMonitor(self, LW_MONITOR(thin));
    }
}

/*
 * Implement java.lang.Thread.interrupt().
 */
void NativeThreadInterrupt(
    /* [in] */ NativeThread* thread)
{
    assert(thread != NULL);

    NativeLockMutex(&thread->mWaitMutex);

    /*
     * If the interrupted flag is already set no additional action is
     * required.
     */
    if (thread->mInterrupted == TRUE) {
        NativeUnlockMutex(&thread->mWaitMutex);
        return;
    }

    /*
     * Raise the "interrupted" flag.  This will cause it to bail early out
     * of the next wait() attempt, if it's not currently waiting on
     * something.
     */
    thread->mInterrupted = TRUE;

    /*
     * Is the thread waiting?
     *
     * Note that fat vs. thin doesn't matter here;  waitMonitor
     * is only set when a thread actually waits on a monitor,
     * which implies that the monitor has already been fattened.
     */
    if (thread->mWaitMonitor != NULL) {
        pthread_cond_signal(&thread->mWaitCond);
    }

    NativeUnlockMutex(&thread->mWaitMutex);
}

/*
 * This implements java.lang.Thread.sleep(long msec, int nsec).
 *
 * The sleep is interruptible by other threads, which means we can't just
 * plop into an OS sleep call.  (We probably could if we wanted to send
 * signals around and rely on EINTR, but that's inefficient and relies
 * on native code respecting our signal mask.)
 *
 * We have to do all of this stuff for Object.wait() as well, so it's
 * easiest to just sleep on a private Monitor.
 *
 * It appears that we want sleep(0,0) to go through the motions of sleeping
 * for a very short duration, rather than just returning.
 */
ECode NativeThreadSleep(
    /* [in] */ Int64 msec,
    /* [in] */ Int32 nsec)
{
    NativeThread* self = NativeThreadSelf();
    Monitor* mon = gCore.mThreadSleepMon;

    /* sleep(0,0) wakes up immediately, wait(0,0) means wait forever; adjust */
    if (msec == 0 && nsec == 0) {
        nsec++;
    }

    ECode ec, ec1;
    LockMonitor(self, mon);
    ec = WaitMonitor(self, mon, msec, nsec, TRUE);
    ec1 = UnlockMonitor(self, mon);
    if (FAILED(ec1)) {
        ec = ec1;
    }
    return ec;
}

//misc
/*
 * Get the current time, in nanoseconds.  This is "relative" time, meaning
 * it could be wall-clock time or a monotonic counter, and is only suitable
 * for computing time deltas.
 */
UInt64 NativeGetRelativeTimeNsec()
{
#ifdef HAVE_POSIX_CLOCKS
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (u8)now.tv_sec*1000000000LL + now.tv_nsec;
#else
    struct timeval now;
    gettimeofday(&now, NULL);
    return (UInt64)now.tv_sec*1000000000LL + now.tv_usec * 1000LL;
#endif
}

/*
 * Get the current time, in microseconds.  This is "relative" time, meaning
 * it could be wall-clock time or a monotonic counter, and is only suitable
 * for computing time deltas.
 */
static UInt64 NativeGetRelativeTimeUsec()
{
    return NativeGetRelativeTimeNsec() / 1000;
}

} // namespace Core
} // namespace Elastos
