
#ifndef __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_WAITINGTHREADABORTER_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_WAITINGTHREADABORTER_H__

#include "org/apache/http/impl/conn/tsccm/WaitingThread.h"

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

/** A simple class that can interrupt a {@link WaitingThread}. */
class WaitingThreadAborter : public Object
{
public:
    WaitingThreadAborter()
        : mAborted(FALSE)
    {}

    /**
     * If a waiting thread has been set, interrupts it.
     */
    CARAPI_(void) Abort();

    /**
     * Sets the waiting thread.  If this has already been aborted,
     * the waiting thread is immediately interrupted.
     *
     * @param waitingThread The thread to interrupt when aborting.
     */
    CARAPI_(void) SetWaitingThread(
        /* [in] */ WaitingThread* waitingThread);

private:
    AutoPtr<WaitingThread> mWaitingThread;
    Boolean mAborted;
};

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_WAITINGTHREADABORTER_H__
