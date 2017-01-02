//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

package com.android.settings;

using Elastos::Droid::Settings::Wifi::IWifiApEnabler;
using Elastos::Droid::Settings::Wifi::IWifiApDialog;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Bluetooth::IBluetoothPan;
using Elastos::Droid::Bluetooth::IBluetoothProfile;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IAssetManager;
using Elastos::Droid::Hardware::Usb::IUsbManager;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::Wifi::IWifiConfiguration;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserManager;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceScreen;
using Elastos::Droid::Preference::ISwitchPreference;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Webkit::IWebView;
using Elastos::Droid::Widget::ITextView;

using Elastos::IO::IInputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Concurrent::Atomic::IAtomicReference;
using Elastos::Utility::ILocale;

/*
 * Displays preferences for Tethering.
 */
public class TetherSettings extends SettingsPreferenceFragment
        implements DialogInterface.OnClickListener, Preference.OnPreferenceChangeListener {
    private static const String TAG = "TetherSettings";

    private static const String USB_TETHER_SETTINGS = "usb_tether_settings";
    private static const String ENABLE_WIFI_AP = "enable_wifi_ap";
    private static const String ENABLE_BLUETOOTH_TETHERING = "enable_bluetooth_tethering";
    private static const String TETHER_CHOICE = "TETHER_TYPE";

    private static const Int32 DIALOG_AP_SETTINGS = 1;

    private WebView mView;
    private SwitchPreference mUsbTether;

    private WifiApEnabler mWifiApEnabler;
    private SwitchPreference mEnableWifiAp;

    private SwitchPreference mBluetoothTether;

    private BroadcastReceiver mTetherChangeReceiver;

    private String[] mUsbRegexs;

    private String[] mWifiRegexs;

    private String[] mBluetoothRegexs;
    private AtomicReference<BluetoothPan> mBluetoothPan = new AtomicReference<BluetoothPan>();

    private static const String WIFI_AP_SSID_AND_SECURITY = "wifi_ap_ssid_and_security";
    private static const Int32 CONFIG_SUBTEXT = R::string::wifi_tether_configure_subtext;

    private String[] mSecurityType;
    private Preference mCreateNetwork;

    private WifiApDialog mDialog;
    private WifiManager mWifiManager;
    private WifiConfiguration mWifiConfig = NULL;
    private UserManager mUm;

    private Boolean mUsbConnected;
    private Boolean mMassStorageActive;

    private Boolean mBluetoothEnableForTether;

    private static const Int32 INVALID             = -1;
    private static const Int32 WIFI_TETHERING      = 0;
    private static const Int32 USB_TETHERING       = 1;
    private static const Int32 BLUETOOTH_TETHERING = 2;

    /* One of INVALID, WIFI_TETHERING, USB_TETHERING or BLUETOOTH_TETHERING */
    private Int32 mTetherChoice = INVALID;

    /* Stores the package name and the class name of the provisioning app */
    private String[] mProvisionApp;
    private static const Int32 PROVISION_REQUEST = 0;

    private Boolean mUnavailable;

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        If(icicle != NULL) {
            mTetherChoice = icicle->GetInt(TETHER_CHOICE);
        }
        AddPreferencesFromResource(R.xml.tether_prefs);

        mUm = (UserManager) GetSystemService(Context.USER_SERVICE);

        if (mUm->HasUserRestriction(UserManager.DISALLOW_CONFIG_TETHERING)) {
            mUnavailable = TRUE;
            SetPreferenceScreen(new PreferenceScreen(GetActivity(), NULL));
            return;
        }

        final Activity activity = GetActivity();
        BluetoothAdapter adapter = BluetoothAdapter->GetDefaultAdapter();
        if (adapter != NULL) {
            adapter->GetProfileProxy(activity->GetApplicationContext(), mProfileServiceListener,
                    BluetoothProfile.PAN);
        }

        mEnableWifiAp =
                (SwitchPreference) FindPreference(ENABLE_WIFI_AP);
        Preference wifiApSettings = FindPreference(WIFI_AP_SSID_AND_SECURITY);
        mUsbTether = (SwitchPreference) FindPreference(USB_TETHER_SETTINGS);
        mBluetoothTether = (SwitchPreference) FindPreference(ENABLE_BLUETOOTH_TETHERING);

        ConnectivityManager cm =
                (ConnectivityManager)GetSystemService(Context.CONNECTIVITY_SERVICE);

        mUsbRegexs = cm->GetTetherableUsbRegexs();
        mWifiRegexs = cm->GetTetherableWifiRegexs();
        mBluetoothRegexs = cm->GetTetherableBluetoothRegexs();

        final Boolean usbAvailable = mUsbRegexs.length != 0;
        final Boolean wifiAvailable = mWifiRegexs.length != 0;
        final Boolean bluetoothAvailable = mBluetoothRegexs.length != 0;

        if (!usbAvailable || Utils->IsMonkeyRunning()) {
            GetPreferenceScreen()->RemovePreference(mUsbTether);
        }

        if (wifiAvailable && !Utils->IsMonkeyRunning()) {
            mWifiApEnabler = new WifiApEnabler(activity, mEnableWifiAp);
            InitWifiTethering();
        } else {
            GetPreferenceScreen()->RemovePreference(mEnableWifiAp);
            GetPreferenceScreen()->RemovePreference(wifiApSettings);
        }

        if (!bluetoothAvailable) {
            GetPreferenceScreen()->RemovePreference(mBluetoothTether);
        } else {
            BluetoothPan pan = mBluetoothPan->Get();
            if (pan != NULL && pan->IsTetheringOn()) {
                mBluetoothTether->SetChecked(TRUE);
            } else {
                mBluetoothTether->SetChecked(FALSE);
            }
        }

        mProvisionApp = GetResources()->GetStringArray(
                R.array.config_mobile_hotspot_provision_app);

        mView = new WebView(activity);
    }

    //@Override
    CARAPI OnSaveInstanceState(Bundle savedInstanceState) {
        savedInstanceState->PutInt(TETHER_CHOICE, mTetherChoice);
        super->OnSaveInstanceState(savedInstanceState);
    }

    private void InitWifiTethering() {
        final Activity activity = GetActivity();
        mWifiManager = (WifiManager) GetSystemService(Context.WIFI_SERVICE);
        mWifiConfig = mWifiManager->GetWifiApConfiguration();
        mSecurityType = GetResources()->GetStringArray(R.array.wifi_ap_security);

        mCreateNetwork = FindPreference(WIFI_AP_SSID_AND_SECURITY);

        if (mWifiConfig == NULL) {
            final String s = activity->GetString(
                    R::string::wifi_tether_configure_ssid_default);
            mCreateNetwork->SetSummary(String->Format(activity->GetString(CONFIG_SUBTEXT),
                    s, mSecurityType[WifiApDialog.OPEN_INDEX]));
        } else {
            Int32 index = WifiApDialog->GetSecurityTypeIndex(mWifiConfig);
            mCreateNetwork->SetSummary(String->Format(activity->GetString(CONFIG_SUBTEXT),
                    mWifiConfig.SSID,
                    mSecurityType[index]));
        }
    }

    private BluetoothProfile.ServiceListener mProfileServiceListener =
        new BluetoothProfile->ServiceListener() {
        CARAPI OnServiceConnected(Int32 profile, BluetoothProfile proxy) {
            mBluetoothPan->Set((BluetoothPan) proxy);
        }
        CARAPI OnServiceDisconnected(Int32 profile) {
            mBluetoothPan->Set(NULL);
        }
    };

    //@Override
    public Dialog OnCreateDialog(Int32 id) {
        if (id == DIALOG_AP_SETTINGS) {
            final Activity activity = GetActivity();
            mDialog = new WifiApDialog(activity, this, mWifiConfig);
            return mDialog;
        }

        return NULL;
    }

    private class TetherChangeReceiver extends BroadcastReceiver {
        //@Override
        CARAPI OnReceive(Context content, Intent intent) {
            String action = intent->GetAction();
            if (action->Equals(ConnectivityManager.ACTION_TETHER_STATE_CHANGED)) {
                // TODO - this should understand the interface types
                ArrayList<String> available = intent->GetStringArrayListExtra(
                        ConnectivityManager.EXTRA_AVAILABLE_TETHER);
                ArrayList<String> active = intent->GetStringArrayListExtra(
                        ConnectivityManager.EXTRA_ACTIVE_TETHER);
                ArrayList<String> errored = intent->GetStringArrayListExtra(
                        ConnectivityManager.EXTRA_ERRORED_TETHER);
                UpdateState(available->ToArray(new String[available->Size()]),
                        active->ToArray(new String[active->Size()]),
                        errored->ToArray(new String[errored->Size()]));
            } else if (action->Equals(IIntent::ACTION_MEDIA_SHARED)) {
                mMassStorageActive = TRUE;
                UpdateState();
            } else if (action->Equals(IIntent::ACTION_MEDIA_UNSHARED)) {
                mMassStorageActive = FALSE;
                UpdateState();
            } else if (action->Equals(UsbManager.ACTION_USB_STATE)) {
                mUsbConnected = intent->GetBooleanExtra(UsbManager.USB_CONNECTED, FALSE);
                UpdateState();
            } else if (action->Equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                if (mBluetoothEnableForTether) {
                    switch (intent
                            .GetIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR)) {
                        case BluetoothAdapter.STATE_ON:
                            BluetoothPan bluetoothPan = mBluetoothPan->Get();
                            if (bluetoothPan != NULL) {
                                bluetoothPan->SetBluetoothTethering(TRUE);
                                mBluetoothEnableForTether = FALSE;
                            }
                            break;

                        case BluetoothAdapter.STATE_OFF:
                        case BluetoothAdapter.ERROR:
                            mBluetoothEnableForTether = FALSE;
                            break;

                        default:
                            // ignore transition states
                    }
                }
                UpdateState();
            }
        }
    }

    //@Override
    CARAPI OnStart() {
        super->OnStart();

        if (mUnavailable) {
            TextView emptyView = (TextView) GetView()->FindViewById(android.R.id.empty);
            GetListView()->SetEmptyView(emptyView);
            if (emptyView != NULL) {
                emptyView->SetText(R::string::tethering_settings_not_available);
            }
            return;
        }

        final Activity activity = GetActivity();

        mMassStorageActive = Environment.MEDIA_SHARED->Equals(Environment->GetExternalStorageState());
        mTetherChangeReceiver = new TetherChangeReceiver();
        IntentFilter filter = new IntentFilter(ConnectivityManager.ACTION_TETHER_STATE_CHANGED);
        Intent intent = activity->RegisterReceiver(mTetherChangeReceiver, filter);

        filter = new IntentFilter();
        filter->AddAction(UsbManager.ACTION_USB_STATE);
        activity->RegisterReceiver(mTetherChangeReceiver, filter);

        filter = new IntentFilter();
        filter->AddAction(IIntent::ACTION_MEDIA_SHARED);
        filter->AddAction(IIntent::ACTION_MEDIA_UNSHARED);
        filter->AddDataScheme("file");
        activity->RegisterReceiver(mTetherChangeReceiver, filter);

        filter = new IntentFilter();
        filter->AddAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        activity->RegisterReceiver(mTetherChangeReceiver, filter);

        if (intent != NULL) mTetherChangeReceiver->OnReceive(activity, intent);
        if (mWifiApEnabler != NULL) {
            mEnableWifiAp->SetOnPreferenceChangeListener(this);
            mWifiApEnabler->Resume();
        }

        UpdateState();
    }

    //@Override
    CARAPI OnStop() {
        super->OnStop();

        if (mUnavailable) {
            return;
        }
        GetActivity()->UnregisterReceiver(mTetherChangeReceiver);
        mTetherChangeReceiver = NULL;
        if (mWifiApEnabler != NULL) {
            mEnableWifiAp->SetOnPreferenceChangeListener(NULL);
            mWifiApEnabler->Pause();
        }
    }

    private void UpdateState() {
        ConnectivityManager cm =
                (ConnectivityManager)GetSystemService(Context.CONNECTIVITY_SERVICE);

        String[] available = cm->GetTetherableIfaces();
        String[] tethered = cm->GetTetheredIfaces();
        String[] errored = cm->GetTetheringErroredIfaces();
        UpdateState(available, tethered, errored);
    }

    private void UpdateState(String[] available, String[] tethered,
            String[] errored) {
        UpdateUsbState(available, tethered, errored);
        UpdateBluetoothState(available, tethered, errored);
    }


    private void UpdateUsbState(String[] available, String[] tethered,
            String[] errored) {
        ConnectivityManager cm =
                (ConnectivityManager)GetSystemService(Context.CONNECTIVITY_SERVICE);
        Boolean usbAvailable = mUsbConnected && !mMassStorageActive;
        Int32 usbError = ConnectivityManager.TETHER_ERROR_NO_ERROR;
        for (String s : available) {
            for (String regex : mUsbRegexs) {
                if (s->Matches(regex)) {
                    if (usbError == ConnectivityManager.TETHER_ERROR_NO_ERROR) {
                        usbError = cm->GetLastTetherError(s);
                    }
                }
            }
        }
        Boolean usbTethered = FALSE;
        for (String s : tethered) {
            for (String regex : mUsbRegexs) {
                if (s->Matches(regex)) usbTethered = TRUE;
            }
        }
        Boolean usbErrored = FALSE;
        for (String s: errored) {
            for (String regex : mUsbRegexs) {
                if (s->Matches(regex)) usbErrored = TRUE;
            }
        }

        if (usbTethered) {
            mUsbTether->SetSummary(R::string::usb_tethering_active_subtext);
            mUsbTether->SetEnabled(TRUE);
            mUsbTether->SetChecked(TRUE);
        } else if (usbAvailable) {
            if (usbError == ConnectivityManager.TETHER_ERROR_NO_ERROR) {
                mUsbTether->SetSummary(R::string::usb_tethering_available_subtext);
            } else {
                mUsbTether->SetSummary(R::string::usb_tethering_errored_subtext);
            }
            mUsbTether->SetEnabled(TRUE);
            mUsbTether->SetChecked(FALSE);
        } else if (usbErrored) {
            mUsbTether->SetSummary(R::string::usb_tethering_errored_subtext);
            mUsbTether->SetEnabled(FALSE);
            mUsbTether->SetChecked(FALSE);
        } else if (mMassStorageActive) {
            mUsbTether->SetSummary(R::string::usb_tethering_storage_active_subtext);
            mUsbTether->SetEnabled(FALSE);
            mUsbTether->SetChecked(FALSE);
        } else {
            mUsbTether->SetSummary(R::string::usb_tethering_unavailable_subtext);
            mUsbTether->SetEnabled(FALSE);
            mUsbTether->SetChecked(FALSE);
        }
    }

    private void UpdateBluetoothState(String[] available, String[] tethered,
            String[] errored) {
        Boolean bluetoothErrored = FALSE;
        for (String s: errored) {
            for (String regex : mBluetoothRegexs) {
                if (s->Matches(regex)) bluetoothErrored = TRUE;
            }
        }

        BluetoothAdapter adapter = BluetoothAdapter->GetDefaultAdapter();
        if (adapter == NULL)
            return;
        Int32 btState = adapter->GetState();
        if (btState == BluetoothAdapter.STATE_TURNING_OFF) {
            mBluetoothTether->SetEnabled(FALSE);
            mBluetoothTether->SetSummary(R::string::bluetooth_turning_off);
        } else if (btState == BluetoothAdapter.STATE_TURNING_ON) {
            mBluetoothTether->SetEnabled(FALSE);
            mBluetoothTether->SetSummary(R::string::bluetooth_turning_on);
        } else {
            BluetoothPan bluetoothPan = mBluetoothPan->Get();
            if (btState == BluetoothAdapter.STATE_ON && bluetoothPan != NULL &&
                    bluetoothPan->IsTetheringOn()) {
                mBluetoothTether->SetChecked(TRUE);
                mBluetoothTether->SetEnabled(TRUE);
                Int32 bluetoothTethered = bluetoothPan->GetConnectedDevices()->Size();
                if (bluetoothTethered > 1) {
                    String summary = GetString(
                            R::string::bluetooth_tethering_devices_connected_subtext,
                            bluetoothTethered);
                    mBluetoothTether->SetSummary(summary);
                } else if (bluetoothTethered == 1) {
                    mBluetoothTether->SetSummary(
                            R::string::bluetooth_tethering_device_connected_subtext);
                } else if (bluetoothErrored) {
                    mBluetoothTether->SetSummary(R::string::bluetooth_tethering_errored_subtext);
                } else {
                    mBluetoothTether->SetSummary(R::string::bluetooth_tethering_available_subtext);
                }
            } else {
                mBluetoothTether->SetEnabled(TRUE);
                mBluetoothTether->SetChecked(FALSE);
                mBluetoothTether->SetSummary(R::string::bluetooth_tethering_off_subtext);
            }
        }
    }

    public Boolean OnPreferenceChange(Preference preference, Object value) {
        Boolean enable = (Boolean) value;

        if (enable) {
            StartProvisioningIfNecessary(WIFI_TETHERING);
        } else {
            mWifiApEnabler->SetSoftapEnabled(FALSE);
        }
        return FALSE;
    }

    public static Boolean IsProvisioningNeededButUnavailable(Context context) {
        String[] provisionApp = context->GetResources()->GetStringArray(
                R.array.config_mobile_hotspot_provision_app);
        return (IsProvisioningNeeded(provisionApp)
                && !IsIntentAvailable(context, provisionApp));
    }

    private static Boolean IsIntentAvailable(Context context, String[] provisionApp) {
        if (provisionApp.length <  2) {
            throw new IllegalArgumentException("provisionApp length should at least be 2");
        }
        final PackageManager packageManager = context->GetPackageManager();
        Intent intent = new Intent(IIntent::ACTION_MAIN);
        intent->SetClassName(provisionApp[0], provisionApp[1]);

        return (packageManager->QueryIntentActivities(intent,
                PackageManager.MATCH_DEFAULT_ONLY).Size() > 0);
    }


    private static Boolean IsProvisioningNeeded(String[] provisionApp) {
        if (SystemProperties->GetBoolean("net.tethering.noprovisioning", FALSE)
                || provisionApp == NULL) {
            return FALSE;
        }
        return (provisionApp.length == 2);
    }

    private void StartProvisioningIfNecessary(Int32 choice) {
        mTetherChoice = choice;
        if (IsProvisioningNeeded(mProvisionApp)) {
            Intent intent = new Intent(IIntent::ACTION_MAIN);
            intent->SetClassName(mProvisionApp[0], mProvisionApp[1]);
            intent->PutExtra(TETHER_CHOICE, mTetherChoice);
            StartActivityForResult(intent, PROVISION_REQUEST);
        } else {
            StartTethering();
        }
    }

    CARAPI OnActivityResult(Int32 requestCode, Int32 resultCode, Intent intent) {
        super->OnActivityResult(requestCode, resultCode, intent);
        if (requestCode == PROVISION_REQUEST) {
            if (resultCode == Activity.RESULT_OK) {
                StartTethering();
            } else {
                //BT and USB need switch turned off on failure
                //Wifi tethering is never turned on until afterwards
                switch (mTetherChoice) {
                    case BLUETOOTH_TETHERING:
                        mBluetoothTether->SetChecked(FALSE);
                        break;
                    case USB_TETHERING:
                        mUsbTether->SetChecked(FALSE);
                        break;
                }
                mTetherChoice = INVALID;
            }
        }
    }

    private void StartTethering() {
        switch (mTetherChoice) {
            case WIFI_TETHERING:
                mWifiApEnabler->SetSoftapEnabled(TRUE);
                break;
            case BLUETOOTH_TETHERING:
                // turn on Bluetooth first
                BluetoothAdapter adapter = BluetoothAdapter->GetDefaultAdapter();
                if (adapter->GetState() == BluetoothAdapter.STATE_OFF) {
                    mBluetoothEnableForTether = TRUE;
                    adapter->Enable();
                    mBluetoothTether->SetSummary(R::string::bluetooth_turning_on);
                    mBluetoothTether->SetEnabled(FALSE);
                } else {
                    BluetoothPan bluetoothPan = mBluetoothPan->Get();
                    if (bluetoothPan != NULL) bluetoothPan->SetBluetoothTethering(TRUE);
                    mBluetoothTether->SetSummary(R::string::bluetooth_tethering_available_subtext);
                }
                break;
            case USB_TETHERING:
                SetUsbTethering(TRUE);
                break;
            default:
                //should not happen
                break;
        }
    }

    private void SetUsbTethering(Boolean enabled) {
        ConnectivityManager cm =
            (ConnectivityManager)GetSystemService(Context.CONNECTIVITY_SERVICE);
        mUsbTether->SetChecked(FALSE);
        if (cm->SetUsbTethering(enabled) != ConnectivityManager.TETHER_ERROR_NO_ERROR) {
            mUsbTether->SetSummary(R::string::usb_tethering_errored_subtext);
            return;
        }
        mUsbTether->SetSummary("");
    }

    //@Override
    public Boolean OnPreferenceTreeClick(PreferenceScreen screen, Preference preference) {
        ConnectivityManager cm =
                (ConnectivityManager)GetSystemService(Context.CONNECTIVITY_SERVICE);

        if (preference == mUsbTether) {
            Boolean newState = mUsbTether->IsChecked();

            if (newState) {
                StartProvisioningIfNecessary(USB_TETHERING);
            } else {
                SetUsbTethering(newState);
            }
        } else if (preference == mBluetoothTether) {
            Boolean bluetoothTetherState = mBluetoothTether->IsChecked();

            if (bluetoothTetherState) {
                StartProvisioningIfNecessary(BLUETOOTH_TETHERING);
            } else {
                Boolean errored = FALSE;

                String [] tethered = cm->GetTetheredIfaces();
                String bluetoothIface = FindIface(tethered, mBluetoothRegexs);
                if (bluetoothIface != NULL &&
                        cm->Untether(bluetoothIface) != ConnectivityManager.TETHER_ERROR_NO_ERROR) {
                    errored = TRUE;
                }

                BluetoothPan bluetoothPan = mBluetoothPan->Get();
                if (bluetoothPan != NULL) bluetoothPan->SetBluetoothTethering(FALSE);
                if (errored) {
                    mBluetoothTether->SetSummary(R::string::bluetooth_tethering_errored_subtext);
                } else {
                    mBluetoothTether->SetSummary(R::string::bluetooth_tethering_off_subtext);
                }
            }
        } else if (preference == mCreateNetwork) {
            ShowDialog(DIALOG_AP_SETTINGS);
        }

        return super->OnPreferenceTreeClick(screen, preference);
    }

    private static String FindIface(String[] ifaces, String[] regexes) {
        for (String iface : ifaces) {
            for (String regex : regexes) {
                if (iface->Matches(regex)) {
                    return iface;
                }
            }
        }
        return NULL;
    }

    CARAPI OnClick(DialogInterface dialogInterface, Int32 button) {
        if (button == DialogInterface.BUTTON_POSITIVE) {
            mWifiConfig = mDialog->GetConfig();
            if (mWifiConfig != NULL) {
                /**
                 * if soft AP is stopped, bring up
                 * else restart with new config
                 * TODO: update config on a running access point when framework support is added
                 */
                if (mWifiManager->GetWifiApState() == WifiManager.WIFI_AP_STATE_ENABLED) {
                    mWifiManager->SetWifiApEnabled(NULL, FALSE);
                    mWifiManager->SetWifiApEnabled(mWifiConfig, TRUE);
                } else {
                    mWifiManager->SetWifiApConfiguration(mWifiConfig);
                }
                Int32 index = WifiApDialog->GetSecurityTypeIndex(mWifiConfig);
                mCreateNetwork->SetSummary(String->Format(GetActivity()->GetString(CONFIG_SUBTEXT),
                        mWifiConfig.SSID,
                        mSecurityType[index]));
            }
        }
    }

    //@Override
    public Int32 GetHelpResource() {
        return R::string::help_url_tether;
    }

    /**
     * Checks whether this screen will have anything to show on this device. This is called by
     * the shortcut picker for Settings shortcuts (home screen widget).
     * @param context a context object for getting a system service.
     * @return whether Tether & portable hotspot should be shown in the shortcuts picker.
     */
    public static Boolean ShowInShortcuts(Context context) {
        final ConnectivityManager cm =
                (ConnectivityManager)context->GetSystemService(Context.CONNECTIVITY_SERVICE);
        final Boolean isSecondaryUser = UserHandle->MyUserId() != UserHandle.USER_OWNER;
        return !isSecondaryUser && cm->IsTetheringSupported();
    }
}
