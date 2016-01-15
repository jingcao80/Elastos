
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISCANNERBSSIDINFO_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISCANNERBSSIDINFO_H__

#include "_Elastos_Droid_Wifi_CWifiScannerBssidInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiScannerBssidInfo)
    , public Object
    , public IWifiScannerBssidInfo
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetBssid(
        /* [out] */ String* result);

    CARAPI SetBssid(
        /* [in] */ const String& bssid);

    CARAPI GetLow(
        /* [out] */ Int32* result);

    CARAPI SetLow(
        /* [in] */ Int32 low);

    CARAPI GetHigh(
        /* [out] */ Int32* result);

    CARAPI SetHigh(
        /* [in] */ Int32 high);

    CARAPI GetFrequencyHint(
        /* [out] */ Int32* result);

    CARAPI SetFrequencyHint(
        /* [in] */ Int32 frequencyHint);

private:
    /** bssid of the access point; in XX:XX:XX:XX:XX:XX format */
    String mBssid;
    /** low signal strength threshold; more information at {@link ScanResult#level} */
    Int32 mLow;                                            /* minimum RSSI */
    /** high signal threshold; more information at {@link ScanResult#level} */
    Int32 mHigh;                                           /* maximum RSSI */
    /** channel frequency (in KHz) where you may find this BSSID */
    Int32 mFrequencyHint;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFISCANNERBSSIDINFO_H__
