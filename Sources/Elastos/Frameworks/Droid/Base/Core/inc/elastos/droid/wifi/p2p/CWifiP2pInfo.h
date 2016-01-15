
#ifndef __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PINFO_H__
#define __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PINFO_H__

#include <Elastos.CoreLibrary.Net.h>
#include "_Elastos_Droid_Wifi_P2p_CWifiP2pInfo.h"
#include <elastos/core/Object.h>

using Elastos::Net::IInetAddress;
using Elastos::Droid::Wifi::P2p::IWifiP2pInfo;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

CarClass(CWifiP2pInfo)
    , public Object
    , public IWifiP2pInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IWifiP2pInfo* source);

    CARAPI GetGroupFormed(
        /* [out] */ Boolean* formed);

    CARAPI SetGroupFormed(
        /* [in] */ Boolean formed);

    CARAPI GetIsGroupOwner(
        /* [out] */ Boolean* result);

    CARAPI IsGroupOwner(
        /* [out] */ Boolean* isGroupOwner);

    CARAPI SetIsGroupOwner(
        /* [in] */ Boolean isGroupOwner);

    CARAPI GetGroupOwnerAddress(
        /* [out] */ IInetAddress** address);

    CARAPI SetGroupOwnerAddress(
        /* [in] */ IInetAddress* address);

    CARAPI ToString(
        /* [out] */ String* string);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

private:
    /** Indicates if a p2p group has been successfully formed */
    Boolean mGroupFormed;

    /** Indicates if the current device is the group owner */
    Boolean mIsGroupOwner;

    /** Group owner address */
    AutoPtr<IInetAddress> mGroupOwnerAddress;
};

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_P2P_CWIFIP2PINFO_H__
