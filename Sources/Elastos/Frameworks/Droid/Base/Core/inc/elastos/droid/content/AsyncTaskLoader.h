
#ifndef __ELASTOS_DROID_CONTENT_ASYNCTASKLOADER_H__
#define __ELASTOS_DROID_CONTENT_ASYNCTASKLOADER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/content/Loader.h"

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Utility::Concurrent::ICountDownLatch;
using Elastos::Utility::Concurrent::IExecutor;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Content {

class ECO_PUBLIC AsyncTaskLoader
    : public Loader
    , public IAsyncTaskLoader
{
private:
    class ECO_LOCAL LoadTask
        : public AsyncTask
        , public IRunnable
    {
    friend class AsyncTaskLoader;

    public:
        CAR_INTERFACE_DECL()

        LoadTask(
            /* [in] */ IWeakReference* loader);

        virtual ~LoadTask();

        /* Runs on the UI thread, when the waiting task is posted to a handler.
         * This method is only executed when task execution was deferred (waiting was true). */
        CARAPI Run();

        /* Used for testing purposes to wait for the task to complete. */
        CARAPI WaitForLoader();

    protected:
        /* Runs on a worker thread */
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        /* Runs on the UI thread */
        CARAPI OnPostExecute(
            /* [in] */ IInterface* data);

        /* Runs on the UI thread */
        CARAPI OnCancelled(
            /* [in] */ IInterface* data);

    private:
        AutoPtr<ICountDownLatch> mDone;
        // Set to true to indicate that the task has been posted to a handler for
        // execution at a later time.  Used to throttle updates.
        Boolean mWaiting;
        AutoPtr<IWeakReference> mWeakHost;
    };

public:
    CAR_INTERFACE_DECL()

    AsyncTaskLoader();

    virtual ~AsyncTaskLoader();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IExecutor* excutor);

    CARAPI ToString(
        /* [out] */ String* info);

    /**
     * Set amount to throttle updates by.  This is the minimum time from
     * when the last {@link #loadInBackground()} call has completed until
     * a new load is scheduled.
     *
     * @param delayMS Amount of delay, in milliseconds.
     */
    CARAPI SetUpdateThrottle(
        /* [in] */ Int64 delayMS);

    /**
     * Called if the task was canceled before it was completed.  Gives the class a chance
     * to clean up post-cancellation and to properly dispose of the result.
     *
     * @param data The value that was returned by {@link #loadInBackground}, or null
     * if the task threw {@link OperationCanceledException}.
     */
    CARAPI OnCanceled(
        /* [in] */ IInterface* data);

    /**
     * Called on a worker thread to perform the actual load and to return
     * the result of the load operation.
     *
     * Implementations should not deliver the result directly, but should return them
     * from this method, which will eventually end up calling {@link #deliverResult} on
     * the UI thread.  If implementations need to process the results on the UI thread
     * they may override {@link #deliverResult} and do so there.
     *
     * To support cancellation, this method should periodically check the value of
     * {@link #isLoadInBackgroundCanceled} and terminate when it returns true.
     * Subclasses may also override {@link #cancelLoadInBackground} to interrupt the load
     * directly instead of polling {@link #isLoadInBackgroundCanceled}.
     *
     * When the load is canceled, this method may either return normally or throw
     * {@link OperationCanceledException}.  In either case, the {@link Loader} will
     * call {@link #onCanceled} to perform post-cancellation cleanup and to dispose of the
     * result object, if any.
     *
     * @return The result of the load operation.
     *
     * @throws OperationCanceledException if the load is canceled during execution.
     *
     * @see #isLoadInBackgroundCanceled
     * @see #cancelLoadInBackground
     * @see #onCanceled
     */
    virtual CARAPI LoadInBackground(
        /* [out] */ IInterface** result) = 0;

    /**
     * Called on the main thread to abort a load in progress.
     *
     * Override this method to abort the current invocation of {@link #loadInBackground}
     * that is running in the background on a worker thread.
     *
     * This method should do nothing if {@link #loadInBackground} has not started
     * running or if it has already finished.
     *
     * @see #loadInBackground
     */
    CARAPI CancelLoadInBackground();

    /**
     * Returns true if the current invocation of {@link #loadInBackground} is being canceled.
     *
     * @return True if the current invocation of {@link #loadInBackground} is being canceled.
     *
     * @see #loadInBackground
     */
    CARAPI IsLoadInBackgroundCanceled(
        /* [out] */ Boolean* isCanceled);

    /**
     * Locks the current thread until the loader completes the current load
     * operation. Returns immediately if there is no load operation running.
     * Should not be called from the UI thread: calling it from the UI
     * thread would cause a deadlock.
     * <p>
     * Use for testing only.  <b>Never</b> call this from a UI thread.
     *
     * @hide
     */
    CARAPI WaitForLoader();

    CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

protected:
    CARAPI OnForceLoad();

    CARAPI OnCancelLoad(
        /* [out] */ Boolean* isCanceled);

    /**
     * Calls {@link #loadInBackground()}.
     *
     * This method is reserved for use by the loader framework.
     * Subclasses should override {@link #loadInBackground} instead of this method.
     *
     * @return The result of the load operation.
     *
     * @throws OperationCanceledException if the load is canceled during execution.
     *
     * @see #loadInBackground
     */
    CARAPI OnLoadInBackground(
        /* [out] */ IInterface** result);

    CARAPI ExecutePendingTask();

    CARAPI DispatchOnCancelled(
        /* [in] */ LoadTask* task,
        /* [in] */ IInterface* data);

    CARAPI DispatchOnLoadComplete(
        /* [in] */ LoadTask* task,
        /* [in] */ IInterface* data);

private:
    ECO_LOCAL static const String TAG;
    ECO_LOCAL static const Boolean DEBUG;

private:
    AutoPtr<IExecutor> mExecutor;

    /*volatile*/ AutoPtr<LoadTask> mTask;
    /*volatile*/ AutoPtr<LoadTask> mCancellingTask;

    Int64 mUpdateThrottle;
    Int64 mLastLoadCompleteTime;
    AutoPtr<IHandler> mHandler;

};

}
}
}

#endif //__ELASTOS_DROID_CONTENT_ASYNCTASKLOADER_H__
