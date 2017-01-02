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

#ifndef __ELASTOS_SECURITY_POLICY_H__
#define __ELASTOS_SECURITY_POLICY_H__

#include "Elastos.CoreLibrary.Security.h"
#include "PermissionCollection.h"

namespace Elastos {
namespace Security {

class Policy
    : public Object
    , public IPolicy
{
public:
    CAR_INTERFACE_DECL()

    virtual ~Policy() = 0;

    Policy();

    static CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ IPolicyParameters* params,
        /* [out] */ IPolicy** result) /*throws NoSuchAlgorithmException*/;

    static CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ IPolicyParameters* params,
        /* [in] */ const String& provider,
        /* [out] */ IPolicy** result) /*throws NoSuchProviderException, NoSuchAlgorithmException*/;

    static CARAPI GetInstance(
        /* [in] */ const String& type,
        /* [in] */ IPolicyParameters* params,
        /* [in] */ IProvider* provider,
        /* [out] */ IPolicy** result) /*throws NoSuchAlgorithmException*/;

    CARAPI GetParameters(
        /* [out] */ IPolicyParameters** result);

    CARAPI GetProvider(
        /* [out] */ IProvider** result);

    CARAPI GetType(
        /* [out] */ String* result);

    CARAPI GetPermissions(
        /* [in] */ ICodeSource* cs,
        /* [out] */ IPermissionCollection** result);

    CARAPI Refresh();

    CARAPI GetPermissions(
        /* [in] */ IProtectionDomain* domain,
        /* [out] */ IPermissionCollection** result);

    CARAPI Implies(
        /* [in] */ IProtectionDomain* domain,
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);

    static CARAPI_(AutoPtr<IPolicy>) GetPolicy();

    static CARAPI SetPolicy(
        /* [in] */ IPolicy* policy);

public:
    static AutoPtr<PermissionCollection> UNSUPPORTED_EMPTY_COLLECTION;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_POLICY_H__
