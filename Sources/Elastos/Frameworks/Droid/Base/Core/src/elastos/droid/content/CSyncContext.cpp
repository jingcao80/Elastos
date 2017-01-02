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

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/CSyncContext.h"
#include "elastos/droid/os/SystemClock.h"

using Elastos::Droid::Os::SystemClock;

namespace Elastos {
namespace Droid {
namespace Content {

const Int64 CSyncContext::HEARTBEAT_SEND_INTERVAL_IN_MS;

CAR_INTERFACE_IMPL(CSyncContext, Object, ISyncContext)

CAR_OBJECT_IMPL(CSyncContext)

CSyncContext::CSyncContext()
    : mLastHeartbeatSendTime(0)
{}

CSyncContext::~CSyncContext()
{}

ECode CSyncContext::SetStatusText(
    /* [in] */ const String& message)
{
    return UpdateHeartbeat();
}

ECode CSyncContext::OnFinished(
    /* [in] */ ISyncResult* result)
{
    if (NULL != mSyncContext) {
        FAIL_RETURN(mSyncContext->OnFinished(result))
    }

    return NOERROR;
}

ECode CSyncContext::GetSyncContextBinder(
    /* [out] */ IBinder** binder)
{
    VALIDATE_NOT_NULL(binder)
    *binder = IBinder::Probe(mSyncContext);
    REFCOUNT_ADD(*binder)
    return NOERROR;
}

ECode CSyncContext::constructor(
    /* [in] */ IISyncContext* syncContextInterface)
{
    mSyncContext = syncContextInterface;
    mLastHeartbeatSendTime = 0;
    return NOERROR;
}

ECode CSyncContext::UpdateHeartbeat()
{
    Int64 now = SystemClock::GetElapsedRealtime();
    if (now < mLastHeartbeatSendTime + HEARTBEAT_SEND_INTERVAL_IN_MS) return NOERROR;

    mLastHeartbeatSendTime = now;
    if (NULL != mSyncContext) {
        FAIL_RETURN(mSyncContext->SendHeartbeat())
    }

    return NOERROR;
}

}
}
}

