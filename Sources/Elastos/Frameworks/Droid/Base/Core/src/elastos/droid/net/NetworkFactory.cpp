
#include "elastos/droid/internal/utility/CAsyncChannel.h"
#include "elastos/droid/net/NetworkFactory.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CNetworkCapabilities.h"
#include "elastos/droid/net/CConnectivityManager.h"
#include "elastos/droid/net/Network.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/utility/CSparseArray.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Utility::CSparseArray;

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(NetworkFactory, Handler, INetworkFactory)

const Boolean NetworkFactory::DBG = TRUE;
const Boolean NetworkFactory::VDBG = FALSE;
const Int32 NetworkFactory::BASE = IProtocol::BASE_NETWORK_FACTORY;
const Int32 NetworkFactory::CMD_SET_SCORE = BASE + 2;
const Int32 NetworkFactory::CMD_SET_FILTER = BASE + 3;

AutoPtr<ISparseArray> NetworkFactory::CreateSparseArray()
{
    AutoPtr<ISparseArray> rev;
    CSparseArray::New((ISparseArray**)&rev);
    return rev;
}

NetworkFactory::NetworkFactory()
    : mRefCount(0)
    , mMessenger(NULL)
{
    mNetworkRequests = CreateSparseArray();
}

ECode NetworkFactory::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ IContext* context,
    /* [in] */ const String& logTag,
    /* [in] */ INetworkCapabilities* filter)
{
    Handler::constructor(looper);
    mLOG_TAG = logTag;
    mContext = context;
    mCapabilityFilter = filter;
    return NOERROR;
}

ECode NetworkFactory::Register()
{
    if (DBG) Logger::E(mLOG_TAG, "Registering NetworkFactory");
    if (mMessenger == NULL) {
        CMessenger::New(this, (IMessenger**)&mMessenger);
        AutoPtr<IConnectivityManager> connectivityManager;
        CConnectivityManager::From(mContext, (IConnectivityManager**)&connectivityManager);
        connectivityManager->RegisterNetworkFactory(mMessenger, mLOG_TAG);
    }
    return NOERROR;
}

ECode NetworkFactory::Unregister()
{
    if (DBG) Logger::E(mLOG_TAG, "Unregistering NetworkFactory");
    if (mMessenger != NULL) {
        AutoPtr<IConnectivityManager> connectivityManager;
        CConnectivityManager::From(mContext, (IConnectivityManager**)&connectivityManager);
        connectivityManager->UnregisterNetworkFactory(mMessenger);
        mMessenger = NULL;
    }
    return NOERROR;
}

ECode NetworkFactory::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    Int32 arg1;
    msg->GetArg1(&arg1);
    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION: {
            if (mAsyncChannel != NULL) {
                Logger::E(mLOG_TAG, "asyncchannel is connected");
            }
            else {
                AutoPtr<IMessenger> replyTo;
                msg->GetReplyTo((IMessenger**)&replyTo);
                AutoPtr<IAsyncChannel> ac;
                CAsyncChannel::New((IAsyncChannel**)&ac);
                ac->Connected(NULL, this, replyTo);
                ac->ReplyToMessage(msg, IAsyncChannel::CMD_CHANNEL_FULLY_CONNECTED,
                        IAsyncChannel::STATUS_SUCCESSFUL);
                mAsyncChannel = ac;
            }
            break;
        }
        case IAsyncChannel::CMD_CHANNEL_DISCONNECT: {
            if (mAsyncChannel != NULL) {
                mAsyncChannel->Disconnect();
            }
            break;
        }
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            mAsyncChannel = NULL;
            break;
        }
        case CMD_REQUEST_NETWORK: {
            HandleAddRequest(INetworkRequest::Probe(obj), arg1);
            break;
        }
        case CMD_CANCEL_REQUEST: {
            HandleRemoveRequest(INetworkRequest::Probe(obj));
            break;
        }
        case CMD_SET_SCORE: {
            HandleSetScore(arg1);
            break;
        }
        case CMD_SET_FILTER: {
            HandleSetFilter(INetworkCapabilities::Probe(obj));
            break;
        }
    }
    return NOERROR;
}

ECode NetworkFactory::HandleAddRequest(
    /* [in] */ INetworkRequest* request,
    /* [in] */ Int32 score)
{
    Int32 requestID;
    request->GetRequestId(&requestID);
    AutoPtr<IInterface> obj;
    mNetworkRequests->Get(requestID, (IInterface**)&obj);
    AutoPtr<NetworkRequestInfo> n = (NetworkRequestInfo*) IObject::Probe(obj);
    if (n == NULL) {
        String s;
        IObject::Probe(request)->ToString(&s);
        if (DBG) Logger::E(mLOG_TAG, "got request %s with score %d", s.string(), score);
        n = new NetworkRequestInfo(request, score);
        Int32 requestId;
        n->mRequest->GetRequestId(&requestId);
        mNetworkRequests->Put(requestId, TO_IINTERFACE(n.Get()));
    } else {
        String s;
        IObject::Probe(request)->ToString(&s);
        if (VDBG) Logger::E(mLOG_TAG, "new score %d for exisiting request %s", score, s.string());
        n->mScore = score;
    }
    String s;
    IObject::Probe(mCapabilityFilter)->ToString(&s);
    if (VDBG) Logger::E(mLOG_TAG, "  my score=%d, my filter= %s", mScore, s.string());
    EvalRequest(n);
    return NOERROR;
}

ECode NetworkFactory::HandleRemoveRequest(
    /* [in] */ INetworkRequest* request)
{
    Int32 requestId;
    request->GetRequestId(&requestId);
    AutoPtr<IInterface> obj;
    mNetworkRequests->Get(requestId, (IInterface**)&obj);
    AutoPtr<NetworkRequestInfo> n = (NetworkRequestInfo*) IObject::Probe(obj);
    if (n != NULL) {
        if (n->mRequested) {
            Int32 requestId;
            n->mRequest->GetRequestId(&requestId);
            mNetworkRequests->Remove(requestId);
            ReleaseNetworkFor(n->mRequest);
        }
    }
    return NOERROR;
}

ECode NetworkFactory::HandleSetScore(
    /* [in] */ Int32 score)
{
    mScore = score;
    return EvalRequests();
}

ECode NetworkFactory::HandleSetFilter(
    /* [in] */ INetworkCapabilities* netCap)
{
    mCapabilityFilter = netCap;
    return EvalRequests();
}

ECode NetworkFactory::AcceptRequest(
    /* [in] */ INetworkRequest* request,
    /* [in] */ Int32 score,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = TRUE;
    return NOERROR;
}

ECode NetworkFactory::EvalRequest(
    /* [in] */ NetworkRequestInfo* n)
{
    AutoPtr<INetworkCapabilities> networkCapabilities;
    n->mRequest->GetNetworkCapabilities((INetworkCapabilities**)&networkCapabilities);
    Boolean bSatisfiedByNetworkCapabilities;
    networkCapabilities->SatisfiedByNetworkCapabilities(mCapabilityFilter, &bSatisfiedByNetworkCapabilities);
    Boolean bAcceptRequest;
    this->AcceptRequest(n->mRequest, n->mScore, &bAcceptRequest);
    if (n->mRequested == FALSE && n->mScore < mScore &&
            bSatisfiedByNetworkCapabilities && bAcceptRequest) {
        NeedNetworkFor(n->mRequest, n->mScore);
        n->mRequested = TRUE;
    } else if (n->mRequested == TRUE &&
            (n->mScore > mScore || bSatisfiedByNetworkCapabilities == FALSE || bAcceptRequest == FALSE)) {
        ReleaseNetworkFor(n->mRequest);
        n->mRequested = FALSE;
    }
    return NOERROR;
}

ECode NetworkFactory::EvalRequests()
{
    Int32 size;
    mNetworkRequests->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        mNetworkRequests->ValueAt(i, (IInterface**)&obj);
        AutoPtr<NetworkRequestInfo> n = (NetworkRequestInfo*) IObject::Probe(obj);
        EvalRequest(n);
    }
    return NOERROR;
}

ECode NetworkFactory::StartNetwork()
{
    return NOERROR;
}

ECode NetworkFactory::StopNetwork()
{
    return NOERROR;
}

ECode NetworkFactory::NeedNetworkFor(
    /* [in] */ INetworkRequest* networkRequest,
    /* [in] */ Int32 score)
{
    if (++mRefCount == 1) StartNetwork();
    return NOERROR;
}

ECode NetworkFactory::ReleaseNetworkFor(
    /* [in] */ INetworkRequest* networkRequest)
{
    if (--mRefCount == 0) StopNetwork();
    return NOERROR;
}

ECode NetworkFactory::AddNetworkRequest(
    /* [in] */ INetworkRequest* networkRequest,
    /* [in] */ Int32 score)
{
    AutoPtr<IMessage> msg;
    IHandler::Probe(this)->ObtainMessage(CMD_REQUEST_NETWORK, TO_IINTERFACE(new NetworkRequestInfo(networkRequest, score)), (IMessage**)&msg);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode NetworkFactory::RemoveNetworkRequest(
    /* [in] */ INetworkRequest* networkRequest)
{
    AutoPtr<IMessage> msg;
    IHandler::Probe(this)->ObtainMessage(CMD_CANCEL_REQUEST, networkRequest, (IMessage**)&msg);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode NetworkFactory::SetScoreFilter(
    /* [in] */ Int32 score)
{
    AutoPtr<IMessage> msg;
    IHandler::Probe(this)->ObtainMessage(CMD_SET_SCORE, score, 0, (IMessage**)&msg);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode NetworkFactory::SetCapabilityFilter(
    /* [in] */ INetworkCapabilities* netCap)
{
    AutoPtr<INetworkCapabilities> newNetworkCapabilities;
    CNetworkCapabilities::New(netCap, (INetworkCapabilities**)&newNetworkCapabilities);
    AutoPtr<IMessage> msg;
    IHandler::Probe(this)->ObtainMessage(CMD_SET_FILTER, newNetworkCapabilities, (IMessage**)&msg);
    Boolean b;
    SendMessage(msg, &b);
    return NOERROR;
}

ECode NetworkFactory::Log(
    /* [in] */ const char* fmt, ...)
{
    Logger::E(mLOG_TAG, "NetworkFactory::Log no out put");
    //va_list ap;
    //va_start(ap, fmt);
    //Logger::E(mLOG_TAG, fmt, ap);
    //va_end(ap);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
