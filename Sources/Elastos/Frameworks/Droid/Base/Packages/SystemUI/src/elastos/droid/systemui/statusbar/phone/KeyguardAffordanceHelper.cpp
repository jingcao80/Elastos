
#include "elastos/droid/systemui/statusbar/phone/KeyguardAffordanceHelper.h"
#include "elastos/droid/systemui/statusbar/CKeyguardAffordanceView.h"
#include "elastos/droid/systemui/statusbar/FlingAnimationUtils.h"
#include "../../R.h"
#include <elastos/droid/R.h>
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::CViewConfigurationHelper;
using Elastos::Droid::View::CVelocityTrackerHelper;
using Elastos::Droid::View::IVelocityTrackerHelper;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::IViewConfigurationHelper;
using Elastos::Core::IFloat;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

KeyguardAffordanceHelper::FlingEndListener::FlingEndListener(
    /* [in] */ KeyguardAffordanceHelper* host)
    : mHost(host)
{}

ECode KeyguardAffordanceHelper::FlingEndListener::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mSwipeAnimator = NULL;
    mHost->SetSwipingInProgress(FALSE);
    return NOERROR;
}

KeyguardAffordanceHelper::AnimationEndRunnable::AnimationEndRunnable(
    /* [in] */ KeyguardAffordanceHelper* host)
    : mHost(host)
{}

ECode KeyguardAffordanceHelper::AnimationEndRunnable::Run()
{
    mHost->mCallback->OnAnimationToSideEnded();
    return NOERROR;
}

KeyguardAffordanceHelper::AnimatorListenerAdapter1::AnimatorListenerAdapter1(
    /* [in] */ KeyguardAffordanceHelper* host,
    /* [in] */ IRunnable* onFinishedListener,
    /* [in] */ IKeyguardAffordanceView* targetView,
    /* [in] */ Boolean right)
    : mHost(host)
    , mOnFinishedListener(onFinishedListener)
    , mTargetView(targetView)
    , mRight(right)
    , mCancelled(FALSE)
{}

ECode KeyguardAffordanceHelper::AnimatorListenerAdapter1::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    mCancelled = TRUE;
    return NOERROR;
}

ECode KeyguardAffordanceHelper::AnimatorListenerAdapter1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mCancelled) {
        mHost->mSwipeAnimator = NULL;
        mOnFinishedListener->Run();
        mTargetView->ShowArrow(FALSE);
    }
    else {
        mHost->StartUnlockHintAnimationPhase2(mRight, mOnFinishedListener);
    }
    return NOERROR;
}

KeyguardAffordanceHelper::AnimatorListenerAdapter2::AnimatorListenerAdapter2(
    /* [in] */ KeyguardAffordanceHelper* host,
    /* [in] */ IKeyguardAffordanceView* targetView,
    /* [in] */ IRunnable* onFinishedListener)
    : mHost(host)
    , mTargetView(targetView)
    , mOnFinishedListener(onFinishedListener)
{}

ECode KeyguardAffordanceHelper::AnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mSwipeAnimator = NULL;
    mTargetView->ShowArrow(FALSE);
    mOnFinishedListener->Run();
    return NOERROR;
}

ECode KeyguardAffordanceHelper::AnimatorListenerAdapter2::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    mTargetView->ShowArrow(FALSE);
    return NOERROR;
}

CAR_INTERFACE_IMPL(KeyguardAffordanceHelper::AnimatorUpdateListener1, Object, IAnimatorUpdateListener);
KeyguardAffordanceHelper::AnimatorUpdateListener1::AnimatorUpdateListener1(
    /* [in] */ KeyguardAffordanceHelper* host,
    /* [in] */ IKeyguardAffordanceView* targetView,
    /* [in] */ Boolean right)
    : mHost(host)
    , mTargetView(targetView)
    , mRight(right)
{}

ECode KeyguardAffordanceHelper::AnimatorUpdateListener1::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    Float newRadius = 0;
    IFloat::Probe(obj)->GetValue(&newRadius);
    mTargetView->SetCircleRadiusWithoutAnimation(newRadius);
    Float translation = mHost->GetTranslationFromRadius(newRadius);
    mHost->mTranslation = mRight ? -translation : translation;
    mHost->UpdateIconsFromRadius(mTargetView, newRadius);
    return NOERROR;
}

CAR_INTERFACE_IMPL(KeyguardAffordanceHelper::AnimatorUpdateListener2, Object, IAnimatorUpdateListener);
KeyguardAffordanceHelper::AnimatorUpdateListener2::AnimatorUpdateListener2(
    /* [in] */ KeyguardAffordanceHelper* host)
    : mHost(host)
{}

ECode KeyguardAffordanceHelper::AnimatorUpdateListener2::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    IFloat::Probe(obj)->GetValue(&mHost->mTranslation);
    return NOERROR;
}

const Float KeyguardAffordanceHelper::SWIPE_RESTING_ALPHA_AMOUNT = 0.5f;
const Int64 KeyguardAffordanceHelper::HINT_PHASE1_DURATION = 200;
const Int64 KeyguardAffordanceHelper::HINT_PHASE2_DURATION = 350;
const Float KeyguardAffordanceHelper::BACKGROUND_RADIUS_SCALE_FACTOR = 0.15f;
const Int32 KeyguardAffordanceHelper::HINT_CIRCLE_OPEN_DURATION = 500;
KeyguardAffordanceHelper::KeyguardAffordanceHelper(
    /* [in] */ IKeyguardAffordanceHelperCallback* callback,
    /* [in] */ IContext* context)
    : mSwipingInProgress(FALSE)
    , mInitialTouchX(0)
    , mInitialTouchY(0)
    , mTranslation(0)
    , mTranslationOnDown(0)
    , mTouchSlop(0)
    , mMinTranslationAmount(0)
    , mMinFlingVelocity(0)
    , mHintGrowAmount(0)
    , mMinBackgroundRadius(0)
    , mMotionPerformedByUser(FALSE)
    , mMotionCancelled(FALSE)
{
    mFlingEndListener = new FlingEndListener(this);
    mAnimationEndRunnable = new AnimationEndRunnable(this);
    mContext = context;
    mCallback = callback;
    InitIcons();
    UpdateIcon(mLeftIcon, 0.0f, SWIPE_RESTING_ALPHA_AMOUNT, FALSE, FALSE);
    UpdateIcon(mCenterIcon, 0.0f, SWIPE_RESTING_ALPHA_AMOUNT, FALSE, FALSE);
    UpdateIcon(mRightIcon, 0.0f, SWIPE_RESTING_ALPHA_AMOUNT, FALSE, FALSE);
    InitDimens();
}

void KeyguardAffordanceHelper::InitDimens()
{
    AutoPtr<IViewConfigurationHelper> helper;
    CViewConfigurationHelper::AcquireSingleton((IViewConfigurationHelper**)&helper);
    AutoPtr<IViewConfiguration> configuration;
    helper->Get(mContext, (IViewConfiguration**)&configuration);
    configuration->GetScaledPagingTouchSlop(&mTouchSlop);
    configuration->GetScaledMinimumFlingVelocity(&mMinFlingVelocity);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(R::dimen::keyguard_min_swipe_amount, &mMinTranslationAmount);
    res->GetDimensionPixelSize(R::dimen::keyguard_affordance_min_background_radius, &mMinBackgroundRadius);
    res->GetDimensionPixelSize(R::dimen::hint_grow_amount_sideways, &mHintGrowAmount);
    mFlingAnimationUtils = new FlingAnimationUtils(mContext, 0.4f);
    AnimationUtils::LoadInterpolator(mContext, Elastos::Droid::R::interpolator::linear_out_slow_in
            , (IInterpolator**)&mAppearInterpolator);
    AnimationUtils::LoadInterpolator(mContext, Elastos::Droid::R::interpolator::fast_out_linear_in
            , (IInterpolator**)&mDisappearInterpolator);
}

void KeyguardAffordanceHelper::InitIcons()
{
    mCallback->GetLeftIcon((IKeyguardAffordanceView**)&mLeftIcon);
    mLeftIcon->SetIsLeft(TRUE);
    mCallback->GetCenterIcon((IKeyguardAffordanceView**)&mCenterIcon);
    mCallback->GetRightIcon((IKeyguardAffordanceView**)&mRightIcon);
    mRightIcon->SetIsLeft(FALSE);
    AutoPtr<IView> view;
    mCallback->GetLeftPreview((IView**)&view);
    mLeftIcon->SetPreviewView(view);

    view = NULL;
    mCallback->GetRightPreview((IView**)&view);
    mRightIcon->SetPreviewView(view);
}

Boolean KeyguardAffordanceHelper::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    Int32 masked = 0;
    if (mMotionCancelled && (event->GetActionMasked(&masked), masked) != IMotionEvent::ACTION_DOWN) {
        return FALSE;
    }
    Float y = 0;
    event->GetY(&y);
    Float x = 0;
    event->GetX(&x);

    Boolean isUp = FALSE;
    switch (event->GetActionMasked(&masked), masked) {
        case IMotionEvent::ACTION_DOWN:
            if (mSwipingInProgress) {
                CancelAnimation();
            }
            mInitialTouchY = y;
            mInitialTouchX = x;
            mTranslationOnDown = mTranslation;
            InitVelocityTracker();
            TrackMovement(event);
            mMotionPerformedByUser = FALSE;
            mMotionCancelled = FALSE;
            break;
        case IMotionEvent::ACTION_POINTER_DOWN:
            mMotionCancelled = TRUE;
            EndMotion(event, TRUE /* forceSnapBack */);
            break;
        case IMotionEvent::ACTION_MOVE: {
            const Float w = x - mInitialTouchX;
            TrackMovement(event);
            if (((LeftSwipePossible() && w > mTouchSlop)
                    || (RightSwipePossible() && w < -mTouchSlop))
                    && Elastos::Core::Math::Abs(w) > Elastos::Core::Math::Abs(y - mInitialTouchY)
                    && !mSwipingInProgress) {
                CancelAnimation();
                mInitialTouchY = y;
                mInitialTouchX = x;
                mTranslationOnDown = mTranslation;
                SetSwipingInProgress(TRUE);
            }
            if (mSwipingInProgress) {
                SetTranslation(mTranslationOnDown + x - mInitialTouchX, FALSE, FALSE);
            }
            break;
        }

        case IMotionEvent::ACTION_UP:
            isUp = TRUE;
        case IMotionEvent::ACTION_CANCEL:
            TrackMovement(event);
            EndMotion(event, !isUp);
            break;
    }
    return TRUE;
}

void KeyguardAffordanceHelper::EndMotion(
    /* [in] */ IMotionEvent* event,
    /* [in] */ Boolean forceSnapBack)
{
    if (mSwipingInProgress) {
        FlingWithCurrentVelocity(forceSnapBack);
    }
    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
        mVelocityTracker = NULL;
    }
}

void KeyguardAffordanceHelper::SetSwipingInProgress(
    /* [in] */ Boolean inProgress)
{
    mSwipingInProgress = inProgress;
    if (inProgress) {
        mCallback->OnSwipingStarted();
    }
}

Boolean KeyguardAffordanceHelper::RightSwipePossible()
{
    Int32 v = 0;
    return (IView::Probe(mRightIcon)->GetVisibility(&v), v) == IView::VISIBLE;
}

Boolean KeyguardAffordanceHelper::LeftSwipePossible()
{
    Int32 v = 0;
    return (IView::Probe(mLeftIcon)->GetVisibility(&v), v) == IView::VISIBLE;
}

Boolean KeyguardAffordanceHelper::OnInterceptTouchEvent(
    /* [in] */ IMotionEvent* ev)
{
    return FALSE;
}

void KeyguardAffordanceHelper::StartHintAnimation(
    /* [in] */ Boolean right,
    /* [in] */ IRunnable* onFinishedListener)
{
    StartHintAnimationPhase1(right, onFinishedListener);
}

void KeyguardAffordanceHelper::StartHintAnimationPhase1(
    /* [in] */ Boolean right,
    /* [in] */ IRunnable* onFinishedListener)
{
    AutoPtr<IKeyguardAffordanceView> targetView = right ? mRightIcon : mLeftIcon;
    targetView->ShowArrow(TRUE);
    AutoPtr<IValueAnimator> animator = GetAnimatorToRadius(right, mHintGrowAmount);
    AutoPtr<AnimatorListenerAdapter1> listener = new AnimatorListenerAdapter1(this, onFinishedListener
            , targetView, right);
    IAnimator::Probe(animator)->AddListener(listener);
    IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(mAppearInterpolator));
    IAnimator::Probe(animator)->SetDuration(HINT_PHASE1_DURATION);
    IAnimator::Probe(animator)->Start();
    mSwipeAnimator = IAnimator::Probe(animator);
}

/**
 * Phase 2: Move back.
 */
void KeyguardAffordanceHelper::StartUnlockHintAnimationPhase2(
    /* [in] */ Boolean right,
    /* [in] */ IRunnable* onFinishedListener)
{
    AutoPtr<IKeyguardAffordanceView> targetView = right ? mRightIcon : mLeftIcon;
    AutoPtr<IValueAnimator> animator = GetAnimatorToRadius(right, 0);
    AutoPtr<AnimatorListenerAdapter2> listener = new AnimatorListenerAdapter2(this
                , targetView, onFinishedListener);
    IAnimator::Probe(animator)->AddListener(listener);
    IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(mDisappearInterpolator));
    IAnimator::Probe(animator)->SetDuration(HINT_PHASE2_DURATION);
    IAnimator::Probe(animator)->SetStartDelay(HINT_CIRCLE_OPEN_DURATION);
    IAnimator::Probe(animator)->Start();
    mSwipeAnimator = IAnimator::Probe(animator);
}

AutoPtr<IValueAnimator> KeyguardAffordanceHelper::GetAnimatorToRadius(
    /* [in] */ Boolean right,
    /* [in] */ Int32 radius)
{
    AutoPtr<IKeyguardAffordanceView> targetView = right ? mRightIcon : mLeftIcon;
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<IValueAnimator> animator;
    Float fv = 0;
    targetView->GetCircleRadius(&fv);
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = fv;
    (*fvs)[1] = radius;
    helper->OfFloat(fvs, (IValueAnimator**)&animator);
    AutoPtr<AnimatorUpdateListener1> listener = new AnimatorUpdateListener1(this, targetView, right);
    animator->AddUpdateListener(listener);
    return animator;
}

void KeyguardAffordanceHelper::CancelAnimation()
{
    if (mSwipeAnimator != NULL) {
        mSwipeAnimator->Cancel();
    }
}

void KeyguardAffordanceHelper::FlingWithCurrentVelocity(
    /* [in] */ Boolean forceSnapBack)
{
    Float vel = GetCurrentVelocity();

    // We snap back if the current translation is not far enough
    Boolean snapBack = IsBelowFalsingThreshold();

    // or if the velocity is in the opposite direction.
    Boolean velIsInWrongDirection = vel * mTranslation < 0;
    snapBack |= Elastos::Core::Math::Abs(vel) > mMinFlingVelocity && velIsInWrongDirection;
    vel = snapBack ^ velIsInWrongDirection ? 0 : vel;
    Fling(vel, snapBack || forceSnapBack);
}

Boolean KeyguardAffordanceHelper::IsBelowFalsingThreshold()
{
    return Elastos::Core::Math::Abs(mTranslation) <
        Elastos::Core::Math::Abs(mTranslationOnDown) + GetMinTranslationAmount();
}

Int32 KeyguardAffordanceHelper::GetMinTranslationAmount()
{
    Float factor = 0;
    mCallback->GetAffordanceFalsingFactor(&factor);
    return (Int32) (mMinTranslationAmount * factor);
}

void KeyguardAffordanceHelper::Fling(
    /* [in] */ Float vel,
    /* [in] */ Boolean snapBack)
{
    Float fv = 0;
    Float target = mTranslation < 0 ? (mCallback->GetPageWidth(&fv), -fv) : (mCallback->GetPageWidth(&fv), fv);
    target = snapBack ? 0 : target;

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > fvs = ArrayOf<Float>::Alloc(2);
    (*fvs)[0] = mTranslation;
    (*fvs)[1] = target;

    AutoPtr<IValueAnimator> animator;
    helper->OfFloat(fvs, (IValueAnimator**)&animator);
    mFlingAnimationUtils->Apply(IAnimator::Probe(animator), mTranslation, target, vel);
    AutoPtr<AnimatorUpdateListener2> listener = new AnimatorUpdateListener2(this);
    animator->AddUpdateListener(listener);
    IAnimator::Probe(animator)->AddListener(mFlingEndListener);
    if (!snapBack) {
        StartFinishingCircleAnimation(vel * 0.375f, mAnimationEndRunnable);
        mCallback->OnAnimationToSideStarted(mTranslation < 0);
    } else {
        Reset(TRUE);
    }
    IAnimator::Probe(animator)->Start();
    mSwipeAnimator = IAnimator::Probe(animator);
}

void KeyguardAffordanceHelper::StartFinishingCircleAnimation(
    /* [in] */ Float velocity,
    /* [in] */ IRunnable* mAnimationEndRunnable)
{
    AutoPtr<IKeyguardAffordanceView> targetView = mTranslation > 0 ? mLeftIcon : mRightIcon;
    targetView->FinishAnimation(velocity, mAnimationEndRunnable);
}

void KeyguardAffordanceHelper::SetTranslation(
    /* [in] */ Float translation,
    /* [in] */ Boolean isReset,
    /* [in] */ Boolean animateReset)
{
    translation = RightSwipePossible() ? translation : Elastos::Core::Math::Max((Float)0, translation);
    translation = LeftSwipePossible() ? translation : Elastos::Core::Math::Min((Float)0, translation);
    Float absTranslation = Elastos::Core::Math::Abs(translation);
    if (absTranslation > Elastos::Core::Math::Abs(mTranslationOnDown) + GetMinTranslationAmount() ||
            mMotionPerformedByUser) {
        mMotionPerformedByUser = TRUE;
    }
    if (translation != mTranslation || isReset) {
        AutoPtr<IKeyguardAffordanceView> targetView = translation > 0 ? mLeftIcon : mRightIcon;
        AutoPtr<IKeyguardAffordanceView> otherView = translation > 0 ? mRightIcon : mLeftIcon;
        Float alpha = absTranslation / GetMinTranslationAmount();

        // We interpolate the alpha of the other icons to 0
        Float fadeOutAlpha = SWIPE_RESTING_ALPHA_AMOUNT * (1.0f - alpha);
        fadeOutAlpha = Elastos::Core::Math::Max(0.0f, fadeOutAlpha);

        // We interpolate the alpha of the targetView to 1
        alpha = fadeOutAlpha + alpha;

        Boolean animateIcons = isReset && animateReset;
        Float radius = GetRadiusFromTranslation(absTranslation);
        Boolean slowAnimation = isReset && IsBelowFalsingThreshold();
        if (!isReset) {
            UpdateIcon(targetView, radius, alpha, FALSE, FALSE);
        } else {
            UpdateIcon(targetView, 0.0f, fadeOutAlpha, animateIcons, slowAnimation);
        }
        UpdateIcon(otherView, 0.0f, fadeOutAlpha, animateIcons, slowAnimation);
        UpdateIcon(mCenterIcon, 0.0f, fadeOutAlpha, animateIcons, slowAnimation);

        mTranslation = translation;
    }
}

void KeyguardAffordanceHelper::UpdateIconsFromRadius(
    /* [in] */ IKeyguardAffordanceView* targetView,
    /* [in] */ Float newRadius)
{
    Float alpha = newRadius / mMinBackgroundRadius;

    // We interpolate the alpha of the other icons to 0
    Float fadeOutAlpha = SWIPE_RESTING_ALPHA_AMOUNT * (1.0f - alpha);
    fadeOutAlpha = Elastos::Core::Math::Max(0.0f, fadeOutAlpha);

    // We interpolate the alpha of the targetView to 1
    alpha = fadeOutAlpha + alpha;
    AutoPtr<IKeyguardAffordanceView> otherView = targetView == mRightIcon ? mLeftIcon : mRightIcon;
    UpdateIconAlpha(targetView, alpha, FALSE);
    UpdateIconAlpha(otherView, fadeOutAlpha, FALSE);
    UpdateIconAlpha(mCenterIcon, fadeOutAlpha, FALSE);
}

Float KeyguardAffordanceHelper::GetTranslationFromRadius(
    /* [in] */ Float circleSize)
{
    Float translation = (circleSize - mMinBackgroundRadius) / BACKGROUND_RADIUS_SCALE_FACTOR;
    return Elastos::Core::Math::Max((Float)0, translation);
}

Float KeyguardAffordanceHelper::GetRadiusFromTranslation(
    /* [in] */ Float translation)
{
    return translation * BACKGROUND_RADIUS_SCALE_FACTOR + mMinBackgroundRadius;
}

void KeyguardAffordanceHelper::AnimateHideLeftRightIcon()
{
    UpdateIcon(mRightIcon, 0.f, 0.f, TRUE, FALSE);
    UpdateIcon(mLeftIcon, 0.f, 0.f, TRUE, FALSE);
}

void KeyguardAffordanceHelper::UpdateIcon(
    /* [in] */ IKeyguardAffordanceView* view,
    /* [in] */ Float circleRadius,
    /* [in] */ Float alpha,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean slowRadiusAnimation)
{
    Int32 v = 0;
    if ((IView::Probe(view)->GetVisibility(&v), v) != IView::VISIBLE) {
        return;
    }
    view->SetCircleRadius(circleRadius, slowRadiusAnimation);
    UpdateIconAlpha(view, alpha, animate);
}

void KeyguardAffordanceHelper::UpdateIconAlpha(
    /* [in] */ IKeyguardAffordanceView* view,
    /* [in] */ Float alpha,
    /* [in] */ Boolean animate)
{
    Float scale = GetScale(alpha);
    alpha = Elastos::Core::Math::Min(1.0f, alpha);
    view->SetImageAlpha(alpha, animate);
    view->SetImageScale(scale, animate);
}

Float KeyguardAffordanceHelper::GetScale(
    /* [in] */ Float alpha)
{
    Float scale = alpha / SWIPE_RESTING_ALPHA_AMOUNT * 0.2f +
            CKeyguardAffordanceView::MIN_ICON_SCALE_AMOUNT;
    return Elastos::Core::Math::Min(scale, CKeyguardAffordanceView::MAX_ICON_SCALE_AMOUNT);
}

void KeyguardAffordanceHelper::TrackMovement(
    /* [in] */ IMotionEvent* event)
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->AddMovement(event);
    }
}

void KeyguardAffordanceHelper::InitVelocityTracker()
{
    if (mVelocityTracker != NULL) {
        mVelocityTracker->Recycle();
    }
    AutoPtr<IVelocityTrackerHelper> helper;
    CVelocityTrackerHelper::AcquireSingleton((IVelocityTrackerHelper**)&helper);
    helper->Obtain((IVelocityTracker**)&mVelocityTracker);
}

Float KeyguardAffordanceHelper::GetCurrentVelocity()
{
    if (mVelocityTracker == NULL) {
        return 0;
    }
    mVelocityTracker->ComputeCurrentVelocity(1000);
    Float fv = 0;
    mVelocityTracker->GetXVelocity(&fv);
    return fv;
}

void KeyguardAffordanceHelper::OnConfigurationChanged()
{
    InitDimens();
    InitIcons();
}

void KeyguardAffordanceHelper::Reset(
    /* [in] */ Boolean animate)
{
    if (mSwipeAnimator != NULL) {
        mSwipeAnimator->Cancel();
    }
    SetTranslation(0.0f, TRUE, animate);
    SetSwipingInProgress(FALSE);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
