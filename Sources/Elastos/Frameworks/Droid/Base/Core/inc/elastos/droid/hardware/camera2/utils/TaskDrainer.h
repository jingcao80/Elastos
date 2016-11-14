
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TASKDRAINER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TASKDRAINER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Core::Runnable;
using Elastos::Core::Object;
using Elastos::Utility::ISet;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class TaskDrainer
    : public Object
    , public ITaskDrainer
{
private:
    class MyRunnable : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ TaskDrainer* host);

        CARAPI Run();

    private:
        TaskDrainer* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TaskDrainer();

    virtual ~TaskDrainer();

    /**
     * Create a new task drainer; {@code onDrained} callbacks will be posted to the listener
     * via the {@code handler}.
     *
     * @param handler a non-{@code null} handler to use to post runnables to
     * @param listener a non-{@code null} listener where {@code onDrained} will be called
     */
    CARAPI constructor(
        /* [in] */ IHandler* handler,
        /* [in] */ ITaskDrainerDrainListener* listener);

    /**
     * Create a new task drainer; {@code onDrained} callbacks will be posted to the listener
     * via the {@code handler}.
     *
     * @param handler a non-{@code null} handler to use to post runnables to
     * @param listener a non-{@code null} listener where {@code onDrained} will be called
     * @param name an optional name used for debug logging
     */
    CARAPI constructor(
        /* [in] */ IHandler* handler,
        /* [in] */ ITaskDrainerDrainListener* listener,
        /* [in] */ const String& name);

    /**
     * Mark an asynchronous task as having started.
     *
     * <p>A task cannot be started more than once without first having finished. Once
     * draining begins with {@link #beginDrain}, no new tasks can be started.</p>
     *
     * @param task a key to identify a task
     *
     * @see #taskFinished
     * @see #beginDrain
     *
     * @throws IllegalStateException
     *          If attempting to start a task which is already started (and not finished),
     *          or if attempting to start a task after draining has begun.
     */
    CARAPI TaskStarted(
        /* [in] */ IInterface* task);

    /**
     * Mark an asynchronous task as having finished.
     *
     * <p>A task cannot be finished if it hasn't started. Once finished, a task
     * cannot be finished again (unless it's started again).</p>
     *
     * @param task a key to identify a task
     *
     * @see #taskStarted
     * @see #beginDrain
     *
     * @throws IllegalStateException
     *          If attempting to start a task which is already finished (and not re-started),
     */
    CARAPI TaskFinished(
        /* [in] */ IInterface* task);

    /**
     * Do not allow any more tasks to be started; once all existing started tasks are finished,
     * fire the {@link DrainListener#onDrained} callback asynchronously.
     *
     * <p>This operation is idempotent; calling it more than once has no effect.</p>
     */
    CARAPI BeginDrain();

private:
    CARAPI CheckIfDrainFinished();

    CARAPI PostDrained();

private:
    static const String TAG;
    Boolean VERBOSE;//TODO: = Log.isLoggable(TAG, Log.VERBOSE);

    AutoPtr<IHandler> mHandler;
    AutoPtr<ITaskDrainerDrainListener> mListener;
    String mName;

    /** Set of tasks which have been started but not yet finished with #taskFinished */
    AutoPtr<ISet> mTaskSet;
    Object mLock;

    Boolean mDraining;
    Boolean mDrainFinished;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TASKDRAINER_H__
