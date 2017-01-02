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

#include "elastos/droid/service/media/MediaBrowserServiceResult.h"

namespace Elastos {
namespace Droid {
namespace Service {
namespace Media {

MediaBrowserServiceResult::MediaBrowserServiceResult()
    : mDetachCalled(FALSE)
    , mSendResultCalled(FALSE)
{
}

ECode MediaBrowserServiceResult::constructor(
    /* [in] */ IInterface * debug)
{
    mDebug = debug;
    return NOERROR;
}

/**
 * Send the result back to the caller.
 */
ECode MediaBrowserServiceResult::SendResult(
    /* [in] */ IInterface * result)
{
    if (mSendResultCalled) {
        // throw new IllegalStateException("sendResult() called twice for: " + mDebug);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mSendResultCalled = TRUE;
    OnResultSent(result);
    return NOERROR;
}

/**
 * Detach this message from the current thread and allow the {@link #sendResult}
 * call to happen later.
 */
ECode MediaBrowserServiceResult::Detach()
{
    if (mDetachCalled) {
        // throw new IllegalStateException("detach() called when detach() had already"
        //         + " been called for: " + mDebug);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mSendResultCalled) {
        // throw new IllegalStateException("detach() called when sendResult() had already"
        //         + " been called for: " + mDebug);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    mDetachCalled = TRUE;
    return NOERROR;
}

Boolean MediaBrowserServiceResult::IsDone()
{
    return mDetachCalled || mSendResultCalled;
}

/**
 * Called when the result is sent, after assertions about not being called twice
 * have happened.
 */
ECode MediaBrowserServiceResult::OnResultSent(
    /* [in] */ IInterface * result)
{
    return NOERROR;
}

} // namespace Media
} // namespace Service
} // namepsace Droid
} // namespace Elastos
