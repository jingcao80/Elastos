#include "org/alljoyn/bus/InterfaceDescription.h"
#include "org/alljoyn/bus/NativeApi.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include <elastos/utility/logging/Logger.h>
#include <alljoyn/AllJoynStd.h>
#include <alljoyn/DBusStd.h>
#include <alljoyn/InterfaceDescription.h>

using Elastos::Utility::Logging::Logger;

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
const String InterfaceDescription::TAG("InterfaceDescription");

CAR_INTERFACE_IMPL(InterfaceDescription, Object, IInterfaceDescription);

InterfaceDescription::InterfaceDescription()
    : mAnnounced(FALSE)
{}

ECode InterfaceDescription::Create(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ const String& name,
    /* [in] */ Int32 securePolicy,
    /* [in] */ Int32 numProps,
    /* [in] */ Int32 numMembers)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(busAttachment->mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "Create(): NULL bus pointer");
        return E_STATUS_FAIL;
    }

    ajn::InterfaceDescription* intf;
    QStatus status = busPtr->CreateInterface(name.string(), intf, static_cast<ajn::InterfaceSecurityPolicy>(securePolicy));
    if (ER_BUS_IFACE_ALREADY_EXISTS == status) {
        /*
         * We know that an interface exists with the same name, but it may differ in other parameters,
         * so confirm that other parameters are the same too before returning ER_OK.
         *
         * Note that we're not checking members or properties here, that check will be done later in
         * addMember and addProperty.
         */
        intf = (ajn::InterfaceDescription*)busPtr->GetInterface(name.string());
        assert(intf);
        if ((intf->GetSecurityPolicy() == static_cast<ajn::InterfaceSecurityPolicy>(securePolicy)) &&
            (intf->GetProperties() == (size_t)numProps) &&
            (intf->GetMembers() == (size_t)numMembers)) {
            status = ER_OK;
        }
        /*
         * When using org.freedesktop.DBus interfaces, we treat them as a special
         * case to remain backwards compatible. It cannot add the 'off' security
         * annotation. However, to work properly with object security, it must
         * still report its interface security as 'off'.
         */
        bool isDBusStandardIfac;
        if (name.string() == NULL) {     // passing NULL into strcmp is undefined behavior.
            isDBusStandardIfac = false;
        }
        else {
            isDBusStandardIfac = (strcmp(ajn::org::freedesktop::DBus::Introspectable::InterfaceName, name.string()) == 0) ||
                                 (strcmp(ajn::org::freedesktop::DBus::Peer::InterfaceName, name.string()) == 0) ||
                                 (strcmp(ajn::org::freedesktop::DBus::Properties::InterfaceName, name.string()) == 0) ||
                                 (strcmp(ajn::org::allseen::Introspectable::InterfaceName, name.string()) == 0);
        }
        if ((status != ER_OK) &&
            ((isDBusStandardIfac &&
              (intf->GetSecurityPolicy() == static_cast<ajn::InterfaceSecurityPolicy>(org_alljoyn_bus_InterfaceDescription_AJ_IFC_SECURITY_OFF))) ||
             (strcmp(ajn::org::alljoyn::Bus::Security::Application::InterfaceName, name.string()) == 0) ||
             (strcmp(ajn::org::alljoyn::Bus::Security::ClaimableApplication::InterfaceName, name.string()) == 0) ||
             (strcmp(ajn::org::alljoyn::Bus::Security::ManagedApplication::InterfaceName, name.string()) == 0) ||
             (strcmp(ajn::org::alljoyn::Bus::Application::InterfaceName, name.string()) == 0)
            )
            ) {
            status = ER_OK;
        }
    }
    if (ER_OK == status) {
        mHandle = reinterpret_cast<Int64>(intf);
    }

    return status;
}

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

    // now we need to add the DBus annotations for the interface;
    // this must be done *before* calling create
    // TODO:
    // BusAnnotations busAnnotations = busInterface.getAnnotation(BusAnnotations.class);
    // if (busAnnotations != null) {
    //     for (BusAnnotation annotation : busAnnotations.value()) {
    //         addAnnotation(annotation.name(), annotation.value());
    //     }
    // }

    ConfigureDescriptions(busAttachment, busInterface);

    // TODO:
    // BusInterface intf = busInterface.getAnnotation(BusInterface.class);
    // if (intf != null) {
    //     if(intf.announced().equals("true")) {
    //         announced = true;
    //     } else {
    //         announced = false;
    //     }
    // } else {
    //     announced = false;
    // }

    Activate();
    return E_STATUS_OK;
}

ECode InterfaceDescription::AddProperties(
    /* [in] */ IInterfaceInfo* busInterface)
{
    // TODO:
    // for (Property property : properties.values()) {
    //     int access = ((property.get != null) ? READ : 0) | ((property.set != null) ? WRITE : 0);
    //     int annotation = 0;

    //     for (Method method : busInterface.getMethods()) {
    //         BusProperty p = method.getAnnotation(BusProperty.class);
    //         if (p != null) {
    //             if (getName(method).equals(property.name)) {
    //                 annotation = p.annotation();
    //             }
    //         }
    //     }
    //     Status status = addProperty(property.name, property.signature, access, annotation);
    //     if (status != Status.OK) {
    //         return status;
    //     }

    //     if (annotation == BusProperty.ANNOTATE_EMIT_CHANGED_SIGNAL) {
    //         property.annotations.put("org.freedesktop.DBus.Property.EmitsChangedSignal", "true");
    //     } else if (annotation == BusProperty.ANNOTATE_EMIT_CHANGED_SIGNAL_INVALIDATES) {
    //         property.annotations.put("org.freedesktop.DBus.Property.EmitsChangedSignal", "invalidates");
    //     }

    //     // loop through the map of properties and add them via native code
    //     for(Entry<String, String> entry : property.annotations.entrySet()) {
    //         addPropertyAnnotation(property.name, entry.getKey(), entry.getValue());
    //     }
    // }
    return E_STATUS_OK;
}

ECode InterfaceDescription::GetMembers(
    /* [in] */ IInterfaceInfo* busInterface)
{
    Int32 count;
    busInterface->GetMethodCount(&count);
    AutoPtr< ArrayOf<IMethodInfo*> > methods = ArrayOf<IMethodInfo*>::Alloc(count);
    busInterface->GetAllMethodInfos(methods);
    for (Int32 i = 0; i < methods->GetLength(); i++) {
        IMethodInfo* method = (*methods)[i];
        mMembers.PushBack(method);
        // TODO:
        // if (method.getAnnotation(BusMethod.class) != null) {
        //     members.add(method);
        // } else if (method.getAnnotation(BusSignal.class) != null) {
        //     members.add(method);
        // }
    }
    return E_STATUS_OK;
}

ECode InterfaceDescription::AddMembers(
    /* [in] */ IInterfaceInfo* busInterface)
{
    // for (Method member : members) {
    //     int type = INVALID;
    //     int annotation = 0;
    //     String accessPerm = null;
    //     BusMethod m = member.getAnnotation(BusMethod.class);
    //     BusSignal s = member.getAnnotation(BusSignal.class);
    //     AccessPermission ap = member.getAnnotation(AccessPermission.class);

    //     if (m != null) {
    //         type = METHOD_CALL;
    //         annotation = m.annotation();
    //     } else if (s != null) {
    //         type = SIGNAL;
    //         annotation = s.annotation();
    //     }
    //     if (type != INVALID) {
    //         if(ap != null) {
    //             accessPerm = ap.value();
    //         }

    //         String memberName = getName(member);
    //         Status status = addMember(type, memberName, getInputSig(member),
    //                                   getOutSig(member), annotation, accessPerm);
    //         if (status != Status.OK) {
    //             return status;
    //         }

    //         // pull out the DBus annotations
    //         BusAnnotations dbusAnnotations = member.getAnnotation(BusAnnotations.class);
    //         if (dbusAnnotations != null)
    //         {
    //             for (BusAnnotation busAnnotation : dbusAnnotations.value()) {
    //                 addMemberAnnotation(memberName, busAnnotation.name(), busAnnotation.value());
    //             }
    //         }
    //     }
    // }
    // return Status.OK;
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
