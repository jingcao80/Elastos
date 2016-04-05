
#ifndef __ELASTOS_DROID_CONTENT_LOADER_H__
#define __ELASTOS_DROID_CONTENT_LOADER_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Object.h>

using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * A class that performs asynchronous loading of data. While Loaders are active
 * they should monitor the source of their data and deliver new results when the contents
 * change.  See {@link android.app.LoaderManager} for more detail.
 *
 * <p><b>Note on threading:</b> Clients of loaders should as a rule perform
 * any calls on to a Loader from the main thread of their process (that is,
 * the thread the Activity callbacks and other things occur on).  Subclasses
 * of Loader (such as {@link AsyncTaskLoader}) will often perform their work
 * in a separate thread, but when delivering their results this too should
 * be done on the main thread.</p>
 *
 * <p>Subclasses generally must implement at least {@link #onStartLoading()},
 * {@link #onStopLoading()}, {@link #onForceLoad()}, and {@link #onReset()}.</p>
 *
 * <p>Most implementations should not derive directly from this class, but
 * instead inherit from {@link AsyncTaskLoader}.</p>
 *
 * <div class="special reference">
 * <h3>Developer Guides</h3>
 * <p>For more information about using loaders, read the
 * <a href="{@docRoot}guide/topics/fundamentals/loaders.html">Loaders</a> developer guide.</p>
 * </div>
 *
 * @param <D> The result returned when the load is complete
 */
class ECO_PUBLIC Loader
    : public Object
    , public ILoader
{
public:
    CAR_INTERFACE_DECL()

    Loader();

    virtual ~Loader();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Sends the result of the load to the registered listener. Should only be called by subclasses.
     *
     * Must be called from the process's main thread.
     *
     * @param data the result of the load
     */
    CARAPI DeliverResult(
        /* [in] */ IInterface* data);

    /**
     * Informs the registered {@link OnLoadCanceledListener} that the load has been canceled.
     * Should only be called by subclasses.
     *
     * Must be called from the process's main thread.
     */
    CARAPI DeliverCancellation();

    /**
     * @return an application context retrieved from the Context passed to the constructor.
     */
    CARAPI GetContext(
        /* [out] */ IContext** context);

    /**
     * @return the ID of this loader
     */
    CARAPI GetId(
        /* [out] */ Int32* id);

    /**
     * Registers a class that will receive callbacks when a load is complete.
     * The callback will be called on the process's main thread so it's safe to
     * pass the results to widgets.
     *
     * <p>Must be called from the process's main thread.
     */
    CARAPI RegisterListener(
        /* [in] */ Int32 id,
        /* [in] */ ILoaderOnLoadCompleteListener* listener);

    /**
     * Remove a listener that was previously added with {@link #registerListener}.
     *
     * Must be called from the process's main thread.
     */
    CARAPI UnregisterListener(
        /* [in] */ ILoaderOnLoadCompleteListener* listener);

    /**
     * Registers a listener that will receive callbacks when a load is canceled.
     * The callback will be called on the process's main thread so it's safe to
     * pass the results to widgets.
     *
     * Must be called from the process's main thread.
     *
     * @param listener The listener to register.
     */
    CARAPI RegisterOnLoadCanceledListener(
        /* [in] */ ILoaderOnLoadCanceledListener* listener);

    /**
     * Unregisters a listener that was previously added with
     * {@link #registerOnLoadCanceledListener}.
     *
     * Must be called from the process's main thread.
     *
     * @param listener The listener to unregister.
     */
    CARAPI UnregisterOnLoadCanceledListener(
        /* [in] */ ILoaderOnLoadCanceledListener* listener);

    /**
     * Return whether this load has been started.  That is, its {@link #startLoading()}
     * has been called and no calls to {@link #stopLoading()} or
     * {@link #reset()} have yet been made.
     */
    CARAPI IsStarted(
        /* [out] */ Boolean* isStarted);

    /**
     * Return whether this loader has been abandoned.  In this state, the
     * loader <em>must not</em> report any new data, and <em>must</em> keep
     * its last reported data valid until it is finally reset.
     */
    CARAPI IsAbandoned(
        /* [out] */ Boolean* isAbandoned);

    /**
     * Return whether this load has been reset.  That is, either the loader
     * has not yet been started for the first time, or its {@link #reset()}
     * has been called.
     */
    CARAPI IsReset(
        /* [out] */ Boolean* isReset);

    /**
     * This function will normally be called for you automatically by
     * {@link android.app.LoaderManager} when the associated fragment/activity
     * is being started.  When using a Loader with {@link android.app.LoaderManager},
     * you <em>must not</em> call this method yourself, or you will conflict
     * with its management of the Loader.
     *
     * Starts an asynchronous load of the Loader's data. When the result
     * is ready the callbacks will be called on the process's main thread.
     * If a previous load has been completed and is still valid
     * the result may be passed to the callbacks immediately.
     * The loader will monitor the source of
     * the data set and may deliver future callbacks if the source changes.
     * Calling {@link #stopLoading} will stop the delivery of callbacks.
     *
     * <p>This updates the Loader's internal state so that
     * {@link #isStarted()} and {@link #isReset()} will return the correct
     * values, and then calls the implementation's {@link #onStartLoading()}.
     *
     * <p>Must be called from the process's main thread.
     */
    CARAPI StartLoading();

    /**
     * Attempt to cancel the current load task.
     * Must be called on the main thread of the process.
     *
     * <p>Cancellation is not an immediate operation, since the load is performed
     * in a background thread.  If there is currently a load in progress, this
     * method requests that the load be canceled, and notes this is the case;
     * once the background thread has completed its work its remaining state
     * will be cleared.  If another load request comes in during this time,
     * it will be held until the canceled load is complete.
     *
     * @return Returns <tt>false</tt> if the task could not be canceled,
     * typically because it has already completed normally, or
     * because {@link #startLoading()} hasn't been called; returns
     * <tt>true</tt> otherwise.  When <tt>true</tt> is returned, the task
     * is still running and the {@link OnLoadCanceledListener} will be called
     * when the task completes.
     */
    CARAPI CancelLoad(
        /* [out] */ Boolean* succeeded);

    /**
     * Force an asynchronous load. Unlike {@link #startLoading()} this will ignore a previously
     * loaded data set and load a new one.  This simply calls through to the
     * implementation's {@link #onForceLoad()}.  You generally should only call this
     * when the loader is started -- that is, {@link #isStarted()} returns true.
     *
     * <p>Must be called from the process's main thread.
     */
    CARAPI ForceLoad();

    /**
     * This function will normally be called for you automatically by
     * {@link android.app.LoaderManager} when the associated fragment/activity
     * is being stopped.  When using a Loader with {@link android.app.LoaderManager},
     * you <em>must not</em> call this method yourself, or you will conflict
     * with its management of the Loader.
     *
     * <p>Stops delivery of updates until the next time {@link #startLoading()} is called.
     * Implementations should <em>not</em> invalidate their data at this point --
     * clients are still free to use the last data the loader reported.  They will,
     * however, typically stop reporting new data if the data changes; they can
     * still monitor for changes, but must not report them to the client until and
     * if {@link #startLoading()} is later called.
     *
     * <p>This updates the Loader's internal state so that
     * {@link #isStarted()} will return the correct
     * value, and then calls the implementation's {@link #onStopLoading()}.
     *
     * <p>Must be called from the process's main thread.
     */
    CARAPI StopLoading();

    /**
     * This function will normally be called for you automatically by
     * {@link android.app.LoaderManager} when restarting a Loader.  When using
     * a Loader with {@link android.app.LoaderManager},
     * you <em>must not</em> call this method yourself, or you will conflict
     * with its management of the Loader.
     *
     * Tell the Loader that it is being abandoned.  This is called prior
     * to {@link #reset} to have it retain its current data but not report
     * any new data.
     */
    CARAPI Abandon();

    /**
     * This function will normally be called for you automatically by
     * {@link android.app.LoaderManager} when destroying a Loader.  When using
     * a Loader with {@link android.app.LoaderManager},
     * you <em>must not</em> call this method yourself, or you will conflict
     * with its management of the Loader.
     *
     * Resets the state of the Loader.  The Loader should at this point free
     * all of its resources, since it may never be called again; however, its
     * {@link #startLoading()} may later be called at which point it must be
     * able to start running again.
     *
     * <p>This updates the Loader's internal state so that
     * {@link #isStarted()} and {@link #isReset()} will return the correct
     * values, and then calls the implementation's {@link #onReset()}.
     *
     * <p>Must be called from the process's main thread.
     */
    CARAPI Reset();

    /**
     * Take the current flag indicating whether the loader's content had
     * changed while it was stopped.  If it had, true is returned and the
     * flag is cleared.
     */
    CARAPI TakeContentChanged(
        /* [out] */ Boolean* succeeded);

    /**
     * Commit that you have actually fully processed a content change that
     * was returned by {@link #takeContentChanged}.  This is for use with
     * {@link #rollbackContentChanged()} to handle situations where a load
     * is cancelled.  Call this when you have completely processed a load
     * without it being cancelled.
     */
    CARAPI CommitContentChanged();

    /**
     * Report that you have abandoned the processing of a content change that
     * was returned by {@link #takeContentChanged()} and would like to rollback
     * to the state where there is again a pending content change.  This is
     * to handle the case where a data load due to a content change has been
     * canceled before its data was delivered back to the loader.
     */
    CARAPI RollbackContentChanged();

    /**
     * Called when {@link ForceLoadContentObserver} detects a change.  The
     * default implementation checks to see if the loader is currently started;
     * if so, it simply calls {@link #forceLoad()}; otherwise, it sets a flag
     * so that {@link #takeContentChanged()} returns true.
     *
     * <p>Must be called from the process's main thread.
     */
    CARAPI OnContentChanged();

    /**
     * For debugging, converts an instance of the Loader's data class to
     * a string that can be printed.  Must handle a null data.
     */
    CARAPI DataToString(
        /* [in] */ IInterface* data,
        /* [out] */ String* str);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Print the Loader's state into the given stream.
     *
     * @param prefix Text to print at the front of each line.
     * @param fd The raw file descriptor that the dump is being sent to.
     * @param writer A PrintWriter to which the dump is to be set.
     * @param args Additional arguments to the dump request.
     */
    CARAPI Dump(
        /* [in] */ const String& prefix,
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* writer,
        /* [in] */ ArrayOf<String>* args);

protected:
    /**
     * Subclasses must implement this to take care of loading their data,
     * as per {@link #startLoading()}.  This is not called by clients directly,
     * but as a result of a call to {@link #startLoading()}.
     */
    CARAPI OnStartLoading();

    /**
     * Subclasses must implement this to take care of requests to {@link #cancelLoad()}.
     * This will always be called from the process's main thread.
     *
     * @return Returns <tt>false</tt> if the task could not be canceled,
     * typically because it has already completed normally, or
     * because {@link #startLoading()} hasn't been called; returns
     * <tt>true</tt> otherwise.  When <tt>true</tt> is returned, the task
     * is still running and the {@link OnLoadCanceledListener} will be called
     * when the task completes.
     */
    CARAPI OnCancelLoad(
        /* [out] */ Boolean* isCanceled);

    /**
     * Subclasses must implement this to take care of requests to {@link #forceLoad()}.
     * This will always be called from the process's main thread.
     */
    CARAPI OnForceLoad();

    /**
     * Subclasses must implement this to take care of stopping their loader,
     * as per {@link #stopLoading()}.  This is not called by clients directly,
     * but as a result of a call to {@link #stopLoading()}.
     * This will always be called from the process's main thread.
     */
    CARAPI OnStopLoading();

    /**
     * Subclasses implement this to take care of being abandoned.  This is
     * an optional intermediate state prior to {@link #onReset()} -- it means that
     * the client is no longer interested in any new data from the loader,
     * so the loader must not report any further updates.  However, the
     * loader <em>must</em> keep its last reported data valid until the final
     * {@link #onReset()} happens.  You can retrieve the current abandoned
     * state with {@link #isAbandoned}.
     */
    CARAPI OnAbandon();

    /**
     * Subclasses must implement this to take care of resetting their loader,
     * as per {@link #reset()}.  This is not called by clients directly,
     * but as a result of a call to {@link #reset()}.
     * This will always be called from the process's main thread.
     */
    CARAPI OnReset();

protected:
    Boolean mContentChanged;

private:
    Int32 mId;
    AutoPtr<ILoaderOnLoadCompleteListener> mListener;
    AutoPtr<ILoaderOnLoadCanceledListener> mOnLoadCanceledListener;
    AutoPtr<IContext> mContext;
    Boolean mStarted;
    Boolean mAbandoned;
    Boolean mReset;
    Boolean mProcessingChange;// = false;
};

}
}
}

#endif //__ELASTOS_DROID_CONTENT_LOADER_H__
