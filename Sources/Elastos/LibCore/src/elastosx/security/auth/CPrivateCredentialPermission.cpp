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

#include "CPrivateCredentialPermission.h"

namespace Elastosx {
namespace Security {
namespace Auth {

CAR_OBJECT_IMPL(CPrivateCredentialPermission)
CAR_INTERFACE_IMPL(CPrivateCredentialPermission, Permission, IPrivateCredentialPermission)
ECode CPrivateCredentialPermission::GetActions(
    /* [out] */ String *actions)
{
    VALIDATE_NOT_NULL(actions)
    *actions = String(NULL);
    return NOERROR;
}

ECode CPrivateCredentialPermission::Implies(
    /* [in] */ IPermission *permission,
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    VALIDATE_NOT_NULL(permission)

    return NOERROR;
}

ECode CPrivateCredentialPermission::GetPrincipals(
    /* [out, callee] */ ArrayOf<IArrayOf*> **principals)
{
    VALIDATE_NOT_NULL(principals)
    *principals = NULL;

    return NOERROR;
}

ECode CPrivateCredentialPermission::GetCredentialClass(
    /* [out] */ String *credential)
{
    VALIDATE_NOT_NULL(credential)
    *credential = String(NULL);
    return NOERROR;
}

ECode CPrivateCredentialPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& action)
{
    return Permission::constructor(String(""));
}

} // namespace Auth
} // namespace Security
} // namespace Elastosx
