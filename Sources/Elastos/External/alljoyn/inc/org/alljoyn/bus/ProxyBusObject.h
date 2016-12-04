
#ifndef __ORG_ALLJOYN_BUS_PROXYBUSOBJECT_H__
#define __ORG_ALLJOYN_BUS_PROXYBUSOBJECT_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::Object;
using Elastos::Core::Reflect::IInvocationHandler;
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
            IMethodInfo* mMethod;

            Boolean mIsMethod;
            // boolean isGet;

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
        /* [in] */ IInterfaceInfo* intfInfo,
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

private:
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
