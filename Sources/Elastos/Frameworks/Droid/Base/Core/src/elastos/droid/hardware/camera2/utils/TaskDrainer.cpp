
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Os.h"
#include "elastos/droid/hardware/camera2/utils/TaskDrainer.h"
#include "elastos/droid/internal/utility/Preconditions.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Internal::Utility::Preconditions;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::CHashSet;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Utils {

CAR_INTERFACE_IMPL(TaskDrainer, Object, ITaskDrainer)

const String TaskDrainer::TAG("TaskDrainer");

TaskDrainer::MyRunnable::MyRunnable(
    /* [in] */ TaskDrainer* host)
    : mHost(host)
{
}

ECode TaskDrainer::MyRunnable::Run()
{
    if (mHost->VERBOSE) {
        StringBuilder sb;
        sb += mHost->TAG;
        sb += "[";
        sb += mHost->mName;
        sb += "]";
        Slogger::V(sb.ToString(), "onDrained");
    }

    return mHost->mListener->OnDrained();
}

TaskDrainer::TaskDrainer()
    : mName(NULL)
    , mDraining(FALSE)
    , mDrainFinished(FALSE)
{
    CHashSet::New((ISet**)&mTaskSet);
}

TaskDrainer::~TaskDrainer()
{
}

ECode TaskDrainer::constructor()
{
    return NOERROR;
}

ECode TaskDrainer::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ ITaskDrainerDrainListener* listener)
{
    FAIL_RETURN(Preconditions::CheckNotNull(handler, String("handler must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(listener, String("listener must not be null")))

    mHandler = handler;
    mListener = listener;
    mName = NULL;
    return NOERROR;
}

ECode TaskDrainer::constructor(
    /* [in] */ IHandler* handler,
    /* [in] */ ITaskDrainerDrainListener* listener,
    /* [in] */ const String& name)
{
    // XX: Probably don't need a handler at all here
    FAIL_RETURN(Preconditions::CheckNotNull(handler, String("handler must not be null")))
    FAIL_RETURN(Preconditions::CheckNotNull(listener, String("listener must not be null")))

    mHandler = handler;
    mListener = listener;
    mName = name;
    return NOERROR;
}

ECode TaskDrainer::TaskStarted(
    /* [in] */ IInterface* task)
{
    {    AutoLock syncLock(mLock);
        if (VERBOSE) {
            StringBuilder sb;
            sb += TAG;
            sb += "[";
            sb += mName;
            sb += "]";
            String str;
            IObject::Probe(task)->ToString(&str);
            Slogger::V(sb.ToString(), "taskStarted %s", str.string());
        }

        if (mDraining) {
            //throw new IllegalStateException("Can't start more tasks after draining has begun");
            Slogger::E(TAG, "Can't start more tasks after draining has begun");
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        Boolean result;
        mTaskSet->Add(task, &result);
        if (!result) {
            //throw new IllegalStateException("Task " + task + " was already started");
            String str;
            IObject::Probe(task)->ToString(&str);
            Slogger::E(TAG, "Task %s was already started", str.string());
            return E_ILLEGAL_STATE_EXCEPTION;
        }
    }
    return NOERROR;
}

ECode TaskDrainer::TaskFinished(
    /* [in] */ IInterface* task)
{
    {    AutoLock syncLock(mLock);
        if (VERBOSE) {
            StringBuilder sb;
            sb += TAG;
            sb += "[";
            sb += mName;
            sb += "]";
            String str;
            IObject::Probe(task)->ToString(&str);
            Slogger::V(sb.ToString(), "taskFinished %s", str.string());
        }

        Boolean result;
        mTaskSet->Remove(task, &result);
        if (!result) {
            //throw new IllegalStateException("Task " + task + " was already finished");
            String str;
            IObject::Probe(task)->ToString(&str);
            Slogger::E(TAG, "Task %s was already finished", str.string());
            return E_ILLEGAL_STATE_EXCEPTION;
        }

        // If this is the last finished task and draining has already begun, fire #onDrained
        CheckIfDrainFinished();
    }
    return NOERROR;
}

ECode TaskDrainer::BeginDrain()
{
    {    AutoLock syncLock(mLock);
        if (!mDraining) {
            if (VERBOSE) {
                StringBuilder sb;
                sb += TAG;
                sb += "[";
                sb += mName;
                sb += "]";
                Slogger::V(sb.ToString(), "beginDrain started");
            }

            mDraining = TRUE;

            // If all tasks that had started had already finished by now, fire #onDrained
            CheckIfDrainFinished();
        }
        else {
            if (VERBOSE) {
                StringBuilder sb;
                sb += TAG;
                sb += "[";
                sb += mName;
                sb += "]";
                Slogger::V(sb.ToString(), "beginDrain ignored");
            }
        }
    }
    return NOERROR;
}

ECode TaskDrainer::CheckIfDrainFinished()
{
    Boolean result;
    mTaskSet->IsEmpty(&result);
    if (result && mDraining && !mDrainFinished) {
        mDrainFinished = TRUE;
        PostDrained();
    }
    return NOERROR;
}

ECode TaskDrainer::PostDrained()
{
    AutoPtr<IRunnable> run = new MyRunnable(this);
    Boolean result;
    return mHandler->Post(run, &result);
}

} // namespace Utils
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos