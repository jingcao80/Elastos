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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/SystemService.h"
#include "elastos/droid/os/SystemProperties.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Os {

HashMap<String, SystemServiceState> SystemService::sStates;
Object SystemService::sPropertyLock;
Boolean SystemService::sInitialized = SystemService::Init();

static const char* StateToString(SystemServiceState state)
{
    switch (state) {
        case SystemServiceState_NONE:
            return "NONE";
        case SystemServiceState_RUNNING:
            return "RUNNING";
        case SystemServiceState_STOPPING:
            return "STOPPING";
        case SystemServiceState_STOPPED:
            return "STOPPED";
        case SystemServiceState_RESTARTING:
            return "RESTARTING";
    }
    return "NONE";
}

ECode SystemService::Start(
    /* [in] */ const String& name)
{
    return SystemProperties::Set(String("ctl.start"), name);
}

ECode SystemService::Stop(
    /* [in] */ const String& name)
{
    return SystemProperties::Set(String("ctl.stop"), name);
}

ECode SystemService::Restart(
    /* [in] */ const String& name)
{
    return SystemProperties::Set(String("ctl.restart"), name);
}

SystemServiceState SystemService::GetState(
    /* [in] */ const String& service)
{
    StringBuilder sb("init.svc.");
    sb += service;
    String rawState;
    SystemProperties::Get(sb.ToString(), &rawState);

    HashMap<String, SystemServiceState>::Iterator it = sStates.Find(rawState);
    if (it != sStates.End()) {
        return it->mSecond;
    }

    return SystemServiceState_STOPPED;
}

Boolean SystemService::IsStopped(
    /* [in] */ const String& service)
{
    return (SystemServiceState_STOPPED == GetState(service));
}

Boolean SystemService::IsRunning(
    /* [in] */ const String& service)
{
    return (SystemServiceState_RUNNING == GetState(service));
}

ECode SystemService::WaitForState(
    /* [in] */ const String& service,
    /* [in] */ SystemServiceState state,
    /* [in] */ Int64 timeoutMillis)
{
    Int64 endMillis = SystemClock::GetElapsedRealtime() + timeoutMillis;
    while (TRUE) {
        {    AutoLock syncLock(sPropertyLock);
            SystemServiceState currentState = GetState(service);
            if (state == currentState) {
                return NOERROR;
            }

            if (SystemClock::GetElapsedRealtime() >= endMillis) {
                Logger::E("SystemService", "Service %s currently %d; waited %lld ms for %d",
                    service.string(), StateToString(currentState), timeoutMillis, StateToString(state));
                return E_TIMEOUT_EXCEPTION;
            }

            // try {
                sPropertyLock.Wait(timeoutMillis);
            // } catch (InterruptedException e) {
            // }
        }
    }
    return NOERROR;
}

ECode SystemService::WaitForAnyStopped(
    /* [in] */ ArrayOf<String>* services)
{
    while (TRUE) {
        {    AutoLock syncLock(sPropertyLock);
            for (Int32 i = 0; i < services->GetLength(); ++i) {
                if (SystemServiceState_STOPPED == GetState((*services)[i])) {
                    return NOERROR;
                }
            }

            // try {
                sPropertyLock.Wait();
            // } catch (InterruptedException e) {
            // }
        }
    }
    return NOERROR;
}

Boolean SystemService::Init()
{
    AutoPtr<IRunnable> runnable = new MyRunnable();
    return SystemProperties::AddChangeCallback(runnable);
}

ECode SystemService::MyRunnable::Run()
{
    AutoLock lock(SystemService::sPropertyLock);
    SystemService::sPropertyLock.NotifyAll();

    return NOERROR;
}


} // namespace Os
} // namespace Droid
} // namespace Elastos
