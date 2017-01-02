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

#ifndef __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PCONFIG_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PCONFIG_H__

#include "_Elastos_Droid_Wifi_P2p_CWifiP2pConfig.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Wifi::IWpsInfo;
using Elastos::Droid::Wifi::P2p::IWifiP2pConfig;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pConfig)
    , public Object
    , public IWifiP2pConfig
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiP2pConfig();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& supplicantEvent);

    CARAPI constructor(
        /* [in] */ IWifiP2pConfig* source);

    CARAPI GetDeviceAddress(
        /* [out] */ String* deviceAddress);

    CARAPI SetDeviceAddress(
        /* [in] */ const String& deviceAddress);

    CARAPI GetWps(
        /* [out] */ IWpsInfo** wpsInfo);

    CARAPI SetWps(
        /* [in] */ IWpsInfo* wpsInfo);

    CARAPI GetGroupOwnerIntent(
        /* [out] */ Int32* intent);

    CARAPI SetGroupOwnerIntent(
        /* [in] */ Int32 intent);

    CARAPI GetNetId(
        /* [out] */ Int32* netId);

    CARAPI SetNetId(
        /* [in] */ Int32 netId);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Invalidate();

private:
    /**
     * The device MAC address uniquely identifies a Wi-Fi p2p device
     */
    String mDeviceAddress;

    /**
     * Wi-Fi Protected Setup information
     */
    AutoPtr<IWpsInfo> mWps;

    /**
     * This is an integer value between 0 and 15 where 0 indicates the least
     * inclination to be a group owner and 15 indicates the highest inclination
     * to be a group owner.
     *
     * A value of -1 indicates the system can choose an appropriate value.
     */
    Int32 mGroupOwnerIntent;

    /** @hide */
    Int32 mNetId;
};

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PCONFIG_H__
