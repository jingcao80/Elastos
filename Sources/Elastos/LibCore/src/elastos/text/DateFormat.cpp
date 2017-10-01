//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "DateFormat.h"
#include "StringBuffer.h"
#include "AttributedCharacterIteratorAttribute.h"
#include "CSimpleDateFormat.h"
#include "CDate.h"
#include "ICUUtil.h"
#include "CLocaleHelper.h"
#include "CString.h"
#include "CDouble.h"
#include "CLocaleDataHelper.h"
#include "CLocaleData.h"
#include "CFieldPosition.h"
#include "CParsePosition.h"
#include "CDateFormatField.h"
#include "CDecimalFormatSymbols.h"

using Elastos::Core::INumber;
using Elastos::Core::EIID_INumber;
using Elastos::Core::ICharSequence;
using Elastos::Text::CFieldPosition;
using Elastos::Text::CParsePosition;
using Elastos::Utility::EIID_IDate;
using Elastos::Utility::CDate;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::CLocaleDataHelper;
using Libcore::ICU::ILocaleData;
using Libcore::ICU::CLocaleData;
using Libcore::ICU::ICUUtil;

namespace Elastos {
namespace Text {

static AutoPtr<IDateFormatField> InitField(const String& name, Int32 value)
{
    AutoPtr<IDateFormatField> cf;
    CDateFormatField::New(name, value, (IDateFormatField**)&cf);
    return cf;
}

const AutoPtr<IDateFormatField> DateFormat::Field::ERA
    = InitField(String("era"), ICalendar::ERA);
const AutoPtr<IDateFormatField> DateFormat::Field::YEAR
    = InitField(String("year"), ICalendar::YEAR);
const AutoPtr<IDateFormatField> DateFormat::Field::MONTH
    = InitField(String("month"), ICalendar::MONTH);
const AutoPtr<IDateFormatField> DateFormat::Field::HOUR_OF_DAY0
    = InitField(String("hour of day"), ICalendar::HOUR_OF_DAY);
const AutoPtr<IDateFormatField> DateFormat::Field::HOUR_OF_DAY1
    = InitField(String("hour of day 1"), -1);
const AutoPtr<IDateFormatField> DateFormat::Field::MINUTE
    = InitField(String("minute"), ICalendar::MINUTE);
const AutoPtr<IDateFormatField> DateFormat::Field::SECOND
    = InitField(String("second"), ICalendar::SECOND);
const AutoPtr<IDateFormatField> DateFormat::Field::MILLISECOND
    = InitField(String("millisecond"), ICalendar::MILLISECOND);
const AutoPtr<IDateFormatField> DateFormat::Field::DAY_OF_WEEK
    = InitField(String("day of week"), ICalendar::DAY_OF_WEEK);
const AutoPtr<IDateFormatField> DateFormat::Field::DAY_OF_MONTH
    = InitField(String("day of month"), ICalendar::DAY_OF_MONTH);
const AutoPtr<IDateFormatField> DateFormat::Field::DAY_OF_YEAR
    = InitField(String("day of year"), ICalendar::DAY_OF_YEAR);
const AutoPtr<IDateFormatField> DateFormat::Field::DAY_OF_WEEK_IN_MONTH
    = InitField(String("day of week in month"), ICalendar::DAY_OF_WEEK_IN_MONTH);
const AutoPtr<IDateFormatField> DateFormat::Field::WEEK_OF_YEAR
    = InitField(String("week of year"), ICalendar::WEEK_OF_YEAR);
const AutoPtr<IDateFormatField> DateFormat::Field::WEEK_OF_MONTH
    = InitField(String("week of month"), ICalendar::WEEK_OF_MONTH);
const AutoPtr<IDateFormatField> DateFormat::Field::AM_PM
    = InitField(String("am pm"), ICalendar::AM_PM);
const AutoPtr<IDateFormatField> DateFormat::Field::HOUR0
    = InitField(String("hour"), ICalendar::HOUR);
const AutoPtr<IDateFormatField> DateFormat::Field::HOUR1
    = InitField(String("hour 1"), -1);
const AutoPtr<IDateFormatField> DateFormat::Field::TIME_ZONE
    = InitField(String("time zone"), -1);

HashMap<Int32, AutoPtr<IDateFormatField> > DateFormat::Field::sTable(11);

CAR_INTERFACE_IMPL(DateFormat::Field, FormatBase::Field, IDateFormatField)

DateFormat::Field::Field()
    : mCalendarField(-1)
{ }

ECode DateFormat::Field::constructor(
    /* [in] */ const String& fieldName,
    /* [in] */ Int32 calendarField)
{
    FAIL_RETURN(FormatBase::Field::constructor(fieldName));
    mCalendarField = calendarField;
    // if (calendarField != -1 && sTable.Find(calendarField) == sTable.End()) {
    if (calendarField != -1 && sTable[calendarField] == NULL) {
        sTable[calendarField] = this;
    }
    return NOERROR;
}
ECode DateFormat::Field::GetCalendarField(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mCalendarField;
    return NOERROR;
}

ECode DateFormat::Field::OfCalendarField(
    /* [in] */ Int32 calendarField,
    /* [out] */ IDateFormatField** field)
{
    VALIDATE_NOT_NULL(field);
    *field = NULL;

    if (calendarField < 0 || calendarField >= ICalendar::FIELD_COUNT) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    HashMap<Int32, AutoPtr<IDateFormatField> >::Iterator it = sTable.Find(calendarField);
    if (it != sTable.End()) {
        *field = it->mSecond;
        REFCOUNT_ADD(*field);
    }
    return NOERROR;
}

CAR_INTERFACE_IMPL(DateFormat, FormatBase, IDateFormat)

DateFormat::DateFormat()
{}

ECode DateFormat::CloneImpl(
    /* [in] */ IDateFormat* object)
{
    VALIDATE_NOT_NULL(object)

    FormatBase::CloneImpl(IFormat::Probe(object));

    DateFormat* df = (DateFormat*)object;
    AutoPtr<IInterface> temp;
    ICloneable::Probe(mCalendar)->Clone((IInterface**)&temp);
    df->mCalendar = ICalendar::Probe(temp);
    temp = NULL;
    ICloneable::Probe(mNumberFormat)->Clone((IInterface**)&temp);
    df->mNumberFormat = INumberFormat::Probe(temp);
    return NOERROR;
}

ECode DateFormat::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean * result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    IDateFormat* df = IDateFormat::Probe(object);
    if (df == NULL) {
        return NOERROR;
    }

    DateFormat* other = (DateFormat*)df;
    if (other == this) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean value = Object::Equals(mNumberFormat, other->mNumberFormat);
    if (!value) return NOERROR;

    AutoPtr<ITimeZone> ltz, rtz;
    mCalendar->GetTimeZone((ITimeZone**)&ltz);
    other->mCalendar->GetTimeZone((ITimeZone**)&rtz);
    value = Object::Equals(ltz, rtz);
    if (!value) return NOERROR;

    Int32 li, ri;
    mCalendar->GetFirstDayOfWeek(&li);
    other->mCalendar->GetFirstDayOfWeek(&ri);
    value = li == ri;
    if (!value) return NOERROR;

    mCalendar->GetMinimalDaysInFirstWeek(&li);
    other->mCalendar->GetMinimalDaysInFirstWeek(&ri);
    value = li == ri;
    if (!value) return NOERROR;

    Boolean lb, rb;
    mCalendar->IsLenient(&lb);
    other->mCalendar->IsLenient(&rb);
    *result = (lb == rb);
    return NOERROR;
}

ECode DateFormat::Format(
    /* [in] */ IInterface* object,
    /* [in] */ IStringBuffer * buffer,
    /* [in] */ IFieldPosition* field)
{
    VALIDATE_NOT_NULL(object)
    VALIDATE_NOT_NULL(buffer)
    VALIDATE_NOT_NULL(field)

    IDate* date = IDate::Probe(object);
    if (date != NULL) {
        return Format(date, buffer, field);
    }

    INumber* number = INumber::Probe(object) ;
    if (number != NULL) {
        Int64 v;
        number->Int64Value(&v);
        AutoPtr<IDate> dateObj;
        CDate::New(v, (IDate**)&dateObj);
        return Format(dateObj, buffer, field);
    }

    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode DateFormat::Format(
    /* [in] */ IDate* date,
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);

    AutoPtr<IFieldPosition> field;
    CFieldPosition::New(0, (IFieldPosition**)&field);
    AutoPtr<IStringBuffer> sb = new StringBuffer();
    Format(date, sb, field);
    return ICharSequence::Probe(sb)->ToString(value);
}

ECode DateFormat::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    return ICUUtil::GetAvailableDateFormatLocales(locales);
}

ECode DateFormat::GetCalendar(
    /* [out] */ ICalendar** calendar)
{
    VALIDATE_NOT_NULL(calendar);
    *calendar = mCalendar;
    REFCOUNT_ADD(*calendar);
    return NOERROR;
}

ECode DateFormat::GetDateInstance(
    /* [out] */ IDateFormat** instance)
{
    return GetDateInstance(IDateFormat::DEFAULT, instance);
}

ECode DateFormat::GetDateInstance(
    /* [in] */ Int32 style,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;

    FAIL_RETURN(CheckDateStyle(style));

    AutoPtr<ILocaleHelper> localeHelper;
    FAIL_RETURN(CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper));
    AutoPtr<ILocale> locale;
    FAIL_RETURN(localeHelper->GetDefault((ILocale**)&locale));

    return GetDateInstance(style, locale, instance);
}

ECode DateFormat::GetDateInstance(
    /* [in] */ Int32 style,
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;

    FAIL_RETURN(CheckDateStyle(style));

    if (locale == NULL)
        return E_NULL_POINTER_EXCEPTION;

    AutoPtr<ILocaleDataHelper> localeDataHelper;
    FAIL_RETURN(CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&localeDataHelper));
    AutoPtr<ILocaleData> localeData;
    FAIL_RETURN(localeDataHelper->Get(locale, (ILocaleData**)&localeData));
    String format;
    localeData->GetDateFormat(style, &format);

    AutoPtr<ISimpleDateFormat> f;
    FAIL_RETURN(CSimpleDateFormat::New(format, locale, (ISimpleDateFormat**)&f));
    *instance = IDateFormat::Probe(f.Get());
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode DateFormat::GetDateTimeInstance(
    /* [out] */ IDateFormat** instance)
{
    return GetDateTimeInstance(IDateFormat::DEFAULT, IDateFormat::DEFAULT, instance);
}

ECode DateFormat::GetDateTimeInstance(
    /* [in] */ Int32 dateStyle,
    /* [in] */ Int32 timeStyle,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;

    FAIL_RETURN(CheckTimeStyle(timeStyle));
    FAIL_RETURN(CheckDateStyle(dateStyle));

    AutoPtr<ILocaleHelper> localeHelper;
    FAIL_RETURN(CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper));
    AutoPtr<ILocale> locale;
    localeHelper->GetDefault((ILocale**)&locale);

    return GetDateTimeInstance(dateStyle, timeStyle, locale, instance);
}

ECode DateFormat::GetDateTimeInstance(
    /* [in] */ Int32 dateStyle,
    /* [in] */ Int32 timeStyle,
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;

    FAIL_RETURN(CheckTimeStyle(timeStyle));
    FAIL_RETURN(CheckDateStyle(dateStyle));

    if (locale == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ILocaleDataHelper> localeDataHelper;
    FAIL_RETURN(CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&localeDataHelper));
    AutoPtr<ILocaleData> localeData;
    FAIL_RETURN(localeDataHelper->Get(locale, (ILocaleData**)&localeData));
    String dateFormat;
    String timeFormat;
    localeData->GetDateFormat(dateStyle, &dateFormat);
    localeData->GetTimeFormat(timeStyle, &timeFormat);
    String pattern = dateFormat + " " + timeFormat;

    AutoPtr<ISimpleDateFormat> f;
    FAIL_RETURN(CSimpleDateFormat::New(pattern, locale, (ISimpleDateFormat**)&f));
    *instance = IDateFormat::Probe(f);
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode DateFormat::GetInstance(
    /* [out] */ IDateFormat** instance)
{
    return GetDateTimeInstance(IDateFormat::SHORT, IDateFormat::SHORT, instance);
}

ECode DateFormat::Set24HourTimePref(
        /* [in] */ Boolean is24Hour)
{
    return NOERROR;
}

ECode DateFormat::GetNumberFormat(
    /* [out] */ INumberFormat** numberFormat)
{
    VALIDATE_NOT_NULL(numberFormat);
    *numberFormat = mNumberFormat;
    REFCOUNT_ADD(*numberFormat);
    return NOERROR;
}

ECode DateFormat::GetTimeInstance(
    /* [out] */ IDateFormat** instance)
{
    return GetTimeInstance(IDateFormat::DEFAULT, instance);
}

ECode DateFormat::GetTimeInstance(
    /* [in] */ Int32 style,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;

    FAIL_RETURN(CheckTimeStyle(style));
    AutoPtr<ILocaleHelper> localeHelper;
    FAIL_RETURN(CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localeHelper));
    AutoPtr<ILocale> locale;
    FAIL_RETURN(localeHelper->GetDefault((ILocale**)&locale));

    return GetTimeInstance(style, locale, instance);
}

ECode DateFormat::GetTimeInstance(
    /* [in] */ Int32 style,
    /* [in] */ ILocale* locale,
    /* [out] */ IDateFormat** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;

    FAIL_RETURN(CheckTimeStyle(style));

    if (locale == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    AutoPtr<ILocaleDataHelper> localeDataHelper;
    FAIL_RETURN(CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&localeDataHelper));
    AutoPtr<ILocaleData> localeData;
    FAIL_RETURN(localeDataHelper->Get(locale, (ILocaleData**)&localeData));
    String timeFormat;
    localeData->GetTimeFormat(style, &timeFormat);

    AutoPtr<ISimpleDateFormat> f;
    FAIL_RETURN(CSimpleDateFormat::New(timeFormat, locale, (ISimpleDateFormat**)&f));
    *instance = IDateFormat::Probe(f.Get());
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode DateFormat::GetTimeZone(
    /* [out] */ ITimeZone** tz)
{
    return mCalendar->GetTimeZone(tz);
}

ECode DateFormat::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)

    Int32 value = 0, temp;
    mCalendar->GetFirstDayOfWeek(&temp);
    value += temp;

    mCalendar->GetMinimalDaysInFirstWeek(&temp);
    value += temp;

    AutoPtr<ITimeZone> tz;
    mCalendar->GetTimeZone((ITimeZone**)&tz);
    value += Object::GetHashCode(tz);

    Boolean bval;
    mCalendar->IsLenient(&bval);
    value += bval ? 1231 : 1237;

    value += Object::GetHashCode(mNumberFormat);

    *hash = value;
    return NOERROR;
}

ECode DateFormat::IsLenient(
    /* [out] */ Boolean* isLenient)
{
    return mCalendar->IsLenient(isLenient);
}

ECode DateFormat::Parse(
    /* [in] */ const String& string,
    /* [out] */ IDate** date)
{
    VALIDATE_NOT_NULL(date);
    *date = NULL;

    AutoPtr<IParsePosition> position;
    CParsePosition::New(0, (IParsePosition**)&position);
    AutoPtr<IDate> d;
    FAIL_RETURN(Parse(string, position, (IDate**)&d));

    Int32 index;
    position->GetIndex(&index);
    if (index == 0) {
        return E_PARSE_EXCEPTION;
    }

    *date = d;
    REFCOUNT_ADD(*date);
    return NOERROR;
}

ECode DateFormat::ParseObject(
    /* [in] */ const String& string,
    /* [in] */ IParsePosition* position,
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    *object = NULL;

    AutoPtr<IDate> date;
    FAIL_RETURN(Parse(string, position, (IDate**)&date))
    *object = date;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode DateFormat::SetCalendar(
    /* [in] */ ICalendar* cal)
{
    mCalendar = cal;
    return NOERROR;
}

ECode DateFormat::SetLenient(
    /* [in] */ Boolean value)
{
    return mCalendar->SetLenient(value);
}

ECode DateFormat::SetNumberFormat(
   /* [in] */ INumberFormat* format)
{
    mNumberFormat = format;
    return NOERROR;
}

ECode DateFormat::SetTimeZone(
    /* [in] */ ITimeZone* timezone)
{
    return mCalendar->SetTimeZone(timezone);
}

ECode DateFormat::CheckDateStyle(
    /* [in] */ Int32 style)
{
    if (!(style == IDateFormat::SHORT
            || style == IDateFormat::MEDIUM
            || style == IDateFormat::LONG
            || style == IDateFormat::FULL
            || style == IDateFormat::DEFAULT)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode DateFormat::CheckTimeStyle(
    /* [in] */ Int32 style)
{
    if (!(style == IDateFormat::SHORT
            || style == IDateFormat::MEDIUM
            || style == IDateFormat::LONG
            || style == IDateFormat::FULL
            || style == IDateFormat::DEFAULT)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

} // namespace Text
} // namespace Elastos
