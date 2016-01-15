
#include "elastos/droid/wifi/CWifiConfigurationHelper.h"
#include "elastos/droid/wifi/CWifiConfiguration.h"

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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
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
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos