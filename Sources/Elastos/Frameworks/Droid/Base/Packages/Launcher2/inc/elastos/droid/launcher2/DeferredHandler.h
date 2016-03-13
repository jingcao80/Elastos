#ifndef  __ELASTOS_DROID_LAUNCHER2_DEFERREDHANDLER_H__
#define  __ELASTOS_DROID_LAUNCHER2_DEFERREDHANDLER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * Queue of things to run on a looper thread.  Items posted with {@link #post} will not
 * be actually enqued on the handler until after the last one has run, to keep from
 * starving the thread.
 *
 * This class is fifo.
 */
class DeferredHandler
    : public Object
{
private:
    class Impl
        : public Handler
        , public IIdleHandler
    {
    public:
        CAR_INTERFACE_DECL();

        Impl(
            /* [in] */ DeferredHandler* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

        CARAPI QueueIdle(
            /* [out] */ Boolean* result);

    private:
        DeferredHandler* mHost;
    };

    class IdleRunnable
        : public Runnable
        , public IIdleRunnable
    {
    public:
        CAR_INTERFACE_DECL();

        IdleRunnable(
            /* [in] */ IRunnable* r);

        CARAPI Run();

    private:
        AutoPtr<IRunnable> mRunnable;
    };

public:
    DeferredHandler();

    /** Schedule runnable to run after everything that's on the queue right now. */
    CARAPI Post(
        /* [in] */ IRunnable* runnable);

    CARAPI Post(
        /* [in] */ IRunnable* runnable,
        /* [in] */ Int32 type);

    /** Schedule runnable to run when the queue goes idle. */
    CARAPI PostIdle(
        /* [in] */ IRunnable* runnable);

    CARAPI PostIdle(
        /* [in] */ IRunnable* runnable,
        /* [in] */ Int32 type);

    CARAPI CancelRunnable(
        /* [in] */ IRunnable* runnable);

    CARAPI CancelAllRunnablesOfType(
        /* [in] */ Int32 type);

    CARAPI Cancel();

    /** Runs all queued Runnables from the calling thread. */
    CARAPI Flush();

    CARAPI ScheduleNextLocked();

private:
    AutoPtr<ILinkedList> mQueue = new LinkedList<Pair<Runnable, Integer>>();
    Object mQueueLock;
    AutoPtr<IMessageQueue> mMessageQueue = Looper.myQueue();
    AutoPTR<Impl> mHandler = new Impl();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_DEFERREDHANDLER_H__