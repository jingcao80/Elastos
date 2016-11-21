
#include "CTimestamp.h"
#include "StringUtils.h"
#include "Pattern.h"
#include "CLocale.h"
#include "CSimpleDateFormat.h"
#include "CParsePosition.h"
#include <cutils/log.h>

using Elastos::Core::StringUtils;
using Elastos::Utility::IDate;
using Elastos::Utility::ILocale;
using Elastos::Utility::CLocale;
using Elastos::Utility::Regex::Pattern;
using Elastos::Text::IDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IParsePosition;
using Elastos::Text::CParsePosition;

namespace Elastos {
namespace Sql {

const String CTimestamp::TIME_FORMAT_REGEX("[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}.*");
const String CTimestamp::PADDING("000000000");

CAR_INTERFACE_IMPL(CTimestamp, Date, ITimestamp)

CAR_OBJECT_IMPL(CTimestamp)

CTimestamp::CTimestamp()
    : mNanos(0)
{}

ECode CTimestamp::constructor(
    /* [in] */ Int32 theYear,
    /* [in] */ Int32 theMonth,
    /* [in] */ Int32 theDate,
    /* [in] */ Int32 theHour,
    /* [in] */ Int32 theMinute,
    /* [in] */ Int32 theSecond,
    /* [in] */ Int32 theNano)
{
    FAIL_RETURN(Date::constructor(theYear, theMonth, theDate, theHour, theMinute, theSecond))

    if (theNano < 0 || theNano > 999999999) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mNanos = theNano;
    return NOERROR;
}

ECode CTimestamp::constructor(
    /* [in] */ Int64 theTime)
{
    FAIL_RETURN(Date::constructor(theTime))

    /*
     * Now set the time for this Timestamp object - which deals with the
     * nanosecond value as well as the base time
     */
    return SetTimeImpl(theTime);
}

ECode CTimestamp::CompareTo(
    /* [in] */ IInterface* date,
    /* [out] */ Int32* result)
{
    return CompareTo(ITimestamp::Probe(date), result);
}

ECode CTimestamp::GetTime(
    /* [out] */ Int64* time)
{
    VALIDATE_NOT_NULL(time)

    Int64 theTime = 0;
    Date::GetTime(&theTime);
    theTime = theTime + (mNanos / 1000000);
    *time = theTime;
    return NOERROR;
}

ECode CTimestamp::SetTime(
    /* [in] */ Int64 milliseconds)
{
    return SetTimeImpl(milliseconds);
}

ECode CTimestamp::ToString(
    /* [out] */ String* localeStr)
{
    StringBuilder sb(29);

    Int32 value = 0;
    Date::GetYear(&value);
    Format(value + 1900, 4, &sb);
    sb.AppendChar('-');

    Date::GetMonth(&value);
    Format(value + 1, 2, &sb);
    sb.AppendChar('-');

    Date::GetDate(&value);
    Format(value, 2, &sb);
    sb.AppendChar(' ');

    Date::GetHours(&value);
    Format(value, 2, &sb);
    sb.AppendChar(':');

    Date::GetMinutes(&value);
    Format(value, 2, &sb);
    sb.AppendChar(':');

    Date::GetSeconds(&value);
    Format(value, 2, &sb);
    sb.AppendChar('.');
    if (mNanos == 0) {
        sb.AppendChar('0');
    } else {
        Format(mNanos, 9, &sb);
        while (sb.GetCharAt(sb.GetLength() - 1) == '0') {
            sb.SetLength(sb.GetLength() - 1);
        }
    }

    sb.ToString(localeStr);
    return NOERROR;
}

ECode CTimestamp::After(
    /* [in] */ ITimestamp * theTimestamp,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    Int64 thisTime = 0;
    GetTime(&thisTime);
    Int64 compareTime = 0;
    IDate::Probe(theTimestamp)->GetTime(&compareTime);

    Int32 thisNano = 0;
    GetNanos(&thisNano);
    Int32 comparenano = 0;
    theTimestamp->GetNanos(&comparenano);

    // If the time value is later, the timestamp is later
    if (thisTime > compareTime) {
        *value = TRUE;
    }
    // If the time value is earlier, the timestamp is not later
    else if (thisTime < compareTime) {
        *value = FALSE;
    }
    /*
     * Otherwise the time values are equal in which case the nanoseconds
     * value determines whether this timestamp is later...
     */
    else if (thisNano > comparenano) {
        *value = TRUE;
    }
    else {
        *value = FALSE;
    }
    return NOERROR;
}

ECode CTimestamp::Before(
    /* [in] */ ITimestamp * theTimestamp,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value)
    *value = FALSE;

    Int64 thisTime = 0;
    GetTime(&thisTime);
    Int64 compareTime = 0;
    IDate::Probe(theTimestamp)->GetTime(&compareTime);

    Int32 thisNano = 0;
    GetNanos(&thisNano);
    Int32 comparenano = 0;
    theTimestamp->GetNanos(&comparenano);

    // If the time value is later, the timestamp is later
    if (thisTime < compareTime) {
        *value = TRUE;
    }
    // If the time value is earlier, the timestamp is not later
    else if (thisTime > compareTime) {
        *value = FALSE;
    }
    /*
     * Otherwise the time values are equal in which case the nanoseconds
     * value determines whether this timestamp is later...
     */
    else if (thisNano < comparenano) {
        *value = TRUE;
    }
    else {
        *value = FALSE;
    }
    return NOERROR;
}

ECode CTimestamp::CompareTo(
    /* [in] */ ITimestamp * theTimestamp,
    /* [out] */ Int32 * value)
{
    VALIDATE_NOT_NULL(value)

    Int32 result = 0;
    Date::CompareTo(TO_IINTERFACE(theTimestamp), &result);
    if (result == 0) {
        Int32 thatNano = 0;
        theTimestamp->GetNanos(&thatNano);
        if (mNanos > thatNano) {
            *value = 1;
        }
        else if (mNanos == thatNano) {
            *value = 0;
        }
        else {
            *value = -1;
        }
    }
    else {
        *value = result;
    }
    return NOERROR;
}

ECode CTimestamp::Equals(
    /* [in] */ IInterface * pTheObject,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;
    VALIDATE_NOT_NULL(pTheObject);

    if (ITimestamp::Probe(pTheObject) == NULL) return NOERROR;

    return Equals(ITimestamp::Probe(pTheObject), value);
}

ECode CTimestamp::Equals(
    /* [in] */ ITimestamp * theTimestamp,
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value);
    *value = FALSE;
    VALIDATE_NOT_NULL(theTimestamp);

    Int64 atime = 0;
    Int64 btime = 0;
    Int32 bnanos = 0;
    GetTime(&atime);
    IDate::Probe(theTimestamp)->GetTime(&btime);
    theTimestamp->GetNanos(&bnanos);
    *value = (atime == btime) && (mNanos == bnanos);
    return NOERROR;
}

ECode CTimestamp::GetNanos(
    /* [out] */ Int32 * pNano)
{
    VALIDATE_NOT_NULL(pNano)
    *pNano = mNanos;
    return NOERROR;
}

ECode CTimestamp::SetNanos(
    /* [in] */ Int32 n)
{
    if ((n < 0) || (n > 999999999)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mNanos = n;
    return NOERROR;
}

ECode CTimestamp::ValueOf(
    /* [in] */ const String& str,
    /* [out] */ ITimestamp** ts)
{
    VALIDATE_NOT_NULL(ts)
    *ts = NULL;

    if (str.IsNull()) {
        //throw new IllegalArgumentException("Argument cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    // Omit trailing whitespace
    String s = str.Trim();
    Boolean ispat = Pattern::Matches(TIME_FORMAT_REGEX, s);
    if (!ispat) {
        return BadTimestampString(s);;
    }

    AutoPtr<ISimpleDateFormat> df;
    CSimpleDateFormat::New(String("yyyy-MM-dd HH:mm:ss"), CLocale::US, (ISimpleDateFormat**)&df);
    AutoPtr<IParsePosition> pp;
    CParsePosition::New(0, (IParsePosition**)&pp);

    /*
     * First parse out the yyyy-MM-dd HH:mm:ss component of the String into
     * a Date object using the SimpleDateFormat. This should stop after the
     * seconds value, according to the definition of SimpleDateFormat.parse,
     * with the ParsePosition indicating the index of the "." which should
     * precede the nanoseconds value
     */
    AutoPtr<IDate> theDate;
    ECode ec = IDateFormat::Probe(df)->Parse(s, pp, (IDate**)&theDate);
    if (FAILED(ec)) {
        return BadTimestampString(s);;
    }

    if (theDate == NULL) {
        return BadTimestampString(s);;
    }

    /*
     * If we get here, the Date part of the string was OK - now for the
     * nanoseconds value. Strictly, this requires the remaining part of the
     * String to look like ".nnnnnnnnn". However, we accept anything with a
     * '.' followed by 1 to 9 digits - we also accept nothing (no fractions
     * of a second). Anything else is interpreted as incorrect format which
     * will generate an IllegalArgumentException
     */
    Int32 position = 0;
    pp->GetIndex(&position);
    Int32 remaining = s.GetLength() - position;
    Int32 nanos;

    if (remaining == 0) {
        // First, allow for the case where no fraction of a second is given:
        nanos = 0;
    } else {
        // Validate the string is in the range ".0" to ".999999999"
        if (remaining < 2 || remaining > 10 || s.GetChar(position) != '.') {
            return BadTimestampString(s);
        }

        Int64 tmp = 0;
        if (FAILED(StringUtils::ParsePositiveInt64(s.Substring(position + 1), &tmp))) {
            return BadTimestampString(s);
        }
        nanos = tmp;
        // We must adjust for the cases where the nanos String was not 9
        // characters long (i.e. ".123" means 123000000 nanos)
        if (nanos != 0) {
            for (Int32 i = remaining - 1; i < 9; i++) {
                nanos *= 10;
            }
        }
    }

    AutoPtr<ITimestamp> theTimestamp;
    Int64 outtime = 0;
    theDate->GetTime(&outtime);
    CTimestamp::New(outtime, (ITimestamp**)&theTimestamp);
    return theTimestamp->SetNanos(nanos);
}

ECode CTimestamp::BadTimestampString(
    /* [in] */ const String& s)
{
    ALOGE("E_ILLEGAL_ARGUMENT_EXCEPTION: Timestamp format must be "
        "yyyy-MM-dd HH:mm:ss.fffffffff; was '%s'", s.string());
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

void CTimestamp::Format(
    /* [in] */ Int32 date,
    /* [in] */ Int32 digits,
    /* [in] */ StringBuilder* sb)
{
    String str = StringUtils::ToString(date);
    if (digits - str.GetLength() > 0) {
        sb->Append(PADDING.Substring(0, digits - str.GetLength()));
    }
    sb->Append(str);
}

ECode CTimestamp::SetTimeImpl(
    /* [in] */ Int64 theTime)
{
    Int32 milliseconds = (Int32) (theTime % 1000);
    theTime = theTime - milliseconds;
    if (milliseconds < 0) {
        theTime = theTime - 1000;
        milliseconds = 1000 + milliseconds;
    }
    FAIL_RETURN(Date::SetTime(theTime))
    return SetNanos(milliseconds * 1000000);
}

} // namespace Sql
} // namespace Elastos
