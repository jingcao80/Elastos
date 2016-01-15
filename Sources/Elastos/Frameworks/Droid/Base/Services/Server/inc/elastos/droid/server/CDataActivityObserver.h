
#ifndef __ELASTOS_DROID_SERVER_CDATAACTIVITYOBSERVER_H__
#define __ELASTOS_DROID_SERVER_CDATAACTIVITYOBSERVER_H__

#include "_Elastos_Droid_Server_CDataActivityObserver.h"
#include "elastos/droid/server/net/BaseNetworkObserver.h"

using Elastos::Droid::Net::IIConnectivityManager;
using Elastos::Droid::Server::Net::BaseNetworkObserver;

namespace Elastos {
namespace Droid {
namespace Server {

class CConnectivityService;

CarClass(CDataActivityObserver), public BaseNetworkObserver
{
public:
    /**
     * Interface data activity status is changed.
     *
     * @param iface The interface.
     * @param active  True if the interface is actively transmitting data, false if it is idle.
     */
    CARAPI InterfaceClassDataActivityChanged(
        /* [in] */ const String& label,
        /* [in] */ Boolean active,
        /* [in] */ Int64 tsNanos);

    CARAPI constructor(
        /* [in] */ IIConnectivityManager* host);

private:
    CConnectivityService* mHost;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CDATAACTIVITYOBSERVER_H__
