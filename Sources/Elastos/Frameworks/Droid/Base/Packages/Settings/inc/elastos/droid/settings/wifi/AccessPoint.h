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

using Elastos::Droid::Settings::IR;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IStateListDrawable;
using Elastos::Droid::Net::NetworkInfo::IDetailedState;
using Elastos::Droid::Net::Wifi::IScanResult;
using Elastos::Droid::Net::Wifi::IWifiConfiguration;
using Elastos::Droid::Net::Wifi::WifiConfiguration::IKeyMgmt;
using Elastos::Droid::Net::Wifi::IWifiInfo;
using Elastos::Droid::Net::Wifi::IWifiManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Utility::ILruCache;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

using Elastos::Utility::IMap;


class AccessPoint extends Preference {
    static const String TAG = "Settings.AccessPoint";

    /**
     * Lower bound on the 2.4 GHz (802.11b/g/n) WLAN channels
     */
    public static const Int32 LOWER_FREQ_24GHZ = 2400;

    /**
     * Upper bound on the 2.4 GHz (802.11b/g/n) WLAN channels
     */
    public static const Int32 HIGHER_FREQ_24GHZ = 2500;

    /**
     * Lower bound on the 5.0 GHz (802.11a/h/j/n/ac) WLAN channels
     */
    public static const Int32 LOWER_FREQ_5GHZ = 4900;

    /**
     * Upper bound on the 5.0 GHz (802.11a/h/j/n/ac) WLAN channels
     */
    public static const Int32 HIGHER_FREQ_5GHZ = 5900;

    /**
     * Experimental: we should be able to show the user the list of BSSIDs and bands
     *  for that SSID.
     *  For now this data is used only with Verbose Logging so as to show the band and number
     *  of BSSIDs on which that network is seen.
     */
    public LruCache<String, ScanResult> mScanResultCache;


    private static const String KEY_DETAILEDSTATE = "key_detailedstate";
    private static const String KEY_WIFIINFO = "key_wifiinfo";
    private static const String KEY_SCANRESULT = "key_scanresult";
    private static const String KEY_CONFIG = "key_config";

    private static const Int32[] STATE_SECURED = {
        R.attr.state_encrypted
    };
    private static const Int32[] STATE_NONE = {};

    private static Int32[] wifi_signal_attributes = { R.attr.wifi_signal };

    /**
     * These values are matched in string arrays -- changes must be kept in sync
     */
    static const Int32 SECURITY_NONE = 0;
    static const Int32 SECURITY_WEP = 1;
    static const Int32 SECURITY_PSK = 2;
    static const Int32 SECURITY_EAP = 3;

    enum PskType {
        UNKNOWN,
        WPA,
        WPA2,
        WPA_WPA2
    }

    String ssid;
    String bssid;
    Int32 security;
    Int32 networkId = -1;
    Boolean wpsAvailable = FALSE;
    Boolean showSummary = TRUE;

    PskType pskType = PskType.UNKNOWN;

    private WifiConfiguration mConfig;
    /* package */ScanResult mScanResult;

    private Int32 mRssi = Integer.MAX_VALUE;
    private Int64 mSeen = 0;

    private WifiInfo mInfo;
    private DetailedState mState;

    private static const Int32 VISIBILITY_MAX_AGE_IN_MILLI = 1000000;
    private static const Int32 VISIBILITY_OUTDATED_AGE_IN_MILLI = 20000;
    private static const Int32 SECOND_TO_MILLI = 1000;

    static Int32 GetSecurity(WifiConfiguration config) {
        if (config.allowedKeyManagement->Get(KeyMgmt.WPA_PSK)) {
            return SECURITY_PSK;
        }
        if (config.allowedKeyManagement->Get(KeyMgmt.WPA_EAP) ||
                config.allowedKeyManagement->Get(KeyMgmt.IEEE8021X)) {
            return SECURITY_EAP;
        }
        return (config.wepKeys[0] != NULL) ? SECURITY_WEP : SECURITY_NONE;
    }

    private static Int32 GetSecurity(ScanResult result) {
        if (result.capabilities->Contains("WEP")) {
            return SECURITY_WEP;
        } else if (result.capabilities->Contains("PSK")) {
            return SECURITY_PSK;
        } else if (result.capabilities->Contains("EAP")) {
            return SECURITY_EAP;
        }
        return SECURITY_NONE;
    }

    public String GetSecurityString(Boolean concise) {
        Context context = GetContext();
        Switch(security) {
            case SECURITY_EAP:
                return concise ? context->GetString(R::string::wifi_security_short_eap) :
                    context->GetString(R::string::wifi_security_eap);
            case SECURITY_PSK:
                switch (pskType) {
                    case WPA:
                        return concise ? context->GetString(R::string::wifi_security_short_wpa) :
                            context->GetString(R::string::wifi_security_wpa);
                    case WPA2:
                        return concise ? context->GetString(R::string::wifi_security_short_wpa2) :
                            context->GetString(R::string::wifi_security_wpa2);
                    case WPA_WPA2:
                        return concise ? context->GetString(R::string::wifi_security_short_wpa_wpa2) :
                            context->GetString(R::string::wifi_security_wpa_wpa2);
                    case UNKNOWN:
                    default:
                        return concise ? context->GetString(R::string::wifi_security_short_psk_generic)
                                : context->GetString(R::string::wifi_security_psk_generic);
                }
            case SECURITY_WEP:
                return concise ? context->GetString(R::string::wifi_security_short_wep) :
                    context->GetString(R::string::wifi_security_wep);
            case SECURITY_NONE:
            default:
                return concise ? "" : context->GetString(R::string::wifi_security_none);
        }
    }

    private static PskType GetPskType(ScanResult result) {
        Boolean wpa = result.capabilities->Contains("WPA-PSK");
        Boolean wpa2 = result.capabilities->Contains("WPA2-PSK");
        if (wpa2 && wpa) {
            return PskType.WPA_WPA2;
        } else if (wpa2) {
            return PskType.WPA2;
        } else if (wpa) {
            return PskType.WPA;
        } else {
            Logger::W(TAG, "Received abnormal flag string: " + result.capabilities);
            return PskType.UNKNOWN;
        }
    }

    AccessPoint(Context context, WifiConfiguration config) {
        Super(context);
        LoadConfig(config);
        Refresh();
    }

    AccessPoint(Context context, ScanResult result) {
        Super(context);
        LoadResult(result);
        Refresh();
    }

    AccessPoint(Context context, Bundle savedState) {
        Super(context);

        mConfig = savedState->GetParcelable(KEY_CONFIG);
        if (mConfig != NULL) {
            LoadConfig(mConfig);
        }
        mScanResult = (ScanResult) savedState->GetParcelable(KEY_SCANRESULT);
        if (mScanResult != NULL) {
            LoadResult(mScanResult);
        }
        mInfo = (WifiInfo) savedState->GetParcelable(KEY_WIFIINFO);
        if (savedState->ContainsKey(KEY_DETAILEDSTATE)) {
            mState = DetailedState->ValueOf(savedState->GetString(KEY_DETAILEDSTATE));
        }
        Update(mInfo, mState);
    }

    CARAPI SaveWifiState(Bundle savedState) {
        savedState->PutParcelable(KEY_CONFIG, mConfig);
        savedState->PutParcelable(KEY_SCANRESULT, mScanResult);
        savedState->PutParcelable(KEY_WIFIINFO, mInfo);
        if (mState != NULL) {
            savedState->PutString(KEY_DETAILEDSTATE, mState->ToString());
        }
    }

    private void LoadConfig(WifiConfiguration config) {
        ssid = (config.SSID == NULL ? "" : RemoveDoubleQuotes(config.SSID));
        bssid = config.BSSID;
        security = GetSecurity(config);
        networkId = config.networkId;
        mConfig = config;
    }

