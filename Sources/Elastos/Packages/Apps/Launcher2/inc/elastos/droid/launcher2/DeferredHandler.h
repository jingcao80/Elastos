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

#ifndef  __ELASTOS_DROID_LAUNCHER2_DEFERREDHANDLER_H__
#define  __ELASTOS_DROID_LAUNCHER2_DEFERREDHANDLER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>
#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IIdleHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IMessageQueue;
using Elastos::Core::Object;
using Elastos::Core::IRunnable;
using Elastos::Utility::ILinkedList;

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
        TO_STRING_IMPL("DeferredHandler::Impl")

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
    AutoPtr<ILinkedList> mQueue;
    Object mQueueLock;
    AutoPtr<IMessageQueue> mMessageQueue;
    AutoPtr<Impl> mHandler;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_DEFERREDHANDLER_H__