
#include "elastos/droid/wifi/CRttManagerParcelableRttParams.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CRttManagerParcelableRttParams, Object, IParcelable, IRttManagerParcelableRttParams)

CAR_OBJECT_IMPL(CRttManagerParcelableRttParams)

ECode CRttManagerParcelableRttParams::constructor(
    /* [in] */ ArrayOf<IRttManagerRttParams*>* params)
{
    mParams = params;
    return NOERROR;
}

ECode CRttManagerParcelableRttParams::GetParams(
    /* [out, callee] */ ArrayOf<IRttManagerRttParams*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mParams;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CRttManagerParcelableRttParams::SetParams(
    /* [in] */ ArrayOf<IRttManagerRttParams*>* params)
{
    mParams = params;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CRttManagerParcelableRttParams::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CRttManagerParcelableRttParams::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    if (mParams != NULL) {
        Int32 length = mParams->GetLength();
        dest->WriteInt32(length);

        for (Int32 i = 0; i < length; i++) {
            AutoPtr<IRttManagerRttParams> params = (*mParams)[i];
            Int32 deviceType;
            params->GetDeviceType(&deviceType);
            dest->WriteInt32(deviceType);
            Int32 requestType;
            params->GetRequestType(&requestType);
            dest->WriteInt32(requestType);
            String bssid;
            params->GetBssid(&bssid);
            dest->WriteString(bssid);
            Int32 frequency;
            params->GetFrequency(&frequency);
            dest->WriteInt32(frequency);
            Int32 channelWidth;
            params->GetChannelWidth(&channelWidth);
            dest->WriteInt32(channelWidth);
            Int32 num_samples;
            params->GetNum_samples(&num_samples);
            dest->WriteInt32(num_samples);
            Int32 num_retries;
            params->GetNum_retries(&num_retries);
            dest->WriteInt32(num_retries);
        }
    }
    else {
        dest->WriteInt32(0);
    }

    return NOERROR;
}

ECode CRttManagerParcelableRttParams::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CRttManagerParcelableRttParams::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
