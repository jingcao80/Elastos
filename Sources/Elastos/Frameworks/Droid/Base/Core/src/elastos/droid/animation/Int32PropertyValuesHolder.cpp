
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/animation/Int32PropertyValuesHolder.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace Animation {

Int32PropertyValuesHolder::ClassMethodMap Int32PropertyValuesHolder::sJNISetterPropertyMap;

CAR_INTERFACE_IMPL(Int32PropertyValuesHolder, PropertyValuesHolder, IInt32PropertyValuesHolder);
Int32PropertyValuesHolder::Int32PropertyValuesHolder(
    /* [in] */ const String& propertyName,
    /* [in] */ IInt32Keyframes* keyframes)
    : PropertyValuesHolder(propertyName)
    , mInt32Keyframes(keyframes)
{
    mValueType = EIID_IInteger32;
    mKeyframes = IKeyframes::Probe(keyframes);
}

Int32PropertyValuesHolder::Int32PropertyValuesHolder(
    /* [in] */ IProperty* property,
    /* [in] */ IInt32Keyframes* keyframes)
    : PropertyValuesHolder(property)
    , mInt32Keyframes(keyframes)
{
    mValueType = EIID_IInteger32;
    mKeyframes = IKeyframes::Probe(keyframes);
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
   if(property->Probe(EIID_IInt32Property)) {
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
    CloneSuperData(v);
    v->mJniSetter = mJniSetter;
    v->mInt32Keyframes = mInt32Keyframes;
    v->mInt32AnimatedValue = mInt32AnimatedValue;
    *holder = (IInt32PropertyValuesHolder*)v->Probe(EIID_IInt32PropertyValuesHolder);
    REFCOUNT_ADD(*holder);

    return NOERROR;
}

ECode Int32PropertyValuesHolder::SetAnimatedValue(
    /* [in] */ IInterface* target)
{
    if (mInt32Property != NULL) {
        return mInt32Property->SetValue(target, mInt32AnimatedValue);
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
        if(carType == CarDataType_Int32)
        {
            args->SetInputArgumentOfInt32(0, mInt32AnimatedValue);
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
        if(carType == CarDataType_Int32)
        {
            args->SetInputArgumentOfInt32(0, mInt32AnimatedValue);
        } else if(carType == CarDataType_Interface){
            args->SetInputArgumentOfObjectPtr(0, animatedValue);
        } else {
            assert(0);
        }
        mSetter->Invoke(target, args);

        return NOERROR;
    }
    return NOERROR;
}

ECode Int32PropertyValuesHolder::SetupSetter(
        /* [in] */ IInterface* target)
{
    if (mProperty != NULL) {
        return NOERROR;
    }
    // Check new static hashmap<propName, int> for setter method
    {
        AutoLock lock(mPropertyMapLock);
        AutoPtr<IClassInfo> clInfo = TransformClassInfo(target);
        ClassMethodMapIterator exit = sJNISetterPropertyMap.Find(clInfo);
        AutoPtr<MethodMap> propertyMap = NULL;
        if(exit != sJNISetterPropertyMap.End()) {
            propertyMap = exit->mSecond;
            if(propertyMap != NULL) {
                MethodMapIterator it = propertyMap->Find(mPropertyName);
                if(it != propertyMap->End()) {
                    AutoPtr<IMethodInfo> mtInfo = it->mSecond;
                    if(mtInfo != NULL) {
                        mJniSetter = mtInfo;
                    }
                }
            }
        }

        if(mJniSetter == NULL) {
            String methodName = GetMethodName(String("Set"), mPropertyName);
            clInfo->GetMethodInfo(methodName, String("(I32)E"), (IMethodInfo**)&mJniSetter);
            if(mJniSetter != NULL) {
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
