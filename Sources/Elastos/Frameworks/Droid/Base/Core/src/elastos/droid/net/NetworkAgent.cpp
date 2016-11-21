
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/net/NetworkAgent.h"
#include "elastos/droid/internal/utility/CAsyncChannel.h"
#include "elastos/droid/net/CLinkProperties.h"
#include "elastos/droid/net/CNetwork.h"
#include "elastos/droid/net/CNetworkCapabilities.h"
#include "elastos/droid/net/CNetworkInfo.h"
#include "elastos/droid/net/LinkProperties.h"
#include "elastos/droid/os/CMessage.h"
#include "elastos/droid/os/CMessenger.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::CAsyncChannel;
using Elastos::Droid::Internal::Utility::IAsyncChannel;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Net::CLinkProperties;
using Elastos::Droid::Net::CNetworkInfo;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::CMessage;
using Elastos::Droid::Os::CMessenger;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;

using Elastos::Core::CInteger32;
using Elastos::Core::CArrayOf;
using Elastos::Core::IInteger32;
using Elastos::Core::IArrayOf;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_INTERFACE_IMPL(NetworkAgent, Handler, INetworkAgent)

const Boolean NetworkAgent::DBG = TRUE;
const Boolean NetworkAgent::VDBG = FALSE;
const Int32 NetworkAgent::BASE = IProtocol::BASE_NETWORK_AGENT;

AutoPtr<IArrayList> NetworkAgent::CreatePreConnectQueue()
{
    AutoPtr<IArrayList> rev;
    CArrayList::New((IArrayList**)&rev);
    return rev;
}

NetworkAgent::NetworkAgent()
    : mPreConnectedQueue(CreatePreConnectQueue())
{}

NetworkAgent::~NetworkAgent()
{}

ECode NetworkAgent::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ IContext* context,
    /* [in] */ const String& logTag,
    /* [in] */ INetworkInfo* ni,
    /* [in] */ INetworkCapabilities* nc,
    /* [in] */ ILinkProperties* lp,
    /* [in] */ Int32 score)
{
    return constructor(looper, context, logTag, ni, nc, lp, score, NULL);
}

ECode NetworkAgent::constructor(
    /* [in] */ ILooper* looper,
    /* [in] */ IContext* context,
    /* [in] */ const String& logTag,
    /* [in] */ INetworkInfo* ni,
    /* [in] */ INetworkCapabilities* nc,
    /* [in] */ ILinkProperties* lp,
    /* [in] */ Int32 score,
    /* [in] */ INetworkMisc* misc)
{
    Handler::constructor(looper);
    mLOG_TAG = logTag;
    mContext = context;
    if (ni == NULL || nc == NULL || lp == NULL) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (VDBG) Log("Registering NetworkAgent");
    AutoPtr<IInterface> obj;
    mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IConnectivityManager> cm = IConnectivityManager::Probe(obj);
    AutoPtr<IMessenger> newMessenger;
    CMessenger::New(this, (IMessenger**)&newMessenger);
    AutoPtr<INetworkInfo> newNetworkInfo;
    CNetworkInfo::New(ni, (INetworkInfo**)&newNetworkInfo);
    AutoPtr<ILinkProperties> newLinkProperties;
    CLinkProperties::New(lp, (ILinkProperties**)&newLinkProperties);
    AutoPtr<INetworkCapabilities> newNetworkCapabilities;
    CNetworkCapabilities::New(nc, (INetworkCapabilities**)&newNetworkCapabilities);
    cm->RegisterNetworkAgent(newMessenger, newNetworkInfo, newLinkProperties, newNetworkCapabilities, score, misc);
    return NOERROR;
}

ECode NetworkAgent::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IMessenger> replyTo;
    msg->GetReplyTo((IMessenger**)&replyTo);
    Int32 arg1;
    msg->GetArg1(&arg1);
    switch (what) {
        case IAsyncChannel::CMD_CHANNEL_FULL_CONNECTION: {
            if (mAsyncChannel != NULL) {
                Log("Received new connection while already connected!");
            } else {
                if (VDBG) Log("NetworkAgent fully connected");
                AutoPtr<IAsyncChannel> ac;
                CAsyncChannel::New((IAsyncChannel**)&ac);
                ac->Connected(NULL, this, replyTo);
                ac->ReplyToMessage(msg, IAsyncChannel::CMD_CHANNEL_FULLY_CONNECTED,
                        IAsyncChannel::STATUS_SUCCESSFUL);
                {    AutoLock syncLock(mPreConnectedQueue);
                    mAsyncChannel = ac;
                    Int32 size;
                    mPreConnectedQueue->GetSize(&size);
                    for (Int32 i = 0; i < size; ++i) {
                        AutoPtr<IInterface> m;
                        mPreConnectedQueue->Get(i, (IInterface**)&m);
                        ac->SendMessage(IMessage::Probe(m));
                    }
                    mPreConnectedQueue->Clear();
                }
            }
            break;
        }
        case IAsyncChannel::CMD_CHANNEL_DISCONNECT: {
            if (VDBG) Log("CMD_CHANNEL_DISCONNECT");
            if (mAsyncChannel != NULL) mAsyncChannel->Disconnect();
            break;
        }
        case IAsyncChannel::CMD_CHANNEL_DISCONNECTED: {
            if (DBG) Log("NetworkAgent channel lost");
            // let the client know CS is done with us.
            Unwanted();
            {    AutoLock syncLock(mPreConnectedQueue);
                mAsyncChannel = NULL;
            }
            break;
        }
        case INetworkAgent::CMD_SUSPECT_BAD: {
            Log("Unhandled Message %s", msg);
            break;
        }
        case INetworkAgent::CMD_REPORT_NETWORK_STATUS: {
            if (VDBG) {
                Log("CMD_REPORT_NETWORK_STATUS(%s",
                        (arg1 == VALID_NETWORK ? "VALID)" : "INVALID)"));
            }
            NetworkStatus(arg1);
            break;
        }
    }
    return NOERROR;
}

ECode NetworkAgent::QueueOrSendMessage(
    /* [in] */ Int32 what,
    /* [in] */ IInterface* obj)
{
    {    AutoLock syncLock(mPreConnectedQueue);
        if (mAsyncChannel != NULL) {
            mAsyncChannel->SendMessage(what, obj);
        } else {
            AutoPtr<IMessage> msg = CMessage::Obtain();
            msg->SetWhat(what);
            msg->SetObj(obj);
            mPreConnectedQueue->Add(msg);
        }
    }
    return NOERROR;
}

ECode NetworkAgent::SendLinkProperties(
    /* [in] */ ILinkProperties* linkProperties)
{
    AutoPtr<ILinkProperties> newLinkProperties;
    CLinkProperties::New(linkProperties, (ILinkProperties**)&newLinkProperties);
    QueueOrSendMessage(EVENT_NETWORK_PROPERTIES_CHANGED, newLinkProperties);
    return NOERROR;
}

ECode NetworkAgent::SendNetworkInfo(
    /* [in] */ INetworkInfo* networkInfo)
{
    AutoPtr<INetworkInfo> newNetworkInfo;
    CNetworkInfo::New(networkInfo, (INetworkInfo**)&newNetworkInfo);
    QueueOrSendMessage(EVENT_NETWORK_INFO_CHANGED, newNetworkInfo);
    return NOERROR;
}

ECode NetworkAgent::SendNetworkCapabilities(
    /* [in] */ INetworkCapabilities* networkCapabilities)
{
    AutoPtr<INetworkCapabilities> newNetworkCapabilities;
    CNetworkCapabilities::New(networkCapabilities, (INetworkCapabilities**)&newNetworkCapabilities);
    QueueOrSendMessage(EVENT_NETWORK_CAPABILITIES_CHANGED, newNetworkCapabilities);
    return NOERROR;
}

ECode NetworkAgent::SendNetworkScore(
    /* [in] */ Int32 score)
{
    if (score < 0) {
        Log("Score must be >= 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    AutoPtr<IInteger32> newInteger;
    CInteger32::New(score, (IInteger32**)&newInteger);
    QueueOrSendMessage(EVENT_NETWORK_SCORE_CHANGED, newInteger);
    return NOERROR;
}

ECode NetworkAgent::AddUidRanges(
    /* [in] */ ArrayOf<IUidRange*>* ranges)
{
    AutoPtr<IArrayOf> obj;
    CArrayOf::New(EIID_IUidRange, ranges->GetLength(), (IArrayOf**)&obj);
    for (Int32 i = 0; i < ranges->GetLength(); ++i) {
        obj->Set(i, (*ranges)[i]);
    }
    return QueueOrSendMessage(EVENT_UID_RANGES_ADDED, obj);
}

ECode NetworkAgent::RemoveUidRanges(
    /* [in] */ ArrayOf<IUidRange*>* ranges)
{
    AutoPtr<IArrayOf> obj;
    CArrayOf::New(EIID_IUidRange, ranges->GetLength(), (IArrayOf**)&obj);
    for (Int32 i = 0; i < ranges->GetLength(); ++i) {
        obj->Set(i, (*ranges)[i]);
    }
    return QueueOrSendMessage(EVENT_UID_RANGES_REMOVED, obj);
}

ECode NetworkAgent::ExplicitlySelected()
{
    return QueueOrSendMessage(EVENT_SET_EXPLICITLY_SELECTED, 0);
}

ECode NetworkAgent::NetworkStatus(
    /* [in] */ Int32 status)
{
    return NOERROR;
}

ECode NetworkAgent::Log(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    Logger::D(mLOG_TAG, (String("NetworkAgent: ") + fmt).string(), ap);
    va_end(ap);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
