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

#include "elastos/droid/app/CActivityManagerAppTask.h"
#include "elastos/droid/app/CActivityThread.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace App {

const String CActivityManagerAppTask::TAG("CActivityManagerAppTask");

CAR_INTERFACE_IMPL(CActivityManagerAppTask, Object, IActivityManagerAppTask)

CAR_OBJECT_IMPL(CActivityManagerAppTask)

ECode CActivityManagerAppTask::constructor(
    /* [in] */ IIAppTask* task)
{
    mAppTaskImpl = task;
    return NOERROR;
}

ECode CActivityManagerAppTask::FinishAndRemoveTask()
{
    // try {
    ECode ec = mAppTaskImpl->FinishAndRemoveTask();
    if (FAILED(ec)) {
        Slogger::E(TAG, "Invalid AppTask");
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Invalid AppTask", e);
    // }
}

ECode CActivityManagerAppTask::GetTaskInfo(
    /* [out] */ IActivityManagerRecentTaskInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = NULL;
    // try {
    ECode ec = mAppTaskImpl->GetTaskInfo(info);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Invalid AppTask");
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Invalid AppTask", e);
    //     return null;
    // }
}

ECode CActivityManagerAppTask::MoveToFront()
{
    // try {
    ECode ec = mAppTaskImpl->MoveToFront();
    if (FAILED(ec)) {
        Slogger::E(TAG, "Invalid AppTask");
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Invalid AppTask", e);
    // }
}

ECode CActivityManagerAppTask::StartActivity(
    /* [in] */ IContext* context,
    /* [in] */ IIntent* intent,
    /* [in] */ IBundle* options)
{
    AutoPtr<IActivityThread> thread = CActivityThread::GetCurrentActivityThread();
    AutoPtr<IInstrumentation> instrumentation;
    thread->GetInstrumentation((IInstrumentation**)&instrumentation);
    AutoPtr<IApplicationThread> appThread;
    thread->GetApplicationThread((IApplicationThread**)&appThread);
    return instrumentation->ExecStartActivityFromAppTask(context,
            IBinder::Probe(appThread), mAppTaskImpl, intent, options);
}

ECode CActivityManagerAppTask::SetExcludeFromRecents(
    /* [in] */ Boolean exclude)
{
    // try {
    ECode ec = mAppTaskImpl->SetExcludeFromRecents(exclude);
    if (FAILED(ec)) {
        Slogger::E(TAG, "Invalid AppTask");
    }
    return NOERROR;
    // } catch (RemoteException e) {
    //     Slog.e(TAG, "Invalid AppTask", e);
    // }
}

} // namespace App
} // namespace Droid
} // namespace Elastos
