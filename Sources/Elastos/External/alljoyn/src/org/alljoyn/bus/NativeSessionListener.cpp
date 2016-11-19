
#include "org/alljoyn/bus/NativeSessionListener.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

NativeSessionListener::NativeSessionListener(
    /* [in] */ ISessionListener* sessionListener);

NativeSessionListener::~NativeSessionListener();

void NativeSessionListener::SessionLost(
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ ajn::SessionListener::SessionLostReason reason);

void NativeSessionListener::SessionMemberAdded(
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ const char* uniqueName);

void NativeSessionListener::SessionMemberRemoved(
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ const char* uniqueName);

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
