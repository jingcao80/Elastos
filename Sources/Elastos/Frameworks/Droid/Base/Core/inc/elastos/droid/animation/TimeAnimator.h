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

#ifndef __ELASTOS_DROID_ANIMATION_TIMEANIMATOR_H__
#define __ELASTOS_DROID_ANIMATION_TIMEANIMATOR_H__

#include "elastos/droid/animation/ValueAnimator.h"

namespace Elastos {
namespace Droid {
namespace Animation {

class TimeAnimator
    : public ValueAnimator
    , public ITimeAnimator
{
public:
    CAR_INTERFACE_DECL()

    TimeAnimator();

    CARAPI constructor();

    virtual ~TimeAnimator();

    virtual CARAPI Start();

    virtual CARAPI_(Boolean) AnimationFrame(
        /* [in] */ Int64 currentTime);

    /**
     * Sets a listener that is sent update events throughout the life of
     * an animation.
     *
     * @param listener the listener to be set.
     */
    virtual CARAPI SetTimeListener(
        /* [in] */ ITimeListener* listener);

    //@Override
    virtual CARAPI AnimateValue(
        /* [in] */ Float fraction);

    //@Override
    virtual CARAPI InitAnimation();

    virtual CARAPI Clone(
        /* [out] */ IInterface** object);

protected:
    CARAPI CloneImpl(
        /* [in] */ ITimeAnimator* anim);

private:
    AutoPtr<ITimeListener> mListener;
    Int64 mPreviousTime;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_TIMEANIMATOR_H__
