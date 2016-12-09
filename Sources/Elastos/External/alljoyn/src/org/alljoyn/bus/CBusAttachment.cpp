
#include "org/alljoyn/bus/BusListener.h"
#include "org/alljoyn/bus/CBusAttachment.h"
#include "org/alljoyn/bus/AuthListener.h"
#include "org/alljoyn/bus/InterfaceDescription.h"
#include "org/alljoyn/bus/NativeApi.h"
#include "org/alljoyn/bus/NativeBusAttachment.h"
#include "org/alljoyn/bus/NativeBusListener.h"
#include "org/alljoyn/bus/NativeMessageContext.h"
#include "org/alljoyn/bus/NativeSessionListener.h"
#include "org/alljoyn/bus/NativeSessionPortListener.h"
#include "org/alljoyn/bus/NativePendingAsyncJoin.h"
#include "org/alljoyn/bus/NativeMessageContext.h"
#include "org/alljoyn/bus/NativeTranslator.h"
#include "org/alljoyn/bus/NativeOnJoinSessionListener.h"
#include "org/alljoyn/bus/NativeOnPingListener.h"
#include "org/alljoyn/bus/Translator.h"
#include "org/alljoyn/bus/ProxyBusObject.h"
#include "org/alljoyn/bus/SessionListener.h"
#include "org/alljoyn/bus/SessionPortListener.h"
#include "org/alljoyn/bus/OnJoinSessionListener.h"
#include "org/alljoyn/bus/OnPingListener.h"
#include "org/alljoyn/bus/MessageContext.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Org::Alljoyn::Bus::Ifaces::EIID_IDBusProxyObj;
using Elastos::Core::AutoLock;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::IO::IBuffer;
using Elastos::IO::IByteBuffer;
using Elastos::IO::CCharBufferHelper;
using Elastos::IO::ICharBufferHelper;
using Elastos::IO::ICharBuffer;
using Elastos::IO::Charset::CCharsetHelper;
using Elastos::IO::Charset::ICharsetHelper;
using Elastos::IO::Charset::ICharset;
using Elastos::IO::Charset::ICharsetEncoder;
using Elastos::Utility::IIterator;
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
    mAuthListener = authListener;
    return NOERROR;
}

ECode CBusAttachment::AuthListenerInternal::AuthListenerSet(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mAuthListener != NULL;
    return NOERROR;
}

ECode CBusAttachment::AuthListenerInternal::SetSecurityViolationListener(
    /* [in] */ ISecurityViolationListener* violationListener)
{
    mViolationListener = violationListener;
    return NOERROR;
}

ECode CBusAttachment::AuthListenerInternal::RequestCredentials(
    /* [in] */ const String& authMechanism,
    /* [in] */ const String& authPeer,
    /* [in] */ Int32 authCount,
    /* [in] */ const String& userName,
    /* [in] */ Int32 credMask,
    /* [out] */ ICredentials** retCredentials)
{
    VALIDATE_NOT_NULL(retCredentials)
    *retCredentials = NULL;
    if (mAuthListener == NULL) {
        // throw new BusException("No registered application AuthListener");
        Logger::E(TAG, "No registered application AuthListener");
        assert(0);
        return NOERROR;
    }

    AutoPtr<Credentials> credentials = new Credentials();
    AutoPtr<IList> requests;
    CArrayList::New((IList**)&requests);
    AutoPtr<IAuthRequest> request;
    if ((credMask & PASSWORD) == PASSWORD) {
        Boolean isNew = (credMask & NEW_PASSWORD) == NEW_PASSWORD;
        Boolean isOneTime = (credMask & ONE_TIME_PWD) == ONE_TIME_PWD;
        request = new PasswordRequest(credentials, isNew, isOneTime);
        requests->Add(request);
    }
    if ((credMask & USER_NAME) == USER_NAME) {
        request = new UserNameRequest(credentials);
        requests->Add(request);
    }
    if ((credMask & CERT_CHAIN) == CERT_CHAIN) {
        request = new CertificateRequest(credentials);
        requests->Add(request);
    }
    if ((credMask & PRIVATE_KEY) == PRIVATE_KEY) {
        request = new PrivateKeyRequest(credentials);
        requests->Add(request);
    }
    if ((credMask & LOGON_ENTRY) == LOGON_ENTRY) {
        request = new LogonEntryRequest(credentials);
        requests->Add(request);
    }
    /*
     * Always add this as it doesn't show up in credMask, but can be set by the application.
     */
    request = new ExpirationRequest(credentials);
    requests->Add(request);

    AutoPtr<ArrayOf<IAuthRequest*> > array;
    requests->ToArray((ArrayOf<IInterface*>**)&array);
    Boolean res;
    mAuthListener->Requested(authMechanism, authPeer, authCount, userName, array, &res);
    if (res) {
        *retCredentials = credentials;
        REFCOUNT_ADD(*retCredentials)
    }
    return NOERROR;
}

ECode CBusAttachment::AuthListenerInternal::VerifyCredentials(
    /* [in] */ const String& authMechanism,
    /* [in] */ const String& peerName,
    /* [in] */ const String& userName,
    /* [in] */ const String& cert,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mAuthListener == NULL) {
        // throw new BusException("No registered application AuthListener");
        Logger::E(TAG, "No registered application AuthListener");
        assert(0);
        return NOERROR;
    }
    /*
     * authCount is set to 0 here since it can't be cached from
     * requestCredentials, and it's assumed that the application will
     * not immediately reject a request with an authCount of 0.
     */
    AutoPtr<ArrayOf<IAuthRequest*> > array = ArrayOf<IAuthRequest*>::Alloc(1);
    AutoPtr<IAuthRequest> request = new VerifyRequest(cert);
    array->Set(0, request);
    return mAuthListener->Requested(authMechanism, peerName, 0,
        userName == NULL ? String("") : userName, array, result);
}

ECode CBusAttachment::AuthListenerInternal::SecurityViolation(
    /* [in] */ ECode status)
{
    if (mViolationListener != NULL) {
        mViolationListener->Violated(status);
    }
    return NOERROR;
}

ECode CBusAttachment::AuthListenerInternal::AuthenticationComplete(
    /* [in] */ const String& authMechanism,
    /* [in] */ const String& peerName,
    /* [in] */ Boolean success)
{
    if (mAuthListener != NULL) {
        mAuthListener->Completed(authMechanism, peerName, success);
    }
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

CBusAttachment::~CBusAttachment()
{
    if (mIsConnected == TRUE) {
        Disconnect();
    }

    mDbusbo = NULL;
    mDbus = NULL;
    Destroy();
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
    AutoPtr<IInterfaceInfo> dbusProxyObj;
    GetModuleClassLoader()->LoadInterface(String("Org.Alljoyn.Bus.Ifaces.IDBusProxyObj"), (IInterfaceInfo**)&dbusProxyObj);
    busInterfaces->Set(0, dbusProxyObj);
    mDbusbo = new ProxyBusObject();
    mDbusbo->constructor(this, String("org.freedesktop.DBus"), String("/org/freedesktop/DBus"), SESSION_ID_ANY,
            busInterfaces);
    mDbusbo->GetInterface(dbusProxyObj, (IInterface**)&mDbus);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
        return status;
    }

    _sessionPort->SetValue(sessionPort);
    return NOERROR;
}

ECode CBusAttachment::SetDescriptionTranslator(
    /* [in] */ ITranslator* _translator)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    NativeTranslator* translator = NULL;
    if (_translator) {
        translator = reinterpret_cast<NativeTranslator*>(((Translator*)_translator)->mHandle);
        assert(translator);

        AutoLock lock(busPtr->mBaCommonLock);
        busPtr->mTranslators.PushBack(_translator);
    }

    busPtr->SetDescriptionTranslator(translator);
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
    else {
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

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

    NativeSessionListener* listener = reinterpret_cast<NativeSessionListener*>(((SessionListener*)_listener)->mHandle);
    assert(listener);

    ajn::SessionId sessionId = 0;

    QStatus status = busPtr->JoinSession(sessionHost.string(), sessionPort, listener, sessionId, sessionOpts);

    if (status == ER_OK) {
        AutoLock lock(busPtr->mBaCommonLock);
        busPtr->mSessionListenerMap[sessionId].mListener = _listener;
    }
    else {
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
    /* [in] */ ISessionListener* _listener,
    /* [in] */ IOnJoinSessionListener* onJoinSession,
    /* [in] */ IInterface* context)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

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

    AutoPtr<NativePendingAsyncJoin> paj = new NativePendingAsyncJoin(_listener, onJoinSession, context);

    NativeSessionListener* listener = reinterpret_cast<NativeSessionListener*>(((SessionListener*)_listener)->mHandle);
    assert(listener);

    QStatus status = ER_OK;

    NativeOnJoinSessionListener* callback = reinterpret_cast<NativeOnJoinSessionListener*>(((OnJoinSessionListener*)_listener)->mHandle);
    assert(callback);
    callback->Setup(busPtr);

    status = busPtr->JoinSessionAsync(
        sessionHost.string(), sessionPort, listener, sessionOpts, callback, paj);

    if (status == ER_OK) {
        AutoLock lock(busPtr->mBaCommonLock);
        busPtr->mPendingAsyncJoins.PushBack(paj);
    }
    else {
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
        return status;
    }

    return status;
}

ECode CBusAttachment::LeaveGenericSession(
    /* [in] */ Int32 sessionId,
    /* [in] */ BusAttachmentSessionListenerIndex index)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = ER_OK;

    /*
     * Make the AllJoyn call.
     */
    HashMap<ajn::SessionId, NativeBusAttachment::BusAttachmentSessionListeners>::Iterator it;
    it = busPtr->mSessionListenerMap.Find(sessionId);
    switch (index) {
    case BA_HSL:
        status = busPtr->LeaveHostedSession(sessionId);
        if (ER_OK == status) {
            AutoLock lock(busPtr->mBaCommonLock);
            if (it != busPtr->mSessionListenerMap.End()) {
                it->mSecond.mHostedListener = NULL;
            }
        }
        break;

    case BA_JSL:
        status = busPtr->LeaveJoinedSession(sessionId);
        if (ER_OK == status) {
            AutoLock lock(busPtr->mBaCommonLock);
            if (it != busPtr->mSessionListenerMap.End()) {
                it->mSecond.mJoinedListener = NULL;
            }
        }
        break;

    case BA_SL:
        status = busPtr->LeaveSession(sessionId);
        if (ER_OK == status) {
            AutoLock lock(busPtr->mBaCommonLock);
            if (it != busPtr->mSessionListenerMap.End()) {
                it->mSecond.mListener = NULL;
            }
        }
        break;

    default:
        Logger::E(TAG, "Exception unknown BusAttachmentSessionListenerIndex %d", index);
        assert(0);
        return ER_FAIL;
    }

    if (status != ER_OK) {
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
    }

    return status;
}

ECode CBusAttachment::LeaveSession(
    /* [in] */ Int32 sessionId)
{
    return LeaveGenericSession(sessionId, BA_SL);
}

ECode CBusAttachment::LeaveHostedSession(
    /* [in] */ Int32 sessionId)
{
    return LeaveGenericSession(sessionId, BA_HSL);
}

ECode CBusAttachment::LeaveJoinedSession(
    /* [in] */ Int32 sessionId)
{
    return LeaveGenericSession(sessionId, BA_JSL);
}

ECode CBusAttachment::RemoveSessionMember(
    /* [in] */ Int32 sessionId,
    /* [in] */ const String& sessionMemberName)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->RemoveSessionMember(sessionId, sessionMemberName.string());
    if (status != ER_OK) {
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
    }
    return NOERROR;
}

ECode CBusAttachment::SetGenericSessionListener(
    /* [in] */ Int32 sessionId,
    /* [in] */ ISessionListener* _listener,
    /* [in] */ BusAttachmentSessionListenerIndex index)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    NativeSessionListener* listener = NULL;
    if (_listener) {
        listener = reinterpret_cast<NativeSessionListener*>(((SessionListener*)_listener)->mHandle);
        assert(listener);
    }

    QStatus status = ER_OK;

    /*
     * Make the AllJoyn call.
     */
    HashMap<ajn::SessionId, NativeBusAttachment::BusAttachmentSessionListeners>::Iterator it;
    it = busPtr->mSessionListenerMap.Find(sessionId);
    switch (index) {
    case BA_HSL:
        status = busPtr->SetHostedSessionListener(sessionId, listener);
        if (status == ER_OK) {
            AutoLock lock(busPtr->mBaCommonLock);
            if (it != busPtr->mSessionListenerMap.End()) {
                it->mSecond.mHostedListener = _listener;
            }
        }
        break;

    case BA_JSL:
        status = busPtr->SetJoinedSessionListener(sessionId, listener);
        if (status == ER_OK) {
            AutoLock lock(busPtr->mBaCommonLock);
            if (it != busPtr->mSessionListenerMap.End()) {
                it->mSecond.mJoinedListener = _listener;
            }
        }
        break;

    case BA_SL:
        status = busPtr->SetSessionListener(sessionId, listener);
        if (status == ER_OK) {
            AutoLock lock(busPtr->mBaCommonLock);
            if (it != busPtr->mSessionListenerMap.End()) {
                it->mSecond.mListener = _listener;
            }
        }
        break;

    default:
        Logger::E(TAG, "Exception unknown BusAttachmentSessionListenerIndex %d", index);
        assert(0);
    }

    if (status != ER_OK) {
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
    }

    return status;
}

ECode CBusAttachment::SetSessionListener(
    /* [in] */ Int32 sessionId,
    /* [in] */ ISessionListener* listener)
{
    return SetGenericSessionListener(sessionId, listener, BA_SL);
}

ECode CBusAttachment::SetJoinedSessionListener(
    /* [in] */ Int32 sessionId,
    /* [in] */ ISessionListener* listener)
{
    return SetGenericSessionListener(sessionId, listener, BA_JSL);
}

ECode CBusAttachment::SetHostedSessionListener(
    /* [in] */ Int32 sessionId,
    /* [in] */ ISessionListener* listener)
{
    return SetGenericSessionListener(sessionId, listener, BA_HSL);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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

    QStatus status = ER_OK;

    NativeOnPingListener* callback = reinterpret_cast<NativeOnPingListener*>(((OnPingListener*)onPing)->mHandle);
    assert(callback);

    callback->Setup(busPtr);

    NativePendingAsyncPing* pap = new NativePendingAsyncPing(onPing, context);

    status = busPtr->PingAsync(name.string(), timeout, callback, pap);

    if (status == ER_OK) {
        AutoLock lock(busPtr->mBaCommonLock);

        busPtr->mPendingAsyncPings.PushBack(pap);
    }
    else {
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
    }

    return status;
}

ECode CBusAttachment::Ping(
    /* [in] */ const String& name,
    /* [in] */ Int32 timeout,
    /* [in] */ IOnPingListener* onPing,
    /* [in] */ IInterface* context)
{
    return PingAsync(name, timeout, onPing, context);
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
    Boolean bval;
    mRegisteredAboutListeners->IsEmpty(&bval);
    if (bval) {
        RegisterSignalHandlers((IBusAttachment*)this);
    }
    mRegisteredAboutListeners->Add(listener);
    return NOERROR;
}

ECode CBusAttachment::UnregisterAboutListener(
    /* [in] */ IAboutListener* listener)
{
    mRegisteredAboutListeners->Remove(listener);
    Boolean bval;
    mRegisteredAboutListeners->IsEmpty(&bval);
    if (bval) {
        UnregisterSignalHandlers((IBusAttachment*)this);
    }
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
    AutoLock lock(sBusAttachmentSet);

    ReleaseWithoutRemove();

    /* Remove this bus attachment from the busAttachmentSet */
    AutoPtr<IIterator> it;
    sBusAttachmentSet->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> obj;
        it->GetNext((IInterface**)&obj);
        AutoPtr<IBusAttachment> attachment;
        IWeakReference::Probe(obj)->Resolve(EIID_IBusAttachment, (IInterface**)&attachment);
        if (attachment && Object::Equals(attachment, (IBusAttachment*)this)) {
            it->Remove();
        }
    }
    return NOERROR;
}

void CBusAttachment::ReleaseWithoutRemove()
{
    if (mIsConnected == TRUE) {
        Disconnect();
    }
    if (mDbusbo != NULL) {
        mDbusbo->ReleaseResources();
        mDbusbo = NULL;
    }
    mDbus = NULL;
    Destroy();
}

void CBusAttachment::Destroy()
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return;
    }

    busPtr->Destroy();
    mHandle = 0;
}

AutoPtr<ArrayOf<Byte> > CBusAttachment::Encode(
    /* [in] */ ArrayOf<Char32>* charArray)
{
    AutoPtr<ICharsetHelper> helper;
    CCharsetHelper::AcquireSingleton((ICharsetHelper**)&helper);
    AutoPtr<ICharset> charset;
    helper->ForName(String("UTF-8"), (ICharset**)&charset);
    AutoPtr<ICharsetEncoder> encoder;
    charset->NewEncoder((ICharsetEncoder**)&encoder);
    AutoPtr<ICharBufferHelper> cbHelper;
    CCharBufferHelper::AcquireSingleton((ICharBufferHelper**)&cbHelper);
    AutoPtr<ICharBuffer> charBuffer;
    cbHelper->Wrap(charArray, (ICharBuffer**)&charBuffer);
    AutoPtr<IByteBuffer> bb;
    if (FAILED(encoder->Encode(charBuffer, (IByteBuffer**)&bb)))
        return NULL;
    Int32 limit;
    IBuffer::Probe(bb)->GetLimit(&limit);
    AutoPtr<ArrayOf<Byte> > ba = ArrayOf<Byte>::Alloc(limit);
    bb->Get(ba);
    return ba;
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
    VALIDATE_NOT_NULL(secure)
    *secure = FALSE;

    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }
    *secure =  busPtr->IsSecureBusObject(busObject);
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
    VALIDATE_NOT_NULL(proxy)
    AutoPtr<ProxyBusObject> obj = new ProxyBusObject();
    obj->constructor(this, busName, objPath, sessionId, busInterfaces);
    *proxy = obj;
    REFCOUNT_ADD(*proxy)
    return NOERROR;
}

ECode CBusAttachment::GetProxyBusObject(
    /* [in] */ const String& busName,
    /* [in] */ const String& objPath,
    /* [in] */ Int32 sessionId,
    /* [in] */ ArrayOf<IInterfaceInfo*>* busInterfaces,
    /* [in] */ Boolean secure,
    /* [out] */ IProxyBusObject** proxy)
{
    VALIDATE_NOT_NULL(proxy);

    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::GetDBusProxyObj(
    /* [out] */ IDBusProxyObj** proxy)
{
    assert(0 && "TODO");
    return NOERROR;
}

ECode CBusAttachment::GetUniqueName(
    /* [out] */ String* uniqueName)
{
    VALIDATE_NOT_NULL(uniqueName)
    *uniqueName = NULL;

    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    *uniqueName = String(busPtr->GetUniqueName().c_str());
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
    return RegisterSignalHandler(ifaceName, signalName, obj, handlerMethod, String(""));
}

ECode CBusAttachment::RegisterSignalHandler(
    /* [in] */ const String& ifaceName,
    /* [in] */ const String& signalName,
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* handlerMethod,
    /* [in] */ const String& source)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->RegisterNativeSignalHandlerWithSrcPath(
        ifaceName.string(), signalName.string(), obj, handlerMethod, source.string());
    if (status == ER_BUS_NO_SUCH_INTERFACE) {
        assert(0 && "TODO");
        // AutoPtr<IInterfaceInfo> itfcInfo = Object:
        // InterfaceDescription desc = new InterfaceDescription();

        // Class<?> iface = Class.forName(ifaceName);
        // status = desc->Create(this, itfcInfo);
        // if (status == ER_OK) {
        //     ifaceName = InterfaceDescription->GetName(iface);
        //     try {
        //         Method signal = iface->GetMethod(signalName, handlerMethod.getParameterTypes());
        //         signalName = InterfaceDescription->GetName(signal);
        //     } catch (NoSuchMethodException ex) {
        //         // Ignore, use signalName parameter provided
        //     }
        //     status = RegisterNativeSignalHandlerWithSrcPath(ifaceName, signalName, obj, handlerMethod,
        //                                          source);
        // }

        // } catch (ClassNotFoundException ex) {
        //     BusException.log(ex);
        //     status = Status.BUS_NO_SUCH_INTERFACE;
        // } catch (AnnotationBusException ex) {
        //     BusException.log(ex);
        //     status = Status.BAD_ANNOTATION;
        // }
    }
    return status;
}

ECode CBusAttachment::RegisterSignalHandlerWithRule(
    /* [in] */ const String& ifaceName,
    /* [in] */ const String& signalName,
    /* [in] */ IInterface* obj,
    /* [in] */ IMethodInfo* handlerMethod,
    /* [in] */ const String& matchRule)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    QStatus status = busPtr->RegisterNativeSignalHandlerWithRule(
        ifaceName.string(), signalName.string(), obj, handlerMethod, matchRule.string());
    if (status == ER_BUS_NO_SUCH_INTERFACE) {
        assert(0 && "TODO");
        // try {
        //     Class<?> iface = Class.forName(ifaceName);
        //     ajn::InterfaceDescription desc = new ajn::InterfaceDescription();
        //     status = desc.create(this, iface);
        //     if (status == ER_OK) {
        //         ifaceName = InterfaceDescription.getName(iface);
        //         // try {
        //             Method signal = iface.getMethod(signalName, handlerMethod.getParameterTypes());
        //             signalName = InterfaceDescription.getName(signal);
        //         // }
        //         // catch (NoSuchMethodException ex) {
        //         //     // Ignore, use signalName parameter provided
        //         // }
        //         status = busPtr->RegisterNativeSignalHandlerWithRule(ifaceName, signalName, obj, handlerMethod, matchRule);
        //     }
        // }
        // catch (ClassNotFoundException ex) {
        //     BusException.log(ex);
        //     status = ER_BUS_NO_SUCH_INTERFACE;
        // }
        // catch (AnnotationBusException ex) {
        //     BusException.log(ex);
        //     status = ER_BAD_ANNOTATION;
        // }
    }
    return status;
}

ECode CBusAttachment::RegisterSignalHandlers(
    /* [in] */ IInterface* obj)
{
    VALIDATE_NOT_NULL(obj)

    ECode ec = NOERROR;

    Int32 count;
    AutoPtr<IClassInfo> classInfo = Object::GetClassInfo(obj);
    classInfo->GetMethodCount(&count);
    if (count > 0) {
        IMethodInfo* method;
        AutoPtr< ArrayOf<IMethodInfo *> > methodInfos = ArrayOf<IMethodInfo *>::Alloc(count);
        for (Int32 i = 0; i < count; ++count) {
            method = (*methodInfos)[i];
            AutoPtr<IAnnotationInfo> annotation;
            method->GetAnnotation(
                String("Org.Alljoyn.Bus.Annotation.BusSignalHandler"),
                (IAnnotationInfo**)&annotation);
            if (annotation != NULL) {
                String matchRule;
                annotation->GetValue(String("rule"), &matchRule);
                if (matchRule.IsNullOrEmpty()) {
                    annotation->GetValue(String("source"), &matchRule);
                }

                String ifaceName, signalName;
                annotation->GetValue(String("iface"), &ifaceName);
                annotation->GetValue(String("signal"), &signalName);

                ec = RegisterSignalHandler(ifaceName, signalName, obj, method, matchRule);
                if (FAILED(ec)) {
                    break;
                }
            }
        }
    }

    return ec;
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
    VALIDATE_NOT_NULL(obj)

    Int32 count;
    AutoPtr<IClassInfo> classInfo = Object::GetClassInfo(obj);
    classInfo->GetMethodCount(&count);
    if (count > 0) {
        IMethodInfo* method;
        AutoPtr< ArrayOf<IMethodInfo *> > methodInfos = ArrayOf<IMethodInfo *>::Alloc(count);
        for (Int32 i = 0; i < count; ++count) {
            method = (*methodInfos)[i];
            AutoPtr<IAnnotationInfo> annotation;
            method->GetAnnotation(
                String("Org.Alljoyn.Bus.Annotation.BusSignalHandler"),
                (IAnnotationInfo**)&annotation);
            if (annotation != NULL) {
                UnregisterSignalHandler(obj, method);
            }
        }
    }

    return NOERROR;
}

ECode CBusAttachment::RegisterKeyStoreListener(
    /* [in] */ IKeyStoreListener* listener)
{
    mKeyStoreListener = listener;
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
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

    timeout->SetValue(t);

    if (status != ER_OK) {
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
        return status;
    }

    return status;
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
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
    }
    return status;
}

ECode CBusAttachment::RegisterAuthListener(
    /* [in] */ const String& authMechanisms,
    /* [in] */ IAuthListener* listener,
    /* [in] */ const String& keyStoreFileName,
    /* [in] */ Boolean isShared)
{
    NativeBusAttachment* busPtr = reinterpret_cast<NativeBusAttachment*>(mHandle);
    if (busPtr == NULL) {
        Logger::E(TAG, "%s(): NULL bus pointer", __FUNCTION__);
        return ER_FAIL;
    }

    /*
     * It is not possible to register multiple AuthListeners or replace an
     * existing AuthListener.
     */
    Boolean bval;
    mBusAuthListener->AuthListenerSet(&bval);
    if (bval) {
        Logger::D(TAG, "%s: ER_ALREADY_REGISTERED", __FUNCTION__);
        return NOERROR;
    }

    mAuthMechanisms = authMechanisms;
    mBusAuthListener->SetAuthListener(listener);
    mKeyStoreFileName = keyStoreFileName;
    mIsShared = isShared;
    QStatus status = busPtr->EnablePeerSecurity(mAuthMechanisms, mBusAuthListener,
        mKeyStoreFileName, isShared);
    if (status != ER_OK) {
        Logger::E(TAG, "%s: Exception: status=%08x", __FUNCTION__);
        mBusAuthListener->SetAuthListener(NULL);
        mAuthMechanisms = NULL;
    }
    return NOERROR;
}

ECode CBusAttachment::RegisterAuthListener(
    /* [in] */ const String& authMechanisms,
    /* [in] */ IAuthListener* listener,
    /* [in] */ const String& keyStoreFileName)
{
    return RegisterAuthListener(authMechanisms, listener, keyStoreFileName, FALSE);
}

ECode CBusAttachment::RegisterAuthListener(
    /* [in] */ const String& authMechanisms,
    /* [in] */ IAuthListener* listener)
{
    return RegisterAuthListener(authMechanisms, listener, String(NULL), FALSE);
}

ECode CBusAttachment::RegisterSecurityViolationListener(
    /* [in] */ ISecurityViolationListener* listener)
{
    return mBusAuthListener->SetSecurityViolationListener(listener);
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

    *context = new MessageContext(msg->IsUnreliable(), jobjectPath,
        jinterfaceName, jmemberName, jdestination,
        jsender, sessionId, jsignature, jauthMechanism,
        serial);
    REFCOUNT_ADD(*context);
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
