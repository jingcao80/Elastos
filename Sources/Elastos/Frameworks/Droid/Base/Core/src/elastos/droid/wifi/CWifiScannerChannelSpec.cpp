
#include "elastos/droid/wifi/CWifiScannerChannelSpec.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CWifiScannerChannelSpec, Object, IWifiScannerChannelSpec)

CAR_OBJECT_IMPL(CWifiScannerChannelSpec)

ECode CWifiScannerChannelSpec::constructor(
    /* [in] */ Int32 frequency)
{
    mFrequency = frequency;
    mPassive = FALSE;
    mDwellTimeMS = 0;
    return NOERROR;
}

/**
 * channel frequency in MHz; for example channel 1 is specified as 2412
 */
ECode CWifiScannerChannelSpec::GetFrequency(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFrequency;
    return NOERROR;
}

ECode CWifiScannerChannelSpec::SetFrequency(
    /* [in] */ Int32 frequency)
{
    mFrequency = frequency;
    return NOERROR;
}

/**
 * if true, scan this channel in passive fashion.
 * This flag is ignored on DFS channel specification.
 * @hide
 */
ECode CWifiScannerChannelSpec::GetPassive(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPassive;
    return NOERROR;
}

ECode CWifiScannerChannelSpec::SetPassive(
    /* [in] */ Boolean passive)
{
    mPassive = passive;
    return NOERROR;
}

/**
 * how long to dwell on this channel
 * @hide
 */
ECode CWifiScannerChannelSpec::GetDwellTimeMS(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDwellTimeMS;
    return NOERROR;
}

ECode CWifiScannerChannelSpec::SetDwellTimeMS(
    /* [in] */ Int32 dwellTimeMS)
{
    mDwellTimeMS = dwellTimeMS;
    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
