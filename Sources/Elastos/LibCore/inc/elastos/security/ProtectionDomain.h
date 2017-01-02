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

#ifndef __ELASTOS_SECURITY_PROTECTIONDOMAIN_H__
#define __ELASTOS_SECURITY_PROTECTIONDOMAIN_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::Core::IClassLoader;

namespace Elastos {
namespace Security {

/**
 * Legacy security code; do not use.
 */
class ProtectionDomain
    : public Object
    , public IProtectionDomain
{
public:
    CAR_INTERFACE_DECL()

    ProtectionDomain(
        /* [in] */ ICodeSource* cs,
        /* [in] */ IPermissionCollection* permissions);

    ProtectionDomain(
        /* [in] */ ICodeSource* cs,
        /* [in] */ IPermissionCollection* permissions,
        /* [in] */ IClassLoader* cl,
        /* [in] */ ArrayOf<IPrincipal*>* principals);

    CARAPI GetClassLoader(
        /* [out] */ IClassLoader** loader);

    CARAPI GetCodeSource(
        /* [out] */ ICodeSource** result);

    CARAPI GetPermissions(
        /* [out] */ IPermissionCollection** result);

    CARAPI GetPrincipals(
        /* [out] */ ArrayOf<IPrincipal*>** result);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* result);
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_PROTECTIONDOMAIN_H__
