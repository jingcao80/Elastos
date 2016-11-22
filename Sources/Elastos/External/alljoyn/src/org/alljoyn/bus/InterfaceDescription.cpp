#include "org/alljoyn/bus/InterfaceDescription.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

CAR_INTERFACE_IMPL(InterfaceDescription, Object, IInterfaceDescription);

ECode InterfaceDescription::IsAnnounced(
    /* [out] */ Boolean* res)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

AutoPtr<IMethodInfo> InterfaceDescription::GetMember(
    /* [in] */ const String& name)
{
    assert(0);
    // for (Method m : members) {
    //     if (InterfaceDescription.getName(m).equals(name)) {
    //         return m;
    //     }
    // }
    return NULL;
}

/**
 * Called by the native code when registering bus objects to obtain the property
 * implementations.
 */
AutoPtr<ArrayOf<IMethodInfo*> > InterfaceDescription::GetProperty(
    /* [in] */ const String& name)
{
    assert(0);
    // for (Property p : properties.values()) {
    //     if (p.name.equals(name)) {
    //         return new Method[] { p.get, p.set };
    //     }
    // }
    return NULL;
}


} // namespace Bus
} // namespace Alljoyn
} // namespace Org
