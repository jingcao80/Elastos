
#include "MyActivityController.h"
#include "CMyActivityController.h"

using Elastos::Core::ISystem;
using Elastos::IO::IInputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CInputStreamReader;
using Elastos::IO::CBufferedReader;
using Elastos::Droid::App::IIActivityController;
using Elastos::Droid::App::EIID_IIActivityController;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

// 93d86a0a-2d29-4d95-a677-e8a17134beda
extern "C" const InterfaceID EIID_MyActivityController =
    { 0x93d86a0a, 0x2d29, 0x4d95, { 0xa6, 0x77, 0xe8, 0xa1, 0x71, 0x34, 0xbe, 0xda } };

MyActivityController::MyActivityController()
    : mGdbPort()
    , mState()
    , mResult(0)
    , mGdbThread()
    , mGotGdbPrint(FALSE)
    , mFile(NULL)
    , mHost(NULL)
{}

MyActivityController::~MyActivityController()
{
    if (mFile)
        pclose(mFile);
}

ECode MyActivityController::ActivityStarting(
    /* [in] */ IIntent * intent,
    /* [in] */ const String& pkg,
    /* [out] */ Boolean * res)
{
    VALIDATE_NOT_NULL(res);
    AutoLock lock(mLock);
    PFL_EX("** Activity starting: %s", pkg.string());
    *res = TRUE;
    return NOERROR;
}

ECode MyActivityController::ActivityResuming(
    /* [in] */ const String& pkg,
    /* [out] */ Boolean * res)
{
    VALIDATE_NOT_NULL(res);
    AutoLock lock(mLock);
    PFL_EX("** Activity resuming: %s", pkg.string());
    *res = TRUE;
    return NOERROR;
}

ECode MyActivityController::AppCrashed(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& shortMsg,
    /* [in] */ const String& longMsg,
    /* [in] */ Int64 timeMillis,
    /* [in] */ const String& stackTrace,
    /* [out] */ Boolean * res)
{
    VALIDATE_NOT_NULL(res);
    AutoLock lock(mLock);
    PFL_EX("** ERROR: PROCESS CRASHED");
    PFL_EX("processName: %s", processName.string());
    PFL_EX("processPid: %d", pid);
    PFL_EX("shortMsg: %s", shortMsg.string());
    PFL_EX("longMsg: %s", longMsg.string());
    PFL_EX("timeMillis: %lld", timeMillis);
    PFL_EX("stack:");
    PFL_EX(stackTrace.string());
    PFL_EX("#");
    Int32 result;
    ECode ec = WaitControllerLocked(pid, STATE_CRASHED, &result);
    if (FAILED(ec))
        return ec;

    *res = result == RESULT_CRASH_KILL ? FALSE : TRUE;
    return ec;
}

ECode MyActivityController::AppEarlyNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& annotation,
    /* [out] */ Int32 * res)
{
    AutoLock lock(mLock);
    VALIDATE_NOT_NULL(res);
    PFL_EX("** ERROR: EARLY PROCESS NOT RESPONDING");
    PFL_EX("processName: %s", processName.string());
    PFL_EX("processPid: %d", pid);
    PFL_EX("annotation: %s", annotation.string());
    Int32 result;
    ECode ec = WaitControllerLocked(pid, STATE_EARLY_ANR, &result);
    if (FAILED(ec))
        return ec;
    if (result == RESULT_EARLY_ANR_KILL) {
        *res = -1;
        return ec;
    }
    *res = 0;
    return ec;
}

ECode MyActivityController::AppNotResponding(
    /* [in] */ const String& processName,
    /* [in] */ Int32 pid,
    /* [in] */ const String& processStats,
    /* [out] */ Int32 * res)
{
    AutoLock lock(mLock);
    VALIDATE_NOT_NULL(res);
    PFL_EX("** ERROR: PROCESS NOT RESPONDING");
    PFL_EX("processName: %s", processName.string());
    PFL_EX("processPid: %d", pid);
    PFL_EX("processStats:");
    PFL_EX("%s#", processStats.string());
    Int32 result;
    ECode ec = WaitControllerLocked(pid, STATE_ANR, &result);
    if (FAILED(ec))
        return ec;
    if (result == RESULT_ANR_KILL) {
        *res = -1;
    } else if (result == RESULT_ANR_WAIT) {
        *res = 1;
    } else {
        *res = 0;
    }
    return ec;
}

ECode MyActivityController::Init(
    /* [in] */ const String& gdbPort)
{
    mGdbPort = gdbPort;
    return NOERROR;
}

ECode MyActivityController::KillGdbLocked()
{
    mGotGdbPrint = FALSE;
    if (mFile)
    {
        fclose(mFile);
        mFile = NULL;
    }
    if (mGdbThread != NULL) {
        mGdbThread->Interrupt();
        mGdbThread = NULL;
    }
    return NOERROR;
}

ECode MyActivityController::WaitControllerLocked(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 state,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    if (!mGdbPort.IsNull()) {
        KillGdbLocked();

        PFL_EX("Starting gdbserver on port %s", mGdbPort.string());
        PFL_EX("Do the following:");
        PFL_EX("  adb forward tcp:%s tcp:%s", mGdbPort.string(), mGdbPort.string());
        PFL_EX("  gdbclient app_process :%s", mGdbPort.string());

        String cmd;
        cmd.AppendFormat("gdbserver :%s --attach %d", mGdbPort.string(), pid);
        mFile = popen(cmd.string(), "r");

        mGdbThread = NULL;
        AutoPtr<GdbThreadRunnable> runable = new GdbThreadRunnable(this);
        CThread::New(runable, (IThread**)&mGdbThread);
        mGdbThread->Start();
        mLock.Wait(500);
    }

    mState = state;
    PFL_EX("");
    PrintMessageForState();

    while (mState != STATE_NORMAL) {
        mLock.Wait(500);
    }
    KillGdbLocked();
    *result = mResult;
    return NOERROR;
}

ECode MyActivityController::ResumeController(
    /* [in] */ Int32 result)
{
    AutoLock lock(mLock);
    mState = STATE_NORMAL;
    mResult = result;
    ECode ec = mLock.NotifyAll();
    return ec;
}

ECode MyActivityController::PrintMessageForState()
{
    switch (mState) {
        case STATE_NORMAL:
            PFL_EX("Monitoring activity manager...  available commands:");
            break;
        case STATE_CRASHED:
            PFL_EX("Waiting after crash...  available commands:");
            PFL_EX("(c)ontinue: show crash dialog");
            PFL_EX("(k)ill: immediately kill app");
            break;
        case STATE_EARLY_ANR:
            PFL_EX("Waiting after early ANR...  available commands:");
            PFL_EX("(c)ontinue: standard ANR processing");
            PFL_EX("(k)ill: immediately kill app");
            break;
        case STATE_ANR:
            PFL_EX("Waiting after ANR...  available commands:");
            PFL_EX("(c)ontinue: show ANR dialog");
            PFL_EX("(k)ill: immediately kill app");
            PFL_EX("(w)ait: wait some more");
            break;
    }
    PFL_EX("(q)uit: finish monitoring");
    return NOERROR;
}

ECode MyActivityController::Run()
{
    PrintMessageForState();

    mHost->GetAm()->SetActivityController(THIS_PROBE(IIActivityController));
    mState = STATE_NORMAL;

    AutoPtr<IInputStreamReader> converter;
    AutoPtr<IBufferedReader> in;

    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    AutoPtr<IInputStream> systemIn;
    system->GetIn((IInputStream**)&systemIn);
    CInputStreamReader::New(systemIn, (IInputStreamReader**)&converter);
    CBufferedReader::New(converter, (IBufferedReader**)&in);
    String line;
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
                PFL_EX("Invalid command: %s", line.string());
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
                PFL_EX("Invalid command: %s", line.string());
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
                PFL_EX("Invalid command: %s", line.string());
            }
        }
        else {
            PFL_EX("Invalid command: %s", line.string());
        }

        AutoLock lock(mLock);
        if (addNewline) {
            PFL_EX("");
        }
        PrintMessageForState();
    }

    return mHost->GetAm()->SetActivityController(NULL);
}

ECode MyActivityController::Init(
    /* [in] */ Handle32 am)
{
    mHost = (CAm*)am;
    return NOERROR;
}

ECode MyActivityController::Init(
    /* [in] */ Handle32 am,
    /* [in] */ const String& gdbPort)
{
    mHost = (CAm*)am;
    mGdbPort = gdbPort;
    return NOERROR;
}

MyActivityController::GdbThreadRunnable::GdbThreadRunnable(
    /* [in] */ MyActivityController* host)
    : mHost(host)
{
}

ECode MyActivityController::GdbThreadRunnable::Run()
{
    char oneLine[500];
    Int32 count = 0;
    while (TRUE) {
        {
            AutoLock lock(mHost->mLock);
            if (mHost->mGdbThread == NULL) {
                return NOERROR;
            }
            if (count == 2) {
                mHost->mGotGdbPrint = TRUE;
                mHost->mLock.NotifyAll();
            }
        }

        if (!fgets(oneLine, sizeof(oneLine), mHost->mFile)) {
            return NOERROR;
        }

        PFL_EX("GDB: %s", oneLine);
        count++;
    }

    return NOERROR;
}

}
}
}
}
