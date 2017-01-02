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

#ifndef __Elastos_DevSamples_CameraDemo_CAutoFitTextureView_H__
#define __Elastos_DevSamples_CameraDemo_CAutoFitTextureView_H__

#include "_Elastos_DevSamples_CameraDemo_CAutoFitTextureView.h"
#include <elastos/droid/view/TextureView.h>

using Elastos::Droid::View::TextureView;

namespace Elastos {
namespace DevSamples {
namespace CameraDemo {

CarClass(CAutoFitTextureView)
    , public TextureView
{
public:
    CAR_OBJECT_DECL()

    CAutoFitTextureView();

    /**
     * Sets the aspect ratio for this view. The size of the view will be measured based on the ratio
     * calculated from the parameters. Note that the actual sizes of parameters don't matter, that
     * is, calling setAspectRatio(2, 3) and setAspectRatio(4, 6) make the same result.
     *
     * @param width  Relative horizontal size
     * @param height Relative vertical size
     */
    void SetAspectRatio(
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    Int32 mRatioWidth;
    Int32 mRatioHeight;
};

} // namespace CameraDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_CameraDemo_CAutoFitTextureView_H__
