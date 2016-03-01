
#include "Thread.h"
#include "ThreadGroup.h"
#include "Math.h"
#include "AutoLock.h"
#include "CSystem.h"

namespace Elastos {
namespace Core {

const Int32 Thread::ParkState::UNPARKED;
const Int32 Thread::ParkState::PREEMPTIVELY_UNPARKED;
const Int32 Thread::ParkState::PARKED;
AutoPtr<IThreadUncaughtExceptionHandler> Thread::sDefaultUncaughtHandler;

const Int32 Thread::NANOS_PER_MILLI = 1000000;

const ThreadState Thread::STATE_MAP[] = {
    ThreadState_TERMINATED,     // ZOMBIE
    ThreadState_RUNNABLE,       // RUNNING
    ThreadState_TIMED_WAITING,  // TIMED_WAIT
    ThreadState_BLOCKED,        // MONITOR
    ThreadState_WAITING,        // WAIT
    ThreadState_NEW,            // INITIALIZING
    ThreadState_NEW,            // STARTING
    ThreadState_RUNNABLE,       // NATIVE
    ThreadState_WAITING,        // VMWAIT
    ThreadState_RUNNABLE        // SUSPENDED
};

CAR_INTERFACE_IMPL_2(Thread, Object, IThread, IRunnable)

Thread::Thread()
    : mNativeThread(NULL)
    , mDaemon(FALSE)
    , mStackSize(0)
    , mHasBeenStarted(FALSE)
    , mId(-1)
    , mParkState(ParkState::UNPARKED)
{}

Thread::~Thread()
{}

ECode Thread::constructor()
{
    return Create(NULL, NULL, String(NULL), 0);
}

ECode Thread::constructor(
    /* [in] */ IRunnable* runnable)
{
    return Create(NULL, runnable, String(NULL), 0);
}

ECode Thread::constructor(
    /* [in] */ IRunnable* runnable,
    /* [in] */ const String& threadName)
{
    if (threadName.IsNull()) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    return Create(NULL, runnable, threadName, 0);
}

ECode Thread::constructor(
    /* [in] */ const String& threadName)
{
    if (threadName.IsNull()) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    return Create(NULL, NULL, threadName, 0);
}

ECode Thread::constructor(
    /* [in] */ IThreadGroup* group,
    /* [in] */ IRunnable* runnable)
{
    return Create(group, runnable, String(NULL), 0);
}

ECode Thread::constructor(
    /* [in] */ IThreadGroup* group,
    /* [in] */ IRunnable* runnable,
    /* [in] */ const String& threadName)
{
    if (threadName.IsNull()) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    return Create(group, runnable, threadName, 0);
}

ECode Thread::constructor(
    /* [in] */ IThreadGroup* group,
    /* [in] */ const String& threadName)
{
    if (threadName.IsNull()) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }

    return Create(group, NULL, threadName, 0);
}

ECode Thread::constructor(
    /* [in] */ IThreadGroup* group,
    /* [in] */ IRunnable* runnable,
    /* [in] */ const String& threadName,
    /* [in] */ Int64 stackSize)
{
    if (threadName.IsNull()) {
        // throw new NullPointerException();
        return E_NULL_POINTER_EXCEPTION;
    }
    return Create(group, runnable, threadName, stackSize);
}

ECode Thread::constructor(
    /* [in] */ IThreadGroup* group,
    /* [in] */ const String& name,
    /* [in] */ Int32 priority,
    /* [in] */ Boolean daemon)
{
    mId = NativeGetCount();

    if (name.IsNull()) {
        mName = "Thread-";
        mName.AppendFormat("%d", mId);
    }
    else {
        mName = name;
    }

    if (group == NULL) {
        // throw new InternalError("group not specified");
        return E_INTERNAL_ERROR;
    }

    mGroup = group;

    mTarget = NULL;
    mStackSize = 0;
    mPriority = priority;
    mDaemon = daemon;

    /* add ourselves to our ThreadGroup of choice */
    ThreadGroup* tg = (ThreadGroup*)group;
    tg->AddThread(THIS_PROBE(IThread));
    return NOERROR;
}

ECode Thread::Create(
    /* [in] */ IThreadGroup* _group,
    /* [in] */ IRunnable* runnable,
    /* [in] */ const String& threadName,
    /* [in] */ Int64 stackSize)
{
    Object obj;
    synchronized(obj) {

    }

    AutoPtr<IThreadGroup> group = _group;
    AutoPtr<IThread> currentThread = Thread::GetCurrentThread();
    if (group == NULL) {
        currentThread->GetThreadGroup((IThreadGroup**)&group);
    }

    Boolean isDestroyed;
    if (group->IsDestroyed(&isDestroyed), isDestroyed) {
        // throw new IllegalThreadStateException("Group already destroyed");
        return E_ILLEGAL_THREAD_STATE_EXCEPTION;
    }

    mGroup = group;

    mId = NativeGetCount();

    if (threadName.IsNull()) {
        mName = "Thread-";
        mName.AppendFormat("%d", mId);
    }
    else {
        mName = threadName;
    }

    mTarget = runnable;
    mStackSize = stackSize;

    currentThread->GetPriority(&mPriority);

    // this.contextClassLoader = currentThread.contextClassLoader;

    // Transfer over InheritableThreadLocals.
    // if (currentThread.inheritableValues != null) {
    //     inheritableValues
    //             = new ThreadLocal.Values(currentThread.inheritableValues);
    // }

    // add ourselves to our ThreadGroup of choice
    ThreadGroup* tg = (ThreadGroup*)group.Get();
    tg->AddThread(THIS_PROBE(IThread));
    return NOERROR;
}

Int32 Thread::GetActiveCount()
{
    AutoPtr<IThreadGroup> group;
    GetCurrentThread()->GetThreadGroup((IThreadGroup**)&group);
    Int32 num;
    group->ActiveCount(&num);
    return num;
}

ECode Thread::CheckAccess()
{
    // Forwards the message to the SecurityManager (if there's one) passing
    // the receiver as parameter

    AutoPtr<CSystem> cs;
    CSystem::AcquireSingletonByFriend((CSystem**)&cs);
    AutoPtr<ISystem> system = (ISystem*)cs.Get();

    // SecurityManager currentManager = System.getSecurityManager();
    // if (currentManager != null) {
    //     currentManager.checkAccess(this);
    // }
    return NOERROR;
}

ECode Thread::CountStackFrames(
    /* [out] */ Int32* number)
{
//    VALIDATE_NOT_NULL(number);
//    return getStackTrace().length;

    return E_NOT_IMPLEMENTED;
}

AutoPtr<IThread> Thread::GetCurrentThread()
{
    return NativeGetCurrentThread();
}

AutoPtr<IThread> Thread::NativeGetCurrentThread()
{
    // the following codes are different from Java.lang.Thread.
    // Java can translate native thread to java thread in jni codes,
    // So when Java calls the method, the current thread is java thread.
    // But Elastos maybe doesn't have the better time to translate native
    // thread to Elastos thread.
    // So we call Attach here to translate native thread;
    AutoPtr<IThread> t;
    Attach((IThread**)&t);
    return t;
    // NativeThread* self = NativeThreadSelf();
    // assert(self != NULL);
    // return (IThread*)self->mThreadObj->Probe(EIID_IThread);
}

ECode Thread::Destroy()
{
    return E_NO_SUCH_METHOD_EXCEPTION;
//    throw new NoSuchMethodError("Thread.destroy()"); // TODO Externalize???
}

void Thread::DumpStack()
{}

ECode Thread::Enumerate(
    /* [out] */ ArrayOf<IThread*>* threads,
    /* [out] */ Int32* number)
{
    AutoPtr<IThread> currentThread = Thread::GetCurrentThread();
    FAIL_RETURN(currentThread->CheckAccess());
    AutoPtr<IThreadGroup> group;
    currentThread->GetThreadGroup((IThreadGroup**)&group);

    return group->Enumerate(threads, number);
}

//    static CARAPI_(HashMap<AutoPtr<IThread>, StackTraceElement[]>) GetAllStackTraces();

ECode Thread::GetContextClassLoader(
    /* [out] */ IClassLoader** outload)
{
    VALIDATE_NOT_NULL(outload)

    *outload = mContextClassLoader;
    REFCOUNT_ADD(*outload)
    return NOERROR;
}

AutoPtr<IThreadUncaughtExceptionHandler> Thread::GetDefaultUncaughtExceptionHandler()
{
    return sDefaultUncaughtHandler;
}

ECode Thread::GetId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode Thread::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode Thread::GetPriority(
    /* [out] */ Int32* priority)
{
    VALIDATE_NOT_NULL(priority);
    *priority = mPriority;
    return NOERROR;
}

//        public StackTraceElement[] getStackTrace()

ECode Thread::GetState(
    /* [out] */ ThreadState* state)
{
    VALIDATE_NOT_NULL(state);
    // // TODO This is ugly and should be implemented better.
    // VMThread vmt = this.vmThread;

    // // Make sure we have a valid reference to an object. If native code
    // // deletes the reference we won't run into a null reference later.
    // VMThread thread = vmThread;
    // if (thread != null) {
    //     // If the Thread Object became invalid or was not yet started,
    //     // getStatus() will return -1.
    //     int state = thread.getStatus();
    //     if(state != -1) {
    //         return VMThread.STATE_MAP[state];
    //     }
    // }
    if (mNativeThread) {
        Int32 ts = NativeGetState();
        if (ts != -1) {
            *state = STATE_MAP[ts];
            return NOERROR;
        }
    }

    *state = mHasBeenStarted ? ThreadState_TERMINATED : ThreadState_NEW;
    return NOERROR;
}

Int32 Thread::NativeGetState()
{
    NativeThread* thread;
    Int32 result;

    NativeLockThreadList(NULL);
    thread = NativeGetThreadFromThreadObject(reinterpret_cast<Int32>(this));
    if (thread != NULL) {
        result = thread->mStatus;
    }
    else {
        result = NTHREAD_ZOMBIE;// assume it used to exist and is now gone
    }
    NativeUnlockThreadList();

    return result;
}

ECode Thread::GetThreadGroup(
    /* [out] */ IThreadGroup** group)
{
    VALIDATE_NOT_NULL(group);
    // TODO This should actually be done at native termination.
    ThreadState state;
    GetState(&state);
    if (state == ThreadState_TERMINATED) {
        *group = NULL;
        return NOERROR;
    }
    else {
        *group = (IThreadGroup*)mGroup->Probe(EIID_IThreadGroup);
        REFCOUNT_ADD(*group);
        return NOERROR;
    }
}

ECode Thread::GetUncaughtExceptionHandler(
    /* [out] */ IThreadUncaughtExceptionHandler** handler)
{
    VALIDATE_NOT_NULL(handler)

    if (mUncaughtHandler != NULL)
        *handler = mUncaughtHandler;
    else
        assert(0 && "TODO");
        // *handler = mGroup;           // ThreadGroup is instance of UEH

    REFCOUNT_ADD(*handler)
    return NOERROR;
}

ECode Thread::Interrupt()
{
    FAIL_RETURN(CheckAccess());

    if (mInterruptAction != NULL) {
        mInterruptAction->Run();
    }

    if (mNativeThread != NULL) {
        NativeInterrupt();
    }
    return NOERROR;
}

void Thread::NativeInterrupt()
{
    NativeThread* thread;

    NativeLockThreadList(NULL);
    thread = NativeGetThreadFromThreadObject(reinterpret_cast<Int32>(this));
    if (thread != NULL) {
        NativeThreadInterrupt(thread);
    }
    NativeUnlockThreadList();
}

Boolean Thread::Interrupted()
{
    NativeThread* self = NativeThreadSelf();
    Boolean interrupted = self->mInterrupted;
    self->mInterrupted = FALSE;
    return interrupted;
}

ECode Thread::IsAlive(
    /* [out] */ Boolean* isAlive)
{
    VALIDATE_NOT_NULL(isAlive);
    *isAlive = (mNativeThread != NULL);
    return NOERROR;
}

ECode Thread::IsDaemon(
    /* [out] */ Boolean* isDaemon)
{
    VALIDATE_NOT_NULL(isDaemon);
    *isDaemon = mDaemon;
    return NOERROR;
}

ECode Thread::IsInterrupted(
    /* [out] */ Boolean* isInterrupted)
{
    VALIDATE_NOT_NULL(isInterrupted);
    *isInterrupted = FALSE;
    if (mNativeThread != NULL) {
        *isInterrupted = NativeIsInterrupted();
    }
    return NOERROR;
}

Boolean Thread::NativeIsInterrupted()
{
    Boolean interrupted;

    NativeLockThreadList(NULL);
    NativeThread* thread = NativeGetThreadFromThreadObject(reinterpret_cast<Int32>(this));
    if (thread != NULL) {
        interrupted = thread->mInterrupted;
    }
    else {
        interrupted = FALSE;
    }
    NativeUnlockThreadList();

    return interrupted;
}

ECode Thread::Join()
{
    if (mNativeThread == NULL) {
        return NOERROR;
    }

    AutoLock lock(this);
    Boolean isAlive;
    while (IsAlive(&isAlive), isAlive) {
        Wait(0, 0);
    }
    return NOERROR;
}

ECode Thread::Join(
    /* [in] */ Int64 millis)
{
    return Join(millis, 0);
}

ECode Thread::Join(
    /* [in] */ Int64 millis,
    /* [in] */ Int32 nanos)
{
    if (millis < 0 || nanos < 0 || nanos >= NANOS_PER_MILLI) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException();
    }

    // avoid overflow: if total > 292,277 years, just wait forever
    Boolean overflow = millis >= (Math::INT64_MAX_VALUE - nanos) / NANOS_PER_MILLI;
    Boolean forever = (millis | nanos) == 0;
    if (forever | overflow) {
        return Join();
    }

    if (mNativeThread == NULL) {
        return NOERROR;
    }

    AutoLock lock(this);
    Boolean isAlive;
    IsAlive(&isAlive);
    if (!isAlive) {
        return NOERROR;
    }

    AutoPtr<CSystem> cs;
    CSystem::AcquireSingletonByFriend((CSystem**)&cs);
    AutoPtr<ISystem> system = (ISystem*)cs.Get();

//     // guaranteed not to overflow
    Int64 nanosToWait = millis * NANOS_PER_MILLI + nanos;

    // wait until this thread completes or the timeout has elapsed
    Int64 start, nanosElapsed, nanosRemaining;
    system->GetNanoTime(&start);
    while (TRUE) {
        Wait(millis, nanos);
        IsAlive(&isAlive);
        if (!isAlive) {
            break;
        }
        system->GetNanoTime(&nanosElapsed);
        nanosElapsed -= start;
        nanosRemaining = nanosToWait - nanosElapsed;
        if (nanosRemaining <= 0) {
            break;
        }
        millis = nanosRemaining / NANOS_PER_MILLI;
        nanos = (Int32) (nanosRemaining - millis * NANOS_PER_MILLI);
    }

    return NOERROR;
}

ECode Thread::Resume()
{
    FAIL_RETURN(CheckAccess());

    // VMThread vmt = this.vmThread;
    // if (vmt != null) {
    //     vmt.resume();
    // }
    // Logger.global.log(Level.SEVERE, UNSUPPORTED_THREAD_METHOD,
    //         new UnsupportedOperationException());

    return NOERROR;
}

ECode Thread::Run()
{
    ECode ec = NOERROR;
    if (mTarget != NULL) {
        ec = mTarget->Run();
    }
    return ec;
}

//        public void setContextClassLoader(ClassLoader cl)

ECode Thread::SetDaemon(
    /* [in] */ Boolean isDaemon)
{
    FAIL_RETURN(CheckAccess());

    if (mHasBeenStarted) {
        return E_ILLEGAL_THREAD_STATE_EXCEPTION;
//        throw new IllegalThreadStateException("Thread already started."); // TODO Externalize?
    }

    // if (vmThread == null) {
    //     daemon = isDaemon;
    // }
    mDaemon = isDaemon;
    return NOERROR;
}

void Thread::SetDefaultUncaughtExceptionHandler(
    /* [in] */ IThreadUncaughtExceptionHandler* handler)
{
    sDefaultUncaughtHandler = handler;
}

void Thread::SetInterruptAction(
    /* [in] */ IRunnable* action)
{
    mInterruptAction = action;
}

ECode Thread::SetName(
    /* [in] */ const String& threadName)
{
    if (threadName.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    FAIL_RETURN(CheckAccess());

    mName = threadName;
    if (mNativeThread != NULL) {
        /* notify the VM that the thread name has changed */
        NativeNameChanged(threadName);
    }
    return NOERROR;
}

void Thread::NativeNameChanged(
    /* [in] */ const String& threadName)
{
    // Int32 threadId = -1;

    /* get the thread's ID */
    NativeLockThreadList(NULL);
    NativeThread* thread = NativeGetThreadFromThreadObject(reinterpret_cast<Int32>(this));
    if (thread != NULL) {
        // threadId = thread->mThreadId;
    }
    NativeUnlockThreadList();

    //vmDdmSendThreadNameChange(threadId, nameStr);
    //char* str = dvmCreateCstrFromString(nameStr);
    //LOGI("UPDATE: threadid=%d now '%s'\n", threadId, str);
    //free(str);
}

ECode Thread::SetPriority(
    /* [in] */ Int32 priority)
{
    FAIL_RETURN(CheckAccess());

    if (priority < IThread::MIN_PRIORITY || priority > IThread::MAX_PRIORITY) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
//        throw new IllegalArgumentException("Priority out of range"); // TODO Externalize?
    }

    Int32 groupPriority;
    AutoPtr<IThreadGroup> group = (IThreadGroup*)mGroup->Probe(EIID_IThreadGroup);
    group->GetMaxPriority(&groupPriority);
    if (priority > groupPriority) {
        priority = groupPriority;
    }

    mPriority = priority;

    if (mNativeThread != NULL) {
        NativeSetPriority(priority);
    }
    return NOERROR;
}

