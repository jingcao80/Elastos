
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/animation/MultiInt32ValuesHolder.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Core::IInteger32;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Animation {

MultiInt32ValuesHolder::ClassMethodMap MultiInt32ValuesHolder::sNativeSetterPropertyMap;

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

    AutoPtr<MethodMap> propertyMap;
    typename ClassMethodMap::Iterator it = sNativeSetterPropertyMap.Find(targetClass);
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
        CalculateValue(0.f);
        AutoPtr<IInterface> values;
        GetAnimatedValue((IInterface**)&values);
        Int32 numParams = 0;
        IArrayList::Probe(values)->GetSize(&numParams);
        mNativeSetter = nGetMultipleInt32Method(targetClass, methodName, numParams);
        if (mNativeSetter != NULL) {
            if (propertyMap == NULL) {
                propertyMap = new MethodMap();
                sNativeSetterPropertyMap[targetClass] = propertyMap;
            }
            (*propertyMap)[mPropertyName] = mNativeSetter;
        }
    }
    // } finally {
    //     mPropertyMapLock.writeLock().unlock();
    // }
    return NOERROR;
}

} // namespace Animation
} // namepsace Droid
} // namespace Elastos
