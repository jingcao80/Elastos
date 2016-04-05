

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::ISELinux;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndex;
using Elastos::Droid::Settings::Search::IIndexable;

using Elastos::IO::IBufferedReader;
using Elastos::IO::IFileReader;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::IList;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPattern;

public class DeviceInfoSettings extends SettingsPreferenceFragment implements Indexable {

    private static const String LOG_TAG = "DeviceInfoSettings";
    private static const String FILENAME_PROC_VERSION = "/proc/version";
    private static const String FILENAME_MSV = "/sys/board_properties/soc/msv";

    private static const String KEY_CONTAINER = "container";
    private static const String KEY_REGULATORY_INFO = "regulatory_info";
    private static const String KEY_TERMS = "terms";
    private static const String KEY_LICENSE = "license";
    private static const String KEY_COPYRIGHT = "copyright";
    private static const String KEY_SYSTEM_UPDATE_SETTINGS = "system_update_settings";
    private static const String PROPERTY_URL_SAFETYLEGAL = "ro.url.safetylegal";
    private static const String PROPERTY_SELINUX_STATUS = "ro.build.selinux";
    private static const String KEY_KERNEL_VERSION = "kernel_version";
    private static const String KEY_BUILD_NUMBER = "build_number";
    private static const String KEY_DEVICE_MODEL = "device_model";
    private static const String KEY_SELINUX_STATUS = "selinux_status";
    private static const String KEY_BASEBAND_VERSION = "baseband_version";
    private static const String KEY_FIRMWARE_VERSION = "firmware_version";
    private static const String KEY_UPDATE_SETTING = "additional_system_update_settings";
    private static const String KEY_EQUIPMENT_ID = "fcc_equipment_id";
    private static const String PROPERTY_EQUIPMENT_ID = "ro.ril.fccid";
    private static const String KEY_DEVICE_FEEDBACK = "device_feedback";
    private static const String KEY_SAFETY_LEGAL = "safetylegal";

    static const Int32 TAPS_TO_BE_A_DEVELOPER = 7;

    Int64[] mHits = new Int64[3];
    Int32 mDevHitCountdown;
    Toast mDevHitToast;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        AddPreferencesFromResource(R.xml.device_info_settings);

        SetStringSummary(KEY_FIRMWARE_VERSION, Build.VERSION.RELEASE);
        FindPreference(KEY_FIRMWARE_VERSION).SetEnabled(TRUE);
        SetValueSummary(KEY_BASEBAND_VERSION, "gsm.version.baseband");
        SetStringSummary(KEY_DEVICE_MODEL, Build.MODEL + GetMsvSuffix());
        SetValueSummary(KEY_EQUIPMENT_ID, PROPERTY_EQUIPMENT_ID);
        SetStringSummary(KEY_DEVICE_MODEL, Build.MODEL);
        SetStringSummary(KEY_BUILD_NUMBER, Build.DISPLAY);
        FindPreference(KEY_BUILD_NUMBER).SetEnabled(TRUE);
        FindPreference(KEY_KERNEL_VERSION).SetSummary(GetFormattedKernelVersion());

        if (!SELinux->IsSELinuxEnabled()) {
            String status = GetResources()->GetString(R::string::selinux_status_disabled);
            SetStringSummary(KEY_SELINUX_STATUS, status);
        } else if (!SELinux->IsSELinuxEnforced()) {
            String status = GetResources()->GetString(R::string::selinux_status_permissive);
            SetStringSummary(KEY_SELINUX_STATUS, status);
        }

        // Remove selinux information if property is not present
        RemovePreferenceIfPropertyMissing(GetPreferenceScreen(), KEY_SELINUX_STATUS,
                PROPERTY_SELINUX_STATUS);

        // Remove Safety information preference if PROPERTY_URL_SAFETYLEGAL is not set
        RemovePreferenceIfPropertyMissing(GetPreferenceScreen(), KEY_SAFETY_LEGAL,
                PROPERTY_URL_SAFETYLEGAL);

        // Remove Equipment id preference if FCC ID is not set by RIL
        RemovePreferenceIfPropertyMissing(GetPreferenceScreen(), KEY_EQUIPMENT_ID,
                PROPERTY_EQUIPMENT_ID);

        // Remove Baseband version if wifi-only device
        if (Utils->IsWifiOnly(GetActivity())) {
            GetPreferenceScreen()->RemovePreference(FindPreference(KEY_BASEBAND_VERSION));
        }

        // Dont show feedback option if there is no reporter.
        if (TextUtils->IsEmpty(GetFeedbackReporterPackage(GetActivity()))) {
            GetPreferenceScreen()->RemovePreference(FindPreference(KEY_DEVICE_FEEDBACK));
        }

        /*
         * Settings is a generic app and should not contain any device-specific
         * info.
         */
        final Activity act = GetActivity();
        // These are contained in the "container" preference group
        PreferenceGroup parentPreference = (PreferenceGroup) FindPreference(KEY_CONTAINER);
        Utils->UpdatePreferenceToSpecificActivityOrRemove(act, parentPreference, KEY_TERMS,
                Utils.UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY);
        Utils->UpdatePreferenceToSpecificActivityOrRemove(act, parentPreference, KEY_LICENSE,
                Utils.UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY);
        Utils->UpdatePreferenceToSpecificActivityOrRemove(act, parentPreference, KEY_COPYRIGHT,
                Utils.UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY);

        // These are contained by the root preference screen
        parentPreference = GetPreferenceScreen();
        if (UserHandle->MyUserId() == UserHandle.USER_OWNER) {
            Utils->UpdatePreferenceToSpecificActivityOrRemove(act, parentPreference,
                    KEY_SYSTEM_UPDATE_SETTINGS,
                    Utils.UPDATE_PREFERENCE_FLAG_SET_TITLE_TO_MATCHING_ACTIVITY);
        } else {
            // Remove for secondary users
            RemovePreference(KEY_SYSTEM_UPDATE_SETTINGS);
        }

        // Read platform settings for additional system update setting
        RemovePreferenceIfBoolFalse(KEY_UPDATE_SETTING,
                R.bool.config_additional_system_update_setting_enable);

