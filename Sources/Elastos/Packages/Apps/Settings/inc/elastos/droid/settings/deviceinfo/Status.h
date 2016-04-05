

package com.android.settings.deviceinfo;

using Elastos::Droid::Bluetooth::IBluetoothAdapter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IClipboardManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::Wifi::IWifiInfo;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IBuild;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemClock;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Preference::IPreferenceActivity;
using Elastos::Droid::Telephony::ICellBroadcastMessage;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IListAdapter;
using Elastos::Droid::Widget::IToast;

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneStateIntentReceiver;
using Elastos::Droid::Internal::Utility::IArrayUtils;
using Elastos::Droid::Settings::IR;
using Elastos::Droid::Settings::IUtils;

using Elastos::Lang::Ref::IWeakReference;

/**
 * Display the following information
 * # Phone Number
 * # Network
 * # Roaming
 * # Device Id (IMEI in GSM and MEID in CDMA)
 * # Network type
 * # Operator info (area info cell broadcast for Brazil)
 * # Signal Strength
 * # Battery Strength  : TODO
 * # Uptime
 * # Awake Time
 * # XMPP/buzz/tickle status : TODO
 *
 */
public class Status extends PreferenceActivity {

    private static const String KEY_DATA_STATE = "data_state";
    private static const String KEY_SERVICE_STATE = "service_state";
    private static const String KEY_OPERATOR_NAME = "operator_name";
    private static const String KEY_ROAMING_STATE = "roaming_state";
    private static const String KEY_NETWORK_TYPE = "network_type";
    private static const String KEY_LATEST_AREA_INFO = "latest_area_info";
    private static const String KEY_PHONE_NUMBER = "number";
    private static const String KEY_IMEI_SV = "imei_sv";
    private static const String KEY_IMEI = "imei";
    private static const String KEY_PRL_VERSION = "prl_version";
    private static const String KEY_MIN_NUMBER = "min_number";
    private static const String KEY_MEID_NUMBER = "meid_number";
    private static const String KEY_SIGNAL_STRENGTH = "signal_strength";
    private static const String KEY_BATTERY_STATUS = "battery_status";
    private static const String KEY_BATTERY_LEVEL = "battery_level";
    private static const String KEY_IP_ADDRESS = "wifi_ip_address";
    private static const String KEY_WIFI_MAC_ADDRESS = "wifi_mac_address";
    private static const String KEY_BT_ADDRESS = "bt_address";
    private static const String KEY_SERIAL_NUMBER = "serial_number";
    private static const String KEY_ICC_ID = "icc_id";
    private static const String KEY_WIMAX_MAC_ADDRESS = "wimax_mac_address";
    private static const String[] PHONE_RELATED_ENTRIES = {
        KEY_DATA_STATE,
        KEY_SERVICE_STATE,
        KEY_OPERATOR_NAME,
        KEY_ROAMING_STATE,
        KEY_NETWORK_TYPE,
        KEY_LATEST_AREA_INFO,
        KEY_PHONE_NUMBER,
        KEY_IMEI,
        KEY_IMEI_SV,
        KEY_PRL_VERSION,
        KEY_MIN_NUMBER,
        KEY_MEID_NUMBER,
        KEY_SIGNAL_STRENGTH,
        KEY_ICC_ID
    };

    static const String CB_AREA_INFO_RECEIVED_ACTION =
            "android.cellbroadcastreceiver.CB_AREA_INFO_RECEIVED";

    static const String GET_LATEST_CB_AREA_INFO_ACTION =
            "android.cellbroadcastreceiver.GET_LATEST_CB_AREA_INFO";

    // Require the sender to have this permission to prevent third-party spoofing.
    static const String CB_AREA_INFO_SENDER_PERMISSION =
            "android.permission.RECEIVE_EMERGENCY_BROADCAST";

    // Broadcasts to listen to for connectivity changes.
    private static const String[] CONNECTIVITY_INTENTS = {
            BluetoothAdapter.ACTION_STATE_CHANGED,
            ConnectivityManager.CONNECTIVITY_ACTION_IMMEDIATE,
            WifiManager.LINK_CONFIGURATION_CHANGED_ACTION,
            WifiManager.NETWORK_STATE_CHANGED_ACTION,
    };

    private static const Int32 EVENT_SIGNAL_STRENGTH_CHANGED = 200;
    private static const Int32 EVENT_SERVICE_STATE_CHANGED = 300;

    private static const Int32 EVENT_UPDATE_STATS = 500;

    private static const Int32 EVENT_UPDATE_CONNECTIVITY = 600;

    private ConnectivityManager mCM;
    private TelephonyManager mTelephonyManager;
    private WifiManager mWifiManager;

    private Phone mPhone = NULL;
    private PhoneStateIntentReceiver mPhoneStateReceiver;
    private Resources mRes;
    private Boolean mShowLatestAreaInfo;

    private String mUnknown;
    private String mUnavailable;

    private Preference mSignalStrength;
    private Preference mUptime;
    private Preference mBatteryStatus;
    private Preference mBatteryLevel;
    private Preference mBtAddress;
    private Preference mIpAddress;
    private Preference mWifiMacAddress;
    private Preference mWimaxMacAddress;

    private Handler mHandler;

    private static class MyHandler extends Handler {
        private WeakReference<Status> mStatus;

        public MyHandler(Status activity) {
            mStatus = new WeakReference<Status>(activity);
        }

        //@Override
        CARAPI HandleMessage(Message msg) {
            Status status = mStatus->Get();
            if (status == NULL) {
                return;
            }

            switch (msg.what) {
                case EVENT_SIGNAL_STRENGTH_CHANGED:
                    status->UpdateSignalStrength();
                    break;

                case EVENT_SERVICE_STATE_CHANGED:
                    ServiceState serviceState = status.mPhoneStateReceiver->GetServiceState();
                    status->UpdateServiceState(serviceState);
                    break;

                case EVENT_UPDATE_STATS:
                    status->UpdateTimes();
                    SendEmptyMessageDelayed(EVENT_UPDATE_STATS, 1000);
                    break;

                case EVENT_UPDATE_CONNECTIVITY:
                    status->UpdateConnectivity();
                    break;
            }
        }
    }

    private BroadcastReceiver mBatteryInfoReceiver = new BroadcastReceiver() {

        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (IIntent::ACTION_BATTERY_CHANGED->Equals(action)) {
                mBatteryLevel->SetSummary(Utils->GetBatteryPercentage(intent));
                mBatteryStatus->SetSummary(Utils->GetBatteryStatus(GetResources(), intent));
            }
        }
    };

    private PhoneStateListener mPhoneStateListener = new PhoneStateListener() {
        //@Override
        CARAPI OnDataConnectionStateChanged(Int32 state) {
            UpdateDataState();
            UpdateNetworkType();
        }
    };

    private BroadcastReceiver mAreaInfoReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (CB_AREA_INFO_RECEIVED_ACTION->Equals(action)) {
                Bundle extras = intent->GetExtras();
                if (extras == NULL) {
                    return;
                }
                CellBroadcastMessage cbMessage = (CellBroadcastMessage) extras->Get("message");
                if (cbMessage != NULL && cbMessage->GetServiceCategory() == 50) {
                    String latestAreaInfo = cbMessage->GetMessageBody();
                    UpdateAreaInfo(latestAreaInfo);
                }
            }
        }
    };

    private IntentFilter mConnectivityIntentFilter;
    private final BroadcastReceiver mConnectivityReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            String action = intent->GetAction();
            if (ArrayUtils->Contains(CONNECTIVITY_INTENTS, action)) {
                mHandler->SendEmptyMessage(EVENT_UPDATE_CONNECTIVITY);
            }
        }
    };

    private Boolean HasBluetooth() {
        return BluetoothAdapter->GetDefaultAdapter() != NULL;
    }

    private Boolean HasWimax() {
        return  mCM->GetNetworkInfo(ConnectivityManager.TYPE_WIMAX) != NULL;
    }

    //@Override
    protected void OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        mHandler = new MyHandler(this);

        mCM = (ConnectivityManager) GetSystemService(CONNECTIVITY_SERVICE);
        mTelephonyManager = (TelephonyManager)GetSystemService(TELEPHONY_SERVICE);
        mWifiManager = (WifiManager) GetSystemService(WIFI_SERVICE);

        AddPreferencesFromResource(R.xml.device_info_status);
        mBatteryLevel = FindPreference(KEY_BATTERY_LEVEL);
        mBatteryStatus = FindPreference(KEY_BATTERY_STATUS);
        mBtAddress = FindPreference(KEY_BT_ADDRESS);
        mWifiMacAddress = FindPreference(KEY_WIFI_MAC_ADDRESS);
        mWimaxMacAddress = FindPreference(KEY_WIMAX_MAC_ADDRESS);
        mIpAddress = FindPreference(KEY_IP_ADDRESS);

        mRes = GetResources();
        mUnknown = mRes->GetString(R::string::device_info_default);
        mUnavailable = mRes->GetString(R::string::status_unavailable);

        if (UserHandle->MyUserId() == UserHandle.USER_OWNER) {
            mPhone = PhoneFactory->GetDefaultPhone();
        }
        // Note - missing in zaku build, be careful later...
        mSignalStrength = FindPreference(KEY_SIGNAL_STRENGTH);
        mUptime = FindPreference("up_time");

        if (mPhone == NULL || Utils->IsWifiOnly(GetApplicationContext())) {
            for (String key : PHONE_RELATED_ENTRIES) {
                RemovePreferenceFromScreen(key);
            }
        } else {
            // NOTE "imei" is the "Device ID" since it represents
            //  the IMEI in GSM and the MEID in CDMA
            if (mPhone->GetPhoneName()->Equals("CDMA")) {
                SetSummaryText(KEY_MEID_NUMBER, mPhone->GetMeid());
                SetSummaryText(KEY_MIN_NUMBER, mPhone->GetCdmaMin());
                if (GetResources()->GetBoolean(R.bool.config_msid_enable)) {
                    FindPreference(KEY_MIN_NUMBER).SetTitle(R::string::status_msid_number);
                }
                SetSummaryText(KEY_PRL_VERSION, mPhone->GetCdmaPrlVersion());
                RemovePreferenceFromScreen(KEY_IMEI_SV);

                if (mPhone->GetLteOnCdmaMode() == PhoneConstants.LTE_ON_CDMA_TRUE) {
                    // Show ICC ID and IMEI for LTE device
                    SetSummaryText(KEY_ICC_ID, mPhone->GetIccSerialNumber());
                    SetSummaryText(KEY_IMEI, mPhone->GetImei());
                } else {
                    // device is not GSM/UMTS, do not display GSM/UMTS features
                    // check Null in case no specified preference in overlay xml
                    RemovePreferenceFromScreen(KEY_IMEI);
                    RemovePreferenceFromScreen(KEY_ICC_ID);
                }
            } else {
                SetSummaryText(KEY_IMEI, mPhone->GetDeviceId());

                SetSummaryText(KEY_IMEI_SV,
                        ((TelephonyManager) GetSystemService(TELEPHONY_SERVICE))
                            .GetDeviceSoftwareVersion());

                // device is not CDMA, do not display CDMA features
                // check Null in case no specified preference in overlay xml
                RemovePreferenceFromScreen(KEY_PRL_VERSION);
                RemovePreferenceFromScreen(KEY_MEID_NUMBER);
                RemovePreferenceFromScreen(KEY_MIN_NUMBER);
                RemovePreferenceFromScreen(KEY_ICC_ID);

                // only show area info when SIM country is Brazil
                if ("br".Equals(mTelephonyManager->GetSimCountryIso())) {
                    mShowLatestAreaInfo = TRUE;
                }
            }

            String rawNumber = mTelephonyManager->GetLine1Number();  // may be NULL or empty
            String formattedNumber = NULL;
            if (!TextUtils->IsEmpty(rawNumber)) {
                formattedNumber = PhoneNumberUtils->FormatNumber(rawNumber);
            }
            // If formattedNumber is NULL or empty, it'll display as "Unknown".
            SetSummaryText(KEY_PHONE_NUMBER, formattedNumber);

            mPhoneStateReceiver = new PhoneStateIntentReceiver(this, mHandler);
            mPhoneStateReceiver->NotifySignalStrength(EVENT_SIGNAL_STRENGTH_CHANGED);
            mPhoneStateReceiver->NotifyServiceState(EVENT_SERVICE_STATE_CHANGED);

            if (!mShowLatestAreaInfo) {
                RemovePreferenceFromScreen(KEY_LATEST_AREA_INFO);
            }
        }

        if (!HasBluetooth()) {
            GetPreferenceScreen()->RemovePreference(mBtAddress);
            mBtAddress = NULL;
        }

        if (!HasWimax()) {
            GetPreferenceScreen()->RemovePreference(mWimaxMacAddress);
            mWimaxMacAddress = NULL;
        }

        mConnectivityIntentFilter = new IntentFilter();
        for (String intent: CONNECTIVITY_INTENTS) {
             mConnectivityIntentFilter->AddAction(intent);
        }

        UpdateConnectivity();

        String serial = Build.SERIAL;
        if (serial != NULL && !serial->Equals("")) {
            SetSummaryText(KEY_SERIAL_NUMBER, serial);
        } else {
            RemovePreferenceFromScreen(KEY_SERIAL_NUMBER);
        }

        // Make every pref on this screen copy its data to the clipboard on longpress.
        // Super convenient for capturing the IMEI, MAC addr, serial, etc.
        GetListView()->SetOnItemLongClickListener(
            new AdapterView->OnItemLongClickListener() {
                //@Override
                public Boolean OnItemLongClick(AdapterView<?> parent, View view,
                        Int32 position, Int64 id) {
                    ListAdapter listAdapter = (ListAdapter) parent->GetAdapter();
                    Preference pref = (Preference) listAdapter->GetItem(position);

                    ClipboardManager cm = (ClipboardManager)
                            GetSystemService(Context.CLIPBOARD_SERVICE);
                    cm->SetText(pref->GetSummary());
                    Toast->MakeText(
                        Status.this,
                        R::string::text_copied,
                        Toast.LENGTH_SHORT).Show();
                    return TRUE;
                }
            });
    }

    //@Override
    protected void OnResume() {
        super->OnResume();

        if (mPhone != NULL && !Utils->IsWifiOnly(GetApplicationContext())) {
            mPhoneStateReceiver->RegisterIntent();

            UpdateSignalStrength();
            UpdateServiceState(mPhone->GetServiceState());
            UpdateDataState();
            mTelephonyManager->Listen(mPhoneStateListener,
                    PhoneStateListener.LISTEN_DATA_CONNECTION_STATE);
            if (mShowLatestAreaInfo) {
                RegisterReceiver(mAreaInfoReceiver, new IntentFilter(CB_AREA_INFO_RECEIVED_ACTION),
                        CB_AREA_INFO_SENDER_PERMISSION, NULL);
                // Ask CellBroadcastReceiver to broadcast the latest area info received
                Intent getLatestIntent = new Intent(GET_LATEST_CB_AREA_INFO_ACTION);
                SendBroadcastAsUser(getLatestIntent, UserHandle.ALL,
                        CB_AREA_INFO_SENDER_PERMISSION);
            }
        }
        RegisterReceiver(mConnectivityReceiver, mConnectivityIntentFilter,
                         Manifest::permission::CHANGE_NETWORK_STATE, NULL);
        RegisterReceiver(mBatteryInfoReceiver, new IntentFilter(IIntent::ACTION_BATTERY_CHANGED));
        mHandler->SendEmptyMessage(EVENT_UPDATE_STATS);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();

        if (mPhone != NULL && !Utils->IsWifiOnly(GetApplicationContext())) {
            mPhoneStateReceiver->UnregisterIntent();
            mTelephonyManager->Listen(mPhoneStateListener, PhoneStateListener.LISTEN_NONE);
        }
        if (mShowLatestAreaInfo) {
            UnregisterReceiver(mAreaInfoReceiver);
        }
        UnregisterReceiver(mBatteryInfoReceiver);
        UnregisterReceiver(mConnectivityReceiver);
        mHandler->RemoveMessages(EVENT_UPDATE_STATS);
    }

    /**
     * Removes the specified preference, if it exists.
     * @param key the key for the Preference item
     */
    private void RemovePreferenceFromScreen(String key) {
        Preference pref = FindPreference(key);
        if (pref != NULL) {
            GetPreferenceScreen()->RemovePreference(pref);
        }
    }

    /**
     * @param preference The key for the Preference item
     * @param property The system property to fetch
     * @param alt The default value, if the property doesn't exist
     */
    private void SetSummary(String preference, String property, String alt) {
        try {
            FindPreference(preference).SetSummary(
                    SystemProperties->Get(property, alt));
        } catch (RuntimeException e) {

        }
    }

    private void SetSummaryText(String preference, String text) {
            if (TextUtils->IsEmpty(text)) {
               text = mUnknown;
             }
             // some preferences may be missing
             if (FindPreference(preference) != NULL) {
                 FindPreference(preference).SetSummary(text);
             }
    }

    private void UpdateNetworkType() {
        // Whether EDGE, UMTS, etc...
        String networktype = NULL;
        if (TelephonyManager.NETWORK_TYPE_UNKNOWN != mTelephonyManager->GetNetworkType()) {
            networktype = mTelephonyManager->GetNetworkTypeName();
        }
        SetSummaryText(KEY_NETWORK_TYPE, networktype);
    }

    private void UpdateDataState() {
        Int32 state = mTelephonyManager->GetDataState();
        String display = mRes->GetString(R::string::radioInfo_unknown);

        switch (state) {
            case TelephonyManager.DATA_CONNECTED:
                display = mRes->GetString(R::string::radioInfo_data_connected);
                break;
            case TelephonyManager.DATA_SUSPENDED:
                display = mRes->GetString(R::string::radioInfo_data_suspended);
                break;
            case TelephonyManager.DATA_CONNECTING:
                display = mRes->GetString(R::string::radioInfo_data_connecting);
                break;
            case TelephonyManager.DATA_DISCONNECTED:
                display = mRes->GetString(R::string::radioInfo_data_disconnected);
                break;
        }

        SetSummaryText(KEY_DATA_STATE, display);
    }

    private void UpdateServiceState(ServiceState serviceState) {
        Int32 state = serviceState->GetState();
        String display = mRes->GetString(R::string::radioInfo_unknown);

        switch (state) {
            case ServiceState.STATE_IN_SERVICE:
                display = mRes->GetString(R::string::radioInfo_service_in);
                break;
            case ServiceState.STATE_OUT_OF_SERVICE:
            case ServiceState.STATE_EMERGENCY_ONLY:
                display = mRes->GetString(R::string::radioInfo_service_out);
                break;
            case ServiceState.STATE_POWER_OFF:
                display = mRes->GetString(R::string::radioInfo_service_off);
                break;
        }

        SetSummaryText(KEY_SERVICE_STATE, display);

        if (serviceState->GetRoaming()) {
            SetSummaryText(KEY_ROAMING_STATE, mRes->GetString(R::string::radioInfo_roaming_in));
        } else {
            SetSummaryText(KEY_ROAMING_STATE, mRes->GetString(R::string::radioInfo_roaming_not));
        }
        SetSummaryText(KEY_OPERATOR_NAME, serviceState->GetOperatorAlphaLong());
    }

    private void UpdateAreaInfo(String areaInfo) {
        if (areaInfo != NULL) {
            SetSummaryText(KEY_LATEST_AREA_INFO, areaInfo);
        }
    }

    void UpdateSignalStrength() {
        // TODO PhoneStateIntentReceiver is deprecated and PhoneStateListener
        // should probably used instead.

        // not loaded in some versions of the code (e.g., zaku)
        if (mSignalStrength != NULL) {
            Int32 state =
                    mPhoneStateReceiver->GetServiceState()->GetState();
            Resources r = GetResources();

            if ((ServiceState.STATE_OUT_OF_SERVICE == state) ||
                    (ServiceState.STATE_POWER_OFF == state)) {
                mSignalStrength->SetSummary("0");
                return;
            }

            Int32 signalDbm = mPhoneStateReceiver->GetSignalStrengthDbm();

            if (-1 == signalDbm) signalDbm = 0;

            Int32 signalAsu = mPhoneStateReceiver->GetSignalStrengthLevelAsu();

            if (-1 == signalAsu) signalAsu = 0;

            mSignalStrength->SetSummary(String->ValueOf(signalDbm) + " "
                        + r->GetString(R::string::radioInfo_display_dbm) + "   "
                        + String->ValueOf(signalAsu) + " "
                        + r->GetString(R::string::radioInfo_display_asu));
        }
    }

    private void SetWimaxStatus() {
        if (mWimaxMacAddress != NULL) {
            String macAddress = SystemProperties->Get("net.wimax.mac.address", mUnavailable);
            mWimaxMacAddress->SetSummary(macAddress);
        }
    }

    private void SetWifiStatus() {
        WifiInfo wifiInfo = mWifiManager->GetConnectionInfo();
        String macAddress = wifiInfo == NULL ? NULL : wifiInfo->GetMacAddress();
        mWifiMacAddress->SetSummary(!TextUtils->IsEmpty(macAddress) ? macAddress : mUnavailable);
    }

    private void SetIpAddressStatus() {
        String ipAddress = Utils->GetDefaultIpAddresses(this.mCM);
        if (ipAddress != NULL) {
            mIpAddress->SetSummary(ipAddress);
        } else {
            mIpAddress->SetSummary(mUnavailable);
        }
    }

    private void SetBtStatus() {
        BluetoothAdapter bluetooth = BluetoothAdapter->GetDefaultAdapter();
        if (bluetooth != NULL && mBtAddress != NULL) {
            String address = bluetooth->IsEnabled() ? bluetooth->GetAddress() : NULL;
            if (!TextUtils->IsEmpty(address)) {
               // Convert the address to lowercase for consistency with the wifi MAC address.
                mBtAddress->SetSummary(address->ToLowerCase());
            } else {
                mBtAddress->SetSummary(mUnavailable);
            }
        }
    }

    void UpdateConnectivity() {
        SetWimaxStatus();
        SetWifiStatus();
        SetBtStatus();
        SetIpAddressStatus();
    }

    void UpdateTimes() {
        Int64 at = SystemClock->UptimeMillis() / 1000;
        Int64 ut = SystemClock->ElapsedRealtime() / 1000;

        if (ut == 0) {
            ut = 1;
        }

        mUptime->SetSummary(Convert(ut));
    }

    private String Pad(Int32 n) {
        if (n >= 10) {
            return String->ValueOf(n);
        } else {
            return "0" + String->ValueOf(n);
        }
    }

    private String Convert(Int64 t) {
        Int32 s = (Int32)(t % 60);
        Int32 m = (Int32)((t / 60) % 60);
        Int32 h = (Int32)((t / 3600));

        return h + ":" + Pad(m) + ":" + Pad(s);
    }
}
