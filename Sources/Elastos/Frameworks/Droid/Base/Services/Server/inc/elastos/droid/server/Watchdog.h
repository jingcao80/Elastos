#ifndef __ELASTOS_DROID_SERVER_WATCHDOG_H__
#define __ELASTOS_DROID_SERVER_WATCHDOG_H__

#include <elastos/droid/ext/frameworkext.h>
#include <_Elastos.Droid.Server.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.IO.h>
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/content/BroadcastReceiver.h>
// #include <elastos/droid/server/CActivityManagerService.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::App::IActivityController;
using Elastos::Core::Thread;
using Elastos::IO::IFile;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {

/** This class calls its monitor every minute. Killing this process if they don't return **/
class Watchdog
    : public Thread
{
public:
    class DropboxThread
        : public Thread
    {
    public:
        DropboxThread(
            /* [in] */ Watchdog* host,
            /* [in] */ const String& subject,
            /* [in] */ IFile* stack);

        // @Override
        CARAPI Run();

    private:
        Watchdog* mHost;
        String mSubject;
        AutoPtr<IFile> mStack;
    };

    /**
     * Used for checking status of handle threads and scheduling monitor callbacks.
     */
    class HandlerChecker
        : public Runnable
    {
    public:

        HandlerChecker(
            /* [in] */ Watchdog* host,
            /* [in] */ IHandler* handler,
            /* [in] */ const String& name,
            /* [in] */ Int64 waitMaxMillis);

        void AddMonitor(
            /* [in] */ IWatchdogMonitor* monitor);

        void ScheduleCheckLocked();

        Boolean IsOverdueLocked();

        Int32 GetCompletionStateLocked();

        AutoPtr<IThread> GetThread();

        String GetName();

        String DescribeBlockedStateLocked();

        // @Override
        CARAPI Run();

    private:
        Watchdog* mHost;
        AutoPtr<IHandler> mHandler;
        String mName;
        Int64 mWaitMax;
        List<AutoPtr<IWatchdogMonitor> > mMonitors;
        Boolean mCompleted;
        AutoPtr<IWatchdogMonitor> mCurrentMonitor;
        Int64 mStartTime;
    };

    class RebootRequestReceiver
        : public BroadcastReceiver
    {
    public:
        RebootRequestReceiver(
            /* [in] */ Watchdog* host);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* c,
            /* [in] */ IIntent* intent);
    private:
        Watchdog* mHost;
    };

public:

    static AutoPtr<Watchdog> GetInstance();

    void Init(
        /* [in] */ IContext* context//,
        /* [in] */ /*CActivityManagerService* activity*/);

    void ProcessStarted(
        /* [in] */ const String& name,
        /* [in] */ Int32 pid);

    void SetActivityController(
        /* [in] */ IActivityController* controller);

    void SetAllowRestart(
        /* [in] */ Boolean allowRestart);

    CARAPI AddMonitor(
        /* [in] */ IWatchdogMonitor* monitor);

    CARAPI AddThread(
        /* [in] */ IHandler* thread);

    CARAPI AddThread(
        /* [in] */ IHandler* thread,
        /* [in] */ Int64 timeoutMillis);

    /**
     * Perform a full reboot of the system.
     */
    void RebootSystem(
        /* [in] */ const String& reason);

    // @Override
    CARAPI Run();

private:
    Watchdog();

    Int32 EvaluateCheckerCompletionLocked();

    AutoPtr< List< AutoPtr<HandlerChecker> > > GetBlockedCheckersLocked();

    String DescribeCheckersLocked(
        /* [in] */ List< AutoPtr<HandlerChecker> >* checkers);

    AutoPtr<IFile> DumpKernelStackTraces();

    void NativeDumpKernelStacks(
        /* [in] */ const String& tracesPath);

public:
    static const String TAG;
    static const Boolean localLOGV;

    // Set this to TRUE to use debug default values.
    static const Boolean DB;

    // Set this to TRUE to have the watchdog record kernel thread stacks when it fires
    static const Boolean RECORD_KERNEL_THREADS;

    static const Int64 DEFAULT_TIMEOUT;
    static const Int64 CHECK_INTERVAL;

    // These are temporally ordered: larger values as lateness increases
    static const Int32 COMPLETED;
    static const Int32 WAITING;
    static const Int32 WAITED_HALF;
    static const Int32 OVERDUE;

    static Object sLock;

    // Which native processes to dump into dropbox's stack traces
    static AutoPtr<ArrayOf<String> > NATIVE_STACKS_OF_INTEREST;

    static AutoPtr<Watchdog> sWatchdog;

    /* This handler will be used to post message back onto the main thread */
    List<AutoPtr<HandlerChecker> > mHandlerCheckers;
    AutoPtr<HandlerChecker> mMonitorChecker;
    AutoPtr<IContentResolver> mResolver;
    // CActivityManagerService* mActivity;

    Int32 mPhonePid;
    AutoPtr<IActivityController> mController;
    Boolean mAllowRestart;

};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_WATCHDOG_H__