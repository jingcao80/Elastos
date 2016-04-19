
#include "CBoolean.h"
#include "StringUtils.h"

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL_3(CBoolean, Object, IBoolean, ISerializable, IComparable)

CAR_OBJECT_IMPL(CBoolean)

ECode CBoolean::constructor(
    /* [in] */ Boolean value)
{
    mValue = value;
    return NOERROR;
}

ECode CBoolean::GetValue(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode CBoolean::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    VALIDATE_NOT_NULL(_other);

    IBoolean* other = (IBoolean*)_other->Probe(EIID_IBoolean);
    if (other == NULL) {
        return E_CLASS_CAST_EXCEPTION;
    }
    Boolean otherValue = FALSE;
    other->GetValue(&otherValue);
    *result = (mValue == otherValue ? 0 : mValue ? 1 : -1);
    return NOERROR;
}

ECode CBoolean::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    if (IBoolean::Probe(other) == NULL) return NOERROR;

    Boolean otherValue = FALSE;
    IBoolean::Probe(other)->GetValue(&otherValue);
    *result = (mValue == otherValue);
    return NOERROR;
}

ECode CBoolean::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mValue ? 1231 : 1237;
    return NOERROR;
}

ECode CBoolean::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = StringUtils::BooleanToString(mValue);
    return NOERROR;
}

} // namespace Core
} // namespace Elastos
