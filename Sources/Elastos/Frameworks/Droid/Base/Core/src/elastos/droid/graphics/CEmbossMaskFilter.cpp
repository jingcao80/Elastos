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
#include "elastos/droid/graphics/CEmbossMaskFilter.h"
#include <skia/effects/SkBlurMaskFilter.h>
#include <skia/effects/SkBlurMask.h>

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CEmbossMaskFilter);
CAR_INTERFACE_IMPL(CEmbossMaskFilter, MaskFilter, IEmbossMaskFilter);
ECode CEmbossMaskFilter::constructor(
    /* [in] */ const ArrayOf<Float>& direction,
    /* [in] */ Float ambient,
    /* [in] */ Float specular,
    /* [in] */ Float blurRadius)
{
    if (direction.GetLength() < 3) {
        // throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    mNativeInstance = NativeConstructor(direction, ambient, specular, blurRadius);
    return NOERROR;
}

Int64 CEmbossMaskFilter::NativeConstructor(
    /* [in] */ const ArrayOf<Float>& dirArray,
    /* [in] */ Float ambient,
    /* [in] */ Float specular,
    /* [in] */ Float blurRadius)
{
    SkScalar direction[3];

    float* values = dirArray.GetPayload();
    for (int i = 0; i < 3; i++) {
        direction[i] = values[i];
    }

    SkScalar sigma = SkBlurMask::ConvertRadiusToSigma(blurRadius);
    SkMaskFilter* filter =  SkBlurMaskFilter::MakeEmboss(sigma,
                direction, ambient, specular).release();
    assert(filter != NULL);
    return reinterpret_cast<Int64>(filter);
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
