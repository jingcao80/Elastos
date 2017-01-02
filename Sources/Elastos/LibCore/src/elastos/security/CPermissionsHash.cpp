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

#include "CPermissionsHash.h"
#include "CHashTable.h"

using Elastos::Utility::CHashTable;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CPermissionsHash)
CPermissionsHash::CPermissionsHash()
{
    CHashTable::New((IHashTable**)&mPerms);
}

ECode CPermissionsHash::Add(
    /* [in] */ IPermission* permission)
{
    mPerms->Put(permission, permission);
    return NOERROR;
}

ECode CPermissionsHash::GetElements(
    /* [out] */ IEnumeration** permissions)
{
    VALIDATE_NOT_NULL(*permissions);
    return mPerms->GetElements(permissions);
}

ECode CPermissionsHash::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IEnumeration> elements;
    GetElements((IEnumeration**)&elements);
    Boolean has = FALSE;
    while (elements->HasMoreElements(&has), has) {
        AutoPtr<IInterface> obj;
        elements->GetNextElement((IInterface**)&obj);
        Boolean tmp = FALSE;
        IPermission::Probe(obj)->Implies(permission, &tmp);
        if (tmp) {
            *result = TRUE;
            return NOERROR;
        }

        elements = NULL;
        GetElements((IEnumeration**)&elements);
    }

    *result = FALSE;
    return NOERROR;
}

}
}
