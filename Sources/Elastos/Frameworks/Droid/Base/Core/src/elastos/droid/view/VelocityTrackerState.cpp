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

#include "elastos/droid/view/VelocityTrackerState.h"

namespace Elastos {
namespace Droid {
namespace View {

const Int32 VelocityTrackerState::ACTIVE_POINTER_ID;

VelocityTrackerState::VelocityTrackerState(
    /* [in] */ const char* strategy)
    : mVelocityTracker(strategy)
    , mActivePointerId(-1)
{
}

VelocityTrackerState::~VelocityTrackerState()
{
}

void VelocityTrackerState::Clear()
{
    mVelocityTracker.clear();
    mActivePointerId = -1;
    mCalculatedIdBits.clear();
}

void VelocityTrackerState::AddMovement(
    /* [in] */ const android::MotionEvent* event)
{
    mVelocityTracker.addMovement(event);
}

void VelocityTrackerState::ComputeCurrentVelocity(
    /* [in] */ Int32 units,
    /* [in] */ Float maxVelocity)
{
    BitSet32 idBits(mVelocityTracker.getCurrentPointerIdBits());
    mCalculatedIdBits = idBits;

    for (UInt32 index = 0; !idBits.isEmpty(); index++) {
        UInt32 id = idBits.clearFirstMarkedBit();

        Float vx, vy;
        mVelocityTracker.getVelocity(id, &vx, &vy);

        vx = vx * units / 1000;
        vy = vy * units / 1000;

        if (vx > maxVelocity) {
            vx = maxVelocity;
        } else if (vx < -maxVelocity) {
            vx = -maxVelocity;
        }
        if (vy > maxVelocity) {
            vy = maxVelocity;
        } else if (vy < -maxVelocity) {
            vy = -maxVelocity;
        }

        Velocity& velocity = mCalculatedVelocity[index];
        velocity.vx = vx;
        velocity.vy = vy;
    }
}

void VelocityTrackerState::GetVelocity(
    /* [in] */ Int32 id,
    /* [out] */ Float* outVx,
    /* [out] */ Float* outVy)
{
    if (id == ACTIVE_POINTER_ID) {
        id = mVelocityTracker.getActivePointerId();
    }

    Float vx, vy;
    if (id >= 0 && id <= MAX_POINTER_ID && mCalculatedIdBits.hasBit(id)) {
        UInt32 index = mCalculatedIdBits.getIndexOfBit(id);
        const Velocity& velocity = mCalculatedVelocity[index];
        vx = velocity.vx;
        vy = velocity.vy;
    } else {
        vx = 0;
        vy = 0;
    }

    if (outVx) {
        *outVx = vx;
    }
    if (outVy) {
        *outVy = vy;
    }
}

Boolean VelocityTrackerState::GetEstimator(
    /* [in] */ Int32 id,
    /* [in] */ android::VelocityTracker::Estimator* outEstimator)
{
    return mVelocityTracker.getEstimator(id, outEstimator);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
