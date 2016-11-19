
#ifndef __ORG_ALLJOYN_BUS_NATIVEBUSLISTENER_H__
#define __ORG_ALLJOYN_BUS_NATIVEBUSLISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/BusListener.h>
#include <alljoyn/TransportMask.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeBusListener : public ajn::BusListener
{
public:
    NativeBusListener(
        /* [in] */ IBusListener* listener);

    ~NativeBusListener();

    void Setup(
        /* [in] */ IBusAttachment* busAttachment);

    void ListenerRegistered(
        /* [in] */ ajn::BusAttachment* bus);

    void ListenerUnregistered();

    void FoundAdvertisedName(
        /* [in] */ const char* name,
        /* [in] */ ajn::TransportMask transport,
        /* [in] */ const char* namePrefix);

    void LostAdvertisedName(
        /* [in] */ const char* name,
        /* [in] */ ajn::TransportMask transport,
        /* [in] */ const char* namePrefix);

    void NameOwnerChanged(
        /* [in] */ const char* busName,
        /* [in] */ const char* previousOwner,
        /* [in] */ const char* newOwner);

    void BusStopping();

    void BusDisconnected();

private:
    NativeBusListener(const NativeBusListener& other);
    NativeBusListener& operator =(const NativeBusListener& other);

    AutoPtr<IWeakReference> mBusListener;
    AutoPtr<IWeakReference> mBusAttachment;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEBUSLISTENER_H__
