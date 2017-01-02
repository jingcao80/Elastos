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

#include "CPermissions.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CPermissions)
ECode CPermissions::Add(
    /* [in] */ IPermission* permission)
{
    return NOERROR;
}

ECode CPermissions::GetElements(
    /* [out] */ IEnumeration** permissions)
{
    VALIDATE_NOT_NULL(*permissions);
    *permissions = NULL;
    return NOERROR;
}

ECode CPermissions::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

}
}
