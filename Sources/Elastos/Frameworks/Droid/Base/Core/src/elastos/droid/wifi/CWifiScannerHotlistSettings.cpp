
#include "elastos/droid/wifi/CWifiScannerHotlistSettings.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CWifiScannerHotlistSettings, Object, IWifiScannerHotlistSettings, IParcelable)

CAR_OBJECT_IMPL(CWifiScannerHotlistSettings)

ECode CWifiScannerHotlistSettings::constructor()
{
    mApLostThreshold = 0;
    return NOERROR;
}

ECode CWifiScannerHotlistSettings::GetBssidInfos(
    /* [out, callee] */ ArrayOf<IWifiScannerBssidInfo*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBssidInfos;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiScannerHotlistSettings::SetBssidInfos(
    /* [in] */ ArrayOf<IWifiScannerBssidInfo*>* bssidInfos)
{
    mBssidInfos = bssidInfos;
    return NOERROR;
}

ECode CWifiScannerHotlistSettings::GetApLostThreshold(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mApLostThreshold;
    return NOERROR;
}

ECode CWifiScannerHotlistSettings::SetApLostThreshold(
    /* [in] */ Int32 apLostThreshold)
{
    mApLostThreshold = apLostThreshold;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerHotlistSettings::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerHotlistSettings::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    dest->WriteInt32(mApLostThreshold);

    if (mBssidInfos != NULL) {
        Int32 length = mBssidInfos->GetLength();
        dest->WriteInt32(length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IWifiScannerBssidInfo> info = (*mBssidInfos)[i];
            String bssid;
            info->GetBssid(&bssid);
            dest->WriteString(bssid);
            Int32 low, high, frequencyHint;
            info->GetLow(&low);
            info->GetHigh(&high);
            info->GetFrequencyHint(&frequencyHint);
            dest->WriteInt32(low);
            dest->WriteInt32(high);
            dest->WriteInt32(frequencyHint);
        }
    }
    else {
        dest->WriteInt32(0);
    }

    return NOERROR;
}

ECode CWifiScannerHotlistSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiScannerHotlistSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos