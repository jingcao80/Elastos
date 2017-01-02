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

#ifndef __ELASTOS_SECURITY_PERMISSION_H__
#define __ELASTOS_SECURITY_PERMISSION_H__

#include "Elastos.CoreLibrary.Security.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ISerializable;

namespace Elastos {
namespace Security {

class Permission
    : public Object
    , public IPermission
    , public IGuard
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    Permission();

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI GetName(
        /* [out] */ String *name);

    CARAPI CheckGuard(
        /* [in] */ IInterface *obj);

    CARAPI NewPermissionCollection(
        /* [out] */ IPermissionCollection **coll);

    virtual CARAPI GetActions(
        /* [out] */ String *actions) = 0;

    virtual CARAPI Implies(
        /* [in] */ IPermission *permission,
        /* [out] */ Boolean *ret) = 0;
};

} // namespace Security
} // namespace Elastos

#endif // __ELASTOS_SECURITY_PERMISSION_H__
