

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManagerNative;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Hardware::Usb::IIUsbManager;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IIBinder;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::Preference::IOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IHardwareRenderer;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Widget::ISwitchBar;
using dalvik::System::IVMRuntime;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;

/*
 * Displays preferences for application developers.
 */
public class DevelopmentSettings extends SettingsPreferenceFragment
        implements DialogInterface.OnClickListener, DialogInterface.OnDismissListener,
                OnPreferenceChangeListener, SwitchBar.OnSwitchChangeListener, Indexable {
    private static const String TAG = "DevelopmentSettings";

    /**
     * Preference file were development settings prefs are stored.
     */
    public static const String PREF_FILE = "development";

    /**
     * Whether to show the development settings to the user.  Default is FALSE.
     */
    public static const String PREF_SHOW = "show";

    private static const String ENABLE_ADB = "enable_adb";
    private static const String CLEAR_ADB_KEYS = "clear_adb_keys";
    private static const String ENABLE_TERMINAL = "enable_terminal";
    private static const String KEEP_SCREEN_ON = "keep_screen_on";
    private static const String BT_HCI_SNOOP_LOG = "bt_hci_snoop_log";
    private static const String ENABLE_OEM_UNLOCK = "oem_unlock_enable";
    private static const String ALLOW_MOCK_LOCATION = "allow_mock_location";
    private static const String HDCP_CHECKING_KEY = "hdcp_checking";
    private static const String HDCP_CHECKING_PROPERTY = "persist.sys.hdcp_checking";
    private static const String LOCAL_BACKUP_PASSWORD = "local_backup_password";
    private static const String HARDWARE_UI_PROPERTY = "persist.sys.ui.hw";
    private static const String MSAA_PROPERTY = "debug.egl.force_msaa";
    private static const String BUGREPORT = "bugreport";
    private static const String BUGREPORT_IN_POWER_KEY = "bugreport_in_power";
    private static const String OPENGL_TRACES_PROPERTY = "debug.egl.trace";

    private static const String DEBUG_APP_KEY = "debug_app";
    private static const String WAIT_FOR_DEBUGGER_KEY = "wait_for_debugger";
    private static const String VERIFY_APPS_OVER_USB_KEY = "verify_apps_over_usb";
    private static const String DEBUG_VIEW_ATTRIBUTES =  "debug_view_attributes";
    private static const String STRICT_MODE_KEY = "strict_mode";
    private static const String POINTER_LOCATION_KEY = "pointer_location";
    private static const String SHOW_TOUCHES_KEY = "show_touches";
    private static const String SHOW_SCREEN_UPDATES_KEY = "show_screen_updates";
    private static const String DISABLE_OVERLAYS_KEY = "disable_overlays";
    private static const String SIMULATE_COLOR_SPACE = "simulate_color_space";
    private static const String USE_NUPLAYER_KEY = "use_nuplayer";
    private static const String USB_AUDIO_KEY = "usb_audio";
    private static const String USE_AWESOMEPLAYER_PROPERTY = "persist.sys.media.use-awesome";
    private static const String SHOW_CPU_USAGE_KEY = "show_cpu_usage";
    private static const String FORCE_HARDWARE_UI_KEY = "force_hw_ui";
    private static const String FORCE_MSAA_KEY = "force_msaa";
    private static const String TRACK_FRAME_TIME_KEY = "track_frame_time";
    private static const String SHOW_NON_RECTANGULAR_CLIP_KEY = "show_non_rect_clip";
    private static const String SHOW_HW_SCREEN_UPDATES_KEY = "show_hw_screen_udpates";
    private static const String SHOW_HW_LAYERS_UPDATES_KEY = "show_hw_layers_udpates";
    private static const String DEBUG_HW_OVERDRAW_KEY = "debug_hw_overdraw";
    private static const String DEBUG_LAYOUT_KEY = "debug_layout";
    private static const String FORCE_RTL_LAYOUT_KEY = "force_rtl_layout_all_locales";
    private static const String WINDOW_ANIMATION_SCALE_KEY = "window_animation_scale";
    private static const String TRANSITION_ANIMATION_SCALE_KEY = "transition_animation_scale";
    private static const String ANIMATOR_DURATION_SCALE_KEY = "animator_duration_scale";
    private static const String OVERLAY_DISPLAY_DEVICES_KEY = "overlay_display_devices";
    private static const String DEBUG_DEBUGGING_CATEGORY_KEY = "debug_debugging_category";
    private static const String DEBUG_APPLICATIONS_CATEGORY_KEY = "debug_applications_category";
    private static const String WIFI_DISPLAY_CERTIFICATION_KEY = "wifi_display_certification";
    private static const String WIFI_VERBOSE_LOGGING_KEY = "wifi_verbose_logging";
    private static const String WIFI_AGGRESSIVE_HANDOVER_KEY = "wifi_aggressive_handover";
    private static const String WIFI_ALLOW_SCAN_WITH_TRAFFIC_KEY = "wifi_allow_scan_with_traffic";
    private static const String SELECT_LOGD_SIZE_KEY = "select_logd_size";
    private static const String SELECT_LOGD_SIZE_PROPERTY = "persist.logd.size";
    private static const String SELECT_LOGD_DEFAULT_SIZE_PROPERTY = "ro.logd.size";

    private static const String OPENGL_TRACES_KEY = "enable_opengl_traces";

    private static const String IMMEDIATELY_DESTROY_ACTIVITIES_KEY
            = "immediately_destroy_activities";
    private static const String APP_PROCESS_LIMIT_KEY = "app_process_limit";

    private static const String SHOW_ALL_ANRS_KEY = "show_all_anrs";

    private static const String PROCESS_STATS = "proc_stats";

    private static const String TAG_CONFIRM_ENFORCE = "confirm_enforce";

    private static const String PACKAGE_MIME_TYPE = "application/vnd.android.package-archive";

    private static const String TERMINAL_APP_PACKAGE = "com.android.terminal";

    private static const Int32 RESULT_DEBUG_APP = 1000;

    private static const String PERSISTENT_DATA_BLOCK_PROP = "ro.frp.pst";

    private static String DEFAULT_LOG_RING_BUFFER_SIZE_IN_BYTES = "262144"; // 256K

    private IWindowManager mWindowManager;
    private IBackupManager mBackupManager;
    private DevicePolicyManager mDpm;
    private UserManager mUm;
    private WifiManager mWifiManager;

    private SwitchBar mSwitchBar;
    private Boolean mLastEnabledState;
    private Boolean mHaveDebugSettings;
    private Boolean mDontPokeProperties;

    private CheckBoxPreference mEnableAdb;
    private Preference mClearAdbKeys;
    private CheckBoxPreference mEnableTerminal;
    private Preference mBugreport;
    private CheckBoxPreference mBugreportInPower;
    private CheckBoxPreference mKeepScreenOn;
    private CheckBoxPreference mBtHciSnoopLog;
    private CheckBoxPreference mEnableOemUnlock;
    private CheckBoxPreference mAllowMockLocation;
    private CheckBoxPreference mDebugViewAttributes;

    private PreferenceScreen mPassword;
    private String mDebugApp;
    private Preference mDebugAppPref;
    private CheckBoxPreference mWaitForDebugger;
    private CheckBoxPreference mVerifyAppsOverUsb;
    private CheckBoxPreference mWifiDisplayCertification;
    private CheckBoxPreference mWifiVerboseLogging;
    private CheckBoxPreference mWifiAggressiveHandover;

    private CheckBoxPreference mWifiAllowScansWithTraffic;
    private CheckBoxPreference mStrictMode;
    private CheckBoxPreference mPointerLocation;
    private CheckBoxPreference mShowTouches;
    private CheckBoxPreference mShowScreenUpdates;
    private CheckBoxPreference mDisableOverlays;
    private CheckBoxPreference mShowCpuUsage;
    private CheckBoxPreference mForceHardwareUi;
    private CheckBoxPreference mForceMsaa;
    private CheckBoxPreference mShowHwScreenUpdates;
    private CheckBoxPreference mShowHwLayersUpdates;
    private CheckBoxPreference mDebugLayout;
    private CheckBoxPreference mForceRtlLayout;
    private ListPreference mDebugHwOverdraw;
    private ListPreference mLogdSize;
    private ListPreference mTrackFrameTime;
    private ListPreference mShowNonRectClip;
    private ListPreference mWindowAnimationScale;
    private ListPreference mTransitionAnimationScale;
    private ListPreference mAnimatorDurationScale;
    private ListPreference mOverlayDisplayDevices;
    private ListPreference mOpenGLTraces;

    private ListPreference mSimulateColorSpace;

    private CheckBoxPreference mUseNuplayer;
    private CheckBoxPreference mUSBAudio;
    private CheckBoxPreference mImmediatelyDestroyActivities;

    private ListPreference mAppProcessLimit;

    private CheckBoxPreference mShowAllANRs;

    private PreferenceScreen mProcessStats;
    private final ArrayList<Preference> mAllPrefs = new ArrayList<Preference>();

    private final ArrayList<CheckBoxPreference> mResetCbPrefs
            = new ArrayList<CheckBoxPreference>();

    private final HashSet<Preference> mDisabledPrefs = new HashSet<Preference>();
    // To track whether a confirmation dialog was clicked.
    private Boolean mDialogClicked;
    private Dialog mEnableDialog;
    private Dialog mAdbDialog;

    private Dialog mAdbKeysDialog;
    private Boolean mUnavailable;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mWindowManager = IWindowManager.Stub->AsInterface(ServiceManager->GetService("window"));
        mBackupManager = IBackupManager.Stub->AsInterface(
                ServiceManager->GetService(Context.BACKUP_SERVICE));
        mDpm = (DevicePolicyManager)GetActivity()->GetSystemService(Context.DEVICE_POLICY_SERVICE);
        mUm = (UserManager) GetSystemService(Context.USER_SERVICE);

        mWifiManager = (WifiManager) GetSystemService(Context.WIFI_SERVICE);

        if (android.os.Process->MyUserHandle()->GetIdentifier() != UserHandle.USER_OWNER
                || mUm->HasUserRestriction(UserManager.DISALLOW_DEBUGGING_FEATURES)) {
            mUnavailable = TRUE;
            SetPreferenceScreen(new PreferenceScreen(GetActivity(), NULL));
            return;
        }

        AddPreferencesFromResource(R.xml.development_prefs);

        final PreferenceGroup debugDebuggingCategory = (PreferenceGroup)
                FindPreference(DEBUG_DEBUGGING_CATEGORY_KEY);

        mEnableAdb = FindAndInitCheckboxPref(ENABLE_ADB);
        mClearAdbKeys = FindPreference(CLEAR_ADB_KEYS);
        if (!SystemProperties->GetBoolean("ro.adb.secure", FALSE)) {
            if (debugDebuggingCategory != NULL) {
                debugDebuggingCategory->RemovePreference(mClearAdbKeys);
            }
        }
        mAllPrefs->Add(mClearAdbKeys);
        mEnableTerminal = FindAndInitCheckboxPref(ENABLE_TERMINAL);
        if (!IsPackageInstalled(GetActivity(), TERMINAL_APP_PACKAGE)) {
            debugDebuggingCategory->RemovePreference(mEnableTerminal);
            mEnableTerminal = NULL;
        }

        mBugreport = FindPreference(BUGREPORT);
        mBugreportInPower = FindAndInitCheckboxPref(BUGREPORT_IN_POWER_KEY);
        mKeepScreenOn = FindAndInitCheckboxPref(KEEP_SCREEN_ON);
        mBtHciSnoopLog = FindAndInitCheckboxPref(BT_HCI_SNOOP_LOG);
        mEnableOemUnlock = FindAndInitCheckboxPref(ENABLE_OEM_UNLOCK);
        if (!ShowEnableOemUnlockPreference()) {
            RemovePreference(mEnableOemUnlock);
            mEnableOemUnlock = NULL;
        }
        mAllowMockLocation = FindAndInitCheckboxPref(ALLOW_MOCK_LOCATION);
        mDebugViewAttributes = FindAndInitCheckboxPref(DEBUG_VIEW_ATTRIBUTES);
        mPassword = (PreferenceScreen) FindPreference(LOCAL_BACKUP_PASSWORD);
        mAllPrefs->Add(mPassword);


        if (!android.os.Process->MyUserHandle()->Equals(UserHandle.OWNER)) {
            DisableForUser(mEnableAdb);
            DisableForUser(mClearAdbKeys);
            DisableForUser(mEnableTerminal);
            DisableForUser(mPassword);
        }

        mDebugAppPref = FindPreference(DEBUG_APP_KEY);
        mAllPrefs->Add(mDebugAppPref);
        mWaitForDebugger = FindAndInitCheckboxPref(WAIT_FOR_DEBUGGER_KEY);
        mVerifyAppsOverUsb = FindAndInitCheckboxPref(VERIFY_APPS_OVER_USB_KEY);
        if (!ShowVerifierSetting()) {
            if (debugDebuggingCategory != NULL) {
                debugDebuggingCategory->RemovePreference(mVerifyAppsOverUsb);
            } else {
                mVerifyAppsOverUsb->SetEnabled(FALSE);
            }
        }
        mStrictMode = FindAndInitCheckboxPref(STRICT_MODE_KEY);
        mPointerLocation = FindAndInitCheckboxPref(POINTER_LOCATION_KEY);
        mShowTouches = FindAndInitCheckboxPref(SHOW_TOUCHES_KEY);
        mShowScreenUpdates = FindAndInitCheckboxPref(SHOW_SCREEN_UPDATES_KEY);
        mDisableOverlays = FindAndInitCheckboxPref(DISABLE_OVERLAYS_KEY);
        mShowCpuUsage = FindAndInitCheckboxPref(SHOW_CPU_USAGE_KEY);
        mForceHardwareUi = FindAndInitCheckboxPref(FORCE_HARDWARE_UI_KEY);
        mForceMsaa = FindAndInitCheckboxPref(FORCE_MSAA_KEY);
        mTrackFrameTime = AddListPreference(TRACK_FRAME_TIME_KEY);
        mShowNonRectClip = AddListPreference(SHOW_NON_RECTANGULAR_CLIP_KEY);
        mShowHwScreenUpdates = FindAndInitCheckboxPref(SHOW_HW_SCREEN_UPDATES_KEY);
        mShowHwLayersUpdates = FindAndInitCheckboxPref(SHOW_HW_LAYERS_UPDATES_KEY);
        mDebugLayout = FindAndInitCheckboxPref(DEBUG_LAYOUT_KEY);
        mForceRtlLayout = FindAndInitCheckboxPref(FORCE_RTL_LAYOUT_KEY);
        mDebugHwOverdraw = AddListPreference(DEBUG_HW_OVERDRAW_KEY);
        mWifiDisplayCertification = FindAndInitCheckboxPref(WIFI_DISPLAY_CERTIFICATION_KEY);
        mWifiVerboseLogging = FindAndInitCheckboxPref(WIFI_VERBOSE_LOGGING_KEY);
        mWifiAggressiveHandover = FindAndInitCheckboxPref(WIFI_AGGRESSIVE_HANDOVER_KEY);
        mWifiAllowScansWithTraffic = FindAndInitCheckboxPref(WIFI_ALLOW_SCAN_WITH_TRAFFIC_KEY);
        mLogdSize = AddListPreference(SELECT_LOGD_SIZE_KEY);

        mWindowAnimationScale = AddListPreference(WINDOW_ANIMATION_SCALE_KEY);
        mTransitionAnimationScale = AddListPreference(TRANSITION_ANIMATION_SCALE_KEY);
        mAnimatorDurationScale = AddListPreference(ANIMATOR_DURATION_SCALE_KEY);
        mOverlayDisplayDevices = AddListPreference(OVERLAY_DISPLAY_DEVICES_KEY);
        mOpenGLTraces = AddListPreference(OPENGL_TRACES_KEY);
        mSimulateColorSpace = AddListPreference(SIMULATE_COLOR_SPACE);
        mUseNuplayer = FindAndInitCheckboxPref(USE_NUPLAYER_KEY);
        mUSBAudio = FindAndInitCheckboxPref(USB_AUDIO_KEY);

        mImmediatelyDestroyActivities = (CheckBoxPreference) FindPreference(
                IMMEDIATELY_DESTROY_ACTIVITIES_KEY);
        mAllPrefs->Add(mImmediatelyDestroyActivities);
        mResetCbPrefs->Add(mImmediatelyDestroyActivities);

        mAppProcessLimit = AddListPreference(APP_PROCESS_LIMIT_KEY);

        mShowAllANRs = (CheckBoxPreference) FindPreference(
                SHOW_ALL_ANRS_KEY);
        mAllPrefs->Add(mShowAllANRs);
        mResetCbPrefs->Add(mShowAllANRs);

        Preference hdcpChecking = FindPreference(HDCP_CHECKING_KEY);
        if (hdcpChecking != NULL) {
            mAllPrefs->Add(hdcpChecking);
            RemovePreferenceForProduction(hdcpChecking);
        }

        mProcessStats = (PreferenceScreen) FindPreference(PROCESS_STATS);
        mAllPrefs->Add(mProcessStats);
    }

    private ListPreference AddListPreference(String prefKey) {
        ListPreference pref = (ListPreference) FindPreference(prefKey);
        mAllPrefs->Add(pref);
        pref->SetOnPreferenceChangeListener(this);
        return pref;
    }

    private void DisableForUser(Preference pref) {
        if (pref != NULL) {
            pref->SetEnabled(FALSE);
            mDisabledPrefs->Add(pref);
        }
    }

    private CheckBoxPreference FindAndInitCheckboxPref(String key) {
        CheckBoxPreference pref = (CheckBoxPreference) FindPreference(key);
        if (pref == NULL) {
            throw new IllegalArgumentException("Cannot find preference with key = " + key);
        }
        mAllPrefs->Add(pref);
        mResetCbPrefs->Add(pref);
        return pref;
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        final SettingsActivity activity = (SettingsActivity) GetActivity();

        mSwitchBar = activity->GetSwitchBar();
       if (mUnavailable) {
            mSwitchBar->SetEnabled(FALSE);
            return;
        }

        mSwitchBar->AddOnSwitchChangeListener(this);
    }

    private Boolean RemovePreferenceForProduction(Preference preference) {
        if ("user".Equals(Build.TYPE)) {
            RemovePreference(preference);
            return TRUE;
        }
        return FALSE;
    }

    private void RemovePreference(Preference preference) {
        GetPreferenceScreen()->RemovePreference(preference);
        mAllPrefs->Remove(preference);
    }

    private void SetPrefsEnabledState(Boolean enabled) {
        for (Int32 i = 0; i < mAllPrefs->Size(); i++) {
            Preference pref = mAllPrefs->Get(i);
            pref->SetEnabled(enabled && !mDisabledPrefs->Contains(pref));
        }
        UpdateAllOptions();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        if (mUnavailable) {
            // Show error message
            TextView emptyView = (TextView) GetView()->FindViewById(android.R.id.empty);
            GetListView()->SetEmptyView(emptyView);
            if (emptyView != NULL) {
                emptyView->SetText(R::string::development_settings_not_available);
            }
            return;
        }

        if (mDpm->GetMaximumTimeToLock(NULL) > 0) {
            // A DeviceAdmin has specified a maximum time until the device
            // will lock...  in this case we can't allow the user to turn
            // on "stay awake when plugged in" because that would defeat the
            // restriction.
            mDisabledPrefs->Add(mKeepScreenOn);
        } else {
            mDisabledPrefs->Remove(mKeepScreenOn);
        }

        final ContentResolver cr = GetActivity()->GetContentResolver();
        mLastEnabledState = Settings::Global::>GetInt(cr,
                Settings::Global::DEVELOPMENT_SETTINGS_ENABLED, 0) != 0;
        mSwitchBar->SetChecked(mLastEnabledState);
        SetPrefsEnabledState(mLastEnabledState);

        if (mHaveDebugSettings && !mLastEnabledState) {
            // Overall debugging is disabled, but there are some debug
            // settings that are enabled.  This is an invalid state.  Switch
            // to debug settings being enabled, so the user knows there is
            // stuff enabled and can turn it all off if they want.
            Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
                    Settings::Global::DEVELOPMENT_SETTINGS_ENABLED, 1);
            mLastEnabledState = TRUE;
            mSwitchBar->SetChecked(mLastEnabledState);
            SetPrefsEnabledState(mLastEnabledState);
        }
        mSwitchBar->Show();
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();

        if (mUnavailable) {
            return;
        }
        mSwitchBar->RemoveOnSwitchChangeListener(this);
        mSwitchBar->Hide();
    }

    void UpdateCheckBox(CheckBoxPreference checkBox, Boolean value) {
        checkBox->SetChecked(value);
        mHaveDebugSettings |= value;
    }

    private void UpdateAllOptions() {
        final Context context = GetActivity();
        final ContentResolver cr = context->GetContentResolver();
        mHaveDebugSettings = FALSE;
        UpdateCheckBox(mEnableAdb, Settings::Global::>GetInt(cr,
                Settings::Global::ADB_ENABLED, 0) != 0);
        if (mEnableTerminal != NULL) {
            UpdateCheckBox(mEnableTerminal,
                    context->GetPackageManager()->GetApplicationEnabledSetting(TERMINAL_APP_PACKAGE)
                    == PackageManager.COMPONENT_ENABLED_STATE_ENABLED);
        }
        UpdateCheckBox(mBugreportInPower, Settings.Secure->GetInt(cr,
                Settings.Secure.BUGREPORT_IN_POWER_MENU, 0) != 0);
        UpdateCheckBox(mKeepScreenOn, Settings::Global::>GetInt(cr,
                Settings::Global::STAY_ON_WHILE_PLUGGED_IN, 0) != 0);
        UpdateCheckBox(mBtHciSnoopLog, Settings.Secure->GetInt(cr,
                Settings.Secure.BLUETOOTH_HCI_LOG, 0) != 0);
        if (mEnableOemUnlock != NULL) {
            UpdateCheckBox(mEnableOemUnlock, Utils->IsOemUnlockEnabled(GetActivity()));
        }
        UpdateCheckBox(mAllowMockLocation, Settings.Secure->GetInt(cr,
                Settings.Secure.ALLOW_MOCK_LOCATION, 0) != 0);
        UpdateCheckBox(mDebugViewAttributes, Settings::Global::>GetInt(cr,
                Settings::Global::DEBUG_VIEW_ATTRIBUTES, 0) != 0);
        UpdateHdcpValues();
        UpdatePasswordSummary();
        UpdateDebuggerOptions();
        UpdateStrictModeVisualOptions();
        UpdatePointerLocationOptions();
        UpdateShowTouchesOptions();
        UpdateFlingerOptions();
        UpdateCpuUsageOptions();
        UpdateHardwareUiOptions();
        UpdateMsaaOptions();
        UpdateTrackFrameTimeOptions();
        UpdateShowNonRectClipOptions();
        UpdateShowHwScreenUpdatesOptions();
        UpdateShowHwLayersUpdatesOptions();
        UpdateDebugHwOverdrawOptions();
        UpdateDebugLayoutOptions();
        UpdateAnimationScaleOptions();
        UpdateOverlayDisplayDevicesOptions();
        UpdateOpenGLTracesOptions();
        UpdateImmediatelyDestroyActivitiesOptions();
        UpdateAppProcessLimitOptions();
        UpdateShowAllANRsOptions();
        UpdateVerifyAppsOverUsbOptions();
        UpdateBugreportOptions();
        UpdateForceRtlOptions();
        UpdateLogdSizeValues();
        UpdateWifiDisplayCertificationOptions();
        UpdateWifiVerboseLoggingOptions();
        UpdateWifiAggressiveHandoverOptions();
        UpdateWifiAllowScansWithTrafficOptions();
        UpdateSimulateColorSpace();
        UpdateUseNuplayerOptions();
        UpdateUSBAudioOptions();
    }

    private void ResetDangerousOptions() {
        mDontPokeProperties = TRUE;
        for (Int32 i=0; i<mResetCbPrefs->Size(); i++) {
            CheckBoxPreference cb = mResetCbPrefs->Get(i);
            if (cb->IsChecked()) {
                cb->SetChecked(FALSE);
                OnPreferenceTreeClick(NULL, cb);
            }
        }
        ResetDebuggerOptions();
        WriteLogdSizeOption(NULL);
        WriteAnimationScaleOption(0, mWindowAnimationScale, NULL);
        WriteAnimationScaleOption(1, mTransitionAnimationScale, NULL);
        WriteAnimationScaleOption(2, mAnimatorDurationScale, NULL);
        // Only poke the color space setting if we control it.
        if (UsingDevelopmentColorSpace()) {
            WriteSimulateColorSpace(-1);
        }
        WriteOverlayDisplayDevicesOptions(NULL);
        WriteAppProcessLimitOptions(NULL);
        mHaveDebugSettings = FALSE;
        UpdateAllOptions();
        mDontPokeProperties = FALSE;
        PokeSystemProperties();
    }

    private void UpdateHdcpValues() {
        ListPreference hdcpChecking = (ListPreference) FindPreference(HDCP_CHECKING_KEY);
        if (hdcpChecking != NULL) {
            String currentValue = SystemProperties->Get(HDCP_CHECKING_PROPERTY);
            String[] values = GetResources()->GetStringArray(R.array.hdcp_checking_values);
            String[] summaries = GetResources()->GetStringArray(R.array.hdcp_checking_summaries);
            Int32 index = 1; // Defaults to drm-only. Needs to match with R.array.hdcp_checking_values
            for (Int32 i = 0; i < values.length; i++) {
                if (currentValue->Equals(values[i])) {
                    index = i;
                    break;
                }
            }
            hdcpChecking->SetValue(values[index]);
            hdcpChecking->SetSummary(summaries[index]);
            hdcpChecking->SetOnPreferenceChangeListener(this);
        }
    }

    private void UpdatePasswordSummary() {
        try {
            if (mBackupManager->HasBackupPassword()) {
                mPassword->SetSummary(R::string::local_backup_password_summary_change);
            } else {
                mPassword->SetSummary(R::string::local_backup_password_summary_none);
            }
        } catch (RemoteException e) {
            // Not much we can do here
        }
    }

    private void WriteBtHciSnoopLogOptions() {
        BluetoothAdapter adapter = BluetoothAdapter->GetDefaultAdapter();
        adapter->ConfigHciSnoopLog(mBtHciSnoopLog->IsChecked());
        Settings.Secure->PutInt(GetActivity()->GetContentResolver(),
                Settings.Secure.BLUETOOTH_HCI_LOG,
                mBtHciSnoopLog->IsChecked() ? 1 : 0);
    }

    private void WriteDebuggerOptions() {
        try {
            ActivityManagerNative->GetDefault()->SetDebugApp(
                mDebugApp, mWaitForDebugger->IsChecked(), TRUE);
        } catch (RemoteException ex) {
        }
    }

    private static void ResetDebuggerOptions() {
        try {
            ActivityManagerNative->GetDefault()->SetDebugApp(
                    NULL, FALSE, TRUE);
        } catch (RemoteException ex) {
        }
    }

    private void UpdateDebuggerOptions() {
        mDebugApp = Settings::Global::>GetString(
                GetActivity()->GetContentResolver(), Settings::Global::DEBUG_APP);
        UpdateCheckBox(mWaitForDebugger, Settings::Global::>GetInt(
                GetActivity()->GetContentResolver(), Settings::Global::WAIT_FOR_DEBUGGER, 0) != 0);
        if (mDebugApp != NULL && mDebugApp->Length() > 0) {
            String label;
            try {
                ApplicationInfo ai = GetActivity()->GetPackageManager().GetApplicationInfo(mDebugApp,
                        PackageManager.GET_DISABLED_COMPONENTS);
                CharSequence lab = GetActivity()->GetPackageManager().GetApplicationLabel(ai);
                label = lab != NULL ? lab->ToString() : mDebugApp;
            } catch (PackageManager.NameNotFoundException e) {
                label = mDebugApp;
            }
            mDebugAppPref->SetSummary(GetResources()->GetString(R::string::debug_app_set, label));
            mWaitForDebugger->SetEnabled(TRUE);
            mHaveDebugSettings = TRUE;
        } else {
            mDebugAppPref->SetSummary(GetResources()->GetString(R::string::debug_app_not_set));
            mWaitForDebugger->SetEnabled(FALSE);
        }
    }

    private void UpdateVerifyAppsOverUsbOptions() {
        UpdateCheckBox(mVerifyAppsOverUsb, Settings::Global::>GetInt(GetActivity()->GetContentResolver(),
                Settings::Global::PACKAGE_VERIFIER_INCLUDE_ADB, 1) != 0);
        mVerifyAppsOverUsb->SetEnabled(EnableVerifierSetting());
    }

    private void WriteVerifyAppsOverUsbOptions() {
        Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
              Settings::Global::PACKAGE_VERIFIER_INCLUDE_ADB, mVerifyAppsOverUsb->IsChecked() ? 1 : 0);
    }

    private Boolean EnableVerifierSetting() {
        final ContentResolver cr = GetActivity()->GetContentResolver();
        if (Settings::Global::>GetInt(cr, Settings::Global::ADB_ENABLED, 0) == 0) {
            return FALSE;
        }
        if (Settings::Global::>GetInt(cr, Settings::Global::PACKAGE_VERIFIER_ENABLE, 1) == 0) {
            return FALSE;
        } else {
            final PackageManager pm = GetActivity()->GetPackageManager();
            final Intent verification = new Intent(IIntent::ACTION_PACKAGE_NEEDS_VERIFICATION);
            verification->SetType(PACKAGE_MIME_TYPE);
            verification->AddFlags(IIntent::FLAG_GRANT_READ_URI_PERMISSION);
            final List<ResolveInfo> receivers = pm->QueryBroadcastReceivers(verification, 0);
            if (receivers->Size() == 0) {
                return FALSE;
            }
        }
        return TRUE;
    }

    private Boolean ShowVerifierSetting() {
        return Settings::Global::>GetInt(GetActivity()->GetContentResolver(),
                Settings::Global::PACKAGE_VERIFIER_SETTING_VISIBLE, 1) > 0;
    }

    private static Boolean ShowEnableOemUnlockPreference() {
        return !SystemProperties->Get(PERSISTENT_DATA_BLOCK_PROP).Equals("");
    }

    private void UpdateBugreportOptions() {
        if ("user".Equals(Build.TYPE)) {
            final ContentResolver resolver = GetActivity()->GetContentResolver();
            final Boolean adbEnabled = Settings::Global::>GetInt(
                    resolver, Settings::Global::ADB_ENABLED, 0) != 0;
            if (adbEnabled) {
                mBugreport->SetEnabled(TRUE);
                mBugreportInPower->SetEnabled(TRUE);
            } else {
                mBugreport->SetEnabled(FALSE);
                mBugreportInPower->SetEnabled(FALSE);
                mBugreportInPower->SetChecked(FALSE);
                Settings.Secure->PutInt(resolver, Settings.Secure.BUGREPORT_IN_POWER_MENU, 0);
            }
        } else {
            mBugreportInPower->SetEnabled(TRUE);
        }
    }

    // Returns the current state of the system property that controls
    // strictmode flashes.  One of:
    //    0: not explicitly set one way or another
    //    1: on
    //    2: off
    private static Int32 CurrentStrictModeActiveIndex() {
        if (TextUtils->IsEmpty(SystemProperties->Get(StrictMode.VISUAL_PROPERTY))) {
            return 0;
        }
        Boolean enabled = SystemProperties->GetBoolean(StrictMode.VISUAL_PROPERTY, FALSE);
        return enabled ? 1 : 2;
    }

    private void WriteStrictModeVisualOptions() {
        try {
            mWindowManager->SetStrictModeVisualIndicatorPreference(mStrictMode->IsChecked()
                    ? "1" : "");
        } catch (RemoteException e) {
        }
    }

    private void UpdateStrictModeVisualOptions() {
        UpdateCheckBox(mStrictMode, CurrentStrictModeActiveIndex() == 1);
    }

    private void WritePointerLocationOptions() {
        Settings.System->PutInt(GetActivity()->GetContentResolver(),
                Settings.System.POINTER_LOCATION, mPointerLocation->IsChecked() ? 1 : 0);
    }

    private void UpdatePointerLocationOptions() {
        UpdateCheckBox(mPointerLocation, Settings.System->GetInt(GetActivity()->GetContentResolver(),
                Settings.System.POINTER_LOCATION, 0) != 0);
    }

    private void WriteShowTouchesOptions() {
        Settings.System->PutInt(GetActivity()->GetContentResolver(),
                Settings.System.SHOW_TOUCHES, mShowTouches->IsChecked() ? 1 : 0);
    }

    private void UpdateShowTouchesOptions() {
        UpdateCheckBox(mShowTouches, Settings.System->GetInt(GetActivity()->GetContentResolver(),
                Settings.System.SHOW_TOUCHES, 0) != 0);
    }

    private void UpdateFlingerOptions() {
        // magic communication with surface flinger.
        try {
            IBinder flinger = ServiceManager->GetService("SurfaceFlinger");
            if (flinger != NULL) {
                Parcel data = Parcel->Obtain();
                Parcel reply = Parcel->Obtain();
                data->WriteInterfaceToken("android.ui.ISurfaceComposer");
                flinger->Transact(1010, data, reply, 0);
                @SuppressWarnings("unused")
                Int32 showCpu = reply->ReadInt();
                @SuppressWarnings("unused")
                Int32 enableGL = reply->ReadInt();
                Int32 showUpdates = reply->ReadInt();
                UpdateCheckBox(mShowScreenUpdates, showUpdates != 0);
                @SuppressWarnings("unused")
                Int32 showBackground = reply->ReadInt();
                Int32 disableOverlays = reply->ReadInt();
                UpdateCheckBox(mDisableOverlays, disableOverlays != 0);
                reply->Recycle();
                data->Recycle();
            }
        } catch (RemoteException ex) {
        }
    }

    private void WriteShowUpdatesOption() {
        try {
            IBinder flinger = ServiceManager->GetService("SurfaceFlinger");
            if (flinger != NULL) {
                Parcel data = Parcel->Obtain();
                data->WriteInterfaceToken("android.ui.ISurfaceComposer");
                final Int32 showUpdates = mShowScreenUpdates->IsChecked() ? 1 : 0;
                data->WriteInt(showUpdates);
                flinger->Transact(1002, data, NULL, 0);
                data->Recycle();

                UpdateFlingerOptions();
            }
        } catch (RemoteException ex) {
        }
    }

    private void WriteDisableOverlaysOption() {
        try {
            IBinder flinger = ServiceManager->GetService("SurfaceFlinger");
            if (flinger != NULL) {
                Parcel data = Parcel->Obtain();
                data->WriteInterfaceToken("android.ui.ISurfaceComposer");
                final Int32 disableOverlays = mDisableOverlays->IsChecked() ? 1 : 0;
                data->WriteInt(disableOverlays);
                flinger->Transact(1008, data, NULL, 0);
                data->Recycle();

                UpdateFlingerOptions();
            }
        } catch (RemoteException ex) {
        }
    }

    private void UpdateHardwareUiOptions() {
        UpdateCheckBox(mForceHardwareUi, SystemProperties->GetBoolean(HARDWARE_UI_PROPERTY, FALSE));
    }

    private void WriteHardwareUiOptions() {
        SystemProperties->Set(HARDWARE_UI_PROPERTY, mForceHardwareUi->IsChecked() ? "TRUE" : "FALSE");
        PokeSystemProperties();
    }

    private void UpdateMsaaOptions() {
        UpdateCheckBox(mForceMsaa, SystemProperties->GetBoolean(MSAA_PROPERTY, FALSE));
    }

    private void WriteMsaaOptions() {
        SystemProperties->Set(MSAA_PROPERTY, mForceMsaa->IsChecked() ? "TRUE" : "FALSE");
        PokeSystemProperties();
    }

    private void UpdateTrackFrameTimeOptions() {
        String value = SystemProperties->Get(HardwareRenderer.PROFILE_PROPERTY);
        if (value == NULL) {
            value = "";
        }

        CharSequence[] values = mTrackFrameTime->GetEntryValues();
        for (Int32 i = 0; i < values.length; i++) {
            if (value->ContentEquals(values[i])) {
                mTrackFrameTime->SetValueIndex(i);
                mTrackFrameTime->SetSummary(mTrackFrameTime->GetEntries()[i]);
                return;
            }
        }
        mTrackFrameTime->SetValueIndex(0);
        mTrackFrameTime->SetSummary(mTrackFrameTime->GetEntries()[0]);
    }

    private void WriteTrackFrameTimeOptions(Object newValue) {
        SystemProperties->Set(HardwareRenderer.PROFILE_PROPERTY,
                newValue == NULL ? "" : newValue->ToString());
        PokeSystemProperties();
        UpdateTrackFrameTimeOptions();
    }

    private void UpdateShowNonRectClipOptions() {
        String value = SystemProperties->Get(
                HardwareRenderer.DEBUG_SHOW_NON_RECTANGULAR_CLIP_PROPERTY);
        if (value == NULL) {
            value = "hide";
        }

        CharSequence[] values = mShowNonRectClip->GetEntryValues();
        for (Int32 i = 0; i < values.length; i++) {
            if (value->ContentEquals(values[i])) {
                mShowNonRectClip->SetValueIndex(i);
                mShowNonRectClip->SetSummary(mShowNonRectClip->GetEntries()[i]);
                return;
            }
        }
        mShowNonRectClip->SetValueIndex(0);
        mShowNonRectClip->SetSummary(mShowNonRectClip->GetEntries()[0]);
    }

    private void WriteShowNonRectClipOptions(Object newValue) {
        SystemProperties->Set(HardwareRenderer.DEBUG_SHOW_NON_RECTANGULAR_CLIP_PROPERTY,
                newValue == NULL ? "" : newValue->ToString());
        PokeSystemProperties();
        UpdateShowNonRectClipOptions();
    }

    private void UpdateShowHwScreenUpdatesOptions() {
        UpdateCheckBox(mShowHwScreenUpdates,
                SystemProperties->GetBoolean(HardwareRenderer.DEBUG_DIRTY_REGIONS_PROPERTY, FALSE));
    }

    private void WriteShowHwScreenUpdatesOptions() {
        SystemProperties->Set(HardwareRenderer.DEBUG_DIRTY_REGIONS_PROPERTY,
                mShowHwScreenUpdates->IsChecked() ? "TRUE" : NULL);
        PokeSystemProperties();
    }

    private void UpdateShowHwLayersUpdatesOptions() {
        UpdateCheckBox(mShowHwLayersUpdates, SystemProperties->GetBoolean(
                HardwareRenderer.DEBUG_SHOW_LAYERS_UPDATES_PROPERTY, FALSE));
    }

    private void WriteShowHwLayersUpdatesOptions() {
        SystemProperties->Set(HardwareRenderer.DEBUG_SHOW_LAYERS_UPDATES_PROPERTY,
                mShowHwLayersUpdates->IsChecked() ? "TRUE" : NULL);
        PokeSystemProperties();
    }

    private void UpdateDebugHwOverdrawOptions() {
        String value = SystemProperties->Get(HardwareRenderer.DEBUG_OVERDRAW_PROPERTY);
        if (value == NULL) {
            value = "";
        }

        CharSequence[] values = mDebugHwOverdraw->GetEntryValues();
        for (Int32 i = 0; i < values.length; i++) {
            if (value->ContentEquals(values[i])) {
                mDebugHwOverdraw->SetValueIndex(i);
                mDebugHwOverdraw->SetSummary(mDebugHwOverdraw->GetEntries()[i]);
                return;
            }
        }
        mDebugHwOverdraw->SetValueIndex(0);
        mDebugHwOverdraw->SetSummary(mDebugHwOverdraw->GetEntries()[0]);
    }

    private void WriteDebugHwOverdrawOptions(Object newValue) {
        SystemProperties->Set(HardwareRenderer.DEBUG_OVERDRAW_PROPERTY,
                newValue == NULL ? "" : newValue->ToString());
        PokeSystemProperties();
        UpdateDebugHwOverdrawOptions();
    }

    private void UpdateDebugLayoutOptions() {
        UpdateCheckBox(mDebugLayout,
                SystemProperties->GetBoolean(View.DEBUG_LAYOUT_PROPERTY, FALSE));
    }

    private void WriteDebugLayoutOptions() {
        SystemProperties->Set(View.DEBUG_LAYOUT_PROPERTY,
                mDebugLayout->IsChecked() ? "TRUE" : "FALSE");
        PokeSystemProperties();
    }

    private void UpdateSimulateColorSpace() {
        final ContentResolver cr = GetContentResolver();
        final Boolean enabled = Settings.Secure->GetInt(
                cr, Settings.Secure.ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, 0) != 0;
        if (enabled) {
            final String mode = Integer->ToString(Settings.Secure->GetInt(
                    cr, Settings.Secure.ACCESSIBILITY_DISPLAY_DALTONIZER,
                    AccessibilityManager.DALTONIZER_DISABLED));
            mSimulateColorSpace->SetValue(mode);
            final Int32 index = mSimulateColorSpace->FindIndexOfValue(mode);
            if (index < 0) {
                // We're using a mode controlled by accessibility preferences.
                mSimulateColorSpace->SetSummary(GetString(R::string::daltonizer_type_overridden,
                        GetString(R::string::accessibility_display_daltonizer_preference_title)));
            } else {
                mSimulateColorSpace->SetSummary("%s");
            }
        } else {
            mSimulateColorSpace->SetValue(
                    Integer->ToString(AccessibilityManager.DALTONIZER_DISABLED));
        }
    }

    /**
     * @return <code>TRUE</code> if the color space preference is currently
     *         controlled by development settings
     */
    private Boolean UsingDevelopmentColorSpace() {
        final ContentResolver cr = GetContentResolver();
        final Boolean enabled = Settings.Secure->GetInt(
                cr, Settings.Secure.ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, 0) != 0;
        if (enabled) {
            final String mode = Integer->ToString(Settings.Secure->GetInt(
                    cr, Settings.Secure.ACCESSIBILITY_DISPLAY_DALTONIZER,
                    AccessibilityManager.DALTONIZER_DISABLED));
            final Int32 index = mSimulateColorSpace->FindIndexOfValue(mode);
            if (index >= 0) {
                // We're using a mode controlled by developer preferences.
                return TRUE;
            }
        }
        return FALSE;
    }

    private void WriteSimulateColorSpace(Object value) {
        final ContentResolver cr = GetContentResolver();
        final Int32 newMode = Integer->ParseInt(value->ToString());
        if (newMode < 0) {
            Settings.Secure->PutInt(cr, Settings.Secure.ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, 0);
        } else {
            Settings.Secure->PutInt(cr, Settings.Secure.ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, 1);
            Settings.Secure->PutInt(cr, Settings.Secure.ACCESSIBILITY_DISPLAY_DALTONIZER, newMode);
        }
    }

    private void UpdateUseNuplayerOptions() {
        UpdateCheckBox(
                mUseNuplayer, !SystemProperties->GetBoolean(USE_AWESOMEPLAYER_PROPERTY, FALSE));
    }

    private void WriteUseNuplayerOptions() {
        SystemProperties->Set(
                USE_AWESOMEPLAYER_PROPERTY, mUseNuplayer->IsChecked() ? "FALSE" : "TRUE");
        PokeSystemProperties();
    }

    private void UpdateUSBAudioOptions() {
        UpdateCheckBox(mUSBAudio, Settings.Secure->GetInt(GetContentResolver(),
                Settings.Secure.USB_AUDIO_AUTOMATIC_ROUTING_DISABLED, 0) != 0);
    }

    private void WriteUSBAudioOptions() {
        Settings.Secure->PutInt(GetContentResolver(),
                Settings.Secure.USB_AUDIO_AUTOMATIC_ROUTING_DISABLED,
                mUSBAudio->IsChecked() ? 1 : 0);
    }

    private void UpdateForceRtlOptions() {
        UpdateCheckBox(mForceRtlLayout, Settings::Global::>GetInt(GetActivity()->GetContentResolver(),
                Settings::Global::DEVELOPMENT_FORCE_RTL, 0) != 0);
    }

    private void WriteForceRtlOptions() {
        Boolean value = mForceRtlLayout->IsChecked();
        Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
                Settings::Global::DEVELOPMENT_FORCE_RTL, value ? 1 : 0);
        SystemProperties->Set(Settings::Global::DEVELOPMENT_FORCE_RTL, value ? "1" : "0");
        LocalePicker->UpdateLocale(GetActivity()->GetResources().GetConfiguration().locale);
    }

    private void UpdateWifiDisplayCertificationOptions() {
        UpdateCheckBox(mWifiDisplayCertification, Settings::Global::>GetInt(
                GetActivity()->GetContentResolver(),
                Settings::Global::WIFI_DISPLAY_CERTIFICATION_ON, 0) != 0);
    }

    private void WriteWifiDisplayCertificationOptions() {
        Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
                Settings::Global::WIFI_DISPLAY_CERTIFICATION_ON,
                mWifiDisplayCertification->IsChecked() ? 1 : 0);
    }

    private void UpdateWifiVerboseLoggingOptions() {
        Boolean enabled = mWifiManager->GetVerboseLoggingLevel() > 0;
        UpdateCheckBox(mWifiVerboseLogging, enabled);
    }

    private void WriteWifiVerboseLoggingOptions() {
        mWifiManager->EnableVerboseLogging(mWifiVerboseLogging->IsChecked() ? 1 : 0);
    }

    private void UpdateWifiAggressiveHandoverOptions() {
        Boolean enabled = mWifiManager->GetAggressiveHandover() > 0;
        UpdateCheckBox(mWifiAggressiveHandover, enabled);
    }

    private void WriteWifiAggressiveHandoverOptions() {
        mWifiManager->EnableAggressiveHandover(mWifiAggressiveHandover->IsChecked() ? 1 : 0);
    }

    private void UpdateWifiAllowScansWithTrafficOptions() {
        Boolean enabled = mWifiManager->GetAllowScansWithTraffic() > 0;
        UpdateCheckBox(mWifiAllowScansWithTraffic, enabled);
    }

    private void WriteWifiAllowScansWithTrafficOptions() {
        mWifiManager->SetAllowScansWithTraffic(mWifiAllowScansWithTraffic->IsChecked() ? 1 : 0);
    }

    private void UpdateLogdSizeValues() {
        if (mLogdSize != NULL) {
            String currentValue = SystemProperties->Get(SELECT_LOGD_SIZE_PROPERTY);
            if (currentValue == NULL) {
                currentValue = SystemProperties->Get(SELECT_LOGD_DEFAULT_SIZE_PROPERTY);
                if (currentValue == NULL) {
                    currentValue = "256K";
                }
            }
            String[] values = GetResources()->GetStringArray(R.array.select_logd_size_values);
            String[] titles = GetResources()->GetStringArray(R.array.select_logd_size_titles);
            if (SystemProperties->Get("ro.config.low_ram").Equals("TRUE")) {
                mLogdSize->SetEntries(R.array.select_logd_size_lowram_titles);
                titles = GetResources()->GetStringArray(R.array.select_logd_size_lowram_titles);
            }
            String[] summaries = GetResources()->GetStringArray(R.array.select_logd_size_summaries);
            Int32 index = 1; // punt to second entry if not found
            for (Int32 i = 0; i < titles.length; i++) {
                if (currentValue->Equals(values[i])
                        || currentValue->Equals(titles[i])) {
                    index = i;
                    break;
                }
            }
            mLogdSize->SetValue(values[index]);
            mLogdSize->SetSummary(summaries[index]);
            mLogdSize->SetOnPreferenceChangeListener(this);
        }
    }

    private void WriteLogdSizeOption(Object newValue) {
        String currentValue = SystemProperties->Get(SELECT_LOGD_DEFAULT_SIZE_PROPERTY);
        if (currentValue != NULL) {
            DEFAULT_LOG_RING_BUFFER_SIZE_IN_BYTES = currentValue;
        }
        final String size = (newValue != NULL) ?
                newValue->ToString() : DEFAULT_LOG_RING_BUFFER_SIZE_IN_BYTES;
        SystemProperties->Set(SELECT_LOGD_SIZE_PROPERTY, size);
        PokeSystemProperties();
        try {
            Process p = Runtime->GetRuntime()->Exec("logcat -b all -G " + size);
            p->WaitFor();
            Logger::I(TAG, "Logcat ring buffer sizes set to: " + size);
        } catch (Exception e) {
            Logger::W(TAG, "Cannot set logcat ring buffer sizes", e);
        }
        UpdateLogdSizeValues();
    }

    private void UpdateCpuUsageOptions() {
        UpdateCheckBox(mShowCpuUsage, Settings::Global::>GetInt(GetActivity()->GetContentResolver(),
                Settings::Global::SHOW_PROCESSES, 0) != 0);
    }

    private void WriteCpuUsageOptions() {
        Boolean value = mShowCpuUsage->IsChecked();
        Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
                Settings::Global::SHOW_PROCESSES, value ? 1 : 0);
        Intent service = (new Intent())
                .SetClassName("com.android.systemui", "com.android.systemui.LoadAverageService");
        if (value) {
            GetActivity()->StartService(service);
        } else {
            GetActivity()->StopService(service);
        }
    }

    private void WriteImmediatelyDestroyActivitiesOptions() {
        try {
            ActivityManagerNative->GetDefault()->SetAlwaysFinish(
                    mImmediatelyDestroyActivities->IsChecked());
        } catch (RemoteException ex) {
        }
    }

    private void UpdateImmediatelyDestroyActivitiesOptions() {
        UpdateCheckBox(mImmediatelyDestroyActivities, Settings::Global::>GetInt(
            GetActivity()->GetContentResolver(), Settings::Global::ALWAYS_FINISH_ACTIVITIES, 0) != 0);
    }

    private void UpdateAnimationScaleValue(Int32 which, ListPreference pref) {
        try {
            Float scale = mWindowManager->GetAnimationScale(which);
            if (scale != 1) {
                mHaveDebugSettings = TRUE;
            }
            CharSequence[] values = pref->GetEntryValues();
            for (Int32 i=0; i<values.length; i++) {
                Float val = Float->ParseFloat(values[i].ToString());
                if (scale <= val) {
                    pref->SetValueIndex(i);
                    pref->SetSummary(pref->GetEntries()[i]);
                    return;
                }
            }
            pref->SetValueIndex(values.length-1);
            pref->SetSummary(pref->GetEntries()[0]);
        } catch (RemoteException e) {
        }
    }

    private void UpdateAnimationScaleOptions() {
        UpdateAnimationScaleValue(0, mWindowAnimationScale);
        UpdateAnimationScaleValue(1, mTransitionAnimationScale);
        UpdateAnimationScaleValue(2, mAnimatorDurationScale);
    }

    private void WriteAnimationScaleOption(Int32 which, ListPreference pref, Object newValue) {
        try {
            Float scale = newValue != NULL ? Float->ParseFloat(newValue->ToString()) : 1;
            mWindowManager->SetAnimationScale(which, scale);
            UpdateAnimationScaleValue(which, pref);
        } catch (RemoteException e) {
        }
    }

    private void UpdateOverlayDisplayDevicesOptions() {
        String value = Settings::Global::>GetString(GetActivity()->GetContentResolver(),
                Settings::Global::OVERLAY_DISPLAY_DEVICES);
        if (value == NULL) {
            value = "";
        }

        CharSequence[] values = mOverlayDisplayDevices->GetEntryValues();
        for (Int32 i = 0; i < values.length; i++) {
            if (value->ContentEquals(values[i])) {
                mOverlayDisplayDevices->SetValueIndex(i);
                mOverlayDisplayDevices->SetSummary(mOverlayDisplayDevices->GetEntries()[i]);
                return;
            }
        }
        mOverlayDisplayDevices->SetValueIndex(0);
        mOverlayDisplayDevices->SetSummary(mOverlayDisplayDevices->GetEntries()[0]);
    }

    private void WriteOverlayDisplayDevicesOptions(Object newValue) {
        Settings::Global::>PutString(GetActivity()->GetContentResolver(),
                Settings::Global::OVERLAY_DISPLAY_DEVICES, (String)newValue);
        UpdateOverlayDisplayDevicesOptions();
    }

    private void UpdateOpenGLTracesOptions() {
        String value = SystemProperties->Get(OPENGL_TRACES_PROPERTY);
        if (value == NULL) {
            value = "";
        }

        CharSequence[] values = mOpenGLTraces->GetEntryValues();
        for (Int32 i = 0; i < values.length; i++) {
            if (value->ContentEquals(values[i])) {
                mOpenGLTraces->SetValueIndex(i);
                mOpenGLTraces->SetSummary(mOpenGLTraces->GetEntries()[i]);
                return;
            }
        }
        mOpenGLTraces->SetValueIndex(0);
        mOpenGLTraces->SetSummary(mOpenGLTraces->GetEntries()[0]);
    }

    private void WriteOpenGLTracesOptions(Object newValue) {
        SystemProperties->Set(OPENGL_TRACES_PROPERTY, newValue == NULL ? "" : newValue->ToString());
        PokeSystemProperties();
        UpdateOpenGLTracesOptions();
    }

    private void UpdateAppProcessLimitOptions() {
        try {
            Int32 limit = ActivityManagerNative->GetDefault()->GetProcessLimit();
            CharSequence[] values = mAppProcessLimit->GetEntryValues();
            for (Int32 i=0; i<values.length; i++) {
                Int32 val = Integer->ParseInt(values[i].ToString());
                if (val >= limit) {
                    if (i != 0) {
                        mHaveDebugSettings = TRUE;
                    }
                    mAppProcessLimit->SetValueIndex(i);
                    mAppProcessLimit->SetSummary(mAppProcessLimit->GetEntries()[i]);
                    return;
                }
            }
            mAppProcessLimit->SetValueIndex(0);
            mAppProcessLimit->SetSummary(mAppProcessLimit->GetEntries()[0]);
        } catch (RemoteException e) {
        }
    }

    private void WriteAppProcessLimitOptions(Object newValue) {
        try {
            Int32 limit = newValue != NULL ? Integer->ParseInt(newValue->ToString()) : -1;
            ActivityManagerNative->GetDefault()->SetProcessLimit(limit);
            UpdateAppProcessLimitOptions();
        } catch (RemoteException e) {
        }
    }

    private void WriteShowAllANRsOptions() {
        Settings.Secure->PutInt(GetActivity()->GetContentResolver(),
                Settings.Secure.ANR_SHOW_BACKGROUND,
                mShowAllANRs->IsChecked() ? 1 : 0);
    }

    private void UpdateShowAllANRsOptions() {
        UpdateCheckBox(mShowAllANRs, Settings.Secure->GetInt(
            GetActivity()->GetContentResolver(), Settings.Secure.ANR_SHOW_BACKGROUND, 0) != 0);
    }

    //@Override
    CARAPI OnSwitchChanged(Switch switchView, Boolean isChecked) {
        if (switchView != mSwitchBar->GetSwitch()) {
            return;
        }
        if (isChecked != mLastEnabledState) {
            if (isChecked) {
                mDialogClicked = FALSE;
                if (mEnableDialog != NULL) DismissDialogs();
                mEnableDialog = new AlertDialog->Builder(GetActivity()).SetMessage(
                        GetActivity()->GetResources().GetString(
                                R::string::dev_settings_warning_message))
                        .SetTitle(R::string::dev_settings_warning_title)
                        .SetPositiveButton(android.R::string::yes, this)
                        .SetNegativeButton(android.R::string::no, this)
                        .Show();
                mEnableDialog->SetOnDismissListener(this);
            } else {
                ResetDangerousOptions();
                Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
                        Settings::Global::DEVELOPMENT_SETTINGS_ENABLED, 0);
                mLastEnabledState = isChecked;
                SetPrefsEnabledState(mLastEnabledState);
            }
        }
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent data) {
        if (requestCode == RESULT_DEBUG_APP) {
            if (resultCode == Activity.RESULT_OK) {
                mDebugApp = data->GetAction();
                WriteDebuggerOptions();
                UpdateDebuggerOptions();
            }
        } else {
            super->OnActivityResult(requestCode, resultCode, data);
        }
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen preferenceScreen, Preference preference) {
        if (Utils->IsMonkeyRunning()) {
            return FALSE;
        }

        if (preference == mEnableAdb) {
            if (mEnableAdb->IsChecked()) {
                mDialogClicked = FALSE;
                if (mAdbDialog != NULL) DismissDialogs();
                mAdbDialog = new AlertDialog->Builder(GetActivity()).SetMessage(
                        GetActivity()->GetResources().GetString(R::string::adb_warning_message))
                        .SetTitle(R::string::adb_warning_title)
                        .SetPositiveButton(android.R::string::yes, this)
                        .SetNegativeButton(android.R::string::no, this)
                        .Show();
                mAdbDialog->SetOnDismissListener(this);
            } else {
                Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
                        Settings::Global::ADB_ENABLED, 0);
                mVerifyAppsOverUsb->SetEnabled(FALSE);
                mVerifyAppsOverUsb->SetChecked(FALSE);
                UpdateBugreportOptions();
            }
        } else if (preference == mClearAdbKeys) {
            if (mAdbKeysDialog != NULL) DismissDialogs();
            mAdbKeysDialog = new AlertDialog->Builder(GetActivity())
                        .SetMessage(R::string::adb_keys_warning_message)
                        .SetPositiveButton(android.R::string::ok, this)
                        .SetNegativeButton(android.R::string::cancel, NULL)
                        .Show();
        } else if (preference == mEnableTerminal) {
            final PackageManager pm = GetActivity()->GetPackageManager();
            pm->SetApplicationEnabledSetting(TERMINAL_APP_PACKAGE,
                    mEnableTerminal->IsChecked() ? PackageManager.COMPONENT_ENABLED_STATE_ENABLED
                            : PackageManager.COMPONENT_ENABLED_STATE_DEFAULT, 0);
        } else if (preference == mBugreportInPower) {
            Settings.Secure->PutInt(GetActivity()->GetContentResolver(),
                    Settings.Secure.BUGREPORT_IN_POWER_MENU,
                    mBugreportInPower->IsChecked() ? 1 : 0);
        } else if (preference == mKeepScreenOn) {
            Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
                    Settings::Global::STAY_ON_WHILE_PLUGGED_IN,
                    mKeepScreenOn->IsChecked() ?
                            (BatteryManager.BATTERY_PLUGGED_AC | BatteryManager.BATTERY_PLUGGED_USB) : 0);
        } else if (preference == mBtHciSnoopLog) {
            WriteBtHciSnoopLogOptions();
        } else if (preference == mEnableOemUnlock) {
            Utils->SetOemUnlockEnabled(GetActivity(), mEnableOemUnlock->IsChecked());
        } else if (preference == mAllowMockLocation) {
            Settings.Secure->PutInt(GetActivity()->GetContentResolver(),
                    Settings.Secure.ALLOW_MOCK_LOCATION,
                    mAllowMockLocation->IsChecked() ? 1 : 0);
        } else if (preference == mDebugViewAttributes) {
            Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
                    Settings::Global::DEBUG_VIEW_ATTRIBUTES,
                    mDebugViewAttributes->IsChecked() ? 1 : 0);
        } else if (preference == mDebugAppPref) {
            StartActivityForResult(new Intent(GetActivity(), AppPicker.class), RESULT_DEBUG_APP);
        } else if (preference == mWaitForDebugger) {
            WriteDebuggerOptions();
        } else if (preference == mVerifyAppsOverUsb) {
            WriteVerifyAppsOverUsbOptions();
        } else if (preference == mStrictMode) {
            WriteStrictModeVisualOptions();
        } else if (preference == mPointerLocation) {
            WritePointerLocationOptions();
        } else if (preference == mShowTouches) {
            WriteShowTouchesOptions();
        } else if (preference == mShowScreenUpdates) {
            WriteShowUpdatesOption();
        } else if (preference == mDisableOverlays) {
            WriteDisableOverlaysOption();
        } else if (preference == mShowCpuUsage) {
            WriteCpuUsageOptions();
        } else if (preference == mImmediatelyDestroyActivities) {
            WriteImmediatelyDestroyActivitiesOptions();
        } else if (preference == mShowAllANRs) {
            WriteShowAllANRsOptions();
        } else if (preference == mForceHardwareUi) {
            WriteHardwareUiOptions();
        } else if (preference == mForceMsaa) {
            WriteMsaaOptions();
        } else if (preference == mShowHwScreenUpdates) {
            WriteShowHwScreenUpdatesOptions();
        } else if (preference == mShowHwLayersUpdates) {
            WriteShowHwLayersUpdatesOptions();
        } else if (preference == mDebugLayout) {
            WriteDebugLayoutOptions();
        } else if (preference == mForceRtlLayout) {
            WriteForceRtlOptions();
        } else if (preference == mWifiDisplayCertification) {
            WriteWifiDisplayCertificationOptions();
        } else if (preference == mWifiVerboseLogging) {
            WriteWifiVerboseLoggingOptions();
        } else if (preference == mWifiAggressiveHandover) {
            WriteWifiAggressiveHandoverOptions();
        } else if (preference == mWifiAllowScansWithTraffic) {
            WriteWifiAllowScansWithTrafficOptions();
        } else if (preference == mUseNuplayer) {
            WriteUseNuplayerOptions();
        } else if (preference == mUSBAudio) {
            WriteUSBAudioOptions();
        } else {
            return super->OnPreferenceTreeClick(preferenceScreen, preference);
        }

        return FALSE;
    }

    //@Override
    public Boolean OnPreferenceChange(Preference preference, Object newValue) {
        if (HDCP_CHECKING_KEY->Equals(preference->GetKey())) {
            SystemProperties->Set(HDCP_CHECKING_PROPERTY, newValue->ToString());
            UpdateHdcpValues();
            PokeSystemProperties();
            return TRUE;
        } else if (preference == mLogdSize) {
            WriteLogdSizeOption(newValue);
            return TRUE;
        } else if (preference == mWindowAnimationScale) {
            WriteAnimationScaleOption(0, mWindowAnimationScale, newValue);
            return TRUE;
        } else if (preference == mTransitionAnimationScale) {
            WriteAnimationScaleOption(1, mTransitionAnimationScale, newValue);
            return TRUE;
        } else if (preference == mAnimatorDurationScale) {
            WriteAnimationScaleOption(2, mAnimatorDurationScale, newValue);
            return TRUE;
        } else if (preference == mOverlayDisplayDevices) {
            WriteOverlayDisplayDevicesOptions(newValue);
            return TRUE;
        } else if (preference == mOpenGLTraces) {
            WriteOpenGLTracesOptions(newValue);
            return TRUE;
        } else if (preference == mTrackFrameTime) {
            WriteTrackFrameTimeOptions(newValue);
            return TRUE;
        } else if (preference == mDebugHwOverdraw) {
            WriteDebugHwOverdrawOptions(newValue);
            return TRUE;
        } else if (preference == mShowNonRectClip) {
            WriteShowNonRectClipOptions(newValue);
            return TRUE;
        } else if (preference == mAppProcessLimit) {
            WriteAppProcessLimitOptions(newValue);
            return TRUE;
        } else if (preference == mSimulateColorSpace) {
            WriteSimulateColorSpace(newValue);
            return TRUE;
        }
        return FALSE;
    }

    private void DismissDialogs() {
        if (mAdbDialog != NULL) {
            mAdbDialog->Dismiss();
            mAdbDialog = NULL;
        }
        if (mAdbKeysDialog != NULL) {
            mAdbKeysDialog->Dismiss();
            mAdbKeysDialog = NULL;
        }
        if (mEnableDialog != NULL) {
            mEnableDialog->Dismiss();
            mEnableDialog = NULL;
        }
    }

    CARAPI OnClick(DialogInterface dialog, Int32 which) {
        if (dialog == mAdbDialog) {
            if (which == DialogInterface.BUTTON_POSITIVE) {
                mDialogClicked = TRUE;
                Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
                        Settings::Global::ADB_ENABLED, 1);
                mVerifyAppsOverUsb->SetEnabled(TRUE);
                UpdateVerifyAppsOverUsbOptions();
                UpdateBugreportOptions();
            } else {
                // Reset the toggle
                mEnableAdb->SetChecked(FALSE);
            }
        } else if (dialog == mAdbKeysDialog) {
            if (which == DialogInterface.BUTTON_POSITIVE) {
                try {
                    IBinder b = ServiceManager->GetService(Context.USB_SERVICE);
                    IUsbManager service = IUsbManager.Stub->AsInterface(b);
                    service->ClearUsbDebuggingKeys();
                } catch (RemoteException e) {
                    Logger::E(TAG, "Unable to clear adb keys", e);
                }
            }
        } else if (dialog == mEnableDialog) {
            if (which == DialogInterface.BUTTON_POSITIVE) {
                mDialogClicked = TRUE;
                Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
                        Settings::Global::DEVELOPMENT_SETTINGS_ENABLED, 1);
                mLastEnabledState = TRUE;
                SetPrefsEnabledState(mLastEnabledState);
            } else {
                // Reset the toggle
                mSwitchBar->SetChecked(FALSE);
            }
        }
    }

    CARAPI OnDismiss(DialogInterface dialog) {
        // Assuming that onClick gets called first
        if (dialog == mAdbDialog) {
            if (!mDialogClicked) {
                mEnableAdb->SetChecked(FALSE);
            }
            mAdbDialog = NULL;
        } else if (dialog == mEnableDialog) {
            if (!mDialogClicked) {
                mSwitchBar->SetChecked(FALSE);
            }
            mEnableDialog = NULL;
        }
    }

    //@Override
    CARAPI OnDestroy() {
        DismissDialogs();
        super->OnDestroy();
    }

    void PokeSystemProperties() {
        if (!mDontPokeProperties) {
            //noinspection unchecked
            (new SystemPropPoker()).Execute();
        }
    }

    static class SystemPropPoker extends AsyncTask<Void, Void, Void> {
        //@Override
        protected Void DoInBackground(Void... params) {
            String[] services;
            try {
                services = ServiceManager->ListServices();
            } catch (RemoteException e) {
                return NULL;
            }
            for (String service : services) {
                IBinder obj = ServiceManager->CheckService(service);
                if (obj != NULL) {
                    Parcel data = Parcel->Obtain();
                    try {
                        obj->Transact(IBinder.SYSPROPS_TRANSACTION, data, NULL, 0);
                    } catch (RemoteException e) {
                    } catch (Exception e) {
                        Logger::I(TAG, "Someone wrote a bad service '" + service
                                + "' that doesn't like to be poked: " + e);
                    }
                    data->Recycle();
                }
            }
            return NULL;
        }
    }

    private static Boolean IsPackageInstalled(Context context, String packageName) {
        try {
            return context->GetPackageManager()->GetPackageInfo(packageName, 0) != NULL;
        } catch (NameNotFoundException e) {
            return FALSE;
        }
    }

    /**
     * For Search.
     */
    public static const Indexable.SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
            new BaseSearchIndexProvider() {

                private Boolean IsShowingDeveloperOptions(Context context) {
                    return context->GetSharedPreferences(DevelopmentSettings.PREF_FILE,
                            Context.MODE_PRIVATE).GetBoolean(
                                    DevelopmentSettings.PREF_SHOW,
                                    android.os.Build.TYPE->Equals("eng"));
                }

                //@Override
                public List<SearchIndexableResource> GetXmlResourcesToIndex(
                        Context context, Boolean enabled) {

                    if (!IsShowingDeveloperOptions(context)) {
                        return NULL;
                    }

                    final SearchIndexableResource sir = new SearchIndexableResource(context);
                    sir.xmlResId = R.xml.development_prefs;
                    return Arrays->AsList(sir);
                }

                //@Override
                public List<String> GetNonIndexableKeys(Context context) {
                    if (!IsShowingDeveloperOptions(context)) {
                        return NULL;
                    }

                    final List<String> keys = new ArrayList<String>();
                    if (!ShowEnableOemUnlockPreference()) {
                        keys->Add(ENABLE_OEM_UNLOCK);
                    }
                    return keys;
                }
            };
}
