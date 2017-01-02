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

#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICERESPONSEBASE_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICERESPONSEBASE_H__

#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Wifi::P2p::IWifiP2pDevice;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

#define MAX_BUF_SIZE (1024)

/**
 * The class for a response of service discovery.
 *
 * @hide
 */
class WifiP2pServiceResponse
    : public Object
    , public IWifiP2pServiceResponse
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    WifiP2pServiceResponse();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 serviceType,
        /* [in] */ Int32 status,
        /* [in] */ Int32 transId,
        /* [in] */ IWifiP2pDevice* device,
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI GetServiceType(
        /* [out] */ Int32* serviceType);

    CARAPI GetStatus(
        /* [out] */ Int32* status);

    CARAPI GetTransactionId(
        /* [out] */ Int32 * pTransactionId);

    CARAPI GetRawData(
        /* [out, callee] */ ArrayOf<Byte>** rawData);

    CARAPI GetSrcDevice(
        /* [out] */ IWifiP2pDevice** srcDevice);

    CARAPI SetSrcDevice(
        /* [in] */ IWifiP2pDevice* dev);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    static CARAPI_(String) StatusToString(
        /* [in] */ Int32 status);

protected:
    Boolean Equals(
        /* [in] */ const String& a,
        /* [in] */ const String& b);

    /**
     * Converts hex string to byte array.
     *
     * @param hex hex string. if invalid, return null.
     * @return binary data.
     */
    static ECode HexStr2Bin(
        /* [in] */ const String& hex,
        /* [out] */ ArrayOf<Byte>** array);

protected:
    /**
     * Service type. It's defined in table63 in Wi-Fi Direct specification.
     */
    Int32 mServiceType;

    /**
     * Status code of service discovery response.
     * It's defined in table65 in Wi-Fi Direct specification.
     * @see Status
     */
    Int32 mStatus;

    /**
     * Service transaction ID.
     * This is a nonzero value used to match the service request/response TLVs.
     */
    Int32 mTransId;

    /**
     * Source device.
     */
    AutoPtr<IWifiP2pDevice> mDevice;

    /**
     * Service discovery response data based on the requested on
     * the service protocol type. The protocol format depends on the service type.
     */
    AutoPtr<ArrayOf<Byte> > mData;
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICERESPONSEBASE_H__
