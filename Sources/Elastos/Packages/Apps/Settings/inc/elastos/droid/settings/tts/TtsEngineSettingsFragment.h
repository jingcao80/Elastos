

package com.android.settings.tts;

using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Speech::Tts::ITextToSpeech;
using Elastos::Droid::Speech::Tts::ITtsEngines;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::IPair;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::ILocale;


public class TtsEngineSettingsFragment extends SettingsPreferenceFragment implements
        OnPreferenceClickListener, OnPreferenceChangeListener {
    private static const String TAG = "TtsEngineSettings";
    private static const Boolean DBG = FALSE;

    private static const String KEY_ENGINE_LOCALE = "tts_default_lang";
    private static const String KEY_ENGINE_SETTINGS = "tts_engine_settings";
    private static const String KEY_INSTALL_DATA = "tts_install_data";

    private static const String STATE_KEY_LOCALE_ENTRIES = "locale_entries";
    private static const String STATE_KEY_LOCALE_ENTRY_VALUES= "locale_entry_values";
    private static const String STATE_KEY_LOCALE_VALUE = "locale_value";

    private static const Int32 VOICE_DATA_INTEGRITY_CHECK = 1977;

    private TtsEngines mEnginesHelper;
    private ListPreference mLocalePreference;
    private Preference mEngineSettingsPreference;
    private Preference mInstallVoicesPreference;
    private Intent mEngineSettingsIntent;
    private Intent mVoiceDataDetails;

    private TextToSpeech mTts;

    private Int32 mSelectedLocaleIndex = -1;

    private final TextToSpeech.OnInitListener mTtsInitListener = new TextToSpeech->OnInitListener() {
        //@Override
        CARAPI OnInit(Int32 status) {
            if (status != TextToSpeech.SUCCESS) {
                FinishFragment();
            } else {
                GetActivity()->RunOnUiThread(new Runnable() {
                    //@Override
                    CARAPI Run() {
                        mLocalePreference->SetEnabled(TRUE);
                    }
                });
            }
        }
    };

    private final BroadcastReceiver mLanguagesChangedReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            // Installed or uninstalled some data packs
            if (TextToSpeech.Engine.ACTION_TTS_DATA_INSTALLED->Equals(intent->GetAction())) {
                CheckTtsData();
            }
        }
    };

    public TtsEngineSettingsFragment() {
        Super();
    }

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        AddPreferencesFromResource(R.xml.tts_engine_settings);
        mEnginesHelper = new TtsEngines(GetActivity());

        final PreferenceScreen root = GetPreferenceScreen();
        mLocalePreference = (ListPreference) root->FindPreference(KEY_ENGINE_LOCALE);
        mLocalePreference->SetOnPreferenceChangeListener(this);
        mEngineSettingsPreference = root->FindPreference(KEY_ENGINE_SETTINGS);
        mEngineSettingsPreference->SetOnPreferenceClickListener(this);
        mInstallVoicesPreference = root->FindPreference(KEY_INSTALL_DATA);
        mInstallVoicesPreference->SetOnPreferenceClickListener(this);

        root->SetTitle(GetEngineLabel());
        root->SetKey(GetEngineName());
        mEngineSettingsPreference->SetTitle(GetResources()->GetString(
                R::string::tts_engine_settings_title, GetEngineLabel()));

        mEngineSettingsIntent = mEnginesHelper->GetSettingsIntent(GetEngineName());
        if (mEngineSettingsIntent == NULL) {
            mEngineSettingsPreference->SetEnabled(FALSE);
        }
        mInstallVoicesPreference->SetEnabled(FALSE);

        if (savedInstanceState == NULL) {
            mLocalePreference->SetEnabled(FALSE);
            mLocalePreference->SetEntries(new CharSequence[0]);
            mLocalePreference->SetEntryValues(new CharSequence[0]);
        } else {
            // Repopulate mLocalePreference with saved state. Will be updated later with
            // up-to-date values when CheckTtsData() calls back with results.
            final CharSequence[] entries =
                    savedInstanceState->GetCharSequenceArray(STATE_KEY_LOCALE_ENTRIES);
            final CharSequence[] entryValues =
                    savedInstanceState->GetCharSequenceArray(STATE_KEY_LOCALE_ENTRY_VALUES);
            final CharSequence value =
                    savedInstanceState->GetCharSequence(STATE_KEY_LOCALE_VALUE);

            mLocalePreference->SetEntries(entries);
            mLocalePreference->SetEntryValues(entryValues);
            mLocalePreference->SetValue(value != NULL ? value->ToString() : NULL);
            mLocalePreference->SetEnabled(entries.length > 0);
        }

        mVoiceDataDetails = GetArguments()->GetParcelable(TtsEnginePreference.FRAGMENT_ARGS_VOICES);

        mTts = new TextToSpeech(GetActivity()->GetApplicationContext(), mTtsInitListener,
                GetEngineName());

        // Check if data packs changed
        CheckTtsData();

        GetActivity()->RegisterReceiver(mLanguagesChangedReceiver,
                new IntentFilter(TextToSpeech.Engine.ACTION_TTS_DATA_INSTALLED));
    }

    //@Override
    CARAPI OnDestroy() {
        GetActivity()->UnregisterReceiver(mLanguagesChangedReceiver);
        mTts->Shutdown();
        super->OnDestroy();
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);

        // Save the mLocalePreference values, so we can repopulate it with entries.
        outState->PutCharSequenceArray(STATE_KEY_LOCALE_ENTRIES,
                mLocalePreference->GetEntries());
        outState->PutCharSequenceArray(STATE_KEY_LOCALE_ENTRY_VALUES,
                mLocalePreference->GetEntryValues());
        outState->PutCharSequence(STATE_KEY_LOCALE_VALUE,
                mLocalePreference->GetValue());
    }

    private final void CheckTtsData() {
        Intent intent = new Intent(TextToSpeech.Engine.ACTION_CHECK_TTS_DATA);
        intent->SetPackage(GetEngineName());
        try {
            if (DBG) Logger::D(TAG, "Updating engine: Checking voice data: " + intent->ToUri(0));
            StartActivityForResult(intent, VOICE_DATA_INTEGRITY_CHECK);
        } catch (ActivityNotFoundException ex) {
            Logger::E(TAG, "Failed to check TTS data, no activity found for " + intent + ")");
        }
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        if (requestCode == VOICE_DATA_INTEGRITY_CHECK) {
            if (resultCode != TextToSpeech.Engine.CHECK_VOICE_DATA_FAIL) {
                UpdateVoiceDetails(data);
            } else {
                Logger::E(TAG, "CheckVoiceData activity failed");
            }
        }
    }

    private void UpdateVoiceDetails(Intent data) {
        if (data == NULL){
            Logger::E(TAG, "Engine failed voice data integrity check (NULL return)" +
                    mTts->GetCurrentEngine());
            return;
        }
        mVoiceDataDetails = data;

        if (DBG) Logger::D(TAG, "Parsing voice data details, data: " + mVoiceDataDetails->ToUri(0));

        final ArrayList<String> available = mVoiceDataDetails->GetStringArrayListExtra(
                TextToSpeech.Engine.EXTRA_AVAILABLE_VOICES);
        final ArrayList<String> unavailable = mVoiceDataDetails->GetStringArrayListExtra(
                TextToSpeech.Engine.EXTRA_UNAVAILABLE_VOICES);

        if (unavailable != NULL && unavailable->Size() > 0) {
            mInstallVoicesPreference->SetEnabled(TRUE);
        } else {
            mInstallVoicesPreference->SetEnabled(FALSE);
        }

        if (available == NULL){
            Logger::E(TAG, "TTS data check failed (available == NULL).");
            mLocalePreference->SetEnabled(FALSE);
            return;
        } else {
            UpdateDefaultLocalePref(available);
        }
    }

    private void UpdateDefaultLocalePref(ArrayList<String> availableLangs) {
        if (availableLangs == NULL || availableLangs->Size() == 0) {
            mLocalePreference->SetEnabled(FALSE);
            return;
        }
        Locale currentLocale = NULL;
        if (!mEnginesHelper->IsLocaleSetToDefaultForEngine(GetEngineName())) {
            currentLocale = mEnginesHelper->GetLocalePrefForEngine(GetEngineName());
        }

        ArrayList<Pair<String, Locale>> entryPairs =
                new ArrayList<Pair<String, Locale>>(availableLangs->Size());
        for (Int32 i = 0; i < availableLangs->Size(); i++) {
            Locale locale = mEnginesHelper->ParseLocaleString(availableLangs->Get(i));
            if (locale != NULL){
                entryPairs->Add(new Pair<String, Locale>(
                        locale->GetDisplayName(), locale));
            }
        }

        // Sort it
        Collections->Sort(entryPairs, new Comparator<Pair<String, Locale>>() {
            //@Override
            public Int32 Compare(Pair<String, Locale> lhs, Pair<String, Locale> rhs) {
                return lhs.first->CompareToIgnoreCase(rhs.first);
            }
        });

        // Get two arrays out of one of pairs
        mSelectedLocaleIndex = 0; // Will point to the R::string::tts_lang_use_system value
        CharSequence[] entries = new CharSequence[availableLangs->Size()+1];
        CharSequence[] entryValues = new CharSequence[availableLangs->Size()+1];

        entries[0] = GetActivity()->GetString(R::string::tts_lang_use_system);
        entryValues[0] = "";

        Int32 i = 1;
        for (Pair<String, Locale> entry : entryPairs) {
            if (entry.second->Equals(currentLocale)) {
                mSelectedLocaleIndex = i;
            }
            entries[i] = entry.first;
            entryValues[i++] = entry.second->ToString();
        }

        mLocalePreference->SetEntries(entries);
        mLocalePreference->SetEntryValues(entryValues);
        mLocalePreference->SetEnabled(TRUE);
        SetLocalePreference(mSelectedLocaleIndex);
    }

    /** Set entry from entry table in mLocalePreference */
    private void SetLocalePreference(Int32 index) {
        if (index < 0) {
            mLocalePreference->SetValue("");
            mLocalePreference->SetSummary(R::string::tts_lang_not_selected);
        } else {
            mLocalePreference->SetValueIndex(index);
            mLocalePreference->SetSummary(mLocalePreference->GetEntries()[index]);
        }
    }

    /**
     * Ask the current default engine to launch the matching INSTALL_TTS_DATA activity
     * so the required TTS files are properly installed.
     */
    private void InstallVoiceData() {
        if (TextUtils->IsEmpty(GetEngineName())) return;
        Intent intent = new Intent(TextToSpeech.Engine.ACTION_INSTALL_TTS_DATA);
        intent->SetPackage(GetEngineName());
        try {
            Logger::V(TAG, "Installing voice data: " + intent->ToUri(0));
            StartActivity(intent);
        } catch (ActivityNotFoundException ex) {
            Logger::E(TAG, "Failed to install TTS data, no acitivty found for " + intent + ")");
        }
    }

    //@Override
    public Boolean OnPreferenceClick(Preference preference) {
        if (preference == mInstallVoicesPreference) {
            InstallVoiceData();
            return TRUE;
        } else if (preference == mEngineSettingsPreference) {
            StartActivity(mEngineSettingsIntent);
            return TRUE;
        }

        return FALSE;
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        if (preference == mLocalePreference) {
            String localeString = (String) newValue;
            UpdateLanguageTo((!TextUtils->IsEmpty(localeString) ?
                    mEnginesHelper->ParseLocaleString(localeString) : NULL));
            return TRUE;
        }
        return FALSE;
    }

    private void UpdateLanguageTo(Locale locale) {
        Int32 selectedLocaleIndex = -1;
        String localeString = (locale != NULL) ? locale->ToString() : "";
        for (Int32 i=0; i < mLocalePreference->GetEntryValues().length; i++) {
            if (localeString->EqualsIgnoreCase(mLocalePreference->GetEntryValues()[i].ToString())) {
                selectedLocaleIndex = i;
                break;
            }
        }

        if (selectedLocaleIndex == -1) {
            Logger::W(TAG, "updateLanguageTo called with unknown locale argument");
            return;
        }
        mLocalePreference->SetSummary(mLocalePreference->GetEntries()[selectedLocaleIndex]);
        mSelectedLocaleIndex = selectedLocaleIndex;

        mEnginesHelper->UpdateLocalePrefForEngine(GetEngineName(), locale);

        if (GetEngineName()->Equals(mTts->GetCurrentEngine())) {
            // Null locale means "use system default"
            mTts->SetLanguage((locale != NULL) ? locale : Locale->GetDefault());
        }
    }

    private String GetEngineName() {
        return GetArguments()->GetString(TtsEnginePreference.FRAGMENT_ARGS_NAME);
    }

    private String GetEngineLabel() {
        return GetArguments()->GetString(TtsEnginePreference.FRAGMENT_ARGS_LABEL);
    }
}
