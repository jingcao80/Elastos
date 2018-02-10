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

#include "elastos/droid/view/VelocityTracker.h"
#include "elastos/droid/view/GLES20Canvas.h"
#include "elastos/droid/view/VelocityTrackerState.h"
#include "elastos/droid/utility/Pools.h"

#include <elastos/core/Math.h>
#include <elastos/core/StringBuffer.h>
#include <stdio.h>

using Elastos::Droid::Utility::Pools;

namespace Elastos {
namespace Droid {
namespace View {

const Int32 VelocityTracker::ACTIVE_POINTER_ID = -1;
AutoPtr<Pools::SynchronizedPool<IVelocityTracker> > VelocityTracker::sPool =
        new Pools::SynchronizedPool<IVelocityTracker>(2);

//========================================================================================
//              VelocityTracker::Estimator
//========================================================================================
const Int32 VelocityTracker::Estimator::MAX_DEGREE = 4;

CAR_INTERFACE_IMPL(VelocityTracker::Estimator, Object, IEstimator)

VelocityTracker::Estimator::Estimator()
{
    mXCoeff = ArrayOf<Float>::Alloc(MAX_DEGREE + 1);
    mYCoeff = ArrayOf<Float>::Alloc(MAX_DEGREE + 1);
}

ECode VelocityTracker::Estimator::EstimateX(
    /* [in] */ Float time,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Estimate(time, mXCoeff);
    return NOERROR;
}

ECode VelocityTracker::Estimator::EstimateY(
    /* [in] */ Float time,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = Estimate(time, mYCoeff);
    return NOERROR;
}

ECode VelocityTracker::Estimator::GetXCoeff(
    /* [in] */ Int32 index,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = index <= mDegree ? (*mXCoeff)[index] : 0.0f;
    return NOERROR;
}

ECode VelocityTracker::Estimator::GetYCoeff(
    /* [in] */ Int32 index,
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    *result = index <= mDegree ? (*mYCoeff)[index] : 0.0f;
    return NOERROR;
}

Float VelocityTracker::Estimator::Estimate(
    /* [in] */ Float time,
    /* [in] */ ArrayOf<Float>* c)
{
    Float a = 0;
    Float scale = 1;
    for (Int32 i = 0; i <= mDegree; i++) {
        a += (*c)[i] * scale;
        scale *= time;
    }
    return a;
}

//========================================================================================
//              VelocityTracker::
//========================================================================================
AutoPtr<VelocityTrackerState> VelocityTracker::NativeInitialize(
    /* [in] */ const String& strategy)
{
    AutoPtr<VelocityTrackerState> temp = new VelocityTrackerState(strategy.string());
    return temp;
}

void VelocityTracker::NativeClear(
    /* [in] */ VelocityTrackerState* state)
{
    if (state)
        state->Clear();
}

void VelocityTracker::NativeAddMovement(
    /* [in] */ VelocityTrackerState* state,
    /* [in] */ IMotionEvent* event)
{
    if (!event) {
        return;
    }
    HANDLE handle;
    event->GetNative((HANDLE*)&handle);
    const android::MotionEvent* nativeEvent = reinterpret_cast<android::MotionEvent*>(handle);
    if (!nativeEvent) {
        ALOGW("nativeAddMovement failed because MotionEvent was finalized.");
        return;
    }

    state->AddMovement(nativeEvent);
}

void VelocityTracker::NativeComputeCurrentVelocity(
    /* [in] */ VelocityTrackerState* state,
    /* [in] */ Int32 units,
    /* [in] */ Float maxVelocity)
{
    state->ComputeCurrentVelocity(units, maxVelocity);
}

Float VelocityTracker::NativeGetXVelocity(
    /* [in] */ VelocityTrackerState* state,
    /* [in] */ Int32 id)
{
    Float vx;
    state->GetVelocity(id, &vx, NULL);
    return vx;
}

Float VelocityTracker::NativeGetYVelocity(
    /* [in] */ VelocityTrackerState* state,
    /* [in] */ Int32 id)
{
    Float vy;
    state->GetVelocity(id, NULL, &vy);
    return vy;
}

Boolean VelocityTracker::NativeGetEstimator(
    /* [in] */ VelocityTrackerState* state,
    /* [in] */ Int32 id,
    /* [in] */ Estimator* outEstimator)
{
    android::VelocityTracker::Estimator estimator;
    Boolean result = state->GetEstimator(id, &estimator);

    for(Int32 i=0; i < VelocityTracker::Estimator::MAX_DEGREE + 1; i++) {
        (*(outEstimator->mXCoeff))[i] = estimator.xCoeff[i];
        (*(outEstimator->mYCoeff))[i] = estimator.yCoeff[i];
    }

    outEstimator->mDegree = estimator.degree;
    outEstimator->mConfidence = estimator.confidence;

    return result;
}

CAR_INTERFACE_IMPL(VelocityTracker, Object, IVelocityTracker)

AutoPtr<VelocityTracker> VelocityTracker::Obtain()
{
    AutoPtr<IVelocityTracker> instance = sPool->AcquireItem();
    return (instance != NULL) ? (VelocityTracker*)instance.Get() : new VelocityTracker(String(NULL));
}

AutoPtr<VelocityTracker> VelocityTracker::Obtain(
    /* [in] */ const String& strategy)
{
    if (strategy.IsNull()) {
        return Obtain();
    }
    AutoPtr<VelocityTracker> vt = new VelocityTracker(strategy);
    return vt;
}

ECode VelocityTracker::Recycle()
{
    if (mStrategy.IsNull()) {
        Clear();
        sPool->ReleaseItem(this);
    }

    return NOERROR;
}

VelocityTracker::VelocityTracker(
    /* [in] */ const String& strategy)
    : mStrategy(strategy)
{
    mPtr = NativeInitialize(strategy);
}

VelocityTracker::~VelocityTracker()
{
    mPtr = NULL;
}

ECode VelocityTracker::Clear()
{
    NativeClear(mPtr);
    return NOERROR;
}

ECode VelocityTracker::AddMovement(
    /* [in] */ IMotionEvent* ev)
{
    if(ev == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    NativeAddMovement(mPtr, ev);
    return NOERROR;
}

ECode VelocityTracker::ComputeCurrentVelocity(
    /* [in] */ Int32 units)
{
    return ComputeCurrentVelocity(units, Elastos::Core::Math::FLOAT_MAX_VALUE);
}

ECode VelocityTracker::ComputeCurrentVelocity(
    /* [in] */ Int32 units,
    /* [in] */ Float maxVelocity)
{
    NativeComputeCurrentVelocity(mPtr, units, maxVelocity);
    return NOERROR;
}

ECode VelocityTracker::GetXVelocity(
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);

    *x = NativeGetXVelocity(mPtr, ACTIVE_POINTER_ID);
    return NOERROR;
}

ECode VelocityTracker::GetYVelocity(
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);

    *y = NativeGetYVelocity(mPtr, ACTIVE_POINTER_ID);
    return NOERROR;
}

ECode VelocityTracker::GetXVelocity(
    /* [in] */ Int32 id,
    /* [out] */ Float* x)
{
    VALIDATE_NOT_NULL(x);

    *x = NativeGetXVelocity(mPtr, id);
    return NOERROR;
}

ECode VelocityTracker::GetYVelocity(
    /* [in] */ Int32 id,
    /* [out] */ Float* y)
{
    VALIDATE_NOT_NULL(y);

    *y = NativeGetYVelocity(mPtr, id);
    return NOERROR;
}

ECode VelocityTracker::GetEstimator(
    /* [in] */ Int32 id,
    /* [in] */ IEstimator* outEstimatorObj,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if(outEstimatorObj == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *result = NativeGetEstimator(mPtr, id, (Estimator*)outEstimatorObj);
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
