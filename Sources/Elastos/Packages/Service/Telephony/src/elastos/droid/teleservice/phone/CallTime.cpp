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

#include "elastos/droid/teleservice/phone/CallTime.h"
#include "elastos/droid/teleservice/phone/PhoneGlobals.h"
#include "elastos/droid/os/SystemClock.h"
#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Content.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::ICallState;
using Elastos::Droid::Internal::Telephony::ICallState_ACTIVE;
using Elastos::Droid::Internal::Telephony::IConnection;
using Elastos::Droid::Os::SystemClock;
using Elastos::Core::StringBuilder;
using Elastos::IO::CFile;
using Elastos::IO::CFileHelper;
using Elastos::IO::IFile;
using Elastos::IO::IFileHelper;
using Elastos::Utility::IList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

ECode CallTime::PeriodicTimerCallback::Run()
{
    if (PROFILE && mHost->IsTraceRunning()) {
        mHost->StopTrace();
    }

    mHost->mTimerRunning = false;
    return mHost->PeriodicUpdateTimer();
}

const String CallTime::TAG("PHONE/CallTime");
const Boolean CallTime::DBG = FALSE;
const Boolean CallTime::PROFILE = TRUE;

const Int32 CallTime::PROFILE_STATE_NONE = 0;
const Int32 CallTime::PROFILE_STATE_READY = 1;
const Int32 CallTime::PROFILE_STATE_RUNNING = 2;

Int32 CallTime::sProfileState = PROFILE_STATE_NONE;

CallTime::CallTime(
    /* [in] */ ICallTimeOnTickListener* listener)
    : mLastReportedTime(0)
    , mInterval(0)
    , mListener(listener)
{
    Handler::constructor();

    mTimerCallback = new PeriodicTimerCallback(this);
}

ECode CallTime::SetActiveCallMode(
    /* [in] */ ICall* call)
{
    if (DBG) {
        StringBuilder sb;
        sb += "setActiveCallMode(";
        sb += TO_CSTR(call);
        sb += ")...";
        Log(sb.ToString());
    }
    mCall = call;

    // How frequently should we update the UI?
    mInterval = 1000;  // once per second
    return NOERROR;
}

ECode CallTime::Reset()
{
    if (DBG) {
        Log(String("reset()..."));
    }

    mLastReportedTime = SystemClock::GetUptimeMillis() - mInterval;
    return NOERROR;
}

ECode CallTime::PeriodicUpdateTimer()
{
    if (!mTimerRunning) {
        mTimerRunning = TRUE;

        Int64 now = SystemClock::GetUptimeMillis();
        Int64 nextReport = mLastReportedTime + mInterval;

        while (now >= nextReport) {
            nextReport += mInterval;
        }

        if (DBG) {
            StringBuilder sb;
            sb += "periodicUpdateTimer() @ ";
            sb += nextReport;
            Log(sb.ToString());
        }
        Boolean tmp = FALSE;
        PostAtTime(mTimerCallback, nextReport, &tmp);
        mLastReportedTime = nextReport;

        if (mCall != NULL) {
            ICallState state;
            mCall->GetState(&state);

            if (state == ICallState_ACTIVE) {
                UpdateElapsedTime(mCall);
            }
        }

        if (PROFILE && IsTraceReady()) {
            StartTrace();
        }
    }
    else {
        if (DBG) Log(String("periodicUpdateTimer: timer already running, bail"));
    }
    return NOERROR;
}

ECode CallTime::CancelTimer()
{
    if (DBG) Log(String("cancelTimer()..."));
    RemoveCallbacks(mTimerCallback);
    mTimerRunning = FALSE;
    return NOERROR;
}

void CallTime::UpdateElapsedTime(
    /* [in] */ ICall* call)
{
    if (mListener != NULL) {
        Int64 duration = GetCallDuration(call);
        mListener->OnTickForCallTimeElapsed(duration / 1000);
    }
    return;
}

Int64 CallTime::GetCallDuration(
    /* [in] */ ICall* call)
{
    Int64 duration = 0;
    AutoPtr<IList> connections;
    call->GetConnections((IList**)&connections);
    Int32 count;
    connections->GetSize(&count);
    AutoPtr<IConnection> c;

    if (count == 1) {
        AutoPtr<IInterface> obj;
        connections->Get(0, (IInterface**)&obj);
        c = IConnection::Probe(obj);
        //duration = (state == Call.State.ACTIVE
        //            ? c.getDurationMillis() : c.getHoldDurationMillis());
        c->GetDurationMillis(&duration);
    }
    else {
        for (Int32 i = 0; i < count; i++) {
            AutoPtr<IInterface> obj;
            connections->Get(i, (IInterface**)&obj);
            c = IConnection::Probe(obj);
            //long t = (state == Call.State.ACTIVE
            //          ? c.getDurationMillis() : c.getHoldDurationMillis());
            Int64 t;
            c->GetDurationMillis(&t);
            if (t > duration) {
                duration = t;
            }
        }
    }

    if (DBG) {
        StringBuilder sb;
        sb += "updateElapsedTime, count=";
        sb += count;
        sb += ", duration=";
        sb += duration;
        Log(sb.ToString());
    }
    return duration;
}

void CallTime::Log(
    /* [in] */ const String& msg)
{
    StringBuilder sb;
    sb += "[CallTime] ";
    sb += msg;
    Logger::D(TAG, sb.ToString());
    return;
}

void CallTime::SetTraceReady()
{
    if (sProfileState == PROFILE_STATE_NONE) {
        sProfileState = PROFILE_STATE_READY;
        Log(String("trace ready..."));
    }
    else {
        StringBuilder sb;
        sb += "current trace state = ";
        sb += sProfileState;
        Log(sb.ToString());
    }
    return;
}

Boolean CallTime::IsTraceReady()
{
    return sProfileState == PROFILE_STATE_READY;
}

Boolean CallTime::IsTraceRunning()
{
    return sProfileState == PROFILE_STATE_RUNNING;
}

void CallTime::StartTrace()
{
    if ((PROFILE & sProfileState) == PROFILE_STATE_READY) {
        // For now, we move away from temp directory in favor of
        // the application's data directory to store the trace
        // information (/data/data/com.android.phone).
        AutoPtr<PhoneGlobals> phoneGlobals;
        PhoneGlobals::GetInstance((PhoneGlobals**)&phoneGlobals);
        AutoPtr<IFile> file;
        IContext::Probe(phoneGlobals)->GetDir(String("phoneTrace"), IContext::MODE_PRIVATE, (IFile**)&file);

        Boolean result = FALSE;
        if ((file->Exists(&result), result) == FALSE) {
            file->Mkdirs(&result);
        }
        String path;
        file->GetPath(&path);
        AutoPtr<IFileHelper> helper;
        CFileHelper::AcquireSingleton((IFileHelper**)&helper);
        String separator;
        helper->GetSeparator(&separator);
        String baseName = path + separator + String("callstate");
        String dataFile = baseName + String(".data");
        String keyFile = baseName + String(".key");

        CFile::New(dataFile, (IFile**)&file);
        if ((file->Exists(&result), result) == TRUE) {
            file->Delete();
        }

        CFile::New(keyFile, (IFile**)&file);
        if ((file->Exists(&result), result) == TRUE) {
            file->Delete();
        }

        sProfileState = PROFILE_STATE_RUNNING;
        Log(String("startTrace"));
        assert(0 && "TODO Need Debug");
        // Debug::StartMethodTracing(baseName, 8 * 1024 * 1024);
    }
    return;
}

void CallTime::StopTrace()
{
    if (PROFILE) {
        if (sProfileState == PROFILE_STATE_RUNNING) {
            sProfileState = PROFILE_STATE_NONE;
            Log(String("stopTrace"));
            assert(0 && "TODO Need Debug");
            // Debug::StopMethodTracing();
        }
    }
    return;
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos