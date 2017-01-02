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

#include "CFilePermission.h"
// #include "AllPermissionCollection.h"

// using Elastos::Security::AllPermissionCollection;
using Elastos::Security::EIID_IGuard;

namespace Elastos {
namespace IO {

CAR_OBJECT_IMPL(CFilePermission)

CAR_INTERFACE_IMPL_3(CFilePermission, Object, IFilePermission, IPermission, IGuard)

ECode CFilePermission::constructor(
    /* [in] */ const String& path,
    /* [in] */ const String& actions)
{
    // return Permision::constructor(String(""));
    return NOERROR;
}

ECode CFilePermission::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = NULL;
    return NOERROR;
}

ECode CFilePermission::NewPermissionCollection(
    /* [out] */ IPermissionCollection** object)
{
    VALIDATE_NOT_NULL(object);
    // todo delete
    return NOERROR;
}

ECode CFilePermission::GetActions(
    /* [out] */ String* actions)
{
    VALIDATE_NOT_NULL(actions);

    *actions = String(NULL);
    return NOERROR;
}

ECode CFilePermission::Implies(
    /* [in] */ IPermission* p,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

ECode CFilePermission::CheckGuard(
    /* [in] */ IInterface* object)
{
    // todo delete
    return NOERROR;
}

} // namespace IO
} // namespace Elastos
