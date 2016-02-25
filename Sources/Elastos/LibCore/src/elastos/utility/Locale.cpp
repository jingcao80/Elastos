
#include "Locale.h"
#include "ICUUtil.h"
#include "StringBuilder.h"
#include "CSystem.h"
#include "CString.h"
#include "CLocale.h"
#include "CLocaleBuilder.h"
#include "Collections.h"
#include "CTreeSet.h"
#include "CTreeMap.h"
#include "StringUtils.h"
#include "CChar32.h"
#include "CArrayList.h"
#include "CoreUtils.h"

using Elastos::Core::IChar32;
using Elastos::Core::CChar32;
using Elastos::Core::CSystem;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::CString;
using Elastos::Core::ICharSequence;
using Elastos::IO::EIID_ISerializable;
using Libcore::ICU::ICUUtil;

namespace Elastos {
namespace Utility {

static AutoPtr<ILocale> CreateLocale(
    /* [in] */ const String& language,
    /* [in] */ const String& country)
{
    AutoPtr<CLocale> l;
    ASSERT_SUCCEEDED(CLocale::NewByFriend(TRUE, language, country, (CLocale**)&l));
    return (ILocale*)l.Get();
}

INIT_PROI_3 const AutoPtr<ILocale> Locale::CANADA = CreateLocale(String("en"), String("CA"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::CANADA_FRENCH = CreateLocale(String("fr"), String("CA"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::CHINA = CreateLocale(String("zh"), String("CN"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::CHINESE = CreateLocale(String("zh"), String(""));
INIT_PROI_3 const AutoPtr<ILocale> Locale::ENGLISH = CreateLocale(String("en"), String(""));
INIT_PROI_3 const AutoPtr<ILocale> Locale::FRANCE = CreateLocale(String("fr"), String("FR"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::FRENCH = CreateLocale(String("fr"), String(""));
INIT_PROI_3 const AutoPtr<ILocale> Locale::GERMAN = CreateLocale(String("de"), String(""));
INIT_PROI_3 const AutoPtr<ILocale> Locale::GERMANY = CreateLocale(String("de"), String("DE"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::ITALIAN = CreateLocale(String("it"), String(""));
INIT_PROI_3 const AutoPtr<ILocale> Locale::ITALY = CreateLocale(String("it"), String("IT"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::JAPAN = CreateLocale(String("ja"), String("JP"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::JAPANESE = CreateLocale(String("ja"), String(""));
INIT_PROI_3 const AutoPtr<ILocale> Locale::KOREA = CreateLocale(String("ko"), String("KR"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::KOREAN = CreateLocale(String("ko"), String(""));
INIT_PROI_3 const AutoPtr<ILocale> Locale::PRC = CreateLocale(String("zh"), String("CN"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::ROOT = CreateLocale(String(""), String(""));
INIT_PROI_3 const AutoPtr<ILocale> Locale::SIMPLIFIED_CHINESE = CreateLocale(String("zh"), String("CN"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::TAIWAN = CreateLocale(String("zh"), String("TW"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::TRADITIONAL_CHINESE = CreateLocale(String("zh"), String("TW"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::UK = CreateLocale(String("en"), String("GB"));
INIT_PROI_3 const AutoPtr<ILocale> Locale::US = CreateLocale(String("en"), String("US"));

String Locale::UNDETERMINED_LANGUAGE("und");

static AutoPtr<ArrayOf<IObjectStreamField*> > InitSerialPersistentFields()
{
    AutoPtr<ArrayOf<IObjectStreamField*> > notimpl;
    // new ObjectStreamField("country", String.class),
    // new ObjectStreamField("hashcode", int.class),
    // new ObjectStreamField("language", String.class),
    // new ObjectStreamField("variant", String.class),
    // new ObjectStreamField("script", String.class),
    // new ObjectStreamField("extensions", String.class),
    return notimpl;
}
INIT_PROI_3 AutoPtr<ArrayOf<IObjectStreamField*> > Locale::sSerialPersistentFields = InitSerialPersistentFields();

static AutoPtr< StringMap > InitGRANDFATHERED_LOCALES()
{
    AutoPtr< StringMap > map = new StringMap();

    // From http://tools.ietf.org/html/bcp47
    //
    // grandfathered = irregular           ; non-redundant tags registered
    //               / regular             ; during the RFC 3066 era
    //  irregular =
    (*map)[String("en-GB-oed")] = String("en-GB-x-oed");
    (*map)[String("i-ami")] = String("ami");
    (*map)[String("i-bnn")] = String("bnn");
    (*map)[String("i-default")] = String("en-x-i-default");
    (*map)[String("i-enochian")] = String("und-x-i-enochian");
    (*map)[String("i-hak")] = String("hak");
    (*map)[String("i-klingon")] = String("tlh");
    (*map)[String("i-lux")] = String("lb");
    (*map)[String("i-mingo")] = String("see-x-i-mingo");
    (*map)[String("i-navajo")] = String("nv");
    (*map)[String("i-pwn")] = String("pwn");
    (*map)[String("i-tao")] = String("tao");
    (*map)[String("i-tay")] = String("tay");
    (*map)[String("i-tsu")] = String("tsu");
    (*map)[String("sgn-BE-FR")] = String("sfb");
    (*map)[String("sgn-BE-NL")] = String("vgt");
    (*map)[String("sgn-CH-DE")] = String("sgg");

    // regular =
    (*map)[String("art-lojban")] = String("jbo");
    (*map)[String("cel-gaulish")] = String("xtg-x-cel-gaulish");
    (*map)[String("no-bok")] = String("nb");
    (*map)[String("no-nyn")] = String("nn");
    (*map)[String("zh-guoyu")] = String("cmn");
    (*map)[String("zh-hakka")] = String("hak");
    (*map)[String("zh-min")] = String("nan-x-zh-min");
    (*map)[String("zh-min-nan")] = String("nan");
    (*map)[String("zh-xiang")] = String("hsn");
    return map;
}

INIT_PROI_3 AutoPtr< StringMap > Locale::GRANDFATHERED_LOCALES = InitGRANDFATHERED_LOCALES();

/**
 * The current default locale. It is temporarily assigned to US because we
 * need a default locale to lookup the real default locale.
 */
INIT_PROI_3 AutoPtr<ILocale> Locale::sDefaultLocale = Locale::US;
Boolean Locale::sIsInited = FALSE;

CAR_INTERFACE_IMPL_3(Locale, Object, ILocale, ICloneable, ISerializable)

Locale::~Locale()
{}

ECode Locale::ForLanguageTag(
    /* [in] */ const String& languageTag,
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = NULL;

    if (languageTag.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    return ForLanguageTag(languageTag, FALSE /* strict */, locale);
}

Locale::Locale()
    : mHasValidatedFields(FALSE)
{}

ECode Locale::constructor(
    /* [in] */ Boolean hasValidatedFields,
    /* [in] */ const String& lowerCaseLanguageCode,
    /* [in] */ const String& upperCaseCountryCode)
{
    mLanguageCode = lowerCaseLanguageCode;
    mCountryCode = upperCaseCountryCode;
    mVariantCode = String("");
    mScriptCode = String("");

    mUnicodeAttributes = Collections::EMPTY_SET;
    mUnicodeKeywords = Collections::EMPTY_MAP;
    mExtensions = Collections::EMPTY_MAP;

    mHasValidatedFields = hasValidatedFields;

    return NOERROR;
}

ECode Locale::constructor()
{
    mLanguageCode = "en";
    mCountryCode = "US";
    mVariantCode = "";
    return NOERROR;
}

ECode Locale::constructor(
    /* [in] */ const String& language)
{
    String empty("");
    return constructor(language, empty, empty, empty,
        Collections::EMPTY_SET,
        Collections::EMPTY_MAP,
        Collections::EMPTY_MAP,
        FALSE /* has validated fields */);
}

ECode Locale::constructor(
    /* [in] */ const String& language,
    /* [in] */ const String& country)
{
    String empty("");
    return constructor(language, country, empty, empty,
        Collections::EMPTY_SET,
        Collections::EMPTY_MAP,
        Collections::EMPTY_MAP,
        FALSE /* has validated fields */);
}

ECode Locale::constructor(
    /* [in] */ const String& language,
    /* [in] */ const String& country,
    /* [in] */ const String& variant)
{
    return constructor(language, country, variant, String(""),
        Collections::EMPTY_SET,
        Collections::EMPTY_MAP,
        Collections::EMPTY_MAP,
        FALSE /* has validated fields */);
}

ECode Locale::constructor(
    /* [in] */ const String& language,
    /* [in] */ const String& country,
    /* [in] */ const String& variant,
    /* [in] */ const String& scriptCode,
    /* [in] */ /* nonnull */ ISet* unicodeAttributes,
    /* [in] */ /* nonnull */ IMap* unicodeKeywords,
    /* [in] */ /* nonnull */ IMap* extensions,
    /* [in] */ Boolean hasValidatedFields)
{
    if (language.IsNull() || country.IsNull() || variant.IsNull()) {
        // throw new NullPointerException("language=" + language +
        //         ",country=" + country +
        //         ",variant=" + variant);
        return E_NULL_POINTER_EXCEPTION;
    }

    if (hasValidatedFields) {
        mLanguageCode = AdjustLanguageCode(language);
        mCountryCode = country;
        mVariantCode = variant;
    }
    else {
        if (language.IsEmpty() && country.IsEmpty()) {
            mLanguageCode = String("");
            mCountryCode =String("");
            mVariantCode = variant;
        }
        else {
            mLanguageCode = AdjustLanguageCode(language);
            mCountryCode = country.ToUpperCase(/*Locale.US*/);
            mVariantCode = variant;
        }
    }

    mScriptCode = scriptCode;

    if (hasValidatedFields) {
        AutoPtr<ISet> attribsCopy;
        CTreeSet::New(ICollection::Probe(unicodeAttributes), (ISet**)&attribsCopy);
        AutoPtr<IMap> keywordsCopy, extensionsCopy;
        CTreeMap::New(unicodeKeywords, (IMap**)&keywordsCopy);
        CTreeMap::New(extensions, (IMap**)&extensionsCopy);

        // We need to transform the list of attributes & keywords set on the
        // builder to a unicode locale extension. i.e, if we have any keywords
        // or attributes set, Locale#getExtension('u') should return a well
        // formed extension.
        AddUnicodeExtensionToExtensionsMap(attribsCopy, keywordsCopy, extensionsCopy);

        Collections::UnmodifiableSet(attribsCopy, (ISet**)&mUnicodeAttributes);
        Collections::UnmodifiableMap(keywordsCopy, (IMap**)&mUnicodeKeywords);
        Collections::UnmodifiableMap(extensionsCopy, (IMap**)&mExtensions);
    }
    else {
        mUnicodeAttributes = unicodeAttributes;
        mUnicodeKeywords = unicodeKeywords;
        mExtensions = extensions;
    }

    mHasValidatedFields = hasValidatedFields;
    return NOERROR;
}

ECode Locale::Clone(
    /* [out] */ IInterface** newObj)
{
    VALIDATE_NOT_NULL(newObj);
    AutoPtr<ILocale> locale;
    CLocale::New((ILocale**)&locale);

    CloneImpl(locale);

    *newObj = locale->Probe(EIID_IInterface);
    REFCOUNT_ADD(*newObj);
    return NOERROR;
}

ECode Locale::CloneImpl(
    /* [in] */ ILocale* locale)
{
    assert(locale);

    CLocale* l = (CLocale*)locale;
    l->mCountryCode = mCountryCode;
    l->mLanguageCode = mLanguageCode;
    l->mVariantCode = mVariantCode;
    l->mScriptCode = mScriptCode;

    l->mUnicodeAttributes = mUnicodeAttributes;
    l->mUnicodeKeywords = mUnicodeKeywords;
    l->mExtensions = mExtensions;

    l->mHasValidatedFields = mHasValidatedFields;

    l->mCachedToStringResult = mCachedToStringResult;
    l->mCachedLanguageTag = mCachedLanguageTag;
    l->mCachedIcuLocaleId = mCachedIcuLocaleId;
    return NOERROR;
}

ECode Locale::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (ILocale::Probe(other) == NULL) {
        return NOERROR;
    }

    if (other == this->Probe(EIID_IInterface)) {
        *result = TRUE;
        return NOERROR;
    }

    Boolean equals;
    CLocale* o = (CLocale*)ILocale::Probe(other);
    *result = mLanguageCode.Equals(o->mLanguageCode)
            && mCountryCode.Equals(o->mCountryCode)
            && mVariantCode.Equals(o->mVariantCode)
            && (mExtensions->Equals(o->mExtensions, &equals), equals);
    return NOERROR;
}

ECode Locale::GetAvailableLocales(
    /* [out] */ ArrayOf<ILocale*>** locales)
{
    return ICUUtil::GetAvailableLocales(locales);
}

ECode Locale::GetCountry(
    /* [out] */ String* country)
{
    VALIDATE_NOT_NULL(country);
    *country = mCountryCode;
    return NOERROR;
}

AutoPtr<ILocale> Locale::GetDefault()
{
    if (!sIsInited) {
        sIsInited = TRUE;

        AutoPtr<Elastos::Core::CSystem> cs;
        Elastos::Core::CSystem::AcquireSingletonByFriend((Elastos::Core::CSystem**)&cs);

        String language, region, variant;
        cs->GetProperty(String("user.language"), String("en"), &language);
        cs->GetProperty(String("user.region"), String("US"), &region);
        cs->GetProperty(String("user.variant"), String(""), &variant);

        AutoPtr<CLocale> l;
        ASSERT_SUCCEEDED(CLocale::NewByFriend(language, region, variant, (CLocale**)&l));
        sDefaultLocale = ILocale::Probe(l);
    }

    return sDefaultLocale;
}

ECode Locale::GetDisplayCountry(
    /* [out] */ String* country)
{
    return GetDisplayCountry(GetDefault(), country);
}

ECode Locale::GetDisplayCountry(
    /* [in] */ ILocale* locale,
    /* [out] */ String* country)
{
    VALIDATE_NOT_NULL(country);

    if (mCountryCode.IsEmpty()) {
        *country = String("");
        return NOERROR;
    }

    String normalizedRegion;
    ECode ec = CLocaleBuilder::NormalizeAndValidateRegion(mCountryCode, FALSE /* strict */, &normalizedRegion);
    FAIL_RETURN(ec)

    if (normalizedRegion.IsEmpty()) {
        *country = mCountryCode;
        return NOERROR;
    }

    String result;
    ICUUtil::GetDisplayCountry(THIS_PROBE(ILocale), locale, &result);
    if (result.IsNull()) { // TODO: do we need to do this, or does ICU do it for us?
        ICUUtil::GetDisplayCountry(THIS_PROBE(ILocale), CLocale::GetDefault(), &result);
    }
    *country = result;
    return NOERROR;
}

ECode Locale::GetDisplayLanguage(
    /* [out] */ String* language)
{
    return GetDisplayLanguage(GetDefault(), language);
}

ECode Locale::GetDisplayLanguage(
    /* [in] */ ILocale* locale,
    /* [out] */ String* language)
{
    VALIDATE_NOT_NULL(language);

    if (mLanguageCode.IsEmpty()) {
        *language = String("");
        return NOERROR;
    }

    // Hacks for backward compatibility.
    //
    // Our language tag will contain "und" if the languageCode is invalid
    // or missing. ICU will then return "langue indéterminée" or the equivalent
    // display language for the indeterminate language code.
    //
    // Sigh... ugh... and what not.
    String normalizedLanguage;
    ECode ec = CLocaleBuilder::NormalizeAndValidateLanguage(mLanguageCode, FALSE /* strict */, &normalizedLanguage);
    FAIL_RETURN(ec);
    if (UNDETERMINED_LANGUAGE.Equals(normalizedLanguage)) {
        *language = mLanguageCode;
        return NOERROR;
    }

    String result;
    ICUUtil::GetDisplayLanguage(THIS_PROBE(ILocale), locale, &result);
    if (result.IsNull()) { // TODO: do we need to do this, or does ICU do it for us?
        ICUUtil::GetDisplayLanguage(THIS_PROBE(ILocale), CLocale::GetDefault(), &result);
    }
    *language = result;
    return NOERROR;
}

ECode Locale::GetDisplayName(
    /* [out] */ String* name)
{
    return GetDisplayName(GetDefault(), name);;
}

ECode Locale::GetDisplayName(
    /* [in] */ ILocale* locale,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    Int32 count = 0;
    StringBuilder buffer;
    if (!mLanguageCode.IsEmpty()) {
        String displayLanguage;
        GetDisplayLanguage(locale, &displayLanguage);
        buffer.Append(displayLanguage.IsEmpty() ? mLanguageCode : displayLanguage);
        ++count;
    }
    if (!mScriptCode.IsEmpty()) {
        if (count == 1) {
            buffer.Append(" (");
        }
        String displayScript;
        GetDisplayScript(locale, &displayScript);
        buffer.Append(displayScript.IsEmpty() ? mScriptCode : displayScript);
        ++count;
    }
    if (!mCountryCode.IsEmpty()) {
        if (count == 1) {
            buffer.Append(" (");
        }
        else if (count == 2) {
            buffer.Append(",");
        }
        String displayCountry;
        GetDisplayCountry(locale, &displayCountry);
        buffer.Append(displayCountry.IsEmpty() ? mCountryCode : displayCountry);
        ++count;
    }
    if (!mVariantCode.IsEmpty()) {
        if (count == 1) {
            buffer.Append(" (");
        }
        else if (count == 2 || count == 3) {
            buffer.Append(",");
        }
        String displayVariant;
        GetDisplayVariant(locale, &displayVariant);
        buffer.Append(displayVariant.IsEmpty() ? mVariantCode : displayVariant);
        ++count;
    }
    if (count > 1) {
        buffer.Append(")");
    }
    return buffer.ToString(name);
}

ECode Locale::GetDisplayVariant(
    /* [out] */ String* variantName)
{
    return GetDisplayVariant(GetDefault(), variantName);;
}

ECode Locale::GetDisplayVariant(
    /* [in] */ ILocale* locale,
    /* [out] */ String* variantName)
{
    VALIDATE_NOT_NULL(variantName);

    if (mVariantCode.IsEmpty()) {
        *variantName = String("");
        return NOERROR;
    }

    // try {
    String result;
    ECode ec = CLocaleBuilder::NormalizeAndValidateVariant(mVariantCode, &result);
    if (ec == (ECode)E_ILLFORMED_LOCALE_EXCEPTION) {
        *variantName = mVariantCode;
        return NOERROR;
    }
    // } catch (IllformedLocaleException ilfe) {
    //      return mVariantCode;
    // }

    result = ICUUtil::GetDisplayVariant(THIS_PROBE(ILocale), locale);
    if (result.IsNull()) { // TODO: do we need to do this, or does ICU do it for us?
        result = ICUUtil::GetDisplayVariant(THIS_PROBE(ILocale), CLocale::GetDefault());
    }
    *variantName = result;
    return NOERROR;
}

ECode Locale::GetISO3Country(
    /* [out] */ String* country)
{
    VALIDATE_NOT_NULL(country);

    // The results of getISO3Country do not depend on the languageCode,
    // so we pass an arbitrarily selected language code here. This guards
    // against errors caused by malformed or invalid language codes.
    String cc("en-");
    cc += mCountryCode;
    String code = ICUUtil::GetISO3Country(cc);
    if (!mCountryCode.IsEmpty() && code.IsEmpty()) {
        ALOGE("CLocale::GetISO3Country: MissingResourceException: No 3-letter country code for locale: %s", mCountryCode.string());
        return E_MISSING_RESOURCE_EXCEPTION;
    }
    *country = code;
    return NOERROR;
}

ECode Locale::GetISO3Language(
    /* [out] */ String* code)
{
    VALIDATE_NOT_NULL(code);

    if (mLanguageCode.IsNullOrEmpty()) {
        *code = String("");
        return NOERROR;
    }

    // The results of getISO3Language do not depend on the country code
    // or any of the other locale fields, so we pass just the language here.
    String value = ICUUtil::GetISO3Language(mLanguageCode);
    if (!mLanguageCode.IsEmpty() && value.IsEmpty()) {
        ALOGE("CLocale::GetISO3Language: MissingResourceException: No 3-letter country code for locale: %s", mCountryCode.string());
        return E_MISSING_RESOURCE_EXCEPTION;
    }
    *code = value;
    return NOERROR;
}

ECode Locale::GetISOCountries(
    /* [out] */ ArrayOf<String>** codes)
{
    VALIDATE_NOT_NULL(codes);
    return ICUUtil::GetISOCountries(codes);
}

ECode Locale::GetISOLanguages(
    /* [out] */ ArrayOf<String>** codes)
{
    VALIDATE_NOT_NULL(codes);
    return ICUUtil::GetISOLanguages(codes);
}

ECode Locale::GetLanguage(
    /* [out] */ String* language)
{
    VALIDATE_NOT_NULL(language);
    *language = mLanguageCode;
    return NOERROR;
}

ECode Locale::GetVariant(
    /* [out] */ String* variant)
{
    VALIDATE_NOT_NULL(variant);
    *variant = mVariantCode;
    return NOERROR;
}

ECode Locale::GetScript(
    /* [out] */ String* script)
{
    VALIDATE_NOT_NULL(script);
    *script = mScriptCode;
    return NOERROR;
}

ECode Locale::GetDisplayScript(
    /* [out] */ String* script)
{
    VALIDATE_NOT_NULL(script);
    AutoPtr<ILocale> locale = GetDefault();
    return GetDisplayScript(locale, script);
}

ECode Locale::GetDisplayScript(
    /* [in] */ ILocale* locale,
    /* [out] */ String* script)
{
    VALIDATE_NOT_NULL(script);
    *script = String("");

    if (mScriptCode.IsEmpty()) {
        return NOERROR;
    }

    // TODO upgrade
    // String result = ICUUtil::GetDisplayScript(THIS_PROBE(ILocale), locale);
    // if (result.IsNull()) { // TODO: do we need to do this, or does ICU do it for us?
    //     AutoPtr<ILocale> locale = GetDefault();
    //     result = ICUUtil::GetDisplayScript(THIS_PROBE(ILocale),locale);
    // }

    // *script = result;
    return NOERROR;
}

ECode Locale::ToLanguageTag(
    /* [out] */ String* tag)
{
    VALIDATE_NOT_NULL(tag);
    if (mCachedLanguageTag.IsNull()) {
        mCachedLanguageTag = MakeLanguageTag();
    }

    *tag = mCachedLanguageTag;
    return NOERROR;
}

String Locale::MakeLanguageTag()
{
    // We only need to revalidate the language, country and variant because
    // the rest of the fields can only be set via the builder which validates
    // them anyway.
    String language("");
    String region("");
    String variant("");
    String illFormedVariantSubtags("");

    if (mHasValidatedFields) {
        language = mLanguageCode;
        region = mCountryCode;
        // Note that we are required to normalize hyphens to underscores
        // in the builder, but we must use hyphens in the BCP-47 language tag.
        variant = mVariantCode.Replace('_', '-');
    }
    else {
        CLocaleBuilder::NormalizeAndValidateLanguage(mLanguageCode, FALSE /* strict */, &language);
        CLocaleBuilder::NormalizeAndValidateRegion(mCountryCode, FALSE /* strict */, &region);

        // try {
        ECode ec = CLocaleBuilder::NormalizeAndValidateVariant(mVariantCode, &variant);
        // } catch (IllformedLocaleException ilfe)
        if (ec == (ECode)E_ILLFORMED_LOCALE_EXCEPTION) {
            // If our variant is ill formed, we must attempt to split it into
            // its constituent subtags and preserve the well formed bits and
            // move the rest to the private use extension (if they're well
            // formed extension subtags).
            AutoPtr<ArrayOf<String> > split = SplitIllformedVariant(mVariantCode);

            variant = (*split)[0];
            illFormedVariantSubtags = (*split)[1];
        }
    }

    if (language.IsEmpty()) {
        language = UNDETERMINED_LANGUAGE;
    }

    if (language.Equals("no") && region.Equals("NO") && variant.Equals("NY")) {
        language = "nn";
        region = "NO";
        variant = "";
    }

    StringBuilder sb(16);
    sb.Append(language);

    if (!mScriptCode.IsEmpty()) {
        sb.AppendChar('-');
        sb.Append(mScriptCode);
    }

    if (!region.IsEmpty()) {
        sb.AppendChar('-');
        sb.Append(region);
    }

    if (!variant.IsEmpty()) {
        sb.AppendChar('-');
        sb.Append(variant);
    }

    // Extensions (optional, omitted if empty). Note that we don't
    // emit the private use extension here, but add it in the end.
    AutoPtr<ISet> entrySet;
    mExtensions->GetEntrySet((ISet**)&entrySet);
    AutoPtr<IIterator> it;
    entrySet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    String str;
    AutoPtr<IMapEntry> extension;
    AutoPtr<ICharSequence> csq;
    AutoPtr<IInterface> obj;
    while (it->HasNext(&hasNext), hasNext) {
        it->GetNext((IInterface**)&obj);
        extension = IMapEntry::Probe(obj);

        obj = NULL;
        extension->GetKey((IInterface**)&obj);
        str = Object::ToString(obj);
        if (!str.Equals("x")) {
            sb.AppendChar('-');
            sb.Append(str);
            sb.AppendChar('-');

            obj = NULL;
            extension->GetValue((IInterface**)&obj);
            str = Object::ToString(obj);
            sb.Append(str);
        }
        obj = NULL;
    }

    // The private use extension comes right at the very end.
    csq = NULL;
    CString::New(String("x"), (ICharSequence**)&csq);
    obj = NULL;
    mExtensions->Get(csq, (IInterface**)&obj);
    String privateUse;
    if (obj != NULL) {
        privateUse = Object::ToString(obj);
    }
    if (!privateUse.IsNull()) {
        sb.Append("-x-");
        sb.Append(privateUse);
    }

    // If we have any ill-formed variant subtags, we append them to the
    // private use extension (or add a private use extension if one doesn't
    // exist).
    if (!illFormedVariantSubtags.IsEmpty()) {
        if (!privateUse.IsNull()) {
            sb.Append("-x-lvariant-");
        } else {
            sb.AppendChar('-');
        }
        sb.Append(illFormedVariantSubtags);
    }

    return sb.ToString();
}

AutoPtr<ArrayOf<String> > Locale::SplitIllformedVariant(
    /* [in] */ const String& variant)
{
    String normalizedVariant = variant.Replace('_', '-');
    AutoPtr<ArrayOf<String> > subTags;
    StringUtils::Split(normalizedVariant, String("-"), (ArrayOf<String> **)&subTags);

    AutoPtr<ArrayOf<String> > split = ArrayOf<String>::Alloc(2);
    split->Set(0, String(""));
    split->Set(1, String(""));

    // First go through the list of variant subtags and check if they're
    // valid private use extension subtags. If they're not, we will omit
    // the first such subtag and all subtags after.
    //
    // NOTE: |firstInvalidSubtag| is the index of the first variant
    // subtag we decide to omit altogether, whereas |firstIllformedSubtag| is the
    // index of the first subtag we decide to append to the private use extension.
    //
    // In other words:
    // [0, firstIllformedSubtag) => expressed as variant subtags.
    // [firstIllformedSubtag, firstInvalidSubtag) => expressed as private use
    // extension subtags.
    // [firstInvalidSubtag, subTags.length) => omitted.
    Int32 firstInvalidSubtag = subTags->GetLength();
    for (Int32 i = 0; i < subTags->GetLength(); ++i) {
        if (!IsValidBcp47Alphanum((*subTags)[i], 1, 8)) {
            firstInvalidSubtag = i;
            break;
        }
    }

    if (firstInvalidSubtag == 0) {
        return split;
    }

    // We now consider each subtag that could potentially be appended to
    // the private use extension and check if it's valid.
    Int32 firstIllformedSubtag = firstInvalidSubtag;
    for (Int32 i = 0; i < firstInvalidSubtag; ++i) {
        String subTag = (*subTags)[i];
        // The BCP-47 spec states that :
        // - Subtags can be between [5, 8] alphanumeric chars in length.
        // - Subtags that start with a number are allowed to be 4 chars in length.
        if (subTag.GetLength() >= 5 && subTag.GetLength() <= 8) {
            if (!IsAsciiAlphaNum(subTag)) {
                firstIllformedSubtag = i;
            }
        }
        else if (subTag.GetLength() == 4) {
            Char32 firstChar = subTag.GetChar(0);
            if (!(firstChar >= '0' && firstChar <= '9') || !IsAsciiAlphaNum(subTag)) {
                firstIllformedSubtag = i;
            }
        }
        else {
            firstIllformedSubtag = i;
        }
    }

    split->Set(0, ConcatenateRange(subTags, 0, firstIllformedSubtag));
    split->Set(1, ConcatenateRange(subTags, firstIllformedSubtag, firstInvalidSubtag));

    return split;
}

String Locale::ConcatenateRange(
    /* [in] */ ArrayOf<String>* array,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    StringBuilder builder(32);
    for (Int32 i = start; i < end; ++i) {
        if (i != start) {
            builder.AppendChar('-');
        }
        builder.Append((*array)[i]);
    }

    return builder.ToString();
}

ECode Locale::GetExtensionKeys(
    /* [out] */ ISet** keys)
{
    return mExtensions->GetKeySet(keys);
}

ECode Locale::GetExtension(
    /* [in] */ Char32 extensionKey,
    /* [out] */ String* extension)
{
    VALIDATE_NOT_NULL(extension);
    AutoPtr<IChar32> ch;
    CChar32::New(extensionKey, (IChar32**)&ch);
    AutoPtr<IInterface> obj;
    mExtensions->Get(ch, (IInterface**)&obj);
    ICharSequence* csq = ICharSequence::Probe(obj);
    return csq->ToString(extension);
}

ECode Locale::GetUnicodeLocaleType(
    /* [in] */ const String& keyWord,
    /* [out] */ String* type)
{
    VALIDATE_NOT_NULL(type);
    AutoPtr<ICharSequence> key;
    CString::New(keyWord, (ICharSequence**)&key);
    AutoPtr<IInterface> obj;
    mExtensions->Get(key, (IInterface**)&obj);
    ICharSequence* csq = ICharSequence::Probe(obj);
    return csq->ToString(type);
}

ECode Locale::GetUnicodeLocaleAttributes(
    /* [out] */ ISet** keys)
{
    VALIDATE_NOT_NULL(keys);
    *keys = mUnicodeAttributes;
    REFCOUNT_ADD(*keys)
    return NOERROR;
}

ECode Locale::GetUnicodeLocaleKeys(
    /* [out] */ ISet** keys)
{
    return mUnicodeKeywords->GetKeySet(keys);
}

ECode Locale::GetHashCode(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mCountryCode.GetHashCode()
        + mLanguageCode.GetHashCode()
        + mVariantCode.GetHashCode()
        + mScriptCode.GetHashCode()
        + mVariantCode.GetHashCode()
        + Object::GetHashCode(mExtensions);
    return NOERROR;
}

ECode Locale::SetDefault(
    /* [in] */ ILocale* locale)
{
    if (locale == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    String languageTag;
    locale->ToLanguageTag(&languageTag);
    sDefaultLocale = locale;
    ICUUtil::SetDefaultLocale(languageTag);
    return NOERROR;
}

ECode Locale::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    String result = mCachedToStringResult;
    *str = result.IsNull() ? (mCachedToStringResult = ToNewString(
        mLanguageCode, mCountryCode, mVariantCode, mScriptCode, mExtensions)) : result;
    return NOERROR;
}

String Locale::ToNewString(
    /* [in] */ const String& languageCode,
    /* [in] */ const String& countryCode,
    /* [in] */ const String& variantCode,
    /* [in] */ const String& scriptCode,
    /* [in] */ IMap* extensions)
{
    // The string form of a locale that only has a variant is the empty string.
    if (languageCode.IsNullOrEmpty() || countryCode.IsNullOrEmpty()) {
        return String("");
    }

    // Otherwise, the output format is "ll_cc_variant", where language and country are always
    // two letters, but the variant is an arbitrary length. A size of 11 characters has room
    // for "en_US_POSIX", the largest "common" value. (In practice, the string form is almost
    // always 5 characters: "ll_cc".)
    StringBuilder result(11);
    result.Append(languageCode);

    Boolean empty;
    Boolean hasScriptOrExtensions = !scriptCode.IsNullOrEmpty() || !(extensions->IsEmpty(&empty), empty);

    if (!countryCode.IsNullOrEmpty() || !variantCode.IsNullOrEmpty() || hasScriptOrExtensions) {
         result.AppendChar('_');
    }
    result.Append(countryCode);

    if (!variantCode.IsNullOrEmpty() || hasScriptOrExtensions) {
        result.AppendChar('_');
    }
    result.Append(variantCode);

    if (hasScriptOrExtensions) {
        if (!variantCode.IsNullOrEmpty()) {
            result.AppendChar('_');
        }

        // Note that this is notably different from the BCP-47 spec (for
        // backwards compatibility). We are forced to append a "#" before the script tag.
        // and also put the script code right at the end.
        result.AppendChar('#');
        if (!scriptCode.IsNullOrEmpty() ) {
            result.Append(scriptCode);
        }

        // Note the use of "-" instead of "_" before the extensions.
        if (!(extensions->IsEmpty(&empty), empty)) {
            if (!scriptCode.IsNullOrEmpty()) {
                result.AppendChar('-');
            }
            result.Append(SerializeExtensions(extensions));
        }
    }

    return result.ToString();
}


ECode Locale::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    assert(0 && "TODO");
    // ObjectOutputStream.PutField fields = stream.putFields();
    // fields.put("country", countryCode);
    // fields.put("hashcode", -1);
    // fields.put("language", languageCode);
    // fields.put("variant", variantCode);
    // fields.put("script", scriptCode);

    // if (!extensions.IsEmpty()) {
    //     fields.put("extensions", serializeExtensions(extensions));
    // }

    // stream.writeFields();
    return NOERROR;
}

ECode Locale::ReadObject(
    /* [in] */ IObjectOutputStream* stream)
{
    assert(0 && "TODO");
    // ObjectInputStream.GetField fields = stream.readFields();
    // countryCode = (String) fields.get("country", "");
    // languageCode = (String) fields.get("language", "");
    // variantCode = (String) fields.get("variant", "");
    // scriptCode = (String) fields.get("script", "");

    // municodeKeywords = Collections::EMPTY_MAP;
    // municodeAttributes = Collections::EMPTY_SET;
    // mextensions = Collections::EMPTY_MAP;

    // String extensions = (String) fields.get("extensions", null);
    // if (extensions != null) {
    //     readExtensions(extensions);
    // }
    return NOERROR;
}

ECode Locale::ReadExtensions(
    /* [in] */ const String& extensions)
{
    assert(0 && "TODO");
    // Map<Character, String> extensionsMap = new TreeMap<Character, String>();
    // parseSerializedExtensions(extensions, extensionsMap);
    // mextensions = Collections.unmodifiableMap(extensionsMap);

    // if (extensionsMap.containsKey(UNICODE_LOCALE_EXTENSION)) {
    //     String unicodeExtension = extensionsMap.get(UNICODE_LOCALE_EXTENSION);
    //     String[] subTags = unicodeExtension.split("-");

    //     Map<String, String> unicodeKeywords = new TreeMap<String, String>();
    //     Set<String> unicodeAttributes = new TreeSet<String>();
    //     parseUnicodeExtension(subTags, unicodeKeywords, unicodeAttributes);

    //     municodeKeywords = Collections.unmodifiableMap(unicodeKeywords);
    //     municodeAttributes = Collections.unmodifiableSet(unicodeAttributes);
    // }
    return NOERROR;
}

String Locale::SerializeExtensions(
    /* [in] */ IMap* extensionsMap)
{
    assert(0 && "TODO");
    // Iterator<Map.Entry<Character, String>> entryIterator = extensionsMap.entrySet().iterator();
    StringBuilder sb(64);

    // while (true) {
    //     Map.Entry<Character, String> entry = entryIterator.next();
    //     sb.Append(entry.getKey());
    //     sb.AppendChar('-');
    //     sb.Append(entry.getValue());

    //     if (entryIterator.hasNext()) {
    //         sb.AppendChar('-');
    //     } else {
    //         break;
    //     }
    // }

    return sb.ToString();
}

ECode Locale::ParseSerializedExtensions(
        /* [in] */ const String& extString,
        /* [in] */ IMap* outputMap)
{
    assert(0 && "TODO");
    // This probably isn't the most efficient approach, but it's the
    // most straightforward to code.
    //
    // Start by splitting the string on "-". We will then keep track of
    // where each of the extension keys (single characters) appear in the
    // original string and then use those indices to construct substrings
    // representing the values.
    // String[] subTags = extString.split("-");
    // int[] typeStartIndices = new int[subTags.length / 2];

    // Int32 length = 0;
    // Int32 count = 0;
    // for (String subTag : subTags) {
    //     if (subTag.length() > 0) {
    //         // Account for the length of the "-" at the end of each subtag.
    //         length += (subTag.length() + 1);
    //     }

    //     if (subTag.length() == 1) {
    //         typeStartIndices[count++] = length;
    //     }
    // }

    // for (Int32 i = 0; i < count; ++i) {
    //     Int32 valueStart = typeStartIndices[i];
    //     // Since the start Index points to the beginning of the next type
    //     // ....prev-k-next.....
    //     //            |_ here
    //     // (idx - 2) is the index of the next key
    //     // (idx - 3) is the (non inclusive) end of the previous type.
    //     Int32 valueEnd = (i == (count - 1)) ?
    //             extString.length() : (typeStartIndices[i + 1] - 3);

    //     outputMap.put(extString.charAt(typeStartIndices[i] - 2),
    //             extString.substring(valueStart, valueEnd));
    // }
    return NOERROR;
}

Boolean Locale::IsUnM49AreaCode(
    /* [in] */ const String& code)
{
    if (code.GetLength() != 3) {
        return FALSE;
    }

    for (Int32 i = 0; i < 3; ++i) {
        Char32 character = code.GetChar(i);
        if (!(character >= '0' && character <= '9')) {
            return FALSE;
        }
    }

    return TRUE;
}

Boolean Locale::IsAsciiAlphaNum(
    /* [in] */ const String& string)
{
    AutoPtr<ArrayOf<Char32> > chars = string.GetChars();
    for (Int32 i = 0; i < chars->GetLength(); i++) {
        Char32 character = (*chars)[i];
        if (!((character >= 'a' && character <= 'z') ||
                (character >= 'A' && character <= 'Z') ||
                (character >= '0' && character <= '9'))) {
            return FALSE;
        }
    }

    return TRUE;
}

Boolean Locale::IsValidBcp47Alpha(
    /* [in] */ const String& attributeOrType,
    /* [in] */ Int32 lowerBound,
    /* [in] */ Int32 upperBound)
{
    AutoPtr<ArrayOf<Char32> > chars = attributeOrType.GetChars();
    Int32 length = chars->GetLength();
    if (length < lowerBound || length > upperBound) {
        return FALSE;
    }

    for (Int32 i = 0; i < length; ++i) {
        Char32 character = (*chars)[i];
        if (!((character >= 'a' && character <= 'z') ||
                (character >= 'A' && character <= 'Z'))) {
            return FALSE;
        }
    }

    return TRUE;
}

Boolean Locale::IsValidBcp47Alphanum(
    /* [in] */ const String& attributeOrType,
    /* [in] */ Int32 lowerBound,
    /* [in] */ Int32 upperBound)
{
    if (attributeOrType.GetLength() < lowerBound
        || attributeOrType.GetLength() > upperBound) {
        return FALSE;
    }

    return IsAsciiAlphaNum(attributeOrType);
}

ECode Locale::TitleCaseAsciiWord(
    /* [in] */ const String& word,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    // try {
    String wl = word.ToLowerCase(/*Locale.ROOT*/);
    AutoPtr<ArrayOf<Byte> > chars = wl.GetBytes(/*StandardCharsets.US_ASCII*/);
    (*chars)[0] = (byte) ((Int32) (*chars)[0] + 'A' - 'a');
    *str = String((const char*)chars->GetPayload(), chars->GetLength());
    return NOERROR;
    // } catch (UnsupportedOperationException uoe) {
    //     throw new AssertionError(uoe);
    // }
}

Boolean Locale::IsValidTypeList(
    /* [in] */ const String& lowerCaseTypeList)
{
    AutoPtr<ArrayOf<String> > splitList;
    StringUtils::Split(lowerCaseTypeList, String("-"), (ArrayOf<String>**)&splitList);

    for (Int32 i = 0; i < splitList->GetLength(); ++i) {
        if (!IsValidBcp47Alphanum((*splitList)[i], 3, 8)) {
            return FALSE;
        }
    }

    return TRUE;
}

void Locale::AddUnicodeExtensionToExtensionsMap(
    /* [in] */ ISet* attributes,
    /* [in] */ IMap* keywords,
    /* [in] */ IMap* extensions)
{
    assert(attributes && keywords && extensions);

    Boolean a, b;
    attributes->IsEmpty(&a);
    keywords->IsEmpty(&b);
    if (a && b) {
        return;
    }

    // Assume that the common case is a low number of keywords & attributes
    // (usually one or two).
    StringBuilder sb(32);

    // All attributes must appear before keywords, in lexical order.
    if (!a) {
        Boolean hasNext;
        AutoPtr<IIterator> attributesIterator;
        attributes->GetIterator((IIterator**)&attributesIterator);
        while (TRUE) {
            AutoPtr<IInterface> obj;
            attributesIterator->GetNext((IInterface**)&obj);
            String value = Object::ToString(obj);
            sb.Append(value);

            if (attributesIterator->HasNext(&hasNext), hasNext) {
                sb.AppendChar('-');
            }
            else {
                break;
            }
        }
    }

    if (!b) {
        if (!a) {
            sb.AppendChar('-');
        }

        Boolean hasNext;
        AutoPtr<ISet> entrySet;
        keywords->GetEntrySet((ISet**)&entrySet);
        AutoPtr<IIterator> keywordsIterator;
        entrySet->GetIterator((IIterator**)&keywordsIterator);

        while (TRUE) {
            AutoPtr<IInterface> obj;
            keywordsIterator->GetNext((IInterface**)&obj);
            IMapEntry* entry = IMapEntry::Probe(obj);
            AutoPtr<IInterface> key, value;
            entry->GetKey((IInterface**)&key);
            entry->GetValue((IInterface**)&value);
            String ks = Object::ToString(key);
            String vs = Object::ToString(key);
            sb.Append(ks);
            if (!vs.IsEmpty()) {
                sb.AppendChar('-');
                sb.Append(vs);
            }

            if (keywordsIterator->HasNext(&hasNext), hasNext) {
                sb.AppendChar('-');
            }
            else {
                break;
            }
        }
    }

    AutoPtr<IChar32> ch;
    CChar32::New(ILocale::UNICODE_LOCALE_EXTENSION, (IChar32**)&ch);
    AutoPtr<ICharSequence> csq;
    CString::New(sb.ToString(), (ICharSequence**)&csq);
    extensions->Put(TO_IINTERFACE(ch), TO_IINTERFACE(csq));
}

void Locale::ParseUnicodeExtension(
    /* [in] */ ArrayOf<String>* subtags,
    /* [in] */ IMap* keywords,
    /* [in] */ ISet* attributes)
{
    String lastKeyword;
    AutoPtr<IList> subtagsForKeyword;
    CArrayList::New((IList**)&subtagsForKeyword);
    ICollection * c = ICollection::Probe(subtagsForKeyword);
    ICollection * a = ICollection::Probe(attributes);
    Int32 size;
    for (Int32 i = 0; i < subtags->GetLength(); ++i) {
        String subtag = (*subtags)[i];
        if (subtag.GetLength() == 2) {
            c->GetSize(&size);
            if (size > 0) {
                String jbs = JoinBcp47Subtags(subtagsForKeyword);
                AutoPtr<ICharSequence> c1 = CoreUtils::Convert(lastKeyword);
                AutoPtr<ICharSequence> c2 = CoreUtils::Convert(jbs);
                keywords->Put(TO_IINTERFACE(c1), TO_IINTERFACE(c2));
                c->Clear();
            }

            lastKeyword = subtag;
        }
        else if (subtag.GetLength() > 2) {
            AutoPtr<ICharSequence> c1 = CoreUtils::Convert(subtag);
            if (lastKeyword.IsNull()) {
                a->Add(c1);
            }
            else {
                c->Add(c1);
            }
        }
    }

    c->GetSize(&size);
    if (size > 0) {
        String jbs = JoinBcp47Subtags(subtagsForKeyword);
        AutoPtr<ICharSequence> c1 = CoreUtils::Convert(lastKeyword);
        AutoPtr<ICharSequence> c2 = CoreUtils::Convert(jbs);
        keywords->Put(TO_IINTERFACE(c1), TO_IINTERFACE(c2));
    }
    else if (!lastKeyword.IsNull()) {
        AutoPtr<ICharSequence> c1 = CoreUtils::Convert(lastKeyword);
        AutoPtr<ICharSequence> c2 = CoreUtils::Convert(String(""));
        keywords->Put(TO_IINTERFACE(c1), TO_IINTERFACE(c2));
    }
}

String Locale::JoinBcp47Subtags(
    /* [in] */ IList* strings)
{
    assert(strings);
    Int32 size;
    strings->GetSize(&size);

    AutoPtr<IInterface> obj;
    strings->Get(0, (IInterface**)&obj);
    String info = Object::ToString(obj);
    StringBuilder sb(info.GetByteLength());
    for (Int32 i = 0; i < size; ++i) {
        obj = NULL;
        strings->Get(i, (IInterface**)&obj);
        String info = Object::ToString(obj);

        sb.Append(info);
        if (i != size - 1) {
            sb.AppendChar('-');
        }
    }

    return sb.ToString();
}

String Locale::AdjustLanguageCode(
    /* [in] */ const String& languageCode)
{
    String adjusted = languageCode.ToLowerCase(/*Locale.US*/);
    // Map new language codes to the obsolete language
    // codes so the correct resource bundles will be used.
    if (languageCode.Equals("he")) {
        adjusted = String("iw");
    } else if (languageCode.Equals("id")) {
        adjusted = String("in");
    } else if (languageCode.Equals("yi")) {
        adjusted = String("ji");
    }

    return adjusted;
}

String Locale::ConvertGrandfatheredTag(
    /* [in] */ const String& original)
{
    StringMapIterator it = GRANDFATHERED_LOCALES->Find(original);
    if (it != GRANDFATHERED_LOCALES->End()) {
        return it->mSecond;
    }
    return original;
}

void Locale::ExtractVariantSubtags(
    /* [in] */ ArrayOf<String>* subtags,
    /* [in] */ Int32 startIndex,
    /* [in] */ Int32 endIndex,
    /* [in] */ IList* normalizedVariants)
{
    for (Int32 i = startIndex; i < endIndex; i++) {
        String subtag = (*subtags)[i];

        if (CLocaleBuilder::IsValidVariantSubtag(subtag)) {
            AutoPtr<ICharSequence> csq = CoreUtils::Convert(subtag);
            normalizedVariants->Add(csq);
        }
        else {
            break;
        }
    }
}

Int32 Locale::ExtractExtensions(
    /* [in] */ ArrayOf<String>* subtags,
    /* [in] */ Int32 startIndex,
    /* [in] */ Int32 endIndex,
    /* [in] */ IMap* extensions)
{
    Int32 privateUseExtensionIndex = -1;
    Int32 extensionKeyIndex = -1;

    Boolean contains;
    Int32 i = startIndex;
    for (; i < endIndex; i++) {
        String subtag = (*subtags)[i];

        Boolean parsingPrivateUse = (privateUseExtensionIndex != -1) &&
                (extensionKeyIndex == privateUseExtensionIndex);

        // Note that private use extensions allow subtags of length 1.
        // Private use extensions *must* come last, so there's no ambiguity
        // in that case.
        if (subtag.GetLength() == 1 && !parsingPrivateUse) {
            // Emit the last extension we encountered if any. First check
            // whether we encountered two keys in a row (which is an error).
            // Also checks if we already have an extension with the same key,
            // which is again an error.
            if (extensionKeyIndex != -1) {
                if ((i - 1) == extensionKeyIndex) {
                    return extensionKeyIndex;
                }

                String key = (*subtags)[extensionKeyIndex];
                AutoPtr<IChar32> ch = CoreUtils::ConvertChar32(key.GetChar(0));
                if (extensions->ContainsKey(TO_IINTERFACE(ch), &contains), contains) {
                    return extensionKeyIndex;
                }

                String value = ConcatenateRange(subtags, extensionKeyIndex + 1, i);
                AutoPtr<ICharSequence> csq = CoreUtils::Convert(value.ToLowerCase(/*Locale.ROOT*/));
                extensions->Put(TO_IINTERFACE(ch), TO_IINTERFACE(csq));
            }

            // Mark the start of the next extension. Also keep track of whether this
            // is a private use extension, and throw an error if it doesn't come last.
            extensionKeyIndex = i;
            if (subtag.Equals("x")) {
                privateUseExtensionIndex = i;
            }
            else if (privateUseExtensionIndex != -1) {
                // The private use extension must come last.
                return privateUseExtensionIndex;
            }
        } else if (extensionKeyIndex != -1) {
            // We must have encountered a valid key in order to start parsing
            // its subtags.
            if (!IsValidBcp47Alphanum(subtag, parsingPrivateUse ? 1 : 2, 8)) {
                return i;
            }
        } else {
            // Encountered a value without a preceding key.
            return i;
        }
    }

    if (extensionKeyIndex != -1) {
        if ((i - 1) == extensionKeyIndex) {
            return extensionKeyIndex;
        }

        String key = (*subtags)[extensionKeyIndex];
        AutoPtr<IChar32> ch = CoreUtils::ConvertChar32(key.GetChar(0));
        if (extensions->ContainsKey(TO_IINTERFACE(ch), &contains), contains) {
            return extensionKeyIndex;
        }

        String value = ConcatenateRange(subtags, extensionKeyIndex + 1, i);
        AutoPtr<ICharSequence> csq = CoreUtils::Convert(value.ToLowerCase(/*Locale.ROOT*/));
        extensions->Put(TO_IINTERFACE(ch), TO_IINTERFACE(csq));
    }

    return i;
}

ECode Locale::ForLanguageTag(
    /* @Nonnull */ /* [in] */ const String& tag,
    /* [in] */ Boolean strict,
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale)
    *locale = NULL;

    if (tag.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String converted = ConvertGrandfatheredTag(tag);
    AutoPtr<ArrayOf<String> > subtags;
    StringUtils::Split(converted, "-", (ArrayOf<String>**)&subtags);

    Int32 lastSubtag = subtags->GetLength();
    for (Int32 i = 0; i < subtags->GetLength(); ++i) {
        String subtag = (*subtags)[i];
        if (subtag.IsEmpty() || subtag.GetLength() > 8) {
            if (strict) {
                // throw new IllformedLocaleException("Invalid subtag at index: " + i
                //         + " in tag: " + tag);
                return E_ILLFORMED_LOCALE_EXCEPTION;
            } else {
                lastSubtag = (i - 1);
            }

            break;
        }
    }

    String languageCode;
    FAIL_RETURN(CLocaleBuilder::NormalizeAndValidateLanguage(
        (*subtags)[0], strict, &languageCode))
    String scriptCode("");
    Int32 nextSubtag = 1;
    if (lastSubtag > nextSubtag) {
        FAIL_RETURN(CLocaleBuilder::NormalizeAndValidateScript(
            (*subtags)[nextSubtag], FALSE /* strict */, &scriptCode))
        if (!scriptCode.IsEmpty()) {
            nextSubtag++;
        }
    }

    String regionCode("");
    if (lastSubtag > nextSubtag) {
        FAIL_RETURN(CLocaleBuilder::NormalizeAndValidateRegion(
            (*subtags)[nextSubtag], FALSE /* strict */, &regionCode))
        if (!regionCode.IsEmpty()) {
            nextSubtag++;
        }
    }

    AutoPtr<IList> variants;
    if (lastSubtag > nextSubtag) {
        CArrayList::New((IList**)&variants);
        ExtractVariantSubtags(subtags, nextSubtag, lastSubtag, variants);
        Int32 size;
        variants->GetSize(&size);
        nextSubtag += size;
    }

    AutoPtr<IMap> extensions = Collections::EMPTY_MAP;
    if (lastSubtag > nextSubtag) {
        extensions = NULL;
        CTreeMap::New((IMap**)&extensions);
        nextSubtag = ExtractExtensions(subtags, nextSubtag, lastSubtag, extensions);
    }

    if (nextSubtag != lastSubtag) {
        if (strict) {
            // throw new IllformedLocaleException("Unparseable subtag: " + subtags[nextSubtag]
            //         + " from language tag: " + tag);
            return E_ILLFORMED_LOCALE_EXCEPTION;
        }
    }

    Boolean contains;
    AutoPtr<IChar32> ch = CoreUtils::ConvertChar32(ILocale::UNICODE_LOCALE_EXTENSION);
    AutoPtr<ISet> unicodeKeywords = Collections::EMPTY_SET;
    AutoPtr<IMap> unicodeAttributes = Collections::EMPTY_MAP;
    if (extensions->ContainsKey(TO_IINTERFACE(ch), &contains), contains) {
        unicodeKeywords = NULL;
        unicodeAttributes = NULL;
        CTreeSet::New((ISet**)&unicodeKeywords);
        CTreeMap::New((IMap**)&unicodeAttributes);
        AutoPtr<IInterface> obj;
        extensions->Get(TO_IINTERFACE(ch), (IInterface**)&obj);
        String str = Object::ToString(obj);
        AutoPtr<ArrayOf<String> > strs;
        StringUtils::Split(str, "-", (ArrayOf<String> **)&strs);
        ParseUnicodeExtension(strs, unicodeAttributes, unicodeKeywords);
    }

    String variantCode("");
    if (variants != NULL && (variants->IsEmpty(&contains), !contains)) {
        Int32 size;
        variants->GetSize(&size);
        StringBuilder variantsBuilder(size * 8);
        for (Int32 i = 0; i < size; ++i) {
            if (i != 0) {
                variantsBuilder.AppendChar('_');
            }
            AutoPtr<IInterface> obj;
            variants->Get(i, (IInterface**)&obj);
            variantsBuilder.Append(Object::ToString(obj));
        }
        variantCode = variantsBuilder.ToString();
    }

    AutoPtr<CLocale> l;
    CLocale::NewByFriend((CLocale**)&l);
    l->constructor(languageCode, regionCode, variantCode, scriptCode,
            unicodeKeywords, unicodeAttributes, extensions, TRUE /* has validated fields */);
    *locale = (ILocale*)l.Get();
    REFCOUNT_ADD(*locale)
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos
