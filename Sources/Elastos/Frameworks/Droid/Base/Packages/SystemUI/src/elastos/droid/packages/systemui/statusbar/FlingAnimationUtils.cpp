
#include "elastos/droid/packages/systemui/statusbar/FlingAnimationUtils.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include <elastos/core/Math.h>
#include <elastos/droid/R.h>

using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::R;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::CPathInterpolator;
using Elastos::Droid::View::Animation::EIID_IInterpolator;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::IPathInterpolator;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {

CAR_INTERFACE_IMPL_2(FlingAnimationUtils::InterpolatorInterpolator, Object, IInterpolator, ITimeInterpolator);
FlingAnimationUtils::InterpolatorInterpolator::InterpolatorInterpolator(
    /* [in] */ IInterpolator* interpolator1,
    /* [in] */ IInterpolator* interpolator2,
    /* [in] */ IInterpolator* crossfader)
    : mInterpolator1(interpolator1)
    , mInterpolator2(interpolator2)
    , mCrossfader(crossfader)
{
}

ECode FlingAnimationUtils::InterpolatorInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    Float t = 0;
    ITimeInterpolator::Probe(mCrossfader)->GetInterpolation(input, &t);
    Float t1 = 0, t2 = 0;
    ITimeInterpolator::Probe(mInterpolator1)->GetInterpolation(input, &t1);
    ITimeInterpolator::Probe(mInterpolator2)->GetInterpolation(input, &t2);
    *result = (1 - t) * t1 + t * t2;
    return NOERROR;
}

ECode FlingAnimationUtils::InterpolatorInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}


CAR_INTERFACE_IMPL_2(FlingAnimationUtils::VelocityInterpolator, Object, IInterpolator, ITimeInterpolator);
FlingAnimationUtils::VelocityInterpolator::VelocityInterpolator(
    /* [in] */ Float durationSeconds,
    /* [in] */ Float velocity,
    /* [in] */ Float diff)
    : mDurationSeconds(durationSeconds)
    , mVelocity(velocity)
    , mDiff(diff)
{
}

ECode FlingAnimationUtils::VelocityInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    Float time = input * mDurationSeconds;
    *result = time * mVelocity / mDiff;
    return NOERROR;
}

ECode FlingAnimationUtils::VelocityInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

FlingAnimationUtils::AnimatorProperties::AnimatorProperties()
    : mDuration(0)
{}


const Float FlingAnimationUtils::LINEAR_OUT_SLOW_IN_X2 = 0.35f;
const Float FlingAnimationUtils::LINEAR_OUT_FASTER_IN_X2 = 0.5f;
const Float FlingAnimationUtils::LINEAR_OUT_FASTER_IN_Y2_MIN = 0.4f;
const Float FlingAnimationUtils::LINEAR_OUT_FASTER_IN_Y2_MAX = 0.5f;
const Float FlingAnimationUtils::MIN_VELOCITY_DP_PER_SECOND = 250;
const Float FlingAnimationUtils::HIGH_VELOCITY_DP_PER_SECOND = 3000;
const Float FlingAnimationUtils::LINEAR_OUT_SLOW_IN_START_GRADIENT = 1.0f / LINEAR_OUT_SLOW_IN_X2;
FlingAnimationUtils::FlingAnimationUtils(
    /* [in] */ IContext* ctx,
    /* [in] */ Float maxLengthSeconds)
    : mMaxLengthSeconds(maxLengthSeconds)
{
    mAnimatorProperties = new AnimatorProperties();

    CPathInterpolator::New(0, 0, LINEAR_OUT_SLOW_IN_X2, 1, (IInterpolator**)&mLinearOutSlowIn);
    AutoPtr<IAnimationUtils> utils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&utils);
    utils->LoadInterpolator(ctx, R::interpolator::fast_out_slow_in, (IInterpolator**)&mFastOutSlowIn);
    utils->LoadInterpolator(ctx, R::interpolator::fast_out_linear_in, (IInterpolator**)&mFastOutLinearIn);

    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Float density = 0;
    dm->GetDensity(&density);
    mMinVelocityPxPerSecond = MIN_VELOCITY_DP_PER_SECOND * density;
    mHighVelocityPxPerSecond = HIGH_VELOCITY_DP_PER_SECOND * density;
}

