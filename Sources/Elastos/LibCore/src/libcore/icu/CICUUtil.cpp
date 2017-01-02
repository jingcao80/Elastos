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

#include "CICUUtil.h"
#include "ICUUtil.h"

namespace Libcore {
namespace ICU {

CAR_SINGLETON_IMPL(CICUUtil)

CAR_INTERFACE_IMPL(CICUUtil, Singleton, IICUUtil)

ECode CICUUtil::GetISOLanguages(
    /* [out, callee] */ ArrayOf<String>** languages)
{
    VALIDATE_NOT_NULL(languages);

    return ICUUtil::GetISOLanguages(languages);
}

ECode CICUUtil::GetISOCountries(
    /* [out, callee] */ ArrayOf<String>** countries)
{
    VALIDATE_NOT_NULL(countries);

    return ICUUtil::GetISOCountries(countries);
}

ECode CICUUtil::LocaleFromIcuLocaleId(
    /* [in] */ const String& localeId,
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);

    return ICUUtil::LocaleFromIcuLocaleId(localeId, locale);
}

ECode CICUUtil::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);

    return ICUUtil::GetAvailableLocales(locales);
}

ECode CICUUtil::GetAvailableBreakIteratorLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);

    return ICUUtil::GetAvailableBreakIteratorLocales(locales);
}

ECode CICUUtil::GetAvailableCalendarLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);

    return ICUUtil::GetAvailableCalendarLocales(locales);
}

ECode CICUUtil::GetAvailableCollatorLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);

    return ICUUtil::GetAvailableCollatorLocales(locales);
}

ECode CICUUtil::GetAvailableDateFormatLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);

    return ICUUtil::GetAvailableDateFormatLocales(locales);
}

ECode CICUUtil::GetAvailableDateFormatSymbolsLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);

    return ICUUtil::GetAvailableDateFormatSymbolsLocales(locales);
}

ECode CICUUtil::GetAvailableDecimalFormatSymbolsLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);

    return ICUUtil::GetAvailableDateFormatSymbolsLocales(locales);
}

ECode CICUUtil::GetAvailableNumberFormatLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);

    return ICUUtil::GetAvailableNumberFormatLocales(locales);
}

ECode CICUUtil::GetBestDateTimePattern(
    /* [in] */ const String& skeleton,
    /* [in] */ ILocale* locale,
    /* [out] */ String* rst)
{
    VALIDATE_NOT_NULL(locale);
    VALIDATE_NOT_NULL(rst);

    return ICUUtil::GetBestDateTimePattern(skeleton, locale, rst);
}

ECode CICUUtil::GetIcuVersion(
    /* [out] */ String* icuVersion)
{
    VALIDATE_NOT_NULL(icuVersion);
    *icuVersion = ICUUtil::GetIcuVersion();
    return NOERROR;
}

ECode CICUUtil::GetUnicodeVersion(
    /* [out] */ String* unicodeVersion)
{
    VALIDATE_NOT_NULL(unicodeVersion);
    *unicodeVersion = ICUUtil::GetUnicodeVersion();
    return NOERROR;
}

ECode CICUUtil::GetAvailableCurrencyCodes(
    /* [out] */ ArrayOf<String>** codes)
{
    VALIDATE_NOT_NULL(codes);
    return ICUUtil::GetAvailableCurrencyCodes(codes);
}

ECode CICUUtil::GetCurrencyCode(
    /* [in] */ const String& locale,
    /* [out] */ String* currencyCode)
{
    VALIDATE_NOT_NULL(currencyCode);

    *currencyCode = ICUUtil::GetCurrencyCode(locale);
    return NOERROR;
}

ECode CICUUtil::GetCurrencyDisplayName(
    /* [in] */ ILocale* locale,
    /* [in] */ const String& currencyCode,
    /* [out] */ String* displayName)
{
    VALIDATE_NOT_NULL(locale);
    VALIDATE_NOT_NULL(displayName);

    return ICUUtil::GetCurrencyDisplayName(locale, currencyCode, displayName);
}

ECode CICUUtil::GetCurrencyFractionDigits(
    /* [in] */ const String& currencyCode,
    /* [out] */ Int32* currencyFractionDigits)
{
    VALIDATE_NOT_NULL(currencyFractionDigits);

    *currencyFractionDigits = ICUUtil::GetCurrencyFractionDigits(currencyCode);
    return NOERROR;
}

ECode CICUUtil::GetCurrencySymbol(
        /* [in] */ ILocale* locale,
        /* [in] */ const String& currencyCode,
        /* [out] */ String* currencySymbol)
{
    VALIDATE_NOT_NULL(locale);
    VALIDATE_NOT_NULL(currencySymbol);

    return ICUUtil::GetCurrencySymbol(locale, currencyCode, currencySymbol);
}

ECode CICUUtil::GetDisplayCountry(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale,
        /* [out] */ String* displayCountry)
{
    VALIDATE_NOT_NULL(targetLocale);
    VALIDATE_NOT_NULL(locale);
    VALIDATE_NOT_NULL(displayCountry);

    return ICUUtil::GetDisplayCountry(targetLocale, locale, displayCountry);
}

ECode CICUUtil::GetDisplayLanguage(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale,
        /* [out] */ String* displayLanguage)
{
    VALIDATE_NOT_NULL(targetLocale);
    VALIDATE_NOT_NULL(locale);
    VALIDATE_NOT_NULL(displayLanguage);

    return ICUUtil::GetDisplayLanguage(targetLocale, locale, displayLanguage);
}

ECode CICUUtil::GetDisplayVariant(
    /* [in] */ ILocale* variant,
    /* [in] */ ILocale* locale,
    /* [out] */ String* displayVariant)
{
    VALIDATE_NOT_NULL(variant)
    VALIDATE_NOT_NULL(locale)
    VALIDATE_NOT_NULL(displayVariant)

    *displayVariant = ICUUtil::GetDisplayVariant(variant, locale);
    return NOERROR;
}

ECode CICUUtil::GetDisplayScript(
    /* [in] */ ILocale* targetLocale,
    /* [in] */ ILocale* locale,
    /* [out] */ String* script)
{
    VALIDATE_NOT_NULL(targetLocale)
    VALIDATE_NOT_NULL(locale)
    VALIDATE_NOT_NULL(script)

    *script = ICUUtil::GetDisplayScript(targetLocale, locale);
    return NOERROR;
}

ECode CICUUtil::GetISO3Country(
    /* [in] */ const String& languageTag,
    /* [out] */ String* ISO3country)
{
    VALIDATE_NOT_NULL(ISO3country);

    *ISO3country = ICUUtil::GetISO3Country(languageTag);
    return NOERROR;
}

ECode CICUUtil::GetISO3Language(
    /* [in] */ const String& languageTag,
    /* [out] */ String* ISO3Language)
{
    VALIDATE_NOT_NULL(ISO3Language);

    *ISO3Language = ICUUtil::GetISO3Language(languageTag);
    return NOERROR;
}

ECode CICUUtil::AddLikelySubtags(
    /* [in] */ const String& locale,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)

    *result = ICUUtil::AddLikelySubtags(locale);
    return NOERROR;
}

ECode CICUUtil::AddLikelySubtags(
    /* [in] */ ILocale* locale,
    /* [out] */ ILocale** target)
{
    VALIDATE_NOT_NULL(locale)
    VALIDATE_NOT_NULL(target);

    AutoPtr<ILocale> l = ICUUtil::AddLikelySubtags(locale);
    *target = l;
    REFCOUNT_ADD(*target);
    return NOERROR;
}

ECode CICUUtil::GetDateFormatOrder(
    /* [in] */ const String& pattern,
    /* [out, callee] */ ArrayOf<Char32>** locales)
{
    return ICUUtil::GetDateFormatOrder(pattern, locales);
}

ECode CICUUtil::GetCldrVersion(
    /* [out] */ String* cldrVersion)
{
    return ICUUtil::GetCldrVersion(cldrVersion);
}

ECode CICUUtil::ToLowerCase(
        /* [in] */ const String& s,
        /* [in] */ ILocale* locale,
        /* [out] */ String* ls)
{
    VALIDATE_NOT_NULL(locale);
    VALIDATE_NOT_NULL(ls);

    return ToLowerCase(s, locale, ls);
}

ECode CICUUtil::ToUpperCase(
    /* [in] */ const String& s,
    /* [in] */ ILocale* locale,
    /* [out] */ String* us)
{
    VALIDATE_NOT_NULL(locale);
    VALIDATE_NOT_NULL(us);

    return ToUpperCase(s, locale, us);
}

ECode CICUUtil::GetCurrencyNumericCode(
    /* [in] */ const String& currencyCode,
    /* [out] */ Int32* currencyFractionDigits)
{
    VALIDATE_NOT_NULL(currencyFractionDigits)

    return GetCurrencyNumericCode(currencyCode, currencyFractionDigits);
}

ECode CICUUtil::GetScript(
    /* [in] */ const String& locale,
    /* [out] */ String* script)
{
    VALIDATE_NOT_NULL(script);
    *script = ICUUtil::GetScript(locale);
    return NOERROR;
}

ECode CICUUtil::SetDefaultLocale(
    /* [in] */ const String& languageTag)
{
    return ICUUtil::SetDefaultLocale(languageTag);
}

ECode CICUUtil::GetDefaultLocale(
    /* [out] */ String* defaultLocale)
{
    VALIDATE_NOT_NULL(defaultLocale)

    return ICUUtil::GetDefaultLocale(defaultLocale);
}

ECode CICUUtil::LocalesFromStrings(
    /* [in] */ ArrayOf<String>* localeNames,
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(localeNames)
    VALIDATE_NOT_NULL(locales)

    return ICUUtil::LocalesFromStrings(*localeNames, locales);
}

ECode CICUUtil::UTF16ByteArrayToUTF8ByteArray(
    /* [in] */ ArrayOf<UInt16>* utf16Array,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out, callee] */ ArrayOf<Byte>** utf8Array)
{
    return ICUUtil::UTF16ByteArrayToUTF8ByteArray(utf16Array, offset, length, utf8Array);
}

ECode CICUUtil::UTF16ByteArrayToString(
    /* [in] */ ArrayOf<UInt16>* utf16Array,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* result)
{
    return ICUUtil::UTF16ByteArrayToString(utf16Array, offset, length, result);
}

ECode CICUUtil::UTF8ByteArrayToUTF16ByteArray(
    /* [in] */ ArrayOf<Byte>* utf8Array,
    /* [out, callee] */ ArrayOf<UInt16>** utf16Array)
{
    return ICUUtil::UTF8ByteArrayToUTF16ByteArray(utf8Array, utf16Array);
}

} // namespace ICU
} // namespace Libcore
