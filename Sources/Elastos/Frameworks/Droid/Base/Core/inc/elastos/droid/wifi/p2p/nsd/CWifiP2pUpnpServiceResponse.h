
#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PUPNPSERVICERESPONSE_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PUPNPSERVICERESPONSE_H__

#include "_Elastos_Droid_Wifi_P2p_Nsd_CWifiP2pUpnpServiceResponse.h"
#include "WifiP2pServiceResponse.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

/**
 * A class for a response of upnp service discovery.
 *
 * @hide
 */
CarClass(CWifiP2pUpnpServiceResponse), public WifiP2pServiceResponse
{
public:
    CAR_OBJECT_DECL()

    CWifiP2pUpnpServiceResponse();

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

    CARAPI GetVersion(
        /* [out] */ Int32* versionNumber);

    CARAPI GetUniqueServiceNames(
        /* [out, callee] */ ArrayOf<String>** uniqueServiceNames);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

public:
    static CARAPI NewInstance(
        /* [in] */ Int32 status,
        /* [in] */ Int32 transId,
        /* [in] */ IWifiP2pDevice* dev,
        /* [in] */ ArrayOf<Byte>* data,
        /* [out] */ IWifiP2pUpnpServiceResponse** instance);

private:
    /**
     * Parse UPnP service discovery response
     *
     * @return {@code true} if the operation succeeded
     */
    CARAPI Parse(
        /* [out] */ Boolean* result);

private:
    /**
     * UPnP version. should be {@link WifiP2pUpnpServiceInfo#VERSION_1_0}
     */
    Int32 mVersion;

    /**
     * The list of Unique Service Name.
     * e.g)
     *{"uuid:6859dede-8574-59ab-9332-123456789012",
     *"uuid:6859dede-8574-59ab-9332-123456789012::upnp:rootdevice"}
     */
    AutoPtr<ArrayOf<String> > mUniqueServiceNames;
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PUPNPSERVICERESPONSE_H__
