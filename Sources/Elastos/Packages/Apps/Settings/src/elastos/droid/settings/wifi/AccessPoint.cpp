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

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/settings/wifi/AccessPoint.h"
#include "elastos/droid/settings/wifi/Summary.h"
#include "elastos/droid/settings/wifi/WifiSettings.h"
#include "../R.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::Drawable::IStateListDrawable;
using Elastos::Droid::Net::INetworkInfoHelper;
using Elastos::Droid::Net::CNetworkInfoHelper;
using Elastos::Droid::Net::NetworkInfoDetailedState_IDLE;
using Elastos::Droid::Net::NetworkInfoDetailedState_SCANNING;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTING;
using Elastos::Droid::Net::NetworkInfoDetailedState_AUTHENTICATING;
using Elastos::Droid::Net::NetworkInfoDetailedState_OBTAINING_IPADDR;
using Elastos::Droid::Net::NetworkInfoDetailedState_CONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_SUSPENDED;
using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTING;
using Elastos::Droid::Net::NetworkInfoDetailedState_DISCONNECTED;
using Elastos::Droid::Net::NetworkInfoDetailedState_FAILED;
using Elastos::Droid::Net::NetworkInfoDetailedState_BLOCKED;
using Elastos::Droid::Net::NetworkInfoDetailedState_VERIFYING_POOR_LINK;
using Elastos::Droid::Net::NetworkInfoDetailedState_CAPTIVE_PORTAL_CHECK;
using Elastos::Droid::Net::NetworkInfoDetailedState_NONE;
using Elastos::Droid::Wifi::IWifiConfigurationStatus;
using Elastos::Droid::Wifi::IWifiConfigurationKeyMgmt;
using Elastos::Droid::Wifi::CWifiManagerHelper;
using Elastos::Droid::Wifi::CWifiConfiguration;
using Elastos::Droid::Wifi::CWifiConfigurationHelper;
using Elastos::Droid::Wifi::IWifiConfigurationHelper;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Utility::CLruCache;
using Elastos::Core::CoreUtils;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::IBitSet;
using Elastos::Utility::IMap;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Wifi {

const String AccessPoint::TAG("Settings.AccessPoint");
const Int32 AccessPoint::LOWER_FREQ_24GHZ = 2400;
const Int32 AccessPoint::HIGHER_FREQ_24GHZ = 2500;
const Int32 AccessPoint::LOWER_FREQ_5GHZ = 4900;
const Int32 AccessPoint::HIGHER_FREQ_5GHZ = 5900;

const Int32 AccessPoint::SECURITY_NONE;
const Int32 AccessPoint::SECURITY_WEP;
const Int32 AccessPoint::SECURITY_PSK;
const Int32 AccessPoint::SECURITY_EAP;

const String AccessPoint::KEY_DETAILEDSTATE("key_detailedstate");
const String AccessPoint::KEY_WIFIINFO("key_wifiinfo");
const String AccessPoint::KEY_SCANRESULT("key_scanresult");
const String AccessPoint::KEY_CONFIG("key_config");

static AutoPtr< ArrayOf<Int32> > InitArrayOfInt32(
    /* [in] */ Int32 id)
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(1);
    (*args)[0] = id;
    return args;
}

const AutoPtr< ArrayOf<Int32> > AccessPoint::STATE_SECURED = InitArrayOfInt32(R::attr::state_encrypted);

const AutoPtr< ArrayOf<Int32> > AccessPoint::STATE_NONE = ArrayOf<Int32>::Alloc(0);

AutoPtr< ArrayOf<Int32> > AccessPoint::wifi_signal_attributes = InitArrayOfInt32(R::attr::wifi_signal);

const Int32 AccessPoint::VISIBILITY_MAX_AGE_IN_MILLI = 1000000;
const Int32 AccessPoint::VISIBILITY_OUTDATED_AGE_IN_MILLI = 20000;
const Int32 AccessPoint::SECOND_TO_MILLI = 1000;

AutoPtr<IWifiManagerHelper> AccessPoint::sWifiMgrHelper;

CAR_INTERFACE_IMPL(AccessPoint, Preference, IAccessPoint);

