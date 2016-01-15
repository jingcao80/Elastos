
#include "elastos/droid/wifi/CWifiLinkLayerStats.h"
#include "elastos/droid/wifi/CWifiSsidHelper.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Wifi {

const String CWifiLinkLayerStats::TAG("WifiLinkLayerStats");

CAR_INTERFACE_IMPL_2(CWifiLinkLayerStats, Object, IWifiLinkLayerStats, IParcelable)

CAR_OBJECT_IMPL(CWifiLinkLayerStats)

ECode CWifiLinkLayerStats::constructor()
{
    mStatus = 0;
    mBeaconRx = 0;
    mRssiMgmt = 0;
    mRxmpduBe = 0;
    mTxmpduBe = 0;
    mLostmpduBe = 0;
    mRetriesBe = 0;
    mRxmpduBk = 0;
    mTxmpduBk = 0;
    mLostmpduBk = 0;
    mRetriesBk = 0;
    mRxmpduVi = 0;
    mTxmpduVi = 0;
    mLostmpduVi = 0;
    mRetriesVi = 0;
    mRxmpduVo = 0;
    mTxmpduVo = 0;
    mLostmpduVo = 0;
    mRetriesVo = 0;
    mOnTime = 0;
    mTxTime = 0;
    mRxTime = 0;
    mOnTimeScan = 0;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStatus;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetStatus(
    /* [in] */ Int32 status)
{
    mStatus = status;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetSSID(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSSID;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetSSID(
    /* [in] */ const String& SSID)
{
    mSSID = SSID;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetBSSID(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBSSID;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetBSSID(
    /* [in] */ const String& BSSID)
{
    mBSSID = BSSID;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetBeacon_rx(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBeaconRx;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetBeacon_rx(
    /* [in] */ Int32 beacon_rx)
{
    mBeaconRx = beacon_rx;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetRssi_mgmt(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRssiMgmt;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetRssi_mgmt(
    /* [in] */ Int32 rssi_mgmt)
{
    mRssiMgmt = rssi_mgmt;
    return NOERROR;
}

/* WME Best Effort Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries)*/
ECode CWifiLinkLayerStats::GetRxmpdu_be(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRxmpduBe;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetRxmpdu_be(
    /* [in] */ Int64 rxmpdu_be)
{
    mRxmpduBe = rxmpdu_be;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetTxmpdu_be(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxmpduBe;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetTxmpdu_be(
    /* [in] */ Int64 txmpdu_be)
{
    mTxmpduBe = txmpdu_be;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetLostmpdu_be(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLostmpduBe;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetLostmpdu_be(
    /* [in] */ Int64 lostmpdu_be)
{
    mLostmpduBe = lostmpdu_be;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetRetries_be(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRxmpduBe;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetRetries_be(
    /* [in] */ Int64 retries_be)
{
    mRetriesBe = retries_be;
    return NOERROR;
}

/* WME Background Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries) */
ECode CWifiLinkLayerStats::GetRxmpdu_bk(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRxmpduBk;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetRxmpdu_bk(
    /* [in] */ Int64 rxmpdu_bk)
{
    mRxmpduBk = rxmpdu_bk;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetTxmpdu_bk(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxmpduBk;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetTxmpdu_bk(
    /* [in] */ Int64 txmpdu_bk)
{
    mTxmpduBk = txmpdu_bk;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetLostmpdu_bk(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLostmpduBk;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetLostmpdu_bk(
    /* [in] */ Int64 lostmpdu_bk)
{
    mLostmpduBk = lostmpdu_bk;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetRetries_bk(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRetriesBk;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetRetries_bk(
    /* [in] */ Int64 retries_bk)
{
    mRetriesBk = retries_bk;
    return NOERROR;
}

/* WME Video Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries) */
ECode CWifiLinkLayerStats::GetRxmpdu_vi(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRxmpduVi;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetRxmpdu_vi(
    /* [in] */ Int64 rxmpdu_vi)
{
    mRxmpduVi = rxmpdu_vi;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetTxmpdu_vi(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxmpduVi;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetTxmpdu_vi(
    /* [in] */ Int64 txmpdu_vi)
{
    mTxmpduVi = txmpdu_vi;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetLostmpdu_vi(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLostmpduVi;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetLostmpdu_vi(
    /* [in] */ Int64 lostmpdu_vi)
{
    mLostmpduVi = lostmpdu_vi;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetRetries_vi(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRxmpduVi;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetRetries_vi(
    /* [in] */ Int64 retries_vi)
{
    mRetriesVi = retries_vi;
    return NOERROR;
}

/* WME Voice Access Category (receive mpdu, transmit mpdu, lost mpdu, number of retries) */
ECode CWifiLinkLayerStats::GetRxmpdu_vo(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRxmpduVo;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetRxmpdu_vo(
    /* [in] */ Int64 rxmpdu_vo)
{
    mRxmpduVo = rxmpdu_vo;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetTxmpdu_vo(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxmpduVo;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetTxmpdu_vo(
    /* [in] */ Int64 txmpdu_vo)
{
    mTxmpduVo = txmpdu_vo;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetLostmpdu_vo(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLostmpduVo;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetLostmpdu_vo(
    /* [in] */ Int64 lostmpdu_vo)
{
    mLostmpduVo = lostmpdu_vo;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetRetries_vo(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRetriesVo;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetRetries_vo(
    /* [in] */ Int64 retries_vo)
{
    mRetriesVo = retries_vo;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetOn_time(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOnTime;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetOn_time(
    /* [in] */ Int32 on_time)
{
    mOnTime = on_time;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetTx_time(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxTime;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetTx_time(
    /* [in] */ Int32 tx_time)
{
    mTxTime = tx_time;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetRx_time(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRxTime;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetRx_time(
    /* [in] */ Int32 rx_time)
{
    mRxTime = rx_time;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetOn_time_scan(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mOnTimeScan;
    return NOERROR;
}

ECode CWifiLinkLayerStats::SetOn_time_scan(
    /* [in] */ Int32 on_time_scan)
{
    mOnTimeScan = on_time_scan;
    return NOERROR;
}

/** {@hide} */
ECode CWifiLinkLayerStats::GetPrintableSsid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    if (mSSID == NULL) {
        *result = "";
        return NOERROR;
    }

    const Int32 length = mSSID.GetLength();
    if (length > 2 && (mSSID.GetChar(0) == '"') && mSSID.GetChar(length - 1) == '"') {
        *result = mSSID.Substring(1, length - 1);
        return NOERROR;
    }

    /** The ascii-encoded string format is P"<ascii-encoded-string>"
     * The decoding is implemented in the supplicant for a newly configured
     * network.
     */
    if (length > 3 && (mSSID.GetChar(0) == 'P') && (mSSID.GetChar(1) == '"') &&
            (mSSID.GetChar(length-1) == '"')) {
        AutoPtr<IWifiSsid> wifiSsid;
        AutoPtr<IWifiSsidHelper> helper;
        CWifiSsidHelper::AcquireSingleton((IWifiSsidHelper**)&helper);
        helper->CreateFromAsciiEncoded(
                mSSID.Substring(2, length - 1), (IWifiSsid**)&wifiSsid);
        assert(0);
        // TODO
        // return wifiSsid->ToString(result);
    }

    *result = mSSID;

    return NOERROR;
}

//@Override
/** {@hide} */
ECode CWifiLinkLayerStats::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sbuf;
    sbuf.Append(" WifiLinkLayerStats: ");
    sbuf.Append('\n');

    if (mSSID != NULL) {
        sbuf.Append(" SSID: ");
        sbuf.Append(mSSID);
        sbuf.Append('\n');
    }

    if (mBSSID != NULL) {
        sbuf.Append(" BSSID: ");
        sbuf.Append(mBSSID);
        sbuf.Append('\n');
    }

    sbuf.Append(" my bss beacon rx: ");
    sbuf.Append(mBeaconRx);
    sbuf.Append('\n');
    sbuf.Append(" RSSI mgmt: ");
    sbuf.Append(mRssiMgmt);
    sbuf.Append('\n');
    sbuf.Append(" BE : ");
    sbuf.Append(" rx=");
    sbuf.Append(mRxmpduBe);
    sbuf.Append(" tx=");
    sbuf.Append(mTxmpduBe);
    sbuf.Append(" lost=");
    sbuf.Append(mLostmpduBe);
    sbuf.Append(" retries=");
    sbuf.Append(mRetriesBe);
    sbuf.Append('\n');
    sbuf.Append(" BK : ");
    sbuf.Append(" rx=");
    sbuf.Append(mRxmpduBk);
    sbuf.Append(" tx=");
    sbuf.Append(mTxmpduBk);
    sbuf.Append(" lost=");
    sbuf.Append(mLostmpduBk);
    sbuf.Append(" retries=");
    sbuf.Append(mRetriesBk);
    sbuf.Append('\n');
    sbuf.Append(" VI : ");
    sbuf.Append(" rx=");
    sbuf.Append(mRxmpduVi);
    sbuf.Append(" tx=");
    sbuf.Append(mTxmpduVi);
    sbuf.Append(" lost=");
    sbuf.Append(mLostmpduVi);
    sbuf.Append(" retries=");
    sbuf.Append(mRetriesVi);
    sbuf.Append('\n');
    sbuf.Append(" VO : ");
    sbuf.Append(" rx=");
    sbuf.Append(mRxmpduVo);
    sbuf.Append(" tx=");
    sbuf.Append(mTxmpduVo);
    sbuf.Append(" lost=");
    sbuf.Append(mLostmpduVo);
    sbuf.Append(" retries=");
    sbuf.Append(mRetriesVo);
    sbuf.Append('\n');
    sbuf.Append(" on_time : ");
    sbuf.Append(mOnTime);
    sbuf.Append(" tx_time=");
    sbuf.Append(mTxTime);
    sbuf.Append(" rx_time=");
    sbuf.Append(mRxTime);
    sbuf.Append(" scan_time=");
    sbuf.Append(mOnTimeScan);
    sbuf.Append('\n');

    return sbuf.ToString(str);
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiLinkLayerStats::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiLinkLayerStats::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    dest->WriteString(mSSID);
    dest->WriteString(mBSSID);
    dest->WriteInt32(mOnTime);
    dest->WriteInt32(mTxTime);
    dest->WriteInt32(mRxTime);
    return dest->WriteInt32(mOnTimeScan);
}

ECode CWifiLinkLayerStats::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiLinkLayerStats::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
