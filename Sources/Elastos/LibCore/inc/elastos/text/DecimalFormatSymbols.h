#ifndef __ELASTOS_TEXT_DECIMALFORMATSYMBOLS_H__
#define __ELASTOS_TEXT_DECIMALFORMATSYMBOLS_H__

#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Object.h"

using Elastos::Core::ICloneable;
using Elastos::Utility::ILocale;
using Elastos::Utility::ICurrency;

namespace Elastos {
namespace Text {

class DecimalFormatSymbols
    : public Object
    , public IDecimalFormatSymbols
    , public ICloneable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ILocale* locale);

    static CARAPI GetInstance(
        /* [out] */ IDecimalFormatSymbols** instance);

    static CARAPI GetInstance(
        /* [in] */ ILocale* locale,
        /* [out] */ IDecimalFormatSymbols** instance);

    static CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI Clone(
        /* [out] */ IInterface** object);

    CARAPI CloneImpl(
        /* [out] */ IDecimalFormatSymbols* object);

    CARAPI Equals(
        /* [in] */ IInterface* object,
        /* [out] */ Boolean* res);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetCurrency(
        /* [out] */ ICurrency** currency);

    CARAPI GetInternationalCurrencySymbol(
        /* [out] */ String* symbol);

    CARAPI GetCurrencySymbol(
        /* [out] */ String* symbol);

    CARAPI GetDecimalSeparator(
        /* [out] */ Char32* separator);

    CARAPI GetDigit(
        /* [out] */ Char32* digit);

    CARAPI GetGroupingSeparator(
        /* [out] */ Char32* separator);

    CARAPI GetInfinity(
        /* [out] */ String* infinity);

    CARAPI GetMinusSign(
        /* [out] */ Char32* minusSign);

    CARAPI GetMonetaryDecimalSeparator(
        /* [out] */ Char32* separator);

    CARAPI GetNaN(
            /* [out] */ String* nan);

    CARAPI GetPatternSeparator(
        /* [out] */ Char32* separator);

    CARAPI GetPercent(
            /* [out] */ Char32* percent);

    CARAPI GetPerMill(
        /* [out] */ Char32* perMill);

    CARAPI GetZeroDigit(
        /* [out] */ Char32* zeroDigit);

    CARAPI GetExponentSeparator(
        /* [out] */ String* separator);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI SetCurrency(
        /* [in] */ ICurrency* currency);

    CARAPI SetInternationalCurrencySymbol(
        /* [in] */ const String& value);

    CARAPI SetCurrencySymbol(
        /* [in] */ const String& value);

    CARAPI SetDecimalSeparator(
        /* [in] */ Char32 value);

    CARAPI SetDigit(
        /* [in] */ Char32 value);

    CARAPI SetGroupingSeparator(
            /* [in] */ Char32 value);

    CARAPI SetInfinity(
        /* [in] */ const String& value);

    CARAPI SetMinusSign(
        /* [in] */ Char32 value);

    CARAPI SetMonetaryDecimalSeparator(
        /* [in] */ Char32 value);

    CARAPI SetNaN(
        /* [in] */ const String& value);

    CARAPI SetPatternSeparator(
        /* [in] */ Char32 value);

    CARAPI SetPercent(
        /* [in] */ Char32 value);

    CARAPI SetPerMill(
        /* [in] */ Char32 value);

    CARAPI SetZeroDigit(
        /* [in] */ Char32 value);

    CARAPI SetExponentSeparator(
        /* [in] */ const String& value);

    CARAPI GetMinusSignString(
        /* [out] */ String* rst);

/*    private static ObjectStreamField[] serialPersistentFields = {
        new ObjectStreamField("currencySymbol", String.class),
        new ObjectStreamField("decimalSeparator", char.class),
        new ObjectStreamField("digit", char.class),
        new ObjectStreamField("exponential", char.class),
        new ObjectStreamField("exponentialSeparator", String.class),
        new ObjectStreamField("groupingSeparator", char.class),
        new ObjectStreamField("infinity", String.class),
        new ObjectStreamField("intlCurrencySymbol", String.class),
        new ObjectStreamField("minusSign", char.class),
        new ObjectStreamField("monetarySeparator", Char.class),
        new ObjectStreamField("NaN", String.class),
        new ObjectStreamField("patternSeparator", char.class),
        new ObjectStreamField("percent", char.class),
        new ObjectStreamField("perMill", char.class),
        new ObjectStreamField("serialVersionOnStream", int.class),
        new ObjectStreamField("zeroDigit", char.class),
        new ObjectStreamField("locale", Locale.class),
    };*/

    // private void writeObject(ObjectOutputStream stream);

    // private void readObject(ObjectInputStream stream);

private:
    Char32 mZeroDigit;
    Char32 mDigit;
    Char32 mDecimalSeparator;
    Char32 mGroupingSeparator;
    Char32 mPatternSeparator;
    Char32 mPercent;
    Char32 mPerMill;
    Char32 mMonetarySeparator;
    String mMinusSign;
    String mInfinity, mNaN, mCurrencySymbol, mIntlCurrencySymbol;

    mutable AutoPtr<ICurrency> mCurrency;
    mutable AutoPtr<ILocale> mLocale;
    mutable String mExponentSeparator;
};

} // namespace Text
} // namespace Elastos

#endif //__ELASTOS_TEXT_DECIMALFORMATSYMBOLS_H__
