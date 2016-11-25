
#include "Elastos.Droid.Bluetooth.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/droid/settings/CDevelopmentSettings.h"
#include "elastos/droid/settings/CSettingsActivity.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/settings/widget/CSwitchBar.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Settings::Search::EIID_IIndexable;
using Elastos::Droid::Settings::Widget::EIID_ISwitchBarOnSwitchChangeListener;
using Elastos::Droid::Settings::Widget::CSwitchBar;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::App::CAlertDialogBuilder;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothAdapterHelper;
using Elastos::Droid::Bluetooth::CBluetoothAdapterHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::EIID_IDialogInterfaceOnDismissListener;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Hardware::Usb::IIUsbManager;
using Elastos::Droid::Internal::App::CLocalePickerHelper;
using Elastos::Droid::Internal::App::ILocalePickerHelper;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
// using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IStrictMode;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::IBatteryManager;
using Elastos::Droid::Preference::CPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::ITwoStatePreference;
using Elastos::Droid::Preference::EIID_IPreferenceOnPreferenceChangeListener;
using Elastos::Droid::Provider::CSearchIndexableResource;
using Elastos::Droid::Provider::ISearchIndexableResource;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsSecure;
using Elastos::Droid::Provider::ISettingsSecure;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IHardwareRenderer;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Arrays;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {

//===============================================================================
//                  CDevelopmentSettings::InnerListener
//===============================================================================

CAR_INTERFACE_IMPL_3(CDevelopmentSettings::InnerListener, Object, IDialogInterfaceOnDismissListener,
        IPreferenceOnPreferenceChangeListener, ISwitchBarOnSwitchChangeListener)

CDevelopmentSettings::InnerListener::InnerListener(
    /* [in] */ CDevelopmentSettings* host)
    : mHost(host)
{}

ECode CDevelopmentSettings::InnerListener::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    return mHost->OnDismiss(dialog);
}

ECode CDevelopmentSettings::InnerListener::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    return mHost->OnPreferenceChange(preference, newValue, result);
}

ECode CDevelopmentSettings::InnerListener::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    return mHost->OnSwitchChanged(switchView, isChecked);
}

//===============================================================================
//                  CDevelopmentSettings::SystemPropPoker
//===============================================================================

CDevelopmentSettings::SystemPropPoker::SystemPropPoker()
{}

CDevelopmentSettings::SystemPropPoker::~SystemPropPoker()
{}

ECode CDevelopmentSettings::SystemPropPoker::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr< ArrayOf<String> > services;
    // try {
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    ECode ec = sm->ListServices((ArrayOf<String>**)&services);
    if (ec == (ECode)E_REMOTE_EXCEPTION) {
        return NOERROR;
    }

    for (Int32 i = 0; i < services->GetLength(); ++i) {
        String service = (*services)[i];

        AutoPtr<IInterface> serObj;
        sm->CheckService(service, (IInterface**)&serObj);
        IBinder* obj = IBinder::Probe(serObj);
        if (obj != NULL) {
            AutoPtr<IParcel> data;
            CParcel::New((IParcel**)&data);
            assert(0 && "TODO");
            // Parcel data = Parcel->Obtain();
            // try {
            //     obj->Transact(IBinder::SYSPROPS_TRANSACTION, data, NULL, 0);
            // } catch (RemoteException e) {
            // } catch (Exception e) {
            //     Logger::I(TAG, "Someone wrote a bad service '" + service
            //             + "' that doesn't like to be poked: " + e);
            // }
            // data->Recycle();
        }
    }
    return NOERROR;
}

//===============================================================================
//                  CDevelopmentSettings::MyBaseSearchIndexProvider
//===============================================================================

CDevelopmentSettings::MyBaseSearchIndexProvider::MyBaseSearchIndexProvider()
{}

CDevelopmentSettings::MyBaseSearchIndexProvider::~MyBaseSearchIndexProvider()
{}

Boolean CDevelopmentSettings::MyBaseSearchIndexProvider::IsShowingDeveloperOptions(
    /* [in] */ IContext* context)
{
    AutoPtr<ISharedPreferences> sp;
    context->GetSharedPreferences(CDevelopmentSettings::PREF_FILE,
            IContext::MODE_PRIVATE, (ISharedPreferences**)&sp);
    Boolean res;
    sp->GetBoolean(CDevelopmentSettings::PREF_SHOW, Build::TYPE.Equals("eng"), &res);
    return res;
}

ECode CDevelopmentSettings::MyBaseSearchIndexProvider::GetXmlResourcesToIndex(
    /* [in] */ IContext* context,
    /* [in] */ Boolean enabled,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (!IsShowingDeveloperOptions(context)) {
        return NOERROR;
    }

    AutoPtr<ISearchIndexableResource> sir;
    CSearchIndexableResource::New(context, (ISearchIndexableResource**)&sir);
    sir->SetXmlResId(R::xml::development_prefs);

    AutoPtr< ArrayOf<ISearchIndexableResource*> > args = ArrayOf<ISearchIndexableResource*>::Alloc(1);
    args->Set(0, sir);

    return Arrays::AsList(args, result);
}

ECode CDevelopmentSettings::MyBaseSearchIndexProvider::GetNonIndexableKeys(
    /* [in] */ IContext* context,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (!IsShowingDeveloperOptions(context)) {
        return NOERROR;
    }

    AutoPtr<IList> keys;
    CArrayList::New((IList**)&keys);
    if (!ShowEnableOemUnlockPreference()) {
        keys->Add(CoreUtils::Convert(ENABLE_OEM_UNLOCK));
    }
    *result = keys;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

//===============================================================================
//                 CDevelopmentSettings
//===============================================================================

const String CDevelopmentSettings::PREF_FILE("development");
const String CDevelopmentSettings::PREF_SHOW("show");

AutoPtr<IIndexableSearchIndexProvider> CDevelopmentSettings::SEARCH_INDEX_DATA_PROVIDER;

const String CDevelopmentSettings::TAG("DevelopmentSettings");

const String CDevelopmentSettings::ENABLE_ADB("enable_adb");
const String CDevelopmentSettings::CLEAR_ADB_KEYS("clear_adb_keys");
const String CDevelopmentSettings::ENABLE_TERMINAL("enable_terminal");
const String CDevelopmentSettings::KEEP_SCREEN_ON("keep_screen_on");
const String CDevelopmentSettings::BT_HCI_SNOOP_LOG("bt_hci_snoop_log");
const String CDevelopmentSettings::ENABLE_OEM_UNLOCK("oem_unlock_enable");
const String CDevelopmentSettings::ALLOW_MOCK_LOCATION("allow_mock_location");
const String CDevelopmentSettings::HDCP_CHECKING_KEY("hdcp_checking");
const String CDevelopmentSettings::HDCP_CHECKING_PROPERTY("persist.sys.hdcp_checking");
const String CDevelopmentSettings::LOCAL_BACKUP_PASSWORD("local_backup_password");
const String CDevelopmentSettings::HARDWARE_UI_PROPERTY("persist.sys.ui.hw");
const String CDevelopmentSettings::MSAA_PROPERTY("debug.egl.force_msaa");
const String CDevelopmentSettings::BUGREPORT("bugreport");
const String CDevelopmentSettings::BUGREPORT_IN_POWER_KEY("bugreport_in_power");
const String CDevelopmentSettings::OPENGL_TRACES_PROPERTY("debug.egl.trace");

const String CDevelopmentSettings::DEBUG_APP_KEY("debug_app");
const String CDevelopmentSettings::WAIT_FOR_DEBUGGER_KEY("wait_for_debugger");
const String CDevelopmentSettings::VERIFY_APPS_OVER_USB_KEY("verify_apps_over_usb");
const String CDevelopmentSettings::DEBUG_VIEW_ATTRIBUTES("debug_view_attributes");
const String CDevelopmentSettings::STRICT_MODE_KEY("strict_mode");
const String CDevelopmentSettings::POINTER_LOCATION_KEY("pointer_location");
const String CDevelopmentSettings::SHOW_TOUCHES_KEY("show_touches");
const String CDevelopmentSettings::SHOW_SCREEN_UPDATES_KEY("show_screen_updates");
const String CDevelopmentSettings::DISABLE_OVERLAYS_KEY("disable_overlays");
const String CDevelopmentSettings::SIMULATE_COLOR_SPACE("simulate_color_space");
const String CDevelopmentSettings::USE_NUPLAYER_KEY("use_nuplayer");
const String CDevelopmentSettings::USB_AUDIO_KEY("usb_audio");
const String CDevelopmentSettings::USE_AWESOMEPLAYER_PROPERTY("persist.sys.media.use-awesome");
const String CDevelopmentSettings::SHOW_CPU_USAGE_KEY("show_cpu_usage");
const String CDevelopmentSettings::FORCE_HARDWARE_UI_KEY("force_hw_ui");
const String CDevelopmentSettings::FORCE_MSAA_KEY("force_msaa");
const String CDevelopmentSettings::TRACK_FRAME_TIME_KEY("track_frame_time");
const String CDevelopmentSettings::SHOW_NON_RECTANGULAR_CLIP_KEY("show_non_rect_clip");
const String CDevelopmentSettings::SHOW_HW_SCREEN_UPDATES_KEY("show_hw_screen_udpates");
const String CDevelopmentSettings::SHOW_HW_LAYERS_UPDATES_KEY("show_hw_layers_udpates");
const String CDevelopmentSettings::DEBUG_HW_OVERDRAW_KEY("debug_hw_overdraw");
const String CDevelopmentSettings::DEBUG_LAYOUT_KEY("debug_layout");
const String CDevelopmentSettings::FORCE_RTL_LAYOUT_KEY("force_rtl_layout_all_locales");
const String CDevelopmentSettings::WINDOW_ANIMATION_SCALE_KEY("window_animation_scale");
const String CDevelopmentSettings::TRANSITION_ANIMATION_SCALE_KEY("transition_animation_scale");
const String CDevelopmentSettings::ANIMATOR_DURATION_SCALE_KEY("animator_duration_scale");
const String CDevelopmentSettings::OVERLAY_DISPLAY_DEVICES_KEY("overlay_display_devices");
const String CDevelopmentSettings::DEBUG_DEBUGGING_CATEGORY_KEY("debug_debugging_category");
const String CDevelopmentSettings::DEBUG_APPLICATIONS_CATEGORY_KEY("debug_applications_category");
const String CDevelopmentSettings::WIFI_DISPLAY_CERTIFICATION_KEY("wifi_display_certification");
const String CDevelopmentSettings::WIFI_VERBOSE_LOGGING_KEY("wifi_verbose_logging");
const String CDevelopmentSettings::WIFI_AGGRESSIVE_HANDOVER_KEY("wifi_aggressive_handover");
const String CDevelopmentSettings::WIFI_ALLOW_SCAN_WITH_TRAFFIC_KEY("wifi_allow_scan_with_traffic");
const String CDevelopmentSettings::SELECT_LOGD_SIZE_KEY("select_logd_size");
const String CDevelopmentSettings::SELECT_LOGD_SIZE_PROPERTY("persist.logd.size");
const String CDevelopmentSettings::SELECT_LOGD_DEFAULT_SIZE_PROPERTY("ro.logd.size");

const String CDevelopmentSettings::OPENGL_TRACES_KEY("enable_opengl_traces");

const String CDevelopmentSettings::IMMEDIATELY_DESTROY_ACTIVITIES_KEY("immediately_destroy_activities");
const String CDevelopmentSettings::APP_PROCESS_LIMIT_KEY("app_process_limit");

const String CDevelopmentSettings::SHOW_ALL_ANRS_KEY("show_all_anrs");

const String CDevelopmentSettings::PROCESS_STATS("proc_stats");

const String CDevelopmentSettings::TAG_CONFIRM_ENFORCE("confirm_enforce");

const String CDevelopmentSettings::PACKAGE_MIME_TYPE("application/vnd.android.package-archive");

const String CDevelopmentSettings::TERMINAL_APP_PACKAGE("com.android.terminal");

const Int32 CDevelopmentSettings::RESULT_DEBUG_APP = 1000;

const String CDevelopmentSettings::PERSISTENT_DATA_BLOCK_PROP("ro.frp.pst");

String CDevelopmentSettings::DEFAULT_LOG_RING_BUFFER_SIZE_IN_BYTES("262144"); // 256K

AutoPtr<IIndexableSearchIndexProvider> CDevelopmentSettings::GetSEARCH_INDEX_DATA_PROVIDER()
{
    if (SEARCH_INDEX_DATA_PROVIDER == NULL) {
        SEARCH_INDEX_DATA_PROVIDER = new CDevelopmentSettings::MyBaseSearchIndexProvider();
    }

    return SEARCH_INDEX_DATA_PROVIDER;
}

CAR_INTERFACE_IMPL_2(CDevelopmentSettings, SettingsPreferenceFragment, IDialogInterfaceOnClickListener, IIndexable)

CAR_OBJECT_IMPL(CDevelopmentSettings)

CDevelopmentSettings::CDevelopmentSettings()
    : mLastEnabledState(FALSE)
    , mHaveDebugSettings(FALSE)
    , mDontPokeProperties(FALSE)
    , mDialogClicked(FALSE)
    , mUnavailable(FALSE)
{}

CDevelopmentSettings::~CDevelopmentSettings()
{}

ECode CDevelopmentSettings::constructor()
{
    SettingsPreferenceFragment::constructor();

    CArrayList::New((IArrayList**)&mAllPrefs);
    CArrayList::New((IArrayList**)&mResetCbPrefs);

    CHashSet::New((IHashSet**)&mDisabledPrefs);

    mListener = new InnerListener(this);

    return NOERROR;
}

ECode CDevelopmentSettings::OnCreate(
    /* [in] */ IBundle* icicle)
{
    SettingsPreferenceFragment::OnCreate(icicle);

    mWindowManager = IIWindowManager::Probe(ServiceManager::GetService(String("window")));
    mBackupManager = IIBackupManager::Probe(
            ServiceManager::GetService(IContext::BACKUP_SERVICE));
    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    AutoPtr<IInterface> obj;
    activity->GetSystemService(IContext::DEVICE_POLICY_SERVICE, (IInterface**)&obj);
    mDpm = IDevicePolicyManager::Probe(obj);
    mUm = IUserManager::Probe(GetSystemService(IContext::USER_SERVICE));

    mWifiManager = IWifiManager::Probe(GetSystemService(IContext::WIFI_SERVICE));

    AutoPtr<IUserHandle> uh;
    Process::MyUserHandle((IUserHandle**)&uh);
    Int32 id;
    uh->GetIdentifier(&id);
    Boolean res;
    if (id != IUserHandle::USER_OWNER
            || (mUm->HasUserRestriction(IUserManager::DISALLOW_DEBUGGING_FEATURES, &res), res)) {
        mUnavailable = TRUE;
        AutoPtr<IPreferenceScreen> screen;
        CPreferenceScreen::New(activity, NULL, (IPreferenceScreen**)&screen);
        SetPreferenceScreen(screen);
        return NOERROR;
    }

    AddPreferencesFromResource(R::xml::development_prefs);

    AutoPtr<IPreference> pref;
    FindPreference(CoreUtils::Convert(DEBUG_DEBUGGING_CATEGORY_KEY), (IPreference**)&pref);
    IPreferenceGroup* debugDebuggingCategory = IPreferenceGroup::Probe(pref);

    FindAndInitCheckboxPref(ENABLE_ADB, (ICheckBoxPreference**)&mEnableAdb);
    FindPreference(CoreUtils::Convert(CLEAR_ADB_KEYS), (IPreference**)&mClearAdbKeys);

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->GetBoolean(String("ro.adb.secure"), FALSE, &res);
    if (!res) {
        if (debugDebuggingCategory != NULL) {
            debugDebuggingCategory->RemovePreference(mClearAdbKeys, &res);
        }
    }
    mAllPrefs->Add(mClearAdbKeys);
    FindAndInitCheckboxPref(ENABLE_TERMINAL, (ICheckBoxPreference**)&mEnableTerminal);
    if (!IsPackageInstalled(activity, TERMINAL_APP_PACKAGE)) {
        debugDebuggingCategory->RemovePreference(IPreference::Probe(mEnableTerminal), &res);
        mEnableTerminal = NULL;
    }

    FindPreference(CoreUtils::Convert(BUGREPORT), (IPreference**)&mBugreport);
    FindAndInitCheckboxPref(BUGREPORT_IN_POWER_KEY, (ICheckBoxPreference**)&mBugreportInPower);
    FindAndInitCheckboxPref(KEEP_SCREEN_ON, (ICheckBoxPreference**)&mKeepScreenOn);
    FindAndInitCheckboxPref(BT_HCI_SNOOP_LOG, (ICheckBoxPreference**)&mBtHciSnoopLog);
    FindAndInitCheckboxPref(ENABLE_OEM_UNLOCK, (ICheckBoxPreference**)&mEnableOemUnlock);
    if (!ShowEnableOemUnlockPreference()) {
        RemovePreference(IPreference::Probe(mEnableOemUnlock));
        mEnableOemUnlock = NULL;
    }
    FindAndInitCheckboxPref(ALLOW_MOCK_LOCATION, (ICheckBoxPreference**)&mAllowMockLocation);
    FindAndInitCheckboxPref(DEBUG_VIEW_ATTRIBUTES, (ICheckBoxPreference**)&mDebugViewAttributes);
    pref = NULL;
    FindPreference(CoreUtils::Convert(LOCAL_BACKUP_PASSWORD), (IPreference**)&pref);
    mPassword = IPreferenceScreen::Probe(pref);
    mAllPrefs->Add(mPassword);

    AutoPtr<IUserHandleHelper> uhHelper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&uhHelper);
    AutoPtr<IUserHandle> OWNER;
    uhHelper->GetOWNER((IUserHandle**)&OWNER);
    if (uh->Equals(OWNER, &res), !res) {
        DisableForUser(IPreference::Probe(mEnableAdb));
        DisableForUser(mClearAdbKeys);
        DisableForUser(IPreference::Probe(mEnableTerminal));
        DisableForUser(IPreference::Probe(mPassword));
    }

    FindPreference(CoreUtils::Convert(DEBUG_APP_KEY), (IPreference**)&mDebugAppPref);
    mAllPrefs->Add(mDebugAppPref);
    FindAndInitCheckboxPref(WAIT_FOR_DEBUGGER_KEY, (ICheckBoxPreference**)&mWaitForDebugger);
    FindAndInitCheckboxPref(VERIFY_APPS_OVER_USB_KEY, (ICheckBoxPreference**)&mVerifyAppsOverUsb);
    if (!ShowVerifierSetting()) {
        if (debugDebuggingCategory != NULL) {
            debugDebuggingCategory->RemovePreference(IPreference::Probe(mVerifyAppsOverUsb), &res);
        }
        else {
            IPreference::Probe(mVerifyAppsOverUsb)->SetEnabled(FALSE);
        }
    }
    FindAndInitCheckboxPref(STRICT_MODE_KEY, (ICheckBoxPreference**)&mStrictMode);
    FindAndInitCheckboxPref(POINTER_LOCATION_KEY, (ICheckBoxPreference**)&mPointerLocation);
    FindAndInitCheckboxPref(SHOW_TOUCHES_KEY, (ICheckBoxPreference**)&mShowTouches);
    FindAndInitCheckboxPref(SHOW_SCREEN_UPDATES_KEY, (ICheckBoxPreference**)&mShowScreenUpdates);
    FindAndInitCheckboxPref(DISABLE_OVERLAYS_KEY, (ICheckBoxPreference**)&mDisableOverlays);
    FindAndInitCheckboxPref(SHOW_CPU_USAGE_KEY, (ICheckBoxPreference**)&mShowCpuUsage);
    FindAndInitCheckboxPref(FORCE_HARDWARE_UI_KEY, (ICheckBoxPreference**)&mForceHardwareUi);
    FindAndInitCheckboxPref(FORCE_MSAA_KEY, (ICheckBoxPreference**)&mForceMsaa);
    mTrackFrameTime = AddListPreference(TRACK_FRAME_TIME_KEY);
    mShowNonRectClip = AddListPreference(SHOW_NON_RECTANGULAR_CLIP_KEY);
    FindAndInitCheckboxPref(SHOW_HW_SCREEN_UPDATES_KEY, (ICheckBoxPreference**)&mShowHwScreenUpdates);
    FindAndInitCheckboxPref(SHOW_HW_LAYERS_UPDATES_KEY, (ICheckBoxPreference**)&mShowHwLayersUpdates);
    FindAndInitCheckboxPref(DEBUG_LAYOUT_KEY, (ICheckBoxPreference**)&mDebugLayout);
    FindAndInitCheckboxPref(FORCE_RTL_LAYOUT_KEY, (ICheckBoxPreference**)&mForceRtlLayout);
    mDebugHwOverdraw = AddListPreference(DEBUG_HW_OVERDRAW_KEY);
    FindAndInitCheckboxPref(WIFI_DISPLAY_CERTIFICATION_KEY, (ICheckBoxPreference**)&mWifiDisplayCertification);
    FindAndInitCheckboxPref(WIFI_VERBOSE_LOGGING_KEY, (ICheckBoxPreference**)&mWifiVerboseLogging);
    FindAndInitCheckboxPref(WIFI_AGGRESSIVE_HANDOVER_KEY, (ICheckBoxPreference**)&mWifiAggressiveHandover);
    FindAndInitCheckboxPref(WIFI_ALLOW_SCAN_WITH_TRAFFIC_KEY, (ICheckBoxPreference**)&mWifiAllowScansWithTraffic);
    mLogdSize = AddListPreference(SELECT_LOGD_SIZE_KEY);

    mWindowAnimationScale = AddListPreference(WINDOW_ANIMATION_SCALE_KEY);
    mTransitionAnimationScale = AddListPreference(TRANSITION_ANIMATION_SCALE_KEY);
    mAnimatorDurationScale = AddListPreference(ANIMATOR_DURATION_SCALE_KEY);
    mOverlayDisplayDevices = AddListPreference(OVERLAY_DISPLAY_DEVICES_KEY);
    mOpenGLTraces = AddListPreference(OPENGL_TRACES_KEY);
    mSimulateColorSpace = AddListPreference(SIMULATE_COLOR_SPACE);
    FindAndInitCheckboxPref(USE_NUPLAYER_KEY, (ICheckBoxPreference**)&mUseNuplayer);
    FindAndInitCheckboxPref(USB_AUDIO_KEY, (ICheckBoxPreference**)&mUSBAudio);

    pref = NULL;
    FindPreference(CoreUtils::Convert(IMMEDIATELY_DESTROY_ACTIVITIES_KEY), (IPreference**)&pref);
    mImmediatelyDestroyActivities = ICheckBoxPreference::Probe(pref);
    mAllPrefs->Add(mImmediatelyDestroyActivities);
    mResetCbPrefs->Add(mImmediatelyDestroyActivities);

    mAppProcessLimit = AddListPreference(APP_PROCESS_LIMIT_KEY);

    pref = NULL;
    FindPreference(CoreUtils::Convert(SHOW_ALL_ANRS_KEY), (IPreference**)&pref);
    mShowAllANRs = ICheckBoxPreference::Probe(pref);
    mAllPrefs->Add(mShowAllANRs);
    mResetCbPrefs->Add(mShowAllANRs);

    AutoPtr<IPreference> hdcpChecking;
    FindPreference(CoreUtils::Convert(HDCP_CHECKING_KEY), (IPreference**)&hdcpChecking);
    if (hdcpChecking != NULL) {
        mAllPrefs->Add(hdcpChecking);
        RemovePreferenceForProduction(hdcpChecking);
    }

    pref = NULL;
    FindPreference(CoreUtils::Convert(PROCESS_STATS), (IPreference**)&pref);
    mProcessStats = IPreferenceScreen::Probe(pref);
    mAllPrefs->Add(mProcessStats);
    return NOERROR;
}

AutoPtr<IListPreference> CDevelopmentSettings::AddListPreference(
    /* [in] */ const String& prefKey)
{
    AutoPtr<IPreference> preference;
    FindPreference(CoreUtils::Convert(prefKey), (IPreference**)&preference);
    IListPreference* pref = IListPreference::Probe(preference);
    mAllPrefs->Add(pref);
    preference->SetOnPreferenceChangeListener(mListener);
    return pref;
}

void CDevelopmentSettings::DisableForUser(
    /* [in] */ IPreference* pref)
{
    if (pref != NULL) {
        pref->SetEnabled(FALSE);
        mDisabledPrefs->Add(pref);
    }
}

ECode CDevelopmentSettings::FindAndInitCheckboxPref(
    /* [in] */ const String& key,
    /* [out] */ ICheckBoxPreference** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<IPreference> preference;
    FindPreference(CoreUtils::Convert(key), (IPreference**)&preference);
    ICheckBoxPreference* pref = ICheckBoxPreference::Probe(preference);
    if (pref == NULL) {
        Logger::E(TAG, "Cannot find preference with key = %s", key.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mAllPrefs->Add(pref);
    mResetCbPrefs->Add(pref);

    *result = pref;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CDevelopmentSettings::OnActivityCreated(
    /* [in] */ IBundle* savedInstanceState)
{
    SettingsPreferenceFragment::OnActivityCreated(savedInstanceState);

    AutoPtr<IActivity> act;
    GetActivity((IActivity**)&act);
    CSettingsActivity* activity = (CSettingsActivity*) ISettingsActivity::Probe(act);

    activity->GetSwitchBar((ISwitchBar**)&mSwitchBar);
    CSwitchBar* sb = (CSwitchBar*)mSwitchBar.Get();
    if (mUnavailable) {
        sb->SetEnabled(FALSE);
        return NOERROR;
    }

    sb->AddOnSwitchChangeListener(mListener);
    return NOERROR;
}

Boolean CDevelopmentSettings::RemovePreferenceForProduction(
    /* [in] */ IPreference* preference)
{
    if (String("user").Equals(Build::TYPE)) {
        RemovePreference(preference);
        return TRUE;
    }
    return FALSE;
}

void CDevelopmentSettings::RemovePreference(
    /* [in] */ IPreference* preference)
{
    AutoPtr<IPreferenceScreen> screen;
    GetPreferenceScreen((IPreferenceScreen**)&screen);
    Boolean res;
    IPreferenceGroup::Probe(screen)->RemovePreference(preference, &res);
    mAllPrefs->Remove(preference);
}

void CDevelopmentSettings::SetPrefsEnabledState(
    /* [in] */ Boolean enabled)
{
    Int32 size;
    mAllPrefs->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mAllPrefs->Get(i, (IInterface**)&obj);
        IPreference* pref = IPreference::Probe(obj);
        Boolean res;
        pref->SetEnabled(enabled && (mDisabledPrefs->Contains(pref, &res), !res));
    }
    UpdateAllOptions();
}

ECode CDevelopmentSettings::OnResume()
{
    SettingsPreferenceFragment::OnResume();

    if (mUnavailable) {
        // Show error message
        AutoPtr<IView> tmp;
        GetView((IView**)&tmp);
        AutoPtr<IView> emptyViewTmp;
        tmp->FindViewById(Elastos::Droid::R::id::empty, (IView**)&emptyViewTmp);
        ITextView* emptyView = ITextView::Probe(emptyViewTmp);
        AutoPtr<IListView> listView;
        GetListView((IListView**)&listView);
        IAdapterView::Probe(listView)->SetEmptyView(emptyViewTmp);
        if (emptyView != NULL) {
            emptyView->SetText(R::string::development_settings_not_available);
        }
        return NOERROR;
    }

    Int64 data;
    mDpm->GetMaximumTimeToLock(NULL, &data);
    if (data > 0) {
        // A DeviceAdmin has specified a maximum time until the device
        // will lock...  in this case we can't allow the user to turn
        // on "stay awake when plugged in" because that would defeat the
        // restriction.
        mDisabledPrefs->Add(mKeepScreenOn);
    }
    else {
        mDisabledPrefs->Remove(mKeepScreenOn);
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> cr;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 result;
    global->GetInt32(cr,
            ISettingsGlobal::DEVELOPMENT_SETTINGS_ENABLED, 0, &result);
    mLastEnabledState = result != 0;
    CSwitchBar* switchBar = (CSwitchBar*)mSwitchBar.Get();
    switchBar->SetChecked(mLastEnabledState);
    SetPrefsEnabledState(mLastEnabledState);

    if (mHaveDebugSettings && !mLastEnabledState) {
        // Overall debugging is disabled, but there are some debug
        // settings that are enabled.  This is an invalid state.  Switch
        // to debug settings being enabled, so the user knows there is
        // stuff enabled and can turn it all off if they want.
        Boolean res;
        global->PutInt32(cr, ISettingsGlobal::DEVELOPMENT_SETTINGS_ENABLED, 1, &res);
        mLastEnabledState = TRUE;
        switchBar->SetChecked(mLastEnabledState);
        SetPrefsEnabledState(mLastEnabledState);
    }
    switchBar->Show();
    return NOERROR;
}

ECode CDevelopmentSettings::OnDestroyView()
{
    SettingsPreferenceFragment::OnDestroyView();

    if (mUnavailable) {
        return NOERROR;
    }
    CSwitchBar* switchBar = (CSwitchBar*)mSwitchBar.Get();
    switchBar->RemoveOnSwitchChangeListener(mListener);
    switchBar->Hide();
    return NOERROR;
}

void CDevelopmentSettings::UpdateCheckBox(
    /* [in] */ ICheckBoxPreference* checkBox,
    /* [in] */ Boolean value)
{
    ITwoStatePreference::Probe(checkBox)->SetChecked(value);
    mHaveDebugSettings |= value;
}

void CDevelopmentSettings::UpdateAllOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    IContext* context = IContext::Probe(activity);
    AutoPtr<IContentResolver> cr;
    context->GetContentResolver((IContentResolver**)&cr);
    mHaveDebugSettings = FALSE;
    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 data;
    global->GetInt32(cr, ISettingsGlobal::ADB_ENABLED, 0, &data);
    UpdateCheckBox(mEnableAdb, data != 0);
    if (mEnableTerminal != NULL) {
        AutoPtr<IPackageManager> pm;
        context->GetPackageManager((IPackageManager**)&pm);
        Int32 value;
        pm->GetApplicationEnabledSetting(TERMINAL_APP_PACKAGE, &value);
        UpdateCheckBox(mEnableTerminal, value == IPackageManager::COMPONENT_ENABLED_STATE_ENABLED);
    }

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    secure->GetInt32(cr, ISettingsSecure::BUGREPORT_IN_POWER_MENU, 0, &data);
    UpdateCheckBox(mBugreportInPower, data != 0);
    global->GetInt32(cr, ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN, 0, &data);
    UpdateCheckBox(mKeepScreenOn, data != 0);
    secure->GetInt32(cr, ISettingsSecure::BLUETOOTH_HCI_LOG, 0, &data);
    UpdateCheckBox(mBtHciSnoopLog, data != 0);
    if (mEnableOemUnlock != NULL) {
        UpdateCheckBox(mEnableOemUnlock, Utils::IsOemUnlockEnabled(context));
    }
    secure->GetInt32(cr, ISettingsSecure::ALLOW_MOCK_LOCATION, 0, &data);
    UpdateCheckBox(mAllowMockLocation, data != 0);
    global->GetInt32(cr, ISettingsGlobal::DEBUG_VIEW_ATTRIBUTES, 0, &data);
    UpdateCheckBox(mDebugViewAttributes, data != 0);
    UpdateHdcpValues();
    Logger::D("CDevelopmentSettings::UpdatePasswordSummary", "=============== TODO wait for BackupManager ================");
    // UpdatePasswordSummary();
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

void CDevelopmentSettings::ResetDangerousOptions()
{
    mDontPokeProperties = TRUE;
    Int32 size;
    mResetCbPrefs->GetSize(&size);
    Boolean res;
    for (Int32 i = 0; i< size; i++) {
        AutoPtr<IInterface> obj;
        mResetCbPrefs->Get(i, (IInterface**)&obj);
        ITwoStatePreference* /*ICheckBoxPreference*/ cb = ITwoStatePreference::Probe(obj);
        if (cb->IsChecked(&res), res) {
            cb->SetChecked(FALSE);
            OnPreferenceTreeClick(NULL, IPreference::Probe(cb), &res);
        }
    }
    ResetDebuggerOptions();
    WriteLogdSizeOption(NULL);
    WriteAnimationScaleOption(0, mWindowAnimationScale, NULL);
    WriteAnimationScaleOption(1, mTransitionAnimationScale, NULL);
    WriteAnimationScaleOption(2, mAnimatorDurationScale, NULL);
    // Only poke the color space setting if we control it.
    if (UsingDevelopmentColorSpace()) {
        WriteSimulateColorSpace(CoreUtils::Convert(-1));
    }
    WriteOverlayDisplayDevicesOptions(NULL);
    WriteAppProcessLimitOptions(NULL);
    mHaveDebugSettings = FALSE;
    UpdateAllOptions();
    mDontPokeProperties = FALSE;
    PokeSystemProperties();
}

void CDevelopmentSettings::UpdateHdcpValues()
{
    AutoPtr<IPreference> pref;
    FindPreference(CoreUtils::Convert(HDCP_CHECKING_KEY), (IPreference**)&pref);
    IListPreference* hdcpChecking = IListPreference::Probe(pref);
    if (hdcpChecking != NULL) {
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        String currentValue;
        sp->Get(HDCP_CHECKING_PROPERTY, &currentValue);
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr< ArrayOf<String> > values;
        resources->GetStringArray(R::array::hdcp_checking_values, (ArrayOf<String>**)&values);
        AutoPtr< ArrayOf<String> > summaries;
        resources->GetStringArray(R::array::hdcp_checking_summaries, (ArrayOf<String>**)&summaries);
        Int32 index = 1; // Defaults to drm-only. Needs to match with R.array.hdcp_checking_values
        for (Int32 i = 0; i < values->GetLength(); i++) {
            if (currentValue.Equals((*values)[i])) {
                index = i;
                break;
            }
        }
        hdcpChecking->SetValue((*values)[index]);
        pref->SetSummary(CoreUtils::Convert((*summaries)[index]));
        pref->SetOnPreferenceChangeListener(mListener);
    }
}

void CDevelopmentSettings::UpdatePasswordSummary()
{
    // try {
    Boolean res;
    assert(0 && "TODO");
    if (mBackupManager->HasBackupPassword(&res), res) {
        IPreference::Probe(mPassword)->SetSummary(R::string::local_backup_password_summary_change);
    }
    else {
        IPreference::Probe(mPassword)->SetSummary(R::string::local_backup_password_summary_none);
    }

    // } catch (RemoteException e) {
    //     // Not much we can do here
    // }
}

void CDevelopmentSettings::WriteBtHciSnoopLogOptions()
{
    AutoPtr<IBluetoothAdapterHelper> helper;
    CBluetoothAdapterHelper::AcquireSingleton((IBluetoothAdapterHelper**)&helper);
    AutoPtr<IBluetoothAdapter> adapter;
    helper->GetDefaultAdapter((IBluetoothAdapter**)&adapter);
    Boolean res, result;
    ITwoStatePreference::Probe(mBtHciSnoopLog)->IsChecked(&res);
    adapter->ConfigHciSnoopLog(res, &result);

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    secure->PutInt32(resolver,
            ISettingsSecure::BLUETOOTH_HCI_LOG, res ? 1 : 0, &res);
}

void CDevelopmentSettings::WriteDebuggerOptions()
{
    // try {
        Boolean res;
        ITwoStatePreference::Probe(mWaitForDebugger)->IsChecked(&res);
        ActivityManagerNative::GetDefault()->SetDebugApp(
            mDebugApp, res, TRUE);
    // } catch (RemoteException ex) {
    // }
}

void CDevelopmentSettings::ResetDebuggerOptions()
{
    // try {
        ActivityManagerNative::GetDefault()->SetDebugApp(
                String(NULL), FALSE, TRUE);
    // } catch (RemoteException ex) {
    // }
}

void CDevelopmentSettings::UpdateDebuggerOptions()
{
    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    AutoPtr<IContentResolver> resolver;
    activity->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    global->GetString(
            resolver, ISettingsGlobal::DEBUG_APP, &mDebugApp);
    Int32 data;
    global->GetInt32(resolver, ISettingsGlobal::WAIT_FOR_DEBUGGER, 0, &data);
    UpdateCheckBox(mWaitForDebugger, data != 0);
    if (mDebugApp != NULL && mDebugApp.GetLength() > 0) {
        String label;
        // try {
        AutoPtr<IPackageManager> pm;
        activity->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IApplicationInfo> ai;
        ECode ec = pm->GetApplicationInfo(mDebugApp,
                    IPackageManager::GET_DISABLED_COMPONENTS, (IApplicationInfo**)&ai);
        if (ec == (ECode)E_PACKAGEMANAGER_NAME_NOT_FOUND_EXCEPTION) {
            label = mDebugApp;
        }
        else {
            AutoPtr<ICharSequence> lab;
            pm->GetApplicationLabel(ai, (ICharSequence**)&lab);
            label = lab != NULL ? Object::ToString(lab) : mDebugApp;
        }

        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, CoreUtils::Convert(label));
        String str;
        resources->GetString(R::string::debug_app_set, args, &str);
        mDebugAppPref->SetSummary(CoreUtils::Convert(str));
        IPreference::Probe(mWaitForDebugger)->SetEnabled(TRUE);
        mHaveDebugSettings = TRUE;
    }
    else {
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        String str;
        resources->GetString(R::string::debug_app_not_set, &str);
        mDebugAppPref->SetSummary(CoreUtils::Convert(str));
        IPreference::Probe(mWaitForDebugger)->SetEnabled(FALSE);
    }
}

void CDevelopmentSettings::UpdateVerifyAppsOverUsbOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 data;
    global->GetInt32(resolver, ISettingsGlobal::PACKAGE_VERIFIER_INCLUDE_ADB, 1, &data);
    UpdateCheckBox(mVerifyAppsOverUsb, data != 0);
    IPreference::Probe(mVerifyAppsOverUsb)->SetEnabled(EnableVerifierSetting());
}

void CDevelopmentSettings::WriteVerifyAppsOverUsbOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Boolean res;
    global->PutInt32(resolver,
          ISettingsGlobal::PACKAGE_VERIFIER_INCLUDE_ADB, (ITwoStatePreference::Probe(mVerifyAppsOverUsb)->IsChecked(&res), res) ? 1 : 0, &res);
}

