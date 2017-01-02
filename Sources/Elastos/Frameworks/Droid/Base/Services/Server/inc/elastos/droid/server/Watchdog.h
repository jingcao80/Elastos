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

#ifndef __ELASTOS_DROID_SERVER_WATCHDOG_H__
#define __ELASTOS_DROID_SERVER_WATCHDOG_H__

#include <elastos/droid/ext/frameworkext.h>
#include <_Elastos.Droid.Server.h>
#include <elastos/core/Runnable.h>
#include <elastos/droid/os/SystemClock.h>
#include <elastos/droid/content/BroadcastReceiver.h>
#include <elastos/droid/server/am/CActivityManagerService.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/etl/List.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Core::Runnable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IDebug;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IIPowerManager;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::App::IIActivityController;
using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::Server::Am::CActivityManagerService;
using Elastos::Core::Thread;
using Elastos::IO::IFile;
using Elastos::IO::IFileWriter;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Server {

/** This class calls its monitor every minute. Killing this process if they don't return **/
class Watchdog
    : public Thread
{
public:

    /**
     * Used for checking status of handle threads and scheduling monitor callbacks.
     */
    class HandlerChecker
        : public Runnable
    {
    public:
        HandlerChecker(
            /* [in] */ IHandler* handler,
            /* [in] */ const String& name,
            /* [in] */ Int64 waitMaxMillis,
            /* [in] */ Watchdog* host);

        CARAPI AddMonitor(
            /* [in] */ IWatchdogMonitor* monitor);

        CARAPI ScheduleCheckLocked();

        Boolean IsOverdueLocked();

        Int32 GetCompletionStateLocked();

        AutoPtr<IThread> GetThread();

        String GetName();

        String DescribeBlockedStateLocked();

        // @Override
        CARAPI Run();

    private:
        AutoPtr<IHandler> mHandler;
        String mName;
        Int64 mWaitMax;
        List<AutoPtr<IWatchdogMonitor> > mMonitors;
        Boolean mCompleted;
        AutoPtr<IWatchdogMonitor> mCurrentMonitor;
        Int64 mStartTime;
        Watchdog* mHost;
    };

private:

    class RebootRequestReceiver
        : public BroadcastReceiver
    {
    public:
        RebootRequestReceiver(
            /* [in] */ Watchdog* dog);

        // @Override
        CARAPI OnReceive(
            /* [in] */ IContext* c,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* str);

    private:
        Watchdog* mHost;
    };

    class DropboxThread
        : public Thread
    {
    public:
        DropboxThread(
            /* [in] */ const String& subject,
            /* [in] */ IFile* stack,
            /* [in] */ Watchdog* dog);

        // @Override
        CARAPI Run();

        CARAPI ToString(
            /* [out] */ String* str);
    private:
        String mSubject;
        AutoPtr<IFile> mStack;
        Watchdog* mHost;
    };

public:
    static AutoPtr<Watchdog> GetInstance();

    //@Override
    CARAPI Run();

    CARAPI ProcessStarted(
        /* [in] */ const String& name,
        /* [in] */ Int32 pid);

    CARAPI SetActivityController(
        /* [in] */ IIActivityController* controller);

    CARAPI SetAllowRestart(
        /* [in] */ Boolean allowRestart);

    CARAPI AddMonitor(
        /* [in] */ IWatchdogMonitor* monitor);

    CARAPI AddThread(
        /* [in] */ IHandler* thread);

    CARAPI AddThread(
        /* [in] */ IHandler* thread,
        /* [in] */ Int64 timeoutMillis);

    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ CActivityManagerService* activity);
private:

    Watchdog();

    /**
     * Perform a full reboot of the system.
     */
    void RebootSystem(
        /* [in] */ const String& reason);

    Int32 EvaluateCheckerCompletionLocked();

    AutoPtr< List<AutoPtr<HandlerChecker> > > GetBlockedCheckersLocked();

    String DescribeCheckersLocked(
        /* [in] */ List<AutoPtr<HandlerChecker> >* checkers);

private:
    AutoPtr<IFile> DumpKernelStackTraces();

    void native_dumpKernelStacks(
        /* [in] */ const String& tracesPath);

public:
    static const String TAG;
    static const Boolean localLOGV;

    // Set this to true to use debug default values.
    static const Boolean DB;

    // Set this to true to have the watchdog record kernel thread stacks when it fires
    static const Boolean RECORD_KERNEL_THREADS;

    static const Int64 DEFAULT_TIMEOUT;
    static const Int64 CHECK_INTERVAL;

    // These are temporally ordered: larger values as lateness increases
    static const Int32 COMPLETED;
    static const Int32 WAITING;
    static const Int32 WAITED_HALF;
    static const Int32 OVERDUE;

    // Which native processes to dump into dropbox's stack traces
    static const AutoPtr<ArrayOf<String> > NATIVE_STACKS_OF_INTEREST;

    static AutoPtr<Watchdog> sWatchdog;

    /* This handler will be used to post message back onto the main thread */
    List<AutoPtr<HandlerChecker> > mHandlerCheckers;
    AutoPtr<HandlerChecker> mMonitorChecker;
    AutoPtr<IContentResolver> mResolver;
    CActivityManagerService* mActivity;

    Int32 mPhonePid;
    AutoPtr<IIActivityController> mController;
    Boolean mAllowRestart;
    AutoPtr<ISimpleDateFormat> mTraceDateFormat;
};

} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_WATCHDOG_H__