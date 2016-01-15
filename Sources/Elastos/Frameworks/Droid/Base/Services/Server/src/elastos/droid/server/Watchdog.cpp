#include <elastos/droid/server/Watchdog.h>
#include <elastos/droid/server/FgThread.h>
#include <elastos/droid/server/UiThread.h>
#include <elastos/droid/server/IoThread.h>
#include <elastos/droid/server/DisplayThread.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/Manifest.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IDebug;
// using Elastos::Droid::Os::CDebug;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IBroadcastReceiver;
using Elastos::Core::StringBuilder;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::IThread;
using Elastos::IO::CFile;
using Elastos::IO::IWriter;
using Elastos::IO::IFileWriter;
using Elastos::IO::CFileWriter;
using Elastos::IO::ICloseable;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {

const String Watchdog::TAG("Watchdog");
const Boolean Watchdog::localLOGV = FALSE;

// Set this to TRUE to use debug default values.
const Boolean Watchdog::DB = FALSE;

// Set this to TRUE to have the watchdog record kernel thread stacks when it fires
const Boolean Watchdog::RECORD_KERNEL_THREADS = TRUE;

const Int64 Watchdog::DEFAULT_TIMEOUT = DB ? 10*1000 : 60*1000;
const Int64 Watchdog::CHECK_INTERVAL = (DB ? 10*1000 : 60*1000) / 2;

// These are temporally ordered: larger values as lateness increases
const Int32 Watchdog::COMPLETED = 0;
const Int32 Watchdog::WAITING = 1;
const Int32 Watchdog::WAITED_HALF = 2;
const Int32 Watchdog::OVERDUE = 3;

Object Watchdog::sLock;

AutoPtr<ArrayOf<String> > InitNATIVE_STACKS_OF_INTEREST()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    array->Set(0, String("/system/bin/mediaserver"));
    array->Set(1, String("/system/bin/sdcard"));
    array->Set(2, String("/system/bin/surfaceflinger"));
    return array;
}

// Which native processes to dump into dropbox's stack traces
static AutoPtr<ArrayOf<String> > NATIVE_STACKS_OF_INTEREST = InitNATIVE_STACKS_OF_INTEREST();

AutoPtr<Watchdog> Watchdog::sWatchdog;

//==========================================================================================
// Watchdog::DropboxThread
//==========================================================================================
Watchdog::DropboxThread::DropboxThread(
    /* [in] */ Watchdog* host,
    /* [in] */ const String& subject,
    /* [in] */ IFile* stack)
    : mHost(host)
    , mSubject(subject)
    , mStack(stack)
{
}

// @Override
ECode Watchdog::DropboxThread::Run()
{
    String nullStr;
    assert(0 && "TODO");
    // mHost->mActivity->AddErrorToDropBox(
    //     String("watchdog"), NULL, String("system_server"), NULL, NULL,
    //     mSubject, nullStr, mStack, NULL);
    return NOERROR;
}

//==========================================================================================
// Watchdog::HandlerChecker
//==========================================================================================

Watchdog::HandlerChecker::HandlerChecker(
    /* [in] */ Watchdog* host,
    /* [in] */ IHandler* handler,
    /* [in] */ const String& name,
    /* [in] */ Int64 waitMaxMillis)
    : mHost(host)
    , mHandler(handler)
    , mName(name)
    , mWaitMax(waitMaxMillis)
    , mCompleted(TRUE)
    , mStartTime(0)
{
}

void Watchdog::HandlerChecker::AddMonitor(
    /* [in] */ IWatchdogMonitor* monitor)
{
    mMonitors.PushBack(monitor);
}

void Watchdog::HandlerChecker::ScheduleCheckLocked()
{
    Boolean idle = FALSE;
    if (mMonitors.GetSize() == 0) {
        AutoPtr<ILooper> looper;
        mHandler->GetLooper((ILooper**)&looper);
        looper->IsIdling(&idle);
    }
    if (mMonitors.GetSize() == 0 && idle) {
        // If the target looper is or just recently was idling, then
        // there is no reason to enqueue our checker on it since that
        // is as good as it not being deadlocked.  This avoid having
        // to do a context switch to check the thread.  Note that we
        // only do this if mCheckReboot is FALSE and we have no
        // monitors, since those would need to be executed at this point.
        mCompleted = TRUE;
        return;
    }

    if (!mCompleted) {
        // we already have a check in flight, so no need
        return;
    }

    mCompleted = FALSE;
    mCurrentMonitor = NULL;
    mStartTime = SystemClock::GetUptimeMillis();
    Boolean bval;
    mHandler->PostAtFrontOfQueue(this, &bval);
}

Boolean Watchdog::HandlerChecker::IsOverdueLocked()
{
    return (!mCompleted) && (SystemClock::GetUptimeMillis() > mStartTime + mWaitMax);
}

Int32 Watchdog::HandlerChecker::GetCompletionStateLocked()
{
    if (mCompleted) {
        return COMPLETED;
    }
    else {
        Int64 latency = SystemClock::GetUptimeMillis() - mStartTime;
        if (latency < mWaitMax/2) {
            return WAITING;
        }
        else if (latency < mWaitMax) {
            return WAITED_HALF;
        }
    }
    return OVERDUE;
}

AutoPtr<IThread> Watchdog::HandlerChecker::GetThread()
{
    AutoPtr<ILooper> looper;
    mHandler->GetLooper((ILooper**)&looper);
    AutoPtr<IThread> thread;
    looper->GetThread((IThread**)&thread);
    return thread;
}

String Watchdog::HandlerChecker::GetName()
{
    return mName;
}

String Watchdog::HandlerChecker::DescribeBlockedStateLocked()
{
    AutoPtr<IThread> thread = GetThread();
    String threadName;
    thread->GetName(&threadName);

    if (mCurrentMonitor == NULL) {
        StringBuilder sb("Blocked in handler on ");
        sb += mName;
        sb += "(";
        sb += threadName;
        sb += ")";

        return sb.ToString();
    }
    else {
        StringBuilder sb("Blocked in monitor ");
        sb += Object::ToString(mCurrentMonitor);
        sb += " on ";
        sb += mName;
        sb += "(";
        sb += threadName;
        sb += ")";

        return sb.ToString();
    }
}

// @Override
ECode Watchdog::HandlerChecker::Run()
{
    Int32 size = mMonitors.GetSize();
    for (Int32 i = 0 ; i < size ; i++) {
        Object& obj = mHost->sLock;
        synchronized(obj) {
            mCurrentMonitor = mMonitors[i];
        }
        mCurrentMonitor->Monitor();
    }

    Object& obj = mHost->sLock;
    synchronized(obj) {
        mCompleted = TRUE;
        mCurrentMonitor = NULL;
    }
    return NOERROR;
}

//==========================================================================================
// Watchdog::RebootRequestReceiver
//==========================================================================================

Watchdog::RebootRequestReceiver::RebootRequestReceiver(
    /* [in] */ Watchdog* host)
    : mHost(host)
{}

ECode Watchdog::RebootRequestReceiver::OnReceive(
    /* [in] */ IContext* c,
    /* [in] */ IIntent* intent)
{
    Int32 ival;
    intent->GetInt32Extra(String("nowait"), 0, &ival);
    if (ival != 0) {
        mHost->RebootSystem(String("Received ACTION_REBOOT broadcast"));
        return NOERROR;
    }
    Slogger::W(TAG, "Unsupported ACTION_REBOOT broadcast: %s", Object::ToString(intent).string());
    return NOERROR;
}

//==========================================================================================
// Watchdog
//==========================================================================================

AutoPtr<Watchdog> Watchdog::GetInstance()
{
    if (sWatchdog == NULL) {
        sWatchdog = new Watchdog();
        sWatchdog->constructor(String("watchdog"));
    }

    return sWatchdog;
}

Watchdog::Watchdog()
    : mPhonePid(0)
    , mAllowRestart(TRUE)
{
    // Initialize handler checkers for each common thread we want to check.  Note
    // that we are not currently checking the background thread, since it can
    // potentially hold longer running operations with no guarantees about the timeliness
    // of operations there.

    // The shared foreground thread is the main checker.  It is where we
    // will also dispatch monitor checks and do other work.

    mMonitorChecker = new HandlerChecker(this, FgThread::GetHandler(),
        String("foreground thread"), DEFAULT_TIMEOUT);
    mHandlerCheckers.PushBack(mMonitorChecker);

    // Add checker for main thread.  We only do a quick check since there
    // can be UI running on the thread.
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    AutoPtr<ILooper> looper;
    helper->GetMainLooper((ILooper**)&looper);
    AutoPtr<IHandler> handler;
    CHandler::New(looper, (IHandler**)&handler);
    AutoPtr<HandlerChecker> checker = new HandlerChecker(this, handler,
        String("main thread"), DEFAULT_TIMEOUT);
    mHandlerCheckers.PushBack(checker);

    // Add checker for shared UI thread.
    checker = new HandlerChecker(this, UiThread::GetHandler(),
        String("ui thread"), DEFAULT_TIMEOUT);
    mHandlerCheckers.PushBack(checker);

    // And also check IO thread.
    checker = new HandlerChecker(this, IoThread::GetHandler(),
        String("i/o thread"), DEFAULT_TIMEOUT);
    mHandlerCheckers.PushBack(checker);

    // And the display thread.
    checker = new HandlerChecker(this, DisplayThread::GetHandler(),
        String("display thread"), DEFAULT_TIMEOUT);
    mHandlerCheckers.PushBack(checker);
}

void Watchdog::Init(
    /* [in] */ IContext* context//,
    /* [in] */ /*CActivityManagerService* activity*/)
{
    context->GetContentResolver((IContentResolver**)&mResolver);
    assert(0 && "TODO");
    // mActivity = activity;

    AutoPtr<IBroadcastReceiver> receiver = (IBroadcastReceiver*)new RebootRequestReceiver(this);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_REBOOT, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    context->RegisterReceiver(receiver, filter,
        Manifest::permission::REBOOT, NULL, (IIntent**)&intent);
}

void Watchdog::ProcessStarted(
    /* [in] */ const String& name,
    /* [in] */ Int32 pid)
{
    synchronized(this) {
        if (name.Equals("com.android.phone")) {
            mPhonePid = pid;
        }
    }
}

void Watchdog::SetActivityController(
    /* [in] */ IActivityController* controller)
{
    synchronized(this) {
        mController = controller;
    }
}

void Watchdog::SetAllowRestart(
    /* [in] */ Boolean allowRestart)
{
    synchronized(this) {
        mAllowRestart = allowRestart;
    }
}

ECode Watchdog::AddMonitor(
    /* [in] */ IWatchdogMonitor* monitor)
{
    synchronized(this) {
        Boolean bval;
        IsAlive(&bval);
        if (bval) {
            Slogger::E(TAG, "Monitors can't be added once the Watchdog is running");
            return E_RUNTIME_EXCEPTION;
        }
        mMonitorChecker->AddMonitor(monitor);
    }
    return NOERROR;
}

ECode Watchdog::AddThread(
    /* [in] */ IHandler* thread)
{
    return AddThread(thread, DEFAULT_TIMEOUT);
}

ECode Watchdog::AddThread(
    /* [in] */ IHandler* thread,
    /* [in] */ Int64 timeoutMillis)
{
    synchronized(this) {
        Boolean bval;
        IsAlive(&bval);
        if (bval) {
            Slogger::E(TAG, "Threads can't be added once the Watchdog is running");
            return E_RUNTIME_EXCEPTION;
        }

        AutoPtr<ILooper> looper;
        thread->GetLooper((ILooper**)&looper);
        AutoPtr<IThread> t;
        looper->GetThread((IThread**)&t);
        String name;
        t->GetName(&name);
        AutoPtr<HandlerChecker> checker = new HandlerChecker(this, thread, name, timeoutMillis);
        mHandlerCheckers.PushBack(checker);
    }
    return NOERROR;
}

void Watchdog::RebootSystem(
    /* [in] */ const String& reason)
{
    Slogger::I(TAG, "Rebooting system because: %s", reason.string());
    AutoPtr<IServiceManager> mgr;
    CServiceManager::AcquireSingleton((IServiceManager**)&mgr);
    AutoPtr<IInterface> obj;
    mgr->GetService(IContext::POWER_SERVICE, (IInterface**)&obj);
    IIPowerManager* pms = IIPowerManager::Probe(obj);
    if (pms) {
        pms->Reboot(FALSE, reason, FALSE);
    }
}

Int32 Watchdog::EvaluateCheckerCompletionLocked()
{
    Int32 state = COMPLETED;
    HandlerChecker* hc;
    for (Int32 i = 0; i<mHandlerCheckers.GetSize(); i++) {
        hc = mHandlerCheckers[i];
        state = Elastos::Core::Math::Max(state, hc->GetCompletionStateLocked());
    }
    return state;
}

AutoPtr< List< AutoPtr<Watchdog::HandlerChecker> > > Watchdog::GetBlockedCheckersLocked()
{
    AutoPtr< List< AutoPtr<HandlerChecker> > > checkers = new List< AutoPtr<HandlerChecker> >();
    for (Int32 i = 0; i < mHandlerCheckers.GetSize(); i++) {
        AutoPtr<HandlerChecker> hc = mHandlerCheckers[i];
        if (hc->IsOverdueLocked()) {
            checkers->PushBack(hc);
        }
    }
    return checkers;
}

String Watchdog::DescribeCheckersLocked(
    /* [in] */ List< AutoPtr<HandlerChecker> >* checkers)
{
    StringBuilder builder(128);
    Boolean isFirst = TRUE;
    for (Int32 i = 0; i < checkers->GetSize(); i++) {
        if (!isFirst) {
            builder.Append(", ");
        }
        builder.Append((*checkers)[i]->DescribeBlockedStateLocked());

        isFirst = FALSE;
    }
    return builder.ToString();
}

ECode Watchdog::Run()
{
    AutoPtr<IDebug> debug;
    // CDebug::AcquireSingleton((IDebug**)&debug);
    Boolean waitedHalf = FALSE;
    Boolean bval;
    ECode ec = NOERROR;
    while (TRUE) {
        AutoPtr< List< AutoPtr<HandlerChecker> > > blockedCheckers;
        String subject;
        Boolean allowRestart;
        Int32 debuggerWasConnected = 0;

        synchronized(this) {
            Int64 timeout = CHECK_INTERVAL;
            // Make sure we (re)spin the checkers that have become idle within
            // this wait-and-check interval
            for (Int32 i = 0; i < mHandlerCheckers.GetSize(); i++) {
                mHandlerCheckers[i]->ScheduleCheckLocked();
            }

            if (debuggerWasConnected > 0) {
                debuggerWasConnected--;
            }

            // NOTE: We use uptimeMillis() here because we do not want to increment the time we
            // wait while asleep. If the device is asleep then the thing that we are waiting
            // to timeout on is asleep as well and won't have a chance to run, causing a FALSE
            // positive on when to kill things.
            Int64 start = SystemClock::GetUptimeMillis();

            Boolean bval;
            while (timeout > 0) {
                debug->IsDebuggerConnected(&bval);
                if (bval) {
                    debuggerWasConnected = 2;
                }
                // try {
                ec = Wait(timeout);
                if (ec == (ECode)E_INTERRUPTED_EXCEPTION) {
                    Slogger::E(TAG, "InterruptedException: Wait %d", timeout);
                }

                debug->IsDebuggerConnected(&bval);
                if (bval) {
                    debuggerWasConnected = 2;
                }
                timeout = CHECK_INTERVAL - (SystemClock::GetUptimeMillis() - start);
            }

            Int32 waitState = EvaluateCheckerCompletionLocked();
            if (waitState == COMPLETED) {
                // The monitors have returned; reset
                waitedHalf = FALSE;
                continue;
            }
            else if (waitState == WAITING) {
                // still waiting but within their configured intervals; back off and recheck
                continue;
            }
            else if (waitState == WAITED_HALF) {
                if (!waitedHalf) {
                    // We've waited half the deadlock-detection interval.  Pull a stack
                    // trace and wait another half.
                    assert(0 && "TODO");
                    // ArrayList<Integer> pids = new ArrayList<Integer>();
                    // pids.add(Process::myPid());
                    // CActivityManagerService::DumpStackTraces(TRUE, pids, NULL, NULL,
                    //         NATIVE_STACKS_OF_INTEREST);
                    waitedHalf = TRUE;
                }
                continue;
            }

            // something is overdue!
            blockedCheckers = GetBlockedCheckersLocked();
            subject = DescribeCheckersLocked(blockedCheckers);
            allowRestart = mAllowRestart;
        }

        // If we got here, that means that the system is most likely hung.
        // First collect stack traces from all threads of the system process.
        // Then kill this process so that the system will restart.
        // EventLog.writeEvent(EventLogTags.WATCHDOG, subject);

        assert(0 && "TODO");
        // ArrayList<Integer> pids = new ArrayList<Integer>();
        // pids.add(Process.myPid());
        // if (mPhonePid > 0) pids.add(mPhonePid);
        // // Pass !waitedHalf so that just in case we somehow wind up here without having
        // // dumped the halfway stacks, we properly re-initialize the trace file.
        AutoPtr<IFile> stack;
        // final File stack = ActivityManagerService.dumpStackTraces(
        //         !waitedHalf, pids, NULL, NULL, NATIVE_STACKS_OF_INTEREST);

        // Give some extra time to make sure the stack traces get written.
        // The system's been hanging for a minute, another second or two won't hurt much.
        SystemClock::Sleep(2000);

        // Pull our own kernel thread stacks as well if we're configured for that
        if (RECORD_KERNEL_THREADS) {
            DumpKernelStackTraces();
        }

        // Trigger the kernel to dump all blocked threads to the kernel log
        AutoPtr<IWriter> sysrq_trigger;
        CFileWriter::New(String("/proc/sysrq-trigger"), (IWriter**)&sysrq_trigger);
        ec = sysrq_trigger->Write(String("w"));
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Failed to write to /proc/sysrq-trigger");
        }
        ICloseable::Probe(sysrq_trigger)->Close();

        // Try to add the error to the dropbox, but assuming that the ActivityManager
        // itself may be deadlocked.  (which has happened, causing this statement to
        // deadlock and the watchdog as a whole to be ineffective)
        AutoPtr<DropboxThread> dropboxThread = new DropboxThread(this, subject, stack);
        dropboxThread->constructor(String("watchdogWriteToDropbox"));
        dropboxThread->Start();
        dropboxThread->Join(2000);  // wait up to 2 seconds for it to return.

        AutoPtr<IActivityController> controller;
        synchronized(this) {
            controller = mController;
        }
        if (controller != NULL) {
            Slogger::I(TAG, "Reporting stuck state to activity controller");

            // Binder.setDumpDisabled("Service dumps disabled due to hung system process.");
            // 1 = keep waiting, -1 = kill system
            Int32 res;
            controller->SystemNotResponding(subject, &res);
            if (res >= 0) {
                Slogger::I(TAG, "Activity controller requested to coninue to wait");
                waitedHalf = FALSE;
                continue;
            }
        }

        // Only kill the process if the debugger is not attached.
        debug->IsDebuggerConnected(&bval);
        if (bval) {
            debuggerWasConnected = 2;
        }
        if (debuggerWasConnected >= 2) {
            Slogger::W(TAG, "Debugger connected: Watchdog is *not* killing the system process");
        }
        else if (debuggerWasConnected > 0) {
            Slogger::W(TAG, "Debugger was connected: Watchdog is *not* killing the system process");
        }
        else if (!allowRestart) {
            Slogger::W(TAG, "Restart not allowed: Watchdog is *not* killing the system process");
        }
        else {
            Slogger::W(TAG, "*** WATCHDOG KILLING SYSTEM PROCESS: %s", subject.string());
            HandlerChecker* hc;
            for (Int32 i = 0; i < blockedCheckers->GetSize(); i++) {
                hc = (*blockedCheckers)[i];
                Slogger::W(TAG, "%s stack trace:", hc->GetName().string());
                assert(0 && "TODO");
                // StackTraceElement[] stackTrace
                //         = blockedCheckers->Get(i)->GetThread()->GetStackTrace();
                // for (StackTraceElement element: stackTrace) {
                //     Slogger::W(TAG, "    at " + element);
                // }
            }
            Slogger::W(TAG, "*** GOODBYE!");
            Process::KillProcess(Process::MyPid());

            AutoPtr<ISystem> system;
            CSystem::AcquireSingleton((ISystem**)&system);
            system->Exit(10);
        }

        waitedHalf = FALSE;
    }
}

AutoPtr<IFile> Watchdog::DumpKernelStackTraces()
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String tracesPath;
    sysProp->Get(String("dalvik.vm.stack-trace-file"), String(NULL), &tracesPath);
    if (tracesPath.IsNullOrEmpty()) {
        return NULL;
    }

    NativeDumpKernelStacks(tracesPath);
    AutoPtr<IFile> file;
    CFile::New(tracesPath, (IFile**)&file);
    return file;
}

//===============================================================================
// native
//===============================================================================
#include <utils/Log.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

static void dumpOneStack(int tid, int outFd)
{
    char buf[64];

    snprintf(buf, sizeof(buf), "/proc/%d/stack", tid);
    int stackFd = open(buf, O_RDONLY);
    if (stackFd >= 0) {
        // header for readability
        strncat(buf, ":\n", sizeof(buf) - strlen(buf) - 1);
        write(outFd, buf, strlen(buf));

        // copy the stack dump text
        int nBytes;
        while ((nBytes = read(stackFd, buf, sizeof(buf))) > 0) {
            write(outFd, buf, nBytes);
        }

        // footer and done
        write(outFd, "\n", 1);
        close(stackFd);
    } else {
        ALOGE("Unable to open stack of tid %d : %d (%s)", tid, errno, strerror(errno));
    }
}

void Watchdog::NativeDumpKernelStacks(
    /* [in] */ const String& tracesPath)
{
    char buf[128];
    DIR* taskdir;

    ALOGI("dumpKernelStacks");

    const char *path = tracesPath.string();

    int outFd = open(path, O_WRONLY | O_APPEND | O_CREAT,
        S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH);
    if (outFd < 0) {
        ALOGE("Unable to open stack dump file: %d (%s)", errno, strerror(errno));
        return;
    }

    snprintf(buf, sizeof(buf), "\n----- begin pid %d kernel stacks -----\n", getpid());
    write(outFd, buf, strlen(buf));

    // look up the list of all threads in this process
    snprintf(buf, sizeof(buf), "/proc/%d/task", getpid());
    taskdir = opendir(buf);
    if (taskdir != NULL) {
        struct dirent * ent;
        while ((ent = readdir(taskdir)) != NULL) {
            int tid = atoi(ent->d_name);
            if (tid > 0 && tid <= 65535) {
                // dump each stack trace
                dumpOneStack(tid, outFd);
            }
        }
        closedir(taskdir);
    }

    snprintf(buf, sizeof(buf), "----- end pid %d kernel stacks -----\n", getpid());
    write(outFd, buf, strlen(buf));

    close(outFd);
}


} // namespace Server
} // namepsace Droid
} // namespace Elastos
