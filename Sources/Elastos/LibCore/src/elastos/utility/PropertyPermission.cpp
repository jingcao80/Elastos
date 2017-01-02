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

#include "PropertyPermission.h"

namespace Elastos {
namespace Utility {

CAR_INTERFACE_IMPL(PropertyPermission, BasicPermission, IPropertyPermission)

PropertyPermission::PropertyPermission()
{}

ECode PropertyPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& actions)
{
    return BasicPermission::constructor(String(""));
}

ECode PropertyPermission::GetName(
    /* [out] */ String* name)
{
    return BasicPermission::GetName(name);
}

ECode PropertyPermission::NewPermissionCollection(
    /* [out] */ IPermissionCollection** permissions)
{
    return BasicPermission::NewPermissionCollection(permissions);
}

ECode PropertyPermission::GetActions(
    /* [out] */ String* actions)
{
    VALIDATE_NOT_NULL(actions)
    *actions = NULL;
    return NOERROR;
}

ECode PropertyPermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

} // namespace Utility
} // namespace Elastos