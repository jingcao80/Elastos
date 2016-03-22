
#include <Elastos.CoreLibrary.IO.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/CProcessStartResult.h"
#include "elastos/droid/os/CUserHandleHelper.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/CSystemProperties.h"
#include "elastos/droid/net/CLocalSocket.h"
#include "elastos/droid/net/CLocalSocketAddress.h"
#include <elastos/droid/DroidRuntime.h>

#include <elastos/droid/system/Os.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/etl/Vector.h>
#include <elastos/utility/etl/Algorithm.h>

#include <binder/ProcessState.h>
#include <cutils/process_name.h>
#include <cutils/sched_policy.h>
#include <processgroup/processgroup.h>

#include <dirent.h>
#include <fcntl.h>
#include <grp.h>
#include <inttypes.h>
#include <pwd.h>
#include <signal.h>
#include <sys/errno.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::Thread;
using Elastos::IO::IWriter;
using Elastos::IO::ICloseable;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IOutputStreamWriter;
using Elastos::IO::COutputStreamWriter;
using Elastos::IO::CBufferedWriter;
using Elastos::IO::IFlushable;
using Elastos::IO::IDataInput;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;
using Elastos::Utility::Etl::Vector;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::DroidRuntime;
using Elastos::Droid::Net::CLocalSocket;
using Elastos::Droid::Net::ILocalSocketAddress;
using Elastos::Droid::Net::CLocalSocketAddress;
using Elastos::Droid::Net::LocalSocketAddressNamespace_RESERVED;

namespace Elastos {
namespace Droid {
namespace Os {

const String Process::TAG("Process");
const String Process::ZYGOTE_SOCKET_ELASTOS("elzygote");
const String Process::SECONDARY_ZYGOTE_SOCKET_ELASTOS("elzygote_secondary");
const String Process::ZYGOTE_SOCKET_JAVA("zygote");
const String Process::SECONDARY_ZYGOTE_SOCKET_JAVA("zygote_secondary");

const Int32 Process::ROOT_UID = 0;

AutoPtr<Process::ZygoteState> Process::mPrimaryZygoteState;
AutoPtr<Process::ZygoteState> Process::mSecondaryZygoteState;

const Int32 Process::ZYGOTE_RETRY_MILLIS = 500;

const Int32 Process::PROC_TERM_MASK = 0xff;
const Int32 Process::PROC_ZERO_TERM = 0;
const Int32 Process::PROC_SPACE_TERM = (Int32)' ';
const Int32 Process::PROC_TAB_TERM = (Int32)'\t';
const Int32 Process::PROC_COMBINE = 0x100;
const Int32 Process::PROC_PARENS = 0x200;
const Int32 Process::PROC_QUOTES = 0x400;
const Int32 Process::PROC_OUT_STRING = 0x1000;
const Int32 Process::PROC_OUT_LONG = 0x2000;
const Int32 Process::PROC_OUT_FLOAT = 0x4000;

Object Process::sLock;

//===============================================================================
// Process::ZygoteState
//===============================================================================

Process::ZygoteState::ZygoteState(
    /* [in] */ ILocalSocket* socket,
    /* [in] */ IDataInputStream* inputStream,
    /* [in] */ IBufferedWriter* writer,
    /* [in] */ List<String>* abiList)
    : mSocket(socket)
    , mInputStream(inputStream)
    , mWriter(writer)
    , mAbiList(abiList)
    , mClosed(FALSE)
{}

ECode Process::ZygoteState::Connect(
    /* [in] */ const String& socketAddress,
    /* [out] */ Process::ZygoteState** state)
{
    VALIDATE_NOT_NULL(state)
    *state = NULL;

    Logger::D(TAG, "Process::ZygoteState::Connect: %s", socketAddress.string());
    AutoPtr<IDataInputStream> zygoteInputStream;
    AutoPtr<IBufferedWriter> zygoteWriter;
    AutoPtr<ILocalSocket> zygoteSocket;
    CLocalSocket::New((ILocalSocket**)&zygoteSocket);

    AutoPtr<IInputStream> inputStream;
    AutoPtr<IOutputStream> outputStream;
    AutoPtr<IOutputStreamWriter> outputStreamWriter;
    AutoPtr<ArrayOf<String> > splits;
    AutoPtr<List<String> > lists;
    String abiListString;

    ECode ec = NOERROR;
    AutoPtr<ILocalSocketAddress> lsa;
    ec = CLocalSocketAddress::New(socketAddress, LocalSocketAddressNamespace_RESERVED, (ILocalSocketAddress**)&lsa);
    FAIL_GOTO(ec, _EXIT_)

    ec = zygoteSocket->Connect(lsa);
    FAIL_GOTO(ec, _EXIT_)

    ec = zygoteSocket->GetInputStream((IInputStream**)&inputStream);
    FAIL_GOTO(ec, _EXIT_)

    ec = CDataInputStream::New(inputStream, (IDataInputStream**)&zygoteInputStream);
    FAIL_GOTO(ec, _EXIT_)

    ec = zygoteSocket->GetOutputStream((IOutputStream**)&outputStream);
    FAIL_GOTO(ec, _EXIT_)

    ec = COutputStreamWriter::New(outputStream, (IWriter**)&outputStreamWriter);
    FAIL_GOTO(ec, _EXIT_)

    ec = CBufferedWriter::New(IWriter::Probe(outputStreamWriter), 256, (IBufferedWriter**)&zygoteWriter);
    FAIL_GOTO(ec, _EXIT_)

    ec =  Process::GetAbiList(zygoteWriter, zygoteInputStream, &abiListString);
    FAIL_GOTO(ec, _EXIT_)

    Logger::I(TAG, "Process: zygote socket opened, supported ABIS: [%s]", abiListString.string());

    StringUtils::Split(abiListString, String(","), (ArrayOf<String>**)&splits);
    if (splits != NULL) {
        lists = new List<String>(splits->GetLength());
        for (Int32 i = 0; i < splits->GetLength(); ++i) {
            lists->PushBack((*splits)[i]);
        }
    }

    *state = new ZygoteState(zygoteSocket, zygoteInputStream, zygoteWriter, lists);
    REFCOUNT_ADD(*state)
    return NOERROR;

_EXIT_:
    if (zygoteSocket) {
        ICloseable::Probe(zygoteSocket)->Close();
    }
    Logger::E(TAG, "Error: failed to Connect %s", socketAddress.string());
    return ec;
}

Boolean Process::ZygoteState::Matches(
    /* [in] */ const String& abi)
{
    List<String>::Iterator it = Find(mAbiList->Begin(), mAbiList->End(), abi);
    return it != mAbiList->End();
}

void Process::ZygoteState::Close()
{
    ECode ec = ICloseable::Probe(mSocket)->Close();
    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::E("Process", "I/O exception on routine close E_IO_EXCEPTION");
    }

    mClosed = TRUE;
}

Boolean Process::ZygoteState::IsClosed()
{
    return mClosed;
}

//===============================================================================
// Process
//===============================================================================
ECode Process::Start(
    /* [in] */ const String& processClass,
    /* [in] */ const String& niceName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid,
    /* [in] */ ArrayOf<Int32>* gids,
    /* [in] */ Int32 debugFlags,
    /* [in] */ Int32 mountExternal,
    /* [in] */ Int32 targetSdkVersion,
    /* [in] */ const String& seInfo,
    /* [in] */ const String& abi,
    /* [in] */ const String& instructionSet,
    /* [in] */ const String& appDataDir,
    /* [in] */ Boolean refreshTheme,
    /* [in] */ ArrayOf<String>* zygoteArgs,
    /* [out] */ IProcessStartResult** result)
{
    VALIDATE_NOT_NULL(result);

    // try {
    ECode ec = StartViaZygote(processClass, niceName, uid, gid, gids,
            debugFlags, mountExternal, targetSdkVersion, seInfo,
            abi, instructionSet, appDataDir, refreshTheme, zygoteArgs, result);
    if (ec == (ECode)E_ZYGOTE_START_FAILED_EXCEPTION) {
        Logger::E(TAG, "Starting process through Zygote failed");
        return E_RUNTIME_EXCEPTION;
    }
    return ec;
}

ECode Process::GetAbiList(
    /* [in] */ IBufferedWriter* writer,
    /* [in] */ IDataInputStream* inputStream,
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    ECode ec = NOERROR;
    IDataInput* di = IDataInput::Probe(inputStream);
    Int32 numBytes;
    AutoPtr<ArrayOf<Byte> > bytes;
Logger::I(TAG, " >> 1");
    ec = IWriter::Probe(writer)->Write(String("1"));
    // Each query starts with the argument count (1 in this case)
    FAIL_GOTO(ec, _EXIT_)

    // ... followed by a new-line.
    ec = writer->NewLine();
    FAIL_GOTO(ec, _EXIT_)
Logger::I(TAG, " >> 3");
    // ... followed by our only argument.
    ec = IWriter::Probe(writer)->Write(String("--query-abi-list"));
    FAIL_GOTO(ec, _EXIT_)
Logger::I(TAG, " >> 4");
    ec = writer->NewLine();
    FAIL_GOTO(ec, _EXIT_)
Logger::I(TAG, " >> 5");
    ec = IFlushable::Probe(writer)->Flush();
    FAIL_GOTO(ec, _EXIT_)
Logger::I(TAG, " >> 6");

    // The response is a length prefixed stream of ASCII bytes.
    ec = di->ReadInt32(&numBytes);
    FAIL_GOTO(ec, _EXIT_)
Logger::I(TAG, " >> 7");
    bytes = ArrayOf<Byte>::Alloc(numBytes);
Logger::I(TAG, " >> 8");
    ec = di->ReadFully(bytes);
    FAIL_GOTO(ec, _EXIT_)
Logger::I(TAG, " >> 9");
    // Logger::I(TAG, " GetAbiList: %d, %s", numBytes, (const char*)bytes->GetPayload());
    *result = String((const char*)bytes->GetPayload());
    return NOERROR;

_EXIT_:
    Logger::E(TAG, "failed to GetAbiList");
    return ec;
}

ECode Process::ElastosZygoteSendArgsAndGetResult(
    /* [in] */ Process::ZygoteState* zygoteState,
    /* [in] */ List<String>* args,
    /* [out] */ IProcessStartResult** _result)
{
    VALIDATE_NOT_NULL(_result);
    *_result = NULL;

    // try {
    /**
     * See com.andrgoid.internal.os.ZygoteInit.readArgumentList()
     * Presently the wire format to the zygote process is:
     * a) a count of arguments (argc, in essence)
     * b) a number of newline-separated argument strings equal to count
     *
     * After the zygote process reads these it will write the pid of
     * the child or -1 on failure, followed by boolean to
     * indicate whether a wrapper process was used.
     */

    AutoPtr<IBufferedWriter> writer = zygoteState->mWriter;
    AutoPtr<IDataInputStream> inputStream = zygoteState->mInputStream;

    IWriter::Probe(writer)->Write(StringUtils::ToString((Int32)args->GetSize()));
    writer->NewLine();

    List<String>::Iterator it;
    for (it = args->Begin(); it != args->End(); ++it) {
        String& arg = *it;
        if (arg.IndexOf('\n') >= 0) {
            Logger::E(TAG, "ZygoteStartFailed: embedded newlines not allowed");
            return E_ZYGOTE_START_FAILED_EXCEPTION;
        }
        IWriter::Probe(writer)->Write(arg);
        writer->NewLine();
    }

    IFlushable::Probe(writer)->Flush();

    Int32 pid;
    Boolean usingWrapper;

    // Should there be a timeout on this?
    AutoPtr<IProcessStartResult> result;
    CProcessStartResult::New((IProcessStartResult**)&result);
    ECode ec = IDataInput::Probe(inputStream)->ReadInt32(&pid);
    FAIL_GOTO(ec, _EXIT_)

    result->SetPid(pid);
    if (pid < 0) {
        Logger::E(TAG, "ZygoteStartFailed: fork() failed");
        return E_ZYGOTE_START_FAILED_EXCEPTION;
    }

    ec = IDataInput::Probe(inputStream)->ReadBoolean(&usingWrapper);
    FAIL_GOTO(ec, _EXIT_)

    result->SetUsingWrapper(usingWrapper);
    *_result = result;
    REFCOUNT_ADD(*_result);
    return NOERROR;

_EXIT_:
    zygoteState->Close();
    Logger::E(TAG, "failed to start process");
    return E_ZYGOTE_START_FAILED_EXCEPTION;
    // } catch (IOException ex) {
            // zygoteState.close();
            // throw new ZygoteStartFailedEx(ex);
    // }
}

ECode Process::JavaZygoteSendArgsAndGetResult(
    /* [in] */ ZygoteState* zygoteState,
    /* [in] */ List<String>* args,
    /* [out] */ IProcessStartResult** _result)
{
    VALIDATE_NOT_NULL(_result);
    *_result = NULL;

    // try {
    /**
     * See com.andrgoid.internal.os.ZygoteInit.readArgumentList()
     * Presently the wire format to the zygote process is:
     * a) a count of arguments (argc, in essence)
     * b) a number of newline-separated argument strings equal to count
     *
     * After the zygote process reads these it will write the pid of
     * the child or -1 on failure, followed by boolean to
     * indicate whether a wrapper process was used.
     */

    AutoPtr<IBufferedWriter> writer = zygoteState->mWriter;
    AutoPtr<IDataInputStream> inputStream = zygoteState->mInputStream;

    IWriter::Probe(writer)->Write(StringUtils::ToString((Int32)args->GetSize()));
    writer->NewLine();

    List<String>::Iterator it;
    for (it = args->Begin(); it != args->End(); ++it) {
        String& arg = *it;
        if (arg.IndexOf('\n') >= 0) {
            Logger::E(TAG, "ZygoteStartFailed: embedded newlines not allowed");
            return E_ZYGOTE_START_FAILED_EXCEPTION;
        }
        IWriter::Probe(writer)->Write(arg);
        writer->NewLine();
    }

    IFlushable::Probe(writer)->Flush();

    Int32 pid;
    Boolean usingWrapper;

    // Should there be a timeout on this?
    AutoPtr<IProcessStartResult> result;
    CProcessStartResult::New((IProcessStartResult**)&result);

    ECode ec = IDataInput::Probe(inputStream)->ReadInt32(&pid);
    FAIL_GOTO(ec, _EXIT_)

    result->SetPid(pid);
    if (pid < 0) {
        Logger::E(TAG, "ZygoteStartFailed: fork() failed");
        return E_ZYGOTE_START_FAILED_EXCEPTION;
    }

    ec = IDataInput::Probe(inputStream)->ReadBoolean(&usingWrapper);
    FAIL_GOTO(ec, _EXIT_)

    result->SetUsingWrapper(usingWrapper);
    *_result = result;
    REFCOUNT_ADD(*_result);
    return NOERROR;

_EXIT_:
    zygoteState->Close();
    Logger::E(TAG, "failed to start process");
    return E_ZYGOTE_START_FAILED_EXCEPTION;
    // } catch (IOException ex) {
            // zygoteState.close();
            // throw new ZygoteStartFailedEx(ex);
    // }
}

ECode Process::StartViaZygote(
    /* [in] */ const String& processClass,
    /* [in] */ const String& niceName,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid,
    /* [in] */ ArrayOf<Int32>* gids,
    /* [in] */ Int32 debugFlags,
    /* [in] */ Int32 mountExternal,
    /* [in] */ Int32 targetSdkVersion,
    /* [in] */ const String& seInfo,
    /* [in] */ const String& abi,
    /* [in] */ const String& instructionSet,
    /* [in] */ const String& appDataDir,
    /* [in] */ Boolean refreshTheme,
    /* [in] */ ArrayOf<String>* extraArgs,
    /* [out] */ IProcessStartResult** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;

    Logger::I(TAG, "Process::StartViaZygote isJava:%d, processClass:%s, niceName:%s, uid:%d, gid:%d",
        processClass.StartWith("android."), processClass.string(), niceName.string(), uid, gid);

    {
        AutoLock lock(sLock);

        Boolean startJavaProcess = processClass.StartWith("android.");

        AutoPtr<List<String> > argsForZygote = new List<String>();

        // --runtime-init, --setuid=, --setgid=,
        // and --setgroups= must go first
        argsForZygote->PushBack(String("--runtime-init"));
        argsForZygote->PushBack(String("--setuid=") + StringUtils::ToString(uid));
        argsForZygote->PushBack(String("--setgid=") + StringUtils::ToString(gid));
        // if ((debugFlags & Zygote.DEBUG_ENABLE_JNI_LOGGING) != 0) {
        //     argsForZygote->add("--enable-jni-logging");
        // }
        // if ((debugFlags & Zygote.DEBUG_ENABLE_SAFEMODE) != 0) {
        //     argsForZygote->add("--enable-safemode");
        // }
        // if ((debugFlags & Zygote.DEBUG_ENABLE_DEBUGGER) != 0) {
        //     argsForZygote->add("--enable-debugger");
        // }
        // if ((debugFlags & Zygote.DEBUG_ENABLE_CHECKJNI) != 0) {
        //     argsForZygote->add("--enable-checkjni");
        // }
        // if ((debugFlags & Zygote.DEBUG_ENABLE_ASSERT) != 0) {
        //     argsForZygote->add("--enable-assert");
        // }
        // if (mountExternal == Zygote.MOUNT_EXTERNAL_MULTIUSER) {
        //     argsForZygote->add("--mount-external-multiuser");
        // }
        // else if (mountExternal == Zygote.MOUNT_EXTERNAL_MULTIUSER_ALL) {
        //     argsForZygote->add("--mount-external-multiuser-all");
        // }
        if (refreshTheme) {
            argsForZygote->PushBack(String("--refresh_theme"));
        }
        argsForZygote->PushBack(String("--target-sdk-version=") + StringUtils::ToString(targetSdkVersion));

        //TODO optionally enable debuger
        //argsForZygote->add("--enable-debugger");

        // --setgroups is a comma-separated list
        if (gids != NULL && gids->GetLength() > 0) {
            StringBuilder sb;
            sb.Append("--setgroups=");

            Int32 sz = gids->GetLength();
            for (Int32 i = 0; i < sz; i++) {
                if (i != 0) {
                    sb.AppendChar(',');
                }
                sb.Append((*gids)[i]);
            }
            Logger::I("permissionRelated", "processClass:%s niceName:%s groups:%s",
                processClass.string(), niceName.string(), sb.ToString().string());
            argsForZygote->PushBack(sb.ToString());
        }

        if (!niceName.IsNull()) {
            argsForZygote->PushBack(String("--nice-name=") + niceName);
        }

        if (!seInfo.IsNull()) {
            argsForZygote->PushBack(String("--seinfo=") + seInfo);
        }

        if (!instructionSet.IsNull()) {
            argsForZygote->PushBack(String("--instruction-set=") + instructionSet);
        }

        if (!appDataDir.IsNull()) {
            argsForZygote->PushBack(String("--app-data-dir=") + appDataDir);
        }

        //TODO:
        if(!startJavaProcess){
            argsForZygote->PushBack(String("Elastos.Droid.Core.eco"));
        }

        argsForZygote->PushBack(processClass);

        if (extraArgs != NULL) {
            Int32 sz = extraArgs->GetLength();
            for (Int32 i = 0; i < sz; i++) {
                argsForZygote->PushBack((*extraArgs)[i]);
            }
        }

        if (startJavaProcess){
            AutoPtr<ZygoteState> zygoteState;
            FAIL_RETURN(OpenJavaZygoteSocketIfNeeded(abi, (ZygoteState**)&zygoteState))
            return JavaZygoteSendArgsAndGetResult(zygoteState, argsForZygote, result);
        }
        else {
            AutoPtr<ZygoteState> zygoteState;
            FAIL_RETURN(OpenElastosZygoteSocketIfNeeded(abi, (ZygoteState**)&zygoteState))
            return ElastosZygoteSendArgsAndGetResult(zygoteState, argsForZygote, result);
        }
    }
}

Int64 Process::GetElapsedCpuTime()
{
    struct timespec ts;

    int res = clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);

    if (res != 0) {
        return (Int64) 0;
    }

    nsecs_t when = seconds_to_nanoseconds(ts.tv_sec) + ts.tv_nsec;
    return (Int64) nanoseconds_to_milliseconds(when);
}

Int32 Process::MyPid()
{
    Int32 id;
    Elastos::Droid::System::Os::Getpid(&id);
    return id;
}

Int32 Process::MyPpid()
{
    Int32 id;
    Elastos::Droid::System::Os::Getppid(&id);
    return id;
}

Int32 Process::MyTid()
{
    Int32 id;
    Elastos::Droid::System::Os::Gettid(&id);
    return id;
}

Int32 Process::MyUid()
{
    Int32 id;
    Elastos::Droid::System::Os::Getuid(&id);
    return id;
}

ECode Process::MyUserHandle(
    /* [out] */ IUserHandle** userHandle)
{
    VALIDATE_NOT_NULL(userHandle);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);

    Int32 userId = 0;
    helper->GetUserId(MyUid(), &userId);
    return CUserHandle::New(userId, userHandle);
}

Boolean Process::IsIsolated()
{
    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);

    Int32 uid = 0;
    helper->GetAppId(MyUid(), &uid);
    return uid >= IProcess::FIRST_ISOLATED_UID && uid <= IProcess::LAST_ISOLATED_UID;
}

Int32 Process::GetUidForName(
    /* [in] */ const String& name)
{
    Int32 N = name.GetLength();
    if (N > 0) {
        const char* str = name.string();
        for (Int32 i = 0; i < N; i++) {
            if (str[i] < '0' || str[i] > '9') {
                struct passwd* pwd = getpwnam(str);
                if (pwd == NULL) {
                    return -1;
                }
                return pwd->pw_uid;
            }
        }
        return atoi(str);
    }

    return -1;
}

Int32 Process::GetGidForName(
    /* [in] */ const String& name)
{
    if (name.IsNull()) {
        return -1;
    }

    const size_t N = name.GetLength();
    if (N > 0) {
        const char* str = name.string();
        for (size_t i=0; i<N; i++) {
            if (str[i] < '0' || str[i] > '9') {
                struct group* grp = getgrnam(str);
                if (grp == NULL) {
                    return -1;
                }
                return grp->gr_gid;
            }
        }
        return atoi(str);
    }
    return -1;
}

Int32 Process::GetUidForPid(
    /* [in] */ Int32 pid)
{
    AutoPtr< ArrayOf<String> > procStatusLabels = ArrayOf<String>::Alloc(1);
    (*procStatusLabels)[0] = "Uid:";
    AutoPtr< ArrayOf<Int64> > procStatusValues = ArrayOf<Int64>::Alloc(1);
    (*procStatusValues)[0] = -1;
    StringBuilder sb("/proc/");
    sb += StringUtils::ToString(pid);
    sb += "/status";
    Process::ReadProcLines(sb.ToString(), *procStatusLabels, procStatusValues);
    return (Int32)(*procStatusValues)[0];
}

Int32 Process::GetParentPid(
    /* [in] */ Int32 pid)
{
    AutoPtr< ArrayOf<String> > procStatusLabels = ArrayOf<String>::Alloc(1);
    (*procStatusLabels)[0] = "PPid:";
    AutoPtr< ArrayOf<Int64> > procStatusValues = ArrayOf<Int64>::Alloc(1);
    (*procStatusValues)[0] = -1;
    StringBuilder sb("/proc/");
    sb += StringUtils::ToString(pid);
    sb += "/status";
    Process::ReadProcLines(sb.ToString(), *procStatusLabels, procStatusValues);
    return (Int32)(*procStatusValues)[0];
}

Int32 Process::GetThreadGroupLeader(
    /* [in] */ Int32 pid)
{
    AutoPtr< ArrayOf<String> > procStatusLabels = ArrayOf<String>::Alloc(1);
    (*procStatusLabels)[0] = "Tgid:";
    AutoPtr< ArrayOf<Int64> > procStatusValues = ArrayOf<Int64>::Alloc(1);
    (*procStatusValues)[0] = -1;
    StringBuilder sb("/proc/");
    sb += StringUtils::ToString(pid);
    sb += "/status";
    Process::ReadProcLines(sb.ToString(), *procStatusLabels, procStatusValues);
    return (Int32)(*procStatusValues)[0];
}

// For both of these, err should be in the errno range (positive), not a status_t (negative)
static ECode SignalExceptionForPriorityError(int err)
{
    switch (err) {
        case EINVAL:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case ESRCH:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case EPERM:
            return E_SECURITY_EXCEPTION;
        case EACCES:
            return E_SECURITY_EXCEPTION;
        default:
            return E_RUNTIME_EXCEPTION;
    }
}

static ECode SignalExceptionForGroupError(int err)
{
    switch (err) {
        case EINVAL:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case ESRCH:
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        case EPERM:
            return E_SECURITY_EXCEPTION;
        case EACCES:
            return E_SECURITY_EXCEPTION;
        default:
            return E_RUNTIME_EXCEPTION;
    }
}

ECode Process::SetThreadPriority(
    /* [in] */ Int32 tid,
    /* [in] */ Int32 priority)
{
#if GUARD_THREAD_PRIORITY
    // if we're putting the current thread into the background, check the TLS
    // to make sure this thread isn't guarded.  If it is, raise an exception.
    if (priority >= ANDROID_PRIORITY_BACKGROUND) {
        if (tid == androidGetTid()) {
            void* bgOk = pthread_getspecific(gBgKey);
            if (bgOk == ((void*)0xbaad)) {
                Logger::E("Process", "Thread marked fg-only put self in background!");
                //jniThrowException(env, "java/lang/SecurityException", "May not put this thread into background");
                return E_JNI_THROW_EXCEPTION;
            }
        }
    }
#endif

    int rc = androidSetThreadPriority(tid, priority);
    if (rc != 0) {
        if (rc == android::INVALID_OPERATION) {
            return SignalExceptionForPriorityError(errno);
        }
        else {
            return SignalExceptionForGroupError(errno);
        }
    }

    //ALOGI("Setting priority of %d: %d, getpriority returns %d\n",
    //     tid, priority, getpriority(PRIO_PROCESS, tid));
    return NOERROR;
}

ECode Process::SetCanSelfBackground(
    /* [in] */ Boolean backgroundOk)
{
    // Establishes the calling thread as illegal to put into the background.
    // Typically used only for the system process's main looper.
#if GUARD_THREAD_PRIORITY
    Logger::V("Process", "Process.setCanSelfBackground(%d) : tid=%d", backgroundOk, androidGetTid());
    {
        AutoLock _l(gKeyCreateMutex);
        if (gBgKey == -1) {
            pthread_key_create(&gBgKey, NULL);
        }
    }

    // inverted:  not-okay, we set a sentinel value
    pthread_setspecific(gBgKey, (void*)(backgroundOk ? 0 : 0xbaad));
#endif
    return NOERROR;
}

ECode Process::SetThreadGroup(
    /* [in] */ Int32 tid,
    /* [in] */ Int32 group)
{
    SchedPolicy sp = (SchedPolicy) group;
    int res = set_sched_policy(tid, sp);
    if (res != android::NO_ERROR) {
        return SignalExceptionForGroupError(-res);
    }
    return NOERROR;
}

ECode Process::SetProcessGroup(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 group)
{
    DIR *d;
    char proc_path[255];
    struct dirent *de;

    if ((group == SP_FOREGROUND) || (group > SP_MAX)) {
        return SignalExceptionForGroupError(EINVAL);
    }

    bool isDefault = FALSE;
    if (group < 0) {
        group = SP_FOREGROUND;
        isDefault = TRUE;
    }
    SchedPolicy sp = (SchedPolicy) group;

#if POLICY_DEBUG
    char cmdline[32];
    int fd;

    strcpy(cmdline, "unknown");

    sprintf(proc_path, "/proc/%d/cmdline", pid);
    fd = open(proc_path, O_RDONLY);
    if (fd >= 0) {
        int rc = read(fd, cmdline, sizeof(cmdline)-1);
        cmdline[rc] = 0;
        close(fd);
    }

    if (sp == SP_BACKGROUND) {
        Logger::D("Process", "setProcessGroup: vvv pid %d (%s)", pid, cmdline);
    } else {
        Logger::D("Process", "setProcessGroup: ^^^ pid %d (%s)", pid, cmdline);
    }
#endif
    sprintf(proc_path, "/proc/%d/task", pid);
    if (!(d = opendir(proc_path))) {
        // If the process exited on us, don't generate an exception
        if (errno != ENOENT) {
            return SignalExceptionForGroupError(errno);
        }
        return NOERROR;
    }

    ECode ec = NOERROR;
    while ((de = readdir(d))) {
        int t_pid;
        int t_pri;

        if (de->d_name[0] == '.') {
            continue;
        }
        t_pid = atoi(de->d_name);

        if (!t_pid) {
            Logger::E("Process", "Error getting pid for '%s'\n", de->d_name);
            continue;
        }

        t_pri = getpriority(PRIO_PROCESS, t_pid);

        if (t_pri <= ANDROID_PRIORITY_AUDIO) {
            int scheduler = sched_getscheduler(t_pid);
            if ((scheduler == SCHED_FIFO) || (scheduler == SCHED_RR)) {
                // This task wants to stay in it's current audio group so it can keep it's budget
                continue;
            }
        }

        if (isDefault) {
            if (t_pri >= ANDROID_PRIORITY_BACKGROUND) {
                // This task wants to stay at background
                continue;
            }
        }

        int err = set_sched_policy(t_pid, sp);
        if (err != android::NO_ERROR) {
            ec = SignalExceptionForGroupError(-err);
            break;
        }
    }
    closedir(d);
    return ec;
}

ECode Process::GetProcessGroup(
    /* [in] */ Int32 pid,
    /* [out] */ Int32* group)
{
    VALIDATE_NOT_NULL(group)
    SchedPolicy sp;
    if (get_sched_policy(pid, &sp) != 0) {
        return SignalExceptionForGroupError(errno);
    }
    *group = (int) sp;
    return NOERROR;
}

ECode Process::SetThreadPriority(
    /* [in] */ Int32 priority)
{
    return SetThreadPriority(MyTid(), priority);
}

ECode Process::GetThreadPriority(
    /* [in] */ Int32 tid,
    /* [out] */ Int32* priority)
{
    VALIDATE_NOT_NULL(priority);
    ECode ec = NOERROR;
    errno = 0;
    Int32 pri = getpriority(PRIO_PROCESS, tid);
    if (errno != 0) {
        ec = SignalExceptionForPriorityError(errno);
    }
    //ALOGI("Returning priority of %d: %d\n", tid, pri);
    *priority = pri;
    return ec;
}

ECode Process::SetThreadScheduler(
    /* [in] */ Int32 tid,
    /* [in] */ Int32 policy,
    /* [in] */ Int32 priority)
{
#ifdef HAVE_SCHED_SETSCHEDULER
    struct sched_param param;
    param.sched_priority = priority;
    int rc = sched_setscheduler(tid, policy, &param);
    if (rc) {
        return SignalExceptionForPriorityError(errno);
    }
#else
    return SignalExceptionForPriorityError(ENOSYS);
#endif
}

Boolean Process::SupportsProcesses()
{
    return TRUE;
}

Boolean Process::SetSwappiness(
    /* [in] */ Int32 pid,
    /* [in] */ Boolean is_increased)
{
    char text[64];

    if (is_increased) {
        strcpy(text, "/sys/fs/cgroup/memory/sw/tasks");
    } else {
        strcpy(text, "/sys/fs/cgroup/memory/tasks");
    }

    struct stat st;
    if (stat(text, &st) || !S_ISREG(st.st_mode)) {
        return false;
    }

    int fd = open(text, O_WRONLY);
    if (fd >= 0) {
        sprintf(text, "%" PRId32, pid);
        write(fd, text, strlen(text));
        close(fd);
    }

    return TRUE;
}

ECode Process::SetArgV0(
    /* [in] */ const String& procName)
{
    if (procName.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (!procName.IsEmpty()) {
        set_process_name(procName);
        DroidRuntime::GetRuntime()->SetArgv0(procName);
    }
    return NOERROR;
}

ECode Process::KillProcess(
    /* [in] */ Int32 pid)
{
    return SendSignal(pid, IProcess::SIGNAL_KILL);
}

Int32 Process::SetUid(
    /* [in] */ Int32 uid)
{
    return setuid(uid) == 0 ? 0 : errno;
}

Int32 Process::SetGid(
    /* [in] */ Int32 uid)
{
    return setgid(uid) == 0 ? 0 : errno;
}

ECode Process::SendSignal(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 signal)
{
    if (pid > 0) {
        Logger::I("Process", "Sending signal. PID: %d SIG: %d", pid, signal);
        kill(pid, signal);
    }

    return NOERROR;
}

ECode Process::KillProcessQuiet(
    /* [in] */ Int32 pid)
{
    return SendSignalQuiet(pid, IProcess::SIGNAL_KILL);
}

ECode Process::SendSignalQuiet(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 signal)
{
    if (pid > 0) {
        kill(pid, signal);
    }

    return NOERROR;
}

static int Pid_compare(const void* v1, const void* v2)
{
    //ALOGI("Compare %d vs %d\n", *((const jint*)v1), *((const jint*)v2));
    return *((const Int32*)v1) - *((const Int32*)v2);
}

static Int64 GetFreeMemoryImpl(
    /* [in] */ const char* const sums[],
    /* [in] */ const int sumsLen[],
    /* [in] */ int num)
{
    int fd = open("/proc/meminfo", O_RDONLY);

    if (fd < 0) {
        Logger::W("Process","Unable to open /proc/meminfo");
        return -1LL;
    }

    char buffer[256];
    const int len = read(fd, buffer, sizeof(buffer)-1);
    close(fd);

    if (len < 0) {
        Logger::W("Process","Unable to read /proc/meminfo");
        return -1LL;
    }
    buffer[len] = 0;

    int numFound = 0;
    Int64 mem = 0;

    char* p = buffer;
    while (*p && numFound < num) {
        int i = 0;
        while (sums[i]) {
            if (strncmp(p, sums[i], sumsLen[i]) == 0) {
                p += sumsLen[i];
                while (*p == ' ') p++;
                char* num = p;
                while (*p >= '0' && *p <= '9') p++;
                if (*p != 0) {
                    *p = 0;
                    p++;
                    if (*p == 0) p--;
                }
                mem += atoll(num) * 1024;
                numFound++;
                break;
            }
            i++;
        }
        p++;
    }

    return numFound > 0 ? mem : -1LL;
}

Int64 Process::GetFreeMemory()
{
    static const char* const sums[] = { "MemFree:", "Cached:", NULL };
    static const int sumsLen[] = { (int)strlen("MemFree:"), (int)strlen("Cached:"), 0 };
    return GetFreeMemoryImpl(sums, sumsLen, 2);
}

Int64 Process::GetTotalMemory()
{
    static const char* const sums[] = { "MemTotal:", NULL };
    static const int sumsLen[] = { (int)strlen("MemTotal:"), 0 };
    return GetFreeMemoryImpl(sums, sumsLen, 1);
}

ECode Process::ReadProcLines(
    /* [in] */ const String& path,
    /* [in] */ const ArrayOf<String>& reqFields,
    /* [out] */ ArrayOf<Int64>* outFields)
{
    //ALOGI("getMemInfo: %p %p", reqFields, outFields);

    if (path.IsNull() || outFields == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }

    if (path.IsEmpty()) {
        return NOERROR;
    }

    Int32 count = reqFields.GetLength();
    if (count > outFields->GetLength()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    Vector<String> fields;
    int i;

    for (i = 0; i < count; i++) {
        String req = reqFields[i];
        if (!req.IsNull()) {
            //ALOGI("String at %d: %p = %s", i, obj, str8);
            fields.PushBack(req);
        }
        else {
            return E_NULL_POINTER_EXCEPTION;
        }
    }

    Int64* sizesArray = outFields->GetPayload();
    //ALOGI("Clearing %d sizes", count);
    for (i = 0; i < count; i++) {
        sizesArray[i] = 0;
    }

    int fd = open(path.string(), O_RDONLY);

    if (fd >= 0) {
        const size_t BUFFER_SIZE = 2048;
        char* buffer = (char*)malloc(BUFFER_SIZE);
        int len = read(fd, buffer, BUFFER_SIZE-1);
        close(fd);

        if (len < 0) {
            Logger::W("Unable to read %s", path.string());
            len = 0;
        }
        buffer[len] = 0;

        int foundCount = 0;

        char* p = buffer;
        while (*p && foundCount < count) {
            bool skipToEol = true;
            //ALOGI("Parsing at: %s", p);
            for (i=0; i<count; i++) {
                const String& field = fields[i];
                if (strncmp(p, field.string(), field.GetLength()) == 0) {
                    p += field.GetLength();
                    while (*p == ' ' || *p == '\t') p++;
                    char* num = p;
                    while (*p >= '0' && *p <= '9') p++;
                    skipToEol = *p != '\n';
                    if (*p != 0) {
                        *p = 0;
                        p++;
                    }
                    char* end;
                    sizesArray[i] = strtoll(num, &end, 10);
                    //ALOGI("Field %s = %d", field.string(), sizesArray[i]);
                    foundCount++;
                    break;
                }
            }
            if (skipToEol) {
                while (*p && *p != '\n') {
                    p++;
                }
                if (*p == '\n') {
                    p++;
                }
            }
        }

        free(buffer);
    }
    else {
        Logger::W("Unable to open %s", path.string());
    }

    //ALOGI("Done!");
    return NOERROR;
}

ECode Process::GetPids(
    /* [in] */ const String& path,
    /* [in] */ ArrayOf<Int32>* _lastArray,
    /* [out, callee] */ ArrayOf<Int32>** pids)
{
    VALIDATE_NOT_NULL(pids);
    if (path.IsNull()) {
        *pids = NULL;
        return E_NULL_POINTER_EXCEPTION;
    }

    DIR* dirp = opendir(path.string());

    if (dirp == NULL) {
        *pids = NULL;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > lastArray = _lastArray;
    Int32 curCount = 0;
    Int32* curData = NULL;
    if (lastArray != NULL) {
        curCount = lastArray->GetLength();
        curData = lastArray->GetPayload();
    }

    Int32 curPos = 0;

    struct dirent* entry;
    while ((entry = readdir(dirp)) != NULL) {
        const char* p = entry->d_name;
        while (*p) {
            if (*p < '0' || *p > '9') break;
            p++;
        }
        if (*p != 0) continue;

        char* end;
        int pid = strtol(entry->d_name, &end, 10);
        //ALOGI("File %s pid=%d\n", entry->d_name, pid);
        if (curPos >= curCount) {
            Int32 newCount = (curCount == 0) ? 10 : (curCount * 2);
            AutoPtr< ArrayOf<Int32> > newArray = ArrayOf<Int32>::Alloc(newCount);
            if (newArray == NULL) {
                closedir(dirp);
                *pids = NULL;
                return E_OUT_OF_MEMORY_ERROR;
            }
            Int32* newData = newArray->GetPayload();
            if (curData != NULL) {
                memcpy(newData, curData, sizeof(Int32)*curCount);
            }
            lastArray = newArray;
            curCount = newCount;
            curData = newData;
        }

        curData[curPos] = pid;
        curPos++;
    }

    closedir(dirp);

    if (curData != NULL && curPos > 0) {
        qsort(curData, curPos, sizeof(int), Pid_compare);
    }

    while (curPos < curCount) {
        curData[curPos] = -1;
        curPos++;
    }

    *pids = lastArray;
    REFCOUNT_ADD(*pids);
    return NOERROR;
}

static ECode ParseProcLineArray(
    /* [in] */ char* buffer,
    /* [in] */ Int32 startIndex,
    /* [in] */ Int32 endIndex,
    /* [in] */ const ArrayOf<Int32>& format,
    /* [out] */ ArrayOf<String>* outStrings,
    /* [out] */ ArrayOf<Int64>* outInt64s,
    /* [out] */ ArrayOf<Float>* outFloats,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    const Int32 NF = format.GetLength();
    const Int32 NS = outStrings ? outStrings->GetLength() : 0;
    const Int32 NL = outInt64s ? outInt64s->GetLength() : 0;
    const Int32 NR = outFloats ? outFloats->GetLength() : 0;

    Int32* formatData = format.GetPayload();
    Int64* int64sData = outInt64s ? outInt64s->GetPayload() : NULL;
    Float* floatsData = outFloats ? outFloats->GetPayload() : NULL;
    if (formatData == NULL || (NL > 0 && int64sData == NULL)
            || (NR > 0 && floatsData == NULL)) {
        *result = FALSE;
        return E_OUT_OF_MEMORY_ERROR;
    }

    Int32 i = startIndex;
    Int32 di = 0;

    Boolean res = TRUE;

    for (Int32 fi = 0; fi < NF; fi++) {
        const Int32 mode = formatData[fi];
        if ((mode & Process::PROC_PARENS) != 0) {
            i++;
        }
        const char term = (char)(mode & Process::PROC_TERM_MASK);
        const Int32 start = i;
        if (i >= endIndex) {
            res = FALSE;
            break;
        }

        Int32 end = -1;
        if ((mode & Process::PROC_PARENS) != 0) {
            while (buffer[i] != ')' && i < endIndex) {
                i++;
            }
            end = i;
            i++;
        }
        while (buffer[i] != term && i < endIndex) {
            i++;
        }
        if (end < 0) {
            end = i;
        }

        if (i < endIndex) {
            i++;
            if ((mode & Process::PROC_COMBINE) != 0) {
                while (buffer[i] == term && i < endIndex) {
                    i++;
                }
            }
        }

        //ALOGI("Field %d: %d-%d dest=%d mode=0x%x\n", i, start, end, di, mode);

        if ((mode & (Process::PROC_OUT_FLOAT | Process::PROC_OUT_LONG | Process::PROC_OUT_STRING)) != 0) {
            char c = buffer[end];
            buffer[end] = 0;
            if ((mode & Process::PROC_OUT_FLOAT) != 0 && di < NR) {
                char* end;
                floatsData[di] = strtof(buffer+start, &end);
            }
            if ((mode & Process::PROC_OUT_LONG) != 0 && di < NL) {
                char* end;
                int64sData[di] = strtoll(buffer + start, &end, 10);
            }
            if ((mode & Process::PROC_OUT_STRING) != 0 && di < NS) {
                String str(buffer + start);
                (*outStrings)[di] = str;
            }
            buffer[end] = c;
            di++;
        }
    }

    *result = res;
    return NOERROR;
}

ECode Process::ReadProcFile(
    /* [in] */ const String& file,
    /* [in] */ const ArrayOf<Int32>& format,
    /* [out] */ ArrayOf<String>* outStrings,
    /* [out] */ ArrayOf<Int64>* outInt64s,
    /* [out] */ ArrayOf<Float>* outFloats,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (file.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }

    int fd = open(file.string(), O_RDONLY);

    if (fd < 0) {
        //ALOGW("Unable to open process file: %s\n", file8);
        *result = FALSE;
        return NOERROR;
    }

    char buffer[256];
    const int len = read(fd, buffer, sizeof(buffer) - 1);
    close(fd);

    if (len < 0) {
        //ALOGW("Unable to open process file: %s fd=%d\n", file8, fd);
        *result = FALSE;
        return NOERROR;
    }
    buffer[len] = 0;

    return ParseProcLineArray(buffer, 0, len,
            format, outStrings, outInt64s, outFloats, result);
}

ECode Process::ParseProcLine(
    /* [in] */ const ArrayOf<Byte>* buffer,
    /* [in] */ Int32 startIndex,
    /* [in] */ Int32 endIndex,
    /* [in] */ const ArrayOf<Int32>& format,
    /* [out] */ ArrayOf<String>* outStrings,
    /* [out] */ ArrayOf<Int64>* outInt64s,
    /* [out] */ ArrayOf<Float>* outFloats,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    return ParseProcLineArray((char*)buffer->GetPayload(),
            startIndex, endIndex, format, outStrings, outInt64s, outFloats, result);
}

ECode Process::GetPidsForCommands(
    /* [in] */ const ArrayOf<String>& cmds,
    /* [out] */ ArrayOf<Int32>** pids)
{
    VALIDATE_NOT_NULL(pids);
    *pids = NULL;

    Vector<String> commands;

    Int32 count = cmds.GetLength();

    for (Int32 i = 0; i < count; i++) {
        String command = cmds[i];
        if (!command.IsNull()) {
            commands.PushBack(command);
        }
        else {
            return E_NULL_POINTER_EXCEPTION;
        }
    }

    Vector<Int32> _pids;

    DIR *proc = opendir("/proc");
    if (proc == NULL) {
        fprintf(stderr, "/proc: %s\n", strerror(errno));
        return NOERROR;
    }

    struct dirent *d;
    while ((d = readdir(proc))) {
        int pid = atoi(d->d_name);
        if (pid <= 0) continue;

        char path[PATH_MAX];
        char data[PATH_MAX];
        snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);

        int fd = open(path, O_RDONLY);
        if (fd < 0) {
            continue;
        }
        const int len = read(fd, data, sizeof(data) - 1);
        close(fd);

        if (len < 0) {
            continue;
        }
        data[len] = 0;

        for (int i = 0; i < len; i++) {
            if (data[i] == ' ') {
                data[i] = 0;
                break;
            }
        }

        for (size_t i = 0; i < commands.GetSize(); i++) {
            if (commands[i] == data) {
                _pids.PushBack(pid);
                break;
            }
        }
    }

    closedir(proc);

    AutoPtr<ArrayOf<Int32> > pidArray = ArrayOf<Int32>::Alloc(_pids.GetSize());
    if (pidArray == NULL) {
        return E_OUT_OF_MEMORY_ERROR;
    }

    Int32 N = _pids.GetSize();
    if (N > 0) {
        for (Int32 i = 0; i < N; i++) {
            (*pidArray)[i] = _pids[i];
        }
    }

    *pids = pidArray;
    REFCOUNT_ADD(*pids);
    return NOERROR;
}

/**
* Gets the total Pss value for a given process, in bytes.
*
* @param pid the process to the Pss for
* @return the total Pss value for the given process in bytes,
*  or -1 if the value cannot be determined
* @hide
*/
Int64 Process::GetPss(
    /* [in] */ Int32 pid)
{
    char filename[64];

    snprintf(filename, sizeof(filename), "/proc/%d/smaps", pid);

    FILE * file = fopen(filename, "r");
    if (!file) {
        return -1LL;
    }

    // Tally up all of the Pss from the various maps
    char line[256];
    Int64 pss = 0;
    while (fgets(line, sizeof(line), file)) {
        Int64 v;
        if (sscanf(line, "Pss: %lld kB", &v) == 1) {
            pss += v;
        }
    }

    fclose(file);

    // Return the Pss value in bytes, not kilobytes
    return pss * 1024;
}

Int32 Process::KillProcessGroup(
    /* [in] */ Int32 uid,
    /* [in] */ Int32 pid)
{
    return killProcessGroup(uid, pid, SIGKILL);
}

ECode Process::RemoveAllProcessGroups()
{
    removeAllProcessGroups();
    return NOERROR;
}

ECode Process::OpenElastosZygoteSocketIfNeeded(
    /* [in] */ const String& abi,
    /* [out] */ Process::ZygoteState** state)
{
    VALIDATE_NOT_NULL(state)
    *state = NULL;

    if (mPrimaryZygoteState == NULL || mPrimaryZygoteState->IsClosed()) {
        mPrimaryZygoteState = NULL;
        ECode ec = ZygoteState::Connect(ZYGOTE_SOCKET_ELASTOS, (ZygoteState**)&mPrimaryZygoteState);
        if (FAILED(ec)) {
            Logger::E(TAG, "Error connecting to primary zygote");
            return E_ZYGOTE_START_FAILED_EXCEPTION;
        }
    }

    if (mPrimaryZygoteState->Matches(abi)) {
        *state = mPrimaryZygoteState;
        REFCOUNT_ADD(*state)
        return NOERROR;
    }

    // The primary zygote didn't match. Try the secondary.
    if (mSecondaryZygoteState == NULL || mSecondaryZygoteState->IsClosed()) {
        mSecondaryZygoteState = NULL;
        ECode ec = ZygoteState::Connect(SECONDARY_ZYGOTE_SOCKET_ELASTOS, (ZygoteState**)&mSecondaryZygoteState);
        if (FAILED(ec)) {
            Logger::E(TAG, "Error connecting to secondary zygote");
            return E_ZYGOTE_START_FAILED_EXCEPTION;
        }
    }

    if (mSecondaryZygoteState->Matches(abi)) {
        *state = mSecondaryZygoteState;
        REFCOUNT_ADD(*state)
        return NOERROR;
    }

    // throw new ZygoteStartFailedEx("Unsupported zygote ABI: " + abi);
    Logger::E(TAG, "OpenElastosZygoteSocketIfNeeded::Unsupported zygote ABI: %s", abi.string());
    return E_ZYGOTE_START_FAILED_EXCEPTION;
}

ECode Process::OpenJavaZygoteSocketIfNeeded(
    /* [in] */ const String& abi,
    /* [out] */ Process::ZygoteState** state)
{
    VALIDATE_NOT_NULL(state)
    *state = NULL;

    if (mPrimaryZygoteState == NULL || mPrimaryZygoteState->IsClosed()) {
        // try {
        mPrimaryZygoteState = NULL;
        ECode ec = ZygoteState::Connect(ZYGOTE_SOCKET_JAVA, (ZygoteState**)&mPrimaryZygoteState);
        // } catch (IOException ioe) {
        if (ec == (ECode)E_IO_EXCEPTION) {
            // throw new ZygoteStartFailedEx("Error connecting to primary zygote", ioe);
            Logger::E(TAG, "Error connecting to primary zygote");
            return E_ZYGOTE_START_FAILED_EXCEPTION;
        }
    }

    if (mPrimaryZygoteState->Matches(abi)) {
        *state = mPrimaryZygoteState;
        REFCOUNT_ADD(*state)
        return NOERROR;
    }

    // The primary zygote didn't match. Try the secondary.
    if (mSecondaryZygoteState == NULL || mSecondaryZygoteState->IsClosed()) {
        // try {
        mSecondaryZygoteState = NULL;
        ECode ec = ZygoteState::Connect(SECONDARY_ZYGOTE_SOCKET_JAVA, (ZygoteState**)&mSecondaryZygoteState);
        if (ec == (ECode)E_IO_EXCEPTION) {
        // } catch (IOException ioe) {
        //     throw new ZygoteStartFailedEx("Error connecting to secondary zygote", ioe);
            Logger::E(TAG, "Error connecting to secondary zygote");
            return E_ZYGOTE_START_FAILED_EXCEPTION;
        }
    }

    if (mSecondaryZygoteState->Matches(abi)) {
        *state = mSecondaryZygoteState;
        REFCOUNT_ADD(*state)
        return NOERROR;
    }

    // throw new ZygoteStartFailedEx("Unsupported zygote ABI: " + abi);
    Logger::E(TAG, "OpenJavaZygoteSocketIfNeeded::Unsupported zygote ABI: %s", abi.string());
    return E_ZYGOTE_START_FAILED_EXCEPTION;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
