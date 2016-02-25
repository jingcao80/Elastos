

package com.android.settings.voice;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Service::Voice::IVoiceInteractionService;
using Elastos::Droid::Service::Voice::IVoiceInteractionServiceInfo;
using Elastos::Droid::Speech::IRecognitionService;
using Elastos::Droid::Utility::IArraySet;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::IXml;
using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;

public class VoiceInputHelper {
    static const String TAG = "VoiceInputHelper";
    final Context mContext;

    final List<ResolveInfo> mAvailableVoiceInteractions;
    final List<ResolveInfo> mAvailableRecognition;

    static public class BaseInfo implements Comparable {
        public final ServiceInfo service;
        public final ComponentName componentName;
        public final String key;
        public final ComponentName settings;
        public final CharSequence label;
        public final String labelStr;
        public final CharSequence appLabel;

        public BaseInfo(PackageManager pm, ServiceInfo _service, String _settings) {
            service = _service;
            componentName = new ComponentName(_service.packageName, _service.name);
            key = componentName->FlattenToShortString();
            settings = _settings != NULL
                    ? new ComponentName(_service.packageName, _settings) : NULL;
            label = _service->LoadLabel(pm);
            labelStr = label->ToString();
            appLabel = _service.applicationInfo->LoadLabel(pm);
        }

        //@Override
        public Int32 CompareTo(Object another) {
            return labelStr->CompareTo(((BaseInfo)another).labelStr);
        }
    }

    static public class InteractionInfo extends BaseInfo {
        public final VoiceInteractionServiceInfo serviceInfo;

        public InteractionInfo(PackageManager pm, VoiceInteractionServiceInfo _service) {
            Super(pm, _service->GetServiceInfo(), _service->GetSettingsActivity());
            serviceInfo = _service;
        }
    }

    static public class RecognizerInfo extends BaseInfo {
        public RecognizerInfo(PackageManager pm, ServiceInfo _service, String _settings) {
            Super(pm, _service, _settings);
        }
    }

    final ArrayList<InteractionInfo> mAvailableInteractionInfos = new ArrayList<>();
    final ArrayList<RecognizerInfo> mAvailableRecognizerInfos = new ArrayList<>();

    ComponentName mCurrentVoiceInteraction;
    ComponentName mCurrentRecognizer;

    public VoiceInputHelper(Context context) {
        mContext = context;

        mAvailableVoiceInteractions = mContext->GetPackageManager()->QueryIntentServices(
                        new Intent(VoiceInteractionService.SERVICE_INTERFACE),
                        PackageManager.GET_META_DATA);
        mAvailableRecognition = mContext->GetPackageManager()->QueryIntentServices(
                        new Intent(RecognitionService.SERVICE_INTERFACE),
                        PackageManager.GET_META_DATA);
    }

    public Boolean HasItems() {
        return mAvailableVoiceInteractions->Size() > 0 || mAvailableRecognition->Size() > 0;
    }

    CARAPI BuildUi() {
        // Get the currently selected interactor from the secure setting.
        String currentSetting = Settings.Secure->GetString(
                mContext->GetContentResolver(), Settings.Secure.VOICE_INTERACTION_SERVICE);
        if (currentSetting != NULL && !currentSetting->IsEmpty()) {
            mCurrentVoiceInteraction = ComponentName->UnflattenFromString(currentSetting);
        } else {
            mCurrentVoiceInteraction = NULL;
        }

        ArraySet<ComponentName> interactorRecognizers = new ArraySet<>();

        // Iterate through all the available interactors and load up their info to show
        // in the preference.
        Int32 size = mAvailableVoiceInteractions->Size();
        for (Int32 i = 0; i < size; i++) {
            ResolveInfo resolveInfo = mAvailableVoiceInteractions->Get(i);
            VoiceInteractionServiceInfo info = new VoiceInteractionServiceInfo(
                    mContext->GetPackageManager(), resolveInfo.serviceInfo);
            if (info->GetParseError() != NULL) {
                Logger::W("VoiceInteractionService", "Error in VoiceInteractionService "
                        + resolveInfo.serviceInfo.packageName + "/"
                        + resolveInfo.serviceInfo.name + ": " + info->GetParseError());
                continue;
            }
            mAvailableInteractionInfos->Add(new InteractionInfo(mContext->GetPackageManager(), info));
            if (info->GetRecognitionService() != NULL) {
                interactorRecognizers->Add(new ComponentName(resolveInfo.serviceInfo.packageName,
                        info->GetRecognitionService()));
            }
        }
        Collections->Sort(mAvailableInteractionInfos);

        // Get the currently selected recognizer from the secure setting.
        currentSetting = Settings.Secure->GetString(
                mContext->GetContentResolver(), Settings.Secure.VOICE_RECOGNITION_SERVICE);
        if (currentSetting != NULL && !currentSetting->IsEmpty()) {
            mCurrentRecognizer = ComponentName->UnflattenFromString(currentSetting);
        } else {
            mCurrentRecognizer = NULL;
        }

        // Iterate through all the available recognizers and load up their info to show
        // in the preference.
        size = mAvailableRecognition->Size();
        for (Int32 i = 0; i < size; i++) {
            ResolveInfo resolveInfo = mAvailableRecognition->Get(i);
            ComponentName comp = new ComponentName(resolveInfo.serviceInfo.packageName,
                    resolveInfo.serviceInfo.name);
            if (interactorRecognizers->Contains(comp)) {
                //continue;
            }
            ServiceInfo si = resolveInfo.serviceInfo;
            XmlResourceParser parser = NULL;
            String settingsActivity = NULL;
            try {
                parser = si->LoadXmlMetaData(mContext->GetPackageManager(),
                        RecognitionService.SERVICE_META_DATA);
                if (parser == NULL) {
                    throw new XmlPullParserException("No " + RecognitionService.SERVICE_META_DATA +
                            " meta-data for " + si.packageName);
                }

                Resources res = mContext->GetPackageManager()->GetResourcesForApplication(
                        si.applicationInfo);

                AttributeSet attrs = Xml->AsAttributeSet(parser);

                Int32 type;
                while ((type=parser->Next()) != XmlPullParser.END_DOCUMENT
                        && type != XmlPullParser.START_TAG) {
                }

                String nodeName = parser->GetName();
                if (!"recognition-service".Equals(nodeName)) {
                    throw new XmlPullParserException(
                            "Meta-data does not start with recognition-service tag");
                }

                TypedArray array = res->ObtainAttributes(attrs,
                        R.styleable.RecognitionService);
                settingsActivity = array->GetString(
                        R.styleable.RecognitionService_settingsActivity);
                array->Recycle();
            } catch (XmlPullParserException e) {
                Logger::E(TAG, "error parsing recognition service meta-data", e);
            } catch (IOException e) {
                Logger::E(TAG, "error parsing recognition service meta-data", e);
            } catch (PackageManager.NameNotFoundException e) {
                Logger::E(TAG, "error parsing recognition service meta-data", e);
            } finally {
                if (parser != NULL) parser->Close();
            }
            mAvailableRecognizerInfos->Add(new RecognizerInfo(mContext->GetPackageManager(),
                    resolveInfo.serviceInfo, settingsActivity));
        }
        Collections->Sort(mAvailableRecognizerInfos);
    }
}
