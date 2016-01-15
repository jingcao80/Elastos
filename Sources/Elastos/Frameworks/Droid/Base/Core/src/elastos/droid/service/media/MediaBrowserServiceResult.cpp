
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
