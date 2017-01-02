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

#ifndef __ELASTOS_DROID_VIEW_VELOCITYTRACKERSTATE_H__
#define __ELASTOS_DROID_VIEW_VELOCITYTRACKERSTATE_H__

#include <input/Input.h>
#include <input/VelocityTracker.h>
#include <utils/BitSet.h>

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace View {

using android::BitSet32;

class ECO_PUBLIC VelocityTrackerState
    : public Object
{
public:
    VelocityTrackerState(
        /* [in] */ const char* strategy);

    virtual ~VelocityTrackerState();

    void Clear();

    void AddMovement(
        /* [in] */ const android::MotionEvent* event);

    void ComputeCurrentVelocity(
        /* [in] */ Int32 units,
        /* [in] */ Float maxVelocity);

    void GetVelocity(
        /* [in] */ Int32 id,
        /* [in] */ Float* outVx,
        /* [in] */ Float* outVy);

    Boolean GetEstimator(
        /* [in] */ Int32 id,
        /* [in] */ android::VelocityTracker::Estimator* outEstimator);

private:
    class Velocity {
    public:
        Float vx, vy;
    };

    android::VelocityTracker mVelocityTracker;
    Int32 mActivePointerId;
    BitSet32 mCalculatedIdBits;
    Velocity mCalculatedVelocity[MAX_POINTERS];

    ECO_LOCAL static const Int32 ACTIVE_POINTER_ID = -1;
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif
