/*
 * Copyright (C) 2010 The Android Open Source Project
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

package com.android.settings;

using Elastos::Droid::Internal::View::IRotationPolicy;
using Elastos::Droid::Settings::Notification::IDropDownPreference;
using Elastos::Droid::Settings::Notification::DropDownPreference::ICallback;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;

using static android::Provider::Settings::Secure::IDOZE_ENABLED;
using static android::Provider::Settings::Secure::IWAKE_GESTURE_ENABLED;
using static android::Provider::Settings::System::ISCREEN_BRIGHTNESS_MODE;
using static android::Provider::Settings::System::ISCREEN_BRIGHTNESS_MODE_AUTOMATIC;
using static android::Provider::Settings::System::ISCREEN_BRIGHTNESS_MODE_MANUAL;
using static android::Provider::Settings::System::ISCREEN_OFF_TIMEOUT;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Hardware::ISensor;
using Elastos::Droid::Hardware::ISensorManager;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceClickListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class DisplaySettings extends SettingsPreferenceFragment implements
        Preference.OnPreferenceChangeListener, OnPreferenceClickListener, Indexable {
    private static const String TAG = "DisplaySettings";

    /** If there is no setting in the provider, use this. */
    private static const Int32 FALLBACK_SCREEN_TIMEOUT_VALUE = 30000;

    private static const String KEY_SCREEN_TIMEOUT = "screen_timeout";
    private static const String KEY_FONT_SIZE = "font_size";
    private static const String KEY_SCREEN_SAVER = "screensaver";
    private static const String KEY_LIFT_TO_WAKE = "lift_to_wake";
    private static const String KEY_DOZE = "doze";
    private static const String KEY_AUTO_BRIGHTNESS = "auto_brightness";
    private static const String KEY_AUTO_ROTATE = "auto_rotate";

    private static const Int32 DLG_GLOBAL_CHANGE_WARNING = 1;

    private WarnedListPreference mFontSizePref;

    private final Configuration mCurConfig = new Configuration();

    private ListPreference mScreenTimeoutPreference;
    private Preference mScreenSaverPreference;
    private SwitchPreference mLiftToWakePreference;
    private SwitchPreference mDozePreference;
    private SwitchPreference mAutoBrightnessPreference;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        final Activity activity = GetActivity();
        final ContentResolver resolver = activity->GetContentResolver();

        AddPreferencesFromResource(R.xml.display_settings);

        mScreenSaverPreference = FindPreference(KEY_SCREEN_SAVER);
        if (mScreenSaverPreference != NULL
                && GetResources()->GetBoolean(
                        R.bool.config_dreamsSupported) == FALSE) {
            GetPreferenceScreen()->RemovePreference(mScreenSaverPreference);
        }

        mScreenTimeoutPreference = (ListPreference) FindPreference(KEY_SCREEN_TIMEOUT);
        final Int64 currentTimeout = Settings.System->GetLong(resolver, SCREEN_OFF_TIMEOUT,
                FALLBACK_SCREEN_TIMEOUT_VALUE);
        mScreenTimeoutPreference->SetValue(String->ValueOf(currentTimeout));
        mScreenTimeoutPreference->SetOnPreferenceChangeListener(this);
        DisableUnusableTimeouts(mScreenTimeoutPreference);
        UpdateTimeoutPreferenceDescription(currentTimeout);

        mFontSizePref = (WarnedListPreference) FindPreference(KEY_FONT_SIZE);
        mFontSizePref->SetOnPreferenceChangeListener(this);
        mFontSizePref->SetOnPreferenceClickListener(this);

        if (IsAutomaticBrightnessAvailable(GetResources())) {
            mAutoBrightnessPreference = (SwitchPreference) FindPreference(KEY_AUTO_BRIGHTNESS);
            mAutoBrightnessPreference->SetOnPreferenceChangeListener(this);
        } else {
            RemovePreference(KEY_AUTO_BRIGHTNESS);
        }

        if (IsLiftToWakeAvailable(activity)) {
            mLiftToWakePreference = (SwitchPreference) FindPreference(KEY_LIFT_TO_WAKE);
            mLiftToWakePreference->SetOnPreferenceChangeListener(this);
        } else {
            RemovePreference(KEY_LIFT_TO_WAKE);
        }

        if (IsDozeAvailable(activity)) {
            mDozePreference = (SwitchPreference) FindPreference(KEY_DOZE);
            mDozePreference->SetOnPreferenceChangeListener(this);
        } else {
            RemovePreference(KEY_DOZE);
        }

        if (RotationPolicy->IsRotationLockToggleVisible(activity)) {
            DropDownPreference rotatePreference =
                    (DropDownPreference) FindPreference(KEY_AUTO_ROTATE);
            rotatePreference->AddItem(activity->GetString(R::string::display_auto_rotate_rotate),
                    FALSE);
            Int32 rotateLockedResourceId;
            // The following block sets the string used when rotation is locked.
            // If the device locks specifically to portrait or landscape (rather than current
            // rotation), then we use a different string to include this information.
            if (AllowAllRotations(activity)) {
                rotateLockedResourceId = R::string::display_auto_rotate_stay_in_current;
            } else {
                if (RotationPolicy->GetRotationLockOrientation(activity)
                        == Configuration.ORIENTATION_PORTRAIT) {
                    rotateLockedResourceId =
                            R::string::display_auto_rotate_stay_in_portrait;
                } else {
                    rotateLockedResourceId =
                            R::string::display_auto_rotate_stay_in_landscape;
                }
            }
            rotatePreference->AddItem(activity->GetString(rotateLockedResourceId), TRUE);
            rotatePreference->SetSelectedItem(RotationPolicy->IsRotationLocked(activity) ?
                    1 : 0);
            rotatePreference->SetCallback(new Callback() {
                //@Override
                public Boolean OnItemSelected(Int32 pos, Object value) {
                    RotationPolicy->SetRotationLock(activity, (Boolean) value);
                    return TRUE;
                }
            });
        } else {
            RemovePreference(KEY_AUTO_ROTATE);
        }
    }

    private static Boolean AllowAllRotations(Context context) {
        return Resources->GetSystem()->GetBoolean(
                R.bool.config_allowAllRotations);
    }

    private static Boolean IsLiftToWakeAvailable(Context context) {
        SensorManager sensors = (SensorManager) context->GetSystemService(Context.SENSOR_SERVICE);
        return sensors != NULL && sensors->GetDefaultSensor(Sensor.TYPE_WAKE_GESTURE) != NULL;
    }

    private static Boolean IsDozeAvailable(Context context) {
        String name = Build.IS_DEBUGGABLE ? SystemProperties->Get("debug.doze.component") : NULL;
        if (TextUtils->IsEmpty(name)) {
            name = context->GetResources()->GetString(
                    R::string::config_dozeComponent);
        }
        return !TextUtils->IsEmpty(name);
    }

    private static Boolean IsAutomaticBrightnessAvailable(Resources res) {
        return res->GetBoolean(R.bool.config_automatic_brightness_available);
    }

    private void UpdateTimeoutPreferenceDescription(Int64 currentTimeout) {
        ListPreference preference = mScreenTimeoutPreference;
        String summary;
        if (currentTimeout < 0) {
            // Unsupported value
            summary = "";
        } else {
            final CharSequence[] entries = preference->GetEntries();
            final CharSequence[] values = preference->GetEntryValues();
            if (entries == NULL || entries.length == 0) {
                summary = "";
            } else {
                Int32 best = 0;
                for (Int32 i = 0; i < values.length; i++) {
                    Int64 timeout = Long->ParseLong(values[i].ToString());
                    if (currentTimeout >= timeout) {
                        best = i;
                    }
                }
                summary = preference->GetContext()->GetString(R::string::screen_timeout_summary,
                        entries[best]);
            }
        }
        preference->SetSummary(summary);
    }

    private void DisableUnusableTimeouts(ListPreference screenTimeoutPreference) {
        final DevicePolicyManager dpm =
                (DevicePolicyManager) GetActivity()->GetSystemService(
                Context.DEVICE_POLICY_SERVICE);
        final Int64 maxTimeout = dpm != NULL ? dpm->GetMaximumTimeToLock(NULL) : 0;
        if (maxTimeout == 0) {
            return; // policy not enforced
        }
        final CharSequence[] entries = screenTimeoutPreference->GetEntries();
        final CharSequence[] values = screenTimeoutPreference->GetEntryValues();
        ArrayList<CharSequence> revisedEntries = new ArrayList<CharSequence>();
        ArrayList<CharSequence> revisedValues = new ArrayList<CharSequence>();
        for (Int32 i = 0; i < values.length; i++) {
            Int64 timeout = Long->ParseLong(values[i].ToString());
            if (timeout <= maxTimeout) {
                revisedEntries->Add(entries[i]);
                revisedValues->Add(values[i]);
            }
        }
        if (revisedEntries->Size() != entries.length || revisedValues->Size() != values.length) {
            final Int32 userPreference = Integer->ParseInt(screenTimeoutPreference->GetValue());
            screenTimeoutPreference->SetEntries(
                    revisedEntries->ToArray(new CharSequence[revisedEntries->Size()]));
            screenTimeoutPreference->SetEntryValues(
                    revisedValues->ToArray(new CharSequence[revisedValues->Size()]));
            if (userPreference <= maxTimeout) {
                screenTimeoutPreference->SetValue(String->ValueOf(userPreference));
            } else if (revisedValues->Size() > 0
                    && Long->ParseLong(revisedValues->Get(revisedValues->Size() - 1).ToString())
                    == maxTimeout) {
                // If the last one happens to be the same as the max timeout, select that
                screenTimeoutPreference->SetValue(String->ValueOf(maxTimeout));
            } else {
                // There will be no highlighted selection since nothing in the list matches
                // maxTimeout. The user can still select anything less than maxTimeout.
                // TODO: maybe append maxTimeout to the list and mark selected.
            }
        }
        screenTimeoutPreference->SetEnabled(revisedEntries->Size() > 0);
    }

    Int32 FloatToIndex(Float val) {
        String[] indices = GetResources()->GetStringArray(R.array.entryvalues_font_size);
        Float lastVal = Float->ParseFloat(indices[0]);
        for (Int32 i=1; i<indices.length; i++) {
            Float thisVal = Float->ParseFloat(indices[i]);
            if (val < (lastVal + (thisVal-lastVal)*.5f)) {
                return i-1;
            }
            lastVal = thisVal;
        }
        return indices.length-1;
    }

    CARAPI ReadFontSizePreference(ListPreference pref) {
        try {
            mCurConfig->UpdateFrom(ActivityManagerNative->GetDefault()->GetConfiguration());
        } catch (RemoteException e) {
            Logger::W(TAG, "Unable to retrieve font size");
        }

        // mark the appropriate item in the preferences list
        Int32 index = FloatToIndex(mCurConfig.fontScale);
        pref->SetValueIndex(index);

        // report the current size in the summary text
        final Resources res = GetResources();
        String[] fontSizeNames = res->GetStringArray(R.array.entries_font_size);
        pref->SetSummary(String->Format(res->GetString(R::string::summary_font_size),
                fontSizeNames[index]));
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        UpdateState();
    }

    //@Override
    public Dialog OnCreateDialog(Int32 dialogId) {
        if (dialogId == DLG_GLOBAL_CHANGE_WARNING) {
            return Utils->BuildGlobalChangeWarningDialog(GetActivity(),
                    R::string::global_font_change_title,
                    new Runnable() {
                        CARAPI Run() {
                            mFontSizePref->Click();
                        }
                    });
        }
        return NULL;
    }

    private void UpdateState() {
        ReadFontSizePreference(mFontSizePref);
        UpdateScreenSaverSummary();

        // Update auto brightness if it is available.
        if (mAutoBrightnessPreference != NULL) {
            Int32 brightnessMode = Settings.System->GetInt(GetContentResolver(),
                    SCREEN_BRIGHTNESS_MODE, SCREEN_BRIGHTNESS_MODE_MANUAL);
            mAutoBrightnessPreference->SetChecked(brightnessMode != SCREEN_BRIGHTNESS_MODE_MANUAL);
        }

        // Update lift-to-wake if it is available.
        if (mLiftToWakePreference != NULL) {
            Int32 value = Settings.Secure->GetInt(GetContentResolver(), WAKE_GESTURE_ENABLED, 0);
            mLiftToWakePreference->SetChecked(value != 0);
        }

        // Update doze if it is available.
        if (mDozePreference != NULL) {
            Int32 value = Settings.Secure->GetInt(GetContentResolver(), DOZE_ENABLED, 1);
            mDozePreference->SetChecked(value != 0);
        }
    }

    private void UpdateScreenSaverSummary() {
        if (mScreenSaverPreference != NULL) {
            mScreenSaverPreference->SetSummary(
                    DreamSettings->GetSummaryTextWithDreamName(GetActivity()));
        }
    }

    CARAPI WriteFontSizePreference(Object objValue) {
        try {
            mCurConfig.fontScale = Float->ParseFloat(objValue->ToString());
            ActivityManagerNative->GetDefault()->UpdatePersistentConfiguration(mCurConfig);
        } catch (RemoteException e) {
            Logger::W(TAG, "Unable to save font size");
        }
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        return super->OnPreferenceTreeClick(preferenceScreen, preference);
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object objValue) {
        final String key = preference->GetKey();
        if (KEY_SCREEN_TIMEOUT->Equals(key)) {
            try {
                Int32 value = Integer->ParseInt((String) objValue);
                Settings.System->PutInt(GetContentResolver(), SCREEN_OFF_TIMEOUT, value);
                UpdateTimeoutPreferenceDescription(value);
            } catch (NumberFormatException e) {
                Logger::E(TAG, "could not persist screen timeout setting", e);
            }
        }
        if (KEY_FONT_SIZE->Equals(key)) {
            WriteFontSizePreference(objValue);
        }
        if (preference == mAutoBrightnessPreference) {
            Boolean auto = (Boolean) objValue;
            Settings.System->PutInt(GetContentResolver(), SCREEN_BRIGHTNESS_MODE,
                    auto ? SCREEN_BRIGHTNESS_MODE_AUTOMATIC : SCREEN_BRIGHTNESS_MODE_MANUAL);
        }
        if (preference == mLiftToWakePreference) {
            Boolean value = (Boolean) objValue;
            Settings.Secure->PutInt(GetContentResolver(), WAKE_GESTURE_ENABLED, value ? 1 : 0);
        }
        if (preference == mDozePreference) {
            Boolean value = (Boolean) objValue;
            Settings.Secure->PutInt(GetContentResolver(), DOZE_ENABLED, value ? 1 : 0);
        }
        return TRUE;
    }

    //@Override
    public Boolean OnPreferenceClick(Preference preference) {
        if (preference == mFontSizePref) {
            if (Utils->HasMultipleUsers(GetActivity())) {
                ShowDialog(DLG_GLOBAL_CHANGE_WARNING);
                return TRUE;
            } else {
                mFontSizePref->Click();
            }
        }
        return FALSE;
    }

    public static const Indexable.SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
            new BaseSearchIndexProvider() {
                //@Override
                public List<SearchIndexableResource> GetXmlResourcesToIndex(Context context,
                        Boolean enabled) {
                    ArrayList<SearchIndexableResource> result =
                            new ArrayList<SearchIndexableResource>();

                    SearchIndexableResource sir = new SearchIndexableResource(context);
                    sir.xmlResId = R.xml.display_settings;
                    result->Add(sir);

                    return result;
                }

                //@Override
                public List<String> GetNonIndexableKeys(Context context) {
                    ArrayList<String> result = new ArrayList<String>();
                    if (!context->GetResources()->GetBoolean(
                            R.bool.config_dreamsSupported)) {
                        result->Add(KEY_SCREEN_SAVER);
                    }
                    if (!IsAutomaticBrightnessAvailable(context->GetResources())) {
                        result->Add(KEY_AUTO_BRIGHTNESS);
                    }
                    if (!IsLiftToWakeAvailable(context)) {
                        result->Add(KEY_LIFT_TO_WAKE);
                    }
                    if (!IsDozeAvailable(context)) {
                        result->Add(KEY_DOZE);
                    }
                    if (!RotationPolicy->IsRotationLockToggleVisible(context)) {
                        result->Add(KEY_AUTO_ROTATE);
                    }
                    return result;
                }
            };
}
