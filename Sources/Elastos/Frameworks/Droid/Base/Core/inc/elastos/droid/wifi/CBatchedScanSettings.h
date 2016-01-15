
#ifndef __ELASTOS_DROID_NET_WIFI_CBATCHEDSCANSETTINGS_H__
#define __ELASTOS_DROID_NET_WIFI_CBATCHEDSCANSETTINGS_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Wifi_CBatchedScanSettings.h"
#include <elastos/core/Object.h>

using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CBatchedScanSettings)
    , public Object
    , public IBatchedScanSettings
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CBatchedScanSettings();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IBatchedScanSettings* source);

    CARAPI GetMaxScansPerBatch(
        /* [out] */ Int32* result);

    CARAPI SetMaxScansPerBatch(
        /* [in] */ Int32 maxScansPerBatch);

    CARAPI GetMaxApPerScan(
        /* [out] */ Int32* result);

    CARAPI SetMaxApPerScan(
        /* [in] */ Int32 maxApPerScan);

    CARAPI GetChannelSet(
        /* [out] */ ICollection** result);

    CARAPI SetChannelSet(
        /* [in] */ ICollection* channelSet);

    CARAPI GetScanIntervalSec(
        /* [out] */ Int32* result);

    CARAPI SetScanIntervalSec(
        /* [in] */ Int32 scanIntervalSec);

    CARAPI GetMaxApForDistance(
        /* [out] */ Int32* result);

    CARAPI SetMaxApForDistance(
        /* [in] */ Int32 maxApForDistance);

    CARAPI Clear();

    /** @hide */
    CARAPI IsInvalid(
        /* [out] */ Boolean* result);

    /** @hide */
    CARAPI Constrain();

    //@Override
    CARAPI Equals(
        /* [in] */ IObject* obj,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI HashCode(
        /* [out] */ Int32* code);

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

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
    // public static final Creator<BatchedScanSettings> CREATOR =
    //     new Creator<BatchedScanSettings>() {
    //         public BatchedScanSettings createFromParcel(Parcel in) {
    //             BatchedScanSettings settings = new BatchedScanSettings();
    //             settings.maxScansPerBatch = in.readInt();
    //             settings.maxApPerScan = in.readInt();
    //             settings.scanIntervalSec = in.readInt();
    //             settings.maxApForDistance = in.readInt();
    //             int channelCount = in.readInt();
    //             if (channelCount > 0) {
    //                 settings.channelSet = new ArrayList(channelCount);
    //                 while (channelCount-- > 0) {
    //                     settings.channelSet.add(in.readString());
    //                 }
    //             }
    //             return settings;
    //         }

    //         public BatchedScanSettings[] newArray(int size) {
    //             return new BatchedScanSettings[size];
    //         }
    //     };

private:
    CARAPI_(Boolean) ChannelSetIsValid();

private:
    static const String TAG;

    /** The expected number of scans per batch.  Note that the firmware may drop scans
     *  leading to fewer scans during the normal batch scan duration.  This value need not
     *  be specified (may be set to {@link UNSPECIFIED}) by the application and we will try
     *  to scan as many times as the firmware can support.  If another app requests fewer
     *  scans per batch we will attempt to honor that.
     */
    Int32 mMaxScansPerBatch;

    /** The maximum desired AP listed per scan.  Fewer AP may be returned if that's all
     *  that the driver detected.  If another application requests more AP per scan that
     *  will take precedence.  The if more channels are detected than we request, the APs
     *  with the lowest signal strength will be dropped.
     */
    Int32 mMaxApPerScan;

    /** The channels used in the scan.  If all channels should be used, {@code null} may be
     *  specified.  If another application requests more channels or all channels, that
     *  will take precedence.
     */
    AutoPtr<ICollection> mChannelSet;

    /** The time between the start of two sequential scans, in seconds.  If another
     *  application requests more frequent scans, that will take precedence.  If this
     * value is less than the duration of a scan, the next scan should start immediately.
     */
    Int32 mScanIntervalSec;

    /** The number of the best (strongest signal) APs for which the firmware will
     *  attempt to get distance information (RTT).  Not all firmware supports this
     *  feature, so it may be ignored.  If another application requests a greater
     *  number, that will take precedence.
     */
    Int32 mMaxApForDistance;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CBATCHEDSCANSETTINGS_H__
