/*
 * Copyright (C) 2006 The Android Open Source Project
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

package com.android.settings;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::ITrafficStats;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IAsyncResult;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Telephony::ICellInfo;
using Elastos::Droid::Telephony::ICellLocation;
using Elastos::Droid::Telephony::IDataConnectionRealTimeInfo;
using Elastos::Droid::Telephony::IPhoneStateListener;
using Elastos::Droid::Telephony::IServiceState;
using Elastos::Droid::Telephony::ITelephonyManager;
using Elastos::Droid::Telephony::INeighboringCellInfo;
using Elastos::Droid::Telephony::Cdma::ICdmaCellLocation;
using Elastos::Droid::Telephony::Gsm::IGsmCellLocation;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IEditText;

using Elastos::Droid::Internal::Telephony::IPhone;
using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::IPhoneFactory;
using Elastos::Droid::Internal::Telephony::IPhoneStateIntentReceiver;
using Elastos::Droid::Internal::Telephony::ITelephonyProperties;
using org::Apache::Http::IHttpResponse;
using org::Apache::Http::Client::IHttpClient;
using org::Apache::Http::Client::Methods::IHttpGet;
using org::Apache::Http::Impl::Client::IDefaultHttpClient;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;

public class RadioInfo extends Activity {
    private final String TAG = "phone";

    private static const Int32 EVENT_PHONE_STATE_CHANGED = 100;
    private static const Int32 EVENT_SIGNAL_STRENGTH_CHANGED = 200;
    private static const Int32 EVENT_SERVICE_STATE_CHANGED = 300;
    private static const Int32 EVENT_CFI_CHANGED = 302;

    private static const Int32 EVENT_QUERY_PREFERRED_TYPE_DONE = 1000;
    private static const Int32 EVENT_SET_PREFERRED_TYPE_DONE = 1001;
    private static const Int32 EVENT_QUERY_NEIGHBORING_CIDS_DONE = 1002;
    private static const Int32 EVENT_QUERY_SMSC_DONE = 1005;
    private static const Int32 EVENT_UPDATE_SMSC_DONE = 1006;

    private static const Int32 MENU_ITEM_SELECT_BAND  = 0;
    private static const Int32 MENU_ITEM_VIEW_ADN     = 1;
    private static const Int32 MENU_ITEM_VIEW_FDN     = 2;
    private static const Int32 MENU_ITEM_VIEW_SDN     = 3;
    private static const Int32 MENU_ITEM_GET_PDP_LIST = 4;
    private static const Int32 MENU_ITEM_TOGGLE_DATA  = 5;

    static const String ENABLE_DATA_STR = "Enable data connection";
    static const String DISABLE_DATA_STR = "Disable data connection";

    private TextView mDeviceId; //DeviceId is the IMEI in GSM and the MEID in CDMA
    private TextView number;
    private TextView callState;
    private TextView operatorName;
    private TextView roamingState;
    private TextView gsmState;
    private TextView gprsState;
    private TextView network;
    private TextView dBm;
    private TextView mMwi;
    private TextView mCfi;
    private TextView mLocation;
    private TextView mNeighboringCids;
    private TextView mCellInfo;
    private TextView mDcRtInfoTv;
    private TextView resets;
    private TextView attempts;
    private TextView successes;
    private TextView disconnects;
    private TextView sentSinceReceived;
    private TextView sent;
    private TextView received;
    private TextView mPingIpAddr;
    private TextView mPingHostname;
    private TextView mHttpClientTest;
    private TextView dnsCheckState;
    private EditText smsc;
    private Button radioPowerButton;
    private Button cellInfoListRateButton;
    private Button dnsCheckToggleButton;
    private Button pingTestButton;
    private Button updateSmscButton;
    private Button refreshSmscButton;
    private Button oemInfoButton;
    private Spinner preferredNetworkType;

    private TelephonyManager mTelephonyManager;
    private Phone phone = NULL;
    private PhoneStateIntentReceiver mPhoneStateReceiver;

    private String mPingIpAddrResult;
    private String mPingHostnameResult;
    private String mHttpClientTestResult;
    private Boolean mMwiValue = FALSE;
    private Boolean mCfiValue = FALSE;
    private List<CellInfo> mCellInfoValue;

    private PhoneStateListener mPhoneStateListener = new PhoneStateListener() {
        //@Override
        CARAPI OnDataConnectionStateChanged(Int32 state) {
            UpdateDataState();
            UpdateDataStats();
            UpdatePdpList();
            UpdateNetworkType();
        }

        //@Override
        CARAPI OnDataActivity(Int32 direction) {
            UpdateDataStats2();
        }

        //@Override
        CARAPI OnCellLocationChanged(CellLocation location) {
            UpdateLocation(location);
        }

        //@Override
        CARAPI OnMessageWaitingIndicatorChanged(Boolean mwi) {
            mMwiValue = mwi;
            UpdateMessageWaiting();
        }

        //@Override
        CARAPI OnCallForwardingIndicatorChanged(Boolean cfi) {
            mCfiValue = cfi;
            UpdateCallRedirect();
        }

        //@Override
        CARAPI OnCellInfoChanged(List<CellInfo> arrayCi) {
            Log("onCellInfoChanged: arrayCi=" + arrayCi);
            UpdateCellInfoTv(arrayCi);
        }

        //@Override
        CARAPI OnDataConnectionRealTimeInfoChanged(DataConnectionRealTimeInfo dcRtInfo) {
            Log("onDataConnectionRealTimeInfoChanged: dcRtInfo=" + dcRtInfo);
            UpdateDcRtInfoTv(dcRtInfo);
        }
    };

    private Handler mHandler = new Handler() {
        CARAPI HandleMessage(Message msg) {
            AsyncResult ar;
            switch (msg.what) {
                case EVENT_PHONE_STATE_CHANGED:
                    UpdatePhoneState();
                    break;

                case EVENT_SIGNAL_STRENGTH_CHANGED:
                    UpdateSignalStrength();
                    break;

                case EVENT_SERVICE_STATE_CHANGED:
                    UpdateServiceState();
                    UpdatePowerState();
                    break;

                case EVENT_QUERY_PREFERRED_TYPE_DONE:
                    ar= (AsyncResult) msg.obj;
                    if (ar.exception == NULL) {
                        Int32 type = ((Int32[])ar.result)[0];
                        if (type >= mPreferredNetworkLabels.length) {
                            Log("EVENT_QUERY_PREFERRED_TYPE_DONE: unknown " +
                                    "type=" + type);
                            type = mPreferredNetworkLabels.length - 1;
                        }
                        preferredNetworkType->SetSelection(type, TRUE);
                    } else {
                        preferredNetworkType->SetSelection(mPreferredNetworkLabels.length - 1, TRUE);
                    }
                    break;
                case EVENT_SET_PREFERRED_TYPE_DONE:
                    ar= (AsyncResult) msg.obj;
                    if (ar.exception != NULL) {
                        phone->GetPreferredNetworkType(
                                ObtainMessage(EVENT_QUERY_PREFERRED_TYPE_DONE));
                    }
                    break;
                case EVENT_QUERY_NEIGHBORING_CIDS_DONE:
                    ar= (AsyncResult) msg.obj;
                    if (ar.exception == NULL) {
                        UpdateNeighboringCids((ArrayList<NeighboringCellInfo>)ar.result);
                    } else {
                        mNeighboringCids->SetText("unknown");
                    }
                    break;
                case EVENT_QUERY_SMSC_DONE:
                    ar= (AsyncResult) msg.obj;
                    if (ar.exception != NULL) {
                        smsc->SetText("refresh error");
                    } else {
                        smsc->SetText((String)ar.result);
                    }
                    break;
                case EVENT_UPDATE_SMSC_DONE:
                    updateSmscButton->SetEnabled(TRUE);
                    ar= (AsyncResult) msg.obj;
                    if (ar.exception != NULL) {
                        smsc->SetText("update error");
                    }
                    break;
                default:
                    break;

            }
        }
    };

    //@Override
    CARAPI OnCreate(Bundle icicle) {
        super->OnCreate(icicle);

        SetContentView(R.layout.radio_info);

        mTelephonyManager = (TelephonyManager)GetSystemService(TELEPHONY_SERVICE);
        phone = PhoneFactory->GetDefaultPhone();

        mDeviceId= (TextView) FindViewById(R.id.imei);
        number = (TextView) FindViewById(R.id.number);
        callState = (TextView) FindViewById(R.id.call);
        operatorName = (TextView) FindViewById(R.id.operator);
        roamingState = (TextView) FindViewById(R.id.roaming);
        gsmState = (TextView) FindViewById(R.id.gsm);
        gprsState = (TextView) FindViewById(R.id.gprs);
        network = (TextView) FindViewById(R.id.network);
        dBm = (TextView) FindViewById(R.id.dbm);
        mMwi = (TextView) FindViewById(R.id.mwi);
        mCfi = (TextView) FindViewById(R.id.cfi);
        mLocation = (TextView) FindViewById(R.id.location);
        mNeighboringCids = (TextView) FindViewById(R.id.neighboring);
        mCellInfo = (TextView) FindViewById(R.id.cellinfo);
        mDcRtInfoTv = (TextView) FindViewById(R.id.dcrtinfo);

        resets = (TextView) FindViewById(R.id.resets);
        attempts = (TextView) FindViewById(R.id.attempts);
        successes = (TextView) FindViewById(R.id.successes);
        disconnects = (TextView) FindViewById(R.id.disconnects);
        sentSinceReceived = (TextView) FindViewById(R.id.sentSinceReceived);
        sent = (TextView) FindViewById(R.id.sent);
        received = (TextView) FindViewById(R.id.received);
        smsc = (EditText) FindViewById(R.id.smsc);
        dnsCheckState = (TextView) FindViewById(R.id.dnsCheckState);

        mPingIpAddr = (TextView) FindViewById(R.id.pingIpAddr);
        mPingHostname = (TextView) FindViewById(R.id.pingHostname);
        mHttpClientTest = (TextView) FindViewById(R.id.httpClientTest);

        preferredNetworkType = (Spinner) FindViewById(R.id.preferredNetworkType);
        ArrayAdapter<String> adapter = new ArrayAdapter<String> (this,
                android.R.layout.simple_spinner_item, mPreferredNetworkLabels);
        adapter->SetDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        preferredNetworkType->SetAdapter(adapter);
        preferredNetworkType->SetOnItemSelectedListener(mPreferredNetworkHandler);

        radioPowerButton = (Button) FindViewById(R.id.radio_power);
        radioPowerButton->SetOnClickListener(mPowerButtonHandler);

        cellInfoListRateButton = (Button) FindViewById(R.id.cell_info_list_rate);
        cellInfoListRateButton->SetOnClickListener(mCellInfoListRateHandler);

        imsRegRequiredButton = (Button) FindViewById(R.id.ims_reg_required);
        imsRegRequiredButton->SetOnClickListener(mImsRegRequiredHandler);

        smsOverImsButton = (Button) FindViewById(R.id.sms_over_ims);
        smsOverImsButton->SetOnClickListener(mSmsOverImsHandler);

        lteRamDumpButton = (Button) FindViewById(R.id.lte_ram_dump);
        lteRamDumpButton->SetOnClickListener(mLteRamDumpHandler);

        pingTestButton = (Button) FindViewById(R.id.ping_test);
        pingTestButton->SetOnClickListener(mPingButtonHandler);
        updateSmscButton = (Button) FindViewById(R.id.update_smsc);
        updateSmscButton->SetOnClickListener(mUpdateSmscButtonHandler);
        refreshSmscButton = (Button) FindViewById(R.id.refresh_smsc);
        refreshSmscButton->SetOnClickListener(mRefreshSmscButtonHandler);
        dnsCheckToggleButton = (Button) FindViewById(R.id.dns_check_toggle);
        dnsCheckToggleButton->SetOnClickListener(mDnsCheckButtonHandler);

        oemInfoButton = (Button) FindViewById(R.id.oem_info);
        oemInfoButton->SetOnClickListener(mOemInfoButtonHandler);
        PackageManager pm = GetPackageManager();
        Intent oemInfoIntent = new Intent("com.android.settings.OEM_RADIO_INFO");
        List<ResolveInfo> oemInfoIntentList = pm->QueryIntentActivities(oemInfoIntent, 0);
        if (oemInfoIntentList->Size() == 0) {
            oemInfoButton->SetEnabled(FALSE);
        }

        mPhoneStateReceiver = new PhoneStateIntentReceiver(this, mHandler);
        mPhoneStateReceiver->NotifySignalStrength(EVENT_SIGNAL_STRENGTH_CHANGED);
        mPhoneStateReceiver->NotifyServiceState(EVENT_SERVICE_STATE_CHANGED);
        mPhoneStateReceiver->NotifyPhoneCallState(EVENT_PHONE_STATE_CHANGED);

        phone->GetPreferredNetworkType(
                mHandler->ObtainMessage(EVENT_QUERY_PREFERRED_TYPE_DONE));
        phone->GetNeighboringCids(
                mHandler->ObtainMessage(EVENT_QUERY_NEIGHBORING_CIDS_DONE));

        CellLocation->RequestLocationUpdate();

        // Get current cell info
        mCellInfoValue = mTelephonyManager->GetAllCellInfo();
        Log("onCreate: mCellInfoValue=" + mCellInfoValue);
    }

    //@Override
    protected void OnResume() {
        super->OnResume();

        UpdatePhoneState();
        UpdateSignalStrength();
        UpdateMessageWaiting();
        UpdateCallRedirect();
        UpdateServiceState();
        UpdateLocation(mTelephonyManager->GetCellLocation());
        UpdateDataState();
        UpdateDataStats();
        UpdateDataStats2();
        UpdatePowerState();
        UpdateCellInfoListRate();
        UpdateImsRegRequiredState();
        UpdateSmsOverImsState();
        UpdateLteRamDumpState();
        UpdateProperties();
        UpdateDnsCheckState();

        Log("onResume: register phone & data intents");

        mPhoneStateReceiver->RegisterIntent();
        mTelephonyManager->Listen(mPhoneStateListener,
                  PhoneStateListener.LISTEN_DATA_CONNECTION_STATE
                | PhoneStateListener.LISTEN_DATA_ACTIVITY
                | PhoneStateListener.LISTEN_CELL_LOCATION
                | PhoneStateListener.LISTEN_MESSAGE_WAITING_INDICATOR
                | PhoneStateListener.LISTEN_CALL_FORWARDING_INDICATOR
                | PhoneStateListener.LISTEN_CELL_INFO
                | PhoneStateListener.LISTEN_DATA_CONNECTION_REAL_TIME_INFO);
    }

    //@Override
    CARAPI OnPause() {
        super->OnPause();

        Log("onPause: unregister phone & data intents");

        mPhoneStateReceiver->UnregisterIntent();
        mTelephonyManager->Listen(mPhoneStateListener, PhoneStateListener.LISTEN_NONE);
    }

    //@Override
    public Boolean OnCreateOptionsMenu(Menu menu) {
        menu->Add(0, MENU_ITEM_SELECT_BAND, 0, R::string::radio_info_band_mode_label)
                .SetOnMenuItemClickListener(mSelectBandCallback)
                .SetAlphabeticShortcut('b');
        menu->Add(1, MENU_ITEM_VIEW_ADN, 0,
                R::string::radioInfo_menu_viewADN).SetOnMenuItemClickListener(mViewADNCallback);
        menu->Add(1, MENU_ITEM_VIEW_FDN, 0,
                R::string::radioInfo_menu_viewFDN).SetOnMenuItemClickListener(mViewFDNCallback);
        menu->Add(1, MENU_ITEM_VIEW_SDN, 0,
                R::string::radioInfo_menu_viewSDN).SetOnMenuItemClickListener(mViewSDNCallback);
        menu->Add(1, MENU_ITEM_GET_PDP_LIST,
                0, R::string::radioInfo_menu_getPDP).SetOnMenuItemClickListener(mGetPdpList);
        menu->Add(1, MENU_ITEM_TOGGLE_DATA,
                0, DISABLE_DATA_STR).SetOnMenuItemClickListener(mToggleData);
        return TRUE;
    }

    //@Override
    public Boolean OnPrepareOptionsMenu(Menu menu) {
        // Get the TOGGLE DATA menu item in the right state.
        MenuItem item = menu->FindItem(MENU_ITEM_TOGGLE_DATA);
        Int32 state = mTelephonyManager->GetDataState();
        Boolean visible = TRUE;

        switch (state) {
            case TelephonyManager.DATA_CONNECTED:
            case TelephonyManager.DATA_SUSPENDED:
                item->SetTitle(DISABLE_DATA_STR);
                break;
            case TelephonyManager.DATA_DISCONNECTED:
                item->SetTitle(ENABLE_DATA_STR);
                break;
            default:
                visible = FALSE;
                break;
        }
        item->SetVisible(visible);
        return TRUE;
    }

    private Boolean IsRadioOn() {
        return phone->GetServiceState()->GetState() != ServiceState.STATE_POWER_OFF;
    }

    private void UpdatePowerState() {
        String buttonText = IsRadioOn() ?
                            GetString(R::string::turn_off_radio) :
                            GetString(R::string::turn_on_radio);
        radioPowerButton->SetText(buttonText);
    }

    private void UpdateCellInfoListRate() {
        cellInfoListRateButton->SetText("CellInfoListRate " + mCellInfoListRateHandler->GetRate());
        UpdateCellInfoTv(mTelephonyManager->GetAllCellInfo());
    }

    private void UpdateDnsCheckState() {
        dnsCheckState->SetText(phone->IsDnsCheckDisabled() ?
                "0.0.0.0 allowed" :"0.0.0.0 not allowed");
    }

    private final void
    UpdateSignalStrength() {
        // TODO PhoneStateIntentReceiver is deprecated and PhoneStateListener
        // should probably used instead.
        Int32 state = mPhoneStateReceiver->GetServiceState()->GetState();
        Resources r = GetResources();

        if ((ServiceState.STATE_OUT_OF_SERVICE == state) ||
                (ServiceState.STATE_POWER_OFF == state)) {
            dBm->SetText("0");
        }

        Int32 signalDbm = mPhoneStateReceiver->GetSignalStrengthDbm();

        if (-1 == signalDbm) signalDbm = 0;

        Int32 signalAsu = mPhoneStateReceiver->GetSignalStrengthLevelAsu();

        if (-1 == signalAsu) signalAsu = 0;

        dBm->SetText(String->ValueOf(signalDbm) + " "
            + r->GetString(R::string::radioInfo_display_dbm) + "   "
            + String->ValueOf(signalAsu) + " "
            + r->GetString(R::string::radioInfo_display_asu));
    }

    private final void UpdateLocation(CellLocation location) {
        Resources r = GetResources();
        if (location instanceof GsmCellLocation) {
            GsmCellLocation loc = (GsmCellLocation)location;
            Int32 lac = loc->GetLac();
            Int32 cid = loc->GetCid();
            mLocation->SetText(r->GetString(R::string::radioInfo_lac) + " = "
                    + ((lac == -1) ? "unknown" : Integer->ToHexString(lac))
                    + "   "
                    + r->GetString(R::string::radioInfo_cid) + " = "
                    + ((cid == -1) ? "unknown" : Integer->ToHexString(cid)));
        } else if (location instanceof CdmaCellLocation) {
            CdmaCellLocation loc = (CdmaCellLocation)location;
            Int32 bid = loc->GetBaseStationId();
            Int32 sid = loc->GetSystemId();
            Int32 nid = loc->GetNetworkId();
            Int32 lat = loc->GetBaseStationLatitude();
            Int32 lon = loc->GetBaseStationLongitude();
            mLocation->SetText("BID = "
                    + ((bid == -1) ? "unknown" : Integer->ToHexString(bid))
                    + "   "
                    + "SID = "
                    + ((sid == -1) ? "unknown" : Integer->ToHexString(sid))
                    + "   "
                    + "NID = "
                    + ((nid == -1) ? "unknown" : Integer->ToHexString(nid))
                    + "\n"
                    + "LAT = "
                    + ((lat == -1) ? "unknown" : Integer->ToHexString(lat))
                    + "   "
                    + "LONG = "
                    + ((lon == -1) ? "unknown" : Integer->ToHexString(lon)));
        } else {
            mLocation->SetText("unknown");
        }


    }

    private final void UpdateNeighboringCids(ArrayList<NeighboringCellInfo> cids) {
        StringBuilder sb = new StringBuilder();

        if (cids != NULL) {
            if ( cids->IsEmpty() ) {
                sb->Append("no neighboring cells");
            } else {
                for (NeighboringCellInfo cell : cids) {
                    sb->Append(cell->ToString()).Append(" ");
                }
            }
        } else {
            sb->Append("unknown");
        }
        mNeighboringCids->SetText(sb->ToString());
    }

    private final void UpdateCellInfoTv(List<CellInfo> arrayCi) {
        mCellInfoValue = arrayCi;
        StringBuilder value = new StringBuilder();
        if (mCellInfoValue != NULL) {
            Int32 index = 0;
            for (CellInfo ci : mCellInfoValue) {
                value->AppendChar32('[');
                value->Append(index);
                value->Append("]=");
                value->Append(ci->ToString());
                if (++index < mCellInfoValue->Size()) {
                    value->Append("\n");
                }
            }
        }
        mCellInfo->SetText(value->ToString());
    }

    private final void UpdateDcRtInfoTv(DataConnectionRealTimeInfo dcRtInfo) {
        mDcRtInfoTv->SetText(dcRtInfo->ToString());
    }

    private final void
    UpdateMessageWaiting() {
        mMwi->SetText(String->ValueOf(mMwiValue));
    }

    private final void
    UpdateCallRedirect() {
        mCfi->SetText(String->ValueOf(mCfiValue));
    }


    private final void
    UpdateServiceState() {
        ServiceState serviceState = mPhoneStateReceiver->GetServiceState();
        Int32 state = serviceState->GetState();
        Resources r = GetResources();
        String display = r->GetString(R::string::radioInfo_unknown);

        switch (state) {
            case ServiceState.STATE_IN_SERVICE:
                display = r->GetString(R::string::radioInfo_service_in);
                break;
            case ServiceState.STATE_OUT_OF_SERVICE:
            case ServiceState.STATE_EMERGENCY_ONLY:
                display = r->GetString(R::string::radioInfo_service_emergency);
                break;
            case ServiceState.STATE_POWER_OFF:
                display = r->GetString(R::string::radioInfo_service_off);
                break;
        }

        gsmState->SetText(display);

        if (serviceState->GetRoaming()) {
            roamingState->SetText(R::string::radioInfo_roaming_in);
        } else {
            roamingState->SetText(R::string::radioInfo_roaming_not);
        }

        operatorName->SetText(serviceState->GetOperatorAlphaLong());
    }

    private final void
    UpdatePhoneState() {
        PhoneConstants.State state = mPhoneStateReceiver->GetPhoneState();
        Resources r = GetResources();
        String display = r->GetString(R::string::radioInfo_unknown);

        switch (state) {
            case IDLE:
                display = r->GetString(R::string::radioInfo_phone_idle);
                break;
            case RINGING:
                display = r->GetString(R::string::radioInfo_phone_ringing);
                break;
            case OFFHOOK:
                display = r->GetString(R::string::radioInfo_phone_offhook);
                break;
        }

        callState->SetText(display);
    }

    private final void
    UpdateDataState() {
        Int32 state = mTelephonyManager->GetDataState();
        Resources r = GetResources();
        String display = r->GetString(R::string::radioInfo_unknown);

        switch (state) {
            case TelephonyManager.DATA_CONNECTED:
                display = r->GetString(R::string::radioInfo_data_connected);
                break;
            case TelephonyManager.DATA_CONNECTING:
                display = r->GetString(R::string::radioInfo_data_connecting);
                break;
            case TelephonyManager.DATA_DISCONNECTED:
                display = r->GetString(R::string::radioInfo_data_disconnected);
                break;
            case TelephonyManager.DATA_SUSPENDED:
                display = r->GetString(R::string::radioInfo_data_suspended);
                break;
        }

        gprsState->SetText(display);
    }

    private final void UpdateNetworkType() {
        Resources r = GetResources();
        String display = SystemProperties->Get(TelephonyProperties.PROPERTY_DATA_NETWORK_TYPE,
                r->GetString(R::string::radioInfo_unknown));

        network->SetText(display);
    }

    private final void
    UpdateProperties() {
        String s;
        Resources r = GetResources();

        s = phone->GetDeviceId();
        if (s == NULL) s = r->GetString(R::string::radioInfo_unknown);
        mDeviceId->SetText(s);


        s = phone->GetLine1Number();
        if (s == NULL) s = r->GetString(R::string::radioInfo_unknown);
        number->SetText(s);
    }

    private final void UpdateDataStats() {
        String s;

        s = SystemProperties->Get("net.gsm.radio-reset", "0");
        resets->SetText(s);

        s = SystemProperties->Get("net.gsm.attempt-gprs", "0");
        attempts->SetText(s);

        s = SystemProperties->Get("net.gsm.succeed-gprs", "0");
        successes->SetText(s);

        //s = SystemProperties->Get("net.gsm.disconnect", "0");
        //disconnects->SetText(s);

        s = SystemProperties->Get("net.ppp.reset-by-timeout", "0");
        sentSinceReceived->SetText(s);
    }

    private final void UpdateDataStats2() {
        Resources r = GetResources();

        Int64 txPackets = TrafficStats->GetMobileTxPackets();
        Int64 rxPackets = TrafficStats->GetMobileRxPackets();
        Int64 txBytes   = TrafficStats->GetMobileTxBytes();
        Int64 rxBytes   = TrafficStats->GetMobileRxBytes();

        String packets = r->GetString(R::string::radioInfo_display_packets);
        String bytes   = r->GetString(R::string::radioInfo_display_bytes);

        sent->SetText(txPackets + " " + packets + ", " + txBytes + " " + bytes);
        received->SetText(rxPackets + " " + packets + ", " + rxBytes + " " + bytes);
    }

    /**
     * Ping a IP address.
     */
    private final void PingIpAddr() {
        try {
            // This is hardcoded IP addr. This is for testing purposes.
            // We would need to get rid of this before release.
            String ipAddress = "74.125.47.104";
            Process p = Runtime->GetRuntime()->Exec("ping -c 1 " + ipAddress);
            Int32 status = p->WaitFor();
            if (status == 0) {
                mPingIpAddrResult = "Pass";
            } else {
                mPingIpAddrResult = "Fail: IP addr not reachable";
            }
        } catch (IOException e) {
            mPingIpAddrResult = "Fail: IOException";
        } catch (InterruptedException e) {
            mPingIpAddrResult = "Fail: InterruptedException";
        }
    }

    /**
     *  Ping a host name
     */
    private final void PingHostname() {
        try {
            Process p = Runtime->GetRuntime()->Exec("ping -c 1 www.google.com");
            Int32 status = p->WaitFor();
            if (status == 0) {
                mPingHostnameResult = "Pass";
            } else {
                mPingHostnameResult = "Fail: Host unreachable";
            }
        } catch (UnknownHostException e) {
            mPingHostnameResult = "Fail: Unknown Host";
        } catch (IOException e) {
            mPingHostnameResult= "Fail: IOException";
        } catch (InterruptedException e) {
            mPingHostnameResult = "Fail: InterruptedException";
        }
    }

    /**
     * This function checks for basic functionality of HTTP Client.
     */
    private void HttpClientTest() {
        HttpClient client = new DefaultHttpClient();
        try {
            HttpGet request = new HttpGet("http://www.google.com");
            HttpResponse response = client->Execute(request);
            if (response->GetStatusLine()->GetStatusCode() == 200) {
                mHttpClientTestResult = "Pass";
            } else {
                mHttpClientTestResult = "Fail: Code: " + String->ValueOf(response);
            }
            request->Abort();
        } catch (IOException e) {
            mHttpClientTestResult = "Fail: IOException";
        }
    }

    private void RefreshSmsc() {
        phone->GetSmscAddress(mHandler->ObtainMessage(EVENT_QUERY_SMSC_DONE));
    }

    private final void UpdatePingState() {
        final Handler handler = new Handler();
        // Set all to unknown since the threads will take a few secs to update.
        mPingIpAddrResult = GetResources()->GetString(R::string::radioInfo_unknown);
        mPingHostnameResult = GetResources()->GetString(R::string::radioInfo_unknown);
        mHttpClientTestResult = GetResources()->GetString(R::string::radioInfo_unknown);

        mPingIpAddr->SetText(mPingIpAddrResult);
        mPingHostname->SetText(mPingHostnameResult);
        mHttpClientTest->SetText(mHttpClientTestResult);

        final Runnable updatePingResults = new Runnable() {
            CARAPI Run() {
                mPingIpAddr->SetText(mPingIpAddrResult);
                mPingHostname->SetText(mPingHostnameResult);
                mHttpClientTest->SetText(mHttpClientTestResult);
            }
        };
        Thread ipAddr = new Thread() {
            //@Override
            CARAPI Run() {
                PingIpAddr();
                handler->Post(updatePingResults);
            }
        };
        ipAddr->Start();

        Thread hostname = new Thread() {
            //@Override
            CARAPI Run() {
                PingHostname();
                handler->Post(updatePingResults);
            }
        };
        hostname->Start();

        Thread httpClient = new Thread() {
            //@Override
            CARAPI Run() {
                HttpClientTest();
                handler->Post(updatePingResults);
            }
        };
        httpClient->Start();
    }

    private final void UpdatePdpList() {
        StringBuilder sb = new StringBuilder("========DATA=======\n");

//        List<DataConnection> dcs = phone->GetCurrentDataConnectionList();
//
//        for (DataConnection dc : dcs) {
//            sb->Append("    State=").Append(dc->GetStateAsString()).Append("\n");
//            if (dc->IsActive()) {
//                Int64 timeElapsed =
//                    (System->CurrentTimeMillis() - dc->GetConnectionTime())/1000;
//                sb->Append("    connected at ")
//                  .Append(DateUtils->TimeString(dc->GetConnectionTime()))
//                  .Append(" and elapsed ")
//                  .Append(DateUtils->FormatElapsedTime(timeElapsed));
//
//                if (dc instanceof GsmDataConnection) {
//                    GsmDataConnection pdp = (GsmDataConnection)dc;
//                    sb->Append("\n    to ")
//                      .Append(pdp->GetApn()->ToString());
//                }
//                sb->Append("\nLinkProperties: ");
//                sb->Append(phone->GetLinkProperties(phone->GetActiveApnTypes()[0]).ToString());
//            } else if (dc->IsInactive()) {
//                sb->Append("    disconnected with last try at ")
//                  .Append(DateUtils->TimeString(dc->GetLastFailTime()))
//                  .Append("\n    fail because ")
//                  .Append(dc->GetLastFailCause()->ToString());
//            } else {
//                if (dc instanceof GsmDataConnection) {
//                    GsmDataConnection pdp = (GsmDataConnection)dc;
//                    sb->Append("    is connecting to ")
//                      .Append(pdp->GetApn()->ToString());
//                } else {
//                    sb->Append("    is connecting");
//                }
//            }
//            sb->Append("\n===================");
//        }

        disconnects->SetText(sb->ToString());
    }

    private MenuItem.OnMenuItemClickListener mViewADNCallback = new MenuItem->OnMenuItemClickListener() {
        public Boolean OnMenuItemClick(MenuItem item) {
            Intent intent = new Intent(IIntent::ACTION_VIEW);
            // XXX We need to specify the component here because if we don't
            // the activity manager will try to resolve the type by calling
            // the content provider, which causes it to be loaded in a process
            // other than the Dialer process, which causes a lot of stuff to
            // break.
            intent->SetClassName("com.android.phone",
                    "com.android.phone.SimContacts");
            StartActivity(intent);
            return TRUE;
        }
    };

    private MenuItem.OnMenuItemClickListener mViewFDNCallback = new MenuItem->OnMenuItemClickListener() {
        public Boolean OnMenuItemClick(MenuItem item) {
            Intent intent = new Intent(IIntent::ACTION_VIEW);
            // XXX We need to specify the component here because if we don't
            // the activity manager will try to resolve the type by calling
            // the content provider, which causes it to be loaded in a process
            // other than the Dialer process, which causes a lot of stuff to
            // break.
            intent->SetClassName("com.android.phone",
                    "com.android.phone.FdnList");
            StartActivity(intent);
            return TRUE;
        }
    };

    private MenuItem.OnMenuItemClickListener mViewSDNCallback = new MenuItem->OnMenuItemClickListener() {
        public Boolean OnMenuItemClick(MenuItem item) {
            Intent intent = new Intent(
                    IIntent::ACTION_VIEW, Uri->Parse("content://icc/sdn"));
            // XXX We need to specify the component here because if we don't
            // the activity manager will try to resolve the type by calling
            // the content provider, which causes it to be loaded in a process
            // other than the Dialer process, which causes a lot of stuff to
            // break.
            intent->SetClassName("com.android.phone",
                    "com.android.phone.ADNList");
            StartActivity(intent);
            return TRUE;
        }
    };

    private MenuItem.OnMenuItemClickListener mGetPdpList = new MenuItem->OnMenuItemClickListener() {
        public Boolean OnMenuItemClick(MenuItem item) {
            phone->GetDataCallList(NULL);
            return TRUE;
        }
    };

    private MenuItem.OnMenuItemClickListener mSelectBandCallback = new MenuItem->OnMenuItemClickListener() {
        public Boolean OnMenuItemClick(MenuItem item) {
            Intent intent = new Intent();
            intent->SetClass(RadioInfo.this, BandMode.class);
            StartActivity(intent);
            return TRUE;
        }
    };

    private MenuItem.OnMenuItemClickListener mToggleData = new MenuItem->OnMenuItemClickListener() {
        public Boolean OnMenuItemClick(MenuItem item) {
            Int32 state = mTelephonyManager->GetDataState();
            switch (state) {
                case TelephonyManager.DATA_CONNECTED:
                    phone->SetDataEnabled(FALSE);
                    break;
                case TelephonyManager.DATA_DISCONNECTED:
                    phone->SetDataEnabled(TRUE);
                    break;
                default:
                    // do nothing
                    break;
            }
            return TRUE;
        }
    };

    OnClickListener mPowerButtonHandler = new OnClickListener() {
        CARAPI OnClick(View v) {
            //Log("toggle radio power: currently " + (IsRadioOn()?"on":"off"));
            phone->SetRadioPower(!IsRadioOn());
        }
    };

    class CellInfoListRateHandler implements OnClickListener {
        Int32 rates[] = {Integer.MAX_VALUE, 0, 1000};
        Int32 index = 0;

        public Int32 GetRate() {
            return rates[index];
        }

        //@Override
        CARAPI OnClick(View v) {
            index += 1;
            if (index >= rates.length) {
                index = 0;
            }
            phone->SetCellInfoListRate(rates[index]);
            UpdateCellInfoListRate();
        }
    }
    CellInfoListRateHandler mCellInfoListRateHandler = new CellInfoListRateHandler();

    private Button imsRegRequiredButton;
    static const String PROPERTY_IMS_REG_REQUIRED = "persist.radio.imsregrequired";
    OnClickListener mImsRegRequiredHandler = new OnClickListener() {
        //@Override
        CARAPI OnClick(View v) {
            Log(String->Format("toggle %s: currently %s",
                PROPERTY_IMS_REG_REQUIRED, (IsImsRegRequired() ? "on":"off")));
            Boolean newValue = !IsImsRegRequired();
            SystemProperties->Set(PROPERTY_IMS_REG_REQUIRED,
                    newValue ? "1":"0");
            UpdateImsRegRequiredState();
        }
    };

    private Boolean IsImsRegRequired() {
        return SystemProperties->GetBoolean(PROPERTY_IMS_REG_REQUIRED, FALSE);
    }

    private void UpdateImsRegRequiredState() {
        Log("updateImsRegRequiredState IsImsRegRequired()=" + IsImsRegRequired());
        String buttonText = IsImsRegRequired() ?
                            GetString(R::string::ims_reg_required_off) :
                            GetString(R::string::ims_reg_required_on);
        imsRegRequiredButton->SetText(buttonText);
    }

    private Button smsOverImsButton;
    static const String PROPERTY_SMS_OVER_IMS = "persist.radio.imsallowmtsms";
    OnClickListener mSmsOverImsHandler = new OnClickListener() {
        //@Override
        CARAPI OnClick(View v) {
            Log(String->Format("toggle %s: currently %s",
                    PROPERTY_SMS_OVER_IMS, (IsSmsOverImsEnabled() ? "on":"off")));
            Boolean newValue = !IsSmsOverImsEnabled();
            SystemProperties->Set(PROPERTY_SMS_OVER_IMS, newValue ? "1":"0");
            UpdateSmsOverImsState();
        }
    };

    private Boolean IsSmsOverImsEnabled() {
        return SystemProperties->GetBoolean(PROPERTY_SMS_OVER_IMS, FALSE);
    }

    private void UpdateSmsOverImsState() {
        Log("updateSmsOverImsState IsSmsOverImsEnabled()=" + IsSmsOverImsEnabled());
        String buttonText = IsSmsOverImsEnabled() ?
                            GetString(R::string::sms_over_ims_off) :
                            GetString(R::string::sms_over_ims_on);
        smsOverImsButton->SetText(buttonText);
    }

    private Button lteRamDumpButton;
    static const String PROPERTY_LTE_RAM_DUMP = "persist.radio.ramdump";
    OnClickListener mLteRamDumpHandler = new OnClickListener() {
        //@Override
        CARAPI OnClick(View v) {
            Log(String->Format("toggle %s: currently %s",
                    PROPERTY_LTE_RAM_DUMP, (IsSmsOverImsEnabled() ? "on":"off")));
            Boolean newValue = !IsLteRamDumpEnabled();
            SystemProperties->Set(PROPERTY_LTE_RAM_DUMP, newValue ? "1":"0");
            UpdateLteRamDumpState();
        }
    };

    private Boolean IsLteRamDumpEnabled() {
        return SystemProperties->GetBoolean(PROPERTY_LTE_RAM_DUMP, FALSE);
    }

    private void UpdateLteRamDumpState() {
        Log("updateLteRamDumpState IsLteRamDumpEnabled()=" + IsLteRamDumpEnabled());
        String buttonText = IsLteRamDumpEnabled() ?
                            GetString(R::string::lte_ram_dump_off) :
                            GetString(R::string::lte_ram_dump_on);
        lteRamDumpButton->SetText(buttonText);
    }

    OnClickListener mDnsCheckButtonHandler = new OnClickListener() {
        CARAPI OnClick(View v) {
            phone->DisableDnsCheck(!phone->IsDnsCheckDisabled());
            UpdateDnsCheckState();
        }
    };

    OnClickListener mOemInfoButtonHandler = new OnClickListener() {
        CARAPI OnClick(View v) {
            Intent intent = new Intent("com.android.settings.OEM_RADIO_INFO");
            try {
                StartActivity(intent);
            } catch (android.content.ActivityNotFoundException ex) {
                Log("OEM-specific Info/Settings Activity Not Found : " + ex);
                // If the activity does not exist, there are no OEM
                // settings, and so we can just do nothing...
            }
        }
    };

    OnClickListener mPingButtonHandler = new OnClickListener() {
        CARAPI OnClick(View v) {
            UpdatePingState();
        }
    };

    OnClickListener mUpdateSmscButtonHandler = new OnClickListener() {
        CARAPI OnClick(View v) {
            updateSmscButton->SetEnabled(FALSE);
            phone->SetSmscAddress(smsc->GetText()->ToString(),
                    mHandler->ObtainMessage(EVENT_UPDATE_SMSC_DONE));
        }
    };

    OnClickListener mRefreshSmscButtonHandler = new OnClickListener() {
        CARAPI OnClick(View v) {
            RefreshSmsc();
        }
    };

    AdapterView.OnItemSelectedListener
            mPreferredNetworkHandler = new AdapterView->OnItemSelectedListener() {
        CARAPI OnItemSelected(AdapterView parent, View v, Int32 pos, Int64 id) {
            Message msg = mHandler->ObtainMessage(EVENT_SET_PREFERRED_TYPE_DONE);
            if (pos>=0 && pos<=(mPreferredNetworkLabels.length - 2)) {
                phone->SetPreferredNetworkType(pos, msg);
            }
        }

        CARAPI OnNothingSelected(AdapterView parent) {
        }
    };

    private String[] mPreferredNetworkLabels = {
            "WCDMA preferred",
            "GSM only",
            "WCDMA only",
            "GSM auto (PRL)",
            "CDMA auto (PRL)",
            "CDMA only",
            "EvDo only",
            "GSM/CDMA auto (PRL)",
            "LTE/CDMA auto (PRL)",
            "LTE/GSM auto (PRL)",
            "LTE/GSM/CDMA auto (PRL)",
            "LTE only",
            "Unknown"};

    private void Log(String s) {
        Logger::D(TAG, "[RadioInfo] " + s);
    }
}
