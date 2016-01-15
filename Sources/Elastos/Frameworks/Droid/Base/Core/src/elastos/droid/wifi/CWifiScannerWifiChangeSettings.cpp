
#include "elastos/droid/wifi/CWifiScannerWifiChangeSettings.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CWifiScannerWifiChangeSettings, Object, IWifiScannerWifiChangeSettings, IParcelable)

CAR_OBJECT_IMPL(CWifiScannerWifiChangeSettings)

ECode CWifiScannerWifiChangeSettings::constructor()
{
    mRssiSampleSize = 0;
    mLostApSampleSize = 0;
    mUnchangedSampleSize = 0;
    mMinApsBreachingThreshold = 0;
    mPeriodInMs = 0;
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::GetRssiSampleSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRssiSampleSize;
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::SetRssiSampleSize(
    /* [in] */ Int32 rssiSampleSize)
{
    mRssiSampleSize = rssiSampleSize;
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::GetLostApSampleSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLostApSampleSize;
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::SetLostApSampleSize(
    /* [in] */ Int32 lostApSampleSize)
{
    mLostApSampleSize = lostApSampleSize;
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::GetUnchangedSampleSize(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mUnchangedSampleSize;
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::SetUnchangedSampleSize(
    /* [in] */ Int32 unchangedSampleSize)
{
    mUnchangedSampleSize = unchangedSampleSize;
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::GetMinApsBreachingThreshold(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMinApsBreachingThreshold;
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::SetMinApsBreachingThreshold(
    /* [in] */ Int32 minApsBreachingThreshold)
{
    mMinApsBreachingThreshold = minApsBreachingThreshold;
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::GetPeriodInMs(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPeriodInMs;
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::SetPeriodInMs(
    /* [in] */ Int32 periodInMs)
{
    mPeriodInMs = periodInMs;
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::GetBssidInfos(
    /* [out, callee] */ ArrayOf<IWifiScannerBssidInfo*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBssidInfos;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiScannerWifiChangeSettings::SetBssidInfos(
    /* [in] */ ArrayOf<IWifiScannerBssidInfo*>* bssidInfos)
{
    mBssidInfos = bssidInfos;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerWifiChangeSettings::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerWifiChangeSettings::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    dest->WriteInt32(mRssiSampleSize);
    dest->WriteInt32(mLostApSampleSize);
    dest->WriteInt32(mUnchangedSampleSize);
    dest->WriteInt32(mMinApsBreachingThreshold);
    dest->WriteInt32(mPeriodInMs);
    if (mBssidInfos != NULL) {
        Int32 length = mBssidInfos->GetLength();
        dest->WriteInt32(length);
        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IWifiScannerBssidInfo> info = (*mBssidInfos)[i];
            String bssid;
            info->GetBssid(&bssid);
            Int32 low, high, frequencyHint;
            info->GetLow(&low);
            info->GetHigh(&high);
            info->GetFrequencyHint(&frequencyHint);
            dest->WriteString(bssid);
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

ECode CWifiScannerWifiChangeSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiScannerWifiChangeSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos