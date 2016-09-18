
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/widget/Scroller.h"
#include "elastos/droid/view/ViewConfiguration.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/utility/FloatMath.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::View::ViewConfiguration;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::EIID_IInterpolator;
using Elastos::Droid::Animation::EIID_ITimeInterpolator;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::FloatMath;

namespace Elastos {
namespace Droid {
namespace Widget {

static Float InitDECELERATION_RATE()
{
    return (Float) (Elastos::Core::Math::Log(0.78) / Elastos::Core::Math::Log(0.9));
}

static AutoPtr<ArrayOf<Float> > InitSPLINE_POSITION()
{
    const Int32 NB_SAMPLES = 100;
    const Float START_TENSION = 0.5f;
    const Float P1 = 0.5f * 0.35f;
    const Float P2 = 1.0f - 1.0f * (1.0f - 0.35f);

    AutoPtr<ArrayOf<Float> > positions = ArrayOf<Float>::Alloc(NB_SAMPLES + 1);

    Float x_min = 0.0f;
    for (Int32 i = 0; i < NB_SAMPLES; i++) {
        Float alpha = (Float) i / NB_SAMPLES;

        Float x_max = 1.0f;
        Float x, tx, coef;
        while (TRUE) {
            x = x_min + (x_max - x_min) / 2.0f;
            coef = 3.0f * x * (1.0f - x);
            tx = coef * ((1.0f - x) * P1 + x * P2) + x * x * x;
            if (Elastos::Core::Math::Abs(tx - alpha) < 1E-5) break;
            if (tx > alpha) x_max = x;
            else x_min = x;
        }
        (*positions)[i] = coef * ((1.0f - x) * START_TENSION + x) + x * x * x;
    }
    (*positions)[NB_SAMPLES] = 1.0f;
    return positions;
}

static AutoPtr<ArrayOf<Float> > InitSPLINE_TIME()
{
    const Int32 NB_SAMPLES = 100;
    const Float START_TENSION = 0.5f;
    const Float P1 = 0.5f * 0.35f;
    const Float P2 = 1.0f - 1.0f * (1.0f - 0.35f);

    AutoPtr<ArrayOf<Float> > times = ArrayOf<Float>::Alloc(NB_SAMPLES + 1);
    Float y_min = 0.0f;
    for (Int32 i = 0; i < NB_SAMPLES; i++) {
        const Float alpha = (Float) i / NB_SAMPLES;

        Float y_max = 1.0f;
        Float y, dy, coef;
        while (TRUE) {
            y = y_min + (y_max - y_min) / 2.0f;
            coef = 3.0f * y * (1.0f - y);
            dy = coef * ((1.0f - y) * START_TENSION + y) + y * y * y;
            if (Elastos::Core::Math::Abs(dy - alpha) < 1E-5) break;
            if (dy > alpha) y_max = y;
            else y_min = y;
        }
        (*times)[i] = coef * ((1.0f - y) * P1 + y * P2) + y * y * y;
    }
    (*times)[NB_SAMPLES] = 1.0f;
    return times;
}

const Float Scroller::ViscousFluidInterpolator::VISCOUS_FLUID_SCALE = 8.0f;
// must be set to 1.0 (used in viscousFluid())
const Float Scroller::ViscousFluidInterpolator::VISCOUS_FLUID_NORMALIZE
        = 1.0f / Scroller::ViscousFluidInterpolator::ViscousFluid(1.0f);
// account for very small floating-point error
const Float Scroller::ViscousFluidInterpolator::VISCOUS_FLUID_OFFSET
        = 1.0f - VISCOUS_FLUID_NORMALIZE * Scroller::ViscousFluidInterpolator::ViscousFluid(1.0f);

CAR_INTERFACE_IMPL_2(Scroller::ViscousFluidInterpolator, Object, IInterpolator, ITimeInterpolator);

Float Scroller::ViscousFluidInterpolator::ViscousFluid(
    /* [in] */ Float x)
{
    x *= VISCOUS_FLUID_SCALE;
    if (x < 1.0f) {
        x -= (1.0f - (Float)Elastos::Core::Math::Exp(-x));
    } else {
        Float start = 0.36787944117f;   // 1/e == exp(-1)
        x = 1.0f - (Float)Elastos::Core::Math::Exp(1.0f - x);
        x = start + x * (1.0f - start);
    }
    return x;
}

ECode Scroller::ViscousFluidInterpolator::GetInterpolation(
    /* [in] */ Float input,
    /* [out] */ Float* interpolation)
{
    VALIDATE_NOT_NULL(interpolation);
    Float interpolated = VISCOUS_FLUID_NORMALIZE * ViscousFluid(input);
    if (interpolated > 0) {
        *interpolation = interpolated + VISCOUS_FLUID_OFFSET;
        return NOERROR;
    }
    *interpolation = interpolated;
    return NOERROR;
}

ECode Scroller::ViscousFluidInterpolator::HasNativeInterpolator(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = FALSE;
    return NOERROR;
}

const Int32 Scroller::DEFAULT_DURATION  = 250;
const Int32 Scroller::SCROLL_MODE = 0;
const Int32 Scroller::FLING_MODE = 1;

Float Scroller::DECELERATION_RATE = InitDECELERATION_RATE();
const Float Scroller::INFLEXION = 0.35f;
const Float Scroller::START_TENSION = 0.5f;
const Float Scroller::END_TENSION = 1.0f;
const Float Scroller::P1 = 0.5f * 0.35f; //START_TENSION * INFLEXION;
const Float Scroller::P2 = 1.0f - 1.0f * (1.0f - 0.35f); //1.0f - END_TENSION * (1.0f - INFLEXION);

const Int32 Scroller::NB_SAMPLES = 100;

AutoPtr<ArrayOf<Float> > Scroller::SPLINE_POSITION = InitSPLINE_POSITION();
AutoPtr<ArrayOf<Float> > Scroller::SPLINE_TIME = InitSPLINE_TIME();

const Float GRAVITY_EARTH = 9.80665f;   // g (m/s^2) SensorManager.GRAVITY_EARTH

CAR_INTERFACE_IMPL(Scroller, Object, IScroller);
Scroller::Scroller()
    : mMode(0)
    , mStartX(0)
    , mStartY(0)
    , mFinalX(0)
    , mFinalY(0)
    , mMinX(0)
    , mMaxX(0)
    , mMinY(0)
    , mMaxY(0)
    , mCurrX(0)
    , mCurrY(0)
    , mStartTime(0)
    , mDuration(0)
    , mDurationReciprocal(0)
    , mDeltaX(0)
    , mDeltaY(0)
    , mFinished(FALSE)
    , mFlywheel(FALSE)
    , mVelocity(0)
    , mCurrVelocity(0)
    , mDistance(0)
    , mPpi(0)
    , mPhysicalCoeff(0)
{
}

ECode Scroller::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode Scroller::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IInterpolator* interpolator)
{
    AutoPtr<IApplicationInfo> info;
    context->GetApplicationInfo((IApplicationInfo**)&info);
    Int32 targetSdkVersion = 0;
    info->GetTargetSdkVersion(&targetSdkVersion);
    return constructor(context, interpolator, targetSdkVersion >= Build::VERSION_CODES::HONEYCOMB);
}

ECode Scroller::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IInterpolator* interpolator,
    /* [in] */ Boolean flywheel)
{
    mFlingFriction = ViewConfiguration::GetScrollFriction();

    mFinished = TRUE;
    if (interpolator == NULL) {
        mInterpolator = new ViscousFluidInterpolator();
    } else {
        mInterpolator = interpolator;
    }

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);

    mPpi = density * 160.0f;
    mDeceleration = ComputeDeceleration(mFlingFriction);
    mFlywheel = flywheel;

    mPhysicalCoeff = ComputeDeceleration(0.84f); // look and feel tuning

    AutoPtr<IInterface> pmTmp;
    context->GetSystemService(IContext::POWER_SERVICE, (IInterface**)&pmTmp);
    mPm = IPowerManager::Probe(pmTmp);

    return NOERROR;
}

ECode Scroller::SetFriction(
    /* [in] */ Float friction)
{
    mDeceleration = ComputeDeceleration(friction);
    mFlingFriction = friction;
    return NOERROR;
}

Float Scroller::ComputeDeceleration(
    /* [in] */ Float friction)
{
    return GRAVITY_EARTH //SensorManager.GRAVITY_EARTH   // g (m/s^2)
                  * 39.37f               // inch/meter
                  * mPpi                 // pixels per inch
                  * friction;
}

ECode Scroller::IsFinished(
    /* [out] */ Boolean* finished)
{
    VALIDATE_NOT_NULL(finished);
    *finished = mFinished;
    return NOERROR;
}

ECode Scroller::ForceFinished(
    /* [in] */ Boolean finished)
{
    mFinished = finished;
    return NOERROR;
}

ECode Scroller::GetDuration(
    /* [out] */ Int32* duration)
{
    VALIDATE_NOT_NULL(duration);
    *duration = mDuration;
    return NOERROR;
}

ECode Scroller::GetCurrX(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCurrX;
    return NOERROR;
}

ECode Scroller::GetCurrY(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCurrY;
    return NOERROR;
}

ECode Scroller::GetCurrVelocity(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 passed = 0;
    TimePassed(&passed);
    *result = mMode == FLING_MODE ?
            mCurrVelocity : mVelocity - mDeceleration * passed / 2000.0f;
    return NOERROR;
}

ECode Scroller::GetStartX(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStartX;
    return NOERROR;
}

ECode Scroller::GetStartY(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStartY;
    return NOERROR;
}

ECode Scroller::GetFinalX(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFinalX;
    return NOERROR;
}

ECode Scroller::GetFinalY(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFinalY;
    return NOERROR;
}

ECode Scroller::ComputeScrollOffset(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (mFinished) {
        return NOERROR;
    }

    Int32 timePassed = (Int32)(SystemClock::GetUptimeMillis() - mStartTime);

    if (timePassed < mDuration) {
        switch (mMode) {
        case SCROLL_MODE:
            {
                Float x = 0;
                ITimeInterpolator::Probe(mInterpolator)->GetInterpolation(timePassed * mDurationReciprocal, &x);
                mCurrX = mStartX + Elastos::Core::Math::Round(x * mDeltaX );
                mCurrY = mStartY + Elastos::Core::Math::Round(x * mDeltaY);
            }
            break;
        case FLING_MODE:
            {
                Float t = (Float) timePassed / mDuration;
                Int32 index = (Int32) (NB_SAMPLES * t);
                Float distanceCoef = 1.f;
                Float velocityCoef = 0.f;
                if (index < NB_SAMPLES) {
                    Float t_inf = (Float) index / NB_SAMPLES;
                    Float t_sup = (Float) (index + 1) / NB_SAMPLES;
                    Float d_inf = (*SPLINE_POSITION)[index];
                    Float d_sup = (*SPLINE_POSITION)[index + 1];
                    velocityCoef = (d_sup - d_inf) / (t_sup - t_inf);
                    distanceCoef = d_inf + (t - t_inf) * velocityCoef;
                }

                mCurrVelocity = velocityCoef * mDistance / mDuration * 1000.0f;

                mCurrX = mStartX + Elastos::Core::Math::Round(distanceCoef * (mFinalX - mStartX));
                // Pin to mMinX <= mCurrX <= mMaxX
                mCurrX = Elastos::Core::Math::Min(mCurrX, mMaxX);
                mCurrX = Elastos::Core::Math::Max(mCurrX, mMinX);

                mCurrY = mStartY + Elastos::Core::Math::Round(distanceCoef * (mFinalY - mStartY));
                // Pin to mMinY <= mCurrY <= mMaxY
                mCurrY = Elastos::Core::Math::Min(mCurrY, mMaxY);
                mCurrY = Elastos::Core::Math::Max(mCurrY, mMinY);

                if (mCurrX == mFinalX && mCurrY == mFinalY) {
                    mFinished = TRUE;
                }
            }
            break;
        default:
            break;
        }
    }
    else {
        mCurrX = mFinalX;
        mCurrY = mFinalY;
        mFinished = TRUE;
    }

    *result = TRUE;
    return NOERROR;
}

ECode Scroller::StartScroll(
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy)
{
    return StartScroll(startX, startY, dx, dy, DEFAULT_DURATION);
}

ECode Scroller::StartScroll(
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 dx,
    /* [in] */ Int32 dy,
    /* [in] */ Int32 duration)
{
    mMode = SCROLL_MODE;
    mFinished = FALSE;
    mDuration = duration;
    AnimationUtils::CurrentAnimationTimeMillis(&mStartTime);
    mStartX = startX;
    mStartY = startY;
    mFinalX = startX + dx;
    mFinalY = startY + dy;
    mDeltaX = dx;
    mDeltaY = dy;
    mDurationReciprocal = 1.0f / (Float) mDuration;
    mPm->CpuBoost(duration * 1000);

    return NOERROR;
}

ECode Scroller::Fling(
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 velocityX,
    /* [in] */ Int32 velocityY,
    /* [in] */ Int32 minX,
    /* [in] */ Int32 maxX,
    /* [in] */ Int32 minY,
    /* [in] */ Int32 maxY)
{
    // Continue a scroll or fling in progress
    if (mFlywheel && !mFinished) {
        Float oldVel = 0;
        GetCurrVelocity(&oldVel);

        Float dx = (Float) (mFinalX - mStartX);
        Float dy = (Float) (mFinalY - mStartY);
        Float hyp = FloatMath::Sqrt(dx * dx + dy * dy);

        Float ndx = dx / hyp;
        Float ndy = dy / hyp;

        Float oldVelocityX = ndx * oldVel;
        Float oldVelocityY = ndy * oldVel;
        if (Elastos::Core::Math::Signum(velocityX) == Elastos::Core::Math::Signum(oldVelocityX) &&
                Elastos::Core::Math::Signum(velocityY) == Elastos::Core::Math::Signum(oldVelocityY)) {
            velocityX += oldVelocityX;
            velocityY += oldVelocityY;
        }
    }

    mMode = FLING_MODE;
    mFinished = FALSE;

    Float velocity = FloatMath::Sqrt(velocityX * velocityX + velocityY * velocityY);

    mVelocity = velocity;
    mDuration = GetSplineFlingDuration(velocity);
    AnimationUtils::CurrentAnimationTimeMillis(&mStartTime);
    mStartX = startX;
    mStartY = startY;

    Float coeffX = velocity == 0 ? 1.0f : velocityX / velocity;
    Float coeffY = velocity == 0 ? 1.0f : velocityY / velocity;

    Double totalDistance = GetSplineFlingDistance(velocity);
    mDistance = (Int32) (totalDistance * Elastos::Core::Math::Signum(velocity));

    mMinX = minX;
    mMaxX = maxX;
    mMinY = minY;
    mMaxY = maxY;

    mFinalX = startX + (Int32) Elastos::Core::Math::Round(totalDistance * coeffX);
    // Pin to mMinX <= mFinalX <= mMaxX
    mFinalX = Elastos::Core::Math::Min(mFinalX, mMaxX);
    mFinalX = Elastos::Core::Math::Max(mFinalX, mMinX);

    mFinalY = startY + (Int32) Elastos::Core::Math::Round(totalDistance * coeffY);
    // Pin to mMinY <= mFinalY <= mMaxY
    mFinalY = Elastos::Core::Math::Min(mFinalY, mMaxY);
    mFinalY = Elastos::Core::Math::Max(mFinalY, mMinY);
    return NOERROR;
}

Double Scroller::GetSplineDeceleration(
    /* [in] */ Float velocity)
{
    return Elastos::Core::Math::Log(INFLEXION * Elastos::Core::Math::Abs(velocity) / (mFlingFriction * mPhysicalCoeff));
}

Int32 Scroller::GetSplineFlingDuration(
    /* [in] */ Float velocity)
{
    Double l = GetSplineDeceleration(velocity);
    Double decelMinusOne = DECELERATION_RATE - 1.0;
    return (Int32) (1000.0 * Elastos::Core::Math::Exp(l / decelMinusOne));
}

Double Scroller::GetSplineFlingDistance(
    /* [in] */ Float velocity)
{
    Double l = GetSplineDeceleration(velocity);
    Double decelMinusOne = DECELERATION_RATE - 1.0;
    return mFlingFriction * mPhysicalCoeff * Elastos::Core::Math::Exp(DECELERATION_RATE / decelMinusOne * l);
}

ECode Scroller::AbortAnimation()
{
    mCurrX = mFinalX;
    mCurrY = mFinalY;
    mFinished = TRUE;
    return NOERROR;
}

ECode Scroller::ExtendDuration(
    /* [in] */ Int32 extend)
{
    Int32 passed = 0;
    TimePassed(&passed);
    mDuration = passed + extend;
    mDurationReciprocal = 1.0f / (Float)mDuration;
    mFinished = FALSE;
    return NOERROR;
}

ECode Scroller::TimePassed(
    /* [out] */ Int32* passed)
{
    VALIDATE_NOT_NULL(passed);
    Int64 time = 0;
    AnimationUtils::CurrentAnimationTimeMillis(&time);
    *passed = (Int32)(time - mStartTime);
    return NOERROR;
}

ECode Scroller::SetFinalX(
    /* [in] */ Int32 newX)
{
    mFinalX = newX;
    mDeltaX = mFinalX - mStartX;
    mFinished = FALSE;
    return NOERROR;
}

ECode Scroller::SetFinalY(
    /* [in] */ Int32 newY)
{
    mFinalY = newY;
    mDeltaY = mFinalY - mStartY;
    mFinished = FALSE;
    return NOERROR;
}

ECode Scroller::IsScrollingInDirection(
    /* [in] */ Float xvel,
    /* [in] */ Float yvel,
    /* [out] */ Boolean* scrolling)
{
    VALIDATE_NOT_NULL(scrolling);
    *scrolling = !mFinished && Elastos::Core::Math::Signum(xvel) == Elastos::Core::Math::Signum(mFinalX - mStartX) &&
            Elastos::Core::Math::Signum(yvel) == Elastos::Core::Math::Signum(mFinalY - mStartY);
    return NOERROR;
}

}// namespace Widget
}// namespace Droid
}// namespace Elastos
