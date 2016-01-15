
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATIONHELPER_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATIONHELPER_H__

#include "_Elastos_Droid_Wifi_CWifiConfigurationHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiConfigurationHelper)
    , public Singleton
    , public IWifiConfigurationHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetWepKeyVarName(
        /* [in] */ Int32 index,
        /* [out] */ String* wepKeyVarName);

    CARAPI GetINVALID_RSSI(
        /* [out] */ Int32* result);

    CARAPI SetINVALID_RSSI(
        /* [in] */ Int32 INVALID_RSSI);

    CARAPI GetUNWANTED_BLACKLIST_SOFT_RSSI_24(
        /* [out] */ Int32* result);

    CARAPI SetUNWANTED_BLACKLIST_SOFT_RSSI_24(
        /* [in] */ Int32 UNWANTED_BLACKLIST_SOFT_RSSI_24);

    CARAPI GetUNWANTED_BLACKLIST_SOFT_RSSI_5(
        /* [out] */ Int32* result);

    CARAPI SetUNWANTED_BLACKLIST_SOFT_RSSI_5(
        /* [in] */ Int32 UNWANTED_BLACKLIST_SOFT_RSSI_5);

    CARAPI GetGOOD_RSSI_24(
        /* [out] */ Int32* result);

    CARAPI SetGOOD_RSSI_24(
        /* [in] */ Int32 GOOD_RSSI_24);

    CARAPI GetLOW_RSSI_24(
        /* [out] */ Int32* result);

    CARAPI SetLOW_RSSI_24(
        /* [in] */ Int32 LOW_RSSI_24);

    CARAPI GetBAD_RSSI_24(
        /* [out] */ Int32* result);

    CARAPI SetBAD_RSSI_24(
        /* [in] */ Int32 BAD_RSSI_24);

    CARAPI GetGOOD_RSSI_5(
        /* [out] */ Int32* result);

    CARAPI SetGOOD_RSSI_5(
        /* [in] */ Int32 GOOD_RSSI_5);

    CARAPI GetLOW_RSSI_5(
        /* [out] */ Int32* result);

    CARAPI SetLOW_RSSI_5(
        /* [in] */ Int32 LOW_RSSI_5);

    CARAPI GetBAD_RSSI_5(
        /* [out] */ Int32* result);

    CARAPI SetBAD_RSSI_5(
        /* [in] */ Int32 BAD_RSSI_5);

    CARAPI GetUNWANTED_BLACKLIST_SOFT_BUMP(
        /* [out] */ Int32* result);

    CARAPI SetUNWANTED_BLACKLIST_SOFT_BUMP(
        /* [in] */ Int32 UNWANTED_BLACKLIST_SOFT_BUMP);

    CARAPI GetUNWANTED_BLACKLIST_HARD_BUMP(
        /* [out] */ Int32* result);

    CARAPI SetUNWANTED_BLACKLIST_HARD_BUMP(
        /* [in] */ Int32 UNWANTED_BLACKLIST_HARD_BUMP);

    CARAPI GetUNBLACKLIST_THRESHOLD_24_SOFT(
        /* [out] */ Int32* result);

    CARAPI SetUNBLACKLIST_THRESHOLD_24_SOFT(
        /* [in] */ Int32 UNBLACKLIST_THRESHOLD_24_SOFT);

    CARAPI GetUNBLACKLIST_THRESHOLD_24_HARD(
        /* [out] */ Int32* result);

    CARAPI SetUNBLACKLIST_THRESHOLD_24_HARD(
        /* [in] */ Int32 UNBLACKLIST_THRESHOLD_24_HARD);

    CARAPI GetUNBLACKLIST_THRESHOLD_5_SOFT(
        /* [out] */ Int32* result);

    CARAPI SetUNBLACKLIST_THRESHOLD_5_SOFT(
        /* [in] */ Int32 UNBLACKLIST_THRESHOLD_5_SOFT);

    CARAPI GetUNBLACKLIST_THRESHOLD_5_HARD(
        /* [out] */ Int32* result);

    CARAPI SetUNBLACKLIST_THRESHOLD_5_HARD(
        /* [in] */ Int32 UNBLACKLIST_THRESHOLD_5_HARD);

    CARAPI GetINITIAL_AUTO_JOIN_ATTEMPT_MIN_24(
        /* [out] */ Int32* result);

    CARAPI SetINITIAL_AUTO_JOIN_ATTEMPT_MIN_24(
        /* [in] */ Int32 INITIAL_AUTO_JOIN_ATTEMPT_MIN_24);

    CARAPI GetINITIAL_AUTO_JOIN_ATTEMPT_MIN_5(
        /* [out] */ Int32* result);

    CARAPI SetINITIAL_AUTO_JOIN_ATTEMPT_MIN_5(
        /* [in] */ Int32 INITIAL_AUTO_JOIN_ATTEMPT_MIN_5);

    CARAPI GetA_BAND_PREFERENCE_RSSI_THRESHOLD(
        /* [out] */ Int32* result);

    CARAPI SetA_BAND_PREFERENCE_RSSI_THRESHOLD(
        /* [in] */ Int32 A_BAND_PREFERENCE_RSSI_THRESHOLD);

    CARAPI GetG_BAND_PREFERENCE_RSSI_THRESHOLD(
        /* [out] */ Int32* result);

    CARAPI SetG_BAND_PREFERENCE_RSSI_THRESHOLD(
        /* [in] */ Int32 G_BAND_PREFERENCE_RSSI_THRESHOLD);

    CARAPI GetHOME_NETWORK_RSSI_BOOST(
        /* [out] */ Int32* result);

    CARAPI SetHOME_NETWORK_RSSI_BOOST(
        /* [in] */ Int32 HOME_NETWORK_RSSI_BOOST);

    CARAPI GetMAX_INITIAL_AUTO_JOIN_RSSI_BOOST(
        /* [out] */ Int32* result);

    CARAPI SetMAX_INITIAL_AUTO_JOIN_RSSI_BOOST(
        /* [in] */ Int32 MAX_INITIAL_AUTO_JOIN_RSSI_BOOST);

    CARAPI ConfigKey(
        /* [in] */ IScanResult* scanResult,
        /* [out] */ String* result);
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_NET_WIFI_CWIFICONFIGURATIONHELPER_H__
