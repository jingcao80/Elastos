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