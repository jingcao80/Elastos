
#ifndef __ELASTOS_DROID_SERVER_CONNECTIVITY_CVPNOBSERVER_H__
#define __ELASTOS_DROID_SERVER_CONNECTIVITY_CVPNOBSERVER_H__

#include "_Elastos_Droid_Server_Connectivity_CVpnObserver.h"
#include "elastos/droid/server/net/BaseNetworkObserver.h"

using Elastos::Droid::Server::Net::BaseNetworkObserver;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Connectivity {

class Vpn;

CarClass(CVpnObserver)
    , public BaseNetworkObserver
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Handle32 vpnHost);

    CARAPI InterfaceStatusChanged(
        /* [in] */ const String& iface,
        /* [in] */ Boolean up);

    CARAPI InterfaceRemoved(
        /* [in] */ const String& iface);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    Vpn* mHost;
};

} // Connectivity
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CONNECTIVITY_CVPNOBSERVER_H__
