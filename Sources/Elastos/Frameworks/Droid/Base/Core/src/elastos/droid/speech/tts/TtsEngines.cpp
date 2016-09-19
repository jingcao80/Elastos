#include "elastos/droid/speech/tts/TtsEngines.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/content/pm/CResolveInfo.h"
#include "elastos/droid/content/pm/CServiceInfo.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/speech/tts/TextToSpeech.h"
#include "elastos/droid/R.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::CServiceInfo;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Provider::Settings;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Text::TextUtils;
using Elastos::Core::CString;
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Core::IComparator;
using Elastos::Core::EIID_IComparator;
using Elastos::Utility::CLocale;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Speech {
namespace Tts {

/******************
 * TtsEngines::EngineInfoComparator
 *******************************************************************************************************/

AutoPtr<TtsEngines::EngineInfoComparator> TtsEngines::EngineInfoComparator::INSTANCE = new TtsEngines::EngineInfoComparator();

CAR_INTERFACE_IMPL(TtsEngines::EngineInfoComparator, Object, IComparator);

TtsEngines::EngineInfoComparator::EngineInfoComparator()
{}

TtsEngines::EngineInfoComparator::~EngineInfoComparator()
{}

ECode TtsEngines::EngineInfoComparator::constructor()
{
    return NOERROR;
}

ECode TtsEngines::EngineInfoComparator::Compare(
    /* [in] */ IInterface* object1,
    /* [in] */ IInterface* object2,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    TextToSpeech::TextToSpeechEngineInfo * lhs, *rhs;

    lhs = (TextToSpeech::TextToSpeechEngineInfo*)ITextToSpeechEngineInfo::Probe(object1);
    rhs = (TextToSpeech::TextToSpeechEngineInfo*)ITextToSpeechEngineInfo::Probe(object2);
    if (lhs->system && !rhs->system) {
        *result = -1;
    }
    else if (rhs->system && !lhs->system) {
        *result = 1;
    }
    else {
        // Either both system engines, or both non system
        // engines.
        //
        // Note, this isn't a typo. Higher priority numbers imply
        // higher priority, but are "lower" in the sort order.
        *result = rhs->priority - lhs->priority;
    }

    return NOERROR;
}

/******************
 * TtsEngines
 *******************************************************************************************************/

const String TtsEngines::TAG("TtsEngines");
const Boolean TtsEngines::DBG = TRUE;

const String TtsEngines::LOCALE_DELIMITER_OLD("-");
const String TtsEngines::LOCALE_DELIMITER_NEW("_");
const String TtsEngines::XML_TAG_NAME("tts-engine");

static AutoPtr<HashMap<String, String> > InitNormalizeCountry()
{
    AutoPtr<HashMap<String, String> > map = new HashMap<String, String>();

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);

    AutoPtr<ArrayOf<String> > codes;
    helper->GetISOLanguages((ArrayOf<String>**)&codes);
    if (codes != NULL) {
        for (Int32 i = 0; i < codes->GetLength(); ++i) {
            String language = (*codes)[i];
            AutoPtr<ILocale> locale;
            CLocale::New(language, (ILocale**)&locale);
            String iso3;
            locale->GetISO3Language(&iso3);
            (*map)[iso3] = language;
        }
    }
    return map;
}

static AutoPtr<HashMap<String, String> > InitNormalizeLanguage()
{
    AutoPtr<HashMap<String, String> > map = new HashMap<String, String>();

    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);

    AutoPtr<ArrayOf<String> > codes;
    helper->GetISOCountries((ArrayOf<String>**)&codes);
    if (codes != NULL) {
        for (Int32 i = 0; i < codes->GetLength(); ++i) {
            String country = (*codes)[i];
            AutoPtr<ILocale> locale;
            CLocale::New(country, (ILocale**)&locale);
            String iso3;
            locale->GetISO3Country(&iso3);
            (*map)[iso3] = country;
        }
    }

    return map;
}

AutoPtr<HashMap<String, String> > TtsEngines::sNormalizeCountry = InitNormalizeCountry();
AutoPtr<HashMap<String, String> > TtsEngines::sNormalizeLanguage = InitNormalizeLanguage();

CAR_INTERFACE_IMPL(TtsEngines, Object, ITtsEngines)

TtsEngines::TtsEngines()
{
}

TtsEngines::~TtsEngines()
{}

ECode TtsEngines::constructor(
    /* [in] */ IContext* ctx)
{
    mContext = ctx;
    return NOERROR;
}

ECode TtsEngines::GetDefaultEngine(
    /* [out] */ String* pRet)
{
    VALIDATE_NOT_NULL(pRet);

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String engine;
    Settings::Secure::GetString(cr, ISettingsSecure::TTS_DEFAULT_SYNTH, &engine);

    Boolean b;
    IsEngineInstalled(engine, &b);
    if (b) {
        *pRet = engine;
    }
    else {
        GetHighestRankedEngineName(pRet);
    }

    return NOERROR;
}

ECode TtsEngines::GetHighestRankedEngineName(
    /* [out] */ String* pRet)
{
    VALIDATE_NOT_NULL(pRet);

    AutoPtr<IList> engines;
    GetEngines((IList**)&engines);

    if (engines != NULL ) {

        Boolean b;
        engines->IsEmpty(&b);
        if (!b) {
            TextToSpeech::TextToSpeechEngineInfo* engine;
            AutoPtr<IInterface> obj;
            engines->Get(0, (IInterface**)&obj);
            engine = (TextToSpeech::TextToSpeechEngineInfo*)IObject::Probe(obj);
            if (engine->system) {
                *pRet = engine->name;
            }
        }

    } else {
        *pRet = String(NULL);
    }

    return NOERROR;
}

ECode TtsEngines::GetEngineInfo(
    /* [in] */ const String& packageName,
    /* [out] */ ITextToSpeechEngineInfo** ppRet)
{
    VALIDATE_NOT_NULL(ppRet);

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> intent;
    CIntent::New(ITextToSpeechEngine::INTENT_ACTION_TTS_SERVICE, (IIntent**)&intent);
    intent->SetPackage(packageName);
    List< AutoPtr<IResolveInfo> > resolveInfos;
    AutoPtr<IList> oc;
    pm->QueryIntentServices(intent.Get(),IPackageManager::MATCH_DEFAULT_ONLY, (IList**)&oc);

    ppRet = NULL;
    if (oc == NULL) {
        return NOERROR;
    }

    // Note that the current API allows only one engine per
    // package name. Since the "engine name" is the same as
    // the package name.
    AutoPtr<IInterface> obj;
    oc->Get(0, (IInterface**)&obj);
    *ppRet = ITextToSpeechEngineInfo::Probe(obj);
    REFCOUNT_ADD(*ppRet)
    return NOERROR;
}

ECode TtsEngines::GetEngines(
    /* [out] */ IList** ppRet)
{
    VALIDATE_NOT_NULL(ppRet);

    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> intent;
    CIntent::New(ITextToSpeechEngine::INTENT_ACTION_TTS_SERVICE, (IIntent**)&intent);
    AutoPtr<IList> oc;
    pm->QueryIntentServices(intent.Get(), IPackageManager::MATCH_DEFAULT_ONLY, (IList**)&oc);

    AutoPtr<ICollections> c;
    CCollections::AcquireSingleton((ICollections**)&c);

    if (oc == NULL) {
        c->GetEmptyList(ppRet);
        return NOERROR;
    }

    AutoPtr<IList> engines;
    CArrayList::New((IList**)&engines);
    Int32 resolveInfosSize;
    oc->GetSize(&resolveInfosSize);

    for (Int32 i = 0; i < resolveInfosSize; i++) {
        AutoPtr<IInterface> obj;
        oc->Get(i, (IInterface**)&obj);
        IResolveInfo* resolve = IResolveInfo::Probe(obj);
        AutoPtr<TextToSpeech::TextToSpeechEngineInfo> engine = GetEngineInfo(resolve, pm);
        if (engine != NULL) {
            engines->Add((ITextToSpeechEngineInfo*)engine.Get());
        }
    }

    c->Sort(engines, EngineInfoComparator::INSTANCE.Get());
    *ppRet = engines;
    REFCOUNT_ADD(*ppRet)
    return NOERROR;
}

Boolean TtsEngines::IsSystemEngine(
    /* [in] */ IServiceInfo* info)
{
    AutoPtr<IApplicationInfo> appInfo;
    ((CServiceInfo*)info)->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 flags;
    appInfo->GetFlags(&flags);
    return appInfo != NULL && (flags & IApplicationInfo::FLAG_SYSTEM) != 0;
}

ECode TtsEngines::IsEngineInstalled(
    /* [in] */ const String& engine,
    /* [out] */ Boolean* pRet)
{
    if (engine.IsNullOrEmpty()) {
        *pRet = FALSE;
    }
    AutoPtr<ITextToSpeechEngineInfo> p;

    GetEngineInfo(engine, (ITextToSpeechEngineInfo**)&p);

    *pRet = (p != NULL);
    return NOERROR;
}

ECode TtsEngines::GetSettingsIntent(
    /* [in] */ const String& engine,
    /* [out] */ IIntent** ppRet)
{
    AutoPtr<IPackageManager> pm;
    mContext->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IIntent> intent;
    CIntent::New(ITextToSpeechEngine::INTENT_ACTION_TTS_SERVICE, (IIntent**)&intent);
    intent->SetPackage(engine);
    List< AutoPtr<IResolveInfo> > resolveInfos;

    AutoPtr<IList> oc;
    pm->QueryIntentServices(intent.Get(),
        IPackageManager::MATCH_DEFAULT_ONLY | IPackageManager::GET_META_DATA, (IList**)&oc);

    // Note that the current API allows only one engine per
    // package name. Since the "engine name" is the same as
    // the package name.
    AutoPtr<IInterface> obj;
    oc->Get(0, (IInterface**)&obj);
    IServiceInfo* service = IServiceInfo::Probe(obj);
    if (service != NULL) {
        String settings = SettingsActivityFromServiceInfo(service, pm);
        if (!settings.IsNull()) {
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(engine, settings);
            *ppRet = intent;
            REFCOUNT_ADD(*ppRet)
            return NOERROR;
        }
    }

    *ppRet = NULL;
    return NOERROR;
}

ECode TtsEngines::NormalizeTTSLocale(
    /* [in] */ ILocale* ttsLocale,
    /* [out] */ ILocale** outLocale)
{
    VALIDATE_NOT_NULL(outLocale)
    *outLocale = NULL;

    HashMap<String, String>::Iterator it;
    String language;
    ttsLocale->GetLanguage(&language);
    if (!TextUtils::IsEmpty(language)) {
        it = sNormalizeLanguage->Find(language);
        if (it != sNormalizeLanguage->End()) {
            language = it->mSecond;
        }
    }

    String country;
    ttsLocale->GetCountry(&country);
    if (!TextUtils::IsEmpty(country)) {
        it = sNormalizeCountry->Find(country);
        if (it != sNormalizeCountry->End()) {
            country = it->mSecond;
        }
    }

    String variant;
    ttsLocale->GetVariant(&variant);

    return CLocale::New(language, country, variant, outLocale);
}

String TtsEngines::SettingsActivityFromServiceInfo(
    /* [in] */ IServiceInfo* si,
    /* [in] */ IPackageManager* pm)
{
    IPackageItemInfo* pi = IPackageItemInfo::Probe(si);

    ECode ec = NOERROR;
    AutoPtr<IXmlResourceParser> resParser;
    ec = pi->LoadXmlMetaData(pm, ITextToSpeechEngine::SERVICE_META_DATA, (IXmlResourceParser**)&resParser);
    if (resParser == NULL) {
        Logger::W(TAG, "No meta-data found for : %s", TO_CSTR(si));
        return String(NULL);
    }

    IXmlPullParser* parser = IXmlPullParser::Probe(resParser);
    AutoPtr<IResources> res;
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(si)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
    pm->GetResourcesForApplication(applicationInfo, (IResources**)&res);

    String strParserName;
    AutoPtr<IAttributeSet> attrs;
    AutoPtr<ArrayOf<Int32> > vals = TO_ATTRS_ARRAYOF(R::styleable::TextToSpeechEngine);
    Int32 type;
    ec = parser->Next(&type);
    if (FAILED(ec)) goto __EXIT__;

    while (type != IXmlPullParser::END_DOCUMENT) {
        if (type == IXmlPullParser::START_TAG) {
            parser->GetName(&strParserName);
            if (!XML_TAG_NAME.Equals(strParserName)) {
                Logger::W(TAG, "Package %s uses unknown tag :%s", TO_CSTR(si), strParserName.string());
                return String(NULL);
            }

            attrs = Xml::AsAttributeSet(parser);

            AutoPtr<ITypedArray> array;
            res->ObtainAttributes(attrs, vals, (ITypedArray**)&array);

            String settings;
            array->GetString(R::styleable::TextToSpeechEngine_settingsActivity, &settings);
            array->Recycle();
            resParser->Close();
            return settings;
        }

        ec = parser->Next(&type);
        if (FAILED(ec)) goto __EXIT__;
    }

__EXIT__:
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Logger::W(TAG, "Could not load resources for %s", TO_CSTR(si));
    }
    else if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        Logger::W(TAG, "Error parsing metadata for %s", TO_CSTR(si));
    }
    else if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::W(TAG, "Error parsing metadata for %s", TO_CSTR(si));
    }

    if (resParser != NULL) {
        resParser->Close();
    }

    return String(NULL);
}

