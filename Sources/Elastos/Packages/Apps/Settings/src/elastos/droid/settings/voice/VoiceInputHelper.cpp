
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Speech.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.External.h"
#include "elastos/droid/settings/voice/VoiceInputHelper.h"
#include "elastos/droid/R.h"
#include "elastos/droid/utility/Xml.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IComponentInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Service::Voice::IVoiceInteractionService;
using Elastos::Droid::Service::Voice::CVoiceInteractionServiceInfo;
using Elastos::Droid::Speech::IRecognitionService;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::CArraySet;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::Xml;
using Elastos::Core::EIID_IComparable;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Utility::Logging::Logger;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Voice {

const String VoiceInputHelper::TAG("VoiceInputHelper");

//===============================================================================
//                  VoiceInputHelper::BaseInfo
//===============================================================================

CAR_INTERFACE_IMPL(VoiceInputHelper::BaseInfo, Object, IComparable)

VoiceInputHelper::BaseInfo::BaseInfo()
{}

VoiceInputHelper::BaseInfo::~BaseInfo()
{}

ECode VoiceInputHelper::BaseInfo::constructor(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IServiceInfo* _service,
    /* [in] */ const String& _settings)
{
    mService = _service;
    AutoPtr<IPackageItemInfo> pii = IPackageItemInfo::Probe(_service);
    String packageName, name;
    pii->GetPackageName(&packageName);
    pii->GetName(&name);
    CComponentName::New(packageName, name, (IComponentName**)&mComponentName);
    mComponentName->FlattenToShortString(&mKey);
    if (!_settings.IsNull()) {
        CComponentName::New(packageName, _settings, (IComponentName**)&mSettings);
    }
    pii->LoadLabel(pm, (ICharSequence**)&mLabel);
    mLabel->ToString(&mLabelStr);
    AutoPtr<IApplicationInfo> applicationInfo;
    IComponentInfo::Probe(_service)->GetApplicationInfo(
            (IApplicationInfo**)&applicationInfo);
    IPackageItemInfo::Probe(applicationInfo)->LoadLabel(pm,
            (ICharSequence**)&mApplabel);
    return NOERROR;
}

ECode VoiceInputHelper::BaseInfo::CompareTo(
    /* [in] */ IInterface* another,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mLabelStr.Compare(((BaseInfo*)IComparable::Probe(another))->mLabelStr);
    return NOERROR;
}

//===============================================================================
//                  VoiceInputHelper::InteractionInfo
//===============================================================================

VoiceInputHelper::InteractionInfo::InteractionInfo()
{}

VoiceInputHelper::InteractionInfo::~InteractionInfo()
{}

ECode VoiceInputHelper::InteractionInfo::constructor(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IVoiceInteractionServiceInfo* _service)
{
    AutoPtr<IServiceInfo> info;
    _service->GetServiceInfo((IServiceInfo**)&info);
    String str;
    _service->GetSettingsActivity(&str);

    BaseInfo::constructor(pm, info, str);
    mServiceInfo = _service;
    return NOERROR;
}

//===============================================================================
//                  VoiceInputHelper::RecognizerInfo
//===============================================================================

VoiceInputHelper::RecognizerInfo::RecognizerInfo()
{}

VoiceInputHelper::RecognizerInfo::~RecognizerInfo()
{}

ECode VoiceInputHelper::RecognizerInfo::constructor(
    /* [in] */ IPackageManager* pm,
    /* [in] */ IServiceInfo* _service,
    /* [in] */ const String& _settings)
{
    return BaseInfo::constructor(pm, _service, _settings);
}

//===============================================================================
//                  VoiceInputHelper
//===============================================================================

VoiceInputHelper::VoiceInputHelper(
    /* [in] */ IContext* context)
{
    mContext = context;

    CArrayList::New((IArrayList**)&mAvailableInteractionInfos);
    CArrayList::New((IArrayList**)&mAvailableRecognizerInfos);

    AutoPtr<IIntent> intent1, intent2;
    CIntent::New(IVoiceInteractionService::SERVICE_INTERFACE, (IIntent**)&intent1);
    CIntent::New(IRecognitionService::SERVICE_INTERFACE, (IIntent**)&intent2);
    AutoPtr<IPackageManager> pkm;
    mContext->GetPackageManager((IPackageManager**)&pkm);
    pkm->QueryIntentServices(intent1, IPackageManager::GET_META_DATA,
            (IList**)&mAvailableVoiceInteractions);
    pkm->QueryIntentServices(intent2, IPackageManager::GET_META_DATA,
            (IList**)&mAvailableRecognition);
}

Boolean VoiceInputHelper::HasItems()
{
    Int32 size1, size2;
    mAvailableVoiceInteractions->GetSize(&size1);
    mAvailableRecognition->GetSize(&size2);
    return size1 > 0 || size2 > 0;
}

ECode VoiceInputHelper::BuildUi()
{
    // Get the currently selected interactor from the secure setting.
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    AutoPtr<IContentResolver> resolver;
    mContext->GetContentResolver((IContentResolver**)&resolver);
    String currentSetting;
    secure->GetString(
            resolver, ISettingsSecure::VOICE_INTERACTION_SERVICE, &currentSetting);
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    if (!currentSetting.IsNull() && !currentSetting.IsEmpty()) {
        mCurrentVoiceInteraction = NULL;
        helper->UnflattenFromString(currentSetting,
                (IComponentName**)&mCurrentVoiceInteraction);
    }
    else {
        mCurrentVoiceInteraction = NULL;
    }

    AutoPtr<IArraySet> interactorRecognizers;//ArraySet<ComponentName>
    CArraySet::New((IArraySet**)&interactorRecognizers);

    // Iterate through all the available interactors and load up their info to show
    // in the preference.
    Int32 size;
    mAvailableVoiceInteractions->GetSize(&size);

    AutoPtr<IPackageManager> pkm;
    mContext->GetPackageManager((IPackageManager**)&pkm);

    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mAvailableVoiceInteractions->Get(i, (IInterface**)&obj);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);
        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);

        IPackageItemInfo* pii = IPackageItemInfo::Probe(serviceInfo);
        String packageName, name;
        pii->GetPackageName(&packageName);
        pii->GetName(&name);

        AutoPtr<IVoiceInteractionServiceInfo> info;
        CVoiceInteractionServiceInfo::New(pkm, serviceInfo,
                (IVoiceInteractionServiceInfo**)&info);
        String parseError;
        info->GetParseError(&parseError);
        if (!parseError.IsNull()) {
            Logger::W("VoiceInteractionService", "Error in VoiceInteractionService %s/%s: %s",
                    packageName.string(), name.string(), parseError.string());
            continue;
        }
        AutoPtr<InteractionInfo> iif = new InteractionInfo();
        iif->constructor(pkm, info);
        mAvailableInteractionInfos->Add((IComparable*)iif);
        String recognitionService;
        info->GetRecognitionService(&recognitionService);
        if (!recognitionService.IsNull()) {
            AutoPtr<IComponentName> com;
            CComponentName::New(packageName,
                    recognitionService, (IComponentName**)&com);
            interactorRecognizers->Add(com);
        }
    }
    AutoPtr<ICollections> coll;
    CCollections::AcquireSingleton((ICollections**)&coll);
    coll->Sort(IList::Probe(mAvailableInteractionInfos));

    // Get the currently selected recognizer from the secure setting.
    secure->GetString(
            resolver, ISettingsSecure::VOICE_RECOGNITION_SERVICE, &currentSetting);
    if (!currentSetting.IsNull() && !currentSetting.IsEmpty()) {
        mCurrentRecognizer = NULL;
        helper->UnflattenFromString(currentSetting,
                (IComponentName**)&mCurrentRecognizer);
    }
    else {
        mCurrentRecognizer = NULL;
    }

    // Iterate through all the available recognizers and load up their info to show
    // in the preference.
    mAvailableRecognition->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mAvailableRecognition->Get(i, (IInterface**)&obj);
        IResolveInfo* resolveInfo = IResolveInfo::Probe(obj);

        AutoPtr<IServiceInfo> serviceInfo;
        resolveInfo->GetServiceInfo((IServiceInfo**)&serviceInfo);

        IPackageItemInfo* pii = IPackageItemInfo::Probe(serviceInfo);
        String packageName, name;
        pii->GetPackageName(&packageName);
        pii->GetName(&name);

        AutoPtr<IComponentName> comp;
        CComponentName::New(packageName, name, (IComponentName**)&comp);
        Boolean result;
        if (interactorRecognizers->Contains(comp, &result), result) {
            //continue;
        }

        AutoPtr<IServiceInfo> si = serviceInfo;
        AutoPtr<IXmlResourceParser> parser = NULL;
        String settingsActivity;
        // try {
        IPackageItemInfo::Probe(si)->LoadXmlMetaData(pkm,
                IRecognitionService::SERVICE_META_DATA,
                (IXmlResourceParser**)&parser);
        if (parser == NULL) {
            Logger::E(TAG, "No %s meta-data for %s",
                    IRecognitionService::SERVICE_META_DATA.string(),
                    packageName.string());
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        AutoPtr<IApplicationInfo> applicationInfo;
        IComponentInfo::Probe(si)->GetApplicationInfo(
                (IApplicationInfo**)&applicationInfo);
        AutoPtr<IResources> res;
        pkm->GetResourcesForApplication(applicationInfo, (IResources**)&res);

        IXmlPullParser* xpp = IXmlPullParser::Probe(parser);
        AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(xpp);

        Int32 type;
        while ((xpp->Next(&type), type) != IXmlPullParser::END_DOCUMENT
                && type != IXmlPullParser::START_TAG) {
        }

        String nodeName;
        xpp->GetName(&nodeName);
        if (!nodeName.Equals("recognition-service")) {
            if (parser != NULL) parser->Close();
            Logger::E(TAG, "Meta-data does not start with recognition-service tag");
            return E_XML_PULL_PARSER_EXCEPTION;
        }

        AutoPtr< ArrayOf<Int32> > attrsId = ArrayOf<Int32>::Alloc(
                const_cast<Int32*>(Elastos::Droid::R::styleable::RecognitionService),
                ArraySize(Elastos::Droid::R::styleable::RecognitionService));
        AutoPtr<ITypedArray> array;
        res->ObtainAttributes(attrs, attrsId, (ITypedArray**)&array);
        array->GetString(
                Elastos::Droid::R::styleable::RecognitionService_settingsActivity,
                &settingsActivity);
        array->Recycle();
        // } catch (XmlPullParserException e) {
        //     Logger::E(TAG, "error parsing recognition service meta-data", e);
        // } catch (IOException e) {
        //     Logger::E(TAG, "error parsing recognition service meta-data", e);
        // } catch (PackageManager.NameNotFoundException e) {
        //     Logger::E(TAG, "error parsing recognition service meta-data", e);
        // } finally {
        if (parser != NULL) parser->Close();
        // }
        AutoPtr<RecognizerInfo> rif = new RecognizerInfo();
        rif->constructor(pkm, serviceInfo, settingsActivity);
        mAvailableRecognizerInfos->Add((IComparable*)rif);
    }
    coll->Sort(IList::Probe(mAvailableRecognizerInfos));
    return NOERROR;
}

} // namespace Voice
} // namespace Settings
} // namespace Droid
} // namespace Elastos
