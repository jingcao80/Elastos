
#include "elastos/droid/animation/Int32KeyframeSet.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

using Elastos::Core::INumber;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL_2(Int32KeyframeSet, KeyframeSet, IInt32KeyframeSet, IInt32Keyframes)
Int32KeyframeSet::Int32KeyframeSet(
    /* [in] */ ArrayOf<IInt32Keyframe*>* keyframes)
    : mFirstValue(0)
    , mLastValue(0)
    , mDeltaValue(0)
    , mFirstTime(TRUE)
{
    Int32 len = keyframes->GetLength();
    AutoPtr<ArrayOf<IKeyframe*> > objs = ArrayOf<IKeyframe*>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        objs->Set(i, IKeyframe::Probe((*keyframes)[i]));
    }
    KeyframeSet::constructor(objs);
}

ECode Int32KeyframeSet::GetValue(
    /* [in] */ Float fraction,
    /* [out] */ IInterface** value)
{
    Int32 vTemp;
    GetInt32Value(fraction, &vTemp);
    AutoPtr<IInteger32> iv;
    CInteger32::New(vTemp, (IInteger32**)&iv);
    *value = iv;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode Int32KeyframeSet::GetInt32Value(
    /* [in] */ Float frac,
    /* [out] */ Int32* value)
{
    Float fraction = frac;
    if (mNumKeyframes == 2) {
        if (mFirstTime) {
            mFirstTime = FALSE;
            AutoPtr<IInt32Keyframe> frameTmp = (IInt32Keyframe*)((*mKeyframes)[0]->Probe(EIID_IInt32Keyframe));
            frameTmp->GetInt32Value(&mFirstValue);

            frameTmp = (IInt32Keyframe*)((*mKeyframes)[1]->Probe(EIID_IInt32Keyframe));
            frameTmp->GetInt32Value(&mLastValue);;
            mDeltaValue = mLastValue - mFirstValue;
        }

        if (mInterpolator != NULL) {
            mInterpolator->GetInterpolation(fraction, &fraction);
        }

        if (mEvaluator == NULL) {
            *value = mFirstValue + (Int32)(fraction * mDeltaValue);
            return NOERROR;
        }
        else {
            AutoPtr<IInteger32> fValue, lValue;
            CInteger32::New(mFirstValue, (IInteger32**)&fValue);
            CInteger32::New(mLastValue, (IInteger32**)&lValue);
            AutoPtr<IInterface> obj;
            mEvaluator->Evaluate(fraction, fValue, lValue, (IInterface**)&obj);
            assert(obj != NULL);
            IInteger32::Probe(obj)->GetValue(value);
            return NOERROR;
        }
    }

    if (fraction <= 0.0f) {
        AutoPtr<IInt32Keyframe> prevKeyframe = (IInt32Keyframe*)((*mKeyframes)[0]->Probe(EIID_IInt32Keyframe));
        AutoPtr<IInt32Keyframe> nextKeyframe = (IInt32Keyframe*)((*mKeyframes)[1]->Probe(EIID_IInt32Keyframe));
        Int32 prevValue;
        prevKeyframe->GetInt32Value(&prevValue);
        Int32 nextValue;
        nextKeyframe->GetInt32Value(&nextValue);
        Float prevFraction;
        IKeyframe::Probe(prevKeyframe)->GetFraction(&prevFraction);
        Float nextFraction;
        IKeyframe::Probe(nextKeyframe)->GetFraction(&nextFraction);
        AutoPtr<ITimeInterpolator> interpolator;
        IKeyframe::Probe(nextKeyframe)->GetInterpolator((ITimeInterpolator**)&interpolator);
        if (interpolator != NULL) {
            interpolator->GetInterpolation(fraction, &fraction);
        }

        Float intervalFraction = (fraction - prevFraction) / (nextFraction - prevFraction);
        if (mEvaluator == NULL) {
            *value = prevValue + (Int32)(intervalFraction * (nextValue - prevValue)) ;
            return NOERROR;
        }

        AutoPtr<IInteger32> pValue, nValue;
        CInteger32::New(prevValue, (IInteger32**)&pValue);
        CInteger32::New(nextValue, (IInteger32**)&nValue);
        AutoPtr<IInterface> obj;
        mEvaluator->Evaluate(intervalFraction, pValue, nValue, (IInterface**)&obj);
        assert(obj != NULL);
        IInteger32::Probe(obj)->GetValue(value);
        return NOERROR;
    }
    else if (fraction >= 1.0f) {
        AutoPtr<IInt32Keyframe> prevKeyframe =
                (IInt32Keyframe*)((*mKeyframes)[mNumKeyframes - 2]->Probe(EIID_IInt32Keyframe));
        AutoPtr<IInt32Keyframe> nextKeyframe =
                (IInt32Keyframe*)((*mKeyframes)[mNumKeyframes - 1]->Probe(EIID_IInt32Keyframe));
        Int32 prevValue;
        prevKeyframe->GetInt32Value(&prevValue);
        Int32 nextValue;
        nextKeyframe->GetInt32Value(&nextValue);
        Float prevFraction;
        IKeyframe::Probe(prevKeyframe)->GetFraction(&prevFraction);
        Float nextFraction;
        IKeyframe::Probe(nextKeyframe)->GetFraction(&nextFraction);
        AutoPtr<ITimeInterpolator> interpolator;
        IKeyframe::Probe(nextKeyframe)->GetInterpolator((ITimeInterpolator**)&interpolator);
        if (interpolator != NULL) {
            interpolator->GetInterpolation(fraction, &fraction);
        }

        Float intervalFraction = (fraction - prevFraction) / (nextFraction - prevFraction);
        if (mEvaluator == NULL) {
            *value = prevValue + (Int32)(intervalFraction * (nextValue - prevValue));
            return NOERROR;
        }

        AutoPtr<IInteger32> pValue, nValue;
        CInteger32::New(prevValue, (IInteger32**)&pValue);
        CInteger32::New(nextValue, (IInteger32**)&nValue);
        AutoPtr<IInterface> obj;
        mEvaluator->Evaluate(intervalFraction, pValue, nValue, (IInterface**)&obj);
        assert(obj != NULL);
        IInteger32::Probe(obj)->GetValue(value);
        return NOERROR;
    }

    AutoPtr<IInt32Keyframe> prevKeyframe = (IInt32Keyframe*)((*mKeyframes)[0]->Probe(EIID_IInt32Keyframe));
    for (Int32 i = 1; i < mNumKeyframes; ++i) {
        AutoPtr<IInt32Keyframe> nextKeyframe = (IInt32Keyframe*)((*mKeyframes)[i]->Probe(EIID_IInt32Keyframe));
        Float nFraction;
        IKeyframe::Probe(nextKeyframe)->GetFraction(&nFraction);
        if (fraction < nFraction) {
            AutoPtr<ITimeInterpolator> interpolator;
            IKeyframe::Probe(nextKeyframe)->GetInterpolator((ITimeInterpolator**)&interpolator);
            if (interpolator != NULL) {
                interpolator->GetInterpolation(fraction, &fraction);
            }

            Float pFraction, nFraction;
            IKeyframe::Probe(prevKeyframe)->GetFraction(&pFraction);
            IKeyframe::Probe(nextKeyframe)->GetFraction(&nFraction);
            Float intervalFraction = (fraction - pFraction) / (nFraction - pFraction);
            Int32 prevValue;
            prevKeyframe->GetInt32Value(&prevValue);
            Int32 nextValue;
            nextKeyframe->GetInt32Value(&nextValue);

            if (mEvaluator == NULL) {
                *value = prevValue + (Int32)(intervalFraction * (nextValue - prevValue));
                return NOERROR;
            }

            AutoPtr<IInteger32> pValue, nValue;
            CInteger32::New(prevValue, (IInteger32**)&pValue);
            CInteger32::New(nextValue, (IInteger32**)&nValue);
            AutoPtr<IInterface> obj;
            mEvaluator->Evaluate(intervalFraction, pValue, nValue, (IInterface**)&obj);
            assert(obj != NULL);
            IInteger32::Probe(obj)->GetValue(value);
            return NOERROR;
        }

        prevKeyframe = nextKeyframe;
    }

    // shouldn't get here
    AutoPtr<IInt32Keyframe> obj = (IInt32Keyframe*)((*mKeyframes)[mNumKeyframes - 1]->Probe(EIID_IInt32Keyframe));
    AutoPtr<IInterface> rst;
    IKeyframe::Probe(obj)->GetValue((IInterface**)&rst);
    INumber::Probe(rst)->Int32Value(value);
    return NOERROR;
}

ECode Int32KeyframeSet::InvalidateCache()
{
    mFirstTime = TRUE;
    return NOERROR;
}

ECode Int32KeyframeSet::Clone(
    /* [out] */ IInterface** obj)
{
    Int32 numKeyframes = mKeyframes->GetLength();
    AutoPtr<ArrayOf<IInt32Keyframe*> > newKeyframes = ArrayOf<IInt32Keyframe*>::Alloc(numKeyframes);
    for (Int32 i = 0; i < numKeyframes; ++i) {
        AutoPtr<IInterface> newone;
        ICloneable::Probe((*mKeyframes)[i])->Clone((IInterface**)&newone);
        newKeyframes->Set(i, IInt32Keyframe::Probe(newone));
    }
    AutoPtr<IInt32KeyframeSet> newSet = new Int32KeyframeSet(newKeyframes);
    *obj = newSet;
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode Int32KeyframeSet::GetKeyframes(
    /* [out, callee] */ ArrayOf<IKeyframe*>** frames)
{
    VALIDATE_NOT_NULL(frames);
    *frames = mKeyframes;
    REFCOUNT_ADD(*frames);
    return NOERROR;
}

ECode Int32KeyframeSet::GetType(
    /* [out] */ InterfaceID* type)
{
    VALIDATE_NOT_NULL(type);
    *type = EIID_IInteger32;
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
