
#include "elastos/droid/server/NativeDaemonConnector.h"
#include "elastos/droid/server/FgThread.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include "elastos/core/CoreUtils.h"
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>

using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Net::LocalSocketAddressNamespace_RESERVED;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::Core::EIID_IRunnable;
using Elastos::IO::IInputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::Concurrent::IArrayBlockingQueue;
using Elastos::Utility::Concurrent::CArrayBlockingQueue;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

CAR_INTERFACE_IMPL(NativeDaemonConnector::SensitiveArg, Object, ISensitiveArg)

//==============================================================================
// NativeDaemonConnector::Command
//==============================================================================
NativeDaemonConnector::Command::Command(
    /* [in] */ const String& cmd,
    /* [in] */ ArrayOf<IInterface*>* args)
    : mCmd(cmd)
{
    if (args != NULL) {
        for (Int32 i = 0; i < args->GetLength(); ++i) {
            AppendArg((*args)[i]);
        }
    }
}

void NativeDaemonConnector::Command::AppendArg(
    /* [in] */ IInterface* arg)
{
    mArguments.PushBack(arg);
}

//==============================================================================
// NativeDaemonConnector::ResponseQueue::PendingCmd
//==============================================================================
NativeDaemonConnector::ResponseQueue::PendingCmd::PendingCmd(
    /* [in] */ Int32 c,
    /* [in] */ const String& r)
    : mCmdNum(c)
    , mLogCmd(r)
{
    CArrayBlockingQueue::New(10, (IBlockingQueue**)&mResponses);
}

//==============================================================================
// NativeDaemonConnector::ResponseQueue
//==============================================================================
NativeDaemonConnector::ResponseQueue::ResponseQueue(
    /* [in] */ Int32 maxCount)
    : mMaxCount(maxCount)
{
}

void NativeDaemonConnector::ResponseQueue::Add(
    /* [in] */ Int32 cmdNum,
    /* [in] */ NativeDaemonEvent* response)
{
    AutoPtr<PendingCmd> found;
    {
        AutoLock lock(mPendingCmdsLock);

        List< AutoPtr<PendingCmd> >::Iterator iter;
        for (iter = mPendingCmds.Begin(); iter != mPendingCmds.End(); ++iter) {
            AutoPtr<PendingCmd> pendingCmd = *iter;
            if (pendingCmd->mCmdNum == cmdNum) {
                found = pendingCmd;
                break;
            }
        }
        if (found == NULL) {
            // didn't find it - make sure our queue isn't too big before adding
            while (mPendingCmds.GetSize() >= mMaxCount) {
//                Slog.e("NativeDaemonConnector.ResponseQueue",
//                        "more buffered than allowed: " + mPendingCmds.size() +
//                        " >= " + mMaxCount);
                // let any waiter timeout waiting for this
                AutoPtr<PendingCmd> pendingCmd = *mPendingCmds.Begin();
                mPendingCmds.PopFront();
//                Slog.e("NativeDaemonConnector.ResponseQueue",
//                        "Removing request: " + pendingCmd.mLogCmd + " (" +
//                        pendingCmd.cmdNum + ")");
            }
            found = new PendingCmd(cmdNum, String(NULL));
            mPendingCmds.PushBack(found);
        }
        found->mAvailableResponseCount++;
        // if a matching remove call has already retrieved this we can remove this
        // instance from our list
        if (found->mAvailableResponseCount == 0) mPendingCmds.Remove(found);
    }
//    try {
    AutoLock lock(found->mResponses);
    found->mResponses->Put(TO_IINTERFACE(response));
//    } catch (InterruptedException e) { }
}

AutoPtr<NativeDaemonEvent> NativeDaemonConnector::ResponseQueue::Remove(
    /* [in] */ Int32 cmdNum,
    /* [in] */ Int32 timeoutMs,
    /* [in] */ const String& logCmd)
{
    AutoPtr<PendingCmd> found;
    {
        AutoLock lock(mPendingCmdsLock);

        List< AutoPtr<PendingCmd> >::Iterator iter;
        for (iter = mPendingCmds.Begin(); iter != mPendingCmds.End(); ++iter) {
            AutoPtr<PendingCmd> pendingCmd = *iter;
            if (pendingCmd->mCmdNum == cmdNum) {
                found = pendingCmd;
                break;
            }
        }
        if (found == NULL) {
            found = new PendingCmd(cmdNum, logCmd);
            mPendingCmds.PushBack(found);
        }
        found->mAvailableResponseCount--;
        // if a matching add call has already retrieved this we can remove this
        // instance from our list
        if (found->mAvailableResponseCount == 0) mPendingCmds.Remove(found);
    }

    AutoPtr<NativeDaemonEvent> result;

//    try {
    AutoPtr<ITimeUnitHelper> helper;
    CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
    AutoPtr<ITimeUnit> MILLISECONDS;
    helper->GetMILLISECONDS((ITimeUnit**)&MILLISECONDS);
    AutoPtr<IInterface> obj;
    ECode ec = found->mResponses->Poll(timeoutMs, MILLISECONDS, (IInterface**)&obj);
//    } catch (InterruptedException e) {}
    if (obj == NULL || ec == (ECode)E_INTERRUPTED_EXCEPTION) {
        Slogger::E("NativeDaemonConnector.ResponseQueue", "Timeout waiting for response");
    }
    else {
        result = (NativeDaemonEvent*)IObject::Probe(obj);
    }
    return result;
}

void NativeDaemonConnector::ResponseQueue::Dump(
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* pw,
    /* [in] */ ArrayOf<IInterface*>* args)
{
//    pw->Println(String("Pending requests:"));
//    {
//        AutoLock lock(mPendingCmdsLock);
//
//        List< AutoPtr<PendingCmd> >::Iterator iter;
//        for (iter = mPendingCmds.Begin(); iter != mPendingCmds.End(); ++iter) {
//            AutoPtr<PendingCmd> pendingCmd = *iter;
//            pw->Println("  Cmd " + pendingCmd->cmdNum + " - " + pendingCmd->request);
//        }
//    }
}

//==============================================================================
// NativeDaemonConnector
//==============================================================================

const Boolean NativeDaemonConnector::LOGD = FALSE;
const Boolean NativeDaemonConnector::VDBG = FALSE;
const Int32 NativeDaemonConnector::DEFAULT_TIMEOUT;
const Int64 NativeDaemonConnector::WARN_EXECUTE_DELAY_MS;
const Int32 NativeDaemonConnector::BUFFER_SIZE;

CAR_INTERFACE_IMPL_2(NativeDaemonConnector, Object, IRunnable, IHandlerCallback)

NativeDaemonConnector::NativeDaemonConnector(
    /* [in] */ INativeDaemonConnectorCallbacks* callbacks,
    /* [in] */ const String& socket,
    /* [in] */ Int32 responseQueueSize,
    /* [in] */ const String& logTag,
    /* [in] */ Int32 maxLogSize,
    /* [in] */ IPowerManagerWakeLock* wl)
    : mSocket(socket)
    , mCallbacks(callbacks)
{
    mResponseQueue = new ResponseQueue(responseQueueSize);
    mWakeLock = wl;
    if (mWakeLock != NULL) {
        mWakeLock->SetReferenceCounted(TRUE);
    }

    AutoPtr<FgThread> fgThread = FgThread::Get();
    fgThread->GetLooper((ILooper**)&mLooper);
    CAtomicInteger32::New(0, (IAtomicInteger32**)&mSequenceNumber);
    TAG = logTag != NULL ? logTag : String("NativeDaemonConnector");
    // mLocalLog = new LocalLog(maxLogSize);

}

NativeDaemonConnector::NativeDaemonConnector(
    /* [in] */ INativeDaemonConnectorCallbacks* callbacks,
    /* [in] */ const String& socket,
    /* [in] */ Int32 responseQueueSize,
    /* [in] */ const String& logTag,
    /* [in] */ Int32 maxLogSize,
    /* [in] */ IPowerManagerWakeLock* wl,
    /* [in] */ ILooper* looper)
    : mSocket(socket)
    , mCallbacks(callbacks)
{
    mResponseQueue = new ResponseQueue(responseQueueSize);
    mWakeLock = wl;
    if (mWakeLock != NULL) {
        mWakeLock->SetReferenceCounted(TRUE);
    }
    mLooper = looper;
    CAtomicInteger32::New(0, (IAtomicInteger32**)&mSequenceNumber);
    TAG = logTag != NULL ? logTag : String("NativeDaemonConnector");
    // mLocalLog = new LocalLog(maxLogSize);
}

ECode NativeDaemonConnector::Run()
{
    CHandler::New(mLooper, this, FALSE, (IHandler**)&mCallbackHandler);

    while (TRUE) {
//        try {
        ECode ec = ListenToSocket();
        if (FAILED(ec)) {
            Slogger::E(TAG, "Error in NativeDaemonConnector: 0x%08x", ec);
            SystemClock::Sleep(5000);
        }
        // } catch (Exception e) {
        // loge("Error in NativeDaemonConnector: " + e);
        // SystemClock.sleep(5000);
        // }
    }
    return NOERROR;
}

ECode NativeDaemonConnector::HandleMessage(
   /* [in] */ IMessage* msg,
   /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    ICharSequence* seq = ICharSequence::Probe(obj);
    assert(seq != NULL);
    String event;
    seq->ToString(&event);

//    try {
    Boolean bval;
    ECode ec = mCallbacks->OnEvent(what, event, NativeDaemonEvent::UnescapeArgs(event), &bval);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Error handling event [%s]", event.string());
    }
    else if (!bval) {
        Slogger::D(TAG, "Unhandled event [%s]", event.string());
    }
//    } catch (Exception e) {
//        loge("Error handling '" + event + "': " + e);
//    }
    if (result) {
        *result = bval;
    }

    mCallbacks->OnCheckHoldWakeLock(what, &bval);
    if (bval && mWakeLock != NULL) {
        mWakeLock->ReleaseLock();
    }
    return NOERROR;
}

AutoPtr<ILocalSocketAddress> NativeDaemonConnector::DetermineSocketAddress()
{
    AutoPtr<ILocalSocketAddress> lsa;
    // If we're testing, set up a socket in a namespace that's accessible to test code.
    // In order to ensure that unprivileged apps aren't able to impersonate native daemons on
    // production devices, even if said native daemons ill-advisedly pick a socket name that
    // starts with __test__, only allow this on debug builds.
    if (mSocket.StartWith("__test__") && Build::IS_DEBUGGABLE) {
        CLocalSocketAddress::New(mSocket, (ILocalSocketAddress**)&lsa);
    } else {
        CLocalSocketAddress::New(mSocket, LocalSocketAddressNamespace_RESERVED, (ILocalSocketAddress**)&lsa);
    }
    return lsa;
}

ECode NativeDaemonConnector::ListenToSocket()
{
    AutoPtr<ILocalSocket> socket;

//    try {
    FAIL_RETURN(CLocalSocket::New((ILocalSocket**)&socket));
    AutoPtr<ILocalSocketAddress> socketAddress = DetermineSocketAddress();

    ECode ec = socket->Connect(socketAddress);
    if (FAILED(ec)) {
        ICloseable::Probe(socket)->Close();
        return ec;
    }

    AutoPtr<IInputStream> inputStream;
    socket->GetInputStream((IInputStream**)&inputStream);
    {
        AutoLock lock(mDaemonLock);
        mOutputStream = NULL;
        socket->GetOutputStream((IOutputStream**)&mOutputStream);
    }

    mCallbacks->OnDaemonConnected();

    AutoPtr< ArrayOf<Byte> > buffer = ArrayOf<Byte>::Alloc(BUFFER_SIZE);
    Int32 start = 0;

    AutoPtr<IMessageHelper> helper;
    CMessageHelper::AcquireSingleton((IMessageHelper**)&helper);
    Boolean bval;

    while (TRUE) {
        Int32 count;
        ec = inputStream->Read(buffer, start, BUFFER_SIZE - start, &count);
        if (FAILED(ec)) {
            goto _EXIT_;
        }
        if (count < 0) {
            Slogger::E(TAG, "got %d reading with start = %d", count, start);
            break;
        }

        // Add our starting point to the count and reset the start.
        count += start;
        start = 0;

        for (Int32 i = 0; i < count; i++) {
            if ((*buffer)[i] == 0) {
//                    const String rawEvent = new String(
//                            buffer, start, i - start, Charsets.UTF_8);
                String rawEvent((char*)buffer->GetPayload() + start, i - start);
                if (LOGD) Slogger::D(TAG, "RCV <- {%s}", rawEvent.string());

                Boolean releaseWl = FALSE;
//                try {
                AutoPtr<NativeDaemonEvent> event;
                ec = NativeDaemonEvent::ParseRawEvent(rawEvent, (NativeDaemonEvent**)&event);
                if (FAILED(ec)) {
                    if (LOGD) Slogger::D(TAG, "ListenToSocket line:%d", __LINE__);
                    goto _EXIT_;
                }
                if (event->IsClassUnsolicited()) {
                    if (LOGD) Slogger::D(TAG, "ListenToSocket line:%d", __LINE__);
                    // TODO: migrate to sending NativeDaemonEvent instances
                    Int32 code = event->GetCode();
                    mCallbacks->OnCheckHoldWakeLock(code, &bval);
                    if (bval && mWakeLock != NULL) {
                        mWakeLock->AcquireLock();
                        releaseWl = TRUE;
                    }
                    AutoPtr<ICharSequence> seq;
                    CString::New(event->GetRawEvent(), (ICharSequence**)&seq);
                    AutoPtr<IMessage> msg;
                    mCallbackHandler->ObtainMessage(code, seq.Get(), (IMessage**)&msg);
                    mCallbackHandler->SendMessage(msg, &bval);
                    if (bval) {
                        releaseWl = FALSE;
                    }
                }
                else {
                    if (LOGD) Slogger::D(TAG, "add receive event to responseQueue ListenToSocket line:%d", __LINE__);
                    mResponseQueue->Add(event->GetCmdNumber(), event);
                }
//                } catch (IllegalArgumentException e) {
//                    log("Problem parsing message: " + rawEvent + " - " + e);
//                }
                // finally
                {
                    if (releaseWl) {
                        mWakeLock->AcquireLock();
                    }
                }
                start = i + 1;
            }
        }

        if (start == 0) {
//                const String rawEvent = new String(buffer, start, count, Charsets.UTF_8);
            String rawEvent((char*)buffer->GetPayload() + start, count);
            if (LOGD) Slogger::D(TAG, "RCV incomplete <- {%s}", rawEvent.string());
        }

        // We should end at the amount we read. If not, compact then
        // buffer and read again.
        if (start != count) {
            const Int32 remaining = BUFFER_SIZE - start;
            buffer->Copy(buffer, start, remaining);
            start = remaining;
        }
        else {
            start = 0;
        }
    }

_EXIT_:
    {
        AutoLock lock(mDaemonLock);

        if (mOutputStream != NULL) {
            // try {
            //     loge("closing stream for " + mSocket);
            if (ICloseable::Probe(mOutputStream)) {
                ICloseable::Probe(mOutputStream)->Close();
            }
            // } catch (IOException e) {
            //     loge("Failed closing output stream: " + e);
            // }
            mOutputStream = NULL;
        }
    }
    // try {
    if (socket != NULL) {
        ICloseable::Probe(socket)->Close();
        socket = NULL;
    }
    // } catch (IOException ex) {
    //     loge("Failed closing socket: " + ex);
    // }

//    } catch (IOException ex) {
//        loge("Communications error: " + ex);
//        throw ex;
//    } finally {
//        synchronized(mDaemonLock) {
//            if (mOutputStream != NULL) {
//                try {
//                    loge("closing stream for " + mSocket);
//                    mOutputStream.close();
//                } catch (IOException e) {
//                    loge("Failed closing output stream: " + e);
//                }
//                mOutputStream = NULL;
//            }
//        }
//        try {
//            if (socket != NULL) {
//                socket.close();
//            }
//        } catch (IOException ex) {
//            loge("Failed closing socket: " + ex);
//        }
//    }
    return ec;
}

