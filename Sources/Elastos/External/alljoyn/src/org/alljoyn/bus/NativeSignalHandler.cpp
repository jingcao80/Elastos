
#include "org/alljoyn/bus/NativeSignalHandler.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Alljoyn {
namespace Bus {

//=============================================================
// NativeSignalHandler
//=============================================================
NativeSignalHandler::NativeSignalHandler(
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* method)
    : mMethod(method)
    , mMember(NULL)
{
    IWeakReferenceSource::Probe(obj)->GetWeakReference((IWeakReference**)&mSignalHandler);

}

NativeSignalHandler::~NativeSignalHandler()
{
}

Boolean NativeSignalHandler::IsSameObject(
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* method)
{
    AutoPtr<IInterface> handler;
    mSignalHandler->Resolve(EIID_IInterface, (IInterface**)&handler);
    if (!handler) {
        return FALSE;
    }

    return Object::Equals(obj, handler);
}

QStatus NativeSignalHandler::Register(
    /* [in] */ ajn::BusAttachment& bus,
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ const char* ancillary)
{
    if (bus.IsConnected() == false) {
        return ER_BUS_NOT_CONNECTED;
    }
    const ajn::InterfaceDescription* intf = bus.GetInterface(ifaceName);
    if (!intf) {
        return ER_BUS_NO_SUCH_INTERFACE;
    }
    mMember = intf->GetMember(signalName);
    if (!mMember) {
        return ER_BUS_INTERFACE_NO_SUCH_MEMBER;
    }
    mAncillaryData = ancillary;
    return ER_OK;
}

void NativeSignalHandler::SignalHandler(
    /* [in] */ const ajn::InterfaceDescription::Member* member,
    /* [in] */ const char* sourcePath,
    /* [in] */ ajn::Message& msg)
{
    assert(0 && "TODO");
    // /*
    //  * JScopedEnv will automagically attach the JVM to the current native
    //  * thread.
    //  */
    // JScopedEnv env;

    // MessageContext context(msg);

    // JLocalRef<jobjectArray> jargs;
    // QStatus status = Unmarshal(msg, jmethod, jargs);
    // if (ER_OK != status) {
    //     return;
    // }

    // JLocalRef<jclass> clazz = env->GetObjectClass(jmethod);
    // jmethodID mid = env->GetMethodID(clazz, "invoke",
    //                                  "(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
    // if (!mid) {
    //     return;
    // }

    // /*
    //  * The weak global reference jsignalHandler cannot be directly used.  We
    //  * have to get a "hard" reference to it and then use that.  If you try to
    //  * use a weak reference directly you will crash and burn.
    //  */
    // jobject jo = env->NewLocalRef(jsignalHandler);
    // if (!jo) {
    //     return;
    // }
    // CallObjectMethod(env, jmethod, mid, jo, (jobjectArray)jargs);
}

//=============================================================
// NativeSignalHandlerWithSrc
//=============================================================
QStatus NativeSignalHandlerWithSrc::Register(
    /* [in] */ ajn::BusAttachment& bus,
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ const char* ancillary)
{
    QStatus status = NativeSignalHandler::Register(bus, ifaceName, signalName, ancillary);
    if (status != ER_OK) {
        return status;
    }

    return bus.RegisterSignalHandler(this,
        static_cast<ajn::MessageReceiver::SignalHandler>(&NativeSignalHandler::SignalHandler),
        mMember,
        mAncillaryData.c_str());
}

void NativeSignalHandlerWithSrc::Unregister(
    /* [in] */ ajn::BusAttachment& bus)
{
    if (bus.IsConnected() == false) {
        return;
    }

    if (mMember) {
        bus.UnregisterSignalHandler(this,
            static_cast<ajn::MessageReceiver::SignalHandler>(&NativeSignalHandler::SignalHandler),
            mMember,
            mAncillaryData.c_str());
    }
}


//=============================================================
// NativeSignalHandlerWithRule
//=============================================================
QStatus NativeSignalHandlerWithRule::Register(
    /* [in] */ ajn::BusAttachment& bus,
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ const char* ancillary)
{
    QStatus status = NativeSignalHandler::Register(bus, ifaceName, signalName, ancillary);
    if (status != ER_OK) {
        return status;
    }

    return bus.RegisterSignalHandlerWithRule(this,
        static_cast<ajn::MessageReceiver::SignalHandler>(&NativeSignalHandler::SignalHandler),
        mMember,
        mAncillaryData.c_str());
}

void NativeSignalHandlerWithRule::Unregister(
    /* [in] */ ajn::BusAttachment& bus)
{
    if (bus.IsConnected() == false) {
        return;
    }

    if (mMember) {
        bus.UnregisterSignalHandlerWithRule(this,
            static_cast<ajn::MessageReceiver::SignalHandler>(&NativeSignalHandler::SignalHandler),
            mMember,
            mAncillaryData.c_str());
    }
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org