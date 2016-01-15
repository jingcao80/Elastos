
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/animation/MultiInt32ValuesHolder.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Core::IInteger32;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Animation {

MultiInt32ValuesHolder::ClassMethodMap MultiInt32ValuesHolder::sJNISetterPropertyMap;
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
    assert(list != NULL);
    list->GetSize(&numParameters);

    AutoPtr<ArrayOf<Int32> > values = ArrayOf<Int32>::Alloc(numParameters);
    Int32 iv = 0;
    for (Int32 i = 0; i < numParameters; i++) {
        AutoPtr<IInteger32> value;
        list->Get(i, (IInterface**)&value);
        value->GetValue(&iv);
        (*values)[i] = iv;
    }

    if (mJniSetter != NULL) {
        switch (numParameters) {
            case 1:
                nCallInt32Method(target, mJniSetter, (*values)[0]);
                break;
            case 2:
                nCallTwoInt32Method(target, mJniSetter, (*values)[0], (*values)[1]);
                break;
            case 4:
                nCallFourInt32Method(target, mJniSetter, (*values)[0], (*values)[1],
                        (*values)[2], (*values)[3]);
                break;
            default: {
                nCallMultipleInt32Method(target, mJniSetter, values);
                break;
            }
        }
    }

    return NOERROR;
}

ECode MultiInt32ValuesHolder::SetupSetterAndGetter(
    /* [in] */ IInterface* target)
{
    return SetupSetter(target/*.getClass()*/);
}

ECode MultiInt32ValuesHolder::SetupSetter(
    /* [in] */ IInterface* targetClass)
{
    if (mJniSetter != NULL) {
        return NOERROR;
    }
    // try {
        // mPropertyMapLock.writeLock().lock();
    AutoLock lock(mPropertyMapLock);
    AutoPtr<IClassInfo> info = TransformClassInfo(targetClass);
    AutoPtr<MethodMap> propertyMap = sJNISetterPropertyMap[info];

    typename ClassMethodMap::Iterator it = sJNISetterPropertyMap.Find(info);
    if ((it != sJNISetterPropertyMap.End()) && (it->mSecond != NULL)) {
        propertyMap = it->mSecond;
        typename MethodMap::Iterator it2 = propertyMap->Find(mPropertyName);
        if ((it2 != propertyMap->End()) && (it2->mSecond != NULL)) {
            mJniSetter = it2->mSecond;
        }
    }
    if (mJniSetter == NULL) {
        String methodName = GetMethodName(String("Set"), mPropertyName);
        CalculateValue(0.f);
        AutoPtr<IArrayList> values;
        GetAnimatedValue((IInterface**)&values);
        Int32 numParams = 0;
        values->GetSize(&numParams);
        try {
            mJniSetter = nGetMultipleInt32Method(info, methodName, numParams);
        } catch (NoSuchMethodError e) {
            // try without the 'set' prefix
            mJniSetter = nGetMultipleInt32Method(info, mPropertyName, numParams);
        }
        if (mJniSetter != NULL) {
            if (propertyMap == NULL) {
                propertyMap = new MethodMap();
                sJNISetterPropertyMap[info] = propertyMap;
            }
            (*propertyMap)[mPropertyName] = mJniSetter;
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