void Thread::NativeSetPriority(
    /* [in] */ Int32 priority)
{
    NativeLockThreadList(NULL);
    NativeThread* thread = NativeGetThreadFromThreadObject(reinterpret_cast<Int32>(this));
    if (thread != NULL)
        NativeChangeThreadPriority(thread, priority);
    //dvmDumpAllThreads(false);
    NativeUnlockThreadList();
}

ECode Thread::Sleep(
    /* [in] */ Int64 time)
{
    return Thread::Sleep(time, 0);
}

ECode Thread::Sleep(
    /* [in] */ Int64 millis,
    /* [in] */ Int32 nanos)
{
    return NativeThreadSleep(millis, nanos);
}

ECode Thread::SetUncaughtExceptionHandler(
    /* [in] */ IThreadUncaughtExceptionHandler* handler)
{
    mUncaughtHandler = handler;
    return NOERROR;
}

ECode Thread::Start()
{
    AutoLock lock(this);

    if (mHasBeenStarted) {
        // throw new IllegalThreadStateException("Thread already started."); // TODO Externalize?
        return E_ILLEGAL_THREAD_STATE_EXCEPTION;
    }

    mHasBeenStarted = TRUE;

    /* copying collector will pin threadObj for us since it was an argument */
    return NativeCreateThread(reinterpret_cast<Int32>(this), (Int32)mStackSize) ? NOERROR : E_RUNTIME_EXCEPTION;
}

ECode Thread::Stop()
{
    AutoPtr<CSystem> cs;
    CSystem::AcquireSingletonByFriend((CSystem**)&cs);
    AutoPtr<ISystem> system = (ISystem*)cs.Get();

    // SecurityManager securityManager = System.getSecurityManager();
    // if (securityManager != null) {
    //     securityManager.checkAccess(this);
    //     if (Thread.currentThread() != this) {
    //         securityManager.checkPermission(new RuntimePermission("stopThread"));
    //     }
    // }

    // if (throwable == null) {
    //     throw new NullPointerException();
    // }

    if (mNativeThread != NULL) {
        NativeStop();
    }
    return NOERROR;
}

void Thread::NativeStop()
{
    // Logger.global.log(Level.SEVERE, UNSUPPORTED_THREAD_METHOD,
    //         new UnsupportedOperationException());
}

//@Deprecated
//        public final synchronized void stop(Throwable throwable)

ECode Thread::Suspend()
{
    FAIL_RETURN(CheckAccess());

    if (mNativeThread != NULL) {
        NativeSuspend();
    }
    return NOERROR;
}

void Thread::NativeSuspend()
{
    // Logger.global.log(Level.SEVERE, UNSUPPORTED_THREAD_METHOD,
    //         new UnsupportedOperationException());
}

void Thread::Yield()
{
}

ECode Thread::Unpark()
{
    NativeThread* nt = mNativeThread;
    if (nt == NULL) {
        /*
         * vmThread is null before the thread is start()ed. In
         * this case, we just go ahead and set the state to
         * PREEMPTIVELY_UNPARKED. Since this happens before the
         * thread is started, we don't have to worry about
         * synchronizing with it.
         */
        mParkState = ParkState::PREEMPTIVELY_UNPARKED;
        return NOERROR;
    }

    {
        AutoLock lock(this);

        switch (mParkState) {
            case ParkState::PREEMPTIVELY_UNPARKED: {
                /*
                 * Nothing to do in this case: By definition, a
                 * preemptively unparked thread is to remain in
                 * the preemptively unparked state if it is told
                 * to unpark.
                 */
                break;
            }
            case ParkState::UNPARKED: {
                mParkState = ParkState::PREEMPTIVELY_UNPARKED;
                break;
            }
            default /*parked*/: {
                mParkState = ParkState::UNPARKED;
                NotifyAll();
                break;
            }
        }
    }
    return NOERROR;
}

ECode Thread::ParkFor(
    /* [in] */ Int64 nanos)
{
    NativeThread* nt = mNativeThread;
    if (nt == NULL) {
        // Running threads should always have an associated vmThread.
        // throw new AssertionError();
        return E_ASSERTION_ERROR;
    }

    {
        AutoLock lock(this);

        switch (mParkState) {
            case ParkState::PREEMPTIVELY_UNPARKED: {
                mParkState = ParkState::UNPARKED;
                break;
            }
            case ParkState::UNPARKED: {
                Int64 millis = nanos / NANOS_PER_MILLI;
                nanos %= NANOS_PER_MILLI;

                mParkState = ParkState::PARKED;
                // try {
                ECode ec = Wait(millis, (Int32)nanos);
                if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
                    Interrupt();
                }
                /*
                 * Note: If parkState manages to become
                 * PREEMPTIVELY_UNPARKED before hitting this
                 * code, it should left in that state.
                 */
                if (mParkState == ParkState::PARKED) {
                    mParkState = ParkState::UNPARKED;
                }
                // } catch (InterruptedException ex) {
                //     interrupt();
                // } finally {
                //     /*
                //      * Note: If parkState manages to become
                //      * PREEMPTIVELY_UNPARKED before hitting this
                //      * code, it should left in that state.
                //      */
                //     if (mParkState == ParkState.PARKED) {
                //         mParkState = ParkState.UNPARKED;
                //     }
                // }
                break;
            }
            default /*parked*/: {
                // throw new AssertionError(
                //         "shouldn't happen: attempt to repark");
                return E_ASSERTION_ERROR;
            }
        }
    }
    return NOERROR;
}

ECode Thread::ParkUntil(
    /* [in] */ Int64 time)
{
    NativeThread* nt = mNativeThread;
    if (nt == NULL) {
        // Running threads should always have an associated vmThread.
        // throw new AssertionError();
        return E_ASSERTION_ERROR;
    }

    {
        AutoLock lock(this);

        /*
         * Note: This conflates the two time bases of "wall clock"
         * time and "monotonic uptime" time. However, given that
         * the underlying system can only wait on monotonic time,
         * it is unclear if there is any way to avoid the
         * conflation. The downside here is that if, having
         * calculated the delay, the wall clock gets moved ahead,
         * this method may not return until well after the wall
         * clock has reached the originally designated time. The
         * reverse problem (the wall clock being turned back)
         * isn't a big deal, since this method is allowed to
         * spuriously return for any reason, and this situation
         * can safely be construed as just such a spurious return.
         */

        AutoPtr<CSystem> cs;
        CSystem::AcquireSingletonByFriend((CSystem**)&cs);
        AutoPtr<ISystem> system = (ISystem*)cs.Get();

        Int64 delayMillis;
        system->GetCurrentTimeMillis(&delayMillis);
        delayMillis = time - delayMillis;

        if (delayMillis <= 0) {
            mParkState = ParkState::UNPARKED;
            return NOERROR;
        }
        else {
            return ParkFor(delayMillis * NANOS_PER_MILLI);
        }
    }

    return NOERROR;
}

Boolean Thread::NativeHoldsLock(
    /* [in] */ Object* object)
{
    if (object == NULL) {
        return FALSE;
    }

    assert(0 && "TODO");
    // NativeThread* thread = NativeGetThreadFromThreadObject(reinterpret_cast<Int32>(this));
    // return object->GetLockOwnerThreadId() == GetThreadId();
    return TRUE;
}

Boolean Thread::HoldsLock(
    /* [in] */ Object* object)
{
    AutoPtr<IThread> thread = NativeGetCurrentThread();
    Thread* t = (Thread*)thread.Get();
    return t->NativeHoldsLock(object);
}

ECode Thread::Attach(
    /* [out] */ IThread** thread)
{
    return Thread::Attach(String(NULL), thread);
}

ECode Thread::Attach(
    /* [in] */ const String& name,
    /* [out] */ IThread** thread)
{
    VALIDATE_NOT_NULL(thread);

    // JavaVMAttachArgs* args = (JavaVMAttachArgs*) thr_args;
    NativeThread* self;
    // Boolean result = FALSE;

    /*
     * Return immediately if we're already one with the VM.
     */
    self = NativeThreadSelf();
    if (self != NULL) {
        // *p_env = self->jniEnv;
        // return JNI_OK;
        *thread = (IThread*)reinterpret_cast<Thread*>(self->mThreadObj)->Probe(EIID_IThread);
        REFCOUNT_ADD(*thread);
        return NOERROR;
    }

    /*
     * No threads allowed in zygote mode.
     */
    // if (gDvm.zygote) {
    //     return JNI_ERR;
    // }

    /* increment the count to keep the VM from bailing while we run */
    // dvmLockThreadList(NULL);
    // if (gDvm.nonDaemonThreadCount == 0) {
    //     // dead or dying
    //     LOGV("Refusing to attach thread '%s' -- VM is shutting down\n",
    //         (thr_args == NULL) ? "(unknown)" : args->name);
    //     dvmUnlockThreadList();
    //     return JNI_ERR;
    // }
    // gDvm.nonDaemonThreadCount++;
    // dvmUnlockThreadList();

    /* tweak the JavaVMAttachArgs as needed */
    NativeAttachArgs argsCopy;
    // if (args == NULL) {
    //     /* allow the v1.1 calling convention */
    //     argsCopy.version = JNI_VERSION_1_2;
    //     argsCopy.name = NULL;
    //     argsCopy.group = dvmGetMainThreadGroup();
    // } else {
    //     assert(args->version >= JNI_VERSION_1_2);

    //     argsCopy.version = args->version;
    //     argsCopy.name = args->name;
    //     if (args->group != NULL)
    //         argsCopy.group = args->group;
    //     else
    //         argsCopy.group = dvmGetMainThreadGroup();
    // }
    AutoPtr<IThreadGroup> threadGroup = NativeGetMainThreadGroup();
    argsCopy.mName = name;
    argsCopy.mGroup = reinterpret_cast<Int32>((ThreadGroup *)threadGroup.Get());
    ECode ec = NativeAttachCurrentThread(&argsCopy, FALSE, thread);

    /* restore the count */
    // dvmLockThreadList(NULL);
    // gDvm.nonDaemonThreadCount--;
    // dvmUnlockThreadList();

    /*
     * Change the status to indicate that we're out in native code.  This
     * call is not guarded with state-change macros, so we have to do it
     * by hand.
     */
    // if (result) {
    //     self = dvmThreadSelf();
    //     assert(self != NULL);
    //     dvmChangeStatus(self, THREAD_NATIVE);
    //     *p_env = self->jniEnv;
    //     return JNI_OK;
    // } else {
    //     return JNI_ERR;
    // }
    return ec;
}

ECode Thread::Detach()
{
    NativeThread* self = NativeThreadSelf();

    if (self == NULL || mNativeThread != self) {              /* not attached, can't do anything */
        return E_ILLEGAL_THREAD_STATE_EXCEPTION;
    }

    /* switch to "running" to check for suspension */
    // NativeChangeStatus(self, NTHREAD_RUNNING);
    NativeChangeStatus(self, NTHREAD_NATIVE);

    /* detach the thread */
    NativeDetachCurrentThread();

    /* (no need to change status back -- we have no status) */
    return NOERROR;
}

ECode Thread::SetContextClassLoader(
    /* [in] */ IClassLoader* cl)
{
    mContextClassLoader = cl;
    return NOERROR;
}

ECode Thread::PushInterruptAction(
    /* [in] */ IRunnable* interruptAction)
{
    assert(0 && "TODO");
    // synchronized (mLock) {
    //     interruptActions.add(interruptAction);
    // }

    Boolean isflag = FALSE;
    if (interruptAction != NULL && (IsInterrupted(&isflag), isflag)) {
        interruptAction->Run();
    }
    return NOERROR;
}

ECode Thread::PopInterruptAction(
    /* [in] */ IRunnable* interruptAction)
{
    assert(0 && "TODO");
    // synchronized (interruptActions) {
    //     Runnable removed = interruptActions.remove(interruptActions.size() - 1);
    //     if (interruptAction != removed) {
    //         throw new IllegalArgumentException(
    //                 "Expected " + interruptAction + " but was " + removed);
    //     }
    // }
    return NOERROR;
}

} // namespace Core
} // namespace Elastos
