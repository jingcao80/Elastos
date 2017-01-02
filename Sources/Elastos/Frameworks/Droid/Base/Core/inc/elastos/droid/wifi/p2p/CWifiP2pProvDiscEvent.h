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

#ifndef __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PPROVDISCEVENT_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PPROVDISCEVENT_H__

#include "_Elastos_Droid_Wifi_P2p_CWifiP2pProvDiscEvent.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pProvDiscEvent)
    , public Object
    , public IWifiP2pProvDiscEvent
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CWifiP2pProvDiscEvent();

    ~CWifiP2pProvDiscEvent();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& string);

    CARAPI GetEvent(
        /* [out] */ Int32* event);

    CARAPI SetEvent(
        /* [in] */ Int32 event);

    CARAPI GetDevice(
        /* [out] */ IWifiP2pDevice** device);

    CARAPI SetDevice(
        /* [in] */ IWifiP2pDevice* device);

    CARAPI GetPin(
        /* [out] */ String* pin);

    CARAPI SetPin(
        /* [in] */ const String& pin);

    CARAPI ToString(
        /* [out] */ String* string);

public:
    static const String TAG;

    /* One of PBC_REQ, PBC_RSP, ENTER_PIN or SHOW_PIN */
    Int32 mEvent;

    AutoPtr<IWifiP2pDevice> mDevice;

    /* Valid when event = SHOW_PIN */
    String mPin;
};

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PPROVDISCEVENT_H__
