

package com.android.settings.inputmethod;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Hardware::Input::IInputDeviceIdentifier;
using Elastos::Droid::Hardware::Input::IInputManager;
using Elastos::Droid::Hardware::Input::IKeyboardLayout;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Provider::Settings::ISystem;
using Elastos::Droid::Speech::IRecognitionService;
using Elastos::Droid::Speech::Tts::ITtsEngines;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IInputDevice;
using Elastos::Droid::View::Inputmethod::IInputMethodInfo;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::View::Inputmethod::IInputMethodSubtype;
using Elastos::Droid::View::Textservice::ISpellCheckerInfo;
using Elastos::Droid::View::Textservice::ITextServicesManager;

using Elastos::Droid::Internal::App::ILocalePicker;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::Settings::IKeyboardLayoutPickerActivity;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::ISubSettings;
using Elastos::Droid::Settings::IUserDictionarySettings;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::IVoiceInputOutputSettings;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Text::ICollator;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IComparator;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;
using Elastos::Utility::ITreeSet;

public class InputMethodAndLanguageSettings extends SettingsPreferenceFragment
        implements Preference.OnPreferenceChangeListener, InputManager.InputDeviceListener,
        KeyboardLayoutDialogFragment.OnSetupKeyboardLayoutsListener, Indexable,
        InputMethodPreference.OnSavePreferenceListener {
    private static const String KEY_SPELL_CHECKERS = "spellcheckers_settings";
    private static const String KEY_PHONE_LANGUAGE = "phone_language";
    private static const String KEY_CURRENT_INPUT_METHOD = "current_input_method";
    private static const String KEY_INPUT_METHOD_SELECTOR = "input_method_selector";
    private static const String KEY_USER_DICTIONARY_SETTINGS = "key_user_dictionary_settings";
    private static const String KEY_PREVIOUSLY_ENABLED_SUBTYPES = "previously_enabled_subtypes";
    // FALSE: on ICS or later
    private static const Boolean SHOW_INPUT_METHOD_SWITCHER_SETTINGS = FALSE;

    private Int32 mDefaultInputMethodSelectorVisibility = 0;
    private ListPreference mShowInputMethodSelectorPref;
    private PreferenceCategory mKeyboardSettingsCategory;
    private PreferenceCategory mHardKeyboardCategory;
    private PreferenceCategory mGameControllerCategory;
    private Preference mLanguagePref;
    private final ArrayList<InputMethodPreference> mInputMethodPreferenceList = new ArrayList<>();
    private final ArrayList<PreferenceScreen> mHardKeyboardPreferenceList = new ArrayList<>();
    private InputManager mIm;
    private InputMethodManager mImm;
    private Boolean mShowsOnlyFullImeAndKeyboardList;
    private Handler mHandler;
    private SettingsObserver mSettingsObserver;
    private Intent mIntentWaitingForResult;
    private InputMethodSettingValuesWrapper mInputMethodSettingValues;
    private DevicePolicyManager mDpm;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        AddPreferencesFromResource(R.xml.language_settings);

        final Activity activity = GetActivity();
        mImm = (InputMethodManager) GetSystemService(Context.INPUT_METHOD_SERVICE);
        mInputMethodSettingValues = InputMethodSettingValuesWrapper->GetInstance(activity);

        try {
            mDefaultInputMethodSelectorVisibility = Integer->ValueOf(
                    GetString(R::string::input_method_selector_visibility_default_value));
        } catch (NumberFormatException e) {
        }

        if (activity->GetAssets()->GetLocales().length == 1) {
            // No "Select language" pref if there's only one system locale available.
            GetPreferenceScreen()->RemovePreference(FindPreference(KEY_PHONE_LANGUAGE));
        } else {
            mLanguagePref = FindPreference(KEY_PHONE_LANGUAGE);
        }
        if (SHOW_INPUT_METHOD_SWITCHER_SETTINGS) {
            mShowInputMethodSelectorPref = (ListPreference)FindPreference(
                    KEY_INPUT_METHOD_SELECTOR);
            mShowInputMethodSelectorPref->SetOnPreferenceChangeListener(this);
            // TODO: Update current input method name on summary
            UpdateInputMethodSelectorSummary(LoadInputMethodSelectorVisibility());
        }

        new VoiceInputOutputSettings(this).OnCreate();

        // Get references to dynamically constructed categories.
        mHardKeyboardCategory = (PreferenceCategory)FindPreference("hard_keyboard");
        mKeyboardSettingsCategory = (PreferenceCategory)FindPreference(
                "keyboard_settings_category");
        mGameControllerCategory = (PreferenceCategory)FindPreference(
                "game_controller_settings_category");

        final Intent startingIntent = activity->GetIntent();
        // Filter out irrelevant features if invoked from IME settings button.
        mShowsOnlyFullImeAndKeyboardList = Settings.ACTION_INPUT_METHOD_SETTINGS->Equals(
                startingIntent->GetAction());
        if (mShowsOnlyFullImeAndKeyboardList) {
            GetPreferenceScreen()->RemoveAll();
            GetPreferenceScreen()->AddPreference(mHardKeyboardCategory);
            if (SHOW_INPUT_METHOD_SWITCHER_SETTINGS) {
                GetPreferenceScreen()->AddPreference(mShowInputMethodSelectorPref);
            }
            mKeyboardSettingsCategory->RemoveAll();
            GetPreferenceScreen()->AddPreference(mKeyboardSettingsCategory);
        }

        // Build hard keyboard and game controller preference categories.
        mIm = (InputManager)activity->GetSystemService(Context.INPUT_SERVICE);
        UpdateInputDevices();

        // Spell Checker
        final Preference spellChecker = FindPreference(KEY_SPELL_CHECKERS);
        if (spellChecker != NULL) {
            // Note: KEY_SPELL_CHECKERS preference is marked as persistent="FALSE" in XML.
            InputMethodAndSubtypeUtil->RemoveUnnecessaryNonPersistentPreference(spellChecker);
            final Intent intent = new Intent(IIntent::ACTION_MAIN);
            intent->SetClass(activity, SubSettings.class);
            intent->PutExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT,
                    SpellCheckersSettings.class->GetName());
            intent->PutExtra(SettingsActivity.EXTRA_SHOW_FRAGMENT_TITLE_RESID,
                    R::string::spellcheckers_settings_title);
            spellChecker->SetIntent(intent);
        }

        mHandler = new Handler();
        mSettingsObserver = new SettingsObserver(mHandler, activity);
        mDpm = (DevicePolicyManager) (GetActivity().
                GetSystemService(Context.DEVICE_POLICY_SERVICE));

        // If we've launched from the keyboard layout notification, go ahead and just show the
        // keyboard layout dialog.
        final InputDeviceIdentifier identifier =
                startingIntent->GetParcelableExtra(Settings.EXTRA_INPUT_DEVICE_IDENTIFIER);
        if (mShowsOnlyFullImeAndKeyboardList && identifier != NULL) {
            ShowKeyboardLayoutDialog(identifier);
        }
    }

    private void UpdateInputMethodSelectorSummary(Int32 value) {
        String[] inputMethodSelectorTitles = GetResources()->GetStringArray(
                R.array.input_method_selector_titles);
        if (inputMethodSelectorTitles.length > value) {
            mShowInputMethodSelectorPref->SetSummary(inputMethodSelectorTitles[value]);
            mShowInputMethodSelectorPref->SetValue(String->ValueOf(value));
        }
    }

    private void UpdateUserDictionaryPreference(Preference userDictionaryPreference) {
        final Activity activity = GetActivity();
        final TreeSet<String> localeSet = UserDictionaryList->GetUserDictionaryLocalesSet(activity);
        if (NULL == localeSet) {
            // The locale list is NULL if and only if the user dictionary service is
            // not present or disabled. In this case we need to remove the preference.
            GetPreferenceScreen()->RemovePreference(userDictionaryPreference);
        } else {
            userDictionaryPreference->SetOnPreferenceClickListener(
                    new OnPreferenceClickListener() {
                        //@Override
                        public Boolean OnPreferenceClick(Preference arg0) {
                            // Redirect to UserDictionarySettings if the user needs only one
                            // language.
                            final Bundle extras = new Bundle();
                            final Class<? extends Fragment> targetFragment;
                            if (localeSet->Size() <= 1) {
                                if (!localeSet->IsEmpty()) {
                                    // If the size of localeList is 0, we don't set the locale
                                    // parameter in the extras. This will be interpreted by the
                                    // UserDictionarySettings class as meaning
                                    // "the current locale". Note that with the current code for
                                    // UserDictionaryList#GetUserDictionaryLocalesSet()
                                    // the locale list always has at least one element, since it
                                    // always includes the current locale explicitly.
                                    // @see UserDictionaryList->GetUserDictionaryLocalesSet().
                                    extras->PutString("locale", localeSet->First());
                                }
                                targetFragment = UserDictionarySettings.class;
                            } else {
                                targetFragment = UserDictionaryList.class;
                            }
                            StartFragment(InputMethodAndLanguageSettings.this,
                                    targetFragment->GetCanonicalName(), -1, -1, extras);
                            return TRUE;
                        }
                    });
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        mSettingsObserver->Resume();
        mIm->RegisterInputDeviceListener(this, NULL);

        final Preference spellChecker = FindPreference(KEY_SPELL_CHECKERS);
        if (spellChecker != NULL) {
            final TextServicesManager tsm = (TextServicesManager) GetSystemService(
                    Context.TEXT_SERVICES_MANAGER_SERVICE);
            if (tsm->IsSpellCheckerEnabled()) {
                final SpellCheckerInfo sci = tsm->GetCurrentSpellChecker();
                spellChecker->SetSummary(sci->LoadLabel(GetPackageManager()));
            } else {
                spellChecker->SetSummary(R::string::switch_off_text);
            }
        }

        if (!mShowsOnlyFullImeAndKeyboardList) {
            if (mLanguagePref != NULL) {
                String localeName = GetLocaleName(GetActivity());
                mLanguagePref->SetSummary(localeName);
            }

            UpdateUserDictionaryPreference(FindPreference(KEY_USER_DICTIONARY_SETTINGS));
            if (SHOW_INPUT_METHOD_SWITCHER_SETTINGS) {
                mShowInputMethodSelectorPref->SetOnPreferenceChangeListener(this);
            }
        }

        UpdateInputDevices();

        // Refresh internal states in mInputMethodSettingValues to keep the latest
        // "InputMethodInfo"s and "InputMethodSubtype"s
        mInputMethodSettingValues->RefreshAllInputMethodAndSubtypes();
        UpdateInputMethodPreferenceViews();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();

        mIm->UnregisterInputDeviceListener(this);
        mSettingsObserver->Pause();

        if (SHOW_INPUT_METHOD_SWITCHER_SETTINGS) {
            mShowInputMethodSelectorPref->SetOnPreferenceChangeListener(NULL);
        }
        // TODO: Consolidate the logic to InputMethodSettingsWrapper
        InputMethodAndSubtypeUtil->SaveInputMethodSubtypeList(
                this, GetContentResolver(), mInputMethodSettingValues->GetInputMethodList(),
                !mHardKeyboardPreferenceList->IsEmpty());
    }

    //@Override
    CARAPI OnInputDeviceAdded(Int32 deviceId) {
        UpdateInputDevices();
    }

    //@Override
    CARAPI OnInputDeviceChanged(Int32 deviceId) {
        UpdateInputDevices();
    }

    //@Override
    CARAPI OnInputDeviceRemoved(Int32 deviceId) {
        UpdateInputDevices();
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        // Input Method stuff
        if (Utils->IsMonkeyRunning()) {
            return FALSE;
        }
        if (preference instanceof PreferenceScreen) {
            if (preference->GetFragment() != NULL) {
                // Fragment will be handled correctly by the super class.
            } else if (KEY_CURRENT_INPUT_METHOD->Equals(preference->GetKey())) {
                final InputMethodManager imm = (InputMethodManager)
                        GetSystemService(Context.INPUT_METHOD_SERVICE);
                imm->ShowInputMethodPicker();
            }
        } else if (preference instanceof CheckBoxPreference) {
            final CheckBoxPreference chkPref = (CheckBoxPreference) preference;
            if (chkPref == mGameControllerCategory->FindPreference("vibrate_input_devices")) {
                System->PutInt(GetContentResolver(), Settings.System.VIBRATE_INPUT_DEVICES,
                        chkPref->IsChecked() ? 1 : 0);
                return TRUE;
            }
        }
        return super->OnPreferenceTreeClick(preferenceScreen, preference);
    }

    private static String GetLocaleName(Context context) {
        // We want to show the same string that the LocalePicker used.
        // TODO: should this method be in LocalePicker instead?
        Locale currentLocale = context->GetResources()->GetConfiguration().locale;
        List<LocalePicker.LocaleInfo> locales = LocalePicker->GetAllAssetLocales(context, TRUE);
        for (LocalePicker.LocaleInfo locale : locales) {
            if (locale->GetLocale()->Equals(currentLocale)) {
                return locale->GetLabel();
            }
        }
        // This can't happen as Int64 as the locale was one set by Settings.
        // Fall back in case a developer is testing an unsupported locale.
        return currentLocale->GetDisplayName(currentLocale);
    }

    private void SaveInputMethodSelectorVisibility(String value) {
        try {
            Int32 intValue = Integer->ValueOf(value);
            Settings.Secure->PutInt(GetContentResolver(),
                    Settings.Secure.INPUT_METHOD_SELECTOR_VISIBILITY, intValue);
            UpdateInputMethodSelectorSummary(intValue);
        } Catch(NumberFormatException e) {
        }
    }

    private Int32 LoadInputMethodSelectorVisibility() {
        return Settings.Secure->GetInt(GetContentResolver(),
                Settings.Secure.INPUT_METHOD_SELECTOR_VISIBILITY,
                mDefaultInputMethodSelectorVisibility);
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object value) {
        if (SHOW_INPUT_METHOD_SWITCHER_SETTINGS) {
            if (preference == mShowInputMethodSelectorPref) {
                if (value instanceof String) {
                    SaveInputMethodSelectorVisibility((String)value);
                }
            }
        }
        return FALSE;
    }

    private void UpdateInputMethodPreferenceViews() {
        synchronized(mInputMethodPreferenceList) {
            // Clear existing "InputMethodPreference"s
            for (final InputMethodPreference pref : mInputMethodPreferenceList) {
                mKeyboardSettingsCategory->RemovePreference(pref);
            }
            mInputMethodPreferenceList->Clear();
            List<String> permittedList = mDpm->GetPermittedInputMethodsForCurrentUser();
            final Context context = GetActivity();
            final List<InputMethodInfo> imis = mShowsOnlyFullImeAndKeyboardList
                    ? mInputMethodSettingValues->GetInputMethodList()
                    : mImm->GetEnabledInputMethodList();
            final Int32 N = (imis == NULL ? 0 : imis->Size());
            for (Int32 i = 0; i < N; ++i) {
                final InputMethodInfo imi = imis->Get(i);
                final Boolean isAllowedByOrganization = permittedList == NULL
                        || permittedList->Contains(imi->GetPackageName());
                final InputMethodPreference pref = new InputMethodPreference(
                        context, imi, mShowsOnlyFullImeAndKeyboardList /* hasSwitch */,
                        isAllowedByOrganization, this);
                mInputMethodPreferenceList->Add(pref);
            }
            final Collator collator = Collator->GetInstance();
            Collections->Sort(mInputMethodPreferenceList, new Comparator<InputMethodPreference>() {
                //@Override
                public Int32 Compare(InputMethodPreference lhs, InputMethodPreference rhs) {
                    return lhs->CompareTo(rhs, collator);
                }
            });
            for (Int32 i = 0; i < N; ++i) {
                final InputMethodPreference pref = mInputMethodPreferenceList->Get(i);
                mKeyboardSettingsCategory->AddPreference(pref);
                InputMethodAndSubtypeUtil->RemoveUnnecessaryNonPersistentPreference(pref);
                pref->UpdatePreferenceViews();
            }
        }
        UpdateCurrentImeName();
        // TODO: Consolidate the logic with InputMethodSettingsWrapper
        // CAVEAT: The preference class here does not know about the default value - that is
        // managed by the Input Method Manager Service, so in this case it could save the wrong
        // value. Hence we must update the checkboxes here.
        InputMethodAndSubtypeUtil->LoadInputMethodSubtypeList(
                this, GetContentResolver(),
                mInputMethodSettingValues->GetInputMethodList(), NULL);
    }

    //@Override
    CARAPI OnSaveInputMethodPreference(final InputMethodPreference pref) {
        final InputMethodInfo imi = pref->GetInputMethodInfo();
        if (!pref->IsChecked()) {
            // An IME is being disabled. Save enabled subtypes of the IME to shared preference to be
            // able to re-enable these subtypes when the IME gets re-enabled.
            SaveEnabledSubtypesOf(imi);
        }
        final Boolean hasHardwareKeyboard = GetResources()->GetConfiguration().keyboard
                == Configuration.KEYBOARD_QWERTY;
        InputMethodAndSubtypeUtil->SaveInputMethodSubtypeList(this, GetContentResolver(),
                mImm->GetInputMethodList(), hasHardwareKeyboard);
        // Update input method settings and preference list.
        mInputMethodSettingValues->RefreshAllInputMethodAndSubtypes();
        if (pref->IsChecked()) {
            // An IME is being enabled. Load the previously enabled subtypes from shared preference
            // and enable these subtypes.
            RestorePreviouslyEnabledSubtypesOf(imi);
        }
        for (final InputMethodPreference p : mInputMethodPreferenceList) {
            p->UpdatePreferenceViews();
        }
    }

    private void SaveEnabledSubtypesOf(final InputMethodInfo imi) {
        final HashSet<String> enabledSubtypeIdSet = new HashSet<>();
        final List<InputMethodSubtype> enabledSubtypes = mImm->GetEnabledInputMethodSubtypeList(
                imi, TRUE /* allowsImplicitlySelectedSubtypes */);
        for (final InputMethodSubtype subtype : enabledSubtypes) {
            final String subtypeId = Integer->ToString(subtype->HashCode());
            enabledSubtypeIdSet->Add(subtypeId);
        }
        final HashMap<String, HashSet<String>> imeToEnabledSubtypeIdsMap =
                LoadPreviouslyEnabledSubtypeIdsMap();
        final String imiId = imi->GetId();
        imeToEnabledSubtypeIdsMap->Put(imiId, enabledSubtypeIdSet);
        SavePreviouslyEnabledSubtypeIdsMap(imeToEnabledSubtypeIdsMap);
    }

    private void RestorePreviouslyEnabledSubtypesOf(final InputMethodInfo imi) {
        final HashMap<String, HashSet<String>> imeToEnabledSubtypeIdsMap =
                LoadPreviouslyEnabledSubtypeIdsMap();
        final String imiId = imi->GetId();
        final HashSet<String> enabledSubtypeIdSet = imeToEnabledSubtypeIdsMap->Remove(imiId);
        if (enabledSubtypeIdSet == NULL) {
            return;
        }
        SavePreviouslyEnabledSubtypeIdsMap(imeToEnabledSubtypeIdsMap);
        InputMethodAndSubtypeUtil->EnableInputMethodSubtypesOf(
                GetContentResolver(), imiId, enabledSubtypeIdSet);
    }

    private HashMap<String, HashSet<String>> LoadPreviouslyEnabledSubtypeIdsMap() {
        final Context context = GetActivity();
        final SharedPreferences prefs = PreferenceManager->GetDefaultSharedPreferences(context);
        final String imesAndSubtypesString = prefs->GetString(KEY_PREVIOUSLY_ENABLED_SUBTYPES, NULL);
        return InputMethodAndSubtypeUtil->ParseInputMethodsAndSubtypesString(imesAndSubtypesString);
    }

    private void SavePreviouslyEnabledSubtypeIdsMap(
            final HashMap<String, HashSet<String>> subtypesMap) {
        final Context context = GetActivity();
        final SharedPreferences prefs = PreferenceManager->GetDefaultSharedPreferences(context);
        final String imesAndSubtypesString = InputMethodAndSubtypeUtil
                .BuildInputMethodsAndSubtypesString(subtypesMap);
        prefs->Edit()->PutString(KEY_PREVIOUSLY_ENABLED_SUBTYPES, imesAndSubtypesString).Apply();
    }

    private void UpdateCurrentImeName() {
        final Context context = GetActivity();
        if (context == NULL || mImm == NULL) return;
        final Preference curPref = GetPreferenceScreen()->FindPreference(KEY_CURRENT_INPUT_METHOD);
        if (curPref != NULL) {
            final CharSequence curIme =
                    mInputMethodSettingValues->GetCurrentInputMethodName(context);
            if (!TextUtils->IsEmpty(curIme)) {
                synchronized(this) {
                    curPref->SetSummary(curIme);
                }
            }
        }
    }

    private void UpdateInputDevices() {
        UpdateHardKeyboards();
        UpdateGameControllers();
    }

    private void UpdateHardKeyboards() {
        mHardKeyboardPreferenceList->Clear();
        final Int32[] devices = InputDevice->GetDeviceIds();
        for (Int32 i = 0; i < devices.length; i++) {
            InputDevice device = InputDevice->GetDevice(devices[i]);
            if (device != NULL
                    && !device->IsVirtual()
                    && device->IsFullKeyboard()) {
                final InputDeviceIdentifier identifier = device->GetIdentifier();
                final String keyboardLayoutDescriptor =
                    mIm->GetCurrentKeyboardLayoutForInputDevice(identifier);
                final KeyboardLayout keyboardLayout = keyboardLayoutDescriptor != NULL ?
                    mIm->GetKeyboardLayout(keyboardLayoutDescriptor) : NULL;

                final PreferenceScreen pref = new PreferenceScreen(GetActivity(), NULL);
                pref->SetTitle(device->GetName());
                if (keyboardLayout != NULL) {
                    pref->SetSummary(keyboardLayout->ToString());
                } else {
                    pref->SetSummary(R::string::keyboard_layout_default_label);
                }
                pref->SetOnPreferenceClickListener(new Preference->OnPreferenceClickListener() {
                    //@Override
                    public Boolean OnPreferenceClick(Preference preference) {
                        ShowKeyboardLayoutDialog(identifier);
                        return TRUE;
                    }
                });
                mHardKeyboardPreferenceList->Add(pref);
            }
        }

        if (!mHardKeyboardPreferenceList->IsEmpty()) {
            for (Int32 i = mHardKeyboardCategory->GetPreferenceCount(); i-- > 0; ) {
                final Preference pref = mHardKeyboardCategory->GetPreference(i);
                if (pref->GetOrder() < 1000) {
                    mHardKeyboardCategory->RemovePreference(pref);
                }
            }

            Collections->Sort(mHardKeyboardPreferenceList);
            final Int32 count = mHardKeyboardPreferenceList->Size();
            for (Int32 i = 0; i < count; i++) {
                final Preference pref = mHardKeyboardPreferenceList->Get(i);
                pref->SetOrder(i);
                mHardKeyboardCategory->AddPreference(pref);
            }

            GetPreferenceScreen()->AddPreference(mHardKeyboardCategory);
        } else {
            GetPreferenceScreen()->RemovePreference(mHardKeyboardCategory);
        }
    }

    private void ShowKeyboardLayoutDialog(InputDeviceIdentifier inputDeviceIdentifier) {
        KeyboardLayoutDialogFragment fragment = new KeyboardLayoutDialogFragment(
                inputDeviceIdentifier);
        fragment->SetTargetFragment(this, 0);
        fragment->Show(GetActivity()->GetFragmentManager(), "keyboardLayout");
    }

    //@Override
    CARAPI OnSetupKeyboardLayouts(InputDeviceIdentifier inputDeviceIdentifier) {
        final Intent intent = new Intent(IIntent::ACTION_MAIN);
        intent->SetClass(GetActivity(), KeyboardLayoutPickerActivity.class);
        intent->PutExtra(KeyboardLayoutPickerFragment.EXTRA_INPUT_DEVICE_IDENTIFIER,
                inputDeviceIdentifier);
        mIntentWaitingForResult = intent;
        StartActivityForResult(intent, 0);
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        super->OnActivityResult(requestCode, resultCode, data);

        if (mIntentWaitingForResult != NULL) {
            InputDeviceIdentifier inputDeviceIdentifier = mIntentWaitingForResult
                    .GetParcelableExtra(KeyboardLayoutPickerFragment.EXTRA_INPUT_DEVICE_IDENTIFIER);
            mIntentWaitingForResult = NULL;
            ShowKeyboardLayoutDialog(inputDeviceIdentifier);
        }
    }

    private void UpdateGameControllers() {
        if (HaveInputDeviceWithVibrator()) {
            GetPreferenceScreen()->AddPreference(mGameControllerCategory);

            CheckBoxPreference chkPref = (CheckBoxPreference)
                    mGameControllerCategory->FindPreference("vibrate_input_devices");
            chkPref->SetChecked(System->GetInt(GetContentResolver(),
                    Settings.System.VIBRATE_INPUT_DEVICES, 1) > 0);
        } else {
            GetPreferenceScreen()->RemovePreference(mGameControllerCategory);
        }
    }

    private static Boolean HaveInputDeviceWithVibrator() {
        final Int32[] devices = InputDevice->GetDeviceIds();
        for (Int32 i = 0; i < devices.length; i++) {
            InputDevice device = InputDevice->GetDevice(devices[i]);
            if (device != NULL && !device->IsVirtual() && device->GetVibrator()->HasVibrator()) {
                return TRUE;
            }
        }
        return FALSE;
    }

    private class SettingsObserver extends ContentObserver {
        private Context mContext;

        public SettingsObserver(Handler handler, Context context) {
            Super(handler);
            mContext = context;
        }

        //@Override CARAPI OnChange(Boolean selfChange) {
            UpdateCurrentImeName();
        }

        CARAPI Resume() {
            final ContentResolver cr = mContext->GetContentResolver();
            cr->RegisterContentObserver(
                    Settings.Secure->GetUriFor(Settings.Secure.DEFAULT_INPUT_METHOD), FALSE, this);
            cr->RegisterContentObserver(Settings.Secure->GetUriFor(
                    Settings.Secure.SELECTED_INPUT_METHOD_SUBTYPE), FALSE, this);
        }

        CARAPI Pause() {
            mContext->GetContentResolver()->UnregisterContentObserver(this);
        }
    }

    public static const Indexable.SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
            new BaseSearchIndexProvider() {
        //@Override
        public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
            List<SearchIndexableRaw> indexables = new ArrayList<>();

            final String screenTitle = context->GetString(R::string::language_keyboard_settings_title);

            // Locale picker.
            if (context->GetAssets()->GetLocales().length > 1) {
                String localeName = GetLocaleName(context);
                SearchIndexableRaw indexable = new SearchIndexableRaw(context);
                indexable.key = KEY_PHONE_LANGUAGE;
                indexable.title = context->GetString(R::string::phone_language);
                indexable.summaryOn = localeName;
                indexable.summaryOff = localeName;
                indexable.screenTitle = screenTitle;
                indexables->Add(indexable);
            }

            // Spell checker.
            SearchIndexableRaw indexable = new SearchIndexableRaw(context);
            indexable.key = KEY_SPELL_CHECKERS;
            indexable.title = context->GetString(R::string::spellcheckers_settings_title);
            indexable.screenTitle = screenTitle;
            indexables->Add(indexable);

            // User dictionary.
            if (UserDictionaryList->GetUserDictionaryLocalesSet(context) != NULL) {
                indexable = new SearchIndexableRaw(context);
                indexable.key = "user_dict_settings";
                indexable.title = context->GetString(R::string::user_dict_settings_title);
                indexable.screenTitle = screenTitle;
                indexables->Add(indexable);
            }

            // Keyboard settings.
            indexable = new SearchIndexableRaw(context);
            indexable.key = "keyboard_settings";
            indexable.title = context->GetString(R::string::keyboard_settings_category);
            indexable.screenTitle = screenTitle;
            indexables->Add(indexable);

            InputMethodSettingValuesWrapper immValues = InputMethodSettingValuesWrapper
                    .GetInstance(context);
            immValues->RefreshAllInputMethodAndSubtypes();

            // Current IME.
            String currImeName = immValues->GetCurrentInputMethodName(context).ToString();
            indexable = new SearchIndexableRaw(context);
            indexable.key = KEY_CURRENT_INPUT_METHOD;
            indexable.title = context->GetString(R::string::current_input_method);
            indexable.summaryOn = currImeName;
            indexable.summaryOff = currImeName;
            indexable.screenTitle = screenTitle;
            indexables->Add(indexable);

            InputMethodManager inputMethodManager = (InputMethodManager) context->GetSystemService(
                    Context.INPUT_METHOD_SERVICE);

            // All other IMEs.
            List<InputMethodInfo> inputMethods = immValues->GetInputMethodList();
            final Int32 inputMethodCount = (inputMethods == NULL ? 0 : inputMethods->Size());
            for (Int32 i = 0; i < inputMethodCount; ++i) {
                InputMethodInfo inputMethod = inputMethods->Get(i);

                StringBuilder builder = new StringBuilder();
                List<InputMethodSubtype> subtypes = inputMethodManager
                        .GetEnabledInputMethodSubtypeList(inputMethod, TRUE);
                final Int32 subtypeCount = subtypes->Size();
                for (Int32 j = 0; j < subtypeCount; j++) {
                    InputMethodSubtype subtype = subtypes->Get(j);
                    if (builder->Length() > 0) {
                        builder->Append(',');
                    }
                    CharSequence subtypeLabel = subtype->GetDisplayName(context,
                            inputMethod->GetPackageName(), inputMethod->GetServiceInfo()
                                    .applicationInfo);
                    builder->Append(subtypeLabel);
                }
                String summary = builder->ToString();

                ServiceInfo serviceInfo = inputMethod->GetServiceInfo();
                ComponentName componentName = new ComponentName(serviceInfo.packageName,
                        serviceInfo.name);

                indexable = new SearchIndexableRaw(context);
                indexable.key = componentName->FlattenToString();
                indexable.title = inputMethod->LoadLabel(context->GetPackageManager()).ToString();
                indexable.summaryOn = summary;
                indexable.summaryOff = summary;
                indexable.screenTitle = screenTitle;
                indexables->Add(indexable);
            }

            // Hard keyboards
            InputManager inputManager = (InputManager) context->GetSystemService(
                    Context.INPUT_SERVICE);
            Boolean hasHardKeyboards = FALSE;

            final Int32[] devices = InputDevice->GetDeviceIds();
            for (Int32 i = 0; i < devices.length; i++) {
                InputDevice device = InputDevice->GetDevice(devices[i]);
                if (device == NULL || device->IsVirtual() || !device->IsFullKeyboard()) {
                    continue;
                }

                hasHardKeyboards = TRUE;

                InputDeviceIdentifier identifier = device->GetIdentifier();
                String keyboardLayoutDescriptor =
                        inputManager->GetCurrentKeyboardLayoutForInputDevice(identifier);
                KeyboardLayout keyboardLayout = keyboardLayoutDescriptor != NULL ?
                        inputManager->GetKeyboardLayout(keyboardLayoutDescriptor) : NULL;

                String summary;
                if (keyboardLayout != NULL) {
                    summary = keyboardLayout->ToString();
                } else {
                    summary = context->GetString(R::string::keyboard_layout_default_label);
                }

                indexable = new SearchIndexableRaw(context);
                indexable.key = device->GetName();
                indexable.title = device->GetName();
                indexable.summaryOn = summary;
                indexable.summaryOff = summary;
                indexable.screenTitle = screenTitle;
                indexables->Add(indexable);
            }

            if (hasHardKeyboards) {
                // Hard keyboard category.
                indexable = new SearchIndexableRaw(context);
                indexable.key = "builtin_keyboard_settings";
                indexable.title = context->GetString(
                        R::string::builtin_keyboard_settings_title);
                indexable.screenTitle = screenTitle;
                indexables->Add(indexable);
            }

            // Voice input
            indexable = new SearchIndexableRaw(context);
            indexable.key = "voice_input_settings";
            indexable.title = context->GetString(R::string::voice_input_settings);
            indexable.screenTitle = screenTitle;
            indexables->Add(indexable);

            // Text-to-speech.
            TtsEngines ttsEngines = new TtsEngines(context);
            if (!ttsEngines->GetEngines()->IsEmpty()) {
                indexable = new SearchIndexableRaw(context);
                indexable.key = "tts_settings";
                indexable.title = context->GetString(R::string::tts_settings_title);
                indexable.screenTitle = screenTitle;
                indexables->Add(indexable);
            }

            // Pointer settings.
            indexable = new SearchIndexableRaw(context);
            indexable.key = "pointer_settings_category";
            indexable.title = context->GetString(R::string::pointer_settings_category);
            indexable.screenTitle = screenTitle;
            indexables->Add(indexable);

            indexable = new SearchIndexableRaw(context);
            indexable.key = "pointer_speed";
            indexable.title = context->GetString(R::string::pointer_speed);
            indexable.screenTitle = screenTitle;
            indexables->Add(indexable);

            // Game controllers.
            if (HaveInputDeviceWithVibrator()) {
                indexable = new SearchIndexableRaw(context);
                indexable.key = "vibrate_input_devices";
                indexable.title = context->GetString(R::string::vibrate_input_devices);
                indexable.summaryOn = context->GetString(R::string::vibrate_input_devices_summary);
                indexable.summaryOff = context->GetString(R::string::vibrate_input_devices_summary);
                indexable.screenTitle = screenTitle;
                indexables->Add(indexable);
            }

            return indexables;
        }
    };
}
