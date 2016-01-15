
#include "remote/RemoteCmdListener.h"
#include "remote/RemoteUtils.h"
#include "remote/ResultMsg.h"
#include "remote/SessionCmdTask.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IOutputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::Net::ISocket;
using Elastos::Net::IURLDecoder;
using Elastos::Net::CURLDecoder;
using Elastos::Net::IURLEncoder;
using Elastos::Net::CURLEncoder;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::IBlockingQueue;
using Elastos::Utility::Concurrent::CLinkedBlockingQueue;
using Elastos::Utility::Concurrent::IThreadPoolExecutor;
using Elastos::Utility::Concurrent::CThreadPoolExecutor;
using Elastos::Utility::Concurrent::IFuture;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Remote {

const String RemoteCmdListener::TAG("Kortide_RemoteCmdListener");
const Boolean RemoteCmdListener::DBG = RemoteUtils::IsDebug();
const Int32 RemoteCmdListener::CMDTIMEOUT_SEC = 3;

RemoteCmdListener::RemoteCmdListener(
    /* [in] */ IContext* context,
    /* [in] */ IServerSocket* listenSocket)
    : mSocketListener(listenSocket)
    , mContext(context)
{
    Thread::constructor();
}

void RemoteCmdListener::Quit()
{
    // try {
        mSocketListener->Close(); // if the TcpListener thread is blocked on
        // accept,
        // closing the socket will raise an
        // exception
    // } catch (Exception ex) {}
}

ECode RemoteCmdListener::Run()
{
    AcceptStringCmd();
    return NOERROR;
}

void RemoteCmdListener::AcceptStringCmd()
{
    Boolean isInterrupted;
    ECode ec = NOERROR;
    AutoPtr<ISocket> clientSocket;
    AutoPtr<IBufferedReader> inFromClient;
    AutoPtr<IDataOutputStream> outToClient;
    while (!(Thread::GetCurrentThread()->IsInterrupted(&isInterrupted), isInterrupted)) {
        ec = mSocketListener->Accept((ISocket**)&clientSocket);
        if (FAILED(ec))
            break;
        String cmdResult = ResultMsg::ES_NORESPONSE->ToString();

        /** Get client cmd from socket*/
        AutoPtr<IInputStream> inputStream;
        ec = clientSocket->GetInputStream((IInputStream**)&inputStream);
        if (FAILED(ec))
            break;
        AutoPtr<IInputStreamReader> isReader;
        ec = CInputStreamReader::New(inputStream, (IInputStreamReader**)&isReader);
        if (FAILED(ec))
            break;
        ec = CBufferedReader::New(isReader, (IBufferedReader**)&inFromClient);
        if (FAILED(ec))
            break;
        String clientCmd;
        ec = inFromClient->ReadLine(&clientCmd);
        if (FAILED(ec))
            break;
        if (DBG) Slogger::D(TAG, "Get clientCmd: %s", clientCmd.string());
        // try {
            AutoPtr<IURLDecoder> urlDecoder;
            CURLDecoder::AcquireSingleton((IURLDecoder**)&urlDecoder);
            String temp;
            if (FAILED(urlDecoder->Decode(clientCmd, String("UTF-8"), &temp)))
                Slogger::E(TAG, "UnsupportedEncodingException!!");
            clientCmd = temp;
        // } catch (UnsupportedEncodingException ex) {
        //     ex.printStackTrace();
        // }

        /** Doing cmd dispatch and wait for result by SessionCmdTask*/
        // ExecutorService executor = Executors.newSingleThreadExecutor();
        AutoPtr<ITimeUnitHelper> tuHelper;
        CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&tuHelper);
        AutoPtr<ITimeUnit> milliseconds;
        tuHelper->GetMILLISECONDS((ITimeUnit**)&milliseconds);
        AutoPtr<IBlockingQueue> bq;
        CLinkedBlockingQueue::New((IBlockingQueue**)&bq);
        AutoPtr<IThreadPoolExecutor> executor;
        CThreadPoolExecutor::New(1, 1, 0L, milliseconds, bq, (IThreadPoolExecutor**)&executor);
        AutoPtr<SessionCmdTask> sct = new SessionCmdTask(mContext, clientCmd);
        AutoPtr<IFuture> future;
        executor->Submit(sct, (IFuture**)&future);

        do {
            AutoPtr<ITimeUnit> seconds;
            tuHelper->GetSECONDS((ITimeUnit**)&seconds);
            AutoPtr<IInterface> cmdInterface;
            ec = future->Get(CMDTIMEOUT_SEC, seconds, (IInterface**)&cmdInterface);
            if (FAILED(ec))
                break;
            ICharSequence::Probe(cmdInterface)->ToString(&cmdResult);
            cmdResult = RemoteUtils::ComposeResult(sct->GetResultNum(), cmdResult);
            AutoPtr<IURLEncoder> urlEncoder;
            CURLEncoder::AcquireSingleton((IURLEncoder**)&urlEncoder);
            ec = urlEncoder->Encode(cmdResult, String("UTF-8"), &temp);
            if (FAILED(ec))
                break;
            cmdResult = temp;
        } while (0);

        if (ec == (ECode)E_TIMEOUT_EXCEPTION) {
            cmdResult = ResultMsg::ES_CMD_TIMEOUT->ToString();
            cmdResult = RemoteUtils::ComposeResult(ResultMsg::ES_CMD_TIMEOUT->ToInt32(), String(""));
            // e.printStackTrace();
        }
        else if (ec == (ECode)E_UNSUPPORTED_ENCODING_EXCEPTION) {
            Slogger::E(TAG, "UnsupportedEncodingException! cmdResult is %s", cmdResult.string());
        }
        else if (FAILED(ec)) {
            break;
        }

        /** Write client cmd result to socket*/
        AutoPtr<IOutputStream> outputStream;
        ec = clientSocket->GetOutputStream((IOutputStream**)&outputStream);
        if (FAILED(ec))
            break;
        ec = CDataOutputStream::New(outputStream, (IDataOutputStream**)&outToClient);
        if (FAILED(ec))
            break;
        cmdResult += '\n';
        ec = outToClient->WriteBytes(*cmdResult.GetBytes());

        if (DBG) Slogger::D(TAG, "cmdResult %s", cmdResult.string());
        if (inFromClient != NULL) {
            ICloseable::Probe(inFromClient)->Close();
            inFromClient = NULL;
        }
        if (outToClient != NULL) {
            outToClient->Close();
            outToClient = NULL;
        }
        if (clientSocket != NULL) {
            clientSocket->Close();
            clientSocket = NULL;
        }
    }
    if (inFromClient != NULL)
        ICloseable::Probe(inFromClient)->Close();
    if (outToClient != NULL)
        outToClient->Close();
    if (clientSocket != NULL)
        clientSocket->Close();
}

} // Remote
} // Server
} // Droid
} // Elastos
