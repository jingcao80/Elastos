
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/animation/Int32PropertyValuesHolder.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Animation {

Int32PropertyValuesHolder::ClassMethodMap Int32PropertyValuesHolder::sNativeSetterPropertyMap;

CAR_INTERFACE_IMPL(Int32PropertyValuesHolder, PropertyValuesHolder, IInt32PropertyValuesHolder);

Int32PropertyValuesHolder::Int32PropertyValuesHolder(
    /* [in] */ const String& propertyName,
    /* [in] */ IInt32Keyframes* keyframes)
    : PropertyValuesHolder(propertyName)
{
    mValueType = EIID_IInteger32;
    mKeyframes = IKeyframes::Probe(keyframes);
    mInt32Keyframes = keyframes;
}

Int32PropertyValuesHolder::Int32PropertyValuesHolder(
    /* [in] */ IProperty* property,
    /* [in] */ IInt32Keyframes* keyframes)
    : PropertyValuesHolder(property)
{
    mValueType = EIID_IInteger32;
    mKeyframes = IKeyframes::Probe(keyframes);
    mInt32Keyframes = keyframes;
}

Int32PropertyValuesHolder::Int32PropertyValuesHolder(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Int32>* values)
    : PropertyValuesHolder(propertyName)
{
    SetInt32Values(values);
}

Int32PropertyValuesHolder::Int32PropertyValuesHolder(
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Int32>* values)
    : PropertyValuesHolder(property)
{
    SetInt32Values(values);
    if (IInt32Property::Probe(property) != NULL) {
       mInt32Property = IInt32Property::Probe(mProperty);
    }
}

ECode Int32PropertyValuesHolder::SetInt32Values(
    /* [in] */ ArrayOf<Int32>* values)
{
    PropertyValuesHolder::SetInt32Values(values);
    mInt32Keyframes = IInt32Keyframes::Probe(mKeyframes);
    return NOERROR;
}

ECode Int32PropertyValuesHolder::CalculateValue(
    /* [in] */ Float fraction)
{
    return mInt32Keyframes->GetInt32Value(fraction, &mInt32AnimatedValue);
}

ECode Int32PropertyValuesHolder::GetAnimatedValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<IInteger32> rst;
    CInteger32::New(mInt32AnimatedValue, (IInteger32**)&rst);
    *value = rst;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode Int32PropertyValuesHolder::Clone(
    /* [out] */ IInterface** holder)
{
    AutoPtr<Int32PropertyValuesHolder> v = new Int32PropertyValuesHolder(mPropertyName, mInt32Keyframes);
    CloneImpl(v);
    v->mNativeSetter = mNativeSetter;
    v->mInt32Keyframes = mInt32Keyframes;
    v->mInt32AnimatedValue = mInt32AnimatedValue;
    *holder = (IInt32PropertyValuesHolder*)v;
    REFCOUNT_ADD(*holder);
    return NOERROR;
}

ECode Int32PropertyValuesHolder::SetAnimatedValue(
    /* [in] */ IInterface* target)
{
    if (mInt32Property != NULL) {
        return mInt32Property->SetValue(target, mInt32AnimatedValue);
    }
    if (mProperty != NULL) {
        mProperty->Set(target, CoreUtils::Convert(mInt32AnimatedValue));
        return NOERROR;
    }
    if (mNativeSetter != NULL) {
        nCallInt32Method(target, mNativeSetter, mInt32AnimatedValue);
        return NOERROR;
    }
    return NOERROR;
}

ECode Int32PropertyValuesHolder::SetupSetter(
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
            mNativeSetter = nGetInt32Method(target, methodName);
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
