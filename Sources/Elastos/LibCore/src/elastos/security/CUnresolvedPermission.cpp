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

#include "CUnresolvedPermission.h"

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CUnresolvedPermission)
CAR_INTERFACE_IMPL(CUnresolvedPermission, Permission, IUnresolvedPermission)
ECode CUnresolvedPermission::constructor(
    /* [in] */ const String& type,
    /* [in] */ const String& name,
    /* [in] */ const String& actions,
    /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*>* certs)
{
    return Permission::constructor(String(""));
}

ECode CUnresolvedPermission::GetUnresolvedName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CUnresolvedPermission::GetUnresolvedActions(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CUnresolvedPermission::GetUnresolvedType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CUnresolvedPermission::GetUnresolvedCerts(
    /* [out] */ ArrayOf<Elastos::Security::Cert::ICertificate*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

// @Override
ECode CUnresolvedPermission::GetActions(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

// @Override
ECode CUnresolvedPermission::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

}
}
