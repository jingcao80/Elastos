
#include "org/alljoyn/bus/ProxyBusObject.h"

using Elastos::Core::IClassLoader;
using Elastos::Core::Reflect::CProxyFactory;
using Elastos::Core::Reflect::IProxyFactory;
using Elastos::Core::Reflect::EIID_IInvocationHandler;

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
    // TODO:
    // if (method.getAnnotation(BusProperty.class) != null) {
    //     this.isGet = method.getName().startsWith("get");
    //     this.outSig = InterfaceDescription.getPropertySig(method);
    // } else {
    //     this.isMethod = true;
    //     this.outSig = InterfaceDescription.getOutSig(method);
    //     this.inputSig = InterfaceDescription.getInputSig(method);
    // }
    // this.interfaceName = InterfaceDescription.getName(method.getDeclaringClass());
    // this.methodName = InterfaceDescription.getName(method);
    // this.genericReturnType = method.getGenericReturnType();
    // this.returnType = method.getReturnType();
}

//=============================================================
// ProxyBusObject::Handler
//=============================================================
CAR_INTERFACE_IMPL(ProxyBusObject::Handler, Object, IInvocationHandler)

ECode ProxyBusObject::Handler::Invoke(
    /* [in] */ IInterface* proxy,
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    AutoPtr<Invocation> invocation;
    String methodName;
    method->GetName(&methodName);
    List< AutoPtr<Invocation> >* invocationList;
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


    return NOERROR;
}


//=============================================================
// ProxyBusObject
//=============================================================
CAR_INTERFACE_IMPL(ProxyBusObject, Object, IProxyBusObject)

ECode ProxyBusObject::constructor(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ const String& busName,
    /* [in] */ const String& objPath,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces)
{
    AutoPtr<IInterface> loader;
    (*busInterfaces)[0]->GetClassLoader((IInterface**)&loader);
    AutoPtr<IInvocationHandler> handler = new Handler();
    AutoPtr<IProxyFactory> pf;
    CProxyFactory::AcquireSingleton((IProxyFactory**)&pf);
    pf->NewProxyInstance(IClassLoader::Probe(loader), busInterfaces, handler, (IInterface**)&mProxy);
    return NOERROR;
}

ECode ProxyBusObject::constructor(
    /* [in] */ CBusAttachment* busAttachment,
    /* [in] */ const String& busName,
    /* [in] */ const String& objPath,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces,
    /* [in] */ Boolean secure)
{
    return NOERROR;
}

ECode ProxyBusObject::EnablePropertyCaching()
{
    return NOERROR;
}

ECode ProxyBusObject::ReleaseResources()
{
    return NOERROR;
}

ECode ProxyBusObject::GetBusName(
    /* [out] */ String* busName)
{
    return NOERROR;
}

ECode ProxyBusObject::GetObjPath(
    /* [out] */ String* objPath)
{
    return NOERROR;
}

ECode ProxyBusObject::GetInterface(
    /* [in] */ IInterfaceInfo* intfInfo,
    /* [out] */ IInterface** intf)
{
    VALIDATE_NOT_NULL(intf);
    InterfaceID iid;
    intfInfo->GetId(&iid);
    *intf = mProxy->Probe(iid);
    REFCOUNT_ADD(*intf);
    return NOERROR;
}

ECode ProxyBusObject::SetReplyTimeout(
    /* [in] */ Int32 timeoutMsecs)
{
    return NOERROR;
}

ECode ProxyBusObject::SetAutoStart(
    /* [in] */ Boolean autoStart)
{
    return NOERROR;
}

ECode ProxyBusObject::IsSecure(
    /* [out] */ Boolean* isSecure)
{
    return NOERROR;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
