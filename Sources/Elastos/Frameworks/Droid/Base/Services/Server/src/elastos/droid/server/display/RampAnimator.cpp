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

#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Utility.h>
#include <Elastos.Droid.Animation.h>
#include "elastos/droid/server/display/RampAnimator.h"
#include <elastos/core/Math.h>

using Elastos::Droid::Animation::IValueAnimatorHelper;
using Elastos::Droid::Animation::CValueAnimatorHelper;
using Elastos::Droid::View::IChoreographerHelper;
using Elastos::Droid::View::CChoreographerHelper;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

RampAnimator::MyRunnable::MyRunnable(
    /* [in] */ RampAnimator* host)
    : mHost(host)
{}

ECode RampAnimator::MyRunnable::Run()
{
    using Elastos::Core::Math;
    Int64 frameTimeNanos;
    mHost->mChoreographer->GetFrameTimeNanos(&frameTimeNanos);
    Float timeDelta = (frameTimeNanos - mHost->mLastFrameTimeNanos)
            * 0.000000001f;
    mHost->mLastFrameTimeNanos = frameTimeNanos;

    // Advance the animated value towards the target at the specified rate
    // and clamp to the target. This gives us the new current value but
    // we keep the animated value around to allow for fractional increments
    // towards the target.
    AutoPtr<IValueAnimatorHelper> helper;
    CValueAnimatorHelper::AcquireSingleton((IValueAnimatorHelper**)&helper);
    Float scale;
    helper->GetDurationScale(&scale);
    if (scale == 0) {
        // Animation off.
        mHost->mAnimatedValue = mHost->mTargetValue;
    }
    else {
        Float amount = timeDelta * mHost->mRate / scale;
        if (mHost->mTargetValue > mHost->mCurrentValue) {
            mHost->mAnimatedValue = Math::Min(
                mHost->mAnimatedValue + amount,
                (Float)mHost->mTargetValue);
        }
        else {
            mHost->mAnimatedValue = Math::Max(
                mHost->mAnimatedValue - amount,
                (Float)mHost->mTargetValue);
        }
    }
    Int32 oldCurrentValue = mHost->mCurrentValue;
    mHost->mCurrentValue = Math::Round(mHost->mAnimatedValue);

    if (oldCurrentValue != mHost->mCurrentValue) {
        mHost->mProperty->SetValue(mHost->mObject, mHost->mCurrentValue);
    }

    if (mHost->mTargetValue != mHost->mCurrentValue) {
        mHost->PostAnimationCallback();
    } else {
        mHost->mAnimating = FALSE;
        if (mHost->mListener != NULL) {
            mHost->mListener->OnAnimationEnd();
        }
    }
    return NOERROR;
}

//==================================================================
// RampAnimator
//==================================================================
RampAnimator::RampAnimator(
    /* [in] */ IInterface* object,
    /* [in] */ IInt32Property* property)
    : mCurrentValue(0)
    , mTargetValue(0)
    , mRate(0)
    , mAnimating(FALSE)
    , mAnimatedValue(0)
    , mLastFrameTimeNanos(0)
    , mFirstTime(TRUE)
{
    mAnimationCallback = new MyRunnable(this);
    mObject = object;
    mProperty = property;
    AutoPtr<IChoreographerHelper> helper;
    CChoreographerHelper::AcquireSingleton((IChoreographerHelper**)&helper);
    helper->GetInstance((IChoreographer**)&mChoreographer);
}

Boolean RampAnimator::AnimateTo(
    /* [in] */ Int32 target,
    /* [in] */ Int32 rate)
{
    // Immediately jump to the target the first time.
    if (mFirstTime || rate <= 0) {
        if (mFirstTime || target != mCurrentValue) {
            mFirstTime = FALSE;
            mRate = 0;
            mTargetValue = target;
            mCurrentValue = target;
            mProperty->SetValue(mObject, target);
            if (mAnimating) {
                mAnimating = FALSE;
                CancelAnimationCallback();
            }
            if (mListener != NULL) {
                mListener->OnAnimationEnd();
            }
            return TRUE;
        }
        return FALSE;
    }

    // Adjust the rate based on the closest target.
    // If a faster rate is specified, then use the new rate so that we converge
    // more rapidly based on the new request.
    // If a slower rate is specified, then use the new rate only if the current
    // value is somewhere in between the new and the old target meaning that
    // we will be ramping in a different direction to get there.
    // Otherwise, continue at the previous rate.
    if (!mAnimating
            || rate > mRate
            || (target <= mCurrentValue && mCurrentValue <= mTargetValue)
            || (mTargetValue <= mCurrentValue && mCurrentValue <= target)) {
        mRate = rate;
    }

    Boolean changed = (mTargetValue != target);
    mTargetValue = target;

    // Start animating.
    if (!mAnimating && target != mCurrentValue) {
        mAnimating = TRUE;
        mAnimatedValue = mCurrentValue;
        AutoPtr<ISystem> system;
        CSystem::AcquireSingleton((ISystem**)&system);
        system->GetNanoTime(&mLastFrameTimeNanos);
        PostAnimationCallback();
    }

    return changed;
}

Boolean RampAnimator::IsAnimating()
{
    return mAnimating;
}

void RampAnimator::SetListener(
    /* [in] */ IRampAnimatorListener* listener)
{
    mListener = listener;
}

void RampAnimator::PostAnimationCallback()
{
    mChoreographer->PostCallback(IChoreographer::CALLBACK_ANIMATION, mAnimationCallback, NULL);
}

void RampAnimator::CancelAnimationCallback()
{
    mChoreographer->RemoveCallbacks(IChoreographer::CALLBACK_ANIMATION, mAnimationCallback, NULL);
}


} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos
