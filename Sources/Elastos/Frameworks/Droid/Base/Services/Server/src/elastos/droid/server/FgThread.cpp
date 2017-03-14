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

#include "elastos/droid/server/FgThread.h"
#include "elastos/droid/server/UiThread.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::CHandler;

namespace Elastos {
namespace Droid {
namespace Server {

AutoPtr<FgThread> FgThread::sInstance;
AutoPtr<IHandler> FgThread::sHandler;

FgThread::FgThread()
    : ServiceThread(String("android.fg"), IProcess::THREAD_PRIORITY_DEFAULT, TRUE /*allowIo*/)
{
}

void FgThread::EnsureThreadLocked()
{
    if (sInstance == NULL) {
        sInstance = new FgThread();
        sInstance->Start();
        AutoPtr<ILooper> looper;
        sInstance->GetLooper((ILooper**)&looper);
        CHandler::New(looper, (IHandler**)&sHandler);
    }
}

AutoPtr<FgThread> FgThread::Get()
{
    {
        AutoLock syncLock(UiThread::sLock);
        EnsureThreadLocked();
    }
    return sInstance;
}

AutoPtr<IHandler> FgThread::GetHandler()
{
    {
        AutoLock syncLock(UiThread::sLock);
        EnsureThreadLocked();
    }
    return sHandler;
}


} // namespace Server
} // namepsace Droid
} // namespace Elastos
