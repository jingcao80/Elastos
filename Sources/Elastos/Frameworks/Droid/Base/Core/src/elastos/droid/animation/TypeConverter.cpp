
#include "elastos/droid/animation/TypeConverter.h"

namespace Elastos {
namespace Droid {
namespace Animation {

CAR_INTERFACE_IMPL(TypeConverter, Object, ITypeConverter);

TypeConverter::TypeConverter()
{}

ECode TypeConverter::constructor(
    /* [in] */ const InterfaceID& fromClass,
    /* [in] */ const InterfaceID& toClass)
{
    mFromClass = fromClass;
    mToClass = toClass;
    return NOERROR;
}

ECode TypeConverter::GetTargetType(
    /* [out] */ InterfaceID* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mToClass;
    return NOERROR;
}

ECode TypeConverter::GetSourceType(
    /* [out] */ InterfaceID* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mFromClass;
    return NOERROR;
}

} // namespace Animation
} // namepsace Droid
} // namespace Elastos

