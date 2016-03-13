
#ifndef __ELASTOS_DROID_SETTINGS_SETTINGSACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_SETTINGSACTIVITY_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::App::IFragmentManagerOnBackStackChangedListener;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IXmlResourceParser;
using Elastos::Droid::Nfc::INfcAdapter;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceFragment;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::IPreferenceManagerOnPreferenceTreeClickListener;
using Elastos::Droid::Preference::IPreferenceFragmentOnPreferenceStartFragmentCallback;
using Elastos::Droid::Transition::ITransitionManager;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IMenuItemOnActionExpandListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ISearchView;
using Elastos::Droid::Widget::ISearchViewOnCloseListener;
using Elastos::Droid::Widget::ISearchViewOnQueryTextListener;

using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::Xml;
using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Droid::Internal::Utility::XmlUtils;

// using Elastos::Droid::Settings::Accessibility::IAccessibilitySettings;
// using Elastos::Droid::Settings::Accessibility::ICaptionPropertiesFragment;
// using Elastos::Droid::Settings::Accounts::IAccountSettings;
// using Elastos::Droid::Settings::Accounts::IAccountSyncSettings;
// using Elastos::Droid::Settings::Applications::IInstalledAppDetails;
// using Elastos::Droid::Settings::Applications::IManageApplications;
// using Elastos::Droid::Settings::Applications::IProcessStatsUi;
// using Elastos::Droid::Settings::Bluetooth::IBluetoothSettings;
// using Elastos::Droid::Settings::Dashboard::IDashboardCategory;
// using Elastos::Droid::Settings::Dashboard::IDashboardSummary;
// using Elastos::Droid::Settings::Dashboard::IDashboardTile;
// using Elastos::Droid::Settings::Dashboard::INoHomeDialogFragment;
// using Elastos::Droid::Settings::Dashboard::ISearchResultsSummary;
// using Elastos::Droid::Settings::Deviceinfo::IMemory;
// using Elastos::Droid::Settings::Deviceinfo::IUsbSettings;
// using Elastos::Droid::Settings::Fuelgauge::IBatterySaverSettings;
// using Elastos::Droid::Settings::Fuelgauge::IPowerUsageSummary;
// using Elastos::Droid::Settings::Notification::INotificationAppList;
// using Elastos::Droid::Settings::Notification::IOtherSoundSettings;
// using Elastos::Droid::Settings::Quicklaunch::IQuickLaunchSettings;
// using Elastos::Droid::Settings::Search::IDynamicIndexableContentMonitor;
// using Elastos::Droid::Settings::Search::IIndex;
// using Elastos::Droid::Settings::Inputmethod::IInputMethodAndLanguageSettings;
// using Elastos::Droid::Settings::Inputmethod::IKeyboardLayoutPickerFragment;
// using Elastos::Droid::Settings::Inputmethod::ISpellCheckersSettings;
// using Elastos::Droid::Settings::Inputmethod::IUserDictionaryList;
// using Elastos::Droid::Settings::Location::ILocationSettings;
// using Elastos::Droid::Settings::Nfc::IAndroidBeam;
// using Elastos::Droid::Settings::Nfc::IPaymentSettings;
// using Elastos::Droid::Settings::Notification::IAppNotificationSettings;
// using Elastos::Droid::Settings::Notification::IConditionProviderSettings;
// using Elastos::Droid::Settings::Notification::INotificationAccessSettings;
// using Elastos::Droid::Settings::Notification::INotificationSettings;
// using Elastos::Droid::Settings::Notification::INotificationStation;
// using Elastos::Droid::Settings::Notification::IZenModeSettings;
// using Elastos::Droid::Settings::Print::IPrintJobSettingsFragment;
// using Elastos::Droid::Settings::Print::IPrintSettingsFragment;
// using Elastos::Droid::Settings::Sim::ISimSettings;
// using Elastos::Droid::Settings::Tts::ITextToSpeechSettings;
// using Elastos::Droid::Settings::Users::IUserSettings;
// using Elastos::Droid::Settings::Voice::IVoiceInputSettings;
// using Elastos::Droid::Settings::Vpn2::IVpnSettings;
// using Elastos::Droid::Settings::Wfd::IWifiDisplaySettings;
// using Elastos::Droid::Settings::Widget::ISwitchBar;
// using Elastos::Droid::Settings::Wifi::IAdvancedWifiSettings;
// using Elastos::Droid::Settings::Wifi::ISavedAccessPointsWifiSettings;
// using Elastos::Droid::Settings::Wifi::IWifiSettings;
// using Elastos::Droid::Settings::Wifi::P2p::IWifiP2pSettings;

using Org::Xmlpull::V1::IXmlPullParser;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;

// using static com::Android::Settings::Dashboard::DashboardTile::ITILE_ID_UNDEFINED;

namespace Elastos {
namespace Droid {
namespace Settings {

class SettingsActivity
    : public Activity
    , public IPreferenceManagerOnPreferenceTreeClickListener
    , public IPreferenceFragmentOnPreferenceStartFragmentCallback
    , public IButtonBarHandler
    , public IFragmentManagerOnBackStackChangedListener
    , public ISearchViewOnQueryTextListener
    , public ISearchViewOnCloseListener
    , public IOnActionExpandListener
{
private:
    class BatteryInfoReceiver
        : public BroadcastReceiver
    {
    public:
        BatteryInfoReceiver(
            /* [in] */ SettingsActivity* host);

        ~BatteryInfoReceiver();

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

    private:
        SettingsActivity* mHost;
    };

    class BuildCategoriesHandler
        : public Handler
    {
    public:
        BuildCategoriesHandler(
            /* [in] */ SettingsActivity* host);

        ~BuildCategoriesHandler();

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        SettingsActivity* mHost;
    };

    class OnCreateOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnCreateOnClickListener(
            /* [in] */ SettingsActivity* host,
            /* [in] */ Int32 id);

        ~OnCreateOnClickListener();

        // @override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        SettingsActivity* mHost;
        Int32 mId;
    };

    class OnResumeOnSharedPreferenceChangeListener
        : public Object
        , public ISharedPreferencesOnSharedPreferenceChangeListener
    {
    public:
        CAR_INTERFACE_DECL();

        OnResumeOnSharedPreferenceChangeListener(
            /* [in] */ SettingsActivity* host);

        ~OnResumeOnSharedPreferenceChangeListener();

        // @override
        CARAPI OnSharedPreferenceChanged(
            /* [in] */ ISharedPreferences* sharedPreferences,
            /* [in] */ const String& key);

    private:
        SettingsActivity* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    SettingsActivity();

    ~SettingsActivity();

    CARAPI GetSwitchBar(
        /* [out] */ ISwitchBar** switchBar);

    CARAPI GetDashboardCategories(
        /* [in] */ Boolean forceRefresh,
        /* [out] */ IList** categories); //List<DashboardCategory>

    //@Override
    CARAPI OnPreferenceStartFragment(
        /* [in] */ IPreferenceFragment* caller,
        /* [in] */ IPreference* pref,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ PreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* pref,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnBackStackChanged();

    //@Override
    CARAPI OnResume();

    //@Override
    CARAPI OnPause();

    //@Override
    CARAPI OnDestroy();

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    /**
     * Start a new fragment containing a preference panel.  If the preferences
     * are being displayed in multi-pane mode, the given fragment class will
     * be instantiated and placed in the appropriate pane.  If running in
     * single-pane mode, a new activity will be launched in which to show the
     * fragment.
     *
     * @param fragmentClass Full name of the class implementing the fragment.
     * @param args Any desired arguments to supply to the fragment.
     * @param titleRes Optional resource identifier of the title of this
     * fragment.
     * @param titleText Optional text of the title of this fragment.
     * @param resultTo Optional fragment that result data should be sent to.
     * If non-NULL, resultTo->OnActivityResult() will be called when this
     * preference panel is done.  The launched panel must use
     * {@link #FinishPreferencePanel(Fragment, Int32, Intent)} when done.
     * @param resultRequestCode If resultTo is non-NULL, this is the caller's
     * request code to be received with the result.
     */
    CARAPI StartPreferencePanel(
        /* [in] */ const String& fragmentClass,
        /* [in] */ IBundle* args,
        /* [in] */ Int32 titleRes,
        /* [in] */ ICharSequence* titleText,
        /* [in] */ IFragment* resultTo,
        /* [in] */ Int32 resultRequestCode);

    /**
     * Start a new fragment in a new activity containing a preference panel for a given user. If the
     * preferences are being displayed in multi-pane mode, the given fragment class will be
     * instantiated and placed in the appropriate pane. If running in single-pane mode, a new
     * activity will be launched in which to show the fragment.
     *
     * @param fragmentClass Full name of the class implementing the fragment.
     * @param args Any desired arguments to supply to the fragment.
     * @param titleRes Optional resource identifier of the title of this fragment.
     * @param titleText Optional text of the title of this fragment.
     * @param userHandle The user for which the panel has to be started.
     */
    CARAPI StartPreferencePanelAsUser(
        /* [in] */ const String& fragmentClass,
        /* [in] */ IBundle* args,
        /* [in] */ Int32 titleRes,
        /* [in] */ ICharSequence* titleText,
        /* [in] */ IUserHandle* userHandle);

    /**
     * Called by a preference panel fragment to finish itself.
     *
     * @param caller The fragment that is asking to be finished.
     * @param resultCode Optional result code to send back to the original
     * launching fragment.
     * @param resultData Optional result data to send back to the original
     * launching fragment.
     */
    CARAPI FinishPreferencePanel(
        /* [in] */ IFragment* caller,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* resultData);

    /**
     * Start a new fragment.
     *
     * @param fragment The fragment to start
     * @param push If TRUE, the current fragment will be pushed onto the back stack.  If FALSE,
     * the current fragment will be replaced.
     */
    CARAPI StartPreferenceFragment(
        /* [in] */ IFragment* fragment,
        /* [in] */ Boolean push);

    // give subclasses access to the Next button
    CARAPI HasNextButton(
        /* [out] */ Boolean* result);

    CARAPI GetNextButton(
        /* [out] */ IButton** button);

    //@Override
    CARAPI ShouldUpRecreateTask(
        /* [in] */ IIntent* targetIntent,
        /* [out] */ Boolean* result);

    static CARAPI RequestHomeNotice();

    //@Override
    CARAPI OnQueryTextSubmit(
        /* [in] */ const String& query,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnQueryTextChange(
        /* [in] */ const String& newText,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnClose(
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnMenuItemActionExpand(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnMenuItemActionCollapse(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    CARAPI NeedToRevertToInitialFragment();

    CARAPI GetResultIntentData(
        /* [out] */ IIntent** intent);

    CARAPI SetResultIntentData(
        /* [in] */ IIntent* resultIntentData);

protected:
    //@Override
     CARAPI OnStart();

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedState);

    //@Override
    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI_(Boolean) IsValidFragment(
        /* [in] */ const String& fragmentName);

private:
    CARAPI_(void) InvalidateCategories(
        /* [in] */ Boolean forceRefresh);

    static CARAPI_(Boolean) IsShortCutIntent(
        /* [in] */ IIntent* intent);

    static CARAPI_(Boolean) IsLikeShortCutIntent(
        /* [in] */ IIntent* intent);

    CARAPI_(Int32) GetHomeActivitiesCount();

    CARAPI_(void) SetTitleFromIntent(
        /* [in] */ IIntent* intent);

    CARAPI_(Int32) SetTitleFromBackStack();

    CARAPI_(void) SetTitleFromBackStackEntry(
        /* [in] */ IFragmentManagerBackStackEntry* bse);

    /**
     * Checks if the component name in the intent is different from the Settings class and
     * returns the class name to load as a fragment.
     */
    CARAPI_(String) GetStartingFragmentClass(
        /* [in] */ IIntent* intent);

    /**
     * Switch to a specific Fragment with taking care of validation, Title and BackStack
     */
    CARAPI SwitchToFragment(
        /* [in] */ const String& fragmentName,
        /* [in] */ IBundle* args,
        /* [in] */ Boolean validate,
        /* [in] */ Boolean addToBackStack,
        /* [in] */ Int32 titleResId,
        /* [in] */ ICharSequence* title,
        /* [in] */ Boolean withTransition,
        /* [out] */ IFragment** fragment);

    /**
     * Called when the activity needs its list of categories/tiles built.
     *
     * @param categories The list in which to place the tiles categories.
     */
    CARAPI BuildDashboardCategories(
        /* [in] */ IList* categories); //List<DashboardCategory>

    /**
     * Parse the given XML file as a categories description, adding each
     * parsed categories and tiles into the target list.
     *
     * @param resid The XML resource to load and parse.
     * @param target The list in which the parsed categories and tiles should be placed.
     */
    CARAPI LoadCategoriesFromResource(
        /* [in] */ Int32 resid,
        /* [in] */ IList* target); //List<DashboardCategory>

    CARAPI UpdateTilesList(
        /* [in] */ IList* target); // List<DashboardCategory>

    CARAPI UpdateHomeSettingTiles(
        /* [in] */ DashboardTile* tile,
        /* [out] */ Boolean* res);

    CARAPI GetMetaData();

    CARAPI_(void) SwitchToSearchResultsFragmentIfNeeded();

    CARAPI_(void) RevertToInitialFragment();

public:
    /**
     * When starting this activity, the invoking Intent can contain this extra
     * string to specify which fragment should be initially displayed.
     * <p/>Starting from Key Lime Pie, when this argument is passed in, the activity
     * will call IsValidFragment() to confirm that the fragment class name is valid for this
     * activity.
     */
    static const String EXTRA_SHOW_FRAGMENT;

    /**
     * When starting this activity and using {@link #EXTRA_SHOW_FRAGMENT},
     * this extra can also be specified to supply a Bundle of arguments to pass
     * to that fragment when it is instantiated during the initial creation
     * of the activity.
     */
    static const String EXTRA_SHOW_FRAGMENT_ARGUMENTS;

    /**
     * Fragment "key" argument passed thru {@link #EXTRA_SHOW_FRAGMENT_ARGUMENTS}
     */
    static const String EXTRA_FRAGMENT_ARG_KEY;

    static const String BACK_STACK_PREFS;

    /**
     * When starting this activity and using {@link #EXTRA_SHOW_FRAGMENT},
     * those extra can also be specify to supply the title or title res id to be shown for
     * that fragment.
     */
    static const String EXTRA_SHOW_FRAGMENT_TITLE;
    static const String EXTRA_SHOW_FRAGMENT_TITLE_RESID;
    static const String EXTRA_SHOW_FRAGMENT_AS_SHORTCUT;

    static const String EXTRA_SHOW_FRAGMENT_AS_SUBSETTING;

private:
    // extras that allow any preference activity to be launched as part of a wizard

    // show Back and Next buttons? takes Boolean parameter
    // Back will then return RESULT_CANCELED and Next RESULT_OK
    static const String EXTRA_PREFS_SHOW_BUTTON_BAR;

    // specify custom text for the Back or Next buttons, or cause a button to not appear
    // at all by setting it to NULL
    static const String EXTRA_PREFS_SET_NEXT_TEXT;
    static const String EXTRA_PREFS_SET_BACK_TEXT;

private:
    static const String LOG_TAG;

    // Constants for state save/restore
    static const String SAVE_KEY_CATEGORIES;
    static const String SAVE_KEY_SEARCH_MENU_EXPANDED;
    static const String SAVE_KEY_SEARCH_QUERY;
    static const String SAVE_KEY_SHOW_HOME_AS_UP;
    static const String SAVE_KEY_SHOW_SEARCH;
    static const String SAVE_KEY_HOME_ACTIVITIES_COUNT;

    // add a Skip button?
    static const String EXTRA_PREFS_SHOW_SKIP;

    static const String META_DATA_KEY_FRAGMENT_CLASS;

    static const String EXTRA_UI_OPTIONS;

    static const String EMPTY_QUERY;

    static Boolean sShowNoHomeNotice;

    String mFragmentClass;

    AutoPtr<ICharSequence> mInitialTitle;
    Int32 mInitialTitleResId;

    // Show only these settings for restricted users
    AutoPtr< ArrayOf<Int32> > SETTINGS_FOR_RESTRICTED;

    static const AutoPtr<ArrayOf<String> > ENTRY_FRAGMENTS;

    static const AutoPtr< ArrayOf<String> > LIKE_SHORTCUT_INTENT_ACTION_ARRAY;

    AutoPtr<ISharedPreferences> mDevelopmentPreferences;
    AutoPtr<ISharedPreferencesOnSharedPreferenceChangeListener> mDevelopmentPreferencesListener;

    Boolean mBatteryPresent = TRUE;
    AutoPtr<IBroadcastReceiver> mBatteryInfoReceiver;

    AutoPtr<DynamicIndexableContentMonitor> mDynamicIndexableContentMonitor;
        // = new DynamicIndexableContentMonitor();

    AutoPtr<IActionBar> mActionBar;
    AutoPtr<ISwitchBar> mSwitchBar;

    AutoPtr<IButton> mNextButton;

    Boolean mDisplayHomeAsUpEnabled;
    Boolean mDisplaySearch;

    Boolean mIsShowingDashboard;
    Boolean mIsShortcut;

    AutoPtr<IViewGroup> mContent;

    AutoPtr<ISearchView> mSearchView;
    AutoPtr<IMenuItem> mSearchMenuItem;
    Boolean mSearchMenuItemExpanded;
    AutoPtr<ISearchResultsSummary> mSearchResultsFragment;
    String mSearchQuery;

    // Categories
    AutoPtr<IArrayList> mCategories;// = new ArrayList<DashboardCategory>();

    static const String MSG_DATA_FORCE_REFRESH;
    static const Int32 MSG_BUILD_CATEGORIES = 1;
    AutoPtr<BuildCategoriesHandler> mHandler;

    Boolean mNeedToRevertToInitialFragment;
    Int32 mHomeActivitiesCount;

    AutoPtr<IIntent> mResultIntentData;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SETTINGSACTIVITY_H__
