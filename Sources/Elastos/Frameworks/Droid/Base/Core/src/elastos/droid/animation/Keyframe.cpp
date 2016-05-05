
#include "elastos/droid/animation/Keyframe.h"

using Elastos::Core::EIID_ICloneable;
using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::ECLSID_CInteger32;

namespace Elastos {
namespace Droid {
namespace Animation {

//==============================================================================
//                  Keyframe
//==============================================================================
CAR_INTERFACE_IMPL_2(Keyframe, Object, IKeyframe, ICloneable);
Keyframe::Keyframe()
    : mFraction(0.0f)
    , mHasValue(FALSE)
    , mValueWasSetOnStart(FALSE)
{
}

ECode Keyframe::HasValue(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mHasValue;
    return NOERROR;
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

ECode Keyframe::OfInt32(
    /* [in] */ Float fraction,
    /* [in] */ Int32 value,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IInt32Keyframe> rTmp = new Int32Keyframe(fraction, value);
    *obj = IKeyframe::Probe(rTmp);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Keyframe::OfInt32(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IInt32Keyframe> rTmp = new Int32Keyframe(fraction);
    *obj = IKeyframe::Probe(rTmp);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Keyframe::OfFloat(
    /* [in] */ Float fraction,
    /* [in] */ Float value,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IFloatKeyframe> rTmp = new FloatKeyframe(fraction, value);
    *obj = IKeyframe::Probe(rTmp);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Keyframe::OfFloat(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IFloatKeyframe> rTmp = new FloatKeyframe(fraction);
    *obj = IKeyframe::Probe(rTmp);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Keyframe::OfObject(
    /* [in] */ Float fraction,
    /* [in] */ IInterface* value,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IObjectKeyframe> rTmp = new ObjectKeyframe(fraction, value);
    *obj = IKeyframe::Probe(rTmp);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode Keyframe::OfObject(
    /* [in] */ Float fraction,
    /* [out] */ IKeyframe** obj)
{
    AutoPtr<IObjectKeyframe> rTmp = new ObjectKeyframe(fraction, NULL);
    *obj = IKeyframe::Probe(rTmp);
    REFCOUNT_ADD(*obj)
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
        AutoPtr<IObject> obj = IObject::Probe(value);
        obj->GetInterfaceID(value, &mValueType);
    } else {
        assert(0 && "TODO");
        // mValueType = ECLSID_CDummyObject;
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
    Boolean has = FALSE;
    AutoPtr<ObjectKeyframe> kfClone = new ObjectKeyframe(mFraction, (HasValue(&has), has) ? mValue : NULL);
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
    mValueType = EIID_IFloat;
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
    } else{
        mHasValue = FALSE;
    }
    return NOERROR;
}

ECode Int32Keyframe::Clone(
    /* [out] */ IInterface** object)
{
    AutoPtr<Int32Keyframe> kfClone;
    if(mHasValue) {
        Int32 value;
        mValue->GetValue(&value);
        kfClone = new Int32Keyframe(mFraction, value);
    } else {
        kfClone = new Int32Keyframe(mFraction);
    }

    AutoPtr<ITimeInterpolator> ti;
    Keyframe::GetInterpolator((ITimeInterpolator**)&ti);
    kfClone->SetInterpolator(ti);
    kfClone->mValueWasSetOnStart = mValueWasSetOnStart;
    *object = (IKeyframe*)kfClone->Probe(EIID_IKeyframe);
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
    } else {
        mHasValue = FALSE;
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
    } else {
        kfClone = new FloatKeyframe(mFraction);
    }

    AutoPtr<ITimeInterpolator> ti;
    Keyframe::GetInterpolator((ITimeInterpolator**)&ti);
    kfClone->SetInterpolator(ti);
    kfClone->mValueWasSetOnStart = mValueWasSetOnStart;
    *object = (IKeyframe*)kfClone->Probe(EIID_IKeyframe);
    REFCOUNT_ADD(*object)
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

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
