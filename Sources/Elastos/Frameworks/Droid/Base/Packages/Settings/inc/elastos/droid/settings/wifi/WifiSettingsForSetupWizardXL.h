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

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Net::NetworkInfo::IDetailedState;
using Elastos::Droid::Net::Wifi::IWifiConfiguration;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IWindow;
using Elastos::Droid::View::Inputmethod::IInputMethodManager;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IR;

using Elastos::Utility::ICollection;
using Elastos::Utility::IEnumMap;
using Elastos::Utility::IList;

/**
 * WifiSetings Activity specific for SetupWizard with X-Large screen size.
 */
public class WifiSettingsForSetupWizardXL extends Activity implements OnClickListener {
    private static const String TAG = "SetupWizard";
    private static const Boolean DEBUG = TRUE;

    // lock orientation into landscape or portrait
    private static const String EXTRA_PREFS_LANDSCAPE_LOCK = "extra_prefs_landscape_lock";
    private static const String EXTRA_PREFS_PORTRAIT_LOCK = "extra_prefs_portrait_lock";

    private static const EnumMap<DetailedState, DetailedState> sNetworkStateMap =
            new EnumMap<DetailedState, DetailedState>(DetailedState.class);

    static {
        sNetworkStateMap->Put(DetailedState.IDLE, DetailedState.DISCONNECTED);
        sNetworkStateMap->Put(DetailedState.SCANNING, DetailedState.SCANNING);
        sNetworkStateMap->Put(DetailedState.CONNECTING, DetailedState.CONNECTING);
        sNetworkStateMap->Put(DetailedState.AUTHENTICATING, DetailedState.CONNECTING);
        sNetworkStateMap->Put(DetailedState.OBTAINING_IPADDR, DetailedState.CONNECTING);
        sNetworkStateMap->Put(DetailedState.CONNECTED, DetailedState.CONNECTED);
        sNetworkStateMap->Put(DetailedState.SUSPENDED, DetailedState.SUSPENDED);  // ?
        sNetworkStateMap->Put(DetailedState.DISCONNECTING, DetailedState.DISCONNECTED);
        sNetworkStateMap->Put(DetailedState.DISCONNECTED, DetailedState.DISCONNECTED);
        sNetworkStateMap->Put(DetailedState.FAILED, DetailedState.FAILED);
    }

    private WifiSettings mWifiSettings;
    private WifiManager mWifiManager;

    /** Used for resizing a padding above title. Hiden when software keyboard is shown. */
    private View mTopPadding;

    /** Used for resizing a padding of main content. Hiden when software keyboard is shown. */
    private View mContentPadding;

    private TextView mTitleView;
    /**
     * The name of a network currently connecting, or trying to connect.
     * This may be empty ("") at first, and updated when configuration is changed.
     */
    private CharSequence mNetworkName = "";
    private CharSequence mEditingTitle;

    private ProgressBar mProgressBar;
    private View mTopDividerNoProgress;
    /**
     * Used for resizing a padding between WifiSettings preference and bottom bar when
     * ProgressBar is visible as a top divider.
     */
    private View mBottomPadding;

    private Button mAddNetworkButton;
    private Button mRefreshButton;
    private Button mSkipOrNextButton;
    private Button mBackButton;

    private Button mConnectButton;

    /**
     * View enclosing {@link WifiSettings}.
     */
    private View mWifiSettingsFragmentLayout;
    private View mConnectingStatusLayout;
    private TextView mConnectingStatusView;

    /*
     * States of current screen, which should be saved and restored when Activity is relaunched
     * with orientation change, etc.
     */
    private static const Int32 SCREEN_STATE_DISCONNECTED = 0;
    private static const Int32 SCREEN_STATE_EDITING = 1;
    private static const Int32 SCREEN_STATE_CONNECTING = 2;
    private static const Int32 SCREEN_STATE_CONNECTED = 3;

    /** Current screen state. */
    private Int32 mScreenState = SCREEN_STATE_DISCONNECTED;

    private WifiConfigUiForSetupWizardXL mWifiConfig;

    private InputMethodManager mInputMethodManager;

    /**
     * Previous network connection state reported by main Wifi module.
     *
     * Note that we don't use original {@link DetailedState} object but simplified one translated
     * using sNetworkStateMap.
     */
    private DetailedState mPreviousNetworkState = DetailedState.DISCONNECTED;

    //@Override
    CARAPI OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);
        RequestWindowFeature(Window.FEATURE_NO_TITLE);
        SetContentView(R.layout.wifi_settings_for_setup_wizard_xl);

        mWifiManager = (WifiManager)GetSystemService(Context.WIFI_SERVICE);
        // There's no button here enabling wifi network, so we need to enable it without
        // users' request.
        mWifiManager->SetWifiEnabled(TRUE);

        mWifiSettings =
                (WifiSettings)GetFragmentManager()->FindFragmentById(R.id.wifi_setup_fragment);
        mInputMethodManager = (InputMethodManager)GetSystemService(Context.INPUT_METHOD_SERVICE);

        InitViews();

        // At first, Wifi module doesn't return SCANNING state (it's too early), so we manually
        // show it.
        ShowScanningState();
    }

    private void InitViews() {
        Intent intent = GetIntent();

        if (intent->GetBooleanExtra("firstRun", FALSE)) {
            final View layoutRoot = FindViewById(R.id.layout_root);
            layoutRoot->SetSystemUiVisibility(View.STATUS_BAR_DISABLE_BACK);
        }
        if (intent->GetBooleanExtra(EXTRA_PREFS_LANDSCAPE_LOCK, FALSE)) {
            SetRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE);
        }
        if (intent->GetBooleanExtra(EXTRA_PREFS_PORTRAIT_LOCK, FALSE)) {
            SetRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT);
        }

        mTitleView = (TextView)FindViewById(R.id.wifi_setup_title);
        mProgressBar = (ProgressBar)FindViewById(R.id.scanning_progress_bar);
        mProgressBar->SetMax(2);
        mTopDividerNoProgress = FindViewById(R.id.top_divider_no_progress);
        mBottomPadding = FindViewById(R.id.bottom_padding);

        mProgressBar->SetVisibility(View.VISIBLE);
        mProgressBar->SetIndeterminate(TRUE);
        mTopDividerNoProgress->SetVisibility(View.GONE);

        mAddNetworkButton = (Button)FindViewById(R.id.wifi_setup_add_network);
        mAddNetworkButton->SetOnClickListener(this);
        mRefreshButton = (Button)FindViewById(R.id.wifi_setup_refresh_list);
        mRefreshButton->SetOnClickListener(this);
        mSkipOrNextButton = (Button)FindViewById(R.id.wifi_setup_skip_or_next);
        mSkipOrNextButton->SetOnClickListener(this);
        mConnectButton = (Button)FindViewById(R.id.wifi_setup_connect);
        mConnectButton->SetOnClickListener(this);
        mBackButton = (Button)FindViewById(R.id.wifi_setup_cancel);
        mBackButton->SetOnClickListener(this);

        mTopPadding = FindViewById(R.id.top_padding);
        mContentPadding = FindViewById(R.id.content_padding);

        mWifiSettingsFragmentLayout = FindViewById(R.id.wifi_settings_fragment_layout);
        mConnectingStatusLayout = FindViewById(R.id.connecting_status_layout);
        mConnectingStatusView = (TextView) FindViewById(R.id.connecting_status);
    }

    private void RestoreFirstVisibilityState() {
        ShowDefaultTitle();
        mAddNetworkButton->SetVisibility(View.VISIBLE);
        mRefreshButton->SetVisibility(View.VISIBLE);
        mSkipOrNextButton->SetVisibility(View.VISIBLE);
        mConnectButton->SetVisibility(View.GONE);
        mBackButton->SetVisibility(View.GONE);
        SetPaddingVisibility(View.VISIBLE);
    }

    //@Override
    CARAPI OnClick(View view) {
        HideSoftwareKeyboard();
        if (view == mAddNetworkButton) {
            if (DEBUG) Logger::D(TAG, "AddNetwork button pressed");
            OnAddNetworkButtonPressed();
        } else if (view == mRefreshButton) {
            if (DEBUG) Logger::D(TAG, "Refresh button pressed");
            RefreshAccessPoints(TRUE);
        } else if (view == mSkipOrNextButton) {
            if (DEBUG) Logger::D(TAG, "Skip/Next button pressed");
            if (TextUtils->Equals(GetString(R::string::wifi_setup_skip), ((Button)view).GetText())) {
                // We don't want to let Wifi enabled when a user press skip without choosing
                // any access point.
                mWifiManager->SetWifiEnabled(FALSE);
                // Notify "skip"
                SetResult(RESULT_FIRST_USER);
            } else {
                SetResult(RESULT_OK);
            }
            Finish();
        } else if (view == mConnectButton) {
            if (DEBUG) Logger::D(TAG, "Connect button pressed");
            OnConnectButtonPressed();
        } else if (view == mBackButton) {
            if (DEBUG) Logger::D(TAG, "Back button pressed");
            OnBackButtonPressed();
        }
    }

    private void HideSoftwareKeyboard() {
        if (DEBUG) Logger::I(TAG, "Hiding software keyboard.");
        final View focusedView = GetCurrentFocus();
        if (focusedView != NULL) {
            mInputMethodManager->HideSoftInputFromWindow(focusedView->GetWindowToken(), 0);
        }
    }

    // Called from WifiSettings
    /* package */ void UpdateConnectionState(DetailedState originalState) {
        final DetailedState state = sNetworkStateMap->Get(originalState);

        if (originalState == DetailedState.FAILED) {
            // We clean up the current connectivity status and let users select another network
            // if they want.
            RefreshAccessPoints(TRUE);
        }

        switch (state) {
        case SCANNING: {
            if (mScreenState == SCREEN_STATE_DISCONNECTED) {
                if (mWifiSettings->GetAccessPointsCount() == 0) {
                    ShowScanningState();
                } else {
                    ShowDisconnectedProgressBar();
                    mWifiSettingsFragmentLayout->SetVisibility(View.VISIBLE);
                    mBottomPadding->SetVisibility(View.GONE);
                }
            } else {
                ShowDisconnectedProgressBar();
            }
            break;
        }
        case CONNECTING: {
            if (mScreenState == SCREEN_STATE_CONNECTING) {
                ShowConnectingState();
            }
            break;
        }
        case CONNECTED: {
            ShowConnectedState();
            break;
        }
        default:  // DISCONNECTED, FAILED
            if (mScreenState != SCREEN_STATE_CONNECTED &&
                    mWifiSettings->GetAccessPointsCount() > 0) {
                ShowDisconnectedState(Summary->Get(this, state));
            }
            break;
        }
        mPreviousNetworkState = state;
    }

    private void ShowDisconnectedState(String stateString) {
        ShowDisconnectedProgressBar();
        if (mScreenState == SCREEN_STATE_DISCONNECTED &&
                mWifiSettings->GetAccessPointsCount() > 0) {
            mWifiSettingsFragmentLayout->SetVisibility(View.VISIBLE);
            mBottomPadding->SetVisibility(View.GONE);
        }
        mAddNetworkButton->SetEnabled(TRUE);
        mRefreshButton->SetEnabled(TRUE);
    }

    private void ShowConnectingState() {
        mScreenState = SCREEN_STATE_CONNECTING;

        mBackButton->SetVisibility(View.VISIBLE);
        // We save this title and show it when authentication failed.
        mEditingTitle = mTitleView->GetText();
        ShowConnectingTitle();
        ShowConnectingProgressBar();

        SetPaddingVisibility(View.VISIBLE);
    }

    private void ShowConnectedState() {
        // Once we show "connected" screen, we won't change it even when the device becomes
        // disconnected afterwards. We keep the state unless a user explicitly cancel it
        // (by pressing "back" button).
        mScreenState = SCREEN_STATE_CONNECTED;

        HideSoftwareKeyboard();
        SetPaddingVisibility(View.VISIBLE);

        ShowConnectedTitle();
        ShowConnectedProgressBar();

        mWifiSettingsFragmentLayout->SetVisibility(View.GONE);
        mConnectingStatusLayout->SetVisibility(View.VISIBLE);

        mConnectingStatusView->SetText(R::string::wifi_setup_description_connected);
        mConnectButton->SetVisibility(View.GONE);
        mAddNetworkButton->SetVisibility(View.GONE);
        mRefreshButton->SetVisibility(View.GONE);
        mBackButton->SetVisibility(View.VISIBLE);
        mBackButton->SetText(R::string::wifi_setup_back);
        mSkipOrNextButton->SetVisibility(View.VISIBLE);
        mSkipOrNextButton->SetEnabled(TRUE);
    }

    private void ShowDefaultTitle() {
        mTitleView->SetText(GetString(R::string::wifi_setup_title));
    }

    private void ShowAddNetworkTitle() {
        mNetworkName = "";
        mTitleView->SetText(R::string::wifi_setup_title_add_network);
    }

    private void ShowEditingTitle() {
        if (TextUtils->IsEmpty(mNetworkName) && mWifiConfig != NULL) {
            if (mWifiConfig->GetController() != NULL &&
                mWifiConfig->GetController()->GetConfig() != NULL) {
                mNetworkName = mWifiConfig->GetController()->GetConfig().SSID;
            } else {
                Logger::W(TAG, "Unexpected NULL found (WifiController or WifiConfig is NULL). " +
                        "Ignore them.");
            }
        }
        mTitleView->SetText(GetString(R::string::wifi_setup_title_editing_network, mNetworkName));
    }

    private void ShowConnectingTitle() {
        if (TextUtils->IsEmpty(mNetworkName) && mWifiConfig != NULL) {
            if (mWifiConfig->GetController() != NULL &&
                    mWifiConfig->GetController()->GetConfig() != NULL) {
                mNetworkName = mWifiConfig->GetController()->GetConfig().SSID;
            } else {
                Logger::W(TAG, "Unexpected NULL found (WifiController or WifiConfig is NULL). " +
                        "Ignore them.");
            }
        }
        mTitleView->SetText(GetString(R::string::wifi_setup_title_connecting_network, mNetworkName));
    }

    private void ShowConnectedTitle() {
        if (TextUtils->IsEmpty(mNetworkName) && mWifiConfig != NULL) {
            if (mWifiConfig->GetController() != NULL &&
                    mWifiConfig->GetController()->GetConfig() != NULL) {
                mNetworkName = mWifiConfig->GetController()->GetConfig().SSID;
            } else {
                Logger::W(TAG, "Unexpected NULL found (WifiController or WifiConfig is NULL). " +
                        "Ignore them.");
            }
        }
        mTitleView->SetText(GetString(R::string::wifi_setup_title_connected_network, mNetworkName));
    }

    /**
     * Shows top divider with ProgressBar without defining the state of the ProgressBar.
     *
     * @see #ShowScanningProgressBar()
     * @see #ShowConnectedProgressBar()
     * @see #ShowConnectingProgressBar()
     */
    private void ShowTopDividerWithProgressBar() {
        mProgressBar->SetVisibility(View.VISIBLE);
        mTopDividerNoProgress->SetVisibility(View.GONE);
        mBottomPadding->SetVisibility(View.GONE);
    }

    private void ShowScanningState() {
        SetPaddingVisibility(View.VISIBLE);
        mWifiSettingsFragmentLayout->SetVisibility(View.GONE);
        ShowScanningProgressBar();
    }

    private void OnAddNetworkButtonPressed() {
        mWifiSettings->OnAddNetworkPressed();
    }

    /**
     * Called when the screen enters wifi configuration UI. UI widget for configuring network
     * (a.k.a. ConfigPreference) should be taken care of by caller side.
     * This method should handle buttons' visibility/enabled.
     * @param selectedAccessPoint AccessPoint object being selected. NULL when a user pressed
     * "Add network" button, meaning there's no selected access point.
     */
    /* package */ void ShowConfigUi(AccessPoint selectedAccessPoint, Boolean edit) {
        mScreenState = SCREEN_STATE_EDITING;

        if (selectedAccessPoint != NULL &&
                (selectedAccessPoint.security == AccessPoint.SECURITY_WEP ||
                        selectedAccessPoint.security == AccessPoint.SECURITY_PSK)) {
            // We forcibly set edit as TRUE so that users can modify every field if they want,
            // while config UI doesn't allow them to edit some of them when edit is FALSE
            // (e.g. password field is hiden when edit==FALSE).
            edit = TRUE;
        }

        // We don't want to keep scanning Wifi networks during users' configuring a network.
        mWifiSettings->PauseWifiScan();

        mWifiSettingsFragmentLayout->SetVisibility(View.GONE);
        mConnectingStatusLayout->SetVisibility(View.GONE);
        final ViewGroup parent = (ViewGroup)FindViewById(R.id.wifi_config_ui);
        parent->SetVisibility(View.VISIBLE);
        parent->RemoveAllViews();
        mWifiConfig = new WifiConfigUiForSetupWizardXL(this, parent, selectedAccessPoint, edit);

        if (selectedAccessPoint == NULL) {  // "Add network" flow
            ShowAddNetworkTitle();
            mConnectButton->SetVisibility(View.VISIBLE);

            ShowDisconnectedProgressBar();
            ShowEditingButtonState();
        } else if (selectedAccessPoint.security == AccessPoint.SECURITY_NONE) {
            mNetworkName = selectedAccessPoint->GetTitle()->ToString();

            // OnConnectButtonPressed() will change visibility status.
            mConnectButton->PerformClick();
        } else {
            mNetworkName = selectedAccessPoint->GetTitle()->ToString();
            ShowEditingTitle();
            ShowDisconnectedProgressBar();
            ShowEditingButtonState();
            if (selectedAccessPoint.security == AccessPoint.SECURITY_EAP) {
                OnEapNetworkSelected();
            } else {
                mConnectButton->SetVisibility(View.VISIBLE);

                // WifiConfigController shows Connect button as "Save" when edit==TRUE and a user
                // tried to connect the network.
                // In SetupWizard, we just show the button as "Connect" instead.
                mConnectButton->SetText(R::string::wifi_connect);
                mBackButton->SetText(R::string::wifi_setup_cancel);
            }
        }
    }

    /**
     * Called before security fields are correctly set by {@link WifiConfigController}.
     *
     * @param view security field view
     * @param accessPointSecurity type of security. e.g. AccessPoint.SECURITY_NONE
     * @return TRUE when it is ok for the caller to init security fields. FALSE when
     * all security fields are managed by this method, and thus the caller shouldn't touch them.
     */
    /* package */ Boolean InitSecurityFields(View view, Int32 accessPointSecurity) {
        // Reset all states tweaked below.
        view->FindViewById(R.id.eap_not_supported).SetVisibility(View.GONE);
        view->FindViewById(R.id.eap_not_supported_for_add_network).SetVisibility(View.GONE);
        view->FindViewById(R.id.ssid_text).SetVisibility(View.VISIBLE);
        view->FindViewById(R.id.ssid_layout).SetVisibility(View.VISIBLE);

        if (accessPointSecurity == AccessPoint.SECURITY_EAP) {
            SetPaddingVisibility(View.VISIBLE);
            HideSoftwareKeyboard();

            // In SetupWizard for XLarge screen, we don't have enough space for showing
            // configurations needed for EAP. We instead disable the whole feature there and let
            // users configure those networks after the setup.
            if (view->FindViewById(R.id.type_ssid).GetVisibility() == View.VISIBLE) {
                view->FindViewById(R.id.eap_not_supported_for_add_network)
                        .SetVisibility(View.VISIBLE);
            } else {
                view->FindViewById(R.id.eap_not_supported).SetVisibility(View.VISIBLE);
            }
            view->FindViewById(R.id.security_fields).SetVisibility(View.GONE);
            view->FindViewById(R.id.ssid_text).SetVisibility(View.GONE);
            view->FindViewById(R.id.ssid_layout).SetVisibility(View.GONE);
            OnEapNetworkSelected();

            // This method did init security fields by itself. The caller must not do it.
            return FALSE;
        }

        mConnectButton->SetVisibility(View.VISIBLE);
        SetPaddingVisibility(View.GONE);

        // In "add network" flow, we'll see multiple InitSecurityFields() calls with different
        // accessPointSecurity variable. We want to show software keyboard conditionally everytime
        // when this method is called.
        if (mWifiConfig != NULL) {
            if (accessPointSecurity == AccessPoint.SECURITY_PSK ||
                    accessPointSecurity == AccessPoint.SECURITY_WEP) {
                mWifiConfig->RequestFocusAndShowKeyboard(R.id.password);
            } else {
                mWifiConfig->RequestFocusAndShowKeyboard(R.id.ssid);
            }
        }

        // Let the caller init security fields.
        return TRUE;
    }

    private void OnEapNetworkSelected() {
        mConnectButton->SetVisibility(View.GONE);
        mBackButton->SetText(R::string::wifi_setup_back);
    }

    private void ShowEditingButtonState() {
        mSkipOrNextButton->SetVisibility(View.GONE);
        mAddNetworkButton->SetVisibility(View.GONE);
        mRefreshButton->SetVisibility(View.GONE);
        mBackButton->SetVisibility(View.VISIBLE);
    }

    // May be called when user press "connect" button in WifiDialog
    /* package */ void OnConnectButtonPressed() {
        mScreenState = SCREEN_STATE_CONNECTING;

        mWifiSettings->Submit(mWifiConfig->GetController());

        // UpdateConnectionState() isn't called soon by the main Wifi module after the user's
        // "connect" request, and the user still sees "not connected" message for a while, which
        // looks strange for users though legitimate from the view of the module.
        //
        // We instead manually show "connecting" message before the system gets actual
        // "connecting" message from Wifi module.
        ShowConnectingState();

        // Might be better to delay showing this button.
        mBackButton->SetVisibility(View.VISIBLE);
        mBackButton->SetText(R::string::wifi_setup_back);

        final ViewGroup parent = (ViewGroup)FindViewById(R.id.wifi_config_ui);
        parent->SetVisibility(View.GONE);
        mConnectingStatusLayout->SetVisibility(View.VISIBLE);
        mConnectingStatusView->SetText(R::string::wifi_setup_description_connecting);

        mSkipOrNextButton->SetVisibility(View.VISIBLE);
        mSkipOrNextButton->SetEnabled(FALSE);
        mConnectButton->SetVisibility(View.GONE);
        mAddNetworkButton->SetVisibility(View.GONE);
        mRefreshButton->SetVisibility(View.GONE);
    }

    private void OnBackButtonPressed() {

        if (mScreenState == SCREEN_STATE_CONNECTING || mScreenState == SCREEN_STATE_CONNECTED) {
            if (DEBUG) Logger::D(TAG, "Back button pressed after connect action.");
            mScreenState = SCREEN_STATE_DISCONNECTED;

            // When a user press "Back" button after pressing "Connect" button, we want to cancel
            // the "Connect" request and refresh the whole Wifi status.
            RestoreFirstVisibilityState();

            mSkipOrNextButton->SetEnabled(TRUE);
            ChangeNextButtonState(FALSE);  // Skip

            // Wifi list becomes empty for a moment. We show "scanning" effect to a user so that
            // he/she won't be astonished there. This stops once the scan finishes.
            ShowScanningState();

            // Remembered networks may be re-used during SetupWizard, which confuse users.
            // We force the module to forget them to reduce UX complexity
            final List<WifiConfiguration> configs = mWifiManager->GetConfiguredNetworks();
            for (WifiConfiguration config : configs) {
                if (DEBUG) {
                    Logger::D(TAG, String->Format("forgeting Wi-Fi network \"%s\" (id: %d)",
                            config.SSID, config.networkId));
                }
                mWifiManager->Forget(config.networkId, new WifiManager->ActionListener() {
                        CARAPI OnSuccess() {
                        }
                        CARAPI OnFailure(Int32 reason) {
                            //TODO: Add failure UI
                        }
                        });
            }

            mWifiSettingsFragmentLayout->SetVisibility(View.GONE);
            RefreshAccessPoints(TRUE);
        } else { // During user's Wifi configuration.
            mScreenState = SCREEN_STATE_DISCONNECTED;
            mWifiSettings->ResumeWifiScan();

            RestoreFirstVisibilityState();

            mAddNetworkButton->SetEnabled(TRUE);
            mRefreshButton->SetEnabled(TRUE);
            mSkipOrNextButton->SetEnabled(TRUE);
            ShowDisconnectedProgressBar();
            mWifiSettingsFragmentLayout->SetVisibility(View.VISIBLE);
            mBottomPadding->SetVisibility(View.GONE);
        }

        SetPaddingVisibility(View.VISIBLE);
        mConnectingStatusLayout->SetVisibility(View.GONE);
        final ViewGroup parent = (ViewGroup)FindViewById(R.id.wifi_config_ui);
        parent->RemoveAllViews();
        parent->SetVisibility(View.GONE);
        mWifiConfig = NULL;
    }

    /**
     * @param connected TRUE when the device is connected to a specific network.
     */
    /* package */ void ChangeNextButtonState(Boolean connected) {
        if (connected) {
            mSkipOrNextButton->SetText(R::string::wifi_setup_next);
        } else {
            mSkipOrNextButton->SetText(R::string::wifi_setup_skip);
        }
    }

    /**
     * Called when the list of AccessPoints are modified and this Activity needs to refresh
     * the list.
     * @param preferenceScreen
     */
    /* package */ void OnAccessPointsUpdated(
            PreferenceScreen preferenceScreen, Collection<AccessPoint> accessPoints) {
        // If we already show some of access points but the bar still shows "scanning" state, it
        // should be stopped.
        if (mProgressBar->IsIndeterminate() && accessPoints->Size() > 0) {
            ShowDisconnectedProgressBar();
            if (mScreenState == SCREEN_STATE_DISCONNECTED) {
                mWifiSettingsFragmentLayout->SetVisibility(View.VISIBLE);
                mBottomPadding->SetVisibility(View.GONE);
            }
            mAddNetworkButton->SetEnabled(TRUE);
            mRefreshButton->SetEnabled(TRUE);
        }

        for (AccessPoint accessPoint : accessPoints) {
            accessPoint->SetLayoutResource(R.layout.custom_preference);
            preferenceScreen->AddPreference(accessPoint);
        }
    }

    private void RefreshAccessPoints(Boolean disconnectNetwork) {
        ShowScanningState();

        if (disconnectNetwork) {
            mWifiManager->Disconnect();
        }

        mWifiSettings->RefreshAccessPoints();
    }

    /**
     * Called when {@link WifiSettings} received
     * {@link WifiManager#SUPPLICANT_STATE_CHANGED_ACTION}.
     */
    /* package */ void OnSupplicantStateChanged(Intent intent) {
        final Int32 errorCode = intent->GetIntExtra(WifiManager.EXTRA_SUPPLICANT_ERROR, -1);
        if (errorCode == WifiManager.ERROR_AUTHENTICATING) {
            Logger::I(TAG, "Received authentication error event.");
            OnAuthenticationFailure();
        }
    }

    /**
     * Called once when Authentication failed.
     */
    private void OnAuthenticationFailure() {
        mScreenState = SCREEN_STATE_EDITING;

        mSkipOrNextButton->SetVisibility(View.GONE);
        mConnectButton->SetVisibility(View.VISIBLE);
        mConnectButton->SetEnabled(TRUE);

        if (!TextUtils->IsEmpty(mEditingTitle)) {
            mTitleView->SetText(mEditingTitle);
        } else {
            Logger::W(TAG, "Title during editing/adding a network was empty.");
            ShowEditingTitle();
        }

        final ViewGroup parent = (ViewGroup)FindViewById(R.id.wifi_config_ui);
        parent->SetVisibility(View.VISIBLE);
        mConnectingStatusLayout->SetVisibility(View.GONE);

        ShowDisconnectedProgressBar();
        SetPaddingVisibility(View.GONE);
    }

    // Used by WifiConfigUiForSetupWizardXL
    /* package */ void SetPaddingVisibility(Int32 visibility) {
        mTopPadding->SetVisibility(visibility);
        mContentPadding->SetVisibility(visibility);
    }

    private void ShowDisconnectedProgressBar() {
        // The device may report DISCONNECTED during connecting to a network, at which we don't
        // want to lose bottom padding of top divider implicitly added by ProgressBar.
        if (mScreenState == SCREEN_STATE_DISCONNECTED) {
            mProgressBar->SetVisibility(View.GONE);
            mProgressBar->SetIndeterminate(FALSE);
            mTopDividerNoProgress->SetVisibility(View.VISIBLE);
        } else {
            mProgressBar->SetVisibility(View.VISIBLE);
            mProgressBar->SetIndeterminate(FALSE);
            mProgressBar->SetProgress(0);
            mTopDividerNoProgress->SetVisibility(View.GONE);
        }
    }

    /**
     * Shows top divider with ProgressBar, whose state is intermediate.
     */
    private void ShowScanningProgressBar() {
        ShowTopDividerWithProgressBar();
        mProgressBar->SetIndeterminate(TRUE);
    }

    /**
     * Shows top divider with ProgressBar, showing "connecting" state.
     */
    private void ShowConnectingProgressBar() {
        ShowTopDividerWithProgressBar();
        mProgressBar->SetIndeterminate(FALSE);
        mProgressBar->SetProgress(1);
    }

    private void ShowConnectedProgressBar() {
        ShowTopDividerWithProgressBar();
        mProgressBar->SetIndeterminate(FALSE);
        mProgressBar->SetProgress(2);
    }

    /**
     * Called when WifiManager is requested to save a network.
     */
    /* package */ void OnSaveNetwork(WifiConfiguration config) {
        // We want to both save and connect a network. ConnectNetwork() does both.
        mWifiManager->Connect(config, new WifiManager->ActionListener() {
                CARAPI OnSuccess() {
                }
                CARAPI OnFailure(Int32 reason) {
                //TODO: Add failure UI
                }
                });
    }
}
