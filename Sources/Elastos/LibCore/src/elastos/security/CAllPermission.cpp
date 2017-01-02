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

#include "CAllPermission.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CAllPermission)
ECode CAllPermission::constructor()
{
    return Permission::constructor(String(""));
}

ECode CAllPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    return Permission::constructor(String(""));
}

ECode CAllPermission::GetActions(
    /* [out] */ String* actions)
{
    VALIDATE_NOT_NULL(actions);
    *actions = String(NULL);
    return NOERROR;
}

ECode CAllPermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

}
}
