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

#include "BasicPermission.h"

namespace Elastos {
namespace Security {

BasicPermission::BasicPermission()
{
}

ECode BasicPermission::GetActions(
    /* [out] */ String *actions)
{
    VALIDATE_NOT_NULL(actions)
    *actions = String();
    return NOERROR;
}

ECode BasicPermission::Implies(
    /* [in] */ IPermission *permission,
    /* [out] */ Boolean *ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = TRUE;
    return NOERROR;
}

ECode BasicPermission::constructor(
    /* [in] */ const String& name)
{
    return Permission::constructor(String(""));
}

ECode BasicPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& action)
{
    return Permission::constructor(String(""));
}

} // namespace Security
} // namespace Elastos
