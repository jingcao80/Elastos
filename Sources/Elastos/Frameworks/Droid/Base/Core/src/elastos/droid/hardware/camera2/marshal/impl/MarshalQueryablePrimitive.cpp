
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryablePrimitive.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalHelpers.h"
#include "elastos/droid/utility/CRational.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Utility::CRational;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryablePrimitive::MarshalerPrimitive::MarshalerPrimitive(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryablePrimitive* host)
{
    Marshaler::constructor(host, typeReference, nativeType);

    // Turn primitives into wrappers, otherwise int.class.cast(Integer) will fail
    assert(0);
    // typeReference->GetRawType();
    // mClass = WrapClassIfPrimitive((Class<T>)typeReference.getRawType());
}

ECode MarshalQueryablePrimitive::MarshalerPrimitive::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    return UnmarshalObject(buffer, outface);
}

ECode MarshalQueryablePrimitive::MarshalerPrimitive::CalculateMarshalSize(
    /* [in] */ IInterface* value,
    /* [out] */ Int32* outvalue)
{
    VALIDATE_NOT_NULL(outvalue);
    return MarshalHelpers::GetPrimitiveTypeSize(mNativeType, outvalue);
}

ECode MarshalQueryablePrimitive::MarshalerPrimitive::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    if (IInteger32::Probe(value) != NULL) {
        Int32 tmp;
        MarshalHelpers::CheckNativeTypeEquals(ICameraMetadataNative::TYPE_INT32, mNativeType, &tmp);
        AutoPtr<IInteger32> obj = IInteger32::Probe(value);
        Int32 val;
        obj->GetValue(&val);
        MarshalPrimitive(val, buffer);
    }
    else if (IFloat::Probe(value) != NULL) {
        Int32 tmp;
        MarshalHelpers::CheckNativeTypeEquals(ICameraMetadataNative::TYPE_FLOAT, mNativeType, &tmp);
        AutoPtr<IFloat> obj = IFloat::Probe(value);
        Float val;
        obj->GetValue(&val);
        MarshalPrimitive(val, buffer);
    }
    else if (IInteger64::Probe(value) != NULL) {
        Int32 tmp;
        MarshalHelpers::CheckNativeTypeEquals(ICameraMetadataNative::TYPE_INT64, mNativeType, &tmp);
        AutoPtr<IInteger64> obj = IInteger64::Probe(value);
        Int64 val;
        obj->GetValue(&val);
        MarshalPrimitive(val, buffer);
    }
    else if (IRational::Probe(value) != NULL) {
        Int32 tmp;
        MarshalHelpers::CheckNativeTypeEquals(ICameraMetadataNative::TYPE_RATIONAL, mNativeType, &tmp);
        AutoPtr<IRational> obj = IRational::Probe(value);
        MarshalPrimitive(obj, buffer);
    }
    else if (IDouble::Probe(value) != NULL) {
        Int32 tmp;
        MarshalHelpers::CheckNativeTypeEquals(ICameraMetadataNative::TYPE_DOUBLE, mNativeType, &tmp);
        AutoPtr<IDouble> obj = IDouble::Probe(value);
        Double val;
        obj->GetValue(&val);
        MarshalPrimitive(val, buffer);
    }
    else if (IByte::Probe(value) != NULL) {
        Int32 tmp;
        MarshalHelpers::CheckNativeTypeEquals(ICameraMetadataNative::TYPE_BYTE, mNativeType, &tmp);
        AutoPtr<IByte> obj = IByte::Probe(value);
        Byte val;
        obj->GetValue(&val);
        MarshalPrimitive(val, buffer);
    }
    else {
        // throw new UnsupportedOperationException(
        //         "Can't marshal managed type " + mTypeReference);
        String str;
        IObject::Probe(mTypeReference)->ToString(&str);
        Slogger::E("MarshalQueryablePrimitive", "Can't marshal managed type %s", str.string());
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

ECode MarshalQueryablePrimitive::MarshalerPrimitive::MarshalPrimitive(
    /* [in] */ Int32 value,
    /* [in] */ IByteBuffer* buffer)
{
    return buffer->PutInt32(value);
}

ECode MarshalQueryablePrimitive::MarshalerPrimitive::MarshalPrimitive(
    /* [in] */ Float value,
    /* [in] */ IByteBuffer* buffer)
{
    return buffer->PutFloat(value);
}

ECode MarshalQueryablePrimitive::MarshalerPrimitive::MarshalPrimitive(
    /* [in] */ Double value,
    /* [in] */ IByteBuffer* buffer)
{
    return buffer->PutDouble(value);
}

ECode MarshalQueryablePrimitive::MarshalerPrimitive::MarshalPrimitive(
    /* [in] */ Int64 value,
    /* [in] */ IByteBuffer* buffer)
{
    return buffer->PutInt64(value);
}

ECode MarshalQueryablePrimitive::MarshalerPrimitive::MarshalPrimitive(
    /* [in] */ IRational* value,
    /* [in] */ IByteBuffer* buffer)
{
    Int32 numerator;
    value->GetNumerator(&numerator);
    buffer->PutInt32(numerator);
    Int32 denominator;
    value->GetDenominator(&denominator);
    return buffer->PutInt32(denominator);
}

ECode MarshalQueryablePrimitive::MarshalerPrimitive::MarshalPrimitive(
    /* [in] */ Byte value,
    /* [in] */ IByteBuffer* buffer)
{
    return buffer->Put(value);
}

ECode MarshalQueryablePrimitive::MarshalerPrimitive::UnmarshalObject(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);
    *obj = NULL;

    switch (mNativeType) {
        case ICameraMetadataNative::TYPE_INT32:
        {
            Int32 value;
            buffer->GetInt32(&value);
            AutoPtr<IInteger32> _obj = CoreUtils::Convert(value);
            *obj = _obj;
            REFCOUNT_ADD(*obj);
            return NOERROR;

        }
        case ICameraMetadataNative::TYPE_FLOAT:
        {
            Float value;
            buffer->GetFloat(&value);
            AutoPtr<IFloat> _obj = CoreUtils::Convert(value);
            *obj = _obj;
            REFCOUNT_ADD(*obj);
            return NOERROR;
        }
        case ICameraMetadataNative::TYPE_INT64:
        {
            Int64 value;
            buffer->GetInt64(&value);
            AutoPtr<IInteger64> _obj = CoreUtils::Convert(value);
            *obj = _obj;
            REFCOUNT_ADD(*obj);
            return NOERROR;
        }
        case ICameraMetadataNative::TYPE_RATIONAL:
        {
            Int32 numerator;
            buffer->GetInt32(&numerator);
            Int32 denominator;
            buffer->GetInt32(&denominator);
            AutoPtr<IRational> _obj;
            CRational::New(numerator, denominator, (IRational**)&_obj);
            *obj = _obj;
            REFCOUNT_ADD(*obj);
            return NOERROR;
        }
        case ICameraMetadataNative::TYPE_DOUBLE:
        {
            Double value;
            buffer->GetDouble(&value);
            AutoPtr<IDouble> _obj = CoreUtils::Convert(value);
            *obj = _obj;
            REFCOUNT_ADD(*obj);
            return NOERROR;
        }
        case ICameraMetadataNative::TYPE_BYTE:
        {
            Byte value;
            buffer->Get(&value); // getByte
            AutoPtr<IByte> _obj = CoreUtils::ConvertByte(value);
            *obj = _obj;
            REFCOUNT_ADD(*obj);
            return NOERROR;
        }
        default:
            // throw new UnsupportedOperationException(
            //         "Can't unmarshal native type " + mNativeType);
            Slogger::E("MarshalQueryablePrimitive", "Can't unmarshal native type %d", mNativeType);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }
    return NOERROR;
}

