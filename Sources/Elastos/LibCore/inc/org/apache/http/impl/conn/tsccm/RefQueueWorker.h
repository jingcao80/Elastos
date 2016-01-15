
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_REFQUEUEWORKER_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_REFQUEUEWORKER_H__

#include "Elastos.CoreLibrary.Apache.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/core/Object.h"

using Elastos::Core::IRunnable;
using Elastos::Core::IThread;
using Elastos::Utility::IQueue;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

/**
 * A worker thread for processing queued references.
 * {@link Reference Reference}s can be
 * {@link ReferenceQueue queued}
 * automatically by the garbage collector.
 * If that feature is used, a daemon thread should be executing
 * this worker. It will pick up the queued references and pass them
 * on to a handler for appropriate processing.
 */
class RefQueueWorker
    : public Object
    , public IRunnable
{
public:
    /**
     * Instantiates a new worker to listen for lost connections.
     *
     * @param queue     the queue on which to wait for references
     * @param handler   the handler to pass the references to
     */
    RefQueueWorker(
        /* [in] */ IQueue* queue,
        /* [in] */ IRefQueueHandler* handler);

    CAR_INTERFACE_DECL()

    /**
     * The main loop of this worker.
     * If initialization succeeds, this method will only return
     * after {@link #shutdown shutdown()}. Only one thread can
     * execute the main loop at any time.
     */
    CARAPI Run();

    /**
     * Shuts down this worker.
     * It can be re-started afterwards by another call to {@link #run run()}.
     */
    CARAPI_(void) Shutdown();

    /**
     * Obtains a description of this worker.
     *
     * @return  a descriptive string for this worker
     */
    CARAPI ToString(
        /* [out] */ String* str);

protected:
    /** The reference queue to monitor. */
    AutoPtr<IQueue> mRefQueue;

    /** The handler for the references found. */
    AutoPtr<IRefQueueHandler> mRefHandler;

    /**
     * The thread executing this handler.
     * This attribute is also used as a shutdown indicator.
     */
    AutoPtr<IThread> mWorkerThread;

private:
    // private final Log log = LogFactory.getLog(getClass());
};

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_RefQueueWorker_H__
