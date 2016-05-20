
#include "NetworkEventListener.h"

namespace Libcore {
namespace Net {
namespace Event {

CAR_INTERFACE_IMPL(NetworkEventListener, Object, INetworkEventListener)

ECode NetworkEventListener::OnNetworkConfigurationChanged()
{
    // no-op
    return NOERROR;
}

} // namespace Event
} // namespace Net
} // namespace Libcore
