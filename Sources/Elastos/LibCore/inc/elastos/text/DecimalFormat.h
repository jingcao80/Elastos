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

#ifndef __ELASTOS_TEXT_DECIMALFORMAT__
#define __ELASTOS_TEXT_DECIMALFORMAT__

#include "Elastos.CoreLibrary.Libcore.h"
#include "NumberFormat.h"

using Libcore::ICU::INativeDecimalFormat;
using Elastos::Text::IAttributedCharacterIterator;;
using Elastos::Text::IDecimalFormatSymbols;
using Elastos::Text::IFieldPosition;
using Elastos::Text::IParsePosition;
using Elastos::Utility::ICurrency;
using Elastos::Core::IDouble;

namespace Elastos {
namespace Text {

class DecimalFormat
    : public NumberFormat
    , public IDecimalFormat
{
public:
    CAR_INTERFACE_DECL()

    DecimalFormat();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& pattern);

    CARAPI constructor(
        /* [in] */ const String& pattern,
        /* [in] */ IDecimalFormatSymbols* value);

    CARAPI constructor(
        /* [in] */ const String& pattern,
        /* [in] */ ILocale* locale);

    CARAPI ApplyLocalizedPattern(
        /* [in] */ const String& pattern);

    CARAPI ApplyPattern(
        /* [in] */ const String& pattern);

    //@Override
    CARAPI Clone(
        /* [out] */ IInterface** outobj);

    CARAPI CloneImpl(
        /* [in] */ IDecimalFormat* outobj);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI FormatToCharacterIterator(
        /* [in] */ IInterface* object,
        /* [out] */ IAttributedCharacterIterator** characterIterator);

    //@Override
    CARAPI Format(
        /* [in] */ Double value,
        /* [in] */ IStringBuffer * buffer,
        /* [in] */ IFieldPosition* field);

    //@Override
    CARAPI Format(
        /* [in] */ Int64 value,
        /* [in] */ IStringBuffer * buffer,
        /* [in] */ IFieldPosition* field);

    //@Override
    CARAPI Format(
        /* [in] */ IInterface* object,
        /* [in] */ IStringBuffer * buffer,
        /* [in] */ IFieldPosition* field);

    CARAPI GetDecimalFormatSymbols(
        /* [out] */ IDecimalFormatSymbols** symbols);

    // @Override
    CARAPI GetCurrency(
        /* [out] */ ICurrency** currency);

    CARAPI GetGroupingSize(
        /* [out] */ Int32* size);

    CARAPI GetMultiplier(
        /* [out] */ Int32* multiplier);

    CARAPI GetNegativePrefix(
        /* [out] */ String* prefix);

    CARAPI GetNegativeSuffix(
        /* [out] */ String* suffix);

    CARAPI GetPositivePrefix(
        /* [out] */ String* prefix);

    CARAPI GetPositiveSuffix(
        /* [out] */ String* suffix);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI IsParseBigDecimal(
        /* [out] */ Boolean* isBigDecimal);

    CARAPI IsDecimalSeparatorAlwaysShown(
        /* [out] */ Boolean* alwaysShown);

    //@Override
    CARAPI SetParseIntegerOnly(
        /* [in] */ Boolean value);

    //@Override
    CARAPI IsParseIntegerOnly(
        /* [out] */ Boolean* isParseIntegerOnly);

    //@Override
    CARAPI Parse(
        /* [in] */ const String& string,
        /* [in] */ IParsePosition* position,
        /* [out] */ INumber** value);

    CARAPI SetDecimalFormatSymbols(
        /* [in] */ IDecimalFormatSymbols* value);

    //@Override
    CARAPI SetCurrency(
        /* [in] */ ICurrency* currency);

    CARAPI SetDecimalSeparatorAlwaysShown(
        /* [in] */ Boolean value);

    CARAPI SetGroupingSize(
        /* [in] */ Int32 value);

    //@Override
    CARAPI SetGroupingUsed(
        /* [in] */ Boolean value);

    //@Override
    CARAPI IsGroupingUsed(
        /* [out] */ Boolean* isGroupingUsed);

    //@Override
    CARAPI SetMaximumFractionDigits(
        /* [in] */ Int32 value);

    //@Override
    CARAPI SetMaximumIntegerDigits(
        /* [in] */ Int32 value);

    //@Override
    CARAPI SetMinimumFractionDigits(
        /* [in] */ Int32 value);

    //@Override
    CARAPI SetMinimumIntegerDigits(
        /* [in] */ Int32 value);

    CARAPI SetMultiplier(
        /* [in] */ Int32 value);

    CARAPI SetNegativePrefix(
        /* [in] */ const String& value);

    CARAPI SetNegativeSuffix(
        /* [in] */ const String& value);

    CARAPI SetPositivePrefix(
        /* [in] */ const String& value);

    CARAPI SetPositiveSuffix(
        /* [in] */ const String& value);

    CARAPI SetParseBigDecimal(
        /* [in] */ Boolean newValue);

    CARAPI ToLocalizedPattern(
        /* [out] */ String* pattern);

    CARAPI ToPattern(
        /* [out] */ String* pattern);

    //override
    CARAPI GetRoundingMode(
        /* [out] */ RoundingMode* mode);

    //override
    CARAPI SetRoundingMode(
        /* [in] */ RoundingMode mode);

    CARAPI GetNdf(
        /* [out] */ INativeDecimalFormat** ndf);

    CARAPI ToString(
        /* [out] */ String* str);

private:

    CARAPI InitNative(
        /* [in] */ const String& pattern);

    CARAPI UpdateFieldsFromNative();

    CARAPI CheckBufferAndFieldPosition(
        /* [in] */ IStringBuffer * buffer,
        /* [in] */ IFieldPosition* position);

    // @SuppressWarnings("nls")
    // private void writeObject(ObjectOutputStream stream);


    // private void readObject(ObjectInputStream stream);

private:

    mutable AutoPtr<IDecimalFormatSymbols> mSymbols;

    mutable AutoPtr<INativeDecimalFormat> mNdf;

    mutable RoundingMode mRoundingMode;

    static const AutoPtr<IDouble> NEGATIVE_ZERO_DOUBLE;

/*
    private static final ObjectStreamField[] serialPersistentFields = {
            new ObjectStreamField("positivePrefix", String.class),
            new ObjectStreamField("positiveSuffix", String.class),
            new ObjectStreamField("negativePrefix", String.class),
            new ObjectStreamField("negativeSuffix", String.class),
            new ObjectStreamField("posPrefixPattern", String.class),
            new ObjectStreamField("posSuffixPattern", String.class),
            new ObjectStreamField("negPrefixPattern", String.class),
            new ObjectStreamField("negSuffixPattern", String.class),
            new ObjectStreamField("multiplier", int.class),
            new ObjectStreamField("groupingSize", byte.class),
            new ObjectStreamField("groupingUsed", boolean.class),
            new ObjectStreamField("decimalSeparatorAlwaysShown", boolean.class),
            new ObjectStreamField("parseBigDecimal", boolean.class),
            new ObjectStreamField("roundingMode", RoundingMode.class),
            new ObjectStreamField("symbols", DecimalFormatSymbols.class),
            new ObjectStreamField("useExponentialNotation", boolean.class),
            new ObjectStreamField("minExponentDigits", byte.class),
            new ObjectStreamField("maximumIntegerDigits", int.class),
            new ObjectStreamField("minimumIntegerDigits", int.class),
            new ObjectStreamField("maximumFractionDigits", int.class),
            new ObjectStreamField("minimumFractionDigits", int.class),
            new ObjectStreamField("serialVersionOnStream", int.class), };
*/
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_DECIMALFORMAT__
