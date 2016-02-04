
#include "elastos/droid/hardware/camera2/marshal/CMarshalHelpers.h"
#include "elastos/droid/hardware/camera2/marshal/MarshalHelpers.h"

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Marshal {

CAR_INTERFACE_IMPL(CMarshalHelpers, Singleton, IMarshalHelpers)

CAR_SINGLETON_IMPL(CMarshalHelpers)

ECode CMarshalHelpers::GetPrimitiveTypeSize(
    /* [in] */ Int32 nativeType,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return MarshalHelpers::GetPrimitiveTypeSize(nativeType, value);
}

ECode CMarshalHelpers::CheckPrimitiveClass(
    /* [in] */ const ClassID& klass,
    /* [out] */ ClassID* outcla)
{
    VALIDATE_NOT_NULL(outcla);

    return MarshalHelpers::CheckPrimitiveClass(klass, outcla);
}

ECode CMarshalHelpers::IsPrimitiveClass(
    /* [in] */ const ClassID& klass,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);

    return MarshalHelpers::IsPrimitiveClass(klass, value);
}

ECode CMarshalHelpers::WrapClassIfPrimitive(
    /* [in] */ const ClassID& klass,
    /* [out] */ ClassID* outcl)
{
    VALIDATE_NOT_NULL(outcl);

    return MarshalHelpers::WrapClassIfPrimitive(klass, outcl);
}

ECode CMarshalHelpers::ToStringNativeType(
    /* [in] */ Int32 nativeType,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    return MarshalHelpers::ToStringNativeType(nativeType, str);
}

ECode CMarshalHelpers::CheckNativeType(
    /* [in] */ Int32 nativeType,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return MarshalHelpers::CheckNativeType(nativeType, value);
}

ECode CMarshalHelpers::CheckNativeTypeEquals(
    /* [in] */ Int32 expectedNativeType,
    /* [in] */ Int32 actualNativeType,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    return MarshalHelpers::CheckNativeTypeEquals(expectedNativeType, actualNativeType, value);
}

} // namespace Marshal
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos
