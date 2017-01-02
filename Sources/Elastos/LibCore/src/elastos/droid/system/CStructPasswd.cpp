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

#include "CStructPasswd.h"

namespace Elastos {
namespace Droid {
namespace System {
CAR_OBJECT_IMPL(CStructPasswd)

CAR_INTERFACE_IMPL(CStructPasswd, Object, IStructPasswd)

ECode CStructPasswd::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    *name = mPw_name;
    return NOERROR;
}

ECode CStructPasswd::GetUid(
    /* [out] */ Int32* uid)
{
    VALIDATE_NOT_NULL(uid);

    *uid = mPw_uid;
    return NOERROR;
}

ECode CStructPasswd::GetGid(
    /* [out] */ Int32* gid)
{
    VALIDATE_NOT_NULL(gid);

    *gid = mPw_gid;
    return NOERROR;
}

ECode CStructPasswd::GetDir(
    /* [out] */ String* dir)
{
    VALIDATE_NOT_NULL(dir);

    *dir = mPw_dir;
    return NOERROR;
}

ECode CStructPasswd::GetShell(
    /* [out] */ String* shell)
{
    VALIDATE_NOT_NULL(shell);

    *shell = mPw_shell;
    return NOERROR;
}

ECode CStructPasswd::constructor(
    /* [in] */ const String& name,
    /* [in] */ Int32 uid,
    /* [in] */ Int32 gid,
    /* [in] */ const String& dir,
    /* [in] */ const String& shell)
{
    mPw_name = name;
    mPw_uid = uid;
    mPw_gid = gid;
    mPw_dir = dir;
    mPw_shell = shell;
    return NOERROR;
}

} // namespace System
} // namespace Droid
} // namespace Elastos
