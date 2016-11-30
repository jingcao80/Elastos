
#ifndef __ORG_ALLJOYN_BUS_NATIVEONJOINSESSIONLISTENER_H__
#define __ORG_ALLJOYN_BUS_NATIVEONJOINSESSIONLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/BusAttachment.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeBusAttachment;

class NativeOnJoinSessionListener
    : public ajn::BusAttachment::JoinSessionAsyncCB
{
public:
    NativeOnJoinSessionListener(
        /* [in] */ IOnJoinSessionListener* OnJoinSessionListener);

    ~NativeOnJoinSessionListener();

    void Setup(NativeBusAttachment* jbap);
    void JoinSessionCB(
        QStatus status, ajn::SessionId sessionId, const ajn::SessionOpts& sessionOpts, void* context);

private:
    NativeOnJoinSessionListener(const NativeOnJoinSessionListener& other);
    NativeOnJoinSessionListener& operator =(const NativeOnJoinSessionListener& other);

private:
    NativeBusAttachment* mBusPtr;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEONJOINSESSIONLISTENER_H__
