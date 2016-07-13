
#include <Elastos.CoreLibrary.External.h>
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/hardware/soundtrigger/KeyphraseEnrollmentInfo.h"
#include "elastos/droid/hardware/soundtrigger/KeyphraseMetadata.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/utility/Xml.h"
#include "elastos/droid/utility/CArraySet.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/R.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Hardware::Soundtrigger::KeyphraseMetadata;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::R;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::IList;
using Elastos::Utility::Arrays;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

const String KeyphraseEnrollmentInfo::TAG = String("KeyphraseEnrollmentInfo");
const String KeyphraseEnrollmentInfo::VOICE_KEYPHRASE_META_DATA = String("android.voice_enrollment");

CAR_INTERFACE_IMPL(KeyphraseEnrollmentInfo, Object, IKeyphraseEnrollmentInfo)

KeyphraseEnrollmentInfo::KeyphraseEnrollmentInfo()
    : mEnrollmentPackage(NULL)
    , mParseError(NULL)
{
}

ECode KeyphraseEnrollmentInfo::constructor(
    /* [in] */ IPackageManager* pm)
{
    // Find the apps that supports enrollment for hotword keyhphrases,
    // Pick a privileged app and obtain the information about the supported keyphrases
    // from its metadata.
    AutoPtr<IIntent> _intent;
    CIntent::New(ACTION_MANAGE_VOICE_KEYPHRASES, (IIntent**)&_intent);
    AutoPtr<IList> ris;
    FAIL_RETURN(pm->QueryIntentActivities(_intent, IPackageManager::MATCH_DEFAULT_ONLY, (IList**)&ris))
    Boolean result;
    if (ris == NULL || (ris->IsEmpty(&result), result)) {
        // No application capable of enrolling for voice keyphrases is present.
        mParseError = String("No enrollment application found");
        return NOERROR;
    }

    Boolean found = FALSE;
    AutoPtr<IApplicationInfo> ai;

    Int32 length;
    FAIL_RETURN(ris->GetSize(&length))
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IInterface> obj;
        ris->Get(i, (IInterface**)&obj);
        AutoPtr<IResolveInfo> ri = IResolveInfo::Probe(obj);

        //try {
        AutoPtr<IActivityInfo> info;
        FAIL_RETURN(ri->GetActivityInfo((IActivityInfo**)&info))
        String name;
        FAIL_RETURN(IPackageItemInfo::Probe(info)->GetPackageName(&name))
        FAIL_RETURN(pm->GetApplicationInfo(name, IPackageManager::GET_META_DATA, (IApplicationInfo**)&ai))

        Int32 flags;
        FAIL_RETURN(ai->GetFlags(&flags))
        if ((flags & IApplicationInfo::FLAG_PRIVILEGED) == 0) {
            // The application isn't privileged (/system/priv-app).
            // The enrollment application needs to be a privileged system app.
            String packageName;
            IPackageItemInfo::Probe(ai)->GetPackageName(&packageName);
            Slogger::W(TAG, packageName + String("is not a privileged system app"));
            continue;
        }
        assert(0 && "TODO: MANAGE_VOICE_KEYPHRASES");
        // if (!Manifest.permission.MANAGE_VOICE_KEYPHRASES.equals(ai.permission)) {
        //     // The application trying to manage keyphrases doesn't
        //     // require the MANAGE_VOICE_KEYPHRASES permission.
        //     Slog.w(TAG, ai.packageName + " does not require MANAGE_VOICE_KEYPHRASES");
        //     continue;
        // }
        FAIL_RETURN(IPackageItemInfo::Probe(ai)->GetPackageName(&mEnrollmentPackage))
        found = TRUE;
        break;
        //} catch (PackageManager.NameNotFoundException e) {
        //    Slog.w(TAG, "error parsing voice enrollment meta-data", e);
        //}
    }

    if (!found) {
        mKeyphrases = NULL;
        mParseError = String("No suitable enrollment application found");
        return NOERROR;
    }

    AutoPtr<IXmlResourceParser> parser;
    //try {
    ECode ec = NOERROR;
    AutoPtr<IResources> res;
    AutoPtr<IAttributeSet> attrs;
    AutoPtr<ITypedArray> array;
    String nodeName;

    ec = IPackageItemInfo::Probe(ai)->LoadXmlMetaData(pm,
            VOICE_KEYPHRASE_META_DATA, (IXmlResourceParser**)&parser);
    FAIL_GOTO(ec, EXIT1)
    if (parser == NULL) {
        String packageName;
        ec = IPackageItemInfo::Probe(ai)->GetPackageName(&packageName);
        FAIL_GOTO(ec, EXIT1)
        mParseError = String("No ") + VOICE_KEYPHRASE_META_DATA + String(" meta-data for ")
                + packageName;
        goto EXIT;
    }

    ec = pm->GetResourcesForApplication(ai, (IResources**)&res);
    FAIL_GOTO(ec, EXIT1)

    attrs = Xml::AsAttributeSet(IXmlPullParser::Probe(parser));
    FAIL_GOTO(ec, EXIT1)

    Int32 type;
    while ((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && type != IXmlPullParser::START_TAG) {
    }

    ec = IXmlPullParser::Probe(parser)->GetName(&nodeName);
    FAIL_GOTO(ec, EXIT1)
    if (!nodeName.Equals("voice-enrollment-application")) {
        mParseError = String("Meta-data does not start with voice-enrollment-application tag");
        goto EXIT;
    }

    assert(0 && "TODO:com.android.internal.R.styleable.VoiceEnrollmentApplication");
    //ec = res->ObtainAttributes(attrs, com.android.internal.R.styleable.VoiceEnrollmentApplication, (ITypedArray**)&array);
    FAIL_GOTO(ec, EXIT1)
    ec = InitializeKeyphrasesFromTypedArray(array);
    FAIL_GOTO(ec, EXIT1)
    ec = array->Recycle();
    FAIL_GOTO(ec, EXIT1)
    goto EXIT;
    // } catch (XmlPullParserException e) {
    //     mParseError = "Error parsing keyphrase enrollment meta-data: " + e;
    //     Slog.w(TAG, "error parsing keyphrase enrollment meta-data", e);
    //     return;
    // } catch (IOException e) {
    //     mParseError = "Error parsing keyphrase enrollment meta-data: " + e;
    //     Slog.w(TAG, "error parsing keyphrase enrollment meta-data", e);
    //     return;
    // } catch (PackageManager.NameNotFoundException e) {
    //     mParseError = "Error parsing keyphrase enrollment meta-data: " + e;
    //     Slog.w(TAG, "error parsing keyphrase enrollment meta-data", e);
    //     return;
    // } finally {
    //     if (parser != null) parser.close();
    // }

EXIT1:
    if (ec == (ECode)E_XML_PULL_PARSER_EXCEPTION) {
        mParseError = String("Error parsing keyphrase enrollment meta-data: ") + StringUtils::ToString(ec);
        Slogger::W(TAG, String("error parsing keyphrase enrollment meta-data") + StringUtils::ToString(ec));
    }
    if (ec == (ECode)E_IO_EXCEPTION) {
        mParseError = String("Error parsing keyphrase enrollment meta-data: ") + StringUtils::ToString(ec);
        Slogger::W(TAG, String("error parsing keyphrase enrollment meta-data") + StringUtils::ToString(ec));
    }
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        mParseError = String("Error parsing keyphrase enrollment meta-data: ") + StringUtils::ToString(ec);
        Slogger::W(TAG, String("error parsing keyphrase enrollment meta-data") + StringUtils::ToString(ec));
    }

EXIT:
    if (parser != NULL) {
        ICloseable::Probe(parser)->Close();
    }
    return ec;
}

ECode KeyphraseEnrollmentInfo::InitializeKeyphrasesFromTypedArray(
    /* [in] */ ITypedArray* array)
{
    // Get the keyphrase ID.
    Int32 searchKeyphraseId;
    FAIL_RETURN(array->GetInt32(
            R::styleable::VoiceEnrollmentApplication_searchKeyphraseId, -1, &searchKeyphraseId))
    if (searchKeyphraseId <= 0) {
        mParseError = String("No valid searchKeyphraseId specified in meta-data");
        Slogger::W(TAG, mParseError);
        return NOERROR;
    }

    // Get the keyphrase text.
    String searchKeyphrase;
    FAIL_RETURN(array->GetString(
            R::styleable::VoiceEnrollmentApplication_searchKeyphrase, &searchKeyphrase))
    if (searchKeyphrase.IsNull()) {
        mParseError = String("No valid searchKeyphrase specified in meta-data");
        Slogger::W(TAG, mParseError);
        return NOERROR;
    }

    // Get the supported locales.
    String searchKeyphraseSupportedLocales;
    FAIL_RETURN(array->GetString(
            R::styleable::VoiceEnrollmentApplication_searchKeyphraseSupportedLocales, &searchKeyphraseSupportedLocales))
    if (searchKeyphraseSupportedLocales.IsNull()) {
        mParseError = String("No valid searchKeyphraseSupportedLocales specified in meta-data");
        Slogger::W(TAG, mParseError);
        return NOERROR;
    }

    AutoPtr<IArraySet> locales;
    CArraySet::New((IArraySet**)&locales);
    ISet* set = ISet::Probe(locales);
    // Try adding locales if the locale string is non-empty.
    if (!TextUtils::IsEmpty(searchKeyphraseSupportedLocales)) {
        //try {
        ECode ec = NOERROR;
        AutoPtr<ArrayOf<String> > supportedLocalesDelimited;
        ec = StringUtils::Split(searchKeyphraseSupportedLocales, ",",
                (ArrayOf<String>**)&supportedLocalesDelimited);
        if (FAILED(ec)) {
            mParseError = String("Error reading searchKeyphraseSupportedLocales from meta-data");
            Slogger::W(TAG, mParseError + StringUtils::ToString(ec));
            return ec;
        }
        for (Int32 i = 0; i < supportedLocalesDelimited->GetLength(); i++) {
            AutoPtr<ILocaleHelper> helper;
            AutoPtr<ILocale> locale;
            CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
            ec = helper->ForLanguageTag((*supportedLocalesDelimited)[i], (ILocale**)&locale);
            if (FAILED(ec)) {
                mParseError = String("Error reading searchKeyphraseSupportedLocales from meta-data");
                Slogger::W(TAG, mParseError + StringUtils::ToString(ec));
                return ec;
            }
            ec = set->Add(TO_IINTERFACE(locale));
            if (FAILED(ec)) {
                mParseError = String("Error reading searchKeyphraseSupportedLocales from meta-data");
                Slogger::W(TAG, mParseError + StringUtils::ToString(ec));
                return ec;
            }
        }
        //} catch (Exception ex) {
        // We catch a generic exception here because we don't want the system service
        // to be affected by a malformed metadata because invalid locales were specified
        // by the system application.
        // mParseError = String("Error reading searchKeyphraseSupportedLocales from meta-data");
        // Slogger::W(TAG, mParseError, ex);
        // return ec;
        //}
    }

    // Get the supported recognition modes.
    Int32 recognitionModes;
    FAIL_RETURN(array->GetInt32(
            R::styleable::VoiceEnrollmentApplication_searchKeyphraseRecognitionFlags, -1, &recognitionModes))
    if (recognitionModes < 0) {
        mParseError = String("No valid searchKeyphraseRecognitionFlags specified in meta-data");
        Slogger::W(TAG, mParseError);
        return NOERROR;
    }
    mKeyphrases = ArrayOf<IKeyphraseMetadata*>::Alloc(1);
    AutoPtr<IKeyphraseMetadata> metadata = new KeyphraseMetadata();
    FAIL_RETURN(((KeyphraseMetadata*)metadata.Get())->constructor(searchKeyphraseId, searchKeyphrase, locales,
            recognitionModes))
    mKeyphrases->Set(0, metadata);

    return NOERROR;
}

ECode KeyphraseEnrollmentInfo::GetParseError(
    /* [out] */ String* error)
{
    VALIDATE_NOT_NULL(error);

    *error = mParseError;
    return NOERROR;
}

ECode KeyphraseEnrollmentInfo::ListKeyphraseMetadata(
    /* [out, callee] */ ArrayOf<IKeyphraseMetadata*>** data)
{
    VALIDATE_NOT_NULL(data);

    *data = mKeyphrases;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode KeyphraseEnrollmentInfo::GetManageKeyphraseIntent(
    /* [in] */ Int32 action,
    /* [in] */ const String& keyphrase,
    /* [in] */ ILocale* locale,
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);

    if (mEnrollmentPackage.IsNull() || mEnrollmentPackage.IsEmpty()) {
        //Slog.w(TAG, "No enrollment application exists");
        *intent = NULL;
        return NOERROR;
    }

    AutoPtr<IKeyphraseMetadata> data;
    FAIL_RETURN(GetKeyphraseMetadata(keyphrase, locale, (IKeyphraseMetadata**)&data))
    if (data != NULL) {
        AutoPtr<IIntent> _intent;
        CIntent::New(ACTION_MANAGE_VOICE_KEYPHRASES, (IIntent**)&_intent);
        FAIL_RETURN(_intent->SetPackage(mEnrollmentPackage))
        FAIL_RETURN(_intent->PutExtra(EXTRA_VOICE_KEYPHRASE_HINT_TEXT, keyphrase))
        String tag;
        FAIL_RETURN(locale->ToLanguageTag(&tag))
        FAIL_RETURN(_intent->PutExtra(EXTRA_VOICE_KEYPHRASE_LOCALE, tag))
        FAIL_RETURN(_intent->PutExtra(EXTRA_VOICE_KEYPHRASE_ACTION, action))
        *intent = _intent;
        REFCOUNT_ADD(*intent);
        return NOERROR;
    }
    *intent = NULL;
    return NOERROR;
}

