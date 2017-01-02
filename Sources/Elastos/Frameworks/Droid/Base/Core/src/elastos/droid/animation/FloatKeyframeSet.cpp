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

#include "elastos/droid/animation/FloatKeyframeSet.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

using Elastos::Core::EIID_IFloat;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL_2(FloatKeyframeSet, KeyframeSet, IFloatKeyframeSet, IFloatKeyframes)

FloatKeyframeSet::FloatKeyframeSet(
    /* [in] */ ArrayOf<IFloatKeyframe*>* keyframes)
    : mFirstValue(0.0f)
    , mLastValue(0.0f)
    , mDeltaValue(0.0f)
    , mFirstTime(TRUE)
{
    Int32 len = keyframes->GetLength();
    AutoPtr<ArrayOf<IKeyframe*> > objs = ArrayOf<IKeyframe*>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        objs->Set(i, IKeyframe::Probe((*keyframes)[i]));
    }
    KeyframeSet::constructor(objs);
}

ECode FloatKeyframeSet::GetValue(
    /* [in] */ Float fraction,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value)
    Float v;
    GetFloatValue(fraction, &v);
    AutoPtr<IFloat> rst;
    CFloat::New(v, (IFloat**)&rst);
    *value = rst;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode FloatKeyframeSet::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    Int32 numKeyframes = mKeyframes->GetLength();
    AutoPtr<ArrayOf<IFloatKeyframe*> > newKeyframes = ArrayOf<IFloatKeyframe*>::Alloc(numKeyframes);
    for (Int32 i = 0; i < numKeyframes; ++i) {
        AutoPtr<IInterface> frame;
        ICloneable::Probe((*mKeyframes)[i])->Clone((IInterface**)&frame);
        newKeyframes->Set(i, IFloatKeyframe::Probe(frame));
    }
    AutoPtr<IFloatKeyframeSet> newSet = new FloatKeyframeSet(newKeyframes);
    *obj = newSet;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode FloatKeyframeSet::InvalidateCache()
{
    mFirstTime = TRUE;
    return NOERROR;
}

