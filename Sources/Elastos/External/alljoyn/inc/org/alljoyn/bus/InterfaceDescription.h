
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
    public:
        Property(
            /* [in] */ const String& name,
            /* [in] */ const String& signature,
            /* [in] */ HashMap<String, String>* annotations)
        {
            mName = name;
            mSignature = signature;
            mAnnotations = annotations;
        }

    public:
        String mName;
        AutoPtr<HashMap<String, String> > mAnnotations; // TreeMap
        String mSignature;
        AutoPtr<IMethodInfo> mGet;
        AutoPtr<IMethodInfo> mSet;
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

    /**
     * Get the DBus member or property name.
     *
     * @param method The method.
     */
    static CARAPI_(String) GetName(
        /* [in] */ IMethodInfo* method);

    /**
     * Get the DBus member input signature.
     *
     * @param method The method.
     */
    static CARAPI_(String) GetInputSig(
        /* [in] */ IMethodInfo* method);

    /**
     * Get the DBus member output signature.
     *
     * @param method the method
     */
    static CARAPI_(String) GetOutSig(
        /* [in] */ IMethodInfo* method);

    /**
     * Get the DBus property signature.
     *
     * @param method the method
     */
    static CARAPI_(String) GetPropertySig(
        /* [in] */ IMethodInfo* method);

private:
    /** Allocate native resources. */
    CARAPI Create(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ const String& name,
        /* [in] */ Int32 securePolicy,
        /* [in] */ Int32 numProps,
        /* [in] */ Int32 numMembers);

    /** Add a member to the native interface description. */
    CARAPI AddMember(
        /* [in] */ Int32 type,
        /* [in] */ const String& name,
        /* [in] */ const String& inputSig,
        /* [in] */ const String& outSig,
        /* [in] */ Int32 annotation,
        /* [in] */ const String& accessPerm);

    /** Add an annotation to the specified interface member */
    CARAPI AddMemberAnnotation(
        /* [in] */ const String& member,
        /* [in] */ const String& annotation,
        /* [in] */ const String& value);

    /** Add a property to the native interface description. */
    CARAPI AddProperty(
        /* [in] */ const String& name,
        /* [in] */ const String& signature,
        /* [in] */ Int32 access,
        /* [in] */ Int32 annotation);

    /** Add an annotation to the specified interface property */
    CARAPI AddPropertyAnnotation(
        /* [in] */ const String& property,
        /* [in] */ const String& annotation,
        /* [in] */ const String& value);

    /** Add an annotation to the interface */
    CARAPI AddAnnotation(
        /* [in] */ const String& annotation,
        /* [in] */ const String& value);

    CARAPI_(void) SetDescriptionLanguage(
        /* [in] */ const String& language);

    CARAPI_(void) SetDescription(
        /* [in] */ const String& description);

    CARAPI_(void) SetDescriptionTranslator(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ ITranslator* dt);

    CARAPI SetMemberDescription(
        /* [in] */ const String& member,
        /* [in] */ const String& description,
        /* [in] */ Boolean isSessionlessSignal);

    CARAPI SetPropertyDescription(
        /* [in] */ const String& propName,
        /* [in] */ const String& description);


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

    CARAPI ConfigureDescriptions(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ IInterfaceInfo* busInterface);

    CARAPI GetProperties(
        /* [in] */ IInterfaceInfo* busInterface);

    CARAPI AddProperties(
        /* [in] */ IInterfaceInfo* busInterface);

    CARAPI GetMembers(
        /* [in] */ IInterfaceInfo* busInterface);

    CARAPI AddMembers(
        /* [in] */ IInterfaceInfo* busInterface);

private:
    friend class NativeBusObject;
    friend class NativeBusAttachment;

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

    HashMap<String, AutoPtr<ITranslator> > mTranslatorCache;

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
