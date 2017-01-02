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

#ifndef __ELASTOS_UTILITY_LOCALE_H__
#define __ELASTOS_UTILITY_LOCALE_H__

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Core::ICloneable;
using Elastos::Core::Object;
using Elastos::IO::ISerializable;
using Elastos::IO::IObjectOutputStream;
using Elastos::IO::IObjectStreamField;

namespace Elastos {
namespace Utility {

class CLocaleBuilder;
class CLocaleHelper;

typedef Elastos::Utility::Etl::HashMap<String, String> StringMap;
typedef typename StringMap::Iterator StringMapIterator;

class Locale
    : public Object
    , public ILocale
    , public ICloneable
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    Locale();

    virtual ~Locale();

    /**
     * There's a circular dependency between toLowerCase/toUpperCase and
     * Locale.US. Work around this by avoiding these methods when constructing
     * the built-in locales.
     *
     * @param unused required for this constructor to have a unique signature
     */
    CARAPI constructor(
        /* [in] */ Boolean hasValidatedFields,
        /* [in] */ const String& lowerCaseLanguageCode,
        /* [in] */ const String& upperCaseCountryCode);

    /**
     * Constructs a new {@code Locale} using the specified language.
     */
    CARAPI constructor(
        /* [in] */ const String& language);

    /**
     * Constructs a new {@code Locale} using the specified language and country codes.
     */
    CARAPI constructor(
        /* [in] */ const String& language,
        /* [in] */ const String& country);

    /**
     * Constructs a new {@code Locale} using the specified language, country,
     * and variant codes.
     */
    CARAPI constructor(
        /* [in] */ const String& language,
        /* [in] */ const String& country,
        /* [in] */ const String& variant);

    /**
     * Required by libcore.icu.ICU.
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ const String& language,
        /* [in] */ const String& country,
        /* [in] */ const String& variant,
        /* [in] */ const String& scriptCode,
        /* [in] */ /* nonnull */ ISet* unicodeAttributes,
        /* [in] */ /* nonnull */ IMap* unicodeKeywords,
        /* [in] */ /* nonnull */ IMap* extensions,
        /* [in] */ Boolean hasValidatedFields);

    CARAPI Clone(
        /* [out] */ IInterface** newObj);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    /**
     * Returns a locale for a given BCP-47 language tag. This method is more
     * lenient than {@link Builder#setLanguageTag}. For a given language tag, parsing
     * will proceed up to the first malformed subtag. All subsequent tags are discarded.
     * Note that language tags use {@code -} rather than {@code _}, for example {@code en-US}.
     *
     * @throws NullPointerException if {@code languageTag} is {@code null}.
     *
     * @since 1.7
     */
    static CARAPI ForLanguageTag(
        /* [in] */ const String& languageTag,
        /* [out] */ ILocale** locale);

    /**
     * Returns the system's installed locales. This array always includes {@code
     * Locale.US}, and usually several others. Most locale-sensitive classes
     * offer their own {@code getAvailableLocales} method, which should be
     * preferred over this general purpose method.
     *
     * @see java.text.BreakIterator#getAvailableLocales()
     * @see java.text.Collator#getAvailableLocales()
     * @see java.text.DateFormat#getAvailableLocales()
     * @see java.text.DateFormatSymbols#getAvailableLocales()
     * @see java.text.DecimalFormatSymbols#getAvailableLocales()
     * @see java.text.NumberFormat#getAvailableLocales()
     * @see java.util.Calendar#getAvailableLocales()
     */
    static CARAPI GetAvailableLocales(
        /* [out] */ ArrayOf<ILocale*>** locales);

    /**
     * Returns the country code for this locale, or {@code ""} if this locale
     * doesn't correspond to a specific country.
     */
    CARAPI GetCountry(
        /* [out] */ String* country);

    /**
     * Returns the user's preferred locale. This may have been overridden for
     * this process with {@link #setDefault}.
     *
     * <p>Since the user's locale changes dynamically, avoid caching this value.
     * Instead, use this method to look it up for each use.
     */
    static CARAPI_(AutoPtr<ILocale>) GetDefault();

    /**
     * Equivalent to {@code getDisplayCountry(Locale.getDefault())}.
     */
    CARAPI GetDisplayCountry(
        /* [out] */ String* country);

     /**
     * Returns the name of this locale's country, localized to {@code locale}.
     * Returns the empty string if this locale does not correspond to a specific
     * country.
     */
    CARAPI GetDisplayCountry(
        /* [in] */ ILocale* locale,
        /* [out] */ String* country);

    /**
     * Equivalent to {@code getDisplayLanguage(Locale.getDefault())}.
     */
    CARAPI GetDisplayLanguage(
        /* [out] */ String* language);

    /**
     * Returns the name of this locale's language, localized to {@code locale}.
     * If the language name is unknown, the language code is returned.
     */
    CARAPI GetDisplayLanguage(
        /* [in] */ ILocale* locale,
        /* [out] */ String* language);

    /**
     * Equivalent to {@code getDisplayName(Locale.getDefault())}.
     */
    CARAPI GetDisplayName(
        /* [out] */ String* name);

    /**
     * Returns this locale's language name, country name, and variant, localized
     * to {@code locale}. The exact output form depends on whether this locale
     * corresponds to a specific language, country and variant, such as:
     * {@code English}, {@code English (United States)}, {@code English (United
     * States,Computer)}, {@code anglais (&#x00c9;tats-Unis)}, {@code anglais
     * (&#x00c9;tats-Unis,informatique)}.
     */
    CARAPI GetDisplayName(
        /* [in] */ ILocale* locale,
        /* [out] */ String* name);

    /**
     * Gets the full variant name in the default {@code Locale} for the variant code of
     * this {@code Locale}. If there is no matching variant name, the variant code is
     * returned.
     *
     * @return a variant name.
     */
    CARAPI GetDisplayVariant(
        /* [out] */ String* variantName);

    /**
     * Gets the full variant name in the specified {@code Locale} for the variant code
     * of this {@code Locale}. If there is no matching variant name, the variant code is
     * returned.
     *
     * @param locale
     *            the {@code Locale} for which the display name is retrieved.
     * @return a variant name.
     */
    CARAPI GetDisplayVariant(
        /* [in] */ ILocale* locale,
        /* [out] */ String* variantName);

     /**
     * Gets the three letter ISO country code which corresponds to the country
     * code for this {@code Locale}.
     *
     * @return a three letter ISO language code.
     * @throws MissingResourceException
     *                if there is no matching three letter ISO country code.
     */
    CARAPI GetISO3Country(
        /* [out] */ String* country);

    /**
     * Gets the three letter ISO language code which corresponds to the language
     * code for this {@code Locale}.
     *
     * @return a three letter ISO language code.
     * @throws MissingResourceException
     *                if there is no matching three letter ISO language code.
     */
    CARAPI GetISO3Language(
        /* [out] */ String* language);

    /**
     * Gets the list of two letter ISO country codes which can be used as the
     * country code for a {@code Locale}.
     *
     * @return an array of strings.
     */
    static CARAPI GetISOCountries(
        /* [out] */ ArrayOf<String>** codes);

    /**
     * Gets the list of two letter ISO language codes which can be used as the
     * language code for a {@code Locale}.
     *
     * @return an array of strings.
     */
    static CARAPI GetISOLanguages(
        /* [out] */ ArrayOf<String>** codes);

    /**
     * Gets the language code for this {@code Locale} or the empty string of no language
     * was set.
     *
     * @return a language code.
     */
    CARAPI GetLanguage(
        /* [out] */ String* language);

    /**
     * Gets the variant code for this {@code Locale} or an empty {@code String} if no variant
     * was set.
     *
     * @return a variant code.
     */
    CARAPI GetVariant(
        /* [out] */ String* variant);

    /**
     * Returns the script code for this {@code Locale} or an empty {@code String} if no script
     * was set.
     *
     * If set, the script code will be a title cased string of length 4, as per the ISO 15924
     * specification.
     *
     * @since 1.7
     */
    CARAPI GetScript(
        /* [out] */ String* script);

    /**
     * Equivalent to {@code getDisplayScript(Locale.getDefault()))}
     *
     * @since 1.7
     */
    CARAPI GetDisplayScript(
        /* [out] */ String* script);

    /**
     * Returns the name of this locale's script code, localized to {@link Locale}. If the
     * script code is unknown, the return value of this method is the same as that of
     * {@link #getScript()}.
     *
     * @since 1.7
     */
    CARAPI GetDisplayScript(
        /* [in] */ ILocale* locale,
        /* [out] */ String* script);

    /**
     * Returns a well formed BCP-47 language tag that identifies this locale.
     *
     * Note that this locale itself might consist of ill formed fields, since the
     * public {@code Locale} constructors do not perform validity checks to maintain
     * backwards compatibility. When this is the case, this method will either replace
     * ill formed fields with standard BCP-47 subtags (For eg. "und" (undetermined)
     * for invalid languages) or omit them altogether.
     *
     * Additionally, ill formed variants will result in the remainder of the tag
     * (both variants and extensions) being moved to the private use extension,
     * where they will appear after a subtag whose value is {@code "lvariant"}.
     *
     * It's also important to note that the BCP-47 tag is well formed in the sense
     * that it is unambiguously parseable into its specified components. We do not
     * require that any of the components are registered with the applicable registries.
     * For example, we do not require scripts to be a registered ISO 15924 scripts or
     * languages to appear in the ISO-639-2 code list.
     *
     * @since 1.7
     */
    CARAPI ToLanguageTag(
        /* [out] */ String* tag);

    /**
     * Returns the set of BCP-47 extensions this locale contains.
     *
     * See <a href="https://tools.ietf.org/html/bcp47#section-2.1">
     *     the IETF BCP-47 specification</a> (Section 2.2.6) for details.
     *
     * @since 1.7
     */
    CARAPI GetExtensionKeys(
        /* [out] */ ISet** keys);

    /**
     * Returns the BCP-47 extension whose key is {@code extensionKey}, or {@code null}
     * if this locale does not contain the extension.
     *
     * Individual Keywords and attributes for the unicode
     * locale extension can be fetched using {@link #getUnicodeLocaleAttributes()},
     * {@link #getUnicodeLocaleKeys()}  and {@link #getUnicodeLocaleType}.
     *
     * @since 1.7
     */
    CARAPI GetExtension(
        /* [in] */ Char32 extensionKey,
        /* [out] */ String* extension);

    /**
     * Returns the {@code type} for the specified unicode locale extension {@code key}.
     *
     * For more information about types and keywords, see {@link Builder#setUnicodeLocaleKeyword}
     * and <a href="http://www.unicode.org/reports/tr35/#BCP47">Unicode Technical Standard #35</a>
     *
     * @since 1.7
     */
    CARAPI GetUnicodeLocaleType(
        /* [in] */ const String& keyWord,
        /* [out] */ String* type);

    /**
     * Returns the set of unicode locale extension attributes this locale contains.
     *
     * For more information about attributes, see {@link Builder#addUnicodeLocaleAttribute}
     * and <a href="http://www.unicode.org/reports/tr35/#BCP47">Unicode Technical Standard #35</a>
     *
     * @since 1.7
     */
    CARAPI GetUnicodeLocaleAttributes(
        /* [out] */ ISet** keys);

    /**
     * Returns the set of unicode locale extension keywords this locale contains.
     *
     * For more information about types and keywords, see {@link Builder#setUnicodeLocaleKeyword}
     * and <a href="http://www.unicode.org/reports/tr35/#BCP47">Unicode Technical Standard #35</a>
     *
     * @since 1.7
     */
    CARAPI GetUnicodeLocaleKeys(
        /* [out] */ ISet** keys);

    /**
     * Overrides the default locale. This does not affect system configuration,
     * and attempts to override the system-provided default locale may
     * themselves be overridden by actual changes to the system configuration.
     * Code that calls this method is usually incorrect, and should be fixed by
     * passing the appropriate locale to each locale-sensitive method that's
     * called.
     */
    static CARAPI SetDefault(
        /* [in] */ ILocale* locale);

    /**
     * Returns the string representation of this {@code Locale}. It consists of the
     * language code, country code and variant separated by underscores.
     * If the language is missing the string begins
     * with an underscore. If the country is missing there are 2 underscores
     * between the language and the variant. The variant cannot stand alone
     * without a language and/or country code: in this case this method would
     * return the empty string.
     *
     * <p>Examples: "en", "en_US", "_US", "en__POSIX", "en_US_POSIX"
     *
     * @return the string representation of this {@code Locale}.
     */
    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    //@Override
    CARAPI GetHashCode(
        /* [out] */ Int32* value);

    /**
     * The serialized form for extensions is straightforward. It's simply
     * of the form key1-value1-key2-value2 where each value might in turn contain
     * multiple subtags separated by hyphens. Each key is guaranteed to be a single
     * character in length.
     *
     * This method assumes that {@code extensionsMap} is non-empty.
     *
     * Visible for testing.
     *
     * @hide
     */
    static CARAPI_(String) SerializeExtensions(
        /* [in] */ IMap* extensionsMap);

    /**
     * Visible for testing.
     *
     * @hide
     */
    static CARAPI ParseSerializedExtensions(
        /* [in] */ const String& extString,
        /* [in] */ IMap* outputMap);

    /**
     * This extension is described by http://www.unicode.org/reports/tr35/#RFC5234
     * unicode_locale_extensions = sep "u" (1*(sep keyword) / 1*(sep attribute) *(sep keyword)).
     *
     * It must contain at least one keyword or attribute and attributes (if any)
     * must appear before keywords. Attributes can't appear after keywords because
     * they will be indistinguishable from a subtag of the keyword type.
     *
     * Visible for testing.
     *
     * @hide
     */
    static void ParseUnicodeExtension(
        /* [in] */ ArrayOf<String>* subtags,
        /* [in] */ IMap* keywords,
        /* [in] */ ISet* attributes);

public:
    /**
     * Locale constant for en_CA.
     */
    static const AutoPtr<ILocale> CANADA;

    /**
     * Locale constant for fr_CA.
     */
    static const AutoPtr<ILocale> CANADA_FRENCH;

    /**
     * Locale constant for zh_CN.
     */
    static const AutoPtr<ILocale> CHINA;

    /**
     * Locale constant for zh.
     */
    static const AutoPtr<ILocale> CHINESE;

    /**
     * Locale constant for en.
     */
    static const AutoPtr<ILocale> ENGLISH;

    /**
     * Locale constant for fr_FR.
     */
    static const AutoPtr<ILocale> FRANCE;

    /**
     * Locale constant for fr.
     */
    static const AutoPtr<ILocale> FRENCH;

    /**
     * Locale constant for de.
     */
    static const AutoPtr<ILocale> GERMAN;

    /**
     * Locale constant for de_DE.
     */
    static const AutoPtr<ILocale> GERMANY;

    /**
     * Locale constant for it.
     */
    static const AutoPtr<ILocale> ITALIAN;

    /**
     * Locale constant for it_IT.
     */
    static const AutoPtr<ILocale> ITALY;

    /**
     * Locale constant for ja_JP.
     */
    static const AutoPtr<ILocale> JAPAN;

    /**
     * Locale constant for ja.
     */
    static const AutoPtr<ILocale> JAPANESE;

    /**
     * Locale constant for ko_KR.
     */
    static const AutoPtr<ILocale> KOREA;

    /**
     * Locale constant for ko.
     */
    static const AutoPtr<ILocale> KOREAN;

    /**
     * Locale constant for zh_CN.
     */
    static const AutoPtr<ILocale> PRC;

    /**
     * Locale constant for the root locale. The root locale has an empty language,
     * country, and variant.
     *
     * @since 1.6
     */
    static const AutoPtr<ILocale> ROOT;

    /**
     * Locale constant for zh_CN.
     */
    static const AutoPtr<ILocale> SIMPLIFIED_CHINESE;

    /**
     * Locale constant for zh_TW.
     */
    static const AutoPtr<ILocale> TAIWAN;

    /**
     * Locale constant for zh_TW.
     */
    static const AutoPtr<ILocale> TRADITIONAL_CHINESE;

    /**
     * Locale constant for en_GB.
     */
    static const AutoPtr<ILocale> UK;

    /**
     * Locale constant for en_US.
     */
    static const AutoPtr<ILocale> US;

protected:

    CARAPI CloneImpl(
        /* [in] */ ILocale* locale);

private:
    friend class CLocaleBuilder;

    static CARAPI_(String) ToNewString(
        /* [in] */ const String& languageCode,
        /* [in] */ const String& countryCode,
        /* [in] */ const String& variantCode,
        /* [in] */ const String& scriptCode,
        /* [in] */ IMap* extensions);

    /**
     * Constructs a valid BCP-47 language tag from locale fields. Additional validation
     * is required when this Locale was not constructed using a Builder and variants
     * set this way are treated specially.
     *
     * In both cases, we convert empty language tags to "und", omit invalid country tags
     * and perform a special case conversion of "no-NO-NY" to "nn-NO".
     */
    CARAPI_(String) MakeLanguageTag();

    /**
     * Splits ill formed variants into a set of valid variant subtags (which
     * can be used directly in language tag construction) and a set of invalid
     * variant subtags (which can be appended to the private use extension),
     * provided that each subtag is a valid private use extension subtag.
     *
     * This method returns a two element String array. The first element is a string
     * containing the concatenation of valid variant subtags which can be appended
     * to a BCP-47 tag directly and the second containing the concatenation of
     * invalid variant subtags which can be appended to the private use extension
     * directly.
     *
     * This method assumes that {@code variant} contains at least one ill formed
     * variant subtag.
     */
    static AutoPtr<ArrayOf<String> > SplitIllformedVariant(
        /* [in] */ const String& variant);

