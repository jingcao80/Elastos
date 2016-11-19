
#include "org/alljoyn/bus/SessionListener.h"
#include "org/alljoyn/bus/NativeSessionListener.h"

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

void SessionListener::Create()
{
    NativeSessionListener* nsl = new NativeSessionListener(this);
    assert(nsl != NULL);

    mHandle = reinterpret_cast<Int64>(nsl);
}

void SessionListener::Destroy()
{
    NativeSessionListener* nsl = reinterpret_cast<NativeSessionListener*>(mHandle);

    assert(nsl != NULL);
    delete nsl;

    mHandle = 0;
    return;
}

ECode SessionListener::SessionLost(
    /* [in] */ Int32 sessionId,
    /* [in] */ Int32 reason)
{
    return NOERROR;
}

ECode SessionListener::SessionMemberAdded(
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& uniqueName)
{
    return NOERROR;
}

ECode SessionListener::SessionMemberRemoved(
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& uniqueName)
{
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
