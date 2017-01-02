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

#include "elastos/droid/internal/os/BackgroundThread.h"
#include "elastos/droid/os/CHandler.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::ILooper;
using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

AutoPtr<BackgroundThread> BackgroundThread::sInstance;
AutoPtr<IHandler> BackgroundThread::sHandler;
Object BackgroundThread::sLock;

CAR_INTERFACE_IMPL(BackgroundThread, HandlerThread, IBackgroundThread)

BackgroundThread::BackgroundThread()
{
     HandlerThread::constructor(String("android.bg"), IProcess::THREAD_PRIORITY_BACKGROUND);
}

void BackgroundThread::EnsureThreadLocked()
{
    if (sInstance == NULL) {
        sInstance = new BackgroundThread();
        sInstance->Start();
        AutoPtr<ILooper> looper;
        sInstance->GetLooper((ILooper**)&looper);
        CHandler::New(looper, (IHandler**)&sHandler);
    }
}

AutoPtr<IBackgroundThread> BackgroundThread::GetInstance()
{
    AutoLock lock(sLock);
    EnsureThreadLocked();
    return sInstance;
}

AutoPtr<IHandler> BackgroundThread::GetHandler()
{
    AutoLock lock(sLock);
    EnsureThreadLocked();
    return sHandler;
}

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos
