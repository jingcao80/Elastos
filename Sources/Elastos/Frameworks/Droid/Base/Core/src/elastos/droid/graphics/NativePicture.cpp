
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/NativeCanvas.h"
#include "elastos/droid/graphics/NativePicture.h"
#include "elastos/droid/ext/frameworkext.h"
#include "SkStream.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

NativePicture::NativePicture(
    /* [in] */ const NativePicture* src)
{
    if (NULL != src) {
        mWidth = src->GetWidth();
        mHeight = src->GetHeight();
        if (NULL != src->mPicture.get()) {
            mPicture.reset(SkRef(src->mPicture.get()));
        }
        if (NULL != src->mRecorder.get()) {
            mPicture.reset(src->MakePartialCopy());
        }
    }
    else {
        mWidth = 0;
        mHeight = 0;
    }
}

NativeCanvas* NativePicture::BeginRecording(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mPicture.reset(NULL);
    mRecorder.reset(new SkPictureRecorder);
    mWidth = width;
    mHeight = height;
    SkCanvas* canvas = mRecorder->beginRecording(width, height, NULL, 0);
    // the java side will wrap this guy in a Canvas.java, which will call
    // unref in its finalizer, so we have to ref it here, so that both that
    // Canvas.java and our picture can both be owners
    canvas->ref();
    return NativeCanvas::create_canvas(canvas);
}

void NativePicture::EndRecording()
{
    if (NULL != mRecorder.get()) {
        mPicture.reset(mRecorder->endRecording());
        mRecorder.reset(NULL);
    }
}

Int32 NativePicture::GetWidth() const
{
    if (NULL != mPicture.get()) {
        SkASSERT(mPicture->width() == mWidth);
        SkASSERT(mPicture->height() == mHeight);
    }

    return mWidth;
}

Int32 NativePicture::GetHeight() const
{
    if (NULL != mPicture.get()) {
        SkASSERT(mPicture->width() == mWidth);
        SkASSERT(mPicture->height() == mHeight);
    }

    return mHeight;
}

NativePicture* NativePicture::CreateFromStream(
    /* [in] */ SkStream* stream)
{
    NativePicture* newPict = new NativePicture;

    newPict->mPicture.reset(SkPicture::CreateFromStream(stream));
    if (NULL != newPict->mPicture.get()) {
        newPict->mWidth = newPict->mPicture->width();
        newPict->mHeight = newPict->mPicture->height();
    }

    return newPict;
}

void NativePicture::Serialize(
    /* [in] */ SkWStream* stream) const
{
    if (NULL != mRecorder.get()) {
        SkAutoTDelete<SkPicture> tempPict(this->MakePartialCopy());
        tempPict->serialize(stream);
    }
    else if (NULL != mPicture.get()) {
        mPicture->serialize(stream);
    }
    else {
        SkPicture empty;
        empty.serialize(stream);
    }
}

void NativePicture::Draw(
    /* [in] */ NativeCanvas* canvas)
{
    if (NULL != mRecorder.get()) {
        EndRecording();
        SkASSERT(NULL != mPicture.get());
    }
    if (NULL != mPicture.get()) {
        // TODO: remove this const_cast once pictures are immutable
        const_cast<SkPicture*>(mPicture.get())->draw(canvas->getSkCanvas());
    }
}

SkPicture* NativePicture::MakePartialCopy() const
{
    SkASSERT(NULL != mRecorder.get());

    SkPictureRecorder reRecorder;

    SkCanvas* canvas = reRecorder.beginRecording(mWidth, mHeight, NULL, 0);
    mRecorder->partialReplay(canvas);
    return reRecorder.endRecording();
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