AutoPtr<TextToSpeech::TextToSpeechEngineInfo> TtsEngines::GetEngineInfo(
    /* [in] */ IResolveInfo* resolve,
    /* [in] */ IPackageManager* pm)
{
    AutoPtr<IServiceInfo> service;
    resolve->GetServiceInfo((IServiceInfo**)&service);
    if (service != NULL) {
        AutoPtr<TextToSpeech::TextToSpeechEngineInfo> engine = new TextToSpeech::TextToSpeechEngineInfo();
        // Using just the package name isn't great, since it disallows having
        // multiple engines in the same package, but that's what the existing API does.
        IPackageItemInfo::Probe(service)->GetPackageName(&(engine->name));
        AutoPtr<ICharSequence> label;
        IPackageItemInfo::Probe(service)->LoadLabel(pm, (ICharSequence**)&label);
        if (TextUtils::IsEmpty(label)) {
            engine->label = engine->name;
        }
        else {
            label->ToString(&(engine->label));
        }
        IComponentInfo::Probe(service)->GetIconResource(&(engine->icon));
        resolve->GetPriority(&(engine->priority));
        engine->system = IsSystemEngine(service.Get());
        return engine;
    }

    return NULL;
}

ECode TtsEngines::GetLocalePrefForEngine(
    /* [in] */ const String& engineName,
    /* [out] */ ILocale** ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    String str;
    Settings::Secure::GetString(cr, ISettingsSecure::TTS_DEFAULT_LOCALE, &str);
    return GetLocalePrefForEngine(engineName, str, ret);
}

ECode TtsEngines::GetLocalePrefForEngine(
    /* [in] */ const String& engineName,
    /* [in] */ const String& prefValue,
    /* [in] */ ILocale** locale)
{
    String localeString = ParseEnginePrefFromList(prefValue, engineName);
    if (TextUtils::IsEmpty(localeString)) {
        // The new style setting is unset, attempt to return the old style setting.
        localeString = GetV1Locale();
    }

    AutoPtr<ILocale> result;
    ParseLocaleString(localeString, (ILocale**)&result);
    if (result == NULL) {
        Logger::W(TAG, "Failed to parse locale %s returning en_US instead", engineName.string());
        AutoPtr<ILocaleHelper> helper;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
        helper->GetUS((ILocale**)&result);
    }

    if (DBG) {
        Logger::D(TAG, "getLocalePrefForEngine(%s)= %s\n", engineName.string(), localeString.string());
    }

    *locale = result;
    REFCOUNT_ADD(*locale)
    return NOERROR;
}

ECode TtsEngines::IsLocaleSetToDefaultForEngine(
    /* [in] */ const String& engineName,
    /* [out] */ Boolean* ret)
{
    VALIDATE_NOT_NULL(ret)

    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String prefValue;
    Settings::Secure::GetString(cr, ISettingsSecure::TTS_DEFAULT_LOCALE, &prefValue);

    String localeString = ParseEnginePrefFromList(prefValue, engineName);
    *ret = TextUtils::IsEmpty(localeString);
    return NOERROR;
}

ECode TtsEngines::ParseLocaleString(
    /* [in] */ const String& localeString,
    /* [out] */ ILocale** ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = NULL;

    String language("");
    String country("");
    String variant("");

    if (!TextUtils::IsEmpty(localeString)) {
        Int32 localeDelimiterLen = localeString.GetLength();
        Int32 splitCount = 1;
        Int32 splitStartPos = 0;

        while( splitStartPos >= 0 && splitStartPos < localeDelimiterLen){
            splitStartPos = localeString.IndexOf(LOCALE_DELIMITER_OLD, splitStartPos);
            if (splitStartPos >= 0 && splitStartPos <= localeDelimiterLen - 1) {
                splitCount++;
            }
            else {
                splitStartPos = localeString.IndexOf(LOCALE_DELIMITER_NEW, splitStartPos);
                if(splitStartPos >= 0 && splitStartPos <= localeDelimiterLen - 1) {
                    splitCount++;
                }
            }
            splitStartPos++;
        }

        AutoPtr< ArrayOf<String> > split = ArrayOf<String>::Alloc(splitCount);
        Int32 aryIndexRem = 0;
        Int32 curFindIndex=0;
        splitStartPos = 0;

        while( splitStartPos >= 0 && splitStartPos < localeDelimiterLen) {
            curFindIndex = splitStartPos;
            splitStartPos = localeString.IndexOf(LOCALE_DELIMITER_OLD, splitStartPos);
            if (splitStartPos >= 0 && splitStartPos <= localeDelimiterLen - 1) {
                (*split)[aryIndexRem] = localeString.Substring(curFindIndex, splitStartPos-curFindIndex);
                aryIndexRem++;
            }
            else {
                splitStartPos = localeString.IndexOf(LOCALE_DELIMITER_NEW, splitStartPos);
                if (splitStartPos >= 0 && splitStartPos <= localeDelimiterLen - 1) {
                    (*split)[aryIndexRem] = localeString.Substring(curFindIndex, splitStartPos-curFindIndex);
                    aryIndexRem++;
                }
            }
            splitStartPos++;
        }

        String str;

        language = (*split)[0].ToLowerCase();
        if ((*split).GetLength() == 0) {
            Logger::W(TAG, "Failed to convert %s to a valid Locale object. Only separators\n", localeString.string());
            *ret = NULL;
            return NOERROR;
        }
        if ((*split).GetLength() > 3) {
            Logger::W(TAG, "Failed to convert %s to a valid Locale object. Too many separators\n", localeString.string());
            *ret = NULL;
            return NOERROR;
        }
        if ((*split).GetLength() >= 2) {
            country = (*split)[1].ToUpperCase();
        }
        if ((*split).GetLength() >= 3) {
            variant = (*split)[2];
        }
    }

    HashMap<String, String>::Iterator it;
    it = sNormalizeLanguage->Find(language);
    if (it != sNormalizeLanguage->End()) {
        language = it->mSecond;
    }

    it = sNormalizeCountry->Find(country);
    if (it != sNormalizeCountry->End()) {
        country = it->mSecond;
    }

    if (DBG) {
        Logger::D(TAG, "ParseLocalePref(%s, %s, %s)",
            language.string(), country.string(), variant.string());
    }

    AutoPtr<ILocale> mLocale;
    CLocale::New(language, country, variant, (ILocale**)&mLocale);

    String strISO3Language, strISO3Country, strVariant;
    mLocale->GetISO3Language(&strISO3Language);
    mLocale->GetISO3Country(&strISO3Country);
    mLocale->GetVariant(&strVariant);

    if (strISO3Language == NULL || strISO3Country == NULL) {
        Logger::W(TAG, "Failed to convert %s to a valid Locale object.\n", localeString.string());
        return E_MISSING_RESOURCE_EXCEPTION;
    }

    *ret = mLocale;
    return NOERROR;
}

AutoPtr<ILocale> TtsEngines::NormalizeTTSLocale(
    /* [in] */ ILocale* ttsLocale)
{
    AutoPtr<ILocale> locale;
    NormalizeTTSLocale(ttsLocale, (ILocale**)&locale);
    return locale;
}

/**
 * Return the old-style string form of the locale. It consists of 3 letter codes:
 * <ul>
 *   <li>"ISO 639-2/T language code" if the locale has no country entry</li>
 *   <li> "ISO 639-2/T language code{@link #LOCALE_DELIMITER}ISO 3166 country code"
 *     if the locale has no variant entry</li>
 *   <li> "ISO 639-2/T language code{@link #LOCALE_DELIMITER}ISO 3166 country
 *     code{@link #LOCALE_DELIMITER}variant" if the locale has a variant entry</li>
 * </ul>
 * If we fail to generate those codes using {@link Locale#getISO3Country()} and
 * {@link Locale#getISO3Language()}, then we return new String[]{"eng","USA",""};
 */
AutoPtr<ArrayOf<String> > TtsEngines::ToOldLocaleStringFormat(ILocale* locale)
{
    AutoPtr< ArrayOf<String> > returnVal = ArrayOf<String>::Alloc(3);
    (*returnVal)[0] = String("");
    (*returnVal)[1] = String("");
    (*returnVal)[2] = String("");

    // Note that the default locale might have an empty variant
    // or language, and we take care that the construction is
    // the same as {@link #getV1Locale} i.e no trailing delimiters
    // or spaces.
    String strISO3Language, strISO3Country, strVariant;
    locale->GetISO3Language(&strISO3Language);
    locale->GetISO3Country(&strISO3Country);
    locale->GetVariant(&strVariant);

    if (strISO3Language == NULL || strISO3Country == NULL) {
        (*returnVal)[0] = String("eng");
        (*returnVal)[1] = String("USA");
    }

    return returnVal;
}

String TtsEngines::GetV1Locale()
{
    return String("");
}

String TtsEngines::GetDefaultLocale()
{
    AutoPtr<ILocaleHelper> lh;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&lh);
    AutoPtr<ILocale> locale;
    lh->GetDefault((ILocale**)&locale);

    // Note that the default locale might have an empty variant
    // or language, and we take care that the construction is
    // the same as {@link #getV1Locale} i.e no trailing delimiters
    // or spaces.
    String defaultLocale;
    locale->GetISO3Language(&defaultLocale);

    AutoPtr<ICharSequence> cs;
    CString::New(defaultLocale, (ICharSequence**)&cs);
    if (TextUtils::IsEmpty(cs)) {
        Logger::W(TAG, String("Default locale is empty.\n"));
        return String("");
    }

    String localeCountry;
    locale->GetISO3Country(&localeCountry);
    cs = NULL;
    CString::New(localeCountry, (ICharSequence**)&cs);
    if (!TextUtils::IsEmpty(cs)) {
        defaultLocale += String(LOCALE_DELIMITER_NEW) + localeCountry;
    } else {
        // Do not allow locales of the form lang--variant with
        // an empty country.
        return defaultLocale;
    }

    String strVariant;
    locale->GetVariant(&strVariant);
    cs = NULL;
    CString::New(strVariant, (ICharSequence**)&cs);
    if (!TextUtils::IsEmpty(cs)) {
        defaultLocale += String(LOCALE_DELIMITER_NEW) + strVariant;
    }

    return defaultLocale;
}

String TtsEngines::ParseEnginePrefFromList(
    /* [in] */ const String& prefValue,
    /* [in] */ const String& engineName)
{
    AutoPtr<ICharSequence> cs;
    CString::New(prefValue, (ICharSequence**)&cs);
    if (TextUtils::IsEmpty(cs)) {
        return String(NULL);
    }

    Int32 localeDelimiterLen = prefValue.GetLength();
    Int32 splitCount = 1;
    Int32 splitStartPos = 0;
    while( splitStartPos>=0 && splitStartPos<localeDelimiterLen){
        splitStartPos = prefValue.IndexOf(String(","), splitStartPos);
        if(splitStartPos>=0 && splitStartPos<=localeDelimiterLen-1) {
            splitCount++;
        }
        splitStartPos++;
    }
    AutoPtr< ArrayOf<String> > prefValues = ArrayOf<String>::Alloc(splitCount);
    Int32 aryIndexRem = 0;
    Int32 curFindIndex=0;
    splitStartPos = 0;
    while( splitStartPos>=0 && splitStartPos<localeDelimiterLen){
        curFindIndex = splitStartPos;
        splitStartPos = prefValue.IndexOf(String(","), splitStartPos);
        if(splitStartPos>=0 && splitStartPos<=localeDelimiterLen-1) {
            (*prefValues)[aryIndexRem] = prefValue.Substring(curFindIndex, splitStartPos-curFindIndex);
            aryIndexRem++;
        }
        splitStartPos++;
    }

    for(Int32 i = 0; i<splitCount; i++) {
        String value = (*prefValues)[i];
        Int32 delimiter = value.IndexOf(':');
        if (delimiter > 0) {
            if (engineName.Equals(value.Substring(0, delimiter))) {
                return value.Substring(delimiter + 1);
            }
        }
    }
    return String(NULL);
}

ECode TtsEngines::UpdateLocalePrefForEngine(
    /* [in] */ const String& engineName,
    /* [in] */ ILocale* newLocale)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    String newPrefList;
    {
        AutoLock syncLock(mLock);

        String prefList;
        Settings::Secure::GetString(cr, ISettingsSecure::TTS_DEFAULT_LOCALE, &prefList);
        if (DBG) {
            Logger::D(TAG, "updateLocalePrefForEngine(%s, %s), originally: %s",
                engineName.string(), TO_CSTR(newLocale), prefList.string());
        }

        String str("");
        if (newLocale != NULL) {
            str = TO_STR(newLocale);
        }
        newPrefList = UpdateValueInCommaSeparatedList(prefList, engineName, str);

        if (DBG) {
            Logger::D(TAG, "updateLocalePrefForEngine(), writing: %s", newPrefList.string());
        }
    }

    Boolean bval;
    Settings::Secure::PutString(cr, ISettingsSecure::TTS_DEFAULT_LOCALE, newPrefList, &bval);
    return NOERROR;
}

String TtsEngines::UpdateValueInCommaSeparatedList(
    /* [in] */ const String& list,
    /* [in] */ const String& key,
    /* [in] */ const String&  newValue)
{
    StringBuilder newPrefList;
    if (TextUtils::IsEmpty(list)) {
        // If empty, create a new list with a single entry.
        newPrefList.Append(key);
        newPrefList.Append(String(":"));
        newPrefList.Append(newValue);
    }
    else {
        //Java:    String[] prefValues = list.split(",");
        Int32 localeDelimiterLen = list.GetLength();
        Int32 splitCount = 1;
        Int32 splitStartPos = 0;
        while( splitStartPos >= 0 && splitStartPos < localeDelimiterLen) {
            splitStartPos = list.IndexOf(String(","), splitStartPos);
            if(splitStartPos>=0 && splitStartPos<=localeDelimiterLen-1) {
                splitCount++;
            }
            splitStartPos++;
        }
        AutoPtr< ArrayOf<String> > prefValues = ArrayOf<String>::Alloc(splitCount);
        Int32 aryIndexRem = 0;
        Int32 curFindIndex=0;
        splitStartPos = 0;
        while( splitStartPos >= 0 && splitStartPos < localeDelimiterLen){
            curFindIndex = splitStartPos;
            splitStartPos = list.IndexOf(String(","), splitStartPos);
            if(splitStartPos >= 0 && splitStartPos <= localeDelimiterLen - 1) {
                (*prefValues)[aryIndexRem] = list.Substring(curFindIndex, splitStartPos - curFindIndex);
                aryIndexRem++;
            }
            splitStartPos++;
        }

        // Whether this is the first iteration in the loop.
        Boolean first = TRUE;
        // Whether we found the given key.
        Boolean found = FALSE;


        for(Int32 i = 0; i < splitCount; i++) {
            String value = (*prefValues)[i];

            Int32 delimiter = value.IndexOf(':');
            if (delimiter > 0) {
                if (key.Equals(value.Substring(0, delimiter))) {
                    if (first) {
                        first = FALSE;
                    } else {
                        newPrefList.Append(String(","));
                    }
                    found = TRUE;
                    newPrefList.Append(key);
                    newPrefList.Append(String(":"));
                    newPrefList.Append(newValue);
                } else {
                    if (first) {
                        first = FALSE;
                    } else {
                        newPrefList.Append(String(","));
                    }
                    // Copy across the entire key + value as is.
                    newPrefList.Append(value);
                }
            }

        }

        if (!found) {
            // Not found, but the rest of the keys would have been copied
            // over already, so just append it to the end.
            newPrefList.Append(String(","));
            newPrefList.Append(key);
            newPrefList.Append(String(":"));
            newPrefList.Append(newValue);
        }
    }

    return newPrefList.ToString();
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
