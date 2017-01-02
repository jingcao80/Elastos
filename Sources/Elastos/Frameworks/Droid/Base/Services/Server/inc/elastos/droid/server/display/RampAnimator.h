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

#ifndef __ELASTOS_DROID_SERVER_DISPLAY_RAMPANIMATOR_H__
#define __ELASTOS_DROID_SERVER_DISPLAY_RAMPANIMATOR_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Runnable.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Animation::IValueAnimator;
using Elastos::Droid::Utility::IInt32Property;
using Elastos::Droid::View::IChoreographer;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Display {

/**
 * A custom animator that progressively updates a property value at
 * a given variable rate until it reaches a particular target value.
 */
class RampAnimator
    : public Object
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ RampAnimator* host);

        // @Override // Choreographer callback
        CARAPI Run();
    private:
        RampAnimator* mHost;
    };

public:
    RampAnimator(
        /* [in] */ IInterface* object,
        /* [in] */ IInt32Property* property);

    /**
     * Starts animating towards the specified value.
     *
     * If this is the first time the property is being set or if the rate is 0,
     * the value jumps directly to the target.
     *
     * @param target The target value.
     * @param rate The convergence rate in units per second, or 0 to set the value immediately.
     * @return True if the target differs from the previous target.
     */
    Boolean AnimateTo(
        /* [in] */ Int32 target,
        /* [in] */ Int32 rate);

    /**
     * Returns TRUE if the animation is running.
     */
    Boolean IsAnimating();

    /**
     * Sets a listener to watch for animation events.
     */
    void SetListener(
        /* [in] */ IRampAnimatorListener* listener);

private:
    void PostAnimationCallback();

    void CancelAnimationCallback();

private:
    AutoPtr<IRunnable> mAnimationCallback;

    AutoPtr<IInterface> mObject;
    AutoPtr<IInt32Property> mProperty;
    AutoPtr<IChoreographer> mChoreographer;

    Int32 mCurrentValue;
    Int32 mTargetValue;
    Int32 mRate;

    Boolean mAnimating;
    Float mAnimatedValue; // higher precision copy of mCurrentValue
    Int64 mLastFrameTimeNanos;

    Boolean mFirstTime;

    AutoPtr<IRampAnimatorListener> mListener;
};

} // namespace Display
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_DISPLAY_RAMPANIMATOR_H__