Boolean CDevelopmentSettings::EnableVerifierSetting()
{
    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    AutoPtr<IContentResolver> cr;
    activity->GetContentResolver((IContentResolver**)&cr);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 data;
    global->GetInt32(cr, ISettingsGlobal::ADB_ENABLED, 0, &data);
    if (data == 0) {
        return FALSE;
    }
    global->GetInt32(cr, ISettingsGlobal::PACKAGE_VERIFIER_ENABLE, 1, &data);
    if (data == 0) {
        return FALSE;
    }
    else {
        AutoPtr<IPackageManager> pm;
        activity->GetPackageManager((IPackageManager**)&pm);
        AutoPtr<IIntent> verification;
        CIntent::New(IIntent::ACTION_PACKAGE_NEEDS_VERIFICATION, (IIntent**)&verification);
        verification->SetType(PACKAGE_MIME_TYPE);
        verification->AddFlags(IIntent::FLAG_GRANT_READ_URI_PERMISSION);
        AutoPtr<IList> receivers;// List<ResolveInfo>
        pm->QueryBroadcastReceivers(verification, 0, (IList**)&receivers);
        Int32 size;
        if ((receivers->GetSize(&size), size) == 0) {
            return FALSE;
        }
    }
    return TRUE;
}

Boolean CDevelopmentSettings::ShowVerifierSetting()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 data;
    global->GetInt32(resolver,
            ISettingsGlobal::PACKAGE_VERIFIER_SETTING_VISIBLE, 1, &data);
    return data > 0;
}

Boolean CDevelopmentSettings::ShowEnableOemUnlockPreference()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String str;
    sp->Get(PERSISTENT_DATA_BLOCK_PROP, &str);
    return !str.Equals("");
}

void CDevelopmentSettings::UpdateBugreportOptions()
{
    if (String("user").Equals(Build::TYPE)) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IContentResolver> resolver;
        IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

        AutoPtr<ISettingsGlobal> global;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
        Int32 data;
        global->GetInt32(resolver, ISettingsGlobal::ADB_ENABLED, 0, &data);
        Boolean adbEnabled = data != 0;
        if (adbEnabled) {
            mBugreport->SetEnabled(TRUE);
            IPreference::Probe(mBugreportInPower)->SetEnabled(TRUE);
        }
        else {
            mBugreport->SetEnabled(FALSE);
            IPreference::Probe(mBugreportInPower)->SetEnabled(FALSE);
            ITwoStatePreference::Probe(mBugreportInPower)->SetChecked(FALSE);
            AutoPtr<ISettingsSecure> secure;
            CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
            Boolean res;
            secure->PutInt32(resolver, ISettingsSecure::BUGREPORT_IN_POWER_MENU, 0, &res);
        }
    }
    else {
        IPreference::Probe(mBugreportInPower)->SetEnabled(TRUE);
    }
}

Int32 CDevelopmentSettings::CurrentStrictModeActiveIndex()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String str;
    sp->Get(IStrictMode::VISUAL_PROPERTY, &str);
    if (TextUtils::IsEmpty(str)) {
        return 0;
    }
    Boolean enabled;
    sp->GetBoolean(IStrictMode::VISUAL_PROPERTY, FALSE, &enabled);
    return enabled ? 1 : 2;
}

void CDevelopmentSettings::WriteStrictModeVisualOptions()
{
    // try {
    Boolean res;
    ITwoStatePreference::Probe(mStrictMode)->IsChecked(&res);
    mWindowManager->SetStrictModeVisualIndicatorPreference(res ? String("1") : String(""));
    // } catch (RemoteException e) {
    // }
}

void CDevelopmentSettings::UpdateStrictModeVisualOptions()
{
    UpdateCheckBox(mStrictMode, CurrentStrictModeActiveIndex() == 1);
}

void CDevelopmentSettings::WritePointerLocationOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);

    Boolean res;
    sys->PutInt32(resolver,
            ISettingsSystem::POINTER_LOCATION, (ITwoStatePreference::Probe(mPointerLocation)->IsChecked(&res), res) ? 1 : 0, &res);
}

void CDevelopmentSettings::UpdatePointerLocationOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);

    Int32 data;
    sys->GetInt32(resolver, ISettingsSystem::POINTER_LOCATION, 0, &data);
    UpdateCheckBox(mPointerLocation, data != 0);
}

void CDevelopmentSettings::WriteShowTouchesOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);

    Boolean res;
    sys->PutInt32(resolver,
            ISettingsSystem::SHOW_TOUCHES, (ITwoStatePreference::Probe(mShowTouches)->IsChecked(&res), res) ? 1 : 0, &res);
}

void CDevelopmentSettings::UpdateShowTouchesOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSystem> sys;
    CSettingsSystem::AcquireSingleton((ISettingsSystem**)&sys);
    Int32 data;
    sys->GetInt32(resolver, ISettingsSystem::SHOW_TOUCHES, 0, &data);
    UpdateCheckBox(mShowTouches, data != 0);
}

void CDevelopmentSettings::UpdateFlingerOptions()
{
    // magic communication with surface flinger.
    // try {
    IBinder* flinger = IBinder::Probe(ServiceManager::GetService(String("SurfaceFlinger")));
    if (flinger != NULL) {
        assert(0 && "TODO");
        // AutoPtr<IParcel> data;
        // CParcel::New((IParcel**)&data);
        // AutoPtr<IParcel> reply;
        // CParcel::New((IParcel**)&reply);
        // // Parcel data = Parcel->Obtain();
        // // Parcel reply = Parcel->Obtain();
        // data->WriteInterfaceToken("android.ui.ISurfaceComposer");
        // flinger->Transact(1010, data, reply, 0);
        // // @SuppressWarnings("unused")
        // Int32 showCpu;
        // reply->ReadInt32(&showCpu);
        // // @SuppressWarnings("unused")
        // Int32 enableGL;
        // reply->ReadInt32(&enableGL);
        // Int32 showUpdates;
        // reply->ReadInt32(&showUpdates);
        // UpdateCheckBox(mShowScreenUpdates, showUpdates != 0);
        // // @SuppressWarnings("unused")
        // Int32 showBackground;
        // reply->ReadInt32(&showBackground);
        // Int32 disableOverlays;
        // reply->ReadInt32(&disableOverlays);
        // UpdateCheckBox(mDisableOverlays, disableOverlays != 0);
        // reply->Recycle();
        // data->Recycle();
    }
    // } catch (RemoteException ex) {
    // }
}

void CDevelopmentSettings::WriteShowUpdatesOption()
{
    // try {
        assert(0 && "TODO");
        // IBinder* flinger = ServiceManager::GetService("SurfaceFlinger");
        // if (flinger != NULL) {
        //     Parcel data = Parcel->Obtain();
        //     data->WriteInterfaceToken("android.ui.ISurfaceComposer");
        //     final Int32 showUpdates = mShowScreenUpdates->IsChecked() ? 1 : 0;
        //     data->WriteInt(showUpdates);
        //     flinger->Transact(1002, data, NULL, 0);
        //     data->Recycle();

        //     UpdateFlingerOptions();
        // }
    // } catch (RemoteException ex) {
    // }
}

void CDevelopmentSettings::WriteDisableOverlaysOption()
{
    // try {
        assert(0 && "TODO");
        // IBinder flinger = ServiceManager->GetService("SurfaceFlinger");
        // if (flinger != NULL) {
        //     Parcel data = Parcel->Obtain();
        //     data->WriteInterfaceToken("android.ui.ISurfaceComposer");
        //     final Int32 disableOverlays = mDisableOverlays->IsChecked() ? 1 : 0;
        //     data->WriteInt(disableOverlays);
        //     flinger->Transact(1008, data, NULL, 0);
        //     data->Recycle();

        //     UpdateFlingerOptions();
        // }
    // } catch (RemoteException ex) {
    // }
}

void CDevelopmentSettings::UpdateHardwareUiOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->GetBoolean(HARDWARE_UI_PROPERTY, FALSE, &res);
    UpdateCheckBox(mForceHardwareUi, res);
}

void CDevelopmentSettings::WriteHardwareUiOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->Set(HARDWARE_UI_PROPERTY, (ITwoStatePreference::Probe(mForceHardwareUi)->IsChecked(&res), res) ? String("TRUE") : String("FALSE"));
    PokeSystemProperties();
}

void CDevelopmentSettings::UpdateMsaaOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->GetBoolean(MSAA_PROPERTY, FALSE, &res);
    UpdateCheckBox(mForceMsaa, res);
}

void CDevelopmentSettings::WriteMsaaOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->Set(MSAA_PROPERTY, (ITwoStatePreference::Probe(mForceMsaa)->IsChecked(&res), res) ? String("TRUE") : String("FALSE"));
    PokeSystemProperties();
}

void CDevelopmentSettings::UpdateTrackFrameTimeOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String value;
    sp->Get(IHardwareRenderer::PROFILE_PROPERTY, &value);
    if (value.IsNull()) {
        value = "";
    }

    AutoPtr< ArrayOf<ICharSequence*> > values;
    mTrackFrameTime->GetEntryValues((ArrayOf<ICharSequence*>**)&values);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    mTrackFrameTime->GetEntries((ArrayOf<ICharSequence*>**)&entries);

    IPreference* trackFrameTime = IPreference::Probe(mTrackFrameTime);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        if ((*values)[i] != NULL && value.Equals(Object::ToString((*values)[i]))) {
            mTrackFrameTime->SetValueIndex(i);
            trackFrameTime->SetSummary((*entries)[i]);
            return;
        }
    }
    mTrackFrameTime->SetValueIndex(0);
    trackFrameTime->SetSummary((*entries)[0]);
}

void CDevelopmentSettings::WriteTrackFrameTimeOptions(
    /* [in] */ IInterface* newValue)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Set(IHardwareRenderer::PROFILE_PROPERTY,
            newValue == NULL ? String("") : Object::ToString(newValue));
    PokeSystemProperties();
    UpdateTrackFrameTimeOptions();
}

void CDevelopmentSettings::UpdateShowNonRectClipOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String value;
    sp->Get(IHardwareRenderer::DEBUG_SHOW_NON_RECTANGULAR_CLIP_PROPERTY, &value);
    if (value.IsNull()) {
        value = "hide";
    }

    AutoPtr< ArrayOf<ICharSequence*> > values;
    mShowNonRectClip->GetEntryValues((ArrayOf<ICharSequence*>**)&values);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    mShowNonRectClip->GetEntries((ArrayOf<ICharSequence*>**)&entries);

    IPreference* showNonRectClip = IPreference::Probe(mShowNonRectClip);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        if ((*values)[i] != NULL && value.Equals(Object::ToString((*values)[i]))) {
            mShowNonRectClip->SetValueIndex(i);
            showNonRectClip->SetSummary((*entries)[i]);
            return;
        }
    }
    mShowNonRectClip->SetValueIndex(0);
    showNonRectClip->SetSummary((*entries)[0]);
}

void CDevelopmentSettings::WriteShowNonRectClipOptions(
    /* [in] */ IInterface* newValue)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Set(IHardwareRenderer::DEBUG_SHOW_NON_RECTANGULAR_CLIP_PROPERTY,
            newValue == NULL ? String("") : Object::ToString(newValue));
    PokeSystemProperties();
    UpdateShowNonRectClipOptions();
}

void CDevelopmentSettings::UpdateShowHwScreenUpdatesOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->GetBoolean(IHardwareRenderer::DEBUG_DIRTY_REGIONS_PROPERTY, FALSE, &res);
    UpdateCheckBox(mShowHwScreenUpdates, res);
}

void CDevelopmentSettings::WriteShowHwScreenUpdatesOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->Set(IHardwareRenderer::DEBUG_DIRTY_REGIONS_PROPERTY,
            (ITwoStatePreference::Probe(mShowHwScreenUpdates)->IsChecked(&res), res) ? String("TRUE") : String(NULL));
    PokeSystemProperties();
}

void CDevelopmentSettings::UpdateShowHwLayersUpdatesOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->GetBoolean(IHardwareRenderer::DEBUG_SHOW_LAYERS_UPDATES_PROPERTY, FALSE, &res);
    UpdateCheckBox(mShowHwLayersUpdates, res);
}

void CDevelopmentSettings::WriteShowHwLayersUpdatesOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->Set(IHardwareRenderer::DEBUG_SHOW_LAYERS_UPDATES_PROPERTY,
            (ITwoStatePreference::Probe(mShowHwLayersUpdates)->IsChecked(&res), res) ? String("TRUE") : String(NULL));
    PokeSystemProperties();
}

void CDevelopmentSettings::UpdateDebugHwOverdrawOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String value;
    sp->Get(IHardwareRenderer::DEBUG_OVERDRAW_PROPERTY, &value);
    if (value.IsNull()) {
        value = "";
    }

    AutoPtr< ArrayOf<ICharSequence*> > values;
    mDebugHwOverdraw->GetEntryValues((ArrayOf<ICharSequence*>**)&values);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    mDebugHwOverdraw->GetEntries((ArrayOf<ICharSequence*>**)&entries);

    IPreference* debugHwOverdraw = IPreference::Probe(mDebugHwOverdraw);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        if ((*values)[i] != NULL && value.Equals(Object::ToString((*values)[i]))) {
            mDebugHwOverdraw->SetValueIndex(i);
            debugHwOverdraw->SetSummary((*entries)[i]);
            return;
        }
    }
    mDebugHwOverdraw->SetValueIndex(0);
    debugHwOverdraw->SetSummary((*entries)[0]);
}

void CDevelopmentSettings::WriteDebugHwOverdrawOptions(
    /* [in] */ IInterface* newValue)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Set(IHardwareRenderer::DEBUG_OVERDRAW_PROPERTY,
            newValue == NULL ? String("") : Object::ToString(newValue));
    PokeSystemProperties();
    UpdateDebugHwOverdrawOptions();
}

void CDevelopmentSettings::UpdateDebugLayoutOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->GetBoolean(IView::DEBUG_LAYOUT_PROPERTY, FALSE, &res);
    UpdateCheckBox(mDebugLayout, res);
}

void CDevelopmentSettings::WriteDebugLayoutOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->Set(IView::DEBUG_LAYOUT_PROPERTY,
            (ITwoStatePreference::Probe(mDebugLayout)->IsChecked(&res), res) ? String("TRUE") : String("FALSE"));
    PokeSystemProperties();
}

void CDevelopmentSettings::UpdateSimulateColorSpace()
{
    AutoPtr<IContentResolver> cr = GetContentResolver();
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 data;
    secure->GetInt32(cr, ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, 0, &data);
    Boolean enabled = data != 0;
    if (enabled) {
        secure->GetInt32(cr, ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER,
                IAccessibilityManager::DALTONIZER_DISABLED, &data);
        String mode = StringUtils::ToString(data);
        mSimulateColorSpace->SetValue(mode);
        Int32 index;
        mSimulateColorSpace->FindIndexOfValue(mode, &index);
        if (index < 0) {
            // We're using a mode controlled by accessibility preferences.
            String str;
            GetString(R::string::accessibility_display_daltonizer_preference_title, &str);
            AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
            args->Set(0, CoreUtils::Convert(str));
            GetString(R::string::daltonizer_type_overridden, args, &str);
            IPreference::Probe(mSimulateColorSpace)->SetSummary(CoreUtils::Convert(str));
        }
        else {
            IPreference::Probe(mSimulateColorSpace)->SetSummary(CoreUtils::Convert("%s"));
        }
    }
    else {
        mSimulateColorSpace->SetValue(
                StringUtils::ToString(IAccessibilityManager::DALTONIZER_DISABLED));
    }
}

Boolean CDevelopmentSettings::UsingDevelopmentColorSpace()
{
    AutoPtr<IContentResolver> cr = GetContentResolver();
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 data;
    secure->GetInt32(cr, ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, 0, &data);
    Boolean enabled = data != 0;
    if (enabled) {
        secure->GetInt32(
                cr, ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER,
                IAccessibilityManager::DALTONIZER_DISABLED, &data);
        String mode = StringUtils::ToString(data);
        Int32 index;
        mSimulateColorSpace->FindIndexOfValue(mode, &index);
        if (index >= 0) {
            // We're using a mode controlled by developer preferences.
            return TRUE;
        }
    }
    return FALSE;
}

void CDevelopmentSettings::WriteSimulateColorSpace(
    /* [in] */ IInterface* value)
{
    AutoPtr<IContentResolver> cr = GetContentResolver();
    Int32 newMode = StringUtils::ParseInt32(Object::ToString(value));
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Boolean res;
    if (newMode < 0) {
        secure->PutInt32(cr, ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, 0, &res);
    }
    else {
        secure->PutInt32(cr, ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER_ENABLED, 1, &res);
        secure->PutInt32(cr, ISettingsSecure::ACCESSIBILITY_DISPLAY_DALTONIZER, newMode, &res);
    }
}

void CDevelopmentSettings::UpdateUseNuplayerOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->GetBoolean(USE_AWESOMEPLAYER_PROPERTY, FALSE, &res);
    UpdateCheckBox(mUseNuplayer, !res);
}

void CDevelopmentSettings::WriteUseNuplayerOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    Boolean res;
    sp->Set(
            USE_AWESOMEPLAYER_PROPERTY, (ITwoStatePreference::Probe(mUseNuplayer)->IsChecked(&res), res) ? String("FALSE") : String("TRUE"));
    PokeSystemProperties();
}

