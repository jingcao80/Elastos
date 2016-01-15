
#ifndef __ELASTOS_DROID_NET_WIFI_CWIFISCANNERSCANSETTINGS_H__
#define __ELASTOS_DROID_NET_WIFI_CWIFISCANNERSCANSETTINGS_H__

#include "_Elastos_Droid_Wifi_CWifiScannerScanSettings.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CWifiScannerScanSettings)
    , public Object
    , public IParcelable
    , public IWifiScannerScanSettings
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    /** one of the WIFI_BAND values */
    CARAPI GetBand(
        /* [out] */ Int32* result);

    CARAPI SetBand(
        /* [in] */ Int32 band);

    /** list of channels; used when band is set to WIFI_BAND_UNSPECIFIED */
    CARAPI GetChannels(
        /* [out, callee] */ ArrayOf<IWifiScannerChannelSpec*>** result);

    CARAPI SetChannels(
        /* [in] */ ArrayOf<IWifiScannerChannelSpec*>* channels);

    /** period of background scan; in millisecond, 0 => single shot scan */
    CARAPI GetPeriodInMs(
        /* [out] */ Int32* result);

    CARAPI SetPeriodInMs(
        /* [in] */ Int32 periodInMs);

    /** must have a valid REPORT_EVENT value */
    CARAPI GetReportEvents(
        /* [out] */ Int32* result);

    CARAPI SetReportEvents(
        /* [in] */ Int32 reportEvents);

    /** defines number of bssids to cache from each scan */
    CARAPI GetNumBssidsPerScan(
        /* [out] */ Int32* result);

    CARAPI SetNumBssidsPerScan(
        /* [in] */ Int32 numBssidsPerScan);

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
    // public static final Creator<ScanSettings> CREATOR =
    //         new Creator<ScanSettings>() {
    //             public ScanSettings createFromParcel(Parcel in) {

    //                 ScanSettings settings = new ScanSettings();
    //                 settings.band = in.readInt();
    //                 settings.periodInMs = in.readInt();
    //                 settings.reportEvents = in.readInt();
    //                 settings.numBssidsPerScan = in.readInt();
    //                 int num_channels = in.readInt();
    //                 settings.channels = new ChannelSpec[num_channels];
    //                 for (int i = 0; i < num_channels; i++) {
    //                     int frequency = in.readInt();

    //                     ChannelSpec spec = new ChannelSpec(frequency);
    //                     spec.dwellTimeMS = in.readInt();
    //                     spec.passive = in.readInt() == 1;
    //                     settings.channels[i] = spec;
    //                 }

    //                 return settings;
    //             }

    //             public ScanSettings[] newArray(int size) {
    //                 return new ScanSettings[size];
    //             }
    //         };

private:
    /** one of the WIFI_BAND values */
    Int32 mBand;
    /** list of channels; used when band is set to WIFI_BAND_UNSPECIFIED */
    AutoPtr< ArrayOf<IWifiScannerChannelSpec*> > mChannels;
    /** period of background scan; in millisecond, 0 => single shot scan */
    Int32 mPeriodInMs;
    /** must have a valid REPORT_EVENT value */
    Int32 mReportEvents;
    /** defines number of bssids to cache from each scan */
    Int32 mNumBssidsPerScan;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CWIFISCANNERSCANSETTINGS_H__
