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

#ifndef __ELASTOS_TEXT_SIMPLEDATEFORMAT_H__
#define __ELASTOS_TEXT_SIMPLEDATEFORMAT_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "DateFormat.h"
#include <elastos/utility/etl/List.h>
#include "elastos/core/StringBuffer.h"
#include "elastos/core/Character.h"

using Elastos::Core::IArrayOf;
using Elastos::Core::StringBuffer;
using Elastos::Core::Character;
using Elastos::Core::INumber;
using Elastos::Text::IAttributedCharacterIterator;;
using Elastos::Text::IFieldPosition;
using Elastos::Text::IParsePosition;
using Elastos::Utility::Etl::List;


namespace Elastos {
namespace Text {

class SimpleDateFormat
    : public DateFormat
    , public ISimpleDateFormat
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    virtual ~SimpleDateFormat();

    CARAPI constructor(
        /* [in] */ const String& pattern);

    CARAPI constructor(
        /* [in] */ const String& tem,
        /* [in] */ IDateFormatSymbols* value);

    CARAPI constructor(
        /* [in] */ const String& tem,
        /* [in] */ ILocale* locale);

    CARAPI constructor(
        /* [in] */ ILocale* locale);

    virtual CARAPI ApplyLocalizedPattern(
        /* [in] */ const String& tem);

    virtual CARAPI ApplyPattern(
        /* [in] */ const String& tem);

    //@Override
    CARAPI FormatToCharacterIterator(
        /* [in] */ IInterface* object,
        /* [out] */ IAttributedCharacterIterator** charactorIterator);

    //@Override
    CARAPI Format(
        /* [in] */ IDate* date,
        /* [in] */ IStringBuffer * buffer,
        /* [in] */ IFieldPosition* fieldPos);

    virtual CARAPI Get2DigitYearStart(
        /* [out] */ IDate** start);

    virtual CARAPI GetDateFormatSymbols(
        /* [out] */ IDateFormatSymbols** symbols);

    using DateFormat::Parse;

    //@Override
    CARAPI Parse(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ IDate** date);

    virtual CARAPI Set2DigitYearStart(
        /* [in] */ IDate* date);

    virtual CARAPI SetDateFormatSymbols(
        /* [in] */ IDateFormatSymbols* value);

    virtual CARAPI ToLocalizedPattern(
        /* [out] */ String* pattern);

    virtual CARAPI ToPattern(
        /* [out] */ String* pattern);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    CARAPI Clone(
        /* [out] */ IInterface** outface);

    CARAPI CloneImpl(
        /* [in] */ ISimpleDateFormat* object);

private:
    CARAPI ValidatePatternCharacter(
        /* [in] */ Char32 format);

    CARAPI ValidatePattern(
        /* [in] */ const String& tem);

    static CARAPI DefaultPattern(
        /* [out] */ String* pattern);

    CARAPI FormatToCharacterIteratorImpl(
        /* [in] */ IDate* date,
        /* [out] */ IAttributedCharacterIterator** charactorIterator);

    CARAPI FormatImpl(
        /* [in] */ IDate* date,
        /* [in] */ IStringBuffer* buffer,
        /* [in] */ IFieldPosition* field,
        /* [in] */ List< AutoPtr<IFieldPosition> >* fields);

    CARAPI Append(
        /* [in] */ IStringBuffer* buffer,
        /* [in] */ IFieldPosition* position,
        /* [in] */ List< AutoPtr<IFieldPosition> >* fields,
        /* [in] */ Char32 format,
        /* [in] */ Int32 count);

    CARAPI AppendDayOfWeek(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ Int32 count,
        /* [in] */ Boolean standAlone);

    CARAPI AppendMonth(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ Int32 count,
        /* [in] */ Boolean standAlone);

    CARAPI AppendTimeZone(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ Int32 count,
        /* [in] */ Boolean generalTimeZone);

    CARAPI AppendNumericTimeZone(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ Int32 count,
        /* [in] */ Boolean generalTimeZone);

    CARAPI AppendMilliseconds(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ Int32 count,
        /* [in] */ Int32 value);

    CARAPI AppendNumber(
        /* [in] */ StringBuffer* buffer,
        /* [in] */ Int32 count,
        /* [in] */ Int32 value);

    CARAPI Error(
        /* [in] */ IParsePosition* position,
        /* [in] */ Int32 offset,
        /* [in] */ ITimeZone* zone,
        /* [out] */ IDate** date);

    CARAPI Parse(
        /* [in] */ const String& string,
        /* [in] */ Int32 offset,
        /* [in] */ Char32 format,
        /* [in] */ Int32 count,
        /* [out] */ Int32* value);

    CARAPI ParseDayOfWeek(
        /* [in] */ const String& string,
        /* [in] */ Int32 offset,
        /* [in] */ Boolean standAlone,
        /* [out] */ Int32* value);

    CARAPI ParseMonth(
        /* [in] */ const String& string,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [in] */ Int32 absolute,
        /* [in] */ Boolean standAlone,
        /* [out] */ Int32* value);

    CARAPI ParseNumber(
        /* [in] */ Int32 max,
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ INumber** number);

    CARAPI ParseNumber(
        /* [in] */ Int32 max,
        /* [in] */ const String& string,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 field,
        /* [in] */ Int32 skew,
        /* [out] */ Int32* value);

    CARAPI ParseText(
        /* [in] */ const String& string,
        /* [in] */ Int32 offset,
        /* [in] */ ArrayOf<String>* text,
        /* [in] */ Int32 field,
        /* [out] */ Int32* value);

    CARAPI ParseTimeZone(
        /* [in] */ const String& string,
        /* [in] */ Int32 offset,
        /* [out] */ Int32* value);

    CARAPI ParseFractionalSeconds(
        /* [in] */ const String& buffer,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 count,
        /* [out] */ Int32* result);

    static CARAPI ConvertPattern(
        /* [in] */ const String& tem,
        /* [in] */ const String& fromChars,
        /* [in] */ const String& toChars,
        /* [in] */ Boolean check,
        /* [out] */ String* pattern);

    //CARAPI WriteObject(ObjectOutputStream stream);

    //CARAPI ReadObject(ObjectInputStream stream);

    static AutoPtr<ArrayOf<String> > IArrayofToStringArray(
        /* [in] */ IArrayOf * arrayOf);

protected:
    const static String PATTERN_CHARS;

private:
    //const static Int64 serialVersionUID = 4774881970558875024L;

    const static Int32 RFC_822_TIMEZONE_FIELD;

    const static Int32 STAND_ALONE_MONTH_FIELD;

    const static Int32 STAND_ALONE_DAY_OF_WEEK_FIELD;

    String mPattern;

    AutoPtr<IDateFormatSymbols> mFormatData;

    mutable Int32 mCreationYear;

    AutoPtr<IDate> mDefaultCenturyStart;

//    private static final ObjectStreamField[] serialPersistentFields = {
//            new ObjectStreamField("defaultCenturyStart", Date.class),
//            new ObjectStreamField("formatData", DateFormatSymbols.class),
//            new ObjectStreamField("pattern", String.class),
//            new ObjectStreamField("serialVersionOnStream", int.class), };
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_SIMPLEDATEFORMAT_H__
