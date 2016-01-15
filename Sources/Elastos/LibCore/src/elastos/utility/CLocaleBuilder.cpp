#include "CLocaleBuilder.h"
#include "CTreeSet.h"
#include "CTreeMap.h"
#include "CLocale.h"
#include "StringUtils.h"
#include "CString.h"
#include "CChar32.h"
#include "CoreUtils.h"

using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::IChar32;
using Elastos::Core::CChar32;

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(CLocaleBuilder, Object, ILocaleBuilder)

CAR_OBJECT_IMPL(CLocaleBuilder)

CLocaleBuilder::CLocaleBuilder()
    : mLanguage("")
    , mRegion("")
    , mVariant("")
    , mScript("")
{
}

ECode CLocaleBuilder::constructor()
{
    // NOTE: We use sorted maps in the builder & the locale class itself
    // because serialized forms of the unicode locale extension (and
    // of the extension map itself) are specified to be in alphabetic
    // order of keys.
    CTreeSet::New((ISet**)&mAttributes);
    CTreeMap::New((IMap**)&mKeywords);
    CTreeMap::New((IMap**)&mExtensions);
    return NOERROR;
}

ECode CLocaleBuilder::SetLanguage(
    /* [in] */ const String& language)
{
    return NormalizeAndValidateLanguage(language, TRUE /* strict */, &mLanguage);
}

ECode CLocaleBuilder::NormalizeAndValidateLanguage(
    /* [in] */ const String& language,
    /* [in] */ Boolean strict,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("");

   if (language.IsNullOrEmpty()) {
        return NOERROR;
    }

    String lowercaseLanguage = language.ToLowerCase();//ToLowerCase(Locale.ROOT);
    if (!CLocale::IsValidBcp47Alpha(lowercaseLanguage, 2, 3)) {
        if (strict) {
            ALOGE("CLocaleBuilder::NormalizeAndValidateLanguage: error Invalid language: %s", language.string());
            return E_ILLFORMED_LOCALE_EXCEPTION;
        } else {
            *str = CLocale::UNDETERMINED_LANGUAGE;
            return NOERROR;
        }
    }

    *str = lowercaseLanguage;
    return NOERROR;
}

ECode CLocaleBuilder::SetLanguageTag(
    /* [in] */ const String& languageTag)
{
    if (languageTag.IsNullOrEmpty()) {
        Clear();
        return NOERROR;
    }

    AutoPtr<ILocale> fromIcu;
    FAIL_RETURN(CLocale::ForLanguageTag(languageTag, true /* strict */, (ILocale**)&fromIcu));
    // When we ask ICU for strict parsing, it might return a null locale
    // if the language tag is malformed.
    if (fromIcu == NULL) {
        ALOGE("CLocaleBuilder::SetLanguageTag: IllformedLocaleException, Invalid languageTag: %s", languageTag.string());
        return E_ILLFORMED_LOCALE_EXCEPTION;
    }

    SetLocale(fromIcu);
    return NOERROR;
}

ECode CLocaleBuilder::SetRegion(
    /* [in] */ const String& region)
{
    return NormalizeAndValidateRegion(region, TRUE /* strict */, &mRegion);
}

ECode CLocaleBuilder::NormalizeAndValidateRegion(
    /* [in] */ const String& region,
    /* [in] */ Boolean strict,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("");

    if (region.IsNullOrEmpty()) {
        return NOERROR;
    }

    String uppercaseRegion = region.ToUpperCase(); // toUpperCase(Locale.ROOT);
    if (!CLocale::IsValidBcp47Alpha(uppercaseRegion, 2, 2)
        && !CLocale::IsUnM49AreaCode(uppercaseRegion)) {
        if (strict) {
            ALOGE("CLocaleBuilder::NormalizeAndValidateRegion: IllformedLocaleException, Invalid region: %s", region.string());
            return E_ILLFORMED_LOCALE_EXCEPTION;
        }
        else {
            return NOERROR;
        }
    }

    *str = uppercaseRegion;
    return NOERROR;
}

ECode CLocaleBuilder::SetVariant(
    /* [in] */ const String& variant)
{
    return NormalizeAndValidateVariant(variant, &mVariant);
}

ECode CLocaleBuilder::NormalizeAndValidateVariant(
    /* [in] */ const String& variant,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("");

    if (variant.IsNullOrEmpty()) {
        return NOERROR;
    }

    // Note that unlike extensions, we canonicalize to lower case alphabets
    // and underscores instead of hyphens.
    String normalizedVariant = variant.Replace('-', '_');
    AutoPtr<ArrayOf<String> > subTags;
    StringUtils::Split(normalizedVariant, String("_"), (ArrayOf<String>**)&subTags);

    for (Int32 i = 0; i < subTags->GetLength(); ++i) {
        if (!IsValidVariantSubtag((*subTags)[i])) {
            ALOGE("CLocaleBuilder::NormalizeAndValidateVariant: IllformedLocaleException, Invalid variant: %s", variant.string());
            return E_ILLFORMED_LOCALE_EXCEPTION;
        }
    }

    *str = normalizedVariant;
    return NOERROR;
}

Boolean CLocaleBuilder::IsValidVariantSubtag(
    /* [in] */ const String& subTag)
{
    // The BCP-47 spec states that :
    // - Subtags can be between [5, 8] alphanumeric chars in length.
    // - Subtags that start with a number are allowed to be 4 chars in length.
    if (subTag.GetLength() >= 5 && subTag.GetLength() <= 8) {
        if (CLocale::IsAsciiAlphaNum(subTag)) {
            return TRUE;
        }
    }
    else if (subTag.GetLength() == 4) {
        Char32 firstChar = subTag.GetChar(0);
        if ((firstChar >= '0' && firstChar <= '9') && CLocale::IsAsciiAlphaNum(subTag)) {
            return TRUE;
        }
    }

    return FALSE;
}

ECode CLocaleBuilder::SetScript(
    /* [in] */ const String& script)
{
    return NormalizeAndValidateScript(script, TRUE /* strict */, &mScript);
}

ECode CLocaleBuilder::NormalizeAndValidateScript(
    /* [in] */ const String& script,
    /* [in] */ Boolean strict,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = String("");
    if (script.IsNullOrEmpty()) {
        return NOERROR;
    }

    if (!CLocale::IsValidBcp47Alpha(script, 4, 4)) {
        if (strict) {
            ALOGE("CLocaleBuilder::NormalizeAndValidateScript: IllformedLocaleException, Invalid script: %s", script.string());
            return E_ILLFORMED_LOCALE_EXCEPTION;
        }
        else {
            return NOERROR;
        }
    }

    return CLocale::TitleCaseAsciiWord(script, str);
}

ECode CLocaleBuilder::SetLocale(
    /* [in] */ ILocale* locale)
{
    if (locale == NULL) {
        ALOGE("CLocaleBuilder::SetLocale, NullPointerExceptionm, locale == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    CLocale* l = (CLocale*)locale;
    // Make copies of the existing values so that we don't partially
    // update the state if we encounter an error.
    String backupLanguage = mLanguage;
    String backupRegion = mRegion;
    String backupVariant = mVariant;

    String language, country, variant;
    locale->GetLanguage(&language);
    locale->GetCountry(&country);
    locale->GetVariant(&variant);

    ECode ec = SetLanguage(language);
    FAIL_GOTO(ec, _FAIL_)

    ec = SetRegion(country);
    FAIL_GOTO(ec, _FAIL_)

    ec = SetVariant(variant);
    FAIL_GOTO(ec, _FAIL_)

    // The following values can be set only via the builder class, so
    // there's no need to normalize them or check their validity.

    locale->GetScript(&mScript);

    mExtensions->Clear();
    mExtensions->PutAll(IMap::Probe(l->mExtensions));

    mKeywords->Clear();
    mKeywords->PutAll(IMap::Probe(l->mUnicodeKeywords));

    mAttributes->Clear();
    mAttributes->AddAll(ICollection::Probe(l->mUnicodeAttributes));
    return NOERROR;

_FAIL_:
    mLanguage = backupLanguage;
    mRegion = backupRegion;
    mVariant = backupVariant;
    return ec;
}

ECode CLocaleBuilder::AddUnicodeLocaleAttribute(
    /* [in] */ const String& attribute)
{
    if (attribute.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    String lowercaseAttribute = attribute.ToLowerCase();//toLowerCase(Locale.ROOT);
    if (!CLocale::IsValidBcp47Alphanum(lowercaseAttribute, 3, 8)) {
        ALOGE("CLocaleBuilder::AddUnicodeLocaleAttribute: IllformedLocaleException, Invalid attribute: %s", attribute.string());
        return E_ILLFORMED_LOCALE_EXCEPTION;
    }

    AutoPtr<ICharSequence> csq;
    CString::New(lowercaseAttribute, (ICharSequence**)&csq);
    return mAttributes->Add(csq->Probe(EIID_IInterface));
}

ECode CLocaleBuilder::RemoveUnicodeLocaleAttribute(
    /* [in] */ const String& attribute)
{
    if (attribute.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    // Weirdly, remove is specified to check whether the attribute
    // is valid, so we have to perform the full alphanumeric check here.
    String lowercaseAttribute = attribute.ToLowerCase();//toLowerCase(Locale.ROOT);
    if (!CLocale::IsValidBcp47Alphanum(lowercaseAttribute, 3, 8)) {
        ALOGE("CLocaleBuilder::RemoveUnicodeLocaleAttribute: IllformedLocaleException, Invalid attribute: %s", attribute.string());
        return E_ILLFORMED_LOCALE_EXCEPTION;
    }

    AutoPtr<ICharSequence> csq;
    CString::New(lowercaseAttribute, (ICharSequence**)&csq);
    return mAttributes->Remove(csq->Probe(EIID_IInterface));
}

ECode CLocaleBuilder::SetExtension(
    /* [in] */ Char32 key,
    /* [in] */ const String& value)
{
    AutoPtr<IChar32> keyObj;
    CChar32::New(key, (IChar32**)&keyObj);
    if (value.IsNullOrEmpty()) {
        mExtensions->Remove(keyObj->Probe(EIID_IInterface));
        return NOERROR;
    }

    String normalizedValue = value.ToLowerCase();//ToLowerCase(Locale.ROOT)
    normalizedValue = normalizedValue.Replace('_', '-');
    AutoPtr<ArrayOf<String> > subtags;
    StringUtils::Split(normalizedValue, String("-"), (ArrayOf<String>**)&subtags);

    // Lengths for subtags in the private use extension should be [1, 8] chars.
    // For all other extensions, they should be [2, 8] chars.
    //
    // http://www.rfc-editor.org/rfc/bcp/bcp47.txt
    Int32 minimumLength = (key == ILocale::PRIVATE_USE_EXTENSION) ? 1 : 2;
    for (Int32 i = 0; i < subtags->GetLength(); ++i) {
        String subtag = (*subtags)[i];
        if (!CLocale::IsValidBcp47Alphanum(subtag, minimumLength, 8)) {
            ALOGE("CLocaleBuilder::SetExtension: IllformedLocaleException, Invalid private use extension : %s", value.string());
            return E_ILLFORMED_LOCALE_EXCEPTION;
        }
    }

    // We need to take special action in the case of unicode extensions,
    // since we claim to understand their keywords and mAttributes->
    if (key == ILocale::UNICODE_LOCALE_EXTENSION) {
        // First clear existing attributes and mKeywords->
        mExtensions->Clear();
        mAttributes->Clear();

        CLocale::ParseUnicodeExtension(subtags, mKeywords, mAttributes);
    }
    else {
        AutoPtr<ICharSequence> csq;
        CString::New(normalizedValue, (ICharSequence**)&csq);
        mExtensions->Put(keyObj, TO_IINTERFACE(csq));
    }
    return NOERROR;
}

ECode CLocaleBuilder::ClearExtensions()
{
    mExtensions->Clear();
    mAttributes->Clear();
    mKeywords->Clear();
    return NOERROR;
}

ECode CLocaleBuilder::SetUnicodeLocaleKeyword(
    /* [in] */ const String& key,
    /* [in] */ const String& type)
{
    if (key.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (type.IsNull() && mKeywords != NULL) {
        AutoPtr<ICharSequence> csq;
        CString::New(key, (ICharSequence**)&csq);
        mKeywords->Remove(TO_IINTERFACE(csq));
        return NOERROR;
    }

    String lowerCaseKey = key.ToLowerCase();//ToLowerCase(Locale.ROOT)
    // The key must be exactly two alphanumeric characters.
    if (lowerCaseKey.GetLength() != 2 || !CLocale::IsAsciiAlphaNum(lowerCaseKey)) {
        ALOGE("CLocaleBuilder::SetExtension: IllformedLocaleException, Invalid unicode locale keyword : %s", key.string());
        return E_ILLFORMED_LOCALE_EXCEPTION;
    }

    // The type can be one or more alphanumeric strings of length [3, 8] characters,
    // separated by a separator char, which is one of "_" or "-". Though the spec
    // doesn't require it, we normalize all "_" to "-" to make the rest of our
    // processing easier.
    String lowerCaseType = type.ToLowerCase();//ToLowerCase(Locale.ROOT)
    lowerCaseType = lowerCaseType.Replace('_', '-');
    if (!CLocale::IsValidTypeList(lowerCaseType)) {
        ALOGE("CLocaleBuilder::SetExtension: IllformedLocaleException, Invalid unicode locale type : %s", type.string());
        return E_ILLFORMED_LOCALE_EXCEPTION;
    }

    // Everything checks out fine, add the <key, type> mapping to the list.
    AutoPtr<ICharSequence> keycsq = CoreUtils::Convert(lowerCaseKey);
    AutoPtr<ICharSequence> typecsq = CoreUtils::Convert(lowerCaseType);
    mKeywords->Put(TO_IINTERFACE(keycsq), TO_IINTERFACE(typecsq));
    return NOERROR;
}

ECode CLocaleBuilder::Clear()
{
    ClearExtensions();
    mLanguage = mRegion = mVariant = mScript = String("");
    return NOERROR;
}

ECode CLocaleBuilder::Build(
    /* [out] */ ILocale** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // NOTE: We need to make a copy of attributes, keywords and extensions
    // because the RI allows this builder to reused.
    AutoPtr<CLocale> locale;
    CLocale::NewByFriend((CLocale**)&locale);
    FAIL_RETURN(locale->constructor(mLanguage, mRegion, mVariant,
        mScript, mAttributes, mKeywords, mExtensions, TRUE /* has validated fields */));
    AutoPtr<ILocale> l = (ILocale*)locale->Probe(EIID_ILocale);
    *result = l;
    REFCOUNT_ADD(*result);
    return NOERROR;
}


} // namespace Utility
} // namespace Elastos