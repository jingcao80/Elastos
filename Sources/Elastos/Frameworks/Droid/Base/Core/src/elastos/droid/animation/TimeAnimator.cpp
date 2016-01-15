
#include "elastos/droid/animation/TimeAnimator.h"
#include "elastos/droid/animation/CTimeAnimator.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(TimeAnimator, ValueAnimator, ITimeAnimator)
TimeAnimator::TimeAnimator()
    : mPreviousTime(-1)
{
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
        mListener->OnTimeUpdate((ITimeAnimator*)(this->Probe(EIID_ITimeAnimator)), totalTime, deltaTime);
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
    AutoPtr<CTimeAnimator> newObject;
    CTimeAnimator::NewByFriend((CTimeAnimator**)&newObject);
    CloneSuperData(newObject.Get());
    CloneInternal(newObject);
    TimeAnimator* temp = newObject;
    temp->mListener = mListener;
    temp->mPreviousTime = mPreviousTime;
    *object = newObject->Probe(EIID_ITimeAnimator);
    REFCOUNT_ADD(*object);
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
