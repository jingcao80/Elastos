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

#include "Elastos.Droid.Content.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/CBinder.h"
#include "elastos/droid/os/CBinderHelper.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CBinderHelper, Singleton, IBinderHelper)

CAR_SINGLETON_IMPL(CBinderHelper)

ECode CBinderHelper::GetCallingPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);
    *pid = CBinder::GetCallingPid();
    return NOERROR;
}

ECode CBinderHelper::GetCallingUid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);
    *pid = CBinder::GetCallingUid();
    return NOERROR;
}

ECode CBinderHelper::GetCallingUserHandle(
    /* [out] */ IUserHandle** h)
{
    VALIDATE_NOT_NULL(h);
    AutoPtr<IUserHandle> userHandle = CBinder::GetCallingUserHandle();
    *h = userHandle;
    REFCOUNT_ADD(*h);
    return NOERROR;
}

ECode CBinderHelper::ClearCallingIdentity(
    /* [out] */ Int64* token)
{
    VALIDATE_NOT_NULL(token);
    *token = CBinder::ClearCallingIdentity();
    return NOERROR;
}

ECode CBinderHelper::RestoreCallingIdentity(
    /* [in] */ Int64 token)
{
    return CBinder::RestoreCallingIdentity(token);
}

ECode CBinderHelper::SetThreadStrictModePolicy(
    /* [in] */ Int32 policyMask)
{
    return CBinder::SetThreadStrictModePolicy(policyMask);
}

ECode CBinderHelper::GetThreadStrictModePolicy(
    /* [out] */ Int32* policy)
{
    VALIDATE_NOT_NULL(policy);
    *policy = CBinder::GetThreadStrictModePolicy();
    return NOERROR;
}

ECode CBinderHelper::FlushPendingCommands()
{
    return CBinder::FlushPendingCommands();
}

ECode CBinderHelper::JoinThreadPool()
{
    return CBinder::JoinThreadPool();
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
