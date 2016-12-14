
#ifndef __ORG_ALLJOYN_BUS_NATIVE_PROPERTIES_CHANGED_LISTENER_H__
#define __ORG_ALLJOYN_BUS_NATIVE_PROPERTIES_CHANGED_LISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/ProxyBusObject.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativePropertiesChangedListener : public ajn::ProxyBusObject::PropertiesChangedListener
{
public:
    NativePropertiesChangedListener(
        /* [in] */ IWeakReference* listener,
        /* [in] */ IInterfaceInfo* changed,
        /* [in] */ IInterfaceInfo* invalidated);

    ~NativePropertiesChangedListener();

    void PropertiesChanged(
        /* [in] */ ajn::ProxyBusObject& obj,
        /* [in] */ const char* ifaceName,
        /* [in] */ const ajn::MsgArg& changed,
        /* [in] */ const ajn::MsgArg& invalidated,
        /* [in] */ void* context);

private:
    NativePropertiesChangedListener();
    NativePropertiesChangedListener(const NativePropertiesChangedListener& other);
    NativePropertiesChangedListener& operator =(const NativePropertiesChangedListener& other);

    AutoPtr<IWeakReference> mListener;

    AutoPtr<IInterfaceInfo> mChangedType;
    AutoPtr<IInterfaceInfo> mInvalidatedType;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVE_PROPERTIES_CHANGED_LISTENER_H__
