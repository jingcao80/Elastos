
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/animation/LayoutAnimationController.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/view/animation/Animation.h"
#include "elastos/droid/view/animation/CLinearInterpolator.h"
#include "elastos/droid/view/CViewGroupLayoutParams.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <stdlib.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::R;
using Elastos::Core::ICloneable;
using Elastos::Utility::CRandom;

namespace Elastos {
namespace Droid {
namespace View {
namespace Animation {

/* LayoutAnimationController::AnimationParameters */
CAR_INTERFACE_IMPL(LayoutAnimationController::AnimationParameters, Object, IAnimationParameters);

LayoutAnimationController::AnimationParameters::AnimationParameters()
    : mCount(0)
    , mIndex(0)
{}

LayoutAnimationController::AnimationParameters::~AnimationParameters()
{}

ECode LayoutAnimationController::AnimationParameters::constructor()
{
    return NOERROR;
}

ECode LayoutAnimationController::AnimationParameters::GetCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);
    *count = mCount;
    return NOERROR;
}

ECode LayoutAnimationController::AnimationParameters::SetCount(
    /* [in] */ Int32 count)
{
    mCount = count;
    return NOERROR;
}

ECode LayoutAnimationController::AnimationParameters::GetIndex(
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index);
    *index = mIndex;
    return NOERROR;
}

ECode LayoutAnimationController::AnimationParameters::SetIndex(
    /* [in] */ Int32 index)
{
    mIndex = index;
    return NOERROR;
}

/* LayoutAnimationController */
CAR_INTERFACE_IMPL(LayoutAnimationController, Object, ILayoutAnimationController);

LayoutAnimationController::LayoutAnimationController()
    : mDelay(0.f)
    , mOrder(0)
    , mDuration(0ll)
    , mMaxDelay(0ll)
{}

LayoutAnimationController::~LayoutAnimationController()
{}

ECode LayoutAnimationController::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::LayoutAnimation);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&a);

    AutoPtr<ITypedValue> value;
    a->PeekValue(R::styleable::LayoutAnimation_delay, (ITypedValue**)&value);

    AutoPtr<Animation::Description> d = Animation::Description::ParseValue(value);
    mDelay = d->mValue;

    a->GetInt32(R::styleable::LayoutAnimation_animationOrder,
            ILayoutAnimationController::ORDER_NORMAL, &mOrder);

    Int32 resource;
    a->GetResourceId(R::styleable::LayoutAnimation_animation, 0, &resource);
    if (resource > 0) {
        SetAnimation(context, resource);
    }

    a->GetResourceId(R::styleable::LayoutAnimation_interpolator, 0, &resource);
    if (resource > 0) {
        SetInterpolator(context, resource);
    }

    a->Recycle();

    return NOERROR;
}

ECode LayoutAnimationController::constructor(
    /* [in] */ IAnimation* animation)
{
    return constructor(animation, 0.5f);
}

ECode LayoutAnimationController::constructor(
    /* [in] */ IAnimation* animation,
    /* [in] */ Float delay)
{
    mDelay = delay;
    return SetAnimation(animation);
}

ECode LayoutAnimationController::GetOrder(
    /* [out] */ Int32* order)
{
    VALIDATE_NOT_NULL(order);
    *order = mOrder;
    return NOERROR;
}

ECode LayoutAnimationController::SetOrder(
    /* [in] */ Int32 order)
{
    mOrder = order;
    return NOERROR;
}

ECode LayoutAnimationController::SetAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceID)
{
    AutoPtr<IAnimation> animation;
    FAIL_RETURN(AnimationUtils::LoadAnimation(
            context, resourceID, (IAnimation**)&animation));
    return SetAnimation(animation);
}

ECode LayoutAnimationController::SetAnimation(
    /* [in] */ IAnimation* animation)
{
    mAnimation = animation;
    mAnimation->SetFillBefore(TRUE);
    return NOERROR;
}

ECode LayoutAnimationController::GetAnimation(
    /* [out] */ IAnimation** animation)
{
    VALIDATE_NOT_NULL(animation);
    *animation = mAnimation;
    REFCOUNT_ADD(*animation);
    return NOERROR;
}

ECode LayoutAnimationController::SetInterpolator(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resourceID)
{
    AutoPtr<IInterpolator> interpolator;
    FAIL_RETURN(AnimationUtils::LoadInterpolator(
            context, resourceID, (IInterpolator**)&interpolator));
    return SetInterpolator(interpolator);
}

ECode LayoutAnimationController::SetInterpolator(
    /* [in] */ IInterpolator* interpolator)
{
    mInterpolator = interpolator;
    return NOERROR;
}

ECode LayoutAnimationController::GetInterpolator(
    /* [out] */ IInterpolator** interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = mInterpolator;
    REFCOUNT_ADD(*interpolator);
    return NOERROR;
}

ECode LayoutAnimationController::GetDelay(
    /* [out] */ Float* delay)
{
    VALIDATE_NOT_NULL(delay);
    *delay = mDelay;
    return NOERROR;
}

ECode LayoutAnimationController::SetDelay(
    /* [in] */ Float delay)
{
    mDelay = delay;
    return NOERROR;
}

ECode LayoutAnimationController::WillOverlap(
    /* [out] */ Boolean* willOverlap)
{
    VALIDATE_NOT_NULL(willOverlap);
    *willOverlap = mDelay < 1.0f;
    return NOERROR;
}

ECode LayoutAnimationController::Start()
{
    mAnimation->GetDuration(&mDuration);
    mMaxDelay = Elastos::Core::Math::INT64_MIN_VALUE;
    mAnimation->SetStartTime(-1);

    return NOERROR;
}

ECode LayoutAnimationController::GetAnimationForView(
    /* [in] */ IView* view,
    /* [out] */ IAnimation** animation)
{
    VALIDATE_NOT_NULL(animation);
    Int64 startOffset;
    mAnimation->GetStartOffset(&startOffset);
    Int64 delay = GetDelayForView(view) + startOffset;
    mMaxDelay = Elastos::Core::Math::Max(mMaxDelay, delay);

    AutoPtr<IInterface> obj;
    ICloneable::Probe(mAnimation)->Clone((IInterface**)&obj);
    AutoPtr<IAnimation> anim = IAnimation::Probe(obj);
    anim->SetStartOffset(delay);
    *animation = anim;
    REFCOUNT_ADD(*animation);

    return NOERROR;
}

ECode LayoutAnimationController::IsDone(
    /* [out] */ Boolean* done)
{
    VALIDATE_NOT_NULL(done);
    Int64 startTime;
    mAnimation->GetStartTime(&startTime);
    Int64 time;
    *done = (AnimationUtils::CurrentAnimationTimeMillis(&time), time) >
            startTime + mMaxDelay + mDuration;
    return NOERROR;
}

Int64 LayoutAnimationController::GetDelayForView(
    /* [in] */ IView* view)
{
    AutoPtr<IViewGroupLayoutParams> lp;
    view->GetLayoutParams((IViewGroupLayoutParams**)&lp);
    AutoPtr<IAnimationParameters> basic;
    lp->GetLayoutAnimationParameters((IAnimationParameters**)&basic);
    AutoPtr<AnimationParameters> params =
        (AnimationParameters*)IAnimationParameters::Probe(basic);
    if (params == NULL) {
        return 0;
    }

    Int64 duration;
    mAnimation->GetDuration(&duration);
    Float delay = mDelay * duration;
    Int64 viewDelay = (Int64)(GetTransformedIndex(params) * delay);
    Float totalDelay = delay * params->mCount;

    if (mInterpolator == NULL) {
        ASSERT_SUCCEEDED(CLinearInterpolator::New((IInterpolator**)&mInterpolator));
    }

    Float normalizedDelay = viewDelay / totalDelay;
    ITimeInterpolator::Probe(mInterpolator)->GetInterpolation(normalizedDelay, &normalizedDelay);

    return (Int64)(normalizedDelay * totalDelay);
}

Int32 LayoutAnimationController::GetTransformedIndex(
    /* [in] */ AnimationParameters* params)
{
    Int32 order;
    GetOrder(&order);
    switch(order) {
        case ILayoutAnimationController::ORDER_REVERSE:
            return params->mCount - 1 - params->mIndex;
        case ILayoutAnimationController::ORDER_RANDOM: {
            if (mRandomizer == NULL) {
                CRandom::New((IRandom**)&mRandomizer);
            }
            Float value;
            mRandomizer->NextFloat(&value);
            return (Int32) (params->mCount * value);
        }
        case ILayoutAnimationController::ORDER_NORMAL:
        default:
            return params->mIndex;
    }
}

}   //namespace Animation
}   //namespace View
}   //namespace Droid
}   //namespace Elastos
