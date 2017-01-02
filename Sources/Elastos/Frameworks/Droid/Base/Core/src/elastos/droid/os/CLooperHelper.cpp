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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/os/CLooperHelper.h"
#include "elastos/droid/os/Looper.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CLooperHelper, Singleton, ILooperHelper)

CAR_SINGLETON_IMPL(CLooperHelper)

ECode CLooperHelper::Prepare()
{
    return Looper::Prepare();
}

ECode CLooperHelper::PrepareMainLooper()
{
    return Looper::PrepareMainLooper();
}

ECode CLooperHelper::GetMyLooper(
    /* [out] */ ILooper** looper)
{
    VALIDATE_NOT_NULL(looper);
    AutoPtr<ILooper> l = Looper::GetMyLooper();
    *looper = l;
    REFCOUNT_ADD(*looper);
    return NOERROR;
}

ECode CLooperHelper::GetMyQueue(
    /* [out] */ IMessageQueue** queue)
{
    VALIDATE_NOT_NULL(queue);
    AutoPtr<IMessageQueue> q = Looper::GetMyQueue();
    *queue = q;
    REFCOUNT_ADD(*queue);
    return NOERROR;
}

ECode CLooperHelper::GetMainLooper(
    /* [out] */ ILooper** looper)
{
    VALIDATE_NOT_NULL(looper);
    AutoPtr<ILooper> l = Looper::GetMainLooper();
    *looper = l;
    REFCOUNT_ADD(*looper);
    return NOERROR;
}

ECode CLooperHelper::Loop()
{
    return Looper::Loop();
}

} // namespace Os
} // namespace Droid
} // namespace Elastos

