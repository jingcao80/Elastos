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

#include "elastos/droid/os/CSystemService.h"
#include "elastos/droid/os/SystemService.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CSystemService, Singleton, ISystemService)

CAR_SINGLETON_IMPL(CSystemService)

ECode CSystemService::Start(
    /* [in] */ const String& name)
{
    return SystemService::Start(name);
}

ECode CSystemService::Stop(
    /* [in] */ const String& name)
{
    return SystemService::Stop(name);
}

ECode CSystemService::Restart(
    /* [in] */ const String& name)
{
    return SystemService::Restart(name);
}

ECode CSystemService::GetState(
    /* [in] */ const String& service,
    /* [out] */ SystemServiceState* state)
{
    VALIDATE_NOT_NULL(state);
    *state = SystemService::GetState(service);
    return NOERROR;
}

ECode CSystemService::IsStopped(
    /* [in] */ const String& service,
    /* [out] */ Boolean* isStopped)
{
    VALIDATE_NOT_NULL(isStopped);
    *isStopped = SystemService::IsStopped(service);
    return NOERROR;
}

ECode CSystemService::IsRunning(
    /* [in] */ const String& service,
    /* [out] */ Boolean* isRunning)
{
    VALIDATE_NOT_NULL(isRunning);
    *isRunning = SystemService::IsRunning(service);
    return NOERROR;
}

ECode CSystemService::WaitForState(
    /* [in] */ const String& service,
    /* [in] */ SystemServiceState state,
    /* [in] */ Int64 timeoutMillis)
{
    return SystemService::WaitForState(service, state, timeoutMillis);
}

ECode CSystemService::WaitForAnyStopped(
    /* [in] */ ArrayOf<String>* services)
{
    return SystemService::WaitForAnyStopped(services);
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
