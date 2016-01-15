
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
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Graphics::EIID_IPointF;
using Elastos::Core::EIID_IArrayOf;
using Elastos::Core::IArrayOf;
using Elastos::Core::CArrayOf;
using Elastos::Core::IDouble;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::EIID_IDouble;
using Elastos::Core::CDouble;
using Elastos::Core::EIID_IFloat;
using Elastos::Core::EIID_IInteger32;
using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::ECLSID_CDouble;
using Elastos::Core::AutoLock;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Animation {

using Elastos::Core::Math;

CAR_INTERFACE_IMPL(PropertyValuesHolder::PointFToFloatArray, TypeConverter, IPropertyValuesHolder);
PropertyValuesHolder::PointFToFloatArray::PointFToFloatArray()
{
    TypeConverter::constructor(EIID_IPointF, EIID_IArrayOf /*float[].class*/);
    mCoordinates = ArrayOf<Float>::Alloc(2);
}

ECode PropertyValuesHolder::PointFToFloatArray::Convert(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** values)
{
    VALIDATE_NOT_NULL(values);
    IPointF::Probe(value)->Get(&(*mCoordinates)[0], &(*mCoordinates)[1]);
    // (*mCoordinates)[0] = value.x;
    // mCoordinates[1] = value.y;
    AutoPtr<IFloat> x, y;
    CFloat::New((*mCoordinates)[0], (IFloat**)&x);
    CFloat::New((*mCoordinates)[1], (IFloat**)&y);

    AutoPtr<IArrayList> v;
    CArrayList::New((IArrayList**)&v);
    v->Set(0, x);
    v->Set(1, y);
    *values = v;
    REFCOUNT_ADD(*values);
    return NOERROR;
}

CAR_INTERFACE_IMPL(PropertyValuesHolder::PointFToInt32Array, TypeConverter, IPropertyValuesHolder);
PropertyValuesHolder::PointFToInt32Array::PointFToInt32Array()
{
    TypeConverter::constructor(EIID_IPointF, EIID_IArrayOf /*int[].class*/);
    mCoordinates = ArrayOf<Int32>::Alloc(2);
}

ECode PropertyValuesHolder::PointFToInt32Array::Convert(
    /* [in] */ IInterface* value,
    /* [out] */ IInterface** values)
{
    VALIDATE_NOT_NULL(values);
    Float _x, _y;
    IPointF::Probe(value)->Get(&_x, &_y);
    (*mCoordinates)[0] = Math::Round(_x);
    (*mCoordinates)[1] = Math::Round(_y);
    AutoPtr<IInteger32> x, y;
    CInteger32::New((*mCoordinates)[0], (IInteger32**)&x);
    CInteger32::New((*mCoordinates)[1], (IInteger32**)&y);

    AutoPtr<IArrayList> v;
    CArrayList::New((IArrayList**)&v);
    v->Set(0, x);
    v->Set(1, y);
    *values = v;
    REFCOUNT_ADD(*values);
    return NOERROR;
}

AutoPtr<ITypeEvaluator> PropertyValuesHolder::InitInt32Evaluator()
{
    AutoPtr<CInt32Evaluator> rst;
    CInt32Evaluator::NewByFriend((CInt32Evaluator**)&rst);
    return rst;
}

AutoPtr<ITypeEvaluator> PropertyValuesHolder::InitFloatEvaluator()
{
    AutoPtr<CFloatEvaluator> rst;
    CFloatEvaluator::NewByFriend((CFloatEvaluator**)&rst);
    return rst;
}

enum ClassType {
    intclass = 0,
    floatclass = 1,
    doubleclass = 2,
    Integerclass = 3,
    Floatclass = 4,
    Doubleclass = 5
};

AutoPtr<ITypeEvaluator> PropertyValuesHolder::sInt32Evaluator = InitInt32Evaluator();
AutoPtr<ITypeEvaluator> PropertyValuesHolder::sFloatEvaluator = InitFloatEvaluator();
Int32 PropertyValuesHolder::FLOAT_VARIANTS[6] = {
    floatclass/*float.class*/, Floatclass/*Float.class*/,
    doubleclass/*double.class*/, intclass/*int.class*/,
    Doubleclass/*Double.class*/, Integerclass/*Integer.class*/};

Int32 PropertyValuesHolder::INTEGER_VARIANTS[6] = {
    intclass/*int.class*/, Integerclass/*Integer.class*/,
    floatclass/*float.class*/, doubleclass/*double.class*/,
    Floatclass/*Float.class*/, Doubleclass/*Double.class*/};

Int32 PropertyValuesHolder::DOUBLE_VARIANTS[6] = {
    doubleclass/*double.class*/, Doubleclass/*Double.class*/,
    floatclass/*float.class*/, intclass/*int.class*/,
    Floatclass/*Float.class*/, Integerclass/*Integer.class*/};

AutoPtr< PropertyValuesHolder::ClassMethodMap > PropertyValuesHolder::sSetterPropertyMap = new ClassMethodMap();
AutoPtr< PropertyValuesHolder::ClassMethodMap > PropertyValuesHolder::sGetterPropertyMap = new ClassMethodMap();

String GetSignature(
    /* [in] */ Int32 type)
{
    switch(type) {
        case intclass: {
            return String("(I32)E");
        }
        case floatclass: {
            return String("(F)E");
        }
        case doubleclass: {
            return String("(D)E");
        }
        case Integerclass: {
            return String("(LElastos/Core/IInteger32;)E");
        }
        case Floatclass: {
            return String("(LElastos/Core/IFloat;)E");
        }
        case Doubleclass: {
            return String("(LElastos/Core/IDouble;)E");
        }
    }

    assert(0 && "TODO");
    return String(NULL);
}

CAR_INTERFACE_IMPL(PropertyValuesHolder, Object, IPropertyValuesHolder);
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
    AutoPtr<IInt32PropertyValuesHolder> holder = new Int32PropertyValuesHolder(propertyName, values);
    return IPropertyValuesHolder::Probe(holder);
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfInt32(
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Int32>* values)
{
    AutoPtr<IInt32PropertyValuesHolder> holder = new Int32PropertyValuesHolder(property, values);
    return IPropertyValuesHolder::Probe(holder);
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfFloat(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<Float>* values)
{
    AutoPtr<IFloatPropertyValuesHolder> holder = new FloatPropertyValuesHolder(propertyName, values);
    return IPropertyValuesHolder::Probe(holder);
}

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfFloat(
    /* [in] */ IProperty* property,
    /* [in] */ ArrayOf<Float>* values)
{
    AutoPtr<IFloatPropertyValuesHolder> holder = new FloatPropertyValuesHolder(property, values);
    return IPropertyValuesHolder::Probe(holder);
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
    /* [in] */ IProperty* property,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<IPropertyValuesHolder> holder = new PropertyValuesHolder(property);
    holder->SetObjectValues(values);
    holder->SetEvaluator(evaluator);
    return holder;
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

ECode PropertyValuesHolder::OfMultiInt32(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<ArrayOf<Int32>* >* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    VALIDATE_NOT_NULL(holder);
    if (values->GetLength() < 2) {
        // throw new IllegalArgumentException("At least 2 values must be supplied");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 numParameters = 0;
    AutoPtr<ArrayOf<IInterface*> > arrays = ArrayOf<IInterface*>::Alloc(values->GetLength());
    for (Int32 i = 0; i < values->GetLength(); i++) {
        if ((*values)[i] == NULL) {
            // throw new IllegalArgumentException("values must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        Int32 length = (*values)[i]->GetLength();
        if (i == 0) {
            numParameters = length;
        } else if (length != numParameters) {
            // throw new IllegalArgumentException("Values must all have the same length");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        AutoPtr<IArrayOf> items;
        CArrayOf::New(EIID_IInteger32, length, (IArrayOf**)&items);
        for (Int32 n = 0; n < length; n++) {
            AutoPtr<IInteger32> item;
            CInteger32::New((*((*values)[i]))[n], (IInteger32**)&item);
            items->Set(n, item);
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

ECode PropertyValuesHolder::OfMultiFloat(
    /* [in] */ const String& propertyName,
    /* [in] */ ArrayOf<ArrayOf<Float>* >* values,
    /* [out] */ IPropertyValuesHolder** holder)
{
    VALIDATE_NOT_NULL(holder);
    if (values->GetLength() < 2) {
        // throw new IllegalArgumentException("At least 2 values must be supplied");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 numParameters = 0;
    AutoPtr<ArrayOf<IInterface*> > arrays = ArrayOf<IInterface*>::Alloc(values->GetLength());
    for (Int32 i = 0; i < values->GetLength(); i++) {
        if ((*values)[i] == NULL) {
            // throw new IllegalArgumentException("values must not be null");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        Int32 length = (*values)[i]->GetLength();
        if (i == 0) {
            numParameters = length;
        } else if (length != numParameters) {
            // throw new IllegalArgumentException("Values must all have the same length");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        AutoPtr<IArrayOf> items;
        CArrayOf::New(EIID_IFloat, length, (IArrayOf**)&items);
        for (Int32 n = 0; n < length; n++) {
            AutoPtr<IFloat> item;
            CFloat::New((*((*values)[i]))[n], (IFloat**)&item);
            items->Set(n, item);
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
    /* [in] */ ITypeConverter* converter,
    /* [in] */ ITypeEvaluator* evaluator,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    AutoPtr<IPropertyValuesHolder> pvh = new PropertyValuesHolder(property);
    ((PropertyValuesHolder*)pvh.Get())->SetConverter(converter);
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

AutoPtr<IPropertyValuesHolder> PropertyValuesHolder::OfKeyframes(
    /* [in] */ const String& propertyName,
    /* [in] */ IKeyframes* keyframes)
{
    if (IInt32Keyframes::Probe(keyframes)) {
        return new Int32PropertyValuesHolder(propertyName, IInt32Keyframes::Probe(keyframes));
    } else if (IFloatKeyframes::Probe(keyframes)) {
        return new FloatPropertyValuesHolder(propertyName, IFloatKeyframes::Probe(keyframes));
    } else {
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
    if (IInt32Keyframes::Probe(keyframes)) {
        return new Int32PropertyValuesHolder(property, IInt32Keyframes::Probe(keyframes));
    } else if (IFloatKeyframes::Probe(keyframes)) {
        return new FloatPropertyValuesHolder(property, IFloatKeyframes::Probe(keyframes));
    } else {
        AutoPtr<IPropertyValuesHolder> pvh = new PropertyValuesHolder(property);
        ((PropertyValuesHolder*)pvh.Get())->mKeyframes = keyframes;
        keyframes->GetType(&(((PropertyValuesHolder*)pvh.Get())->mValueType));
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
    mKeyframes = new KeyframeSet(keyframes);
    return NOERROR;
}

ECode PropertyValuesHolder::SetObjectValues(
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (IObject::Probe((*values)[0])) {
        IObject::Probe((*values)[0])->GetInterfaceID((*values)[0], &mValueType);
    }
    else {
        assert(0 && "TODO");
        // mValueType = ECLSID_CDummyObject;
    }
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

ECode PropertyValuesHolder::SetupSetter(
    /* [in] */ IInterface* targetClass)
{
    InterfaceID propertyType = mValueType;
    if (mConverter != NULL) {
        mConverter->GetTargetType(&propertyType);
    }

    AutoPtr<IClassInfo> info = TransformClassInfo(targetClass);
    assert(info != NULL);
    mSetter = SetupSetterOrGetter(info, sSetterPropertyMap.Get(), String("Set"), &propertyType);
    return NOERROR;
}

ECode PropertyValuesHolder::SetupGetter(
     /* [in] */ IInterface* target)
{
    AutoPtr<IClassInfo> info = TransformClassInfo(target);
    if (info) {
        mGetter = SetupSetterOrGetter(info, sGetterPropertyMap.Get(), String("Get"), NULL);
        return NOERROR;
    }

    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode PropertyValuesHolder::SetupSetterAndGetter(
    /* [in] */ IInterface* target)
{
    mKeyframes->InvalidateCache();
    if (mProperty) {
        AutoPtr<IInterface> testValue;
        AutoPtr<ArrayOf<IKeyframe*> > keyframes;
        mKeyframes->GetKeyframes((ArrayOf<IKeyframe*>**)&keyframes);
        Int32 keyframeCount = keyframes == NULL ? 0 : keyframes->GetLength();
        for (Int32 i = 0; i < keyframeCount; i++) {
            AutoPtr<IKeyframe> kf = (*keyframes)[i];
            Boolean has = FALSE, wasSet = FALSE;
            kf->HasValue(&has);
            wasSet = ((Keyframe*)kf.Get())->ValueWasSetOnStart();
            if (!has || wasSet) {
                if (testValue == NULL) {
                    AutoPtr<IInterface> v;
                    ASSERT_SUCCEEDED(mProperty->Get(target, (IInterface**)&v));
                    ConvertBack(v, (IInterface**)&testValue);
                }
                ASSERT_SUCCEEDED(kf->SetValue(testValue));
                ((Keyframe*)kf.Get())->SetValueWasSetOnStart(FALSE);
            }
        }
        return NOERROR;
    }

    if (mSetter == NULL) {
        SetupSetter(target);
    }
    AutoPtr<ArrayOf<IKeyframe*> > frames;
    mKeyframes->GetKeyframes((ArrayOf<IKeyframe*>**)&frames);
    Int32 keyframeCount = frames == NULL ? 0 : frames->GetLength();

    for(Int32 i = 0; i < keyframeCount; i++) {
        IKeyframe* kf = (*frames)[i];
        Boolean has = FALSE, wasSet = FALSE;
        kf->HasValue(&has);
        wasSet = ((Keyframe*)kf)->ValueWasSetOnStart();
        if (!has || wasSet) {
            if (mGetter == NULL) {
                SetupGetter(target);
                if (mGetter == NULL) {
                    // Already logged the error - just return to avoid NPE
                    return NOERROR;
                }
            }
            AutoPtr<IArgumentList> arg;
            mGetter->CreateArgumentList((IArgumentList**)&arg);
            AutoPtr<IFunctionInfo> funcInfo;
            arg->GetFunctionInfo((IFunctionInfo**)&funcInfo);
            AutoPtr<IParamInfo> paramInfo;
            funcInfo->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
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
                    AutoPtr<IInteger32> getRst;
                    arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));

                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(getRst, (IInterface**)&value));
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
                    AutoPtr<IFloat> getRst;
                    arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));

                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(getRst, (IInterface**)&value));
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
                    AutoPtr<IDouble> getRst;
                    arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
                    FAIL_RETURN(mGetter->Invoke(target, arg));

                    AutoPtr<IInterface> value;
                    FAIL_RETURN(ConvertBack(getRst, (IInterface**)&value));
                    kf->SetValue(value);
                    ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
                }
                else{
                    assert(0);
                }
            }
            else {
                AutoPtr<IInterface> getRst;
                arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
                FAIL_RETURN(mGetter->Invoke(target, arg));

                AutoPtr<IInterface> value;
                FAIL_RETURN(ConvertBack(getRst, (IInterface**)&value));
                kf->SetValue(value);
                ((Keyframe*)kf)->SetValueWasSetOnStart(TRUE);
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
        if (IBidirectionalTypeConverter::Probe(mConverter) == NULL) {
            // throw new IllegalArgumentException("Converter "
            //         + mConverter.getClass().getName()
            //         + " must be a BidirectionalTypeConverter");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        IBidirectionalTypeConverter::Probe(mConverter)->ConvertBack(value, outValue);
    }

    return NOERROR;
}

ECode PropertyValuesHolder::SetupValue(
     /* [in] */ IInterface* target,
     /* [in] */ IKeyframe* kf)
{
    if (mProperty) {
        AutoPtr<IInterface> value;
        mProperty->Get(target, (IInterface**)&value);

        AutoPtr<IInterface> cValue;
        FAIL_RETURN(ConvertBack(value, (IInterface**)&cValue));
        kf->SetValue(cValue);
    }
    if (mGetter == NULL) {
        SetupGetter(target);
        if (mGetter == NULL)
        {
            // Already logged the error - just return to avoid NPE
            return NOERROR;
        }
    }

    AutoPtr<IArgumentList> arg;
    mGetter->CreateArgumentList((IArgumentList**)&arg);
    AutoPtr<IFunctionInfo> funcInfo;
    arg->GetFunctionInfo((IFunctionInfo**)&funcInfo);
    AutoPtr<IParamInfo> paramInfo;
    funcInfo->GetParamInfoByIndex(0, (IParamInfo**)&paramInfo);
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
            AutoPtr<IInteger32> getRst;
            arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));

            AutoPtr<IInterface> cValue;
            FAIL_RETURN(ConvertBack(getRst, (IInterface**)&cValue));
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
            AutoPtr<IFloat> getRst;
            arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));

            AutoPtr<IInterface> cValue;
            FAIL_RETURN(ConvertBack(getRst, (IInterface**)&cValue));
            kf->SetValue(cValue);
        }
        else{
            assert(0);
        }
    } else if(mValueType == ECLSID_CDouble) {
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
            AutoPtr<IDouble> getRst;
            arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
            FAIL_RETURN(mGetter->Invoke(target, arg));

            AutoPtr<IInterface> cValue;
            FAIL_RETURN(ConvertBack(getRst, (IInterface**)&cValue));
            kf->SetValue(cValue);
        }
        else{
            assert(0);
        }
    } else {
       AutoPtr<IInterface> getRst;
       arg->SetOutputArgumentOfObjectPtrPtr(0, (IInterface**)&getRst);
       FAIL_RETURN(mGetter->Invoke(target, arg));

        AutoPtr<IInterface> cValue;
        FAIL_RETURN(ConvertBack(getRst, (IInterface**)&cValue));
       kf->SetValue(cValue);
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

ECode PropertyValuesHolder::CloneSuperData(
    /* [in] */ PropertyValuesHolder* holder)
{
    holder->mPropertyName = mPropertyName;
    holder->mProperty = mProperty;
    holder->mSetter = mSetter;
    holder->mValueType = mValueType;
    ICloneable::Probe(mKeyframes)->Clone((IInterface**)(&(holder->mKeyframes)));
    holder->mTmpValueArray = mTmpValueArray;
    holder->mGetter = mGetter;
    holder->mEvaluator = mEvaluator;
    holder->mAnimatedValue = mAnimatedValue;
    return NOERROR;
}

ECode PropertyValuesHolder::SetAnimatedValue(
    /* [in] */ IInterface* target)
{
    AutoPtr<IInterface> animatedValue;
    GetAnimatedValue((IInterface**)&animatedValue);
    if(mProperty)
    {
        if(animatedValue)
            mProperty->Set(target, animatedValue);
    }

    if(mSetter)
    {
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

        if (mValueType == ECLSID_CInteger32)
        {
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
            if (carType == CarDataType_Float)
            {
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
                (mValueType == ECLSID_CFloat) ? sFloatEvaluator :
                NULL;
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
    } else {
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
    CloneSuperData(v);
    *holder = (IPropertyValuesHolder*)v->Probe(EIID_IPropertyValuesHolder);
    REFCOUNT_ADD(*holder)
    return NOERROR;
}

ECode PropertyValuesHolder::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    String tmp;
    IObject::Probe(mKeyframes)->ToString(&tmp);
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

AutoPtr<IMethodInfo> PropertyValuesHolder::GetPropertyFunction(
    /* [in] */ IClassInfo* targetClass,
    /* [in] */ const String& prefix,
    /* [in] */ InterfaceID* valueType)
{
    // TODO: faster implementation...
    AutoPtr<IMethodInfo> returnVal;
    String methodName = GetMethodName(prefix, mPropertyName);
    // Class args[] = null;
    if (valueType == NULL) {
        // try {
        // returnVal = targetClass.getMethod(methodName, args);
        targetClass->GetMethodInfo(methodName, String("()E"), (IMethodInfo**)&returnVal);
        // } catch (NoSuchMethodException e) {
        //     // Swallow the error, log it later
        // }
    } else {
        // args = new Class[1];
        Int32* typeVariants = NULL;
        Int32 length = 1;
        String signature;
        if (*valueType == EIID_IFloat) {
            typeVariants = FLOAT_VARIANTS;
            length = sizeof(FLOAT_VARIANTS)/sizeof(FLOAT_VARIANTS[0]);
        } else if (*valueType == EIID_IInteger32) {
            typeVariants = INTEGER_VARIANTS;
            length = sizeof(INTEGER_VARIANTS)/sizeof(INTEGER_VARIANTS[0]);
        } else if (*valueType == EIID_IDouble) {
            typeVariants = DOUBLE_VARIANTS;
            length = sizeof(DOUBLE_VARIANTS)/sizeof(DOUBLE_VARIANTS[0]);
        } else {
            // typeVariants = new Class[1];
            if (*valueType == EIID_IPointF) {
                length = 1;
                signature = "(LElastos/Droid/Graphics/IPointF;)E";
            } else {
                length = 0;
            }
        }

        for (Int32 i = 0; i < length; i++) {
            if (signature.IsNull()) {
                signature = GetSignature(typeVariants[i]);
            }

            if (FAILED(targetClass->GetMethodInfo(methodName, signature, (IMethodInfo**)&returnVal)) || returnVal == NULL) {
                continue;
            }

            if (mConverter == NULL) {
                // change the value type to suit
                mValueType = *valueType;
            }
            return returnVal;
        }

        // If we got here, then no appropriate function was found
    }

    if (returnVal == NULL) {
        Slogger::W(String("PropertyValuesHolder"), String("Method ") +
                methodName + String("() with type ") /*+ valueType */+
                String(" not found on target class ") /*+ targetClass*/);
    }

    return returnVal;
}

AutoPtr<IMethodInfo> PropertyValuesHolder::SetupSetterOrGetter(
    /* [in] */ IClassInfo* targetClass,
    /* [in] */ PropertyValuesHolder::ClassMethodMap * propertyMapMap,
    /* [in] */ const String& prefix,
    /* [in] */ InterfaceID* valueType)
{
    AutoPtr<IMethodInfo> setterOrGetter;
    {
        AutoLock lock(mPropertyMapLock);
        AutoPtr<IClassInfo> key = targetClass;
        AutoPtr< MethodMap > propertyMap;
        typename ClassMethodMap::Iterator it = propertyMapMap->Find(key);
        if ((it != propertyMapMap->End()) && (it->mSecond != NULL)) {
            propertyMap = it->mSecond;
            typename MethodMap::Iterator it2 = propertyMap->Find(mPropertyName);
            if ((it2 != propertyMap->End()) && (it2->mSecond != NULL)) {
                setterOrGetter = it2->mSecond;
            }
        }

        if (setterOrGetter == NULL) {
            setterOrGetter = GetPropertyFunction(targetClass, prefix, valueType);
            if (propertyMap == NULL) {
                propertyMap = new MethodMap();
                (*propertyMapMap)[key] = propertyMap;
            }

            (*propertyMap)[mPropertyName] = setterOrGetter;
        }
        return setterOrGetter;
    }
    return NULL;
}

AutoPtr<IClassInfo> PropertyValuesHolder::TransformClassInfo(
    /* [in] */ IInterface* o)
{
    AutoPtr<IObject> obj = IObject::Probe(o);
    assert(obj != NULL);
    ClassID objId;
    obj->GetClassID(&objId);
    AutoPtr<IModuleInfo> moduleInfo;
    String path(objId.mUunm);
    ASSERT_SUCCEEDED(_CReflector_AcquireModuleInfo(
           path, (IModuleInfo**)&moduleInfo));
    Int32 clsCount;
    moduleInfo->GetClassCount(&clsCount);
    AutoPtr< ArrayOf<IClassInfo*> > buf = ArrayOf<IClassInfo*>::Alloc(clsCount);
    moduleInfo->GetAllClassInfos(buf);
    AutoPtr<IClassInfo> info;
    ClassID id;
    id.mUunm = (char*)malloc(80);
    for (Int32 i = 0; i < clsCount; i++) {
        (*buf)[i]->GetId(&id);
        if (id == objId) {
           info = (*buf)[i];
           break;
        }
    }

    free(id.mUunm);
    return info;
}

AutoPtr<IMethodInfo> PropertyValuesHolder::nGetInt32Method(
    /* [in] */ IInterface* targetClass,
    /* [in] */ const String& methodName)
{
    AutoPtr<IClassInfo> clInfo = TransformClassInfo(targetClass);
    AutoPtr<IMethodInfo> mi;
    clInfo->GetMethodInfo(methodName, String("(I32)E"), (IMethodInfo**)&mi);
    return mi;
}

AutoPtr<IMethodInfo> PropertyValuesHolder::nGetFloatMethod(
    /* [in] */ IInterface* targetClass,
    /* [in] */ const String& methodName)
{
    AutoPtr<IClassInfo> clInfo = TransformClassInfo(targetClass);
    AutoPtr<IMethodInfo> mi;
    clInfo->GetMethodInfo(methodName, String("(F)E"), (IMethodInfo**)&mi);
    return mi;
}

AutoPtr<IMethodInfo> PropertyValuesHolder::nGetMultipleInt32Method(
    /* [in] */ IInterface* targetClass,
    /* [in] */ const String& methodName,
    /* [in] */ Int32 numParams)
{
    AutoPtr<IClassInfo> clInfo = TransformClassInfo(targetClass);
    AutoPtr<IMethodInfo> mi;
    clInfo->GetMethodInfo(methodName, String("(I32I32I32I32)E"), (IMethodInfo**)&mi);
    return mi;
}

AutoPtr<IMethodInfo> PropertyValuesHolder::nGetMultipleFloatMethod(
    /* [in] */ IInterface* targetClass,
    /* [in] */ const String& methodName,
    /* [in] */ Int32 numParams)
{
    AutoPtr<IClassInfo> clInfo = TransformClassInfo(targetClass);
    AutoPtr<IMethodInfo> mi;
    clInfo->GetMethodInfo(methodName, String("(FFFF)E"), (IMethodInfo**)&mi);
    return mi;
}

void PropertyValuesHolder::nCallInt32Method(
    /* [in] */ IInterface* target,
    /* [in] */ IMethodInfo* methodID,
    /* [in] */ Int32 arg)
{
    assert(methodID != NULL);
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
    assert(methodID != NULL);
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
    assert(methodID != NULL);
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
    assert(methodID != NULL);
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
    assert(methodID != NULL);
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
    assert(methodID != NULL);
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
    assert(methodID != NULL);
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
    assert(methodID != NULL);
    AutoPtr<IArgumentList> args;
    methodID->CreateArgumentList((IArgumentList**)&args);
    args->SetInputArgumentOfCarArray(0, arg);
    methodID->Invoke(target, args);
}

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos
