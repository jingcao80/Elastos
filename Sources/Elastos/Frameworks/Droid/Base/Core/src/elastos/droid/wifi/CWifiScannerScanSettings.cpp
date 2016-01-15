
#include "elastos/droid/wifi/CWifiScannerScanSettings.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CWifiScannerScanSettings, Object, IWifiScannerScanSettings, IParcelable)

CAR_OBJECT_IMPL(CWifiScannerScanSettings)

ECode CWifiScannerScanSettings::constructor()
{
    mBand = 0;
    mPeriodInMs = 0;
    mReportEvents = 0;
    mNumBssidsPerScan = 0;
    return NOERROR;
}

/** one of the WIFI_BAND values */
ECode CWifiScannerScanSettings::GetBand(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBand;
    return NOERROR;
}

ECode CWifiScannerScanSettings::SetBand(
    /* [in] */ Int32 band)
{
    mBand = band;
    return NOERROR;
}

/** list of channels; used when band is set to WIFI_BAND_UNSPECIFIED */
ECode CWifiScannerScanSettings::GetChannels(
    /* [out, callee] */ ArrayOf<IWifiScannerChannelSpec*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mChannels;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CWifiScannerScanSettings::SetChannels(
    /* [in] */ ArrayOf<IWifiScannerChannelSpec*>* channels)
{
    mChannels = channels;
    return NOERROR;
}

/** period of background scan; in millisecond, 0 => single shot scan */
ECode CWifiScannerScanSettings::GetPeriodInMs(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPeriodInMs;
    return NOERROR;
}

ECode CWifiScannerScanSettings::SetPeriodInMs(
    /* [in] */ Int32 periodInMs)
{
    mPeriodInMs = periodInMs;
    return NOERROR;
}

/** must have a valid REPORT_EVENT value */
ECode CWifiScannerScanSettings::GetReportEvents(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mReportEvents;
    return NOERROR;
}

ECode CWifiScannerScanSettings::SetReportEvents(
    /* [in] */ Int32 reportEvents)
{
    mReportEvents = reportEvents;
    return NOERROR;
}

/** defines number of bssids to cache from each scan */
ECode CWifiScannerScanSettings::GetNumBssidsPerScan(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNumBssidsPerScan;
    return NOERROR;
}

ECode CWifiScannerScanSettings::SetNumBssidsPerScan(
    /* [in] */ Int32 numBssidsPerScan)
{
    mNumBssidsPerScan = numBssidsPerScan;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerScanSettings::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerScanSettings::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    dest->WriteInt32(mBand);
    dest->WriteInt32(mPeriodInMs);
    dest->WriteInt32(mReportEvents);
    dest->WriteInt32(mNumBssidsPerScan);

    if (mChannels != NULL) {
        Int32 length = mChannels->GetLength();
        dest->WriteInt32(length);
        for (Int32 i = 0; i < length; i++) {
            Int32 frequency, dwellTimeMS;
            Boolean passive;
            (*mChannels)[i]->GetFrequency(&frequency);
            (*mChannels)[i]->GetDwellTimeMS(&dwellTimeMS);
            (*mChannels)[i]->GetPassive(&passive);
            dest->WriteInt32(frequency);
            dest->WriteInt32(dwellTimeMS);
            dest->WriteInt32(passive ? 1 : 0);
        }
    }
    else {
        dest->WriteInt32(0);
    }

    return NOERROR;
}

ECode CWifiScannerScanSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiScannerScanSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
