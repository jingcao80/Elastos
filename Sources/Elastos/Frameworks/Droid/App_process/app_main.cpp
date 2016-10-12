
#include <elastos/coredef.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <utils/Log.h>
#include <cutils/memory.h>
#include <cutils/process_name.h>
#include <cutils/properties.h>
#include <cutils/trace.h>
#include <elastos/droid/DroidRuntime.h>
#include <private/android_filesystem_config.h>  // for AID_SYSTEM

#include <skia/core/SkGraphics.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>

using namespace Elastos;
using namespace android;

namespace Elastos {
namespace Droid {

class AppRuntime : public DroidRuntime
{
public:

    AppRuntime(
        /* [in] */ char* argBlockStart,
        /* [in] */ const size_t argBlockLength)
        : DroidRuntime(argBlockStart, argBlockLength)
    {
        SkGraphics::Init();
    }

    virtual ~AppRuntime()
    {
        SkGraphics::Term();
    }

    CARAPI_(void) SetClassNameAndArgs(
        /* [in] */ const String& className,
        /* [in] */ Int32 argc,
        /* [in] */ char * const *argv);

    CARAPI_(void) OnStarted();

    virtual CARAPI_(void) OnZygoteInit();

    virtual CARAPI_(void) OnExit(
        /* [in] */ Int32 code);

public:
    String mModuleName;
    String mClassName;
    AutoPtr<ArrayOf<String> > mArgs;
};

void AppRuntime::SetClassNameAndArgs(
    /* [in] */ const String& className,
    /* [in] */ Int32 argc,
    /* [in] */ char * const *argv)
{
    assert(argc >= 0);
    mClassName = className;
    mArgs = ArrayOf<String>::Alloc(argc);
    for (int i = 0; i < argc; ++i) {
        String arg(argv[i]);
        mArgs->Set(i, arg);
    }
}

void AppRuntime::OnStarted()
{
    ALOGV("App process: OnStarted.\n");
    sp<ProcessState> proc = ProcessState::self();
    ALOGV("App process: starting thread pool.\n");
    proc->startThreadPool();

    AutoPtr<DroidRuntime> ar = DroidRuntime::GetRuntime();
    ar->CallMain(mModuleName, mClassName, mArgs);

    IPCThreadState::self()->stopProcess();
}

void AppRuntime::OnZygoteInit()
{
    ALOGV("App process: OnZygoteInit.\n");
    // Re-enable tracing now that we're no longer in Zygote.
    // TODO
    //atrace_set_tracing_enabled(true);

    sp<ProcessState> proc = ProcessState::self();
    ALOGV("App process: starting thread pool.\n");
    proc->startThreadPool();
}

void AppRuntime::OnExit(int code)
{
    if (mClassName.IsNullOrEmpty()) {
        // if zygote
        IPCThreadState::self()->stopProcess();
    }

    DroidRuntime::OnExit(code);
}


} // namespace Droid
} // namespace Elastos

void app_usage()
{
    // /system/bin/ElApp_process /system/bin --zygote --start-system-server
    fprintf(stderr,
        "Usage: ElApp_process [java-options] cmd-dir start-class-name [options]\n");
}

static size_t computeArgBlockSize(int argc, char* const argv[])
{
    // TODO: This assumes that all arguments are allocated in
    // contiguous memory. There isn't any documented guarantee
    // that this is the case, but this is how the kernel does it
    // (see fs/exec.c).
    //
    // Also note that this is a constant for "normal" android apps.
    // Since they're forked from zygote, the size of their command line
    // is the size of the zygote command line.
    //
    // We change the process name of the process by over-writing
    // the start of the argument block (argv[0]) with the new name of
    // the process, so we'd mysteriously start getting truncated process
    // names if the zygote command line decreases in size.
    uintptr_t start = reinterpret_cast<uintptr_t>(argv[0]);
    uintptr_t end = reinterpret_cast<uintptr_t>(argv[argc - 1]);
    end += strlen(argv[argc - 1]) + 1;
    return (end - start);
}

static void maybeCreateDalvikCache()
{
#if defined(__aarch64__)
    static const char kInstructionSet[] = "arm64";
#elif defined(__x86_64__)
    static const char kInstructionSet[] = "x86_64";
#elif defined(__arm__)
    static const char kInstructionSet[] = "arm";
#elif defined(__i386__)
    static const char kInstructionSet[] = "x86";
#elif defined (__mips__)
    static const char kInstructionSet[] = "mips";
#else
#error "Unknown instruction set"
#endif
    const char* androidRoot = getenv("ANDROID_DATA");
    LOG_ALWAYS_FATAL_IF(androidRoot == NULL, "ANDROID_DATA environment variable unset");

    char dalvikCacheDir[PATH_MAX];
    const int numChars = snprintf(dalvikCacheDir, PATH_MAX,
            "%s/dalvik-cache/%s", androidRoot, kInstructionSet);
    LOG_ALWAYS_FATAL_IF((numChars >= PATH_MAX || numChars < 0),
            "Error constructing dalvik cache : %s", strerror(errno));

    int result = mkdir(dalvikCacheDir, 0711);
    LOG_ALWAYS_FATAL_IF((result < 0 && errno != EEXIST),
            "Error creating cache dir %s : %s", dalvikCacheDir, strerror(errno));

    // We always perform these steps because the directory might
    // already exist, with wider permissions and a different owner
    // than we'd like.
    result = chown(dalvikCacheDir, AID_ROOT, AID_ROOT);
    LOG_ALWAYS_FATAL_IF((result < 0), "Error changing dalvik-cache ownership : %s", strerror(errno));

    result = chmod(dalvikCacheDir, 0711);
    LOG_ALWAYS_FATAL_IF((result < 0),
            "Error changing dalvik-cache permissions : %s", strerror(errno));
}

#if defined(__LP64__)
static const char ABI_LIST_PROPERTY[] = "ro.product.cpu.abilist64";
static const char ZYGOTE_NICE_NAME[] = "elzygote64";
#else
static const char ABI_LIST_PROPERTY[] = "ro.product.cpu.abilist32";
static const char ZYGOTE_NICE_NAME[] = "elzygote";
#endif

int main(int argc, char* argv[])
{
    ALOGV("App_process main()\n");
    for (Int32 i = 0; i < argc; ++i) {
        ALOGV(" > raw arg %d : %s", i, argv[i]);
    }

    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0) {
        // Older kernels don't understand PR_SET_NO_NEW_PRIVS and return
        // EINVAL. Don't die on such kernels.
        if (errno != EINVAL) {
            LOG_ALWAYS_FATAL("PR_SET_NO_NEW_PRIVS failed: %s", strerror(errno));
            return 12;
        }
    }

    Elastos::Droid::AppRuntime runtime(argv[0], computeArgBlockSize(argc, argv));
    // Process command line arguments
    // ignore argv[0]
    argc--;
    argv++;

    // Everything up to '--' or first non '-' arg goes to the vm.
    //
    // The first argument after the VM args is the "parent dir", which
    // is currently unused.
    //
    // After the parent dir, we expect one or more the following internal
    // arguments :
    //
    // --zygote : Start in zygote mode
    // --start-system-server : Start the system server.
    // --application : Start in application (stand alone, non zygote) mode.
    // --nice-name : The nice name for this process.
    //
    // For non zygote starts, these arguments will be followed by
    // the main class name. All remaining arguments are passed to
    // the main method of this class.
    //
    // For zygote starts, all remaining arguments are passed to the zygote.
    // main function.
    //
    // Note that we must copy argument string values since we will rewrite the
    // entire argument block when we apply the nice name to argv0.

    Int32 i;
    for (i = 0; i < argc; i++) {
        if (argv[i][0] != '-') {
            break;
        }
        if (argv[i][1] == '-' && argv[i][2] == 0) {
            ++i; // Skip --.
            break;
        }
        runtime.AddOption(String(strdup(argv[i])));
    }

    // Parse runtime arguments.  Stop at first unrecognized option.
    Boolean zygote = FALSE;
    Boolean startSystemServer = FALSE;
    Boolean application = FALSE;
    const char* niceName = NULL;
    const char* className = NULL;

    ++i;  // Skip unused "parent dir" argument.
    while (i < argc) {
        const char* arg = argv[i++];
        if (strcmp(arg, "--zygote") == 0) {
            zygote = TRUE;
            niceName = ZYGOTE_NICE_NAME;
        }
        else if (strcmp(arg, "--start-system-server") == 0) {
            startSystemServer = TRUE;
        }
        else if (strcmp(arg, "--application") == 0) {
            application = TRUE;
        }
        else if (strncmp(arg, "--nice-name=", 12) == 0) {
            niceName = arg + 12;
        }
        else if (strncmp(arg, "--", 2) != 0) {
            className = arg;
            break;
        }
        else {
            --i;
            break;
        }
    }

    Vector<String> args;
    if (className) {
        // We're not in zygote mode, the only argument we need to pass
        // to RuntimeInit is the application argument.
        //
        // The Remainder of args get passed to startup class main(). Make
        // copies of them before we overwrite them with the process name.
        args.add(application ? String("application") : String("tool"));
        runtime.SetClassNameAndArgs(String(className), argc - i, argv + i);
    }
    else {
        // We're in zygote mode.
        maybeCreateDalvikCache();

        if (startSystemServer) {
            args.add(String("start-system-server"));
        }

        char prop[PROP_VALUE_MAX];
        if (property_get(ABI_LIST_PROPERTY, prop, NULL) == 0) {
            LOG_ALWAYS_FATAL("app_process: Unable to determine ABI list from property %s.",
                ABI_LIST_PROPERTY);
            return 11;
        }

        String abiFlag("--abi-list=");
        abiFlag.Append(prop);
        args.add(abiFlag);

        // In zygote mode, pass all remaining arguments to the zygote
        // main() method.
        for (; i < argc; ++i) {
            args.add(String(argv[i]));
        }
    }

    if (niceName) {
        runtime.SetArgv0(String(niceName));
        set_process_name(niceName);
    }

    const size_t numArgs = args.size();
    AutoPtr<ArrayOf<String> > argArray = ArrayOf<String>::Alloc(numArgs);
    for (size_t i = 0; i < numArgs; i++) {
        argArray->Set(i, args[i]);
        ALOGV(" > arg %d: %s", i, args[i].string());
    }

    if (zygote) {
        runtime.Start(
            String("Elastos.Droid.Core.eco"),
            String("Elastos.Droid.Internal.Os.CZygoteInit"),
            argArray);
    }
    else if (className) {
        if (0 == strcmp(className, "CMonkey")) {
            runtime.mModuleName = "Elastos.Droid.Commands.Monkey.eco";
        }
        else if (0 == strcmp(className, "CAm")) {
            runtime.mModuleName = "Elastos.Droid.Commands.Am.eco";
        }

        runtime.Start(
            String("Elastos.Droid.Core.eco"),
            String("Elastos.Droid.Internal.Os.CRuntimeInit"),
            argArray);
    }
    else {
       fprintf(stderr, "Error: no class name or --zygote supplied.\n");
       app_usage();
       LOG_ALWAYS_FATAL("Elapp_process: no class name or --zygote supplied.");
       return 10;
    }
    return 0;
}
