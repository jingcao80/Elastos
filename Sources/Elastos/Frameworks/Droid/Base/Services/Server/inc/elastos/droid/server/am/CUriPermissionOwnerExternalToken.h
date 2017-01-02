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

#ifndef __ELASTOS_DROID_SERVER_AM_CURIPERMISSIONOWNEREXTERNALTOKEN_H__
#define __ELASTOS_DROID_SERVER_AM_CURIPERMISSIONOWNEREXTERNALTOKEN_H__

#include "_Elastos_Droid_Server_Am_CUriPermissionOwnerExternalToken.h"
#include "elastos/droid/server/am/UriPermissionOwner.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class UriPermissionOwner;

CarClass(CUriPermissionOwnerExternalToken)
    , public Object
    , public IUriPermissionOwnerExternalToken
    , public IBinder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ IObject* owner);

    CARAPI GetOwner(
        /* [out] */ IObject** owner);

    CARAPI ToString(
        /* [out] */ String* str);

public:
    AutoPtr<IObject> mUriOwner; // UriPermissionOwner
};

} // namespace Am
} // namespace Droid
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_AM_CURIPERMISSIONOWNEREXTERNALTOKEN_H__
