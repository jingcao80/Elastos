
#include "remote/ServerSocketTimer.h"
#include "remote/RemoteUtils.h"
#include "remote/ResultMsg.h"
#include "remote/SessionCmdTask.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::CEnvironment;
using Elastos::IO::ICloseable;
using Elastos::IO::CFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::CFileOutputStream;
using Elastos::IO::IBufferedInputStream;
using Elastos::IO::CBufferedInputStream;
using Elastos::IO::IBufferedOutputStream;
using Elastos::IO::CBufferedOutputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IDataOutputStream;
using Elastos::IO::CDataOutputStream;
using Elastos::IO::IFlushable;
using Elastos::Net::IURLDecoder;
using Elastos::Net::CURLDecoder;
using Elastos::Net::IURLEncoder;
using Elastos::Net::CURLEncoder;
using Elastos::Net::CServerSocket;
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

ServerSocketTimer::TimerThread::TimerThreadHandler::TimerThreadHandler(
    /* [in] */ ILooper* looper,
    /* [in] */ TimerThread* host)
    : HandlerBase(looper)
    , mHost(host)
{
}

ECode ServerSocketTimer::TimerThread::TimerThreadHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    const Int64 millisLeft = mHost->mStopTimeInFuture - SystemClock::GetElapsedRealtime();
    if (DBG) Slogger::D(TAG, " HandleMessage  millisLeft1 %lld", millisLeft);
    if (millisLeft <= 0) {
        mHost->OnFinish();
    }
    else if (millisLeft < mHost->mCountdownInterval) {
        if (DBG) Slogger::D(TAG, " HandleMessage  millisLeft2 %lld", millisLeft);
        // no tick, just delay until done
        AutoPtr<IMessage> m;
        ObtainMessage(MSG, (IMessage**)&m);
        Boolean res;
        SendMessageDelayed(m, millisLeft, &res);
    }
    else {
        Int64 lastTickStart = SystemClock::GetElapsedRealtime();
        if (DBG) Slogger::D(TAG, " HandleMessage  lastTickStart %lld", lastTickStart);

        // take into account user's onTick taking time to execute
        Int64 delay = lastTickStart + mHost->mCountdownInterval - SystemClock::GetElapsedRealtime();
        if (DBG) Slogger::D(TAG, " HandleMessage  delay %lld", delay);

        // special case: user's onTick took more than interval to
        // complete, skip to next interval
        while (delay < 0) delay += mHost->mCountdownInterval;

        AutoPtr<IMessage> m;
        ObtainMessage(MSG, (IMessage**)&m);
        Boolean res;
        SendMessageDelayed(m, delay, &res);
    }

    return NOERROR;
}

const Int32 ServerSocketTimer::TimerThread::MSG = 1;

ServerSocketTimer::TimerThread::TimerThread(
    /* [in] */ const String& name,
    /* [in] */ ServerSocketTimer* host)
    : HandlerThread(name)
    , mMillisInFuture(20*60*1000)
    , mCountdownInterval(7*60*1000)
    , mStopTimeInFuture(0)
    , mHost(host)
{
}

/**
 * Callback fired when the time is up.
 */
void ServerSocketTimer::TimerThread::OnFinish()
{
    if (DBG) Slogger::D(TAG, "TimerThread onFinish()");

    if (mHost->mServerSocket != NULL)
        mHost->mServerSocket->Close();

    mHost->ServerSocketTimerFinish();
}

/**
 * Start the countdown.
 */
void ServerSocketTimer::TimerThread::StartCountDown()
{
    AutoLock lock(mLock);
    AutoPtr<ILooper> looper;
    GetLooper((ILooper**)&looper);
    mHandler = new TimerThreadHandler(looper, this);

    if (mMillisInFuture <= 0) {
        OnFinish();
    }
    mStopTimeInFuture = SystemClock::GetElapsedRealtime() + mMillisInFuture;
    AutoPtr<IMessage> m;
    mHandler->ObtainMessage(MSG, (IMessage**)&m);
    Boolean res;
    mHandler->SendMessage(m, &res);

    if (DBG) Slogger::D(TAG, " startCountDown %lld", mStopTimeInFuture);
}

