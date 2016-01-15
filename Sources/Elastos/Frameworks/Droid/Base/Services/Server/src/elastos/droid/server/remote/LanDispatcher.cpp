
#include "remote/LanDispatcher.h"
#include "remote/RemoteUtils.h"
#include "remote/ResultMsg.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Net::CDatagramSocket;
using Elastos::Net::IDatagramPacket;
using Elastos::Net::CDatagramPacket;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Net::CInetAddressHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

const String LanDispatcher::TAG("Kortide_LanDispatcher");
const Boolean LanDispatcher::DBG = RemoteUtils::IsDebug();
const String LanDispatcher::LISTENADDR = String("0.0.0.0");
const Int32 LanDispatcher::LISTENPORT = 49528;

LanDispatcher::LanDispatcher(
    /* [in] */ IContext* context)
{
    Thread::constructor();
    if (DBG) Slogger::D(TAG, "constructor");
    AutoPtr<IInetAddressHelper> iah;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&iah);
    AutoPtr<IInetAddress> inetAddr;
    iah->GetByName(LISTENADDR, (IInetAddress**)&inetAddr);
    CDatagramSocket::New(LISTENPORT, inetAddr, (IDatagramSocket**)&mLanListenSocket);
    mLanListenSocket->SetBroadcast(TRUE);
    mContext = context;
    mLogin = String("LOGIN:5566");
    mCheck = String("CHECK:5566");
}

String LanDispatcher::GetVerifiedPort(
    /* [in] */ const String& clientMsg)
{
    if (DBG) Slogger::D(TAG, "GetVerifiedPort %s", clientMsg.string());
    if (clientMsg.Equals(mLogin)) {
        StringBuilder sb("VerifiedPort:");
        AutoPtr<ServerSocketTimer> socketTimer = new ServerSocketTimer(mContext);
        socketTimer->Start();
        mServerSocketList.PushBack(socketTimer);
        sb.Append(socketTimer->GetSocketPort());
        return sb.ToString();
    }
    else if (clientMsg.Equals(mCheck)) {
        return String("OK");
    }
    return ResultMsg::ES_VERIFIEDFAIL->ToString();
}

ECode LanDispatcher::Run()
{
    Boolean isInterrupted;
    while (!(Thread::GetCurrentThread()->IsInterrupted(&isInterrupted), isInterrupted)) {
        if (DBG) Slogger::D(TAG, "Ready to receive broadcast packets!");
        //Receive a packet
        AutoPtr<ArrayOf<Byte> > recvBuf = ArrayOf<Byte>::Alloc(23);
        AutoPtr<IDatagramPacket> packet;
        CDatagramPacket::New(recvBuf, recvBuf->GetLength(), (IDatagramPacket**)&packet);

        ECode ec = NOERROR;
        do {
            ec = mLanListenSocket->Receive(packet);
            if (FAILED(ec))
                break;

            AutoPtr<IInetAddress> address;
            ec = packet->GetAddress((IInetAddress**)&address);
            if (FAILED(ec))
                break;
            //Packet received
            if (DBG) {
                String hostAddress;
                address->GetHostAddress(&hostAddress);
                Slogger::D(TAG, "Discovery packet received from: %s", hostAddress.string());
            }

            AutoPtr<ArrayOf<Byte> > data;
            ec = packet->GetData((ArrayOf<Byte>**)&data);
            if (FAILED(ec))
                break;
            String clientMsg((char*)data->GetPayload(), data->GetLength());
            clientMsg.Trim();
            if (DBG) Slogger::D(TAG, "Packet received: %s", clientMsg.string());

            //See if the packet holds the right command (message)
            String verifiedResult = GetVerifiedPort(clientMsg);
            AutoPtr<ArrayOf<Byte> > sendData = verifiedResult.GetBytes();
            //Send a response
            Int32 port;
            packet->GetPort(&port);
            AutoPtr<IDatagramPacket> sendPacket;
            ec = CDatagramPacket::New(sendData, sendData->GetLength(), address, port, (IDatagramPacket**)&sendPacket);
            if (FAILED(ec))
                break;
            ec = mLanListenSocket->Send(sendPacket);
            if (FAILED(ec))
                break;

            if (DBG) {
                AutoPtr<IInetAddress> address;
                sendPacket->GetAddress((IInetAddress**)&address);
                String hostAddress;
                address->GetHostAddress(&hostAddress);
                Slogger::D(TAG, "Sent packet to: %s", hostAddress.string());
            }
        } while (0);

        if (ec == (ECode)E_SOCKET_TIMEOUT_EXCEPTION) {
            Slogger::E(TAG, "Socket timeout!");
        }
        else if (FAILED(ec)) {
            Slogger::E(TAG, "Failed: ec = 0x%08x", ec);
        }
    }

    return NOERROR;
}

} // Remote
} // Server
} // Droid
} // Elastos