
#include "elastos/droid/server/wifi/RttService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Wifi {

//=====================================================================
//      RttService::RttServiceImpl::RttStateMachine::DefaultState
//=====================================================================
Boolean RttService::RttServiceImpl::RttStateMachine::DefaultState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "DefaultState got" + msg);
    // switch (msg.what) {
    //     case CMD_DRIVER_LOADED:
    //         transitionTo(mEnabledState);
    //         break;
    //     case CMD_ISSUE_NEXT_REQUEST:
    //         deferMessage(msg);
    //         break;
    //     case RttManager.CMD_OP_START_RANGING:
    //         replyFailed(msg, RttManager.REASON_NOT_AVAILABLE, "Try later");
    //         break;
    //     case RttManager.CMD_OP_STOP_RANGING:
    //         return HANDLED;
    //     default:
    //         return NOT_HANDLED;
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
}

//=====================================================================
//      RttService::RttServiceImpl::RttStateMachine::EnabledState
//=====================================================================
Boolean RttService::RttServiceImpl::RttStateMachine::EnabledState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "EnabledState got" + msg);
    // ClientInfo ci = mClients.get(msg.replyTo);
    //
    // switch (msg.what) {
    //     case CMD_DRIVER_UNLOADED:
    //         transitionTo(mDefaultState);
    //         break;
    //     case CMD_ISSUE_NEXT_REQUEST:
    //         deferMessage(msg);
    //         transitionTo(mRequestPendingState);
    //         break;
    //     case RttManager.CMD_OP_START_RANGING: {
    //             RttManager.ParcelableRttParams params =
    //                     (RttManager.ParcelableRttParams)msg.obj;
    //             if (params == null) {
    //                 replyFailed(msg,
    //                         RttManager.REASON_INVALID_REQUEST, "No params");
    //             } else if (ci.addRttRequest(msg.arg2, params) == false) {
    //                 replyFailed(msg,
    //                         RttManager.REASON_INVALID_REQUEST, "Unspecified");
    //             } else {
    //                 sendMessage(CMD_ISSUE_NEXT_REQUEST);
    //             }
    //         }
    //         break;
    //     case RttManager.CMD_OP_STOP_RANGING:
    //         for (Iterator<RttRequest> it = mRequestQueue.iterator();
    //                 it.hasNext(); ) {
    //             RttRequest request = it.next();
    //             if (request.key == msg.arg2) {
    //                 if (DBG) Log.d(TAG, "Cancelling not-yet-scheduled RTT");
    //                 mRequestQueue.remove(request);
    //                 request.ci.reportAborted(request.key);
    //                 break;
    //             }
    //         }
    //         break;
    //     default:
    //         return NOT_HANDLED;
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
}

//=====================================================================
//   RttService::RttServiceImpl::RttStateMachine::RequestPendingState
//=====================================================================
Boolean RttService::RttServiceImpl::RttStateMachine::RequestPendingState::ProcessMessage(
    /* [in] */ IMessage* msg)
{
    // ==================before translated======================
    // if (DBG) Log.d(TAG, "RequestPendingState got" + msg);
    // switch (msg.what) {
    //     case CMD_DRIVER_UNLOADED:
    //         if (mOutstandingRequest != null) {
    //             WifiNative.cancelRtt(mOutstandingRequest.params);
    //             mOutstandingRequest.ci.reportAborted(mOutstandingRequest.key);
    //             mOutstandingRequest = null;
    //         }
    //         transitionTo(mDefaultState);
    //         break;
    //     case CMD_ISSUE_NEXT_REQUEST:
    //         if (mOutstandingRequest == null) {
    //             mOutstandingRequest = issueNextRequest();
    //             if (mOutstandingRequest == null) {
    //                 transitionTo(mEnabledState);
    //             }
    //         } else {
    //             /* just wait; we'll issue next request after
    //              * current one is finished */
    //             if (DBG) Log.d(TAG, "Ignoring CMD_ISSUE_NEXT_REQUEST");
    //         }
    //         break;
    //     case CMD_RTT_RESPONSE:
    //         if (DBG) Log.d(TAG, "Received an RTT response");
    //         mOutstandingRequest.ci.reportResult(
    //                 mOutstandingRequest, (RttManager.RttResult[])msg.obj);
    //         mOutstandingRequest = null;
    //         sendMessage(CMD_ISSUE_NEXT_REQUEST);
    //         break;
    //     case RttManager.CMD_OP_STOP_RANGING:
    //         if (mOutstandingRequest != null
    //                 && msg.arg2 == mOutstandingRequest.key) {
    //             if (DBG) Log.d(TAG, "Cancelling ongoing RTT");
    //             WifiNative.cancelRtt(mOutstandingRequest.params);
    //             mOutstandingRequest.ci.reportAborted(mOutstandingRequest.key);
    //             mOutstandingRequest = null;
    //             sendMessage(CMD_ISSUE_NEXT_REQUEST);
    //         } else {
    //             /* Let EnabledState handle this */
    //             return NOT_HANDLED;
    //         }
    //         break;
    //     default:
    //         return NOT_HANDLED;
    // }
    // return HANDLED;
    assert(0);
    return FALSE;
}

