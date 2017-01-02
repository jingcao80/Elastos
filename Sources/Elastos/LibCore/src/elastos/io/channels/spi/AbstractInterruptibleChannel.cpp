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

#include "AbstractInterruptibleChannel.h"
#include "Thread.h"
#include "AutoLock.h"

using Elastos::Core::Thread;

namespace Elastos {
namespace IO {
namespace Channels {
namespace Spi {

//==========================================================
//       AbstractInterruptibleChannel::ActionRunnable
//==========================================================

AbstractInterruptibleChannel::ActionRunnable::ActionRunnable(
    /* [in] */ AbstractInterruptibleChannel* channel)
{
    mChannel = channel;
}

ECode AbstractInterruptibleChannel::ActionRunnable::Run()
{
    mChannel->SetInterrupted(TRUE);
    return mChannel->Close();
}

//==========================================================
//       AbstractInterruptibleChannel
//==========================================================

CAR_INTERFACE_IMPL_4(AbstractInterruptibleChannel, Object, IChannel, ICloseable, IInterruptibleChannel, IAbstractInterruptibleChannel)

AbstractInterruptibleChannel::AbstractInterruptibleChannel()
    : mClosed(FALSE)
    , mInterrupted(FALSE)
{
    mInterruptAndCloseRunnable = new ActionRunnable(this);
}

AbstractInterruptibleChannel::~AbstractInterruptibleChannel()
{
}

Boolean AbstractInterruptibleChannel::IsOpen()
{
    AutoLock lock(this);
    Boolean ret = !mClosed;
    return ret;
}

ECode AbstractInterruptibleChannel::IsOpen(
    /* [out] */ Boolean* isOpen)
{
    VALIDATE_NOT_NULL(isOpen);
    *isOpen = IsOpen();
    return NOERROR;
}

ECode AbstractInterruptibleChannel::Close()
{
    ECode retCode = E_INTERRUPTED;

    if (!mClosed) {
        AutoLock lock(this);
        if (!mClosed) {
            mClosed = TRUE;
            retCode = this->ImplCloseChannel();
        }
    }

    return retCode;
}

void AbstractInterruptibleChannel::SetInterrupted(
    /* [in] */ Boolean value)
{
    mInterrupted = value;
}

void AbstractInterruptibleChannel::Begin()
{
    Thread::GetCurrentThread()->PushInterruptAction(mInterruptAndCloseRunnable);
}

ECode AbstractInterruptibleChannel::End(Boolean success)
{
    Thread::GetCurrentThread()->PopInterruptAction(mInterruptAndCloseRunnable);

    if (mInterrupted) {
        mInterrupted = FALSE;
        return E_CLOSED_BY_INTERRUPT_EXCEPTION;
    }
    if (!success && mClosed) {
        return E_ASYNCHRONOUS_CLOSE_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Spi
} // namespace Channels
} // namespace IO
} // namespace Elastos
