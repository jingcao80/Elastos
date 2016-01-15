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
