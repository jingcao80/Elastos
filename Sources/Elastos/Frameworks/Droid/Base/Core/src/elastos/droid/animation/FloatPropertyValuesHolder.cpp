
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/animation/FloatPropertyValuesHolder.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Utility::EIID_IFloatProperty;
using Elastos::Core::AutoLock;
using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::IFloat;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::CFloat;

namespace Elastos {
namespace Droid {
namespace Animation {

FloatPropertyValuesHolder::ClassMethodMap FloatPropertyValuesHolder::sJNISetterPropertyMap;

CAR_INTERFACE_IMPL(FloatPropertyValuesHolder, PropertyValuesHolder, IFloatPropertyValuesHolder);
FloatPropertyValuesHolder::FloatPropertyValuesHolder(
    /* [in] */ const String& propertyName,
    /* [in] */ IFloatKeyframes* keyframes)
    : PropertyValuesHolder(propertyName)
    , mFloatKeyframes(keyframes)
{
    mValueType = EIID_IFloat;
    mKeyframes = IKeyframes::Probe(keyframes);
}

FloatPropertyValuesHolder::FloatPropertyValuesHolder(
    /* [in] */ IProperty* property,
    /* [in] */ IFloatKeyframes* keyframes)
    : PropertyValuesHolder(property)
    , mFloatKeyframes(keyframes)
{
    mValueType = EIID_IFloat;
    mKeyframes = IKeyframes::Probe(keyframes);
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
    if(property->Probe(EIID_IFloatProperty)) {
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
    return CFloat::New(mFloatAnimatedValue, (IFloat**)&rst);
    *value = rst;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode FloatPropertyValuesHolder::Clone(
    /* [out] */ IInterface** holder)
{
    AutoPtr<FloatPropertyValuesHolder> v = new FloatPropertyValuesHolder(mPropertyName, mFloatKeyframes);
    CloneSuperData(v);
    v->mJniSetter = mJniSetter;
    v->mFloatKeyframes = mFloatKeyframes;
    v->mFloatAnimatedValue = mFloatAnimatedValue;
    *holder = (IFloatPropertyValuesHolder*)v.Get();
    REFCOUNT_ADD(*holder);

    return NOERROR;
}

ECode FloatPropertyValuesHolder::SetAnimatedValue(
    /* [in] */ IInterface* target)
{
    if (mFloatProperty != NULL) {
        return mFloatProperty->SetValue(target, mFloatAnimatedValue);
    }

    AutoPtr<IInterface> animatedValue;
    GetAnimatedValue((IInterface**)&animatedValue);
    if (mProperty != NULL) {
        mProperty->Set(target, animatedValue);
        return NOERROR;
    }
    if (mJniSetter != NULL) {
        AutoPtr<IArgumentList> args;
        mJniSetter->CreateArgumentList((IArgumentList**)&args);
        AutoPtr<IFunctionInfo> funcInfo;
        args->GetFunctionInfo((IFunctionInfo**)&funcInfo);
        AutoPtr<IParamInfo> paramInfo;
        funcInfo->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
        AutoPtr<IDataTypeInfo> dataTypeInfo;
        paramInfo->GetTypeInfo((IDataTypeInfo**)&dataTypeInfo);
        CarDataType carType;
        dataTypeInfo->GetDataType(&carType);
        if(carType == CarDataType_Float)
        {
            args->SetInputArgumentOfFloat(0, mFloatAnimatedValue);
        } else if(carType == CarDataType_Interface){
            args->SetInputArgumentOfObjectPtr(0, animatedValue);
        } else {
            assert(0);
        }
        mJniSetter->Invoke(target, args);
        return NOERROR;
    }
    if (mSetter != NULL) {
        AutoPtr<IArgumentList> args;
        mSetter->CreateArgumentList((IArgumentList**)&args);
        AutoPtr<IFunctionInfo> funcInfo;
        args->GetFunctionInfo((IFunctionInfo**)&funcInfo);
        AutoPtr<IParamInfo> paramInfo;
        funcInfo->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
        AutoPtr<IDataTypeInfo> dataTypeInfo;
        paramInfo->GetTypeInfo((IDataTypeInfo**)&dataTypeInfo);
        CarDataType carType;
        dataTypeInfo->GetDataType(&carType);
        if (carType == CarDataType_Float) {
            args->SetInputArgumentOfFloat(0, mFloatAnimatedValue);
        }
        else if(carType == CarDataType_Interface){
            args->SetInputArgumentOfObjectPtr(0, animatedValue);
        }
        else {
            assert(0);
        }
        mSetter->Invoke(target, args);

        return NOERROR;
    }
    return NOERROR;
}

ECode FloatPropertyValuesHolder::SetupSetter(
        /* [in] */ IInterface* target)
{
    if (mProperty != NULL) {
        return NOERROR;
    }
    // Check new static hashmap<propName, int> for setter method
    {
        AutoLock lock(mPropertyMapLock);
        AutoPtr<IClassInfo> clInfo = TransformClassInfo(target);
        ClassID id;
        id.mUunm = (char*)malloc(80);
        clInfo->GetId(&id);
        free(id.mUunm);
        ClassMethodMapIterator exit = sJNISetterPropertyMap.Find(clInfo);
        AutoPtr<MethodMap> propertyMap;
        if (exit != sJNISetterPropertyMap.End()) {
            propertyMap = exit->mSecond;
            if (propertyMap != NULL) {
                MethodMapIterator it = propertyMap->Find(mPropertyName);
                if (it != propertyMap->End()) {
                    AutoPtr<IMethodInfo> mtInfo = it->mSecond;
                    if (mtInfo != NULL) {
                        mJniSetter = mtInfo;
                    }
                }
            }
        }

        if (mJniSetter == NULL) {
            String methodName = GetMethodName(String("Set"), mPropertyName);
            // clInfo->GetMethodInfo(methodName, (IMethodInfo**)&mJniSetter);
            mJniSetter = nGetFloatMethod(target, methodName);
            if (mJniSetter != NULL) {
                if (propertyMap == NULL) {
                    propertyMap = new MethodMap();
                    sJNISetterPropertyMap[clInfo] = propertyMap;
                }
                (*propertyMap)[mPropertyName] = mJniSetter;
            }
        }
    }

    if (mJniSetter == NULL) {
        // Couldn't find method through fast JNI approach - just use reflection
        return PropertyValuesHolder::SetupSetter(target);
    }
    return NOERROR;
}

} // namespace Animation
} // namepsace Droid
} // namespace Elastos
