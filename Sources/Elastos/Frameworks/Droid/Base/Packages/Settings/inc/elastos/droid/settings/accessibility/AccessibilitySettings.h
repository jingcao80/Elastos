/*
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.settings.accessibility;

using Elastos::Droid::Accessibilityservice::IAccessibilityServiceInfo;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Pm::IServiceInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::TextUtils::ISimpleStringSplitter;
using Elastos::Droid::View::IKeyCharacterMap;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Content::IPackageMonitor;
using Elastos::Droid::Internal::View::IRotationPolicy;
using Elastos::Droid::Internal::View::RotationPolicy::IRotationPolicyListener;
using Elastos::Droid::Settings::IDialogCreatable;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;
using Elastos::Droid::Settings::IUtils;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;

/**
 * Activity with the accessibility settings.
 */
public class AccessibilitySettings extends SettingsPreferenceFragment implements DialogCreatable,
        Preference.OnPreferenceChangeListener, Indexable {

    private static const Float LARGE_FONT_SCALE = 1.3f;

    static const Char32 ENABLED_ACCESSIBILITY_SERVICES_SEPARATOR = ':';

    // Preference categories
    private static const String SERVICES_CATEGORY = "services_category";
    private static const String SYSTEM_CATEGORY = "system_category";

    // Preferences
    private static const String TOGGLE_LARGE_TEXT_PREFERENCE =
            "toggle_large_text_preference";
    private static const String TOGGLE_HIGH_TEXT_CONTRAST_PREFERENCE =
            "toggle_high_text_contrast_preference";
    private static const String TOGGLE_INVERSION_PREFERENCE =
            "toggle_inversion_preference";
    private static const String TOGGLE_POWER_BUTTON_ENDS_CALL_PREFERENCE =
            "toggle_power_button_ends_call_preference";
    private static const String TOGGLE_LOCK_SCREEN_ROTATION_PREFERENCE =
            "toggle_lock_screen_rotation_preference";
    private static const String TOGGLE_SPEAK_PASSWORD_PREFERENCE =
            "toggle_speak_password_preference";
    private static const String SELECT_LONG_PRESS_TIMEOUT_PREFERENCE =
            "select_long_press_timeout_preference";
    private static const String ENABLE_ACCESSIBILITY_GESTURE_PREFERENCE_SCREEN =
            "enable_global_gesture_preference_screen";
    private static const String CAPTIONING_PREFERENCE_SCREEN =
            "captioning_preference_screen";
    private static const String DISPLAY_MAGNIFICATION_PREFERENCE_SCREEN =
            "screen_magnification_preference_screen";
    private static const String DISPLAY_DALTONIZER_PREFERENCE_SCREEN =
            "daltonizer_preference_screen";

    // Extras passed to sub-fragments.
    static const String EXTRA_PREFERENCE_KEY = "preference_key";
    static const String EXTRA_CHECKED = "checked";
    static const String EXTRA_TITLE = "title";
    static const String EXTRA_SUMMARY = "summary";
    static const String EXTRA_SETTINGS_TITLE = "settings_title";
    static const String EXTRA_COMPONENT_NAME = "component_name";
    static const String EXTRA_SETTINGS_COMPONENT_NAME = "settings_component_name";

    // Timeout before we update the services if packages are added/removed
    // since the AccessibilityManagerService has to do that processing first
    // to generate the AccessibilityServiceInfo we need for proper
    // presentation.
    private static const Int64 DELAY_UPDATE_SERVICES_MILLIS = 1000;

    // Auxiliary members.
    static const SimpleStringSplitter sStringColonSplitter =
            new SimpleStringSplitter(ENABLED_ACCESSIBILITY_SERVICES_SEPARATOR);

    static const Set<ComponentName> sInstalledServices = new HashSet<ComponentName>();

    private final Map<String, String> mLongPressTimeoutValuetoTitleMap =
            new HashMap<String, String>();

    private final Configuration mCurConfig = new Configuration();

    private final Handler mHandler = new Handler();

    private final Runnable mUpdateRunnable = new Runnable() {
        //@Override
        CARAPI Run() {
            LoadInstalledServices();
            UpdateServicesPreferences();
        }
    };

    private final PackageMonitor mSettingsPackageMonitor = new PackageMonitor() {
        //@Override
        CARAPI OnPackageAdded(String packageName, Int32 uid) {
            SendUpdate();
        }

        //@Override
        CARAPI OnPackageAppeared(String packageName, Int32 reason) {
            SendUpdate();
        }

        //@Override
        CARAPI OnPackageDisappeared(String packageName, Int32 reason) {
            SendUpdate();
        }

        //@Override
        CARAPI OnPackageRemoved(String packageName, Int32 uid) {
            SendUpdate();
        }

        private void SendUpdate() {
            mHandler->PostDelayed(mUpdateRunnable, DELAY_UPDATE_SERVICES_MILLIS);
        }
    };

    private final SettingsContentObserver mSettingsContentObserver =
            new SettingsContentObserver(mHandler) {
                //@Override
                CARAPI OnChange(Boolean selfChange, Uri uri) {
                    LoadInstalledServices();
                    UpdateServicesPreferences();
                }
            };

    private final RotationPolicyListener mRotationPolicyListener = new RotationPolicyListener() {
        //@Override
        CARAPI OnChange() {
            UpdateLockScreenRotationCheckbox();
        }
    };

    // Preference controls.
    private PreferenceCategory mServicesCategory;
    private PreferenceCategory mSystemsCategory;

    private CheckBoxPreference mToggleLargeTextPreference;
    private CheckBoxPreference mToggleHighTextContrastPreference;
    private CheckBoxPreference mTogglePowerButtonEndsCallPreference;
    private CheckBoxPreference mToggleLockScreenRotationPreference;
    private CheckBoxPreference mToggleSpeakPasswordPreference;
    private ListPreference mSelectLongPressTimeoutPreference;
    private Preference mNoServicesMessagePreference;
    private PreferenceScreen mCaptioningPreferenceScreen;
    private PreferenceScreen mDisplayMagnificationPreferenceScreen;
    private PreferenceScreen mGlobalGesturePreferenceScreen;
    private PreferenceScreen mDisplayDaltonizerPreferenceScreen;
    private SwitchPreference mToggleInversionPreference;

    private Int32 mLongPressTimeoutDefault;

    private DevicePolicyManager mDpm;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);
        AddPreferencesFromResource(R.xml.accessibility_settings);
        InitializeAllPreferences();
        mDpm = (DevicePolicyManager) (GetActivity()
                .GetSystemService(Context.DEVICE_POLICY_SERVICE));
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        LoadInstalledServices();
        UpdateAllPreferences();

        mSettingsPackageMonitor->Register(GetActivity(), GetActivity()->GetMainLooper(), FALSE);
        mSettingsContentObserver->Register(GetContentResolver());
        if (RotationPolicy->IsRotationSupported(GetActivity())) {
            RotationPolicy->RegisterRotationPolicyListener(GetActivity(),
                    mRotationPolicyListener);
        }
    }

    //@Override
    CARAPI OnPause() {
        mSettingsPackageMonitor->Unregister();
        mSettingsContentObserver->Unregister(GetContentResolver());
        if (RotationPolicy->IsRotationSupported(GetActivity())) {
            RotationPolicy->UnregisterRotationPolicyListener(GetActivity(),
                    mRotationPolicyListener);
        }
        super->OnPause();
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        if (mSelectLongPressTimeoutPreference == preference) {
            HandleLongPressTimeoutPreferenceChange((String) newValue);
            return TRUE;
        } else if (mToggleInversionPreference == preference) {
            HandleToggleInversionPreferenceChange((Boolean) newValue);
            return TRUE;
        }
        return FALSE;
    }

    private void HandleLongPressTimeoutPreferenceChange(String stringValue) {
        Settings.Secure->PutInt(GetContentResolver(),
                Settings.Secure.LONG_PRESS_TIMEOUT, Integer->ParseInt(stringValue));
        mSelectLongPressTimeoutPreference->SetSummary(
                mLongPressTimeoutValuetoTitleMap->Get(stringValue));
    }

    private void HandleToggleInversionPreferenceChange(Boolean checked) {
        Settings.Secure->PutInt(GetContentResolver(),
                Settings.Secure.ACCESSIBILITY_DISPLAY_INVERSION_ENABLED, (checked ? 1 : 0));
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (mToggleLargeTextPreference == preference) {
            HandleToggleLargeTextPreferenceClick();
            return TRUE;
        } else if (mToggleHighTextContrastPreference == preference) {
            HandleToggleTextContrastPreferenceClick();
            return TRUE;
        } else if (mTogglePowerButtonEndsCallPreference == preference) {
            HandleTogglePowerButtonEndsCallPreferenceClick();
            return TRUE;
        } else if (mToggleLockScreenRotationPreference == preference) {
            HandleLockScreenRotationPreferenceClick();
            return TRUE;
        } else if (mToggleSpeakPasswordPreference == preference) {
            HandleToggleSpeakPasswordPreferenceClick();
            return TRUE;
        } else if (mGlobalGesturePreferenceScreen == preference) {
            HandleToggleEnableAccessibilityGesturePreferenceClick();
            return TRUE;
        } else if (mDisplayMagnificationPreferenceScreen == preference) {
            HandleDisplayMagnificationPreferenceScreenClick();
            return TRUE;
        }
        return super->OnPreferenceTreeClick(preferenceScreen, preference);
    }

    private void HandleToggleLargeTextPreferenceClick() {
        try {
            mCurConfig.fontScale = mToggleLargeTextPreference->IsChecked() ? LARGE_FONT_SCALE : 1;
            ActivityManagerNative->GetDefault()->UpdatePersistentConfiguration(mCurConfig);
        } catch (RemoteException re) {
            /* ignore */
        }
    }

    private void HandleToggleTextContrastPreferenceClick() {
        Settings.Secure->PutInt(GetContentResolver(),
                Settings.Secure.ACCESSIBILITY_HIGH_TEXT_CONTRAST_ENABLED,
                (mToggleHighTextContrastPreference->IsChecked() ? 1 : 0));
    }

    private void HandleTogglePowerButtonEndsCallPreferenceClick() {
        Settings.Secure->PutInt(GetContentResolver(),
                Settings.Secure.INCALL_POWER_BUTTON_BEHAVIOR,
                (mTogglePowerButtonEndsCallPreference->IsChecked()
                        ? Settings.Secure.INCALL_POWER_BUTTON_BEHAVIOR_HANGUP
                        : Settings.Secure.INCALL_POWER_BUTTON_BEHAVIOR_SCREEN_OFF));
    }

    private void HandleLockScreenRotationPreferenceClick() {
        RotationPolicy->SetRotationLockForAccessibility(GetActivity(),
                !mToggleLockScreenRotationPreference->IsChecked());
    }

    private void HandleToggleSpeakPasswordPreferenceClick() {
        Settings.Secure->PutInt(GetContentResolver(),
                Settings.Secure.ACCESSIBILITY_SPEAK_PASSWORD,
                mToggleSpeakPasswordPreference->IsChecked() ? 1 : 0);
    }

    private void HandleToggleEnableAccessibilityGesturePreferenceClick() {
        Bundle extras = mGlobalGesturePreferenceScreen->GetExtras();
        extras->PutString(EXTRA_TITLE, GetString(
                R::string::accessibility_global_gesture_preference_title));
        extras->PutString(EXTRA_SUMMARY, GetString(
                R::string::accessibility_global_gesture_preference_description));
        extras->PutBoolean(EXTRA_CHECKED, Settings::Global::>GetInt(GetContentResolver(),
                Settings::Global::ENABLE_ACCESSIBILITY_GLOBAL_GESTURE_ENABLED, 0) == 1);
        super->OnPreferenceTreeClick(mGlobalGesturePreferenceScreen,
                mGlobalGesturePreferenceScreen);
    }

    private void HandleDisplayMagnificationPreferenceScreenClick() {
        Bundle extras = mDisplayMagnificationPreferenceScreen->GetExtras();
        extras->PutString(EXTRA_TITLE, GetString(
                R::string::accessibility_screen_magnification_title));
        extras->PutCharSequence(EXTRA_SUMMARY, GetActivity()->GetResources().GetText(
                R::string::accessibility_screen_magnification_summary));
        extras->PutBoolean(EXTRA_CHECKED, Settings.Secure->GetInt(GetContentResolver(),
                Settings.Secure.ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED, 0) == 1);
        super->OnPreferenceTreeClick(mDisplayMagnificationPreferenceScreen,
                mDisplayMagnificationPreferenceScreen);
    }

    private void InitializeAllPreferences() {
        mServicesCategory = (PreferenceCategory) FindPreference(SERVICES_CATEGORY);
        mSystemsCategory = (PreferenceCategory) FindPreference(SYSTEM_CATEGORY);

        // Large text.
        mToggleLargeTextPreference =
                (CheckBoxPreference) FindPreference(TOGGLE_LARGE_TEXT_PREFERENCE);

        // Text contrast.
        mToggleHighTextContrastPreference =
                (CheckBoxPreference) FindPreference(TOGGLE_HIGH_TEXT_CONTRAST_PREFERENCE);

        // Display inversion.
        mToggleInversionPreference = (SwitchPreference) FindPreference(TOGGLE_INVERSION_PREFERENCE);
        mToggleInversionPreference->SetOnPreferenceChangeListener(this);

        // Power button ends calls.
        mTogglePowerButtonEndsCallPreference =
                (CheckBoxPreference) FindPreference(TOGGLE_POWER_BUTTON_ENDS_CALL_PREFERENCE);
        if (!KeyCharacterMap->DeviceHasKey(KeyEvent.KEYCODE_POWER)
                || !Utils->IsVoiceCapable(GetActivity())) {
            mSystemsCategory->RemovePreference(mTogglePowerButtonEndsCallPreference);
        }

        // Lock screen rotation.
        mToggleLockScreenRotationPreference =
                (CheckBoxPreference) FindPreference(TOGGLE_LOCK_SCREEN_ROTATION_PREFERENCE);
        if (!RotationPolicy->IsRotationSupported(GetActivity())) {
            mSystemsCategory->RemovePreference(mToggleLockScreenRotationPreference);
        }

        // Speak passwords.
        mToggleSpeakPasswordPreference =
                (CheckBoxPreference) FindPreference(TOGGLE_SPEAK_PASSWORD_PREFERENCE);

        // Long press timeout.
        mSelectLongPressTimeoutPreference =
                (ListPreference) FindPreference(SELECT_LONG_PRESS_TIMEOUT_PREFERENCE);
        mSelectLongPressTimeoutPreference->SetOnPreferenceChangeListener(this);
        if (mLongPressTimeoutValuetoTitleMap->Size() == 0) {
            String[] timeoutValues = GetResources()->GetStringArray(
                    R.array.long_press_timeout_selector_values);
            mLongPressTimeoutDefault = Integer->ParseInt(timeoutValues[0]);
            String[] timeoutTitles = GetResources()->GetStringArray(
                    R.array.long_press_timeout_selector_titles);
            final Int32 timeoutValueCount = timeoutValues.length;
            for (Int32 i = 0; i < timeoutValueCount; i++) {
                mLongPressTimeoutValuetoTitleMap->Put(timeoutValues[i], timeoutTitles[i]);
            }
        }

        // Captioning.
        mCaptioningPreferenceScreen = (PreferenceScreen) FindPreference(
                CAPTIONING_PREFERENCE_SCREEN);

        // Display magnification.
        mDisplayMagnificationPreferenceScreen = (PreferenceScreen) FindPreference(
                DISPLAY_MAGNIFICATION_PREFERENCE_SCREEN);

        // Display color adjustments.
        mDisplayDaltonizerPreferenceScreen = (PreferenceScreen) FindPreference(
                DISPLAY_DALTONIZER_PREFERENCE_SCREEN);

        // Global gesture.
        mGlobalGesturePreferenceScreen =
                (PreferenceScreen) FindPreference(ENABLE_ACCESSIBILITY_GESTURE_PREFERENCE_SCREEN);
        final Int32 longPressOnPowerBehavior = GetActivity()->GetResources().GetInteger(
                R.integer.config_longPressOnPowerBehavior);
        final Int32 LONG_PRESS_POWER_GLOBAL_ACTIONS = 1;
        if (!KeyCharacterMap->DeviceHasKey(KeyEvent.KEYCODE_POWER)
                || longPressOnPowerBehavior != LONG_PRESS_POWER_GLOBAL_ACTIONS) {
            // Remove accessibility shortcut if power key is not present
            // nor Int64 press power does not show global actions menu.
            mSystemsCategory->RemovePreference(mGlobalGesturePreferenceScreen);
        }
    }

    private void UpdateAllPreferences() {
        UpdateServicesPreferences();
        UpdateSystemPreferences();
    }

    private void UpdateServicesPreferences() {
        // Since services category is auto generated we have to do a pass
        // to generate it since services can come and go and then based on
        // the global accessibility state to decided whether it is enabled.

        // Generate.
        mServicesCategory->RemoveAll();

        AccessibilityManager accessibilityManager = AccessibilityManager->GetInstance(GetActivity());

        List<AccessibilityServiceInfo> installedServices =
                accessibilityManager->GetInstalledAccessibilityServiceList();
        Set<ComponentName> enabledServices = AccessibilityUtils->GetEnabledServicesFromSettings(
                GetActivity());
        List<String> permittedServices = mDpm->GetPermittedAccessibilityServices(
                UserHandle->MyUserId());
        final Boolean accessibilityEnabled = Settings.Secure->GetInt(GetContentResolver(),
                Settings.Secure.ACCESSIBILITY_ENABLED, 0) == 1;

        for (Int32 i = 0, count = installedServices->Size(); i < count; ++i) {
            AccessibilityServiceInfo info = installedServices->Get(i);

            PreferenceScreen preference = GetPreferenceManager()->CreatePreferenceScreen(
                    GetActivity());
            String title = info->GetResolveInfo()->LoadLabel(GetPackageManager()).ToString();

            ServiceInfo serviceInfo = info->GetResolveInfo().serviceInfo;
            ComponentName componentName = new ComponentName(serviceInfo.packageName,
                    serviceInfo.name);

            preference->SetKey(componentName->FlattenToString());

            preference->SetTitle(title);
            final Boolean serviceEnabled = accessibilityEnabled
                    && enabledServices->Contains(componentName);
            String serviceEnabledString;
            if (serviceEnabled) {
                serviceEnabledString = GetString(R::string::accessibility_feature_state_on);
            } else {
                serviceEnabledString = GetString(R::string::accessibility_feature_state_off);
            }

            // Disable all accessibility services that are not permitted.
            String packageName = serviceInfo.packageName;
            Boolean serviceAllowed =
                    permittedServices == NULL || permittedServices->Contains(packageName);
            preference->SetEnabled(serviceAllowed || serviceEnabled);

            String summaryString;
            if (serviceAllowed) {
                summaryString = serviceEnabledString;
            } else  {
                summaryString = GetString(R::string::accessibility_feature_or_input_method_not_allowed);
            }
            preference->SetSummary(summaryString);

            preference->SetOrder(i);
            preference->SetFragment(ToggleAccessibilityServicePreferenceFragment.class->GetName());
            preference->SetPersistent(TRUE);

            Bundle extras = preference->GetExtras();
            extras->PutString(EXTRA_PREFERENCE_KEY, preference->GetKey());
            extras->PutBoolean(EXTRA_CHECKED, serviceEnabled);
            extras->PutString(EXTRA_TITLE, title);

            String description = info->LoadDescription(GetPackageManager());
            if (TextUtils->IsEmpty(description)) {
                description = GetString(R::string::accessibility_service_default_description);
            }
            extras->PutString(EXTRA_SUMMARY, description);

            String settingsClassName = info->GetSettingsActivityName();
            if (!TextUtils->IsEmpty(settingsClassName)) {
                extras->PutString(EXTRA_SETTINGS_TITLE,
                        GetString(R::string::accessibility_menu_item_settings));
                extras->PutString(EXTRA_SETTINGS_COMPONENT_NAME,
                        new ComponentName(info->GetResolveInfo().serviceInfo.packageName,
                                settingsClassName).FlattenToString());
            }

            extras->PutParcelable(EXTRA_COMPONENT_NAME, componentName);

            mServicesCategory->AddPreference(preference);
        }

        if (mServicesCategory->GetPreferenceCount() == 0) {
            if (mNoServicesMessagePreference == NULL) {
                mNoServicesMessagePreference = new Preference(GetActivity());
                mNoServicesMessagePreference->SetPersistent(FALSE);
                mNoServicesMessagePreference->SetLayoutResource(
                        R.layout.text_description_preference);
                mNoServicesMessagePreference->SetSelectable(FALSE);
                mNoServicesMessagePreference->SetSummary(
                        GetString(R::string::accessibility_no_services_installed));
            }
            mServicesCategory->AddPreference(mNoServicesMessagePreference);
        }
    }

    private void UpdateSystemPreferences() {
        // Large text.
        try {
            mCurConfig->UpdateFrom(ActivityManagerNative->GetDefault()->GetConfiguration());
        } catch (RemoteException re) {
            /* ignore */
        }
        mToggleLargeTextPreference->SetChecked(mCurConfig.fontScale == LARGE_FONT_SCALE);

        mToggleHighTextContrastPreference->SetChecked(
                Settings.Secure->GetInt(GetContentResolver(),
                        Settings.Secure.ACCESSIBILITY_HIGH_TEXT_CONTRAST_ENABLED, 0) == 1);

        // If the quick setting is enabled, the preference MUST be enabled.
        mToggleInversionPreference->SetChecked(Settings.Secure->GetInt(GetContentResolver(),
                Settings.Secure.ACCESSIBILITY_DISPLAY_INVERSION_ENABLED, 0) == 1);

        // Power button ends calls.
        if (KeyCharacterMap->DeviceHasKey(KeyEvent.KEYCODE_POWER)
                && Utils->IsVoiceCapable(GetActivity())) {
            final Int32 incallPowerBehavior = Settings.Secure->GetInt(GetContentResolver(),
                    Settings.Secure.INCALL_POWER_BUTTON_BEHAVIOR,
                    Settings.Secure.INCALL_POWER_BUTTON_BEHAVIOR_DEFAULT);
            final Boolean powerButtonEndsCall =
                    (incallPowerBehavior == Settings.Secure.INCALL_POWER_BUTTON_BEHAVIOR_HANGUP);
            mTogglePowerButtonEndsCallPreference->SetChecked(powerButtonEndsCall);
        }

        // Auto-rotate screen
        UpdateLockScreenRotationCheckbox();

        // Speak passwords.
        final Boolean speakPasswordEnabled = Settings.Secure->GetInt(GetContentResolver(),
                Settings.Secure.ACCESSIBILITY_SPEAK_PASSWORD, 0) != 0;
        mToggleSpeakPasswordPreference->SetChecked(speakPasswordEnabled);

        // Long press timeout.
        final Int32 longPressTimeout = Settings.Secure->GetInt(GetContentResolver(),
                Settings.Secure.LONG_PRESS_TIMEOUT, mLongPressTimeoutDefault);
        String value = String->ValueOf(longPressTimeout);
        mSelectLongPressTimeoutPreference->SetValue(value);
        mSelectLongPressTimeoutPreference->SetSummary(mLongPressTimeoutValuetoTitleMap->Get(value));

        UpdateFeatureSummary(Settings.Secure.ACCESSIBILITY_CAPTIONING_ENABLED,
                mCaptioningPreferenceScreen);
        UpdateFeatureSummary(Settings.Secure.ACCESSIBILITY_DISPLAY_MAGNIFICATION_ENABLED,
                mDisplayMagnificationPreferenceScreen);
        UpdateFeatureSummary(Settings.Secure.ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED,
                mDisplayDaltonizerPreferenceScreen);

        // Global gesture
        final Boolean globalGestureEnabled = Settings::Global::>GetInt(GetContentResolver(),
                Settings::Global::ENABLE_ACCESSIBILITY_GLOBAL_GESTURE_ENABLED, 0) == 1;
        if (globalGestureEnabled) {
            mGlobalGesturePreferenceScreen->SetSummary(
                    R::string::accessibility_global_gesture_preference_summary_on);
        } else {
            mGlobalGesturePreferenceScreen->SetSummary(
                    R::string::accessibility_global_gesture_preference_summary_off);
        }
    }

    private void UpdateFeatureSummary(String prefKey, Preference pref) {
        final Boolean enabled = Settings.Secure->GetInt(GetContentResolver(), prefKey, 0) == 1;
        pref->SetSummary(enabled ? R::string::accessibility_feature_state_on
                : R::string::accessibility_feature_state_off);
    }

    private void UpdateLockScreenRotationCheckbox() {
        Context context = GetActivity();
        if (context != NULL) {
            mToggleLockScreenRotationPreference->SetChecked(
                    !RotationPolicy->IsRotationLocked(context));
        }
    }

    private void LoadInstalledServices() {
        Set<ComponentName> installedServices = sInstalledServices;
        installedServices->Clear();

        List<AccessibilityServiceInfo> installedServiceInfos =
                AccessibilityManager->GetInstance(GetActivity())
                        .GetInstalledAccessibilityServiceList();
        if (installedServiceInfos == NULL) {
            return;
        }

        final Int32 installedServiceInfoCount = installedServiceInfos->Size();
        for (Int32 i = 0; i < installedServiceInfoCount; i++) {
            ResolveInfo resolveInfo = installedServiceInfos->Get(i).GetResolveInfo();
            ComponentName installedService = new ComponentName(
                    resolveInfo.serviceInfo.packageName,
                    resolveInfo.serviceInfo.name);
            installedServices->Add(installedService);
        }
    }

    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
            new BaseSearchIndexProvider() {
        //@Override
        public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
            List<SearchIndexableRaw> indexables = new ArrayList<SearchIndexableRaw>();

            PackageManager packageManager = context->GetPackageManager();
            AccessibilityManager accessibilityManager = (AccessibilityManager)
                    context->GetSystemService(Context.ACCESSIBILITY_SERVICE);

            String screenTitle = context->GetResources()->GetString(
                    R::string::accessibility_services_title);

            // Indexing all services, regardless if enabled.
            List<AccessibilityServiceInfo> services = accessibilityManager
                    .GetInstalledAccessibilityServiceList();
            final Int32 serviceCount = services->Size();
            for (Int32 i = 0; i < serviceCount; i++) {
                AccessibilityServiceInfo service = services->Get(i);
                if (service == NULL || service->GetResolveInfo() == NULL) {
                    continue;
                }

                ServiceInfo serviceInfo = service->GetResolveInfo().serviceInfo;
                ComponentName componentName = new ComponentName(serviceInfo.packageName,
                        serviceInfo.name);

                SearchIndexableRaw indexable = new SearchIndexableRaw(context);
                indexable.key = componentName->FlattenToString();
                indexable.title = service->GetResolveInfo()->LoadLabel(packageManager).ToString();
                indexable.summaryOn = context->GetString(R::string::accessibility_feature_state_on);
                indexable.summaryOff = context->GetString(R::string::accessibility_feature_state_off);
                indexable.screenTitle = screenTitle;
                indexables->Add(indexable);
            }

            return indexables;
        }

        //@Override
        public List<SearchIndexableResource> GetXmlResourcesToIndex(Context context,
               Boolean enabled) {
            List<SearchIndexableResource> indexables = new ArrayList<SearchIndexableResource>();
            SearchIndexableResource indexable = new SearchIndexableResource(context);
            indexable.xmlResId = R.xml.accessibility_settings;
            indexables->Add(indexable);
            return indexables;
        }
    };
}
