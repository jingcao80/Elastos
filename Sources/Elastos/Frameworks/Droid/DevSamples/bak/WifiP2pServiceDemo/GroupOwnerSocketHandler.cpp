#include "GroupOwnerSocketHandler.h"
#include "ChatManager.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Concurrent::IBlockingQueue;
using Elastos::Utility::Concurrent::CLinkedBlockingQueue;
using Elastos::Utility::Concurrent::CThreadPoolExecutor;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Net::CServerSocket;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace WifiP2pServiceDemo {

const String GroupOwnerSocketHandler::TAG("GroupOwnerSocketHandler");
const Int32 GroupOwnerSocketHandler::THREAD_COUNT = 10;

GroupOwnerSocketHandler::GroupOwnerSocketHandler(
    /* [in] */ IHandler* handler)
    : mHandler(handler)
{
    Thread::constructor(String("GroupOwnerSocketHandler"));

    AutoPtr<ITimeUnitHelper> helper;
    CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
    AutoPtr<ITimeUnit> seconds;
    helper->GetSECONDS((ITimeUnit**)&seconds);

    AutoPtr<IBlockingQueue> blockingQueue;
    CLinkedBlockingQueue::New((IBlockingQueue**)&blockingQueue);
    CThreadPoolExecutor::New(THREAD_COUNT, THREAD_COUNT, 10,
        seconds, blockingQueue, (IThreadPoolExecutor**)&mPool);

    // try {
    ECode ec = CServerSocket::New(4545, (IServerSocket**)&mSocket);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Failed to create server socket.");
        mPool->ShutdownNow(NULL);
    }
    else {
        Slogger::D(TAG, "Server socket Started.");
    }
    // } catch (IOException e) {
    //     e.printStackTrace();
    //     pool.shutdownNow();
    //     throw e;
    // }
}

ECode GroupOwnerSocketHandler::Run()
{
    ECode ec = NOERROR;
    while (TRUE) {
        // try {
        // A blocking operation. Initiate a ChatManager instance when
        // there is a new connection
        AutoPtr<ISocket> cs;
        AutoPtr<ChatManager> chat;

        ec = mSocket->Accept((ISocket**)&cs);
        if (FAILED(ec)) goto _Exit_;

        chat = new ChatManager(cs, mHandler);
        ec = mPool->Execute((IRunnable*)(chat->Probe(EIID_IRunnable)));
        if (FAILED(ec)) {
            goto _Exit_;
        }
        else {
            Slogger::D(TAG, "Launching the I/O handler");
        }

        continue;

_Exit_:
        if (cs != NULL) {
            Boolean isClosed;
            cs->IsClosed(&isClosed);
            if (!isClosed) {
                cs->Close();
            }
        }

        mPool->ShutdownNow(NULL);
        break;
        // } catch (IOException e) {
        //     try {
        //         if (socket != null && !socket.isClosed())
        //             socket.close();
        //     } catch (IOException ioe) {

        //     }
        //     e.printStackTrace();
        //     pool.shutdownNow();
        //     break;
        // }
    }
    return NOERROR;
}


} // namespace WifiP2pServiceDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
