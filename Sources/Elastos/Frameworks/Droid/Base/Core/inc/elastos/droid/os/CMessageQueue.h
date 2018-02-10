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

#ifndef __ELASTOS_DROID_OS_CMESSAGEQUEUE_H__
#define __ELASTOS_DROID_OS_CMESSAGEQUEUE_H__

#include "_Elastos_Droid_Os_CMessageQueue.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::IRunnable;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CMessageQueue)
    , public Object
    , public IMessageQueue
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CMessageQueue();

    ~CMessageQueue();

    CARAPI constructor(
        /* [in] */ Boolean quitAllowed);

    /**
     * Add a new {@link IdleHandler} to this message queue.  This may be
     * removed automatically for you by returning false from
     * {@link IdleHandler#queueIdle IdleHandler.queueIdle()} when it is
     * invoked, or explicitly removing it with {@link #removeIdleHandler}.
     *
     * <p>This method is safe to call from any thread.
     *
     * @param handler The IdleHandler to be added.
     */
    CARAPI AddIdleHandler(
        /* [in] */ IIdleHandler* handler);

    /**
     * Remove an {@link IdleHandler} from the queue that was previously added
     * with {@link #addIdleHandler}.  If the given object is not currently
     * in the idle list, nothing is done.
     *
     * @param handler The IdleHandler to be removed.
     */
    CARAPI RemoveIdleHandler(
        /* [in] */ IIdleHandler* handler);

    CARAPI GetNext(
        /* [out] */ IMessage** msg);

    CARAPI Quit(
        /* [in] */ Boolean safe);

    CARAPI EnqueueMessage(
        /* [in] */ IMessage* msg,
        /* [in] */ Int64 when,
        /* [out] */ Boolean* result);

    CARAPI HasMessages(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI HasMessages(
        /* [in] */ IHandler* h,
        /* [in] */ IRunnable* r,
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI IsIdling(
        /* [out] */ Boolean* result);

    CARAPI RemoveMessages(
        /* [in] */ IHandler* h,
        /* [in] */ Int32 what,
        /* [in] */ IInterface* object);

    CARAPI RemoveMessages(
        /* [in] */ IHandler* h,
        /* [in] */ IRunnable* r,
        /* [in] */ IInterface* object);

    CARAPI RemoveCallbacksAndMessages(
        /* [in] */ IHandler* h,
        /* [in] */ IInterface* object);

    CARAPI GetNativeMessageQueue(
        /* [out] */ HANDLE* queue);

    CARAPI RemoveSyncBarrier(
        /* [in] */ Int32 token);

    CARAPI EnqueueSyncBarrier(
        /* [in] */ Int64 when,
        /* [out] */ Int32* result);

private:
    // Disposes of the underlying message queue.
    // Must only be called on the looper thread or the finalizer.
    CARAPI Dispose();

    CARAPI NativeInit();

    CARAPI_(void) NativeDestroy();

    CARAPI_(void) NativePollOnce(
        /* [in] */ Int32 timeoutMillis);

    CARAPI_(void) NativeWake();

    CARAPI_(Boolean) NativeIsIdling();

    CARAPI DebugMessage(
        /* [in] */ IMessage* msg,
        /* [in] */ const char* info);

    CARAPI RemoveAllMessagesLocked();

    CARAPI RemoveAllFutureMessagesLocked();

    CARAPI_(Boolean) IsIdlingLocked();

public:
    HANDLE mPtr;

private:
    static const String TAG;
    static const Boolean DBG;

    // True if the message queue can be quit.
    Boolean mQuitAllowed;

     // used by native code

    AutoPtr<IMessage> mMessages;
    List<AutoPtr<IIdleHandler> > mIdleHandlers;
    AutoPtr<ArrayOf<IIdleHandler*> > mPendingIdleHandlers;
    Boolean mQuitting;

    // Indicates whether next() is blocked waiting in pollOnce() with a non-zero timeout.
    Boolean mBlocked;

    // The next barrier token.
    // Barriers are indicated by messages with a null target whose arg1 field carries the token.
    Int32 mNextBarrierToken;

    Object mLock;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CMESSAGEQUEUE_H__
