/*
 * Copyright (C) 2012 The Android Open Source Project
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

package com.android.settings.wfd;

using Elastos::Droid::App::IActionBar;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IService;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Hardware::Display::IWifiDisplay;
using Elastos::Droid::Hardware::Display::IWifiDisplayStatus;
using Elastos::Droid::Media::IMediaRouter;
using Elastos::Droid::Media::MediaRouter::IRouteInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Net::Wifi::P2p::WifiP2pManager::IActionListener;
using Elastos::Droid::Net::Wifi::P2p::WifiP2pManager::IChannel;
using Elastos::Droid::Net::Wifi::IWpsInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Preference::ICheckBoxPreference;
using Elastos::Droid::Preference::IListPreference;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Provider::ISettings;
using Elastos::Droid::Utility::ISlog;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Internal::App::IMediaRouteDialogPresenter;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;

/**
 * The Settings screen for WifiDisplay configuration and connection management.
 *
 * The wifi display routes are integrated together with other remote display routes
 * from the media router.  It may happen that wifi display isn't actually available
 * on the system.  In that case, the enable option will not be shown but other
 * remote display routes will continue to be made available.
 */
public class WifiDisplaySettings extends SettingsPreferenceFragment {
    private static const String TAG = "WifiDisplaySettings";
    private static const Boolean DEBUG = FALSE;

    private static const Int32 MENU_ID_ENABLE_WIFI_DISPLAY = Menu.FIRST;

    private static const Int32 CHANGE_SETTINGS = 1 << 0;
    private static const Int32 CHANGE_ROUTES = 1 << 1;
    private static const Int32 CHANGE_WIFI_DISPLAY_STATUS = 1 << 2;
    private static const Int32 CHANGE_ALL = -1;

    private static const Int32 ORDER_CERTIFICATION = 1;
    private static const Int32 ORDER_CONNECTED = 2;
    private static const Int32 ORDER_AVAILABLE = 3;
    private static const Int32 ORDER_UNAVAILABLE = 4;

    private final Handler mHandler;

    private MediaRouter mRouter;
    private DisplayManager mDisplayManager;

    private Boolean mStarted;
    private Int32 mPendingChanges;

    private Boolean mWifiDisplayOnSetting;
    private WifiDisplayStatus mWifiDisplayStatus;

    private TextView mEmptyView;

    /* certification */
    private Boolean mWifiDisplayCertificationOn;
    private WifiP2pManager mWifiP2pManager;
    private Channel mWifiP2pChannel;
    private PreferenceGroup mCertCategory;
    private Boolean mListen;
    private Boolean mAutoGO;
    private Int32 mWpsConfig = WpsInfo.INVALID;
    private Int32 mListenChannel;
    private Int32 mOperatingChannel;

    public WifiDisplaySettings() {
        mHandler = new Handler();
    }

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        final Context context = GetActivity();
        mRouter = (MediaRouter)context->GetSystemService(Context.MEDIA_ROUTER_SERVICE);
        mDisplayManager = (DisplayManager)context->GetSystemService(Context.DISPLAY_SERVICE);
        mWifiP2pManager = (WifiP2pManager)context->GetSystemService(Context.WIFI_P2P_SERVICE);
        mWifiP2pChannel = mWifiP2pManager->Initialize(context, Looper->GetMainLooper(), NULL);

        AddPreferencesFromResource(R.xml.wifi_display_settings);
        SetHasOptionsMenu(TRUE);
    }

    //@Override
    protected Int32 GetHelpResource() {
        return R::string::help_url_remote_display;
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        super->OnActivityCreated(savedInstanceState);

        mEmptyView = (TextView) GetView()->FindViewById(android.R.id.empty);
        mEmptyView->SetText(R::string::wifi_display_no_devices_found);
        GetListView()->SetEmptyView(mEmptyView);
    }

    //@Override
    CARAPI OnStart() {
        super->OnStart();
        mStarted = TRUE;

        final Context context = GetActivity();
        IntentFilter filter = new IntentFilter();
        filter->AddAction(DisplayManager.ACTION_WIFI_DISPLAY_STATUS_CHANGED);
        context->RegisterReceiver(mReceiver, filter);

        GetContentResolver()->RegisterContentObserver(Settings::Global::>GetUriFor(
                Settings::Global::WIFI_DISPLAY_ON), FALSE, mSettingsObserver);
        GetContentResolver()->RegisterContentObserver(Settings::Global::>GetUriFor(
                Settings::Global::WIFI_DISPLAY_CERTIFICATION_ON), FALSE, mSettingsObserver);
        GetContentResolver()->RegisterContentObserver(Settings::Global::>GetUriFor(
                Settings::Global::WIFI_DISPLAY_WPS_CONFIG), FALSE, mSettingsObserver);

        mRouter->AddCallback(MediaRouter.ROUTE_TYPE_REMOTE_DISPLAY, mRouterCallback,
                MediaRouter.CALLBACK_FLAG_PERFORM_ACTIVE_SCAN);

        Update(CHANGE_ALL);
    }

    //@Override
    CARAPI OnStop() {
        super->OnStop();
        mStarted = FALSE;

        final Context context = GetActivity();
        context->UnregisterReceiver(mReceiver);

        GetContentResolver()->UnregisterContentObserver(mSettingsObserver);

        mRouter->RemoveCallback(mRouterCallback);

        UnscheduleUpdate();
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        if (mWifiDisplayStatus != NULL && mWifiDisplayStatus->GetFeatureState()
                != WifiDisplayStatus.FEATURE_STATE_UNAVAILABLE) {
            MenuItem item = menu->Add(Menu.NONE, MENU_ID_ENABLE_WIFI_DISPLAY, 0,
                    R::string::wifi_display_enable_menu_item);
            item->SetCheckable(TRUE);
            item->SetChecked(mWifiDisplayOnSetting);
        }
        super->OnCreateOptionsMenu(menu, inflater);
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
            case MENU_ID_ENABLE_WIFI_DISPLAY:
                mWifiDisplayOnSetting = !item->IsChecked();
                item->SetChecked(mWifiDisplayOnSetting);
                Settings::Global::>PutInt(GetContentResolver(),
                        Settings::Global::WIFI_DISPLAY_ON, mWifiDisplayOnSetting ? 1 : 0);
                return TRUE;
        }
        return super->OnOptionsItemSelected(item);
    }

    private void ScheduleUpdate(Int32 changes) {
        if (mStarted) {
            if (mPendingChanges == 0) {
                mHandler->Post(mUpdateRunnable);
            }
            mPendingChanges |= changes;
        }
    }

    private void UnscheduleUpdate() {
        if (mPendingChanges != 0) {
            mPendingChanges = 0;
            mHandler->RemoveCallbacks(mUpdateRunnable);
        }
    }

    private void Update(Int32 changes) {
        Boolean invalidateOptions = FALSE;

        // Update settings.
        if ((changes & CHANGE_SETTINGS) != 0) {
            mWifiDisplayOnSetting = Settings::Global::>GetInt(GetContentResolver(),
                    Settings::Global::WIFI_DISPLAY_ON, 0) != 0;
            mWifiDisplayCertificationOn = Settings::Global::>GetInt(GetContentResolver(),
                    Settings::Global::WIFI_DISPLAY_CERTIFICATION_ON, 0) != 0;
            mWpsConfig = Settings::Global::>GetInt(GetContentResolver(),
                Settings::Global::WIFI_DISPLAY_WPS_CONFIG, WpsInfo.INVALID);

            // The wifi display enabled setting may have changed.
            invalidateOptions = TRUE;
        }

        // Update wifi display state.
        if ((changes & CHANGE_WIFI_DISPLAY_STATUS) != 0) {
            mWifiDisplayStatus = mDisplayManager->GetWifiDisplayStatus();

            // The wifi display feature state may have changed.
            invalidateOptions = TRUE;
        }

        // Rebuild the routes.
        final PreferenceScreen preferenceScreen = GetPreferenceScreen();
        preferenceScreen->RemoveAll();

        // Add all known remote display routes.
        final Int32 routeCount = mRouter->GetRouteCount();
        for (Int32 i = 0; i < routeCount; i++) {
            MediaRouter.RouteInfo route = mRouter->GetRouteAt(i);
            if (route->MatchesTypes(MediaRouter.ROUTE_TYPE_REMOTE_DISPLAY)) {
                preferenceScreen->AddPreference(CreateRoutePreference(route));
            }
        }

        // Additional features for wifi display routes.
        if (mWifiDisplayStatus != NULL
                && mWifiDisplayStatus->GetFeatureState() == WifiDisplayStatus.FEATURE_STATE_ON) {
            // Add all unpaired wifi displays.
            for (WifiDisplay display : mWifiDisplayStatus->GetDisplays()) {
                if (!display->IsRemembered() && display->IsAvailable()
                        && !display->Equals(mWifiDisplayStatus->GetActiveDisplay())) {
                    preferenceScreen->AddPreference(new UnpairedWifiDisplayPreference(
                            GetActivity(), display));
                }
            }

            // Add the certification menu if enabled in developer options.
            if (mWifiDisplayCertificationOn) {
                BuildCertificationMenu(preferenceScreen);
            }
        }

        // Invalidate menu options if needed.
        if (invalidateOptions) {
            GetActivity()->InvalidateOptionsMenu();
        }
    }

    private RoutePreference CreateRoutePreference(MediaRouter.RouteInfo route) {
        WifiDisplay display = FindWifiDisplay(route->GetDeviceAddress());
        if (display != NULL) {
            return new WifiDisplayRoutePreference(GetActivity(), route, display);
        } else {
            return new RoutePreference(GetActivity(), route);
        }
    }

    private WifiDisplay FindWifiDisplay(String deviceAddress) {
        if (mWifiDisplayStatus != NULL && deviceAddress != NULL) {
            for (WifiDisplay display : mWifiDisplayStatus->GetDisplays()) {
                if (display->GetDeviceAddress()->Equals(deviceAddress)) {
                    return display;
                }
            }
        }
        return NULL;
    }

    private void BuildCertificationMenu(final PreferenceScreen preferenceScreen) {
        if (mCertCategory == NULL) {
            mCertCategory = new PreferenceCategory(GetActivity());
            mCertCategory->SetTitle(R::string::wifi_display_certification_heading);
            mCertCategory->SetOrder(ORDER_CERTIFICATION);
        } else {
            mCertCategory->RemoveAll();
        }
        preferenceScreen->AddPreference(mCertCategory);

        // display session info if there is an active p2p session
        if (!mWifiDisplayStatus->GetSessionInfo()->GetGroupId().IsEmpty()) {
            Preference p = new Preference(GetActivity());
            p->SetTitle(R::string::wifi_display_session_info);
            p->SetSummary(mWifiDisplayStatus->GetSessionInfo()->ToString());
            mCertCategory->AddPreference(p);

            // show buttons for Pause/Resume when a WFD session is established
            if (mWifiDisplayStatus->GetSessionInfo()->GetSessionId() != 0) {
                mCertCategory->AddPreference(new Preference(GetActivity()) {
                    //@Override
                    public View GetView(View convertView, ViewGroup parent) {
                        final View v;
                        if (convertView == NULL) {
                            LayoutInflater li = (LayoutInflater) GetActivity().
                                    GetSystemService(Service.LAYOUT_INFLATER_SERVICE);
                            v = li->Inflate(R.layout.two_buttons_panel, NULL);
                        } else {
                            v = convertView;
                        }

                        Button b = (Button)v->FindViewById(R.id.left_button);
                        b->SetText(R::string::wifi_display_pause);
                        b->SetOnClickListener(new OnClickListener() {
                            //@Override
                            CARAPI OnClick(View v) {
                                mDisplayManager->PauseWifiDisplay();
                            }
                        });

                        b = (Button)v->FindViewById(R.id.right_button);
                        b->SetText(R::string::wifi_display_resume);
                        b->SetOnClickListener(new OnClickListener() {
                            //@Override
                            CARAPI OnClick(View v) {
                                mDisplayManager->ResumeWifiDisplay();
                            }
                        });

                        return v;
                    }
                });
            }
        }

        // switch for Listen Mode
        CheckBoxPreference cbp = new CheckBoxPreference(GetActivity()) {
            //@Override
            protected void OnClick() {
                mListen = !mListen;
                SetListenMode(mListen);
                SetChecked(mListen);
            }
        };
        cbp->SetTitle(R::string::wifi_display_listen_mode);
        cbp->SetChecked(mListen);
        mCertCategory->AddPreference(cbp);

        // switch for Autonomous GO
        cbp = new CheckBoxPreference(GetActivity()) {
            //@Override
            protected void OnClick() {
                mAutoGO = !mAutoGO;
                if (mAutoGO) {
                    StartAutoGO();
                } else {
                    StopAutoGO();
                }
                SetChecked(mAutoGO);
            }
        };
        cbp->SetTitle(R::string::wifi_display_autonomous_go);
        cbp->SetChecked(mAutoGO);
        mCertCategory->AddPreference(cbp);

        // Drop down list for choosing WPS method (PBC/KEYPAD/DISPLAY)
        ListPreference lp = new ListPreference(GetActivity()) {
            //@Override
            protected void OnDialogClosed(Boolean positiveResult) {
                super->OnDialogClosed(positiveResult);
                if (positiveResult) {
                    mWpsConfig = Integer->ParseInt(GetValue());
                    SetSummary("%1$s");
                    GetActivity()->InvalidateOptionsMenu();
                    Settings::Global::>PutInt(GetActivity()->GetContentResolver(),
                            Settings::Global::WIFI_DISPLAY_WPS_CONFIG, mWpsConfig);
                }
            }
        };
        mWpsConfig = Settings::Global::>GetInt(GetActivity()->GetContentResolver(),
                Settings::Global::WIFI_DISPLAY_WPS_CONFIG, WpsInfo.INVALID);
        String[] wpsEntries = { "Default", "PBC", "KEYPAD", "DISPLAY" };
        String[] wpsValues = {
            "" + WpsInfo.INVALID,
            "" + WpsInfo.PBC,
            "" + WpsInfo.KEYPAD,
            "" + WpsInfo.DISPLAY };
        lp->SetTitle(R::string::wifi_display_wps_config);
        lp->SetEntries(wpsEntries);
        lp->SetEntryValues(wpsValues);
        lp->SetValue("" + mWpsConfig);
        lp->SetSummary("%1$s");
        mCertCategory->AddPreference(lp);

        // Drop down list for choosing listen channel
        lp = new ListPreference(GetActivity()) {
            //@Override
            protected void OnDialogClosed(Boolean positiveResult) {
                super->OnDialogClosed(positiveResult);
                if (positiveResult) {
                    mListenChannel = Integer->ParseInt(GetValue());
                    SetSummary("%1$s");
                    GetActivity()->InvalidateOptionsMenu();
                    SetWifiP2pChannels(mListenChannel, mOperatingChannel);
                }
            }
        };
        String[] lcEntries = { "Auto", "1", "6", "11" };
        String[] lcValues = { "0", "1", "6", "11" };
        lp->SetTitle(R::string::wifi_display_listen_channel);
        lp->SetEntries(lcEntries);
        lp->SetEntryValues(lcValues);
        lp->SetValue("" + mListenChannel);
        lp->SetSummary("%1$s");
        mCertCategory->AddPreference(lp);

        // Drop down list for choosing operating channel
        lp = new ListPreference(GetActivity()) {
            //@Override
            protected void OnDialogClosed(Boolean positiveResult) {
                super->OnDialogClosed(positiveResult);
                if (positiveResult) {
                    mOperatingChannel = Integer->ParseInt(GetValue());
                    SetSummary("%1$s");
                    GetActivity()->InvalidateOptionsMenu();
                    SetWifiP2pChannels(mListenChannel, mOperatingChannel);
                }
            }
        };
        String[] ocEntries = { "Auto", "1", "6", "11", "36" };
        String[] ocValues = { "0", "1", "6", "11", "36" };
        lp->SetTitle(R::string::wifi_display_operating_channel);
        lp->SetEntries(ocEntries);
        lp->SetEntryValues(ocValues);
        lp->SetValue("" + mOperatingChannel);
        lp->SetSummary("%1$s");
        mCertCategory->AddPreference(lp);
    }

    private void StartAutoGO() {
        if (DEBUG) {
            Slogger::D(TAG, "Starting Autonomous GO...");
        }
        mWifiP2pManager->CreateGroup(mWifiP2pChannel, new ActionListener() {
            //@Override
            CARAPI OnSuccess() {
                if (DEBUG) {
                    Slogger::D(TAG, "Successfully started AutoGO.");
                }
            }

            //@Override
            CARAPI OnFailure(Int32 reason) {
                Slogger::E(TAG, "Failed to start AutoGO with reason " + reason + ".");
            }
        });
    }

    private void StopAutoGO() {
        if (DEBUG) {
            Slogger::D(TAG, "Stopping Autonomous GO...");
        }
        mWifiP2pManager->RemoveGroup(mWifiP2pChannel, new ActionListener() {
            //@Override
            CARAPI OnSuccess() {
                if (DEBUG) {
                    Slogger::D(TAG, "Successfully stopped AutoGO.");
                }
            }

            //@Override
            CARAPI OnFailure(Int32 reason) {
                Slogger::E(TAG, "Failed to stop AutoGO with reason " + reason + ".");
            }
        });
    }

    private void SetListenMode(final Boolean enable) {
        if (DEBUG) {
            Slogger::D(TAG, "Setting listen mode to: " + enable);
        }
        mWifiP2pManager->Listen(mWifiP2pChannel, enable, new ActionListener() {
            //@Override
            CARAPI OnSuccess() {
                if (DEBUG) {
                    Slogger::D(TAG, "Successfully " + (enable ? "entered" : "exited")
                            +" listen mode.");
                }
            }

            //@Override
            CARAPI OnFailure(Int32 reason) {
                Slogger::E(TAG, "Failed to " + (enable ? "entered" : "exited")
                        +" listen mode with reason " + reason + ".");
            }
        });
    }

    private void SetWifiP2pChannels(final Int32 lc, final Int32 oc) {
        if (DEBUG) {
            Slogger::D(TAG, "Setting wifi p2p channel: lc=" + lc + ", oc=" + oc);
        }
        mWifiP2pManager->SetWifiP2pChannels(mWifiP2pChannel,
                lc, oc, new ActionListener() {
            //@Override
            CARAPI OnSuccess() {
                if (DEBUG) {
                    Slogger::D(TAG, "Successfully set wifi p2p channels.");
                }
            }

            //@Override
            CARAPI OnFailure(Int32 reason) {
                Slogger::E(TAG, "Failed to set wifi p2p channels with reason " + reason + ".");
            }
        });
    }

    private void ToggleRoute(MediaRouter.RouteInfo route) {
        if (route->IsSelected()) {
            MediaRouteDialogPresenter->ShowDialogFragment(GetActivity(),
                    MediaRouter.ROUTE_TYPE_REMOTE_DISPLAY, NULL);
        } else {
            route->Select();
        }
    }

    private void PairWifiDisplay(WifiDisplay display) {
        if (display->CanConnect()) {
            mDisplayManager->ConnectWifiDisplay(display->GetDeviceAddress());
        }
    }

    private void ShowWifiDisplayOptionsDialog(final WifiDisplay display) {
        View view = GetActivity()->GetLayoutInflater().Inflate(R.layout.wifi_display_options, NULL);
        final EditText nameEditText = (EditText)view->FindViewById(R.id.name);
        nameEditText->SetText(display->GetFriendlyDisplayName());

        DialogInterface.OnClickListener done = new DialogInterface->OnClickListener() {
            //@Override
            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                String name = nameEditText->GetText()->ToString().Trim();
                if (name->IsEmpty() || name->Equals(display->GetDeviceName())) {
                    name = NULL;
                }
                mDisplayManager->RenameWifiDisplay(display->GetDeviceAddress(), name);
            }
        };
        DialogInterface.OnClickListener forget = new DialogInterface->OnClickListener() {
            //@Override
            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                mDisplayManager->ForgetWifiDisplay(display->GetDeviceAddress());
            }
        };

        AlertDialog dialog = new AlertDialog->Builder(GetActivity())
                .SetCancelable(TRUE)
                .SetTitle(R::string::wifi_display_options_title)
                .SetView(view)
                .SetPositiveButton(R::string::wifi_display_options_done, done)
                .SetNegativeButton(R::string::wifi_display_options_forget, forget)
                .Create();
        dialog->Show();
    }

    private final Runnable mUpdateRunnable = new Runnable() {
        //@Override
        CARAPI Run() {
            final Int32 changes = mPendingChanges;
            mPendingChanges = 0;
            Update(changes);
        }
    };

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (action->Equals(DisplayManager.ACTION_WIFI_DISPLAY_STATUS_CHANGED)) {
                ScheduleUpdate(CHANGE_WIFI_DISPLAY_STATUS);
            }
        }
    };

    private final ContentObserver mSettingsObserver = new ContentObserver(new Handler()) {
        //@Override
        CARAPI OnChange(Boolean selfChange, Uri uri) {
            ScheduleUpdate(CHANGE_SETTINGS);
        }
    };

    private final MediaRouter.Callback mRouterCallback = new MediaRouter->SimpleCallback() {
        //@Override
        CARAPI OnRouteAdded(MediaRouter router, RouteInfo info) {
            ScheduleUpdate(CHANGE_ROUTES);
        }

        //@Override
        CARAPI OnRouteChanged(MediaRouter router, RouteInfo info) {
            ScheduleUpdate(CHANGE_ROUTES);
        }

        //@Override
        CARAPI OnRouteRemoved(MediaRouter router, RouteInfo info) {
            ScheduleUpdate(CHANGE_ROUTES);
        }

        //@Override
        CARAPI OnRouteSelected(MediaRouter router, Int32 type, RouteInfo info) {
            ScheduleUpdate(CHANGE_ROUTES);
        }

        //@Override
        CARAPI OnRouteUnselected(MediaRouter router, Int32 type, RouteInfo info) {
            ScheduleUpdate(CHANGE_ROUTES);
        }
    };

    private class RoutePreference extends Preference
            implements Preference.OnPreferenceClickListener {
        private final MediaRouter.RouteInfo mRoute;

        public RoutePreference(Context context, MediaRouter.RouteInfo route) {
            Super(context);

            mRoute = route;
            SetTitle(route->GetName());
            SetSummary(route->GetDescription());
            SetEnabled(route->IsEnabled());
            if (route->IsSelected()) {
                SetOrder(ORDER_CONNECTED);
                if (route->IsConnecting()) {
                    SetSummary(R::string::wifi_display_status_connecting);
                } else {
                    SetSummary(R::string::wifi_display_status_connected);
                }
            } else {
                if (IsEnabled()) {
                    SetOrder(ORDER_AVAILABLE);
                } else {
                    SetOrder(ORDER_UNAVAILABLE);
                    if (route->GetStatusCode() == MediaRouter.RouteInfo.STATUS_IN_USE) {
                        SetSummary(R::string::wifi_display_status_in_use);
                    } else {
                        SetSummary(R::string::wifi_display_status_not_available);
                    }
                }
            }
            SetOnPreferenceClickListener(this);
        }

        //@Override
        public Boolean OnPreferenceClick(Preference preference) {
            ToggleRoute(mRoute);
            return TRUE;
        }
    }

    private class WifiDisplayRoutePreference extends RoutePreference
            implements View.OnClickListener {
        private final WifiDisplay mDisplay;

        public WifiDisplayRoutePreference(Context context, MediaRouter.RouteInfo route,
                WifiDisplay display) {
            Super(context, route);

            mDisplay = display;
            SetWidgetLayoutResource(R.layout.wifi_display_preference);
        }

        //@Override
        protected void OnBindView(View view) {
            super->OnBindView(view);

            ImageView deviceDetails = (ImageView) view->FindViewById(R.id.deviceDetails);
            if (deviceDetails != NULL) {
                deviceDetails->SetOnClickListener(this);
                if (!IsEnabled()) {
                    TypedValue value = new TypedValue();
                    GetContext()->GetTheme().ResolveAttribute(android.R.attr.disabledAlpha,
                            value, TRUE);
                    deviceDetails->SetImageAlpha((Int32)(value->GetFloat() * 255));
                    deviceDetails->SetEnabled(TRUE); // always allow button to be pressed
                }
            }
        }

        //@Override
        CARAPI OnClick(View v) {
            ShowWifiDisplayOptionsDialog(mDisplay);
        }
    }

    private class UnpairedWifiDisplayPreference extends Preference
            implements Preference.OnPreferenceClickListener {
        private final WifiDisplay mDisplay;

        public UnpairedWifiDisplayPreference(Context context, WifiDisplay display) {
            Super(context);

            mDisplay = display;
            SetTitle(display->GetFriendlyDisplayName());
            SetSummary(R::string::wireless_display_route_description);
            SetEnabled(display->CanConnect());
            if (IsEnabled()) {
                SetOrder(ORDER_AVAILABLE);
            } else {
                SetOrder(ORDER_UNAVAILABLE);
                SetSummary(R::string::wifi_display_status_in_use);
            }
            SetOnPreferenceClickListener(this);
        }

        //@Override
        public Boolean OnPreferenceClick(Preference preference) {
            PairWifiDisplay(mDisplay);
            return TRUE;
        }
    }
}