ECode NativeDaemonConnector::MakeCommand(
    /* [in] */ StringBuilder& rawBuilder,
    /* [in] */ StringBuilder& logBuilder,
    /* [in] */ Int32 sequenceNumber,
    /* [in] */ const String& cmd,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    if (cmd.IndexOf((Char32)'\0') >= 0) {
        Slogger::E(TAG, "Unexpected command: %s", cmd.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (cmd.IndexOf((Char32)' ') >= 0) {
        Slogger::E(TAG, "Arguments must be separate from command");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    rawBuilder.Append(sequenceNumber);
    rawBuilder.Append(" ");
    rawBuilder.Append(cmd);
    logBuilder.Append(sequenceNumber);
    logBuilder.Append(" ");
    logBuilder.Append(cmd);
    for (Int32 i = 0; i < args->GetLength(); ++i) {
        IInterface* arg = (*args)[i];
        String argString = Object::ToString(arg);
        if (argString.IndexOf((Char32)'\0') >= 0) {
            Slogger::E(TAG, "Unexpected argument: %s", argString.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        rawBuilder.Append(" ");
        logBuilder.Append(" ");

        AppendEscaped(rawBuilder, argString);
        if (ISensitiveArg::Probe(arg) != NULL) {
            logBuilder.Append("[scrubbed]");
        }
        else {
            AppendEscaped(logBuilder, argString);
        }
    }

    rawBuilder.AppendChar('\0');
    return NOERROR;
}

ECode NativeDaemonConnector::Execute(
    /* [in] */ Command* cmd,
    /* [out] */ NativeDaemonEvent** event)
{
    VALIDATE_NOT_NULL(event);
    *event = NULL;

    List< AutoPtr<IInterface> >& args = cmd->mArguments;
    AutoPtr< ArrayOf<IInterface*> > argsArray = ArrayOf<IInterface*>::Alloc(args.GetSize());
    List< AutoPtr<IInterface> >::Iterator it;
    Int32 i;
    for (it = args.Begin(), i = 0; it != args.End(); ++it, ++i) {
        argsArray->Set(i, *it);
    }
    return Execute(cmd->mCmd, argsArray, event);
}

ECode NativeDaemonConnector::Execute(
    /* [in] */ const String& cmd,
    /* [in] */ const String& arg,
    /* [out] */ NativeDaemonEvent** event)
{
    VALIDATE_NOT_NULL(event);
    *event = NULL;

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    args->Set(0, CoreUtils::Convert(arg));
    return Execute(cmd, args, event);
}

ECode NativeDaemonConnector::Execute(
    /* [in] */ const String& cmd,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out] */ NativeDaemonEvent** event)
{
    VALIDATE_NOT_NULL(event);
    *event = NULL;

    AutoPtr< ArrayOf<NativeDaemonEvent*> > events;
    FAIL_RETURN(ExecuteForList(cmd, args, (ArrayOf<NativeDaemonEvent*>**)&events));
    if (events == NULL || events->GetLength() != 1) {
        Slogger::E(TAG, "Expected exactly one response, but received %d", events ? events->GetLength() : 0);
        return E_NATIVE_DAEMON_CONNECTOR_EXCEPTION;
    }

    *event = (*events)[0];
    REFCOUNT_ADD(*event);
    return NOERROR;
}

ECode NativeDaemonConnector::ExecuteForList(
    /* [in] */ Command* cmd,
    /* [out, callee] */ ArrayOf<NativeDaemonEvent*>** events)
{
    VALIDATE_NOT_NULL(events);
    *events = NULL;

    List< AutoPtr<IInterface> >& args = cmd->mArguments;
    AutoPtr< ArrayOf<IInterface*> > argsArray = ArrayOf<IInterface*>::Alloc(args.GetSize());
    List< AutoPtr<IInterface> >::Iterator it;
    Int32 i;
    for (it = args.Begin(), i = 0; it != args.End(); ++it, ++i) {
        argsArray->Set(i, *it);
    }
    return ExecuteForList(cmd->mCmd, argsArray, events);
}

ECode NativeDaemonConnector::ExecuteForList(
    /* [in] */ const String& cmd,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out, callee] */ ArrayOf<NativeDaemonEvent*>** events)
{
    return Execute(DEFAULT_TIMEOUT, cmd, args, events);
}

ECode NativeDaemonConnector::Execute(
    /* [in] */ Int32 timeout,
    /* [in] */ const String& cmd,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out, callee] */ ArrayOf<NativeDaemonEvent*>** eventsArray)
{
    VALIDATE_NOT_NULL(eventsArray);
    *eventsArray = NULL;

    Int64 startTime = SystemClock::GetElapsedRealtime();
    List< AutoPtr<NativeDaemonEvent> > events;

    StringBuilder rawBuilder;
    StringBuilder logBuilder;
    Int32 sequenceNumber;
    mSequenceNumber->IncrementAndGet(&sequenceNumber);

    MakeCommand(rawBuilder, logBuilder, sequenceNumber, cmd, args);

    String rawCmd = rawBuilder.ToString();
    String logCmd = logBuilder.ToString();

    if (LOGD) Slogger::D(TAG, "Execute: SND -> [%s]", logCmd.string());

    {
        AutoLock lock(mDaemonLock);

        if (mOutputStream == NULL) {
            *eventsArray = ArrayOf<NativeDaemonEvent*>::Alloc(0);
            REFCOUNT_ADD(*eventsArray);
            Slogger::E(TAG, "missing output stream");
            return E_NATIVE_DAEMON_CONNECTOR_EXCEPTION;
        }
        else {
            AutoPtr<ArrayOf<Byte> > bytesWithoutEnd = rawCmd.GetBytes();
            Int32 length = bytesWithoutEnd->GetLength();
            AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(length + 1);
            bytes->Copy(bytesWithoutEnd, length);
            (*bytes)[length] = (Byte)('\0');

            ECode ec = mOutputStream->Write(bytes);
            if (FAILED(ec)) {
                *eventsArray = ArrayOf<NativeDaemonEvent*>::Alloc(0);
                REFCOUNT_ADD(*eventsArray);
                Slogger::E(TAG, "problem sending command, ec=%08x", ec);
                return E_NATIVE_DAEMON_CONNECTOR_EXCEPTION;
            }
        }
    }

    AutoPtr<NativeDaemonEvent> event;
    do {
        if (LOGD) Slogger::D(TAG, "Execute line:%d", __LINE__);
        event = mResponseQueue->Remove(sequenceNumber, timeout, logCmd);
        if (event == NULL) {
            *eventsArray = ArrayOf<NativeDaemonEvent*>::Alloc(0);
            REFCOUNT_ADD(*eventsArray);
            Slogger::E(TAG, "timed-out waiting for response to %s.", logCmd.string());
            return E_NATIVE_DAEMON_CONNECTOR_EXCEPTION;
        }
        if (VDBG) Slogger::D(TAG, "RMV <- {%s}, line:%d", event->GetMessage().string(), __LINE__);
        events.PushBack(event);
    } while (event->IsClassContinue());

    const Int64 endTime = SystemClock::GetElapsedRealtime();
    if (endTime - startTime > WARN_EXECUTE_DELAY_MS) {
//        loge("NDC Command {" + logCmd + "} took too Int64 (" + (endTime - startTime) + "ms)");
    }

    if (event->IsClassClientError()) {
        Slogger::D(TAG, "Execute: NativeDaemonArgumentException -> [%s]", logCmd.string());
//        throw new NativeDaemonArgumentException(logCmd, event);
        *eventsArray = ArrayOf<NativeDaemonEvent*>::Alloc(1);
        REFCOUNT_ADD(*eventsArray);
        (*eventsArray)->Set(0, event);
        return E_NATIVE_DAEMON_ARGUMENT_EXCEPTION;
    }

    if (event->IsClassServerError()) {
        Slogger::D(TAG, "Execute: NativeDaemonFailureException -> [%s]", logCmd.string());
//        throw new NativeDaemonFailureException(logCmd, event);
        *eventsArray = ArrayOf<NativeDaemonEvent*>::Alloc(1);
        REFCOUNT_ADD(*eventsArray);
        (*eventsArray)->Set(0, event);
        return E_NATIVE_DAEMON_FAILURE_EXCEPTION;
    }

    *eventsArray = ArrayOf<NativeDaemonEvent*>::Alloc(events.GetSize());
    REFCOUNT_ADD(*eventsArray);
    List< AutoPtr<NativeDaemonEvent> >::Iterator it;
    Int32 i;
    for (it = events.Begin(), i = 0; it != events.End(); ++it, ++i) {
        (*eventsArray)->Set(i, *it);
    }
    return NOERROR;
}

void NativeDaemonConnector::AppendEscaped(
    /* [in] */ StringBuilder& builder,
    /* [in] */ const String& arg)
{
    const Boolean hasSpaces = arg.IndexOf(' ') >= 0;
    if (hasSpaces) {
        builder.AppendChar('"');
    }

    const Int32 length = arg.GetLength();
    for (Int32 i = 0; i < length; i++) {
        const Char32 c = arg.GetChar(i);

        if (c == '"') {
            builder.Append("\\\"");
        }
        else if (c == '\\') {
            builder.Append("\\\\");
        }
        else {
            builder.AppendChar(c);
        }
    }

    if (hasSpaces) {
        builder.AppendChar('"');
    }
}

///** {@inheritDoc} */
void NativeDaemonConnector::Monitor()
{
    // TODO
    // Watchdog.Monitor
}
//
//void NativeDaemonConnector::Dump(
///* [in] */ AutoPtr<IFileDescriptor> fd,
///* [in] */ AutoPtr<IPrintWriter> pw,
///* [in] */ ArrayOf<String> args)
//{
////    mLocalLog.dump(fd, pw, args);
//    pw->Println();
//    mResponseQueue->Dump(fd, pw, args);
//}
//
//void NativeDaemonConnector::Log(
///* [in] */ const String& logstring)
//{
////    if (LOGD) Slog.d(TAG, logstring);
////    mLocalLog.log(logstring);
//}
//
//void NativeDaemonConnector::Loge(
//    /* [in] */ const String& logstring)
//{
////    Slog.e(TAG, logstring);
////    mLocalLog.log(logstring);
//}
//

} // namespace Server
} // namespace Droid
} // namespace Elastos
