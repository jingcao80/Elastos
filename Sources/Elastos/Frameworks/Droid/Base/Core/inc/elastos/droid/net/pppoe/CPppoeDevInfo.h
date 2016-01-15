
#ifndef __ELASTOS_DROID_NET_PPPOE_CPPPOEDEVINFO_H__
#define __ELASTOS_DROID_NET_PPPOE_CPPPOEDEVINFO_H__

#include "_Elastos_Droid_Net_Pppoe_CPppoeDevInfo.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Net {
namespace Pppoe {

CarClass(CPppoeDevInfo)
{
public:
    CARAPI SetIfName(
        /* [in] */ const String& ifname);

    CARAPI GetIfName(
        /* [out] */ String* ifname);

    CARAPI SetIpAddress(
        /* [in] */ const String& ip);

    CARAPI GetIpAddress(
        /* [out] */ String* ip);

    CARAPI SetNetMask(
        /* [in] */ const String& ip);

    CARAPI GetNetMask(
        /* [out] */ String* netMask);

    CARAPI SetRouteAddr(
        /* [in] */ const String& route);

    CARAPI GetRouteAddr(
        /* [out] */ String* route);

    CARAPI SetDnsAddr(
        /* [in] */ const String& dns);

    CARAPI GetDnsAddr(
        /* [out] */ String* dns);

    CARAPI SetConnectMode(
        /* [in] */ const String& mode,
        /* [out] */ Boolean* result);

    CARAPI GetConnectMode(
        /* [out] */ String* mode);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI constructor();

private:
    String mDev_name;
    String mIpaddr;
    String mNetmask;
    String mRoute;
    String mDns;
    String mMode;
};

} // Pppoe
} // Net
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_NET_PPPOE_CPPPOEDEVINFO_H__
