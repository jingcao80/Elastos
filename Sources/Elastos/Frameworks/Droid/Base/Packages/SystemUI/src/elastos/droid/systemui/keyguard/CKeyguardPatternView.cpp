
#include "elastos/droid/systemui/keyguard/CKeyguardPatternView.h"
#include "elastos/droid/systemui/keyguard/KeyguardUpdateMonitor.h"
#include "elastos/droid/systemui/keyguard/AppearAnimationUtils.h"
#include "elastos/droid/systemui/keyguard/CKeyguardMessageArea.h"
#include "elastos/droid/systemui/keyguard/KeyguardSecurityViewHelper.h"
#include "elastos/droid/view/animation/AnimationUtils.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/CoreUtils.h>
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::IAnimatorListener;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Internal::Widget::CLockPatternUtils;
using Elastos::Droid::Internal::Widget::DisplayMode_Correct;
using Elastos::Droid::Internal::Widget::DisplayMode_Wrong;
using Elastos::Droid::Internal::Widget::EIID_IOnPatternListener;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IViewPropertyAnimator;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CAR_INTERFACE_IMPL(CKeyguardPatternView::UnlockPatternListener, Object,
        IOnPatternListener)

ECode CKeyguardPatternView::UnlockPatternListener::OnPatternStart()
{
    Boolean res;
    return IView::Probe(mHost->mLockPatternView)->RemoveCallbacks(
            mHost->mCancelPatternRunnable, &res);
}

ECode CKeyguardPatternView::UnlockPatternListener::OnPatternCleared()
{
    return NOERROR;
}

ECode CKeyguardPatternView::UnlockPatternListener::OnPatternCellAdded(
    /* [in] */ IList* pattern)
{
    return mHost->mCallback->UserActivity();
}

ECode CKeyguardPatternView::UnlockPatternListener::OnPatternDetected(
    /* [in] */ IList* pattern)
{
    Boolean res;
    if (mHost->mLockPatternUtils->CheckPattern(pattern, &res), res) {
        mHost->mCallback->ReportUnlockAttempt(TRUE);
        mHost->mLockPatternView->SetDisplayMode(DisplayMode_Correct);
        mHost->mCallback->Dismiss(TRUE);
    }
    else {
        Int32 size;
        pattern->GetSize(&size);
        if (size > MIN_PATTERN_BEFORE_POKE_WAKELOCK) {
            mHost->mCallback->UserActivity();
        }
        mHost->mLockPatternView->SetDisplayMode(DisplayMode_Wrong);
        Boolean registeredAttempt =
                size >= ILockPatternUtils::MIN_PATTERN_REGISTER_FAIL;
        if (registeredAttempt) {
            mHost->mCallback->ReportUnlockAttempt(FALSE);
        }
        Int32 attempts;
        mHost->mKeyguardUpdateMonitor->GetFailedUnlockAttempts(&attempts);
        if (registeredAttempt &&
                0 == (attempts % ILockPatternUtils::FAILED_ATTEMPTS_BEFORE_TIMEOUT)) {
            Int64 deadline;
            mHost->mLockPatternUtils->SetLockoutAttemptDeadline(&deadline);
            mHost->HandleAttemptLockout(deadline);
        }
        else {
            mHost->mSecurityMessageDisplay->SetMessage(R::string::kg_wrong_pattern, TRUE);
            Boolean res;
            IView::Probe(mHost->mLockPatternView)->PostDelayed(mHost->mCancelPatternRunnable,
                    PATTERN_CLEAR_TIMEOUT_MS, &res);
        }
    }
    return NOERROR;
}

ECode CKeyguardPatternView::MyRunnable::Run()
{
    return mHost->mLockPatternView->ClearPattern();
}

CKeyguardPatternView::MyCountDownTimer::MyCountDownTimer(
    /* [in] */ CKeyguardPatternView* host,
    /* [in] */ Int64 elapsedRealtimeDeadline,
    /* [in] */ Int64 countDownInterval)
    : mHost(host)
{
    CountDownTimer::constructor(elapsedRealtimeDeadline, countDownInterval);
}

ECode CKeyguardPatternView::MyCountDownTimer::OnTick(
    /* [in] */ Int64 millisUntilFinished)
{
    Int32 secondsRemaining = (Int32) (millisUntilFinished / 1000);

    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<IInteger32> obj = CoreUtils::Convert(secondsRemaining);
    array->Set(0, TO_IINTERFACE(obj));
    return mHost->mSecurityMessageDisplay->SetMessage(
            R::string::kg_too_many_failed_attempts_countdown, TRUE, array);
}

ECode CKeyguardPatternView::MyCountDownTimer::OnFinish()
{
    IView::Probe(mHost->mLockPatternView)->SetEnabled(TRUE);
    mHost->DisplayDefaultSecurityMessage();
    return NOERROR;
}

ECode CKeyguardPatternView::MyRunnable2::Run()
{
    mHost->EnableClipping(TRUE);
    return NOERROR;
}

CAR_INTERFACE_IMPL(CKeyguardPatternView::MyAnimatorUpdateListener,
        Object, IAnimatorUpdateListener)

ECode CKeyguardPatternView::MyAnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    Float animatedFraction;
    animation->GetAnimatedFraction(&animatedFraction);
    mAnimatedCell->SetScale(animatedFraction);
    AutoPtr<IInterface> value;
    animation->GetAnimatedValue((IInterface**)&value);
    AutoPtr<IFloat> obj = IFloat::Probe(value);
    Float y;
    obj->GetValue(&y);
    mAnimatedCell->SetTranslateY(y);
    return IView::Probe(mHost->mLockPatternView)->Invalidate();
}

ECode CKeyguardPatternView::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    return mFinishListener->Run();
}

const String CKeyguardPatternView::TAG("SecurityPatternView");
const Boolean CKeyguardPatternView::DEBUG = IKeyguardConstants::DEBUG;

// how long before we clear the wrong pattern
const Int32 CKeyguardPatternView::PATTERN_CLEAR_TIMEOUT_MS = 2000;

// how long we stay awake after each key beyond MIN_PATTERN_BEFORE_POKE_WAKELOCK
const Int32 CKeyguardPatternView::UNLOCK_PATTERN_WAKE_INTERVAL_MS = 7000;

// how many cells the user has to cross before we poke the wakelock
const Int32 CKeyguardPatternView::MIN_PATTERN_BEFORE_POKE_WAKELOCK = 2;

CAR_OBJECT_IMPL(CKeyguardPatternView)

CAR_INTERFACE_IMPL_2(CKeyguardPatternView, LinearLayout,
        IKeyguardSecurityView, IAppearAnimationCreator)

CKeyguardPatternView::CKeyguardPatternView()
    : mLastPokeTime(-UNLOCK_PATTERN_WAKE_INTERVAL_MS)
{
    mCancelPatternRunnable = new MyRunnable(this);

    CRect::New((IRect**)&mTempRect);
}

ECode CKeyguardPatternView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CKeyguardPatternView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);

    mKeyguardUpdateMonitor = KeyguardUpdateMonitor::GetInstance(mContext);

    AutoPtr<IAnimationUtils> helper;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&helper);
    AutoPtr<IInterpolator> interpolator;
    helper->LoadInterpolator(
            mContext, Elastos::Droid::R::interpolator::linear_out_slow_in, (IInterpolator**)&interpolator);
    mAppearAnimationUtils = new AppearAnimationUtils();
    mAppearAnimationUtils->constructor(context,
            AppearAnimationUtils::DEFAULT_APPEAR_DURATION, 1.5f /* delayScale */,
            2.0f /* transitionScale */, interpolator);
    AutoPtr<IResources> resources;
    GetResources((IResources**)&resources);
    return resources->GetDimensionPixelSize(R::dimen::disappear_y_translation, &mDisappearYTranslation);
}

ECode CKeyguardPatternView::SetKeyguardCallback(
    /* [in] */ IKeyguardSecurityCallback* callback)
{
    mCallback = callback;
    return NOERROR;
}

ECode CKeyguardPatternView::SetLockPatternUtils(
    /* [in] */ ILockPatternUtils* utils)
{
    mLockPatternUtils = utils;
    return NOERROR;
}

ECode CKeyguardPatternView::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();

    if (mLockPatternUtils == NULL) {
        CLockPatternUtils::New(mContext,
                (ILockPatternUtils**)&mLockPatternUtils);
    }

    AutoPtr<IView> view;
    FindViewById(R::id::lockPatternView, (IView**)&view);
    mLockPatternView = ILockPatternView::Probe(view);
    IView::Probe(mLockPatternView)->SetSaveEnabled(FALSE);
    IView::Probe(mLockPatternView)->SetFocusable(FALSE);
    AutoPtr<IOnPatternListener> lis = new UnlockPatternListener(this);
    mLockPatternView->SetOnPatternListener(lis);

    // stealth mode will be the same for the life of this screen
    Boolean res;
    mLockPatternUtils->IsVisiblePatternEnabled(&res);
    mLockPatternView->SetInStealthMode(!res);

    // vibrate mode will be the same for the life of this screen
    mLockPatternUtils->IsTactileFeedbackEnabled(&res);
    mLockPatternView->SetTactileFeedbackEnabled(res);

    SetFocusableInTouchMode(TRUE);

    AutoPtr<CKeyguardMessageArea::Helper> helper =
            new CKeyguardMessageArea::Helper();
    helper->constructor(this);
    mSecurityMessageDisplay = ISecurityMessageDisplay::Probe(helper);
    FindViewById(R::id::keyguard_selector_fade_container, (IView**)&mEcaView);
    AutoPtr<IView> bouncerFrameView;
    FindViewById(R::id::keyguard_bouncer_frame, (IView**)&bouncerFrameView);
    if (bouncerFrameView != NULL) {
        bouncerFrameView->GetBackground((IDrawable**)&mBouncerFrame);
    }

    AutoPtr<IView> view2;
    FindViewById(R::id::keyguard_bouncer_frame, (IView**)&view2);
    mKeyguardBouncerFrame = IViewGroup::Probe(view2);
    AutoPtr<IView> view3;
    FindViewById(R::id::keyguard_message_area, (IView**)&view3);
    mHelpMessage = IKeyguardMessageArea::Probe(view3);
    return NOERROR;
}

ECode CKeyguardPatternView::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)

    Boolean result;
    LinearLayout::OnTouchEvent(ev, &result);
    // as long as the user is entering a pattern (i.e sending a touch event that was handled
    // by this screen), keep poking the wake lock so that the screen will stay on.
    Int64 elapsed = SystemClock::GetElapsedRealtime() - mLastPokeTime;
    if (result && (elapsed > (UNLOCK_PATTERN_WAKE_INTERVAL_MS - 100))) {
        mLastPokeTime = SystemClock::GetElapsedRealtime();
    }
    mTempRect->Set(0, 0, 0, 0);
    OffsetRectIntoDescendantCoords(IView::Probe(mLockPatternView), mTempRect);

    Int32 left;
    mTempRect->GetLeft(&left);
    Int32 top;
    mTempRect->GetTop(&top);
    ev->OffsetLocation(left, top);
    Boolean tmp;
    IView::Probe(mLockPatternView)->DispatchTouchEvent(ev, &tmp);
    result = tmp || result;
    ev->OffsetLocation(-left, -top);
    *res = result;
    return NOERROR;
}

ECode CKeyguardPatternView::Reset()
{
    // reset lock pattern
    mLockPatternView->EnableInput();
    IView::Probe(mLockPatternView)->SetEnabled(TRUE);
    mLockPatternView->ClearPattern();

    // if the user is currently locked out, enforce it.
    Int64 deadline;
    mLockPatternUtils->GetLockoutAttemptDeadline(&deadline);
    if (deadline != 0) {
        HandleAttemptLockout(deadline);
    }
    else {
        DisplayDefaultSecurityMessage();
    }
    return NOERROR;
}

void CKeyguardPatternView::DisplayDefaultSecurityMessage()
{
    Boolean res;
    if (mKeyguardUpdateMonitor->GetMaxBiometricUnlockAttemptsReached(&res), res) {
        mSecurityMessageDisplay->SetMessage(R::string::faceunlock_multiple_failures, TRUE);
    } else {
        mSecurityMessageDisplay->SetMessage(R::string::kg_pattern_instructions, FALSE);
    }
}

ECode CKeyguardPatternView::ShowUsabilityHint()
{
    return NOERROR;
}

ECode CKeyguardPatternView::CleanUp()
{
    if (DEBUG) Logger::V(TAG, "Cleanup() called on %s", TO_CSTR(this));
    mLockPatternUtils = NULL;
    return mLockPatternView->SetOnPatternListener(NULL);
}

void CKeyguardPatternView::HandleAttemptLockout(
    /* [in] */ Int64 elapsedRealtimeDeadline)
{
    mLockPatternView->ClearPattern();
    IView::Probe(mLockPatternView)->SetEnabled(FALSE);
    Int64 elapsedRealtime = SystemClock::GetElapsedRealtime();

    mCountdownTimer = new MyCountDownTimer(this, elapsedRealtimeDeadline - elapsedRealtime, 1000);
    mCountdownTimer->Start();
}

ECode CKeyguardPatternView::NeedsInput(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

ECode CKeyguardPatternView::OnPause()
{
    if (mCountdownTimer != NULL) {
        mCountdownTimer->Cancel();
        mCountdownTimer = NULL;
    }
    return NOERROR;
}

ECode CKeyguardPatternView::OnResume(
    /* [in] */ Int32 reason)
{
    Reset();
    return NOERROR;
}

ECode CKeyguardPatternView::GetCallback(
    /* [out] */ IKeyguardSecurityCallback** callback)
{
    VALIDATE_NOT_NULL(callback)

    *callback = mCallback;
    REFCOUNT_ADD(*callback)
    return NOERROR;
}

ECode CKeyguardPatternView::ShowBouncer(
    /* [in] */ Int32 duration)
{
    return KeyguardSecurityViewHelper::ShowBouncer(mSecurityMessageDisplay, mEcaView, mBouncerFrame, duration);
}

ECode CKeyguardPatternView::HideBouncer(
    /* [in] */ Int32 duration)
{
    return KeyguardSecurityViewHelper::HideBouncer(mSecurityMessageDisplay, mEcaView, mBouncerFrame, duration);
}

ECode CKeyguardPatternView::StartAppearAnimation()
{
    EnableClipping(FALSE);
    SetAlpha(1.0f);
    Float translation;
    mAppearAnimationUtils->GetStartTranslation(&translation);
    SetTranslationY(translation);

    AutoPtr<IViewPropertyAnimator> animator;
    Animate((IViewPropertyAnimator**)&animator);
    animator->SetDuration(500);
    AutoPtr<IInterpolator> interpolator;
    mAppearAnimationUtils->GetInterpolator((IInterpolator**)&interpolator);
    animator->SetInterpolator(ITimeInterpolator::Probe(interpolator));
    animator->TranslationY(0);

    AutoPtr<ArrayOf<IArrayOf*> > states;
    mLockPatternView->GetCellStates((ArrayOf<IArrayOf*>**)&states);

    Int32 size = states->GetLength();
    AutoPtr<ArrayOf<ArrayOf<IInterface*>* > > array =
            ArrayOf<ArrayOf<IInterface*>* >::Alloc(size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IArrayOf> inarray1 = (*states)[i];
        Int32 length;
        inarray1->GetLength(&length);
        AutoPtr<ArrayOf<IInterface*> > inarray2 = ArrayOf<IInterface*>::Alloc(length);
        for (Int32 j = 0; j < length; j++) {
            AutoPtr<IInterface> obj;
            inarray1->Get(j, (IInterface**)&obj);
            inarray2->Set(j, obj);
        }
        array->Set(i, inarray2);
    }

    AutoPtr<IRunnable> r = new MyRunnable2(this);
    mAppearAnimationUtils->StartAppearAnimation(array, r, this);

    AutoPtr<ICharSequence> text;
    ITextView::Probe(mHelpMessage)->GetText((ICharSequence**)&text);
    if (!TextUtils::IsEmpty(text)) {
        Float translation;
        mAppearAnimationUtils->GetStartTranslation(&translation);
        AutoPtr<IInterpolator> interpolator;
        mAppearAnimationUtils->GetInterpolator((IInterpolator**)&interpolator);

        mAppearAnimationUtils->CreateAnimation(mHelpMessage, 0,
                AppearAnimationUtils::DEFAULT_APPEAR_DURATION,
                translation,
                interpolator,
                NULL /* finishRunnable */);
    }
    return NOERROR;
}

ECode CKeyguardPatternView::StartDisappearAnimation(
    /* [in] */ IRunnable* finishRunnable,
    /* [out] */ Boolean* result)
{
    mLockPatternView->ClearPattern();
    AutoPtr<IViewPropertyAnimator> animator;
    Animate((IViewPropertyAnimator**)&animator);

    animator->Alpha(0.0f);
    animator->TranslationY(mDisappearYTranslation);

    AutoPtr<IInterpolator> interpolator;
    AnimationUtils::LoadInterpolator(
            mContext, Elastos::Droid::R::interpolator::fast_out_linear_in,
            (IInterpolator**)&interpolator);
    animator->SetInterpolator(ITimeInterpolator::Probe(interpolator));
    animator->SetDuration(100);
    animator->WithEndAction(finishRunnable);
    *result = TRUE;
    return NOERROR;
}

void CKeyguardPatternView::EnableClipping(
    /* [in] */ Boolean enable)
{
    SetClipChildren(enable);
    mKeyguardBouncerFrame->SetClipToPadding(enable);
    mKeyguardBouncerFrame->SetClipChildren(enable);
}

ECode CKeyguardPatternView::CreateAnimation(
    /* [in] */ IInterface* animatedCell,
    /* [in] */ Int64 delay,
    /* [in] */ Int64 duration,
    /* [in] */ Float startTranslationY,
    /* [in] */ IInterpolator* interpolator,
    /* [in] */ IRunnable* finishListener)
{
    ILockPatternViewCellState* _animatedCell =
            ILockPatternViewCellState::Probe(animatedCell);
    _animatedCell->SetScale(0.0f);
    _animatedCell->SetTranslateY(startTranslationY);

    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(2);
    (*array)[0] = startTranslationY;
    (*array)[1] = 0.0f;
    AutoPtr<IValueAnimator> animator;
    helper->OfFloat(array, (IValueAnimator**)&animator);
    IAnimator::Probe(animator)->SetInterpolator(ITimeInterpolator::Probe(interpolator));
    animator->SetDuration(duration);
    IAnimator::Probe(animator)->SetStartDelay(delay);

    AutoPtr<IAnimatorUpdateListener> lis = new MyAnimatorUpdateListener(this,
            _animatedCell);
    animator->AddUpdateListener(lis);

    if (finishListener != NULL) {
        AutoPtr<IAnimatorListener> lis = new MyAnimatorListenerAdapter(finishListener);
        IAnimator::Probe(animator)->AddListener(lis);

        // Also animate the Emergency call
        mAppearAnimationUtils->CreateAnimation(mEcaView, delay, duration, startTranslationY,
                interpolator, NULL);
    }
    IAnimator::Probe(animator)->Start();
    return IView::Probe(mLockPatternView)->Invalidate();
}

ECode CKeyguardPatternView::HasOverlappingRendering(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = FALSE;
    return NOERROR;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
