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
#include "elastos/droid/os/CProcessStartResult.h"

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(CProcessStartResult, Object, IProcessStartResult)

CAR_OBJECT_IMPL(CProcessStartResult)

CProcessStartResult::CProcessStartResult()
    : mPid(0)
    , mUsingWrapper(FALSE)
{}

ECode CProcessStartResult::GetPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid);
    *pid = mPid;
    return NOERROR;
}

ECode CProcessStartResult::SetPid(
    /* [in] */ Int32 pid)
{
    mPid = pid;
    return NOERROR;
}

ECode CProcessStartResult::GetUsingWrapper(
    /* [out] */ Boolean* usingWrapper)
{
    VALIDATE_NOT_NULL(usingWrapper);
    *usingWrapper = mUsingWrapper;
    return NOERROR;
}

ECode CProcessStartResult::SetUsingWrapper(
    /* [in] */ Boolean usingWrapper)
{
    mUsingWrapper = usingWrapper;
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
