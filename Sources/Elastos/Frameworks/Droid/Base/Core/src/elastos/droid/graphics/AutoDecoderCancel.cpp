
#include "elastos/droid/graphics/AutoDecoderCancel.h"
#include <elastos/core/Object.h>
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Graphics {

static Object gAutoDecoderCancelMutex;
static AutoDecoderCancel* gAutoDecoderCancel;
#ifdef SK_DEBUG
static int gAutoDecoderCancelCount;
#endif

AutoDecoderCancel::AutoDecoderCancel(
    /* [in] */ IBitmapFactoryOptions* options,
    /* [in] */ SkImageDecoder* decoder)
{
    mOptions = options;
    mDecoder = decoder;

    if (NULL != options) {
        AutoLock lock(gAutoDecoderCancelMutex);

        // Add us as the head of the list
        mPrev = NULL;
        mNext = gAutoDecoderCancel;
        if (gAutoDecoderCancel) {
            gAutoDecoderCancel->mPrev = this;
        }
        gAutoDecoderCancel = this;

        SkDEBUGCODE(gAutoDecoderCancelCount += 1;)
        Validate();
    }
}

AutoDecoderCancel::~AutoDecoderCancel()
{
    if (NULL != mOptions) {
        AutoLock lock(gAutoDecoderCancelMutex);

        // take us out of the dllist
        AutoDecoderCancel* prev = mPrev;
        AutoDecoderCancel* next = mNext;

        if (prev) {
            SkASSERT(prev->mNext == this);
            prev->mNext = next;
        }
        else {
            SkASSERT(gAutoDecoderCancel == this);
            gAutoDecoderCancel = next;
        }
        if (next) {
            SkASSERT(next->mPrev == this);
            next->mPrev = prev;
        }

        SkDEBUGCODE(gAutoDecoderCancelCount -= 1;)
        Validate();
    }
}

Boolean AutoDecoderCancel::RequestCancel(
    /* [in] */ IBitmapFactoryOptions* options)
{
    AutoLock lock(gAutoDecoderCancelMutex);

    Validate();

    AutoDecoderCancel* pair = gAutoDecoderCancel;
    while (pair != NULL) {
        if (pair->mOptions.Get() == options) {
            pair->mDecoder->cancelDecode();
            return TRUE;
        }
        pair = pair->mNext;
    }
    return FALSE;
}

#ifdef SK_DEBUG
// can only call this inside a lock on gAutoDecoderCancelMutex
void AutoDecoderCancel::Validate()
{
    const int gCount = gAutoDecoderCancelCount;

    if (gCount == 0) {
        SkASSERT(gAutoDecoderCancel == NULL);
    }
    else {
        SkASSERT(gCount > 0);

        AutoDecoderCancel* curr = gAutoDecoderCancel;
        SkASSERT(curr);
        SkASSERT(curr->mPrev == NULL);

        int count = 0;
        while (curr) {
            count += 1;
            SkASSERT(count <= gCount);
            if (curr->mPrev) {
                SkASSERT(curr->mPrev->mNext == curr);
            }
            if (curr->mNext) {
                SkASSERT(curr->mNext->mPrev == curr);
            }
            curr = curr->mNext;
        }
        SkASSERT(count == gCount);
    }
}
#endif

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
