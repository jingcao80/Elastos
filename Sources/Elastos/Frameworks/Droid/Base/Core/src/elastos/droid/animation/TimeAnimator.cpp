//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

    AutoPtr<ITimeAnimator> newObject = new CTimeAnimator();
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
