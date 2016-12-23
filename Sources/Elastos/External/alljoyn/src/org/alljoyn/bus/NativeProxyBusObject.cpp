
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/PropertiesChangedListener.h"
#include "org/alljoyn/bus/ProxyBusObject.h"
#include "org/alljoyn/bus/MsgArg.h"
#include "org/alljoyn/bus/NativeProxyBusObject.h"
#include "org/alljoyn/bus/NativePropertiesChangedListener.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("NativeProxyBusObject");

NativeProxyBusObject::NativeProxyBusObject(
    /* [in] */ IProxyBusObject* pbo,
    /* [in] */ NativeBusAttachment* nba,
    /* [in] */ const char* endpoint,
    /* [in] */ const char* path,
    /* [in] */ ajn::SessionId sessionId,
    /* [in] */ bool secure)
    : ajn::ProxyBusObject(*nba, endpoint, path, sessionId, secure)
    , mBusPtr(nba)
{
    assert(mBusPtr != NULL);
    mBusPtr->IncRef();

    IWeakReferenceSource::Probe(pbo)->GetWeakReference((IWeakReference**)&mPbo);
}

NativeProxyBusObject::~NativeProxyBusObject()
{
    assert(mBusPtr != NULL);

    AutoLock lock(mPropertiesChangedListenersLock);
    NativePropertiesChangedListener* ptr;
    IPropertiesChangedListener* listener;
    List< AutoPtr<Listener> >::Iterator it;
    for (it = mPropertiesChangedListeners.Begin(); it != mPropertiesChangedListeners.End(); ++it) {
        listener = (*it)->mListener;
        ptr = reinterpret_cast<NativePropertiesChangedListener*>(
            ((Org::Alljoyn::Bus::PropertiesChangedListener*)listener)->mHandle);
        if (ptr) {
            ajn::ProxyBusObject::UnregisterPropertiesChangedListener((*it)->mIfaceName.string(), *ptr);
        }
    }
}

ECode NativeProxyBusObject::AddInterfaceStatus(
    /* [in] */ IProxyBusObject* pbo,
    /* [in] */ NativeBusAttachment* busPtr,
    /* [in] */ const String& interfaceName)
{
    if (busPtr == NULL) {
        Logger::E(TAG, "AddInterface(): NULL bus pointer");
        return ER_FAIL;
    }

    Org::Alljoyn::Bus::ProxyBusObject* pboObj = (Org::Alljoyn::Bus::ProxyBusObject*)pbo;
    NativeProxyBusObject* proxyBusObj = reinterpret_cast<NativeProxyBusObject*>(pboObj->mHandle);
    assert(proxyBusObj);

    ECode status = pboObj->AddInterface(interfaceName);
    if (E_STATUS_OK != status) {
        Logger::E(TAG, "AddInterface(): Exception, ec=%08x", status);
        return status;
    }

    const ajn::InterfaceDescription* intf = busPtr->GetInterface(interfaceName.string());
    assert(intf);

    status = proxyBusObj->AddInterface(*intf);
    return status;
}

ECode NativeProxyBusObject::AddInterface(
    /* [in] */ IProxyBusObject* pbo,
    /* [in] */ NativeBusAttachment* busPtr,
    /* [in] */ const String& interfaceName)
{
    ECode status = AddInterfaceStatus(pbo, busPtr, interfaceName);
    if (status != E_STATUS_OK) {
        Logger::E(TAG, " AddInterface %s failed, ec=%08x.", interfaceName.string(), status);
    }
    return status;
}

ECode NativeProxyBusObject::RegisterPropertiesChangedListener(
    /* [in] */ const String& ifaceName,
    /* [in] */ ArrayOf<String>* properties,
    /* [in] */ IPropertiesChangedListener* inListener)
{
    Int32 numProps = properties ? properties->GetLength() : 0;
    NativePropertiesChangedListener* listener;
    listener = reinterpret_cast<NativePropertiesChangedListener*>(
        ((Org::Alljoyn::Bus::PropertiesChangedListener*)inListener)->mHandle);

    if (!listener) {
        Logger::E(TAG, "RegisterPropertiesChangedListener: listener is null.");
        return E_STATUS_FAIL;
    }

    AutoPtr<IProxyBusObject> pbo;
    mPbo->Resolve(EIID_IProxyBusObject, (IInterface**)&pbo);
    if (!pbo) {
        Logger::E(TAG, "RegisterPropertiesChangedListener(): Can't get new local reference to ProxyBusObject");
        return E_STATUS_FAIL;
    }

    ECode status = E_STATUS_FAIL;

    if (!ImplementsInterface(ifaceName.string())) {
        status = AddInterfaceStatus(pbo, mBusPtr, ifaceName);
        if (status != E_STATUS_OK) {
            Logger::E(TAG, "RegisterPropertiesChangedListener(): AddInterfaceStatus Exception");
            return status;
        }
    }

    const char** props = new const char*[numProps];
    memset(props, 0, numProps * sizeof(props[0]));
    for (Int32 i = 0; i < numProps; ++i) {
        props[i] = (*properties)[i].string();
    }

    status = ajn::ProxyBusObject::RegisterPropertiesChangedListener(ifaceName.string(), props, numProps, *listener, this);
    if (status == ER_OK) {
        AutoLock lock(mPropertiesChangedListenersLock);
        AutoPtr<Listener> l = new Listener(ifaceName, inListener);
        mPropertiesChangedListeners.PushBack(l);
    }

    delete [] props;
    return status;
}

ECode NativeProxyBusObject::UnregisterPropertiesChangedListener(
    /* [in] */ const String& ifaceName,
    /* [in] */ IPropertiesChangedListener* inListener)
{
    NativePropertiesChangedListener* listener;
    listener = reinterpret_cast<NativePropertiesChangedListener*>(
        ((Org::Alljoyn::Bus::PropertiesChangedListener*)inListener)->mHandle);
    ECode status = ajn::ProxyBusObject::UnregisterPropertiesChangedListener(ifaceName.string(), *listener);

    if (status == E_STATUS_OK) {
        AutoLock lock(mPropertiesChangedListenersLock);

        List< AutoPtr<Listener> >::Iterator it = mPropertiesChangedListeners.Begin();
        for (; it != mPropertiesChangedListeners.End(); ++it) {
            if (inListener == (*it)->mListener) {
                mPropertiesChangedListeners.Erase(it);
                break;
            }
        }
    }
    return status;
}


} // namespace Bus
} // namespace Alljoyn
} // namespace Org
