//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_WAITINGTHREAD_H__
#define __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_WAITINGTHREAD_H__

#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "org/apache/http/impl/conn/tsccm/RouteSpecificPool.h"

using Elastos::Core::IThread;
using Elastos::Utility::IDate;
using Elastos::Utility::Concurrent::Locks::ICondition;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

/**
 * Represents a thread waiting for a connection.
 * This class implements throwaway objects. It is instantiated whenever
 * a thread needs to wait. Instances are not re-used, except if the
 * waiting thread experiences a spurious wakeup and continues to wait.
 * <br/>
 * All methods assume external synchronization on the condition
 * passed to the constructor.
 * Instances of this class do <i>not</i> synchronize access!
 *
 * @author <a href="mailto:rolandw at apache.org">Roland Weber</a>
 */
class WaitingThread : public Object
{
public:
    /**
     * Creates a new entry for a waiting thread.
     *
     * @param cond      the condition for which to wait
     * @param pool      the pool on which the thread will be waiting,
     *                  or <code>null</code>
     */
    WaitingThread(
        /* [in] */ ICondition* cond,
        /* [in] */ RouteSpecificPool* pool);

    /**
     * Obtains the condition.
     *
     * @return  the condition on which to wait, never <code>null</code>
     */
    CARAPI_(AutoPtr<ICondition>) GetCondition();

    /**
     * Obtains the pool, if there is one.
     *
     * @return  the pool on which a thread is or was waiting,
     *          or <code>null</code>
     */
    CARAPI_(AutoPtr<RouteSpecificPool>) GetPool();

    /**
     * Obtains the thread, if there is one.
     *
     * @return  the thread which is waiting, or <code>null</code>
     */
    CARAPI_(AutoPtr<IThread>) GetThread();

    /**
     * Blocks the calling thread.
     * This method returns when the thread is notified or interrupted,
     * if a timeout occurrs, or if there is a spurious wakeup.
     * <br/>
     * This method assumes external synchronization.
     *
     * @param deadline  when to time out, or <code>null</code> for no timeout
     *
     * @return  <code>true</code> if the condition was satisfied,
     *          <code>false</code> in case of a timeout.
     *          Typically, a call to {@link #wakeup} is used to indicate
     *          that the condition was satisfied. Since the condition is
     *          accessible outside, this cannot be guaranteed though.
     *
     * @throws InterruptedException     if the waiting thread was interrupted
     *
     * @see #wakeup
     */
    CARAPI Await(
        /* [in] */ IDate* deadline,
        /* [out] */ Boolean* result);

    /**
     * Wakes up the waiting thread.
     * <br/>
     * This method assumes external synchronization.
     */
    CARAPI Wakeup();

    CARAPI_(void) Interrupt();

private:
    /** The condition on which the thread is waiting. */
    AutoPtr<ICondition> mCond;

    /** The route specific pool on which the thread is waiting. */
    //@@@ replace with generic pool interface
    AutoPtr<RouteSpecificPool> mPool;

    /** The thread that is waiting for an entry. */
    AutoPtr<IThread> mWaiter;

    /** True if this was interrupted. */
    Boolean mAborted;
};

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HTTP_IMPL_CONN_TSCCM_WAITINGTHREAD_H__
