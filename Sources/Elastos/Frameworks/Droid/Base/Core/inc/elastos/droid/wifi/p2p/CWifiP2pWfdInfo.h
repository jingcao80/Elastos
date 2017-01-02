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

#ifndef __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PWFDINFO_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PWFDINFO_H__

#include "_Elastos_Droid_Wifi_P2p_CWifiP2pWfdInfo.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pWfdInfo)
    , public Object
    , public IWifiP2pWfdInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiP2pWfdInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 devInfo,
        /* [in] */ Int32 ctrlPort,
        /* [in] */ Int32 maxTput);

    CARAPI constructor(
        /* [in] */ IWifiP2pWfdInfo* source);

    CARAPI IsWfdEnabled(
        /* [out] */ Boolean* isWfdEnabled);

    CARAPI SetWfdEnabled(
        /* [in] */ Boolean enabled);

    CARAPI GetDeviceType(
        /* [out] */ Int32* deviceType);

    CARAPI SetDeviceType(
        /* [in] */ Int32 deviceType,
        /* [out] */ Boolean* result);

    CARAPI IsCoupledSinkSupportedAtSource(
        /* [out] */ Boolean* isCoupledSinkSupportedAtSource);

    CARAPI SetCoupledSinkSupportAtSource(
        /* [in] */ Boolean enabled);

    CARAPI IsCoupledSinkSupportedAtSink(
        /* [out] */ Boolean* isCoupledSinkSupportedAtSink);

    CARAPI SetCoupledSinkSupportAtSink(
        /* [in] */ Boolean enabled);

    CARAPI IsSessionAvailable(
        /* [out] */ Boolean* isSessionAvailable);

    CARAPI SetSessionAvailable(
        /* [in] */ Boolean enabled);

    CARAPI GetControlPort(
        /* [out] */ Int32* controlPort);

    CARAPI SetControlPort(
        /* [in] */ Int32 port);

    CARAPI SetMaxThroughput(
        /* [in] */ Int32 maxThroughput);

    CARAPI GetMaxThroughput(
        /* [out] */ Int32* maxThroughput);

    CARAPI GetDeviceInfoHex(
        /* [out] */ String* deviceInfoHex);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    static const String TAG;

    /* Device information bitmap */
    /** One of {@link #WFD_SOURCE}, {@link #PRIMARY_SINK}, {@link #SECONDARY_SINK}
     * or {@link #SOURCE_OR_PRIMARY_SINK}
     */
    static const Int32 DEVICE_TYPE                            = 0x3;
    static const Int32 COUPLED_SINK_SUPPORT_AT_SOURCE         = 0x4;
    static const Int32 COUPLED_SINK_SUPPORT_AT_SINK           = 0x8;
    static const Int32 SESSION_AVAILABLE                      = 0x30;
    static const Int32 SESSION_AVAILABLE_BIT1                 = 0x10;
    static const Int32 SESSION_AVAILABLE_BIT2                 = 0x20;

    Int32 mDeviceInfo;
    Int32 mCtrlPort;
    Int32 mMaxThroughput;
    Boolean mWfdEnabled;
};

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PWFDINFO_H__
