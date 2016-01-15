
#ifndef __ELASTOS_DROID_NET_MOBILELINKQUALITYINFO_H__
#define __ELASTOS_DROID_NET_MOBILELINKQUALITYINFO_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/net/LinkQualityInfo.h"

namespace Elastos {
namespace Droid {
namespace Net {

/**
 *  Class that represents useful attributes of mobile network links
 *  such as the upload/download throughput or error rate etc.
 *  @hide
 */
class MobileLinkQualityInfo
    : public LinkQualityInfo
    , public IMobileLinkQualityInfo
{
public:
    CAR_INTERFACE_DECL()

    MobileLinkQualityInfo();

    /**
     * @hide
     */
    static CARAPI CreateFromParcelBody(
        /* [in] */ IParcel* in,
        /* [out] */ IMobileLinkQualityInfo** result);

    /**
     * returns mobile network type as defined by {@link android.telephony.TelephonyManager}
     * @return network type or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetMobileNetworkType(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetMobileNetworkType(
        /* [in] */ Int32 mobileNetworkType);

    /**
     * returns signal strength for GSM networks
     * @return signal strength in db or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetRssi(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetRssi(
        /* [in] */ Int32 Rssi);

    /**
     * returns error rates for GSM networks
     * @return error rate or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetGsmErrorRate(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetGsmErrorRate(
        /* [in] */ Int32 gsmErrorRate);

    /**
     * returns signal strength for CDMA networks
     * @return signal strength in db or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetCdmaDbm(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetCdmaDbm(
        /* [in] */ Int32 cdmaDbm);

    /**
     * returns signal to noise ratio for CDMA networks
     * @return signal to noise ratio in db or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetCdmaEcio(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetCdmaEcio(
        /* [in] */ Int32 cdmaEcio);

    /**
     * returns signal strength for EVDO networks
     * @return signal strength in db or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetEvdoDbm(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetEvdoDbm(
        /* [in] */ Int32 evdoDbm);

    /**
     * returns signal to noise ratio for EVDO spectrum
     * @return signal to noise ration in db or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetEvdoEcio(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetEvdoEcio(
        /* [in] */ Int32 evdoEcio);

    /**
     * returns end-to-end signal to noise ratio for EVDO networks
     * @return signal to noise ration in db or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetEvdoSnr(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetEvdoSnr(
        /* [in] */ Int32 evdoSnr);

    /**
     * returns signal strength for LTE network
     * @return signal strength in db or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetLteSignalStrength(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetLteSignalStrength(
        /* [in] */ Int32 lteSignalStrength);

    /**
     * returns RSRP (Reference Signal Received Power) for LTE network
     * @return RSRP in db or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetLteRsrp(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetLteRsrp(
        /* [in] */ Int32 lteRsrp);

    /**
     * returns RSRQ (Reference Signal Received Quality) for LTE network
     * @return RSRQ ??? or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetLteRsrq(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetLteRsrq(
        /* [in] */ Int32 lteRsrq);

    /**
     * returns signal to noise ratio for LTE networks
     * @return signal to noise ration in db or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetLteRssnr(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetLteRssnr(
        /* [in] */ Int32 lteRssnr);

    /**
     * returns channel quality indicator for LTE networks
     * @return CQI or {@link android.net.LinkQualityInfo#UNKNOWN_INT}
     */
    CARAPI GetLteCqi(
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI SetLteCqi(
        /* [in] */ Int32 lteCqi);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* parcel);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    // Represents TelephonyManager.NetworkType
    Int32 mMobileNetworkType;

    Int32 mRssi;

    Int32 mGsmErrorRate;

    Int32 mCdmaDbm;

    Int32 mCdmaEcio;

    Int32 mEvdoDbm;

    Int32 mEvdoEcio;

    Int32 mEvdoSnr;

    Int32 mLteSignalStrength;

    Int32 mLteRsrp;

    Int32 mLteRsrq;

    Int32 mLteRssnr;

    Int32 mLteCqi;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_MOBILELINKQUALITYINFO_H__
