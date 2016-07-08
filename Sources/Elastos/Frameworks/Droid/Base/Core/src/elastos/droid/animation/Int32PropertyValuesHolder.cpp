
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/animation/Int32PropertyValuesHolder.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Animation {

Int32PropertyValuesHolder::ClassMethodMap Int32PropertyValuesHolder::sNativeGetterSetterPropertyMap;

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

        String methodName = GetMethodName(String("Set"), mPropertyName);

        AutoPtr<MethodMap> propertyMap;
        ClassMethodMapIterator it = sNativeGetterSetterPropertyMap.Find(target);
        if (it != sNativeGetterSetterPropertyMap.End()) {
            propertyMap = it->mSecond;
        }
        if (propertyMap != NULL) {
            MethodMapIterator mit = propertyMap->Find(methodName);
            AutoPtr<IMethodInfo> mtInfo;
            if (mit != propertyMap->End()) {
                mtInfo = mit->mSecond;
            }
            if (mtInfo != NULL) {
                mNativeSetter = mit->mSecond;
            }
        }
        if (mNativeSetter == NULL) {
            mNativeSetter = nGetInt32Method(target, methodName);
            if (mNativeSetter != NULL) {
                if (propertyMap == NULL) {
                    propertyMap = new MethodMap();
                    sNativeGetterSetterPropertyMap[target] = propertyMap;
                }
                (*propertyMap)[methodName] = mNativeSetter;
            }
            else {
                String className;
                target->GetName(&className);
                Logger::E("Int32PropertyValuesHolder", "Error: Method [%s] is not found on target class %s",
                    methodName.string(), className.string());
            }
        }
    }
    return NOERROR;
}

ECode Int32PropertyValuesHolder::SetupGetter(
    /* [in] */ IClassInfo* target)
{
    if (mProperty != NULL) {
        return NOERROR;
    }
    // Check new static hashmap<propName, int> for setter method
    {
        AutoLock lock(mPropertyMapLock);

        String methodName = GetMethodName(String("Get"), mPropertyName);

        AutoPtr<MethodMap> propertyMap;
        ClassMethodMapIterator it = sNativeGetterSetterPropertyMap.Find(target);
        if (it != sNativeGetterSetterPropertyMap.End()) {
            propertyMap = it->mSecond;
        }
        if (propertyMap != NULL) {
            MethodMapIterator mit = propertyMap->Find(methodName);
            AutoPtr<IMethodInfo> mtInfo;
            if (mit != propertyMap->End()) {
                mtInfo = mit->mSecond;
            }
            if (mtInfo != NULL) {
                mGetter = mit->mSecond;
            }
        }
        if (mGetter == NULL) {
            target->GetMethodInfo(methodName, String("(I32*)E"), (IMethodInfo**)&mGetter);
            if (mGetter != NULL) {
                if (propertyMap == NULL) {
                    propertyMap = new MethodMap();
                    sNativeGetterSetterPropertyMap[target] = propertyMap;
                }
                (*propertyMap)[methodName] = mGetter;
            }
        }
    }
    return NOERROR;
}

ECode Int32PropertyValuesHolder::CallGetter(
    /* [in] */ IInterface* target,
    /* [out] */ IInterface** value)
{
    AutoPtr<IArgumentList> arg;
    mGetter->CreateArgumentList((IArgumentList**)&arg);
    Int32 result;
    arg->SetOutputArgumentOfInt32Ptr(0, &result);
    ECode ec = mGetter->Invoke(target, arg);
    if (FAILED(ec)) {
        *value = NULL;
        return ec;
    }
    AutoPtr<IInteger32> resultObj;
    CInteger32::New(result, (IInteger32**)&resultObj);
    *value = resultObj;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

} // namespace Animation
} // namepsace Droid
} // namespace Elastos
