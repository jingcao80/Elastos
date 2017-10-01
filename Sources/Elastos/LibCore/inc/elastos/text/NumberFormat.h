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

#ifndef __ELASTOS_TEXT_NUMBERFORMAT_H__
#define __ELASTOS_TEXT_NUMBERFORMAT_H__

#include "FormatBase.h"

using Elastos::Utility::ILocale;
using Elastos::Text::IFieldPosition;
using Elastos::Text::IParsePosition;
using Elastos::Utility::ICurrency;
using Elastos::Core::INumber;
using Elastos::Core::IStringBuffer;
using Elastos::Math::RoundingMode;

namespace Elastos {
namespace Text {

class NumberFormat
    : public FormatBase
    , public INumberFormat
{
public:
    class Field
        : public FormatBase::Field
        , public INumberFormatField
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ const String& fn);

    public:
        const static AutoPtr<INumberFormatField> SIGN;
        const static AutoPtr<INumberFormatField> INTEGER;
        const static AutoPtr<INumberFormatField> FRACTION;
        const static AutoPtr<INumberFormatField> EXPONENT;
        const static AutoPtr<INumberFormatField> EXPONENT_SIGN;
        const static AutoPtr<INumberFormatField> EXPONENT_SYMBOL;
        const static AutoPtr<INumberFormatField> DECIMAL_SEPARATOR;
        const static AutoPtr<INumberFormatField> GROUPING_SEPARATOR;
        const static AutoPtr<INumberFormatField> PERCENT;
        const static AutoPtr<INumberFormatField> PERMILLE;
        const static AutoPtr<INumberFormatField> CURRENCY;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI CloneImpl(
        /* [in] */ INumberFormat* format);

    using FormatBase::Format;

    CARAPI Format(
        /* [in] */ Double value,
        /* [out] */ String* result);

    virtual CARAPI Format(
        /* [in] */ Double value,
        /* [in] */ IStringBuffer * inbuffer,
        /* [in] */ IFieldPosition * field) = 0;

    CARAPI Format(
        /* [in] */ Int64 value,
        /* [out] */ String* result);

    virtual CARAPI Format(
        /* [in] */ Int64 value,
        /* [in] */ IStringBuffer * inbuffer,
        /* [in] */ IFieldPosition * field) = 0;

    //@Override
    CARAPI Format(
        /* [in] */ IInterface* object,
        /* [in] */ IStringBuffer * buffer,
        /* [in] */ IFieldPosition* field);

    static CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetCurrency(
        /* [out] */ ICurrency** currency);

    static CARAPI GetCurrencyInstance(
        /* [out] */ INumberFormat** instance);

    static CARAPI GetCurrencyInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    static CARAPI GetIntegerInstance(
        /* [out] */ INumberFormat** instance);

    static CARAPI GetIntegerInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    static CARAPI GetInstance(
        /* [out] */ INumberFormat** instance);

    static CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    CARAPI GetMaximumFractionDigits(
        /* [out] */ Int32* number);

    CARAPI GetMaximumIntegerDigits(
        /* [out] */ Int32* number);

    CARAPI GetMinimumFractionDigits(
        /* [out] */ Int32* number);

    CARAPI GetMinimumIntegerDigits(
        /* [out] */ Int32* number);

    static CARAPI GetNumberInstance(
        /* [out] */ INumberFormat** instance);

    static CARAPI GetNumberInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

    static CARAPI GetPercentInstance(
        /* [out] */ INumberFormat** instance);

    static CARAPI GetPercentInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

//    @Override
    CARAPI GetHashCode(
        /* [out] */ Int32 * hash);

    CARAPI IsGroupingUsed(
        /* [out] */ Boolean* isGroupingUsed);

    CARAPI IsParseIntegerOnly(
        /* [out] */ Boolean* isParseIntegerOnly);

    CARAPI Parse(
        /* [in] */ const String& string,
        /* [out] */ INumber** value);

    virtual CARAPI Parse(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ INumber** value) = 0;

    //@Override
    CARAPI ParseObject(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ IInterface** object);

    CARAPI SetCurrency(
        /* [in] */ ICurrency* currency);

    CARAPI SetGroupingUsed(
        /* [in] */ Boolean value);

    CARAPI SetMaximumFractionDigits(
        /* [in] */ Int32 value);

    CARAPI SetMaximumIntegerDigits(
        /* [in] */ Int32 value);

    CARAPI SetMinimumFractionDigits(
        /* [in] */ Int32 value);

    CARAPI SetMinimumIntegerDigits(
        /* [in] */ Int32 value);

    CARAPI SetParseIntegerOnly(
        /* [in] */ Boolean value);

    CARAPI GetRoundingMode(
        /* [out] */ RoundingMode* roundingMode);

    CARAPI SetRoundingMode(
        /* [in] */ RoundingMode roundingMode);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

protected:
    NumberFormat();

private:
    static CARAPI GetInstance(
        /* [in] */ const String& pattern,
        /* [in] */ ILocale* locale,
        /* [out] */ INumberFormat** instance);

//    private static final ObjectStreamField[] serialPersistentFields = {
//            new ObjectStreamField("groupingUsed", Boolean.class),
//            new ObjectStreamField("maxFractionDigits", Byte.class),
//            new ObjectStreamField("maximumFractionDigits", Integer.class),
//            new ObjectStreamField("maximumIntegerDigits", Integer.class),
//            new ObjectStreamField("maxIntegerDigits", Byte.class),
//            new ObjectStreamField("minFractionDigits", Byte.class),
//            new ObjectStreamField("minimumFractionDigits", Integer.class),
//            new ObjectStreamField("minimumIntegerDigits", Integer.class),
//            new ObjectStreamField("minIntegerDigits", Byte.class),
//            new ObjectStreamField("parseIntegerOnly", Boolean.class),
//            new ObjectStreamField("serialVersionOnStream", Integer.class),
//    };

//    private void writeObject(ObjectOutputStream stream);

//    private void readObject(ObjectInputStream stream);

protected:
    Boolean mGroupingUsed;
    Boolean mParseIntegerOnly;

    Int32 mMaximumIntegerDigits;
    Int32 mMinimumIntegerDigits;
    Int32 mMaximumFractionDigits;
    Int32 mMinimumFractionDigits;
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_NUMBERFORMAT_H__
