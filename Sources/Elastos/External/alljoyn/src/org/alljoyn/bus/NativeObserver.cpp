
#include "org/alljoyn/bus/Observer.h"
#include "org/alljoyn/bus/NativeObserver.h"
#include "org/alljoyn/bus/CBusAttachment.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include <elastos/utility/logging/Logger.h>

#include <alljoyn/CoreObserver.h>
#include <alljoyn/BusInternal.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("NativeObserver");

NativeObserver::NativeObserver(
        /* [in] */ IBusAttachment* inbus,
    /* [in] */ IObserver* listener,
    /* [in] */ const InterfaceSet& mandatory)
    : ajn::CoreObserver(mandatory)
{
    mBus = inbus;
    CBusAttachment* busObj = (CBusAttachment*)inbus;
    NativeBusAttachment* bus = reinterpret_cast<NativeBusAttachment*>(busObj->mHandle);
    if (bus == NULL) {
        Logger::E(TAG, "Observer_create(): NULL BusAttachment");
        return;
    }

    bus->GetInternal().GetObserverManager().RegisterObserver(this);
}

NativeObserver::~NativeObserver()
{}

void NativeObserver::ObjectDiscovered(
    /* [in] */ const ajn::ObjectId& oid,
    /* [in] */ const InterfaceSet& interfaces,
    /* [in] */ ajn::SessionId sessionid)
{
    assert(0 && "TODO");
    // JScopedEnv env;

    // jobject jo = env->NewLocalRef(jobserver);
    // if (!jo) {
    //     return;
    // }

    // JLocalRef<jclass> clazz = env->GetObjectClass(jo);
    // if (clazz == NULL) {
    //     return;
    // }
    // jmethodID mid = env->GetMethodID(clazz, "objectDiscovered", "(Ljava/lang/String;Ljava/lang/String;[Ljava/lang/String;I)V");
    // if (!mid) {
    //     return;
    // }

    // JLocalRef<jstring> busname = env->NewStringUTF(oid.uniqueBusName.c_str());
    // if (env->ExceptionCheck()) {
    //     QCC_LogError(ER_FAIL, ("JObserver::ObjectDiscovered exception"));
    //     return;
    // }
    // JLocalRef<jstring> path = env->NewStringUTF(oid.objectPath.c_str());
    // if (env->ExceptionCheck()) {
    //     QCC_LogError(ER_FAIL, ("JObserver::ObjectDiscovered exception"));
    //     return;
    // }
    // jint jsessionid = sessionid;
    // JLocalRef<jobjectArray> jinterfaces = env->NewObjectArray(interfaces.size(), CLS_String, NULL);
    // if (env->ExceptionCheck()) {
    //     QCC_LogError(ER_FAIL, ("JObserver::ObjectDiscovered exception"));
    //     return;
    // }

    // jsize i = 0;
    // for (InterfaceSet::iterator it = interfaces.begin(); it != interfaces.end(); ++it) {
    //     JLocalRef<jstring> intfname = env->NewStringUTF(it->c_str());
    //     if (env->ExceptionCheck()) {
    //         QCC_LogError(ER_FAIL, ("JObserver::ObjectDiscovered exception"));
    //         return;
    //     }
    //     env->SetObjectArrayElement(jinterfaces, i++, intfname);
    //     if (env->ExceptionCheck()) {
    //         QCC_LogError(ER_FAIL, ("JObserver::ObjectDiscovered exception"));
    //         return;
    //     }
    // }

    // env->CallVoidMethod(jo, mid, jstring(busname), jstring(path), jobjectArray(jinterfaces), jsessionid);
}

void NativeObserver::ObjectLost(
    /* [in] */ const ajn::ObjectId& oid)
{
    AutoPtr<IObserver> o;
    mObserver->Resolve(EIID_IObserver, (IInterface**)&o);
    if (o == NULL) {
        return;
    }

    String busname(oid.uniqueBusName.c_str());
    String path(oid.objectPath.c_str());

    Observer* observer = (Observer*)o.Get();
    observer->ObjectLost(busname, path);
}

void NativeObserver::EnablePendingListeners()
{
    AutoPtr<IObserver> o;
    mObserver->Resolve(EIID_IObserver, (IInterface**)&o);
    if (o == NULL) {
        return;
    }

    Observer* observer = (Observer*)o.Get();
    observer->EnablePendingListeners();
}

void NativeObserver::TriggerEnablePendingListeners()
{
    CBusAttachment* busObj = (CBusAttachment*)mBus.Get();
    NativeBusAttachment* bus = reinterpret_cast<NativeBusAttachment*>(busObj->mHandle);
    if (bus == NULL) {
        Logger::E(TAG, "TriggerEnablePendingListeners(): NULL BusAttachment");
        return;
    }

    bus->GetInternal().GetObserverManager().EnablePendingListeners(this);
}

void NativeObserver::Detach()
{
    CBusAttachment* busObj = (CBusAttachment*)mBus.Get();
    NativeBusAttachment* bus = reinterpret_cast<NativeBusAttachment*>(busObj->mHandle);
    if (bus == NULL) {
        Logger::E(TAG, "Detach(): NULL BusAttachment");
        return;
    }

    bus->GetInternal().GetObserverManager().UnregisterObserver(this);
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org