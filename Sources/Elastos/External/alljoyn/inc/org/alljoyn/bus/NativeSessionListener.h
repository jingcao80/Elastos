
#ifndef __ORG_ALLJOYN_BUS_NATIVESESSIONLISTENER_H__
#define __ORG_ALLJOYN_BUS_NATIVESESSIONLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/SessionListener.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeSessionListener : public ajn::SessionListener
{
public:
    NativeSessionListener(
        /* [in] */ ISessionListener* sessionListener);

    ~NativeSessionListener();

    void SessionLost(
        /* [in] */ ajn::SessionId sessionId,
        /* [in] */ ajn::SessionListener::SessionLostReason reason);

    void SessionMemberAdded(
        /* [in] */ ajn::SessionId sessionId,
        /* [in] */ const char* uniqueName);

    void SessionMemberRemoved(
        /* [in] */ ajn::SessionId sessionId,
        /* [in] */ const char* uniqueName);

private:
    NativeSessionListener(const NativeSessionListener& other);
    NativeSessionListener& operator =(const NativeSessionListener& other);

    AutoPtr<IWeakReference> mSessionListener;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVESESSIONLISTENER_H__
