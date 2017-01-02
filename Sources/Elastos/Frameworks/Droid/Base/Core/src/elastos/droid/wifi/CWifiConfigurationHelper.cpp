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

#include "elastos/droid/wifi/CWifiConfigurationHelper.h"
#include "elastos/droid/wifi/CWifiConfiguration.h"
#include "elastos/droid/wifi/CWifiConfigurationKeyMgmt.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CWifiConfigurationHelper, Singleton, IWifiConfigurationHelper)

CAR_SINGLETON_IMPL(CWifiConfigurationHelper)

ECode CWifiConfigurationHelper::GetWepKeyVarName(
    /* [in] */ Int32 index,
    /* [out] */ String* wepKeyVarName)
{
    return CWifiConfiguration::GetWepKeyVarName(index, wepKeyVarName);
}

ECode CWifiConfigurationHelper::GetINVALID_RSSI(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -127;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetINVALID_RSSI(
    /* [in] */ Int32 INVALID_RSSI)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetUNWANTED_BLACKLIST_SOFT_RSSI_24(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -80;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetUNWANTED_BLACKLIST_SOFT_RSSI_24(
    /* [in] */ Int32 UNWANTED_BLACKLIST_SOFT_RSSI_24)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetUNWANTED_BLACKLIST_SOFT_RSSI_5(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -70;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetUNWANTED_BLACKLIST_SOFT_RSSI_5(
    /* [in] */ Int32 UNWANTED_BLACKLIST_SOFT_RSSI_5)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetGOOD_RSSI_24(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -65;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetGOOD_RSSI_24(
    /* [in] */ Int32 GOOD_RSSI_24)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetLOW_RSSI_24(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -77;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetLOW_RSSI_24(
    /* [in] */ Int32 LOW_RSSI_24)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetBAD_RSSI_24(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -87;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetBAD_RSSI_24(
    /* [in] */ Int32 BAD_RSSI_24)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetGOOD_RSSI_5(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -60;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetGOOD_RSSI_5(
    /* [in] */ Int32 GOOD_RSSI_5)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetLOW_RSSI_5(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -72;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetLOW_RSSI_5(
    /* [in] */ Int32 LOW_RSSI_5)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetBAD_RSSI_5(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -82;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetBAD_RSSI_5(
    /* [in] */ Int32 BAD_RSSI_5)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetUNWANTED_BLACKLIST_SOFT_BUMP(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 4;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetUNWANTED_BLACKLIST_SOFT_BUMP(
    /* [in] */ Int32 UNWANTED_BLACKLIST_SOFT_BUMP)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetUNWANTED_BLACKLIST_HARD_BUMP(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 8;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetUNWANTED_BLACKLIST_HARD_BUMP(
    /* [in] */ Int32 UNWANTED_BLACKLIST_HARD_BUMP)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetUNBLACKLIST_THRESHOLD_24_SOFT(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -77;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetUNBLACKLIST_THRESHOLD_24_SOFT(
    /* [in] */ Int32 UNBLACKLIST_THRESHOLD_24_SOFT)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetUNBLACKLIST_THRESHOLD_24_HARD(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -68;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetUNBLACKLIST_THRESHOLD_24_HARD(
    /* [in] */ Int32 UNBLACKLIST_THRESHOLD_24_HARD)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetUNBLACKLIST_THRESHOLD_5_SOFT(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -63;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetUNBLACKLIST_THRESHOLD_5_SOFT(
    /* [in] */ Int32 UNBLACKLIST_THRESHOLD_5_SOFT)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetUNBLACKLIST_THRESHOLD_5_HARD(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -56;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetUNBLACKLIST_THRESHOLD_5_HARD(
    /* [in] */ Int32 UNBLACKLIST_THRESHOLD_5_HARD)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetINITIAL_AUTO_JOIN_ATTEMPT_MIN_24(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -80;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetINITIAL_AUTO_JOIN_ATTEMPT_MIN_24(
    /* [in] */ Int32 INITIAL_AUTO_JOIN_ATTEMPT_MIN_24)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetINITIAL_AUTO_JOIN_ATTEMPT_MIN_5(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -70;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetINITIAL_AUTO_JOIN_ATTEMPT_MIN_5(
    /* [in] */ Int32 INITIAL_AUTO_JOIN_ATTEMPT_MIN_5)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetA_BAND_PREFERENCE_RSSI_THRESHOLD(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -65;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetA_BAND_PREFERENCE_RSSI_THRESHOLD(
    /* [in] */ Int32 A_BAND_PREFERENCE_RSSI_THRESHOLD)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetG_BAND_PREFERENCE_RSSI_THRESHOLD(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = -75;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetG_BAND_PREFERENCE_RSSI_THRESHOLD(
    /* [in] */ Int32 G_BAND_PREFERENCE_RSSI_THRESHOLD)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetHOME_NETWORK_RSSI_BOOST(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 5;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetHOME_NETWORK_RSSI_BOOST(
    /* [in] */ Int32 HOME_NETWORK_RSSI_BOOST)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::GetMAX_INITIAL_AUTO_JOIN_RSSI_BOOST(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 8;
    return NOERROR;
}

ECode CWifiConfigurationHelper::SetMAX_INITIAL_AUTO_JOIN_RSSI_BOOST(
    /* [in] */ Int32 MAX_INITIAL_AUTO_JOIN_RSSI_BOOST)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiConfigurationHelper::ConfigKey(
    /* [in] */ IScanResult* scanResult,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String ssid;
    scanResult->GetSSID(&ssid);
    String key = String("\"") + ssid + "\"";

    String capabilities;
    scanResult->GetCapabilities(&capabilities);
    if (capabilities.Contains("WEP")) {
            key = key + "-WEP";
    }

    AutoPtr<IWifiConfigurationKeyMgmt> keyMgmt;
    CWifiConfigurationKeyMgmt::AcquireSingleton((IWifiConfigurationKeyMgmt**)&keyMgmt);
    AutoPtr<ArrayOf<String> > strings;
    keyMgmt->GetStrings((ArrayOf<String>**)&strings);
    if (capabilities.Contains("PSK")) {
            key = key + "-" + (*strings)[IWifiConfigurationKeyMgmt::WPA_PSK];
    }

    if (capabilities.Contains("EAP")) {
            key = key + "-" + (*strings)[IWifiConfigurationKeyMgmt::WPA_EAP];
    }

    *result = key;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
