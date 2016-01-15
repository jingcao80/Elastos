#include "elastos/droid/speech/tts/TtsEngines.h"
#include "elastos/droid/content/CIntent.h"
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/content/pm/CResolveInfo.h"
#include "elastos/droid/content/pm/CServiceInfo.h"
#include "elastos/droid/text/TextUtils.h"
//#include "elastos/droid/provider/Settings.h"
#include "elastos/droid/speech/tts/TextToSpeech.h"
#include "elastos/droid/R.h"
#include <elastos/core/AutoLock.h>
#include "Elastos.Droid.Utility.h"
#include <Elastos.CoreLibrary.Utility.h>

using Elastos::Core::IComparator;
using Elastos::Core::EIID_IComparator;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::CServiceInfo;
using Elastos::Droid::Content::Pm::CResolveInfo;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocale;
using Elastos::Utility::CLocaleHelper;
using Elastos::Core::CString;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::IList;

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

ECode TtsEngines::EngineInfoComparator::Equals(
    /* [in] */ IInterface* object,
    /* [out] */ Boolean* isEqual)
{
    *isEqual = TRUE;

    return NOERROR;
}

ECode TtsEngines::EngineInfoComparator::Compare(
    /* [in] */ IInterface* object1,
    /* [in] */ IInterface* object2,
    /* [out] */ Int32* result)
{
#if 0
    AutoPtr<TextToSpeech::TextToSpeechEngineInfo> lhs = (TextToSpeech::TextToSpeechEngineInfo*)object1;
    AutoPtr<TextToSpeech::TextToSpeechEngineInfo> rhs = (TextToSpeech::TextToSpeechEngineInfo*)object2;
    if (lhs->system && !rhs->system) {
        *result = -1;
    } else if (rhs->system && !lhs->system) {
        *result = 1;
    } else {
        // Either both system engines, or both non system
        // engines.
        //
        // Note, this isn't a typo. Higher priority numbers imply
        // higher priority, but are "lower" in the sort order.
        *result = rhs->priority - lhs->priority;
    }
#endif
    return NOERROR;
}

Int32 TtsEngines::EngineInfoComparator::Comparator(
    /* [in] */ AutoPtr<TextToSpeech::TextToSpeechEngineInfo> lhs,
    /* [in] */ AutoPtr<TextToSpeech::TextToSpeechEngineInfo> rhs)
{
    Int32 returnVal;
    if (lhs->system && !rhs->system) {
        returnVal = -1;
    } else if (rhs->system && !lhs->system) {
        returnVal = 1;
    } else {
        returnVal = rhs->priority - lhs->priority;
    }
    return returnVal;
}

/******************
 * TtsEngines
 *******************************************************************************************************/

const String TtsEngines::TAG("TtsEngines");
const Boolean TtsEngines::DBG = FALSE;

const String TtsEngines::LOCALE_DELIMITER_OLD("-");
const String TtsEngines::LOCALE_DELIMITER_NEW("_");
const String TtsEngines::XML_TAG_NAME("tts-engine");
Map<String, String> TtsEngines::sNormalizeCountry;
Map<String, String> TtsEngines::sNormalizeLanguage;

CAR_INTERFACE_IMPL(TtsEngines, Object, ITtsEngines)

TtsEngines::TtsEngines()
{
#if 0
   // Populate the sNormalize* maps
    static {
        HashMap<String, String> normalizeLanguage = new HashMap<String, String>();
        for (String language : Locale.getISOLanguages()) {
            try {
                normalizeLanguage.put(new Locale(language).getISO3Language(), language);
            } catch (MissingResourceException e) {
                continue;
            }
        }
        sNormalizeLanguage = Collections.unmodifiableMap(normalizeLanguage);

        HashMap<String, String> normalizeCountry = new HashMap<String, String>();
        for (String country : Locale.getISOCountries()) {
            try {
                normalizeCountry.put(new Locale("", country).getISO3Country(), country);
            } catch (MissingResourceException e) {
                continue;
            }
        }
        sNormalizeCountry = Collections.unmodifiableMap(normalizeCountry);
    }
#endif

}

TtsEngines::~TtsEngines()
{}

