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

#include "elastos/droid/animation/Keyframe.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::EIID_IInteger32;

namespace Elastos {
namespace Droid {
namespace Animation {

//==============================================================================
//                  Keyframe
//==============================================================================
CAR_INTERFACE_IMPL_2(Keyframe, Object, IKeyframe, ICloneable);

Keyframe::Keyframe()
    : mHasValue(FALSE)
    , mValueWasSetOnStart(FALSE)
    , mFraction(0.0f)
{}

ECode Keyframe::OfInt32(
    /* [in] */ Float fraction,
    /* [in] */ Int32 value,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IKeyframe> rTmp = new Int32Keyframe(fraction, value);
    *obj = rTmp;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Keyframe::OfInt32(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IKeyframe> rTmp = new Int32Keyframe(fraction);
    *obj = rTmp;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Keyframe::OfFloat(
    /* [in] */ Float fraction,
    /* [in] */ Float value,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IKeyframe> rTmp = new FloatKeyframe(fraction, value);
    *obj = rTmp;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Keyframe::OfFloat(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IKeyframe> rTmp = new FloatKeyframe(fraction);
    *obj = rTmp;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Keyframe::OfObject(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* value,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IKeyframe> rTmp = new ObjectKeyframe(fraction, value);
    *obj = rTmp;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Keyframe::OfObject(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IKeyframe> rTmp = new ObjectKeyframe(fraction, NULL);
    *obj = rTmp;
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Keyframe::HasValue(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mHasValue;
    return NOERROR;
}

Boolean Keyframe::ValueWasSetOnStart()
{
    return mValueWasSetOnStart;
}

void Keyframe::SetValueWasSetOnStart(
    /* [in] */ Boolean valueWasSetOnStart)
{
    mValueWasSetOnStart = valueWasSetOnStart;
}

ECode Keyframe::GetFraction(
    /* [out] */ Float* fraction)
{
    VALIDATE_NOT_NULL(fraction);
    *fraction = mFraction;
    return NOERROR;
}

ECode Keyframe::SetFraction(
    /* [in] */ Float fraction)
{
    mFraction = fraction;
    return NOERROR;
}

ECode Keyframe::GetInterpolator(
    /* [out] */ ITimeInterpolator** interpolator)
{
    VALIDATE_NOT_NULL(interpolator);
    *interpolator = mInterpolator;
    REFCOUNT_ADD(*interpolator);
    return NOERROR;
}

ECode Keyframe::SetInterpolator(
    /* [in] */ ITimeInterpolator* interpolator)
{
    mInterpolator = interpolator;
    return NOERROR;
}

ECode Keyframe::GetType(
    /* [out] */ InterfaceID* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mValueType;
    return NOERROR;
}


//==============================================================================
//                  ObjectKeyframe
//==============================================================================
CAR_INTERFACE_IMPL(ObjectKeyframe, Keyframe, IObjectKeyframe);

ObjectKeyframe::ObjectKeyframe(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* value)
{
    mFraction = fraction;
    mValue = value;
    mHasValue = (mValue != NULL);
    if (mHasValue) {
        value->GetInterfaceID(value, &mValueType);
    }
    else {
        mValueType = EIID_IObject;
    }
}

ECode ObjectKeyframe::GetValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode ObjectKeyframe::SetValue(
    /* [in] */ IInterface* value)
{
    mValue = value;
    mHasValue = (mValue != NULL);
    return NOERROR;
}

ECode ObjectKeyframe::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    Boolean hasValue = FALSE;
    HasValue(&hasValue);
    AutoPtr<ObjectKeyframe> kfClone = new ObjectKeyframe(mFraction, hasValue ? mValue : NULL);
    kfClone->mValueWasSetOnStart = mValueWasSetOnStart;
    AutoPtr<ITimeInterpolator> ti;
    GetInterpolator((ITimeInterpolator**)&ti);
    kfClone->SetInterpolator(ti);
    *object = (IKeyframe*)kfClone.Get();
    REFCOUNT_ADD(*object);
    return NOERROR;
}


//==============================================================================
//                  Int32Keyframe
//==============================================================================
CAR_INTERFACE_IMPL(Int32Keyframe, Keyframe, IInt32Keyframe);

Int32Keyframe::Int32Keyframe(
    /* [in] */ Float fraction,
    /* [in] */ Int32 value)
{
    mFraction = fraction;
    CInteger32::New(value, (IInteger32**)&mValue);
    mValueType = EIID_IInteger32;
    mHasValue = TRUE;
}

Int32Keyframe::Int32Keyframe(
    /* [in] */ Float fraction)
{
    CInteger32::New(0, (IInteger32**)&mValue);
    mFraction = fraction;
    mValueType = EIID_IInteger32;
}

ECode Int32Keyframe::GetInt32Value(
    /* [in] */ Int32* value)
{
    mValue->GetValue(value);
    return NOERROR;
}

ECode Int32Keyframe::GetValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode Int32Keyframe::SetValue(
    /* [in] */ IInterface* value)
{
    if (value != NULL && IInteger32::Probe(value) != NULL) {
        mValue = IInteger32::Probe(value);
        mHasValue = TRUE;
    }
    return NOERROR;
}

ECode Int32Keyframe::Clone(
    /* [out] */ IInterface** object)
{
    AutoPtr<Int32Keyframe> kfClone;
    if (mHasValue) {
        Int32 value;
        mValue->GetValue(&value);
        kfClone = new Int32Keyframe(mFraction, value);
    }
    else {
        kfClone = new Int32Keyframe(mFraction);
    }
    AutoPtr<ITimeInterpolator> ti;
    Keyframe::GetInterpolator((ITimeInterpolator**)&ti);
    kfClone->SetInterpolator(ti);
    kfClone->mValueWasSetOnStart = mValueWasSetOnStart;
    *object = (IKeyframe*)kfClone.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}


//==============================================================================
//                  FloatKeyframe
//==============================================================================
CAR_INTERFACE_IMPL(FloatKeyframe, Keyframe, IFloatKeyframe);

FloatKeyframe::FloatKeyframe(
    /* [in] */ Float fraction,
    /* [in] */ Float value)
{
    mFraction = fraction;
    CFloat::New(value, (IFloat**)&mValue);
    mValueType = EIID_IFloat;
    mHasValue = TRUE;
}

FloatKeyframe::FloatKeyframe(
    /* [in] */ Float fraction)
{
    CFloat::New(0.0, (IFloat**)&mValue);
    mFraction = fraction;
    mValueType = EIID_IFloat;
}

ECode FloatKeyframe::GetFloatValue(
    /* [in] */ Float* value)
{
    mValue->GetValue(value);
    return NOERROR;
}

ECode FloatKeyframe::GetValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode FloatKeyframe::SetValue(
    /* [in] */ IInterface* value)
{
    if (value != NULL && IFloat::Probe(value) != NULL) {
        mValue = IFloat::Probe(value);
        mHasValue = TRUE;
    }
    return NOERROR;
}

ECode FloatKeyframe::Clone(
    /* [out] */ IInterface** object)
{
    AutoPtr<FloatKeyframe> kfClone;
    if(mHasValue) {
        Float value;
        mValue->GetValue(&value);
        kfClone = new FloatKeyframe(mFraction, value);
    }
    else {
        kfClone = new FloatKeyframe(mFraction);
    }
    AutoPtr<ITimeInterpolator> ti;
    Keyframe::GetInterpolator((ITimeInterpolator**)&ti);
    kfClone->SetInterpolator(ti);
    kfClone->mValueWasSetOnStart = mValueWasSetOnStart;
    *object = (IKeyframe*)kfClone.Get();
    REFCOUNT_ADD(*object)
    return NOERROR;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
