#include "elastos/droid/utility/CRational.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_INumber;
using Elastos::Core::EIID_IComparable;

namespace Elastos {
namespace Droid {
namespace Utility {

static AutoPtr<IRational> CreateRational(
    /* [in] */ Int32 numerator,
    /* [in] */ Int32 denominator)
{
    AutoPtr<CRational> r;
    CRational::NewByFriend(numerator, denominator, (CRational**)&r);
    return (IRational*)r.Get();
}

const AutoPtr<IRational> CRational::NaN = CreateRational(0, 0);// = new Rational(0, 0);
const AutoPtr<IRational> CRational::POSITIVE_INFINITY = CreateRational(1, 0);// = new Rational(1, 0);
const AutoPtr<IRational> CRational::NEGATIVE_INFINITY = CreateRational(-1, 0);// = new Rational(-1, 0);
const AutoPtr<IRational> CRational::ZERO = CreateRational(0, 1);// = new Rational(0, 1);

CAR_INTERFACE_IMPL_3(CRational, Object, IRational, INumber, IComparable)

CAR_OBJECT_IMPL(CRational)

CRational::CRational()
    : mNumerator(0)
    , mDenominator(0)
{}

ECode CRational::constructor(
    /* [in] */ Int32 numerator,
    /* [in] */ Int32 denominator)
{
    if (denominator < 0) {
        numerator = -numerator;
        denominator = -denominator;
    }

    // Convert to reduced form
    if (denominator == 0 && numerator > 0) {
        mNumerator = 1; // +Inf
        mDenominator = 0;
    }
    else if (denominator == 0 && numerator < 0) {
        mNumerator = -1; // -Inf
        mDenominator = 0;
    }
    else if (denominator == 0 && numerator == 0) {
        mNumerator = 0; // NaN
        mDenominator = 0;
    }
    else if (numerator == 0) {
        mNumerator = 0;
        mDenominator = 1;
    }
    else {
        Int32 gcd = Gcd(numerator, denominator);

        mNumerator = numerator / gcd;
        mDenominator = denominator / gcd;
    }
    return NOERROR;
}

ECode CRational::GetNumerator(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mNumerator;
    return NOERROR;
}

ECode CRational::GetDenominator(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mDenominator;
    return NOERROR;
}

ECode CRational::IsNaN(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mDenominator == 0 && mNumerator == 0;
    return NOERROR;
}

ECode CRational::IsInfinite(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mNumerator != 0 && mDenominator == 0;
    return NOERROR;
}

ECode CRational::IsFinite(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mDenominator != 0;
    return NOERROR;
}

ECode CRational::IsZero(
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    Boolean result;
    IsFinite(&result);
    *value = result && mNumerator == 0;
    return NOERROR;
}

Boolean CRational::IsPosInf()
{
    return mDenominator == 0 && mNumerator > 0;
}

Boolean CRational::IsNegInf()
{
    return mDenominator == 0 && mNumerator < 0;
}

ECode CRational::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    if (IRational::Probe(obj) == NULL) {
        return NOERROR;
    }

    CRational* cr = (CRational*)IRational::Probe(obj);
    *value = (mNumerator == cr->mNumerator && mDenominator == cr->mDenominator);
    return NOERROR;
}

ECode CRational::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string)

    Boolean bval;
    if (IsNaN(&bval), bval) {
        *string = String("NaN");
        return NOERROR;
    }
    else if (IsPosInf()) {
         *string = String("Infinity");
         return NOERROR;
    }
    else if (IsNegInf()) {
         *string = String("-Infinity");
         return NOERROR;
    }

    StringBuilder sb;
    sb.Append(mNumerator);
    sb.Append("/");
    sb.Append(mDenominator);
    *string = sb.ToString();
    return NOERROR;
}

ECode CRational::ToFloat(
    /* [out] */ Float* value)
{
    // TODO: remove this duplicate function (used in CTS and the shim)
    return FloatValue(value);
}

ECode CRational::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    // Bias the hash code for the first (2^16) values for both numerator and denominator
    Int32 numeratorFlipped = mNumerator << 16 | (((Int64)mNumerator) >> 16);
    *value = mDenominator ^ numeratorFlipped;
    return NOERROR;
}

Int32 CRational::Gcd(
    /* [in] */ Int32 numerator,
    /* [in] */ Int32 denominator)
{
    /*
     * Non-recursive implementation of Euclid's algorithm:
     *
     *  gcd(a, 0) := a
     *  gcd(a, b) := gcd(b, a mod b)
     *
     */
    Int32 a = numerator;
    Int32 b = denominator;

    while (b != 0) {
        Int32 oldB = b;

        b = a % b;
        a = oldB;
    }

    return Elastos::Core::Math::Abs(a);
}

ECode CRational::ByteValue(
    /* [out] */ Byte* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 value;
    Int32Value(&value);
    *result = (Byte)value;
    return NOERROR;
}

ECode CRational::Int16Value(
    /* [out] */ Int16* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 value;
    Int32Value(&value);
    *result = (Int16)value;
    return NOERROR;
}

ECode CRational::Int32Value(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    // Mimic float to Int32 conversion rules from JLS 5.1.3
    Boolean bval;
    if (IsPosInf()) {
        *result = Elastos::Core::Math::INT32_MAX_VALUE;
    }
    else if (IsNegInf()) {
        *result = Elastos::Core::Math::INT32_MIN_VALUE;
    }
    else if (IsNaN(&bval), bval) {
        *result = 0;
    }
    else { // finite
        *result = mNumerator / mDenominator;
    }
    return NOERROR;
}

ECode CRational::Int64Value(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    // Mimic float to Int64 conversion rules from JLS 5.1.3

    Boolean bval;
    if (IsPosInf()) {
        *result = Elastos::Core::Math::INT64_MAX_VALUE;
    }
    else if (IsNegInf()) {
        *result = Elastos::Core::Math::INT64_MIN_VALUE;
    }
    else if (IsNaN(&bval), bval) {
        *result = 0;
    }
    else { // finite
        *result = mNumerator / mDenominator;
    }
    return NOERROR;
}

ECode CRational::FloatValue(
    /* [out] */ Float* result)
{
    VALIDATE_NOT_NULL(result)
    Float num = mNumerator;
    Float den = mDenominator;

    *result = num / den;
    return NOERROR;
}

ECode CRational::DoubleValue(
    /* [out] */ Double* result)
{
    VALIDATE_NOT_NULL(result)
    Double num = mNumerator;
    Double den = mDenominator;

    *result = num / den;
    return NOERROR;
}

ECode CRational::CompareTo(
    /* [in] */ IInterface* obj,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = -1;
    VALIDATE_NOT_NULL(obj)

    CRational* another= (CRational*)IRational::Probe(obj);
    VALIDATE_NOT_NULL(another)

    Boolean bval;
    if (Equals(obj, &bval), bval) {
        *result = 0;
        return NOERROR;
    }
    else if (IsNaN(&bval), bval) { // NaN is greater than the other non-NaN value
        *result = 1;
        return NOERROR;
    }
    else if (another->IsNaN(&bval), bval) { // the other NaN is greater than this non-NaN value
        *result = -1;
        return NOERROR;
    }
    else if ((IsPosInf()) || (another->IsNegInf())) {
        *result = 1; // positive infinity is greater than any non-NaN/non-posInf value
        return NOERROR;
    }
    else if ((IsNegInf()) || (another->IsPosInf())) {
        *result = -1; // negative infinity is less than any non-NaN/non-negInf value
        return NOERROR;
    }

    // else both this and another are finite numbers

    // make the denominators the same, then compare numerators
    Int64 thisNumerator = ((Int64)mNumerator) * another->mDenominator; // Int64 to avoid overflow
    Int64 otherNumerator = ((Int64)another->mNumerator) * mDenominator; // Int64 to avoid overflow

    // avoid underflow from subtraction by doing comparisons
    if (thisNumerator < otherNumerator) {
        *result = -1;
        return NOERROR;
    }
    else if (thisNumerator > otherNumerator) {
        *result = 1;
        return NOERROR;
    }
    else {
        // This should be covered by #equals, but have this code path just in case
        *result = 0;
        return NOERROR;
    }
}

ECode CRational::ParseRational(
    /* [in] */ const String& string,
    /* [out] */ IRational** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (string.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    //checkNotNull(string, "string must not be null");

    if (string.Equals("NaN")) {
        *result = NaN;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (string.Equals("Infinity")) {
        *result = POSITIVE_INFINITY;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (string.Equals("-Infinity")) {
        *result = NEGATIVE_INFINITY;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    Int32 sep_ix = string.IndexOf(':');
    if (sep_ix < 0) {
        sep_ix = string.IndexOf('/');
    }
    if (sep_ix < 0) {
        //throw invalidRational(string);
        return E_NUMBER_FORMAT_EXCEPTION;
    }

    // try {
    Int32 n, d;
    FAIL_RETURN(StringUtils::Parse(string.Substring(0, sep_ix), &n))
    FAIL_RETURN(StringUtils::Parse(string.Substring(sep_ix + 1), &d))

    AutoPtr<CRational> r;
    CRational::NewByFriend(n, d, (CRational**)&r);
    // } catch (NumberFormatException e) {
    //     throw invalidRational(string);
    // }
    *result = (IRational*)r.Get();
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // namespace Utility
} // namespace Droid
} // namespace Elastos
