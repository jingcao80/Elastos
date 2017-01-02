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
