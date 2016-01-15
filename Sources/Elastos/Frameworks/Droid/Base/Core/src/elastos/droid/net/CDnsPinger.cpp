
#include "Elastos.Droid.Internal.h"
#include "elastos/droid/R.h"
#include "elastos/droid/net/CConnectivityManager.h"
#include "elastos/droid/net/CConnectivityManagerHelper.h"
#include "elastos/droid/net/CDnsPinger.h"
#include "elastos/droid/net/NetworkUtils.h"
#include "elastos/droid/os/SystemClock.h"
// #include "elastos/droid/provider/Settings.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Internal::Utility::IProtocol;
using Elastos::Droid::Net::CConnectivityManager;
using Elastos::Droid::Net::IConnectivityManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Provider::ISettingsGlobal;
// using Elastos::Droid::Provider::Settings;

using Elastos::Core::CInteger32;
using Elastos::Core::StringUtils;
using Elastos::Net::CDatagramPacket;
using Elastos::Net::CDatagramSocket;
using Elastos::Net::CNetworkInterfaceHelper;
using Elastos::Net::IDatagramPacket;
using Elastos::Net::INetworkInterface;
using Elastos::Net::INetworkInterfaceHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CRandom;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Net {

CAR_OBJECT_IMPL(CDnsPinger)

CAR_INTERFACE_IMPL(CDnsPinger, Handler, IDnsPinger)

AutoPtr<IRandom> CreateRandom()
{
    AutoPtr<IRandom> result;
    CRandom::New((IRandom**)&result);
    return result;
}

AutoPtr<IAtomicInteger32> CreateCounter()
{
    AutoPtr<IAtomicInteger32> result;
    CAtomicInteger32::New((IAtomicInteger32**)&result);
    return result;
}

const Boolean CDnsPinger::DBG = FALSE;
const Int32 CDnsPinger::RECEIVE_POLL_INTERVAL_MS = 200;
const Int32 CDnsPinger::DNS_PORT = 53;
const Int32 CDnsPinger::SOCKET_TIMEOUT_MS = 1;
const AutoPtr<IRandom> CDnsPinger::sRandom = CreateRandom();
const AutoPtr<IAtomicInteger32> CDnsPinger::sCounter = CreateCounter();

const Int32 CDnsPinger::BASE = IProtocol::BASE_DNS_PINGER; //Protocol.BASE_DNS_PINGER; 0x00050000
const Int32 CDnsPinger::ACTION_PING_DNS = CDnsPinger::BASE + 1; //0x00050001; // = BASE + 1;
const Int32 CDnsPinger::ACTION_LISTEN_FOR_RESPONSE = CDnsPinger::BASE + 2; //0x00050002; // = BASE + 2;
const Int32 CDnsPinger::ACTION_CANCEL_ALL_PINGS = CDnsPinger::BASE + 3; //0x00050003; // = BASE + 3;
AutoPtr<ArrayOf<Byte> > CDnsPinger::mDnsQuery = InitDnsQuery();

AutoPtr<ArrayOf<Byte> > CDnsPinger::InitDnsQuery()
{
    AutoPtr<ArrayOf<Byte> > rev;
    Byte src[] = {
        0, 0, // [0-1] is for ID (will set each time)
        1, 0, // [2-3] are flags.  Set byte[2] = 1 for recursion desired (RD) on.  Currently on.
        0, 1, // [4-5] bytes are for number of queries (QCOUNT)
        0, 0, // [6-7] unused count field for dns response packets
        0, 0, // [8-9] unused count field for dns response packets
        0, 0, // [10-11] unused count field for dns response packets
        3, 'w', 'w', 'w',
        6, 'g', 'o', 'o', 'g', 'l', 'e',
        3, 'c', 'o', 'm',
        0,    // null terminator of address (also called empty TLD)
        0, 1, // QTYPE, set to 1 = A (host address)
        0, 1  // QCLASS, set to 1 = IN (internet)
    };
    rev = ArrayOf<Byte>::Alloc(sizeof(src)/sizeof(Byte));
    rev->Copy(src, sizeof(src)/sizeof(Byte));
    return rev;
}

CDnsPinger::DnsArg::DnsArg(
    /* [in] */ IInetAddress* dns,
    /* [in] */ Int32 seq)
    : mDns(dns)
    , mSeq(seq)
{}

CDnsPinger::CDnsPinger()
{
    CAtomicInteger32::New((IAtomicInteger32**)&mCurrentToken);
    AutoPtr<IArrayList> temp;
    CArrayList::New((IArrayList**)&temp);
    mActivePings = IList::Probe(temp);
}

ECode CDnsPinger::constructor(
    /* [in] */ IContext* context,
    /* [in] */ const String& tagstr,
    /* [in] */ ILooper* looper,
    /* [in] */ IHandler* target,
    /* [in] */ Int32 connectionType)
{
    Handler::constructor(looper);

    TAG = tagstr;
    mContext = context;
    mTarget = target;
    mConnectionType = connectionType;
    Boolean isValid;
    CConnectivityManager::IsNetworkTypeValid(connectionType, &isValid);
    if (!isValid) {
        Logger::E(TAG, "Invalid connectionType in constructor: %d", connectionType);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInetAddress> defaultDns;
    GetDefaultDns((IInetAddress**)&defaultDns);
    AutoPtr<IArrayList> tmp;
    CArrayList::New((IArrayList**)&tmp);
    mDefaultDns = IList::Probe(tmp);
    mDefaultDns->Add(defaultDns);
    mEventCounter = 0;
    return NOERROR;
}

ECode CDnsPinger::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what, arg1, arg2;
    msg->GetWhat(&what);
    msg->GetArg1(&arg1);
    msg->GetArg2(&arg2);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);

    switch(what) {
        case ACTION_PING_DNS: {
            DnsArg* arg = (DnsArg*)IObject::Probe(obj);
            HandleActionPingDNS(arg1, arg2, arg);
            break;
        }
        case ACTION_LISTEN_FOR_RESPONSE: {
            HandleActionListenForResponse(arg1);
            break;
        }
        case ACTION_CANCEL_ALL_PINGS:
            HandleActionCancelAllPings();
            break;
    }

    return NOERROR;
}

ECode CDnsPinger::Log(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    Logger::D(TAG, fmt, ap);
    va_end(ap);
    return NOERROR;
}

ECode CDnsPinger::Loge(
    /* [in] */ const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    Logger::E(TAG, fmt, ap);
    va_end(ap);
    return NOERROR;
}

void CDnsPinger::HandleActionPingDNS(
    /* [in] */ Int32 arg1,
    /* [in] */ Int32 arg2,
    /* [in] */ DnsArg* dnsArg)
{
    assert(dnsArg != NULL);
    Int32 val;
    mCurrentToken->Get(&val);
    if (dnsArg->mSeq != val) {
        return;
    }

//  try {
    AutoPtr<ActivePing> newActivePing = new ActivePing();
    AutoPtr<IInetAddress> dnsAddress = dnsArg->mDns;
    newActivePing->mInternalId = arg1;
    newActivePing->mTimeout = arg2;
    CDatagramSocket::New((IDatagramSocket**)&(newActivePing->mSocket));
    // Set some socket properties
    newActivePing->mSocket->SetSoTimeout(SOCKET_TIMEOUT_MS);

    // Try to bind but continue ping if bind fails
    AutoPtr<ILinkProperties> linkProp;
    GetCurrentLinkProperties((ILinkProperties**)&linkProp);
    String name;
    linkProp->GetInterfaceName(&name);
    AutoPtr<INetworkInterfaceHelper> netInterfaceHelper;
    CNetworkInterfaceHelper::AcquireSingleton((INetworkInterfaceHelper**)&netInterfaceHelper);
    AutoPtr<INetworkInterface> netInterface;
    netInterfaceHelper->GetByName(name, (INetworkInterface**)&netInterface);
    ECode ec = newActivePing->mSocket->SetNetworkInterface(netInterface);
    if (FAILED(ec)) {
        Loge("sendDnsPing::Error binding to socket %s", StringUtils::ToString(ec).string());
    }

    Int32 num;
    sRandom->NextInt32(&num);
    newActivePing->mPacketId = (Int16)num;
    AutoPtr< ArrayOf<Byte> > buf = ArrayOf<Byte>::Alloc(sizeof(mDnsQuery));
    for (Int32 i = 0; i < 32; i++) {
        (*buf)[i] = (*mDnsQuery)[i];
    }
    (*buf)[0] = (Byte) (newActivePing->mPacketId >> 8);
    (*buf)[1] = (Byte) newActivePing->mPacketId;

    // Send the DNS query
    AutoPtr<IDatagramPacket> packet;
    CDatagramPacket::New(buf, buf->GetLength(), dnsAddress, DNS_PORT, (IDatagramPacket**)&packet);
    if (DBG) {
        String hostAddress;
        dnsAddress->GetHostAddress(&hostAddress);
        Log("Sending a ping %s to %s with packetId %s.", StringUtils::ToString(newActivePing->mInternalId).string(), hostAddress.string(), StringUtils::ToString(newActivePing->mPacketId).string());
    }

    AutoPtr<IMessage> msg;

    ec = newActivePing->mSocket->Send(packet);
    if (FAILED(ec)) {
        SendResponse(arg1, -9999, SOCKET_EXCEPTION);
        return;
    }
    mActivePings->Add(TO_IINTERFACE(newActivePing.Get()));
    mEventCounter++;

    ObtainMessage(ACTION_LISTEN_FOR_RESPONSE, mEventCounter, 0, (IMessage**)&msg);
    Boolean result;
    SendMessageDelayed(msg, RECEIVE_POLL_INTERVAL_MS, &result);
}

void CDnsPinger::HandleActionListenForResponse(
    /* [in] */ Int32 arg1)
{
    if (arg1 != mEventCounter) {
        return;
    }

    AutoPtr<IIterator> iter;
    mActivePings->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> iCur;
        iter->GetNext((IInterface**)&iCur);
        AutoPtr<ActivePing> curPing = (ActivePing*)IObject::Probe(iCur);
    //   try {
        /** Each socket will block for {@link #SOCKET_TIMEOUT_MS} in receive() */
        AutoPtr< ArrayOf<Byte> > responseBuf = ArrayOf<Byte>::Alloc(2);
        AutoPtr<IDatagramPacket> replyPacket;
        CDatagramPacket::New(responseBuf, 2, (IDatagramPacket**)&replyPacket);

        ECode ec = curPing->mSocket->Receive(replyPacket);
        if (FAILED(ec)) {
            if (ec == (ECode)E_SOCKET_TIMEOUT_EXCEPTION) {
                // A timeout here doesn't mean anything - squelsh this exception
                continue;
            }
            if (DBG) {
                Log("DnsPinger.pingDns got socket exception: %s", StringUtils::ToString(ec).string());
            }
            curPing->mResult = NULL;
            CInteger32::New(SOCKET_EXCEPTION, (IInteger32**)&curPing->mResult);
            continue;
        }
        // Check that ID field matches (we're throwing out the rest of the packet)
        if ((*responseBuf)[0] == (Byte) (curPing->mPacketId >> 8) &&
           (*responseBuf)[1] == (Byte) curPing->mPacketId) {
            curPing->mResult = NULL;
            CInteger32::New((Int32) (SystemClock::GetElapsedRealtime() - curPing->mStart), (IInteger32**)&curPing->mResult);
        }
        else {
            if (DBG) {
                Logger::I(TAG, "response ID didn't match, ignoring packet");
            }
        }
    }

    iter = NULL;
    mActivePings->GetIterator((IIterator**)&iter);
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> iCur;
        iter->GetNext((IInterface**)&iCur);
        AutoPtr<ActivePing> curPing = (ActivePing*)IObject::Probe(iCur);
        if (curPing->mResult != NULL) {
            Int32 result;
            curPing->mResult->GetValue(&result);
            SendResponse(curPing->mInternalId, curPing->mPacketId, result);
            curPing->mSocket->Close();
            iter->Remove();
        }
        else if (SystemClock::GetElapsedRealtime() > curPing->mStart + curPing->mTimeout) {
            SendResponse(curPing->mInternalId, curPing->mPacketId, IDnsPinger::TIMEOUT);
            curPing->mSocket->Close();
            iter->Remove();
        }
    }

    Boolean isEmpty;
    if (mActivePings->IsEmpty(&isEmpty), !isEmpty) {
        AutoPtr<IMessage> msg;
        ObtainMessage(ACTION_LISTEN_FOR_RESPONSE, mEventCounter, 0, (IMessage**)&msg);
        Boolean result;
        SendMessageDelayed(msg, RECEIVE_POLL_INTERVAL_MS, &result);
    }
    return;
}

void CDnsPinger::HandleActionCancelAllPings()
{
    AutoPtr<IIterator> iter;
    mActivePings->GetIterator((IIterator**)&iter);
    Boolean hasNext;
    while (iter->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> iCur;
        iter->GetNext((IInterface**)&iCur);
        AutoPtr<ActivePing> activePing = (ActivePing*)IObject::Probe(iCur);
        activePing->mSocket->Close();
    }
    mActivePings->Clear();
}

