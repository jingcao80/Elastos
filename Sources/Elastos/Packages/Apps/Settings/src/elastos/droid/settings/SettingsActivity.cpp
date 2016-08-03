
#include "Elastos.Droid.Transition.h"
#include "elastos/droid/settings/SettingsActivity.h"
#include "elastos/droid/settings/DevelopmentSettings.h"
#include "elastos/droid/settings/HomeSettings.h"
#include "elastos/droid/settings/dashboard/CDashboardCategory.h"
#include "elastos/droid/settings/dashboard/NoHomeDialogFragment.h"
#include "elastos/droid/settings/dashboard/CSearchResultsSummary.h"
#include "elastos/droid/settings/search/Index.h"
#include "elastos/droid/settings/Utils.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include "elastos/droid/internal/utility/XmlUtils.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/Xml.h"
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include "R.h"

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::CFragmentHelper;
using Elastos::Droid::App::IFragmentHelper;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::App::EIID_IFragmentManagerOnBackStackChangedListener;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::EIID_ISharedPreferencesOnSharedPreferenceChangeListener;
using Elastos::Droid::Content::ISharedPreferencesEditor;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::XmlUtils;
// using Elastos::Droid::Nfc::INfcAdapter;
// using Elastos::Droid::Nfc::CNfcAdapterHelper;
using Elastos::Droid::Nfc::INfcAdapterHelper;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Os::IUserManagerHelper;
using Elastos::Droid::Os::CUserManagerHelper;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Preference::EIID_IPreferenceFragmentOnPreferenceStartFragmentCallback;
using Elastos::Droid::Preference::EIID_IPreferenceManagerOnPreferenceTreeClickListener;
using Elastos::Droid::Settings::Dashboard::CDashboardCategory;
using Elastos::Droid::Settings::Dashboard::CSearchResultsSummary;
using Elastos::Droid::Settings::Dashboard::NoHomeDialogFragment;
using Elastos::Droid::Settings::Search::Index;
using Elastos::Droid::Settings::EIID_IButtonBarHandler;
using Elastos::Droid::Transition::ITransitionManagerHelper;
// using Elastos::Droid::Transition::CTransitionManagerHelper;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IOnActionExpandListener;
using Elastos::Droid::Widget::EIID_ISearchViewOnCloseListener;
using Elastos::Droid::Widget::EIID_ISearchViewOnQueryTextListener;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Settings {

const String SettingsActivity::EXTRA_SHOW_FRAGMENT(":settings:show_fragment");

const String SettingsActivity::EXTRA_SHOW_FRAGMENT_ARGUMENTS(":settings:show_fragment_args");

const String SettingsActivity::EXTRA_FRAGMENT_ARG_KEY(":settings:fragment_args_key");

const String SettingsActivity::BACK_STACK_PREFS(":settings:prefs");

const String SettingsActivity::EXTRA_SHOW_FRAGMENT_TITLE(":settings:show_fragment_title");
const String SettingsActivity::EXTRA_SHOW_FRAGMENT_TITLE_RESID(":settings:show_fragment_title_resid");
const String SettingsActivity::EXTRA_SHOW_FRAGMENT_AS_SHORTCUT(":settings:show_fragment_as_shortcut");

const String SettingsActivity::EXTRA_SHOW_FRAGMENT_AS_SUBSETTING(":settings:show_fragment_as_subsetting");

const String SettingsActivity::EXTRA_PREFS_SHOW_BUTTON_BAR("extra_prefs_show_button_bar");

const String SettingsActivity::EXTRA_PREFS_SET_NEXT_TEXT("extra_prefs_set_next_text");
const String SettingsActivity::EXTRA_PREFS_SET_BACK_TEXT("extra_prefs_set_back_text");

const String SettingsActivity::TAG("SettingsActivity");

const String SettingsActivity::SAVE_KEY_CATEGORIES(":settings:categories");
const String SettingsActivity::SAVE_KEY_SEARCH_MENU_EXPANDED(":settings:search_menu_expanded");
const String SettingsActivity::SAVE_KEY_SEARCH_QUERY(":settings:search_query");
const String SettingsActivity::SAVE_KEY_SHOW_HOME_AS_UP(":settings:show_home_as_up");
const String SettingsActivity::SAVE_KEY_SHOW_SEARCH(":settings:show_search");
const String SettingsActivity::SAVE_KEY_HOME_ACTIVITIES_COUNT(":settings:home_activities_count");

const String SettingsActivity::EXTRA_PREFS_SHOW_SKIP("extra_prefs_show_skip");

const String SettingsActivity::META_DATA_KEY_FRAGMENT_CLASS("com.android.settings.FRAGMENT_CLASS");

const String SettingsActivity::EXTRA_UI_OPTIONS("settings:ui_options");

const String SettingsActivity::EMPTY_QUERY("");

Boolean SettingsActivity::sShowNoHomeNotice = FALSE;

static AutoPtr< ArrayOf<String> > InitENTRY_FRAGMENTS()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(61);
    (*args)[0] = "Elastos.Droid.Settings.CWirelessSettings";
    (*args)[1] = "Elastos.Droid.Settings.Wifi.CWifiSettings";
    (*args)[2] = "Elastos.Droid.Settings.Wifi.CAdvancedWifiSettings";
    (*args)[3] = "Elastos.Droid.Settings.Wifi.CSavedAccessPointsWifiSettings";
    (*args)[4] = "Elastos.Droid.Settings.Bluetooth.BluetoothSettings";
    (*args)[5] = "Elastos.Droid.Settings.Sim.SimSettings";
    (*args)[6] = "Elastos.Droid.Settings.TetherSettings";
    (*args)[7] = "Elastos.Droid.Settings.Wifi.P2p.CWifiP2pSettings";
    (*args)[8] = "Elastos.Droid.Settings.Vpn2.VpnSettings";
    (*args)[9] = "Elastos.Droid.Settings.CDateTimeSettings";
    (*args)[10] = "Elastos.Droid.Settings.CLocalePicker";
    (*args)[11] = "Elastos.Droid.Settings.Inputmethod.CInputMethodAndLanguageSettings";
    (*args)[12] = "Elastos.Droid.Settings.Voice.VoiceInputSettings";
    (*args)[13] = "Elastos.Droid.Settings.Inputmethod.CSpellCheckersSettings";
    (*args)[14] = "Elastos.Droid.Settings.Inputmethod.CUserDictionaryList";
    (*args)[15] = "Elastos.Droid.Settings.UserDictionarySettings";
    (*args)[16] = "Elastos.Droid.Settings.HomeSettings";
    (*args)[17] = "Elastos.Droid.Settings.DisplaySettings";
    (*args)[18] = "Elastos.Droid.Settings.DeviceInfoSettings";
    (*args)[19] = "Elastos.Droid.Settings.Applications.ManageApplications";
    (*args)[20] = "Elastos.Droid.Settings.Applications.ProcessStatsUi";
    (*args)[21] = "Elastos.Droid.Settings.Notification.NotificationStation";
    (*args)[22] = "Elastos.Droid.Settings.Location.LocationSettings";
    (*args)[23] = "Elastos.Droid.Settings.CSecuritySettings";
    (*args)[24] = "Elastos.Droid.Settings.UsageAccessSettings";
    (*args)[25] = "Elastos.Droid.Settings.PrivacySettings";
    (*args)[26] = "Elastos.Droid.Settings.DeviceAdminSettings";
    (*args)[27] = "Elastos.Droid.Settings.Accessibility.AccessibilitySettings";
    (*args)[28] = "Elastos.Droid.Settings.Accessibility.CaptionPropertiesFragment";
    (*args)[29] = "Elastos.Droid.Settings.Accessibility.ToggleDaltonizerPreferenceFragment";
    (*args)[30] = "Elastos.Droid.Settings.Tts.TextToSpeechSettings";
    (*args)[31] = "Elastos.Droid.Settings.Deviceinfo.Memory";
    (*args)[32] = "Elastos.Droid.Settings.DevelopmentSettings";
    (*args)[33] = "Elastos.Droid.Settings.Deviceinfo.UsbSettings";
    (*args)[34] = "Elastos.Droid.Settings.Nfc.AndroidBeam";
    (*args)[35] = "Elastos.Droid.Settings.Wfd.WifiDisplaySettings";
    (*args)[36] = "Elastos.Droid.Settings.Fuelgauge.PowerUsageSummary";
    (*args)[37] = "Elastos.Droid.Settings.Accounts.AccountSyncSettings";
    (*args)[38] = "Elastos.Droid.Settings.Accounts.AccountSettings";
    (*args)[39] = "Elastos.Droid.Settings.CCryptKeeperSettings";
    (*args)[40] = "Elastos.Droid.Settings.DataUsageSummary";
    (*args)[41] = "Elastos.Droid.Settings.DreamSettings";
    (*args)[42] = "Elastos.Droid.Settings.Users.UserSettings";
    (*args)[43] = "Elastos.Droid.Settings.Notification.NotificationAccessSettings";
    (*args)[44] = "Elastos.Droid.Settings.Notification.ConditionProviderSettings";
    (*args)[45] = "Elastos.Droid.Settings.Print.PrintSettingsFragment";
    (*args)[46] = "Elastos.Droid.Settings.Print.PrintJobSettingsFragment";
    (*args)[47] = "Elastos.Droid.Settings.TrustedCredentialsSettings";
    (*args)[48] = "Elastos.Droid.Settings.Nfc.PaymentSettings";
    (*args)[49] = "Elastos.Droid.Settings.Inputmethod.KeyboardLayoutPickerFragment";
    (*args)[50] = "Elastos.Droid.Settings.Notification.ZenModeSettings";
    (*args)[51] = "Elastos.Droid.Settings.Notification.NotificationSettings";
    (*args)[52] = "Elastos.Droid.Settings.ChooseLockPassword.ChooseLockPasswordFragment";
    (*args)[53] = "Elastos.Droid.Settings.ChooseLockPattern.ChooseLockPatternFragment";
    (*args)[54] = "Elastos.Droid.Settings.Applications.InstalledAppDetails";
    (*args)[55] = "Elastos.Droid.Settings.Fuelgauge.BatterySaverSettings";
    (*args)[56] = "Elastos.Droid.Settings.Notification.NotificationAppList";
    (*args)[57] = "Elastos.Droid.Settings.Notification.AppNotificationSettings";
    (*args)[58] = "Elastos.Droid.Settings.Notification.OtherSoundSettings";
    (*args)[59] = "Elastos.Droid.Settings.Quicklaunch.QuickLaunchSettings";
    (*args)[60] = "Elastos.Droid.Settings.ApnSettings";

    return args;
}

const AutoPtr<ArrayOf<String> > SettingsActivity::ENTRY_FRAGMENTS = InitENTRY_FRAGMENTS();

static AutoPtr< ArrayOf<String> > InitLIKE_SHORTCUT_INTENT_ACTION_ARRAY()
{
    AutoPtr< ArrayOf<String> > args = ArrayOf<String>::Alloc(1);
    (*args)[0] = "android.settings.APPLICATION_DETAILS_SETTINGS";
    return args;
}

const AutoPtr< ArrayOf<String> > SettingsActivity::LIKE_SHORTCUT_INTENT_ACTION_ARRAY = InitLIKE_SHORTCUT_INTENT_ACTION_ARRAY();

const String SettingsActivity::MSG_DATA_FORCE_REFRESH("msg_data_force_refresh");
const Int32 SettingsActivity::MSG_BUILD_CATEGORIES;

//===============================================================================
//                  SettingsActivity::BatteryInfoReceiver
//===============================================================================

SettingsActivity::BatteryInfoReceiver::BatteryInfoReceiver(
    /* [in] */ SettingsActivity* host)
    : mHost(host)
{}

SettingsActivity::BatteryInfoReceiver::~BatteryInfoReceiver()
{}

ECode SettingsActivity::BatteryInfoReceiver::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (IIntent::ACTION_BATTERY_CHANGED.Equals(action)) {
        Boolean batteryPresent = Utils::IsBatteryPresent(intent);

        if (mHost->mBatteryPresent != batteryPresent) {
            mHost->mBatteryPresent = batteryPresent;
            mHost->InvalidateCategories(TRUE);
        }
    }
    return NOERROR;
}

//===============================================================================
//                  SettingsActivity::BuildCategoriesHandler
//===============================================================================

SettingsActivity::BuildCategoriesHandler::BuildCategoriesHandler(
    /* [in] */ SettingsActivity* host)
    : mHost(host)
{}

SettingsActivity::BuildCategoriesHandler::~BuildCategoriesHandler()
{}

ECode SettingsActivity::BuildCategoriesHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what) {
        case MSG_BUILD_CATEGORIES: {
            AutoPtr<IBundle> data;
            msg->GetData((IBundle**)&data);
            Boolean forceRefresh;
            data->GetBoolean(MSG_DATA_FORCE_REFRESH, &forceRefresh);
            if (forceRefresh) {
                mHost->BuildDashboardCategories(IList::Probe(mHost->mCategories));
            }
        } break;
    }
    return NOERROR;
}

//===============================================================================
//                  SettingsActivity::OnCreateOnClickListener
//===============================================================================

CAR_INTERFACE_IMPL(SettingsActivity::OnCreateOnClickListener, Object, IViewOnClickListener);

SettingsActivity::OnCreateOnClickListener::OnCreateOnClickListener(
    /* [in] */ SettingsActivity* host,
    /* [in] */ Int32 id)
    : mHost(host)
    , mId(id)
{}

SettingsActivity::OnCreateOnClickListener::~OnCreateOnClickListener()
{}

ECode SettingsActivity::OnCreateOnClickListener::OnClick(
    /* [in] */ IView* v)
{
    switch (mId) {
        case 0: {
            AutoPtr<IIntent> intent;
            mHost->GetResultIntentData((IIntent**)&intent);
            mHost->SetResult(RESULT_CANCELED, intent);
            mHost->Finish();
            break;
        }
        case 1: {
            AutoPtr<IIntent> intent;
            mHost->GetResultIntentData((IIntent**)&intent);
            mHost->SetResult(RESULT_OK, intent);
            mHost->Finish();
            break;
        }
    }
    return NOERROR;
}

//===============================================================================
//                  SettingsActivity::OnResumeOnSharedPreferenceChangeListener
//===============================================================================

CAR_INTERFACE_IMPL(SettingsActivity::OnResumeOnSharedPreferenceChangeListener,
        Object, ISharedPreferencesOnSharedPreferenceChangeListener);

SettingsActivity::OnResumeOnSharedPreferenceChangeListener::OnResumeOnSharedPreferenceChangeListener(
    /* [in] */ SettingsActivity* host)
    : mHost(host)
{}

SettingsActivity::OnResumeOnSharedPreferenceChangeListener::~OnResumeOnSharedPreferenceChangeListener()
{}

ECode SettingsActivity::OnResumeOnSharedPreferenceChangeListener::OnSharedPreferenceChanged(
    /* [in] */ ISharedPreferences* sharedPreferences,
    /* [in] */ const String& key)
{
    mHost->InvalidateCategories(TRUE);
    return NOERROR;
}

//===============================================================================
//                  SettingsActivity
//===============================================================================

CAR_INTERFACE_IMPL_8(SettingsActivity, Activity, ISettingsActivity, IPreferenceManagerOnPreferenceTreeClickListener,
        IPreferenceFragmentOnPreferenceStartFragmentCallback, IButtonBarHandler,
        IFragmentManagerOnBackStackChangedListener, ISearchViewOnQueryTextListener,
        ISearchViewOnCloseListener, IOnActionExpandListener);

SettingsActivity::SettingsActivity()
    : mInitialTitleResId(0)
    , mBatteryPresent(TRUE)
    , mDisplayHomeAsUpEnabled(FALSE)
    , mDisplaySearch(FALSE)
    , mIsShowingDashboard(FALSE)
    , mIsShortcut(FALSE)
    , mSearchMenuItemExpanded(FALSE)
    , mNeedToRevertToInitialFragment(FALSE)
    , mHomeActivitiesCount(1)
{
    SETTINGS_FOR_RESTRICTED = ArrayOf<Int32>::Alloc(26);
    (*SETTINGS_FOR_RESTRICTED)[0] = R::id::wireless_section;
    (*SETTINGS_FOR_RESTRICTED)[1] = R::id::wifi_settings;
    (*SETTINGS_FOR_RESTRICTED)[2] = R::id::bluetooth_settings;
    (*SETTINGS_FOR_RESTRICTED)[3] = R::id::data_usage_settings;
    (*SETTINGS_FOR_RESTRICTED)[4] = R::id::sim_settings;
    (*SETTINGS_FOR_RESTRICTED)[5] = R::id::wireless_settings;
    (*SETTINGS_FOR_RESTRICTED)[6] = R::id::device_section;
    (*SETTINGS_FOR_RESTRICTED)[7] = R::id::notification_settings;
    (*SETTINGS_FOR_RESTRICTED)[8] = R::id::display_settings;
    (*SETTINGS_FOR_RESTRICTED)[9] = R::id::storage_settings;

    (*SETTINGS_FOR_RESTRICTED)[10] = R::id::application_settings;
    (*SETTINGS_FOR_RESTRICTED)[11] = R::id::battery_settings;
    (*SETTINGS_FOR_RESTRICTED)[12] = R::id::personal_section;
    (*SETTINGS_FOR_RESTRICTED)[13] = R::id::location_settings;
    (*SETTINGS_FOR_RESTRICTED)[14] = R::id::security_settings;
    (*SETTINGS_FOR_RESTRICTED)[15] = R::id::language_settings;
    (*SETTINGS_FOR_RESTRICTED)[16] = R::id::user_settings;
    (*SETTINGS_FOR_RESTRICTED)[17] = R::id::account_settings;
    (*SETTINGS_FOR_RESTRICTED)[18] = R::id::system_section;
    (*SETTINGS_FOR_RESTRICTED)[19] = R::id::date_time_settings;

    (*SETTINGS_FOR_RESTRICTED)[20] = R::id::about_settings;
    (*SETTINGS_FOR_RESTRICTED)[21] = R::id::accessibility_settings;
    (*SETTINGS_FOR_RESTRICTED)[22] = R::id::print_settings;
    (*SETTINGS_FOR_RESTRICTED)[23] = R::id::nfc_payment_settings;
    (*SETTINGS_FOR_RESTRICTED)[24] = R::id::home_settings;
    (*SETTINGS_FOR_RESTRICTED)[25] = R::id::dashboard;
}

SettingsActivity::~SettingsActivity()
{}

ECode SettingsActivity::constructor()
{
    Activity::constructor();

    mBatteryInfoReceiver = new BatteryInfoReceiver(this);

    mDynamicIndexableContentMonitor = new DynamicIndexableContentMonitor();

    CArrayList::New((IArrayList**)&mCategories);

    mHandler = new BuildCategoriesHandler(this);
    mHandler->constructor();

    return NOERROR;
}

ECode SettingsActivity::GetSwitchBar(
    /* [out] */ ISwitchBar** switchBar)
{
    VALIDATE_NOT_NULL(switchBar);
    *switchBar = mSwitchBar;
    REFCOUNT_ADD(*switchBar);
    return NOERROR;
}

ECode SettingsActivity::GetDashboardCategories(
    /* [in] */ Boolean forceRefresh,
    /* [out] */ IList** categories)
{
    VALIDATE_NOT_NULL(categories);

    Int32 size;
    mCategories->GetSize(&size);
    if (forceRefresh || size == 0) {
        BuildDashboardCategories(IList::Probe(mCategories));
    }

    *categories = IList::Probe(mCategories);
    REFCOUNT_ADD(*categories);
    return NOERROR;
}

ECode SettingsActivity::OnPreferenceStartFragment(
    /* [in] */ IPreferenceFragment* caller,
    /* [in] */ IPreference* pref,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Slogger::I(TAG, " >> enter SettingsActivity::OnPreferenceStartFragment");


    // Override the fragment title for Wallpaper settings
    Int32 titleRes;
    pref->GetTitleRes(&titleRes);
    String str;
    pref->GetFragment(&str);
    if (str.Equals("Elastos.Droid.Settings.WallpaperTypeSettings")) {
        titleRes = R::string::wallpaper_settings_fragment_title;
    }
    else if (str.Equals("Elastos.Droid.Settings.COwnerInfoSettings")
            && UserHandle::GetMyUserId() != IUserHandle::USER_OWNER) {
        AutoPtr<IUserManagerHelper> helper;
        CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
        AutoPtr<IUserManager> manager;
        helper->Get(IContext::Probe(this), (IUserManager**)&manager);
        Boolean res;
        if (manager->IsLinkedUser(&res), res) {
            titleRes = R::string::profile_info_settings_title;
        }
        else {
            titleRes = R::string::user_info_settings_title;
        }
    }
    AutoPtr<IBundle> bundle;
    pref->GetExtras((IBundle**)&bundle);
    AutoPtr<ICharSequence> cs;
    pref->GetTitle((ICharSequence**)&cs);
    StartPreferencePanel(str, bundle, titleRes, cs, NULL, 0);

    *result = TRUE;
    Slogger::I(TAG, " << leave SettingsActivity::OnPreferenceStartFragment");
    return NOERROR;
}

ECode SettingsActivity::OnPreferenceTreeClick(
    /* [in] */ IPreferenceScreen* preferenceScreen,
    /* [in] */ IPreference* preference,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = FALSE;
    return NOERROR;
}

void SettingsActivity::InvalidateCategories(
    /* [in] */ Boolean forceRefresh)
{
    Boolean res;
    if (mHandler->HasMessages(MSG_BUILD_CATEGORIES, &res), !res) {
        AutoPtr<IMessage> msg;
        CMessage::New((IMessage**)&msg);
        msg->SetWhat(MSG_BUILD_CATEGORIES);
        AutoPtr<IBundle> data;
        msg->GetData((IBundle**)&data);
        data->PutBoolean(MSG_DATA_FORCE_REFRESH, forceRefresh);
    }
}

ECode SettingsActivity::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    Activity::OnConfigurationChanged(newConfig);
    Index::GetInstance(this)->Update();
    return NOERROR;
}

ECode SettingsActivity::OnStart()
{
    Slogger::I(TAG, " >> enter SettingsActivity::OnStart");
    Activity::OnStart();

    if (mNeedToRevertToInitialFragment) {
        RevertToInitialFragment();
    }
    Slogger::I(TAG, " << leave SettingsActivity::OnStart");
    return NOERROR;
}

ECode SettingsActivity::OnCreateOptionsMenu(
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    Slogger::I(TAG, " >> enter OnCreateOptionsMenu ");

    if (!mDisplaySearch) {
        Slogger::I(TAG, " << leave OnCreateOptionsMenu 1 ");
        return NOERROR;
    }

    AutoPtr<IMenuInflater> inflater;
    GetMenuInflater((IMenuInflater**)&inflater);
    inflater->Inflate(R::menu::options_menu, menu);

    // Cache the search query (can be overriden by the OnQueryTextListener)
    const String query = mSearchQuery;

    mSearchMenuItem = NULL;
    menu->FindItem(R::id::search, (IMenuItem**)&mSearchMenuItem);
    AutoPtr<IView> view;
    mSearchMenuItem->GetActionView((IView**)&view);
    mSearchView = ISearchView::Probe(view);

    if (mSearchMenuItem == NULL || mSearchView == NULL) {
        Slogger::I(TAG, " << leave OnCreateOptionsMenu 2 ");
        return NOERROR;
    }

    if (mSearchResultsFragment != NULL) {
        ((CSearchResultsSummary*)mSearchResultsFragment.Get())->SetSearchView(mSearchView);
    }

    mSearchMenuItem->SetOnActionExpandListener(this);
    mSearchView->SetOnQueryTextListener(this);
    mSearchView->SetOnCloseListener(this);

    if (mSearchMenuItemExpanded) {
        Boolean res;
        mSearchMenuItem->ExpandActionView(&res);
    }
    mSearchView->SetQuery(CoreUtils::Convert(query), TRUE /* submit */);

    *result = TRUE;
    Slogger::I("SettingsActivity", " << leave OnCreateOptionsMenu ");
    return NOERROR;
}

Boolean SettingsActivity::IsShortCutIntent(
    /* [in] */ IIntent* intent)
{
    AutoPtr< ArrayOf<String> > categories;
    intent->GetCategories((ArrayOf<String>**)&categories);
    if (categories != NULL) {
        for (Int32 i = 0; i < categories->GetLength(); i++) {
            if ((*categories)[i].Equals(
                    "com.android.settings.SHORTCUT")) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

Boolean SettingsActivity::IsLikeShortCutIntent(
    /* [in] */ IIntent* intent)
{
    String action;
    intent->GetAction(&action);
    if (action == NULL) {
        return FALSE;
    }
    for (Int32 i = 0; i < LIKE_SHORTCUT_INTENT_ACTION_ARRAY->GetLength(); i++) {
        if ((*LIKE_SHORTCUT_INTENT_ACTION_ARRAY)[i].Equals(action)) return TRUE;
    }
    return FALSE;
}

ECode SettingsActivity::OnCreate(
    /* [in] */ IBundle* savedState)
{
    Slogger::I(TAG, " >> enter SettingsActivity::OnCreate");
    Activity::OnCreate(savedState);

    // Should happen before any call to GetIntent()
    GetMetaData();

    AutoPtr<IIntent> intent;
    GetIntent((IIntent**)&intent);
    Boolean res;
    if (intent->HasExtra(EXTRA_UI_OPTIONS, &res), res) {
        Int32 value;
        intent->GetInt32Extra(EXTRA_UI_OPTIONS, 0, &value);
        AutoPtr<IWindow> window;
        GetWindow((IWindow**)&window);
        window->SetUiOptions(value);
    }

    mDevelopmentPreferences = NULL;
    GetSharedPreferences(DevelopmentSettings::PREF_FILE,
            IContext::MODE_PRIVATE, (ISharedPreferences**)&mDevelopmentPreferences);

    // Getting Intent properties can only be done after the super.OnCreate(...)
    String initialFragmentName;
    intent->GetStringExtra(EXTRA_SHOW_FRAGMENT, &initialFragmentName);

    mIsShortcut = IsShortCutIntent(intent) || IsLikeShortCutIntent(intent)
            || (intent->GetBooleanExtra(EXTRA_SHOW_FRAGMENT_AS_SHORTCUT, FALSE, &res), res);

    AutoPtr<IComponentName> cn;
    intent->GetComponent((IComponentName**)&cn);
    String className;
    cn->GetClassName(&className);

    mIsShowingDashboard = className.Equals("Elastos.Droid.Settings.CSettings");

    // This is a "Sub Settings" when:
    // - this is a real SubSettings
    // - or :settings:show_fragment_as_subsetting is passed to the Intent
    Boolean isSubSettings = className.Equals("Elastos.Droid.Settings.CSubSettings")
            || (intent->GetBooleanExtra(EXTRA_SHOW_FRAGMENT_AS_SUBSETTING, FALSE, &res), res);

    // If this is a sub settings, then apply the SubSettings Theme for the ActionBar content insets
    if (isSubSettings) {
        // Check also that we are not a Theme Dialog as we don't want to override them
        Int32 themeResId;
        GetThemeResId(&themeResId);
        if (themeResId != R::style::Theme_DialogWhenLarge &&
                themeResId != R::style::Theme_SubSettingsDialogWhenLarge) {
            SetTheme(R::style::Theme_SubSettings);
        }
    }

    SetContentView(mIsShowingDashboard ?
            R::layout::settings_main_dashboard : R::layout::settings_main_prefs);

    AutoPtr<IView> view;
    FindViewById(R::id::main_content, (IView**)&view);
    mContent = IViewGroup::Probe(view);

    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    manager->AddOnBackStackChangedListener(this);

    if (mIsShowingDashboard) {
        AutoPtr<IContext> context;
        GetApplicationContext((IContext**)&context);
        Index::GetInstance(context)->Update();
    }

    if (savedState != NULL) {
        // We are restarting from a previous saved state; used that to initialize, instead
        // of starting fresh.
        savedState->GetBoolean(SAVE_KEY_SEARCH_MENU_EXPANDED, &mSearchMenuItemExpanded);
        savedState->GetString(SAVE_KEY_SEARCH_QUERY, &mSearchQuery);

        SetTitleFromIntent(intent);

        AutoPtr<IArrayList> categories;
        savedState->GetParcelableArrayList(SAVE_KEY_CATEGORIES, (IArrayList**)&categories);
        if (categories != NULL) {
            mCategories->Clear();
            mCategories->AddAll(ICollection::Probe(categories));
            SetTitleFromBackStack();
        }

        savedState->GetBoolean(SAVE_KEY_SHOW_HOME_AS_UP, &mDisplayHomeAsUpEnabled);
        savedState->GetBoolean(SAVE_KEY_SHOW_SEARCH, &mDisplaySearch);
        savedState->GetInt32(SAVE_KEY_HOME_ACTIVITIES_COUNT,
                1 /* one home activity by default */, &mHomeActivitiesCount);
    }
    else {
        if (!mIsShowingDashboard) {
            // Search is shown we are launched thru a  "shortcut". UP will be shown
            // only if it is a sub settings
            if (mIsShortcut) {
                mDisplayHomeAsUpEnabled = isSubSettings;
                mDisplaySearch = FALSE;
            }
            else if (isSubSettings) {
                mDisplayHomeAsUpEnabled = TRUE;
                mDisplaySearch = TRUE;
            }
            else {
                mDisplayHomeAsUpEnabled = FALSE;
                mDisplaySearch = FALSE;
            }
            SetTitleFromIntent(intent);

            AutoPtr<IBundle> initialArguments;
            intent->GetBundleExtra(EXTRA_SHOW_FRAGMENT_ARGUMENTS,
                    (IBundle**)&initialArguments);
            AutoPtr<IFragment> fragment;
            SwitchToFragment(initialFragmentName, initialArguments, TRUE, FALSE,
                    mInitialTitleResId, mInitialTitle, FALSE, (IFragment**)&fragment);
        }
        else {
            // No UP affordance if we are displaying the main Dashboard
            mDisplayHomeAsUpEnabled = FALSE;
            // Show Search affordance
            mDisplaySearch = TRUE;
            mInitialTitleResId = R::string::dashboard_title;
            AutoPtr<IFragment> fragment;
            SwitchToFragment(
                    String("Elastos.Droid.Settings.Dashboard.CDashboardSummary"),
                    NULL, FALSE, FALSE, mInitialTitleResId, mInitialTitle,
                    FALSE, (IFragment**)&fragment);
        }
    }

    mActionBar = NULL;
    GetActionBar((IActionBar**)&mActionBar);
    if (mActionBar != NULL) {
        mActionBar->SetDisplayHomeAsUpEnabled(mDisplayHomeAsUpEnabled);
        mActionBar->SetHomeButtonEnabled(mDisplayHomeAsUpEnabled);
    }

    view = NULL;
    FindViewById(R::id::switch_bar, (IView**)&view);
    mSwitchBar = ISwitchBar::Probe(view);

    // see if we should show Back/Next buttons
    if (intent->GetBooleanExtra(EXTRA_PREFS_SHOW_BUTTON_BAR, FALSE, &res), res) {

        AutoPtr<IView> buttonBar;
        FindViewById(R::id::button_bar, (IView**)&buttonBar);
        if (buttonBar != NULL) {
            buttonBar->SetVisibility(IView::VISIBLE);

            AutoPtr<IView> _view;
            FindViewById(R::id::back_button, (IView**)&_view);
            AutoPtr<IButton> backButton = IButton::Probe(_view);
            AutoPtr<OnCreateOnClickListener> myListener =
                    new OnCreateOnClickListener(this, 0);
            _view->SetOnClickListener(myListener);

            _view = NULL;
            FindViewById(R::id::skip_button, (IView**)&_view);
            AutoPtr<IButton> skipButton = IButton::Probe(_view);
            AutoPtr<OnCreateOnClickListener> myListener1 =
                    new OnCreateOnClickListener(this, 1);
            _view->SetOnClickListener(myListener1);

            _view = NULL;
            FindViewById(R::id::next_button, (IView**)&_view);
            mNextButton = IButton::Probe(_view);
            _view->SetOnClickListener(myListener1);

            // set our various button parameters
            if (intent->HasExtra(EXTRA_PREFS_SET_NEXT_TEXT, &res), res) {
                String buttonText;
                intent->GetStringExtra(EXTRA_PREFS_SET_NEXT_TEXT, &buttonText);
                if (TextUtils::IsEmpty(buttonText)) {
                    IView::Probe(mNextButton)->SetVisibility(IView::GONE);
                }
                else {
                    ITextView::Probe(mNextButton)->SetText(CoreUtils::Convert(buttonText));
                }
            }
            if (intent->HasExtra(EXTRA_PREFS_SET_BACK_TEXT, &res), res) {
                String buttonText;
                intent->GetStringExtra(EXTRA_PREFS_SET_BACK_TEXT, &buttonText);
                if (TextUtils::IsEmpty(buttonText)) {
                    IView::Probe(backButton)->SetVisibility(IView::GONE);
                }
                else {
                    ITextView::Probe(backButton)->SetText(CoreUtils::Convert(buttonText));
                }
            }
            if (intent->GetBooleanExtra(EXTRA_PREFS_SHOW_SKIP, FALSE, &res), res) {
                IView::Probe(skipButton)->SetVisibility(IView::VISIBLE);
            }
        }
    }

    mHomeActivitiesCount = GetHomeActivitiesCount();
    Slogger::I(TAG, " << leave SettingsActivity::OnCreate");
    return NOERROR;
}

Int32 SettingsActivity::GetHomeActivitiesCount()
{
    AutoPtr<IArrayList> homeApps;
    CArrayList::New((IArrayList**)&homeApps);
    AutoPtr<IPackageManager> manager;
    GetPackageManager((IPackageManager**)&manager);
    AutoPtr<IComponentName> name;
    manager->GetHomeActivities(IList::Probe(homeApps), (IComponentName**)&name);
    Int32 size;
    homeApps->GetSize(&size);
    return size;
}

void SettingsActivity::SetTitleFromIntent(
    /* [in] */ IIntent* intent)
{
    Int32 initialTitleResId;
    intent->GetInt32Extra(EXTRA_SHOW_FRAGMENT_TITLE_RESID, -1, &initialTitleResId);
    if (initialTitleResId > 0) {
        mInitialTitle = NULL;
        mInitialTitleResId = initialTitleResId;
        SetTitle(mInitialTitleResId);
    }
    else {
        mInitialTitleResId = -1;
        String initialTitle;
        intent->GetStringExtra(EXTRA_SHOW_FRAGMENT_TITLE, &initialTitle);
        if (!initialTitle.IsNull()) {
            mInitialTitle = CoreUtils::Convert(initialTitle);
        }
        else {
            mInitialTitle = NULL;
            GetTitle((ICharSequence**)&mInitialTitle);
        }
        SetTitle(mInitialTitle);
    }
}

ECode SettingsActivity::OnBackStackChanged()
{
    SetTitleFromBackStack();
    return NOERROR;
}

Int32 SettingsActivity::SetTitleFromBackStack()
{
    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    Int32 count;
    manager->GetBackStackEntryCount(&count);

    if (count == 0) {
        if (mInitialTitleResId > 0) {
            SetTitle(mInitialTitleResId);
        }
        else {
            SetTitle(mInitialTitle);
        }
        return 0;
    }

    AutoPtr<IFragmentManagerBackStackEntry> bse;
    manager->GetBackStackEntryAt((count - 1), (IFragmentManagerBackStackEntry**)&bse);
    SetTitleFromBackStackEntry(bse);

    return count;
}

void SettingsActivity::SetTitleFromBackStackEntry(
    /* [in] */ IFragmentManagerBackStackEntry* bse)
{
    AutoPtr<ICharSequence> title;
    Int32 titleRes;
    bse->GetBreadCrumbTitleRes(&titleRes);
    if (titleRes > 0) {
        GetText(titleRes, (ICharSequence**)&title);
    }
    else {
        bse->GetBreadCrumbTitle((ICharSequence**)&title);
    }
    if (title != NULL) {
        SetTitle(title);
    }
}

ECode SettingsActivity::OnSaveInstanceState(
    /* [in] */ IBundle* outState)
{
    Activity::OnSaveInstanceState(outState);

    Int32 size;
    mCategories->GetSize(&size);
    if (size > 0) {
        outState->PutParcelableArrayList(SAVE_KEY_CATEGORIES, mCategories);
    }

    outState->PutBoolean(SAVE_KEY_SHOW_HOME_AS_UP, mDisplayHomeAsUpEnabled);
    outState->PutBoolean(SAVE_KEY_SHOW_SEARCH, mDisplaySearch);

    if (mDisplaySearch) {
        // The option menus are created if the ActionBar is visible and they are also created
        // asynchronously. If you launch  with an Intent action like
        // android.intent.action.POWER_USAGE_SUMMARY and at the same time your device is locked
        // thru a LockScreen, OnCreateOptionsMenu() is not yet called and references to the search
        // menu item and search view are NULL.
        Boolean res;
        Boolean isExpanded = (mSearchMenuItem != NULL) &&
                (mSearchMenuItem->IsActionViewExpanded(&res), res);
        outState->PutBoolean(SAVE_KEY_SEARCH_MENU_EXPANDED, isExpanded);

        String query;
        if (mSearchView != NULL) {
            AutoPtr<ICharSequence> cs;
            mSearchView->GetQuery((ICharSequence**)&cs);
            cs->ToString(&query);
        }
        else {
            query = EMPTY_QUERY;
        }
        outState->PutString(SAVE_KEY_SEARCH_QUERY, query);
    }

    outState->PutInt32(SAVE_KEY_HOME_ACTIVITIES_COUNT, mHomeActivitiesCount);
    return NOERROR;
}

ECode SettingsActivity::OnResume()
{
    Slogger::I(TAG, " >> enter SettingsActivity::OnResume");
    Activity::OnResume();

    Int32 newHomeActivityCount = GetHomeActivitiesCount();
    if (newHomeActivityCount != mHomeActivitiesCount) {
        mHomeActivitiesCount = newHomeActivityCount;
        InvalidateCategories(TRUE);
    }

    mDevelopmentPreferencesListener = new OnResumeOnSharedPreferenceChangeListener(this);

    mDevelopmentPreferences->RegisterOnSharedPreferenceChangeListener(mDevelopmentPreferencesListener);

    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_BATTERY_CHANGED, (IIntentFilter**)&filter);

    AutoPtr<IIntent> intent;
    RegisterReceiver(mBatteryInfoReceiver, filter, (IIntent**)&intent);

    // TODO:
    Slogger::I("SettingsActivity::OnResume", "Register is TODO");
    // mDynamicIndexableContentMonitor->Register(this);

    if (mDisplaySearch && !TextUtils::IsEmpty(mSearchQuery)) {
        Boolean res;
        OnQueryTextSubmit(mSearchQuery, &res);
    }
    Slogger::I(TAG, " << leave SettingsActivity::OnResume");
    return NOERROR;
}

ECode SettingsActivity::OnPause()
{
    Activity::OnPause();

    UnregisterReceiver(mBatteryInfoReceiver);
    // TODO:
    Slogger::I("SettingsActivity::OnPause", "Unregister is TODO");
    // mDynamicIndexableContentMonitor->Unregister();
    return NOERROR;
}

ECode SettingsActivity::OnDestroy()
{
    Activity::OnDestroy();

    mDevelopmentPreferences->UnregisterOnSharedPreferenceChangeListener(
            mDevelopmentPreferencesListener);
    mDevelopmentPreferencesListener = NULL;
    return NOERROR;
}

Boolean SettingsActivity::IsValidFragment(
    /* [in] */ const String& fragmentName)
{
    // Almost all fragments are wrapped in this,
    // except for a few that have their own activities.
    for (Int32 i = 0; i < ENTRY_FRAGMENTS->GetLength(); i++) {
        if ((*ENTRY_FRAGMENTS)[i].Equals(fragmentName)) return TRUE;
    }
    return FALSE;
}

ECode SettingsActivity::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);

    AutoPtr<IIntent> superIntent;
    Activity::GetIntent((IIntent**)&superIntent);

    String startingFragment = GetStartingFragmentClass(superIntent);
    // This is called from super.onCreate, IsMultiPane() is not yet reliable
    // Do not use onIsHidingHeaders either, which relies itself on this method
    if (!startingFragment.IsNull()) {
        AutoPtr<IIntent> modIntent;
        CIntent::New(superIntent, (IIntent**)&modIntent);
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, startingFragment);
        AutoPtr<IBundle> args;
        superIntent->GetExtras((IBundle**)&args);
        if (args != NULL) {
            AutoPtr<IBundle> otherBundle;
            CBundle::New(args, (IBundle**)&otherBundle);
            args = otherBundle;
        }
        else {
            CBundle::New((IBundle**)&args);
        }
        args->PutParcelable(String("intent"), IParcelable::Probe(superIntent));
        modIntent->PutExtra(EXTRA_SHOW_FRAGMENT_ARGUMENTS, args);
        *intent = modIntent;
        REFCOUNT_ADD(*intent);
        return NOERROR;
    }
    *intent = superIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

String SettingsActivity::GetStartingFragmentClass(
    /* [in] */ IIntent* intent)
{
    if (!mFragmentClass.IsNull()) return mFragmentClass;

    AutoPtr<IComponentName> comp;
    intent->GetComponent((IComponentName**)&comp);
    String intentClass;
    comp->GetClassName(&intentClass);

    AutoPtr<IClassInfo> thisKlass;
    CObject::ReflectClassInfo((IActivity*)this, (IClassInfo**)&thisKlass);
    String klassName, klassNamespace;
    thisKlass->GetName(&klassName);
    thisKlass->GetNamespace(&klassNamespace);
    if (intentClass.Equals(klassNamespace + "." + klassName)) return String(NULL);

    if (intentClass.Equals("Elastos.Droid.Settings.ManageApplications")
            || intentClass.Equals("Elastos.Droid.Settings.RunningServices")
            || intentClass.Equals("Elastos.Droid.Settings.Applications.StorageUse")) {
        // Old names of manage apps.
        intentClass = "Elastos.Droid.Settings.Applications.ManageApplications";
    }

    return intentClass;
}

ECode SettingsActivity::StartPreferencePanel(
    /* [in] */ const String& fragmentClass,
    /* [in] */ IBundle* args,
    /* [in] */ Int32 titleRes,
    /* [in] */ ICharSequence* titleText,
    /* [in] */ IFragment* resultTo,
    /* [in] */ Int32 resultRequestCode)
{
    Slogger::I(TAG, " >> enter SettingsActivity::StartPreferencePanel");
    String title;
    if (titleRes < 0) {
        if (titleText != NULL) {
            titleText->ToString(&title);
        }
        else {
            // There not much we can do in that case
            title = "";
        }
    }
    Utils::StartWithFragment(this, fragmentClass, args, resultTo, resultRequestCode,
            titleRes, CoreUtils::Convert(title), mIsShortcut);
    Slogger::I(TAG, " << leave SettingsActivity::StartPreferencePanel");
    return NOERROR;
}

ECode SettingsActivity::StartPreferencePanelAsUser(
    /* [in] */ const String& fragmentClass,
    /* [in] */ IBundle* args,
    /* [in] */ Int32 titleRes,
    /* [in] */ ICharSequence* titleText,
    /* [in] */ IUserHandle* userHandle)
{
    String title;
    if (titleRes < 0) {
        if (titleText != NULL) {
            titleText->ToString(&title);
        }
        else {
            // There not much we can do in that case
            title = "";
        }
    }
    Utils::StartWithFragmentAsUser(this, fragmentClass, args,
            titleRes, CoreUtils::Convert(title), mIsShortcut, userHandle);
    return NOERROR;
}

ECode SettingsActivity::FinishPreferencePanel(
    /* [in] */ IFragment* caller,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData)
{
    SetResult(resultCode, resultData);
    Finish();
    return NOERROR;
}

ECode SettingsActivity::StartPreferenceFragment(
    /* [in] */ IFragment* fragment,
    /* [in] */ Boolean push)
{
    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    AutoPtr<IFragmentTransaction> transaction;
    manager->BeginTransaction((IFragmentTransaction**)&transaction);
    transaction->Replace(R::id::main_content, fragment);
    if (push) {
        transaction->SetTransition(IFragmentTransaction::TRANSIT_FRAGMENT_OPEN);
        transaction->AddToBackStack(BACK_STACK_PREFS);
    }
    else {
        transaction->SetTransition(IFragmentTransaction::TRANSIT_FRAGMENT_FADE);
    }
    Int32 value;
    return transaction->CommitAllowingStateLoss(&value);
}

ECode SettingsActivity::SwitchToFragment(
    /* [in] */ const String& fragmentName,
    /* [in] */ IBundle* args,
    /* [in] */ Boolean validate,
    /* [in] */ Boolean addToBackStack,
    /* [in] */ Int32 titleResId,
    /* [in] */ ICharSequence* title,
    /* [in] */ Boolean withTransition,
    /* [out] */ IFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    *fragment = NULL;

    if (validate && !IsValidFragment(fragmentName)) {
        Slogger::E(TAG, "Invalid fragment for this activity: %s", fragmentName.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
        // throw new IllegalArgumentException("Invalid fragment for this activity: "
        //         + fragmentName);
    }

    AutoPtr<IFragmentHelper> helper;
    CFragmentHelper::AcquireSingleton((IFragmentHelper**)&helper);
    AutoPtr<IFragment> f;
    helper->Instantiate(this, fragmentName, args, (IFragment**)&f);
    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    AutoPtr<IFragmentTransaction> transaction;
    manager->BeginTransaction((IFragmentTransaction**)&transaction);
    transaction->Replace(R::id::main_content, f);
    if (withTransition) {
        AutoPtr<ITransitionManagerHelper> helper;
        assert(0 && "TODO");
        // CTransitionManagerHelper::AcquireSingleton((ITransitionManagerHelper**)&helper);
        helper->BeginDelayedTransition(IViewGroup::Probe(mContent));
    }
    if (addToBackStack) {
        transaction->AddToBackStack(SettingsActivity::BACK_STACK_PREFS);
    }
    if (titleResId > 0) {
        transaction->SetBreadCrumbTitle(titleResId);
    }
    else if (title != NULL) {
        transaction->SetBreadCrumbTitle(title);
    }
    Int32 value;
    transaction->CommitAllowingStateLoss(&value);
    Boolean res;
    manager->ExecutePendingTransactions(&res);

    *fragment = f;
    REFCOUNT_ADD(*fragment);
    return NOERROR;
}

ECode SettingsActivity::BuildDashboardCategories(
    /* [in] */ IList* categories)
{
    categories->Clear();
    FAIL_RETURN(LoadCategoriesFromResource(R::xml::dashboard_categories, categories));
    FAIL_RETURN(UpdateTilesList(categories));
    return NOERROR;
}

ECode SettingsActivity::LoadCategoriesFromResource(
    /* [in] */ Int32 resid,
    /* [in] */ IList* target)
{
    AutoPtr<IXmlResourceParser> parser;
    // try {
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetXml(resid, (IXmlResourceParser**)&parser);
    AutoPtr<IAttributeSet> attrs = Xml::AsAttributeSet(IXmlPullParser::Probe(parser));

    Int32 type;
    while ((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && type != IXmlPullParser::START_TAG) {
        // Parse next until start tag is found
    }

    String nodeName;
    IXmlPullParser::Probe(parser)->GetName(&nodeName);
    if (!nodeName.Equals("dashboard-categories")) {
        String desc;
        IXmlPullParser::Probe(parser)->GetPositionDescription(&desc);
        Slogger::E(TAG, "XML document must start with <preference-categories> tag; found %s at %s",
                nodeName.string(), desc.string());
        if (parser != NULL) parser->Close();
        return E_RUNTIME_EXCEPTION;
        // throw new RuntimeException(
        //         "XML document must start with <preference-categories> tag; found"
        //                 + nodeName + " at " + parser.getPositionDescription());
    }

    AutoPtr<IBundle> curBundle;

    Int32 outerDepth;
    IXmlPullParser::Probe(parser)->GetDepth(&outerDepth);
    Int32 depth;
    while ((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_DOCUMENT
            && (type != IXmlPullParser::END_TAG
                || (IXmlPullParser::Probe(parser)->GetDepth(&depth), depth) > outerDepth)) {
        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
            continue;
        }

        nodeName = String(NULL);
        IXmlPullParser::Probe(parser)->GetName(&nodeName);
        if (nodeName.Equals("dashboard-category")) {
            AutoPtr<CDashboardCategory> category;
            CDashboardCategory::NewByFriend((CDashboardCategory**)&category);

            AutoPtr< ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
                    const_cast<Int32 *>(Elastos::Droid::R::styleable::PreferenceHeader),
                    ArraySize(Elastos::Droid::R::styleable::PreferenceHeader));
            AutoPtr<ITypedArray> sa;
            ObtainStyledAttributes(attrs, attrIds, (ITypedArray**)&sa);
            Int32 id;
            sa->GetResourceId(
                    Elastos::Droid::R::styleable::PreferenceHeader_id,
                    (Int32)CDashboardCategory::CAT_ID_UNDEFINED, &id);
            category->mId = id;

            AutoPtr<ITypedValue> tv;
            sa->PeekValue(
                    Elastos::Droid::R::styleable::PreferenceHeader_title, (ITypedValue**)&tv);
            Int32 type;
            Int32 resourceId;
            if (tv != NULL && (tv->GetType(&type), type) == ITypedValue::TYPE_STRING) {
                if ((tv->GetResourceId(&resourceId), resourceId) != 0) {
                    category->mTitleRes = resourceId;
                }
                else {
                    tv->GetString((ICharSequence**)&category->mTitle);
                }
            }
            sa->Recycle();

            Int32 innerDepth;
            IXmlPullParser::Probe(parser)->GetDepth(&innerDepth);
            while ((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_DOCUMENT
                    && (type != IXmlPullParser::END_TAG
                        || (IXmlPullParser::Probe(parser)->GetDepth(&depth), depth) > innerDepth)) {
                if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
                    continue;
                }

                String innerNodeName;
                IXmlPullParser::Probe(parser)->GetName(&innerNodeName);
                if (innerNodeName.Equals("dashboard-tile")) {
                    AutoPtr<CDashboardTile> tile;
                    CDashboardTile::NewByFriend((CDashboardTile**)&tile);

                    sa = NULL;
                    ObtainStyledAttributes(
                            attrs, attrIds, (ITypedArray**)&sa);
                    sa->GetResourceId(
                            Elastos::Droid::R::styleable::PreferenceHeader_id,
                            (Int32)CDashboardTile::TILE_ID_UNDEFINED, &id);
                    tile->mId = id;
                    tv = NULL;
                    sa->PeekValue(
                            Elastos::Droid::R::styleable::PreferenceHeader_title, (ITypedValue**)&tv);
                    if (tv != NULL && (tv->GetType(&type), type) == ITypedValue::TYPE_STRING) {
                        if ((tv->GetResourceId(&resourceId), resourceId) != 0) {
                            tile->mTitleRes = resourceId;
                        }
                        else {
                            tv->GetString((ICharSequence**)&tile->mTitle);
                        }
                    }
                    tv = NULL;
                    sa->PeekValue(
                            Elastos::Droid::R::styleable::PreferenceHeader_summary, (ITypedValue**)&tv);
                    if (tv != NULL && (tv->GetType(&type), type) == ITypedValue::TYPE_STRING) {
                        if ((tv->GetResourceId(&resourceId), resourceId) != 0) {
                            tile->mSummaryRes = resourceId;
                        }
                        else {
                            tv->GetString((ICharSequence**)&tile->mSummary);
                        }
                    }
                    sa->GetResourceId(
                            Elastos::Droid::R::styleable::PreferenceHeader_icon, 0, &tile->mIconRes);
                    sa->GetString(
                            Elastos::Droid::R::styleable::PreferenceHeader_fragment, &tile->mFragment);
                    sa->Recycle();

                    if (curBundle == NULL) {
                        CBundle::New((IBundle**)&curBundle);
                    }

                    Int32 innerDepth2;
                    IXmlPullParser::Probe(parser)->GetDepth(&innerDepth2);
                    while ((IXmlPullParser::Probe(parser)->Next(&type), type) != IXmlPullParser::END_DOCUMENT
                            && (type != IXmlPullParser::END_TAG ||
                                (IXmlPullParser::Probe(parser)->GetDepth(&depth), depth) > innerDepth2)) {
                        if (type == IXmlPullParser::END_TAG || type == IXmlPullParser::TEXT) {
                            continue;
                        }

                        String innerNodeName2;
                        IXmlPullParser::Probe(parser)->GetName(&innerNodeName2);
                        if (innerNodeName2.Equals("extra")) {
                            AutoPtr<IResources> res;
                            GetResources((IResources**)&res);
                            res->ParseBundleExtra(String("extra"), attrs, curBundle);
                            XmlUtils::SkipCurrentTag(IXmlPullParser::Probe(parser));

                        }
                        else if (innerNodeName2.Equals("intent")) {
                            AutoPtr<IResources> res;
                            GetResources((IResources**)&res);
                            AutoPtr<IIntentHelper> helper;
                            CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
                            helper->ParseIntent(res, IXmlPullParser::Probe(parser), attrs, (IIntent**)&tile->mIntent);
                        }
                        else {
                            XmlUtils::SkipCurrentTag(IXmlPullParser::Probe(parser));
                        }
                    }

                    Int32 size;
                    if ((curBundle->GetSize(&size), size) > 0) {
                        tile->mFragmentArguments = curBundle;
                        curBundle = NULL;
                    }

                    // Show the SIM Cards setting if there are more than 2 SIMs installed.
                    if (tile->mId != R::id::sim_settings || Utils::ShowSimCardTile(this)){
                        category->AddTile(tile);
                    }

                }
                else {
                    XmlUtils::SkipCurrentTag(IXmlPullParser::Probe(parser));
                }
            }

            target->Add((IObject*)category);
        }
        else {
            XmlUtils::SkipCurrentTag(IXmlPullParser::Probe(parser));
        }
    }

    // } catch (XmlPullParserException e) {
    //     throw new RuntimeException("Error parsing categories", e);
    // } catch (IOException e) {
    //     throw new RuntimeException("Error parsing categories", e);
    // } finally {
    if (parser != NULL) parser->Close();
    // }
    return NOERROR;
}

ECode SettingsActivity::UpdateTilesList(
    /* [in] */ IList* target)
{
    Boolean showDev;
    mDevelopmentPreferences->GetBoolean(
            DevelopmentSettings::PREF_SHOW,
            Build::TYPE.Equals("eng"), &showDev);

    AutoPtr<IInterface> obj;
    GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
    IUserManager* um = IUserManager::Probe(obj);

    Int32 size;
    target->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        target->Get(i, (IInterface**)&obj);
        CDashboardCategory* category = (CDashboardCategory*)IObject::Probe(obj);

        // Ids are integers, so downcasting is ok
        Int32 id = (Int32) category->mId;
        Int32 n = category->GetTilesCount() - 1;
        Boolean res;
        while (n >= 0) {
            AutoPtr<CDashboardTile> tile = category->GetTile(n);
            Boolean removeTile = FALSE;
            id = (Int32) tile->mId;
            if (id == R::id::operator_settings || id == R::id::manufacturer_settings) {
                if (!Utils::UpdateTileToSpecificActivityFromMetaDataOrRemove(this, tile)) {
                    removeTile = TRUE;
                }
            }
            else if (id == R::id::wifi_settings) {
                // Remove WiFi  if WiFi service is not available.
                AutoPtr<IPackageManager> manager;
                GetPackageManager((IPackageManager**)&manager);
                if (manager->HasSystemFeature(IPackageManager::FEATURE_WIFI, &res), !res) {
                    removeTile = TRUE;
                }
            }
            else if (id == R::id::bluetooth_settings) {
                // Remove Bluetooth  if Bluetooth service is not available.
                AutoPtr<IPackageManager> manager;
                GetPackageManager((IPackageManager**)&manager);
                if (manager->HasSystemFeature(IPackageManager::FEATURE_BLUETOOTH, &res), !res) {
                    removeTile = TRUE;
                }
            }
            else if (id == R::id::data_usage_settings) {
                // Remove data usage when kernel module not enabled
                AutoPtr<IInterface> obj = ServiceManager::GetService(IContext::NETWORKMANAGEMENT_SERVICE);
                IINetworkManagementService* netManager = IINetworkManagementService::Probe(obj);
                // try {
                if (netManager->IsBandwidthControlEnabled(&res), !res) {
                    removeTile = TRUE;
                }
                // } catch (RemoteException e) {
                //     // ignored
                // }
            }
            else if (id == R::id::battery_settings) {
                // Remove battery settings when battery is not available. (e.g. TV)

                if (!mBatteryPresent) {
                    removeTile = TRUE;
                }
            }
            else if (id == R::id::home_settings) {
                Boolean res;
                if (UpdateHomeSettingTiles(tile, &res), !res) {
                    removeTile = TRUE;
                }
            }
            else if (id == R::id::user_settings) {
                AutoPtr<IInterface> obj;
                GetSystemService(IContext::USER_SERVICE, (IInterface**)&obj);
                Int32 count;
                IUserManager::Probe(obj)->GetUserCount(&count);
                Boolean hasMultipleUsers = count > 1;

                AutoPtr<IUserManagerHelper> helper;
                CUserManagerHelper::AcquireSingleton((IUserManagerHelper**)&helper);
                helper->SupportsMultipleUsers(&res);
                if (!IUserHandle::MU_ENABLED
                        || (!res && !hasMultipleUsers)
                        || Utils::IsMonkeyRunning()) {
                    removeTile = TRUE;
                }
            }
            else if (id == R::id::nfc_payment_settings) {
                AutoPtr<IPackageManager> manager;
                GetPackageManager((IPackageManager**)&manager);
                if (manager->HasSystemFeature(IPackageManager::FEATURE_NFC, &res), !res) {
                    removeTile = TRUE;
                }
                else {
                    // Only show if NFC is on and we have the HCE feature

                    // TODO
                    Slogger::I(TAG, "CNfcAdapterHelper is TODO");
                    // AutoPtr<INfcAdapterHelper> helper;
                    // CNfcAdapterHelper::AcquireSingleton((INfcAdapterHelper**)&helper);
                    // AutoPtr<INfcAdapter> adapter;
                    // helper->GetDefaultAdapter(IContext::Probe(this), (INfcAdapter**)&adapter);
                    // Boolean isEnabled;
                    // if (adapter == NULL
                    //         || (adapter->IsEnabled(&isEnabled), !isEnabled)
                    //         || (manager->HasSystemFeature(
                    //                 IPackageManager::FEATURE_NFC_HOST_CARD_EMULATION, &res), !res)) {
                    //     removeTile = TRUE;
                    // }
                }
            }
            else if (id == R::id::print_settings) {
                AutoPtr<IPackageManager> manager;
                GetPackageManager((IPackageManager**)&manager);
                Boolean hasPrintingSupport;
                manager->HasSystemFeature(
                        IPackageManager::FEATURE_PRINTING, &hasPrintingSupport);
                if (!hasPrintingSupport) {
                    removeTile = TRUE;
                }
            }
            else if (id == R::id::development_settings) {
                if (!showDev || (um->HasUserRestriction(
                        IUserManager::DISALLOW_DEBUGGING_FEATURES, &res), res)) {
                    removeTile = TRUE;
                }
            }

            if (IUserHandle::MU_ENABLED && UserHandle::GetMyUserId() != 0
                    && !ArrayUtils::Contains(SETTINGS_FOR_RESTRICTED.Get(), id)) {
                removeTile = TRUE;
            }

            if (removeTile && n < category->GetTilesCount()) {
                category->RemoveTile(n);
            }
            n--;
        }
    }
    return NOERROR;
}

ECode SettingsActivity::UpdateHomeSettingTiles(
    /* [in] */ CDashboardTile* tile,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    // Once we decide to show Home settings, keep showing it forever
    AutoPtr<ISharedPreferences> sp;
    GetSharedPreferences(HomeSettings::HOME_PREFS,
            IContext::MODE_PRIVATE, (ISharedPreferences**)&sp);
    Boolean result;
    if (sp->GetBoolean(HomeSettings::HOME_PREFS_DO_SHOW, FALSE, &result), result) {
        *res = TRUE;
        return NOERROR;
    }

    // try {
    mHomeActivitiesCount = GetHomeActivitiesCount();
    if (mHomeActivitiesCount < 2) {
        // When there's only one available home app, omit this settings
        // category entirely at the top level UI.  If the user just
        // uninstalled the penultimate home app candidiate, we also
        // now tell them about why they aren't seeing 'Home' in the list.
        if (sShowNoHomeNotice) {
            sShowNoHomeNotice = FALSE;
            NoHomeDialogFragment::Show(this);
        }
        *res = FALSE;
        return NOERROR;
    }
    else {
        // Okay, we're allowing the Home settings category.  Tell it, when
        // invoked via this front door, that we'll need to be told about the
        // case when the user uninstalls all but one home app.
        if (tile->mFragmentArguments == NULL) {
            CBundle::New((IBundle**)&tile->mFragmentArguments);
        }
        tile->mFragmentArguments->PutBoolean(HomeSettings::HOME_SHOW_NOTICE, TRUE);
    }
    // } catch (Exception e) {
    //     // Can't look up the home activity; bail on configuring the icon
    //     Logger::W(TAG, "Problem looking up home activity!", e);
    // }

    AutoPtr<ISharedPreferencesEditor> editor;
    sp->Edit((ISharedPreferencesEditor**)&editor);
    editor->PutBoolean(HomeSettings::HOME_PREFS_DO_SHOW, TRUE);
    editor->Apply();
    *res = TRUE;
    return NOERROR;
}

ECode SettingsActivity::GetMetaData()
{
    // try {
    AutoPtr<IComponentName> name;
    GetComponentName((IComponentName**)&name);
    AutoPtr<IPackageManager> manager;
    GetPackageManager((IPackageManager**)&manager);
    AutoPtr<IActivityInfo> ai;
    manager->GetActivityInfo(name,
            IPackageManager::GET_META_DATA, (IActivityInfo**)&ai);
    AutoPtr<IBundle> metaData;
    if (ai == NULL ||
            (IPackageItemInfo::Probe(ai)->GetMetaData(
                (IBundle**)&metaData), metaData) == NULL) {
        return NOERROR;
    }

    IPackageItemInfo::Probe(ai)->GetMetaData((IBundle**)&metaData);
    metaData->GetString(META_DATA_KEY_FRAGMENT_CLASS, &mFragmentClass);
    // } catch (NameNotFoundException nnfe) {
    //     // No recovery
    //     Logger::D(TAG, "Cannot get Metadata for: " + GetComponentName()->ToString());
    // }
    return NOERROR;
}

ECode SettingsActivity::HasNextButton(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNextButton != NULL;
    return NOERROR;
}

ECode SettingsActivity::GetNextButton(
    /* [out] */ IButton** button)
{
    VALIDATE_NOT_NULL(button);
    *button = mNextButton;
    REFCOUNT_ADD(*button);
    return NOERROR;
}

ECode SettingsActivity::ShouldUpRecreateTask(
    /* [in] */ IIntent* targetIntent,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IIntent> intent;
    CIntent::New(this, ECLSID_CSettingsActivity, (IIntent**)&intent);
    return Activity::ShouldUpRecreateTask(intent, result);
}

ECode SettingsActivity::RequestHomeNotice()
{
    sShowNoHomeNotice = TRUE;
    return NOERROR;
}

ECode SettingsActivity::OnQueryTextSubmit(
    /* [in] */ const String& query,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    SwitchToSearchResultsFragmentIfNeeded();
    mSearchQuery = query;
    *result = ((CSearchResultsSummary*)mSearchResultsFragment.Get())->OnQueryTextSubmit(query);
    return NOERROR;
}

ECode SettingsActivity::OnQueryTextChange(
    /* [in] */ const String& newText,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    mSearchQuery = newText;
    if (mSearchResultsFragment == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    *result = ((CSearchResultsSummary*)mSearchResultsFragment.Get())->OnQueryTextChange(newText);
    return NOERROR;
}

ECode SettingsActivity::OnClose(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    return NOERROR;
}

ECode SettingsActivity::OnMenuItemActionExpand(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 id1, id2;
    item->GetItemId(&id1);
    mSearchMenuItem->GetItemId(&id2);
    if (id1 == id2) {
        SwitchToSearchResultsFragmentIfNeeded();
    }
    *result = TRUE;
    return NOERROR;
}

ECode SettingsActivity::OnMenuItemActionCollapse(
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 id1, id2;
    item->GetItemId(&id1);
    mSearchMenuItem->GetItemId(&id2);
    if (id1 == id2) {
        if (mSearchMenuItemExpanded) {
            RevertToInitialFragment();
        }
    }
    *result = TRUE;
    return NOERROR;
}

void SettingsActivity::SwitchToSearchResultsFragmentIfNeeded()
{
    if (mSearchResultsFragment != NULL) {
        return;
    }

    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    AutoPtr<IFragment> current;
    manager->FindFragmentById(R::id::main_content, (IFragment**)&current);
    if (current != NULL && ISearchResultsSummary::Probe(current) != NULL) {
        mSearchResultsFragment = ISearchResultsSummary::Probe(current);
    }
    else {
        AutoPtr<IFragment> fragment;
        SwitchToFragment(
                String("Elastos.Droid.Settings.Dashboard.CSearchResultsSummary"),
                NULL, FALSE, TRUE, R::string::search_results_title, NULL,
                TRUE, (IFragment**)&fragment);
        mSearchResultsFragment = ISearchResultsSummary::Probe(fragment);
    }
    ((CSearchResultsSummary*)mSearchResultsFragment.Get())->SetSearchView(mSearchView);
    mSearchMenuItemExpanded = TRUE;
}

ECode SettingsActivity::NeedToRevertToInitialFragment()
{
    mNeedToRevertToInitialFragment = TRUE;
    return NOERROR;
}

void SettingsActivity::RevertToInitialFragment()
{
    mNeedToRevertToInitialFragment = FALSE;
    mSearchResultsFragment = NULL;
    mSearchMenuItemExpanded = FALSE;
    AutoPtr<IFragmentManager> manager;
    GetFragmentManager((IFragmentManager**)&manager);
    Boolean res;
    manager->PopBackStackImmediate(SettingsActivity::BACK_STACK_PREFS,
            IFragmentManager::POP_BACK_STACK_INCLUSIVE, &res);
    if (mSearchMenuItem != NULL) {
        Boolean res;
        mSearchMenuItem->CollapseActionView(&res);
    }
}

ECode SettingsActivity::GetResultIntentData(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = mResultIntentData;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode SettingsActivity::SetResultIntentData(
    /* [in] */ IIntent* resultIntentData)
{
    mResultIntentData = resultIntentData;
    return NOERROR;
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
