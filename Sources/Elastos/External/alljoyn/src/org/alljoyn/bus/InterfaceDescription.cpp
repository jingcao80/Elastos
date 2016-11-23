#include "org/alljoyn/bus/InterfaceDescription.h"

namespace Org {
namespace Alljoyn {
namespace Bus {

const Int32 InterfaceDescription::INVALID;
const Int32 InterfaceDescription::METHOD_CALL;
const Int32 InterfaceDescription::SIGNAL;
const Int32 InterfaceDescription::READ;
const Int32 InterfaceDescription::WRITE;
const Int32 InterfaceDescription::RW;
const Int32 InterfaceDescription::AJ_IFC_SECURITY_INHERIT;
const Int32 InterfaceDescription::AJ_IFC_SECURITY_REQUIRED;
const Int32 InterfaceDescription::AJ_IFC_SECURITY_OFF;

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

ECode InterfaceDescription::Create(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ IInterfaceInfo* busInterface)
{
    // TODO:
    // Status status = getProperties(busInterface);
    // if (status != Status.OK) {
    //     return status;
    // }
    ECode ec = GetMembers(busInterface);
    if (ec != E_STATUS_OK) {
        return ec;
    }

    Int32 securePolicy = AJ_IFC_SECURITY_INHERIT;
    // TODO:
    // Secure secureAnnotation = busInterface.getAnnotation(Secure.class);
    // if (secureAnnotation != null) {
    //     if (secureAnnotation.value().equals("required")) {
    //         securePolicy = AJ_IFC_SECURITY_REQUIRED;
    //     } else if (secureAnnotation.value().equals("off")) {
    //         securePolicy = AJ_IFC_SECURITY_OFF;
    //     } else {
    //         /*
    //          * In C++ if an interface provides an unknown security annotation
    //          * it automatically defaults to the inherit for security. For
    //          * that reason the Java code will do the same.
    //          */
    //         securePolicy = AJ_IFC_SECURITY_INHERIT;
    //     }
    // }
    // else {
        securePolicy = AJ_IFC_SECURITY_INHERIT;
    // }
    ec = Create(busAttachment, GetName(busInterface), securePolicy,
            mProperties.GetSize(), mMembers.GetSize());
    if (ec != E_STATUS_OK) {
        return ec;
    }
    ec = AddProperties(busInterface);
    if (ec != E_STATUS_OK) {
        return ec;
    }
    ec = AddMembers(busInterface);
    if (ec != E_STATUS_OK) {
        return ec;
    }
}

/**
 * Create the native interface descriptions needed by
 * busInterfaces.  The Java interface descriptions are returned
 * in the descs list.
 * @param busAttachment The connection the interfaces are on.
 * @param busInterfaces The interfaces.
 * @param descs The returned interface descriptions.
 */
ECode InterfaceDescription::Create(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces,
    /* [in] */ IList* descs)
{
    for (Int32 i = 0; i < busInterfaces->GetLength(); i++) {
        IInterfaceInfo* intf = (*busInterfaces)[i];
        String ns, name;
        intf->GetNamespace(&ns);
        intf->GetName(&name);
        String fullName = ns + "." + name;
        if (fullName.Equals("org.freedesktop.DBus.Properties")) {
            /* The Properties interface is handled automatically by the underlying library. */
            continue;
        }
        // TODO:
        // if (intf.getAnnotation(BusInterface.class) != null) {
            AutoPtr<InterfaceDescription> desc = new InterfaceDescription();

            ECode ec = desc->Create(busAttachment, intf);
            if (ec != E_STATUS_OK) {
                return ec;
            }
            descs->Add((IInterfaceDescription*)desc.Get());
        // }
    }
    return E_STATUS_OK;
}


} // namespace Bus
} // namespace Alljoyn
} // namespace Org
