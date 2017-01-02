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

#include "CAutoFitTextureView.h"

namespace Elastos {
namespace DevSamples {
namespace CameraDemo {

static const String TAG("CAutoFitTextureView");

CAR_OBJECT_IMPL(CAutoFitTextureView)

CAutoFitTextureView::CAutoFitTextureView()
    : mRatioWidth(0)
    , mRatioHeight(0)
{}

void CAutoFitTextureView::SetAspectRatio(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    if (width < 0 || height < 0) {
        Logger::E(TAG, "Size cannot be negative.");
        return;
    }
    mRatioWidth = width;
    mRatioHeight = height;
    RequestLayout();
}

ECode CAutoFitTextureView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    TextureView::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    Int32 width = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 height = View::MeasureSpec::GetSize(heightMeasureSpec);
    if (0 == mRatioWidth || 0 == mRatioHeight) {
        SetMeasuredDimension(width, height);
    }
    else {
        if (width < height * mRatioWidth / mRatioHeight) {
            SetMeasuredDimension(width, width * mRatioHeight / mRatioWidth);
        }
        else {
            SetMeasuredDimension(height * mRatioWidth / mRatioHeight, height);
        }
    }
    return NOERROR;
}

} // namespace CameraDemo
} // namespace DevSamples
} // namespace Elastos