ECode TtsEngines::constructor()
{
    return NOERROR;
}

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

    //Java:    String engine = getString(mContext.getContentResolver(), Settings.Secure.TTS_DEFAULT_SYNTH);
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);
    String engine;      // = GetString(cr.Get(), /*ISettingsSecure::TTS_DEFAULT_SYNTH*/String("tts_default_synth"));

    Boolean b;
    String str;
    IsEngineInstalled(engine, &b);

    if (b) {
        *pRet = engine;
    } else {
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
        AutoPtr<TextToSpeech::TextToSpeechEngineInfo> engine;

        engines->IsEmpty(&b);
        engines->Get(0, (IInterface**)&engine);
        if ( !b && engine->system) {
            *pRet = engine->name;
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
    oc->Get(0, (IInterface**)ppRet);

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
    c->GetEmptyList((IList**)ppRet);

    if (oc == NULL) {
        return NOERROR;
    }

    Int32 resolveInfosSize;
    oc->GetSize(&resolveInfosSize);

    for (Int32 i = 0; i < resolveInfosSize; i++) {
        AutoPtr<IResolveInfo> resolve;
        AutoPtr<TextToSpeech::TextToSpeechEngineInfo> engine;
        oc->Get(i, (IInterface**)&resolve);

        engine = GetEngineInfo(resolve, pm.Get());
        if (engine != NULL) {
            (*ppRet)->Set(i, TO_IINTERFACE(engine));
        }
    }

    c->Sort((IList*)(*ppRet), (IComparator*)EngineInfoComparator::Comparator);

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

    AutoPtr<IServiceInfo> service;

    // Note that the current API allows only one engine per
    // package name. Since the "engine name" is the same as
    // the package name.
    oc->Get(0, (IInterface**)&service);

    if (service != NULL) {
        String settings = SettingsActivityFromServiceInfo(service.Get(), pm.Get());
        if (!settings.IsNull()) {
            AutoPtr<IIntent> intent;
            CIntent::New((IIntent**)&intent);
            intent->SetClassName(engine, settings);
            *ppRet = intent;
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
    return NOERROR;
/*
        String language = ttsLocale.getLanguage();
        if (!TextUtils.isEmpty(language)) {
            String normalizedLanguage = sNormalizeLanguage.get(language);
            if (normalizedLanguage != null) {
                language = normalizedLanguage;
            }
        }

        String country = ttsLocale.getCountry();
        if (!TextUtils.isEmpty(country)) {
            String normalizedCountry= sNormalizeCountry.get(country);
            if (normalizedCountry != null) {
                country = normalizedCountry;
            }
        }
        return new Locale(language, country, ttsLocale.getVariant());
*/
}


String TtsEngines::SettingsActivityFromServiceInfo(
    /* [in] */ IServiceInfo* si,
    /* [in] */ IPackageManager* pm)
{
/*
// WAITING for android.content.pm.ResolveInfo;
//
**
 * Information you can retrieve about a particular application
 * service. This corresponds to information collected from the
 * AndroidManifest.xml's &lt;service&gt; tags.
 *
public class ServiceInfo extends ComponentInfo
        implements Parcelable {
*/
#if 0
    AutoPtr<IXmlResourceParser> parser;
    //try {
        si->LoadXmlMetaData(pm, ITextToSpeechEngine::SERVICE_META_DATA, (IXmlResourceParser**)&parser);
        if (parser == NULL) {
            //Java:    Log.w(TAG, "No meta-data found for :" + si);
            Logger::W(TAG, String("No meta-data found for :")+String("\n"));
            return String(NULL);
        }

        AutoPtr<IResources> res;
        AutoPtr<IApplicationInfo> applicationInfo;
        ((CServiceInfo*)si)->GetApplicationInfo((IApplicationInfo**)&applicationInfo);
        pm->GetResourcesForApplication(applicationInfo.Get(), (IResources**)&res);

        Int32 type;
        parser->Next(&type);
        while ((type) != IXmlPullParser::END_DOCUMENT) {
            if (type == IXmlPullParser::START_TAG) {
                String strParserName;
                parser->GetName(&strParserName);
                if (!XML_TAG_NAME.Equals(strParserName)) {
                    //JavaL:    Log.w(TAG, "Package " + si + " uses unknown tag :"  + parser.getName());
                    Logger::W(TAG, String("Package ... uses unknown tag :")+strParserName+String("\n"));
                    return String(NULL);
                }

                AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(parser);
                AutoPtr< ArrayOf<Int32> >  aryTextToSpeechEngine = ArrayOf<Int32>::Alloc(1);
                for(Int32 i=0; i<1; i++){
                    (*aryTextToSpeechEngine)[i] = (R::styleable::TextToSpeechEngine)[i];
                }
                AutoPtr<ITypedArray> array;
                res->ObtainAttributes(attrs.Get(), aryTextToSpeechEngine.Get(), (ITypedArray**)&array);
                String settings;
                array->GetString(R::styleable::TextToSpeechEngine_settingsActivity, &settings);
                array->Recycle();
                parser->Close();
                return settings;
            }
        }

    //} catch (NameNotFoundException e) {
            //Java:    Log.w(TAG, "Could not load resources for : " + si);
//            Logger::W(TAG, String("Could not load resources for : ...")+String("\n"));
    //} catch (XmlPullParserException e) {
            //Java:    Log.w(TAG, "Error parsing metadata for " + si + ":" + e);
//            Logger::W(TAG, String("Error parsing metadata for... : ... ")+String("\n"));
    //} catch (IOException e) {
            //Java:    Log.w(TAG, "Error parsing metadata for " + si + ":" + e);
//            Logger::W(TAG, String("Error parsing metadata for ... : ... ")+String("\n"));
    //} finally {
            if (parser != NULL) {
                parser->Close();
            }
    //}
#endif
    return String(NULL);
}

AutoPtr<TextToSpeech::TextToSpeechEngineInfo> TtsEngines::GetEngineInfo(
    /* [in] */ IResolveInfo* resolve,
    /* [in] */ IPackageManager* pm)
{
#if 0
    AutoPtr<IServiceInfo> service;
    resolve->GetServiceInfo((IServiceInfo**)&service);
    if (service != NULL) {
        AutoPtr<TextToSpeech::TextToSpeechEngineInfo> engine = new TextToSpeech::TextToSpeechEngineInfo();
        // Using just the package name isn't great, since it disallows having
        // multiple engines in the same package, but that's what the existing API does.
        ((CServiceInfo*)(service.Get()))->GetPackageName(&(engine->name));    //engine->name = ((CServiceInfo*)service)->packageName;
        AutoPtr<ICharSequence> label;
        service->LoadLabel(pm, (ICharSequence**)&label);
        if(TextUtils::IsEmpty(label)) {
            engine->label = engine->name;
        } else {
            label->ToString(&(engine->label));
        }
        service->GetIconResource(&(engine->icon));
        resolve->GetPriority(&(engine->priority));
        engine->system = IsSystemEngine(service.Get());
        return engine;
    }
#endif
    return NULL;
}

ECode TtsEngines::GetLocalePrefForEngine(
    /* [in] */ const String& engineName,
    /* [in] */ ILocale** ret)
{
    AutoPtr<IContentResolver> cr;
    mContext->GetContentResolver((IContentResolver**)&cr);

    return GetLocalePrefForEngine(engineName, /*GetString(cr.Get(), ISettingsSecure::TTS_DEFAULT_LOCALE)*/String(NULL), ret);
}

ECode TtsEngines::GetLocalePrefForEngine(
    /* [in] */ const String& engineName,
    /* [in] */ const String& prefValue,
    /* [in] */ ILocale** locale)
{
    String localeString;
    localeString = ParseEnginePrefFromList(prefValue, engineName);

    AutoPtr<ICharSequence> cs;
    CString::New(localeString, (ICharSequence**)&cs);
    if (TextUtils::IsEmpty(cs)) {
        // The new style setting is unset, attempt to return the old style setting.
        localeString = GetV1Locale();
    }

    AutoPtr<ILocale> result;
    ParseLocaleString(localeString, (ILocale**)&result);
    if (result == NULL) {
        Logger::W(TAG, "Failed to parse locale %s returning en_US instead", engineName.string());
//TODO
//        result = Locale.US;
    }


    if (DBG) {
        //Java:    Log.d(TAG, "getLocalePrefForEngine(" + engineName + ")= " + locale);
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
    assert(0 && "TODO");
    // return TextUtils.isEmpty(parseEnginePrefFromList(
    //             getString(mContext.getContentResolver(), Settings.Secure.TTS_DEFAULT_LOCALE),
    //             engineName));
    return NOERROR;
}

ECode TtsEngines::ParseLocaleString(
    /* [in] */ const String& localeString,
    /* [in] */ ILocale** ret)
{
    String language("");
    String country("");
    String variant("");

    AutoPtr<ICharSequence> cs;
    CString::New(localeString, (ICharSequence**)&cs);
    if (!TextUtils::IsEmpty(cs)) {

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

    String normalizedLanguage = sNormalizeLanguage[language];
    if (normalizedLanguage != NULL) {
        language = normalizedLanguage;
    }

    String normalizedCountry = sNormalizeCountry[country];
    if (normalizedCountry != NULL) {
        country = normalizedCountry;
    }

    if (DBG) {
        Logger::D(TAG, String("parseLocalePref(")+ language + "," + country +
                "," + variant + String(")\n"));
    }

    AutoPtr<ILocale> mLocale;
    CLocale::New(language, country, variant, (ILocale**)&mLocale);

    String strISO3Language, strISO3Country, strVariant;
    mLocale->GetISO3Language(&strISO3Language);
    mLocale->GetISO3Country(&strISO3Country);
    mLocale->GetVariant(&strVariant);

    if (strISO3Language == NULL || strISO3Country == NULL) {
        Logger::W(TAG, "Failed to convert %s to a valid Locale object.\n", localeString.string());
        //E_MISSING_RESOURCE_EXCEPTION;
        *ret = NULL;
        return NOERROR;
    }

    *ret = mLocale;
    return NOERROR;
}

ILocale* TtsEngines::NormalizeTTSLocale(ILocale* ttsLocale)
{
    String language;
    ttsLocale->GetLanguage(&language);

    if (!TextUtils::IsEmpty(language)) {
        String normalizedLanguage = sNormalizeLanguage[language];
        if (normalizedLanguage != NULL) {
            language = normalizedLanguage;
        }
    }

    String country;
    ttsLocale->GetCountry(&country);
    if (!TextUtils::IsEmpty(country)) {
        String normalizedCountry = sNormalizeCountry[country];
        if (normalizedCountry != NULL) {
            country = normalizedCountry;
        }
    }

    String variant;
    ttsLocale->GetVariant(&variant);

    AutoPtr<ILocale> mLocale;
    CLocale::New(language, country, variant, (ILocale**)&mLocale);

    return mLocale;
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
    synchronized(mLock) {

        AutoPtr<IContentResolver> cr;
        mContext->GetContentResolver((IContentResolver**)&cr);

        String prefList;
    //    prefList = Settings::Secure::GetString(cr.Get(), ISettingsSecure::TTS_DEFAULT_LOCALE);
        if (DBG) {
            //Java:    Log.d(TAG, "updateLocalePrefForEngine(" + name + ", " + newLocale + "), originally: " + prefList);
            Logger::D(TAG, String("updateLocalePrefForEngine(") + engineName + String(", ") + ToString(newLocale) + String("), originally: ") + prefList+String("\n"));
        }

        String newPrefList;
        if (newLocale != NULL) {
            newPrefList = UpdateValueInCommaSeparatedList(prefList, engineName, Object::ToString(newLocale));
        } else {
            newPrefList = UpdateValueInCommaSeparatedList(prefList, engineName, String(""));
        }

        if (DBG) {
            //Java:    Log.d(TAG, "updateLocalePrefForEngine(), writing: " + newPrefList.toString());
            Logger::D(TAG, String("updateLocalePrefForEngine(), writing: ")+ newPrefList + String("\n"));

        }

    }

//    Settings::Secure::PutString(cr.Get(), ISettingsSecure::TTS_DEFAULT_LOCALE, newPrefList);
    return NOERROR;
}

String TtsEngines::UpdateValueInCommaSeparatedList(
    /* [in] */ const String& list,
    /* [in] */ const String& key,
    /* [in] */ const String&  newValue)
{
    //Java:    StringBuilder newPrefList = new StringBuilder();
    String newPrefList;
    AutoPtr<ICharSequence> cs;
    CString::New(list, (ICharSequence**)&cs);
    if (TextUtils::IsEmpty(cs)) {
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

    return newPrefList;
}

} // namespace Tts
} // namespace Speech
} // namepsace Droid
} // namespace Elastos
