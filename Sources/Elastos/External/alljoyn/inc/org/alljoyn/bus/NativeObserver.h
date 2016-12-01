
#ifndef __ORG_ALLJOYN_BUS_NATIVEOBSERVER_H__
#define __ORG_ALLJOYN_BUS_NATIVEOBSERVER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/BusListener.h>
#include <alljoyn/TransportMask.h>
#include <alljoyn/CoreObserver.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeObserver : public ajn::CoreObserver
{
public:
    NativeObserver(
        /* [in] */ IBusAttachment* bus,
        /* [in] */ IObserver* listener,
        /* [in] */ const InterfaceSet& mandatory);

    ~NativeObserver();

    /* interface towards ObserverManager */
    virtual void ObjectDiscovered(
        /* [in] */ const ajn::ObjectId& oid,
        /* [in] */ const InterfaceSet& interfaces,
        /* [in] */ ajn::SessionId sessionid);

    virtual void ObjectLost(
        /* [in] */ const ajn::ObjectId& oid);

    /**
     * Enable all disabled listeners for this observer.
     *
     * Called from the ObserverManager work queue to make sure the
     * initial callbacks of triggerOnExisting listeners are called
     * from the local endpoint dispatcher threads.
     */
    virtual void EnablePendingListeners();

    void TriggerEnablePendingListeners();

    void Detach();

private:
    NativeObserver(const NativeObserver& other);
    NativeObserver& operator =(const NativeObserver& other);

    AutoPtr<IWeakReference> mObserver;
    AutoPtr<IBusAttachment> mBus;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEOBSERVER_H__
