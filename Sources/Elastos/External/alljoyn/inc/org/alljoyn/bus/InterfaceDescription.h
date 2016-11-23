
#ifndef __ORG_ALLJOYN_BUS_INTERFACEDESCRIPTION_H__
#define __ORG_ALLJOYN_BUS_INTERFACEDESCRIPTION_H__

#include "_Org.Alljoyn.Bus.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "org/alljoyn/bus/CBusAttachment.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

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
private:
    class Property : public Object
    {

    };

public:
    InterfaceDescription();

    CAR_INTERFACE_DECL();

    /**
     * Create the native interface description for the busInterface.
     *
     * @param busAttachment the connection the interface is on
     * @param busInterface the interface
     */
    CARAPI Create(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ IInterfaceInfo* busInterface);

    /**
     * Create the native interface descriptions needed by
     * busInterfaces.  The Java interface descriptions are returned
     * in the descs list.
     * @param busAttachment The connection the interfaces are on.
     * @param busInterfaces The interfaces.
     * @param descs The returned interface descriptions.
     */
    static CARAPI Create(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces,
        /* [in] */ IList* descs);

    /**
     * Get the DBus interface name.
     *
     * @param intf The interface.
     */
    static CARAPI_(String) GetName(
        /* [in] */ IInterfaceInfo* intf);

    CARAPI IsAnnounced(
        /* [out] */ Boolean* res);

private:
    /** Allocate native resources. */
    CARAPI Create(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ const String& name,
        /* [in] */ Int32 securePolicy,
        /* [in] */ Int32 numProps,
        /* [in] */ Int32 numMembers);

    /** Activate the interface on the bus. */
    CARAPI_(void) Activate();

    /**
     * Called by the native code when registering bus objects to obtain the member
     * implementations.
     */
    CARAPI_(AutoPtr<IMethodInfo>) GetMember(
        /* [in] */ const String& name);

    /**
     * Called by the native code when registering bus objects to obtain the property
     * implementations.
     */
    CARAPI_(AutoPtr<ArrayOf<IMethodInfo*> >) GetProperty(
        /* [in] */ const String& name);

    CARAPI_(void) ConfigureDescriptions(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ IInterfaceInfo* busInterface);

    CARAPI AddProperties(
        /* [in] */ IInterfaceInfo* busInterface);

    CARAPI GetMembers(
        /* [in] */ IInterfaceInfo* busInterface);

    CARAPI AddMembers(
        /* [in] */ IInterfaceInfo* busInterface);

private:
    friend class NativeBusObject;

    static const Int32 INVALID     = 0; /**< An invalid member type. */
    static const Int32 METHOD_CALL = 1; /**< A method call member. */
    static const Int32 SIGNAL      = 4; /**< A signal member. */

    static const Int32 READ        = 1;              /**< Read access type. */
    static const Int32 WRITE       = 2;              /**< Write access type. */
    static const Int32 RW          = READ | WRITE;   /**< Read-write access type. */

    static const Int32 AJ_IFC_SECURITY_INHERIT   = 0; /**< Inherit the security of the object that implements the interface */
    static const Int32 AJ_IFC_SECURITY_REQUIRED  = 1; /**< Security is required for an interface */
    static const Int32 AJ_IFC_SECURITY_OFF       = 2; /**< Security does not apply to this interface */

    static const String TAG;

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

    /** The members of this interface. */
    List< AutoPtr<IMethodInfo> > mMembers;

    /** The properties of this interface. */
    HashMap<String, AutoPtr<Property> > mProperties;

    Boolean mAnnounced;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_INTERFACEDESCRIPTION_H__