//=====================================================================
//             RttService::RttServiceImpl::RttStateMachine
//=====================================================================
RttService::RttServiceImpl::RttStateMachine::RttStateMachine(
    /* [in] */ ILooper* looper)
{
    // ==================before translated======================
    // super("RttStateMachine", looper);
    //
    // addState(mDefaultState);
    // addState(mEnabledState);
    //     addState(mRequestPendingState, mEnabledState);
    //
    // setInitialState(mDefaultState);
}

//=====================================================================
//              RttService::RttServiceImpl::ClientHandler
//=====================================================================
RttService::RttServiceImpl::ClientHandler::ClientHandler(
    /* [in] */  android)
{
    // ==================before translated======================
    // super(looper);
}

ECode RttService::RttServiceImpl::ClientHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    //
    // if (DBG) Log.d(TAG, "ClientHandler got" + msg);
    //
    // switch (msg.what) {
    //
    //     case AsyncChannel.CMD_CHANNEL_HALF_CONNECTED:
    //         if (msg.arg1 == AsyncChannel.STATUS_SUCCESSFUL) {
    //             AsyncChannel c = (AsyncChannel) msg.obj;
    //             if (DBG) Slog.d(TAG, "New client listening to asynchronous messages: " +
    //                     msg.replyTo);
    //             ClientInfo cInfo = new ClientInfo(c, msg.replyTo);
    //             mClients.put(msg.replyTo, cInfo);
    //         } else {
    //             Slog.e(TAG, "Client connection failure, error=" + msg.arg1);
    //         }
    //         return;
    //     case AsyncChannel.CMD_CHANNEL_DISCONNECTED:
    //         if (msg.arg1 == AsyncChannel.STATUS_SEND_UNSUCCESSFUL) {
    //             Slog.e(TAG, "Send failed, client connection lost");
    //         } else {
    //             if (DBG) Slog.d(TAG, "Client connection lost with reason: " + msg.arg1);
    //         }
    //         if (DBG) Slog.d(TAG, "closing client " + msg.replyTo);
    //         ClientInfo ci = mClients.remove(msg.replyTo);
    //         ci.cleanup();
    //         return;
    //     case AsyncChannel.CMD_CHANNEL_FULL_CONNECTION:
    //         AsyncChannel ac = new AsyncChannel();
    //         ac.connect(mContext, this, msg.replyTo);
    //         return;
    // }
    //
    // ClientInfo ci = mClients.get(msg.replyTo);
    // if (ci == null) {
    //     Slog.e(TAG, "Could not find client info for message " + msg.replyTo);
    //     replyFailed(msg, RttManager.REASON_INVALID_LISTENER, "Could not find listener");
    //     return;
    // }
    //
    // int validCommands[] = {
    //         RttManager.CMD_OP_START_RANGING,
    //         RttManager.CMD_OP_STOP_RANGING
    //         };
    //
    // for(int cmd : validCommands) {
    //     if (cmd == msg.what) {
    //         mStateMachine.sendMessage(Message.obtain(msg));
    //         return;
    //     }
    // }
    //
    // replyFailed(msg, RttManager.REASON_INVALID_REQUEST, "Invalid request");
    assert(0);
    return NOERROR;
}

//=====================================================================
//         RttService::RttServiceImpl::InnerBroadcastReceiver1
//=====================================================================
RttService::RttServiceImpl::InnerBroadcastReceiver1::InnerBroadcastReceiver1(
    /* [in] */ RttServiceImpl* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode RttService::RttServiceImpl::InnerBroadcastReceiver1::OnReceive(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(context);
    VALIDATE_NOT_NULL(intent);
    // ==================before translated======================
    // int state = intent.getIntExtra(
    //         WifiManager.EXTRA_SCAN_AVAILABLE, WifiManager.WIFI_STATE_DISABLED);
    // if (DBG) Log.d(TAG, "SCAN_AVAILABLE : " + state);
    // if (state == WifiManager.WIFI_STATE_ENABLED) {
    //     mStateMachine.sendMessage(CMD_DRIVER_LOADED);
    // } else if (state == WifiManager.WIFI_STATE_DISABLED) {
    //     mStateMachine.sendMessage(CMD_DRIVER_UNLOADED);
    // }
    assert(0);
    return NOERROR;
}

//=====================================================================
//                RttService::RttServiceImpl::ClientInfo
//=====================================================================
RttService::RttServiceImpl::ClientInfo::ClientInfo(
    /* [in] */ IAsyncChannel* c,
    /* [in] */ IMessenger* m)
{
    // ==================before translated======================
    // mChannel = c;
    // mMessenger = m;
}

ECode RttService::RttServiceImpl::ClientInfo::AddRttRequest(
    /* [in] */ Int32 key,
    /* [in] */  RttManager,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if (parcelableParams == null) {
    //     return false;
    // }
    //
    // RttManager.RttParams params[] = parcelableParams.mParams;
    //
    // RttRequest request = new RttRequest();
    // request.key = key;
    // request.ci = this;
    // request.params = params;
    // mRequests.put(key, request);
    // mRequestQueue.add(request);
    // return true;
    assert(0);
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::RemoveRttRequest(
    /* [in] */ Int32 key)
{
    // ==================before translated======================
    // mRequests.remove(key);
    assert(0);
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::ReportResult(
    /* [in] */ RttRequest* request,
    /* [in] */  RttManager)
{
    VALIDATE_NOT_NULL(request);
    // ==================before translated======================
    // RttManager.ParcelableRttResults parcelableResults =
    //         new RttManager.ParcelableRttResults(results);
    //
    // mChannel.sendMessage(RttManager.CMD_OP_SUCCEEDED,
    //         0, request.key, parcelableResults);
    // mRequests.remove(request.key);
    assert(0);
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::ReportFailed(
    /* [in] */ RttRequest* request,
    /* [in] */ Int32 reason,
    /* [in] */ String description)
{
    VALIDATE_NOT_NULL(request);
    // ==================before translated======================
    // reportFailed(request.key, reason, description);
    assert(0);
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::ReportFailed(
    /* [in] */ Int32 key,
    /* [in] */ Int32 reason,
    /* [in] */ String description)
{
    // ==================before translated======================
    // Bundle bundle = new Bundle();
    // bundle.putString(RttManager.DESCRIPTION_KEY, description);
    // mChannel.sendMessage(RttManager.CMD_OP_FAILED, key, reason, bundle);
    // mRequests.remove(key);
    assert(0);
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::ReportAborted(
    /* [in] */ Int32 key)
{
    // ==================before translated======================
    // mChannel.sendMessage(RttManager.CMD_OP_ABORTED, key);
    // mRequests.remove(key);
    assert(0);
    return NOERROR;
}

ECode RttService::RttServiceImpl::ClientInfo::Cleanup()
{
    // ==================before translated======================
    // mRequests.clear();
    assert(0);
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

RttService::RttServiceImpl::RttServiceImpl()
{
}

RttService::RttServiceImpl::RttServiceImpl(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // mContext = context;
}

AutoPtr<IMessenger> RttService::RttServiceImpl::GetMessenger()
{
    // ==================before translated======================
    // return new Messenger(mClientHandler);
    assert(0);
    AutoPtr<IMessenger> empty;
    return empty;
}

ECode RttService::RttServiceImpl::StartService(
    /* [in] */ IContext* context)
{
    VALIDATE_NOT_NULL(context);
    // ==================before translated======================
    // mContext = context;
    //
    // HandlerThread thread = new HandlerThread("WifiRttService");
    // thread.start();
    //
    // mClientHandler = new ClientHandler(thread.getLooper());
    // mStateMachine = new RttStateMachine(thread.getLooper());
    //
    // mContext.registerReceiver(
    //         new BroadcastReceiver() {
    //             @Override
    //             public void onReceive(Context context, Intent intent) {
    //                 int state = intent.getIntExtra(
    //                         WifiManager.EXTRA_SCAN_AVAILABLE, WifiManager.WIFI_STATE_DISABLED);
    //                 if (DBG) Log.d(TAG, "SCAN_AVAILABLE : " + state);
    //                 if (state == WifiManager.WIFI_STATE_ENABLED) {
    //                     mStateMachine.sendMessage(CMD_DRIVER_LOADED);
    //                 } else if (state == WifiManager.WIFI_STATE_DISABLED) {
    //                     mStateMachine.sendMessage(CMD_DRIVER_UNLOADED);
    //                 }
    //             }
    //         }, new IntentFilter(WifiManager.WIFI_SCAN_AVAILABLE));
    //
    // mStateMachine.start();
    assert(0);
    return NOERROR;
}

ECode RttService::RttServiceImpl::ReplySucceeded(
    /* [in] */ IMessage* msg,
    /* [in] */ Object* obj)
{
    VALIDATE_NOT_NULL(msg);
    VALIDATE_NOT_NULL(obj);
    // ==================before translated======================
    // if (msg.replyTo != null) {
    //     Message reply = Message.obtain();
    //     reply.what = RttManager.CMD_OP_SUCCEEDED;
    //     reply.arg2 = msg.arg2;
    //     reply.obj = obj;
    //     try {
    //         msg.replyTo.send(reply);
    //     } catch (RemoteException e) {
    //         // There's not much we can do if reply can't be sent!
    //     }
    // } else {
    //     // locally generated message; doesn't need a reply!
    // }
    assert(0);
    return NOERROR;
}

ECode RttService::RttServiceImpl::ReplyFailed(
    /* [in] */ IMessage* msg,
    /* [in] */ Int32 reason,
    /* [in] */ String description)
{
    VALIDATE_NOT_NULL(msg);
    // ==================before translated======================
    // Message reply = Message.obtain();
    // reply.what = RttManager.CMD_OP_FAILED;
    // reply.arg1 = reason;
    // reply.arg2 = msg.arg2;
    //
    // Bundle bundle = new Bundle();
    // bundle.putString(RttManager.DESCRIPTION_KEY, description);
    // reply.obj = bundle;
    //
    // try {
    //     msg.replyTo.send(reply);
    // } catch (RemoteException e) {
    //     // There's not much we can do if reply can't be sent!
    // }
    assert(0);
    return NOERROR;
}

ECode RttService::RttServiceImpl::IssueNextRequest(
    /* [out] */ RttRequest** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // RttRequest request = null;
    // while (mRequestQueue.isEmpty() == false) {
    //     request = mRequestQueue.remove();
    //     if (WifiNative.requestRtt(request.params, mEventHandler)) {
    //         if (DBG) Log.d(TAG, "Issued next RTT request");
    //         return request;
    //     } else {
    //         request.ci.reportFailed(request,
    //                 RttManager.REASON_UNSPECIFIED, "Failed to start");
    //     }
    // }
    //
    // /* all requests exhausted */
    // if (DBG) Log.d(TAG, "No more requests left");
    // return null;
    assert(0);
    return NOERROR;
}

//=====================================================================
//              RttService::InnerWifiNativeRttEventHandler
//=====================================================================
RttService::InnerWifiNativeRttEventHandler::InnerWifiNativeRttEventHandler(
    /* [in] */ RttServiceImpl* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // mOwner = owner;
}

ECode RttService::InnerWifiNativeRttEventHandler::OnRttResults(
    /* [in] */  RttManager)
{
    // ==================before translated======================
    // mStateMachine.sendMessage(CMD_RTT_RESPONSE, result);
    assert(0);
    return NOERROR;
}

//=====================================================================
//                              RttService
//=====================================================================
const Boolean RttService::DBG = true;
const String RttService::TAG("RttService");

RttService::RttService(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // super(context);
    // Log.i(TAG, "Creating " + Context.WIFI_RTT_SERVICE);
}

ECode RttService::OnStart()
{
    // ==================before translated======================
    // mImpl = new RttServiceImpl(getContext());
    //
    // Log.i(TAG, "Starting " + Context.WIFI_RTT_SERVICE);
    // publishBinderService(Context.WIFI_RTT_SERVICE, mImpl);
    assert(0);
    return NOERROR;
}

ECode RttService::OnBootPhase(
    /* [in] */ Int32 phase)
{
    // ==================before translated======================
    // if (phase == SystemService.PHASE_SYSTEM_SERVICES_READY) {
    //     Log.i(TAG, "Registering " + Context.WIFI_RTT_SERVICE);
    //     if (mImpl == null) {
    //         mImpl = new RttServiceImpl(getContext());
    //     }
    //     mImpl.startService(getContext());
    // }
    assert(0);
    return NOERROR;
}

} // namespace Wifi
} // namespace Server
} // namespace Droid
} // namespace Elastos


