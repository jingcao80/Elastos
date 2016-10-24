#ifndef __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETSERVICE_H__
#define __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETSERVICE_H__

#include "elastos/droid/server/SystemService.h"
#include "elastos/droid/server/ethernet/CEthernetServiceImpl.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Server::SystemService;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Ethernet {

class EthernetService
    : public SystemService
{
public:
    EthernetService();

    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnStart();

    // @Override
    CARAPI OnBootPhase(
        /* [in] */ Int32 phase);

public:
    AutoPtr<CEthernetServiceImpl> mImpl;

private:
    static const String TAG;
};

} // namespace Ethernet
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_ETHERNET_ETHERNETSERVICE_H__

