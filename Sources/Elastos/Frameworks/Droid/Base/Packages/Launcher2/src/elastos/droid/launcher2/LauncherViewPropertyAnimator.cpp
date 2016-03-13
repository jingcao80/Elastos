
#include "elastos/droid/launcher2/LauncherViewPropertyAnimator.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(LauncherViewPropertyAnimator, Animator, IAnimatorListener);

LauncherViewPropertyAnimator::LauncherViewPropertyAnimator(
    /* [in] */ IView* target)
    : mTarget(target)
    , mTranslationX(0.0f)
    , mTranslationY(0.0f)
    , mScaleX(0.0f)
    , mScaleY(0.0f)
    , mRotationY(0.0f)
    , mAlpha(0.0f)
    , mStartDelay(0)
    , mDuration(0)
    , mRunning(FALSE)
{
    CArrayList::New((ArrayList**)&mListeners);
}

ECode LauncherViewPropertyAnimator::AddListener(
    /* [in] */ IAnimatorListener* listener)
{
    return mListeners->Add(TO_IINTERFACE(listener));
}

ECode LauncherViewPropertyAnimator::Cancel()
{
    if (mViewPropertyAnimator != NULL) {
        mViewPropertyAnimator->Cancel();
    }
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::Clone()
{
    //throw new RuntimeException("Not implemented");
    Slogger::E("LauncherViewPropertyAnimator", "Not implemented");
    return E_RUNTIME_EXCEPTION;
}

ECode LauncherViewPropertyAnimator::End()
{
    //throw new RuntimeException("Not implemented");
    Slogger::E("LauncherViewPropertyAnimator", "Not implemented");
    return E_RUNTIME_EXCEPTION;
}

ECode LauncherViewPropertyAnimator::GetDuration(
    /* [out] */Int64* duration)
{
    VALIDATR_NOT_NULL(duration);

    *duration = mDuration;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::GetListeners(
    /* [out, callee] */ ArrayOf<IAnimatorListener*>** listeners)
{
    VALIDATR_NOT_NULL(listeners);

    *listeners = mListeners;
    REFCOUNT_ADD(*listeners);
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::GetStartDelay(
    /* [out] */ Int64* delay)
{
    VALIDATR_NOT_NULL(delay);

    *delay = mStartDelay;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    Int32 size;
    mListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mListeners->Get(i, (IInterface**)&obj);
        AutoPtr<IAnimatorListener> listener = IAnimatorListener::Probe(obj);
        listener->OnAnimationCancel(this);
    }
    mRunning = FALSE;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    Int32 size;
    mListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mListeners->Get(i, (IInterface**)&obj);
        AutoPtr<IAnimatorListener> listener = IAnimatorListener::Probe(obj);
        listener->OnAnimationEnd(this);
    }
    mRunning = FALSE;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::OnAnimationRepeat(
    /* [in] */ IAnimator* animation)
{
    Int32 size;
    mListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mListeners->Get(i, (IInterface**)&obj);
        AutoPtr<IAnimatorListener> listener = IAnimatorListener::Probe(obj);
        listener->OnAnimationRepeat(this);
    }
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    // This is the first time we get a handle to the internal ValueAnimator
    // used by the ViewPropertyAnimator.
    mFirstFrameHelper->OnAnimationStart(animation);

    Int32 size;
    mListeners->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mListeners->Get(i, (IInterface**)&obj);
        AutoPtr<IAnimatorListener> listener = IAnimatorListener::Probe(obj);
        listener->OnAnimationStart(this);
    }
    mRunning = TRUE;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::IsRunning(
    /* [out] */ Boolean* running)
{
    VALIDATR_NOT_NULL(running);

    *running = mRunning;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::IsStarted(
    /* [out] */ Boolean* started)
{
    VALIDATR_NOT_NULL(started);

    *started = mViewPropertyAnimator != NULL;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::RemoveAllListeners()
{
    return mListeners->Clear();
}

ECode LauncherViewPropertyAnimator::RemoveListener(
    /* [in] */ IAnimatorListener* listener)
{
    return mListeners->Remove(listener);
}

ECode LauncherViewPropertyAnimator::SetDuration(
    /* [in] */ Int64 duration)
{
    mPropertiesToSet->Add(Properties::DURATION);
    mDuration = duration;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::SetInterpolator(
    /* [in] */ ITimeInterpolator* value)
{
    mPropertiesToSet->Add(Properties::INTERPOLATOR);
    mInterpolator = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::SetStartDelay(
    /* [in] */ Int64 startDelay)
{
    mPropertiesToSet->Add(Properties::START_DELAY);
    mStartDelay = startDelay;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::SetTarget(
    /* [in] */ IInterface* target)
{
    //throw new RuntimeException("Not implemented");
    Slogger::E("LauncherViewPropertyAnimator", "Not implemented");
    return E_RUNTIME_EXCEPTION;
}

ECode LauncherViewPropertyAnimator::SetupEndValues()
{
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::SetupStartValues()
{
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::Start()
{
    mTarget->Animate((IViewPropertyAnimator**)&mViewPropertyAnimator);

    // FirstFrameAnimatorHelper hooks itself up to the updates on the animator,
    // and then adjusts the play time to keep the first two frames jank-free
    mFirstFrameHelper = new FirstFrameAnimatorHelper(mViewPropertyAnimator, mTarget);

    Boolean res;
    if (mPropertiesToSet->Contains(Properties::TRANSLATION_X, &res), res) {
        mViewPropertyAnimator->TranslationX(mTranslationX);
    }
    if (mPropertiesToSet->Contains(Properties::TRANSLATION_Y, &res), res) {
        mViewPropertyAnimator->TranslationY(mTranslationY);
    }
    if (mPropertiesToSet->Contains(Properties::SCALE_X, &res), res) {
        mViewPropertyAnimator->ScaleX(mScaleX);
    }
    if (mPropertiesToSet->Contains(Properties::ROTATION_Y, &res), res) {
        mViewPropertyAnimator->RotationY(mRotationY);
    }
    if (mPropertiesToSet->Contains(Properties::SCALE_Y, &res), res) {
        mViewPropertyAnimator->ScaleY(mScaleY);
    }
    if (mPropertiesToSet->Contains(Properties::ALPHA, &res), res) {
        mViewPropertyAnimator->Alpha(mAlpha);
    }
    if (mPropertiesToSet->Contains(Properties::START_DELAY, &res), res) {
        mViewPropertyAnimator->SetStartDelay(mStartDelay);
    }
    if (mPropertiesToSet->Contains(Properties::DURATION, &res), res) {
        mViewPropertyAnimator->SetDuration(mDuration);
    }
    if (mPropertiesToSet->Contains(Properties::INTERPOLATOR, &res), res) {
        mViewPropertyAnimator->SetInterpolator(mInterpolator);
    }
    mViewPropertyAnimator->SetListener(this);
    mViewPropertyAnimator->Start();
    return LauncherAnimUtils->CancelOnDestroyActivity(this);
}

ECode LauncherViewPropertyAnimator::TranslationX(
    /* [in] */ Float value)
{
    mPropertiesToSet->Add(Properties::TRANSLATION_X);
    mTranslationX = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::TranslationY(
    /* [in] */ Float value)
{
    mPropertiesToSet->Add(Properties::TRANSLATION_Y);
    mTranslationY = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::ScaleX(
    /* [in] */ Float value)
{
    mPropertiesToSet->Add(Properties::SCALE_X);
    mScaleX = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::ScaleY(
    /* [in] */ Float value)
{
    mPropertiesToSet->Add(Properties::SCALE_Y);
    mScaleY = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::RotationY(
    /* [in] */ Float value)
{
    mPropertiesToSet->Add(Properties::ROTATION_Y);
    mRotationY = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::Alpha(
    /* [in] */ Float value)
{
    mPropertiesToSet->Add(Properties::ALPHA);
    mAlpha = value;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos