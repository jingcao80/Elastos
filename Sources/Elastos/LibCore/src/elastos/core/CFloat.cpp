
#include "CFloat.h"
#include "Math.h"
#include "RealToString.h"

using Elastos::IO::EIID_ISerializable;
using Elastos::Core::RealToString;

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL_4(CFloat, Object, IFloat, INumber, ISerializable, IComparable)

CAR_OBJECT_IMPL(CFloat)

ECode CFloat::constructor(
    /* [in] */ Float value)
{
    mValue = value;
    return NOERROR;
}

ECode CFloat::GetValue(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;
    return NOERROR;
}

ECode CFloat::IsInfinite(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Math::IsInfinite(mValue);

    return NOERROR;
}

ECode CFloat::IsNaN(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = Math::IsNaN(mValue);

    return NOERROR;
}

ECode CFloat::ByteValue(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Byte)mValue;

    return NOERROR;
}

ECode CFloat::Int16Value(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Int16)mValue;

    return NOERROR;
}

ECode CFloat::Int32Value(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Int32)mValue;

    return NOERROR;
}

ECode CFloat::Int64Value(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Int64)mValue;

    return NOERROR;
}

ECode CFloat::FloatValue(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CFloat::DoubleValue(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CFloat::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    VALIDATE_NOT_NULL(_other);

    IFloat* other = (IFloat*)_other->Probe(EIID_IFloat);
    if (other == NULL) {
        return NOERROR;
    }
    Float otherValue;
    other->GetValue(&otherValue);
    *result = Math::Compare(mValue, otherValue);
    return NOERROR;
}

ECode CFloat::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    if (IFloat::Probe(other) == NULL) return NOERROR;

    Float otherValue;
    IFloat::Probe(other)->GetValue(&otherValue);
    *result = Math::Equals(mValue, otherValue);
    return NOERROR;
}

ECode CFloat::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mValue;
    return NOERROR;
}

ECode CFloat::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = RealToString::GetInstance()->ToString(mValue);

    return NOERROR;
}

} // namespace Core
} // namespace Elastos
