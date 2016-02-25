
#include "elastos/droid/ext/frameworkdef.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/R.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/net/CLocalServerSocket.h"
#include "elastos/droid/content/res/CResources.h"
#include <elastos/droid/system/Os.h>
#include <elastos/droid/system/OsConstants.h>
#include "elastos/droid/internal/os/Zygote.h"
#include "elastos/droid/internal/os/CZygoteInit.h"
#include "elastos/droid/internal/os/WrapperInit.h"
#include "elastos/droid/internal/os/RuntimeInit.h"
#include "elastos/droid/webkit/WebViewFactory.h"
#include "elastos/droid/opengl/CEGL14.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/Vector.h>
#include <elastos/utility/logging/Logger.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::System::Os;
using Elastos::Droid::System::OsConstants;
using Elastos::Droid::Net::CLocalServerSocket;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Content::Res::CResources;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Webkit::WebViewFactory;
using Elastos::Droid::Opengl::IEGL14;
using Elastos::Droid::Opengl::CEGL14;
using Elastos::Droid::Opengl::IEGLDisplay;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::Thread;
using Elastos::IO::CFileDescriptor;
using Elastos::IO::IFileDescriptor;
using Elastos::Utility::Etl::Vector;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

const String CZygoteInit::TAG("CZygoteInit");
const String CZygoteInit::PROPERTY_DISABLE_OPENGL_PRELOADING("ro.zygote.disable_gl_preload");
const String CZygoteInit::ANDROID_SOCKET_PREFIX("ANDROID_SOCKET_");
const Int32 CZygoteInit::LOG_BOOT_PROGRESS_PRELOAD_START = 3020;
const Int32 CZygoteInit::LOG_BOOT_PROGRESS_PRELOAD_END = 3030;
/** when preloading, GC after allocating this many bytes */
const Int32 CZygoteInit::PRELOAD_GC_THRESHOLD = 50000;
const String CZygoteInit::ABI_LIST_ARG("--abi-list=");
const String CZygoteInit::SOCKET_NAME_ARG("--socket-name=");
AutoPtr<ILocalServerSocket> CZygoteInit::sServerSocket;
AutoPtr<IResources> CZygoteInit::mResources;
const Int32 CZygoteInit::GC_LOOP_COUNT;
const String CZygoteInit::PRELOADED_CLASSES("preloaded-classes");
const Boolean CZygoteInit::PRELOAD_RESOURCES = TRUE;

//====================================================================================
// CZygoteInit::MethodAndArgsCaller
//====================================================================================
CZygoteInit::MethodAndArgsCaller::MethodAndArgsCaller(
    /* [in] */ IInterface* object,
    /* [in] */ IMethodInfo* method,
    /* [in] */ ArrayOf<String>* args)
    : mObject(object)
    , mMethod(method)
    , mArgs(args)
{}

ECode CZygoteInit::MethodAndArgsCaller::Run()
{
    AutoPtr<IArgumentList> argumentList;
    ECode ec = mMethod->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        Logger::E("CZygoteInit", "Create \"Main\" method argument list failed!\n");
        return ec;
    }

    argumentList->SetInputArgumentOfCarArray(0, mArgs.Get());

    ec = mMethod->Invoke(mObject, argumentList);
    if (FAILED(ec)) {
        Logger::E("CZygoteInit", "Invoke \"Main\" method failed!\n");
        return ec;
    }
    return ec;
}

//====================================================================================
// CZygoteInit
//====================================================================================
CAR_INTERFACE_IMPL(CZygoteInit, Singleton, IZygoteInit)

CAR_SINGLETON_IMPL(CZygoteInit)

ECode CZygoteInit::RegisterZygoteSocket(
    /* [in] */ const String& socketName)
{
    if (sServerSocket == NULL) {
        String fullSocketName = ANDROID_SOCKET_PREFIX + socketName;
        AutoPtr<Elastos::Core::ISystem> system;
        Elastos::Core::CSystem::AcquireSingleton((Elastos::Core::ISystem**)&system);

        Int32 fileDesc;
        // try {
        String env;
        system->GetEnv(fullSocketName, &env);
        if (env.IsNull()) {
            Logger::E("CZygoteInit", "%s unset or invalid", fullSocketName.string());
            return E_RUNTIME_EXCEPTION;
        }

        ECode ec = StringUtils::Parse(env, &fileDesc);
        if (FAILED(ec)) {
            Logger::E("CZygoteInit", "%s, %s unset or invalid", fullSocketName.string(), env.string());
            return E_RUNTIME_EXCEPTION;
        }
        // } catch (RuntimeException ex) {
        //     throw new RuntimeException(
        //             fullSocketName + " unset or invalid", ex);
        // }

        // try {
        ec = CLocalServerSocket::New(CreateFileDescriptor(fileDesc), (ILocalServerSocket**)&sServerSocket);
        if (FAILED(ec) || sServerSocket == NULL) {
            Logger::E("CZygoteInit", "Error binding to local socket! %s, ec=%08x\n",
                fullSocketName.string(), ec);
            return E_RUNTIME_EXCEPTION;
        }
        // } catch (IOException ex) {
        //     throw new RuntimeException(
        //             "Error binding to local socket '" + fileDesc + "'", ex);
        // }
    }

    Logger::I(TAG, " RegisterZygoteSocket %s: %p", socketName.string(), sServerSocket.Get());
    return NOERROR;
}

ECode CZygoteInit::AcceptCommandPeer(
    /* [in] */ const String& abiList,
    /* [out] */ ZygoteConnection** peer)
{
    VALIDATE_NOT_NULL(peer)
    // try {
    AutoPtr<ILocalSocket> socket;
    FAIL_RETURN(sServerSocket->Accept((ILocalSocket**)&socket));
    *peer = new ZygoteConnection(socket, abiList);
    REFCOUNT_ADD(*peer);
    return NOERROR;
    // } catch (IOException ex) {
    //     throw new RuntimeException(
    //             "IOException during accept()", ex);
    // }
}

void CZygoteInit::CloseServerSocket()
{
    ECode ec = NOERROR;
    do {
        if (sServerSocket != NULL) {
            AutoPtr<IFileDescriptor> fd;
            ec = sServerSocket->GetFileDescriptor((IFileDescriptor**)&fd);
            if (FAILED(ec))
                break;
            ec = sServerSocket->Close();
            if (FAILED(ec))
                break;
            if (fd != NULL) {
                ec = Elastos::Droid::System::Os::Close(fd);
            }
        }
    } while (0);

    if (ec == (ECode)E_IO_EXCEPTION) {
        Logger::E(TAG, "Zygote:  error closing sockets");
    }
    else if (ec == (ECode)E_ERRNO_EXCEPTION) {
        Logger::E(TAG, "Zygote:  error closing descriptor");
    }

    sServerSocket = NULL;
}

AutoPtr<IFileDescriptor> CZygoteInit::GetServerSocketFileDescriptor()
{
    AutoPtr<IFileDescriptor> fd;
    sServerSocket->GetFileDescriptor((IFileDescriptor**)&fd);
    return fd;
}

void CZygoteInit::Preload()
{
    Logger::D(TAG, "begin preload");
    // preloadClasses();
    PreloadResources();
    PreloadOpenGL();
    // preloadSharedLibraries();
    // Ask the WebViewFactory to do any initialization that must run in the zygote process,
    // for memory sharing purposes.
    WebViewFactory::PrepareWebViewInZygote();
    Logger::D(TAG, "end preload");
}

void CZygoteInit::PreloadOpenGL()
{
    Logger::I(TAG, "Preloading opengl...");
    Boolean bval;
    SystemProperties::GetBoolean(PROPERTY_DISABLE_OPENGL_PRELOADING, FALSE, &bval);
    if (!bval) {
        AutoPtr<IEGL14> egl14;
        CEGL14::AcquireSingleton((IEGL14**)&egl14);
        AutoPtr<IEGLDisplay> eglDisplay;
        egl14->EglGetDisplay(IEGL14::_EGL_DEFAULT_DISPLAY, (IEGLDisplay**)&eglDisplay);
    }
}

