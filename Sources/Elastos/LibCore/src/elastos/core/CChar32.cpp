
#include "CChar32.h"
#include "IntegralToString.h"

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Core {

CAR_INTERFACE_IMPL_4(CChar32, Object, IChar32, INumber, ISerializable, IComparable)

CAR_OBJECT_IMPL(CChar32)

ECode CChar32::constructor(
    /* [in] */ Char32 value)
{
    mValue = value;
    return NOERROR;
}

ECode CChar32::GetValue(
    /* [out] */ Char32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;
    return NOERROR;
}

ECode CChar32::ByteValue(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Byte)mValue;

    return NOERROR;
}

ECode CChar32::Int16Value(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result);
    *result = (Int16)mValue;

    return NOERROR;
}

ECode CChar32::Int32Value(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CChar32::Int64Value(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CChar32::FloatValue(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CChar32::DoubleValue(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mValue;

    return NOERROR;
}

ECode CChar32::CompareTo(
    /* [in] */ IInterface* _other,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -1;
    VALIDATE_NOT_NULL(_other);

    IChar32* other = (IChar32*)_other->Probe(EIID_IChar32);
    if (other == NULL) {
        return NOERROR;
    }

    Char32 otherValue;
    other->GetValue(&otherValue);
    *result = (mValue > otherValue ? 1 : (mValue < otherValue ? -1 : 0));
    return NOERROR;
}

ECode CChar32::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    VALIDATE_NOT_NULL(other);

    if (IChar32::Probe(other) == NULL) return NOERROR;

    Char32 otherValue;
    IChar32::Probe(other)->GetValue(&otherValue);
    *result = (mValue == otherValue);
    return NOERROR;
}

ECode CChar32::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);
    *hashCode = mValue;
    return NOERROR;
}

ECode CChar32::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr< ArrayOf<Char32> > strArray = ArrayOf<Char32>::Alloc(1);
    (*strArray)[0] = mValue;
    *result = String(*strArray);

    return NOERROR;
}

} // namespace Core
} // namespace Elastos
