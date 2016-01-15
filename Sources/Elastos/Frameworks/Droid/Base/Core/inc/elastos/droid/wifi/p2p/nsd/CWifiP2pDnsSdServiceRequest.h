
#ifndef __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEREQUEST_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEREQUEST_H__

#include "_Elastos_Droid_Wifi_P2p_Nsd_CWifiP2pDnsSdServiceRequest.h"
#include "WifiP2pServiceRequest.h"

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CarClass(CWifiP2pDnsSdServiceRequest), public WifiP2pServiceRequest
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ const String& query);

    CARAPI constructor(
        /* [in] */ const String& dnsQuery,
        /* [in] */ Int32 dnsType,
        /* [in] */ Int32 versionNumber);

    CARAPI GetTransactionId(
        /* [out] */ Int32* transactionId);

    CARAPI SetTransactionId(
        /* [in] */ Int32 id);

    CARAPI GetSupplicantQuery(
        /* [out] */ String* supplicantQuery);

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
        /* [out] */ IWifiP2pDnsSdServiceRequest** instance);

    static CARAPI NewInstance(
        /* [in] */ const String& serviceType,
        /* [out] */ IWifiP2pDnsSdServiceRequest** instance);

    static CARAPI NewInstance(
        /* [in] */ const String& instanceName,
        /* [in] */ const String& serviceType,
        /* [out] */ IWifiP2pDnsSdServiceRequest** instance);
};

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_NSD_CWIFIP2PDNSSDSERVICEREQUEST_H__
