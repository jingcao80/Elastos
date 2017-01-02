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

#include "org/apache/http/impl/conn/tsccm/RefQueueWorker.h"
#include "elastos/core/Thread.h"
#include "elastos/utility/logging/Logger.h"

using Elastos::Core::Thread;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Apache {
namespace Http {
namespace Impl {
namespace Conn {
namespace Tsccm {

RefQueueWorker::RefQueueWorker(
    /* [in] */ IQueue* queue,
    /* [in] */ IRefQueueHandler* handler)
{
    if (queue == NULL) {
        Logger::E("RefQueueWorker", "Queue must not be null.");
        assert(0);
        // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (handler == NULL) {
        Logger::E("RefQueueWorker", "Handler must not be null.");
        assert(0);
        // return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mRefQueue   = queue;
    mRefHandler = handler;
}

ECode RefQueueWorker::Run()
{
    if (mWorkerThread == NULL) {
        mWorkerThread = Thread::GetCurrentThread();
    }

    while (mWorkerThread == Thread::GetCurrentThread()) {
        // try {
        // remove the next reference and process it
        AutoPtr<IInterface> ref;
        mRefQueue->Remove((IInterface**)&ref);
        mRefHandler->HandleReference(ref);
        // } catch (InterruptedException e) {
        //     //@@@ is logging really necessary? this here is the
        //     //@@@ only reason for having a log in this class
        //     if (log.isDebugEnabled()) {
        //         log.debug(this.toString() + " interrupted", e);
        //     }
        // }
    }
    return NOERROR;
}

void RefQueueWorker::Shutdown()
{
    AutoPtr<IThread> wt = mWorkerThread;
    if (wt != NULL) {
        mWorkerThread = NULL; // indicate shutdown
        wt->Interrupt();
    }
}

ECode RefQueueWorker::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    String s;
    IObject::Probe(mWorkerThread)->ToString(&s);
    *str = String("RefQueueWorker::") + s;
    return NOERROR;
}

} // namespace Tsccm
} // namespace Conn
} // namespace Impl
} // namespace Http
} // namespace Apache
} // namespace Org