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

#include "elastos/droid/animation/CValueAnimatorHelper.h"
#include "elastos/droid/animation/CValueAnimator.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_SINGLETON_IMPL(CValueAnimatorHelper)
CAR_INTERFACE_IMPL(CValueAnimatorHelper, Singleton, IValueAnimatorHelper)

ECode CValueAnimatorHelper::SetDurationScale(
    /* [in] */ Float durationScale)
{
    CValueAnimator::SetDurationScale(durationScale);
    return NOERROR;
}

ECode CValueAnimatorHelper::GetDurationScale(
    /* [out] */ Float* durationScale)
{
    VALIDATE_NOT_NULL(durationScale);
    *durationScale = CValueAnimator::GetDurationScale();
    return NOERROR;
}

ECode CValueAnimatorHelper::OfInt32(
    /* [in] */ ArrayOf<Int32>* values,
    /* [out] */ IValueAnimator** obj)
{
    VALIDATE_NOT_NULL(obj);
    AutoPtr<IValueAnimator> tmp = CValueAnimator::OfInt32(values);
    *obj = tmp;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CValueAnimatorHelper::OfFloat(
    /* [in] */ ArrayOf<Float>* values,
    /* [out] */ IValueAnimator** obj)
{
    VALIDATE_NOT_NULL(obj);
    AutoPtr<IValueAnimator> tmp = CValueAnimator::OfFloat(values);
    *obj = tmp;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CValueAnimatorHelper::OfPropertyValuesHolder(
    /* [in] */ ArrayOf<IPropertyValuesHolder*>* values,
    /* [out] */ IValueAnimator** obj)
{
    VALIDATE_NOT_NULL(obj);
    AutoPtr<IValueAnimator> tmp = CValueAnimator::OfPropertyValuesHolder(values);
    *obj = tmp;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CValueAnimatorHelper::OfObject(
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values,
    /* [out] */ IValueAnimator** obj)
{
    VALIDATE_NOT_NULL(obj);
    AutoPtr<IValueAnimator> tmp = CValueAnimator::OfObject(evaluator, values);
    *obj = tmp;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CValueAnimatorHelper::GetFrameDelay(
    /* [out] */ Int64* frameDelay)
{
    VALIDATE_NOT_NULL(frameDelay);
    *frameDelay = CValueAnimator::GetFrameDelay();
    return NOERROR;
}

ECode CValueAnimatorHelper::SetFrameDelay(
    /* [in] */ Int64 frameDelay)
{
    CValueAnimator::SetFrameDelay(frameDelay);
    return NOERROR;
}

ECode CValueAnimatorHelper::GetCurrentAnimationsCount(
    /* [out] */ Int32* animationsCount)
{
    VALIDATE_NOT_NULL(animationsCount);
    *animationsCount = CValueAnimator::GetCurrentAnimationsCount();
    return NOERROR;
}

ECode CValueAnimatorHelper::ClearAllAnimations()
{
    CValueAnimator::ClearAllAnimations();
    return NOERROR;
}

} // namespace Animation
} // namepsace Droid
} // namespace Elastos
