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

#ifndef __ELASTOS_DROID_OS_CCANCELLATIONSIGNAL_H__
#define __ELASTOS_DROID_OS_CCANCELLATIONSIGNAL_H__

#include "_Elastos_Droid_Os_CCancellationSignal.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Provides the ability to cancel an operation in progress.
 */
CarClass(CCancellationSignal)
    , public Object
    , public ICancellationSignal
{
public:
    class Transport
        : public Object
        , public IICancellationSignal
        , public IBinder
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor();

        CARAPI Cancel();

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        friend class CCancellationSignal;
        AutoPtr<ICancellationSignal> mCancellationSignal;
    };

public:
    /**
     * Creates a cancellation signal, initially not canceled.
     */
    CCancellationSignal();

    ~CCancellationSignal();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /**
     * Returns true if the operation has been canceled.
     *
     * @return True if the operation has been canceled.
     */
    CARAPI IsCanceled(
        /* [out] */ Boolean* result);

    /**
     * Throws {@link OperationCanceledException} if the operation has been canceled.
     *
     * @throws OperationCanceledException if the operation has been canceled.
     */
    CARAPI ThrowIfCanceled();

    /**
     * Cancels the operation and signals the cancellation listener.
     * If the operation has not yet started, then it will be canceled as soon as it does.
     */
    CARAPI Cancel();

    /**
     * Sets the cancellation listener to be called when canceled.
     *
     * This method is intended to be used by the recipient of a cancellation signal
     * such as a database or a content provider to handle cancellation requests
     * while performing a long-running operation.  This method is not intended to be
     * used by applications themselves.
     *
     * If {@link CancellationSignal#cancel} has already been called, then the provided
     * listener is invoked immediately.
     *
     * This method is guaranteed that the listener will not be called after it
     * has been removed.
     *
     * @param listener The cancellation listener, or null to remove the current listener.
     */
    CARAPI SetOnCancelListener(
        /* [in] */ ICancellationSignalOnCancelListener* listener);

    /**
     * Sets the remote transport.
     *
     * If {@link CancellationSignal#cancel} has already been called, then the provided
     * remote transport is canceled immediately.
     *
     * This method is guaranteed that the remote transport will not be called after it
     * has been removed.
     *
     * @param remote The remote transport, or null to remove.
     *
     * @hide
     */
    CARAPI SetRemote(
        /* [in] */ IICancellationSignal* remote);

public:
    /**
     * Creates a transport that can be returned back to the caller of
     * a Binder function and subsequently used to dispatch a cancellation signal.
     *
     * @return The new cancellation signal transport.
     *
     * @hide
     */
    static AutoPtr<IICancellationSignal> CreateTransport();

    /**
     * Given a locally created transport, returns its associated cancellation signal.
     *
     * @param transport The locally created transport, or null if none.
     * @return The associated cancellation signal, or null if none.
     *
     * @hide
     */
    static AutoPtr<ICancellationSignal> FromTransport(
        /* [in] */ IICancellationSignal* transport);

private:
    void WaitForCancelFinishedLocked();

private:
    Boolean mIsCanceled;
    AutoPtr<ICancellationSignalOnCancelListener> mOnCancelListener;
    AutoPtr<IICancellationSignal> mRemote;
    Boolean mCancelInProgress;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CCANCELLATIONSIGNAL_H__
