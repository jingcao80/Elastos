
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/NativeProxyBusObject.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

NativeProxyBusObject::NativeProxyBusObject(
    /* [in] */ IProxyBusObject* pbo,
    /* [in] */ NativeBusAttachment* nba,
    /* [in] */ const char* endpoint,
    /* [in] */ const char* path,
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ bool secure)
    : mBusPtr(nba)
{
    assert(mBusPtr != NULL);
    mBusPtr->IncRef();

    IWeakReferenceSource::Probe(pbo)->GetWeakReference((IWeakReference**)&mPbo);
}

NativeProxyBusObject::~NativeProxyBusObject()
{
    assert(mBusPtr != NULL);

    // propertiesChangedListenersLock.Lock();
    // for (list<Listener>::iterator i = propertiesChangedListeners.begin(); i != propertiesChangedListeners.end(); ++i) {
    //     JPropertiesChangedListener* listener = GetHandle<JPropertiesChangedListener*>(i->jlistener);
    //     if (env->ExceptionCheck()) {
    //         env->ExceptionClear();
    //     }
    //     if (listener) {
    //         ProxyBusObject::UnregisterPropertiesChangedListener(i->ifaceName.c_str(), *listener);
    //     }
    //     env->DeleteGlobalRef(i->jlistener);
    // }
    // propertiesChangedListenersLock.Unlock();
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
