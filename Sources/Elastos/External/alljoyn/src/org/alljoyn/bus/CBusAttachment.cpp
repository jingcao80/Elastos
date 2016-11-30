
#include "org/alljoyn/bus/BusListener.h"
#include "org/alljoyn/bus/CBusAttachment.h"
#include "org/alljoyn/bus/InterfaceDescription.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/NativeBusListener.h"
#include "org/alljoyn/bus/NativeSessionListener.h"
#include "org/alljoyn/bus/NativeSessionPortListener.h"
#include "org/alljoyn/bus/SessionListener.h"
#include "org/alljoyn/bus/SessionPortListener.h"
#include "org/alljoyn/bus/NativeMessageContext.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Org::Alljoyn::Bus::Ifaces::EIID_IDBusProxyObj;
using Elastos::Core::AutoLock;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::Concurrent::CExecutors;
using Elastos::Utility::Concurrent::IExecutors;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

static const String TAG("CBusAttachment");

extern void GlobalInitialize();

typedef enum {
    BA_HSL,     // BusAttachment hosted session listener index
    BA_JSL,     // BusAttachment joined session listener index
    BA_SL,     // BusAttachment session listener index
    BA_LAST     // indicates the size of the enum
} BusAttachmentSessionListenerIndex;

//============================================================================
// CBusAttachment::AuthListenerInternal
//============================================================================
const Int32 CBusAttachment::AuthListenerInternal::PASSWORD;
const Int32 CBusAttachment::AuthListenerInternal::USER_NAME;
const Int32 CBusAttachment::AuthListenerInternal::CERT_CHAIN;
const Int32 CBusAttachment::AuthListenerInternal::PRIVATE_KEY;
const Int32 CBusAttachment::AuthListenerInternal::LOGON_ENTRY;
const Int32 CBusAttachment::AuthListenerInternal::EXPIRATION;
const Int32 CBusAttachment::AuthListenerInternal::NEW_PASSWORD;
const Int32 CBusAttachment::AuthListenerInternal::ONE_TIME_PWD;

CAR_INTERFACE_IMPL(CBusAttachment::AuthListenerInternal, Object, IAuthListenerInternal);

ECode CBusAttachment::AuthListenerInternal::SetAuthListener(
    /* [in] */ IAuthListener* authListener)
{
    return NOERROR;
}

ECode CBusAttachment::AuthListenerInternal::AuthListenerSet(
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CBusAttachment::AuthListenerInternal::SetSecurityViolationListener(
    /* [in] */ ISecurityViolationListener* violationListener)
{
    return NOERROR;
}

ECode CBusAttachment::AuthListenerInternal::RequestCredentials(
    /* [in] */ const String& authMechanism,
    /* [in] */ const String& authPeer,
    /* [in] */ Int32 authCount,
    /* [in] */ const String& userName,
    /* [in] */ Int32 credMask,
    /* [out] */ ICredentials** credentials)
{
    return NOERROR;
}

ECode CBusAttachment::AuthListenerInternal::VerifyCredentials(
    /* [in] */ const String& authMechanism,
    /* [in] */ const String& peerName,
    /* [in] */ const String& userName,
    /* [in] */ const String& cert,
    /* [out] */ Boolean* result)
{
    return NOERROR;
}

ECode CBusAttachment::AuthListenerInternal::SecurityViolation(
    /* [in] */ ECode status)
{
    return NOERROR;
}

ECode CBusAttachment::AuthListenerInternal::AuthenticationComplete(
    /* [in] */ const String& authMechanism,
    /* [in] */ const String& peerName,
    /* [in] */ Boolean success)
{
    return NOERROR;
}


//============================================================================
// CBusAttachment
//============================================================================
static AutoPtr<IHashSet> Init_sBusAttachmentSet()
{
    AutoPtr<IHashSet> hashSet;
    CHashSet::New((IHashSet**)&hashSet);
    return hashSet;
}

const Int32 CBusAttachment::DEFAULT_CONCURRENCY = 4;
AutoPtr<IHashSet> CBusAttachment::sBusAttachmentSet = Init_sBusAttachmentSet();
Boolean CBusAttachment::sShutdownHookRegistered = FALSE;
const String CBusAttachment::TAG("CBusAttachment");

CAR_INTERFACE_IMPL(CBusAttachment, Object, IBusAttachment);

CAR_OBJECT_IMPL(CBusAttachment);

CBusAttachment::CBusAttachment()
    : mIsShared(FALSE)
    , mIsConnected(FALSE)
{
    GlobalInitialize();
}

/**
 * Constructs a BusAttachment.
 *
 * @param applicationName the name of the application
 * @param policy if this attachment is allowed to receive messages from
 *               remote devices
 * @param concurrency The maximum number of concurrent method and signal
 *                    handlers locally executing.
 */
ECode CBusAttachment::constructor(
    /* [in] */ const String& applicationName,
    /* [in] */ RemoteMessage policy,
    /* [in] */ Int32 concurrency)
{
    mAllowRemoteMessages = (policy == RemoteMessage_Receive);
    mBusAuthListener = new AuthListenerInternal();
    Create(applicationName, mAllowRemoteMessages, concurrency);

    /*
     * Create a separate dbus bus object (dbusbo) and interface so we get at
     * it and can quickly release its resources when we're done with it.
     * The corresponding interface (dbus) is what we give the clients.
     */
    AutoPtr< ArrayOf<IInterfaceInfo*> > busInterfaces = ArrayOf<IInterfaceInfo*>::Alloc(1);
    assert(0);
    // (*busInterfaces)[0] = EIID_IDBusProxyObj;
    mDbusbo = new ProxyBusObject();
    mDbusbo->constructor(this, String("org.freedesktop.DBus"), String("/org/freedesktop/DBus"), SESSION_ID_ANY,
            busInterfaces);
    mDbusbo->GetInterface(EIID_IDBusProxyObj, (IInterface**)&mDbus);
    AutoPtr<IExecutors> executors;
    CExecutors::AcquireSingleton((IExecutors**)&executors);
    executors->NewSingleThreadExecutor((IExecutorService**)&mExecutor);
    CHashSet::New((ISet**)&mRegisteredAboutListeners);
    return NOERROR;
}

/**
 * Constructs a BusAttachment.
 *
 * @param applicationName the name of the application
 * @param policy if this attachment is allowed to receive messages
 *               from remote devices
 */
ECode CBusAttachment::constructor(
    /* [in] */ const String& applicationName,
    /* [in] */ RemoteMessage policy)
{
    return constructor(applicationName, policy, DEFAULT_CONCURRENCY);
}

/**
 * Construct a BusAttachment that will only communicate on the local device.
 *
 * @param applicationName the name of the application
 */
ECode CBusAttachment::constructor(
    /* [in] */ const String& applicationName)
{
    return constructor(applicationName, RemoteMessage_Ignore, DEFAULT_CONCURRENCY);
}

void CBusAttachment::Create(
    /* [in] */ const String& applicationName,
    /* [in] */ Boolean allowRemoteMessages,
    /* [in] */ Int32 concurrency)
{
    NativeBusAttachment* busPtr = new NativeBusAttachment(applicationName.string(), allowRemoteMessages, concurrency);
    assert(busPtr != NULL);

    mHandle = reinterpret_cast<Int64>(busPtr);
}

ECode CBusAttachment::Connect(
    /* [in] */ const String& connectArgs,
    /* [in] */ IKeyStoreListener* keyStoreListener,
    /* [in] */ const String& authMechanisms,
    /* [in] */ AuthListenerInternal* busAuthListener,
    /* [in] */ const String& keyStoreFileName,
    /* [in] */ Boolean isShared)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->Connect(connectArgs.string(), keyStoreListener, authMechanisms.string(),
            busAuthListener, keyStoreFileName.string(), isShared);
    if (status != ER_OK) {
        Logger::E(TAG, "Connect(): Exception");
    }
    return status;
}

ECode CBusAttachment::RegisterBusObject(
    /* [in] */ const String& objPath,
    /* [in] */ IBusObject* busObj,
    /* [in] */ ArrayOf<IInterfaceDescription*>* busInterfaces,
    /* [in] */ Boolean secure,
    /* [in] */ const String& languageTag,
    /* [in] */ const String& description,
    /* [in] */ ITranslator* dt)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->RegisterBusObject(objPath.string(), busObj,
            busInterfaces, secure, languageTag, description, dt);
    if (status != ER_OK) {
        Logger::E(TAG, "RegisterBusObject(): Exception");
    }
    return status;
}

ECode CBusAttachment::EmitChangedSignal(
    /* [in] */ IBusObject* busObject,
    /* [in] */ const String& ifcName,
    /* [in] */ const String& propName,
    /* [in] */ IInterface* val,
    /* [in] */ Int32 sessionId)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->EmitChangedSignal(busObject,
        ifcName.string(), propName.string(), val, sessionId);
    if (status != ER_OK) {
        Logger::E(TAG, "RegisterBusObject(): Exception");
    }
    return status;
}

ECode CBusAttachment::RequestName(
    /* [in] */ const String& name,
    /* [in] */ Int32 flags)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->RequestName(name.string(), flags);
    if (status != ER_OK) {
        Logger::E(TAG, "RequestName(): Exception(0x%08x)", status);
    }

    return status;
}

ECode CBusAttachment::ReleaseName(
    /* [in] */ const String& name)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->ReleaseName(name.string());
    if (status != ER_OK) {
        Logger::E(TAG, "ReleaseName(): Exception(0x%08x)", status);
    }

    return status;
}

ECode CBusAttachment::AddMatch(
    /* [in] */ const String& rule)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->AddMatch(rule.string());
    if (status != ER_OK) {
        Logger::E(TAG, "AddMatch(): Exception(0x%08x)", status);
    }

    return status;
}

ECode CBusAttachment::RemoveMatch(
    /* [in] */ const String& rule)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->RemoveMatch(rule.string());
    if (status != ER_OK) {
        Logger::E(TAG, "RemoveMatch(): Exception(0x%08x)", status);
    }

    return status;
}

ECode CBusAttachment::AdvertiseName(
    /* [in] */ const String& name,
    /* [in] */ Int16 transports)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "AdvertiseName(): NULL bus pointer");
        return ER_FAIL;
    }

    QStatus status = busPtr->AdvertiseName(name.string(), transports);
    if (status != ER_OK) {
        Logger::E(TAG, "AdvertiseName(): Exception(0x%08x)", status);
    }

    return status;
}

ECode CBusAttachment::CancelAdvertiseName(
    /* [in] */ const String& name,
    /* [in] */ Int16 transports)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->CancelAdvertiseName(name.string(), transports);
    if (status != ER_OK) {
        Logger::E(TAG, "CancelAdvertiseName(): Exception(0x%08x)", status);
    }

    return status;
}

ECode CBusAttachment::FindAdvertisedName(
    /* [in] */ const String& namePrefix)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->FindAdvertisedName(namePrefix.string());
    if (status != ER_OK) {
        Logger::E(TAG, "FindAdvertisedName(): Exception(0x%08x)", status);
    }

    return status;
}

ECode CBusAttachment::FindAdvertisedNameByTransport(
    /* [in] */ const String& namePrefix,
    /* [in] */ Int16 transports)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->FindAdvertisedNameByTransport(
        namePrefix.string(), transports);
    if (status != ER_OK) {
        Logger::E(TAG, "FindAdvertisedNameByTransport(): Exception(0x%08x)", status);
    }

    return status;
}

ECode CBusAttachment::CancelFindAdvertisedName(
    /* [in] */ const String& namePrefix)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->CancelFindAdvertisedName(namePrefix.string());
    if (status != ER_OK) {
        Logger::E(TAG, "CancelFindAdvertisedName(): Exception(0x%08x)", status);
    }

    return status;
}

ECode CBusAttachment::CancelFindAdvertisedNameByTransport(
    /* [in] */ const String& namePrefix,
    /* [in] */ Int16 transports)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->CancelFindAdvertisedNameByTransport(
        namePrefix.string(), transports);
    if (status != ER_OK) {
        Logger::E(TAG, "CancelFindAdvertisedNameByTransport(): Exception(0x%08x)", status);
    }

    return status;
}

ECode CBusAttachment::BindSessionPort(
    /* [in] */ IMutableInteger16Value* _sessionPort,
    /* [in] */ ISessionOpts* opts,
    /* [in] */ ISessionPortListener* _listener)
{
    ajn::SessionPort sessionPort;
    Int16 value;
    _sessionPort->GetValue(&value);
    sessionPort = value;

    ajn::SessionOpts sessionOpts;
    Byte traffic;
    opts->GetTraffic(&traffic);
    sessionOpts.traffic = static_cast<ajn::SessionOpts::TrafficType>(traffic);

    Boolean isMultipoint;
    opts->IsMultipoint(&isMultipoint);
    sessionOpts.isMultipoint = isMultipoint;

    Byte proximity;
    opts->GetProximity(&proximity);
    sessionOpts.proximity = proximity;

    Int16 transports;
    opts->GetTransports(&transports);
    sessionOpts.transports = transports;

    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "BindSessionPort(): Exception or NULL bus pointer");
        return NOERROR;
    }

    NativeSessionPortListener* listener = reinterpret_cast<NativeSessionPortListener*>(((SessionPortListener*)_listener)->mHandle);
    assert(listener);

    QStatus status = busPtr->BindSessionPort(sessionPort, sessionOpts, *listener);

    if (status == ER_OK) {
        AutoLock lock(busPtr->mBaCommonLock);

        busPtr->mSessionPortListenerMap[sessionPort] = _listener;
    }
    else {
        Logger::E(TAG, "BindSessionPort(): Exception(0x%08x)", status);
        return status;
    }

    _sessionPort->SetValue(sessionPort);
    return NOERROR;
}

ECode CBusAttachment::SetDescriptionTranslator(
    /* [in] */ ITranslator* translator)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::UnbindSessionPort(
    /* [in] */ Int16 sessionPort)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->UnbindSessionPort(sessionPort);
    if (status == ER_OK) {
        AutoLock lock(busPtr->mBaCommonLock);
        busPtr->mSessionPortListenerMap[sessionPort] = NULL;
    }
    return status;
}

ECode CBusAttachment::JoinSession(
    /* [in] */ const String& sessionHost,
    /* [in] */ Int16 sessionPort,
    /* [in] */ IMutableInteger32Value* _sessionId,
    /* [in] */ ISessionOpts* opts,
    /* [in] */ ISessionListener* _listener)
{
    ajn::SessionOpts sessionOpts;
    Byte traffic;
    opts->GetTraffic(&traffic);
    sessionOpts.traffic = static_cast<ajn::SessionOpts::TrafficType>(traffic);

    Boolean isMultipoint;
    opts->IsMultipoint(&isMultipoint);
    sessionOpts.isMultipoint = isMultipoint;

    Byte proximity;
    opts->GetProximity(&proximity);
    sessionOpts.proximity = proximity;

    Int16 transports;
    opts->GetTransports(&transports);
    sessionOpts.transports = transports;

    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "JoinSession(): Exception or NULL bus pointer");
        return NOERROR;
    }

    NativeSessionListener* listener = reinterpret_cast<NativeSessionListener*>(((SessionListener*)_listener)->mHandle);
    assert(listener);

    ajn::SessionId sessionId = 0;

    QStatus status = busPtr->JoinSession(sessionHost.string(), sessionPort, listener, sessionId, sessionOpts);

    if (status == ER_OK) {
        AutoLock lock(busPtr->mBaCommonLock);
        busPtr->mSessionListenerMap[sessionId].mListener = _listener;
    }
    else {
        return status;
    }

    _sessionId->SetValue(sessionId);

    opts->SetTraffic(sessionOpts.traffic);
    opts->SetMultipoint(sessionOpts.isMultipoint);
    opts->SetProximity(sessionOpts.proximity);
    opts->SetTransports(sessionOpts.transports);

    return status;
}

ECode CBusAttachment::JoinSession(
    /* [in] */ const String& sessionHost,
    /* [in] */ Int16 sessionPort,
    /* [in] */ ISessionOpts* opts,
    /* [in] */ ISessionListener* listener,
    /* [in] */ IOnJoinSessionListener* onJoinSession,
    /* [in] */ IInterface* context)
{
    assert(0 && "TODO");
    return NOERROR;
}

static ECode LeaveGenericSession(
    /* [in] */ Int64 handle,
    /* [in] */ Int32 sessionId,
    /* [in] */ BusAttachmentSessionListenerIndex index)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(handle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    /*
     * Make the AllJoyn call.
     */
    QStatus status = ER_OK;
    switch (index) {
    case BA_HSL:
        status = busPtr->LeaveHostedSession(sessionId);
        if (ER_OK == status) {
            busPtr->mSessionListenerMap[sessionId].mHostedListener = NULL;
        }
        break;

    case BA_JSL:
        status = busPtr->LeaveJoinedSession(sessionId);
        if (ER_OK == status) {
            busPtr->mSessionListenerMap[sessionId].mJoinedListener = NULL;
        }
        break;

    case BA_SL:
        status = busPtr->LeaveSession(sessionId);
        if (ER_OK == status) {
            busPtr->mSessionListenerMap[sessionId].mListener = NULL;
        }
        break;

    default:
        Logger::E(TAG, "Exception unknown BusAttachmentSessionListenerIndex %d", index);
        assert(0);
        return ER_FAIL;
    }

    if (status != ER_OK) {
        Logger::I(TAG, "Error: LeaveGenericSession");
    }

    return status;
}

ECode CBusAttachment::LeaveSession(
    /* [in] */ Int32 sessionId)
{
    return LeaveGenericSession(mHandle, sessionId, BA_SL);
}

ECode CBusAttachment::LeaveHostedSession(
    /* [in] */ Int32 sessionId)
{
    return LeaveGenericSession(mHandle, sessionId, BA_HSL);
}

ECode CBusAttachment::LeaveJoinedSession(
    /* [in] */ Int32 sessionId)
{
    return LeaveGenericSession(mHandle, sessionId, BA_JSL);
}

ECode CBusAttachment::RemoveSessionMember(
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& sessionMemberName)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::SetSessionListener(
    /* [in] */ Int32 sessionId,
    /* [in] */ ISessionListener* listener)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::SetJoinedSessionListener(
    /* [in] */ Int32 sessionId,
    /* [in] */ ISessionListener* listener)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::SetHostedSessionListener(
    /* [in] */ Int32 sessionId,
    /* [in] */ ISessionListener* listener)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::GetSessionFd(
    /* [in] */ Int32 sessionId,
    /* [in] */ IMutableInteger32Value* sockFd)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    qcc::SocketFd sockfd = -1; //qcc::INVALID_SOCKET_FD;

    QStatus status = busPtr->GetSessionFd(sessionId, sockfd);

    if (status != ER_OK) {
        Logger::E(TAG, "GetSessionFd(): fails");
        return status;
    }

    /*
     * Store the sockFd in its corresponding out parameter.
     */
    return sockFd->SetValue(sockfd);
}

ECode CBusAttachment::SetLinkTimeout(
    /* [in] */ Int32 sessionId,
    /* [in] */ IMutableInteger32Value* linkTimeout)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    Int32 timeout;
    linkTimeout->GetValue(&timeout);
    uint32_t t = timeout;
    QStatus status = busPtr->SetLinkTimeout(sessionId, t);
    if (status == ER_OK) {
        timeout  = t;
        return linkTimeout->SetValue(timeout);
    }
    else {
        Logger::E(TAG, "SetLinkTimeout(): fails");
    }
    return status;
}

ECode CBusAttachment::GetPeerGUID(
    /* [in] */ const String& name,
    /* [in] */ IMutableStringValue* guid)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }


    qcc::String guidstr;
    QStatus status = busPtr->GetPeerGUID(name.string(), guidstr);
    if (status == ER_OK) {
        String str(guidstr.c_str());
        return guid->SetValue(str);
    }
    else {
        Logger::E(TAG, "GetPeerGUID(): fails");
    }
    return status;
}

ECode CBusAttachment::Ping(
    /* [in] */ const String& name,
    /* [in] */ Int32 timeout)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->Ping(name.string(), timeout);
    if (status != ER_OK) {
        Logger::E(TAG, "GetPeerGUID(): fails");
    }
    return status;
}

ECode CBusAttachment::PingAsync(
    /* [in] */ const String& name,
    /* [in] */ Int32 timeout,
    /* [in] */ IOnPingListener* onPing,
    /* [in] */ IInterface* context)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    assert(0 && "TODO");
    return busPtr->PingAsync(name.string(), timeout, 0, 0);
}

ECode CBusAttachment::Ping(
    /* [in] */ const String& name,
    /* [in] */ Int32 timeout,
    /* [in] */ IOnPingListener* onPing,
    /* [in] */ IInterface* context)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    assert(0 && "TODO");
    return busPtr->Ping(name.string(), timeout);
}

ECode CBusAttachment::SetDaemonDebug(
    /* [in] */ const String& moduleName,
    /* [in] */ Int32 level)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::SetLogLevels(
    /* [in] */ const String& logEnv)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::SetDebugLevel(
    /* [in] */ const String& moduleName,
    /* [in] */ Int32 level)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::UseOSLogging(
    /* [in] */ Boolean useOSLog)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::RegisterAboutListener(
    /* [in] */ IAboutListener* listener)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::UnregisterAboutListener(
    /* [in] */ IAboutListener* listener)
{
    assert(0 && "TODO");
    return NOERROR;
}

// @BusSignalHandler(iface = "org.alljoyn.About", signal = "Announce")
// public void announce(short version, short port, AboutObjectDescription[] objectDescriptions, Map<String, Variant> aboutData);

// public Status setAnnounceFlag(BusObject busObject, Class<?> intf, boolean isAnnounced);

ECode CBusAttachment::SetAnnounceFlag(
    /* [in] */ IBusObject* busObject,
    /* [in] */ const String& ifaceName,
    /* [in] */ Boolean isAnnounced)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->SetAnnounceFlag(busObject, ifaceName.string(), isAnnounced);
    if (status != ER_OK) {
        Logger::E(TAG, "SetAnnounceFlag(): Exception");
        return status;
    }

    return status;
}

