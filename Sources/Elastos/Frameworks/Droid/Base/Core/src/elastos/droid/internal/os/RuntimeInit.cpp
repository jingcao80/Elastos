
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/os/RuntimeInit.h"
#include "elastos/droid/internal/os/AndroidPrintStream.h"
#include "elastos/droid/internal/os/CZygoteInit.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/server/NetworkManagementSocketTagger.h"
#include <elastos/droid/DroidRuntime.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::Server::NetworkManagementSocketTagger;
using Elastos::Droid::DroidRuntime;
using Elastos::Core::StringBuilder;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::IO::ICloseable;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

ECO_PUBLIC AutoPtr<Elastos::Droid::DroidRuntime> sCurRuntime;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

//=================================================================================
// RuntimeInit::MyTimeZoneGetter
//=================================================================================

ECode RuntimeInit::MyTimeZoneGetter::GetId(
    /* [out] */ String* id)
{
    VALIDATE_NOT_NULL(id);
    return SystemProperties::Get(String("persist.sys.timezone"), id);
}

//=================================================================================
// RuntimeInit::Arguments
//=================================================================================
RuntimeInit::Arguments::Arguments(
    /* [in] */ ArrayOf<String>* args)
{
    ASSERT_SUCCEEDED(ParseArgs(args));
}

ECode RuntimeInit::Arguments::ParseArgs(
    /* [in] */ ArrayOf<String>* args)
{
    assert(args != NULL);

    Int32 curArg = 0;
    for (; curArg < args->GetLength(); curArg++) {
        String arg = (*args)[curArg];

        if (arg.Equals("--")) {
            curArg++;
            break;
        }
        else if (!arg.StartWith("--")) {
            break;
        }
    }

    if (curArg == args->GetLength()) {
        // throw new IllegalArgumentException("Missing classname argument to RuntimeInit!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mModule = (*args)[curArg++];
    mStartClass = (*args)[curArg++];
    mStartArgs = ArrayOf<String>::Alloc(args->GetLength() - curArg);
    mStartArgs->Copy(0, args, curArg, mStartArgs->GetLength());
    return NOERROR;
}

//=================================================================================
// RuntimeInit
//=================================================================================
const String RuntimeInit::TAG("RuntimeInit");
const Boolean RuntimeInit::DEBUG = TRUE;

/** true if commonInit() has been called */
Boolean RuntimeInit::mInitialized = FALSE;

AutoPtr<IBinder> RuntimeInit::mApplicationObject;

volatile Boolean RuntimeInit::mCrashing = FALSE;

void RuntimeInit::nativeZygoteInit()
{
    sCurRuntime->OnZygoteInit();
}

void RuntimeInit::nativeFinishInit()
{
    sCurRuntime->OnStarted();
}

void RuntimeInit::nativeSetExitWithoutCleanup(
    /* [in] */ Boolean exitWithoutCleanup)
{
    sCurRuntime->SetExitWithoutCleanup(exitWithoutCleanup);
}

ECode RuntimeInit::CommonInit()
{
    /* set default handler; this applies to all threads in the VM */
    // Thread::SetDefaultUncaughtExceptionHandler(new UncaughtHandler());

    /*
     * Install a TimezoneGetter subclass for ZoneInfo.db
     */
    AutoPtr<ITimeZoneGetter> tzg = new MyTimeZoneGetter();
    TimeZoneGetter::SetInstance(tzg);

    AutoPtr<ITimeZoneHelper> tzHelper;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzHelper);
    tzHelper->SetDefault(NULL);

    /*
     * Sets handler for java.util.logging to use Android log facilities.
     * The odd "new instance-and-then-throw-away" is a mirror of how
     * the "java.util.logging.config.class" system property works. We
     * can't use the system property here since the logger has almost
     * certainly already been initialized.
     */
    // LogManager.getLogManager().reset();
    // new AndroidConfig();

    /*
     * Sets the default HTTP User-Agent used by HttpURLConnection.
     */
    String userAgent = GetDefaultUserAgent();
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String oldValue;
    system->SetProperty(String("http.agent"), userAgent, &oldValue);

    /*
     * Wire socket tagging to traffic stats.
     */
    NetworkManagementSocketTagger::Install();

    /*
     * If we're running in an emulator launched with "-trace", put the
     * VM into emulator trace profiling mode so that the user can hit
     * F9/F10 at any time to capture traces.  This has performance
     * consequences, so it's not something you want to do always.
     */
    String trace;
    SystemProperties::Get(String("ro.kernel.android.tracing"), &trace);
    if (trace.Equals("1")) {
        Slogger::I(TAG, "NOTE: emulator trace profiling enabled");
        //TODO
        //Debug::EnableEmulatorTraceOutput();
    }

    mInitialized = TRUE;

    return NOERROR;
}

String RuntimeInit::GetDefaultUserAgent()
{
    StringBuilder result(64);
    result += "Dalvik/";

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    String vmVersion;
    system->GetProperty(String("java.vm.version"), &vmVersion);//// such as 1.1.0
    result += vmVersion;
    result += " (Linux; U; Android ";

    String version = Build::VERSION::RELEASE; // "1.0" or "3.4b5"
    result += version.GetLength() > 0 ? version : "1.0";

    // add the model for the release build
    if (Build::VERSION::CODENAME.Equals("REL")) {
        String model = Build::MODEL;
        if (model.GetLength() > 0) {
            result += "; ";
            result += model;
        }
    }
    String id = Build::ID; // "MASTER" or "M4-rc20"
    if (id.GetLength() > 0) {
        result += " Build/";
        result += id;
    }
    result += ")";
    return result.ToString();
}

ECode RuntimeInit::InvokeStaticMain(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [in] */ ArrayOf<String>* argv,
    /* [out] */ IRunnable** task)
{
    VALIDATE_NOT_NULL(task);
    *task = NULL;

    if (DEBUG) {
        Slogger::I(TAG, "InvokeStaticMain: %s, %s", moduleName.string(), className.string());
        if (argv) {
            for (Int32 i = 0; i < argv->GetLength(); ++i) {
                Slogger::I(TAG, "  >> arg %d: %s", i, (*argv)[i].string());
            }
        }
    }

    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IInterface> object;
    AutoPtr<IMethodInfo> methodInfo;

    ECode ec = _CReflector_AcquireModuleInfo(moduleName, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        Slogger::E("RuntimeInit::InvokeStaticMain", "Acquire %s module info failed!\n", moduleName.string());
        return ec;
    }

    ec = moduleInfo->GetClassInfo(className, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        Slogger::E("RuntimeInit::InvokeStaticMain", "Acquire %s, %s class info failed!\n",
            moduleName.string(), className.string());
        return ec;
    }

    ec = classInfo->CreateObject((IInterface**)&object);
    if (FAILED(ec)) {
        Slogger::E("RuntimeInit::InvokeStaticMain: %s, %s", moduleName.string(), className.string());
        return ec;
    }

    ec = classInfo->GetMethodInfo(
            String("Main"), String("([LElastos/String;)E"), (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        Slogger::E("RuntimeInit::InvokeStaticMain", "Acquire  %s, %s \"Main\" method info failed!",
            moduleName.string(), className.string());
        return ec;
    }

    /*
     * This throw gets caught in ZygoteInit.main(), which responds
     * by invoking the exception's run() method. This arrangement
     * clears up all the stack frames that were required in setting
     * up the process.
     */
    *task = new CZygoteInit::MethodAndArgsCaller(object, methodInfo, argv);
    REFCOUNT_ADD(*task);
    return NOERROR;
}

ECode RuntimeInit::ZygoteInit(
    /* [in] */ Int32 targetSdkVersion,
    /* [in] */ ArrayOf<String>* argv,
    /* [out] */ IRunnable** task)
{
    VALIDATE_NOT_NULL(task)

    RedirectLogStreams();

    CommonInit();
    nativeZygoteInit();

    return ApplicationInit(targetSdkVersion, argv, task);
}

ECode RuntimeInit::Main(
    /* [in] */ const ArrayOf<String>& args)
{
    Int32 length = args.GetLength();
    if (length == 2 && args[1].Equals("application")) {
        if (DEBUG) Slogger::D(TAG, "RuntimeInit: Starting application");
        //redirectLogStreams();
    }
    else {
        if (DEBUG) Slogger::D(TAG, "RuntimeInit: Starting tool");
    }

    CommonInit();

    /*
     * Now that we're running in interpreted code, call back into native code
     * to run the system.
     */
    nativeFinishInit();

    return NOERROR;
}

ECode RuntimeInit::WrapperInit(
    /* [in] */ Int32 targetSdkVersion,
    /* [in] */ ArrayOf<String>* argv,
    /* [out] */ IRunnable** task)
{
    return ApplicationInit(targetSdkVersion, argv, task);
}

ECode RuntimeInit::ApplicationInit(
    /* [in] */ Int32 targetSdkVersion,
    /* [in] */ ArrayOf<String>* argv,
    /* [out] */ IRunnable** task)
{
    VALIDATE_NOT_NULL(task)
    *task = NULL;

    // If the application calls System.exit(), terminate the process
    // immediately without running any shutdown hooks.  It is not possible to
    // shutdown an Android application gracefully.  Among other things, the
    // Android runtime shutdown hooks close the Binder driver, which can cause
    // leftover running threads to crash before the process actually exits.
    nativeSetExitWithoutCleanup(TRUE);

    // We want to be fairly aggressive about heap utilization, to avoid
    // holding on to a lot of memory that isn't needed.
//    VMRuntime.getRuntime().setTargetHeapUtilization(0.75f);
//    VMRuntime.getRuntime().setTargetSdkVersion(targetSdkVersion);

    AutoPtr<Arguments> args;
    // try {
    args = new Arguments(argv);
    // } catch (IllegalArgumentException ex) {
    //     Slog.e(TAG, ex.getMessage());
    //     // let the process exit
    //     return;
    // }

    // Remaining arguments are passed to the start class's static main
    return InvokeStaticMain(args->mModule, args->mStartClass, args->mStartArgs, task);
}

void RuntimeInit::RedirectLogStreams()
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    AutoPtr<IPrintStream> out;
    system->GetOut((IPrintStream**)&out);
    ICloseable::Probe(out)->Close();
    AutoPtr<AndroidPrintStream> infoStream = new AndroidPrintStream(Logger::INFO, String("System.out"));
    system->SetOut(infoStream);

    AutoPtr<IPrintStream> err;
    system->GetErr((IPrintStream**)&err);
    ICloseable::Probe(err)->Close();
    AutoPtr<AndroidPrintStream> warmStream = new AndroidPrintStream(Logger::WARN, String("System.err"));
    system->SetErr(warmStream);
}

ECode RuntimeInit::SetApplicationObject(
    /* [in] */ IBinder* app)
{
    mApplicationObject = app;
    return NOERROR;
}

ECode RuntimeInit::GetApplicationObject(
    /* [out] */ IBinder** app)
{
    VALIDATE_NOT_NULL(app)
    *app = mApplicationObject;
    REFCOUNT_ADD(*app)
    return NOERROR;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
