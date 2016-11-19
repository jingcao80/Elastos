
#include "org/alljoyn/bus/SessionListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(SessionListener, Object, ISessionListener)

SessionListener::SessionListener()
    : mHandle(0)
{
    Create();
}

SessionListener::~SessionListener()
{
    Destroy();
}

void SessionListener::Create();

void SessionListener::Destroy();

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
