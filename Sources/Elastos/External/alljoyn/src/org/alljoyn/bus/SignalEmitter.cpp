
#include "org/alljoyn/bus/SignalEmitter.h"
#include "org/alljoyn/bus/InterfaceDescription.h"
#include "org/alljoyn/bus/MessageContext.h"
#include "org/alljoyn/bus/MsgArg.h"
#include "org/alljoyn/bus/NativeApi.h"
#include "org/alljoyn/bus/NativeBusObject.h"
#include "org/alljoyn/bus/NativeApi.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::Reflect::EIID_IInvocationHandler;
using Elastos::Core::AutoLock;
using Elastos::Core::IClassLoader;
using Elastos::Core::Reflect::CProxyFactory;
using Elastos::Core::Reflect::IProxyFactory;
using Elastos::Core::Reflect::EIID_IInvocationHandler;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("SignalEmitter");

CAR_INTERFACE_IMPL(SignalEmitter::Emitter, Object, IInvocationHandler)

SignalEmitter::Emitter::Emitter(
    /* [in] */ SignalEmitter* emitter)
    : mSignalEmitter(emitter)
{}

ECode SignalEmitter::Emitter::Invoke(
    /* [in] */ IInterface* proxy,
    /* [in] */ IMethodInfo* method,
    /* [in] */ IArgumentList* args)
{
    String name, signature;
    method->GetName(&name);
    method->GetSignature(&signature);

    IProxy* prx = IProxy::Probe(proxy);
    Int32 count;
    prx->GetInterfaceCount(&count);
    AutoPtr< ArrayOf<IInterfaceInfo*> > ifceInfos = ArrayOf<IInterfaceInfo*>::Alloc(count);
    prx->GetAllInterfaceInfos(ifceInfos);
    IInterfaceInfo* ifceInfo;
    for (Int32 i = 0; i < count; ++i) {
        ifceInfo = (*ifceInfos)[i];

        AutoPtr<IMethodInfo> methodInfo;
        ifceInfo->GetMethodInfo(name, signature, (IMethodInfo**)&methodInfo);
        if (methodInfo) {
            mSignalEmitter->Signal(mSignalEmitter->mSource,
               mSignalEmitter->mDestination,
               mSignalEmitter->mSessionId,
               InterfaceDescription::GetName(ifceInfo),
               methodInfo,
               args,
               mSignalEmitter->mTimeToLive,
               mSignalEmitter->mFlags,
               mSignalEmitter->mMsgContext);
        }
    }
    return NOERROR;
}

Int32 SignalEmitter::GLOBAL_BROADCAST = 0x20;
Int32 SignalEmitter::SESSIONLESS = 0x10;

CAR_INTERFACE_IMPL(SignalEmitter, Object, ISignalEmitter)

SignalEmitter::SignalEmitter()
    : mSessionId(0)
    , mTimeToLive(0)
    , mFlags(0)
{}

SignalEmitter::~SignalEmitter()
{
}

ECode SignalEmitter::constructor(
    /* [in] */ IBusObject* source,
    /* [in] */ const String& destination,
    /* [in] */ Int32 sessionId,
    /* [in] */ GlobalBroadcast globalBroadcast)
{
    mSource = source;
    mDestination = destination;
    mSessionId = sessionId;
    mFlags = (globalBroadcast == GlobalBroadcast_On)
            ? mFlags | GLOBAL_BROADCAST
            : mFlags & ~GLOBAL_BROADCAST;

    AutoPtr<IClassInfo> classInfo = Object::GetClassInfo(source);
    Int32 count;
    classInfo->GetInterfaceCount(&count);
    AutoPtr<ArrayOf<IInterfaceInfo*> > interfaces = ArrayOf<IInterfaceInfo*>::Alloc(count + 1);
    classInfo->GetAllInterfaceInfos(interfaces.Get());

    AutoPtr<IClassLoader> moduleClassLoader = GetModuleClassLoader();
    AutoPtr<IInterfaceInfo> propertiesInfo;
    moduleClassLoader->LoadInterface(String("Org.Alljoyn.Bus.Ifaces.IProperties"),
        (IInterfaceInfo**)&propertiesInfo);
    interfaces->Set(count, propertiesInfo);

    AutoPtr<IInvocationHandler> handler = new Emitter(this);
    AutoPtr<IProxyFactory> pf;
    CProxyFactory::AcquireSingleton((IProxyFactory**)&pf);
    AutoPtr<IInterface> obj;
    pf->NewProxyInstance(moduleClassLoader, interfaces, handler, (IInterface**)&obj);
    mProxy = IProxy::Probe(obj);

    mMsgContext = new MessageContext();
    return NOERROR;
}

ECode SignalEmitter::constructor(
    /* [in] */ IBusObject* source,
    /* [in] */ Int32 sessionId,
    /* [in] */ GlobalBroadcast globalBroadcast)
{
    return constructor(source, String(NULL), sessionId, globalBroadcast);
}

ECode SignalEmitter::constructor(
    /* [in] */ IBusObject* source,
    /* [in] */ GlobalBroadcast globalBroadcast)
{
    return constructor(source, String(NULL), IBusAttachment::SESSION_ID_ANY, globalBroadcast);
}

ECode SignalEmitter::constructor(
    /* [in] */ IBusObject* source)
{
    return constructor(source, String(NULL), IBusAttachment::SESSION_ID_ANY, GlobalBroadcast_Off);
}

ECode SignalEmitter::SetTimeToLive(
    /* [in] */ Int32 timeToLive)
{
    mTimeToLive = timeToLive;
    return NOERROR;
}

ECode SignalEmitter::SetCompressHeader(
    /* [in] */ Boolean compress)
{
    return NOERROR;
}

ECode SignalEmitter::SetSessionlessFlag(
    /* [in] */ Boolean sessionless)
{
    mFlags = sessionless ? mFlags | SESSIONLESS : mFlags & ~SESSIONLESS;
    return NOERROR;
}

ECode SignalEmitter::GetMessageContext(
    /* [out] */ IMessageContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)
    *ctx = mMsgContext;
    REFCOUNT_ADD(*ctx)
    return NOERROR;
}

ECode SignalEmitter::CancelSessionlessSignal(
    /* [in] */ Int32 serialNum)
{
    return CancelSessionlessSignal(mSource, serialNum);;
}

ECode SignalEmitter::GetInterface(
    /* [in] */ const InterfaceID& intf,
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    *obj = NULL;
    if (mProxy) {
        *obj = mProxy->Probe(intf);
        REFCOUNT_ADD(*obj)
    }
    return NOERROR;
}

ECode SignalEmitter::Signal(
    /* [in] */ IBusObject* busObj,
    /* [in] */ const String& destination,
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& ifaceName,
    /* [in] */ IMethodInfo* methodInfo,
    /* [in] */ IArgumentList* inArgs,
    /* [in] */ Int32 timeToLive,
    /* [in] */ Int32 flags,
    /* [in] */ IMessageContext* ctx)
{
    String signalName = InterfaceDescription::GetName(methodInfo);
    String inputSig = InterfaceDescription::GetInputSig(methodInfo);

    ajn::MsgArg args;
    if (FAILED(MsgArg::MarshalIn((Int64)&args, inputSig, methodInfo, inArgs))) {
        Logger::E(TAG, "Signal(): Marshal() error: ifaceName:%s, signalName:%s, inputSig:%s",
            ifaceName.string(), signalName.string(), inputSig.string());
        return E_FAIL;
    }

    /*
     * We have to find the C++ object that backs our Java Bus Object.  Since we are
     * provided a Java Bus Object reference here, there should be a corresponding
     * C++ backing object stored in the global map of such things.  Just because we
     * think it should be there doesn't mean that the client is playing games that
     * would mess us up.  For example, she could call signal on one thread and also
     * "simultaneously" call UnregisterBusObject on another, which could cause the
     * C++ backing object to be deleted out from under us.  To prevent such scenarios
     * we take the global bus object map lock during the entire signal processing
     * time.  This does mean that if the Signal function causes the execution of
     * something that needs to come back in and manage the global bus objects, we
     * will deadlock.
     */
    AutoLock lock(gBusObjectMapLock);
    NativeBusObject* busObject = GetBackingObject(busObj);
    if (!busObject) {
        Logger::E(TAG, "Signal(): GetBackingObject Exception");
        return ER_BUS_NO_SUCH_OBJECT;
    }

    QStatus status = ER_OK;

    ajn::BusAttachment& bus = const_cast<ajn::BusAttachment&>(busObject->GetBusAttachment());
    ajn::Message msg(bus);
    status = busObject->Signal(
        destination.string(), sessionId, ifaceName.string(), signalName.string(),
        args.v_struct.members, args.v_struct.numMembers, timeToLive, flags, msg);

    if (ER_OK == status) {
        /* Update MessageContext */
        MessageContext* msgContext = (MessageContext*)ctx;
        msgContext->mIsUnreliable = msg->IsUnreliable();
        msgContext->mObjectPath = msg->GetObjectPath();
        msgContext->mInterfaceName = msg->GetInterface();
        msgContext->mMemberName = msg->GetMemberName();
        msgContext->mDestination = msg->GetDestination();
        msgContext->mSender = msg->GetSender();
        msgContext->mSessionId = msg->GetSessionId();
        msgContext->mSerial = msg->GetCallSerial();
        msgContext->mSignature = msg->GetSignature();
        msgContext->mAuthMechanism = msg->GetAuthMechanism().c_str();
    }
    else {
        Logger::E(TAG, "%s Signal(): Signal Exception, ifaceName:%s, signalName:%s, inputSig:%s, status=%08x",
            TO_CSTR(busObj), ifaceName.string(), signalName.string(), inputSig.string(), status);
        return status;
    }

    return NOERROR;
}

ECode SignalEmitter::CancelSessionlessSignal(
    /* [in] */ IBusObject* busObj,
    /* [in] */ Int32 serialNum)
{
    AutoLock lock(gBusObjectMapLock);
    NativeBusObject* busObject = GetBackingObject(busObj);
    if (!busObject) {
        Logger::E(TAG, "CancelSessionlessSignal(): Exception");
        return E_FAIL;
    }

    QStatus status = busObject->CancelSessionlessMessage(serialNum);
    if (status != ER_OK) {
        Logger::E(TAG, "CancelSessionlessSignal(): Exception");
    }

    return status;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org