    private void LoadResult(ScanResult result) {
        ssid = result.SSID;
        bssid = result.BSSID;
        security = GetSecurity(result);
        wpsAvailable = security != SECURITY_EAP && result.capabilities->Contains("WPS");
        if (security == SECURITY_PSK)
            pskType = GetPskType(result);
        mRssi = result.level;
        mScanResult = result;
        if (result.seen > mSeen) {
            mSeen = result.seen;
        }
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);
        UpdateIcon(GetLevel(), GetContext());

        final TextView summaryView = (TextView) view->FindViewById(
                R.id.summary);
        summaryView->SetVisibility(showSummary ? View.VISIBLE : View.GONE);

        NotifyChanged();
    }

    protected void UpdateIcon(Int32 level, Context context) {
        if (level == -1) {
            SetIcon(NULL);
        } else {
            Drawable drawable = GetIcon();

            if (drawable == NULL) {
                // To avoid a drawing race condition, we first set the state (SECURE/NONE) and then
                // set the icon (drawable) to that state's drawable.
                StateListDrawable sld = (StateListDrawable) context->GetTheme()
                        .ObtainStyledAttributes(wifi_signal_attributes).GetDrawable(0);
                // If sld is NULL then we are indexing and therefore do not have access to
                // (nor need to display) the drawable.
                if (sld != NULL) {
                    sld->SetState((security != SECURITY_NONE) ? STATE_SECURED : STATE_NONE);
                    drawable = sld->GetCurrent();
                    SetIcon(drawable);
                }
            }

            if (drawable != NULL) {
                drawable->SetLevel(level);
            }
        }
    }

    //@Override
    public Int32 CompareTo(Preference preference) {
        if (!(preference instanceof AccessPoint)) {
            return 1;
        }
        AccessPoint other = (AccessPoint) preference;
        // Active one goes first.
        if (mInfo != NULL && other.mInfo == NULL) return -1;
        if (mInfo == NULL && other.mInfo != NULL) return 1;

        // Reachable one goes before unreachable one.
        if (mRssi != Integer.MAX_VALUE && other.mRssi == Integer.MAX_VALUE) return -1;
        if (mRssi == Integer.MAX_VALUE && other.mRssi != Integer.MAX_VALUE) return 1;
        if (mRssi == Integer.MAX_VALUE && other.mRssi != Integer.MAX_VALUE) return 1;

        // Configured one goes before unconfigured one.
        if (networkId != WifiConfiguration.INVALID_NETWORK_ID
                && other.networkId == WifiConfiguration.INVALID_NETWORK_ID) return -1;
        if (networkId == WifiConfiguration.INVALID_NETWORK_ID
                && other.networkId != WifiConfiguration.INVALID_NETWORK_ID) return 1;

        // Sort by signal strength.
        Int32 difference = WifiManager->CompareSignalLevel(other.mRssi, mRssi);
        if (difference != 0) {
            return difference;
        }
        // Sort by ssid.
        return ssid->CompareToIgnoreCase(other.ssid);
    }

    //@Override
    public Boolean Equals(Object other) {
        if (!(other instanceof AccessPoint)) return FALSE;
        return (this->CompareTo((AccessPoint) other) == 0);
    }

    //@Override
    public Int32 HashCode() {
        Int32 result = 0;
        if (mInfo != NULL) result += 13 * mInfo->HashCode();
        result += 19 * mRssi;
        result += 23 * networkId;
        result += 29 * ssid->HashCode();
        return result;
    }

    Boolean Update(ScanResult result) {
        if (result.seen > mSeen) {
            mSeen = result.seen;
        }
        if (WifiSettings.mVerboseLogging > 0) {
            if (mScanResultCache == NULL) {
                mScanResultCache = new LruCache<String, ScanResult>(32);
            }
            mScanResultCache->Put(result.BSSID, result);
        }

        if (ssid->Equals(result.SSID) && security == GetSecurity(result)) {
            if (WifiManager->CompareSignalLevel(result.level, mRssi) > 0) {
                Int32 oldLevel = GetLevel();
                mRssi = result.level;
                if (GetLevel() != oldLevel) {
                    NotifyChanged();
                }
            }
            // This flag only comes from scans, is not easily saved in config
            if (security == SECURITY_PSK) {
                pskType = GetPskType(result);
            }
            mScanResult = result;
            Refresh();
            return TRUE;
        }
        return FALSE;
    }

    void Update(WifiInfo info, DetailedState state) {
        Boolean reorder = FALSE;
        if (info != NULL && networkId != WifiConfiguration.INVALID_NETWORK_ID
                && networkId == info->GetNetworkId()) {
            reorder = (mInfo == NULL);
            mRssi = info->GetRssi();
            mInfo = info;
            mState = state;
            Refresh();
        } else if (mInfo != NULL) {
            reorder = TRUE;
            mInfo = NULL;
            mState = NULL;
            Refresh();
        }
        if (reorder) {
            NotifyHierarchyChanged();
        }
    }

    Int32 GetLevel() {
        if (mRssi == Integer.MAX_VALUE) {
            return -1;
        }
        return WifiManager->CalculateSignalLevel(mRssi, 4);
    }

    WifiConfiguration GetConfig() {
        return mConfig;
    }

    WifiInfo GetInfo() {
        return mInfo;
    }

    DetailedState GetState() {
        return mState;
    }

    static String RemoveDoubleQuotes(String string) {
        Int32 length = string->Length();
        if ((length > 1) && (string->CharAt(0) == '"')
                && (string->CharAt(length - 1) == '"')) {
            return string->Substring(1, length - 1);
        }
        return string;
    }

    static String ConvertToQuotedString(String string) {
        return "\"" + string + "\"";
    }

    /**
     * Shows or Hides the Summary of an AccessPoint.
     *
     * @param showSummary TRUE will show the summary, FALSE will hide the summary
     */
    CARAPI SetShowSummary(Boolean showSummary){
        this.showSummary = showSummary;
    }

    /**
     * Returns the visibility status of the WifiConfiguration.
     *
     * @return autojoin debugging information
     * TODO: use a string formatter
     * ["rssi 5Ghz", "num results on 5GHz" / "rssi 5Ghz", "num results on 5GHz"]
     * For instance [-40,5/-30,2]
     */
    private String GetVisibilityStatus() {
        StringBuilder visibility = new StringBuilder();
        StringBuilder scans24GHz = NULL;
        StringBuilder scans5GHz = NULL;
        String bssid = NULL;

        Int64 now = System->CurrentTimeMillis();

        if (mInfo != NULL) {
            bssid = mInfo->GetBSSID();
            if (bssid != NULL) {
                visibility->Append(" ").Append(bssid);
            }
            visibility->Append(" score=").Append(mInfo.score);
            visibility->Append(" ");
            visibility->Append(String->Format("tx=%.1f,", mInfo.txSuccessRate));
            visibility->Append(String->Format("%.1f,", mInfo.txRetriesRate));
            visibility->Append(String->Format("%.1f ", mInfo.txBadRate));
            visibility->Append(String->Format("rx=%.1f", mInfo.rxSuccessRate));
        }

        if (mScanResultCache != NULL) {
            Int32 rssi5 = WifiConfiguration.INVALID_RSSI;
            Int32 rssi24 = WifiConfiguration.INVALID_RSSI;
            Int32 num5 = 0;
            Int32 num24 = 0;
            Int32 numBlackListed = 0;
            Int32 n24 = 0; // Number scan results we included in the string
            Int32 n5 = 0; // Number scan results we included in the string
            Map<String, ScanResult> list = mScanResultCache->Snapshot();
            // TODO: sort list by RSSI or age
            for (ScanResult result : list->Values()) {
                if (result.seen == 0)
                    continue;

                if (result.autoJoinStatus != ScanResult.ENABLED) numBlackListed++;

                if (result.frequency >= LOWER_FREQ_5GHZ
                        && result.frequency <= HIGHER_FREQ_5GHZ) {
                    // Strictly speaking: [4915, 5825]
                    // number of known BSSID on 5GHz band
                    num5 = num5 + 1;
                } else if (result.frequency >= LOWER_FREQ_24GHZ
                        && result.frequency <= HIGHER_FREQ_24GHZ) {
                    // Strictly speaking: [2412, 2482]
                    // number of known BSSID on 2.4Ghz band
                    num24 = num24 + 1;
                }

                // Ignore results seen, older than 20 seconds
                if (now - result.seen > VISIBILITY_OUTDATED_AGE_IN_MILLI) continue;

                if (result.frequency >= LOWER_FREQ_5GHZ
                        && result.frequency <= HIGHER_FREQ_5GHZ) {
                    if (result.level > rssi5) {
                        rssi5 = result.level;
                    }
                    if (n5 < 4) {
                        if (scans5GHz == NULL) scans5GHz = new StringBuilder();
                        scans5GHz->Append(" {").Append(result.BSSID);
                        if (bssid != NULL && result.BSSID->Equals(bssid)) scans5GHz->Append("*");
                        scans5GHz->Append("=").Append(result.frequency);
                        scans5GHz->Append(",").Append(result.level);
                        if (result.autoJoinStatus != 0) {
                            scans5GHz->Append(",st=").Append(result.autoJoinStatus);
                        }
                        if (result.numIpConfigFailures != 0) {
                            scans5GHz->Append(",ipf=").Append(result.numIpConfigFailures);
                        }
                        scans5GHz->Append("}");
                        n5++;
                    }
                } else if (result.frequency >= LOWER_FREQ_24GHZ
                        && result.frequency <= HIGHER_FREQ_24GHZ) {
                    if (result.level > rssi24) {
                        rssi24 = result.level;
                    }
                    if (n24 < 4) {
                        if (scans24GHz == NULL) scans24GHz = new StringBuilder();
                        scans24GHz->Append(" {").Append(result.BSSID);
                        if (bssid != NULL && result.BSSID->Equals(bssid)) scans24GHz->Append("*");
                        scans24GHz->Append("=").Append(result.frequency);
                        scans24GHz->Append(",").Append(result.level);
                        if (result.autoJoinStatus != 0) {
                            scans24GHz->Append(",st=").Append(result.autoJoinStatus);
                        }
                        if (result.numIpConfigFailures != 0) {
                            scans24GHz->Append(",ipf=").Append(result.numIpConfigFailures);
                        }
                        scans24GHz->Append("}");
                        n24++;
                    }
                }
            }
            visibility->Append(" [");
            if (num24 > 0) {
                visibility->Append("(").Append(num24).Append(")");
                if (n24 <= 4) {
                    if (scans24GHz != NULL) {
                        visibility->Append(scans24GHz->ToString());
                    }
                } else {
                    visibility->Append("max=").Append(rssi24);
                    if (scans24GHz != NULL) {
                        visibility->Append(",").Append(scans24GHz->ToString());
                    }
                }
            }
            visibility->Append(";");
            if (num5 > 0) {
                visibility->Append("(").Append(num5).Append(")");
                if (n5 <= 4) {
                    if (scans5GHz != NULL) {
                        visibility->Append(scans5GHz->ToString());
                    }
                } else {
                    visibility->Append("max=").Append(rssi5);
                    if (scans5GHz != NULL) {
                        visibility->Append(",").Append(scans5GHz->ToString());
                    }
                }
            }
            if (numBlackListed > 0)
                visibility->Append("!").Append(numBlackListed);
            visibility->Append("]");
        } else {
            if (mRssi != Integer.MAX_VALUE) {
                visibility->Append(" rssi=");
                visibility->Append(mRssi);
                if (mScanResult != NULL) {
                    visibility->Append(", f=");
                    visibility->Append(mScanResult.frequency);
                }
            }
        }

        return visibility->ToString();
    }

    /**
     * Updates the title and summary; may indirectly call NotifyChanged().
     */
    private void Refresh() {
        SetTitle(ssid);

        final Context context = GetContext();
        UpdateIcon(GetLevel(), context);

        // Force new summary
        SetSummary(NULL);

        // Update to new summary
        StringBuilder summary = new StringBuilder();

        if (mState != NULL) { // This is the active connection
            summary->Append(Summary->Get(context, mState));
        } else if (mConfig != NULL && ((mConfig.status == WifiConfiguration.Status.DISABLED &&
                mConfig.disableReason != WifiConfiguration.DISABLED_UNKNOWN_REASON)
               || mConfig.autoJoinStatus
                >= WifiConfiguration.AUTO_JOIN_DISABLED_ON_AUTH_FAILURE)) {
            if (mConfig.autoJoinStatus
                    >= WifiConfiguration.AUTO_JOIN_DISABLED_ON_AUTH_FAILURE) {
                if (mConfig.disableReason == WifiConfiguration.DISABLED_DHCP_FAILURE) {
                    summary->Append(context->GetString(R::string::wifi_disabled_network_failure));
                } else {
                    summary->Append(context->GetString(R::string::wifi_disabled_password_failure));
                }
            } else {
                switch (mConfig.disableReason) {
                    case WifiConfiguration.DISABLED_AUTH_FAILURE:
                        summary->Append(context->GetString(R::string::wifi_disabled_password_failure));
                        break;
                    case WifiConfiguration.DISABLED_DHCP_FAILURE:
                    case WifiConfiguration.DISABLED_DNS_FAILURE:
                        summary->Append(context->GetString(R::string::wifi_disabled_network_failure));
                        break;
                    case WifiConfiguration.DISABLED_UNKNOWN_REASON:
                    case WifiConfiguration.DISABLED_ASSOCIATION_REJECT:
                        summary->Append(context->GetString(R::string::wifi_disabled_generic));
                        break;
                }
            }
        } else if (mRssi == Integer.MAX_VALUE) { // Wifi out of range
            summary->Append(context->GetString(R::string::wifi_not_in_range));
        } else { // In range, not disabled.
            if (mConfig != NULL) { // Is saved network
                summary->Append(context->GetString(R::string::wifi_remembered));
            }
        }

        if (WifiSettings.mVerboseLogging > 0) {
            //add RSSI/band information for this config, what was seen up to 6 seconds ago
            //verbose WiFi Logging is only turned on thru developers settings
            if (mInfo != NULL && mState != NULL) { // This is the active connection
                summary->Append(" (f=" + Integer->ToString(mInfo->GetFrequency()) + ")");
            }
            summary->Append(" " + GetVisibilityStatus());
            if (mConfig != NULL && mConfig.autoJoinStatus > 0) {
                summary->Append(" (" + mConfig.autoJoinStatus);
                if (mConfig.blackListTimestamp > 0) {
                    Int64 now = System->CurrentTimeMillis();
                    Int64 diff = (now - mConfig.blackListTimestamp)/1000;
                    Int64 sec = diff%60; //seconds
                    Int64 min = (diff/60)%60; //minutes
                    Int64 hour = (min/60)%60; //hours
                    summary->Append(", ");
                    if (hour > 0) summary->Append(Long->ToString(hour) + "h ");
                    summary->Append( Long->ToString(min) + "m ");
                    summary->Append( Long->ToString(sec) + "s ");
                }
                summary->Append(")");
            }
        }

        if (summary->Length() > 0) {
            SetSummary(summary->ToString());
        } else {
            showSummary = FALSE;
        }
    }

    /**
     * Generate and save a default wifiConfiguration with common values.
     * Can only be called for unsecured networks.
     * @hide
     */
    protected void GenerateOpenNetworkConfig() {
        if (security != SECURITY_NONE)
            throw new IllegalStateException();
        if (mConfig != NULL)
            return;
        mConfig = new WifiConfiguration();
        mConfig.SSID = AccessPoint->ConvertToQuotedString(ssid);
        mConfig.allowedKeyManagement->Set(KeyMgmt.NONE);
    }
}
