
#ifndef __ORG_ALLJOYN_BUS_INTERFACEDESCRIPTION_H__
#define __ORG_ALLJOYN_BUS_INTERFACEDESCRIPTION_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

/**
 * InterfaceDescription represents a message bus interface.
 * This class is used internally by registered bus objects.
 */
class InterfaceDescription
    : public Object
    , public IInterfaceDescription
{
public:
    CAR_INTERFACE_DECL();

    CARAPI IsAnnounced(
        /* [out] */ Boolean* res);

private:
    /**
     * Called by the native code when registering bus objects to obtain the member
     * implementations.
     */
    AutoPtr<IMethodInfo> GetMember(
        /* [in] */ const String& name);

    /**
     * Called by the native code when registering bus objects to obtain the property
     * implementations.
     */
    AutoPtr<ArrayOf<IMethodInfo*> > GetProperty(
        /* [in] */ const String& name);

private:
    friend class NativeBusObject;

    /**
     * The native interface description handle.
     *
     * This differs from most of the native bindings code in that the underlying
     * object points directly to the corresponding AllJoyn object without a JNI
     * intermediary.  The underlying AlljJoyn object is actually managed there,
     * so we have a pointer but have no responsibility for deleting it.  You
     * will see a create() method, but that method gets the pointer, it doesn't
     * allocate an object.
     */
    Int64 mHandle;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_INTERFACEDESCRIPTION_H__
