
#include "CByte.h"
#include "IntegralToString.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::IntegralToString;

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL_4(CByte, Object, IByte, INumber, ISerializable, IComparable)

CAR_OBJECT_IMPL(CByte)

ECode CByte::constructor(
    /* [in] */ Byte value)
{
    mValue = value;
    return NOERROR;
}

ECode CByte::GetValue(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;
    return NOERROR;
}

ECode CByte::ByteValue(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CByte::Int16Value(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CByte::Int32Value(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CByte::Int64Value(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CByte::FloatValue(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CByte::DoubleValue(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CByte::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    VALIDATE_NOT_NULL(_other);

    IByte* other = (IByte*)_other->Probe(EIID_IByte);
    if (other == NULL) {
        return NOERROR;
    }
    Byte otherValue;
    other->GetValue(&otherValue);
    *result = (mValue > otherValue ? 1 : (mValue < otherValue ? -1 : 0));
    return NOERROR;
}

ECode CByte::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    if (IByte::Probe(other) == NULL) return NOERROR;

    Byte otherValue;
    IByte::Probe(other)->GetValue(&otherValue);
    *result = (mValue == otherValue);
    return NOERROR;
}

ECode CByte::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mValue;
    return NOERROR;
}

ECode CByte::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    //*result = IntegralToString::ToString(mValue);

    return NOERROR;
}

} // namespace Core
} // namespace Elastos
