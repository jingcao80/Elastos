//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <elastos/droid/Manifest.h>
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/os/Process.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/server/DisplayThread.h>
#include <elastos/droid/server/FgThread.h>
#include <elastos/droid/server/IoThread.h>
#include <elastos/droid/server/UiThread.h>
#include <elastos/droid/server/Watchdog.h>
#include <elastos/utility/logging/Slogger.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <Elastos.CoreLibrary.Text.h>
#include <utils/Log.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Manifest;
using Elastos::Droid::Os::Binder;
// using Elastos::Droid::Os::CDebug;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::SystemClock;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IDateFormat;
using Elastos::Text::ISimpleDateFormat;
using Elastos::IO::ICloseable;
using Elastos::IO::CFile;
using Elastos::IO::CFileWriter;
using Elastos::IO::IFileWriter;
using Elastos::IO::IWriter;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Core::IThread;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {

AutoPtr<ArrayOf<String> > InitNATIVE_STACKS_OF_INTEREST()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(3);
    array->Set(0, String("/system/bin/mediaserver"));
    array->Set(1, String("/system/bin/sdcard"));
    array->Set(2, String("/system/bin/surfaceflinger"));
    return array;
}

const String Watchdog::TAG("Watchdog");
const Boolean Watchdog::localLOGV = FALSE;

// Set this to TRUE to use debug default values.
const Boolean Watchdog::DB = FALSE;

// Set this to TRUE to have the watchdog record kernel thread stacks when it fires
const Boolean Watchdog::RECORD_KERNEL_THREADS = TRUE;

const Int64 Watchdog::DEFAULT_TIMEOUT = Watchdog::DB ? 10*1000 : 60*1000;
const Int64 Watchdog::CHECK_INTERVAL = DEFAULT_TIMEOUT / 2;

// These are temporally ordered: larger values as lateness increases
const Int32 Watchdog::COMPLETED = 0;
const Int32 Watchdog::WAITING = 1;
const Int32 Watchdog::WAITED_HALF = 2;
const Int32 Watchdog::OVERDUE = 3;

// Which native processes to dump into dropbox's stack traces
const AutoPtr<ArrayOf<String> > Watchdog::NATIVE_STACKS_OF_INTEREST = InitNATIVE_STACKS_OF_INTEREST();

AutoPtr<Watchdog> Watchdog::sWatchdog;


//============================================================================
// Watchdog::DropboxThread
//============================================================================
Watchdog::DropboxThread::DropboxThread(
    /* [in] */ const String& subject,
    /* [in] */ IFile* stack,
    /* [in] */ Watchdog* dog)
    : mSubject(subject)
    , mStack(stack)
    , mHost(dog)
{
    Thread::constructor(String("DropboxThread"));
}

// @Override
ECode Watchdog::DropboxThread::Run()
{
    return mHost->mActivity->AddErrorToDropBox(
        String("watchdog"), NULL, String("system_server"), NULL, NULL,
        mSubject, String(NULL), mStack, NULL);
}

ECode Watchdog::DropboxThread::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "Watchdog::DropboxThread";
    return NOERROR;
}

//============================================================================
// Watchdog::HandlerChecker
//============================================================================

Watchdog::HandlerChecker::HandlerChecker(
    /* [in] */ IHandler* handler,
    /* [in] */ const String& name,
    /* [in] */ Int64 waitMaxMillis,
    /* [in] */ Watchdog* host)
    : mHandler(handler)
    , mName(name)
    , mWaitMax(waitMaxMillis)
    , mCompleted(TRUE)
    , mStartTime(0)
    , mHost(host)
{
}

ECode Watchdog::HandlerChecker::AddMonitor(
    /* [in] */ IWatchdogMonitor* monitor)
{
    AutoPtr<IWatchdogMonitor> m(monitor);
    mMonitors.PushBack(m);
    return NOERROR;
}

ECode Watchdog::HandlerChecker::ScheduleCheckLocked()
{
    Boolean bval;
    if (mMonitors.IsEmpty()) {
        AutoPtr<ILooper> looper;
        mHandler->GetLooper((ILooper**)&looper);
        looper->IsIdling(&bval);
        if (bval) {
            // If the target looper is or just recently was idling, then
            // there is no reason to enqueue our checker on it since that
            // is as good as it not being deadlocked.  This avoid having
            // to do a context switch to check the thread.  Note that we
            // only do this if mCheckReboot is FALSE and we have no
            // monitors, since those would need to be executed at this point.
            mCompleted = TRUE;
            return NOERROR;
        }
    }

    if (!mCompleted) {
        // we already have a check in flight, so no need
        return NOERROR;
    }

    mCompleted = FALSE;
    mCurrentMonitor = NULL;
    mStartTime = SystemClock::GetUptimeMillis();
    return mHandler->PostAtFrontOfQueue(this, &bval);
}

Boolean Watchdog::HandlerChecker::IsOverdueLocked()
{
    return (!mCompleted) && (SystemClock::GetUptimeMillis() > mStartTime + mWaitMax);
}

Int32 Watchdog::HandlerChecker::GetCompletionStateLocked()
{
    if (mCompleted) {
        return Watchdog::COMPLETED;
    }
    else {
        Int64 latency = SystemClock::GetUptimeMillis() - mStartTime;
        if (latency < mWaitMax/2) {
            return Watchdog::WAITING;
        }
        else if (latency < mWaitMax) {
            return Watchdog::WAITED_HALF;
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
    String tn;
    thread->GetName(&tn);
    StringBuilder sb;
    if (mCurrentMonitor == NULL) {
        sb += "Blocked in handler on ";
        sb += mName;
        sb += "(";
        sb += tn;
        sb += ")";
    }
    else {
        sb += "Blocked in monitor ";
        sb += Object::ToString(mCurrentMonitor);
        sb += " on ";
        sb += mName;
        sb += "(";
        sb += tn;
        sb += ")";
    }
    return sb.ToString();
}

// @Override
ECode Watchdog::HandlerChecker::Run()
{
    List<AutoPtr<IWatchdogMonitor> >::Iterator it;
    for (it = mMonitors.Begin(); it != mMonitors.End(); ++it) {
        {
            AutoLock lock(mHost);
            mCurrentMonitor = *it;
        }
        mCurrentMonitor->Monitor();
    }

    {
        AutoLock lock(mHost);
        mCompleted = TRUE;
        mCurrentMonitor = NULL;
    }
    return NOERROR;
}

//============================================================================
// Watchdog::RebootRequestReceiver
//============================================================================

Watchdog::RebootRequestReceiver::RebootRequestReceiver(
    /* [in] */ Watchdog* dog)
    : mHost(dog)
{
}

// @Override
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

    Slogger::W("Watchdog::RebootRequestReceiver", "Unsupported ACTION_REBOOT broadcast: %s",
        TO_CSTR(intent));
    return NOERROR;
}

ECode Watchdog::RebootRequestReceiver::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = "Watchdog::RebootRequestReceiver";
    return NOERROR;
}

//============================================================================
// Watchdog
//============================================================================
AutoPtr<Watchdog> Watchdog::GetInstance()
{
    if (sWatchdog == NULL) {
        sWatchdog = new Watchdog();
    }

    return sWatchdog;
}

Watchdog::Watchdog()
    : mPhonePid(0)
    , mAllowRestart(TRUE)
{
    Thread::constructor(String("watchdog"));

    // Initialize handler checkers for each common thread we want to check.  Note
    // that we are not currently checking the background thread, since it can
    // potentially hold longer running operations with no guarantees about the timeliness
    // of operations there.

    // The shared foreground thread is the main checker.  It is where we
    // will also dispatch monitor checks and do other work.
    mMonitorChecker = new HandlerChecker(FgThread::GetHandler(),
        String("foreground thread"), DEFAULT_TIMEOUT, this);
    mHandlerCheckers.PushBack(mMonitorChecker);

    // Add checker for main thread.  We only do a quick check since there
    // can be UI running on the thread.
    AutoPtr<ILooper> ml;
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    helper->GetMainLooper((ILooper**)&ml);
    AutoPtr<IHandler> handle;
    CHandler::New(ml, (IHandler**)&handle);

    mHandlerCheckers.PushBack(new HandlerChecker(handle,
        String("main thread"), DEFAULT_TIMEOUT, this));

    // Add checker for shared UI thread.
    mHandlerCheckers.PushBack(new HandlerChecker(UiThread::GetHandler(),
        String("ui thread"), DEFAULT_TIMEOUT, this));

    // And also check IO thread.
    mHandlerCheckers.PushBack(new HandlerChecker(IoThread::GetHandler(),
        String("i/o thread"), DEFAULT_TIMEOUT, this));

    // And the display thread.
    mHandlerCheckers.PushBack(new HandlerChecker(DisplayThread::GetHandler(),
        String("display thread"), DEFAULT_TIMEOUT, this));

    CSimpleDateFormat::New(String("dd_MMM_HH_mm_ss.SSS"), (ISimpleDateFormat**)&mTraceDateFormat);
}

ECode Watchdog::Init(
    /* [in] */ IContext* context,
    /* [in] */ CActivityManagerService* activity)
{
    context->GetContentResolver((IContentResolver**)&mResolver);

    mActivity = activity;

    AutoPtr<IBroadcastReceiver> receiver = new RebootRequestReceiver(this);
    AutoPtr<IIntentFilter> filter;
    CIntentFilter::New(IIntent::ACTION_REBOOT, (IIntentFilter**)&filter);
    AutoPtr<IIntent> intent;
    return context->RegisterReceiver(receiver, filter, Manifest::permission::REBOOT, NULL, (IIntent**)&intent);
}

ECode Watchdog::ProcessStarted(
    /* [in] */ const String& name,
    /* [in] */ Int32 pid)
{
    Slogger::D(TAG, "ProcessStarted: %s, pid: %d", name.string(), pid);
    {    AutoLock syncLock(this);
        // assert(0 && "TODO check namespace");
        if (name.Equals("com.android.phone")) {
            mPhonePid = pid;
        }
    }
    return NOERROR;
}

ECode Watchdog::SetActivityController(
    /* [in] */ IIActivityController* controller)
{
    {    AutoLock syncLock(this);
        mController = controller;
    }
    return NOERROR;
}

ECode Watchdog::SetAllowRestart(
    /* [in] */ Boolean allowRestart)
{
    {    AutoLock syncLock(this);
        mAllowRestart = allowRestart;
    }
    return NOERROR;
}

ECode Watchdog::AddMonitor(
    /* [in] */ IWatchdogMonitor* monitor)
{
    {    AutoLock syncLock(this);
        Boolean bval;
        if (IsAlive(&bval), bval) {
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
    {    AutoLock syncLock(this);
        Boolean bval;
        if (IsAlive(&bval), bval) {
            Slogger::E(TAG, "Monitors can't be added once the Watchdog is running");
            return E_RUNTIME_EXCEPTION;
        }

        AutoPtr<ILooper> looper;
        thread->GetLooper((ILooper**)&looper);
        AutoPtr<IThread> t;
        looper->GetThread((IThread**)&t);
        String name;
        t->GetName(&name);
        mHandlerCheckers.PushBack(new HandlerChecker(thread, name, timeoutMillis, this));
    }
    return NOERROR;
}

void Watchdog::RebootSystem(
    /* [in] */ const String& reason)
{
    Slogger::I(TAG, "Rebooting system because: %s", reason.string());
    AutoPtr<IServiceManager> sm;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    AutoPtr<IInterface> service;
    sm->GetService(IContext::POWER_SERVICE, (IInterface**)&service);
    IIPowerManager* pms = IIPowerManager::Probe(service);
    pms->Reboot(FALSE, reason, FALSE);
}

Int32 Watchdog::EvaluateCheckerCompletionLocked()
{
    Int32 state = COMPLETED;
    List<AutoPtr<HandlerChecker> >::Iterator it;
    for (it = mHandlerCheckers.Begin(); it != mHandlerCheckers.End(); ++it) {
        AutoPtr<HandlerChecker> hc = *it;
        state = Elastos::Core::Math::Max(state, hc->GetCompletionStateLocked());
    }
    return state;
}

AutoPtr< List<AutoPtr<Watchdog::HandlerChecker> > > Watchdog::GetBlockedCheckersLocked()
{
    AutoPtr< List<AutoPtr<HandlerChecker> > > checkers = new List<AutoPtr<HandlerChecker> >();

    List<AutoPtr<HandlerChecker> >::Iterator it;
    for (it = mHandlerCheckers.Begin(); it != mHandlerCheckers.End(); ++it) {
        AutoPtr<HandlerChecker> hc = *it;
        if (hc->IsOverdueLocked()) {
            checkers->PushBack(hc);
        }
    }
    return checkers;
}

String Watchdog::DescribeCheckersLocked(
    /* [in] */ List<AutoPtr<Watchdog::HandlerChecker> >* checkers)
{
    StringBuilder builder;
    Boolean isFirst = FALSE;
    List<AutoPtr<HandlerChecker> >::Iterator it;
    for (it = checkers->Begin(); it != checkers->End(); ++it) {
        AutoPtr<HandlerChecker> hc = *it;
        if (!isFirst) {
            builder += ", ";
        }
        else {
            isFirst = FALSE;
        }

        builder += hc->DescribeBlockedStateLocked();
    }
    return builder.ToString();
}

ECode Watchdog::Run()
{
    AutoPtr<IDebug> debug;
    // CDebug::AcquireSingleton((IDebug**)&debug);

    Boolean waitedHalf = FALSE;
    while (TRUE) {
        AutoPtr< List<AutoPtr<HandlerChecker> > > blockedCheckers;
        String subject;
        Boolean allowRestart;
        Int32 debuggerWasConnected = 0;
        {
            AutoLock lock(this);
            Int64 timeout = CHECK_INTERVAL;
            // Make sure we (re)spin the checkers that have become idle within
            // this wait-and-check interval
            List<AutoPtr<HandlerChecker> >::Iterator it;
            for (it = mHandlerCheckers.Begin(); it != mHandlerCheckers.End(); ++it) {
                AutoPtr<HandlerChecker> hc = *it;
                hc->ScheduleCheckLocked();
            }

            if (debuggerWasConnected > 0) {
                debuggerWasConnected--;
            }

            // NOTE: We use uptimeMillis() here because we do not want to increment the time we
            // wait while asleep. If the device is asleep then the thing that we are waiting
            // to timeout on is asleep as well and won't have a chance to run, causing a FALSE
            // positive on when to kill things.
            Int64 start = SystemClock::GetUptimeMillis();
            while (timeout > 0) {
                // assert(0 && "TODO");
                // if (Debug.isDebuggerConnected()) {
                //     debuggerWasConnected = 2;
                // }
                // try {
                    Wait(timeout);
                // } catch (InterruptedException e) {
                //     Log.wtf(TAG, e);
                // }
                // if (Debug.isDebuggerConnected()) {
                //     debuggerWasConnected = 2;
                // }
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
                    List<Int32> pids;
                    pids.PushBack(Process::MyPid());
                    CActivityManagerService::DumpStackTraces(TRUE, &pids, NULL, NULL, NATIVE_STACKS_OF_INTEREST);
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

        List<Int32> pids;
        pids.PushBack(Process::MyPid());

        if (mPhonePid > 0) pids.PushBack(mPhonePid);
        // Pass !waitedHalf so that just in case we somehow wind up here without having
        // dumped the halfway stacks, we properly re-initialize the trace file.
        AutoPtr<IFile> stack;
        stack = CActivityManagerService::DumpStackTraces(
           !waitedHalf, &pids, NULL, NULL, NATIVE_STACKS_OF_INTEREST);

        // Give some extra time to make sure the stack traces get written.
        // The system's been hanging for a minute, another second or two won't hurt much.
        SystemClock::Sleep(2000);

        // Pull our own kernel thread stacks as well if we're configured for that
        if (RECORD_KERNEL_THREADS) {
            DumpKernelStackTraces();
        }

        // Trigger the kernel to dump all blocked threads to the kernel log
        // try {
        AutoPtr<IFileWriter> sysrq_trigger;
        ECode ec = CFileWriter::New(String("/proc/sysrq-trigger"), (IFileWriter**)&sysrq_trigger);
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Failed to write to /proc/sysrq-trigger");
        }

        ec = IWriter::Probe(sysrq_trigger)->Write(String("w"));
        if (ec == (ECode)E_IO_EXCEPTION) {
            Slogger::E(TAG, "Failed to write to /proc/sysrq-trigger");
        }
        ICloseable::Probe(sysrq_trigger)->Close();

        AutoPtr<ISystemProperties> sysProp;
        CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
        String tracesPath;
        sysProp->Get(String("dalvik.vm.stack-trace-file"), String(NULL), &tracesPath);
        AutoPtr<IDate> date;
        CDate::New((IDate**)&date);
        String str;
        IDateFormat::Probe(mTraceDateFormat)->Format(date, &str);
        String traceFileNameAmendment = String("_SystemServer_WDT") + str;

        if (!tracesPath.IsNullOrEmpty()) {
            AutoPtr<IFile> traceRenameFile;
            CFile::New(tracesPath, (IFile**)&traceRenameFile);
            String newTracesPath;
            Int32 lpos = tracesPath.LastIndexOf(String("."));
            if (-1 != lpos) {
                newTracesPath = tracesPath.Substring (0, lpos) + traceFileNameAmendment + tracesPath.Substring(lpos);
            }
            else {
                newTracesPath = tracesPath + traceFileNameAmendment;
            }
            AutoPtr<IFile> file;
            CFile::New(newTracesPath, (IFile**)&file);
            Boolean flag = FALSE;
            traceRenameFile->RenameTo(file, &flag);
            tracesPath = newTracesPath;
        }

        AutoPtr<IFile> newFd;
        CFile::New(tracesPath, (IFile**)&newFd);

        // Try to add the error to the dropbox, but assuming that the ActivityManager
        // itself may be deadlocked.  (which has happened, causing this statement to
        // deadlock and the watchdog as a whole to be ineffective)
        AutoPtr<DropboxThread> dropboxThread = new DropboxThread(subject, newFd, this);
        dropboxThread->Start();
        // try {
            dropboxThread->Join(2000);  // wait up to 2 seconds for it to return.
        // } catch (InterruptedException ignored) {}

        AutoPtr<IIActivityController> controller;
        {    AutoLock syncLock(this);
            controller = mController;
        }
        if (controller != NULL) {
            Slogger::I(TAG, "Reporting stuck state to activity controller");
            // try {
            // Binder::SetDumpDisabled(String("Service dumps disabled due to hung system process."));
            // 1 = keep waiting, -1 = kill system
            Int32 res;
            controller->SystemNotResponding(subject, &res);
            if (res >= 0) {
                Slogger::I(TAG, "Activity controller requested to coninue to wait");
                waitedHalf = FALSE;
                continue;
            }
            // } catch (RemoteException e) {
            // }
        }

        // Only kill the process if the debugger is not attached.
        // assert(0 && "TODO");
        // if (Debug.isDebuggerConnected()) {
        //     debuggerWasConnected = 2;
        // }
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
            assert(blockedCheckers != NULL);
            List<AutoPtr<HandlerChecker> >::Iterator it;
            for (it = blockedCheckers->Begin(); it != blockedCheckers->End(); ++it) {
                AutoPtr<HandlerChecker> hc = *it;
                Slogger::W(TAG, "%s stack trace:", hc->GetName().string());
                // StackTraceElement[] stackTrace
                //         = blockedCheckers.get(i).GetThread().getStackTrace();
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
    return NOERROR;
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

    native_dumpKernelStacks(tracesPath);
    AutoPtr<IFile> file;
    CFile::New(tracesPath, (IFile**)&file);
    return file;
}

static void dumpOneStack(int tid, int outFd) {
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

void Watchdog::native_dumpKernelStacks(
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