
#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICERESPONSE_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICERESPONSE_H__

#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Wifi_P2p_Nsd_CWifiP2pDnsSdServiceResponse.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include "WifiP2pServiceResponse.h"

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IDataInput;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

/**
 * A class for a response of bonjour service discovery.
 *
 * @hide
 */
CarClass(CWifiP2pDnsSdServiceResponse), public WifiP2pServiceResponse
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int32 status,
        /* [in] */ Int32 transId,
        /* [in] */ IWifiP2pDevice* device,
        /* [in] */ ArrayOf<Byte>* data);

    CARAPI GetServiceType(
        /* [out] */ Int32* serviceType);

    CARAPI GetStatus(
        /* [out] */ Int32* status);

    CARAPI GetTransactionId(
        /* [out] */ Int32* transactionId);

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

    CARAPI GetDnsQueryName(
        /* [out] */ String* dnsQueryName);

    CARAPI GetDnsType(
        /* [out] */ Int32* dnsType);

    CARAPI GetVersion(
        /* [out] */ Int32* versionNumber);

    CARAPI GetInstanceName(
        /* [out] */ String* instanceName);

    CARAPI GetTxtRecord(
        /* [out] */ IMap** txtRecord);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    /**
     * Creates DnsSd service response.
     *  This is only called from WifiP2pServiceResponse
     *
     * @param status status code.
     * @param dev source device.
     * @param data DnsSd response data.
     * @return DnsSd service response data.
     * @hide
     */
    static CARAPI NewInstance(
        /* [in] */ Int32 status,
        /* [in] */ Int32 transId,
        /* [in] */ IWifiP2pDevice* dev,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IWifiP2pDnsSdServiceResponse** instance);
private:
    /**
     * Parse DnsSd service discovery response.
     *
     * @return {@code true} if the operation succeeded
     */
    CARAPI Parse(
        /* [out] */ Boolean* result);

    /**
     * Read dns name.
     *
     * @param dis data input stream.
     * @return dns name
     */
    CARAPI ReadDnsName(
        /* [in] */ IDataInput* idi,
        /* [out] */ String* dnsName);

    /**
     * Read TXT record data.
     *
     * @param dis
     * @return true if TXT data is valid
     */
    CARAPI ReadTxtData(
        /* [in] */ IDataInput* idi,
        /* [out] */ Boolean* isValid);

public:
    /**
     * Virtual memory packet.
     * see E.3 of the Wi-Fi Direct technical specification for the detail.<br>
     * The spec can be obtained from wi-fi.org
     * Key: pointer Value: domain name.<br>
     */
    class CStatic
    {
    public:
        CStatic()
        {
            sVmpack[0x0c] = String("_tcp.local.");
            sVmpack[0x11] = String("local.");
            sVmpack[0x1c] = String("_udp.local.");
        }
    public:
        HashMap<Int32, String> sVmpack;
    };

    static CStatic sStatic;

private:
    /**
     * DNS query name.
     * e.g)
     * for PTR
     * "_ipp._tcp.local."
     * for TXT
     * "MyPrinter._ipp._tcp.local."
     */
    String mDnsQueryName;

    /**
     * Service instance name.
     * e.g) "MyPrinter"
     * This field is only used when the dns type equals to
     * {@link WifiP2pDnsSdServiceInfo#DNS_TYPE_PTR}.
     */
    String mInstanceName;

    /**
     * DNS Type.
     * Should be {@link WifiP2pDnsSdServiceInfo#DNS_TYPE_PTR} or
     * {@link WifiP2pDnsSdServiceInfo#DNS_TYPE_TXT}.
     */
    Int32 mDnsType;

    /**
     * DnsSd version number.
     * Should be {@link WifiP2pDnsSdServiceInfo#VERSION_1}.
     */
    Int32 mVersion;

    /**
     * Txt record.
     * This field is only used when the dns type equals to
     * {@link WifiP2pDnsSdServiceInfo#DNS_TYPE_TXT}.
     */
    AutoPtr<IMap> mTxtRecord;
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICERESPONSE_H__
