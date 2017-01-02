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

#ifndef __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PDEVICELIST_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PDEVICELIST_H__

#include "_Elastos_Droid_Wifi_P2p_CWifiP2pDeviceList.h"
#include "CWifiP2pDevice.h"
#include <elastos/core/Object.h>
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::ICollection;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

/**
 * A class representing a Wi-Fi P2p device list.
 *
 * Note that the operations are not thread safe.
 * {@see WifiP2pManager}
 */
CarClass(CWifiP2pDeviceList)
    , public Object
    , public IWifiP2pDeviceList
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IWifiP2pDeviceList* source);

    CARAPI constructor(
        /* [in] */ ArrayOf<IWifiP2pDevice*>* devices);

    CARAPI Clear(
        /* [out] */ Boolean* ret);

    CARAPI Update(
        /* [in] */ IWifiP2pDevice* device);

    CARAPI UpdateSupplicantDetails(
        /* [in] */ IWifiP2pDevice* device);

    CARAPI UpdateGroupCapability(
        /* [in] */ const String& deviceAddress,
        /* [in] */ Int32 groupCapab);

    CARAPI UpdateStatus(
        /* [in] */ const String& deviceAddress,
        /* [in] */ Int32 status);

    CARAPI Get(
        /* [in] */ const String& deviceAddress,
        /* [out] */ IWifiP2pDevice** device);

    CARAPI Remove(
        /* [in] */ IWifiP2pDevice* device,
        /* [out] */ Boolean* ret);

    CARAPI Remove(
        /* [in] */ const String& deviceAddress,
        /* [out] */ IWifiP2pDevice** result);

    CARAPI Remove(
        /* [in] */ IWifiP2pDeviceList* list,
        /* [out] */ Boolean* ret);

    CARAPI GetDeviceList(
        /* [out] */ ICollection** result);

    CARAPI GetDeviceList(
        /* [out, callee] */ ArrayOf<IWifiP2pDevice*>** list);

    CARAPI IsGroupOwner(
        /* [in] */ const String& deviceAddress,
        /* [out] */ Boolean* isGroupOwner);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    CARAPI ValidateDevice(
        /* [in] */ IWifiP2pDevice* device);

    CARAPI ValidateDeviceAddress(
        /* [in] */ const String& deviceAddress);

private:
    HashMap<String, AutoPtr<IWifiP2pDevice> > mDevices;
};

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PDEVICELIST_H__