ECode KeyphraseEnrollmentInfo::GetKeyphraseMetadata(
    /* [in] */ const String& keyphrase,
    /* [in] */ ILocale* locale,
    /* [out] */ IKeyphraseMetadata** data)
{
    VALIDATE_NOT_NULL(data);

    if (mKeyphrases == NULL || mKeyphrases->GetLength() == 0) {
        //Slog.w(TAG, "Enrollment application doesn't support keyphrases");
        *data = NULL;
        return NOERROR;
    }

    for (Int32 i = 0; i < mKeyphrases->GetLength(); i++) {
        // Check if the given keyphrase is supported in the locale provided by
        // the enrollment application.
        AutoPtr<IKeyphraseMetadata> keyphraseMetadata = (*mKeyphrases)[i];
        Boolean result1, result2;
        FAIL_RETURN(keyphraseMetadata->SupportsPhrase(keyphrase, &result1))
        FAIL_RETURN(keyphraseMetadata->SupportsLocale(locale, &result2))
        if (result1 && result2) {
            *data = keyphraseMetadata;
            REFCOUNT_ADD(*data);
            return NOERROR;
        }
    }
    //Slog.w(TAG, "Enrollment application doesn't support the given keyphrase/locale");
    *data = NULL;
    return NOERROR;
}

ECode KeyphraseEnrollmentInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "KeyphraseEnrollmentInfo [Keyphrases=";
    sb += Arrays::ToString(mKeyphrases);
    sb += ", EnrollmentPackage=";
    sb += mEnrollmentPackage;
    sb += ", ParseError=";
    sb += mParseError;
    sb += "]";

    return sb.ToString(str);
}

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos