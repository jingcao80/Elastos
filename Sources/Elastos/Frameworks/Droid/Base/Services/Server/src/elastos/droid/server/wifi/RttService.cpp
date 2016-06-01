
#include "elastos/droid/server/wifi/RttService.h"
#include "elastos/droid/server/wifi/CRttServiceImpl.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Wifi::EIID_IIRttManager;
using Elastos::Droid::Wifi::IRttManager;
using Elastos::Droid::Wifi::IWifiManager;
using Elastos::Droid::Wifi::CRttManagerParcelableRttResults;
using Elastos::Droid::Wifi::IRttManagerParcelableRttResults;
using Elastos::Droid::Wifi::CRttManagerParcelableRttResults;
using Elastos::Core::IThread;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CLinkedList;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//      RttService::RttServiceImpl::RttStateMachine::DefaultState
//=====================================================================
RttService::RttServiceImpl::RttStateMachine::DefaultState::DefaultState(
    /* [in] */ RttStateMachine* host)
    : mHost(host)
{}

ECode RttService::RttServiceImpl::RttStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logger::D(TAG, "DefaultState got%p", msg);
    Int32 what = 0;
    msg->GetWhat(&what);
    switch (what) {
        case CMD_DRIVER_LOADED:
            mHost->TransitionTo(mHost->mEnabledState);
            break;
        case CMD_ISSUE_NEXT_REQUEST:
            mHost->DeferMessage(msg);
            break;
        case IRttManager::CMD_OP_START_RANGING:
            mHost->mHost->ReplyFailed(msg, IRttManager::REASON_NOT_AVAILABLE, String("Try later"));
            break;
        case IRttManager::CMD_OP_STOP_RANGING:
            *result = HANDLED;
            return NOERROR;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String RttService::RttServiceImpl::RttStateMachine::DefaultState::GetName()
{
    String name("DefaultState"); // getClass().getName();
    Int32 lastDollar = name.LastIndexOf('$');
    return name.Substring(lastDollar + 1);
}

//=====================================================================
//      RttService::RttServiceImpl::RttStateMachine::EnabledState
//=====================================================================
RttService::RttServiceImpl::RttStateMachine::EnabledState::EnabledState(
    /* [in] */ RttStateMachine* host)
    : mHost(host)
{}

ECode RttService::RttServiceImpl::RttStateMachine::EnabledState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logger::D(TAG, "EnabledState got%p", msg);
    AutoPtr<IMessenger> rt;
    msg->GetReplyTo((IMessenger**)&rt);
    AutoPtr<IInterface> p;
    mHost->mHost->mClients->Get(rt, (IInterface**)&p);
    AutoPtr<ClientInfo> ci = (ClientInfo*)IObject::Probe(p);

    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Int32 arg2 = 0;
    msg->GetArg2(&arg2);
    AutoPtr<IIterator> it;
    Boolean bHasNxt = FALSE;
    switch (what) {
        case CMD_DRIVER_UNLOADED:
            mHost->TransitionTo(mHost->mDefaultState);
            break;
        case CMD_ISSUE_NEXT_REQUEST:
            mHost->DeferMessage(msg);
            mHost->TransitionTo(mHost->mRequestPendingState);
            break;
        case IRttManager::CMD_OP_START_RANGING: {
                AutoPtr<IRttManagerParcelableRttParams> params =
                        IRttManagerParcelableRttParams::Probe(obj);
                Boolean bAdd = FALSE;
                if (params == NULL) {
                    mHost->mHost->ReplyFailed(msg,
                            IRttManager::REASON_INVALID_REQUEST, String("No params"));
                }
                else if ((ci->AddRttRequest(arg2, params, &bAdd), bAdd) == FALSE) {
                    mHost->mHost->ReplyFailed(msg,
                            IRttManager::REASON_INVALID_REQUEST, String("Unspecified"));
                }
                else {
                    mHost->SendMessage(CMD_ISSUE_NEXT_REQUEST);
                }
            }
            break;
        case IRttManager::CMD_OP_STOP_RANGING:
            for (mHost->mHost->mRequestQueue->GetIterator((IIterator**)&it);
                    (it->HasNext(&bHasNxt), bHasNxt); ) {
                AutoPtr<IInterface> p;
                it->GetNext((IInterface**)&p);
                AutoPtr<RttRequest> request = (RttRequest*)IObject::Probe(p);
                Int32 val = 0;
                request->mKey->GetValue(&val);
                if (val == arg2) {
                    if (DBG) Logger::D(TAG, "Cancelling not-yet-scheduled RTT");
                    Boolean b = FALSE;
                    mHost->mHost->mRequestQueue->Remove((IInterface*)(IObject*)request.Get(), &b);
                    request->mCi->ReportAborted(val);
                    break;
                }
            }
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String RttService::RttServiceImpl::RttStateMachine::EnabledState::GetName()
{
    String name("EnabledState"); // getClass().getName();
    Int32 lastDollar = name.LastIndexOf('$');
    return name.Substring(lastDollar + 1);
}

//=====================================================================
//   RttService::RttServiceImpl::RttStateMachine::RequestPendingState
//=====================================================================
RttService::RttServiceImpl::RttStateMachine::RequestPendingState::RequestPendingState(
    /* [in] */ RttStateMachine* host)
    : mHost(host)
{}

ECode RttService::RttServiceImpl::RttStateMachine::RequestPendingState::ProcessMessage(
    /* [in] */ IMessage* msg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (DBG) Logger::D(TAG, "RequestPendingState got%p", msg);
    Int32 what = 0;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Int32 arg2 = 0;
    msg->GetArg2(&arg2);
    Int32 val = 0;
    switch (what) {
        case CMD_DRIVER_UNLOADED:
            if (mOutstandingRequest != NULL) {
                WifiNative::CancelRtt(mOutstandingRequest->mParams);
                mOutstandingRequest->mKey->GetValue(&val);
                mOutstandingRequest->mCi->ReportAborted(val);
                mOutstandingRequest = NULL;
            }
            mHost->TransitionTo(mHost->mDefaultState);
            break;
        case CMD_ISSUE_NEXT_REQUEST:
            if (mOutstandingRequest == NULL) {
                mHost->mHost->IssueNextRequest((RttRequest**)&mOutstandingRequest);
                if (mOutstandingRequest == NULL) {
                    mHost->TransitionTo(mHost->mEnabledState);
                }
            }
            else {
                /* just wait; we'll issue next request after
                 * current one is finished */
                if (DBG) Logger::D(TAG, "Ignoring CMD_ISSUE_NEXT_REQUEST");
            }
            break;
        case CMD_RTT_RESPONSE:
            if (DBG) Logger::D(TAG, "Received an RTT response");
            assert(0 && "TODO");
            // mOutstandingRequest->mCi->ReportResult(
            //         mOutstandingRequest, (ArrayOf<IRttManagerRttResult*>*)obj);
            mOutstandingRequest = NULL;
            mHost->SendMessage(CMD_ISSUE_NEXT_REQUEST);
            break;
        case IRttManager::CMD_OP_STOP_RANGING:
            mOutstandingRequest->mKey->GetValue(&val);
            if (mOutstandingRequest != NULL
                    && arg2 == val) {
                if (DBG) Logger::D(TAG, "Cancelling ongoing RTT");
                WifiNative::CancelRtt(mOutstandingRequest->mParams);
                mOutstandingRequest->mCi->ReportAborted(val);
                mOutstandingRequest = NULL;
                mHost->SendMessage(CMD_ISSUE_NEXT_REQUEST);
            }
            else {
                /* Let EnabledState handle this */
                *result = NOT_HANDLED;
                return NOERROR;
            }
            break;
        default:
            *result = NOT_HANDLED;
            return NOERROR;
    }
    *result = HANDLED;
    return NOERROR;
}

String RttService::RttServiceImpl::RttStateMachine::RequestPendingState::GetName()
{
    String name("RequestPendingState"); // getClass().getName();
    Int32 lastDollar = name.LastIndexOf('$');
    return name.Substring(lastDollar + 1);
}

//=====================================================================
//             RttService::RttServiceImpl::RttStateMachine
//=====================================================================
RttService::RttServiceImpl::RttStateMachine::RttStateMachine(
    /* [in] */ ILooper* looper,
    /* [in] */ RttServiceImpl* host)
    : mHost(host)
{
    StateMachine::constructor(String("RttStateMachine"), looper);
    mDefaultState = new DefaultState(this);
    mEnabledState = new EnabledState(this);
    mRequestPendingState = new RequestPendingState(this);

    AddState(mDefaultState);
    AddState(mEnabledState);
    AddState(mRequestPendingState, mEnabledState);

    SetInitialState(mDefaultState);
}

//=====================================================================
//              RttService::RttServiceImpl::ClientHandler
//=====================================================================
RttService::RttServiceImpl::ClientHandler::ClientHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ RttServiceImpl* host)
    : Handler(looper)
    , mHost(host)
{
}

ECode RttService::RttServiceImpl::ClientHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);

    if (DBG) Logger::D(TAG, "ClientHandler got%p", msg);

    Int32 what = 0;
    msg->GetWhat(&what);
    Int32 arg1 = 0;
    msg->GetArg1(&arg1);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_HALF_CONNECTED:
            if (arg1 == IAsyncChannel::STATUS_SUCCESSFUL) {
                AutoPtr<IAsyncChannel> c = IAsyncChannel::Probe(obj);
                if (DBG) Slogger::D(TAG, "New client listening to asynchronous messages: %p",
                        replyTo.Get());
                AutoPtr<ClientInfo> cInfo = new ClientInfo(c, replyTo, mHost);
                mHost->mClients->Put(replyTo, (IInterface*)(IObject*)cInfo.Get());
            }
            else {
                Slogger::E(TAG, "Client connection failure, error=%d", arg1);
            }
            return NOERROR;
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            if (arg1 == IAsyncChannel::STATUS_SEND_UNSUCCESSFUL) {
                Slogger::E(TAG, "Send failed, client connection lost");
            }
            else {
                if (DBG) Slogger::D(TAG, "Client connection lost with reason: %d", arg1);
            }
            if (DBG) Slogger::D(TAG, "closing client %p", replyTo.Get());
            AutoPtr<IInterface> p;
            mHost->mClients->Remove(replyTo, (IInterface**)&p);
            AutoPtr<ClientInfo> pCi = (ClientInfo*)IObject::Probe(p);
            pCi->Cleanup();
            return NOERROR;
        }
        case IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION: {
            AutoPtr<IAsyncChannel> ac;
            CAsyncChannel::New((IAsyncChannel**)&ac);
            ac->Connect(mHost->mContext, this, replyTo);
            return NOERROR;
        }
    }

    AutoPtr<IInterface> _ci;
    mHost->mClients->Get(replyTo, (IInterface**)&_ci);
    AutoPtr<ClientInfo> ci = (ClientInfo*)IObject::Probe(_ci);
    if (ci == NULL) {
        Slogger::E(TAG, "Could not find client info for message %p", replyTo.Get());
        mHost->ReplyFailed(msg, IRttManager::REASON_INVALID_LISTENER, String("Could not find listener"));
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > validCommands;
    (*validCommands)[0] = IRttManager::CMD_OP_START_RANGING;
    (*validCommands)[1] = IRttManager::CMD_OP_STOP_RANGING;

    for(Int32 i = 0; i < validCommands->GetLength(); i++) {
        Int32 cmd = (*validCommands)[i];
        if (cmd == what) {
            AutoPtr<IMessageHelper> hlp;
            CMessageHelper::AcquireSingleton((IMessageHelper**)&hlp);
            AutoPtr<IMessage> senMsg;
            hlp->Obtain(msg, (IMessage**)&senMsg);
            mHost->mStateMachine->SendMessage(senMsg);
            return NOERROR;
        }
    }

    mHost->ReplyFailed(msg, IRttManager::REASON_INVALID_REQUEST, String("Invalid request"));
    return NOERROR;
}

//=====================================================================
//         RttService::RttServiceImpl::InnerBroadcastReceiver1
//=====================================================================
RttService::RttServiceImpl::InnerBroadcastReceiver1::InnerBroadcastReceiver1(
    /* [in] */ RttServiceImpl* owner)
    : mOwner(owner)
{
}

ECode RttService::RttServiceImpl::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);

    Int32 state = 0;
    intent->GetInt32Extra(
            IWifiManager::EXTRA_SCAN_AVAILABLE, IWifiManager::WIFI_STATE_DISABLED, &state);
    if (DBG) Logger::D(TAG, "SCAN_AVAILABLE : %d", state);
    if (state == IWifiManager::WIFI_STATE_ENABLED) {
        mOwner->mStateMachine->SendMessage(CMD_DRIVER_LOADED);
    }
    else if (state == IWifiManager::WIFI_STATE_DISABLED) {
        mOwner->mStateMachine->SendMessage(CMD_DRIVER_UNLOADED);
    }
    return NOERROR;
}

//=====================================================================
//                RttService::RttServiceImpl::ClientInfo
//=====================================================================
RttService::RttServiceImpl::ClientInfo::ClientInfo(
    /* [in] */ IAsyncChannel* c,
    /* [in] */ IMessenger* m,
    /* [in] */ RttServiceImpl* host)
    : mHost(host)
{
    mChannel = c;
    mMessenger = m;
    CHashMap::New((IHashMap**)&mRequests);
}

ECode RttService::RttServiceImpl::ClientInfo::AddRttRequest(
    /* [in] */ Int32 key,
    /* [in] */ IRttManagerParcelableRttParams* parcelableParams,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (parcelableParams == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ArrayOf<IRttManagerRttParams*> > params;
    parcelableParams->GetParams((ArrayOf<IRttManagerRttParams*>**)&params);

    AutoPtr<RttRequest> request = new RttRequest();
    request->mKey = CoreUtils::Convert(key);
    request->mCi = this;
    request->mParams = params;
    mRequests->Put(CoreUtils::Convert(key), (IInterface*)(IObject*)request.Get());
    mHost->mRequestQueue->Add((IInterface*)(IObject*)request.Get());
    *result = TRUE;
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::RemoveRttRequest(
    /* [in] */ Int32 key)
{
    mRequests->Remove(CoreUtils::Convert(key));
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::ReportResult(
    /* [in] */ RttRequest* request,
    /* [in] */ ArrayOf<IRttManagerRttResult*>* results)
{
    VALIDATE_NOT_NULL(request);
    AutoPtr<IRttManagerParcelableRttResults> parcelableResults;
    CRttManagerParcelableRttResults::New(results, (IRttManagerParcelableRttResults**)&parcelableResults);

    Int32 val = 0;
    request->mKey->GetValue(&val);
    mChannel->SendMessage(IRttManager::CMD_OP_SUCCEEDED,
            0, val, parcelableResults);
    mRequests->Remove(request->mKey);
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::ReportFailed(
    /* [in] */ RttRequest* request,
    /* [in] */ Int32 reason,
    /* [in] */ const String& description)
{
    VALIDATE_NOT_NULL(request);
    Int32 val = 0;
    request->mKey->GetValue(&val);
    ReportFailed(val, reason, description);
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::ReportFailed(
    /* [in] */ Int32 key,
    /* [in] */ Int32 reason,
    /* [in] */ String description)
{
    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(IRttManager::DESCRIPTION_KEY, description);
    mChannel->SendMessage(IRttManager::CMD_OP_FAILED, key, reason, bundle);
    mRequests->Remove(CoreUtils::Convert(key));
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::ReportAborted(
    /* [in] */ Int32 key)
{
    mChannel->SendMessage(IRttManager::CMD_OP_ABORTED, key);
    mRequests->Remove(CoreUtils::Convert(key));
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::Cleanup()
{
    mRequests->Clear();
    return NOERROR;
}

//=====================================================================
//                      RttService::RttServiceImpl
//=====================================================================
const Int32 RttService::RttServiceImpl::BASE;
const Int32 RttService::RttServiceImpl::CMD_DRIVER_LOADED;
const Int32 RttService::RttServiceImpl::CMD_DRIVER_UNLOADED;
const Int32 RttService::RttServiceImpl::CMD_ISSUE_NEXT_REQUEST;
const Int32 RttService::RttServiceImpl::CMD_RTT_RESPONSE;

CAR_INTERFACE_IMPL_2(RttService::RttServiceImpl, Object, IIRttManager, IBinder);

RttService::RttServiceImpl::RttServiceImpl()
{
    AutoPtr<ILinkedList> ll;
    CLinkedList::New((ILinkedList**)&ll);
    mRequestQueue = IQueue::Probe(ll);
    CHashMap::New(4, (IHashMap**)&mClients);
}

ECode RttService::RttServiceImpl::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    return NOERROR;
}

ECode RttService::RttServiceImpl::GetMessenger(
    /* [out] */ IMessenger** messenger)
{
    VALIDATE_NOT_NULL(messenger)
    AutoPtr<IMessenger> msg;
    CMessenger::New(IHandler::Probe(mClientHandler), (IMessenger**)&msg);
    *messenger = msg.Get();
    REFCOUNT_ADD(*messenger)
    return NOERROR;
}

ECode RttService::RttServiceImpl::StartService(
    /* [in] */ IContext* context)
{
    VALIDATE_NOT_NULL(context);
    mContext = context;

    AutoPtr<IHandlerThread> thread;
    CHandlerThread::New(String("WifiRttService"), (IHandlerThread**)&thread);
    IThread::Probe(thread)->Start();

    AutoPtr<ILooper> lp;
    thread->GetLooper((ILooper**)&lp);
    mClientHandler = new ClientHandler(lp, this);
    mStateMachine = new RttStateMachine(lp, this);

    AutoPtr<IIntentFilter> ifl;
    CIntentFilter::New(IWifiManager::WIFI_SCAN_AVAILABLE, (IIntentFilter**)&ifl);
    AutoPtr<IIntent> res;
    AutoPtr<IBroadcastReceiver> br = new InnerBroadcastReceiver1(this);
    mContext->RegisterReceiver(br, ifl, (IIntent**)&res);

    mStateMachine->Start();
    return NOERROR;
}

ECode RttService::RttServiceImpl::ReplySucceeded(
    /* [in] */ IMessage* msg,
    /* [in] */ IObject* obj)
{
    VALIDATE_NOT_NULL(msg);
    VALIDATE_NOT_NULL(obj);

    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    if (replyTo != NULL) {
        AutoPtr<IMessageHelper> hlp;
        CMessageHelper::AcquireSingleton((IMessageHelper**)&hlp);
        AutoPtr<IMessage> reply;
        hlp->Obtain((IMessage**)&reply);
        reply->SetWhat(IRttManager::CMD_OP_SUCCEEDED);
        Int32 arg2 = 0;
        msg->GetArg2(&arg2);
        reply->SetArg2(arg2);
        reply->SetObj(obj);

        replyTo->Send(reply);
    }
    else {
        // locally generated message; doesn't need a reply!
    }
    return NOERROR;
}

ECode RttService::RttServiceImpl::ReplyFailed(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 reason,
    /* [in] */ const String& description)
{
    VALIDATE_NOT_NULL(msg);
    AutoPtr<IMessageHelper> hlp;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&hlp);
    AutoPtr<IMessage> reply;
    hlp->Obtain((IMessage**)&reply);
    reply->SetWhat(IRttManager::CMD_OP_FAILED);
    reply->SetArg1(reason);
    Int32 arg2 = 0;
    msg->GetArg2(&arg2);
    reply->SetArg2(arg2);

    AutoPtr<IBundle> bundle;
    CBundle::New((IBundle**)&bundle);
    bundle->PutString(IRttManager::DESCRIPTION_KEY, description);
    reply->SetObj(bundle);

    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    replyTo->Send(reply);
    return NOERROR;
}

ECode RttService::RttServiceImpl::IssueNextRequest(
    /* [out] */ RttRequest** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<RttRequest> request;
    Boolean bEmp = FALSE;
    while ((mRequestQueue->IsEmpty(&bEmp), bEmp == FALSE)) {
        AutoPtr<IInterface> _request;
        mRequestQueue->Remove((IInterface**)&_request);
        request = (RttRequest*)IObject::Probe(_request);
        if (WifiNative::RequestRtt(request->mParams, mEventHandler)) {
            if (DBG) Logger::D(TAG, "Issued next RTT request");
            *result = request;
            REFCOUNT_ADD(*result)
            return NOERROR;
        }
        else {
            request->mCi->ReportFailed(request,
                    IRttManager::REASON_UNSPECIFIED, String("Failed to start"));
        }
    }

    /* all requests exhausted */
    if (DBG) Logger::D(TAG, "No more requests left");
    *result = NULL;
    return NOERROR;
}

//=====================================================================
//              RttService::InnerWifiNativeRttEventHandler
//=====================================================================
CAR_INTERFACE_IMPL(RttService::InnerWifiNativeRttEventHandler, Object, IWifiNativeRttEventHandler);

RttService::InnerWifiNativeRttEventHandler::InnerWifiNativeRttEventHandler(
    /* [in] */ RttServiceImpl* owner)
    : mOwner(owner)
{
}

ECode RttService::InnerWifiNativeRttEventHandler::OnRttResults(
    /* [in] */ ArrayOf<IRttManagerRttResult*>* result)
{
    assert(0 && "TODO");
    // mOwner->mStateMachine->SendMessage(RttServiceImpl::CMD_RTT_RESPONSE, result);
    return NOERROR;
}

//=====================================================================
//                              RttService
//=====================================================================
const Boolean RttService::DBG = TRUE;
const String RttService::TAG("RttService");

RttService::RttService()
{
}

ECode RttService::constructor(
    /* [in] */ IContext* context)
{
    SystemService::constructor(context);
    Logger::I(TAG, "Creating %s", IContext::WIFI_RTT_SERVICE.string());
    return NOERROR;
}

ECode RttService::OnStart()
{
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<IIRttManager> rttManager;
    CRttServiceImpl::New(ctx, (IIRttManager**)&rttManager);
    mImpl = (RttService::RttServiceImpl*)rttManager.Get();

    Logger::I(TAG, "Starting %s", IContext::WIFI_RTT_SERVICE.string());
    PublishBinderService(IContext::WIFI_RTT_SERVICE, IBinder::Probe(mImpl));
    return NOERROR;
}

ECode RttService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    if (phase == ISystemService::PHASE_SYSTEM_SERVICES_READY) {
        Logger::I(TAG, "Registering %s", IContext::WIFI_RTT_SERVICE.string());
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        if (mImpl == NULL) {
            AutoPtr<IIRttManager> rttManager;
            CRttServiceImpl::New(ctx, (IIRttManager**)&rttManager);
            mImpl = (RttService::RttServiceImpl*)rttManager.Get();
        }
        mImpl->StartService(ctx);
    }
    return NOERROR;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos
