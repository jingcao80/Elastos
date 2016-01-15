
#include "elastos/droid/wifi/CRttManagerRttResult.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CRttManagerRttResult, Object, IRttManagerRttResult)

CAR_OBJECT_IMPL(CRttManagerRttResult)

ECode CRttManagerRttResult::constructor()
{
    mStatus = 0;
    mRequestType = 0;
    mTs = 0;
    mRssi = 0;
    mRssiSpread = 0;
    mTxRate = 0;
    mRttNs = 0;
    mRttSdNs = 0;
    mRttSpreadNs = 0;
    mDistanceCm = 0;
    mDistanceSdCm = 0;
    mDistanceSpreadCm = 0;
    return NOERROR;
}

ECode CRttManagerRttResult::GetBssid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBssid;
    return NOERROR;
}

ECode CRttManagerRttResult::SetBssid(
    /* [in] */ const String& bssid)
{
    mBssid = bssid;
    return NOERROR;
}

ECode CRttManagerRttResult::GetStatus(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStatus;
    return NOERROR;
}

ECode CRttManagerRttResult::SetStatus(
    /* [in] */ Int32 status)
{
    mStatus = status;
    return NOERROR;
}

ECode CRttManagerRttResult::GetRequestType(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRequestType;
    return NOERROR;
}

ECode CRttManagerRttResult::SetRequestType(
    /* [in] */ Int32 requestType)
{
    mRequestType = requestType;
    return NOERROR;
}

ECode CRttManagerRttResult::GetTs(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTs;
    return NOERROR;
}

ECode CRttManagerRttResult::SetTs(
    /* [in] */ Int64 ts)
{
    mTs = ts;
    return NOERROR;
}

ECode CRttManagerRttResult::GetRssi(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRssi;
    return NOERROR;
}

ECode CRttManagerRttResult::SetRssi(
    /* [in] */ Int32 rssi)
{
    mRssi = rssi;
    return NOERROR;
}

ECode CRttManagerRttResult::GetRssi_spread(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRssiSpread;
    return NOERROR;
}

ECode CRttManagerRttResult::SetRssi_spread(
    /* [in] */ Int32 rssi_spread)
{
    mRssiSpread = rssi_spread;
    return NOERROR;
}

ECode CRttManagerRttResult::GetTx_rate(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mTxRate;
    return NOERROR;
}

ECode CRttManagerRttResult::SetTx_rate(
    /* [in] */ Int32 tx_rate)
{
    mTxRate = tx_rate;
    return NOERROR;
}

ECode CRttManagerRttResult::GetRtt_ns(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRttNs;
    return NOERROR;
}

ECode CRttManagerRttResult::SetRtt_ns(
    /* [in] */ Int64 rtt_ns)
{
    mRttNs = rtt_ns;
    return NOERROR;
}

ECode CRttManagerRttResult::GetRtt_sd_ns(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRttSdNs;
    return NOERROR;
}

ECode CRttManagerRttResult::SetRtt_sd_ns(
    /* [in] */ Int64 rtt_sd_ns)
{
    mRttSdNs = rtt_sd_ns;
    return NOERROR;
}

ECode CRttManagerRttResult::GetRtt_spread_ns(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRttSpreadNs;
    return NOERROR;
}

ECode CRttManagerRttResult::SetRtt_spread_ns(
    /* [in] */ Int64 rtt_spread_ns)
{
    mRttSpreadNs = rtt_spread_ns;
    return NOERROR;
}

ECode CRttManagerRttResult::GetDistance_cm(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDistanceCm;
    return NOERROR;
}

ECode CRttManagerRttResult::SetDistance_cm(
    /* [in] */ Int32 distance_cm)
{
    mDistanceCm = distance_cm;
    return NOERROR;
}

ECode CRttManagerRttResult::GetDistance_sd_cm(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDistanceSdCm;
    return NOERROR;
}

ECode CRttManagerRttResult::SetDistance_sd_cm(
    /* [in] */ Int32 distance_sd_cm)
{
    mDistanceSdCm = distance_sd_cm;
    return NOERROR;
}

ECode CRttManagerRttResult::GetDistance_spread_cm(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDistanceSpreadCm;
    return NOERROR;
}

ECode CRttManagerRttResult::SetDistance_spread_cm(
    /* [in] */ Int32 distance_spread_cm)
{
    mDistanceSpreadCm = distance_spread_cm;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
