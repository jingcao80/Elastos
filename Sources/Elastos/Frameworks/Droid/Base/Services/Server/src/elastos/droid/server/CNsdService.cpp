#include "elastos/droid/ext/frameworkext.h"
#include "CNsdService.h"
#include "util/IState.h"
#include <elastos/utility/logging/Slogger.h>
#include "NativeDaemonEvent.h"
#include "elastos/droid/Manifest.h"
#include <elastos/core/StringUtils.h>

using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Droid::Server::NativeDaemonEvent;
using Elastos::Utility::Concurrent::CCountDownLatch;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::Nsd::CNsdServiceInfo;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;
using Elastos::Core::IThread;
using Elastos::Core::CThread;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Server {

// 2d5d16db-461c-4eec-82ed-9358f877c13f
extern "C" const InterfaceID EIID_NativeEvent =
    { 0x2d5d16db, 0x461c, 0x4eec, { 0x82, 0xed, 0x93, 0x58, 0xf8, 0x77, 0xc1, 0x3f } };

static AutoPtr<ArrayOf<String> > InitSCmdToString()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(CNsdService::CMD_TO_STRING_COUNT);
    array->Set(INsdManager::DISCOVER_SERVICES - CNsdService::BASE, String("DISCOVER"));
    array->Set(INsdManager::STOP_DISCOVERY - CNsdService::BASE, String("STOP-DISCOVER"));
    array->Set(INsdManager::REGISTER_SERVICE - CNsdService::BASE, String("REGISTER"));
    array->Set(INsdManager::UNREGISTER_SERVICE - CNsdService::BASE, String("UNREGISTER"));
    array->Set(INsdManager::RESOLVE_SERVICE - CNsdService::BASE, String("RESOLVE"));
    return array;
}

const String CNsdService::TAG("NsdService");
const String CNsdService::MDNS_TAG("mDnsConnector");

const Boolean CNsdService::DBG = TRUE;

const Int32 CNsdService::BASE;
const Int32 CNsdService::CMD_TO_STRING_COUNT;

AutoPtr<ArrayOf<String> > CNsdService::sCmdToString = InitSCmdToString();

const Int32 CNsdService::ClientInfo::MAX_LIMIT;

const Int32 CNsdService::NativeResponseCode::SERVICE_DISCOVERY_FAILED;
const Int32 CNsdService::NativeResponseCode::SERVICE_FOUND;
const Int32 CNsdService::NativeResponseCode::SERVICE_LOST;

const Int32 CNsdService::NativeResponseCode::SERVICE_REGISTRATION_FAILED;
const Int32 CNsdService::NativeResponseCode::SERVICE_REGISTERED;

const Int32 CNsdService::NativeResponseCode::SERVICE_RESOLUTION_FAILED;
const Int32 CNsdService::NativeResponseCode::SERVICE_RESOLVED;

const Int32 CNsdService::NativeResponseCode::SERVICE_UPDATED;
const Int32 CNsdService::NativeResponseCode::SERVICE_UPDATE_FAILED;

const Int32 CNsdService::NativeResponseCode::SERVICE_GET_ADDR_FAILED;
const Int32 CNsdService::NativeResponseCode::SERVICE_GET_ADDR_SUCCESS;

//====================================================================
// CNsdService
//====================================================================

CNsdService::CNsdService()
    : INVALID_ID(0)
    , mUniqueId(1)
{
    CCountDownLatch::New(1, (ICountDownLatch**)&mNativeDaemonConnected);
}

CNsdService::~CNsdService()
{
    ClientInfo* ci = NULL;
    HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it = mClients.Begin();
    for (; it != mClients.End(); ++it) {
        ci = it->mSecond;
        if (ci && ci->mChannel) {
            ci->mChannel->Disconnected();
            ci->mChannel = NULL;
        }
    }
    mClients.Clear();

    if (mReplyChannel) {
        mReplyChannel->Disconnected();
        mReplyChannel = NULL;
    }
}

ECode CNsdService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    context->GetContentResolver((IContentResolver**)&mContentResolver);
    AutoPtr<INativeDaemonConnectorCallbacks> receiver = new NativeCallbackReceiver(this);
    mNativeConnector = new NativeDaemonConnector(receiver, String("mdns"), 10, MDNS_TAG, 25);
    mNsdStateMachine = new NsdStateMachine(TAG, this);
    mNsdStateMachine->Start();

    AutoPtr<IThread> th;
    CThread::New((IRunnable*)mNativeConnector.Get(), MDNS_TAG, (IThread**)&th);
    th->Start();
    return NOERROR;
}

ECode CNsdService::Create(
    /* [in] */ IContext* context,
    /* [out] */ CNsdService** nsdService)
{
    VALIDATE_NOT_NULL(nsdService);

    AutoPtr<CNsdService> service;
    CNsdService::NewByFriend(context, (CNsdService**)&service);
    service->mNativeDaemonConnected->Await();
    *nsdService = service.Get();
    REFCOUNT_ADD(*nsdService);
    return NOERROR;
}

ECode CNsdService::GetMessenger(
    /* [out] */ IMessenger** messenger)
{
    VALIDATE_NOT_NULL(messenger);
    *messenger = NULL;

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::INTERNET,
        String("NsdService")));
    AutoPtr<IHandler> handler = mNsdStateMachine->GetHandler();
    CMessenger::New(handler, messenger);
    return NOERROR;
}

ECode CNsdService::SetEnabled(
    /* [in] */ Boolean enable)
{
    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(
        Elastos::Droid::Manifest::permission::CONNECTIVITY_INTERNAL,
        String("NsdService")));
    Boolean result;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->PutInt32(mContentResolver, ISettingsGlobal::NSD_ON, enable ? 1 : 0, &result);
    if (enable) {
        mNsdStateMachine->SendMessage(INsdManager::ENABLE);
    } else {
        mNsdStateMachine->SendMessage(INsdManager::DISABLE);
    }
    return NOERROR;
}

ECode CNsdService::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    String ret("CNsdService:");
    ret.AppendFormat("%d", mUniqueId);
    *result = ret;
    return NOERROR;
}

String CNsdService::CmdToString(
    /* [in] */ Int32 cmd)
{
    cmd -= BASE;
    if ((cmd >= 0) && (cmd < sCmdToString->GetLength())) {
        return (*sCmdToString)[cmd];
    }
    else {
        return String(NULL);
    }
}

void CNsdService::SendNsdStateChangeBroadcast(
    /* [in] */ Boolean enabled)
{
    AutoPtr<IIntent> intent;
    CIntent::New(INsdManager::ACTION_NSD_STATE_CHANGED, (IIntent**)&intent);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);
    if (enabled) {
        intent->PutExtra(INsdManager::EXTRA_NSD_STATE, INsdManager::NSD_STATE_ENABLED);
    }
    else {
        intent->PutExtra(INsdManager::EXTRA_NSD_STATE, INsdManager::NSD_STATE_DISABLED);
    }
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> ALL;
    helper->GetALL((IUserHandle**)&ALL);
    mContext->SendStickyBroadcastAsUser(intent, ALL);
}

Boolean CNsdService::IsNsdEnabled()
{
    Int32 value;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetInt32(mContentResolver, ISettingsGlobal::NSD_ON, 1, &value);
    Boolean ret = value == 1;
    if (DBG) {
        Slogger::D(TAG, "Network service discovery enabled %d", ret);
    }
    return ret;
}

Int32 CNsdService::GetUniqueId()
{
    if (++mUniqueId == INVALID_ID)
        return ++mUniqueId;
    return mUniqueId;
}

void CNsdService::HandleNativeEvent(
    /* [in] */ Int32 code,
    /* [in] */ const String& raw,
    /* [in] */ ArrayOf<String>* cooked)
{
    AutoPtr<INsdServiceInfo> servInfo;
    Int32 id = StringUtils::ParseInt32((*cooked)[1]);
    HashMap<Int32, AutoPtr<ClientInfo> >::Iterator it = mIdToClientInfoMap.Find(id);
    if (it == mIdToClientInfoMap.End()) {
        Slogger::E(TAG, "Unique id with no client mapping: %d", id);
        return;
    }

    AutoPtr<ClientInfo> clientInfo = it->mSecond;
    /* This goes in response as msg.arg2 */
    Int32 clientId = -1;
    HashMap<Int32, Int32>::Iterator iter = clientInfo->mClientIds.Begin();
    for (Int32 i = 0; iter != clientInfo->mClientIds.End(); ++iter, ++i) {
        if (iter->mSecond == id) {
            clientId = iter->mFirst;
            break;
        }
    }

    switch (code)
    {
        case NativeResponseCode::SERVICE_FOUND:
        {
            /* NNN uniqueId serviceName regType domain */
            if (DBG) Slogger::D(TAG, "SERVICE_FOUND Raw: %s", raw.string());
            CNsdServiceInfo::New((*cooked)[2], (*cooked)[3], NULL, (INsdServiceInfo**)&servInfo);
            clientInfo->mChannel->SendMessage(INsdManager::SERVICE_FOUND, 0,
                clientId, servInfo);
            break;
        }
        case NativeResponseCode::SERVICE_LOST:
        {
            /* NNN uniqueId serviceName regType domain */
            if (DBG) Slogger::D(TAG, "SERVICE_LOST Raw: %s", raw.string());
            CNsdServiceInfo::New((*cooked)[2], (*cooked)[3], NULL, (INsdServiceInfo**)&servInfo);
            clientInfo->mChannel->SendMessage(INsdManager::SERVICE_LOST, 0,
                clientId, servInfo);
            break;
        }
        case NativeResponseCode::SERVICE_DISCOVERY_FAILED:
        {
            /* NNN uniqueId errorCode */
            if (DBG) Slogger::D(TAG, "SERVICE_DISC_FAILED Raw: %s", raw.string());
            clientInfo->mChannel->SendMessage(INsdManager::DISCOVER_SERVICES_FAILED,
               INsdManager::FAILURE_INTERNAL_ERROR, clientId);
            break;
        }
        case NativeResponseCode::SERVICE_REGISTERED:
        {
            /* NNN regId serviceName regType */
            if (DBG) Slogger::D(TAG, "SERVICE_REGISTERED Raw: %s", raw.string());
            CNsdServiceInfo::New((*cooked)[2], String(NULL), NULL, (INsdServiceInfo**)&servInfo);
            clientInfo->mChannel->SendMessage(INsdManager::REGISTER_SERVICE_SUCCEEDED,
                id, clientId, servInfo);
            break;
        }
        case NativeResponseCode::SERVICE_REGISTRATION_FAILED:
        {
            /* NNN regId errorCode */
            if (DBG) Slogger::D(TAG, "SERVICE_REGISTER_FAILED Raw: %s", raw.string());
            clientInfo->mChannel->SendMessage(INsdManager::REGISTER_SERVICE_FAILED,
               INsdManager::FAILURE_INTERNAL_ERROR, clientId);
            break;
        }
        case NativeResponseCode::SERVICE_UPDATED:
            /* NNN regId */
            break;
        case NativeResponseCode::SERVICE_UPDATE_FAILED:
            /* NNN regId errorCode */
            break;
        case NativeResponseCode::SERVICE_RESOLVED:
        {
            /* NNN resolveId fullName hostName port txtlen txtdata */
            if (DBG) Slogger::D(TAG, "SERVICE_RESOLVED Raw: %s",raw.string());
            Int32 index = (*cooked)[2].IndexOf(".");
            if (index == -1) {
                Slogger::E(TAG, "Invalid service found %s",raw.string());
                break;
            }
            String name = (*cooked)[2].Substring(0, index);
            String rest = (*cooked)[2].Substring(index);
            String type;
            StringUtils::ReplaceAll(rest, String(".local."), String(""), &type);

            clientInfo->mResolvedService->SetServiceName(name);
            clientInfo->mResolvedService->SetServiceType(type);
            clientInfo->mResolvedService->SetPort(
                StringUtils::ParseInt32((*cooked)[4]));

            StopResolveService(id);
            if (!GetAddrInfo(id, (*cooked)[3])) {
                clientInfo->mChannel->SendMessage(INsdManager::RESOLVE_SERVICE_FAILED,
                    INsdManager::FAILURE_INTERNAL_ERROR, clientId);
                mIdToClientInfoMap.Erase(id);
                clientInfo->mResolvedService = NULL;
            }
            break;
        }
        case NativeResponseCode::SERVICE_RESOLUTION_FAILED:
        {
            /* NNN resolveId errorCode */
            if (DBG) Slogger::D(TAG, "SERVICE_RESOLVE_FAILED Raw: %s" , raw.string());
            StopResolveService(id);
            mIdToClientInfoMap.Erase(id);
            clientInfo->mResolvedService = NULL;
            clientInfo->mChannel->SendMessage(INsdManager::RESOLVE_SERVICE_FAILED,
                INsdManager::FAILURE_INTERNAL_ERROR, clientId);
            break;
        }
        case NativeResponseCode::SERVICE_GET_ADDR_FAILED:
        {
            /* NNN resolveId errorCode */
            StopGetAddrInfo(id);
            mIdToClientInfoMap.Erase(id);
            clientInfo->mResolvedService = NULL;
            if (DBG) Slogger::D(TAG, "SERVICE_RESOLVE_FAILED Raw: %s", raw.string());
            clientInfo->mChannel->SendMessage(INsdManager::RESOLVE_SERVICE_FAILED,
                INsdManager::FAILURE_INTERNAL_ERROR, clientId);
            break;
        }
        case NativeResponseCode::SERVICE_GET_ADDR_SUCCESS:
        {
            /* NNN resolveId hostname ttl addr */
            if (DBG) Slogger::D(TAG, "SERVICE_GET_ADDR_SUCCESS Raw: %s", raw.string());
            // try {
            AutoPtr<IInetAddressHelper> helper;
            CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
            AutoPtr<IInetAddress> address;
            helper->GetByName((*cooked)[4], (IInetAddress**)&address);
            ECode ec = clientInfo->mResolvedService->SetHost(address.Get());
            if (FAILED(ec)) {
                clientInfo->mChannel->SendMessage(INsdManager::RESOLVE_SERVICE_FAILED,
                    INsdManager::FAILURE_INTERNAL_ERROR, clientId);
            }
            else {
                clientInfo->mChannel->SendMessage(INsdManager::RESOLVE_SERVICE_SUCCEEDED,
                    0, clientId, clientInfo->mResolvedService);
            }
            // } catch (java.net.UnknownHostException e) {
            // clientInfo->mChannel->SendMessage(INsdManager::RESOLVE_SERVICE_FAILED,
            //         INsdManager::FAILURE_INTERNAL_ERROR, clientId);
            // }
            StopGetAddrInfo(id);
            mIdToClientInfoMap.Erase(id);
            clientInfo->mResolvedService = NULL;
            break;
        }
        default:
            break;
    }
}

Boolean CNsdService::StartMDnsDaemon()
{
    if (DBG) Slogger::D(TAG, "startMDnsDaemon");
    // try {
    AutoPtr<NativeDaemonEvent> ndEvnet;
    ECode ec = mNativeConnector->Execute(String("mdnssd"), String("start-service"),
        (NativeDaemonEvent**)&ndEvnet);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to start daemon: mdnssd-start-service ec=%08x", ec);
        return FALSE;
    }
    // } catch(NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "Failed to start daemon" + e);
    //     return false;
    // }
    return TRUE;
}

Boolean CNsdService::StopMDnsDaemon()
{
    if (DBG) Slogger::D(TAG, "stopMDnsDaemon");
    // try {
    AutoPtr<NativeDaemonEvent> ndEvnet;
    ECode ec = mNativeConnector->Execute(String("mdnssd"), String("stop-service"),
        (NativeDaemonEvent**)&ndEvnet);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to stop daemon: mdnssd-start-service ec=%08x", ec);
        return FALSE;
    }
    // } catch(NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "Failed to start daemon" + e);
    //     return false;
    // }
    return TRUE;
}

Boolean CNsdService::RegisterService(
    /* [in] */ Int32 regId,
    /* [in] */ INsdServiceInfo* service)
{
    if (service == NULL) {
        return FALSE;
    }

    if (DBG) {
        String info;
        service->ToString(&info);
        Slogger::D(TAG, "registerService: %d, service: %s", regId, info.string());
    }

    // try {
    //Add txtlen and txtdata
    String name, type;
    Int32 port;
    service->GetServiceName(&name);
    service->GetServiceType(&type);
    service->GetPort(&port);

    AutoPtr<ICharSequence> actionSeq, nameSeq, typeSeq;
    CString::New(String("register"), (ICharSequence**)&actionSeq);
    CString::New(name, (ICharSequence**)&nameSeq);
    CString::New(type, (ICharSequence**)&typeSeq);
    AutoPtr<IInteger32> portObj;
    CInteger32::New(port, (IInteger32**)&portObj);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
    args->Set(0, actionSeq->Probe(EIID_IInterface));
    args->Set(1, nameSeq->Probe(EIID_IInterface));
    args->Set(2, typeSeq->Probe(EIID_IInterface));
    args->Set(3, portObj->Probe(EIID_IInterface));

    AutoPtr<NativeDaemonEvent> ndEvnet;
    ECode ec = mNativeConnector->Execute(String("mdnssd"), args, (NativeDaemonEvent**)&ndEvnet);
    if (FAILED(ec)) {
        String info;
        service->ToString(&info);
        Slogger::E(TAG, "Failed to registerService %s ec=%08x", info.string(), ec);
        return FALSE;
    }
    // } catch(NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "Failed to execute registerService " + e);
    //     return false;
    // }
    return TRUE;
}

Boolean CNsdService::UnregisterService(
    /* [in] */ Int32 regId)
{
    if (DBG) Slogger::D(TAG, "unregisterService: %d", regId);
    // try {

    AutoPtr<ICharSequence> actionSeq;
    CString::New(String("stop-register"), (ICharSequence**)&actionSeq);
    AutoPtr<IInteger32> regObj;
    CInteger32::New(regId, (IInteger32**)&regObj);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, actionSeq->Probe(EIID_IInterface));
    args->Set(1, regObj->Probe(EIID_IInterface));

    AutoPtr<NativeDaemonEvent> ndEvnet;
    ECode ec = mNativeConnector->Execute(String("mdnssd"), args, (NativeDaemonEvent**)&ndEvnet);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to execute unregisterService %d, ec=%08x", regId, ec);
        return FALSE;
    }
    // } catch(NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "Failed to execute unregisterService " + e);
    //     return false;
    // }
    return TRUE;
}

Boolean CNsdService::UpdateService(
    /* [in] */ Int32 regId,
    /* [in] */ IDnsSdTxtRecord* t)
{
    if (t == NULL) {
        return FALSE;
    }

    if (DBG) {
        String info;
        t->ToString(&info);
        Slogger::D(TAG, "updateService: %d, IDnsSdTxtRecord: %s", regId, info.string());
    }
    //try {
    Int32 size;
    t->Size(&size);
    AutoPtr<ArrayOf<Byte> > rawData;
    t->GetRawData((ArrayOf<Byte>**)&rawData);

    AutoPtr<ICharSequence> actionSeq, rawDataSeq;
    CString::New(String("update"), (ICharSequence**)&actionSeq);
    AutoPtr<IInteger32> regObj, sizeObj;
    CInteger32::New(regId, (IInteger32**)&regObj);
    CInteger32::New(size, (IInteger32**)&sizeObj);

    String rawStr((const char*)rawData->GetPayload(), rawData->GetLength());
    CString::New(rawStr, (ICharSequence**)&rawDataSeq);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(4);
    args->Set(0, actionSeq->Probe(EIID_IInterface));
    args->Set(1, regObj->Probe(EIID_IInterface));
    args->Set(2, sizeObj->Probe(EIID_IInterface));
    args->Set(3, rawDataSeq->Probe(EIID_IInterface));

    AutoPtr<NativeDaemonEvent> ndEvnet;
    ECode ec = mNativeConnector->Execute(String("mdnssd"), args, (NativeDaemonEvent**)&ndEvnet);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to updateServices regId %d, ec=%08x", regId, ec);
        return FALSE;
    }
    //} catch(NativeDaemonConnectorException e) {
    //    Slog.e(TAG, "Failed to updateServices " + e);
    //    return false;
    //}
    return TRUE;
}

Boolean CNsdService::DiscoverServices(
    /* [in] */ Int32 discoveryId,
    /* [in] */ const String& serviceType)
{
    if (DBG) {
        Slogger::D(TAG, "discoverServices: %d %s", discoveryId , serviceType.string());
    }
    // try {
    AutoPtr<ICharSequence> actionSeq, serviceTypeSeq;
    CString::New(String("discover"), (ICharSequence**)&actionSeq);
    CString::New(String("serviceType"), (ICharSequence**)&serviceTypeSeq);
    AutoPtr<IInteger32> idObj;
    CInteger32::New(discoveryId, (IInteger32**)&idObj);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, actionSeq->Probe(EIID_IInterface));
    args->Set(1, idObj->Probe(EIID_IInterface));
    args->Set(2, serviceTypeSeq->Probe(EIID_IInterface));

    AutoPtr<NativeDaemonEvent> ndEvnet;
    ECode ec = mNativeConnector->Execute(String("mdnssd"), args, (NativeDaemonEvent**)&ndEvnet);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to discoverServices regId %d, %s, ec=%08x",
            discoveryId, serviceType.string(), ec);
        return FALSE;
    }
    // } catch(NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "Failed to discoverServices " + e);
    //     return false;
    // }
    return TRUE;
}

Boolean CNsdService::StopServiceDiscovery(
    /* [in] */ Int32 discoveryId)
{
    if (DBG) {
        Slogger::D(TAG, "stopServiceDiscovery: %d", discoveryId);
    }
    // try {
    AutoPtr<ICharSequence> actionSeq;
    CString::New(String("stop-discover"), (ICharSequence**)&actionSeq);
    AutoPtr<IInteger32> idObj;
    CInteger32::New(discoveryId, (IInteger32**)&idObj);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, actionSeq->Probe(EIID_IInterface));
    args->Set(1, idObj->Probe(EIID_IInterface));

    AutoPtr<NativeDaemonEvent> ndEvnet;
    ECode ec = mNativeConnector->Execute(String("mdnssd"), args, (NativeDaemonEvent**)&ndEvnet);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to stopServiceDiscovery regId %d, ec=%08x",
            discoveryId, ec);
        return FALSE;
    }
    // } catch(NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "Failed to stopServiceDiscovery " + e);
    //     return false;
    // }
    return TRUE;
}

Boolean CNsdService::ResolveService(
    /* [in] */ Int32 resolveId,
    /* [in] */ INsdServiceInfo* service)
{
    if (service == NULL)
        return FALSE;

    if (DBG) {
        String info;
        service->ToString(&info);
        Slogger::E(TAG, "resolveService: %d, %s", resolveId, info.string());
    }
    // try {
    String name, type;
    service->GetServiceName(&name);
    service->GetServiceType(&type);

    AutoPtr<ICharSequence> actionSeq, nameSeq, typeSeq, localSeq;
    CString::New(String("resolve"), (ICharSequence**)&actionSeq);
    AutoPtr<IInteger32> idObj;
    CInteger32::New(resolveId, (IInteger32**)&idObj);
    CString::New(name, (ICharSequence**)&nameSeq);
    CString::New(type, (ICharSequence**)&typeSeq);
    CString::New(String("local."), (ICharSequence**)&localSeq);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(5);
    args->Set(0, actionSeq->Probe(EIID_IInterface));
    args->Set(1, idObj->Probe(EIID_IInterface));
    args->Set(2, nameSeq->Probe(EIID_IInterface));
    args->Set(3, typeSeq->Probe(EIID_IInterface));
    args->Set(4, localSeq->Probe(EIID_IInterface));

    AutoPtr<NativeDaemonEvent> ndEvnet;
    ECode ec = mNativeConnector->Execute(String("mdnssd"), args, (NativeDaemonEvent**)&ndEvnet);
    if (FAILED(ec)) {
        String info;
        service->ToString(&info);
        Slogger::E(TAG, "Failed to resolveService %d, %s, ec=%08x",
            resolveId, info.string(), ec);
        return FALSE;
    }
    // } catch(NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "Failed to resolveService " + e);
    //     return false;
    // }
    return TRUE;
}

Boolean CNsdService::StopResolveService(
        /* [in] */ Int32 resolveId)
{
    if (DBG) Slogger::D(TAG, "stopResolveService: %d", resolveId);
    // try {
    AutoPtr<ICharSequence> actionSeq;
    CString::New(String("stop-resolve"), (ICharSequence**)&actionSeq);
    AutoPtr<IInteger32> idObj;
    CInteger32::New(resolveId, (IInteger32**)&idObj);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, actionSeq->Probe(EIID_IInterface));
    args->Set(1, idObj->Probe(EIID_IInterface));

    AutoPtr<NativeDaemonEvent> ndEvnet;
    ECode ec = mNativeConnector->Execute(String("mdnssd"), args, (NativeDaemonEvent**)&ndEvnet);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to stop resolve %d, ec=%08x", resolveId, ec);
        return FALSE;
    }
    // } catch(NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "Failed to stop resolve " + e);
    //     return false;
    // }
    return TRUE;
}

