
#ifndef __ELASTOS_UTILITY_ICU_H__
#define __ELASTOS_UTILITY_ICU_H__

#include "Elastos.Utility_server.h"
#include "LocaleData.h"

namespace Elastos {
namespace Utility {

class ICU
{
public:
    /**
     * Returns an array of ISO language names (two-letter codes), fetched either
     * from ICU's database or from our memory cache.
     *
     * @return The array.
     */
    static CARAPI GetISOLanguages(
        /* [out, callee] */ ArrayOf<String>** languages);

    /**
     * Returns an array of ISO country names (two-letter codes), fetched either
     * from ICU's database or from our memory cache.
     *
     * @return The array.
     */
    static CARAPI GetISOCountries(
        /* [out, callee] */ ArrayOf<String>** countries);

    /**
     * Returns the appropriate {@code Locale} given a {@code String} of the form returned
     * by {@code toString}. This is very lenient, and doesn't care what's between the underscores:
     * this method can parse strings that {@code Locale.toString} won't produce.
     * Used to remove duplication.
     */
    static CARAPI LocaleFromString(
        /* [in] */ const String& localeName,
        /* [out] */ ILocale** locale);

    static CARAPI LocalesFromStrings(
        /* [in] */ const ArrayOf<String>& localeNames,
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    static CARAPI GetAvailableLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    static CARAPI GetAvailableBreakIteratorLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    static CARAPI GetAvailableCalendarLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    static CARAPI GetAvailableCollatorLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    static CARAPI GetAvailableDateFormatLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    static CARAPI GetAvailableDateFormatSymbolsLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    static CARAPI GetAvailableDecimalFormatSymbolsLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    static CARAPI GetAvailableNumberFormatLocales(
        /* [out, callee] */ ArrayOf<ILocale*>** locales);

    /**
     * Returns the ICU version in use. This is "4.4" for gingerbread, for example.
     */
    static CARAPI_(String) GetIcuVersion();

    /**
     * Returns the Unicode version our ICU supports. This is "5.2" for gingerbread, for example.
     */
    static CARAPI_(String) GetUnicodeVersion();

    static CARAPI_(String) ToLowerCase(
        /* [in] */ const String& s,
        /* [in] */ const String& localname);

    static CARAPI_(String) ToUpperCase(
        /* [in] */ const String& s,
        /* [in] */ const String& localname);

    static CARAPI GetAvailableCurrencyCodes(
        /* [out, callee] */ ArrayOf<String>** codes);

    static CARAPI_(String) GetCurrencyCode(
        /* [in] */ const String& locale);

    static CARAPI_(String) GetCurrencyDisplayName(
        /* [in] */ const String& locale,
        /* [in] */ const String& currencyCode);

    static CARAPI_(Int32) GetCurrencyFractionDigits(
        /* [in] */ const String& currencyCode);

    static CARAPI_(String) GetCurrencySymbol(
        /* [in] */ const String& locale,
        /* [in] */ const String& currencyCode);

    static CARAPI_(String) GetDisplayCountry(
        /* [in] */ const String& countryCode,
        /* [in] */ const String& locale);

    static CARAPI_(String) GetDisplayLanguage(
        /* [in] */ const String& languageCode,
        /* [in] */ const String& locale);

    static CARAPI_(String) GetDisplayVariant(
        /* [in] */ const String& variantCode,
        /* [in] */ const String& locale);

    static CARAPI_(String) GetISO3Country(
        /* [in] */ const String& locale);

    static CARAPI_(String) GetISO3Language(
        /* [in] */ const String& locale);

    static CARAPI_(String) AddLikelySubtags(
        /* [in] */ const String& locale);

    static CARAPI_(String) GetScript(
        /* [in] */ const String& locale);

    /* pacakge */
    static CARAPI_(Boolean) InitLocaleDataImpl(
        /* [in] */ const String& locale,
        /* [in] */ LocaleData* result);

private:
    static CARAPI GetAvailableBreakIteratorLocalesNative(
        /* [out, callee] */ ArrayOf<String>** locales);

    static CARAPI GetAvailableCalendarLocalesNative(
        /* [out, callee] */ ArrayOf<String>** locales);

    static CARAPI GetAvailableCollatorLocalesNative(
        /* [out, callee] */ ArrayOf<String>** locales);

    static CARAPI GetAvailableDateFormatLocalesNative(
        /* [out, callee] */ ArrayOf<String>** locales);

    static CARAPI GetAvailableLocalesNative(
        /* [out, callee] */ ArrayOf<String>** locales);

    static CARAPI GetAvailableNumberFormatLocalesNative(
        /* [out, callee] */ ArrayOf<String>** locales);

    static CARAPI_(AutoPtr<ArrayOf<String> >) GetISOLanguagesNative();

    static CARAPI_(AutoPtr<ArrayOf<String> >) GetISOCountriesNative();

private:
    /**
     * Cache for ISO language names.
     */
    static AutoPtr<ArrayOf<String> > sIsoLanguages;

    /**
     * Cache for ISO country names.
     */
    static AutoPtr<ArrayOf<String> > sIsoCountries;

    static AutoPtr<ArrayOf<ILocale *> > sAvailableLocalesCache;
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_ICU_H__
