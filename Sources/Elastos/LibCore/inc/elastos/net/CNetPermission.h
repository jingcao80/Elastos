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

#ifndef __ELASTOS_NET_CNETPERMISSION_H__
#define __ELASTOS_NET_CNETPERMISSION_H__

#include "_Elastos_Net_CNetPermission.h"
//#include "security/BasicPermission.h"
#include "Object.h"

using Elastos::Core::Object;
//using Elastos::Security::BasicPermission;

namespace Elastos {
namespace Net {

CarClass(CNetPermission)
    //, public BasicPermission  // BasicPermission has not be compiled ,use Object temp
    , public Object
    , public INetPermission
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ const String& name);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ const String& actions);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CNETPERMISSION_H__
