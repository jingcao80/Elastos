
#include "CSqlDate.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Sql {

const String CSqlDate::PADDING("0000");

CAR_INTERFACE_IMPL(CSqlDate, Date, ISQLDate);
CAR_OBJECT_IMPL(CSqlDate);

ECode CSqlDate::constructor(
    /* [in] */ Int32 theYear,
    /* [in] */ Int32 theMonth,
    /* [in] */ Int32 theDay)
{
    return Date::constructor(theYear, theMonth, theDay);
}

ECode CSqlDate::constructor(
    /* [in] */ Int64 theDate)
{
    return Date::constructor(NormalizeTime(theDate));
}

ECode CSqlDate::GetHours(
    /* [out] */ Int32 * value)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CSqlDate::GetMinutes(
    /* [out] */ Int32 * value)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CSqlDate::GetSeconds(
    /* [out] */ Int32 * value)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CSqlDate::SetHours(
    /* [in] */ Int32 hour)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CSqlDate::SetMinutes(
    /* [in] */ Int32 minute)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CSqlDate::SetSeconds(
    /* [in] */ Int32 second)
{
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CSqlDate::SetTime(
    /* [in] */ Int64 milliseconds)
{
    return Date::SetTime(NormalizeTime(milliseconds));
}

ECode CSqlDate::ToString(
    /* [out] */ String * str)
{
    StringBuilder sb(10);

    Int32 value = 0;
    Date::GetYear(&value);
    Format(value + 1900, 4, &sb);
    sb.AppendChar('-');

    Date::GetMonth(&value);
    Format(value + 1, 2, &sb);
    sb.AppendChar('-');

    Date::GetDate(&value);
    Format(value, 2, &sb);
    sb.ToString(str);
    return NOERROR;
}

ECode CSqlDate::ValueOf(
    /* [in] */ const String& dateString,
    /* [out] */ ISQLDate** date)
{
    VALIDATE_NOT_NULL(date)
    *date = NULL;

    if (dateString == NULL) {
        // throw new IllegalArgumentException("dateString == null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (dateString.GetLength() > 10) {
        // early fail to avoid parsing huge invalid strings
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<String> > parts;
    StringUtils::Split(dateString, "-", (ArrayOf<String>**)&parts);
    if (parts->GetLength() != 3) {
        // throw new IllegalArgumentException();
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Int64 value = 0;
    StringUtils::ParsePositiveInt64((*parts)[0], &value);
    Int32 year = value;
    StringUtils::ParsePositiveInt64((*parts)[1], &value);
    Int32 month = value;
    StringUtils::ParsePositiveInt64((*parts)[2], &value);
    Int32 day = value;

    return CSqlDate::New(year - 1900, month - 1, day, date);
}

Int64 CSqlDate::NormalizeTime(
    /* [in] */ Int64 theTime)
{
    return theTime;
}

void CSqlDate::Format(
    /* [in] */ Int32 date,
    /* [in] */ Int32 digits,
    /* [in] */ StringBuilder * sb)
{
    String str = StringUtils::ToString(date);
    if (digits - str.GetLength() > 0) {
        sb->Append(PADDING.Substring(0, digits - str.GetLength()));
    }
    sb->Append(str);
}

} // namespace Sql
} // namespace Elastos
