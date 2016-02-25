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

using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Net::IIpConfiguration;
using Elastos::Droid::Net::IpConfiguration::IIpAssignment;
using Elastos::Droid::Net::IpConfiguration::IProxySettings;
using Elastos::Droid::Net::ILinkAddress;
using Elastos::Droid::Net::NetworkInfo::IDetailedState;
using Elastos::Droid::Net::INetworkUtils;
using Elastos::Droid::Net::IProxyInfo;
using Elastos::Droid::Net::IRouteInfo;
using Elastos::Droid::Net::IStaticIpConfiguration;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Wifi::IWifiConfiguration;
using Elastos::Droid::Net::Wifi::WifiConfiguration::IAuthAlgorithm;
using Elastos::Droid::Net::Wifi::WifiConfiguration::IKeyMgmt;
using Elastos::Droid::Net::Wifi::IWifiEnterpriseConfig;
using Elastos::Droid::Net::Wifi::WifiEnterpriseConfig::IEap;
using Elastos::Droid::Net::Wifi::WifiEnterpriseConfig::IPhase2;
using Elastos::Droid::Net::Wifi::IWifiInfo;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Security::ICredentials;
using Elastos::Droid::Security::IKeyStore;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IButton;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::CompoundButton::IOnCheckedChangeListener;
using Elastos::Droid::Widget::IEditText;
using Elastos::Droid::Widget::ISpinner;
using Elastos::Droid::Widget::ITextView;

using Elastos::Droid::Settings::IProxySelector;
using Elastos::Droid::Settings::IR;

using Elastos::Net::IInetAddress;
using Elastos::Net::IInet4Address;
using Elastos::Utility::IIterator;

/**
 * The class for allowing UIs like {@link WifiDialog} and {@link WifiConfigUiBase} to
 * share the logic for controlling buttons, text fields, etc.
 */
public class WifiConfigController implements TextWatcher,
       AdapterView.OnItemSelectedListener, OnCheckedChangeListener {
    private static const String TAG = "WifiConfigController";

    private final WifiConfigUiBase mConfigUi;
    private final View mView;
    private final AccessPoint mAccessPoint;

    /* This value comes from "wifi_ip_settings" resource array */
    private static const Int32 DHCP = 0;
    private static const Int32 STATIC_IP = 1;

    /* These values come from "wifi_proxy_settings" resource array */
    public static const Int32 PROXY_NONE = 0;
    public static const Int32 PROXY_STATIC = 1;
    public static const Int32 PROXY_PAC = 2;

    /* These values come from "wifi_eap_method" resource array */
    public static const Int32 WIFI_EAP_METHOD_PEAP = 0;
    public static const Int32 WIFI_EAP_METHOD_TLS  = 1;
    public static const Int32 WIFI_EAP_METHOD_TTLS = 2;
    public static const Int32 WIFI_EAP_METHOD_PWD  = 3;

    /* These values come from "wifi_peap_phase2_entries" resource array */
    public static const Int32 WIFI_PEAP_PHASE2_NONE 	    = 0;
    public static const Int32 WIFI_PEAP_PHASE2_MSCHAPV2 	= 1;
    public static const Int32 WIFI_PEAP_PHASE2_GTC        = 2;

    /* Phase2 methods supported by PEAP are limited */
    private final ArrayAdapter<String> PHASE2_PEAP_ADAPTER;
    /* Full list of phase2 methods */
    private final ArrayAdapter<String> PHASE2_FULL_ADAPTER;

    // True when this instance is used in SetupWizard XL context.
    private final Boolean mInXlSetupWizard;

    private final Handler mTextViewChangedHandler;

    // e.g. AccessPoint.SECURITY_NONE
    private Int32 mAccessPointSecurity;
    private TextView mPasswordView;

    private String unspecifiedCert = "unspecified";
    private static const Int32 unspecifiedCertIndex = 0;

    private Spinner mSecuritySpinner;
    private Spinner mEapMethodSpinner;
    private Spinner mEapCaCertSpinner;
    private Spinner mPhase2Spinner;
    // Associated with mPhase2Spinner, one of PHASE2_FULL_ADAPTER or PHASE2_PEAP_ADAPTER
    private ArrayAdapter<String> mPhase2Adapter;
    private Spinner mEapUserCertSpinner;
    private TextView mEapIdentityView;
    private TextView mEapAnonymousView;

    private Spinner mIpSettingsSpinner;
    private TextView mIpAddressView;
    private TextView mGatewayView;
    private TextView mNetworkPrefixLengthView;
    private TextView mDns1View;
    private TextView mDns2View;

    private Spinner mProxySettingsSpinner;
    private TextView mProxyHostView;
    private TextView mProxyPortView;
    private TextView mProxyExclusionListView;
    private TextView mProxyPacView;

    private IpAssignment mIpAssignment = IpAssignment.UNASSIGNED;
    private ProxySettings mProxySettings = ProxySettings.UNASSIGNED;
    private ProxyInfo mHttpProxy = NULL;
    private StaticIpConfiguration mStaticIpConfiguration = NULL;

    private String[] mLevels;
    private Boolean mEdit;
    private TextView mSsidView;

    private Context mContext;

    public WifiConfigController(
            WifiConfigUiBase parent, View view, AccessPoint accessPoint, Boolean edit) {
        mConfigUi = parent;
        mInXlSetupWizard = (parent instanceof WifiConfigUiForSetupWizardXL);

        mView = view;
        mAccessPoint = accessPoint;
        mAccessPointSecurity = (accessPoint == NULL) ? AccessPoint.SECURITY_NONE :
                accessPoint.security;
        mEdit = edit;

        mTextViewChangedHandler = new Handler();
        mContext = mConfigUi->GetContext();
        final Resources res = mContext->GetResources();

        mLevels = res->GetStringArray(R.array.wifi_signal);
        PHASE2_PEAP_ADAPTER = new ArrayAdapter<String>(
            mContext, android.R.layout.simple_spinner_item,
            res->GetStringArray(R.array.wifi_peap_phase2_entries));
        PHASE2_PEAP_ADAPTER->SetDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        PHASE2_FULL_ADAPTER = new ArrayAdapter<String>(
                mContext, android.R.layout.simple_spinner_item,
                res->GetStringArray(R.array.wifi_phase2_entries));
        PHASE2_FULL_ADAPTER->SetDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);

        unspecifiedCert = mContext->GetString(R::string::wifi_unspecified);
        mIpSettingsSpinner = (Spinner) mView->FindViewById(R.id.ip_settings);
        mIpSettingsSpinner->SetOnItemSelectedListener(this);
        mProxySettingsSpinner = (Spinner) mView->FindViewById(R.id.proxy_settings);
        mProxySettingsSpinner->SetOnItemSelectedListener(this);

        if (mAccessPoint == NULL) { // new network
            mConfigUi->SetTitle(R::string::wifi_add_network);

            mSsidView = (TextView) mView->FindViewById(R.id.ssid);
            mSsidView->AddTextChangedListener(this);
            mSecuritySpinner = ((Spinner) mView->FindViewById(R.id.security));
            mSecuritySpinner->SetOnItemSelectedListener(this);
            if (mInXlSetupWizard) {
                mView->FindViewById(R.id.type_ssid).SetVisibility(View.VISIBLE);
                mView->FindViewById(R.id.type_security).SetVisibility(View.VISIBLE);
                // We want custom layout. The content must be same as the other cases.

                ArrayAdapter<String> adapter = new ArrayAdapter<String>(mContext,
                        R.layout.wifi_setup_custom_list_item_1, android.R.id.text1,
                        res->GetStringArray(R.array.wifi_security_no_eap));
                mSecuritySpinner->SetAdapter(adapter);
            } else {
                mView->FindViewById(R.id.type).SetVisibility(View.VISIBLE);
            }

            ShowIpConfigFields();
            ShowProxyFields();
            mView->FindViewById(R.id.wifi_advanced_toggle).SetVisibility(View.VISIBLE);
            ((CheckBox)mView->FindViewById(R.id.wifi_advanced_togglebox))
                    .SetOnCheckedChangeListener(this);


            mConfigUi->SetSubmitButton(res->GetString(R::string::wifi_save));
        } else {
            mConfigUi->SetTitle(mAccessPoint.ssid);

            ViewGroup group = (ViewGroup) mView->FindViewById(R.id.info);

            Boolean showAdvancedFields = FALSE;
            if (mAccessPoint.networkId != INVALID_NETWORK_ID) {
                WifiConfiguration config = mAccessPoint->GetConfig();
                if (config->GetIpAssignment() == IpAssignment.STATIC) {
                    mIpSettingsSpinner->SetSelection(STATIC_IP);
                    showAdvancedFields = TRUE;
                    // Display IP address.
                    StaticIpConfiguration staticConfig = config->GetStaticIpConfiguration();
                    if (staticConfig != NULL && staticConfig.ipAddress != NULL) {
                        AddRow(group, R::string::wifi_ip_address,
                           staticConfig.ipAddress->GetAddress()->GetHostAddress());
                    }
                } else {
                    mIpSettingsSpinner->SetSelection(DHCP);
                }


                if (config->GetProxySettings() == ProxySettings.STATIC) {
                    mProxySettingsSpinner->SetSelection(PROXY_STATIC);
                    showAdvancedFields = TRUE;
                } else if (config->GetProxySettings() == ProxySettings.PAC) {
                    mProxySettingsSpinner->SetSelection(PROXY_PAC);
                    showAdvancedFields = TRUE;
                } else {
                    mProxySettingsSpinner->SetSelection(PROXY_NONE);
                }
            }

            if (mAccessPoint.networkId == INVALID_NETWORK_ID || mEdit) {
                ShowSecurityFields();
                ShowIpConfigFields();
                ShowProxyFields();
                mView->FindViewById(R.id.wifi_advanced_toggle).SetVisibility(View.VISIBLE);
                ((CheckBox)mView->FindViewById(R.id.wifi_advanced_togglebox))
                    .SetOnCheckedChangeListener(this);
                if (showAdvancedFields) {
                    ((CheckBox)mView->FindViewById(R.id.wifi_advanced_togglebox)).SetChecked(TRUE);
                    mView->FindViewById(R.id.wifi_advanced_fields).SetVisibility(View.VISIBLE);
                }
            }

            if (mEdit) {
                mConfigUi->SetSubmitButton(res->GetString(R::string::wifi_save));
            } else {
                final DetailedState state = mAccessPoint->GetState();
                final String signalLevel = GetSignalString();

                if (state == NULL && signalLevel != NULL) {
                    mConfigUi->SetSubmitButton(res->GetString(R::string::wifi_connect));
                } else {
                    if (state != NULL) {
                        AddRow(group, R::string::wifi_status, Summary->Get(mConfigUi->GetContext(),
                                state));
                    }

                    if (signalLevel != NULL) {
                        AddRow(group, R::string::wifi_signal, signalLevel);
                    }

                    WifiInfo info = mAccessPoint->GetInfo();
                    if (info != NULL && info->GetLinkSpeed() != -1) {
                        AddRow(group, R::string::wifi_speed, info->GetLinkSpeed()
                                + WifiInfo.LINK_SPEED_UNITS);
                    }

                    if (info != NULL && info->GetFrequency() != -1) {
                        final Int32 frequency = info->GetFrequency();
                        String band = NULL;

                        if (frequency >= AccessPoint.LOWER_FREQ_24GHZ
                                && frequency < AccessPoint.HIGHER_FREQ_24GHZ) {
                            band = res->GetString(R::string::wifi_band_24ghz);
                        } else if (frequency >= AccessPoint.LOWER_FREQ_5GHZ
                                && frequency < AccessPoint.HIGHER_FREQ_5GHZ) {
                            band = res->GetString(R::string::wifi_band_5ghz);
                        } else {
                            Logger::E(TAG, "Unexpected frequency " + frequency);
                        }
                        if (band != NULL) {
                            AddRow(group, R::string::wifi_frequency, band);
                        }
                    }

                    AddRow(group, R::string::wifi_security, mAccessPoint->GetSecurityString(FALSE));
                    mView->FindViewById(R.id.ip_fields).SetVisibility(View.GONE);
                }
                if (mAccessPoint.networkId != INVALID_NETWORK_ID
                        && ActivityManager->GetCurrentUser() == UserHandle.USER_OWNER) {
                    mConfigUi->SetForgetButton(res->GetString(R::string::wifi_forget));
                }
            }
        }

        if (mEdit || (mAccessPoint->GetState() == NULL && mAccessPoint->GetLevel() != -1)){
            mConfigUi->SetCancelButton(res->GetString(R::string::wifi_cancel));
        }else{
            mConfigUi->SetCancelButton(res->GetString(R::string::wifi_display_options_done));
        }
        if (mConfigUi->GetSubmitButton() != NULL) {
            EnableSubmitIfAppropriate();
        }
    }

    private void AddRow(ViewGroup group, Int32 name, String value) {
        View row = mConfigUi->GetLayoutInflater()->Inflate(R.layout.wifi_dialog_row, group, FALSE);
        ((TextView) row->FindViewById(R.id.name)).SetText(name);
        ((TextView) row->FindViewById(R.id.value)).SetText(value);
        group->AddView(row);
    }

    private String GetSignalString(){
        final Int32 level = mAccessPoint->GetLevel();

        return (level > -1 && level < mLevels.length) ? mLevels[level] : NULL;
    }

    void HideSubmitButton() {
        Button submit = mConfigUi->GetSubmitButton();
        if (submit == NULL) return;

        submit->SetVisibility(View.GONE);
    }

    /* show submit button if password, ip and proxy settings are valid */
    void EnableSubmitIfAppropriate() {
        Button submit = mConfigUi->GetSubmitButton();
        if (submit == NULL) return;

        Boolean enabled = FALSE;
        Boolean passwordInvalid = FALSE;

        if (mPasswordView != NULL &&
            ((mAccessPointSecurity == AccessPoint.SECURITY_WEP && mPasswordView->Length() == 0) ||
            (mAccessPointSecurity == AccessPoint.SECURITY_PSK && mPasswordView->Length() < 8))) {
            passwordInvalid = TRUE;
        }

        if ((mSsidView != NULL && mSsidView->Length() == 0) ||
            ((mAccessPoint == NULL || mAccessPoint.networkId == INVALID_NETWORK_ID) &&
            passwordInvalid)) {
            enabled = FALSE;
        } else {
            if (IpAndProxyFieldsAreValid()) {
                enabled = TRUE;
            } else {
                enabled = FALSE;
            }
        }
        submit->SetEnabled(enabled);
    }

    /* package */ WifiConfiguration GetConfig() {
        if (mAccessPoint != NULL && mAccessPoint.networkId != INVALID_NETWORK_ID && !mEdit) {
            return NULL;
        }

        WifiConfiguration config = new WifiConfiguration();

        if (mAccessPoint == NULL) {
            config.SSID = AccessPoint->ConvertToQuotedString(
                    mSsidView->GetText()->ToString());
            // If the user adds a network manually, assume that it is hidden.
            config.hiddenSSID = TRUE;
        } else if (mAccessPoint.networkId == INVALID_NETWORK_ID) {
            config.SSID = AccessPoint->ConvertToQuotedString(
                    mAccessPoint.ssid);
        } else {
            config.networkId = mAccessPoint.networkId;
        }

        switch (mAccessPointSecurity) {
            case AccessPoint.SECURITY_NONE:
                config.allowedKeyManagement->Set(KeyMgmt.NONE);
                break;

            case AccessPoint.SECURITY_WEP:
                config.allowedKeyManagement->Set(KeyMgmt.NONE);
                config.allowedAuthAlgorithms->Set(AuthAlgorithm.OPEN);
                config.allowedAuthAlgorithms->Set(AuthAlgorithm.SHARED);
                if (mPasswordView->Length() != 0) {
                    Int32 length = mPasswordView->Length();
                    String password = mPasswordView->GetText()->ToString();
                    // WEP-40, WEP-104, and 256-bit WEP (WEP-232?)
                    if ((length == 10 || length == 26 || length == 58) &&
                            password->Matches("[0-9A-Fa-f]*")) {
                        config.wepKeys[0] = password;
                    } else {
                        config.wepKeys[0] = '"' + password + '"';
                    }
                }
                break;

            case AccessPoint.SECURITY_PSK:
                config.allowedKeyManagement->Set(KeyMgmt.WPA_PSK);
                if (mPasswordView->Length() != 0) {
                    String password = mPasswordView->GetText()->ToString();
                    if (password->Matches("[0-9A-Fa-f]{64}")) {
                        config.preSharedKey = password;
                    } else {
                        config.preSharedKey = '"' + password + '"';
                    }
                }
                break;

            case AccessPoint.SECURITY_EAP:
                config.allowedKeyManagement->Set(KeyMgmt.WPA_EAP);
                config.allowedKeyManagement->Set(KeyMgmt.IEEE8021X);
                config.enterpriseConfig = new WifiEnterpriseConfig();
                Int32 eapMethod = mEapMethodSpinner->GetSelectedItemPosition();
                Int32 phase2Method = mPhase2Spinner->GetSelectedItemPosition();
                config.enterpriseConfig->SetEapMethod(eapMethod);
                switch (eapMethod) {
                    case Eap.PEAP:
                        // PEAP supports limited phase2 values
                        // Map the index from the PHASE2_PEAP_ADAPTER to the one used
                        // by the API which has the full list of PEAP methods.
                        Switch(phase2Method) {
                            case WIFI_PEAP_PHASE2_NONE:
                                config.enterpriseConfig->SetPhase2Method(Phase2.NONE);
                                break;
                            case WIFI_PEAP_PHASE2_MSCHAPV2:
                                config.enterpriseConfig->SetPhase2Method(Phase2.MSCHAPV2);
                                break;
                            case WIFI_PEAP_PHASE2_GTC:
                                config.enterpriseConfig->SetPhase2Method(Phase2.GTC);
                                break;
                            default:
                                Logger::E(TAG, "Unknown phase2 method" + phase2Method);
                                break;
                        }
                        break;
                    default:
                        // The default index from PHASE2_FULL_ADAPTER maps to the API
                        config.enterpriseConfig->SetPhase2Method(phase2Method);
                        break;
                }
                String caCert = (String) mEapCaCertSpinner->GetSelectedItem();
                if (caCert->Equals(unspecifiedCert)) caCert = "";
                config.enterpriseConfig->SetCaCertificateAlias(caCert);
                String clientCert = (String) mEapUserCertSpinner->GetSelectedItem();
                if (clientCert->Equals(unspecifiedCert)) clientCert = "";
                config.enterpriseConfig->SetClientCertificateAlias(clientCert);
                config.enterpriseConfig->SetIdentity(mEapIdentityView->GetText()->ToString());
                config.enterpriseConfig->SetAnonymousIdentity(
                        mEapAnonymousView->GetText()->ToString());

                if (mPasswordView->IsShown()) {
                    // For security reasons, a previous password is not displayed to user.
                    // Update only if it has been changed.
                    if (mPasswordView->Length() > 0) {
                        config.enterpriseConfig->SetPassword(mPasswordView->GetText()->ToString());
                    }
                } else {
                    // clear password
                    config.enterpriseConfig->SetPassword(mPasswordView->GetText()->ToString());
                }
                break;
            default:
                return NULL;
        }

        config->SetIpConfiguration(
                new IpConfiguration(mIpAssignment, mProxySettings,
                                    mStaticIpConfiguration, mHttpProxy));

        return config;
    }

    private Boolean IpAndProxyFieldsAreValid() {
        mIpAssignment = (mIpSettingsSpinner != NULL &&
                mIpSettingsSpinner->GetSelectedItemPosition() == STATIC_IP) ?
                IpAssignment.STATIC : IpAssignment.DHCP;

        if (mIpAssignment == IpAssignment.STATIC) {
            mStaticIpConfiguration = new StaticIpConfiguration();
            Int32 result = ValidateIpConfigFields(mStaticIpConfiguration);
            if (result != 0) {
                return FALSE;
            }
        }

        final Int32 selectedPosition = mProxySettingsSpinner->GetSelectedItemPosition();
        mProxySettings = ProxySettings.NONE;
        mHttpProxy = NULL;
        if (selectedPosition == PROXY_STATIC && mProxyHostView != NULL) {
            mProxySettings = ProxySettings.STATIC;
            String host = mProxyHostView->GetText()->ToString();
            String portStr = mProxyPortView->GetText()->ToString();
            String exclusionList = mProxyExclusionListView->GetText()->ToString();
            Int32 port = 0;
            Int32 result = 0;
            try {
                port = Integer->ParseInt(portStr);
                result = ProxySelector->Validate(host, portStr, exclusionList);
            } catch (NumberFormatException e) {
                result = R::string::proxy_error_invalid_port;
            }
            if (result == 0) {
                mHttpProxy = new ProxyInfo(host, port, exclusionList);
            } else {
                return FALSE;
            }
        } else if (selectedPosition == PROXY_PAC && mProxyPacView != NULL) {
            mProxySettings = ProxySettings.PAC;
            CharSequence uriSequence = mProxyPacView->GetText();
            if (TextUtils->IsEmpty(uriSequence)) {
                return FALSE;
            }
            Uri uri = Uri->Parse(uriSequence->ToString());
            if (uri == NULL) {
                return FALSE;
            }
            mHttpProxy = new ProxyInfo(uri);
        }
        return TRUE;
    }

    private Inet4Address GetIPv4Address(String text) {
        try {
            return (Inet4Address) NetworkUtils->NumericToInetAddress(text);
        } catch (IllegalArgumentException|ClassCastException e) {
            return NULL;
        }
    }

    private Int32 ValidateIpConfigFields(StaticIpConfiguration staticIpConfiguration) {
        if (mIpAddressView == NULL) return 0;

        String ipAddr = mIpAddressView->GetText()->ToString();
        if (TextUtils->IsEmpty(ipAddr)) return R::string::wifi_ip_settings_invalid_ip_address;

        Inet4Address inetAddr = GetIPv4Address(ipAddr);
        if (inetAddr == NULL) {
            return R::string::wifi_ip_settings_invalid_ip_address;
        }

        Int32 networkPrefixLength = -1;
        try {
            networkPrefixLength = Integer->ParseInt(mNetworkPrefixLengthView->GetText()->ToString());
            if (networkPrefixLength < 0 || networkPrefixLength > 32) {
                return R::string::wifi_ip_settings_invalid_network_prefix_length;
            }
            staticIpConfiguration.ipAddress = new LinkAddress(inetAddr, networkPrefixLength);
        } catch (NumberFormatException e) {
            // Set the hint as default after user types in ip address
            mNetworkPrefixLengthView->SetText(mConfigUi->GetContext()->GetString(
                    R::string::wifi_network_prefix_length_hint));
        }

        String gateway = mGatewayView->GetText()->ToString();
        if (TextUtils->IsEmpty(gateway)) {
            try {
                //Extract a default gateway from IP address
                InetAddress netPart = NetworkUtils->GetNetworkPart(inetAddr, networkPrefixLength);
                Byte[] addr = netPart->GetAddress();
                addr[addr.length-1] = 1;
                mGatewayView->SetText(InetAddress->GetByAddress(addr).GetHostAddress());
            } catch (RuntimeException ee) {
            } catch (java.net.UnknownHostException u) {
            }
        } else {
            InetAddress gatewayAddr = GetIPv4Address(gateway);
            if (gatewayAddr == NULL) {
                return R::string::wifi_ip_settings_invalid_gateway;
            }
            staticIpConfiguration.gateway = gatewayAddr;
        }

        String dns = mDns1View->GetText()->ToString();
        InetAddress dnsAddr = NULL;

        if (TextUtils->IsEmpty(dns)) {
            //If everything else is valid, provide hint as a default option
            mDns1View->SetText(mConfigUi->GetContext()->GetString(R::string::wifi_dns1_hint));
        } else {
            dnsAddr = GetIPv4Address(dns);
            if (dnsAddr == NULL) {
                return R::string::wifi_ip_settings_invalid_dns;
            }
            staticIpConfiguration.dnsServers->Add(dnsAddr);
        }

        if (mDns2View->Length() > 0) {
            dns = mDns2View->GetText()->ToString();
            dnsAddr = GetIPv4Address(dns);
            if (dnsAddr == NULL) {
                return R::string::wifi_ip_settings_invalid_dns;
            }
            staticIpConfiguration.dnsServers->Add(dnsAddr);
        }
        return 0;
    }

    private void ShowSecurityFields() {
        if (mInXlSetupWizard) {
            // Note: XL SetupWizard won't hide "EAP" settings here.
            if (!((WifiSettingsForSetupWizardXL)mConfigUi->GetContext()).InitSecurityFields(mView,
                        mAccessPointSecurity)) {
                return;
            }
        }
        if (mAccessPointSecurity == AccessPoint.SECURITY_NONE) {
            mView->FindViewById(R.id.security_fields).SetVisibility(View.GONE);
            return;
        }
        mView->FindViewById(R.id.security_fields).SetVisibility(View.VISIBLE);

        if (mPasswordView == NULL) {
            mPasswordView = (TextView) mView->FindViewById(R.id.password);
            mPasswordView->AddTextChangedListener(this);
            ((CheckBox) mView->FindViewById(R.id.show_password))
                .SetOnCheckedChangeListener(this);

            if (mAccessPoint != NULL && mAccessPoint.networkId != INVALID_NETWORK_ID) {
                mPasswordView->SetHint(R::string::wifi_unchanged);
            }
        }

        if (mAccessPointSecurity != AccessPoint.SECURITY_EAP) {
            mView->FindViewById(R.id.eap).SetVisibility(View.GONE);
            return;
        }
        mView->FindViewById(R.id.eap).SetVisibility(View.VISIBLE);

        if (mEapMethodSpinner == NULL) {
            mEapMethodSpinner = (Spinner) mView->FindViewById(R.id.method);
            mEapMethodSpinner->SetOnItemSelectedListener(this);
            mPhase2Spinner = (Spinner) mView->FindViewById(R.id.phase2);
            mEapCaCertSpinner = (Spinner) mView->FindViewById(R.id.ca_cert);
            mEapUserCertSpinner = (Spinner) mView->FindViewById(R.id.user_cert);
            mEapIdentityView = (TextView) mView->FindViewById(R.id.identity);
            mEapAnonymousView = (TextView) mView->FindViewById(R.id.anonymous);

            LoadCertificates(mEapCaCertSpinner, Credentials.CA_CERTIFICATE);
            LoadCertificates(mEapUserCertSpinner, Credentials.USER_PRIVATE_KEY);

            // Modifying an existing network
            if (mAccessPoint != NULL && mAccessPoint.networkId != INVALID_NETWORK_ID) {
                WifiEnterpriseConfig enterpriseConfig = mAccessPoint->GetConfig().enterpriseConfig;
                Int32 eapMethod = enterpriseConfig->GetEapMethod();
                Int32 phase2Method = enterpriseConfig->GetPhase2Method();
                mEapMethodSpinner->SetSelection(eapMethod);
                ShowEapFieldsByMethod(eapMethod);
                switch (eapMethod) {
                    case Eap.PEAP:
                        switch (phase2Method) {
                            case Phase2.NONE:
                                mPhase2Spinner->SetSelection(WIFI_PEAP_PHASE2_NONE);
                                break;
                            case Phase2.MSCHAPV2:
                                mPhase2Spinner->SetSelection(WIFI_PEAP_PHASE2_MSCHAPV2);
                                break;
                            case Phase2.GTC:
                                mPhase2Spinner->SetSelection(WIFI_PEAP_PHASE2_GTC);
                                break;
                            default:
                                Logger::E(TAG, "Invalid phase 2 method " + phase2Method);
                                break;
                        }
                        break;
                    default:
                        mPhase2Spinner->SetSelection(phase2Method);
                        break;
                }
                SetSelection(mEapCaCertSpinner, enterpriseConfig->GetCaCertificateAlias());
                SetSelection(mEapUserCertSpinner, enterpriseConfig->GetClientCertificateAlias());
                mEapIdentityView->SetText(enterpriseConfig->GetIdentity());
                mEapAnonymousView->SetText(enterpriseConfig->GetAnonymousIdentity());
            } else {
                // Choose a default for a new network and show only appropriate
                // fields
                mEapMethodSpinner->SetSelection(Eap.PEAP);
                ShowEapFieldsByMethod(Eap.PEAP);
            }
        } else {
            ShowEapFieldsByMethod(mEapMethodSpinner->GetSelectedItemPosition());
        }
    }

    /**
     * EAP-PWD valid fields include
     *   identity
     *   password
     * EAP-PEAP valid fields include
     *   phase2: MSCHAPV2, GTC
     *   ca_cert
     *   identity
     *   anonymous_identity
     *   password
     * EAP-TLS valid fields include
     *   user_cert
     *   ca_cert
     *   identity
     * EAP-TTLS valid fields include
     *   phase2: PAP, MSCHAP, MSCHAPV2, GTC
     *   ca_cert
     *   identity
     *   anonymous_identity
     *   password
     */
    private void ShowEapFieldsByMethod(Int32 eapMethod) {
        // Common defaults
        mView->FindViewById(R.id.l_method).SetVisibility(View.VISIBLE);
        mView->FindViewById(R.id.l_identity).SetVisibility(View.VISIBLE);

        // Defaults for most of the EAP methods and over-riden by
        // by certain EAP methods
        mView->FindViewById(R.id.l_ca_cert).SetVisibility(View.VISIBLE);
        mView->FindViewById(R.id.password_layout).SetVisibility(View.VISIBLE);
        mView->FindViewById(R.id.show_password_layout).SetVisibility(View.VISIBLE);

        Context context = mConfigUi->GetContext();
        switch (eapMethod) {
            case WIFI_EAP_METHOD_PWD:
                SetPhase2Invisible();
                SetCaCertInvisible();
                SetAnonymousIdentInvisible();
                SetUserCertInvisible();
                break;
            case WIFI_EAP_METHOD_TLS:
                mView->FindViewById(R.id.l_user_cert).SetVisibility(View.VISIBLE);
                SetPhase2Invisible();
                SetAnonymousIdentInvisible();
                SetPasswordInvisible();
                break;
            case WIFI_EAP_METHOD_PEAP:
                // Reset adapter if needed
                if (mPhase2Adapter != PHASE2_PEAP_ADAPTER) {
                    mPhase2Adapter = PHASE2_PEAP_ADAPTER;
                    mPhase2Spinner->SetAdapter(mPhase2Adapter);
                }
                mView->FindViewById(R.id.l_phase2).SetVisibility(View.VISIBLE);
                mView->FindViewById(R.id.l_anonymous).SetVisibility(View.VISIBLE);
                SetUserCertInvisible();
                break;
            case WIFI_EAP_METHOD_TTLS:
                // Reset adapter if needed
                if (mPhase2Adapter != PHASE2_FULL_ADAPTER) {
                    mPhase2Adapter = PHASE2_FULL_ADAPTER;
                    mPhase2Spinner->SetAdapter(mPhase2Adapter);
                }
                mView->FindViewById(R.id.l_phase2).SetVisibility(View.VISIBLE);
                mView->FindViewById(R.id.l_anonymous).SetVisibility(View.VISIBLE);
                SetUserCertInvisible();
                break;
        }
    }

    private void SetPhase2Invisible() {
        mView->FindViewById(R.id.l_phase2).SetVisibility(View.GONE);
        mPhase2Spinner->SetSelection(Phase2.NONE);
    }

    private void SetCaCertInvisible() {
        mView->FindViewById(R.id.l_ca_cert).SetVisibility(View.GONE);
        mEapCaCertSpinner->SetSelection(unspecifiedCertIndex);
    }

    private void SetUserCertInvisible() {
        mView->FindViewById(R.id.l_user_cert).SetVisibility(View.GONE);
        mEapUserCertSpinner->SetSelection(unspecifiedCertIndex);
    }

    private void SetAnonymousIdentInvisible() {
        mView->FindViewById(R.id.l_anonymous).SetVisibility(View.GONE);
        mEapAnonymousView->SetText("");
    }

    private void SetPasswordInvisible() {
        mPasswordView->SetText("");
        mView->FindViewById(R.id.password_layout).SetVisibility(View.GONE);
        mView->FindViewById(R.id.show_password_layout).SetVisibility(View.GONE);
    }

    private void ShowIpConfigFields() {
        WifiConfiguration config = NULL;

        mView->FindViewById(R.id.ip_fields).SetVisibility(View.VISIBLE);

        if (mAccessPoint != NULL && mAccessPoint.networkId != INVALID_NETWORK_ID) {
            config = mAccessPoint->GetConfig();
        }

        if (mIpSettingsSpinner->GetSelectedItemPosition() == STATIC_IP) {
            mView->FindViewById(R.id.staticip).SetVisibility(View.VISIBLE);
            if (mIpAddressView == NULL) {
                mIpAddressView = (TextView) mView->FindViewById(R.id.ipaddress);
                mIpAddressView->AddTextChangedListener(this);
                mGatewayView = (TextView) mView->FindViewById(R.id.gateway);
                mGatewayView->AddTextChangedListener(this);
                mNetworkPrefixLengthView = (TextView) mView->FindViewById(
                        R.id.network_prefix_length);
                mNetworkPrefixLengthView->AddTextChangedListener(this);
                mDns1View = (TextView) mView->FindViewById(R.id.dns1);
                mDns1View->AddTextChangedListener(this);
                mDns2View = (TextView) mView->FindViewById(R.id.dns2);
                mDns2View->AddTextChangedListener(this);
            }
            if (config != NULL) {
                StaticIpConfiguration staticConfig = config->GetStaticIpConfiguration();
                if (staticConfig != NULL) {
                    if (staticConfig.ipAddress != NULL) {
                        mIpAddressView->SetText(
                                staticConfig.ipAddress->GetAddress()->GetHostAddress());
                        mNetworkPrefixLengthView->SetText(Integer->ToString(staticConfig.ipAddress
                                .GetNetworkPrefixLength()));
                    }

                    if (staticConfig.gateway != NULL) {
                        mGatewayView->SetText(staticConfig.gateway->GetHostAddress());
                    }

                    Iterator<InetAddress> dnsIterator = staticConfig.dnsServers->Iterator();
                    if (dnsIterator->HasNext()) {
                        mDns1View->SetText(dnsIterator->Next()->GetHostAddress());
                    }
                    if (dnsIterator->HasNext()) {
                        mDns2View->SetText(dnsIterator->Next()->GetHostAddress());
                    }
                }
            }
        } else {
            mView->FindViewById(R.id.staticip).SetVisibility(View.GONE);
        }
    }

    private void ShowProxyFields() {
        WifiConfiguration config = NULL;

        mView->FindViewById(R.id.proxy_settings_fields).SetVisibility(View.VISIBLE);

        if (mAccessPoint != NULL && mAccessPoint.networkId != INVALID_NETWORK_ID) {
            config = mAccessPoint->GetConfig();
        }

        if (mProxySettingsSpinner->GetSelectedItemPosition() == PROXY_STATIC) {
            SetVisibility(R.id.proxy_warning_limited_support, View.VISIBLE);
            SetVisibility(R.id.proxy_fields, View.VISIBLE);
            SetVisibility(R.id.proxy_pac_field, View.GONE);
            if (mProxyHostView == NULL) {
                mProxyHostView = (TextView) mView->FindViewById(R.id.proxy_hostname);
                mProxyHostView->AddTextChangedListener(this);
                mProxyPortView = (TextView) mView->FindViewById(R.id.proxy_port);
                mProxyPortView->AddTextChangedListener(this);
                mProxyExclusionListView = (TextView) mView->FindViewById(R.id.proxy_exclusionlist);
                mProxyExclusionListView->AddTextChangedListener(this);
            }
            if (config != NULL) {
                ProxyInfo proxyProperties = config->GetHttpProxy();
                if (proxyProperties != NULL) {
                    mProxyHostView->SetText(proxyProperties->GetHost());
                    mProxyPortView->SetText(Integer->ToString(proxyProperties->GetPort()));
                    mProxyExclusionListView->SetText(proxyProperties->GetExclusionListAsString());
                }
            }
        } else if (mProxySettingsSpinner->GetSelectedItemPosition() == PROXY_PAC) {
            SetVisibility(R.id.proxy_warning_limited_support, View.GONE);
            SetVisibility(R.id.proxy_fields, View.GONE);
            SetVisibility(R.id.proxy_pac_field, View.VISIBLE);

            if (mProxyPacView == NULL) {
                mProxyPacView = (TextView) mView->FindViewById(R.id.proxy_pac);
                mProxyPacView->AddTextChangedListener(this);
            }
            if (config != NULL) {
                ProxyInfo proxyInfo = config->GetHttpProxy();
                if (proxyInfo != NULL) {
                    mProxyPacView->SetText(proxyInfo->GetPacFileUrl()->ToString());
                }
            }
        } else {
            SetVisibility(R.id.proxy_warning_limited_support, View.GONE);
            SetVisibility(R.id.proxy_fields, View.GONE);
            SetVisibility(R.id.proxy_pac_field, View.GONE);
        }
    }

    private void SetVisibility(Int32 id, Int32 visibility) {
        final View v = mView->FindViewById(id);
        if (v != NULL) {
            v->SetVisibility(visibility);
        }
    }

    private void LoadCertificates(Spinner spinner, String prefix) {
        final Context context = mConfigUi->GetContext();

        String[] certs = KeyStore->GetInstance()->Saw(prefix, android.os.Process.WIFI_UID);
        if (certs == NULL || certs.length == 0) {
            certs = new String[] {unspecifiedCert};
        } else {
            final String[] array = new String[certs.length + 1];
            array[0] = unspecifiedCert;
            System->Arraycopy(certs, 0, array, 1, certs.length);
            certs = array;
        }

        final ArrayAdapter<String> adapter = new ArrayAdapter<String>(
                context, android.R.layout.simple_spinner_item, certs);
        adapter->SetDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spinner->SetAdapter(adapter);
    }

    private void SetSelection(Spinner spinner, String value) {
        if (value != NULL) {
            @SuppressWarnings("unchecked")
            ArrayAdapter<String> adapter = (ArrayAdapter<String>) spinner->GetAdapter();
            for (Int32 i = adapter->GetCount() - 1; i >= 0; --i) {
                if (value->Equals(adapter->GetItem(i))) {
                    spinner->SetSelection(i);
                    break;
                }
            }
        }
    }

    public Boolean IsEdit() {
        return mEdit;
    }

    //@Override
    CARAPI AfterTextChanged(Editable s) {
        mTextViewChangedHandler->Post(new Runnable() {
                CARAPI Run() {
                    EnableSubmitIfAppropriate();
                }
            });
    }

    //@Override
    CARAPI BeforeTextChanged(CharSequence s, Int32 start, Int32 count, Int32 after) {
        // work done in afterTextChanged
    }

    //@Override
    CARAPI OnTextChanged(CharSequence s, Int32 start, Int32 before, Int32 count) {
        // work done in afterTextChanged
    }

    //@Override
    CARAPI OnCheckedChanged(CompoundButton view, Boolean isChecked) {
        if (view->GetId() == R.id.show_password) {
            Int32 pos = mPasswordView->GetSelectionEnd();
            mPasswordView->SetInputType(
                    InputType.TYPE_CLASS_TEXT | (isChecked ?
                            InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD :
                                InputType.TYPE_TEXT_VARIATION_PASSWORD));
            if (pos >= 0) {
                ((EditText)mPasswordView).SetSelection(pos);
            }
        } else if (view->GetId() == R.id.wifi_advanced_togglebox) {
            if (isChecked) {
                mView->FindViewById(R.id.wifi_advanced_fields).SetVisibility(View.VISIBLE);
            } else {
                mView->FindViewById(R.id.wifi_advanced_fields).SetVisibility(View.GONE);
            }
        }
    }

    //@Override
    CARAPI OnItemSelected(AdapterView<?> parent, View view, Int32 position, Int64 id) {
        if (parent == mSecuritySpinner) {
            mAccessPointSecurity = position;
            ShowSecurityFields();
        } else if (parent == mEapMethodSpinner) {
            ShowSecurityFields();
        } else if (parent == mProxySettingsSpinner) {
            ShowProxyFields();
        } else {
            ShowIpConfigFields();
        }
        EnableSubmitIfAppropriate();
    }

    //@Override
    CARAPI OnNothingSelected(AdapterView<?> parent) {
        //
    }

    /**
     * Make the characters of the password visible if show_password is checked.
     */
    private void UpdatePasswordVisibility(Boolean checked) {
        Int32 pos = mPasswordView->GetSelectionEnd();
        mPasswordView->SetInputType(
                InputType.TYPE_CLASS_TEXT | (checked ?
                        InputType.TYPE_TEXT_VARIATION_VISIBLE_PASSWORD :
                            InputType.TYPE_TEXT_VARIATION_PASSWORD));
        if (pos >= 0) {
            ((EditText)mPasswordView).SetSelection(pos);
        }
    }
}
