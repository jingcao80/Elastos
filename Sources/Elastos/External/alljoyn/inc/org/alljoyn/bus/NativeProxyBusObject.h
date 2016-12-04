
#ifndef __ORG_ALLJOYN_BUS_NATIVEPROXYBUSOBJECT_H__
#define __ORG_ALLJOYN_BUS_NATIVEPROXYBUSOBJECT_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/ProxyBusObject.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeBusAttachment;

class NativeProxyBusObject : public ajn::ProxyBusObject
{
public:
    NativeProxyBusObject(
        /* [in] */ IProxyBusObject* pbo,
        /* [in] */ NativeBusAttachment* nba,
        /* [in] */ const char* endpoint,
        /* [in] */ const char* path,
        /* [in] */ ajn::SessionId sessionId,
        /* [in] */ bool secure);

    ~NativeProxyBusObject();

    // QStatus RegisterPropertiesChangedListener(jstring jifaceName, jobjectArray jproperties, jobject jpropertiesChangedListener);

    // QStatus UnregisterPropertiesChangedListener(jstring jifaceName, jobject jpropertiesChangedListener);

private:
    NativeProxyBusObject(const NativeProxyBusObject& other);
    NativeProxyBusObject& operator =(const NativeProxyBusObject& other);

    // Mutex propertiesChangedListenersLock;
    // class Listener {
    //   public:
    //     const String ifaceName;
    //     jobject jlistener;
    //     Listener(const String& ifaceName, jobject jlistener) : ifaceName(ifaceName), jlistener(jlistener) { }
    //   private:
    //     Listener& operator =(const Listener& other);
    // };
    // list<Listener> propertiesChangedListeners;

public:
    NativeBusAttachment* mBusPtr;
    AutoPtr<IWeakReference> mPbo;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEPROXYBUSOBJECT_H__
