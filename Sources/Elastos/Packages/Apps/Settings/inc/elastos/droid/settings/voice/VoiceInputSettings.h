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

package com.android.settings.voice;

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Service::Voice::IVoiceInteractionService;
using Elastos::Droid::Service::Voice::IVoiceInteractionServiceInfo;
using Elastos::Droid::Speech::IRecognitionService;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;
using Elastos::Droid::Settings::Voice::VoiceInputPreference::IRadioButtonGroupState;

using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Widget::ICheckable;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class VoiceInputSettings extends SettingsPreferenceFragment implements
        Preference.OnPreferenceClickListener, RadioButtonGroupState, Indexable {

    private static const String TAG = "VoiceInputSettings";
    private static const Boolean DBG = FALSE;

    /**
     * Preference key for the engine selection preference.
     */
    private static const String KEY_SERVICE_PREFERENCE_SECTION =
            "voice_service_preference_section";

    private PreferenceCategory mServicePreferenceCategory;

    private CharSequence mInteractorSummary;
    private CharSequence mRecognizerSummary;
    private CharSequence mInteractorWarning;

    /**
     * The currently selected engine.
     */
    private String mCurrentKey;

    /**
     * The engine checkbox that is currently checked. Saves us a bit of effort
     * in deducing the right one from the currently selected engine.
     */
    private Checkable mCurrentChecked;

    private VoiceInputHelper mHelper;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        AddPreferencesFromResource(R.xml.voice_input_settings);

        mServicePreferenceCategory = (PreferenceCategory) FindPreference(
                KEY_SERVICE_PREFERENCE_SECTION);

        mInteractorSummary = GetActivity()->GetText(
                R::string::voice_interactor_preference_summary);
        mRecognizerSummary = GetActivity()->GetText(
                R::string::voice_recognizer_preference_summary);
        mInteractorWarning = GetActivity()->GetText(R::string::voice_interaction_security_warning);
    }

    //@Override
    CARAPI OnStart() {
        super->OnStart();
        InitSettings();
    }

    private void InitSettings() {
        mHelper = new VoiceInputHelper(GetActivity());
        mHelper->BuildUi();

        mServicePreferenceCategory->RemoveAll();

        if (mHelper.mCurrentVoiceInteraction != NULL) {
            mCurrentKey = mHelper.mCurrentVoiceInteraction->FlattenToShortString();
        } else if (mHelper.mCurrentRecognizer != NULL) {
            mCurrentKey = mHelper.mCurrentRecognizer->FlattenToShortString();
        } else {
            mCurrentKey = NULL;
        }

        for (Int32 i=0; i<mHelper.mAvailableInteractionInfos->Size(); i++) {
            VoiceInputHelper.InteractionInfo info = mHelper.mAvailableInteractionInfos->Get(i);
            VoiceInputPreference pref = new VoiceInputPreference(GetActivity(), info,
                    mInteractorSummary, mInteractorWarning, this);
            mServicePreferenceCategory->AddPreference(pref);
        }

        for (Int32 i=0; i<mHelper.mAvailableRecognizerInfos->Size(); i++) {
            VoiceInputHelper.RecognizerInfo info = mHelper.mAvailableRecognizerInfos->Get(i);
            VoiceInputPreference pref = new VoiceInputPreference(GetActivity(), info,
                    mRecognizerSummary, NULL, this);
            mServicePreferenceCategory->AddPreference(pref);
        }
    }

    //@Override
    public Checkable GetCurrentChecked() {
        return mCurrentChecked;
    }

    //@Override
    public String GetCurrentKey() {
        return mCurrentKey;
    }

    //@Override
    CARAPI SetCurrentChecked(Checkable current) {
        mCurrentChecked = current;
    }

    //@Override
    CARAPI SetCurrentKey(String key) {
        mCurrentKey = key;
        for (Int32 i=0; i<mHelper.mAvailableInteractionInfos->Size(); i++) {
            VoiceInputHelper.InteractionInfo info = mHelper.mAvailableInteractionInfos->Get(i);
            if (info.key->Equals(key)) {
                // Put the new value back into secure settings.
                Settings.Secure->PutString(GetActivity()->GetContentResolver(),
                        Settings.Secure.VOICE_INTERACTION_SERVICE, key);
                // Eventually we will require that an interactor always specify a recognizer
                if (info.settings != NULL) {
                    Settings.Secure->PutString(GetActivity()->GetContentResolver(),
                            Settings.Secure.VOICE_RECOGNITION_SERVICE,
                            info.settings->FlattenToShortString());
                }
                return;
            }
        }

        for (Int32 i=0; i<mHelper.mAvailableRecognizerInfos->Size(); i++) {
            VoiceInputHelper.RecognizerInfo info = mHelper.mAvailableRecognizerInfos->Get(i);
            if (info.key->Equals(key)) {
                Settings.Secure->PutString(GetActivity()->GetContentResolver(),
                        Settings.Secure.VOICE_INTERACTION_SERVICE, "");
                Settings.Secure->PutString(GetActivity()->GetContentResolver(),
                        Settings.Secure.VOICE_RECOGNITION_SERVICE, key);
                return;
            }
        }
    }

    //@Override
    public Boolean OnPreferenceClick(Preference preference) {
        if (preference instanceof VoiceInputPreference) {
            ((VoiceInputPreference)preference).DoClick();
        }
        return TRUE;
    }

    // For Search
    public static const Indexable.SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {

            //@Override
            public List<SearchIndexableRaw> GetRawDataToIndex(Context context,
                    Boolean enabled) {

                List<SearchIndexableRaw> indexables = new ArrayList<>();

                final String screenTitle = context->GetString(R::string::voice_input_settings_title);

                SearchIndexableRaw indexable = new SearchIndexableRaw(context);
                indexable.key = "voice_service_preference_section_title";
                indexable.title = context->GetString(R::string::voice_service_preference_section_title);
                indexable.screenTitle = screenTitle;
                indexables->Add(indexable);

                final List<ResolveInfo> voiceInteractions =
                        context->GetPackageManager()->QueryIntentServices(
                                new Intent(VoiceInteractionService.SERVICE_INTERFACE),
                                PackageManager.GET_META_DATA);

                final Int32 countInteractions = voiceInteractions->Size();
                for (Int32 i = 0; i < countInteractions; i++) {
                    ResolveInfo info = voiceInteractions->Get(i);
                    VoiceInteractionServiceInfo visInfo = new VoiceInteractionServiceInfo(
                            context->GetPackageManager(), info.serviceInfo);
                    if (visInfo->GetParseError() != NULL) {
                        continue;
                    }
                    indexables->Add(GetSearchIndexableRaw(context, info, screenTitle));
                }

                final List<ResolveInfo> recognitions =
                        context->GetPackageManager()->QueryIntentServices(
                                new Intent(RecognitionService.SERVICE_INTERFACE),
                                PackageManager.GET_META_DATA);

                final Int32 countRecognitions = recognitions->Size();
                for (Int32 i = 0; i < countRecognitions; i++) {
                    ResolveInfo info = recognitions->Get(i);
                    indexables->Add(GetSearchIndexableRaw(context, info, screenTitle));
                }

                return indexables;
            }

            private SearchIndexableRaw GetSearchIndexableRaw(Context context,
                    ResolveInfo info, String screenTitle) {

                ServiceInfo serviceInfo = info.serviceInfo;
                ComponentName componentName = new ComponentName(serviceInfo.packageName,
                        serviceInfo.name);

                SearchIndexableRaw indexable = new SearchIndexableRaw(context);
                indexable.key = componentName->FlattenToString();
                indexable.title = info->LoadLabel(context->GetPackageManager()).ToString();
                indexable.screenTitle = screenTitle;

                return indexable;
            }
        };
}
