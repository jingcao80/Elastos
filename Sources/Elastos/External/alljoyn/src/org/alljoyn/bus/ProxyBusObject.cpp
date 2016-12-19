
#include "org/alljoyn/bus/CBusAttachment.h"
#include "org/alljoyn/bus/InterfaceDescription.h"
#include "org/alljoyn/bus/MsgArg.h"
#include "org/alljoyn/bus/CVariant.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/NativeProxyBusObject.h"
#include "org/alljoyn/bus/ProxyBusObject.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Core::IClassLoader;
using Elastos::Core::Reflect::CProxyFactory;
using Elastos::Core::Reflect::IProxyFactory;
using Elastos::Core::Reflect::EIID_IInvocationHandler;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

//=============================================================
// ProxyBusObject::Handler::Invocation
//=============================================================
ProxyBusObject::Handler::Invocation::Invocation(
    /* [in] */ IMethodInfo* method)
    : mMethod(method)
{
    AutoPtr<IAnnotationInfo> ainfo;
    method->GetAnnotation(String("Org.Alljoyn.Bus.Annotation.BusProperty"), (IAnnotationInfo**)&ainfo);
    if (ainfo != NULL) {
        assert(0 && "TODO");
        // this.isGet = method.getName().startsWith("get");
        // this.outSig = InterfaceDescription.getPropertySig(method);
    }
    else {
        mIsMethod = TRUE;
        mOutSig = InterfaceDescription::GetOutSig(method);
        mInputSig = InterfaceDescription::GetInputSig(method);
    }
    AutoPtr<IInterfaceInfo> iinfo;
    method->GetDeclaringInterface((IInterfaceInfo**)&iinfo);
    mInterfaceName = InterfaceDescription::GetName(iinfo);
    mMethodName = InterfaceDescription::GetName(method);
    // TODO:
    // this.genericReturnType = method.getGenericReturnType();
    // this.returnType = method.getReturnType();
}

//=============================================================
// ProxyBusObject::Handler
//=============================================================
CAR_INTERFACE_IMPL(ProxyBusObject::Handler, Object, IInvocationHandler)

ProxyBusObject::Handler::Handler(
    /* [in] */ ProxyBusObject* host)
    : mHost(host)
{}

ECode ProxyBusObject::Handler::Invoke(
    /* [in] */ IInterface* proxy,
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    AutoPtr<Invocation> invocation;
    String methodName;
    method->GetName(&methodName);
    AutoPtr< List< AutoPtr<Invocation> > > invocationList;
    HashMap<String, AutoPtr< List< AutoPtr<Invocation> > > >::Iterator it =
            mInvocationCache.Find(methodName);
    if (it != mInvocationCache.End()) {
        invocationList = it->mSecond;
    }
    if (invocationList != NULL) {
        if (invocationList->GetSize() == 1) {
            invocation = invocationList->GetFront();
        }
        else {
            List< AutoPtr<Invocation> >::Iterator it;
            for (it = invocationList->Begin(); it != invocationList->End(); ++it) {
                if (method == (*it)->mMethod) {
                    invocation = (*it);
                    break;
                }
            }
            if (invocation == NULL) {
                invocation = new Invocation(method);
                invocationList->PushBack(invocation);
            }
        }
    }
    else {
        invocationList = new List< AutoPtr<Invocation> >();
        IProxy* prx = IProxy::Probe(proxy);
        Int32 N;
        prx->GetInterfaceCount(&N);
        AutoPtr< ArrayOf<IInterfaceInfo*> > infos = ArrayOf<IInterfaceInfo*>::Alloc(N);
        prx->GetAllInterfaceInfos(infos);
        for (Int32 i = 0; i < N; i++) {
            Int32 count;
            (*infos)[i]->GetMethodCount(&count);
            AutoPtr< ArrayOf<IMethodInfo*> > mInfos = ArrayOf<IMethodInfo*>::Alloc(count);
            (*infos)[i]->GetAllMethodInfos(mInfos);
            for (Int32 j = 0; j < count; j++) {
                IMethodInfo* m = (*mInfos)[j];
                String name;
                m->GetName(&name);
                if (methodName.Equals(name)) {
                    AutoPtr<Invocation> in = new Invocation(m);
                    invocationList->PushBack(in);
                    if (method == in->mMethod) {
                        invocation = in;
                    }
                }
            }
        }
        if (invocation == NULL) {
            // TODO:
            // try {
            //     Class<Object> objectClass = Object.class;
            //     if (method.equals(objectClass.getMethod("toString"))) {
            //         return proxyToString(proxy);
            //     }
            //     if (method.equals(objectClass.getMethod("equals",
            //             objectClass))) {
            //         return proxy == args[0];
            //     }
            //     if (method.equals(objectClass.getMethod("hashCode"))) {
            //         return System.identityHashCode(proxy);
            //     }
            // } catch (NoSuchMethodException e) {
            //     /*
            //      * If NoSuchMethodException is encountered the code will
            //      * drop through and throw a BusException.
            //      */
            // }
            // throw new BusException("No such method: " + method);
        }
        mInvocationCache[methodName] = invocationList;
    }

    ECode ec = NOERROR;
    if (invocation->mIsMethod) {
        ec = mHost->MethodCall(mHost->mBus,
                invocation->mInterfaceName,
                invocation->mMethodName,
                invocation->mInputSig,
                // invocation->mGenericReturnType,
                method,
                args,
                mHost->mReplyTimeoutMsecs,
                mHost->mFlags);
    }
    else {
        // TODO:
        // if (invocation.isGet) {
        //     Variant v = getProperty(bus,
        //                             invocation.interfaceName,
        //                             invocation.methodName);
        //     value = v.getObject(invocation.genericReturnType);
        // } else {
        //     setProperty(bus,
        //                 invocation.interfaceName,
        //                 invocation.methodName,
        //                 invocation.outSig,
        //                 args[0]);
        // }
    }

    //Unmarshal_out

    return ec;
}


//=============================================================
// ProxyBusObject
//=============================================================
const String ProxyBusObject::TAG("ProxyBusObject");

const Int32 ProxyBusObject::AUTO_START;
const Int32 ProxyBusObject::ENCRYPTED;

CAR_INTERFACE_IMPL(ProxyBusObject, Object, IProxyBusObject)

ECode ProxyBusObject::constructor(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ const String& busName,
    /* [in] */ const String& objPath,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces)
{
    return constructor(busAttachment, busName, objPath, sessionId, busInterfaces, FALSE);
}

ECode ProxyBusObject::constructor(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ const String& busName,
    /* [in] */ const String& objPath,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces,
    /* [in] */ Boolean secure)
{
    mBus = busAttachment;
    mBusName = busName;
    mObjPath = objPath;
    mFlags = 0;
    Create(busAttachment, busName, objPath, sessionId, secure);
    mReplyTimeoutMsecs = 25000;
    AutoPtr<IInterface> loader;
    (*busInterfaces)[0]->GetClassLoader((IInterface**)&loader);
    AutoPtr<IInvocationHandler> handler = new Handler(this);
    AutoPtr<IProxyFactory> pf;
    CProxyFactory::AcquireSingleton((IProxyFactory**)&pf);
    pf->NewProxyInstance(IClassLoader::Probe(loader), busInterfaces, handler, (IInterface**)&mProxy);
    // try {
    //     busConnectionLost =
    //         getClass().getDeclaredMethod("busConnectionLost", String.class);
    //     busConnectionLost.setAccessible(true);
    // } catch (NoSuchMethodException ex) {
    //     /* This will not happen */
    // }
    return NOERROR;
}

void ProxyBusObject::Create(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ const String& busName,
    /* [in] */ const String& objPath,
    /* [in] */ Int32 sessionId,
    /* [in] */ Boolean secure)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(busAttachment->mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "Create(): NULL bus pointer");
        return;
    }

    NativeProxyBusObject* npbo = new NativeProxyBusObject(this, busPtr,
            busName.string(), objPath.string(), sessionId, secure);
    assert(npbo != NULL);

    mHandle = reinterpret_cast<Int64>(npbo);
}

void ProxyBusObject::Destroy()
{
    NativeProxyBusObject* proxyBusObject =  reinterpret_cast<NativeProxyBusObject*>(mHandle);
    if (proxyBusObject == NULL) {
        Logger::D(TAG, "Destroy(): Already destroyed. Returning.");
        return;
    }

    NativeBusAttachment* busPtr = proxyBusObject->mBusPtr;
    delete proxyBusObject;
    busPtr->DecRef();
    mHandle = 0;
}

ECode ProxyBusObject::AddInterface(
    /* [in] */ const String& name)
{
    if (!name.IsNull()) {
        IProxy* prx = IProxy::Probe(mProxy);
        Int32 N;
        prx->GetInterfaceCount(&N);
        AutoPtr< ArrayOf<IInterfaceInfo*> > infos = ArrayOf<IInterfaceInfo*>::Alloc(N);
        prx->GetAllInterfaceInfos(infos);
        for (Int32 i = 0; i < N; i++) {
            IInterfaceInfo* intf = (*infos)[i];
            if (name.Equals(InterfaceDescription::GetName(intf))) {
                AutoPtr<InterfaceDescription> desc = new InterfaceDescription();
                return desc->Create(mBus, intf);
            }
        }
    }
    return E_STATUS_BUS_NO_SUCH_INTERFACE;
}

/*
 * if the interface security policy is Required return true,
 * if the interface security policy is off return false
 * otherwise return the object security.
 */
static inline Boolean SecurityApplies(
    /* [in] */ const NativeProxyBusObject* obj,
    /* [in] */ const ajn::InterfaceDescription* ifc)
{
    ajn::InterfaceSecurityPolicy ifcSec = ifc->GetSecurityPolicy();
    if (ifcSec == ajn::AJ_IFC_SECURITY_REQUIRED) {
        return TRUE;
    }
    else if (ifcSec == ajn::AJ_IFC_SECURITY_OFF) {
        return FALSE;
    }
    else {
        return obj->IsSecure();
    }
}

ECode ProxyBusObject::MethodCall(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ const String& interfaceName,
    /* [in] */ const String& methodName,
    /* [in] */ const String& inputSig,
    // Type outType,
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* carArgs,
    /* [in] */ Int32 replyTimeoutMsecs,
    /* [in] */ Int32 flags)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(busAttachment->mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "MethodCall(): NULL bus pointer");
        return E_STATUS_FAIL;
    }

    ECode ec = NOERROR;

    busPtr->mBaProxyLock.Lock();

    ajn::Message replyMsg(*busPtr);

    NativeProxyBusObject* proxyBusObj = reinterpret_cast<NativeProxyBusObject*>(mHandle);
    assert(proxyBusObj != NULL);

    const ajn::InterfaceDescription::Member* member = NULL;

    const ajn::InterfaceDescription* intf = proxyBusObj->GetInterface(interfaceName.string());
    if (!intf) {
        ec = AddInterface(busPtr, interfaceName);
        if (FAILED(ec)) {
            Logger::E(TAG, "MethodCall(): Exception, failed to GetInterface %s", interfaceName.string());
            busPtr->mBaProxyLock.Unlock();
            return ec;
        }
        intf = proxyBusObj->GetInterface(interfaceName.string());
        assert(intf);
    }

    member = intf->GetMember(methodName.string());
    if (!member) {
        Logger::E(TAG, "MethodCall(): Exception");
        busPtr->mBaProxyLock.Unlock();
        return E_STATUS_BUS_INTERFACE_NO_SUCH_MEMBER;
    }

    busPtr->mBaProxyLock.Unlock();

    ajn::MsgArg args;
    QStatus status;
    const ajn::MsgArg* replyArgs;
    size_t numReplyArgs;

    Logger::I(TAG, " >> MarshalIn: inputSig: '%s'", inputSig.string());
    ec = MsgArg::MarshalIn((Int64)&args, inputSig, method, carArgs);
    if (FAILED(ec)) {
        Logger::E(TAG, "MethodCall(): Marshal failure");
        return E_STATUS_FAIL;
    }
    Logger::I(TAG, " >> MarshalIn: result: '%s'", args.ToString().c_str());

    /*
     * If we call any method on the org.freedesktop.DBus.Properties interface
     *    - org.freedesktop.DBus.Properties.Get(ssv)
     *    - org.freedesktop.DBus.Properties.Set(ssv)
     *    - org.freedesktop.DBus.Properties.GetAll(sa{sv})
     * If the properties are part of an encrypted interface then the they must
     * also be encrypted.  The first parameter of Get, Set, and GetAll is the
     * interface name that the property belongs to.
     *    - this code reads the interface name from the Properties method call
     *    - tries to Get the InterfaceDescription from the proxyBusObj based on
     *      the interface name
     *    - Checks the InterfaceDescription to see if it has Security Annotation
     *      or object security
     *    - if security is set change the security flag to for the property
     *      method so the properties are encrypted.
     *    - if it is unable to get the InterfaceDescription it will check the
     *      security of the ProxyObject.
     *    - Failure to find a security indication will result the properties
     *      methods being used without encryption.
     */
    if (!interfaceName.IsNull()) {     //if interfaceName.c_str() is null strcmp is undefined behavior
        if (strcmp(interfaceName.string(), ajn::org::freedesktop::DBus::Properties::InterfaceName) == 0) {
            char* interface_name;
            /* the fist member of the struct is the interface name*/
            args.v_struct.members[0].Get("s", &interface_name);
            const ajn::InterfaceDescription* ifac_with_property = proxyBusObj->GetInterface(interface_name);
            /*
             * If the object or the property interface is secure method call
             * must be encrypted.
             */
            if (ifac_with_property == NULL) {
                if (proxyBusObj->IsSecure()) {
                    flags |= ajn::ALLJOYN_FLAG_ENCRYPTED;
                }
            }
            else if (SecurityApplies(proxyBusObj, ifac_with_property)) {
                flags |= ajn::ALLJOYN_FLAG_ENCRYPTED;
            }
        }
    }
    qcc::String val;
    if (member->GetAnnotation(ajn::org::freedesktop::DBus::AnnotateNoReply, val) && val == "true") {
        status = proxyBusObj->MethodCallAsync(*member, NULL, NULL, args.v_struct.members,
                                              args.v_struct.numMembers, NULL, replyTimeoutMsecs, flags);
        if (ER_OK != status) {
            Logger::E(TAG, "MethodCall(): Exception");
            ec = status;
        }
    }
    else {
        status = proxyBusObj->MethodCall(*member, args.v_struct.members, args.v_struct.numMembers,
                                         replyMsg, replyTimeoutMsecs, flags);
        if (ER_OK == status) {
            replyMsg->GetArgs(numReplyArgs, replyArgs);
            if (numReplyArgs > 1) {
                ajn::MsgArg structArg(ajn::ALLJOYN_STRUCT);
                structArg.v_struct.numMembers = numReplyArgs;
                structArg.v_struct.members = new ajn::MsgArg[numReplyArgs];
                for (size_t i = 0; i < numReplyArgs; ++i) {
                    structArg.v_struct.members[i] = replyArgs[i];
                }
                structArg.SetOwnershipFlags(ajn::MsgArg::OwnsArgs);
                MsgArg::UnmarshalOut((Int64)&structArg, method, carArgs);
            }
            else if (numReplyArgs > 0) {
                MsgArg::UnmarshalOut((Int64)&replyArgs[0], method, carArgs);
            }
        }
        else if (ER_BUS_REPLY_IS_ERROR_MESSAGE == status) {
            qcc::String errorMessage;
            const char* errorName = replyMsg->GetErrorName(&errorMessage);
            if (errorName) {
                if (!strcmp("org.alljoyn.bus.BusException", errorName)) {
                    Logger::E(TAG, "MethodCall(): %s", errorMessage.c_str());
                }
                else {
                    Logger::E(TAG, "MethodCall(): %s", errorMessage.c_str());
                }
            }
            else {
                Logger::E(TAG, "MethodCall(): Exception");
            }
            ec = E_STATUS_FAIL;
        }
        else {
            Logger::E(TAG, "MethodCall(): Exception");
            ec = E_STATUS_FAIL;
        }
    }

    return ec;
}

ECode ProxyBusObject::EnablePropertyCaching()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode ProxyBusObject::ReleaseResources()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode ProxyBusObject::GetBusName(
    /* [out] */ String* busName)
{
    VALIDATE_NOT_NULL(busName)
    *busName = mBusName;
    return NOERROR;
}

ECode ProxyBusObject::GetObjPath(
    /* [out] */ String* objPath)
{
    VALIDATE_NOT_NULL(objPath)
    *objPath = mObjPath;
    return NOERROR;
}

ECode ProxyBusObject::GetInterface(
    /* [in] */ const InterfaceID& iid,
    /* [out] */ IInterface** intf)
{
    VALIDATE_NOT_NULL(intf);
    *intf = mProxy->Probe(iid);
    REFCOUNT_ADD(*intf);
    return NOERROR;
}

ECode ProxyBusObject::SetReplyTimeout(
    /* [in] */ Int32 timeoutMsecs)
{
    mReplyTimeoutMsecs = timeoutMsecs;
    return NOERROR;
}

ECode ProxyBusObject::SetAutoStart(
    /* [in] */ Boolean autoStart)
{
    mFlags = autoStart ? mFlags | AUTO_START : mFlags & ~AUTO_START;
    return NOERROR;
}

ECode ProxyBusObject::IsSecure(
    /* [out] */ Boolean* isSecure)
{
    VALIDATE_NOT_NULL(isSecure)
    *isSecure = IsProxyBusObjectSecure();;
    return NOERROR;
}

Boolean ProxyBusObject::IsProxyBusObjectSecure()
{
    NativeProxyBusObject* proxyBusObj = reinterpret_cast<NativeProxyBusObject*>(mHandle);
    if (proxyBusObj == NULL) {
        Logger::E(TAG, "IsProxyBusObjectSecure(): NULL ProxyBusObject pointer");
        return FALSE;
    }
    return proxyBusObj->IsSecure();
}

ECode ProxyBusObject::RegisterPropertiesChangedListener(
    /* [in] */ const String& iface,
    /* [in] */ ArrayOf<String>* properties,
    /* [in] */ IPropertiesChangedListener* listener)
{
    NativeProxyBusObject* proxyBusObj = reinterpret_cast<NativeProxyBusObject*>(mHandle);
    if (proxyBusObj == NULL) {
        Logger::E(TAG, "RegisterPropertiesChangedListener(): NULL ProxyBusObject pointer");
        return FALSE;
    }
    return proxyBusObj->RegisterPropertiesChangedListener(iface, properties, listener);
}

ECode ProxyBusObject::UnregisterPropertiesChangedListener(
    /* [in] */ const String& iface,
    /* [in] */ IPropertiesChangedListener* listener)
{
    NativeProxyBusObject* proxyBusObj = reinterpret_cast<NativeProxyBusObject*>(mHandle);
    if (proxyBusObj == NULL) {
        Logger::E(TAG, "UnregisterPropertiesChangedListener(): NULL ProxyBusObject pointer");
        return FALSE;
    }
    return proxyBusObj->UnregisterPropertiesChangedListener(iface, listener);
}

ECode ProxyBusObject::AddInterface(
    /* [in] */ NativeBusAttachment* busPtr,
    /* [in] */ const String& name)
{
    NativeProxyBusObject* proxyBusObj = reinterpret_cast<NativeProxyBusObject*>(mHandle);
    assert(proxyBusObj != NULL);

    ECode ec = AddInterface(name);
    if (ec != (ECode)E_STATUS_OK) {
        Logger::E(TAG, "AddInterface(): Exception");
        return ec;
    }

    if (busPtr == NULL) {
        Logger::E(TAG, "AddInterface(): NULL bus pointer");
        return E_STATUS_FAIL;
    }

    const ajn::InterfaceDescription* intf = busPtr->GetInterface(name.string());
    assert(intf);

    ec = proxyBusObj->AddInterface(*intf);
    return ec;
}

ECode ProxyBusObject::GetProperty(
    /* [in] */ IInterfaceInfo* iface,
    /* [in] */ const String& propertyName,
    /* [out] */ IVariant** value)
{
    VALIDATE_NOT_NULL(value)
    *value = NULL;

    NativeProxyBusObject* proxyBusObj = reinterpret_cast<NativeProxyBusObject*>(mHandle);
    assert(proxyBusObj != NULL);

    return proxyBusObj->GetProperty(InterfaceDescription::GetName(iface), propertyName, value);
}

ECode ProxyBusObject::SetProperty(
    /* [in] */ IInterfaceInfo* iface,
    /* [in] */ const String& propertyName,
    /* [in] */ IVariant* value)
{
    NativeProxyBusObject* proxyBusObj = reinterpret_cast<NativeProxyBusObject*>(mHandle);
    assert(proxyBusObj != NULL);

    String signature;
    value->GetSignature(&signature);
    return proxyBusObj->SetProperty(
        InterfaceDescription::GetName(iface), propertyName,
        signature, ((CVariant*)value)->GetValue());
}

ECode ProxyBusObject::GetAllProperties(
    /* [in] */ IInterfaceInfo* iface,
    /* [out] */ IMap** map)  //Map<String, Variant>
{
    VALIDATE_NOT_NULL(map)
    *map = NULL;

    NativeProxyBusObject* proxyBusObj = reinterpret_cast<NativeProxyBusObject*>(mHandle);
    assert(proxyBusObj != NULL);

    assert(0 && "TODO");
    CarDataType returnType = CarDataType_Interface;
    //     returnType = org.alljoyn.bus.ifaces.Properties.class.getMethod("GetAll", String.class).getGenericReturnType();
    return proxyBusObj->GetAllProperties(InterfaceDescription::GetName(iface), returnType, map);

}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