void FlingAnimationUtils::Apply(
    /* [in] */ IAnimator* animator,
    /* [in] */ Float currValue,
    /* [in] */ Float endValue,
    /* [in] */ Float velocity)
{
    Apply(animator, currValue, endValue, velocity, Elastos::Core::Math::Abs(endValue - currValue));
}

void FlingAnimationUtils::Apply(
    /* [in] */ IViewPropertyAnimator* animator,
    /* [in] */ Float currValue,
    /* [in] */ Float endValue,
    /* [in] */ Float velocity)
{
    Apply(animator, currValue, endValue, velocity, Elastos::Core::Math::Abs(endValue - currValue));
}

void FlingAnimationUtils::Apply(
    /* [in] */ IAnimator* animator,
    /* [in] */ Float currValue,
    /* [in] */ Float endValue,
    /* [in] */ Float velocity,
    /* [in] */ Float maxDistance)
{
    AutoPtr<AnimatorProperties> properties = GetProperties(currValue, endValue, velocity,
            maxDistance);
    animator->SetDuration(properties->mDuration);
    animator->SetInterpolator(ITimeInterpolator::Probe(properties->mInterpolator));
}

void FlingAnimationUtils::Apply(
    /* [in] */ IViewPropertyAnimator* animator,
    /* [in] */ Float currValue,
    /* [in] */ Float endValue,
    /* [in] */ Float velocity,
    /* [in] */ Float maxDistance)
{
    AutoPtr<AnimatorProperties> properties = GetProperties(currValue, endValue, velocity,
            maxDistance);
    animator->SetDuration(properties->mDuration);
    animator->SetInterpolator(ITimeInterpolator::Probe(properties->mInterpolator));
}

AutoPtr<FlingAnimationUtils::AnimatorProperties> FlingAnimationUtils::GetProperties(
    /* [in] */ Float currValue,
    /* [in] */ Float endValue,
    /* [in] */ Float velocity,
    /* [in] */ Float maxDistance)
{
    using Elastos::Core::Math;

    Float maxLengthSeconds = (Float) (mMaxLengthSeconds
            * Math::Sqrt(Math::Abs(endValue - currValue) / maxDistance));
    Float diff = Math::Abs(endValue - currValue);
    Float velAbs = Math::Abs(velocity);
    Float durationSeconds = LINEAR_OUT_SLOW_IN_START_GRADIENT * diff / velAbs;
    if (durationSeconds <= maxLengthSeconds) {
        mAnimatorProperties->mInterpolator = mLinearOutSlowIn;
    }
    else if (velAbs >= mMinVelocityPxPerSecond) {

        // Cross fade between fast-out-slow-in and linear interpolator with current velocity.
        durationSeconds = maxLengthSeconds;
        AutoPtr<VelocityInterpolator> velocityInterpolator
                = new VelocityInterpolator(durationSeconds, velAbs, diff);
        AutoPtr<InterpolatorInterpolator> superInterpolator = new InterpolatorInterpolator(
                velocityInterpolator, mLinearOutSlowIn, mLinearOutSlowIn);
        mAnimatorProperties->mInterpolator = superInterpolator;
    }
    else {

        // Just use a normal interpolator which doesn't take the velocity into account.
        durationSeconds = maxLengthSeconds;
        mAnimatorProperties->mInterpolator = mFastOutSlowIn;
    }
    mAnimatorProperties->mDuration = (Int64) (durationSeconds * 1000);
    return mAnimatorProperties;
}

void FlingAnimationUtils::ApplyDismissing(
    /* [in] */ IAnimator* animator,
    /* [in] */ Float currValue,
    /* [in] */ Float endValue,
    /* [in] */ Float velocity,
    /* [in] */ Float maxDistance)
{
    AutoPtr<AnimatorProperties> properties = GetDismissingProperties(currValue, endValue, velocity,
            maxDistance);
    animator->SetDuration(properties->mDuration);
    animator->SetInterpolator(ITimeInterpolator::Probe(properties->mInterpolator));
}

void FlingAnimationUtils::ApplyDismissing(
    /* [in] */ IViewPropertyAnimator* animator,
    /* [in] */ Float currValue,
    /* [in] */ Float endValue,
    /* [in] */ Float velocity,
    /* [in] */ Float maxDistance)
{
    AutoPtr<AnimatorProperties> properties = GetDismissingProperties(currValue, endValue, velocity,
            maxDistance);
    animator->SetDuration(properties->mDuration);
    animator->SetInterpolator(ITimeInterpolator::Probe(properties->mInterpolator));
}

AutoPtr<FlingAnimationUtils::AnimatorProperties> FlingAnimationUtils::GetDismissingProperties(
    /* [in] */ Float currValue,
    /* [in] */ Float endValue,
    /* [in] */ Float velocity,
    /* [in] */ Float maxDistance)
{
    using Elastos::Core::Math;

    Float maxLengthSeconds = (Float) (mMaxLengthSeconds
            * Math::Pow(Math::Abs(endValue - currValue) / maxDistance, 0.5f));
    Float diff = Math::Abs(endValue - currValue);
    Float velAbs = Math::Abs(velocity);
    Float y2 = CalculateLinearOutFasterInY2(velAbs);

    Float startGradient = y2 / LINEAR_OUT_FASTER_IN_X2;
    AutoPtr<IInterpolator> mLinearOutFasterIn;
    CPathInterpolator::New(0, 0, LINEAR_OUT_FASTER_IN_X2, y2, (IInterpolator**)&mLinearOutFasterIn);
    Float durationSeconds = startGradient * diff / velAbs;
    if (durationSeconds <= maxLengthSeconds) {
        mAnimatorProperties->mInterpolator = mLinearOutFasterIn;
    }
    else if (velAbs >= mMinVelocityPxPerSecond) {

        // Cross fade between linear-out-faster-in and linear interpolator with current
        // velocity.
        durationSeconds = maxLengthSeconds;
        AutoPtr<VelocityInterpolator> velocityInterpolator
                = new VelocityInterpolator(durationSeconds, velAbs, diff);
        AutoPtr<InterpolatorInterpolator> superInterpolator = new InterpolatorInterpolator(
                velocityInterpolator, mLinearOutFasterIn, mLinearOutSlowIn);
        mAnimatorProperties->mInterpolator = superInterpolator;
    }
    else {
        // Just use a normal interpolator which doesn't take the velocity into account.
        durationSeconds = maxLengthSeconds;
        mAnimatorProperties->mInterpolator = mFastOutLinearIn;
    }
    mAnimatorProperties->mDuration = (Int64) (durationSeconds * 1000);
    return mAnimatorProperties;
}

Float FlingAnimationUtils::CalculateLinearOutFasterInY2(
    /* [in] */ Float velocity)
{
    Float t = (velocity - mMinVelocityPxPerSecond)
            / (mHighVelocityPxPerSecond - mMinVelocityPxPerSecond);
    t = Elastos::Core::Math::Max(0, Elastos::Core::Math::Min(1, (Int32)t));
    return (1 - t) * LINEAR_OUT_FASTER_IN_Y2_MIN + t * LINEAR_OUT_FASTER_IN_Y2_MAX;
}

Float FlingAnimationUtils::GetMinVelocityPxPerSecond()
{
    return mMinVelocityPxPerSecond;
}

} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos
