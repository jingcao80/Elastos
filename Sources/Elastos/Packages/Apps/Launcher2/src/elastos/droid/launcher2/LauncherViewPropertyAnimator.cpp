
#include "elastos/droid/launcher2/LauncherViewPropertyAnimator.h"
#include "elastos/droid/launcher2/LauncherAnimUtils.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Animation::EIID_IAnimatorListener;
using Elastos::Core::IInteger32;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CEnumSetHelper;
using Elastos::Utility::IEnumSetHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL_2(LauncherViewPropertyAnimator, Animator, ILauncherViewPropertyAnimator,
        IAnimatorListener);

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
    mPropertiesToSet.Clear();

    CArrayList::New((IArrayList**)&mListeners);
}

ECode LauncherViewPropertyAnimator::AddListener(
    /* [in] */ IAnimatorListener* listener)
{
    return mListeners->Add(listener);
}

ECode LauncherViewPropertyAnimator::Cancel()
{
    if (mViewPropertyAnimator != NULL) {
        mViewPropertyAnimator->Cancel();
    }
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::Clone(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);

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
    VALIDATE_NOT_NULL(duration);

    *duration = mDuration;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::GetListeners(
    /* [out, callee] */ IArrayList** listeners)
{
    VALIDATE_NOT_NULL(listeners);

    *listeners = mListeners;
    REFCOUNT_ADD(*listeners);
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::GetStartDelay(
    /* [out] */ Int64* delay)
{
    VALIDATE_NOT_NULL(delay);

    *delay = mStartDelay;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::OnAnimationCancel(
    /* [in] */ IAnimator* animation)
{
    Int32 size;
    for (Int32 i = 0; i < (mListeners->GetSize(&size), size); i++) {
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
    for (Int32 i = 0; i < (mListeners->GetSize(&size), size); i++) {
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
    for (Int32 i = 0; i < (mListeners->GetSize(&size), size); i++) {
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
    for (Int32 i = 0; i < (mListeners->GetSize(&size), size); i++) {
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
    VALIDATE_NOT_NULL(running);

    *running = mRunning;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::IsStarted(
    /* [out] */ Boolean* started)
{
    VALIDATE_NOT_NULL(started);

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
    mPropertiesToSet.Insert(LauncherViewPropertyAnimator_DURATION);
    mDuration = duration;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::SetInterpolator(
    /* [in] */ ITimeInterpolator* value)
{
    mPropertiesToSet.Insert(LauncherViewPropertyAnimator_INTERPOLATOR);
    mInterpolator = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::SetStartDelay(
    /* [in] */ Int64 startDelay)
{
    mPropertiesToSet.Insert(LauncherViewPropertyAnimator_START_DELAY);
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
    mViewPropertyAnimator = NULL;
    mTarget->Animate((IViewPropertyAnimator**)&mViewPropertyAnimator);

    // FirstFrameAnimatorHelper hooks itself up to the updates on the animator,
    // and then adjusts the play time to keep the first two frames jank-free
    mFirstFrameHelper = new FirstFrameAnimatorHelper(mViewPropertyAnimator, mTarget);

    if (mPropertiesToSet.Find(LauncherViewPropertyAnimator_TRANSLATION_X) != mPropertiesToSet.End()) {
        mViewPropertyAnimator->TranslationX(mTranslationX);
    }

    if (mPropertiesToSet.Find(LauncherViewPropertyAnimator_TRANSLATION_Y) != mPropertiesToSet.End()) {
        mViewPropertyAnimator->TranslationY(mTranslationY);
    }

    if (mPropertiesToSet.Find(LauncherViewPropertyAnimator_SCALE_X) != mPropertiesToSet.End()) {
        mViewPropertyAnimator->ScaleX(mScaleX);
    }

    if (mPropertiesToSet.Find(LauncherViewPropertyAnimator_ROTATION_Y) != mPropertiesToSet.End()) {
        mViewPropertyAnimator->RotationY(mRotationY);
    }

    if (mPropertiesToSet.Find(LauncherViewPropertyAnimator_SCALE_Y) != mPropertiesToSet.End()) {
        mViewPropertyAnimator->ScaleY(mScaleY);
    }

    if (mPropertiesToSet.Find(LauncherViewPropertyAnimator_ALPHA) != mPropertiesToSet.End()) {
        mViewPropertyAnimator->Alpha(mAlpha);
    }

    if (mPropertiesToSet.Find(LauncherViewPropertyAnimator_START_DELAY) != mPropertiesToSet.End()) {
        mViewPropertyAnimator->SetStartDelay(mStartDelay);
    }

    if (mPropertiesToSet.Find(LauncherViewPropertyAnimator_DURATION) != mPropertiesToSet.End()) {
        mViewPropertyAnimator->SetDuration(mDuration);
    }

    if (mPropertiesToSet.Find(LauncherViewPropertyAnimator_INTERPOLATOR) != mPropertiesToSet.End()) {
        mViewPropertyAnimator->SetInterpolator(mInterpolator);
    }

    mViewPropertyAnimator->SetListener(this);
    mViewPropertyAnimator->Start();
    LauncherAnimUtils::CancelOnDestroyActivity(IAnimator::Probe(this));
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::TranslationX(
    /* [in] */ Float value)
{
    mPropertiesToSet.Insert(LauncherViewPropertyAnimator_TRANSLATION_X);
    mTranslationX = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::TranslationY(
    /* [in] */ Float value)
{
    mPropertiesToSet.Insert(LauncherViewPropertyAnimator_TRANSLATION_Y);
    mTranslationY = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::ScaleX(
    /* [in] */ Float value)
{
    mPropertiesToSet.Insert(LauncherViewPropertyAnimator_SCALE_X);
    mScaleX = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::ScaleY(
    /* [in] */ Float value)
{
    mPropertiesToSet.Insert(LauncherViewPropertyAnimator_SCALE_Y);
    mScaleY = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::RotationY(
    /* [in] */ Float value)
{
    mPropertiesToSet.Insert(LauncherViewPropertyAnimator_ROTATION_Y);
    mRotationY = value;
    return NOERROR;
}

ECode LauncherViewPropertyAnimator::Alpha(
    /* [in] */ Float value)
{
    mPropertiesToSet.Insert(LauncherViewPropertyAnimator_ALPHA);
    mAlpha = value;
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos