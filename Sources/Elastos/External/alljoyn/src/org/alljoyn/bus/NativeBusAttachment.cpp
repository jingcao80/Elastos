
#include "org/alljoyn/bus/BusListener.h"
#include "org/alljoyn/bus/NativeApi.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/NativeBusListener.h"
#include "org/alljoyn/bus/NativeBusObject.h"
#include <alljoyn/MsgArg.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("NativeBusAttachment");

NativeBusAttachment::NativeBusAttachment(
    /* [in] */ const char* applicationName,
    /* [in] */ bool allowRemoteMessages,
    /* [in] */ int concurrency)
    : BusAttachment(applicationName, allowRemoteMessages, concurrency)
    , mKeyStoreListener(NULL)
    , mAuthListener(NULL)
    , mAboutObj(NULL)
    , mRefCount(1)
{}

NativeBusAttachment::~NativeBusAttachment()
{
    /*
     * Note that the Bus Objects for this Bus Attachment are assumed to have
     * previously been released, since they will have held references to the
     * bus attachment that would have prevented its reference count from
     * going to zero and thus kept the bus attachment alive.
     */
    assert(mBusObjects.GetSize() == 0);
}


void NativeBusAttachment::Destroy()
{
    /*
     * We want to allow users to forget the BusAttachent in Java by setting a
     * reference to null.  We want to reclaim all of our resources, including
     * those held by BusObjects which hold references to our bus attachment.  We
     * don't want to force our user to explicitly unregister those bus objects,
     * which is the only way we can get an indication that the BusObject is
     * going away.  This is becuase BusObjects are interfaces and we have no way
     * to hook the finalize on those objects and drive release of the underlying
     * resources.
     *
     * So we want to and can use this method (destroy) to drive the release of
     * all of the Java bus object C++ backing objects.  Since the garbage
     * collector has run on the bus attachment (we are running here) we know
     * there is no way for a user to access the bus attachment.  We assume that
     * the BusAttachment release() and or finalize() methods have ensured that
     * the BusAttachment is disconnected and stopped, so it will never call out
     * to any of its associated objects.
     *
     * So, we release references to the Bus Objects that this particular Bus
     * Attachment holds now.  The theory is that nothing else can be accessing
     * the bus attachment or the bus obejcts, so we don't need to take the
     * multithread locks any more than the bus attachment destructor will.
     */
    List< AutoPtr<IBusObject> >::Iterator it;
    for (it = mBusObjects.Begin(); it != mBusObjects.End(); ++it) {
        /*
         * If we are the last BusAttachment to use this bus Object, we acquire
         * the memory management responsibility for the associated C++ object.
         * This is a vestige of an obsolete idea, but we still need to do it.
         * We expect we will always have the memory management responsibility.
         */
        NativeBusObject* cppObject = DecRefBackingObject(*it);
        if (cppObject) {
            delete cppObject;
            cppObject = NULL;
        }
    }
    mBusObjects.Clear();
}

QStatus NativeBusAttachment::Connect(
    /* [in] */ const char* connectArgs,
    /* [in] */ IKeyStoreListener* keyStoreListener,
    /* [in] */ const char* authMechanisms,
    /* [in] */ IAuthListenerInternal* authListener,
    /* [in] */ const char* keyStoreFileName,
    /* [in] */ Boolean isShared)
{
    QStatus status = Start();
    if (ER_OK != status) {
        return status;
    }

    AutoLock lock(mBaAuthenticationChangeLock);

    if (keyStoreListener) {
        mKeyStoreListenerRef = keyStoreListener;

        mKeyStoreListener = new NativeKeyStoreListener(keyStoreListener);
        assert(mKeyStoreListener != NULL);

        RegisterKeyStoreListener(*mKeyStoreListener);
    }

    status = EnablePeerSecurity(authMechanisms, authListener, keyStoreFileName, isShared);
    if (ER_OK != status) {
        goto exit;
    }

    status = BusAttachment::Connect(connectArgs);

exit:
    if (ER_OK != status) {
        Disconnect();

        mKeyStoreListenerRef = NULL;

        delete mKeyStoreListener;
        mKeyStoreListener = NULL;
    }

    return status;
}

void NativeBusAttachment::Disconnect()
{
    if (IsConnected()) {
        QStatus status = BusAttachment::Disconnect();
        if (ER_OK != status) {
            Logger::E(TAG, "Disconnect failed");
        }
    }

    // TODO: DisablePeerSecurity
    // TODO: UnregisterKeyStoreListener
    if (IsStarted()) {
        QStatus status = Stop();
        if (ER_OK != status) {
            Logger::E(TAG, "Stop failed");
        }

        status = Join();
        if (ER_OK != status) {
            Logger::E(TAG, "Join failed");
        }
    }

    AutoLock lock1(gBusObjectMapLock);

    AutoLock lock2(mBaCommonLock);

    /*
     * Release any strong references we may hold to Java bus listener objects.
     */
    for (List< AutoPtr<IBusListener> >::Iterator i = mBusListeners.Begin(); i != mBusListeners.End(); ++i) {
        NativeBusListener* listener = reinterpret_cast<NativeBusListener*>(((BusListener*)(*i).Get())->mHandle);
        UnregisterBusListener(*listener);
    }
    mBusListeners.Clear();

    /*
     * Release any strong references we may hold to Java translator objects.
     */
    mTranslators.Clear();

    /*
     * Release any strong references we may hold to objects passed in through an
     * async join.  We assume that since we have done a disconnect/stop/join, there
     * will never be a callback firing that expects to call out into one of these.
     */
    mPendingAsyncJoins.Clear();

    /*
     * Release any strong references we may hold to objects passed in through an
     * async ping.  We assume that since we have done a disconnect/stop/join, there
     * will never be a callback firing that expects to call out into one of these.
     */
    mPendingAsyncPings.Clear();

    /*
     * Release any strong references we may hold to objects passed in through a
     * bind.
     */
    for (HashMap<ajn::SessionPort, AutoPtr<ISessionPortListener> >::Iterator i = mSessionPortListenerMap.Begin(); i != mSessionPortListenerMap.End(); ++i) {
        if (i->mSecond != NULL) {
            UnbindSessionPort(i->mFirst);
        }
    }
    mSessionPortListenerMap.Clear();

    /*
     * Release any strong references we may hold to objects passed in through a
     * join session.
     */
    for (HashMap<ajn::SessionId, BusAttachmentSessionListeners>::Iterator i = mSessionListenerMap.Begin(); i != mSessionListenerMap.End(); ++i) {
        if (i->mSecond.mHostedListener) {
            SetHostedSessionListener(i->mFirst, 0);
        }
        if (i->mSecond.mJoinedListener) {
            SetJoinedSessionListener(i->mFirst, 0);
        }
        if (i->mSecond.mListener) {
            SetSessionListener(i->mFirst, 0);
        }
    }
    mSessionListenerMap.Clear();

    /*
     * Release any strong references we may hold to objects passed in through a
     * security API.
     */
    if (mAuthListener) {
        EnablePeerSecurity(0, 0, 0, true);
        delete mAuthListener;
    }
    mAuthListener = NULL;
    mAuthListenerRef = NULL;

    delete mKeyStoreListener;
    mKeyStoreListener = NULL;
    mKeyStoreListenerRef = NULL;

    if (mAboutObj != NULL) {
        if (mAboutObj->mAboutObjGlobalRef != NULL) {
            mAboutObj->mAboutObjGlobalRef = NULL;
        }
    }
}

QStatus NativeBusAttachment::EnablePeerSecurity(
    /* [in] */ const char* authMechanisms,
    /* [in] */ IAuthListenerInternal* authListener,
    /* [in] */ const char* keyStoreFileName,
    /* [in] */ Boolean isShared)
{
    if (!authMechanisms || !IsStarted()) {
        return ER_OK;
    }

    AutoLock lock1(mBaAuthenticationChangeLock);

    {
        AutoLock lock2(mBaCommonLock);

        if (authListener == NULL) return ER_FAIL;
        mAuthListenerRef = authListener;

        delete mAuthListener;
        mAuthListener = new NativeAuthListenerInternal(this, authListener);
        assert(mAuthListener != NULL);
    }

    QStatus status = ajn::BusAttachment::EnablePeerSecurity(authMechanisms, mAuthListener, keyStoreFileName, isShared);

    {
        AutoLock lock2(mBaCommonLock);

        if (ER_OK != status) {
            delete mAuthListener;
            mAuthListener = NULL;

            mAuthListenerRef = NULL;
        }
    }

    return status;
}

bool NativeBusAttachment::IsSecureBusObject(
    /* [in] */ IBusObject* busObject)
{
    AutoLock lock(gBusObjectMapLock);

    NativeBusObject* ntBusObject = GetBackingObject(busObject);
    if (ntBusObject) {
        Logger::E(TAG, "%s: Exception.", __FUNCTION__);
        return false;
    }

    return ntBusObject->IsSecure();
}

bool NativeBusAttachment::IsLocalBusObject(
    /* [in] */ IBusObject* busObject)
{
    for (List< AutoPtr<IBusObject> >::Iterator i = mBusObjects.Begin(); i != mBusObjects.End(); ++i) {
        if (busObject == (*i).Get()) {
            return true;
        }
    }

    return false;
}

void NativeBusAttachment::ForgetLocalBusObject(
    /* [in] */ IBusObject* busObject)
{
    for (List< AutoPtr<IBusObject> >::Iterator i = mBusObjects.Begin(); i != mBusObjects.End(); ++i) {
        if (busObject == (*i).Get()) {
            mBusObjects.Erase(i);
            return;
        }
    }
}

QStatus NativeBusAttachment::RegisterBusObject(
    /* [in] */ const char* objPath,
    /* [in] */ IBusObject* busObject,
    /* [in] */ ArrayOf<IInterfaceDescription*>* busInterfaces,
    /* [in] */ Boolean secure,
    /* [in] */ const String& langTag,
    /* [in] */ const String& desc,
    /* [in] */ ITranslator* translator)
{
    AutoLock lock1(gBusObjectMapLock);

    AutoLock lock2(mBaCommonLock);

    /*
     * It is a programming error to register any bus object with a given bus
     * attachment multiple times.
     */
    if (IsLocalBusObject(busObject)) {
        return ER_BUS_OBJ_ALREADY_EXISTS;
    }

    if (busObject == NULL) return ER_FAIL;

    /*
     * It is a programming error to register the same Java Bus Object with
     * multiple bus attachments.  It looks like it should be possible from
     * the top, but that is not the case.
     */
    NativeBusObject* ntBusObject = GetBackingObject(busObject);
    if (ntBusObject) {
        return ER_BUS_OBJ_ALREADY_EXISTS;
    }
    else {
        ntBusObject = new NativeBusObject(this, objPath, busObject);
        QStatus status = ntBusObject->AddInterfaces(busInterfaces);
        if (status != ER_OK) {
            delete ntBusObject;
            return ER_FAIL;
        }
        ntBusObject->SetDescriptions(langTag, desc, translator);

        NewRefBackingObject(busObject, ntBusObject);
    }

    /*
     * After we enter this call, AllJoyn has its hands on the bus object and
     * calls in can start flowing.
     */
    QStatus status = BusAttachment::RegisterBusObject(*ntBusObject, secure);
    if (status != ER_OK) {
        /*
         * AllJoyn balked at us for some reason.  As a result we really don't
         * need to have a hold on any of the objects we've acquired references
         * to or created.  If we created the C++ backing object, we'll get
         * responsibility for its disposition from DecRefBackingObject.
         * release our global reference to that as well.
         */
        NativeBusObject* cppObject = DecRefBackingObject(busObject);
        if (cppObject) {
            delete cppObject;
            cppObject = NULL;
        }
    }
    else {
        /*
         * The registration is successful. Put the global ref in the internal
         * list of busobjects registered to this the busattachment.
         */
        mBusObjects.PushBack(busObject);
    }

    return status;
}

void NativeBusAttachment::UnregisterBusObject(
    /* [in] */ IBusObject* busObject)
{
    AutoLock lock1(gBusObjectMapLock);

    AutoLock lock2(mBaCommonLock);

    /*
     * It is a programming error to 1) register a Bus Object on one Bus
     * Attachment and unregister it on another; 2) unregister a Bus Object that
     * has never been regsitered with the given Bus Attachment; and 3)
     * unregister a Bus Object multiple times on a given Bus Attachment.  All of
     * these cases are caught by making sure the provided Java Bus Object is
     * currently in the list of Java Objects associated with this bus
     * Attachment.
     */
    if (!IsLocalBusObject(busObject)) {
        Logger::E(TAG, "UnregisterBusObject(): No existing Bus Object");
        return;
    }

    NativeBusObject* cppObject = GetBackingObject(busObject);
    if (cppObject == NULL) {
        Logger::E(TAG, "UnregisterBusObject(): No existing Backing Object");
        return;
    }

    /*
     * As soon as this call completes, AllJoyn will not make any further
     * calls into the object, so we can safely get rid of it, and we can
     * release our hold on the corresponding Java object and allow it to
     * be garbage collected.
     */
    BusAttachment::UnregisterBusObject(*cppObject);

    /*
     * AllJoyn doesn't have its grubby little hands on the C++ Object any
     * more.  As a result we shouldn't have a hold on any of the objects
     * we've acquired to support the plumbing.
     *
     * Just because we don't need the C++ object doesn't mean that other
     * bus attachments don't need it, so we need to pay attention to the
     * reference counting mechanism.  If DecRefBackingObject returns a
     * pointer to the object, we
     */
    AutoPtr<IBusObject> jo = GetGlobalRefForObject(busObject);

    NativeBusObject* cppObjectToDelete = DecRefBackingObject(jo);
    if (cppObjectToDelete) {
        /*
         * The object we delete had better be the object we just told AllJoyn
         * about.
         */
        assert(cppObjectToDelete == cppObject);
        delete cppObject;
        cppObject = NULL;
    }

    /*
     * AllJoyn shouldn't be remembering the Java Bus Object as a bus
     * object associated with this bus attachment.  We've now changed
     * the structure of the busObjects list so the iterator is
     * invalid, so mark it as such.
     */
    ForgetLocalBusObject(jo);
}

QStatus NativeBusAttachment::EmitChangedSignal(
    /* [in] */ IBusObject* busObject,
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* propName,
    /* [in] */ IInterface* val,
    /* [in] */ Int32 sessionId)
{
    AutoLock lock1(gBusObjectMapLock);

    NativeBusObject* cppObject = GetBackingObject(busObject);
    if (cppObject == NULL) {
        Logger::E(TAG, "EmitChangedSignal(): No existing Backing Object");
        return ER_FAIL;
    }

    QStatus status = ER_OK;

    ajn::MsgArg* arg = NULL;
    ajn::MsgArg value;

    if (propName) {
        const ajn::BusAttachment& bus = cppObject->GetBusAttachment();
        const ajn::InterfaceDescription* iface = bus.GetInterface(ifaceName);
        assert(iface);
        const ajn::InterfaceDescription::Property* prop = iface->GetProperty(propName);
        assert(prop);
        // arg = MsgArg::::Marshal(prop->signature.c_str(), propName, &value);
        assert(0 && "TODO");
    }

    if (cppObject) {
        cppObject->EmitPropChanged(ifaceName, propName, (arg ? *arg : value), sessionId);
    }

    return status;
}

QStatus NativeBusAttachment::SetAnnounceFlag(
    /* [in] */ IBusObject* busObject,
    /* [in] */ const char* ifaceName,
    /* [in] */ Boolean isAnnounced)
{
    AutoLock lock(gBusObjectMapLock);

    NativeBusObject* cppObject = GetBackingObject(busObject);

    if (!cppObject) {
        Logger::E(TAG, "SetAnnounceFlag: BusObject not found");
        return ER_BUS_NO_SUCH_OBJECT;
    }

    QStatus status = ER_OK;
    const ajn::InterfaceDescription* iface = GetInterface(ifaceName);
    if (!iface) {
        return ER_BUS_OBJECT_NO_SUCH_INTERFACE;
    }

    if (isAnnounced) {
        status = cppObject->SetAnnounceFlag(iface, ajn::BusObject::ANNOUNCED);
    }
    else {
        status = cppObject->SetAnnounceFlag(iface, ajn::BusObject::UNANNOUNCED);
    }

    return status;
}

template <typename T>
QStatus NativeBusAttachment::RegisterSignalHandler(
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ IInterface* signalHandler,
    /* [in] */ IMethodInfo* method,
    /* [in] */ const char* ancillary)
{
    AutoLock lock(mBaCommonLock);

    if (signalHandler == NULL) return ER_FAIL;

    /*
     * Create the C++ object that backs the Java signal handler object.
     */
    NativeSignalHandler* ntSignalHandler = new T(signalHandler, method);
    if (ntSignalHandler == NULL) return ER_FAIL;

    /*
     * Wire the C++ signal handler to the Java signal handler and if the
     * operation was successful, remember both the Java object and the C++
     * object.  If it didn't work then we might as well forget them both.
     */
    QStatus status = ntSignalHandler->Register(*this, ifaceName, signalName, ancillary);
    if (ER_OK == status) {
        mSignalHandlers.PushBack(Pair<AutoPtr<IInterface>, NativeSignalHandler*>(signalHandler, ntSignalHandler));
    }
    else {
        delete ntSignalHandler;
    }

    return status;
}

void NativeBusAttachment::UnregisterSignalHandler(
    /* [in] */ IInterface* signalHandler,
    /* [in] */ IMethodInfo* method)
{
    AutoLock lock(mBaCommonLock);

    for (Vector<Pair<AutoPtr<IInterface>, NativeSignalHandler*> >::Iterator i = mSignalHandlers.Begin();
            i != mSignalHandlers.End(); ++i) {
        if ((*i).mSecond->IsSameObject(signalHandler, method)) {
            (*i).mSecond->Unregister(*this);
            delete ((*i).mSecond);
            mSignalHandlers.Erase(i);
            break;
        }
    }
}

QStatus NativeBusAttachment::RegisterNativeSignalHandlerWithSrcPath(
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ IInterface* signalHandler,
    /* [in] */ IMethodInfo* method,
    /* [in] */ const char* ancillary)
{
    return RegisterSignalHandler<NativeSignalHandlerWithSrc>(
        ifaceName, signalName, signalHandler, method, ancillary);
}

QStatus NativeBusAttachment::RegisterNativeSignalHandlerWithRule(
    /* [in] */ const char* ifaceName,
    /* [in] */ const char* signalName,
    /* [in] */ IInterface* signalHandler,
    /* [in] */ IMethodInfo* method,
    /* [in] */ const char* ancillary)
{
    return RegisterSignalHandler<NativeSignalHandlerWithRule>(
        ifaceName, signalName, signalHandler, method, ancillary);
}


} // namespace Bus
} // namespace Alljoyn
} // namespace Org
