
#ifndef __ELASTOS_DROID_SETTINGS_SETTINGS_ACTIVITY_H__
#define __ELASTOS_DROID_SETTINGS_SETTINGS_ACTIVITY_H__

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
    , public IPreferenceManagerOnPreferenceTreeClickListener,
    , public IPreferenceFragmentOnPreferenceStartFragmentCallback,
    , public IButtonBarHandler,
    , public IFragmentManagerOnBackStackChangedListener,
    , public ISearchViewOnQueryTextListener,
    , public ISearchViewOnCloseListener,
    , public IMenuItemOnActionExpandListener
{
private:
    class BatteryInfoReceiver
        : public BroadcastReceiver
    {
    public:
        CARAPI constructor(
            /* [in] */ SettingsActivity* host);

        //@Override
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent)
        {
            String action = intent->GetAction();
            if (IIntent::ACTION_BATTERY_CHANGED->Equals(action)) {
                Boolean batteryPresent = Utils->IsBatteryPresent(intent);

                if (mBatteryPresent != batteryPresent) {
                    mBatteryPresent = batteryPresent;
                    InvalidateCategories(TRUE);
                }
            }
        }

    private:
        SettingsActivity* mHost;
    };

    class BuildCategoriesHandler
        : public Handler()
    {
    public:
        CARAPI constructor(
            /* [in] */ SettingsActivity* host);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg)
        {
            switch (msg.what) {
                case MSG_BUILD_CATEGORIES: {
                    final Boolean forceRefresh = msg->GetData()->GetBoolean(MSG_DATA_FORCE_REFRESH);
                    if (forceRefresh) {
                        BuildDashboardCategories(mCategories);
                    }
                } break;
            }
        }
    private:
        SettingsActivity* mHost;
    };

public:

    CARAPI GetSwitchBar(
        /* [out] */ ISwitchBar** switchBar)
    {
        return mSwitchBar;
    }

    CARAPI GetDashboardCategories(
        /* [in] */ Boolean forceRefresh,
        /* [out] */ IList** categories) //List<DashboardCategory>
    {
        if (forceRefresh || mCategories->Size() == 0) {
            BuildDashboardCategories(mCategories);
        }
        return mCategories;
    }

    //@Override
    CARAPI OnPreferenceStartFragment(
        /* [in] */ IPreferenceFragment* caller,
        /* [in] */ IPreference* pref,
        /* [out] */ Boolean* result)
    {
        // Override the fragment title for Wallpaper settings
        Int32 titleRes = pref->GetTitleRes();
        if (pref->GetFragment()->Equals(WallpaperTypeSettings.class->GetName())) {
            titleRes = R::string::wallpaper_settings_fragment_title;
        } else if (pref->GetFragment()->Equals(OwnerInfoSettings.class->GetName())
                && UserHandle->MyUserId() != UserHandle.USER_OWNER) {
            if (UserManager->Get(this).IsLinkedUser()) {
                titleRes = R::string::profile_info_settings_title;
            } else {
                titleRes = R::string::user_info_settings_title;
            }
        }
        StartPreferencePanel(pref->GetFragment(), pref->GetExtras(), titleRes, pref->GetTitle(),
                NULL, 0);
        return TRUE;
    }

    //@Override
    CARAPI OnPreferenceTreeClick(
        /* [in] */ PreferenceScreen* preferenceScreen,
        /* [in] */ IPreference* pref,
        /* [out] */ Boolean* result)
    (
        return FALSE;
    }

    private void InvalidateCategories(
        /* [in] */ Boolean forceRefresh)
    {
        if (!mHandler->HasMessages(MSG_BUILD_CATEGORIES)) {
            Message msg = new Message();
            msg.what = MSG_BUILD_CATEGORIES;
            msg->GetData()->PutBoolean(MSG_DATA_FORCE_REFRESH, forceRefresh);
        }
    }

    //@Override
    CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig)
    {
        super->OnConfigurationChanged(newConfig);
        Index->GetInstance(this).Update();
    }

    //@Override
    protected CARAPI OnStart()
    {
        super->OnStart();

        if (mNeedToRevertToInitialFragment) {
            RevertToInitialFragment();
        }
    }

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result)
    {
        if (!mDisplaySearch) {
            return FALSE;
        }

        MenuInflater inflater = GetMenuInflater();
        inflater->Inflate(R.menu.options_menu, menu);

        // Cache the search query (can be overriden by the OnQueryTextListener)
        final String query = mSearchQuery;

        mSearchMenuItem = menu->FindItem(R.id.search);
        mSearchView = (SearchView) mSearchMenuItem->GetActionView();

        if (mSearchMenuItem == NULL || mSearchView == NULL) {
            return FALSE;
        }

        if (mSearchResultsFragment != NULL) {
            mSearchResultsFragment->SetSearchView(mSearchView);
        }

        mSearchMenuItem->SetOnActionExpandListener(this);
        mSearchView->SetOnQueryTextListener(this);
        mSearchView->SetOnCloseListener(this);

        if (mSearchMenuItemExpanded) {
            mSearchMenuItem->ExpandActionView();
        }
        mSearchView->SetQuery(query, TRUE /* submit */);

        return TRUE;
    }

    private static Boolean IsShortCutIntent(
        /* [in] */ IIntent* intent)
    {
        Set<String> categories = intent->GetCategories();
        return (categories != NULL) && categories->Contains("com.android.settings.SHORTCUT");
    }

    private static Boolean IsLikeShortCutIntent(
        /* [in] */ IIntent* intent)
    {
        String action = intent->GetAction();
        if (action == NULL) {
            return FALSE;
        }
        for (Int32 i = 0; i < LIKE_SHORTCUT_INTENT_ACTION_ARRAY.length; i++) {
            if (LIKE_SHORTCUT_INTENT_ACTION_ARRAY[i].Equals(action)) return TRUE;
        }
        return FALSE;
    }

    //@Override
    protected CARAPI OnCreate(
        /* [in] */ IBundle* savedState)
    {
        super->OnCreate(savedState);

        // Should happen before any call to GetIntent()
        GetMetaData();

        final Intent intent = GetIntent();
        if (intent->HasExtra(EXTRA_UI_OPTIONS)) {
            GetWindow()->SetUiOptions(intent->GetIntExtra(EXTRA_UI_OPTIONS, 0));
        }

        mDevelopmentPreferences = GetSharedPreferences(DevelopmentSettings.PREF_FILE,
                Context.MODE_PRIVATE);

        // Getting Intent properties can only be done after the super->OnCreate(...)
        final String initialFragmentName = intent->GetStringExtra(EXTRA_SHOW_FRAGMENT);

        mIsShortcut = IsShortCutIntent(intent) || IsLikeShortCutIntent(intent) ||
                intent->GetBooleanExtra(EXTRA_SHOW_FRAGMENT_AS_SHORTCUT, FALSE);

        final ComponentName cn = intent->GetComponent();
        final String className = cn->GetClassName();

        mIsShowingDashboard = className->Equals(Settings.class->GetName());

        // This is a "Sub Settings" when:
        // - this is a real SubSettings
        // - or :settings:show_fragment_as_subsetting is passed to the Intent
        final Boolean isSubSettings = className->Equals(SubSettings.class->GetName()) ||
                intent->GetBooleanExtra(EXTRA_SHOW_FRAGMENT_AS_SUBSETTING, FALSE);

        // If this is a sub settings, then apply the SubSettings Theme for the ActionBar content insets
        if (isSubSettings) {
            // Check also that we are not a Theme Dialog as we don't want to override them
            final Int32 themeResId = GetThemeResId();
            if (themeResId != R.style.Theme_DialogWhenLarge &&
                    themeResId != R.style.Theme_SubSettingsDialogWhenLarge) {
                SetTheme(R.style.Theme_SubSettings);
            }
        }

        SetContentView(mIsShowingDashboard ?
                R.layout.settings_main_dashboard : R.layout.settings_main_prefs);

        mContent = (ViewGroup) FindViewById(R.id.main_content);

        GetFragmentManager()->AddOnBackStackChangedListener(this);

        if (mIsShowingDashboard) {
            Index->GetInstance(GetApplicationContext()).Update();
        }

        if (savedState != NULL) {
            // We are restarting from a previous saved state; used that to initialize, instead
            // of starting fresh.
            mSearchMenuItemExpanded = savedState->GetBoolean(SAVE_KEY_SEARCH_MENU_EXPANDED);
            mSearchQuery = savedState->GetString(SAVE_KEY_SEARCH_QUERY);

            SetTitleFromIntent(intent);

            ArrayList<DashboardCategory> categories =
                    savedState->GetParcelableArrayList(SAVE_KEY_CATEGORIES);
            if (categories != NULL) {
                mCategories->Clear();
                mCategories->AddAll(categories);
                SetTitleFromBackStack();
            }

            mDisplayHomeAsUpEnabled = savedState->GetBoolean(SAVE_KEY_SHOW_HOME_AS_UP);
            mDisplaySearch = savedState->GetBoolean(SAVE_KEY_SHOW_SEARCH);
            mHomeActivitiesCount = savedState->GetInt(SAVE_KEY_HOME_ACTIVITIES_COUNT,
                    1 /* one home activity by default */);
        } else {
            if (!mIsShowingDashboard) {
                // Search is shown we are launched thru a Settings "shortcut". UP will be shown
                // only if it is a sub settings
                if (mIsShortcut) {
                    mDisplayHomeAsUpEnabled = isSubSettings;
                    mDisplaySearch = FALSE;
                } else if (isSubSettings) {
                    mDisplayHomeAsUpEnabled = TRUE;
                    mDisplaySearch = TRUE;
                } else {
                    mDisplayHomeAsUpEnabled = FALSE;
                    mDisplaySearch = FALSE;
                }
                SetTitleFromIntent(intent);

                Bundle initialArguments = intent->GetBundleExtra(EXTRA_SHOW_FRAGMENT_ARGUMENTS);
                SwitchToFragment(initialFragmentName, initialArguments, TRUE, FALSE,
                        mInitialTitleResId, mInitialTitle, FALSE);
            } else {
                // No UP affordance if we are displaying the main Dashboard
                mDisplayHomeAsUpEnabled = FALSE;
                // Show Search affordance
                mDisplaySearch = TRUE;
                mInitialTitleResId = R::string::dashboard_title;
                SwitchToFragment(DashboardSummary.class->GetName(), NULL, FALSE, FALSE,
                        mInitialTitleResId, mInitialTitle, FALSE);
            }
        }

        mActionBar = GetActionBar();
        if (mActionBar != NULL) {
            mActionBar->SetDisplayHomeAsUpEnabled(mDisplayHomeAsUpEnabled);
            mActionBar->SetHomeButtonEnabled(mDisplayHomeAsUpEnabled);
        }
        mSwitchBar = (SwitchBar) FindViewById(R.id.switch_bar);

        // see if we should show Back/Next buttons
        if (intent->GetBooleanExtra(EXTRA_PREFS_SHOW_BUTTON_BAR, FALSE)) {

            View buttonBar = FindViewById(R.id.button_bar);
            if (buttonBar != NULL) {
                buttonBar->SetVisibility(View.VISIBLE);

                Button backButton = (Button)FindViewById(R.id.back_button);
                backButton->SetOnClickListener(new OnClickListener() {
                    CARAPI OnClick(View v) {
                        SetResult(RESULT_CANCELED, GetResultIntentData());
                        Finish();
                    }
                });
                Button skipButton = (Button)FindViewById(R.id.skip_button);
                skipButton->SetOnClickListener(new OnClickListener() {
                    CARAPI OnClick(View v) {
                        SetResult(RESULT_OK, GetResultIntentData());
                        Finish();
                    }
                });
                mNextButton = (Button)FindViewById(R.id.next_button);
                mNextButton->SetOnClickListener(new OnClickListener() {
                    CARAPI OnClick(View v) {
                        SetResult(RESULT_OK, GetResultIntentData());
                        Finish();
                    }
                });

                // set our various button parameters
                if (intent->HasExtra(EXTRA_PREFS_SET_NEXT_TEXT)) {
                    String buttonText = intent->GetStringExtra(EXTRA_PREFS_SET_NEXT_TEXT);
                    if (TextUtils->IsEmpty(buttonText)) {
                        mNextButton->SetVisibility(View.GONE);
                    }
                    else {
                        mNextButton->SetText(buttonText);
                    }
                }
                if (intent->HasExtra(EXTRA_PREFS_SET_BACK_TEXT)) {
                    String buttonText = intent->GetStringExtra(EXTRA_PREFS_SET_BACK_TEXT);
                    if (TextUtils->IsEmpty(buttonText)) {
                        backButton->SetVisibility(View.GONE);
                    }
                    else {
                        backButton->SetText(buttonText);
                    }
                }
                if (intent->GetBooleanExtra(EXTRA_PREFS_SHOW_SKIP, FALSE)) {
                    skipButton->SetVisibility(View.VISIBLE);
                }
            }
        }

        mHomeActivitiesCount = GetHomeActivitiesCount();
    }

    private Int32 GetHomeActivitiesCount()
    {
        final ArrayList<ResolveInfo> homeApps = new ArrayList<ResolveInfo>();
        GetPackageManager()->GetHomeActivities(homeApps);
        return homeApps->Size();
    }

    private void SetTitleFromIntent(
        /* [in] */ IIntent* intent)
    {
        final Int32 initialTitleResId = intent->GetIntExtra(EXTRA_SHOW_FRAGMENT_TITLE_RESID, -1);
        if (initialTitleResId > 0) {
            mInitialTitle = NULL;
            mInitialTitleResId = initialTitleResId;
            SetTitle(mInitialTitleResId);
        } else {
            mInitialTitleResId = -1;
            final String initialTitle = intent->GetStringExtra(EXTRA_SHOW_FRAGMENT_TITLE);
            mInitialTitle = (initialTitle != NULL) ? initialTitle : GetTitle();
            SetTitle(mInitialTitle);
        }
    }

    //@Override
    CARAPI OnBackStackChanged()
    {
        SetTitleFromBackStack();
    }

    private Int32 SetTitleFromBackStack()
    {
        final Int32 count = GetFragmentManager()->GetBackStackEntryCount();

        if (count == 0) {
            if (mInitialTitleResId > 0) {
                SetTitle(mInitialTitleResId);
            } else {
                SetTitle(mInitialTitle);
            }
            return 0;
        }

        FragmentManager.BackStackEntry bse = GetFragmentManager()->GetBackStackEntryAt(count - 1);
        SetTitleFromBackStackEntry(bse);

        return count;
    }

    private void SetTitleFromBackStackEntry(
        /* [in] */ IFragmentManagerBackStackEntry* bse)
    {
        final CharSequence title;
        final Int32 titleRes = bse->GetBreadCrumbTitleRes();
        if (titleRes > 0) {
            title = GetText(titleRes);
        } else {
            title = bse->GetBreadCrumbTitle();
        }
        if (title != NULL) {
            SetTitle(title);
        }
    }

    //@Override
    protected CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState)
    {
        super->OnSaveInstanceState(outState);

        if (mCategories->Size() > 0) {
            outState->PutParcelableArrayList(SAVE_KEY_CATEGORIES, mCategories);
        }

        outState->PutBoolean(SAVE_KEY_SHOW_HOME_AS_UP, mDisplayHomeAsUpEnabled);
        outState->PutBoolean(SAVE_KEY_SHOW_SEARCH, mDisplaySearch);

        if (mDisplaySearch) {
            // The option menus are created if the ActionBar is visible and they are also created
            // asynchronously. If you launch Settings with an Intent action like
            // android.intent.action.POWER_USAGE_SUMMARY and at the same time your device is locked
            // thru a LockScreen, OnCreateOptionsMenu() is not yet called and references to the search
            // menu item and search view are NULL.
            Boolean isExpanded = (mSearchMenuItem != NULL) && mSearchMenuItem->IsActionViewExpanded();
            outState->PutBoolean(SAVE_KEY_SEARCH_MENU_EXPANDED, isExpanded);

            String query = (mSearchView != NULL) ? mSearchView->GetQuery()->ToString() : EMPTY_QUERY;
            outState->PutString(SAVE_KEY_SEARCH_QUERY, query);
        }

        outState->PutInt(SAVE_KEY_HOME_ACTIVITIES_COUNT, mHomeActivitiesCount);
    }

    //@Override
    CARAPI OnResume()
    {
        super->OnResume();

        final Int32 newHomeActivityCount = GetHomeActivitiesCount();
        if (newHomeActivityCount != mHomeActivitiesCount) {
            mHomeActivitiesCount = newHomeActivityCount;
            InvalidateCategories(TRUE);
        }

        mDevelopmentPreferencesListener = new SharedPreferences->OnSharedPreferenceChangeListener() {
            //@Override
            CARAPI OnSharedPreferenceChanged(SharedPreferences sharedPreferences, String key) {
                InvalidateCategories(TRUE);
            }
        };
        mDevelopmentPreferences->RegisterOnSharedPreferenceChangeListener(
                mDevelopmentPreferencesListener);

        RegisterReceiver(mBatteryInfoReceiver, new IntentFilter(IIntent::ACTION_BATTERY_CHANGED));

        mDynamicIndexableContentMonitor->Register(this);

        If(mDisplaySearch && !TextUtils->IsEmpty(mSearchQuery)) {
            OnQueryTextSubmit(mSearchQuery);
        }
    }

    //@Override
    CARAPI OnPause()
    {
        super->OnPause();

        UnregisterReceiver(mBatteryInfoReceiver);
        mDynamicIndexableContentMonitor->Unregister();
    }

    //@Override
    CARAPI OnDestroy()
    {
        super->OnDestroy();

        mDevelopmentPreferences->UnregisterOnSharedPreferenceChangeListener(
                mDevelopmentPreferencesListener);
        mDevelopmentPreferencesListener = NULL;
    }

    protected Boolean IsValidFragment(
        /* [in] */ const String& fragmentName)
    {
        // Almost all fragments are wrapped in this,
        // except for a few that have their own activities.
        for (Int32 i = 0; i < ENTRY_FRAGMENTS.length; i++) {
            if (ENTRY_FRAGMENTS[i].Equals(fragmentName)) return TRUE;
        }
        return FALSE;
    }

    //@Override
    CARAPI GetIntent(
        /* [out] */ IIntent** intent)
    {
        Intent superIntent = super->GetIntent();
        String startingFragment = GetStartingFragmentClass(superIntent);
        // This is called from super.onCreate, IsMultiPane() is not yet reliable
        // Do not use onIsHidingHeaders either, which relies itself on this method
        if (startingFragment != NULL) {
            Intent modIntent = new Intent(superIntent);
            modIntent->PutExtra(EXTRA_SHOW_FRAGMENT, startingFragment);
            Bundle args = superIntent->GetExtras();
            if (args != NULL) {
                args = new Bundle(args);
            } else {
                args = new Bundle();
            }
            args->PutParcelable("intent", superIntent);
            modIntent->PutExtra(EXTRA_SHOW_FRAGMENT_ARGUMENTS, args);
            return modIntent;
        }
        return superIntent;
    }

    /**
     * Checks if the component name in the intent is different from the Settings class and
     * returns the class name to load as a fragment.
     */
    private String GetStartingFragmentClass(
        /* [in] */ IIntent* intent)
    {
        if (mFragmentClass != NULL) return mFragmentClass;

        String intentClass = intent->GetComponent()->GetClassName();
        if (intentClass->Equals(GetClass()->GetName())) return NULL;

        if ("com.android.settings.ManageApplications".Equals(intentClass)
                || "com.android.settings.RunningServices".Equals(intentClass)
                || "com.android.settings.applications.StorageUse".Equals(intentClass)) {
            // Old names of manage apps.
            intentClass = com.android.settings.applications.ManageApplications.class->GetName();
        }

        return intentClass;
    }

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
        /* [in] */ Int32 resultRequestCode)
    {
        String title = NULL;
        if (titleRes < 0) {
            if (titleText != NULL) {
                title = titleText->ToString();
            } else {
                // There not much we can do in that case
                title = "";
            }
        }
        Utils->StartWithFragment(this, fragmentClass, args, resultTo, resultRequestCode,
                titleRes, title, mIsShortcut);
    }

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
        /* [in] */ IUserHandle* userHandle)
    {
        String title = NULL;
        if (titleRes < 0) {
            if (titleText != NULL) {
                title = titleText->ToString();
            } else {
                // There not much we can do in that case
                title = "";
            }
        }
        Utils->StartWithFragmentAsUser(this, fragmentClass, args,
                titleRes, title, mIsShortcut, userHandle);
    }

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
        /* [in] */ IIntent* resultData)
    {
        SetResult(resultCode, resultData);
        Finish();
    }

    /**
     * Start a new fragment.
     *
     * @param fragment The fragment to start
     * @param push If TRUE, the current fragment will be pushed onto the back stack.  If FALSE,
     * the current fragment will be replaced.
     */
    CARAPI StartPreferenceFragment(
        /* [in] */ IFragment* fragment,
        /* [in] */ Boolean push)
    {
        FragmentTransaction transaction = GetFragmentManager()->BeginTransaction();
        transaction->Replace(R.id.main_content, fragment);
        if (push) {
            transaction->SetTransition(FragmentTransaction.TRANSIT_FRAGMENT_OPEN);
            transaction->AddToBackStack(BACK_STACK_PREFS);
        } else {
            transaction->SetTransition(FragmentTransaction.TRANSIT_FRAGMENT_FADE);
        }
        transaction->CommitAllowingStateLoss();
    }

    /**
     * Switch to a specific Fragment with taking care of validation, Title and BackStack
     */
    private Fragment SwitchToFragment(
        /* [in] */ const String& fragmentName,
        /* [in] */ IBundle* args,
        /* [in] */ Boolean validate,
        /* [in] */ Boolean addToBackStack,
        /* [in] */ Int32 titleResId,
        /* [in] */ ICharSequence* title,
        /* [in] */ Boolean withTransition)
    {
        if (validate && !IsValidFragment(fragmentName)) {
            throw new IllegalArgumentException("Invalid fragment for this activity: "
                    + fragmentName);
        }
        Fragment f = Fragment->Instantiate(this, fragmentName, args);
        FragmentTransaction transaction = GetFragmentManager()->BeginTransaction();
        transaction->Replace(R.id.main_content, f);
        if (withTransition) {
            TransitionManager->BeginDelayedTransition(mContent);
        }
        if (addToBackStack) {
            transaction->AddToBackStack(SettingsActivity.BACK_STACK_PREFS);
        }
        if (titleResId > 0) {
            transaction->SetBreadCrumbTitle(titleResId);
        } else if (title != NULL) {
            transaction->SetBreadCrumbTitle(title);
        }
        transaction->CommitAllowingStateLoss();
        GetFragmentManager()->ExecutePendingTransactions();
        return f;
    }

    /**
     * Called when the activity needs its list of categories/tiles built.
     *
     * @param categories The list in which to place the tiles categories.
     */
    private void BuildDashboardCategories(
        /* [in] */ IList* categories) //List<DashboardCategory>
    {
        categories->Clear();
        LoadCategoriesFromResource(R.xml.dashboard_categories, categories);
        UpdateTilesList(categories);
    }

    /**
     * Parse the given XML file as a categories description, adding each
     * parsed categories and tiles into the target list.
     *
     * @param resid The XML resource to load and parse.
     * @param target The list in which the parsed categories and tiles should be placed.
     */
    private void LoadCategoriesFromResource(
        /* [in] */ Int32 resid,
        /* [in] */ IList* target) //List<DashboardCategory>
    {
        XmlResourceParser parser = NULL;
        try {
            parser = GetResources()->GetXml(resid);
            AttributeSet attrs = Xml->AsAttributeSet(parser);

            Int32 type;
            while ((type=parser->Next()) != XmlPullParser.END_DOCUMENT
                    && type != XmlPullParser.START_TAG) {
                // Parse next until start tag is found
            }

            String nodeName = parser->GetName();
            if (!"dashboard-categories".Equals(nodeName)) {
                throw new RuntimeException(
                        "XML document must start with <preference-categories> tag; found"
                                + nodeName + " at " + parser->GetPositionDescription());
            }

            Bundle curBundle = NULL;

            final Int32 outerDepth = parser->GetDepth();
            while ((type=parser->Next()) != XmlPullParser.END_DOCUMENT
                    && (type != XmlPullParser.END_TAG || parser->GetDepth() > outerDepth)) {
                if (type == XmlPullParser.END_TAG || type == XmlPullParser.TEXT) {
                    continue;
                }

                nodeName = parser->GetName();
                if ("dashboard-category".Equals(nodeName)) {
                    DashboardCategory category = new DashboardCategory();

                    TypedArray sa = ObtainStyledAttributes(
                            attrs, R.styleable.PreferenceHeader);
                    category.id = sa->GetResourceId(
                            R.styleable.PreferenceHeader_id,
                            (Int32)DashboardCategory.CAT_ID_UNDEFINED);

                    TypedValue tv = sa->PeekValue(
                            R.styleable.PreferenceHeader_title);
                    if (tv != NULL && tv.type == TypedValue.TYPE_STRING) {
                        if (tv.resourceId != 0) {
                            category.titleRes = tv.resourceId;
                        } else {
                            category.title = tv.string;
                        }
                    }
                    sa->Recycle();

                    final Int32 innerDepth = parser->GetDepth();
                    while ((type=parser->Next()) != XmlPullParser.END_DOCUMENT
                            && (type != XmlPullParser.END_TAG || parser->GetDepth() > innerDepth)) {
                        if (type == XmlPullParser.END_TAG || type == XmlPullParser.TEXT) {
                            continue;
                        }

                        String innerNodeName = parser->GetName();
                        if (innerNodeName->Equals("dashboard-tile")) {
                            DashboardTile tile = new DashboardTile();

                            sa = ObtainStyledAttributes(
                                    attrs, R.styleable.PreferenceHeader);
                            tile.id = sa->GetResourceId(
                                    R.styleable.PreferenceHeader_id,
                                    (Int32)TILE_ID_UNDEFINED);
                            tv = sa->PeekValue(
                                    R.styleable.PreferenceHeader_title);
                            if (tv != NULL && tv.type == TypedValue.TYPE_STRING) {
                                if (tv.resourceId != 0) {
                                    tile.titleRes = tv.resourceId;
                                } else {
                                    tile.title = tv.string;
                                }
                            }
                            tv = sa->PeekValue(
                                    R.styleable.PreferenceHeader_summary);
                            if (tv != NULL && tv.type == TypedValue.TYPE_STRING) {
                                if (tv.resourceId != 0) {
                                    tile.summaryRes = tv.resourceId;
                                } else {
                                    tile.summary = tv.string;
                                }
                            }
                            tile.iconRes = sa->GetResourceId(
                                    R.styleable.PreferenceHeader_icon, 0);
                            tile.fragment = sa->GetString(
                                    R.styleable.PreferenceHeader_fragment);
                            sa->Recycle();

                            if (curBundle == NULL) {
                                curBundle = new Bundle();
                            }

                            final Int32 innerDepth2 = parser->GetDepth();
                            while ((type=parser->Next()) != XmlPullParser.END_DOCUMENT
                                    && (type != XmlPullParser.END_TAG || parser->GetDepth() > innerDepth2)) {
                                if (type == XmlPullParser.END_TAG || type == XmlPullParser.TEXT) {
                                    continue;
                                }

                                String innerNodeName2 = parser->GetName();
                                if (innerNodeName2->Equals("extra")) {
                                    GetResources()->ParseBundleExtra("extra", attrs, curBundle);
                                    XmlUtils->SkipCurrentTag(parser);

                                } else if (innerNodeName2->Equals("intent")) {
                                    tile.intent = Intent->ParseIntent(GetResources(), parser, attrs);

                                } else {
                                    XmlUtils->SkipCurrentTag(parser);
                                }
                            }

                            if (curBundle->Size() > 0) {
                                tile.fragmentArguments = curBundle;
                                curBundle = NULL;
                            }

                            // Show the SIM Cards setting if there are more than 2 SIMs installed.
                            If(tile.id != R.id.sim_settings || Utils->ShowSimCardTile(this)){
                                category->AddTile(tile);
                            }

                        } else {
                            XmlUtils->SkipCurrentTag(parser);
                        }
                    }

                    target->Add(category);
                } else {
                    XmlUtils->SkipCurrentTag(parser);
                }
            }

        } catch (XmlPullParserException e) {
            throw new RuntimeException("Error parsing categories", e);
        } catch (IOException e) {
            throw new RuntimeException("Error parsing categories", e);
        } finally {
            if (parser != NULL) parser->Close();
        }
    }

    private void UpdateTilesList(
        /* [in] */ IList* target) // List<DashboardCategory>
    {
        final Boolean showDev = mDevelopmentPreferences->GetBoolean(
                DevelopmentSettings.PREF_SHOW,
                android.os.Build.TYPE->Equals("eng"));

        final UserManager um = (UserManager) GetSystemService(Context.USER_SERVICE);

        final Int32 size = target->Size();
        for (Int32 i = 0; i < size; i++) {

            DashboardCategory category = target->Get(i);

            // Ids are integers, so downcasting is ok
            Int32 id = (Int32) category.id;
            Int32 n = category->GetTilesCount() - 1;
            while (n >= 0) {

                DashboardTile tile = category->GetTile(n);
                Boolean removeTile = FALSE;
                id = (Int32) tile.id;
                if (id == R.id.operator_settings || id == R.id.manufacturer_settings) {
                    if (!Utils->UpdateTileToSpecificActivityFromMetaDataOrRemove(this, tile)) {
                        removeTile = TRUE;
                    }
                } else if (id == R.id.wifi_settings) {
                    // Remove WiFi Settings if WiFi service is not available.
                    if (!GetPackageManager()->HasSystemFeature(PackageManager.FEATURE_WIFI)) {
                        removeTile = TRUE;
                    }
                } else if (id == R.id.bluetooth_settings) {
                    // Remove Bluetooth Settings if Bluetooth service is not available.
                    if (!GetPackageManager()->HasSystemFeature(PackageManager.FEATURE_BLUETOOTH)) {
                        removeTile = TRUE;
                    }
                } else if (id == R.id.data_usage_settings) {
                    // Remove data usage when kernel module not enabled
                    final INetworkManagementService netManager = INetworkManagementService.Stub
                            .AsInterface(ServiceManager->GetService(Context.NETWORKMANAGEMENT_SERVICE));
                    try {
                        if (!netManager->IsBandwidthControlEnabled()) {
                            removeTile = TRUE;
                        }
                    } catch (RemoteException e) {
                        // ignored
                    }
                } else if (id == R.id.battery_settings) {
                    // Remove battery settings when battery is not available. (e.g. TV)

                    if (!mBatteryPresent) {
                        removeTile = TRUE;
                    }
                } else if (id == R.id.home_settings) {
                    if (!UpdateHomeSettingTiles(tile)) {
                        removeTile = TRUE;
                    }
                } else if (id == R.id.user_settings) {
                    Boolean hasMultipleUsers =
                            ((UserManager) GetSystemService(Context.USER_SERVICE))
                                    .GetUserCount() > 1;
                    if (!UserHandle.MU_ENABLED
                            || (!UserManager->SupportsMultipleUsers()
                                    && !hasMultipleUsers)
                            || Utils->IsMonkeyRunning()) {
                        removeTile = TRUE;
                    }
                } else if (id == R.id.nfc_payment_settings) {
                    if (!GetPackageManager()->HasSystemFeature(PackageManager.FEATURE_NFC)) {
                        removeTile = TRUE;
                    } else {
                        // Only show if NFC is on and we have the HCE feature
                        NfcAdapter adapter = NfcAdapter->GetDefaultAdapter(this);
                        if (adapter == NULL || !adapter->IsEnabled() ||
                                !GetPackageManager()->HasSystemFeature(
                                        PackageManager.FEATURE_NFC_HOST_CARD_EMULATION)) {
                            removeTile = TRUE;
                        }
                    }
                } else if (id == R.id.print_settings) {
                    Boolean hasPrintingSupport = GetPackageManager()->HasSystemFeature(
                            PackageManager.FEATURE_PRINTING);
                    if (!hasPrintingSupport) {
                        removeTile = TRUE;
                    }
                } else if (id == R.id.development_settings) {
                    if (!showDev || um->HasUserRestriction(
                            UserManager.DISALLOW_DEBUGGING_FEATURES)) {
                        removeTile = TRUE;
                    }
                }

                if (UserHandle.MU_ENABLED && UserHandle->MyUserId() != 0
                        && !ArrayUtils->Contains(SETTINGS_FOR_RESTRICTED, id)) {
                    removeTile = TRUE;
                }

                if (removeTile && n < category->GetTilesCount()) {
                    category->RemoveTile(n);
                }
                n--;
            }
        }
    }

    private Boolean UpdateHomeSettingTiles(
        /* [in] */ IDashboardTile* tile)
    {
        // Once we decide to show Home settings, keep showing it forever
        SharedPreferences sp = GetSharedPreferences(HomeSettings.HOME_PREFS, Context.MODE_PRIVATE);
        if (sp->GetBoolean(HomeSettings.HOME_PREFS_DO_SHOW, FALSE)) {
            return TRUE;
        }

        try {
            mHomeActivitiesCount = GetHomeActivitiesCount();
            if (mHomeActivitiesCount < 2) {
                // When there's only one available home app, omit this settings
                // category entirely at the top level UI.  If the user just
                // uninstalled the penultimate home app candidiate, we also
                // now tell them about why they aren't seeing 'Home' in the list.
                if (sShowNoHomeNotice) {
                    sShowNoHomeNotice = FALSE;
                    NoHomeDialogFragment->Show(this);
                }
                return FALSE;
            } else {
                // Okay, we're allowing the Home settings category.  Tell it, when
                // invoked via this front door, that we'll need to be told about the
                // case when the user uninstalls all but one home app.
                if (tile.fragmentArguments == NULL) {
                    tile.fragmentArguments = new Bundle();
                }
                tile.fragmentArguments->PutBoolean(HomeSettings.HOME_SHOW_NOTICE, TRUE);
            }
        } catch (Exception e) {
            // Can't look up the home activity; bail on configuring the icon
            Logger::W(LOG_TAG, "Problem looking up home activity!", e);
        }

        sp->Edit()->PutBoolean(HomeSettings.HOME_PREFS_DO_SHOW, TRUE).Apply();
        return TRUE;
    }

    private void GetMetaData()
    {
        try {
            ActivityInfo ai = GetPackageManager()->GetActivityInfo(GetComponentName(),
                    PackageManager.GET_META_DATA);
            if (ai == NULL || ai.metaData == NULL) return;
            mFragmentClass = ai.metaData->GetString(META_DATA_KEY_FRAGMENT_CLASS);
        } catch (NameNotFoundException nnfe) {
            // No recovery
            Logger::D(LOG_TAG, "Cannot get Metadata for: " + GetComponentName()->ToString());
        }
    }

    // give subclasses access to the Next button
    CARAPI HasNextButton(
        /* [out] */ Boolean* result)
    {
        return mNextButton != NULL;
    }

    CARAPI GetNextButton(
        /* [out] */ IButton** button)
    {
        return mNextButton;
    }

    //@Override
    CARAPI ShouldUpRecreateTask(
        /* [in] */ IIntent* targetIntent,
        /* [out] */ Boolean* result)
    {
        return super->ShouldUpRecreateTask(new Intent(this, SettingsActivity.class));
    }

    public static CARAPI RequestHomeNotice()
    {
        sShowNoHomeNotice = TRUE;
    }

    //@Override
    public CARAPI OnQueryTextSubmit(
        /* [in] */ const String& query,
        /* [out] */ Boolean* result)
    {
        SwitchToSearchResultsFragmentIfNeeded();
        mSearchQuery = query;
        return mSearchResultsFragment->OnQueryTextSubmit(query);
    }

    //@Override
    public CARAPI OnQueryTextChange(
        /* [in] */ const String& newText,
        /* [out] */ Boolean* result)
    {
        mSearchQuery = newText;
        if (mSearchResultsFragment == NULL) {
            return FALSE;
        }
        return mSearchResultsFragment->OnQueryTextChange(newText);
    }

    //@Override
    public CARAPI OnClose(
        /* [out] */ Boolean* result)
    {
        return FALSE;
    }

    //@Override
    public CARAPI OnMenuItemActionExpand(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result)
    {
        if (item->GetItemId() == mSearchMenuItem->GetItemId()) {
            SwitchToSearchResultsFragmentIfNeeded();
        }
        return TRUE;
    }

    //@Override
    public CARAPI OnMenuItemActionCollapse(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result)
    {
        if (item->GetItemId() == mSearchMenuItem->GetItemId()) {
            if (mSearchMenuItemExpanded) {
                RevertToInitialFragment();
            }
        }
        return TRUE;
    }

    private void SwitchToSearchResultsFragmentIfNeeded()
    {
        if (mSearchResultsFragment != NULL) {
            return;
        }
        Fragment current = GetFragmentManager()->FindFragmentById(R.id.main_content);
        if (current != NULL && current instanceof SearchResultsSummary) {
            mSearchResultsFragment = (SearchResultsSummary) current;
        } else {
            mSearchResultsFragment = (SearchResultsSummary) SwitchToFragment(
                    SearchResultsSummary.class->GetName(), NULL, FALSE, TRUE,
                    R::string::search_results_title, NULL, TRUE);
        }
        mSearchResultsFragment->SetSearchView(mSearchView);
        mSearchMenuItemExpanded = TRUE;
    }

    CARAPI NeedToRevertToInitialFragment()
    {
        mNeedToRevertToInitialFragment = TRUE;
    }

    private void RevertToInitialFragment()
    {
        mNeedToRevertToInitialFragment = FALSE;
        mSearchResultsFragment = NULL;
        mSearchMenuItemExpanded = FALSE;
        GetFragmentManager()->PopBackStackImmediate(SettingsActivity.BACK_STACK_PREFS,
                FragmentManager.POP_BACK_STACK_INCLUSIVE);
        if (mSearchMenuItem != NULL) {
            mSearchMenuItem->CollapseActionView();
        }
    }

    public CARAPI GetResultIntentData(
        /* [out] */ IIntent** intent)
    {
        return mResultIntentData;
    }

    CARAPI SetResultIntentData(
        /* [in] */ IIntent* resultIntentData)
    {
        mResultIntentData = resultIntentData;
    }

public:
    /**
     * When starting this activity, the invoking Intent can contain this extra
     * string to specify which fragment should be initially displayed.
     * <p/>Starting from Key Lime Pie, when this argument is passed in, the activity
     * will call IsValidFragment() to confirm that the fragment class name is valid for this
     * activity.
     */
    static const String EXTRA_SHOW_FRAGMENT = ":settings:show_fragment";

    /**
     * When starting this activity and using {@link #EXTRA_SHOW_FRAGMENT},
     * this extra can also be specified to supply a Bundle of arguments to pass
     * to that fragment when it is instantiated during the initial creation
     * of the activity.
     */
    static const String EXTRA_SHOW_FRAGMENT_ARGUMENTS = ":settings:show_fragment_args";

    /**
     * Fragment "key" argument passed thru {@link #EXTRA_SHOW_FRAGMENT_ARGUMENTS}
     */
    static const String EXTRA_FRAGMENT_ARG_KEY = ":settings:fragment_args_key";

    static const String BACK_STACK_PREFS = ":settings:prefs";

    /**
     * When starting this activity and using {@link #EXTRA_SHOW_FRAGMENT},
     * those extra can also be specify to supply the title or title res id to be shown for
     * that fragment.
     */
    static const String EXTRA_SHOW_FRAGMENT_TITLE = ":settings:show_fragment_title";
    static const String EXTRA_SHOW_FRAGMENT_TITLE_RESID =
            ":settings:show_fragment_title_resid";
    static const String EXTRA_SHOW_FRAGMENT_AS_SHORTCUT =
            ":settings:show_fragment_as_shortcut";

    static const String EXTRA_SHOW_FRAGMENT_AS_SUBSETTING =
            ":settings:show_fragment_as_subsetting";

private:
    // extras that allow any preference activity to be launched as part of a wizard

    // show Back and Next buttons? takes Boolean parameter
    // Back will then return RESULT_CANCELED and Next RESULT_OK
    static const String EXTRA_PREFS_SHOW_BUTTON_BAR = "extra_prefs_show_button_bar";

    // specify custom text for the Back or Next buttons, or cause a button to not appear
    // at all by setting it to NULL
    static const String EXTRA_PREFS_SET_NEXT_TEXT = "extra_prefs_set_next_text";
    static const String EXTRA_PREFS_SET_BACK_TEXT = "extra_prefs_set_back_text";

private:

    static const String LOG_TAG = "Settings";

    // Constants for state save/restore
    static const String SAVE_KEY_CATEGORIES = ":settings:categories";
    static const String SAVE_KEY_SEARCH_MENU_EXPANDED = ":settings:search_menu_expanded";
    static const String SAVE_KEY_SEARCH_QUERY = ":settings:search_query";
    static const String SAVE_KEY_SHOW_HOME_AS_UP = ":settings:show_home_as_up";
    static const String SAVE_KEY_SHOW_SEARCH = ":settings:show_search";
    static const String SAVE_KEY_HOME_ACTIVITIES_COUNT = ":settings:home_activities_count";


    // add a Skip button?
    static const String EXTRA_PREFS_SHOW_SKIP = "extra_prefs_show_skip";

    static const String META_DATA_KEY_FRAGMENT_CLASS =
        "com.android.settings.FRAGMENT_CLASS";

    static const String EXTRA_UI_OPTIONS = "settings:ui_options";

    static const String EMPTY_QUERY = "";

    static Boolean sShowNoHomeNotice = FALSE;

    String mFragmentClass;

    AutoPtr<CharSequence> mInitialTitle;
    Int32 mInitialTitleResId;

    // Show only these settings for restricted users
    Int32[26] SETTINGS_FOR_RESTRICTED = {
            R.id.wireless_section,
            R.id.wifi_settings,
            R.id.bluetooth_settings,
            R.id.data_usage_settings,
            R.id.sim_settings,
            R.id.wireless_settings,
            R.id.device_section,
            R.id.notification_settings,
            R.id.display_settings,
            R.id.storage_settings,
            R.id.application_settings,
            R.id.battery_settings,
            R.id.personal_section,
            R.id.location_settings,
            R.id.security_settings,
            R.id.language_settings,
            R.id.user_settings,
            R.id.account_settings,
            R.id.system_section,
            R.id.date_time_settings,
            R.id.about_settings,
            R.id.accessibility_settings,
            R.id.print_settings,
            R.id.nfc_payment_settings,
            R.id.home_settings,
            R.id.dashboard
    };

    static const AutoPtr<ArrayOf<String> > ENTRY_FRAGMENTS = {
            WirelessSettings.class->GetName(),
            WifiSettings.class->GetName(),
            AdvancedWifiSettings.class->GetName(),
            SavedAccessPointsWifiSettings.class->GetName(),
            BluetoothSettings.class->GetName(),
            SimSettings.class->GetName(),
            TetherSettings.class->GetName(),
            WifiP2pSettings.class->GetName(),
            VpnSettings.class->GetName(),
            DateTimeSettings.class->GetName(),
            LocalePicker.class->GetName(),
            InputMethodAndLanguageSettings.class->GetName(),
            VoiceInputSettings.class->GetName(),
            SpellCheckersSettings.class->GetName(),
            UserDictionaryList.class->GetName(),
            UserDictionarySettings.class->GetName(),
            HomeSettings.class->GetName(),
            DisplaySettings.class->GetName(),
            DeviceInfoSettings.class->GetName(),
            ManageApplications.class->GetName(),
            ProcessStatsUi.class->GetName(),
            NotificationStation.class->GetName(),
            LocationSettings.class->GetName(),
            SecuritySettings.class->GetName(),
            UsageAccessSettings.class->GetName(),
            PrivacySettings.class->GetName(),
            DeviceAdminSettings.class->GetName(),
            AccessibilitySettings.class->GetName(),
            CaptionPropertiesFragment.class->GetName(),
            com.android.settings.accessibility.ToggleDaltonizerPreferenceFragment.class->GetName(),
            TextToSpeechSettings.class->GetName(),
            Memory.class->GetName(),
            DevelopmentSettings.class->GetName(),
            UsbSettings.class->GetName(),
            AndroidBeam.class->GetName(),
            WifiDisplaySettings.class->GetName(),
            PowerUsageSummary.class->GetName(),
            AccountSyncSettings.class->GetName(),
            AccountSettings.class->GetName(),
            CryptKeeperSettings.class->GetName(),
            DataUsageSummary.class->GetName(),
            DreamSettings.class->GetName(),
            UserSettings.class->GetName(),
            NotificationAccessSettings.class->GetName(),
            ConditionProviderSettings.class->GetName(),
            PrintSettingsFragment.class->GetName(),
            PrintJobSettingsFragment.class->GetName(),
            TrustedCredentialsSettings.class->GetName(),
            PaymentSettings.class->GetName(),
            KeyboardLayoutPickerFragment.class->GetName(),
            ZenModeSettings.class->GetName(),
            NotificationSettings.class->GetName(),
            ChooseLockPassword.ChooseLockPasswordFragment.class->GetName(),
            ChooseLockPattern.ChooseLockPatternFragment.class->GetName(),
            InstalledAppDetails.class->GetName(),
            BatterySaverSettings.class->GetName(),
            NotificationAppList.class->GetName(),
            AppNotificationSettings.class->GetName(),
            OtherSoundSettings.class->GetName(),
            QuickLaunchSettings.class->GetName(),
            ApnSettings.class->GetName()
    };


    static const AutoPtr< ArrayOf<String> > LIKE_SHORTCUT_INTENT_ACTION_ARRAY = {
            "android.settings.APPLICATION_DETAILS_SETTINGS"
    };

    AutoPtr<ISharedPreferences> mDevelopmentPreferences;
    AutoPtr<ISharedPreferencesOnSharedPreferenceChangeListener> mDevelopmentPreferencesListener;

    Boolean mBatteryPresent = TRUE;
    AutoPtr<IBroadcastReceiver> mBatteryInfoReceiver;

    AutoPtr<IDynamicIndexableContentMonitor> mDynamicIndexableContentMonitor;
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
    Boolean mSearchMenuItemExpanded = FALSE;
    AutoPtr<ISearchResultsSummary> mSearchResultsFragment;
    String mSearchQuery;

    // Categories
    AutoPtr<IArrayList> mCategories;// = new ArrayList<DashboardCategory>();

    static const String MSG_DATA_FORCE_REFRESH = "msg_data_force_refresh";
    static const Int32 MSG_BUILD_CATEGORIES = 1;
    AutoPtr<BuildCategoriesHandler> mHandler;

    Boolean mNeedToRevertToInitialFragment = FALSE;
    Int32 mHomeActivitiesCount = 1;

    AutoPtr<IIntent> mResultIntentData;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_SETTINGS_ACTIVITY_H__