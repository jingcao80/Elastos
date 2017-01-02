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

#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICEREQUESTBASE_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICEREQUESTBASE_H__

#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

class WifiP2pServiceRequest
    : public Object
    , public IWifiP2pServiceRequest
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    WifiP2pServiceRequest();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 protocolType,
        /* [in] */ const String& query);

    CARAPI constructor(
        /* [in] */ Int32 serviceType,
        /* [in] */ Int32 length,
        /* [in] */ Int32 transId,
        /* [in] */ const String& query);

    CARAPI GetTransactionId(
        /* [out] */ Int32* transactionId);

    CARAPI SetTransactionId(
        /* [in] */ Int32 id);

    CARAPI GetSupplicantQuery(
        /* [out] */ String* supplicantQuery);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

private:
    CARAPI ValidateQuery(
        /* [in] */ const String& query);

public:
    /**
     * Service discovery protocol. It's defined in table63 in Wi-Fi Direct specification.
     */
    Int32 mProtocolType;

    /**
     * The length of the service request TLV.
     * The value is equal to 2 plus the number of octets in the
     * query data field.
     */
    Int32 mLength;

    /**
     * Service transaction ID.
     * This is a nonzero value used to match the service request/response TLVs.
     */
    Int32 mTransId;

    /**
     * The hex dump string of query data for the requested service information.
     *
     * e.g) DnsSd apple file sharing over tcp (dns name=_afpovertcp._tcp.local.)
     * 0b5f6166706f766572746370c00c000c01
     */
    String mQuery;
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICEREQUESTBASE_H__
