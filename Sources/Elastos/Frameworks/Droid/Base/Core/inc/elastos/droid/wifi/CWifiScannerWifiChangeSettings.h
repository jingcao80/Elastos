
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISCANNERWIFICHANGESETTINGS_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISCANNERWIFICHANGESETTINGS_H__

#include "_Elastos_Droid_Wifi_CWifiScannerWifiChangeSettings.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiScannerWifiChangeSettings)
    , public Object
    , public IParcelable
    , public IWifiScannerWifiChangeSettings
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetRssiSampleSize(
        /* [out] */ Int32* result);

    CARAPI SetRssiSampleSize(
        /* [in] */ Int32 rssiSampleSize);

    CARAPI GetLostApSampleSize(
        /* [out] */ Int32* result);

    CARAPI SetLostApSampleSize(
        /* [in] */ Int32 lostApSampleSize);

    CARAPI GetUnchangedSampleSize(
        /* [out] */ Int32* result);

    CARAPI SetUnchangedSampleSize(
        /* [in] */ Int32 unchangedSampleSize);

    CARAPI GetMinApsBreachingThreshold(
        /* [out] */ Int32* result);

    CARAPI SetMinApsBreachingThreshold(
        /* [in] */ Int32 minApsBreachingThreshold);

    CARAPI GetPeriodInMs(
        /* [out] */ Int32* result);

    CARAPI SetPeriodInMs(
        /* [in] */ Int32 periodInMs);

    CARAPI GetBssidInfos(
        /* [out, callee] */ ArrayOf<IWifiScannerBssidInfo*>** result);

    CARAPI SetBssidInfos(
        /* [in] */ ArrayOf<IWifiScannerBssidInfo*>* bssidInfos);

    /** Implement the Parcelable interface {@hide} */
    CARAPI DescribeContents(
        /* [out] */ Int32* contents);

    /** Implement the Parcelable interface {@hide} */
    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest,
        /* [in] */ Int32 flags);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    /** Implement the Parcelable interface {@hide} */
    // public static final Creator<WifiChangeSettings> CREATOR =
    //         new Creator<WifiChangeSettings>() {
    //             public WifiChangeSettings createFromParcel(Parcel in) {
    //                 WifiChangeSettings settings = new WifiChangeSettings();
    //                 settings.rssiSampleSize = in.readInt();
    //                 settings.lostApSampleSize = in.readInt();
    //                 settings.unchangedSampleSize = in.readInt();
    //                 settings.minApsBreachingThreshold = in.readInt();
    //                 settings.periodInMs = in.readInt();
    //                 int len = in.readInt();
    //                 settings.bssidInfos = new BssidInfo[len];
    //                 for (int i = 0; i < len; i++) {
    //                     BssidInfo info = new BssidInfo();
    //                     info.bssid = in.readString();
    //                     info.low = in.readInt();
    //                     info.high = in.readInt();
    //                     info.frequencyHint = in.readInt();
    //                     settings.bssidInfos[i] = info;
    //                 }
    //                 return settings;
    //             }

    //             public WifiChangeSettings[] newArray(int size) {
    //                 return new WifiChangeSettings[size];
    //             }
    //         };

private:
    Int32 mRssiSampleSize;                          /* sample size for RSSI averaging */
    Int32 mLostApSampleSize;                        /* samples to confirm AP's loss */
    Int32 mUnchangedSampleSize;                     /* samples to confirm no change */
    Int32 mMinApsBreachingThreshold;                /* change threshold to trigger event */
    Int32 mPeriodInMs;                              /* scan period in millisecond */
    AutoPtr< ArrayOf<IWifiScannerBssidInfo*> > mBssidInfos;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFISCANNERWIFICHANGESETTINGS_H__
