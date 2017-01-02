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

#ifndef __ELASTOS_NET_CSOCKETPERMISSION_H__
#define __ELASTOS_NET_CSOCKETPERMISSION_H__

#include "_Elastos_Net_CSocketPermission.h"
//#include "security/Permission.h"
#include "Object.h"

using Elastos::Core::Object;
using Elastos::IO::ISerializable;
using Elastos::Security::IPermission;
using Elastos::Security::IPermissionCollection;

namespace Elastos {
namespace Net {

CarClass(CSocketPermission)
    //, public Permission  // BasicPermission has not compile ,use Object temp
    , public Object
    , public ISocketPermission
    , public ISerializable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSocketPermission();

    CARAPI constructor(
        /* [in] */ const String& host,
        /* [in] */ const String& action);

    CARAPI GetActions(
        /* [out] */ String* actions);

    CARAPI Implies(
        /* [in] */ IPermission* permission,
        /* [out] */ Boolean* isImplied);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CSOCKETPERMISSION_H__