    /**
     * Builds a string by concatenating array elements within the range [start, end).
     * The supplied range is assumed to be valid and no checks are performed.
     */
    static String ConcatenateRange(
        /* [in] */ ArrayOf<String>* array,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI WriteObject(
        /* [in] */ IObjectOutputStream* stream);

    CARAPI ReadObject(
        /* [in] */ IObjectOutputStream* stream);

    CARAPI ReadExtensions(
        /* [in] */ const String& extensions);

    /**
     * A UN M.49 is a 3 digit numeric code.
     */
    static Boolean IsUnM49AreaCode(
        /* [in] */ const String& code);

    /*
     * Checks whether a given string is an ASCII alphanumeric string.
     */
    static Boolean IsAsciiAlphaNum(
        /* [in] */ const String& string);

    static Boolean IsValidBcp47Alpha(
        /* [in] */ const String& attributeOrType,
        /* [in] */ Int32 lowerBound,
        /* [in] */ Int32 upperBound);

    static Boolean IsValidBcp47Alphanum(
        /* [in] */ const String& attributeOrType,
        /* [in] */ Int32 lowerBound,
        /* [in] */ Int32 upperBound);

    static CARAPI TitleCaseAsciiWord(
        /* [in] */ const String& word,
        /* [out] */ String* str);

    /**
     * A type list must contain one or more alphanumeric subtags whose lengths
     * are between 3 and 8.
     */
    static Boolean IsValidTypeList(
        /* [in] */ const String& lowerCaseTypeList);

    static void AddUnicodeExtensionToExtensionsMap(
        /* [in] */ ISet* attributes,
        /* [in] */ IMap* keywords,
        /* [in] */ IMap* extensions);

    /**
     * Joins a list of subtags into a BCP-47 tag using the standard separator
     * ("-").
     */
    static String JoinBcp47Subtags(
        /* [in] */ IList* strings);

    /**
     * @hide for internal use only.
     */
    static String AdjustLanguageCode(
        /* [in] */ const String& languageCode);

    static String ConvertGrandfatheredTag(
        /* [in] */ const String& original);

    /**
     * Scans elements of {@code subtags} in the range {@code [startIndex, endIndex)}
     * and appends valid variant subtags upto the first invalid subtag  (if any) to
     * {@code normalizedVariants}.
     */
    static void ExtractVariantSubtags(
        /* [in] */ ArrayOf<String>* subtags,
        /* [in] */ Int32 startIndex,
        /* [in] */ Int32 endIndex,
        /* [in] */ IList* normalizedVariants);

   /**
     * Scans elements of {@code subtags} in the range {@code [startIndex, endIndex)}
     * and inserts valid extensions into {@code extensions}. The scan is aborted
     * when an invalid extension is encountered. Returns the index of the first
     * unparsable element of {@code subtags}.
     */
    static Int32 ExtractExtensions(
        /* [in] */ ArrayOf<String>* subtags,
        /* [in] */ Int32 startIndex,
        /* [in] */ Int32 endIndex,
        /* [in] */ IMap* extensions);

    static CARAPI ForLanguageTag(
        /* @Nonnull */ /* [in] */ const String& tag,
        /* [in] */ Boolean strict,
        /* [out] */ ILocale** locale);

private:
    friend class CLocaleHelper;

    static AutoPtr<ArrayOf<IObjectStreamField*> > sSerialPersistentFields;
    static String UNDETERMINED_LANGUAGE;

    /**
     * Map of grandfathered language tags to their modern replacements.
     */
    static AutoPtr< StringMap > GRANDFATHERED_LOCALES;


    // Initialize a default which is used during static
    // initialization of the default for the platform.
    static AutoPtr<ILocale> sDefaultLocale;
    static Boolean sIsInited;

    /* transient */ String mCountryCode;
    /* transient */ String mLanguageCode;
    /* transient */ String mVariantCode;
    /* transient */ String mScriptCode;

    /* Sorted, Unmodifiable */
    /* transient */ AutoPtr<ISet> mUnicodeAttributes;
    /* Sorted, Unmodifiable */
    /* transient */ AutoPtr<IMap> mUnicodeKeywords; // Map<String, String>
    /* Sorted, Unmodifiable */
    /* transient */ AutoPtr<IMap> mExtensions; //Map<Character, String>

    /**
     * Whether this instance was constructed from a builder. We can make
     * stronger assumptions about the validity of Locale fields if this was
     * constructed by a builder.
     */
    /* transient */ Boolean mHasValidatedFields;

    /* transient */ String mCachedToStringResult;
    /* transient */ String mCachedLanguageTag;
    /* transient */ String mCachedIcuLocaleId;

};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_LOCALE_H__
