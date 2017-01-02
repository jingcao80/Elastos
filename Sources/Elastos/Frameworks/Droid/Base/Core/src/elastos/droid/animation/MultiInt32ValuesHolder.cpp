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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/animation/MultiInt32ValuesHolder.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace Animation {

MultiInt32ValuesHolder::ClassMethodMap MultiInt32ValuesHolder::sNativeGetterSetterPropertyMap;

CAR_INTERFACE_IMPL(MultiInt32ValuesHolder, PropertyValuesHolder, IMultiInt32ValuesHolder);

MultiInt32ValuesHolder::MultiInt32ValuesHolder(
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
    : PropertyValuesHolder(propertyName)
{
    SetConverter(converter);
    SetObjectValues(values);
    SetEvaluator(evaluator);
}

MultiInt32ValuesHolder::MultiInt32ValuesHolder(
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ IKeyframes* keyframes)
    : PropertyValuesHolder(propertyName)
{
    SetConverter(converter);
    mKeyframes = keyframes;
    SetEvaluator(evaluator);
}

ECode MultiInt32ValuesHolder::SetAnimatedValue(
    /* [in] */ IInterface* target)
{
    AutoPtr<IInterface> v;
    GetAnimatedValue((IInterface**)&v);
    AutoPtr<IArrayList> list = IArrayList::Probe(v);
    Int32 numParameters = 0;
    list->GetSize(&numParameters);
    AutoPtr<ArrayOf<Int32> > values = ArrayOf<Int32>::Alloc(numParameters);
    for (Int32 i = 0; i < numParameters; i++) {
        AutoPtr<IInterface> value;
        list->Get(i, (IInterface**)&value);
        Int32 iv = 0;
        IInteger32::Probe(value)->GetValue(&iv);
        (*values)[i] = iv;
    }
    if (mNativeSetter != NULL) {
        switch (numParameters) {
            case 1:
                nCallInt32Method(target, mNativeSetter, (*values)[0]);
                break;
            case 2:
                nCallTwoInt32Method(target, mNativeSetter, (*values)[0], (*values)[1]);
                break;
            case 4:
                nCallFourInt32Method(target, mNativeSetter, (*values)[0], (*values)[1],
                        (*values)[2], (*values)[3]);
                break;
            default: {
                nCallMultipleInt32Method(target, mNativeSetter, values);
                break;
            }
        }
    }
    return NOERROR;
}

ECode MultiInt32ValuesHolder::SetupSetterAndGetter(
    /* [in] */ IInterface* target)
{
    return SetupSetter(GetClassInfo(target));
}

ECode MultiInt32ValuesHolder::SetupSetter(
    /* [in] */ IClassInfo* targetClass)
{
    if (mNativeSetter != NULL) {
        return NOERROR;
    }
    // try {
        // mPropertyMapLock.writeLock().lock();
    AutoLock lock(mPropertyMapLock);

    String methodName = GetMethodName(String("Set"), mPropertyName);

    AutoPtr<MethodMap> propertyMap;
    typename ClassMethodMap::Iterator it = sNativeGetterSetterPropertyMap.Find(targetClass);
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
        CalculateValue(0.f);
        AutoPtr<IInterface> values;
        GetAnimatedValue((IInterface**)&values);
        Int32 numParams = 0;
        IArrayList::Probe(values)->GetSize(&numParams);
        mNativeSetter = nGetMultipleInt32Method(targetClass, methodName, numParams);
        if (mNativeSetter != NULL) {
            if (propertyMap == NULL) {
                propertyMap = new MethodMap();
                sNativeGetterSetterPropertyMap[targetClass] = propertyMap;
            }
            (*propertyMap)[methodName] = mNativeSetter;
        }
    }
    // } finally {
    //     mPropertyMapLock.writeLock().unlock();
    // }
    return NOERROR;
}

ECode MultiInt32ValuesHolder::SetupGetter(
    /* [in] */ IClassInfo* targetClass)
{
    if (mProperty != NULL) {
        return NOERROR;
    }
    // Check new static hashmap<propName, int> for setter method
    {
        AutoLock lock(mPropertyMapLock);

        String methodName = GetMethodName(String("Get"), mPropertyName);

        AutoPtr<MethodMap> propertyMap;
        ClassMethodMapIterator it = sNativeGetterSetterPropertyMap.Find(targetClass);
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
            CalculateValue(0.f);
            AutoPtr<IInterface> values;
            GetAnimatedValue((IInterface**)&values);
            Int32 numParams = 0;
            IArrayList::Probe(values)->GetSize(&numParams);
            mGetter = nGetMultiparameterMethod(targetClass, methodName, numParams, String("I32*"));
            if (mGetter != NULL) {
                if (propertyMap == NULL) {
                    propertyMap = new MethodMap();
                    sNativeGetterSetterPropertyMap[targetClass] = propertyMap;
                }
                (*propertyMap)[methodName] = mGetter;
            }
        }
    }
    return NOERROR;
}

ECode MultiInt32ValuesHolder::CallGetter(
    /* [in] */ IInterface* target,
    /* [out] */ IInterface** value)
{
    AutoPtr<IInterface> values;
    GetAnimatedValue((IInterface**)&values);
    Int32 numParams = 0;
    IArrayList::Probe(values)->GetSize(&numParams);
    AutoPtr< ArrayOf<Int32> > tmpArray = ArrayOf<Int32>::Alloc(numParams);
    AutoPtr<IArgumentList> arg;
    mGetter->CreateArgumentList((IArgumentList**)&arg);
    for (Int32 i = 0; i < numParams; i++) {
        arg->SetOutputArgumentOfInt32Ptr(i, &((*tmpArray)[i]));
    }
    ECode ec = mGetter->Invoke(target, arg);
    if (FAILED(ec)) {
        *value = NULL;
        return ec;
    }
    AutoPtr<IArrayList> resultObj;
    CArrayList::New((IArrayList**)&resultObj);
    for (Int32 i = 0; i < numParams; i++) {
        resultObj->Add(i, CoreUtils::Convert((*tmpArray)[i]));
    }
    *value = resultObj;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

} // namespace Animation
} // namepsace Droid
} // namespace Elastos
