
#ifndef __ELASTOS_UTILITY_CLOCALEBUILDER_H__
#define __ELASTOS_UTILITY_CLOCALEBUILDER_H__

#include "_Elastos_Utility_CLocaleBuilder.h"
#include "Object.h"

using Elastos::Core::Object;

namespace Elastos {
namespace Utility {

class Locale;

CarClass(CLocaleBuilder)
    , public Object
    , public ILocaleBuilder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CLocaleBuilder();

    CARAPI constructor();

    /**
     * Sets the locale language. If {@code language} is {@code null} or empty, the
     * previous value is cleared.
     *
     * As per BCP-47, the language must be between 2 and 3 ASCII characters
     * in length and must only contain characters in the range {@code [a-zA-Z]}.
     *
     * This value is usually an <a href="http://www.loc.gov/standards/iso639-2/">
     * ISO-639-2</a> alpha-2 or alpha-3 code, though no explicit checks are
     * carried out that it's a valid code in that namespace.
     *
     * Values are normalized to lower case.
     *
     * Note that we don't support BCP-47 "extlang" languages because they were
     * only ever used to substitute for a lack of 3 letter language codes.
     *
     * @throws IllformedLocaleException if the language was invalid.
     */
    CARAPI SetLanguage(
        /* [in] */ const String& language);

    /**
     * Set the state of this builder to the parsed contents of the BCP-47 language
     * tag {@code languageTag}.
     *
     * This method is equivalent to a call to {@link #clear} if {@code languageTag}
     * is {@code null} or empty.
     *
     * <b>NOTE:</b> In contrast to {@link Locale#forLanguageTag(String)}, which
     * simply ignores malformed input, this method will throw an exception if
     * its input is malformed.
     *
     * @throws IllformedLocaleException if {@code languageTag} is not a well formed
     *         BCP-47 tag.
     */
    CARAPI SetLanguageTag(
        /* [in] */ const String& tag);

    /**
     * Sets the locale region. If {@code region} is {@code null} or empty, the
     * previous value is cleared.
     *
     * As per BCP-47, the region must either be a 2 character ISO-3166-1 code
     * (each character in the range [a-zA-Z]) OR a 3 digit UN M.49 code.
     *
     * Values are normalized to upper case.
     *
     * @throws IllformedLocaleException if {@code} region is invalid.
     */
    CARAPI SetRegion(
        /* [in] */ const String& region);

    /**
     * Sets the locale variant. If {@code variant} is {@code null} or empty,
     * the previous value is cleared.
     *
     * The input string my consist of one or more variants separated by
     * valid separators ('-' or '_').
     *
     * As per BCP-47, each variant must be between 5 and 8 alphanumeric characters
     * in length (each character in the range {@code [a-zA-Z0-9]}) but
     * can be exactly 4 characters in length if the first character is a digit.
     *
     * Note that this is a much stricter interpretation of {@code variant}
     * than the public {@code Locale} constructors. The latter allowed free form
     * variants.
     *
     * Variants are case sensitive and all separators are normalized to {@code '_'}.
     *
     * @throws IllformedLocaleException if {@code} variant is invalid.
     */
    CARAPI SetVariant(
        /* [in] */ const String& variant);

    /**
     * Sets the locale script. If {@code script} is {@code null} or empty,
     * the previous value is cleared.
     *
     * As per BCP-47, the script must be 4 characters in length, and
     * each character in the range {@code [a-zA-Z]}.
     *
     * A script usually represents a valid ISO 15924 script code, though no
     * other registry or validity checks are performed.
     *
     * Scripts are normalized to title cased values.
     *
     * @throws IllformedLocaleException if {@code script} is invalid.
     */
    CARAPI SetScript(
        /* [in] */ const String& script);

    /**
     * Sets the state of the builder to the {@link Locale} represented by
     * {@code locale}.
     *
     * Note that the locale's language, region and variant are validated as per
     * the rules specified in {@link #setLanguage}, {@link #setRegion} and
     * {@link #setVariant}.
     *
     * All existing builder state is discarded.
     *
     * @throws IllformedLocaleException if {@code locale} is invalid.
     * @throws NullPointerException if {@code locale} is null.
     */
    CARAPI SetLocale(
        /* [in] */ ILocale* locale);

    /**
     * Adds the specified attribute to the list of attributes in the unicode
     * locale extension.
     *
     * Attributes must be between 3 and 8 characters in length, and each character
     * must be in the range {@code [a-zA-Z0-9]}.
     *
     * Attributes are normalized to lower case values. All added attributes and
     * keywords are combined to form a complete unicode locale extension on
     * {@link Locale} objects built by this builder, and accessible via
     * {@link Locale#getExtension(char)} with the {@link Locale#UNICODE_LOCALE_EXTENSION}
     * key.
     *
     * @throws IllformedLocaleException if {@code attribute} is invalid.
     * @throws NullPointerException if {@code attribute} is null.
     */
    CARAPI AddUnicodeLocaleAttribute(
        /* [in] */ const String& attribute);

    /**
     * Removes an attribute from the list of attributes in the unicode locale
     * extension.
     *
     * {@code attribute} must be valid as per the rules specified in
     * {@link #addUnicodeLocaleAttribute}.
     *
     * This method has no effect if {@code attribute} hasn't already been
     * added.
     *
     * @throws IllformedLocaleException if {@code attribute} is invalid.
     * @throws NullPointerException if {@code attribute} is null.
     */
    CARAPI RemoveUnicodeLocaleAttribute(
        /* [in] */ const String& attribute);

    /**
     * Sets the extension identified by {@code key} to {@code value}.
     *
     * {@code key} must be in the range {@code [a-zA-Z0-9]}.
     *
     * If {@code value} is {@code null} or empty, the extension is removed.
     *
     * In the general case, {@code value} must be a series of subtags separated
     * by ({@code "-"} or {@code "_"}). Each subtag must be between
     * 2 and 8 characters in length, and each character in the subtag must be in
     * the range {@code [a-zA-Z0-9]}.
     *
     * <p>
     * There are two special cases :
     * <li>
     *     <ul>
     *         The unicode locale extension
     *         ({@code key == 'u'}, {@link Locale#UNICODE_LOCALE_EXTENSION}) : Setting
     *         the unicode locale extension results in all existing keyword and attribute
     *         state being replaced by the parsed result of {@code value}. For example,
     *         {@code  builder.setExtension('u', "baaaz-baaar-fo-baar-ba-baaz")}
     *         is equivalent to:
     *         <pre>
     *             builder.addUnicodeLocaleAttribute("baaaz");
     *             builder.addUnicodeLocaleAttribute("baaar");
     *             builder.setUnicodeLocaleKeyword("fo", "baar");
     *             builder.setUnicodeLocaleKeyword("ba", "baaa");
     *         </pre>
     *     </ul>
     *     <ul>
     *         The private use extension
     *         ({@code key == 'x'}, {@link Locale#PRIVATE_USE_EXTENSION}) : Each subtag in a
     *         private use extension can be between 1 and 8 characters in length (in contrast
     *         to a minimum length of 2 for all other extensions).
     *     </ul>
     * </li>
     *
     * @throws IllformedLocaleException if {@code value} is invalid.
     */
    CARAPI SetExtension(
        /* [in] */ Char32 key,
        /* [in] */ const String& value);

    /**
     * Clears all extensions from this builder. Note that this also implicitly
     * clears all state related to the unicode locale extension; all attributes
     * and keywords set by {@link #addUnicodeLocaleAttribute} and
     * {@link #setUnicodeLocaleKeyword} are cleared.
     */
    CARAPI ClearExtensions();

    /**
     * Adds a key / type pair to the list of unicode locale extension keys.
     *
     * {@code key} must be 2 characters in length, and each character must be
     * in the range {@code [a-zA-Z0-9]}.
     *
     * {#code type} can either be empty, or a series of one or more subtags
     * separated by a separator ({@code "-"} or {@code "_"}). Each subtag must
     * be between 3 and 8 characters in length and each character in the subtag
     * must be in the range {@code [a-zA-Z0-9]}.
     *
     * Note that the type is normalized to lower case, and all separators
     * are normalized to {@code "-"}. All added attributes and
     * keywords are combined to form a complete unicode locale extension on
     * {@link Locale} objects built by this builder, and accessible via
     * {@link Locale#getExtension(char)} with the {@link Locale#UNICODE_LOCALE_EXTENSION}
     * key.
     *
     * @throws IllformedLocaleException if {@code key} or {@code value} are
     *         invalid.
     */
    CARAPI SetUnicodeLocaleKeyword(
        /* [in] */ const String& key,
        /* [in] */ const String& type);

    /**
     * Clears all existing state from this builder.
     */
    CARAPI Clear();

    /**
     * Constructs a locale from the existing state of the builder. Note that this
     * method is guaranteed to succeed since field validity checks are performed
     * at the point of setting them.
     */
    CARAPI Build(
        /* [out] */ ILocale** locale);

private:
    friend class Locale;

    static CARAPI NormalizeAndValidateLanguage(
        /* [in] */ const String& language,
        /* [in] */ Boolean strict,
        /* [out] */ String* str);

    static CARAPI NormalizeAndValidateRegion(
        /* [in] */ const String& region,
        /* [in] */ Boolean strict,
        /* [out] */ String* str);

    static CARAPI NormalizeAndValidateVariant(
        /* [in] */ const String& region,
        /* [out] */ String* str);

    static Boolean IsValidVariantSubtag(
        /* [in] */ const String& subTag);

    static CARAPI NormalizeAndValidateScript(
        /* [in] */ const String& script,
        /* [in] */ Boolean strict,
        /* [out] */ String* str);

private:
    String mLanguage;
    String mRegion;
    String mVariant;
    String mScript;

    AutoPtr<ISet> mAttributes;  // Set<String>
    AutoPtr<IMap> mKeywords;    // Map<String, String>
    AutoPtr<IMap> mExtensions;  // Map<Character, String>
};

} // namespace Utility
} // namespace Elastos

#endif //__ELASTOS_UTILITY_CLOCALEBUILDER_H__