void CDevelopmentSettings::UpdateUSBAudioOptions()
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 data;
    secure->GetInt32(GetContentResolver(),
            ISettingsSecure::USB_AUDIO_AUTOMATIC_ROUTING_DISABLED, 0, &data);
    UpdateCheckBox(mUSBAudio, data != 0);
}

void CDevelopmentSettings::WriteUSBAudioOptions()
{
    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Boolean res;
    secure->PutInt32(GetContentResolver(),
            ISettingsSecure::USB_AUDIO_AUTOMATIC_ROUTING_DISABLED,
            (ITwoStatePreference::Probe(mUSBAudio)->IsChecked(&res), res) ? 1 : 0, &res);
}

void CDevelopmentSettings::UpdateForceRtlOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 data;
    global->GetInt32(resolver, ISettingsGlobal::DEVELOPMENT_FORCE_RTL, 0, &data);
    UpdateCheckBox(mForceRtlLayout, data != 0);
}

void CDevelopmentSettings::WriteForceRtlOptions()
{
    Boolean value;
    ITwoStatePreference::Probe(mForceRtlLayout)->IsChecked(&value);

    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    AutoPtr<IContentResolver> resolver;
    activity->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Boolean res;
    global->PutInt32(resolver, ISettingsGlobal::DEVELOPMENT_FORCE_RTL, value ? 1 : 0, &res);

    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Set(ISettingsGlobal::DEVELOPMENT_FORCE_RTL, value ? String("1") : String("0"));

    AutoPtr<IResources> resources;
    activity->GetResources((IResources**)&resources);
    AutoPtr<IConfiguration> config;
    resources->GetConfiguration((IConfiguration**)&config);
    AutoPtr<ILocale> locale;
    config->GetLocale((ILocale**)&locale);
    AutoPtr<ILocalePickerHelper> helper;
    CLocalePickerHelper::AcquireSingleton((ILocalePickerHelper**)&helper);
    helper->UpdateLocale(locale);
}

void CDevelopmentSettings::UpdateWifiDisplayCertificationOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 data;
    global->GetInt32(resolver, ISettingsGlobal::WIFI_DISPLAY_CERTIFICATION_ON, 0, &data);
    UpdateCheckBox(mWifiDisplayCertification, data != 0);
}

void CDevelopmentSettings::WriteWifiDisplayCertificationOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Boolean res;
    global->PutInt32(resolver,
            ISettingsGlobal::WIFI_DISPLAY_CERTIFICATION_ON,
            (ITwoStatePreference::Probe(mWifiDisplayCertification)->IsChecked(&res), res) ? 1 : 0, &res);
}

void CDevelopmentSettings::UpdateWifiVerboseLoggingOptions()
{
    Int32 data;
    mWifiManager->GetVerboseLoggingLevel(&data);
    Boolean enabled = data > 0;
    UpdateCheckBox(mWifiVerboseLogging, enabled);
}

void CDevelopmentSettings::WriteWifiVerboseLoggingOptions()
{
    Boolean res;
    ITwoStatePreference::Probe(mWifiVerboseLogging)->IsChecked(&res);
    mWifiManager->EnableVerboseLogging(res ? 1 : 0);
}

void CDevelopmentSettings::UpdateWifiAggressiveHandoverOptions()
{
    Int32 data;
    mWifiManager->GetAggressiveHandover(&data);
    Boolean enabled = data > 0;
    UpdateCheckBox(mWifiAggressiveHandover, enabled);
}

void CDevelopmentSettings::WriteWifiAggressiveHandoverOptions()
{
    Boolean res;
    ITwoStatePreference::Probe(mWifiAggressiveHandover)->IsChecked(&res);
    mWifiManager->EnableAggressiveHandover(res ? 1 : 0);
}

void CDevelopmentSettings::UpdateWifiAllowScansWithTrafficOptions()
{
    Int32 data;
    mWifiManager->GetAllowScansWithTraffic(&data);
    Boolean enabled = data > 0;
    UpdateCheckBox(mWifiAllowScansWithTraffic, enabled);
}

void CDevelopmentSettings::WriteWifiAllowScansWithTrafficOptions()
{
    Boolean res;
    ITwoStatePreference::Probe(mWifiAllowScansWithTraffic)->IsChecked(&res);
    mWifiManager->SetAllowScansWithTraffic(res ? 1 : 0);
}

void CDevelopmentSettings::UpdateLogdSizeValues()
{
    if (mLogdSize != NULL) {
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        String currentValue;
        sp->Get(SELECT_LOGD_SIZE_PROPERTY, &currentValue);
        if (currentValue.IsNull()) {
            sp->Get(SELECT_LOGD_DEFAULT_SIZE_PROPERTY, &currentValue);
            if (currentValue.IsNull()) {
                currentValue = "256K";
            }
        }
        AutoPtr<IResources> resources;
        GetResources((IResources**)&resources);
        AutoPtr< ArrayOf<String> > values;
        resources->GetStringArray(R::array::select_logd_size_values, (ArrayOf<String>**)&values);
        AutoPtr< ArrayOf<String> > titles;
        resources->GetStringArray(R::array::select_logd_size_titles, (ArrayOf<String>**)&titles);

        String str;
        sp->Get(String("ro.config.low_ram"), &str);
        if (str.Equals(String("TRUE"))) {
            mLogdSize->SetEntries(R::array::select_logd_size_lowram_titles);
            titles = NULL;
            resources->GetStringArray(R::array::select_logd_size_lowram_titles, (ArrayOf<String>**)&titles);
        }
        AutoPtr< ArrayOf<String> > summaries;
        resources->GetStringArray(R::array::select_logd_size_summaries, (ArrayOf<String>**)&summaries);
        Int32 index = 1; // punt to second entry if not found
        for (Int32 i = 0; i < titles->GetLength(); i++) {
            if (currentValue.Equals((*values)[i])
                    || currentValue.Equals((*titles)[i])) {
                index = i;
                break;
            }
        }
        mLogdSize->SetValue((*values)[index]);
        IPreference* logdSize = IPreference::Probe(mLogdSize);
        logdSize->SetSummary(CoreUtils::Convert((*summaries)[index]));
        logdSize->SetOnPreferenceChangeListener(mListener);
    }
}

void CDevelopmentSettings::WriteLogdSizeOption(
    /* [in] */ IInterface* newValue)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String currentValue;
    sp->Get(SELECT_LOGD_DEFAULT_SIZE_PROPERTY, &currentValue);
    if (!currentValue.IsNull()) {
        DEFAULT_LOG_RING_BUFFER_SIZE_IN_BYTES = currentValue;
    }
    const String size = (newValue != NULL) ?
            Object::ToString(newValue) : DEFAULT_LOG_RING_BUFFER_SIZE_IN_BYTES;
    sp->Set(SELECT_LOGD_SIZE_PROPERTY, size);
    PokeSystemProperties();
    assert(0 && "TODO");
    // try {
    //     Process p = Runtime->GetRuntime()->Exec("logcat -b all -G " + size);
    //     p->WaitFor();
    //     Logger::I(TAG, "Logcat ring buffer sizes set to: " + size);
    // } catch (Exception e) {
    //     Logger::W(TAG, "Cannot set logcat ring buffer sizes", e);
    // }
    UpdateLogdSizeValues();
}

void CDevelopmentSettings::UpdateCpuUsageOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 data;
    global->GetInt32(resolver, ISettingsGlobal::SHOW_PROCESSES, 0, &data);
    UpdateCheckBox(mShowCpuUsage, data != 0);
}

void CDevelopmentSettings::WriteCpuUsageOptions()
{
    Boolean value;
    ITwoStatePreference::Probe(mShowCpuUsage)->IsChecked(&value);
    AutoPtr<IActivity> _activity;
    GetActivity((IActivity**)&_activity);
    IContext* activity = IContext::Probe(_activity);
    AutoPtr<IContentResolver> resolver;
    activity->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Boolean res;
    global->PutInt32(resolver, ISettingsGlobal::SHOW_PROCESSES, value ? 1 : 0, &res);
    AutoPtr<IIntent> service;
    CIntent::New((IIntent**)&service);
    service->SetClassName(String("com.android.systemui"), String("com.android.systemui.LoadAverageService"));
    if (value) {
        AutoPtr<IComponentName> comp;
        activity->StartService(service, (IComponentName**)&comp);
    }
    else {
        activity->StopService(service, &res);
    }
}

void CDevelopmentSettings::WriteImmediatelyDestroyActivitiesOptions()
{
    // try {
    Boolean res;
    ITwoStatePreference::Probe(mImmediatelyDestroyActivities)->IsChecked(&res);
    ActivityManagerNative::GetDefault()->SetAlwaysFinish(res);
    // } catch (RemoteException ex) {
    // }
}

void CDevelopmentSettings::UpdateImmediatelyDestroyActivitiesOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Int32 data;
    global->GetInt32(resolver, ISettingsGlobal::ALWAYS_FINISH_ACTIVITIES, 0, &data);
    UpdateCheckBox(mImmediatelyDestroyActivities, data != 0);
}

void CDevelopmentSettings::UpdateAnimationScaleValue(
    /* [in] */ Int32 which,
    /* [in] */ IListPreference* pref)
{
    // try {
    Float scale;
    mWindowManager->GetAnimationScale(which, &scale);
    if (scale != 1) {
        mHaveDebugSettings = TRUE;
    }

    AutoPtr< ArrayOf<ICharSequence*> > values;
    pref->GetEntryValues((ArrayOf<ICharSequence*>**)&values);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    pref->GetEntries((ArrayOf<ICharSequence*>**)&entries);

    IPreference* preference = IPreference::Probe(pref);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        Float val = StringUtils::ParseFloat(Object::ToString((*values)[i]));
        if (scale <= val) {
            pref->SetValueIndex(i);
            preference->SetSummary((*entries)[i]);
            return;
        }
    }
    pref->SetValueIndex(values->GetLength() -1);
    preference->SetSummary((*entries)[0]);
    // } catch (RemoteException e) {
    // }
}

void CDevelopmentSettings::UpdateAnimationScaleOptions()
{
    UpdateAnimationScaleValue(0, mWindowAnimationScale);
    UpdateAnimationScaleValue(1, mTransitionAnimationScale);
    UpdateAnimationScaleValue(2, mAnimatorDurationScale);
}

void CDevelopmentSettings::WriteAnimationScaleOption(
    /* [in] */ Int32 which,
    /* [in] */ IListPreference* pref,
    /* [in] */ IInterface* newValue)
{
    // try {
    Float scale = newValue != NULL ? StringUtils::ParseFloat(Object::ToString(newValue)) : 1;
    mWindowManager->SetAnimationScale(which, scale);
    UpdateAnimationScaleValue(which, pref);
    // } catch (RemoteException e) {
    // }
}

void CDevelopmentSettings::UpdateOverlayDisplayDevicesOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    String value;
    global->GetString(resolver, ISettingsGlobal::OVERLAY_DISPLAY_DEVICES, &value);
    if (value.IsNull()) {
        value = "";
    }

    AutoPtr< ArrayOf<ICharSequence*> > values;
    mOverlayDisplayDevices->GetEntryValues((ArrayOf<ICharSequence*>**)&values);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    mOverlayDisplayDevices->GetEntries((ArrayOf<ICharSequence*>**)&entries);

    IPreference* overlayDisplayDevices = IPreference::Probe(mOverlayDisplayDevices);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        if ((*values)[i] != NULL && value.Equals(Object::ToString((*values)[i]))) {
            mOverlayDisplayDevices->SetValueIndex(i);
            overlayDisplayDevices->SetSummary((*entries)[i]);
            return;
        }
    }
    mOverlayDisplayDevices->SetValueIndex(0);
    overlayDisplayDevices->SetSummary((*entries)[0]);
}

void CDevelopmentSettings::WriteOverlayDisplayDevicesOptions(
    /* [in] */ IInterface* newValue)
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsGlobal> global;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
    Boolean res;
    global->PutString(resolver, ISettingsGlobal::OVERLAY_DISPLAY_DEVICES, Object::ToString(newValue), &res);
    UpdateOverlayDisplayDevicesOptions();
}

void CDevelopmentSettings::UpdateOpenGLTracesOptions()
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    String value;
    sp->Get(OPENGL_TRACES_PROPERTY, &value);
    if (value.IsNull()) {
        value = "";
    }

    AutoPtr< ArrayOf<ICharSequence*> > values;
    mOpenGLTraces->GetEntryValues((ArrayOf<ICharSequence*>**)&values);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    mOpenGLTraces->GetEntries((ArrayOf<ICharSequence*>**)&entries);

    IPreference* openGLTraces = IPreference::Probe(mOpenGLTraces);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        if ((*values)[i] != NULL && value.Equals(Object::ToString((*values)[i]))) {
            mOpenGLTraces->SetValueIndex(i);
            openGLTraces->SetSummary((*entries)[i]);
            return;
        }
    }
    mOpenGLTraces->SetValueIndex(0);
    openGLTraces->SetSummary((*entries)[0]);
}

void CDevelopmentSettings::WriteOpenGLTracesOptions(
    /* [in] */ IInterface* newValue)
{
    AutoPtr<ISystemProperties> sp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
    sp->Set(OPENGL_TRACES_PROPERTY, newValue == NULL ? String("") : Object::ToString(newValue));
    PokeSystemProperties();
    UpdateOpenGLTracesOptions();
}

void CDevelopmentSettings::UpdateAppProcessLimitOptions()
{
    // try {
    Int32 limit;
    ActivityManagerNative::GetDefault()->GetProcessLimit(&limit);

    AutoPtr< ArrayOf<ICharSequence*> > values;
    mAppProcessLimit->GetEntryValues((ArrayOf<ICharSequence*>**)&values);
    AutoPtr< ArrayOf<ICharSequence*> > entries;
    mAppProcessLimit->GetEntries((ArrayOf<ICharSequence*>**)&entries);

    IPreference* appProcessLimit = IPreference::Probe(mAppProcessLimit);
    for (Int32 i = 0; i < values->GetLength(); i++) {
        Int32 val = StringUtils::ParseInt32(Object::ToString((*values)[i]));
        if (val >= limit) {
            if (i != 0) {
                mHaveDebugSettings = TRUE;
            }
            mAppProcessLimit->SetValueIndex(i);
            appProcessLimit->SetSummary((*entries)[i]);
            return;
        }
    }
    mAppProcessLimit->SetValueIndex(0);
    appProcessLimit->SetSummary((*entries)[0]);
    // } catch (RemoteException e) {
    // }
}

void CDevelopmentSettings::WriteAppProcessLimitOptions(
    /* [in] */ IInterface* newValue)
{
    // try {
    Int32 limit = newValue != NULL ? StringUtils::ParseInt32(Object::ToString(newValue)) : -1;
    ActivityManagerNative::GetDefault()->SetProcessLimit(limit);
    UpdateAppProcessLimitOptions();
    // } catch (RemoteException e) {
    // }
}

void CDevelopmentSettings::WriteShowAllANRsOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Boolean res;
    secure->PutInt32(resolver, ISettingsSecure::ANR_SHOW_BACKGROUND,
            (ITwoStatePreference::Probe(mShowAllANRs)->IsChecked(&res), res) ? 1 : 0, &res);
}

void CDevelopmentSettings::UpdateShowAllANRsOptions()
{
    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IContentResolver> resolver;
    IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

    AutoPtr<ISettingsSecure> secure;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
    Int32 data;
    secure->GetInt32(resolver, ISettingsSecure::ANR_SHOW_BACKGROUND, 0, &data);
    UpdateCheckBox(mShowAllANRs, data != 0);
}

ECode CDevelopmentSettings::OnSwitchChanged(
    /* [in] */ ISwitch* switchView,
    /* [in] */ Boolean isChecked)
{
    if (switchView != ISwitch::Probe(((CSwitchBar*)mSwitchBar.Get())->GetSwitch())) {
        return NOERROR;
    }
    if (isChecked != mLastEnabledState) {
        if (isChecked) {
            mDialogClicked = FALSE;
            if (mEnableDialog != NULL) DismissDialogs();
            AutoPtr<IActivity> _activity;
            GetActivity((IActivity**)&_activity);
            IContext* activity = IContext::Probe(_activity);
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(activity, (IAlertDialogBuilder**)&builder);
            AutoPtr<IResources> resources;
            activity->GetResources((IResources**)&resources);
            String str;
            resources->GetString(R::string::dev_settings_warning_message, &str);
            builder->SetMessage(CoreUtils::Convert(str));
            builder->SetTitle(R::string::dev_settings_warning_title);
            builder->SetPositiveButton(Elastos::Droid::R::string::yes, this);
            builder->SetNegativeButton(Elastos::Droid::R::string::no, this);
            AutoPtr<IAlertDialog> dialog;
            builder->Show((IAlertDialog**)&dialog);
            mEnableDialog = IDialog::Probe(dialog);
            mEnableDialog->SetOnDismissListener(mListener);
        }
        else {
            ResetDangerousOptions();
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            AutoPtr<IContentResolver> resolver;
            IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

            AutoPtr<ISettingsGlobal> global;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
            Boolean res;
            global->PutInt32(resolver, ISettingsGlobal::DEVELOPMENT_SETTINGS_ENABLED, 0, &res);
            mLastEnabledState = isChecked;
            SetPrefsEnabledState(mLastEnabledState);
        }
    }
    return NOERROR;
}

ECode CDevelopmentSettings::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* data)
{
    if (requestCode == RESULT_DEBUG_APP) {
        if (resultCode == Activity::RESULT_OK) {
            data->GetAction(&mDebugApp);
            WriteDebuggerOptions();
            UpdateDebuggerOptions();
        }
    }
    else {
        SettingsPreferenceFragment::OnActivityResult(requestCode, resultCode, data);
    }
    return NOERROR;
}

ECode CDevelopmentSettings::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (Utils::IsMonkeyRunning()) {
        return NOERROR;
    }

    if (preference == IPreference::Probe(mEnableAdb)) {
        Boolean res;
        if (ITwoStatePreference::Probe(mEnableAdb)->IsChecked(&res), res) {
            mDialogClicked = FALSE;
            if (mAdbDialog != NULL) DismissDialogs();
            AutoPtr<IActivity> _activity;
            GetActivity((IActivity**)&_activity);
            IContext* activity = IContext::Probe(_activity);
            AutoPtr<IAlertDialogBuilder> builder;
            CAlertDialogBuilder::New(activity, (IAlertDialogBuilder**)&builder);
            AutoPtr<IResources> resources;
            activity->GetResources((IResources**)&resources);
            String str;
            resources->GetString(R::string::adb_warning_message, &str);
            builder->SetMessage(CoreUtils::Convert(str));
            builder->SetTitle(R::string::adb_warning_title);
            builder->SetPositiveButton(Elastos::Droid::R::string::yes, this);
            builder->SetNegativeButton(Elastos::Droid::R::string::no, this);
            AutoPtr<IAlertDialog> dialog;
            builder->Show((IAlertDialog**)&dialog);
            mAdbDialog = IDialog::Probe(dialog);
            mAdbDialog->SetOnDismissListener(mListener);
        }
        else {
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            AutoPtr<IContentResolver> resolver;
            IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

            AutoPtr<ISettingsGlobal> global;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
            Boolean res;
            global->PutInt32(resolver, ISettingsGlobal::ADB_ENABLED, 0, &res);
            IPreference::Probe(mVerifyAppsOverUsb)->SetEnabled(FALSE);
            ITwoStatePreference::Probe(mVerifyAppsOverUsb)->SetChecked(FALSE);
            UpdateBugreportOptions();
        }
    }
    else if (preference == mClearAdbKeys.Get()) {
        if (mAdbKeysDialog != NULL) DismissDialogs();
        AutoPtr<IActivity> _activity;
        GetActivity((IActivity**)&_activity);
        AutoPtr<IAlertDialogBuilder> builder;
        CAlertDialogBuilder::New(IContext::Probe(_activity), (IAlertDialogBuilder**)&builder);

        builder->SetMessage(R::string::adb_keys_warning_message);
        builder->SetPositiveButton(Elastos::Droid::R::string::ok, this);
        builder->SetNegativeButton(Elastos::Droid::R::string::cancel, NULL);
        AutoPtr<IAlertDialog> dialog;
        builder->Show((IAlertDialog**)&dialog);
        mAdbKeysDialog = IDialog::Probe(dialog);
    }
    else if (preference == IPreference::Probe(mEnableTerminal)) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IPackageManager> pm;
        IContext::Probe(activity)->GetPackageManager((IPackageManager**)&pm);
        Boolean res;
        ITwoStatePreference::Probe(mEnableTerminal)->IsChecked(&res);
        pm->SetApplicationEnabledSetting(TERMINAL_APP_PACKAGE,
                res ? IPackageManager::COMPONENT_ENABLED_STATE_ENABLED
                        : IPackageManager::COMPONENT_ENABLED_STATE_DEFAULT, 0);
    }
    else if (preference == IPreference::Probe(mBugreportInPower)) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IContentResolver> resolver;
        IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        Boolean res;
        ITwoStatePreference::Probe(mBugreportInPower)->IsChecked(&res);
        secure->PutInt32(resolver,
                ISettingsSecure::BUGREPORT_IN_POWER_MENU,
                res ? 1 : 0, &res);
    }
    else if (preference == IPreference::Probe(mKeepScreenOn)) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IContentResolver> resolver;
        IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

        AutoPtr<ISettingsGlobal> global;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
        Boolean res;
        ITwoStatePreference::Probe(mKeepScreenOn)->IsChecked(&res);
        global->PutInt32(resolver,
                ISettingsGlobal::STAY_ON_WHILE_PLUGGED_IN,
                res ? (IBatteryManager::BATTERY_PLUGGED_AC | IBatteryManager::BATTERY_PLUGGED_USB) : 0, &res);
    }
    else if (preference == IPreference::Probe(mBtHciSnoopLog)) {
        WriteBtHciSnoopLogOptions();
    }
    else if (preference == IPreference::Probe(mEnableOemUnlock)) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        Boolean res;
        ITwoStatePreference::Probe(mEnableOemUnlock)->IsChecked(&res);
        Utils::SetOemUnlockEnabled(IContext::Probe(activity), res);
    }
    else if (preference == IPreference::Probe(mAllowMockLocation)) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IContentResolver> resolver;
        IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

        AutoPtr<ISettingsSecure> secure;
        CSettingsSecure::AcquireSingleton((ISettingsSecure**)&secure);
        Boolean res;
        ITwoStatePreference::Probe(mAllowMockLocation)->IsChecked(&res);
        secure->PutInt32(resolver,
                ISettingsSecure::ALLOW_MOCK_LOCATION,
                res ? 1 : 0, &res);
    }
    else if (preference == IPreference::Probe(mDebugViewAttributes)) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IContentResolver> resolver;
        IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

        AutoPtr<ISettingsGlobal> global;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
        Boolean res;
        ITwoStatePreference::Probe(mDebugViewAttributes)->IsChecked(&res);
        global->PutInt32(resolver,
                ISettingsGlobal::DEBUG_VIEW_ATTRIBUTES,
                res ? 1 : 0, &res);
    }
    else if (preference == mDebugAppPref.Get()) {
        AutoPtr<IActivity> activity;
        GetActivity((IActivity**)&activity);
        AutoPtr<IIntent> intent;
        CIntent::New(IContext::Probe(activity), ECLSID_CAppPicker, (IIntent**)&intent);
        StartActivityForResult(intent, RESULT_DEBUG_APP);
    }
    else if (preference == IPreference::Probe(mWaitForDebugger)) {
        WriteDebuggerOptions();
    }
    else if (preference == IPreference::Probe(mVerifyAppsOverUsb)) {
        WriteVerifyAppsOverUsbOptions();
    }
    else if (preference == IPreference::Probe(mStrictMode)) {
        WriteStrictModeVisualOptions();
    }
    else if (preference == IPreference::Probe(mPointerLocation)) {
        WritePointerLocationOptions();
    }
    else if (preference == IPreference::Probe(mShowTouches)) {
        WriteShowTouchesOptions();
    }
    else if (preference == IPreference::Probe(mShowScreenUpdates)) {
        WriteShowUpdatesOption();
    }
    else if (preference == IPreference::Probe(mDisableOverlays)) {
        WriteDisableOverlaysOption();
    }
    else if (preference == IPreference::Probe(mShowCpuUsage)) {
        WriteCpuUsageOptions();
    }
    else if (preference == IPreference::Probe(mImmediatelyDestroyActivities)) {
        WriteImmediatelyDestroyActivitiesOptions();
    }
    else if (preference == IPreference::Probe(mShowAllANRs)) {
        WriteShowAllANRsOptions();
    }
    else if (preference == IPreference::Probe(mForceHardwareUi)) {
        WriteHardwareUiOptions();
    }
    else if (preference == IPreference::Probe(mForceMsaa)) {
        WriteMsaaOptions();
    }
    else if (preference == IPreference::Probe(mShowHwScreenUpdates)) {
        WriteShowHwScreenUpdatesOptions();
    }
    else if (preference == IPreference::Probe(mShowHwLayersUpdates)) {
        WriteShowHwLayersUpdatesOptions();
    }
    else if (preference == IPreference::Probe(mDebugLayout)) {
        WriteDebugLayoutOptions();
    }
    else if (preference == IPreference::Probe(mForceRtlLayout)) {
        WriteForceRtlOptions();
    }
    else if (preference == IPreference::Probe(mWifiDisplayCertification)) {
        WriteWifiDisplayCertificationOptions();
    }
    else if (preference == IPreference::Probe(mWifiVerboseLogging)) {
        WriteWifiVerboseLoggingOptions();
    }
    else if (preference == IPreference::Probe(mWifiAggressiveHandover)) {
        WriteWifiAggressiveHandoverOptions();
    }
    else if (preference == IPreference::Probe(mWifiAllowScansWithTraffic)) {
        WriteWifiAllowScansWithTrafficOptions();
    }
    else if (preference == IPreference::Probe(mUseNuplayer)) {
        WriteUseNuplayerOptions();
    }
    else if (preference == IPreference::Probe(mUSBAudio)) {
        WriteUSBAudioOptions();
    }
    else {
        return SettingsPreferenceFragment::OnPreferenceTreeClick(preferenceScreen, preference, result);
    }

    return NOERROR;
}

ECode CDevelopmentSettings::OnPreferenceChange(
    /* [in] */ IPreference* preference,
    /* [in] */ IInterface* newValue,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    String prefStr;
    preference->GetKey(&prefStr);
    if (HDCP_CHECKING_KEY.Equals(prefStr)) {
        AutoPtr<ISystemProperties> sp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sp);
        sp->Set(HDCP_CHECKING_PROPERTY, Object::ToString(newValue));
        UpdateHdcpValues();
        PokeSystemProperties();
        *result = TRUE;
        return NOERROR;
    }
    else if (preference == IPreference::Probe(mLogdSize)) {
        WriteLogdSizeOption(newValue);
        *result = TRUE;
        return NOERROR;
    }
    else if (preference == IPreference::Probe(mWindowAnimationScale)) {
        WriteAnimationScaleOption(0, mWindowAnimationScale, newValue);
        *result = TRUE;
        return NOERROR;
    }
    else if (preference == IPreference::Probe(mTransitionAnimationScale)) {
        WriteAnimationScaleOption(1, mTransitionAnimationScale, newValue);
        *result = TRUE;
        return NOERROR;
    }
    else if (preference == IPreference::Probe(mAnimatorDurationScale)) {
        WriteAnimationScaleOption(2, mAnimatorDurationScale, newValue);
        *result = TRUE;
        return NOERROR;
    }
    else if (preference == IPreference::Probe(mOverlayDisplayDevices)) {
        WriteOverlayDisplayDevicesOptions(newValue);
        *result = TRUE;
        return NOERROR;
    }
    else if (preference == IPreference::Probe(mOpenGLTraces)) {
        WriteOpenGLTracesOptions(newValue);
        *result = TRUE;
        return NOERROR;
    }
    else if (preference == IPreference::Probe(mTrackFrameTime)) {
        WriteTrackFrameTimeOptions(newValue);
        *result = TRUE;
        return NOERROR;
    }
    else if (preference == IPreference::Probe(mDebugHwOverdraw)) {
        WriteDebugHwOverdrawOptions(newValue);
        *result = TRUE;
        return NOERROR;
    }
    else if (preference == IPreference::Probe(mShowNonRectClip)) {
        WriteShowNonRectClipOptions(newValue);
        *result = TRUE;
        return NOERROR;
    }
    else if (preference == IPreference::Probe(mAppProcessLimit)) {
        WriteAppProcessLimitOptions(newValue);
        *result = TRUE;
        return NOERROR;
    }
    else if (preference == IPreference::Probe(mSimulateColorSpace)) {
        WriteSimulateColorSpace(newValue);
        *result = TRUE;
        return NOERROR;
    }
    return NOERROR;
}

void CDevelopmentSettings::DismissDialogs()
{
    if (mAdbDialog != NULL) {
        IDialogInterface::Probe(mAdbDialog)->Dismiss();
        mAdbDialog = NULL;
    }
    if (mAdbKeysDialog != NULL) {
        IDialogInterface::Probe(mAdbKeysDialog)->Dismiss();
        mAdbKeysDialog = NULL;
    }
    if (mEnableDialog != NULL) {
        IDialogInterface::Probe(mEnableDialog)->Dismiss();
        mEnableDialog = NULL;
    }
}

ECode CDevelopmentSettings::OnClick(
    /* [in] */ IDialogInterface* dialog,
    /* [in] */ Int32 which)
{
    if (dialog == IDialogInterface::Probe(mAdbDialog)) {
        if (which == IDialogInterface::BUTTON_POSITIVE) {
            mDialogClicked = TRUE;
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            AutoPtr<IContentResolver> resolver;
            IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

            AutoPtr<ISettingsGlobal> global;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
            Boolean res;
            global->PutInt32(resolver, ISettingsGlobal::ADB_ENABLED, 1, &res);
            IPreference::Probe(mVerifyAppsOverUsb)->SetEnabled(TRUE);
            UpdateVerifyAppsOverUsbOptions();
            UpdateBugreportOptions();
        }
        else {
            // Reset the toggle
            ITwoStatePreference::Probe(mEnableAdb)->SetChecked(FALSE);
        }
    }
    else if (dialog == IDialogInterface::Probe(mAdbKeysDialog)) {
        if (which == IDialogInterface::BUTTON_POSITIVE) {
            // try {
            AutoPtr<IBinder> b = IBinder::Probe(ServiceManager::GetService(IContext::USB_SERVICE));
            IIUsbManager* service = IIUsbManager::Probe(b);
            ECode ec = service->ClearUsbDebuggingKeys();
            if (FAILED(ec)) {
                Logger::E(TAG, "Unable to clear adb keys 0x%08x", ec);
            }
            // } catch (RemoteException e) {
            //     Logger::E(TAG, "Unable to clear adb keys", e);
            // }
        }
    }
    else if (dialog == IDialogInterface::Probe(mEnableDialog)) {
        if (which == IDialogInterface::BUTTON_POSITIVE) {
            AutoPtr<IActivity> activity;
            GetActivity((IActivity**)&activity);
            AutoPtr<IContentResolver> resolver;
            IContext::Probe(activity)->GetContentResolver((IContentResolver**)&resolver);

            mDialogClicked = TRUE;
            AutoPtr<ISettingsGlobal> global;
            CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&global);
            Boolean res;
            global->PutInt32(resolver, ISettingsGlobal::DEVELOPMENT_SETTINGS_ENABLED, 1, &res);
            mLastEnabledState = TRUE;
            SetPrefsEnabledState(mLastEnabledState);
        }
        else {
            // Reset the toggle
            ((CSwitchBar*)mSwitchBar.Get())->SetChecked(FALSE);
        }
    }
    return NOERROR;
}

ECode CDevelopmentSettings::OnDismiss(
    /* [in] */ IDialogInterface* dialog)
{
    // Assuming that onClick gets called first
    if (dialog == IDialogInterface::Probe(mAdbDialog)) {
        if (!mDialogClicked) {
            ITwoStatePreference::Probe(mEnableAdb)->SetChecked(FALSE);
        }
        mAdbDialog = NULL;
    }
    else if (dialog == IDialogInterface::Probe(mEnableDialog)) {
        if (!mDialogClicked) {
            ((CSwitchBar*)mSwitchBar.Get())->SetChecked(FALSE);
        }
        mEnableDialog = NULL;
    }
    return NOERROR;
}

ECode CDevelopmentSettings::OnDestroy()
{
    DismissDialogs();
    return SettingsPreferenceFragment::OnDestroy();
}

void CDevelopmentSettings::PokeSystemProperties()
{
    if (!mDontPokeProperties) {
        //noinspection unchecked
        AutoPtr<AsyncTask> task = new SystemPropPoker();
        AutoPtr< ArrayOf<IInterface*> > args;
        task->Execute(args);
    }
}

Boolean CDevelopmentSettings::IsPackageInstalled(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName)
{
    AutoPtr<IPackageManager> pm;
    context->GetPackageManager((IPackageManager**)&pm);
    AutoPtr<IPackageInfo> info;
    ECode ec = pm->GetPackageInfo(packageName, 0, (IPackageInfo**)&info);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        return FALSE;

    }
    return info != NULL;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
