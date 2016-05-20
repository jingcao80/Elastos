
#include "elastos/droid/animation/TimeAnimator.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(TimeAnimator, ValueAnimator, ITimeAnimator)
TimeAnimator::TimeAnimator()
    : mPreviousTime(-1)
{
}

ECode TimeAnimator::constructor()
{
    return NOERROR;
}

TimeAnimator::~TimeAnimator()
{}

ECode TimeAnimator::Start()
{
    mPreviousTime = -1;
    return ValueAnimator::Start();
}

Boolean TimeAnimator::AnimationFrame(
    /* [in] */ Int64 currentTime)
{
    if (mListener != NULL) {
        Int64 totalTime = currentTime - mStartTime;
        Int64 deltaTime = (mPreviousTime < 0) ? 0 : (currentTime - mPreviousTime);
        mPreviousTime = currentTime;
        mListener->OnTimeUpdate(this, totalTime, deltaTime);
    }
    return FALSE;
}

ECode TimeAnimator::SetTimeListener(
    /* [in] */ ITimeListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode TimeAnimator::AnimateValue(
    /* [in] */ Float fraction)
{
    // Noop
    return NOERROR;
}

ECode TimeAnimator::InitAnimation()
{
    // noop
    return NOERROR;
}

ECode TimeAnimator::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)

    AutoPtr<ITimeAnimator> newObject = new TimeAnimator();
    CloneImpl(newObject);

    *object = newObject;
    REFCOUNT_ADD(*object);
    return NOERROR;
}

ECode TimeAnimator::CloneImpl(
    /* [in] */ ITimeAnimator* object)
{
    ValueAnimator::CloneImpl(IValueAnimator::Probe(object));

    TimeAnimator* temp = (TimeAnimator*)object;
    temp->mListener = mListener;
    temp->mPreviousTime = mPreviousTime;

    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
