
#include "elastos/droid/hardware/camera2/marshal/impl/MarshalQueryableEnum.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalHelpers.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Core::IInteger32;
using Elastos::Core::Math;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CHashMap;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {
namespace Impl {

MarshalQueryableEnum::MarshalerEnum::MarshalerEnum(
    /* [in] */ ITypeReference* typeReference,
    /* [in] */ Int32 nativeType,
    /* [in] */ MarshalQueryableEnum* host)
{
    Marshaler::constructor(host, typeReference, nativeType);

    assert(0);
    // mClass = (Class<T>)typeReference.getRawType();
}

ECode MarshalQueryableEnum::MarshalerEnum::Marshal(
    /* [in] */ IInterface* value,
    /* [in] */ IByteBuffer* buffer)
{
    Int32 enumValue = GetEnumValue(value);

    if (mNativeType == ICameraMetadataNative::TYPE_INT32) {
        buffer->PutInt32(enumValue);
    }
    else if (mNativeType == ICameraMetadataNative::TYPE_BYTE) {
        if (enumValue < MARSHALQUERYABLEENUM_UINT8_MIN || enumValue > MARSHALQUERYABLEENUM_UINT8_MAX) {
            // throw new UnsupportedOperationException(String.format(
            //         "Enum value %x too large to fit into unsigned byte", enumValue));
            Slogger::E(TAG, "Enum value %x too large to fit into unsigned byte", enumValue);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        }
        buffer->Put((Byte)enumValue);
    }
    else {
        //throw new AssertionError();
        Slogger::E(TAG, "AssertionError");
        return E_ASSERTION_ERROR;
    }
    return NOERROR;
}

ECode MarshalQueryableEnum::MarshalerEnum::Unmarshal(
    /* [in] */ IByteBuffer* buffer,
    /* [out] */ IInterface** outface)
{
    VALIDATE_NOT_NULL(outface);
    *outface = NULL;

    Int32 enumValue;

    switch (mNativeType) {
        case ICameraMetadataNative::TYPE_INT32:
            buffer->GetInt32(&enumValue);
            break;
        case ICameraMetadataNative::TYPE_BYTE:
        {
            // get the unsigned byte value; avoid sign extension
            Byte value;
            buffer->Get(&value);
            enumValue = value & MARSHALQUERYABLEENUM_UINT8_MASK;
            break;
        }
        default:
            // throw new AssertionError(
            //         "Unexpected native type; impossible since its not supported");
            Slogger::E(TAG, "Unexpected native type; impossible since its not supported");
            return E_ASSERTION_ERROR;
    }

    return GetEnumFromValue(mClass, enumValue, outface);
}

ECode MarshalQueryableEnum::MarshalerEnum::GetNativeSize(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return MarshalHelpers::GetPrimitiveTypeSize(mNativeType, value);
}

static AutoPtr<IHashMap> initEnumValues()
{
    AutoPtr<IHashMap> map;
    CHashMap::New((IHashMap**)&map);
    return map;
}

const String MarshalQueryableEnum::TAG("MarshalQueryableEnum");// = MarshalQueryableEnum.class.getSimpleName();
const Boolean MarshalQueryableEnum::VERBOSE = FALSE;// Log.isLoggable(TAG, Log.VERBOSE);

const Int32 MarshalQueryableEnum::MARSHALQUERYABLEENUM_UINT8_MIN = 0x0;
const Int32 MarshalQueryableEnum::MARSHALQUERYABLEENUM_UINT8_MAX = (1 << Elastos::Core::Math::BYTE_SIZE) - 1;
const Int32 MarshalQueryableEnum::MARSHALQUERYABLEENUM_UINT8_MASK = UINT8_MAX;

static AutoPtr<IHashMap> sEnumValues = initEnumValues();

CAR_INTERFACE_IMPL_2(MarshalQueryableEnum, Object,
        IMarshalQueryableEnum, IMarshalQueryable)

ECode MarshalQueryableEnum::CreateMarshaler(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ IMarshaler** outmar)
{
    VALIDATE_NOT_NULL(outmar);

    AutoPtr<IMarshaler> _outmar = new MarshalerEnum(managedType, nativeType, this);
    *outmar = _outmar;
    REFCOUNT_ADD(*outmar);
    return NOERROR;
}

ECode MarshalQueryableEnum::IsTypeMappingSupported(
    /* [in] */ ITypeReference* managedType,
    /* [in] */ Int32 nativeType,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    assert(0);
    if (nativeType == ICameraMetadataNative::TYPE_INT32 || nativeType == ICameraMetadataNative::TYPE_BYTE) {
    //     if (managedType.getType() instanceof Class<?>) {
    //         Class<?> typeClass = (Class<?>)managedType.getType();

    //         if (typeClass.isEnum()) {
    //             if (VERBOSE) {
    //                 Log.v(TAG, "possible enum detected for " + typeClass);
    //             }

    //             // The enum must not take extra arguments
    //             try {
    //                 // match a class like: "public enum Fruits { Apple, Orange; }"
    //                 typeClass.getDeclaredConstructor(String.class, int.class);
    //                 return true;
    //             } catch (NoSuchMethodException e) {
    //                 // Skip: custom enum with a special constructor e.g. Foo(T), but need Foo()
    //                 Log.e(TAG, "Can't marshal class " + typeClass + "; no default constructor");
    //             } catch (SecurityException e) {
    //                 // Skip: wouldn't be able to touch the enum anyway
    //                 Log.e(TAG, "Can't marshal class " + typeClass + "; not accessible");
    //             }
    //         }
    //     }
    }

    // return false;
    return NOERROR;
}

ECode MarshalQueryableEnum::RegisterEnumValues(
    /* [in] */ ClassID enumType,
    /* [in] */ ArrayOf<Int32>* values)
{
    assert(0);
    // if (enumType.getEnumConstants().length != values.length) {
    //     throw new IllegalArgumentException(
    //             "Expected values array to be the same size as the enumTypes values "
    //                     + values.length + " for type " + enumType);
    // }
    if (VERBOSE) {
        Slogger::V(TAG, "Registered enum values for type %d values", enumType);
    }

    assert(0);
    //return sEnumValues->Put(enumType, values);
    return NOERROR;
}

Int32 MarshalQueryableEnum::GetEnumValue(
    /* [in] */ IInterface* enumValue)
{
    AutoPtr<ArrayOf<Int32> > values;
    assert(0);
    //values = sEnumValues.get(enumValue.getClass());

    assert(0);
    Int32 ordinal;// = enumValue.ordinal();
    if (values != NULL) {
        return (*values)[ordinal];
    }

    return ordinal;
}

ECode MarshalQueryableEnum::GetEnumFromValue(
    /* [in] */ ClassID enumType,
    /* [in] */ Int32 value,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Int32 ordinal;
    AutoPtr<ArrayOf<Int32> > registeredValues;// = sEnumValues.get(enumType);
    if (registeredValues != NULL) {
        ordinal = -1;

        for (Int32 i = 0; i < registeredValues->GetLength(); ++i) {
            if ((*registeredValues)[i] == value) {
                ordinal = i;
                break;
            }
        }
    }
    else {
        ordinal = value;
    }

    assert(0);
    // T[] values = enumType.getEnumConstants();

    // if (ordinal < 0 || ordinal >= values.length) {
    //     throw new IllegalArgumentException(
    //             String.format(
    //                     "Argument 'value' (%d) was not a valid enum value for type %s "
    //                             + "(registered? %b)",
    //                     value,
    //                     enumType, (registeredValues != null)));
    // }

    // return values[ordinal];
    return NOERROR;
}

} // namespace Impl
} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
