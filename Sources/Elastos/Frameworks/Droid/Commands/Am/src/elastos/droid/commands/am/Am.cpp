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

#include "Elastos.Droid.Commands.Am.h"
#include "Am.h"
#include "CMyActivityController.h"
#include "CIntentReceiver.h"
#include "CInstrumentationWatcher.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/app/ActivityManagerNative.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringToIntegral.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::App::ActivityManagerNative;
using Elastos::Droid::App::CActivityManagerWaitResult;
using Elastos::Droid::App::CProfilerInfo;
using Elastos::Droid::App::CUiAutomationConnection;
using Elastos::Droid::App::EIID_IIActivityController;
using Elastos::Droid::App::EIID_IInstrumentationWatcher;
using Elastos::Droid::App::IIActivityContainer;
using Elastos::Droid::App::IActivityManager;
using Elastos::Droid::App::IActivityManagerStackInfo;
using Elastos::Droid::App::IActivityManagerWaitResult;
using Elastos::Droid::App::IAppOpsManager;
using Elastos::Droid::App::IIUiAutomationConnection;
using Elastos::Droid::App::IProfilerInfo;
using Elastos::Droid::App::Usage::IConfigurationStats;
using Elastos::Droid::App::Usage::IIUsageStatsManager;
using Elastos::Droid::App::Usage::IUsageStatsManager;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::EIID_IIntentReceiver;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IActivityInfo;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::Content::Pm::IPackageItemInfo;
using Elastos::Droid::Content::Pm::IParceledListSlice;
using Elastos::Droid::Content::Pm::IResolveInfo;
using Elastos::Droid::Content::Res::CConfigurationHelper;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Content::Res::IConfigurationHelper;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Os::CBinder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CParcelFileDescriptorHelper;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Os::CSystemProperties;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Os::IParcelFileDescriptorHelper;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CArrayMap;
using Elastos::Droid::View::IIWindowManager;
using Elastos::Core::AutoLock;
using Elastos::Core::CoreUtils;
using Elastos::Core::CThread;
using Elastos::Core::CSystem;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::IComparator;
using Elastos::Core::ISystem;
using Elastos::Core::StringToIntegral;
using Elastos::Core::StringUtils;
using Elastos::Core::Thread;
using Elastos::IO::CBufferedReader;
using Elastos::IO::CFile;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IFile;
using Elastos::IO::IInputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::CStringTokenizer;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::IIterator;
using Elastos::Utility::ISet;
using Elastos::Utility::IStringTokenizer;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

static const String FATAL_ERROR_CODE("Error type 1");
static const String NO_SYSTEM_ERROR_CODE("Error type 2");
static const String NO_CLASS_ERROR_CODE("Error type 3");

static const String TAG("COMMAND_AM");

static AutoPtr<IPrintStream> InitPrintStream(
    /* [in] */ Int32 no)
{
    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    static AutoPtr<IPrintStream> stream;
    if (no == 1) {
        system->GetOut((IPrintStream**)&stream);
    }
    else if (no == 2) {
        system->GetErr((IPrintStream**)&stream);
    }
    return stream;
}

AutoPtr<IPrintStream> Am::System::Out = InitPrintStream(1);
AutoPtr<IPrintStream> Am::System::Err = InitPrintStream(2);

Am::MyActivityController::GdbRunnable::GdbRunnable(
    /* [in] */ MyActivityController* host,
    /* [in] */ IReader* converter)
    : mHost(host)
    , mConverter(converter)
{
    assert(host && converter);
}

ECode Am::MyActivityController::GdbRunnable::Run()
{
    AutoPtr<IBufferedReader> in;
    CBufferedReader::New(mConverter, (IBufferedReader**)&in);
    String line;
    Int32 count = 0;
    while (TRUE) {
        {    AutoLock syncLock(mHost);
            if (mHost->mGdbThread == NULL) {
                return NOERROR;
            }
            if (count == 2) {
                mHost->mGotGdbPrint = TRUE;
                mHost->NotifyAll();
            }
        }
        // try {
        ECode ec = in->ReadLine(&line);
        if (FAILED(ec)) {
            return ec == (ECode)E_IO_EXCEPTION ? NOERROR : ec;
        }
        if (line.IsNull()) {
            return NOERROR;
        }
        System::Out->Println(String("GDB: ") + line);
        count++;
        // } catch (IOException e) {
        // return;
        // }
    }

    assert(0 && "Will not go here.");
    return NOERROR;
}

CAR_INTERFACE_IMPL(Am::MyActivityController, Object, IIActivityController);

Am::MyActivityController::MyActivityController()
    : mState(STATE_NORMAL)
    , mResult(RESULT_DEFAULT)
    , mGotGdbPrint(FALSE)
{}

Am::MyActivityController::~MyActivityController()
{}

ECode Am::MyActivityController::constructor(
    /* [in] */ IIActivityManager* am,
    /* [in] */ const String& gdbPort)
{
    mAm = am;
    mGdbPort = gdbPort;
    return NOERROR;
}

ECode Am::MyActivityController::ActivityResuming(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(this);
        System::Out->Println(String("** Activity resuming: ") + pkg);
    }
    *result = TRUE;
    return NOERROR;

}

ECode Am::MyActivityController::ActivityStarting(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(this);
        System::Out->Println(String("** Activity starting: ") + pkg);
    }
    *result = TRUE;
    return NOERROR;
}

ECode Am::MyActivityController::AppCrashed(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& shortMsg,
    /* [in] */ const String& longMsg,
    /* [in] */ Int64 timeMillis,
    /* [in] */ const String& stackTrace,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(this);
        System::Out->Println(String("** ERROR: PROCESS CRASHED"));
        System::Out->Println(String("processName: ") + processName);
        System::Out->Println(String("processPid: ") + StringUtils::ToString(pid));
        System::Out->Println(String("shortMsg: ") + shortMsg);
        System::Out->Println(String("longMsg: ") + longMsg);
        System::Out->Println(String("timeMillis: ") + StringUtils::ToString(timeMillis));
        System::Out->Println(String("stack:"));
        System::Out->Print(stackTrace);
        System::Out->Println(String("#"));
        Int32 result2;
        WaitControllerLocked(pid, STATE_CRASHED, &result2);
        *result = result2 == RESULT_CRASH_KILL ? FALSE : TRUE;
    }
    return NOERROR;
}

ECode Am::MyActivityController::AppEarlyNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& annotation,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(this);
        System::Out->Println(String("** ERROR: EARLY PROCESS NOT RESPONDING"));
        System::Out->Println(String("processName: ") + processName);
        System::Out->Println(String("processPid: ") + StringUtils::ToString(pid));
        System::Out->Println(String("annotation: ") + annotation);
        Int32 result2;
        WaitControllerLocked(pid, STATE_EARLY_ANR, &result2);
        *result = result2 == RESULT_EARLY_ANR_KILL ? -1 : 0;
    }
    return NOERROR;
}

ECode Am::MyActivityController::AppNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& processStats,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(this);
        System::Out->Println(String("** ERROR: PROCESS NOT RESPONDING"));
        System::Out->Println(String("processName: ") + processName);
        System::Out->Println(String("processPid: ") + StringUtils::ToString(pid));
        System::Out->Println(String("processStats:"));
        System::Out->Print(processStats);
        System::Out->Println(String("#"));
        Int32 result2;
        WaitControllerLocked(pid, STATE_ANR, &result2);
        if (result2 == RESULT_ANR_KILL) {
            *result = -1;
        }
        else if (result2 == RESULT_ANR_WAIT) {
            *result = 1;
        }
        else {
            *result = 0;
        }
    }
    return NOERROR;
}

ECode Am::MyActivityController::SystemNotResponding(
    /* [in] */ const String& message,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(this);
        System::Out->Println(String("** ERROR: PROCESS NOT RESPONDING"));
        System::Out->Println(String("message: ") + message);
        System::Out->Println(String("#"));
        System::Out->Println(String("Allowing system to die."));
        *result = -1;
    }
    return NOERROR;
}

void Am::MyActivityController::KillGdbLocked()
{
    mGotGdbPrint = FALSE;
    if (mGdbProcess != NULL) {
        System::Out->Println(String("Stopping gdbserver"));
        assert(0 && "TODO");
        // mGdbProcess->Destroy();
        mGdbProcess = NULL;
    }
    if (mGdbThread != NULL) {
        mGdbThread->Interrupt();
        mGdbThread = NULL;
    }
}

void Am::MyActivityController::WaitControllerLocked(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 state,
    /* [out] */ Int32* result)
{
    if (!result) return;

    if (!mGdbPort.IsNull()) {
        KillGdbLocked();

        // try {
        System::Out->Println(String("Starting gdbserver on port ") + mGdbPort);
        System::Out->Println(String("Do the following:"));
        System::Out->Println(String("  adb forward tcp:") + mGdbPort + " tcp:" + mGdbPort);
        System::Out->Println(String("  gdbclient app_process :") + mGdbPort);

        assert(0 && "TODO");
        // mGdbProcess = Runtime.getRuntime().exec(new String[] {
        //         "gdbserver", ":" + mGdbPort, "--attach", Integer.toString(pid)
        // });
        // final InputStreamReader converter = new InputStreamReader(
        //         mGdbProcess.getInputStream());
        AutoPtr<IInputStreamReader> converter;
        AutoPtr<GdbRunnable> runnable = new GdbRunnable(this, IReader::Probe(converter));
        if (runnable == NULL) return;
        mGdbThread = NULL;
        CThread::New((IRunnable*)runnable, (IThread**)&mGdbThread);
        mGdbThread->Start();

        // Stupid waiting for .5s.  Doesn't matter if we end early.
        // try {
        Wait(500);
        // } catch (InterruptedException e) {
        // }

        // } catch (IOException e) {
        System::Err->Println(String("Failure starting gdbserver: ") + "IOException");
        KillGdbLocked();
        // }
    }
    mState = state;
    System::Out->Println(String(""));
    PrintMessageForState();

    while (mState != STATE_NORMAL) {
        // try {
        Wait();
        // } catch (InterruptedException e) {
        // }
    }

    KillGdbLocked();

    *result = mResult;
    return;
}

void Am::MyActivityController::ResumeController(
    /* [in] */ Int32 result)
{
    {    AutoLock syncLock(this);
        mState = STATE_NORMAL;
        mResult = result;
        NotifyAll();
    }
}

void Am::MyActivityController::PrintMessageForState()
{
    switch (mState) {
        case STATE_NORMAL:
            System::Out->Println(String("Monitoring activity manager...  available commands:"));
            break;
        case STATE_CRASHED:
            System::Out->Println(String("Waiting after crash...  available commands:"));
            System::Out->Println(String("(c)ontinue: show crash dialog"));
            System::Out->Println(String("(k)ill: immediately kill app"));
            break;
        case STATE_EARLY_ANR:
            System::Out->Println(String("Waiting after early ANR...  available commands:"));
            System::Out->Println(String("(c)ontinue: standard ANR processing"));
            System::Out->Println(String("(k)ill: immediately kill app"));
            break;
        case STATE_ANR:
            System::Out->Println(String("Waiting after ANR...  available commands:"));
            System::Out->Println(String("(c)ontinue: show ANR dialog"));
            System::Out->Println(String("(k)ill: immediately kill app"));
            System::Out->Println(String("(w)ait: wait some more"));
            break;
    }
    System::Out->Println(String("(q)uit: finish monitoring"));
}

ECode Am::MyActivityController::Run()
{
    // try {
    PrintMessageForState();

    FAIL_RETURN(mAm->SetActivityController(this));
    mState = STATE_NORMAL;

    AutoPtr<ISystem> system;
    AutoPtr<IInputStream> systemIn;
    AutoPtr<IInputStreamReader> converter;
    AutoPtr<IBufferedReader> in;
    String line;
    FAIL_GOTO(CSystem::AcquireSingleton((ISystem**)&system), Exit);
    FAIL_GOTO(system->GetIn((IInputStream**)&systemIn), Exit);
    FAIL_GOTO(CInputStreamReader::New(systemIn, (IInputStreamReader**)&converter), Exit);
    FAIL_GOTO(CBufferedReader::New(IReader::Probe(converter), (IBufferedReader**)&in), Exit);

    while (!(in->ReadLine(&line), line).IsNull()) {
        Boolean addNewline = TRUE;
        if (line.GetLength() <= 0) {
            addNewline = FALSE;
        }
        else if (line.Equals("q") || line.Equals("quit")) {
            ResumeController(RESULT_DEFAULT);
            break;
        }
        else if (mState == STATE_CRASHED) {
            if (line.Equals("c") || line.Equals("continue")) {
                ResumeController(RESULT_CRASH_DIALOG);
            }
            else if (line.Equals("k") || line.Equals("kill")) {
                ResumeController(RESULT_CRASH_KILL);
            }
            else {
                System::Out->Println(String("Invalid command: ") + line);
            }
        }
        else if (mState == STATE_ANR) {
            if (line.Equals("c") || line.Equals("continue")) {
                ResumeController(RESULT_ANR_DIALOG);
            }
            else if (line.Equals("k") || line.Equals("kill")) {
                ResumeController(RESULT_ANR_KILL);
            }
            else if (line.Equals("w") || line.Equals("wait")) {
                ResumeController(RESULT_ANR_WAIT);
            }
            else {
                System::Out->Println(String("Invalid command: ") + line);
            }
        }
        else if (mState == STATE_EARLY_ANR) {
            if (line.Equals("c") || line.Equals("continue")) {
                ResumeController(RESULT_EARLY_ANR_CONTINUE);
            }
            else if (line.Equals("k") || line.Equals("kill")) {
                ResumeController(RESULT_EARLY_ANR_KILL);
            }
            else {
                System::Out->Println(String("Invalid command: ") + line);
            }
        }
        else {
            System::Out->Println(String("Invalid command: ") + line);
        }

        {    AutoLock syncLock(this);
            if (addNewline) {
                System::Out->Println(String(""));
            }
            PrintMessageForState();
        }
    }

    return NOERROR;
    // } catch (IOException e) {
    //     e.printStackTrace();
    // } finally {
Exit:
    mAm->SetActivityController(NULL);
    // }
    return E_REMOTE_EXCEPTION;
}

CAR_INTERFACE_IMPL(Am::IntentReceiver, Object, IIntentReceiver);

Am::IntentReceiver::IntentReceiver()
    : mFinished(FALSE)
{}

Am::IntentReceiver::~IntentReceiver()
{}

ECode Am::IntentReceiver::PerformReceive(
    /* [in] */ IIntent* intent,
    /* [in] */ Int32 resultCode,
    /* [in] */ const String& data,
    /* [in] */ IBundle* extras,
    /* [in] */ Boolean ordered,
    /* [in] */ Boolean sticky,
    /* [in] */ Int32 sendingUser)
{
    StringBuilder line("Broadcast completed: result=");
    line += resultCode;
    if (!data.IsNull()) {
        line +=", data=\"";
        line += data; line += "\"";
    }
    if (extras != NULL) {
        String extrasString;
        IObject::Probe(extras)->ToString(&extrasString);
        line += ", extras: "; line += extrasString;
    }
    System::Out->Println(line.ToString());

    {
        AutoLock syncLock(this);
        mFinished = TRUE;
        return NotifyAll();
    }

    assert(0 && "Will not go here.");
    return NOERROR;
}

ECode Am::IntentReceiver::WaitForFinish()
{
    {
        AutoLock syncLock(this);
    // try {
        while (!mFinished) {
            ECode ec = Wait();
            if (FAILED(ec)) return E_ILLEGAL_STATE_EXCEPTION;
        }
        return NOERROR;
    // } catch (InterruptedException e) {
    //     throw new IllegalStateException(e);
    // }
    }

    assert(0 && "Will not go here.");
    return NOERROR;
}

CAR_INTERFACE_IMPL(Am::InstrumentationWatcher, Object, IInstrumentationWatcher);

Am::InstrumentationWatcher::InstrumentationWatcher()
    : mFinished(FALSE)
    , mRawMode(FALSE)
{}

Am::InstrumentationWatcher::~InstrumentationWatcher()
{}

ECode Am::InstrumentationWatcher::InstrumentationStatus(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    VALIDATE_NOT_NULL(results);

    {
        AutoLock syncLock(this);
        // pretty printer mode?
        String pretty;
        if (!mRawMode && results != NULL) {
            results->GetString(IInstrumentation::REPORT_KEY_STREAMRESULT, &pretty);
        }

        if (!pretty.IsNull()) {
            System::Out->Println(pretty);
        }
        else {
            if (results != NULL) {
                AutoPtr<ISet> keySet;
                results->GetKeySet((ISet**)&keySet);
                AutoPtr<IIterator> it;
                keySet->GetIterator((IIterator**)&it);
                Boolean hasNext = FALSE;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> element;
                    it->GetNext((IInterface**)&element);
                    String key;
                    assert(ICharSequence::Probe(element));
                    ICharSequence::Probe(element)->ToString(&key);
                    AutoPtr<IInterface> value;
                    results->Get(key, (IInterface**)&value);
                    String valueString;
                    IObject::Probe(value)->ToString(&valueString);
                    System::Out->Println(String(
                            "INSTRUMENTATION_STATUS: ") + key + "=" + valueString);
                }
            }
            System::Out->Println(String("INSTRUMENTATION_STATUS_CODE: ")
                    + StringUtils::ToString(resultCode));
        }
        return NotifyAll();
    }

    assert(0 && "Will not go here.");
    return NOERROR;
}

ECode Am::InstrumentationWatcher::InstrumentationFinished(
    /* [in] */ IComponentName* name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle* results)
{
    VALIDATE_NOT_NULL(results);

    {    AutoLock syncLock(this);
        // pretty printer mode?
        String pretty;
        if (!mRawMode && results != NULL) {
            results->GetString(IInstrumentation::REPORT_KEY_STREAMRESULT, &pretty);
        }

        if (!pretty.IsNull()) {
            System::Out->Println(pretty);
        }
        else {
            if (results != NULL) {
                AutoPtr<ISet> keySet;
                results->GetKeySet((ISet**)&keySet);
                AutoPtr<IIterator> it;
                keySet->GetIterator((IIterator**)&it);
                Boolean hasNext = FALSE;
                while (it->HasNext(&hasNext), hasNext) {
                    AutoPtr<IInterface> element;
                    it->GetNext((IInterface**)&element);
                    String key;
                    assert(ICharSequence::Probe(element));
                    ICharSequence::Probe(element)->ToString(&key);
                    AutoPtr<IInterface> value;
                    results->Get(key, (IInterface**)&value);
                    String valueString;
                    IObject::Probe(value)->ToString(&valueString);
                    System::Out->Println(String(
                            "INSTRUMENTATION_RESULT: ") + key + "=" + valueString);
                }
            }
            System::Out->Println(String("INSTRUMENTATION_CODE: ")
                    + StringUtils::ToString(resultCode));
        }
        mFinished = TRUE;
        return NotifyAll();
    }

    assert(0 && "Will not go here.");
    return NOERROR;
}

ECode Am::InstrumentationWatcher::SetRawOutput(
    /* [in] */ Boolean rawMode)
{
    mRawMode = rawMode;
    return NOERROR;
}

ECode Am::InstrumentationWatcher::WaitForFinish(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    {    AutoLock syncLock(this);
        while (!mFinished) {
            // try {
            // if (!mAm.asBinder().pingBinder()) {
            assert(0 && "TODO");
            //     *result = FALSE;
            //     return NOERROR;
            // }
            ECode ec = Wait(1000);
            if (FAILED(ec)) return E_ILLEGAL_STATE_EXCEPTION;
            // } catch (InterruptedException e) {
            //     throw new IllegalStateException(e);
            // }
        }
    }
    *result = TRUE;
    return NOERROR;
}

CAR_INTERFACE_IMPL(Am::Comparator, Object, IComparator);

Am::Comparator::Comparator(
    /* [in] */ IArrayMap* am)
    : mAm(am)
{
    assert(am);
}

ECode Am::Comparator::Compare(
    /* [in] */ IInterface* a,
    /* [in] */ IInterface* b,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IInterface> av, bv;
    mAm->Get(a, (IInterface**)&av);
    mAm->Get(b, (IInterface**)&bv);

    *result = CoreUtils::Unbox(IInteger32::Probe(bv))
            - CoreUtils::Unbox(IInteger32::Probe(av));
    return NOERROR;
}

Am::Am()
    : mStartFlags(0)
    , mWaitOption(FALSE)
    , mStopOption(FALSE)
    , mRepeat(0)
    , mUserId(-1)
    , mReceiverPermission(NULL)
    , mProfileFile(NULL)
    , mSamplingInterval(0)
    , mAutoStop(FALSE)
{}

ECode Am::OnRun()
{
    mAm = ActivityManagerNative::GetDefault();
    if (mAm == NULL) {
        System::Err->Println(NO_SYSTEM_ERROR_CODE);
        Logger::E(TAG, "Can't connect to activity manager; is the system running?");
        return E_ANDROID_EXCEPTION;
    }

    String op;
    NextArgRequired(&op);

    if (op.Equals("start")) {
        return RunStart();
    }
    else if (op.Equals("startservice")) {
        return RunStartService();
    }
    else if (op.Equals("stopservice")) {
        return RunStopService();
    }
    else if (op.Equals("force-stop")) {
        return RunForceStop();
    }
    else if (op.Equals("kill")) {
        return RunKill();
    }
    else if (op.Equals("kill-all")) {
        return RunKillAll();
    }
    else if (op.Equals("instrument")) {
        return RunInstrument();
    }
    else if (op.Equals("broadcast")) {
        return SendBroadcast();
    }
    else if (op.Equals("profile")) {
        return RunProfile();
    }
    else if (op.Equals("dumpheap")) {
        return RunDumpHeap();
    }
    else if (op.Equals("set-debug-app")) {
        return RunSetDebugApp();
    }
    else if (op.Equals("clear-debug-app")) {
        return RunClearDebugApp();
    }
    else if (op.Equals("bug-report")) {
        return RunBugReport();
    }
    else if (op.Equals("monitor")) {
        return RunMonitor();
    }
    else if (op.Equals("hang")) {
        return RunHang();
    }
    else if (op.Equals("restart")) {
        return RunRestart();
    }
    else if (op.Equals("idle-maintenance")) {
        return RunIdleMaintenance();
    }
    else if (op.Equals("screen-compat")) {
        return RunScreenCompat();
    }
    else if (op.Equals("to-uri")) {
        return RunToUri(FALSE);
    }
    else if (op.Equals("to-intent-uri")) {
        return RunToUri(TRUE);
    }
    else if (op.Equals("switch-user")) {
        return RunSwitchUser();
    }
    else if (op.Equals("start-user")) {
        return RunStartUserInBackground();
    }
    else if (op.Equals("stop-user")) {
        return RunStopUser();
    }
    else if (op.Equals("stack")) {
        return RunStack();
    }
    else if (op.Equals("lock-task")) {
        return RunLockTask();
    }
    else if (op.Equals("get-config")) {
        return RunGetConfig();
    }
    else {
        ShowError(String("Error: unknown command '") + op + "'");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

ECode Am::OnShowUsage(
    /* [in] */ IPrintStream* outPrint)
{
    outPrint->Println(String(
        "usage: am [subcommand] [options]\n"
        "usage: am start [-D] [-W] [-P <FILE>] [--start-profiler <FILE>]\n"
        "               [--sampling INTERVAL] [-R COUNT] [-S] [--opengl-trace]\n"
        "               [--user <USER_ID> | current] <INTENT>\n"
        "       am startservice [--user <USER_ID> | current] <INTENT>\n"
        "       am stopservice [--user <USER_ID> | current] <INTENT>\n"
        "       am force-stop [--user <USER_ID> | all | current] <PACKAGE>\n"
        "       am kill [--user <USER_ID> | all | current] <PACKAGE>\n"
        "       am kill-all\n"
        "       am broadcast [--user <USER_ID> | all | current] <INTENT>\n"
        "       am instrument [-r] [-e <NAME> <VALUE>] [-p <FILE>] [-w]\n"
        "               [--user <USER_ID> | current]\n"
        "               [--no-window-animation] [--abi <ABI>] <COMPONENT>\n"
        "       am profile start [--user <USER_ID> current] <PROCESS> <FILE>\n"
        "       am profile stop [--user <USER_ID> current] [<PROCESS>]\n"
        "       am dumpheap [--user <USER_ID> current] [-n] <PROCESS> <FILE>\n"
        "       am set-debug-app [-w] [--persistent] <PACKAGE>\n"
        "       am clear-debug-app\n"
        "       am monitor [--gdb <port>]\n"
        "       am hang [--allow-restart]\n"
        "       am restart\n"
        "       am idle-maintenance\n"
        "       am screen-compat [on|off] <PACKAGE>\n"
        "       am to-uri [INTENT]\n"
        "       am to-intent-uri [INTENT]\n"
        "       am switch-user <USER_ID>\n"
        "       am start-user <USER_ID>\n"
        "       am stop-user <USER_ID>\n"
        "       am stack start <DISPLAY_ID> <INTENT>\n"
        "       am stack movetask <TASK_ID> <STACK_ID> [true|false]\n"
        "       am stack resize <STACK_ID> <LEFT,TOP,RIGHT,BOTTOM>\n"
        "       am stack list\n"
        "       am stack info <STACK_ID>\n"
        "       am lock-task <TASK_ID>\n"
        "       am lock-task stop\n"
        "       am get-config\n"
        "\n"
        "am start: start an Activity.  Options are:\n"
        "    -D: enable debugging\n"
        "    -W: wait for launch to complete\n"
        "    --start-profiler <FILE>: start profiler and send results to <FILE>\n"
        "    --sampling INTERVAL: use sample profiling with INTERVAL microseconds\n"
        "        between samples (use with --start-profiler)\n"
        "    -P <FILE>: like above, but profiling stops when app goes idle\n"
        "    -R: repeat the activity launch <COUNT> times.  Prior to each repeat,\n"
        "        the top activity will be finished.\n"
        "    -S: force stop the target app before starting the activity\n"
        "    --opengl-trace: enable tracing of OpenGL functions\n"
        "    --user <USER_ID> | current: Specify which user to run as; if not\n"
        "        specified then run as the current user.\n"
        "\n"
        "am startservice: start a Service.  Options are:\n"
        "    --user <USER_ID> | current: Specify which user to run as; if not\n"
        "        specified then run as the current user.\n"
        "\n"
        "am stopservice: stop a Service.  Options are:\n"
        "    --user <USER_ID> | current: Specify which user to run as; if not\n"
        "        specified then run as the current user.\n"
        "\n"
        "am force-stop: force stop everything associated with <PACKAGE>.\n"
        "    --user <USER_ID> | all | current: Specify user to force stop;\n"
        "        all users if not specified.\n"
        "\n"
        "am kill: Kill all processes associated with <PACKAGE>.  Only kills.\n"
        "  processes that are safe to kill -- that is, will not impact the user\n"
        "  experience.\n"
        "    --user <USER_ID> | all | current: Specify user whose processes to kill;\n"
        "        all users if not specified.\n"
        "\n"
        "am kill-all: Kill all background processes.\n"
        "\n"
        "am broadcast: send a broadcast Intent.  Options are:\n"
        "    --user <USER_ID> | all | current: Specify which user to send to; if not\n"
        "        specified then send to all users.\n"
        "    --receiver-permission <PERMISSION>: Require receiver to hold permission.\n"
        "\n"
        "am instrument: start an Instrumentation.  Typically this target <COMPONENT>\n"
        "  is the form <TEST_PACKAGE>/<RUNNER_CLASS>.  Options are:\n"
        "    -r: print raw results (otherwise decode REPORT_KEY_STREAMRESULT).  Use with\n"
        "        [-e perf true] to generate raw output for performance measurements.\n"
        "    -e <NAME> <VALUE>: set argument <NAME> to <VALUE>.  For test runners a\n"
        "        common form is [-e <testrunner_flag> <value>[,<value>...]].\n"
        "    -p <FILE>: write profiling data to <FILE>\n"
        "    -w: wait for instrumentation to finish before returning.  Required for\n"
        "        test runners.\n"
        "    --user <USER_ID> | current: Specify user instrumentation runs in;\n"
        "        current user if not specified.\n"
        "    --no-window-animation: turn off window animations while running.\n"
        "    --abi <ABI>: Launch the instrumented process with the selected ABI.\n"
        "        This assumes that the process supports the selected ABI.\n"
        "\n"
        "am profile: start and stop profiler on a process.  The given <PROCESS> argument\n"
        "  may be either a process name or pid.  Options are:\n"
        "    --user <USER_ID> | current: When supplying a process name,\n"
        "        specify user of process to profile; uses current user if not specified.\n"
        "\n"
        "am dumpheap: dump the heap of a process.  The given <PROCESS> argument may\n"
        "  be either a process name or pid.  Options are:\n"
        "    -n: dump native heap instead of managed heap\n"
        "    --user <USER_ID> | current: When supplying a process name,\n"
        "        specify user of process to dump; uses current user if not specified.\n"
        "\n"
        "am set-debug-app: set application <PACKAGE> to debug.  Options are:\n"
        "    -w: wait for debugger when application starts\n"
        "    --persistent: retain this value\n"
        "\n"
        "am clear-debug-app: clear the previously set-debug-app.\n"
        "\n"
        "am bug-report: request bug report generation; will launch UI\n"
        "    when done to select where it should be delivered.\n"
        "\n"
        "am monitor: start monitoring for crashes or ANRs.\n"
        "    --gdb: start gdbserv on the given port at crash/ANR\n"
        "\n"
        "am hang: hang the system.\n"
        "    --allow-restart: allow watchdog to perform normal system restart\n"
        "\n"
        "am restart: restart the user-space system.\n"
        "\n"
        "am idle-maintenance: perform idle maintenance now.\n"
        "\n"
        "am screen-compat: control screen compatibility mode of <PACKAGE>.\n"
        "\n"
        "am to-uri: print the given Intent specification as a URI.\n"
        "\n"
        "am to-intent-uri: print the given Intent specification as an intent: URI.\n"
        "\n"
        "am switch-user: switch to put USER_ID in the foreground, starting\n"
        "  execution of that user if it is currently stopped.\n"
        "\n"
        "am start-user: start USER_ID in background if it is currently stopped,\n"
        "  use switch-user if you want to start the user in foreground.\n"
        "\n"
        "am stop-user: stop execution of USER_ID, not allowing it to run any\n"
        "  code until a later explicit start or switch to it.\n"
        "\n"
        "am stack start: start a new activity on <DISPLAY_ID> using <INTENT>.\n"
        "\n"
        "am stack movetask: move <TASK_ID> from its current stack to the top (true) or"
        "   bottom (false) of <STACK_ID>.\n"
        "\n"
        "am stack resize: change <STACK_ID> size and position to <LEFT,TOP,RIGHT,BOTTOM>.\n"
        "\n"
        "am stack list: list all of the activity stacks and their sizes.\n"
        "\n"
        "am stack info: display the information about activity stack <STACK_ID>.\n"
        "\n"
        "am lock-task: bring <TASK_ID> to the front and don't allow other tasks to run\n"
        "\n"
        "am get-config: retrieve the configuration and any recent configurations\n"
        "  of the device\n"
        "\n"
        "<INTENT> specifications include these flags and arguments:\n"
        "    [-a <ACTION>] [-d <DATA_URI>] [-t <MIME_TYPE>]\n"
        "    [-c <CATEGORY> [-c <CATEGORY>] ...]\n"
        "    [-e|--es <EXTRA_KEY> <EXTRA_STRING_VALUE> ...]\n"
        "    [--esn <EXTRA_KEY> ...]\n"
        "    [--ez <EXTRA_KEY> <EXTRA_BOOLEAN_VALUE> ...]\n"
        "    [--ei <EXTRA_KEY> <EXTRA_INT_VALUE> ...]\n"
        "    [--el <EXTRA_KEY> <EXTRA_LONG_VALUE> ...]\n"
        "    [--ef <EXTRA_KEY> <EXTRA_FLOAT_VALUE> ...]\n"
        "    [--eu <EXTRA_KEY> <EXTRA_URI_VALUE> ...]\n"
        "    [--ecn <EXTRA_KEY> <EXTRA_COMPONENT_NAME_VALUE>]\n"
        "    [--eia <EXTRA_KEY> <EXTRA_INT_VALUE>[,<EXTRA_INT_VALUE...]]\n"
        "    [--ela <EXTRA_KEY> <EXTRA_LONG_VALUE>[,<EXTRA_LONG_VALUE...]]\n"
        "    [--efa <EXTRA_KEY> <EXTRA_FLOAT_VALUE>[,<EXTRA_FLOAT_VALUE...]]\n"
        "    [--esa <EXTRA_KEY> <EXTRA_STRING_VALUE>[,<EXTRA_STRING_VALUE...]]\n"
        "        (to embed a comma into a string escape it using \"\\,\")\n"
        "    [-n <COMPONENT>] [-f <FLAGS>]\n"
        "    [--grant-read-uri-permission] [--grant-write-uri-permission]\n"
        "    [--grant-persistable-uri-permission] [--grant-prefix-uri-permission]\n"
        "    [--debug-log-resolution] [--exclude-stopped-packages]\n"
        "    [--include-stopped-packages]\n"
        "    [--activity-brought-to-front] [--activity-clear-top]\n"
        "    [--activity-clear-when-task-reset] [--activity-exclude-from-recents]\n"
        "    [--activity-launched-from-history] [--activity-multiple-task]\n"
        "    [--activity-no-animation] [--activity-no-history]\n"
        "    [--activity-no-user-action] [--activity-previous-is-top]\n"
        "    [--activity-reorder-to-front] [--activity-reset-task-if-needed]\n"
        "    [--activity-single-top] [--activity-clear-task]\n"
        "    [--activity-task-on-home]\n"
        "    [--receiver-registered-only] [--receiver-replace-pending]\n"
        "    [--selector]\n"
        "    [<URI> | <PACKAGE> | <COMPONENT>]\n"));
    return NOERROR;
}

Int32 Am::ParseUserArg(
    /* [in] */ const String& arg)
{
    Int32 userId;
    if (arg.Equals("all")) {
        userId = IUserHandle::USER_ALL;
    }
    else if (arg.Equals("current") || arg.Equals("cur")) {
        userId = IUserHandle::USER_CURRENT;
    }
    else {
        userId = StringUtils::ParseInt32(arg);
    }
    return userId;
}

void Am::RemoveWallOption()
{
    AutoPtr<ISystemProperties> sysProp;
    CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp);
    String props;
    sysProp->Get(String("dalvik.vm.extra-opts"), &props);
    if (!props.IsNull() && props.Contains("-Xprofile:wallclock")) {
        String noWall;
        StringUtils::Replace(props, "-Xprofile:wallclock", "", &noWall);
        props = noWall.Trim();
        sysProp->Set(String("dalvik.vm.extra-opts"), props);
    }
}

ECode Am::MakeIntent(// throws URISyntaxException
    /* [in] */ Int32 defUser,
    /* [out] */ IIntent** newIntent)
{
    VALIDATE_NOT_NULL(newIntent);
    *newIntent = NULL;

    AutoPtr<IIntent> intent;
    CIntent::New((IIntent**)&intent);
    AutoPtr<IIntent> baseIntent = intent;
    Boolean hasIntentInfo = FALSE;

    AutoPtr<IUriHelper> urlHelper;
    CUriHelper::AcquireSingleton((IUriHelper**)&urlHelper);
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);

    mStartFlags = 0;
    mWaitOption = FALSE;
    mStopOption = FALSE;
    mRepeat = 0;
    mProfileFile = String(NULL);
    mSamplingInterval = 0;
    mAutoStop = FALSE;
    mUserId = defUser;
    AutoPtr<IUri> data;
    String type = String(NULL);

    String opt;
    while (!(NextOption(&opt), opt).IsNull()) {
        if (opt.Equals("-a")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            intent->SetAction(nextArgRequired);
            if (intent == baseIntent) {
                hasIntentInfo = TRUE;
            }
        }
        else if (opt.Equals("-d")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            data = NULL;
            urlHelper->Parse(nextArgRequired, (IUri**)&data);
            if (intent == baseIntent) {
                hasIntentInfo = TRUE;
            }
        }
        else if (opt.Equals("-t")) {
            NextArgRequired(&type);
            if (intent == baseIntent) {
                hasIntentInfo = TRUE;
            }
        }
        else if (opt.Equals("-c")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            intent->AddCategory(nextArgRequired);
            if (intent == baseIntent) {
                hasIntentInfo = TRUE;
            }
        }
        else if (opt.Equals("-e") || opt.Equals("--es")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            intent->PutExtra(key, value);
        }
        else if (opt.Equals("--esn")) {
            String key;
            NextArgRequired(&key);
            intent->PutExtra(key, String(NULL));
        }
        else if (opt.Equals("--ei")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            intent->PutExtra(key, StringUtils::ParseInt32(value));
        }
        else if (opt.Equals("--eu")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            AutoPtr<IUri> uri;
            urlHelper->Parse(value, (IUri**)&uri);
            intent->PutExtra(key, IParcelable::Probe(uri));
        }
        else if (opt.Equals("--ecn")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            AutoPtr<IComponentName> cn;
            helper->UnflattenFromString(value, (IComponentName**)&cn);
            if (cn == NULL) {
                Logger::E(TAG, "Bad component name: %s", value.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            intent->PutExtra(key, IParcelable::Probe(cn));
        }
        else if (opt.Equals("--eia")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            AutoPtr<IStringTokenizer> tok;
            CStringTokenizer::New(value, String(","), (IStringTokenizer**)&tok);
            Int32 tokens;
            tok->CountTokens(&tokens);
            AutoPtr<ArrayOf<Int32> > list = ArrayOf<Int32>::Alloc(tokens);
            if (list == NULL) return E_OUT_OF_MEMORY;
            for (Int32 i = 0; i < tokens; i++) {
                String token;
                tok->GetNextToken(&token);
                (*list)[i] = StringUtils::ParseInt32(token);
            }
            intent->PutExtra(key, list);
        }
        else if (opt.Equals("--el")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            intent->PutExtra(key, StringUtils::ParseInt64(value));
        }
        else if (opt.Equals("--ela")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            AutoPtr<IStringTokenizer> tok;
            CStringTokenizer::New(value, String(","), (IStringTokenizer**)&tok);
            Int32 tokens;
            tok->CountTokens(&tokens);
            AutoPtr<ArrayOf<Int64> > list = ArrayOf<Int64>::Alloc(tokens);
            if (list == NULL) return E_OUT_OF_MEMORY;
            for (Int32 i = 0; i < tokens; i++) {
                String token;
                tok->GetNextToken(&token);
                (*list)[i] = StringUtils::ParseInt64(token);
            }
            intent->PutExtra(key, list);
            hasIntentInfo = TRUE;
        }
        else if (opt.Equals("--ef")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            intent->PutExtra(key, StringUtils::ParseFloat(value));
            hasIntentInfo = TRUE;
        }
        else if (opt.Equals("--efa")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            AutoPtr<IStringTokenizer> tok;
            CStringTokenizer::New(value, String(","), (IStringTokenizer**)&tok);
            Int32 tokens;
            tok->CountTokens(&tokens);
            AutoPtr<ArrayOf<Float> > list = ArrayOf<Float>::Alloc(tokens);
            if (list == NULL) return E_OUT_OF_MEMORY;
            for (Int32 i = 0; i < tokens; i++) {
                String token;
                tok->GetNextToken(&token);
                (*list)[i] = StringUtils::ParseFloat(token);
            }
            intent->PutExtra(key, list);
            hasIntentInfo = TRUE;
        }
        else if (opt.Equals("--esa")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            // Split on commas unless they are preceeded by an escape.
            // The escape character must be escaped for the string and
            // again for the regex, thus four escape characters become one.
            AutoPtr<IStringTokenizer> tok;
            CStringTokenizer::New(value, String("(?<!\\\\),"), (IStringTokenizer**)&tok);
            Int32 tokens;
            tok->CountTokens(&tokens);
            AutoPtr<ArrayOf<String> > list = ArrayOf<String>::Alloc(tokens);
            if (list == NULL) return E_OUT_OF_MEMORY;
            for (Int32 i = 0; i < tokens; i++) {
                tok->GetNextToken(&(*list)[i]);
            }
            intent->PutExtra(key, list);
            hasIntentInfo = TRUE;
        }
        else if (opt.Equals("--ez")) {
            String key, value;
            NextArgRequired(&key);
            NextArgRequired(&value);
            intent->PutBooleanExtra(key, value.EqualsIgnoreCase("true"));
        }
        else if (opt.Equals("-n")) {
            String str;
            NextArgRequired(&str);
            AutoPtr<IComponentName> cn;
            helper->UnflattenFromString(str, (IComponentName**)&cn);
            if (cn == NULL) {
                Logger::E(TAG, "Bad component name: %s", str.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            intent->SetComponent(cn);
            if (intent == baseIntent) {
                hasIntentInfo = TRUE;
            }
        }
        else if (opt.Equals("-f")) {
            String str;
            NextArgRequired(&str);
            Int32 flag;
            intent->SetFlags((StringToIntegral::Decode(str, &flag), flag));
        }
        else if (opt.Equals("--grant-read-uri-permission")) {
            intent->AddFlags(IIntent::FLAG_GRANT_READ_URI_PERMISSION);
        }
        else if (opt.Equals("--grant-write-uri-permission")) {
            intent->AddFlags(IIntent::FLAG_GRANT_WRITE_URI_PERMISSION);
        }
        else if (opt.Equals("--grant-persistable-uri-permission")) {
            intent->AddFlags(IIntent::FLAG_GRANT_PERSISTABLE_URI_PERMISSION);
        }
        else if (opt.Equals("--grant-prefix-uri-permission")) {
            intent->AddFlags(IIntent::FLAG_GRANT_PREFIX_URI_PERMISSION);
        }
        else if (opt.Equals("--exclude-stopped-packages")) {
            intent->AddFlags(IIntent::FLAG_EXCLUDE_STOPPED_PACKAGES);
        }
        else if (opt.Equals("--include-stopped-packages")) {
            intent->AddFlags(IIntent::FLAG_INCLUDE_STOPPED_PACKAGES);
        }
        else if (opt.Equals("--debug-log-resolution")) {
            intent->AddFlags(IIntent::FLAG_DEBUG_LOG_RESOLUTION);
        }
        else if (opt.Equals("--activity-brought-to-front")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_BROUGHT_TO_FRONT);
        }
        else if (opt.Equals("--activity-clear-top")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TOP);
        }
        else if (opt.Equals("--activity-clear-when-task-reset")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET);
        }
        else if (opt.Equals("--activity-exclude-from-recents")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_EXCLUDE_FROM_RECENTS);
        }
        else if (opt.Equals("--activity-launched-from-history")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_LAUNCHED_FROM_HISTORY);
        }
        else if (opt.Equals("--activity-multiple-task")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_MULTIPLE_TASK);
        }
        else if (opt.Equals("--activity-no-animation")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NO_ANIMATION);
        }
        else if (opt.Equals("--activity-no-history")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NO_HISTORY);
        }
        else if (opt.Equals("--activity-no-user-action")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_NO_USER_ACTION);
        }
        else if (opt.Equals("--activity-previous-is-top")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_PREVIOUS_IS_TOP);
        }
        else if (opt.Equals("--activity-reorder-to-front")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_REORDER_TO_FRONT);
        }
        else if (opt.Equals("--activity-reset-task-if-needed")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
        }
        else if (opt.Equals("--activity-single-top")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_SINGLE_TOP);
        }
        else if (opt.Equals("--activity-clear-task")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TASK);
        }
        else if (opt.Equals("--activity-task-on-home")) {
            intent->AddFlags(IIntent::FLAG_ACTIVITY_TASK_ON_HOME);
        }
        else if (opt.Equals("--receiver-registered-only")) {
            intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY);
        }
        else if (opt.Equals("--receiver-replace-pending")) {
            intent->AddFlags(IIntent::FLAG_RECEIVER_REPLACE_PENDING);
        }
        else if (opt.Equals("--selector")) {
            intent->SetDataAndType(data, type);
            intent = NULL;
            CIntent::New((IIntent**)&intent);
        }
        else if (opt.Equals("-D")) {
            mStartFlags |= IActivityManager::START_FLAG_DEBUG;
        }
        else if (opt.Equals("-W")) {
            mWaitOption = TRUE;
        }
        else if (opt.Equals("-P")) {
            NextArgRequired(&mProfileFile);
            mAutoStop = TRUE;
        }
        else if (opt.Equals("--start-profiler")) {
            NextArgRequired(&mProfileFile);
            mAutoStop = FALSE;
        }
        else if (opt.Equals("--sampling")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            mSamplingInterval = StringUtils::ParseInt32(nextArgRequired);
        }
        else if (opt.Equals("-R")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            mRepeat = StringUtils::ParseInt32(nextArgRequired);
        }
        else if (opt.Equals("-S")) {
            mStopOption = TRUE;
        }
        else if (opt.Equals("--opengl-trace")) {
            mStartFlags |= IActivityManager::START_FLAG_OPENGL_TRACES;
        }
        else if (opt.Equals("--user")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            mUserId = ParseUserArg(nextArgRequired);
        }
        else if (opt.Equals("--receiver-permission")) {
            NextArgRequired(&mReceiverPermission);
        }
        else {
            System::Err->Println(String("Error: Unknown option: ") + opt);
            return NOERROR;
        }
    }
    intent->SetDataAndType(data, type);

    const Boolean hasSelector = intent != baseIntent;
    if (hasSelector) {
        // A selector was specified; fix up.
        baseIntent->SetSelector(intent);
        intent = baseIntent;
    }

    String arg;
    NextArg(&arg);
    baseIntent = NULL;
    if (arg.IsNull()) {
        if (hasSelector) {
            // If a selector has been specified, and no arguments
            // have been supplied for the main Intent, then we can
            // assume it is ACTION_MAIN CATEGORY_LAUNCHER; we don't
            // need to have a component name specified yet, the
            // selector will take care of that.
            CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&baseIntent);
            baseIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
        }
    }
    else if (arg.IndexOf(':') >= 0) {
        // The argument is a URI.  Fully parse it, and use that result
        // to fill in any data not specified so far.
        AutoPtr<IIntentHelper> helper;
        CIntentHelper::AcquireSingleton((IIntentHelper**)&helper);
        helper->ParseUri(arg, IIntent::URI_INTENT_SCHEME, (IIntent**)&baseIntent);
    }
    else if (arg.IndexOf('/') >= 0) {
        // The argument is a component name.  Build an Intent to launch
        // it.
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&baseIntent);
        baseIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
        AutoPtr<IComponentName> cn;
        helper->UnflattenFromString(arg, (IComponentName**)&cn);
        baseIntent->SetComponent(cn);
    }
    else {
        // Assume the argument is a package name.
        CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&baseIntent);
        baseIntent->AddCategory(IIntent::CATEGORY_LAUNCHER);
        baseIntent->SetPackage(arg);
    }

    if (baseIntent != NULL) {
        AutoPtr<IBundle> extras;
        intent->GetExtras((IBundle**)&extras);
        intent->ReplaceExtras((IBundle*)NULL);
        AutoPtr<IBundle> uriExtras;
        baseIntent->GetExtras((IBundle**)&uriExtras);
        baseIntent->ReplaceExtras((IBundle*)NULL);
        String action;
        AutoPtr<ArrayOf<String> > categories;
        if (!(intent->GetAction(&action), action).IsNull() &&
                (baseIntent->GetCategories((ArrayOf<String>**)&categories), categories) != NULL) {
            Int32 length = categories->GetLength();
            for (Int32 i = 0; i < length; ++i) {
                baseIntent->RemoveCategory((*categories)[i]);
            }
        }
        Int32 result;
        intent->FillIn(baseIntent, IIntent::FILL_IN_COMPONENT | IIntent::FILL_IN_SELECTOR,
            &result);
        if (extras == NULL) {
            extras = uriExtras;
        }
        else if (uriExtras != NULL) {
            uriExtras->PutAll(extras);
            extras = uriExtras;
        }
        intent->ReplaceExtras(extras);
        hasIntentInfo = TRUE;
    }

    if (!hasIntentInfo) {
        Logger::E(TAG, "No intent supplied");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    *newIntent = intent;
    REFCOUNT_ADD(*newIntent);
    return NOERROR;
}

