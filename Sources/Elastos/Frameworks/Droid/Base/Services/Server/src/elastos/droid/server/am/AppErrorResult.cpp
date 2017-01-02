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

#include "elastos/droid/server/am/AppErrorResult.h"
#include <elastos/core/AutoLock.h>

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

AppErrorResult::AppErrorResult()
    : mHasResult(FALSE)
    , mResult(0)
{}

void AppErrorResult::SetResult(
    /* [in] */ Int32 res)
{
    AutoLock lock(this);
    mHasResult = TRUE;
    mResult = res;
    NotifyAll();
}

Int32 AppErrorResult::GetResult()
{
    AutoLock lock(this);
    while (!mHasResult) {
        Wait();
    }

    return mResult;
}

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos
