
#include "elastos/droid/wifi/CRttManagerParcelableRttResults.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CRttManagerParcelableRttResults, Object, IParcelable, IRttManagerParcelableRttResults)

CAR_OBJECT_IMPL(CRttManagerParcelableRttResults)

ECode CRttManagerParcelableRttResults::constructor(
    /* [in] */ ArrayOf<IRttManagerRttResult*>* results)
{
    mResults = results;
    return NOERROR;
}

ECode CRttManagerParcelableRttResults::GetResults(
    /* [out, callee] */ ArrayOf<IRttManagerRttResult*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mResults;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRttManagerParcelableRttResults::SetResults(
    /* [in] */ ArrayOf<IRttManagerRttResult*>* results)
{
    mResults = results;
    return NOERROR;
}

ECode CRttManagerParcelableRttResults::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CRttManagerParcelableRttResults::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    if (mResults != NULL) {
        Int32 length = mResults->GetLength();
        dest->WriteInt32(length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IRttManagerRttResult> result = (*mResults)[i];
            String bssid;
            result->GetBssid(&bssid);
            dest->WriteString(bssid);
            Int32 status;
            result->GetStatus(&status);
            dest->WriteInt32(status);
            Int32 requestType;
            result->GetRequestType(&requestType);
            dest->WriteInt32(requestType);
            Int64 ts;
            result->GetTs(&ts);
            dest->WriteInt64(ts);
            Int32 rssi;
            result->GetRssi(&rssi);
            dest->WriteInt32(rssi);
            Int32 rssi_spread;
            result->GetRssi_spread(&rssi_spread);
            dest->WriteInt32(rssi_spread);
            Int32 tx_rate;
            result->GetTx_rate(&tx_rate);
            dest->WriteInt32(tx_rate);
            Int64 rtt_ns;
            result->GetRtt_ns(&rtt_ns);
            dest->WriteInt64(rtt_ns);
            Int64 rtt_sd_ns;
            result->GetRtt_sd_ns(&rtt_sd_ns);
            dest->WriteInt64(rtt_sd_ns);
            Int64 rtt_spread_ns;
            result->GetRtt_spread_ns(&rtt_spread_ns);
            dest->WriteInt64(rtt_spread_ns);
            Int32 distance_cm;
            result->GetDistance_cm(&distance_cm);
            dest->WriteInt32(distance_cm);
            Int32 distance_sd_cm;
            result->GetDistance_sd_cm(&distance_sd_cm);
            dest->WriteInt32(distance_sd_cm);
            Int32 distance_spread_cm;
            result->GetDistance_spread_cm(&distance_spread_cm);
            dest->WriteInt32(distance_spread_cm);
        }
    }
    else {
        dest->WriteInt32(0);
    }

    return NOERROR;
}

ECode CRttManagerParcelableRttResults::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CRttManagerParcelableRttResults::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
