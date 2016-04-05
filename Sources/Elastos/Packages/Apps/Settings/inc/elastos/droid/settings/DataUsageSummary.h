

package com.android.settings;

using static android::Net::ConnectivityManager::ITYPE_ETHERNET;
using static android::Net::ConnectivityManager::ITYPE_MOBILE;
using static android::Net::ConnectivityManager::ITYPE_WIFI;
using static android::Net::ConnectivityManager::ITYPE_WIMAX;
using static android::Net::NetworkPolicy::ILIMIT_DISABLED;
using static android::Net::NetworkPolicy::IWARNING_DISABLED;
using static android::Net::NetworkPolicyManager::IEXTRA_NETWORK_TEMPLATE;
using static android::Net::NetworkPolicyManager::IPOLICY_NONE;
using static android::Net::NetworkPolicyManager::IPOLICY_REJECT_METERED_BACKGROUND;
using static android::Net::NetworkPolicyManager::IcomputeLastCycleBoundary;
using static android::Net::NetworkPolicyManager::IcomputeNextCycleBoundary;
using static android::Net::NetworkTemplate::IMATCH_MOBILE_3G_LOWER;
using static android::Net::NetworkTemplate::IMATCH_MOBILE_4G;
using static android::Net::NetworkTemplate::IMATCH_MOBILE_ALL;
using static android::Net::NetworkTemplate::IMATCH_WIFI;
using static android::Net::NetworkTemplate::IbuildTemplateEthernet;
using static android::Net::NetworkTemplate::IbuildTemplateMobile3gLower;
using static android::Net::NetworkTemplate::IbuildTemplateMobile4g;
using static android::Net::NetworkTemplate::IbuildTemplateMobileAll;
using static android::Net::NetworkTemplate::IbuildTemplateWifiWildcard;
using static android::Net::TrafficStats::IGB_IN_BYTES;
using static android::Net::TrafficStats::IMB_IN_BYTES;
using static android::Net::TrafficStats::IUID_REMOVED;
using static android::Net::TrafficStats::IUID_TETHERING;
using static android::Telephony::TelephonyManager::ISIM_STATE_READY;
using static android::Text::Format::DateUtils::IFORMAT_ABBREV_MONTH;
using static android::Text::Format::DateUtils::IFORMAT_SHOW_DATE;
using static android::View::ViewGroup::LayoutParams::IWRAP_CONTENT;
using static com::Android::Internal::Utility::Preconditions::IcheckNotNull;
using static com::Android::Settings::Utils::IprepareCustomPreferencesList;

using Elastos::Droid::Animation::ILayoutTransition;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IDialogFragment;
using Elastos::Droid::App::IFragment;
using Elastos::Droid::App::IFragmentTransaction;
using Elastos::Droid::App::LoaderManager::ILoaderCallbacks;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::IINetworkPolicyManager;
using Elastos::Droid::Net::IINetworkStatsService;
using Elastos::Droid::Net::IINetworkStatsSession;
using Elastos::Droid::Net::INetworkPolicy;
using Elastos::Droid::Net::INetworkPolicyManager;
using Elastos::Droid::Net::INetworkStats;
using Elastos::Droid::Net::INetworkStatsHistory;
using Elastos::Droid::Net::INetworkTemplate;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Os::IAsyncTask;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IINetworkManagementService;
using Elastos::Droid::Os::IParcel;
using Elastos::Droid::Os::IParcelable;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::Text::Format::ITime;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::Droid::Utility::ISparseBooleanArray;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::AdapterView::IOnItemClickListener;
using Elastos::Droid::Widget::AdapterView::IOnItemSelectedListener;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ILinearLayout;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::Widget::INumberPicker;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ISwitch;
using Elastos::Droid::Widget::ITabHost;
using Elastos::Droid::Widget::TabHost::IOnTabChangeListener;
using Elastos::Droid::Widget::TabHost::ITabContentFactory;
using Elastos::Droid::Widget::TabHost::ITabSpec;
using Elastos::Droid::Widget::ITabWidget;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Settings::Drawable::IInsetBoundsDrawable;
using Elastos::Droid::Settings::Net::IChartData;
using Elastos::Droid::Settings::Net::IChartDataLoader;
using Elastos::Droid::Settings::Net::IDataUsageMeteredSettings;
using Elastos::Droid::Settings::Net::INetworkPolicyEditor;
using Elastos::Droid::Settings::Net::ISummaryForAllUidLoader;
using Elastos::Droid::Settings::Net::IUidDetail;
using Elastos::Droid::Settings::Net::IUidDetailProvider;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;
using Elastos::Droid::Settings::Widget::IChartDataUsageView;
using Elastos::Droid::Settings::Widget::ChartDataUsageView::IDataUsageChartListener;
using com::Google::Android::Collect::ILists;

using libcore::Utility::IObjects;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;
using Elastos::Utility::ILocale;

/**
 * Panel showing data usage history across various networks, including options
 * to inspect based on usage cycle and control through {@link NetworkPolicy}.
 */
public class DataUsageSummary extends HighlightingFragment implements Indexable {
    private static const String TAG = "DataUsage";
    private static const Boolean LOGD = FALSE;

    // TODO: remove this testing code
    private static const Boolean TEST_ANIM = FALSE;
    private static const Boolean TEST_RADIOS = FALSE;

    private static const String TEST_RADIOS_PROP = "test.radios";
    private static const String TEST_SUBSCRIBER_PROP = "test.subscriberid";

    private static const String TAB_3G = "3g";
    private static const String TAB_4G = "4g";
    private static const String TAB_MOBILE = "mobile";
    private static const String TAB_WIFI = "wifi";
    private static const String TAB_ETHERNET = "ethernet";

    private static const String TAG_CONFIRM_DATA_DISABLE = "confirmDataDisable";
    private static const String TAG_CONFIRM_LIMIT = "confirmLimit";
    private static const String TAG_CYCLE_EDITOR = "cycleEditor";
    private static const String TAG_WARNING_EDITOR = "warningEditor";
    private static const String TAG_LIMIT_EDITOR = "limitEditor";
    private static const String TAG_CONFIRM_RESTRICT = "confirmRestrict";
    private static const String TAG_DENIED_RESTRICT = "deniedRestrict";
    private static const String TAG_CONFIRM_APP_RESTRICT = "confirmAppRestrict";
    private static const String TAG_APP_DETAILS = "appDetails";

    private static const String DATA_USAGE_ENABLE_MOBILE_KEY = "data_usage_enable_mobile";
    private static const String DATA_USAGE_DISABLE_MOBILE_LIMIT_KEY =
            "data_usage_disable_mobile_limit";
    private static const String DATA_USAGE_CYCLE_KEY = "data_usage_cycle";

    private static const Int32 LOADER_CHART_DATA = 2;
    private static const Int32 LOADER_SUMMARY = 3;

    private INetworkManagementService mNetworkService;
    private INetworkStatsService mStatsService;
    private NetworkPolicyManager mPolicyManager;
    private TelephonyManager mTelephonyManager;

    private INetworkStatsSession mStatsSession;

    private static const String PREF_FILE = "data_usage";
    private static const String PREF_SHOW_WIFI = "show_wifi";
    private static const String PREF_SHOW_ETHERNET = "show_ethernet";

    private SharedPreferences mPrefs;

    private TabHost mTabHost;
    private ViewGroup mTabsContainer;
    private TabWidget mTabWidget;
    private ListView mListView;
    private DataUsageAdapter mAdapter;

    /** Distance to inset content from sides, when needed. */
    private Int32 mInsetSide = 0;

    private ViewGroup mHeader;

    private ViewGroup mNetworkSwitchesContainer;
    private LinearLayout mNetworkSwitches;
    private Boolean mDataEnabledSupported;
    private Switch mDataEnabled;
    private View mDataEnabledView;
    private Boolean mDisableAtLimitSupported;
    private Switch mDisableAtLimit;
    private View mDisableAtLimitView;

    private View mCycleView;
    private Spinner mCycleSpinner;
    private CycleAdapter mCycleAdapter;
    private TextView mCycleSummary;

    private ChartDataUsageView mChart;
    private View mDisclaimer;
    private TextView mEmpty;
    private View mStupidPadding;

    private View mAppDetail;
    private ImageView mAppIcon;
    private ViewGroup mAppTitles;
    private TextView mAppTotal;
    private TextView mAppForeground;
    private TextView mAppBackground;
    private Button mAppSettings;

    private LinearLayout mAppSwitches;
    private Switch mAppRestrict;
    private View mAppRestrictView;

    private Boolean mShowWifi = FALSE;
    private Boolean mShowEthernet = FALSE;

    private NetworkTemplate mTemplate;
    private ChartData mChartData;

    private AppItem mCurrentApp = NULL;

    private Intent mAppSettingsIntent;

    private NetworkPolicyEditor mPolicyEditor;

    private String mCurrentTab = NULL;
    private String mIntentTab = NULL;

    private MenuItem mMenuRestrictBackground;
    private MenuItem mMenuShowWifi;
    private MenuItem mMenuShowEthernet;
    private MenuItem mMenuSimCards;
    private MenuItem mMenuCellularNetworks;

    /** Flag used to ignore listeners during binding. */
    private Boolean mBinding;

    private UidDetailProvider mUidDetailProvider;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        final Context context = GetActivity();

        mNetworkService = INetworkManagementService.Stub->AsInterface(
                ServiceManager->GetService(Context.NETWORKMANAGEMENT_SERVICE));
        mStatsService = INetworkStatsService.Stub->AsInterface(
                ServiceManager->GetService(Context.NETWORK_STATS_SERVICE));
        mPolicyManager = NetworkPolicyManager->From(context);
        mTelephonyManager = TelephonyManager->From(context);

        mPrefs = GetActivity()->GetSharedPreferences(PREF_FILE, Context.MODE_PRIVATE);

        mPolicyEditor = new NetworkPolicyEditor(mPolicyManager);
        mPolicyEditor->Read();

        try {
            if (!mNetworkService->IsBandwidthControlEnabled()) {
                Logger::W(TAG, "No bandwidth control; leaving");
                GetActivity()->Finish();
            }
        } catch (RemoteException e) {
            Logger::W(TAG, "No bandwidth control; leaving");
            GetActivity()->Finish();
        }

        try {
            mStatsSession = mStatsService->OpenSession();
        } catch (RemoteException e) {
            throw new RuntimeException(e);
        }

        mShowWifi = mPrefs->GetBoolean(PREF_SHOW_WIFI, FALSE);
        mShowEthernet = mPrefs->GetBoolean(PREF_SHOW_ETHERNET, FALSE);

        // override preferences when no mobile radio
        if (!HasReadyMobileRadio(context)) {
            mShowWifi = TRUE;
            mShowEthernet = TRUE;
        }

        SetHasOptionsMenu(TRUE);
    }

    //@Override
    public View OnCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {

        final Context context = inflater->GetContext();
        final View view = inflater->Inflate(R.layout.data_usage_summary, container, FALSE);

        mUidDetailProvider = new UidDetailProvider(context);

        mTabHost = (TabHost) view->FindViewById(android.R.id.tabhost);
        mTabsContainer = (ViewGroup) view->FindViewById(R.id.tabs_container);
        mTabWidget = (TabWidget) view->FindViewById(android.R.id.tabs);
        mListView = (ListView) view->FindViewById(android.R.id.list);

        // decide if we need to manually inset our content, or if we should rely
        // on parent container for inset.
        final Boolean shouldInset = mListView->GetScrollBarStyle()
                == View.SCROLLBARS_OUTSIDE_OVERLAY;
        mInsetSide = 0;

        // adjust padding around tabwidget as needed
        PrepareCustomPreferencesList(container, view, mListView, FALSE);

        mTabHost->Setup();
        mTabHost->SetOnTabChangedListener(mTabListener);

        mHeader = (ViewGroup) inflater->Inflate(R.layout.data_usage_header, mListView, FALSE);
        mHeader->SetClickable(TRUE);

        mListView->AddHeaderView(new View(context), NULL, TRUE);
        mListView->AddHeaderView(mHeader, NULL, TRUE);
        mListView->SetItemsCanFocus(TRUE);

        if (mInsetSide > 0) {
            // inset selector and divider drawables
            InsetListViewDrawables(mListView, mInsetSide);
            mHeader->SetPaddingRelative(mInsetSide, 0, mInsetSide, 0);
        }

        {
            // bind network switches
            mNetworkSwitchesContainer = (ViewGroup) mHeader->FindViewById(
                    R.id.network_switches_container);
            mNetworkSwitches = (LinearLayout) mHeader->FindViewById(R.id.network_switches);

            mDataEnabled = new Switch(inflater->GetContext());
            mDataEnabled->SetClickable(FALSE);
            mDataEnabled->SetFocusable(FALSE);
            mDataEnabledView = InflatePreference(inflater, mNetworkSwitches, mDataEnabled);
            mDataEnabledView->SetTag(R.id.preference_highlight_key,
                    DATA_USAGE_ENABLE_MOBILE_KEY);
            mDataEnabledView->SetClickable(TRUE);
            mDataEnabledView->SetFocusable(TRUE);
            mDataEnabledView->SetOnClickListener(mDataEnabledListener);
            mNetworkSwitches->AddView(mDataEnabledView);

            mDisableAtLimit = new Switch(inflater->GetContext());
            mDisableAtLimit->SetClickable(FALSE);
            mDisableAtLimit->SetFocusable(FALSE);
            mDisableAtLimitView = InflatePreference(inflater, mNetworkSwitches, mDisableAtLimit);
            mDisableAtLimitView->SetTag(R.id.preference_highlight_key,
                    DATA_USAGE_DISABLE_MOBILE_LIMIT_KEY);
            mDisableAtLimitView->SetClickable(TRUE);
            mDisableAtLimitView->SetFocusable(TRUE);
            mDisableAtLimitView->SetOnClickListener(mDisableAtLimitListener);
            mNetworkSwitches->AddView(mDisableAtLimitView);

            mCycleView = inflater->Inflate(R.layout.data_usage_cycles, mNetworkSwitches, FALSE);
            mCycleView->SetTag(R.id.preference_highlight_key, DATA_USAGE_CYCLE_KEY);
            mCycleSpinner = (Spinner) mCycleView->FindViewById(R.id.cycles_spinner);
            mCycleAdapter = new CycleAdapter(context);
            mCycleSpinner->SetAdapter(mCycleAdapter);
            mCycleSpinner->SetOnItemSelectedListener(mCycleListener);
            mCycleSummary = (TextView) mCycleView->FindViewById(R.id.cycle_summary);
            mNetworkSwitches->AddView(mCycleView);
        }

        mChart = (ChartDataUsageView) mHeader->FindViewById(R.id.chart);
        mChart->SetListener(mChartListener);
        mChart->BindNetworkPolicy(NULL);

        {
            // bind app detail controls
            mAppDetail = mHeader->FindViewById(R.id.app_detail);
            mAppIcon = (ImageView) mAppDetail->FindViewById(R.id.app_icon);
            mAppTitles = (ViewGroup) mAppDetail->FindViewById(R.id.app_titles);
            mAppForeground = (TextView) mAppDetail->FindViewById(R.id.app_foreground);
            mAppBackground = (TextView) mAppDetail->FindViewById(R.id.app_background);
            mAppSwitches = (LinearLayout) mAppDetail->FindViewById(R.id.app_switches);

            mAppSettings = (Button) mAppDetail->FindViewById(R.id.app_settings);

            mAppRestrict = new Switch(inflater->GetContext());
            mAppRestrict->SetClickable(FALSE);
            mAppRestrict->SetFocusable(FALSE);
            mAppRestrictView = InflatePreference(inflater, mAppSwitches, mAppRestrict);
            mAppRestrictView->SetClickable(TRUE);
            mAppRestrictView->SetFocusable(TRUE);
            mAppRestrictView->SetOnClickListener(mAppRestrictListener);
            mAppSwitches->AddView(mAppRestrictView);
        }

        mDisclaimer = mHeader->FindViewById(R.id.disclaimer);
        mEmpty = (TextView) mHeader->FindViewById(android.R.id.empty);
        mStupidPadding = mHeader->FindViewById(R.id.stupid_padding);

        final UserManager um = (UserManager) context->GetSystemService(Context.USER_SERVICE);
        mAdapter = new DataUsageAdapter(um, mUidDetailProvider, mInsetSide);
        mListView->SetOnItemClickListener(mListListener);
        mListView->SetAdapter(mAdapter);

        return view;
    }

    //@Override
    CARAPI OnViewStateRestored(Bundle savedInstanceState) {
        super->OnViewStateRestored(savedInstanceState);

        // pick default tab based on incoming intent
        final Intent intent = GetActivity()->GetIntent();
        mIntentTab = ComputeTabFromIntent(intent);

        // this kicks off chain reaction which creates tabs, binds the body to
        // selected network, and binds chart, cycles and detail list.
        UpdateTabs();
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();

        GetView()->Post(new Runnable() {
            //@Override
            CARAPI Run() {
                HighlightViewIfNeeded();
            }
        });

        // kick off background task to update stats
        new AsyncTask<Void, Void, Void>() {
            //@Override
            protected Void DoInBackground(Void... params) {
                try {
                    // wait a few seconds before kicking off
                    Thread->Sleep(2 * DateUtils.SECOND_IN_MILLIS);
                    mStatsService->ForceUpdate();
                } catch (InterruptedException e) {
                } catch (RemoteException e) {
                }
                return NULL;
            }

            //@Override
            protected void OnPostExecute(Void result) {
                if (IsAdded()) {
                    UpdateBody();
                }
            }
        }.ExecuteOnExecutor(AsyncTask.THREAD_POOL_EXECUTOR);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        inflater->Inflate(R.menu.data_usage, menu);
    }

    //@Override
    CARAPI OnPrepareOptionsMenu(Menu menu) {
        final Context context = GetActivity();
        final Boolean appDetailMode = IsAppDetailMode();
        final Boolean isOwner = ActivityManager->GetCurrentUser() == UserHandle.USER_OWNER;

        mMenuShowWifi = menu->FindItem(R.id.data_usage_menu_show_wifi);
        if (HasWifiRadio(context) && HasReadyMobileRadio(context)) {
            mMenuShowWifi->SetVisible(!appDetailMode);
        } else {
            mMenuShowWifi->SetVisible(FALSE);
        }

        mMenuShowEthernet = menu->FindItem(R.id.data_usage_menu_show_ethernet);
        if (HasEthernet(context) && HasReadyMobileRadio(context)) {
            mMenuShowEthernet->SetVisible(!appDetailMode);
        } else {
            mMenuShowEthernet->SetVisible(FALSE);
        }

        mMenuRestrictBackground = menu->FindItem(R.id.data_usage_menu_restrict_background);
        mMenuRestrictBackground->SetVisible(
                HasReadyMobileRadio(context) && isOwner && !appDetailMode);

        final MenuItem metered = menu->FindItem(R.id.data_usage_menu_metered);
        if (HasReadyMobileRadio(context) || HasWifiRadio(context)) {
            metered->SetVisible(!appDetailMode);
        } else {
            metered->SetVisible(FALSE);
        }

        // TODO: show when multiple sims available
        mMenuSimCards = menu->FindItem(R.id.data_usage_menu_sim_cards);
        mMenuSimCards->SetVisible(FALSE);

        mMenuCellularNetworks = menu->FindItem(R.id.data_usage_menu_cellular_networks);
        mMenuCellularNetworks->SetVisible(HasReadyMobileRadio(context)
                && !appDetailMode && isOwner);

        final MenuItem help = menu->FindItem(R.id.data_usage_menu_help);
        String helpUrl;
        if (!TextUtils->IsEmpty(helpUrl = GetResources()->GetString(R::string::help_url_data_usage))) {
            HelpUtils->PrepareHelpMenuItem(context, help, helpUrl);
        } else {
            help->SetVisible(FALSE);
        }

        UpdateMenuTitles();
    }

    private void UpdateMenuTitles() {
        if (mPolicyManager->GetRestrictBackground()) {
            mMenuRestrictBackground->SetTitle(R::string::data_usage_menu_allow_background);
        } else {
            mMenuRestrictBackground->SetTitle(R::string::data_usage_menu_restrict_background);
        }

        if (mShowWifi) {
            mMenuShowWifi->SetTitle(R::string::data_usage_menu_hide_wifi);
        } else {
            mMenuShowWifi->SetTitle(R::string::data_usage_menu_show_wifi);
        }

        if (mShowEthernet) {
            mMenuShowEthernet->SetTitle(R::string::data_usage_menu_hide_ethernet);
        } else {
            mMenuShowEthernet->SetTitle(R::string::data_usage_menu_show_ethernet);
        }
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
            case R.id.data_usage_menu_restrict_background: {
                final Boolean restrictBackground = !mPolicyManager->GetRestrictBackground();
                if (restrictBackground) {
                    ConfirmRestrictFragment->Show(this);
                } else {
                    // no confirmation to drop restriction
                    SetRestrictBackground(FALSE);
                }
                return TRUE;
            }
            case R.id.data_usage_menu_show_wifi: {
                mShowWifi = !mShowWifi;
                mPrefs->Edit()->PutBoolean(PREF_SHOW_WIFI, mShowWifi).Apply();
                UpdateMenuTitles();
                UpdateTabs();
                return TRUE;
            }
            case R.id.data_usage_menu_show_ethernet: {
                mShowEthernet = !mShowEthernet;
                mPrefs->Edit()->PutBoolean(PREF_SHOW_ETHERNET, mShowEthernet).Apply();
                UpdateMenuTitles();
                UpdateTabs();
                return TRUE;
            }
            case R.id.data_usage_menu_sim_cards: {
                // TODO: hook up to sim cards
                return TRUE;
            }
            case R.id.data_usage_menu_cellular_networks: {
                final Intent intent = new Intent(IIntent::ACTION_MAIN);
                intent->SetComponent(new ComponentName("com.android.phone",
                        "com.android.phone.MobileNetworkSettings"));
                StartActivity(intent);
                return TRUE;
            }
            case R.id.data_usage_menu_metered: {
                final SettingsActivity sa = (SettingsActivity) GetActivity();
                sa->StartPreferencePanel(DataUsageMeteredSettings.class->GetCanonicalName(), NULL,
                        R::string::data_usage_metered_title, NULL, this, 0);
                return TRUE;
            }
        }
        return FALSE;
    }

    //@Override
    CARAPI OnDestroy() {
        mDataEnabledView = NULL;
        mDisableAtLimitView = NULL;

        mUidDetailProvider->ClearCache();
        mUidDetailProvider = NULL;

        TrafficStats->CloseQuietly(mStatsSession);

        super->OnDestroy();
    }

    /**
     * Build and assign {@link LayoutTransition} to various containers. Should
     * only be assigned after initial layout is complete.
     */
    private void EnsureLayoutTransitions() {
        // skip when already setup
        if (mChart->GetLayoutTransition() != NULL) return;

        mTabsContainer->SetLayoutTransition(BuildLayoutTransition());
        mHeader->SetLayoutTransition(BuildLayoutTransition());
        mNetworkSwitchesContainer->SetLayoutTransition(BuildLayoutTransition());

        final LayoutTransition chartTransition = BuildLayoutTransition();
        chartTransition->DisableTransitionType(LayoutTransition.APPEARING);
        chartTransition->DisableTransitionType(LayoutTransition.DISAPPEARING);
        mChart->SetLayoutTransition(chartTransition);
    }

    private static LayoutTransition BuildLayoutTransition() {
        final LayoutTransition transition = new LayoutTransition();
        if (TEST_ANIM) {
            transition->SetDuration(1500);
        }
        transition->SetAnimateParentHierarchy(FALSE);
        return transition;
    }

    /**
     * Rebuild all tabs based on {@link NetworkPolicyEditor} and
     * {@link #mShowWifi}, hiding the tabs entirely when applicable. Selects
     * first tab, and kicks off a full rebind of body contents.
     */
    private void UpdateTabs() {
        final Context context = GetActivity();
        mTabHost->ClearAllTabs();

        final Boolean mobileSplit = IsMobilePolicySplit();
        if (mobileSplit && HasReadyMobile4gRadio(context)) {
            mTabHost->AddTab(BuildTabSpec(TAB_3G, R::string::data_usage_tab_3g));
            mTabHost->AddTab(BuildTabSpec(TAB_4G, R::string::data_usage_tab_4g));
        } else if (HasReadyMobileRadio(context)) {
            mTabHost->AddTab(BuildTabSpec(TAB_MOBILE, R::string::data_usage_tab_mobile));
        }
        if (mShowWifi && HasWifiRadio(context)) {
            mTabHost->AddTab(BuildTabSpec(TAB_WIFI, R::string::data_usage_tab_wifi));
        }
        if (mShowEthernet && HasEthernet(context)) {
            mTabHost->AddTab(BuildTabSpec(TAB_ETHERNET, R::string::data_usage_tab_ethernet));
        }

        final Boolean noTabs = mTabWidget->GetTabCount() == 0;
        final Boolean multipleTabs = mTabWidget->GetTabCount() > 1;
        mTabWidget->SetVisibility(multipleTabs ? View.VISIBLE : View.GONE);
        if (mIntentTab != NULL) {
            if (Objects->Equal(mIntentTab, mTabHost->GetCurrentTabTag())) {
                // already hit UpdateBody() when added; ignore
                UpdateBody();
            } else {
                mTabHost->SetCurrentTabByTag(mIntentTab);
            }
            mIntentTab = NULL;
        } else if (noTabs) {
            // no usable tabs, so hide body
            UpdateBody();
        } else {
            // already hit UpdateBody() when added; ignore
        }
    }

    /**
     * Factory that provide empty {@link View} to make {@link TabHost} happy.
     */
    private TabContentFactory mEmptyTabContent = new TabContentFactory() {
        //@Override
        public View CreateTabContent(String tag) {
            return new View(mTabHost->GetContext());
        }
    };

    /**
     * Build {@link TabSpec} with thin indicator, and empty content.
     */
    private TabSpec BuildTabSpec(String tag, Int32 titleRes) {
        return mTabHost->NewTabSpec(tag).SetIndicator(GetText(titleRes)).SetContent(
                mEmptyTabContent);
    }

    private OnTabChangeListener mTabListener = new OnTabChangeListener() {
        //@Override
        CARAPI OnTabChanged(String tabId) {
            // user changed tab; update body
            UpdateBody();
        }
    };

    /**
     * Update body content based on current tab. Loads
     * {@link NetworkStatsHistory} and {@link NetworkPolicy} from system, and
     * binds them to visible controls.
     */
    private void UpdateBody() {
        mBinding = TRUE;
        if (!IsAdded()) return;

        final Context context = GetActivity();
        final String currentTab = mTabHost->GetCurrentTabTag();
        final Boolean isOwner = ActivityManager->GetCurrentUser() == UserHandle.USER_OWNER;

        if (currentTab == NULL) {
            Logger::W(TAG, "no tab selected; hiding body");
            mListView->SetVisibility(View.GONE);
            return;
        } else {
            mListView->SetVisibility(View.VISIBLE);
        }

        mCurrentTab = currentTab;

        if (LOGD) Logger::D(TAG, "UpdateBody() with currentTab=" + currentTab);

        mDataEnabledSupported = isOwner;
        mDisableAtLimitSupported = TRUE;

        // TODO: remove mobile tabs when SIM isn't ready

        if (TAB_MOBILE->Equals(currentTab)) {
            SetPreferenceTitle(mDataEnabledView, R::string::data_usage_enable_mobile);
            SetPreferenceTitle(mDisableAtLimitView, R::string::data_usage_disable_mobile_limit);
            mTemplate = BuildTemplateMobileAll(GetActiveSubscriberId(context));

        } else if (TAB_3G->Equals(currentTab)) {
            SetPreferenceTitle(mDataEnabledView, R::string::data_usage_enable_3g);
            SetPreferenceTitle(mDisableAtLimitView, R::string::data_usage_disable_3g_limit);
            // TODO: bind mDataEnabled to 3G radio state
            mTemplate = BuildTemplateMobile3gLower(GetActiveSubscriberId(context));

        } else if (TAB_4G->Equals(currentTab)) {
            SetPreferenceTitle(mDataEnabledView, R::string::data_usage_enable_4g);
            SetPreferenceTitle(mDisableAtLimitView, R::string::data_usage_disable_4g_limit);
            // TODO: bind mDataEnabled to 4G radio state
            mTemplate = BuildTemplateMobile4g(GetActiveSubscriberId(context));

        } else if (TAB_WIFI->Equals(currentTab)) {
            // wifi doesn't have any controls
            mDataEnabledSupported = FALSE;
            mDisableAtLimitSupported = FALSE;
            mTemplate = BuildTemplateWifiWildcard();

        } else if (TAB_ETHERNET->Equals(currentTab)) {
            // ethernet doesn't have any controls
            mDataEnabledSupported = FALSE;
            mDisableAtLimitSupported = FALSE;
            mTemplate = BuildTemplateEthernet();

        } else {
            throw new IllegalStateException("unknown tab: " + currentTab);
        }

        // kick off loader for network history
        // TODO: consider chaining two loaders together instead of reloading
        // network history when showing app detail.
        GetLoaderManager()->RestartLoader(LOADER_CHART_DATA,
                ChartDataLoader->BuildArgs(mTemplate, mCurrentApp), mChartDataCallbacks);

        // detail mode can change visible menus, invalidate
        GetActivity()->InvalidateOptionsMenu();

        mBinding = FALSE;
    }

    private Boolean IsAppDetailMode() {
        return mCurrentApp != NULL;
    }

    /**
     * Update UID details panels to match {@link #mCurrentApp}, showing or
     * hiding them depending on {@link #IsAppDetailMode()}.
     */
    private void UpdateAppDetail() {
        final Context context = GetActivity();
        final PackageManager pm = context->GetPackageManager();
        final LayoutInflater inflater = GetActivity()->GetLayoutInflater();

        if (IsAppDetailMode()) {
            mAppDetail->SetVisibility(View.VISIBLE);
            mCycleAdapter->SetChangeVisible(FALSE);
        } else {
            mAppDetail->SetVisibility(View.GONE);
            mCycleAdapter->SetChangeVisible(TRUE);

            // hide detail stats when not in detail mode
            mChart->BindDetailNetworkStats(NULL);
            return;
        }

        // remove warning/limit sweeps while in detail mode
        mChart->BindNetworkPolicy(NULL);

        // show icon and all labels appearing under this app
        final Int32 uid = mCurrentApp.key;
        final UidDetail detail = mUidDetailProvider->GetUidDetail(uid, TRUE);
        mAppIcon->SetImageDrawable(detail.icon);

        mAppTitles->RemoveAllViews();

        View title = NULL;
        if (detail.detailLabels != NULL) {
            final Int32 n = detail.detailLabels.length;
            for (Int32 i = 0; i < n; ++i) {
                CharSequence label = detail.detailLabels[i];
                CharSequence contentDescription = detail.detailContentDescriptions[i];
                title = inflater->Inflate(R.layout.data_usage_app_title, mAppTitles, FALSE);
                TextView appTitle = (TextView) title->FindViewById(R.id.app_title);
                appTitle->SetText(label);
                appTitle->SetContentDescription(contentDescription);
                mAppTitles->AddView(title);
            }
        } else {
            title = inflater->Inflate(R.layout.data_usage_app_title, mAppTitles, FALSE);
            TextView appTitle = (TextView) title->FindViewById(R.id.app_title);
            appTitle->SetText(detail.label);
            appTitle->SetContentDescription(detail.contentDescription);
            mAppTitles->AddView(title);
        }

        // Remember last slot for summary
        if (title != NULL) {
            mAppTotal = (TextView) title->FindViewById(R.id.app_summary);
        } else {
            mAppTotal = NULL;
        }

        // enable settings button when package provides it
        final String[] packageNames = pm->GetPackagesForUid(uid);
        if (packageNames != NULL && packageNames.length > 0) {
            mAppSettingsIntent = new Intent(IIntent::ACTION_MANAGE_NETWORK_USAGE);
            mAppSettingsIntent->AddCategory(Intent.CATEGORY_DEFAULT);

            // Search for match across all packages
            Boolean matchFound = FALSE;
            for (String packageName : packageNames) {
                mAppSettingsIntent->SetPackage(packageName);
                if (pm->ResolveActivity(mAppSettingsIntent, 0) != NULL) {
                    matchFound = TRUE;
                    break;
                }
            }

            mAppSettings->SetOnClickListener(new OnClickListener() {
                //@Override
                CARAPI OnClick(View v) {
                    if (!IsAdded()) {
                        return;
                    }

                    // TODO: target towards entire UID instead of just first package
                    GetActivity()->StartActivityAsUser(mAppSettingsIntent,
                            new UserHandle(UserHandle->GetUserId(uid)));
                }
            });
            mAppSettings->SetEnabled(matchFound);
            mAppSettings->SetVisibility(View.VISIBLE);

        } else {
            mAppSettingsIntent = NULL;
            mAppSettings->SetOnClickListener(NULL);
            mAppSettings->SetVisibility(View.GONE);
        }

        UpdateDetailData();

        if (UserHandle->IsApp(uid) && !mPolicyManager->GetRestrictBackground()
                && IsBandwidthControlEnabled() && HasReadyMobileRadio(context)) {
            SetPreferenceTitle(mAppRestrictView, R::string::data_usage_app_restrict_background);
            SetPreferenceSummary(mAppRestrictView,
                    GetString(R::string::data_usage_app_restrict_background_summary));

            mAppRestrictView->SetVisibility(View.VISIBLE);
            mAppRestrict->SetChecked(GetAppRestrictBackground());

        } else {
            mAppRestrictView->SetVisibility(View.GONE);
        }
    }

    private void SetPolicyWarningBytes(Int64 warningBytes) {
        if (LOGD) Logger::D(TAG, "SetPolicyWarningBytes()");
        mPolicyEditor->SetPolicyWarningBytes(mTemplate, warningBytes);
        UpdatePolicy(FALSE);
    }

    private void SetPolicyLimitBytes(Int64 limitBytes) {
        if (LOGD) Logger::D(TAG, "SetPolicyLimitBytes()");
        mPolicyEditor->SetPolicyLimitBytes(mTemplate, limitBytes);
        UpdatePolicy(FALSE);
    }

    /**
     * Local cache of value, used to work around delay when
     * {@link ConnectivityManager#SetMobileDataEnabled(Boolean)} is async.
     */
    private Boolean mMobileDataEnabled;

    private Boolean IsMobileDataEnabled() {
        if (mMobileDataEnabled != NULL) {
            // TODO: deprecate and remove this once enabled flag is on policy
            return mMobileDataEnabled;
        } else {
            return mTelephonyManager->GetDataEnabled();
        }
    }

    private void SetMobileDataEnabled(Boolean enabled) {
        if (LOGD) Logger::D(TAG, "SetMobileDataEnabled()");
        mTelephonyManager->SetDataEnabled(enabled);
        mMobileDataEnabled = enabled;
        UpdatePolicy(FALSE);
    }

    private Boolean IsNetworkPolicyModifiable(NetworkPolicy policy) {
        return policy != NULL && IsBandwidthControlEnabled() && mDataEnabled->IsChecked()
                && ActivityManager->GetCurrentUser() == UserHandle.USER_OWNER;
    }

    private Boolean IsBandwidthControlEnabled() {
        try {
            return mNetworkService->IsBandwidthControlEnabled();
        } catch (RemoteException e) {
            Logger::W(TAG, "problem talking with INetworkManagementService: " + e);
            return FALSE;
        }
    }

    CARAPI SetRestrictBackground(Boolean restrictBackground) {
        mPolicyManager->SetRestrictBackground(restrictBackground);
        UpdateMenuTitles();
    }

    private Boolean GetAppRestrictBackground() {
        final Int32 uid = mCurrentApp.key;
        final Int32 uidPolicy = mPolicyManager->GetUidPolicy(uid);
        return (uidPolicy & POLICY_REJECT_METERED_BACKGROUND) != 0;
    }

    private void SetAppRestrictBackground(Boolean restrictBackground) {
        if (LOGD) Logger::D(TAG, "SetAppRestrictBackground()");
        final Int32 uid = mCurrentApp.key;
        mPolicyManager->SetUidPolicy(
                uid, restrictBackground ? POLICY_REJECT_METERED_BACKGROUND : POLICY_NONE);
        mAppRestrict->SetChecked(restrictBackground);
    }

    /**
     * Update chart sweeps and cycle list to reflect {@link NetworkPolicy} for
     * current {@link #mTemplate}.
     */
    private void UpdatePolicy(Boolean refreshCycle) {
        Boolean dataEnabledVisible = mDataEnabledSupported;
        Boolean disableAtLimitVisible = mDisableAtLimitSupported;

        if (IsAppDetailMode()) {
            dataEnabledVisible = FALSE;
            disableAtLimitVisible = FALSE;
        }

        // TODO: move enabled state directly into policy
        if (TAB_MOBILE->Equals(mCurrentTab)) {
            mBinding = TRUE;
            mDataEnabled->SetChecked(IsMobileDataEnabled());
            mBinding = FALSE;
        }

        final NetworkPolicy policy = mPolicyEditor->GetPolicy(mTemplate);
        if (IsNetworkPolicyModifiable(policy)) {
            mDisableAtLimit->SetChecked(policy != NULL && policy.limitBytes != LIMIT_DISABLED);
            if (!IsAppDetailMode()) {
                mChart->BindNetworkPolicy(policy);
            }

        } else {
            // controls are disabled; don't bind warning/limit sweeps
            disableAtLimitVisible = FALSE;
            mChart->BindNetworkPolicy(NULL);
        }

        mDataEnabledView->SetVisibility(dataEnabledVisible ? View.VISIBLE : View.GONE);
        mDisableAtLimitView->SetVisibility(disableAtLimitVisible ? View.VISIBLE : View.GONE);

        if (refreshCycle) {
            // generate cycle list based on policy and available history
            UpdateCycleList(policy);
        }
    }

    /**
     * Rebuild {@link #mCycleAdapter} based on {@link NetworkPolicy#cycleDay}
     * and available {@link NetworkStatsHistory} data. Always selects the newest
     * item, updating the inspection range on {@link #mChart}.
     */
    private void UpdateCycleList(NetworkPolicy policy) {
        // stash away currently selected cycle to try restoring below
        final CycleItem previousItem = (CycleItem) mCycleSpinner->GetSelectedItem();
        mCycleAdapter->Clear();

        final Context context = mCycleSpinner->GetContext();

        Int64 historyStart = Long.MAX_VALUE;
        Int64 historyEnd = Long.MIN_VALUE;
        if (mChartData != NULL) {
            historyStart = mChartData.network->GetStart();
            historyEnd = mChartData.network->GetEnd();
        }

        final Int64 now = System->CurrentTimeMillis();
        if (historyStart == Long.MAX_VALUE) historyStart = now;
        if (historyEnd == Long.MIN_VALUE) historyEnd = now + 1;

        Boolean hasCycles = FALSE;
        if (policy != NULL) {
            // find the next cycle boundary
            Int64 cycleEnd = ComputeNextCycleBoundary(historyEnd, policy);

            // walk backwards, generating all valid cycle ranges
            while (cycleEnd > historyStart) {
                final Int64 cycleStart = ComputeLastCycleBoundary(cycleEnd, policy);
                Logger::D(TAG, "generating cs=" + cycleStart + " to ce=" + cycleEnd + " waiting for hs="
                        + historyStart);
                mCycleAdapter->Add(new CycleItem(context, cycleStart, cycleEnd));
                cycleEnd = cycleStart;
                hasCycles = TRUE;
            }

            // one last cycle entry to modify policy cycle day
            mCycleAdapter->SetChangePossible(IsNetworkPolicyModifiable(policy));
        }

        if (!hasCycles) {
            // no policy defined cycles; show entry for each four-week period
            Int64 cycleEnd = historyEnd;
            while (cycleEnd > historyStart) {
                final Int64 cycleStart = cycleEnd - (DateUtils.WEEK_IN_MILLIS * 4);
                mCycleAdapter->Add(new CycleItem(context, cycleStart, cycleEnd));
                cycleEnd = cycleStart;
            }

            mCycleAdapter->SetChangePossible(FALSE);
        }

        // force pick the current cycle (first item)
        if (mCycleAdapter->GetCount() > 0) {
            final Int32 position = mCycleAdapter->FindNearestPosition(previousItem);
            mCycleSpinner->SetSelection(position);

            // only force-update cycle when changed; skipping preserves any
            // user-defined inspection region.
            final CycleItem selectedItem = mCycleAdapter->GetItem(position);
            if (!Objects->Equal(selectedItem, previousItem)) {
                mCycleListener->OnItemSelected(mCycleSpinner, NULL, position, 0);
            } else {
                // but still kick off loader for detailed list
                UpdateDetailData();
            }
        } else {
            UpdateDetailData();
        }
    }

    private View.OnClickListener mDataEnabledListener = new View->OnClickListener() {
        //@Override
        CARAPI OnClick(View v) {
            if (mBinding) return;

            final Boolean dataEnabled = !mDataEnabled->IsChecked();
            final String currentTab = mCurrentTab;
            if (TAB_MOBILE->Equals(currentTab)) {
                if (dataEnabled) {
                    SetMobileDataEnabled(TRUE);
                } else {
                    // disabling data; show confirmation dialog which eventually
                    // calls SetMobileDataEnabled() once user confirms.
                    ConfirmDataDisableFragment->Show(DataUsageSummary.this);
                }
            }

            UpdatePolicy(FALSE);
        }
    };

    private View.OnClickListener mDisableAtLimitListener = new View->OnClickListener() {
        //@Override
        CARAPI OnClick(View v) {
            final Boolean disableAtLimit = !mDisableAtLimit->IsChecked();
            if (disableAtLimit) {
                // enabling limit; show confirmation dialog which eventually
                // calls SetPolicyLimitBytes() once user confirms.
                ConfirmLimitFragment->Show(DataUsageSummary.this);
            } else {
                SetPolicyLimitBytes(LIMIT_DISABLED);
            }
        }
    };

    private View.OnClickListener mAppRestrictListener = new View->OnClickListener() {
        //@Override
        CARAPI OnClick(View v) {
            final Boolean restrictBackground = !mAppRestrict->IsChecked();

            if (restrictBackground) {
                // enabling restriction; show confirmation dialog which
                // eventually calls SetRestrictBackground() once user
                // confirms.
                ConfirmAppRestrictFragment->Show(DataUsageSummary.this);
            } else {
                SetAppRestrictBackground(FALSE);
            }
        }
    };

    private OnItemClickListener mListListener = new OnItemClickListener() {
        //@Override
        CARAPI OnItemClick(AdapterView<?> parent, View view, Int32 position, Int64 id) {
            final Context context = view->GetContext();
            final AppItem app = (AppItem) parent->GetItemAtPosition(position);

            // TODO: sigh, remove this hack once we understand 6450986
            if (mUidDetailProvider == NULL || app == NULL) return;

            final UidDetail detail = mUidDetailProvider->GetUidDetail(app.key, TRUE);
            AppDetailsFragment->Show(DataUsageSummary.this, app, detail.label);
        }
    };

    private OnItemSelectedListener mCycleListener = new OnItemSelectedListener() {
        //@Override
        CARAPI OnItemSelected(AdapterView<?> parent, View view, Int32 position, Int64 id) {
            final CycleItem cycle = (CycleItem) parent->GetItemAtPosition(position);
            if (cycle instanceof CycleChangeItem) {
                // show cycle editor; will eventually call SetPolicyCycleDay()
                // when user finishes editing.
                CycleEditorFragment->Show(DataUsageSummary.this);

                // reset spinner to something other than "change cycle..."
                mCycleSpinner->SetSelection(0);

            } else {
                if (LOGD) {
                    Logger::D(TAG, "showing cycle " + cycle + ", start=" + cycle.start + ", end="
                            + cycle.end + "]");
                }

                // update chart to show selected cycle, and update detail data
                // to match updated sweep bounds.
                mChart->SetVisibleRange(cycle.start, cycle.end);

                UpdateDetailData();
            }
        }

        //@Override
        CARAPI OnNothingSelected(AdapterView<?> parent) {
            // ignored
        }
    };

    /**
     * Update details based on {@link #mChart} inspection range depending on
     * current mode. In network mode, updates {@link #mAdapter} with sorted list
     * of applications data usage, and when {@link #IsAppDetailMode()} update
     * app details.
     */
    private void UpdateDetailData() {
        if (LOGD) Logger::D(TAG, "UpdateDetailData()");

        final Int64 start = mChart->GetInspectStart();
        final Int64 end = mChart->GetInspectEnd();
        final Int64 now = System->CurrentTimeMillis();

        final Context context = GetActivity();

        NetworkStatsHistory.Entry entry = NULL;
        if (IsAppDetailMode() && mChartData != NULL && mChartData.detail != NULL) {
            // bind foreground/background to piechart and labels
            entry = mChartData.detailDefault->GetValues(start, end, now, entry);
            final Int64 defaultBytes = entry.rxBytes + entry.txBytes;
            entry = mChartData.detailForeground->GetValues(start, end, now, entry);
            final Int64 foregroundBytes = entry.rxBytes + entry.txBytes;
            final Int64 totalBytes = defaultBytes + foregroundBytes;

            if (mAppTotal != NULL) {
                mAppTotal->SetText(Formatter->FormatFileSize(context, totalBytes));
            }
            mAppBackground->SetText(Formatter->FormatFileSize(context, defaultBytes));
            mAppForeground->SetText(Formatter->FormatFileSize(context, foregroundBytes));

            // and finally leave with summary data for label below
            entry = mChartData.detail->GetValues(start, end, now, NULL);

            GetLoaderManager()->DestroyLoader(LOADER_SUMMARY);

            mCycleSummary->SetVisibility(View.GONE);

        } else {
            if (mChartData != NULL) {
                entry = mChartData.network->GetValues(start, end, now, NULL);
            }

            mCycleSummary->SetVisibility(View.VISIBLE);

            // kick off loader for detailed stats
            GetLoaderManager()->RestartLoader(LOADER_SUMMARY,
                    SummaryForAllUidLoader->BuildArgs(mTemplate, start, end), mSummaryCallbacks);
        }

        final Int64 totalBytes = entry != NULL ? entry.rxBytes + entry.txBytes : 0;
        final String totalPhrase = Formatter->FormatFileSize(context, totalBytes);
        mCycleSummary->SetText(totalPhrase);

        if (TAB_MOBILE->Equals(mCurrentTab) || TAB_3G->Equals(mCurrentTab)
                || TAB_4G->Equals(mCurrentTab)) {
            if (IsAppDetailMode()) {
                mDisclaimer->SetVisibility(View.GONE);
            } else {
                mDisclaimer->SetVisibility(View.VISIBLE);
            }
        } else {
            mDisclaimer->SetVisibility(View.GONE);
        }

        // initial layout is finished above, ensure we have transitions
        EnsureLayoutTransitions();
    }

    private final LoaderCallbacks<ChartData> mChartDataCallbacks = new LoaderCallbacks<
            ChartData>() {
        //@Override
        public Loader<ChartData> OnCreateLoader(Int32 id, Bundle args) {
            return new ChartDataLoader(GetActivity(), mStatsSession, args);
        }

        //@Override
        CARAPI OnLoadFinished(Loader<ChartData> loader, ChartData data) {
            mChartData = data;
            mChart->BindNetworkStats(mChartData.network);
            mChart->BindDetailNetworkStats(mChartData.detail);

            // calcuate policy cycles based on available data
            UpdatePolicy(TRUE);
            UpdateAppDetail();

            // force scroll to top of body when showing detail
            if (mChartData.detail != NULL) {
                mListView->SmoothScrollToPosition(0);
            }
        }

        //@Override
        CARAPI OnLoaderReset(Loader<ChartData> loader) {
            mChartData = NULL;
            mChart->BindNetworkStats(NULL);
            mChart->BindDetailNetworkStats(NULL);
        }
    };

    private final LoaderCallbacks<NetworkStats> mSummaryCallbacks = new LoaderCallbacks<
            NetworkStats>() {
        //@Override
        public Loader<NetworkStats> OnCreateLoader(Int32 id, Bundle args) {
            return new SummaryForAllUidLoader(GetActivity(), mStatsSession, args);
        }

        //@Override
        CARAPI OnLoadFinished(Loader<NetworkStats> loader, NetworkStats data) {
            final Int32[] restrictedUids = mPolicyManager->GetUidsWithPolicy(
                    POLICY_REJECT_METERED_BACKGROUND);
            mAdapter->BindStats(data, restrictedUids);
            UpdateEmptyVisible();
        }

        //@Override
        CARAPI OnLoaderReset(Loader<NetworkStats> loader) {
            mAdapter->BindStats(NULL, new Int32[0]);
            UpdateEmptyVisible();
        }

        private void UpdateEmptyVisible() {
            final Boolean isEmpty = mAdapter->IsEmpty() && !IsAppDetailMode();
            mEmpty->SetVisibility(isEmpty ? View.VISIBLE : View.GONE);
            mStupidPadding->SetVisibility(isEmpty ? View.VISIBLE : View.GONE);
        }
    };

    @Deprecated
    private Boolean IsMobilePolicySplit() {
        final Context context = GetActivity();
        if (HasReadyMobileRadio(context)) {
            final TelephonyManager tele = TelephonyManager->From(context);
            return mPolicyEditor->IsMobilePolicySplit(GetActiveSubscriberId(context));
        } else {
            return FALSE;
        }
    }

    @Deprecated
    private void SetMobilePolicySplit(Boolean split) {
        final Context context = GetActivity();
        if (HasReadyMobileRadio(context)) {
            final TelephonyManager tele = TelephonyManager->From(context);
            mPolicyEditor->SetMobilePolicySplit(GetActiveSubscriberId(context), split);
        }
    }

    private static String GetActiveSubscriberId(Context context) {
        final TelephonyManager tele = TelephonyManager->From(context);
        final String actualSubscriberId = tele->GetSubscriberId();
        return SystemProperties->Get(TEST_SUBSCRIBER_PROP, actualSubscriberId);
    }

    private DataUsageChartListener mChartListener = new DataUsageChartListener() {
        //@Override
        CARAPI OnWarningChanged() {
            SetPolicyWarningBytes(mChart->GetWarningBytes());
        }

        //@Override
        CARAPI OnLimitChanged() {
            SetPolicyLimitBytes(mChart->GetLimitBytes());
        }

        //@Override
        CARAPI RequestWarningEdit() {
            WarningEditorFragment->Show(DataUsageSummary.this);
        }

        //@Override
        CARAPI RequestLimitEdit() {
            LimitEditorFragment->Show(DataUsageSummary.this);
        }
    };

    /**
     * List item that reflects a specific data usage cycle.
     */
    public static class CycleItem implements Comparable<CycleItem> {
        public CharSequence label;
        public Int64 start;
        public Int64 end;

        CycleItem(CharSequence label) {
            this.label = label;
        }

        public CycleItem(Context context, Int64 start, Int64 end) {
            this.label = FormatDateRange(context, start, end);
            this.start = start;
            this.end = end;
        }

        //@Override
        CARAPI ToString(
        /* [out] */ String* str)
    {
            return label->ToString();
        }

        //@Override
        public Boolean Equals(Object o) {
            if (o instanceof CycleItem) {
                final CycleItem another = (CycleItem) o;
                return start == another.start && end == another.end;
            }
            return FALSE;
        }

        //@Override
        public Int32 CompareTo(CycleItem another) {
            return Long->Compare(start, another.start);
        }
    }

    private static const StringBuilder sBuilder = new StringBuilder(50);
    private static const java.util.Formatter sFormatter = new java.util->Formatter(
            sBuilder, Locale->GetDefault());

    public static String FormatDateRange(Context context, Int64 start, Int64 end) {
        final Int32 flags = FORMAT_SHOW_DATE | FORMAT_ABBREV_MONTH;

        synchronized(sBuilder) {
            sBuilder->SetLength(0);
            return DateUtils->FormatDateRange(context, sFormatter, start, end, flags, NULL)
                    .ToString();
        }
    }

    /**
     * Special-case data usage cycle that triggers dialog to change
     * {@link NetworkPolicy#cycleDay}.
     */
    public static class CycleChangeItem extends CycleItem {
        public CycleChangeItem(Context context) {
            Super(context->GetString(R::string::data_usage_change_cycle));
        }
    }

    public static class CycleAdapter extends ArrayAdapter<CycleItem> {
        private Boolean mChangePossible = FALSE;
        private Boolean mChangeVisible = FALSE;

        private final CycleChangeItem mChangeItem;

        public CycleAdapter(Context context) {
            Super(context, R.layout.data_usage_cycle_item);
            SetDropDownViewResource(R.layout.data_usage_cycle_item_dropdown);
            mChangeItem = new CycleChangeItem(context);
        }

        CARAPI SetChangePossible(Boolean possible) {
            mChangePossible = possible;
            UpdateChange();
        }

        CARAPI SetChangeVisible(Boolean visible) {
            mChangeVisible = visible;
            UpdateChange();
        }

        private void UpdateChange() {
            Remove(mChangeItem);
            if (mChangePossible && mChangeVisible) {
                Add(mChangeItem);
            }
        }

        /**
         * Find position of {@link CycleItem} in this adapter which is nearest
         * the given {@link CycleItem}.
         */
        public Int32 FindNearestPosition(CycleItem target) {
            if (target != NULL) {
                final Int32 count = GetCount();
                for (Int32 i = count - 1; i >= 0; i--) {
                    final CycleItem item = GetItem(i);
                    if (item instanceof CycleChangeItem) {
                        continue;
                    } else if (item->CompareTo(target) >= 0) {
                        return i;
                    }
                }
            }
            return 0;
        }
    }

    public static class AppItem implements Comparable<AppItem>, Parcelable {
        public static const Int32 CATEGORY_USER = 0;
        public static const Int32 CATEGORY_APP_TITLE = 1;
        public static const Int32 CATEGORY_APP = 2;

        public final Int32 key;
        public Boolean restricted;
        public Int32 category;

        public SparseBooleanArray uids = new SparseBooleanArray();
        public Int64 total;

        public AppItem() {
            this.key = 0;
        }

        public AppItem(Int32 key) {
            this.key = key;
        }

        public AppItem(Parcel parcel) {
            key = parcel->ReadInt();
            uids = parcel->ReadSparseBooleanArray();
            total = parcel->ReadLong();
        }

        CARAPI AddUid(Int32 uid) {
            uids->Put(uid, TRUE);
        }

        //@Override
        CARAPI WriteToParcel(Parcel dest, Int32 flags) {
            dest->WriteInt(key);
            dest->WriteSparseBooleanArray(uids);
            dest->WriteLong(total);
        }

        //@Override
        public Int32 DescribeContents() {
            return 0;
        }

        //@Override
        public Int32 CompareTo(AppItem another) {
            Int32 comparison = Integer->Compare(category, another.category);
            if (comparison == 0) {
                comparison = Long->Compare(another.total, total);
            }
            return comparison;
        }

        public static const Creator<AppItem> CREATOR = new Creator<AppItem>() {
            //@Override
            public AppItem CreateFromParcel(Parcel in) {
                return new AppItem(in);
            }

            //@Override
            public AppItem[] NewArray(Int32 size) {
                return new AppItem[size];
            }
        };
    }

    /**
     * Adapter of applications, sorted by total usage descending.
     */
    public static class DataUsageAdapter extends BaseAdapter {
        private final UidDetailProvider mProvider;
        private final Int32 mInsetSide;
        private final UserManager mUm;

        private ArrayList<AppItem> mItems = Lists->NewArrayList();
        private Int64 mLargest;

        public DataUsageAdapter(final UserManager userManager, UidDetailProvider provider, Int32 insetSide) {
            mProvider = CheckNotNull(provider);
            mInsetSide = insetSide;
            mUm = userManager;
        }

        /**
         * Bind the given {@link NetworkStats}, or {@code NULL} to clear list.
         */
        CARAPI BindStats(NetworkStats stats, Int32[] restrictedUids) {
            mItems->Clear();
            mLargest = 0;

            final Int32 currentUserId = ActivityManager->GetCurrentUser();
            final List<UserHandle> profiles = mUm->GetUserProfiles();
            final SparseArray<AppItem> knownItems = new SparseArray<AppItem>();

            NetworkStats.Entry entry = NULL;
            final Int32 size = stats != NULL ? stats->Size() : 0;
            for (Int32 i = 0; i < size; i++) {
                entry = stats->GetValues(i, entry);

                // Decide how to collapse items together
                final Int32 uid = entry.uid;

                final Int32 collapseKey;
                final Int32 category;
                final Int32 userId = UserHandle->GetUserId(uid);
                if (UserHandle->IsApp(uid)) {
                    if (profiles->Contains(new UserHandle(userId))) {
                        if (userId != currentUserId) {
                            // Add to a managed user item.
                            final Int32 managedKey = UidDetailProvider->BuildKeyForUser(userId);
                            Accumulate(managedKey, knownItems, entry,
                                    AppItem.CATEGORY_USER);
                        }
                        // Add to app item.
                        collapseKey = uid;
                        category = AppItem.CATEGORY_APP;
                    } else {
                        // Add to other user item.
                        collapseKey = UidDetailProvider->BuildKeyForUser(userId);
                        category = AppItem.CATEGORY_USER;
                    }
                } else if (uid == UID_REMOVED || uid == UID_TETHERING) {
                    collapseKey = uid;
                    category = AppItem.CATEGORY_APP;
                } else {
                    collapseKey = android.os.Process.SYSTEM_UID;
                    category = AppItem.CATEGORY_APP;
                }
                Accumulate(collapseKey, knownItems, entry, category);
            }

            final Int32 restrictedUidsMax = restrictedUids.length;
            for (Int32 i = 0; i < restrictedUidsMax; ++i) {
                final Int32 uid = restrictedUids[i];
                // Only splice in restricted state for current user or managed users
                if (!profiles->Contains(new UserHandle(UserHandle->GetUserId(uid)))) {
                    continue;
                }

                AppItem item = knownItems->Get(uid);
                if (item == NULL) {
                    item = new AppItem(uid);
                    item.total = -1;
                    mItems->Add(item);
                    knownItems->Put(item.key, item);
                }
                item.restricted = TRUE;
            }

            if (!mItems->IsEmpty()) {
                final AppItem title = new AppItem();
                title.category = AppItem.CATEGORY_APP_TITLE;
                mItems->Add(title);
            }

            Collections->Sort(mItems);
            NotifyDataSetChanged();
        }

        /**
         * Accumulate data usage of a network stats entry for the item mapped by the collapse key.
         * Creates the item if needed.
         *
         * @param collapseKey the collapse key used to map the item.
         * @param knownItems collection of known (already existing) items.
         * @param entry the network stats entry to extract data usage from.
         * @param itemCategory the item is categorized on the list view by this category. Must be
         *            either AppItem.APP_ITEM_CATEGORY or AppItem.MANAGED_USER_ITEM_CATEGORY
         */
        private void Accumulate(Int32 collapseKey, final SparseArray<AppItem> knownItems,
                NetworkStats.Entry entry, Int32 itemCategory) {
            final Int32 uid = entry.uid;
            AppItem item = knownItems->Get(collapseKey);
            if (item == NULL) {
                item = new AppItem(collapseKey);
                item.category = itemCategory;
                mItems->Add(item);
                knownItems->Put(item.key, item);
            }
            item->AddUid(uid);
            item.total += entry.rxBytes + entry.txBytes;
            if (mLargest < item.total) {
                mLargest = item.total;
            }
        }

        //@Override
        public Int32 GetCount() {
            return mItems->Size();
        }

        //@Override
        public Object GetItem(Int32 position) {
            return mItems->Get(position);
        }

        //@Override
        public Int64 GetItemId(Int32 position) {
            return mItems->Get(position).key;
        }

        /**
         * See {@link #getItemViewType} for the view types.
         */
        //@Override
        public Int32 GetViewTypeCount() {
            return 2;
        }

        /**
         * Returns 1 for separator items and 0 for anything else.
         */
        //@Override
        public Int32 GetItemViewType(Int32 position) {
            final AppItem item = mItems->Get(position);
            if (item.category == AppItem.CATEGORY_APP_TITLE) {
                return 1;
            } else {
                return 0;
            }
        }

        //@Override
        public Boolean AreAllItemsEnabled() {
            return FALSE;
        }

        //@Override
        public Boolean IsEnabled(Int32 position) {
            if (position > mItems->Size()) {
                throw new ArrayIndexOutOfBoundsException();
            }
            return GetItemViewType(position) == 0;
        }

        //@Override
        public View GetView(Int32 position, View convertView, ViewGroup parent) {
            final AppItem item = mItems->Get(position);
            if (GetItemViewType(position) == 1) {
                if (convertView == NULL) {
                    convertView = InflateCategoryHeader(LayoutInflater->From(parent->GetContext()),
                            parent);
                }

                final TextView title = (TextView) convertView->FindViewById(android.R.id.title);
                title->SetText(R::string::data_usage_app);

            } else {
                if (convertView == NULL) {
                    convertView = LayoutInflater->From(parent->GetContext()).Inflate(
                            R.layout.data_usage_item, parent, FALSE);

                    if (mInsetSide > 0) {
                        convertView->SetPaddingRelative(mInsetSide, 0, mInsetSide, 0);
                    }
                }

                final Context context = parent->GetContext();

                final TextView text1 = (TextView) convertView->FindViewById(android.R.id.text1);
                final ProgressBar progress = (ProgressBar) convertView->FindViewById(
                        android.R.id.progress);

                // kick off async load of app details
                UidDetailTask->BindView(mProvider, item, convertView);

                if (item.restricted && item.total <= 0) {
                    text1->SetText(R::string::data_usage_app_restricted);
                    progress->SetVisibility(View.GONE);
                } else {
                    text1->SetText(Formatter->FormatFileSize(context, item.total));
                    progress->SetVisibility(View.VISIBLE);
                }

                final Int32 percentTotal = mLargest != 0 ? (Int32) (item.total * 100 / mLargest) : 0;
                progress->SetProgress(percentTotal);
            }

            return convertView;
        }
    }

    /**
     * Empty {@link Fragment} that controls display of UID details in
     * {@link DataUsageSummary}.
     */
    public static class AppDetailsFragment extends Fragment {
        private static const String EXTRA_APP = "app";

        public static void Show(DataUsageSummary parent, AppItem app, CharSequence label) {
            if (!parent->IsAdded()) return;

            final Bundle args = new Bundle();
            args->PutParcelable(EXTRA_APP, app);

            final AppDetailsFragment fragment = new AppDetailsFragment();
            fragment->SetArguments(args);
            fragment->SetTargetFragment(parent, 0);
            final FragmentTransaction ft = parent->GetFragmentManager()->BeginTransaction();
            ft->Add(fragment, TAG_APP_DETAILS);
            ft->AddToBackStack(TAG_APP_DETAILS);
            ft->SetBreadCrumbTitle(
                    parent->GetResources()->GetString(R::string::data_usage_app_summary_title));
            ft->CommitAllowingStateLoss();
        }

        //@Override
        CARAPI OnStart() {
            super->OnStart();
            final DataUsageSummary target = (DataUsageSummary) GetTargetFragment();
            target.mCurrentApp = GetArguments()->GetParcelable(EXTRA_APP);
            target->UpdateBody();
        }

        //@Override
        CARAPI OnStop() {
            super->OnStop();
            final DataUsageSummary target = (DataUsageSummary) GetTargetFragment();
            target.mCurrentApp = NULL;
            target->UpdateBody();
        }
    }

    /**
     * Dialog to request user confirmation before setting
     * {@link NetworkPolicy#limitBytes}.
     */
    public static class ConfirmLimitFragment extends DialogFragment {
        private static const String EXTRA_MESSAGE = "message";
        private static const String EXTRA_LIMIT_BYTES = "limitBytes";

        public static void Show(DataUsageSummary parent) {
            if (!parent->IsAdded()) return;

            final NetworkPolicy policy = parent.mPolicyEditor->GetPolicy(parent.mTemplate);
            if (policy == NULL) return;

            final Resources res = parent->GetResources();
            final CharSequence message;
            final Int64 minLimitBytes = (Int64) (policy.warningBytes * 1.2f);
            final Int64 limitBytes;

            // TODO: customize default limits based on network template
            final String currentTab = parent.mCurrentTab;
            if (TAB_3G->Equals(currentTab)) {
                message = res->GetString(R::string::data_usage_limit_dialog_mobile);
                limitBytes = Math->Max(5 * GB_IN_BYTES, minLimitBytes);
            } else if (TAB_4G->Equals(currentTab)) {
                message = res->GetString(R::string::data_usage_limit_dialog_mobile);
                limitBytes = Math->Max(5 * GB_IN_BYTES, minLimitBytes);
            } else if (TAB_MOBILE->Equals(currentTab)) {
                message = res->GetString(R::string::data_usage_limit_dialog_mobile);
                limitBytes = Math->Max(5 * GB_IN_BYTES, minLimitBytes);
            } else {
                throw new IllegalArgumentException("unknown current tab: " + currentTab);
            }

            final Bundle args = new Bundle();
            args->PutCharSequence(EXTRA_MESSAGE, message);
            args->PutLong(EXTRA_LIMIT_BYTES, limitBytes);

            final ConfirmLimitFragment dialog = new ConfirmLimitFragment();
            dialog->SetArguments(args);
            dialog->SetTargetFragment(parent, 0);
            dialog->Show(parent->GetFragmentManager(), TAG_CONFIRM_LIMIT);
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            final Context context = GetActivity();

            final CharSequence message = GetArguments()->GetCharSequence(EXTRA_MESSAGE);
            final Int64 limitBytes = GetArguments()->GetLong(EXTRA_LIMIT_BYTES);

            final AlertDialog.Builder builder = new AlertDialog->Builder(context);
            builder->SetTitle(R::string::data_usage_limit_dialog_title);
            builder->SetMessage(message);

            builder->SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                    final DataUsageSummary target = (DataUsageSummary) GetTargetFragment();
                    if (target != NULL) {
                        target->SetPolicyLimitBytes(limitBytes);
                    }
                }
            });

            return builder->Create();
        }
    }

    /**
     * Dialog to edit {@link NetworkPolicy#cycleDay}.
     */
    public static class CycleEditorFragment extends DialogFragment {
        private static const String EXTRA_TEMPLATE = "template";

        public static void Show(DataUsageSummary parent) {
            if (!parent->IsAdded()) return;

            final Bundle args = new Bundle();
            args->PutParcelable(EXTRA_TEMPLATE, parent.mTemplate);

            final CycleEditorFragment dialog = new CycleEditorFragment();
            dialog->SetArguments(args);
            dialog->SetTargetFragment(parent, 0);
            dialog->Show(parent->GetFragmentManager(), TAG_CYCLE_EDITOR);
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            final Context context = GetActivity();
            final DataUsageSummary target = (DataUsageSummary) GetTargetFragment();
            final NetworkPolicyEditor editor = target.mPolicyEditor;

            final AlertDialog.Builder builder = new AlertDialog->Builder(context);
            final LayoutInflater dialogInflater = LayoutInflater->From(builder->GetContext());

            final View view = dialogInflater->Inflate(R.layout.data_usage_cycle_editor, NULL, FALSE);
            final NumberPicker cycleDayPicker = (NumberPicker) view->FindViewById(R.id.cycle_day);

            final NetworkTemplate template = GetArguments()->GetParcelable(EXTRA_TEMPLATE);
            final Int32 cycleDay = editor->GetPolicyCycleDay(template);

            cycleDayPicker->SetMinValue(1);
            cycleDayPicker->SetMaxValue(31);
            cycleDayPicker->SetValue(cycleDay);
            cycleDayPicker->SetWrapSelectorWheel(TRUE);

            builder->SetTitle(R::string::data_usage_cycle_editor_title);
            builder->SetView(view);

            builder->SetPositiveButton(R::string::data_usage_cycle_editor_positive,
                    new DialogInterface->OnClickListener() {
                        //@Override
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            // clear focus to finish pending text edits
                            cycleDayPicker->ClearFocus();

                            final Int32 cycleDay = cycleDayPicker->GetValue();
                            final String cycleTimezone = new Time().timezone;
                            editor->SetPolicyCycleDay(template, cycleDay, cycleTimezone);
                            target->UpdatePolicy(TRUE);
                        }
                    });

            return builder->Create();
        }
    }

    /**
     * Dialog to edit {@link NetworkPolicy#warningBytes}.
     */
    public static class WarningEditorFragment extends DialogFragment {
        private static const String EXTRA_TEMPLATE = "template";

        public static void Show(DataUsageSummary parent) {
            if (!parent->IsAdded()) return;

            final Bundle args = new Bundle();
            args->PutParcelable(EXTRA_TEMPLATE, parent.mTemplate);

            final WarningEditorFragment dialog = new WarningEditorFragment();
            dialog->SetArguments(args);
            dialog->SetTargetFragment(parent, 0);
            dialog->Show(parent->GetFragmentManager(), TAG_WARNING_EDITOR);
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            final Context context = GetActivity();
            final DataUsageSummary target = (DataUsageSummary) GetTargetFragment();
            final NetworkPolicyEditor editor = target.mPolicyEditor;

            final AlertDialog.Builder builder = new AlertDialog->Builder(context);
            final LayoutInflater dialogInflater = LayoutInflater->From(builder->GetContext());

            final View view = dialogInflater->Inflate(R.layout.data_usage_bytes_editor, NULL, FALSE);
            final NumberPicker bytesPicker = (NumberPicker) view->FindViewById(R.id.bytes);

            final NetworkTemplate template = GetArguments()->GetParcelable(EXTRA_TEMPLATE);
            final Int64 warningBytes = editor->GetPolicyWarningBytes(template);
            final Int64 limitBytes = editor->GetPolicyLimitBytes(template);

            bytesPicker->SetMinValue(0);
            if (limitBytes != LIMIT_DISABLED) {
                bytesPicker->SetMaxValue((Int32) (limitBytes / MB_IN_BYTES) - 1);
            } else {
                bytesPicker->SetMaxValue(Integer.MAX_VALUE);
            }
            bytesPicker->SetValue((Int32) (warningBytes / MB_IN_BYTES));
            bytesPicker->SetWrapSelectorWheel(FALSE);

            builder->SetTitle(R::string::data_usage_warning_editor_title);
            builder->SetView(view);

            builder->SetPositiveButton(R::string::data_usage_cycle_editor_positive,
                    new DialogInterface->OnClickListener() {
                        //@Override
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            // clear focus to finish pending text edits
                            bytesPicker->ClearFocus();

                            final Int64 bytes = bytesPicker->GetValue() * MB_IN_BYTES;
                            editor->SetPolicyWarningBytes(template, bytes);
                            target->UpdatePolicy(FALSE);
                        }
                    });

            return builder->Create();
        }
    }

    /**
     * Dialog to edit {@link NetworkPolicy#limitBytes}.
     */
    public static class LimitEditorFragment extends DialogFragment {
        private static const String EXTRA_TEMPLATE = "template";

        public static void Show(DataUsageSummary parent) {
            if (!parent->IsAdded()) return;

            final Bundle args = new Bundle();
            args->PutParcelable(EXTRA_TEMPLATE, parent.mTemplate);

            final LimitEditorFragment dialog = new LimitEditorFragment();
            dialog->SetArguments(args);
            dialog->SetTargetFragment(parent, 0);
            dialog->Show(parent->GetFragmentManager(), TAG_LIMIT_EDITOR);
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            final Context context = GetActivity();
            final DataUsageSummary target = (DataUsageSummary) GetTargetFragment();
            final NetworkPolicyEditor editor = target.mPolicyEditor;

            final AlertDialog.Builder builder = new AlertDialog->Builder(context);
            final LayoutInflater dialogInflater = LayoutInflater->From(builder->GetContext());

            final View view = dialogInflater->Inflate(R.layout.data_usage_bytes_editor, NULL, FALSE);
            final NumberPicker bytesPicker = (NumberPicker) view->FindViewById(R.id.bytes);

            final NetworkTemplate template = GetArguments()->GetParcelable(EXTRA_TEMPLATE);
            final Int64 warningBytes = editor->GetPolicyWarningBytes(template);
            final Int64 limitBytes = editor->GetPolicyLimitBytes(template);

            bytesPicker->SetMaxValue(Integer.MAX_VALUE);
            if (warningBytes != WARNING_DISABLED && limitBytes > 0) {
                bytesPicker->SetMinValue((Int32) (warningBytes / MB_IN_BYTES) + 1);
            } else {
                bytesPicker->SetMinValue(0);
            }
            bytesPicker->SetValue((Int32) (limitBytes / MB_IN_BYTES));
            bytesPicker->SetWrapSelectorWheel(FALSE);

            builder->SetTitle(R::string::data_usage_limit_editor_title);
            builder->SetView(view);

            builder->SetPositiveButton(R::string::data_usage_cycle_editor_positive,
                    new DialogInterface->OnClickListener() {
                        //@Override
                        CARAPI OnClick(DialogInterface dialog, Int32 which) {
                            // clear focus to finish pending text edits
                            bytesPicker->ClearFocus();

                            final Int64 bytes = bytesPicker->GetValue() * MB_IN_BYTES;
                            editor->SetPolicyLimitBytes(template, bytes);
                            target->UpdatePolicy(FALSE);
                        }
                    });

            return builder->Create();
        }
    }
    /**
     * Dialog to request user confirmation before disabling data.
     */
    public static class ConfirmDataDisableFragment extends DialogFragment {
        public static void Show(DataUsageSummary parent) {
            if (!parent->IsAdded()) return;

            final ConfirmDataDisableFragment dialog = new ConfirmDataDisableFragment();
            dialog->SetTargetFragment(parent, 0);
            dialog->Show(parent->GetFragmentManager(), TAG_CONFIRM_DATA_DISABLE);
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            final Context context = GetActivity();

            final AlertDialog.Builder builder = new AlertDialog->Builder(context);
            builder->SetMessage(R::string::data_usage_disable_mobile);

            builder->SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                    final DataUsageSummary target = (DataUsageSummary) GetTargetFragment();
                    if (target != NULL) {
                        // TODO: extend to modify policy enabled flag.
                        target->SetMobileDataEnabled(FALSE);
                    }
                }
            });
            builder->SetNegativeButton(android.R::string::cancel, NULL);

            return builder->Create();
        }
    }

    /**
     * Dialog to request user confirmation before setting
     * {@link INetworkPolicyManager#SetRestrictBackground(Boolean)}.
     */
    public static class ConfirmRestrictFragment extends DialogFragment {
        public static void Show(DataUsageSummary parent) {
            if (!parent->IsAdded()) return;

            final ConfirmRestrictFragment dialog = new ConfirmRestrictFragment();
            dialog->SetTargetFragment(parent, 0);
            dialog->Show(parent->GetFragmentManager(), TAG_CONFIRM_RESTRICT);
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            final Context context = GetActivity();

            final AlertDialog.Builder builder = new AlertDialog->Builder(context);
            builder->SetTitle(R::string::data_usage_restrict_background_title);
            if (Utils->HasMultipleUsers(context)) {
                builder->SetMessage(R::string::data_usage_restrict_background_multiuser);
            } else {
                builder->SetMessage(R::string::data_usage_restrict_background);
            }

            builder->SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                    final DataUsageSummary target = (DataUsageSummary) GetTargetFragment();
                    if (target != NULL) {
                        target->SetRestrictBackground(TRUE);
                    }
                }
            });
            builder->SetNegativeButton(android.R::string::cancel, NULL);

            return builder->Create();
        }
    }

    /**
     * Dialog to inform user that {@link #POLICY_REJECT_METERED_BACKGROUND}
     * change has been denied, usually based on
     * {@link DataUsageSummary#HasLimitedNetworks()}.
     */
    public static class DeniedRestrictFragment extends DialogFragment {
        public static void Show(DataUsageSummary parent) {
            if (!parent->IsAdded()) return;

            final DeniedRestrictFragment dialog = new DeniedRestrictFragment();
            dialog->SetTargetFragment(parent, 0);
            dialog->Show(parent->GetFragmentManager(), TAG_DENIED_RESTRICT);
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            final Context context = GetActivity();

            final AlertDialog.Builder builder = new AlertDialog->Builder(context);
            builder->SetTitle(R::string::data_usage_app_restrict_background);
            builder->SetMessage(R::string::data_usage_restrict_denied_dialog);
            builder->SetPositiveButton(android.R::string::ok, NULL);

            return builder->Create();
        }
    }

    /**
     * Dialog to request user confirmation before setting
     * {@link #POLICY_REJECT_METERED_BACKGROUND}.
     */
    public static class ConfirmAppRestrictFragment extends DialogFragment {
        public static void Show(DataUsageSummary parent) {
            if (!parent->IsAdded()) return;

            final ConfirmAppRestrictFragment dialog = new ConfirmAppRestrictFragment();
            dialog->SetTargetFragment(parent, 0);
            dialog->Show(parent->GetFragmentManager(), TAG_CONFIRM_APP_RESTRICT);
        }

        //@Override
        public Dialog OnCreateDialog(Bundle savedInstanceState) {
            final Context context = GetActivity();

            final AlertDialog.Builder builder = new AlertDialog->Builder(context);
            builder->SetTitle(R::string::data_usage_app_restrict_dialog_title);
            builder->SetMessage(R::string::data_usage_app_restrict_dialog);

            builder->SetPositiveButton(android.R::string::ok, new DialogInterface->OnClickListener() {
                //@Override
                CARAPI OnClick(DialogInterface dialog, Int32 which) {
                    final DataUsageSummary target = (DataUsageSummary) GetTargetFragment();
                    if (target != NULL) {
                        target->SetAppRestrictBackground(TRUE);
                    }
                }
            });
            builder->SetNegativeButton(android.R::string::cancel, NULL);

            return builder->Create();
        }
    }

    /**
     * Compute default tab that should be selected, based on
     * {@link NetworkPolicyManager#EXTRA_NETWORK_TEMPLATE} extra.
     */
    private static String ComputeTabFromIntent(Intent intent) {
        final NetworkTemplate template = intent->GetParcelableExtra(EXTRA_NETWORK_TEMPLATE);
        if (template == NULL) return NULL;

        switch (template->GetMatchRule()) {
            case MATCH_MOBILE_3G_LOWER:
                return TAB_3G;
            case MATCH_MOBILE_4G:
                return TAB_4G;
            case MATCH_MOBILE_ALL:
                return TAB_MOBILE;
            case MATCH_WIFI:
                return TAB_WIFI;
            default:
                return NULL;
        }
    }

    /**
     * Background task that loads {@link UidDetail}, binding to
     * {@link DataUsageAdapter} row item when finished.
     */
    private static class UidDetailTask extends AsyncTask<Void, Void, UidDetail> {
        private final UidDetailProvider mProvider;
        private final AppItem mItem;
        private final View mTarget;

        private UidDetailTask(UidDetailProvider provider, AppItem item, View target) {
            mProvider = CheckNotNull(provider);
            mItem = CheckNotNull(item);
            mTarget = CheckNotNull(target);
        }

        public static void BindView(
                UidDetailProvider provider, AppItem item, View target) {
            final UidDetailTask existing = (UidDetailTask) target->GetTag();
            if (existing != NULL) {
                existing->Cancel(FALSE);
            }

            final UidDetail cachedDetail = provider->GetUidDetail(item.key, FALSE);
            if (cachedDetail != NULL) {
                BindView(cachedDetail, target);
            } else {
                target->SetTag(new UidDetailTask(provider, item, target).ExecuteOnExecutor(
                        AsyncTask.THREAD_POOL_EXECUTOR));
            }
        }

        private static void BindView(UidDetail detail, View target) {
            final ImageView icon = (ImageView) target->FindViewById(android.R.id.icon);
            final TextView title = (TextView) target->FindViewById(android.R.id.title);

            if (detail != NULL) {
                icon->SetImageDrawable(detail.icon);
                title->SetText(detail.label);
                title->SetContentDescription(detail.contentDescription);
            } else {
                icon->SetImageDrawable(NULL);
                title->SetText(NULL);
            }
        }

        //@Override
        protected void OnPreExecute() {
            BindView(NULL, mTarget);
        }

        //@Override
        protected UidDetail DoInBackground(Void... params) {
            return mProvider->GetUidDetail(mItem.key, TRUE);
        }

        //@Override
        protected void OnPostExecute(UidDetail result) {
            BindView(result, mTarget);
        }
    }

    /**
     * Test if device has a mobile data radio with SIM in ready state.
     */
    public static Boolean HasReadyMobileRadio(Context context) {
        if (TEST_RADIOS) {
            return SystemProperties->Get(TEST_RADIOS_PROP).Contains("mobile");
        }

        final ConnectivityManager conn = ConnectivityManager->From(context);
        final TelephonyManager tele = TelephonyManager->From(context);

        // require both supported network and ready SIM
        return conn->IsNetworkSupported(TYPE_MOBILE) && tele->GetSimState() == SIM_STATE_READY;
    }

    /**
     * Test if device has a mobile 4G data radio.
     */
    public static Boolean HasReadyMobile4gRadio(Context context) {
        if (!NetworkPolicyEditor.ENABLE_SPLIT_POLICIES) {
            return FALSE;
        }
        if (TEST_RADIOS) {
            return SystemProperties->Get(TEST_RADIOS_PROP).Contains("4g");
        }

        final ConnectivityManager conn = ConnectivityManager->From(context);
        final TelephonyManager tele = TelephonyManager->From(context);

        final Boolean hasWimax = conn->IsNetworkSupported(TYPE_WIMAX);
        final Boolean hasLte = (tele->GetLteOnCdmaMode() == PhoneConstants.LTE_ON_CDMA_TRUE)
                && HasReadyMobileRadio(context);
        return hasWimax || hasLte;
    }

    /**
     * Test if device has a Wi-Fi data radio.
     */
    public static Boolean HasWifiRadio(Context context) {
        if (TEST_RADIOS) {
            return SystemProperties->Get(TEST_RADIOS_PROP).Contains("wifi");
        }

        final ConnectivityManager conn = ConnectivityManager->From(context);
        return conn->IsNetworkSupported(TYPE_WIFI);
    }

    /**
     * Test if device has an ethernet network connection.
     */
    public Boolean HasEthernet(Context context) {
        if (TEST_RADIOS) {
            return SystemProperties->Get(TEST_RADIOS_PROP).Contains("ethernet");
        }

        final ConnectivityManager conn = ConnectivityManager->From(context);
        final Boolean hasEthernet = conn->IsNetworkSupported(TYPE_ETHERNET);

        final Int64 ethernetBytes;
        if (mStatsSession != NULL) {
            try {
                ethernetBytes = mStatsSession->GetSummaryForNetwork(
                        NetworkTemplate->BuildTemplateEthernet(), Long.MIN_VALUE, Long.MAX_VALUE)
                        .GetTotalBytes();
            } catch (RemoteException e) {
                throw new RuntimeException(e);
            }
        } else {
            ethernetBytes = 0;
        }

        // only show ethernet when both hardware present and traffic has occurred
        return hasEthernet && ethernetBytes > 0;
    }

    /**
     * Inflate a {@link Preference} style layout, adding the given {@link View}
     * widget into {@link android.R.id#widget_frame}.
     */
    private static View InflatePreference(LayoutInflater inflater, ViewGroup root, View widget) {
        final View view = inflater->Inflate(R.layout.preference, root, FALSE);
        final LinearLayout widgetFrame = (LinearLayout) view->FindViewById(
                android.R.id.widget_frame);
        widgetFrame->AddView(widget, new LinearLayout->LayoutParams(WRAP_CONTENT, WRAP_CONTENT));
        return view;
    }

    private static View InflateCategoryHeader(LayoutInflater inflater, ViewGroup root) {
        final TypedArray a = inflater->GetContext()->ObtainStyledAttributes(NULL,
                R.styleable.Preference,
                R.attr.preferenceCategoryStyle, 0);
        final Int32 resId = a->GetResourceId(R.styleable.Preference_layout, 0);
        return inflater->Inflate(resId, root, FALSE);
    }

    /**
     * Test if any networks are currently limited.
     */
    private Boolean HasLimitedNetworks() {
        return !BuildLimitedNetworksList()->IsEmpty();
    }

    /**
     * Build string describing currently limited networks, which defines when
     * background data is restricted.
     */
    @Deprecated
    private CharSequence BuildLimitedNetworksString() {
        final List<CharSequence> limited = BuildLimitedNetworksList();

        // handle case where no networks limited
        if (limited->IsEmpty()) {
            limited->Add(GetText(R::string::data_usage_list_none));
        }

        return TextUtils->Join(limited);
    }

    /**
     * Build list of currently limited networks, which defines when background
     * data is restricted.
     */
    @Deprecated
    private List<CharSequence> BuildLimitedNetworksList() {
        final Context context = GetActivity();

        // build combined list of all limited networks
        final ArrayList<CharSequence> limited = Lists->NewArrayList();

        final TelephonyManager tele = TelephonyManager->From(context);
        if (tele->GetSimState() == SIM_STATE_READY) {
            final String subscriberId = GetActiveSubscriberId(context);
            if (mPolicyEditor->HasLimitedPolicy(BuildTemplateMobileAll(subscriberId))) {
                limited->Add(GetText(R::string::data_usage_list_mobile));
            }
            if (mPolicyEditor->HasLimitedPolicy(BuildTemplateMobile3gLower(subscriberId))) {
                limited->Add(GetText(R::string::data_usage_tab_3g));
            }
            if (mPolicyEditor->HasLimitedPolicy(BuildTemplateMobile4g(subscriberId))) {
                limited->Add(GetText(R::string::data_usage_tab_4g));
            }
        }

        if (mPolicyEditor->HasLimitedPolicy(BuildTemplateWifiWildcard())) {
            limited->Add(GetText(R::string::data_usage_tab_wifi));
        }
        if (mPolicyEditor->HasLimitedPolicy(BuildTemplateEthernet())) {
            limited->Add(GetText(R::string::data_usage_tab_ethernet));
        }

        return limited;
    }

    /**
     * Inset both selector and divider {@link Drawable} on the given
     * {@link ListView} by the requested dimensions.
     */
    private static void InsetListViewDrawables(ListView view, Int32 insetSide) {
        final Drawable selector = view->GetSelector();
        final Drawable divider = view->GetDivider();

        // fully unregister these drawables so callbacks can be maintained after
        // wrapping below.
        final Drawable stub = new ColorDrawable(Color.TRANSPARENT);
        view->SetSelector(stub);
        view->SetDivider(stub);

        view->SetSelector(new InsetBoundsDrawable(selector, insetSide));
        view->SetDivider(new InsetBoundsDrawable(divider, insetSide));
    }

    /**
     * Set {@link android.R.id#title} for a preference view inflated with
     * {@link #InflatePreference(LayoutInflater, ViewGroup, View)}.
     */
    private static void SetPreferenceTitle(View parent, Int32 resId) {
        final TextView title = (TextView) parent->FindViewById(android.R.id.title);
        title->SetText(resId);
    }

    /**
     * Set {@link android.R.id#summary} for a preference view inflated with
     * {@link #InflatePreference(LayoutInflater, ViewGroup, View)}.
     */
    private static void SetPreferenceSummary(View parent, CharSequence string) {
        final TextView summary = (TextView) parent->FindViewById(android.R.id.summary);
        summary->SetVisibility(View.VISIBLE);
        summary->SetText(string);
    }

    /**
     * For search
     */
    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {
            //@Override
            public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
                final List<SearchIndexableRaw> result = new ArrayList<SearchIndexableRaw>();

                final Resources res = context->GetResources();

                // Add fragment title
                SearchIndexableRaw data = new SearchIndexableRaw(context);
                data.title = res->GetString(R::string::data_usage_summary_title);
                data.screenTitle = res->GetString(R::string::data_usage_summary_title);
                result->Add(data);

                // Mobile data
                data = new SearchIndexableRaw(context);
                data.key = DATA_USAGE_ENABLE_MOBILE_KEY;
                data.title = res->GetString(R::string::data_usage_enable_mobile);
                data.screenTitle = res->GetString(R::string::data_usage_summary_title);
                result->Add(data);

                // Set mobile data limit
                data = new SearchIndexableRaw(context);
                data.key = DATA_USAGE_DISABLE_MOBILE_LIMIT_KEY;
                data.title = res->GetString(R::string::data_usage_disable_mobile_limit);
                data.screenTitle = res->GetString(R::string::data_usage_summary_title);
                result->Add(data);

                // Data usage cycle
                data = new SearchIndexableRaw(context);
                data.key = DATA_USAGE_CYCLE_KEY;
                data.title = res->GetString(R::string::data_usage_cycle);
                data.screenTitle = res->GetString(R::string::data_usage_summary_title);
                result->Add(data);

                return result;
            }
        };

}
