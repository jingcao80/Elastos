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

#ifndef __ELASTOSX_SECURITY_AUTH_CPRIVATECREDENTIALPERMISSION_H__
#define __ELASTOSX_SECURITY_AUTH_CPRIVATECREDENTIALPERMISSION_H__

#include "_Elastosx_Security_Auth_CPrivateCredentialPermission.h"
#include "Permission.h"

using Elastos::Security::IPermissionCollection;
using Elastos::Security::IPermission;
using Elastos::Security::Permission;
using Elastos::Core::IArrayOf;

namespace Elastosx {
namespace Security {
namespace Auth {

CarClass(CPrivateCredentialPermission)
    , public Permission
    , public IPrivateCredentialPermission
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetPrincipals(
        /* [out, callee] */ ArrayOf<IArrayOf*> **principals);

    CARAPI GetCredentialClass(
        /* [out] */ String *credential);

    CARAPI GetActions(
        /* [out] */ String *actions);

    CARAPI Implies(
        /* [in] */ IPermission *permission,
        /* [out] */ Boolean *result);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& action);
};

} // namespace Auth
} // namespace Security
} // namespace Elastosx

#endif // __ELASTOSX_SECURITY_AUTH_CPRIVATECREDENTIALPERMISSION_H__
