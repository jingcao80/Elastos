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

#include "CStructUcred.h"

namespace Elastos {
namespace Droid {
namespace System {
CAR_OBJECT_IMPL(CStructUcred)

CAR_INTERFACE_IMPL(CStructUcred, Object, IStructUcred)

ECode CStructUcred::constructor(
    /* [in] */ Int32 pid,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid)
{
    mPid = pid;
    mUid = uid;
    mGid = gid;
    return NOERROR;
}

ECode CStructUcred::GetPid(
    /* [out] */ Int32* pid)
{
    VALIDATE_NOT_NULL(pid)
    *pid = mPid;
    return NOERROR;
}

ECode CStructUcred::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid)
    *uid = mUid;
    return NOERROR;
}

ECode CStructUcred::GetGid(
    /* [out] */ Int32* gid)
{
    VALIDATE_NOT_NULL(gid)
    *gid = mGid;
    return NOERROR;
}

}// namespace System
}// namespace Droid
}// namespace Elastos