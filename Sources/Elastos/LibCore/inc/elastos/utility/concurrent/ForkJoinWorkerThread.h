
#ifndef __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_FORKJOINWORKERTHREAD_H__
#define __ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_FORKJOINWORKERTHREAD_H__

#include "Thread.h"
#include "CForkJoinPool.h"

using Elastos::Core::Thread;
using Elastos::Core::IThrowable;
using Elastos::Core::IClassLoader;
using Elastos::Core::IRunnable;
using Elastos::Utility::Concurrent::CForkJoinPool;

namespace Elastos {
namespace Utility {
namespace Concurrent {

class ForkJoinWorkerThread
    : public Thread
    , public IForkJoinWorkerThread
{
    friend class CForkJoinPool;
public:
    CAR_INTERFACE_DECL()

    CARAPI GetPool(
        /* [out] */ IForkJoinPool** outpool);

    CARAPI GetPoolIndex(
        /* [out] */ Int32* value);

    CARAPI Run();

    /**
     * Creates a ForkJoinWorkerThread operating in the given pool.
     *
     * @param pool the pool this thread works in
     * @throws NullPointerException if pool is null
     */
    ForkJoinWorkerThread(
        /* [in] */ IForkJoinPool* pool);

    /**
     * Initializes internal state after construction but before
     * processing any tasks. If you override this method, you must
     * invoke {@code super.onStart()} at the beginning of the method.
     * Initialization requires care: Most fields must have legal
     * default values, to ensure that attempted accesses from other
     * threads work correctly even before this thread starts
     * processing tasks.
     */
    CARAPI_(void) OnStart();

    /**
     * Performs cleanup associated with termination of this worker
     * thread.  If you override this method, you must invoke
     * {@code super.onTermination} at the end of the overridden method.
     *
     * @param exception the exception causing this thread to abort due
     * to an unrecoverable error, or {@code null} if completed normally
     */
    CARAPI_(void) OnTermination(
        /* [in] */ IThrowable* exception);

public:
    AutoPtr<IForkJoinPool> mPool;                // the pool this thread works in
    AutoPtr<CForkJoinPool::WorkQueue> mWorkQueue; // work-stealing mechanics
};

} // namespace Concurrent
} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CONCURRENT_ELASTOS_UTILITY_FORKJOINWORKERTHREAD_H__
