
#ifndef __ORG_ALLJOYN_BUS_NATIVESESSIONPORTLISTENER_H__
#define __ORG_ALLJOYN_BUS_NATIVESESSIONPORTLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/SessionPortListener.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeSessionPortListener : public ajn::SessionPortListener
{
public:
    NativeSessionPortListener(
        /* [in] */ ISessionPortListener* sessionPortListener);

    ~NativeSessionPortListener();

    bool AcceptSessionJoiner(
        /* [in] */ ajn::SessionPort sessionPort,
        /* [in] */ const char* joiner,
        /* [in] */ const ajn::SessionOpts& opts);

    void SessionJoined(
        /* [in] */ ajn::SessionPort sessionPort,
        /* [in] */ ajn::SessionId id,
        /* [in] */ const char* joiner);

private:
    NativeSessionPortListener(const NativeSessionPortListener& other);
    NativeSessionPortListener& operator =(const NativeSessionPortListener& other);

    AutoPtr<IWeakReference> mSessionPortListener;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVESESSIONPORTLISTENER_H__
