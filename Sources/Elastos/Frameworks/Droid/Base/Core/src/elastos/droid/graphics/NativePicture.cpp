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
#include "Elastos.Droid.Os.h"
#include "elastos/droid/graphics/NativePicture.h"
#include <skia/core/SkStream.h>

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
            mPicture = src->mPicture;
        }
        if (NULL != src->mRecorder.get()) {
            mPicture = src->MakePartialCopy();
        }
        Validate();
    }
    else {
        mWidth = 0;
        mHeight = 0;
    }
}

android::Canvas* NativePicture::BeginRecording(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    mPicture.reset(NULL);
    mRecorder.reset(new SkPictureRecorder);
    mWidth = width;
    mHeight = height;
    SkCanvas* canvas = mRecorder->beginRecording(SkIntToScalar(width), SkIntToScalar(height));
    return android::Canvas::create_canvas(canvas, android::Canvas::XformToSRGB::kDefer);
}

void NativePicture::EndRecording()
{
    if (NULL != mRecorder.get()) {
        mPicture = mRecorder->finishRecordingAsPicture();
        Validate();
        mRecorder.reset(NULL);
    }
}

Int32 NativePicture::GetWidth() const
{
    Validate();
    return mWidth;
}

Int32 NativePicture::GetHeight() const
{
    Validate();
    return mHeight;
}

NativePicture* NativePicture::CreateFromStream(
    /* [in] */ SkStream* stream)
{
    NativePicture* newPict = new NativePicture;

    sk_sp<SkPicture> skPicture = SkPicture::MakeFromStream(stream);
    if (NULL != skPicture) {
        newPict->mPicture = skPicture;

        const SkIRect cullRect = skPicture->cullRect().roundOut();
        newPict->mWidth = cullRect.width();
        newPict->mHeight = cullRect.height();
    }

    return newPict;
}

void NativePicture::Serialize(
    /* [in] */ SkWStream* stream) const
{
    if (NULL != mRecorder.get()) {
        MakePartialCopy()->serialize(stream);
    }
    else if (NULL != mPicture.get()) {
        Validate();
        mPicture->serialize(stream);
    }
    else {
        // serialize "empty" picture
        SkPictureRecorder recorder;
        recorder.beginRecording(0, 0);
        recorder.finishRecordingAsPicture()->serialize(stream);
    }
}

void NativePicture::Draw(
    /* [in] */ android::Canvas* canvas)
{
    if (NULL != mRecorder.get()) {
        EndRecording();
        SkASSERT(NULL != mPicture.get());
    }
    Validate();
    if (NULL != mPicture.get()) {
        mPicture->playback(canvas->asSkCanvas());
    }
}

sk_sp<SkPicture> NativePicture::MakePartialCopy() const
{
    SkASSERT(NULL != mRecorder.get());

    SkPictureRecorder reRecorder;

    SkCanvas* canvas = reRecorder.beginRecording(mWidth, mHeight, NULL, 0);
    mRecorder->partialReplay(canvas);
    return reRecorder.finishRecordingAsPicture();
}

void NativePicture::Validate() const
{
#ifdef SK_DEBUG
    if (NULL != mPicture.get()) {
        SkRect cullRect = mPicture->cullRect();
        SkRect myRect = SkRect::MakeWH(SkIntToScalar(mWidth), SkIntToScalar(mHeight));
        SkASSERT(cullRect == myRect);
    }
#endif
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