ECode CBusAttachment::WhoImplements(
    /* [in] */ ArrayOf<String>* interfaces)
{
    QStatus status = ER_OK;

    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    int len = (interfaces != NULL) ? interfaces->GetLength() : 0;
    if (0 == len) {
        // both null and size zero interfaces are used the same.
        status = busPtr->WhoImplements(NULL, 0);
    }
    else {
        const char** rawIntfString = new const char*[len];
        memset(rawIntfString, 0, len * sizeof(const char*));

        for (int i = 0; i < len; ++i) {
            String str = (*interfaces)[i];
            if (NULL == str) {
                Logger::E(TAG, "BusAttachment::WhoImplements(): Exception");
                status = ER_BAD_ARG_1;
                goto cleanup;
            }

            rawIntfString[i] = str.string();
            if (NULL == rawIntfString[i]) {
                status = ER_BAD_ARG_1;
                goto cleanup;
            }
        }
        status = busPtr->WhoImplements(rawIntfString, len);

    cleanup:
        delete [] rawIntfString;
        rawIntfString = NULL;
    }
    return status;
}

ECode CBusAttachment::CancelWhoImplements(
    /* [in] */ ArrayOf<String>* interfaces)
{
    QStatus status = ER_OK;

    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    int len = (interfaces != NULL) ? interfaces->GetLength() : 0;
    if (0 == len) {
        // both null and size zero interfaces are used the same.
        status = busPtr->CancelWhoImplements(NULL, 0);
    }
    else {
        const char** rawIntfString = new const char*[len];
        memset(rawIntfString, 0, len * sizeof(const char*));

        for (int i = 0; i < len; ++i) {
            String str = (*interfaces)[i];
            if (NULL == str) {
                Logger::E(TAG, "BusAttachment_whoImplements(): Exception");
                status = ER_BAD_ARG_1;
                goto cleanup;
            }

            rawIntfString[i] = str.string();
            if (NULL == rawIntfString[i]) {
                status = ER_BAD_ARG_1;
                goto cleanup;
            }
        }

        status = busPtr->CancelWhoImplements(rawIntfString, len);

    cleanup:
        delete [] rawIntfString;
        rawIntfString = NULL;
    }

    return status;
}

ECode CBusAttachment::RegisterBusListener(
    /* [in] */ IBusListener* listener)
{
    if (listener == NULL) return NOERROR;

    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    {
        AutoLock lock(busPtr->mBaCommonLock);

        busPtr->mBusListeners.PushBack(listener);
    }

    NativeBusListener* ntListener = reinterpret_cast<NativeBusListener*>(((BusListener*)listener)->mHandle);

    assert(ntListener);
    ntListener->Setup(this);

    busPtr->RegisterBusListener(*ntListener);
    return NOERROR;
}

ECode CBusAttachment::UnregisterBusListener(
    /* [in] */ IBusListener* listener)
{
    if (listener == NULL) return NOERROR;

    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    NativeBusListener* ntListener = reinterpret_cast<NativeBusListener*>(((BusListener*)listener)->mHandle);
    assert(ntListener);
    busPtr->UnregisterBusListener(*ntListener);

    {
        AutoLock lock(busPtr->mBaCommonLock);
        AutoPtr<IBusListener> temp(listener);
        List< AutoPtr<IBusListener> >::Iterator it = Find(
            busPtr->mBusListeners.Begin(), busPtr->mBusListeners.End(), temp);
        if (it != busPtr->mBusListeners.End()) {
            busPtr->mBusListeners.Erase(it);
        }
    }
    return NOERROR;
}

ECode CBusAttachment::ReleaseResources()
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::Connect()
{
    /*
     * os.name is one of the standard system properties will be used to
     * decide the value of org.alljoyn.bus.address.
     */
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    system->GetProperty(String("org.alljoyn.bus.address"), String("unix:abstract=alljoyn"), &mAddress);
    if (!mAddress.IsNull()) {
        ECode ec = Connect(mAddress, mKeyStoreListener, mAuthMechanisms, mBusAuthListener, mKeyStoreFileName, mIsShared);
        if (ec == (ECode)E_STATUS_OK) {
            /* Add this BusAttachment to the busAttachmentSet */
            {
                AutoLock lock(sBusAttachmentSet);
                /* Register a shutdown hook if it is not already registered. */
                if (sShutdownHookRegistered == FALSE) {
                    // TODO:
                    // Runtime.getRuntime().addShutdownHook(new ShutdownHookThread());
                    sShutdownHookRegistered = TRUE;
                }
                AutoPtr<IWeakReference> wr;
                GetWeakReference((IWeakReference**)&wr);
                sBusAttachmentSet->Add(wr);
            }
            mIsConnected = TRUE;
        }
        return ec;
    }
    else {
        return E_STATUS_INVALID_CONNECT_ARGS;
    }
}

ECode CBusAttachment::IsConnected(
    /* [out] */ Boolean* connected)
{
    VALIDATE_NOT_NULL(connected)
    *connected = FALSE;

    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    *connected = busPtr->IsConnected();
    return NOERROR;
}

ECode CBusAttachment::Disconnect()
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    busPtr->Disconnect();
    return NOERROR;
}

ECode CBusAttachment::RegisterBusObject(
    /* [in] */ IBusObject* busObj,
    /* [in] */ const String& objPath)
{
    return RegisterBusObject(busObj, objPath, FALSE, String(NULL), String(NULL), NULL);
}

ECode CBusAttachment::RegisterBusObject(
    /* [in] */ IBusObject* busObj,
    /* [in] */ const String& objPath,
    /* [in] */ Boolean secure)
{
    return RegisterBusObject(busObj, objPath, secure, String(NULL), String(NULL), NULL);
}

ECode CBusAttachment::RegisterBusObject(
    /* [in] */ IBusObject* busObj,
    /* [in] */ const String& objPath,
    /* [in] */ Boolean secure,
    /* [in] */ const String& languageTag,
    /* [in] */ const String& description)
{
    return RegisterBusObject(busObj, objPath, secure, languageTag, description, NULL);
}

ECode CBusAttachment::RegisterBusObject(
    /* [in] */ IBusObject* busObj,
    /* [in] */ const String& objPath,
    /* [in] */ Boolean secure,
    /* [in] */ const String& languageTag,
    /* [in] */ const String& description,
    /* [in] */ ITranslator* dt)
{
    // try {
    AutoPtr<IList> descs;
    CArrayList::New((IList**)&descs);
    AutoPtr<IClassInfo> clsInfo;
    CObject::ReflectClassInfo(busObj, (IClassInfo**)&clsInfo);
    Int32 count;
    clsInfo->GetInterfaceCount(&count);
    AutoPtr< ArrayOf<IInterfaceInfo*> > busInterfaces = ArrayOf<IInterfaceInfo*>::Alloc(count);
    clsInfo->GetAllInterfaceInfos(busInterfaces);
    ECode ec = InterfaceDescription::Create(this, busInterfaces, descs);
    if (ec != E_STATUS_OK) {
        return ec;
    }
    AutoPtr< ArrayOf<IInterfaceDescription*> > busInterfaceArray;
    descs->ToArray((ArrayOf<IInterface*>**)&busInterfaceArray);
    return RegisterBusObject(objPath, busObj, busInterfaceArray, secure,
            languageTag, description, dt);
    // } catch (AnnotationBusException ex) {
    //     BusException.log(ex);
    //     return Status.BAD_ANNOTATION;
    // }
}

ECode CBusAttachment::IsBusObjectSecure(
    /* [in] */ IBusObject* busObject,
    /* [out] */ Boolean* secure)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::UnregisterBusObject(
    /* [in] */ IBusObject* busObject)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    busPtr->UnregisterBusObject(busObject);
    return NOERROR;
}

ECode CBusAttachment::GetProxyBusObject(
    /* [in] */ const String& busName,
    /* [in] */ const String& objPath,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces,
    /* [out] */ IProxyBusObject** proxy)
{
    assert(0 && "TODO");
    return NOERROR;
}

// public ProxyBusObject getProxyBusObject(String busName,
//         String objPath,
//         int sessionId,
//         Class<?>[] busInterfaces,
//         boolean secure);

// public DBusProxyObj getDBusProxyObj();

ECode CBusAttachment::GetUniqueName(
    /* [out] */ String* uniqueName)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::GetGlobalGUIDString(
    /* [out] */ String* guid)
{
    VALIDATE_NOT_NULL(guid)
    *guid = NULL;

    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    *guid = String(busPtr->GetGlobalGUIDString().c_str());
    return NOERROR;
}

ECode CBusAttachment::RegisterSignalHandler(
    /* [in] */ const String& ifaceName,
    /* [in] */ const String& signalName,
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* handlerMethod)
{
    return RegisterSignalHandler(ifaceName, signalName, obj, handlerMethod, String(NULL));
}

ECode CBusAttachment::RegisterSignalHandler(
    /* [in] */ const String& ifaceName,
    /* [in] */ const String& signalName,
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* handlerMethod,
    /* [in] */ const String& ancillary)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    assert(0 && "TODO");
    // return busPtr->RegisterSignalHandler(ifaceName.string(), signalName.string(),
    //     obj, handlerMethod, ancillary.string());
    return NOERROR;
}

ECode CBusAttachment::RegisterSignalHandlerWithRule(
    /* [in] */ const String& ifaceName,
    /* [in] */ const String& signalName,
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* handlerMethod,
    /* [in] */ const String& matchRule)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::RegisterSignalHandlers(
    /* [in] */ IInterface* obj)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::UnregisterSignalHandler(
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* handlerMethod)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    busPtr->UnregisterSignalHandler(obj, handlerMethod);
    return NOERROR;
}

ECode CBusAttachment::UnregisterSignalHandlers(
    /* [in] */ IInterface* obj)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::RegisterKeyStoreListener(
    /* [in] */ IKeyStoreListener* listener)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::ClearKeyStore()
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    busPtr->ClearKeyStore();
    return NOERROR;
}

ECode CBusAttachment::ClearKeys(
    /* [in] */ const String& guid)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->ClearKeys(guid.string());
    if (status != ER_OK) {
        Logger::E(TAG, "ClearKeys(): Exception");
        return status;
    }

    return NOERROR;
}

ECode CBusAttachment::SetKeyExpiration(
    /* [in] */ const String& guid,
    /* [in] */ Int32 timeout)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->SetKeyExpiration(guid.string(), timeout);

    if (status != ER_OK) {
        Logger::E(TAG, "SetKeyExpiration(): Exception");
        return status;
    }

    return NOERROR;
}

ECode CBusAttachment::GetKeyExpiration(
    /* [in] */ const String& guid,
    /* [in] */ IMutableInteger32Value* timeout)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    /*
     * Make the AllJoyn call.
     */
    uint32_t t = 0;
    QStatus status = busPtr->GetKeyExpiration(guid.string(), t);

    if (status != ER_OK) {
        Logger::E(TAG, "GetKeyExpiration(): Exception");
        return status;
    }

    return timeout->SetValue(t);
}

ECode CBusAttachment::ReloadKeyStore()
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->ReloadKeyStore();

    if (status != ER_OK) {
        Logger::E(TAG, "ReloadKeyStore(): Exception");
    }
    return status;
}

ECode CBusAttachment::RegisterAuthListener(
    /* [in] */ const String& authMechanisms,
    /* [in] */ IAuthListener* listener,
    /* [in] */ const String& keyStoreFileName,
    /* [in] */ Boolean isShared)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::RegisterAuthListener(
    /* [in] */ const String& authMechanisms,
    /* [in] */ IAuthListener* listener,
    /* [in] */ const String& keyStoreFileName)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::RegisterAuthListener(
    /* [in] */ const String& authMechanisms,
    /* [in] */ IAuthListener* listener)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::RegisterSecurityViolationListener(
    /* [in] */ ISecurityViolationListener* listener)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::GetMessageContext(
    /* [out] */ IMessageContext** context)
{
    VALIDATE_NOT_NULL(context)
    *context = NULL;

    ajn::Message msg = NativeMessageContext::GetMessage();

    String jobjectPath(msg->GetObjectPath());
    if (NULL == jobjectPath) {
        return NOERROR;
    }

    String jinterfaceName(msg->GetInterface());
    if (NULL == jinterfaceName) {
        return NOERROR;
    }

    String jmemberName(msg->GetMemberName());
    if (NULL == jmemberName) {
        return NOERROR;
    }

    String jdestination(msg->GetDestination());
    if (NULL == jdestination) {
        return NOERROR;
    }

    String jsender(msg->GetSender());
    if (NULL == jsender) {
        return NOERROR;
    }

    String jsignature(msg->GetSignature());
    if (NULL == jsignature) {
        return NOERROR;
    }

    String jauthMechanism(msg->GetAuthMechanism().c_str());
    if (NULL == jauthMechanism) {
        return NOERROR;
    }

    ajn::SessionId sessionId = msg->GetSessionId();
    uint32_t serial = msg->GetCallSerial();

    assert(0 && "TODO");
    // return CMessageContext::New(mid, msg->IsUnreliable(), jobjectPath,
    //     jinterfaceName, jmemberName, jdestination,
    //     jsender, sessionId, jsignature, jauthMechanism,
    //     serial, context);
    return NOERROR;
}

ECode CBusAttachment::EnableConcurrentCallbacks()
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    busPtr->EnableConcurrentCallbacks();
    return NOERROR;
}


} // namespace Bus
} // namespace Alljoyn
} // namespace Org
