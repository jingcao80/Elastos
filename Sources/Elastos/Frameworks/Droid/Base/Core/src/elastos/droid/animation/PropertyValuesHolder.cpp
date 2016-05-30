
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/animation/PropertyValuesHolder.h"
#include "elastos/droid/animation/CInt32Evaluator.h"
#include "elastos/droid/animation/CFloatEvaluator.h"
#include "elastos/droid/animation/Int32PropertyValuesHolder.h"
#include "elastos/droid/animation/FloatPropertyValuesHolder.h"
#include "elastos/droid/animation/Int32ArrayEvaluator.h"
#include "elastos/droid/animation/MultiInt32ValuesHolder.h"
#include "elastos/droid/animation/FloatArrayEvaluator.h"
#include "elastos/droid/animation/MultiFloatValuesHolder.h"
#include "elastos/droid/animation/Keyframe.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Graphics::EIID_IPointF;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::EIID_IArrayOf;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IDouble;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::CDouble;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::ECLSID_CDouble;
using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Animation {

using Elastos::Core::Math;

static const String TAG("PropertyValuesHolder");

//==============================================================================
// PropertyValuesHolder::PointFToFloatArray
//==============================================================================
PropertyValuesHolder::PointFToFloatArray::PointFToFloatArray()
{
    TypeConverter::constructor(EIID_IPointF, EIID_IArrayOf);
    CArrayOf::New(EIID_IFloat, 2, (IArrayOf**)&mCoordinates);
}

ECode PropertyValuesHolder::PointFToFloatArray::Convert(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** values)
{
    VALIDATE_NOT_NULL(values);
    Float x, y;
    IPointF::Probe(value)->Get(&x, &y);
    mCoordinates->Set(0, CoreUtils::Convert(x));
    mCoordinates->Set(1, CoreUtils::Convert(y));
    *values = mCoordinates;
    REFCOUNT_ADD(*values);
    return NOERROR;
}


//==============================================================================
// PropertyValuesHolder::PointFToInt32Array
//==============================================================================
PropertyValuesHolder::PointFToInt32Array::PointFToInt32Array()
{
    TypeConverter::constructor(EIID_IPointF, EIID_IArrayOf);
    CArrayOf::New(EIID_IInteger32, 2, (IArrayOf**)&mCoordinates);
}

ECode PropertyValuesHolder::PointFToInt32Array::Convert(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** values)
{
    VALIDATE_NOT_NULL(values);
    Float x, y;
    IPointF::Probe(value)->Get(&x, &y);
    mCoordinates->Set(0, CoreUtils::Convert(Math::Round(x)));
    mCoordinates->Set(1, CoreUtils::Convert(Math::Round(y)));
    *values = mCoordinates;
    REFCOUNT_ADD(*values);
    return NOERROR;
}


//==============================================================================
// PropertyValuesHolder
//==============================================================================
AutoPtr<ITypeEvaluator> PropertyValuesHolder::InitInt32Evaluator()
{
    AutoPtr<ITypeEvaluator> eva;
    CInt32Evaluator::New((ITypeEvaluator**)&eva);
    return eva;
}

AutoPtr<ITypeEvaluator> PropertyValuesHolder::InitFloatEvaluator()
{
    AutoPtr<ITypeEvaluator> eva;
    CFloatEvaluator::New((ITypeEvaluator**)&eva);
    return eva;
}

static AutoPtr< ArrayOf<InterfaceID> > Init_FLOAT_VARIANTS()
{
    AutoPtr< ArrayOf<InterfaceID> > variants = ArrayOf<InterfaceID>::Alloc(3);
    (*variants)[0] = EIID_IFloat; //Float.class
    (*variants)[1] = EIID_IDouble; //Double.class
    (*variants)[2] = EIID_IInteger32; //Integer.class
    return variants;
}

static AutoPtr< ArrayOf<InterfaceID> > Init_INTEGER_VARIANTS()
{
    AutoPtr< ArrayOf<InterfaceID> > variants = ArrayOf<InterfaceID>::Alloc(3);
    (*variants)[0] = EIID_IInteger32; //Integer.class
    (*variants)[1] = EIID_IFloat; //Float.class
    (*variants)[2] = EIID_IDouble; //Double.class
    return variants;
}

static AutoPtr< ArrayOf<InterfaceID> > Init_DOUBLE_VARIANTS()
{
    AutoPtr< ArrayOf<InterfaceID> > variants = ArrayOf<InterfaceID>::Alloc(3);
    (*variants)[0] = EIID_IDouble; //Double.class
    (*variants)[1] = EIID_IFloat; //Float.class
    (*variants)[2] = EIID_IInteger32; //Integer.class
    return variants;
}

AutoPtr<ITypeEvaluator> PropertyValuesHolder::sInt32Evaluator = InitInt32Evaluator();
AutoPtr<ITypeEvaluator> PropertyValuesHolder::sFloatEvaluator = InitFloatEvaluator();
AutoPtr< ArrayOf<InterfaceID> > PropertyValuesHolder::FLOAT_VARIANTS = Init_FLOAT_VARIANTS();
AutoPtr< ArrayOf<InterfaceID> > PropertyValuesHolder::INTEGER_VARIANTS = Init_INTEGER_VARIANTS();
AutoPtr< ArrayOf<InterfaceID> > PropertyValuesHolder::DOUBLE_VARIANTS = Init_DOUBLE_VARIANTS();

PropertyValuesHolder::ClassMethodMap PropertyValuesHolder::sSetterPropertyMap;
PropertyValuesHolder::ClassMethodMap PropertyValuesHolder::sGetterPropertyMap;

CAR_INTERFACE_IMPL_2(PropertyValuesHolder, Object, IPropertyValuesHolder, ICloneable);

PropertyValuesHolder::PropertyValuesHolder(
    /* [in] */ const String& propertyName)
    : mPropertyName(propertyName)
{
    mTmpValueArray = ArrayOf<IInterface*>::Alloc(1);
}

PropertyValuesHolder::PropertyValuesHolder(
    /* [in] */ IProperty* property)
    : mProperty(property)
{
    mTmpValueArray = ArrayOf<IInterface*>::Alloc(1);
    if (property != NULL) {
        property->GetName(&mPropertyName);
    }
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfInt32(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Int32>* values)
{
    AutoPtr<IPropertyValuesHolder> holder = new Int32PropertyValuesHolder(propertyName, values);
    return holder;
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfInt32(
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Int32>* values)
{
    AutoPtr<IPropertyValuesHolder> holder = new Int32PropertyValuesHolder(property, values);
    return holder;
}

ECode PropertyValuesHolder::OfMultiInt32(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<ArrayOf<Int32>* >* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    VALIDATE_NOT_NULL(holder);
    *holder = NULL;
    if (values->GetLength() < 2) {
        Logger::E(TAG, "At least 2 values must be supplied.");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 numParameters = 0;
    AutoPtr<ArrayOf<IInterface*> > arrays = ArrayOf<IInterface*>::Alloc(values->GetLength());
    for (Int32 i = 0; i < values->GetLength(); i++) {
        if ((*values)[i] == NULL) {
            Logger::E(TAG, "values must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        Int32 length = (*values)[i]->GetLength();
        if (i == 0) {
            numParameters = length;
        }
        else if (length != numParameters) {
            Logger::E(TAG, "Values must all have the same length");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IArrayOf> items;
        CArrayOf::New(EIID_IInteger32, length, (IArrayOf**)&items);
        for (Int32 j = 0; j < length; j++) {
            items->Set(j, CoreUtils::Convert((*((*values)[i]))[j]));
        }
        arrays->Set(i, items);
    }
    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(numParameters);
    AutoPtr<Int32ArrayEvaluator> evaluator = new Int32ArrayEvaluator(array);
    AutoPtr<IPropertyValuesHolder> v = new MultiInt32ValuesHolder(propertyName, NULL, evaluator, arrays);
    *holder = v;
    REFCOUNT_ADD(*holder);
    return NOERROR;
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfMultiInt32(
    /* [in] */ const String& propertyName,
    /* [in] */ IPath* path)
{
    AutoPtr<IKeyframes> keyframes = IKeyframes::Probe(KeyframeSet::OfPath(path));
    AutoPtr<PointFToInt32Array> converter = new PointFToInt32Array();
    return new MultiInt32ValuesHolder(propertyName, converter, NULL, keyframes);
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfMultiInt32(
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    return new MultiInt32ValuesHolder(propertyName, converter, evaluator, values);
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfMultiInt32(
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IKeyframe*>* values)
{
    AutoPtr<IKeyframeSet> keyframeSet = KeyframeSet::OfKeyframe(values);
    return new MultiInt32ValuesHolder(propertyName, converter, evaluator, IKeyframes::Probe(keyframeSet));
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfFloat(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Float>* values)
{
    AutoPtr<IPropertyValuesHolder> holder = new FloatPropertyValuesHolder(propertyName, values);
    return holder;
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfFloat(
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Float>* values)
{
    AutoPtr<IPropertyValuesHolder> holder = new FloatPropertyValuesHolder(property, values);
    return holder;
}

ECode PropertyValuesHolder::OfMultiFloat(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<ArrayOf<Float>* >* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    VALIDATE_NOT_NULL(holder);
    *holder = NULL;
    if (values->GetLength() < 2) {
        Logger::E(TAG, "At least 2 values must be supplied");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 numParameters = 0;
    AutoPtr<ArrayOf<IInterface*> > arrays = ArrayOf<IInterface*>::Alloc(values->GetLength());
    for (Int32 i = 0; i < values->GetLength(); i++) {
        if ((*values)[i] == NULL) {
            Logger::E(TAG, "values must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        Int32 length = (*values)[i]->GetLength();
        if (i == 0) {
            numParameters = length;
        }
        else if (length != numParameters) {
            Logger::E(TAG, "Values must all have the same length");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        AutoPtr<IArrayOf> items;
        CArrayOf::New(EIID_IFloat, length, (IArrayOf**)&items);
        for (Int32 n = 0; n < length; n++) {
            items->Set(n, CoreUtils::Convert((*((*values)[i]))[n]));
        }
        arrays->Set(i, items);
    }
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(numParameters);
    AutoPtr<FloatArrayEvaluator> evaluator = new FloatArrayEvaluator(array);
    AutoPtr<IPropertyValuesHolder> v = new MultiFloatValuesHolder(propertyName, NULL, evaluator, arrays);
    *holder = v;
    REFCOUNT_ADD(*holder);
    return NOERROR;
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfMultiFloat(
    /* [in] */ const String& propertyName,
    /* [in] */ IPath* path)
{
    AutoPtr<IKeyframes> keyframes = IKeyframes::Probe(KeyframeSet::OfPath(path));
    AutoPtr<PointFToFloatArray> converter = new PointFToFloatArray();
    return new MultiFloatValuesHolder(propertyName, converter, NULL, keyframes);
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfMultiFloat(
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    return new MultiFloatValuesHolder(propertyName, converter, evaluator, values);
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfMultiFloat(
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IKeyframe*>* values)
{
    AutoPtr<IKeyframeSet> keyframeSet = KeyframeSet::OfKeyframe(values);
    return new MultiFloatValuesHolder(propertyName, converter, evaluator, IKeyframes::Probe(keyframeSet));
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfObject(
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<IPropertyValuesHolder> holder = new PropertyValuesHolder(propertyName);
    holder->SetObjectValues(values);
    holder->SetEvaluator(evaluator);
    return holder;
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfObject(
    /* [in] */ const String& propertyName,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ IPath* path)
{
    AutoPtr<PropertyValuesHolder> pvh = new PropertyValuesHolder(propertyName);
    pvh->mKeyframes = IKeyframes::Probe(KeyframeSet::OfPath(path));
    pvh->mValueType = EIID_IPointF;
    pvh->SetConverter(converter);
    return pvh;
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfObject(
    /* [in] */ IProperty* property,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<IPropertyValuesHolder> holder = new PropertyValuesHolder(property);
    holder->SetObjectValues(values);
    holder->SetEvaluator(evaluator);
    return holder;
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfObject(
    /* [in] */ IProperty* property,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<PropertyValuesHolder> pvh = new PropertyValuesHolder(property);
    pvh->SetConverter(converter);
    pvh->SetObjectValues(values);
    pvh->SetEvaluator(evaluator);
    return pvh;
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfObject(
    /* [in] */ IProperty* property,
    /* [in] */ ITypeConverter* converter,
    /* [in] */ IPath* path)
{
    AutoPtr<PropertyValuesHolder> pvh = new PropertyValuesHolder(property);
    pvh->mKeyframes = IKeyframes::Probe(KeyframeSet::OfPath(path));
    pvh->mValueType = EIID_IPointF;
    pvh->SetConverter(converter);
    return pvh;
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfKeyframe(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<IKeyframe*>* values)
{
    AutoPtr<IKeyframeSet> keyframeSet = KeyframeSet::OfKeyframe(values);
    return OfKeyframes(propertyName, IKeyframes::Probe(keyframeSet));
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfKeyframe(
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<IKeyframe*>* values)
{
    AutoPtr<IKeyframeSet> keyframeSet = KeyframeSet::OfKeyframe(values);
    return OfKeyframes(property, IKeyframes::Probe(keyframeSet));
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfKeyframes(
    /* [in] */ const String& propertyName,
    /* [in] */ IKeyframes* keyframes)
{
    if (IInt32Keyframes::Probe(keyframes) != NULL) {
        return new Int32PropertyValuesHolder(propertyName, IInt32Keyframes::Probe(keyframes));
    }
    else if (IFloatKeyframes::Probe(keyframes) != NULL) {
        return new FloatPropertyValuesHolder(propertyName, IFloatKeyframes::Probe(keyframes));
    }
    else {
        AutoPtr<PropertyValuesHolder> pvh = new PropertyValuesHolder(propertyName);
        pvh->mKeyframes = keyframes;
        keyframes->GetType(&pvh->mValueType);
        return pvh;
    }
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfKeyframes(
    /* [in] */ IProperty* property,
    /* [in] */ IKeyframes* keyframes)
{
    if (IInt32Keyframes::Probe(keyframes) != NULL) {
        return new Int32PropertyValuesHolder(property, IInt32Keyframes::Probe(keyframes));
    }
    else if (IFloatKeyframes::Probe(keyframes) != NULL) {
        return new FloatPropertyValuesHolder(property, IFloatKeyframes::Probe(keyframes));
    }
    else {
        AutoPtr<PropertyValuesHolder> pvh = new PropertyValuesHolder(property);
        pvh->mKeyframes = keyframes;
        keyframes->GetType(&pvh->mValueType);
        return pvh;
    }
}

ECode PropertyValuesHolder::SetInt32Values(
    /* [in] */ ArrayOf<Int32>* values)
{
    mValueType = EIID_IInteger32;
    mKeyframes = IKeyframes::Probe(KeyframeSet::OfInt32(values));
    return NOERROR;
}

ECode PropertyValuesHolder::SetFloatValues(
    /* [in] */ ArrayOf<Float>* values)
{
    mValueType = EIID_IFloat;
    mKeyframes = IKeyframes::Probe(KeyframeSet::OfFloat(values));
    return NOERROR;
}

ECode PropertyValuesHolder::SetKeyframes(
    /* [in] */ ArrayOf<IKeyframe*>* values)
{
    Int32 numKeyframes = values->GetLength();
    AutoPtr<ArrayOf<IKeyframe*> > keyframes = ArrayOf<IKeyframe*>::Alloc(Math::Max(numKeyframes, 2));
    (*values)[0]->GetType(&mValueType);
    for (Int32 i = 0; i < numKeyframes; ++i) {
        keyframes->Set(i, (*values)[i]);
    }
    AutoPtr<KeyframeSet> set = new KeyframeSet();
    set->constructor(keyframes);
    mKeyframes = set;
    return NOERROR;
}

ECode PropertyValuesHolder::SetObjectValues(
    /* [in] */ ArrayOf<IInterface*>* values)
{
    (*values)[0]->GetInterfaceID((*values)[0], &mValueType);
    mKeyframes = IKeyframes::Probe(KeyframeSet::OfObject(values));
    if (mEvaluator != NULL) {
        mKeyframes->SetEvaluator(mEvaluator);
    }
    return NOERROR;
}

ECode PropertyValuesHolder::SetConverter(
    /* [in] */ ITypeConverter* converter)
{
    mConverter = converter;
    return NOERROR;
}

AutoPtr<IMethodInfo> PropertyValuesHolder::GetPropertyFunction(
    /* [in] */ IClassInfo* targetClass,
    /* [in] */ const String& prefix,
    /* [in] */ const InterfaceID& valueType)
{
    // TODO: faster implementation...
    AutoPtr<IMethodInfo> returnVal;
    String methodName = GetMethodName(prefix, mPropertyName);
    AutoPtr< ArrayOf<InterfaceID> > typeVariants;
    if (valueType == ECLSID_CFloat) {
        typeVariants = FLOAT_VARIANTS;
    }
    else if (valueType == ECLSID_CInteger32) {
        typeVariants = INTEGER_VARIANTS;
    }
    else if (valueType == ECLSID_CDouble) {
        typeVariants = DOUBLE_VARIANTS;
    }
    else {
        typeVariants = ArrayOf<InterfaceID>::Alloc(1);
        (*typeVariants)[0] = valueType;
    }
    for (Int32 i = 0; i < typeVariants->GetLength(); i++) {
        AutoPtr<IMethodInfo> method = GetMethodInfo(targetClass, methodName, prefix, typeVariants);
        if (method == NULL) continue;
        if (mConverter == NULL && prefix.Equals("Set")) {
            // change the value type to suit
            mValueType = valueType;
        }
        return returnVal;
    }
    // If we got here, then no appropriate function was found

    if (returnVal == NULL) {
        String className;
        targetClass->GetName(&className);
        Logger::E(TAG, "Error: Method [%s] is not found on target class %s",
            methodName.string(), className.string());
    }

    return returnVal;
}

AutoPtr<IMethodInfo> PropertyValuesHolder::SetupSetterOrGetter(
    /* [in] */ IClassInfo* targetClass,
    /* [in] */ PropertyValuesHolder::ClassMethodMap& propertyMapMap,
    /* [in] */ const String& prefix,
    /* [in] */ const InterfaceID& valueType)
{
    AutoPtr<IMethodInfo> setterOrGetter;
    {
        AutoLock lock(mPropertyMapLock);

        AutoPtr<IClassInfo> key = targetClass;
        AutoPtr< MethodMap > propertyMap;
        typename ClassMethodMap::Iterator it = propertyMapMap.Find(key);
        if (it != propertyMapMap.End()) {
            propertyMap = it->mSecond;
        }
        if (propertyMap != NULL) {
            typename MethodMap::Iterator it2 = propertyMap->Find(mPropertyName);
            if (it2 != propertyMap->End()) {
                setterOrGetter = it2->mSecond;
            }
        }
        if (setterOrGetter == NULL) {
            setterOrGetter = GetPropertyFunction(targetClass, prefix, valueType);
            if (propertyMap == NULL) {
                propertyMap = new MethodMap();
                propertyMapMap[key] = propertyMap;
            }
            (*propertyMap)[mPropertyName] = setterOrGetter;
        }
        return setterOrGetter;
    }
}

ECode PropertyValuesHolder::SetupSetter(
    /* [in] */ IClassInfo* targetClass)
{
    InterfaceID propertyType = mValueType;
    if (mConverter != NULL) {
        mConverter->GetTargetType(&propertyType);
    }
    mSetter = SetupSetterOrGetter(targetClass, sSetterPropertyMap, String("Set"), propertyType);
    return NOERROR;
}

ECode PropertyValuesHolder::SetupGetter(
     /* [in] */ IClassInfo* targetClass)
{
    InterfaceID propertyType = mValueType;
    if (mConverter != NULL) {
        mConverter->GetTargetType(&propertyType);
    }
    mGetter = SetupSetterOrGetter(targetClass, sGetterPropertyMap, String("Get"), propertyType);
    return NOERROR;
}

ECode PropertyValuesHolder::SetupSetterAndGetter(
    /* [in] */ IInterface* target)
{
    mKeyframes->InvalidateCache();
    if (mProperty != NULL) {
        // check to make sure that mProperty is on the class of target
        AutoPtr<IInterface> testValue;
        AutoPtr<ArrayOf<IKeyframe*> > keyframes;
        mKeyframes->GetKeyframes((ArrayOf<IKeyframe*>**)&keyframes);
        Int32 keyframeCount = keyframes == NULL ? 0 : keyframes->GetLength();
        for (Int32 i = 0; i < keyframeCount; i++) {
            AutoPtr<IKeyframe> kf = (*keyframes)[i];
            Boolean has = FALSE;
            kf->HasValue(&has);
            if (!has || ((Keyframe*)kf.Get())->ValueWasSetOnStart()) {
                if (testValue == NULL) {
                    AutoPtr<IInterface> v;
                    mProperty->Get(target, (IInterface**)&v);
                    ECode ec = ConvertBack(v, (IInterface**)&testValue);
                    if (FAILED(ec)) {
                        String name;
                        mProperty->GetName(&name);
                        Logger::W(TAG, "No such property (%s) on target object %s. Trying reflection instead",
                                name.string(), TO_CSTR(target));
                        mProperty = NULL;
                        goto NEXT;
                    }
                }
                kf->SetValue(testValue);
                ((Keyframe*)kf.Get())->SetValueWasSetOnStart(TRUE);
            }
        }
        return NOERROR;
    }
NEXT:
    if (mSetter == NULL) {
        SetupSetter(GetClassInfo(target));
    }
    AutoPtr<ArrayOf<IKeyframe*> > frames;
    mKeyframes->GetKeyframes((ArrayOf<IKeyframe*>**)&frames);
    Int32 keyframeCount = frames == NULL ? 0 : frames->GetLength();
    for(Int32 i = 0; i < keyframeCount; i++) {
        IKeyframe* kf = (*frames)[i];
        Boolean has = FALSE;
        kf->HasValue(&has);
        if (!has || ((Keyframe*)kf)->ValueWasSetOnStart()) {
            if (mGetter == NULL) {
                SetupGetter(GetClassInfo(target));
                if (mGetter == NULL) {
                    // Already logged the error - just return to avoid NPE
                    return NOERROR;
                }
            }
            AutoPtr<IArgumentList> arg;
            mGetter->CreateArgumentList((IArgumentList**)&arg);
            AutoPtr<IParamInfo> paramInfo;
            mGetter->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
            AutoPtr<IDataTypeInfo> dataTypeInfo;
            paramInfo->GetTypeInfo((IDataTypeInfo**)&dataTypeInfo);
            CarDataType carType;
            dataTypeInfo->GetDataType(&carType);
            if (mValueType == ECLSID_CInteger32) {
                if (carType == CarDataType_Int32) {
                    Int32 getRst;
                    arg->SetOutputArgumentOfInt32Ptr(0, &getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));
                    AutoPtr<IInteger32> carRst;
                    CInteger32::New(getRst, (IInteger32**)&carRst);

                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(carRst, (IInterface**)&value));
                    kf->SetValue(value);
                    ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
                }
                else if(carType == CarDataType_Interface) {
                    AutoPtr<IInterface> getRst;
                    arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));

                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(IInteger32::Probe(getRst), (IInterface**)&value));
                    kf->SetValue(value);
                    ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
                }
                else {
                    assert(0);
                }
            }
            else if (mValueType == ECLSID_CFloat) {
                if (carType == CarDataType_Float) {
                    Float getRst;
                    arg->SetOutputArgumentOfFloatPtr(0, &getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));
                    AutoPtr<IFloat> carRst;
                    CFloat::New(getRst, (IFloat**)&carRst);

                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(carRst, (IInterface**)&value));
                    kf->SetValue(value);
                    ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
                }
                else if (carType == CarDataType_Interface) {
                    AutoPtr<IInterface> getRst;
                    arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));

                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(IFloat::Probe(getRst), (IInterface**)&value));
                    kf->SetValue(value);
                    ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
                }
                else{
                    assert(0);
                }
            }
            else if (mValueType == ECLSID_CDouble) {
                if(carType == CarDataType_Double) {
                    Double getRst;
                    arg->SetOutputArgumentOfDoublePtr(0, &getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));
                    AutoPtr<IDouble> carRst;
                    CDouble::New(getRst, (IDouble**)&carRst);

                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(carRst, (IInterface**)&value));
                    kf->SetValue(value);
                    ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
                }
                else if(carType == CarDataType_Interface) {
                    AutoPtr<IInterface> getRst;
                    arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));

                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(IDouble::Probe(getRst), (IInterface**)&value));
                    kf->SetValue(value);
                    ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
                }
                else {
                    assert(0);
                }
            }
            else {
                if (carType == CarDataType_Float) {
                    Float getRst;
                    arg->SetOutputArgumentOfFloatPtr(0, &getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));
                    AutoPtr<IFloat> carRst;
                    CFloat::New(getRst, (IFloat**)&carRst);

                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(carRst, (IInterface**)&value));
                    kf->SetValue(value);
                    ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
                }
                else if (carType == CarDataType_Int32) {
                    Int32 getRst;
                    arg->SetOutputArgumentOfInt32Ptr(0, &getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));
                    AutoPtr<IInteger32> carRst;
                    CInteger32::New(getRst, (IInteger32**)&carRst);

                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(carRst, (IInterface**)&value));
                    kf->SetValue(value);
                    ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
                }
                else if(carType == CarDataType_Interface) {
                    AutoPtr<IInterface> getRst;
                    arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));
                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(getRst, (IInterface**)&value));
                    kf->SetValue(value);
                    ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
                }
                else {
                    assert(0 && "TODO please add process to carType!");
                }
            }
        }
    }

    return NOERROR;
}

ECode PropertyValuesHolder::ConvertBack(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** outValue)
{
    VALIDATE_NOT_NULL(outValue);
    *outValue = NULL;
    if (mConverter != NULL) {
        IBidirectionalTypeConverter* btc = IBidirectionalTypeConverter::Probe(mConverter);
        if (btc == NULL) {
            Logger::E(TAG, "Converter %s must be a BidirectionalTypeConverter", TO_CSTR(mConverter));
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        btc->ConvertBack(value, outValue);
    }
    return NOERROR;
}

ECode PropertyValuesHolder::SetupValue(
     /* [in] */ IInterface* target,
     /* [in] */ IKeyframe* kf)
{
    if (mProperty != NULL) {
        AutoPtr<IInterface> value;
        mProperty->Get(target, (IInterface**)&value);
        AutoPtr<IInterface> cValue;
        FAIL_RETURN(ConvertBack(value, (IInterface**)&cValue));
        kf->SetValue(cValue);
    }
    if (mGetter == NULL) {
        SetupGetter(GetClassInfo(target));
        if (mGetter == NULL) {
            // Already logged the error - just return to avoid NPE
            return NOERROR;
        }
    }

    AutoPtr<IArgumentList> arg;
    mGetter->CreateArgumentList((IArgumentList**)&arg);
    AutoPtr<IParamInfo> paramInfo;
    mGetter->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
    AutoPtr<IDataTypeInfo> dataTypeInfo;
    paramInfo->GetTypeInfo((IDataTypeInfo**)&dataTypeInfo);
    CarDataType carType;
    dataTypeInfo->GetDataType(&carType);
    if (mValueType == ECLSID_CInteger32) {
        if (carType == CarDataType_Int32) {
            Int32 getRst;
            arg->SetOutputArgumentOfInt32Ptr(0, &getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));
            AutoPtr<IInteger32> carRst;
            CInteger32::New(getRst, (IInteger32**)&carRst);

            AutoPtr<IInterface> cValue;
            FAIL_RETURN(ConvertBack(carRst, (IInterface**)&cValue));
            kf->SetValue(cValue);
        }
        else if (carType == CarDataType_Interface) {
            AutoPtr<IInterface> getRst;
            arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));

            AutoPtr<IInterface> cValue;
            FAIL_RETURN(ConvertBack(IInteger32::Probe(getRst), (IInterface**)&cValue));
            kf->SetValue(cValue);
        }
        else{
            assert(0);
        }
    }
    else if (mValueType == ECLSID_CFloat) {
        if (carType == CarDataType_Float) {
            Float getRst;
            arg->SetOutputArgumentOfFloatPtr(0, &getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));
            AutoPtr<IFloat> carRst;
            CFloat::New(getRst, (IFloat**)&carRst);

            AutoPtr<IInterface> cValue;
            FAIL_RETURN(ConvertBack(carRst, (IInterface**)&cValue));
            kf->SetValue(cValue);
        }
        else if (carType == CarDataType_Interface) {
            AutoPtr<IInterface> getRst;
            arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));

            AutoPtr<IInterface> cValue;
            FAIL_RETURN(ConvertBack(IFloat::Probe(getRst), (IInterface**)&cValue));
            kf->SetValue(cValue);
        }
        else{
            assert(0);
        }
    }
    else if(mValueType == ECLSID_CDouble) {
        if (carType == CarDataType_Double) {
            Double getRst;
            arg->SetOutputArgumentOfDoublePtr(0, &getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));
            AutoPtr<IDouble> carRst;
            CDouble::New(getRst, (IDouble**)&carRst);

            AutoPtr<IInterface> cValue;
            FAIL_RETURN(ConvertBack(carRst, (IInterface**)&cValue));
            kf->SetValue(cValue);
        }
        else if (carType == CarDataType_Interface) {
            AutoPtr<IInterface> getRst;
            arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));

            AutoPtr<IInterface> cValue;
            FAIL_RETURN(ConvertBack(IDouble::Probe(getRst), (IInterface**)&cValue));
            kf->SetValue(cValue);
        }
        else{
            assert(0);
        }
    }
    else {
        if (carType == CarDataType_Float) {
            Float getRst;
            arg->SetOutputArgumentOfFloatPtr(0, &getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));
            AutoPtr<IFloat> carRst;
            CFloat::New(getRst, (IFloat**)&carRst);

            AutoPtr<IInterface> value;
            FAIL_RETURN(ConvertBack(carRst, (IInterface**)&value));
            kf->SetValue(value);
            ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
        }
        else if (carType == CarDataType_Int32) {
            Int32 getRst;
            arg->SetOutputArgumentOfInt32Ptr(0, &getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));
            AutoPtr<IInteger32> carRst;
            CInteger32::New(getRst, (IInteger32**)&carRst);

            AutoPtr<IInterface> value;
            FAIL_RETURN(ConvertBack(carRst, (IInterface**)&value));
            kf->SetValue(value);
            ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
        }
        else if (carType == CarDataType_Interface) {
            AutoPtr<IInterface> getRst;
            arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));

            AutoPtr<IInterface> cValue;
            FAIL_RETURN(ConvertBack(getRst, (IInterface**)&cValue));
            kf->SetValue(cValue);
        }
        else {
            assert(0 && "TODO please add process to carType!");
        }
    }
    return NOERROR;
}

ECode PropertyValuesHolder::SetupStartValue(
    /* [in] */ IInterface* target)
{
    AutoPtr<ArrayOf<IKeyframe*> > frames;
    mKeyframes->GetKeyframes((ArrayOf<IKeyframe*>**)&frames);
    if (frames->GetLength() > 0) {
        return SetupValue(target, (*frames)[0]);
    }
    return NOERROR;
}
ECode PropertyValuesHolder::SetupEndValue(
    /* [in] */ IInterface* target)
{
    AutoPtr<ArrayOf<IKeyframe*> > frames;
    mKeyframes->GetKeyframes((ArrayOf<IKeyframe*>**)&frames);
    Int32 length = frames->GetLength();
    if (length > 0) {
        SetupValue(target, (*frames)[length - 1]);
    }
    return NOERROR;
}

ECode PropertyValuesHolder::CloneImpl(
    /* [in] */ PropertyValuesHolder* holder)
{
    holder->mPropertyName = mPropertyName;
    holder->mProperty = mProperty;
    AutoPtr<IInterface> obj;
    ICloneable::Probe(mKeyframes)->Clone((IInterface**)&obj);
    holder->mKeyframes = IKeyframes::Probe(obj);
    holder->mEvaluator = mEvaluator;
    holder->mSetter = mSetter;
    holder->mValueType = mValueType;
    holder->mGetter = mGetter;
    holder->mAnimatedValue = mAnimatedValue;
    return NOERROR;
}

ECode PropertyValuesHolder::SetAnimatedValue(
    /* [in] */ IInterface* target)
{
    AutoPtr<IInterface> animatedValue;
    GetAnimatedValue((IInterface**)&animatedValue);
    if (mProperty != NULL) {
        mProperty->Set(target, animatedValue);
    }
    if (mSetter != NULL) {
        AutoPtr<IArgumentList> args;
        mSetter->CreateArgumentList((IArgumentList**)&args);
        AutoPtr<IParamInfo> paramInfo;
        mSetter->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
        AutoPtr<IDataTypeInfo> dataTypeInfo;
        paramInfo->GetTypeInfo((IDataTypeInfo**)&dataTypeInfo);
        CarDataType carType;
        dataTypeInfo->GetDataType(&carType);
        if (mValueType == ECLSID_CInteger32) {
            if(carType == CarDataType_Int32) {
                Int32 setRst;
                AutoPtr<IInteger32> valTemp = IInteger32::Probe(animatedValue);
                if(!valTemp)
                    return NOERROR;
                valTemp->GetValue(&setRst);
                args->SetInputArgumentOfInt32(0, setRst);
                FAIL_RETURN(mSetter->Invoke(target, args));
            }
            else if(carType == CarDataType_Interface) {
                if(!animatedValue)
                    return NOERROR;
                args->SetInputArgumentOfObjectPtr(0, animatedValue);
                FAIL_RETURN(mSetter->Invoke(target, args));
            }
            else{
                return NOERROR;
            }
        }
        else if(mValueType == ECLSID_CFloat) {
            if (carType == CarDataType_Float) {
                Float setRst;
                AutoPtr<IFloat> valTemp = IFloat::Probe(animatedValue);
                if (!valTemp)
                    return NOERROR;
                valTemp->GetValue(&setRst);
                args->SetInputArgumentOfInt32(0, setRst);
                FAIL_RETURN(mSetter->Invoke(target, args));
            }
            else if (carType == CarDataType_Interface) {
                if(!animatedValue)
                    return NOERROR;
                args->SetInputArgumentOfObjectPtr(0, animatedValue);
                FAIL_RETURN(mSetter->Invoke(target, args));
            }
            else{
                return NOERROR;
            }
        }
        else if (mValueType == ECLSID_CDouble) {
            if (carType == CarDataType_Double) {
                Double setRst;
                AutoPtr<IDouble> valTemp = IDouble::Probe(animatedValue);
                if (!valTemp)
                    return NOERROR;
                valTemp->GetValue(&setRst);
                args->SetInputArgumentOfInt32(0, setRst);
                FAIL_RETURN(mSetter->Invoke(target, args));
            }
            else if (carType == CarDataType_Interface) {
                if(!animatedValue)
                    return NOERROR;
                args->SetInputArgumentOfObjectPtr(0, animatedValue);
                FAIL_RETURN(mSetter->Invoke(target, args));
            }
            else {
                return NOERROR;
            }
        }
        else {
            if (!animatedValue)
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            args->SetInputArgumentOfObjectPtr(0, animatedValue);
            FAIL_RETURN(mSetter->Invoke(target, args));
        }
    }
    return NOERROR;
}

ECode PropertyValuesHolder::Init()
{
    if (mEvaluator == NULL) {
        // We already handle int and float automatically, but not their Object
        // equivalents
        mEvaluator = (mValueType == ECLSID_CInteger32) ? sInt32Evaluator :
            (mValueType == ECLSID_CFloat) ? sFloatEvaluator : NULL;
    }
    if (mEvaluator != NULL) {
        // KeyframeSet knows how to evaluate the common types - only give it a custom
        // evaluator if one has been set on this class
        mKeyframes->SetEvaluator(mEvaluator);
    }
    return NOERROR;
}

ECode PropertyValuesHolder::SetEvaluator(
    /* [in] */ ITypeEvaluator* evaluator)
{
    mEvaluator = evaluator;
    mKeyframes->SetEvaluator(evaluator);
    return NOERROR;
}

ECode PropertyValuesHolder::CalculateValue(
    /* [in] */ Float fraction)
{
    AutoPtr<IInterface> value;
    mKeyframes->GetValue(fraction, (IInterface**)&value);
    if (mConverter == NULL) {
        mAnimatedValue = value;
    }
    else {
        mAnimatedValue = NULL;
        mConverter->Convert(value, (IInterface**)&mAnimatedValue);
    }
    return NOERROR;
}

ECode PropertyValuesHolder::SetPropertyName(
    /* [in] */ const String& propertyName)
{
    mPropertyName = propertyName;
    return NOERROR;
}

ECode PropertyValuesHolder::SetProperty(
    /* [in] */ IProperty* property)
{
    mProperty = property;
    return NOERROR;
}

ECode PropertyValuesHolder::GetPropertyName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mPropertyName;
    return NOERROR;
}

ECode PropertyValuesHolder::GetAnimatedValue(
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(value);
    *value = mAnimatedValue;
    REFCOUNT_ADD(*value);
    return NOERROR;
}

ECode PropertyValuesHolder::Clone(
    /* [out] */ IInterface** holder)
{
    AutoPtr<PropertyValuesHolder> v = new PropertyValuesHolder(mPropertyName);
    CloneImpl(v);
    *holder = (IPropertyValuesHolder*)v.Get();
    REFCOUNT_ADD(*holder)
    return NOERROR;
}

ECode PropertyValuesHolder::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    String tmp = Object::ToString(mKeyframes);
    *str = mPropertyName + String(": ") + tmp;
    return NOERROR;
}

String PropertyValuesHolder::GetMethodName(
    /* [in] */ const String& prefix,
    /* [in] */ const String& propertyName)
{
    if (propertyName.IsNullOrEmpty()) {
        // shouldn't get here
        return prefix;
    }
    String temp = propertyName.ToUpperCase(0, 1);
    return prefix + temp;
}

AutoPtr<IMethodInfo> PropertyValuesHolder::nGetInt32Method(
    /* [in] */ IClassInfo* targetClass,
    /* [in] */ const String& methodName)
{
    AutoPtr<IMethodInfo> mi;
    targetClass->GetMethodInfo(methodName, String("(I32)E"), (IMethodInfo**)&mi);
    return mi;
}

AutoPtr<IMethodInfo> PropertyValuesHolder::nGetFloatMethod(
    /* [in] */ IClassInfo* targetClass,
    /* [in] */ const String& methodName)
{
    AutoPtr<IMethodInfo> mi;
    targetClass->GetMethodInfo(methodName, String("(F)E"), (IMethodInfo**)&mi);
    return mi;
}

static AutoPtr<IMethodInfo> GetMultiparameterMethod(
    /* [in] */ IClassInfo* targetClass,
    /* [in] */ const String& methodName,
    /* [in] */ Int32 parameterCount,
    /* [in] */ const String& parameterType)
{
    StringBuilder sb;
    sb += "(";
    for (Int32 i = 0; i < parameterCount; i++) {
        sb += parameterType;
    }
    sb += ")E";
    AutoPtr<IMethodInfo> mi;
    targetClass->GetMethodInfo(methodName, sb.ToString(), (IMethodInfo**)&mi);
    return mi;
}

AutoPtr<IMethodInfo> PropertyValuesHolder::nGetMultipleInt32Method(
    /* [in] */ IClassInfo* targetClass,
    /* [in] */ const String& methodName,
    /* [in] */ Int32 numParams)
{
    return GetMultiparameterMethod(targetClass, methodName, numParams, String("I32"));
}

AutoPtr<IMethodInfo> PropertyValuesHolder::nGetMultipleFloatMethod(
    /* [in] */ IClassInfo* targetClass,
    /* [in] */ const String& methodName,
    /* [in] */ Int32 numParams)
{
    return GetMultiparameterMethod(targetClass, methodName, numParams, String("F"));
}

void PropertyValuesHolder::nCallInt32Method(
    /* [in] */ IInterface* target,
    /* [in] */ IMethodInfo* methodID,
    /* [in] */ Int32 arg)
{
    AutoPtr<IArgumentList> args;
    methodID->CreateArgumentList((IArgumentList**)&args);
    args->SetInputArgumentOfInt32(0, arg);
    methodID->Invoke(target, args);
}

void PropertyValuesHolder::nCallFloatMethod(
    /* [in] */ IInterface* target,
    /* [in] */ IMethodInfo* methodID,
    /* [in] */ Float arg)
{
    AutoPtr<IArgumentList> args;
    methodID->CreateArgumentList((IArgumentList**)&args);
    args->SetInputArgumentOfFloat(0, arg);
    methodID->Invoke(target, args);
}

void PropertyValuesHolder::nCallTwoInt32Method(
    /* [in] */ IInterface* target,
    /* [in] */ IMethodInfo* methodID,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2)
{
    AutoPtr<IArgumentList> args;
    methodID->CreateArgumentList((IArgumentList**)&args);
    args->SetInputArgumentOfInt32(0, arg1);
    args->SetInputArgumentOfInt32(1, arg2);
    methodID->Invoke(target, args);
}

void PropertyValuesHolder::nCallFourInt32Method(
    /* [in] */ IInterface* target,
    /* [in] */ IMethodInfo* methodID,
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ Int32 arg3,
    /* [in] */ Int32 arg4)
{
    AutoPtr<IArgumentList> args;
    methodID->CreateArgumentList((IArgumentList**)&args);
    args->SetInputArgumentOfInt32(0, arg1);
    args->SetInputArgumentOfInt32(1, arg2);
    args->SetInputArgumentOfInt32(2, arg3);
    args->SetInputArgumentOfInt32(3, arg4);
    methodID->Invoke(target, args);
}

void PropertyValuesHolder::nCallMultipleInt32Method(
    /* [in] */ IInterface* target,
    /* [in] */ IMethodInfo* methodID,
    /* [in] */ ArrayOf<Int32>* arg)
{
    AutoPtr<IArgumentList> args;
    methodID->CreateArgumentList((IArgumentList**)&args);
    args->SetInputArgumentOfCarArray(0, arg);
    methodID->Invoke(target, args);
}

void PropertyValuesHolder::nCallTwoFloatMethod(
    /* [in] */ IInterface* target,
    /* [in] */ IMethodInfo* methodID,
    /* [in] */ Float arg1,
    /* [in] */ Float arg2)
{
    AutoPtr<IArgumentList> args;
    methodID->CreateArgumentList((IArgumentList**)&args);
    args->SetInputArgumentOfFloat(0, arg1);
    args->SetInputArgumentOfFloat(1, arg2);
    methodID->Invoke(target, args);
}

void PropertyValuesHolder::nCallFourFloatMethod(
    /* [in] */ IInterface* target,
    /* [in] */ IMethodInfo* methodID,
    /* [in] */ Float arg1,
    /* [in] */ Float arg2,
    /* [in] */ Float arg3,
    /* [in] */ Float arg4)
{
    AutoPtr<IArgumentList> args;
    methodID->CreateArgumentList((IArgumentList**)&args);
    args->SetInputArgumentOfFloat(0, arg1);
    args->SetInputArgumentOfFloat(1, arg2);
    args->SetInputArgumentOfFloat(2, arg3);
    args->SetInputArgumentOfFloat(3, arg4);
    methodID->Invoke(target, args);
}

void PropertyValuesHolder::nCallMultipleFloatMethod(
    /* [in] */ IInterface* target,
    /* [in] */ IMethodInfo* methodID,
    /* [in] */ ArrayOf<Float>* arg)
{
    AutoPtr<IArgumentList> args;
    methodID->CreateArgumentList((IArgumentList**)&args);
    args->SetInputArgumentOfCarArray(0, arg);
    methodID->Invoke(target, args);
}

AutoPtr<IClassInfo> PropertyValuesHolder::GetClassInfo(
    /* [in] */ IInterface* o)
{
    AutoPtr<IClassInfo> klass;
    CObject::ReflectClassInfo(o, (IClassInfo**)&klass);
    if (klass == NULL) {
        Logger::E(TAG, "%s is not a CAR Object.", TO_CSTR(o));
        assert(0 && "It's not a CAR Object.");
    }
    return klass;
}

// if (valueType == ECLSID_CPointF) {
//     length = 1;
//     signature = "(LElastos/Droid/Graphics/IPointF;)E";
// }
// else {
//     length = 0;
//     assert(0 && "TODO");
// }

AutoPtr<IMethodInfo> PropertyValuesHolder::GetMethodInfo(
    /* [in] */ IClassInfo* targetClass,
    /* [in] */ const String& method,
    /* [in] */ const String& prefix,
    /* [in] */ ArrayOf<InterfaceID>* args)
{
    if (method.IsNullOrEmpty()) return NULL;

    Int32 mtdCount;
    targetClass->GetMethodCount(&mtdCount);
    AutoPtr< ArrayOf<IMethodInfo*> > methodInfos = ArrayOf<IMethodInfo*>::Alloc(mtdCount);
    targetClass->GetAllMethodInfos(methodInfos);
    for (Int32 i = 0; i < mtdCount; i++) {
        IMethodInfo* methodInfo = (*methodInfos)[i];
        String methodName;
        methodInfo->GetName(&methodName);
        if (!method.Equals(methodName)) continue;
        Int32 paramCount;
        methodInfo->GetParamCount(&paramCount);
        if (paramCount != args->GetLength()) continue;
        AutoPtr< ArrayOf<IParamInfo*> > paramInfos = ArrayOf<IParamInfo*>::Alloc(paramCount);
        methodInfo->GetAllParamInfos(paramInfos);
        for (Int32 j = 0; j < paramCount; ++j) {
            IParamInfo* paramInfo = (*paramInfos)[j];
            ParamIOAttribute ioAttr;
            paramInfo->GetIOAttribute(&ioAttr);
            if (prefix.Equals(String("Set")) && (ioAttr != ParamIOAttribute_In)) break;
            else if (ioAttr == ParamIOAttribute_In) break;
            AutoPtr<IDataTypeInfo> dataTypeInfo;
            paramInfo->GetTypeInfo((IDataTypeInfo**)&dataTypeInfo);
            if (IInterfaceInfo::Probe(dataTypeInfo) == NULL) break;
            InterfaceID id;
            IInterfaceInfo::Probe(dataTypeInfo)->GetId(&id);
            if ((*args)[j] != id) break;
        }
        return methodInfo;
    }
    return NULL;
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
