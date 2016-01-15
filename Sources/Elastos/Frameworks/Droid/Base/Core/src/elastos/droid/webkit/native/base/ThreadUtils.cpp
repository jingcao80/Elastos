
#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/native/base/api/ThreadUtils_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

//const Object ThreadUtils::sLock;
Boolean ThreadUtils::sWillOverride = FALSE;
AutoPtr<IHandler> ThreadUtils::sUiThreadHandler;

void ThreadUtils::SetWillOverrideUiThread()
{
    assert(0);
#if 0
    synchronized(sLock) {
        sWillOverride = true;
    }
#endif
}

void ThreadUtils::SetUiThread(
    /* [in] */ ILooper* looper)
{
    assert(0);
#if 0
    synchronized(sLock) {
        if (sUiThreadHandler != null && sUiThreadHandler.getLooper() != looper) {
            throw new RuntimeException("UI thread looper is already set to " +
                    sUiThreadHandler.getLooper() + " (Main thread looper is " +
                    Looper.getMainLooper() + "), cannot set to new looper " + looper);
        } else {
            sUiThreadHandler = new Handler(looper);
        }
    }
#endif
}

AutoPtr<IHandler> ThreadUtils::GetUiThreadHandler()
{
    assert(0);
#if 0
    synchronized(sLock) {
        if (sUiThreadHandler == null) {
            if (sWillOverride) {
                throw new RuntimeException("Did not yet override the UI thread");
            }
            sUiThreadHandler = new Handler(Looper.getMainLooper());
        }
        return sUiThreadHandler;
    }
#endif
}

/**
 * Run the supplied Runnable on the main thread. The method will block until the Runnable
 * completes.
 *
 * @param r The Runnable to run.
 */
void ThreadUtils::RunOnUiThreadBlocking(
    /* [in] */ const IRunnable* r)
{
    assert(0);
#if 0
    if (runningOnUiThread()) {
        r.run();
    } else {
        FutureTask<Void> task = new FutureTask<Void>(r, null);
        postOnUiThread(task);
        try {
            task.get();
        } catch (Exception e) {
            throw new RuntimeException("Exception occured while waiting for runnable", e);
        }
    }
#endif
}

/**
 * Run the supplied Callable on the main thread, wrapping any exceptions in a RuntimeException.
 * The method will block until the Callable completes.
 *
 * @param c The Callable to run
 * @return The result of the callable
 */
AutoPtr<IInterface> ThreadUtils::RunOnUiThreadBlockingNoException(
    /* [in] */ ICallable* c)
{
    assert(0);
#if 0
    try {
        return runOnUiThreadBlocking(c);
    } catch (ExecutionException e) {
        throw new RuntimeException("Error occured waiting for callable", e);
    }
#endif
    return NULL;
}

/**
 * Run the supplied Callable on the main thread, The method will block until the Callable
 * completes.
 *
 * @param c The Callable to run
 * @return The result of the callable
 * @throws ExecutionException c's exception
 */
AutoPtr<IInterface> ThreadUtils::RunOnUiThreadBlocking(
    /* [in] */ ICallable* c)
{
    assert(0);
#if 0
    FutureTask<T> task = new FutureTask<T>(c);
    runOnUiThread(task);
    try {
        return task.get();
    } catch (InterruptedException e) {
        throw new RuntimeException("Interrupted waiting for callable", e);
    }
#endif
}

/**
 * Run the supplied FutureTask on the main thread. The method will block only if the current
 * thread is the main thread.
 *
 * @param task The FutureTask to run
 * @return The queried task (to aid inline construction)
 */
// AutoPtr<IFutureTask> ThreadUtils::RunOnUiThread(
//     /* [in] */ IFutureTask* task)
// {
//     assert(0);
// #if 0
//     if (runningOnUiThread()) {
//         task.run();
//     } else {
//         postOnUiThread(task);
//     }
//     return task;
// #endif
//     return NULL;
// }

/**
 * Run the supplied Callable on the main thread. The method will block only if the current
 * thread is the main thread.
 *
 * @param c The Callable to run
 * @return A FutureTask wrapping the callable to retrieve results
 */
// AutoPtr<IFutureTask> ThreadUtils::RunOnUiThread(
//     /* [in] */ ICallable* c)
// {
//     assert(0);
// //    return runOnUiThread(new FutureTask<T>(c));
//     return NULL;
// }

/**
 * Run the supplied Runnable on the main thread. The method will block only if the current
 * thread is the main thread.
 *
 * @param r The Runnable to run
 */
void ThreadUtils::RunOnUiThread(
    /* [in] */ IRunnable* r)
{
    assert(0);
#if 0
    if (runningOnUiThread()) {
        r.run();
    } else {
        getUiThreadHandler().post(r);
    }
#endif
}

/**
 * Post the supplied FutureTask to run on the main thread. The method will not block, even if
 * called on the UI thread.
 *
 * @param task The FutureTask to run
 * @return The queried task (to aid inline construction)
 */
// AutoPtr<IFutureTask> ThreadUtils::PostOnUiThread(
//     /* [in] */ IFutureTask* task)
// {
//     assert(0);
// #if 0
//     getUiThreadHandler().post(task);
//     return task;
// #endif
//     return NULL;
// }

/**
 * Post the supplied Runnable to run on the main thread. The method will not block, even if
 * called on the UI thread.
 *
 * @param task The Runnable to run
 */
void ThreadUtils::PostOnUiThread(
    /* [in] */ IRunnable* task)
{
    assert(0);
#if 0
    getUiThreadHandler().post(task);
#endif
}

/**
 * Post the supplied Runnable to run on the main thread after the given amount of time. The
 * method will not block, even if called on the UI thread.
 *
 * @param task The Runnable to run
 * @param delayMillis The delay in milliseconds until the Runnable will be run
 */
void ThreadUtils::PostOnUiThreadDelayed(
    /* [in] */ IRunnable* task,
    /* [in] */ Int64 delayMillis)
{
    assert(0);
#if 0
    getUiThreadHandler().postDelayed(task, delayMillis);
#endif
}

/**
 * Asserts that the current thread is running on the main thread.
 */
void ThreadUtils::AssertOnUiThread()
{
    assert(RunningOnUiThread());
}

/**
 * @return true iff the current thread is the main (UI) thread.
 */
Boolean ThreadUtils::RunningOnUiThread()
{
    assert(0);
#if 0
    return getUiThreadHandler().getLooper() == Looper.myLooper();
#endif
}

AutoPtr<ILooper> ThreadUtils::GetUiThreadLooper()
{
    assert(0);
#if 0
    return getUiThreadHandler().getLooper();
#endif
    return NULL;
}

/**
 * Set thread priority to audio.
 */
//@CalledByNative
void ThreadUtils::SetThreadPriorityAudio(
    /* [in] */ Int32 tid)
{
    assert(0);
#if 0
    Process.setThreadPriority(tid, Process.THREAD_PRIORITY_AUDIO);
#endif
}

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
