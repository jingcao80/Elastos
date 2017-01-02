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
#include "elastos/droid/internal/view/CWindowManagerPolicyThread.h"
#include "elastos/droid/internal/view/WindowManagerPolicyThread.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CAR_INTERFACE_IMPL(CWindowManagerPolicyThread, Singleton, IWindowManagerPolicyThread)

CAR_SINGLETON_IMPL(CWindowManagerPolicyThread)

ECode CWindowManagerPolicyThread::Set(
    /* [in] */ IThread* thread,
    /* [in] */ ILooper* looper)
{
    return WindowManagerPolicyThread::Set(thread, looper);
}

ECode CWindowManagerPolicyThread::GetThread(
    /* [out] */ IThread** thread)
{
    VALIDATE_NOT_NULL(thread);
    AutoPtr<IThread> temp = WindowManagerPolicyThread::GetThread();
    *thread = temp;
    REFCOUNT_ADD(*thread);
    return NOERROR;
}

ECode CWindowManagerPolicyThread::GetLooper(
    /* [out] */ ILooper** looper)
{
    VALIDATE_NOT_NULL(looper);
    AutoPtr<ILooper> temp = WindowManagerPolicyThread::GetLooper();
    *looper = temp;
    REFCOUNT_ADD(*looper);
    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namespace Droid
} // namespace Elastos
