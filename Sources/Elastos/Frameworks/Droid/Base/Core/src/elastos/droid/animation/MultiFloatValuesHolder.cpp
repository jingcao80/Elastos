
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/animation/MultiFloatValuesHolder.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Core::IFloat;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Animation {

MultiFloatValuesHolder::ClassMethodMap MultiFloatValuesHolder::sJNISetterPropertyMap;
CAR_INTERFACE_IMPL(MultiFloatValuesHolder, PropertyValuesHolder, IMultiFloatValuesHolder);
MultiFloatValuesHolder::MultiFloatValuesHolder(
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

MultiFloatValuesHolder::MultiFloatValuesHolder(
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

ECode MultiFloatValuesHolder::SetAnimatedValue(
    /* [in] */ IInterface* target)
{
    AutoPtr<IInterface> v;
    GetAnimatedValue((IInterface**)&v);
    AutoPtr<IArrayList> list = IArrayList::Probe(v);
    Int32 numParameters = 0;
    assert(list != NULL);
    list->GetSize(&numParameters);

    AutoPtr<ArrayOf<Float> > values = ArrayOf<Float>::Alloc(numParameters);
    Float fv = 0.f;
    for (Int32 i = 0; i < numParameters; i++) {
        AutoPtr<IFloat> value;
        list->Get(i, (IInterface**)&value);
        value->GetValue(&fv);
        (*values)[i] = fv;
    }
    if (mJniSetter != 0) {
        switch (numParameters) {
            case 1: {
                nCallFloatMethod(target, mJniSetter, (*values)[0]);
                break;
            }
            case 2: {
                nCallTwoFloatMethod(target, mJniSetter, (*values)[0], (*values)[1]);
                break;
            }
            case 4: {
                nCallFourFloatMethod(target, mJniSetter, (*values)[0], (*values)[1],
                        (*values)[2], (*values)[3]);
                break;
            }
            default: {
                nCallMultipleFloatMethod(target, mJniSetter, values);
                break;
            }
        }
    }
    return NOERROR;
}

ECode MultiFloatValuesHolder::SetupSetterAndGetter(
    /* [in] */ IInterface* target)
{
    return SetupSetter(target);
}

ECode MultiFloatValuesHolder::SetupSetter(
    /* [in] */ IInterface* targetClass)
{
    if (mJniSetter != NULL) {
        return NOERROR;
    }
    // try {
    AutoLock lock(mPropertyMapLock);
    // mPropertyMapLock.writeLock().lock();
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
            mJniSetter = nGetMultipleFloatMethod(info, methodName, numParams);
        } catch (NoSuchMethodError e) {
            // try without the 'set' prefix
            mJniSetter = nGetMultipleFloatMethod(info, mPropertyName, numParams);
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
