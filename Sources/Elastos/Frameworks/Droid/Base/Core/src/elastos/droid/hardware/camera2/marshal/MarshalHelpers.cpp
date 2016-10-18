
#include "elastos/droid/hardware/camera2/marshal/MarshalHelpers.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Hardware::Camera2::Impl::ICameraMetadataNative;
using Elastos::Droid::Utility::ECLSID_CRational;
using Elastos::Core::ECLSID_CByte;
using Elastos::Core::ECLSID_CFloat;
using Elastos::Core::ECLSID_CDouble;
using Elastos::Core::ECLSID_CInteger32;
using Elastos::Core::ECLSID_CInteger64;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {

ECode MarshalHelpers::GetPrimitiveTypeSize(
    /* [in] */ Int32 nativeType,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    switch (nativeType) {
        case ICameraMetadataNative::TYPE_BYTE:
            *value = IMarshalHelpers::SIZEOF_BYTE;
            return NOERROR;
        case ICameraMetadataNative::TYPE_INT32:
            *value = IMarshalHelpers::SIZEOF_INT32;
            return NOERROR;
        case ICameraMetadataNative::TYPE_FLOAT:
            *value = IMarshalHelpers::SIZEOF_FLOAT;
            return NOERROR;
        case ICameraMetadataNative::TYPE_INT64:
            *value = IMarshalHelpers::SIZEOF_INT64;
            return NOERROR;
        case ICameraMetadataNative::TYPE_DOUBLE:
            *value = IMarshalHelpers::SIZEOF_DOUBLE;
            return NOERROR;
        case ICameraMetadataNative::TYPE_RATIONAL:
            *value = IMarshalHelpers::SIZEOF_RATIONAL;
            return NOERROR;
    }


    Logger::E("MarshalHelpers", "Unknown type, can't get size for %d", nativeType);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MarshalHelpers::CheckPrimitiveClass(
    /* [in] */ const ClassID& klass,
    /* [out] */ ClassID* outcla)
{
    VALIDATE_NOT_NULL(outcla);

    //checkNotNull(klass, "klass must not be null");

    Boolean result;
    IsPrimitiveClass(klass, &result);
    if (result) {
        *outcla = klass;
        return NOERROR;
    }

    Logger::E("MarshalHelpers", "Unsupported class '%d'; "
            "expected a metadata primitive class", klass);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MarshalHelpers::IsPrimitiveClass(
    /* [in] */ const ClassID& klass,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;

    if (klass == ECLSID_CByte) {
        *value = TRUE;
        return NOERROR;
    }
    else if (klass == ECLSID_CInteger32) {
        *value = TRUE;
        return NOERROR;
    }
    else if (klass == ECLSID_CFloat) {
        *value = TRUE;
        return NOERROR;
    }
    else if (klass == ECLSID_CInteger64) {
        *value = TRUE;
        return NOERROR;
    }
    else if (klass == ECLSID_CDouble) {
        *value = TRUE;
        return NOERROR;
    }
    else if (klass == ECLSID_CRational) {
        *value = TRUE;
        return NOERROR;
    }

    *value = FALSE;
    return NOERROR;
}

ECode MarshalHelpers::WrapClassIfPrimitive(
    /* [in] */ const ClassID& klass,
    /* [out] */ ClassID* outcl)
{
    VALIDATE_NOT_NULL(outcl);

    assert(0);
    return NOERROR;
}

ECode MarshalHelpers::ToStringNativeType(
    /* [in] */ Int32 nativeType,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    switch (nativeType) {
        case ICameraMetadataNative::TYPE_BYTE:
            *str = String("TYPE_BYTE");
            return NOERROR;
        case ICameraMetadataNative::TYPE_INT32:
            *str = String("TYPE_INT32");
            return NOERROR;
        case ICameraMetadataNative::TYPE_FLOAT:
            *str = String("TYPE_FLOAT");
            return NOERROR;
        case ICameraMetadataNative::TYPE_INT64:
            *str = String("TYPE_INT64");
            return NOERROR;
        case ICameraMetadataNative::TYPE_DOUBLE:
            *str = String("TYPE_DOUBLE");
            return NOERROR;
        case ICameraMetadataNative::TYPE_RATIONAL:
            *str = String("TYPE_RATIONAL");
            return NOERROR;
    }

    StringBuilder sb;
    sb += "UNKNOWN(";
    sb += nativeType;
    sb += ")";
    return sb.ToString(str);
}

ECode MarshalHelpers::CheckNativeType(
    /* [in] */ Int32 nativeType,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = 0;

    switch (nativeType) {
        case ICameraMetadataNative::TYPE_BYTE:
        case ICameraMetadataNative::TYPE_INT32:
        case ICameraMetadataNative::TYPE_FLOAT:
        case ICameraMetadataNative::TYPE_INT64:
        case ICameraMetadataNative::TYPE_DOUBLE:
        case ICameraMetadataNative::TYPE_RATIONAL:
            *value = nativeType;
            return NOERROR;
    }

    Logger::E("MarshalHelpers", "Unknown nativeType %d", nativeType);
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode MarshalHelpers::CheckNativeTypeEquals(
    /* [in] */ Int32 expectedNativeType,
    /* [in] */ Int32 actualNativeType,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    if (expectedNativeType != actualNativeType) {
        Logger::E("MarshalHelpers", "Expected native type %d, but got %d",
            expectedNativeType, actualNativeType);
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    *value = actualNativeType;
    return NOERROR;
}

} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
