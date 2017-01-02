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

#include "elastos/droid/dialer/util/AsyncTaskExecutors.h"
#include "elastos/droid/os/AsyncTask.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include <elastos/core/AutoLock.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::Dialer::Util::EIID_IAsyncTaskExecutor;
using Elastos::Droid::Internal::Utility::IPreconditions;
using Elastos::Droid::Internal::Utility::CPreconditions;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::ILooperHelper;
using Elastos::Droid::Os::CLooperHelper;
using Elastos::Core::IThread;
using Elastos::Core::Thread;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Util {

//=================================================================
// AsyncTaskExecutors::SimpleAsyncTaskExecutor
//=================================================================
CAR_INTERFACE_IMPL(AsyncTaskExecutors::SimpleAsyncTaskExecutor, Object, IAsyncTaskExecutor);

AsyncTaskExecutors::SimpleAsyncTaskExecutor::SimpleAsyncTaskExecutor(
    /* [in] */ IExecutor* executor)
    : mExecutor(executor)
{}

ECode AsyncTaskExecutors::SimpleAsyncTaskExecutor::Submit(
    /* [in] */ IInterface* identifier,
    /* [in] */ IInterface* task,
    /* [in] */ ArrayOf<IInterface*>* params)
{
    AsyncTaskExecutors::CheckCalledFromUiThread();
    return ((AsyncTask*)(IObject*)task)->ExecuteOnExecutor(mExecutor, params);
}


//=================================================================
// AsyncTaskExecutors
//=================================================================
AutoPtr<IAsyncTaskExecutorFactory> AsyncTaskExecutors::sInjectedAsyncTaskExecutorFactory;
Object AsyncTaskExecutors::sSyncObject;

AutoPtr<IAsyncTaskExecutor> AsyncTaskExecutors::CreateAsyncTaskExecutor()
{
    AutoLock syncLock(sSyncObject);
    if (sInjectedAsyncTaskExecutorFactory != NULL) {
        AutoPtr<IAsyncTaskExecutor> executor;
        sInjectedAsyncTaskExecutorFactory->CreateAsyncTaskExecutor(
                (IAsyncTaskExecutor**)&executor);
        return executor;
    }
    AutoPtr<SimpleAsyncTaskExecutor> simpleExecutor = new SimpleAsyncTaskExecutor(
            AsyncTask::SERIAL_EXECUTOR);
    return (IAsyncTaskExecutor*)simpleExecutor;
}

AutoPtr<IAsyncTaskExecutor> AsyncTaskExecutors::CreateThreadPoolExecutor()
{
    AutoLock syncLock(sSyncObject);
    if (sInjectedAsyncTaskExecutorFactory != NULL) {
        AutoPtr<IAsyncTaskExecutor> executor;
        sInjectedAsyncTaskExecutorFactory->CreateAsyncTaskExecutor(
                (IAsyncTaskExecutor**)&executor);
        return executor;
    }
    AutoPtr<SimpleAsyncTaskExecutor> simpleExecutor = new SimpleAsyncTaskExecutor(
            AsyncTask::THREAD_POOL_EXECUTOR);
    return (IAsyncTaskExecutor*)simpleExecutor;
}

void AsyncTaskExecutors::SetFactoryForTest(
    /* [in] */ IAsyncTaskExecutorFactory* factory)
{
    AutoLock syncLock(sSyncObject);
    sInjectedAsyncTaskExecutorFactory = factory;
}

void AsyncTaskExecutors::CheckCalledFromUiThread()
{
    AutoPtr<ILooperHelper> helper;
    CLooperHelper::AcquireSingleton((ILooperHelper**)&helper);
    AutoPtr<ILooper> looper;
    helper->GetMainLooper((ILooper**)&looper);
    AutoPtr<IThread> thread;
    looper->GetThread((IThread**)&thread);
    AutoPtr<IThread> currentThread = Thread::GetCurrentThread();
    // String str;
    // IObject::Probe(currentThread)->ToString(&str);

    AutoPtr<IPreconditions> preconditions;
    CPreconditions::AcquireSingleton((IPreconditions**)&preconditions);
    preconditions->CheckState(currentThread == thread/*, "submit method must be called from ui thread, was: " + str*/);
}

} // Util
} // Dialer
} // Droid
} // Elastos
