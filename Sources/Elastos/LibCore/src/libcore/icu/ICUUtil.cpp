
#include "ElStringByteSink.h"
#include "ICUUtil.h"
#include "CLocale.h"
#include "logging/Logger.h"
#include "StringBuilder.h"
#include <UniquePtr.h>
#include "CBasicLruCache.h"
#include "AutoLock.h"
#include "utility/Collections.h"
#include "Set.h"
#include "CHashMap.h"
#include "CHashSet.h"
#include "StringUtils.h"
#include "CString.h"
#include "Collections.h"
#include <ustrenum.h>

#include <unicode/ucat.h>
#include <unicode/ureslocs.h>
#include <unicode/calendar.h>
#include <unicode/datefmt.h>
#include <unicode/dcfmtsym.h>
#include <unicode/decimfmt.h>
#include <unicode/dtfmtsym.h>
#include <unicode/gregocal.h>
#include <unicode/uidna.h>
#include <unicode/locid.h>
#include <unicode/numfmt.h>
#include <unicode/strenum.h>
#include <unicode/ubrk.h>
#include <unicode/ucal.h>
#include <unicode/uclean.h>
#include <unicode/ucol.h>
#include <unicode/ucurr.h>
#include <unicode/udat.h>
#include <unicode/ustring.h>
#include <unicode/unum.h>
#include <unicode/brkiter.h>
// #include <unicode/ustrenum.h>
#include <unicode/ucasemap.h>
#include <unicode/dtptngen.h>
#include <unicode/ulocdata.h>
#include <unicode/ucurr.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Libcore::Utility::CBasicLruCache;
using Elastos::Core::StringUtils;
using Elastos::Core::IString;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Utility::Collections;
using Elastos::Utility::ISet;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CLocale;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::EIID_ICollection;

namespace Libcore {
namespace ICU {

static const String TAG("ICUUtil");

extern String UnicodeStringToString(const UnicodeString& us);

static String getCurrencyName(const String& languageTag, const String& currencyCode, UCurrNameStyle nameStyle)
{
    Logger::I(TAG, "getCurrencyName for languageTag %s, countryCode %s", languageTag.string(), languageTag.string());
    if (languageTag.IsNull()) {
        return String(NULL);
    }
    if (currencyCode.IsNull()) {
        return String(NULL);
    }
    UnicodeString icuCurrencyCode = UnicodeString::fromUTF8(currencyCode.string());
    UErrorCode status = U_ZERO_ERROR;
    UBool isChoiceFormat = false;
    Int32 charCount;
    const UChar* chars = ucurr_getName(icuCurrencyCode.getTerminatedBuffer(), languageTag,
                                     nameStyle, &isChoiceFormat, &charCount, &status);
    if (U_USING_DEFAULT_WARNING == status) {
        if (UCURR_SYMBOL_NAME == nameStyle) {
            // ICU doesn't distinguish between falling back to the root locale and meeting a genuinely
            // unknown currency. The Currency class does.
            if (!ucurr_isAvailable(icuCurrencyCode.getTerminatedBuffer(), U_DATE_MIN, U_DATE_MAX, &status)) {
                return String(NULL);
            }
        }
        if (UCURR_LONG_NAME == nameStyle) {
            // ICU's default is English. We want the ISO 4217 currency code instead.
            chars = icuCurrencyCode.getBuffer();
            charCount = icuCurrencyCode.length();
        }
    }
    String result;
    if (charCount != 0) {
        result = UnicodeStringToString(UnicodeString(chars));
    }
    Logger::I(TAG, "getCurrencyName %s", result.string());
    return result;
}

// TODO: put this in a header file and use it everywhere!
// DISALLOW_COPY_AND_ASSIGN disallows the copy and operator= functions.
// It goes in the private: declarations in a class.
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
    TypeName(const TypeName&); \
    void operator=(const TypeName&)

class ScopedResourceBundle
{
public:
    ScopedResourceBundle(UResourceBundle* bundle)
        : mBundle(bundle)
    {}

    ~ScopedResourceBundle()
    {
        if (mBundle != NULL) {
            ures_close(mBundle);
        }
    }

    UResourceBundle* get()
    {
        return mBundle;
    }

    Boolean hasKey(const char* key) {
        UErrorCode status = U_ZERO_ERROR;
        ures_getStringByKey(mBundle, key, NULL, &status);
        return U_SUCCESS(status);
      }

private:
    UResourceBundle* mBundle;
    DISALLOW_COPY_AND_ASSIGN(ScopedResourceBundle);
};

AutoPtr<IBasicLruCache> createCache()
{
    AutoPtr<IBasicLruCache> tmp;
    CBasicLruCache::New(8, (IBasicLruCache**)&tmp);
    return tmp;
}

INIT_PROI_3 const AutoPtr<IBasicLruCache> ICUUtil::CACHED_PATTERNS = createCache();
INIT_PROI_3 AutoPtr<ArrayOf<String> > ICUUtil::sIsoLanguages;
INIT_PROI_3 AutoPtr<ArrayOf<String> > ICUUtil::sIsoCountries;
INIT_PROI_3 AutoPtr<ArrayOf<ILocale *> > ICUUtil::sAvailableLocalesCache;
const Int32 ICUUtil::IDX_LANGUAGE = 0;
const Int32 ICUUtil::IDX_SCRIPT = 1;
const Int32 ICUUtil::IDX_REGION = 2;
const Int32 ICUUtil::IDX_VARIANT = 3;

// const Int32 ICUUtil::U_ZERO_ERROR = 0;
// const Int32 ICUUtil::U_INVALID_CHAR_FOUND = 10;
// const Int32 ICUUtil::U_TRUNCATED_CHAR_FOUND = 11;
// const Int32 ICUUtil::U_ILLEGAL_CHAR_FOUND = 12;
// const Int32 ICUUtil::U_BUFFER_OVERFLOW_ERROR = 15;

// Boolean ICUUtil::U_FAILURE(
//     /* [in] */ Int32 error)
// {
//     return error > U_ZERO_ERROR;
// }

ECode ICUUtil::GetISOLanguages(
    /* [out, callee] */ ArrayOf<String>** languages)
{
    VALIDATE_NOT_NULL(languages);

    if (sIsoLanguages == NULL) {
        sIsoLanguages = GetISOLanguagesNative();
    }
    *languages = sIsoLanguages->Clone();
    REFCOUNT_ADD(*languages);
    return NOERROR;
}

ECode ICUUtil::GetISOCountries(
    /* [out, callee] */ ArrayOf<String>** countries)
{
    VALIDATE_NOT_NULL(countries);

    if (sIsoCountries == NULL) {
        sIsoCountries = GetISOCountriesNative();
    }
    *countries = sIsoCountries->Clone();
    REFCOUNT_ADD(*countries);
    return NOERROR;
}

void ICUUtil::ParseLangScriptRegionAndVariants(
    /* [in] */ const String& string,
    /* [in] */ ArrayOf<String>* outputArray)
{
    const Int32 first = string.IndexOf('_');
    const Int32 second = string.IndexOf('_', first + 1);
    const Int32 third = string.IndexOf('_', second + 1);

    if (first == -1) {
        (*outputArray)[IDX_LANGUAGE] = string;
    } else if (second == -1) {
        // Language and country ("ja_JP") OR
        // Language and script ("en_Latn") OR
        // Language and variant ("en_POSIX").

        (*outputArray)[IDX_LANGUAGE] = string.Substring(0, first);
        const String secondString = string.Substring(first + 1);

        if (secondString.GetLength() == 4) {
            // 4 Letter ISO script code.
            (*outputArray)[IDX_SCRIPT] = secondString;
        } else if (secondString.GetLength() == 2 || secondString.GetLength() == 3) {
            // 2 or 3 Letter region code.
            (*outputArray)[IDX_REGION] = secondString;
        } else {
            // If we're here, the length of the second half is either 1 or greater
            // than 5. Assume that ICU won't hand us malformed tags, and therefore
            // assume the rest of the string is a series of variant tags.
            (*outputArray)[IDX_VARIANT] = secondString;
        }
    } else if (third == -1) {
        // Language and country and variant ("ja_JP_TRADITIONAL") OR
        // Language and script and variant ("en_Latn_POSIX") OR
        // Language and script and region ("en_Latn_US"). OR
        // Language and variant with multiple subtags ("en_POSIX_XISOP")

        (*outputArray)[IDX_LANGUAGE] = string.Substring(0, first);
        const String secondString = string.Substring(first + 1, second);
        const String thirdString = string.Substring(second + 1);

        if (secondString.GetLength() == 4) {
            // The second subtag is a script.
            (*outputArray)[IDX_SCRIPT] = secondString;

            // The third subtag can be either a region or a variant, depending
            // on its length.
            if (thirdString.GetLength() == 2 || thirdString.GetLength() == 3 ||
                thirdString.IsEmpty()) {
                (*outputArray)[IDX_REGION] = thirdString;
            } else {
                (*outputArray)[IDX_VARIANT] = thirdString;
            }
        } else if (secondString.IsEmpty() ||
            secondString.GetLength() == 2 || secondString.GetLength() == 3) {
            // The second string is a region, and the third a variant.
            (*outputArray)[IDX_REGION] = secondString;
            (*outputArray)[IDX_VARIANT] = thirdString;
        } else {
            // Variant with multiple subtags.
            (*outputArray)[IDX_VARIANT] = string.Substring(first + 1);
        }
    } else {
        // Language, script, region and variant with 1 or more subtags
        // ("en_Latn_US_POSIX") OR
        // Language, region and variant with 2 or more subtags
        // (en_US_POSIX_VARIANT).
        (*outputArray)[IDX_LANGUAGE] = string.Substring(0, first);
        const String secondString = string.Substring(first + 1, second);
        if (secondString.GetLength() == 4) {
            (*outputArray)[IDX_SCRIPT] = secondString;
            (*outputArray)[IDX_REGION] = string.Substring(second + 1, third);
            (*outputArray)[IDX_VARIANT] = string.Substring(third + 1);
        } else {
            (*outputArray)[IDX_REGION] = secondString;
            (*outputArray)[IDX_VARIANT] = string.Substring(second + 1);
        }
    }
}

ECode ICUUtil::LocaleFromIcuLocaleId(
    /* [in] */ const String& localeId,
    /* [out, callee] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);
    *locale = NULL;

    // @ == ULOC_KEYWORD_SEPARATOR_UNICODE (uloc.h).
    const Int32 extensionsIndex = localeId.IndexOf('@');

    AutoPtr<IMap> pExtensionsMap;
    Collections::GetEmptyMap((IMap**)&pExtensionsMap);
    AutoPtr<IMap> pUnicodeKeywordsMap;
    Collections::GetEmptyMap((IMap**)&pUnicodeKeywordsMap);
    AutoPtr<ISet> pUnicodeAttributeSet;
    Collections::GetEmptySet((ISet**)&pUnicodeAttributeSet);

    if (extensionsIndex != -1) {
        pExtensionsMap = NULL;
        pUnicodeKeywordsMap = NULL;
        pUnicodeAttributeSet = NULL;
        CHashMap::New((IMap**)&pExtensionsMap);
        CHashMap::New((IMap**)&pUnicodeKeywordsMap);
        CHashSet::New((ISet**)&pUnicodeAttributeSet);

        // ICU sends us a semi-colon (ULOC_KEYWORD_ITEM_SEPARATOR) delimited string
        // containing all "keywords" it could parse. An ICU keyword is a key-value pair
        // separated by an "=" (ULOC_KEYWORD_ASSIGN).
        //
        // Each keyword item can be one of three things :
        // - A unicode extension attribute list: In this case the item key is "attribute"
        //   and the value is a hyphen separated list of unicode attributes.
        // - A unicode extension keyword: In this case, the item key will be larger than
        //   1 char in length, and the value will be the unicode extension value.
        // - A BCP-47 extension subtag: In this case, the item key will be exactly one
        //   char in length, and the value will be a sequence of unparsed subtags that
        //   represent the extension.
        //
        // Note that this implies that unicode extension keywords are "promoted" to
        // to the same namespace as the top level extension subtags and their values.
        // There can't be any collisions in practice because the BCP-47 spec imposes
        // restrictions on their lengths.
        const String extensionsString = localeId.Substring(extensionsIndex + 1);
        AutoPtr<ArrayOf<String> > pExtensions;
        StringUtils::Split(extensionsString, String(";"), (ArrayOf<String>**)&pExtensions);
        for(Int32 i = 0; i < pExtensions->GetLength(); ++i)
        {
            const String extension = (*pExtensions)[i];
            // This is the special key for the unicode attributes
            if (extension.StartWith("attribute=")) {
                String unicodeAttributeValues = extension.Substring(String("attribute=").GetLength());
                AutoPtr<ArrayOf<String> > pSplitValues;
                StringUtils::Split(unicodeAttributeValues, "-", (ArrayOf<String>**)&pSplitValues);
                for (Int32 j = 0; j < pSplitValues->GetLength(); ++j)
                {
                    AutoPtr<ICharSequence> csq;
                    CString::New((*pSplitValues)[i], (ICharSequence**)&csq);
                    ((ICollection*)pUnicodeAttributeSet->Probe(EIID_ICollection))->Add(csq);
                }
            } else {
                const Int32 separatorIndex = extension.IndexOf('=');

                if (1 == separatorIndex) {
                    // This is a BCP-47 extension subtag.
                    const String value = extension.Substring(2);
                    const Char32 extensionId = extension.GetChar(0);
                    pExtensionsMap->Put((IInterface*)(&extensionId), (IInterface*)(&value));
                } else {
                    // This is a unicode extension keyword.
                    AutoPtr<ICharSequence> csq1, csq2;
                    CString::New(extension.Substring(0, separatorIndex), (ICharSequence**)&csq1);
                    CString::New(extension.Substring(separatorIndex + 1), (ICharSequence**)&csq2);
                    pUnicodeKeywordsMap->Put(csq1, csq2);
                }
            }
        }
    }

    AutoPtr<ArrayOf<String> > pOutputArray = ArrayOf<String>::Alloc(4);
    for (Int32 i = 0; i < pOutputArray->GetLength(); ++i) {
        (*pOutputArray)[i] = "";
    }

    if (-1 == extensionsIndex) {
        ParseLangScriptRegionAndVariants(localeId, pOutputArray);
    } else {
        ParseLangScriptRegionAndVariants(localeId.Substring(0, extensionsIndex), pOutputArray);
    }

    return CLocale::New((*pOutputArray)[IDX_LANGUAGE],
        (*pOutputArray)[IDX_REGION],
        (*pOutputArray)[IDX_VARIANT],
        (*pOutputArray)[IDX_SCRIPT],
        pUnicodeAttributeSet,
        pUnicodeKeywordsMap,
        pExtensionsMap,
        TRUE,
        locale);
}

ECode ICUUtil::LocalesFromStrings(
    /* [in] */ const ArrayOf<String>& localeNames,
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);

    AutoPtr<ArrayOf<ILocale*> > temp = ArrayOf<ILocale*>::Alloc(localeNames.GetLength());
    for (Int32 i = 0; i < localeNames.GetLength(); ++i) {
        AutoPtr<ILocale> locale;
        FAIL_RETURN(LocaleFromIcuLocaleId(localeNames[i], (ILocale**)&locale));
        temp->Set(i, locale);
    }
    *locales = temp;
    REFCOUNT_ADD(*locales)
    return NOERROR;
}

ECode ICUUtil::GetBestDateTimePattern(
    /* [in] */ const String& skeleton,
    /* [in] */ ILocale* locale,
    /* [out] */ String* rst)
{
    String languageTag;
    locale->ToLanguageTag(&languageTag);
    String key = skeleton + "\t" + languageTag;
    AutoPtr<ICharSequence> key_cs;
    CString::New(key, (ICharSequence**)&key_cs);
    {    AutoLock syncLock(CACHED_PATTERNS);
        AutoPtr<IInterface> tmp;
        CACHED_PATTERNS->Get(key_cs, (IInterface**)&tmp);
        AutoPtr<ICharSequence> pattern_cs = ICharSequence::Probe(tmp);
        if (pattern_cs == NULL) {
            String pattern;
            FAIL_RETURN(GetBestDateTimePatternNative(skeleton, languageTag, &pattern))
            CString::New(pattern, (ICharSequence**)&pattern_cs);
            AutoPtr<IInterface> old;
            CACHED_PATTERNS->Put(key_cs, pattern_cs, (IInterface**)&old);
        }

        pattern_cs->ToString(rst);
    }

    return NOERROR;
}

ECode ICUUtil::GetAvailableLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);
    *locales = NULL;

    if (sAvailableLocalesCache == NULL) {
        AutoPtr<ArrayOf<String> > _locales;
        FAIL_RETURN(GetAvailableLocalesNative((ArrayOf<String>**)&_locales))
        FAIL_RETURN(LocalesFromStrings(*_locales, (ArrayOf<ILocale*>**)&sAvailableLocalesCache))
    }
    AutoPtr<ArrayOf<ILocale*> > temp = sAvailableLocalesCache->Clone();

    *locales = temp;
    REFCOUNT_ADD(*locales)
    return NOERROR;
}

ECode ICUUtil::GetAvailableBreakIteratorLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);
    *locales = NULL;
    AutoPtr<ArrayOf<String> > _locales;
    FAIL_RETURN(GetAvailableBreakIteratorLocalesNative((ArrayOf<String>**)&_locales));
    return LocalesFromStrings(*_locales, locales);
}

ECode ICUUtil::GetAvailableCalendarLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);
    *locales = NULL;
    AutoPtr<ArrayOf<String> > _locales;
    FAIL_RETURN(GetAvailableCalendarLocalesNative((ArrayOf<String>**)&_locales));
    return LocalesFromStrings(*_locales, locales);
}

ECode ICUUtil::GetAvailableCollatorLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);
    *locales = NULL;

    AutoPtr<ArrayOf<String> > _locales;
    FAIL_RETURN(GetAvailableCollatorLocalesNative((ArrayOf<String>**)&_locales));
    return LocalesFromStrings(*_locales, locales);
}

ECode ICUUtil::GetAvailableDateFormatLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);
    *locales = NULL;

    AutoPtr<ArrayOf<String> > _locales;
    FAIL_RETURN(GetAvailableDateFormatLocalesNative((ArrayOf<String>**)&_locales));
    return LocalesFromStrings(*_locales, locales);
}

ECode ICUUtil::GetAvailableDateFormatSymbolsLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    return GetAvailableDateFormatLocales(locales);
}

ECode ICUUtil::GetAvailableDecimalFormatSymbolsLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    return GetAvailableNumberFormatLocales(locales);
}

ECode ICUUtil::GetAvailableNumberFormatLocales(
    /* [out, callee] */ ArrayOf<ILocale*>** locales)
{
    VALIDATE_NOT_NULL(locales);
    *locales = NULL;

    AutoPtr<ArrayOf<String> > _locales;
    FAIL_RETURN(GetAvailableNumberFormatLocalesNative((ArrayOf<String>**)&_locales));
    return LocalesFromStrings(*_locales, locales);
}

static String versionString(
    /* [in] */ const UVersionInfo& version)
{
    char versionString[U_MAX_VERSION_STRING_LENGTH];
    u_versionToString(const_cast<UVersionInfo&>(version), versionString);
    return String(versionString);
}

String ICUUtil::GetIcuVersion()
{
    UVersionInfo icuVersion;
    u_getVersion(icuVersion);
    return versionString(icuVersion);
}

String ICUUtil::GetUnicodeVersion()
{
    UVersionInfo unicodeVersion;
    u_getUnicodeVersion(unicodeVersion);
    return versionString(unicodeVersion);
}

String ICUUtil::ToLowerCase(
    /* [in] */ const String& s,
    /* [in] */ ILocale* locale)
{
    String localeStr;
    locale->ToLanguageTag(&localeStr);
    return ToLowerCase(s, localeStr);
}

String ICUUtil::ToUpperCase(
    /* [in] */ const String& s,
    /* [in] */ ILocale* locale)
{
    String localeStr;
    locale->ToLanguageTag(&localeStr);
    return ToUpperCase(s, localeStr);
}

String ICUUtil::ToLowerCase(
    /* [in] */ const String& s,
    /* [in] */ const String& languageTag)
{
    if (s.IsNull()) {
        return String(NULL);
    }
    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(languageTag);
    if (icuLocale.isBogus()) {
        return String(NULL);
    }
    UnicodeString us = UnicodeString::fromUTF8(s.string());
    UnicodeString original(us);
    us.toLower(icuLocale);
    return us == original ? s : UnicodeStringToString(us);
}

String ICUUtil::ToUpperCase(
    /* [in] */ const String& s,
    /* [in] */ const String& languageTag)
{
    if (s.IsNull()) {
        return String(NULL);
    }
    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(languageTag);
    if (icuLocale.isBogus()) {
        return String(NULL);
    }
    UnicodeString us = UnicodeString::fromUTF8(s.string());
    UnicodeString original(us);
    us.toUpper(icuLocale);
    return us == original ? s : UnicodeStringToString(us);
}

template <typename Counter, typename Getter>
static ECode getAvailableLocales(
    /* [in] */ Counter* counter,
    /* [in] */ Getter* getter,
    /* [out, callee] */ ArrayOf<String>** locales)
{
    VALIDATE_NOT_NULL(locales)

    Int32 count = (*counter)();
    *locales = ArrayOf<String>::Alloc(count);
    REFCOUNT_ADD(*locales);
    for (int32_t i = 0; i < count; ++i) {
        (**locales)[i] = (*getter)(i);
    }
    return NOERROR;
}

ECode ICUUtil::GetAvailableBreakIteratorLocalesNative(
    /* [out, callee] */ ArrayOf<String>** locales)
{
    return getAvailableLocales(ubrk_countAvailable, ubrk_getAvailable, locales);
}

ECode ICUUtil::GetAvailableCalendarLocalesNative(
    /* [out, callee] */ ArrayOf<String>** locales)
{
    return getAvailableLocales(ucal_countAvailable, ucal_getAvailable, locales);
}

ECode ICUUtil::GetAvailableCollatorLocalesNative(
    /* [out, callee] */ ArrayOf<String>** locales)
{
    return getAvailableLocales(ucol_countAvailable, ucol_getAvailable, locales);
}

ECode ICUUtil::GetAvailableDateFormatLocalesNative(
    /* [out, callee] */ ArrayOf<String>** locales)
{
    return getAvailableLocales(udat_countAvailable, udat_getAvailable, locales);
}

ECode ICUUtil::GetAvailableLocalesNative(
    /* [out, callee] */ ArrayOf<String>** locales)
{
    return getAvailableLocales(uloc_countAvailable, uloc_getAvailable, locales);
}

ECode ICUUtil::GetAvailableNumberFormatLocalesNative(
    /* [out, callee] */ ArrayOf<String>** locales)
{
    VALIDATE_NOT_NULL(locales);

    return getAvailableLocales(unum_countAvailable, unum_getAvailable, locales);
}

/**
* Checks if an error has occurred, throwing a suitable exception if so.
* @param env JNI environment
* @param errorCode code to determine if it is an error
* @return 0 if errorCode is not an error, 1 if errorCode is an error, but the
*         creation of the exception to be thrown fails
 * @exception thrown if errorCode represents an error
*/
ECode maybeThrowIcuException(const char* errInfo, UErrorCode errorCode)
{
    if (U_SUCCESS(errorCode)) {
        return NOERROR;
    }

    switch (errorCode) {
        case U_ILLEGAL_ARGUMENT_ERROR:
            Logger::E(TAG, "icu error: %s E_ILLEGAL_ARGUMENT_EXCEPTION", errInfo);
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case U_INDEX_OUTOFBOUNDS_ERROR:
        case U_BUFFER_OVERFLOW_ERROR:
            Logger::E(TAG, "icu error: %s E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION", errInfo);
            return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
        case U_UNSUPPORTED_ERROR:
            Logger::E(TAG, "icu error: %s E_UNSUPPORTED_OPERATION_EXCEPTION", errInfo);
            return E_UNSUPPORTED_OPERATION_EXCEPTION;
        default:
            Logger::E(TAG, "icu error: %s E_RUNTIME_EXCEPTION", errInfo);
            return E_RUNTIME_EXCEPTION;
    }
}

class EnumerationCounter
{
public:
    const size_t count;
    EnumerationCounter(size_t count) : count(count) {}
    size_t operator()() { return count; }
};

class EnumerationGetter
{
public:
    UEnumeration* e;
    UErrorCode* status;
    EnumerationGetter(UEnumeration* e, UErrorCode* status) : e(e), status(status) {}
    const char* operator()(int32_t charCount)
    {
        UnicodeString us = uenum_unext(e, &charCount, status);
        String s("");
        ElStringByteSink sink(&s);
        us.toUTF8(sink);
        return (const char*)s;
    }
};

ECode fromStringEnumeration(
    /* [in] */ UErrorCode& status,
    /* [in] */ const char* provider,
    /* [in] */ StringEnumeration* se,
    /* [out] */ ArrayOf<String>** codes)
{
    VALIDATE_NOT_NULL(codes)
    *codes = NULL;

    FAIL_RETURN(maybeThrowIcuException(provider, status))

    Int32 count = se->count(status);
    FAIL_RETURN(maybeThrowIcuException("StringEnumeration::count", status))

    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(count);
    for (Int32 i = 0; i < count; ++i) {
        const UnicodeString* string = se->snext(status);
        FAIL_RETURN(maybeThrowIcuException("StringEnumeration::snext", status))
        (*result)[i] = UnicodeStringToString(*string);
    }

    *codes = result;
    REFCOUNT_ADD(*codes)
    return NOERROR;
}

ECode ICUUtil::GetAvailableCurrencyCodes(
    /* [out, callee] */ ArrayOf<String>** codes)
{
    VALIDATE_NOT_NULL(codes)
    *codes = NULL;

    UErrorCode status = U_ZERO_ERROR;
    UStringEnumeration e(ucurr_openISOCurrencies(UCURR_COMMON|UCURR_NON_DEPRECATED, &status));
    return fromStringEnumeration(status, "ucurr_openISOCurrencies", &e, codes);
}

// TODO: rewrite this with int32_t ucurr_forLocale(const char* locale, UChar* buff, int32_t buffCapacity, UErrorCode* ec)...
String ICUUtil::GetCurrencyCode(
    /* [in] */ const String& locale)
{
    String nullStr;
    UErrorCode status = U_ZERO_ERROR;
    ScopedResourceBundle supplData(ures_openDirect(U_ICUDATA_CURR, "supplementalData", &status));
    if (U_FAILURE(status)) {
        return nullStr;
    }

    ScopedResourceBundle currencyMap(ures_getByKey(supplData.get(), "CurrencyMap", NULL, &status));
    if (U_FAILURE(status)) {
        return nullStr;
    }

    ScopedResourceBundle currency(ures_getByKey(currencyMap.get(), locale.string(), NULL, &status));
    if (U_FAILURE(status)) {
        return String(NULL);
    }

    ScopedResourceBundle currencyElem(ures_getByIndex(currency.get(), 0, NULL, &status));
    if (U_FAILURE(status)) {
        return String("XXX");
    }

    // check if there is a 'to' date. If there is, the currency isn't used anymore.
    ScopedResourceBundle currencyTo(ures_getByKey(currencyElem.get(), "to", NULL, &status));
    if (!U_FAILURE(status)) {
        // return and let the caller throw an exception
        return String(NULL);
    }
    // Ignore the failure to find a 'to' date.
    status = U_ZERO_ERROR;

    ScopedResourceBundle currencyId(ures_getByKey(currencyElem.get(), "id", NULL, &status));
    if (U_FAILURE(status)) {
        // No id defined for this country
        return String("XXX");
    }

    Int32 length;
    const UChar* chars = ures_getString(currencyId.get(), &length, &status);
    if (U_FAILURE(status) || length == 0) {
        Logger::E(TAG, "failed to GetCurrencyCode for %s: failed %d, length: %d",
            locale.string(), U_FAILURE(status), length);
        return String("XXX");
    }

    return UnicodeStringToString(UnicodeString(chars));
}

String ICUUtil::GetCurrencyDisplayName(
    /* [in] */ const String& locale,
    /* [in] */ const String& currencyCode)
{
    return getCurrencyName(locale, currencyCode, UCURR_LONG_NAME);
}

Int32 ICUUtil::GetCurrencyFractionDigits(
    /* [in] */ const String& currencyCode)
{
    if (currencyCode.IsNull()) {
        return 0;
    }
    UnicodeString icuCurrencyCode = UnicodeString::fromUTF8(currencyCode.string());
    UErrorCode status = U_ZERO_ERROR;
    return ucurr_getDefaultFractionDigits(icuCurrencyCode.getTerminatedBuffer(), &status);
}

String ICUUtil::GetCurrencySymbol(
    /* [in] */ const String& locale,
    /* [in] */ const String& currencyCode)
{
    return getCurrencyName(locale, currencyCode, UCURR_SYMBOL_NAME);
}

String ICUUtil::GetDisplayCountryNative(
    /* [in] */ const String& targetLanguageTag,
    /* [in] */ const String& languageTag)
{
    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(languageTag);
    if (icuLocale.isBogus()) {
        return String(NULL);
    }
    ::Locale icuTargetLocale;
    icuTargetLocale.setToBogus();
    icuTargetLocale = ::Locale::createFromName(targetLanguageTag);
    if (icuTargetLocale.isBogus()) {
        return String(NULL);
    }
    UnicodeString str;
    icuTargetLocale.getDisplayCountry(icuLocale, str);
    return UnicodeStringToString(str);
}

String ICUUtil::GetDisplayLanguageNative(
    /* [in] */ const String& targetLanguageTag,
    /* [in] */ const String& languageTag)
{
    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(languageTag);
    if (icuLocale.isBogus()) {
        return String(NULL);
    }
    ::Locale icuTargetLocale;
    icuTargetLocale.setToBogus();
    icuTargetLocale = ::Locale::createFromName(targetLanguageTag);
    if (icuTargetLocale.isBogus()) {
        return String(NULL);
    }
    UnicodeString str;
    icuTargetLocale.getDisplayLanguage(icuLocale, str);
    return UnicodeStringToString(str);
}

String ICUUtil::GetDisplayVariant(
        /* [in] */ ILocale* targetLocale,
        /* [in] */ ILocale* locale)
{
    String targetLanguageTag;
    targetLocale->ToLanguageTag(&targetLanguageTag);
    String languageTag;
    locale->ToLanguageTag(&languageTag);

    return GetDisplayVariantNative(targetLanguageTag, languageTag);
}

String ICUUtil::GetDisplayVariantNative(
    /* [in] */ const String& targetLanguageTag,
    /* [in] */ const String& languageTag)
{
    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(languageTag);
    if (icuLocale.isBogus()) {
        return String(NULL);
    }
    ::Locale icuTargetLocale;
    icuTargetLocale.setToBogus();
    icuTargetLocale = ::Locale::createFromName(targetLanguageTag);
    if (icuTargetLocale.isBogus()) {
        return String(NULL);
    }
    UnicodeString str;
    icuTargetLocale.getDisplayVariant(icuLocale, str);
    return UnicodeStringToString(str);
}

String ICUUtil::GetISO3Country(
    /* [in] */ const String& languageTag)
{
    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(languageTag);
    if (icuLocale.isBogus()) {
        return String(NULL);
    }
    return String(icuLocale.getISO3Country());
}

String ICUUtil::GetISO3Language(
    /* [in] */ const String& languageTag)
{
    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(languageTag);
    if (icuLocale.isBogus()) {
        return String(NULL);
    }
    return String(icuLocale.getISO3Language());
}

String ICUUtil::AddLikelySubtags(
    /* [in] */ const String& locale)
{
    UErrorCode status = U_ZERO_ERROR;
    char maximizedLocaleID[ULOC_FULLNAME_CAPACITY];
    uloc_addLikelySubtags(locale.string(), maximizedLocaleID, sizeof(maximizedLocaleID), &status);
    if (U_FAILURE(status)) {
        return locale;
    }
    return String(maximizedLocaleID);
}

String ICUUtil::GetDisplayScript(
    /* [in] */ ILocale* targetLocale,
    /* [in] */ ILocale* locale)
{
    String targetLanguageTag;
    targetLocale->ToLanguageTag(&targetLanguageTag);
    String languageTag;
    locale->ToLanguageTag(&languageTag);

    return GetDisplayScriptNative(targetLanguageTag, languageTag);
}

String ICUUtil::GetDisplayScriptNative(
    const String& targetLanguageTag,
    const String& languageTag)
{
    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(languageTag);
    if (icuLocale.isBogus()) {
        return String(NULL);
    }
    ::Locale icuTargetLocale;
    icuTargetLocale.setToBogus();
    icuTargetLocale = ::Locale::createFromName(targetLanguageTag);
    if (icuTargetLocale.isBogus()) {
        return String(NULL);
    }
    UnicodeString str;
    icuTargetLocale.getDisplayScript(icuLocale, str);
    return UnicodeStringToString(str);
}

String ICUUtil::GetScript(
    /* [in] */ const String& locale)
{
    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(locale);
    if (icuLocale.isBogus())
        return String(NULL);

    return String(icuLocale.getScript());
}

static AutoPtr<ArrayOf<String> > ToStringArray(const char* const* strings)
{
    Int32 count = 0;
    while (strings[count] != NULL) {
        ++count;
    }
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(count);
    for (Int32 i = 0; i < count; ++i) {
        (*result)[i] = strings[i];
    }
    return result;
}

AutoPtr<ArrayOf<String> > ICUUtil::GetISOLanguagesNative()
{
    return ToStringArray(Locale::getISOLanguages());
}

AutoPtr<ArrayOf<String> > ICUUtil::GetISOCountriesNative()
{
    return ToStringArray(Locale::getISOCountries());
}

static void setIntegerField(
    /* [in] */ Int32 value,
    /* [out] */ IInteger32** field)
{
    CInteger32::New(value, field);
}

static void setStringField(
    /* [in] */ UResourceBundle* bundle,
    /* [in] */ Int32 index,
    /* [out] */ String* field)
{
    UErrorCode status = U_ZERO_ERROR;
    Int32 charCount;
    UnicodeString chars = ures_getStringByIndex(bundle, index, &charCount, &status);
    if (U_SUCCESS(status)) {
        String s("");
        ElStringByteSink sink(&s);
        chars.toUTF8(sink);
        *field = s;
    }
    else {
       PFL_EX("Error setting String field %d from ICUUtil resource: %s", index, u_errorName(status));
    }
}

static void setStringArrayField(
    /* [in] */ const UnicodeString* valueArray,
    /* [in] */ int32_t size,
    /* [out] */ ArrayOf<String> ** field)
{
    AutoPtr<ArrayOf<String> > result = ArrayOf<String>::Alloc(size);
    for (int32_t i = 0; i < size ; i++) {
        String str("");
        ElStringByteSink sink(&str);
        valueArray[i].toUTF8(sink);
        (*result)[i] = str;
    }
    *field = result;
    REFCOUNT_ADD(*field);
}

static void setCharField(
    /* [in] */ const UnicodeString& value,
    /* [out] */ Char32* field)
{
    if (value.length() == 0) {
        return;
    }
    *field = (Char32)value.charAt(0);
}

static void setStringField(
    /* [in] */ const UnicodeString& value,
    /* [out] */ String* field)
{
    String str("");
    ElStringByteSink sink(&str);
    value.toUTF8(sink);
    *field = str;
}

static void setNumberPatterns(
    /* [in] */ LocaleData* localeData,
    /* [in] */ ::Locale& locale)
{
    UErrorCode status = U_ZERO_ERROR;

    UnicodeString pattern;
    UniquePtr<DecimalFormat> fmt(static_cast<DecimalFormat*>(
            NumberFormat::createInstance(locale, UNUM_CURRENCY, status)));
    pattern = fmt->toPattern(pattern.remove());
    setStringField(pattern, &localeData->mCurrencyPattern);

    fmt.reset(static_cast<DecimalFormat*>(NumberFormat::createInstance(locale, UNUM_DECIMAL, status)));
    pattern = fmt->toPattern(pattern.remove());
    setStringField(pattern, &localeData->mNumberPattern);

    fmt.reset(static_cast<DecimalFormat*>(NumberFormat::createInstance(locale, UNUM_PERCENT, status)));
    pattern = fmt->toPattern(pattern.remove());
    setStringField(pattern, &localeData->mPercentPattern);
}

static void setDecimalFormatSymbolsData(
    /* [in] */ LocaleData* localeData,
    /* [in] */ ::Locale& locale)
{
    UErrorCode status = U_ZERO_ERROR;
    DecimalFormatSymbols dfs(locale, status);

    setCharField(dfs.getSymbol(DecimalFormatSymbols::kDecimalSeparatorSymbol), &localeData->mDecimalSeparator);
    setCharField(dfs.getSymbol(DecimalFormatSymbols::kGroupingSeparatorSymbol), &localeData->mGroupingSeparator);
    setCharField(dfs.getSymbol(DecimalFormatSymbols::kPatternSeparatorSymbol), &localeData->mPatternSeparator);
    setCharField(dfs.getSymbol(DecimalFormatSymbols::kPercentSymbol), &localeData->mPercent);
    setCharField(dfs.getSymbol(DecimalFormatSymbols::kPerMillSymbol), &localeData->mPerMill);
    setCharField(dfs.getSymbol(DecimalFormatSymbols::kMonetarySeparatorSymbol), &localeData->mMonetarySeparator);
    setStringField(dfs.getSymbol(DecimalFormatSymbols:: kMinusSignSymbol), &localeData->mMinusSign);
    setStringField(dfs.getSymbol(DecimalFormatSymbols::kExponentialSymbol), &localeData->mExponentSeparator);
    setStringField(dfs.getSymbol(DecimalFormatSymbols::kInfinitySymbol), &localeData->mInfinity);
    setStringField(dfs.getSymbol(DecimalFormatSymbols::kNaNSymbol), &localeData->mNaN);
    setCharField(dfs.getSymbol(DecimalFormatSymbols::kZeroDigitSymbol), &localeData->mZeroDigit);
}

// Iterates up through the locale hierarchy. So "en_US" would return "en_US", "en", "".
class LocaleNameIterator {
public:
    LocaleNameIterator(const char* locale_name, UErrorCode& status) : status_(status), has_next_(true) {
        strcpy(locale_name_, locale_name);
        locale_name_length_ = strlen(locale_name_);
    }

    const char* Get() {
        return locale_name_;
    }

    bool HasNext() {
        return has_next_;
    }

    void Up() {
        if (locale_name_length_ == 0) {
            has_next_ = false;
        } else {
            locale_name_length_ = uloc_getParent(locale_name_, locale_name_, sizeof(locale_name_), &status_);
        }
    }

private:
    UErrorCode& status_;
    bool has_next_;
    char locale_name_[ULOC_FULLNAME_CAPACITY];
    int32_t locale_name_length_;

    DISALLOW_COPY_AND_ASSIGN(LocaleNameIterator);
};

static bool getAmPmMarkersNarrow(
    LocaleData* localeData,
    const char* locale_name)
{
    UErrorCode status = U_ZERO_ERROR;
    ScopedResourceBundle root(ures_open(NULL, locale_name, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    ScopedResourceBundle calendar(ures_getByKey(root.get(), "calendar", NULL, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    ScopedResourceBundle gregorian(ures_getByKey(calendar.get(), "gregorian", NULL, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    ScopedResourceBundle amPmMarkersNarrow(ures_getByKey(gregorian.get(), "AmPmMarkersNarrow", NULL, &status));
    if (U_FAILURE(status)) {
        return false;
    }
    setStringField(amPmMarkersNarrow.get(), 0, &localeData->mNarrowAm);
    setStringField(amPmMarkersNarrow.get(), 1, &localeData->mNarrowPm);
    return true;
}

static Boolean getDateTimePatterns(
    /* [in] */ LocaleData* localeData,
    /* [in] */ const char* locale_name)
{
    UErrorCode status = U_ZERO_ERROR;
    ScopedResourceBundle root(ures_open(NULL, locale_name, &status));
    if (U_FAILURE(status)) {
        return FALSE;
    }
    ScopedResourceBundle calendar(ures_getByKey(root.get(), "calendar", NULL, &status));
    if (U_FAILURE(status)) {
        return FALSE;
    }
    ScopedResourceBundle gregorian(ures_getByKey(calendar.get(), "gregorian", NULL, &status));
    if (U_FAILURE(status)) {
        return FALSE;
    }
    ScopedResourceBundle dateTimePatterns(ures_getByKey(gregorian.get(), "DateTimePatterns", NULL, &status));
    if (U_FAILURE(status)) {
        return FALSE;
    }
    setStringField(dateTimePatterns.get(), 0, &localeData->mFullTimeFormat);
    setStringField(dateTimePatterns.get(), 1, &localeData->mLongTimeFormat);
    setStringField(dateTimePatterns.get(), 2, &localeData->mMediumTimeFormat);
    setStringField(dateTimePatterns.get(), 3, &localeData->mShortTimeFormat);
    setStringField(dateTimePatterns.get(), 4, &localeData->mFullDateFormat);
    setStringField(dateTimePatterns.get(), 5, &localeData->mLongDateFormat);
    setStringField(dateTimePatterns.get(), 6, &localeData->mMediumDateFormat);
    setStringField(dateTimePatterns.get(), 7, &localeData->mShortDateFormat);
    return TRUE;
}

static Boolean getYesterdayTodayAndTomorrow(
    /* [in] */ LocaleData* localeData,
    /* [in] */ const ::Locale& locale,
    /* [in] */ const char* locale_name)
{
    UErrorCode status = U_ZERO_ERROR;
    ScopedResourceBundle root(ures_open(NULL, locale_name, &status));
    ScopedResourceBundle fields(ures_getByKey(root.get(), "fields", NULL, &status));
    ScopedResourceBundle day(ures_getByKey(fields.get(), "day", NULL, &status));
    ScopedResourceBundle relative(ures_getByKey(day.get(), "relative", NULL, &status));
    if (U_FAILURE(status)) {
        return FALSE;
    }
    UnicodeString yesterday(ures_getUnicodeStringByKey(relative.get(), "-1", &status));
    UnicodeString today(ures_getUnicodeStringByKey(relative.get(), "0", &status));
    UnicodeString tomorrow(ures_getUnicodeStringByKey(relative.get(), "1", &status));
    if (U_FAILURE(status)) {
        Logger::E(TAG, "Error getting yesterday/today/tomorrow for %s: %s",
            locale_name, u_errorName(status));
        return FALSE;
    }

    // We title-case the strings so they have consistent capitalization (http://b/14493853).
    UniquePtr<BreakIterator> brk(BreakIterator::createSentenceInstance(locale, status));
    if (U_FAILURE(status)) {
        Logger::E(TAG, "Error getting yesterday/today/tomorrow break iterator for %s: %s",
            locale_name, u_errorName(status));
        return FALSE;
    }
    yesterday.toTitle(brk.get(), locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    today.toTitle(brk.get(), locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    tomorrow.toTitle(brk.get(), locale, U_TITLECASE_NO_LOWERCASE | U_TITLECASE_NO_BREAK_ADJUSTMENT);
    setStringField(yesterday, &localeData->mYesterday);
    setStringField(today, &localeData->mToday);
    setStringField(tomorrow, &localeData->mTomorrow);
    return TRUE;
}

Boolean ICUUtil::InitLocaleDataNative(
    /* [in] */ const String& languageTag,
    /* [in] */ LocaleData* localeData)
{
    if (languageTag.string() == NULL) {
        return FALSE;
    }
    if (languageTag.GetByteLength() >= ULOC_FULLNAME_CAPACITY) {
        return FALSE; // ICUUtil has a fixed-length limit.
    }

    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(languageTag);
    if (icuLocale.isBogus()) {
        return NOERROR;
    }

    // Get the DateTimePatterns.
    UErrorCode status = U_ZERO_ERROR;
    Boolean foundDateTimePatterns = FALSE;
    for (LocaleNameIterator it(icuLocale.getBaseName(), status); it.HasNext(); it.Up()) {
      if (getDateTimePatterns(localeData, it.Get())) {
          foundDateTimePatterns = TRUE;
          break;
      }
    }
    if (!foundDateTimePatterns) {
        Logger::E("ICUUtil", "Couldn't find ICUUtil DateTimePatterns for %s", languageTag.string());
        return FALSE;
    }

    // Get the "Yesterday", "Today", and "Tomorrow" strings.
    Boolean foundYesterdayTodayAndTomorrow = FALSE;
    for (LocaleNameIterator it(icuLocale.getBaseName(), status); it.HasNext(); it.Up()) {
        if (getYesterdayTodayAndTomorrow(localeData, icuLocale, it.Get())) {
            foundYesterdayTodayAndTomorrow = TRUE;
            break;
        }
    }
    if (!foundYesterdayTodayAndTomorrow) {
        Logger::E("ICUUtil", "Couldn't find ICUUtil yesterday/today/tomorrow for %s", languageTag.string());
        return FALSE;
    }

    // Get the narrow "AM" and "PM" strings.
    Boolean foundAmPmMarkersNarrow = FALSE;
    for (LocaleNameIterator it(icuLocale.getBaseName(), status); it.HasNext(); it.Up()) {
      if (getAmPmMarkersNarrow(localeData, it.Get())) {
        foundAmPmMarkersNarrow = TRUE;
        break;
      }
    }
    if (!foundAmPmMarkersNarrow) {
        Logger::E("ICUUtil", "Couldn't find ICUUtil AmPmMarkersNarrow for %s", languageTag.string());
        return FALSE;
    }

    status = U_ZERO_ERROR;
    UniquePtr<Calendar> cal(Calendar::createInstance(icuLocale, status));
    if (U_FAILURE(status)) {
        return FALSE;
    }

    localeData->mMinimalDaysInFirstWeek = NULL;
    localeData->mFirstDayOfWeek = NULL;
    setIntegerField(cal->getFirstDayOfWeek(), (IInteger32**)&localeData->mFirstDayOfWeek);
    setIntegerField(cal->getMinimalDaysInFirstWeek(),
            (IInteger32**)&localeData->mMinimalDaysInFirstWeek);

    // Get DateFormatSymbols.
    status = U_ZERO_ERROR;
    DateFormatSymbols dateFormatSym(icuLocale, status);
    if (U_FAILURE(status)) {
        return FALSE;
    }

    // Get AM/PM and BC/AD.
    int32_t count = 0;
    const UnicodeString* amPmStrs = dateFormatSym.getAmPmStrings(count);
    localeData->mAmPm = NULL;
    setStringArrayField(amPmStrs, count, (ArrayOf<String> **)&(localeData->mAmPm));

    const UnicodeString* erasStrs = dateFormatSym.getEras(count);
    localeData->mEras = NULL;
    setStringArrayField(erasStrs, count, (ArrayOf<String> **)&localeData->mEras);

    localeData->mLongMonthNames = NULL;
    const UnicodeString* longMonthNames =
            dateFormatSym.getMonths(count, DateFormatSymbols::FORMAT, DateFormatSymbols::WIDE);
    setStringArrayField(longMonthNames, count, (ArrayOf<String> **)&localeData->mLongMonthNames);

    localeData->mShortMonthNames = NULL;
    const UnicodeString* shortMonthNames =
            dateFormatSym.getMonths(count, DateFormatSymbols::FORMAT, DateFormatSymbols::ABBREVIATED);
    setStringArrayField(shortMonthNames, count, (ArrayOf<String> **)&localeData->mShortMonthNames);

    localeData->mTinyMonthNames = NULL;
    const UnicodeString* tinyMonthNames =
            dateFormatSym.getMonths(count, DateFormatSymbols::FORMAT, DateFormatSymbols::NARROW);
    setStringArrayField(tinyMonthNames, count, (ArrayOf<String> **)&localeData->mTinyMonthNames);

    localeData->mLongWeekdayNames = NULL;
    const UnicodeString* longWeekdayNames =
            dateFormatSym.getWeekdays(count, DateFormatSymbols::FORMAT, DateFormatSymbols::WIDE);
    setStringArrayField(longWeekdayNames, count, (ArrayOf<String> **)&localeData->mLongWeekdayNames);

    localeData->mShortWeekdayNames = NULL;
    const UnicodeString* shortWeekdayNames =
            dateFormatSym.getWeekdays(count, DateFormatSymbols::FORMAT, DateFormatSymbols::ABBREVIATED);
    setStringArrayField(shortWeekdayNames, count, (ArrayOf<String> **)&localeData->mShortWeekdayNames);

    localeData->mTinyWeekdayNames = NULL;
    const UnicodeString* tinyWeekdayNames =
            dateFormatSym.getWeekdays(count, DateFormatSymbols::FORMAT, DateFormatSymbols::NARROW);
    setStringArrayField(tinyWeekdayNames, count, (ArrayOf<String> **)&localeData->mTinyWeekdayNames);

    localeData->mLongStandAloneMonthNames = NULL;
    const UnicodeString* longStandAloneMonthNames =
            dateFormatSym.getMonths(count, DateFormatSymbols::STANDALONE, DateFormatSymbols::WIDE);
    setStringArrayField(longStandAloneMonthNames, count, (ArrayOf<String> **)&localeData->mLongStandAloneMonthNames);

    localeData->mShortStandAloneMonthNames = NULL;
    const UnicodeString* shortStandAloneMonthNames =
            dateFormatSym.getMonths(count, DateFormatSymbols::STANDALONE, DateFormatSymbols::ABBREVIATED);
    setStringArrayField(shortStandAloneMonthNames, count, (ArrayOf<String> **)&localeData->mShortStandAloneMonthNames);

    localeData->mTinyStandAloneMonthNames = NULL;
    const UnicodeString* tinyStandAloneMonthNames =
            dateFormatSym.getMonths(count, DateFormatSymbols::STANDALONE, DateFormatSymbols::NARROW);
    setStringArrayField(tinyStandAloneMonthNames, count, (ArrayOf<String> **)&localeData->mTinyStandAloneMonthNames);

    localeData->mLongStandAloneWeekdayNames = NULL;
    const UnicodeString* longStandAloneWeekdayNames =
            dateFormatSym.getWeekdays(count, DateFormatSymbols::STANDALONE, DateFormatSymbols::WIDE);
    setStringArrayField(longStandAloneWeekdayNames, count, (ArrayOf<String> **)&localeData->mLongStandAloneWeekdayNames);

    localeData->mShortStandAloneWeekdayNames = NULL;
    const UnicodeString* shortStandAloneWeekdayNames =
            dateFormatSym.getWeekdays(count, DateFormatSymbols::STANDALONE, DateFormatSymbols::ABBREVIATED);
    setStringArrayField(shortStandAloneWeekdayNames, count, (ArrayOf<String> **)&localeData->mShortStandAloneWeekdayNames);

    localeData->mTinyStandAloneWeekdayNames = NULL;
    const UnicodeString* tinyStandAloneWeekdayNames =
            dateFormatSym.getWeekdays(count, DateFormatSymbols::STANDALONE, DateFormatSymbols::NARROW);
    setStringArrayField(tinyStandAloneWeekdayNames, count, (ArrayOf<String> **)&localeData->mTinyStandAloneWeekdayNames);

    status = U_ZERO_ERROR;

    // For numberPatterns and symbols.
    setNumberPatterns(localeData, icuLocale);
    setDecimalFormatSymbolsData(localeData, icuLocale);

    String countryCode(icuLocale.getCountry());
    String internationalCurrencySymbol = ICUUtil::GetCurrencyCode(countryCode);
    Logger::I(TAG, "GetCurrencyCode %s for countryCode %s", internationalCurrencySymbol.string(), countryCode.string());
    String currencySymbol;
    if (!internationalCurrencySymbol.IsNull()) {
        currencySymbol = ICUUtil::GetCurrencySymbol(languageTag, internationalCurrencySymbol);
    }
    else {
        internationalCurrencySymbol = "XXX";
    }
    if (currencySymbol.IsNull()) {
        // This is the UTF-8 encoding of U+00A4 (CURRENCY SIGN).
        currencySymbol = "\u00A4"; //\xc2\xa4
    }
    localeData->mCurrencySymbol = currencySymbol;
    localeData->mInternationalCurrencySymbol = internationalCurrencySymbol;

    Logger::I(TAG, " currencySymbol %s, internationalCurrencySymbol %s",
        currencySymbol.string(), internationalCurrencySymbol.string());
    return TRUE;
}

ECode ICUUtil::GetBestDateTimePatternNative(
    /* [in] */ const String& skeleton,
    /* [in] */ const String& languageTag,
    /* [out] */ String* rev)
{
    VALIDATE_NOT_NULL(rev)
    *rev = NULL;

    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(languageTag);
    if (icuLocale.isBogus()) {
        return NOERROR;
    };

    UErrorCode status = U_ZERO_ERROR;
    UniquePtr<DateTimePatternGenerator> generator(DateTimePatternGenerator::createInstance(icuLocale, status));
    FAIL_RETURN(maybeThrowIcuException("DateTimePatternGenerator::createInstance", status))

    if (skeleton.IsNull()) {
        return NOERROR;
    }
    UnicodeString result(generator->getBestPattern(UnicodeString::fromUTF8(skeleton.string()), status));
    FAIL_RETURN(maybeThrowIcuException("DateTimePatternGenerator::getBestPattern", status))

    *rev = UnicodeStringToString(result);
    return NOERROR;
}

ECode ICUUtil::GetDateFormatOrder(
    /* [in] */ const String& pattern,
    /* [out, callee] */ ArrayOf<Char32>** locales)
{
    VALIDATE_NOT_NULL(locales)
    *locales = NULL;

    AutoPtr<ArrayOf<Char32> > result = ArrayOf<Char32>::Alloc(3);
    Int32 resultIndex = 0;
    Boolean sawDay = FALSE;
    Boolean sawMonth = FALSE;
    Boolean sawYear = FALSE;
    Int32 length = pattern.GetLength();
    AutoPtr<ArrayOf<Char32> > chars = pattern.GetChars();

    for (Int32 i = 0; i < length; ++i) {
        Char32 ch = (*chars)[i];
        if ('d' == ch || 'L' == ch || 'M' == ch || 'y' == ch) {
            if ('d' == ch && !sawDay) {
                (*result)[resultIndex++] = 'd';
                sawDay =TRUE;
            } else if (('L' == ch || 'M' == ch) && !sawMonth) {
                (*result)[resultIndex++] = 'M';
                sawMonth = TRUE;
            } else if (('y' == ch) && !sawYear) {
                (*result)[resultIndex++] ='y';
                sawYear =TRUE;
            }
        } else if ('G' == ch) {
            // Ignore the era specifier, if present.
        } else if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
            Logger::E(TAG, "ICUUtil: Bad pattern character %c in %s", ch, pattern.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        } else if ('\''== ch) {
            if (i < length - 1 && '\'' == (*chars)[i + 1]) {
                ++i;
            } else {
                i = pattern.IndexOf('\'', i + 1);
                if (-1 == i) {
                    Logger::E(TAG, "ICUUtil: Bad quoting in %s", pattern.string());
                    return E_ILLEGAL_ARGUMENT_EXCEPTION;
                }
                ++i;
            }
        } else {
            // Ignore spaces and punctuation.
        }
    }

    *locales = result;
    REFCOUNT_ADD(result);
    return NOERROR;
}

ECode ICUUtil::GetCldrVersion(
    /* [out] */ String* rev)
{
    VALIDATE_NOT_NULL(rev)
    UErrorCode status = U_ZERO_ERROR;
    UVersionInfo cldrVersion;
    ulocdata_getCLDRVersion(cldrVersion, &status);
    *rev = versionString(cldrVersion);
    return NOERROR;
}

Int32 ICUUtil::GetCurrencyNumericCode(
    /* [in] */ const String& currencyCode)
{
    if (currencyCode.IsNull()) {
        return 0;
    }
    UnicodeString icuCurrencyCode = UnicodeString::fromUTF8(currencyCode.string());
    return ucurr_getNumericCode(icuCurrencyCode.getTerminatedBuffer());
}

ECode ICUUtil::GetCurrencyDisplayName(
    /* [in] */ ILocale* locale,
    /* [in] */ const String& currencyCode,
    /* [out] */ String* displayName)
{
    VALIDATE_NOT_NULL(displayName)
    String languageTag;
    locale->ToLanguageTag(&languageTag);
    *displayName = GetCurrencyDisplayName(languageTag, currencyCode);
    return NOERROR;
}

ECode ICUUtil::GetCurrencySymbol(
    /* [in] */ ILocale* locale,
    /* [in] */ const String& currencyCode,
    /* [out] */ String* currencySymbol)
{
    VALIDATE_NOT_NULL(currencySymbol)
    String languageTag;
    locale->ToLanguageTag(&languageTag);
    *currencySymbol = GetCurrencySymbol(languageTag, currencyCode);
    return NOERROR;
}

ECode ICUUtil::GetDisplayCountry(
    /* [in] */ ILocale* targetLocale,
    /* [in] */ ILocale* locale,
    /* [out] */ String* displayCountry)
{
    String targetLanguageTag;
    targetLocale->ToLanguageTag(&targetLanguageTag);
    String languageTag;
    locale->ToLanguageTag(&languageTag);

    *displayCountry = GetDisplayCountryNative(targetLanguageTag, languageTag);
    return NOERROR;
}

ECode ICUUtil::GetDisplayLanguage(
    /* [in] */ ILocale* targetLocale,
    /* [in] */ ILocale* locale,
    /* [out] */ String* displayLanguage)
{
    VALIDATE_NOT_NULL(displayLanguage)
    *displayLanguage = NULL;

    String targetLanguageTag;
    targetLocale->ToLanguageTag(&targetLanguageTag);
    String languageTag;
    locale->ToLanguageTag(&languageTag);

    *displayLanguage = GetDisplayLanguageNative(targetLanguageTag, languageTag);
    return NOERROR;
}

AutoPtr<ILocale> ICUUtil::AddLikelySubtags(
    /* [in] */ ILocale* locale)
{
    String languageTag;
    locale->ToLanguageTag(&languageTag);

    AutoPtr<ILocale> rev;
    CLocale::ForLanguageTag(AddLikelySubtags(languageTag), (ILocale**)&rev);
    return rev;
}

ECode ICUUtil::SetDefaultLocale(
    /* [in] */ const String& languageTag)
{
    ::Locale icuLocale;
    icuLocale.setToBogus();
    icuLocale = ::Locale::createFromName(languageTag);
    if (icuLocale.isBogus()) {
        return NOERROR;
    }

    UErrorCode status = U_ZERO_ERROR;
    ::Locale::setDefault(icuLocale, status);
    FAIL_RETURN(maybeThrowIcuException("Locale::setDefault", status))
    return NOERROR;
}

ECode ICUUtil::GetDefaultLocale(
    /* [out] */ String* defaultLocale)
{
    VALIDATE_NOT_NULL(defaultLocale)
    *defaultLocale = UnicodeStringToString(::Locale::getDefault().getName());
    return NOERROR;
}

ECode ICUUtil::UTF16ByteArrayToUTF8ByteArray(
    /* [in] */ ArrayOf<UInt16>* utf16Array,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ ArrayOf<Byte>** utf8Array)
{
    VALIDATE_NOT_NULL(utf8Array);
    UnicodeString us(utf16Array->GetPayload() + offset, length);
    String str("");
    ElStringByteSink sink(&str);
    us.toUTF8(sink);
    *utf8Array = str.GetBytes();
    REFCOUNT_ADD(*utf8Array);
    return NOERROR;
}

ECode ICUUtil::UTF16ByteArrayToString(
    /* [in] */ ArrayOf<UInt16>* utf16Array,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    UnicodeString us(utf16Array->GetPayload() + offset, length);
    String str("");
    ElStringByteSink sink(&str);
    us.toUTF8(sink);
    *result = str;
    return NOERROR;
}

ECode ICUUtil::UTF8ByteArrayToUTF16ByteArray(
    /* [in] */ ArrayOf<Byte>* utf8Array,
    /* [out, callee] */ ArrayOf<UInt16>** utf16Array)
{
    VALIDATE_NOT_NULL(utf16Array);
    *utf16Array = NULL;

    UnicodeString unicodeS = UnicodeString::fromUTF8((char*)utf8Array->GetPayload());
    UChar dst[256];
    UErrorCode status = U_ZERO_ERROR;
    size_t resultLength = uidna_IDNToUnicode(unicodeS.getBuffer(), unicodeS.length(), &dst[0], sizeof(dst), UIDNA_DEFAULT, NULL, &status);
    if (U_FAILURE(status)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr< ArrayOf<UInt16> > outarr = ArrayOf<UInt16>::Alloc(resultLength);
    for (size_t i = 0; i < resultLength; ++i) {
        (*outarr)[i] = dst[i];
    }
    *utf16Array = outarr;
    REFCOUNT_ADD(*utf16Array);
    return NOERROR;
}

} // namespace ICU
} // namespace Libcore
