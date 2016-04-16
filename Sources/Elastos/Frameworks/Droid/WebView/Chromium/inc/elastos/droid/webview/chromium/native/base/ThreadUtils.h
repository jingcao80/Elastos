
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_THREADUTILS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_THREADUTILS_H__

#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "elastos/utility/concurrent/FutureTask.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Core::IRunnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Utility::Concurrent::FutureTask;
using Elastos::Utility::Concurrent::ICallable;

using Elastos::Utility::Concurrent::ICallable;
// import java.util.concurrent.ExecutionException;
// import java.util.concurrent.FutureTask;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * Helper methods to deal with threading related tasks.
 */
class ThreadUtils
    : public Object
{
public:
    static CARAPI_(void) SetWillOverrideUiThread();

    static CARAPI_(void) SetUiThread(
        /* [in] */ ILooper* looper);

    /**
     * Run the supplied Runnable on the main thread. The method will block until the Runnable
     * completes.
     *
     * @param r The Runnable to run.
     */
    static CARAPI_(void) RunOnUiThreadBlocking(
        /* [in] */ IRunnable* r);

    /**
     * Run the supplied Callable on the main thread, wrapping any exceptions in a RuntimeException.
     * The method will block until the Callable completes.
     *
     * @param c The Callable to run
     * @return The result of the callable
     */
    static CARAPI_(AutoPtr<IInterface>) RunOnUiThreadBlockingNoException(
        /* [in] */ ICallable* c);

    /**
     * Run the supplied Callable on the main thread, The method will block until the Callable
     * completes.
     *
     * @param c The Callable to run
     * @return The result of the callable
     * @throws ExecutionException c's exception
     */
    static CARAPI_(AutoPtr<IInterface>) RunOnUiThreadBlocking(
        /* [in] */ ICallable* c);

    /**
     * Run the supplied FutureTask on the main thread. The method will block only if the current
     * thread is the main thread.
     *
     * @param task The FutureTask to run
     * @return The queried task (to aid inline construction)
     */
    static CARAPI_(AutoPtr<FutureTask>) RunOnUiThread(
         /* [in] */ FutureTask* task);

    /**
     * Run the supplied Callable on the main thread. The method will block only if the current
     * thread is the main thread.
     *
     * @param c The Callable to run
     * @return A FutureTask wrapping the callable to retrieve results
     */
    static CARAPI_(AutoPtr<FutureTask>) RunOnUiThread(
         /* [in] */ ICallable* c);

    /**
     * Run the supplied Runnable on the main thread. The method will block only if the current
     * thread is the main thread.
     *
     * @param r The Runnable to run
     */
    static CARAPI_(void) RunOnUiThread(
        /* [in] */ IRunnable* r);

    /**
     * Post the supplied FutureTask to run on the main thread. The method will not block, even if
     * called on the UI thread.
     *
     * @param task The FutureTask to run
     * @return The queried task (to aid inline construction)
     */
    static CARAPI_(AutoPtr<FutureTask>) PostOnUiThread(
         /* [in] */ FutureTask* task);

    /**
     * Post the supplied Runnable to run on the main thread. The method will not block, even if
     * called on the UI thread.
     *
     * @param task The Runnable to run
     */
    static CARAPI_(void) PostOnUiThread(
        /* [in] */ IRunnable* task);

    /**
     * Post the supplied Runnable to run on the main thread after the given amount of time. The
     * method will not block, even if called on the UI thread.
     *
     * @param task The Runnable to run
     * @param delayMillis The delay in milliseconds until the Runnable will be run
     */
    static CARAPI_(void) PostOnUiThreadDelayed(
        /* [in] */ IRunnable* task,
        /* [in] */ Int64 delayMillis);

    /**
     * Asserts that the current thread is running on the main thread.
     */
    static CARAPI_(void) AssertOnUiThread();

    /**
     * @return true iff the current thread is the main (UI) thread.
     */
    static CARAPI_(Boolean) RunningOnUiThread();

    static CARAPI_(AutoPtr<ILooper>) GetUiThreadLooper();

    /**
     * Set thread priority to audio.
     */
    //@CalledByNative
    static CARAPI_(void) SetThreadPriorityAudio(
        /* [in] */ Int32 tid);

    static CARAPI_(void*) ElaThreadUtilsCallback_Init();

private:
    static CARAPI_(AutoPtr<IHandler>) GetUiThreadHandler();

    static Object sLock;

    static Boolean sWillOverride;

    static AutoPtr<IHandler> sUiThreadHandler;
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_THREADUTILS_H__
