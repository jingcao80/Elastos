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

/*
 * Copyright (C) 2009 The Android Open Source Project
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

using Elastos::Droid::Settings::IR;
using Elastos::Droid::Net::Wifi::IScanResult;
using org::Apache::Http::IHttpResponse;
using org::Apache::Http::Client::IHttpClient;
using org::Apache::Http::Client::Methods::IHttpGet;
using org::Apache::Http::Impl::Client::IDefaultHttpClient;
using Elastos::Utility::IList;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Net::INetworkInfo;
using Elastos::Droid::Net::Wifi::ISupplicantState;
using Elastos::Droid::Net::Wifi::IWifiInfo;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::View::IOnClickListener;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ITextView;


/**
 * Show the current status details of Wifi related fields
 */
public class WifiStatusTest extends Activity {

    private static const String TAG = "WifiStatusTest";

    private Button updateButton;
    private TextView mWifiState;
    private TextView mNetworkState;
    private TextView mSupplicantState;
    private TextView mRSSI;
    private TextView mBSSID;
    private TextView mSSID;
    private TextView mHiddenSSID;
    private TextView mIPAddr;
    private TextView mMACAddr;
    private TextView mNetworkId;
    private TextView mLinkSpeed;
    private TextView mScanList;


    private TextView mPingIpAddr;
    private TextView mPingHostname;
    private TextView mHttpClientTest;
    private Button pingTestButton;

    private String mPingIpAddrResult;
    private String mPingHostnameResult;
    private String mHttpClientTestResult;


    private WifiManager mWifiManager;
    private IntentFilter mWifiStateFilter;


    //============================
    // Activity lifecycle
    //============================

    private final BroadcastReceiver mWifiStateReceiver = new BroadcastReceiver() {
        //@Override
        CARAPI OnReceive(Context context, Intent intent) {
            if (intent->GetAction()->Equals(WifiManager.WIFI_STATE_CHANGED_ACTION)) {
                HandleWifiStateChanged(intent->GetIntExtra(WifiManager.EXTRA_WIFI_STATE,
                            WifiManager.WIFI_STATE_UNKNOWN));
            } else if (intent->GetAction()->Equals(WifiManager.NETWORK_STATE_CHANGED_ACTION)) {
                HandleNetworkStateChanged(
                        (NetworkInfo) intent->GetParcelableExtra(WifiManager.EXTRA_NETWORK_INFO));
            } else if (intent->GetAction()->Equals(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION)) {
                HandleScanResultsAvailable();
            } else if (intent->GetAction()->Equals(WifiManager.SUPPLICANT_CONNECTION_CHANGE_ACTION)) {
                /* TODO: handle supplicant connection change later */
            } else if (intent->GetAction()->Equals(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION)) {
                HandleSupplicantStateChanged(
                        (SupplicantState) intent->GetParcelableExtra(WifiManager.EXTRA_NEW_STATE),
                        intent->HasExtra(WifiManager.EXTRA_SUPPLICANT_ERROR),
                        intent->GetIntExtra(WifiManager.EXTRA_SUPPLICANT_ERROR, 0));
            } else if (intent->GetAction()->Equals(WifiManager.RSSI_CHANGED_ACTION)) {
                HandleSignalChanged(intent->GetIntExtra(WifiManager.EXTRA_NEW_RSSI, 0));
            } else if (intent->GetAction()->Equals(WifiManager.NETWORK_IDS_CHANGED_ACTION)) {
                /* TODO: handle network id change info later */
            } else {
                Logger::E(TAG, "Received an unknown Wifi Intent");
            }
        }
    };

    //@Override
    protected void OnCreate(Bundle savedInstanceState) {
        super->OnCreate(savedInstanceState);

        mWifiManager = (WifiManager) GetSystemService(WIFI_SERVICE);

        mWifiStateFilter = new IntentFilter(WifiManager.WIFI_STATE_CHANGED_ACTION);
        mWifiStateFilter->AddAction(WifiManager.NETWORK_STATE_CHANGED_ACTION);
        mWifiStateFilter->AddAction(WifiManager.SCAN_RESULTS_AVAILABLE_ACTION);
        mWifiStateFilter->AddAction(WifiManager.SUPPLICANT_STATE_CHANGED_ACTION);
        mWifiStateFilter->AddAction(WifiManager.RSSI_CHANGED_ACTION);
        mWifiStateFilter->AddAction(WifiManager.WIFI_STATE_CHANGED_ACTION);

        RegisterReceiver(mWifiStateReceiver, mWifiStateFilter);

        SetContentView(R.layout.wifi_status_test);

        updateButton = (Button) FindViewById(R.id.update);
        updateButton->SetOnClickListener(updateButtonHandler);

        mWifiState = (TextView) FindViewById(R.id.wifi_state);
        mNetworkState = (TextView) FindViewById(R.id.network_state);
        mSupplicantState = (TextView) FindViewById(R.id.supplicant_state);
        mRSSI = (TextView) FindViewById(R.id.rssi);
        mBSSID = (TextView) FindViewById(R.id.bssid);
        mSSID = (TextView) FindViewById(R.id.ssid);
        mHiddenSSID = (TextView) FindViewById(R.id.hidden_ssid);
        mIPAddr = (TextView) FindViewById(R.id.ipaddr);
        mMACAddr = (TextView) FindViewById(R.id.macaddr);
        mNetworkId = (TextView) FindViewById(R.id.networkid);
        mLinkSpeed = (TextView) FindViewById(R.id.link_speed);
        mScanList = (TextView) FindViewById(R.id.scan_list);


        mPingIpAddr = (TextView) FindViewById(R.id.pingIpAddr);
        mPingHostname = (TextView) FindViewById(R.id.pingHostname);
        mHttpClientTest = (TextView) FindViewById(R.id.httpClientTest);

        pingTestButton = (Button) FindViewById(R.id.ping_test);
        pingTestButton->SetOnClickListener(mPingButtonHandler);
    }

    //@Override
    protected void OnResume() {
        super->OnResume();
        RegisterReceiver(mWifiStateReceiver, mWifiStateFilter);
    }

    //@Override
    protected void OnPause() {
        super->OnPause();
        UnregisterReceiver(mWifiStateReceiver);
    }

    OnClickListener mPingButtonHandler = new OnClickListener() {
        CARAPI OnClick(View v) {
            UpdatePingState();
        }
    };

    OnClickListener updateButtonHandler = new OnClickListener() {
        CARAPI OnClick(View v) {
            final WifiInfo wifiInfo = mWifiManager->GetConnectionInfo();

            SetWifiStateText(mWifiManager->GetWifiState());
            mBSSID->SetText(wifiInfo->GetBSSID());
            mHiddenSSID->SetText(String->ValueOf(wifiInfo->GetHiddenSSID()));
            Int32 ipAddr = wifiInfo->GetIpAddress();
            StringBuffer ipBuf = new StringBuffer();
            ipBuf->Append(ipAddr  & 0xff).AppendChar32('.').
                Append((ipAddr >>>= 8) & 0xff).AppendChar32('.').
                Append((ipAddr >>>= 8) & 0xff).AppendChar32('.').
                Append((ipAddr >>>= 8) & 0xff);

            mIPAddr->SetText(ipBuf);
            mLinkSpeed->SetText(String->ValueOf(wifiInfo->GetLinkSpeed())+" Mbps");
            mMACAddr->SetText(wifiInfo->GetMacAddress());
            mNetworkId->SetText(String->ValueOf(wifiInfo->GetNetworkId()));
            mRSSI->SetText(String->ValueOf(wifiInfo->GetRssi()));
            mSSID->SetText(wifiInfo->GetSSID());

            SupplicantState supplicantState = wifiInfo->GetSupplicantState();
            SetSupplicantStateText(supplicantState);
        }
    };

    private void SetSupplicantStateText(SupplicantState supplicantState) {
        If(SupplicantState.FOUR_WAY_HANDSHAKE->Equals(supplicantState)) {
            mSupplicantState->SetText("FOUR WAY HANDSHAKE");
        } else If(SupplicantState.ASSOCIATED->Equals(supplicantState)) {
            mSupplicantState->SetText("ASSOCIATED");
        } else If(SupplicantState.ASSOCIATING->Equals(supplicantState)) {
            mSupplicantState->SetText("ASSOCIATING");
        } else If(SupplicantState.COMPLETED->Equals(supplicantState)) {
            mSupplicantState->SetText("COMPLETED");
        } else If(SupplicantState.DISCONNECTED->Equals(supplicantState)) {
            mSupplicantState->SetText("DISCONNECTED");
        } else If(SupplicantState.DORMANT->Equals(supplicantState)) {
            mSupplicantState->SetText("DORMANT");
        } else If(SupplicantState.GROUP_HANDSHAKE->Equals(supplicantState)) {
            mSupplicantState->SetText("GROUP HANDSHAKE");
        } else If(SupplicantState.INACTIVE->Equals(supplicantState)) {
            mSupplicantState->SetText("INACTIVE");
        } else If(SupplicantState.INVALID->Equals(supplicantState)) {
            mSupplicantState->SetText("INVALID");
        } else If(SupplicantState.SCANNING->Equals(supplicantState)) {
            mSupplicantState->SetText("SCANNING");
        } else If(SupplicantState.UNINITIALIZED->Equals(supplicantState)) {
            mSupplicantState->SetText("UNINITIALIZED");
        } else {
            mSupplicantState->SetText("BAD");
            Logger::E(TAG, "supplicant state is bad");
        }
    }

    private void SetWifiStateText(Int32 wifiState) {
        String wifiStateString;
        Switch(wifiState) {
            case WifiManager.WIFI_STATE_DISABLING:
                wifiStateString = GetString(R::string::wifi_state_disabling);
                break;
            case WifiManager.WIFI_STATE_DISABLED:
                wifiStateString = GetString(R::string::wifi_state_disabled);
                break;
            case WifiManager.WIFI_STATE_ENABLING:
                wifiStateString = GetString(R::string::wifi_state_enabling);
                break;
            case WifiManager.WIFI_STATE_ENABLED:
                wifiStateString = GetString(R::string::wifi_state_enabled);
                break;
            case WifiManager.WIFI_STATE_UNKNOWN:
                wifiStateString = GetString(R::string::wifi_state_unknown);
                break;
            default:
                wifiStateString = "BAD";
                Logger::E(TAG, "wifi state is bad");
                break;
        }

        mWifiState->SetText(wifiStateString);
    }

    private void HandleSignalChanged(Int32 rssi) {
        mRSSI->SetText(String->ValueOf(rssi));
    }

    private void HandleWifiStateChanged(Int32 wifiState) {
        SetWifiStateText(wifiState);
    }

    private void HandleScanResultsAvailable() {
        List<ScanResult> list = mWifiManager->GetScanResults();

        StringBuffer scanList = new StringBuffer();
        if (list != NULL) {
            for (Int32 i = list->Size() - 1; i >= 0; i--) {
                final ScanResult scanResult = list->Get(i);

                if (scanResult == NULL) {
                    continue;
                }

                if (TextUtils->IsEmpty(scanResult.SSID)) {
                    continue;
                }

                scanList->Append(scanResult.SSID+" ");
            }
        }
        mScanList->SetText(scanList);
    }

    private void HandleSupplicantStateChanged(SupplicantState state, Boolean hasError, Int32 error) {
        if (hasError) {
            mSupplicantState->SetText("ERROR AUTHENTICATING");
        } else {
            SetSupplicantStateText(state);
        }
    }

    private void HandleNetworkStateChanged(NetworkInfo networkInfo) {
        if (mWifiManager->IsWifiEnabled()) {
            String summary = Summary->Get(this, mWifiManager->GetConnectionInfo()->GetSSID(),
                    networkInfo->GetDetailedState());
            mNetworkState->SetText(summary);
        }
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
        Thread ipAddrThread = new Thread() {
            //@Override
            CARAPI Run() {
                PingIpAddr();
                handler->Post(updatePingResults);
            }
        };
        ipAddrThread->Start();

        Thread hostnameThread = new Thread() {
            //@Override
            CARAPI Run() {
                PingHostname();
                handler->Post(updatePingResults);
            }
        };
        hostnameThread->Start();

        Thread httpClientThread = new Thread() {
            //@Override
            CARAPI Run() {
                HttpClientTest();
                handler->Post(updatePingResults);
            }
        };
        httpClientThread->Start();
    }

    /**
     * The ping functions have been borrowed from Radio diagnostic app to
     * enable quick access on the wifi status screen
     */
    private final void PingIpAddr() {
        try {
            // TODO: Hardcoded for now, make it UI configurable
            String ipAddress = "74.125.47.104";
            Process p = Runtime->GetRuntime()->Exec("ping -c 1 -w 100 " + ipAddress);
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

    private final void PingHostname() {
        try {
            // TODO: Hardcoded for now, make it UI configurable
            Process p = Runtime->GetRuntime()->Exec("ping -c 1 -w 100 www.google.com");
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

    private void HttpClientTest() {
        HttpClient client = new DefaultHttpClient();
        try {
            // TODO: Hardcoded for now, make it UI configurable
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

}
