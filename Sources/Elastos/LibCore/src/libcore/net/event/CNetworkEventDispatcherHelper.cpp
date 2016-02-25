#include "libcore/net/event/NetworkEventDispatcher.h"
#include "libcore/net/event/CNetworkEventDispatcherHelper.h"

namespace Libcore {
namespace Net {
namespace Event {

CAR_INTERFACE_IMPL(CNetworkEventDispatcherHelper, Singleton, INetworkEventDispatcherHelper)

CAR_SINGLETON_IMPL(CNetworkEventDispatcherHelper)

ECode CNetworkEventDispatcherHelper::GetInstance(
    /* [out] */ INetworkEventDispatcher** result)
{
    return NetworkEventDispatcher::GetInstance(result);
}

} // namespace Event
} // namespace Net
} // namespace Libcore