AutoPtr<IWifiManagerHelper> AccessPoint::GetWifiManagerHelper()
{
    if (sWifiMgrHelper == NULL) {
        CWifiManagerHelper::AcquireSingleton((IWifiManagerHelper**)&sWifiMgrHelper);
    }
    return sWifiMgrHelper;
}

AccessPoint::AccessPoint()
    : mSecurity(0)
    , mNetworkId(-1)
    , mWpsAvailable(FALSE)
    , mShowSummary(TRUE)
    , mPskType(AccessPointPskType_UNKNOWN)
    , mRssi(Elastos::Core::Math::INT32_MAX_VALUE)
    , mSeen(0)
    , mState(NetworkInfoDetailedState_NONE)
{}

AccessPoint::~AccessPoint()
{}

ECode AccessPoint::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IWifiConfiguration* config)
{
    Preference::constructor(context);

    LoadConfig(config);
    Refresh();
    return NOERROR;
}

ECode AccessPoint::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IScanResult* result)
{
    Preference::constructor(context);

    LoadResult(result);
    Refresh();
    return NOERROR;
}

ECode AccessPoint::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IBundle* savedState)
{
    Preference::constructor(context);

    AutoPtr<IParcelable> parcel;
    savedState->GetParcelable(KEY_CONFIG, (IParcelable**)&parcel);
    mConfig = IWifiConfiguration::Probe(parcel);
    if (mConfig != NULL) {
        LoadConfig(mConfig);
    }

    parcel = NULL;
    savedState->GetParcelable(KEY_SCANRESULT, (IParcelable**)&parcel);
    mScanResult = IScanResult::Probe(parcel);
    if (mScanResult != NULL) {
        LoadResult(mScanResult);
    }

    parcel = NULL;
    savedState->GetParcelable(KEY_WIFIINFO, (IParcelable**)&parcel);
    mInfo = IWifiInfo::Probe(parcel);

    Boolean res;
    if (savedState->ContainsKey(KEY_DETAILEDSTATE, &res), res) {
        String str;
        savedState->GetString(KEY_DETAILEDSTATE, &str);
        AutoPtr<INetworkInfoHelper> helper;
        CNetworkInfoHelper::AcquireSingleton((INetworkInfoHelper**)&helper);
        helper->NetworkInfoDetailedStateFromString(str, &mState);
    }
    Update(mInfo, mState);
    return NOERROR;
}

Int32 AccessPoint::GetSecurity(
    /* [in] */ IWifiConfiguration* config)
{
    AutoPtr<IBitSet> allowedKeyManagement;
    config->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
    Boolean res;
    if (allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_PSK, &res), res) {
        return SECURITY_PSK;
    }
    if ((allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::WPA_EAP, &res), res) ||
            (allowedKeyManagement->Get(IWifiConfigurationKeyMgmt::IEEE8021X, &res), res)) {
        return SECURITY_EAP;
    }

    AutoPtr< ArrayOf<String> > wepKeys;
    config->GetWepKeys((ArrayOf<String>**)&wepKeys);
    return (!(*wepKeys)[0].IsNull()) ? SECURITY_WEP : SECURITY_NONE;
}

Int32 AccessPoint::GetSecurity(
    /* [in] */ IScanResult* result)
{
    String capabilities;
    result->GetCapabilities(&capabilities);
    if (capabilities.Contains("WEP")) {
        return SECURITY_WEP;
    }
    else if (capabilities.Contains("PSK")) {
        return SECURITY_PSK;
    }
    else if (capabilities.Contains("EAP")) {
        return SECURITY_EAP;
    }
    return SECURITY_NONE;
}

String AccessPoint::GetSecurityString(
    /* [in] */ Boolean concise)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    String str1, str2;
    switch(mSecurity) {
        case SECURITY_EAP:
            return concise ? (context->GetString(R::string::wifi_security_short_eap, &str1), str1) :
                (context->GetString(R::string::wifi_security_eap, &str2), str2);
        case SECURITY_PSK:
            switch (mPskType) {
                case AccessPointPskType_WPA:
                    return concise ? (context->GetString(R::string::wifi_security_short_wpa, &str1), str1) :
                        (context->GetString(R::string::wifi_security_wpa, &str2), str2);
                case AccessPointPskType_WPA2:
                    return concise ? (context->GetString(R::string::wifi_security_short_wpa2, &str1), str1) :
                        (context->GetString(R::string::wifi_security_wpa2, &str2), str2);
                case AccessPointPskType_WPA_WPA2:
                    return concise ? (context->GetString(R::string::wifi_security_short_wpa_wpa2, &str1), str1) :
                        (context->GetString(R::string::wifi_security_wpa_wpa2, &str2), str2);
                case AccessPointPskType_UNKNOWN:
                default:
                    return concise ? (context->GetString(R::string::wifi_security_short_psk_generic, &str1), str1)
                            : (context->GetString(R::string::wifi_security_psk_generic, &str2), str2);
            }
        case SECURITY_WEP:
            return concise ? (context->GetString(R::string::wifi_security_short_wep, &str1), str1) :
                (context->GetString(R::string::wifi_security_wep, &str2), str2);
        case SECURITY_NONE:
        default:
            return concise ? String("") : (context->GetString(R::string::wifi_security_none, &str1), str1);
    }
}

AccessPointPskType AccessPoint::GetPskType(
    /* [in] */ IScanResult* result)
{
    String capabilities;
    result->GetCapabilities(&capabilities);
    Boolean wpa = capabilities.Contains("WPA-PSK");
    Boolean wpa2 = capabilities.Contains("WPA2-PSK");
    if (wpa2 && wpa) {
        return AccessPointPskType_WPA_WPA2;
    }
    else if (wpa2) {
        return AccessPointPskType_WPA2;
    }
    else if (wpa) {
        return AccessPointPskType_WPA;
    }
    else {
        Logger::W(TAG, "Received abnormal flag string: %s", capabilities.string());
        return AccessPointPskType_UNKNOWN;
    }
}

ECode AccessPoint::SaveWifiState(
    /* [in] */ IBundle* savedState)
{
    savedState->PutParcelable(KEY_CONFIG, IParcelable::Probe(mConfig));
    savedState->PutParcelable(KEY_SCANRESULT, IParcelable::Probe(mScanResult));
    savedState->PutParcelable(KEY_WIFIINFO, IParcelable::Probe(mInfo));
    if (mState != NetworkInfoDetailedState_NONE) {
        AutoPtr<INetworkInfoHelper> helper;
        CNetworkInfoHelper::AcquireSingleton((INetworkInfoHelper**)&helper);
        String str;
        helper->NetworkInfoDetailedStateToString(mState, &str);
        savedState->PutString(KEY_DETAILEDSTATE, str);
    }
    return NOERROR;
}

void AccessPoint::LoadConfig(
    /* [in] */ IWifiConfiguration* config)
{
    String SSID;
    config->GetSSID(&SSID);
    mSsid = (SSID.IsNull() ? String("") : RemoveDoubleQuotes(SSID));
    config->GetBSSID(&mBssid);
    mSecurity = GetSecurity(config);
    config->GetNetworkId(&mNetworkId);
    mConfig = config;
}

void AccessPoint::LoadResult(
    /* [in] */ IScanResult* result)
{
    result->GetSSID(&mSsid);
    result->GetBSSID(&mBssid);
    mSecurity = GetSecurity(result);

    String capabilities;
    result->GetCapabilities(&capabilities);

    mWpsAvailable = mSecurity != SECURITY_EAP && capabilities.Contains("WPS");
    if (mSecurity == SECURITY_PSK)
        mPskType = GetPskType(result);

    result->GetLevel(&mRssi);
    mScanResult = result;
    Int64 seen;
    result->GetSeen(&seen);
    if (seen > mSeen) {
        mSeen = seen;
    }
}

ECode AccessPoint::OnBindView(
    /* [in] */ IView* view)
{
    Preference::OnBindView(view);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    UpdateIcon(GetLevel(), context);

    AutoPtr<IView> summaryView;
    view->FindViewById(Elastos::Droid::R::id::summary, (IView**)&summaryView);
    summaryView->SetVisibility(mShowSummary ? IView::VISIBLE : IView::GONE);

    NotifyChanged();
    return NOERROR;
}

void AccessPoint::UpdateIcon(
    /* [in] */ Int32 level,
    /* [in] */ IContext* context)
{
    if (level == -1) {
        SetIcon((IDrawable*)NULL);
    }
    else {
        AutoPtr<IDrawable> drawable;
        GetIcon((IDrawable**)&drawable);

        if (drawable == NULL) {
            // To avoid a drawing race condition, we first set the state (SECURE/NONE) and then
            // set the icon (drawable) to that state's drawable.
            AutoPtr<IResourcesTheme> theme;
            context->GetTheme((IResourcesTheme**)&theme);
            AutoPtr<ITypedArray> styles;
            theme->ObtainStyledAttributes(wifi_signal_attributes, (ITypedArray**)&styles);
            AutoPtr<IDrawable> draw;
            styles->GetDrawable(0, (IDrawable**)&draw);
            IStateListDrawable* sld = IStateListDrawable::Probe(draw);
            // If sld is NULL then we are indexing and therefore do not have access to
            // (nor need to display) the drawable.
            if (sld != NULL) {
                Boolean res;
                IDrawable* _sld = IDrawable::Probe(sld);
                _sld->SetState((mSecurity != SECURITY_NONE) ? STATE_SECURED : STATE_NONE, &res);
                _sld->GetCurrent((IDrawable**)&drawable);
                SetIcon(drawable);
            }
        }

        if (drawable != NULL) {
            Boolean res;
            drawable->SetLevel(level, &res);
        }
    }
}

ECode AccessPoint::CompareTo(
    /* [in] */ IInterface* preference,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    if (IAccessPoint::Probe(preference) == NULL) {
        *result = 1;
        return NOERROR;
    }

    AutoPtr<AccessPoint> other = (AccessPoint*)IAccessPoint::Probe(preference);
    // Active one goes first.
    if (mInfo != NULL && other->mInfo == NULL) {
        *result = -1;
        return NOERROR;
    }
    if (mInfo == NULL && other->mInfo != NULL) {
        *result = 1;
        return NOERROR;
    }

    // Reachable one goes before unreachable one.
    using Elastos::Core::Math;
    if (mRssi != Math::INT32_MAX_VALUE && other->mRssi == Math::INT32_MAX_VALUE) {
        *result = -1;
        return NOERROR;
    }
    if (mRssi == Math::INT32_MAX_VALUE && other->mRssi != Math::INT32_MAX_VALUE) {
        *result = 1;
        return NOERROR;
    }
    if (mRssi == Math::INT32_MAX_VALUE && other->mRssi != Math::INT32_MAX_VALUE) {
        *result = 1;
        return NOERROR;
    }

    // Configured one goes before unconfigured one.
    if (mNetworkId != IWifiConfiguration::INVALID_NETWORK_ID
            && other->mNetworkId == IWifiConfiguration::INVALID_NETWORK_ID) {
        *result = -1;
        return NOERROR;
    }
    if (mNetworkId == IWifiConfiguration::INVALID_NETWORK_ID
            && other->mNetworkId != IWifiConfiguration::INVALID_NETWORK_ID) {
        *result = 1;
        return NOERROR;
    }

    // Sort by signal strength.
    Int32 difference;
    GetWifiManagerHelper()->CompareSignalLevel(other->mRssi, mRssi, &difference);
    if (difference != 0) {
        *result = difference;
        return NOERROR;
    }
    // Sort by ssid.
    *result = mSsid.CompareIgnoreCase(other->mSsid);
    return NOERROR;
}

ECode AccessPoint::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    if (IAccessPoint::Probe(other) == NULL) {
        *res = FALSE;
        return NOERROR;
    }

    Int32 result;
    CompareTo(other, &result);
    *res = result == 0;
    return NOERROR;
}

ECode AccessPoint::GetHashCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code);

    Int32 result = 0;
    Int32 hashcode;
    if (mInfo != NULL) {
        IObject::Probe(mInfo)->GetHashCode(&hashcode);
        result += 13 * hashcode;
    }
    result += 19 * mRssi;
    result += 23 * mNetworkId;

    result += 29 * mSsid.GetHashCode();
    *code = result;
    return NOERROR;
}

Boolean AccessPoint::Update(
    /* [in] */ IScanResult* result)
{
    Int64 seen;
    result->GetSeen(&seen);
    if (seen > mSeen) {
        mSeen = seen;
    }
    if (WifiSettings::mVerboseLogging > 0) {
        if (mScanResultCache == NULL) {
            CLruCache::New(32, (ILruCache**)&mScanResultCache);
        }
        String BSSID;
        result->GetBSSID(&BSSID);
        AutoPtr<ICharSequence> key = CoreUtils::Convert(BSSID);
        mScanResultCache->Put(key, result, NULL);
    }

    String SSID;
    result->GetSSID(&SSID);
    if (mSsid.Equals(SSID) && mSecurity == GetSecurity(result)) {
        Int32 level;
        result->GetLevel(&level);
        Int32 difference;
        GetWifiManagerHelper()->CompareSignalLevel(level, mRssi, &difference);
        if (difference > 0) {
            Int32 oldLevel = GetLevel();
            result->GetLevel(&mRssi);
            if (GetLevel() != oldLevel) {
                NotifyChanged();
            }
        }
        // This flag only comes from scans, is not easily saved in config
        if (mSecurity == SECURITY_PSK) {
            mPskType = GetPskType(result);
        }
        mScanResult = result;
        Refresh();
        return TRUE;
    }
    return FALSE;
}

void AccessPoint::Update(
    /* [in] */ IWifiInfo* info,
    /* [in] */ NetworkInfoDetailedState state)
{
    Boolean reorder = FALSE;
    Int32 result;
    info->GetNetworkId(&result);
    if (info != NULL && mNetworkId != IWifiConfiguration::INVALID_NETWORK_ID
            && mNetworkId == result) {
        reorder = (mInfo == NULL);
        info->GetRssi(&mRssi);
        mInfo = info;
        mState = state;
        Refresh();
    }
    else if (mInfo != NULL) {
        reorder = TRUE;
        mInfo = NULL;
        mState = NetworkInfoDetailedState_NONE;
        Refresh();
    }
    if (reorder) {
        NotifyHierarchyChanged();
    }
}

Int32 AccessPoint::GetLevel()
{
    if (mRssi == Elastos::Core::Math::INT32_MAX_VALUE) {
        return -1;
    }

    Int32 level;
    GetWifiManagerHelper()->CalculateSignalLevel(mRssi, 4, &level);
    return level;
}

AutoPtr<IWifiConfiguration> AccessPoint::GetConfig()
{
    return mConfig;
}

AutoPtr<IWifiInfo> AccessPoint::GetInfo()
{
    return mInfo;
}

NetworkInfoDetailedState AccessPoint::GetState()
{
    return mState;
}

String AccessPoint::RemoveDoubleQuotes(
    /* [in] */ const String& string)
{
    Int32 length = string.GetLength();
    if ((length > 1) && (string.GetChar(0) == '"')
            && (string.GetChar(length - 1) == '"')) {
        return string.Substring(1, length - 1);
    }
    return string;
}

String AccessPoint::ConvertToQuotedString(
    /* [in] */ const String& string)
{
    StringBuilder builder;
    builder += "\"";
    builder += string;
    builder += "\"";
    return builder.ToString();
}

ECode AccessPoint::SetShowSummary(
    /* [in] */ Boolean showSummary)
{
    mShowSummary = showSummary;
    return NOERROR;
}

String AccessPoint::GetVisibilityStatus()
{
    AutoPtr<StringBuilder> visibility = new StringBuilder();
    AutoPtr<StringBuilder> scans24GHz = NULL;
    AutoPtr<StringBuilder> scans5GHz = NULL;
    String bssid;

    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    Int64 now;
    sys->GetCurrentTimeMillis(&now);

    if (mInfo != NULL) {
        mInfo->GetBSSID(&bssid);
        if (bssid != NULL) {
            visibility->Append(" ");
            visibility->Append(bssid);
        }
        visibility->Append(" score=");
        Int32 score;
        mInfo->GetScore(&score);
        visibility->Append(score);
        visibility->Append(" ");

        Double txSuccessRate;
        mInfo->GetTxSuccessRate(&txSuccessRate);
        String str("");
        str.AppendFormat("tx=%.1f,", txSuccessRate);
        visibility->Append(str);

        Double txRetriesRate;
        mInfo->GetTxRetriesRate(&txRetriesRate);
        str = "";
        str.AppendFormat("%.1f,", txRetriesRate);
        visibility->Append(str);

        Double txBadRate;
        mInfo->GetTxBadRate(&txBadRate);
        str = "";
        str.AppendFormat("%.1f ", txBadRate);
        visibility->Append(str);

        Double rxSuccessRate;
        mInfo->GetRxSuccessRate(&rxSuccessRate);
        str = "";
        str.AppendFormat("rx=%.1f", rxSuccessRate);
        visibility->Append(str);
    }

    if (mScanResultCache != NULL) {
        AutoPtr<IWifiConfigurationHelper> helper;
        CWifiConfigurationHelper::AcquireSingleton((IWifiConfigurationHelper**)&helper);
        Int32 INVALID_RSSI;
        helper->GetINVALID_RSSI(&INVALID_RSSI);
        Int32 rssi5 = INVALID_RSSI;
        Int32 rssi24 = INVALID_RSSI;
        Int32 num5 = 0;
        Int32 num24 = 0;
        Int32 numBlackListed = 0;
        Int32 n24 = 0; // Number scan results we included in the string
        Int32 n5 = 0; // Number scan results we included in the string

        AutoPtr<IMap> map;
        mScanResultCache->Snapshot((IMap**)&map);
        AutoPtr<ICollection> values;
        map->GetValues((ICollection**)&values);

        AutoPtr<IIterator> it;
        values->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> obj;
            it->GetNext((IInterface**)&obj);
            IScanResult* result = IScanResult::Probe(obj);
            Int64 seen;
            result->GetSeen(&seen);
            if (seen == 0) {
                continue;
            }

            Int32 autoJoinStatus;
            result->GetAutoJoinStatus(&autoJoinStatus);
            if (autoJoinStatus != IScanResult::ENABLED) numBlackListed++;

            Int32 frequency;
            result->GetFrequency(&frequency);
            if (frequency >= LOWER_FREQ_5GHZ
                    && frequency <= HIGHER_FREQ_5GHZ) {
                // Strictly speaking: [4915, 5825]
                // number of known BSSID on 5GHz band
                num5 = num5 + 1;
            }
            else if (frequency >= LOWER_FREQ_24GHZ
                    && frequency <= HIGHER_FREQ_24GHZ) {
                // Strictly speaking: [2412, 2482]
                // number of known BSSID on 2.4Ghz band
                num24 = num24 + 1;
            }

            // Ignore results seen, older than 20 seconds
            if (now - seen > VISIBILITY_OUTDATED_AGE_IN_MILLI) continue;

            if (frequency >= LOWER_FREQ_5GHZ
                    && frequency <= HIGHER_FREQ_5GHZ) {
                Int32 level;
                result->GetLevel(&level);
                if (level > rssi5) {
                    rssi5 = level;
                }
                if (n5 < 4) {
                    if (scans5GHz == NULL) scans5GHz = new StringBuilder();
                    scans5GHz->Append(" {");
                    String BSSID;
                    result->GetBSSID(&BSSID);
                    scans5GHz->Append(BSSID);
                    if (bssid != NULL && BSSID.Equals(bssid)) scans5GHz->Append("*");
                    scans5GHz->Append("=");
                    scans5GHz->Append(frequency);
                    scans5GHz->Append(",");
                    scans5GHz->Append(level);
                    if (autoJoinStatus != 0) {
                        scans5GHz->Append(",st=");
                        scans5GHz->Append(autoJoinStatus);
                    }

                    Int32 numIpConfigFailures;
                    result->GetNumIpConfigFailures(&numIpConfigFailures);
                    if (numIpConfigFailures != 0) {
                        scans5GHz->Append(",ipf=");
                        scans5GHz->Append(numIpConfigFailures);
                    }
                    scans5GHz->Append("}");
                    n5++;
                }
            }
            else if (frequency >= LOWER_FREQ_24GHZ
                    && frequency <= HIGHER_FREQ_24GHZ) {
                Int32 level;
                result->GetLevel(&level);
                if (level > rssi24) {
                    rssi24 = level;
                }
                if (n24 < 4) {
                    if (scans24GHz == NULL) scans24GHz = new StringBuilder();
                    scans24GHz->Append(" {");
                    String BSSID;
                    result->GetBSSID(&BSSID);
                    scans24GHz->Append(BSSID);
                    if (bssid != NULL && BSSID.Equals(bssid)) scans24GHz->Append("*");
                    scans24GHz->Append("=");
                    scans24GHz->Append(frequency);
                    scans24GHz->Append(",");
                    scans24GHz->Append(level);
                    if (autoJoinStatus != 0) {
                        scans24GHz->Append(",st=");
                        scans24GHz->Append(autoJoinStatus);
                    }

                    Int32 numIpConfigFailures;
                    result->GetNumIpConfigFailures(&numIpConfigFailures);
                    if (numIpConfigFailures != 0) {
                        scans24GHz->Append(",ipf=");
                        scans24GHz->Append(numIpConfigFailures);
                    }
                    scans24GHz->Append("}");
                    n24++;
                }
            }
        }
        visibility->Append(" [");
        if (num24 > 0) {
            visibility->Append("(");
            visibility->Append(num24);
            visibility->Append(")");
            if (n24 <= 4) {
                if (scans24GHz != NULL) {
                    visibility->Append(scans24GHz->ToString());
                }
            }
            else {
                visibility->Append("max=");
                visibility->Append(rssi24);
                if (scans24GHz != NULL) {
                    visibility->Append(",");
                    visibility->Append(scans24GHz->ToString());
                }
            }
        }
        visibility->Append(";");
        if (num5 > 0) {
            visibility->Append("(");
            visibility->Append(num5);
            visibility->Append(")");
            if (n5 <= 4) {
                if (scans5GHz != NULL) {
                    visibility->Append(scans5GHz->ToString());
                }
            }
            else {
                visibility->Append("max=");
                visibility->Append(rssi5);
                if (scans5GHz != NULL) {
                    visibility->Append(",");
                    visibility->Append(scans5GHz->ToString());
                }
            }
        }
        if (numBlackListed > 0)
            visibility->Append("!");
            visibility->Append(numBlackListed);
            visibility->Append("]");
    }
    else {
        if (mRssi != Elastos::Core::Math::INT32_MAX_VALUE) {
            visibility->Append(" rssi=");
            visibility->Append(mRssi);
            if (mScanResult != NULL) {
                visibility->Append(", f=");
                Int32 frequency;
                mScanResult->GetFrequency(&frequency);
                visibility->Append(frequency);
            }
        }
    }

    return visibility->ToString();
}

