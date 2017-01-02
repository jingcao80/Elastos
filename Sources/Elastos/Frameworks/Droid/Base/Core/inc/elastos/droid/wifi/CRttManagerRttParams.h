//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
