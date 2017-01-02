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
#include "elastos/droid/graphics/CBlurMaskFilter.h"
#include <elastos/utility/logging/Slogger.h>
#include <skia/effects/SkBlurMask.h>
#include <skia/effects/SkBlurMaskFilter.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Graphics {

CAR_OBJECT_IMPL(CBlurMaskFilter);

CAR_INTERFACE_IMPL(CBlurMaskFilter, MaskFilter, IBlurMaskFilter);

ECode CBlurMaskFilter::constructor(
    /* [in] */ Float radius,
    /* [in] */ Int32 style)
{
    return NativeConstructor(radius, style, &mNativeInstance);
}

ECode CBlurMaskFilter::NativeConstructor(
    /* [in] */ Float radius,
    /* [in] */ Int32 blurStyle,
    /* [out] */ Int64* outfilter)
{
    VALIDATE_NOT_NULL(outfilter)
    *outfilter = 0;

    SkScalar sigma = SkBlurMask::ConvertRadiusToSigma(radius);
    SkMaskFilter* filter = SkBlurMaskFilter::Create((SkBlurStyle)blurStyle, sigma);
    if (filter == NULL) {
        Slogger::E("CBlurMaskFilter", "IllegalArgumentException: radius:%.2f, blurStyle:%d, sigma: %d",
            radius, blurStyle, sigma);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *outfilter = reinterpret_cast<Int64>(filter);
    return NOERROR;
}

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos
