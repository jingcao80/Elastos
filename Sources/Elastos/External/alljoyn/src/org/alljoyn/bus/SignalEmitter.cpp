
#include "org/alljoyn/bus/SignalEmitter.h"
#include "org/alljoyn/bus/MessageContext.h"
#include "org/alljoyn/bus/NativeApi.h"
#include "org/alljoyn/bus/NativeBusObject.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::Reflect::EIID_IInvocationHandler;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("SignalEmitter");

CAR_INTERFACE_IMPL(SignalEmitter::Emitter, Object, IInvocationHandler)

// public Object SignalEmitter::Emitter::Invoke(Object proxy, Method method, Object[] args) throws BusException {
//     for (Class<?> i : proxy.getClass().getInterfaces()) {
//         for (Method m : i.getMethods()) {
//             if (method.getName().equals(m.getName())) {
//                 signal(source,
//                        destination,
//                        sessionId,
//                        InterfaceDescription.getName(i),
//                        InterfaceDescription.getName(m),
//                        InterfaceDescription.getInputSig(m),
//                        args,
//                        timeToLive,
//                        flags,
//                        msgContext);
//             }
//         }
//     }
//     return null;
// }

Int32 SignalEmitter::GLOBAL_BROADCAST = 0x20;
Int32 SignalEmitter::SESSIONLESS = 0x10;

CAR_INTERFACE_IMPL(SignalEmitter, Object, ISignalEmitter)

SignalEmitter::SignalEmitter()
    : mSessionId(0)
    , mTimeToLive(0)
    , mFlags(0)
{}

SignalEmitter::~SignalEmitter()
{}

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

    assert(0 && "TODO");
    // Class<?>[] interfaces = source.getClass().getInterfaces();
    // Class<?>[] interfacesNew = Arrays.copyOf(interfaces, interfaces.length + 1);
    // interfacesNew[interfaces.length] = Properties.class;
    // proxy = Proxy.newProxyInstance(source.getClass().getClassLoader(), interfacesNew, new Emitter());
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
    /* [in] */ InterfaceID intf,
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
    /* [in] */ const String& signalName,
    /* [in] */ const String& inputSig,
    /* [in] */ ArrayOf<IInterface*>* inArgs,
    /* [in] */ Int32 timeToLive,
    /* [in] */ Int32 flags,
    /* [in] */ IMessageContext* ctx)
{
    assert(0 && "TODO");
    ajn::MsgArg args;
    // if (!MsgArg::Marshal(inputSig.string(), inArgs, &args)) {
    //     Logger::E(TAG, "SignalEmitter_signal(): Marshal() error");
    //     return;
    // }

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
        Logger::E(TAG, "Signal(): Exception");
        return E_FAIL;
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
        Logger::E(TAG, "Signal(): Exception");
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