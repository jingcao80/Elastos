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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/webkit/webview/chromium/native/base/ThreadUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/base/api/ThreadUtils_dec.h"
#include "elastos/droid/os/Looper.h"
#include "elastos/droid/os/Process.h"
#include "elastos/core/AutoLock.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Core::AutoLock;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

Object ThreadUtils::sLock;
Boolean ThreadUtils::sWillOverride = FALSE;
AutoPtr<IHandler> ThreadUtils::sUiThreadHandler;

void ThreadUtils::SetWillOverrideUiThread()
{
    {
        AutoLock lock(sLock);
        sWillOverride = TRUE;
    }
}

void ThreadUtils::SetUiThread(
    /* [in] */ ILooper* looper)
{
    {
        AutoLock lock(sLock);
        AutoPtr<ILooper> tlooper;
        sUiThreadHandler->GetLooper((ILooper**)&tlooper);
        if (sUiThreadHandler != NULL && tlooper.Get() != looper) {
            //throw new RuntimeException("UI thread looper is already set to " +
            //        sUiThreadHandler.getLooper() + " (Main thread looper is " +
            //        Looper.getMainLooper() + "), cannot set to new looper " + looper);
            Logger::E("ThreadUtils::SetUiThread", "UI thread looper is already set");
            assert(0);
        } else {
            //sUiThreadHandler = new Handler(looper);
            CHandler::New(looper, (IHandler**)&sUiThreadHandler);
        }
    }
}

AutoPtr<IHandler> ThreadUtils::GetUiThreadHandler()
{
    {
        AutoLock lock(sLock);
        if (sUiThreadHandler == NULL) {
            if (sWillOverride) {
                //throw new RuntimeException("Did not yet override the UI thread");
                Logger::E("ThreadUtils::GetUiThreadHandler", "Did not yet override the UI thread");
                assert(0);
            }
            //sUiThreadHandler = new Handler(Looper.getMainLooper());
            AutoPtr<ILooper> looper = Looper::GetMainLooper();
            CHandler::New(looper, (IHandler**)&sUiThreadHandler);
        }
        return sUiThreadHandler;
    }
}

/**
 * Run the supplied Runnable on the main thread. The method will block until the Runnable
 * completes.
 *
 * @param r The Runnable to run.
 */
void ThreadUtils::RunOnUiThreadBlocking(
    /* [in] */ IRunnable* r)
{
    if (RunningOnUiThread()) {
        r->Run();
    } else {
        //FutureTask<Void> task = new FutureTask<Void>(r, null);
        AutoPtr<FutureTask> task = new FutureTask(r, NULL);
        PostOnUiThread(task);
        //try {
        AutoPtr<IInterface> obj;
        task->Get((IInterface**)&obj);
        //} catch (Exception e) {
        //    throw new RuntimeException("Exception occured while waiting for runnable", e);
        //}
    }
}

/**
 * Run the supplied Callable on the main thread, wrapping any exceptions in a RuntimeException.
 * The method will block until the Callable completes.
 *
 * @param c The Callable to run
 * @return The result of the callable
 */
AutoPtr<IInterface> ThreadUtils::RunOnUiThreadBlockingNoException(
    /* [in] */ ICallable* c)
{
    //try {
    return RunOnUiThreadBlocking(c);
    //} catch (ExecutionException e) {
    //    throw new RuntimeException("Error occured waiting for callable", e);
    //}
}

/**
 * Run the supplied Callable on the main thread, The method will block until the Callable
 * completes.
 *
 * @param c The Callable to run
 * @return The result of the callable
 * @throws ExecutionException c's exception
 */
AutoPtr<IInterface> ThreadUtils::RunOnUiThreadBlocking(
    /* [in] */ ICallable* c)
{
    AutoPtr<FutureTask> task = new FutureTask(c);
    RunOnUiThread(task);
    //try {
    AutoPtr<IInterface> obj;
    task->Get((IInterface**)&obj);
    return obj;
    //} catch (InterruptedException e) {
    //    throw new RuntimeException("Interrupted waiting for callable", e);
    //}
}

/**
 * Run the supplied FutureTask on the main thread. The method will block only if the current
 * thread is the main thread.
 *
 * @param task The FutureTask to run
 * @return The queried task (to aid inline construction)
 */
 AutoPtr<FutureTask> ThreadUtils::RunOnUiThread(
    /* [in] */ FutureTask* task)
{
    if (RunningOnUiThread()) {
        task->Run();
    } else {
        PostOnUiThread(task);
    }
    return task;
}

/**
 * Run the supplied Callable on the main thread. The method will block only if the current
 * thread is the main thread.
 *
 * @param c The Callable to run
 * @return A FutureTask wrapping the callable to retrieve results
 */
AutoPtr<FutureTask> ThreadUtils::RunOnUiThread(
    /* [in] */ ICallable* c)
{
    return RunOnUiThread(new FutureTask(c));
}

/**
 * Run the supplied Runnable on the main thread. The method will block only if the current
 * thread is the main thread.
 *
 * @param r The Runnable to run
 */
void ThreadUtils::RunOnUiThread(
    /* [in] */ IRunnable* r)
{
    if (RunningOnUiThread()) {
        r->Run();
    } else {
        Boolean res;
        GetUiThreadHandler()->Post(r, &res);
    }
}

/**
 * Post the supplied FutureTask to run on the main thread. The method will not block, even if
 * called on the UI thread.
 *
 * @param task The FutureTask to run
 * @return The queried task (to aid inline construction)
 */
AutoPtr<FutureTask> ThreadUtils::PostOnUiThread(
    /* [in] */ FutureTask* task)
{
    Boolean res;
    GetUiThreadHandler()->Post(IRunnable::Probe(task), &res);
    return task;
}

/**
 * Post the supplied Runnable to run on the main thread. The method will not block, even if
 * called on the UI thread.
 *
 * @param task The Runnable to run
 */
void ThreadUtils::PostOnUiThread(
    /* [in] */ IRunnable* task)
{
    Boolean res;
    GetUiThreadHandler()->Post(task, &res);
}

/**
 * Post the supplied Runnable to run on the main thread after the given amount of time. The
 * method will not block, even if called on the UI thread.
 *
 * @param task The Runnable to run
 * @param delayMillis The delay in milliseconds until the Runnable will be run
 */
void ThreadUtils::PostOnUiThreadDelayed(
    /* [in] */ IRunnable* task,
    /* [in] */ Int64 delayMillis)
{
    Boolean res;
    GetUiThreadHandler()->PostDelayed(task, delayMillis, &res);
}

/**
 * Asserts that the current thread is running on the main thread.
 */
void ThreadUtils::AssertOnUiThread()
{
    assert(RunningOnUiThread());
}

/**
 * @return true iff the current thread is the main (UI) thread.
 */
Boolean ThreadUtils::RunningOnUiThread()
{
    AutoPtr<ILooper> hlooper;
    GetUiThreadHandler()->GetLooper((ILooper**)&hlooper);
    return hlooper == Looper::GetMyLooper();
}

AutoPtr<ILooper> ThreadUtils::GetUiThreadLooper()
{
    AutoPtr<ILooper> hlooper;
    GetUiThreadHandler()->GetLooper((ILooper**)&hlooper);
    return hlooper;
}

/**
 * Set thread priority to audio.
 */
//@CalledByNative
void ThreadUtils::SetThreadPriorityAudio(
    /* [in] */ Int32 tid)
{
    Process::SetThreadPriority(tid, IProcess::THREAD_PRIORITY_AUDIO);
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