ECode MarshalQueryablePrimitive::MarshalerPrimitive::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return MarshalHelpers::GetPrimitiveTypeSize(mNativeType, value);
}

CAR_INTERFACE_IMPL_2(MarshalQueryablePrimitive, Object,
        IMarshalQueryablePrimitive, IMarshalQueryable)

ECode MarshalQueryablePrimitive::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerPrimitive(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryablePrimitive::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    assert(0);
    // if (managedType.getType() instanceof Class<?>) {
    //     Class<?> klass = (Class<?>)managedType.getType();

    //     if (klass == byte.class || klass == Byte.class) {
    //         return nativeType == TYPE_BYTE;
    //     }
    //     else if (klass == int.class || klass == Integer.class) {
    //         return nativeType == TYPE_INT32;
    //     }
    //     else if (klass == float.class || klass == Float.class) {
    //         return nativeType == TYPE_FLOAT;
    //     }
    //     else if (klass == long.class || klass == Long.class) {
    //         return nativeType == TYPE_INT64;
    //     }
    //     else if (klass == double.class || klass == Double.class) {
    //         return nativeType == TYPE_DOUBLE;
    //     }
    //     else if (klass == Rational.class) {
    //         return nativeType == TYPE_RATIONAL;
    //     }
    // }
    // return false;
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
