
#ifndef __ELASTOS_DROID_OS_ASYNCTASK_H__
#define __ELASTOS_DROID_OS_ASYNCTASK_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/utility/concurrent/FutureTask.h>

using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Utility::IArrayDeque;
using Elastos::Utility::Concurrent::FutureTask;
using Elastos::Utility::Concurrent::IThreadFactory;
using Elastos::Utility::Concurrent::IExecutor;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ICallable;
using Elastos::Utility::Concurrent::IBlockingQueue;
using Elastos::Utility::Concurrent::IRunnableFuture;
using Elastos::Utility::Concurrent::IRunnableFuture;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * <p>AsyncTask enables proper and easy use of the UI thread. This class allows to
 * perform background operations and publish results on the UI thread without
 * having to manipulate threads and/or handlers.</p>
 *
 * <p>AsyncTask is designed to be a helper class around {@link Thread} and {@link Handler}
 * and does not constitute a generic threading framework. AsyncTasks should ideally be
 * used for short operations (a few seconds at the most.) If you need to keep threads
 * running for long periods of time, it is highly recommended you use the various APIs
 * provided by the <code>java.util.concurrent</code> package such as {@link Executor},
 * {@link ThreadPoolExecutor} and {@link FutureTask}.</p>
 *
 * <p>An asynchronous task is defined by a computation that runs on a background thread and
 * whose result is published on the UI thread. An asynchronous task is defined by 3 generic
 * types, called <code>Params</code>, <code>Progress</code> and <code>Result</code>,
 * and 4 steps, called <code>onPreExecute</code>, <code>doInBackground</code>,
 * <code>onProgressUpdate</code> and <code>onPostExecute</code>.</p>
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about using tasks and threads, read the
 * <a href="{@docRoot}guide/topics/fundamentals/processes-and-threads.html">Processes and
 * Threads</a> developer guide.</p>
 * </div>
 *
 * <h2>Usage</h2>
 * <p>AsyncTask must be subclassed to be used. The subclass will override at least
 * one method ({@link #doInBackground}), and most often will override a
 * second one ({@link #onPostExecute}.)</p>
 *
 * <p>Here is an example of subclassing:</p>
 * <pre class="prettyprint">
 * private class DownloadFilesTask extends AsyncTask&lt;URL, Integer, Long&gt; {
 *     protected Long doInBackground(URL... urls) {
 *         int count = urls.length;
 *         long totalSize = 0;
 *         for (int i = 0; i < count; i++) {
 *             totalSize += Downloader.downloadFile(urls[i]);
 *             publishProgress((int) ((i / (float) count) * 100));
 *             // Escape early if cancel() is called
 *             if (isCancelled()) break;
 *         }
 *         return totalSize;
 *     }
 *
 *     protected void onProgressUpdate(Integer... progress) {
 *         setProgressPercent(progress[0]);
 *     }
 *
 *     protected void onPostExecute(Long result) {
 *         showDialog("Downloaded " + result + " bytes");
 *     }
 * }
 * </pre>
 *
 * <p>Once created, a task is executed very simply:</p>
 * <pre class="prettyprint">
 * new DownloadFilesTask().execute(url1, url2, url3);
 * </pre>
 *
 * <h2>AsyncTask's generic types</h2>
 * <p>The three types used by an asynchronous task are the following:</p>
 * <ol>
 *     <li><code>Params</code>, the type of the parameters sent to the task upon
 *     execution.</li>
 *     <li><code>Progress</code>, the type of the progress units published during
 *     the background computation.</li>
 *     <li><code>Result</code>, the type of the result of the background
 *     computation.</li>
 * </ol>
 * <p>Not all types are always used by an asynchronous task. To mark a type as unused,
 * simply use the type {@link Void}:</p>
 * <pre>
 * private class MyTask extends AsyncTask&lt;Void, Void, Void&gt; { ... }
 * </pre>
 *
 * <h2>The 4 steps</h2>
 * <p>When an asynchronous task is executed, the task goes through 4 steps:</p>
 * <ol>
 *     <li>{@link #onPreExecute()}, invoked on the UI thread before the task
 *     is executed. This step is normally used to setup the task, for instance by
 *     showing a progress bar in the user interface.</li>
 *     <li>{@link #doInBackground}, invoked on the background thread
 *     immediately after {@link #onPreExecute()} finishes executing. This step is used
 *     to perform background computation that can take a long time. The parameters
 *     of the asynchronous task are passed to this step. The result of the computation must
 *     be returned by this step and will be passed back to the last step. This step
 *     can also use {@link #publishProgress} to publish one or more units
 *     of progress. These values are published on the UI thread, in the
 *     {@link #onProgressUpdate} step.</li>
 *     <li>{@link #onProgressUpdate}, invoked on the UI thread after a
 *     call to {@link #publishProgress}. The timing of the execution is
 *     undefined. This method is used to display any form of progress in the user
 *     interface while the background computation is still executing. For instance,
 *     it can be used to animate a progress bar or show logs in a text field.</li>
 *     <li>{@link #onPostExecute}, invoked on the UI thread after the background
 *     computation finishes. The result of the background computation is passed to
 *     this step as a parameter.</li>
 * </ol>
 *
 * <h2>Cancelling a task</h2>
 * <p>A task can be cancelled at any time by invoking {@link #cancel(boolean)}. Invoking
 * this method will cause subsequent calls to {@link #isCancelled()} to return true.
 * After invoking this method, {@link #onCancelled(Object)}, instead of
 * {@link #onPostExecute(Object)} will be invoked after {@link #doInBackground(Object[])}
 * returns. To ensure that a task is cancelled as quickly as possible, you should always
 * check the return value of {@link #isCancelled()} periodically from
 * {@link #doInBackground(Object[])}, if possible (inside a loop for instance.)</p>
 *
 * <h2>Threading rules</h2>
 * <p>There are a few threading rules that must be followed for this class to
 * work properly:</p>
 * <ul>
 *     <li>The AsyncTask class must be loaded on the UI thread. This is done
 *     automatically as of {@link android.os.Build.VERSION_CODES#JELLY_BEAN}.</li>
 *     <li>The task instance must be created on the UI thread.</li>
 *     <li>{@link #execute} must be invoked on the UI thread.</li>
 *     <li>Do not call {@link #onPreExecute()}, {@link #onPostExecute},
 *     {@link #doInBackground}, {@link #onProgressUpdate} manually.</li>
 *     <li>The task can be executed only once (an exception will be thrown if
 *     a second execution is attempted.)</li>
 * </ul>
 *
 * <h2>Memory observability</h2>
 * <p>AsyncTask guarantees that all callback calls are synchronized in such a way that the following
 * operations are safe without explicit synchronizations.</p>
 * <ul>
 *     <li>Set member fields in the constructor or {@link #onPreExecute}, and refer to them
 *     in {@link #doInBackground}.
 *     <li>Set member fields in {@link #doInBackground}, and refer to them in
 *     {@link #onProgressUpdate} and {@link #onPostExecute}.
 * </ul>
 *
 * <h2>Order of execution</h2>
 * <p>When first introduced, AsyncTasks were executed serially on a single background
 * thread. Starting with {@link android.os.Build.VERSION_CODES#DONUT}, this was changed
 * to a pool of threads allowing multiple tasks to operate in parallel. Starting with
 * {@link android.os.Build.VERSION_CODES#HONEYCOMB}, tasks are executed on a single
 * thread to avoid common application errors caused by parallel execution.</p>
 * <p>If you truly want parallel execution, you can invoke
 * {@link #executeOnExecutor(java.util.concurrent.Executor, Object[])} with
 * {@link #THREAD_POOL_EXECUTOR}.</p>
 */
class AsyncTask
    : public Object
{
private:
    class InternalHandler
        : public Handler
    {
    public:
        InternalHandler()
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    };

public:
    /**
     * Indicates the current status of the task. Each status will be set only once
     * during the lifetime of a task.
     */
    enum Status
    {
        /**
         * Indicates that the task has not been executed yet.
         */
        PENDING,
        /**
         * Indicates that the task is running.
         */
        RUNNING,
        /**
         * Indicates that {@link AsyncTask#onPostExecute} has finished.
         */
        FINISHED,
    };

private:
    class MyThreadFactory
        : public Object
        , public IThreadFactory
    {
    public:
        MyThreadFactory();

        CAR_INTERFACE_DECL();

        CARAPI NewThread(
            /* [in] */ IRunnable* r,
            /* [out] */ IThread** t);

    private:
        AutoPtr<IAtomicInteger32> mCount;
    };

    class SerialExecutor
        : public Object
        , public IExecutor
    {
    private:
        class MyRunnable
            : public Runnable
        {
        public:
            MyRunnable(
                /* [in] */ SerialExecutor* owner,
                /* [in] */ IRunnable* r)
                : mOwner(owner)
                , mR(r)
            {}

            CARAPI Run();

        private:
            SerialExecutor* mOwner;
            AutoPtr<IRunnable> mR;
        };

    public:
        CAR_INTERFACE_DECL();

        SerialExecutor();

        CARAPI Execute(
            /* [in] */ IRunnable* r);

    protected:
        CARAPI ScheduleNext();

    public:
        AutoPtr<IArrayDeque> mTasks;
        AutoPtr<IRunnable> mActive;
    };

    class WorkerRunnable
        : public Object
        , public ICallable
    {
    public:
        WorkerRunnable(
            /* [in] */ AsyncTask* owner)
            : mOwner(owner)
        {}

        CAR_INTERFACE_DECL();

        CARAPI Call(
            /* [out] */ IInterface** result);

    public:
        AutoPtr< ArrayOf<IInterface*> > mParams;
        AsyncTask* mOwner;
    };

    class MyFutureTask
        : public FutureTask
    {
    public:
        MyFutureTask(
            /* [in] */ ICallable* callable,
            /* [in] */ AsyncTask* owner)
            : FutureTask(callable)
            , mOwner(owner)
        {}

    protected:
        virtual CARAPI_(void) Done();

    private:
        AsyncTask* mOwner;
    };

    class AsyncTaskResult
        : public Object
    {
    public:
        AsyncTaskResult(
            /* [in] */ AsyncTask* task,
            /* [in] */ ArrayOf<IInterface*>* data)
            : mTask(task)
            , mData(data)
        {}

    public:
        AutoPtr<AsyncTask> mTask;
        AutoPtr< ArrayOf<IInterface*> > mData;
    };

public:
    /**
     * Creates a new asynchronous task. This constructor must be invoked on the UI thread.
     */
    AsyncTask();

    /** @hide Used to force static handler to be created. */
    static CARAPI Init();

    /** @hide */
    static CARAPI_(void) SetDefaultExecutor(
        /* [in] */ IExecutor* exec);

    /**
     * Returns the current status of this task.
     *
     * @return The current status.
     */
    CARAPI_(Status) GetStatus();

    /**
     * Returns <tt>true</tt> if this task was cancelled before it completed
     * normally. If you are calling {@link #cancel(boolean)} on the task,
     * the value returned by this method should be checked periodically from
     * {@link #doInBackground(Object[])} to end the task as soon as possible.
     *
     * @return <tt>true</tt> if task was cancelled before it completed
     *
     * @see #cancel(boolean)
     */
    CARAPI_(Boolean) IsCancelled();

    /**
     * <p>Attempts to cancel execution of this task.  This attempt will
     * fail if the task has already completed, already been cancelled,
     * or could not be cancelled for some other reason. If successful,
     * and this task has not started when <tt>cancel</tt> is called,
     * this task should never run. If the task has already started,
     * then the <tt>mayInterruptIfRunning</tt> parameter determines
     * whether the thread executing this task should be interrupted in
     * an attempt to stop the task.</p>
     *
     * <p>Calling this method will result in {@link #onCancelled(Object)} being
     * invoked on the UI thread after {@link #doInBackground(Object[])}
     * returns. Calling this method guarantees that {@link #onPostExecute(Object)}
     * is never invoked. After invoking this method, you should check the
     * value returned by {@link #isCancelled()} periodically from
     * {@link #doInBackground(Object[])} to finish the task as early as
     * possible.</p>
     *
     * @param mayInterruptIfRunning <tt>true</tt> if the thread executing this
     *        task should be interrupted; otherwise, in-progress tasks are allowed
     *        to complete.
     *
     * @return <tt>false</tt> if the task could not be cancelled,
     *         typically because it has already completed normally;
     *         <tt>true</tt> otherwise
     *
     * @see #isCancelled()
     * @see #onCancelled(Object)
     */
    CARAPI_(Boolean) Cancel(
        /* [in] */ Boolean mayInterruptIfRunning);

    /**
     * Waits if necessary for the computation to complete, and then
     * retrieves its result.
     *
     * @return The computed result.
     *
     * @throws CancellationException If the computation was cancelled.
     * @throws ExecutionException If the computation threw an exception.
     * @throws InterruptedException If the current thread was interrupted
     *         while waiting.
     */
    CARAPI Get(
        /* [out] */ IInterface** result);

    /**
     * Waits if necessary for at most the given time for the computation
     * to complete, and then retrieves its result.
     *
     * @param timeout Time to wait before cancelling the operation.
     * @param unit The time unit for the timeout.
     *
     * @return The computed result.
     *
     * @throws CancellationException If the computation was cancelled.
     * @throws ExecutionException If the computation threw an exception.
     * @throws InterruptedException If the current thread was interrupted
     *         while waiting.
     * @throws TimeoutException If the wait timed out.
     */
    CARAPI Get(
        /* [in] */ Int64 timeout,
        /* [in] */ ITimeUnit* unit,
        /* [out] */ IInterface** result);

    /**
     * Executes the task with the specified parameters. The task returns
     * itself (this) so that the caller can keep a reference to it.
     *
     * <p>Note: this function schedules the task on a queue for a single background
     * thread or pool of threads depending on the platform version.  When first
     * introduced, AsyncTasks were executed serially on a single background thread.
     * Starting with {@link android.os.Build.VERSION_CODES#DONUT}, this was changed
     * to a pool of threads allowing multiple tasks to operate in parallel. Starting
     * {@link android.os.Build.VERSION_CODES#HONEYCOMB}, tasks are back to being
     * executed on a single thread to avoid common application errors caused
     * by parallel execution.  If you truly want parallel execution, you can use
     * the {@link #executeOnExecutor} version of this method
     * with {@link #THREAD_POOL_EXECUTOR}; however, see commentary there for warnings
     * on its use.
     *
     * <p>This method must be invoked on the UI thread.
     *
     * @param params The parameters of the task.
     *
     * @return This instance of AsyncTask.
     *
     * @throws IllegalStateException If {@link #getStatus()} returns either
     *         {@link AsyncTask.Status#RUNNING} or {@link AsyncTask.Status#FINISHED}.
     *
     * @see #executeOnExecutor(java.util.concurrent.Executor, Object[])
     * @see #execute(Runnable)
     */
    CARAPI Execute(
        /* [in] */ ArrayOf<IInterface*>* params);

    /**
     * Executes the task with the specified parameters. The task returns
     * itself (this) so that the caller can keep a reference to it.
     *
     * <p>This method is typically used with {@link #THREAD_POOL_EXECUTOR} to
     * allow multiple tasks to run in parallel on a pool of threads managed by
     * AsyncTask, however you can also use your own {@link Executor} for custom
     * behavior.
     *
     * <p><em>Warning:</em> Allowing multiple tasks to run in parallel from
     * a thread pool is generally <em>not</em> what one wants, because the order
     * of their operation is not defined.  For example, if these tasks are used
     * to modify any state in common (such as writing a file due to a button click),
     * there are no guarantees on the order of the modifications.
     * Without careful work it is possible in rare cases for the newer version
     * of the data to be over-written by an older one, leading to obscure data
     * loss and stability issues.  Such changes are best
     * executed in serial; to guarantee such work is serialized regardless of
     * platform version you can use this function with {@link #SERIAL_EXECUTOR}.
     *
     * <p>This method must be invoked on the UI thread.
     *
     * @param exec The executor to use.  {@link #THREAD_POOL_EXECUTOR} is available as a
     *              convenient process-wide thread pool for tasks that are loosely coupled.
     * @param params The parameters of the task.
     *
     * @return This instance of AsyncTask.
     *
     * @throws IllegalStateException If {@link #getStatus()} returns either
     *         {@link AsyncTask.Status#RUNNING} or {@link AsyncTask.Status#FINISHED}.
     *
     * @see #execute(Object[])
     */
    CARAPI ExecuteOnExecutor(
        /* [in] */ IExecutor* exec,
        /* [in] */ ArrayOf<IInterface*>* params);

    /**
     * Convenience version of {@link #execute(Object...)} for use with
     * a simple Runnable object. See {@link #execute(Object[])} for more
     * information on the order of execution.
     *
     * @see #execute(Object[])
     * @see #executeOnExecutor(java.util.concurrent.Executor, Object[])
     */
    static CARAPI Execute(
        /* [in] */ IRunnable* runnable);

protected:
    /**
     * Override this method to perform a computation on a background thread. The
     * specified parameters are the parameters passed to {@link #execute}
     * by the caller of this task.
     *
     * This method can call {@link #publishProgress} to publish updates
     * on the UI thread.
     *
     * @param params The parameters of the task.
     *
     * @return A result, defined by the subclass of this task.
     *
     * @see #onPreExecute()
     * @see #onPostExecute
     * @see #publishProgress
     */
    virtual CARAPI DoInBackground(
        /* [in] */ ArrayOf<IInterface*>* params,
        /* [out] */ IInterface** result) = 0;

    /**
     * Runs on the UI thread before {@link #doInBackground}.
     *
     * @see #onPostExecute
     * @see #doInBackground
     */
    virtual CARAPI OnPreExecute() { return NOERROR; }

    /**
     * <p>Runs on the UI thread after {@link #doInBackground}. The
     * specified result is the value returned by {@link #doInBackground}.</p>
     *
     * <p>This method won't be invoked if the task was cancelled.</p>
     *
     * @param result The result of the operation computed by {@link #doInBackground}.
     *
     * @see #onPreExecute
     * @see #doInBackground
     * @see #onCancelled(Object)
     */
    virtual CARAPI OnPostExecute(
        /* [in] */ IInterface* result) { return NOERROR; }

    /**
     * Runs on the UI thread after {@link #publishProgress} is invoked.
     * The specified values are the values passed to {@link #publishProgress}.
     *
     * @param values The values indicating progress.
     *
     * @see #publishProgress
     * @see #doInBackground
     */
    virtual CARAPI OnProgressUpdate(
        /* [in] */ ArrayOf<IInterface*>* values) { return NOERROR; }

    /**
     * <p>Runs on the UI thread after {@link #cancel(boolean)} is invoked and
     * {@link #doInBackground(Object[])} has finished.</p>
     *
     * <p>The default implementation simply invokes {@link #onCancelled()} and
     * ignores the result. If you write your own implementation, do not call
     * <code>super.onCancelled(result)</code>.</p>
     *
     * @param result The result, if any, computed in
     *               {@link #doInBackground(Object[])}, can be null
     *
     * @see #cancel(boolean)
     * @see #isCancelled()
     */
    virtual CARAPI OnCancelled(
        /* [in] */ IInterface* result)
    {
        return OnCancelled();
    }

    /**
     * <p>Applications should preferably override {@link #onCancelled(Object)}.
     * This method is invoked by the default implementation of
     * {@link #onCancelled(Object)}.</p>
     *
     * <p>Runs on the UI thread after {@link #cancel(boolean)} is invoked and
     * {@link #doInBackground(Object[])} has finished.</p>
     *
     * @see #onCancelled(Object)
     * @see #cancel(boolean)
     * @see #isCancelled()
     */
    virtual CARAPI OnCancelled() { return NOERROR; }

    /**
     * This method can be invoked from {@link #doInBackground} to
     * publish updates on the UI thread while the background computation is
     * still running. Each call to this method will trigger the execution of
     * {@link #onProgressUpdate} on the UI thread.
     *
     * {@link #onProgressUpdate} will note be called if the task has been
     * canceled.
     *
     * @param values The progress values to update the UI with.
     *
     * @see #onProgressUpdate
     * @see #doInBackground
     */
    CARAPI PublishProgress(
        /* [in] */ ArrayOf<IInterface*>* values);

private:
    CARAPI_(void) PostResultIfNotInvoked(
        /* [in] */ IInterface* result);

    CARAPI_(AutoPtr<IInterface>) PostResult(
        /* [in] */ IInterface* result);

    CARAPI_(void) Finish(
        /* [in] */ IInterface* result);

    static CARAPI_(AutoPtr<IExecutor>) InitThreadPoolExecutor();

    static CARAPI_(AutoPtr<IHandler>) GetHandler();
public:
    /**
     * An {@link Executor} that executes tasks one at a time in serial
     * order.  This serialization is global to a particular process.
     */
    static const AutoPtr<IExecutor> SERIAL_EXECUTOR;

    /**
     * An {@link Executor} that can be used to execute tasks in parallel.
     */
    static const AutoPtr<IExecutor> THREAD_POOL_EXECUTOR;

private:
    static const String TAG;

    static const Int32 CPU_COUNT;//Runtime.getRuntime().availableProcessors();
    static const Int32 CORE_POOL_SIZE;//CPU_COUNT + 1;
    static const Int32 MAXIMUM_POOL_SIZE;//CPU_COUNT * 2 + 1;
    static const Int32 KEEP_ALIVE = 1;

    static const AutoPtr<IThreadFactory> sThreadFactory;

    static const AutoPtr<IBlockingQueue> sPoolWorkQueue;

    static AutoPtr<IHandler> sHandler;

    static /* volatile */ AutoPtr<IExecutor> sDefaultExecutor;

    static const Int32 MESSAGE_POST_RESULT = 0x1;
    static const Int32 MESSAGE_POST_PROGRESS = 0x2;

    AutoPtr<WorkerRunnable> mWorker;
    AutoPtr<MyFutureTask> mFuture;

    volatile Status mStatus;

    AutoPtr<IAtomicBoolean> mCancelled;
    AutoPtr<IAtomicBoolean> mTaskInvoked;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_ASYNCTASK_H__
