
#include "elastos/droid/systemui/statusbar/phone/CTrustDrawable.h"
#include "../R.h"
#include "Elastos.Droid.Content.h"
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/droid/R.h>

using Elastos::Droid::Animation::CAnimatorSet;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::IAnimatorSet;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::PaintStyle_STROKE;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::CAccelerateDecelerateInterpolator;
using Elastos::Core::IFloat;
using Elastos::Core::IInteger32;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const Int64 CTrustDrawable::ENTERING_FROM_UNSET_START_DELAY = 200;
const Int64 CTrustDrawable::VISIBLE_DURATION = 1000;
const Int64 CTrustDrawable::EXIT_DURATION = 500;
const Int64 CTrustDrawable::ENTER_DURATION = 500;
const Int32 CTrustDrawable::ALPHA_VISIBLE_MIN = 0x26;
const Int32 CTrustDrawable::ALPHA_VISIBLE_MAX = 0x4c;
const Int32 CTrustDrawable::STATE_UNSET = -1;
const Int32 CTrustDrawable::STATE_GONE = 0;
const Int32 CTrustDrawable::STATE_ENTERING = 1;
const Int32 CTrustDrawable::STATE_VISIBLE = 2;
const Int32 CTrustDrawable::STATE_EXITING = 3;
CTrustDrawable::StateUpdateAnimatorListener::StateUpdateAnimatorListener(
    /* [in] */ CTrustDrawable* host)
    : mHost(host)
    , mCancelled(FALSE)
{}

ECode CTrustDrawable::StateUpdateAnimatorListener::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mCancelled = FALSE;
    return NOERROR;
}

ECode CTrustDrawable::StateUpdateAnimatorListener::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCancelled = TRUE;
    return NOERROR;
}

ECode CTrustDrawable::StateUpdateAnimatorListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (!mCancelled) {
        mHost->UpdateState(FALSE);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(CTrustDrawable::AlphaUpdateListener, Object, IAnimatorUpdateListener)
CTrustDrawable::AlphaUpdateListener::AlphaUpdateListener(
    /* [in] */ CTrustDrawable* host)
    : mHost(host)
{}

ECode CTrustDrawable::AlphaUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    IInteger32::Probe(obj)->GetValue(&mHost->mCurAlpha);
    mHost->InvalidateSelf();
    return NOERROR;
}

CAR_INTERFACE_IMPL(CTrustDrawable::RadiusUpdateListener, Object, IAnimatorUpdateListener)
CTrustDrawable::RadiusUpdateListener::RadiusUpdateListener(
    /* [in] */ CTrustDrawable* host)
    : mHost(host)
{}

ECode CTrustDrawable::RadiusUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    IFloat::Probe(obj)->GetValue(&mHost->mCurInnerRadius);
    mHost->InvalidateSelf();
    return NOERROR;
}

CAR_OBJECT_IMPL(CTrustDrawable)
CTrustDrawable::CTrustDrawable()
    : mAlpha(0)
    , mAnimating(FALSE)
    , mCurAlpha(0)
    , mCurInnerRadius(0)
    , mState(STATE_UNSET)
    , mTrustManaged(FALSE)
    , mInnerRadiusVisibleMin(0)
    , mInnerRadiusVisibleMax(0)
    , mInnerRadiusExit(0)
    , mInnerRadiusEnter(0)
    , mThickness(0)
{}

ECode CTrustDrawable::constructor(
    /* [in] */ IContext* context)
{
    mAlphaUpdateListener = new AlphaUpdateListener(this);
    mRadiusUpdateListener = new RadiusUpdateListener(this);
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);
    r->GetDimension(R::dimen::trust_circle_inner_radius_visible_min, &mInnerRadiusVisibleMin);
    r->GetDimension(R::dimen::trust_circle_inner_radius_visible_max, &mInnerRadiusVisibleMax);
    r->GetDimension(R::dimen::trust_circle_inner_radius_exit, &mInnerRadiusExit);
    r->GetDimension(R::dimen::trust_circle_inner_radius_enter, &mInnerRadiusEnter);
    r->GetDimension(R::dimen::trust_circle_thickness, &mThickness);

    mCurInnerRadius = mInnerRadiusEnter;

    AnimationUtils::LoadInterpolator(context,
            Elastos::Droid::R::interpolator::linear_out_slow_in, (IInterpolator**)&mLinearOutSlowInInterpolator);
    AnimationUtils::LoadInterpolator(context,
            Elastos::Droid::R::interpolator::fast_out_slow_in, (IInterpolator**)&mFastOutSlowInInterpolator);
    CAccelerateDecelerateInterpolator::New((IInterpolator**)&mAccelerateDecelerateInterpolator);

    mVisibleAnimator = MakeVisibleAnimator();

    CPaint::New((IPaint**)&mPaint);
    mPaint->SetStyle(PaintStyle_STROKE);
    mPaint->SetColor(IColor::WHITE);
    mPaint->SetAntiAlias(TRUE);
    mPaint->SetStrokeWidth(mThickness);
    return Drawable::constructor();
}

ECode CTrustDrawable::Draw(
    /* [in] */ ICanvas* canvas)
{
    Int32 newAlpha = (mCurAlpha * mAlpha) / 256;
    if (newAlpha == 0) {
        return NOERROR;
    }
    AutoPtr<IRect> r;
    GetBounds((IRect**)&r);
    mPaint->SetAlpha(newAlpha);
    Float x = 0, y = 0;
    r->GetExactCenterX(&x);
    r->GetExactCenterY(&y);
    canvas->DrawCircle(x, y, mCurInnerRadius, mPaint);
    return NOERROR;
}

ECode CTrustDrawable::SetAlpha(
    /* [in] */ Int32 alpha)
{
    mAlpha = alpha;
    return NOERROR;
}

ECode CTrustDrawable::GetAlpha(
    /* [out] */ Int32* alpha)
{
    VALIDATE_NOT_NULL(alpha);
    *alpha = mAlpha;
    return NOERROR;
}

ECode CTrustDrawable::SetColorFilter(
    /* [in] */ IColorFilter* cf)
{
    // throw new UnsupportedOperationException("not implemented");
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CTrustDrawable::GetOpacity(
    /* [out] */ Int32* opacity)
{
    VALIDATE_NOT_NULL(opacity);
    *opacity = IPixelFormat::TRANSLUCENT;
    return NOERROR;
}

void CTrustDrawable::Start()
{
    if (!mAnimating) {
        mAnimating = TRUE;
        UpdateState(TRUE);
    }
}

void CTrustDrawable::Stop()
{
    if (mAnimating) {
        mAnimating = FALSE;
        if (mCurAnimator != NULL) {
            mCurAnimator->Cancel();
            mCurAnimator = NULL;
        }
        mState = STATE_UNSET;
        mCurAlpha = 0;
        mCurInnerRadius = mInnerRadiusEnter;
    }
}

void CTrustDrawable::SetTrustManaged(
    /* [in] */ Boolean trustManaged)
{
    if (trustManaged == mTrustManaged && mState != STATE_UNSET) return;
    mTrustManaged = trustManaged;
    if (mAnimating) {
        UpdateState(TRUE);
    }
}

void CTrustDrawable::UpdateState(
    /* [in] */ Boolean animate)
{
    Int32 nextState = mState;
    if (mState == STATE_UNSET) {
        nextState = mTrustManaged ? STATE_ENTERING : STATE_GONE;
    }
    else if (mState == STATE_GONE) {
        if (mTrustManaged) nextState = STATE_ENTERING;
    }
    else if (mState == STATE_ENTERING) {
        if (!mTrustManaged) nextState = STATE_EXITING;
    }
    else if (mState == STATE_VISIBLE) {
        if (!mTrustManaged) nextState = STATE_EXITING;
    }
    else if (mState == STATE_EXITING) {
        if (mTrustManaged) nextState = STATE_ENTERING;
    }
    if (!animate) {
        if (nextState == STATE_ENTERING) nextState = STATE_VISIBLE;
        if (nextState == STATE_EXITING) nextState = STATE_GONE;
    }

    if (nextState != mState) {
        if (mCurAnimator != NULL) {
            mCurAnimator->Cancel();
            mCurAnimator = NULL;
        }

        if (nextState == STATE_GONE) {
            mCurAlpha = 0;
            mCurInnerRadius = mInnerRadiusEnter;
        }
        else if (nextState == STATE_ENTERING) {
            mCurAnimator = MakeEnterAnimator(mCurInnerRadius, mCurAlpha);
            if (mState == STATE_UNSET) {
                mCurAnimator->SetStartDelay(ENTERING_FROM_UNSET_START_DELAY);
            }
        }
        else if (nextState == STATE_VISIBLE) {
            mCurAlpha = ALPHA_VISIBLE_MAX;
            mCurInnerRadius = mInnerRadiusVisibleMax;
            mCurAnimator = mVisibleAnimator;
        }
        else if (nextState == STATE_EXITING) {
            mCurAnimator = MakeExitAnimator(mCurInnerRadius, mCurAlpha);
        }

        mState = nextState;
        if (mCurAnimator != NULL) {
            mCurAnimator->Start();
        }
        else {
            InvalidateSelf();
        }
    }
}

AutoPtr<IAnimator> CTrustDrawable::MakeVisibleAnimator()
{
    return MakeAnimators(mInnerRadiusVisibleMax, mInnerRadiusVisibleMin,
            ALPHA_VISIBLE_MAX, ALPHA_VISIBLE_MIN, VISIBLE_DURATION,
            mAccelerateDecelerateInterpolator,
            TRUE /* repeating */, FALSE /* stateUpdateListener */);
}

AutoPtr<IAnimator> CTrustDrawable::MakeEnterAnimator(
    /* [in] */ Float radius,
    /* [in] */ Int32 alpha)
{
    return MakeAnimators(radius, mInnerRadiusVisibleMax,
            alpha, ALPHA_VISIBLE_MAX, ENTER_DURATION, mLinearOutSlowInInterpolator,
            FALSE /* repeating */, TRUE /* stateUpdateListener */);
}

AutoPtr<IAnimator> CTrustDrawable::MakeExitAnimator(
    /* [in] */ Float radius,
    /* [in] */ Int32 alpha)
{
    return MakeAnimators(radius, mInnerRadiusExit,
            alpha, 0, EXIT_DURATION, mFastOutSlowInInterpolator,
            FALSE /* repeating */, TRUE /* stateUpdateListener */);
}

AutoPtr<IAnimator> CTrustDrawable::MakeAnimators(
    /* [in] */ Float startRadius,
    /* [in] */ Float endRadius,
    /* [in] */ Int32 startAlpha,
    /* [in] */ Int32 endAlpha,
    /* [in] */ Int64 duration,
    /* [in] */ IInterpolator* interpolator,
    /* [in] */ Boolean repeating,
    /* [in] */ Boolean stateUpdateListener)
{
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> obj;
    AutoPtr<ArrayOf<Int32> > ivs = ArrayOf<Int32>::Alloc(2);
    (*ivs)[0] = startAlpha;
    (*ivs)[1] = endAlpha;
    helper->OfInt32(ivs, (IValueAnimator**)&obj);
    AutoPtr<IValueAnimator> alphaAnimator = ConfigureAnimator(obj,
            duration, mAlphaUpdateListener, interpolator, repeating);

    obj = NULL;
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = startRadius;
    (*fvs)[1] = endRadius;
    helper->OfFloat(fvs, (IValueAnimator**)&obj);
    AutoPtr<IValueAnimator> sizeAnimator = ConfigureAnimator(obj,
            duration, mRadiusUpdateListener, interpolator, repeating);

    AutoPtr<IAnimatorSet> set;
    CAnimatorSet::New((IAnimatorSet**)&set);
    AutoPtr<ArrayOf<IAnimator*> > as = ArrayOf<IAnimator*>::Alloc(2);
    as->Set(0, IAnimator::Probe(alphaAnimator));
    as->Set(1, IAnimator::Probe(sizeAnimator));
    set->PlayTogether(as);
    if (stateUpdateListener) {
        AutoPtr<StateUpdateAnimatorListener> listener = new StateUpdateAnimatorListener(this);
        IAnimator::Probe(set)->AddListener(listener);
    }
    return IAnimator::Probe(set);
}

AutoPtr<IValueAnimator> CTrustDrawable::ConfigureAnimator(
    /* [in] */ IValueAnimator* animator,
    /* [in] */ Int64 duration,
    /* [in] */ IAnimatorUpdateListener* updateListener,
    /* [in] */ IInterpolator* interpolator,
    /* [in] */ Boolean repeating)
{
    animator->SetDuration(duration);
    animator->AddUpdateListener(updateListener);
    IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(interpolator));
    if (repeating) {
        animator->SetRepeatCount(IValueAnimator::ANIMATION_INFINITE);
        animator->SetRepeatMode(IValueAnimator::ANIMATION_REVERSE);
    }
    return animator;
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