/** reset  mStopTimeInFuture */
void ServerSocketTimer::TimerThread::ResetTimer()
{
    AutoLock lock(mLock);
    mStopTimeInFuture = SystemClock::GetElapsedRealtime() + mMillisInFuture;
}

static String InitMNT_SDCARD()
{
    AutoPtr<IEnvironment> environment;
    CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    AutoPtr<IFile> dir;
    environment->GetExternalStorageDirectory((IFile**)&dir);
    String path;
    dir->GetAbsolutePath(&path);
    return path;
}

const String ServerSocketTimer::TAG("KortideServerSocketTimer");
const Boolean ServerSocketTimer::DBG = RemoteUtils::IsDebug();
const Int32 ServerSocketTimer::CMD_TIMEOUT_SEC = 2;
const Int32 ServerSocketTimer::BUFFER_SIZE = 1024 * 8;
const String ServerSocketTimer::MNT_SDCARD = InitMNT_SDCARD();
const String ServerSocketTimer::DOWNLOADACCESS = String("FDOWN");
const String ServerSocketTimer::UPLOADACCESS = String("FUP");
const String ServerSocketTimer::READYFORRECEIVE = String("READY");

ServerSocketTimer::ServerSocketTimer(
    /* [in] */ IContext* context)
    : mSocketPort(0)
{
    Thread::constructor();

    mContext = context;

    /** initialize a ServerSocket to service client cmd */
    CServerSocket::New(0, (IServerSocket**)&mServerSocket);

    mServerSocket->GetLocalPort(&mSocketPort);
    if (DBG) Slogger::D(TAG, " mSocketPort = %d", mSocketPort);

    /** initialize a timerthread to countdown the alive time */
    mTimerThread = new TimerThread(String("Kortide_ServerSocketTimer"), this);
    mTimerThread->Start();
}

void ServerSocketTimer::AcceptCmd()
{
    Boolean isInterrupted;
    AutoPtr<ISocket> clientSocket;
    AutoPtr<IBufferedReader> inFromClient;
    AutoPtr<IDataOutputStream> outToClient;
    ECode ec = NOERROR;
    while (!(Thread::GetCurrentThread()->IsInterrupted(&isInterrupted), isInterrupted)) {
        ec = mServerSocket->Accept((ISocket**)&clientSocket);
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
            ec = future->Get(CMD_TIMEOUT_SEC, seconds, (IInterface**)&cmdInterface);
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
        cmdResult += "\n";
        ec = outToClient->Write(cmdResult.GetBytes());

        if (DBG) Slogger::D(TAG, "cmdResult %s", cmdResult.string());
        mTimerThread->ResetTimer();
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
        ICloseable::Probe(inFromClient)->Close();;
    if (outToClient != NULL)
        outToClient->Close();
    if (clientSocket != NULL)
        clientSocket->Close();
}

void ServerSocketTimer::FactoryCmd(
    /* [in] */ const String& clientCmd,
    /* [in] */ IOutputStream* outstream)
{
    if (DBG) Slogger::D(TAG, "factoryCmd: %s", clientCmd.string());
    String cmdResult = ResultMsg::ES_NORESPONSE->ToString();
    ECode ec = NOERROR;
    do {
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
            ec = future->Get(CMD_TIMEOUT_SEC, seconds, (IInterface**)&cmdInterface);
            if (FAILED(ec))
                break;
            ICharSequence::Probe(cmdInterface)->ToString(&cmdResult);
            cmdResult = RemoteUtils::ComposeResult(sct->GetResultNum(), cmdResult);
            AutoPtr<IURLEncoder> urlEncoder;
            CURLEncoder::AcquireSingleton((IURLEncoder**)&urlEncoder);
            String temp;
            ec = urlEncoder->Encode(cmdResult, String("UTF-8"), &temp);
            if (FAILED(ec))
                break;
            cmdResult = temp;
        } while (0);

        if (ec == (ECode)E_TIMEOUT_EXCEPTION) {
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
        AutoPtr<IDataOutputStream> outToClient;
        ec = CDataOutputStream::New(outstream, (IDataOutputStream**)&outToClient);
        if (FAILED(ec))
            break;
        cmdResult += '\n';
        ec = outToClient->Write(cmdResult.GetBytes());

        if (DBG) Slogger::D(TAG, "cmdResult %s", cmdResult.string());
        if (outToClient != NULL)
            outToClient->Close();
    } while (0);
}

ECode ServerSocketTimer::SendFile(
    /* [in] */ const String& filepath,
    /* [in] */ IOutputStream* outstream)
{
    //DeviceManager devMana = new DeviceManager(mContext);
    if (DBG) Slogger::D(TAG, "sendFile %s", filepath.string());
    AutoPtr<IBufferedOutputStream> bos;
    AutoPtr<IDataOutputStream> dos;
    AutoPtr<IFileInputStream> fis;
    AutoPtr<IBufferedInputStream> bis;
    ECode ec = NOERROR;
    do {
        ec = CBufferedOutputStream::New(outstream, (IBufferedOutputStream**)&bos);
        if (FAILED(ec))
            break;
        ec = CDataOutputStream::New(bos, (IDataOutputStream**)&dos);
        if (FAILED(ec))
            break;

        ec = CFileInputStream::New(filepath, (IFileInputStream**)&fis);
        if (FAILED(ec))
            break;
        ec = CBufferedInputStream::New(fis, (IBufferedInputStream**)&bis);
        if (FAILED(ec))
            break;

        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(BUFFER_SIZE);
        Int32 bytesReaded = 0;
        while ((bis->ReadBytes(buffer, 0, BUFFER_SIZE, &bytesReaded), bytesReaded) > -1) {
            if (DBG) Slogger::D(TAG, "readBuffer %d", bytesReaded);
            dos->Write(buffer, 0, bytesReaded);
        }
        ec = IFlushable::Probe(dos)->Flush();
    } while (0);

    if(bis != NULL)
        bis->Close();
    if(fis != NULL)
        fis->Close();
    if(dos != NULL)
        dos->Close();
    if(bos != NULL)
        bos->Close();

    return ec;
}

ECode ServerSocketTimer::ReceiveFile(
    /* [in] */ const String& filepath,
    /* [in] */ ISocket* clientSocket)
{
    if (DBG) Slogger::D(TAG, "receiveFile %s", filepath.string());
    AutoPtr<IBufferedInputStream> bis;
    AutoPtr<IDataInputStream> dis;
    AutoPtr<IFileOutputStream> fos;
    AutoPtr<IDataOutputStream> outToClient;
    ECode ec = NOERROR;
    do {
        AutoPtr<IInputStream> inputStream;
        ec = clientSocket->GetInputStream((IInputStream**)&inputStream);
        if (FAILED(ec))
            break;
        ec = CBufferedInputStream::New(inputStream, (IBufferedInputStream**)&bis);
        if (FAILED(ec))
            break;
        ec = CDataInputStream::New(bis, (IDataInputStream**)&dis);
        if (FAILED(ec))
            break;

        AutoPtr<IOutputStream> outputStream;
        ec = clientSocket->GetOutputStream((IOutputStream**)&outputStream);
        if (FAILED(ec))
            break;
        ec = CDataOutputStream::New(outputStream, (IDataOutputStream**)&outToClient);
        if (FAILED(ec))
            break;

        String str = READYFORRECEIVE + "\n";
        ec = outToClient->Write(str.GetBytes());
        if (FAILED(ec))
            break;

        AutoPtr<IFile> outfile;
        ec = CFile::New(filepath, (IFile**)&outfile);
        if (FAILED(ec))
            break;
        ec = CFileOutputStream::New(outfile, (IFileOutputStream**)&fos);
        if (FAILED(ec))
            break;
        AutoPtr<ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(BUFFER_SIZE);
        Int32 bytesReaded = 0;
        while ((dis->ReadBytes(buffer, 0, BUFFER_SIZE, &bytesReaded), bytesReaded) > -1) {
            if (DBG) Slogger::D(TAG, "readBuffer %d", bytesReaded);
            fos->Write(buffer, 0, bytesReaded);
            IFlushable::Probe(fos)->Flush();
        }
    } while (0);

    if (fos != NULL)
        fos->Close();
    if (dis != NULL)
        dis->Close();
    if (bis != NULL)
        bis->Close();
    if (outToClient != NULL)
        outToClient->Close();

    return ec;
}

void ServerSocketTimer::HandleCmd()
{
    Boolean isInterrupted;
    ECode ec = NOERROR;
    AutoPtr<ISocket> clientSocket;
    AutoPtr<IBufferedReader> inFromClient;
    while (!(Thread::GetCurrentThread()->IsInterrupted(&isInterrupted), isInterrupted)) {
        ec = mServerSocket->Accept((ISocket**)&clientSocket);
        if (FAILED(ec))
            break;

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

        if (clientCmd.StartWith(DOWNLOADACCESS)) {
            AutoPtr<ArrayOf<String> > parts;
            StringUtils::Split(clientCmd, RemoteUtils::SPLITER, (ArrayOf<String>**)&parts);
            AutoPtr<IOutputStream> outputStream;
            ec = clientSocket->GetOutputStream((IOutputStream**)&outputStream);
            if (FAILED(ec))
                break;
            String filePath = (*parts)[1];
            SendFile(filePath, outputStream);
        }
        else if(clientCmd.StartWith(UPLOADACCESS)) {
            AutoPtr<ArrayOf<String> > parts;
            StringUtils::Split(clientCmd, RemoteUtils::SPLITER, (ArrayOf<String>**)&parts);
            String filePath = (*parts)[1];
            ReceiveFile(filePath, clientSocket);
        }
        else {
            AutoPtr<IOutputStream> outputStream;
            ec = clientSocket->GetOutputStream((IOutputStream**)&outputStream);
            if (FAILED(ec))
                break;
            FactoryCmd(clientCmd, outputStream);
        }

        mTimerThread->ResetTimer();
        if (inFromClient != NULL) {
            ICloseable::Probe(inFromClient)->Close();
            inFromClient = NULL;
        }
        if (clientSocket != NULL) {
            clientSocket->Close();
            clientSocket = NULL;
        }
    }

    if (inFromClient != NULL)
        ICloseable::Probe(inFromClient)->Close();
    if (clientSocket != NULL)
        clientSocket->Close();
}

AutoPtr<IFile> ServerSocketTimer::GetCalledFile(
    /* [in] */ const String& name)
{
    // way1
    // AutoPtr<IEnvironment> environment;
    // CEnvironment::AcquireSingleton((IEnvironment**)&environment);
    // AutoPtr<IFile> dataDir;
    // environment->GetDataDirectory((IFile**)&dataDir);
    // AutoPtr<IFile> fname;
    // CFile::New(dataDir, name, (IFile**)&fname);
    // return fname;

    //way2
    AutoPtr<IFile> dataDir;
    CFile::New(String("/mnt/sata"), (IFile**)&dataDir);
    AutoPtr<IFile> systemDir;
    CFile::New(dataDir, String("8_1/SD"), (IFile**)&systemDir);
    AutoPtr<IFile> fname;
    CFile::New(systemDir, name, (IFile**)&fname);
    return fname;
}

void ServerSocketTimer::ServerSocketTimerFinish()
{
    if (DBG) Slogger::D(TAG, "ServerSocketTimerFinish()");
    Thread::GetCurrentThread()->Interrupt();
}

ECode ServerSocketTimer::Run()
{
    mTimerThread->StartCountDown();
    //acceptCmd();
    HandleCmd();
    return NOERROR;
}

Int32 ServerSocketTimer::GetSocketPort()
{
    return mSocketPort;
}

} // Remote
} // Server
} // Droid
} // Elastos
