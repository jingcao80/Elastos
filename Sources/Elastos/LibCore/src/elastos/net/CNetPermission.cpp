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

#include "CNetPermission.h"

//using Elastos::Security::IBasicPermission;
//using Elastos::Security::IPermission;

namespace Elastos {
namespace Net {

CAR_INTERFACE_IMPL(CNetPermission, Object, INetPermission)

CAR_OBJECT_IMPL(CNetPermission)

ECode CNetPermission::constructor(
    /* [in] */ const String& name)
{
    //return BasicPermission::Init(name);
    return NOERROR;
}

ECode CNetPermission::constructor(
    /* [in] */ const String& name,
    /* [in] */ const String& action)
{
    //return BasicPermission::Init(name, action);
    return NOERROR;
}

} // namespace Net
} // namespace Elastos
