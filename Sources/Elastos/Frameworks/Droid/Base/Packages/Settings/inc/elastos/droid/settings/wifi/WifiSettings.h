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

package com.android.settings.wifi;

using static android::Net::Wifi::WifiConfiguration::IINVALID_NETWORK_ID;
using static android::Os::UserManager::IDISALLOW_CONFIG_WIFI;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::ISharedPreferences;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::NetworkInfo::IDetailedState;
using Elastos::Droid::Net::INetworkScoreManager;
using Elastos::Droid::Net::INetworkScorerAppManager;
using Elastos::Droid::Net::NetworkScorerAppManager::INetworkScorerAppData;
using Elastos::Droid::Net::Wifi::IScanResult;
using Elastos::Droid::Net::Wifi::IWifiConfiguration;
using Elastos::Droid::Net::Wifi::IWifiInfo;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Net::Wifi::IWpsInfo;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IContextMenu;
using Elastos::Droid::View::ContextMenu::IContextMenuInfo;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::Widget::AdapterView::IAdapterContextMenuInfo;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IRestrictedSettingsFragment;
using Elastos::Droid::Settings::ISettingsActivity;
using Elastos::Droid::Settings::Search::IBaseSearchIndexProvider;
using Elastos::Droid::Settings::Search::IIndexable;
using Elastos::Droid::Settings::Search::ISearchIndexableRaw;

using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IHashMap;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

/**
 * Two types of UI are provided here.
 *
 * The first is for "usual Settings", appearing as any other Setup fragment.
 *
 * The second is for Setup Wizard, with a simplified interface that hides the action bar
 * and menus.
 */
public class WifiSettings extends RestrictedSettingsFragment
        implements DialogInterface.OnClickListener, Indexable  {

    private static const String TAG = "WifiSettings";

    private static const Int32 REQUEST_ENABLE_WIFI_ASSISTANT = 1;

    /* package */ static const Int32 MENU_ID_WPS_PBC = Menu.FIRST;
    private static const Int32 MENU_ID_WPS_PIN = Menu.FIRST + 1;
    private static const Int32 MENU_ID_SAVED_NETWORK = Menu.FIRST + 2;
    /* package */ static const Int32 MENU_ID_ADD_NETWORK = Menu.FIRST + 3;
    private static const Int32 MENU_ID_ADVANCED = Menu.FIRST + 4;
    private static const Int32 MENU_ID_SCAN = Menu.FIRST + 5;
    private static const Int32 MENU_ID_CONNECT = Menu.FIRST + 6;
    private static const Int32 MENU_ID_FORGET = Menu.FIRST + 7;
    private static const Int32 MENU_ID_MODIFY = Menu.FIRST + 8;
    private static const Int32 MENU_ID_WRITE_NFC = Menu.FIRST + 9;

    private static const String KEY_ASSISTANT_DISMISS_PLATFORM = "assistant_dismiss_platform";

    public static const Int32 WIFI_DIALOG_ID = 1;
    /* package */ static const Int32 WPS_PBC_DIALOG_ID = 2;
    private static const Int32 WPS_PIN_DIALOG_ID = 3;
    private static const Int32 WRITE_NFC_DIALOG_ID = 6;

    // Combo scans can take 5-6s to complete - set to 10s.
    private static const Int32 WIFI_RESCAN_INTERVAL_MS = 10 * 1000;

    // Instance state keys
    private static const String SAVE_DIALOG_EDIT_MODE = "edit_mode";
    private static const String SAVE_DIALOG_ACCESS_POINT_STATE = "wifi_ap_state";

    private static Boolean savedNetworksExist;

    private final IntentFilter mFilter;
    private final BroadcastReceiver mReceiver;
    private final Scanner mScanner;

    /* package */ WifiManager mWifiManager;
    private WifiManager.ActionListener mConnectListener;
    private WifiManager.ActionListener mSaveListener;
    private WifiManager.ActionListener mForgetListener;

    private WifiEnabler mWifiEnabler;
    // An access point being editted is stored here.
    private AccessPoint mSelectedAccessPoint;

    private DetailedState mLastState;
    private WifiInfo mLastInfo;

    private final AtomicBoolean mConnected = new AtomicBoolean(FALSE);

    private WifiDialog mDialog;
    private WriteWifiConfigToNfcDialog mWifiToNfcDialog;

    private TextView mEmptyView;

    // this Boolean extra specifies whether to disable the Next button when not connected. Used by
    // account creation outside of setup wizard.
    private static const String EXTRA_ENABLE_NEXT_ON_CONNECT = "wifi_enable_next_on_connect";

    // should Next button only be enabled when we have a connection?
    private Boolean mEnableNextOnConnection;

    // Save the dialog details
    private Boolean mDlgEdit;
    private AccessPoint mDlgAccessPoint;
    private Bundle mAccessPointSavedState;
    private View mWifiAssistantCard;
    private NetworkScorerAppData mWifiAssistantApp;

    /** verbose logging flag. this flag is set thru developer debugging options
     * and used so as to assist with in-the-field WiFi connectivity debugging  */
    public static Int32 mVerboseLogging = 0;

    /* End of "used in Wifi Setup context" */

    /** A restricted multimap for use in constructAccessPoints */
    private static class Multimap<K,V> {
        private final HashMap<K,List<V>> store = new HashMap<K,List<V>>();
        /** retrieve a non-NULL list of values with key K */
        List<V> GetAll(K key) {
            List<V> values = store->Get(key);
            return values != NULL ? values : Collections.<V>EmptyList();
        }

        void Put(K key, V val) {
            List<V> curVals = store->Get(key);
            if (curVals == NULL) {
                curVals = new ArrayList<V>(3);
                store->Put(key, curVals);
            }
            curVals->Add(val);
        }
    }

    private static class Scanner extends Handler {
        private Int32 mRetry = 0;
        private WifiSettings mWifiSettings = NULL;

        Scanner(WifiSettings wifiSettings) {
            mWifiSettings = wifiSettings;
        }

        void Resume() {
            if (!HasMessages(0)) {
                SendEmptyMessage(0);
            }
        }

        void ForceScan() {
            RemoveMessages(0);
            SendEmptyMessage(0);
        }

        void Pause() {
            mRetry = 0;
            RemoveMessages(0);
        }

        //@Override
        CARAPI HandleMessage(Message message) {
            if (mWifiSettings.mWifiManager->StartScan()) {
                mRetry = 0;
            } else if (++mRetry >= 3) {
                mRetry = 0;
                Activity activity = mWifiSettings->GetActivity();
                if (activity != NULL) {
                    Toast->MakeText(activity, R::string::wifi_fail_to_scan, Toast.LENGTH_LONG).Show();
                }
                return;
            }
            SendEmptyMessageDelayed(0, WIFI_RESCAN_INTERVAL_MS);
        }
    }

    public WifiSettings() {
        Super(DISALLOW_CONFIG_WIFI);
        mFilter = new IntentFilter();
        mFilter->AddAction(WifiManager.WIFI_STATE_CHANGED_ACTION);
        mFilter->AddAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
        mFilter->AddAction(WifiManager.NETWORK_IDS_CHANGED_ACTION);
        mFilter->AddAction(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION);
        mFilter->AddAction(WifiManager.CONFIGURED_NETWORKS_CHANGED_ACTION);
        mFilter->AddAction(WifiManager.LINK_CONFIGURATION_CHANGED_ACTION);
        mFilter->AddAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        mFilter->AddAction(WifiManager.RSSI_CHANGED_ACTION);

        mReceiver = new BroadcastReceiver() {
            //@Override
            CARAPI OnReceive(Context context, Intent intent) {
                HandleEvent(intent);
            }
        };

        mScanner = new Scanner(this);
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        mWifiManager = (WifiManager) GetSystemService(Context.WIFI_SERVICE);

        mConnectListener = new WifiManager->ActionListener() {
                                   //@Override
                                   CARAPI OnSuccess() {
                                   }
                                   //@Override
                                   CARAPI OnFailure(Int32 reason) {
                                       Activity activity = GetActivity();
                                       if (activity != NULL) {
                                           Toast->MakeText(activity,
                                                R::string::wifi_failed_connect_message,
                                                Toast.LENGTH_SHORT).Show();
                                       }
                                   }
                               };

        mSaveListener = new WifiManager->ActionListener() {
                                //@Override
                                CARAPI OnSuccess() {
                                }
                                //@Override
                                CARAPI OnFailure(Int32 reason) {
                                    Activity activity = GetActivity();
                                    if (activity != NULL) {
                                        Toast->MakeText(activity,
                                            R::string::wifi_failed_save_message,
                                            Toast.LENGTH_SHORT).Show();
                                    }
                                }
                            };

        mForgetListener = new WifiManager->ActionListener() {
                                   //@Override
                                   CARAPI OnSuccess() {
                                   }
                                   //@Override
                                   CARAPI OnFailure(Int32 reason) {
                                       Activity activity = GetActivity();
                                       if (activity != NULL) {
                                           Toast->MakeText(activity,
                                               R::string::wifi_failed_forget_message,
                                               Toast.LENGTH_SHORT).Show();
                                       }
                                   }
                               };

        if (savedInstanceState != NULL) {
            mDlgEdit = savedInstanceState->GetBoolean(SAVE_DIALOG_EDIT_MODE);
            if (savedInstanceState->ContainsKey(SAVE_DIALOG_ACCESS_POINT_STATE)) {
                mAccessPointSavedState =
                    savedInstanceState->GetBundle(SAVE_DIALOG_ACCESS_POINT_STATE);
            }
        }

        // if we're supposed to enable/disable the Next button based on our current connection
        // state, start it off in the right state
        Intent intent = GetActivity()->GetIntent();
        mEnableNextOnConnection = intent->GetBooleanExtra(EXTRA_ENABLE_NEXT_ON_CONNECT, FALSE);

        if (mEnableNextOnConnection) {
            if (HasNextButton()) {
                final ConnectivityManager connectivity = (ConnectivityManager)
                        GetActivity()->GetSystemService(Context.CONNECTIVITY_SERVICE);
                if (connectivity != NULL) {
                    NetworkInfo info = connectivity->GetNetworkInfo(
                            ConnectivityManager.TYPE_WIFI);
                    ChangeNextButtonState(info->IsConnected());
                }
            }
        }

        AddPreferencesFromResource(R.xml.wifi_settings);

        PrepareWifiAssistantCard();

        mEmptyView = InitEmptyView();
        RegisterForContextMenu(GetListView());
        SetHasOptionsMenu(TRUE);
    }

    //@Override
    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent resultData) {
        if (requestCode == REQUEST_ENABLE_WIFI_ASSISTANT) {
            if (resultCode == Activity.RESULT_OK) {
                DisableWifiAssistantCardUntilPlatformUpgrade();
                GetListView()->RemoveHeaderView(mWifiAssistantCard);
                mWifiAssistantApp = NULL;
            }
        } else {
            super->OnActivityResult(requestCode, resultCode, resultData);
        }
    }

    //@Override
    CARAPI OnDestroyView() {
        super->OnDestroyView();

        if (mWifiEnabler != NULL) {
            mWifiEnabler->TeardownSwitchBar();
        }
    }

    //@Override
    CARAPI OnStart() {
        super->OnStart();

        // On/off switch is hidden for Setup Wizard (returns NULL)
        mWifiEnabler = CreateWifiEnabler();
    }

    /**
     * @return new WifiEnabler or NULL (as overridden by WifiSettingsForSetupWizard)
     */
    /* package */ WifiEnabler CreateWifiEnabler() {
        final SettingsActivity activity = (SettingsActivity) GetActivity();
        return new WifiEnabler(activity, activity->GetSwitchBar());
    }

    //@Override
    CARAPI OnResume() {
        final Activity activity = GetActivity();
        super->OnResume();
        if (mWifiEnabler != NULL) {
            mWifiEnabler->Resume(activity);
        }

        activity->RegisterReceiver(mReceiver, mFilter);
        UpdateAccessPoints();
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        if (mWifiEnabler != NULL) {
            mWifiEnabler->Pause();
        }

        GetActivity()->UnregisterReceiver(mReceiver);
        mScanner->Pause();
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        // If the user is not allowed to configure wifi, do not show the menu.
        if (IsUiRestricted()) return;

        AddOptionsMenuItems(menu);
        super->OnCreateOptionsMenu(menu, inflater);
    }

    /**
     * @param menu
     */
    void AddOptionsMenuItems(Menu menu) {
        final Boolean wifiIsEnabled = mWifiManager->IsWifiEnabled();
        TypedArray ta = GetActivity()->GetTheme().ObtainStyledAttributes(
                new Int32[] {R.attr.ic_menu_add, R.attr.ic_wps});
        menu->Add(Menu.NONE, MENU_ID_ADD_NETWORK, 0, R::string::wifi_add_network)
                .SetIcon(ta->GetDrawable(0))
                .SetEnabled(wifiIsEnabled)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        if (savedNetworksExist) {
            menu->Add(Menu.NONE, MENU_ID_SAVED_NETWORK, 0, R::string::wifi_saved_access_points_label)
                    .SetIcon(ta->GetDrawable(0))
                    .SetEnabled(wifiIsEnabled)
                    .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        }
        menu->Add(Menu.NONE, MENU_ID_SCAN, 0, R::string::menu_stats_refresh)
               .SetEnabled(wifiIsEnabled)
               .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        menu->Add(Menu.NONE, MENU_ID_ADVANCED, 0, R::string::wifi_menu_advanced)
                .SetShowAsAction(MenuItem.SHOW_AS_ACTION_NEVER);
        ta->Recycle();
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        super->OnSaveInstanceState(outState);

        // If the dialog is showing, save its state.
        if (mDialog != NULL && mDialog->IsShowing()) {
            outState->PutBoolean(SAVE_DIALOG_EDIT_MODE, mDlgEdit);
            if (mDlgAccessPoint != NULL) {
                mAccessPointSavedState = new Bundle();
                mDlgAccessPoint->SaveWifiState(mAccessPointSavedState);
                outState->PutBundle(SAVE_DIALOG_ACCESS_POINT_STATE, mAccessPointSavedState);
            }
        }
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        // If the user is not allowed to configure wifi, do not handle menu selections.
        if (IsUiRestricted()) return FALSE;

        switch (item->GetItemId()) {
            case MENU_ID_WPS_PBC:
                ShowDialog(WPS_PBC_DIALOG_ID);
                return TRUE;
                /*
            case MENU_ID_P2P:
                if (GetActivity() instanceof SettingsActivity) {
                    ((SettingsActivity) GetActivity()).StartPreferencePanel(
                            WifiP2pSettings.class->GetCanonicalName(),
                            NULL,
                            R::string::wifi_p2p_settings_title, NULL,
                            this, 0);
                } else {
                    StartFragment(this, WifiP2pSettings.class->GetCanonicalName(),
                            R::string::wifi_p2p_settings_title, -1, NULL);
                }
                return TRUE;
                */
            case MENU_ID_WPS_PIN:
                ShowDialog(WPS_PIN_DIALOG_ID);
                return TRUE;
            case MENU_ID_SCAN:
                if (mWifiManager->IsWifiEnabled()) {
                    mScanner->ForceScan();
                }
                return TRUE;
            case MENU_ID_ADD_NETWORK:
                if (mWifiManager->IsWifiEnabled()) {
                    OnAddNetworkPressed();
                }
                return TRUE;
            case MENU_ID_SAVED_NETWORK:
                if (GetActivity() instanceof SettingsActivity) {
                    ((SettingsActivity) GetActivity()).StartPreferencePanel(
                            SavedAccessPointsWifiSettings.class->GetCanonicalName(), NULL,
                            R::string::wifi_saved_access_points_titlebar, NULL, this, 0);
                } else {
                    StartFragment(this, SavedAccessPointsWifiSettings.class->GetCanonicalName(),
                            R::string::wifi_saved_access_points_titlebar,
                            -1 /* Do not request a result */, NULL);
                }
                return TRUE;
            case MENU_ID_ADVANCED:
                if (GetActivity() instanceof SettingsActivity) {
                    ((SettingsActivity) GetActivity()).StartPreferencePanel(
                            AdvancedWifiSettings.class->GetCanonicalName(), NULL,
                            R::string::wifi_advanced_titlebar, NULL, this, 0);
                } else {
                    StartFragment(this, AdvancedWifiSettings.class->GetCanonicalName(),
                            R::string::wifi_advanced_titlebar, -1 /* Do not request a results */,
                            NULL);
                }
                return TRUE;
        }
        return super->OnOptionsItemSelected(item);
    }

    //@Override
    CARAPI OnCreateContextMenu(ContextMenu menu, View view, ContextMenuInfo info) {
        if (info instanceof AdapterContextMenuInfo) {
            Preference preference = (Preference) GetListView()->GetItemAtPosition(
                    ((AdapterContextMenuInfo) info).position);

            if (preference instanceof AccessPoint) {
                mSelectedAccessPoint = (AccessPoint) preference;
                menu->SetHeaderTitle(mSelectedAccessPoint.ssid);
                if (mSelectedAccessPoint->GetLevel() != -1
                        && mSelectedAccessPoint->GetState() == NULL) {
                    menu->Add(Menu.NONE, MENU_ID_CONNECT, 0, R::string::wifi_menu_connect);
                }
                if (mSelectedAccessPoint.networkId != INVALID_NETWORK_ID) {
                    if (ActivityManager->GetCurrentUser() == UserHandle.USER_OWNER) {
                        menu->Add(Menu.NONE, MENU_ID_FORGET, 0, R::string::wifi_menu_forget);
                    }
                    menu->Add(Menu.NONE, MENU_ID_MODIFY, 0, R::string::wifi_menu_modify);

                    if (mSelectedAccessPoint.security != AccessPoint.SECURITY_NONE) {
                        // Only allow writing of NFC tags for password-protected networks.
                        menu->Add(Menu.NONE, MENU_ID_WRITE_NFC, 0, R::string::wifi_menu_write_to_nfc);
                    }
                }
            }
        }
    }

    //@Override
    public Boolean OnContextItemSelected(MenuItem item) {
        if (mSelectedAccessPoint == NULL) {
            return super->OnContextItemSelected(item);
        }
        switch (item->GetItemId()) {
            case MENU_ID_CONNECT: {
                if (mSelectedAccessPoint.networkId != INVALID_NETWORK_ID) {
                    Connect(mSelectedAccessPoint.networkId);
                } else if (mSelectedAccessPoint.security == AccessPoint.SECURITY_NONE) {
                    /** Bypass dialog for unsecured networks */
                    mSelectedAccessPoint->GenerateOpenNetworkConfig();
                    Connect(mSelectedAccessPoint->GetConfig());
                } else {
                    ShowDialog(mSelectedAccessPoint, TRUE);
                }
                return TRUE;
            }
            case MENU_ID_FORGET: {
                mWifiManager->Forget(mSelectedAccessPoint.networkId, mForgetListener);
                return TRUE;
            }
            case MENU_ID_MODIFY: {
                ShowDialog(mSelectedAccessPoint, TRUE);
                return TRUE;
            }
            case MENU_ID_WRITE_NFC:
                ShowDialog(WRITE_NFC_DIALOG_ID);
                return TRUE;

        }
        return super->OnContextItemSelected(item);
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen screen, Preference preference) {
        if (preference instanceof AccessPoint) {
            mSelectedAccessPoint = (AccessPoint) preference;
            /** Bypass dialog for unsecured, unsaved networks */
            if (mSelectedAccessPoint.security == AccessPoint.SECURITY_NONE &&
                    mSelectedAccessPoint.networkId == INVALID_NETWORK_ID) {
                mSelectedAccessPoint->GenerateOpenNetworkConfig();
                if (!savedNetworksExist) {
                    savedNetworksExist = TRUE;
                    GetActivity()->InvalidateOptionsMenu();
                }
                Connect(mSelectedAccessPoint->GetConfig());
            } else {
                ShowDialog(mSelectedAccessPoint, FALSE);
            }
        } else {
            return super->OnPreferenceTreeClick(screen, preference);
        }
        return TRUE;
    }

    private void ShowDialog(AccessPoint accessPoint, Boolean edit) {
        if (mDialog != NULL) {
            RemoveDialog(WIFI_DIALOG_ID);
            mDialog = NULL;
        }

        // Save the access point and edit mode
        mDlgAccessPoint = accessPoint;
        mDlgEdit = edit;

        ShowDialog(WIFI_DIALOG_ID);
    }

    //@Override
    public Dialog OnCreateDialog(Int32 dialogId) {
        switch (dialogId) {
            case WIFI_DIALOG_ID:
                AccessPoint ap = mDlgAccessPoint; // For manual launch
                if (ap == NULL) { // For re-launch from saved state
                    if (mAccessPointSavedState != NULL) {
                        ap = new AccessPoint(GetActivity(), mAccessPointSavedState);
                        // For repeated orientation changes
                        mDlgAccessPoint = ap;
                        // Reset the saved access point data
                        mAccessPointSavedState = NULL;
                    }
                }
                // If it's NULL, fine, it's for Add Network
                mSelectedAccessPoint = ap;
                mDialog = new WifiDialog(GetActivity(), this, ap, mDlgEdit);
                return mDialog;
            case WPS_PBC_DIALOG_ID:
                return new WpsDialog(GetActivity(), WpsInfo.PBC);
            case WPS_PIN_DIALOG_ID:
                return new WpsDialog(GetActivity(), WpsInfo.DISPLAY);
            case WRITE_NFC_DIALOG_ID:
                if (mSelectedAccessPoint != NULL) {
                    mWifiToNfcDialog = new WriteWifiConfigToNfcDialog(
                            GetActivity(), mSelectedAccessPoint, mWifiManager);
                    return mWifiToNfcDialog;
                }

        }
        return super->OnCreateDialog(dialogId);
    }

    /**
     * Shows the latest access points available with supplemental information like
     * the strength of network and the security for it.
     */
    private void UpdateAccessPoints() {
        // Safeguard from some delayed event handling
        if (GetActivity() == NULL) return;

        if (IsUiRestricted()) {
            AddMessagePreference(R::string::wifi_empty_list_user_restricted);
            return;
        }
        final Int32 wifiState = mWifiManager->GetWifiState();

        //when we update the screen, check if verbose logging has been turned on or off
        mVerboseLogging = mWifiManager->GetVerboseLoggingLevel();

        switch (wifiState) {
            case WifiManager.WIFI_STATE_ENABLED:
                // AccessPoints are automatically sorted with TreeSet.
                final Collection<AccessPoint> accessPoints =
                        ConstructAccessPoints(GetActivity(), mWifiManager, mLastInfo, mLastState);
                GetPreferenceScreen()->RemoveAll();
                if (accessPoints->Size() == 0) {
                    AddMessagePreference(R::string::wifi_empty_list_wifi_on);
                }

                GetListView()->RemoveHeaderView(mWifiAssistantCard);
                if (mWifiAssistantApp != NULL) {
                    GetListView()->AddHeaderView(mWifiAssistantCard);
                }

                for (AccessPoint accessPoint : accessPoints) {
                    // Ignore access points that are out of range.
                    if (accessPoint->GetLevel() != -1) {
                        GetPreferenceScreen()->AddPreference(accessPoint);
                    }
                }
                break;

            case WifiManager.WIFI_STATE_ENABLING:
                GetPreferenceScreen()->RemoveAll();
                break;

            case WifiManager.WIFI_STATE_DISABLING:
                AddMessagePreference(R::string::wifi_stopping);
                break;

            case WifiManager.WIFI_STATE_DISABLED:
                SetOffMessage();
                break;
        }
    }

    /**
     * Returns the Network Scorer for the Wifi Assistant App.
     */
    public static NetworkScorerAppData GetWifiAssistantApp(Context context) {
        Collection<NetworkScorerAppData> scorers =
                NetworkScorerAppManager->GetAllValidScorers(context);

        if (scorers->IsEmpty()) {
            return NULL;
        }

        // TODO: b/13780935 - Implement proper scorer selection. Rather than pick the first
        // scorer on the system, we should allow the user to select one.
        return scorers->Iterator()->Next();
    }

    private void PrepareWifiAssistantCard() {
        if (GetActivity() instanceof WifiPickerActivity) {
            return;
        }

        if (NetworkScorerAppManager->GetActiveScorer(GetActivity()) != NULL) {
            // A scorer is already enabled; don't show the card.
            return;
        }

        Collection<NetworkScorerAppData> scorers =
                NetworkScorerAppManager->GetAllValidScorers(GetActivity());
        if (scorers->IsEmpty()) {
            // No scorers are available to enable; don't show the card.
            return;
        }

        SharedPreferences sharedPreferences = GetPreferenceScreen()->GetSharedPreferences();
        Int32 lastDismissPlatform = sharedPreferences->GetInt(KEY_ASSISTANT_DISMISS_PLATFORM, 0);

        if (Build.VERSION.SDK_INT <= lastDismissPlatform) {
            // User has dismissed the Wi-Fi assistant card on this SDK release. Suppress the card
            // until the next major platform upgrade.
            return;
        }

        // TODO: b/13780935 - Implement proper scorer selection. Rather than pick the first
        // scorer on the system, we should allow the user to select one.
        mWifiAssistantApp = scorers->Iterator()->Next();

        if (mWifiAssistantCard == NULL) {
            mWifiAssistantCard = LayoutInflater->From(GetActivity())
                    .Inflate(R.layout.wifi_assistant_card, GetListView(), FALSE);
            Button setup = (Button) mWifiAssistantCard->FindViewById(R.id.setup);
            Button noThanks = (Button) mWifiAssistantCard->FindViewById(R.id.no_thanks_button);
            TextView assistantText =
                (TextView) mWifiAssistantCard->FindViewById(R.id.wifi_assistant_text);
            assistantText->SetText(GetResources()->GetString(
                    R::string::wifi_assistant_title_message, mWifiAssistantApp.mScorerName));

            if (setup != NULL && noThanks != NULL) {
                setup->SetOnClickListener(new OnClickListener() {
                    //@Override
                    CARAPI OnClick(View v) {
                        Intent intent = new Intent();
                        if (mWifiAssistantApp.mConfigurationActivityClassName != NULL) {
                            // App has a custom configuration activity; launch that.
                            // This custom activity will be responsible for launching the system
                            // dialog.
                            intent->SetClassName(mWifiAssistantApp.mPackageName,
                                    mWifiAssistantApp.mConfigurationActivityClassName);
                        } else {
                            // Fall back on the system dialog.
                            intent->SetAction(NetworkScoreManager.ACTION_CHANGE_ACTIVE);
                            intent->PutExtra(NetworkScoreManager.EXTRA_PACKAGE_NAME,
                                    mWifiAssistantApp.mPackageName);
                        }
                        StartActivityForResult(intent, REQUEST_ENABLE_WIFI_ASSISTANT);
                    }
                });

                noThanks->SetOnClickListener(new OnClickListener() {
                    //@Override
                    CARAPI OnClick(View v) {
                        DisableWifiAssistantCardUntilPlatformUpgrade();
                        GetListView()->RemoveHeaderView(mWifiAssistantCard);
                        mWifiAssistantApp = NULL;
                    }
                });
            }
        }
    }

    private void DisableWifiAssistantCardUntilPlatformUpgrade() {
        SharedPreferences sharedPreferences = GetPreferenceScreen()->GetSharedPreferences();
        SharedPreferences.Editor editor = sharedPreferences->Edit();
        editor->PutInt(KEY_ASSISTANT_DISMISS_PLATFORM, Build.VERSION.SDK_INT);
        editor->Apply();
    }

    protected TextView InitEmptyView() {
        TextView emptyView = (TextView) GetActivity()->FindViewById(android.R.id.empty);
        GetListView()->SetEmptyView(emptyView);
        return emptyView;
    }

    private void SetOffMessage() {
        if (mEmptyView != NULL) {
            mEmptyView->SetText(R::string::wifi_empty_list_wifi_off);
            if (android.provider.Settings::Global::>GetInt(GetActivity()->GetContentResolver(),
                    android.provider.Settings::Global::WIFI_SCAN_ALWAYS_AVAILABLE, 0) == 1) {
                mEmptyView->Append("\n\n");
                Int32 resId;
                if (android.provider.Settings.Secure->IsLocationProviderEnabled(
                        GetActivity()->GetContentResolver(), LocationManager.NETWORK_PROVIDER)) {
                    resId = R::string::wifi_scan_notify_text_location_on;
                } else {
                    resId = R::string::wifi_scan_notify_text_location_off;
                }
                CharSequence charSeq = GetText(resId);
                mEmptyView->Append(charSeq);
            }
        }
        GetPreferenceScreen()->RemoveAll();
    }

    private void AddMessagePreference(Int32 messageId) {
        if (mEmptyView != NULL) mEmptyView->SetText(messageId);
        GetPreferenceScreen()->RemoveAll();
    }

    /** Returns sorted list of access points */
    private static List<AccessPoint> ConstructAccessPoints(Context context,
            WifiManager wifiManager, WifiInfo lastInfo, DetailedState lastState) {
        ArrayList<AccessPoint> accessPoints = new ArrayList<AccessPoint>();
        /** Lookup table to more quickly update AccessPoints by only considering objects with the
         * correct SSID.  Maps SSID -> List of AccessPoints with the given SSID.  */
        Multimap<String, AccessPoint> apMap = new Multimap<String, AccessPoint>();

        final List<WifiConfiguration> configs = wifiManager->GetConfiguredNetworks();
        if (configs != NULL) {
            // Update "Saved Networks" menu option.
            if (savedNetworksExist != (configs->Size() > 0)) {
                savedNetworksExist = !savedNetworksExist;
                if (context instanceof Activity) {
                    ((Activity) context).InvalidateOptionsMenu();
                }
            }
            for (WifiConfiguration config : configs) {
                if (config.selfAdded && config.numAssociation == 0) {
                    continue;
                }
                AccessPoint accessPoint = new AccessPoint(context, config);
                if (lastInfo != NULL && lastState != NULL) {
                    accessPoint->Update(lastInfo, lastState);
                }
                accessPoints->Add(accessPoint);
                apMap->Put(accessPoint.ssid, accessPoint);
            }
        }

        final List<ScanResult> results = wifiManager->GetScanResults();
        if (results != NULL) {
            for (ScanResult result : results) {
                // Ignore hidden and ad-hoc networks.
                if (result.SSID == NULL || result.SSID->Length() == 0 ||
                        result.capabilities->Contains("[IBSS]")) {
                    continue;
                }

                Boolean found = FALSE;
                for (AccessPoint accessPoint : apMap->GetAll(result.SSID)) {
                    if (accessPoint->Update(result))
                        found = TRUE;
                }
                if (!found) {
                    AccessPoint accessPoint = new AccessPoint(context, result);
                    accessPoints->Add(accessPoint);
                    apMap->Put(accessPoint.ssid, accessPoint);
                }
            }
        }

        // Pre-sort accessPoints to speed preference insertion
        Collections->Sort(accessPoints);
        return accessPoints;
    }

    private void HandleEvent(Intent intent) {
        String action = intent->GetAction();
        if (WifiManager.WIFI_STATE_CHANGED_ACTION->Equals(action)) {
            UpdateWifiState(intent->GetIntExtra(WifiManager.EXTRA_WIFI_STATE,
                    WifiManager.WIFI_STATE_UNKNOWN));
        } else if (WifiManager.SCAN_RESULTS_AVAILABLE_ACTION->Equals(action) ||
                WifiManager.CONFIGURED_NETWORKS_CHANGED_ACTION->Equals(action) ||
                WifiManager.LINK_CONFIGURATION_CHANGED_ACTION->Equals(action)) {
                UpdateAccessPoints();
        } else if (WifiManager.NETWORK_STATE_CHANGED_ACTION->Equals(action)) {
            NetworkInfo info = (NetworkInfo) intent->GetParcelableExtra(
                    WifiManager.EXTRA_NETWORK_INFO);
            mConnected->Set(info->IsConnected());
            ChangeNextButtonState(info->IsConnected());
            UpdateAccessPoints();
            UpdateConnectionState(info->GetDetailedState());
        } else if (WifiManager.RSSI_CHANGED_ACTION->Equals(action)) {
            UpdateConnectionState(NULL);
        }
    }

    private void UpdateConnectionState(DetailedState state) {
        /* sticky broadcasts can call this when wifi is disabled */
        if (!mWifiManager->IsWifiEnabled()) {
            mScanner->Pause();
            return;
        }

        if (state == DetailedState.OBTAINING_IPADDR) {
            mScanner->Pause();
        } else {
            mScanner->Resume();
        }

        mLastInfo = mWifiManager->GetConnectionInfo();
        if (state != NULL) {
            mLastState = state;
        }

        for (Int32 i = GetPreferenceScreen()->GetPreferenceCount() - 1; i >= 0; --i) {
            // Maybe there's a WifiConfigPreference
            Preference preference = GetPreferenceScreen()->GetPreference(i);
            if (preference instanceof AccessPoint) {
                final AccessPoint accessPoint = (AccessPoint) preference;
                accessPoint->Update(mLastInfo, mLastState);
            }
        }
    }

    private void UpdateWifiState(Int32 state) {
        Activity activity = GetActivity();
        if (activity != NULL) {
            activity->InvalidateOptionsMenu();
        }

        switch (state) {
            case WifiManager.WIFI_STATE_ENABLED:
                mScanner->Resume();
                return; // not break, to avoid the call to Pause() below

            case WifiManager.WIFI_STATE_ENABLING:
                AddMessagePreference(R::string::wifi_starting);
                break;

            case WifiManager.WIFI_STATE_DISABLED:
                SetOffMessage();
                break;
        }

        mLastInfo = NULL;
        mLastState = NULL;
        mScanner->Pause();
    }

    /**
     * Renames/replaces "Next" button when appropriate. "Next" button usually exists in
     * Wifi setup screens, not in usual wifi settings screen.
     *
     * @param enabled TRUE when the device is connected to a wifi network.
     */
    private void ChangeNextButtonState(Boolean enabled) {
        if (mEnableNextOnConnection && HasNextButton()) {
            GetNextButton()->SetEnabled(enabled);
        }
    }

    //@Override
    CARAPI OnClick(DialogInterface dialogInterface, Int32 button) {
        if (button == WifiDialog.BUTTON_FORGET && mSelectedAccessPoint != NULL) {
            Forget();
        } else if (button == WifiDialog.BUTTON_SUBMIT) {
            if (mDialog != NULL) {
                Submit(mDialog->GetController());
            }
        }
    }

    /* package */ void Submit(WifiConfigController configController) {

        final WifiConfiguration config = configController->GetConfig();

        if (config == NULL) {
            if (mSelectedAccessPoint != NULL
                    && mSelectedAccessPoint.networkId != INVALID_NETWORK_ID) {
                Connect(mSelectedAccessPoint.networkId);
            }
        } else if (config.networkId != INVALID_NETWORK_ID) {
            if (mSelectedAccessPoint != NULL) {
                mWifiManager->Save(config, mSaveListener);
            }
        } else {
            if (configController->IsEdit()) {
                mWifiManager->Save(config, mSaveListener);
            } else {
                Connect(config);
            }
        }

        if (mWifiManager->IsWifiEnabled()) {
            mScanner->Resume();
        }
        UpdateAccessPoints();
    }

    /* package */ void Forget() {
        if (mSelectedAccessPoint.networkId == INVALID_NETWORK_ID) {
            // Should not happen, but a monkey seems to trigger it
            Logger::E(TAG, "Failed to forget invalid network " + mSelectedAccessPoint->GetConfig());
            return;
        }

        mWifiManager->Forget(mSelectedAccessPoint.networkId, mForgetListener);

        if (mWifiManager->IsWifiEnabled()) {
            mScanner->Resume();
        }
        UpdateAccessPoints();

        // We need to rename/replace "Next" button in wifi setup context.
        ChangeNextButtonState(FALSE);
    }

    protected void Connect(final WifiConfiguration config) {
        mWifiManager->Connect(config, mConnectListener);
    }

    protected void Connect(final Int32 networkId) {
        mWifiManager->Connect(networkId, mConnectListener);
    }

    /**
     * Refreshes acccess points and ask Wifi module to scan networks again.
     */
    /* package */ void RefreshAccessPoints() {
        if (mWifiManager->IsWifiEnabled()) {
            mScanner->Resume();
        }

        GetPreferenceScreen()->RemoveAll();
    }

    /**
     * Called when "add network" button is pressed.
     */
    /* package */ void OnAddNetworkPressed() {
        // No exact access point is selected.
        mSelectedAccessPoint = NULL;
        ShowDialog(NULL, TRUE);
    }

    /* package */ Int32 GetAccessPointsCount() {
        final Boolean wifiIsEnabled = mWifiManager->IsWifiEnabled();
        if (wifiIsEnabled) {
            return GetPreferenceScreen()->GetPreferenceCount();
        } else {
            return 0;
        }
    }

    /**
     * Requests wifi module to pause wifi scan. May be ignored when the module is disabled.
     */
    /* package */ void PauseWifiScan() {
        if (mWifiManager->IsWifiEnabled()) {
            mScanner->Pause();
        }
    }

    /**
     * Requests wifi module to resume wifi scan. May be ignored when the module is disabled.
     */
    /* package */ void ResumeWifiScan() {
        if (mWifiManager->IsWifiEnabled()) {
            mScanner->Resume();
        }
    }

    //@Override
    protected Int32 GetHelpResource() {
        return R::string::help_url_wifi;
    }

    public static const SearchIndexProvider SEARCH_INDEX_DATA_PROVIDER =
        new BaseSearchIndexProvider() {
            //@Override
            public List<SearchIndexableRaw> GetRawDataToIndex(Context context, Boolean enabled) {
                final List<SearchIndexableRaw> result = new ArrayList<SearchIndexableRaw>();
                final Resources res = context->GetResources();

                // Add fragment title
                SearchIndexableRaw data = new SearchIndexableRaw(context);
                data.title = res->GetString(R::string::wifi_settings);
                data.screenTitle = res->GetString(R::string::wifi_settings);
                data.keywords = res->GetString(R::string::keywords_wifi);
                result->Add(data);

                // Add available Wi-Fi access points
                WifiManager wifiManager =
                        (WifiManager) context->GetSystemService(Context.WIFI_SERVICE);
                final Collection<AccessPoint> accessPoints =
                        ConstructAccessPoints(context, wifiManager, NULL, NULL);
                for (AccessPoint accessPoint : accessPoints) {
                    // We are indexing only the saved Wi-Fi networks.
                    if (accessPoint->GetConfig() == NULL) continue;
                    data = new SearchIndexableRaw(context);
                    data.title = accessPoint->GetTitle()->ToString();
                    data.screenTitle = res->GetString(R::string::wifi_settings);
                    data.enabled = enabled;
                    result->Add(data);
                }

                return result;
            }
        };
}
