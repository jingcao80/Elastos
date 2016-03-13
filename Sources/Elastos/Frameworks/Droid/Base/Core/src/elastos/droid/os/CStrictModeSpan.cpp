#include "elastos/droid/os/CStrictModeSpan.h"
#include <elastos/core/AutoLock.h>

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CStrictModeSpan, Object, IStrictModeSpan)

CAR_OBJECT_IMPL(CStrictModeSpan)

ECode CStrictModeSpan::constructor()
{
    mContainerState = NULL;
    return NOERROR;
}

ECode CStrictModeSpan::constructor(
    /* [in] */ Handle32 threadState)
{
    mContainerState = (CStrictMode::ThreadSpanState*)threadState;
    return NOERROR;
}

ECode CStrictModeSpan::Finish()
{
    AutoPtr<CStrictMode::ThreadSpanState> state = mContainerState;
    {
        AutoLock lock(mContainerStateLock);
        if (mName.IsNull()) {
            // Duplicate finish call.  Ignore.
            return NOERROR;
        }

        // Remove ourselves from the active list.
        if (mPrev != NULL) {
            mPrev->mNext = mNext;
        }
        if (mNext != NULL) {
            mNext->mPrev = mPrev;
        }
        if ((state->mActiveHead).Get() == this) {
            state->mActiveHead = mNext;
        }

        state->mActiveSize--;

        //if (LOG_V) Log.d(TAG, "Span finished=" + mName + "; size=" + state->mActiveSize);

        mCreateMillis = -1;
        mName = String(NULL);
        mPrev = NULL;
        mNext = NULL;

        // Add ourselves to the freeList, if it's not already
        // too big.
        if (state->mFreeListSize < 5) {
            mNext = state->mFreeListHead;
            state->mFreeListHead = this;
            state->mFreeListSize++;
        }
    }
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos