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

#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEINFO_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEINFO_H__

#include "_Elastos_Droid_Wifi_P2p_Nsd_CWifiP2pDnsSdServiceInfo.h"
#include "WifiP2pServiceInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Net::Nsd::IDnsSdTxtRecord;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IList;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

/**
 * A class for storing Bonjour service information that is advertised
 * over a Wi-Fi peer-to-peer setup.
 *
 * {@see android.net.wifi.p2p.WifiP2pManager#addLocalService}
 * {@see android.net.wifi.p2p.WifiP2pManager#removeLocalService}
 * {@see WifiP2pServiceInfo}
 * {@see WifiP2pUpnpServiceInfo}
 */
CarClass(CWifiP2pDnsSdServiceInfo), public WifiP2pServiceInfo
{
public:
    CAR_OBJECT_DECL()

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
    static CARAPI NewInstance(
        /* [in] */ const String& instanceName,
        /* [in] */ const String& serviceType,
        /* [in] */ IMap* txtMap, // Map<String, String>
        /* [out] */ IWifiP2pDnsSdServiceInfo** instance);

public:
    /**
     * Create wpa_supplicant service query for PTR record.
     *
     * @param instanceName instance name.<br>
     *  e.g) "MyPrinter"
     * @param serviceType service type.<br>
     *  e.g) "_ipp._tcp"
     * @return wpa_supplicant service query.
     */
    static CARAPI CreatePtrServiceQuery(
        /* [in] */ const String& instanceName,
        /* [in] */ const String& serviceType,
        /* [out] */ String* query);

    /**
     * Create wpa_supplicant service query for TXT record.
     *
     * @param instanceName instance name.<br>
     *  e.g) "MyPrinter"
     * @param serviceType service type.<br>
     *  e.g) "_ipp._tcp"
     * @param txtRecord TXT record.<br>
     * @return wpa_supplicant service query.
     */
    static CARAPI CreateTxtServiceQuery(
        /* [in] */ const String& instanceName,
        /* [in] */ const String& serviceType,
        /* [in] */ IDnsSdTxtRecord* txtRecord,
        /* [out] */ String* query);

    /**
     * Create bonjour service discovery request.
     *
     * @param dnsName dns name
     * @param dnsType dns type
     * @param version version number
     * @hide
     */
    static CARAPI CreateRequest(
        /* [in] */ const String& dnsName,
        /* [in] */ const Int32 dnsType,
        /* [in] */ const Int32 versionNumber,
        /* [out] */ String* request);

    /**
     * Compress DNS data.
     *
     * see E.3 of the Wi-Fi Direct technical specification for the detail.
     *
     * @param dnsName dns name
     * @return compressed dns name
     */
    static CARAPI CompressDnsName(
        /* [in] */ const String& dnsName,
        /* [out] */ String* name);

private:
    /**
     * virtual memory packet.
     * see E.3 of the Wi-Fi Direct technical specification for the detail.<br>
     * Key: domain name Value: pointer address.<br>
     */
    class CStatic
    {
    public:
        CStatic()
        {
            mVmPacket[String("_tcp.local.")] = String("c00c");
            mVmPacket[String("local.")] = String("c011");
            mVmPacket[String("_udp.local.")] = String("c01c");
        }
    public:
        HashMap<String, String> mVmPacket;
    };

    static CStatic sStatic;
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEINFO_H__
