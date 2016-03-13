
#include "elastos/droid/launcher2/InterruptibleInOutAnimator.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

InterruptibleInOutAnimator::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ InterruptibleInOutAnimator* host)
    : mHost(host)
{
}

CARAPI InterruptibleInOutAnimator::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mDirection = STOPPED;
    return NOERROR;
}

const Int32 InterruptibleInOutAnimator::STOPPED = 0;
const Int32 InterruptibleInOutAnimator::IN = 1;
const Int32 InterruptibleInOutAnimator::OUT = 2;

InterruptibleInOutAnimator::InterruptibleInOutAnimator(
    /* [in] */ IView* view,
    /* [in] */ Int64 duration,
    /* [in] */ Float fromValue,
    /* [in] */ Float toValue)
    : mOriginalDuration(0)
    , mOriginalFromValue(0.0f)
    , mOriginalToValue(0.0f)
    , mFirstRun(TRUE)
    , mDirection(STOPPED)
{
    mAnimator = LauncherAnimUtils::OfFloat(view, fromValue, toValue);
    mAnimator->SetDuration(duration);
    mOriginalDuration = duration;
    mOriginalFromValue = fromValue;
    mOriginalToValue = toValue;

    AutoPtr<IAnimatorListener> listener = new MyAnimatorListenerAdapter(this);
    IAnimator::Probe(mAnimator)->AddListener(listener);
}

ECode InterruptibleInOutAnimator::Animate(
    /* [in] */ Int32 direction)
{
    Int64 currentPlayTime;
    mAnimator->GetCurrentPlayTime(&currentPlayTime);
    Float toValue = (direction == IN) ? mOriginalToValue : mOriginalFromValue;
    Float startValue;
    if (mFirstRun) {
        startValue = mOriginalFromValue;
    }
    else {
        AutoPtr<IInterface> value;
        mAnimator->GetAnimatedValue((IInterface**)&value);
        AutoPtr<IFloat> obj = IFloat::Probe(value);
        obj->GetValue(&startValue);
    }

    // Make sure it's stopped before we modify any values
    Cancel();

    // TODO: We don't really need to do the animation if startValue == toValue, but
    // somehow that doesn't seem to work, possibly a quirk of the animation framework
    mDirection = direction;

    // Ensure we don't calculate a non-sensical duration
    Int64 duration = mOriginalDuration - currentPlayTime;
    mAnimator->SetDuration(Math::Max(0, Math::Min(duration, mOriginalDuration)));

    mAnimator->SetFloatValues(startValue, toValue);
    mAnimator->Start();
    mFirstRun = FALSE;
    return NOERROR;
}

ECode InterruptibleInOutAnimator::Cancel()
{
    mAnimator->Cancel();
    mDirection = STOPPED;
    return NOERROR;
}

ECode InterruptibleInOutAnimator::End()
{
    mAnimator->End();
    mDirection = STOPPED;
    return NOERROR;
}

Boolean InterruptibleInOutAnimator::IsStopped()
{
    return mDirection == STOPPED;
}

ECode InterruptibleInOutAnimator::AnimateIn()
{
    return Animate(IN);
}

ECode InterruptibleInOutAnimator::AnimateOut()
{
    return Animate(OUT);
}

ECode InterruptibleInOutAnimator::SetTag(
    /* [in] */ IInterface* tag)
{
    mTag = tag;
    return NOERROR;
}

AutoPtr<IInterface> InterruptibleInOutAnimator::GetTag()
{
    return mTag;
}

AutoPtr<IValueAnimator> InterruptibleInOutAnimator::GetAnimator()
{
    return mAnimator;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos