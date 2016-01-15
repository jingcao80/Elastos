
#include "elastos/droid/wifi/CWifiScannerBssidInfo.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CWifiScannerBssidInfo, Object, IWifiScannerBssidInfo)

CAR_OBJECT_IMPL(CWifiScannerBssidInfo)

ECode CWifiScannerBssidInfo::constructor()
{
    mLow = 0;
    mHigh = 0;
    mFrequencyHint = 0;
    return NOERROR;
}

ECode CWifiScannerBssidInfo::GetBssid(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBssid;
    return NOERROR;
}

ECode CWifiScannerBssidInfo::SetBssid(
    /* [in] */ const String& bssid)
{
    mBssid = bssid;
    return NOERROR;
}

ECode CWifiScannerBssidInfo::GetLow(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mLow;
    return NOERROR;
}

ECode CWifiScannerBssidInfo::SetLow(
    /* [in] */ Int32 low)
{
    mLow = low;
    return NOERROR;
}

ECode CWifiScannerBssidInfo::GetHigh(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mHigh;
    return NOERROR;
}

ECode CWifiScannerBssidInfo::SetHigh(
    /* [in] */ Int32 high)
{
    mHigh = high;
    return NOERROR;
}

ECode CWifiScannerBssidInfo::GetFrequencyHint(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFrequencyHint;
    return NOERROR;
}

ECode CWifiScannerBssidInfo::SetFrequencyHint(
    /* [in] */ Int32 frequencyHint)
{
    mFrequencyHint = frequencyHint;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos