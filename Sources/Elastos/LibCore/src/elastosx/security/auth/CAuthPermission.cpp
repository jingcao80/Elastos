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

#include "CAuthPermission.h"
#include "AllPermissionCollection.h"

using Elastos::Security::AllPermissionCollection;

namespace Elastosx {
namespace Security {
namespace Auth {

CAR_OBJECT_IMPL(CAuthPermission)
CAR_INTERFACE_IMPL(CAuthPermission, BasicPermission, IAuthPermission)

ECode CAuthPermission::GetActions(
    /* [out] */ String *actions)
{
    VALIDATE_NOT_NULL(actions)
    *actions = String(NULL);

    return NOERROR;
}

ECode CAuthPermission::Implies(
    /* [in] */ IPermission *permission,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    VALIDATE_NOT_NULL(permission)

    return NOERROR;
}

ECode CAuthPermission::constructor(
    /* [in] */ const String& name)
{
    return BasicPermission::constructor(String(""));
}

ECode CAuthPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    return BasicPermission::constructor(String(""), String(""));
}

}
}
}
