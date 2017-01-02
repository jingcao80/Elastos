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

#include "elastos/droid/server/telecom/ThreadUtil.h"
#include "elastos/droid/server/telecom/Log.h"
#include <elastos/droid/os/Looper.h>

using Elastos::Droid::Os::Looper;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

const String ThreadUtil::TAG("ThreadUtil");

ThreadUtil::ThreadUtil()
{
}

ECode ThreadUtil::IsOnMainThread(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)

    *result = Looper::GetMainLooper() == Looper::GetMyLooper();
    return NOERROR;
}

ECode ThreadUtil::CheckOnMainThread()
{
    Boolean isOnMainThread;
    IsOnMainThread(&isOnMainThread);
    if (!isOnMainThread) {
        Log::Wtf(TAG, "Must be on the main thread!");
    }
    return NOERROR;
}

ECode ThreadUtil::CheckNotOnMainThread()
{
    Boolean isOnMainThread;
    IsOnMainThread(&isOnMainThread);
    if (isOnMainThread) {
        Log::Wtf(TAG, "Must not be on the main thread!");
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
