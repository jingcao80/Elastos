
#include "CTime.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Sql {

const String CTime::PADDING("00");

CAR_OBJECT_IMPL(CTime);

CAR_INTERFACE_IMPL(CTime, Date, ITime);

ECode CTime::GetDate(
    /* [out] */ Int32 * pDate)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CTime::GetDay(
    /* [out] */ Int32 * pDay)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CTime::GetMonth(
    /* [out] */ Int32 * pMonth)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CTime::GetYear(
    /* [out] */ Int32 * pYear)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CTime::SetDate(
    /* [in] */ Int32 day)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CTime::SetMonth(
    /* [in] */ Int32 month)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CTime::SetTime(
    /* [in] */ Int64 milliseconds)
{
    return Date::SetTime(milliseconds);
}

ECode CTime::SetYear(
    /* [in] */ Int32 year)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CTime::ToString(
    /* [out] */ String * pLocaleStr)
{
    StringBuilder sb(8);

    Int32 value = 0;
    Date::GetHours(&value);
    Format(value, 2, &sb);
    sb.AppendChar(':');

    Date::GetMinutes(&value);
    Format(value, 2, &sb);
    sb.AppendChar(':');

    Date::GetSeconds(&value);
    Format(value, 2, &sb);

    sb.ToString(pLocaleStr);
    return NOERROR;
}

ECode CTime::constructor(
    /* [in] */ Int32 theHour,
    /* [in] */ Int32 theMinute,
    /* [in] */ Int32 theSecond)
{
    Date::constructor(70, 0, 1, theHour, theMinute, theSecond);
    return NOERROR;
}

ECode CTime::constructor(
    /* [in] */ Int64 theTime)
{
    Date::constructor(theTime);
    return NOERROR;
}

ECode CTime::ValueOf(
    /* [in] */ const String& timeString,
    /* [out] */ ITime** t)
{
    VALIDATE_NOT_NULL(t)
    *t = NULL;

    if (timeString.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    Int32 firstIndex = timeString.IndexOf(':');
    Int32 secondIndex = timeString.IndexOf(':', firstIndex + 1);
    // secondIndex == -1 means none or only one separator '-' has been
    // found.
    // The string is separated into three parts by two separator characters,
    // if the first or the third part is null string, we should throw
    // IllegalArgumentException to follow RI
    if (secondIndex == -1 || firstIndex == 0
        || secondIndex + 1 == timeString.GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    // parse each part of the string
    Int32 hour, minute, second;
    FAIL_RETURN(StringUtils::Parse(
        timeString.Substring(0, firstIndex), &hour))
    FAIL_RETURN(StringUtils::Parse(
        timeString.Substring(firstIndex + 1, secondIndex), &minute))
    FAIL_RETURN(StringUtils::Parse(
        timeString.Substring(secondIndex + 1, timeString.GetLength()), &second))
    AutoPtr<CTime> theTime;
    CTime::NewByFriend(hour, minute, second, (CTime **)&theTime);
    *t = (CTime*)theTime.Get();
    REFCOUNT_ADD(*t)
    return NOERROR;
}

void CTime::Format(
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

} // namespace Sql
} // namespace Elastos
