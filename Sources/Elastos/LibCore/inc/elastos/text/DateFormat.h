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

#ifndef __ELASTOS_TEXT_DATEFORMAT_H__
#define __ELASTOS_TEXT_DATEFORMAT_H__

#include "FormatBase.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::IDate;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ILocale;
using Elastos::Text::IFieldPosition;
using Elastos::Text::IParsePosition;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Text {

class DateFormat
    : public FormatBase
    , public IDateFormat
{
public:
    class Field
        : public FormatBase::Field
        , public IDateFormatField
    {
    public:
        CAR_INTERFACE_DECL()

        Field();

        CARAPI constructor(
            /* [in] */ const String& fieldName,
            /* [in] */ Int32 calendarField);

        CARAPI GetCalendarField(
            /* [out] */ Int32* field);

        static CARAPI OfCalendarField(
            /* [in] */ Int32 calendarField,
            /* [out] */ IDateFormatField** field);

    public:

        static const AutoPtr<IDateFormatField> ERA;

        static const AutoPtr<IDateFormatField> YEAR;

        static const AutoPtr<IDateFormatField> MONTH;

        static const AutoPtr<IDateFormatField> HOUR_OF_DAY0;

        static const AutoPtr<IDateFormatField> HOUR_OF_DAY1;

        static const AutoPtr<IDateFormatField> MINUTE;

        static const AutoPtr<IDateFormatField> SECOND;

        static const AutoPtr<IDateFormatField> MILLISECOND;

        static const AutoPtr<IDateFormatField> DAY_OF_WEEK;

        static const AutoPtr<IDateFormatField> DAY_OF_MONTH;

        static const AutoPtr<IDateFormatField> DAY_OF_YEAR;

        static const AutoPtr<IDateFormatField> DAY_OF_WEEK_IN_MONTH;

        static const AutoPtr<IDateFormatField> WEEK_OF_YEAR;

        static const AutoPtr<IDateFormatField> WEEK_OF_MONTH;

        static const AutoPtr<IDateFormatField> AM_PM;

        static const AutoPtr<IDateFormatField> HOUR0;

        static const AutoPtr<IDateFormatField> HOUR1;

        static const AutoPtr<IDateFormatField> TIME_ZONE;

    private:

        static HashMap<Int32, AutoPtr<IDateFormatField> > sTable;

        Int32 mCalendarField;
    };

public:
    CAR_INTERFACE_DECL()

    DateFormat();

    CARAPI CloneImpl(
        /* [in] */ IDateFormat* object);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean * result);

    using FormatBase::Format;

    //@Override
    CARAPI Format(
        /* [in] */ IInterface* object,
        /* [in] */ IStringBuffer * buffer,
        /* [in] */ IFieldPosition* field);

    CARAPI Format(
        /* [in] */ IDate* date,
        /* [out] */ String* result);

    virtual CARAPI Format(
        /* [in] */ IDate* date,
        /* [in] */ IStringBuffer * buffer,
        /* [in] */ IFieldPosition* field) = 0;

    static CARAPI GetAvailableLocales(
        /* [out] */ ArrayOf<ILocale*>** locales);

    CARAPI GetCalendar(
        /* [out] */ ICalendar** calendar);

    static CARAPI GetDateInstance(
        /* [out] */ IDateFormat** instance);

    static CARAPI GetDateInstance(
        /* [in] */ Int32 style,
        /* [out] */ IDateFormat** instance);

    static CARAPI GetDateInstance(
        /* [in] */ Int32 style,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);

    static CARAPI GetDateTimeInstance(
        /* [out] */ IDateFormat** instance);

    static CARAPI GetDateTimeInstance(
        /* [in] */ Int32 dateStyle,
        /* [in] */ Int32 timeStyle,
        /* [out] */ IDateFormat** instance);

    static CARAPI GetDateTimeInstance(
        /* [in] */ Int32 dateStyle,
        /* [in] */ Int32 timeStyle,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);

    static CARAPI GetInstance(
        /* [out] */ IDateFormat** instance);

    CARAPI GetNumberFormat(
        /* [out] */ INumberFormat** numberFormat);

    static CARAPI GetTimeInstance(
        /* [out] */ IDateFormat** instance);

    static CARAPI GetTimeInstance(
        /* [in] */ Int32 style,
        /* [out] */ IDateFormat** instance);

    static CARAPI GetTimeInstance(
        /* [in] */ Int32 style,
        /* [in] */ ILocale* locale,
        /* [out] */ IDateFormat** instance);

    CARAPI GetTimeZone(
        /* [out] */ ITimeZone** timeZone);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI IsLenient(
        /* [out] */ Boolean* isLenient);

    CARAPI Parse(
        /* [in] */ const String& string,
        /* [out] */ IDate** date);

    virtual CARAPI Parse(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ IDate** date) = 0;

    //@Override
    CARAPI ParseObject(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ IInterface** object);

    CARAPI SetCalendar(
        /* [in] */ ICalendar* cal);

    CARAPI SetLenient(
        /* [in] */ Boolean value);

    CARAPI SetNumberFormat(
       /* [in] */ INumberFormat* format);

    CARAPI SetTimeZone(
        /* [in] */ ITimeZone* timezone);

    /**
     * @hide for internal use only.
     */
    static CARAPI Set24HourTimePref(
        /* [in] */ Boolean is24Hour);

private:
    static CARAPI CheckDateStyle(
        /* [in] */ Int32 style);

    static CARAPI CheckTimeStyle(
        /* [in] */ Int32 style);

    CARAPI InitNative(
        /* [in] */ const String & patter);

protected:
    AutoPtr<ICalendar> mCalendar;

    AutoPtr<INumberFormat> mNumberFormat;
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_DATEFORMAT_H__
