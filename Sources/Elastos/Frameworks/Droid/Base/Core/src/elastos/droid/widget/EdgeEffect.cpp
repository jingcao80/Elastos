
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/EdgeEffect.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CPorterDuffXfermode.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/utility/FloatMath.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/view/animation/CDecelerateInterpolator.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::CPorterDuffXfermode;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IXfermode;
using Elastos::Droid::Utility::FloatMath;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::IInterpolator;

namespace Elastos {
namespace Droid {
namespace Widget {

const String EdgeEffect::TAG("EdgeEffect");
const Int32 EdgeEffect::RECEDE_TIME;
const Int32 EdgeEffect::PULL_TIME;
const Int32 EdgeEffect::PULL_DECAY_TIME;
const Float EdgeEffect::MAX_ALPHA = 0.5f;
const Float EdgeEffect::MAX_GLOW_SCALE = 2.f;
const Float EdgeEffect::PULL_GLOW_BEGIN = 0.f;
const Int32 EdgeEffect::MIN_VELOCITY;
const Int32 EdgeEffect::MAX_VELOCITY;
const Float EdgeEffect::EPSILON = 0.001f;
const Double EdgeEffect::ANGLE = Elastos::Core::Math::PI / 6;
const Float EdgeEffect::SIN = (Float) Elastos::Core::Math::Sin(ANGLE);
const Float EdgeEffect::COS = (Float) Elastos::Core::Math::Cos(ANGLE);
const Int32 EdgeEffect::STATE_IDLE;
const Int32 EdgeEffect::STATE_PULL;
const Int32 EdgeEffect::STATE_ABSORB;
const Int32 EdgeEffect::STATE_RECEDE;
const Int32 EdgeEffect::STATE_PULL_DECAY;
const Float EdgeEffect::PULL_DISTANCE_ALPHA_GLOW_FACTOR = 0.8f;
const Int32 EdgeEffect::VELOCITY_GLOW_FACTOR;

CAR_INTERFACE_IMPL(EdgeEffect, Object, IEdgeEffect);

EdgeEffect::EdgeEffect()
    : mGlowAlpha(0.0f)
    , mGlowScaleY(0.0f)
    , mGlowAlphaStart(0.0f)
    , mGlowAlphaFinish(0.0f)
    , mGlowScaleYStart(0.0f)
    , mGlowScaleYFinish(0.0f)
    , mStartTime(0)
    , mDuration(0.0f)
    , mState(STATE_IDLE)
    , mPullDistance(0.0f)
    , mRadius(0.0f)
    , mBaseGlowScale(1.0f)
    , mDisplacement(0.5f)
    , mTargetDisplacement(0.5f)
{
    ASSERT_SUCCEEDED(CRect::New((IRect**)&mBounds));
    ASSERT_SUCCEEDED(CPaint::New((IPaint**)&mPaint));
}

ECode EdgeEffect::constructor(
    /* [in] */ IContext* context)
{
    mPaint->SetAntiAlias(TRUE);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
            const_cast<Int32 *>(R::styleable::EdgeEffect),
            ArraySize(R::styleable::EdgeEffect));

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrIds, (ITypedArray**)&a);

    Int32 themeColor;
    a->GetColor(R::styleable::EdgeEffect_colorEdgeEffect, 0xff666666, &themeColor);
    a->Recycle();
    mPaint->SetColor((themeColor & 0xffffff) | 0x33000000);
    mPaint->SetStyle(Elastos::Droid::Graphics::PaintStyle_FILL);

    AutoPtr<IXfermode> mode;
    CPorterDuffXfermode::New(Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP, (IXfermode**)&mode);
    mPaint->SetXfermode(mode);

    using Elastos::Droid::View::Animation::IInterpolator;
    ASSERT_SUCCEEDED(CDecelerateInterpolator::New((IInterpolator**)&mInterpolator));

    return NOERROR;
}

ECode EdgeEffect::SetSize(
    /* [in] */ Int32 width,
    /* [in] */ Int32 height)
{
    const Float r = width * 0.75f / SIN;
    const Float y = COS * r;
    const Float h = r - y;
    const Float _or = height * 0.75f / SIN;
    const Float oy = COS * _or;
    const Float oh = _or - oy;

    mRadius = r;
    mBaseGlowScale = h > 0 ? Elastos::Core::Math::Min(oh / h, 1.f) : 1.f;

    Int32 left, top;
    mBounds->GetLeft(&left);
    mBounds->GetTop(&top);
    mBounds->Set(left, top, width, (Int32) Elastos::Core::Math::Min((Float)height, h));
    return NOERROR;
}

ECode EdgeEffect::IsFinished(
    /* [out] */ Boolean* isFinished)
{
    VALIDATE_NOT_NULL(isFinished);
    *isFinished = mState == STATE_IDLE;
    return NOERROR;
}

ECode EdgeEffect::Finish()
{
    mState = STATE_IDLE;
    return NOERROR;
}

ECode EdgeEffect::OnPull(
    /* [in] */ Float deltaDistance)
{
    return OnPull(deltaDistance, 0.5f);
}

ECode EdgeEffect::OnPull(
    /* [in] */ Float deltaDistance,
    /* [in] */ Float displacement)
{
    Int64 now;
    AnimationUtils::CurrentAnimationTimeMillis(&now);
    mTargetDisplacement = displacement;
    if (mState == STATE_PULL_DECAY && now - mStartTime < mDuration) {
        return NOERROR;
    }

    if (mState != STATE_PULL) {
        mGlowScaleY = Elastos::Core::Math::Max(PULL_GLOW_BEGIN, mGlowScaleY);
    }

    mState = STATE_PULL;

    mStartTime = now;
    mDuration = PULL_TIME;

    mPullDistance += deltaDistance;

    const Float absdd = Elastos::Core::Math::Abs(deltaDistance);

    mGlowAlpha = mGlowAlphaStart = Elastos::Core::Math::Min(MAX_ALPHA,
            mGlowAlpha + (absdd * PULL_DISTANCE_ALPHA_GLOW_FACTOR));

    if (mPullDistance == 0) {
        mGlowScaleY = 0;
    }

    if (mPullDistance == 0) {
        mGlowScaleY = mGlowScaleYStart = 0;
    }
    else {
        Float absff = Elastos::Core::Math::Abs(mPullDistance);
        Int32 height;
        mBounds->GetHeight(&height);
        const Float scale = Elastos::Core::Math::Max(0.0f, 1 - 1 /
                FloatMath::Sqrt(absff * height) - 0.3f) / 0.7f;

        mGlowScaleY = mGlowScaleYStart = scale;
    }

    mGlowAlphaFinish = mGlowAlpha;
    mGlowScaleYFinish = mGlowScaleY;
    return NOERROR;
}

ECode EdgeEffect::OnRelease()
{
    mPullDistance = 0;

    if (mState != STATE_PULL && mState != STATE_PULL_DECAY) {
        return NOERROR;
    }

    mState = STATE_RECEDE;
    mGlowAlphaStart = mGlowAlpha;
    mGlowScaleYStart = mGlowScaleY;

    mGlowAlphaFinish = 0.f;
    mGlowScaleYFinish = 0.f;

    AnimationUtils::CurrentAnimationTimeMillis(&mStartTime);
    mDuration = RECEDE_TIME;
    return NOERROR;
}

ECode EdgeEffect::OnAbsorb(
    /* [in] */ Int32 velocity)
{
    mState = STATE_ABSORB;
    velocity = Elastos::Core::Math::Min(Elastos::Core::Math::Max(MIN_VELOCITY, Elastos::Core::Math::Abs(velocity)), MAX_VELOCITY);

    AnimationUtils::CurrentAnimationTimeMillis(&mStartTime);
    mDuration = 0.15f + (velocity * 0.02f);

    // The glow depends more on the velocity, and therefore starts out
    // nearly invisible.
    mGlowAlphaStart = 0.3f;
    mGlowScaleYStart = Elastos::Core::Math::Max(mGlowScaleY, 0.f);

    // Growth for the size of the glow should be quadratic to properly
    // respond
    // to a user's scrolling speed. The faster the scrolling speed, the more
    // intense the effect should be for both the size and the saturation.
    mGlowScaleYFinish = Elastos::Core::Math::Min(0.025f + (velocity * (velocity / 100) * 0.00015f) / 2, 1.f);
    // Alpha should change for the glow as well as size.
    mGlowAlphaFinish = Elastos::Core::Math::Max(
            mGlowAlphaStart, Elastos::Core::Math::Min(velocity * VELOCITY_GLOW_FACTOR * .00001f, MAX_ALPHA));
    mTargetDisplacement = 0.5f;
    return NOERROR;
}

ECode EdgeEffect::SetColor(
    /* [in] */ Int32 color)
{
    return mPaint->SetColor(color);
}

ECode EdgeEffect::GetColor(
    /* [out] */ Int32* color)
{
    VALIDATE_NOT_NULL(color);
    return mPaint->GetColor(color);
}

ECode EdgeEffect::Draw(
    /* [in] */ ICanvas* canvas,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    Update();

    Int32 count;
    canvas->Save(&count);

    Int32 _centerX;
    mBounds->GetCenterX(&_centerX);
    Float centerX = (Float)_centerX;
    Int32 height;
    mBounds->GetHeight(&height);
    Float centerY = (Float)height - mRadius;

    canvas->Scale(1.f, Elastos::Core::Math::Min(mGlowScaleY, 1.f) * mBaseGlowScale, centerX, 0);

    const Float displacement = Elastos::Core::Math::Max(0.0f, Elastos::Core::Math::Min(mDisplacement, 1.f)) - 0.5f;
    Int32 width;
    mBounds->GetWidth(&width);
    Float translateX = width * displacement / 2;

    Boolean result;
    canvas->ClipRect(mBounds, &result);
    canvas->Translate(translateX, 0);
    mPaint->SetAlpha((Int32) (0xff * mGlowAlpha));
    canvas->DrawCircle(centerX, centerY, mRadius, mPaint);
    canvas->RestoreToCount(count);

    Boolean oneLastFrame = FALSE;
    if (mState == STATE_RECEDE && mGlowScaleY == 0) {
        mState = STATE_IDLE;
        oneLastFrame = TRUE;
    }

    *res = mState != STATE_IDLE || oneLastFrame;
    return NOERROR;
}

ECode EdgeEffect::GetMaxHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);

    Int32 h;
    mBounds->GetHeight(&h);
    *height = (Int32) (h * MAX_GLOW_SCALE + 0.5f);
    return NOERROR;
}

void EdgeEffect::Update()
{
    Int64 time;
    AnimationUtils::CurrentAnimationTimeMillis(&time);
    Float t = Elastos::Core::Math::Min((time - mStartTime) / mDuration, 1.f);

    Float interp;
    ITimeInterpolator::Probe(mInterpolator)->GetInterpolation(t, &interp);

    mGlowAlpha = mGlowAlphaStart + (mGlowAlphaFinish - mGlowAlphaStart) * interp;
    mGlowScaleY = mGlowScaleYStart + (mGlowScaleYFinish - mGlowScaleYStart) * interp;
    mDisplacement = (mDisplacement + mTargetDisplacement) / 2;

    if (t >= 1.f - EPSILON) {
        switch (mState) {
            case STATE_ABSORB: {
                    mState = STATE_RECEDE;
                    AnimationUtils::CurrentAnimationTimeMillis(&mStartTime);
                    mDuration = RECEDE_TIME;

                    mGlowAlphaStart = mGlowAlpha;
                    mGlowScaleYStart = mGlowScaleY;

                    // After absorb, the glow should fade to nothing.
                    mGlowAlphaFinish = 0.f;
                    mGlowScaleYFinish = 0.f;
                }
                break;
            case STATE_PULL: {
                    mState = STATE_PULL_DECAY;
                    AnimationUtils::CurrentAnimationTimeMillis(&mStartTime);
                    mDuration = PULL_DECAY_TIME;

                    mGlowAlphaStart = mGlowAlpha;
                    mGlowScaleYStart = mGlowScaleY;

                    // After pull, the glow should fade to nothing.
                    mGlowAlphaFinish = 0.f;
                    mGlowScaleYFinish = 0.f;
                }
                break;
            case STATE_PULL_DECAY:
                    mState = STATE_RECEDE;
                break;
            case STATE_RECEDE:
                mState = STATE_IDLE;
                break;
            default:
                break;
        }
    }
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
