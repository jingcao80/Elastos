
#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TASKSINGLEDRAINER_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TASKSINGLEDRAINER_H__

#include "Elastos.Droid.Hardware.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

class TaskSingleDrainer
    : public Object
    , public ITaskSingleDrainer
{
public:
    CAR_INTERFACE_DECL()

    TaskSingleDrainer();

    virtual ~TaskSingleDrainer();

    CARAPI constructor();

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
     * Mark this asynchronous task as having started.
     *
     * <p>The task cannot be started more than once without first having finished. Once
     * draining begins with {@link #beginDrain}, no new tasks can be started.</p>
     *
     * @see #taskFinished
     * @see #beginDrain
     *
     * @throws IllegalStateException
     *          If attempting to start a task which is already started (and not finished),
     *          or if attempting to start a task after draining has begun.
     */
    CARAPI TaskStarted();

    /**
     * Do not allow any more task re-starts; once the existing task is finished,
     * fire the {@link DrainListener#onDrained} callback asynchronously.
     *
     * <p>This operation is idempotent; calling it more than once has no effect.</p>
     */
    CARAPI BeginDrain();

    /**
     * Mark this asynchronous task as having finished.
     *
     * <p>The task cannot be finished if it hasn't started. Once finished, a task
     * cannot be finished again (unless it's started again).</p>
     *
     * @see #taskStarted
     * @see #beginDrain
     *
     * @throws IllegalStateException
     *          If attempting to start a task which is already finished (and not re-started),
     */
    CARAPI TaskFinished();

private:
    AutoPtr<ITaskDrainer> mTaskDrainer;
    const Object mSingleTask;
};

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_TASKSINGLEDRAINER_H__
