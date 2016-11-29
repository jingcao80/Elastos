
#ifndef __ORG_ALLJOYN_BUS_NATIVEBUSATTACHMENT_H__
#define __ORG_ALLJOYN_BUS_NATIVEBUSATTACHMENT_H__

#include "org/alljoyn/bus/NativeAboutObject.h"
#include "org/alljoyn/bus/NativeAuthListenerInternal.h"
#include "org/alljoyn/bus/NativeKeyStoreListener.h"
#include "org/alljoyn/bus/NativePendingAsyncJoin.h"
#include "org/alljoyn/bus/NativePendingAsyncPing.h"
#include "org/alljoyn/bus/NativeSignalHandler.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/Pair.h>
#include <elastos/utility/etl/Vector.h>
#include <alljoyn/BusAttachment.h>
#include <qcc/atomic.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Etl::Pair;
using Elastos::Utility::Etl::Vector;

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeBusAttachment : public ajn::BusAttachment
{
public:
    NativeBusAttachment(
        /* [in] */ const char* applicationName,
        /* [in] */ bool allowRemoteMessages,
        /* [in] */ int concurrency);

    QStatus Connect(
        /* [in] */ const char* connectArgs,
        /* [in] */ IKeyStoreListener* keyStoreListener,
        /* [in] */ const char* authMechanisms,
        /* [in] */ IAuthListenerInternal* authListener,
        /* [in] */ const char* keyStoreFileName,
        /* [in] */ Boolean isShared);

    void Disconnect();

    QStatus EnablePeerSecurity(
        /* [in] */ const char* authMechanisms,
        /* [in] */ IAuthListenerInternal* authListener,
        /* [in] */ const char* keyStoreFileName,
        /* [in] */ Boolean isShared);

    QStatus RegisterBusObject(
        /* [in] */ const char* objPath,
        /* [in] */ IBusObject* busObject,
        /* [in] */ ArrayOf<IInterfaceDescription*>* busInterfaces,
        /* [in] */ Boolean secure,
        /* [in] */ const String& langTag,
        /* [in] */ const String& desc,
        /* [in] */ ITranslator* translator);

    void UnregisterBusObject(
        /* [in] */ IBusObject* busObject);

    QStatus EmitChangedSignal(
        /* [in] */ IBusObject* busObject,
        /* [in] */ const char* ifaceName,
        /* [in] */ const char* propName,
        /* [in] */ IInterface* val,
        /* [in] */ Int32 sessionId);

    QStatus SetAnnounceFlag(
        /* [in] */ IBusObject* busObject,
        /* [in] */ const char* ifaceName,
        /* [in] */ Boolean isAnnounced);

    template <typename T>
    QStatus RegisterSignalHandler(
        /* [in] */ const char* ifaceName,
        /* [in] */ const char* signalName,
        /* [in] */ IInterface* signalHandler,
        /* [in] */ IMethodInfo* method,
        /* [in] */ const char* ancillary);

    void UnregisterSignalHandler(
        /* [in] */ IInterface* signalHandler,
        /* [in] */ IMethodInfo* method);

    bool IsLocalBusObject(
        /* [in] */ IBusObject* busObject);

    void ForgetLocalBusObject(
        /* [in] */ IBusObject* busObject);

    int32_t IncRef(void)
    {
        return qcc::IncrementAndFetch(&mRefCount);
    }

    int32_t DecRef(void)
    {
        int32_t refs = qcc::DecrementAndFetch(&mRefCount);
        if (refs == 0) {
            delete this;
        }
        return refs;
    }

    int32_t GetRef(void)
    {
        return mRefCount;
    }

private:
    NativeBusAttachment(const NativeBusAttachment& other);
    NativeBusAttachment& operator =(const NativeBusAttachment& other);

    /*
     * Destructor is marked private since it should only be called from DecRef.
     */
    virtual ~NativeBusAttachment();

public:
    /**
     * A mutex to serialize access to bus attachment critical sections.  It
     * doesn't seem worthwhile to have any finer granularity than this.  Note
     * that this member is public since we trust that the native binding we
     * wrote will use it correctly.
     */
    Object mBaCommonLock;

    /**
     * A mutex to serialize method call, property, etc., access in any attached
     * ProxyBusObject.  This is a blunt instrument, but support for
     * multi-threading on client and service sides has not been completely
     * implemented, so we simply disallow it for now.
     */
    Object mBaProxyLock;

    /**
     * A vector of all of the C++ "halves" of the signal handler objects
     * associated with this bus attachment.  Note that this member is public
     * since we trust that the native binding we wrote will use it correctly.
     */
    Vector<Pair<AutoPtr<IInterface>, NativeSignalHandler*> > mSignalHandlers;

    /*
     * The single (optionsl) KeyStoreListener associated with this bus
     * attachment.  The KeyStoreListener and AuthListener work together to deal
     * with security exchanges over secure interfaces.  Note that this member is
     * public since we trust that the native binding we wrote will usse it
     * correctly.  When keyStoreListener is set, there must be a corresponding
     * strong reference to the associated Java Object set in
     * jkeyStoreListenerRef.
     */
    NativeKeyStoreListener* mKeyStoreListener;

    /**
     * A JNI strong global reference to The single (optional) Java KeyStoreListener
     * that has been associated with this bus attachment.  When jkeystoreListenerRef is
     * set, there must be a corresponding object pointer to an associated
     * C++ backing object set in keyStoreListener.
     */
    AutoPtr<IKeyStoreListener> mKeyStoreListenerRef;

    /**
     * The single (optional) C++ backing class for a provided AuthListener that
     * has been associated with this bus attachment.  The KeyStoreListener and
     * AuthListener work together to deal with security exchanges over secure
     * interfaces.  Note that this member is public since we trust that the
     * native binding we wrote will use it correctly.  When authListener is
     * set, there must be a corresponding strong reference to the associated
     * Java Object set in jauthListenerRef.
     */
    NativeAuthListenerInternal* mAuthListener;

    /**
     * The single (optional) C++ backing class for JAboutObject. The aboutObj
     * contain a global ref jaboutObjGlobalRef that must be cleared when the
     * BusAttachment is disconnected.
     */
    NativeAboutObject* mAboutObj;

    /**
     * A JNI strong global reference to The single (optional) Java AuthListener
     * that has been associated with this bus attachment.  When jauthListenerRef is
     * set, there must be a corresponding object pointer to an associated
     * C++ backing object set in authListener.
     */
    AutoPtr<IAuthListenerInternal> mAuthListenerRef;

    /**
     * A dedicated mutex to serialize access to the authListener,
     * authListenerRef, keyStoreListener and keyStoreListenerRef.  This is
     * required since we can't hold the common lock during callouts to Alljoyn
     * that may result in callins.  This describes the authentication process.
     * In order to prevent users from calling in right in the middle of an
     * authentication session and changing the authentication listeners out
     * from under us, we dedicate a lock that must be taken in order to make
     * a change.  This lock is held during the authentication process and during
     * the change process.
     */
    Object mBaAuthenticationChangeLock;

    /**
     * A list of strong references to Java bus listener objects.
     *
     * If clients use the unnamed parameter / unnamed class idiom to provide bus
     * listeners to registerBusListener, they can forget that the listeners
     * exist after the register call and never explicitly call unregister.
     *
     * Since we need these Java objects around, we need to hold a strong
     * reference to them to keep them from being garbage collected.
     *
     * Note that this member is public since we trust that the native binding we
     * wrote will use it correctly.
     */
    List< AutoPtr<IBusListener> > mBusListeners;

    /**
     * A list of strong references to Java translator objects.
     *
     * If clients use the unnamed parameter / unnamed class idiom to provide bus
     * listeners to setDescriptionTranslator, they can forget that the listeners
     * exist after the register call and never explicitly call unregister.
     *
     * Since we need these Java objects around, we need to hold a strong
     * reference to them to keep them from being garbage collected.
     *
     * Note that this member is public since we trust that the native binding we
     * wrote will usse it correctly.
     */
    List< AutoPtr<ITranslator> > mTranslators;

    /**
     * A list of strong references to Java Bus Objects we use to indicate that
     * we have a part ownership in a given object.  Used during destruction.
     *
     */
    List< AutoPtr<IBusObject> > mBusObjects;

    /**
     * A map from session ports to their associated Java session port listeners.
     *
     * This mapping must be on a per-bus attachment basis since the scope of the
     * uniqueness of a session port is per-bus attachment
     *
     * Note that this member is public since we trust that the native binding we
     * wrote will usse it correctly.
     */
    HashMap<ajn::SessionPort, AutoPtr<ISessionPortListener> > mSessionPortListenerMap;

    typedef struct {
        AutoPtr<ISessionListener> mHostedListener;
        AutoPtr<ISessionListener> mJoinedListener;
        AutoPtr<ISessionListener> mListener;
    } BusAttachmentSessionListeners;

    /**
     * A map from sessions to their associated Java session listeners.
     *
     * This mapping must be on a per-bus attachment basis since the uniqueness of a
     * session is per-bus attachment.
     *
     * Note that this member is public since we trust that the native binding we
     * wrote will usse it correctly.
     */

    HashMap<ajn::SessionId, BusAttachmentSessionListeners> mSessionListenerMap;

    /**
     * A List of pending asynchronous join operation informations.  We store
     * Java object references here while AllJoyn mulls over what it can do about
     * the operation. Note that this member is public since we trust that the
     * native binding we wrote will use it correctly.
     */
    List< AutoPtr<NativePendingAsyncJoin> > mPendingAsyncJoins;

    /**
     * A List of pending asynchronous ping operation informations.  We store
     * Java object references here while AllJoyn mulls over what it can do about
     * the operation. Note that this member is public since we trust that the
     * native binding we wrote will use it correctly.
     */
    List< AutoPtr<NativePendingAsyncPing> > mPendingAsyncPings;

private:
    /*
     * An intrusive reference count
     */
    volatile int32_t mRefCount;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_NATIVEBUSATTACHMENT_H__