void AccessPoint::Refresh()
{
    SetTitle(CoreUtils::Convert(mSsid));

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    UpdateIcon(GetLevel(), context);

    // Force new summary
    SetSummary((ICharSequence*)NULL);

    // Update to new summary
    StringBuilder summary;

    Int32 status, disableReason, autoJoinStatus;
    String str;
    if (mState != NetworkInfoDetailedState_NONE) { // This is the active connection
        summary.Append(Summary::Get(context, mState));
    }
    else if (mConfig != NULL && (((mConfig->GetStatus(&status), status == IWifiConfigurationStatus::DISABLED) &&
            (mConfig->GetDisableReason(&disableReason), disableReason != IWifiConfiguration::DISABLED_UNKNOWN_REASON))
           || (mConfig->GetAutoJoinStatus(&autoJoinStatus), autoJoinStatus >= IWifiConfiguration::AUTO_JOIN_DISABLED_ON_AUTH_FAILURE))) {
        if (mConfig->GetAutoJoinStatus(&autoJoinStatus), autoJoinStatus >= IWifiConfiguration::AUTO_JOIN_DISABLED_ON_AUTH_FAILURE) {
            if (disableReason == IWifiConfiguration::DISABLED_DHCP_FAILURE) {
                context->GetString(R::string::wifi_disabled_network_failure, &str);
                summary.Append(str);
            }
            else {
                context->GetString(R::string::wifi_disabled_password_failure, &str);
                summary.Append(str);
            }
        }
        else {
            switch (disableReason) {
                case IWifiConfiguration::DISABLED_AUTH_FAILURE:
                    context->GetString(R::string::wifi_disabled_password_failure, &str);
                    summary.Append(str);
                    break;
                case IWifiConfiguration::DISABLED_DHCP_FAILURE:
                case IWifiConfiguration::DISABLED_DNS_FAILURE:
                    context->GetString(R::string::wifi_disabled_network_failure, &str);
                    summary.Append(str);
                    break;
                case IWifiConfiguration::DISABLED_UNKNOWN_REASON:
                case IWifiConfiguration::DISABLED_ASSOCIATION_REJECT:
                    context->GetString(R::string::wifi_disabled_generic, &str);
                    summary.Append(str);
                    break;
            }
        }
    }
    else if (mRssi == Elastos::Core::Math::INT32_MAX_VALUE) { // Wifi out of range
        context->GetString(R::string::wifi_not_in_range, &str);
        summary.Append(str);
    }
    else { // In range, not disabled.
        if (mConfig != NULL) { // Is saved network
            context->GetString(R::string::wifi_remembered, &str);
            summary.Append(str);
        }
    }

    if (WifiSettings::mVerboseLogging > 0) {
        //add RSSI/band information for this config, what was seen up to 6 seconds ago
        //verbose WiFi Logging is only turned on thru developers settings
        if (mInfo != NULL && mState != NetworkInfoDetailedState_NONE) { // This is the active connection
            Int32 result;
            mInfo->GetFrequency(&result);
            StringBuilder builder;
            builder += " (f=";
            builder += StringUtils::ToString(result);
            builder += ")";
            summary.Append(builder.ToString());
        }
        summary.Append(String(" ") + GetVisibilityStatus());
        if (mConfig != NULL && (mConfig->GetAutoJoinStatus(&autoJoinStatus), autoJoinStatus > 0)) {
            StringBuilder builder;
            builder += " (";
            builder += autoJoinStatus;
            summary.Append(builder.ToString());
            Int64 blackListTimestamp;
            mConfig->GetBlackListTimestamp(&blackListTimestamp);
            if (blackListTimestamp > 0) {
                AutoPtr<ISystem> sys;
                CSystem::AcquireSingleton((ISystem**)&sys);
                Int64 now;
                sys->GetCurrentTimeMillis(&now);
                Int64 diff = (now - blackListTimestamp)/1000;
                Int64 sec = diff%60; //seconds
                Int64 min = (diff/60)%60; //minutes
                Int64 hour = (min/60)%60; //hours
                summary.Append(", ");
                if (hour > 0) summary.Append(StringUtils::ToString(hour) + "h ");
                summary.Append(StringUtils::ToString(min) + "m ");
                summary.Append(StringUtils::ToString(sec) + "s ");
            }
            summary.Append(")");
        }
    }

    if (summary.GetLength() > 0) {
        SetSummary(CoreUtils::Convert(summary.ToString()));
    }
    else {
        mShowSummary = FALSE;
    }
}

ECode AccessPoint::GenerateOpenNetworkConfig()
{
    if (mSecurity != SECURITY_NONE) {
        return E_ILLEGAL_STATE_EXCEPTION;
        // throw new IllegalStateException();
    }
    if (mConfig != NULL) {
        return NOERROR;
    }
    CWifiConfiguration::New((IWifiConfiguration**)&mConfig);
    mConfig->SetSSID(AccessPoint::ConvertToQuotedString(mSsid));
    AutoPtr<IBitSet> allowedKeyManagement;
    mConfig->GetAllowedKeyManagement((IBitSet**)&allowedKeyManagement);
    allowedKeyManagement->Set(IWifiConfigurationKeyMgmt::NONE);
    return NOERROR;
}

} // namespace Wifi
} // namespace Settings
} // namespace Droid
} // namespace Elastos
