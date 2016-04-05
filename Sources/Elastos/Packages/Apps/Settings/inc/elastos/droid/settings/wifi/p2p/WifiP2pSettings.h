

package com.android.settings.wifi.p2p;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::DialogInterface::IOnClickListener;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::Wifi::P2p::IWifiP2pConfig;
using Elastos::Droid::Net::Wifi::P2p::IWifiP2pInfo;
using Elastos::Droid::Net::Wifi::P2p::IWifiP2pDevice;
using Elastos::Droid::Net::Wifi::P2p::IWifiP2pDeviceList;
using Elastos::Droid::Net::Wifi::P2p::IWifiP2pGroup;
using Elastos::Droid::Net::Wifi::P2p::IWifiP2pGroupList;
using Elastos::Droid::Net::Wifi::P2p::IWifiP2pManager;
using Elastos::Droid::Net::Wifi::P2p::WifiP2pManager::IPeerListListener;
using Elastos::Droid::Net::Wifi::P2p::WifiP2pManager::IPersistentGroupInfoListener;
using Elastos::Droid::Net::Wifi::IWpsInfo;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceCategory;
using Elastos::Droid::Preference::IPreferenceGroup;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Text::IInputFilter;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuInflater;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::ISettingsPreferenceFragment;

/*
 * Displays Wi-fi p2p settings UI
 */
public class WifiP2pSettings extends SettingsPreferenceFragment
        implements PersistentGroupInfoListener, PeerListListener {

    private static const String TAG = "WifiP2pSettings";
    private static const Boolean DBG = FALSE;
    private static const Int32 MENU_ID_SEARCH = Menu.FIRST;
    private static const Int32 MENU_ID_RENAME = Menu.FIRST + 1;

    private final IntentFilter mIntentFilter = new IntentFilter();
    private WifiP2pManager mWifiP2pManager;
    private WifiP2pManager.Channel mChannel;
    private OnClickListener mRenameListener;
    private OnClickListener mDisconnectListener;
    private OnClickListener mCancelConnectListener;
    private OnClickListener mDeleteGroupListener;
    private WifiP2pPeer mSelectedWifiPeer;
    private WifiP2pPersistentGroup mSelectedGroup;
    private String mSelectedGroupName;
    private EditText mDeviceNameText;

    private Boolean mWifiP2pEnabled;
    private Boolean mWifiP2pSearching;
    private Int32 mConnectedDevices;
    private Boolean mLastGroupFormed = FALSE;

    private PreferenceGroup mPeersGroup;
    private PreferenceGroup mPersistentGroup;
    private Preference mThisDevicePref;

    private static const Int32 DIALOG_DISCONNECT  = 1;
    private static const Int32 DIALOG_CANCEL_CONNECT = 2;
    private static const Int32 DIALOG_RENAME = 3;
    private static const Int32 DIALOG_DELETE_GROUP = 4;

    private static const String SAVE_DIALOG_PEER = "PEER_STATE";
    private static const String SAVE_DEVICE_NAME = "DEV_NAME";
    private static const String SAVE_SELECTED_GROUP = "GROUP_NAME";

    private WifiP2pDevice mThisDevice;
    private WifiP2pDeviceList mPeers = new WifiP2pDeviceList();

    private String mSavedDeviceName;

    private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();

            if (WifiP2pManager.WIFI_P2P_STATE_CHANGED_ACTION->Equals(action)) {
                mWifiP2pEnabled = intent->GetIntExtra(WifiP2pManager.EXTRA_WIFI_STATE,
                    WifiP2pManager.WIFI_P2P_STATE_DISABLED) == WifiP2pManager.WIFI_P2P_STATE_ENABLED;
                HandleP2pStateChanged();
            } else if (WifiP2pManager.WIFI_P2P_PEERS_CHANGED_ACTION->Equals(action)) {
                mPeers = (WifiP2pDeviceList) intent->GetParcelableExtra(
                        WifiP2pManager.EXTRA_P2P_DEVICE_LIST);
                HandlePeersChanged();
            } else if (WifiP2pManager.WIFI_P2P_CONNECTION_CHANGED_ACTION->Equals(action)) {
                if (mWifiP2pManager == NULL) return;
                NetworkInfo networkInfo = (NetworkInfo) intent->GetParcelableExtra(
                        WifiP2pManager.EXTRA_NETWORK_INFO);
                WifiP2pInfo wifip2pinfo = (WifiP2pInfo) intent->GetParcelableExtra(
                        WifiP2pManager.EXTRA_WIFI_P2P_INFO);
                if (networkInfo->IsConnected()) {
                    if (DBG) Logger::D(TAG, "Connected");
                } else if (mLastGroupFormed != TRUE) {
                    //start a search when we are disconnected
                    //but not on group removed broadcast event
                    StartSearch();
                }
                mLastGroupFormed = wifip2pinfo.groupFormed;
            } else if (WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION->Equals(action)) {
                mThisDevice = (WifiP2pDevice) intent->GetParcelableExtra(
                        WifiP2pManager.EXTRA_WIFI_P2P_DEVICE);
                if (DBG) Logger::D(TAG, "Update device info: " + mThisDevice);
                UpdateDevicePref();
            } else if (WifiP2pManager.WIFI_P2P_DISCOVERY_CHANGED_ACTION->Equals(action)) {
                Int32 discoveryState = intent->GetIntExtra(WifiP2pManager.EXTRA_DISCOVERY_STATE,
                    WifiP2pManager.WIFI_P2P_DISCOVERY_STOPPED);
                if (DBG) Logger::D(TAG, "Discovery state changed: " + discoveryState);
                if (discoveryState == WifiP2pManager.WIFI_P2P_DISCOVERY_STARTED) {
                    UpdateSearchMenu(TRUE);
                } else {
                    UpdateSearchMenu(FALSE);
                }
            } else if (WifiP2pManager.WIFI_P2P_PERSISTENT_GROUPS_CHANGED_ACTION->Equals(action)) {
                if (mWifiP2pManager != NULL) {
                    mWifiP2pManager->RequestPersistentGroupInfo(mChannel, WifiP2pSettings.this);
                }
            }
        }
    };

    public WifiP2pSettings() {
        if (DBG) Logger::D(TAG, "Creating WifiP2pSettings ...");
    }

    //@Override
    CARAPI OnActivityCreated(Bundle savedInstanceState) {
        AddPreferencesFromResource(R.xml.wifi_p2p_settings);

        mIntentFilter->AddAction(WifiP2pManager.WIFI_P2P_STATE_CHANGED_ACTION);
        mIntentFilter->AddAction(WifiP2pManager.WIFI_P2P_PEERS_CHANGED_ACTION);
        mIntentFilter->AddAction(WifiP2pManager.WIFI_P2P_CONNECTION_CHANGED_ACTION);
        mIntentFilter->AddAction(WifiP2pManager.WIFI_P2P_THIS_DEVICE_CHANGED_ACTION);
        mIntentFilter->AddAction(WifiP2pManager.WIFI_P2P_DISCOVERY_CHANGED_ACTION);
        mIntentFilter->AddAction(WifiP2pManager.WIFI_P2P_PERSISTENT_GROUPS_CHANGED_ACTION);

        final Activity activity = GetActivity();
        mWifiP2pManager = (WifiP2pManager) GetSystemService(Context.WIFI_P2P_SERVICE);
        if (mWifiP2pManager != NULL) {
            mChannel = mWifiP2pManager->Initialize(activity, GetActivity()->GetMainLooper(), NULL);
            if (mChannel == NULL) {
                //Failure to set up connection
                Logger::E(TAG, "Failed to set up connection with wifi p2p service");
                mWifiP2pManager = NULL;
            }
        } else {
            Logger::E(TAG, "mWifiP2pManager is NULL !");
        }

        if (savedInstanceState != NULL && savedInstanceState->ContainsKey(SAVE_DIALOG_PEER)) {
            WifiP2pDevice device = savedInstanceState->GetParcelable(SAVE_DIALOG_PEER);
            mSelectedWifiPeer = new WifiP2pPeer(GetActivity(), device);
        }
        if (savedInstanceState != NULL && savedInstanceState->ContainsKey(SAVE_DEVICE_NAME)) {
            mSavedDeviceName = savedInstanceState->GetString(SAVE_DEVICE_NAME);
        }
        if (savedInstanceState != NULL && savedInstanceState->ContainsKey(SAVE_SELECTED_GROUP)) {
            mSelectedGroupName = savedInstanceState->GetString(SAVE_SELECTED_GROUP);
        }

        mRenameListener = new OnClickListener() {
            //@Override
            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                if (which == DialogInterface.BUTTON_POSITIVE) {
                    if (mWifiP2pManager != NULL) {
                        mWifiP2pManager->SetDeviceName(mChannel,
                                mDeviceNameText->GetText()->ToString(),
                                new WifiP2pManager->ActionListener() {
                            CARAPI OnSuccess() {
                                if (DBG) Logger::D(TAG, " device rename success");
                            }
                            CARAPI OnFailure(Int32 reason) {
                                Toast->MakeText(GetActivity(),
                                        R::string::wifi_p2p_failed_rename_message,
                                        Toast.LENGTH_LONG).Show();
                            }
                        });
                    }
                }
            }
        };

        //disconnect dialog listener
        mDisconnectListener = new OnClickListener() {
            //@Override
            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                if (which == DialogInterface.BUTTON_POSITIVE) {
                    if (mWifiP2pManager != NULL) {
                        mWifiP2pManager->RemoveGroup(mChannel, new WifiP2pManager->ActionListener() {
                            CARAPI OnSuccess() {
                                if (DBG) Logger::D(TAG, " remove group success");
                            }
                            CARAPI OnFailure(Int32 reason) {
                                if (DBG) Logger::D(TAG, " remove group fail " + reason);
                            }
                        });
                    }
                }
            }
        };

        //cancel connect dialog listener
        mCancelConnectListener = new OnClickListener() {
            //@Override
            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                if (which == DialogInterface.BUTTON_POSITIVE) {
                    if (mWifiP2pManager != NULL) {
                        mWifiP2pManager->CancelConnect(mChannel,
                                new WifiP2pManager->ActionListener() {
                            CARAPI OnSuccess() {
                                if (DBG) Logger::D(TAG, " cancel connect success");
                            }
                            CARAPI OnFailure(Int32 reason) {
                                if (DBG) Logger::D(TAG, " cancel connect fail " + reason);
                            }
                        });
                    }
                }
            }
        };

        //delete persistent group dialog listener
        mDeleteGroupListener = new OnClickListener() {
            //@Override
            CARAPI OnClick(DialogInterface dialog, Int32 which) {
                if (which == DialogInterface.BUTTON_POSITIVE) {
                    if (mWifiP2pManager != NULL) {
                        if (mSelectedGroup != NULL) {
                            if (DBG) Logger::D(TAG, " deleting group " + mSelectedGroup->GetGroupName());
                            mWifiP2pManager->DeletePersistentGroup(mChannel,
                                    mSelectedGroup->GetNetworkId(),
                                    new WifiP2pManager->ActionListener() {
                                CARAPI OnSuccess() {
                                    if (DBG) Logger::D(TAG, " delete group success");
                                }
                                CARAPI OnFailure(Int32 reason) {
                                    if (DBG) Logger::D(TAG, " delete group fail " + reason);
                                }
                            });
                            mSelectedGroup = NULL;
                        } else {
                            if (DBG) Logger::W(TAG, " No selected group to delete!" );
                        }
                    }
                } else if (which == DialogInterface.BUTTON_NEGATIVE) {
                    if (DBG) {
                        Logger::D(TAG, " forgetting selected group " + mSelectedGroup->GetGroupName());
                    }
                    mSelectedGroup = NULL;
                }
            }
        };

        SetHasOptionsMenu(TRUE);

        final PreferenceScreen preferenceScreen = GetPreferenceScreen();
        preferenceScreen->RemoveAll();
        preferenceScreen->SetOrderingAsAdded(TRUE);

        mThisDevicePref = new Preference(GetActivity());
        mThisDevicePref->SetPersistent(FALSE);
        mThisDevicePref->SetSelectable(FALSE);
        preferenceScreen->AddPreference(mThisDevicePref);

        mPeersGroup = new PreferenceCategory(GetActivity());
        mPeersGroup->SetTitle(R::string::wifi_p2p_peer_devices);
        preferenceScreen->AddPreference(mPeersGroup);

        mPersistentGroup = new PreferenceCategory(GetActivity());
        mPersistentGroup->SetTitle(R::string::wifi_p2p_remembered_groups);
        preferenceScreen->AddPreference(mPersistentGroup);

        super->OnActivityCreated(savedInstanceState);
    }

    //@Override
    CARAPI OnResume() {
        super->OnResume();
        GetActivity()->RegisterReceiver(mReceiver, mIntentFilter);
        if (mWifiP2pManager != NULL) {
            mWifiP2pManager->RequestPeers(mChannel, WifiP2pSettings.this);
        }
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();
        if (mWifiP2pManager != NULL) {
            mWifiP2pManager->StopPeerDiscovery(mChannel, NULL);
        }
        GetActivity()->UnregisterReceiver(mReceiver);
    }

    //@Override
    CARAPI OnCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        Int32 textId = mWifiP2pSearching ? R::string::wifi_p2p_menu_searching :
                R::string::wifi_p2p_menu_search;
        menu->Add(Menu.NONE, MENU_ID_SEARCH, 0, textId)
            .SetEnabled(mWifiP2pEnabled)
            .SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        menu->Add(Menu.NONE, MENU_ID_RENAME, 0, R::string::wifi_p2p_menu_rename)
            .SetEnabled(mWifiP2pEnabled)
            .SetShowAsAction(MenuItem.SHOW_AS_ACTION_IF_ROOM);
        super->OnCreateOptionsMenu(menu, inflater);
    }

    //@Override
    CARAPI OnPrepareOptionsMenu(Menu menu) {
        MenuItem searchMenu = menu->FindItem(MENU_ID_SEARCH);
        MenuItem renameMenu = menu->FindItem(MENU_ID_RENAME);
        if (mWifiP2pEnabled) {
            searchMenu->SetEnabled(TRUE);
            renameMenu->SetEnabled(TRUE);
        } else {
            searchMenu->SetEnabled(FALSE);
            renameMenu->SetEnabled(FALSE);
        }

        if (mWifiP2pSearching) {
            searchMenu->SetTitle(R::string::wifi_p2p_menu_searching);
        } else {
            searchMenu->SetTitle(R::string::wifi_p2p_menu_search);
        }
    }

    //@Override
    public Boolean OnOptionsItemSelected(MenuItem item) {
        switch (item->GetItemId()) {
            case MENU_ID_SEARCH:
                StartSearch();
                return TRUE;
            case MENU_ID_RENAME:
                ShowDialog(DIALOG_RENAME);
                return TRUE;
        }
        return super->OnOptionsItemSelected(item);
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen screen, Preference preference) {
        if (preference instanceof WifiP2pPeer) {
            mSelectedWifiPeer = (WifiP2pPeer) preference;
            if (mSelectedWifiPeer.device.status == WifiP2pDevice.CONNECTED) {
                ShowDialog(DIALOG_DISCONNECT);
            } else if (mSelectedWifiPeer.device.status == WifiP2pDevice.INVITED) {
                ShowDialog(DIALOG_CANCEL_CONNECT);
            } else {
                WifiP2pConfig config = new WifiP2pConfig();
                config.deviceAddress = mSelectedWifiPeer.device.deviceAddress;

                Int32 forceWps = SystemProperties->GetInt("wifidirect.wps", -1);

                if (forceWps != -1) {
                    config.wps.setup = forceWps;
                } else {
                    if (mSelectedWifiPeer.device->WpsPbcSupported()) {
                        config.wps.setup = WpsInfo.PBC;
                    } else if (mSelectedWifiPeer.device->WpsKeypadSupported()) {
                        config.wps.setup = WpsInfo.KEYPAD;
                    } else {
                        config.wps.setup = WpsInfo.DISPLAY;
                    }
                }

                mWifiP2pManager->Connect(mChannel, config,
                        new WifiP2pManager->ActionListener() {
                            CARAPI OnSuccess() {
                                if (DBG) Logger::D(TAG, " connect success");
                            }
                            CARAPI OnFailure(Int32 reason) {
                                Logger::E(TAG, " connect fail " + reason);
                                Toast->MakeText(GetActivity(),
                                        R::string::wifi_p2p_failed_connect_message,
                                        Toast.LENGTH_SHORT).Show();
                            }
                    });
            }
        } else if (preference instanceof WifiP2pPersistentGroup) {
            mSelectedGroup = (WifiP2pPersistentGroup) preference;
            ShowDialog(DIALOG_DELETE_GROUP);
        }
        return super->OnPreferenceTreeClick(screen, preference);
    }

    //@Override
    public Dialog OnCreateDialog(Int32 id) {
        if (id == DIALOG_DISCONNECT) {
            String deviceName = TextUtils->IsEmpty(mSelectedWifiPeer.device.deviceName) ?
                    mSelectedWifiPeer.device.deviceAddress :
                    mSelectedWifiPeer.device.deviceName;
            String msg;
            if (mConnectedDevices > 1) {
                msg = GetActivity()->GetString(R::string::wifi_p2p_disconnect_multiple_message,
                        deviceName, mConnectedDevices - 1);
            } else {
                msg = GetActivity()->GetString(R::string::wifi_p2p_disconnect_message, deviceName);
            }
            AlertDialog dialog = new AlertDialog->Builder(GetActivity())
                .SetTitle(R::string::wifi_p2p_disconnect_title)
                .SetMessage(msg)
                .SetPositiveButton(GetActivity()->GetString(R::string::dlg_ok), mDisconnectListener)
                .SetNegativeButton(GetActivity()->GetString(R::string::dlg_cancel), NULL)
                .Create();
            return dialog;
        } else if (id == DIALOG_CANCEL_CONNECT) {
            Int32 stringId = R::string::wifi_p2p_cancel_connect_message;
            String deviceName = TextUtils->IsEmpty(mSelectedWifiPeer.device.deviceName) ?
                    mSelectedWifiPeer.device.deviceAddress :
                    mSelectedWifiPeer.device.deviceName;

            AlertDialog dialog = new AlertDialog->Builder(GetActivity())
                .SetTitle(R::string::wifi_p2p_cancel_connect_title)
                .SetMessage(GetActivity()->GetString(stringId, deviceName))
                .SetPositiveButton(GetActivity()->GetString(R::string::dlg_ok), mCancelConnectListener)
                .SetNegativeButton(GetActivity()->GetString(R::string::dlg_cancel), NULL)
                .Create();
            return dialog;
        } else if (id == DIALOG_RENAME) {
            mDeviceNameText = new EditText(GetActivity());
            mDeviceNameText->SetFilters(new InputFilter[] {new InputFilter->LengthFilter(30)});
            if (mSavedDeviceName != NULL) {
                mDeviceNameText->SetText(mSavedDeviceName);
                mDeviceNameText->SetSelection(mSavedDeviceName->Length());
            } else if (mThisDevice != NULL && !TextUtils->IsEmpty(mThisDevice.deviceName)) {
                mDeviceNameText->SetText(mThisDevice.deviceName);
                mDeviceNameText->SetSelection(0, mThisDevice.deviceName->Length());
            }
            mSavedDeviceName = NULL;
            AlertDialog dialog = new AlertDialog->Builder(GetActivity())
                .SetTitle(R::string::wifi_p2p_menu_rename)
                .SetView(mDeviceNameText)
                .SetPositiveButton(GetActivity()->GetString(R::string::dlg_ok), mRenameListener)
                .SetNegativeButton(GetActivity()->GetString(R::string::dlg_cancel), NULL)
                .Create();
            return dialog;
        } else if (id == DIALOG_DELETE_GROUP) {
            Int32 stringId = R::string::wifi_p2p_delete_group_message;

            AlertDialog dialog = new AlertDialog->Builder(GetActivity())
                .SetMessage(GetActivity()->GetString(stringId))
                .SetPositiveButton(GetActivity()->GetString(R::string::dlg_ok), mDeleteGroupListener)
                .SetNegativeButton(GetActivity()->GetString(R::string::dlg_cancel),
                        mDeleteGroupListener).Create();
            return dialog;
        }
        return NULL;
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle outState) {
        if (mSelectedWifiPeer != NULL) {
            outState->PutParcelable(SAVE_DIALOG_PEER, mSelectedWifiPeer.device);
        }
        if (mDeviceNameText != NULL) {
            outState->PutString(SAVE_DEVICE_NAME, mDeviceNameText->GetText()->ToString());
        }
        if (mSelectedGroup != NULL) {
            outState->PutString(SAVE_SELECTED_GROUP, mSelectedGroup->GetGroupName());
        }
    }

    private void HandlePeersChanged() {
        mPeersGroup->RemoveAll();

        mConnectedDevices = 0;
        if (DBG) Logger::D(TAG, "List of available peers");
        for (WifiP2pDevice peer: mPeers->GetDeviceList()) {
            if (DBG) Logger::D(TAG, "-> " + peer);
            mPeersGroup->AddPreference(new WifiP2pPeer(GetActivity(), peer));
            if (peer.status == WifiP2pDevice.CONNECTED) mConnectedDevices++;
        }
        if (DBG) Logger::D(TAG, " mConnectedDevices " + mConnectedDevices);
    }

    //@Override
    CARAPI OnPersistentGroupInfoAvailable(WifiP2pGroupList groups) {
        mPersistentGroup->RemoveAll();

        for (WifiP2pGroup group: groups->GetGroupList()) {
            if (DBG) Logger::D(TAG, " group " + group);
            WifiP2pPersistentGroup wppg = new WifiP2pPersistentGroup(GetActivity(), group);
            mPersistentGroup->AddPreference(wppg);
            if (wppg->GetGroupName()->Equals(mSelectedGroupName)) {
                if (DBG) Logger::D(TAG, "Selecting group " + wppg->GetGroupName());
                mSelectedGroup = wppg;
                mSelectedGroupName = NULL;
            }
        }
        if (mSelectedGroupName != NULL) {
            // Looks like there's a dialog pending getting user confirmation to delete the
            // selected group. When user hits OK on that dialog, we won't do anything; but we
            // shouldn't be in this situation in first place, because these groups are persistent
            // groups and they shouldn't just get deleted!
            Logger::W(TAG, " Selected group " + mSelectedGroupName + " disappered on next query ");
        }
    }

    //@Override
    CARAPI OnPeersAvailable(WifiP2pDeviceList peers) {
        if (DBG) Logger::D(TAG, "Requested peers are available");
        mPeers = peers;
        HandlePeersChanged();
    }

    private void HandleP2pStateChanged() {
        UpdateSearchMenu(FALSE);
        mThisDevicePref->SetEnabled(mWifiP2pEnabled);
        mPeersGroup->SetEnabled(mWifiP2pEnabled);
        mPersistentGroup->SetEnabled(mWifiP2pEnabled);
    }

    private void UpdateSearchMenu(Boolean searching) {
       mWifiP2pSearching = searching;
       Activity activity = GetActivity();
       if (activity != NULL) activity->InvalidateOptionsMenu();
    }

    private void StartSearch() {
        if (mWifiP2pManager != NULL && !mWifiP2pSearching) {
            mWifiP2pManager->DiscoverPeers(mChannel, new WifiP2pManager->ActionListener() {
                CARAPI OnSuccess() {
                }
                CARAPI OnFailure(Int32 reason) {
                    if (DBG) Logger::D(TAG, " discover fail " + reason);
                }
            });
        }
    }

    private void UpdateDevicePref() {
        if (mThisDevice != NULL) {
            if (TextUtils->IsEmpty(mThisDevice.deviceName)) {
                mThisDevicePref->SetTitle(mThisDevice.deviceAddress);
            } else {
                mThisDevicePref->SetTitle(mThisDevice.deviceName);
            }
        }
    }
}
