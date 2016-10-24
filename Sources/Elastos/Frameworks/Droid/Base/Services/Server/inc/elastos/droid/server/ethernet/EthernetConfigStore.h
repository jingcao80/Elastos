
#ifndef __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETCONFIGSTORE_H__
#define __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETCONFIGSTORE_H__

#include "elastos/droid/server/net/IpConfigStore.h"

using Elastos::Droid::Net::IIpConfiguration;
using Elastos::Droid::Server::Net::IpConfigStore;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Ethernet {

/**
 * This class provides an API to store and manage Ethernet network configuration.
 */
class EthernetConfigStore : public IpConfigStore
{
public:
    CARAPI constructor();

    CARAPI_(AutoPtr<IIpConfiguration>) ReadIpAndProxyConfigurations();

    CARAPI_(void) WriteIpAndProxyConfigurations(
        /* [in] */ IIpConfiguration* config);

private:
    static const String TAG;
    static const String sIpConfigFile;
};

} // namespace Ethernet
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETCONFIGSTORE_H__
