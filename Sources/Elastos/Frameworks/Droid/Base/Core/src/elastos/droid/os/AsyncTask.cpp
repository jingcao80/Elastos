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

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/os/Process.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

#include <unistd.h>

using Elastos::Core::EIID_IRunnable;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::CThread;
using Elastos::Utility::CArrayDeque;
using Elastos::Utility::IDeque;
using Elastos::Utility::Concurrent::CLinkedBlockingQueue;
using Elastos::Utility::Concurrent::EIID_IThreadFactory;
using Elastos::Utility::Concurrent::EIID_IExecutor;
using Elastos::Utility::Concurrent::EIID_ICallable;
using Elastos::Utility::Concurrent::EIID_IRunnableFuture;
using Elastos::Utility::Concurrent::IThreadPoolExecutor;
using Elastos::Utility::Concurrent::CThreadPoolExecutor;
using Elastos::Utility::Concurrent::ITimeUnitHelper;
using Elastos::Utility::Concurrent::CTimeUnitHelper;
using Elastos::Utility::Concurrent::Atomic::CAtomicInteger32;
using Elastos::Utility::Concurrent::Atomic::CAtomicBoolean;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;

namespace Elastos {
namespace Droid {
namespace Os {

static Int32 GetCPUCount()
{
    return sysconf(_SC_NPROCESSORS_CONF);
}

static AutoPtr<IBlockingQueue> InitPoolWorkQueue()
{
    AutoPtr<IBlockingQueue> bq;
    ASSERT_SUCCEEDED(CLinkedBlockingQueue::New(10, (IBlockingQueue**)&bq));
    return bq;
}

const String AsyncTask::TAG("AsyncTask");
const Int32 AsyncTask::CPU_COUNT = GetCPUCount();//Runtime.getRuntime().availableProcessors();
const Int32 AsyncTask::CORE_POOL_SIZE = CPU_COUNT + 1;
const Int32 AsyncTask::MAXIMUM_POOL_SIZE = CPU_COUNT * 2 + 1;
const Int32 AsyncTask::KEEP_ALIVE;
const AutoPtr<IThreadFactory> AsyncTask::sThreadFactory = new AsyncTask::MyThreadFactory();
const AutoPtr<IBlockingQueue> AsyncTask::sPoolWorkQueue = InitPoolWorkQueue();
const AutoPtr<IExecutor> AsyncTask::THREAD_POOL_EXECUTOR = InitThreadPoolExecutor();

const AutoPtr<IExecutor> AsyncTask::SERIAL_EXECUTOR = new AsyncTask::SerialExecutor();
const Int32 AsyncTask::MESSAGE_POST_RESULT;
const Int32 AsyncTask::MESSAGE_POST_PROGRESS;
AutoPtr<IHandler> AsyncTask::sHandler;
/* volatile */ AutoPtr<IExecutor> AsyncTask::sDefaultExecutor = SERIAL_EXECUTOR;

//====================================================================
// AsyncTask::InternalHandler
//====================================================================
ECode AsyncTask::InternalHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 what;
    msg->GetWhat(&what);
    AutoPtr<IInterface> obj;
    msg->GetObj((IInterface**)&obj);
    AsyncTask::AsyncTaskResult* result = (AsyncTask::AsyncTaskResult*)(IObject*)obj.Get();

    switch(what) {
        case AsyncTask::MESSAGE_POST_RESULT:
            // There is only one result
            result->mTask->Finish((*result->mData)[0]);
            break;
        case AsyncTask::MESSAGE_POST_PROGRESS:
            result->mTask->OnProgressUpdate(result->mData);
            break;
    }

    return NOERROR;
}

//====================================================================
// AsyncTask::MyThreadFactory
//====================================================================
CAR_INTERFACE_IMPL(AsyncTask::MyThreadFactory, Object, IThreadFactory);

AsyncTask::MyThreadFactory::MyThreadFactory()
{
    CAtomicInteger32::New(1, (IAtomicInteger32**)&mCount);
}

ECode AsyncTask::MyThreadFactory::NewThread(
    /* [in] */ IRunnable* r,
    /* [out] */ IThread** t)
{
    VALIDATE_NOT_NULL(t);

    Int32 value;
    mCount->GetAndIncrement(&value);
    StringBuilder sb("AsyncTask #");
    sb.Append(value);
    return CThread::New(r, sb.ToString(), t);
}

//====================================================================
// AsyncTask::SerialExecutor::MyRunnable
//====================================================================

ECode AsyncTask::SerialExecutor::MyRunnable::Run()
{
    // try {
    mR->Run();
    // } finally {
    mOwner->ScheduleNext();
    // }
    return NOERROR;
}


//====================================================================
// AsyncTask::SerialExecutor
//====================================================================
CAR_INTERFACE_IMPL(AsyncTask::SerialExecutor, Object, IExecutor)

AsyncTask::SerialExecutor::SerialExecutor()
{
    CArrayDeque::New((IArrayDeque**)&mTasks);
}

ECode AsyncTask::SerialExecutor::Execute(
    /* [in] */ IRunnable* r)
{
    AutoLock lock(this);

    AutoPtr<IRunnable> runnable = new MyRunnable(this, r);
    Boolean result;
    IDeque::Probe(mTasks)->Offer(TO_IINTERFACE(runnable), &result);

    if (mActive == NULL) {
        ScheduleNext();
    }
    return NOERROR;
}

ECode AsyncTask::SerialExecutor::ScheduleNext()
{
    AutoLock lock(this);

    AutoPtr<IInterface> obj;
    IDeque::Probe(mTasks)->Poll((IInterface**)&obj);
    mActive = IRunnable::Probe(obj);

    if (mActive != NULL) {
        THREAD_POOL_EXECUTOR->Execute(mActive);
    }
    return NOERROR;
}

//====================================================================
// AsyncTask::WorkerRunnable
//====================================================================
CAR_INTERFACE_IMPL(AsyncTask::WorkerRunnable, Object, ICallable)

AsyncTask::WorkerRunnable::~WorkerRunnable()
{
    if (mIsExecuted) {
        mOwner->Release();
    }
}

ECode AsyncTask::WorkerRunnable::Call(
    /* [out] */ IInterface** result)
{
    mOwner->mTaskInvoked->Set(TRUE);

    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_BACKGROUND);
    //noinspection unchecked
    AutoPtr<IInterface> r;
    mOwner->DoInBackground(mParams, (IInterface**)&r);
    AutoPtr<IInterface> _r = mOwner->PostResult(r);
    *result = _r;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

//====================================================================
// AsyncTask::MyFutureTask
//====================================================================
void AsyncTask::MyFutureTask::Done()
{
    // try {
    AutoPtr<IInterface> object;
    Get((IInterface**)&object);
    mOwner->PostResultIfNotInvoked(object);
    // } catch (InterruptedException e) {
    //     android.util.Log.w(TAG, e);
    // } catch (ExecutionException e) {
    //     throw new RuntimeException("An error occured while executing doInBackground()",
    //             e.getCause());
    // } catch (CancellationException e) {
    //     postResultIfNotInvoked(null);
    // }
}

//====================================================================
// AsyncTask
//====================================================================
AsyncTask::AsyncTask()
    : mIsExecuted(FALSE)
    , mStatus(PENDING)
{
    CAtomicBoolean::New((IAtomicBoolean**)&mCancelled);
    CAtomicBoolean::New((IAtomicBoolean**)&mTaskInvoked);
    mWorker = new WorkerRunnable(this);
    mFuture = new MyFutureTask(mWorker, this);
}

AsyncTask::~AsyncTask()
{
    if (!mIsExecuted) {
        delete mWorker;
    }
}

ECode AsyncTask::Init()
{
    AutoPtr<ILooper> looper;
    return GetHandler()->GetLooper((ILooper**)&looper);
}

AutoPtr<IHandler> AsyncTask::GetHandler()
{
    if (sHandler == NULL) {
        AutoPtr<InternalHandler> ih = new InternalHandler();
        ih->constructor();
        sHandler = (IHandler*)ih.Get();

    }

    return sHandler;
}

/** @hide */
void AsyncTask::SetDefaultExecutor(
    /* [in] */ IExecutor* exec)
{
    sDefaultExecutor = exec;
}

void AsyncTask::PostResultIfNotInvoked(
    /* [in] */ IInterface* result)
{
    Boolean wasTaskInvoked;
    mTaskInvoked->Get(&wasTaskInvoked);
    if (!wasTaskInvoked) {
        PostResult(result);
    }
}

AutoPtr<IInterface> AsyncTask::PostResult(
    /* [in] */ IInterface* result)
{
    AutoPtr< ArrayOf<IInterface*> > data = ArrayOf<IInterface*>::Alloc(1);
    data->Set(0, result);
    AutoPtr<AsyncTaskResult> atResult = new AsyncTaskResult(this, data);

    AutoPtr<IMessage> msg;
    GetHandler()->ObtainMessage(MESSAGE_POST_RESULT, TO_IINTERFACE(atResult), (IMessage**)&msg);
    msg->SendToTarget();

    return result;
}

AsyncTask::Status AsyncTask::GetStatus()
{
    return mStatus;
}

Boolean AsyncTask::IsCancelled()
{
    Boolean value;
    mCancelled->Get(&value);
    return value;
}

Boolean AsyncTask::Cancel(
    /* [in] */ Boolean mayInterruptIfRunning)
{
    mCancelled->Set(TRUE);
    Boolean result;
    mFuture->Cancel(mayInterruptIfRunning, &result);
    return result;
}

ECode AsyncTask::Get(
    /* [out] */ IInterface** result)
{
    return mFuture->Get(result);
}

ECode AsyncTask::Get(
    /* [in] */ Int64 timeout,
    /* [in] */ ITimeUnit* unit,
    /* [out] */ IInterface** result)
{
    return mFuture->Get(timeout, unit, result);
}

ECode AsyncTask::Execute(
    /* [in] */ ArrayOf<IInterface*>* params)
{
    return ExecuteOnExecutor(sDefaultExecutor, params);
}

ECode AsyncTask::ExecuteOnExecutor(
    /* [in] */ IExecutor* exec,
    /* [in] */ ArrayOf<IInterface*>* params)
{
    if (mStatus != PENDING) {
        switch (mStatus) {
            case RUNNING:
                Logger::E(TAG, "Cannot execute task: the task is already running.");
                return E_ILLEGAL_STATE_EXCEPTION;
            case FINISHED:
                Logger::E(TAG, "Cannot execute task: the task has already been executed "
                    "(a task can be executed only once)");
                return E_ILLEGAL_STATE_EXCEPTION;
            case PENDING:
                break;
        }
    }

    mStatus = RUNNING;

    OnPreExecute();

    mWorker->mParams = params;
    exec->Execute(IRunnable::Probe(mFuture));

    mWorker->mOwner->AddRef();
    mWorker->mIsExecuted = TRUE;
    mIsExecuted = TRUE;

    return NOERROR;
}

ECode AsyncTask::Execute(
    /* [in] */ IRunnable* runnable)
{
    return sDefaultExecutor->Execute(runnable);
}

ECode AsyncTask::PublishProgress(
    /* [in] */ ArrayOf<IInterface*>* values)
{
    if (!IsCancelled()) {
        AutoPtr<AsyncTaskResult> atResult = new AsyncTaskResult(this, values);

        AutoPtr<IMessage> msg;
        GetHandler()->ObtainMessage(MESSAGE_POST_PROGRESS, TO_IINTERFACE(atResult), (IMessage**)&msg);
        return msg->SendToTarget();
    }
    return NOERROR;
}

void AsyncTask::Finish(
    /* [in] */ IInterface* result)
{
    if (IsCancelled()) {
        OnCancelled(result);
    }
    else {
        OnPostExecute(result);
    }
    mStatus = FINISHED;
}

AutoPtr<IExecutor> AsyncTask::InitThreadPoolExecutor()
{
    assert(sPoolWorkQueue != NULL);
    assert(sThreadFactory != NULL);
    AutoPtr<ITimeUnitHelper> tuHelper;
    CTimeUnitHelper::AcquireSingleton((ITimeUnitHelper**)&tuHelper);
    AutoPtr<ITimeUnit> seconds;
    tuHelper->GetSECONDS((ITimeUnit**)&seconds);
    AutoPtr<IExecutor> executor;
    CThreadPoolExecutor::New(CORE_POOL_SIZE, MAXIMUM_POOL_SIZE, KEEP_ALIVE,
        seconds, sPoolWorkQueue, sThreadFactory, (IExecutor**)&executor);
    return executor;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
