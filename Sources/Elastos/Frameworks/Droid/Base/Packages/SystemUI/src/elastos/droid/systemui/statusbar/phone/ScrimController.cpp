
#include "elastos/droid/systemui/statusbar/phone/ScrimController.h"
#include "elastos/droid/systemui/statusbar/phone/UnlockMethodCache.h"
#include "elastos/droid/systemui/doze/DozeLog.h"
#include "../../R.h"
#include "Elastos.Droid.Graphics.h"
#include <elastos/droid/R.h>
#include <elastos/droid/view/animation/AnimationUtils.h>
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::Animation::EIID_IAnimatorUpdateListener;
using Elastos::Droid::Animation::ITimeInterpolator;
using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Graphics::CColor;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::SystemUI::Doze::DozeLog;
using Elastos::Droid::View::Animation::AnimationUtils;
using Elastos::Droid::View::Animation::CDecelerateInterpolator;
using Elastos::Droid::View::EIID_IOnPreDrawListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewTreeObserver;
using Elastos::Core::IInteger32;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

const Int64 ScrimController::ANIMATION_DURATION = 220;
const String ScrimController::TAG("ScrimController");
const Boolean ScrimController::DEBUG = Logger::IsLoggable(TAG, Logger::___DEBUG);
const Float ScrimController::SCRIM_BEHIND_ALPHA = 0.62f;
const Float ScrimController::SCRIM_BEHIND_ALPHA_KEYGUARD = 0.55f;
const Float ScrimController::SCRIM_BEHIND_ALPHA_UNLOCKING = 0.2f;
const Float ScrimController::SCRIM_IN_FRONT_ALPHA = 0.75f;
const Int32 ScrimController::TAG_KEY_ANIM = R::id::scrim;

ScrimController::PulseIn::PulseIn(
    /* [in] */ ScrimController* host)
    : mHost(host)
{}

ECode ScrimController::PulseIn::Run()
{
    if (DEBUG) Logger::D(TAG, "Pulse in, mDozing=%d", mHost->mDozing);
    if (!mHost->mDozing) return NOERROR;
    DozeLog::TracePulseStart();
    mHost->mDurationOverride = mHost->mDozeParameters->GetPulseInDuration();
    mHost->mAnimationDelay = 0;
    mHost->mAnimateChange = TRUE;
    mHost->mOnAnimationFinished = mHost->mPulseInFinished;
    mHost->SetScrimColor(mHost->mScrimInFront, 0);

    // Signal that the pulse is ready to turn the screen on and draw.
    mHost->PulseStarted();
    return NOERROR;
}

ScrimController::PulseInFinished::PulseInFinished(
    /* [in] */ ScrimController* host)
    : mHost(host)
{}

ECode ScrimController::PulseInFinished::Run()
{
    if (DEBUG) Logger::D(TAG, "Pulse in finished, mDozing=%d", mHost->mDozing);
    if (!mHost->mDozing) return NOERROR;
    Boolean tmp = FALSE;
    IView::Probe(mHost->mScrimInFront)->PostDelayed(mHost->mPulseOut,
        mHost->mDozeParameters->GetPulseVisibleDuration(), &tmp);
    return NOERROR;
}

ScrimController::PulseOut::PulseOut(
    /* [in] */ ScrimController* host)
    : mHost(host)
{}

ECode ScrimController::PulseOut::Run()
{
    if (DEBUG) Logger::D(TAG, "Pulse out, mDozing=%d", mHost->mDozing);
    if (!mHost->mDozing) return NOERROR;
    mHost->mDurationOverride = mHost->mDozeParameters->GetPulseOutDuration();
    mHost->mAnimationDelay = 0;
    mHost->mAnimateChange = TRUE;
    mHost->mOnAnimationFinished = mHost->mPulseOutFinished;
    mHost->SetScrimColor(mHost->mScrimInFront, 1);
    return NOERROR;
}

ScrimController::PulseOutFinished::PulseOutFinished(
    /* [in] */ ScrimController* host)
    : mHost(host)
{}

ECode ScrimController::PulseOutFinished::Run()
{
    if (DEBUG) Logger::D(TAG, "Pulse out finished");
    DozeLog::TracePulseFinish();

    // Signal that the pulse is all finished so we can turn the screen off now.
    mHost->PulseFinished();
    return NOERROR;
}

CAR_INTERFACE_IMPL(ScrimController::AnimatorUpdateListener, Object, IAnimatorUpdateListener);
ScrimController::AnimatorUpdateListener::AnimatorUpdateListener(
    /* [in] */ IScrimView* scrim)
    : mScrim(scrim)
{}

ECode ScrimController::AnimatorUpdateListener::OnAnimationUpdate(
    /* [in] */ IValueAnimator* animation)
{
    AutoPtr<IInterface> obj;
    animation->GetAnimatedValue((IInterface**)&obj);
    Int32 value = 0;
    IInteger32::Probe(obj)->GetValue(&value);

    AutoPtr<IColor> helper;
    CColor::AcquireSingleton((IColor**)&helper);
    Int32 argb = 0;
    helper->Argb(value, 0, 0, 0, &argb);
    mScrim->SetScrimColor(argb);
    return NOERROR;
}

ScrimController::AnimatorListenerAdapter1::AnimatorListenerAdapter1(
    /* [in] */ ScrimController* host,
    /* [in] */ IScrimView* scrim)
    : mHost(host)
    , mScrim(scrim)
{}

ECode ScrimController::AnimatorListenerAdapter1::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    if (mHost->mOnAnimationFinished != NULL) {
        mHost->mOnAnimationFinished->Run();
        mHost->mOnAnimationFinished = NULL;
    }
    IView::Probe(mScrim)->SetTag(TAG_KEY_ANIM, NULL);
    return NOERROR;
}

ScrimController::Runnable1::Runnable1(
    /* [in] */ ScrimController* host)
    : mHost(host)
{}

ECode ScrimController::Runnable1::Run()
{
    mHost->UpdateScrimBehindDrawingMode();
    return NOERROR;
}

CAR_INTERFACE_IMPL_2(ScrimController, Object, IScrimController, IOnPreDrawListener);
ScrimController::ScrimController(
    /* [in] */ IScrimView* scrimBehind,
    /* [in] */ IScrimView* scrimInFront,
    /* [in] */ Boolean scrimSrcEnabled)
    : mKeyguardShowing(FALSE)
    , mFraction(0)
    , mDarkenWhileDragging(FALSE)
    , mBouncerShowing(FALSE)
    , mAnimateChange(FALSE)
    , mUpdatePending(FALSE)
    , mExpanding(FALSE)
    , mAnimateKeyguardFadingOut(FALSE)
    , mDurationOverride(-1)
    , mAnimationDelay(0)
    , mAnimationStarted(FALSE)
    , mDozing(FALSE)
    , mScrimSrcEnabled(FALSE)
{
    CDecelerateInterpolator::New((IInterpolator**)&mInterpolator);
    mPulseIn = new PulseIn(this);
    mPulseInFinished = new PulseInFinished(this);
    mPulseOut = new PulseOut(this);
    mPulseOutFinished = new PulseOutFinished(this);
    mScrimBehind = scrimBehind;
    mScrimInFront = scrimInFront;
    AutoPtr<IContext> context;
    IView::Probe(scrimBehind)->GetContext((IContext**)&context);
    mUnlockMethodCache = UnlockMethodCache::GetInstance(context);
    AnimationUtils::LoadInterpolator(context, Elastos::Droid::R::interpolator::linear_out_slow_in
                , (IInterpolator**)&mLinearOutSlowInInterpolator);
    mDozeParameters = new DozeParameters(context);
    mScrimSrcEnabled = scrimSrcEnabled;
}

ECode ScrimController::SetKeyguardShowing(
    /* [in] */ Boolean showing)
{
    mKeyguardShowing = showing;
    ScheduleUpdate();
    return NOERROR;
}

ECode ScrimController::OnTrackingStarted()
{
    mExpanding = TRUE;
    mDarkenWhileDragging = !mUnlockMethodCache->IsMethodInsecure();
    return NOERROR;
}

ECode ScrimController::OnExpandingFinished()
{
    mExpanding = FALSE;
    return NOERROR;
}

ECode ScrimController::SetPanelExpansion(
    /* [in] */ Float fraction)
{
    if (mFraction != fraction) {
        mFraction = fraction;
        ScheduleUpdate();
    }
    return NOERROR;
}

ECode ScrimController::SetBouncerShowing(
    /* [in] */ Boolean showing)
{
    mBouncerShowing = showing;
    mAnimateChange = !mExpanding;
    ScheduleUpdate();
    return NOERROR;
}

ECode ScrimController::AnimateKeyguardFadingOut(
    /* [in] */ Int64 delay,
    /* [in] */ Int64 duration,
    /* [in] */ IRunnable* onAnimationFinished)
{
    mAnimateKeyguardFadingOut = TRUE;
    mDurationOverride = duration;
    mAnimationDelay = delay;
    mAnimateChange = TRUE;
    mOnAnimationFinished = onAnimationFinished;
    ScheduleUpdate();
    return NOERROR;
}

ECode ScrimController::AnimateGoingToFullShade(
    /* [in] */ Int64 delay,
    /* [in] */ Int64 duration)
{
    mDurationOverride = duration;
    mAnimationDelay = delay;
    mAnimateChange = TRUE;
    ScheduleUpdate();
    return NOERROR;
}

ECode ScrimController::SetDozing(
    /* [in] */ Boolean dozing)
{
    if (mDozing == dozing) return NOERROR;
    mDozing = dozing;
    if (!mDozing) {
        CancelPulsing();
        mAnimateChange = TRUE;
    }
    else {
        mAnimateChange = FALSE;
    }
    ScheduleUpdate();
    return NOERROR;
}

ECode ScrimController::Pulse(
    /* [in] */ /*@NonNull*/ IDozeHostPulseCallback* callback)
{
    if (callback == NULL) {
        // throw new IllegalArgumentException("callback must not be NULL");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (!mDozing || mPulseCallback != NULL) {
        // Pulse suppressed.
        callback->OnPulseFinished();
        return NOERROR;
    }

    // Begin pulse.  Note that it's very important that the pulse finished callback
    // be invoked when we're done so that the caller can drop the pulse wakelock.
    mPulseCallback = callback;
    Boolean tmp = FALSE;
    IView::Probe(mScrimInFront)->Post(mPulseIn, &tmp);
    return NOERROR;
}

ECode ScrimController::IsPulsing(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPulseCallback != NULL;
    return NOERROR;
}

void ScrimController::CancelPulsing()
{
    if (DEBUG) Logger::D(TAG, "Cancel pulsing");

    if (mPulseCallback != NULL) {
        Boolean tmp = FALSE;
        IView::Probe(mScrimInFront)->RemoveCallbacks(mPulseIn, &tmp);
        IView::Probe(mScrimInFront)->RemoveCallbacks(mPulseOut, &tmp);
        PulseFinished();
    }
}

void ScrimController::PulseStarted()
{
    if (mPulseCallback != NULL) {
        mPulseCallback->OnPulseStarted();
    }
}

void ScrimController::PulseFinished()
{
    if (mPulseCallback != NULL) {
        mPulseCallback->OnPulseFinished();
        mPulseCallback = NULL;
    }
}

void ScrimController::ScheduleUpdate()
{
    if (mUpdatePending) return;

    // Make sure that a frame gets scheduled.
    IView::Probe(mScrimBehind)->Invalidate();

    AutoPtr<IViewTreeObserver> vto;
    IView::Probe(mScrimBehind)->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->AddOnPreDrawListener(this);
    mUpdatePending = TRUE;
}

void ScrimController::UpdateScrims()
{
    if (mAnimateKeyguardFadingOut) {
        SetScrimInFrontColor(0.f);
        SetScrimBehindColor(0.f);
    }
    else if (!mKeyguardShowing && !mBouncerShowing) {
        UpdateScrimNormal();
        SetScrimInFrontColor(0);
    }
    else {
        UpdateScrimKeyguard();
    }
    mAnimateChange = FALSE;
}

void ScrimController::UpdateScrimKeyguard()
{
    if (mExpanding && mDarkenWhileDragging) {
        Float behindFraction = Elastos::Core::Math::Max((Float)0, Elastos::Core::Math::Min(mFraction, (Float)1));
        Float fraction = 1 - behindFraction;
        fraction = (Float) Elastos::Core::Math::Pow(fraction, 0.8f);
        behindFraction = (Float) Elastos::Core::Math::Pow(behindFraction, 0.8f);
        SetScrimInFrontColor(fraction * SCRIM_IN_FRONT_ALPHA);
        SetScrimBehindColor(behindFraction * SCRIM_BEHIND_ALPHA_KEYGUARD);
    }
    else if (mBouncerShowing) {
        SetScrimInFrontColor(SCRIM_IN_FRONT_ALPHA);
        SetScrimBehindColor(0.f);
    }
    else if (mDozing) {
        SetScrimInFrontColor(1);
    }
    else {
        Float fraction = Elastos::Core::Math::Max((Float)0, Elastos::Core::Math::Min(mFraction, (Float)1));
        SetScrimInFrontColor(0.f);
        SetScrimBehindColor(fraction
                * (SCRIM_BEHIND_ALPHA_KEYGUARD - SCRIM_BEHIND_ALPHA_UNLOCKING)
                + SCRIM_BEHIND_ALPHA_UNLOCKING);
    }
}

void ScrimController::UpdateScrimNormal()
{
    Float frac = mFraction;
    // let's start this 20% of the way down the screen
    frac = frac * 1.2f - 0.2f;
    if (frac <= 0) {
        SetScrimBehindColor(0);
    }
    else {
        // woo, special effects
        const Float k = (Float)(1.f - 0.5f *
                (1.f - Elastos::Core::Math::Cos(3.14159f * Elastos::Core::Math::Pow(1.f-frac, 2.f))));
        SetScrimBehindColor(k * SCRIM_BEHIND_ALPHA);
    }
}

void ScrimController::SetScrimBehindColor(
    /* [in] */ Float alpha)
{
    SetScrimColor(mScrimBehind, alpha);
}

void ScrimController::SetScrimInFrontColor(
    /* [in] */ Float alpha)
{
    SetScrimColor(mScrimInFront, alpha);
    if (alpha == 0.f) {
        IView::Probe(mScrimInFront)->SetClickable(FALSE);
    }
    else {
        // Eat touch events (unless dozing).
        IView::Probe(mScrimInFront)->SetClickable(!mDozing);
    }
}

void ScrimController::SetScrimColor(
    /* [in] */ IScrimView* scrim,
    /* [in] */ Float alpha)
{
    AutoPtr<IInterface> runningAnim;
    IView::Probe(scrim)->GetTag(TAG_KEY_ANIM, (IInterface**)&runningAnim);
    if (IValueAnimator::Probe(runningAnim)) {
        IAnimator::Probe(runningAnim)->Cancel();
        IView::Probe(scrim)->SetTag(TAG_KEY_ANIM, NULL);
    }

    AutoPtr<IColor> helper;
    CColor::AcquireSingleton((IColor**)&helper);
    Int32 color = 0;
    helper->Argb((Int32) (alpha * 255), 0, 0, 0, &color);
    if (mAnimateChange) {
        StartScrimAnimation(scrim, color);
    }
    else {
        scrim->SetScrimColor(color);
    }
}

void ScrimController::StartScrimAnimation(
    /* [in] */ IScrimView* scrim,
    /* [in] */ Int32 targetColor)
{
    AutoPtr<IColor> color;
    CColor::AcquireSingleton((IColor**)&color);
    Int32 v = 0;
    scrim->GetScrimColor(&v);
    Int32 current = 0;
    color->Alpha(v, &current);
    Int32 target = 0;
    color->Alpha(targetColor, &target);
    if (current == targetColor) {
        return;
    }
    AutoPtr<IValueAnimator> anim;
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    AutoPtr<ArrayOf<Int32> > ivs = ArrayOf<Int32>::Alloc(2);
    (*ivs)[0] = current;
    (*ivs)[1] = target;
    helper->OfInt32(ivs, (IValueAnimator**)&anim);

    AutoPtr<AnimatorUpdateListener> ul = new AnimatorUpdateListener(scrim);
    anim->AddUpdateListener(ul);
    IAnimator::Probe(anim)->SetInterpolator(mAnimateKeyguardFadingOut
            ? ITimeInterpolator::Probe(mLinearOutSlowInInterpolator)
            : ITimeInterpolator::Probe(mInterpolator));
    IAnimator::Probe(anim)->SetStartDelay(mAnimationDelay);
    anim->SetDuration(mDurationOverride != -1 ? mDurationOverride : ANIMATION_DURATION);

    AutoPtr<AnimatorListenerAdapter1> la = new AnimatorListenerAdapter1(this, scrim);
    IAnimator::Probe(anim)->AddListener(la);
    IAnimator::Probe(anim)->Start();
    IView::Probe(scrim)->SetTag(TAG_KEY_ANIM, anim);
    mAnimationStarted = TRUE;
}

ECode ScrimController::OnPreDraw(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IViewTreeObserver> vto;
    IView::Probe(mScrimBehind)->GetViewTreeObserver((IViewTreeObserver**)&vto);
    vto->RemoveOnPreDrawListener(this);
    mUpdatePending = FALSE;
    UpdateScrims();
    mAnimateKeyguardFadingOut = FALSE;
    mDurationOverride = -1;
    mAnimationDelay = 0;

    // Make sure that we always call the listener even if we didn't start an animation.
    if (!mAnimationStarted && mOnAnimationFinished != NULL) {
        mOnAnimationFinished->Run();
        mOnAnimationFinished = NULL;
    }
    mAnimationStarted = FALSE;
    *result = TRUE;
    return NOERROR;
}

ECode ScrimController::SetBackDropView(
    /* [in] */ IBackDropView* backDropView)
{
    mBackDropView = backDropView;
    AutoPtr<Runnable1> run = new Runnable1(this);
    mBackDropView->SetOnVisibilityChangedRunnable(run);
    UpdateScrimBehindDrawingMode();
    return NOERROR;
}

void ScrimController::UpdateScrimBehindDrawingMode()
{
    Int32 v = 0;
    IView::Probe(mBackDropView)->GetVisibility(&v);
    Boolean asSrc = v != IView::VISIBLE && mScrimSrcEnabled;
    mScrimBehind->SetDrawAsSrc(asSrc);
}

} // namespace Phone
} // namespace StatusBar
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
