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

#ifndef __ELASTOS_DROID_OS_HANDLERTHREAD_H__
#define __ELASTOS_DROID_OS_HANDLERTHREAD_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/Thread.h>

using Elastos::Core::Thread;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Handy class for starting a new thread that has a looper. The looper can then be
 * used to create handler classes. Note that start() must still be called.
 */
class ECO_PUBLIC HandlerThread
    : public Thread
    , public IHandlerThread
{
public:
    CAR_INTERFACE_DECL()

    HandlerThread(
        /* [in] */ const String& name);

    HandlerThread(
        /* [in] */ const String& name,
        /* [in] */ Int32 priority);

    CARAPI constructor(
        /* [in] */ const String& name);

    /**
     * Constructs a HandlerThread.
     * @param name
     * @param priority The priority to run the thread at. The value supplied must be from
     * {@link android.os.Process} and not from java.lang.Thread.
     */
    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ Int32 priority);

    CARAPI Run();

    CARAPI GetLooper(
        /* [out] */ ILooper** looper);

    /**
     * Quits the handler thread's looper.
     * <p>
     * Causes the handler thread's looper to terminate without processing any
     * more messages in the message queue.
     * </p><p>
     * Any attempt to post messages to the queue after the looper is asked to quit will fail.
     * For example, the {@link Handler#sendMessage(Message)} method will return false.
     * </p><p class="note">
     * Using this method may be unsafe because some messages may not be delivered
     * before the looper terminates.  Consider using {@link #quitSafely} instead to ensure
     * that all pending work is completed in an orderly manner.
     * </p>
     *
     * @return True if the looper looper has been asked to quit or false if the
     * thread had not yet started running.
     *
     * @see #quitSafely
     */
    CARAPI Quit(
        /* [out] */ Boolean* result);

    /**
     * Quits the handler thread's looper safely.
     * <p>
     * Causes the handler thread's looper to terminate as soon as all remaining messages
     * in the message queue that are already due to be delivered have been handled.
     * Pending delayed messages with due times in the future will not be delivered.
     * </p><p>
     * Any attempt to post messages to the queue after the looper is asked to quit will fail.
     * For example, the {@link Handler#sendMessage(Message)} method will return false.
     * </p><p>
     * If the thread has not been started or has finished (that is if
     * {@link #getLooper} returns null), then false is returned.
     * Otherwise the looper is asked to quit and true is returned.
     * </p>
     *
     * @return True if the looper looper has been asked to quit or false if the
     * thread had not yet started running.
     */
    CARAPI QuitSafely(
        /* [out] */ Boolean* result);

    CARAPI GetThreadId(
        /* [out] */ Int32* tid);

protected:
    HandlerThread();

    /**
     * Call back method that can be explicitly overridden if needed to execute some
     * setup before Looper loops.
     */
    virtual CARAPI_(void) OnLooperPrepared();

protected:
    Int32 mPriority;
    Int32 mTid;
    AutoPtr<ILooper> mLooper;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_HANDLERTHREAD_H__
