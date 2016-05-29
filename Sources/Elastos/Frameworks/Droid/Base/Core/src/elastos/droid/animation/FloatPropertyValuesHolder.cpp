
#include "elastos/droid/animation/FloatPropertyValuesHolder.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Core::AutoLock;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Animation {

FloatPropertyValuesHolder::ClassMethodMap FloatPropertyValuesHolder::sNativeSetterPropertyMap;

CAR_INTERFACE_IMPL(FloatPropertyValuesHolder, PropertyValuesHolder, IFloatPropertyValuesHolder);

FloatPropertyValuesHolder::FloatPropertyValuesHolder(
    /* [in] */ const String& propertyName,
    /* [in] */ IFloatKeyframes* keyframes)
    : PropertyValuesHolder(propertyName)
{
    mValueType = EIID_IFloat;
    mKeyframes = IKeyframes::Probe(keyframes);
    mFloatKeyframes = keyframes;
}

FloatPropertyValuesHolder::FloatPropertyValuesHolder(
    /* [in] */ IProperty* property,
    /* [in] */ IFloatKeyframes* keyframes)
    : PropertyValuesHolder(property)
{
    mValueType = EIID_IFloat;
    mKeyframes = IKeyframes::Probe(keyframes);
    mFloatKeyframes = keyframes;
}

FloatPropertyValuesHolder::FloatPropertyValuesHolder(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Float>* values)
    : PropertyValuesHolder(propertyName)
{
    SetFloatValues(values);
}

FloatPropertyValuesHolder::FloatPropertyValuesHolder(
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Float>* values)
    : PropertyValuesHolder(property)
{
    SetFloatValues(values);
    if (IFloatProperty::Probe(property) != NULL) {
        mFloatProperty = IFloatProperty::Probe(mProperty);
    }
}

ECode FloatPropertyValuesHolder::SetFloatValues(
    /* [in] */ ArrayOf<Float>* values)
{
    PropertyValuesHolder::SetFloatValues(values);
    mFloatKeyframes = IFloatKeyframes::Probe(mKeyframes);
    return NOERROR;
}

ECode FloatPropertyValuesHolder::CalculateValue(
    /* [in] */ Float fraction)
{
    mFloatKeyframes->GetFloatValue(fraction, &mFloatAnimatedValue);
    return NOERROR;
}

ECode FloatPropertyValuesHolder::GetAnimatedValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IFloat> rst;
    CFloat::New(mFloatAnimatedValue, (IFloat**)&rst);
    *value = rst;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode FloatPropertyValuesHolder::Clone(
    /* [out] */ IInterface** holder)
{
    AutoPtr<FloatPropertyValuesHolder> v = new FloatPropertyValuesHolder(mPropertyName, mFloatKeyframes);
    CloneImpl(v);
    v->mNativeSetter = mNativeSetter;
    v->mFloatKeyframes = mFloatKeyframes;
    v->mFloatAnimatedValue = mFloatAnimatedValue;
    *holder = (IFloatPropertyValuesHolder*)v;
    REFCOUNT_ADD(*holder);
    return NOERROR;
}

ECode FloatPropertyValuesHolder::SetAnimatedValue(
    /* [in] */ IInterface* target)
{
    if (mFloatProperty != NULL) {
        return mFloatProperty->SetValue(target, mFloatAnimatedValue);
    }
    if (mProperty != NULL) {
        mProperty->Set(target, CoreUtils::Convert(mFloatAnimatedValue));
        return NOERROR;
    }
    if (mNativeSetter != NULL) {
        nCallFloatMethod(target, mNativeSetter, mFloatAnimatedValue);
        return NOERROR;
    }
    return NOERROR;
}

ECode FloatPropertyValuesHolder::SetupSetter(
        /* [in] */ IClassInfo* target)
{
    if (mProperty != NULL) {
        return NOERROR;
    }
    // Check new static hashmap<propName, int> for setter method
    {
        AutoLock lock(mPropertyMapLock);

        AutoPtr<MethodMap> propertyMap;
        ClassMethodMapIterator it = sNativeSetterPropertyMap.Find(target);
        if (it != sNativeSetterPropertyMap.End()) {
            propertyMap = it->mSecond;
        }
        if (propertyMap != NULL) {
            MethodMapIterator mit = propertyMap->Find(mPropertyName);
            AutoPtr<IMethodInfo> mtInfo;
            if (mit != propertyMap->End()) {
                mtInfo = mit->mSecond;
            }
            if (mtInfo != NULL) {
                mNativeSetter = mit->mSecond;
            }
        }
        if (mNativeSetter == NULL) {
            String methodName = GetMethodName(String("Set"), mPropertyName);
            mNativeSetter = nGetFloatMethod(target, methodName);
            if (mNativeSetter != NULL) {
                if (propertyMap == NULL) {
                    propertyMap = new MethodMap();
                    sNativeSetterPropertyMap[target] = propertyMap;
                }
                (*propertyMap)[mPropertyName] = mNativeSetter;
            }
        }
    }
    return NOERROR;
}

} // namespace Animation
} // namepsace Droid
} // namespace Elastos
