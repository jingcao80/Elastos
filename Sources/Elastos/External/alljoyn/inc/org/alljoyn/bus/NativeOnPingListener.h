
#ifndef __ORG_ALLJOYN_BUS_NATIVEONPINGLISTENER_H__
#define __ORG_ALLJOYN_BUS_NATIVEONPINGLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/BusAttachment.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeBusAttachment;

class NativeOnPingListener
    : public ajn::BusAttachment::PingAsyncCB
{
public:
    NativeOnPingListener(
        /* [in] */ IOnPingListener* OnPingListener);

    ~NativeOnPingListener();

    void Setup(NativeBusAttachment* jbap);
    void PingCB(QStatus status, void* context);

private:
    NativeOnPingListener(const NativeOnPingListener& other);
    NativeOnPingListener& operator =(const NativeOnPingListener& other);

private:
    NativeBusAttachment* mBusPtr;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEONPINGLISTENER_H__
