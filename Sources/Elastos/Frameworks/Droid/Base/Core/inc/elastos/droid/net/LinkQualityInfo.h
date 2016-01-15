
#ifndef __ELASTOS_DROID_NET_LINKQUALITYINFO_H__
#define __ELASTOS_DROID_NET_LINKQUALITYINFO_H__

#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Net {

/**
 *  Class that represents useful attributes of generic network links
 *  such as the upload/download throughput or packet error rate.
 *  Generally speaking, you should be dealing with instances of
 *  LinkQualityInfo subclasses, such as {@link android.net.#WifiLinkQualityInfo}
 *  or {@link android.net.#MobileLinkQualityInfo} which provide additional
 *  information.
 *  @hide
 */
class LinkQualityInfo
    : public Object
    , public ILinkQualityInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    LinkQualityInfo();

    CARAPI constructor();

    /**
     * @hide
     */
    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * returns the type of network this link is connected to
     * @return network type as defined by {@link android.net.ConnectivityManager} or
     * {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetNetworkType(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetNetworkType(
        /* [in] */ Int32 networkType);

    /**
     * returns the signal strength normalized across multiple types of networks
     * @return an integer value from 0 - 99 or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetNormalizedSignalStrength(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetNormalizedSignalStrength(
        /* [in] */ Int32 normalizedSignalStrength);

    /**
     * returns the total number of packets sent or received in sample duration
     * @return number of packets or {@link android.net.LinkQualityInfo#UNKNOWN_LONG}
     */
    CARAPI GetPacketCount(
        /* [out] */ Int64* result);

    /**
     * @hide
     */
    CARAPI SetPacketCount(
        /* [in] */ Int64 packetCount);

    /**
     * returns the total number of packets errors encountered in sample duration
     * @return number of errors or {@link android.net.LinkQualityInfo#UNKNOWN_LONG}
     */
    CARAPI GetPacketErrorCount(
        /* [out] */ Int64* result);

    /**
     * @hide
     */
    CARAPI SetPacketErrorCount(
        /* [in] */ Int64 packetErrorCount);

    /**
     * returns the theoretical upload bandwidth of this network
     * @return bandwidth in Kbps or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetTheoreticalTxBandwidth(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetTheoreticalTxBandwidth(
        /* [in] */ Int32 theoreticalTxBandwidth);

    /**
     * returns the theoretical download bandwidth of this network
     * @return bandwidth in Kbps or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetTheoreticalRxBandwidth(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetTheoreticalRxBandwidth(
        /* [in] */ Int32 theoreticalRxBandwidth);

    /**
     * returns the theoretical latency of this network
     * @return latency in milliseconds or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetTheoreticalLatency(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetTheoreticalLatency(
        /* [in] */ Int32 theoreticalLatency);

    /**
     * returns the time stamp of the last sample
     * @return milliseconds elapsed since start and sample time or
     * {@link android.net.LinkQualityInfo#UNKNOWN_LONG}
     */
    CARAPI GetLastDataSampleTime(
        /* [out] */ Int64* result);

    /**
     * @hide
     */
    CARAPI SetLastDataSampleTime(
        /* [in] */ Int64 lastDataSampleTime);

    /**
     * returns the sample duration used
     * @return duration in milliseconds or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetDataSampleDuration(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetDataSampleDuration(
        /* [in] */ Int32 dataSampleDuration);

public:
    /**
     * Implement the Parcelable interface.
     */
    static const Int32 sOBJECT_TYPE_LINK_QUALITY_INFO;
    static const Int32 sOBJECT_TYPE_WIFI_LINK_QUALITY_INFO;
    static const Int32 sOBJECT_TYPE_MOBILE_LINK_QUALITY_INFO;

private:
    /**
     * Represents a value that you can use to test if an integer field is set to a good value
     */
    static const Int32 sUNKNOWN_INT;
    /**
     * Represents a value that you can use to test if a long field is set to a good value
     */
    static const Int64 sUNKNOWN_LONG;
    static const Int32 sNORMALIZED_MIN_SIGNAL_STRENGTH;
    static const Int32 sNORMALIZED_MAX_SIGNAL_STRENGTH;
    static const Int32 sNORMALIZED_SIGNAL_STRENGTH_RANGE;

    /* Network type as defined by ConnectivityManager */
    Int32 mNetworkType;

    Int32 mNormalizedSignalStrength;

    Int64 mPacketCount;
    Int64 mPacketErrorCount;
    Int32 mTheoreticalTxBandwidth;
    Int32 mTheoreticalRxBandwidth;
    Int32 mTheoreticalLatency;

    /* Timestamp when last sample was made available */
    Int64 mLastDataSampleTime;

    /* Sample duration in millisecond */
    Int32 mDataSampleDuration;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_LINKQUALITYINFO_H__
