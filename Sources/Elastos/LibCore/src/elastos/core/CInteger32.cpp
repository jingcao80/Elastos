
#include "CInteger32.h"
#include "IntegralToString.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::IntegralToString;

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL_4(CInteger32, Object,IInteger32, INumber, ISerializable, IComparable)

CAR_OBJECT_IMPL(CInteger32)

ECode CInteger32::constructor(
    /* [in] */ Int32 value)
{
    mValue = value;
    return NOERROR;
}

ECode CInteger32::GetValue(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;
    return NOERROR;
}

ECode CInteger32::ByteValue(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Byte)mValue;

    return NOERROR;
}

ECode CInteger32::Int16Value(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Int16)mValue;

    return NOERROR;
}

ECode CInteger32::Int32Value(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CInteger32::Int64Value(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CInteger32::FloatValue(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CInteger32::DoubleValue(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CInteger32::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    VALIDATE_NOT_NULL(_other);

    IInteger32* other = IInteger32::Probe(_other);
    if (other == NULL) {
        return NOERROR;
    }
    Int32 otherValue;
    other->GetValue(&otherValue);
    *result = (mValue > otherValue ? 1 : (mValue < otherValue ? -1 : 0));
    return NOERROR;
}

ECode CInteger32::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    if (IInteger32::Probe(other) == NULL) return NOERROR;

    Int32 otherValue;
    IInteger32::Probe(other)->GetValue(&otherValue);
    *result = (mValue == otherValue);
    return NOERROR;
}

ECode CInteger32::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mValue;
    return NOERROR;
}

ECode CInteger32::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = IntegralToString::ToString(mValue);

    return NOERROR;
}

} // namespace Core
} // namespace Elastos
