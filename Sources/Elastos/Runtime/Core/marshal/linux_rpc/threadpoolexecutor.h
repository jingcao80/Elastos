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

#ifndef __THREADPOOLEXECUTOR_H__
#define __THREADPOOLEXECUTOR_H__

#include <eltypes.h>
#include <utils/Vector.h>
#include <utils/Mutex.h>
#include <pthread.h>

namespace Elastos {
namespace RPC {

class ThreadPoolExecutor : public ElLightRefBase
{
public:
    class Runnable : public ElLightRefBase
    {
    public:
        virtual ~Runnable() {}

        virtual CARAPI Run() = 0;
    };

private:
    class Worker : public ElLightRefBase
    {
    public:
        Worker(
            /* [in] */ Runnable* task,
            /* [in] */ ThreadPoolExecutor* owner)
            : mThread(0)
            , mTask(task)
            , mOwner(owner)
        {}

        virtual ~Worker() {}

        CARAPI Run()
        {
            return mTask->Run();
        }

    public:
        pthread_t mThread;
        AutoPtr<Runnable> mTask;
        ThreadPoolExecutor* mOwner;
        android::Mutex mLock;
    };

public:
    static CARAPI_(AutoPtr<ThreadPoolExecutor>) Self();

    static CARAPI_(void) Prepare();

    CARAPI RunTask(
        /* [in] */ Runnable* task);

    CARAPI StopTask(
        /* [in] */ Runnable* task);

private:
    static void* ThreadEntry(void* arg);

private:
    static AutoPtr<ThreadPoolExecutor> sInstance;
    static android::Mutex sInstanceLock;

    android::Mutex mMainLock;
    android::Vector< AutoPtr<Worker> > mWorkers;
};

} // namespace RPC
} // namespace Elastos

#endif //__THREADPOOLEXECUTOR_H__