        // Remove regulatory information if none present.
        final Intent intent = new Intent(Settings.ACTION_SHOW_REGULATORY_INFO);
        if (GetPackageManager()->QueryIntentActivities(intent, 0).IsEmpty()) {
            Preference pref = FindPreference(KEY_REGULATORY_INFO);
            if (pref != NULL) {
                GetPreferenceScreen()->RemovePreference(pref);
            }
        }
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        mDevHitCountdown = GetActivity()->GetSharedPreferences(DevelopmentSettings.PREF_FILE,
                Context.MODE_PRIVATE).GetBoolean(DevelopmentSettings.PREF_SHOW,
                        android.os.Build.TYPE->Equals("eng")) ? -1 : TAPS_TO_BE_A_DEVELOPER;
        mDevHitToast = NULL;
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (preference->GetKey()->Equals(KEY_FIRMWARE_VERSION)) {
            System->Arraycopy(mHits, 1, mHits, 0, mHits.length-1);
            mHits[mHits.length-1] = SystemClock->UptimeMillis();
            if (mHits[0] >= (SystemClock->UptimeMillis()-500)) {
                Intent intent = new Intent(IIntent::ACTION_MAIN);
                intent->SetClassName("android",
                        com.android.internal.app.PlatLogoActivity.class->GetName());
                try {
                    StartActivity(intent);
                } catch (Exception e) {
                    Logger::E(LOG_TAG, "Unable to start activity " + intent->ToString());
                }
            }
        } else if (preference->GetKey()->Equals(KEY_BUILD_NUMBER)) {
            // Don't enable developer options for secondary users.
            if (UserHandle->MyUserId() != UserHandle.USER_OWNER) return TRUE;

            final UserManager um = (UserManager) GetSystemService(Context.USER_SERVICE);
            if (um->HasUserRestriction(UserManager.DISALLOW_DEBUGGING_FEATURES)) return TRUE;

            if (mDevHitCountdown > 0) {
                mDevHitCountdown--;
                if (mDevHitCountdown == 0) {
                    GetActivity()->GetSharedPreferences(DevelopmentSettings.PREF_FILE,
                            Context.MODE_PRIVATE).Edit()->PutBoolean(
                                    DevelopmentSettings.PREF_SHOW, TRUE).Apply();
                    if (mDevHitToast != NULL) {
                        mDevHitToast->Cancel();
                    }
                    mDevHitToast = Toast->MakeText(GetActivity(), R::string::show_dev_on,
                            Toast.LENGTH_LONG);
                    mDevHitToast->Show();
                    // This is good time to index the Developer Options
                    Index->GetInstance(
                            GetActivity()->GetApplicationContext()).UpdateFromClassNameResource(
                                    DevelopmentSettings.class->GetName(), TRUE, TRUE);

                } else if (mDevHitCountdown > 0
                        && mDevHitCountdown < (TAPS_TO_BE_A_DEVELOPER-2)) {
                    if (mDevHitToast != NULL) {
                        mDevHitToast->Cancel();
                    }
                    mDevHitToast = Toast->MakeText(GetActivity(), GetResources()->GetQuantityString(
                            R.plurals.show_dev_countdown, mDevHitCountdown, mDevHitCountdown),
                            Toast.LENGTH_SHORT);
                    mDevHitToast->Show();
                }
            } else if (mDevHitCountdown < 0) {
                if (mDevHitToast != NULL) {
                    mDevHitToast->Cancel();
                }
                mDevHitToast = Toast->MakeText(GetActivity(), R::string::show_dev_already,
                        Toast.LENGTH_LONG);
                mDevHitToast->Show();
            }
        } else if (preference->GetKey()->Equals(KEY_DEVICE_FEEDBACK)) {
            SendFeedback();
        }
        return super->OnPreferenceTreeClick(preferenceScreen, preference);
    }

    private void RemovePreferenceIfPropertyMissing(PreferenceGroup preferenceGroup,
            String preference, String property ) {
        if (SystemProperties->Get(property).Equals("")) {
            // Property is missing so remove preference from group
            try {
                preferenceGroup->RemovePreference(FindPreference(preference));
            } catch (RuntimeException e) {
                Logger::D(LOG_TAG, "Property '" + property + "' missing and no '"
                        + preference + "' preference");
            }
        }
    }

    private void RemovePreferenceIfBoolFalse(String preference, Int32 resId) {
        if (!GetResources()->GetBoolean(resId)) {
            Preference pref = FindPreference(preference);
            if (pref != NULL) {
                GetPreferenceScreen()->RemovePreference(pref);
            }
        }
    }

    private void SetStringSummary(String preference, String value) {
        try {
            FindPreference(preference).SetSummary(value);
        } catch (RuntimeException e) {
            FindPreference(preference).SetSummary(
                GetResources()->GetString(R::string::device_info_default));
        }
    }

    private void SetValueSummary(String preference, String property) {
        try {
            FindPreference(preference).SetSummary(
                    SystemProperties->Get(property,
                            GetResources()->GetString(R::string::device_info_default)));
        } catch (RuntimeException e) {
            // No recovery
        }
    }

    private void SendFeedback() {
        String reporterPackage = GetFeedbackReporterPackage(GetActivity());
        if (TextUtils->IsEmpty(reporterPackage)) {
            return;
        }
        Intent intent = new Intent(IIntent::ACTION_BUG_REPORT);
        intent->SetPackage(reporterPackage);
        StartActivityForResult(intent, 0);
    }

    /**
     * Reads a line from the specified file.
     * @param filename the file to read from
     * @return the first line, if any.
     * @throws IOException if the file couldn't be read
     */
    private static String ReadLine(String filename) throws IOException {
        BufferedReader reader = new BufferedReader(new FileReader(filename), 256);
        try {
            return reader->ReadLine();
        } finally {
            reader->Close();
        }
    }

    public static String GetFormattedKernelVersion() {
        try {
            return FormatKernelVersion(ReadLine(FILENAME_PROC_VERSION));

        } catch (IOException e) {
            Logger::E(LOG_TAG,
                "IO Exception when getting kernel version for Device Info screen",
                e);

            return "Unavailable";
        }
    }

    public static String FormatKernelVersion(String rawKernelVersion) {
        // Example (see tests for more):
        // Linux version 3.0.31-g6fb96c9 (android-build@xxx.xxx.xxx.xxx.com) \
        //     (gcc version 4.6.x-xxx 20120106 (prerelease) (GCC) ) #1 SMP PREEMPT \
        //     Thu Jun 28 11:02:39 PDT 2012

        final String PROC_VERSION_REGEX =
            "Linux version (\\S+) " + /* group 1: "3.0.31-g6fb96c9" */
            "\\((\\S+?)\\) " +        /* group 2: "x@y.com" (kernel builder) */
            "(?:\\(gcc.+? \\)) " +    /* ignore: GCC version information */
            "(#\\d+) " +              /* group 3: "#1" */
            "(?:.*?)?" +              /* ignore: optional SMP, PREEMPT, and any CONFIG_FLAGS */
            "((Sun|Mon|Tue|Wed|Thu|Fri|Sat).+)"; /* group 4: "Thu Jun 28 11:02:39 PDT 2012" */

        Matcher m = Pattern->Compile(PROC_VERSION_REGEX).Matcher(rawKernelVersion);
        if (!m->Matches()) {
            Logger::E(LOG_TAG, "Regex did not match on /proc/version: " + rawKernelVersion);
            return "Unavailable";
        } else if (m->GroupCount() < 4) {
            Logger::E(LOG_TAG, "Regex match on /proc/version only returned " + m->GroupCount()
                    + " groups");
            return "Unavailable";
        }
        return m->Group(1) + "\n" +                 // 3.0.31-g6fb96c9
            m->Group(2) + " " + m->Group(3) + "\n" + // x@y.com #1
            m->Group(4);                            // Thu Jun 28 11:02:39 PDT 2012
    }

    /**
     * Returns " (ENGINEERING)" if the msv file has a zero value, else returns "".
     * @return a string to append to the model number description.
     */
    private String GetMsvSuffix() {
        // Production devices should have a non-zero value. If we can't read it, assume it's a
        // production device so that we don't accidentally show that it's an ENGINEERING device.
        try {
            String msv = ReadLine(FILENAME_MSV);
            // Parse as a hex number. If it evaluates to a zero, then it's an engineering build.
            if (Long->ParseLong(msv, 16) == 0) {
                return " (ENGINEERING)";
            }
        } catch (IOException ioe) {
            // Fail quietly, as the file may not exist on some devices.
        } catch (NumberFormatException nfe) {
            // Fail quietly, returning empty string should be sufficient
        }
        return "";
    }

    private static String GetFeedbackReporterPackage(Context context) {
        final String feedbackReporter =
                context->GetResources()->GetString(R::string::oem_preferred_feedback_reporter);
        if (TextUtils->IsEmpty(feedbackReporter)) {
            // Reporter not configured. Return.
            return feedbackReporter;
        }
        // Additional checks to ensure the reporter is on system image, and reporter is
        // configured to listen to the intent. Otherwise, dont show the "send feedback" option.
        final Intent intent = new Intent(IIntent::ACTION_BUG_REPORT);

        PackageManager pm = context->GetPackageManager();
        List<ResolveInfo> resolvedPackages =
                pm->QueryIntentActivities(intent, PackageManager.GET_RESOLVED_FILTER);
        for (ResolveInfo info : resolvedPackages) {
            if (info.activityInfo != NULL) {
                if (!TextUtils->IsEmpty(info.activityInfo.packageName)) {
                    try {
                        ApplicationInfo ai = pm->GetApplicationInfo(info.activityInfo.packageName, 0);
                        if ((ai.flags & ApplicationInfo.FLAG_SYSTEM) != 0) {
                            // Package is on the system image
                            if (TextUtils->Equals(
                                        info.activityInfo.packageName, feedbackReporter)) {
                                return feedbackReporter;
                            }
                        }
                    } catch (PackageManager.NameNotFoundException e) {
                         // No need to do anything here.
                    }
                }
            }
        }
        return NULL;
    }

    /**
     * For Search.
     */
    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {

            //@Override
            public List<SearchIndexableResource> GetXmlResourcesToIndex(
                    Context context, Boolean enabled) {
                final SearchIndexableResource sir = new SearchIndexableResource(context);
                sir.xmlResId = R.xml.device_info_settings;
                return Arrays->AsList(sir);
            }

            //@Override
            public List<String> GetNonIndexableKeys(Context context) {
                final List<String> keys = new ArrayList<String>();
                if (IsPropertyMissing(PROPERTY_SELINUX_STATUS)) {
                    keys->Add(KEY_SELINUX_STATUS);
                }
                if (IsPropertyMissing(PROPERTY_URL_SAFETYLEGAL)) {
                    keys->Add(KEY_SAFETY_LEGAL);
                }
                if (IsPropertyMissing(PROPERTY_EQUIPMENT_ID)) {
                    keys->Add(KEY_EQUIPMENT_ID);
                }
                // Remove Baseband version if wifi-only device
                if (Utils->IsWifiOnly(context)) {
                    keys->Add((KEY_BASEBAND_VERSION));
                }
                // Dont show feedback option if there is no reporter.
                if (TextUtils->IsEmpty(GetFeedbackReporterPackage(context))) {
                    keys->Add(KEY_DEVICE_FEEDBACK);
                }
                if (!CheckIntentAction(context, "android.settings.TERMS")) {
                    keys->Add(KEY_TERMS);
                }
                if (!CheckIntentAction(context, "android.settings.LICENSE")) {
                    keys->Add(KEY_LICENSE);
                }
                if (!CheckIntentAction(context, "android.settings.COPYRIGHT")) {
                    keys->Add(KEY_COPYRIGHT);
                }
                if (UserHandle->MyUserId() != UserHandle.USER_OWNER) {
                    keys->Add(KEY_SYSTEM_UPDATE_SETTINGS);
                }
                if (!context->GetResources()->GetBoolean(
                        R.bool.config_additional_system_update_setting_enable)) {
                    keys->Add(KEY_UPDATE_SETTING);
                }
                return keys;
            }

            private Boolean IsPropertyMissing(String property) {
                return SystemProperties->Get(property).Equals("");
            }

            private Boolean CheckIntentAction(Context context, String action) {
                final Intent intent = new Intent(action);

                // Find the activity that is in the system image
                final PackageManager pm = context->GetPackageManager();
                final List<ResolveInfo> list = pm->QueryIntentActivities(intent, 0);
                final Int32 listSize = list->Size();

                for (Int32 i = 0; i < listSize; i++) {
                    ResolveInfo resolveInfo = list->Get(i);
                    if ((resolveInfo.activityInfo.applicationInfo.flags &
                            ApplicationInfo.FLAG_SYSTEM) != 0) {
                        return TRUE;
                    }
                }

                return FALSE;
            }
        };

}

