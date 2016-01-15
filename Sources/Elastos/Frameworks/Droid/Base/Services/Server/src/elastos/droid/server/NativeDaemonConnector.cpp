
#include "NativeDaemonConnector.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/Handler.h"
#include "Elastos.Droid.Server_server.h"
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>

using Elastos::Core::StringUtils;
using Elastos::Core::IInteger32;
using Elastos::Core::IInteger64;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::Thread;
using Elastos::Core::IThread;
using Elastos::IO::IInputStream;
using Elastos::IO::ICloseable;
using Elastos::Utility::Concurrent::IArrayBlockingQueue;
using Elastos::Utility::Concurrent::CArrayBlockingQueue;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Logging::Slogger;
using Elastos::Droid::Net::ILocalSocket;
using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Net::LocalSocketAddressNamespace_RESERVED;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::EIID_IHandlerCallback;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::IMessageHelper;
using Elastos::Droid::Os::CMessageHelper;

namespace Elastos {
namespace Droid {
namespace Server {

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
    , mRequest(r)
{
    //CArrayBlockingQueue::New(10, (IArrayBlockingQueue**)&mResponses);
    //assert(mResponses != NULL);
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
//                        "Removing request: " + pendingCmd.request + " (" +
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
    // TODO: delete
    AutoLock lock(found->mResponsesLock);
    found->mResponses.Push(response);
    //found->mResponses->Put(response->Probe(EIID_IInterface));
//    } catch (InterruptedException e) { }
}

AutoPtr<NativeDaemonEvent> NativeDaemonConnector::ResponseQueue::Remove(
    /* [in] */ Int32 cmdNum,
    /* [in] */ Int32 timeoutMs,
    /* [in] */ const String& origCmd)
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
            found = new PendingCmd(cmdNum, origCmd);
            mPendingCmds.PushBack(found);
        }
        found->mAvailableResponseCount--;
        // if a matching add call has already retrieved this we can remove this
        // instance from our list
        if (found->mAvailableResponseCount == 0) mPendingCmds.Remove(found);
    }


    AutoPtr<NativeDaemonEvent> result;
    // TODO:delete
    AutoPtr<IThread> thread;
    Thread::Attach((IThread**)&thread);
    do {
        {
            AutoLock lock(found->mResponsesLock);
            if (!found->mResponses.IsEmpty()) {
                result = found->mResponses.GetFront();
                found->mResponses.Pop();
                break;
            }
        }
        SystemClock::Sleep(1000);
        timeoutMs -= 1000;
    } while (timeoutMs > 0);

//    try {
    //AutoPtr<ITimeUnitHelper> helper;
    //CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&helper);
    //AutoPtr<ITimeUnit> MILLISECONDS;
    //helper->GetMILLISECONDS((ITimeUnit**)&MILLISECONDS);
    //AutoPtr<IInterface> obj;
    //found->mResponses->Poll(timeoutMs, MILLISECONDS, (IInterface**)&obj);
    // if (obj != NULL) {
    //     result = reinterpret_cast<NativeDaemonEvent*>(obj->Probe(EIID_NativeDaemonEvent));
    // }
//    } catch (InterruptedException e) {}
    if (result == NULL) {
        Slogger::E("NativeDaemonConnector.ResponseQueue", "Timeout waiting for response");
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

const Int32 NativeDaemonConnector::DEFAULT_TIMEOUT;
const Int64 NativeDaemonConnector::WARN_EXECUTE_DELAY_MS;
const Int32 NativeDaemonConnector::BUFFER_SIZE;
const Boolean NativeDaemonConnector::LOGD = FALSE;

NativeDaemonConnector::NativeDaemonConnector(
    /* [in] */ INativeDaemonConnectorCallbacks* callbacks,
    /* [in] */ const String& socket,
    /* [in] */ Int32 responseQueueSize,
    /* [in] */ const String& logTag,
    /* [in] */ Int32 maxLogSize)
    : mSocket(socket)
    , mCallbacks(callbacks)
{
    mResponseQueue = new ResponseQueue(responseQueueSize);
    CAtomicInteger32::New(0, (IAtomicInteger32**)&mSequenceNumber);
    TAG = !logTag.IsNull() ? logTag : String("NativeDaemonConnector");
//    mLocalLog = new LocalLog(maxLogSize);
}

PInterface NativeDaemonConnector::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (PInterface)(IRunnable*)this;
    }
    else if (riid == Elastos::Core::EIID_IRunnable) {
        return (IRunnable*)this;
    }
    else if (riid == Elastos::Droid::Os::EIID_IHandlerCallback) {
        return (IHandlerCallback*)this;
    }
    else if (riid == EIID_IWeakReferenceSource) {
        return (IWeakReferenceSource*)this;
    }

    return NULL;
}

UInt32 NativeDaemonConnector::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 NativeDaemonConnector::Release()
{
    return ElRefBase::Release();
}

ECode NativeDaemonConnector::GetInterfaceID(
    /* [in] */ IInterface* pObject,
    /* [in] */ InterfaceID* pIID)
{
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode NativeDaemonConnector::GetWeakReference(
    /* [out] */ IWeakReference** weakReference)
{
    VALIDATE_NOT_NULL(weakReference)
    *weakReference = new WeakReferenceImpl(Probe(EIID_IInterface), CreateWeak(this));
    REFCOUNT_ADD(*weakReference)
    return NOERROR;
}

ECode NativeDaemonConnector::Run()
{
    AutoPtr<IHandlerThread> thread;
    CHandlerThread::New(TAG + String(".CallbackHandler"), (IHandlerThread**)&thread);
    thread->Start();
    AutoPtr<ILooper> looper;
    thread->GetLooper((ILooper**)&looper);
    CHandler::New(looper, THIS_PROBE(IHandlerCallback), FALSE, (IHandler**)&mCallbackHandler);

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

    Int32 code;
    msg->GetWhat(&code);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    ICharSequence* seq = ICharSequence::Probe(obj);
    assert(seq != NULL);
    String event;
    seq->ToString(&event);

//    try {
    Boolean bval;
    ECode ec = mCallbacks->OnEvent(code, event, *NativeDaemonEvent::UnescapeArgs(event), &bval);
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
    return NOERROR;
}

ECode NativeDaemonConnector::ListenToSocket()
{
    AutoPtr<ILocalSocket> socket;

//    try {
    FAIL_RETURN(CLocalSocket::New((ILocalSocket**)&socket));
    AutoPtr<ILocalSocketAddress> socketAddress;
    CLocalSocketAddress::New(mSocket, LocalSocketAddressNamespace_RESERVED,
        (ILocalSocketAddress**)&socketAddress);

    ECode ec = socket->Connect(socketAddress);
    if (FAILED(ec)) {
        socket->Close();
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
    Boolean result;

    while (TRUE) {
        Int32 count;
        ec = inputStream->ReadBytes(buffer, start, BUFFER_SIZE - start, &count);
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

//                try {
                AutoPtr<NativeDaemonEvent> event;
                ec = NativeDaemonEvent::ParseRawEvent(rawEvent, (NativeDaemonEvent**)&event);
                if (FAILED(ec)) {
                    goto _EXIT_;
                }
                if (event->IsClassUnsolicited()) {
                    // TODO: migrate to sending NativeDaemonEvent instances
                    AutoPtr<ICharSequence> seq;
                    CString::New(event->GetRawEvent(), (ICharSequence**)&seq);
                    AutoPtr<IMessage> msg;
                    helper->Obtain(mCallbackHandler, event->GetCode(), seq, (IMessage**)&msg);
                    mCallbackHandler->SendMessage(msg, &result);
                }
                else {
                    mResponseQueue->Add(event->GetCmdNumber(), event);
                }
//                } catch (IllegalArgumentException e) {
//                    log("Problem parsing message: " + rawEvent + " - " + e);
//                }

                start = i + 1;
            }
        }

        if (start == 0) {
            if (LOGD) {
//                const String rawEvent = new String(buffer, start, count, Charsets.UTF_8);
                String rawEvent((char*)buffer->GetPayload() + start, count);
                Slogger::D(TAG, "RCV incomplete <- {%s}", rawEvent.string());
            }
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
        socket->Close();
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

/**
* Make command for daemon, escaping arguments as needed.
*/
ECode NativeDaemonConnector::MakeCommand(
    /* [in] */ StringBuilder& builder,
    /* [in] */ const String& cmd,
    /* [in] */ ArrayOf<IInterface*>* args)
{
    // TODO: eventually enforce that cmd doesn't contain arguments
    if (cmd.IndexOf(Char32('\0')) >= 0) {
        Slogger::E(TAG, "MakeCommand: unexpected command: %s", cmd.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    builder.AppendString(cmd);
    if (args != NULL) {
        for (Int32 i = 0; i < args->GetLength(); i++) {
            AutoPtr<IInterface> arg = (*args)[i];
            String argString;// final String argString = String.valueOf(arg);
            if (arg == NULL) {
                argString = "NULL";
            }
            else if (ICharSequence::Probe(arg) != NULL) {
                AutoPtr<ICharSequence> valueOfarg = ICharSequence::Probe(arg);
                valueOfarg->ToString(&argString);
            }
            else if (IInteger32::Probe(arg) != NULL) {
                AutoPtr<IInteger32> valueOfarg = IInteger32::Probe(arg);
                Int32 num;
                valueOfarg->GetValue(&num);
                argString = StringUtils::ToString(num);
            }
            else if (IInteger64::Probe(arg) != NULL) {
                AutoPtr<IInteger64> valueOfarg = IInteger64::Probe(arg);
                Int64 num;
                valueOfarg->GetValue(&num);
                argString = StringUtils::ToString(num);
            }
            else {
                Slogger::E(TAG, "MakeCommand: unexpected argument type!");
                assert(0);
            }

            if (argString.IndexOf(Char32('\0')) >= 0) {
                Slogger::E(TAG, "MakeCommand: unexpected argument %s!", argString.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            builder.AppendChar(' ');
            AppendEscaped(builder, argString);
        }
    }
    return NOERROR;
}

/**
* Issue the given command to the native daemon and return a single expected
* response.
*
* @throws NativeDaemonConnectorException when problem communicating with
*             native daemon, or if the response matches
*             {@link NativeDaemonEvent#isClassClientError()} or
*             {@link NativeDaemonEvent#isClassServerError()}.
*/
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

/**
* Issue the given command to the native daemon and return a single expected
* response.
*
* @throws NativeDaemonConnectorException when problem communicating with
*             native daemon, or if the response matches
*             {@link NativeDaemonEvent#isClassClientError()} or
*             {@link NativeDaemonEvent#isClassServerError()}.
*/
ECode NativeDaemonConnector::Execute(
    /* [in] */ const String& cmd,
    /* [in] */ const String& arg,
    /* [out] */ NativeDaemonEvent** event)
{
    VALIDATE_NOT_NULL(event);
    *event = NULL;

    AutoPtr<ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
    AutoPtr<ICharSequence> seq;
    CString::New(arg, (ICharSequence**)&seq);
    args->Set(0, seq->Probe(EIID_IInterface));
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

/**
* Issue the given command to the native daemon and return any
* {@link NativeDaemonEvent#isClassContinue()} responses, including the
* final terminal response.
*
* @throws NativeDaemonConnectorException when problem communicating with
*             native daemon, or if the response matches
*             {@link NativeDaemonEvent#isClassClientError()} or
*             {@link NativeDaemonEvent#isClassServerError()}.
*/
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

/**
* Issue the given command to the native daemon and return any
* {@link NativeDaemonEvent#isClassContinue()} responses, including the
* final terminal response.
*
* @throws NativeDaemonConnectorException when problem communicating with
*             native daemon, or if the response matches
*             {@link NativeDaemonEvent#isClassClientError()} or
*             {@link NativeDaemonEvent#isClassServerError()}.
*/
ECode NativeDaemonConnector::ExecuteForList(
    /* [in] */ const String& cmd,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out, callee] */ ArrayOf<NativeDaemonEvent*>** events)
{
    return Execute(DEFAULT_TIMEOUT, cmd, args, events);
}

/**
* Issue the given command to the native daemon and return any
* {@linke NativeDaemonEvent@isClassContinue()} responses, including the
* final terminal response.  Note that the timeout does not count time in
* deep sleep.
*
* @throws NativeDaemonConnectorException when problem communicating with
*             native daemon, or if the response matches
*             {@link NativeDaemonEvent#isClassClientError()} or
*             {@link NativeDaemonEvent#isClassServerError()}.
*/
ECode NativeDaemonConnector::Execute(
    /* [in] */ Int32 timeout,
    /* [in] */ const String& cmd,
    /* [in] */ ArrayOf<IInterface*>* args,
    /* [out, callee] */ ArrayOf<NativeDaemonEvent*>** eventsArray)
{
    VALIDATE_NOT_NULL(eventsArray);
    *eventsArray = NULL;

    List< AutoPtr<NativeDaemonEvent> > events;

    Int32 sequenceNumber;
    mSequenceNumber->IncrementAndGet(&sequenceNumber);
    StringBuilder cmdBuilder(StringUtils::Int32ToString(sequenceNumber));
    cmdBuilder.AppendChar(' ');
    const Int64 startTime = SystemClock::GetElapsedRealtime();

    MakeCommand(cmdBuilder, cmd, args);

    String logCmd = cmdBuilder.ToString(); /* includes cmdNum, cmd, args */
    if (LOGD) Slogger::D(TAG, "Execute: SND -> [%s]", logCmd.string());

    cmdBuilder.AppendChar('\0');
    String sentCmd = cmdBuilder.ToString(); /* logCmd + \0 */

    {
        AutoLock lock(mDaemonLock);

        if (mOutputStream == NULL) {
            *eventsArray = ArrayOf<NativeDaemonEvent*>::Alloc(0);
            REFCOUNT_ADD(*eventsArray);
            Slogger::E(TAG, "missing output stream");
            return E_NATIVE_DAEMON_CONNECTOR_EXCEPTION;
        }
        else {
            ECode ec = mOutputStream->WriteBytes(ArrayOf<Byte>((Byte*)(sentCmd.string()), sentCmd.GetByteLength() + 1));
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
        event = mResponseQueue->Remove(sequenceNumber, timeout, sentCmd);
        if (event == NULL) {
            *eventsArray = ArrayOf<NativeDaemonEvent*>::Alloc(0);
            REFCOUNT_ADD(*eventsArray);
            Slogger::E(TAG, "timed-out waiting for response to %s.", logCmd.string());
            return E_NATIVE_DAEMON_CONNECTOR_EXCEPTION;
        }
//        log("RMV <- {" + event + "}");
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

/**
* Issue a command to the native daemon and return the raw responses.
*
* @deprecated callers should move to {@link #execute(String, Object...)}
*             which returns parsed {@link NativeDaemonEvent}.
*/
ECode NativeDaemonConnector::DoCommand(
    /* [in] */ const String& cmd,
    /* [out] */ List<String>& responses)
{
    AutoPtr< ArrayOf<NativeDaemonEvent*> > events;
    FAIL_RETURN(ExecuteForList(cmd, NULL, (ArrayOf<NativeDaemonEvent*>**)&events));
    for (Int32 i = 0; i < events->GetLength(); ++i) {
        responses.PushBack((*events)[i]->GetRawEvent());
    }
    return NOERROR;
}

/**
* Issues a list command and returns the cooked list of all
* {@link NativeDaemonEvent#getMessage()} which match requested code.
*/
ECode NativeDaemonConnector::DoListCommand(
    /* [in] */ const String& cmd,
    /* [in] */ Int32 expectedCode,
    /* [out, callee] */ ArrayOf<String>** responses)
{
    VALIDATE_NOT_NULL(responses);
    *responses = NULL;

    List<String> list;

    AutoPtr< ArrayOf<NativeDaemonEvent*> > events;
    FAIL_RETURN(ExecuteForList(cmd, NULL, (ArrayOf<NativeDaemonEvent*>**)&events));
    for (Int32 i = 0; i < events->GetLength(); ++i) {
        AutoPtr<NativeDaemonEvent> event = (*events)[i];
        Int32 code = event->GetCode();
        if (code == expectedCode) {
            list.PushBack(event->GetMessage());
        }
        else {
            Slogger::E(TAG, "unexpected list response %d instead of %d.", code, expectedCode);
            return E_NATIVE_DAEMON_CONNECTOR_EXCEPTION;
//            throw new NativeDaemonConnectorException(
//                    "unexpected list response " + code + " instead of " + expectedCode);
        }
    }

    AutoPtr<NativeDaemonEvent> finalEvent = (*events)[events->GetLength() - 1];
    if (!finalEvent->IsClassOk()) {
        Slogger::E(TAG, "unexpected final event.");
//        throw new NativeDaemonConnectorException("unexpected final event: " + finalEvent);
        return E_NATIVE_DAEMON_CONNECTOR_EXCEPTION;
    }

    *responses = ArrayOf<String>::Alloc(list.GetSize());
    REFCOUNT_ADD(*responses);
    List<String>::Iterator it;
    Int32 i;
    for (it = list.Begin(), i = 0; it != list.End(); ++it, ++i) {
        (**responses)[i] = *it;
    }
    return NOERROR;
}

/**
* Append the given argument to {@link StringBuilder}, escaping as needed,
* and surrounding with quotes when it contains spaces.
*/
// @VisibleForTesting
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
            builder.AppendCStr("\\\"");
        }
        else if (c == '\\') {
            builder.AppendCStr("\\\\");
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
