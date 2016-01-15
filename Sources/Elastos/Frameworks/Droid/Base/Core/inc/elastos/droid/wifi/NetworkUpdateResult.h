
#ifndef __ELASTOS_DROID_NET_WIFI_NETWORKUPDATERESULT_H__
#define __ELASTOS_DROID_NET_WIFI_NETWORKUPDATERESULT_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

class NetworkUpdateResult : public ElRefBase
{
public:
    NetworkUpdateResult(
        /* [in] */ Int32 id);

    NetworkUpdateResult(
        /* [in] */ Boolean ip,
        /* [in] */ Boolean proxy);

    CARAPI_(void) SetNetworkId(
        /* [in] */ Int32 id);

    CARAPI_(Int32) GetNetworkId();

    CARAPI_(void) SetIpChanged(
        /* [in] */ Boolean ip);

    CARAPI_(Boolean) HasIpChanged();

    CARAPI_(void) SetProxyChanged(
        /* [in] */ Boolean proxy);

    CARAPI_(Boolean) HasProxyChanged();

    CARAPI_(Boolean) IsNewNetwork();

    CARAPI_(void) SetIsNewNetwork(
        /* [in] */ Boolean isNew);

private:
    Int32 mNetId;
    Boolean mIpChanged;
    Boolean mProxyChanged;
    Boolean mIsNewNetwork;
};

} // namespace Wifi
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_WIFI_NETWORKUPDATERESULT_H__
