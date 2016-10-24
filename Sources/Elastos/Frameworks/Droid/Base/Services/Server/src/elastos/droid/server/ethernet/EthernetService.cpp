
#include "elastos/droid/server/ethernet/EthernetService.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Ethernet {

//=====================================================================
//                             EthernetService
//=====================================================================
const String EthernetService::TAG("EthernetService");

EthernetService::EthernetService()
{
}

ECode EthernetService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    CEthernetServiceImpl::NewByFriend(context, (CEthernetServiceImpl**)&mImpl);
    return NOERROR;
}

ECode EthernetService::OnStart()
{
    Logger::I(TAG, "Registering service %s", IContext::ETHERNET_SERVICE.string());
    PublishBinderService(IContext::ETHERNET_SERVICE, mImpl);
    return NOERROR;
}

ECode EthernetService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
         mImpl->Start();
    }
    return NOERROR;
}

} // namespace Ethernet
} // namespace Server
} // namespace Droid
} // namespace Elastos
