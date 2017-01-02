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

#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/content/AsyncTaskLoader.h"
#include "elastos/droid/os/CHandler.h"
#include "elastos/droid/utility/TimeUtils.h"
#include "elastos/droid/os/SystemClock.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Utility::TimeUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::Concurrent::CCountDownLatch;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Content {

const String AsyncTaskLoader::TAG("AsyncTaskLoader");
const Boolean AsyncTaskLoader::DEBUG = FALSE;

//==================================================================
// AsyncTaskLoader::LoadTask
//==================================================================
CAR_INTERFACE_IMPL(AsyncTaskLoader::LoadTask, AsyncTask, IRunnable)

AsyncTaskLoader::LoadTask::LoadTask(
    /* [in] */ IWeakReference* loader)
    : mWaiting(FALSE)
    , mWeakHost(loader)
{
    CCountDownLatch::New(1, (ICountDownLatch**)&mDone);
}

AsyncTaskLoader::LoadTask::~LoadTask()
{}

ECode AsyncTaskLoader::LoadTask::Run()
{
    AutoPtr<IAsyncTaskLoader> atl;
    mWeakHost->Resolve(EIID_IAsyncTaskLoader, (IInterface**)&atl);
    if (atl == NULL) return NOERROR;

    AsyncTaskLoader* loader = (AsyncTaskLoader*)atl.Get();
    mWaiting = FALSE;
    return loader->ExecutePendingTask();
}

ECode AsyncTaskLoader::LoadTask::WaitForLoader()
{
    return mDone->Await();
}

ECode AsyncTaskLoader::LoadTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (AsyncTaskLoader::DEBUG) {
//        Slogger::V(TAG, this + " >>> doInBackground");
    }

    AutoPtr<IAsyncTaskLoader> atl;
    mWeakHost->Resolve(EIID_IAsyncTaskLoader, (IInterface**)&atl);
    if (atl == NULL) return NOERROR;

    AsyncTaskLoader* loader = (AsyncTaskLoader*)atl.Get();

//    try {
    FAIL_RETURN(loader->OnLoadInBackground(result));
    if (AsyncTaskLoader::DEBUG) {
//        Slogger::V(TAG, this + "  <<< doInBackground");
    }
    return NOERROR;
//    } catch (OperationCanceledException ex) {
//        if (!isCancelled()) {
            // onLoadInBackground threw a canceled exception spuriously.
            // This is problematic because it means that the LoaderManager did not
            // cancel the Loader itself and still expects to receive a result.
            // Additionally, the Loader's own state will not have been updated to
            // reflect the fact that the task was being canceled.
            // So we treat this case as an unhandled exception.
//            throw ex;
//        }
//        if (AsyncTaskLoader::DEBUG) Slogger::V(TAG, this + "  <<< doInBackground (was canceled)");
//        return null;
//    }
}

ECode AsyncTaskLoader::LoadTask::OnPostExecute(
    /* [in] */ IInterface* data)
{
    if (AsyncTaskLoader::DEBUG) {
//        Slogger::V(TAG, this + " onPostExecute");
    }

    AutoPtr<IAsyncTaskLoader> atl;
    mWeakHost->Resolve(EIID_IAsyncTaskLoader, (IInterface**)&atl);
    if (atl == NULL) return NOERROR;

    AsyncTaskLoader* loader = (AsyncTaskLoader*)atl.Get();
    ECode ec = loader->DispatchOnLoadComplete(this, data);
    FAIL_RETURN(mDone->CountDown());
    return ec;
}

ECode AsyncTaskLoader::LoadTask::OnCancelled(
    /* [in] */ IInterface* data)
{
    if (AsyncTaskLoader::DEBUG) {
//        Slogger::V(TAG, this + " onCancelled");
    }

    AutoPtr<IAsyncTaskLoader> atl;
    mWeakHost->Resolve(EIID_IAsyncTaskLoader, (IInterface**)&atl);
    if (atl == NULL) return NOERROR;

    AsyncTaskLoader* loader = (AsyncTaskLoader*)atl.Get();
    ECode ec = loader->DispatchOnCancelled(this, data);
    FAIL_RETURN(mDone->CountDown());
    return ec;
}

//==================================================================
// AsyncTaskLoader
//==================================================================
CAR_INTERFACE_IMPL(AsyncTaskLoader, Loader, IAsyncTaskLoader)

AsyncTaskLoader::AsyncTaskLoader()
    : mUpdateThrottle(0)
    , mLastLoadCompleteTime(-10000)
{}

AsyncTaskLoader::~AsyncTaskLoader()
{}

ECode AsyncTaskLoader::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, AsyncTask::THREAD_POOL_EXECUTOR);
}

ECode AsyncTaskLoader::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IExecutor* executor)
{
    FAIL_RETURN(Loader::constructor(context))
    mExecutor = executor;
    return NOERROR;
}

ECode AsyncTaskLoader::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);
    StringBuilder sb("AsyncTaskLoader:(");
    sb += StringUtils::ToString((Int32)this);
    sb += ")";
    sb.ToString(info);
    return NOERROR;
}

ECode AsyncTaskLoader::SetUpdateThrottle(
    /* [in] */ Int64 delayMS)
{
    mUpdateThrottle = delayMS;
    if (delayMS != 0) {
        mHandler = 0;
        CHandler::New((IHandler**)&mHandler);
    }
    return NOERROR;
}

ECode AsyncTaskLoader::OnForceLoad()
{
    FAIL_RETURN(Loader::OnForceLoad());
    Boolean ret = FALSE;
    FAIL_RETURN(CancelLoad(&ret))
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    mTask = new LoadTask(wr);
    if (DEBUG) {
//        Slogger::V(TAG, "Preparing load: mTask=" + mTask);
    }

    return ExecutePendingTask();
}

ECode AsyncTaskLoader::OnCancelLoad(
    /* [out] */ Boolean* isCanceled)
{
    VALIDATE_NOT_NULL(isCanceled)
    *isCanceled = FALSE;

    if (DEBUG) {
//        Slogger::V(TAG, "onCancelLoad: mTask=" + mTask);
    }

    if (NULL != mTask.Get()) {
        if (NULL != mCancellingTask) {
            // There was a pending task already waiting for a previous
            // one being canceled; just drop it.
            if (DEBUG) {
//                Slogger::V(TAG, "cancelLoad: still waiting for cancelled task; dropping next");
            }
            if (mTask->mWaiting) {
                mTask->mWaiting = FALSE;
                mHandler->RemoveCallbacks(mTask);
            }
            mTask = NULL;
            *isCanceled = FALSE;
            return NOERROR;
        }
        else if (mTask->mWaiting) {
            // There is a task, but it is waiting for the time it should
            // execute.  We can just toss it.
            if (DEBUG) {
//                Slogger::V(TAG, "cancelLoad: task is waiting, dropping it");
            }
            mTask->mWaiting = FALSE;
            mHandler->RemoveCallbacks(mTask);
            mTask = NULL;
            *isCanceled = FALSE;
            return NOERROR;
        }
        else {
            *isCanceled =  mTask->Cancel(FALSE);
            if (DEBUG) {
//                Slogger::V(TAG, "cancelLoad: cancelled=" + cancelled);
            }
            if (*isCanceled) {
                mCancellingTask = mTask;
                FAIL_RETURN(CancelLoadInBackground());
            }
            mTask = NULL;
            return NOERROR;
        }
    }

    *isCanceled = FALSE;
    return NOERROR;
}

ECode AsyncTaskLoader::OnCanceled(
    /* [in] */ IInterface* data)
{
    return NOERROR;
}

ECode AsyncTaskLoader::ExecutePendingTask()
{
    if (NULL == mCancellingTask && NULL != mTask) {
        if (mTask->mWaiting) {
            mTask->mWaiting = FALSE;
            mHandler->RemoveCallbacks(mTask);
        }
        if (mUpdateThrottle > 0) {
            Int64 now = 0;
            now = SystemClock::GetUptimeMillis();
            if (now < (mLastLoadCompleteTime+mUpdateThrottle)) {
                // Not yet time to do another load.
                if (DEBUG) {
//                    Slogger::V(TAG, "Waiting until " + (mLastLoadCompleteTime+mUpdateThrottle)
//                        + " to execute: " + mTask);
                }
                mTask->mWaiting = TRUE;
                Boolean bval;
                mHandler->PostAtTime(mTask, mLastLoadCompleteTime + mUpdateThrottle, &bval);
                return NOERROR;
            }
        }
        if (DEBUG) {
//            Slogger::V(TAG, "Executing: " + mTask);
        }
        mTask->ExecuteOnExecutor(AsyncTask::THREAD_POOL_EXECUTOR, NULL);
    }

    return NOERROR;
}

ECode AsyncTaskLoader::DispatchOnCancelled(
    /* [in] */ LoadTask* task,
    /* [in] */ IInterface* data)
{
    FAIL_RETURN(OnCanceled(data));
    if (mCancellingTask.Get() == task) {
        if (DEBUG) {
//            Slogger::V(TAG, "Cancelled task is now canceled!");
        }
        mLastLoadCompleteTime = SystemClock::GetUptimeMillis();
        mCancellingTask = NULL;
        if (DEBUG) {
//            Slogger::V(TAG, "Delivering cancellation");
        }
        FAIL_RETURN(DeliverCancellation())
        FAIL_RETURN(ExecutePendingTask())
    }

    return NOERROR;
}

ECode AsyncTaskLoader::DispatchOnLoadComplete(
    /* [in] */ LoadTask* task,
    /* [in] */ IInterface* data)
{
    if (mTask.Get() != task) {
        if (DEBUG) {
            Slogger::V(TAG, "Load complete of old task, trying to cancel");
        }
        FAIL_RETURN(DispatchOnCancelled(task, data));
    }
    else {
        Boolean ret = FALSE;
        if (IsAbandoned(&ret), ret) {
            // This cursor has been abandoned; just cancel the new data.
            FAIL_RETURN(OnCanceled(data));
        }
        else {
            mLastLoadCompleteTime = SystemClock::GetUptimeMillis();
            mTask = NULL;
            if (DEBUG) {
                Slogger::V(TAG, "Delivering result");
            }
            FAIL_RETURN(DeliverResult(data));
        }
    }

    return NOERROR;
}

ECode AsyncTaskLoader::OnLoadInBackground(
    /* [out] */ IInterface** result)
{
    return LoadInBackground(result);
}

ECode AsyncTaskLoader::CancelLoadInBackground()
{
    return NOERROR;
}

ECode AsyncTaskLoader::IsLoadInBackgroundCanceled(
    /* [out] */ Boolean* isCanceled)
{
    VALIDATE_NOT_NULL(isCanceled)
    *isCanceled = mCancellingTask != NULL;
    return NOERROR;
}

ECode AsyncTaskLoader::WaitForLoader()
{
    AutoPtr<LoadTask> task = mTask;
    if (NULL != task) {
        FAIL_RETURN(task->WaitForLoader());
    }

    return NOERROR;
}

ECode AsyncTaskLoader::Dump(
    /* [in] */ const String& prefix,
    /* [in] */ IFileDescriptor* fd,
    /* [in] */ IPrintWriter* writer,
    /* [in] */ ArrayOf<String>* args)
{
    FAIL_RETURN(Loader::Dump(prefix, fd, writer, args));

    if (NULL != mTask) {
        writer->Print(prefix);
        writer->Print(String("mTask="));
        writer->Print(TO_IINTERFACE(mTask));
        writer->Print(String(" waiting="));
        writer->Print(mTask->mWaiting);
    }
    if (NULL != mCancellingTask) {
        writer->Print(prefix);
        writer->Print(String("mCancellingTask="));
        writer->Print(TO_IINTERFACE(mCancellingTask));
        writer->Print(String(" waiting="));
        writer->Print(mCancellingTask->mWaiting);
    }
    if (mUpdateThrottle != 0) {
        writer->Print(prefix);
        writer->Print(String("mUpdateThrottle="));
        TimeUtils::FormatDuration(mUpdateThrottle, writer);
        writer->Print(String(" mLastLoadCompleteTime="));
        TimeUtils::FormatDuration(mLastLoadCompleteTime,
            SystemClock::GetUptimeMillis(), writer);
        writer->Println();
    }

    return NOERROR;
}

}
}
}
