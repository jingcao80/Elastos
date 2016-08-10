
#ifndef __LIBCORE_ICU_ICU_ICUUTIL_H__
#define __LIBCORE_ICU_ICU_ICUUTIL_H__

#include "LocaleData.h"

using Elastos::Utility::ILocale;
using Libcore::Utility::IBasicLruCache;

namespace Libcore {
namespace ICU {

class ICUUtil
{
private:
    ICUUtil();
    ICUUtil(const ICUUtil&);

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
    static CARAPI LocaleFromIcuLocaleId(
        /* [in] */ const String& localeId,
        /* [out, callee] */ ILocale** locale);

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
        /* [in] */ ILocale* locale);

    static CARAPI_(String) ToUpperCase(
        /* [in] */ const String& s,
        /* [in] */ ILocale* locale);

    // // Just the subset of error codes needed by CharsetDecoderICU/CharsetEncoderICU.
    // static const Int32 U_ZERO_ERROR;
    // static const Int32 U_INVALID_CHAR_FOUND;
    // static const Int32 U_TRUNCATED_CHAR_FOUND;
    // static const Int32 U_ILLEGAL_CHAR_FOUND;
    // static const Int32 U_BUFFER_OVERFLOW_ERROR;

    // static CARAPI_(Boolean) U_FAILURE(
    //     /* [in] */ Int32 error);

    static CARAPI GetAvailableCurrencyCodes(
        /* [out, callee] */ ArrayOf<String>** codes);

    static CARAPI_(String) GetCurrencyCode(
        /* [in] */ const String& locale);

    static CARAPI GetCurrencyDisplayName(
        /* [in] */ ILocale* locale,
        /* [in] */ const String& currencyCode,
        /* [out] */ String* displayName);

    static CARAPI_(String) GetCurrencyDisplayName(
        /* [in] */ const String& locale,
        /* [in] */ const String& currencyCode);

    static CARAPI_(Int32) GetCurrencyFractionDigits(
        /* [in] */ const String& currencyCode);

    static CARAPI_(Int32) GetCurrencyNumericCode(
        /* [in] */ const String& currencyCode);

    static CARAPI GetCurrencySymbol(
        /* [in] */ ILocale* locale,
        /* [in] */ const String& currencyCode,
        /* [out] */ String* currencySymbol);

    static CARAPI_(String) GetCurrencySymbol(
        /* [in] */ const String& locale,
        /* [in] */ const String& currencyCode);

    static CARAPI GetDisplayCountry(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale,
        /* [out] */ String* displayCountry);

    static CARAPI_(String) GetDisplayCountryNative(
        /* [in] */ const String& targetLanguageTag,
        /* [in] */ const String& languageTag);

    static CARAPI GetDisplayLanguage(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale,
        /* [out] */ String* displayLanguage);

    static CARAPI_(String) GetDisplayLanguageNative(
        /* [in] */ const String& targetLanguageTag,
        /* [in] */ const String& languageTag);

    static CARAPI_(String) GetDisplayVariant(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale);

    static CARAPI_(String) GetDisplayVariantNative(
        /* [in] */ const String& targetLanguageTag,
        /* [in] */ const String& languageTag);

    static CARAPI_(String) GetDisplayScript(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale);

    static CARAPI_(String) GetDisplayScriptNative(
        /* [in] */ const String& targetLanguageTag,
        /* [in] */ const String& languageTag);

    static CARAPI_(String) GetISO3Country(
        /* [in] */ const String& languageTag);

    static CARAPI_(String) GetISO3Language(
        /* [in] */ const String& languageTag);

    static CARAPI_(AutoPtr<ILocale>) AddLikelySubtags(
        /* [in] */ ILocale* locale);

    static CARAPI_(String) AddLikelySubtags(
        /* [in] */ const String& locale);

    static CARAPI_(String) GetScript(
        /* [in] */ const String& locale);

    /* pacakge */
    static CARAPI_(Boolean) InitLocaleDataNative(
        /* [in] */ const String& locale,
        /* [in] */ LocaleData* result);

    static CARAPI SetDefaultLocale(
        /* [in] */ const String& languageTag);

    static CARAPI GetDefaultLocale(
        /* [out] */ String* defaultLocale);

    static CARAPI UTF16ByteArrayToUTF8ByteArray(
        /* [in] */ ArrayOf<UInt16>* utf16Array,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ ArrayOf<Byte>** utf8Array);

    static CARAPI UTF16ByteArrayToString(
        /* [in] */ ArrayOf<UInt16>* utf16Array,
        /* [in] */ Int32 offset,
        /* [in] */ Int32 length,
        /* [out] */ String* result);

private:
    static CARAPI_(String) ToLowerCase(
        /* [in] */ const String& s,
        /* [in] */ const String& languageTag);

    static CARAPI_(String) ToUpperCase(
        /* [in] */ const String& s,
        /* [in] */ const String& languageTag);

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

    /*
     * Parse the {Language, Script, Region, Variant*} section of the ICU locale
     * ID. This is the bit that appears before the keyword separate "@". The general
     * structure is a series of ASCII alphanumeric strings (subtags)
     * separated by underscores.
     *
     * Each subtag is interpreted according to its position in the list of subtags
     * AND its length (groan...). The various cases are explained in comments
     * below.
     */
    static CARAPI_(void) ParseLangScriptRegionAndVariants(
        /* [in] */ const String& string,
        /* [in] */ ArrayOf<String>* outputArray);

public:
    static CARAPI GetBestDateTimePattern(
        /* [in] */ const String& skeleton,
        /* [in] */ ILocale* locale,
        /* [out] */ String* rst);

private:
    static CARAPI GetBestDateTimePatternNative(
        /* [in] */ const String& skeleton,
        /* [in] */ const String& languageTag,
        /* [in] */ String* rev);

public:
    static CARAPI GetDateFormatOrder(
        /* [in] */ const String& pattern,
        /* [out, callee] */ ArrayOf<Char32>** locales);

    static CARAPI GetCldrVersion(
        /* [out] */ String* cldrVersion);

private:
    static const AutoPtr<IBasicLruCache> CACHED_PATTERNS;

    static AutoPtr<ArrayOf<String> > sIsoLanguages;

    static AutoPtr<ArrayOf<String> > sIsoCountries;

    static AutoPtr<ArrayOf<ILocale *> > sAvailableLocalesCache;

    static const Int32 IDX_LANGUAGE;
    static const Int32 IDX_SCRIPT;
    static const Int32 IDX_REGION;
    static const Int32 IDX_VARIANT;
};

} // namespace ICU
} // namespace Libcore

#endif //__LIBCORE_ICU_ICU_ICUUTIL_H__
