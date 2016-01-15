
#ifndef __ELASTOS_DROID_NET_WIFI_CRTTMANAGERRTTPARAMS_H__
#define __ELASTOS_DROID_NET_WIFI_CRTTMANAGERRTTPARAMS_H__

#include "_Elastos_Droid_Wifi_CRttManagerRttParams.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {

CarClass(CRttManagerRttParams)
    , public Object
    , public IRttManagerRttParams
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI GetDeviceType(
        /* [out] */ Int32* result);

    CARAPI SetDeviceType(
        /* [in] */ Int32 deviceType);

    CARAPI GetRequestType(
        /* [out] */ Int32* result);

    CARAPI SetRequestType(
        /* [in] */ Int32 requestType);

    CARAPI GetBssid(
        /* [out] */ String* result);

    CARAPI SetBssid(
        /* [in] */ const String& bssid);

    CARAPI GetFrequency(
        /* [out] */ Int32* result);

    CARAPI SetFrequency(
        /* [in] */ Int32 frequency);

    CARAPI GetChannelWidth(
        /* [out] */ Int32* result);

    CARAPI SetChannelWidth(
        /* [in] */ Int32 channelWidth);

    CARAPI GetNum_samples(
        /* [out] */ Int32* result);

    CARAPI SetNum_samples(
        /* [in] */ Int32 num_samples);

    CARAPI GetNum_retries(
        /* [out] */ Int32* result);

    CARAPI SetNum_retries(
        /* [in] */ Int32 num_retries);

private:
    /** type of device being ranged; one of RTT_PEER_TYPE_AP or RTT_PEER_TYPE_STA */
    Int32 mDeviceType;

    /** type of RTT being sought; one of RTT_TYPE_ONE_SIDED
     *  RTT_TYPE_11_V or RTT_TYPE_11_MC or RTT_TYPE_UNSPECIFIED */
    Int32 mRequestType;

    /** mac address of the device being ranged */
    String mBssid;

    /** channel frequency that the device is on; optional */
    Int32 mFrequency;

    /** optional channel width. wider channels result in better accuracy,
     *  but they take longer time, and even get aborted may times; use
     *  RTT_CHANNEL_WIDTH_UNSPECIFIED if not specifying */
    Int32 mChannelWidth;

    /** number of samples to be taken */
    Int32 mNumSamples;

    /** number of retries if a sample fails */
    Int32 mNumRetries;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_NET_WIFI_CRTTMANAGERRTTPARAMS_H__
