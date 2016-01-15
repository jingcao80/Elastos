#ifndef __ELASTOS_NET_CINETADDRESSHELPER_H__
#define __ELASTOS_NET_CINETADDRESSHELPER_H__

#include "_Elastos_Net_CInetAddressHelper.h"
#include "InetAddress.h"
#include "elastos/core/Singleton.h"


namespace Elastos {
namespace Net {

CarClass(CInetAddressHelper)
    , public Singleton
    , public IInetAddressHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetUNSPECIFIED(
        /* [out] */ IInetAddress** addr);

    CARAPI GetAllByName(
        /* [in] */ const String& host,
        /* [out, callee] */ ArrayOf<IInetAddress*>** interAddress);

    CARAPI GetAllByNameOnNet(
        /* [in] */ const String& host,
        /* [in] */ Int32 netId,
        /* [out, callee] */ ArrayOf<IInetAddress*>** addresses);

    CARAPI GetByName(
        /* [in] */ const String& host,
        /* [out] */ IInetAddress** interAddr);

    CARAPI GetByNameOnNet(
        /* [in] */ const String& host,
        /* [in] */ Int32 netId,
        /* [out] */ IInetAddress** address);

    CARAPI GetLocalHost(
        /* [out] */ IInetAddress** interAddr);

    CARAPI ClearDnsCache();

    CARAPI IsNumeric(
        /* [in] */ const String& address,
        /* [out] */ Boolean* isNumeric);

    CARAPI ParseNumericAddress(
        /* [in] */ const String& numericAddress,
        /* [out] */ IInetAddress** interAddr);

    CARAPI GetLoopbackAddress(
        /* [out] */ IInetAddress** interAddr);

    CARAPI GetByAddress(
        /* [in] */ ArrayOf<Byte>* ipAddress,
        /* [out] */ IInetAddress** interAddr);

    CARAPI GetByAddress(
        /* [in] */ const String& hostName,
        /* [in] */ ArrayOf<Byte>* ipAddress,
        /* [out] */ IInetAddress** interAddr);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CINETADDRESSHELPER_H__
