
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
