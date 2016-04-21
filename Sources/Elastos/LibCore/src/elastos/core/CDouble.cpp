
#include "CDouble.h"
#include "Math.h"
#include "RealToString.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::RealToString;

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL_4(CDouble, Object, IDouble, INumber, ISerializable, IComparable)

CAR_OBJECT_IMPL(CDouble)

ECode CDouble::constructor(
    /* [in] */ Double value)
{
    mValue = value;
    return NOERROR;
}

ECode CDouble::GetValue(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;
    return NOERROR;
}

ECode CDouble::IsInfinite(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Math::IsInfinite(mValue);

    return NOERROR;
}

ECode CDouble::IsNaN(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Math::IsNaN(mValue);

    return NOERROR;
}

ECode CDouble::ByteValue(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Byte)mValue;

    return NOERROR;
}

ECode CDouble::Int16Value(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Int16)mValue;

    return NOERROR;
}

ECode CDouble::Int32Value(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Int32)mValue;

    return NOERROR;
}

ECode CDouble::Int64Value(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Int64)mValue;

    return NOERROR;
}

ECode CDouble::FloatValue(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Float)mValue;

    return NOERROR;
}

ECode CDouble::DoubleValue(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CDouble::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    VALIDATE_NOT_NULL(_other);

    IDouble* other = (IDouble*)_other->Probe(EIID_IDouble);
    if (other == NULL) {
        return E_CLASS_CAST_EXCEPTION;
    }
    Double otherValue;
    other->GetValue(&otherValue);
    *result = Math::Compare(mValue, otherValue);
    return NOERROR;
}

ECode CDouble::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    if (IDouble::Probe(other) == NULL) return NOERROR;

    Double otherValue;
    IDouble::Probe(other)->GetValue(&otherValue);
    *result = Math::Equals(mValue, otherValue);
    return NOERROR;
}

ECode CDouble::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mValue;
    return NOERROR;
}

ECode CDouble::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RealToString::GetInstance()->ToString(mValue);

    return NOERROR;
}

} // namespace Core
} // namespace Elastos
