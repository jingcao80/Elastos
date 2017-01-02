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

#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICEINFOBASE_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICEINFOBASE_H__

#include "Elastos.Droid.Wifi.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

class WifiP2pServiceInfo
    : public Object
    , public IWifiP2pServiceInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ArrayOf<String>* queryList);

    CARAPI GetSupplicantQueryList(
        /* [out, callee] */ ArrayOf<String>** list);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    /**
     * Converts byte array to hex string.
     *
     * @param data
     * @return hex string.
     * @hide
     */
    static CARAPI Bin2HexStr(
        /* [in] */ const ArrayOf<Byte>& data,
        /* [out] */ String* hexStr);

    static CARAPI Bin2HexStr(
        /* [in] */ const char * data,
        /* [in] */ Int32 length,
        /* [out] */ String* hexStr);

public:
    /**
     * the list of query string for wpa_supplicant
     *
     * e.g)
     * # IP Printing over TCP (PTR) (RDATA=MyPrinter._ipp._tcp.local.)
     * {"bonjour", "045f697070c00c000c01", "094d795072696e746572c027"
     *
     * # IP Printing over TCP (TXT) (RDATA=txtvers=1,pdl=application/postscript)
     * {"bonjour", "096d797072696e746572045f697070c00c001001",
     *  "09747874766572733d311a70646c3d6170706c69636174696f6e2f706f7374736372797074"}
     *
     * [UPnP]
     * # UPnP uuid
     * {"upnp", "10", "uuid:6859dede-8574-59ab-9332-123456789012"}
     *
     * # UPnP rootdevice
     * {"upnp", "10", "uuid:6859dede-8574-59ab-9332-123456789012::upnp:rootdevice"}
     *
     * # UPnP device
     * {"upnp", "10", "uuid:6859dede-8574-59ab-9332-123456789012::urn:schemas-upnp
     * -org:device:InternetGatewayDevice:1"}
     *
     *  # UPnP service
     * {"upnp", "10", "uuid:6859dede-8574-59ab-9322-123456789012::urn:schemas-upnp
     * -org:service:ContentDirectory:2"}
     */
    AutoPtr<ArrayOf<String> > mQueryList;
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PSERVICEINFOBASE_H__
