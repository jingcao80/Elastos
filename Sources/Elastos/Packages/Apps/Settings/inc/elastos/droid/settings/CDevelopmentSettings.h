
#ifndef __ELASTOS_DROID_SETTINGS_CDEVELOPMENTSETTINGS_H__
#define __ELASTOS_DROID_SETTINGS_CDEVELOPMENTSETTINGS_H__

#include "Elastos.Droid.Wifi.h"
#include "_Elastos_Droid_Settings_CDevelopmentSettings.h"
#include "elastos/droid/settings/SettingsPreferenceFragment.h"
#include "elastos/droid/settings/search/BaseSearchIndexProvider.h"
#include "elastos/droid/os/AsyncTask.h"

using Elastos::Droid::Settings::Search::BaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::IIndexableSearchIndexProvider;
using Elastos::Droid::Settings::Widget::ISwitchBar;
using Elastos::Droid::Settings::Widget::ISwitchBarOnSwitchChangeListener;

using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::Admin::IDevicePolicyManager;
using Elastos::Droid::App::Backup::IIBackupManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Wifi::IWifiManager;

// using dalvik::System::IVMRuntime;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Settings {

/*
 * Displays preferences for application developers.
 */
CarClass(CDevelopmentSettings)
    , public SettingsPreferenceFragment
    , public IDialogInterfaceOnClickListener
    , public IIndexable
{
public:
    class InnerListener
        : public Object
        , public IDialogInterfaceOnDismissListener
        , public IPreferenceOnPreferenceChangeListener
        , public ISwitchBarOnSwitchChangeListener
    {
    public:
        TO_STRING_IMPL("CDevelopmentSettings::InnerListener")

        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CDevelopmentSettings* host);

        //@Override
        CARAPI OnDismiss(
            /* [in] */ IDialogInterface* dialog);

        //@Override
        CARAPI OnPreferenceChange(
            /* [in] */ IPreference* preference,
            /* [in] */ IInterface* newValue,
            /* [out] */ Boolean* result);

        //@Override
        CARAPI OnSwitchChanged(
            /* [in] */ ISwitch* switchView,
            /* [in] */ Boolean isChecked);

    private:
        CDevelopmentSettings* mHost;
    };

    class SystemPropPoker
        // AsyncTask<Void, Void, Void>
        : public AsyncTask
    {
    public:
        TO_STRING_IMPL("CDevelopmentSettings::SystemPropPoker")

        SystemPropPoker();

        ~SystemPropPoker();

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);
    };

    /**
     * For search.
     */
    class MyBaseSearchIndexProvider
        : public BaseSearchIndexProvider
    {
    public:
        TO_STRING_IMPL("CDeviceInfoSettings::MyBaseSearchIndexProvider")

        MyBaseSearchIndexProvider();

        ~MyBaseSearchIndexProvider();

        //@Override
        CARAPI GetXmlResourcesToIndex(
            /* [in] */ IContext* context,
            /* [in] */ Boolean enabled,
            /* [out] */ IList** result);

        //@Override
        CARAPI GetNonIndexableKeys(
            /* [in] */ IContext* context,
            /* [out] */ IList** result);

    private:
        CARAPI_(Boolean) IsShowingDeveloperOptions(
            /* [in] */ IContext* context);
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CDevelopmentSettings();

    ~CDevelopmentSettings();

    CARAPI constructor();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnActivityCreated(
        /* [in] */ IBundle* savedInstanceState);

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnDestroyView();


    //@Override
    CARAPI OnSwitchChanged(
        /* [in] */ ISwitch* switchView,
        /* [in] */ Boolean isChecked);

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ IPreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* preference,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPreferenceChange(
        /* [in] */ IPreference* preference,
        /* [in] */ IInterface* newValue,
        /* [out] */ Boolean* result);

    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    CARAPI OnDismiss(
        /* [in] */ IDialogInterface* dialog);

    //@Override
    CARAPI OnDestroy();

    static CARAPI_(AutoPtr<IIndexableSearchIndexProvider>) GetSEARCH_INDEX_DATA_PROVIDER();

protected:
    CARAPI_(void) UpdateCheckBox(
        /* [in] */ ICheckBoxPreference* checkBox,
        /* [in] */ Boolean value);

    CARAPI_(void) PokeSystemProperties();

private:
    CARAPI_(AutoPtr<IListPreference>) AddListPreference(
        /* [in] */ const String& prefKey);

    CARAPI_(void) DisableForUser(
        /* [in] */ IPreference* pref);

    CARAPI FindAndInitCheckboxPref(
        /* [in] */ const String& key,
        /* [out] */ ICheckBoxPreference** result);

    CARAPI_(Boolean) RemovePreferenceForProduction(
        /* [in] */ IPreference* preference);

    CARAPI_(void) RemovePreference(
        /* [in] */ IPreference* preference);

    CARAPI_(void) SetPrefsEnabledState(
        /* [in] */ Boolean enabled);


    CARAPI_(void) UpdateAllOptions();

    CARAPI_(void) ResetDangerousOptions();

    CARAPI_(void) UpdateHdcpValues();

    CARAPI_(void) UpdatePasswordSummary();

    CARAPI_(void) WriteBtHciSnoopLogOptions();

    CARAPI_(void) WriteDebuggerOptions();

    static CARAPI_(void) ResetDebuggerOptions();

    CARAPI_(void) UpdateDebuggerOptions();

    CARAPI_(void) UpdateVerifyAppsOverUsbOptions();

    CARAPI_(void) WriteVerifyAppsOverUsbOptions();

    CARAPI_(Boolean) EnableVerifierSetting();

    CARAPI_(Boolean) ShowVerifierSetting();

    static CARAPI_(Boolean) ShowEnableOemUnlockPreference();

    CARAPI_(void) UpdateBugreportOptions();

    // Returns the current state of the system property that controls
    // strictmode flashes.  One of:
    //    0: not explicitly set one way or another
    //    1: on
    //    2: off
    static CARAPI_(Int32) CurrentStrictModeActiveIndex();

    CARAPI_(void) WriteStrictModeVisualOptions();

    CARAPI_(void) UpdateStrictModeVisualOptions();

    CARAPI_(void) WritePointerLocationOptions();

    CARAPI_(void) UpdatePointerLocationOptions();

    CARAPI_(void) WriteShowTouchesOptions();

    CARAPI_(void) UpdateShowTouchesOptions();

    CARAPI_(void) UpdateFlingerOptions();

    CARAPI_(void) WriteShowUpdatesOption();

    CARAPI_(void) WriteDisableOverlaysOption();

    CARAPI_(void) UpdateHardwareUiOptions();

    CARAPI_(void) WriteHardwareUiOptions();

    CARAPI_(void) UpdateMsaaOptions();

    CARAPI_(void) WriteMsaaOptions();

    CARAPI_(void) UpdateTrackFrameTimeOptions();

    CARAPI_(void) WriteTrackFrameTimeOptions(
        /* [in] */ IInterface* newValue);

    CARAPI_(void) UpdateShowNonRectClipOptions();

    CARAPI_(void) WriteShowNonRectClipOptions(
        /* [in] */ IInterface* newValue);

    CARAPI_(void) UpdateShowHwScreenUpdatesOptions();

    CARAPI_(void) WriteShowHwScreenUpdatesOptions();

    CARAPI_(void) UpdateShowHwLayersUpdatesOptions();

    CARAPI_(void) WriteShowHwLayersUpdatesOptions();

    CARAPI_(void) UpdateDebugHwOverdrawOptions();

    CARAPI_(void) WriteDebugHwOverdrawOptions(
        /* [in] */ IInterface* newValue);

    CARAPI_(void) UpdateDebugLayoutOptions();

    CARAPI_(void) WriteDebugLayoutOptions();

    CARAPI_(void) UpdateSimulateColorSpace();

    /**
     * @return <code>TRUE</code> if the color space preference is currently
     *         controlled by development settings
     */
    CARAPI_(Boolean) UsingDevelopmentColorSpace();

    CARAPI_(void) WriteSimulateColorSpace(
        /* [in] */ IInterface* value);

    CARAPI_(void) UpdateUseNuplayerOptions();

    CARAPI_(void) WriteUseNuplayerOptions();

    CARAPI_(void) UpdateUSBAudioOptions();

    CARAPI_(void) WriteUSBAudioOptions();

    CARAPI_(void) UpdateForceRtlOptions();

    CARAPI_(void) WriteForceRtlOptions();

    CARAPI_(void) UpdateWifiDisplayCertificationOptions();

    CARAPI_(void) WriteWifiDisplayCertificationOptions();

    CARAPI_(void) UpdateWifiVerboseLoggingOptions();

    CARAPI_(void) WriteWifiVerboseLoggingOptions();

    CARAPI_(void) UpdateWifiAggressiveHandoverOptions();

    CARAPI_(void) WriteWifiAggressiveHandoverOptions();

    CARAPI_(void) UpdateWifiAllowScansWithTrafficOptions();

    CARAPI_(void) WriteWifiAllowScansWithTrafficOptions();

    CARAPI_(void) UpdateLogdSizeValues();

    CARAPI_(void) WriteLogdSizeOption(
        /* [in] */ IInterface* newValue);

    CARAPI_(void) UpdateCpuUsageOptions();

    CARAPI_(void) WriteCpuUsageOptions();

    CARAPI_(void) WriteImmediatelyDestroyActivitiesOptions();

    CARAPI_(void) UpdateImmediatelyDestroyActivitiesOptions();

    CARAPI_(void) UpdateAnimationScaleValue(
        /* [in] */ Int32 which,
        /* [in] */ IListPreference* pref);

    CARAPI_(void) UpdateAnimationScaleOptions();

    CARAPI_(void) WriteAnimationScaleOption(
        /* [in] */ Int32 which,
        /* [in] */ IListPreference* pref,
        /* [in] */ IInterface* newValue);

    CARAPI_(void) UpdateOverlayDisplayDevicesOptions();

    CARAPI_(void) WriteOverlayDisplayDevicesOptions(
        /* [in] */ IInterface* newValue);

    CARAPI_(void) UpdateOpenGLTracesOptions();

    CARAPI_(void) WriteOpenGLTracesOptions(
        /* [in] */ IInterface* newValue);

    CARAPI_(void) UpdateAppProcessLimitOptions();

    CARAPI_(void) WriteAppProcessLimitOptions(
        /* [in] */ IInterface* newValue);

    CARAPI_(void) WriteShowAllANRsOptions();

    CARAPI_(void) UpdateShowAllANRsOptions();

    CARAPI_(void) DismissDialogs();

    static CARAPI_(Boolean) IsPackageInstalled(
        /* [in] */ IContext* context,
        /* [in] */ const String& packageName);

public:
    /**
     * Preference file were development settings prefs are stored.
     */
    static const String PREF_FILE;

    /**
     * Whether to show the development settings to the user.  Default is FALSE.
     */
    static const String PREF_SHOW;

private:
    static AutoPtr<IIndexableSearchIndexProvider> SEARCH_INDEX_DATA_PROVIDER;

    static const String TAG;// = "DevelopmentSettings";

    static const String ENABLE_ADB;// = "enable_adb";
    static const String CLEAR_ADB_KEYS;// = "clear_adb_keys";
    static const String ENABLE_TERMINAL;// = "enable_terminal";
    static const String KEEP_SCREEN_ON;// = "keep_screen_on";
    static const String BT_HCI_SNOOP_LOG;// = "bt_hci_snoop_log";
    static const String ENABLE_OEM_UNLOCK;// = "oem_unlock_enable";
    static const String ALLOW_MOCK_LOCATION;// = "allow_mock_location";
    static const String HDCP_CHECKING_KEY;// = "hdcp_checking";
    static const String HDCP_CHECKING_PROPERTY;// = "persist.sys.hdcp_checking";
    static const String LOCAL_BACKUP_PASSWORD;// = "local_backup_password";
    static const String HARDWARE_UI_PROPERTY;// = "persist.sys.ui.hw";
    static const String MSAA_PROPERTY;// = "debug.egl.force_msaa";
    static const String BUGREPORT;// = "bugreport";
    static const String BUGREPORT_IN_POWER_KEY;// = "bugreport_in_power";
    static const String OPENGL_TRACES_PROPERTY;// = "debug.egl.trace";

    static const String DEBUG_APP_KEY;// = "debug_app";
    static const String WAIT_FOR_DEBUGGER_KEY;// = "wait_for_debugger";
    static const String VERIFY_APPS_OVER_USB_KEY;// = "verify_apps_over_usb";
    static const String DEBUG_VIEW_ATTRIBUTES;// = "debug_view_attributes";
    static const String STRICT_MODE_KEY;// = "strict_mode";
    static const String POINTER_LOCATION_KEY;// = "pointer_location";
    static const String SHOW_TOUCHES_KEY;// = "show_touches";
    static const String SHOW_SCREEN_UPDATES_KEY;// = "show_screen_updates";
    static const String DISABLE_OVERLAYS_KEY;// = "disable_overlays";
    static const String SIMULATE_COLOR_SPACE;// = "simulate_color_space";
    static const String USE_NUPLAYER_KEY;// = "use_nuplayer";
    static const String USB_AUDIO_KEY;// = "usb_audio";
    static const String USE_AWESOMEPLAYER_PROPERTY;// = "persist.sys.media.use-awesome";
    static const String SHOW_CPU_USAGE_KEY;// = "show_cpu_usage";
    static const String FORCE_HARDWARE_UI_KEY;// = "force_hw_ui";
    static const String FORCE_MSAA_KEY;// = "force_msaa";
    static const String TRACK_FRAME_TIME_KEY;// = "track_frame_time";
    static const String SHOW_NON_RECTANGULAR_CLIP_KEY;// = "show_non_rect_clip";
    static const String SHOW_HW_SCREEN_UPDATES_KEY;// = "show_hw_screen_udpates";
    static const String SHOW_HW_LAYERS_UPDATES_KEY;// = "show_hw_layers_udpates";
    static const String DEBUG_HW_OVERDRAW_KEY;// = "debug_hw_overdraw";
    static const String DEBUG_LAYOUT_KEY;// = "debug_layout";
    static const String FORCE_RTL_LAYOUT_KEY;// = "force_rtl_layout_all_locales";
    static const String WINDOW_ANIMATION_SCALE_KEY;// = "window_animation_scale";
    static const String TRANSITION_ANIMATION_SCALE_KEY;// = "transition_animation_scale";
    static const String ANIMATOR_DURATION_SCALE_KEY;// = "animator_duration_scale";
    static const String OVERLAY_DISPLAY_DEVICES_KEY;// = "overlay_display_devices";
    static const String DEBUG_DEBUGGING_CATEGORY_KEY;// = "debug_debugging_category";
    static const String DEBUG_APPLICATIONS_CATEGORY_KEY;// = "debug_applications_category";
    static const String WIFI_DISPLAY_CERTIFICATION_KEY;// = "wifi_display_certification";
    static const String WIFI_VERBOSE_LOGGING_KEY;// = "wifi_verbose_logging";
    static const String WIFI_AGGRESSIVE_HANDOVER_KEY;// = "wifi_aggressive_handover";
    static const String WIFI_ALLOW_SCAN_WITH_TRAFFIC_KEY;// = "wifi_allow_scan_with_traffic";
    static const String SELECT_LOGD_SIZE_KEY;// = "select_logd_size";
    static const String SELECT_LOGD_SIZE_PROPERTY;// = "persist.logd.size";
    static const String SELECT_LOGD_DEFAULT_SIZE_PROPERTY;// = "ro.logd.size";

    static const String OPENGL_TRACES_KEY;// = "enable_opengl_traces";

    static const String IMMEDIATELY_DESTROY_ACTIVITIES_KEY;// = "immediately_destroy_activities";

    static const String APP_PROCESS_LIMIT_KEY;// = "app_process_limit";

    static const String SHOW_ALL_ANRS_KEY;// = "show_all_anrs";

    static const String PROCESS_STATS;// = "proc_stats";

    static const String TAG_CONFIRM_ENFORCE;// = "confirm_enforce";

    static const String PACKAGE_MIME_TYPE;// = "application/vnd.android.package-archive";

    static const String TERMINAL_APP_PACKAGE;// = "com.android.terminal";

    static const Int32 RESULT_DEBUG_APP;// = 1000;

    static const String PERSISTENT_DATA_BLOCK_PROP;// = "ro.frp.pst";

    static String DEFAULT_LOG_RING_BUFFER_SIZE_IN_BYTES;// = "262144"; // 256K

    AutoPtr<IIWindowManager> mWindowManager;
    AutoPtr<IIBackupManager> mBackupManager;
    AutoPtr<IDevicePolicyManager> mDpm;
    AutoPtr<IUserManager> mUm;
    AutoPtr<IWifiManager> mWifiManager;

    AutoPtr<ISwitchBar> mSwitchBar;
    Boolean mLastEnabledState;
    Boolean mHaveDebugSettings;
    Boolean mDontPokeProperties;

    AutoPtr<ICheckBoxPreference> mEnableAdb;
    AutoPtr<IPreference> mClearAdbKeys;
    AutoPtr<ICheckBoxPreference> mEnableTerminal;
    AutoPtr<IPreference> mBugreport;
    AutoPtr<ICheckBoxPreference> mBugreportInPower;
    AutoPtr<ICheckBoxPreference> mKeepScreenOn;
    AutoPtr<ICheckBoxPreference> mBtHciSnoopLog;
    AutoPtr<ICheckBoxPreference> mEnableOemUnlock;
    AutoPtr<ICheckBoxPreference> mAllowMockLocation;
    AutoPtr<ICheckBoxPreference> mDebugViewAttributes;

    AutoPtr<IPreferenceScreen> mPassword;
    String mDebugApp;
    AutoPtr<IPreference> mDebugAppPref;
    AutoPtr<ICheckBoxPreference> mWaitForDebugger;
    AutoPtr<ICheckBoxPreference> mVerifyAppsOverUsb;
    AutoPtr<ICheckBoxPreference> mWifiDisplayCertification;
    AutoPtr<ICheckBoxPreference> mWifiVerboseLogging;
    AutoPtr<ICheckBoxPreference> mWifiAggressiveHandover;

    AutoPtr<ICheckBoxPreference> mWifiAllowScansWithTraffic;
    AutoPtr<ICheckBoxPreference> mStrictMode;
    AutoPtr<ICheckBoxPreference> mPointerLocation;
    AutoPtr<ICheckBoxPreference> mShowTouches;
    AutoPtr<ICheckBoxPreference> mShowScreenUpdates;
    AutoPtr<ICheckBoxPreference> mDisableOverlays;
    AutoPtr<ICheckBoxPreference> mShowCpuUsage;
    AutoPtr<ICheckBoxPreference> mForceHardwareUi;
    AutoPtr<ICheckBoxPreference> mForceMsaa;
    AutoPtr<ICheckBoxPreference> mShowHwScreenUpdates;
    AutoPtr<ICheckBoxPreference> mShowHwLayersUpdates;
    AutoPtr<ICheckBoxPreference> mDebugLayout;
    AutoPtr<ICheckBoxPreference> mForceRtlLayout;
    AutoPtr<IListPreference> mDebugHwOverdraw;
    AutoPtr<IListPreference> mLogdSize;
    AutoPtr<IListPreference> mTrackFrameTime;
    AutoPtr<IListPreference> mShowNonRectClip;
    AutoPtr<IListPreference> mWindowAnimationScale;
    AutoPtr<IListPreference> mTransitionAnimationScale;
    AutoPtr<IListPreference> mAnimatorDurationScale;
    AutoPtr<IListPreference> mOverlayDisplayDevices;
    AutoPtr<IListPreference> mOpenGLTraces;

    AutoPtr<IListPreference> mSimulateColorSpace;

    AutoPtr<ICheckBoxPreference> mUseNuplayer;
    AutoPtr<ICheckBoxPreference> mUSBAudio;
    AutoPtr<ICheckBoxPreference> mImmediatelyDestroyActivities;

    AutoPtr<IListPreference> mAppProcessLimit;

    AutoPtr<ICheckBoxPreference> mShowAllANRs;

    AutoPtr<IPreferenceScreen> mProcessStats;
    // final ArrayList<Preference> mAllPrefs = new ArrayList<Preference>();
    AutoPtr<IArrayList> mAllPrefs;

    // final ArrayList<CheckBoxPreference> mResetCbPrefs
    //         = new ArrayList<CheckBoxPreference>();
    AutoPtr<IArrayList> mResetCbPrefs;

    // final HashSet<Preference> mDisabledPrefs = new HashSet<Preference>();
    AutoPtr<IHashSet> mDisabledPrefs;
    // To track whether a confirmation dialog was clicked.
    Boolean mDialogClicked;
    AutoPtr<IDialog> mEnableDialog;
    AutoPtr<IDialog> mAdbDialog;

    AutoPtr<IDialog> mAdbKeysDialog;
    Boolean mUnavailable;
    AutoPtr<InnerListener> mListener;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CDEVELOPMENTSETTINGS_H__