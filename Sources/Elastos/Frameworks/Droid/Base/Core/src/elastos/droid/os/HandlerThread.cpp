
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/HandlerThread.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/Looper.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(HandlerThread, Thread, IHandlerThread)

HandlerThread::HandlerThread()
    : mPriority(0)
    , mTid(0)
{}

HandlerThread::HandlerThread(
    /* [in] */ const String& name)
{
    constructor(name);
}

HandlerThread::HandlerThread(
    /* [in] */ const String& name,
    /* [in] */ Int32 priority)
{
    constructor(name, priority);
}

ECode HandlerThread::constructor(
    /* [in] */ const String& name)
{
    Thread::constructor(name);
    mPriority = IProcess::THREAD_PRIORITY_DEFAULT;
    return NOERROR;
}
ECode HandlerThread::constructor(
    /* [in] */ const String& name,
    /* [in] */ Int32 priority)
{
    Thread::constructor(name);
    mPriority = priority;
    return NOERROR;
}

void HandlerThread::OnLooperPrepared()
{}

ECode HandlerThread::Run()
{
    mTid = Process::MyTid();
    Looper::Prepare();
    {
        Lock();

        mLooper = Looper::GetMyLooper();
        NotifyAll();

        Unlock();
    }
    Process::SetThreadPriority(mPriority);
    OnLooperPrepared();
    Looper::Loop();
    mTid = -1;
    return NOERROR;
}

ECode HandlerThread::GetLooper(
    /* [out] */ ILooper** looper)
{
    VALIDATE_NOT_NULL(looper);

    Boolean isAlive;
    if (IsAlive(&isAlive), !isAlive) {
        *looper = NULL;
        return NOERROR;
    }

    // If the thread has been started, wait until the looper has been created.
    {
        Lock();

        while ((IsAlive(&isAlive), isAlive) && mLooper == NULL) {
            // try {
            Wait();
            // } catch (InterruptedException e) {
            // }
        }

        Unlock();
    }
    *looper = mLooper;
    REFCOUNT_ADD(*looper);
    return NOERROR;
}

ECode HandlerThread::Quit(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ILooper> looper;
    FAIL_RETURN(GetLooper((ILooper**)&looper));
    if (looper != NULL) {
        looper->Quit();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode HandlerThread::QuitSafely(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ILooper> looper;
    FAIL_RETURN(GetLooper((ILooper**)&looper));
    if (looper != NULL) {
        looper->QuitSafely();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}
ECode HandlerThread::GetThreadId(
    /* [out] */ Int32* tid)
{
    VALIDATE_NOT_NULL(tid);

    *tid = mTid;
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
