
#include <sys/mount.h>
#include "elastos/droid/DroidRuntime.h"
#include <elastos/coredef.h>
#include <elastos/utility/logging/Logger.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <utils/Log.h>
#include <utils/misc.h>
#include <binder/Parcel.h>
#include <utils/threads.h>
#include <cutils/properties.h>

#include <stdio.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <dirent.h>
#include <assert.h>

using Elastos::Utility::Logging::Logger;

#ifndef ASSERT_TRUE
#define ASSERT_TRUE(expr) \
    do { \
        Boolean assert_result_ = (Boolean)(expr); \
        assert(assert_result_); \
    } while(0);
#endif

extern AutoPtr<Elastos::Droid::DroidRuntime> sCurRuntime;

namespace Elastos {
namespace Droid {

static const String TAG("DroidRuntime");

static int hasDir(const char* dir)
{
    struct stat s;
    int res = stat(dir, &s);
    if (res == 0) {
        return S_ISDIR(s.st_mode);
    }
    return 0;
}

/*
 * We just want failed write() calls to just return with an error.
 */
static void BlockSigpipe()
{
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGPIPE);
    if (sigprocmask(SIG_BLOCK, &mask, NULL) != 0) {
        Logger::W(TAG, "WARNING: SIGPIPE not blocked\n");
    }
}


// ----------------------------------------------------------------------

DroidRuntime::DroidRuntime(
    /* [in] */ char* argBlockStart,
    /* [in] */ const size_t argBlockLength)
    : mArgBlockStart(argBlockStart)
    , mArgBlockLength(argBlockLength)
    , mExitWithoutCleanup(FALSE)
{
    // move to AppRuntime　in app_main.cpp
    //SkGraphics::Init();
    // There is also a global font cache, but its budget is specified in code
    // see SkFontHost_android.cpp

    // Pre-allocate enough space to hold a fair number of options.
    //mOptions.setCapacity(20);

    SetInstructionSet(kRuntimeISA);

    assert(sCurRuntime == NULL);        // one per process
    sCurRuntime = this;
}

DroidRuntime::~DroidRuntime()
{
    // move to AppRuntime　in app_main.cpp
    //SkGraphics::Term();
}

void DroidRuntime::SetArgv0(
    /* [in] */ const String& argv0)
{
    memset(mArgBlockStart, 0, mArgBlockLength);
    strlcpy(mArgBlockStart, argv0.string(), mArgBlockLength);
}

void DroidRuntime::AddOption(
    /* [in] */ const String& optionString,
    /* [in] */ void* extraInfo)
{
    // JavaVMOption opt;
    // opt.optionString = optionString;
    // opt.extraInfo = extraInfo;
    // mOptions.add(opt);
}

ECode DroidRuntime::CallMain(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [in] */ ArrayOf<String>* args)
{
    Logger::D(TAG, "Calling main entry \"%s/%s\"", moduleName.string(), className.string());
    // for (Int32 i = 0; i < args->GetLength(); i++) {
    //     Logger::I(TAG, "  >> arg %d: %s", i, (*args)[i].string());
    // }

    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IInterface> object;
    AutoPtr<IMethodInfo> methodInfo;
    AutoPtr<IArgumentList> argumentList;

    ECode ec = _CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        Logger::E(TAG, "Acquire \"%s\" module info failed!\n",
            moduleName.string());
        return ec;
    }

    ec = moduleInfo->GetClassInfo(className, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        Logger::E(TAG, "Acquire \"%s/%s\" class info failed!\n",
            moduleName.string(), className.string());
        return ec;
    }

    ec = classInfo->CreateObject((IInterface**)&object);
    if (FAILED(ec)) {
        Logger::E(TAG, "Create object failed! \"%s/%s\" \n",
            moduleName.string(), className.string());
        return ec;
    }

    ec = classInfo->GetMethodInfo(
            String("Main"), String("([LElastos/String;)E"), (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        Logger::E(TAG, "Acquire \"%s/%s\" \"Main\" method info failed!\n",
            moduleName.string(), className.string());
        return ec;
    }

    ec = methodInfo->CreateArgumentList((IArgumentList**)&argumentList);
    if (FAILED(ec)) {
        Logger::E(TAG, "Create \"%s/%s\" \"Main\" method argument list failed!\n",
            moduleName.string(), className.string());
        return ec;
    }

    ec = argumentList->SetInputArgumentOfCarArray(0, *args);
    if (FAILED(ec)) {
        Logger::E(TAG, "Set Input Argument of Car array in \"%s/%s\" \"Main\" method failed!\n",
            moduleName.string(), className.string());
        return ec;
    }

    ec = methodInfo->Invoke(object, argumentList);
    if (FAILED(ec)) {
        Logger::E(TAG, "Invoke \"%s/%s\" \"Main\" method failed!\n",
            moduleName.string(), className.string());
        return ec;
    }
    return NOERROR;
}

/*
 * Configure signals.  We need to block SIGQUIT so that the signal only
 * reaches the dump-stack-trace thread.
 *
 * This can be disabled with the "-Xrs" flag.
 */
static void BlockSignals()
{
    sigset_t mask;
    int UNUSED(cc);

    sigemptyset(&mask);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGUSR1);      // used to initiate heap dump
//#if defined(WITH_JIT) && defined(WITH_JIT_TUNING)
//    sigaddset(&mask, SIGUSR2);      // used to investigate JIT internals
//#endif
    //sigaddset(&mask, SIGPIPE);
    cc = sigprocmask(SIG_BLOCK, &mask, NULL);
    assert(cc == 0);

//    if (false) {
//        /* TODO: save the old sigaction in a global */
//        struct sigaction sa;
//        memset(&sa, 0, sizeof(sa));
//        sa.sa_sigaction = busCatcher;
//        sa.sa_flags = SA_SIGINFO;
//        cc = sigaction(SIGBUS, &sa, NULL);
//        assert(cc == 0);
//    }
}

// Do zygote-mode-only initialization.
static bool InitZygote()
{
#ifdef __linux__
    // zygote goes into its own process group
    setpgid(0, 0);

    // See storage config details at http://source.android.com/tech/storage/
    // Create private mount namespace shared by all children
    if (unshare(CLONE_NEWNS) == -1) {
        Logger::E("DroidRuntime", "Failed to unshare()");
        return false;
    }

    // Mark rootfs as being a slave so that changes from default
    // namespace only flow into our children.
    if (mount("rootfs", "/", NULL, (MS_SLAVE | MS_REC), NULL) == -1) {
        Logger::E("DroidRuntime", "Failed to mount() rootfs as MS_SLAVE");
        return false;
    }

    // Create a staging tmpfs that is shared by our children; they will
    // bind mount storage into their respective private namespaces, which
    // are isolated from each other.
    const char* target_base = getenv("EMULATED_STORAGE_TARGET");
    if (target_base != NULL) {
        if (mount("tmpfs", target_base, "tmpfs", MS_NOSUID | MS_NODEV,
                "uid=0,gid=1028,mode=0751") == -1) {
            Logger::E("DroidRuntime", "Failed to mount tmpfs to %s", target_base);
            return false;
        }
    }

    return true;
#else
    UNIMPLEMENTED(FATAL);
    return false;
#endif
}

/*
 * Start the Android runtime.  This involves starting the virtual machine
 * and calling the "static void main(String[] args)" method in the class
 * named by "className".
 *
 * Passes the main function two arguments, the class name and the specified
 * options string.
 */
void DroidRuntime::Start(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [in] */ ArrayOf<String>* options)
{
    Logger::D(TAG, "\n>>>>>> DroidRuntime START \"%s/%s\" <<<<<<\n",
            !moduleName.IsNull() ? moduleName.string() : "(unknown)",
            !className.IsNull() ? className.string() : "(unknown)");

    BlockSigpipe();

    static const String startSystemServer("start-system-server");

    /*
     * 'startSystemServer == true' means runtime is obsolete and not run from
     * init.rc anymore, so we print out the boot start event here.
     */
    if (options) {
        for (Int32 i = 0; i < options->GetLength(); ++i) {
            if ((*options)[i].Equals(startSystemServer)) {
               /* track our progress through the boot sequence */
               const int LOG_BOOT_PROGRESS_START = 3000;
               LOG_EVENT_LONG(LOG_BOOT_PROGRESS_START,  ns2ms(systemTime(SYSTEM_TIME_MONOTONIC)));
            }
        }
    }

    const char* rootDir = getenv("ANDROID_ROOT");
    if (rootDir == NULL) {
        rootDir = "/system";
        if (!hasDir("/system")) {
            Logger::E(TAG, "No root directory specified, and /android does not exist.");
            return;
        }
        setenv("ANDROID_ROOT", rootDir, 1);
    }

    //const char* kernelHack = getenv("LD_ASSUME_KERNEL");
    //ALOGD("Found LD_ASSUME_KERNEL='%s'\n", kernelHack);

    // from startVm
    BlockSignals(); // call from Runtime::Create in JNI_CreateJavaVM

    bool inited = InitZygote(); // call from Runtime::Start in JNI_CreateJavaVM
    assert(inited);

    CallMain(moduleName, className, options);

    Logger::D(TAG, "Shutting down\n");
}

void DroidRuntime::Exit(
    /* [in] */ Int32 code)
{
    if (mExitWithoutCleanup) {
        ALOGI("VM exiting with result code %d, cleanup skipped.", code);
        ::_exit(code);
    }
    else {
        ALOGI("VM exiting with result code %d.", code);
        OnExit(code);
        ::exit(code);
    }
}

AutoPtr<Elastos::Droid::DroidRuntime> DroidRuntime::GetRuntime()
{
    return sCurRuntime;
}

String DroidRuntime::GetInstructionSetString()
{
    InstructionSet is = GetInstructionSet();
    return GetInstructionSetString(is);
}

InstructionSet DroidRuntime::GetInstructionSetFromString(
    /* [in] */ const String& instruction_set)
{
    if (instruction_set.Equals("arm")) {
        return kArm;
    }
    else if (instruction_set.Equals("arm64")) {
        return kArm64;
    }
    else if (instruction_set.Equals("x86")) {
        return kX86;
    }
    else if (instruction_set.Equals("x86_64")) {
        return kX86_64;
    }
    else if (instruction_set.Equals("mips")) {
        return kMips;
    }

    return kNone;
}

String DroidRuntime::GetInstructionSetString(
    /* [in] */ InstructionSet isa)
{
    switch (isa) {
    case kArm:
    case kThumb2:
        return String("arm");
    case kArm64:
        return String("arm64");
    case kX86:
        return String("x86");
    case kX86_64:
        return String("x86_64");
    case kMips:
        return String("mips");
    case kNone:
        return String("none");
    default:
        ALOGE("Unknown ISA %d", isa);
      return String(NULL);
    }
}

Boolean DroidRuntime::Is64BitInstructionSet(
    /* [in] */ InstructionSet isa)
{
    switch (isa) {
    case kArm:
    case kThumb2:
    case kX86:
    case kMips:
      return FALSE;

    case kArm64:
    case kX86_64:
      return TRUE;

    case kNone:
      ALOGE("ISA kNone does not have bit width.");
      return FALSE;
    default:
      ALOGE("Unknown ISA %d", isa);
      return FALSE;
    }
}

} // namespace Droid
} // namespace Elastos