ECode FloatKeyframeSet::GetFloatValue(
    /* [in] */ Float frc,
    /* [out] */ Float* value)
{
    VALIDATE_NOT_NULL(value)
    Float fraction = frc;
    if (mNumKeyframes == 2) {
        if (mFirstTime) {
            mFirstTime = FALSE;
            AutoPtr<IFloatKeyframe> frameTmp = IFloatKeyframe::Probe((*mKeyframes)[0]);
            frameTmp->GetFloatValue(&mFirstValue);
            frameTmp = IFloatKeyframe::Probe((*mKeyframes)[1]);
            frameTmp->GetFloatValue(&mLastValue);
            mDeltaValue = mLastValue - mFirstValue;
        }

        if (mInterpolator != NULL) {
             mInterpolator->GetInterpolation(fraction, &fraction);
        }
        if (mEvaluator == NULL) {
            *value = mFirstValue + fraction * mDeltaValue;
            return NOERROR;
        }
        else {
            AutoPtr<IFloat> fValue, lValue;
            CFloat::New(mFirstValue, (IFloat**)&fValue);
            CFloat::New(mLastValue, (IFloat**)&lValue);
            AutoPtr<IInterface> obj;
            mEvaluator->Evaluate(fraction, fValue, lValue, (IInterface**)&obj);
            assert(obj != NULL);
            IFloat::Probe(obj)->GetValue(value);
            return NOERROR;
        }
    }

    if (fraction <= 0.0f) {
        AutoPtr<IFloatKeyframe> prevKeyframe = IFloatKeyframe::Probe((*mKeyframes)[0]);
        AutoPtr<IFloatKeyframe> nextKeyframe = IFloatKeyframe::Probe((*mKeyframes)[1]);
        Float prevValue, nextValue, prevFraction, nextFraction;
        prevKeyframe->GetFloatValue(&prevValue);
        nextKeyframe->GetFloatValue(&nextValue);
        IKeyframe::Probe(prevKeyframe)->GetFraction(&prevFraction);
        IKeyframe::Probe(nextKeyframe)->GetFraction(&nextFraction);
        AutoPtr<ITimeInterpolator> interpolator;
        IKeyframe::Probe(nextKeyframe)->GetInterpolator((ITimeInterpolator**)&interpolator);
        if (interpolator != NULL) {
            interpolator->GetInterpolation(fraction, &fraction);
        }

        Float intervalFraction = (fraction - prevFraction) / (nextFraction - prevFraction);
        if (mEvaluator == NULL) {
            *value = (prevValue + intervalFraction * (nextValue - prevValue));
            return NOERROR;
        }

        AutoPtr<IFloat> pValue, nValue;
        CFloat::New(prevValue, (IFloat**)&pValue);
        CFloat::New(nextValue, (IFloat**)&nValue);
        AutoPtr<IInterface> obj;
        mEvaluator->Evaluate(intervalFraction, pValue, nValue, (IInterface**)&obj);
        assert(obj != NULL);
        IFloat::Probe(obj)->GetValue(value);
        return NOERROR;
    }
    else if (fraction >= 1.0f) {
        AutoPtr<IFloatKeyframe> prevKeyframe = IFloatKeyframe::Probe((*mKeyframes)[mNumKeyframes - 2]);
        AutoPtr<IFloatKeyframe> nextKeyframe = IFloatKeyframe::Probe((*mKeyframes)[mNumKeyframes - 1]);

        Float prevValue, nextValue, prevFraction, nextFraction;
        prevKeyframe->GetFloatValue(&prevValue);
        nextKeyframe->GetFloatValue(&nextValue);
        IKeyframe::Probe(prevKeyframe)->GetFraction(&prevFraction);
        IKeyframe::Probe(nextKeyframe)->GetFraction(&nextFraction);
        AutoPtr<ITimeInterpolator> interpolator;
        IKeyframe::Probe(nextKeyframe)->GetInterpolator((ITimeInterpolator**)&interpolator);
        if (interpolator != NULL) {
            interpolator->GetInterpolation(fraction, &fraction);
        }

        Float intervalFraction = (fraction - prevFraction) / (nextFraction - prevFraction);
        if (mEvaluator == NULL) {
            *value = prevValue + intervalFraction * (nextValue - prevValue);
            return NOERROR;
        }

        AutoPtr<IFloat> pValue, nValue;
        CFloat::New(prevValue, (IFloat**)&pValue);
        CFloat::New(nextValue, (IFloat**)&nValue);
        AutoPtr<IInterface> obj;
        mEvaluator->Evaluate(intervalFraction, pValue, nValue, (IInterface**)&obj);
        assert(obj != NULL);
        IFloat::Probe(obj)->GetValue(value);
        return NOERROR;
    }

    AutoPtr<IFloatKeyframe> prevKeyframe = IFloatKeyframe::Probe((*mKeyframes)[0]);
    for (Int32 i = 1; i < mNumKeyframes; ++i) {
        AutoPtr<IFloatKeyframe> nextKeyframe = IFloatKeyframe::Probe((*mKeyframes)[1]);
        Float nFraction;
        IKeyframe::Probe(nextKeyframe)->GetFraction(&nFraction);
        if (fraction < nFraction) {
            AutoPtr<ITimeInterpolator> interpolator;
            IKeyframe::Probe(nextKeyframe)->GetInterpolator((ITimeInterpolator**)&interpolator);
            if (interpolator != NULL) {
                interpolator->GetInterpolation(fraction, &fraction);
            }

            IKeyframe::Probe(nextKeyframe)->GetFraction(&nFraction);
            Float pFraction;
            IKeyframe::Probe(prevKeyframe)->GetFraction(&pFraction);
            Float intervalFraction = (fraction - pFraction) / (nFraction - pFraction);
            Float prevValue;
            prevKeyframe->GetFloatValue(&prevValue);
            Float nextValue;
            nextKeyframe->GetFloatValue(&nextValue);

            if (mEvaluator == NULL) {
                *value = prevValue + intervalFraction * (nextValue - prevValue);
                return NOERROR;
            }

            AutoPtr<IFloat> pValue, nValue;
            CFloat::New(prevValue, (IFloat**)&pValue);
            CFloat::New(nextValue, (IFloat**)&nValue);
            AutoPtr<IInterface> obj;
            mEvaluator->Evaluate(intervalFraction, pValue, nValue, (IInterface**)&obj);
            assert(obj != NULL);
            IFloat::Probe(obj)->GetValue(value);
            return NOERROR;
        }

        prevKeyframe = nextKeyframe;
    }

    // shouldn't get here
    AutoPtr<IFloatKeyframe> frame =
            IFloatKeyframe::Probe((*mKeyframes)[mNumKeyframes - 1]);
    AutoPtr<IInterface> obj;
    IKeyframe::Probe(frame)->GetValue((IInterface**)&obj);
    assert(obj != NULL);
    IFloat::Probe(obj)->GetValue(value);
    return NOERROR;
}

ECode FloatKeyframeSet::GetKeyframes(
    /* [out, callee] */ ArrayOf<IKeyframe*>** frames)
{
    VALIDATE_NOT_NULL(frames);
    *frames = mKeyframes;
    REFCOUNT_ADD(*frames);
    return NOERROR;
}

ECode FloatKeyframeSet::GetType(
    /* [out] */ InterfaceID* type)
{
    VALIDATE_NOT_NULL(type);
    *type = EIID_IFloat;
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
