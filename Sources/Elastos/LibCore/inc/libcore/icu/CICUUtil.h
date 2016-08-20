
#ifndef __LIBCORE_ICU_CICUUTIL_H__
#define __LIBCORE_ICU_CICUUTIL_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Singleton.h"
#include "_Libcore_ICU_CICUUtil.h"

using Elastos::Core::Singleton;
using Elastos::Utility::ILocale;

namespace Libcore {
namespace ICU {

CarClass(CICUUtil)
    , public Singleton
    , public IICUUtil
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetISOLanguages(
        /* [out, callee] */ ArrayOf<String>** languages);

    CARAPI GetISOCountries(
        /* [out, callee] */ ArrayOf<String>** countries);

    CARAPI LocalesFromStrings(
        /* [in] */ ArrayOf<String>* localeNames,
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI LocaleFromIcuLocaleId(
        /* [in] */ const String& localeName,
        /* [out, callee] */ ILocale** locale);

    CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetAvailableBreakIteratorLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetAvailableCalendarLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetAvailableCollatorLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetAvailableDateFormatLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetAvailableDateFormatSymbolsLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetAvailableDecimalFormatSymbolsLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetAvailableNumberFormatLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    CARAPI GetCldrVersion(
        /* [out] */ String* cldrVersion);

    /**
     * Returns the icu4c version in use, such as "50.1.1".
     */
    CARAPI GetIcuVersion(
        /* [out] */ String* icuVersion);

    /**
     * Returns the Unicode version our ICU supports, such as "6.2".
     */
    CARAPI GetUnicodeVersion(
        /* [out] */ String* unicodeVersion);

    CARAPI ToLowerCase(
        /* [in] */ const String& s,
        /* [in] */ ILocale* locale,
        /* [out] */ String* ls);

    CARAPI ToUpperCase(
        /* [in] */ const String& s,
        /* [in] */ ILocale* locale,
        /* [out] */ String* us);

    CARAPI GetAvailableCurrencyCodes(
        /* [out, callee] */ ArrayOf<String>** codes);

    CARAPI GetCurrencyCode(
        /* [in] */ const String& locale,
        /* [out] */ String* currencyCode);

    CARAPI GetCurrencyDisplayName(
        /* [in] */ ILocale* locale,
        /* [in] */ const String& currencyCode,
        /* [out] */ String* displayName);

    CARAPI GetCurrencyFractionDigits(
        /* [in] */ const String& currencyCode,
        /* [out] */ Int32* currencyFractionDigits);

    CARAPI GetCurrencyNumericCode(
        /* [in] */ const String& currencyCode,
        /* [out] */ Int32* currencyFractionDigits);

    CARAPI GetCurrencySymbol(
        /* [in] */ ILocale* locale,
        /* [in] */ const String& currencyCode,
        /* [out] */ String* currencySymbol);

    CARAPI GetDisplayCountry(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale,
        /* [out] */ String* displayCountry);

    CARAPI GetDisplayLanguage(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale,
        /* [out] */ String* displayLanguage);

    CARAPI GetDisplayVariant(
        /* [in] */ ILocale* variant,
        /* [in] */ ILocale* locale,
        /* [out] */ String* displayVariant);

    CARAPI GetDisplayScript(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale,
        /* [out] */ String* script);

    CARAPI GetISO3Country(
        /* [in] */ const String& languageTag,
        /* [out] */ String* ISO3country);

    CARAPI GetISO3Language(
        /* [in] */ const String& languageTag,
        /* [out] */ String* ISO3Language);

    CARAPI AddLikelySubtags(
        /* [in] */ ILocale* locale,
        /* [out] */ ILocale** target);

    CARAPI AddLikelySubtags(
        /* [in] */ const String& locale,
        /* [out] */ String* result);

    CARAPI GetBestDateTimePattern(
        /* [in] */ const String& skeleton,
        /* [in] */ ILocale* locale,
        /* [out] */ String* rst);

    CARAPI GetDateFormatOrder(
        /* [in] */ const String& pattern,
        /* [out, callee] */ ArrayOf<Char32>** locales);

    CARAPI GetScript(
        /* [in] */ const String& locale,
        /* [out] */ String* script);

    CARAPI SetDefaultLocale(
        /* [in] */ const String& languageTag);

    CARAPI GetDefaultLocale(
        /* [out] */ String* defaultLocale);

    CARAPI UTF16ByteArrayToUTF8ByteArray(
        /* [in] */ ArrayOf<UInt16>* utf16Array,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out, callee] */ ArrayOf<Byte>** utf8Array);

    CARAPI UTF16ByteArrayToString(
        /* [in] */ ArrayOf<UInt16>* utf16Array,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* result);

    CARAPI UTF8ByteArrayToUTF16ByteArray(
        /* [in] */ ArrayOf<Byte>* utf8Array,
        /* [out, callee] */ ArrayOf<UInt16>** utf16Array);
};

} // namespace ICU
} // namespace Libcore

#endif // __LIBCORE_ICU_CICUUTILHELPER_H__
