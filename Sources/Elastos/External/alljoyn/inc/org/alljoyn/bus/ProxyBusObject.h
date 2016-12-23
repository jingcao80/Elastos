
#ifndef __ORG_ALLJOYN_BUS_PROXYBUSOBJECT_H__
#define __ORG_ALLJOYN_BUS_PROXYBUSOBJECT_H__

#include "_Org.Alljoyn.Bus.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::Object;
using Elastos::Core::Reflect::IInvocationHandler;
using Elastos::Utility::IMap;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

namespace Org {
namespace Alljoyn {
namespace Bus {

class CBusAttachment;
class NativeBusAttachment;

class ProxyBusObject
    : public Object
    , public IProxyBusObject
{
private:
    class Handler
        : public Object
        , public IInvocationHandler
    {
    private:
        class Invocation
            : public Object
        {
        public:
            Invocation(
                /* [in] */ IMethodInfo* method);

        public:
            AutoPtr<IMethodInfo> mMethod;

            Boolean mIsMethod;
            Boolean mIsGet;

            String mInputSig;
            String mOutSig;

            String mInterfaceName;
            String mMethodName;

            // Type genericReturnType;
            // Class<?> returnType;
        };

    public:
        CAR_INTERFACE_DECL();

        Handler(
            /* [in] */ ProxyBusObject* host);

        // @Override
        CARAPI Invoke(
            /* [in] */ IInterface* proxy,
            /* [in] */ IMethodInfo* method,
            /* [in] */ IArgumentList* args);

    private:
        HashMap<String, AutoPtr< List< AutoPtr<Invocation> > > > mInvocationCache;
        ProxyBusObject* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Construct a ProxyBusObject.
     *
     * @param busAttachment  The connection the remote object is on.
     * @param busName        Well-known or unique bus name of remote object.
     * @param objPath        Object path of remote object.
     * @param sessionId      The session ID corresponding to the connection to the object.
     * @param busInterfaces  A list of BusInterfaces that this proxy should respond to.
     */
    CARAPI constructor(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ const String& busName,
        /* [in] */ const String& objPath,
        /* [in] */ Int32 sessionId,
        /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces);

    /**
     * Construct a ProxyBusObject.
     *
     * @param busAttachment  The connection the remote object is on.
     * @param busName        Well-known or unique bus name of remote object.
     * @param objPath        Object path of remote object.
     * @param sessionId      The session ID corresponding to the connection to the object.
     * @param busInterfaces  A list of BusInterfaces that this proxy should respond to.
     * @param secure         the security mode for the remote object
     */
    CARAPI constructor(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ const String& busName,
        /* [in] */ const String& objPath,
        /* [in] */ Int32 sessionId,
        /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces,
        /* [in] */ Boolean secure);

    /** Enable property caching for this proxy bus object. */
    CARAPI EnablePropertyCaching();

    /**
     * Release resources immediately.
     *
     * Normally, when all references are removed to a given object, the Java
     * garbage collector notices the fact that the object is no longer required
     * and will destory it.  This can happen at the garbage collector's leisure
     * an so any resources held by the object will not be released until "some
     * time later" after the object is no longer needed.
     *
     * Often, in test programs, we cycle through many BusAttachments in a very
     * short time, and if we rely on the garbage collector to clean up, we can
     * fairly quickly run out of scarce underlying resources -- especially file
     * descriptors.
     *
     * We provide an explicity release() method to allow test programs to release
     * the underlying resources immediately.  The garbage collector will still
     * call finalize, but the resrouces held by the underlying C++ objects will
     * go away immediately.
     *
     * It is a programming error to call another method on the ProxyBusObject
     * after the release() method has been called.
     */
    CARAPI ReleaseResources();

    /**
     * Gets the bus name.
     *
     * @return the bus name
     */
    CARAPI GetBusName(
        /* [out] */ String* busName);

    /**
     * Gets the object path.
     *
     * @return the object path
     */
    CARAPI GetObjPath(
        /* [out] */ String* objPath);

    /**
     * Gets a proxy to an interface of this remote bus object.
     *
     * @param <T> any class implementation of a interface annotated with AllJoyn interface annotations
     * @param intf one of the interfaces supplied when the proxy bus object was
     *             created
     * @return the proxy implementing the interface
     * @see BusAttachment#getProxyBusObject(String, String, int, Class[])
     */
    CARAPI GetInterface(
        /* [in] */ const InterfaceID& iid,
        /* [out] */ IInterface** intf);

    /**
     * Sets the reply timeout for method invocations on this remote bus object.
     * This affects all future method invocations of this bus object.  The
     * default reply timeout is 25 seconds.
     *
     * @param timeoutMsecs the timeout to wait for a reply
     */
    CARAPI SetReplyTimeout(
        /* [in] */ Int32 timeoutMsecs);

    /**
     * Tells the bus to start an application to handle a method invocation of
     * this bus object if needed.
     * This affects all future method invocations of this bus object.  The
     * default behavior is to not start an application.
     *
     * @param autoStart if {@code true} the bus should automatically start an
     *                  application to handle the method
     */
    CARAPI SetAutoStart(
        /* [in] */ Boolean autoStart);

    /**
     * Indicates if the remote object for this proxy bus object is secure.
     *
     * @return  true if the object is secure
     */
    CARAPI IsSecure(
        /* [out] */ Boolean* isSecure);

    /**
     * Get a property from an interface on the remote object.
     *
     * @param <T> any class implementation of a interface annotated with AllJoyn interface annotations
     * @param iface the interface that the property exists on
     * @param propertyName the name of the property
     * @return Variant containing the value of the property
     * @throws BusException if the named property doesn't exist
     */
    CARAPI GetProperty(
        /* [in] */ IInterfaceInfo* iface,
        /* [in] */ const String& propertyName,
        /* [out] */ IVariant** value);

    /**
     * Set a property on an interface on the remote object.
     *
     * @param <T> any class implementation of a interface annotated with AllJoyn interface annotations
     * @param iface the interface that the property exists on
     * @param propertyName the name of the property
     * @param value the value for the property
     * @throws BusException if the named property doesn't exist or cannot be set
     */
    CARAPI SetProperty(
        /* [in] */ IInterfaceInfo* iface,
        /* [in] */ const String& propertyName,
        /* [in] */ IVariant* value);

    /**
     * Get all properties from an interface on the remote object.
     *
     * @param <T> any class implementation of a interface annotated with AllJoyn interface annotations
     * @param iface the interface
     * @return a Map of name/value associations
     * @throws BusException if request cannot be honored
     */
    CARAPI GetAllProperties(
        /* [in] */ IInterfaceInfo* iface,
        /* [out] */ IMap** map);  //Map<String, Variant>

    /**
     * Function to register a handler for property change events.
     * Note that registering the same handler callback for the same
     * interface will overwrite the previous registration.  The same
     * handler callback may be registered for several different
     * interfaces simultaneously.
     *
     * @param iface             Remote object's interface on which the property is defined.
     * @param properties        The name of the properties to monitor (NULL for all).
     * @param listener          Reference to the object that will receive the callback.
     * @return
     *      - #ER_OK if the handler was registered successfully
     *      - #ER_BUS_NO_SUCH_INTERFACE if the specified interfaces does not exist on the remote object.
     *      - #ER_BUS_NO_SUCH_PROPERTY if the property does not exist
     */
    CARAPI RegisterPropertiesChangedListener(
        /* [in] */ const String& iface,
        /* [in] */ ArrayOf<String>* properties,
        /* [in] */ IPropertiesChangedListener* listener);

    /**
     * Function to unregister a handler for property change events.
     *
     * @param iface     Remote object's interface on which the property is defined.
     * @param listener  Reference to the object that used to receive the callback.
     * @return
     *      - #ER_OK if the handler was registered successfully
     *      - #ER_BUS_NO_SUCH_INTERFACE if the specified interfaces does not exist on the remote object.
     */
    CARAPI UnregisterPropertiesChangedListener(
        /* [in] */ const String& iface,
        /* [in] */ IPropertiesChangedListener* listener);

    CARAPI ToString(
        /* [out] */ String* str);

protected:
    /** Called by native code to lazily add an interface when a proxy method is invoked. */
    virtual CARAPI AddInterface(
        /* [in] */ const String& name);

private:
    /** Allocate native resources. */
    CARAPI_(void) Create(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ const String& busName,
        /* [in] */ const String& objPath,
        /* [in] */ Int32 sessionId,
        /* [in] */ Boolean secure);

    /** Release native resources. */
    CARAPI_(void) Destroy();

    /** Perform a method call on the remote object. */
    CARAPI MethodCall(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ const String& interfaceName,
        /* [in] */ const String& methodName,
        /* [in] */ const String& inputSig,
        // Type outType,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args,
        /* [in] */ Int32 replyTimeoutMsecs,
        /* [in] */ Int32 flags);

    CARAPI AddInterface(
        /* [in] */ NativeBusAttachment* busAttachment,
        /* [in] */ const String& name);

    // /** Get a property of the remote object. */
    CARAPI GetProperty(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ const String& interfaceName,
        /* [in] */ const String& propertyName,
        /* [in] */ const String& signature,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

    CARAPI GetProperty(
        /* [in] */ const String& interfaceName,
        /* [in] */ const String& propertyName,
        /* [out] */ IVariant** value);

    // private native Map<String, Variant> getAllProperties(BusAttachment busAttachment, Type outType, String interfaceName) throws BusException;

    /** Set a property of the remote object. */
    CARAPI SetProperty(
        /* [in] */ CBusAttachment* busAttachment,
        /* [in] */ const String& interfaceName,
        /* [in] */ const String& propertyName,
        /* [in] */ const String& signature,
        /* [in] */ IMethodInfo* method,
        /* [in] */ IArgumentList* args);

    /** Is the remote object for this proxy bus object secure. */
    Boolean IsProxyBusObjectSecure();

private:
    friend class NativeProxyBusObject;

    static const Int32 AUTO_START = 0x02;
    static const Int32 ENCRYPTED = 0x80;

    /** The bus the remote object is connected to. */
    CBusAttachment* mBus;

    /** Well-known or unique name of remote object. */
    String mBusName;

    /** Object path. */
    String mObjPath;

    /** Native proxy bus object handle. */
    Int64 mHandle;

    /** Remote interfaces proxy. */
    AutoPtr<IInterface> mProxy;

    Int32 mReplyTimeoutMsecs;

    Int32 mFlags;

    static const String TAG;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif //__ORG_ALLJOYN_BUS_PROXYBUSOBJECT_H__
