
#include "elastos/droid/wifi/CRttManagerParcelableRttParams.h"
#include "elastos/droid/wifi/CRttManagerRttParams.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CRttManagerParcelableRttParams, Object, IParcelable, IRttManagerParcelableRttParams)

CAR_OBJECT_IMPL(CRttManagerParcelableRttParams)

ECode CRttManagerParcelableRttParams::constructor()
{
    return NOERROR;
}

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
    Int32 num;
    source->ReadInt32(&num);

    if (num == 0) {
        //return new ParcelableRttParams(null);
        mParams = NULL;
    }

    //RttParams params[] = new RttParams[num];
    mParams = ArrayOf<IRttManagerRttParams*>::Alloc(num);
    for (Int32 i = 0; i < num; i++) {
        AutoPtr<IRttManagerRttParams> rttParams;
        CRttManagerRttParams::New((IRttManagerRttParams**)&rttParams);
        Int32 deviceType;
        source->ReadInt32(&deviceType);
        rttParams->SetDeviceType(deviceType);
        Int32 requestType;
        source->ReadInt32(&requestType);
        rttParams->SetRequestType(requestType);
        String bssid;
        source->ReadString(&bssid);
        rttParams->SetBssid(bssid);
        Int32 frequency;
        source->ReadInt32(&frequency);
        rttParams->SetFrequency(frequency);
        Int32 channelWidth;
        source->ReadInt32(&channelWidth);
        rttParams->SetChannelWidth(channelWidth);
        Int32 num_samples;
        source->ReadInt32(&num_samples);
        rttParams->SetNum_samples(num_samples);
        Int32 num_retries;
        source->ReadInt32(&num_retries);
        rttParams->SetNum_retries(num_retries);

        mParams->Set(i, rttParams);

    }

    return NOERROR;
}

ECode CRttManagerParcelableRttParams::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    if (mParams != NULL) {
        Int32 length = mParams->GetLength();
        dest->WriteInt32(length);

        for(Int32 i = 0; i < length; ++i) {
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
    } else {
        dest->WriteInt32(0);
    }

    return NOERROR;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
