
#include "org/alljoyn/bus/BusListener.h"
#include "org/alljoyn/bus/NativeApi.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/NativeBusListener.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

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
            Logger::E("NativeBusAttachment", "Disconnect failed");
        }
    }

    // TODO: DisablePeerSecurity
    // TODO: UnregisterKeyStoreListener
    if (IsStarted()) {
        QStatus status = Stop();
        if (ER_OK != status) {
            Logger::E("NativeBusAttachment", "Stop failed");
        }

        status = Join();
        if (ER_OK != status) {
            Logger::E("NativeBusAttachment", "Join failed");
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
    for (List<NativePendingAsyncJoin*>::Iterator i = mPendingAsyncJoins.Begin(); i != mPendingAsyncJoins.End(); ++i) {
        delete (*i);
    }
    mPendingAsyncJoins.Clear();

    /*
     * Release any strong references we may hold to objects passed in through an
     * async ping.  We assume that since we have done a disconnect/stop/join, there
     * will never be a callback firing that expects to call out into one of these.
     */
    for (List<NativePendingAsyncPing*>::Iterator i = mPendingAsyncPings.Begin(); i != mPendingAsyncPings.End(); ++i) {
        delete (*i);
    }
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

bool NativeBusAttachment::IsLocalBusObject(
    /* [in] */ IBusObject* busObject)
{
    for (List< AutoPtr<IBusObject> >::Iterator i = mBusObjects.Begin(); i != mBusObjects.End(); ++i) {
        if (Object::Equals(busObject, (*i).Get())) {
            return true;
        }
    }

    return false;
}

void NativeBusAttachment::ForgetLocalBusObject(
    /* [in] */ IBusObject* busObject)
{
    for (List< AutoPtr<IBusObject> >::Iterator i = mBusObjects.Begin(); i != mBusObjects.End(); ++i) {
        if (Object::Equals(busObject, (*i).Get())) {
            mBusObjects.Erase(i);
            return;
        }
    }
}

QStatus NativeBusAttachment::RegisterBusObject(
    /* [in] */ const char* objPath,
    /* [in] */ IBusObject* busObject,
    /* [in] */ ArrayOf<InterfaceID>* busInterfaces,
    /* [in] */ Boolean secure,
    /* [in] */ const String& langTag,
    /* [in] */ const String& desc,
    /* [in] */ ITranslator* translator)
{

}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org