void CZygoteInit::PreloadResources()
{
    // Debug.startAllocCounting();
    Logger::I(TAG, "Preloading resources...");
    mResources = CResources::GetSystem();
    mResources->StartPreloading();
    if (PRELOAD_RESOURCES) {
        Int64 startTime = SystemClock::GetUptimeMillis();
        AutoPtr<ITypedArray> ar;
        mResources->ObtainTypedArray(R::array::preloaded_drawables, (ITypedArray**)&ar);
        if (ar != NULL) {
            Int32 N = PreloadDrawables(ar);
            ar->Recycle();
            Logger::I(TAG, "...preloaded %d drawables in %lld ms",
                N, (SystemClock::GetUptimeMillis() - startTime));

            startTime = SystemClock::GetUptimeMillis();
            ar = NULL;
            mResources->ObtainTypedArray(R::array::preloaded_color_state_lists, (ITypedArray**)&ar);
            N = PreloadColorStateLists(ar);
            ar->Recycle();
            Logger::I(TAG, "...preloaded %d color state lists in %lld ms",
                N, (SystemClock::GetUptimeMillis() - startTime));
        }
        else {
            Logger::E(TAG, "failed to ObtainTypedArray R::array::preloaded_drawables");
        }
    }
    mResources->FinishPreloading();

    // Debug.stopAllocCounting();
}

Int32 CZygoteInit::PreloadColorStateLists(
    /* [in] */ ITypedArray* ar)
{
    Logger::I(TAG, "Preloading ColorStateLists...");
    Int32 N, id;
    ar->GetLength(&N);
    for (Int32 i = 0; i < N; i++) {
        // if (Debug.getGlobalAllocSize() > PRELOAD_GC_THRESHOLD) {
        //     if (false) {
        //         Logger::V(TAG, " GC at " + Debug.getGlobalAllocSize());
        //     }
        //     System.gc();
        //     runtime.runFinalizationSync();
        //     Debug.resetGlobalAllocSize();
        // }

        ar->GetResourceId(i, 0, &id);
        if (FALSE) {
            Logger::V(TAG, "Preloading resource #0x%08x", id);
        }
        if (id != 0) {
            AutoPtr<IColorStateList> csl;
            mResources->GetColorStateList(id, (IColorStateList**)&csl);
            if (csl == NULL) {
                String str;
                ar->GetString(i, &str);
                Logger::E(TAG, "Unable to find preloaded color resource %d/%d #0x%08x (%s)",
                    i + 1, N, id, str.string());
                return i;
            }
        }
    }
    return N;
}

Int32 CZygoteInit::PreloadDrawables(
    /* [in] */ ITypedArray* ar)
{
    Logger::I(TAG, "Preloading Drawables...");
    Int32 N, id;
    ar->GetLength(&N);
    for (Int32 i = 0; i < N; i++) {
        // if (Debug.getGlobalAllocSize() > PRELOAD_GC_THRESHOLD) {
        //     if (false) {
        //         Logger::V(TAG, " GC at " + Debug.getGlobalAllocSize());
        //     }
        //     System.gc();
        //     runtime.runFinalizationSync();
        //     Debug.resetGlobalAllocSize();
        // }

        ar->GetResourceId(i, 0, &id);
        if (FALSE) {
            Logger::V(TAG, "Preloading resource #0x%08x", id);
        }
        if (id != 0) {
            AutoPtr<IDrawable> drawable;
            mResources->GetDrawable(id, NULL, (IDrawable**)&drawable);
            if (drawable == NULL) {
                String str;
                ar->GetString(i, &str);
                Logger::E(TAG, "Unable to find preloaded drawable resource %d/%d #0x%08x (%s)",
                    i + 1, N, id, str.string());
                return i;
            }
        }
    }
    return N;
}

ECode CZygoteInit::HandleSystemServerProcess(
    /* [in] */ ZygoteConnection::Arguments* parsedArgs,
    /* [out] */ IRunnable** task)
{
    Logger::I(TAG, "Handleing system server process...");
    VALIDATE_NOT_NULL(task);
    *task = NULL;

    CloseServerSocket();

    // set umask to 0077 so new files and directories will default to owner-only permissions.
    Int32 result;
    Elastos::Droid::System::Os::Umask(OsConstants::_S_IRWXG | OsConstants::_S_IRWXO, &result);

    if (!parsedArgs->mNiceName.IsNull()) {
        Process::SetArgV0(parsedArgs->mNiceName);
    }

    String systemServerClasspath;
    Elastos::Droid::System::Os::Getenv(String("SYSTEMSERVERCLASSPATH"), &systemServerClasspath);
    if (systemServerClasspath != NULL) {
        // PerformSystemServerDexOpt(systemServerClasspath);
    }

    if (!parsedArgs->mInvokeWith.IsNull()) {
        AutoPtr<ArrayOf<String> > args = parsedArgs->mRemainingArgs;
        // If we have a non-null system server class path, we'll have to duplicate the
        // existing arguments and append the classpath to it. ART will handle the classpath
        // correctly when we exec a new process.
        if (systemServerClasspath != NULL) {
            AutoPtr<ArrayOf<String> > amendedArgs = ArrayOf<String>::Alloc(args->GetLength() + 2);
            amendedArgs->Set(0, String("-cp"));
            amendedArgs->Set(1, systemServerClasspath);
            amendedArgs->Copy(2, args);
        }

        return WrapperInit::ExecApplication(parsedArgs->mInvokeWith,
            parsedArgs->mNiceName, parsedArgs->mTargetSdkVersion,
            NULL, args);
    }
    else {
        // ClassLoader cl = null;
        // if (systemServerClasspath != null) {
        //     cl = new PathClassLoader(systemServerClasspath, ClassLoader.getSystemClassLoader());
        //     Thread.currentThread().setContextClassLoader(cl);
        // }
        /*
         * Pass the remaining arguments to SystemServer.
         */
        return RuntimeInit::ZygoteInit(parsedArgs->mTargetSdkVersion, parsedArgs->mRemainingArgs, task);
    }
}

/**
 * Prepare the arguments and fork for the system server process.
 */
ECode CZygoteInit::StartSystemServer(
    /* [in] */ const String& abiList,
    /* [in] */ const String& socketName,
    /* [out] */ IRunnable** task)
{
    Logger::I(TAG, "CZygoteInit::StartSystemServer: abiList:[%s], socketName:[%s]", abiList.string(), socketName.string());
    VALIDATE_NOT_NULL(task);
    *task = NULL;

    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(11);
    (*array)[0] = OsConstants::_CAP_BLOCK_SUSPEND;
    (*array)[1] = OsConstants::_CAP_KILL;
    (*array)[2] = OsConstants::_CAP_NET_ADMIN;
    (*array)[3] = OsConstants::_CAP_NET_BIND_SERVICE;
    (*array)[4] = OsConstants::_CAP_NET_BROADCAST;
    (*array)[5] = OsConstants::_CAP_NET_RAW;
    (*array)[6] = OsConstants::_CAP_SYS_MODULE;
    (*array)[7] = OsConstants::_CAP_SYS_NICE;
    (*array)[8] = OsConstants::_CAP_SYS_RESOURCE;
    (*array)[9] = OsConstants::_CAP_SYS_TIME;
    (*array)[10] = OsConstants::_CAP_SYS_TTY_CONFIG;
    Int64 capabilities = 0;
    FAIL_RETURN(PosixCapabilitiesAsBits(array, &capabilities));

//    /* Hardcoded command line to start the system server */
    AutoPtr<ArrayOf<String> > args = ArrayOf<String>::Alloc(8);
    (*args)[0] = "--setuid=1000";
    (*args)[1] = "--setgid=1000";
    (*args)[2] = "--setgroups=1001,1002,1003,1004,1005,1006,1007,1008,1009,1010,1018,1032,3001,3002,3003,3006,3007";
    StringBuilder sb("--capabilities=");
    sb += capabilities;
    sb += ",";
    sb += capabilities;
    (*args)[3] = sb.ToString();
    (*args)[4] = "--runtime-init";
    (*args)[5] = "--nice-name=elsystemserver";
    (*args)[6] = "Elastos.Droid.Server.eco";
    (*args)[7] = "LElastos/Droid/Server/CSystemServer;";

    Int32 pid;
//    try {
    AutoPtr<ZygoteConnection::Arguments> parsedArgs = new ZygoteConnection::Arguments(*args);
    ZygoteConnection::ApplyDebuggerSystemProperty(parsedArgs);
    ZygoteConnection::ApplyInvokeWithSystemProperty(parsedArgs);

    Logger::I(TAG, "Request to fork the system server process...");
    /* Request to fork the system server process */
    pid = Zygote::ForkSystemServer(
            parsedArgs->mUid, parsedArgs->mGid,
            parsedArgs->mGids,
            parsedArgs->mDebugFlags,
            NULL,
            parsedArgs->mPermittedCapabilities,
            parsedArgs->mEffectiveCapabilities);
//    } catch (IllegalArgumentException ex) {
//        throw new RuntimeException(ex);
//    }
//
    Logger::I(TAG, "Request to fork the system server process done pid: %d", pid);
    /* For child process */
    if (pid == 0) {
        if (HasSecondZygote(abiList)) {
            WaitForSecondaryZygote(socketName);
        }
        HandleSystemServerProcess(parsedArgs, task);
    }

    return NOERROR;
}

ECode CZygoteInit::PosixCapabilitiesAsBits(
    /* [in] */ ArrayOf<Int32>* capabilities,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    for (Int32 i = 0; i < capabilities->GetLength(); ++i) {
        Int32 capability = (*capabilities)[i];
        if ((capability < 0) || (capability > OsConstants::_CAP_LAST_CAP)) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        *result |= (1LL << capability);
    }
    return NOERROR;
}

ECode CZygoteInit::Main(
    /* [in] */ const ArrayOf<String>& argv)
{
    Logger::I(TAG, "CZygoteInit::Main.");
    for (Int32 i = 0; i < argv.GetLength(); i++) {
        Logger::I(TAG, "  >> arg %d: %s", i, argv[i].string());
    }

    // try {
        // Start profiling the zygote initialization.
        //SamplingProfilerIntegration.start();

        ECode ec = NOERROR;
        AutoPtr<IRunnable> task;
        Boolean startSystemServer = FALSE;
        String socketName("elzygote");
        String abiList;
        for (Int32 i = 0; i < argv.GetLength(); i++) {
            if (argv[i].Equals("start-system-server")) {
                startSystemServer = TRUE;
            }
            else if (argv[i].StartWith(ABI_LIST_ARG)) {
                abiList = argv[i].Substring(ABI_LIST_ARG.GetLength());
            }
            else if (argv[i].StartWith(SOCKET_NAME_ARG)) {
                socketName = argv[i].Substring(SOCKET_NAME_ARG.GetLength());
            }
            else {
                Logger::E(TAG, "Unknown command line argument: %s", argv[i].string());
                goto _RUNTIME_EXCEPTION_EXIT_;
                //throw new RuntimeException("Unknown command line argument: " + argv[i]);
            }
        }

        if (abiList.IsNull()) {
            Logger::E(TAG, "No ABI list supplied.");
            goto _RUNTIME_EXCEPTION_EXIT_;
            //throw new RuntimeException("No ABI list supplied.");
        }

        ec = RegisterZygoteSocket(socketName);
        if (FAILED(ec)) goto _RUNTIME_EXCEPTION_EXIT_;

        // EventLog.writeEvent(LOG_BOOT_PROGRESS_PRELOAD_START,
        //     SystemClock.uptimeMillis());
        Preload();
        // EventLog.writeEvent(LOG_BOOT_PROGRESS_PRELOAD_END,
        //     SystemClock.uptimeMillis());

        // Finish profiling the zygote initialization.
        //SamplingProfilerIntegration.writeZygoteSnapshot();

        // Do an initial gc to clean up after startup
        //gc();

        // Disable tracing so that forked processes do not inherit stale tracing tags from
        // Zygote.
        //Trace.setTracingEnabled(false);

        if (startSystemServer) {
            StartSystemServer(abiList, socketName, (IRunnable**)&task);
            if (task != NULL) goto _RUN_TASK_;
        }

        Logger::V(TAG, "Accepting command socket connections");
        task = NULL;
        RunSelectLoop(abiList, (IRunnable**)&task);
        if (task != NULL) goto _RUN_TASK_;

        CloseServerSocket();
        return NOERROR;

_RUN_TASK_:
        /* For child process */
        return task->Run();

    // } catch (MethodAndArgsCaller caller) {
    //    caller.run();
    // } catch (RuntimeException ex) {
_RUNTIME_EXCEPTION_EXIT_:
        Logger::E(TAG, "Zygote died with runtime exception");
        CloseServerSocket();
        //throw ex;
        return E_RUNTIME_EXCEPTION;
    // }
}

/**
 * Return {@code true} if this device configuration has another zygote.
 *
 * We determine this by comparing the device ABI list with this zygotes
 * list. If this zygote supports all ABIs this device supports, there won't
 * be another zygote.
 */
Boolean CZygoteInit::HasSecondZygote(
    /* [in] */ const String& abiList)
{
    String prop;
    SystemProperties::Get(String("ro.product.cpu.abilist"), &prop);
    return !prop.Equals(abiList);
}

void CZygoteInit::WaitForSecondaryZygote(
    /* [in] */ const String& socketName)
{
    Logger::I(TAG, "WaitForSecondaryZygote %s", socketName.string());
    ECode ec = NOERROR;
    String otherZygoteName = Process::ZYGOTE_SOCKET_ELASTOS.Equals(socketName) ?
            Process::SECONDARY_ZYGOTE_SOCKET_ELASTOS : Process::ZYGOTE_SOCKET_ELASTOS;
    while (TRUE) {
        AutoPtr<Process::ZygoteState> zs;
        ec = Process::ZygoteState::Connect(otherZygoteName, (Process::ZygoteState**)&zs);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Logger::W(TAG, "Got error connecting to zygote, retrying. msg= " /*+ ioe.getMessage()*/);
        }
        else {
            zs->Close();
            break;
        }

        Thread::Sleep(1000);
    }
}

ECode CZygoteInit::RunSelectLoop(
    /* [in] */ const String& abiList,
    /* [out] */ IRunnable** task)
{
    VALIDATE_NOT_NULL(task);
    *task = NULL;

    Vector< AutoPtr<IFileDescriptor> > fds;
    Vector< AutoPtr<ZygoteConnection> > peers;
    AutoPtr< ArrayOf<IFileDescriptor*> > fdArray = ArrayOf<IFileDescriptor*>::Alloc(4);

    AutoPtr<IFileDescriptor> fd;
    assert(sServerSocket != NULL);
    sServerSocket->GetFileDescriptor((IFileDescriptor**)&fd);
    fds.PushBack(fd);
    peers.PushBack(NULL);

    while (TRUE) {
        Int32 index;

        // try {
        Int32 count = fds.GetSize();
        if (count > fdArray->GetLength()) {
            fdArray = ArrayOf<IFileDescriptor*>::Alloc(count);
        }
        for (Int32 i = 0; i < count; i++) {
            fdArray->Set(i, fds[i]);
        }
        ECode ec = SelectReadable(fdArray, &index);
        if (ec == (ECode)E_IO_EXCEPTION) {
            return E_RUNTIME_EXCEPTION;
        }

        if (index < 0) {
            Logger::E(TAG, "Error in select()");
            return E_RUNTIME_EXCEPTION;
        }
        else if (index == 0) {
            // first connection
            AutoPtr<ZygoteConnection> newPeer;
            FAIL_RETURN(AcceptCommandPeer(abiList, (ZygoteConnection**)&newPeer));
            peers.PushBack(newPeer);
            AutoPtr<IFileDescriptor> fd;
            newPeer->GetFileDescriptor((IFileDescriptor**)&fd);
            fds.PushBack(fd);
        }
        else {
            // recieve data
            Boolean done = peers[index]->RunOnce(task);
            if (*task != NULL) return NOERROR;

            if (done) {
                peers.Erase(peers.Begin() + index);
                fds.Erase(fds.Begin() + index);
            }
        }
    }
    return NOERROR;
}

Int32 CZygoteInit::Setreuid(
    /* [in] */ Int32 ruid,
    /* [in] */ Int32 euid)
{
    if (setreuid(ruid, euid) < 0) {
        return errno;
    }
    return 0;
}

Int32 CZygoteInit::Setregid(
    /* [in] */ Int32 rgid,
    /* [in] */ Int32 egid)
{
    if (setregid(rgid, egid) < 0) {
        return errno;
    }
    return 0;
}

Int32 CZygoteInit::Setpgid(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 pgid)
{
    if (setpgid(pid, pgid) < 0) {
        return errno;
    }
    return 0;
}

ECode CZygoteInit::Getpgid(
    /* [in] */ Int32 pid,
    /* [out] */ Int32* pgid)
{
    VALIDATE_NOT_NULL(pgid);
    *pgid = 0;

    pid_t ret;
    ret = getpgid(pid);

    if (ret < 0) {
        // jniThrowIOException(env, errno);
        return E_IO_EXCEPTION;
    }

    *pgid = (Int32)ret;
    return NOERROR;
}

void CZygoteInit::ReopenStdio(
    /* [in] */ IFileDescriptor* infd,
    /* [in] */ IFileDescriptor* outfd,
    /* [in] */ IFileDescriptor* errfd)
{
    Int32 fd;
    int err;

    infd->GetDescriptor(&fd);

    do {
        err = dup2(fd, STDIN_FILENO);
    } while (err < 0 && errno == EINTR);

    outfd->GetDescriptor(&fd);

    do {
        err = dup2(fd, STDOUT_FILENO);
    } while (err < 0 && errno == EINTR);

    errfd->GetDescriptor(&fd);

    do {
        err = dup2(fd, STDERR_FILENO);
    } while (err < 0 && errno == EINTR);
}

ECode CZygoteInit::SetCloseOnExec(
    /* [in] */ IFileDescriptor* descriptor,
    /* [in] */ Boolean flag)
{
    Int32 fd;
    int err;
    int fdFlags;

    descriptor->GetDescriptor(&fd);

    fdFlags = fcntl(fd, F_GETFD);

    if (fdFlags < 0) {
        // jniThrowIOException(env, errno);
        return E_IO_EXCEPTION;
    }

    if (flag) {
        fdFlags |= FD_CLOEXEC;
    }
    else {
        fdFlags &= ~FD_CLOEXEC;
    }

    err = fcntl(fd, F_SETFD, fdFlags);

    if (err < 0) {
        // jniThrowIOException(env, errno);
        return E_IO_EXCEPTION;
    }
    return NOERROR;
}

ECode CZygoteInit::SelectReadable(
    /* [in] */ ArrayOf<IFileDescriptor*>* fds,
    /* [out] */ Int32* index)
{
    VALIDATE_NOT_NULL(index)
    *index = -1;

    if (fds == NULL) {
        // jniThrowNullPointerException(env, "fds == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    Int32 length = fds->GetLength();
    fd_set fdset;

    FD_ZERO(&fdset);

    int nfds = 0;
    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IFileDescriptor> fdObj = (*fds)[i];
        if (fdObj == NULL) {
            continue;
        }
        Int32 fd;
        fdObj->GetDescriptor(&fd);

        FD_SET(fd, &fdset);

        if (fd >= nfds) {
            nfds = fd + 1;
        }
    }

    int err;
    do {
        err = select(nfds, &fdset, NULL, NULL, NULL);
    } while (err < 0 && errno == EINTR);

    if (err < 0) {
        // jniThrowIOException(env, errno);
        return E_IO_EXCEPTION;
    }

    for (Int32 i = 0; i < length; i++) {
        AutoPtr<IFileDescriptor> fdObj = (*fds)[i];
        if (fdObj == NULL) {
            continue;
        }
        Int32 fd;
        fdObj->GetDescriptor(&fd);
        if (FD_ISSET(fd, &fdset)) {
            *index = i;
            return NOERROR;
        }
    }

    return NOERROR;
}

AutoPtr<IFileDescriptor> CZygoteInit::CreateFileDescriptor(
    /* [in] */ Int32 fd)
{
    AutoPtr<IFileDescriptor> fileDesc;
    CFileDescriptor::New((IFileDescriptor**)&fileDesc);
    fileDesc->SetDescriptor(fd);
    return fileDesc;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