ECode CDnsPinger::GetDnsList(
    /* [out] */ IList** dnslist)
{
    VALIDATE_NOT_NULL(dnslist);

    AutoPtr<ILinkProperties> curLinkProps;
    GetCurrentLinkProperties((ILinkProperties**)&curLinkProps);
    if (curLinkProps == NULL) {
        Loge("getCurLinkProperties:: LP for type %s is NULL!", StringUtils::ToString(mConnectionType).string());
        *dnslist = mDefaultDns;
        REFCOUNT_ADD(*dnslist);
        return NOERROR;
    }

    AutoPtr<IList> l;
    curLinkProps->GetDnsServers((IList**)&l);
    AutoPtr<ICollection> dnses = ICollection::Probe(l);
    Int32 num;
    if (dnses != NULL) dnses->GetSize(&num);
    if (dnses == NULL || num == 0) {
        Loge("getDns::LinkProps has NULL dns - returning default");
        *dnslist = mDefaultDns;
        REFCOUNT_ADD(*dnslist);
        return NOERROR;
    }

    return CArrayList::New(dnses, dnslist);
}

/**
 * Send a ping.  The response will come via a {@link #DNS_PING_RESULT} to the handler
 * specified at creation.
 * @param dns address of dns server to ping
 * @param timeout timeout for ping
 * @return an ID field, which will also be included in the {@link #DNS_PING_RESULT} message.
 */
ECode CDnsPinger::PingDnsAsync(
    /* [in] */ IInetAddress* dns,
    /* [in] */ Int32 timeout,
    /* [in] */ Int32 delay,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    Int32 id;
    sCounter->IncrementAndGet(&id);
    *result = id;

    Int32 val;
    mCurrentToken->Get(&val);
    AutoPtr<DnsArg> newDnsArg = new DnsArg(dns, val);
    AutoPtr<IMessage> msg;
    ObtainMessage(ACTION_PING_DNS, id, timeout, TO_IINTERFACE(newDnsArg), (IMessage**)&msg);
    Boolean bval;
    return SendMessageDelayed(msg, delay, &bval);
}

ECode CDnsPinger::CancelPings()
{
    Int32 num;
    mCurrentToken->IncrementAndGet(&num);
    Boolean result;
    return SendEmptyMessage(ACTION_CANCEL_ALL_PINGS, &result);
}

ECode CDnsPinger::SendResponse(
    /* [in] */ Int32 internalId,
    /* [in] */ Int32 externalId,
    /* [in] */ Int32 responseVal)
{
    String s("Responding to packet ");
    s += StringUtils::ToString(internalId);
    s += " externalId ";
    s += StringUtils::ToString(externalId);
    s += " and val ";
    s += StringUtils::ToString(responseVal);

    AutoPtr<IMessage> msg;
    ObtainMessage(DNS_PING_RESULT, internalId, responseVal, (IMessage**)&msg);
    Boolean result;
    return mTarget->SendMessage(msg, &result);
}

ECode CDnsPinger::GetCurrentLinkProperties(
    /* [out] */ ILinkProperties** result)
{
    VALIDATE_NOT_NULL(result);
    if (mConnectivityManager == NULL) {
        AutoPtr<IInterface> obj;
        mContext->GetSystemService(IContext::CONNECTIVITY_SERVICE, (IInterface**)&obj);
        mConnectivityManager = IConnectivityManager::Probe(obj);
    }

    return mConnectivityManager->GetLinkProperties(mConnectionType, result);
}

ECode CDnsPinger::GetDefaultDns(
    /* [out] */ IInetAddress** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IContentResolver> resolver;
    FAIL_RETURN(mContext->GetContentResolver((IContentResolver**)&resolver))
    String dns;
    // TODO: Waiting for Settings
    assert(0);
    // FAIL_RETURN(Settings::Global::GetString(resolver, ISettingsGlobal::DEFAULT_DNS_SERVER, &dns))
    if (dns.IsNullOrEmpty()) {
        AutoPtr<IResources> resources;
        mContext->GetResources((IResources**)&resources);
        resources->GetString(R::string::config_default_dns_server, &dns);
    }

    AutoPtr< IInetAddress> addr;
    ECode ec = NetworkUtils::NumericToInetAddress(dns, (IInetAddress**)&addr);
    if (FAILED(ec)) {
        Loge("getDefaultDns::malformed default dns address");
        *result = NULL;
        return ec;
    }
    *result = addr;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace Net
} // namespace Droid
} // namespace Elastos
