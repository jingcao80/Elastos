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

#include "Policy.h"
#include "AllPermissionCollection.h"

namespace Elastos {
namespace Security {

CAR_INTERFACE_IMPL(Policy, Object, IPolicy)
AutoPtr<PermissionCollection> Policy::UNSUPPORTED_EMPTY_COLLECTION = new AllPermissionCollection();
Policy::~Policy()
{}

Policy::Policy()
{}

ECode Policy::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ IPolicyParameters* params,
    /* [out] */ IPolicy** result) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ IPolicyParameters* params,
    /* [in] */ const String& provider,
    /* [out] */ IPolicy** result) /*throws NoSuchProviderException, NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::GetInstance(
    /* [in] */ const String& type,
    /* [in] */ IPolicyParameters* params,
    /* [in] */ IProvider* provider,
    /* [out] */ IPolicy** result) /*throws NoSuchAlgorithmException*/
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::GetParameters(
    /* [out] */ IPolicyParameters** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::GetProvider(
    /* [out] */ IProvider** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::GetType(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = String(NULL);
    return NOERROR;
}

ECode Policy::GetPermissions(
    /* [in] */ ICodeSource* cs,
    /* [out] */ IPermissionCollection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::Refresh()
{
    return NOERROR;
}

ECode Policy::GetPermissions(
    /* [in] */ IProtectionDomain* domain,
    /* [out] */ IPermissionCollection** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode Policy::Implies(
    /* [in] */ IProtectionDomain* domain,
    /* [in] */ IPermission* permission,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = TRUE;
    return NOERROR;
}

AutoPtr<IPolicy> Policy::GetPolicy()
{
    return NULL;
}

ECode Policy::SetPolicy(
    /* [in] */ IPolicy* policy)
{
    return NOERROR;
}

} // namespace Security
} // namespace Elastos
