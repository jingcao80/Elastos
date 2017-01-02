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

#include "elastos/droid/internal/view/animation/FallbackLUTInterpolator.h"
#include "elastos/droid/internal/view/animation/NativeInterpolatorFactoryHelper.h"
#include "elastos/droid/utility/TimeUtils.h"
#include "elastos/droid/view/Choreographer.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Utility::TimeUtils;
using Elastos::Droid::View::Choreographer;
using Elastos::Droid::Animation::EIID_ITimeInterpolator;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {
namespace Animation {

CAR_INTERFACE_IMPL_3(FallbackLUTInterpolator, Object, IFallbackLUTInterpolator, \
    INativeInterpolatorFactory, ITimeInterpolator)

ECode FallbackLUTInterpolator::constructor(
    /* [in] */ ITimeInterpolator* interpolator,
    /* [in] */ Int64 duration)
{
    mSourceInterpolator = interpolator;
    mLut = CreateLUT(interpolator, duration);
    return NOERROR;
}

AutoPtr<ArrayOf<Float> > FallbackLUTInterpolator::CreateLUT(
    /* [in] */ ITimeInterpolator* interpolator,
    /* [in] */ Int64 duration)
{
    Int64 frameIntervalNanos;
    Choreographer::GetInstance()->GetFrameIntervalNanos(&frameIntervalNanos);
    Int32 animIntervalMs = (Int32)(frameIntervalNanos / TimeUtils::NANOS_PER_MS);
    Int32 numAnimFrames = (Int32)Elastos::Core::Math::Ceil(duration / animIntervalMs);
    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(numAnimFrames);
    Float lastFrame = numAnimFrames - 1;
    for (Int32 i = 0; i < numAnimFrames; i++) {
        Float inValue = i / lastFrame;
        Float value;
        interpolator->GetInterpolation(inValue, &value);
        (*values)[i] = value;
    }
    return values;
}

ECode FallbackLUTInterpolator::CreateNativeInterpolator(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NativeInterpolatorFactoryHelper::CreateLutInterpolator(mLut);
    return NOERROR;
}

Int64 FallbackLUTInterpolator::CreateNativeInterpolator(
    /* [in] */ ITimeInterpolator* interpolator,
    /* [in] */ Int64 duration)
{
    AutoPtr<ArrayOf<Float> > lut = CreateLUT(interpolator, duration);
    return NativeInterpolatorFactoryHelper::CreateLutInterpolator(lut);
}

ECode FallbackLUTInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* result)
{
    return mSourceInterpolator->GetInterpolation(input, result);
}

ECode FallbackLUTInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = TRUE;
    return NOERROR;
}

} //namespace Animation
} //namespace View
} //namespace Internal
} //namespace Droid
} //namespace Elastos
