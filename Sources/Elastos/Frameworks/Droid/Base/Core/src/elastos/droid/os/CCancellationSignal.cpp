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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/CCancellationSignal.h"
#include "elastos/droid/os/CCancellationSignalTransport.h"
#include <elastos/core/AutoLock.h>

namespace Elastos {
namespace Droid {
namespace Os {

//===============================================================================
//                  CCancellationSignal::Transport
//===============================================================================
CAR_INTERFACE_IMPL_2(CCancellationSignal::Transport, Object, IICancellationSignal, IBinder)

ECode CCancellationSignal::Transport::constructor()
{
    return CCancellationSignal::New((ICancellationSignal**)&mCancellationSignal);
}

ECode CCancellationSignal::Transport::Cancel()
{
    return mCancellationSignal->Cancel();;
}

ECode CCancellationSignal::Transport::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

//===============================================================================
//                  CCancellationSignal
//===============================================================================
CAR_OBJECT_IMPL(CCancellationSignal)

CAR_INTERFACE_IMPL(CCancellationSignal, Object, ICancellationSignal)

CCancellationSignal::CCancellationSignal()
    : mIsCanceled(FALSE)
    , mCancelInProgress(FALSE)
{
}

CCancellationSignal::~CCancellationSignal()
{
}

ECode CCancellationSignal::constructor()
{
    return NOERROR;
}

ECode CCancellationSignal::IsCanceled(
    /* [out] */ Boolean* result)
{
    AutoLock lock(this);
    *result = mIsCanceled;
    return NOERROR;
}

ECode CCancellationSignal::ThrowIfCanceled()
{
    Boolean isCanceled;
    if (IsCanceled(&isCanceled), isCanceled) {
        return E_OPERATION_CANCELED_EXCEPTION;
    }
    return NOERROR;
}

ECode CCancellationSignal::Cancel()
{
    AutoPtr<ICancellationSignalOnCancelListener> listener;
    AutoPtr<IICancellationSignal> remote;
    {
        AutoLock lock(this);
        if (mIsCanceled) {
            return NOERROR;
        }

        mIsCanceled = TRUE;
        mCancelInProgress = TRUE;
        listener = mOnCancelListener;
        remote = mRemote;
    }

    // try {
    if (listener != NULL) {
        listener->OnCancel();
    }
    if (remote != NULL) {
        // try {
        remote->Cancel();
        // } catch (RemoteException ex) {
        // }
    }
    // } finally {
    {
        AutoLock lock(this);
        mCancelInProgress = FALSE;
        NotifyAll();
    }
    // }
    return NOERROR;
}

ECode CCancellationSignal::SetOnCancelListener(
    /* [in] */ ICancellationSignalOnCancelListener* listener)
{
    {
        AutoLock lock(this);
        WaitForCancelFinishedLocked();

        if (mOnCancelListener.Get() == listener) {
            return NOERROR;
        }

        mOnCancelListener = listener;
        if (!mIsCanceled || listener == NULL) {
            return NOERROR;
        }
    }

    listener->OnCancel();
    return NOERROR;
}

ECode CCancellationSignal::SetRemote(
    /* [in] */ IICancellationSignal* remote)
{
    {
        AutoLock lock(this);
        WaitForCancelFinishedLocked();

        if (mRemote.Get() == remote) {
            return NOERROR;
        }

        mRemote = remote;
        if (!mIsCanceled || remote == NULL) {
            return NOERROR;
        }
    }

    // try {
    remote->Cancel();
    // } catch (RemoteException ex) {
    // }
    return NOERROR;
}

void CCancellationSignal::WaitForCancelFinishedLocked()
{
    while (mCancelInProgress) {
        // try {
        Wait();
        // } catch (InterruptedException ex) {
        // }
    }
}

AutoPtr<IICancellationSignal> CCancellationSignal::CreateTransport()
{
    AutoPtr<IICancellationSignal> transport;
    CCancellationSignalTransport::New((IICancellationSignal**)&transport);
    return transport;
}

AutoPtr<ICancellationSignal> CCancellationSignal::FromTransport(
    /* [in] */ IICancellationSignal* cancellationSignal)
{
    if (cancellationSignal != NULL) {
        Transport* transport = (Transport*)cancellationSignal;
        if (transport) {
            return transport->mCancellationSignal;
        }
    }

    return NULL;
}


} // namespace Os
} // namespace Droid
} // namespace Elastos
