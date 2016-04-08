#include "ClientSocketHandler.h"
#include "ChatManager.h"
#include "CActivityOne.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;
using Elastos::Net::CSocket;
using Elastos::Net::IInetSocketAddress;
using Elastos::Net::CInetSocketAddress;
using Elastos::Core::EIID_IRunnable;
using Elastos::Core::CThread;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pServiceDemo {

const String ClientSocketHandler::TAG("ClientSocketHandler");

ClientSocketHandler::ClientSocketHandler(
    /* [in] */ IHandler* handler,
    /* [in] */ IInetAddress* groupOwnerAddress)
    : mHandler(handler)
    , mAddress(groupOwnerAddress)
{
    Thread::constructor(String("ClientSocketHandler"));
}

ECode ClientSocketHandler::Run()
{
    AutoPtr<ISocket> socket;
    CSocket::New((ISocket**)&socket);
    // try {
    String hostAddress;
    AutoPtr<IInetSocketAddress> isa;
    AutoPtr<IThread> thread;

    ECode ec = socket->Bind(NULL);
    if (FAILED(ec)) goto _Exit_;

    mAddress->GetHostAddress(&hostAddress);
    ec = CInetSocketAddress::New(hostAddress, CActivityOne::SERVER_PORT,
        (IInetSocketAddress**)&isa);
    if (FAILED(ec)) goto _Exit_;

    ec = socket->Connect(isa, 5000);
    if (FAILED(ec)) goto _Exit_;

    Slogger::D(TAG, "Launching the I/O handler");
    mChat = new ChatManager(socket, mHandler);

    CThread::New((IRunnable*)(mChat->Probe(EIID_IRunnable)), (IThread**)&thread);
    thread->Start();

    return NOERROR;

_Exit_:
    Slogger::E(TAG, "Failed to run ClientSocketHandler.");
    if (socket != NULL) {
        socket->Close();
    }
    // } catch (IOException e) {
    //     e.printStackTrace();
    //     try {
    //         socket.close();
    //     } catch (IOException e1) {
    //         e1.printStackTrace();
    //     }
    //     return;
    // }
    return ec;
}

AutoPtr<ChatManager> ClientSocketHandler::GetChat()
{
    return mChat;
}

} // namespace WifiP2pServiceDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