ECode Am::RunStartService()
{
    AutoPtr<IIntent> intent;
    FAIL_RETURN(MakeIntent(IUserHandle::USER_CURRENT, (IIntent**)&intent));
    if (mUserId == IUserHandle::USER_ALL) {
        System::Err->Println(String("Error: Can't start activity with user 'all'"));
        return NOERROR;
    }

    String intentString;
    intent->ToString(&intentString);
    System::Out->Println(String("Starting service: ") + intentString);

    String type;
    intent->GetType(&type);
    AutoPtr<IComponentName> cn;
    FAIL_RETURN(mAm->StartService(NULL, intent, type, mUserId, (IComponentName**)&cn));
    if (cn == NULL) {
        System::Err->Println(String("Error: Not found; no service started."));
    }
    else {
        String packageName;
        String className;
        cn->GetPackageName(&packageName);
        cn->GetClassName(&className);
        if (packageName.Equals("!")) {
            System::Err->Println(String("Error: Requires permission ") + className);
        }
        else if (packageName.Equals("!!")) {
            System::Err->Println(String("Error: ") + className);
        }
    }
    return NOERROR;
}

ECode Am::RunStopService()
{
    AutoPtr<IIntent> intent;
    FAIL_RETURN(MakeIntent(IUserHandle::USER_CURRENT, (IIntent**)&intent));
    if (mUserId == IUserHandle::USER_ALL) {
        System::Err->Println(String("Error: Can't stop activity with user 'all'"));
        return NOERROR;
    }

    String intentString;
    intent->ToString(&intentString);
    System::Out->Println(String("Stopping service: ") + intentString);

    String type;
    intent->GetType(&type);
    Int32 result;
    FAIL_RETURN(mAm->StopService(NULL, intent, type, mUserId, &result));
    if (result == 0) {
        System::Err->Println(String("Service not stopped: was not running."));
    }
    else if (result == 1) {
        System::Err->Println(String("Service stopped"));
    }
    else if (result == -1) {
        System::Err->Println(String("Error stopping service"));
    }
    return NOERROR;
}

ECode Am::RunStart()
{
    AutoPtr<IIntent> intent;
    FAIL_RETURN(MakeIntent(IUserHandle::USER_CURRENT, (IIntent**)&intent));

    if (mUserId == IUserHandle::USER_ALL) {
        System::Err->Println(String("Error: Can't start service with user 'all'"));
        return NOERROR;
    }

    String mimeType;
    intent->GetType(&mimeType);
    if (mimeType.IsNull()) {
        AutoPtr<IUri> data;
        intent->GetData((IUri**)&data);
        if (data != NULL) {
            String scheme;
            data->GetScheme(&scheme);
            if (scheme.Equals("content")) {
                FAIL_RETURN(mAm->GetProviderMimeType(data, mUserId, &mimeType));
            }
        }
    }

    do {
        String intentString;
        intent->ToString(&intentString);
        if (mStopOption) {
            String packageName;
            AutoPtr<IComponentName> component;
            intent->GetComponent((IComponentName**)&component);
            if (component != NULL) {
                component->GetPackageName(&packageName);
            }
            else {
                AutoPtr<IServiceManager> sm;
                FAIL_RETURN(CServiceManager::AcquireSingleton((IServiceManager**)&sm));
                AutoPtr<IInterface> base;
                FAIL_RETURN(sm->GetService(String("package"), (IInterface**)&base));
                AutoPtr<IIPackageManager> pm = IIPackageManager::Probe(base);
                if (pm == NULL) {
                    System::Err->Println(String("Error: Package manager not running; aborting"));
                    return NOERROR;
                }

                AutoPtr<IList> activities;
                pm->QueryIntentActivities(intent, mimeType, 0, mUserId, (IList**)&activities);
                Int32 size = 0;
                if (activities == NULL || (activities->GetSize(&size), size) <= 0) {
                    System::Err->Println(String("Error: Intent does not match any activities: ") + intentString);
                    return NOERROR;
                }
                else if (size > 1) {
                    System::Err->Println(String("Error: Intent matches multiple activities; can't stop: ") + intentString);
                    return NOERROR;
                }

                AutoPtr<IInterface> ri;
                activities->Get(0, (IInterface**)&ri);
                ri = IResolveInfo::Probe(ri);
                AutoPtr<IActivityInfo> activityInfo;
                ri->GetActivityInfo((IActivityInfo**)&activityInfo);
                IPackageItemInfo::Probe(activityInfo)->GetPackageName(&packageName);
            }
            System::Out->Println(String("Stopping: ") + packageName);
            FAIL_RETURN(mAm->ForceStopPackage(packageName, mUserId));
            Thread::Sleep(250);
        }

        System::Out->Println(String("Starting: ") + intentString);
        intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

        AutoPtr<IParcelFileDescriptor> fd;
        AutoPtr<IProfilerInfo> profilerInfo;

        if (!mProfileFile.IsNull()) {
            // try {
            AutoPtr<IParcelFileDescriptorHelper> parcelHelper;
            FAIL_RETURN(CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&parcelHelper));
            AutoPtr<IFile> file;
            FAIL_RETURN(CFile::New(mProfileFile, (IFile**)&file));
            ECode ec = parcelHelper->Open(file,
                    IParcelFileDescriptor::MODE_CREATE |
                    IParcelFileDescriptor::MODE_TRUNCATE |
                    IParcelFileDescriptor::MODE_READ_WRITE,
                    (IParcelFileDescriptor**)&fd);
            // } catch (FileNotFoundException e) {
            if (ec == (ECode)E_NOT_FOUND_EXCEPTION) {
                System::Err->Println(String("Error: Unable to open file: ") + mProfileFile);
                return NOERROR;
            }
            if (FAILED(ec)) return ec;
            // }
            CProfilerInfo::New(mProfileFile, fd, mSamplingInterval, mAutoStop, (IProfilerInfo**)&profilerInfo);
        }

        AutoPtr<IActivityManagerWaitResult> result;
        Int32 res;
        Int64 startTime = SystemClock::GetUptimeMillis();
        if (mWaitOption) {
            FAIL_RETURN(mAm->StartActivityAndWait(NULL, String(NULL), intent, mimeType,
                    NULL, String(NULL), 0, mStartFlags, profilerInfo, NULL, mUserId,
                    (IActivityManagerWaitResult**)&result));
            result->GetResult(&res);
        }
        else {
            FAIL_RETURN(mAm->StartActivityAsUser(NULL, String(NULL), intent, mimeType,
                    NULL, String(NULL), 0, mStartFlags, profilerInfo, NULL, mUserId, &res));
        }
        Int64 endTime = SystemClock::GetUptimeMillis();

        AutoPtr<IPrintStream> out = mWaitOption ? System::Out : System::Err;
        Boolean launched = FALSE;
        switch (res) {
            case IActivityManager::START_SUCCESS:
                launched = TRUE;
                break;
            case IActivityManager::START_SWITCHES_CANCELED:
                launched = TRUE;
                out->Println(String(
                        "Warning: Activity not started because the "
                        "current activity is being kept for the user."));
                break;
            case IActivityManager::START_DELIVERED_TO_TOP:
                launched = TRUE;
                out->Println(String(
                        "Warning: Activity not started, intent has "
                        "been delivered to currently running "
                        "top-most instance."));
                break;
            case IActivityManager::START_RETURN_INTENT_TO_CALLER:
                launched = TRUE;
                out->Println(String(
                        "Warning: Activity not started because intent "
                        "should be handled by the caller"));
                break;
            case IActivityManager::START_TASK_TO_FRONT:
                launched = TRUE;
                out->Println(String(
                        "Warning: Activity not started, its current "
                        "task has been brought to the front"));
                break;
            case IActivityManager::START_INTENT_NOT_RESOLVED:
                out->Println(String(
                        "Error: Activity not started, unable to "
                        "resolve ") + intentString);
                break;
            case IActivityManager::START_CLASS_NOT_FOUND:
            {
                out->Println(NO_CLASS_ERROR_CODE);
                AutoPtr<IComponentName> component;
                intent->GetComponent((IComponentName**)&component);
                String name;
                component->ToShortString(&name);
                out->Println(String("Error: Activity class ") +
                        name
                        + String(" does not exist."));
                break;
            }
            case IActivityManager::START_FORWARD_AND_REQUEST_CONFLICT:
                out->Println(String(
                        "Error: Activity not started, you requested to "
                        "both forward and receive its result"));
                break;
            case IActivityManager::START_PERMISSION_DENIED:
                out->Println(String(
                        "Error: Activity not started, you do not "
                        "have permission to access it."));
                break;
            case IActivityManager::START_NOT_VOICE_COMPATIBLE:
                out->Println(String(
                        "Error: Activity not started, voice control not allowed for: ")
                                + intentString);
                break;
            default:
                out->Println(String(
                        "Error: Activity not started, unknown error code ") + StringUtils::ToString(res));
                break;
        }

        if (mWaitOption && launched) {
            AutoPtr<IComponentName> who;
            if (result == NULL) {
                CActivityManagerWaitResult::New((IActivityManagerWaitResult**)&result);
                intent->GetComponent((IComponentName**)&who);
                result->SetWho(who);
            }

            Boolean timeout;
            System::Out->Println(String("Status: ") + String((result->GetTimeout(&timeout), timeout) ? "timeout" : "ok"));
            who = NULL;
            result->GetWho((IComponentName**)&who);
            if (who != NULL) {
                String shortString;
                System::Out->Println(String("Activity: ") + (who->FlattenToShortString(&shortString), shortString));
            }

            Int64 thisTime;
            if ((result->GetThisTime(&thisTime), thisTime) >= 0) {
                System::Out->Println(String("ThisTime: ") + StringUtils::ToString(thisTime));
            }

            Int64 totalTime;
            if ((result->GetTotalTime(&totalTime), totalTime) >= 0) {
                System::Out->Println(String("TotalTime: ") + StringUtils::ToString(totalTime));
            }
            System::Out->Println(String("WaitTime: ") + StringUtils::ToString(endTime - startTime));
            System::Out->Println(String("Complete"));
        }

        mRepeat--;
        if (mRepeat > 1) {
            FAIL_RETURN(mAm->UnhandledBack());
        }
    }
    while (mRepeat > 1);

    return NOERROR;
}

ECode Am::RunForceStop()
{
    Int32 userId = IUserHandle::USER_ALL;

    String opt;
    while (NextOption(&opt), !opt.IsNull()) {
        if (opt.Equals("--user")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            userId = ParseUserArg(nextArgRequired);
        }
        else {
            System::Err->Println(String("Error: Unknown option: ") + opt);
            return NOERROR;
        }
    }

    String nextArgRequired;
    NextArgRequired(&nextArgRequired);
    return mAm->ForceStopPackage(nextArgRequired, userId);
}

ECode Am::RunKill()
{
    Int32 userId = IUserHandle::USER_ALL;

    String opt;
    while (NextOption(&opt), !opt.IsNull()) {
        if (opt.Equals("--user")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            userId = ParseUserArg(nextArgRequired);
        }
        else {
            System::Err->Println(String("Error: Unknown option: ") + opt);
            return NOERROR;
        }
    }

    String nextArgRequired;
    NextArgRequired(&nextArgRequired);
    return mAm->KillBackgroundProcesses(nextArgRequired, userId);
}

ECode Am::RunKillAll()
{
    return mAm->KillAllBackgroundProcesses();
}

ECode Am::SendBroadcast()
{
    AutoPtr<IIntent> intent;
    FAIL_RETURN(MakeIntent(IUserHandle::USER_CURRENT, (IIntent**)&intent));
    AutoPtr<IIntentReceiver> receiver;
    CIntentReceiver::New((IIntentReceiver**)&receiver);
    String intentString;
    intent->ToString(&intentString);
    System::Out->Println(String("Broadcasting: ") + intentString);
    Int32 result;
    FAIL_RETURN(mAm->BroadcastIntent(NULL, intent, String(NULL), receiver, 0,
            String(NULL), NULL, mReceiverPermission,
            IAppOpsManager::OP_NONE, TRUE, FALSE, mUserId, &result));
    return ((CIntentReceiver*)receiver.Get())->WaitForFinish();
}

ECode Am::RunInstrument()
{
    String profileFile;
    Boolean wait = FALSE;
    Boolean rawMode = FALSE;
    Boolean no_window_animation = FALSE;
    Int32 userId = IUserHandle::USER_CURRENT;
    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    String argKey, argValue;
    AutoPtr<IServiceManager> sm;
    AutoPtr<IInterface> base;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    sm->GetService(String("window"), (IInterface**)&base);
    AutoPtr<IIWindowManager> wm = IIWindowManager::Probe(base);
    String abi;

    String opt;
    while (NextOption(&opt), !opt.IsNull()) {
        if (opt.Equals("-p")) {
            NextArgRequired(&profileFile);
        }
        else if (opt.Equals("-w")) {
            wait = TRUE;
        }
        else if (opt.Equals("-r")) {
            rawMode = TRUE;
        }
        else if (opt.Equals("-e")) {
            NextArgRequired(&argKey);
            NextArgRequired(&argValue);
            args->PutString(argKey, argValue);
        }
        else if (opt.Equals("--no_window_animation")
                || opt.Equals("--no-window-animation")) {
            no_window_animation = TRUE;
        }
        else if (opt.Equals("--user")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            userId = ParseUserArg(nextArgRequired);
        }
        else if (opt.Equals("--abi")) {
            NextArgRequired(&abi);
        }
        else {
            System::Err->Println(String("Error: Unknown option: ") + opt);
            return NOERROR;
        }
    }

    if (userId == IUserHandle::USER_ALL) {
        System::Err->Println(String("Error: Can't start instrumentation with user 'all'"));
        return NOERROR;
    }

    String cnArg;
    NextArgRequired(&cnArg);
    AutoPtr<IComponentName> cn;
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    helper->UnflattenFromString(cnArg, (IComponentName**)&cn);
    if (cn == NULL) {
        Logger::E(TAG, "Bad component name: %s", cnArg.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInstrumentationWatcher> watcher;
    AutoPtr<IIUiAutomationConnection> connection;
    if (wait) {
        CInstrumentationWatcher::New((IInstrumentationWatcher**)&watcher);
        ((CInstrumentationWatcher*)watcher.Get())->SetRawOutput(rawMode);
        CUiAutomationConnection::New((IIUiAutomationConnection**)&connection);
    }

    AutoPtr<ArrayOf<Float> > oldAnims;
    if (no_window_animation) {
        wm->GetAnimationScales((ArrayOf<Float>**)&oldAnims);
        wm->SetAnimationScale(0, 0.0f);
        wm->SetAnimationScale(1, 0.0f);
    }

    if (abi != NULL) {
        AutoPtr<ArrayOf<String> > supportedAbis = Build::SUPPORTED_ABIS;
        Boolean matched = FALSE;
        for (Int32 i = 0; i < supportedAbis->GetLength(); i++) {
            if ((*supportedAbis)[i].Equals(abi)) {
                matched = TRUE;
                break;
            }
        }

        if (!matched) {
            Logger::E(TAG, "INSTRUMENTATION_FAILED: Unsupported instruction set  %s", abi.string());
            return E_ANDROID_EXCEPTION;
        }
    }

    Boolean result;
    FAIL_RETURN(mAm->StartInstrumentation(cn, profileFile, 0, args, watcher, connection, userId, abi, &result));
    if (!result) {
        String cnString;
        cn->FlattenToString(&cnString);
        Logger::E(TAG, "INSTRUMENTATION_FAILED: %s", cnString.string());
        return E_ANDROID_EXCEPTION;
    }

    if (watcher != NULL) {
        if (!(((CInstrumentationWatcher*)watcher.Get())->WaitForFinish(&result), result)) {
            System::Out->Println(String("INSTRUMENTATION_ABORTED: System has crashed."));
        }
    }

    if (oldAnims != NULL) {
        wm->SetAnimationScales(oldAnims);
    }

    return NOERROR;
}

ECode Am::RunProfile()
{
    String profileFile;
    Boolean start = FALSE;
    Boolean wall = FALSE;
    Int32 userId = IUserHandle::USER_CURRENT;
    Int32 profileType = 0;

    String process;

    String cmd;
    NextArgRequired(&cmd);

    if (cmd.Equals("start")) {
        start = TRUE;
        String opt;
        while (NextOption(&opt), !opt.IsNull()) {
            if (opt.Equals("--user")) {
                String userArg;
                NextArgRequired(&userArg);
                userId = ParseUserArg(userArg);
            }
            else if (opt.Equals("--wall")) {
                wall = TRUE;
            }
            else {
                System::Err->Println(String("Error: Unknown option: ") + opt);
                return NOERROR;
            }
        }
        NextArgRequired(&process);
    }
    else if (cmd.Equals("stop")) {
        String opt;
        while (NextOption(&opt), !opt.IsNull()) {
            if (opt.Equals("--user")) {
                String userArg;
                NextArgRequired(&userArg);
                userId = ParseUserArg(userArg);
            }
            else {
                System::Err->Println(String("Error: Unknown option: ") + opt);
                return NOERROR;
            }
        }
        NextArg(&process);
    }
    else {
        // Compatibility with old syntax: process is specified first.
        process = cmd;
        NextArgRequired(&cmd);
        if (cmd.Equals("start")) {
            start = TRUE;
        }
        else if (!cmd.Equals("stop")) {
            Logger::E(TAG, "Profile command %s not valid", process.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    if (userId == IUserHandle::USER_ALL) {
        System::Err->Println(String("Error: Can't profile with user 'all'"));
        return NOERROR;
    }

    AutoPtr<IParcelFileDescriptor> fd;
    AutoPtr<IProfilerInfo> profilerInfo;
    if (start) {
        NextArgRequired(&profileFile);
        // try {
        AutoPtr<IParcelFileDescriptorHelper> parcelHelper;
        FAIL_RETURN(CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&parcelHelper));
        AutoPtr<IFile> file;
        FAIL_RETURN(CFile::New(profileFile, (IFile**)&file));
        ECode ec = parcelHelper->Open(file,
                IParcelFileDescriptor::MODE_CREATE |
                IParcelFileDescriptor::MODE_TRUNCATE |
                IParcelFileDescriptor::MODE_READ_WRITE,
                (IParcelFileDescriptor**)&fd);
        // } catch (FileNotFoundException e) {
        if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
            System::Err->Println(String("Error: Unable to open file: ") + profileFile);
            return NOERROR;
        }
        if (FAILED(ec)) return ec;
        // }

        CProfilerInfo::New(mProfileFile, fd, 0, FALSE, (IProfilerInfo**)&profilerInfo);
    }

    // try {
    if (wall) {
        // XXX doesn't work -- this needs to be set before booting.
        AutoPtr<ISystemProperties> sysProp;
        FAIL_RETURN(CSystemProperties::AcquireSingleton((ISystemProperties**)&sysProp));
        String props;
        sysProp->Get(String("dalvik.vm.extra-opts"), &props);
        if (props.IsNull() || !props.Contains("-Xprofile:wallclock")) {
            props = props + " -Xprofile:wallclock";
            // sysProp->Set("dalvik.vm.extra-opts", props);
        }
    }
    else if (start) {
        // RemoveWallOption();
    }

    Boolean result;
    FAIL_RETURN(mAm->ProfileControl(process, userId, start, profilerInfo, profileType, &result));
    if (!result) {
        wall = FALSE;
        Logger::E("PROFILE FAILED on process %s", process.string());
        return E_ANDROID_EXCEPTION;
    }

    // } finally {
    if (!wall) {
        // RemoveWallOption();
    }
    // }

    return NOERROR;
}

ECode Am::RunDumpHeap()
{
    Boolean managed = TRUE;
    Int32 userId = IUserHandle::USER_CURRENT;

    String opt;
    while (NextOption(&opt), !opt.IsNull()) {
        if (opt.Equals("--user")) {
            String nextArgRequired;
            NextArgRequired(&nextArgRequired);
            userId = ParseUserArg(nextArgRequired);
            if (userId == IUserHandle::USER_ALL) {
                System::Err->Println(String("Error: Can't dump heap with user 'all'"));
                return NOERROR;
            }
        }
        else if (opt.Equals("-n")) {
            managed = FALSE;
        }
        else {
            System::Err->Println(String("Error: Unknown option: ") + opt);
            return NOERROR;
        }
    }

    String process, heapFile;
    NextArgRequired(&process);
    NextArgRequired(&heapFile);
    AutoPtr<IParcelFileDescriptor> fd;

    // try {
    AutoPtr<IFile> file;
    FAIL_RETURN(CFile::New(heapFile, (IFile**)&file));
    Boolean isDelete;
    file->Delete(&isDelete);
    AutoPtr<IParcelFileDescriptorHelper> parcelHelper;
    FAIL_RETURN(CParcelFileDescriptorHelper::AcquireSingleton((IParcelFileDescriptorHelper**)&parcelHelper));
    ECode ec = parcelHelper->Open(file,
            IParcelFileDescriptor::MODE_CREATE |
            IParcelFileDescriptor::MODE_TRUNCATE |
            IParcelFileDescriptor::MODE_READ_WRITE,
            (IParcelFileDescriptor**)&fd);
    // } catch (FileNotFoundException e) {
    if (ec == (ECode)E_FILE_NOT_FOUND_EXCEPTION) {
        System::Err->Println(String("Error: Unable to open file: ") + heapFile);
        return NOERROR;
    }
    if (FAILED(ec)) return ec;
    // }

    Boolean result;
    FAIL_RETURN(mAm->DumpHeap(process, userId, managed, heapFile, fd, &result));
    if (!result) {
        Logger::E("HEAP DUMP FAILED on process %s", process.string());
        return E_ANDROID_EXCEPTION;
    }

    return NOERROR;
}

ECode Am::RunSetDebugApp()
{
    Boolean wait = FALSE;
    Boolean persistent = FALSE;

    String opt;
    while (NextOption(&opt), !opt.IsNull()) {
        if (opt.Equals("-w")) {
            wait = TRUE;
        }
        else if (opt.Equals("--persistent")) {
            persistent = TRUE;
        }
        else {
            System::Err->Println(String("Error: Unknown option: ") + opt);
            return NOERROR;
        }
    }

    String pkg;
    NextArgRequired(&pkg);
    return mAm->SetDebugApp(pkg, wait, persistent);
}

ECode Am::RunClearDebugApp()
{
    return mAm->SetDebugApp(String(NULL), FALSE, TRUE);
}

ECode Am::RunBugReport()
{
    ECode ec = mAm->RequestBugReport();
    System::Out->Println(String("Your lovely bug report is being created; please be patient."));
    return ec;
}

ECode Am::RunSwitchUser()
{
    String user;
    NextArgRequired(&user);
    Boolean result;
    return mAm->SwitchUser(StringUtils::ParseInt32(user), &result);
}

ECode Am::RunStartUserInBackground()
{
    String user;
    NextArgRequired(&user);
    Boolean success;
    FAIL_RETURN(mAm->StartUserInBackground(StringUtils::ParseInt32(user), &success));
    if (success) {
        System::Out->Println(String("Success: user started"));
    }
    else {
        System::Err->Println(String("Error: could not start user"));
    }
    return NOERROR;
}

ECode Am::RunStopUser()
{
    String user;
    NextArgRequired(&user);
    Int32 res;
    FAIL_RETURN(mAm->StopUser(StringUtils::ParseInt32(user), NULL, &res));
    if (res != IActivityManager::USER_OP_SUCCESS) {
        String txt("");
        switch (res) {
            case IActivityManager::USER_OP_IS_CURRENT:
                txt = " (Can't stop current user)";
                break;
            case IActivityManager::USER_OP_UNKNOWN_USER:
                txt = String(" (Unknown user ") + user + ")";
                break;
        }
        System::Err->Println(String("Switch failed: ") + StringUtils::ToString(res) + txt);
    }
    return NOERROR;
}

ECode Am::RunMonitor()
{
    String opt;
    String gdbPort;
    while (NextOption(&opt), !opt.IsNull()) {
        if (opt.Equals("--gdb")) {
            NextArgRequired(&gdbPort);
        }
        else {
            System::Err->Println(String("Error: Unknown option: ") + opt);
            return NOERROR;
        }
    }

    AutoPtr<IIActivityController> controller;
    CMyActivityController::New(mAm, gdbPort, (IIActivityController**)&controller);
    return ((CMyActivityController*)controller.Get())->Run();
}

ECode Am::RunHang()
{
    String opt;
    Boolean allowRestart = FALSE;
    while (NextOption(&opt), !opt.IsNull()) {
        if (opt.Equals("--allow-restart")) {
            allowRestart = TRUE;
        }
        else {
            System::Err->Println(String("Error: Unknown option: ") + opt);
            return NOERROR;
        }
    }

    System::Out->Println(String("Hanging the system..."));
    AutoPtr<IBinder> binder;
    CBinder::New((IBinder**)&binder);
    return mAm->Hang(binder, allowRestart);
}

ECode Am::RunRestart()
{
    String opt;
    while (NextOption(&opt), !opt.IsNull()) {
        System::Err->Println(String("Error: Unknown option: ") + opt);
        return NOERROR;
    }

    System::Out->Println(String("Restart the system..."));
    return mAm->Restart();
}

ECode Am::RunIdleMaintenance()
{
    String opt;
    while (NextOption(&opt), !opt.IsNull()) {
        System::Err->Println(String("Error: Unknown option: ") + opt);
        return NOERROR;
    }

    System::Out->Println(String("Performing idle maintenance..."));
    AutoPtr<IIntent> intent;
    CIntent::New(String("com.elastos.server.task.controllers.IdleController.ACTION_TRIGGER_IDLE"), (IIntent**)&intent);
    Int32 result;
    return mAm->BroadcastIntent(NULL, intent, String(NULL), NULL, 0, String(NULL), NULL, String(NULL),
            IAppOpsManager::OP_NONE, TRUE, FALSE, IUserHandle::USER_ALL, &result);
}

ECode Am::RunScreenCompat()
{
    String mode;
    NextArgRequired(&mode);
    Boolean enabled;
    if (mode.Equals("on")) {
        enabled = TRUE;
    }
    else if (mode.Equals("off")) {
        enabled = FALSE;
    }
    else {
        System::Err->Println(String("Error: enabled mode must be 'on' or 'off' at ") + mode);
        return NOERROR;
    }

    String packageName;
    NextArgRequired(&packageName);
    do {
        // try {
        ECode ec = mAm->SetPackageScreenCompatMode(packageName, enabled
                ? IActivityManager::COMPAT_MODE_ENABLED
                : IActivityManager::COMPAT_MODE_DISABLED);
        // } catch (RemoteException e) {
        if (ec == (ECode)E_REMOTE_EXCEPTION) {
            return NOERROR;
        }
        if (FAILED(ec)) return ec;
        // }
        NextArg(&packageName);
    }
    while (!packageName.IsNull());

    return NOERROR;
}

ECode Am::RunToUri(
    /* [in] */ Boolean intentScheme)
{
    AutoPtr<IIntent> intent;
    FAIL_RETURN(MakeIntent(IUserHandle::USER_CURRENT, (IIntent**)&intent));
    String uri;
    intent->ToUri(intentScheme ? IIntent::URI_INTENT_SCHEME : 0, &uri);
    System::Out->Println(uri);
    return NOERROR;
}

ECode Am::RunStack()
{
    String op;
    NextArgRequired(&op);
    if (op.Equals("start")) {
        return RunStackStart();
    }
    else if (op.Equals("movetask")) {
        return RunStackMoveTask();
    }
    else if (op.Equals("resize")) {
        return RunStackResize();
    }
    else if (op.Equals("list")) {
        return RunStackList();
    }
    else if (op.Equals("info")) {
        return RunStackInfo();
    }
    else {
        ShowError(String("Error: unknown command '") + op + "'");
        return NOERROR;
    }
}

ECode Am::RunStackStart()
{
    String displayIdStr;
    NextArgRequired(&displayIdStr);
    Int32 displayId = StringUtils::ParseInt32(displayIdStr);
    AutoPtr<IIntent> intent;
    FAIL_RETURN(MakeIntent(IUserHandle::USER_CURRENT, (IIntent**)&intent));

    // try {
    AutoPtr<IBinder> homeActivityToken;
    AutoPtr<IIActivityContainer> container;
    ECode ec = mAm->GetHomeActivityToken((IBinder**)&homeActivityToken);
    if (FAILED(ec)) goto ErrorExit;
    ec = mAm->CreateActivityContainer(homeActivityToken, NULL, (IIActivityContainer**)&container);
    if (FAILED(ec)) goto ErrorExit;
    ec = container->AttachToDisplay(displayId);
    if (FAILED(ec)) goto ErrorExit;
    Int32 result;
    ec = container->StartActivity(intent, &result);
    if (FAILED(ec)) goto ErrorExit;
    // } catch (RemoteException e) {
    // }
    return NOERROR;

ErrorExit:
    return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
}

ECode Am::RunStackMoveTask()
{
    String taskIdStr;
    NextArgRequired(&taskIdStr);
    Int32 taskId = StringUtils::ParseInt32(taskIdStr);
    String stackIdStr;
    NextArgRequired(&stackIdStr);
    Int32 stackId = StringUtils::ParseInt32(stackIdStr);
    String toTopStr;
    NextArgRequired(&toTopStr);
    Boolean toTop;
    if (toTopStr.Equals("true")) {
        toTop = TRUE;
    }
    else if (toTopStr.Equals("false")) {
        toTop = FALSE;
    }
    else {
        System::Err->Println(String("Error: bad toTop arg: ") + toTopStr);
        return NOERROR;
    }

    // try {
    ECode ec = mAm->MoveTaskToStack(taskId, stackId, toTop);
    // } catch (RemoteException e) {
    // }
    return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
}

ECode Am::RunStackResize()
{
    String stackIdStr;
    NextArgRequired(&stackIdStr);
    Int32 stackId = StringUtils::ParseInt32(stackIdStr);
    String leftStr;
    NextArgRequired(&leftStr);
    Int32 left = StringUtils::ParseInt32(leftStr);
    String topStr;
    NextArgRequired(&topStr);
    Int32 top = StringUtils::ParseInt32(topStr);
    String rightStr;
    NextArgRequired(&rightStr);
    Int32 right = StringUtils::ParseInt32(rightStr);
    String bottomStr;
    NextArgRequired(&bottomStr);
    Int32 bottom = StringUtils::ParseInt32(bottomStr);

    // try {
    AutoPtr<IRect> rect;
    CRect::New(left, top, right, bottom, (IRect**)&rect);
    ECode ec = mAm->ResizeStack(stackId, rect);
    // } catch (RemoteException e) {
    // }
    return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
}

ECode Am::RunStackList()
{
    // try {
    AutoPtr<IList> stacks;
    ECode ec = mAm->GetAllStackInfos((IList**)&stacks);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
    }

    AutoPtr<IIterator> it;
    stacks->GetIterator((IIterator**)&it);
    Boolean hasNext = FALSE;
    String infoString;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> info;
        it->GetNext((IInterface**)&info);
        IObject::Probe(info)->ToString(&infoString);
        System::Out->Println(infoString);
    }
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

ECode Am::RunStackInfo()
{
    // try {
    String stackIdStr;
    NextArgRequired(&stackIdStr);
    Int32 stackId = StringUtils::ParseInt32(stackIdStr);
    AutoPtr<IActivityManagerStackInfo> info;
    ECode ec = mAm->GetStackInfo(stackId, (IActivityManagerStackInfo**)&info);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
    }
    String infoString;
    IObject::Probe(info)->ToString(&infoString);
    System::Out->Println(infoString);
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

ECode Am::RunLockTask()
{
    String taskIdStr;
    NextArgRequired(&taskIdStr);
    // try {
    ECode ec;
    if (taskIdStr.Equals("stop")) {
        ec = mAm->StopLockTaskMode();
    }
    else {
        Int32 taskId = StringUtils::ParseInt32(taskIdStr);
        ec = mAm->StartLockTaskMode(taskId);
    }
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
    }
    Boolean inLockTaskMode;
    ec = mAm->IsInLockTaskMode(&inLockTaskMode);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
    }
    System::Err->Println(String("Activity manager is ") + (inLockTaskMode ? "" : "not ") +
            "in lockTaskMode");
    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

AutoPtr<IList> Am::GetRecentConfigurations(
    /* [in] */ Int32 days)
{
    AutoPtr<IServiceManager> sm;
    AutoPtr<IInterface> base;
    CServiceManager::AcquireSingleton((IServiceManager**)&sm);
    sm->GetService(IContext::USAGE_STATS_SERVICE, (IInterface**)&base);
    AutoPtr<IIUsageStatsManager> usm = IIUsageStatsManager::Probe(base);

    AutoPtr<ISystem> system;
    CSystem::AcquireSingleton((ISystem**)&system);
    Int64 now;
    system->GetCurrentTimeMillis(&now);
    Int64 nDaysAgo = now - (days * 24 * 60 * 60 * 1000);
    // try {
    AutoPtr<ICollections> collections;
    CCollections::AcquireSingleton((ICollections**)&collections);
    AutoPtr<IList> emptyList;
    collections->GetEmptyList((IList**)&emptyList);

    AutoPtr<IParceledListSlice> configStatsSlice;
    ECode ec = usm->QueryConfigurationStats(
            IUsageStatsManager::INTERVAL_BEST, nDaysAgo, now, String("com.elastos.shell"),
            (IParceledListSlice**)&configStatsSlice);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
    }
    if (configStatsSlice == NULL) {
        return emptyList;
    }

    AutoPtr<IArrayMap> recentConfigs;
    ec = CArrayMap::New((IArrayMap**)&recentConfigs);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
    }
    AutoPtr<IList> configStatsList;
    ec = configStatsSlice->GetList((IList**)&configStatsList);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
    }
    Int32 configStatsListSize;
    ec = configStatsList->GetSize(&configStatsListSize);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
    }
    for (Int32 i = 0; i < configStatsListSize; i++) {
        AutoPtr<IInterface> iStats;
        ec = configStatsList->Get(i, (IInterface**)&iStats);
        if (FAILED(ec)) {
            return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
        }
        AutoPtr<IConfigurationStats> stats = IConfigurationStats::Probe(iStats);
        assert(stats != NULL);
        AutoPtr<IConfiguration> configuration;
        ec = stats->GetConfiguration((IConfiguration**)&configuration);
        if (FAILED(ec)) {
            return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
        }
        Int32 activationCount;
        ec = stats->GetActivationCount(&activationCount);
        if (FAILED(ec)) {
            return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
        }
        Int32 indexOfKey;
        ec = recentConfigs->GetIndexOfKey(configuration, &indexOfKey);
        if (FAILED(ec)) {
            return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
        }
        if (indexOfKey < 0) {
            ec = recentConfigs->Put(configuration, CoreUtils::Convert(activationCount));
        }
        else {
            AutoPtr<IInterface> value;
            ec = recentConfigs->GetValueAt(indexOfKey, (IInterface**)&value);
            if (FAILED(ec)) {
                return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
            }
            ec = recentConfigs->SetValueAt(indexOfKey,
                    CoreUtils::Convert(CoreUtils::Unbox(IInteger32::Probe(value)) + activationCount));
        }
        if (FAILED(ec)) {
            return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
        }
    }

    Int32 recentConfigsSize;
    ec = recentConfigs->GetSize(&recentConfigsSize);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
    }
    AutoPtr<IList> configs;
    ec = CArrayList::New(recentConfigsSize, (IList**)&configs);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
    }
    AutoPtr<ISet> keySet;
    ec = recentConfigs->GetKeySet((ISet**)&keySet);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
    }
    ec = configs->AddAll(ICollection::Probe(keySet));
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
    }
    AutoPtr<Comparator> comparator = new Comparator(recentConfigs);
    if (comparator == NULL) {
        return NULL;
    }
    ec = collections->Sort(configs, comparator);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? emptyList : NULL;
    }
    return configs;
    // } catch (RemoteException e) {
    //     return Collections.emptyList();
    // }
}

ECode Am::RunGetConfig()
{
    Int32 days = 14;
    String option;
    NextOption(&option);
    if (!option.IsNull()) {
        if (!option.Equals("--days")) {
            Logger::E(TAG, "unrecognized option %s", option.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        String daysString;
        NextArgRequired(&daysString);
        days = StringUtils::ParseInt32(daysString);
        if (days <= 0) {
            Logger::E(TAG, "--days must be a positive integer");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
    }

    // try {
    AutoPtr<IConfiguration> config;
    ECode ec = mAm->GetConfiguration((IConfiguration**)&config);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
    }
    if (config == NULL) {
        System::Err->Println(String("Activity manager has no configuration"));
        return NOERROR;
    }

    AutoPtr<IConfigurationHelper> configurationHelper;
    ec = CConfigurationHelper::AcquireSingleton((IConfigurationHelper**)&configurationHelper);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
    }
    String resourceQualifier;
    ec = configurationHelper->ResourceQualifierString(config, &resourceQualifier);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
    }
    System::Out->Println(String("config: ") + resourceQualifier);
    Int32 length = Build::SUPPORTED_ABIS->GetLength();
    AutoPtr<ArrayOf<IInterface*> > abis = ArrayOf<IInterface*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        abis->Set(i, TO_IINTERFACE(CoreUtils::Convert((*Build::SUPPORTED_ABIS)[i])));
    }
    System::Out->Println(String("abi: ") + TextUtils::Join(CoreUtils::Convert(String(",")), abis));

    AutoPtr<IList> recentConfigs = GetRecentConfigurations(days);
    Int32 recentConfigSize;
    recentConfigs->GetSize(&recentConfigSize);
    if (recentConfigSize > 0) {
        System::Out->Println(String("recentConfigs:"));
    }

    AutoPtr<ArrayOf<IInterface*> > array;
    ec = recentConfigs->ToArray((ArrayOf<IInterface*>**)&array);
    if (FAILED(ec)) {
        return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
    }
    for (Int32 i = 0; i < recentConfigSize; i++) {
        ec = configurationHelper->ResourceQualifierString(
                IConfiguration::Probe((*array)[i]), &resourceQualifier);
        if (FAILED(ec)) {
            return ec == (ECode)E_REMOTE_EXCEPTION ? NOERROR : ec;
        }
        System::Out->Println(String("  config: ") + resourceQualifier);
    }

    // } catch (RemoteException e) {
    // }
    return NOERROR;
}

}
}
}
}

