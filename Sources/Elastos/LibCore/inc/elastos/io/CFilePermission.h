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

#ifndef __ELASTOS_IO_CFILEPERMISSION_H__
#define __ELASTOS_IO_CFILEPERMISSION_H__

#include "Object.h"
#include "_Elastos_IO_CFilePermission.h"
// #include "Permission.h"

using Elastos::Core::Object;
using Elastos::Security::IGuard;
// using Elastos::Security::Permission;

namespace Elastos {
namespace IO {

CarClass(CFilePermission)
    , public Object
    // , public Permission
    , public IPermission //TODO
    , public IGuard
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ const String& path,
        /* [in] */ const String& actions);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI NewPermissionCollection(
        /* [out] */ IPermissionCollection** object);

    CARAPI GetActions(
        /* [out] */ String* actions);

    CARAPI Implies(
        /* [in] */ IPermission* p,
        /* [out] */ Boolean* result);

    CARAPI CheckGuard(
        /* [in] */ IInterface* object);
};

} // namespace IO
} // namespace Elastos

#endif //__ELASTOS_IO_CFILEPERMISSION_H__
