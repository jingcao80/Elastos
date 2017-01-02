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

package com.android.settings.tts;

using static android::Provider::Settings::Secure::ITTS_DEFAULT_RATE;
using static android::Provider::Settings::Secure::ITTS_DEFAULT_SYNTH;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::Tts::TtsEnginePreference::IRadioButtonGroupState;

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Speech::Tts::ITextToSpeech;
using Elastos::Droid::Speech::Tts::IUtteranceProgressListener;
using Elastos::Droid::Speech::Tts::TextToSpeech::IEngineInfo;
using Elastos::Droid::Speech::Tts::ITtsEngines;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::ICheckable;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::IObjects;
using Elastos::Utility::ISet;

public class TextToSpeechSettings extends SettingsPreferenceFragment implements
        Preference.OnPreferenceChangeListener, Preference.OnPreferenceClickListener,
        RadioButtonGroupState {

    private static const String TAG = "TextToSpeechSettings";
    private static const Boolean DBG = FALSE;

    /** Preference key for the "play TTS example" preference. */
    private static const String KEY_PLAY_EXAMPLE = "tts_play_example";

    /** Preference key for the TTS rate selection dialog. */
    private static const String KEY_DEFAULT_RATE = "tts_default_rate";

    /** Preference key for the TTS status field. */
    private static const String KEY_STATUS = "tts_status";

    /**
     * Preference key for the engine selection preference.
     */
    private static const String KEY_ENGINE_PREFERENCE_SECTION =
            "tts_engine_preference_section";

    /**
     * These look like birth years, but they aren't mine. I'm much younger than this.
     */
    private static const Int32 GET_SAMPLE_TEXT = 1983;
    private static const Int32 VOICE_DATA_INTEGRITY_CHECK = 1977;

    private PreferenceCategory mEnginePreferenceCategory;
    private ListPreference mDefaultRatePref;
    private Preference mPlayExample;
    private Preference mEngineStatus;

    private Int32 mDefaultRate = TextToSpeech.Engine.DEFAULT_RATE;

    /**
     * The currently selected engine.
     */
    private String mCurrentEngine;

    /**
     * The engine checkbox that is currently checked. Saves us a bit of effort
     * in deducing the right one from the currently selected engine.
     */
    private Checkable mCurrentChecked;

    /**
     * The previously selected TTS engine. Useful for rollbacks if the users
     * choice is not loaded or fails a voice integrity check.
     */
    private String mPreviousEngine;

    private TextToSpeech mTts = NULL;
    private TtsEngines mEnginesHelper = NULL;

    private String mSampleText = NULL;

    /**
     * Default locale used by selected TTS engine, NULL if not connected to any engine.
     */
    private Locale mCurrentDefaultLocale;

    /**
     * List of available locals of selected TTS engine, as returned by
     * {@link TextToSpeech.Engine#ACTION_CHECK_TTS_DATA} activity. If empty, then activity
     * was not yet called.
     */
    private List<String> mAvailableStrLocals;

    /**
     * The initialization listener used when we are initalizing the settings
     * screen for the first time (as opposed to when a user changes his choice
     * of engine).
     */
    private final TextToSpeech.OnInitListener mInitListener = new TextToSpeech->OnInitListener() {
        //@Override
        CARAPI OnInit(Int32 status) {
            OnInitEngine(status);
        }
    };

    /**
     * The initialization listener used when the user changes his choice of
     * engine (as opposed to when then screen is being initialized for the first
     * time).
     */
    private final TextToSpeech.OnInitListener mUpdateListener = new TextToSpeech->OnInitListener() {
        //@Override
        CARAPI OnInit(Int32 status) {
            OnUpdateEngine(status);
        }
    };

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        AddPreferencesFromResource(R.xml.tts_settings);

        GetActivity()->SetVolumeControlStream(TextToSpeech.Engine.DEFAULT_STREAM);

        mPlayExample = FindPreference(KEY_PLAY_EXAMPLE);
        mPlayExample->SetOnPreferenceClickListener(this);
        mPlayExample->SetEnabled(FALSE);

        mEnginePreferenceCategory = (PreferenceCategory) FindPreference(
                KEY_ENGINE_PREFERENCE_SECTION);
        mDefaultRatePref = (ListPreference) FindPreference(KEY_DEFAULT_RATE);

        mEngineStatus = FindPreference(KEY_STATUS);
        UpdateEngineStatus(R::string::tts_status_checking);

        mTts = new TextToSpeech(GetActivity()->GetApplicationContext(), mInitListener);
        mEnginesHelper = new TtsEngines(GetActivity()->GetApplicationContext());

        SetTtsUtteranceProgressListener();
        InitSettings();

        // Prevent restarting the TTS connection on rotation
        SetRetainInstance(TRUE);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        if (mTts == NULL || mCurrentDefaultLocale == NULL) {
            return;
        }
        Locale ttsDefaultLocale = mTts->GetDefaultLanguage();
        if (mCurrentDefaultLocale != NULL && !mCurrentDefaultLocale->Equals(ttsDefaultLocale)) {
            UpdateWidgetState(FALSE);
            CheckDefaultLocale();
        }
    }

    private void SetTtsUtteranceProgressListener() {
        if (mTts == NULL) {
            return;
        }
        mTts->SetOnUtteranceProgressListener(new UtteranceProgressListener() {
            //@Override
            CARAPI OnStart(String utteranceId) {}

            //@Override
            CARAPI OnDone(String utteranceId) {}

            //@Override
            CARAPI OnError(String utteranceId) {
                Logger::E(TAG, "Error while trying to synthesize sample text");
            }
        });
    }

    //@Override
    CARAPI OnDestroy() {
        super->OnDestroy();
        if (mTts != NULL) {
            mTts->Shutdown();
            mTts = NULL;
        }
    }

    private void InitSettings() {
        final ContentResolver resolver = GetContentResolver();

        // Set up the default rate.
        try {
            mDefaultRate = android.provider.Settings.Secure->GetInt(resolver, TTS_DEFAULT_RATE);
        } catch (SettingNotFoundException e) {
            // Default rate setting not found, initialize it
            mDefaultRate = TextToSpeech.Engine.DEFAULT_RATE;
        }
        mDefaultRatePref->SetValue(String->ValueOf(mDefaultRate));
        mDefaultRatePref->SetOnPreferenceChangeListener(this);

        mCurrentEngine = mTts->GetCurrentEngine();

        SettingsActivity activity = NULL;
        if (GetActivity() instanceof SettingsActivity) {
            activity = (SettingsActivity) GetActivity();
        } else {
            throw new IllegalStateException("TextToSpeechSettings used outside a " +
                    "Settings");
        }

        mEnginePreferenceCategory->RemoveAll();

        List<EngineInfo> engines = mEnginesHelper->GetEngines();
        for (EngineInfo engine : engines) {
            TtsEnginePreference enginePref = new TtsEnginePreference(GetActivity(), engine,
                    this, activity);
            mEnginePreferenceCategory->AddPreference(enginePref);
        }

        CheckVoiceData(mCurrentEngine);
    }

    /**
     * Called when the TTS engine is initialized.
     */
    CARAPI OnInitEngine(Int32 status) {
        if (status == TextToSpeech.SUCCESS) {
            if (DBG) Logger::D(TAG, "TTS engine for settings screen initialized.");
            CheckDefaultLocale();
        } else {
            if (DBG) Logger::D(TAG, "TTS engine for settings screen failed to initialize successfully.");
            UpdateWidgetState(FALSE);
        }
    }

    private void CheckDefaultLocale() {
        Locale defaultLocale = mTts->GetDefaultLanguage();
        if (defaultLocale == NULL) {
            Logger::E(TAG, "Failed to get default language from engine " + mCurrentEngine);
            UpdateWidgetState(FALSE);
            UpdateEngineStatus(R::string::tts_status_not_supported);
            return;
        }

        // ISO-3166 alpha 3 country codes are out of spec. If we won't normalize,
        // we may end up with English (USA)and German (DEU).
        final Locale oldDefaultLocale = mCurrentDefaultLocale;
        mCurrentDefaultLocale = mEnginesHelper->ParseLocaleString(defaultLocale->ToString());
        if (!Objects->Equals(oldDefaultLocale, mCurrentDefaultLocale)) {
            mSampleText = NULL;
        }

        Int32 defaultAvailable = mTts->SetLanguage(defaultLocale);
        if (EvaluateDefaultLocale() && mSampleText == NULL) {
            GetSampleText();
        }
    }

    private Boolean EvaluateDefaultLocale() {
        // Check if we are connected to the engine, and CHECK_VOICE_DATA returned list
        // of available languages.
        if (mCurrentDefaultLocale == NULL || mAvailableStrLocals == NULL) {
            return FALSE;
        }

        Boolean notInAvailableLangauges = TRUE;
        try {
            // Check if language is listed in CheckVoices Action result as available voice.
            String defaultLocaleStr = mCurrentDefaultLocale->GetISO3Language();
            if (!TextUtils->IsEmpty(mCurrentDefaultLocale->GetISO3Country())) {
                defaultLocaleStr += "-" + mCurrentDefaultLocale->GetISO3Country();
            }
            if (!TextUtils->IsEmpty(mCurrentDefaultLocale->GetVariant())) {
                defaultLocaleStr += "-" + mCurrentDefaultLocale->GetVariant();
            }

            for (String loc : mAvailableStrLocals) {
                if (loc->EqualsIgnoreCase(defaultLocaleStr)) {
                  notInAvailableLangauges = FALSE;
                  break;
                }
            }
        } catch (MissingResourceException e) {
            if (DBG) Log->Wtf(TAG, "MissingResourceException", e);
            UpdateEngineStatus(R::string::tts_status_not_supported);
            UpdateWidgetState(FALSE);
            return FALSE;
        }

        Int32 defaultAvailable = mTts->SetLanguage(mCurrentDefaultLocale);
        if (defaultAvailable == TextToSpeech.LANG_NOT_SUPPORTED ||
                defaultAvailable == TextToSpeech.LANG_MISSING_DATA ||
                notInAvailableLangauges) {
            if (DBG) Logger::D(TAG, "Default locale for this TTS engine is not supported.");
            UpdateEngineStatus(R::string::tts_status_not_supported);
            UpdateWidgetState(FALSE);
            return FALSE;
        } else {
            if (IsNetworkRequiredForSynthesis()) {
                UpdateEngineStatus(R::string::tts_status_requires_network);
            } else {
                UpdateEngineStatus(R::string::tts_status_ok);
            }
            UpdateWidgetState(TRUE);
            return TRUE;
        }
    }

    /**
     * Ask the current default engine to return a string of sample text to be
     * spoken to the user.
     */
    private void GetSampleText() {
        String currentEngine = mTts->GetCurrentEngine();

        if (TextUtils->IsEmpty(currentEngine)) currentEngine = mTts->GetDefaultEngine();

        // TODO: This is currently a hidden private API. The intent extras
        // and the intent action should be made public if we intend to make this
        // a public API. We fall back to using a canned set of strings if this
        // doesn't work.
        Intent intent = new Intent(TextToSpeech.Engine.ACTION_GET_SAMPLE_TEXT);

        intent->PutExtra("language", mCurrentDefaultLocale->GetLanguage());
        intent->PutExtra("country", mCurrentDefaultLocale->GetCountry());
        intent->PutExtra("variant", mCurrentDefaultLocale->GetVariant());
        intent->SetPackage(currentEngine);

        try {
            if (DBG) Logger::D(TAG, "Getting sample text: " + intent->ToUri(0));
            StartActivityForResult(intent, GET_SAMPLE_TEXT);
        } catch (ActivityNotFoundException ex) {
            Logger::E(TAG, "Failed to get sample text, no activity found for " + intent + ")");
        }
    }

    /**
     * Called when voice data integrity check returns
     */
    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        if (requestCode == GET_SAMPLE_TEXT) {
            OnSampleTextReceived(resultCode, data);
        } else if (requestCode == VOICE_DATA_INTEGRITY_CHECK) {
            OnVoiceDataIntegrityCheckDone(data);
        }
    }

    private String GetDefaultSampleString() {
        if (mTts != NULL && mTts->GetLanguage() != NULL) {
            try {
                final String currentLang = mTts->GetLanguage()->GetISO3Language();
                String[] strings = GetActivity()->GetResources().GetStringArray(
                        R.array.tts_demo_strings);
                String[] langs = GetActivity()->GetResources().GetStringArray(
                        R.array.tts_demo_string_langs);

                for (Int32 i = 0; i < strings.length; ++i) {
                    if (langs[i].Equals(currentLang)) {
                        return strings[i];
                    }
                }
            } catch (MissingResourceException e) {
                if (DBG) Log->Wtf(TAG, "MissingResourceException", e);
                // Ignore and fall back to default sample string
            }
        }
        return GetString(R::string::tts_default_sample_string);
    }

    private Boolean IsNetworkRequiredForSynthesis() {
        Set<String> features = mTts->GetFeatures(mCurrentDefaultLocale);
        if (features == NULL) {
          return FALSE;
        }
        return features->Contains(TextToSpeech.Engine.KEY_FEATURE_NETWORK_SYNTHESIS) &&
                !features->Contains(TextToSpeech.Engine.KEY_FEATURE_EMBEDDED_SYNTHESIS);
    }

    private void OnSampleTextReceived(Int32 resultCode, Intent data) {
        String sample = GetDefaultSampleString();

        if (resultCode == TextToSpeech.LANG_AVAILABLE && data != NULL) {
            if (data != NULL && data->GetStringExtra("sampleText") != NULL) {
                sample = data->GetStringExtra("sampleText");
            }
            if (DBG) Logger::D(TAG, "Got sample text: " + sample);
        } else {
            if (DBG) Logger::D(TAG, "Using default sample text :" + sample);
        }

        mSampleText = sample;
        if (mSampleText != NULL) {
            UpdateWidgetState(TRUE);
        } else {
            Logger::E(TAG, "Did not have a sample string for the requested language. Using default");
        }
    }

    private void SpeakSampleText() {
        final Boolean networkRequired = IsNetworkRequiredForSynthesis();
        if (!networkRequired || networkRequired &&
                (mTts->IsLanguageAvailable(mCurrentDefaultLocale) >= TextToSpeech.LANG_AVAILABLE)) {
            HashMap<String, String> params = new HashMap<String, String>();
            params->Put(TextToSpeech.Engine.KEY_PARAM_UTTERANCE_ID, "Sample");

            mTts->Speak(mSampleText, TextToSpeech.QUEUE_FLUSH, params);
        } else {
            Logger::W(TAG, "Network required for sample synthesis for requested language");
            DisplayNetworkAlert();
        }
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object objValue) {
        if (KEY_DEFAULT_RATE->Equals(preference->GetKey())) {
            // Default rate
            mDefaultRate = Integer->ParseInt((String) objValue);
            try {
                android.provider.Settings.Secure->PutInt(GetContentResolver(),
                        TTS_DEFAULT_RATE, mDefaultRate);
                if (mTts != NULL) {
                    mTts->SetSpeechRate(mDefaultRate / 100.0f);
                }
                if (DBG) Logger::D(TAG, "TTS default rate changed, now " + mDefaultRate);
            } catch (NumberFormatException e) {
                Logger::E(TAG, "could not persist default TTS rate setting", e);
            }
        }

        return TRUE;
    }

    /**
     * Called when mPlayExample is clicked
     */
    //@Override
    public Boolean OnPreferenceClick(Preference preference) {
        if (preference == mPlayExample) {
            // Get the sample text from the TTS engine; onActivityResult will do
            // the actual speaking
            SpeakSampleText();
            return TRUE;
        }

        return FALSE;
    }

    private void UpdateWidgetState(Boolean enable) {
        mPlayExample->SetEnabled(enable);
        mDefaultRatePref->SetEnabled(enable);
        mEngineStatus->SetEnabled(enable);
    }

    private void UpdateEngineStatus(Int32 resourceId) {
        Locale locale = mCurrentDefaultLocale;
        if (locale == NULL) {
            locale = Locale->GetDefault();
        }
        mEngineStatus->SetSummary(GetString(resourceId, locale->GetDisplayName()));
    }

    private void DisplayNetworkAlert() {
        AlertDialog.Builder builder = new AlertDialog->Builder(GetActivity());
        builder->SetTitle(android.R::string::dialog_alert_title)
                .SetMessage(GetActivity()->GetString(R::string::tts_engine_network_required))
                .SetCancelable(FALSE)
                .SetPositiveButton(android.R::string::ok, NULL);

        AlertDialog dialog = builder->Create();
        dialog->Show();
    }

    private void UpdateDefaultEngine(String engine) {
        if (DBG) Logger::D(TAG, "Updating default synth to : " + engine);

        // Disable the "play sample text" preference and the speech
        // rate preference while the engine is being swapped.
        UpdateWidgetState(FALSE);
        UpdateEngineStatus(R::string::tts_status_checking);

        // Keep track of the previous engine that was being used. So that
        // we can reuse the previous engine.
        //
        // Note that if TextToSpeech#getCurrentEngine is not NULL, it means at
        // the very least that we successfully bound to the engine service.
        mPreviousEngine = mTts->GetCurrentEngine();

        // Step 1: Shut down the existing TTS engine.
        if (mTts != NULL) {
            try {
                mTts->Shutdown();
                mTts = NULL;
            } catch (Exception e) {
                Logger::E(TAG, "Error shutting down TTS engine" + e);
            }
        }

        // Step 2: Connect to the new TTS engine.
        // Step 3 is continued on #onUpdateEngine (below) which is called when
        // the app binds successfully to the engine.
        if (DBG) Logger::D(TAG, "Updating engine : Attempting to connect to engine: " + engine);
        mTts = new TextToSpeech(GetActivity()->GetApplicationContext(), mUpdateListener, engine);
        SetTtsUtteranceProgressListener();
    }

    /*
     * Step 3: We have now bound to the TTS engine the user requested. We will
     * attempt to check voice data for the engine if we successfully bound to it,
     * or revert to the previous engine if we didn't.
     */
    CARAPI OnUpdateEngine(Int32 status) {
        if (status == TextToSpeech.SUCCESS) {
            if (DBG) {
                Logger::D(TAG, "Updating engine: Successfully bound to the engine: " +
                        mTts->GetCurrentEngine());
            }
            CheckVoiceData(mTts->GetCurrentEngine());
        } else {
            if (DBG) Logger::D(TAG, "Updating engine: Failed to bind to engine, reverting.");
            if (mPreviousEngine != NULL) {
                // This is guaranteed to at least bind, since mPreviousEngine would be
                // NULL if the previous bind to this engine failed.
                mTts = new TextToSpeech(GetActivity()->GetApplicationContext(), mInitListener,
                        mPreviousEngine);
                SetTtsUtteranceProgressListener();
            }
            mPreviousEngine = NULL;
        }
    }

    /*
     * Step 4: Check whether the voice data for the engine is ok.
     */
    private void CheckVoiceData(String engine) {
        Intent intent = new Intent(TextToSpeech.Engine.ACTION_CHECK_TTS_DATA);
        intent->SetPackage(engine);
        try {
            if (DBG) Logger::D(TAG, "Updating engine: Checking voice data: " + intent->ToUri(0));
            StartActivityForResult(intent, VOICE_DATA_INTEGRITY_CHECK);
        } catch (ActivityNotFoundException ex) {
            Logger::E(TAG, "Failed to check TTS data, no activity found for " + intent + ")");
        }
    }

    /*
     * Step 5: The voice data check is complete.
     */
    private void OnVoiceDataIntegrityCheckDone(Intent data) {
        final String engine = mTts->GetCurrentEngine();

        if (engine == NULL) {
            Logger::E(TAG, "Voice data check complete, but no engine bound");
            return;
        }

        if (data == NULL){
            Logger::E(TAG, "Engine failed voice data integrity check (NULL return)" +
                    mTts->GetCurrentEngine());
            return;
        }

        android.provider.Settings.Secure->PutString(GetContentResolver(), TTS_DEFAULT_SYNTH, engine);

        mAvailableStrLocals = data->GetStringArrayListExtra(
            TextToSpeech.Engine.EXTRA_AVAILABLE_VOICES);
        if (mAvailableStrLocals == NULL) {
            Logger::E(TAG, "Voice data check complete, but no available voices found");
            // Set mAvailableStrLocals to empty list
            mAvailableStrLocals = new ArrayList<String>();
        }
        if (EvaluateDefaultLocale()) {
            GetSampleText();
        }

        final Int32 engineCount = mEnginePreferenceCategory->GetPreferenceCount();
        for (Int32 i = 0; i < engineCount; ++i) {
            final Preference p = mEnginePreferenceCategory->GetPreference(i);
            if (p instanceof TtsEnginePreference) {
                TtsEnginePreference enginePref = (TtsEnginePreference) p;
                if (enginePref->GetKey()->Equals(engine)) {
                    enginePref->SetVoiceDataDetails(data);
                    break;
                }
            }
        }
    }

    //@Override
    public Checkable GetCurrentChecked() {
        return mCurrentChecked;
    }

    //@Override
    public String GetCurrentKey() {
        return mCurrentEngine;
    }

    //@Override
    CARAPI SetCurrentChecked(Checkable current) {
        mCurrentChecked = current;
    }

    //@Override
    CARAPI SetCurrentKey(String key) {
        mCurrentEngine = key;
        UpdateDefaultEngine(mCurrentEngine);
    }

}
