
#ifndef __ELASTOS_DROID_INTERNAL_OS_RUNTIMEINIT_H__
#define __ELASTOS_DROID_INTERNAL_OS_RUNTIMEINIT_H__

#include "Elastos.Droid.Core.h"
#include <Elastos.Droid.Os.h>
#include <elastos/core/Singleton.h>
#include <elastos/utility/TimeZoneGetter.h>

using Elastos::Droid::Os::IBinder;
using Elastos::Core::Singleton;
using Elastos::Core::IRunnable;
using Elastos::Utility::TimeZoneGetter;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

class RuntimeInit
    : public Singleton
{
private:
    class MyTimeZoneGetter
        : public TimeZoneGetter
    {
    public:
        CARAPI GetId(
                /* [out] */ String* string);
    };

public:
    /**
     * Handles argument parsing for args related to the runtime.
     *
     * Current recognized args:
     * <ul>
     *   <li> <code> [--] &lt;start class name&gt;  &lt;args&gt;
     * </ul>
     */
    class Arguments : public Object
    {
    public:
        /**
         * Constructs instance and parses args
         * @param args runtime command-line args
         * @throws IllegalArgumentException
         */
        Arguments(
            /* [in] */ const ArrayOf<String>& args);

    private:
        /**
         * Parses the commandline arguments intended for the Runtime.
         */
        CARAPI ParseArgs(
            /* [in] */ const ArrayOf<String>& args);

    public:
        /** first non-option argument */
        String mStartClass;

        /** all following arguments */
        AutoPtr< ArrayOf<String> > mStartArgs;

        String mModule;
    };

public:
    /**
     * The main function called when started through the zygote process. This
     * could be unified with main(), if the native code in nativeFinishInit()
     * were rationalized with Zygote startup.<p>
     *
     * Current recognized args:
     * <ul>
     *   <li> <code> [--] &lt;start class name&gt;  &lt;args&gt;
     * </ul>
     *
     * @param targetSdkVersion target SDK version
     * @param argv arg strings
     */
    static CARAPI ZygoteInit(
        /* [in] */ Int32 targetSdkVersion,
        /* [in] */ ArrayOf<String>* argv,
        /* [out] */ IRunnable** task);

    /**
     * The main function called when an application is started through a
     * wrapper process.
     *
     * When the wrapper starts, the runtime starts {@link RuntimeInit#main}
     * which calls {@link WrapperInit#main} which then calls this method.
     * So we don't need to call commonInit() here.
     *
     * @param targetSdkVersion target SDK version
     * @param argv arg strings
     */
    static CARAPI WrapperInit(
        /* [in] */ Int32 targetSdkVersion,
        /* [in] */ ArrayOf<String>* argv,
        /* [out] */ IRunnable** task);

    static CARAPI Main(
        /* [in] */ const ArrayOf<String>& args);

    /**
     * Redirect System.out and System.err to the Android log.
     */
    static CARAPI_(void) RedirectLogStreams();

private:

    static CARAPI_(void) nativeZygoteInit();

    static CARAPI_(void) nativeFinishInit();

    static CARAPI_(void) nativeSetExitWithoutCleanup(
        /* [in] */ Boolean exitWithoutCleanup);

    static CARAPI CommonInit();

    /**
     * Returns an HTTP user agent of the form
     * "Dalvik/1.1.0 (Linux; U; Android Eclair Build/MASTER)".
     */
    static CARAPI_(String) GetDefaultUserAgent();

    /**
     * Invokes a static "main(argv[]) method on class "className".
     * Converts various failing exceptions into RuntimeExceptions, with
     * the assumption that they will then cause the VM instance to exit.
     *
     * @param className Fully-qualified class name
     * @param argv Argument vector for main()
     */
    static CARAPI InvokeStaticMain(
        /* [in] */ const String& moduleName,
        /* [in] */ const String& className,
        /* [in] */ ArrayOf<String>* argv,
        /* [out] */ IRunnable** task);

    static CARAPI ApplicationInit(
        /* [in] */ Int32 targetSdkVersion,
        /* [in] */ ArrayOf<String>* argv,
        /* [out] */ IRunnable** task);

private:
    static const String TAG;
    static const Boolean DEBUG;

    /** true if commonInit() has been called */
    static Boolean mInitialized;

    static AutoPtr<IBinder> mApplicationObject;

    static volatile Boolean mCrashing;

    // /**
    //  * Use this to log a message when a thread exits due to an uncaught
    //  * exception.  The framework catches these for the main threads, so
    //  * this should only matter for threads created by applications.
    //  */
    // private static class UncaughtHandler implements Thread.UncaughtExceptionHandler {
    //     public void uncaughtException(Thread t, Throwable e) {
    //         try {
    //             // Don't re-enter -- avoid infinite loops if crash-reporting crashes.
    //             if (mCrashing) return;
    //             mCrashing = true;

    //             if (mApplicationObject == null) {
    //                 Clog_e(TAG, "*** FATAL EXCEPTION IN SYSTEM PROCESS: " + t.getName(), e);
    //             } else {
    //                 StringBuilder message = new StringBuilder();
    //                 message.append("FATAL EXCEPTION: ").append(t.getName()).append("\n");
    //                 final String processName = ActivityThread.currentProcessName();
    //                 if (processName != null) {
    //                     message.append("Process: ").append(processName).append(", ");
    //                 }
    //                 message.append("PID: ").append(Process.myPid());
    //                 Clog_e(TAG, message.toString(), e);
    //             }

    //             // Bring up crash dialog, wait for it to be dismissed
    //             ActivityManagerNative.getDefault().handleApplicationCrash(
    //                     mApplicationObject, new ApplicationErrorReport.CrashInfo(e));
    //         } catch (Throwable t2) {
    //             try {
    //                 Clog_e(TAG, "Error reporting crash", t2);
    //             } catch (Throwable t3) {
    //                 // Even Clog_e() fails!  Oh well.
    //             }
    //         } finally {
    //             // Try everything to make sure this process goes away.
    //             Process.killProcess(Process.myPid());
    //             System.exit(10);
    //         }
    //     }
    // }
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_RUNTIMEINIT_H__