Boolean CNsdService::GetAddrInfo(
    /* [in] */ Int32 resolveId,
    /* [in] */ const String& hostname)
{
    if (DBG) Slogger::D(TAG, "getAdddrInfo: %d, hostname: %s",
        resolveId, hostname.string());
    // try {
    AutoPtr<ICharSequence> actionSeq, hostnameSeq;
    CString::New(String("getaddrinfo"), (ICharSequence**)&actionSeq);
    AutoPtr<IInteger32> idObj;
    CInteger32::New(resolveId, (IInteger32**)&idObj);
    CString::New(hostname, (ICharSequence**)&hostnameSeq);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(3);
    args->Set(0, actionSeq->Probe(EIID_IInterface));
    args->Set(1, idObj->Probe(EIID_IInterface));
    args->Set(2, hostnameSeq->Probe(EIID_IInterface));

    AutoPtr<NativeDaemonEvent> ndEvnet;
    ECode ec = mNativeConnector->Execute(String("mdnssd"), args, (NativeDaemonEvent**)&ndEvnet);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to getAddrInfo %d, hostname: %s, ec=%08x",
            resolveId, hostname.string(), ec);
        return FALSE;
    }
    // } catch(NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "Failed to getAddrInfo " + e);
    //     return false;
    // }
    return TRUE;
}

Boolean CNsdService::StopGetAddrInfo(
    /* [in] */ Int32 resolveId)
{
    if (DBG) Slogger::D(TAG, "stopGetAdddrInfo: %d", resolveId);
    // try {
    AutoPtr<ICharSequence> actionSeq;
    CString::New(String("stop-getaddrinfo"), (ICharSequence**)&actionSeq);
    AutoPtr<IInteger32> idObj;
    CInteger32::New(resolveId, (IInteger32**)&idObj);

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(2);
    args->Set(0, actionSeq->Probe(EIID_IInterface));
    args->Set(1, idObj->Probe(EIID_IInterface));

    AutoPtr<NativeDaemonEvent> ndEvnet;
    ECode ec = mNativeConnector->Execute(String("mdnssd"), args, (NativeDaemonEvent**)&ndEvnet);
     if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to stopGetAddrInfo %d, ec=%08x", resolveId, ec);
        return FALSE;
    }
    // } catch(NativeDaemonConnectorException e) {
    //     Slog.e(TAG, "Failed to stopGetAddrInfo " + e);
    //     return false;
    // }
    return TRUE;
}

AutoPtr<IMessage> CNsdService::ObtainMessage(
    /* [in] */ IMessage* srcMsg)
{
    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    AutoPtr<IMessage> msg;
    helper->Obtain((IMessage**)&msg);
    Int32 arg2;
    srcMsg->GetArg2(&arg2);
    msg->SetArg2(arg2);
    return msg;
}

void CNsdService::ReplyToMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo == NULL) return;
    AutoPtr<IMessage> dstMsg = ObtainMessage(msg);
    dstMsg->SetWhat(what);
    mReplyChannel->ReplyToMessage(msg, dstMsg);
}

void CNsdService::ReplyToMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what,
    /* [in] */ Int32 arg1)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo == NULL) return;
    AutoPtr<IMessage> dstMsg = ObtainMessage(msg);
    dstMsg->SetWhat(what);
    dstMsg->SetArg1(arg1);
    mReplyChannel->ReplyToMessage(msg, dstMsg);
}

void CNsdService::ReplyToMessage(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo == NULL) return;
    AutoPtr<IMessage> dstMsg = ObtainMessage(msg);
    dstMsg->SetWhat(what);
    dstMsg->SetObj(obj);
    mReplyChannel->ReplyToMessage(msg, dstMsg);
}


//====================================================================
// CNsdService::NsdStateMachine
//====================================================================
String CNsdService::NsdStateMachine::GetWhatToString(
    /* [in] */ Int32 what)
{
    return mHost->CmdToString(what);
}

void CNsdService::NsdStateMachine::RegisterForNsdSetting()
{
    AutoPtr<IHandler> handler = this->GetHandler();
    AutoPtr<MyContentObserver> contentObserver = new MyContentObserver(handler, this);
    AutoPtr<IContentResolver> cr;
    mHost->mContext->GetContentResolver((IContentResolver**)&cr);
    AutoPtr<IUri> uri;
    AutoPtr<ISettingsGlobal> settingsGlobal;
    CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&settingsGlobal);
    settingsGlobal->GetUriFor(ISettingsGlobal::NSD_ON, (IUri**)&uri);
    cr->RegisterContentObserver(uri, FALSE, (IContentObserver*)contentObserver);
}

CNsdService::NsdStateMachine::NsdStateMachine(
    /* [in] */ const String& name,
    /* [in] */ CNsdService* owner)
    : StateMachine(name)
    , mHost(owner)
{
    mDefaultState = new DefaultState(this);
    mDisabledState = new DisabledState(this);
    mEnabledState = new EnabledState(this);

    AddState((State*)mDefaultState);
    AddState(mDisabledState, mDefaultState);
    AddState(mEnabledState, mDefaultState);

    if (mHost->IsNsdEnabled()) {
        SetInitialState(mEnabledState);
    }
    else {
        SetInitialState(mDisabledState);
    }
    SetLogRecSize(25);
    RegisterForNsdSetting();
}


//====================================================================
// CNsdService::NsdStateMachine::MyContentObserver
//====================================================================
CNsdService::NsdStateMachine::MyContentObserver::MyContentObserver(
    /* [in] */ IHandler* handler,
    /* [in] */ NsdStateMachine* owner)
    : ContentObserver(handler)
{
    mHost = owner;
}

ECode CNsdService::NsdStateMachine::MyContentObserver::OnChange(
    /* [in] */ Boolean selfChange)
{
    if (mHost->mHost->IsNsdEnabled()) {
        mHost->mHost->mNsdStateMachine->SendMessage(INsdManager::ENABLE);
    } else {
        mHost->mHost->mNsdStateMachine->SendMessage(INsdManager::DISABLE);
    }
    return NOERROR;
}


//====================================================================
// CNsdService::NsdStateMachine::DefaultState
//====================================================================
CNsdService::NsdStateMachine::DefaultState::DefaultState(
    /* [in] */ NsdStateMachine* owner)
{
    mHost = owner;
}

ECode CNsdService::NsdStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);

    switch(what) {
        case AsyncChannel::CMD_CHANNEL_HALF_CONNECTED:
        {
            if (arg1 == AsyncChannel::STATUS_SUCCESSFUL) {
                AutoPtr<IInterface> obj;
                msg->GetObj((IInterface**)&obj);
                AutoPtr<AsyncChannel> c = (AsyncChannel*)obj.Get();
                if (DBG) {
                    Slogger::D("NsdStateMachine::DefaultState", "New client listening to asynchronous messages");
                }
                c->SendMessage(AsyncChannel::CMD_CHANNEL_FULLY_CONNECTED);
                AutoPtr<IMessenger> replyTo;
                msg->GetReplyTo((IMessenger**)&replyTo);
                AutoPtr<ClientInfo> cInfo = new ClientInfo(c, replyTo);
                mHost->mHost->mClients.Insert(
                     HashMap< AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::ValueType(replyTo, cInfo));
            }
            else {
                Slogger::E("NsdStateMachine::DefaultState", "Client connection failure, error=%d", arg1);
            }
            break;
        }
        case AsyncChannel::CMD_CHANNEL_DISCONNECTED:
        {
            if (arg1 == AsyncChannel::STATUS_SEND_UNSUCCESSFUL) {
                Slogger::E("NsdStateMachine::DefaultState", "Send failed, client connection lost");
            }
            else {
                if (DBG) {
                    Slogger::D("NsdStateMachine::DefaultState", "Client connection lost with reason: %d", arg1);
                }
            }
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);
            HashMap< AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it;
            it = mHost->mHost->mClients.Find(replyTo);
            if (it != mHost->mHost->mClients.End()) {
                AutoPtr<ClientInfo> ci = it->mSecond;
                if (ci && ci->mChannel) {
                    ci->mChannel->Disconnected();
                    ci->mChannel = NULL;
                }
                mHost->mHost->mClients.Erase(it);
            }

            break;
        }
        case AsyncChannel::CMD_CHANNEL_FULL_CONNECTION:
        {
            AutoPtr<AsyncChannel> ac = new AsyncChannel();
            AutoPtr<IHandler> handler = mHost->GetHandler();
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);
            ac->Connect(mHost->mHost->mContext, handler, replyTo);
            break;
        }
        case INsdManager::DISCOVER_SERVICES:
        {
            mHost->mHost->ReplyToMessage(msg, INsdManager::DISCOVER_SERVICES_FAILED,
                INsdManager::FAILURE_INTERNAL_ERROR);
            break;
        }
        case INsdManager::STOP_DISCOVERY:
        {
            mHost->mHost->ReplyToMessage(msg, INsdManager::STOP_DISCOVERY_FAILED,
                INsdManager::FAILURE_INTERNAL_ERROR);
            break;
        }
        case INsdManager::REGISTER_SERVICE:
        {
            mHost->mHost->ReplyToMessage(msg, INsdManager::REGISTER_SERVICE_FAILED,
                INsdManager::FAILURE_INTERNAL_ERROR);
            break;
        }
        case INsdManager::UNREGISTER_SERVICE:
        {
            mHost->mHost->ReplyToMessage(msg, INsdManager::UNREGISTER_SERVICE_FAILED,
                INsdManager::FAILURE_INTERNAL_ERROR);
            break;
        }
        case INsdManager::RESOLVE_SERVICE:
        {
            mHost->mHost->ReplyToMessage(msg, INsdManager::RESOLVE_SERVICE_FAILED,
                INsdManager::FAILURE_INTERNAL_ERROR);
            break;
        }
        case INsdManager::NATIVE_DAEMON_EVENT:
        default:
            Slogger::E("NsdStateMachine::DefaultState", "Unhandled %08x", what);
            *result = StateMachine::NOT_HANDLED;
            return NOERROR;\
    }

    *result = StateMachine::HANDLED;
    return NOERROR;
}


//====================================================================
// CNsdService::NsdStateMachine::DisabledState
//====================================================================
CNsdService::NsdStateMachine::DisabledState::DisabledState(
    /* [in] */ NsdStateMachine* owner)
{
    mHost = owner;
}

ECode CNsdService::NsdStateMachine::DisabledState::Enter()
{
    mHost->mHost->SendNsdStateChangeBroadcast(FALSE);
    return NOERROR;
}

ECode CNsdService::NsdStateMachine::DisabledState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    Int32 what;
    msg->GetWhat(&what);
    switch (what)
    {
        case INsdManager::ENABLE:
        {
            mHost->TransitionTo((IState*)(mHost->mEnabledState));
            break;
        }
        default:
        {
            *result = StateMachine::NOT_HANDLED;
            return NOERROR;
        }
    }

    *result = StateMachine::HANDLED;
    return NOERROR;
}


//====================================================================
// CNsdService::NsdStateMachine::EnabledState
//====================================================================
CNsdService::NsdStateMachine::EnabledState::EnabledState(
    /* [in] */ NsdStateMachine* owner)
{
    mHost = owner;
}

ECode CNsdService::NsdStateMachine::EnabledState::Enter()
{
    mHost->mHost->SendNsdStateChangeBroadcast(FALSE);
    if (!mHost->mHost->mClients.IsEmpty()) {
        mHost->mHost->StartMDnsDaemon();
    }
    return NOERROR;
}

ECode CNsdService::NsdStateMachine::EnabledState::Exit()
{
    if (!mHost->mHost->mClients.IsEmpty()) {
        mHost->mHost->StopMDnsDaemon();
    }
    return NOERROR;
}

Boolean CNsdService::NsdStateMachine::EnabledState::RequestLimitReached(
    /* [in] */ ClientInfo* clientInfo)
{
    if (clientInfo->mClientIds.GetSize() >= (UInt32)ClientInfo::MAX_LIMIT) {
        if (DBG) {
            Slogger::E("NsdStateMachine::EnabledState",
                "Exceeded max outstanding requests %p", clientInfo);
        }
        return TRUE;
    }
    return FALSE;
}

void CNsdService::NsdStateMachine::EnabledState::StoreRequestMap(
    /* [in] */ Int32 clientId,
    /* [in] */ Int32 globalId,
    /* [in] */ ClientInfo* clientInfo)
{
    clientInfo->mClientIds.Insert(
        HashMap<Int32, Int32>::ValueType(clientId, globalId));
    mHost->mHost->mIdToClientInfoMap.Insert(
        HashMap<Int32, AutoPtr<ClientInfo> >::ValueType(globalId, clientInfo));
}

void CNsdService::NsdStateMachine::EnabledState::RemoveRequestMap(
    /* [in] */ Int32 clientId,
    /* [in] */ Int32 globalId,
    /* [in] */ ClientInfo* clientInfo)
{
    clientInfo->mClientIds.Erase(clientId);
    mHost->mHost->mIdToClientInfoMap.Erase(globalId);
}

ECode CNsdService::NsdStateMachine::EnabledState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* value)
{
    VALIDATE_NOT_NULL(value);
    AutoPtr<ClientInfo> clientInfo;
    AutoPtr<INsdServiceInfo> servInfo;
    Boolean result = StateMachine::HANDLED;
    Int32 id;
    Int32 what, arg1;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);

    switch (what)
    {
        case AsyncChannel::CMD_CHANNEL_HALF_CONNECTED:
        {
            //First client
            if (arg1 == AsyncChannel::STATUS_SUCCESSFUL &&
                mHost->mHost->mClients.IsEmpty()) {
                mHost->mHost->StartMDnsDaemon();
            }
            result = StateMachine::NOT_HANDLED;
            break;
        }
        case AsyncChannel::CMD_CHANNEL_DISCONNECTED:
        {
            //Last client
            if (mHost->mHost->mClients.GetSize() == 1) {
                mHost->mHost->StopMDnsDaemon();
            }
            result = StateMachine::NOT_HANDLED;
            break;
        }
        case INsdManager::DISABLE:
        {
            //TODO: cleanup clients
            mHost->TransitionTo((IState*)mHost->mDisabledState);
            break;
        }
        case INsdManager::DISCOVER_SERVICES:
        {
            if (DBG) {
                Slogger::D("NsdStateMachine::EnabledState", "Discover services");
            }
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            servInfo = INsdServiceInfo::Probe(obj);
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);

            HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it;
            it = mHost->mHost->mClients.Find(replyTo);
            if (it != mHost->mHost->mClients.End())
                clientInfo = it->mSecond;

            if (RequestLimitReached(clientInfo)) {
                mHost->mHost->ReplyToMessage(msg, INsdManager::DISCOVER_SERVICES_FAILED,
                        INsdManager::FAILURE_MAX_LIMIT);
                break;
            }

            id = mHost->mHost->GetUniqueId();
            String serviceType;
            servInfo->GetServiceType(&serviceType);
            if (mHost->mHost->DiscoverServices(id, serviceType)) {
                Int32 arg2;
                msg->GetArg2(&arg2);
                if (DBG) {
                    Slogger::D("NsdStateMachine::EnabledState", "Discover %d %d %s", arg2, id , serviceType.string());
                }
                StoreRequestMap(arg2, id, clientInfo);
                mHost->mHost->ReplyToMessage(msg, INsdManager::DISCOVER_SERVICES_STARTED,
                    servInfo->Probe(EIID_IInterface));
            }
            else {
                mHost->mHost->StopServiceDiscovery(id);
                mHost->mHost->ReplyToMessage(msg, INsdManager::DISCOVER_SERVICES_FAILED,
                    INsdManager::FAILURE_INTERNAL_ERROR);
            }
            break;
        }
        case INsdManager::STOP_DISCOVERY:
        {
            if (DBG) Slogger::D("NsdStateMachine::EnabledState", "Stop service discovery");
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);
            HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it;
            it = mHost->mHost->mClients.Find(replyTo);
            if (it != mHost->mHost->mClients.End())
                clientInfo = it->mSecond;

            Int32 arg2;
            msg->GetArg2(&arg2);

            // try {
            Boolean throwNullException = TRUE;
            if (clientInfo != NULL) {
                HashMap<Int32, Int32>::Iterator it;
                it = clientInfo->mClientIds.Find(arg2);
                if (it != clientInfo->mClientIds.End()) {
                    id = it->mSecond;
                    throwNullException = FALSE;
                }
            }

            if (throwNullException) {
               mHost->mHost->ReplyToMessage(msg, INsdManager::STOP_DISCOVERY_FAILED,
                    INsdManager::FAILURE_INTERNAL_ERROR);
                break;
            }
            // } catch (NullPointerException e) {
            //     replyToMessage(msg, INsdManager::STOP_DISCOVERY_FAILED,
            //             INsdManager::FAILURE_INTERNAL_ERROR);
            //     break;
            // }
            RemoveRequestMap(arg2, id, clientInfo);
            if (mHost->mHost->StopServiceDiscovery(id)) {
                mHost->mHost->ReplyToMessage(msg, INsdManager::STOP_DISCOVERY_SUCCEEDED);
            }
            else {
                mHost->mHost->ReplyToMessage(msg, INsdManager::STOP_DISCOVERY_FAILED,
                        INsdManager::FAILURE_INTERNAL_ERROR);
            }
            break;
        }
        case INsdManager::REGISTER_SERVICE:
        {
            if (DBG) Slogger::D("NsdStateMachine::EnabledState", "Register service");
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);
            HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it;
            it = mHost->mHost->mClients.Find(replyTo);
            if (it != mHost->mHost->mClients.End())
                clientInfo = it->mSecond;

            if (RequestLimitReached(clientInfo)) {
                mHost->mHost->ReplyToMessage(msg, INsdManager::REGISTER_SERVICE_FAILED,
                        INsdManager::FAILURE_MAX_LIMIT);
                break;
            }

            id = mHost->mHost->GetUniqueId();
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<INsdServiceInfo> nsInfo = INsdServiceInfo::Probe(obj);
            if (mHost->mHost->RegisterService(id, nsInfo)) {
                Int32 arg2;
                msg->GetArg2(&arg2);
                if (DBG) Slogger::D("NsdStateMachine::EnabledState", "Register %d %d", arg2, id);
                StoreRequestMap(arg2, id, clientInfo);
                // Return success after mDns reports success
            }
            else {
                mHost->mHost->UnregisterService(id);
                mHost->mHost->ReplyToMessage(msg, INsdManager::REGISTER_SERVICE_FAILED,
                        INsdManager::FAILURE_INTERNAL_ERROR);
            }
            break;
        }
        case INsdManager::UNREGISTER_SERVICE:
        {
            if (DBG) Slogger::D("NsdStateMachine::EnabledState", "unregister service");
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);
            HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it;
            it = mHost->mHost->mClients.Find(replyTo);
            if (it != mHost->mHost->mClients.End())
                clientInfo = it->mSecond;

            Int32 arg2;
            msg->GetArg2(&arg2);
            Boolean throwNullException = TRUE;
            // try {
            if (clientInfo != NULL) {
                HashMap<Int32, Int32>::Iterator ite;
                ite = clientInfo->mClientIds.Find(arg2);
                if (ite != clientInfo->mClientIds.End()) {
                    id = ite->mSecond;
                    throwNullException = FALSE;
                }
            }

            if (throwNullException) {
                mHost->mHost->ReplyToMessage(msg, INsdManager::UNREGISTER_SERVICE_FAILED,
                    INsdManager::FAILURE_INTERNAL_ERROR);
                break;
            }
            // } catch (NullPointerException e) {
            //     replyToMessage(msg, INsdManager::UNREGISTER_SERVICE_FAILED,
            //             INsdManager::FAILURE_INTERNAL_ERROR);
            //     break;
            // }
            RemoveRequestMap(arg2, id, clientInfo);
            if (mHost->mHost->UnregisterService(id)) {
                mHost->mHost->ReplyToMessage(msg, INsdManager::UNREGISTER_SERVICE_SUCCEEDED);
            }
            else {
                mHost->mHost->ReplyToMessage(msg, INsdManager::UNREGISTER_SERVICE_FAILED,
                    INsdManager::FAILURE_INTERNAL_ERROR);
            }
            break;
        }
        case INsdManager::RESOLVE_SERVICE:
        {
            if (DBG) Slogger::D("NsdStateMachine::EnabledState", "Resolve service");
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            servInfo = (INsdServiceInfo*) obj.Get();
            AutoPtr<IMessenger> replyTo;
            msg->GetReplyTo((IMessenger**)&replyTo);
            HashMap<AutoPtr<IMessenger>, AutoPtr<ClientInfo> >::Iterator it;
            it = mHost->mHost->mClients.Find(replyTo);
            if (it != mHost->mHost->mClients.End())
                clientInfo = it->mSecond;

            assert(clientInfo != NULL);
            if (clientInfo->mResolvedService != NULL) {
                mHost->mHost->ReplyToMessage(msg, INsdManager::RESOLVE_SERVICE_FAILED,
                    INsdManager::FAILURE_ALREADY_ACTIVE);
                break;
            }

            id = mHost->mHost->GetUniqueId();
            if (mHost->mHost->ResolveService(id, servInfo)) {
                Int32 arg2;
                msg->GetArg2(&arg2);
                CNsdServiceInfo::New((INsdServiceInfo**)&(clientInfo->mResolvedService));
                StoreRequestMap(arg2, id, clientInfo);
            } else {
                mHost->mHost->ReplyToMessage(msg, INsdManager::RESOLVE_SERVICE_FAILED,
                    INsdManager::FAILURE_INTERNAL_ERROR);
            }
            break;
        }
        case INsdManager::NATIVE_DAEMON_EVENT:
        {
            AutoPtr<IInterface> obj;
            msg->GetObj((IInterface**)&obj);
            AutoPtr<NativeEvent> event = reinterpret_cast<NativeEvent*>(obj->Probe(EIID_NativeEvent));
            mHost->mHost->HandleNativeEvent(event->mCode, event->mRaw,
                NativeDaemonEvent::UnescapeArgs(event->mRaw));
            break;
        }
        default:
            result = StateMachine::NOT_HANDLED;
            break;
    }

    *value = result;
    return NOERROR;
}


//====================================================================
// CNsdService::NativeEvent
//====================================================================
CAR_INTERFACE_IMPL_WITH_CPP_CAST(CNsdService::NativeEvent, IInterface, NativeEvent)

CNsdService::NativeEvent::NativeEvent(
    /* [in] */ Int32 code,
    /* [in] */ const String& raw)
{
    mCode = code;
    mRaw = raw;
}

//====================================================================
// CNsdService::NativeCallbackReceiver
//====================================================================
CAR_INTERFACE_IMPL(CNsdService::NativeCallbackReceiver, INativeDaemonConnectorCallbacks)

CNsdService::NativeCallbackReceiver::NativeCallbackReceiver(
    /* [in] */ CNsdService* owner)
{
    mHost = owner;
}

ECode CNsdService::NativeCallbackReceiver::OnDaemonConnected()
{
    if (CNsdService::DBG)
        Slogger::D("CNsdService", "NativeCallbackReceiver::OnDaemonConnected()");
    return mHost->mNativeDaemonConnected->CountDown();
}

ECode CNsdService::NativeCallbackReceiver::OnEvent(
    /* [in] */ Int32 code,
    /* [in] */ const String& raw,
    /* [in] */ const ArrayOf<String>& cooked,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;
    if (CNsdService::DBG)
        Slogger::D("CNsdService", "NativeCallbackReceiver::OnEvent() code: %d, [%s]",
            code, raw.string());

    // TODO: NDC translates a message to a callback, we could enhance NDC to
    // directly interact with a state machine through messages
    AutoPtr<NativeEvent> event = new NativeEvent(code, raw);
    mHost->mNsdStateMachine->SendMessage(INsdManager::NATIVE_DAEMON_EVENT,
        event->Probe(EIID_IInterface));
    *result = TRUE;
    return NOERROR;
}

//====================================================================
// CNsdService::ClientInfo
//====================================================================
CNsdService::ClientInfo::ClientInfo(
    /* [in] */ AsyncChannel* c,
    /* [in] */ IMessenger* m)
{
    mChannel = c;
    mMessenger = m;
    if (CNsdService::DBG)
        Slogger::E("CNsdService", "New client, channel: %p, messenger: %p", c, m);
}

}//Server
}//Droid
}//Elastos
