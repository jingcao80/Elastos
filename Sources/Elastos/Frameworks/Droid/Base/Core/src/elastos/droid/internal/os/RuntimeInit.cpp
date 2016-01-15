
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/os/RuntimeInit.h"
#include "elastos/droid/internal/os/AndroidPrintStream.h"
#include "elastos/droid/internal/os/CZygoteInit.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/logging/Slogger.h>
#include <DroidRuntime.h>

using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Logging::Slogger;

ECO_PUBLIC AutoPtr<Elastos::Droid::DroidRuntime> sCurRuntime;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

RuntimeInit::Arguments::Arguments(
    /* [in] */ const ArrayOf<String>& args)
{
    ASSERT_SUCCEEDED(ParseArgs(args));
}

ECode RuntimeInit::Arguments::ParseArgs(
    /* [in] */ const ArrayOf<String>& args)
{
    Int32 curArg = 0;
    for (; curArg < args.GetLength(); curArg++) {
        String arg = args[curArg];

        if (arg.Equals("--")) {
            curArg++;
            break;
        }
        else if (!arg.StartWith("--")) {
            break;
        }
    }

    if (curArg == args.GetLength()) {
        // throw new IllegalArgumentException("Missing classname argument to RuntimeInit!");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mModule = args[curArg++];
    mStartClass = args[curArg++];
    mStartArgs = ArrayOf<String>::Alloc(args.GetLength() - curArg);
    for (Int32 i = 0; i < mStartArgs->GetLength(); ++i) {
        (*mStartArgs)[i] = args[curArg + i];
    }
    return NOERROR;
}

const String RuntimeInit::TAG("RuntimeInit");
const Boolean RuntimeInit::DEBUG = FALSE;;

ECode RuntimeInit::InvokeStaticMain(
    /* [in] */ const String& moduleName,
    /* [in] */ const String& className,
    /* [in] */ ArrayOf<String>* argv,
    /* [out] */ IRunnable** task)
{
    VALIDATE_NOT_NULL(task);
    *task = NULL;

    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    AutoPtr<IInterface> object;
    AutoPtr<IMethodInfo> methodInfo;

    ECode ec = _CReflector_AcquireModuleInfo(
            moduleName, (IModuleInfo**)&moduleInfo);
    if (FAILED(ec)) {
        Slogger::E("RuntimeInit::InvokeStaticMain", "Acquire \"%s\" module info failed!\n", moduleName.string());
        return ec;
    }

    ec = moduleInfo->GetClassInfo(
            className, (IClassInfo**)&classInfo);
    if (FAILED(ec)) {
        Slogger::E("RuntimeInit::InvokeStaticMain", "Acquire \"%s\" class info failed!\n", className.string());
        return ec;
    }

    ec = classInfo->CreateObject((IInterface**)&object);
    if (FAILED(ec)) {
        Slogger::E("RuntimeInit::InvokeStaticMain", "Create object failed!\n");
        return ec;
    }

    ec = classInfo->GetMethodInfo(
            String("Main"), String("[LElastos/String;)E"), (IMethodInfo**)&methodInfo);
    if (FAILED(ec)) {
        Slogger::E("RuntimeInit::InvokeStaticMain", "Acquire \"Main\" method info failed!\n");
        return ec;
    }

    *task = new CZygoteInit::MethodAndArgsCaller(object, methodInfo, argv);
    REFCOUNT_ADD(*task);
    return NOERROR;
}

ECode RuntimeInit::ZygoteInit(
    /* [in] */ Int32 targetSdkVersion,
    /* [in] */ ArrayOf<String>* argv,
    /* [out] */ IRunnable** task)
{
    if (DEBUG) Slogger::D(TAG, "RuntimeInit: Starting application from zygote");

    // redirectLogStreams();

    // commonInit();
    // nativeZygoteInit();

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

    //commonInit();

    /*
     * Now that we're running in interpreted code, call back into native code
     * to run the system.
     */
    sCurRuntime->OnStarted();

    if (DEBUG) Slogger::D(TAG, "Leaving RuntimeInit!");
    return NOERROR;
}

ECode RuntimeInit::ApplicationInit(
    /* [in] */ Int32 targetSdkVersion,
    /* [in] */ ArrayOf<String>* argv,
    /* [out] */ IRunnable** task)
{
    // If the application calls System.exit(), terminate the process
    // immediately without running any shutdown hooks.  It is not possible to
    // shutdown an Android application gracefully.  Among other things, the
    // Android runtime shutdown hooks close the Binder driver, which can cause
    // leftover running threads to crash before the process actually exits.
//    nativeSetExitWithoutCleanup(true);

    // We want to be fairly aggressive about heap utilization, to avoid
    // holding on to a lot of memory that isn't needed.
//    VMRuntime.getRuntime().setTargetHeapUtilization(0.75f);
//    VMRuntime.getRuntime().setTargetSdkVersion(targetSdkVersion);

    AutoPtr<Arguments> args;
    // try {
    args = new Arguments(*argv);
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
} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
