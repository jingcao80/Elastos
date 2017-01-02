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

#include "ProtectionDomain.h"

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(ProtectionDomain, Object, IProtectionDomain)
ProtectionDomain::ProtectionDomain(
    /* [in] */ ICodeSource* cs,
    /* [in] */ IPermissionCollection* permissions)
{}

ProtectionDomain::ProtectionDomain(
    /* [in] */ ICodeSource* cs,
    /* [in] */ IPermissionCollection* permissions,
    /* [in] */ IClassLoader* cl,
    /* [in] */ ArrayOf<IPrincipal*>* principals)
{}

ECode ProtectionDomain::GetClassLoader(
    /* [out] */ IClassLoader** loader)
{
    VALIDATE_NOT_NULL(loader);
    *loader = NULL;
    return NOERROR;
}

ECode ProtectionDomain::GetCodeSource(
    /* [out] */ ICodeSource** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ProtectionDomain::GetPermissions(
    /* [out] */ IPermissionCollection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ProtectionDomain::GetPrincipals(
    /* [out] */ ArrayOf<IPrincipal*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode ProtectionDomain::Implies(
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

} // namespace Security
} // namespace Elastos
